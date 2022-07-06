#include <shell.h>

int mark_process_status(pid_t pid, int status) {
    if (pid > 0) {
        for (job *j = Shell::jobs->first_job; j; j = j->next) {
            for (process *p = j->firstProcess; p; p = p->next) {
                if (p->pid == pid) {
                    p->status = status;
                    if (WIFSTOPPED(status)) {
                        p->stopped = 1;
                        printf("\n");
                        j->print_job_information();
                    }
                    else {
                        p->completed = 1;
                    }
                    return 0;
                }
            }
        }
        return -1;
    }
    else if (pid == 0 || errno == ECHILD) {
        return -1;
    }
    perror("waitpid");
    return -1;
}

void update_status() {
    int status;
    pid_t pid;

    do {
        pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG|WCONTINUED);
    }
    while (!mark_process_status (pid, status));
}
