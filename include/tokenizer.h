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
    ERROR = 8,
}TokenType;

typedef struct token {
    TokenType type;
    string lexeme;
    void printToken();
}Token;

Token next(StringIterator &it);
vector <Token> genTokens(string &line);
#endif
