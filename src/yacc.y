%{
#include <bits/stdc++.h>
#include <misc.h>
using namespace std;
int yylex();
void yyerror(const char *s);
#define YYERROR_VERBOSE 1
std::vector <std::string> args_vec;
%}

%union {char ch; std::string* str;}
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
%%

goal: args NEWLINE {
    for (auto i: args_vec) cout << i << endl;
    args_vec.clear();
}
| SPACE args NEWLINE {
    for (auto i: args_vec) cout << i << endl;
    args_vec.clear();
}
  | NEWLINE 
  | error NEWLINE { yyerrok; }
 
;
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

