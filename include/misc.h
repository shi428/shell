#ifndef MISC_H
#define MISC_H


#include <unordered_map>
#include <string>
#include <cstring>
#include <iostream>
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

string consumeSpaces(StringIterator &it);
bool isEnviron(char *String);
bool detectWildcard(string &a);
char convertEscapeChar(string &escapeCh);
#endif
