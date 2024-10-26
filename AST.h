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
    NodeType_SimpleFloat,
    NodeType_SimpleString,
    NodeType_SimpleArrIndex,
    NodeType_SimpleStructMember,
    NodeType_Operand,
    NodeType_WriteStmnt,
    NodeType_FuncDeclList,
    NodeType_FuncDecl,
    NodeType_ParamList,
    NodeType_Param,
    NodeType_ReturnStmnt,
    NodeType_IndexAssignment,
    NodeType_ValueList,
    NodeType_ArrAssignment,
    NodeType_StructDeclList,
    NodeType_StructDecl,
    NodeType_StructMemberAssignment,
    NodeType_FunctionCall,
    NodeType_TypeCast

}NodeType;

// structs for each node type
typedef struct ASTNode
{
    NodeType type;
    union 
    {
        struct
        {
            struct ASTNode* StructDeclList;
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
            int declared; // 1 for yes, 0 for no
            char* type;
            int size;
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
            float value;
        }SimpleFloat;

        struct
        {
            char* value;
        }SimpleString;

        struct
        {
            char* id;
        }SimpleID;

        struct
        {
            int index;
            char* id;
        }SimpleArrIndex;

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

        struct
        {
            char* id;
            int index;
            struct ASTNode* Expr;
        }IndexAssignment;

        struct 
        {
            struct ASTNode* Val;
            struct ASTNode* ValueList;
        }ValueList;

        struct
        {
            char* id;
            struct ASTNode* ValueList;
        }ArrAssignment;

        struct
        {
            struct ASTNode* StructDecl;
            struct ASTNode* StructDeclList;
        }StructDeclList;

        struct
        {
            char* id;
            struct ASTNode* VarDeclList;
        }StructDecl;

        struct
        {
            char* id;
            char* member_id;
            struct ASTNode* Expr;
        }StructMemberAssignment;

        struct
        {
            char* id;
            char* member_id;
        }SimpleStructMember;

        struct 
        {
            char* id;
            struct ASTNode* valueList;
        }FunctionCall;

        struct
        {   
            char* type;
            char* id;
        }TypeCast;
        
        
        
    };
    
}ASTNode;

void print_ast(ASTNode* node, int indent);
void freeAST(ASTNode* node);
#endif