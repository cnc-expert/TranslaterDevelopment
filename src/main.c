//#include "main.h"
//#include "nc.tab.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <set>

using namespace std;

set<string> SymbolTable; // the table of symbols
int SymbolCount = 0;

extern "C" void yyerror (const char *s)
{
	cerr << s << endl;
}


// This function adds the element to the table of symbols (type "set")

extern "C" char* AddToSymbolTable(char* yytext) 
{	
	SymbolCount++;
	//cout << "Token #" << SymbolCount << " addition in the table of symbols in process. Please, wait..." << endl;
	
	char *symbol_string = (char *) malloc(strlen(yytext));
	strcpy(symbol_string, yytext);
	return symbol_string;
	
	//string yytextStr(yytext);
	//SymbolTable.insert(yytextStr);
	//return ( *SymbolTable.find(yytextStr) ).c_str();
}

extern "C" int yyparse();

int main(int argc, char *argv[])
{
	yyparse();
}

