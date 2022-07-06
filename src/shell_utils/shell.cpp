#include <shell.h>
#include <iomanip>

void Shell::init_shell() {
    /* See if we are running interactively.  */
    //first_job = NULL;
    jobs = new list_of_jobs;
    shellTerminal = STDIN_FILENO;
    shellIsInteractive = isatty (shellTerminal);
    exitStatus = 0;
    shellPath = realpath("/proc/self/exe", NULL);

    string shellrc_loc = string(getenv("HOME")) + string("/.shellrc");
    const char *source[3] = {"source", shellrc_loc.c_str(), NULL};
    const char *set_shell[4] = {"setenv", "SHELL", shellPath, NULL};
    char *temp = strdup(shellPath);
    string namedPipeDir = string(dirname(temp)) + "/tmp";

    mkdir(namedPipeDir.c_str(), 0777);
    free(temp);

    //get user information
    get_users();

    //set shell env var
    //run_builtin_command_setenv((char **)set_shell);
    if (shellIsInteractive)
    {
        /* Loop until we are in the foreground.  */
        while (tcgetpgrp (shellTerminal) != (shellPgid = getpgrp ()))
            kill (- shellPgid, SIGTTIN);

        /* Ignore interactive and job-control signals.  */
        signal (SIGINT, sigint_handler);
        signal (SIGQUIT, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        signal (SIGCHLD, SIG_DFL);

        /* Put ourselves in our own process group.  */
        shellPgid = getpid ();
        if (setpgid (shellPgid, shellPgid) < 0)
        {
            perror ("Couldn't put the shell in its own process group");
            exit (1);
        }

        /* Grab control of the terminal.  */
        tcsetpgrp (shellTerminal, shellPgid);

        /* Save default terminal attributes for shell.  */
        tcgetattr (shellTerminal, &shellTmodes);
    }

    //source shellrc
    if (shellIsInteractive) {
        run_builtin_command_source((char **)source);
    }
}

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    int rv = remove(fpath);
    return rv;
}

int rmrf(const char *fpath) {
    return nftw(fpath, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

void Shell::destroy_shell() {
    char *temp = strdup(shellPath);
    string namedPipeDir = string(dirname(temp)) + "/tmp";
    free(shellPath);
    free(temp);
    rmrf(namedPipeDir.c_str());
    delete jobs;
}

void Shell::sigint_handler(int signum) {
    //signal(SIGINT, sigint_handler);
    cout << endl;
    if (isatty(0)) {
        print_prompt();
    }
    fflush(stdout);
}

void Shell::insert_job_after(job *j, job *it) {
    //head case
    job *next_job = NULL;
    if (it == NULL) {
        j->next = jobs->first_job;
        jobs->first_job = j;
        return ;
    }
    next_job = it->next;
    it->next = j;
    j->next = next_job;
}

void Shell::insert_job(job *j) {
    int i = 1;
    job *jlast = NULL;
    job *jnext = NULL;
    if (jobs->last_job == NULL) {
        jobs->last_job = j;
        jobs->first_job = j;
        j->jobId = 1;
        return ;
    }
    for (job *it = jobs->first_job; it; it = jnext) {
        jnext = it->next;
        if (it->jobId > i) {
            j->jobId = i;
            insert_job_after(j, jlast);
            return ;
        }
        jlast = it;
        i++;
    }

    //last job
    jobs->last_job->next = j;
    jobs->last_job = jobs->last_job->next;
    j->jobId = i;
}

void Shell::delete_job(job *j) {
    if (jobs->first_job == NULL) return ;
    if (j == jobs->first_job ) {
        if (jobs->first_job->next == NULL) {
            jobs->first_job = NULL;
            jobs->last_job = NULL;
            delete j;
        }
        else {
            jobs->first_job = j->next;
            delete j;
        }
        return ;
    }
    job *it = jobs->first_job;
    while (it && it->next != j) {
        it = it->next;
    }
    if (j == jobs->last_job) {
        jobs->last_job = it;
    }
    it->next = j->next;
    delete j;
}

void Shell::check_zombie() {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        mark_process_status(pid, status);
        job *j = find_job_by_pid(pid);
        if (j && j->job_is_completed() && !j->foreground) {
            Shell::lastJobExitStatus = WEXITSTATUS(status);
            j->print_job_information();
            delete_job(j);
            fflush(stdout);
            return ;
        }
    }
}

void Shell::print_prompt() {
    if (is_environ((char *)"PROMPT")) {
        char *prompt = getenv("PROMPT");
        if (!Shell::lastJobExitStatus) {
            cout << expand_prompt(prompt);
            cout << HGRN << "[ " <<  setw(3) << 0 << " ] $ " << RST;;
        }
        else {
            cout << expand_prompt(prompt);
            cout << HRED << "[ " <<  setw(3) << Shell::lastJobExitStatus << " ] $ " << RST;
        }
    }
    else {
        if (!Shell::lastJobExitStatus) {
            cout << HGRN << "[ " <<  setw(3) << 0 << " ] $ " << RST;;
        }
        else {
            cout << HRED << "[ " <<  setw(3) << Shell::lastJobExitStatus << " ] $ " << RST;
        }
    }
    fflush(stdout);
}

void Shell::print_jobs() {
    job *jnext = NULL;
    printf("printing jobs\n");
    for (job *j = jobs->first_job; j; j = jnext) {
        jnext = j->next;
        j->print_job_information();
        if (j->job_is_completed()) {
            delete_job(j);
        }
    }
}

job *Shell::find_first_stopped_or_bg_job() {
    for (job *j = Shell::jobs->first_job; j; j = j->next) {
        if (j->job_is_stopped() || j->foreground == 0) {
            return j;
        }
    }
    return NULL;
}

void Shell::mark_job_as_running(job *j) {
    for (process *p = j->firstProcess; p; p = p->next) {
        p->stopped = 0;
    }
}

void Shell::get_users() {
    while (true) {
        errno = 0;
        struct passwd *entry = getpwent();
        if (!entry) {
            if (errno) {
                cerr << "error reading password database\n";
                return;
            }
            break;
        }
        users[string(entry->pw_name)] = string(entry->pw_dir);
    }
    endpwent();
}
