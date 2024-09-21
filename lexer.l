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

"if"        { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"else"      { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"switch"    { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"case"      { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"default"   { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"for"       { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"while"     { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"do"        { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"break"     { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"continue"  { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"goto"      { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"return"    { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }
"write"     { words++; chars += strlen(yytext); printf("%s : KEYWORD\n", yytext); return KEYWORD; }


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
			  return FLOAT;
			}

{STRING} {words++; 
		 chars += strlen(yytext);
		 printf("%s : STRING\n", yytext);
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

{ArithmeticOperator} 	{ chars ++;
		 	printf("%s : ARITHMETICOPERATOR\n", yytext);
			yylval.string = strdup(yytext);
			return ARITHMETIC_OPERATOR;
			}

"&&"		{chars += strlen(yytext);printf("%s : LOGICOPERATOR\n", yytext); return LOGIC_OPERATOR;}
"||"		{chars += strlen(yytext); printf("%s : LOGICOPERATOR\n", yytext); return LOGIC_OPERATOR;}
"!"		{chars += strlen(yytext); printf("%s : LOGICOPERATOR\n", yytext); return LOGIC_OPERATOR;}

"="         { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return ASSIGNMENT_OPERATOR; }
"+="        { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return ASSIGNMENT_OPERATOR; }
"-="        { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return ASSIGNMENT_OPERATOR; }
"*="        { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return ASSIGNMENT_OPERATOR; }
"/="        { chars += strlen(yytext); printf("%s : ASSIGNMENT_OPERATOR\n", yytext); yylval.string = strdup(yytext); return ASSIGNMENT_OPERATOR; }


"<="        { chars += strlen(yytext); printf("%s : RELATIONAL_OPERATOR\n", yytext); return RELATIONAL_OPERATOR; }
">="        { chars += strlen(yytext); printf("%s : RELATIONAL_OPERATOR\n", yytext); return RELATIONAL_OPERATOR; }
"=="        { chars += strlen(yytext); printf("%s : RELATIONAL_OPERATOR\n", yytext); return RELATIONAL_OPERATOR; }
"!="        { chars += strlen(yytext); printf("%s : RELATIONAL_OPERATOR\n", yytext); return RELATIONAL_OPERATOR; }
"<"         { chars += strlen(yytext); printf("%s : RELATIONAL_OPERATOR\n", yytext); return RELATIONAL_OPERATOR; }
">"         { chars += strlen(yytext); printf("%s : RELATIONAL_OPERATOR\n", yytext); return RELATIONAL_OPERATOR; }


";"         { chars++; printf("%s : SEMICOLON \n", yytext); return SEMICOLON; }
":"         { chars++; printf("%s : SYMBOL\n", yytext); return SYMBOL; }
","         { chars++; printf("%s : SYMBOL\n", yytext); return SYMBOL; }
"("         { chars++; printf("%s : SYMBOL\n", yytext); return SYMBOL; }
")"         { chars++; printf("%s : SYMBOL\n", yytext); return SYMBOL; }
"["         { chars++; printf("%s : SYMBOL\n", yytext); return SYMBOL; }
"]"         { chars++; printf("%s : SYMBOL\n", yytext); return SYMBOL; }
"{"         { chars++; printf("%s : SYMBOL\n", yytext); return SYMBOL; }
"}"         { chars++; printf("%s : SYMBOL\n", yytext); return SYMBOL; }


[\r\n]+	{lines++; chars=0;}
[ \t]	{chars++;}
.		{chars++;
         printf("%s : Unrecognized symbol at line %d char %d\n", yytext,lines,chars);
		}

%%


