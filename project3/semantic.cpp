#include "semantic.hpp"

#define DEBUG(s, node)

multimap<string, Variable_Type*> var_map = multimap<string, Variable_Type*>();
multimap<string, Structure_Type*> type_map = multimap<string, Structure_Type*>();
multimap<string, Structure_Type*> global_type_map = multimap<string, Structure_Type*>();
void checkExtDefList(AST *node);
void checkExtDef(AST *node);
Type *checkSpecifier(AST *node);
Primitive_Type *checkType(AST *node);
Type *checkSructSpecifier(AST *node);
Variable_Type *checkFunc(AST *node, Type *type);
string checkID(AST *node);
void checkCompSt(AST *node, Type *type);
vector<Variable_Type *> checkDefList(AST *node);
vector<Variable_Type*> checkDef(AST *node);
vector<Variable_Type*> checkDecList(AST *node, Type *type);
void checkStmt(AST *node, Type *type);
void checkStmtList(AST *node, Type *type);
Variable_Type *checkDec(AST *node, Type *type);
Variable_Type *checkVarDec(AST *node, Type *type);
Type *checkExp(AST *node, bool single=false);
void updateVariable(Variable_Type *variable);
Variable_Type* getVariable(string name);
vector<Variable_Type*> checkVarList(AST *node);
Variable_Type * checkParamDec(AST *node);
vector<Type*> checkArgs(AST *node);
int checkINT(AST *node);

Variable_Type* getVariable(string identifier) {
    auto it = var_map.find(identifier);
    int count = var_map.count(identifier);
    if (count == 0) {
        return NULL;
    }
    assert(count == 1);
    for (int i = 0, len = var_map.count(identifier); i < len; ++i,++it) {
		return it->second;
	}
    return NULL;
}

void updateVariable(Variable_Type *variable) {
    Variable_Type *var = getVariable(variable->name);
    if (var) {
        if (variable->isfunction) {
            semantic_error(SemanticErrorType::REDEFINED_FUNCTION, variable->lineno, variable->name.c_str());
        } else {
            semantic_error(SemanticErrorType::REDEFINED_VARIABLE, variable->lineno, variable->name.c_str());
        }
    } else {
        var_map.insert(make_pair(variable->name, variable));
    }
}

Structure_Type *getStructure(string identifier) {
    auto it = type_map.find(identifier);
    int count = type_map.count(identifier);
    if (count == 0) {
        return NULL;
    }
    assert(count == 1);
    for (int i = 0, len = type_map.count(identifier); i < len; ++i,++it) {
		return it->second;
	}
    return NULL;
}

void updateStructure(Structure_Type *structure) {
    Structure_Type *struc = getStructure(structure->name);
    if (!struc) {
        type_map.insert(make_pair(structure->name, structure));
        global_type_map.insert(make_pair(structure->name, structure));
    } else {
        semantic_error(SemanticErrorType::REDEFINED_STRUCTURE, structure->lineno, structure->name.c_str());
    }
}

void semantic_error(SemanticErrorType error_type, ...) {
    va_list args;
    va_start(args, error_type);
    switch (error_type){ 
        case SemanticErrorType::UNDEFINED_VARIABLE:
            vfprintf(stdout, "Error type 1 at Line %d: undefined variable: %s\n", args);
            break;
        case SemanticErrorType::UNDEFINED_FUNCTION:
            vfprintf(stdout, "Error type 2 at Line %d: undefined function: %s\n", args);
            break;
        case SemanticErrorType::REDEFINED_VARIABLE:
            vfprintf(stdout, "Error type 3 at Line %d: redefine variable: %s\n", args);
            break;
        case SemanticErrorType::REDEFINED_FUNCTION:
            vfprintf(stdout, "Error type 4 at Line %d: redefine function: %s\n", args);
            break;
        case SemanticErrorType::UNMATCHING_TYPE_OF_ASSIGNMENT:
            vfprintf(stdout, "Error type 5 at Line %d: unmatching type on both sides of assignment\n", args);
            break;
        case SemanticErrorType::ASSIGN_TO_RAW_VALUE:
            vfprintf(stdout, "Error type 6 at Line %d: left side in assignment is rvalue\n", args);
            break;
        case SemanticErrorType::BINARY_OPERATION_ON_NONE_NUMBER_VARIABLE:
            vfprintf(stdout, "Error type 7 at Line %d: binary operation on non-number variables\n", args);
            break;
        case SemanticErrorType::RETURN_VALUE_MISMATCH:
            vfprintf(stdout, "Error type 8 at Line %d: incompatiable return type\n", args);
            break;
        case SemanticErrorType::INVALID_ARGUMENT_NUMBER:
            vfprintf(stdout, "Error type 9 at Line %d: invalid argument number for compare, expect %d, got %d\n", args);
            break;
        case SemanticErrorType::INDEXING_NONE_ARRAY_VARIABLE:
            vfprintf(stdout, "Error type 10 at Line %d: indexing on non-array variable\n", args);
            break;
        case SemanticErrorType::INVOKING_NONE_FUNCTION_VARIABLE:
            vfprintf(stdout, "Error type 11 at Line %d: invoking non-function variable: %s\n", args);
            break;
        case SemanticErrorType::INVALID_INDEXING:
            vfprintf(stdout, "Error type 12 at Line %d: indexing by non-integer\n", args);
            break;
        case SemanticErrorType::ACCESS_NONE_STRUCT_VARIABLE:
            vfprintf(stdout, "Error type 13 at Line %d: accessing with non-struct variable\n", args);
            break;
        case SemanticErrorType::ACCESS_NONE_EXIST_MEMBER:
            vfprintf(stdout, "Error type 14 at Line %d: no such member: %s\n", args);
            break;
        case SemanticErrorType::REDEFINED_STRUCTURE:
            vfprintf(stdout, "Error type 15 at Line %d: redefine struct: %s\n", args);
            break;
        default:
            assert(false);
            break;
    }
}

bool typecheck(Type *left, Type*right, bool weak, bool allow_empty) {
    if (allow_empty&&(*left==*EMPTYTYPE || *right==*EMPTYTYPE))
        return true;
    else if (weak && typeid(*left) == typeid(Primitive_Type) && typeid(*right) == typeid(Primitive_Type))
        return true;
    else
        return (*left == *right);
}
Type *EMPTYTYPE = new Type();
Variable_Type *EMPTYVAR = new Variable_Type("$", EMPTYTYPE);

void checkProgram(AST *root) {
    DEBUG("checkProgram", root);
    checkExtDefList(root->child[0]);
}

void checkExtDefList(AST *node) {
    DEBUG("checkExtDefList", node);
    switch (node->child_num) {
        case 0: 
            return;
        case 2:
            assert(node->child.size() == 2);
            checkExtDef(node->child[0]);
            checkExtDefList(node->child[1]);
            break;
        default:
            assert(false && "checkExtDefList Failed");
    }
}

void report_semantic_error(const char *s,...) {
    va_list args;
    va_start(args, s);
    vfprintf(stdout, s, args);
    fprintf(stdout, "\n");
}

/**
 * ExtDef: Specifier ExtDecList SEMI
 * ExtDef: Specifier SEMI
 * ExtDef: Specifier FunDec CompSt
 */
void checkExtDef(AST *node) {
    DEBUG("checkExtDef", node);
    assert(node->child[0]->type_name.compare("Specifier") == 0);
    Type *type = checkSpecifier(node->child[0]);
    if (node->child[1]->type_name.compare("ExtDecList") == 0) {
        // ExtDef: Specifier ExtDecList SEMI
        assert(false && "ExtDef: Specifier ExtDecList SEMI");
    }
    if (node->child[1]->type_name.compare("SEMI") == 0) {
        // ExtDef: Specifier SEMI
        return;
    }
    if (node->child[1]->type_name.compare("FunDec") == 0) {
        // ExtDef: Specifier FunDec CompSt
        Variable_Type *func_variable = checkFunc(node->child[1], type);
        updateVariable(func_variable);
        checkCompSt(node->child[2], type);
        return;
    }
    assert(false && "checkExtDef Failed");
}

/**
 * Specifier: TYPE
 * Specifier: StructSpecifier
 */
Type *checkSpecifier(AST *node) {
    DEBUG("parseSpecifier", node);
    if (node->child[0]->type_name.compare("TYPE") == 0) {
        // Specifier: TYPE
        return checkType(node->child[0]);
    }
    if (node->child[0]->type_name.compare("StructSpecifier") == 0) {
        // Specifier: StructSpecifier
        return checkSructSpecifier(node->child[0]);
    }
}

Primitive_Type *checkType(AST *node) {
    DEBUG("checkType", node);
    if (node->value.compare("int") == 0) {
        return new Primitive_Type(TokenType::INT_T, node->lineno);
    }
    if (node->value.compare("float") == 0) {
        return new Primitive_Type(TokenType::FLOAT_T, node->lineno);
    }
    if (node->value.compare("char") == 0) {
        return new Primitive_Type(TokenType::CHAT_T, node->lineno);
    }
    assert(false && "checkType Failed");
}

/**
 * StructSpecifier: STRUCT ID LC DefList RC
 * StructSpecifier: STRUCT ID
 */
Type *checkSructSpecifier(AST *node) {
    DEBUG("checkSructSpecifier", node);
    assert(node->child[0]->type_name.compare("STRUCT") == 0);
    assert(node->child[1]->type_name.compare("ID") == 0);
    string identifier = checkID(node->child[1]);
    if (node->child_num == 2) {
        Type *struct_type = getStructure(identifier);
        assert(struct_type && "struct_type must has defined");
        return struct_type;
    } else if (node->child_num == 5) {
        assert(node->child[2]->type_name.compare("LC") == 0);
        assert(node->child[3]->type_name.compare("DefList") == 0);
        assert(node->child[4]->type_name.compare("RC") == 0);
        vector<Variable_Type*> variables = checkDefList(node->child[3]);
        Structure_Type *struct_type = new Structure_Type(identifier, variables, node->lineno);
        updateStructure(struct_type);
        return struct_type;
    }
    assert(false && "checkSructSpecifier Failed");
}

/**
 * FunDec: ID LP VarList RP
 * FunDec: ID LP RP
 */
Variable_Type *checkFunc(AST *node, Type *type) {
    DEBUG("checkFunc", node);
    assert(node->child[0]->type_name.compare("ID") == 0);
    assert(node->child[1]->type_name.compare("LP") == 0);
    string identifier = checkID(node->child[0]);
    if (node->child[2]->type_name.compare("VarList") == 0) {
        vector<Variable_Type *> variables = checkVarList(node->child[2]);
        return new Variable_Type(identifier, type, variables, true, node->lineno);
    }
    if (node->child[2]->type_name.compare("RP") == 0) {
        return new Variable_Type(identifier, type, vector<Variable_Type*>(), true, node->lineno);
    } 
    assert(false && "checkFunc Failed");
}

string checkID(AST *node) {
    DEBUG("checkID", node);
    return node->value;
}

/**
 * CompSt: LC DefList StmtList RC
 */
void checkCompSt(AST *node, Type *type) {
    DEBUG("checkCompSt", node);
    assert(node->child[0]->type_name.compare("LC") == 0);
    assert(node->child[1]->type_name.compare("DefList") == 0);
    assert(node->child[2]->type_name.compare("StmtList") == 0);
    assert(node->child[3]->type_name.compare("RC") == 0);
    checkDefList(node->child[1]);
    checkStmtList(node->child[2], type);
    return;
}

/**
 * DefList: Def DefList
 * DefList: %empty
 */
vector<Variable_Type*> checkDefList(AST *node) {
    DEBUG("checkDefList", node);
    if (node->child_num == 0) {
        // DefList: %empty
        return vector<Variable_Type*>();
    }
    assert(node->child[0]->type_name.compare("Def") == 0);
    assert(node->child[1]->type_name.compare("DefList") == 0);
    vector<Variable_Type*> vari_left = checkDef(node->child[0]);
    vector<Variable_Type*> vari_right = checkDefList(node->child[1]);
    vari_left.insert(vari_left.end(), vari_right.begin(), vari_right.end());
    return vari_left;
}

/**
 * Specifier DecList SEMI
 */
vector<Variable_Type*> checkDef(AST *node) {
    DEBUG("checkDef", node);
    assert(node->child[0]->type_name.compare("Specifier") == 0);
    assert(node->child[1]->type_name.compare("DecList") == 0);
    assert(node->child[2]->type_name.compare("SEMI") == 0);
    Type *type = checkSpecifier(node->child[0]);
    vector<Variable_Type*> variables = checkDecList(node->child[1], type);
    return variables;
}

/**
 * DecList: Dec
 * DecList: Dec COMMA DecList
 */
vector<Variable_Type*> checkDecList(AST *node, Type *type) {
    DEBUG("checkDecList", node);
    assert(node->type_name.compare("DecList") == 0);
    assert(node->child[0]->type_name.compare("Dec") == 0);
    vector<Variable_Type*> variables = vector<Variable_Type*>();
    Variable_Type *variable = checkDec(node->child[0], type);
    variables.push_back(variable);
    if (node->child_num == 1) {
        return variables;
    } else if (node->child_num == 3) {
        vector<Variable_Type*> next_variables = checkDecList(node->child[2], type);
        variables.insert(variables.end(), next_variables.begin(), next_variables.end());
        return variables;
    }
    assert(false && "checkDecList Failed");
}

/**
 * Dec: VarDec
 * Dec: VarDec ASSIGN Exp
 */
Variable_Type *checkDec(AST *node, Type *type) {
    DEBUG("checkDec", node);
    assert(node->type_name.compare("Dec") == 0);
    assert(node->child[0]->type_name.compare("VarDec") == 0);
    Variable_Type *variable = checkVarDec(node->child[0], type);
    if (node->child_num == 1) {
        // Dec: VarDec
        updateVariable(variable);
    } else if (node->child_num == 3) {
        // Dec: VarDec ASSIGN Exp
        assert(node->child[1]->type_name.compare("ASSIGN") == 0);
        assert(node->child[2]->type_name.compare("Exp") == 0);
        Type *returnType = checkExp(node->child[2]);
        if (!typecheck(type, returnType, true)) {
            report_semantic_error("Error type 5 at line %d: unmatching types on both sides of assignment operator", node->child[1]->lineno);
        } else {
            updateVariable(variable);
        }

    }
    return variable;
}

/**
 * VarDec: ID
 * VarDec: VarDec LB INT RB
 */
Variable_Type *checkVarDec(AST *node, Type *type) {
    DEBUG("checkVarDec", node);
    assert(node->type_name.compare("VarDec") == 0);
    if (node->child_num == 1) {
        assert(node->child[0]->type_name.compare("ID") == 0);
        string identifier = checkID(node->child[0]);
        return new Variable_Type(identifier, type, {}, 0, node->lineno);
    } else if (node->child_num == 4) {
        assert(node->child[0]->type_name.compare("VarDec") == 0);
        assert(node->child[1]->type_name.compare("LB") == 0);
        assert(node->child[2]->type_name.compare("INT") == 0);
        assert(node->child[3]->type_name.compare("RB") == 0);
        Variable_Type *variable_type = checkVarDec(node->child[0], type);
        int size = checkINT(node->child[2]);
        variable_type->type = new Array_Type(variable_type->type, size);
        return variable_type; 
    }
    printf("node->child_num = %d\n", node->child_num);
    assert(false && "checkVarDec Failed");
}

int checkINT(AST *node) {
    return atoi(node->value.c_str());
}

Type *checkExp(AST *node, bool single) {
    DEBUG("checkVarDec", node);
    assert(node->type_name.compare("Exp") == 0);
    if (node->child_num == 1) {
        // CHAR
        if (node->child[0]->type_name.compare("ID") == 0) {
            string identifier = checkID(node->child[0]);
            Variable_Type* variable = getVariable(identifier);
            if (!variable) {
                semantic_error(SemanticErrorType::UNDEFINED_VARIABLE, node->lineno, identifier.c_str());
                return EMPTYTYPE;
            }
            assert(variable && "checkExp Failed");
            return variable->type;
        }
        // ID | INT | FLOAT
        if (single) {
            semantic_error(SemanticErrorType::ASSIGN_TO_RAW_VALUE, node->child[0]->lineno);
        }
        if (node->child[0]->type_name.compare("INT") == 0) {
            return new Primitive_Type(TokenType::INT_T, node->lineno);
        }
        if (node->child[0]->type_name.compare("FLOAT") == 0) {
            return new Primitive_Type(TokenType::FLOAT_T, node->lineno);
        }
        if (node->child[0]->type_name.compare("CHAR") == 0) {
            return new Primitive_Type(TokenType::CHAT_T, node->lineno);
        }
        assert(false && "checkExp Failed");
    } else if (node->child_num == 2) {
        // MINUS Exp
        // NOT Exp
        assert(node->child[1]->type_name.compare("Exp") == 0);
        if (node->child[0]->type_name.compare("MINUS") == 0) {
            Type *returnType = checkExp(node->child[1]);
            return returnType;
        } else if (node->child[0]->type_name.compare("NOT") == 0) {
            assert(false && "checkExp Failed");
        } 
    } else if (node->child_num == 3) {
        // Exp ASSIGN Exp
        if (node->child[1]->type_name.compare("ASSIGN") == 0) {
            assert(node->child[0]->type_name.compare("Exp") == 0);
            assert(node->child[2]->type_name.compare("Exp") == 0);
            Type *targetType = checkExp(node->child[0], true);
            Type *returnType = checkExp(node->child[2]);
            if (!typecheck(targetType, returnType, false, false)) {
                semantic_error(SemanticErrorType::UNMATCHING_TYPE_OF_ASSIGNMENT, node->child[1]->lineno); 
            }
            return EMPTYTYPE;
        }
        // Exp AND Exp
        // Exp OR Exp
        // Exp LT Exp
        // Exp LE Exp
        // Exp GT Exp
        // Exp GE Exp
        // Exp NE Exp
        // Exp EQ Exp
        // Exp PLUS Exp
        // Exp MINUS Exp
        // Exp MUL Exp
        // Exp DIV Exp
        if (node->child[2]->type_name.compare("Exp") == 0) {
            assert(node->child[0]->type_name.compare("Exp") == 0);
            Type *expType1 = checkExp(node->child[0]);
            Type *expType2 = checkExp(node->child[2]);
            if (!typecheck(expType1, expType2, false, false)) {
                semantic_error(SemanticErrorType::BINARY_OPERATION_ON_NONE_NUMBER_VARIABLE, node->child[1]->lineno);
                return EMPTYTYPE;
            }
            return expType1;
        }
        // Exp DOT ID
        if (node->child[1]->type_name.compare("DOT") == 0) {
            assert(node->child[0]->type_name.compare("Exp") == 0);
            assert(node->child[2]->type_name.compare("ID") == 0);
            Type *expType1 = checkExp(node->child[0]);
            string identifier = checkID(node->child[2]);
            if (Structure_Type* struct_type = dynamic_cast<Structure_Type*>(expType1)) {
                for (auto i: struct_type->field) {
                    if (i->name.compare(identifier) == 0) {
                        return i->type;
                    }
                }
                semantic_error(SemanticErrorType::ACCESS_NONE_EXIST_MEMBER, node->child[1]->lineno, identifier.c_str());
                return EMPTYTYPE;
            } else {
                semantic_error(SemanticErrorType::ACCESS_NONE_STRUCT_VARIABLE, node->child[1]->lineno);
                return EMPTYTYPE;
            }
            
        }
        // LP Exp RP
        if (node->child[0]->type_name.compare("LP") == 0) {
            assert(node->child[1]->type_name.compare("Exp") == 0);
            assert(node->child[2]->type_name.compare("RP") == 0);
            Type *expType = checkExp(node->child[1]);
            return expType;
        }
        // ID LP RP
        if (node->child[0]->type_name.compare("ID") == 0) {
            assert(node->child[1]->type_name.compare("LP") == 0);
            assert(node->child[2]->type_name.compare("RP") == 0);
            string identifier = checkID(node->child[0]);
            Variable_Type *func_variable = getVariable(identifier);
            assert(func_variable && func_variable->isfunction);
            return func_variable->type;
            assert(false && "checkExp Failed");
        }
        assert(false && "checkExp Failed");
    } else if (node->child_num == 4) {
        if (node->child[0]->type_name.compare("ID") == 0) {
            // ID LP Args RP
            assert(node->child[1]->type_name.compare("LP") == 0);
            assert(node->child[2]->type_name.compare("Args") == 0);
            assert(node->child[3]->type_name.compare("RP") == 0);
            string identifier = checkID(node->child[0]);
            Variable_Type *func_variable = getVariable(identifier);
            if (!func_variable) {
                semantic_error(SemanticErrorType::UNDEFINED_FUNCTION, node->lineno, identifier.c_str());
                return EMPTYTYPE;
            }
            if (!func_variable->isfunction) {
                semantic_error(SemanticErrorType::INVOKING_NONE_FUNCTION_VARIABLE, node->lineno, identifier.c_str());
                return EMPTYTYPE;
            }
            vector<Type*> args_types = checkArgs(node->child[2]);
            if (args_types.size() != func_variable->args.size()) {
                semantic_error(SemanticErrorType::INVALID_ARGUMENT_NUMBER, 
                node->lineno, func_variable->args.size(), args_types.size());
            } else {
                for (int i = 0; i < args_types.size(); i++) {
                    assert(typecheck(args_types[i], func_variable->args[i]->type));
                }
            }
            return func_variable->type;
        } else if (node->child[0]->type_name.compare("Exp") == 0) {
            // Exp LB Exp RB
            assert(node->child[1]->type_name.compare("LB") == 0);
            assert(node->child[2]->type_name.compare("Exp") == 0);
            assert(node->child[3]->type_name.compare("RB") == 0);
            Type *idType = checkExp(node->child[0]);
            Type *indexType = checkExp(node->child[2]);
            if (!typecheck(indexType, new Primitive_Type(TokenType::INT_T))) {
                semantic_error(SemanticErrorType::INVALID_INDEXING, node->child[0]->lineno);
                return EMPTYTYPE;
            }
            if (Array_Type* array_type = dynamic_cast<Array_Type*>(idType)) {
                return array_type->base;
            } else {
                semantic_error(SemanticErrorType::INDEXING_NONE_ARRAY_VARIABLE, node->child[0]->lineno);
                return EMPTYTYPE;
            }
            assert(false && "checkExp Failed");
        }
        assert(false && "checkExp Failed");
    }
    printf("node->child_num = %d node->type_name = %s\n", node->child_num, node->type_name.c_str());
    assert(false && "checkExp Failed");
}

/**
 * StmtList: Stmt StmtList
 * StmtList: %empty
 */
void checkStmtList(AST *node, Type *type) {
    DEBUG("checkStmtList", node);
    if (node->child_num == 0) {
        return;
    } else if (node->child_num == 2) {
        assert(node->child[0]->type_name.compare("Stmt") == 0);
        assert(node->child[1]->type_name.compare("StmtList") == 0);
        checkStmt(node->child[0], type);
        checkStmtList(node->child[1], type);
        return;
    }
    assert(false && "checkStmtList Failed");
}

/**
 * Stmt: Exp SEMI
 * Stmt: CompSt
 * Stmt: RETURN Exp SEMI
 * Stmt: IF LP Exp RP Stmt
 * Stmt: WHILE LP Exp RP Stmt
 * Stmt: IF LP Exp RP Stmt ELSE Stmt
 */
void checkStmt(AST *node, Type *type) {
    DEBUG("checkStmt", node);
    if (node->child_num == 1) {
        // CompSt
        assert(node->child[0]->type_name.compare("CompSt") == 0);
        checkCompSt(node->child[0], type);
        return;
    } else if (node->child_num == 2) {
        // Exp SEMI
        assert(node->child[0]->type_name.compare("Exp") == 0);
        assert(node->child[1]->type_name.compare("SEMI") == 0);
        checkExp(node->child[0]);
        return;
    } else if (node->child_num == 3) {
        // RETURN Exp SEMI
        assert(node->child[0]->type_name.compare("RETURN") == 0);
        assert(node->child[1]->type_name.compare("Exp") == 0);
        assert(node->child[2]->type_name.compare("SEMI") == 0);
        Type *returnType = checkExp(node->child[1]);
        if (!typecheck(returnType, type, false)){
            semantic_error(SemanticErrorType::RETURN_VALUE_MISMATCH, node->child[0]->lineno);
        }
        return;
    } else if (node->child_num == 5) {
        assert(node->child[1]->type_name.compare("LP") == 0);
        assert(node->child[2]->type_name.compare("Exp") == 0);
        assert(node->child[3]->type_name.compare("RP") == 0);
        assert(node->child[4]->type_name.compare("Stmt") == 0);
        if (node->child[0]->type_name.compare("IF") == 0) {
            // IF LP Exp RP Stmt
            Type *returnType = checkExp(node->child[2]);
            assert(typecheck(returnType, new Primitive_Type(TokenType::INT_T)) == true);
            checkStmt(node->child[4], type);
            return;
        } else if (node->child[0]->type_name.compare("WHILE") == 0) {
            // WHILE LP Exp RP Stmt
            Type *returnType = checkExp(node->child[2]);
            assert(typecheck(returnType, new Primitive_Type(TokenType::INT_T)) == true);
            checkStmt(node->child[4], type);
            return;
        }
        assert(false && "checkStmt Failed");
    } else if (node->child_num == 7) {
        // IF LP Exp RP Stmt ELSE Stmt
        assert(node->child[0]->type_name.compare("IF") == 0);
        assert(node->child[1]->type_name.compare("LP") == 0);
        assert(node->child[2]->type_name.compare("Exp") == 0);
        assert(node->child[3]->type_name.compare("RP") == 0);
        assert(node->child[4]->type_name.compare("Stmt") == 0);
        assert(node->child[5]->type_name.compare("ELSE") == 0);
        assert(node->child[6]->type_name.compare("Stmt") == 0);
        Type *returnType = checkExp(node->child[2]);
        assert(typecheck(returnType, new Primitive_Type(TokenType::INT_T)) == true);
        checkStmt(node->child[4], type);
        checkStmt(node->child[6], type);
        return;
    }
    printf("node->child_num == %d\n", node->child_num);
    assert(false && "checkStmt Failed");
}

/**
 * VarList: ParamDec COMMA VarList
 * VarList: ParamDec
 */
vector<Variable_Type*> checkVarList(AST *node) {
    DEBUG("checkStmt", node);
    assert(node->child[0]->type_name.compare("ParamDec") == 0);
    vector<Variable_Type*> variables = vector<Variable_Type*>();
    Variable_Type *variable = checkParamDec(node->child[0]);
    variables.push_back(variable);
    if (node->child_num == 1) {
        return variables;
    } else if (node->child_num == 3) {
        assert(node->child[1]->type_name.compare("COMMA") == 0);
        assert(node->child[2]->type_name.compare("VarList") == 0);
        vector<Variable_Type*> next_variables = checkVarList(node->child[2]);
        // merge variables
        variables.insert(variables.end(), next_variables.begin(), next_variables.end());
        return variables;
    }
    assert(false && "checkStmt Failed");
}

/**
 * ParamDec: Specifier VarDec
 */
Variable_Type *checkParamDec(AST *node) {
    DEBUG("checkParamDec", node);
    assert(node->child[0]->type_name.compare("Specifier") == 0);
    assert(node->child[1]->type_name.compare("VarDec") == 0);
    Type *type = checkSpecifier(node->child[0]);
    Variable_Type *variable = checkVarDec(node->child[1], type);
    updateVariable(variable);
    return variable;
}

/**
 * Args: Exp COMMA Args
 * Args: Exp
 */
vector<Type*> checkArgs(AST *node) {
    DEBUG("checkArgs", node);
    assert(node->child[0]->type_name.compare("Exp") == 0);
    vector<Type*> types = vector<Type*>();
    Type *type = checkExp(node->child[0]);
    types.push_back(type);
    if (node->child_num == 1) {
        return types;
    } else if (node->child_num == 3) {
        assert(node->child[1]->type_name.compare("COMMA") == 0);
        assert(node->child[2]->type_name.compare("Args") == 0);
        vector<Type*> next_types = checkArgs(node->child[2]);
        // merge variables
        types.insert(types.end(), next_types.begin(), next_types.end());
        return types;
    }
    assert(false && "checkArgs Failed");
}