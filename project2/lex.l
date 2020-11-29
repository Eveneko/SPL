%{
    #include"y.tab.h"
    #include"semantic.hpp"
    #include <assert.h>
    // just let you know you have macros!
    #define EXIT_OK 0
    #define EXIT_FAIL 1

    #define YY_USER_ACTION \
        yylloc.first_line = yylineno; \
        yylloc.first_column = yycolno; \
        yylloc.last_line = yylineno; \
        yylloc.last_column = yycolno + yyleng; \
        yycolno += yyleng; 

    #define ERROR_TYPE_A() fprintf(stdout, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext); syntax_error = 1;
    #define ERROR_TYPE_B(lineno, msg) fprintf(stdout, "Error type B at Line %d: %s\n", lineno, msg); syntax_error = 1;
    #define DEBUG(syntax) fprintf(stdout, "%s(%d)\n", syntax, yylineno);

    #define DISPLAY_SYNTAX(syntax) // fprintf(stdout, "%s(%d)\n", syntax, yylineno);
    #define DISPLAY_TOKEN(token) // fprintf(stdout, "%s[%d](%s)\n", token, yycolno, yytext);
    #define LEXEME_ERROR() // fprintf(stderr, "Error lexeme %s at Line %d\n", yytext, yylineno); 
    #define SYNTAX_ERROR() // fprintf(stderr, "ERROR line %d: %s\n", yylloc.first_line, s);
        
    int yycolno = 1; 
    int syntax_error = 0;
%}

%option yylineno
%x C_COMMENT
int 0|([1-9][0-9]{0,9})|(-?0[xX][1-9a-fA-F][0-9a-fA-F]{0,7})|(0[xX]0)
wrong_int (-?[1-9][0-9]{9}[0-9]+)
float (0|[1-9][0-9]*)\.[0-9]+
letter [a-zA-Z_]
id [a-zA-Z_][0-9a-zA-Z_]*
wrong_id [0-9]{id}
char ('[ -~]')|('\\[xX][0-9a-fA-F]{1,2}')
wrong_char ('.+')
%%

"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>.    { }
<C_COMMENT>\n   { yycolno = 1; }

"//".*[\n\r] { yycolno = 1; }

(0[0-9]+)|(0[xX])|(0[xX][0-9a-fA-F]{8}[0-9a-fA-F]+) { DISPLAY_TOKEN("ERROR_INT"); return ERROE_INT; }
(0\.) { DISPLAY_TOKEN("ERROR_FLOAT"); return ERROR_FLOAT; }
{float} {
    /* floating point number (only dot-form) */
    DISPLAY_TOKEN("FLOAT");
    stringstream ss; 
    ss.str(yytext); 
    string value = ss.str();
    yylval.ast_node = new AST("FLOAT", SymbolType::TOKEN_VALUE, value, yylineno, 0);
    return FLOAT;
}
{wrong_int} {
    ERROR_TYPE_A();
    return INT;
}
{int} {
    /* integer in 32-bits (decimal or hexadecimal) */
    DISPLAY_TOKEN("INT");
    int int_value = 0;
    
    if (strlen(yytext) >= 3 && yytext[0] == '0') /* in hexidecimal form */
        sscanf(yytext, "%x", &int_value);
    else /* in decimal form */
        int_value = atoi(yytext);
    char *str_value = (char *)malloc(10*sizeof(char));
    sprintf(str_value, "%d", int_value);
    yylval.ast_node = new AST("INT", SymbolType::TOKEN_VALUE, str_value, yylineno, 0);
    return INT;
}
{char} {
    /* single character (printable or hex-form) */
    DISPLAY_TOKEN("CHAR");
    stringstream ss; 
    ss.str(yytext); 
    string value = ss.str();
    yylval.ast_node = new AST("CHAR", SymbolType::TOKEN_VALUE, value, yylineno, 0);
    return CHAR; 
}
{wrong_char} {
    LEXEME_ERROR();
    ERROR_TYPE_A();
    return CHAR;
}
"struct" {   
    DISPLAY_TOKEN("STRUCT"); 
    yylval.ast_node=new AST("STRUCT", SymbolType::TOKEN, "", yylineno, 0); 
    return STRUCT; 
}
"if" { 
    DISPLAY_TOKEN("IF"); 
    yylval.ast_node=new AST("IF", SymbolType::TOKEN, "", yylineno, 0); 
    return IF; 
}
"else" { 
    DISPLAY_TOKEN("ELSE"); 
    yylval.ast_node= new AST("ELSE", SymbolType::TOKEN, "", yylineno, 0);
    return ELSE; 
}
"while" {   
    DISPLAY_TOKEN("WHILE"); 
    yylval.ast_node= new AST("WHILE", SymbolType::TOKEN, "", yylineno, 0);
    return WHILE; 
}
"return" { 
    DISPLAY_TOKEN("RETURN");
    yylval.ast_node = new AST("RETURN", SymbolType::TOKEN, "", yylineno, 0);
    return RETURN;
}
"int" {
    DISPLAY_TOKEN("TYPE");
    yylval.ast_node = new AST("TYPE", SymbolType::TOKEN_VALUE, "int", yylineno, 0);
    return TYPE;
}
"float" {
    DISPLAY_TOKEN("TYPE");
    yylval.ast_node = new AST("TYPE", SymbolType::TOKEN_VALUE, "float", yylineno, 0);
    return TYPE;
}
"char" {
    DISPLAY_TOKEN("TYPE");
    yylval.ast_node = new AST("TYPE", SymbolType::TOKEN_VALUE, "char", yylineno, 0);
    return TYPE;
}
{wrong_id} {
    ERROR_TYPE_A();
    return ID; 
}
{id} {
    /* should be after type */
    DISPLAY_TOKEN("ID");
    stringstream ss;
    ss.str(yytext);
    string value = ss.str();
    yylval.ast_node = new AST("ID", SymbolType::TOKEN_VALUE, value, yylineno, 0);
    return ID;
}
"." { 
    DISPLAY_TOKEN("DOT");
    yylval.ast_node = new AST("DOT", SymbolType::TOKEN, "", yylineno, 0);
    return DOT;
}
";" { 
    DISPLAY_TOKEN("SEMI");
    yylval.ast_node = new AST("SEMI", SymbolType::TOKEN, "", yylineno, 0);
    return SEMI; 
}
"," { 
    DISPLAY_TOKEN("COMMA"); 
    yylval.ast_node = new AST("COMMA", SymbolType::TOKEN, "", yylineno, 0);
    return COMMA;
}
"=" {
    DISPLAY_TOKEN("ASSIGN");
    yylval.ast_node = new AST("ASSIGN", SymbolType::TOKEN, "", yylineno, 0);
    return ASSIGN;
}
"<" {
    DISPLAY_TOKEN("LT");
    yylval.ast_node = new AST("LT", SymbolType::TOKEN, "", yylineno, 0);
    return LT;
}
"<=" {
    DISPLAY_TOKEN("LE");
    yylval.ast_node = new AST("LE", SymbolType::TOKEN, "", yylineno, 0);
    return LE; 
}
">" {
    DISPLAY_TOKEN("GT");
    yylval.ast_node = new AST("GT", SymbolType::TOKEN, "", yylineno, 0);
    return GT;
}
">=" {
    DISPLAY_TOKEN("GE");
    yylval.ast_node = new AST("GE", SymbolType::TOKEN, "", yylineno, 0);
    return GE;
}
"!=" {
    DISPLAY_TOKEN("NE");
    yylval.ast_node = new AST("NE", SymbolType::TOKEN, "", yylineno, 0);
    return NE;
}
"==" {
    DISPLAY_TOKEN("EQ");
    yylval.ast_node = new AST("EQ", SymbolType::TOKEN, "", yylineno, 0);
    return EQ; 
}
"+" {
    DISPLAY_TOKEN("PLUS");
    yylval.ast_node = new AST("PLUS", SymbolType::TOKEN, "", yylineno, 0);
    return PLUS;
}
"-" {
    DISPLAY_TOKEN("MINUS");
    yylval.ast_node = new AST("MINUS", SymbolType::TOKEN, "", yylineno, 0);
    return MINUS;
}
"*" {
    DISPLAY_TOKEN("MUL");
    yylval.ast_node = new AST("MUL", SymbolType::TOKEN, "", yylineno, 0);
    return MUL;
}
"/" {
    DISPLAY_TOKEN("DIV");
    yylval.ast_node = new AST("DIV", SymbolType::TOKEN, "", yylineno, 0);
    return DIV;
}
"&&" {
    DISPLAY_TOKEN("AND");
    yylval.ast_node = new AST("AND", SymbolType::TOKEN, "", yylineno, 0);
    return AND;
}
"||" { 
    DISPLAY_TOKEN("OR");
    yylval.ast_node = new AST("OR", SymbolType::TOKEN, "", yylineno, 0);
    return OR; 
}
"!" { 
    DISPLAY_TOKEN("NOT");
    yylval.ast_node = new AST("NOT", SymbolType::TOKEN, "", yylineno, 0);
    return NOT;
}
"(" {
    DISPLAY_TOKEN("LP");
    yylval.ast_node = new AST("LP", SymbolType::TOKEN, "", yylineno, 0);
    return LP;
}
")" {
    DISPLAY_TOKEN("RP");
    yylval.ast_node = new AST("RP", SymbolType::TOKEN, "", yylineno, 0);
    return RP;
}
"[" { 
    DISPLAY_TOKEN("LB");
    yylval.ast_node = new AST("LB", SymbolType::TOKEN, "", yylineno, 0);
    return LB; 
}
"]" { 
    DISPLAY_TOKEN("RB"); 
    yylval.ast_node = new AST("RB", SymbolType::TOKEN, "", yylineno, 0);
    return RB; 
}
"{" {
    DISPLAY_TOKEN("LC");
    yylval.ast_node = new AST("LC", SymbolType::TOKEN, "", yylineno, 0);
    return LC;
}
"}" {
    DISPLAY_TOKEN("RC");
    yylval.ast_node = new AST("RC", SymbolType::TOKEN, "", yylineno, 0);
    return RC;
}
\n { yycolno = 1; }
[ \t]+ {}
. { 
    ERROR_TYPE_A();
    return ERROR_LEXEME; 
}
<<EOF>> {return 0;}

%%