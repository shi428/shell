#include <process.h>
#include <shell.h>
process::process() {
    this->next = NULL;
    this->argv = NULL;
    this->pid = 0;
    this->completed = 0;
    this->stopped = 0;
    this->status = -1;
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

void process::launch_process(AST *ast, pid_t pgid, int in_file, int out_file, int err_file, int foreground) {
    pid_t pid;

    if (Shell::shell_is_interactive) {

        //put process into process group
        pid = getpid();
        if (pgid == 0) { //group doesn't exist yet
            pgid = pid;
            setpgid(pid, pgid);
        }

        //give process group terminal permissions
        if (foreground) {
                tcsetpgrp(Shell::shell_terminal, pgid);
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
    if (in_file != STDIN_FILENO) {
        dup2 (in_file, STDIN_FILENO);
        close (in_file);
    }
    if (out_file != STDOUT_FILENO) {
        if (out_file == err_file) {
            dup2(err_file, STDERR_FILENO);
        }
        dup2 (out_file, STDOUT_FILENO);
        close (out_file);
    }
    if (out_file != err_file && err_file != STDERR_FILENO) {
        dup2 (err_file, STDERR_FILENO);
        close (err_file);
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
    string status_str;
    if (status == -1) {
        status_str = "Running";
    }
    else if (WIFEXITED(status)) {
        status_str = "Done";
    }
    else if (WIFSTOPPED(status)) {
        if (this->stopped) {
        status_str = "Stopped";
        }
        else {
        status_str = "Continued";
        }
    }
    else if (WIFCONTINUED(status)) {
        status_str = "Continued";
    }
    else if (WTERMSIG(status)) {
        status_str = "Terminated";
    }
    printf("%d %s ", this->pid, status_str.c_str());
    int i = 0;
    while (this->argv[i]) {
        cout << this->argv[i] << " ";
        i++;
    }
}
