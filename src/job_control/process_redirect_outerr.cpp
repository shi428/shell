#include <shell.h>
void process::redirect_outerr(vector <int> &pipeFds, int *redirectOutErrPipe, pid_t pgid) {
    if (this->files[3].size() || this->files[5].size()) {
        if (fork() == 0) {
            setpgid(getpid(), pgid);
            dup2(redirectOutErrPipe[0], STDIN_FILENO);
            for (auto i: pipeFds) {
                close(i);
            }
            my_tee(1, this->files[3], this->files[5]);
            exit(0);
        }
    }
}
