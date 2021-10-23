#include <shell.h>

using namespace std;
vector <int> openFiles(vector <string> &files) {
    vector <int> ret;
    for (auto i: files) {
        ret.push_back(open(i.c_str(), O_RDONLY, 0644));
    }
    return ret;
}

void myCat(int inputfd, int outputfd, vector <string> &files, bool readInput) {
    char buffer[4096];
    vector <int> fds = openFiles(files);
    ssize_t n_bytes;
    if (readInput) {
        while ((n_bytes = read(inputfd, buffer, 4095)) != 0) {
            buffer[n_bytes] = '\0';
            write(outputfd, buffer, n_bytes);
        }
    }
    for (auto i: fds) {
        while ((n_bytes = read(i, buffer, 4095)) != 0) {
            buffer[n_bytes] = '\0';
            write(outputfd, buffer, n_bytes);
        }
    }
}

/*int main() {
    vector <string> files;
    int pipefd[2];
    const char *cmd[2] = {"wc", NULL};
    pipe(pipefd);
    files.push_back("tee.cpp");
    files.push_back("cat.cpp");
    myCat(0, pipefd[1], files, false);
    close(pipefd[1]);
    if (fork() == 0) {
        int fd = open("tee.cpp", O_RDONLY);
        dup2(fd, 0);
        close(pipefd[1]);
        close(fd);
        execvp(cmd[0], (char **)cmd);
    }
    close(pipefd[0]);
}*/
