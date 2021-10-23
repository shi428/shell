#ifndef EXEC_H
#define EXEC_H

//#include "command.h"
#include <ast.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <process.h>

int exec(AST *tr, struct passwd *p, Node *node, vector <pair<pid_t, vector <string>>> &bPids, vector <int> &pos);
int exec_node(AST *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds);
int exec_pipe(AST *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds);
int exec_and(AST *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds);
int exec_or(AST *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds);
#endif
