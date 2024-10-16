// symbol_table.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Symbol_Table.h"
//#include "semantic.h"


// Function to create a symbol table for variables in a scope
SymbolTable* create_symbol_table(int size) {
    SymbolTable* new_table = malloc(sizeof(SymbolTable));
    new_table->table = malloc(sizeof(SymbolNode*) * size);
    new_table->size = size;
    
    // Initialize all linked list heads to NULL
    for (int i = 0; i < size; i++) {
        new_table->table[i] = NULL;
    }
    return new_table;
}


// Function to create the outer symbol table for scopes
OuterSymbolTable* create_outer_table(int size) {
    OuterSymbolTable* new_table = malloc(sizeof(OuterSymbolTable));
    new_table->table = malloc(sizeof(ScopeNode*) * size);
    new_table->size = size;
    
    // Initialize all linked list heads to NULL
    for (int i = 0; i < size; i++) {
        new_table->table[i] = NULL;
    }
    return new_table;
}


// Hash function
unsigned int hash(const char* key, int table_size) {
    unsigned int hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash % table_size;  // Return an index within the table size
}


// Function to insert a new scope into the outer symbol table
void insert_scope(OuterSymbolTable* outerTable, char* scopeName, int innerTableSize, char* return_type) {

    // Create a new scope node
    int scopeIndex = hash(scopeName, outerTable->size) % outerTable->size;
    ScopeNode* newScopeNode = malloc(sizeof(ScopeNode));
    newScopeNode->key = strdup(scopeName);
    newScopeNode->return_type = strdup(return_type);

    // Create the symbol table for the new scope
    newScopeNode->scopeTable = create_symbol_table(innerTableSize);  
    newScopeNode->next = outerTable->table[scopeIndex];  
    outerTable->table[scopeIndex] = newScopeNode;
}


// Function to create and insert a new Symbol of type int into the symbol table
void insert_int_symbol(SymbolTable* table, char* varName, int value) {

    // Find the correct index in the hash table
    int index = hash(varName, table->size) % table->size;

    // Check for duplicate symbols in the same scope
    if (lookup_symbol(table, varName) == 1) {
        printf("Error: Symbol %s already exists in this scope.\n", varName);
        return; 
    }

    // Create a new symbol node
    SymbolNode* newNode = malloc(sizeof(SymbolNode));
    newNode->key = strdup(varName);  

    // Create a new symbol with variable info
    Symbol* symbol = malloc(sizeof(Symbol));
    symbol->type_str = strdup("INT"); 
    symbol->value.intValue = value;

    // Add the new symbol node to the front of the linked list at this index
    newNode->var = symbol;
    newNode->next = table->table[index];  
    table->table[index] = newNode;
    symbol->size = 0;

}


// Function to create and insert a new Symbol of type float into the symbol table
void insert_float_symbol(SymbolTable* table, char* varName, float value) {

    // Find the correct index in the hash table
    int index = hash(varName, table->size) % table->size;

    // Check for duplicate symbols in the same scope
    if (lookup_symbol(table, varName) == 1) {
        printf("Error: Symbol %s already exists in this scope.\n", varName);
        return; 
    }

    // Create a new symbol node
    SymbolNode* newNode = malloc(sizeof(SymbolNode));
    newNode->key = strdup(varName);

    // Create a new symbol with variable info
    Symbol* symbol = malloc(sizeof(Symbol));
    symbol->type_str = strdup("FLOAT"); 
    symbol->value.floatValue = value;
    symbol->size = 0;

    // Add the new symbol node to the front of the linked list at this index
    newNode->var = symbol;
    newNode->next = table->table[index];  
    table->table[index] = newNode; 
}

// Function to create and insert a new Symbol of type string into the symbol table
void insert_string_symbol(SymbolTable* table, char* varName, const char* value) {

    // Find the correct index in the hash table
    int index = hash(varName, table->size) % table->size;

    // Check for duplicate symbols in the same scope
    if (lookup_symbol(table, varName) == 1) {
        printf("Error: Symbol %s already exists in this scope.\n", varName);
        return; 
    }

    // Create a new symbol node
    SymbolNode* newNode = malloc(sizeof(SymbolNode));
    newNode->key = strdup(varName);

    // Create a new symbol with variable info
    Symbol* symbol = malloc(sizeof(Symbol));
    symbol->type_str = strdup("STRING");  
    symbol->value.stringValue = strdup(value);  
    symbol->size = 0;

    // Add the new symbol node to the front of the linked list at this index
    newNode->var = symbol;
    newNode->next = table->table[index];  
    table->table[index] = newNode;

}

void insert_int_arr_symbol(SymbolTable* table, char* varName, int size) {
    // Find the correct index in the hash table
    int index = hash(varName, table->size) % table->size;

    // Check for duplicate symbols in the same scope
    if (lookup_symbol(table, varName) == 1) {
        printf("Error: Symbol %s already exists in this scope.\n", varName);
        return; 
    }

    // Create a new symbol node
    SymbolNode* newNode = malloc(sizeof(SymbolNode));
    newNode->key = strdup(varName);  // Copy the variable name

    // Create a new symbol with variable info
    Symbol* symbol = malloc(sizeof(Symbol));
    symbol->type_str = strdup("INT_ARRAY");
    symbol->size = size;
    symbol->value.intArray = malloc(sizeof(int) * size);  // Allocate space for the int array

    // Add the new symbol node to the front of the linked list at this index
    newNode->var = symbol;
    newNode->next = table->table[index];  
    table->table[index] = newNode;
}


void insert_float_arr_symbol(SymbolTable* table, char* varName, int size) {
    // Find the correct index in the hash table
    int index = hash(varName, table->size) % table->size;

    // Check for duplicate symbols in the same scope   
    if (lookup_symbol(table, varName) == 1) {
        printf("Error: Symbol %s already exists in this scope.\n", varName);
        return; 
    }

    // Create a new symbol node
    SymbolNode* newNode = malloc(sizeof(SymbolNode));
    newNode->key = strdup(varName);  // Copy the variable name

    // Create a new symbol with variable info
    Symbol* symbol = malloc(sizeof(Symbol));
    symbol->type_str = strdup("FLOAT_ARRAY");
    symbol->size = size;
    symbol->value.floatArray = malloc(sizeof(float) * size);  // Allocate space for the float array

    // Add the new symbol node to the front of the linked list at this index
    newNode->var = symbol;
    newNode->next = table->table[index];  
    table->table[index] = newNode;
}

void insert_string_arr_symbol(SymbolTable* table, char* varName, int size) {
    // Find the correct index in the hash table
    int index = hash(varName, table->size) % table->size;

    // Check for duplicate symbols in the same scope
    if (lookup_symbol(table, varName) == 1) {
        printf("Error: Symbol %s already exists in this scope.\n", varName);
        return; 
    }

    // Create a new symbol node
    SymbolNode* newNode = malloc(sizeof(SymbolNode));
    newNode->key = strdup(varName);  // Copy the variable name  

    // Create a new symbol with variable info
    Symbol* symbol = malloc(sizeof(Symbol));
    symbol->type_str = strdup("STRING_ARRAY");
    symbol->size = size;
    symbol->value.stringArray = malloc(sizeof(char*) * size);  // Allocate space for the string array

    // Add the new symbol node to the front of the linked list at this index
    newNode->var = symbol;
    newNode->next = table->table[index];  
    table->table[index] = newNode;
}


void update_string_arr_value(SymbolTable* table, char* id, int index, char* value) {
    // Find the symbol in the table
    Symbol* symbol = getSymbol(table, id);

    // If symbol not found, handle the error
    if (symbol == NULL) {
        printf("Error: Symbol %s not found.\n", id);
        return;
    }

    // Update the value at the given index
    symbol->value.stringArray[index] = strdup(value);
}


void update_float_arr_value(SymbolTable* table, char* id, int index, float value) {
    // Find the symbol in the table
    Symbol* symbol = getSymbol(table, id);        

    // If symbol not found, handle the error
    if (symbol == NULL) {
        printf("Error: Symbol %s not found.\n", id);
        return;
    }

    // Check bounds to ensure the index is valid
    if (index < 0 || index >= symbol->size) {
        printf("Error: Index %d out of bounds for array %s.\n", index, id);
        return;
    }

    // Update the value at the given index
    symbol->value.floatArray[index] = value;

}


void update_int_arr_value(SymbolTable* table, char* id, int index, int value) {
    // Find the symbol in the table
    Symbol* symbol = getSymbol(table, id);

    // If symbol not found, handle the error
    if (symbol == NULL) {
        printf("Error: Symbol %s not found.\n", id);
        return;
    }

    // Check bounds to ensure the index is valid
    if (index < 0 || index >= symbol->size) {
        printf("Error: Index %d out of bounds for array %s.\n", index, id);
        return;
    }

    // Update the value at the given index
    symbol->value.intArray[index] = value;
}


void print_int_array(int* arr, int size) {
    printf("Integer Array: [");
    for (int i = 0; i < size; i++) {
        printf("%d", arr[i]);
        if (i < size - 1) printf(", ");  // Add comma between elements
    }
    printf("]\n");
}



void print_float_array(float* arr, int size) {
    printf("Float Array: [");
    for (int i = 0; i < size; i++) {
        printf("%f", arr[i]);
        if (i < size - 1) printf(", ");  // Add comma between elements
    }
    printf("]\n");
}



void print_string_array(char** arr, int size) {
    printf("String Array: [");
    for (int i = 0; i < size; i++) {
        printf("\"%s\"", arr[i]);
        if (i < size - 1) printf(", ");  // Add comma between elements
    }
    printf("]\n");
}




// Function to print a single symbol table (used for each scope)
void print_symbol_table(SymbolTable* table) {
    for (int i = 0; i < table->size; i++) {
        SymbolNode* current = table->table[i];
        while (current != NULL) {
            printf("Variable: %s, Type: %s, TempVar: %s, Size: %d, Value: ",
                   current->key, current->var->type_str, current->var->tempVar, current->var->size);

            // Print the value based on the type_string
            if (strcmp(current->var->type_str, "INT") == 0) {
                printf("%d\n", current->var->value.intValue);
            } else if (strcmp(current->var->type_str, "FLOAT") == 0) {
                printf("%f\n", current->var->value.floatValue);
            }  else if (strcmp(current->var->type_str, "STRING") == 0) {
                printf("%s\n", current->var->value.stringValue);
            }
            else if (strcmp(current->var->type_str, "INT_ARRAY") == 0) {
                print_int_array(current->var->value.intArray, current->var->size);
            }
            else if (strcmp(current->var->type_str, "FLOAT_ARRAY") == 0) {
                print_float_array(current->var->value.floatArray, current->var->size);
            }
            else if (strcmp(current->var->type_str, "STRING_ARRAY") == 0) {
                print_string_array(current->var->value.stringArray, current->var->size);
            }

            current = current->next;
        }
    }
}


// Function to print the entire symbol table (all scopes)
void print_table(OuterSymbolTable* outerTable) {
    printf("Outer Symbol Table (Scopes):\n");
    for (int i = 0; i < outerTable->size; i++) {
        ScopeNode* currentScope = outerTable->table[i];
        while (currentScope != NULL) {
            printf("Scope: %s, Type: %s\n", currentScope->key, currentScope->return_type);
            if (currentScope->scopeTable != NULL) {
                print_symbol_table(currentScope->scopeTable);  // Print the associated symbol table
            } else {
                printf("    (No symbols in this scope)\n");
            }
            currentScope = currentScope->next;  // Move to the next scope in the list
        }
    }
}


// Function to find a scope in the outer symbol table
int lookup_scope(OuterSymbolTable* outerTable, const char* scopeName) {

    // Hash to find index of scope
    int scopeIndex = hash(scopeName, outerTable->size) % outerTable->size;
    ScopeNode* currentScope = outerTable->table[scopeIndex];
    
    // Traverse the linked list to find the correct scope
    while (currentScope != NULL) {
        if (strcmp(currentScope->key, scopeName) == 0) {
            return 1;  
        }
        currentScope = currentScope->next;  
    }


    return 0;  
}

// Function to find a scope in the outer symbol table
char* get_scope_type(OuterSymbolTable* outerTable, const char* scopeName) {

    // Hash to find index of scope
    int scopeIndex = hash(scopeName, outerTable->size) % outerTable->size;
    ScopeNode* currentScope = outerTable->table[scopeIndex];
    
    // Traverse the linked list to find the correct scope
    while (currentScope != NULL) {
        if (strcmp(currentScope->key, scopeName) == 0) {
            return currentScope->return_type;  
        }
        currentScope = currentScope->next;  
    }


    return NULL;  
}

// Function to find a symbol in a specific scope
int lookup_symbol(SymbolTable* symbolTable, const char* symbolName) {

    // Hash the symbol name to find the correct index in the symbol table
    int symbolIndex = hash(symbolName, symbolTable->size) % symbolTable->size;
    SymbolNode* currentSymbol = symbolTable->table[symbolIndex];
    
    // Traverse the linked list to find the correct symbol
    while (currentSymbol != NULL) {
        if (strcmp(currentSymbol->key, symbolName) == 0) {
            return 1;
        }
        currentSymbol = currentSymbol->next; 
    }

    return 0; 
}


// get symbol function
Symbol* getSymbol(SymbolTable* table, const char* key) {
    int index = hash(key, table->size);
    SymbolNode* current = table->table[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->var;
        } else {
            current = current->next;
        }
    }
    return NULL;  // Return NULL if the symbol is not found
}


void updateRegister(SymbolTable* table, const char* key, char* registerName) {
    Symbol* symbol = getSymbol(table, key);
    if (symbol != NULL) {
        symbol->tempVar = registerName;
        print_symbol_table(table);


    } else {
        printf("ERROR: Symbol %s not found in the symbol table\n", key);
    }
}


// Function to return the symbol table for a given scope
SymbolTable* get_symbol_table(OuterSymbolTable* outerTable, const char* scopeName) {
    // Find the correct scope in the outer hash table
    int scopeIndex = hash(scopeName, outerTable->size) % outerTable->size;
    ScopeNode* currentScope = outerTable->table[scopeIndex];

    // Traverse the linked list to find the matching scope
    while (currentScope != NULL) {
        if (strcmp(currentScope->key, scopeName) == 0) {
            return currentScope->scopeTable; 
        }
        currentScope = currentScope->next;
    }

    printf("Scope '%s' not found!\n", scopeName);
    return NULL; 
}