#ifndef EXPANSION_H
#define EXPANSION_H

#include <parser.h>
#include <tokenizer.h>
#include <exec.h>
#include <built-in.h>
vector <Token> expand_subshell(vector <Token> &tokens);
vector <Token> expand_env(vector <Token> &tokens);
string expandPrompt(char *prompt);
char **expandAlias(char **cmd);
#endif
