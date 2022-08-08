#ifndef PROCESS_H
#define PROCESS_H
#include <shell_util.h>
#include <vector>
#include <string>

using namespace std;

typedef struct process {
    process();
    ~process();
    void launch_process(AST *ast, vector <int> &pipeFds, pid_t pgid, int inFile, int outFile, int errFile, int foreground);
    void redirect_out(vector <int> &pipeFds, int processOutFile, int inFile, int outFile, int *redirectOutPipe, pid_t pgid);
    void redirect_err(vector <int> &pipeFds, int errFile, int *redirectErrPipe, pid_t pgid);
    void redirect_outerr(vector <int> &pipeFds, int *redirectOutErrPipe, pid_t pgid);
    void redirect_input(vector <int> &pipeFds, int redirectInFile, int *redirectInPipe, pid_t pgid);
    void setup_input_pipe(vector <int> &pipeFds, int *inPipe, int *redirectInFile, int *inFile);
    void setup_output_pipe(vector <int> &pipeFds, int *myPipe, int *myPipe2, int *processOutFile, int *outFile);
    void setup_err_pipe(vector <int> &pipeFds, int *errPipe, int *errFile);
    void setup_out_err(vector <int> &pipeFds, int *outErrPipe, int *errFile, int *processOutFile);
    void print_process_info();

    vector <string> files[6];
    struct process *next;
    char **argv;
    pid_t pid;
    pid_t redirectPids[4]; //0 input redirection 1 output 2 err 3 outErr
    int redirectStatuses[4]; //0 input redirection 1 output 2 err 3 outErr
    char completed;
    char stopped;
    int status;
    int foreground;
}process;
void delete_argv(char **argv);
#endif
