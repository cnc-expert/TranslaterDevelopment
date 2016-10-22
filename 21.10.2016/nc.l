%{
#include "nc.tab.h"
#include "err.h"
%}

%option header-file="lex.yy.h"
%option noyywrap

%x comm labl
%s dis dis_msg
%x var_or_num_1 var_or_num_2 expect_labl

uint		[0-9]+
uflt		{uint}\.?|[0-9]*\.{uint}
wsp			[ \t]*
labl		[A-Za-z0-9]{1,6}
eob			\r?\n


%%


^{wsp}; {
	BEGIN(comm);
}
<comm>.*[^\r\n] {
	//TO-DO
	//yylval.str = symtab_insert(yytext);
	BEGIN(INITIAL);
	return COMM;
}

{eob} {
	line_num++;
	return EOB;
}

<INITIAL,var_or_num_1,var_or_num_2>{uflt} {
	switch (YYSTATE) {
	case var_or_num_1:
		BEGIN(var_or_num_2);
		break;
	case var_or_num_2:
		BEGIN(expect_labl);
	}
	//TO-DO
	//yylval.str = symtab_insert(yytext);
	return UNUM;
}

<*>\" {
	switch (YYSTATE) {
	case dis:
		BEGIN(dis_msg);
		break;
	case labl:
	case dis_msg:
		BEGIN(INITIAL);
		break;
	case INITIAL:
	default:
		BEGIN(labl);
	}
}
<labl>{labl} {
	//TO-DO
	//yylval.str = symtab_insert(yytext);
	return LABL;
}

{wsp}   { /* Ignore whitespaces */ }
%       { /* Ignore begin/end of tape */ }
<<EOF>> { return PROG_EOF; }


	/***** Std CNC features *****/

X { return yylval.tok_code = X; }
Y { return yylval.tok_code = Y; }
Z { return yylval.tok_code = Z; }
N { return yylval.tok_code = N; }
G { return yylval.tok_code = G; }
M { return yylval.tok_code = M; }
F { return yylval.tok_code = F; }
I { return yylval.tok_code = I; }
J { return yylval.tok_code = J; }
K { return yylval.tok_code = K; }
R { return yylval.tok_code = R; }
S { return yylval.tok_code = S; }
T { return yylval.tok_code = T; }
r { return yylval.tok_code = r; }
b { return yylval.tok_code = b; }


	/***** Math features *****/

<INITIAL,dis,var_or_num_1,var_or_num_2>E {
	switch (YYSTATE) {
	case dis:
		BEGIN(INITIAL);
	}
	return E;
}
\=    { return OPEQUAL; }
\(    { return OPARENT; }
\)    { return CPARENT; }
<*>\+ { return OPPLUS; }
<*>\- { return OPMINUS; }
\*    { return OPMULT; }
\/    { return OPDIV; }
SIN   { return yylval.tok_code = SIN; }
COS   { return yylval.tok_code = COS; }
TAN   { return yylval.tok_code = TAN; }
ARS   { return yylval.tok_code = ARS; }
ARC   { return yylval.tok_code = ARC; }
ART   { return yylval.tok_code = ART; }
NEG   { return yylval.tok_code = NEG; }
SQR   { return yylval.tok_code = SQR; }
ABS   { return yylval.tok_code = ABS; }
INT   { return yylval.tok_code = INT; }
MOD   { return yylval.tok_code = MOD; }


	/***** 3-letter-code features *****/

RPT { return REP; }
ERP { return ERP; }
URT { return URT; }
MIR { return MIR; }
UCG { return UCG; }
UAO { return UAO; }
DLY { return DLY; }

DIS {
	BEGIN(dis);
	return DIS;
}
<dis_msg>[^"]* {
	//TO-DO
	//yylval.str = symtab_insert(yytext);
	return MSG;
}

EPP {
	BEGIN(expect_labl);
	return EPP;
}

BNC {
	BEGIN(expect_labl);
	return BNC;
}
BGT {
	BEGIN(var_or_num_1);
	return BGT;
}
BGE {
	BEGIN(var_or_num_1);
	return BGE;
}
BLT {
	BEGIN(var_or_num_1);
	return BLT;
}
BLE {
	BEGIN(var_or_num_1);
	return BLE;
}
BEQ {
	BEGIN(var_or_num_1);
	return BEQ;
}
BNE {
	BEGIN(var_or_num_1);
	return BNE;
}

<expect_labl>{labl} {
	//TO-DO
	//yylval.str = symtab_insert(yytext);
	return LABL;
}
<expect_labl>\) {
	BEGIN(INITIAL);
	return CPARENT;
}
<*>, {
	return COMMA;
}


<*>. {
	lexerror();
	return -1;
}

