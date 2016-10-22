%{
#include "nc.tab.h"
#include "main.h"
%}

%option header-file = "lex.yy.h"
%option noyywrap

%x labl
%s dis dis_end

uns_int		[0-9]+
uns_flt		{uns_int}+\.?|{uns_int}*\.{uns_int}+

%%

;.* {
	yylval.comm = PutSymTable(yytext + 1); /* yytext -> char* type */
	return COMM;
}

{uns_flt} {
	yylval.comm = PutSymTable(yytext);
	return NUM;
}

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
DIS { BEGIN(dis);  return DIS;}

\r?\n+ {	
	return EOB;
}

[ \t]+ { }

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

<labl>[A-Za-z0-9]{1,6} {
    
	yylval.comm = PutSymTable(yytext);
    
    return LABL;
}

X {
	return X;
} 
Y {
	return Y;
}
Z {
	return Z;
} 
R {
	return R;
} 
T {
	return T;
} 
S {
	return S;
} 
<INITIAL,dis>E{uns_int} {
yylval.comm=PutSymTable(yytext);
	return E;
}
G {
	return G;
}
N {
	return N;
}
M {
	return M;
}
F {
	return F;
}
I {
	return I;
}

\+ { return OPPLUS; }
\- { return OPMINUS; }
\/  { return OPDIV; }
\* { return OPMULT; }
\= { return OPEQUAL;}
\( { return OPARENT;}
\) { return CPARENT;}

<*>\, {
	return COMMA;
}

SIN {
	yylval.tokencode = SIN;
	return SIN;
}
COS {
	yylval.tokencode = COS;
	return COS;
}
TAN {
	yylval.tokencode = TAN;
	return TAN;
}
ARC {
	yylval.tokencode = ARC;
	return ARC;
}
ARS {
yylval.tokencode = ARS;
	return ARS;
}
ART {
yylval.tokencode = ART;
	return ART;
}
INT {
yylval.tokencode = INT;
	return INT;
}
MOD {
yylval.tokencode = MOD;
	return MOD;
}
SQR {
yylval.tokencode = SQR;
	return SQR;
}
ABS {
yylval.tokencode = ABS;
	return ABS;
}

<<EOF>> {
	return PROG_EOF;
}

<dis>[^ \t"]* {
	yylval.comm = PutSymTable(yytext);
	BEGIN(dis_end);
	return MSG;
}
