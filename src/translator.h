#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#ifdef __cplusplus
extern "C"{
#endif

//void PrintInt(int x);
void PrintProgramDeque();
void CreateProgramDeque(void* dequeObject);
void* AddOPDIVtoBlocks(void* dequeObject);
void* EnumerateBlocks(void* listObject, char* currentNumberOfBlock);
void* AddLabelToDequeOfBlock(void* listObject, char* label);
void* CreateDefinedDequeForBlockString(char* blockStr);
void* CreateDequeForBlockString(void* blockStr);
void* ConcatCppString(void* arg1, void* arg2);
void PrintCppString(void* str);
void* TranslateWordWithNumber(int address, const char* sign, char* number);
void* TranslateWordWithVariable(int address, char* variable);
void* TranslateExpressionBlock(char* variableNC,	void* expression);
void* ExecuteNegativeOperation(void* expression);
void* ExecuteArithmeticOperation(void* leftExpression, char sign, void* rightExpression);
void* PutExpressionInBrackets(void* expression);
void* ConvertCharToCppString(char* tokenNum);
void* TranslateFunction(int function, void* expression);
void* TranslateFunctionWithTwoArguments(int function, void* firstExpression, void* secondExpression);

#ifdef __cplusplus
}
#endif
#endif
