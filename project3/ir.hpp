#include <bits/stdc++.h>
#include "semantic.hpp"
using namespace std;

#define DEBUG(msg) fprintf(stdout, "DEBUG: %s\n", msg);

extern const int INFO_SIZE;
string addr2str(int addr);

enum class Operator;
string operator2str(Operator op);

/* Three-Address Code Specification */
class TAC;
extern vector<TAC*> tac_vector;

class TAC{
public:
    int address;
    string name;
    bool swap_flag = false;
    Type *type;
    TAC() {
        swap_flag = false;
    }
    virtual string to_string(){
        return "";
    }
    virtual int genid(){
        int index = tac_vector.size();
        tac_vector.push_back(this);
        return index;
    }
};

class LabelTAC: public TAC {
public:
    LabelTAC(int address){
        TAC::address = address;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "LABEL label%d :", TAC::address);
        return buffer;
    }
};

class FunctionTAC: public TAC {
public:
    FunctionTAC(int address, string name){
        TAC::address = address;
        TAC::name = name;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "FUNCTION %s :", TAC::name.c_str());
        return buffer;
    }
};

class AssignTAC: public TAC{
public:
    int right_address;
    AssignTAC(int address, int right_address){
        TAC::address = address;
        this->right_address = right_address;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "t%d := %s", TAC::address, addr2str(right_address).c_str());
        return buffer;
    }
};

class AssignAddressTAC: public TAC{
public:
    int right_address;
    AssignAddressTAC(int address, int right_address){
        TAC::address = address;
        this->right_address = right_address;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "t%d := &t%d", TAC::address, right_address);
        return buffer;
    }
};

class AssignValueTAC: public TAC{
public:
    int right_address;
    AssignValueTAC(int address, int right_address){
        TAC::address = address;
        this->right_address = right_address;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "t%d := *t%d", TAC::address, right_address);
        return buffer;
    }
};

class CopyValueTAC: public TAC{
public:
    int right_address;
    CopyValueTAC(int address, int right_address){
        TAC::address = address;
        this->right_address = right_address;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "*t%d := t%d", TAC::address, right_address);
        return buffer;
    }
};

class ArithmeticTAC: public TAC{
public:
    Operator op;
    int left_address;
    int right_address;
    ArithmeticTAC(int address, Operator op, int left_address, int right_address){
        TAC::address = address;
        this->op = op;
        this->left_address = left_address;
        this->right_address = right_address;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "t%d := %s %s %s", TAC::address, addr2str(left_address).c_str(), operator2str(op).c_str(), addr2str(right_address).c_str());
        return buffer;
    }
};

class GOTOTAC: public TAC{
public:
    int* label;
    GOTOTAC(int address, int* label){
        TAC::address = address;
        this->label = label;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "GOTO label%d", TAC::address, *label);
        return buffer;
    }
};

class IFTAC: public TAC{
public:
    int* label;
    Operator op;
    int left_address;
    int right_address;
    IFTAC(int address, Operator op, int left_address, int right_address, int* label){
        TAC::address = address;
        this->op = op;
        this->left_address = left_address;
        this->right_address = right_address;
        this->label = label;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "IF t%d %s t%d GOTO label%d", left_address, operator2str(op).c_str(), right_address, *label);
        return buffer;
    }
};

class ReturnTAC: public TAC{
public:
    int right_address;
    ReturnTAC(int address, int right_address){
        TAC::address = address;
        this->right_address = right_address;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "RETURN %s", addr2str(right_address).c_str());
        return buffer;
    }
};

class DecTAC: public TAC{
public:
    vector<int> suffix;
    vector<int> sizes;
    DecTAC(int address, Type *type, string name, vector<int> sizes){
        TAC::address = address;
        TAC::type = type;
        TAC::name = name;
        this->sizes = vector<int>();
        this->sizes.insert(this->sizes.end(), sizes.begin(), sizes.end());
        int totle = 1;
        for(int i = sizes.size() - 1; i >= 0; --i){
            suffix.push_back(totle);
            totle *= sizes[i];
        }
    }
    string to_string(){
        char buffer[INFO_SIZE];
        int size = 1;
        for(auto s: sizes){
            size *= s;
        }
        sprintf(buffer, "DEC t%d %d", TAC::address, TAC::type->getSize() * size);
        return buffer;
    }
};

class ParamTAC: public TAC{
public:
    vector<int> suffix;
    vector<int> sizes;
    ParamTAC(int address, Type *type, vector<int> sizes){
        TAC::address = address;
        TAC::type = type;
        this->sizes = vector<int>();
        this->sizes.insert(this->sizes.end(), sizes.begin(), sizes.end());
        int totle = 1;
        for(int i = sizes.size() - 1; i >= 0; --i){
            suffix.push_back(totle);
            totle *= sizes[i];
        }
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "PARAM t%d", TAC::address);
        return buffer;
    }
};

class ArgTAC: public TAC{
public:
    int right_address;
    ArgTAC(int address, int right_address){
        TAC::address = address;
        this->right_address =right_address;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "ARG t%d", right_address);
        return buffer;
    }
};

class CallTAC: public TAC{
public:
    string label;
    CallTAC(int address, string label){
        TAC::address = address;
        this->label = label;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "t%d := CALL %s", TAC::address, label.c_str());
        return buffer;
    }
};

class ReadTAC: public TAC{
public:
    ReadTAC(int address){
        TAC::address = address;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "READ t%d", TAC::address);
        return buffer;
    }
};

class WriteTAC: public TAC{
public:
    int right_address;
    WriteTAC(int address, int right_address){
        TAC::address = address;
        this->right_address = right_address;
    }
    string to_string(){
        char buffer[INFO_SIZE];
        sprintf(buffer, "WRITE t%d", right_address);
        return buffer;
    }
};

extern vector<TAC *> tac_vector;
extern map<string, int> table;

void irInit();
void irProgram(AST *root);
void irExrDefList(AST *node);
void irExtDecList(AST *node, Type * type);
void irExtDef(AST *node);
Type *irSpecifier(AST *node);
Type *irType(AST *node);
Type *irStructSpecifier(AST *node);
void irFunc(AST *node, Type *type);
void irCompSt(AST *node);
void irDefList(AST *node);
void irDef(AST *node);
void irDecList(AST *node, Type *type);
void irStmt(AST *node);
void irStmtList(AST *node);
void irDec(AST *node, Type *type);
TAC* irVarDec(AST *node, Type* type);
int irExp(AST *node, bool single=false);
void irVarList(AST *node);
void irParamDec(AST *node);
vector<int> irArgs(AST *node);

int getIR(string name);
void putIR(string name, int id);
int genid(TAC *tac);
int *genlist(int id = tac_vector.size() + 1);

void irIF(int id, int tbranch, int fbranch);
void irWHILE(vector<int>* stat_vec, int end, int target);
