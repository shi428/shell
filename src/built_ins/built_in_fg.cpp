#include <shell.h>

void run_builtin_command_fg(char **cmd) {
    job *j =Shell::find_first_stopped_or_bg_job();
    if (!j) {
        cerr << "fg: No such job" << endl;
    }
    Shell::mark_job_as_running(j);
    j->put_job_in_foreground(1);
}
