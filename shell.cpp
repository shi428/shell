#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
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

int detectCarrots(vector <char *> &cmd) {
    for (int i = 0; i < (int)cmd.size(); i++) {
        if (!strcmp(">", cmd[i]) || !strcmp("<", cmd[i])) return i;
    }
    return -1;
}

int handle_command(struct passwd *p, vector <char *> &cmd) {
    int carrot = detectCarrots(cmd);
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
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }
    }
    int io_copy;
    int fd;
    int mode = -1;
    pid_t child;
    if (carrot != -1) {
        if (!strcmp(cmd[carrot], "<")) {
            io_copy = dup(STDIN_FILENO);
            close(STDIN_FILENO);
            if ((int)cmd.size() == carrot + 1) {
                return EXIT_FAILURE;
            }
            fd = open(cmd[carrot + 1], O_RDONLY);
            mode = 0;
            string line;
            cmd.erase(cmd.begin() + carrot, cmd.end());
        }
        if (!strcmp(cmd[carrot], ">")) {
            io_copy = dup(STDOUT_FILENO);
            if ((int)cmd.size() == carrot + 1) {
                return EXIT_FAILURE;
            }
            close(STDOUT_FILENO);
            fd = open(cmd[carrot + 1], O_CREAT | O_TRUNC | O_WRONLY, 00644);
            mode = 1;
            // dup2(fd, STDOUT_FILENO);
            //close(fd);
            cmd.erase(cmd.begin() + carrot, cmd.end());
        }
    }
    char **ptr = new char*[cmd.size() + 1];
    for (int i = 0; i < (int)cmd.size(); i++) {
        ptr[i] = cmd[i];
    }
    ptr[cmd.size()] = NULL;
    child = fork();
    if (child == 0) {
        if (execvp(cmd[0], ptr) == -1) {
            cerr << "\033[1;41mshell: command not found: " << cmd[0] << "\033[0m\n";
            exit(EXIT_FAILURE);
        }
    }
    else {
        waitpid(child, &status, 0);
        if (carrot != -1) {
            int fd2;
            close(fd);
            switch (mode) {
                case 0:
                    fd2 = dup2(io_copy, STDIN_FILENO);
                    cout << fd2 << endl;
                    close(io_copy);
                case 1:
                    fd2 = dup2(io_copy, STDOUT_FILENO);
                    cout << fd2 << endl;
                    close(io_copy);
                default: break;
            }
        }
    }
    delete [] ptr;
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
    string cwd = replaceHomeDir(buf, p->pw_name);
    cout << p->pw_name << "@" << hostname << ":" << cwd << " $ ";
    free(buf);
}

string replaceHomeDir(char *buf, char *pw_name) {
    string ret;
    string token = "/home/" + string(pw_name);
    //cout << token << endl;
    string bufstring = string(buf);
    int ind = bufstring.find(token);
    // cout << ind << endl;
    for (unsigned i = 0; i < bufstring.length(); i++) {
        if ((int)i == ind) {
            ret += "~";
        }
        else if ((int)i < ind + (int)token.length() && ind != -1) {
            continue;
        }
        else {
            ret += buf[i];
        }
    }
    return ret;
}
