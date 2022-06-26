#include <shell.h>
#include "yacc.yy.hpp"
#include "lex.yy.hpp"
//#include <built-in.h>
extern int getLength(char **);
extern pid_t shell_pid;
extern int return_code;
extern pid_t background_process;
extern string last_arg;
extern unordered_map<string, string> users;

string expand_command_subst(string &subshell_command) {
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
    int readOutputPipe[2];

    char templateName[] = "tmp.XXXXXX";
    char *shellPath = strdup(Shell::shell_path);
    char *shellDirectory = dirname(shellPath);
    string namedPipe;
    namedPipe += shellDirectory;
    namedPipe += "/tmp/";
    namedPipe += to_string(create_named_pipe(namedPipe));
    subshellCommand += "\n";
    mkfifo(namedPipe.c_str(), 0666);
    if (!readWrite) {
        //scan and parse
        yyscan_t local;
        yylex_init(&local);
        AST **tr = new AST*;
        YY_BUFFER_STATE buffer = yy_scan_string((char *)subshellCommand.c_str(), local);
        yyparse(local, tr, 1);
        yy_delete_buffer(buffer, local);
        yylex_destroy(local);

        //int old_stdin = dup(STDIN_FILENO);
        pid_t child = fork();
        if (child == 0) {
            job *j = create_job_from_ast(tr);
            //dup2(in_file, STDIN_FILENO);
            //close(in_file);
            setpgid(getpid(), getpid());
            int in_file = open(namedPipe.c_str(), O_CREAT | O_RDONLY);
            j->stdin_fd = in_file;
            j->launch_job(*tr);
            close(in_file);
            delete j;
            delete tr;
            exit(0);
        }
        Shell::currentJob->pgid = child;
        process *p = new process();
        p->pid = child;
        Shell::currentJob->substProcesses.push_back(p);
        //dup2(old:stdin, STDIN_FILENO);
        //delete j;
        delete tr;

    }
    else {
        pipe(writeCommandPipe);
        write(writeCommandPipe[1], subshellCommand.c_str(), subshellCommand.length());
        if (fork() == 0) {
            dup2(writeCommandPipe[0], STDIN_FILENO);
            //if (readWrite) {
            int out_fd = open(namedPipe.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
            close(writeCommandPipe[1]);
            execvp(shell[0], (char **)shell);
            exit(0);
        }
        close(writeCommandPipe[1]);
        close(writeCommandPipe[0]);
        }
        free(shellPath);
        return namedPipe;
    }

    string expand_prompt(char *prompt) {
        string ret;
        while (*prompt) {
            if (*prompt == '\\') {
                if (prompt[1] == 'w') {
                    string user = getenv("USER");
                    string pwd = getenv("PWD");
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
    char **expand_alias(char **cmd) {
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
        char **aliased_cmd = expand_alias(aliases[string(cmd[0])].first);
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
                                        expand_wildcardHelper(newPrefix, newSuffix, new_relative_dir, entries, pwd, pwd);
                                }
                                else {
                                    if (entry->d_type == DT_DIR)
                                        expand_wildcardHelper(newPrefix, newSuffix, relative_dir, entries, relative_dir_flag, pwd);
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
                                    expand_wildcardHelper(newPrefix, newSuffix, new_relative_dir, entries, pwd, pwd);
                            }
                            else {
                                if (entry->d_type == DT_DIR)
                                    expand_wildcardHelper(newPrefix, newSuffix, relative_dir, entries, relative_dir_flag, pwd);
                            }
                        }
                    }
                }
            }
        }

        string expand_arg(Node *argNode, string &command) {
            string expandedArg;
            size_t index = 0;
            vector <char> tokens = argNode->expansionType;
            while (index < tokens.size()) {
                if (tokens[index] == 1) {
                    int delimIndex = find(tokens.begin() + index + 1, tokens.end(), 1) - tokens.begin();
                    string subshell_command = ((string *)argNode->obj)->substr(index + 2, delimIndex - (index + 2));
                    expandedArg += expand_command_subst(subshell_command);
                    index = delimIndex;
                }
                else if (tokens[index] == 2) {
                    int delimIndex = find(tokens.begin() + index + 1, tokens.end(), 2) - tokens.begin();
                    string subshell_command = ((string *)argNode->obj)->substr(index + 2, delimIndex - (index + 2));
                    expandedArg += expand_process_subst(subshell_command, true);
                    index = delimIndex;
                }
                else if (tokens[index] == 3) {
                    int delimIndex = find(tokens.begin() + index + 1, tokens.end(), 3) - tokens.begin();
                    string subshell_command = ((string *)argNode->obj)->substr(index + 2, delimIndex - (index + 2));
                    expandedArg += expand_process_subst(subshell_command, false);
                    index = delimIndex;
                }
                else {
                    expandedArg += (*(string *)argNode->obj)[index];
                }
                index++;
            }
            command += *(string *)argNode->obj;
            command += " ";
            expandedArg += " ";
            return expandedArg;
        }
        string try_expand_command(Node *node, string &command) {
            string expandedCommand;
            if (node->type == COMMAND_NODE) {
                for (auto i: node->children) {
                    expandedCommand += expand_arg(i, command);
                }

                //expand redirect file args
                for (int i = 0; i < 6; i++) {
                    string redirect_symbol;
                    switch(i) {
                        case 0:
                            redirect_symbol = " < ";
                            break;
                        case 1:
                            redirect_symbol = " > ";
                            break;
                        case 2:
                            redirect_symbol = " 2> ";
                            break;
                        case 3:
                            redirect_symbol = " >& ";
                            break;
                        case 4:
                            redirect_symbol = " >> ";
                            break;
                        case 5:
                            redirect_symbol = " >>& ";
                            break;
                    }
                    vector <Node*> files = ((Command *)(node->obj))->files[i];
                    if (files.size()) {
                        expandedCommand += redirect_symbol;
                        for (unsigned int j = 0; j < files.size() - 1; j++) {
                            expandedCommand += expand_arg(files[j], command);
                            expandedCommand += redirect_symbol;
                        }
                        expandedCommand += expand_arg(files[files.size() - 1], command);
                    }

                }
                return expandedCommand;
            }
            expandedCommand += try_expand_command(node->children[0], command);
            command += " | ";
            expandedCommand += " | ";
            expandedCommand += try_expand_command(node->children[1], command);
            return expandedCommand;
        }

        vector <string> expand_wildcard(string &wildcard, bool pwd){
            vector <string> ret;
            string root_dir = "/";
            string relative_dir = "";
            expand_wildcardHelper(root_dir, wildcard, relative_dir, ret, false, pwd);
            sort(ret.begin(), ret.end());
            return ret;
        }
