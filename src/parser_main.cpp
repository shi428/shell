#include <parser.h>
#include <tokenizer.h>

void printPrompt() {
    cout << "parser>";
}

void printTokens(vector <Token> &tokens) {
    for (auto i: tokens) {
        i.printToken();
    }
}
int main(int argc, char *argv[]) {
    string line;
    while (1) {
        printPrompt();
        if (!getline(cin, line)) break;
        vector <Token> tokens = genTokens(line);
        printTokens(tokens);
        Tree *parseTree = newTree(tokens);
        if (parseTree->root) {
            parseTree->root->traverse(0);
        }
        delete parseTree;
    }
    return EXIT_SUCCESS;
}
