#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "AST.h"         // Assuming this includes the definition of ASTNode
#include "Symbol_Table.h" // Assuming this includes the definition of SymbolTable

// Define a structure for TAC instructions

typedef struct TAC {
    char* op;   // Operator
    char* arg1; // Argument 1
    char* arg2; // Argument 2
    char* result; // Result
    char* keyword; //such as write
    struct TAC* next; // Next instruction
} TAC;

typedef struct TempVarMap {
    char* key;   // Variable or expression
    char* tempVar; // Corresponding temp variable (e.g., t0, t1)
    struct TempVarMap* next;
} TempVarMap;

extern int tempVars[20];
extern TAC* tacHead; // Global head of the TAC instructions list
extern char* currentID;

int allocateNextAvailableTempVar(int tempVars[]);
void semanticAnalysis(ASTNode* node, OuterSymbolTable* symTab);
TAC* generateTACForExpr(ASTNode* expr, OuterSymbolTable* outer_table); //returns the TAC for the expression to print on console
char* createTempVar();
//char* createOperand(ASTNode* node);
char* createOperand(ASTNode* node, SymbolTable* symbol_table);
void printTAC(TAC* tac);
void initializeTempVars();
void deallocateTempVar(int tempVars[], int index);
int allocateNextAvailableTempVar(int tempVars[]);
void appendTAC(TAC** head, TAC* newInstruction);
void printTACToFile(const char* filename, TAC* tac);
// You can add more function declarations related to semantic analysis here
TAC* generateTACForSimpleExpr(ASTNode* expr, SymbolTable* symbol_table);

// Function prototypes for array declaration checks
void check_array_not_redeclared(SymbolTable* table, const char* id, int line);

void check_array_type(const char* type, int line);
void check_array_size(int size, int line);
//main function to combine all semantic check for array
void check_array_declaration(SymbolTable* table, const char* type, const char* id, int size, int line);

//Trying TempVar Map
void addTempVarMapping(char* key, char* tempVar);
char* findTempVar(char* key);
//Function for type coercion
void apply_type_coercion(SymbolTable* symbol_table, const char* id, ASTNode* expr, int line);
void def_outer_table_semantic(OuterSymbolTable* outer_table);
#endif // SEMANTIC_H
