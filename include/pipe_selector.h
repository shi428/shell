#ifndef PIPE_SELELECTOR_H
#define PIPE_SELELECTOR_H

int *pipe_select_output(int outFile, int *myPipe, int *myPipe2, int *outErrPipe);
int *pipe_select_input(int inFile, int *myPipe, int *inPipe);
#endif
