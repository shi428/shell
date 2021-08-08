#include <parser.h>
#include <tokenizer.h>
#include <exec.h>
#include <built-in.h>
#include <expansion.h>
#include <sys/stat.h>
vector <pair<pid_t, vector <string>>> bPids;
vector <int> pos;
void printPrompt() {
    if (isEnviron((char *)"PROMPT")) {
        char *prompt = getenv("PROMPT");
        cout << expandPrompt(prompt);
    }
    else {
    cout << "shell>";
    }
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
                cout << endl;
                cout << pos[i] <<". [" << pid << "] ";
                for (auto i: bPids[i].second) {
                    cout << i << " ";
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

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchild_handler);
    string line;
    struct passwd *p = getpwuid(getuid());
    const char *source[3] = {"source", ".shellrc", NULL};
    if (isatty(0)) {
        runBuiltInCommand((char **)source, p);
    }
    while (1) {
        if (isatty(0)) {
            printPrompt();
        }
        if (!getline(cin, line)) break;
        vector <Token> tokens = genTokens(line);
        if (isatty(0)) {//for (auto i: tokens) i.printToken();
        }
        tokens = expand_subshell(tokens);
        //fix tokens
        for (unsigned int i = 0; i < tokens.size(); i++) {
            if ((i < tokens.size() - 1) && ((tokens[i].type == QUOTES && (tokens[i+1].type == COMMAND)) || (tokens[i].type == QUOTES && tokens[i+1].type == QUOTES))) {
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
           // for (auto i: tokens) i.printToken();
        }
        Tree *parseTree = newTree(tokens);
        //tokens.~vector <Token>();
        if (parseTree->root) {
            //        parseTree->root->traverse(0);
            if (exec(p, parseTree->root, bPids, pos)) {
                delete parseTree;
                break;
            }
        }
        delete parseTree;
    }
    return EXIT_SUCCESS;
}
