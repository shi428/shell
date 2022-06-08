#include <shell_util.h>

StringIterator::stringIterator(string &line) {
    escapeChars = {
        {' ', ' '},
        {'a', '\a'},
        {'b', '\b'},
        {'e', '\033'},
        {'f', '\f'},
        {'n', '\n'},
        {'t', '\t'},
        {'r', '\r'},
        {'t', '\t'},
        {'v', '\v'},
        {'\'', '\''},
        {'\"', '\"'},
        {'\?', '\?'},
        {'\\', '\\'},
    };
    this->it = new char *;
    *(this->it) = &(line[0]);
    this->pos = 0;
    this->len = line.length();
}

char convertEscapeChar(string &escapeCh) {
    StringIterator it(escapeCh);
    unordered_map<char, char> escapeChars = it.escapeChars;
    if (escapeChars.find(escapeCh[1]) != escapeChars.end()) {
        return escapeChars[escapeCh[1]];
    }
    return escapeCh[1];
}
StringIterator::~stringIterator() {
    delete this->it;
}

char StringIterator::lookahead(int n) {
    return *(*(this->it) + n);
}

char StringIterator::peek() {
    return **(this->it);
}

char StringIterator::advance() {
    char ch = **(this->it);
    *(this->it) += 1;
    this->pos += 1;
    return ch;
}

void removeWhiteSpace(string &line) {
    while (line.front() == ' ') {
        line.erase(line.begin());
    }
    while (line.back() == ' ') {
        line.pop_back();
    }
}

bool detectWildcard(string &s) {
    return s.find('*') != string::npos || s.find('?') != string::npos;
}

char consumeChar(StringIterator &it, bool parse, escape *escapeChars, bool escapeFlag){
    char c = it.lookahead(1);
    if (it.peek() == '\\' && escapeFlag) {
        if (parse) {
            it.advance();
            it.advance();
            if (it.escapeChars.find(c) != it.escapeChars.end()) {
                return it.escapeChars[c];
            }
            return c;
        }
        else {
            escapeChars->doubleQuote = c == '\"';
            escapeChars->singleQuote = c == '\'';
            escapeChars->rightCarrot = c == '>';
            escapeChars->leftCarrot = c == '<';
            escapeChars->ampersand = c == '&';
            escapeChars->pipe = c == '|';
            escapeChars->dollar = c == '$';
            /* if (c == '<' || c == '>') {
               it.advance();
               it.advance();
               return c;
               }*/
        }
    }
    return it.advance();
}
string consumeSpaces(StringIterator &it) {
    string ret = "";
    char c;
    while ((it.pos < it.len) && ((c = it.peek()) == ' ')) {
        //if (c == ' ') cout << "space" << endl;
        ret += c;
        it.advance();
    }
    return ret;
}

string consumeChars(StringIterator &it, bool parse, bool singlequote, bool doublequote, bool escapeFlag) {
    string ret = "";
    char c;
    escape escapeChars;
    escapeChars.doubleQuote = false;
    escapeChars.singleQuote = false;
    escapeChars.rightCarrot = false; 
    escapeChars.leftCarrot = false; 
    escapeChars.ampersand = false; 
    escapeChars.pipe = false; 
    escapeChars.dollar = false; 
    bool quote = singlequote || doublequote;
    while ((it.pos < it.len) && (((c = it.peek()) != ' ') || quote) && (((c = it.peek()) != '>') || quote || escapeChars.rightCarrot) && (((c = it.peek()) != '<') || quote || escapeChars.leftCarrot) && (((c = it.peek()) != '|') || quote || escapeChars.pipe) && (((c = it.peek()) != '&') || quote || escapeChars.ampersand) && (((c = it.peek())!= '$' || !(it.lookahead(1) == '{' || it.lookahead(1) == '(') || it.pos == it.len - 1) || escapeChars.dollar) && ((c = it.peek()) != '\"' || escapeChars.doubleQuote || singlequote) && ((c = it.peek()) != '\'' || escapeChars.singleQuote || doublequote) ) {
        escapeChars.doubleQuote = false;
        escapeChars.singleQuote = false;
        escapeChars.rightCarrot = false; 
        escapeChars.leftCarrot = false; 
        escapeChars.ampersand = false; 
        escapeChars.dollar = false; 
        ret += consumeChar(it, parse, &escapeChars, escapeFlag);
        //       cout << it.pos << " " << it.len <<  " " << endl; //escapeQuote << endl;
        //it.advance();
    }
    return ret;
}

void indent(int spaces) {
    for (int i = 0; i < spaces; i++) {
        cout << " ";
    }
}
extern char **environ;
bool isEnviron(char *String) {
    int i = 0;
    while (environ[i]) {
        string env = string(environ[i]);
        string token = env.substr(0, env.find('='));
        if (!strcmp(token.c_str(), String)) {
            return true;
        }
        i++;
    }
    return false;
}

