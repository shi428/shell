#include <parser.h>
#include <tokenizer.h>
#include <exec.h>
#include <built-in.h>
#include <expansion.h>
vector <pair<pid_t, vector <string>>> bPids;
vector <int> pos;
void printPrompt() {
    cout << "shell>";
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
