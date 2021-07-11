#include "tokenizer.h"
void printPrompt() {
    cout << "tokenizer>";
}

void removeTrailingWhiteSpace(string &line) {
    while (line.front() == ' ') {
        line.erase(line.begin());
    }
    while (line.back() == ' ') {
        line.pop_back();
    }
}

stringIterator::stringIterator(string &line) {
    this->it = new char *;
    *(this->it) = &(line[0]);
    this->pos = 0;
    this->len = line.length();
}

char stringIterator::lookahead(int n) {
    return *(*(this->it) + n);
}
stringIterator::~stringIterator() {
    delete this->it;
}

char advance(StringIterator &it) {
    char ch = **(it.it);
    *(it.it) += 1;
    it.pos += 1;
    return ch;
}

char peek(StringIterator &it) {
    return **(it.it);
}

string consumeSpaces(StringIterator &it) {
    string ret = "";
    char c;
    while (((c = peek(it)) == ' ') && (it.pos < it.len)) {
        //if (c == ' ') cout << "space" << endl;
        ret += c;
        advance(it);
    }
    return ret;
}

string consumeChars(StringIterator &it) {
    string ret = "";
    char c;
    while ((it.pos < it.len) && ((c = peek(it)) != ' ') && ((c = peek(it)) != '>') && ((c = peek(it)) != '<') && ((c = peek(it)) != '|')) {
        ret += c;
        advance(it);
    }
    return ret;
}

string createFileTokenEntry(StringIterator &it) {
    string ret;
    ret += advance(it);
    ret+= consumeSpaces(it);
    ret+= consumeChars(it);
    ret+= consumeSpaces(it);
    return ret;
}

bool detectError(StringIterator &it) {
    return (peek(it) == '2') && (it.lookahead(1) == '>');
}

Token next(StringIterator &it) {
    Token ret;
    ret.lexeme = string();
    if (peek(it) == '|') {
        ret.lexeme = advance(it);
        ret.type = PIPE;
        consumeSpaces(it);
    }
    else if (detectError(it)) {
        ret.type = FILE_ERR;
        ret.lexeme += advance(it);
        ret.lexeme += createFileTokenEntry(it);
    }
    else if (peek(it) == '<') {
        ret.type = FILE_IN;
        ret.lexeme += createFileTokenEntry(it);
    }
    else if (peek(it) == '>') {
        ret.lexeme += advance(it);
        if (peek(it)== '&')  {
            ret.type = FILE_OUT_ERR;
            ret.lexeme += createFileTokenEntry(it);
        }
        else if (peek(it) == '>') {
            ret.lexeme += advance(it);
            if (peek(it) == '&') {
                ret.type = FILE_APPEND_ERR;
                ret.lexeme += createFileTokenEntry(it);
            }
            else {
            ret.type = FILE_APPEND;
            ret.lexeme += createFileTokenEntry(it);
            }
        }
        else {
        ret.type = FILE_OUT;
        ret.lexeme += createFileTokenEntry(it);
        }
    }
    else {
        ret.type = COMMAND;
        //consumeSpaces(it);
        while (it.pos < it.len && peek(it) != '>' && peek(it) != '<') {
            ret.lexeme += consumeChars(it);
            if (it.pos == it.len || peek(it) == '>' || peek(it) == '<')// reached end
                break;
            string spaces = consumeSpaces(it);
            if (spaces == "") {
                ret.type = ERROR;
                ret.lexeme = "no spaces";
                break;
            }
            if (detectError(it)) {
                break;
            }
            //    else cout << "tard" << spaces.length() << endl;
            ret.lexeme += spaces;
        }
    }
    return ret;
}

void Token::printToken() {
    string commandType;
    switch (this->type) {
        case COMMAND: 
            commandType = "COMMAND";
            break;
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
        case PIPE:
            commandType = "PIPE";
            break;
        case ERROR:
            commandType = "ERROR";
            break;
        default: break;
    }
    cout << commandType << "(" << this->lexeme << ")" << endl;
}

vector <Token> genTokens(string &line) {
    vector <Token> ret;
    removeTrailingWhiteSpace(line);
    cout << line << endl;
    stringIterator it(line);
    while (it.pos < it.len) {
        ret.push_back(next(it));
    }
    //cout << ret.size() << endl;
    return ret;
}
