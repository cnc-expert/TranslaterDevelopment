#include <iostream>
#include <string>
#include <map>
#include <set>
#include <deque>
#include <algorithm>
#include "translator.h"
#include "parser.tab.h"
//#include "SimpleBlocks.cpp"

using namespace std;



Block::Block() {
	indentation = NULL;
	numberOfBlock = -1;
	label = NULL;
	type = TB_ORDINARY;
}	


// Counter of the temporary blocks' numbers.
int CounterOfBlocks = 0;

map<char*, int> LabledBlocksTable;

int MaximalNumberOfBlock = 0;

map<char*, int> MatchLabelAndNumberOfBlock;


deque<Block*> programFanuc;

// X Y Z G M T F S N R I J K
map <int, string> SingleLetterFunctionTable = {

	{ X, "X"},	{ Y, "Y"},	{ Z, "Z"},	{ G, "G"},
	{ M, "M"},	{ T, "T"},	{ F, "F"},	{ S, "S"},
	{ N, "N"},	{ R, "R"},	{ I, "I"},	{ J, "J"},
	{ K, "K"}

};

map <int, int> GCodeTable = {

	{ 0, 0 },	{ 1, 1 },	{ 2, 2 },
	{ 3, 3 },	{ 33, 33 },	{ 17, 17 },
	{ 18, 18 },	{ 19, 19 },	{ 34, 34 },
	{ 27, 64 },	{ 28, 64 },	{ 29, 60 },
	{ 21, -1 },	{ 20, -1 },	{ 40, 40 },
	{ 41, 41 },	{ 42, 42 },
	{ 70, -1 }, // to something else ???
	{ 71, -1 }, // to something else ???
	{ 80, 80 },	{ 81, 81 },	{ 82, 82 }, { 83, 83 },
	{ 84, 84 },	{ 85, -1 },	{ 86, -1 },
	{ 89, -1 },	{ 90, 90 },	{ 91, 91 },
	{ 79, 53 },	{ 4, 4 },	{ 9, 9 },
	{ 72, -1 },	{ 73, -1 },	{ 74, -1 },
	{ 93, -1 },	{ 94, 94 },	{ 95, 95 },
	{ 35, -1 }
};



string IndetifyVariableOrNumber(char* expression) {
	
	if (expression[0] == 'E') {
		return string("#") + to_string(MatchinFanucVariableToNC(expression));
	}
	else {
		return string(expression);
	}
	
}


extern "C" void PrintProgramDeque() {

	while (!programFanuc.empty()) {
		Block *b = programFanuc.front();
		if (b->indentation) {
			cout << b->indentation;
		}
		cout << *b->translatedBlock << endl;
		programFanuc.pop_front();
		delete b;
	}

}

extern "C" void CreateProgramDeque(void* dequeObject) {
	deque<Block*>* dequeTmp = (deque<Block*>*)dequeObject;

	while (!dequeTmp->empty()) {
		programFanuc.push_front(dequeTmp->front());
		dequeTmp->pop_front();
	}

	delete dequeTmp;
}

extern "C" void* AddIndentationToBlock(char* indentation, void *deque_of_blocks) {
	deque<Block*>* blocks = (deque<Block*>*) deque_of_blocks;

	for (auto i = blocks->begin(); i != blocks->end(); i++) {

		(*i)->indentation = indentation;
	}

	return blocks;
}

extern "C" void* AddOPDIVtoBlocks(void* dequeObject) {
	deque<Block*>* dequeTmp = (deque<Block*>*)dequeObject;

	string* tmpStr = new string("/");
	// add to the start of string func
	*tmpStr += *dequeTmp->front()->translatedBlock;
	delete dequeTmp->front()->translatedBlock;

	dequeTmp->front()->translatedBlock = tmpStr;

	return dequeObject;
}

extern "C" void* EnumerateBlocks(void* dequeObject, char* currentNumberOfBlock) {

	deque<Block*>* dequeTmp = (deque<Block*>*)dequeObject;

	if ( atoi(currentNumberOfBlock) > MaximalNumberOfBlock )
		MaximalNumberOfBlock = atoi(currentNumberOfBlock);

	string* tmpStr = new string("N");
	// add to the start of string func
	*tmpStr += currentNumberOfBlock + ( " " + *dequeTmp->front()->translatedBlock );
	delete dequeTmp->front()->translatedBlock;

	dequeTmp->front()->translatedBlock = tmpStr;

	return dequeObject;
	// correct mistake -> core dump
}

extern "C" void* AddLabelToDequeOfBlock(void* dequeObject, char* label) {

	deque<Block*>* dequeTmp = (deque<Block*>*)dequeObject;
	dequeTmp->front()->numberOfBlock = CounterOfBlocks;
	dequeTmp->front()->label = label;
	
	// Add label name as comment.
	string* tmpStr = new string(label);
	*tmpStr = "(" + *tmpStr + ") " + *dequeTmp->front()->translatedBlock;
	delete dequeTmp->front()->translatedBlock;
	dequeTmp->front()->translatedBlock = tmpStr;

	LabledBlocksTable[label] = CounterOfBlocks;
	CounterOfBlocks++;

	return dequeObject;

}



extern "C" void* CreateDequeForBlockString(void* blockStr) {

	Block* blockObject = new Block();
	blockObject->translatedBlock = (string*)blockStr;

	deque<Block*> *blocks = new deque<Block*>();

	blocks->push_back(blockObject);

	return blocks;
}



extern "C" void* CreateDefinedDequeForBlockString(char* blockStr) {
	
	string *s = new string(blockStr);

	return CreateDequeForBlockString(s);
}



extern "C" void* CreateDefinedDequeForComments(char* blockStr) {

	Block* blockObject = new Block();

	blockObject->translatedBlock = new string(blockStr);
	replace(blockObject->translatedBlock->begin(),blockObject->translatedBlock->end(), ')', ' ');
	replace(blockObject->translatedBlock->begin(),blockObject->translatedBlock->end(), '(', ' ');
	
	*blockObject->translatedBlock = ("(" + *blockObject->translatedBlock + ")");

	deque<Block*> *programFanuc = new deque<Block*>();
	programFanuc->push_back(blockObject);

	return programFanuc;
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

extern "C" void* TranslateWordWithNumber(int address, const char* sign, const char* number) {

	string* resultWord = new string("");
	string expressionBeforeDot;
	string expressionAfterDot;
	bool resultOfValidationTFunction;

	switch (address) {
		case G:	*resultWord = "G" + to_string(GCodeTable[atoi(number)]);	break;
		case M:	*resultWord = "M" + string(number);	break;
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
		case F:	*resultWord = "F" + string(number);	break;
		case S:	*resultWord = "S" + string(number);	break;
		case N:	*resultWord = "N" + string(number);	break;
		case R:	*resultWord = "R" + string(sign) + string(number);	break;
		case I:	*resultWord = "I" + string(sign) + string(number);	break;
		case J:	*resultWord = "J" + string(sign) + string(number);	break;
		case K:	*resultWord = "K" + string(number);	break;
		case X:	*resultWord = "X" + string(sign) + string(number);	break;
		case Y:	*resultWord = "Y" + string(sign) + string(number);	break;
		case Z:	*resultWord = "Z" + string(sign) + string(number);	break;
		default: *resultWord = to_string(address) + string(sign) + string(number);	break;
	}

	return (void*)resultWord;
}

extern "C" void* TranslateExpressionBlock(char* variableNC, void* expression) {

	//cout << "Translating expression block in process. Please, wait..." << endl;

	string* x = (string*)expression;
	*x = "#" + to_string(MatchinFanucVariableToNC(variableNC)) + "=" + *x; // E15 --> #100
	
	
	return (void*)x;
}


extern "C" void* TranslateWordWithVariable(int address, char* variable)
{
	string *varString = (string *) SubstituteNCvarAsCppString(variable);

	return TranslateWordWithNumber(address, "", varString->c_str());
}




extern "C" void* ChooseCoordinateSystem(char* expression) {
	
	// UAO
	
	Block* blockObject = new Block();
	blockObject->translatedBlock = new string("G");
	
	if (expression[0] == 'E') {
		*blockObject->translatedBlock += string("[#") + to_string(MatchinFanucVariableToNC(expression)) + "+53]";
	}
	else
		*blockObject->translatedBlock += to_string( atoi(expression) + 53 );

	deque<Block*> *programFanuc = new deque<Block*>();	
	programFanuc->push_back(blockObject);

	return programFanuc;
	
}


extern "C" void* CreateURTBlock(char* value){
	Block* blockObject = new Block();
	blockObject->translatedBlock = new string();
	if (value[0] == 'E')
	{
		*blockObject->translatedBlock += "G68 X0 Y0 R#" +
		                                 to_string(MatchinFanucVariableToNC(value));
	} else if (0 == atoi(value)) {
		*blockObject->translatedBlock += "G69";
	} else {
		*blockObject->translatedBlock += "G68 X0 Y0 R#" + string(value);
	}

	deque<Block*> *programFanuc = new deque<Block*>();
	programFanuc->push_back(blockObject);

	return programFanuc;
}

extern "C" void* CreateDelayDeque(char* blockStr) {

	//cout << "CreateDefinedDequeForBlockString in process..." << endl;
	Block* blockObject = new Block();
	blockObject->translatedBlock = new string("G4 X");

	*blockObject->translatedBlock += IndetifyVariableOrNumber(blockStr);

	deque<Block*> *programFanuc = new deque<Block*>();

	programFanuc->push_back(blockObject);

	//cout << programFanuc->size() << endl;;

	return programFanuc;
}
