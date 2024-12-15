#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Structure to represent a TAC (Three Address Code) instruction
typedef struct TAC {
    char* result;
    char* arg1;
    char* op;
    char* arg2;
    struct TAC* next;
} TAC;

// Helper function to create a TAC node
TAC* createTAC(char* result, char* arg1, char* op, char* arg2) 
{
    TAC* newTAC = (TAC*)malloc(sizeof(TAC));
    if (!newTAC) {
        perror("Failed to allocate memory for TAC");
        exit(EXIT_FAILURE);
    }
    newTAC->result = result ? strdup(result) : NULL;
    newTAC->arg1 = arg1 ? strdup(arg1) : NULL;
    newTAC->op = op ? strdup(op) : NULL;
    newTAC->arg2 = arg2 ? strdup(arg2) : NULL;
    newTAC->next = NULL;
    return newTAC;
}

// Helper function to append TAC node
void addTAC(TAC** head, TAC* newTAC) 
{
    if (*head == NULL) {
        *head = newTAC;
    } else {
        TAC* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newTAC;
    }
}

// Check if a string is an integer constant.
bool isConstant(const char* str) 
{
    if (str == NULL || *str == '\0') {
        return false;
    }
    if (*str == '-') {
        ++str; 
    }
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return false;
        }
        ++str;
    }
    return true;
}

// Check if a string is a valid variable name.
bool isVariable(const char* str) 
{
    if (str == NULL || *str == '\0') {
        return false;
    }
    if (!isalpha((unsigned char)*str) && *str != '_') {
        return false;
    }
    ++str;
    while (*str) {
        if (!isalnum((unsigned char)*str) && *str != '_') {
            return false;
        }
        ++str;
    }
    return true;
}

// Function to free TAC list
void freeTAC(TAC* head) 
{
    while (head) {
        TAC* temp = head;
        head = head->next;
        free(temp->result);
        free(temp->arg1);
        free(temp->op);
        free(temp->arg2);
        free(temp);
    }
}

// Constant propagation optimization
void constantPropagation(TAC** head) 
{
    TAC* current = *head;
    while (current != NULL) {
        if (current->op != NULL && strcmp(current->op, "write") == 0) {
            current = current->next;
            continue;
        }

        if (current->op != NULL && strcmp(current->op, "=") == 0 && isConstant(current->arg1)) {
            TAC* temp = current->next;
            while (temp != NULL) {
                if (temp->arg1 != NULL && strcmp(temp->arg1, current->result) == 0) {
                    free(temp->arg1);
                    temp->arg1 = strdup(current->arg1);
                }
                if (temp->arg2 != NULL && strcmp(temp->arg2, current->result) == 0) {
                    free(temp->arg2);
                    temp->arg2 = strdup(current->arg1);
                }
                temp = temp->next;
            }
        }
        current = current->next;
    }
}

// Constant folding optimization
void constantFolding(TAC** head) 
{
    TAC* current = *head;
    while (current != NULL) {
        if (current->op != NULL && strcmp(current->op, "write") == 0) {
            current = current->next;
            continue;
        }

        char* arg1Value = current->arg1;
        char* arg2Value = current->arg2;

        if (isVariable(arg1Value)) {
            TAC* temp = *head;
            while (temp != NULL) {
                if (temp->op != NULL && strcmp(temp->op, "=") == 0 && strcmp(temp->result, arg1Value) == 0) {
                    arg1Value = temp->arg1; // Propagate constant
                    break;
                }
                temp = temp->next;
            }
        }

        if (isVariable(arg2Value)) {
            TAC* temp = *head;
            while (temp != NULL) {
                if (temp->op != NULL && strcmp(temp->op, "=") == 0 && strcmp(temp->result, arg2Value) == 0) {
                    arg2Value = temp->arg1; // Propagate constant
                    break;
                }
                temp = temp->next;
            }
        }

        if (isConstant(arg1Value) && isConstant(arg2Value)) {
            int result = 0;
            int val1 = atoi(arg1Value);
            int val2 = atoi(arg2Value);

            if (strcmp(current->op, "+") == 0) {
                result = val1 + val2;
            } else if (strcmp(current->op, "-") == 0) {
                result = val1 - val2;
            } else if (strcmp(current->op, "*") == 0) {
                result = val1 * val2;
            } else if (strcmp(current->op, "/") == 0 && val2 != 0) {
                result = val1 / val2;
            }

            char resultStr[20];
            sprintf(resultStr, "%d", result);

            if (current->arg1 != NULL) {
                free(current->arg1);
            }

            if (current->arg2 != NULL) {
                free(current->arg2);
            }

            current->arg1 = strdup(resultStr);
            current->op = strdup("=");
            current->arg2 = NULL;
        }

        current = current->next;
    }
}

// Copy propagation optimization
void copyPropagation(TAC** head) 
{
    TAC* current = *head;
    while (current != NULL) {
        if (current->op != NULL && strcmp(current->op, "write") == 0) {
            current = current->next;
            continue;
        }

        if (current->op != NULL && strcmp(current->op, "=") == 0 && isVariable(current->arg1)) {
            TAC* temp = current->next;
            while (temp != NULL) {
                if (temp->arg1 != NULL && strcmp(temp->arg1, current->result) == 0) {
                    free(temp->arg1);
                    temp->arg1 = strdup(current->arg1);
                }
                if (temp->arg2 != NULL && strcmp(temp->arg2, current->result) == 0) {
                    free(temp->arg2);
                    temp->arg2 = strdup(current->arg1);
                }
                temp = temp->next;
            }
        }
        current = current->next;
    }
}

// Dead code elimination optimization
void deadCodeElimination(TAC** head) 
{
    TAC* current = *head;
    TAC* prev = NULL;
    while (current != NULL) {
        if (current->op != NULL && strcmp(current->op, "=") == 0) {
            int isUsed = 0;
            TAC* temp = current->next;
            while (temp != NULL) {
                if ((temp->arg1 != NULL && strcmp(temp->arg1, current->result) == 0) ||
                    (temp->arg2 != NULL && strcmp(temp->arg2, current->result) == 0)) {
                    isUsed = 1;
                    break;
                }
                temp = temp->next;
            }
            if (!isUsed) {
                // Remove the dead code
                if (current == *head) {
                    *head = current->next;
                } else {
                    prev->next = current->next;
                }
                free(current->op);
                free(current->arg1);
                free(current->arg2);
                free(current->result);
                TAC* toDelete = current;
                current = current->next;
                free(toDelete);
                continue; // Skip incrementing current
            }
        }
        prev = current;
        current = current->next;
    }
}

// Print the optimized TAC list
void printOptimizedTAC(const char* filename, TAC* head) 
{
    FILE* outputFile = fopen(filename, "w");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    TAC* current = head;
    while (current != NULL) {
        if (current->op != NULL && strcmp(current->op, "write") == 0) {
            fprintf(outputFile, "write %s\n", current->arg1);
            printf("write %s\n", current->arg1);
        } else {
            if (current->result != NULL)
                fprintf(outputFile, "%s = ", current->result);
            if (current->arg1 != NULL)
                fprintf(outputFile, "%s ", current->arg1);
            if (current->op != NULL && strcmp(current->op, "=") != 0)
                fprintf(outputFile, "%s ", current->op);
            if (current->arg2 != NULL)
                fprintf(outputFile, "%s ", current->arg2);
            fprintf(outputFile, "\n");

            if (current->result != NULL)
                printf("%s = ", current->result);
            if (current->arg1 != NULL)
                printf("%s ", current->arg1);
            if (current->op != NULL && strcmp(current->op, "=") != 0)
                printf("%s ", current->op);
            if (current->arg2 != NULL)
                printf("%s ", current->arg2);
            printf("\n");
        }
        current = current->next;
    }
    fclose(outputFile);
}

// Optimize the TAC with all methods
void optimizeTAC(TAC** head) 
{
    deadCodeElimination(head);
    constantPropagation(head);
    copyPropagation(head);
    constantFolding(head);     
}

// Load TAC from file
void loadTAC(const char* filename, TAC** head) 
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        char result[32], arg1[32], op[4], arg2[32];
        int parsed = sscanf(line, "%s = %s %s %s", result, arg1, op, arg2);
        if (parsed == 2) {
            addTAC(head, createTAC(result, arg1, "=", NULL));
        } else if (parsed == 4) {
            addTAC(head, createTAC(result, arg1, op, arg2));
        } else if (sscanf(line, "write %s", arg1) == 1) {
            addTAC(head, createTAC(NULL, arg1, "write", NULL)); // No result needed for write
        }
    }

    fclose(file);
}

// Main function
void optimizer(const char* filename)
{
    TAC* head = NULL;

    // Load TAC from file
    loadTAC(filename, &head);
    printf("Original TAC:\n");
    printOptimizedTAC("original_tac.txt", head);

    // Optimize TAC
    optimizeTAC(&head);

    printf("\nOptimized TAC:\n");
    printOptimizedTAC("TAC.ir", head); // Overwrite TAC.ir with optimized TAC

    // Free the allocated TAC
    freeTAC(head);
}
