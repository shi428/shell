#include <jobs.h>
#include <shell.h>

job::job() {
    this->job_id = 0;
    this->next = NULL;
    this->command = NULL;
    this->first_process = NULL;
    this->last_process = NULL;
    this->pgid = 0;
    this->notified = 0;
    this->stdin_fd = 0;
    this->stdout_fd = 1;
    this->stderr_fd = 2;
    this->foreground = 1;
}

job::~job() {
    process *p = first_process;
    while (p) {
        process* next = p->next;
        delete p;
        p = next;
    }
    free(command);
}

void job::launch_job() {
    pid_t pid;
    int my_pipe[2];
    int in_file;
    int out_file;

    in_file = this->stdin_fd;

    int old_stdin = dup(STDIN_FILENO);
    int old_stdout = dup(STDOUT_FILENO);
    //execute a pipeline
    for (process *p = this->first_process; p; p = p->next) {

        if (p->next) { 
            if (pipe(my_pipe) == -1) {
                perror("pipe");
                exit(1);
            }
            out_file = my_pipe[1];
        }
        else { //end of pipeline
            out_file = this->stdout_fd;
        }
        pid = fork();
        if (pid == 0) { //child process
            p->launch_process(this->pgid, in_file, out_file, this->stderr_fd, foreground);
        }
        else if (pid < 0) {
            perror("fork");
            exit(1);
        }
        else { //parent process
            //assign process group
            p->pid = pid;
            if (Shell::shell_is_interactive) {
                if (this->pgid == 0) {
                    this->pgid = pid;
                }
                setpgid(pid, this->pgid);
            }

            //clean pipes
            if (in_file != this->stdin_fd) {
                close(in_file);
            }
            if (out_file != this->stdout_fd) {
                close(out_file);
            }
        }
        in_file = my_pipe[0];
    }

    if (!Shell::shell_is_interactive) {
        this->wait_for_job();
    }
    else if (this->foreground) {
        this->put_job_in_foreground(0);
    }
    else {
        print_process_information();
        this->put_job_in_background(0);
    }
    dup2(old_stdin, STDIN_FILENO);
    dup2(old_stdout, STDOUT_FILENO);
}

void job::wait_for_job() {
    int status;
    pid_t pid;

    do {
        pid = waitpid (-this->pgid, &status, WUNTRACED);
    }
    while (!mark_process_status (pid, status)
            && !this->job_is_stopped ()
            && !this->job_is_completed ());
}

int job::job_is_stopped () {
    for (process *p = this->first_process; p != NULL; p = p->next) {
        if (p->completed == 0 && p->stopped == 0) {
            return 0;
        }
    }
    return 1;
}

int job::job_is_completed () {
    for (process *p = this->first_process; p != NULL; p = p->next) {
        if (p->completed == 0) {
            return 0;
        }
    }
    return 1;
}

void job::put_job_in_foreground(int cont) {
    tcsetpgrp(Shell::shell_terminal, this->pgid);

    //continue job
    if (cont) {
        tcsetattr (Shell::shell_terminal, TCSADRAIN, &this->tmodes);
        if (kill (- this->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
    }

    this->wait_for_job();
    tcsetpgrp (Shell::shell_terminal, Shell::shell_pgid);
    tcsetattr (Shell::shell_terminal, TCSADRAIN, &Shell::shell_tmodes);
}

void job::put_job_in_background (int cont)
{
    //continue job
    if (cont) {
        if (kill (-this->pgid, SIGCONT) < 0) {
            perror ("kill (SIGCONT)");
        }
    }
}

void job::append_process(process *p) {
    if (this->last_process == NULL) {
        this->last_process = p;
        this->first_process = p;
        return ;
    }
    this->last_process->next = p;
    this->last_process = p;
}

void job::print_process_information() {
    printf("[%d] ", this->job_id);
    for (process *p = this->first_process; p; p = p->next) {
        printf("%d ", p->pid);
    }
    printf("\n");
}

void format_job_info (job *j) {
    printf("[%d] ", j->job_id);
    for (process *p = j->first_process; p->next; p = p->next) {
        if (p != j->first_process) {
            printf("    ");
        }
        p->print_process_info();
        printf("|\n");
    }
    if (j->last_process != j->first_process) {
        printf("    ");
    }
    j->last_process->print_process_info();
    printf("\n");
    //fprintf (stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}

void job::print_job_information() {
    format_job_info(this);
}


int mark_process_status(pid_t pid, int status) {
    if (pid > 0) {
        for (job *j = Shell::jobs->first_job; j; j = j->next) {
            for (process *p = j->first_process; p; p = p->next) {
                if (p->pid == pid) {
                    p->status = status;
                    if (WIFSTOPPED(status)) {
                        printf("\n");
                        format_job_info(j); 
                        p->stopped = 1;
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

    do
        pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG|WCONTINUED);
    while (!mark_process_status (pid, status));
}

job *find_job_by_pid(pid_t pid)
{
    for (job *j = Shell::jobs->first_job; j != NULL; j = j->next) {
        for (process *p = j->first_process; p != NULL; p = p->next) {
            if (p->pid == pid) {
                return j;
            }
        }
    }
    return NULL;
}

job *create_job_from_ast(AST *tr) {
    job *j = new job;
    string command;
    j->foreground = !tr->root->background;
    traverse_helper(tr->root, j, command);
    j->command = strdup(command.c_str());
    return j;
}

void traverse_helper(Node *node, job *j, string &command) {
    if (node->type == COMMAND_NODE) {
        vector <string> args;
        for (auto i: node->children) {
            args.push_back(*(string *)i->obj);
            command += *(string *)i->obj;
            command += " ";
            //cout << *(string *)i->obj << endl;
        }
        ((Command *)node->obj)->createArgs(args);
        process *p = new process();
        p->argv = new char *[args.size() + 1];
        int i = 0;
        while (((Command *)node->obj)->cmd[i]) {
            p->argv[i] = strdup(((Command *)node->obj)->cmd[i]);
            i++;
        }
        p->argv[i] = NULL;
        j->append_process(p);
    }
    else {
        traverse_helper(node->children[0], j, command);
        command += " | ";
        traverse_helper(node->children[1], j, command);
    }
}
