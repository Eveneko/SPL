#include "ir.hpp"

const int INFO_SIZE = 1024;
string addr2str(int addr){
    char buffer[INFO_SIZE];
    if (addr > 0){
        sprintf(buffer, "t%d", addr);
    }else{
        sprintf(buffer, "#%d", addr);
    }
    return buffer;
}

enum class Operator {
    PLUS_OPERATOR,
    MINUS_OPERATOR,
    MUL_OPERATOR,
    DIV_OPERATOR,
    LE_OPERATOR,
    LT_OPERATOR,
    GE_OPERATOR,
    GT_OPERATOR,
    NE_OPERATOR,
    EQ_OPERATOR,
};

string operator2str(Operator op){
    switch(op){
        case Operator::PLUS_OPERATOR: return "+";
        case Operator::MINUS_OPERATOR: return "-";
        case Operator::MUL_OPERATOR: return "*";
        case Operator::DIV_OPERATOR: return "/";
        case Operator::LE_OPERATOR: return "<=";
        case Operator::LT_OPERATOR: return "<";
        case Operator::GE_OPERATOR: return ">=";
        case Operator::GT_OPERATOR: return ">";
        case Operator::NE_OPERATOR: return "!=";
        case Operator::EQ_OPERATOR: return "==";
        default: exit(-2);
    }
}

void irinit(){
    tac_vector.clear();
    tac_vector.push_back(new TAC());
    table.clear();
}

/**
 * Program: ExtDefList
*/
void irProgram(AST *root){
    irinit();

    irExrDefList(root->child[0]);
    for(int i = 1; i < tac_vector.size(); ++i){
        fprintf(stdout, "%s\n", string(tac_vector[i]->to_string()).c_str());
    }
}

/**
 * ExtDefList: ExtDef ExtDefList | %empty
*/
void irExrDefList(AST *node){

}

/**
 * ExtDef: Specifier ExtDecList SEMI
 *       | Specifier SEMI
 *       | Specifier FunDec CompSt
*/
void irExtDef(AST *node){

}

/**
 * Specifier: TYPE
 * Specifier: StructSpecifier
*/
Type* irSpecifier(AST *node){

}

/**
 * 
*/
Type *irType(AST *node){

}

/**
 * StructSpecifier: STRUCT ID LC DefList RC
 * StructSpecifier: STRUCT ID
 */
Type *irStructSpecifier(AST *node){

}

/**
 * CompSt: LC DefList StmtList RC
*/
void irCompSt(AST *node){
    
}

/**
 * DefList: Def DefList
 * DefList: %empty
 */
void irDefList(AST *node){

}

/**
 * Specifier DecList SEMI
 */
void irDef(AST *node){

}

/**
 * Stmt: Exp SEMI
 * Stmt: CompSt
 * Stmt: RETURN Exp SEMI
 * Stmt: IF LP Exp RP Stmt
 * Stmt: WHILE LP Exp RP Stmt
 * Stmt: IF LP Exp RP Stmt ELSE Stmt
 */
void irStmt(AST *node){

}

/**
 * StmtList: Stmt StmtList
 * StmtList: %empty
 */
void irStmtList(AST *node){

}

/**
 * Dec: VarDec
 * Dec: VarDec ASSIGN Exp
 */
void irDec(AST *node, Type *type){

}

/**
 * VarDec: ID
 * VarDec: VarDec LB INT RB
 */
TAC* irVarDec(AST *node, Type* type){

}

/**
 * Exp: Exp ASSIGN Exp
 *    | Exp [{AND}|{OR}] Exp 
 *    | Exp [{LT}|{LE}|{GT}|{GE}|{NE}|{EQ}] Exp
 *    | Exp [{PLUS}|{MINUS}|{MUL}|{DIV}] Exp
 *    | LP Exp RP
 *    | MINUS Exp | MINUS Exp %prec UMINUS
 *    | NOT Exp
 *    | ID LP Args RP
 *    | ID LP RP
 *    | ID
 *    | Exp LB Exp RB
 *    | Exp DOT ID
 *    | INT | FLOAT | CHAR
 *    | READ LP RP
 */
int irExp(AST *node, bool single=false){

}

/**
 * VarList: ParamDec COMMA VarList
 * VarList: ParamDec
 */
void irVarList(AST *node){

}

/**
 * ParamDec: Specifier VarDec
 */
void irParamDec(AST *node){

}

/**
 * Args: Exp COMMA Args
 * Args: Exp
 */
vector<int> irArgs(AST *node){

}