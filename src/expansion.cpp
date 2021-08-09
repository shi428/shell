#include <expansion.h>

vector <Token> expand_subshell(vector <Token> &tokens) {
    for (unsigned int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == SUBSHELL) {
        //      cout << "subshell" << endl;
            //cout << tokens[i].lexeme.c_str() << endl;
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
            //line2 += '\n';
            close(pipefd2[0]);
            //dup2(og_stdin, STDIN_FILENO);
            //cout << line2<< endl;
            // free(line_ptr);
            // fclose(input);
            vector <Token> tokens2 = genTokens(line2, true);
            // cout << tokens2.size() << endl;
            for (unsigned int j = 0; j < tokens2.size(); j++) {
                //   cout << tokens2[j].lexeme << endl;
                if (tokens2[j].type == COMMAND) {
                    if (tokens[i].type == COMMAND) {
                        tokens[i].lexeme += tokens2[j].lexeme;
                    }
                    else {
                        //          cout << "insert" << endl;
                        tokens.insert(tokens.begin() + i + 1, tokens2[j]);
                        i++;
                    }
                }
                else {
            //        cout << "insert" << endl;
                    tokens.insert(tokens.begin() + i + 1, tokens2[j]);
                    i++;
                }
            }
            if (i + 1 < tokens.size() && tokens[i+1].type == COMMAND && tokens[i].type == COMMAND) {
                tokens[i].lexeme += tokens[i+1].lexeme;
                tokens.erase(tokens.begin() + i + 1);
            }
        }
    }
    return tokens;
}

vector <Token> expand_env(vector <Token> &tokens) {
    for (unsigned int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == ENV) {
            string line2 = isEnviron((char *)tokens[i].lexeme.c_str()) ? getenv(tokens[i].lexeme.c_str()) : "";
            tokens.erase(tokens.begin() + i);
            i--;
            vector <Token> tokens2 = genTokens(line2, false);
            // cout << tokens2.size() << endl;
            for (unsigned int j = 0; j < tokens2.size(); j++) {
                //   cout << tokens2[j].lexeme << endl;
                tokens2[j].flag = true;
                if (tokens2[j].type == COMMAND) {
                    if (tokens[i].type == COMMAND) {
                        tokens[i].lexeme += tokens2[j].lexeme;
                        tokens[i].flag = true;
                    }
                    else {
                        //          cout << "insert" << endl;
                        tokens.insert(tokens.begin() + i + 1, tokens2[j]);
                        i++;
                    }
                }
                else {
            //        cout << "insert" << endl;
                    tokens.insert(tokens.begin() + i + 1, tokens2[j]);
                    i++;
                }
            }
            if (i + 1 < tokens.size() && tokens[i+1].type == COMMAND && tokens[i].type == COMMAND) {
                tokens[i].lexeme += tokens[i+1].lexeme;
                tokens[i].flag = tokens[i+1].flag;
                tokens.erase(tokens.begin() + i + 1);
            }
        }
    }
    //for (auto i: tokens) i.printToken();
    return tokens;
}

string expandPrompt(char *prompt) {
    string ret;
    while (*prompt) {
        if (*prompt == '\\') {
            if (prompt[1] == 'w') {
                ret += getenv("PWD");
                prompt++;
            }
            if (prompt[1] == 'u') {
                ret += getenv("USER");
                prompt++;
            }
        }
        else {
            ret += *prompt;
        }
        prompt++;
    }
    return ret;
}
