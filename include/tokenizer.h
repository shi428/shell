#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <bits/stdc++.h>
#include "misc.h"

using namespace std;
typedef enum tokenType {
    CHAR = 1,
    ESCAPE_CHAR = 2,
    SPACE = 3,
    AMPERSAND = 4,
    PIPE = 5,
    SUBSHELL = 6,
    ENV = 7,
    LEFT_PAREN = 8,
    RIGHT_PAREN = 9,
    GREAT = 10,
    GREATAND = 11,
    GREATGREAT = 12,
    GREATGREATAND = 13,
    IOERR = 14,
    LESS = 15, //|
    DQUOTE = 16, //' or "
    SQUOTE = 17,
    LEFT_BRACE = 18,
    RIGHT_BRACE = 19,
    NEWLINE = 20,
}TokenType;

typedef struct token {
    token(){
        flag = false;
    }
    TokenType type;
    bool flag;
    string lexeme;
    void printToken();
}Token;

Token next(StringIterator &it, bool escape);
vector <Token> genTokens(string &line, bool escape);
#endif
