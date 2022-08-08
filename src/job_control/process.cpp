//#include <process.h>
#include <shell.h>
process::process() {
    this->next = NULL;
    this->argv = NULL;
    this->pid = 0;
    this->status = -1;
    for (int i = 0; i < 4; i++) {
        this->redirectPids[i] = 0;
        this->redirectStatuses[i] = -1;
    }
    this->completed = 0;
    this->stopped = 0;
}

process::~process() {
    if (this->argv == NULL) return ;
    delete_argv(this->argv);
    this->argv = NULL;
}

void delete_argv(char **argv) {
    int i = 0;
    while (argv[i]) {
        free(argv[i]);
        argv[i] = NULL;
        i++;
    }
    delete [] argv;
}

void process::launch_process(AST *ast, vector <int> &pipeFds, pid_t pgid, int inFile, int outFile, int errFile, int foreground) {
    pid_t pid;

    if (Shell::shellIsInteractive) {

        //put process into process group
        pid = getpid();
        if (pgid == 0) { //group doesn't exist yet
            pgid = pid;
            setpgid(pid, pgid);
        }

        //give process group terminal permissions
        if (foreground) {
            tcsetpgrp(Shell::shellTerminal, pgid);
        }

        //Set the handling for job control signals back to the default.
        signal (SIGINT, SIG_DFL);
        signal (SIGQUIT, SIG_DFL);
        signal (SIGTSTP, SIG_DFL);
        signal (SIGTTIN, SIG_DFL);
        signal (SIGTTOU, SIG_DFL);
        signal (SIGCHLD, SIG_DFL);
    }

    //setup pipes
    if (inFile != STDIN_FILENO) {
        dup2 (inFile, STDIN_FILENO);
        close(inFile);
    }
    if (outFile != STDOUT_FILENO) {
        dup2 (outFile, STDOUT_FILENO);
        if (outFile == errFile) {
            dup2(errFile, STDERR_FILENO);
        }
    }
    if (outFile != errFile && errFile != STDERR_FILENO) {
        dup2 (errFile, STDERR_FILENO);
    }

    for (auto i: pipeFds) {
        if (i != inFile) {
        close(i);
        }
    }
    if (is_builtin(this->argv[0])) {
        run_builtin_command(this->argv);
        delete ast;
        Shell::destroy_shell();
        exit(0);
    }
    //exec process, exit if theres an error
    execvp (this->argv[0], this->argv);
    perror ("execvp");
    exit (1);
}

void process::print_process_info() {
    string statusStr;
    if (status == -1) {
        statusStr = "Running";
    }
    else if (WIFEXITED(status)) {
        statusStr = "Done";
    }
    else if (WIFSTOPPED(status)) {
        statusStr = "Stopped";
    }
    else if (WIFCONTINUED(status)) {
        statusStr = "Continued";
    }
    else if (WTERMSIG(status)) {
        statusStr = "Terminated";
    }
    printf("%d %s ", this->pid, statusStr.c_str());
    int i = 0;
    while (this->argv[i]) {
        cout << this->argv[i] << " ";
        i++;
    }
}
