%{
    #include "lex.yy.c"
    
    void yyerror(const char*);

    astNode *root;
%}

%locations
%error-verbose
%start Program
%union {
    struct astNode *_astNode;
    int intVal;
    float floatVal;
    char *strVal;
}

%token <_astNode> LEXEME_ERROR
%token <_astNode> END_OF_FILE

%token <_astNode> INT CHAR FLOAT
%token <_astNode> ID TYPE
%token <_astNode> STRUCT
%token <_astNode> IF ELSE WHILE
%token <_astNode> RETURN
%token <_astNode> DOT COMMA ASSIGN
%token <_astNode> SEMI
%token <_astNode> LP RP LC RC LB RB
%token <_astNode> PLUS MINUS MUL DIV
%token <_astNode> NOT OR AND NE LT LE GT GE EQ

%type <_astNode> ExtDefList ExtDef ExtDecList
%type <_astNode> Specifier StructSpecifier
%type <_astNode> VarDec FunDec VarList ParamDec
%type <_astNode> CompSt Stmt StmtList
%type <_astNode> DefList Def DecList Dec
%type <_astNode> Exp
%type <_astNode> Args

%nonassoc error
%right ASSIGN
%left OR
%left AND
%left NE
%left EQ
%left GE
%left GT
%left LE
%left LT
%left MINUS
%left PLUS
%left DIV
%left MUL
%right NOT
%left LP RP
%left LB RB
%left DOT

%%

/* high-level definition */
Program:
      ExtDefList { root = newAst("Program", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    ;
ExtDefList:
      ExtDef ExtDefList { $$ = newAst("ExtDefList", NON_TERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | %empty { $$ = newAst("ExtDefList", NON_TERMINAL, NULL, yylineno, 0); }
    ;
ExtDef:
      Specifier ExtDecList SEMI { $$ = newAst("ExtDef", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Specifier SEMI { $$ = newAst("ExtDef", NON_TERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | Specifier FunDec CompSt { $$ = newAst("ExtDef", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;
ExtDecList:
      VarDec { $$ = newAst("ExtDecList", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    | VarDec COMMA ExtDecList { $$ = newAst("ExtDecList", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;

/* specifier */
Specifier:
      TYPE { $$ = newAst("Specifier", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    | StructSpecifier { $$ = newAst("Specifier", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    ;
StructSpecifier:
      STRUCT ID LC DefList RC { $$ = newAst("StructSpecifier", NON_TERMINAL, NULL, @1.first_line, 5, $1, $2, $3, $4, $5); }
    | STRUCT ID { $$ = newAst("StructSpecifier", NON_TERMINAL, NULL, @1.first_line, 2, $1, $2); }
    ;

/* declarator */
VarDec:
      ID { $$ = newAst("VarDec", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    | VarDec LB INT RB { $$ = newAst("VarDec", NON_TERMINAL, NULL, @1.first_line, 4, $1, $2, $3, $4); }
    ;
FunDec:
      ID LP VarList error { ERROR_B(@1.last_line, "Missing closing parenthesis ')'"); }
    | ID LP VarList RP { $$ = newAst("FunDec", NON_TERMINAL, NULL, @1.first_line, 4, $1, $2, $3, $4); }
    | ID LP error { ERROR_B(@1.last_line, "Missing closing parenthesis ')'"); }
    | ID LP RP { $$ = newAst("FunDec", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;
VarList:
      ParamDec COMMA VarList { $$ = newAst("VarList", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | ParamDec { $$ = newAst("VarList", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    ;
ParamDec:
      Specifier VarDec { $$ = newAst("ParamDec", NON_TERMINAL, NULL, @1.first_line, 2, $1, $2); }
    ;

/* statement */
CompSt:
      LC DefList StmtList RC { $$ = newAst("CompSt", NON_TERMINAL, NULL, @1.first_line, 4, $1, $2, $3, $4); }
    | LC DefList StmtList DefList error { ERROR_B(@3.last_line, "Missing specifier"); }
    ;
StmtList:
      Stmt StmtList { $$ = newAst("StmtList", NON_TERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | %empty { $$ = newAst("StmtList", NON_TERMINAL, NULL, yylineno, 0); }
    ;
Stmt:
      Exp SEMI { $$ = newAst("Stmt", NON_TERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | CompSt { $$ = newAst("Stmt", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    | RETURN Exp error { ERROR_B(@1.last_line, "Missing semicolon ';'"); }
    | RETURN Exp SEMI { $$ = newAst("Stmt", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | IF LP Exp RP Stmt { $$ = newAst("Stmt", NON_TERMINAL, NULL, @1.first_line, 5, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt { $$ = newAst("Stmt", NON_TERMINAL, NULL, @1.first_line, 7, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt { $$ = newAst("Stmt", NON_TERMINAL, NULL, @1.first_line, 5, $1, $2, $3, $4, $5); }
    ;

/* local definition */
DefList:
      Def DefList { $$ = newAst("DefList", NON_TERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | %empty { $$ = newAst("DefList", NON_TERMINAL, NULL, yylineno, 0); }
    ;
Def:
      Specifier DecList error { ERROR_B(@1.last_line, "Missing semicolon ';'"); }
    | Specifier DecList SEMI { $$ = newAst("Def", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;
DecList:
      Dec { $$ = newAst("DecList", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    | Dec COMMA DecList { $$ = newAst("DecList", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;
Dec:
      VarDec { $$ = newAst("Dec", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    | VarDec ASSIGN Exp { $$ = newAst("Dec", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    ;

/* Expression */
Exp:
      Exp ASSIGN Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp AND Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp OR Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp LT Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp LE Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp GT Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp GE Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp NE Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp EQ Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp PLUS Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp MINUS Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp MUL Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp DIV Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | LP Exp RP { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | MINUS Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | NOT Exp { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 2, $1, $2); }
    | ID LP Args error { ERROR_B(@1.last_line, "Missing closing parenthesis ')'"); }
    | ID LP Args RP { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 4, $1, $2, $3, $4); }
    | ID LP RP { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp LB Exp RB { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 4, $1, $2, $3, $4); }
    | Exp DOT ID { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | ID { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    | INT { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    | FLOAT { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    | CHAR { $$ = newAst("Exp", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    | Exp LEXEME_ERROR Exp {}
    | LEXEME_ERROR {}
    ;
Args:
      Exp COMMA Args { $$ = newAst("Args", NON_TERMINAL, NULL, @1.first_line, 3, $1, $2, $3); }
    | Exp { $$ = newAst("Args", NON_TERMINAL, NULL, @1.first_line, 1, $1); }
    ;

%%

void init() {
    root = NULL;
    yylineno = 1;
    yycolno = 1;
    syntaxError = 0;
}

void yyerror(const char *s){
}

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(-1);
    }
    else if(!(yyin = fopen(argv[1], "r"))){
        perror(argv[1]);
        exit(-1);
    }
    init();
    yyparse();
    if(syntaxError == 0){
        printAst(root, 0);
    }
    return 0;
}