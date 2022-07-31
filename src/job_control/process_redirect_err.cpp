#include <shell.h>
void process::redirect_err(vector <int> &pipeFds, int errFile, int *redirectErrPipe, pid_t pgid) {
    if (this->files[2].size()) {
        if (fork() == 0) {
            setpgid(getpid(), pgid);
            vector <string> empty;
            dup2(redirectErrPipe[0], STDIN_FILENO);
            for (auto i: pipeFds) {
                close(i);
            }
            my_tee(1, this->files[2], empty);
            exit(0);
        }
    }
}
