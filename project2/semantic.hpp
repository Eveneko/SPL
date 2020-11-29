#include <bits/stdc++.h>
#include "ast.hpp"

using namespace std;

enum class TokenType {
    INT_T, CHAT_T, FLOAT_T,
};

enum class CheckType {
    OK = 1,
    APPLY_FUNCTION_ON_VARIABLE_Type = 2,
    ARGS_MISMATCH = 3,
    VARIABLE_Type_NOT_FOUND = 4,
    FUNCTION_NOT_FOUND = 5,
    STRUCTURE_Type_NOT_FOUND = 6,
    VARIABLE_Type_REDEFINED = 7,
    FUNCTION_REDEFINED = 8,
    STRUCTURE_Type_REDEFINED = 9,
};

class Type
{
    public:
        int lineno;
        Type(int lineno = 1): lineno(lineno){}
        virtual bool operator == (const Type &other) const {
            return (typeid(*this)==typeid(other));
        }
        virtual bool operator!=(const Type &other) const {
            return !(*this == other);
        }
};

extern bool typecheck(Type *left, Type*right, bool weak=false, bool allow_empty=true);


/* 基本数据类型 */
class Primitive_Type: public Type{
public:
    /* defined in y.tab.h */
    TokenType token_type;

    Primitive_Type(TokenType token_type, int lineno=1): token_type(token_type) {
        assert(token_type == TokenType::CHAT_T || token_type == TokenType::INT_T || token_type == TokenType::FLOAT_T);
        Type::lineno = lineno;
    };
    bool operator==(const Type &other) const{
        return (typeid(*this) == typeid(other)) && (token_type == dynamic_cast<const Primitive_Type &>(other).token_type);
    }
    bool operator!=(const Type &other) const {
        return !(*this == other);
    }
};

/* 数组类型 */
class Array_Type: public Type {
public:
    Type *base;
    int size;
    Array_Type(Type *base, int size, int lineno=1): base(base), size(size) {
        Type::lineno = lineno;
    }
    bool operator==(const Type &other) const{
        return (typeid(*this) == typeid(other)) && \
        (size == dynamic_cast<const Array_Type &>(other).size) && \
        (*base == *(dynamic_cast<const Array_Type &>(other).base));
    }
    bool operator!=(const Type &other) const {
        return !(*this == other);
    }
};

class Variable_Type{
public:
    string name;
    Type *type;
    vector<Variable_Type *> args;
    int lineno;
    bool isfunction;
    Variable_Type(string name, Type *type, vector<Variable_Type *> ls = {}, bool isfunction=true, int lineno = 1): name(name), type(type)
    {
        this->isfunction = isfunction;
        this->lineno = lineno;
        for (auto it = ls.begin(); it != ls.end(); ++it){
            this->args.push_back(*it);
        }
        // printf("new variable with name %s\n", name.c_str());
    }
    bool MatchArgs(vector<Type*> ls, bool weak=false){
        if (args.size()!=ls.size())
            return false;
        int len = args.size();
        for (int i = 0; i < len; ++i){
            if (!typecheck(args[i]->type,ls[i], weak))
                return false;
        }
        return true;
    }
    bool operator!=(const Variable_Type &other) const{
        return !(*this == other);
    }
    bool operator==(const Variable_Type &other) const{
        if (name.compare(other.name)!=0 || args.size()!=other.args.size()) return false;
        int len = args.size();
        for (int i = 0; i < len;++i){
            if (*(args[i]->type)!=*(other.args[i]->type)){
                return false;
            }
        }
        return true;
    }
};


class Structure_Type: public Type{
public:
    string name;
    vector<Variable_Type*> field;

    string tostring(){
        return name;
    }
    Structure_Type(string name, vector<Variable_Type *> field = {}, int lineno=1) : name(name)
    {
        Type::lineno = lineno;
        this->field = vector<Variable_Type *>();
        for (auto it : field)
        {
            this->field.push_back(it);
        }
    }
    Type* TypeOfMember(string a){
        for (auto it: field){
            if (it->name.compare(a)==0){
                return it->type;
            }
        }
        return NULL;
    }
    bool operator==(const Type &other) const{
        return (typeid(*this)==typeid(other)) && (name.compare(dynamic_cast<const Structure_Type &>(other).name)==0);
    }
    bool operator!=(const Type &other) const {
        return !(*this == other);
    }
};

extern Type *EMPTYTYPE;
extern Variable_Type *EMPTYVAR;

extern multimap<string, Variable_Type*> var_map;
extern multimap<string, Structure_Type*> type_map;
extern void report_semantic_error(const char *s,...);
void checkProgram(AST *root);
enum class SemanticErrorType {
    UNDEFINED_VARIABLE = 1,
    UNDEFINED_FUNCTION = 2,
    REDEFINED_VARIABLE = 3,
    REDEFINED_FUNCTION = 4,
    UNMATCHING_TYPE_OF_ASSIGNMENT = 5,
    ASSIGN_TO_RAW_VALUE = 6,
    BINARY_OPERATION_ON_NONE_NUMBER_VARIABLE = 7,
    RETURN_VALUE_MISMATCH = 8,
    INVALID_ARGUMENT_NUMBER = 9,
    INDEXING_NONE_ARRAY_VARIABLE = 10,
    INVOKING_NONE_FUNCTION_VARIABLE = 11,
    INVALID_INDEXING = 12,
    ACCESS_NONE_STRUCT_VARIABLE = 13,
    ACCESS_NONE_EXIST_MEMBER = 14,
    REDEFINED_STRUCTURE = 15,
};
extern void semantic_error(SemanticErrorType error_type, ...);