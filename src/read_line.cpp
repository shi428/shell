#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>

using namespace std;
extern unsigned int ind;
extern vector <string> history;
void set_tty_raw_mode();
void tty_reset();
string read_line() {
    string ret;
    set_tty_raw_mode();
    unsigned int pos = 0;
    errno = 0;
    while (1) {
        errno = 0;
        char ch;
        if (read(0, &ch, 1) == 0){
            return "";
        }
        if (ch == '\n') {
            if (isatty(0))
            write(1, &ch, 1);
            ret.push_back(ch);
            break;
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
            else {
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
    }
    tty_reset();
    return ret;
}
