#include <shell.h>
#include "../yacc.yy.hpp"
#include "../lex.yy.hpp"

void run_builtin_command_source(char **cmd) {
    string line;
    ifstream fin(cmd[1]);
    if (!fin.good()) {
        cerr << "source: no such file or directory: " << cmd[1] << endl;
        return;
    }

    yyscan_t local;
    yylex_init(&local);

    while (getline(fin, line)) {
        line += '\n';
        YY_BUFFER_STATE buffer = yy_scan_string((char *)line.c_str(), local);
        AST *ast = NULL;
        yyparse(local, &ast, 0);
        yy_delete_buffer(buffer, local);
        if (ast && ast->root) {
            job *j = create_job_from_ast(&ast);
            Shell::insert_job(j);
            j->launch_job(ast);
            if (Shell::exitStatus) {
                Shell::delete_job(j);
            }
            delete ast;
        }
    }

    yylex_destroy(local);
    fin.close();
}
