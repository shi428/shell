#include <shell_util.h>

using namespace std;
extern unsigned int ind;
extern vector <string> history;
void set_tty_raw_mode();
void tty_reset();
Trie *trie;
Trie *buildTrie(string &currentdir);
string current;
vector <pair <unsigned int, string>> args;
void splitString(string &line) {
    StringIterator it(line);
    while (it.pos < it.len) {
        string spaces = consume_spaces(it);
        string arg = "";
        unsigned int pos = it.pos;
        if (spaces.length() && it.pos == it.len) {
            args.push_back(pair<unsigned int, string>(pos, ""));
            break;
        }
        while (it.pos < it.len && it.peek() != ' ') {
            arg += it.advance();
        }
        args.push_back(pair<unsigned int, string>(pos, arg));
    }
}

int getIndex(unsigned int pos) {
    for (unsigned int i = 0; i < args.size(); i++) {
        //cout << pos << " " <<  args[i].first << " " << args[i].first + args[i].second.length() << endl;
        if (pos >= args[i].first && pos <= args[i].second.length() + args[i].first) {
            return i;
        }
        else {
            if (i < args.size() - 1 && pos < args[i+1].first){
                if (pos - (args[i].second.length() + args[i].first) < args[i+1].first - pos) {
                    return i;
                }
                else {
                    return i + 1;
                }
            }
        }
    }
    return -1;
}

unsigned int position;
string read_line() {
    string dir = string(getenv("PWD")) + '/';
    string ret = current;

    set_tty_raw_mode();
    errno = 0;

    write(1, ret.c_str(), ret.length());
    for (unsigned int i= 0; i < ret.length() - position; i++) {
        char ch = '\b';
        write(1, &ch, 1);
    }

    while (1) {
        string trie_str = ret;
        if (isatty(0)) {
            splitString(ret);
            if (args.size()) {
                int index = getIndex(position);
                trie_str = args[index].second;
            }
            trie = buildTrie(trie_str);
        }
        errno = 0;
        char ch;
        if (read(0, &ch, 1) == 0){
            return "";
        }
        if (ch == 4) {
            return "";
        }

        if (ch == '\n') {
            if (isatty(0))
                write(1, &ch, 1);
            ret.push_back(ch);
            current = "";
            position = 0;
            delete trie;
            break;
        }

        if (ch == '\t') {
            size_t slashPos = trie_str.find_last_of('/');
            size_t period = trie_str.find_last_of('.');
            if (period == 0 && period == slashPos - 1 && slashPos < trie_str.length() - 1) {
                string substr = trie_str.substr(0, period);
                slashPos = substr.find_last_of('/');
            }
            string filename = slashPos != string::npos? trie_str.substr(slashPos + 1) : trie_str;
            string empty = filename;
            string fill = trie->try_complete(filename);
            TrieNode *node = trie->search(filename);
            ret.insert(position, fill);
            position += fill.length();

            write(1, fill.c_str(), fill.length());
            write(1, ret.substr(position).c_str(), ret.length()-position);
            for (unsigned int i= 0; i < ret.length() - position; i++) {
                ch = '\b';
                write(1, &ch, 1);
            }
            if (!fill.compare("")) {
                if (node && node->n_children) {
                    ch = '\n';
                    write(1, &ch, 1);
                    trie->traverse(node, empty);
                    current = ret;
                    ret = ch;
                    tty_reset();
                    delete trie;
                    return ret;
                }
                else {
                    current = "";
                }
            }
        }
        if (ch == 127) {
            if (position > 0) {
                for (unsigned int i = 0; i < ret.length() - position; i++) {
                    char str[] = "\e[C";
                    write(1, str, strlen(str));
                }
                for (unsigned int i = 0; i < ret.length() - position; i++) {
                    ch = '\b';
                    write(1, &ch, 1);
                    ch = ' ';
                    write(1, &ch, 1);
                    ch = '\b';
                    write(1, &ch, 1);
                }
                ret.erase(ret.begin() + --position);
                ch = '\b';
                write(1, &ch, 1);
                ch = ' ';
                write(1, &ch, 1);
                ch = '\b';
                write(1, &ch, 1);
                write(1, ret.substr(position).c_str(), ret.length()-position);
                for (unsigned int i= 0; i < ret.length() - position; i++) {
                    ch = '\b';
                    write(1, &ch, 1);
                }
            }
        }
        else {
            if (ch == 27)  {
                char ch1;
                char ch2;
                read(0, &ch1, 1);
                read(0, &ch2, 1);
                if (ch1 == 91 && ch2 == 68 && position > 0) {
                    ch = '\b';
                    write(1, &ch, 1);
                    position--;
                }
                if (ch1 == 91 && ch2 == 67 && position < ret.length()) {
                    char str[] = "\e[C";
                    write(1, str, strlen(str));
                    position++;
                }
                if (ch1 == 91 && ch2 == 65 && ind > 0) {
                    ind--;
                    for (unsigned int i = 0; i < ret.length() - position; i++) {
                        char str[] = "\e[C";
                        write(1, str, strlen(str));
                    }
                    for (unsigned int i = 0; i < ret.length(); i++) {
                        ch = '\b';
                        write(1, &ch, 1);
                        ch = ' ';
                        write(1, &ch, 1);
                        ch = '\b';
                        write(1, &ch, 1);
                    }
                    ret = history[ind];
                    ret.pop_back();
                    position = ret.length();
                    write(1, ret.c_str(), ret.length());
                }
                if (ch1 == 91 && ch2 == 66 && history.size()) {
                    if ( ind < history.size() - 1) {
                        ind++;
                        for (unsigned int i = 0; i < ret.length() - position; i++) {
                            char str[] = "\e[C";
                            write(1, str, strlen(str));
                        }
                        for (unsigned int i = 0; i < ret.length(); i++) {
                            ch = '\b';
                            write(1, &ch, 1);
                            ch = ' ';
                            write(1, &ch, 1);
                            ch = '\b';
                            write(1, &ch, 1);
                        }
                        ret = history[ind];
                        ret.pop_back();
                        position = ret.length();
                        write(1, ret.c_str(), ret.length());
                    }
                    else {
                        ind = history.size();
                        for (unsigned int i = 0; i < ret.length() - position; i++) {
                            char str[] = "\e[C";
                            write(1, str, strlen(str));
                        }
                        for (unsigned int i = 0; i < ret.length(); i++) {
                            ch = '\b';
                            write(1, &ch, 1);
                            ch = ' ';
                            write(1, &ch, 1);
                            ch = '\b';
                            write(1, &ch, 1);
                        }
                        ret = "";
                        position = 0;
                        write(1, ret.c_str(), ret.length());
                    }
                }
            }
            else if (ch >= 0x20) {
                if (position == ret.length()) {
                    ret.push_back(ch);
                    position++;
                }
                else {
                    ret.insert(ret.begin() + position, ch);
                    position++;
                }
                if (isatty(0))
                    write(1, &ch, 1);
                write(1, ret.substr(position).c_str(), ret.length()-position);
                for (unsigned int i= 0; i < ret.length() - position; i++) {
                    ch = '\b';
                    write(1, &ch, 1);
                }
            }
        }
        if (isatty(0)) {
            delete trie;
        }
        args.clear();
    }
    tty_reset();
    args.clear();
    return ret;
}
