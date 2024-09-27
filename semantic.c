#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

TAC* tacHead = NULL;  // Global head of the TAC instructions list
extern int declaredSymbol;
extern int lines;
// Error handling function for semantic analysis
void semantic_error(const char* message, int line) {
    fprintf(stderr, "SEMANTIC ERROR (Line %d): %s\n", line, message);
    //exit(1);
}

// Check if a variable is declared
void check_variable_declared(SymbolTable* symbol_table, const char* id, int line) {
    Symbol* symbol = getSymbol(symbol_table, id);
    if (symbol == NULL) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Variable '%s' used before declaration", id);
        semantic_error(error_message, lines);
    }
}

// Check if a variable has been assigned a value
void check_variable_initialized(SymbolTable* symbol_table, const char* id, int line) {
    Symbol* symbol = getSymbol(symbol_table, id);
    if (symbol == NULL) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Variable '%s' used before declaration", id);
        semantic_error(error_message, lines);
    }
    
    if (symbol->value == 0) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Variable '%s' used without being assigned a value, therefore default value is 0", id);
        semantic_error(error_message, lines);
    }
}

// Check for type consistency during assignments
void check_type_consistency(SymbolTable* symbol_table, const char* id, ASTNode* expr, int line) {
    Symbol* symbol = getSymbol(symbol_table, id);
    
    if (symbol == NULL) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Variable '%s' used before declaration", id);
        semantic_error(error_message, lines);
    }


    if (strcmp(symbol->type, "int") == 0 && expr->type != NodeType_Expr && expr->type != NodeType_SimpleExpr) {
    // This will allow both integer literals and expressions
    char error_message[256];
    snprintf(error_message, sizeof(error_message), "Type mismatch: Variable '%s' is an int, but assigned a non-integer value", id);
    semantic_error(error_message, lines);
}
    /*
    // Perform type checking based on the expression's type
    if (strcmp(symbol->type, "int") == 0 && expr->type != NodeType_SimpleExpr) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Type mismatch: Variable '%s' is an int, but assigned a non-integer value", id);
        semantic_error(error_message, lines);
    }
    */
    if (strcmp(symbol->type, "float") == 0 && expr->type != NodeType_SimpleExpr) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Type mismatch: Variable '%s' is a float, but assigned a non-float value", id);
        semantic_error(error_message, lines);
    }

    // Add more type checks as necessary
}

// Recursive function for semantic analysis and TAC generation
void semanticAnalysis(ASTNode* node, SymbolTable* symbol_table) {
    if (node == NULL) return;

    switch (node->type) {
        case NodeType_VarDecl:
            // Variable declaration
            break;

        case NodeType_Stmnt:
            check_variable_declared(symbol_table, node->Stmnt.id, lines);  // Check if variable is declared
            semanticAnalysis(node->Stmnt.Expr, symbol_table);        // Analyze the expression on the right-hand side

            // Check for type consistency and initialization
            check_type_consistency(symbol_table, node->Stmnt.id, node->Stmnt.Expr, lines);
            
            // Fetch the variable symbol to update
            Symbol* symbol = getSymbol(symbol_table, node->Stmnt.id);
            if (symbol != NULL && node->Stmnt.Expr->type == NodeType_SimpleExpr) {
            // Assuming the expression has an integer literal, update the variable's value
           
            updateValue(symbol_table, node->Stmnt.id, node->Stmnt.Expr->SimpleExpr.value);
            printf("updateValue has no problem\n");
            
            
            // Generate TAC for the assignment statement
           // TAC* tac = generateTACForExpr(node->Stmnt.Expr);
            //appendTAC(&tacHead, tac);
            //printf("Tac is fine\n");
            // Automatically update the value in the symbol table
            //int resultValue = evaluateExpression(node->Stmnt.expression, symbol_table);
            //updateValue(symbol_table, node->Stmnt.id, resultValue);  // Update the value of the variable after the assignment
            break;
        case NodeType_Expr:
            // Recursively analyze left and right expressions
            semanticAnalysis(node->Expr.left, symbol_table);
            semanticAnalysis(node->Expr.right, symbol_table);
            break;

        case NodeType_SimpleID:
            check_variable_initialized(symbol_table, node->SimpleID.id, lines);  // Line number passed as placeholder
            break;

        case NodeType_SimpleExpr:
            semanticAnalysis(node->Expr.left, symbol_table);
            semanticAnalysis(node->Expr.right, symbol_table);
            // Nothing to check here
            // Check if the types of the left and right expressions are consistent
            
            break;

        //default:
          //  break;
    }

    // Traverse the AST recursively
   // switch (node->type) {
        case NodeType_program:
            semanticAnalysis(node->program.VarDeclList, symbol_table);
            semanticAnalysis(node->program.StmntList, symbol_table);
            break;

        case NodeType_VarDeclList:
            semanticAnalysis(node->VarDeclList.VarDecl, symbol_table);
            semanticAnalysis(node->VarDeclList.VarDeclList, symbol_table);
            break;

        case NodeType_StmntList:
            semanticAnalysis(node->StmntList.Stmnt, symbol_table);
            semanticAnalysis(node->StmntList.StmntList, symbol_table);
            break;

        default:
            break;
    }
      if (node->type == NodeType_Expr || node->type == NodeType_SimpleExpr) {
        TAC* tac = generateTACForExpr(node);
        // Process or store the generated TAC
        //printTAC(tac);
        appendTAC(&tacHead,tac);
    }

}

// Generate TAC for an expression
// Implementation of the functions
TAC* generateTACForExpr(ASTNode* expr) {
    if (expr == NULL) {
        printf("Error: Expression is NULL.\n");
        return NULL;
    }
    TAC* newTac = (TAC*)malloc(sizeof(TAC));
    if (!newTac) return NULL;
    switch (expr-> type) {
        case NodeType_Expr: {
            printf("Generating TAC for expression\n");
            newTac->op = strdup(expr->Expr.op);  // e.g., "+", "-", "*", etc.
            newTac->arg1 = createOperand(expr->Expr.left);
            newTac->arg2 = createOperand(expr->Expr.right);
            newTac->result = createTempVar();  // Create a temporary variable for the result
            newTac->next = NULL;
            printf("Generated TAC: %s %s %s %s\n", newTac->op, newTac->arg1, newTac->arg2, newTac->result);  // Debug   
            break;
        }
        case NodeType_SimpleExpr: {
            printf("Generating TAC for simple expression\n");
            char buffer[20];
            snprintf(buffer, 20, "%d", expr->SimpleExpr.value);
            newTac->arg1 = strdup(buffer);
            newTac->op = "=";  // Assignment operator
            newTac->arg2 = NULL;
            newTac->result = createTempVar();
            break;
        }
        // Add more cases if necessary...
    }
    return newTac;
}
// Create a temporary variable
char* createTempVar() {
    static int tempCounter = 0;
    char* tempVar = (char*)malloc(10 * sizeof(char));
    snprintf(tempVar, 10, "t%d", tempCounter++);
    return tempVar;
}

// Create operand (either a variable or a literal)
char* createOperand(ASTNode* node) {
    char* operand = (char*)malloc(32 * sizeof(char));
    switch (node->type) {
        case NodeType_SimpleID:
            strcpy(operand, node->SimpleID.id);
            break;
        case NodeType_SimpleExpr:
            snprintf(operand, 32, "%d", node->SimpleExpr.value);
            break;
        default:
            strcpy(operand, "");  // Unknown operand
            break;
    }
    return operand;
}

// Print the TAC (three address code) instructions
void printTAC(TAC* tac) {
    TAC* current = tac;
    while (current != NULL) {
        printf("%s = %s %s %s\n", current->result, current->arg1, current->op, current->arg2);
        current = current->next;
    }
}

// Append a TAC instruction to the linked list
void appendTAC(TAC** head, TAC* newInstruction) {
    if (*head == NULL) {
        *head = newInstruction;
    } else {
        TAC* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newInstruction;
    }
}

// Initialize temporary variables array
void initializeTempVars() {
    for (int i = 0; i < 20; i++) {
        tempVars[i] = 0;  // All temp vars are initially available
    }
}

// Allocate the next available temp var
int allocateNextAvailableTempVar(int tempVars[]) {
    for (int i = 0; i < 20; i++) {
        if (tempVars[i] == 0) {
            tempVars[i] = 1;  // Mark temp var as used
            return i;
        }
    }
    return -1; // No available temp vars
}

// Deallocate a temporary variable
void deallocateTempVar(int tempVars[], int index) {
    if (index >= 0 && index < 20) {
        tempVars[index] = 0;  // Mark temp var as available
    }
}

// Print the TAC instructions to a file
void printTACToFile(const char* filename, TAC* tac) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    TAC* current = tac;
    while (current != NULL) {
        if (current->arg2!=NULL) {
        fprintf(file, "%s = %s %s %s\n", current->result, current->arg1, current->op, current->arg2);
        current = current->next;
        }
        //avoid printing NULL into the TAC when it's simpleExpr 
        else {

        fprintf(file, "%s = %s %s\n", current->result, current->arg1, current->op);
        current = current->next; 
        }
    }

    fclose(file);
}

