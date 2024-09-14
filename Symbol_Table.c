// symbol_table.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Symbol_Table.h"

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

        new_node->value = value;
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
        new_node->value = value;
        new_node->next = NULL;
        current->next = new_node;

    }
}

void print_table(SymbolTable* table) {
    for (int i = 0; i < table->size; i++) {
        SymbolNode* current = table->table[i];
        while (current != NULL) {
            printf("Identifier: %s, Type: %s, Value: %d\n", current->key, current->value->type, current->value->value);
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
            return 1;
        }
        else {
            current = current->next;
        }
    }
    return 0;
}



