#include "nc.tab.h"
#include <iostream>
#include <string>
#include <map>
#include <set>

using namespace std;

// Fanuc Variable
// #100...199
// #500...999
map<char*, int> VariableTable;

set<int> CreateEmptyVariablesMap() {
	
	set<int> emptyVariablesSet;
	
	for (int i = 100; i <= 199; i++) {
		emptyVariablesSet.insert(i);
	}
	for (int i = 500; i <= 999; i++) {
		emptyVariablesSet.insert(i);
	}
	
	return emptyVariablesSet;
}

set<int> EmptyVariables = CreateEmptyVariablesMap();

extern "C" void PrintCPPstring(void* str) {
	string* x = (string*)str;
	cout << *x << endl << endl;
}

int GetVariableNCIndex(char* variableNC) {
	
	if (VariableTable.find(variableNC) == VariableTable.end() ){
		VariableTable[variableNC] = *EmptyVariables.begin();
		EmptyVariables.erase(EmptyVariables.begin());
	}
	return VariableTable[variableNC];
}

extern "C" void* CreateExpressionBlock(char* variableNC,	void* expression) {
	
	string* x = (string*)expression;
	*x = "#" + to_string(GetVariableNCIndex(variableNC)) + "=" + *x;
	return (void*)x;
}

extern "C" void* MinusOperation(void* expression) {
	string* x = (string*)expression;
	*x = "-" + *x;
	return (void*)x;
}

extern "C" void* BinaryOperation(void* leftExpression, char sign, void* rightExpression) {
	string* x = (string*)leftExpression;
	string* x1 = (string*)rightExpression;
	*x = *x + sign + *x1;
	return (void*)x;
}

extern "C" void* PutInBrackets(void* expression) {
	string* x = (string*)expression;
	*x = "[" + *x + "]";
	return (void*)x;
}

extern "C" void*  CharToCppString(char* tokenNum ) {
	return  new string(tokenNum);
}

extern "C" void* Func(int function,void* expression) {
	
	string* x= (string*)expression; // pointer to String*
	
	switch(function){
		case SIN:
			*x= "SIN[" + *x+ "]";
			break;
		case COS:
			*x= "COS[" + *x+ "]";
			break;
		case TAN:
			*x= "TAN[" + *x+ "]";
			break;
		case ARS:
			*x= "ATAN[[" + *x+ "]/SQRT[1-[" + *x+ "]*[" + *x+"]]]";
			break;
		case ARC:
			*x= "2*ATAN[SQRT[[1-["+ *x+ "]]/[1+[" + *x+ "]]]]";
			break;
		case ART:
			*x= "ATAN[" + *x+ "]";
			break;
		case INT:
			*x= "FIX[" + *x+ "]";
			break;
		case ABS:
			*x= "ABS[" + *x+ "]";
			break;
	}
	return (void*)x;
	
}

extern "C" void* Func2(int function,void* expressionOne, void* expressionTwo){
	
	string* x1 = (string*)expressionOne; // result string
	string* x2 = (string*)expressionTwo;
	
	switch(function){
		case MOD:
			// x1 - FIX(x1 / x2)*x2
			*x1 = *x1 + "-FIX[[" + *x1 + "]/[" + *x2 +"]]*[" + *x2 + "]";
			break;
	}
	delete x2;
	
	return x1;
}

