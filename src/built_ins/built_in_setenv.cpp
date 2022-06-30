#include <shell.h>

void run_builtin_command_setenv(char **cmd) {
    if (setenv(cmd[1], cmd[2], 1) == -1) {
        cerr << "setenv: failed to set environment variable: " << cmd[2] << endl;
    }
}
