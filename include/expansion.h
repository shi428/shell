#ifndef EXPANSION_H
#define EXPANSION_H

#include <parser.h>
#include <tokenizer.h>
#include <exec.h>
#include <built-in.h>
#include <dirent.h>
vector <Token> expand_subshell(vector <Token> &tokens);
vector <Token> expand_env(vector <Token> &tokens);
vector <Token> expand_tilde(vector <Token> &tokens);
string expandPrompt(char *prompt);
char **expandAlias(char **cmd);
vector <string> expandWildcard(string &wildcard, bool cwd);
#endif
