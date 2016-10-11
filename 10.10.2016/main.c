#include <stdio.h>
#include "nc.tab.h"


void yyerror (const char *s)
{
	fputs(s, stderr);
	fputc('\n', stderr);
}


int main(int argc, char *argv[])
{
	yyparse();
}

