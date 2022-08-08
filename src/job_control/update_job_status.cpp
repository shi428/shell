#include <shell.h>

int mark_process_status(pid_t pid, int status) {
    if (pid > 0) {
        for (job *j = Shell::jobs->first_job; j; j = j->next) {
            for (process *p = j->firstProcess; p; p = p->next) {
                if (p->pid == pid && status != p->status) {
                    p->status = status;
                    if (WIFSTOPPED(status)) {
                        p->stopped = 1;
                        j->print_job_information();
                    }
                    else if (WIFCONTINUED(status))  {
                        j->print_job_information();
                    }
                    else if (WTERMSIG(status)) {
                        if (j->foreground == 0 || j->job_is_stopped()) {
                            j->print_job_information();
                        }
                        p->completed = 1;
                    }
                }
                for (int i = 0; i < 4; i++) {
                    pid_t redirectPid = p->redirectPids[i];
                    pid_t redirectStatus = p->redirectStatuses[i];
                 //   cout << pid << " " << redirectPid << " " << redirectStatus << " " << status << endl;
                    if (redirectPid == pid && status != redirectStatus) {
                //        cout << "ok" << endl;
                        p->redirectStatuses[i] = status;
                        return 0;
                    }
                }
                /*if (p->inPid == pid && status != p->inStatus) {
                    p->inStatus = status;
                    return 0;
                }*/

                int complete = 1;
                for (int i = 0; i < 4; i++) {
                    if ((p->redirectPids[i] != 0 && !WIFEXITED(p->redirectStatuses[i]))) {
                        complete = 0;
                    }
                }
                if (complete && WIFEXITED(p->status)) {
                    p->completed = 1;
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
