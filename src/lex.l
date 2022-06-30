%{
#include <string>
#include <ast.h>
#include "yacc.yy.hpp"
%}

%option reentrant bison-bridge bison-locations nounput
%%
[ \t]+ {
    return SPACE;
}
[\n] {
    return NEWLINE;
}
& { 
    return AMPERSAND;
}
&& { 
    return AND;
}
"|" { 
    return PIPE;
}
"||" { 
    return OR;
}
"$(" { 
    return SUBSHELL;
}
"<(" { 
    return LEFT_PROCESS_SUBST;
}
">(" { 
    return RIGHT_PROCESS_SUBST;
}
"${" { 
    return ENV;
}
"(" { 
    return LEFT_PAREN;
}
")" { 
    return RIGHT_PAREN;
}
">" { 
    return GREAT;
}
">&" { 
    return GREATAND;
}
">>" { 
    return GREATGREAT;
}
">>&" { 
    return GREATGREATAND;
}
"2>" { 
    return IOERR;
}
"<" { 
    return LESS;
}
"\"" { 
    return DQUOTE;
}
"\'" { 
    return SQUOTE;
}
"{" { 
    return LEFT_BRACE;
}
"}" {
    return RIGHT_BRACE;
}
[^ \t\n] { 
    yylval->ch = *yytext;
    return CHAR;
}
\\[^\n] { 
    yylval->str = new std::string(yytext);
    return ESCAPE_CHAR;
}
%%
int yywrap(yyscan_t scan) {
    return 1;
}
