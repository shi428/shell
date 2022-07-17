#include <process.h>
void process::setup_err_pipe(int *errPipe, int *errFile) {
    if (this->files[2].size()) {
        pipe(errPipe);
        *errFile = errPipe[1];
    }
}
