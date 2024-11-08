%option noyywrap

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"


#define YY_DECL int yylex()

int words = 0;
int chars = 0;
int lines = 1;

%}

letter      [a-zA-Z]
digit       [0-9]
errorID     {digit}({letter}|{digit})*   
ID          {letter}({letter}|{digit})*          
delim       [ \t\n]
INT      	{digit}+
FLOAT		{digit}+\.{digit}+
ws          {delim}+
STRING      \"([^\"\\]|\\.)*\"
CHAR		\'[a-zA-Z]\'
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

"if"        { words++; chars += strlen(yytext); printf("%s : IF\n", yytext); return IF; }
"else"      { words++; chars += strlen(yytext); printf("%s : ELSE\n", yytext); return ELSE; }
"elif"		{ words++; chars += strlen(yytext); printf("%s : ELIF\n", yytext); return ELIF; }
"for"       { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"while"     { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"do"        { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"continue"  { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"goto"      { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"return"    { words++; chars += strlen(yytext); printf("%s : RETURN\n", yytext); return RETURN; }
"write"     { words++; chars += strlen(yytext); printf("%s : WRITE\n", yytext); return WRITE; }
"arr"		{ words++; chars += strlen(yytext); printf("%s : ARR\n", yytext); return ARR; }
"func"		{ words++; chars += strlen(yytext); printf("%s : FUNC\n", yytext); return FUNC; }
"void"		{ words++; chars += strlen(yytext); printf("%s : VOID\n", yytext); return VOID; }
"struct"	{ words++; chars += strlen(yytext); printf("%s : STRUCT\n", yytext); return STRUCT; }
"case"		{ words++; chars += strlen(yytext); printf("%s : CASE\n", yytext); return CASE;}
"switch"    { words++; chars += strlen(yytext); printf("%s : SWITCH\n", yytext); return SWITCH; }
"break"     { words++; chars += strlen(yytext); printf("%s : BREAK\n", yytext); return BREAK; }
"default"   { words++; chars += strlen(yytext); printf("%s : DEFAULT\n", yytext); return DEFAULT; }

int|char|string|float|bool	{words++; chars += strlen(yytext);
		 printf("%s : TYPE\n", yytext);
		 yylval.string = strdup(yytext);
		 return TYPE;
		}
		

{INT}	{words++; chars += strlen(yytext);
			  printf("%s : INT\n",yytext);
			  yylval.intval = atoi(yytext);
			  return INT;
			}

{FLOAT}	{words++; chars += strlen(yytext);
			  printf("%s : FLOAT\n",yytext);
			  yylval.floatval = atof(yytext);
			  return FLOAT;
			}

{STRING} {// Strip the surrounding quotes
		int len = strlen(yytext);
        char stripped[len - 1]; 
        strncpy(stripped, yytext + 1, len - 2); 
        stripped[len - 2] = '\0'; // Null-terminate the string

        words++; 
        chars += len - 2; // Update character count without the quotes
        printf("%s : STRING\n", stripped); 
		yylval.string = strdup(stripped);
        return STRING;
		 }

{CHAR} {words++; 
		 chars += strlen(yytext);
		 printf("%s : CHAR\n", yytext);
		 return CHAR;
		 }
		 

{errorID}	{words++; chars += strlen(yytext);
		 printf("%s : Error - Invalid Identifier at line %d char %d\n", yytext, lines+1, chars);
		}	

{ID}	{words++; chars += strlen(yytext);
		 printf("%s : ID\n", yytext);
		 yylval.string = strdup(yytext);
		 return ID;
		}

"&&"		{chars += strlen(yytext);printf("%s : AND\n", yytext); return AND;}
"||"		{chars += strlen(yytext); printf("%s : OR\n", yytext); return OR;}
"!"			{chars += strlen(yytext); printf("%s : NOT\n", yytext); return NOT;}

"="         { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return ASSIGNMENT_OPERATOR; }
"+="        { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return ASSIGNMENT_OPERATOR; }
"-="        { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return ASSIGNMENT_OPERATOR; }
"*="        { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return ASSIGNMENT_OPERATOR; }
"/="        { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return ASSIGNMENT_OPERATOR; }

"+"         { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return PLUS; }
"-"         { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return MINUS; }
"*"         { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return MULTIPLICATION;}
"/"         { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return DIVISION; }



"<="        { chars += strlen(yytext); printf("%s : LESS_EQUAL\n", yytext); yylval.string = strdup(yytext); return LESS_EQUAL; }
">="        { chars += strlen(yytext); printf("%s : GREATER_EQUAL\n", yytext); yylval.string = strdup(yytext); return GREATER_EQUAL; }
"=="        { chars += strlen(yytext); printf("%s : EQUAL_EQUAL\n", yytext); yylval.string = strdup(yytext); return EQUAL_EQUAL; }
"!="        { chars += strlen(yytext); printf("%s : NOT_EQUAL\n", yytext); return NOT_EQUAL; }
"<"         { chars += strlen(yytext); printf("%s : LESS_THAN\n", yytext); return LESS_THAN; }
">"         { chars += strlen(yytext); printf("%s : GREATER_THAN\n", yytext); return GREATER_THAN; }


";"         { chars++; printf("%s : SEMICOLON \n", yytext); return SEMICOLON; }
":"         { chars++; printf("%s : COLON\n", yytext); return COLON; }
","         { chars++; printf("%s : COMMA\n", yytext); return COMMA; }
"("         { chars++; printf("%s : LPAR\n", yytext); return LPAR; }
")"         { chars++; printf("%s : RPAR\n", yytext); return RPAR; }
"["         { chars++; printf("%s : LBRACKET\n", yytext); return LBRACKET; }
"]"         { chars++; printf("%s : RBRACKET\n", yytext); return RBRACKET; }
"{"         { chars++; printf("%s : LECURL\n", yytext); return LCURL; }
"}"         { chars++; printf("%s : RCURL\n", yytext); return RCURL; }
"_"			{ chars++; printf("%s : UNDERSCORE\n", yytext); return UNDERSCORE; }
"."			{ chars++; printf("%s : PERIOD\n", yytext); return DOT; }


[\r\n]+	{lines++; chars=0;}
[ \t]	{chars++;}
.		{chars++;
         printf("%s : Unrecognized symbol at line %d char %d\n", yytext,lines,chars);
		}

%%
