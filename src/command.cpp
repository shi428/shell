#include <shell_util.h>

extern int fdin[2];
extern int yylex_destroy();
Command::~Command() {
    int i = 0;
    while (cmd[i]) {
        free(cmd[i]);
        i++;
    }
    delete [] cmd;
}

void Command::createArgs(vector <string> &cmd) {
    vector <string> args;
    for (unsigned int i = 0; i < cmd.size(); i++) {
        if (detectWildcard(cmd[i])) {
            bool pwd = !(cmd[i].find('/') == 0);
            string wildcard = !pwd? cmd[i] : string(getenv("PWD")) +"/"+ cmd[i];
            vector <string> strings = expandWildcard(wildcard, pwd); 
            if (strings.size() == 0) {
                args.push_back(wildcard);
            }
            for  (auto i: strings) {
                args.push_back(i);
            }
        }
        else {
            args.push_back(cmd[i]);
        }
    }
    this->cmd = new char*[args.size() + 1];
    for (unsigned int i = 0; i < args.size(); i++) {
        this->cmd[i] = strdup(args[i].c_str());
    }
    this->cmd[args.size()] = NULL;
}

void Command::addFile(int id, string &filename) {
    files[id].push_back(filename);
}

int test_out(vector <string> *files) {
    return files[1].size() || files[3].size() || files[4].size() || files[5].size();
}

int test_error(vector <string> *files) {
    return files[2].size() || files[3].size() || files[5].size();
}

int test_redirect(vector <string> *files) {
    return !files[1].size() && !files[2].size() && !files[3].size() && !files[4].size() && !files[5].size();
}

int tty_reset();
int Command::redirectOut(int *pipefd, int *pipefd2, int i, bool pipeflag, bool _append) {
    char **temp_cmd = NULL;
    int start = 1;
    pid_t child;
    if (_append) {
        start = 2;
        temp_cmd = new char *[files[i].size() + 3];
        temp_cmd[0] = new char[4];
        temp_cmd[1] = new char[3];
        strcpy(temp_cmd[0], "tee");
        strcpy(temp_cmd[1], "-a");
    }
    else {
        temp_cmd = new char *[files[i].size() + 2];
        temp_cmd[0] = new char[4];
        strcpy(temp_cmd[0], "tee");
    }
    for (unsigned int j = 0; j < files[i].size(); j++) {
        temp_cmd[j + start] = new char [files[i][j].length() + 1];
        strcpy(temp_cmd[j + start], files[i][j].c_str());
    }
    temp_cmd[files[i].size() + start] = NULL;
    if ((child = fork()) == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        if (pipeflag) {
            dup2(pipefd2[1], STDOUT_FILENO);
            close(pipefd2[0]);
        }
        else {
            int devnull = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC);
            dup2(devnull, STDOUT_FILENO);
            close(devnull);
        }
        execvp(temp_cmd[0], temp_cmd);
        exit(EXIT_SUCCESS);
    }
    close(pipefd[0]);
    if (pipeflag) {
        close(pipefd2[1]);
    }
    for (unsigned int j = 0; j < files[i].size() + start; j++) {
        delete [] temp_cmd[j];
    }
    delete []temp_cmd;
    return 0;
}

int Command::redirectIn(int *pipefd, vector <pid_t> &children, int readfd, int writefd, bool pipeflag) {
    reverse(files[0].begin(), files[0].end());
    myCat(readfd, pipefd[1], files[0], pipeflag);
    close(pipefd[1]);
    return 1;
}

int Command::execute(AST *tr, struct passwd *p, vector <pid_t> &children, int *pipefd, int readfd, int writefd, vector <string> &cmds) {
    int i = 0;
    process *processPtr = new process();
    char **aliased_cmd = expandAlias(cmd);
    char **ptr = aliased_cmd != cmd ? aliased_cmd : cmd;
    if (strcmp(cmd[0], "exit") == 0) {
        cout << "exit" << endl;
        return 1;
    }
    processPtr->argv = copyCommand(ptr);
    //pid_t current_pid;
    pid_t child;
    int fdout[2];
    int fdin[2];
    int fderr[2];
    pipe(fdout);
    pipe(fdin);
    pipe(fderr);
    if (isBuiltIn(ptr[0])) {
        dup2(readfd, STDIN_FILENO);
        dup2(writefd, STDOUT_FILENO);
        if (files[0].size() > 1 || (readfd != STDIN_FILENO && files[0].size())) {
            redirectIn(fdin, children, readfd, writefd, readfd != STDIN_FILENO);
            dup2(fdin[0], STDIN_FILENO);
            //close(fdin[1]);
        }
        else if (files[0].size() == 1) {
            int fd = open(files[0][0].c_str(), O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        //  close(fdin[1]);
        if (files[1].size() || files[4].size()) {
            dup2(fdout[1], STDOUT_FILENO); 
            //close(fdout[0]);
        }
        if ((files[3].size() || files[5].size()) && (writefd == STDOUT_FILENO) && !files[1].size() && !files[2].size() && !files[4].size()) {
            dup2(fdout[1], STDOUT_FILENO);
            dup2(fdout[1], STDERR_FILENO);
            // close(fdout[0]);
        }
        if (files[2].size()) {
            dup2(fderr[1], STDERR_FILENO); 
            //close(fderr[0]);
        }
        runBuiltInCommand(ptr, p);
    }
    /*if (true) {
      }*/
    else {
        if ((child = fork()) == 0) {
            if (shell_is_interactive) {
                //current_pid = getpid();
                //if (!j->pgid) j->pgid = pid;
                //setpgid (pid, j->pgid);
                //if (foreground) {
                //    tcsetpgrp(shell_terminal, pgid);
                // }
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                signal(SIGTTIN, SIG_DFL);
                signal(SIGTTOU, SIG_DFL);
                signal(SIGCHLD, SIG_DFL);
            }
            dup2(readfd, STDIN_FILENO);
            dup2(writefd, STDOUT_FILENO);
            if (files[0].size() > 1 || (readfd != STDIN_FILENO && files[0].size())) {
                redirectIn(fdin, children, readfd, writefd, readfd != STDIN_FILENO);
                dup2(fdin[0], STDIN_FILENO);
                close(fdin[1]);
            }
            else if (files[0].size() == 1) {
                int fd = open(files[0][0].c_str(), O_RDONLY);
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            // close(fdin[1]);
            if (files[1].size() || files[4].size()) {
                dup2(fdout[1], STDOUT_FILENO); 
                close(fdout[0]);
            }
            if ((files[3].size() || files[5].size()) && (writefd == STDOUT_FILENO) && !files[1].size() && !files[2].size() && !files[4].size()) {
                dup2(fdout[1], STDOUT_FILENO);
                dup2(fdout[1], STDERR_FILENO);
                close(fdout[0]);
            }
            if (files[2].size()) {
                dup2(fderr[1], STDERR_FILENO); 
                close(fderr[0]);
            }
            if (execvp(ptr[0], ptr) == -1) {
                //cerr << "\033[1;41mshell: command not found: " << ptr[0] << "\033[0m\n";
                cerr << "shell: command not found: " << ptr[0] << endl;
                delete tr;
                //        if (aliased_cmd != cmd) delete [] ptr;
                deleteAliasedCommands();
                exit(EXIT_FAILURE);
            }
        }
        else {
            processPtr->pid = child;
            if (shell_is_interactive) {
                //if (!j->pgid) {
                //    j->pgid = pid;
               // }
               // setpgid(pid, j->pgid);
            }
        }
        children.push_back(child);
    }
    close(fdout[1]);
    close(fderr[1]);
    close(fdin[1]);
    close(fdin[0]);
    if (files[1].size()) { //>
        if (files[4].size()) { //>> 
            int fdappend[2];
            pipe(fdappend);
            redirectOut(fdout, fdappend, 1, true, false);
            redirectOut(fdappend, writefd != STDOUT_FILENO ? pipefd : NULL, 4, writefd != STDOUT_FILENO, true);
        }
        else {
            redirectOut(fdout, writefd != STDOUT_FILENO ? pipefd : NULL, 1, writefd != STDOUT_FILENO, false);
        }
    }
    else if (files[4].size()) { //append
        redirectOut(fdout, writefd != STDOUT_FILENO ? pipefd: NULL, 4, writefd != STDOUT_FILENO, true);
    }
    if (files[2].size()) { //2>
        redirectOut(fderr, NULL, 2, false, false);
    }
    if ((files[3].size()) && writefd == STDOUT_FILENO && !files[1].size() && !files[2].size() && !files[4].size()) {
        if (files[5].size()) {
            int fdappend[2];
            pipe(fdappend);
            redirectOut(fdout, fdappend, 3, true, false);
            redirectOut(fdappend, NULL, 5, false, true);
        }
        else {
            redirectOut(fdout, NULL, 3, false, false);
        }
    }
    if ((files[5].size()) && writefd == STDOUT_FILENO && !files[1].size() && !files[2].size() && !files[4].size()) {
        redirectOut(fdout, NULL, 5, false, true);
    }
    while (ptr[i]) {
        cmds.push_back(ptr[i]);
        i++;
    }
    if (aliased_cmd != cmd) delete [] ptr;
    return 0;
}

