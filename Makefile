# Target to build the final executable
all: parser

# Build parser.tab.c and parser.tab.h from parser.y
parser.tab.c parser.tab.h:	parser.y
	bison -t -v -d parser.y

# Build lex.yy.c from lexer.l and ensure it depends on parser.tab.h
lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

# Build the parser executable and link all necessary object files
parser: lex.yy.c parser.tab.c parser.tab.h Symbol_Table.o AST.o
	gcc -g -o parser parser.tab.c lex.yy.c Symbol_Table.o AST.o

# Compile Symbol_Table.c to Symbol_Table.o
Symbol_Table.o: Symbol_Table.c
	gcc -c -o Symbol_Table.o Symbol_Table.c

# Compile AST.c to AST.o
AST.o: AST.c
	gcc -c -o AST.o AST.c

# Clean up generated files
clean:
	rm -f parser parser.tab.c lex.yy.c parser.tab.h Symbol_Table.o AST.o parser.output lex.yy.o mini_parser1.tab.o
	ls -l
