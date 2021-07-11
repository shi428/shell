#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <bits/stdc++.h>

using namespace std;
typedef enum tokenType {
    COMMAND = 0,
    FILE_OUT = 1, // >
    FILE_IN = 2, // <
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

typedef struct stringIterator {
    stringIterator(string &line);
    ~stringIterator();
    char lookahead(int n);
    char **it;
    unsigned int pos;
    unsigned int len;
}StringIterator;

Token next(StringIterator &it);
char peek(StringIterator &it);
char advance(StringIterator &it);
string consumeSpaces(StringIterator &it);
vector <Token> genTokens(string &line);
void removeWhiteSpace(string &line);
void printPrompt();
#endif
