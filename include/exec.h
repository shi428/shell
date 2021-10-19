#ifndef EXEC_H
#define EXEC_H

//#include "command.h"
#include <parser.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <process.h>

int exec(Tree *tr, struct passwd *p, Node *node, vector <pair<pid_t, vector <string>>> &bPids, vector <int> &pos);
int exec_node(Tree *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds);
int exec_pipe(Tree *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds);
int exec_and(Tree *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds);
int exec_or(Tree *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds);
#endif
