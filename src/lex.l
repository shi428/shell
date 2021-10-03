%x src
%{
#include <string>
#include <parser.h>
#include <unistd.h>
#include "yacc.yy.hpp"
    static void yyunput(int c, char *buf, yyscan_t scan);
    void myunput(int c);
   /* extern std::string readline();
    extern std::vector <std::string> history;
    extern unsigned ind;

    int mygetc(FILE *f) {
        static char *p;
        char ch;
        if (!isatty(0)) {
            return getc(f);
        }
        if (!p || !*p) {
            std::string s = readline();
            if (s.compare("\n")) {
            history.push_back(s);
            ind = history.size();
                }

            p = (char *)s.c_str();
        }
        ch = *p;
        p++;
        return ch;
    }*/
//#undef getc
//#define getc(f) mygetc(f)
%}

%option reentrant bison-bridge bison-locations
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

void myunput(int c) {
    //unput(c);
}

