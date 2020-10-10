%{
    #include <ctype.h>
    #include "syntax.tab.h"
    #include "ast.h"   
    
    void yyerror(const char *s);
    
    #define YY_USER_ACTION \
        yylloc.first_line = yylineno; \
        yylloc.first_column = yycolno; \
        yylloc.last_line = yylineno; \
        yylloc.last_column = yycolno + yyleng; \
        yycolno += yyleng;
    #define ERRPR_A(msg) fprintf(stdout, "Error type A at Line %d: %s %s\n", yylineno, yytext); syntaxErrorNum = 1;
    #define ERROR_B(msg) fprintf(stdout, "Error type B at Line %d: %s\n", yylineno, msg); syntaxErrorNum = 1;
    int yycolno = 1;
    int syntaxErrorNum = 0;
    
%}

%option yylineno
digit [0-9]
decInt 0|(-?[1-9][0-9]*)
hexInt ^(0[xX]0|0[xX][1-9a-fA-F][1-9a-fA-F]{0,7})$
int {decInt}|{hexInt}
frac \.[0-9]+
float -?(0|[1-9][0-9]*){frac}
letter [a-zA-Z]
_letter [a-zA-Z_]
char \'{letter}\'
id {_letter}[{_letter}|{digit}]*

%%

{decInt} {
    int intVal = atoi(yytext);
    char *strVal = malloc(10);
    sprintf(strVal, "%d", intVal);
    yylval._astNode = newAst("INT", TOKEN_WITH_VALUE, strVal, yylineno, 0);
    return INT;
}
{hexInt} {
    char *strVal = malloc(strlen(yytext));
    strcpy(strVal, yytext);
    yylval._astNode = newAst("INT", TOKEN_WITH_VALUE, strVal, yylineno, 0);
    return INT;
}
{float} {
    char *strVal = malloc(strlen(yytext));
    strcpy(strVal, yytext);
    yylval._astNode = newAst("FLOAT", TOKEN_WITH_VALUE, strVal, yylineno, 0);
    return FLOAT;
}
{char} {
    char *strVal = malloc(strlen(yytext));
    strcpy(strVal, yytext);
    yylval._astNode = newAst("CHAR", TOKEN_WITH_VALUE, strVal, yylineno, 0);
    return CHAR;
}
{id} {
    char *strVal = malloc(strlen(yytext));
    strcpy(strVal, yytext);
    yylval._astNode = newAst("ID", TOKEN_WITH_VALUE, strVal, yylineno, 0);
    return ID;
}
"int" {
    yylval._astNode = newAst("TYPE", TOKEN_WITH_VALUE, "int", yylineno, 0);
    return TYPE;
}
"float" {
    yylval._astNode = newAst("TYPE", TOKEN_WITH_VALUE, "float", yylineno, 0);
    return TYPE;
}
"char" {
    yylval._astNode = newAst("TYPE", TOKEN_WITH_VALUE, "char", yylineno, 0);
    return TYPE;
}
"struct" {
    yylval._astNode = newAst("STRUCT", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return STRUCT;
}
"if" {
    yylval._astNode = newAst("IF", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return IF;
}
"else" {
    yylval._astNode = newAst("ELSE", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return ELSE;
}
"while" {
    yylval._astNode = newAst("WHILE", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return WHILE;
}
"return" {
    yylval._astNode = newAst("RETURN", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return RETURN;
}
"." {
    yylval._astNode = newAst("DOT", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return DOT;
}
";" {
    yylval._astNode = newAst("SEMI", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return SEMI;
}
"," {
    yylval._astNode = newAst("COMMA", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return COMMA;
}
"=" {
    yylval._astNode = newAst("ASSIGN", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return ASSIGN;
}
"<" {
    yylval._astNode = newAst("LT", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return LT;
}
"<=" {
    yylval._astNode = newAst("LE", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return LE;
}
">" {
    yylval._astNode = newAst("GT", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return GT;
}
">=" {
    yylval._astNode = newAst("GE", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return GE;
}
"!=" {
    yylval._astNode = newAst("NE", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return NE;
}
"==" {
    yylval._astNode = newAst("EQ", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return EQ;
}
"+" {
    yylval._astNode = newAst("PLUS", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return PLUS;
}
"-" {
    yylval._astNode = newAst("MINUS", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return MINUS;
}
"*" {
    yylval._astNode = newAst("MUL", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return MUL;
}
"/" {
    yylval._astNode = newAst("DIV", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return DIV;
}
"&&" {
    yylval._astNode = newAst("AND", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return AND;
}
"||" {
    yylval._astNode = newAst("OR", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return OR;
}
"!" {
    yylval._astNode = newAst("NOT", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return NOT;
}
"(" {
    yylval._astNode = newAst("LP", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return LP;
}
")" {
    yylval._astNode = newAst("RP", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return RP;
}
"[" {
    yylval._astNode = newAst("LB", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return LB;
}
"]" {
    yylval._astNode = newAst("RB", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return RB;
}
"{" {
    yylval._astNode = newAst("LC", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return LC;
}
"}" {
    yylval._astNode = newAst("RC", TOKEN_WITHOUT_VALUE, NULL, yylineno, 0);
    return RC;
}
[\n\r] {
    ++yylineno;
    yycolno = 1;
}
[ \t] {}
. {
    return LEXEME_ERROR;
}
<<EOF>> {
    return END_OF_FILE;
}

%%