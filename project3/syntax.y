%{
    extern "C" int yylex(void);

    #include"lex.yy.c"
    void yyerror(const char *s);
    AST *root;
%}

%locations
%error-verbose
%start Program
%union{
    class AST *ast_node;
}

%token ERROE_INT ERROR_FLOAT ERROR_LEXEME

%token <ast_node> INT CHAR FLOAT
%token <ast_node> WHILE IF ELSE
%token <ast_node> ID TYPE 
%token <ast_node> DOT COMMA ASSIGN
%token <ast_node> STRUCT
%token <ast_node> SEMI
%token <ast_node> LP RP LC RC LB RB
%token <ast_node> RETURN
%token <ast_node> POUND QUOTE LITERAL INCLUDE
%token <ast_node> PLUS MUL MINUS DIV
%token <ast_node> NOT OR AND NE LT LE GT GE EQ

%type <ast_node> ExtDefList ExtDef ExtDecList
%type <ast_node> Specifier StructSpecifier
%type <ast_node> VarDec FunDec VarList ParamDec
%type <ast_node> CompSt Stmt StmtList
%type <ast_node> DefList Def DecList Dec
%type <ast_node> Exp
%type <ast_node> Args

// project3
%token <ast_node> WRITE READ

%nonassoc error
%right ASSIGN
%left OR
%left AND
%left BITOR
%left BITXOR
%left BITAND
%left EQ NE
%left LE LT GE GT
%left PLUS MINUS
%left MUL DIV
%right NOT UMINUS BITWISE
%left LP RP LB RB
%right DOT

%%

/* high-level definition */
Program: 
      ExtDefList { 
    DISPLAY_SYNTAX("Program"); 
    root = new AST("Program", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1); 
}   ;
ExtDefList: 
      ExtDef ExtDefList { 
    DISPLAY_SYNTAX("ExtDefList-1");
    $$ = new AST("ExtDefList", SymbolType::NONTERMINAL, "", @1.first_line, 2, $1, $2);
}   | %empty { 
    DISPLAY_SYNTAX("ExtDefList-2");
    $$ = new AST("ExtDefList", SymbolType::NONTERMINAL, "", yylineno, 0);
}   ;
ExtDef: 
      Specifier ExtDecList SEMI { 
    DISPLAY_SYNTAX("ExtDef-1");
    $$ = new AST("ExtDef", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Specifier SEMI { 
    DISPLAY_SYNTAX("ExtDef-2"); 
    $$ = new AST("ExtDef", SymbolType::NONTERMINAL, "", @1.first_line, 2, $1, $2);
}   | Specifier FunDec CompSt { 
    DISPLAY_SYNTAX("ExtDef-3");
    $$ = new AST("ExtDef", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   ;
ExtDecList: 
      VarDec { 
    DISPLAY_SYNTAX("ExtDecList"); 
    $$ = new AST("ExtDecList", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   | VarDec COMMA ExtDecList { 
    DISPLAY_SYNTAX("ExtDecList");
    $$ = new AST("ExtDecList", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   ;

/* specifier */
Specifier: 
      TYPE { 
    DISPLAY_SYNTAX("Specifier");
    $$ = new AST("Specifier", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   | StructSpecifier { 
    DISPLAY_SYNTAX("Specifier");
    $$ = new AST("Specifier", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   ;
StructSpecifier: 
      STRUCT ID LC DefList RC {
    DISPLAY_SYNTAX("StructSpecifier"); 
    $$ = new AST("StructSpecifier", SymbolType::NONTERMINAL, "", @1.first_line, 5, $1, $2, $3, $4, $5);
}   | STRUCT ID { 
    DISPLAY_SYNTAX("StructSpecifier"); 
    $$ = new AST("StructSpecifier", SymbolType::NONTERMINAL, "", @1.first_line, 2, $1, $2);
}   ;

/* declarator */
VarDec: 
      ID {
    DISPLAY_SYNTAX("ID"); 
    $$ = new AST("VarDec", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   | VarDec LB INT RB { 
    DISPLAY_SYNTAX("ID"); 
    $$ = new AST("VarDec", SymbolType::NONTERMINAL, "", @1.first_line, 4, $1, $2, $3, $4);
}   ;
FunDec: 
      ID LP VarList error {
    DISPLAY_SYNTAX("FunDec");
    ERROR_TYPE_B(@1.last_line, "Missing closing parenthesis ')'");
}   | ID LP VarList RP { 
    DISPLAY_SYNTAX("FunDec");
    $$ = new AST("FunDec", SymbolType::NONTERMINAL, "", @1.first_line, 4, $1, $2, $3, $4);
}   | ID LP error {
    DISPLAY_SYNTAX("FunDec");
    ERROR_TYPE_B(@1.last_line, "Missing closing parenthesis ')'");
}   | ID LP RP { 
    DISPLAY_SYNTAX("FunDec");
    $$ = new AST("FunDec", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   ;
VarList: 
      ParamDec COMMA VarList {
    DISPLAY_SYNTAX("VarList");
    $$ = new AST("VarList", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | ParamDec { 
    DISPLAY_SYNTAX("VarList"); 
    $$ = new AST("VarList", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   ;
ParamDec: 
      Specifier VarDec { 
    DISPLAY_SYNTAX("ParamDec");
    $$ = new AST("ParamDec", SymbolType::NONTERMINAL, "", @1.first_line, 2, $1, $2);
}   ;

/* statement */
CompSt: 
      LC DefList StmtList RC { 
    DISPLAY_SYNTAX("CompSt"); 
    $$ = new AST("CompSt", SymbolType::NONTERMINAL, "", @1.first_line, 4, $1, $2, $3, $4);
}   | LC DefList StmtList DefList error {
    ERROR_TYPE_B(@3.last_line, "Missing specifier");
};
StmtList: 
      Stmt StmtList { 
    DISPLAY_SYNTAX("StmtList"); 
    $$ = new AST("StmtList", SymbolType::NONTERMINAL, "", @1.first_line, 2, $1, $2);
}   | %empty { 
    DISPLAY_SYNTAX("StmtList"); 
    $$ = new AST("StmtList", SymbolType::NONTERMINAL, "", yylineno, 0); 
}   ;
Stmt: 
      Exp SEMI { 
    DISPLAY_SYNTAX("Stmt");
    $$ = new AST("Stmt", SymbolType::NONTERMINAL, "", @1.first_line, 2, $1, $2);
}   | Exp error{
    ERROR_TYPE_B(@1.last_line, "Missing semicolon ';'");
}   | CompSt { 
    DISPLAY_SYNTAX("Stmt");
    $$ = new AST("Stmt", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   | RETURN SEMI error { 
    DISPLAY_SYNTAX("Stmt"); 
    ERROR_TYPE_B(@1.last_line, "Missing expression");
}   | RETURN Exp error { 
    DISPLAY_SYNTAX("Stmt"); 
    ERROR_TYPE_B(@1.last_line, "Missing semicolon ';'");
}   | RETURN Exp SEMI { 
    DISPLAY_SYNTAX("Stmt"); 
    $$ = new AST("Stmt", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | IF LP Exp RP Stmt { 
    DISPLAY_SYNTAX("Stmt");
    $$ = new AST("Stmt", SymbolType::NONTERMINAL, "", @1.first_line, 5, $1, $2, $3, $4, $5);
}   | IF LP Exp RP Stmt ELSE Stmt { 
    DISPLAY_SYNTAX("Stmt");
    $$ = new AST("Stmt", SymbolType::NONTERMINAL, "", @1.first_line, 7, $1, $2, $3, $4, $5, $6, $7);
}   | WHILE LP Exp RP Stmt { 
    DISPLAY_SYNTAX("Stmt");
    $$ = new AST("Stmt", SymbolType::NONTERMINAL, "", @1.first_line, 5, $1, $2, $3, $4, $5);
}   | WRITE LP Exp RP SEMI { 
    DISPLAY_SYNTAX("Stmt");
    $$ = new AST("Stmt", SymbolType::NONTERMINAL, "", @1.first_line, 5, $1, $2, $3, $4, $5); 
};

/* local definition */
DefList: 
      Def DefList {
    DISPLAY_SYNTAX("DefList");
    $$ = new AST("DefList", SymbolType::NONTERMINAL, "", @1.first_line, 2, $1, $2);
}   | %empty {
    DISPLAY_SYNTAX("DefList");
    $$ = new AST("DefList", SymbolType::NONTERMINAL, "", yylineno, 0);
}   ;
Def: 
      Specifier DecList SEMI {
    DISPLAY_SYNTAX("Def");
    $$ = new AST("Def", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Specifier DecList error {
    DISPLAY_SYNTAX("Def");
    ERROR_TYPE_B(@1.last_line, "Missing semicolon ';'");
}   ;
DecList: 
      Dec {
    DISPLAY_SYNTAX("DecList");
    $$ = new AST("DecList", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   | Dec COMMA DecList {
    DISPLAY_SYNTAX("DecList");
    $$ = new AST("DecList", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   ;
Dec: 
      VarDec {
    DISPLAY_SYNTAX("Dec");
    $$ = new AST("Dec", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   | VarDec ASSIGN Exp {
    DISPLAY_SYNTAX("Dec");
    $$ = new AST("Dec", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   ;

/* Expression */
Exp: 
      Exp ASSIGN Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp ERROR_LEXEME Exp
    | Exp AND Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp OR Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp LT Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp LE Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp GT Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp GE Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp NE Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp EQ Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp PLUS Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp MINUS Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp MUL Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp DIV Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | LP Exp RP {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | MINUS Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 2, $1, $2);
}   | MINUS Exp %prec UMINUS { 
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 2, $1, $2); 
}   | NOT Exp {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 2, $1, $2);
}   | ID LP Args error {
    DISPLAY_SYNTAX("Exp");
    ERROR_TYPE_B(@1.last_line, "Missing closing parenthesis ')'");
}   | ID LP Args RP {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 4, $1, $2, $3, $4);
}   | ID LP error {
    DISPLAY_SYNTAX("Exp");
    ERROR_TYPE_B(@1.last_line, "Missing closing parenthesis ')'");
}   | ID LP RP {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp LB Exp RB {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 4, $1, $2, $3, $4);
}   | Exp DOT ID {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | ID {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   | INT {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   | FLOAT {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   | CHAR {
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   | READ LP RP { 
    DISPLAY_SYNTAX("Exp");
    $$ = new AST("Exp", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3); 
}   | ERROR_LEXEME {
};
Args: 
      Exp COMMA Args {
    DISPLAY_SYNTAX("Args");
    $$ = new AST("Args", SymbolType::NONTERMINAL, "", @1.first_line, 3, $1, $2, $3);
}   | Exp {
    DISPLAY_SYNTAX("Args");
    $$ = new AST("Args", SymbolType::NONTERMINAL, "", @1.first_line, 1, $1);
}   ;

%%
void init() {
    root = NULL;
    yylineno = 1;
    yycolno = 1;
    syntax_error = 0;
}

void yyerror(const char *s) {
    SYNTAX_ERROR();
}

int main(int argc, char **argv) {
    char *file_path;
    if ( argc < 2 ) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return EXIT_FAIL;
    } else if( argc > 2 ){
        fprintf(stderr, "Too much arguments!\n");
        return EXIT_FAIL;
    }
    file_path = argv[1];
    if(!(yyin = fopen(file_path, "r"))){
        perror(argv[1]);
        return EXIT_FAIL;
    }
    init();
    yyparse();
    if (!syntax_error) {
        assert(root != NULL);
        checkProgram(root);
    }
    return EXIT_OK;
}