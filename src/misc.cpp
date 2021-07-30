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

char consumeChar(StringIterator &it, bool parse, bool *escapeQuote) {
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
            *escapeQuote = (c == '\"' || c == '\'');
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

string consumeChars(StringIterator &it, bool parse) {
    string ret = "";
    char c;
    bool escapeQuote = false;
    while ((it.pos < it.len) && ((c = it.peek()) != ' ') && ((c = it.peek()) != '>') && ((c = it.peek()) != '<') && ((c = it.peek()) != '|') && ((c = it.peek()) != '&') && ((c = it.peek()) != '\"' || escapeQuote) && ((c = it.peek()) != '\'' || escapeQuote)) {
        escapeQuote = false;
        ret += consumeChar(it, parse, &escapeQuote);
        //it.advance();
    }
    return ret;
}

void indent(int spaces) {
    for (int i = 0; i < spaces; i++) {
        cout << " ";
    }
}

