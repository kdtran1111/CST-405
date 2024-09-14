%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int lines;

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
        printf("PARSER: Recognized Declaration Statement\n");
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
        printf("PARSER: Recognized Operation\n");
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
