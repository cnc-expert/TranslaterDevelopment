#include "translator.h"
#include "parser.tab.h"

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <deque>
#include <algorithm>


using namespace std;

extern "C" void* TranslateFunctionWithTwoArguments(int function, void* firstExpression, void* secondExpression) {
	
	string* x1 = (string*)firstExpression; // first expression and then result string
	string* x2 = (string*)secondExpression;
	
	switch (function) {
		case MOD:
			// MOD(x) --> x1 - FIX(x1 / x2)*x2 --> x1 - FIX[ [x1]/[x2] ] * [x2]
			*x1 = *x1 + "-FIX[[" + *x1 + "]/[" + *x2 +"]]*[" + *x2 + "]";
			break;
	}
	delete x2;
	
	return x1;
}

extern "C" void* TranslateFunction(int function,void* expression) {
	
	string* x = (string*)expression;
	
	switch (function) {
		case SIN:
			*x= "SIN[" + *x + "]"; // SIN(x) --> SIN[x]
			break;
		case COS:
			*x= "COS[" + *x + "]"; // COS(x) --> COS[x]
			break;
		case TAN:
			*x= "TAN[" + *x + "]"; // TAN(x) --> TAN[x]
			break;
		case ARS:
			*x= "ATAN[[" + *x + "]/SQRT[1-[" + *x + "]*[" + *x +"]]]"; // ARS(x) --> ATAN[ [x]/SQRT[ 1-[x]*[x] ] ]
			break;
		case ARC:
			*x= "2*ATAN[SQRT[[1-["+ *x + "]]/[1+[" + *x + "]]]]"; // ARC(x) --> 2*ATAN[ SQRT[ [1-[x]] / [1+[x]] ] ]
			break; 
		case ART:
			*x= "ATAN[" + *x + "]"; // ART(x) --> ATAN[x]
			break;
		case INT:
			*x= "FIX[" + *x + "]";  // INT(x) --> FIX[x]
			break;
		case ABS:
			*x= "ABS[" + *x + "]"; // ABS(x) --> ABS[x]
			break;
		case SQR:
			*x= "SQRT[" + *x + "]"; // SQR(x) --> SQRT[x]
			break;
		case NEG:
			*x= "-[" + *x + "]";    // NEG(x) --> -[x]
			break;
	}
	
	return (void*)x;
	
}

extern "C" void* ExecuteNegativeOperation(void* expression) {
	
	string* x = (string*)expression;
	*x = "-" + *x; // x--> -x
	
	return (void*)x;
}

extern "C" void* ExecuteArithmeticOperation(void* leftExpression, char sign, void* rightExpression) {
	
	string* x = (string*)leftExpression;
	string* x1 = (string*)rightExpression;
	*x = *x + sign + *x1; // x --> x sign x (sign: +-*/)
	
	return (void*)x;
}

extern "C" void* PutExpressionInBrackets(void* expression) {
	
	string* x = (string*)expression;
	*x = "[" + *x + "]"; // x --> [x]
	
	return (void*)x;
}

extern "C" void* ConcatWords(void* arg1, void* arg2) {
	
	// pointers correction
	if(((Block*)arg2)->type==TB_G40) ((Block*)arg1)->type=TB_G40;
	*((Block*)arg1)->translatedBlock+= string(" ") + *((Block*)arg2)->translatedBlock;
	
	delete (Block*)arg2;
	return arg1;
}

extern "C" void* ConvertCharToCppString(char* tokenNum) {
	
	return new string(tokenNum);
}

extern "C" void* SubstituteNCvarAsCppString(char* tokenVar) {
	string *fanucVarString = new string("#");
	*fanucVarString += to_string(MatchinFanucVariableToNC(tokenVar));

	return fanucVarString;
}

extern "C" void PrintCppString(void* str) {
	
	cout << "Printing CppString in process. Please, wait..." << endl;
	
	string* x = (string*)str;
	cout << *x << endl;
}

