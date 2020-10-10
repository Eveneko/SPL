#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ast.h"

astNode *newAst(char *astNodeName, tokenType astNodeType, char *astNodeValue, int lineno, int childNum, ...){
    astNode *_astNode = malloc(sizeof(astNode));

    _astNode->astNodeName = astNodeName;
    _astNode->astNodeType = astNodeType;
    _astNode->astNodeValue = astNodeValue;
    _astNode->lineno = lineno;
    _astNode->childNum = childNum;
    _astNode->childNodes = NULL;

    va_list args;
    va_start(args, childNum);

    for(astNode **ptr = _astNode->childNodes; childNum--; ++ptr){
        *ptr = va_arg(args, astNode*);
    }

    return _astNode;
}

void printAst(astNode *_astNode, int indentNum){
    if(_astNode->astNodeType == NON_TERMINAL && _astNode->childNodes != NULL){
        indent(indentNum);
        fprintf(fout, "%s (%d)\n", _astNode->astNodeName, _astNode->lineno);
    }
    else if(_astNode->astNodeType == TOKEN_WITH_VALUE){
        indent(indentNum);
        fprintf(fout, "%s %s\n", _astNode->astNodeName, _astNode->astNodeValue);
    }
    else if(_astNode->astNodeType == TOKEN_WITHOUT_VALUE){
        indent(indentNum);
        fprintf(fout, "%s\n", _astNode->astNodeName);
    }
    for(int i = 0; i < _astNode->childNum; ++i){
        printAst(_astNode->childNodes[i], indentNum + INDENT_NUM);
    }
}

void freeAst(astNode *_astNode){
    for(int i = 0; i < _astNode->childNum; ++i){
        freeAst(_astNode->childNodes[i]);
    }
    free(_astNode);
}

void indent(int n){
    while(n-- > 0){
        fprintf(fout, " ");
    }
    return;
}