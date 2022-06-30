#include <shell.h>

void run_builtin_command_unsetenv(char **cmd) {
    if (is_environ(cmd[1])) {
        if (unsetenv(cmd[1]) == -1) {
            cerr << "unsetenv: failed to unset environment variable: " << cmd[1] << endl;
        }
    }
    else {
        cerr << "unsetenv: failed to unset environment variable: " << cmd[1] << endl;
    }
}
