#include <shell_util.h>

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
void myTee(int inputfd, int output_fd, vector <string> &out_files, vector <string> &append_files) {
    char buffer[4096];
    vector <int> out_fds = openFiles(out_files, 0);
    vector <int> append_fds = openFiles(append_files, 1);
    ssize_t n_bytes;
    while ((n_bytes = read(inputfd, buffer, (sizeof(buffer) / sizeof(buffer[0])) - 1)) != 0) {
        buffer[n_bytes] = '\0';
        for (auto i: out_fds) {
            write(i, buffer, n_bytes);
        }
        for (auto i: append_fds) {
            write(i, buffer, n_bytes);
        }
        if (output_fd != 1) {
        write(output_fd, buffer, n_bytes);
        }
    }
    for (auto i: out_fds) {
        close(i);
    }
    for (auto i: append_fds) {
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
    const char *cmd[3] = {"cat", "/usr/share/dict/words", NULL};
    const char *cmd2[3] = {"grep","e", NULL};

    pipe(my_pipe);
    pipe(my_pipe2);

    if (fork() == 0) {
        dup2(my_pipe[1], STDOUT_FILENO);
        close(my_pipe[1]);
        execvp(cmd[0], (char **)cmd);
        exit(0);
    }
    close(my_pipe[1]);
    if (fork() == 0) {
    myTee(my_pipe[0], my_pipe2[1], files, files2);
    exit(0);
    }
    close(my_pipe2[1]);
    if (fork() == 0) {
        dup2(my_pipe2[0], STDIN_FILENO);
        close(my_pipe2[0]);
        execvp(cmd2[0], (char **)cmd2);
        exit(0);
    }
    close(my_pipe2[0]);
    return 0;
}
*/
