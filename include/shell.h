#ifndef SHELL_H
#define SHELL_H
#ifndef EXTERN
#define EXTERN extern
#endif

#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
using namespace std;

EXTERN pid_t shell_pgid;
EXTERN struct termios shell_tmodes;
EXTERN int shell_terminal;
EXTERN int shell_is_interactive;
void init_shell();
#endif
