%{
#include <bits/stdc++.h>
#include <misc.h>
#include <command.h>
#include <built-in.h>
#include <exec.h>
int yylex();
void yyerror(const char *s);
extern unsigned int ind;
extern struct passwd *p;
extern std::vector <pair<pid_t, std::vector <std::string>>> bPids;
extern std::vector <int> pos;
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern YY_BUFFER_STATE  buffer;
#define YYERROR_VERBOSE 1
std::vector <std::string> args_vec;
%}

%union {char ch; std::string* str; Node *node;}
%start goal
%token <ch> CHAR
%token <str> ESCAPE_CHAR
%token <ch> AMPERSAND
%token <ch> PIPE
%token SUBSHELL
%token ENV
%token <ch> LEFT_PAREN
%token <ch> RIGHT_PAREN
%token <ch> GREAT
%token GREATAND
%token GREATGREAT
%token GREATGREATAND
%token IOERR
//%token SPACE
%token <ch> LESS
%token <ch> DQUOTE
%token <ch> SQUOTE
%token <ch> LEFT_BRACE
%token <ch> RIGHT_BRACE
%token <ch> NEWLINE
%token SPACE;
%type <ch> ch
%type <str> word;
%type <node> no_arg_command;
%%

goal: no_arg_command NEWLINE {
    Tree *tr = new Tree;
    tr->root = $1;
    if (tr->root) {
        //        parseTree->root->traverse(0);
        if (exec(tr, p, tr->root, bPids, pos)) {
        delete tr;
        deleteAliasedCommands();
        yy_delete_buffer(buffer);
        exit(EXIT_SUCCESS);
        }
            //last_arg = (bPids.back().second).back();
    }
    delete tr;
}
| SPACE no_arg_command NEWLINE {
    Tree *tr = new Tree;
    tr->root = $2;
    if (tr->root) {
        //        parseTree->root->traverse(0);
        if (exec(tr, p, tr->root, bPids, pos)) {
        delete tr;
        deleteAliasedCommands();
        yy_delete_buffer(buffer);
        exit(EXIT_SUCCESS);
        }
            //last_arg = (bPids.back().second).back();
    }
        delete tr;
}
  | NEWLINE 
  | error NEWLINE { yyerrok; }
 
;
no_arg_command: args {
    //for (auto i: args_vec) std::cout << i << std::endl;
    $$=new Node();
    $$->type = COMMAND_NODE;
    $$->obj = new Command();
    ((Command *)($$->obj))->createArgs(args_vec);
    args_vec.clear();
}
args: 
   word {
   if (($1)->length()) {
    args_vec.push_back(*$1);
    delete $1;
    }
   }
   |
   args SPACE word {
   if (($3)->length()) {
    args_vec.push_back(*$3);
    delete $3;
    }
    }
   ;
word:  ch word {
    string ret = $1+ *$2;
    $$ = new string(ret.c_str());
    delete $2;
   }
    | {
   $$ = new string();
   }
    ;

ch: CHAR {
  $$=$1;
  }
  | ESCAPE_CHAR{
  $$=convertEscapeChar(*$1);
  delete $1;
  } ;
%%

void yyerror(const char *s) {
    cerr << s << endl;
}

