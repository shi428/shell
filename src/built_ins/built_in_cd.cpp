#include <c_headers.h>
void run_builtin_command_cd(char **cmd) {
    if (cmd[1] == NULL) {
        char *homedir = getenv("HOME");
        chdir(homedir);
    }
    else {
        if (chdir(cmd[1]) == -1) {
            fprintf(stderr, "cd: no such file or directory: %s\n", cmd[1]);
            return ;
        }
    }

    //set pwd variable
    setenv("PWD", get_current_dir_name(), 1);
    //
}
