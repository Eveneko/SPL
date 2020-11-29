#include "ast.hpp"

using namespace std;

AST::AST(string type_name, SymbolType symbol_type, string value, int lineno, int child_num, ...){
    this->type_name = type_name;
    this->symbol_type = symbol_type;
    this->value = value;
    this->child_num=child_num;
    this->child = vector<AST *>();
    this->lineno = lineno;

    va_list args;
    va_start(args, child_num);
    while (child_num--){
        AST *p = va_arg(args, AST *);
        this->child.push_back(p);
    }
}

void AST::print(int tabnum){
    switch (symbol_type) {
        case SymbolType::NONTERMINAL:
            if (!child.empty()){
                indent(tabnum);
                fprintf(stdout, "%s (%d)\n", type_name.c_str(), lineno);
            }
            break;
        case SymbolType::TOKEN_VALUE:
            indent(tabnum);
            fprintf(stdout, "%s: %s\n", type_name.c_str(), value.c_str());
            break;
        case SymbolType::TOKEN:
            indent(tabnum);
            fprintf(stdout, "%s\n", type_name.c_str());
            break;
        default:
            throw -2;
    }
    for (auto it: child) {
        it->print(tabnum + 2);
    }
}

void AST::print_self() {
    switch (symbol_type) {
        case SymbolType::NONTERMINAL:
            fprintf(stdout, "%s (%d)\n", type_name.c_str(), lineno);
            break;
        case SymbolType::TOKEN_VALUE:
            fprintf(stdout, "%s: %s\n", type_name.c_str(), value.c_str());
            break;
        case SymbolType::TOKEN:
            fprintf(stdout, "%s at line %d\n", type_name.c_str(), lineno);
            break;
        default:
            throw -1;
    }
}

void indent(int tabnum){
    while (tabnum--)
        fprintf(stdout, " ");
}