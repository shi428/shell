#include <shell.h>

void run_builtin_command_bg(char **cmd) {
    job *j =Shell::find_first_stopped_or_bg_job();
    if (!j) {
        cerr << "bg: No such job" << endl;
    }
    Shell::mark_job_as_running(j);
    j->print_job_information();
    j->put_job_in_background(1);
}
