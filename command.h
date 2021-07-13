#ifndef COMMAND_H
#define COMMAND_H
#include <bits/stdc++.h>
#include "tokenizer.h"

typedef struct command {
    ~command();
    void parseCommand(Token &cmd);
    void parseFileList(vector <Token> &files);
    void printCommand(int spaces);

    char **cmd;
    vector <string> files[6];
    StringIterator *it;

}Command;
#endif
