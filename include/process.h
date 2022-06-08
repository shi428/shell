#ifndef PROCESS_H
#define PROCESS_H
#include <shell_util.h>
#include <vector>
#include <string>

using namespace std;

typedef struct process {
    process();
    ~process();
    void launch_process(pid_t pgid, int in_file, int out_file, int err_file, int foreground);
    void print_process_info();

    struct process *next;
    char **argv;
    pid_t pid;
    char completed;
    char stopped;
    int status;
}process;
#endif
