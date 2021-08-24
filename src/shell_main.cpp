#include <parser.h>
#include <tokenizer.h>
#include <exec.h>
#include <built-in.h>
#include <expansion.h>
#include <sys/stat.h>
#include <trie.h>
vector <pair<pid_t, vector <string>>> bPids;
vector <int> pos;
pid_t shell_pid;
pid_t background_process;
int return_code;
string last_arg;
unordered_map<string,string> users;

Trie *buildTrie(const char *currentdir);
void printPrompt() {
    if (isEnviron((char *)"PROMPT")) {
        char *prompt = getenv("PROMPT");
        cout << expandPrompt(prompt);
    }
    else {
        cout << "shell>";
    }
    fflush(stdout);
}

void sigint_handler(int signum) {
    //signal(SIGINT, sigint_handler);
    cout << endl;
    if (isatty(0)) {
        printPrompt();
    }
    fflush(stdout);
}

void sigchild_handler(int signum) {
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        for (unsigned int i = 0; i < bPids.size(); i++) {
            if (bPids[i].first == pid && pos.size()) {
                background_process = pid;
                cout << endl;
                cout << pos[i] <<". [" << pid << "] ";
                for (auto j: bPids[i].second) {
                    cout << j << " ";
                }
                cout << "has exited";
                cout << endl;
                bPids.erase(bPids.begin() + i);
                pos.erase(pos.begin() + i);
                i--;
                if (isatty(0)) {
                    printPrompt();
                }
                fflush(stdout);
            }
        }
    }
}

void getUsers(struct passwd *p) {
/*    string cmd = "cat /etc/passwd | cut -d: -f1,6 > file";
    string line;
    vector <Token> tokens = genTokens(cmd, true);
    Tree *parseTree = newTree(tokens);
    //tokens.~vector <Token>();
    if (parseTree->root) {
        //        parseTree->root->traverse(0);
        if (exec(p, parseTree->root, bPids, pos)) {
            delete parseTree;
        }
        //last_arg = (bPids.back().second).back();
    }
    delete parseTree;
    ifstream fin("file");
    while (getline(fin, line)){
        string user = line.substr(0, line.find(':'));
        string homedir = line.substr(line.find(':') + 1);
        users[user] = homedir;
    }
    fin.close();
    cmd = "rm file";
    tokens = genTokens(cmd, true);
    parseTree = newTree(tokens);
    //tokens.~vector <Token>();
    if (parseTree->root) {
        //        parseTree->root->traverse(0);
        if (exec(p, parseTree->root, bPids, pos)) {
            delete parseTree;
        }
        //last_arg = (bPids.back().second).back();
    }
    delete parseTree;*/
    while (true) {
        errno = 0; // so we can distinguish errors from no more entries
        struct passwd* entry = getpwent();
        if (!entry) {
            if (errno) {
                std::cerr << "Error reading password database\n";
                return ;
            }
            break;
        }
        //std::cout << entry->pw_name << '\n';
        users[string(entry->pw_name)] = string(entry->pw_dir);
    }
    endpwent();
}

vector <string> history;
extern string read_line(bool *tab);
unsigned int ind;
//Trie *trie;
int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchild_handler);
    shell_pid = getpid();
    string line;
    struct passwd *p = getpwuid(getuid());
    const char *source[3] = {"source", ".shellrc", NULL};
    char *shell_path = realpath(argv[0], NULL);
    const char *set_shell[4] = {"setenv", "SHELL", shell_path, NULL};
    runBuiltInCommand((char **)set_shell, p);
    free(shell_path);
    getUsers(p);
    if (isatty(0)) {
        runBuiltInCommand((char **)source, p);
    }
    bool tab = false;
    while (1) {
        //trie = buildTrie(getenv("PWD"));
        if (isatty(0)) {
            printPrompt();
        }
        line = read_line(&tab);
        if (!line.compare("")) break;
        if (line.compare("\n")) {
            history.push_back(line);
            ind = history.size();
        }
        line.pop_back();
        //if (!getline(cin, line)) break;
        vector <Token> tokens = genTokens(line, true);
        if (isatty(0)) {//for (auto i: tokens) i.printToken();
        }
        tokens = expand_subshell(tokens);
        tokens = expand_env(tokens);
        tokens = expand_tilde(tokens);
        //fix tokens
        for (unsigned int i = 0; i < tokens.size(); i++) {
            if ((i < tokens.size() - 1) && ((tokens[i].type == QUOTES && (tokens[i+1].type == COMMAND)) || (tokens[i].type == QUOTES && tokens[i+1].type == QUOTES) || (tokens[i].type == COMMAND && tokens[i+1].type == COMMAND))) {
                tokens[i].lexeme += tokens[i+1].lexeme;
                tokens.erase(tokens.begin() + i + 1);
                i--;
            }
            /*if (i < tokens.size() - 1 && !(!i || (i && tokens[i-1].type == PIPE)) && tokens[i].type == COMMAND && tokens[i+1].type == QUOTES) {
              tokens[i].lexeme += tokens[i+1].lexeme;
              tokens[i].type = QUOTES;
              tokens.erase(tokens.begin() + i + 1);
              }*/
        }
        if (isatty(0)) {
        //     for (auto i: tokens) i.printToken();
        }
        Tree *parseTree = newTree(tokens);
        //tokens.~vector <Token>();
        if (parseTree->root) {
            //        parseTree->root->traverse(0);
            if (exec(p, parseTree->root, bPids, pos)) {
                delete parseTree;
                break;
            }
            //last_arg = (bPids.back().second).back();
        }
        delete parseTree;
    }
    deleteAliasedCommands();
    return EXIT_SUCCESS;
}
