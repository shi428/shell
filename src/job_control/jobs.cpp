#include <shell.h>
#include <pipe_selector.h>
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


    int oldStdin = dup(this->stdinFd);
    int oldStdout = dup(this->stdoutFd);
    //int oldStderr = dup(this->stderrFd);

    int retVal;

    inFile = this->stdinFd;
    redirectInFile = this->stdinFd;

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

        vector <int> pipeFds;
        //if redirection is necessary
        if (p->next) { 
            pipe(myPipe2);
            processOutFile = myPipe2[1];
            errFile = this->stderrFd;

            pipeFds.push_back(myPipe2[0]);
            pipeFds.push_back(myPipe2[1]);

            p->setup_input_pipe(pipeFds, inPipe, &redirectInFile, &inFile);
            p->setup_output_pipe(pipeFds, myPipe, myPipe2, &processOutFile, &outFile);
            p->setup_err_pipe(pipeFds, errPipe, &errFile);
        }
        else { //end of pipeline
            processOutFile = this->stdoutFd;
            errFile = this->stderrFd;

            p->setup_input_pipe(pipeFds, inPipe, &redirectInFile, &inFile);
            p->setup_output_pipe(pipeFds, myPipe, myPipe2, &processOutFile, &outFile);
            p->setup_err_pipe(pipeFds, errPipe, &errFile);
            p->setup_out_err(pipeFds, outErrPipe, &errFile, &processOutFile);

            outFile = this->stdoutFd;
            int i = 0;
            while (p->argv[i]) {
                i++;
            }
            const char *set_uscore[4] = {"setenv", "_", p->argv[i-1], NULL};
            run_builtin_command((char **)set_uscore);
        }
        //input redirection here

        //execute command
        //int *pipeOut = pipe_select_output(processOutFile, myPipe, myPipe2, outErrPipe);
        //int *pipeIn = pipe_select_input(inFile, myPipe2, inPipe);
        if (flag == 0) {
            pid = fork();
        }
        if (pid == 0) { //child process
            //use first pipe output for piping
            if (redirectInFile != this->stdinFd) {
                close(redirectInFile);
            }
            p->launch_process(ast, pipeFds, this->pgid, inFile, processOutFile, errFile, this->foreground);
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

            p->redirect_input(pipeFds, redirectInFile, inPipe, this->pgid);
            p->redirect_out(pipeFds, processOutFile, myPipe2[0], outFile, myPipe, this->pgid);
            p->redirect_err(pipeFds, errFile, errPipe, this->pgid);
            p->redirect_outerr(pipeFds, outErrPipe, this->pgid);

            //clean pipes
            /*if (inFile != this->stdinFd) {
              close(inFile);
              }
              if (processOutFile != STDOUT_FILENO) {
              close(processOutFile);
              }*/
        }
        if (p->next) {
            if (inFile != this->stdinFd) {
            close(inFile);
            }
            if (redirectInFile != this->stdinFd) {
                close(redirectInFile);
            }
            for (auto i: pipeFds) {
                if (i != myPipe2[0]) close(i);
            }
        }
        else {
            for (auto i: pipeFds) {
                close(i);
            }
            if (inFile != this->stdinFd) {
            close(inFile);
            }
            if (redirectInFile != this->stdinFd) {
                close(redirectInFile);
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
    //dup2(oldStderr, this->stderrFd);
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
 
