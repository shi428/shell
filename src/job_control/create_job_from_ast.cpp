#include <shell.h>
#include <yacc.yy.hpp>
#include <lex.yy.hpp>

job *create_job_from_ast(AST **tr) {
    job *j = new job;
    Shell::currentJob = j;
    string command;
    j->foreground = !(*tr)->root->background;
    string expanded_command = try_expand_command((*tr)->root, command);
    //cout << expanded_command << endl;
    delete *tr;

    //parse expanded command
    yyscan_t local;
    yylex_init(&local);
    YY_BUFFER_STATE buffer = yy_scan_string((char *)expanded_command.c_str(), local);
    yyparse(local, tr, 1);
    yy_delete_buffer(buffer, local);
    yylex_destroy(local);

    traverse_helper((*tr)->root, j, command);
    j->command = strdup(command.c_str());
    return j;
}

void traverse_helper(Node *node, job *j, string &command) {
    if (node->type == COMMAND_NODE) {
        vector <string> args;
        int index = 0;
        process *p = new process();
        p->argv = new char *[node->children.size() + 1];
        for (auto i: node->children) {
            args.push_back(*(string *)i->obj);
            p->argv[index] = strdup(((string *)i->obj)->c_str());
            command += *(string *)i->obj;
            command += " ";
            index++;
        }
        p->argv[index] = NULL;
        for (int i = 0; i < 6; i++) {
            for (auto j: (((Command *)node->obj)->files[i])) {
                p->files[i].push_back(*(string*)j->obj);
            }
        }
        j->append_process(p);
    }
    else {
        traverse_helper(node->children[0], j, command);
        command += " | ";
        traverse_helper(node->children[1], j, command);
    }
}
