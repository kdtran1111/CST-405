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
            print_ast(node->program.FuncDeclList, indent + 1);
            print_ast(node->program.VarDeclList, indent + 1);
            print_ast(node->program.StmntList, indent + 1);
            break;

        case NodeType_VarDeclList:
            printf("Variable Declaration List:\n");
            print_ast(node->VarDeclList.VarDecl, indent + 1);
            print_ast(node->VarDeclList.VarDeclList, indent + 1);
            break;

        case NodeType_VarDecl:
            printf("Variable Declaration: Type = %s, ID = %s, Size = %d\n", node->VarDecl.type, node->VarDecl.id, node->VarDecl.size);
            break;

        case NodeType_StmntList:
            printf("Statement List:\n");
            print_ast(node->StmntList.Stmnt, indent + 1);
            print_ast(node->StmntList.StmntList, indent + 1);
            break;

        case NodeType_Stmnt:
            printf("Assignment Statement: ID = %s, Operator = %s\n", node->Stmnt.id, node->Stmnt.op);
            print_ast(node->Stmnt.Expr, indent + 1);
            break;
            
        case NodeType_FuncDeclList:
            printf("Function Declaration List:\n");
            print_ast(node->FuncDeclList.FuncDecl, indent + 1);
            print_ast(node->FuncDeclList.FuncDeclList, indent + 1);
            break;

        case NodeType_FuncDecl:
            printf("Function Declaration: Type = %s, ID = %s\n", node->FuncDecl.type, node->FuncDecl.id);
            print_ast(node->FuncDecl.ParamList, indent + 1);
            print_ast(node->FuncDecl.VarDeclList, indent + 1);
            print_ast(node->FuncDecl.StmntList, indent + 1);
            print_ast(node->FuncDecl.ReturnStmnt, indent + 1);
            break;

        case NodeType_ParamList:
            printf("Parameter List:\n");
            print_ast(node->ParamList.Param, indent + 1);
            print_ast(node->ParamList.ParamList, indent + 1);
            break;

        case NodeType_Param:
            printf("Parameter: Type = %s, ID = %s\n", node->Param.type, node->Param.id);
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
            print_ast(node->ValueList.Val, indent + 1);
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
            printf("Simple Expression: Value = %d\n", node->SimpleExpr.value);
            break;

        case NodeType_SimpleID:
            printf("Simple ID: ID = %s\n", node->SimpleID.id);
            break;
        case NodeType_SimpleFloat:
            printf("Simple Float: Value = %f\n", node->SimpleFloat.value);
            break;
        case NodeType_SimpleString:
            printf("Simple String: Value = %s\n", node->SimpleString.value);
            break;
        case NodeType_SimpleArrIndex:
            printf("Simple Array Index: ID = %s, Index = %d\n", node->SimpleArrIndex.id, node->SimpleArrIndex.index);
            break;
        case NodeType_SimpleStructMember:
            printf("Simple Struct Member: ID = %s, Member ID = %s\n", node->SimpleStructMember.id, node->SimpleStructMember.member_id);
            break;

        case NodeType_WriteStmnt:
            printf("Write Statement: ID = %s\n", node->WriteStmnt.id);
            break;

        case NodeType_TypeCast:
            printf("Type Cast: Type = %s, ID = %s\n", node->TypeCast.type, node->TypeCast.id);
            break;

        case NodeType_StructDeclList:
            printf("Struct Declaration List:\n");
            print_ast(node->StructDeclList.StructDecl, indent + 1);
            print_ast(node->StructDeclList.StructDeclList, indent + 1);
            break;

        case NodeType_StructDecl:
            printf("Struct Declaration: ID = %s\n", node->StructDecl.id);
            print_ast(node->StructDecl.VarDeclList, indent + 1);
            break;

        case NodeType_IfStmnt:
            printf("If Statement:\n");
            print_ast(node->IfStmnt.ConditionList, indent + 1);
            print_ast(node->IfStmnt.StmntList, indent + 1);
            print_ast(node->IfStmnt.ElseIfList, indent + 1);
            print_ast(node->IfStmnt.ElseStmnt, indent + 1);
            break;
        
        case NodeType_ElseIfList:
            printf("Else If List:\n");
            print_ast(node->ElseIfList.ElseIfStmnt, indent + 1);
            print_ast(node->ElseIfList.ElseIfList, indent + 1);
            break;
        
        case NodeType_ElseIfStmnt:
            printf("Else If Statement:\n");
            print_ast(node->ElseIfStmnt.ConditionList, indent + 1);
            print_ast(node->ElseIfStmnt.StmntList, indent + 1);
            break;
        
        case NodeType_ElseStmnt:
            printf("Else Statement:\n");
            print_ast(node->ElseStmnt.StmntList, indent + 1);
            break;

        case NodeType_ConditionList:
            printf("Condition List: Boolean Operator = %s\n", node->ConditionList.BoolOperator);
            print_ast(node->ConditionList.Condition, indent + 1);
            print_ast(node->ConditionList.ConditionList, indent + 1);
            break;
        
        case NodeType_Condition:
            printf("Condition: Comparison Operator = %s\n", node->Condition.ComparisonOperator);
            print_ast(node->Condition.LeftVal, indent + 1);
            print_ast(node->Condition.RightVal, indent + 1);
            break;

        case NodeType_SwitchStmnt:
            printf("Switch Statement:\n");
            print_ast(node->SwitchStmnt.Expr, indent + 1);
            print_ast(node->SwitchStmnt.CaseList, indent + 1);
            print_ast(node->SwitchStmnt.DefaultCase, indent + 1);
            break;

        case NodeType_CaseList:
            printf("Case List:\n");
            print_ast(node->CaseList.CaseStmnt, indent + 1);
            print_ast(node->CaseList.CaseList, indent + 1);
            break;
        
        case NodeType_CaseStmnt:
            printf("Case Statement:\n");
            print_ast(node->CaseStmnt.Val, indent + 1);
            print_ast(node->CaseStmnt.StmntList, indent + 1);
            break;
        
        case NodeType_DefaultCase:
            printf("Default Case Statement:\n");
            print_ast(node->DefaultCase.StmntList, indent + 1);
            break;
        

        case NodeType_WhileLoop:
            printf("While Loop:\n");
            print_ast(node->WhileLoop.ConditionList, indent + 1);
            print_ast(node->WhileLoop.StmntList, indent + 1);
            break;


        default:
            printf("Unknown node type\n");
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