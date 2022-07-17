#include <shell.h>

string convert_to_regex(string &wildcard) {
    string ret;
    for (auto i: wildcard) {
        if (i == '*') {
            ret += ".*";
        }
        else if (i == '?') {
            ret += '.';
        }
        else if (i == '.') {
            ret += "\\.";
        }
        else if (i == '+') {
            ret += "\\+";
        }
        else {
            ret += i;
        }
    }
    return ret;
}

void expand_wildcardHelper(string &prefix, string &suffix, string &relative_dir, vector <string> &entries, bool relative_dir_flag, bool pwd) {
    string current_dir = prefix;
    if (suffix.find('/', 1) == string::npos) {
        DIR *directory = opendir(current_dir.c_str());
        string reg_ex = convert_to_regex(suffix);
        while (struct dirent *entry = readdir(directory)) {
            string dir = string(entry->d_name);
            string regex_substr = reg_ex.substr(1, reg_ex.find('/', 1) - 1);
            regex_substr.insert(0, 1, '^');
            regex_substr.push_back('$');
            regex_t re;
            regcomp(&re, regex_substr.c_str(), REG_EXTENDED | REG_NOSUB);
            if (!regexec(&re, entry->d_name, 1, NULL, 0)) {
                if (dir[0] == '.') {
                    if (suffix.substr(suffix.find('/') + 1)[0] == '.') {
                        if (pwd) {
                            entries.push_back("\""+ relative_dir + dir + "\"");
                        }
                        else  {
                            entries.push_back("\"" + prefix + dir + "\"");
                        }
                    }
                }
                else {
                    if (pwd) {
                        entries.push_back("\"" + relative_dir + dir + "\"");
                    }
                    else  {
                        entries.push_back("\"" + prefix + dir + "\"");
                    }
                }
            }
        }
        return ;
    }
    else {
        DIR *directory = opendir(current_dir.c_str());
        string reg_ex = convert_to_regex(suffix);
        while (struct dirent *entry = readdir(directory)) {
            string dir = string(entry->d_name);
            string regex_substr = reg_ex.substr(1, reg_ex.find('/', 1) - 1);
            string newPrefix = prefix + dir + "/";
            regex_substr.insert(0, 1, '^');
            regex_substr.push_back('$');
            regex_t re;
            regcomp(&re, regex_substr.c_str(), REG_EXTENDED | REG_NOSUB);
            if (!regexec(&re, entry->d_name, 1, NULL, 0)) {
                if (dir[0] == '.') {
                    if (suffix.substr(suffix.find('/') + 1)[0] == '.') {
                        string newSuffix = suffix.substr(suffix.find('/',1));
                        string pwd_str = string(getenv("PWD")) + "/";
                        if (!prefix.compare(pwd_str) || relative_dir_flag) {
                            string new_relative_dir = relative_dir + dir + "/";
                            if (entry->d_type == DT_DIR) {
                                expand_wildcardHelper(newPrefix, newSuffix, new_relative_dir, entries, pwd, pwd);
                            }
                        }
                        else {
                            if (entry->d_type == DT_DIR) {
                                expand_wildcardHelper(newPrefix, newSuffix, relative_dir, entries, relative_dir_flag, pwd);
                            }
                        }
                    }
                }
                else {
                    string newSuffix = suffix.substr(suffix.find('/', 1));
                    string pwd_str = string(getenv("PWD")) + "/";
                    if (!prefix.compare(pwd_str) || relative_dir_flag) {
                        string new_relative_dir = relative_dir + dir + "/";
                        if (entry->d_type == DT_DIR) {
                            expand_wildcardHelper(newPrefix, newSuffix, new_relative_dir, entries, pwd, pwd);
                        }
                    }
                    else {
                        if (entry->d_type == DT_DIR) {
                            expand_wildcardHelper(newPrefix, newSuffix, relative_dir, entries, relative_dir_flag, pwd);
                        }
                    }
                }
            }
        }
    }
}

vector <string> expand_wildcard(string &wildcard, bool pwd){
    vector <string> ret;
    string root_dir = "/";
    string relative_dir = "";
    expand_wildcardHelper(root_dir, wildcard, relative_dir, ret, false, pwd);
    sort(ret.begin(), ret.end());
    return ret;
}
