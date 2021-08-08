#include <built-in.h>
#include <parser.h>
#include <tokenizer.h>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <exec.h>
#include <expansion.h>
extern char **environ;
const char *built_in[4] = {"setenv", "unsetenv", "source", "cd"};

extern vector <pair<pid_t, vector <string>>> bPids;
extern vector <int> pos;
extern void printPrompt();
extern void sigchild_handler(int signum);
void sigchild_handler2(int signum) {
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        for (unsigned int i = 0; i < bPids.size(); i++) {
            if (bPids[i].first == pid && pos.size()) {
                cout << endl;
                cout << pos[i] <<". [" << pid << "] ";
                for (auto i: bPids[i].second) {
                    cout << i << " ";
                }
                cout << "has exited";
                cout << endl;
                bPids.erase(bPids.begin() + i);
                pos.erase(pos.begin() + i);
                i--;
                if (isatty(0)) {
                    printPrompt();
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
                    return -1;
                }
            }
            char **setEnvCmd = new char *[4];
            setEnvCmd[0] = new char[7];
            setEnvCmd[1] = new char[4];
            setEnvCmd[2] = get_current_dir_name();
            setEnvCmd[3] = NULL;
            strcpy(setEnvCmd[0], "setenv");
            strcpy(setEnvCmd[1], "PWD");
            runBuiltInCommand(setEnvCmd, p);
            delete [] setEnvCmd[0];
            delete [] setEnvCmd[1];
            delete [] setEnvCmd[2];
            delete [] setEnvCmd;
        }
        if (!strcmp(cmd[0], "setenv")) {
            if (setenv(cmd[1], cmd[2], 1) == -1) {
                cerr << "setenv: failed to set environment variable: " << cmd[2] << endl;
            }
        }
        if (!strcmp(cmd[0], "unsetenv")) {
            /*int i = 0;
            bool run = false;
            while (environ[i]) {
                string env = environ[i];
                string token = env.substr(0, env.find('='));
                if (!strcmp(token.c_str(), cmd[1])) {
                    run = true;
                    break;
                }
                i++;
            }*/
            if (isEnviron(cmd[1])) {
                if (unsetenv(cmd[1]) == -1) {
                    cerr << "unsetenv: failed to unset environment variable: " << cmd[1] << endl;
                }
            }
            else {
                cerr << "unsetenv: failed to unset environment variable: " << cmd[1] << endl;
            }
        }
        if (!strcmp(cmd[0], "source")) {
            string line;
            ifstream fin(cmd[1]);
            if (!fin.good()) {
                cerr << "source: no such file or directory: " << cmd[1] << endl;
                return -1;
            }
            signal(SIGCHLD, sigchild_handler);
            while (getline(fin, line)) {
                if (isatty(0)) {
                    //                    printPrompt();
                }
                vector <Token> tokens = genTokens(line);
                tokens = expand_subshell(tokens);
                Tree *parseTree = newTree(tokens);
                //tokens.~vector <Token>();
                if (parseTree->root) {
                    //        parseTree->root->traverse(0);
                    if (exec(p, parseTree->root, bPids, pos)) {
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
