#include <shell.h>
void process::redirect_outerr(int *redirectOutErrPipe, pid_t pgid) {
    close(redirectOutErrPipe[1]);
    if (fork() == 0) {
        setpgid(getpid(), pgid);
        my_tee(redirectOutErrPipe[0], 1, this->files[3], this->files[5]);
        exit(0);
    }
    close(redirectOutErrPipe[0]);
}
