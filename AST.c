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
            print_ast(node->program.StructDeclList, indent + 1);
            print_ast(node->program.VarDeclList, indent + 1);
            print_ast(node->program.FuncDeclList, indent + 1);
            print_ast(node->program.StmntList, indent + 1);
            break;

        case NodeType_VarDeclList:
            printf("Var Decl List:\n");
            print_ast(node->VarDeclList.VarDecl, indent + 1);
            print_ast(node->VarDeclList.VarDeclList, indent + 1);
            break;

        case NodeType_VarDecl:
            printf("Var Decl: Type = %s, ID = %s, size = %d\n", node->VarDecl.type, node->VarDecl.id, node->VarDecl.size);
            
            break;

        case NodeType_StmntList:
            printf("Statement List:\n");
            print_ast(node->StmntList.Stmnt, indent + 1);
            print_ast(node->StmntList.StmntList, indent + 1);
            break;

        case NodeType_Stmnt:
            printf("Assign Statement: ID = %s, Operator = %s\n", node->Stmnt.id, node->Stmnt.op);
            print_ast(node->Stmnt.Expr, indent + 1);
            break;
            
        case NodeType_FuncDeclList:
            printf("Func Decl List:\n");
            print_ast(node->FuncDeclList.FuncDecl, indent + 1);
            print_ast(node->FuncDeclList.FuncDeclList, indent + 1);
            break;

        case NodeType_FuncDecl:
            printf("Func Decl: Type = %s, ID = %s\n", node->FuncDecl.type, node->FuncDecl.id);
            print_ast(node->FuncDecl.ParamList, indent + 1);
            print_ast(node->FuncDecl.VarDeclList, indent + 1);
            print_ast(node->FuncDecl.StmntList, indent + 1);
            print_ast(node->FuncDecl.ReturnStmnt, indent + 1);
            break;

        case NodeType_ParamList:
            printf("Param List:\n");
            print_ast(node->ParamList.Param, indent + 1);
            print_ast(node->ParamList.ParamList, indent + 1);
            break;

        case NodeType_Param:
            printf("Param: Type = %s, ID = %s\n", node->Param.type, node->Param.id);
            break;

        case NodeType_ReturnStmnt:
            printf("Return Statement: ID = %s\n", node->ReturnStmnt.id);
            break;

        case NodeType_IndexAssignment:
            printf("Index Assignment: ID = %s, Index = %d\n", node->IndexAssignment.id, node->IndexAssignment.index);
            print_ast(node->IndexAssignment.Expr, indent + 1);
            break;

        case NodeType_ArrAssignment:
            printf("Array Assignment: ID = %s\n", node->ArrAssignment.id);
            print_ast(node->ArrAssignment.ValueList, indent + 1);
            break;

        case NodeType_StructMemberAssignment:
            printf("Struct Member Assignment: ID = %s, Member ID = %s\n", node->StructMemberAssignment.id, node->StructMemberAssignment.member_id);
            print_ast(node->StructMemberAssignment.Expr, indent + 1);
            break;
        
        case NodeType_ValueList:
            printf("Value List:\n");
            print_ast(node->ValueList.Val, indent +1);
            print_ast(node->ValueList.ValueList, indent + 1);
            break;

        case NodeType_Expr:
            printf("Expression: Operator = %s\n", node->Expr.op);
            print_ast(node->Expr.left, indent + 1);
            print_ast(node->Expr.right, indent + 1);
            break;

        case NodeType_FunctionCall:
            printf("Function Call: ID = %s\n", node->FunctionCall.id);
            print_ast(node->FunctionCall.valueList, indent + 1);
            break;

        case NodeType_SimpleExpr:
            printf("Simple Expression: value = %d\n", node->SimpleExpr.value);
            break;

        case NodeType_SimpleID:
            printf("Simple ID: id = %s\n", node->SimpleID.id);
            break;
        case NodeType_SimpleFloat:
            printf("Simple Float: value = %f\n", node->SimpleFloat.value);
            break;
        case NodeType_SimpleString:
            printf("Simple String: value = %s\n", node->SimpleString.value);
            break;
        case NodeType_SimpleArrIndex:
            printf("Simple Array Index: id = %s, index = %d\n", node->SimpleArrIndex.id, node->SimpleArrIndex.index);
            break;
        case NodeType_SimpleStructMember:
            printf("Simple Struct Member: id = %s, member id = %s\n", node->SimpleStructMember.id, node->SimpleStructMember.member_id);
            break;

        case NodeType_WriteStmnt:
            printf("Write Statement: id = %s\n", node->WriteStmnt.id);
            break;

        case NodeType_TypeCast:
            printf("Type Cast: type = %s, id = %s\n", node->TypeCast.type, node->TypeCast.id);
            break;

        case NodeType_StructDeclList:
            printf("Struct Decl List:\n");
            print_ast(node->StructDeclList.StructDecl, indent + 1);
            print_ast(node->StructDeclList.StructDeclList, indent + 1);
            break;
        case NodeType_StructDecl:
            printf("Struct Decl: ID = %s\n", node->StructDecl.id);
            print_ast(node->StructDecl.VarDeclList, indent + 1);
            break;
        
        default:
            break;
    }   
}

//Trying out freeing ASTTree
void freeAST(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NodeType_program:
            // Free the program's variable declaration list and statement list
            freeAST(node->program.VarDeclList);
            freeAST(node->program.StmntList);
            break;

        case NodeType_VarDeclList:
            // Free individual variable declarations and the list itself
            freeAST(node->VarDeclList.VarDecl);
            freeAST(node->VarDeclList.VarDeclList);
            break;

        case NodeType_VarDecl:
            // Free variable type and name strings
            free(node->VarDecl.type);
            free(node->VarDecl.id);
            break;

        case NodeType_StmntList:
            // Free statement and statement list
            freeAST(node->StmntList.Stmnt);
            freeAST(node->StmntList.StmntList);
            break;

        case NodeType_Stmnt:
            // Free statement's ID and operator strings
            free(node->Stmnt.id);
            free(node->Stmnt.op);
            freeAST(node->Stmnt.Expr); // Free the associated expression
            break;

        case NodeType_Expr:
            // Free left and right expressions and the simple expression if any
            freeAST(node->Expr.left);
            freeAST(node->Expr.right);
            // free(node->Expr.op); // Assuming op is not dynamically allocated
            freeAST(node->Expr.SimpleExpr); // Free the simple expression if it's a pointer
            break;

        case NodeType_SimpleExpr:
            // No dynamic allocation in SimpleExpr to free
            break;

        case NodeType_SimpleID:
            // Free the ID string
            free(node->SimpleID.id);
            break;

        case NodeType_Operand:
            // No dynamic allocation in Operand to free
            break;
        case NodeType_WriteStmnt:
            // Free the ID string
            free(node->WriteStmnt.id);
            break;
        default:
            // Handle any unexpected node types
            fprintf(stderr, "Warning: Unrecognized node type encountered during freeAST.\n");
            break;
    }

    free(node); // Free the current node after all children have been freed
}