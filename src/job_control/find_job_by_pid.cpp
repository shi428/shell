#include <shell.h>

job *find_job_by_pid(pid_t pid)
{
    for (job *j = Shell::jobs->first_job; j != NULL; j = j->next) {
        for (process *p = j->firstProcess; p != NULL; p = p->next) {
            if (p->pid == pid) {
                return j;
            }
        }
    }
    return NULL;
}
