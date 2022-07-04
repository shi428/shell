#include <shell.h>

string expand_prompt(char *prompt) {
    string ret;
    while (*prompt) {
        if (*prompt == '\\') {
            switch(prompt[1]) {
                case 'w': {
                              string user = getenv("USER");
                              string pwd = getenv("PWD");
                              size_t pos = pwd.find(Shell::users[user]);
                              if (pos != string::npos) { 
                                  pwd.erase(pwd.begin() + pos, pwd.begin() + pos + Shell::users[user].length());
                                  pwd.insert(pos, "~");
                              }
                              ret += pwd;
                              prompt++;
                              break;
                          }
                case 'u': {
                              ret += getenv("USER");
                              prompt++;
                              break;
                          }
                case 'H': {
                              char hostname[100];
                              gethostname(hostname, sizeof(hostname));
                              ret += hostname;
                              prompt++;
                              break;
                          }
            }
        }
        else {
            ret += *prompt;
        }
        prompt++;
    }
    return ret;
}
