#include <shell.h>

void Shell::init_shell() {
    /* See if we are running interactively.  */
    //first_job = NULL;
    jobs = new list_of_jobs;
    shell_terminal = STDIN_FILENO;
    shell_is_interactive = isatty (shell_terminal);

    if (shell_is_interactive)
    {
        /* Loop until we are in the foreground.  */
        while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
            kill (- shell_pgid, SIGTTIN);

        /* Ignore interactive and job-control signals.  */
        signal (SIGINT, sigint_handler);
        signal (SIGQUIT, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        signal (SIGCHLD, sigchild_handler);

        /* Put ourselves in our own process group.  */
        shell_pgid = getpid ();
        if (setpgid (shell_pgid, shell_pgid) < 0)
        {
            perror ("Couldn't put the shell in its own process group");
            exit (1);
        }

        /* Grab control of the terminal.  */
        tcsetpgrp (shell_terminal, shell_pgid);

        /* Save default terminal attributes for shell.  */
        tcgetattr (shell_terminal, &shell_tmodes);
    }
}
void Shell::sigint_handler(int signum) {
    //signal(SIGINT, sigint_handler);
    cout << endl;
    if (isatty(0)) {
        print_prompt();
    }
    fflush(stdout);
}

void Shell::insert_job(job *j) {
    if (jobs->last_job == NULL) {
        jobs->last_job = j;
        jobs->first_job = j;
    }
    else {
        jobs->last_job->next = j;
        jobs->last_job = jobs->last_job->next;
    }
    j->job_id = ++jobs->n_jobs;
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
        jobs->n_jobs--;
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
    jobs->n_jobs--;
}

void Shell::sigchild_handler(int signum) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        mark_process_status(pid, status);
        job *j = find_job_by_pid(pid);
        if (j->job_is_completed() && !j->foreground) {
            printf("\n");
            j->print_job_information();
            delete_job(j);
            if (isatty(0)) {
                print_prompt();
            }
            fflush(stdout);
            return ;
        }/*for (unsigned int i = 0; i < bPids.size(); i++) {
           if (bPids[i].first == pid && pos.size()) {
           background_process = pid;
           cout << endl;
           cout << pos[i] <<". [" << pid << "] ";
           for (auto j: bPids[i].second) {
           cout << j << " ";
           }
           cout << "has exited";
           cout << endl;
           bPids.erase(bPids.begin() + i);
           pos.erase(pos.begin() + i);
           i--;
           if (isatty(0)) {
           print_prompt();
           }
           fflush(stdout);
           }
           }*/
    }
}

void Shell::print_prompt() {
    if (isEnviron((char *)"PROMPT")) {
        char *prompt = getenv("PROMPT");
        cout << expandPrompt(prompt);
    }
    else {
        cout << "shell>";
    }
    fflush(stdout);
}

void Shell::print_jobs() {
    job *jlast = NULL;
    job *jnext = NULL;
    update_status();
    printf("printing jobs\n");
    for (job *j = jobs->first_job; j; j = jnext) {
        jnext = j->next;
        j->print_job_information();
        if (j->job_is_completed()) {
            delete_job(j);
        }
    }
}
