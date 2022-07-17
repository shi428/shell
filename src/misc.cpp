#include <shell_util.h>

StringIterator::stringIterator(string &line) {
    this->it = new char *;
    *(this->it) = &(line[0]);
    this->pos = 0;
    this->len = line.length();
}

stringIterator::~stringIterator() {
    delete this->it;
}

char convert_escape_char(string &escapeCh) {
    StringIterator it(escapeCh);
    unordered_map<char, char> escapeChars = {
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
    if (escapeChars.find(escapeCh[1]) != escapeChars.end()) {
        return escapeChars[escapeCh[1]];
    }
    return escapeCh[1];
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

bool detect_wildcard(string &s) {
    return s.find('*') != string::npos || s.find('?') != string::npos;
}

string consume_spaces(StringIterator &it) {
    string ret = "";
    char c;
    while ((it.pos < it.len) && ((c = it.peek()) == ' ')) {
        //if (c == ' ') cout << "space" << endl;
        ret += c;
        it.advance();
    }
    return ret;
}

/*extern char **environ;
bool is_environ(char *String) {
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
}*/

