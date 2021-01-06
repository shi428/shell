#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <unistd.h>
#include "shell.h"

using namespace std;

void printVec(vector <char *> &vec) {
    for (auto i: vec) cout << i << endl;
}

int main(int argc, char *argv[]) {
    struct passwd *p = getpwuid(getuid());
    string input;
    while (1) {
        printPrompt(p);
        getline(cin, input);
        if (input.compare("") == 0) {
            continue;
        }
        if (input.compare("exit") == 0) {
            break;
        }
        vector <char *> cmd = parse_command(input);
        handle_command(p, cmd);
        freeStrings(cmd);
    }
    return EXIT_SUCCESS;
}

vector <char *> parse_command(string &input) {
    //stringstream ss(input);
    vector <char *> ret;
    // while (getline(ss, item, ' ')) {
    //     ret.push_back((char *)item.c_str());
    // }
    int begin = 0;
    unsigned i = 0;
    int j = 0;
    bool oneSpace = false;
    while (i < input.length()) {
        bool spaceFound = false;
        while (input[i] == ' ') {
            if (!spaceFound) {
            spaceFound = true;
            j = i;
            }
            oneSpace = true;
            i++;
        }
        if (spaceFound) {
            char *temp = new char[j - begin];
            strcpy(temp, input.substr(begin, j - begin).c_str());
            ret.push_back(temp);
            begin = i;
        }
        i++;
    }
    unsigned int lastSpace = input.rfind(' ');
    if (!oneSpace || lastSpace < input.length() - 1) {
    char *temp = new char[input.length() - begin + 1];
    strcpy(temp, input.substr(begin).c_str());
    ret.push_back(temp);
    }
    
    //for (unsigned i = 0; i < input.length(); i++) {
    //    if (input[i] == ' ') {
    //        char *temp = new char[i - begin];
    //        strcpy(temp, input.substr(begin, i - begin).c_str());
    //        ret.push_back(temp);
    //        begin = i + 1;
    //    }
    //}
    //char *temp = new char[input.length() - begin + 1];
    //strcpy(temp, input.substr(begin).c_str());
    //ret.push_back(temp);
    return ret;
}

int handle_command(struct passwd *p, vector <char *> &cmd) {
    printVec(cmd);
    int status = 0;
    if (!strcmp(cmd[0], "cd")) {
    //    cout << cmd[1] << endl;
        if (cmd.size() == 1) {
            string home = "/home/" + string(p->pw_name);
            return chdir(home.c_str());
        }
        else {
            if (!strcmp(cmd[1], "~")) {
                string home = "/home/" + string(p->pw_name);
                return chdir(home.c_str());
            }
            if (chdir(cmd[1]) == -1) {
                cerr << "\033[1;41mcd: No such file or directory: " << cmd[1] << "\033[0m\n";
                return -1;
            }
        }
    }
    char **ptr = &cmd[0];
    pid_t child;
    child = fork();
    // cout << child << endl;
    if (child == 0) {
        if (execvp(cmd[0], ptr) == -1) {
            cerr << "\033[1;41mshell: command not found: " << cmd[0] << "\033[0m\n";
            return EXIT_FAILURE;
        }
    }
    else {
        waitpid(child, &status, 0);
    }
    return EXIT_SUCCESS;
}

void freeStrings(vector <char *> &cmd) {
    for (auto i: cmd) {
        delete [] i;
    }
}

void printPrompt(struct passwd *p) {
    char hostname[100];
    char *buf = get_current_dir_name();
    gethostname(hostname, sizeof(hostname));
    cout << p->pw_name << "@" << hostname << ":" << buf << " ";
    free(buf);
}
