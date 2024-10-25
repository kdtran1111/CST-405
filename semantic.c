#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "Symbol_Table.h"
//#include "parser.y"
char* global_scope = "global";

int tempVars[20];
char* currentID;
char* writeID;
TAC* tacHead = NULL;  // Global head of the TAC instructions list
extern int declaredSymbol;
extern int lines;
OuterSymbolTable* outer_table;
// Error handling function for semantic analysis
void semantic_error(const char* message, int line) {
    fprintf(stderr, "SEMANTIC ERROR (Line %d): %s\n", line, message);
    //exit(1);
}

void def_outer_table_semantic(OuterSymbolTable* outer_table_semantic){
    outer_table= outer_table_semantic;
};
// Check if a variable is declared
//ignore the incorrect parameter
void check_variable_declared(SymbolTable* symbol_table, const char* id, int line) {
    
    SymbolTable* symbol_Table = get_symbol_table(outer_table, global_scope);
    Symbol* symbol = getSymbol(symbol_Table, id);
    if (symbol == NULL) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Variable '%s' used before declaration", id);
        semantic_error(error_message, lines);
    }
}

// Check if a variable has been assigned a value
void check_variable_initialized(SymbolTable* symbol_table, const char* id, int line) {
    SymbolTable* symbol_Table = get_symbol_table(outer_table, global_scope);
    Symbol* symbol = getSymbol(symbol_Table, id);
    if (symbol == NULL) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Variable '%s' used before declaration", id);
        semantic_error(error_message, lines);
    }
    
    if (symbol/*->value */== 0) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Variable '%s' used without being assigned a value, therefore default value is 0", id);
        semantic_error(error_message, lines);
    }
}

// Check for type consistency during assignments
void check_type_consistency(SymbolTable* symbol_table, const char* id, ASTNode* expr, int line) {
    SymbolTable* symbolTable = get_symbol_table(outer_table, global_scope);
    Symbol* symbol = getSymbol(symbolTable, id);
    if (symbol == NULL) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Variable '%s' used before declaration", id);
        semantic_error(error_message, lines);
    }


    if (symbol->type == TYPE_INT && expr->type != NodeType_Expr && expr->type != NodeType_SimpleExpr) {
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
    if (symbol->type == TYPE_FLOAT && expr->type != NodeType_SimpleExpr) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Type mismatch: Variable '%s' is a float, but assigned a non-float value", id);
        semantic_error(error_message, lines);
    }

    // Add more type checks as necessary
}

// Recursive function for semantic analysis and TAC generation
void semanticAnalysis(ASTNode* node, OuterSymbolTable* outer_table) {
    if (node == NULL) return;
    
    switch (node->type) {
        case NodeType_VarDecl:
            // Variable declaration
            break;

        case NodeType_Stmnt:
            currentID  = node->Stmnt.id;
            if (currentID == NULL) {
            fprintf(stderr, "Error: currentID is NULL\n");
            return;
}
            fprintf(stdout,"Semantic analysis for statement, curr ID is %s\n", currentID);
            //Symbol* symbol = getSymbol(symbol_table, node->Stmnt.id);
            printf("Segmentation 1");
            SymbolTable* symbol_Table = get_symbol_table(outer_table, global_scope);
            if (symbol_Table == NULL) {
                fprintf(stderr, "Error: Symbol table not found\n");
                return;
            }
            
            printf("Segmentation 2");
            Symbol* symbol = getSymbol(symbol_Table, currentID);
            if (symbol == NULL) {
                fprintf(stderr, "Error: Symbol not found for ID: %s\n", currentID);
                return;
            }
            printf("Segmentation 3");
            check_variable_declared(symbol_Table, node->Stmnt.id, lines);  // Check if variable is declared
            semanticAnalysis(node->Stmnt.Expr, symbol_Table);        // Analyze the expression on the right-hand side

            // Check for type consistency and initialization
            check_type_consistency(symbol_Table, node->Stmnt.id, node->Stmnt.Expr, lines);
            
            // Fetch the variable symbol to update
            
            //int temp = lookup(symbol_Table, node->Stmnt.id);
            //fprintf(stdout,"Temp: %d\n",temp);
             
            if (symbol != NULL && node->Stmnt.Expr->type == NodeType_SimpleExpr) {
                TAC* newTac = (TAC*)malloc(sizeof(TAC));
            // Assuming the expression has an integer literal, update the variable's value

            updateValue(symbol_Table, node->Stmnt.id, node->Stmnt.Expr->SimpleExpr.value);
            fprintf(stdout,"updateValue has no problem\n");
           
            fprintf(stdout,"Generating TAC for simple expression\n");
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
            fprintf(stdout,"Generated in generateTAC in simpleExpr\n");
            }

            
            // Generate TAC for the assignment statement
           // TAC* tac = generateTACForExpr(node->Stmnt.Expr);
            //appendTAC(&tacHead, tac);
            //fprintf(stdout,"Tac is fine\n");
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
            fprintf(stdout," check = %d\n", node->Expr.check);
            fprintf(stdout,"----------------left-------------\n");
            semanticAnalysis(node->Expr.left, symbol_Table);
            fprintf(stdout,"----------------right-------------\n");
            semanticAnalysis(node->Expr.right, symbol_Table);
            break;

        case NodeType_SimpleID:
            check_variable_initialized(symbol_Table, node->SimpleID.id, lines);  // Line number passed as placeholder
            break;

        case NodeType_SimpleExpr:
        //default:
           break;


        case NodeType_WriteStmnt: {
            // The id of the variable in the write statement is stored in WriteStmnt.id
            currentID = node->WriteStmnt.id;  // Set currentID to the variable being written
            fprintf(stdout,"WriteStmnt: CurrentID is %s\n", currentID);  // Debugging print

            // Fetch the symbol for currentID from the symbol table
            Symbol* symbol = getSymbol(symbol_Table, currentID);
            if (symbol != NULL) {
                // Generate TAC for the write statement
                TAC* newTac = (TAC*)malloc(sizeof(TAC));
                newTac->keyword = strdup("write");

                // Use the tempVar for the write argument
                if (symbol->tempVar != NULL) {
                    newTac->arg1 = strdup(symbol->tempVar);
                } else {
                    semantic_error("Variable used in write statement has no tempVar", lines);
                }

                // Append the TAC for the write statement
                appendTAC(&tacHead, newTac);
            } else {
                semantic_error("Undeclared variable in write statement", lines);
            }

            break;
        }

        /*
        case NodeType_WriteStmnt: {
            
            semanticAnalysis(node->StmntList.Stmnt,symbol_table);
            if (node->StmntList.Stmnt->type == NodeType_SimpleID) {
            currentID = node->StmntList.Stmnt->SimpleID.id;  // Update currentID to 'x'
            } else {
        // Handle other cases, such as expressions
            semantic_error("Invalid statement in write", lines);
            }

            fprintf(stdout,"WriteStmnt: CurrentID is %s\n", currentID);  // Debugging print
            TAC* newTac = (TAC*)malloc(sizeof(TAC));
            
            newTac->keyword =(char*)"write";
            char* write = strdup(newTac->keyword);
            fprintf(stdout,"Write: %s\n",write);
            fprintf(stdout,"WriteStmID: %s\n",currentID);
            newTac->arg1 = symbol->tempVar;
            //char* WriteTempVar = strdup(newTac->arg1);   //this is where segmentation fault happen
            fprintf(stdout,"WriteTempVar: %s\n",symbol->tempVar);
            //char* write = strdup(newTac->keyword);  
            //fprintf(stdout,"Write: %s\n",write);
            appendTAC(&tacHead,newTac);
           /*
            //Symbol* symbol = getSymbol(symbol_table, currentID);
            newTac->arg1 = symbol->tempVar;
            fprintf(stdout,"Symbol: %s\n", symbol->tempVar);
            fprintf(stdout,"WriteTempVar: %s\n", symbol);
            appendTAC(&tacHead,newTac);
            
            break;
        }
        */
    // Traverse the AST recursively
   // switch (node->type) {
        case NodeType_program:
            semanticAnalysis(node->program.VarDeclList, symbol_Table);
            semanticAnalysis(node->program.StmntList, symbol_Table);
            break;

        case NodeType_VarDeclList:
            semanticAnalysis(node->VarDeclList.VarDecl, symbol_Table);
            semanticAnalysis(node->VarDeclList.VarDeclList, symbol_Table);
            break;

        case NodeType_StmntList:
            semanticAnalysis(node->StmntList.Stmnt, symbol_Table);
            semanticAnalysis(node->StmntList.StmntList, symbol_Table);
            break;

        default:
            break;
    }
    if (node->type == NodeType_Expr ) {
        TAC* tac = generateTACForExpr(node,outer_table);
        // Process or store the generated TAC
        //printTAC(tac);
        appendTAC(&tacHead,tac);
    }
    if ( node->type == NodeType_SimpleExpr){
         
    }

}

TAC* generateTACForExpr(ASTNode* expr, OuterSymbolTable* outer_table) {
    SymbolTable* symbol_Table = get_symbol_table(outer_table, global_scope);
    Symbol* symbol = getSymbol(symbol_Table, currentID);
    if (expr == NULL) {
        fprintf(stdout,"Error: Expression is NULL.\n");
        return NULL;
    }
    char* tempResult;
    TAC* newTac = (TAC*)malloc(sizeof(TAC));
    if (!newTac) return NULL;
    
    switch (expr->type) {
        case NodeType_Expr: {
            fprintf(stdout,"Generating TAC for expression\n");
            newTac->op = strdup(expr->Expr.op); // e.g., "+", "-", "*", etc.
            tempResult = createTempVar();

            newTac->result = strdup(tempResult);
            updateRegister(symbol_Table, currentID, newTac->result);
            
            // Reuse temp vars for operands (x and y)
            newTac->arg1 = createOperand(expr->Expr.left, symbol_Table);  // Left operand (e.g., t0 for x)
           
            fprintf(stdout, "TempResult is: %s\n", tempResult);// DEbug
            if (tempResult){
            //if(strcmp(newTac->result, tempResult) == 0){
            newTac->arg2 = createOperand(expr->Expr.right, symbol_Table); // Right operand (e.g., t1 for y)
            } else if (strcmp(newTac->result, tempResult) >0){
            newTac->arg2=tempResult;
            fprintf(stdout,"tempResult is larger than result\n");
            }

            if( newTac->arg2 ==NULL){
                fprintf(stdout,"!!!!!!!!  arg2 is null  !!!!!!!\n");
            }
            /*
            fprintf(stdout,"Temp Result is NULL \n");
            } else if (tempResult != NULL) {

            fprintf(stdout," check = %d\n", expr->Expr.check);
            newTac->arg2 = (char*)tempResult;
            }
            */
            // Create a new temp var for the result of the expression
            
            tempResult = (char*)newTac->result;  
            fprintf(stdout, "TempResult: %s\n", tempResult); //Debug
            fprintf(stdout,"GEnerated in generateTAC\n");
            newTac->next = NULL;
            //expr->Expr.check=1;

            fprintf(stdout,"Generated TAC: %s = %s %s %s\n", newTac->result, newTac->arg1, newTac->op, newTac->arg2);
            break;
        }
        case NodeType_SimpleExpr: {
         
            break;
        
        }

        default:
            fprintf(stdout,"Unhandled expression type\n");
            break;
    }

    return newTac;
}

char* createOperand(ASTNode* node, SymbolTable* symbol_table) {
    char* operand = (char*)malloc(32 * sizeof(char));

    switch (node->type) {
        case NodeType_SimpleID: {
            Symbol* symbol = getSymbol(symbol_table, node->SimpleID.id); //------
            if (symbol != NULL && symbol->tempVar != NULL) {
                strcpy(operand, symbol->tempVar);
            } else {
                snprintf(operand, 32, "");  // Debug empty case
            //debug
                if(symbol==NULL){
                    fprintf(stdout,"symbolNULL\n");
                } else if (symbol->tempVar==NULL){
                    fprintf(stdout,"tempVarNULL\n");
                }
                fprintf(stdout,"Warning: TempVar not found for SimpleID '%s'\n", node->SimpleID.id);
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
        fprintf(stdout,"%s = %s %s %s\n", current->result, current->arg1, current->op, current->arg2);
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
    fprintf(stdout,"--------------Printing TAC to file---------------\n");

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    int cntr = 0;
    TAC* previous = NULL;
    TAC* current = tac;
    while (current != NULL) {

        if (current->arg2!=NULL && current->arg1!=NULL) 
        {

            if (strcmp(current->arg2, "") == 0) 
            {
                current->arg2 = previous->result;    
            }

            fprintf(file, "%s = %s %s %s\n", current->result, current->arg1, current->op, current->arg2);
            fprintf(stdout,"%s = %s %s %s\n", current->result, current->arg1, current->op, current->arg2);
        }
        //avoid printing NULL into the TAC when it's simpleExpr 
        else if ( current->result!=NULL) {
        
        fprintf(file, "%s = %s\n", current->result, current->arg1);
        fprintf(stdout,"%s = %s\n", current->result, current->arg1);
        } else{
        fprintf(file, "%s %s\n", current->keyword, current->arg1);
        fprintf(stdout,"%s %s\n", current->keyword, current->arg1);
        }

        if (cntr >= 1)
        {
            previous = current;
            current = current->next;
        }

        else 
        {
            current = current->next; 
        }
        cntr++;
    }

    fclose(file);
}

// Create a temporary variable
char* createTempVar() {

    static int tempCounter = 0;
    char* tempVar = (char*)malloc(10 * sizeof(char));
    snprintf(tempVar, 10, "t%d", tempCounter++);
    fprintf(stdout,"Temp var created: %s\n", tempVar);
    return tempVar;
}
