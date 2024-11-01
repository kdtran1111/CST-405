# Target to build the final executable
all: parser

# Build parser.tab.c and parser.tab.h from parser.y
parser.tab.c parser.tab.h: parser.y
	bison -t -v -d parser.y

# Build lex.yy.c from lexer.l and ensure it depends on parser.tab.h
lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

# Build the parser executable and link all necessary object files
parser: lex.yy.c parser.tab.c parser.tab.h Symbol_Table.o AST.o semantic.o optimizer.o codeGenerator.o
	gcc -g -o parser parser.tab.c lex.yy.c Symbol_Table.o AST.o semantic.o optimizer.o codeGenerator.o

# Compile Symbol_Table.c to Symbol_Table.o
Symbol_Table.o: Symbol_Table.c
	gcc -g -c -o Symbol_Table.o Symbol_Table.c

# Compile AST.c to AST.o
AST.o: AST.c
	gcc -g -c -o AST.o AST.c

# Compile semantic.c to semantic.o
semantic.o: semantic.c
	gcc -g -c -o semantic.o semantic.c

# Compile optimizer.c to optimizer.o
optimizer.o: optimizer.c
	gcc -g -c -o optimizer.o optimizer.c

# Compile codeGenerator.c to codeGenerator.o
codeGenerator.o: codeGenerator.c
	gcc -g -c -o codeGenerator.o codeGenerator.c

# Clean up generated files
clean:
	rm -f parser parser.tab.c lex.yy.c parser.tab.h Symbol_Table.o AST.o semantic.o optimizer.o codeGenerator.o parser.output lex.yy.o mini_parser1.tab.o
	ls -l

# Phony targets
.PHONY: all clean
