#ifndef SHELL_UTIL_H
#define SHELL_UTIL_H
#ifndef EXTERN
#define EXTERN extern
#endif

#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <pwd.h>

#include <ast.h>
#include <built-in.h>
#include <command.h>
#include <exec.h>
#include <expansion.h>
#include <misc.h>
#include <process.h>
#include <trie.h>

using namespace std;

EXTERN pid_t shell_pgid;
EXTERN struct termios shell_tmodes;
EXTERN int shell_terminal;
EXTERN int shell_is_interactive;
void init_shell();
void myTee(int inputfd, int outputfd, vector <string> &out_files, vector <string> & append_files);
void myCat();
#endif
