#include <command.h>
#include <ast.h>

extern int fdin[2];
Command::~Command() {
    int i = 0;
    if (cmd) {
    while (cmd[i]) {
        free(cmd[i]);
        i++;
    }
    delete [] cmd;
    }
}

void Command::add_file_list(int id, vector <Node *>fileArgs) {
    for (auto i: fileArgs) {
        files[id].push_back(i);
    }
}


