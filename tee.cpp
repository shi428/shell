#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
vector <int> openFiles(vector <string> &files) {
    vector <int> ret;
    for (auto i: files) {
        ret.push_back(open(i.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644));
    }
    return ret;
}
void myTee(int inputfd, vector <string> &files) {
    cout << files.size() << endl;
    char buffer[4096];
    vector <int> fds = openFiles(files);
    ssize_t n_bytes;
    while (n_bytes = read(inputfd, buffer, (sizeof(buffer) / sizeof(buffer[0])) - 1)) {
        buffer[n_bytes] = '\0';
        for (auto i: fds) {
            write(i, buffer, n_bytes);
        }
        write(1, buffer, n_bytes);
    }
    for (auto i: fds) {
        close(i);
    }
}

int main() {
    vector <string> files;
    files.push_back("tard");
    files.push_back("gay");
    files.push_back("nigger");
    myTee(0, files);
    return 0;
}
