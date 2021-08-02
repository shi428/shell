#include <built-in.h>
#include <parser.h>
#include <tokenizer.h>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <exec.h>

const char *built_in[4] = {"setenv", "unsetenv", "source", "cd"};

vector <pair<pid_t, vector <string>>> bPids2;
vector <int> pos2;
void sigchild_handler2(int signum) {
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        for (unsigned int i = 0; i < bPids2.size(); i++) {
            if (bPids2[i].first == pid && pos2.size()) {
                cout << endl;
                cout << pos2[i] <<". [" << pid << "] ";
                for (auto i: bPids2[i].second) {
                    cout << i << " ";
                }
                cout << "has exited";
                cout << endl;
                bPids2.erase(bPids2.begin() + i);
                pos2.erase(pos2.begin() + i);
                i--;
                if (isatty(0)) {
                    //printPrompt();
                }
                fflush(stdout);
            }
        }
    }
}
bool isBuiltIn(char *cmd) {
    for (int i = 0; i < 4; i++) {
        if (!strcmp(cmd, built_in[i])) return true;
    }
    return false;
}

bool checkSyntax(char **cmd) {
    int args = 0;
    while (cmd[args]) {
        args++;
    }
    if (!strcmp(cmd[0], "setenv")) {
        return args == 3;
    }
    if (!strcmp(cmd[0], "unsetenv")) {
        return args == 2;
    }
    if (!strcmp(cmd[0], "cd")) {
        return args <= 2;
    }
    if (!strcmp(cmd[0], "source")) {
        return args == 2;
    }
    return true;
}

int runBuiltInCommand(char **cmd, struct passwd *p) {
    if (checkSyntax(cmd)) {
        if (!strcmp(cmd[0], "cd")){
            if (cmd[1] == NULL) {
                string homedir = "/home/" + string(p->pw_name);
                chdir(homedir.c_str());
            }
            else {
                if (chdir(cmd[1]) == -1) {
                    cerr << "cd: no such file or directory: " << cmd[1] << endl;
                }
            }
        }
        if (!strcmp(cmd[0], "setenv")) {
            if (setenv(cmd[1], cmd[2], 1) == -1) {
                cerr << "setenv: failed to set environment variable: " << cmd[2] << endl;
            }
        }
        if (!strcmp(cmd[0], "unsetenv")) {
            if (unsetenv(cmd[1]) == -1) {
                cerr << "unsetenv: failed to unset environment variable: " << cmd[2] << endl;
            }
        }
        if (!strcmp(cmd[0], "source")) {
            string line;
            ifstream fin(cmd[1]);
            if (!fin.good()) {
                cerr << "source: no such file or directory: " << cmd[1] << endl;
            }
            signal(SIGCHLD, sigchild_handler2);
            while (getline(fin, line)) {
                if (isatty(0)) {
                    //                    printPrompt();
                }
                vector <Token> tokens = genTokens(line);
                Tree *parseTree = newTree(tokens);
                //tokens.~vector <Token>();
                if (parseTree->root) {
                    //        parseTree->root->traverse(0);
                    if (exec(p, parseTree->root, bPids2, pos2)) {
                        delete parseTree;
                        break;
                    }
                }
                delete parseTree;
            }
                fin.close();
        }
        return 0;
    }
    cerr << cmd[0]<< ": wrong number of arguments" << endl;
    return -1;
}
