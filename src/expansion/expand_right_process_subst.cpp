#include <shell.h>
#include <yacc.yy.hpp>
#include <lex.yy.hpp>
void expand_right_process_subst(int *writeCommandPipe, string &subshellCommand, string &namedPipe, const char **shell) {

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
    delete tr;

}
