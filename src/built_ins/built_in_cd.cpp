#include <shell.h>

void run_builtin_command_cd(char **cmd) {
    if (cmd[1] == NULL) {
        string homedir = getenv("HOME");
        chdir(homedir.c_str());
    }
    else {
        if (chdir(cmd[1]) == -1) {
            cerr << "cd: no such file or directory: " << cmd[1] << endl;
            return ;
        }
    }

    //set pwd variable
    char **setEnvCmd = new char *[4];
    setEnvCmd[0] = strdup("setenv");
    setEnvCmd[1] = strdup("PWD");
    setEnvCmd[2] = get_current_dir_name();
    setEnvCmd[3] = NULL;
    run_builtin_command(setEnvCmd);
    delete [] setEnvCmd[0];
    delete [] setEnvCmd[1];
    delete [] setEnvCmd[2];
    delete [] setEnvCmd;
}
