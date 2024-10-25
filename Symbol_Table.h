#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

// Node structure for individual entries in a symbol table (hash table for variables)
typedef struct SymbolNode {
    char* key; 
    struct Symbol* var;
    struct SymbolNode* next;
} SymbolNode;

// Structure for symbol table (hash table for variables in a scope)
typedef struct SymbolTable {
    SymbolNode** table; 
    int size;           
} SymbolTable;

// Node structure for the outer hash table (scopes)
typedef struct ScopeNode {
    char* key;
    char* return_type;                
    SymbolTable* scopeTable;  
    struct ScopeNode* next; 
} ScopeNode;

// Structure for the outer symbol table (hash table for scopes)
typedef struct OuterSymbolTable {
    ScopeNode** table; 
    int size;           
} OuterSymbolTable;

// enum to define types of variables
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_INT_ARR,
    TYPE_FLOAT_ARR,
    TYPE_STRING_ARR,
    TYPE_STRUCT
} VarType;

// union to define values of variables
typedef union {
    int intValue;
    float floatValue;
    char* stringValue;
    int* intArray;
    float* floatArray;
    char** stringArray;
    SymbolTable* structValue;
} VarValue;

// Structure for Symbol
typedef struct Symbol{
    VarType type;
    VarValue value;
    int size;
    char* tempVar;
    char* type_str;
    char* id;
}Symbol;

//-------Functions---------

// Create functions
SymbolTable* create_symbol_table(int size);
OuterSymbolTable* create_outer_table(int size);

// Insert functions
unsigned int hash(const char* key, int size);
void insert_struct_symbol(SymbolTable* table, char* var_name, SymbolTable* struct_table, char* struct_name);
void insert_int_symbol(SymbolTable* table, char* varName, int value);
void insert_float_symbol(SymbolTable* table, char* varName, float value);
void insert_string_symbol(SymbolTable* table, char* varName, const char* value);
void insert_int_arr_symbol(SymbolTable* table, char* varName, int size);
void insert_float_arr_symbol(SymbolTable* table, char* varName, int size);
void insert_string_arr_symbol(SymbolTable* table, char* varName, int size);
void insert_scope(OuterSymbolTable* outerTable, char* scopeName, int innerTableSize, char* return_type);
void insert_struct(OuterSymbolTable* outer_table, char* scope_name, int inner_table_size);

// print and lookup functions
void print_int_array(int* arr, int size);
void print_float_array(float* arr, int size);
void print_string_array(char** arr, int size);
void print_table(OuterSymbolTable* outerTable);
void print_symbol_table(SymbolTable* table, int indent);
int lookup_scope(OuterSymbolTable* outerTable, const char* scopeName);
int lookup_symbol(SymbolTable* symbolTable, const char* symbolName);
int lookup_struct_variable(SymbolTable* table, const char* struct_id, const char* var_id);
char* get_scope_type(OuterSymbolTable* outerTable, const char* scopeName);
Symbol* get_struct_variable(SymbolTable* table, const char* struct_id, const char* var_id);

// void free_table(SymbolTable* table);
Symbol* getSymbol(SymbolTable* table, const char* key);
char* getTempVar(Symbol* symbol);
SymbolTable* get_symbol_table(OuterSymbolTable* outerTable, const char* scopeName);

// Update Functions
void update_int_arr_value(SymbolTable* table, char* id, int index, int value);
void update_float_arr_value(SymbolTable* table, char* id, int index, float value);
void update_string_arr_value(SymbolTable* table, char* id, int index, char* value);
void updateRegister(SymbolTable* table, const char* key, char* registerName);
void updateValueInt(SymbolTable* table, const char* key, int new_value);

// copy function
SymbolTable* deep_copy_symbol_table(SymbolTable* original);

//Free Table
void freeSymbolTable(SymbolTable* table);
#endif // SYMBOL_TABLE_H

