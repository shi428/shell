#ifndef BUILT_IN_H
#define BUILT_IN_H
#include <unordered_map>
#include <fstream>
bool isBuiltIn(char *cmd);
bool checkSyntax(char **cmd);
int runBuiltInCommand(char **cmd);
void deleteAliasedCommands();
char **copyCommand(char **cmd);
#endif
