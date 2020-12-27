#include "ir.hpp"

const int INFO_SIZE = 1024;
string addr2str(int addr){
    char buffer[INFO_SIZE];
    if (addr > 0){
        sprintf(buffer, "t%d", addr);
    }else{
        sprintf(buffer, "#%d", -addr);
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

/**
 * Program: ExtDefList
*/
void irProgram(AST *root){
    irInit();

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
    string name = node->child[1]->value;
    auto it = global_type_map.find(name);
    Type *type = it->second;
    return type;
}

void irFunc(AST *node, Type *type){
    string name = node->child[0]->value;
    int funcid = genid(new FunctionTAC(tac_vector.size(), name));
    putIR(name, funcid);
    if (node->child[2]->type_name.compare("VarList") == 0) {
        irVarList(node->child[2]);
    }
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
vector<int> cont, br;

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
        int cont_size = cont.size();
        int br_size = br.size();
        int loop_start = genid(new LabelTAC(tac_vector.size()));
        int expid = irExp(node->child[2]);
        int tbranch = genid(new LabelTAC(tac_vector.size()));
        irStmt(node->child[4]);
        int loopbranch = genid(new GOTOTAC(tac_vector.size(), genlist(loop_start)));
        int fbranch = genid(new LabelTAC(tac_vector.size()));
        irIF(expid, tbranch, fbranch);
        irWHILE(&cont, cont_size, loop_start);
        irWHILE(&br, br_size, fbranch);
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
    vector<AST*> ast_vec;
    ast_vec.push_back(node);
    vector<int> int_vec;
    string name;
    while(!ast_vec.empty()){
        AST *top = ast_vec.back();
        if(top->child[0]->type_name.compare("ID") == 0){
            name = top->child[0]->value;
            ast_vec.pop_back();
            while(!ast_vec.empty()){
                AST *ast = ast_vec.back();
                ast_vec.pop_back();
                int size = atof(ast->child[2]->value.c_str());
                int_vec.push_back(size);
            }
        }else{
            ast_vec.push_back(top->child[0]);
        }
    }
    if(int_vec.size()){
        return new DecTAC(tac_vector.size(), type, name, int_vec);
    }else if(typeid(*type)==typeid(Structure_Type)){
        return new DecTAC(tac_vector.size(), type, name, {});
    }else{
        TAC *tac = new AssignTAC(tac_vector.size(), 0);
        tac->name = name;
        return tac;
    }
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
int irExp(AST *node, bool single){
    // Exp ASSIGN Exp
    if(node->child[1]->type_name.compare("ASSIGN") == 0){
        int lexpid = irExp(node->child[0]);
        int rexpid = irExp(node->child[2]);
        if(typeid(*tac_vector[lexpid]) == typeid(AssignTAC)){
            dynamic_cast<AssignTAC *>(tac_vector[lexpid])->right_address = rexpid;
        }else{
            dynamic_cast<CopyValueTAC *>(tac_vector[lexpid])->right_address = rexpid;
        }
        return rexpid;
    }
    // Exp [{AND}|{OR}] Exp
    if(node->child[1]->type_name.compare("AND") == 0){
        int lexpid = irExp(node->child[0]);
        int lswap_flag = tac_vector[lexpid]->swap_flag;
        int labelid = genid(new LabelTAC(tac_vector.size()));
        int rexpid = irExp(node->child[2]);
        int rswap_flag = tac_vector[rexpid]->swap_flag;
        if(lswap_flag){
            *dynamic_cast<GOTOTAC *>(tac_vector[lexpid+1])->label = labelid;
            if(rswap_flag){
                dynamic_cast<IFTAC *>(tac_vector[lexpid])->label = dynamic_cast<IFTAC *>(tac_vector[rexpid])->label;
            }else{
                dynamic_cast<IFTAC *>(tac_vector[lexpid])->label = dynamic_cast<GOTOTAC *>(tac_vector[rexpid+1])->label;
            }
        }else{
            *dynamic_cast<IFTAC *>(tac_vector[lexpid])->label = labelid;
            if(rswap_flag){
                dynamic_cast<GOTOTAC *>(tac_vector[lexpid+1])->label = dynamic_cast<IFTAC *>(tac_vector[rexpid])->label;
            }else{
                dynamic_cast<GOTOTAC *>(tac_vector[lexpid+1])->label = dynamic_cast<GOTOTAC *>(tac_vector[rexpid+1])->label;
            }
        }
        return rexpid;
    }
    if(node->child[1]->type_name.compare("OR") == 0){
        int lexpid = irExp(node->child[0]);
        int lswap_flag = tac_vector[lexpid]->swap_flag;
        int labelid = genid(new LabelTAC(tac_vector.size()));
        int rexpid = irExp(node->child[2]);
        int rswap_flag = tac_vector[rexpid]->swap_flag;
        if(lswap_flag){
            *dynamic_cast<IFTAC *>(tac_vector[lexpid])->label = labelid;
            if(rswap_flag){
                dynamic_cast<GOTOTAC *>(tac_vector[lexpid+1])->label = dynamic_cast<GOTOTAC *>(tac_vector[rexpid+1])->label;
            }else{
                dynamic_cast<GOTOTAC *>(tac_vector[lexpid+1])->label = dynamic_cast<IFTAC *>(tac_vector[rexpid])->label;
            }
        }else{
            *dynamic_cast<GOTOTAC *>(tac_vector[lexpid+1])->label = labelid;
            if(rswap_flag){
                dynamic_cast<IFTAC *>(tac_vector[lexpid])->label = dynamic_cast<GOTOTAC *>(tac_vector[rexpid+1])->label;
            }else{
                dynamic_cast<IFTAC *>(tac_vector[lexpid])->label = dynamic_cast<IFTAC *>(tac_vector[rexpid])->label;
            }
        }
        return rexpid;
    }
    // Exp [{LT}|{LE}|{GT}|{GE}|{NE}|{EQ}] Exp
    if(node->child[1]->type_name.compare("LT") == 0){
        int lexpid = irExp(node->child[0]);
        int rexpid = irExp(node->child[2]);
        IFTAC *iftac = new IFTAC(tac_vector.size(), Operator::LT_OPERATOR, lexpid, rexpid, genlist());
        int ifid = genid(iftac);
        GOTOTAC *gototac = new GOTOTAC(tac_vector.size(), genlist());
        int gotoid = genid(gototac);
        return ifid;
    }
    if(node->child[1]->type_name.compare("LE") == 0){
        int lexpid = irExp(node->child[0]);
        int rexpid = irExp(node->child[2]);
        IFTAC *iftac = new IFTAC(tac_vector.size(), Operator::LE_OPERATOR, lexpid, rexpid, genlist());
        int ifid = genid(iftac);
        GOTOTAC *gototac = new GOTOTAC(tac_vector.size(), genlist());
        int gotoid = genid(gototac);
        return ifid;
    }
    if(node->child[1]->type_name.compare("GT") == 0){
        int lexpid = irExp(node->child[0]);
        int rexpid = irExp(node->child[2]);
        IFTAC *iftac = new IFTAC(tac_vector.size(), Operator::GT_OPERATOR, lexpid, rexpid, genlist());
        int ifid = genid(iftac);
        GOTOTAC *gototac = new GOTOTAC(tac_vector.size(), genlist());
        int gotoid = genid(gototac);
        return ifid;
    }
    if(node->child[1]->type_name.compare("GE") == 0){
        int lexpid = irExp(node->child[0]);
        int rexpid = irExp(node->child[2]);
        IFTAC *iftac = new IFTAC(tac_vector.size(), Operator::GE_OPERATOR, lexpid, rexpid, genlist());
        int ifid = genid(iftac);
        GOTOTAC *gototac = new GOTOTAC(tac_vector.size(), genlist());
        int gotoid = genid(gototac);
        return ifid;
    }
    if(node->child[1]->type_name.compare("NE") == 0){
        int lexpid = irExp(node->child[0]);
        int rexpid = irExp(node->child[2]);
        IFTAC *iftac = new IFTAC(tac_vector.size(), Operator::NE_OPERATOR, lexpid, rexpid, genlist());
        int ifid = genid(iftac);
        GOTOTAC *gototac = new GOTOTAC(tac_vector.size(), genlist());
        int gotoid = genid(gototac);
        return ifid;
    }
    if(node->child[1]->type_name.compare("EQ") == 0){
        int lexpid = irExp(node->child[0]);
        int rexpid = irExp(node->child[2]);
        IFTAC *iftac = new IFTAC(tac_vector.size(), Operator::EQ_OPERATOR, lexpid, rexpid, genlist());
        int ifid = genid(iftac);
        GOTOTAC *gototac = new GOTOTAC(tac_vector.size(), genlist());
        int gotoid = genid(gototac);
        return ifid;
    }
    // Exp [{PLUS}|{MINUS}|{MUL}|{DIV}] Exp
    if(node->child[1]->type_name.compare("PLUS") == 0){
        int lexpid = irExp(node->child[0]);
        int rexpid = irExp(node->child[2]);
        ArithmeticTAC *tac = new ArithmeticTAC(tac_vector.size(), Operator::PLUS_OPERATOR, lexpid, rexpid);
        int id = genid(tac);
        return id;
    }
    if(node->child[1]->type_name.compare("MINUS") == 0){
        int lexpid = irExp(node->child[0]);
        int rexpid = irExp(node->child[2]);
        ArithmeticTAC *tac = new ArithmeticTAC(tac_vector.size(), Operator::MINUS_OPERATOR, lexpid, rexpid);
        int id = genid(tac);
        return id;
    }
    if(node->child[1]->type_name.compare("MUL") == 0){
        int lexpid = irExp(node->child[0]);
        int rexpid = irExp(node->child[2]);
        ArithmeticTAC *tac = new ArithmeticTAC(tac_vector.size(), Operator::MUL_OPERATOR, lexpid, rexpid);
        int id = genid(tac);
        return id;
    }
    if(node->child[1]->type_name.compare("DIV") == 0){
        int lexpid = irExp(node->child[0]);
        int rexpid = irExp(node->child[2]);
        ArithmeticTAC *tac = new ArithmeticTAC(tac_vector.size(), Operator::DIV_OPERATOR, lexpid, rexpid);
        int id = genid(tac);
        return id;
    }
    // LP Exp RP
    if(node->child[0]->type_name.compare("LP") == 0){
        return irExp(node->child[1]);
    }
    // MINUS Exp | MINUS Exp %prec UMINUS
    if(node->child[0]->type_name.compare("MINUS") == 0){
        int expid = irExp(node->child[1]);
        ArithmeticTAC *tac = new ArithmeticTAC(tac_vector.size(), Operator::MINUS_OPERATOR, 0, expid);
        int id = genid(tac);
        return id;
    }
    // NOT Exp
    if(node->child[0]->type_name.compare("NOT") == 0){
        int expid = irExp(node->child[1]);
        tac_vector[expid]->swap_flag ^= 1;
        return expid;
    }
    // ID LP Args RP
    // ID LP RP
    if(node->child[0]->type_name.compare("ID") == 0 && node->child_num == 1){
        string name = node->child[0]->value;
        if(node->child[2]->type_name.compare("Args") == 0){
            auto id_vec = irArgs(node->child[2]);
            for(auto id: id_vec){
                genid(new ArgTAC(tac_vector.size(), id));
            }
        }
        int id = genid(new CallTAC(tac_vector.size(), name));
        return id;
    }
    // ID
    if(node->child[0]->type_name.compare("ID") == 0 && node->child_num == 1){
        string name = node->child[0]->value;
        int id = getIR(name);
        int res_id = 0;
        if(single){
            if(!id){
                id = tac_vector.size();
                putIR(name, id);
            }
            res_id = genid(new AssignTAC(id, 0));
        }else if(!id){
            // specially
            res_id = genid(new AssignTAC(tac_vector.size(), 0));
        }else{
            res_id = id;
        }
        return res_id;
    }
    // Exp LB Exp RB
    if(node->child[1]->type_name.compare("LB") == 0){
        vector<AST *> vec;
        vec.push_back(node);
        int id;
        while(!vec.empty()){
            AST *top = vec.back();
            if(top->child[0]->type_name.compare("ID") == 0){
                id = irExp(top);
                vec.pop_back();
                int typeSize = tac_vector[id]->type->getSize();
                vector<int> *suffix;
                if(typeid(*tac_vector[id]) == typeid(DecTAC)){
                    suffix = &(dynamic_cast<DecTAC *>(tac_vector[id])->suffix);
                    id = genid(new AssignAddressTAC(tac_vector.size(), id));
                }else{
                    id = &(dynamic_cast<ParamTAC *>(tac_vector[id]->suffix));
                }
                int vec_size = vec.size();
                while(vec_size--){
                    AST *ast = vec.back();
                    vec.pop_back();
                    int offset = irExp(ast->child[2]);
                    offset = genid(new ArithmeticTAC(tac_vector.size(), Operator::MUL_OPERATOR, offset, -(*suffix)[vec_size] * typeSize));
                    id = genid(new ArithmeticTAC(tac_vector.size(), Operator::PLUS_OPERATOR, id, offset));
                }
            }else{
                vec.push_back(top->child[0]);
            }
        }
        if(single){
            return genid(new CopyValueTAC(id, 0));
        }else{
            return genid(new AssignValueTAC(tac_vector.size(), id));
        }
    }
    // Exp DOT ID
    if(node->child[1]->type_name.compare("DOT") == 0){
        vector<AST *> vec;
        vec.push_back(node);
        int id;
        while(!vec.empty()){
            AST *top = vec.back();
            if(top->child[0]->type_name.compare("ID") == 0){
                id = irExp(top);
                Type *type = tac_vector[id]->type;
                vec.pop_back();
                if(typeid(*tac_vector[id]) == typeid(DecTAC)){
                    id = genid(new AssignAddressTAC(tac_vector.size(), id));
                }
                int vec_size = vec.size();
                while(vec_size--){
                    AST *ast = vec.back();
                    vec.pop_back();
                    string name = ast->child[2]->value;
                    int offset = dynamic_cast<Structure_Type *>(type)->getOffset(name);
                    id = genid(new ArithmeticTAC(tac_vector.size(), Operator::PLUS_OPERATOR, id, -offset));
                }
            }else if(top->child_num == 3){
                vec.push_back(top->child[0]);
            }
        }
        if(single){
            return genid(new CopyValueTAC(id, 0));
        }else{
            return genid(new AssignValueTAC(tac_vector.size(), id));
        }
    }
    // INT | FLOAT | CHAR
    if(node->child[0]->type_name.compare("INT") == 0 ||
       node->child[0]->type_name.compare("CHAR") == 0 ||
       node->child[0]->type_name.compare("FLOAT") == 0){
        // value < 0, means not address
        AssignTAC *tac = new AssignTAC(tac_vector.size(), -atof(node->child[0]->value.c_str()));
        int id = genid(tac);
        return id;
    }
    // READ LP RP
    if(node->child[0]->type_name.compare("READ") == 0){
        ReadTAC *tac = new ReadTAC(tac_vector.size());
        int id = genid(tac);
        return id;
    }
}

/**
 * VarList: ParamDec COMMA VarList
 * VarList: ParamDec
 */
void irVarList(AST *node){
    vector<AST *> vec = {node->child[0]};
    while(node->child_num > 1){
        node = node->child[2];
        vec.push_back(node->child[0]);
    }
    while(!vec.empty()){
        irParamDec(vec.back());
        vec.pop_back();
    }
}

/**
 * ParamDec: Specifier VarDec
 */
void irParamDec(AST *node){
    Type *type = irSpecifier(node->child[0]);
    TAC *tac = irVarDec(node->child[1], type);
    if(typeid(*tac)==typeid(AssignTAC)){
        putIR(tac->name, genid(new ParamTAC(tac_vector.size(), type, {})));
    }else{
        putIR(tac->name, genid(new ParamTAC(tac_vector.size(), type, dynamic_cast<DecTAC *>(tac)->sizes)));
    }
}

/**
 * Args: Exp COMMA Args
 * Args: Exp
 */
vector<int> irArgs(AST *node){
    vector<int> arg_vec;
    int expid = irExp(node->child[0]);
    if(typeid(*tac_vector[expid]) == typeid(DecTAC)){
        expid = genid(new AssignAddressTAC(tac_vector.size(), expid));
    }
    arg_vec.push_back(expid);
    while(node->child_num > 1){
        node = node->child[2];
        expid = irExp(node->child[0]);
        arg_vec.push_back(expid);
        if(typeid(*tac_vector[expid]) != typeid(DecTAC)){
            expid = genid(new AssignAddressTAC(tac_vector.size(), expid));
        }
    }
    return arg_vec;
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
    *dynamic_cast<GOTOTAC *>(tac_vector[id+1])->label = fbranch;
}

void irWHILE(vector<int>* stat_vec, int end, int target){
    while(stat_vec->size() > end){
        int top = stat_vec->back();
        stat_vec->pop_back();
        *dynamic_cast<GOTOTAC *>(tac_vector[top])->label = target;
    }
}

void irInit(){
    tac_vector.clear();
    tac_vector.push_back(new TAC());
    table.clear();
    cont.clear();
    br.clear();
}