#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
vector <char *> args(string &input) {
    stringstream s(input);
    vector <char *> ret;
    int spaces = count(input.begin(), input.end(), ' ');
    for (int i = 0; i < spaces; i++) {
        string arg;
        s >> arg;
        ret.push_back((char *)arg.c_str());
    }
    return ret;
}

void handle_command(vector <char *> &cmd) {
    char **ptr = &args[0];
    execv(args[0], ptr);
}

int main(int argc, char *argv[]) {
    string input;
    cin >> input;
    vector <char *> cmd = args(input);
    handle_command(cmd);
    return EXIT_SUCCESS;
}
