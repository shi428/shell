#ifndef SHELL_H
#define SHELL_H

#include <shell_util.h>
#include <jobs.h>
#include <unordered_map>

//colors
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define HRED "\e[0;91m"
#define HGRN "\e[0;92m"
#define RST "\e[0m"
struct Shell {
    Shell();
    ~Shell();
    inline static pid_t shellPgid;
    inline static struct termios shellTmodes;
    inline static int shellTerminal;
    inline static int shellIsInteractive;
    inline static int exitStatus;
    inline static int lastJobExitStatus;
    inline static char *shellPath;
    inline static job *currentJob;
    inline static unordered_map<string,string> users;

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
    };

    inline static list_of_jobs *jobs;

    static void init_shell();
    static void destroy_shell();
    static void sigint_handler(int );
    static void check_zombie( );
    static void print_prompt();
    static void insert_job_after(job *j, job *it);
    static void insert_job(job *j);
    static void delete_job(job *j);
    static void print_jobs();
    static void delete_jobs();
    static job *find_first_stopped_or_bg_job();
    static void mark_job_as_running(job *j);
    static void get_users();
};

//helpers
#endif
