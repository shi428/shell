#include <shell.h>
#include <yacc.yy.hpp>
#include <lex.yy.hpp>

int create_named_pipe(string &dirName) {
    DIR *directory = opendir(dirName.c_str());
    struct dirent *entry;
    vector <int> file_nos;
    int id = 1;
    while( (entry=readdir(directory)) )
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        file_nos.push_back(atoi(entry->d_name));
    }
    sort(file_nos.begin(), file_nos.end());

    for (auto i: file_nos) {
        if (id < i) {
            break;
        }
        id++;
    }
    return id;
}

string expand_process_subst(string &subshellCommand, bool readWrite) {
    const char *shell[2] = {"/proc/self/exe", NULL};
    int writeCommandPipe[2];
    char *shellPath = strdup(Shell::shellPath);
    char *shellDirectory = dirname(shellPath);
    string namedPipe;

    namedPipe += shellDirectory;
    namedPipe += "/tmp/";
    namedPipe += to_string(create_named_pipe(namedPipe));
    subshellCommand += "\n";
    mkfifo(namedPipe.c_str(), 0666);
    if (!readWrite) {
        expand_right_process_subst(writeCommandPipe, subshellCommand, namedPipe, shell);
    }
    else {
        expand_left_process_subst(writeCommandPipe, subshellCommand, namedPipe, shell);
    }
    free(shellPath);
    return namedPipe;
}
