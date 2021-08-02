#include <parser.h>
#include <tokenizer.h>
#include <exec.h>
#include <built-in.h>
#include <stdio.h>
vector <pair<pid_t, vector <string>>> bPids;
vector <int> pos;
void printPrompt() {
    cout << "shell>";
}

void sigint_handler(int signum) {
    //signal(SIGINT, sigint_handler);
    cout << endl;
    if (isatty(0)) {
        printPrompt();
    }
    fflush(stdout);
}

void sigchild_handler(int signum) {
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

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchild_handler);
    string line;
    struct passwd *p = getpwuid(getuid());
    const char *source[3] = {"source", ".shellrc", NULL};
    if (isatty(0)) {
        runBuiltInCommand((char **)source, p);
    }
    while (1) {
        if (isatty(0)) {
            printPrompt();
        }
        if (!getline(cin, line)) break;
        vector <Token> tokens = genTokens(line);
       // for (auto i: tokens) i.printToken();
        for (unsigned int i = 0; i < tokens.size(); i++) {
            if (tokens[i].type == SUBSHELL) {
              //  cout << "subshell" << endl;
              //  cout << tokens[i].lexeme.c_str() << endl;
                pid_t child;
                const char *shell[2] = {"./shell", NULL};
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
                    //cout << buf;
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
        if (isatty(0)) {
            //for (auto i: tokens) i.printToken();
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
    return EXIT_SUCCESS;
}
