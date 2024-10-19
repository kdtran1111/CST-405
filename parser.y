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
OuterSymbolTable* outer_table;
char* current_scope = "global";
SymbolTable* current_table;
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
    float floatval;
}

/* Define token types */
%token <string> KEYWORD
%token <char> UNDERSCORE
%token <string> STRUCT
%token <string> TYPE
%token <string> ARR
%token <intval> INT
%token <floatval> FLOAT
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
%token <char> PLUS
%token <char> MINUS
%token <char> MULTIPLICATION
%token <char> DIVISION 
/* Define order of operations */
%left PLUS MINUS
%left MULTIPLICATION DIVISION
%nonassoc LPAR RPAR

%printer { fprintf(yyoutput, "%s", $$); } ID;
%type <ast> program VarDeclList StmntList VarDecl Stmnt Expr FuncDeclList FuncDecl ParamList ReturnStmnt Param ParamListNonEmpty ValueList ValueListNonEmpty Val ArrAssignemnt StructDeclList StructDecl
%%

program:
    StructDeclList VarDeclList FuncDeclList StmntList
    {
        printf("The PARSER has started\n");
        root = malloc(sizeof(ASTNode));
		root->type = NodeType_program;
        root->program.StructDeclList = $1;
		root->program.VarDeclList = $2;
        root->program.FuncDeclList = $3;
		root->program.StmntList = $4;
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

StructDeclList:
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_StructDeclList;
        $$->StructDeclList.StructDecl = NULL;
        $$->StructDeclList.StructDeclList = NULL;
    }
    |
    StructDecl StructDeclList
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_StructDeclList;
        $$->StructDeclList.StructDecl = $1;
        $$->StructDeclList.StructDeclList = $2;
    }

StructDecl:
    STRUCT ID {insert_struct(outer_table, $2, 5); current_scope = $2;} LCURL VarDeclList RCURL
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_StructDecl;
        $$->StructDecl.id = strdup($2);
        $$->StructDecl.VarDeclList = $5;
        current_scope = "global";
        printf("PARSER: recognized struct declaration\n");
    }

VarDeclList:
    {$$ = malloc(sizeof(ASTNode));
		$$->type = NodeType_VarDeclList;
		$$->VarDeclList.VarDecl = NULL;
		$$->VarDeclList.VarDeclList = NULL;
    }
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
    ARR TYPE ID LBRACKET INT RBRACKET SEMICOLON
    {
        printf("PARSER: Recognized %s array declaration: %s\n", $2, $3);

        if (lookup_symbol(get_symbol_table(outer_table, current_scope), $2) == 0)
        {
            $$ = malloc(sizeof(ASTNode));
			$$->type = NodeType_VarDecl;
			$$->VarDecl.id = strdup($2);
            $$->VarDecl.size = $5;
            print_table(outer_table);

            if (strcmp($2, "int") == 0)
            {
                insert_int_arr_symbol(get_symbol_table(outer_table, current_scope), $3, $5);
                $$->VarDecl.type = strdup("int_arr");
            }

            else if (strcmp($2, "float") == 0)
            {
                insert_int_arr_symbol(get_symbol_table(outer_table, current_scope), $3, $5);
                $$->VarDecl.type = strdup("float_arr");
            }
            
            else if (strcmp($2, "string") == 0)
            {
                insert_int_arr_symbol(get_symbol_table(outer_table, current_scope), $3, $5);
                $$->VarDecl.type = strdup("string_arr");
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
            $$->VarDecl.size = 0;
            print_table(outer_table);
            
        }
        else
        {
            printf("ERROR ON LINE %d: ID %s has already been defined\n", lines, $2);

            exit(0);
        }
    }
    |
    UNDERSCORE ID ID SEMICOLON
    {
        if (lookup_scope(outer_table, $2) == 0)
        {
            printf("ERROR: Cannot find namespace %s\n", $2);
            exit(1);
        }

        else if (strcmp(get_scope_type(outer_table, $2), "STRUCT") != 0)
        {
            printf("ERROR: %s is not of type struct", $2);
            exit(1);
        }

        else if (lookup_symbol(get_symbol_table(outer_table, current_scope), $3) == 1)
        {
            printf("ERROR: variable %s has already been declared", $3);
            exit(1);
        }

        else
        {
            $$ = malloc(sizeof(ASTNode));
            $$->type = NodeType_VarDecl;
            $$->VarDecl.type = strdup($2);
			$$->VarDecl.id = strdup($3);
            $$->VarDecl.size = 0;

            insert_struct_symbol(get_symbol_table(outer_table, current_scope), $3, get_symbol_table(outer_table, $2), $2);
            printf("PARSER: recognized struct creation\n");
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

FuncDeclList: 
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_FuncDeclList;
		$$->FuncDeclList.FuncDecl = NULL;
		$$->FuncDeclList.FuncDeclList = NULL;
    }
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
            $$ = malloc (sizeof(ASTNode));
            $$->type = NodeType_IndexAssignment;
            $$->IndexAssignment.id = $1;
            $$->IndexAssignment.index = $3;
            $$->IndexAssignment.Expr = $6;
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
            $$ = malloc (sizeof(ASTNode));
            $$->type = NodeType_ArrAssignment;
            $$->ArrAssignment.id = strdup($1);
            $$->ArrAssignment.ValueList = $6;

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
    Val
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_ValueList;
        $$->ValueList.Val = $1;
        $$->ValueList.ValueList = NULL;
    }
    |
    Val COMMA ValueListNonEmpty
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_ValueList;
        $$->ValueList.Val = $1;
        $$->ValueList.ValueList = $3;
    }

ValueListNonEmpty:
    Val
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_ValueList;
        $$->ValueList.Val = $1;
        $$->ValueList.ValueList = NULL;
    }
    |
    Val COMMA ValueListNonEmpty
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_ValueList;
        $$->ValueList.Val = $1;
        $$->ValueList.ValueList = $3;
    }

Val:
    ID
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_SimpleID;
        $$->SimpleID.id = strdup($1);
    }
    |
    INT
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_SimpleExpr;
        $$->SimpleExpr.value = $1;
    }
    |
    FLOAT
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_SimpleFloat;
        $$->SimpleFloat.value = $1;
    }
    |
    STRING
    {
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_SimpleString;
        $$->SimpleString.value = strdup($1);
    }


Expr: Expr ARITHMETIC_OPERATOR Expr Expr:
    Expr PLUS Expr {
        printf("PARSER: Recognized addition\n");
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        $$->type = NodeType_Expr;
        $$->Expr.left = $1;
        $$->Expr.right = $3;
        $$->Expr.op = strdup("+");
        if ($$->Expr.op == NULL) {
            fprintf(stderr, "Memory allocation for operator failed\n");
            exit(1);
        }
    }
    | Expr MINUS Expr {
        printf("PARSER: Recognized subtraction\n");
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        $$->type = NodeType_Expr;
        $$->Expr.left = $1;
        $$->Expr.right = $3;
        $$->Expr.op = strdup("-");
        if ($$->Expr.op == NULL) {
            fprintf(stderr, "Memory allocation for operator failed\n");
            exit(1);
        }
    }
    | Expr MULTIPLICATION Expr {
        printf("PARSER: Recognized multiplication\n");
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        $$->type = NodeType_Expr;
        $$->Expr.left = $1;
        $$->Expr.right = $3;
        $$->Expr.op = strdup("*");
        if ($$->Expr.op == NULL) {
            fprintf(stderr, "Memory allocation for operator failed\n");
            exit(1);
        }
    }
    | Expr DIVISION Expr {
        printf("PARSER: Recognized division\n");
        $$ = malloc(sizeof(ASTNode));
        if ($$ == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        $$->type = NodeType_Expr;
        $$->Expr.left = $1;
        $$->Expr.right = $3;
        $$->Expr.op = strdup("/");
        if ($$->Expr.op == NULL) {
            fprintf(stderr, "Memory allocation for operator failed\n");
            exit(1);
        }
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
        
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_SimpleExpr;
        $$->SimpleExpr.value = $1;
        
    }
    | LPAR Expr RPAR
    {
        $$ = $2;
    }
    | FLOAT
    {
        printf("PARSER: Recognized float expression: %f\n", $1);
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_SimpleFloat;
        $$->SimpleFloat.value = $1;
    }
    |
    ID LBRACKET INT RBRACKET
    {
        printf("PARSER: Recognized array index expression: %s[$d]\n", $1, $3);
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_SimpleArrIndex;
        $$->SimpleArrIndex.index = $3;
        $$->SimpleArrIndex.id = strdup($1);
    }
;

%%

int main() {
    outer_table = create_outer_table(10);
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
    //Freeing the tree
    fclose(yyin);
    //print_table(outer_table);


    
    return 0;
}
