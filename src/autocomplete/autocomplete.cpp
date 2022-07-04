#include <shell_util.h>

Trie *buildTrie(string &str) {
    size_t period = str.find_last_of('.');
    size_t slash = str.find_last_of('/');
    string currentDir;
    string currFile;
    if (str.length() && str[0] == '/') {
        size_t pos = str.find_last_of('/');
        if (pos == 0) {
            currentDir = str[0];
            currFile = str.substr(pos + 1);
        }
        else {
            currentDir = str.substr(0, pos);
            currFile = str.substr(pos + 1);
        }
    }
    else {
        if (!period && period == slash - 1 && slash < str.length() - 1) {
            string substr = str.substr(0, period);
            currentDir = substr.substr(0, substr.find_last_of('/'));
            currFile = str.substr(period);
        }
        else if (slash != string::npos) {
            currentDir = str.substr(0, slash);
            currFile = str.substr(slash + 1);
        }
        if (!currentDir.compare("")) {
            currentDir = getenv("PWD");
        }
    }

    DIR *dir = opendir(currentDir.c_str());
    if (!dir) return NULL;

    Trie *ret = new Trie;
    while (struct dirent *entry = readdir(dir)) {
        string fileName = entry->d_name;
        string newFile;
        for (auto i: fileName) {
            if (i == ' ') {
                newFile += "\\ ";
            }
            else {
                newFile += i;
            }
        }
        fileName = newFile;
        string fullPath = string(currentDir) + '/' + string(fileName);
        if (entry->d_type == DT_DIR) {
            fileName = string(fileName) + '/';
        }
        else if (access(fullPath.c_str(), F_OK | X_OK) != -1 && !strcmp(currentDir.c_str(), getenv("PWD"))) {
            if (!currentDir.compare(getenv("PWD"))) {
                fileName = "./" + string(entry->d_name);
            }
        }
        if (!strncmp(currFile.c_str(), fileName.c_str(), currFile.length())) {
            ret->insert(fileName);
        }
    }
    closedir(dir);
    return ret;
}
