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

string consume_spaces(StringIterator &it);
bool is_environ(char *String);
bool detect_wildcard(string &a);
char convert_escape_char(string &escapeCh);
#endif
