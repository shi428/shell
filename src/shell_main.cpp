//#include <parser.h>
#define EXTERN
//#include <exec.h>
#include <shell_util.h>
#include <shell.h>
#include <jobs.h>
#include "yacc.yy.hpp"
#include "lex.yy.hpp"
vector <int> pos;
pid_t shell_pid;
//pid_t background_process;
int return_code;
string last_arg;
unordered_map<string,string> users;

Trie *buildTrie(const char *currentdir);
void print_prompt() {
    if (is_environ((char *)"PROMPT")) {
        char *prompt = getenv("PROMPT");
        cout << expand_prompt(prompt);
    }
    else {
        cout << "shell>";
    }
    fflush(stdout);
}

void sigint_handler(int signum) {
    //signal(SIGINT, sigint_handler);
    cout << endl;
    if (isatty(0)) {
        Shell::print_prompt();
    }
    fflush(stdout);
}

void getUsers(struct passwd *p) {
    while (true) {
        errno = 0; // so we can distinguish errors from no more entries
        struct passwd* entry = getpwent();
        if (!entry) {
            if (errno) {
                std::cerr << "Error reading password database\n";
                return ;
            }
            break;
        }
        //std::cout << entry->pw_name << '\n';
        users[string(entry->pw_name)] = string(entry->pw_dir);
    }
    endpwent();
}

vector <string> history;
extern string read_line();
unsigned int ind;
struct passwd *p;
YY_BUFFER_STATE  buffer;
extern int exit_flag;
int main(int argc, char *argv[]) {
    Shell::init_shell();
    shell_pid = getpid();
    string line;
    p = getpwuid(getuid());
    getUsers(p);
    //string shellrc_loc = string(getenv("HOME")) + string("/.shellrc");
    //const char *source[3] = {"source", shellrc_loc.c_str(), NULL};
    //char *shell_path = realpath("/proc/self/exe", NULL);
    //const char *set_shell[4] = {"setenv", "SHELL", shell_path, NULL};
    //run_builtin_command((char **)set_shell, p);
    //free(shell_path);
    if (isatty(0)) {
        //run_builtin_command((char **)source, p);
    }
    yyscan_t local;
    yylex_init(&local);
    while (!Shell::exit_status) {
        //trie = buildTrie(getenv("PWD"));
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
        j->launch_job(ast);
        if (Shell::exit_status) {
            Shell::delete_job(j);
        }
        delete ast;
        }
    }
    yylex_destroy(local);
    delete_aliased_commands();
    Shell::destroy_shell();
    return EXIT_SUCCESS;
}

