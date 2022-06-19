//#include <parser.h>
#define EXTERN
//#include <exec.h>
#include <shell_util.h>
#include <shell.h>
#include <jobs.h>
#include "yacc.yy.hpp"
#include "lex.yy.hpp"
vector <pair<pid_t, vector <string>>> bPids;
vector <int> pos;
job *first_job = NULL;
pid_t shell_pid;
pid_t background_process;
int return_code;
string last_arg;
unordered_map<string,string> users;

Trie *buildTrie(const char *currentdir);
void print_prompt() {
    if (isEnviron((char *)"PROMPT")) {
        char *prompt = getenv("PROMPT");
        cout << expandPrompt(prompt);
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

/*void delete_job(job *j) {
    if (j == first_job) {
        delete j;
        first_job = NULL;
        return ;
    }
    job *it = first_job;
    while (it && it->next != j) {
        it = it->next;
    }
    it->next = j->next;
    delete j;
}*/
/*void sigchild_handler(int signum) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        //mark_process_status(first_job, pid, status);
        //job *j = find_job_by_pid(first_job, pid);
        if (j->job_is_completed()) {
            cout << endl;
            j->print_job_information();
            //delete_job(j);
                if (isatty(0)) {
                    Shell::print_prompt();
                }
                fflush(stdout);
        }for (unsigned int i = 0; i < bPids.size(); i++) {
            if (bPids[i].first == pid && pos.size()) {
                background_process = pid;
                cout << endl;
                cout << pos[i] <<". [" << pid << "] ";
                for (auto j: bPids[i].second) {
                    cout << j << " ";
                }
                cout << "has exited";
                cout << endl;
                bPids.erase(bPids.begin() + i);
                pos.erase(pos.begin() + i);
                i--;
                if (isatty(0)) {
                    Shell::print_prompt();
                }
                fflush(stdout);
            }
        }
    }
}*/

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
    string shellrc_loc = string(getenv("HOME")) + string("/.shellrc");
    const char *source[3] = {"source", shellrc_loc.c_str(), NULL};
    char *shell_path = realpath("/proc/self/exe", NULL);
    const char *set_shell[4] = {"setenv", "SHELL", shell_path, NULL};
    //runBuiltInCommand((char **)set_shell, p);
    free(shell_path);
    if (isatty(0)) {
        //runBuiltInCommand((char **)source, p);
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
        /*if (!Shell::first_job) {
            Shell::first_job = j;
        }
        else {
            job *it = Shell::first_job;
            while (it->next) {
                it = it->next;
            }
            it->next = j;
        }*/
        j->launch_job(ast);
        if (Shell::exit_status) {
            Shell::delete_job(j);
        }
        delete ast;
        }
    }
    yylex_destroy(local);
    deleteAliasedCommands();
    Shell::destroy_shell();
    return EXIT_SUCCESS;
}

