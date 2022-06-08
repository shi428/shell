#ifndef SHELL_H
#define SHELL_H

#include <shell_util.h>
#include <jobs.h>

struct Shell {
    Shell();
    ~Shell();
    inline static pid_t shell_pgid;
    inline static struct termios shell_tmodes;
    inline static int shell_terminal;
    inline static int shell_is_interactive;

    struct list_of_jobs {
        list_of_jobs() {
            first_job = NULL;
            last_job = NULL;
        }

        ~list_of_jobs() {
            job *it = first_job;
            while (it) {
                job *next = it->next;
                delete it;
                it = next;
            }
        }
        job *first_job;
        job *last_job;
        int n_jobs = 0;
    };

    inline static list_of_jobs *jobs;

    static void init_shell();
    static void destroy_shell();
    static void sigint_handler(int );
    static void sigchild_handler(int );
    static void print_prompt();
    static void insert_job(job *j);
    static void delete_job(job *j);
    static void print_jobs();
};

#endif
