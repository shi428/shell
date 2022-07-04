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
    void redirect_out(int processOutFile, int outFile, int *redirectOutPipe, pid_t pgid);
    void redirect_err(int errFile, int *redirectErrPipe, pid_t pgid);
    void redirect_outerr(int *redirectOutErrPipe, pid_t pgid);
    void redirect_input(int redirectInFile, int *redirectInPipe);
    void setup_input_pipe(int *inPipe, int *redirectInFile, int *inFile);
    void setup_output_pipe(int *myPipe, int *myPipe2, int *processOutFile, int *outFile);
    void setup_err_pipe(int *errPipe, int *errFile);
    void setup_out_err(int *outErrPipe, int *errFile, int *processOutFile);
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
