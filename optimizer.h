#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Helper function to create a TAC node
TAC* createTAC(char* result, char* arg1, char* op, char* arg2);

// Helper function to append TAC node to the list
void addTAC(TAC** head, TAC* newTAC);

// Check if a string is an integer constant
bool isConstant(const char* str);

// Check if a string is a valid variable name
bool isVariable(const char* str);

// Function to free the TAC list
void freeTAC(TAC* head);

// Constant propagation optimization
void constantPropagation(TAC** head);

// Constant folding optimization
void constantFolding(TAC** head);

// Copy propagation optimization
void copyPropagation(TAC** head);

// Dead code elimination optimization
void deadCodeElimination(TAC** head);

// Print the optimized TAC list to a file
void printOptimizedTAC(const char* filename, TAC* head);

// Optimize the TAC with all optimization methods
void optimizeTAC(TAC** head);

// Load TAC from file
void loadTAC(const char* filename, TAC** head);

void optimizer(const char* filename);

#endif // OPTIMIZER_H
