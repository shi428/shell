%{
#include <bits/stdc++.h>
#include <misc.h>
using namespace std;
int yylex();
void yyerror(const char *s);
#define YYERROR_VERBOSE 1
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
%token <ch> LESS
%token <ch> DQUOTE
%token <ch> SQUOTE
%token <ch> LEFT_BRACE
%token <ch> RIGHT_BRACE
%token <ch> NEWLINE
%type <ch> ch
%type <str> arg;
%%

goal: arg NEWLINE {
    cout << *$1 << endl;
    delete $1;
}
arg: ch {
   string ret;
   ret += $1;
   $$ = new string(ret.c_str());
   }
   | ch arg {
    string ret = $1 + *$2;
    $$ = new string(ret.c_str());
    delete $2;
   }
ch: CHAR {
  $$=$1;
  }
  | ESCAPE_CHAR{
  $$=convertEscapeChar(*$1);
  delete $1;
  }
%%

void yyerror(const char *s) {
    cerr << s << endl;
}

