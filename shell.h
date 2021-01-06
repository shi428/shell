#ifndef SHELL_H
#define SHELL_H

#include <bits/stdc++.h>
using namespace std;
vector <char *> parse_command(string &input);
int handle_command(struct passwd *p, vector <char *> &cmd);
void freeStrings(vector <char *> &cmd);
void printPrompt(struct passwd *p);
#endif
