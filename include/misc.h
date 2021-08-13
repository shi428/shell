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

struct escape {
    bool doubleQuote;
    bool singleQuote;
    bool rightCarrot;
    bool leftCarrot;
    bool ampersand;
    bool pipe;
    bool dollar;
};
void removeWhiteSpace(string &line);
string consumeSpaces(StringIterator &it);
char consumeChar(StringIterator &it, bool parse, escape *escapeQuote, bool escape);
string consumeChars(StringIterator &it, bool parse, bool singleQuote, bool doubleQuote, bool escape);
bool isEnviron(char *String);
bool detectWildcard(string &a);
void indent(int spaces);
void initEscapeChars();
unordered_map <string, string> getUsers(char *argv);
#endif
