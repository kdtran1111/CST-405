#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>

// enum to differentiate different types of nodes
typedef enum
{
    NodeType_program,
    NodeType_VarDeclList,
    NodeType_VarDecl,
    NodeType_StmntList,
    NodeType_Stmnt,
    NodeType_Expr,
    NodeType_SimpleID,
    NodeType_SimpleExpr,
    NodeType_Operand,
    NodeType_WriteStmnt,
    NodeType_FuncDeclList,
    NodeType_FuncDecl,
    NodeType_ParamList,
    NodeType_Param,
    NodeType_ReturnStmnt
}NodeType;

// structs for each node type
typedef struct ASTNode
{
    NodeType type;
    union 
    {
        struct
        {
            struct ASTNode* VarDeclList;
            struct ASTNode* StmntList;
            struct ASTNode* FuncDeclList;
        }program;

        struct
        {
            struct ASTNode* VarDecl;
            struct ASTNode* VarDeclList;
        }VarDeclList;

        struct
        {
            char* type;
            char* id;
        }VarDecl;

        struct
        {
            struct ASTNode* Stmnt; //was stmnt
            struct ASTNode* StmntList;
        }StmntList;

        struct
        {   
            char* id;
            char* op;
            struct ASTNode* Expr; //was expression
            
        }Stmnt;

        struct
        {
            char* op;
            int check;
            struct ASTNode* left;
            struct ASTNode* right;
            struct ASTNode* SimpleExpr;
        }Expr;

        struct
        {
            int value;
        }SimpleExpr;

        struct
        {
            char* id;
        }SimpleID;

        struct
        {
            int value;
        }Operand;
        struct
        {   
            char* id;
        }WriteStmnt;

        struct
        {
            struct ASTNode* FuncDecl;
            struct ASTNode* FuncDeclList;
        }FuncDeclList;

        struct
        {
            char* id;
            char* type;
            struct ASTNode* ParamList;
            struct ASTNode* VarDeclList;
            struct ASTNode* StmntList;
            struct ASTNode* ReturnStmnt;
        }FuncDecl;

        struct 
        {
            struct ASTNode* Param;
            struct ASTNode* ParamList;
        }ParamList;

        struct
        {
            char* type;
            char* id;
        }Param;

        struct
        {
            char* id;
        }ReturnStmnt;
    };
    
}ASTNode;

void print_ast(ASTNode* node, int indent);
void freeAST(ASTNode* node);
#endif