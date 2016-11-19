//#include "main.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <set>

using namespace std;


int linenum = 0;


extern "C" void yyerror (const char *s)
{
	cerr << s << endl;
	cerr << "\tError occured at line: " << linenum+1 << endl;
}


extern "C" char* AddToSymbolTable(char* yytext) 
{	
	static set<string> SymbolTable; // the table of symbols
	string yytextStr(yytext);
	SymbolTable.insert(yytextStr);

	// Discard const qualifier (return char* without warnings)
	const char *symtab_cstring = SymbolTable.find(yytextStr)->c_str();
	char **symtab_cstr_ptr = (char **) &symtab_cstring;

	return *symtab_cstr_ptr;
}


extern "C" int yyparse();


int main(int argc, char *argv[])
{
	yyparse();
}

