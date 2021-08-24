#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <trie.h>
#include <dirent.h>

using namespace std;
extern unsigned int ind;
extern vector <string> history;
void set_tty_raw_mode();
void tty_reset();
//extern Trie *trie;
Trie *trie;
Trie *buildTrie(string &currentdir);
string current;
string read_line(bool *tab) {
    string dir = string(getenv("PWD")) + '/';
    string ret = current;
    set_tty_raw_mode();
    unsigned int pos = ret.length();
    errno = 0;
    write(1, ret.c_str(), ret.length());
    /*if (!*tab) {
    trie = buildTrie(dir.c_str());
    }*/
    while (1) {
/*        if (ret.back() == '/') {
            string current_dir = dir + '/' + ret;
            if (DIR *d = opendir(current_dir.c_str())) {
                //cout << "ok" << endl;
                closedir(d);
                delete trie;
                trie = buildTrie(current_dir.c_str());
            }
            //cout << current_dir << endl;
        }*/
        string trie_str = ret;
        trie = buildTrie(trie_str);
        errno = 0;
        char ch;
        if (read(0, &ch, 1) == 0){
            return "";
        }
        if (ch == '\n') {
            if (isatty(0))
                write(1, &ch, 1);
            ret.push_back(ch);
            current = "";
            break;
        }
        if (ch == '\t') {
            size_t slash_pos = ret.find_last_of('/');
            size_t period = ret.find_last_of('.');
            if (period == 0 && period == slash_pos - 1 && slash_pos < ret.length() - 1) {
                string substr = ret.substr(0, period);
                slash_pos = substr.find_last_of('/');
            }
            string filename = slash_pos != string::npos? ret.substr(slash_pos + 1) : ret;
            string empty = filename;
            string fill = trie->try_complete(filename);
            TrieNode *node = trie->search(filename);
            ret += fill;
            pos += fill.length();
            write(1, fill.c_str(), fill.length());
            if (!fill.compare("")) {
                if (node && node->n_children) {
                    ch = '\n';
                    write(1, &ch, 1);
                    trie->traverse(node, empty);
                    current = ret;
                    ret = ch;
                    *tab = true;
                    tty_reset();
                    return ret;
                }
                else {
                    current = "";
                }
            }
        }
        if (ch == 127) {
            if (pos > 0) {
                for (unsigned int i = 0; i < ret.length() - pos; i++) {
                    char str[] = "\e[C";
                    write(1, str, strlen(str));
                }
                for (unsigned int i = 0; i < ret.length() - pos; i++) {
                    ch = '\b';
                    write(1, &ch, 1);
                    ch = ' ';
                    write(1, &ch, 1);
                    ch = '\b';
                    write(1, &ch, 1);
                }
                ret.erase(ret.begin() + --pos);
                ch = '\b';
                write(1, &ch, 1);
                ch = ' ';
                write(1, &ch, 1);
                ch = '\b';
                write(1, &ch, 1);
                write(1, ret.substr(pos).c_str(), ret.length()-pos);
                for (unsigned int i= 0; i < ret.length() - pos; i++) {
                    ch = '\b';
                    write(1, &ch, 1);
                }
            }
            /*ch = '\b';
              write(1, &ch, 1);*/
        }
        else {
            if (ch == 27)  {
                char ch1;
                char ch2;
                read(0, &ch1, 1);
                read(0, &ch2, 1);
                if (ch1 == 91 && ch2 == 68 && pos > 0) {
                    ch = '\b';
                    write(1, &ch, 1);
                    pos--;
                }
                if (ch1 == 91 && ch2 == 67 && pos < ret.length()) {
                    char str[] = "\e[C";
                    write(1, str, strlen(str));
                    pos++;
                }
                if (ch1 == 91 && ch2 == 65 && ind > 0) {
                    ind--;
                    for (unsigned int i = 0; i < ret.length() - pos; i++) {
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
                    pos = ret.length();
                    write(1, ret.c_str(), ret.length());
                }
                if (ch1 == 91 && ch2 == 66 && history.size()) {
                    if ( ind < history.size() - 1) {
                        ind++;
                        for (unsigned int i = 0; i < ret.length() - pos; i++) {
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
                        pos = ret.length();
                        write(1, ret.c_str(), ret.length());
                    }
                    else {
                        ind = history.size();
                        for (unsigned int i = 0; i < ret.length() - pos; i++) {
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
                        pos = 0;
                        write(1, ret.c_str(), ret.length());
                    }
                }
            }
            else if (ch >= 0x20) {
                if (pos == ret.length()) {
                    ret.push_back(ch);
                    pos++;
                }
                else {
                    ret.insert(ret.begin() + pos, ch);
                    pos++;
                }
                if (isatty(0))
                    write(1, &ch, 1);
                write(1, ret.substr(pos).c_str(), ret.length()-pos);
                for (unsigned int i= 0; i < ret.length() - pos; i++) {
                    ch = '\b';
                    write(1, &ch, 1);
                }
            }
        }
        *tab = false;
        delete trie;
    }
    tty_reset();
    return ret;
}
