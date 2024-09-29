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
             int temp = lookup(symbol_table, node->Stmnt.id);
            printf("Temp: %d\n",temp);
            if (symbol != NULL && node->Stmnt.Expr->type == NodeType_SimpleExpr) {
            // Assuming the expression has an integer literal, update the variable's value
           
            updateValue(symbol_table, node->Stmnt.id, node->Stmnt.Expr->SimpleExpr.value);
            printf("updateValue has no problem\n");
            TAC* newTac = (TAC*)malloc(sizeof(TAC));
            printf("Generating TAC for simple expression\n");
            char buffer[20];
            //char* tempVar= getTempVar(symbol_table,expr->Stmnt.id);
            snprintf(buffer, 20, "%d", node->Stmnt.Expr->SimpleExpr.value);
            newTac->arg1 = strdup(buffer);  // Literal value
            newTac->op = "=";  // Assignment operator
            newTac->arg2 = NULL;
            //Symbol* TACsymbol = getSymbol(symbol_table,expr->Stmnt.id);
           
           // char* temp = getTempVar(symbol_table, expr->Stmnt.id);
            //newTac->result = getTempVar(symbol_table,);
            newTac->result = getTempVar(symbol);
            appendTAC(&tacHead,newTac);
            //newTac->result=createTempVar();
            printf("Generated in generateTAC in simpleExpr\n");
            }

            
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
            //Symbol* symbol = getSymbol(symbol_table, node->Stmnt.id);
            /*
            if (node->Expr.check == NULL){
            node->Expr.check=3;
            } else if (node->Expr.check == 0 ){
                node->Expr.check = 1;
            }
            */
            printf(" check = %d\n", node->Expr.check);
            printf("----------------left-------------\n");
            semanticAnalysis(node->Expr.left, symbol_table);
            printf("----------------right-------------\n");
            semanticAnalysis(node->Expr.right, symbol_table);
            break;

        case NodeType_SimpleID:
            check_variable_initialized(symbol_table, node->SimpleID.id, lines);  // Line number passed as placeholder
            break;

        case NodeType_SimpleExpr:
            

        //default:
           break;
    

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
    if (node->type == NodeType_Expr ) {
        TAC* tac = generateTACForExpr(node,symbol_table);
        // Process or store the generated TAC
        //printTAC(tac);
        appendTAC(&tacHead,tac);
    }
    if ( node->type == NodeType_SimpleExpr){
         
    }

}

TAC* generateTACForExpr(ASTNode* expr, SymbolTable* symbol_table) {
    if (expr == NULL) {
        printf("Error: Expression is NULL.\n");
        return NULL;
    }
    char* tempResult;
    TAC* newTac = (TAC*)malloc(sizeof(TAC));
    if (!newTac) return NULL;
    
    switch (expr->type) {
        case NodeType_Expr: {
            printf("Generating TAC for expression\n");
            newTac->op = strdup(expr->Expr.op);  // e.g., "+", "-", "*", etc.
            newTac->result = createTempVar();
            
            
            
            // Reuse temp vars for operands (x and y)
            newTac->arg1 = createOperand(expr->Expr.left, symbol_table);  // Left operand (e.g., t0 for x)
           
            printf( "TempResult is: %s\n", tempResult);// DEbug
            if (tempResult){
            //if(strcmp(newTac->result, tempResult) == 0){
            newTac->arg2 = createOperand(expr->Expr.right, symbol_table); // Right operand (e.g., t1 for y)
            } else if (strcmp(newTac->result, tempResult) >0){
            newTac->arg2=tempResult;
            printf("tempResult is larger than result\n");
            }

            if( newTac->arg2 ==NULL){
                printf("!!!!!!!!  arg2 is null  !!!!!!!\n");
            }
            /*
            printf("Temp Result is NULL \n");
            } else if (tempResult != NULL) {

            printf(" check = %d\n", expr->Expr.check);
            newTac->arg2 = (char*)tempResult;
            }
            */
            // Create a new temp var for the result of the expression
            
            tempResult = (char*)newTac->result;  
            printf( "TempResult: %s\n", tempResult); //Debug
            printf("GEnerated in generateTAC\n");
            newTac->next = NULL;
            //expr->Expr.check=1;

            printf("Generated TAC: %s = %s %s %s\n", newTac->result, newTac->arg1, newTac->op, newTac->arg2);
            break;
        }
        case NodeType_SimpleExpr: {
         
            break;
        
        }

        default:
            printf("Unhandled expression type\n");
            break;
    }

    return newTac;
}

char* createOperand(ASTNode* node, SymbolTable* symbol_table) {
    char* operand = (char*)malloc(32 * sizeof(char));

    switch (node->type) {
        case NodeType_SimpleID: {
            Symbol* symbol = getSymbol(symbol_table, node->SimpleID.id);
            if (symbol != NULL && symbol->tempVar != NULL) {
                strcpy(operand, symbol->tempVar);
            } else {
                snprintf(operand, 32, "");  // Debug empty case
            //debug
                if(symbol==NULL){
                    printf("symbolNULL\n");
                } else if (symbol->tempVar==NULL){
                    printf("tempVarNULL\n");
                }
                printf("Warning: TempVar not found for SimpleID '%s'\n", node->SimpleID.id);
            }
            break;
        }
        case NodeType_SimpleExpr:
            snprintf(operand, 32, "%d", node->SimpleExpr.value);
            break;
        default:
            strcpy(operand, "");
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

        fprintf(file, "%s = %s\n", current->result, current->arg1);
        current = current->next; 
        }
    }

    fclose(file);
}

// Create a temporary variable
char* createTempVar() {

    static int tempCounter = 0;
    char* tempVar = (char*)malloc(10 * sizeof(char));
    snprintf(tempVar, 10, "t%d", tempCounter++);
     printf("Temp var created: %s\n", tempVar);
    return tempVar;
}
//char* tempVarForSimpExpr(SymbolTable symbol_table, ){