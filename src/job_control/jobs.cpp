#include <shell.h>

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

int job::launch_job(AST *ast) {
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

    int retVal;


    //execute a pipeline
    pid = 0xffff; //initialize
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
                return 1;
            }

            if (is_builtin(p->argv[0])/* && strcmp(p->argv[0], "printenv")*/) {
                //run_builtin_command(p->argv);
                flag = 1;
            }
        }

        //if redirection is necessary
        if (p->next) { 
            pipe(myPipe2);
            processOutFile = myPipe2[1];
            errFile = this->stderrFd;

            p->setup_input_pipe(inPipe, &redirectInFile, &inFile);
            p->setup_output_pipe(myPipe, myPipe2, &processOutFile, &outFile);
            p->setup_err_pipe(errPipe, &errFile);
        }
        else { //end of pipeline
            processOutFile = this->stdoutFd;
            errFile = this->stderrFd;

            p->setup_input_pipe(inPipe, &redirectInFile, &inFile);
            p->setup_output_pipe(myPipe, myPipe2, &processOutFile, &outFile);
            p->setup_err_pipe(errPipe, &errFile);
            p->setup_out_err(outErrPipe, &errFile, &processOutFile);

            outFile = this->stdoutFd;
            int i = 0;
            while (p->argv[i]) {
                i++;
            }
            const char *set_uscore[4] = {"setenv", "_", p->argv[i-1], NULL};
            run_builtin_command((char **)set_uscore);
        }
        //input redirection here
        p->redirect_input(redirectInFile, inPipe);

        //execute command
        if (flag == 0) {
            pid = fork();
        }
        if (pid == 0) { //child process
            //use first pipe output for piping
            p->launch_process(ast, this->pgid, inFile, processOutFile, errFile, this->foreground);
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
            if (is_builtin(p->argv[0]) && flag) {
                if (inFile != STDIN_FILENO) {
                    dup2 (inFile, STDIN_FILENO);
                    close (inFile);
                }
                if (processOutFile != STDOUT_FILENO) {
                    if (processOutFile == errFile) {
                        dup2(errFile, STDERR_FILENO);
                    }
                    dup2 (processOutFile, STDOUT_FILENO);
                    close (processOutFile);
                }
                if (processOutFile != errFile && errFile != STDERR_FILENO) {
                    dup2 (errFile, STDERR_FILENO);
                    close (errFile);
                }
                run_builtin_command(p->argv);
                dup2(oldStdin, this->stdinFd);
                dup2(oldStdout, this->stdoutFd);
            }

            p->redirect_out(processOutFile, outFile, myPipe, this->pgid);
            p->redirect_err(errFile, errPipe, this->pgid);
            p->redirect_outerr(outErrPipe, pgid);

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
        retVal = this->put_job_in_foreground(0);
    }
    else {
        setenv("!", to_string(pid).c_str(), 1);
        if (Shell::shellIsInteractive) {
        print_process_information();
        }
        retVal = this->put_job_in_background(0);
    }

    //restore old file descriptors
    dup2(oldStdin, this->stdinFd);
    dup2(oldStdout, this->stdoutFd);
    return retVal;
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

