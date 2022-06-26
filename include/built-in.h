#ifndef BUILT_IN_H
#define BUILT_IN_H
#include <unordered_map>
#include <fstream>
bool is_builtin(char *cmd);
bool check_builtin_syntax(char **cmd);
int run_builtin_command(char **cmd);
void delete_aliased_commands();
char **copy_command(char **cmd);
#endif
