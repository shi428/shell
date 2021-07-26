#include <parser.h>
#include <tokenizer.h>
#include <exec.h>

void printPrompt() {
    cout << "shell>";
}

void sigint_handler(int signum) {
    signal(SIGINT, sigint_handler);
    fflush(stdout);
    cout << endl;
    printPrompt();
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);
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
            if (exec(p, parseTree->root)) {
                delete parseTree;
                break;
            }
        }
        delete parseTree;
    }
    return EXIT_SUCCESS;
}
