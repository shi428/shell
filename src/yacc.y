%{
#include <shell_util.h>
//#include <ast.h>
//#include <string>
//#include <vector>
//#include <misc.h>
//#include <command.h>
//#include <exec.h>
//#include <expansion.h>
//#include "lex.yy.hpp"
extern unsigned int ind;
extern struct passwd *p;
extern std::vector <pair<pid_t, std::vector <std::string>>> bPids;
extern std::vector <int> pos;
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern YY_BUFFER_STATE  yy_scan_string(const char *str);
extern YY_BUFFER_STATE  buffer;
extern void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer);
extern void yypush_buffer_state(YY_BUFFER_STATE new_buffer);
extern void printPrompt();
int exit_flag;
Command *currentCommand;
std::vector <Node *> currentArgs;
#define YYERROR_VERBOSE 1
std::vector <std::string> args_vec;
extern void myunput(int c);
%}

%define api.pure full
%lex-param {yyscan_t scanner};
%parse-param {yyscan_t scanner};
%parse-param {AST ** ast};
%locations
%code requires{
    typedef void *yyscan_t;
};
%code {
int yylex(YYSTYPE *yylvalp, YYLTYPE *yylocp, yyscan_t scanner);
void yyerror(YYLTYPE *yylocp, yyscan_t, AST **ast, const char *);
};
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
%type <node> command_word;
%type space;
%type <str> cmd_subst;
%type <str> arg;
%type <str> quote_word;
%type <str> quote;
%%

goal: input 
input:
 space linebreak full_command_line space linebreak {
    AST *tr = new AST;
    *ast = tr;
    tr->root = $3;
    /*if (tr->root) {
        //        parseTree->root->traverse(0);
        if (exec(tr, p, tr->root, bPids, pos) == 1) {
        exit_flag = 1;
        }
            //last_arg = (bPids.back().second).back();
    }
        delete tr;*/
}
  | NEWLINE  {
  }
  //| error NEWLINE { yyerrok; }
 
;

full_command_line: command_line AMPERSAND {
                $$=$1;
                $$->background = 1;
                 } |
                 command_line;
command_line: simple_command space PIPE space linebreak command_line {
            $$=new Node();
            $$->type = PIPE_NODE;
            $$->children.push_back($1);
            $$->children.push_back($6);
            }
            |
            /*simple_command space AND space command_line
            {
            $$=new Node();
            $$->type = AND_NODE;
            $$->children.push_back($1);
            $$->children.push_back($5);
            }
            |
            simple_command space OR space command_line
            {
            $$=new Node();
            $$->type = OR_NODE;
            $$->children.push_back($1);
            $$->children.push_back($5);
            }
            |*/
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

command_word: arg {
    $$=new Node();
    $$->type = COMMAND_NODE;
    $$->obj = new Command();
    currentCommand = (Command *)$$->obj;
    currentArgs.push_back(make_arg_node(*$1));
    delete $1;
    }; /*| quote
    {
    $$=new Node();
    $$->type = COMMAND_NODE;
    $$->obj = new Command();
    currentCommand = (Command *)$$->obj;
    command_stack.push(currentCommand);
    currentCommand->commands.push_back(*$1);
    };*/
no_arg_command: command_word SPACE args {
    //for (auto i: args_vec) std::cout << i << std::endl;
    $$=$1;
    $$->children = currentArgs;
    currentArgs.clear();
}
| command_word {
    $$=$1;
    $$->children = currentArgs;
    currentArgs.clear();
};
args: 
    arg {
   if (($1)->length()) {
    currentArgs.push_back(make_arg_node(*$1));
    delete $1;
    }
    }
    |
   args SPACE arg {
   if (($3)->length()) {
    currentArgs.push_back(make_arg_node(*$3));
    delete $3;
    }
    }
    |
   ;
arg: arg word
{
$$->append(*$2);
delete $2;
} | 
arg quote {
$$->append(*$2);
delete $2;
} | 
arg cmd_subst {
$$->append(*$2);
delete $2;
}
|
{
$$=new string();
};

quote_word: quote_word ch {
          $$->push_back($2);
          } |
          quote_word SPACE {
          $$->push_back(' ');
          } | 
          quote_word AMPERSAND {
          $$->push_back('&');
          } | 
          quote_word PIPE {
          $$->push_back('|');
          } | 
          quote_word OR {
          $$->append("||");
          } | 
          quote_word AND {
          $$->append("&&");
          } | 
          quote_word GREAT {
          $$->append(">");
          } | 
          quote_word GREATGREAT {
          $$->append(">>");
          } | 
          quote_word GREATAND {
          $$->append(">&");
          } | 
          quote_word GREATGREATAND {
          $$->append(">>&");
          } | 
          quote_word IOERR {
          $$->append("2>");
          } | 
          quote_word LESS {
          $$->append("<");
          } | 
          quote_word LEFT_BRACE {
          $$->append("{");
          } | 
          quote_word RIGHT_BRACE {
          $$->append("}");
          } | 
          quote_word LEFT_PAREN {
          $$->append("(");
          } | 
          quote_word RIGHT_PAREN {
          $$->append(")");
          } | 
          {
          $$=new string();
          };
quote: DQUOTE quote_word DQUOTE {
     $$=$2;
} | SQUOTE quote_word SQUOTE {
     $$=$2;
};
word:  ch word {
    string ret = $1+ *$2;
    $$ = new string(ret.c_str());
    delete $2;
   }
    | {
   $$ = new string();
   }
    ;

cmd_subst: SUBSHELL space command_line RIGHT_PAREN {
        currentArgs.push_back(make_cmd_subst_node($3));
/*        AST *tr = new AST;
        tr->root = $3;
        int pipefd[2];
        pipe(pipefd);
        pid_t child;
        child = fork();
        if (child == 0) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
            exec(tr, p, tr->root, bPids, pos);
            exit(EXIT_SUCCESS);
        }
            close(pipefd[1]);
            char buf[4096];
            string line;
            ssize_t n_bytes;
            while ((n_bytes = read(pipefd[0], buf, 4095)) != 0) {
                buf[n_bytes] = '\0';
                char *temp = buf;
                while (*temp != '\0'){
                    if (*temp == '\n') {
                        line += ' ';
                    }
                    else {
                        line += *temp;
                    }
                    temp++;
                }
            }
            close(pipefd[0]);
            //cout << line << endl;
            delete tr;
            for (size_t i = line.size() - 1; i >= 0; i--) {
                myunput(line[i]);
                if (i == 0) break;
            }
    command_stack.pop();
    currentCommand = command_stack.top();
        $$ = new string(line);*/
}
ch: CHAR {
  $$=$1;
  }
  | ESCAPE_CHAR{
  $$=convertEscapeChar(*$1);
  delete $1;
  } ;

space: SPACE 
     | 
     ;

linebreak: newline_list
         |
         ;
newline_list: NEWLINE
            | newline_list NEWLINE
            ;

separator_op: AMPERSAND 
            | ';'
separator: separator_op linebreak
         | newline_list
         ;

%%

void yyerror(YYLTYPE *yyllocp, yyscan_t scan, AST **ast, const char *s) {
    cerr << s << endl;
}

