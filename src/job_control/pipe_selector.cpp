#include <pipe_selector.h>

//myPipe for output redirection
//myPipe2 if piping only
#include <stdio.h>
int *pipe_select_output(int outFile, int *myPipe, int *myPipe2, int *outErrPipe) {
    if (outFile == myPipe[1]) {
       printf("output redir\n");
        return myPipe;
    }
    if (outFile == myPipe2[1]) {
        return myPipe2;
    }
    if (outFile == outErrPipe[1]) {
        return myPipe2;
    }
    return (int *)0;
}

//myPipe for piping only
//inPipe for input redirection
int *pipe_select_input(int inFile, int *myPipe, int *inPipe) {
    if (inFile == myPipe[0]) {
        return myPipe;
    }
    if (inFile == inPipe[0]) {
        return inPipe;
    }
    return (int *)0;
}
