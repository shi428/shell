#include <shell.h>

unordered_map<string, pair<char **, int>> aliases;
vector <char **> alias_ptrs;
void printAliases() {
    cout << "aliases" << endl;
    for (auto i: aliases) {
        cout << i.first << "=";
        for (int j = 0; j < i.second.second; j++) {
            cout << i.second.first[j] << " ";
        }
        cout << endl;
    }
}

void run_builtin_command_alias(char **cmd) {
    if (cmd[1] == NULL) {
        printAliases();
    }
    else {
        char **ptr = copy_command(&cmd[1]);
        aliases[string(ptr[0])] = pair<char **, int>(&ptr[1], get_length(&cmd[2]));
        alias_ptrs.push_back(ptr);
    }
}

void delete_aliased_commands() {
    for (auto i: alias_ptrs) {
        char **ptr = i;
        int j = 0;
        while (ptr[j]) {
            delete [] ptr[j];
            j++;
        }
        delete [] ptr;
    }
}
