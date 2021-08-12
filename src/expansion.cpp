#include <expansion.h>
#include <bits/stdc++.h>
extern int getLength(char **);
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

extern pid_t shell_pid;
extern int return_code;
extern pid_t background_process;
extern string last_arg;
vector <Token> expand_env(vector <Token> &tokens) {
    for (unsigned int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == ENV) {
            string line2 = isEnviron((char *)tokens[i].lexeme.c_str()) ? getenv(tokens[i].lexeme.c_str()) : "";
            if (!tokens[i].lexeme.compare("$")) {
                stringstream ss;
                ss << shell_pid;
                line2 = ss.str();
            }
            if (!tokens[i].lexeme.compare("?")) {
                stringstream ss;
                ss << return_code;
                line2 = ss.str();
            }
            if (!tokens[i].lexeme.compare("!")) {
                stringstream ss;
                ss << background_process;
                line2 = ss.str();
            }
            if (!tokens[i].lexeme.compare("_")) {
                line2 = last_arg;
            }
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

extern unordered_map<string, string> users;

vector <Token> expand_tilde(vector <Token> &tokens) {
    for (unsigned int i = 0; i < tokens.size(); i++) {
        bool flag = false;
        if (tokens[i].type == TILDE) {
            string line2 = tokens[i].lexeme;
            string user;
            size_t slash_pos = tokens[i].lexeme.find('/');
            if (slash_pos == string::npos) {
                user = tokens[i].lexeme.substr(1);
                line2.erase(0, user.length() + 1);
                if (!user.compare("")) {
                    line2.insert(0, users[string(getenv("USER"))]);
                }
                else {
                    if (users.find(user) == users.end()) {
                        tokens[i].type = COMMAND;
                        flag = true;
                    }
                    else {
                    line2.insert(0, users[user]);
                    }
                }
            }
            else {
                user = tokens[i].lexeme.substr(1, slash_pos-1);
                line2.erase(0, user.length() + 1);
                if (!user.compare("")) {
                    line2.insert(0, users[string(getenv("USER"))]);
                }
                else {
                    if (users.find(user) == users.end()) {
                        tokens[i].type = COMMAND;
                        flag = true;
                    }
                    else {
                    line2.insert(0, users[user]);
                    }
                }
            }
            if (!flag) {
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
                string user = getenv("USER");
                string pwd = getenv("PWD");
                /*cout << user << endl;
                  cout << users[user] << endl;
                  cout << pwd << endl;*/
                size_t pos = pwd.find(users[user]);
                if (pos != string::npos) { 
                    pwd.erase(pwd.begin() + pos, pwd.begin() + pos + users[user].length());
                    pwd.insert(pos, "~");
                }
                ret += pwd;
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

extern unordered_map<string, pair<char **, int>> aliases;
char **expandAlias(char **cmd) {
    if (aliases.find(string(cmd[0])) == aliases.end()) {
        return cmd;
    }
    if (!strcmp(aliases[string(cmd[0])].first[0], cmd[0])) {
        char **aliased_cmd = aliases[string(cmd[0])].first;
        int aliased_length = getLength(aliased_cmd);
        int length = getLength(&cmd[1]);
        char **ptr = new char *[aliased_length + 1 + length];
        for (int i = 0; i < aliased_length; i++) {
            ptr[i] = aliased_cmd[i];
        }
        for (int i = aliased_length; i < aliased_length + length; i++) {
            ptr[i] = cmd[i - aliased_length + 1];
        }
        ptr[aliased_length + length] = NULL;
        return ptr;
    }
    char **aliased_cmd = expandAlias(aliases[string(cmd[0])].first);
    int aliased_length = getLength(aliased_cmd);
    int length = getLength(&cmd[1]);
    char **ptr = new char *[aliased_length + length + 1];
    for (int i = 0; i < aliased_length; i++) {
        ptr[i] = aliased_cmd[i];
    }
    for (int i = aliased_length; i < aliased_length + length; i++) {
        ptr[i] = cmd[i - aliased_length + 1];
    }
    ptr[aliased_length + length] = NULL;
    delete []aliased_cmd;
    return ptr;
}
