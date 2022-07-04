#include <shell.h>
#include <yacc.yy.hpp>
#include <lex.yy.hpp>

extern char **environ;


int get_length(char **cmd) {
    int args = 0;
    while (cmd[args]) {
        args++;
    }
    return args;
}

char **copy_command(char **cmd) {
    int length = get_length(cmd);
    char **ret = new char*[length + 1];
    for (int i = 0; i < length; i++) {
        ret[i] = strdup(cmd[i]);
    }
    ret[length] = NULL;
    return ret;
}

bool checkSyntax(char **cmd) {
    int args = get_length(cmd);
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

int run_builtin_command(char **cmd) {
    if (checkSyntax(cmd)) {
        if (!strcmp(cmd[0], "alias")) {
            run_builtin_command_alias(cmd);
        }
        if (!strcmp(cmd[0], "bg")) {
            run_builtin_command_bg(cmd);
        }
        if (!strcmp(cmd[0], "cd")){
            run_builtin_command_cd(cmd);
        }
        if (!strcmp(cmd[0], "fg")) {
            run_builtin_command_fg(cmd);
        }
        if (!strcmp(cmd[0], "help")) {
            run_builtin_command_help(cmd);
        }
        if (!strcmp(cmd[0], "printenv")) {
            run_builtin_command_printenv(cmd);
        }
        if (!strcmp(cmd[0], "setenv")) {
            run_builtin_command_setenv(cmd);
        }
        if (!strcmp(cmd[0], "unsetenv")) {
            run_builtin_command_unsetenv(cmd);
        }
        if (!strcmp(cmd[0], "source")) {
            run_builtin_command_source(cmd);
        }
        return 0;
    }

    cerr << cmd[0]<< ": wrong number of arguments" << endl;
    return -1;
}

