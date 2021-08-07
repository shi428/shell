#include <misc.h>

StringIterator::stringIterator(string &line) {
    escapeChars = {
        {'0', '\0'},
        {'a', '\a'},
        {'b', '\b'},
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

struct escape {
    bool doubleQuote;
    bool singleQuote;
    bool rightCarrot;
    bool leftCarrot;
    bool ampersand;
    bool pipe;
    bool dollar;
};

char consumeChar(StringIterator &it, bool parse, escape *escapeChars){
    char c = it.lookahead(1);
    if (it.peek() == '\\') {
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
    while (((c = it.peek()) == ' ') && (it.pos < it.len)) {
        //if (c == ' ') cout << "space" << endl;
        ret += c;
        it.advance();
    }
    return ret;
}

string consumeChars(StringIterator &it, bool parse, bool singlequote, bool doublequote) {
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
        ret += consumeChar(it, parse, &escapeChars);
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

