#ifndef BUILT_IN_H
#define BUILT_IN_H
bool isBuiltIn(char *cmd);
bool checkSyntax(char **cmd);
int runBuiltInCommand(char **cmd, struct passwd *p);
void deleteAliasedCommands();
char **copyCommand(char **cmd);
#endif
