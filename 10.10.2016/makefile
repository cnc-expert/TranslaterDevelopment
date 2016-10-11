all: lex yacc compile
	./nctranslator

compile:
	gcc -g *.c -o nctranslator

yacc:
	bison nc.y

lex:
	flex LexAnalis.l

clean:
	rm -f nc.tab.[hc]
	rm -f lex.yy.[hc]
	rm -f nctranslator

