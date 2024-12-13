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

// Helper function to parse operands like t5[0]
void parseIndexedOperand(const char* operand, char* base, char* offset) {
    const char* openBracket = strchr(operand, '[');
    const char* closeBracket = strchr(operand, ']');

    if (openBracket && closeBracket && openBracket < closeBracket) {
        // Extract base (e.g., t5)
        strncpy(base, operand, openBracket - operand);
        base[openBracket - operand] = '\0';

        // Extract offset (e.g., 0)
        strncpy(offset, openBracket + 1, closeBracket - openBracket - 1);
        offset[closeBracket - openBracket - 1] = '\0';
    } else {
        // No brackets, treat the whole operand as base with no offset
        strcpy(base, operand);
        offset[0] = '\0';
    }
}

#include <stdbool.h>

// Define a simple linked list to track .data components
typedef struct DataComponent {
    char* name;
    struct DataComponent* next;
} DataComponent;

DataComponent* dataComponentsHead = NULL;

// Function to add a component to the .data list
void addDataComponent(const char* name) {
    DataComponent* newComponent = (DataComponent*)malloc(sizeof(DataComponent));
    newComponent->name = strdup(name);
    newComponent->next = dataComponentsHead;
    dataComponentsHead = newComponent;
}

// Function to check if a name exists in the .data list
bool isDataComponent(const char* name) {
    DataComponent* current = dataComponentsHead;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void generateMIPS(TAC* tacInstructions) {
    TAC* current = tacInstructions;
    bool dataSectionPrinted = false;

    // First pass: Handle .data section declarations
    fprintf(outputFile, ".data\n");
    while (current) {
        if (current->keyword && strcmp(current->keyword, "data_for_array") == 0) {
            if (!dataSectionPrinted) {
                dataSectionPrinted = true;
            }
            fprintf(outputFile, "%s: .space %s\n", current->result, current->arg1);
            addDataComponent(current->result); // Add to list of data symbols
            fprintf(stderr, "DEBUG: Declaring data symbol %s with space %s\n", current->result, current->arg1);
        }
        current = current->next;
    }
    fprintf(outputFile, "\n");

    // Reset pointer to the start for the second pass
    current = tacInstructions;

    // Start .text section and prioritize main
    fprintf(outputFile, ".text\n");
    fprintf(outputFile, ".globl main\n");
    fprintf(stderr, "DEBUG: .text section and .globl main printed\n");

    // Handle the main function first
    current = tacInstructions; // Reset pointer
    while (current) {
        if (current->keyword && strcmp(current->keyword, "label") == 0 && strcmp(current->result, "main") == 0) {
            fprintf(outputFile, "%s:\n", current->result);
            fprintf(stderr, "DEBUG: Main label prioritized and printed\n");

            // Output instructions under main
            TAC* mainInstructions = current->next;
            while (mainInstructions && !(mainInstructions->keyword && strcmp(mainInstructions->keyword, "label") == 0)) {
                printInstruction(mainInstructions);
                mainInstructions = mainInstructions->next;
            }

            // Insert exit syscall after the last conditional block
            fprintf(stderr, "DEBUG: Inserting syscall after last block\n");
        }
        current = current->next;
    }

    // Print all other labels and their instructions
    current = tacInstructions; // Reset pointer
    while (current) {
        if (current->keyword && strcmp(current->keyword, "label") == 0 && strcmp(current->result, "main") != 0) {
            fprintf(outputFile, "%s:\n", current->result);
            fprintf(stderr, "DEBUG: Label %s generated\n", current->result);
            // Output instructions under the current label
            TAC* labelInstructions = current->next;
            while (labelInstructions && !(labelInstructions->keyword && strcmp(labelInstructions->keyword, "label") == 0)) {
                printInstruction(labelInstructions);
                labelInstructions = labelInstructions->next;
            }
        }
        current = current->next;
    }

    // Final exit syscall after all instructions have been processed
    fprintf(outputFile, "li $v0, 10\n");  // Load syscall code for exit
    fprintf(outputFile, "syscall\n");      // Terminate program
    fprintf(stderr, "DEBUG: Automatically inserted li $v0, 10 and syscall at the end\n");
}

void printInstruction(TAC* instruction) {
    if (instruction->keyword && strcmp(instruction->keyword, "jump") == 0) {
        if (strcmp(instruction->arg1, "end_func") == 0) {
            fprintf(outputFile, "jr $ra\n"); // Replace jump to end_func with return
            fprintf(stderr, "DEBUG: Replacing jump to end_func with jr $ra\n");
        } else {
            fprintf(outputFile, "j %s\n", instruction->arg1);
            fprintf(stderr, "DEBUG: Jump to %s\n", instruction->arg1);
        }
    } else if (instruction->keyword && strcmp(instruction->keyword, "write") == 0) {
        fprintf(outputFile, "li $v0, 1\n");         // Load syscall for print integer
        fprintf(outputFile, "move $a0, $%s\n", instruction->arg1);
        fprintf(outputFile, "syscall\n");
        fprintf(stderr, "DEBUG: Writing value in register $%s\n", instruction->arg1);
    } else if (instruction->op && strcmp(instruction->op, "=") == 0) {
        char base[MAX_LINE_LENGTH] = {0};
        char offset[MAX_LINE_LENGTH] = {0};
        parseIndexedOperand(instruction->result, base, offset);

        // Align the offset to the nearest multiple of 4 if it isn't already aligned
        if (strlen(offset) > 0) {
            int offsetValue = atoi(offset); // Convert the offset to an integer
            int alignedOffset = (offsetValue / 4) * 4; // Align to the nearest multiple of 4
            if (offsetValue != alignedOffset) {
                fprintf(stderr, "DEBUG: Adjusting offset %d to aligned offset %d\n", offsetValue, alignedOffset);
            }

            // Generate the store instruction with the aligned offset
            fprintf(outputFile, "sw $%s, %d($%s)\n", instruction->arg1, alignedOffset, base);
            fprintf(stderr, "DEBUG: Storing $%s at offset %d from base $%s\n", instruction->arg1, alignedOffset, base);
        } else {
            // Handle the case where there's no offset
            if (isDataComponent(instruction->arg1)) {
                fprintf(outputFile, "la $%s, %s\n", instruction->result, instruction->arg1);
                fprintf(stderr, "DEBUG: Loading address of .data symbol %s into $%s\n", instruction->arg1, instruction->result);
            } else if (isdigit(instruction->arg1[0])) { // Handle immediate values
                fprintf(outputFile, "li $%s, %s\n", instruction->result, instruction->arg1);
                fprintf(stderr, "DEBUG: Loading immediate value %s into $%s\n", instruction->arg1, instruction->result);
            } else {
                fprintf(outputFile, "move $%s, $%s\n", instruction->result, instruction->arg1);
                fprintf(stderr, "DEBUG: Moving value from $%s to $%s\n", instruction->arg1, instruction->result);
            }
        }
    } else if (instruction->op && strcmp(instruction->op, "+") == 0) {
        // Check if either operand is indexed (e.g., t5[0])
        char base1[MAX_LINE_LENGTH], offset1[MAX_LINE_LENGTH];
        char base2[MAX_LINE_LENGTH], offset2[MAX_LINE_LENGTH];

        bool isIndexed1 = strchr(instruction->arg1, '[') != NULL;
        bool isIndexed2 = strchr(instruction->arg2, '[') != NULL;

        if (isIndexed1) {
            parseIndexedOperand(instruction->arg1, base1, offset1);  // Parse first operand
            fprintf(outputFile, "lw $t8, %s($%s)\n", offset1, base1);  // Load value from memory
            fprintf(stderr, "DEBUG: Loading value from memory $%s[%s] into $t8\n", base1, offset1);
        } else {
            fprintf(outputFile, "move $t8, $%s\n", instruction->arg1);
            fprintf(stderr, "DEBUG: Moving value from $%s to $t8\n", instruction->arg1);
        }

        if (isIndexed2) {
            parseIndexedOperand(instruction->arg2, base2, offset2);  // Parse second operand
            fprintf(outputFile, "lw $t9, %s($%s)\n", offset2, base2);  // Load value from memory
            fprintf(stderr, "DEBUG: Loading value from memory $%s[%s] into $t9\n", base2, offset2);
        } else {
            fprintf(outputFile, "move $t9, $%s\n", instruction->arg2);
            fprintf(stderr, "DEBUG: Moving value from $%s to $t9\n", instruction->arg2);
        }

        // Perform the addition
        fprintf(outputFile, "add $%s, $t8, $t9\n", instruction->result);
        fprintf(stderr, "DEBUG: Adding values in $t8 and $t9 into $%s\n", instruction->result);
    } else if (instruction->keyword && strcmp(instruction->keyword, "IF") == 0) {
    // Print the raw instruction to debug its format
    fprintf(stderr, "DEBUG: Raw IF statement: %s %s\n", instruction->arg1, instruction->arg2);

    // Parse the condition (e.g., "t4 >= 0") and convert it into the correct branch instruction
    char condition[MAX_LINE_LENGTH];
    strcpy(condition, instruction->arg1);

    char op[MAX_LINE_LENGTH];
    sscanf(instruction->op, "%s", op);  // Extract the operator (e.g., ">", "<=", etc.)

    // Debugging: Check the value of the operator
    fprintf(stderr, "DEBUG: Operator found: %s\n", op);

    // Generate the correct branch instruction based on the operator
    if (strcmp(op, ">=") == 0) {
        fprintf(outputFile, "bge %s, $zero, %s\n", condition, instruction->arg2);  // Branch if greater or equal
    } else if (strcmp(op, "<") == 0) {
        fprintf(outputFile, "blt %s, $zero, %s\n", condition, instruction->arg2);  // Branch if less than
    } else if (strcmp(op, ">") == 0) {
        fprintf(outputFile, "bgt %s, $zero, %s\n", condition, instruction->arg2);  // Branch if greater than
    } else if (strcmp(op, "<=") == 0) {
        fprintf(outputFile, "ble %s, $zero, %s\n", condition, instruction->arg2);  // Branch if less or equal
    } else if (strcmp(op, "==") == 0) {
        fprintf(outputFile, "beq %s, $zero, %s\n", condition, instruction->arg2);  // Branch if equal
    } else if (strcmp(op, "!=") == 0) {
        fprintf(outputFile, "bne %s, $zero, %s\n", condition, instruction->arg2);  // Branch if not equal
    } else {
        fprintf(stderr, "DEBUG: Unknown operator in IF statement: '%s'\n", op);  // Additional debugging for unknown operators
    }

    fprintf(stderr, "DEBUG: Generated IF conditional branch: %s %s, $zero, %s\n", op, condition, instruction->arg2);
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
        // Trim newline characters
        line[strcspn(line, "\n")] = '\0';

        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }

        // Allocate a new TAC node
        TAC* newTAC = (TAC*)malloc(sizeof(TAC));
        if (!newTAC) {
            fprintf(stderr, "Memory allocation failed for TAC node.\n");
            exit(EXIT_FAILURE);
        }

        // Initialize all fields to NULL
        newTAC->keyword = NULL;
        newTAC->result = NULL;
        newTAC->arg1 = NULL;
        newTAC->arg2 = NULL;
        newTAC->next = NULL;

        char result[MAX_LINE_LENGTH], arg1[MAX_LINE_LENGTH], op[MAX_LINE_LENGTH], arg2[MAX_LINE_LENGTH];

        // Parse TAC instructions
        if (sscanf(line, "%s = %s %s %s", result, arg1, op, arg2) == 4) {
            newTAC->result = strdup(result);
            newTAC->arg1 = strdup(arg1);
            newTAC->op = strdup(op);
            newTAC->arg2 = strdup(arg2);
        } else if (sscanf(line, "%s = %s", result, arg1) == 2) {
            newTAC->result = strdup(result);
            newTAC->arg1 = strdup(arg1);
            newTAC->op = strdup("=");
        } else if (sscanf(line, "write %s", arg1) == 1) {
            newTAC->keyword = strdup("write");
            newTAC->arg1 = strdup(arg1);
        } 
        // Handle array or struct declarations with `.space`
        else if (sscanf(line, "%[^:]: .space %s", result, arg1) == 2) {
            newTAC->keyword = strdup("data_for_array");
            newTAC->result = strdup(result);
            newTAC->arg1 = strdup(arg1);
        } 
        // Handle section declarations
        else if (strcmp(line, ".data") == 0) {
            newTAC->keyword = strdup("data");
        } else if (strcmp(line, ".text") == 0) {
            newTAC->keyword = strdup("text");
        } else if (strcmp(line, ".globl") == 0) {
            newTAC->keyword = strdup("globl");
        } 
        // Handle labels
        else if (line[strlen(line) - 1] == ':') {
            newTAC->keyword = strdup("label");
            line[strlen(line) - 1] = '\0'; // Remove colon for label
            newTAC->result = strdup(line);
        } 
        // Handle jump instructions
        else if (strncmp(line, "jump ", 5) == 0) {
            newTAC->keyword = strdup("jump");
            newTAC->arg1 = strdup(line + 5); // Skip "jump "
        } 
        // Handle IF conditionals
        else if (strncmp(line, "IF ", 3) == 0) {
            newTAC->keyword = strdup("IF");
            fprintf(stderr, "DEBUG: Parsing IF statement: %s\n", line);

            // Find the position of "GOTO" in the line
            char *gotoPos = strstr(line, " GOTO ");
            if (gotoPos) {
                // Separate the condition from the label
                *gotoPos = '\0';  // Null-terminate the condition part
                char *condition = line + 3;  // Skip the "IF " part

                // The label comes after the "GOTO"
                char *label = gotoPos + 6;  // Skip the " GOTO " part

                // Debugging output
                fprintf(stderr, "DEBUG: Condition parsed: %s\n", condition);
                fprintf(stderr, "DEBUG: Label parsed: %s\n", label);

                // Extract the operator from the condition (e.g., t4 >= 0)
                char var1[MAX_LINE_LENGTH], operator[MAX_LINE_LENGTH], var2[MAX_LINE_LENGTH];
                if (sscanf(condition, "%s %s %s", var1, operator, var2) == 3) {
                    newTAC->arg1 = strdup(var1);  // The first operand (e.g., "t4")
                    newTAC->op = strdup(operator);  // The operator (e.g., ">=")
                    newTAC->arg2 = strdup(var2);  // The second operand (e.g., "0")
                } else {
                    fprintf(stderr, "DEBUG: Failed to parse condition: %s\n", condition);
                    free(newTAC);
                    continue;
                }

                // Assign the label to the "arg2" field (this may need adjustment if more than one operand is involved)
                newTAC->result = strdup(label);  // The label to go to
            } else {
                fprintf(stderr, "Invalid IF statement format (missing GOTO): %s\n", line);
                fprintf(stderr, "DEBUG: Failed to parse condition or label.\n");
                free(newTAC);
                continue;
            }
        }
        // Handle other cases and invalid instructions
        else {
            fprintf(stderr, "Invalid TAC instruction: %s\n", line);
            free(newTAC);
            continue;
        }

        // Append to the TAC list
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
    fprintf(stderr, "TESTING\n");
    generateMIPS(tacInstructions);
    finalizeCodeGenerator();
}
