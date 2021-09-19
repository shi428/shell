#ifndef COMMAND_H
#define COMMAND_H
#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <parser.h>

using namespace std;
extern void myCat(int inputfd, int outputfd, vector <string> &files, bool readInput);
extern vector <int> openFiles(vector <string> &files);
typedef struct command {
    ~command();
    int execute(Tree *tr, struct passwd *p, vector <pid_t> &children, int *pipefd, int readfd, int writefd, vector <string> &cmds);
    int redirectIn(int *pipefd, vector <pid_t> &children, int readfd, int writefd, bool pipeflag);
 //   int redirectOut(int *pipefd, vector <pid_t> &children, int readfd, int writefd, int i, bool pipe);
    int redirectOut(int *pipefd, int *pipefd2, int i, bool pipe, bool _append);
    void createArgs(vector <string> &cmd);
    void addFile(int id, string &filename);
    void printCommand(int spaces);

    char **cmd;
    vector <string> commands;
    vector <string> files[6];
}Command;
#endif

