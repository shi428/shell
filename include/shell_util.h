#ifndef SHELL_UTIL_H
#define SHELL_UTIL_H

#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <pwd.h>
#include <libgen.h>
#include <ftw.h>

#include <ast.h>
#include <built-in.h>
#include <command.h>
#include <exec.h>
#include <expansion.h>
#include <misc.h>
#include <process.h>
#include <trie.h>

using namespace std;

void my_tee(int inputFd, int outputFd, vector <string> &outFiles, vector <string> & appendFiles);
void my_cat(int inputFd, int outputFd, vector <string> &files);
#endif
