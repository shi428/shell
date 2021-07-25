#ifndef EXEC_H
#define EXEC_H

#include "command.h"
#include "parser.h"
#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>

void exec(struct passwd *p, Node *node);
int exec_node(struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd);
int exec_pipe(struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd);
#endif
