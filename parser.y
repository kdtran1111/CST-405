%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Symbol_Table.h"
#include "AST.h"
#include "semantic.h"
extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int lines;
extern TAC* tacHead;  //Declared the head of the Linkedlist of TAC entries
SymbolTable* symbol_table;
ASTNode* root;

void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s);
    fflush(stderr); // Flush stderr to ensure error messages are printed
    exit(1);
}

%}

%union {
    char* string;
    struct ASTNode* ast;
    int intval;
}

/* Define token types */
%token <string> KEYWORD
%token <string> TYPE
%token <intval> INT
%token <int> FLOAT
%token <string> STRING
%token <char> CHAR
%token <string> ARITHMETIC_OPERATOR
%token <string> LOGIC_OPERATOR
%token <string> ASSIGNMENT_OPERATOR
%token <string> RELATIONAL_OPERATOR
%token <char> SEMICOLON
%token <char> SYMBOL
%token <string> ID
%token <string> WRITE
%printer { fprintf(yyoutput, "%s", $$); } ID;
%type <ast> program VarDeclList StmntList VarDecl Stmnt Expr
%%

program:
    VarDeclList StmntList
    {
        printf("The PARSER has started\n");
        root = malloc(sizeof(ASTNode));
		root->type = NodeType_program;
		root->program.VarDeclList = $1;
		root->program.StmntList = $2;
    }
     |
    StmntList
    {
        printf("PARSER: Program without variable declarations\n");
        root = malloc(sizeof(ASTNode));
        root->type = NodeType_program;
        root->program.StmntList = $1;
    }
    ;
    ;

VarDeclList:{/* empty/do nothing */}
    |
    VarDecl VarDeclList
    {
        $$ = malloc(sizeof(ASTNode));
		$$->type = NodeType_VarDeclList;
		$$->VarDeclList.VarDecl = $1;
		$$->VarDeclList.VarDeclList = $2;
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

             $$ = malloc(sizeof(ASTNode));
			 $$->type = NodeType_VarDecl;
			 $$->VarDecl.type = strdup($1);
			 $$->VarDecl.id = strdup($2);

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
        $$ = malloc(sizeof(ASTNode));
		$$->type = NodeType_StmntList;
		$$->StmntList.Stmnt = $1;
		$$->StmntList.StmntList = $2;
        printf("PARSER: Recognized Statement List\n");
    }

Stmnt:
    ID ASSIGNMENT_OPERATOR Expr SEMICOLON
    {
        printf("PARSERAOKFNOEIFWEOFINEWO: %s\n", $2);
        $$ = malloc(sizeof(ASTNode));
		$$->type = NodeType_Stmnt;
		$$->Stmnt.id = strdup($1);
		$$->Stmnt.op = strdup($2);
		$$->Stmnt.Expr = $3;
        printf("PARSER Recognized Assignment Statement\n");
        
    }
    |
    ID ASSIGNMENT_OPERATOR Expr
    {
        fprintf(stderr, "PARSER_ERROR: Missing Semicolon at line %d\n", lines)
    }
    |
    WRITE ID SEMICOLON {
        
        printf("PARSER Recognized Write Statement\n");
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_WriteStmnt;
        $$->WriteStmnt.id = strdup($2);
    }
;
Expr: Expr ARITHMETIC_OPERATOR Expr { printf("PARSER: Recognized expression\n");
                        
						$$ = malloc(sizeof(ASTNode));
                         if ($$ == NULL) {
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);  // Ensure proper exit if malloc fails
                        }
						$$->type = NodeType_Expr;
						$$->Expr.left = $1;
						$$->Expr.right = $3;
						$$->Expr.op = strdup($2);

                        if ($$->Expr.op == NULL) {
                        fprintf(stderr, "Memory allocation for operator failed\n");
                        exit(1);
                        }
						
						// Set other fields as necessary
					  }
 					
	| ID { printf("ASSIGNMENT statement \n"); 
			$$ = malloc(sizeof(ASTNode));
			$$->type = NodeType_SimpleID;
			$$->SimpleID.id = strdup($1);
			// Set other fields as necessary	
		}
	| INT 
    {
        printf("PARSER: Recognized integer expression: %d\n", $1);
        
        print_table(symbol_table);
        $$ = malloc(sizeof(ASTNode));
        /*if ($$->Expr.op == NULL) {
        fprintf(stderr, "Memory allocation for variable initialization failed\n");
        exit(1);
        }
        */
        $$->type = NodeType_SimpleExpr;
        $$->SimpleExpr.value = $1;
        
    }
;

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
        print_ast(root, 1);
    } else {
        fprintf(stderr, "Parsing failed\n");
    }
    printf("---Semantic Analysis---\n");
    semanticAnalysis(root,symbol_table);
    print_table(symbol_table);
    printf("Writing TAC into TAC.ir\n");
    printTACToFile("TAC.ir", tacHead);
    if (tacHead == NULL) {
    printf("Error: TAC head is NULL. No instructions to write.\n");
    }else {

    printf("Writing TAC into TAC.ir successful\n");
    }
    //Freeing the tree
    //freeAST(root);
    fclose(yyin);
    

    
    return 0;
}
