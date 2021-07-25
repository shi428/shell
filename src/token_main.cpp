#include <bits/stdc++.h>
#include <tokenizer.h>
#include <misc.h>

using namespace std;

void printPrompt() {
    cout << "tokenizer>";
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
    }
    return EXIT_SUCCESS;
}
