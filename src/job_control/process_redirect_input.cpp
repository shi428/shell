#include <shell.h>
void process::redirect_input(vector <int> &pipeFds, int redirectInFile, int *redirectInPipe, pid_t pgid) {
    if (this->files[0].size() > 1 || (this->files[0].size() && redirectInFile != STDIN_FILENO)) {
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
            dup2(redirectInFile, STDIN_FILENO);
            dup2(redirectInPipe[1], STDOUT_FILENO);
            for (auto i: pipeFds) {
                //cerr << i << endl;
                close(i);
            }
            my_cat(redirectInFile, redirectInPipe[1], this->files[0]);
            exit(0);
        }

        setpgid(pid, pgid);
        this->redirectPids[0] = pid;
    }
}
