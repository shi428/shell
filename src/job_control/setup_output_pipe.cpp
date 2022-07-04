#include <shell.h>

//setup input redirection in between pipes
void process::setup_output_pipe(int *myPipe, int *myPipe2, int *processOutFile, int *outFile) {
    if (this->files[1].size() || this->files[4].size()) {
        pipe(myPipe);
        *processOutFile = myPipe[1];
        *outFile = myPipe2[1];
    }
}
