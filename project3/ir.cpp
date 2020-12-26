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
    while(node->child_num){
        irExtDef(node->child[0]);
        node = node->child[0];
    }
}

/**
 * ExtDef: Specifier ExtDecList SEMI
 *       | Specifier SEMI
 *       | Specifier FunDec CompSt
*/
void irExtDef(AST *node){
    Type *type = irSpecifier(node->child[0]);
    if(node->child[1]->type_name.compare("ExtDecList") == 0){
        irExtDecList(node->child[1], type);
    }
    if(node->child[1]->type_name.compare("FunDec") == 0){
        irFunc(node->child[1], type);
        irCompSt(node->child[2]);
    }
}

void irExtDecList(AST *node, Type *type) {
    TAC *tac = irVarDec(node->child[0], type);
    while(node->child_num > 1){
        node = node->child[2];
        TAC *tac = irVarDec(node->child[0], type);
    }
    putIR(tac->name, tac->genid());
}

/**
 * Specifier: TYPE
 * Specifier: StructSpecifier
*/
Type* irSpecifier(AST *node){
    Type *type;
    if(node->child[0]->type_name.compare("TYPE") == 0){
        type = irType(node->child[0]);
    }else{
        type = irStructSpecifier(node->child[0]);
    }
    return type;
}

/**
 * 
*/
Type *irType(AST *node){
    return checkType(node);
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
    irDefList(node->child[1]);
    irStmtList(node->child[2]);
}

/**
 * StmtList: Stmt StmtList
 * StmtList: %empty
 */
void irStmtList(AST *node){
    while(node->child_num){
        irStmt(node->child[0]);
        node = node->child[1];
    }
}

/**
 * DefList: Def DefList
 * DefList: %empty
 */
void irDefList(AST *node){
    while(node->child_num){
        irDef(node->child[0]);
        node = node->child[1];
    }
}

/**
 * Specifier DecList SEMI
 */
void irDef(AST *node){
    Type *type = irSpecifier(node->child[0]);
    irDecList(node->child[1], type);
}

/**
 * DecList: Dec | Dec COMMA DecList
*/
void irDecList(AST *node, Type *type){
    irDec(node->child[0], type);
    while(node->child_num > 1){
        node = node->child[2];
        irDec(node->child[0], type);
    }
}

/**
 * Stmt: Exp SEMI
 * Stmt: CompSt
 * Stmt: RETURN Exp SEMI
 * Stmt: IF LP Exp RP Stmt
 * Stmt: IF LP Exp RP Stmt ELSE Stmt
 * Stmt: WHILE LP Exp RP Stmt
 */
void irStmt(AST *node){
    // Exp SEMI
    if(node->child[0]->type_name.compare("Exp") == 0){
        irExp(node->child[0]);
    }
    // CompSt
    else if(node->child[0]->type_name.compare("CompSt") == 0){
        irCompSt(node->child[0]);
    }
    // RETURN Exp SEMI
    else if(node->child[0]->type_name.compare("RETURN") == 0){
        int expid = irExp(node->child[0]);
        genid(new ReturnTAC(tac_vector.size(), expid));
    }
    // IF
    else if(node->child[0]->type_name.compare("IF") == 0){
        int expid = irExp(node->child[0]);
        irStmt(node->child[4]);
        int tbranch = genid(new LabelTAC(tac_vector.size()));
        int jbranch;
        if(node->child_num >= 6){
            jbranch = genid(new GOTOTAC(tac_vector.size(), genlist()));
        }
        int fbranch = genid(new LabelTAC(tac_vector.size()));
        irIF(expid, tbranch, fbranch);
        if(node->child_num >= 6){
            irStmt(node->child[6]);
            int jbranchto = genid(new LabelTAC(tac_vector.size()));
            *dynamic_cast<GOTOTAC *>(tac_vector[jbranch])->label = jbranchto;
        }
    }
    // WHILE LP Exp RP Stmt
    else if(node->child[0]->type_name.compare("WHILE") == 0){
        
    }
}

/**
 * Dec: VarDec
 * Dec: VarDec ASSIGN Exp
 */
void irDec(AST *node, Type *type){
    TAC *tac = irVarDec(node->child[0], type);
    int expid = 0;
    if(node->child_num > 1){
        expid = irExp(node->child[2]);
    }
    if(expid){
        dynamic_cast<AssignTAC *>(tac)->right_address = expid;
    }
    putIR(tac->name, tac->genid());
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

int getIR(string name){
    return table[name];
}

void putIR(string name, int id){
    table[name] = id;
}

int genid(TAC *tac){
    int index = tac_vector.size();
    tac_vector.push_back(tac);
    return index;
}

int *genlist(int id){
    int *label = new int(id);
    return label;
}

void irIF(int id, int tbranch, int fbranch){
    if(tac_vector[id]->swap_flag){
        swap(tbranch, fbranch);
    }
    *dynamic_cast<IFTAC *>(tac_vector[id])->label = tbranch;
    *dynamic_cast<GOTOTAC *>(tac_vector[id + 1])->label = fbranch;
}

void irWHILE(){

}