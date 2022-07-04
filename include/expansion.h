#ifndef EXPANSION_H
#define EXPANSION_H

#include <built-in.h>
#include <dirent.h>
#include <regex.h>
#include <vector>
#include <algorithm>
/*vector <Token> expand_env(vector <Token> &tokens);
vector <Token> expand_tilde(vector <Token> &tokens);*/

enum expansion_t {
    REGULAR=0,
    COMMAND_SUBST=1,
    READ_PROCESS_SUBST=2,
    WRITE_PROCESS_SUBST=3,
};
//#define REGULAR 0
//#define COMMAND_SUBST 1
string expand_prompt(char *prompt);
char **expand_alias(char **cmd);
vector <string> expand_wildcard(string &wildcard, bool cwd);
string expand_command_subst(string &subshellCommand);
string expand_process_subst(string &subshellCommand, bool readWrite);
void expand_left_process_subst(int *writeCommandPipe, string &subshellCommand, string &namedPipe, const char **shell);
void expand_right_process_subst(int *writeCommandPipe, string &subshellCommand, string &namedPipe, const char **shell);
string try_expand_command(Node *node, string &command);
string expand_arg(Node *argNode, string &command);
#endif
