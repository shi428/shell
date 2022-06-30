#include <shell.h>
#include "../yacc.yy.hpp"
#include "../lex.yy.hpp"

string expand_command_subst(string &subshell_command) {
    const char *shell[2] = {"/proc/self/exe", NULL};
    int write_command_pipe[2];
    int read_output_pipe[2];

    pipe(write_command_pipe);
    pipe(read_output_pipe);
    subshell_command += "\n";
    write(write_command_pipe[1], subshell_command.c_str(), subshell_command.length());

    //execute subshell
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

    //read output of the subshell
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
