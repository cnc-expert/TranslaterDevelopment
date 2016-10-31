%{
#include "nc.tab.h"
%}

uns_int		[0-9]+
uns_flt		{uns_int}+\.?|{uns_int}*\.{uns_int}+
coord		[+-]?{uns_flt}

%%
;.* {
	printf ("%s", "Comment line ");
	/*return COMM;*/
}

{coord} {

	return NUM;
}

E{uns_int} {
	return E;
}

G {
	return G;
}

^[ \t]*\([ \t]*[A-Z]{3}[^\)]*\) printf ("3-Function");
^\r?\n+ printf ("Empty line ");

N {
	return N;
}

M {
	return M;
}

F printf ("F-code ");
\"[A-Za-z0-9]{1,6}\" printf ("Mark ");
[ \t]+ { } 
[XYZ] printf("Axis ");
R printf ("Radius ");
S printf ("Speed ");
T printf ("Instrument ");
[\+\-/\*] printf ("Operators ");
[\=] printf ("Equal sign ");
[\(] printf ("Open bracket ");
[\)] printf ("Close bracket ");

\, {
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

ARS|ARC|ART|INT|MOD {
	
}
SQR printf ("Root ");
ABS printf ("Modulus ");
INT {
}
MOD {
}


DIS {
	return DIS;
}


<<EOF>> {
	return PROG_EOF;
}

