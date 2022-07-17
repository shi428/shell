#include <c_headers.h>
const char *built_in[9] = {"alias", "bg", "cd", "fg", "help", "printenv", "setenv", "source", "unsetenv"};
const char *built_in_desc[9] = {"[name] [value]", "", "[dir]", "", "", "", "[name] [value]", "[file]", "[name]"};

bool is_builtin(char *cmd) {
    for (int i = 0; i < 9; i++) {
        if (!strcmp(cmd, built_in[i])) return true;
    }
    return false;
}

void run_builtin_command_help(char **cmd) {
    printf("List of built-in commands\n");
    for (int i = 0; i < 9; i++) {
        printf("\t%s %s\n", built_in[i], built_in_desc[i]);
    }
}
