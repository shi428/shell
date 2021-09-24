%x src
%{
#include <bits/stdc++.h>
#include <parser.h>
#include "yacc.yy.hpp"
    static void yyunput(int c, char *buf);
    void myunput(int c);
%}

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
" source " {
    BEGIN(src);
}
<src>[^ \t\n]+ {
            std::string line;
            std::ifstream fin(yytext);
            FILE *yyin = fopen(yytext, "r+");
            if (!yyin) {
                BEGIN(INITIAL);
                std::cerr << "source: no such file or directory: " << yytext << std::endl;
            }
            else {
            fputc('\n', yyin);
            yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
            BEGIN(INITIAL);
            yyparse();
            yypop_buffer_state();
         /*   while (std::getline(fin, line)) {
                line += '\n';
                YY_BUFFER_STATE buffer = yy_scan_string((char *)line.c_str());
                yypush_buffer_state(buffer);
                //yypush_buffer_state(yy_scan_string((char *)line.c_str()));
                BEGIN(0); 
                yyparse();
                //yy_delete_buffer(buffer);
                yypop_buffer_state();
            }*/
            fclose(yyin);
            }
}
[^ \t\n] { 
    yylval.ch = *yytext;
    return CHAR;
}
\\[^\n] { 
    yylval.str = new std::string(yytext);
    return ESCAPE_CHAR;
}
%%
int yywrap() {
    return 1;
}

void myunput(int c) {
    unput(c);
}
