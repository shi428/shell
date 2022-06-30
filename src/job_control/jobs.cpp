//#include <jobs.h>
#include <shell.h>
#include "../yacc.yy.hpp"
#include "../lex.yy.hpp"

job::job() {
    this->jobId = 0;
    this->next = NULL;
    this->command = NULL;
    this->firstProcess = NULL;
    this->lastProcess = NULL;
    this->pgid = 0;
    this->notified = 0;
    this->stdinFd = 0;
    this->stdoutFd = 1;
    this->stderrFd = 2;
    this->foreground = 0;
}

job::~job() {
    process *p = this->firstProcess;
    while (p) {
        process* next = p->next;
        delete p;
        p = next;
    }
    free(command);
}

void job::launch_job(AST *ast) {
    pid_t pid;
    int myPipe[2]; //to output/append redirection
    int myPipe2[2]; //to pipe redirection
    int errPipe[2]; //error redirection
    int outErrPipe[2]; //output and error redirection
    int inPipe[2]; //input redirection

    int inFile; 
    int outFile;
    int errFile;
    int processOutFile;
    int redirectInFile;

    inFile = this->stdinFd;
    redirectInFile = this->stdinFd;

    int oldStdin = dup(this->stdinFd);
    int oldStdout = dup(this->stdoutFd);


    //execute a pipeline
    for (process *p = this->firstProcess; p; p = p->next) {
        //running builtin commands that have to be run in the parent
        int flag = 0;

        //aliasing
        char **aliasedCmd = expand_alias(p->argv);
        if (aliasedCmd != p->argv) {
            delete_argv(p->argv); 
            p->argv = copy_command(aliasedCmd);
        }
        if (this->foreground) {
            if (!strcmp(p->argv[0], "exit")) {
                cout << "exit" << endl;
                Shell::exitStatus = 1;
                return ;
            }

            if (!strcmp(p->argv[0], "cd")) {
                run_builtin_command(p->argv);
                flag = 1;
            }

            if (!strcmp(p->argv[0], "setenv")) {
                run_builtin_command(p->argv);
                flag = 1;
            }

            if (!strcmp(p->argv[0], "source")) {
                run_builtin_command(p->argv);
                flag = 1;
            }

            if (!strcmp(p->argv[0], "fg")) {
                run_builtin_command(p->argv);
                flag = 1;
            }
            if (!strcmp(p->argv[0], "bg")) {
                run_builtin_command(p->argv);
                flag = 1;
            }
            if (!strcmp(p->argv[0], "unsetenv")) {
                run_builtin_command(p->argv);
                flag = 1;
            }

            if (!strcmp(p->argv[0], "alias") && p->argv[1]) {
                run_builtin_command(p->argv);
                flag = 1;
            }
        }

        //if redirection is necessary
        if (p->next) { 
            pipe(myPipe2);
            processOutFile = myPipe2[1];
            errFile = this->stderrFd;
            if (p->files[0].size()) {
                pipe(inPipe);
                redirectInFile = inFile;
                inFile = inPipe[0];
                if (p->files[0].size() == 1 && redirectInFile == this->stdinFd /*not coming from a pipe*/) {
                    inFile = open(p->files[0][0].c_str(), O_RDONLY);
                }
            }
            if (p->files[1].size() || p->files[4].size()) {
                pipe(myPipe);
                processOutFile = myPipe[1];
                outFile = myPipe2[1];
            }
            if (p->files[2].size()) {
                pipe(errPipe);
                errFile = errPipe[1];
            }
        }
        else { //end of pipeline
            processOutFile = this->stdoutFd;
            errFile = this->stderrFd;
            if (p->files[0].size()) {
                pipe(inPipe);
                redirectInFile = inFile;
                inFile = inPipe[0];
                if (p->files[0].size() == 1 && redirectInFile == this->stdinFd) {
                    inFile = open(p->files[0][0].c_str(), O_RDONLY);
                }
            }
            if (p->files[1].size() || p->files[4].size()) {
                pipe(myPipe);
                processOutFile = myPipe[1];
            }
            if (p->files[2].size()) {
                pipe(errPipe);
                errFile = errPipe[1];
            }
            if (p->files[3].size() || p->files[5].size()) {
                pipe(outErrPipe);
                processOutFile = outErrPipe[1];
                errFile = outErrPipe[1];
            }
            //process_out_file = this->stdoutFd;
            outFile = this->stdoutFd;
        }
        //input redirection here
        if (p->files[0].size() > 1 || (p->files[0].size() && redirectInFile != this->stdinFd)) {
            if (fork() == 0) {
                //reverse(p->files[0].begin(), p->files[0].end());
                my_cat(redirectInFile, inPipe[1], p->files[0]);
                exit(0);
            }
            close(inPipe[1]);
        }

        //execute command
        pid = fork();
        if (pid == 0) { //child process
            if (flag == 0) {
                //use first pipe output for piping
                p->launch_process(ast, this->pgid, inFile, processOutFile, errFile, this->foreground);
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
            if (Shell::shellIsInteractive) {
                if (this->pgid == 0) {
                    this->pgid = pid;
                }
                setpgid(pid, this->pgid);
            }

            //output redirection
            if (p->files[1].size() || p->files[4].size()) {
                //only do if redirect files exist
                close(processOutFile);
                if (fork() == 0) {
                    setpgid(getpid(), this->pgid);
                    my_tee(myPipe[0], outFile, p->files[1], p->files[4]);
                    exit(0);
                }
                //clean up write end of pipe
                if (outFile != this->stdoutFd) {
                    close(outFile);
                }
                close(myPipe[0]);
            }
            //error redirection
            if (p->files[2].size()) {
                close(errFile);
                if (fork() == 0) {
                    setpgid(getpid(), this->pgid);
                    vector <string> empty;
                    my_tee(errPipe[0], 1, p->files[2], empty);
                    exit(0);
                }
                close(errPipe[0]);
            }
            if (p->files[3].size() || p->files[5].size()) {
                close(outErrPipe[1]);
                if (fork() == 0) {
                    setpgid(getpid(), this->pgid);
                    vector <string> empty;
                    my_tee(outErrPipe[0], 1, p->files[3], p->files[5]);
                    exit(0);
                }
                close(outErrPipe[0]);
            }
            //clean pipes
            if (inFile != STDIN_FILENO) {
                close(inFile);
            }
            if (processOutFile != this->stdoutFd) {
                close(processOutFile);
            }
        }
        inFile  = myPipe2[0];
    }

    if (this->foreground) {
        this->put_job_in_foreground(0);
    }
    else {
        print_process_information();
        this->put_job_in_background(0);
    }

    //restore old file descriptors
    dup2(oldStdin, this->stdinFd);
    dup2(oldStdout, this->stdoutFd);
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
    for (auto p: this->substProcesses) {
        delete p;
    }
    return status;
}

int job::job_is_stopped () {
    for (process *p = this->firstProcess; p != NULL; p = p->next) {
        if (p->completed == 0 && p->stopped == 0) {
            return 0;
        }
    }
    /*for (auto p: this->substProcesses) {
        if (p->completed == 0 && p->stopped == 0) {
            return 0;
        }
    }*/
    return 1;
}

int job::job_is_completed () {
    for (process *p = this->firstProcess; p != NULL; p = p->next) {
        if (p->completed == 0) {
            return 0;
        }
    }
    /*for (auto p: this->substProcesses) {
        if (p->completed == 0) {
            return 0;
        }
    }*/
    return 1;
}

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
    //fprintf (stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}

void job::print_job_information() {
    format_job_info(this);
}


int mark_process_status(pid_t pid, int status) {
    if (pid > 0) {
        for (job *j = Shell::jobs->first_job; j; j = j->next) {
            for (process *p = j->firstProcess; p; p = p->next) {
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
            for (auto p: j->substProcesses) {
                if (p->pid == pid) {
                    p->status = status;
                    if (WIFSTOPPED(status)) {
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

    do {
        pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG|WCONTINUED);
    }
    while (!mark_process_status (pid, status));
}

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

job *create_job_from_ast(AST **tr) {
    job *j = new job;
    Shell::currentJob = j;
    string command;
    j->foreground = !(*tr)->root->background;
    string expanded_command = try_expand_command((*tr)->root, command);
    //cout << expanded_command << endl;
    delete *tr;

    //parse expanded command
    yyscan_t local;
    yylex_init(&local);
    YY_BUFFER_STATE buffer = yy_scan_string((char *)expanded_command.c_str(), local);
    yyparse(local, tr, 1);
    yy_delete_buffer(buffer, local);
    yylex_destroy(local);

    traverse_helper((*tr)->root, j, command);
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
        }
        p->argv[index] = NULL;
        for (int i = 0; i < 6; i++) {
            for (auto j: (((Command *)node->obj)->files[i])) {
                p->files[i].push_back(*(string*)j->obj);
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
