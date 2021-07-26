#include <parser.h>
#include <tokenizer.h>
#include <exec.h>

vector <pair<pid_t, vector <string>>> bPids;
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
            if (bPids[i].first == pid) {
                cout << endl;
                cout << "[" << pid << "] ";
                for (auto i: bPids[i].second) {
                    cout << i << " ";
                }
                cout << "has exited";
                cout << endl;
                bPids.erase(bPids.begin() + i);
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
    while (1) {
        if (isatty(0)) {
            printPrompt();
        }
        if (!getline(cin, line)) break;
        vector <Token> tokens = genTokens(line);
        Tree *parseTree = newTree(tokens);
        //tokens.~vector <Token>();
        if (parseTree->root) {
            //        parseTree->root->traverse(0);
            if (exec(p, parseTree->root, bPids)) {
                delete parseTree;
                break;
            }
        }
        delete parseTree;
    }
    return EXIT_SUCCESS;
}
