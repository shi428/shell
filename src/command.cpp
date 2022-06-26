#include <shell.h>

extern int fdin[2];
extern int yylex_destroy();
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

void Command::createArgs(vector <string> &cmd) {
    vector <string> args;
    for (unsigned int i = 0; i < cmd.size(); i++) {
        if (detect_wildcard(cmd[i])) {
            bool pwd = !(cmd[i].find('/') == 0);
            string wildcard = !pwd? cmd[i] : string(getenv("PWD")) +"/"+ cmd[i];
            vector <string> strings = expand_wildcard(wildcard, pwd); 
            if (strings.size() == 0) {
                args.push_back(wildcard);
            }
            for  (auto i: strings) {
                args.push_back(i);
            }
        }
        else {
            args.push_back(cmd[i]);
        }
    }
    this->cmd = new char*[args.size() + 1];
    for (unsigned int i = 0; i < args.size(); i++) {
        this->cmd[i] = strdup(args[i].c_str());
    }
    this->cmd[args.size()] = NULL;
}

void Command::add_file_list(int id, vector <Node *>file_args) {
    for (auto i: file_args) {
        files[id].push_back(i);
    }
}


