#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
vector <int> openFiles(vector <string> &files, int append) {
    vector <int> ret;
    for (auto i: files) {
        if (append) {
        ret.push_back(open(i.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0644));
        }
        else {
        ret.push_back(open(i.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644));
        }
    }
    return ret;
}
void myTee(int inputfd, int output_fd, vector <string> &files, int append) {
    files = vector<string>(files);
    char buffer[4096];
    vector <int> fds = openFiles(files, append);
    ssize_t n_bytes;
    while (n_bytes = read(inputfd, buffer, (sizeof(buffer) / sizeof(buffer[0])) - 1)) {
        buffer[n_bytes] = '\0';
        for (auto i: fds) {
            write(i, buffer, n_bytes);
        }
        if (output_fd) {
        write(output_fd, buffer, n_bytes);
        }
    }
    for (auto i: fds) {
        close(i);
    }
}

/*int main() {

    //run ls > file1 > file2 > file3 >> append| grep a 
    vector <string> files;
    vector <string> files2;
    files.push_back("file1");
    files.push_back("file2");
    files.push_back("file3");

    files2.push_back("file4");

    int my_pipe[2];
    int my_pipe2[2];
    int my_pipe3[2];
    const char *cmd[2] = {"ls", NULL};
    const char *cmd2[3] = {"grep","a", NULL};

    pipe(my_pipe);
    pipe(my_pipe2);
    pipe(my_pipe3);

    if (fork() == 0) {
        dup2(my_pipe[1], STDOUT_FILENO);
        close(my_pipe[1]);
        execvp(cmd[0], (char **)cmd);
    }
    close(my_pipe[1]);

    myTee(my_pipe[0], my_pipe2[1], files, 0);

    close(my_pipe2[1]);

    //append
    myTee(my_pipe2[0], my_pipe3[1], files2, 1);

    close(my_pipe2[0]);
    close(my_pipe3[1]);
    if (fork() == 0) {
        dup2(my_pipe3[0], STDIN_FILENO);
        close(my_pipe3[0]);
        execvp(cmd2[0], (char **)cmd2);
    }
    close(my_pipe3[0]);
    return 0;
}*/

