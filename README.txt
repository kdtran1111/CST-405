Commands to run

flex lexer.l
gcc -o lexer lex.yy.c
./lexer main.c        

This take the source code from the main.c and make a lexer
