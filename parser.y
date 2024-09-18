%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Symbol_Table.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int lines;

SymbolTable* symbol_table;

void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s);
    fflush(stderr); // Flush stderr to ensure error messages are printed
    exit(1);
}

%}

%union {
    char* string;
}

/* Define token types */
%token <string> KEYWORD
%token <string> TYPE
%token <int> INT
%token <int> FLOAT
%token <string> STRING
%token <char> CHAR
%token <char> ARITHMETIC_OPERATOR
%token <string> LOGIC_OPERATOR
%token <string> ASSIGNMENT_OPERATOR
%token <string> RELATIONAL_OPERATOR
%token <char> SEMICOLON
%token <char> SYMBOL
%token <string> ID

%printer { fprintf(yyoutput, "%s", $$); } ID;

%%

program:
    VarDeclList StmntList
    {
        printf("The PARSER has started\n");
    }
    ;

VarDeclList:{/* empty/do nothing */}
    |
    VarDecl VarDeclList
    {
        //printf("PARSER: Identified VardeclList\n");
    }
    ;

VarDecl:
    TYPE ID SEMICOLON
    {
        printf("PARSER: Recognized variable declaration: %s\n", $2);
        
        if (lookup(symbol_table, $2) == 0)
        {
             Symbol* new_symbol = malloc(sizeof(Symbol));
             new_symbol->type = malloc(strlen($1) + 1); 
             strcpy(new_symbol->type, $1);
             new_symbol->value = 0;

             insert(symbol_table, $2, new_symbol);

             print_table(symbol_table);
        }
        else
        {
            printf("ERROR ON LINE %d: ID %s has already been defined\n", lines, $2);

            exit(0);
        }
        
    }
    |
    TYPE ID
    {
        fprintf(stderr, "PARSER_ERROR: Missing Semicolon at line %d\n", lines);
    }
    |
    TYPE INT SEMICOLON
    {
        fprintf(stderr, "PARSER_ERROR: Invalid Identifier Name %d\n", lines);
    }

StmntList:{/* emoty/do nothing*/}
    |
    Stmnt StmntList
    {
        printf("PARSER: Recognized Statement List\n");
    }

Stmnt:
    ID ASSIGNMENT_OPERATOR Expr SEMICOLON
    {
        printf("PARSER Recognized Assignment Statement\n");
    }
    |
    ID ASSIGNMENT_OPERATOR Expr
    {
        fprintf(stderr, "PARSER_ERROR: Missing Semicolon at line %d\n", lines);
    }

Expr:
    Operand ARITHMETIC_OPERATOR Operand 
    {
        printf("PARSER: Recognized Simple Operation\n");
    }
    |
    Operand ARITHMETIC_OPERATOR Expr
    {
        printf("PARSER: Recognized Complex Operation\n");
    }
    |
    INT
    {
        printf("PARSER: Recognized INT\n");
    }
    |
    ID
    {
        printf("PARSER: Recognized ID\n");
    }


Operand:
    ID
    {
        printf("PARSER: Recognized ID operand\n");
    }
    |
    INT
    {
        printf("PARSER: Recognized INT operand\n");
    }
%%

int main() {
    // Initialize file or input source
    yyin = fopen("testProg.cmm", "r");

    symbol_table = create_table(10);
    if (symbol_table == NULL) {
        perror("Failed to create symbol table");
        fclose(yyin);
        exit(1);
    }
    else{
    printf("Symbol Table Created\n");
    }

    // Start parsing
    if (yyparse() == 0) {
        // Successfully parsed
		printf("Parsing successful!\n");
    } else {
        fprintf(stderr, "Parsing failed\n");
    }

    fclose(yyin);
    return 0;
}
