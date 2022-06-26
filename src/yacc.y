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
   #define YYERROR_VERBOSE 1
std::vector <std::string> args_vec;
   extern void myunput(int c);

pair <string *, vector <char> *> * create_arg();
void add_file_list(vector <Node *>**file_lists, int index, vector <Node*> file_list);
void copy_arg(pair <string *, vector <char> * > *argOne, pair<string *, vector <char> * > *argTwo);
void append_arg(pair <string *, vector <char> * > *argOne, pair<string *, vector <char> * > *argTwo);
void delete_arg(pair <string *, vector <char> *> *arg);
%}

%define api.pure full
%lex-param {yyscan_t scanner};
%parse-param {yyscan_t scanner};
%parse-param {AST ** ast};
%parse-param {int parse_quote};
%locations
%code requires{
    typedef void *yyscan_t;
};
%code {
int yylex(YYSTYPE *yylvalp, YYLTYPE *yylocp, yyscan_t scanner);
void yyerror(YYLTYPE *yylocp, yyscan_t, AST **ast, int, const char *);
};
%union {char ch; std::pair <std::string *, vector <char> *> *arg_str; std::string* str; Node *node; std::vector <Node *> *arg_list; std::vector <Node*>*file_list[6];}
%start goal
%token <ch> CHAR
%token <str> ESCAPE_CHAR
%token AMPERSAND
%token AND
%token PIPE
%token OR
%token SUBSHELL
%token LEFT_PROCESS_SUBST
%token RIGHT_PROCESS_SUBST
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
%type <str> ch
%type <str> word;
%type <node> no_arg_command;
%type <node> simple_command;
%type <node> simpler_command;
%type <node> command_line;
%type <node> full_command_line;
%type space;
%type <str> cmd_subst;
%type <str> left_process_subst;
%type <str> right_process_subst;
%type <arg_str> arg;
%type <arg_str> expansion;
%type <arg_list> args;
%type <str> quote_word;
%type <str> quote;
%type <file_list> iomodifier;
%type <file_list> simple_iomodifier;
%%

goal: input 
    input:
 space linebreak full_command_line space linebreak {
    AST *tr = new AST;
    *ast = tr;
    tr->root = $3;
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
              for (int i = 0; i < 6; i++) {
              for (auto j: *($2[i])) {
              ((Command *)$$->obj)->files[i].push_back(j);
              }
              delete $2[i];
              }
              };

simpler_command: no_arg_command simple_iomodifier {
               $$=$1;
              for (int i = 0; i < 6; i++) {
              for (auto j: *($2[i])) {
              ((Command *)$$->obj)->files[i].push_back(j);
              }
              delete $2[i];
              }
               };

simple_iomodifier: 
                 GREATAND space args space simple_iomodifier {
            for (int i = 0; i < 6; i++) {
            $$[i] = new std::vector <Node*>(*($5[i]));
            delete $5[i];
            }
            add_file_list($$, 3, *$3);
            }
          | GREATGREATAND space args space simple_iomodifier {
            for (int i = 0; i < 6; i++) {
            $$[i] = new std::vector <Node*>(*($5[i]));
            delete $5[i];
            }
            add_file_list($$, 5, *$3);
            }| {
            for (int i = 0; i < 6; i++) {
            $$[i] = new std::vector <Node*>();
            }
            }
            ;
iomodifier: 
          GREAT space args space iomodifier {
            for (int i = 0; i < 6; i++) {
            $$[i] = new std::vector <Node*>(*($5[i]));
            delete $5[i];
            }
            add_file_list($$, 1, *$3);
          }
          | LESS space args space iomodifier {
            for (int i = 0; i < 6; i++) {
            $$[i] = new std::vector <Node*>(*($5[i]));
            delete $5[i];
            }
            add_file_list($$, 0, *$3);
}
          | IOERR space args space iomodifier {
            for (int i = 0; i < 6; i++) {
            $$[i] = new std::vector <Node*>(*($5[i]));
            delete $5[i];
            }
            add_file_list($$, 2, *$3);
}
          | GREATGREAT space args space iomodifier {
            for (int i = 0; i < 6; i++) {
            $$[i] = new std::vector <Node*>(*($5[i]));
            delete $5[i];
            }
            add_file_list($$, 4, *$3);
}
          |
          {
            for (int i = 0; i < 6; i++) {
            $$[i] = new std::vector <Node*>();
            }
          };

no_arg_command: args {
              $$=new Node();
    $$->type = COMMAND_NODE;
    $$->obj = new Command();
    //vector <Node *>args(*$1);
    for (auto i: *$1) {
        $$->children.push_back(i);
    }
    delete $1;
    };

args: 
    arg {
   if ($1->first->length()) {
   $$ = new std::vector <Node*>();
   $$->push_back(make_arg_node(*($1->first), *($1->second)));
    }
    delete_arg($1);
    }
    |
   args SPACE arg {
   if (($3)->first->length()) {
   $$ = new std::vector <Node*>(*$1);
   $$->push_back(make_arg_node(*($3->first), *($3->second)));
    delete $1;
    }
    delete_arg($3);
    }
    |
   ;
arg: arg word
   {
$$ = create_arg();
copy_arg($$, $1);
$$->first->append(*$2);
for (auto i: *$2) {
$$->second->push_back(REGULAR);
}
delete_arg($1);
delete $2;
} | 
arg quote {
$$ = create_arg();
copy_arg($$, $1);
string quote_str(*$2);
$$->first->append(quote_str);
for (auto i: quote_str) {
$$->second->push_back(REGULAR);
}
delete_arg($1);
delete $2;
} | 
arg expansion {
$$ = create_arg();
copy_arg($$, $1);
append_arg($$, $2);
/*$$->first->append(*$2);
for (size_t i = 0; i < $2->length(); i++) {
    if (i == 0 || i == $2->length() - 1) {
    $$->second->push_back(COMMAND_SUBST);
    }
    else {
    $$->second->push_back(REGULAR);
    }
}
//delete_arg($1);
delete $2;*/
}
|
{
$$ = create_arg();
/*$$ = new pair <string *, vector<char> *>();
$$->first=new string();
$$->second=new vector<char>();*/
};

quote_word: quote_word ch {
          $$->append(*$2);
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
     if (parse_quote) {
     $$=$2;
     }
     else {
        $$ = new string("\""+*$2+"\"");
        delete $2;
     }
} | SQUOTE quote_word SQUOTE {
     if (parse_quote) {
     $$=$2;
     }
     else {
        $$ = new string("\'"+*$2+"\'");
        delete $2;
     }
     //$$=$2;
};
word:  ch word {
    string ret = *$1+ *$2;
    $$ = new string(ret.c_str());
    delete $2;
    delete $1;
   }
    | {
   $$ = new string();
   }
    ;

expansion: cmd_subst {
$$ = create_arg();
$$->first->append(*$1);
for (size_t i = 0; i < $1->length(); i++) {
    if (i == 0 || i == $1->length() - 1) {
    $$->second->push_back(COMMAND_SUBST);
    }
    else {
    $$->second->push_back(REGULAR);
    }
}
delete $1;
} | left_process_subst{
$$ = create_arg();
$$->first->append(*$1);
for (size_t i = 0; i < $1->length(); i++) {
    if (i == 0 || i == $1->length() - 1) {
    $$->second->push_back(READ_PROCESS_SUBST);
    }
    else {
    $$->second->push_back(REGULAR);
    }
}
delete $1;
} | right_process_subst{
$$ = create_arg();
$$->first->append(*$1);
for (size_t i = 0; i < $1->length(); i++) {
    if (i == 0 || i == $1->length() - 1) {
    $$->second->push_back(WRITE_PROCESS_SUBST);
    }
    else {
    $$->second->push_back(REGULAR);
    }
}
delete $1;
}

cmd_subst: SUBSHELL space linebreak full_command_line space linebreak RIGHT_PAREN {
         $$ = new string("$("+$4->get_command($4)+")");
}

left_process_subst: LEFT_PROCESS_SUBST space linebreak full_command_line space linebreak RIGHT_PAREN {
             $$= new string("<("+$4->get_command($4)+")");
             }
right_process_subst: RIGHT_PROCESS_SUBST space linebreak full_command_line space linebreak RIGHT_PAREN {
             $$= new string(">("+$4->get_command($4)+")");
}
ch: CHAR {
  $$=new string(1, $1);
  }
  | ESCAPE_CHAR{
  if (parse_quote) {
  $$=new string(1, convert_escape_char(*$1));
  delete $1;
  }
  else {
  $$ = new string(*$1);
  }
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

void yyerror(YYLTYPE *yyllocp, yyscan_t scan, AST **ast, int parse_quote, const char *s) {
    cerr << s << endl;
}

pair <string *, vector <char> *> * create_arg() {
    pair <string *, vector <char> *> *ret = new pair <string *, vector<char> *>();
    ret->first=new string();
    ret->second=new vector<char>();
    return ret;
}

void add_file_list(vector <Node *>**file_lists, int index, vector <Node*> file_list) {
    for (auto i: file_list) {
        file_lists[index]->push_back(i);
    }
}

void copy_arg(pair <string *, vector <char> * > *argOne, pair<string *, vector <char> * > *argTwo) {
    //delete old
    delete argOne->first;
    delete argOne->second;
    argOne->first = new string(*argTwo->first);
    argOne->second = new vector <char>(*argTwo->second);
}

void append_arg(pair <string *, vector <char> * > *argOne, pair<string *, vector <char> * > *argTwo) {
    argOne->first->append(*argTwo->first);
    for (auto i: *argTwo->second) {
        argOne->second->push_back(i);
    }
}
void delete_arg(pair <string *, vector <char> *> *arg) {
    delete arg->first;
    delete arg->second;
    delete arg;
}
