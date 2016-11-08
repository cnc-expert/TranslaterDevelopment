%{
#include "nc.tab.h"
#include "main.h"
%}

%option header-file = "lex.yy.h"
%option noyywrap

%x labl
%s dis dis_end

	/* Regular expressions of numbers */
uns_int		[0-9]+ 
uns_flt		{uns_int}+\.?|{uns_int}*\.{uns_int}+

%%
	/* Commentary token */
;.* {
	yylval.comment = AddToSymbolTable(yytext + 1); /* yytext -> char* type */
	return COMM;
}

	/* Number token */
{uns_flt} {
	yylval.numberOrVariable = AddToSymbolTable(yytext);
	return NUM;
}

	/* NC triliteral function token */
BGT { return BGT; }
BGE { return BGE; }
BLT { return BLT; }
BLE { return BLE; }
BNC { return BNC; }
DLY { return DLY; }
URT { return URT; }
UCG { return UCG; }
MIR { return MIR; }
EPP { return EPP; }
RPT { return RPT; }
ERP { return ERP; }
DIS { BEGIN(dis); return DIS;}

	/* End of block token */
\r?\n+ {	
	return EOB;
}

	/* Space token */
[ \t]+ { }

	/* Identification of lable terminals */
<*>\" {
	switch (YYSTATE) {
		case dis:
			break;
		case labl:	
		case dis_end:
			BEGIN(INITIAL);
			break;
		default:
			BEGIN(labl);
	}
}

	/* Lable token */
<labl>[A-Za-z0-9]{1,6} {    
	yylval.comment = AddToSymbolTable(yytext);   
    return LABL;
}

	/* Axis tokens */
X {
	yylval.tokenAxis = X;
	return X;
}

Y {
	yylval.tokenAxis = Y;
	return Y;
}

Z {
	yylval.tokenAxis = Z;
	return Z;
}

	/* NC single-letter function tokens */
R {
	yylval.tokenSingleLetterFunc = R;
	return R;
}

T {
	yylval.tokenSingleLetterFunc = T;
	return T;
}
 
S {
	yylval.tokenSingleLetterFunc = S;
	return S;
}

G {
	yylval.tokenSingleLetterFunc = G;
	return G;
}

N {
	yylval.tokenSingleLetterFunc = N;
	return N;
}

M {
	yylval.tokenSingleLetterFunc = M;
	return M;
}

F {
	yylval.tokenSingleLetterFunc = F;
	return F;
}

I {
	yylval.tokenSingleLetterFunc = I;
	return I;
}

	/* Variable token */
<INITIAL,dis>E{uns_int} { /* variable --> E15 */
	yylval.numberOrVariable = AddToSymbolTable(yytext);
	return E;
}

	/* Operation block */	
\+ { return OPPLUS; }
\- { return OPMINUS; }
\/ { return OPDIV; }
\* { return OPMULT; }
\= { return OPEQUAL;}
\( { return OPARENT;}
\) { return CPARENT;}

	/* Comma (,) token */
<*>\, {
	return COMMA;
}

	/* Mathematical function tokens */
SIN {
	yylval.tokenCodeMathFunc = SIN;
	return SIN;
}

COS {
	yylval.tokenCodeMathFunc = COS;
	return COS;
}

TAN {
	yylval.tokenCodeMathFunc = TAN;
	return TAN;
}

ARC {
	yylval.tokenCodeMathFunc = ARC;
	return ARC;
}

ARS {
	yylval.tokenCodeMathFunc = ARS;
	return ARS;
}

ART {
	yylval.tokenCodeMathFunc = ART;
	return ART;
}

INT {
	yylval.tokenCodeMathFunc = INT;
	return INT;
}

MOD {
	yylval.tokenCodeMathFunc = MOD;
	return MOD;
}

SQR {
	yylval.tokenCodeMathFunc = SQR;
	return SQR;
}

ABS {
	yylval.tokenCodeMathFunc = ABS;
	return ABS;
}

	/* The end of program - token */
<<EOF>> { 
	return PROG_EOF;
}

<dis>[^ \t"]* {
	yylval.comment = AddToSymbolTable(yytext);
	BEGIN(dis_end);
	return MSG;
}
