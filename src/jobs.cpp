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
    this->foreground = 0;
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

void job::launch_job(AST *ast) {
    pid_t pid;
    int my_pipe[2]; //to output/append redirection
    int my_pipe2[2]; //to pipe redirection
    int err_pipe[2]; //error redirection
    int out_err_pipe[2]; //output and error redirection
    int in_pipe[2]; //input redirection

    int in_file; 
    int out_file;
    int err_file;
    int process_out_file;
    int redirect_in_file;

    in_file = this->stdin_fd;
    redirect_in_file = this->stdin_fd;

    int old_stdin = dup(STDIN_FILENO);
    int old_stdout = dup(STDOUT_FILENO);


    //execute a pipeline
    for (process *p = this->first_process; p; p = p->next) {
        //running builtin commands that have to be run in the parent
        int flag = 0;
        if (this->foreground) {
            if (!strcmp(p->argv[0], "exit")) {
                cout << "exit" << endl;
                Shell::exit_status = 1;
                return ;
            }

            if (!strcmp(p->argv[0], "cd")) {
                runBuiltInCommand(p->argv);
                flag = 1;
            }

            if (!strcmp(p->argv[0], "setenv")) {
                runBuiltInCommand(p->argv);
                flag = 1;
            }

            if (!strcmp(p->argv[0], "source")) {
                runBuiltInCommand(p->argv);
                flag = 1;
            }

            if (!strcmp(p->argv[0], "fg")) {
                runBuiltInCommand(p->argv);
                flag = 1;
            }
            if (!strcmp(p->argv[0], "bg")) {
                runBuiltInCommand(p->argv);
                flag = 1;
            }
            if (!strcmp(p->argv[0], "unsetenv")) {
                runBuiltInCommand(p->argv);
                flag = 1;
            }

            if (!strcmp(p->argv[0], "alias") && p->argv[1]) {
                runBuiltInCommand(p->argv);
                flag = 1;
            }
        }

        //if redirection is necessary
        if (p->next) { 
            pipe(my_pipe2);
            process_out_file = my_pipe2[1];
            err_file = this->stderr_fd;
            if (p->files[0].size()) {
                pipe(in_pipe);
                redirect_in_file = in_file;
                in_file = in_pipe[0];
                if (p->files[0].size() == 1 && redirect_in_file == this->stdin_fd /*not coming from a pipe*/) {
                    in_file = open(p->files[0][0].c_str(), O_RDONLY);
                }
            }
            if (p->files[1].size() || p->files[4].size()) {
                pipe(my_pipe);
                process_out_file = my_pipe[1];
                out_file = my_pipe2[1];
            }
            if (p->files[2].size()) {
                pipe(err_pipe);
                err_file = err_pipe[1];
            }
        }
        else { //end of pipeline
            process_out_file = this->stdout_fd;
            err_file = this->stderr_fd;
            if (p->files[0].size()) {
                pipe(in_pipe);
                redirect_in_file = in_file;
                in_file = in_pipe[0];
                if (p->files[0].size() == 1 && redirect_in_file == this->stdin_fd) {
                    in_file = open(p->files[0][0].c_str(), O_RDONLY);
                }
            }
            if (p->files[1].size() || p->files[4].size()) {
                pipe(my_pipe);
                process_out_file = my_pipe[1];
            }
            if (p->files[2].size()) {
                pipe(err_pipe);
                err_file = err_pipe[1];
            }
            if (p->files[3].size() || p->files[5].size()) {
                pipe(out_err_pipe);
                process_out_file = out_err_pipe[1];
                err_file = out_err_pipe[1];
            }
            //process_out_file = this->stdout_fd;
            out_file = this->stdout_fd;
        }
        //input redirection here
        if (p->files[0].size() > 1 || (p->files[0].size() && redirect_in_file != this->stdin_fd)) {
            if (fork() == 0) {
                reverse(p->files[0].begin(), p->files[0].end());
                myCat(redirect_in_file, in_pipe[1], p->files[0]);
                exit(0);
            }
            close(in_pipe[1]);
        }

        //execute command
        pid = fork();
        if (pid == 0) { //child process
            if (flag == 0) {
                //use first pipe output for piping
                p->launch_process(ast, this->pgid, in_file, process_out_file, err_file, this->foreground);
            }
            exit(0);
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

            //output redirection
            if (p->files[1].size() || p->files[4].size()) {
                //only do if redirect files exist
                close(process_out_file);
                if (fork() == 0) {
                    setpgid(getpid(), this->pgid);
                    myTee(my_pipe[0], out_file, p->files[1], p->files[4]);
                    exit(0);
                }
                //clean up write end of pipe
                if (out_file != this->stdout_fd) {
                    close(out_file);
                }
                close(my_pipe[0]);
            }
            //error redirection
            if (p->files[2].size()) {
                close(err_file);
                if (fork() == 0) {
                    setpgid(getpid(), this->pgid);
                    vector <string> empty;
                    myTee(err_pipe[0], 1, p->files[2], empty);
                    exit(0);
                }
                close(err_pipe[0]);
            }
            if (p->files[3].size() || p->files[5].size()) {
                close(out_err_pipe[1]);
                if (fork() == 0) {
                    setpgid(getpid(), this->pgid);
                    vector <string> empty;
                    myTee(out_err_pipe[0], 1, p->files[3], p->files[5]);
                    exit(0);
                }
                close(out_err_pipe[0]);
            }
            //clean pipes
            if (in_file != this->stdin_fd) {
                close(in_file);
            }
            if (process_out_file != this->stdout_fd) {
                close(process_out_file);
            }
        }
        in_file  = my_pipe2[0];
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

int job::wait_for_job() {
    int status;
    pid_t pid;

    do {
        pid = waitpid (-this->pgid, &status, WUNTRACED);
    }
    while (!mark_process_status (pid, status)
            && !this->job_is_stopped ()
            && !this->job_is_completed ());
    return status;
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
    this->foreground = 1;

    //continue job
    tcsetpgrp(Shell::shell_terminal, this->pgid);
    if (cont) {
        //tcsetattr (Shell::shell_terminal, TCSADRAIN, &this->tmodes);
        if (kill (- this->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
    }

    if (WIFSTOPPED(this->wait_for_job()) == 0) {
        Shell::delete_job(this);
    }
    tcsetpgrp (Shell::shell_terminal, Shell::shell_pgid);

    tcgetattr (Shell::shell_terminal, &this->tmodes);
    tcsetattr (Shell::shell_terminal, TCSADRAIN, &Shell::shell_tmodes);
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
                        p->stopped = 1;
                        printf("\n");
                        format_job_info(j); 
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
        int index = 0;
        process *p = new process();
        p->argv = new char *[node->children.size() + 1];
        for (auto i: node->children) {
            args.push_back(*(string *)i->obj);
            p->argv[index] = strdup(((string *)i->obj)->c_str());
            command += *(string *)i->obj;
            command += " ";
            index++;
            //cout << *(string *)i->obj << endl;
        }
        p->argv[index] = NULL;
        //((Command *)node->obj)->createArgs(args);
        /*while (((Command *)node->obj)->cmd[i]) {
          p->argv[i] = strdup(((Command *)node->obj)->cmd[i]);
          i++;
          }*/
        for (int i = 0; i < 6; i++) {
            for (auto j: (((Command *)node->obj)->files[i])) {
                p->files[i].push_back(j);
            }
        }
        j->append_process(p);
    }
    else {
        traverse_helper(node->children[0], j, command);
        command += " | ";
        traverse_helper(node->children[1], j, command);
    }
}
