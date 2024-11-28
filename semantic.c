#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "Symbol_Table.h"
//#include "parser.y"
char* global_scope = "global";

char* curr_scope = "global";
char* tempBoolOp;
int opChecker =0 ; // variable to check if the op is a boolean operator, for labeling purpose, 1 for yes, 0 for no
SymbolTable* symbol_Table;
int paramCounter = 0; // Variable to keep track registers for parameters
char* labelTrue;
char* labelFalse;

int tempIndex;
int tempVars[20];
int paramVars[3];
int main_declared = 0; // Variable to keep track if the main function has been declared
char* currentID;
char* writeID;
char* structID;
char* structMemberID;
int structMemberAssignment = 0; // to keep track of if we are traversing the structMemberAssignment.
Symbol* structMemberIDSymbol;
Symbol* structIDSymbol;
TAC* tacHead = NULL;  // Global head of the TAC instructions list
extern int declaredSymbol;
extern int lines;
TAC* arrTacRegister = NULL; /// to store TAC for array Register ( i.e. )

//OuterSymbolTable* outer_table;

// Variables to keep track of parameter passing
LinkedListNode* curr_param;
int function_called = 0; 

// Variables to keep track of array assignment
int array_assigned = 0;
int array_counter = 0;
char* curr_array;

// Variable to keep track of stmnt start
int stmnt_started = 0;

OuterSymbolTable* outer_table_semantic;

char* createParameterVar() { 
    char* paramVar = (char*)malloc(10 * sizeof(char));
    snprintf(paramVar, 10, "a%d", paramCounter++);
    fprintf(stdout,"Parameter var created: %s\n", paramVar);
    return paramVar;
}

// Function to get head for array linked list
LinkedListNode* populate_data_section(OuterSymbolTable* outer_table_semantic)
{

    print_table(outer_table_semantic);
    LinkedListNode* head = create_array_linked_list(outer_table_semantic);
    print_array_linked_list(head);

    return head;
}

// Error handling function for semantic analysis
void semantic_error(const char* message, int lines) {
    fprintf(stderr, "SEMANTIC ERROR (Line %d): %s\n", lines, message);
    //exit(1);
}


void def_outer_table_semantic(OuterSymbolTable* outer_table_semantic){
    outer_table_semantic= outer_table_semantic;
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

        if (current->arg2!=NULL && current->arg1!=NULL && current->keyword ==NULL) 
        {

            if (strcmp(current->arg1, "") == 0) 
            {
                current->arg1 = previous->result;    
            }

            fprintf(file, "%s = %s %s %s\n", current->result, current->arg1, current->op, current->arg2);
            fprintf(stdout,"%s = %s %s %s\n", current->result, current->arg1, current->op, current->arg2);

        }

        else if(current->keyword != NULL)
        {
            if (strcmp(current->keyword, "data_for_array") == 0)
            {
                fprintf(file, "%s: .space %s\n", current->result, current->arg1);
                fprintf(stdout, "%s: .space %s\n", current->result, current->arg1);
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

            else if(strcmp(current->keyword, "data") == 0)
            {
                fprintf(file, ".%s\n", current->keyword);
                fprintf(stdout,".%s\n", current->keyword);
            }
            else if(strcmp(current->keyword, "text") == 0)
            {
                fprintf(file, "\n.%s\n", current->keyword);
                fprintf(stdout,"\n.%s\n", current->keyword);
            }
            else if(strcmp(current->keyword, "globl") == 0)
            {
                fprintf(file, "\n.%s\n", current->keyword);
                fprintf(stdout,"\n.%s\n", current->keyword);
            }

            else if(strcmp(current->keyword, "arr_decl") == 0)
            {
                fprintf(file, "%s = %s\n", current->result, current->arg1);
                fprintf(stdout,"%s = %s\n", current->result, current->arg1);
            }

            else if(strcmp(current->keyword, "struct_decl") == 0)
            {
                fprintf(file, "%s = %s\n", current->result, current->arg1);
                fprintf(stdout,"%s = %s\n", current->result, current->arg1);
            }

            else if(strcmp(current->keyword, "array_assign") == 0)
            {
                fprintf(file, "%s = %s\n", current->result, current->arg1);
                fprintf(stdout,"%s = %s\n", current->result, current->arg1);
            }
            else if(strcmp(current->keyword, "parameterPass") == 0)
            {
                fprintf(file, "%s = %s\n", current->result, current->arg1);
                fprintf(stdout,"%s = %s\n", current->result, current->arg1);
            }
            else if(strcmp(current->keyword, "cond") == 0)
            {
                fprintf(file, "%s = %s\n", current->result, current->arg1);
                fprintf(stdout,"%s = %s\n", current->result, current->arg1);
            }
            else if(strcmp(current->keyword, "label") == 0)
            {                
                fprintf(file, "IF %s %s %s GOTO %s\n", current->result, current->op , current->arg1, current->arg2);
                fprintf(stdout,"IF %s %s %s GOTO %s\n", current->result, current->op , current->arg1, current->arg2);
            }
            else 
            {
                fprintf(file, "\n%s:\n", current->keyword);
                fprintf(stdout,"\n%s:\n", current->keyword);
            }
        }

        //avoid printing NULL into the TAC when it's simpleExpr 
        else if( current->result!=NULL) {
        
        fprintf(file, "%s = %s\n", current->result, current->arg1);
        fprintf(stdout,"%s = %s\n", current->result, current->arg1);
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
    fprintf(file, "\n");
    fprintf(stdout, "\n");

    fclose(file);

    fprintf(stdout, "finsihed pinting to file\n");
}



void def_outer_table_semantic1(OuterSymbolTable* outer_table_semantic){
    outer_table_semantic= outer_table_semantic;

};
// Check if a variable is declared
//ignore the incorrect parameter
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
    
    if (symbol/*->value */== 0) {
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
        semantic_error(error_message, line);
        return;
    }

    // Print debug information
    printf("Debug: Variable '%s' has type %s, expr->type is %d\n", id, symbol->type_str, expr->type);
    //Check for Float type
    if (strcmp(symbol->type_str, "float") == 0 && expr->type != NodeType_SimpleFloat) {

        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Type mismatch: Variable '%s' is a float, but assigned a non-float value", id);
        semantic_error(error_message, lines);
        return;
    }
    //Check for Int type
    if (strcmp(symbol->type_str, "int")==0  && expr->type != NodeType_Expr && expr->type != NodeType_SimpleExpr) {

        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Type mismatch: Variable '%s' is a float, but assigned a non-float value", id);
        semantic_error(error_message, lines);
        return;
    }

      //Check for String type
    if (strcmp(symbol->type_str, "string")==0   && expr->type != NodeType_SimpleString) {

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
        // temp suppress: semantic_error(errorMsg, line);
    }
}

// Check if the array type is valid
void check_array_type(const char* type, int line) {
    if (strcmp(type, "INT_ARRAY") != 0 && strcmp(type, "FLOAT_ARRAY") != 0 && strcmp(type, "STRING_ARRAY") != 0) {
        char errorMsg[100];
        snprintf(errorMsg, sizeof(errorMsg), "Invalid array type '%s'. Allowed types are int, float, and string.", type);
        // temp suppress:semantic_error(errorMsg, line);
    }
}

// Check if the array size is valid (positive integer)
void check_array_size(int size, int line) {
    if (size <= 0) {
        char errorMsg[100];
        snprintf(errorMsg, sizeof(errorMsg), "Invalid array size '%d'. Size must be a positive integer.", size);
        // temp suppress:semantic_error(errorMsg, line);
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
    Symbol* symbol = getSymbol(symbol_table, id);
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
        //expr->type = NodeType_SimpleFloat;
         printf("===symbol ID: %s type_str = %s===symbol value %f====\n",id, symbol->type_str,symbol->value.floatValue);
        expr->type = NodeType_SimpleFloat;
        printf("After coercion check: symbol->type_str = %s, expr->type = %d\n", symbol->type_str, expr->type);
    } 
    else if (strcmp(symbol->type_str, "INT") == 0 && expr->type == NodeType_SimpleExpr){
        // This case should handle an integer directly
        printf("No coercion needed for variable '%s', assigning value directly.\n", id);
        //expr->type = NodeType_SimpleExpr;  // Ensure the type is correctly set to SimpleExpr
    } 
    else if (strcmp(symbol->type_str, "INT") == 0 && (expr->type == NodeType_Expr | expr->type == NodeType_SimpleExpr)) {
        printf("Coercing float to integer for variable '%s'\n", id);
        expr->SimpleExpr.value = (int)expr->SimpleFloat.value;  // Actual conversion
        //expr->type = NodeType_SimpleExpr;  // Explicitly set type to int post-conversion
    } else if(strcmp(symbol->type_str, "STRING") == 0 && expr->type != NodeType_SimpleString){
        //expr->type = NodeType_SimpleString; 
        printf("are you stupid, a string can't be coerce with number\n");
        exit(1);
        symbol->type=  TYPE_STRING;  
        printf("After coercion check: symbol->type_str = %s, expr->type = %d symbol.type= %u\n", symbol->type_str, expr->type, symbol->type);
        
    } else {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Type coercion failed: Variable '%s' is of incompatible type     are you stupid, a string can't be coerce with number\n", id);
        semantic_error(error_message, line);
    }
}

TAC* generate_arrDecl_tac(ASTNode* node) {
    printf("=======generated in generate_arrDecl_tac\n=======");
    char* tempResult;
    TAC* arrDecl_tac = (TAC*)malloc(sizeof(TAC));

    arrDecl_tac->keyword = strdup("arr_decl");
    tempResult = createTempVar();
    arrDecl_tac->result = strdup(tempResult);
    arrDecl_tac->arg1 = strdup(node->VarDecl.id);

    return arrDecl_tac;
}


TAC* generate_structDecl_tac(ASTNode* node) 
{
    char* tempResult;
    TAC* structDecl_tac = (TAC*)malloc(sizeof(TAC));

    structDecl_tac->keyword = strdup("struct_decl");
    tempResult = createTempVar();
    structDecl_tac->result = strdup(tempResult);
    structDecl_tac->arg1 = strdup(node->VarDecl.id);

    return structDecl_tac;
}


// Recursive function for semantic analysis and TAC generation
void semanticAnalysis(ASTNode* node, OuterSymbolTable* outer_table_semantic) {
    if (node == NULL) return;
    
    //printf("semanticAnalysis: Processing node of type %d\n", node->type);
    //SymbolTable* 
    // make the "global" symbol_Table global
    symbol_Table = get_symbol_table(outer_table_semantic, curr_scope);
    switch (node->type) {

        case NodeType_VarDecl:

            fprintf(stdout, "\nEntering NodeType_VarDecl: %s, current scope is %s\n", node->VarDecl.id, curr_scope);
             

            // If this is an array declaration, perform semantic checks
            if (node->VarDecl.size > 0) {  // Assuming size > 0 indicates an array
                printf("Semantic analysis for array declaration: %s\n", node->VarDecl.id);
                check_array_declaration(symbol_Table, node->VarDecl.type, node->VarDecl.id, node->VarDecl.size, lines);

                TAC* arrDecl_tac = generate_arrDecl_tac(node);

                appendTAC(&tacHead, arrDecl_tac);

                Symbol* symbol = getSymbol(symbol_Table, node->VarDecl.id);
                symbol->arrayDeclVar = strdup(arrDecl_tac->result);

            }

            else if (lookup_scope(outer_table_semantic,node->VarDecl.type) == 1)
            {
                TAC* structDecl_tac = generate_structDecl_tac(node);

                appendTAC(&tacHead, structDecl_tac);
                fprintf(stdout, "%s = %s\n", structDecl_tac->result, structDecl_tac->arg1);

                //update_struct_variable_registers(SymbolTable* symbol_Table, structDecl_tac->result, structDecl_tac->arg1);

                Symbol* struct_symbol = getSymbol(symbol_Table, node->VarDecl.id);
                struct_symbol->tempVar = strdup(structDecl_tac->result);
                update_struct_variable_registers(getSymbol(symbol_Table, node->VarDecl.id)->value.structValue, struct_symbol->tempVar);
                print_symbol_table(getSymbol(symbol_Table, node->VarDecl.id)->value.structValue, 0);
                
            }
        
            break;
        case NodeType_Stmnt:

            printf("Went into Stmnt case\n");
            stmnt_started = 1;
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
            semanticAnalysis(node->Stmnt.Expr, outer_table_semantic);        // Analyze the expression on the right-hand side
            fprintf(stdout, "HERERE\n"); 
            // Check for type consistency and initialization
            //check_type_consistency(symbol_Table, node->Stmnt.id, node->Stmnt.Expr, lines);

            //apply_type_coercion(symbol_Table, node->Stmnt.id, node->Stmnt.Expr, lines);
  
            
          
            
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
            


            if (strcmp(symbol->type_str, "int") ==0 && varType == TYPE_FLOAT) {
                snprintf(buffer, 20, "%d", (int)newValue.floatValue);
            }else if (strcmp(symbol->type_str, "int") ==0 && varType == TYPE_INT) {
                snprintf(buffer, 20, "%d", newValue.intValue);

            } else if (strcmp(symbol->type_str, "float") ==0 && varType == TYPE_FLOAT) {
                snprintf(buffer, 20, "%f", newValue.floatValue);
            }else if (strcmp(symbol->type_str, "float") ==0 && varType == TYPE_INT) {
                snprintf(buffer, 20, "%f", (float)newValue.intValue);   
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
            
            fprintf(stdout,"----------------left-------------\n");
            semanticAnalysis(node->Expr.left, outer_table_semantic);
            fprintf(stdout,"----------------right-------------\n");
            semanticAnalysis(node->Expr.right, outer_table_semantic);
            printf("aslkdnkasjdbka");
            break;

        case NodeType_SimpleID:
            if (function_called == 1)
            {
                generate_param_pass_tac(node, symbol_Table);
                curr_param = curr_param->next;
            }

            if (array_assigned == 1)
            {
                generate_array_assign_tac(node, symbol_Table);
                array_counter += 1;
            }

            check_variable_initialized(symbol_Table, node->SimpleID.id, lines);

            break;

        case NodeType_SimpleExpr:
            if (function_called == 1)
            {
                generate_param_pass_tac(node, symbol_Table);
                curr_param = curr_param->next;
            }

            if (array_assigned == 1)
            {
                generate_array_assign_tac(node, symbol_Table);
                array_counter += 1;
            }
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
  

        case NodeType_StmntList:
            stmnt_started = 0;
            semanticAnalysis(node->StmntList.Stmnt, outer_table_semantic);
            semanticAnalysis(node->StmntList.StmntList, outer_table_semantic);
            break;


        case NodeType_program: {
            // Create data section for arrays
            TAC* DataTac = (TAC*)malloc(sizeof(TAC));
            DataTac->keyword = strdup("data");
            appendTAC(&tacHead, DataTac);
            LinkedListNode* head = populate_data_section(outer_table_semantic);
            
            int size;
            char *size_string = malloc(20 * sizeof(char));

            TAC* arr_tac = NULL;
            Symbol* arr_symbol = NULL;

            print_array_linked_list(head);


            while (head!=NULL)
            {
                arr_symbol = head->symbol;
                arr_tac = (TAC*)malloc(sizeof(TAC));
                arr_tac->keyword = strdup("data_for_array");

                // Convert the integer size to a string
                size = arr_symbol->size * 4;
                snprintf(size_string, 20, "%zu", size);

                if (strcmp(arr_symbol->type_str, "int_array") == 0 || strcmp(arr_symbol->type_str, "float_array") == 0)
                {
                    // Convert the integer size to a string
                    size = arr_symbol->size * 4;
                    snprintf(size_string, 20, "%zu", size);

                    // Create a TAC for the array
                    arr_tac->result = strdup(arr_symbol->id);
                    arr_tac->arg1 = strdup(size_string);
                    appendTAC(&tacHead, arr_tac);
                }

                else if (strcmp(arr_symbol->type_str, "string_array") == 0)
                {
                    // Convert the integer size to a string
                    size = arr_symbol->size * 20;
                    snprintf(size_string, 20, "%zu", size);


                    // Create a TAC for the array
                    arr_tac->result = strdup(arr_symbol->id);
                    arr_tac->arg1 = strdup(size_string);
                    appendTAC(&tacHead, arr_tac);
                }

                else
                {   
                   size = get_struct_size(get_symbol_table(outer_table_semantic, arr_symbol->type_str));
                   snprintf(size_string, 20, "%zu", size);

                    // Create a TAC for the array
                    arr_tac->result = strdup(arr_symbol->id);
                    arr_tac->arg1 = strdup(size_string);
                    appendTAC(&tacHead, arr_tac);
                
                }

                head = head->next;
            }



            // Create tac for .text section
            TAC* TextTac = (TAC*)malloc(sizeof(TAC));
            TextTac->keyword = strdup("text");
            appendTAC(&tacHead, TextTac);

            // Create tac for globl
            TAC* GlobalTac = (TAC*)malloc(sizeof(TAC));
            GlobalTac->keyword = strdup("globl");
            appendTAC(&tacHead, GlobalTac);

            printf("Traversing StructDeclList\n");
            semanticAnalysis(node->program.StructDeclList, outer_table_semantic);
            printf("Traversing FuncDeclList\n");
            semanticAnalysis(node->program.FuncDeclList, outer_table_semantic);
            printf("Traversing VarDeclList in program\n");
            // Add main if first variable declaration
            
            TAC* newTac2 = (TAC*)malloc(sizeof(TAC));
            newTac2->keyword = strdup("main");
            appendTAC(&tacHead, newTac2);
            main_declared = 1;
            
            semanticAnalysis(node->program.VarDeclList, outer_table_semantic);
            printf("Traversing StmntList in program\n");
            semanticAnalysis(node->program.StmntList, outer_table_semantic);
            
            break;
        }
        case NodeType_VarDeclList:
            semanticAnalysis(node->VarDeclList.VarDecl, outer_table_semantic);
            semanticAnalysis(node->VarDeclList.VarDeclList, outer_table_semantic);
            break;

     


        // New cases for remaining NodeTypes
        case NodeType_SimpleFloat:
            if (function_called == 1)
            {
                generate_param_pass_tac(node, symbol_Table);
                curr_param = curr_param->next;
            }

            if(array_assigned == 1)
            {
                generate_array_assign_tac(node, symbol_Table);
                array_counter += 1;
            }

            // Handle float constant or variable logic here
            break;

        case NodeType_SimpleString:
            if (function_called == 1)
            {
                generate_param_pass_tac(node, symbol_Table);
                curr_param = curr_param->next;
            }

            if (array_assigned == 1)
            {
                generate_array_assign_tac(node, symbol_Table);
                array_counter += 1;
            }


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


            semanticAnalysis(node->FuncDeclList.FuncDecl, outer_table_semantic);
            semanticAnalysis(node->FuncDeclList.FuncDeclList, outer_table_semantic);
            break;

        case NodeType_FuncDecl:
            printf("Traversing FuncDecl\n");
            curr_scope = strdup(node->FuncDecl.id);

            TAC* funcTac = (TAC*)malloc(sizeof(TAC));
            funcTac->keyword = strdup(curr_scope);
            appendTAC(&tacHead, funcTac);

            semanticAnalysis(node->FuncDecl.ParamList, outer_table_semantic);
            semanticAnalysis(node->FuncDecl.VarDeclList, outer_table_semantic);
            semanticAnalysis(node->FuncDecl.StmntList, outer_table_semantic);
            semanticAnalysis(node->FuncDecl.ReturnStmnt, outer_table_semantic);

            // Create TAC node
            funcTac = (TAC*)malloc(sizeof(TAC));
            funcTac->keyword = strdup("jump");
            funcTac->arg1 = strdup("end_func");
            appendTAC(&tacHead, funcTac);

            break;

        case NodeType_ParamList:
            // Traverse parameter list


            semanticAnalysis(node->ParamList.Param, outer_table_semantic);
            semanticAnalysis(node->ParamList.ParamList, outer_table_semantic);
            break;

        case NodeType_Param: {
            Symbol* curr_symbol = getSymbol(symbol_Table, node->Param.id);
            if (symbol != NULL) 
            {
                char* paramVar = createParameterVar();
                updateRegister(symbol_Table, node->Param.id, paramVar);
                printf("tempVar for %s is %s\n", node->Param.id, curr_symbol->tempVar);
            }
            else
            {
                printf("ERROR: cant find symbol for %s\n", node->Param.id);
            }


            // Analyze individual parameters
            break;

        case NodeType_ReturnStmnt:

            if (strcmp(get_scope_type(outer_table_semantic, curr_scope), "void") != 0)
            {

                fprintf(stdout, "Function wih no return declared\n");
                Symbol* symbol = getSymbol(symbol_Table, node->ReturnStmnt.id);
                if (symbol == NULL)
                {
                    printf("ERROR: variable %s is not declared\n", node->ReturnStmnt.id);
                    exit(1);
                }

                else if(symbol->tempVar == NULL)
                {
                    printf("ERROR: variable %s is not initialized\n", node->ReturnStmnt.id);
                    exit(1);
                }

                else if(strcmp(get_scope_type(outer_table_semantic, curr_scope), getSymbol(symbol_Table, node->ReturnStmnt.id)->type_str) == 0)
                {
                    printf("currentID is %s\n", currentID);
                    printf("current scope: %s", curr_scope);
                    TAC* return_tac = (TAC*)malloc(sizeof(TAC));
                    return_tac->result = strdup("v0");
                    return_tac->arg1 = strdup(getTempVar(getSymbol(symbol_Table, currentID)));
                    appendTAC(&tacHead, return_tac);
                }

                else
                {
                    printf("ERROR: Function %s returns type %s\n", node->FunctionCall.id, get_scope_type(outer_table_semantic, curr_scope));
                    exit(1);
                }
            }

            paramCounter = 0;
            curr_scope = strdup("global");


            // Analyze return statement and check type consistency
            break;

        case NodeType_IndexAssignment:
            // Array index assignment logic
            currentID  = node->IndexAssignment.id;
            printf("indexAssignment index is: %d and id is %s \n", node->IndexAssignment.index, currentID);
            //redeclare symbol as symbolIndexAssignment to avoid redefinition error, which should not be happening
            Symbol* symbolIndexAssignment = getSymbol(symbol_Table,currentID);
            printf("symbol id is: %s \n", symbolIndexAssignment->id);
            tempIndex=node->IndexAssignment.index;
            printf("tempIndex is %d\n", tempIndex);
            updateTempIndex(symbolIndexAssignment,tempIndex);
            printf("went into IndexAssignment case\n");
            // Perform array assignment semantic checks
            check_array_declaration(symbol_Table, node->VarDecl.type, node->IndexAssignment.id, node->VarDecl.size, lines);
                // Perform array semantic checks
               // check_array_declaration(symbol_Table, node->VarDecl.type, node->VarDecl.id, node->VarDecl.size, lines);
            semanticAnalysis(node->IndexAssignment.Expr, outer_table_semantic);
            //apply_type_coercion(symbol_Table,currentID,node, lines);
            int length = snprintf(NULL, 0, "%s[%d]", symbolIndexAssignment->arrayDeclVar, symbolIndexAssignment->tempIndex);
            char* tempTacResult = (char*)malloc(length + 1);  // +1 for null terminator
            snprintf(tempTacResult, length + 1, "%s[%d]", symbolIndexAssignment->arrayDeclVar, symbolIndexAssignment->tempIndex);
            printf("end of array assignment with tempTacResult : %s\n", tempTacResult);
            // Will need later !!!! apply_type_coercion(symbol_Table, node->IndexAssignment.id, node->IndexAssignment.Expr, lines);  
            TAC* arrTac = (TAC*)malloc(sizeof(TAC));           
            arrTac->result = strdup(tempTacResult);            
            arrTac->op =strdup(node->IndexAssignment.op);            
            arrTac->arg1 =  symbolIndexAssignment->tempVar;          
            appendTAC(&tacHead,arrTac);
            break;
        
        case NodeType_ValueList:
            // Traverse list of values (array or other collection)


            semanticAnalysis(node->ValueList.Val, outer_table_semantic);
            semanticAnalysis(node->ValueList.ValueList, outer_table_semantic);
            break;

        case NodeType_ArrAssignment:
            
            array_assigned = 1;
            curr_array = node->ArrAssignment.id;
            semanticAnalysis(node->ArrAssignment.ValueList, outer_table_semantic);

            if(getSymbol(symbol_Table, curr_array)-> size > array_counter)
            {
                printf("ERROR: Too few arguments for array %s\n", curr_array);
                exit(1);
            }
            
            array_assigned = 0;
            curr_array = NULL;
            array_counter = 0;
            // Array assignment logic
            break;

        case NodeType_StructDeclList:
            // Struct declaration list traversal

            semanticAnalysis(node->StructDeclList.StructDecl, outer_table_semantic);
            semanticAnalysis(node->StructDeclList.StructDeclList, outer_table_semantic);
            break;

        case NodeType_StructDecl:


            break;

        case NodeType_StructMemberAssignment:{
            // Struct member assignment logic
            structMemberAssignment = 1;
            printf("went into Nodetype structmemberassignment \n");
            structID = node->StructMemberAssignment.id;
            
            structMemberID =node->StructMemberAssignment.member_id;
            printf("2\n");

            structIDSymbol = getSymbol(symbol_Table,structID);
            printf(" StructUDSymbol ID is: %s \n", structIDSymbol->id);
            structMemberIDSymbol = getSymbol(structIDSymbol->value.structValue,structMemberID);
            printf("structmemberidsymbol ID is:  %s\n ", structMemberIDSymbol->id);
            
            semanticAnalysis(node->StructMemberAssignment.Expr, outer_table_semantic);
            printf("3\n");
            structMemberAssignment = 0;
            TAC* structTAC = (TAC*)malloc(sizeof(TAC));  
            structTAC->result = strdup(structMemberIDSymbol->tempVar);
            structTAC->op=strdup("=");
            structTAC->arg1 = structMemberIDSymbol->tempVarStruct;
            appendTAC(&tacHead,structTAC);
            break;
        }
        case NodeType_FunctionCall:
       
            printf("Function call: %s\n", node->FunctionCall.id);
            curr_param = create_parameter_linked_list(get_symbol_table(outer_table_semantic, node->FunctionCall.id));
            print_linked_list(curr_param);

            function_called = 1;
            semanticAnalysis(node->FunctionCall.valueList, outer_table_semantic);
            if(curr_param != NULL)
            {
                printf("ERROR: function call to %s has too few parameters\n", node->FunctionCall.id);
                exit(1);
            }
            function_called = 0;

            TAC* call_tac = (TAC*)malloc(sizeof(TAC));
            call_tac->keyword = strdup("jump");
            call_tac->arg1 = strdup(node->FunctionCall.id);
            appendTAC(&tacHead, call_tac);
            

            
            if (strcmp(get_scope_type(outer_table_semantic, node->FunctionCall.id), "void") != 0)
            {
                if (stmnt_started == 0)
                {
                    //do nothing
                }
                else if(strcmp(get_scope_type(outer_table_semantic, node->FunctionCall.id), getSymbol(symbol_Table, currentID)->type_str) == 0)
                {
                    printf("currentID is %s\n", currentID);
                    call_tac = (TAC*)malloc(sizeof(TAC));
                    call_tac->arg1 = strdup("v0");
                    call_tac->result = strdup(getTempVar(getSymbol(symbol_Table, currentID)));
                    appendTAC(&tacHead, call_tac);
                }

                else
                {
                    printf("ERROR: Function %s returns type %s\n", node->FunctionCall.id, get_scope_type(outer_table_semantic, node->FunctionCall.id));
                    exit(1);
                }
            }

            else if (strcmp(get_scope_type(outer_table_semantic, node->FunctionCall.id), "void") == 0)
            {   
                if(stmnt_started == 1)
                {
                    printf("ERROR: Function %s returns void\n", node->FunctionCall.id);
                    exit(1);
                }
            }
            stmnt_started = 0;
            break;
        }
        case NodeType_TypeCast:
            // Type casting logic

            break;
        case NodeType_IfStmnt:
           printf("went into ifStmnt\n");
            semanticAnalysis(node->IfStmnt.ConditionList ,outer_table_semantic);
            semanticAnalysis(node->IfStmnt.StmntList,outer_table_semantic); // evaluate the content inside the block
            printf("end of ifStmnt\n");
            break;
        
        case NodeType_ElseIfList:
            
            break;

        case NodeType_ElseIfStmnt:
        // If-else logic
            break;

        case NodeType_ElseStmnt:

            break;

        case NodeType_ConditionList:
            if(node->ConditionList.BoolOperator != NULL){
            tempBoolOp = strdup(node->ConditionList.BoolOperator);
            
            } else {
                tempBoolOp = NULL;
            }
            labelTrue =  createTempLabel();
            labelFalse = createTempLabel();
            semanticAnalysis(node->ConditionList.Condition, outer_table_semantic);
            semanticAnalysis(node->ConditionList.ConditionList, outer_table_semantic);
            break;

        case NodeType_Condition:
            printf("went into Condition->RightVal\n");
            semanticAnalysis(node->Condition.RightVal ,outer_table_semantic);
            printf("went into Condition->LeftVal\n");
            semanticAnalysis(node->Condition.LeftVal ,outer_table_semantic);


    ////////
            fprintf(stdout,"went into condition\n");
            

            break;

        case NodeType_SwitchStmnt:

            break;
        case NodeType_CaseList:

            break;

        case NodeType_CaseStmnt:
        
            break;
        
        case NodeType_DefaultCase:

            break;
        case NodeType_WhileLoop:

            break;
        default:
            break;
    }
    printf("==========      current NodeType is %d=    and value is: %d ========\n", node->type, node->SimpleExpr.value);
    if (node->type == NodeType_Expr ) {
        //apply_type_coercion(symbol_Table,currentID,node, lines);
        TAC* tac = generateTACForExpr(node,outer_table_semantic);

        // Process or store the generated TAC
        //printTAC(tac);
        appendTAC(&tacHead,tac);
    }
    if ( node->type == NodeType_SimpleExpr && structMemberAssignment ==1){
       TAC* tac = generateTACForExpr(node,outer_table_semantic);

        // Process or store the generated TAC
        //printTAC(tac);
        appendTAC(&tacHead,tac);
    } else if (node->type == NodeType_SimpleFloat && structMemberAssignment ==1){
        TAC* tac = generateTACForExpr(node,outer_table_semantic);

        // Process or store the generated TAC
        //printTAC(tac);
        appendTAC(&tacHead,tac);
    } else if (node->type == NodeType_SimpleString && structMemberAssignment ==1){
        TAC* tac = generateTACForExpr(node,outer_table_semantic);

        // Process or store the generated TAC
        //printTAC(tac);
        appendTAC(&tacHead,tac);
    }
    if(node->Expr.right ==NULL){
        printf("well well well\n");
    }
    if (node->type == NodeType_Condition){
        generateTACForNode(node, &tacHead, labelFalse, labelTrue);
    }
}


TAC* generateTACForExpr(ASTNode* expr, OuterSymbolTable* outer_table) {
    
    SymbolTable* symbol_Table = get_symbol_table(outer_table, curr_scope);
    TAC* newTac = (TAC*)malloc(sizeof(TAC));
    char* tempResult;
    char* temp; // to store the result of expression before next * or /
   
    
   
   
   
    if (structMemberAssignment !=0 ){
            printf("strucmemberAssignment is; %d, memberId: %s, structID: %s\n", structMemberAssignment,structMemberIDSymbol->id, structID);
            printf("well well well\n");
            //SymbolTable* symbol_Table = get_symbol_table(outer_table, curr_scope);
            currentID = structMemberID;
            printf("CurrentID is %s\n", currentID);

            Symbol* symbol = structMemberIDSymbol;
            printf("Hello\n");
            
            if (expr == NULL) {
                fprintf(stdout,"Error: Expression is NULL.\n");
                return NULL;
            }
            ///char* tempResult;
            //char* temp; // to store the result of expression before next * or /
            
            if (!newTac) return NULL;
            
            switch (expr->type) {
                case NodeType_Expr: {
                    fprintf(stdout,"Generating TAC for expression in If\n");
                    if(structMemberIDSymbol->tempVarStruct!=NULL){
                    tempResult= strdup(structMemberIDSymbol->tempVarStruct);
                    }
                    if(structMemberIDSymbol->tempVarStruct==NULL){
                    printf("TempVarStruct is NULL\n");
                    }
                    fprintf(stdout, "HERE");

                    if ((strcmp(expr->Expr.op, "*") == 0 || strcmp(expr->Expr.op, "/") == 0) && structMemberIDSymbol->tempVarStruct != NULL ){
                        updatetemp(structIDSymbol->value.structValue, currentID, structMemberIDSymbol->tempVarStruct);
                    }
                    newTac->op = strdup(expr->Expr.op); // e.g., "+", "-", "*", etc.
                    //tempResult = createTempVar();
                    /*
                    if (strcmp(expr->Expr.right->Expr.op, "*") == 0  || strcmp(expr->Expr.right->Expr.op, "/") == 0 )  {
                        updateRegister(symbol_Table, currentID, symbol->tempVar);
                    }
                    */
                    newTac->result = createTempVar();//strdup(tempResult);
                    //
                    
                    // Reuse temp vars for operands (x and y)
                    if (structMemberIDSymbol->tempVarStruct==NULL ){//&& structMemberIDSymbol->tempVarStruct!=tempResult){
                    fprintf(stdout, "111\n");
                    newTac->arg1 = createOperand(expr->Expr.left, structIDSymbol->value.structValue);  // Left operand (e.g., t0 for x)
                    fprintf(stdout, "newTac->arg1 is:  %s\n", newTac->arg1);// DEbug
                    fprintf(stdout, "TempResult is: %s\n", tempResult);// DEbug
                    } else if (structMemberIDSymbol->tempVarStruct!=NULL && expr->Expr.left->type != NodeType_Expr ){
                        fprintf(stdout, "2\n");
                        newTac->arg1 = createOperand(expr->Expr.left, structIDSymbol->value.structValue);  // Left operand (e.g., t0 for x)
                        fprintf(stdout, "newTac->arg1 is:  %s in else if \n", newTac->arg1);// DEbug
                    } else if (structMemberIDSymbol->tempVarStruct!=NULL && expr->Expr.right->type != NodeType_Expr ){
                        fprintf(stdout, "3\n");
                        newTac->arg1 = strdup(tempResult);
                        fprintf(stdout, "expr type is:  %d in else \n", expr->Expr.left->type);// DEbug
                        fprintf(stdout, "newTac->arg1 is:  %s in else \n", newTac->arg1);// DEbug
                    }  else {
                        fprintf(stdout, "4\n");
                        newTac->arg1 = strdup(structMemberIDSymbol->temp);
                    }
                    if (tempResult==NULL){
                        fprintf(stdout, "5\n");
                        if(expr->Expr.right != NULL && expr->Expr.right->type!=NodeType_Expr){
                            fprintf(stdout, "6\n");
                            newTac->arg2 = createOperand(expr->Expr.right, structIDSymbol->value.structValue); // Right operand (e.g., t1 for y)
                            fprintf(stdout, "newTac->arg2 is:  %s in if Expr.right!=NULL \n", newTac->arg2);// DEbug
                        } else {
                            fprintf(stdout, "7\n");
                            newTac->arg2 = strdup(structMemberIDSymbol->tempVarStruct);
                            //newTac->arg2 = createOperand(expr->Expr.left, symbol_Table); 
                            fprintf(stdout, "newTac->arg2 is:  %s in the else \n", newTac->arg2);// DEbug  
                        }

                    } else if (strcmp(newTac->result, tempResult) > 0 && ( strcmp(expr->Expr.op, "*") ==0 ) || strcmp(expr->Expr.op, "/") ==0){
                        fprintf(stdout, "8\n");
                        newTac->arg2 = createOperand(expr->Expr.right, structIDSymbol->value.structValue);
                    } else {
                        fprintf(stdout, "9\n");
                        if (expr->Expr.right->Expr.right ==NULL){
                        newTac->arg2 = createOperand(expr->Expr.right, structIDSymbol->value.structValue);
                        }else{
                        newTac->arg2=tempResult;
                        fprintf(stdout,"tempResult is larger than result\n");
                        }
                    }
                    

                    if( newTac->arg2 ==NULL){
                        fprintf(stdout,"!!!!!!!!  arg2 is null  !!!!!!!\n");
                    }
                
                    // Create a new temp var for the result of the expression
                    
                    
                    fprintf(stdout, "TempResult: %s\n", tempResult); //Debug
                    fprintf(stdout, "newTac->result: %s\n", newTac->result);
                    fprintf(stdout,"Generated in generateTAC\n");
                    newTac->next = NULL;
                    //expr->Expr.check=1;
                
                    fprintf(stdout,"Generated TAC: %s = %s %s %s\n", tempResult //*newTac->result
                    , newTac->arg1, newTac->op, newTac->arg2);
                    //if(expr->Expr.right->type )
                    //if ((strcmp(expr->Expr.op, "*") == 0 || strcmp(expr->Expr.op, "/") == 0) && symbol->tempVar != NULL ){
                    //   updateRegister(symbol_Table, currentID, tempResult);
                    //} else {
                    //  printf("generate in the else\n");
                        updateRegisterStruct(structIDSymbol->value.structValue, currentID, newTac->result);
                // }
           
                    break;
                }
                case NodeType_SimpleExpr: {
                    printf("Kevin\n");
                    newTac->result= createTempVar();
                    newTac->arg1 = createOperand(expr,structIDSymbol->value.structValue);
                    updateRegisterStruct(structIDSymbol->value.structValue, currentID, newTac->result);
                    break;
                
                }
                case NodeType_SimpleFloat: {
                    newTac->result= createTempVar();
                    newTac->arg1 = createOperand(expr,structIDSymbol->value.structValue);
                    updateRegisterStruct(structIDSymbol->value.structValue, currentID, newTac->result);
                    break;
                
                }
                
                case NodeType_SimpleString: {
                    newTac->result= createTempVar();
                    newTac->arg1 = createOperand(expr,structIDSymbol->value.structValue);
                    updateRegisterStruct(structIDSymbol->value.structValue, currentID, newTac->result);
                    break;              
                }
                case NodeType_SimpleArrIndex:


                    break;
                default:
                    fprintf(stdout,"Unhandled expression type\n");
                    break;
            }
                // CHECK TO SEE IF THE ASSIGNMENT IS DONE, IF DONE MAP IT TO THE ARRDECLVAR
                /*if((strcmp(symbol->type_str, "int_arr")== 0 ||strcmp(symbol->type_str, "float_arr")== 0 
                || strcmp(symbol->type_str, "string_arr")== 0) && expr->Expr.right == NULL){
                    int length =snprintf(NULL, 0, "%s[%d]", symbol->arrayDeclVar, symbol->tempIndex);
                    char* tempTacResult = malloc(length + 1);  // +1 for null terminator
                    printf("tempTacResult is: %s", tempTacResult);
                    newTac->result = strdup(tempTacResult);

                }
                */

                    
        
    } else {

    SymbolTable* symbol_Table = get_symbol_table(outer_table, curr_scope);
    printf("CurrentID is %s\n", currentID);
    Symbol* symbol = getSymbol(symbol_Table, currentID);
    printf("Hello\n");
    if (expr == NULL) {
        fprintf(stdout,"Error: Expression is NULL.\n");
        return NULL;
    }
    char* tempResult;
    char* temp; // to store the result of expression before next * or /
    //TAC* newTac = (TAC*)malloc(sizeof(TAC));
    if (!newTac) return NULL;
    
    switch (expr->type) {
        case NodeType_Expr: {
            fprintf(stdout,"Generating TAC for expression in Else\n");
            if(symbol->tempVar!=NULL){
            tempResult= strdup(symbol->tempVar);
            }
            fprintf(stdout, "HERE");

            if ((strcmp(expr->Expr.op, "*") == 0 || strcmp(expr->Expr.op, "/") == 0) && symbol->tempVar != NULL ){
                updatetemp(symbol_Table, currentID, symbol->tempVar);
            }
            newTac->op = strdup(expr->Expr.op); // e.g., "+", "-", "*", etc.
            //tempResult = createTempVar();
            /*
            if (strcmp(expr->Expr.right->Expr.op, "*") == 0  || strcmp(expr->Expr.right->Expr.op, "/") == 0 )  {
                updateRegister(symbol_Table, currentID, symbol->tempVar);
            }
            */
            newTac->result = createTempVar();//strdup(tempResult);
            //
            
            // Reuse temp vars for operands (x and y)
            if (symbol->tempVar==NULL && symbol->tempVar!=tempResult){
            newTac->arg1 = createOperand(expr->Expr.left, symbol_Table);  // Left operand (e.g., t0 for x)
            fprintf(stdout, "newTac->arg1 is:  %s\n", newTac->arg1);// DEbug
            fprintf(stdout, "TempResult is: %s\n", tempResult);// DEbug
            } else if (symbol->tempVar!=NULL && expr->Expr.left->type != NodeType_Expr ){
                newTac->arg1 = createOperand(expr->Expr.left, symbol_Table);  // Left operand (e.g., t0 for x)
                fprintf(stdout, "newTac->arg1 is:  %s in else if \n", newTac->arg1);// DEbug
            } else if (symbol->tempVar!=NULL && expr->Expr.right->type != NodeType_Expr ){
                newTac->arg1 = strdup(tempResult);
                fprintf(stdout, "expr type is:  %d in else \n", expr->Expr.left->type);// DEbug
                fprintf(stdout, "newTac->arg1 is:  %s in else \n", newTac->arg1);// DEbug
            }  else {
                newTac->arg1 = strdup(symbol->temp);
            }
            if (tempResult){
            //if(strcmp(newTac->result, tempResult) == 0){
                if(expr->Expr.right != NULL && expr->Expr.right->type!=NodeType_Expr){
                    newTac->arg2 = createOperand(expr->Expr.right, symbol_Table); // Right operand (e.g., t1 for y)
                    fprintf(stdout, "newTac->arg2 is:  %s in if Expr.right!=NULL \n", newTac->arg2);// DEbug
                } else {
                    newTac->arg2 = strdup(symbol->tempVar);
                    //newTac->arg2 = createOperand(expr->Expr.left, symbol_Table); 
                    fprintf(stdout, "newTac->arg2 is:  %s in the else \n", newTac->arg2);// DEbug  
                }

            } else if (strcmp(newTac->result, tempResult) >0){
            newTac->arg2=tempResult;
            fprintf(stdout,"tempResult is larger than result\n");
            }

            if( newTac->arg2 ==NULL){
                fprintf(stdout,"!!!!!!!!  arg2 is null  !!!!!!!\n");
            }
         
            // Create a new temp var for the result of the expression
            
            
            fprintf(stdout, "TempResult: %s\n", tempResult); //Debug
            fprintf(stdout, "newTac->result: %s\n", newTac->result);
            fprintf(stdout,"Generated in generateTAC\n");
            newTac->next = NULL;
            //expr->Expr.check=1;
        
            fprintf(stdout,"Generated TAC: %s = %s %s %s\n", newTac->result //*newTac->result
            , newTac->arg1, newTac->op, newTac->arg2);
            //if(expr->Expr.right->type )
            //if ((strcmp(expr->Expr.op, "*") == 0 || strcmp(expr->Expr.op, "/") == 0) && symbol->tempVar != NULL ){
             //   updateRegister(symbol_Table, currentID, tempResult);
            //} else {
              //  printf("generate in the else\n");
                updateRegister(symbol_Table, currentID, newTac->result);
           // }
            







            break;
        }
       case NodeType_SimpleExpr: {
            printf("Kevin\n");
            newTac->result= createTempVar();
            newTac->arg1 = createOperand(expr,symbol_Table);
            updateRegister(symbol_Table, currentID, newTac->result);
            break;
                
            }
        case NodeType_SimpleFloat: {
            newTac->result= createTempVar();
            newTac->arg1 = createOperand(expr,symbol_Table);
            updateRegister(symbol_Table, currentID, newTac->result);
            break;
                
            }
                
        case NodeType_SimpleString: {
            newTac->result= createTempVar();
            newTac->arg1 = createOperand(expr,symbol_Table);
            updateRegister(symbol_Table, currentID, newTac->result);
            break;              
            }

        default:
            fprintf(stdout,"Unhandled expression type\n");
            break;
    }
        // CHECK TO SEE IF THE ASSIGNMENT IS DONE, IF DONE MAP IT TO THE ARRDECLVAR
        /*if((strcmp(symbol->type_str, "int_arr")== 0 ||strcmp(symbol->type_str, "float_arr")== 0 
        || strcmp(symbol->type_str, "string_arr")== 0) && expr->Expr.right == NULL){
            int length =snprintf(NULL, 0, "%s[%d]", symbol->arrayDeclVar, symbol->tempIndex);
            char* tempTacResult = malloc(length + 1);  // +1 for null terminator
            printf("tempTacResult is: %s", tempTacResult);
            newTac->result = strdup(tempTacResult);

        }
        */
        
        
        
    }    
    return newTac;
}


char* createOperand(ASTNode* node, SymbolTable* symbol_table) {
    char* operand = (char*)malloc(32 * sizeof(char));
    Symbol* symbol = getSymbol(symbol_table, currentID);
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
            
            if (strcmp(symbol->type_str, "float_array")==0 || strcmp(symbol->type_str, "float") ==0) {
            printf("1\n");
            snprintf(operand, 32, "%f", (float)node->SimpleExpr.value);
            } else {
                printf("2\n");
                snprintf(operand, 32, "%d", node->SimpleExpr.value);
            }
            break;
        case NodeType_SimpleFloat: { // New case for SimpleFloat
            if (strcmp(symbol->type_str, "int_array")==0 || strcmp(symbol->type_str, "int") ==0) {
                printf("3\n");
            snprintf(operand, 32, "%d", (int)node->SimpleFloat.value);
            } else {
                printf("4\n");
                snprintf(operand, 32, "%f", node->SimpleFloat.value);
            }
            //snprintf(operand, 32, "%f", node->SimpleFloat.value);
            break;
        }
        case NodeType_SimpleString: {
      
            if (strcmp(symbol->type_str, "string_array")==0 || strcmp(symbol->type_str, "string") ==0){
                snprintf(operand, 32, "%s", node->SimpleString.value);
               
            } else {
                printf("The type string is not string or string_array\n");
            }
            break;

        }
        case NodeType_SimpleStructMember:
        {   
            char* tempStructRegister;
            snprintf(operand, 32, "%s", structMemberIDSymbol->tempVar);

            break;
        }
        case NodeType_SimpleArrIndex:
            
            snprintf(operand, 32, "%s[%d]", symbol->arrayDeclVar,node->SimpleArrIndex.index);
            
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


void generate_param_pass_tac(ASTNode* node, SymbolTable* symbol_table)
{
    if(curr_param == NULL)
    {
        printf("ERROR: Too many arguments for function\n");
        exit(1);
    }
    switch (node->type)
    {
        case NodeType_SimpleID:
        {
            Symbol* symbol = getSymbol(symbol_table, node->SimpleID.id);

            if(strcmp(symbol->type_str, curr_param->symbol->type_str) != 0)
            {
                printf("ERROR: Type mismatch\n");
                exit(1);
            }

            if (curr_param->symbol->tempVar == NULL)
            {
                printf("ERROR: ID %s not initialized\n", curr_param->symbol->id);
                exit(1);
            }
            
            else
            {
                TAC* newTac = (TAC*)malloc(sizeof(TAC));
                newTac->keyword = strdup("parameterPass");
                newTac->result = strdup(curr_param->symbol->tempVar);
                newTac->arg1 = strdup(getTempVar(symbol));
                printf("GeneratedTac: %s = %s\n", newTac->result, newTac->arg1);
                appendTAC(&tacHead, newTac);
            }
            break;
        }

        case NodeType_SimpleExpr:
        {
            if(strcmp("INT", curr_param->symbol->type_str) != 0)
            {
                printf("ERROR: Type mismatch\n");
                exit(1);
            }
            
            else
            {
                char* str = (char*)malloc(10 * sizeof(char));
                snprintf(str, 10, "%d", node->SimpleExpr.value);

                // Create TAC node
                TAC* newTac = (TAC*)malloc(sizeof(TAC));
                newTac->keyword = strdup("parameterPass");
                newTac->result = strdup(curr_param->symbol->tempVar);
                newTac->arg1 = strdup(str);
                appendTAC(&tacHead, newTac);

                printf("GeneratedTac: %s = %s\n", newTac->result, newTac->arg1);
            }

            break;
        }

        case NodeType_SimpleFloat:
        {
            if(strcmp("FLOAT", curr_param->symbol->type_str) != 0)
            {
                printf("ERROR: Type mismatch\n");
                exit(1);
            }
            
            else
            {
                char* str = (char*)malloc(20 * sizeof(char));
                snprintf(str, 20, "%f", node->SimpleFloat.value);

                // Create TAC node
                TAC* newTac = (TAC*)malloc(sizeof(TAC));
                newTac->keyword = strdup("parameterPass");
                newTac->result = strdup(curr_param->symbol->tempVar);
                newTac->arg1 = strdup(str);
                appendTAC(&tacHead, newTac);

                printf("GeneratedTac: %s = %s\n", newTac->result, newTac->arg1);
            }
            break;
        }

        case NodeType_SimpleString:
        {
            if(strcmp("STRING", curr_param->symbol->type_str) != 0)
            {
                printf("ERROR: Type mismatch\n");
                exit(1);
            }
            
            else
            {
                // Create TAC node
                TAC* newTac = (TAC*)malloc(sizeof(TAC));
                newTac->keyword = strdup("parameterPass");
                newTac->result = strdup(curr_param->symbol->tempVar);
                newTac->arg1 = strdup(node->SimpleString.value);
                appendTAC(&tacHead, newTac);

                printf("GeneratedTac: %s = %s\n", newTac->result, newTac->arg1);
            }
            break;
        }
        
        default:
            break;
    }
}
void generate_array_assign_tac(ASTNode* node, SymbolTable* symbol_table)
{
    // Get symbol for current array
    Symbol* symbol = getSymbol(symbol_table, curr_array);

    fprintf(stdout, "array size: %d, array_counter: %d\n", symbol->size, array_counter);
    if (array_counter >= symbol->size)
    {
        printf("ERROR: Too many arguments for array %s\n", curr_array);
        exit(1);
    }

    switch (node->type)
    {
        case NodeType_SimpleExpr:
        {
            if (strstr(symbol->type_str, "int") == NULL)
            {
                fprintf(stdout, "ERROR: array %s is of type %s\n", curr_array, symbol->type_str);
                exit(1);
            }

            else
            {
                printf("12345\n");
                char result[10];
                char* str = (char*)malloc(10 * sizeof(char));
                snprintf(str, 10, "%d", node->SimpleExpr.value);
                sprintf(result, "%s[%d]", symbol->arrayDeclVar, array_counter); 
                
                TAC* newTac = (TAC*)malloc(sizeof(TAC));
                newTac->keyword = strdup("array_assign");
                newTac->result = strdup(result);
                newTac->arg1 = strdup(str);
                appendTAC(&tacHead, newTac);
            }
            break;
        }

        case NodeType_SimpleID:
        {
            Symbol* passed_symbol = getSymbol(symbol_table, node->SimpleID.id);
            if (strstr(symbol->type_str, passed_symbol->type_str) == NULL)
            {
                fprintf(stdout, "ERROR: array %s is of type %s\n", curr_array, symbol->type_str);
                exit(1);
            }

            else if (passed_symbol->tempVar == NULL)
            {
                fprintf(stdout, "ERROR: ID %s not initialized\n", passed_symbol->id);
                exit(1);
            }   

            else
            {
                char result[10];
                snprintf(result, 10, "%s[%d]", symbol->arrayDeclVar, array_counter);

                TAC* newTac = (TAC*)malloc(sizeof(TAC));
                newTac->keyword = strdup("array_assign");
                newTac->result = strdup(result);
                newTac->arg1 = strdup(passed_symbol->tempVar);
                appendTAC(&tacHead, newTac);
            }

            break;
        }

        case NodeType_SimpleFloat:
        {
            if (strstr(symbol->type_str, "float") == NULL)
            {
                fprintf(stdout, "ERROR: array %s is of type %s\n", curr_array, symbol->type_str);
                exit(1);
            }

            else 
            {
                char result[10];
                char* str = (char*)malloc(20 * sizeof(char));
                snprintf(str, 20, "%f", node->SimpleFloat.value);
                sprintf(result, "%s[%d]", symbol->arrayDeclVar, array_counter); 
                
                TAC* newTac = (TAC*)malloc(sizeof(TAC));
                newTac->keyword = strdup("array_assign");
                newTac->result = strdup(result);
                newTac->arg1 = strdup(str);
                appendTAC(&tacHead, newTac);
            }
            break;

        }

        case NodeType_SimpleString:
        {
            if (strstr(symbol->type_str, "string") == NULL)
                {
                    fprintf(stdout, "ERROR: array %s is of type %s\n", curr_array, symbol->type_str);
                    exit(1);
                }

            else
            {
                char result[10];
                sprintf(result, "%s[%d]", symbol->tempVar, array_counter); 
                
                TAC* newTac = (TAC*)malloc(sizeof(TAC));
                newTac->keyword = strdup("array_assign");
                newTac->result = strdup(result);
                newTac->arg1 = strdup(node->SimpleString.value);
                appendTAC(&tacHead, newTac);
            }
        }

        break;

    }
}

// Create a temporary variable
char* createTempVar() {
    static int tempCounter = 0;
    char* tempVar = (char*)malloc(10 * sizeof(char));
    snprintf(tempVar, 10, "t%d", tempCounter++);
    fprintf(stdout,"Temp var created: %s\n", tempVar);
    return tempVar;

}




char* createTempLabel(){
    static int tempLabelCounter = 0;
    char* tempLabel = (char*)malloc(10 * sizeof(char));
    snprintf(tempLabel, 10, "L%d", tempLabelCounter++);
    fprintf(stdout,"Temp label created: %s\n", tempLabel);
    return tempLabel;
}


//function to generate TAC for If condition
void generateTACForNode(ASTNode* node, TAC** tacList, char* labelFalse, char* labelTrue) {
    if (node == NULL) return;
    printf("Went into generateTACForNode\n");
    switch (node->type) {
        case NodeType_IfStmnt: {
            printf("asdasdasd\n");
            char* labelEnd = createTempVar(); // Label for end of if statement
            generateTACForNode(node->ConditionList.Condition, tacList, labelTrue, labelFalse);

            // Label for true block
            appendTAC(tacList, createTACSemantic("label", labelTrue, NULL, NULL));
            // Generate TAC for the body of the if statement
            appendTAC(tacList, createTACSemantic("call", "test", NULL, NULL)); // Example: test()
            // Jump to end
            appendTAC(tacList, createTACSemantic("goto", NULL, NULL, labelEnd));

            // Label for false block
            appendTAC(tacList, createTACSemantic("label", labelFalse, NULL, NULL));
            appendTAC(tacList, createTACSemantic("label", labelEnd, NULL, NULL));
            break;
        }
        case NodeType_ConditionList: {
            char* tempLabel = createTempVar(); // For intermediate labels
            printf("went into generateTACFornode conditionlist case\n");
            if (strcmp(node->ConditionList.BoolOperator, "AND") == 0) {
                // Short-circuit for AND: Left then right
                generateTACForNode(node->Condition.LeftVal, tacList, tempLabel, labelFalse);
                appendTAC(tacList, createTACSemantic("label", tempLabel, NULL, NULL));
                generateTACForNode(node->Condition.RightVal, tacList, labelTrue, labelFalse);
            } else if (strcmp(node->ConditionList.BoolOperator, "OR") == 0) {
                // Short-circuit for OR: Left then right
                generateTACForNode(node->Condition.LeftVal, tacList, labelTrue, tempLabel);
                appendTAC(tacList, createTACSemantic("label", tempLabel, NULL, NULL));
                generateTACForNode(node->Condition.RightVal, tacList, labelTrue, labelFalse);
            }
            break;
        }
        case NodeType_Condition: {
            printf("Went into condition in GenerateTacForNode\n");
            //SymbolTable* symbol_Table = get_symbol_table(outer_table_semantic, curr_scope);
            printf("symbol table no problem\n");
            Symbol* symbol = getSymbol(symbol_Table, node->Condition.LeftVal->SimpleID.id);
            printf("symbol id is: %s\n", node->Condition.LeftVal->SimpleID.id);
            printf("symbol no problem\n");
            // Generate TAC for comparison (e.g., <, >, ==)
            char* temp1 = createTempVar();
            char* temp2 = createTempVar();
            printf("LeftVal is: %s,   RightVal is: %d \n", node->Condition.LeftVal->SimpleID,node->Condition.RightVal->SimpleExpr );
            //check if the Variable has tempVar in symbolTable
            if (symbol->tempVar != NULL){
                appendTAC(tacList, createTACSemantic("load", symbol->tempVar, NULL, temp1));       
            } else {
                appendTAC(tacList, createTACSemantic("load", node->Condition.LeftVal->SimpleID.id, NULL, temp1));
            }
            char* buffer = (char*)malloc(32 * sizeof(char));
            switch (node->Condition.RightVal->type){
                    case NodeType_SimpleExpr:{
                        snprintf(buffer, 32, "%d", node->Condition.RightVal->SimpleExpr.value);                      
                        break;
                    }
                    case NodeType_SimpleFloat: { // New case for SimpleFloat
                        snprintf(buffer, 32, "%f", node->Condition.RightVal->SimpleFloat.value);
                        break;
                    }
                    case NodeType_SimpleString: {                
                       snprintf(buffer, 32, "%s", node->Condition.RightVal->SimpleString.value);
                        break;
                    }

            }
            printf("Buffer is: %s\n", buffer);
            
            appendTAC(tacList, createTACSemantic("load",buffer, NULL, temp2));
            //check to see if the condition equal by subtracting
            char* cmpHolder = createTempVar(); // placeholder for comparison. eg. t2 = t1 -t0 
            appendTAC(tacList, createTACSemantic("-",temp1,temp2, cmpHolder) );
            

            /*
            if (strcmp(node->Condition.ComparisonOperator, ">=") == 0 || strcmp(node->Condition.ComparisonOperator, "<=") == 0 ){
                appendTAC(tacList, createTACSemantic("if", temp1, temp2, labelTrue));   
            }
            */
            
    /////////////////////////////        
            
            createTACForLabel(tacList, node,labelFalse,labelTrue,cmpHolder );
            
    /////////////////////////////////////////////////    
            
            
            
            
            
            
          
            
            
            
            
            
            
            
            
    printf("1\n");      
            
            
            
            
            
            appendTAC(tacList, createTACSemantic("if", temp1, temp2, labelTrue));
            printf("2\n"); 
            appendTAC(tacList, createTACSemantic("goto", NULL, NULL, labelFalse));
            printf("3\n"); 
            break;
        }
        default:
            fprintf(stderr, "Node type not handled in TAC generation: %d\n", node->type);
            break;
    }
}


//Function to create TAC for If condition
TAC* createTACSemantic(char* op, char* arg1, char* arg2, char* result) {
    TAC* node = (TAC*)malloc(sizeof(TAC));
    node->op = strdup(op);
    node->arg1 = arg1 ? strdup(arg1) : NULL;
    node->arg2 = arg2 ? strdup(arg2) : NULL;
    node->result = result ? strdup(result) : NULL;
    if (opChecker == 1 ){
        node->keyword = strdup("label"); // for label 
        printf("keyword set to label\n");
        opChecker = 0;
    } else {
    //node->keyword = strdup("cond"); // for condition
    }
    node->next = NULL;
    return node;
}
void createTACForLabel(TAC** tacList, ASTNode* node, char* labelFalse, char* labelTrue, char* cmpHolder){
    // Compare the operator string
    if (strcmp(node->Condition.ComparisonOperator, "==") == 0) {
        // x == y
       
    } else if (strcmp(node->Condition.ComparisonOperator, "!=") == 0) {
        // x != y
        
    } else if (strcmp(node->Condition.ComparisonOperator, ">") == 0) {
        // x > y
        
    } else if (strcmp(node->Condition.ComparisonOperator, "<") == 0) {
        // x < y
        
    } else if (strcmp(node->Condition.ComparisonOperator, "<=") == 0) {
        // x <= y
        
    } else if (strcmp(node->Condition.ComparisonOperator, ">=") == 0) {
        // x >= y
        printf(">= is okay\n");
        //appendTAC(tacList, createTACSemantic("slt", createTempVar(), temp1, "slt")); // temp3 = t0 < t1
        printf("6 labelFalse is: %s cmpHolder is: %s\n", labelFalse, cmpHolder); 
        opChecker =1 ;
        appendTAC(tacList, createTACSemantic("<", "0", labelFalse, cmpHolder));
        printf("5\n"); 
    } else {
        // Unrecognized operator
        fprintf(stderr, "Unknown operator: %s\n", node->Condition.ComparisonOperator);
    }
            
            
}