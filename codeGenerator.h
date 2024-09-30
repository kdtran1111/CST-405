#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdbool.h>

#define NUM_TEMP_REGISTERS 10 // Number of temporary registers

typedef struct TAC {
    char* op;    // Operator
    char* arg1;  // Argument 1
    char* arg2;  // Argument 2
    char* result; // Result
    struct TAC* next; // Next instruction
} TAC;

// Structure to represent a symbol in the symbol table
typedef struct Symbol {
    char* name;  // Variable name
    int value;   // Variable value
    struct Symbol* next; // Pointer to the next symbol
} Symbol;

// Function prototypes
void initCodeGenerator(const char* outputFilename);
void generateMIPS(TAC* tacInstructions);
void finalizeCodeGenerator(); // Remove parameter
TAC* readTACFromFile(const char* filename);
void executeCodeGenerator(const char* tacFilename, const char* outputFilename);
void addSymbol(const char* name, int value); // Added for symbol table management

#endif // CODE_GENERATOR_H
