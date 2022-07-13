#include <shell.h>
#include <yacc.yy.hpp>
#include <lex.yy.hpp>

string get_evaluated_string(string &command) {
    yyscan_t local;
    YY_BUFFER_STATE buffer;
    string evaluatedString;
    AST *tr;

    yylex_init(&local);
    buffer = yy_scan_string((char *)command.c_str(), local);
    yyparse(local, &tr, 1);
    yy_delete_buffer(buffer, local);
    yylex_destroy(local);
    evaluatedString = tr->root->get_command(tr->root);
    delete tr;
    return evaluatedString;
}

vector <char> get_tokens(string &command) {
    yyscan_t local;
    YY_BUFFER_STATE buffer;
    vector <char> tokens;
    AST *tr;

    yylex_init(&local);
    buffer = yy_scan_string((char *)command.c_str(), local);
    yyparse(local, &tr, 1);
    yy_delete_buffer(buffer, local);
    yylex_destroy(local);
    tokens = tr->root->children[0]->expansionFlags;
    delete tr;
    return tokens;
}
