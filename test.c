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
void semantic_error(const char* message, int lines) {
    fprintf(stderr, "SEMANTIC ERROR (Line %d): %s\n", lines, message);
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
        semantic_error(error_message, line);
        return;
    }

    // Print debug information
    printf("Debug: Variable '%s' has type %s, expr->type is %d\n", id, symbol->type_str, expr->type);
    //Check for Float type
    if (strcmp(symbol->type_str, "FLOAT") == 0 && expr->type != NodeType_SimpleFloat) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Type mismatch: Variable '%s' is a float, but assigned a non-float value", id);
        semantic_error(error_message, lines);
        return;
    }
    //Check for Int type
    if (strcmp(symbol->type_str, "INT")==0  && expr->type != NodeType_Expr && expr->type != NodeType_SimpleExpr) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Type mismatch: Variable '%s' is an int, but assigned a non-integer value", id);
        semantic_error(error_message, lines);
        return;
    }
    
      //Check for String type
    if (strcmp(symbol->type_str, "STRING")==0   && expr->type != NodeType_SimpleString) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Type mismatch: Variable '%s' is an int, but assigned a non-integer value", id);
        semantic_error(error_message, lines);
        return;
    }
    
    // Add more type checks as necessary
}
// Check if the array is already declared in the symbol table
void check_array_not_redeclared(SymbolTable* table, const char* id, int line) {
    if (lookup_symbol(table, id) != 0) {
        char errorMsg[100];
        snprintf(errorMsg, sizeof(errorMsg), "Array '%s' has already been declared.", id);
        semantic_error(errorMsg, line);
    }
}

// Check if the array type is valid
void check_array_type(const char* type, int line) {
    if (strcmp(type, "INT_ARRAY") != 0 && strcmp(type, "FLOAT_ARRAY") != 0 && strcmp(type, "STRING_ARRAY") != 0) {
        char errorMsg[100];
        snprintf(errorMsg, sizeof(errorMsg), "Invalid array type '%s'. Allowed types are int, float, and string.", type);
        semantic_error(errorMsg, line);
    }
}

// Check if the array size is valid (positive integer)
void check_array_size(int size, int line) {
    if (size <= 0) {
        char errorMsg[100];
        snprintf(errorMsg, sizeof(errorMsg), "Invalid array size '%d'. Size must be a positive integer.", size);
        semantic_error(errorMsg, line);
    }
}

// Main function to perform all semantic checks on an array declaration
void check_array_declaration(SymbolTable* table, const char* type, const char* id, int size, int line) {
    Symbol* symbol =getSymbol(table,id);
    //check_array_not_redeclared(table, id, line);
    check_array_type(symbol->type_str, line);
    check_array_size(size, line);
}
//Function to do type coercion
void apply_type_coercion(SymbolTable* symbol_table, const char* id, ASTNode* expr, int line) {
    SymbolTable* symbolTable = get_symbol_table(outer_table, global_scope);
    Symbol* symbol = getSymbol(symbolTable, id);
    if (symbol == NULL) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Variable '%s' used before declaration", id);
        semantic_error(error_message, line);
        return;
    }
    printf("Debug: Applying coercion for variable '%s' of type %s with expr type %d\n", 
           id, symbol->type_str, expr->type);
    printf("Before coercion check: symbol->type_str = %s, expr->type = %d\n", symbol->type_str, expr->type);
    char* expectedType = expr->Stmnt.expectedType;
    printf("======value type====== %u\n", expr->type);
    
    if (strcmp(symbol->type_str, "FLOAT") == 0 && (expr->type == NodeType_Expr | expr->type == NodeType_SimpleExpr)) {    
        printf("Coercing integer to float for variable '%s'\n", id);
        expr->SimpleFloat.value = (float)expr->SimpleExpr.value;
        printf("===FLoat.value = %f===\n", expr->SimpleFloat.value);
        expr->type = NodeType_SimpleFloat;
         printf("===symbol ID: %s type_str = %s===symbol value %f====\n",id, symbol->type_str,symbol->value.floatValue);
        expr->type = NodeType_SimpleFloat;
        printf("After coercion check: symbol->type_str = %s, expr->type = %d\n", symbol->type_str, expr->type);
    } 
    else if (strcmp(symbol->type_str, "INT") == 0 && expr->type == NodeType_SimpleExpr){
        // This case should handle an integer directly
        printf("No coercion needed for variable '%s', assigning value directly.\n", id);
        expr->type = NodeType_SimpleExpr;  // Ensure the type is correctly set to SimpleExpr
    } 
    else if (strcmp(symbol->type_str, "INT") == 0 && (expr->type == NodeType_Expr | expr->type == NodeType_SimpleExpr)) {
        printf("Coercing float to integer for variable '%s'\n", id);
        expr->SimpleExpr.value = (int)expr->SimpleFloat.value;  // Actual conversion
        expr->type = NodeType_SimpleExpr;  // Explicitly set type to int post-conversion
    } else if(strcmp(symbol->type_str, "STRING") == 0 && expr->type == NodeType_SimpleString){
        expr->type = NodeType_SimpleString; 
        symbol->type=  TYPE_STRING;  
        printf("After coercion check: symbol->type_str = %s, expr->type = %d symbol.type= %u\n", symbol->type_str, expr->type, symbol->type);
        
    } else {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Type coercion failed: Variable '%s' is of incompatible type", id);
        semantic_error(error_message, line);
    }
}

// Recursive function for semantic analysis and TAC generation
void semanticAnalysis(ASTNode* node, OuterSymbolTable* outer_table) {
    if (node == NULL) return;
    
    //Debug: printf("semanticAnalysis: Processing node of type %d\n", node->type);
    SymbolTable* symbol_Table = get_symbol_table(outer_table, global_scope);
    switch (node->type) {
        
        case NodeType_Stmnt:
            currentID  = node->Stmnt.id;
            if (currentID == NULL) {
            fprintf(stderr, "Error: currentID is NULL\n");
            return;
}
            fprintf(stdout,"Semantic analysis for statement, curr ID is %s\n", currentID);
            //Symbol* symbol = getSymbol(symbol_table, node->Stmnt.id);           
            if (symbol_Table == NULL) {
                fprintf(stderr, "Error: Symbol table not found\n");
                return;
            }            
            Symbol* symbol = getSymbol(symbol_Table, currentID);
            if (symbol == NULL) {
                fprintf(stderr, "Error: Symbol not found for ID: %s\n", currentID);
                return;
            }           
            check_variable_declared(symbol_Table, node->Stmnt.id, lines);  // Check if variable is declared
            semanticAnalysis(node->Stmnt.Expr, outer_table);        // Analyze the expression on the right-hand side

            // Check for type consistency and initialization
            check_type_consistency(symbol_Table, node->Stmnt.id, node->Stmnt.Expr, lines);
            apply_type_coercion(symbol_Table, node->Stmnt.id, node->Stmnt.Expr, lines);
            
          
            
             // Update variable value if the expression has a simple value
        if (symbol != NULL && (node->Stmnt.Expr->type == NodeType_SimpleExpr || node->Stmnt.Expr->type == NodeType_SimpleFloat || node->Stmnt.Expr->type == NodeType_SimpleString)) {
            TAC* newTac = (TAC*)malloc(sizeof(TAC));

            // Prepare the new value based on the type
            VarValue newValue;
            VarType varType;
            char buffer[20];
            if (node->Stmnt.Expr->type == NodeType_SimpleFloat) {
            varType = TYPE_FLOAT;
            printf("DEBUG: Node type is SimpleFloat. Value: %f\n", node->Stmnt.Expr->SimpleFloat.value);
            } else if (node->Stmnt.Expr->type == NodeType_SimpleExpr) {
                varType = TYPE_INT;
                printf("DEBUG: Node type is SimpleExpr. Value: %d\n", node->Stmnt.Expr->SimpleExpr.value);
            } else if (node->Stmnt.Expr->type == NodeType_SimpleString){
                varType = TYPE_STRING;
            }else{
                printf("DEBUG: Node type is unrecognized: %d\n", node->Stmnt.Expr->type); // Add this to catch unexpected types
            }
            if (varType == TYPE_FLOAT) {
                printf("DEBUG: Assigning float value: %f to symbol '%s'\n", node->Stmnt.Expr->SimpleFloat.value, symbol->id);
                newValue.floatValue = node->Stmnt.Expr->SimpleFloat.value;
            } else if (varType == TYPE_INT) {
                printf("DEBUG: Assigning int value: %d to symbol '%s'\n", node->Stmnt.Expr->SimpleExpr.value, symbol->id);
                newValue.intValue = node->Stmnt.Expr->SimpleExpr.value; // Consider casting to float
            }else if(varType == TYPE_STRING){
                printf("DEBUG: Assigning int value: %s to symbol '%s'\n", node->Stmnt.Expr->SimpleString.value, symbol->id);
                newValue.stringValue = node->Stmnt.Expr->SimpleString.value; // Consider casting to float
            }

            // Call the new updateValue function
            updateValue(symbol_Table, currentID, newValue, varType);
            fprintf(stdout, "updateValue has no problem\n");  
            fprintf(stdout, "Generating TAC for simple expression\n");                
            // Generate TAC
            if (varType == TYPE_FLOAT) {
                snprintf(buffer, 20, "%f", newValue.floatValue);
            }else if (varType == TYPE_INT) {
                snprintf(buffer, 20, "%d", newValue.intValue);
            }else if(varType == TYPE_STRING){
                snprintf(buffer, 20, "%s", newValue.stringValue);
            }
            newTac->arg1 = strdup(buffer);  // Literal value
            newTac->op = "=";               // Assignment operator
            newTac->arg2 = NULL;
            newTac->result = getTempVar(symbol);
            appendTAC(&tacHead, newTac);

            fprintf(stdout, "Generated in generateTAC in simpleExpr\n");
        }
            break;
        case NodeType_Expr:
            // Recursively analyze left and right expressions
            
            //fprintf(stdout," check = %d\n", node->Expr.check);
            fprintf(stdout,"----------------left-------------\n");
            semanticAnalysis(node->Expr.left, outer_table);
            fprintf(stdout,"----------------right-------------\n");
            semanticAnalysis(node->Expr.right, outer_table);
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

    // Traverse the AST recursively
  
        case NodeType_program:
            /*
            */
            printf("Traversing StructDeclList\n");
            semanticAnalysis(node->program.StructDeclList, outer_table);
            printf("Traversing FuncDeclList\n");
            semanticAnalysis(node->program.FuncDeclList, outer_table);
            printf("Traversing VarDeclList in program\n");
            semanticAnalysis(node->program.VarDeclList, outer_table);
            printf("Traversing StmntList in program\n");
            semanticAnalysis(node->program.StmntList, outer_table);
            
            break;

        case NodeType_VarDeclList:
            /*semanticAnalysis(node->VarDeclList.VarDecl, outer_table);
            semanticAnalysis(node->VarDeclList.VarDeclList, outer_table);
            break;
            
            printf("Processing VarDeclList node\n");
        
                semanticAnalysis(node->VarDeclList.VarDecl, outer_table);  // Process the actual declaration
                node = node->VarDeclList.VarDeclList;  // Move to the next VarDecl in the list
        
            break;
            */
            //printf("Traversing VarDeclList in program\n");
            //semanticAnalysis(node->program.VarDeclList, outer_table);
        case NodeType_StmntList:
            semanticAnalysis(node->StmntList.Stmnt, outer_table);
            semanticAnalysis(node->StmntList.StmntList, outer_table);
            break;


        // New cases for remaining NodeTypes
        case NodeType_SimpleFloat:
            // Handle float constant or variable logic here
            break;

        case NodeType_SimpleString:
            // Handle string constant or variable logic here
            break;

        case NodeType_SimpleArrIndex:
            // Array indexing logic, such as bounds checking
            break;

        case NodeType_SimpleStructMember:
            // Handle struct member access logic
            break;

        case NodeType_Operand:
            // Handle operand analysis logic here
            break;

        case NodeType_FuncDeclList:
            // Function declaration list traversal
            semanticAnalysis(node->FuncDeclList.FuncDecl, outer_table);
            semanticAnalysis(node->FuncDeclList.FuncDeclList, outer_table);
            break;

        case NodeType_FuncDecl:
            // Analyze function declarations here
            break;

        case NodeType_ParamList:
            // Traverse parameter list
            semanticAnalysis(node->ParamList.Param, outer_table);
            semanticAnalysis(node->ParamList.ParamList, outer_table);
            break;

        case NodeType_Param:
            // Analyze individual parameters
            break;

        case NodeType_ReturnStmnt:
            // Analyze return statement and check type consistency
            break;

        case NodeType_IndexAssignment:
            // Array index assignment logic
            // Perform array assignment semantic checks
            check_array_declaration(symbol_Table, node->VarDecl.type, node->IndexAssignment.id, node->VarDecl.size, lines);
                // Perform array semantic checks
               // check_array_declaration(symbol_Table, node->VarDecl.type, node->VarDecl.id, node->VarDecl.size, lines);
                
            break;

        case NodeType_ValueList:
            // Traverse list of values (array or other collection)
            semanticAnalysis(node->ValueList.Val, outer_table);
            semanticAnalysis(node->ValueList.ValueList, outer_table);
            break;

        case NodeType_ArrAssignment:
            // Array assignment logic
            break;

        case NodeType_StructDeclList:
            // Struct declaration list traversal
            semanticAnalysis(node->StructDeclList.StructDecl, outer_table);
            semanticAnalysis(node->StructDeclList.StructDeclList, outer_table);
            break;

        case NodeType_StructDecl:
            // Struct declaration logic
            break;

        case NodeType_StructMemberAssignment:
            // Struct member assignment logic
            break;

        case NodeType_FunctionCall:
            // Handle function call, check arguments and return type
            break;

        case NodeType_TypeCast:
            // Type casting logic
            break;
        case NodeType_VarDecl:
            
             // If this is an array declaration, perform semantic checks
            /*printf("Semantic analysis for array declaration: %s\n", node->VarDecl.id);
            
            if (node->VarDecl.declared==1){
                check_array_not_redeclared(symbol_Table, node->VarDecl.id, lines);
            }else{
                printf("Array declared successful:\n");
                node->VarDecl.declared=1;
            }
            */
            break;

        default:
            break;
    }
    printf("==========      current NodeType is %d=     ========\n", node->type);
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
         
            // Create a new temp var for the result of the expression
            
            tempResult = (char*)newTac->result;  
            fprintf(stdout, "TempResult: %s\n", tempResult); //Debug
            fprintf(stdout,"Generated in generateTAC\n");
            newTac->next = NULL;
            //expr->Expr.check=1;

            fprintf(stdout,"Generated TAC: %s = %s %s %s\n", tempResult /*newTac->result*/, newTac->arg1, newTac->op, newTac->arg2);
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

            if (strcmp(current->arg1, "") == 0) 
            {
                current->arg1 = previous->result;    
            }

            fprintf(file, "%s = %s %s %s\n", current->result, current->arg1, current->op, current->arg2);
            fprintf(stdout,"%s = %s %s %s\n", current->result, current->arg1, current->op, current->arg2);
        }

        //avoid printing NULL into the TAC when it's simpleExpr 
        else if ( current->result!=NULL) {
        
        fprintf(file, "%s = %s\n", current->result, current->arg1);
        fprintf(stdout,"%s = %s\n", current->result, current->arg1);
        } 
        
        else if (strcmp(current->keyword, "write") == 0)
        {
        fprintf(file, "%s %s\n", current->keyword, current->arg1);
        fprintf(stdout,"%s %s\n", current->keyword, current->arg1);
        }

        else if(strcmp(current->keyword, "jump") == 0)
        {
            fprintf(file, "%s %s\n", current->keyword, current->arg1);
            fprintf(stdout,"%s %s\n", current->keyword, current->arg1);
        }

        else
        {
            fprintf(file, "\n%s:\n", current->keyword);
            fprintf(stdout,"\n%s:\n", current->keyword);
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