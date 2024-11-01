#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "Symbol_Table.h"
//#include "parser.y"
char* global_scope = "global";
char* curr_scope = "global";


int paramCounter = 0; // Variable to keep track registers for parameters

int tempVars[20];
int paramVars[3];
int main_declared = 0; // Variable to keep track if the main function has been declared
char* currentID;
char* writeID;
TAC* tacHead = NULL;  // Global head of the TAC instructions list
extern int declaredSymbol;
extern int lines;

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
}



void def_outer_table_semantic(OuterSymbolTable* outer_table_semantic){
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
    if (strcmp(type, "int") != 0 && strcmp(type, "float") != 0 && strcmp(type, "string") != 0) {
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
    check_array_not_redeclared(table, id, line);
    check_array_type(type, line);
    check_array_size(size, line);
}


TAC* generate_arrDecl_tac(ASTNode* node) {
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
    SymbolTable* symbol_Table = get_symbol_table(outer_table_semantic, curr_scope);
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
                symbol->tempVar = strdup(arrDecl_tac->result);

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
            stmnt_started = 1;
            currentID  = node->Stmnt.id;
            if (currentID == NULL) {
            fprintf(stderr, "Error: currentID is NULL\n");
            return;
            }
            fprintf(stdout,"Semantic analysis for statement, curr ID is %s\n", currentID);
          
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

            // Check for type consistency and initialization
            check_type_consistency(symbol_Table, node->Stmnt.id, node->Stmnt.Expr, lines);
          
            if (symbol != NULL && node->Stmnt.Expr->type == NodeType_SimpleExpr) {
                TAC* newTac = (TAC*)malloc(sizeof(TAC));
            // Assuming the expression has an integer literal, update the variable's value

            updateValueInt(symbol_Table, node->Stmnt.id, node->Stmnt.Expr->SimpleExpr.value);
            fprintf(stdout,"updateValue has no problem\n");
           
            fprintf(stdout,"Generating TAC for simple expression\n");
            char buffer[20];
            //char* tempVar= getTempVar(symbol_table,expr->Stmnt.id);
            snprintf(buffer, 20, "%d", node->Stmnt.Expr->SimpleExpr.value);
            newTac->arg1 = strdup(buffer);  // Literal value
            newTac->op = "=";  // Assignment operator
            newTac->arg2 = NULL;
            newTac->result = getTempVar(symbol);
            appendTAC(&tacHead,newTac);

            //newTac->result=createTempVar();
            fprintf(stdout,"Generated in generateTAC in simpleExpr\n");
            }
            break;
        case NodeType_Expr:
            // Recursively analyze left and right expressions
            
            fprintf(stdout," check = %d\n", node->Expr.check);
            fprintf(stdout,"----------------left-------------\n");
            semanticAnalysis(node->Expr.left, outer_table_semantic);
            fprintf(stdout,"----------------right-------------\n");
            semanticAnalysis(node->Expr.right, outer_table_semantic);
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

        case NodeType_VarDeclList:
            semanticAnalysis(node->VarDeclList.VarDecl, outer_table_semantic);
            semanticAnalysis(node->VarDeclList.VarDeclList, outer_table_semantic);
            break;

        case NodeType_StmntList:
            stmnt_started = 0;
            semanticAnalysis(node->StmntList.Stmnt, outer_table_semantic);
            semanticAnalysis(node->StmntList.StmntList, outer_table_semantic);
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

        case NodeType_Param:
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

        case NodeType_StructMemberAssignment:
            // Struct member assignment logic
            break;

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

        case NodeType_TypeCast:
            // Type casting logic
            break;

        default:
            break;
    }
    if (node->type == NodeType_Expr ) {
        TAC* tac = generateTACForExpr(node,outer_table_semantic);
        // Process or store the generated TAC
        //printTAC(tac);
        appendTAC(&tacHead,tac);
    }
    if ( node->type == NodeType_SimpleExpr){
         
    }

}

TAC* generateTACForExpr(ASTNode* expr, OuterSymbolTable* outer_table_semantic) {
    SymbolTable* symbol_Table = get_symbol_table(outer_table_semantic, curr_scope);
    Symbol* symbol = getSymbol(symbol_Table, currentID);
    char* tempVar = getTempVar(symbol);
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
                char result[10];
                char* str = (char*)malloc(10 * sizeof(char));
                snprintf(str, 10, "%d", node->SimpleExpr.value);
                sprintf(result, "%s[%d]", symbol->tempVar, array_counter); 
                
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
                snprintf(result, 10, "%s[%d]", symbol->tempVar, array_counter);

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
                sprintf(result, "%s[%d]", symbol->tempVar, array_counter); 
                
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


// Create a temporary variable
char* createTempVar() {
    static int tempCounter = 0;
    char* tempVar = (char*)malloc(10 * sizeof(char));
    snprintf(tempVar, 10, "t%d", tempCounter++);
    fprintf(stdout,"Temp var created: %s\n", tempVar);
    return tempVar;
}

