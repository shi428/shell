#include <shell.h>

void expand_left_process_subst(int *writeCommandPipe, string &subshellCommand, string &namedPipe, const char **shell) {
    pipe(writeCommandPipe);
    write(writeCommandPipe[1], subshellCommand.c_str(), subshellCommand.length());
    if (fork() == 0) {
        dup2(writeCommandPipe[0], STDIN_FILENO);
        int out_fd = open(namedPipe.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);
        close(writeCommandPipe[1]);
        execvp(shell[0], (char **)shell);
        exit(0);
    }
    close(writeCommandPipe[1]);
    close(writeCommandPipe[0]);
}
