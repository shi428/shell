#include <shell.h>
void process::setup_out_err(vector <int> &pipeFds, int *outErrPipe, int *errFile, int *processOutFile) {
    if (this->files[3].size() || this->files[5].size()) {
        pipe(outErrPipe);
        *processOutFile = outErrPipe[1];
        *errFile = outErrPipe[1];
        pipeFds.push_back(outErrPipe[0]);
        pipeFds.push_back(outErrPipe[1]);
    }
}
