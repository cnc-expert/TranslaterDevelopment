#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#ifdef __cplusplus

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <deque>
#include <algorithm>

using namespace std;

enum typeOfBlock {TB_EPP, TB_ORDINARY, TB_UNINIT_EPP};

extern int CounterOfBlocks; // counter of the temporary blocks' numbers
extern int MaximalNumberOfBlock;

// Map: label -> temporary block number.
extern map<char*, int> LabledBlocksTable;

// Map: label -> block number.
extern map<char*, int> MatchLabelAndNumberOfBlock;

extern int getUnusedFanucVariable();

class Block {
	public:
		string* translatedBlock;
		int numberOfBlock;
		enum typeOfBlock type;
		char* label;

	Block() ;
};


class EppBlock : public Block {
	public:
		char* labelOne;
		char* labelTwo;

	EppBlock();
};


extern deque<Block*> programFanuc;

// X Y Z G M T F S N R I J K
extern map <int, string> SingleLetterFunctionTable;

extern map <int, int> GCodeTable;

int MatchinFanucVariableToNC(char* variableNC);

extern "C"{
#endif

//void PrintInt(int x);
void PrintProgramDeque();
void CreateProgramDeque(void* dequeObject);
void* AddOPDIVtoBlocks(void* dequeObject);
void* EnumerateBlocks(void* listObject, char* currentNumberOfBlock);
void* AddLabelToDequeOfBlock(void* listObject, char* label);
void* CreateDefinedDequeForBlockString(char* blockStr);
void* CreateDefinedDequeForComments(char* blockStr);
void* CreateDequeForBlockString(void* blockStr);
void* ConcatCppString(void* arg1, void* arg2);
void PrintCppString(void* str);
void* TranslateWordWithNumber(int address, const char* sign, const char* number);
void* TranslateWordWithVariable(int address, char* variable);
void* TranslateExpressionBlock(char* variableNC,	void* expression);
void* ExecuteNegativeOperation(void* expression);
void* ExecuteArithmeticOperation(void* leftExpression, char sign, void* rightExpression);
void* PutExpressionInBrackets(void* expression);
void* ConvertCharToCppString(char* tokenNum);
void* SubstituteNCvarAsCppString(char *tokenVar);
void* TranslateFunction(int function, void* expression);
void* TranslateFunctionWithTwoArguments(int function, void* firstExpression, void* secondExpression);
void* CreateEPPBlock(char* lableOne,char*  labelTwo);
int  ProcessEppBlock();
void* CreateDelayDeque(char* blockStr);
void* CreateURTBlock(char* value);
#ifdef __cplusplus
}
#endif


#endif
