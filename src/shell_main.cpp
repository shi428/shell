#include <shell.h>
#include <jobs.h>
#include "yacc.yy.hpp"
#include "lex.yy.hpp"
vector <int> pos;
pid_t shell_pid;
int return_code;
string last_arg;

//Trie *buildTrie(const char *currentdir);

vector <string> history;
extern string read_line();
unsigned int ind;
YY_BUFFER_STATE  buffer;
extern int exit_flag;
int main(int argc, char *argv[]) {
    Shell::init_shell();
    string line;

    yyscan_t local;
    yylex_init(&local);
    while (!Shell::exitStatus) {
        if (isatty(0)) {
            Shell::print_prompt();
        }
        line = read_line();
        Shell::check_zombie();
        update_status();
        if (!line.compare("")) break;
        if (line.compare("\n")) {
            history.push_back(line);
            ind = history.size();
        }

        if (line.substr(0, 4).compare("jobs") == 0) {
            Shell::print_jobs();
            continue;
        }
        
        AST *ast = NULL;
        buffer = yy_scan_string((char *)line.c_str(), local);
        yyparse(local, &ast, 0);
        yy_delete_buffer(buffer, local);
        if (ast && ast->root) {
        job *j = create_job_from_ast(&ast);
        Shell::insert_job(j);
        if (j->launch_job(ast) || Shell::exitStatus) {
            update_status();
            Shell::delete_jobs();
        }
        delete ast;
        }
    }
    yylex_destroy(local);
    delete_aliased_commands();
    Shell::destroy_shell();
    return EXIT_SUCCESS;
}

