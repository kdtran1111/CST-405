// symbol_table.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Symbol_Table.h"
#include "semantic.h"
// Implementation of functions declared in symbol_table.h

// Function to create a symbol table
SymbolTable* create_table(int size) {
    SymbolTable* new_table = malloc(sizeof(SymbolTable));
    new_table->table = malloc(sizeof(SymbolNode*) * size);
    new_table->size = size;
    for (int i = 0; i < size; i++) {
        new_table->table[i] = NULL;  // Initialize all linked list heads to NULL
    }
    return new_table;
}

unsigned int hash(const char* key, int table_size) {
    unsigned int hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash % table_size;  // Return an index within the table size
}

void insert (SymbolTable* table, const char* key, Symbol* value) {
    int index = hash(key, table->size);
    if (table->table[index] == NULL) 
    {
        SymbolNode* new_node = malloc(sizeof(SymbolNode));

        new_node->key = malloc(strlen(key) + 1);
        strcpy(new_node->key, key);

        new_node->var = value;
        
        //new_node->var->tempVar=createTempVar();
        new_node->next = NULL;
        table->table[index] = new_node; 
    }
    else
    {   SymbolNode* current = table->table[index];
        while (current->next != NULL)
        {
            current = current->next;
        }

        SymbolNode* new_node = malloc(sizeof(SymbolNode));
        new_node->key = malloc(strlen(key) + 1);
        strcpy(new_node->key, key);
        new_node->var = value;
        new_node->next = NULL;
        current->next = new_node;

    }
}

void print_table(SymbolTable* table) {
    for (int i = 0; i < table->size; i++) {
        SymbolNode* current = table->table[i];
        while (current != NULL) {
            printf("Identifier: %s, Type: %s, Value: %d tempVar: %s\n", current->key, current->var->type, current->var->value, current->var->tempVar);
            printf("Address: %p\n", (void*)current);
            current = current->next;
        }
    }
}

int lookup(SymbolTable* table, const char* key) {
    int index = hash(key, table->size);
    SymbolNode* current = table->table[index];

    while (current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            //return 1 if
            return 1;
        }
        else {
            current = current->next;
        }
    }
    return 0;
}


// New function to get the Symbol* by key 
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

//Function to get tempVar 
/*
char* getTempVar(SymbolTable* table, const char* key){
    int index = hash(key, table->size);
    SymbolNode* current = table->table[index];
    
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->var->tempVar;
        } else {
            current = current->next;
        }
    }
    return NULL;  // Return NULL if the symbol is not found
}
*/
// Function to get tempVar from the symbol table based on the given key
char* getTempVar(Symbol* symbol) {
    // Check for valid table and key
    if (symbol == NULL) {
        printf("Error: Symbol is NULL\n");
        return NULL;
    }else{
        return symbol->tempVar;
    }


    // If the symbol is not found in the table
    printf("Error: Symbol not found in the symbol table\n");
    return NULL;
}



// New function to update the value of a symbol. Not yet needed. Will eventually be add into effect
//Trying new updateValue with TempVar
void updateValue(SymbolTable* table, const char* key, int new_value) {
    Symbol* symbol = getSymbol(table, key);
    if (symbol != NULL) {
        symbol->value = new_value;
        print_table(table);
       /*
       No need to update tempVar since it's still be in the same register with the declaration of the variable
       */
        // If this is the first time assigning a value, create a temp variable
        if (symbol->tempVar == NULL) {
            symbol->tempVar = createTempVar();
            printf("new temp var created in updateaValue():   %s\n", symbol->tempVar);
        }
        
        printf("Updated value of %s to %d with tempVar %s\n", key, new_value, symbol->tempVar);
    } else {
        printf("ERROR: Symbol %s not found in the symbol table\n", key);
    }
}
/*
void updateTempVar(SymbolTable* table, const char* key, char* tempVar){
    Symbol
}
*/
/* Working updateValue
void updateValue(SymbolTable* table, const char* key, int new_value) {
    
    Symbol* symbol = getSymbol(table, key);
    if (symbol != NULL) {
         printf("Before update: %s has value %d at address %p\n", key, symbol->value, (void*)symbol);
        symbol->value = new_value;  // Update the value of the symbol
     
        printf("Updated value of %s to %d at address: %p\n", key, new_value,(void*)symbol);
        print_table(table);
    } else {
        printf("ERROR: Symbol %s not found in the symbol table\n", key);
    }
}*/
/*
// Function to free the symbol table
void freeSymbolTable(SymbolTable* table) {
    for (int i = 0; i < table->size; i++) {
        Symbol* sym = table->table[i];
        while (sym != 0) {
            Symbol* nextSym = sym->next;
            free(sym->name);
            free(sym->type);
            // Free other dynamically allocated fields of Symbol
            free(sym);
            sym = nextSym;
        }
    }
    free(table->table);
    free(table);
}
*/