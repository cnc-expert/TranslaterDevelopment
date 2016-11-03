//#include "main.h"
//#include "nc.tab.h"
#include <stdio.h>
#include <iostream>
#include <set>

using namespace std;

set<string> SymbolTable; // the table of symbols
int SymbolCount = 0;

extern "C" void yyerror (const char *s)
{
	fputs(s, stderr);
	fputc('\n', stderr);
}

// Old version: PutSymTable()
// This function adds the element to the table of symbols (type "set")

extern "C" const char* AddToSymbolTable(char* yytext) 
{	
	SymbolCount++;
	cout << "Token #" << SymbolCount << " addition in the table of symbols in process. Please, wait..." << endl;
	
	string yytextStr(yytext);
	SymbolTable.insert(yytextStr);
	
	return ( *SymbolTable.find(yytextStr) ).c_str();
}

extern "C" int yyparse();

int main(int argc, char *argv[])
{
	yyparse();
}

