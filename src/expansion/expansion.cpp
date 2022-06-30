#include <shell.h>
#include "../yacc.yy.hpp"
#include "../lex.yy.hpp"
//#include <built-in.h>
extern pid_t shell_pid;
extern int return_code;
extern pid_t background_process;
extern string last_arg;

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
            j->stdinFd = in_file;
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
                size_t pos = pwd.find(Shell::users[user]);
                if (pos != string::npos) { 
                    pwd.erase(pwd.begin() + pos, pwd.begin() + pos + Shell::users[user].length());
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

string expand_arg(Node *argNode, string &command) {
    string expandedArg;
    size_t index = 0;
    vector <char> tokens = argNode->expansionFlags;
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
