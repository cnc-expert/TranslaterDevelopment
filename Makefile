OUTF = nc2fanuc
CC = gcc
CPP = g++
CFLAGS = -g -c
CPPFLAGS = -g -c -std=c++11
UNITS = lexer parser main translator fanuc_vars translate_epp translate_rpt

objects = $(foreach unit, ${UNITS}, bin/${unit}.o)
tmp := $(shell mkdir -p bin)



all: bin/${OUTF}
	$<

# Build all the units into one.
bin/${OUTF}: ${objects}
	${CPP} -g $^ -o $@

# All the C++ translation units building.
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
t1 t2 t3 t4 t5 t6 t7 t8 t9: bin/${OUTF}
	bin/${OUTF} < tests/$@



clean:
	rm -fr bin
	rm -f src/parser.tab.[hc]
	rm -f src/lex.yy.[hc]

