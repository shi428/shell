#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <command.h>
#include <misc.h>

Command::~Command() {
    int i = 0;
    while (cmd[i]) {
        delete [] cmd[i];
        i++;
    }
    delete [] cmd;
}
void Command::parseCommand(Token &cmd) {
    vector <string> args;
    removeWhiteSpace(cmd.lexeme);
    this->it = new StringIterator(cmd.lexeme); 
    while (this->it->pos < this->it->len) {
        args.push_back(consumeChars(*it));
        consumeSpaces(*it);
    }
    this->cmd = new char*[args.size() + 1];
    for (unsigned int i = 0; i < args.size(); i++) {
        this->cmd[i] = new char[args[i].length() + 1];
        strcpy(this->cmd[i], args[i].c_str());
    }
    this->cmd[args.size()] = NULL;
    delete this->it;
}

string parseFile(Token &file) {
    removeWhiteSpace(file.lexeme);
    StringIterator it(file.lexeme);
    string ret;
    switch (file.type) {
        case FILE_OUT:
            it.advance();
            consumeSpaces(it);
            ret += consumeChars(it);
            break;
        case FILE_IN:
            it.advance();
            consumeSpaces(it);
            ret += consumeChars(it);
            break;
        case FILE_ERR:
            it.advance();
            it.advance();
            consumeSpaces(it);
            ret += consumeChars(it);
            break;
        case FILE_OUT_ERR: 
            it.advance();
            it.advance();
            consumeSpaces(it);
            ret += consumeChars(it);
            break;
        case FILE_APPEND:
            it.advance();
            it.advance();
            consumeSpaces(it);
            ret += consumeChars(it);
            break;
        case FILE_APPEND_ERR:
            it.advance();
            it.advance();
            it.advance();
            consumeSpaces(it);
            ret += consumeChars(it);
            break;
        default: break;
    }
    return ret;
}

void Command::parseFileList(vector <Token> &files) {
    for (auto i: files) {
        this->files[i.type - 1].push_back(parseFile(i));
    }
}

void Command::printCommand(int spaces) {
    indent(spaces);
    cout << "COMMAND = [";
    int i = 0;
    while (cmd[i]) {
        if (cmd[i+1] == NULL) {
            cout << cmd[i] << "]" << endl;
        }
        else {
            cout << cmd[i] << " ";
        }
        i++;
    }
    indent(spaces);
    cout << "FILES = {" << endl;
    for (int i = 0; i < 6; i++) {
        indent(spaces);
        string commandType;
        switch (i + 1) {
            case FILE_OUT:
                commandType = "FILE_OUT";
                break;
            case FILE_IN: 
                commandType = "FILE_IN";
                break;
            case FILE_ERR:
                commandType = "FILE_ERR";
                break;
            case FILE_OUT_ERR:
                commandType = "FILE_OUT_ERR";
                break;
            case FILE_APPEND:
                commandType = "FILE_APPEND";
                break;
            case FILE_APPEND_ERR:
                commandType = "FILE_APPEND_ERR";
                break;
        }
        cout << commandType << " : ";
        for (unsigned int j = 0; j < files[i].size(); j++) {
            if (j == files[i].size() - 1) {
                cout << files[i][j];
            }
            else {
                cout << files[i][j] << " , ";
            }
        }
        cout << endl;
    }
}

int test_out(vector <string> *files) {
    return files[1].size() || files[3].size() || files[4].size() || files[5].size();
}

int test_error(vector <string> *files) {
    return files[2].size() || files[3].size() || files[5].size();
}

int test_redirect(vector <string> *files) {
    return !files[1].size() && !files[2].size() && !files[3].size() && !files[4].size() && !files[5].size();
}

int Command::redirectOut(int *pipefd, int *pipefd2, int i, bool pipeflag, bool _append) {
    char **temp_cmd = NULL;
    int start = 1;
    pid_t child;
    if (_append) {
        start = 2;
        temp_cmd = new char *[files[i].size() + 3];
        temp_cmd[0] = new char[4];
        temp_cmd[1] = new char[3];
        strcpy(temp_cmd[0], "tee");
        strcpy(temp_cmd[1], "-a");
    }
    else {
        temp_cmd = new char *[files[i].size() + 2];
        temp_cmd[0] = new char[4];
        strcpy(temp_cmd[0], "tee");
    }
    for (unsigned int j = 0; j < files[i].size(); j++) {
        temp_cmd[j + start] = new char [files[i][j].length() + 1];
        strcpy(temp_cmd[j + start], files[i][j].c_str());
    }
    temp_cmd[files[i].size() + start] = NULL;
    if ((child = fork()) == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        if (pipeflag) {
            dup2(pipefd2[1], STDOUT_FILENO);
            close(pipefd2[0]);
        }
        else {
            int devnull = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC);
            dup2(devnull, STDOUT_FILENO);
            close(devnull);
        }
        execvp(temp_cmd[0], temp_cmd);
        exit(EXIT_SUCCESS);
    }
    close(pipefd[0]);
    if (pipeflag) {
        close(pipefd2[1]);
    }
    for (unsigned int j = 0; j < files[i].size() + start; j++) {
        delete [] temp_cmd[j];
    }
    delete []temp_cmd;
    return 0;
}
/*int Command::redirectOut(int *pipefd, vector <pid_t> &children, int readfd, int writefd, int i, bool pipe) {
//int status;
pid_t child;
char **temp_cmd = NULL;
int start = 1;
if (!files[i].size()) {
if (pipe) {
temp_cmd = new char *[3];
temp_cmd[0] = new char[4];
temp_cmd[1] = new char[10];
strcpy(temp_cmd[0], "tee");
strcpy(temp_cmd[1], "/tmp/pipe");
temp_cmd[2] = NULL;
child = fork();
if (child == 0) {
dup2(pipefd[0], STDIN_FILENO);
dup2(writefd, STDOUT_FILENO);
//close(readfd);
close(pipefd[1]);
execvp(temp_cmd[0], temp_cmd);
}
//           waitpid(child, &status, 0);
delete [] temp_cmd[0];
delete temp_cmd;
close(pipefd[0]);
children.push_back(child);
return 1;
}
close(pipefd[0]);
return 0;
}
if (i == 4 || i == 5) {
start = 2;
if (pipe) {
start++;
temp_cmd = new char *[files[i].size() + 4];
temp_cmd[2] = new char[10];
strcpy(temp_cmd[2], "/tmp/pipe");
}
else {
temp_cmd = new char *[files[i].size() + 3];
}
temp_cmd[0] = new char[4];
temp_cmd[1] = new char[3];
strcpy(temp_cmd[0], "tee");
strcpy(temp_cmd[1], "-a");
}
else {
if (pipe) {
temp_cmd = new char *[files[i].size() + 3];
temp_cmd[1] = new char[10];
strcpy(temp_cmd[1], "/tmp/pipe");
start++;
}
else {
temp_cmd = new char *[files[i].size() + 2];
}
temp_cmd[0] = new char[4];
strcpy(temp_cmd[0], "tee");
}
for (unsigned int j = 0; j < files[i].size(); j++) {
temp_cmd[j + start] = new char [files[i][j].length() + 1];
strcpy(temp_cmd[j + start], files[i][j].c_str());
}
temp_cmd[files[i].size() + start] = NULL;
child = fork();
if (child == 0) {
dup2(pipefd[0], STDIN_FILENO);
close(pipefd[1]);
if (pipe) {
dup2(writefd, STDOUT_FILENO);
close(readfd);
}
else {
    int devnull = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC);
    dup2(devnull, STDOUT_FILENO);
    close(devnull);
}
execvp(temp_cmd[0], temp_cmd);
}
//    waitpid(child, &status, 0);
for (unsigned int j = 0; j < files[i].size() + start; j++) {
    delete [] temp_cmd[j];
}
delete []temp_cmd;
close(pipefd[0]);
children.push_back(child);
return 1;
}*/

int Command::redirectIn(int *pipefd, vector <pid_t> &children, int readfd, int writefd, bool pipeflag) {
    pid_t child;
    // int status;
    int start = 1;
    char **temp_cmd = NULL;
    if (pipeflag) {
        start = 2;
        temp_cmd = new char *[files[0].size() + 3];
        temp_cmd[0] = new char[4];
        temp_cmd[1] = new char[2];
        strcpy(temp_cmd[0], "cat");
        strcpy(temp_cmd[1], "-");
    }
    else {
        temp_cmd = new char *[files[0].size() + 2];
        temp_cmd[0] = new char[4];
        strcpy(temp_cmd[0], "cat");
    }
    for (unsigned int i = 0; i < files[0].size(); i++) {
        temp_cmd[start + i] = new char [files[0][i].length() + 1];
        strcpy(temp_cmd[start + i], files[0][i].c_str());
    }
    temp_cmd[files[0].size() + start] = NULL;
    child = fork();
    if (child == 0) {
        if (pipeflag) {
            dup2(readfd, STDIN_FILENO);
            close(writefd);
        }
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        execvp(temp_cmd[0], temp_cmd);
    }
    //waitpid(child, &status, 0);
    for (unsigned int i = 0; i < files[0].size() + start; i++) {
        delete [] temp_cmd[i];
    }
    delete []temp_cmd;
    close(pipefd[1]);
    children.push_back(child);
    return 1;
}

int Command::execute(struct passwd *p, vector <pid_t> &children, int *pipefd, int readfd, int writefd, vector <string> &cmds) {
    int i = 0;
    if (strcmp(cmd[0], "cd") == 0) {
        if (cmd[1] == NULL) {
            string homedir = "/home/" + string(p->pw_name);
            chdir(homedir.c_str());
            return 0;
        }
        else {
            chdir(cmd[1]);
            return 0;
        }
    }
    if (strcmp(cmd[0], "exit") == 0) {
        return 1;
    }
    pid_t child;
    int fdout[2];
    int fdin[2];
    int fderr[2];
    pipe(fdout);
    pipe(fdin);
    pipe(fderr);

    if ((child = fork()) == 0) {
        dup2(readfd, STDIN_FILENO);
        dup2(writefd, STDOUT_FILENO);
        if (files[0].size()) {
        redirectIn(fdin, children, readfd, writefd, readfd != STDIN_FILENO);
        dup2(fdin[0], STDIN_FILENO);
        }
        close(fdin[1]);
        if (files[1].size() || files[4].size()) {
            dup2(fdout[1], STDOUT_FILENO); 
            close(fdout[0]);
        }
        if (files[2].size()) {
            dup2(fderr[1], STDERR_FILENO); 
            close(fderr[0]);
        }
        if (execvp(cmd[0], cmd) == -1) {
            cerr << "\033[1;41mshell: command not found: " << cmd[0] << "\033[0m\n";
            exit(EXIT_FAILURE);
        }
    }
    close(fdout[1]);
    close(fderr[1]);
    close(fdin[1]);
    close(fdin[0]);
    if (files[1].size()) { //>
        if (files[4].size()) { //>> 
            int fdappend[2];
            pipe(fdappend);
            redirectOut(fdout, fdappend, 1, true, false);
            redirectOut(fdappend, NULL, 4, false, true);
        }
        else {
            redirectOut(fdout, writefd != STDOUT_FILENO ? pipefd : NULL, 1, writefd != STDOUT_FILENO, false);
        }
    }
    if (files[2].size()) { //2>
        redirectOut(fderr, NULL, 2, false, false);
    }
    else if (files[4].size()) {
            redirectOut(fdout, NULL, 4, false, true);
    }
    children.push_back(child);
    while (cmd[i]) {
        cmds.push_back(cmd[i]);
        i++;
    }
    return 0;
}

