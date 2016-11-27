OUTF = nc2fanuc
CC = gcc
CPP = g++
CFLAGS = -g -c
CPPFLAGS = -g -c -std=c++11
tmp := $(shell mkdir -p bin)



all: bin/${OUTF}
	$<

bin/${OUTF}: bin/lexer.o bin/parser.o bin/main.o bin/translator.o bin/translate_epp.o bin/fanuc_vars.o
	${CPP} -g $^ -o $@

bin/%.o: src/%.cpp
	${CPP} ${CPPFLAGS} $^ -o $@



# Lexer and parser.
bin/parser.o: src/parser.tab.c
	${CC} ${CFLAGS} $^ -o $@
bin/lexer.o: src/lex.yy.c
	${CC} ${CFLAGS} $^ -o $@
src/parser.tab.h  src/parser.tab.c: src/parser.y
	bison $^
	mv parser.tab.[hc] src/
src/lex.yy.h  src/lex.yy.c: src/lexer.l src/parser.tab.h
	flex $<
	mv lex.yy.[hc] src/



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


clean:
	rm -fr bin
	rm -f src/parser.tab.[hc]
	rm -f src/lex.yy.[hc]

