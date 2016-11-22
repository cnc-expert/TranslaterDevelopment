OUTF=nc2fanuc

all: bin/${OUTF}
	bin/${OUTF}

# Tests
t1: bin/${OUTF}
	bin/${OUTF} < tests/t1
t2: bin/${OUTF}
	bin/${OUTF} < tests/t2
t3: bin/${OUTF}
	bin/${OUTF} < tests/t3
t4: bin/${OUTF}
	bin/${OUTF} < tests/t4
t5: bin/${OUTF}
	bin/${OUTF} < tests/t5
t6: bin/${OUTF}
	bin/${OUTF} < tests/t6
t7: bin/${OUTF}
	bin/${OUTF} < tests/t7
t8: bin/${OUTF}
	bin/${OUTF} < tests/t8
t8_full: bin/${OUTF}
	bin/${OUTF} < tests/t8_full

bin/${OUTF}: src/lex.yy.c src/parser.tab.c bin/lexer.o bin/parser.o bin/main.o bin/translator.o
	g++ -g bin/*.o -o bin/${OUTF} 

bin/translator.o: src/translator.cpp bin/translate_epp.o bin/fanuc_vars.o
	g++ -g -std=c++11 -c src/translator.cpp -o bin/translator.o

bin/translate_epp.o: src/translate_epp.cpp bin
	g++ -g -std=c++11 -c src/translate_epp.cpp -o bin/translate_epp.o

bin/fanuc_vars.o: src/fanuc_vars.cpp bin
	g++ -g -std=c++11 -c src/fanuc_vars.cpp -o bin/fanuc_vars.o

bin/main.o: src/main.cpp bin
	g++ -g -c src/main.cpp -o bin/main.o

bin/parser.o: src/parser.tab.c bin
	gcc -g -c src/parser.tab.c -o bin/parser.o

bin/lexer.o: src/lex.yy.c bin
	gcc -g -c src/lex.yy.c -o bin/lexer.o

bin:
	mkdir -p bin

src/parser.tab.c: src/parser.y
	bison src/*.y
	mv parser.tab.[hc] src/

src/lex.yy.c: src/lexer.l
	flex src/*.l
	mv lex.yy.[hc] src/

clean:
	rm -f bin/${OUTF}
	rm -f bin/*.o
	rm -f src/parser.tab.[hc]
	rm -f src/lex.yy.[hc]

