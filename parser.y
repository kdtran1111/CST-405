%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Symbol_Table.h"
#include "AST.h"
#include "semantic.h"
#include "optimizer.h"
#include "codeGenerator.h"
extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int lines;
extern TAC* tacHead;  //Declared the head of the Linkedlist of TAC entries
OuterSymbolTable* outer_table; //name of the big table
char* current_scope = "global";
SymbolTable* current_table;
ASTNode* root;

void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n at line %s\n", s, lines);
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
%token <string> ARR
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
%token <char> LBRACKET
%token <char> LPAR
%token <char> RPAR
%token <string> RETURN
%token <char> COMMA
%token <char> RBRACKET
%token <char> LCURL
%token <char> RCURL
%token <string> VOID
%token <string> FUNC
%token <string> ID
%token <string> WRITE
%printer { fprintf(yyoutput, "%s", $$); } ID;
%type <ast> program VarDeclList StmntList VarDecl Stmnt Expr FuncDeclList FuncDecl ParamList ReturnStmnt Param ParamListNonEmpty
%%

program:
    VarDeclList FuncDeclList StmntList
    {
        printf("The PARSER has started\n");
        root = malloc(sizeof(ASTNode));
		root->type = NodeType_program;
		root->program.VarDeclList = $1;
        root->program.FuncDeclList = $2;
		root->program.StmntList = $3;
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
    ARR TYPE ID LBRACKET INT RBRACKET SEMICOLON //Array
    {
        printf("PARSER: Recognized %s array declaration: %s\n", $2, $3);

        if (lookup_symbol(get_symbol_table(outer_table, current_scope), $2) == 0)
        {
            
            if (strcmp($2, "int") == 0)
            {
                insert_int_arr_symbol(get_symbol_table(outer_table, current_scope), $3, $5);
            }

            else if (strcmp($2, "float") == 0)
            {
                insert_int_arr_symbol(get_symbol_table(outer_table, current_scope), $3, $5);
            }
            
            else if (strcmp($2, "string") == 0)
            {
                insert_int_arr_symbol(get_symbol_table(outer_table, current_scope), $3, $5);
            }

            
        }
        else
        {
            printf("ERROR ON LINE %d: ID %s has already been defined\n", lines, $2);

            exit(0);
        }

    }
    |
    TYPE ID SEMICOLON
    {
        printf("PARSER: Recognized variable declaration: %s\n", $1);

        if (lookup_symbol(get_symbol_table(outer_table, current_scope), $2) == 0)
        {
            
            if (strcmp($1, "int") == 0)
            {
                insert_int_symbol(get_symbol_table(outer_table, current_scope), $2, 0);
            }

            else if (strcmp($1, "float") == 0)
            {
                insert_float_symbol(get_symbol_table(outer_table, current_scope), $2, 0.0);
            }
            
            else if (strcmp($1, "string") == 0)
            {
                insert_string_symbol(get_symbol_table(outer_table, current_scope), $2, "NULL");
            }

             $$ = malloc(sizeof(ASTNode));
			 $$->type = NodeType_VarDecl;
			 $$->VarDecl.type = strdup($1);
			 $$->VarDecl.id = strdup($2);

             print_table(outer_table);
            
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

FuncDeclList: {/* empty/do noting*/}
    |
    FuncDecl FuncDeclList
    {
        $$ = malloc(sizeof(ASTNode));
		$$->type = NodeType_FuncDeclList;
		$$->FuncDeclList.FuncDecl = $1;
		$$->FuncDeclList.FuncDeclList = $2;
    }

FuncDecl:
    FUNC TYPE ID {insert_scope(outer_table, $3, 10, $2); current_scope = $3;} LPAR ParamList RPAR LCURL VarDeclList StmntList ReturnStmnt RCURL
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_FuncDecl;
        $$->FuncDecl.id = $3;
        $$->FuncDecl.type = $2;
        $$->FuncDecl.ParamList = $6;
        $$->FuncDecl.VarDeclList = $9;
        $$->FuncDecl.StmntList = $10;
        $$->FuncDecl.ReturnStmnt = $11;

        current_scope = "global" ; 
        printf("PARSER: recognized function declaration");
    }  
    |
    FUNC VOID ID {insert_scope(outer_table, $3, 10, "void"); current_scope = $3;} LPAR ParamList RPAR LCURL VarDeclList StmntList ReturnStmnt RCURL
    {   
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_FuncDecl;
        $$->FuncDecl.id = $3;
        $$->FuncDecl.type = strdup("void");
        $$->FuncDecl.ParamList = $6;
        $$->FuncDecl.VarDeclList = $9;
        $$->FuncDecl.StmntList = $10;
        $$->FuncDecl.ReturnStmnt = $11;
       
        current_scope = "global" ; 
        printf("PARSER: recognized function declaration");
    }

ReturnStmnt:
    {
        if (strcmp(get_scope_type(outer_table, current_scope), "void") != 0)
        {

            printf("ERROR: function with return type has no return statement at line %d\n", lines); 
            exit(1);
        }

    }
    |
    RETURN ID SEMICOLON
    {  
        if (strcmp(get_scope_type(outer_table, current_scope), "void") == 0)
        {
            printf("ERROR: Function of Type VOID not expecting return at line %d\n", lines);
            exit(1);
        } 

        else
        {
            $$ = malloc(sizeof(ASTNode));
            $$->type = NodeType_ReturnStmnt;
            $$->ReturnStmnt.id = strdup($2);
            printf("PARSER: Recognized return statement\n");
        }

    }
    |
    RETURN ID
    {
        printf("ERROR: Missing Semicolon at Line %d\n", lines);
        exit(1);
    }

ParamList:
    {
        $$ = malloc(sizeof(ASTNode));
        $$-> type = NodeType_ParamList;
        $$-> ParamList.Param = NULL;
        $$-> ParamList.ParamList = NULL; 
    }
    |
    Param
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_ParamList;
		$$->ParamList.Param = $1;
		$$->ParamList.ParamList = NULL;

    }
    |
    Param COMMA ParamListNonEmpty
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_ParamList;
        $$->ParamList.Param = $1;
        $$->ParamList.ParamList = $3;
    }

ParamListNonEmpty:
    Param
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_ParamList;
		$$->ParamList.Param = $1;
		$$->ParamList.ParamList = NULL;
    }
    |
    Param COMMA ParamListNonEmpty
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_ParamList;
        $$->ParamList.Param = $1;
        $$->ParamList.ParamList = $3;
    }

Param:

    TYPE ID
    {
        printf("PARSER: Recognized variable declaration: %s\n", $1);

        if (lookup_symbol(get_symbol_table(outer_table, current_scope), $2) == 0)
        {
            
            if (strcmp($1, "int") == 0)
            {
                insert_int_symbol(get_symbol_table(outer_table, current_scope), $2, 0);
            }

            else if (strcmp($1, "float") == 0)
            {
                insert_float_symbol(get_symbol_table(outer_table, current_scope), $2, 0.0);
            }
            
            else if (strcmp($1, "string") == 0)
            {
                insert_string_symbol(get_symbol_table(outer_table, current_scope), $2, "NULL");
            }


             print_table(outer_table);
            
        }
        else
        {
            printf("ERROR ON LINE %d: ID %s has already been defined\n", lines, $2);

            exit(0);
        }

        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_Param;
        $$->Param.type = $1;
        $$->Param.id = $2;
        
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
        fprintf(stderr, "PARSER_ERROR: Missing Semicolon at line %d\n", lines);
    }
    |
    WRITE ID SEMICOLON {
        
        printf("PARSER Recognized Write Statement\n");
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_WriteStmnt;
        $$->WriteStmnt.id = strdup($2);
    }
    |
    ID LBRACKET INT RBRACKET ASSIGNMENT_OPERATOR Expr SEMICOLON
    {
        if (lookup_symbol(get_symbol_table(outer_table, current_scope), $1) == 1)
        {
            printf("PARSER: Recognized array index assignment\n");
        }

        else
        {
            printf("ERROR: Used undeclared variable at line %d\n", lines);
            exit(1);
        }
    }
    |
    ID LBRACKET RBRACKET ASSIGNMENT_OPERATOR LCURL ValueList RCURL SEMICOLON
    {

        if (lookup_symbol(get_symbol_table(outer_table, current_scope), $1) == 1)
        {
            printf("PARSER: Recognized array assignment\n");
        }

        else
        {
            printf("ERROR: Used undeclared variable at line %d\n", lines);
            exit(1);
        }
    }
    |
    ID LPAR ValueList RPAR SEMICOLON
    {
        if (lookup_scope(outer_table, $1) == 1)
        {
            printf("PARSER: Recognized function call\n");
        }

        else
        {
            printf("ERROR: Call to undefined function at line %d\n", lines);
            exit(1);
        }
    }
    |
    ID ASSIGNMENT_OPERATOR ID LPAR ValueList RPAR SEMICOLON
    {
        if (lookup_symbol(get_symbol_table(outer_table, current_scope), $1) == 0)
        {
            printf("ERROR: Used undeclared variable at line %d\n", lines);
            exit(1);
        }

        else if (lookup_scope(outer_table, $3) == 0)
        {
            printf("ERROR: Call to undefined function at line %d\n", lines);
            exit(1);
        }

        else
        {
            printf("PARSER: Recognized variable assignment to function call\n");
        }
    }
    |
    ID LBRACKET INT RBRACKET ASSIGNMENT_OPERATOR ID LPAR ValueList RPAR SEMICOLON
    {
        if (lookup_symbol(get_symbol_table(outer_table, current_scope), $1) == 0)
        {
            printf("ERROR: Used undeclared variable at line %d\n", lines);
            exit(1);
        }

        else if (lookup_scope(outer_table, $6) == 0)
        {
            printf("ERROR: Call to undefined function at line %d\n", lines);
            exit(1);
        }

        else
        {
            printf("PARSER: Recognized array index assignment to function call");
        }

    }
    |
    ID ASSIGNMENT_OPERATOR LPAR TYPE RPAR Val SEMICOLON
    {
        printf("PARSER: Recognized type cast to %s\n", $4);
    }


ValueList:
    |
    Val
    |
    Val COMMA ValueListNonEmpty

ValueListNonEmpty:
    Val
    |
    Val COMMA ValueListNonEmpty

Val:
    ID
    |
    INT


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
        
        print_table(outer_table);
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
    outer_table = create_outer_table(10);
    def_outer_table_semantic(outer_table);
    insert_scope(outer_table, current_scope, 10, "void");
    current_table = malloc(sizeof(SymbolTable));
    
    // Initialize file or input source
    yyin = fopen("testProg.cmm", "r");

    if (outer_table == NULL) {
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
		printf("PARSING SUCCESFUL!\n");
    } else {
        fprintf(stderr, "Parsing failed\n");
    }

    print_ast(root, 0);

    //semantic and genrate TAC function called
    printf("---Semantic Analysis---\n");
    semanticAnalysis(root,outer_table);
    print_table(outer_table);
    printf("Writing TAC into TAC.ir\n");
    printTACToFile("TAC.ir", tacHead);
    if (tacHead == NULL) {
    printf("Error: TAC head is NULL. No instructions to write.\n");
    }else {

    printf("Writing TAC into TAC.ir successful\n");
    }

    //Freeing the tree
    fclose(yyin);
    //print_table(outer_table);

    
    return 0;
}
