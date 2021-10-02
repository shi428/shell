#include <built-in.h>
#include <parser.h>
#include <misc.h>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <exec.h>
#include <expansion.h>
#include "yacc.yy.hpp"
#include "lex.yy.hpp"

extern char **environ;
const char *built_in[7] = {"alias", "cd", "help", "printenv", "setenv", "source", "unsetenv"};
const char *built_in_desc[7] = {"[name] [value]", "[dir]", "", "", "[name] [value]", "[file]", "[name]"};
unordered_map<string, pair<char **, int>> aliases;

vector <char **> alias_ptrs;
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
    for (int i = 0; i < 7; i++) {
        if (!strcmp(cmd, built_in[i])) return true;
    }
    return false;
}

int getLength(char **cmd) {
    int args = 0;
    while (cmd[args]) {
        args++;
    }
    return args;
}

char **copyCommand(char **cmd) {
    int length = getLength(cmd);
    char **ret = new char*[length + 1];
    for (int i = 0; i < length; i++) {
        ret[i] = new char[strlen(cmd[i]) + 1];
        strcpy(ret[i], cmd[i]);
    }
    ret[length] = NULL;
    return ret;
}

bool checkSyntax(char **cmd) {
    int args = getLength(cmd);
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
    if (!strcmp(cmd[0], "alias")) {
        return args >= 3 || args == 1;
    }
    if (!strcmp(cmd[0], "printenv")) {
        return args == 1;
    }
    if (!strcmp(cmd[0], "help")) {
        return args == 1;
    }
    return true;
}

void printAliases() {
    cout << "aliases" << endl;
    for (auto i: aliases) {
        cout << i.first << "=";
        for (int j = 0; j < i.second.second; j++) {
            cout << i.second.first[j] << " ";
        }
        cout << endl;
    }
}
typedef struct yy_buffer_state * YY_BUFFER_STATE;
void yyrestart(FILE * input_file );
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern void yypush_buffer_state(YY_BUFFER_STATE buffer);
extern void yypop_buffer_state();
extern YY_BUFFER_STATE yy_scan_string(const char * str);
//int yyparse();
//extern void myunput(int c);
int runBuiltInCommand(char **cmd, struct passwd *p) {
    if (checkSyntax(cmd)) {
        if (!strcmp(cmd[0], "alias")) {
            if (cmd[1] == NULL) {
                printAliases();
            }
            else {
                char **ptr = copyCommand(&cmd[1]);
                //auto it = aliases.find(ptr[0]);
                //if (it == aliases.end()) {
                aliases[string(ptr[0])] = pair<char **, int>(&ptr[1], getLength(&cmd[2]));
                alias_ptrs.push_back(ptr);
                //}
                /*else {
                    it->second = pair<char **, int>(&ptr[1], getLength(&cmd[2]));
                }*/
            }
        }
        if (!strcmp(cmd[0], "help")) {
            cout << "List of built-in commands" << endl;
            for (int i = 0; i < 7; i++) {
                cout << '\t' << built_in[i] << " " << built_in_desc[i] << endl;
            }
        }
        if (!strcmp(cmd[0], "printenv")) {
            int i = 0;
            while (environ[i]) {
                printf("%s\n", environ[i++]);
            }
        }
        if (!strcmp(cmd[0], "cd")){
            if (cmd[1] == NULL) {
                string homedir = getenv("HOME");
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
            //signal(SIGCHLD, sigchild_handler);
            
            yyscan_t local;
            yylex_init(&local);
            while (getline(fin, line)) {
                line += '\n';
                YY_BUFFER_STATE buffer = yy_scan_string((char *)line.c_str(), local);
                yyparse(local);
                yy_delete_buffer(buffer, local);
/*                for (size_t i = line.length() - 1; i >= 0; i--) {
                    myunput(line[i]);
                }*/
            }
            yylex_destroy(local);
            fin.close();
        }
        return 0;
    }
    cerr << cmd[0]<< ": wrong number of arguments" << endl;
    return -1;
}

void deleteAliasedCommands() {
    for (auto i: alias_ptrs) {
        char **ptr = i;
        int j = 0;
        while (ptr[j]) {
            delete [] ptr[j];
            j++;
        }
        delete [] ptr;
    }
}
