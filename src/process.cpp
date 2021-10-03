#include <process.h>

process::process() {
    argv = NULL;
    completed = 0;
    stopped = 0;
}

process::~process() {
    int i = 0;
    while (argv[i]) {
        delete [] argv[i];
        i++;
    }
    delete [] argv;
}

