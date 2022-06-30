#ifndef PROCESS_H
#define PROCESS_H
#include <shell_util.h>
#include <vector>
#include <string>

using namespace std;

typedef struct process {
    process();
    ~process();
    void launch_process(AST *ast, pid_t pgid, int inFile, int outFile, int errFile, int foreground);
    void print_process_info();

    vector <string> files[6];
    struct process *next;
    char **argv;
    pid_t pid;
    char completed;
    char stopped;
    int status;
}process;
void delete_argv(char **argv);
#endif
