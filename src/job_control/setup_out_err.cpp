#include <shell.h>
void process::setup_out_err(int *outErrPipe, int *errFile, int *processOutFile) {
    if (this->files[3].size() || this->files[5].size()) {
        pipe(outErrPipe);
        *processOutFile = outErrPipe[1];
        *errFile = outErrPipe[1];
    }
}
