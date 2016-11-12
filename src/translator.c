#include "nc.tab.h"
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <deque>

using namespace std;

// EPP function:
//
//#123 = 1
//N12
//...
//GOTO #123
//N1
//...
//#123 = 13 (N (12 + 1) )
//GOTO 1
//N13

class Block {
	public:
		string* translatedBlock;
		int numberOfBlock;
		int numberOfBlockGoTo;
	
	Block() {
		numberOfBlock = -1;
		numberOfBlockGoTo = -1;
	}
};


deque<Block> programFanuc;

// map: lable -> temporary block number
map<char*, int> LabledBlocksTable;

int CounterOfBlocks = 0; // counter of the temporary blocks' numbers
int MaximalNumberOfBlock = 0;


// X Y Z G M T F S N R I J K 
map <int, string> SingleLetterFunctionTable = {
	
	{ X, "X"},
	{ Y, "Y"},
	{ Z, "Z"},
	{ G, "G"},
	{ M, "M"},
	{ T, "T"},
	{ F, "F"},
	{ S, "S"},
	{ N, "N"},
	{ R, "R"},
	{ I, "I"},
	{ J, "J"},
	{ K, "K"}
	
};

map <int, int> GCodeTable = {
	
	{ 0, 0 },
	{ 1, 1 },
	{ 2, 2 },
	{ 3, 3 },
	{ 33, 33 },
	{ 17, 17 },
	{ 18, 18 },
	{ 19, 19 },
	{ 34, 34 },
	{ 27, 64 },
	{ 28, 64 },
	{ 29, 60 },
	{ 21, -1 },
	{ 20, -1 },
	{ 40, 40 },
	{ 41, 41 },
	{ 42, 42 },
	{ 70, -1 }, // to something else ???
	{ 71, -1 }, // to something else ???
	{ 80, 80 },
	{ 81, 81 },
	{ 82, 82 },
	{ 84, 84 },
	{ 85, -1 },
	{ 86, -1 },
	{ 89, -1 },
	{ 90, 90 },
	{ 91, 91 },
	{ 79, 53 },
	{ 4, 4 },
	{ 9, 9 },
	{ 72, -1 },
	{ 73, -1 },
	{ 74, -1 },
	{ 93, -1 },
	{ 94, 94 },
	{ 95, 95 },
	{ 35, -1 }
};

map<char*, int> UsedVariableTable; // table containing the variables in use (type "set")

// Fanuc Variable Range
// #100...199
// #500...999
set<int> CreateEmptyVariablesIndexTable() {
	
	set<int> emptyVariablesTableTmp;
	
	for (int i = 100; i <= 199; i++) {
		emptyVariablesTableTmp.insert(i);
	}
	
	for (int i = 500; i <= 999; i++) {
		emptyVariablesTableTmp.insert(i);
	}
	
	return emptyVariablesTableTmp;
}

set<int> EmptyVariablesIndexTable = CreateEmptyVariablesIndexTable(); // table containing only empty variables (type "set")

extern "C" void PrintProgramDeque() {
	cout << programFanuc.size() << endl;
	while (!programFanuc.empty()) {
		cout << programFanuc.front().numberOfBlock << " " << programFanuc.front().translatedBlock << endl;
		programFanuc.pop_front();
	}
	
}

extern "C" void CreateProgramDeque(void* dequeObject) {
	deque<Block>* dequeTmp = (deque<Block>*)dequeObject;
	
	cout << dequeTmp->size() << endl;
	
	while (!dequeTmp->empty()) {
		programFanuc.push_back(dequeTmp->front());
		dequeTmp->pop_front();
	}
	
	delete dequeTmp;
}

extern "C" void* AddOPDIVtoBlocks(void* dequeObject) {
	deque<Block>* dequeTmp = (deque<Block>*)dequeObject;
	
	string* tmpStr = new string("/");
	// add to the start of string func
	*tmpStr += *dequeTmp->front().translatedBlock;
	delete dequeTmp->front().translatedBlock;
	
	dequeTmp->front().translatedBlock = tmpStr;
	
	return dequeObject;	
}

extern "C" void* EnumerateBlocks(void* dequeObject, char* currentNumberOfBlock) {
	
	deque<Block>* dequeTmp = (deque<Block>*)dequeObject;
	
	if ( atoi(currentNumberOfBlock) > MaximalNumberOfBlock )
		MaximalNumberOfBlock = atoi(currentNumberOfBlock);
	
	string* tmpStr = new string("N");
	// add to the start of string func
	*tmpStr += currentNumberOfBlock + ( " " + *dequeTmp->front().translatedBlock );
	delete dequeTmp->front().translatedBlock;
	
	dequeTmp->front().translatedBlock = tmpStr;
	
	return dequeObject;
	// correct mistake -> core dump
}

extern "C" void* AddLabelToDequeOfBlock(void* dequeObject, char* label) {
	
	deque<Block>* dequeTmp = (deque<Block>*)dequeObject;
	dequeTmp->front().numberOfBlock = CounterOfBlocks;
	
	LabledBlocksTable[label] = CounterOfBlocks;
	
	CounterOfBlocks++;
	
	return dequeObject;
	
}

extern "C" void* CreateDefinedDequeForBlockString(char* blockStr) {
	
	cout << "CreateDefinedDequeForBlockString in process..." << endl;
	Block blockObject;
	blockObject.translatedBlock = new string(blockStr);
	
	deque<Block> *programFanuc = new deque<Block>();
	
	programFanuc->push_back(blockObject);
	
	cout << programFanuc->size() << endl;;
	
	return programFanuc;
}

extern "C" void* CreateDequeForBlockString(void* blockStr) {
	
	Block blockObject;
	blockObject.translatedBlock = (string*)blockStr;
	
	deque<Block> *programFanuc = new deque<Block>();
	
	programFanuc->push_back(blockObject);	
	
	return programFanuc;
}

extern "C" void PrintCppString(void* str) {
	
	cout << "Printing CppString in process. Please, wait..." << endl;
	
	string* x = (string*)str;
	cout << *x << endl;
}

extern "C" void PrintInt(int x) {
	
	cout << "Printing CppString in process. Please, wait..." << endl;
	cout << x << endl;
}

int GetVariableNCIndexForFanuc(char* variableNC) { // get the index of variable in NC code
	
	if ( UsedVariableTable.find(variableNC) == UsedVariableTable.end() ) {
		
		UsedVariableTable[variableNC] = *EmptyVariablesIndexTable.begin();
		EmptyVariablesIndexTable.erase(EmptyVariablesIndexTable.begin());
		
	}
	
	return UsedVariableTable[variableNC];
}

bool ValidateExpressionAboutDot(string expressionStr) { // validation
	// add T.19
	string result = expressionStr.substr(0, expressionStr.find('.') );
	//cout << "\nResult: " << result << endl;
	if ( result.length() == expressionStr.length() ) {
		return false;
	}
	else {
		return true;
	}
}

extern "C" void* TranslateWordWithNumber(int address, const char* sign, char* number) {
	
	string* resultWord = new string("");
	string expressionBeforeDot;
	string expressionAfterDot;
	bool resultOfValidationTFunction;
	
	//10
	//E18
	// G M T F S N R I J K 
	switch (address) {
		case G:
			//*resultWord = to_string(address) + string(sign) + to_string(GCodeTable[atoi(number)]);
			*resultWord = "G" + string(sign) + to_string(GCodeTable[atoi(number)]);
			break;
		case M:
			*resultWord = "M" + string(sign) + string(number);
			break;
		case T:
			resultOfValidationTFunction = ValidateExpressionAboutDot(string(number));
			if (resultOfValidationTFunction) {
				expressionBeforeDot = string(number).substr(0, string(number).find('.'));
				expressionAfterDot = string(number).substr(string(number).find('.') + 1);
				*resultWord = "T" +  expressionBeforeDot + " D" + expressionAfterDot + " H" + expressionAfterDot;
			}
			else {
				cout << "This expression is invalid. Example of correct expression: T1.5 \nPlease, try again.\n";
			}
			// add error for dot absent
			break;
		case F:
			*resultWord = "F" + string(number);
			break;
		case S:
			*resultWord = "S" + string(number);
			break;
		case N:
			*resultWord = "N" + string(number);
			break;
		case R:
			*resultWord = "R" + string(number);
			break;
		case I:
			*resultWord = "I" + string(number);
			break;
		case J:
			*resultWord = "J" + string(number);
			break;
		case K:
			*resultWord = "K" + string(number);
			break;
		case X:
			*resultWord = "X" + string(number);
			break;
		case Y:
			*resultWord = "Y" + string(number);
			break;
		case Z:
			*resultWord = "Z" + string(number);
			break;	
		default:
			*resultWord = to_string(address) + string(sign) + string(number);
			break;
	}
	
	return (void*)resultWord;
}

extern "C" void* TranslateWordWithVariable(int address, char* variable) 
{	
	string* resultWord = new string("");
	
	*resultWord = to_string(address) + "#" + to_string(GetVariableNCIndexForFanuc(variable));
	
	return (void*)resultWord;
}

extern "C" void* TranslateExpressionBlock(char* variableNC, void* expression) {
	
	//cout << "Translating expression block in process. Please, wait..." << endl;
	
	string* x = (string*)expression;
	*x = "#" + to_string(GetVariableNCIndexForFanuc(variableNC)) + "=" + *x; // E15 --> #100
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

extern "C" void* ConcatCppString(void* arg1, void* arg2) {
	
	// pointers correction
	
	*(string*)arg1 = *(string*)arg1 + " " + *(string*)arg2;
	delete (string*)arg2;
	return arg1;
}

extern "C" void* ConvertCharToCppString(char* tokenNum ) {
	
	return new string(tokenNum);
}

extern "C" void* TranslateFunction(int function,void* expression) {
	
	string* x = (string*)expression; // pointer to String*
	
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
			*x= "FIX[" + *x + "]"; // INT(x) --> FIX[x]
			break;
		case ABS:
			*x= "ABS[" + *x + "]"; // ABS(x) --> ABS[x]
			break;
	}
	
	return (void*)x;
	
}

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

