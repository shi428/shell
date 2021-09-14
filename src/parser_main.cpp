#include <parser.h>
#include <tokenizer.h>

void printPrompt() {
    cout << "parser>";
    fflush(stdout);
}

void printTokens(vector <Token> &tokens) {
    for (auto i: tokens) {
        i.printToken();
    }
}

vector <string> history;
extern string read_line();
unsigned int ind;
extern int yylex();
extern int yyparse();
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern char *yytext;
int main(int argc, char *argv[]) {
    string line;
    while (1) {
        printPrompt();
        line = read_line();
        if (line.compare("\n")) {
            history.push_back(line);
            ind = history.size();
        }
        YY_BUFFER_STATE  buffer = yy_scan_string((char *)line.c_str());
        yyparse();
        /*vector <Token> tokens = genTokens(line, true);
        printTokens(tokens);
        Tree *parseTree = newTree(tokens);
        if (parseTree->root) {
            cout << "BACKGROUND: " << parseTree->root->background << endl;
            parseTree->root->traverse(0);
        }
        delete parseTree;*/
        //exit(EXIT_SUCCESS);
        yy_delete_buffer(buffer);
    }
    return EXIT_SUCCESS;
}
