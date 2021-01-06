#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
string item;
vector <char *> args(string &input) {
    //stringstream ss(input);
    vector <char *> ret;
   // while (getline(ss, item, ' ')) {
   //     ret.push_back((char *)item.c_str());
   // }
    int begin = 0;
    for (unsigned i = 0; i < input.length(); i++) {
        if (input[i] == ' ') {
            char *temp = new char[i - begin];
            strcpy(temp, input.substr(begin, i - begin).c_str());
            ret.push_back(temp);
            begin = i + 1;
        }
    }
    char *temp = new char[input.length() - begin + 1];
    strcpy(temp, input.substr(begin).c_str());
    ret.push_back(temp);
    return ret;
}

void handle_command(vector <char *> &cmd) {
    int status = 0;
    char **ptr = &cmd[0];
    pid_t child;
    child = fork();
    cout << child << endl;
    if (child == 0) {
        if (execvp(cmd[0], ptr) == -1) {
            cerr << "\033[1;41mshell: command not found: " << cmd[0] << "\033[0m\n";
        }
    }
    else {
        waitpid(child, &status, 0);
    }
}

void freeStrings(vector <char *> &cmd) {
    for (auto i: cmd) {
        delete [] i;
    }
}

int main(int argc, char *argv[]) {
    string input;
    while (1) {
    getline(cin, input);
    if (input.compare("exit") == 0) {
        break;
    }
    vector <char *> cmd = args(input);
    handle_command(cmd);
    freeStrings(cmd);
    }
    return EXIT_SUCCESS;
}
