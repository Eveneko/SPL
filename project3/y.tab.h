/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ERROE_INT = 258,
    ERROR_FLOAT = 259,
    ERROR_LEXEME = 260,
    INT = 261,
    CHAR = 262,
    FLOAT = 263,
    WHILE = 264,
    IF = 265,
    ELSE = 266,
    ID = 267,
    TYPE = 268,
    DOT = 269,
    COMMA = 270,
    ASSIGN = 271,
    STRUCT = 272,
    SEMI = 273,
    LP = 274,
    RP = 275,
    LC = 276,
    RC = 277,
    LB = 278,
    RB = 279,
    RETURN = 280,
    POUND = 281,
    QUOTE = 282,
    LITERAL = 283,
    INCLUDE = 284,
    PLUS = 285,
    MUL = 286,
    MINUS = 287,
    DIV = 288,
    NOT = 289,
    OR = 290,
    AND = 291,
    NE = 292,
    LT = 293,
    LE = 294,
    GT = 295,
    GE = 296,
    EQ = 297,
    WRITE = 298,
    READ = 299,
    BITOR = 300,
    BITXOR = 301,
    BITAND = 302,
    UMINUS = 303,
    BITWISE = 304
  };
#endif
/* Tokens.  */
#define ERROE_INT 258
#define ERROR_FLOAT 259
#define ERROR_LEXEME 260
#define INT 261
#define CHAR 262
#define FLOAT 263
#define WHILE 264
#define IF 265
#define ELSE 266
#define ID 267
#define TYPE 268
#define DOT 269
#define COMMA 270
#define ASSIGN 271
#define STRUCT 272
#define SEMI 273
#define LP 274
#define RP 275
#define LC 276
#define RC 277
#define LB 278
#define RB 279
#define RETURN 280
#define POUND 281
#define QUOTE 282
#define LITERAL 283
#define INCLUDE 284
#define PLUS 285
#define MUL 286
#define MINUS 287
#define DIV 288
#define NOT 289
#define OR 290
#define AND 291
#define NE 292
#define LT 293
#define LE 294
#define GT 295
#define GE 296
#define EQ 297
#define WRITE 298
#define READ 299
#define BITOR 300
#define BITXOR 301
#define BITAND 302
#define UMINUS 303
#define BITWISE 304

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 12 "syntax.y" /* yacc.c:1909  */

    class AST *ast_node;

#line 156 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
