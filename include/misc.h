#ifndef MISC_H
#define MISC_H

#include <bits/stdc++.h>
using namespace std;
typedef struct stringIterator {
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
string consumeChars(StringIterator &it);
void indent(int spaces);
#endif
