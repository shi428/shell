#ifndef COMMAND_H
#define COMMAND_H
#include <vector>
#include <string>
#include <iostream>
/*#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>*/
#include <ast.h>

using namespace std;
typedef struct command {
    ~command();
    void createArgs(vector <string> &cmd);
    void add_file_list(int id, vector <Node *>fileArgs);
    void printCommand(int spaces);

    char **cmd;
    vector <string> commands;
    vector <Node*> files[6]; //later nodes
}Command;
#endif

