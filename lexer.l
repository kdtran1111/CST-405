%option noyywrap

%{
#include <stdio.h>
#include <string.h>

// Macro to declare the lexer function
#define YY_DECL int yylex()

// Global counters for words, characters, lines, and lexical errors
int words = 0;
int chars = 0;
int lines = 1;
int lexical_errors = 0;  // New variable to track errors

%}

// Define patterns for different tokens
letter      [a-zA-Z]
digit       [0-9]
errorID     {digit}({letter}|{digit})*   
ID          {letter}({letter}|{digit})*          
delim       [ \t\n]
INT         {digit}+
FLOAT       {digit}+\.{digit}+
ws          {delim}+
STRING      \"([^\"\\]|\\.)*\"
CHAR        \'[a-zA-Z]\'
ArithmeticOperator    [-+*/]

%%

// Handle multi-line comments
"/*"    {
            int c;
            while((c = input()) != 0) {  // Read until end of input or end of comment
                if(c == '*') {  // Check for end of comment
                    if((c = input()) == '/')
                        break;  // End of comment
                    else
                        unput(c);  // Put back the character for further processing
                }
            }
        }

// Keywords and types
"if"        { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"else"      { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"switch"    { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"case"      { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"default"   { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"for"       { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"while"     { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"do"        { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"break"     { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"continue"  { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"goto"      { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"return"    { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }
"write"     { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); }

int|char|string|float|bool  { words++; chars += strlen(yytext); printf("%s : TYPE\n", yytext); }

// Numeric and string literals
{INT}       { words++; chars += strlen(yytext); printf("%s : INT\n",yytext); }
{FLOAT}     { words++; chars += strlen(yytext); printf("%s : FLOAT\n",yytext); }
{STRING}    { words++; chars += strlen(yytext); printf("%s : STRING\n", yytext); }
{CHAR}      { words++; chars += strlen(yytext); printf("%s : CHAR\n", yytext); }

// Error handling for invalid identifiers
{errorID}   { words++; chars += strlen(yytext); lexical_errors++;  // Increment error count
              printf("%s : Error - Invalid Identifier at line %d char %d\n", yytext, lines+1, chars);
            }

// Valid identifiers
{ID}        { words++; chars += strlen(yytext); printf("%s : IDENTIFIER\n", yytext); }

// Arithmetic operators
{ArithmeticOperator} { chars++; printf("%s : ARITHMETICOPERATOR\n", yytext); }

// Logical operators
"&&"        { chars += strlen(yytext); printf("%s : LOGICOPERATOR\n", yytext); }
"||"        { chars += strlen(yytext); printf("%s : LOGICOPERATOR\n", yytext); }
"!"         { chars += strlen(yytext); printf("%s : LOGICOPERATOR\n", yytext); }

// Assignment operators
"="         { chars += strlen(yytext); printf("%s : ASSIGNMENT OPERATOR\n", yytext); }
"+="        { chars += strlen(yytext); printf("%s : ASSIGNMENT OPERATOR\n", yytext); }
"-="        { chars += strlen(yytext); printf("%s : ASSIGNMENT OPERATOR\n", yytext); }
"*="        { chars += strlen(yytext); printf("%s : ASSIGNMENT OPERATOR\n", yytext); }
"/="        { chars += strlen(yytext); printf("%s : ASSIGNMENT OPERATOR\n", yytext); }

// Relational operators
"<="        { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }
">="        { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }
"=="        { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }
"!="        { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }
"<"         { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }
">"         { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }

// Symbols and delimiters
";"         { chars++; printf("%s : SEMICOLON\n", yytext); }
":"         { chars++; printf("%s : SYMBOL\n", yytext); }
","         { chars++; printf("%s : SYMBOL\n", yytext); }
"("         { chars++; printf("%s : SYMBOL\n", yytext); }
")"         { chars++; printf("%s : SYMBOL\n", yytext); }
"["         { chars++; printf("%s : SYMBOL\n", yytext); }
"]"         { chars++; printf("%s : SYMBOL\n", yytext); }
"{"         { chars++; printf("%s : SYMBOL\n", yytext); }
"}"         { chars++; printf("%s : SYMBOL\n", yytext); }

// Count line breaks and reset character count
[\r\n]+     { lines++; chars = 0; }

// Count spaces and tabs as characters
[ \t]       { chars++; }

// Catch-all for unrecognized symbols
.           { chars++; lexical_errors++;  // Increment error count for unrecognized symbols
              printf("%s : Unrecognized symbol at line %d char %d\n", yytext, lines, chars);
            }

%%

int main(int argc, char **argv) {
    #ifdef YYDEBUG
        yydebug = 1;  // Enable debugging if defined
    #endif

    printf("Compiler started.\n\n");

    // Open the input file if provided as an argument
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);  // Print error if file cannot be opened
            return 1;
        }
    }

    yylex();  // Start lexical analysis

    // Report lexical errors, if any
    if (lexical_errors == 0) {
        printf("\nSource code is lexically correct.\n");
    } else {
        printf("\nLexical analysis completed with %d error(s).\n", lexical_errors);
    }

    return 0;
}
