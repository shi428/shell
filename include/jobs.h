#ifndef JOBS_H
#define JOBS_H

#include <shell_util.h>
#include <process.h>

typedef struct job {
    job();
    ~job();
    void launch_job(AST *ast);
    int wait_for_job();
    int job_is_stopped();
    int job_is_completed();
    void put_job_in_foreground(int cont);
    void put_job_in_background(int cont);
    void append_process(process *p);
    void print_process_information();
    void print_job_information();

    int job_id;
    struct job *next;           
    char *command;              
    process *first_process;     
    process *last_process;
    pid_t pgid;                 
    char notified;              
    struct termios tmodes;      
    int stdin_fd, stdout_fd, stderr_fd;  
    int foreground;
    vector <process *> substProcesses;
} job;

job *find_job_by_pid(pid_t pid);
int mark_process_status(pid_t pid, int status);
void update_status();
job *create_job_from_ast(AST **tr);
void traverse_helper(Node *node, job *j, string &command);
#endif
