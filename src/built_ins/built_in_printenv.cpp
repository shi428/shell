#include <shell.h>

void run_builtin_command_printenv(char **cmd) {
    int i = 0;
    while (environ[i]) {
        printf("%s\n", environ[i++]);
    }
}
