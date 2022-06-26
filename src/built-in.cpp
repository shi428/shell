#include <shell.h>
//#include <expansion.h>
//#include <fstream>
#include "yacc.yy.hpp"
#include "lex.yy.hpp"

extern char **environ;
const char *built_in[9] = {"alias", "bg", "cd", "fg", "help", "printenv", "setenv", "source", "unsetenv"};
const char *built_in_desc[9] = {"[name] [value]", "", "[dir]", "", "", "", "[name] [value]", "[file]", "[name]"};
unordered_map<string, pair<char **, int>> aliases;

vector <char **> alias_ptrs;
//extern vector <int> pos;
bool is_builtin(char *cmd) {
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

char **copy_command(char **cmd) {
    int length = getLength(cmd);
    char **ret = new char*[length + 1];
    for (int i = 0; i < length; i++) {
        ret[i] = strdup(cmd[i]);
    }
    ret[length] = NULL;
    return ret;
}

bool checkSyntax(char **cmd) {
    int args = getLength(cmd);
    if (!strcmp(cmd[0], "setenv")) {
        return args == 3;
    }
    if (!strcmp(cmd[0], "fg")) {
        return args == 1;
    }
    if (!strcmp(cmd[0], "bg")) {
        return args == 1;
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
int run_builtin_command(char **cmd) {
    if (checkSyntax(cmd)) {
        if (!strcmp(cmd[0], "alias")) {
            if (cmd[1] == NULL) {
                printAliases();
            }
            else {
                char **ptr = copy_command(&cmd[1]);
                //auto it = aliases.find(ptr[0]);
                //if (it == aliases.end()) {
                aliases[string(ptr[0])] = pair<char **, int>(&ptr[1], getLength(&cmd[2]));
                alias_ptrs.push_back(ptr);
                //}
            }
        }
        if (!strcmp(cmd[0], "help")) {
            cout << "List of built-in commands" << endl;
            for (int i = 0; i < 9; i++) {
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
            run_builtin_command(setEnvCmd);
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
            if (is_environ(cmd[1])) {
                if (unsetenv(cmd[1]) == -1) {
                    cerr << "unsetenv: failed to unset environment variable: " << cmd[1] << endl;
                }
            }
            else {
                cerr << "unsetenv: failed to unset environment variable: " << cmd[1] << endl;
            }
        }
        if (!strcmp(cmd[0], "fg")) {
            job *j =Shell::find_first_stopped_or_bg_job();
            if (!j) {
                cerr << "fg: No such job" << endl;
                return -1;
            }
            Shell::mark_job_as_running(j);
            j->print_job_information();
            //if (j->foreground || j->job_is_stopped()) {
            j->put_job_in_foreground(1);
            //}
           /* else {
                j->put_job_in_foreground(0);
            }*/
        }
        if (!strcmp(cmd[0], "bg")) {
            job *j =Shell::find_first_stopped_or_bg_job();
            if (!j) {
                cerr << "bg: No such job" << endl;
                return -1;
            }
            Shell::mark_job_as_running(j);
            j->print_job_information();
            j->put_job_in_background(1);
        }
        if (!strcmp(cmd[0], "source")) {
            string line;
            ifstream fin(cmd[1]);
            if (!fin.good()) {
                cerr << "source: no such file or directory: " << cmd[1] << endl;
                return -1;
            }

            yyscan_t local;
            yylex_init(&local);

            signal(SIGTTOU, SIG_IGN);
            while (getline(fin, line)) {
                line += '\n';
                YY_BUFFER_STATE buffer = yy_scan_string((char *)line.c_str(), local);
                AST *ast = NULL;
                yyparse(local, &ast, 0);
                yy_delete_buffer(buffer, local);
                if (ast && ast->root) {
                    job *j = create_job_from_ast(&ast);
                    Shell::insert_job(j);
                    j->launch_job(ast);
                    if (Shell::exit_status) {
                        Shell::delete_job(j);
                    }
                    delete ast;
                }
            }

            yylex_destroy(local);
            fin.close();
        }
        return 0;
    }

    cerr << cmd[0]<< ": wrong number of arguments" << endl;
    return -1;
}

void delete_aliased_commands() {
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
