%{
#include "nc.tab.h"
#include "main.h"
%}

%option header-file = "lex.yy.h"
%option noyywrap

%x labl
%s dis dis_msg
%x var_or_num_1 var_or_num_2 expect_labl


uns_int		[0-9]+ 
uns_flt		{uns_int}\.?|[0-9]*\.{uns_int}
wsp			[ \t]*


%%



	/* Commentary token */
;[^\n\r]+ {
	yylval.comment = AddToSymbolTable(yytext + 1); /* yytext -> char* type */
	return COMM;
}

	/* Number token */
<INITIAL,var_or_num_1,var_or_num_2>{uns_flt} {
	switch (YYSTATE) {
	case var_or_num_1:
		BEGIN(var_or_num_2);
		break;
	case var_or_num_2:
		BEGIN(expect_labl);
	}
	yylval.numberOrVariable = AddToSymbolTable(yytext);
	return NUM;
}


	/* NC triliteral function token */
BGT { BEGIN(var_or_num_1); return BGT; }
BGE { BEGIN(var_or_num_1); return BGE; }
BLT { BEGIN(var_or_num_1); return BLT; }
BLE { BEGIN(var_or_num_1); return BLE; }
BNC { BEGIN(expect_labl); return BNC; }
EPP { BEGIN(expect_labl); return EPP; }
DIS { BEGIN(dis); return DIS;}
DLY { return DLY; }
UAO { return UAO; }
URT { return URT; }
UCG { return UCG; }
MIR { return MIR; }
RPT { return RPT; }
ERP { return ERP; }

<expect_labl>[A-Za-z0-9]{1,6} {
	yylval.comment = AddToSymbolTable(yytext);
	return LABL;
}
<expect_labl>\) {
	BEGIN(INITIAL);
	return CPARENT;
}


	/* End of block token */
\r?\n|\r {
	linenum++;
	return EOB;
}

	/* Space token */
[ \t]+ { }

	/* Identification of lable terminals */
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

	/* Label token */
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
<INITIAL,dis,var_or_num_1,var_or_num_2>E{uns_int} {
	switch (YYSTATE) {
	case var_or_num_1:
		BEGIN(var_or_num_2);
		break;
	case var_or_num_2:
		BEGIN(expect_labl);
		break;
	case dis:
		BEGIN(INITIAL);
	}
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

<dis_msg>[^"]* {
	yylval.comment = AddToSymbolTable(yytext);
	return MSG;
}

