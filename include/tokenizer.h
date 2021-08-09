#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <bits/stdc++.h>
#include "misc.h"

using namespace std;
typedef enum tokenType {
    COMMAND = 0,
    FILE_IN = 1, // <
    FILE_OUT = 2, // >
    FILE_ERR = 3, // 2>
    FILE_OUT_ERR = 4, // >&
    FILE_APPEND = 5, //>>
    FILE_APPEND_ERR = 6, //>>&
    PIPE = 7, //|
    AMPERSAND = 8, //&
    QUOTES = 9, //' or "
    SUBSHELL = 10,
    ENV = 11,
    ERROR = 12,
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
