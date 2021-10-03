#ifndef PROCESS_H
#define PROCESS_H
#include <shell.h>
#include <vector>
#include <string>

using namespace std;

struct process {
    process();
    ~process();
    void launch_process(pid_t pgid, vector <string> files, int foreground);
    void print_process();
    char **argv;
    pid_t pid;
    char completed;
    char stopped;
    int status;
};
#endif
