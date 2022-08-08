#include <shell.h>
void process::redirect_out(vector <int> &pipeFds, int processOutFile, int inFile, int outFile, int *redirectOutPipe, pid_t pgid) {
    if (this->files[1].size() || this->files[4].size()) {
        pid_t pid;
        pid = fork();
        if (pid == 0) {
            setpgid(getpid(), pgid); //put process in process group

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

        setpgid(pid, pgid);
        this->redirectPids[1] = pid;
    }
}
