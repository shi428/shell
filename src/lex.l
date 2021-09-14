%{
#include <bits/stdc++.h>
#include "yacc.yy.hpp"
%}

%option nounput
%%
[ \t] {
}
[\n] {
    return NEWLINE;
}
& { 
    return AMPERSAND;
}
"|" { 
    return PIPE;
}
"$(" { 
    return SUBSHELL;
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
">2" { 
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
    yylval.ch = *yytext;
    return CHAR;
}
\\[^ \t\n] { 
    yylval.str = new std::string(yytext);
    return ESCAPE_CHAR;
}
%%
int yywrap() {
    return 1;
}
