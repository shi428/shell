#include <shell.h>

//setup input redirection in between pipes
void process::setup_input_pipe(int *inPipe, int *redirectInFile, int *inFile) {
    if (this->files[0].size()) {
        pipe(inPipe);
        *redirectInFile = *inFile;
        *inFile = inPipe[0];
        if (this->files[0].size() == 1 && *redirectInFile == STDIN_FILENO /*not coming from a pipe*/) {
            *inFile = open(this->files[0][0].c_str(), O_RDONLY);
        }
    }
}