OUTF=nc2fanuc

all: bin/${OUTF}
	bin/${OUTF}

t1: bin/${OUTF}
	bin/${OUTF} < tests/t1
t2: bin/${OUTF}
	bin/${OUTF} < tests/t2
t3: bin/${OUTF}
	bin/${OUTF} < tests/t3
t4: bin/${OUTF}
	bin/${OUTF} < tests/t4

bin/${OUTF}: src/lex.yy.c src/nc.tab.c bin/lexer.o bin/parser.o bin/main.o bin/translator.o
	g++ -g bin/*.o -o bin/${OUTF} 

bin/translator.o: src/translator.c bin
	g++ -g -std=c++11 -c src/translator.c -o bin/translator.o

bin/main.o: src/main.c bin
	g++ -g -c src/main.c -o bin/main.o

bin/parser.o: src/nc.tab.c bin
	gcc -g -c src/nc.tab.c -o bin/parser.o

bin/lexer.o: src/lex.yy.c bin
	gcc -g -c src/lex.yy.c -o bin/lexer.o

bin:
	mkdir -p bin

src/nc.tab.c: src/nc.y
	bison src/nc.y
	mv nc.tab.[hc] src/

src/lex.yy.c: src/LexAnalis.l
	flex src/LexAnalis.l
	mv lex.yy.[hc] src/

clean:
	rm -f bin/${OUTF}
	rm -f bin/*.o
	rm -f src/nc.tab.[hc]
	rm -f src/lex.yy.[hc]

