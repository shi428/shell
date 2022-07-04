#include <shell.h>
#include <yacc.yy.hpp>
#include <lex.yy.hpp>

extern unordered_map<string, pair<char **, int>> aliases;
char **expand_alias(char **cmd) {
    if (aliases.find(string(cmd[0])) == aliases.end()) {
        return cmd;
    }
    if (!strcmp(aliases[string(cmd[0])].first[0], cmd[0])) {
        char **aliased_cmd = aliases[string(cmd[0])].first;
        int aliased_length = get_length(aliased_cmd);
        int length = get_length(&cmd[1]);
        char **ptr = new char *[aliased_length + 1 + length];
        for (int i = 0; i < aliased_length; i++) {
            ptr[i] = strdup(aliased_cmd[i]);
        }
        for (int i = aliased_length; i < aliased_length + length; i++) {
            ptr[i] = strdup(cmd[i - aliased_length + 1]);
        }
        ptr[aliased_length + length] = NULL;
        return ptr;
    }
    char **aliased_cmd = expand_alias(aliases[string(cmd[0])].first);
    int aliased_length = get_length(aliased_cmd);
    int length = get_length(&cmd[1]);
    char **ptr = new char *[aliased_length + length + 1];
    for (int i = 0; i < aliased_length; i++) {
        ptr[i] = strdup(aliased_cmd[i]);
    }
    for (int i = aliased_length; i < aliased_length + length; i++) {
        ptr[i] = strdup(cmd[i - aliased_length + 1]);
    }
    ptr[aliased_length + length] = NULL;
    if (aliases[string(cmd[0])].first != aliased_cmd) {
        delete []aliased_cmd;
    }
    return ptr;
}
