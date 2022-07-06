#include <jobs.h>

void job::print_process_information() {
    printf("[%d] ", this->jobId);
    for (process *p = this->firstProcess; p; p = p->next) {
        printf("%d ", p->pid);
    }
    printf("\n");
}

void format_job_info (job *j) {
    printf("[%d] ", j->jobId);
    for (process *p = j->firstProcess; p->next; p = p->next) {
        if (p != j->firstProcess) {
            printf("    ");
        }
        p->print_process_info();
        printf("|\n");
    }
    if (j->lastProcess != j->firstProcess) {
        printf("    ");
    }
    j->lastProcess->print_process_info();
    printf("\n");
}

void job::print_job_information() {
    format_job_info(this);
}
