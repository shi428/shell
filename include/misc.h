#ifndef MISC_H
#define MISC_H

#include <bits/stdc++.h>
using namespace std;
typedef struct stringIterator {
    unordered_map<char, char> escapeChars;
    stringIterator(string &line);
    ~stringIterator();
    char lookahead(int n);
    char peek();
    char advance();

    char **it;
    unsigned int pos;
    unsigned int len;
}StringIterator;

void removeWhiteSpace(string &line);
string consumeSpaces(StringIterator &it);
char consumeChar(StringIterator &it, bool parse, bool *escapeQuote);
string consumeChars(StringIterator &it, bool parse);
void indent(int spaces);
void initEscapeChars();
#endif
