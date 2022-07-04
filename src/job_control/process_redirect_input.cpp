#include <shell.h>
void process::redirect_input(int redirectInFile, int *redirectInPipe) {
    if (fork() == 0) {
        my_cat(redirectInFile, redirectInPipe[1], this->files[0]);
        exit(0);
    }
    close(redirectInPipe[1]);
}
