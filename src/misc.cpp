#include <misc.h>

StringIterator::stringIterator(string &line) {
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

string consumeChars(StringIterator &it) {
    string ret = "";
    char c;
    while ((it.pos < it.len) && ((c = it.peek()) != ' ') && ((c = it.peek()) != '>') && ((c = it.peek()) != '<') && ((c = it.peek()) != '|') && ((c = it.peek()) != '&')) {
        ret += c;
        it.advance();
    }
    return ret;
}

void indent(int spaces) {
    for (int i = 0; i < spaces; i++) {
        cout << " ";
    }
}

