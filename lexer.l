%option noyywrap

%{
#include <stdio.h>
#include <string.h>

#define YY_DECL int yylex()

int words = 0;
int chars = 0;
int lines = 1;
int lexical_errors = 0;  // New variable to track errors

%}

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
"/*"    {
            int c;
            while((c = input()) != 0) {
                if(c == '*') {
                    if((c = input()) == '/')
                        break;
                    else
                        unput(c);
                }
            }
        }

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

{INT}       { words++; chars += strlen(yytext); printf("%s : INT\n",yytext); }

{FLOAT}     { words++; chars += strlen(yytext); printf("%s : FLOAT\n",yytext); }

{STRING}    { words++; chars += strlen(yytext); printf("%s : STRING\n", yytext); }

{CHAR}      { words++; chars += strlen(yytext); printf("%s : CHAR\n", yytext); }

{errorID}   { words++; chars += strlen(yytext); lexical_errors++;  // Increment error count
              printf("%s : Error - Invalid Identifier at line %d char %d\n", yytext, lines+1, chars);
            }

{ID}        { words++; chars += strlen(yytext); printf("%s : IDENTIFIER\n", yytext); }

{ArithmeticOperator} { chars++; printf("%s : ARITHMETICOPERATOR\n", yytext); }

"&&"        { chars += strlen(yytext); printf("%s : LOGICOPERATOR\n", yytext); }
"||"        { chars += strlen(yytext); printf("%s : LOGICOPERATOR\n", yytext); }
"!"         { chars += strlen(yytext); printf("%s : LOGICOPERATOR\n", yytext); }

"="         { chars += strlen(yytext); printf("%s : ASSIGNMENT OPERATOR\n", yytext); }
"+="        { chars += strlen(yytext); printf("%s : ASSIGNMENT OPERATOR\n", yytext); }
"-="        { chars += strlen(yytext); printf("%s : ASSIGNMENT OPERATOR\n", yytext); }
"*="        { chars += strlen(yytext); printf("%s : ASSIGNMENT OPERATOR\n", yytext); }
"/="        { chars += strlen(yytext); printf("%s : ASSIGNMENT OPERATOR\n", yytext); }

"<="        { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }
">="        { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }
"=="        { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }
"!="        { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }
"<"         { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }
">"         { chars += strlen(yytext); printf("%s : RELATIONAL OPERATOR\n", yytext); }

";"         { chars++; printf("%s : SEMICOLON\n", yytext); }
":"         { chars++; printf("%s : SYMBOL\n", yytext); }
","         { chars++; printf("%s : SYMBOL\n", yytext); }
"("         { chars++; printf("%s : SYMBOL\n", yytext); }
")"         { chars++; printf("%s : SYMBOL\n", yytext); }
"["         { chars++; printf("%s : SYMBOL\n", yytext); }
"]"         { chars++; printf("%s : SYMBOL\n", yytext); }
"{"         { chars++; printf("%s : SYMBOL\n", yytext); }
"}"         { chars++; printf("%s : SYMBOL\n", yytext); }

[\r\n]+     { lines++; chars = 0; }
[ \t]       { chars++; }
.           { chars++; lexical_errors++;  // Increment error count for unrecognized symbols
              printf("%s : Unrecognized symbol at line %d char %d\n", yytext, lines, chars);
            }

%%

int main(int argc, char **argv) {
    #ifdef YYDEBUG
        yydebug = 1;
    #endif

    printf("Compiler started.\n\n");

    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }

    yylex();

    // Check if any lexical errors were encountered
    if (lexical_errors == 0) {
        printf("\nSource code is lexically correct.\n");
    } else {
        printf("\nLexical analysis completed with %d error(s).\n", lexical_errors);
    }

    return 0;
}
