#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ast.h"

typedef enum tokenType
{
    NONTERMINAL,
    TOKENWITHVALUE,
    TOKENWITHOUTVALUE
}tokenType;

typedef struct astNode
{
    char* astNodeName;
    tokenType astNodeType;
    char* astNodeValue;
    int lineno;
    int childNum; 
    struct astNode** childNodes;
}astNode;

astNode* newAst(char* astNodeName, tokenType astNodeType, char* astNodeValue, int lineno, int childNum, ...);
void printAst(astNode* node, int indentNum);
void freeAst(astNode* node);

void indent(int n);