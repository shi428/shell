#include "parser.h"
#include "tokenizer.h"
#include "exec.h"

void printPrompt() {
    cout << "shell>";
}

int main(int argc, char *argv[]) {
    string line;
    struct passwd *p = getpwuid(getuid());
    while (1) {
        printPrompt();
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
