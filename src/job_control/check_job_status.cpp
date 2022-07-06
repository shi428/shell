#include <jobs.h>

int job::job_is_stopped () {
    for (process *p = this->firstProcess; p != NULL; p = p->next) {
        if (p->completed == 0 && p->stopped == 0) {
            return 0;
        }
    }
    return 1;
}

int job::job_is_completed () {
    for (process *p = this->firstProcess; p != NULL; p = p->next) {
        if (p->completed == 0) {
            return 0;
        }
    }
    return 1;
}
