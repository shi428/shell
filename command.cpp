#include "command.h"
#include "misc.h"

Command::~command() {
    int i = 0;
    while (cmd[i]) {
        delete [] cmd[i];
        i++;
    }
    delete [] cmd;
}
void Command::parseCommand(Token &cmd) {
    vector <string> args;
    removeWhiteSpace(cmd.lexeme);
    this->it = new StringIterator(cmd.lexeme); 
    while (this->it->pos < this->it->len) {
        args.push_back(consumeChars(*it));
        consumeSpaces(*it);
    }
    this->cmd = new char*[args.size() + 1];
    for (unsigned int i = 0; i < args.size(); i++) {
        this->cmd[i] = new char[args[i].length() + 1];
        strcpy(this->cmd[i], args[i].c_str());
    }
    this->cmd[args.size()] = NULL;
    delete this->it;
}

string parseFile(Token &file) {
    removeWhiteSpace(file.lexeme);
    StringIterator it(file.lexeme);
    string ret;
    switch (file.type) {
       case FILE_OUT:
           it.advance();
           consumeSpaces(it);
           ret += consumeChars(it);
           break;
       case FILE_IN:
           it.advance();
           consumeSpaces(it);
           ret += consumeChars(it);
           break;
       case FILE_ERR:
           it.advance();
           it.advance();
           consumeSpaces(it);
           ret += consumeChars(it);
           break;
       case FILE_OUT_ERR: 
           it.advance();
           it.advance();
           consumeSpaces(it);
           ret += consumeChars(it);
           break;
       case FILE_APPEND:
           it.advance();
           it.advance();
           consumeSpaces(it);
           ret += consumeChars(it);
           break;
       case FILE_APPEND_ERR:
           it.advance();
           it.advance();
           it.advance();
           consumeSpaces(it);
           ret += consumeChars(it);
           break;
       default: break;
    }
    return ret;
}

void Command::parseFileList(vector <Token> &files) {
    for (auto i: files) {
        this->files[i.type - 1].push_back(parseFile(i));
    }
}

void Command::printCommand(int spaces) {
    indent(spaces);
    cout << "COMMAND = [";
    int i = 0;
    while (cmd[i]) {
        if (cmd[i+1] == NULL) {
            cout << cmd[i] << "]" << endl;
        }
        else {
        cout << cmd[i] << " ";
        }
        i++;
    }
    indent(spaces);
    cout << "FILES = {" << endl;
    for (int i = 0; i < 6; i++) {
        indent(spaces);
        string commandType;
        switch (i + 1) {
        case FILE_OUT:
            commandType = "FILE_OUT";
            break;
        case FILE_IN: 
            commandType = "FILE_IN";
            break;
        case FILE_ERR:
            commandType = "FILE_ERR";
            break;
        case FILE_OUT_ERR:
            commandType = "FILE_OUT_ERR";
            break;
        case FILE_APPEND:
            commandType = "FILE_APPEND";
            break;
        case FILE_APPEND_ERR:
            commandType = "FILE_APPEND_ERR";
            break;
        }
        cout << commandType << " : ";
        for (unsigned int j = 0; j < files[i].size(); j++) {
        if (j == files[i].size() - 1) {
            cout << files[i][j];
        }
        else {
        cout << files[i][j] << " , ";
        }
        }
        cout << endl;
    }
}
