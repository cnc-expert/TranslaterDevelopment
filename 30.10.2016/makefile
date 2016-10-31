all: lex yacc compile
	./nctranslator

compile:
	gcc -g lex.yy.c -o AnalyzerLex -c
	gcc -g nc.tab.c -o AnalyzerNc -c
	rm nc.tab.c lex.yy.c
	g++ -g -c main.c -o main
	g++ -g -std=c++11 -c translator.c -o translator
	g++ -g AnalyzerLex AnalyzerNc main translator -o nctranslator 

yacc:
	bison nc.y

lex:
	flex LexAnalis.l

clean:
	rm -f nc.tab.[hc]
	rm -f lex.yy.[hc]
	rm -f nctranslator
	rm -f AnalyzerLex AnalyzerNc main

