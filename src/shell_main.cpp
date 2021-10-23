//#include <parser.h>
#define EXTERN
//#include <exec.h>
#include <shell.h>
#include "yacc.yy.hpp"
#include "lex.yy.hpp"
vector <pair<pid_t, vector <string>>> bPids;
vector <int> pos;
pid_t shell_pid;
pid_t background_process;
int return_code;
string last_arg;
unordered_map<string,string> users;

Trie *buildTrie(const char *currentdir);
void printPrompt() {
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
        printPrompt();
    }
    fflush(stdout);
}

void sigchild_handler(int signum) {
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        for (unsigned int i = 0; i < bPids.size(); i++) {
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
                    printPrompt();
                }
                fflush(stdout);
            }
        }
    }
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
    init_shell();
    shell_pid = getpid();
    string line;
    p = getpwuid(getuid());
    getUsers(p);
    string shellrc_loc = string(getenv("HOME")) + string("/.shellrc");
    const char *source[3] = {"source", shellrc_loc.c_str(), NULL};
    char *shell_path = realpath("/proc/self/exe", NULL);
    const char *set_shell[4] = {"setenv", "SHELL", shell_path, NULL};
    runBuiltInCommand((char **)set_shell, p);
    free(shell_path);
    if (isatty(0)) {
        runBuiltInCommand((char **)source, p);
    }
    yyscan_t local;
    yylex_init(&local);
    while (!exit_flag) {
        //trie = buildTrie(getenv("PWD"));
        if (isatty(0)) {
            printPrompt();
        }
        line = read_line();
        if (!line.compare("")) break;
        if (line.compare("\n")) {
            history.push_back(line);
            ind = history.size();
        }

        buffer = yy_scan_string((char *)line.c_str(), local);
        yyparse(local);
        yy_delete_buffer(buffer, local);
    }
    yylex_destroy(local);
    deleteAliasedCommands();
    return EXIT_SUCCESS;
}

void init_shell() {
    /* See if we are running interactively.  */
  shell_terminal = STDIN_FILENO;
  shell_is_interactive = isatty (shell_terminal);

  if (shell_is_interactive)
    {
      /* Loop until we are in the foreground.  */
      while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
        kill (- shell_pgid, SIGTTIN);

      /* Ignore interactive and job-control signals.  */
      signal (SIGINT, sigint_handler);
      signal (SIGQUIT, SIG_IGN);
      signal (SIGTSTP, SIG_IGN);
      signal (SIGTTIN, SIG_IGN);
      signal (SIGTTOU, SIG_IGN);
      signal (SIGCHLD, sigchild_handler);

      /* Put ourselves in our own process group.  */
      shell_pgid = getpid ();
      if (setpgid (shell_pgid, shell_pgid) < 0)
        {
          perror ("Couldn't put the shell in its own process group");
          exit (1);
        }

      /* Grab control of the terminal.  */
      tcsetpgrp (shell_terminal, shell_pgid);

      /* Save default terminal attributes for shell.  */
      tcgetattr (shell_terminal, &shell_tmodes);
    }
}
