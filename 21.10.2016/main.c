//#include "main.h"
//#include "nc.tab.h"
#include <stdio.h>
#include <iostream>
#include <set>

using namespace std;

set<string> SymTable;

extern "C" void yyerror (const char *s)
{
	fputs(s, stderr);
	fputc('\n', stderr);
}

extern "C" const char* PutSymTable(char* yytext) {
	
	string yytextStr(yytext);
	SymTable.insert(yytextStr); 
	return ( *SymTable.find(yytextStr) ).c_str();
}

extern "C" int yyparse();
int main(int argc, char *argv[])
{
	yyparse();
}

