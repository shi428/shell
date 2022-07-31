#include <shell_util.h>

using namespace std;
vector <int> open_files(vector <string> &files, int append) {
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

void my_tee(int outFile, vector <string> &outFiles, vector <string> & appendFiles) {
    char buffer[4096];
    vector <int> outFds = open_files(outFiles, 0);
    vector <int> appendFds = open_files(appendFiles, 1);
    ssize_t n_bytes;

    while ((n_bytes = read(STDIN_FILENO, buffer, (sizeof(buffer) / sizeof(buffer[0])) - 1)) != 0) {
        buffer[n_bytes] = '\0';
        for (auto i: outFds) {
            write(i, buffer, n_bytes);
        }
        for (auto i: appendFds) {
            write(i, buffer, n_bytes);
        }
        if (outFile != 1) {
        write(STDOUT_FILENO, buffer, n_bytes);
        }
    }
    for (auto i: outFds) {
        close(i);
    }
    for (auto i: appendFds) {
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
    my_tee(my_pipe[0], my_pipe2[1], files, files2);
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
