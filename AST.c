#include "AST.h"

void print_ast(ASTNode* node, int indent) 
{
    if (node == NULL) {
        return;
    }

    // Print indentation for better readability
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    // Switch on the node type
    switch (node->type) {
        case NodeType_program:
            printf("Program:\n");
            print_ast(node->program.VarDeclList, indent + 1);
            print_ast(node->program.StmntList, indent + 1);
            break;

        case NodeType_VarDeclList:
            printf("Var Decl List:\n");
            print_ast(node->VarDeclList.VarDecl, indent + 1);
            print_ast(node->VarDeclList.VarDeclList, indent + 1);
            break;

        case NodeType_VarDecl:
            printf("Var Decl: Type = %s, ID = %s\n", node->VarDecl.type, node->VarDecl.id);
            break;

        case NodeType_StmntList:
            printf("Statement List:\n");
            print_ast(node->StmntList.stmnt, indent + 1);
            print_ast(node->StmntList.StmntList, indent + 1);
            break;

        case NodeType_Stmnt:
            printf("Assign Statement: ID = %s, Operator = %s\n", node->Stmnt.id, node->Stmnt.op);
            print_ast(node->Stmnt.expression, indent + 1);
            break;

        case NodeType_Expr:
            printf("Expression: Operator = %s\n", node->Expr.op);
            print_ast(node->Expr.left, indent + 1);
            print_ast(node->Expr.right, indent + 1);
            break;

        case NodeType_SimpleExpr:
            printf("Simple Expression: value = %d\n", node->SimpleExpr.value);
            break;

        case NodeType_SimpleID:
            printf("Simple ID: id = %s\n", node->SimpleID.id);
            break;

        default:
            printf("Unknown node type!\n");
            break;
    }   
}