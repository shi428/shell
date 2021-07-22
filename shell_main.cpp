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
        if (parseTree->root) {
    //        parseTree->root->traverse(0);
            exec(p, parseTree->root);
        }
        delete parseTree;
    }
return EXIT_SUCCESS;
}
