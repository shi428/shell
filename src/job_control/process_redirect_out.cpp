#include <shell.h>
void process::redirect_out(vector <int> &pipeFds, int processOutFile, int inFile, int outFile, int *redirectOutPipe, pid_t pgid) {
    if (this->files[1].size() || this->files[4].size()) {
        if (fork() == 0) {
            setpgid(getpid(), pgid);
            dup2(redirectOutPipe[0], STDIN_FILENO);
            if (outFile != STDOUT_FILENO) {
                dup2(outFile, STDOUT_FILENO);
            }
            for (auto i: pipeFds) {
                close(i);
            }
            my_tee(outFile, this->files[1], this->files[4]);
            exit(0);
        }
    }
}
