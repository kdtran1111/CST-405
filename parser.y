%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int yyparse();
extern FILE* yyin;

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

%%

program:
    statement
    {
        printf("The PARSER has started\n");
    }
    ;

statement:
    ID
    {
        printf("PARSER: Recognized ID\n"); // Add newline for better output formatting
    }
    ;

%%

int main() {
    // Initialize file or input source
    yyin = fopen("testProg.cmm", "r");

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
