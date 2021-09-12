%{
#include <bits/stdc++.h>
#include <tokenizer.h>
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
    return CHAR;
}
\\[^ \t\n] { 
    return ESCAPE_CHAR;
}
%%
int yywrap() {
    return 1;
}
