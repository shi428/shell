/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_SRC_YACC_YY_HPP_INCLUDED
# define YY_YY_SRC_YACC_YY_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    CHAR = 258,
    ESCAPE_CHAR = 259,
    AMPERSAND = 260,
    PIPE = 261,
    SUBSHELL = 262,
    ENV = 263,
    LEFT_PAREN = 264,
    RIGHT_PAREN = 265,
    GREAT = 266,
    GREATAND = 267,
    GREATGREAT = 268,
    GREATGREATAND = 269,
    IOERR = 270,
    LESS = 271,
    DQUOTE = 272,
    SQUOTE = 273,
    LEFT_BRACE = 274,
    RIGHT_BRACE = 275,
    NEWLINE = 276
  };
#endif
/* Tokens.  */
#define CHAR 258
#define ESCAPE_CHAR 259
#define AMPERSAND 260
#define PIPE 261
#define SUBSHELL 262
#define ENV 263
#define LEFT_PAREN 264
#define RIGHT_PAREN 265
#define GREAT 266
#define GREATAND 267
#define GREATGREAT 268
#define GREATGREATAND 269
#define IOERR 270
#define LESS 271
#define DQUOTE 272
#define SQUOTE 273
#define LEFT_BRACE 274
#define RIGHT_BRACE 275
#define NEWLINE 276

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 10 "src/yacc.y"
char ch; std::string* str;

#line 102 "src/yacc.yy.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_YACC_YY_HPP_INCLUDED  */
