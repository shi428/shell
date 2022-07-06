#include <shell.h>

void job::put_job_in_foreground(int cont) {
    int status;

    this->foreground = 1;

    //continue job
    tcsetpgrp(Shell::shellTerminal, this->pgid);
    if (cont) {
        //tcsetattr (Shell::shellTerminal, TCSADRAIN, &this->tmodes);
        if (kill (- this->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
    }
    status = this->wait_for_job();
    tcsetpgrp (Shell::shellTerminal, Shell::shellPgid);

    tcgetattr (Shell::shellTerminal, &this->tmodes);
    tcsetattr (Shell::shellTerminal, TCSADRAIN, &Shell::shellTmodes);
    if (WIFSTOPPED(status) == 0) {
        Shell::lastJobExitStatus = WEXITSTATUS(status);
        Shell::delete_job(this);
    }
}

void job::put_job_in_background (int cont)
{
    this->foreground = 0;
    //continue job
    if (cont) {
        if (kill (-this->pgid, SIGCONT) < 0) {
            perror ("kill (SIGCONT)");
        }
    }
}

void job::append_process(process *p) {
    if (this->lastProcess == NULL) {
        this->lastProcess = p;
        this->firstProcess = p;
        return ;
    }
    this->lastProcess->next = p;
    this->lastProcess = p;
}
