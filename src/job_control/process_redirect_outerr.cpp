#include <shell.h>
void process::redirect_outerr(vector <int> &pipeFds, int *redirectOutErrPipe, pid_t pgid) {
    if (this->files[3].size() || this->files[5].size()) {
        pid_t pid;
        pid = fork();
        if (pid == 0) {
            setpgid(getpid(), pgid);

            //set terminal permissions
            if (this->foreground) {
                tcsetpgrp(Shell::shellTerminal, pgid);
            }

            //set signals back to default
            signal (SIGINT, SIG_DFL);
            signal (SIGQUIT, SIG_DFL);
            signal (SIGTSTP, SIG_DFL);
            signal (SIGTTIN, SIG_DFL);
            signal (SIGTTOU, SIG_DFL);
            signal (SIGCHLD, SIG_DFL);
            
            dup2(redirectOutErrPipe[0], STDIN_FILENO);
            for (auto i: pipeFds) {
                close(i);
            }
            my_tee(1, this->files[3], this->files[5]);
            exit(0);
        }

        setpgid(pid, pgid);
        this->redirectPids[3] = pid;
    }
}
