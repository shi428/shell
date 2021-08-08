#ifndef EXPANSION_H
#define EXPANSION_H

#include <parser.h>
#include <tokenizer.h>
#include <exec.h>
#include <built-in.h>
vector <Token> expand_subshell(vector <Token> &tokens);
string expandPrompt(char *prompt);
#endif
