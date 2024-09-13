all: parser

parser.tab.c parser.tab.h:	parser.y
	bison -t -v -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

parser: lex.yy.c parser.tab.c parser.tab.h
	gcc -g -o parser parser.tab.c lex.yy.c
	./parser testProg.cmm

clean:
	rm -f parser parser.tab.c lex.yy.c parser.tab.h parser.output lex.yy.o mini_parser1.tab.o
	ls -l
