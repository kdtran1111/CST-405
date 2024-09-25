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
    NodeType_Operand
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
    };
    
}ASTNode;

void print_ast(ASTNode* node, int indent);

#endif