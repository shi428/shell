#include <process.h>
void process::setup_err_pipe(vector <int> &pipeFds, int *errPipe, int *errFile) {
    if (this->files[2].size()) {
        pipe(errPipe);
        *errFile = errPipe[1];
        pipeFds.push_back(errPipe[0]);
        pipeFds.push_back(errPipe[1]);
    }
}
