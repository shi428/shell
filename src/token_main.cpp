#include <bits/stdc++.h>
#include <misc.h>
#include <trie.h>
#include "yacc.yy.hpp"

using namespace std;

void printPrompt() {
    cout << "tokenizer>";
    fflush(stdout);
}

/*void printTokens(vector <Token> &tokens) {
    for (auto i: tokens) {
        i.printToken();
    }
}*/
vector <string> history;
extern string read_line();
unsigned int ind;
extern int yylex();
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern char *yytext;
int main(int argc, char *argv[]) {
    string line;
    while (1) {
        printPrompt();
        line = read_line();
        if (!line.compare("")) break;
        if (line.compare("\n")) {
            history.push_back(line);
            ind = history.size();
        }
        YY_BUFFER_STATE  buffer = yy_scan_string((char *)line.c_str());
        while (int token_id = yylex()) {
            switch (token_id) {
                case NEWLINE: 
                    cout << "NEWLINE" << endl;
                    break;
                case CHAR: 
                    cout << "CHAR(" << yytext << ")" << endl;
                    break;
                case ESCAPE_CHAR: 
                    cout << "ESCAPE CHAR(" << yytext << ")" << endl;
                    break;
                case AMPERSAND: 
                    cout << "AMPERSAND(" << yytext << ")" << endl;
                    break;
                case PIPE: 
                    cout << "PIPE(" << yytext << ")" << endl;
                    break;
                case SUBSHELL: 
                    cout << "SUBSHELL(" << yytext << ")" << endl;
                    break;
                case ENV: 
                    cout << "ENV(" << yytext << ")" << endl;
                    break;
                case LEFT_PAREN: 
                    cout << "LEFT_PAREN(" << yytext << ")" << endl;
                    break;
                case RIGHT_PAREN: 
                    cout << "RIGHT_PAREN(" << yytext << ")" << endl;
                    break;
                case GREAT: 
                    cout << "GREAT(" << yytext << ")" << endl;
                    break;
                case GREATAND: 
                    cout << "GREATAND(" << yytext << ")" << endl;
                    break;
                case GREATGREAT: 
                    cout << "GREATGREAT(" << yytext << ")" << endl;
                    break;
                case GREATGREATAND: 
                    cout << "GREATGREATAND(" << yytext << ")" << endl;
                    break;
                case IOERR: 
                    cout << "IOERR(" << yytext << ")" << endl;
                    break;
                case LESS: 
                    cout << "LESS(" << yytext << ")" << endl;
                    break;
                case DQUOTE: 
                    cout << "DQUOTE(" << yytext << ")" << endl;
                    break;
                case SQUOTE: 
                    cout << "SQUOTE(" << yytext << ")" << endl;
                    break;
                case LEFT_BRACE: 
                    cout << "LEFT_BRACE(" << yytext << ")" << endl;
                    break;
                case RIGHT_BRACE: 
                    cout << "RIGHT_BRACE(" << yytext << ")" << endl;
                    break;
                default: break;
            }
        }
        yy_delete_buffer(buffer);
    //    vector <Token> tokens = genTokens(line, true);
     //   printTokens(tokens);
    }
    return EXIT_SUCCESS;
}

