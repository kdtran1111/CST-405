#include "semantic.h"
#include <stdio.h>

TAC* tacHead =NULL;


void semanticAnalysis(ASTNode* node, SymbolTable* symTab) {
    if (node == NULL) return;

    switch (node->type) {
         case NodeType_program:
            printf("Performing semantic analysis on program\n");
            semanticAnalysis(node->program.VarDeclList, symTab);
            semanticAnalysis(node->program.StmntList, symTab);
            break;
        case NodeType_VarDeclList:
            semanticAnalysis(node->VarDeclList.VarDecl, symTab);
            semanticAnalysis(node->VarDeclList.VarDeclList, symTab);
            break;
        case NodeType_VarDecl:
            
            // Check for redeclaration of variables
            break;
        case NodeType_StmntList:
            semanticAnalysis(node->StmntList.stmnt, symTab);
            semanticAnalysis(node->StmntList.StmntList, symTab);
            break;
        /*
        case NodeType_Assignstmnt:
            semanticAnalysis(node->assignstmnt.Expr, symTab);
            break;  
        
        */
       case NodeType_Expr:
            semanticAnalysis(node->Expr.left, symTab);
            semanticAnalysis(node->Expr.right, symTab);
            break;
        /*
        case NodeType_BinOp:
            // Check for declaration of variables
            if (lookupSymbol(symTab, node->binOp.left->VarDecl.varName) == NULL) {
                fprintf(stderr, "Semantic error: Variable %s has not been declared\n", node->VarDecl.varName);
            }
            if (lookupSymbol(symTab, node->binOp.right->VarDecl.varName) == NULL) {
                fprintf(stderr, "Semantic error: Variable %s has not been declared\n", node->VarDecl.varName);
            }
            semanticAnalysis(node->binOp.left, symTab);
            break;
        */
        case NodeType_SimpleID:
            // Check for declaration of variable
            if (lookupSymbol(symTab, node->SimpleID.id) == NULL) {
                fprintf(stderr, "Semantic error: Variable %s has not been declared\n", node->SimpleID.id);
            }
        case NodeType_SimpleExpr:
            // no checks necessary for number
        // ... handle other node types ...

        default:
            fprintf(stderr, "Unknown Node Type\n");
    }

       // ... other code ...

    if (node->type == NodeType_Expr || node->type == NodeType_SimpleExpr) {
        TAC* tac = generateTACForExpr(node);
        // Process or store the generated TAC
        printTAC(tac);
    }

    // ... other code ...

}
