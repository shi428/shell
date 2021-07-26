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

int exec(struct passwd *p, Node *node, vector <pair<pid_t, vector <string>>> &bPids);
int exec_node(struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds);
int exec_pipe(struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds);
#endif
