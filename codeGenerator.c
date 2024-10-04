#include "codeGenerator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

static FILE* outputFile;
static ASMSymbol* symbolTable = NULL;
static TAC* head = NULL;

void initCodeGenerator(const char* outputFilename) {
    outputFile = fopen(outputFilename, "w");
    if (!outputFile) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }
}

void finalizeCodeGenerator() {
    if (outputFile) {
        fclose(outputFile);
    }
}

void addSymbol(const char* name, int value) {
    ASMSymbol* newSymbol = (ASMSymbol*)malloc(sizeof(ASMSymbol));
    newSymbol->name = strdup(name);
    newSymbol->value = value;
    newSymbol->next = symbolTable;
    symbolTable = newSymbol;
}

ASMSymbol* findSymbol(const char* name) {
    ASMSymbol* current = symbolTable;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void generateMIPS(TAC* tacInstructions) {
    fprintf(outputFile, ".data\n");
    fprintf(outputFile, ".text\n");
    fprintf(outputFile, ".global main\n\n");
    fprintf(outputFile, "main:\n");

    TAC* current = tacInstructions;
    while (current) {
        if (strcmp(current->op, "+") == 0) {
            fprintf(outputFile, "    add $%s, $%s, $%s\n", current->result, current->arg1, current->arg2);
        } else if (strcmp(current->op, "-") == 0) {
            fprintf(outputFile, "    sub $%s, $%s, $%s\n", current->result, current->arg1, current->arg2);
        } else if (strcmp(current->op, "*") == 0) {
            fprintf(outputFile, "    mul $%s, $%s, $%s\n", current->result, current->arg1, current->arg2);
        } else if (strcmp(current->op, "/") == 0) {
            fprintf(outputFile, "    div $%s, $%s\n", current->arg1, current->arg2);
            fprintf(outputFile, "    mflo $%s\n", current->result);
        } else if (strcmp(current->op, "write") == 0) {
            fprintf(outputFile, "    li $v0, 1\n");
            fprintf(outputFile, "    move $a0, $%s\n", current->arg1);
            fprintf(outputFile, "    syscall\n");
        } else if (strcmp(current->op, "=") == 0) {
            // Check if arg1 is a number and load it as an immediate value
            int immediateValue;
            if (sscanf(current->arg1, "%d", &immediateValue) == 1) {
                fprintf(outputFile, "    li $%s, %d\n", current->result, immediateValue);
            } else {
                // Use move for variable assignments
                fprintf(outputFile, "    move $%s, $%s\n", current->result, current->arg1);
            }
        }
        current = current->next;
    }
}

TAC* readTACFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open TAC file");
        exit(EXIT_FAILURE);
    }

    TAC* head = NULL;
    TAC* tail = NULL;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        TAC* newTAC = (TAC*)malloc(sizeof(TAC));
        newTAC->next = NULL;

        char result[MAX_LINE_LENGTH], arg1[MAX_LINE_LENGTH], op[MAX_LINE_LENGTH], arg2[MAX_LINE_LENGTH];

        if (sscanf(line, "%s = %s %s %s", result, arg1, op, arg2) == 4) {
            newTAC->result = strdup(result);
            newTAC->arg1 = strdup(arg1);
            newTAC->op = strdup(op);
            newTAC->arg2 = strdup(arg2);
        } else if (sscanf(line, "%s = %s", result, arg1) == 2) {
            newTAC->result = strdup(result);
            newTAC->arg1 = strdup(arg1);
            newTAC->op = strdup("=");
            newTAC->arg2 = NULL;
        } else if (sscanf(line, "write %s", arg1) == 1) {
            newTAC->result = NULL;
            newTAC->arg1 = strdup(arg1);
            newTAC->op = strdup("write");
            newTAC->arg2 = NULL;
        } else {
            fprintf(stderr, "Invalid TAC instruction: %s", line);
            free(newTAC);
            continue;
        }

        if (!head) {
            head = newTAC;
        } else {
            tail->next = newTAC;
        }
        tail = newTAC;
    }

    fclose(file);
    return head;
}
void executeCodeGenerator(const char* tacFilename, const char* outputFilename) {
    initCodeGenerator(outputFilename);
    TAC* tacInstructions = readTACFromFile(tacFilename);
    generateMIPS(tacInstructions);
    finalizeCodeGenerator();
}
