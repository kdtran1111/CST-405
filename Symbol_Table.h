// symbol_table.h
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct Symbol{
    char* type;
    int value;
}Symbol;

// Define your symbol table struct
typedef struct SymbolNode {
    char* key;
    struct Symbol* value;
    struct SymbolNode* next; 
} SymbolNode;

typedef struct SymbolTable {
    SymbolNode** table; // Array of linked list heads (hash table)
    int size;
} SymbolTable;


// Function prototypes
SymbolTable* create_table(int size);  // Create a symbol table
unsigned int hash(const char* key, int size);
void insert(SymbolTable* table, const char* key, Symbol* value);
void print_table(SymbolTable* table);
int lookup(SymbolTable* table, const char* key);
// void free_table(SymbolTable* table);

#endif // SYMBOL_TABLE_H
