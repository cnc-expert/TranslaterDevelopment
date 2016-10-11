%{
#include "nc.tab.h"
%}

%option noyywrap

%x labl
%s dis dis_end

uns_int		[0-9]+
uns_flt		{uns_int}+\.?|{uns_int}*\.{uns_int}+

%%

;.* {
	yylval.r = symtab_put(yytext + 1, typ_comm);
	return COMM;
}

{uns_flt} {
	yylval.num = atod(yytext);
	return NUM;
}

BGT {return BGT;}
BGE {return BGE;}
BLT {return BLT;}
BLE {return BLE;}
BNC {return BNC;}
DLY {return DLY;}
URT {return URT;}
UCG {return UCG;}
MIR {return MIR;}
EPP {return EPP;}
RPT {return RPT;}
ERP {return ERP;}
DIS { BEGIN(dis); return DIS;}

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
	yylval.r = symtab_put(yytext, typ_label);
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

\+ {return OPPLUS;}
\- {return OPMINUS;}
\/  {return OPDIV;}
\* {return OPMULT;}
\= {return OPEQUAL;}
\( {return OPARENT;}
\) {return CPARENT;}

<*>\, {
	return COMMA;
}

SIN {
	return SIN;
}
COS {
	return COS;
}
TAN {
	return TAN;
}
ARC {
	return ARC;
}
ARS {
	return ARS;
}
ART {
	return ART;
}
INT {
	return INT;
}
MOD {
	return MOD;
}
SQR {
	return INT;
}
ABS {
	return ABS;
}

<<EOF>> {
	return PROG_EOF;
}

<dis>[^ \t"]* {
	yylval.r = symtab_put(yytext, typ_msg);
	BEGIN(dis_end);
	return MSG;
}

