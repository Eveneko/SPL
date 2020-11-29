#include <bits/stdc++.h>

using namespace std;
#define INDENT_NUM 2

enum class SymbolType {
    NONTERMINAL = 1,
    TOKEN_VALUE = 2,
    TOKEN = 3
};

class AST
{
    public:
        string type_name;
        SymbolType symbol_type;
        string value;
        int child_num;
        int lineno;
        vector<AST *> child;

        AST(string type_name, SymbolType symbol_type, string value, int lineno, int child_num, ...);
        void print();
        void print_self();
    private:
        void print(int tabnum);
};

void indent(int tabnum);
string to_str(int x);