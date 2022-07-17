#include <c_headers.h>

void run_builtin_command_setenv(char **cmd) {
    if (setenv(cmd[1], cmd[2], 1) == -1) {
        fprintf(stderr, "setenv: failed to set environment variable: %s\n", cmd[1]);
    }
}
