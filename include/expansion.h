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
string expandPrompt(char *prompt);
char **expandAlias(char **cmd);
vector <string> expandWildcard(string &wildcard, bool cwd);
string expandSubshell(string &subshell_command);
string tryExpandCommand(Node *node, string &command);
#endif
