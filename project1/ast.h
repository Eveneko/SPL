#define INDENT_NUM 2

FILE *fin, *fout;

typedef enum tokenType{
    NON_TERMINAL,
    TOKEN_WITH_VALUE,
    TOKEN_WITHOUT_VALUE
}tokenType;

typedef struct astNode{
    char *astNodeName;
    tokenType astNodeType;
    char *astNodeValue;
    int lineno;
    int childNum; 
    struct astNode **childNodes;
}astNode;

astNode *newAst(char *astNodeName, tokenType astNodeType, char *astNodeValue, int lineno, int childNum, ...);
void printAst(astNode *_astNode, int indentNum);
void freeAst(astNode *_astNode);

void indent(int n);
