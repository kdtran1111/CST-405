// symbol_table.h
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct Symbol{
    char* type;
    int value;
    char* tempVar; //tempVar for t0,t1,etc...
}Symbol;

// Define your symbol table struct
typedef struct SymbolNode {
    char* key;
    struct Symbol* var;
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
Symbol* getSymbol(SymbolTable* table, const char* key);
char* getTempVar(Symbol* symbol);
//char* getTempVar(SymbolTable* table, const char* key);
void updateRegister(SymbolTable* table, const char* key, char* registerName);
void updateValue(SymbolTable* table, const char* key, int new_value);
//Free Table
void freeSymbolTable(SymbolTable* table);
#endif // SYMBOL_TABLE_H
