#include <shell_util.h>
//#include <built-in.h>
extern int getLength(char **);
extern pid_t shell_pid;
extern int return_code;
extern pid_t background_process;
extern string last_arg;
extern unordered_map<string, string> users;

string expandSubshell(string &subshell_command) {
    const char *shell[2] = {"/proc/self/exe", NULL};
    int write_command_pipe[2];
    int read_output_pipe[2];

    pipe(write_command_pipe);
    pipe(read_output_pipe);
    subshell_command += "\n";
    write(write_command_pipe[1], subshell_command.c_str(), subshell_command.length());

    if (fork() == 0) {
        dup2(write_command_pipe[0], STDIN_FILENO);
        dup2(read_output_pipe[1], STDOUT_FILENO);
        close(read_output_pipe[0]);
        close(write_command_pipe[1]);
        execvp(shell[0], (char **)shell);
        exit(0);
    }

    close(write_command_pipe[0]);
    close(write_command_pipe[1]);
    close(read_output_pipe[1]);
    
    char buf[4096];
    string output;
    ssize_t n_bytes;
    while ((n_bytes = read(read_output_pipe[0], buf, 4095)) != 0) {
        buf[n_bytes] = '\0';
        char *temp = buf;
        while (*temp != '\0'){
            if (*temp == '\n') {
                output += ' ';
            }
            else {
                output += *temp;
            }
            temp++;
        }
    }
    close(read_output_pipe[0]);
    if (output.back() == ' ') {
    output.pop_back();
    }
    return output;
}
string expandPrompt(char *prompt) {
    string ret;
    while (*prompt) {
        if (*prompt == '\\') {
            if (prompt[1] == 'w') {
                string user = getenv("USER");
                string pwd = getenv("PWD");
                /*cout << user << endl;
                  cout << users[user] << endl;
                  cout << pwd << endl;*/
                size_t pos = pwd.find(users[user]);
                if (pos != string::npos) { 
                    pwd.erase(pwd.begin() + pos, pwd.begin() + pos + users[user].length());
                    pwd.insert(pos, "~");
                }
                ret += pwd;
                prompt++;
            }
            if (prompt[1] == 'u') {
                ret += getenv("USER");
                prompt++;
            }
            if (prompt[1] == 'H') {
                char hostname[100];
                gethostname(hostname, sizeof(hostname));
                ret += hostname;
                prompt++;
            }
        }
        else {
            ret += *prompt;
        }
        prompt++;
    }
    return ret;
}

extern unordered_map<string, pair<char **, int>> aliases;
char **expandAlias(char **cmd) {
    if (aliases.find(string(cmd[0])) == aliases.end()) {
        return cmd;
    }
    if (!strcmp(aliases[string(cmd[0])].first[0], cmd[0])) {
        char **aliased_cmd = aliases[string(cmd[0])].first;
        int aliased_length = getLength(aliased_cmd);
        int length = getLength(&cmd[1]);
        char **ptr = new char *[aliased_length + 1 + length];
        for (int i = 0; i < aliased_length; i++) {
            ptr[i] = aliased_cmd[i];
        }
        for (int i = aliased_length; i < aliased_length + length; i++) {
            ptr[i] = cmd[i - aliased_length + 1];
        }
        ptr[aliased_length + length] = NULL;
        return ptr;
    }
    char **aliased_cmd = expandAlias(aliases[string(cmd[0])].first);
    int aliased_length = getLength(aliased_cmd);
    int length = getLength(&cmd[1]);
    char **ptr = new char *[aliased_length + length + 1];
    for (int i = 0; i < aliased_length; i++) {
        ptr[i] = aliased_cmd[i];
    }
    for (int i = aliased_length; i < aliased_length + length; i++) {
        ptr[i] = cmd[i - aliased_length + 1];
    }
    ptr[aliased_length + length] = NULL;
    if (aliases[string(cmd[0])].first != aliased_cmd) {
        delete []aliased_cmd;
    }
    return ptr;
}

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

void expandWildcardHelper(string &prefix, string &suffix, string &relative_dir, vector <string> &entries, bool relative_dir_flag, bool pwd) {
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
            //cout << dir << " " << regex_substr << " " << suffix << endl;
            if (!regexec(&re, entry->d_name, 1, NULL, 0)/*regex_match(dir, regex(regex_substr))*/) {
                //if (regex_match(dir, regex(regex_substr))) {
                if (dir[0] == '.') {
                    if (suffix.substr(suffix.find('/') + 1)[0] == '.') {
                        if (pwd) {
                            entries.push_back(relative_dir + dir);
                        }
                        else  {
                            entries.push_back(prefix + dir);
                        }
                    }
                }
                else {
                    if (pwd) {
                        entries.push_back(relative_dir + dir);
                    }
                    else  {
                        entries.push_back(prefix + dir);
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
                if (!regexec(&re, entry->d_name, 1, NULL, 0)/*regex_match(dir, regex(regex_substr))*/) {
                    if (dir[0] == '.') {
                        if (suffix.substr(suffix.find('/') + 1)[0] == '.') {
                            string newSuffix = suffix.substr(suffix.find('/',1));
                            string pwd_str = string(getenv("PWD")) + "/";
                            if (!prefix.compare(pwd_str) || relative_dir_flag) {
                                string new_relative_dir = relative_dir + dir + "/";
                                if (entry->d_type == DT_DIR)
                                    expandWildcardHelper(newPrefix, newSuffix, new_relative_dir, entries, pwd, pwd);
                            }
                            else {
                                if (entry->d_type == DT_DIR)
                                    expandWildcardHelper(newPrefix, newSuffix, relative_dir, entries, relative_dir_flag, pwd);
                            }
                        }
                    }
                    else {
                        string newSuffix = suffix.substr(suffix.find('/', 1));
                        //cout << newPrefix <<  " " << newSuffix << endl;
                        string pwd_str = string(getenv("PWD")) + "/";
                        if (!prefix.compare(pwd_str) || relative_dir_flag) {
                            string new_relative_dir = relative_dir + dir + "/";
                            if (entry->d_type == DT_DIR)
                                expandWildcardHelper(newPrefix, newSuffix, new_relative_dir, entries, pwd, pwd);
                        }
                        else {
                            if (entry->d_type == DT_DIR)
                                expandWildcardHelper(newPrefix, newSuffix, relative_dir, entries, relative_dir_flag, pwd);
                        }
                    }
                }
            }
        }
    }

    vector <string> expandWildcard(string &wildcard, bool pwd){
        vector <string> ret;
        string root_dir = "/";
        string relative_dir = "";
        expandWildcardHelper(root_dir, wildcard, relative_dir, ret, false, pwd);
        sort(ret.begin(), ret.end());
        return ret;
    }
