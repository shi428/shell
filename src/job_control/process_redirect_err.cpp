#include <shell.h>
void process::redirect_err(int errFile, int *redirectErrPipe, pid_t pgid) {
    if (this->files[2].size()) {
        close(errFile);
        if (fork() == 0) {
            setpgid(getpid(), pgid);
            vector <string> empty;
            my_tee(redirectErrPipe[0], 1, this->files[2], empty);
            exit(0);
        }
        close(redirectErrPipe[0]);
    }
}
