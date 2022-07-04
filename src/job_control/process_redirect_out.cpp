#include <shell.h>
void process::redirect_out(int processOutFile, int outFile, int *redirectOutPipe, pid_t pgid) {
    close(processOutFile);
    if (fork() == 0) {
        setpgid(getpid(), pgid);
        my_tee(redirectOutPipe[0], outFile, this->files[1], this->files[4]);
        exit(0);
    }
    //clean up write end of pipe
    if (outFile != STDOUT_FILENO) {
        close(outFile);
    }
    close(redirectOutPipe[0]);
}
