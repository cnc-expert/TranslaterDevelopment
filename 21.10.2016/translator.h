#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#ifdef cplusplus
extern "C"{
#endif
void PrintCPPstring(void* str);
void* CreateExpressionBlock(char* variableNC,	void* expression);
void* MinusOperation(void* expression);
void* BinaryOperation(void* leftExpression, char sign, void* rightExpression);
void*  PutInBrackets(void* expression);
void*  CharToCppString(char* tokenNum );
void* Func(int function, void* expression);
void* Func2(int function, void* expressionOne, void* expressionTwo);
#ifdef cplusplus
}
#endif
#endif