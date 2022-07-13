#include <shell.h>

string expand_tilde(string &tilde) {
    string ret;
    ssize_t slash_pos = tilde.find('/');
    string user;
    string remaining;
    if (slash_pos != string::npos) {
        user = tilde.substr(0, slash_pos);
        remaining = tilde.substr(slash_pos);
    }
    else {
        user = tilde;
    }
    if (user.length() == 0) {
        ret = getenv("HOME") + remaining;
    }
    else {
        if (Shell::users.find(user) != Shell::users.end()) {
            ret = Shell::users[user]+remaining;
        }
        else {
            ret = "\\~"+tilde;
        }
    }
    return ret;
}
