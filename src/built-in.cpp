#include <built-in.h>
#include <parser.h>
#include <tokenizer.h>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <exec.h>

const char *built_in[4] = {"setenv", "unsetenv", "source", "cd"};

extern vector <pair<pid_t, vector <string>>> bPids;
extern vector <int> pos;
extern void printPrompt();
extern void sigchild_handler(int signum);
void sigchild_handler2(int signum) {
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        for (unsigned int i = 0; i < bPids.size(); i++) {
            if (bPids[i].first == pid && pos.size()) {
                cout << endl;
                cout << pos[i] <<". [" << pid << "] ";
                for (auto i: bPids[i].second) {
                    cout << i << " ";
                }
                cout << "has exited";
                cout << endl;
                bPids.erase(bPids.begin() + i);
                pos.erase(pos.begin() + i);
                i--;
                if (isatty(0)) {
                    printPrompt();
                }
                fflush(stdout);
            }
        }
    }
}
bool isBuiltIn(char *cmd) {
    for (int i = 0; i < 4; i++) {
        if (!strcmp(cmd, built_in[i])) return true;
    }
    return false;
}

bool checkSyntax(char **cmd) {
    int args = 0;
    while (cmd[args]) {
        args++;
    }
    if (!strcmp(cmd[0], "setenv")) {
        return args == 3;
    }
    if (!strcmp(cmd[0], "unsetenv")) {
        return args == 2;
    }
    if (!strcmp(cmd[0], "cd")) {
        return args <= 2;
    }
    if (!strcmp(cmd[0], "source")) {
        return args == 2;
    }
    return true;
}

int runBuiltInCommand(char **cmd, struct passwd *p) {
    if (checkSyntax(cmd)) {
        if (!strcmp(cmd[0], "cd")){
            if (cmd[1] == NULL) {
                string homedir = "/home/" + string(p->pw_name);
                chdir(homedir.c_str());
            }
            else {
                if (chdir(cmd[1]) == -1) {
                    cerr << "cd: no such file or directory: " << cmd[1] << endl;
                }
            }
        }
        if (!strcmp(cmd[0], "setenv")) {
            if (setenv(cmd[1], cmd[2], 1) == -1) {
                cerr << "setenv: failed to set environment variable: " << cmd[2] << endl;
            }
        }
        if (!strcmp(cmd[0], "unsetenv")) {
            if (unsetenv(cmd[1]) == -1) {
                cerr << "unsetenv: failed to unset environment variable: " << cmd[2] << endl;
            }
        }
        if (!strcmp(cmd[0], "source")) {
            string line;
            ifstream fin(cmd[1]);
            if (!fin.good()) {
                cerr << "source: no such file or directory: " << cmd[1] << endl;
                return -1;
            }
            signal(SIGCHLD, sigchild_handler);
            while (getline(fin, line)) {
                if (isatty(0)) {
                    //                    printPrompt();
                }
                vector <Token> tokens = genTokens(line);
                for (unsigned int i = 0; i < tokens.size(); i++) {
                    if (tokens[i].type == SUBSHELL) {
                        //  cout << "subshell" << endl;
                        //  cout << tokens[i].lexeme.c_str() << endl;
                        pid_t child;
                        const char *shell[2] = {"/proc/self/exe", NULL};
                        int pipefd[2];
                        //int og_stdin = dup(STDIN_FILENO);
                        int pipefd2[2];
                        pipe(pipefd);
                        pipe(pipefd2);
                        write(pipefd[1], tokens[i].lexeme.c_str(), tokens[i].lexeme.length());
                        tokens.erase(tokens.begin() + i);
                        i--;
                        if ((child = fork()) == 0) {
                            dup2(pipefd[0], STDIN_FILENO);
                            dup2(pipefd2[1], STDOUT_FILENO);
                            close(pipefd2[0]);
                            close(pipefd[1]);
                            execvp(shell[0], (char **)shell);
                            exit(EXIT_SUCCESS);
                        }
                        // waitpid(child, NULL, 0);
                        close(pipefd[0]);
                        close(pipefd[1]);
                        // dup2(pipefd2[0], STDIN_FILENO);
                        close(pipefd2[1]);
                        char buf[4096];
                        //FILE *input = fdopen(STDIN_FILENO,"r");
                        //size_t size;
                        // getline(&line_ptr, &size, input);
                        string line2;
                        ssize_t n_bytes;
                        while ((n_bytes = read(pipefd2[0], buf, 4095)) != 0) {
                            buf[n_bytes] = '\0';
                            char *temp = buf;
                            while (*temp != '\0'){
                                if (*temp == '\n') {
                                    line2 += ' ';
                                }
                                else {
                                    line2 += *temp;
                                }
                                temp++;
                            }
                        }
                        close(pipefd2[0]);
                        //dup2(og_stdin, STDIN_FILENO);
                        //cout << line << endl;
                        // free(line_ptr);
                        // fclose(input);
                        vector <Token> tokens2 = genTokens(line2);
                        for (unsigned int j = 0; j < tokens2.size(); j++) {
                            if (tokens2[j].type == COMMAND) {
                                if (tokens[i].type == COMMAND) {
                                    tokens[i].lexeme += tokens2[j].lexeme;
                                }
                                /*  else {
                                    tokens.insert(tokens.begin() + i - 1, tokens2[j]);
                                    i++;
                                    }*/
                            }
                            else {
                                tokens.insert(tokens.begin() + i - 1, tokens2[j]);
                                i++;
                            }
                        }
                        if (tokens[i+1].type == COMMAND && tokens[i].type == COMMAND) {
                            tokens[i].lexeme += tokens[i+1].lexeme;
                            tokens.erase(tokens.begin() + i + 1);
                        }
                    }
                }

                Tree *parseTree = newTree(tokens);
                //tokens.~vector <Token>();
                if (parseTree->root) {
                    //        parseTree->root->traverse(0);
                    if (exec(p, parseTree->root, bPids, pos)) {
                        delete parseTree;
                        break;
                    }
                }
                delete parseTree;
            }
            fin.close();
        }
        return 0;
    }
    cerr << cmd[0]<< ": wrong number of arguments" << endl;
    return -1;
}
