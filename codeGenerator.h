#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdbool.h>
#include "semantic.h"

#define NUM_TEMP_REGISTERS 10 // Number of temporary registers

typedef struct ASMSymbol {
    char* name;
    int value;
    struct ASMSymbol* next;
} ASMSymbol;


// Function prototypes
void initCodeGenerator(const char* outputFilename);
void generateMIPS(TAC* tacInstructions);
void finalizeCodeGenerator();
TAC* readTACFromFile(const char* filename);
void executeCodeGenerator(const char* tacFilename, const char* outputFilename);
void addSymbolToTable(const char* varName); // Function to add variable to symbol table
void generateSymbolTable(); // Function to output the symbol table

#endif // CODE_GENERATOR_H
