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

enum typeOfBlock {TB_EPP, TB_ORDINARY, TB_UNINIT_EPP, TB_RPT, TB_ERP, TB_JUMP, TB_CHAMFER, TB_G40};

extern int CounterOfBlocks; // counter of the temporary blocks' numbers
extern int MaximalNumberOfBlock;

// Map: label -> temporary block number.
extern map<char*, int> LabledBlocksTable;

// Map: label -> block number.
extern map<char*, int> MatchLabelAndNumberOfBlock;

extern int getUnusedFanucVariable();

class Block {
	public:
		char *indentation;
		string* translatedBlock;
		int numberOfBlock;
		enum typeOfBlock type;
		char* label;

	Block() ;
	~Block();
};


class EppBlock : public Block {
	public:
		char* labelOne;
		char* labelTwo;

	EppBlock();
};

class RptBlock : public Block {
	public:
		char* counter;
		
};


extern deque<Block*> programFanuc;

// X Y Z G M T F S N R I J K
extern map <int, string> SingleLetterFunctionTable;

extern map <int, int> GCodeTable;

int MatchinFanucVariableToNC(char* variableNC);

deque<Block*>::iterator FindFrontTypedBlock(enum typeOfBlock type);

deque<Block*>::iterator FindLabeledBlock(char* label);

string IndetifyVariableOrNumber(char* expression);



extern "C"{
#endif
void ProcessG40Block();
void ProcessChamferBlock();
void* CreateDISWithVarBlock(char* variable);
void* CreateMIRBlock(int axis);
void* ChooseCoordinateSystem(char* expression);
void* CreateJumpBlock(char* condition, char* arg1, char* arg2, char* label);
void ProcessJumpBlock();
void* CreateBNCBlock(char* label);
void ProcessRptBlock();
void* CreateERPDeque();
void* CreateRPTDeque(char* counter);
void PrintProgramDeque();
void CreateProgramDeque(void* dequeObject);
void* AddIndentationToBlock(char* indentation, void *deque);
void* AddOPDIVtoBlocks(void* dequeObject);
void* EnumerateBlocks(void* listObject, char* currentNumberOfBlock);
void* AddLabelToDequeOfBlock(void* listObject, char* label);
void* CreateDefinedDequeForBlockString(char* blockStr);
void* CreateDefinedDequeForComments(char* blockStr);
void* CreateDequeForBlock(void* block);
void* CreateDequeForBlockString(void* blockStr);
void* ConcatWords(void* arg1, void* arg2);
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
char* PreMinusCString(char *s);
#ifdef __cplusplus
}
#endif


#endif
