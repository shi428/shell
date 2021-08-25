#include <dirent.h>
#include <trie.h>
#include <unistd.h>

Trie *buildTrie(string &str) {
    size_t period = str.find_last_of('.');
    size_t slash = str.find_last_of('/');
    string currentDir;
    string currFile;
    /*if (period < str.length() - 1 && str[period + 1] == '/' && str[period - 1] == '/'){
        string substr = str.substr(0, period);
        currentDir = substr.substr(0, substr.find_last_of('/'));
        currFile = str.substr(period);
        //cout << currentDir << currFile << endl;
    }
    else {
    currentDir = str.substr(0, str.find_last_of('/'));
    currFile = str.substr(str.find_last_of('/') + 1);
    }*/
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
    //cout << currFile.length() << endl;
    }
    if (!currentDir.compare("")) {
        currentDir = getenv("PWD");
    }
    }
    //cout << currentDir << endl;
    DIR *dir = opendir(currentDir.c_str());
    if (!dir) return NULL;
    Trie *ret = new Trie;
    //cout << currFile.length() << endl;
    while (struct dirent *entry = readdir(dir)) {
        string file_name = entry->d_name;
        string newFile;
        for (auto i: file_name) {
            if (i == ' ') {
                newFile += "\\ ";
            }
            else {
                newFile += i;
            }
        }
        file_name = newFile;
        //cout << file_name << endl;
        string full_path = string(currentDir) + '/' + string(file_name);
        if (entry->d_type == DT_DIR) {
            file_name = string(file_name) + '/';
        }
        else if (access(full_path.c_str(), F_OK | X_OK) != -1 && !strcmp(currentDir.c_str(), getenv("PWD"))) {
            if (!currentDir.compare(getenv("PWD"))) {
            file_name = "./" + string(entry->d_name);
            }
        }
        //cout << strncmp(currFile.c_str(), file_name.c_str(), currFile.length()) << endl;
        if (!strncmp(currFile.c_str(), file_name.c_str(), currFile.length())) {
        ret->insert(file_name);
        }
    }
    closedir(dir);
    return ret;
}
