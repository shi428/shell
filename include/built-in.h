#ifndef BUILT_IN_H
#define BUILT_IN_H
#include <unordered_map>
#include <fstream>
int get_length(char **cmd);
bool is_builtin(char *cmd);
bool check_builtin_syntax(char **cmd);
int run_builtin_command(char **cmd);
void delete_aliased_commands();
char **copy_command(char **cmd);

//aliasing
void print_aliases();
void run_builtin_command_alias(char **cmd);

//cd
void run_builtin_command_cd(char **cmd);

//bg
void run_builtin_command_bg(char **cmd);

//fg
void run_builtin_command_fg(char **cmd);

//help menu
void run_builtin_command_help(char **cmd);

//printenv
void run_builtin_command_printenv(char **cmd);

//setenv
void run_builtin_command_setenv(char **cmd);

//source
void run_builtin_command_source(char **cmd);

//unsetenv
void run_builtin_command_unsetenv(char **cmd);
#endif
