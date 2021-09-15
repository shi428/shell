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
int exit_flag;
Command *currentCommand;
#define YYERROR_VERBOSE 1
std::vector <std::string> args_vec;
%}

%union {char ch; std::string* str; Node *node;}
%start goal
%token <ch> CHAR
%token <str> ESCAPE_CHAR
%token AMPERSAND
%token AND
%token PIPE
%token OR
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
%type <node> simple_command;
%type <node> simpler_command;
%type <node> command_line;
%type <node> full_command_line;
%type space;
%%

goal:
 space full_command_line space NEWLINE {
    Tree *tr = new Tree;
    tr->root = $2;
    if (tr->root) {
        //        parseTree->root->traverse(0);
        if (exec(tr, p, tr->root, bPids, pos)) {
        exit_flag = 1;
        }
            //last_arg = (bPids.back().second).back();
    }
        delete tr;
}
  | NEWLINE 
  | error NEWLINE { yyerrok; }
 
;

full_command_line: command_line AMPERSAND {
                $$=$1;
                $$->background = 1;
                 } |
                 command_line;
command_line: simple_command space PIPE space command_line {
            $$=new Node();
            $$->type = PIPE_NODE;
            $$->left = $1;
            $$->right = $5;
            }
            |
            simple_command space AND space command_line
            {
            $$=new Node();
            $$->type = AND_NODE;
            $$->left = $1;
            $$->right = $5;
            }
            |
            simple_command space OR space command_line
            {
            $$=new Node();
            $$->type = OR_NODE;
            $$->left = $1;
            $$->right = $5;
            }
            |
            simple_command {
            $$=$1;
            }
            |
            simpler_command {
            if (yychar == PIPE) {
                yyerrok;
            }
            else {
            $$=$1;
            }
            };
simple_command: no_arg_command iomodifier {
              $$=$1;
              };

simpler_command: no_arg_command simple_iomodifier {
              $$=$1;
               };

simple_iomodifier: 
          GREATAND space word space simple_iomodifier {
            currentCommand->addFile(3, *$3);
            }
          | GREATGREATAND space word space simple_iomodifier {
            currentCommand->addFile(5, *$3);
            }|
            ;
iomodifier: 
          GREAT space word space iomodifier {
            currentCommand->addFile(1, *$3);
}
          | LESS space word space iomodifier {
            currentCommand->addFile(0, *$3);
}
          | IOERR space word space iomodifier {
            currentCommand->addFile(2, *$3);
}
          | GREATGREAT space word space iomodifier {
            currentCommand->addFile(4, *$3);
}
        |;

no_arg_command: args {
    //for (auto i: args_vec) std::cout << i << std::endl;
    $$=new Node();
    $$->type = COMMAND_NODE;
    $$->obj = new Command();
    currentCommand = (Command *)$$->obj;
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

space: SPACE | ;
%%

void yyerror(const char *s) {
    cerr << s << endl;
}

