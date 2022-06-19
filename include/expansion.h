#ifndef EXPANSION_H
#define EXPANSION_H

#include <exec.h>
#include <built-in.h>
#include <dirent.h>
#include <regex.h>
#include <vector>
#include <algorithm>
/*vector <Token> expand_subshell(vector <Token> &tokens);
vector <Token> expand_env(vector <Token> &tokens);
vector <Token> expand_tilde(vector <Token> &tokens);*/

#define REGULAR 0
#define COMMAND_SUBST 1
string expandPrompt(char *prompt);
char **expandAlias(char **cmd);
vector <string> expandWildcard(string &wildcard, bool cwd);
string expandSubshell(string &subshell_command);
#endif
