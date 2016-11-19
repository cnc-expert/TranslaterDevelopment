#include "nc.tab.h"
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <deque>
#include <algorithm>
#include "translator.h"
#include "ThreeWordFunc.c"

using namespace std;

Block::Block() {
	numberOfBlock = -1;
	numberOfBlockGoTo = -1;
	type = TB_ORDINARY;
}
EppBlock ::EppBlock() {
		numberOfBlock = CounterOfBlocks++;
		numberOfBlockGoTo = -1;
		type = TB_EPP;
	}

	


int CounterOfBlocks = 0; // counter of the temporary blocks' numbers
int MaximalNumberOfBlock = 0;

map<char*, int> LabledBlocksTable;

// map: lable -> temporary block number
map<char*, int> MatchLabelAndNumberOfBlock;
map<char*, int> MatchLabelAndVariable;
map<char*, int> MatchLabels; // Lable2 and highest label ( Label1 ) for EPP function


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
	{ 80, 80 },	{ 81, 81 },	{ 82, 82 },
	{ 84, 84 },	{ 85, -1 },	{ 86, -1 },
	{ 89, -1 },	{ 90, 90 },	{ 91, 91 },
	{ 79, 53 },	{ 4, 4 },	{ 9, 9 },
	{ 72, -1 },	{ 73, -1 },	{ 74, -1 },
	{ 93, -1 },	{ 94, 94 },	{ 95, 95 },
	{ 35, -1 }
};


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
	//cout <<"PrintProgramDeque "<< programFanuc.size() << endl;
	while (!programFanuc.empty()) {
		cout << endl << *programFanuc.back()->translatedBlock ;
		programFanuc.pop_back();
	}

}

extern "C" void CreateProgramDeque(void* dequeObject) {
	deque<Block*>* dequeTmp = (deque<Block*>*)dequeObject;

	//cout <<"CreateProgramDeque "<< dequeTmp->size() << endl;

	while (!dequeTmp->empty()) {
		programFanuc.push_back(dequeTmp->front());
		dequeTmp->pop_front();
	}
	//cout <<"PrintProgramDeque "<< programFanuc.size() << endl;
	delete dequeTmp;
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
	
	string* tmpStr = new string(label);
	// add lable name as comment
	*tmpStr = "(" + *tmpStr + ") " + *dequeTmp->front()->translatedBlock;
	delete dequeTmp->front()->translatedBlock;
	
	dequeTmp->front()->translatedBlock = tmpStr;

	LabledBlocksTable[label] = CounterOfBlocks;

	CounterOfBlocks++;

	return dequeObject;

}



extern "C" void* CreateDefinedDequeForBlockString(char* blockStr) {

	//cout << "CreateDefinedDequeForBlockString in process..." << endl;
	Block* blockObject = new Block();
	blockObject->translatedBlock = new string(blockStr);

	deque<Block*> *programFanuc = new deque<Block*>();

	programFanuc->push_back(blockObject);

	//cout << programFanuc->size() << endl;;

	return programFanuc;
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



extern "C" void* CreateDequeForBlockString(void* blockStr) {

	Block* blockObject = new Block();
	blockObject->translatedBlock = (string*)blockStr;

	deque<Block*> *programFanuc = new deque<Block*>();

	programFanuc->push_back(blockObject);

	return programFanuc;
}





int GetVariableNCIndexForFanuc(char* variableNC) { // get the index of variable in NC code

	// table containing the variables in use (type "set")
	static map<char*, int> UsedVariableTable;

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
	*x = "#" + to_string(GetVariableNCIndexForFanuc(variableNC)) + "=" + *x; // E15 --> #100
	
	
	return (void*)x;
}


extern "C" void* TranslateWordWithVariable(int address, char* variable)
{
	string* resultWord = new string("");
	
	//*resultWord = to_string(address) + "#" + to_string(GetVariableNCIndexForFanuc(variable));
	
	return  TranslateWordWithNumber(address,"", ("#" + to_string(GetVariableNCIndexForFanuc(variable))).c_str() );
	
	
}


/////////////////////////////////////////////////////////////////////////////////////////


extern "C" void* CreateEPPBlock(char* labelOne,char*  labelTwo){
	EppBlock* blockObject = new EppBlock();
	blockObject->translatedBlock = new string("EppBlock");////delete

	blockObject->labelOne = labelOne;
	blockObject->labelTwo = labelTwo;

	deque<Block*> *programFanuc = new deque<Block*>();

	programFanuc->push_back(blockObject);

	return programFanuc;
}

static deque<Block*>::iterator FindBlock(char* label){
	int numBlock = LabledBlocksTable[label];
	for(auto curBlock = programFanuc.begin(); curBlock!= programFanuc.end(); curBlock++ ){

		if((*curBlock)->numberOfBlock == numBlock)
			return curBlock;
	}
	return programFanuc.end();
}

extern "C" void* AddGOTOBlock(char* label)
{
	//BNC
	Block* blockObject = new Block();

	/*create*/
	//dont work
	deque<Block*> *programFanuc = new deque<Block*>();
	blockObject->translatedBlock = new string("GOTO ");
	++MaximalNumberOfBlock;
	int numBlock = LabledBlocksTable[label] + MaximalNumberOfBlock;
	
	programFanuc->push_back(blockObject);

	//cout << programFanuc->size() << endl;;

	return programFanuc;
	
}



void TranslateEppBlock(deque<Block*>::iterator currentBlock, int variableNumber, int blockNumber) {
	
	int numberOfBlockAfterEpp = ++MaximalNumberOfBlock;
	
	cout << "Type:" << (*currentBlock)->type << endl;
	cout << " (currentBlock)->translatedBlock: " << *(*currentBlock)->translatedBlock << endl;
	
	*(*currentBlock)->translatedBlock = "#" + to_string(variableNumber) + "=" + to_string(numberOfBlockAfterEpp);
	
	
	Block* NBlock = new Block();				
	NBlock->translatedBlock =  new string( string("N") + to_string(numberOfBlockAfterEpp) );
	
	programFanuc.insert(currentBlock, NBlock);
	
	NBlock = new Block();				
	NBlock->translatedBlock =  new string( string("GOTO ") + to_string(blockNumber) );
	
	programFanuc.insert(currentBlock, NBlock);
	
}


extern "C" int ProcessEppBlock() {

	
	for(auto curBlock = programFanuc.begin(); curBlock!= programFanuc.end(); curBlock++ ) {
		if( (*curBlock)->type == TB_EPP ) {
			
			(*curBlock)->type = TB_ORDINARY;
			
			Block* tmpCurBlock = *curBlock;
			EppBlock* curEppBlock = (EppBlock*)tmpCurBlock;
			
			if ( MatchLabels.find(curEppBlock->labelTwo) != MatchLabels.end() ) {
				// addition to MatchLabels
				MatchLabels[curEppBlock->labelTwo] = LabledBlocksTable[curEppBlock->labelOne];
			}
			else {
				
				if (MatchLabels[curEppBlock->labelTwo] > LabledBlocksTable[curEppBlock->labelOne]) {
					MatchLabels[curEppBlock->labelTwo] = LabledBlocksTable[curEppBlock->labelOne];
				}
				
			}
			
			if ( MatchLabelAndNumberOfBlock.find(curEppBlock->labelOne) == MatchLabelAndNumberOfBlock.end() ) {
				auto firstLabelBlock = FindBlock(curEppBlock->labelOne);
				Block* NBlock = new Block();
				
				firstLabelBlock++;
				
				NBlock->translatedBlock =  new string(string("N") + to_string(++MaximalNumberOfBlock));
				
				programFanuc.insert(firstLabelBlock, NBlock);
				
				MatchLabelAndNumberOfBlock[curEppBlock->labelOne] = MaximalNumberOfBlock;
			}
			// add function for addition of number block
			if ( MatchLabelAndNumberOfBlock.find(curEppBlock->labelTwo) == MatchLabelAndNumberOfBlock.end() ) {
				auto secondLabelBlock = FindBlock(curEppBlock->labelTwo);
				
				secondLabelBlock++;
				
				Block* NBlock = new Block();				
				NBlock->translatedBlock =  new string(string("N") + to_string(++MaximalNumberOfBlock));
				
				programFanuc.insert(secondLabelBlock, NBlock);
				
				MatchLabelAndNumberOfBlock[curEppBlock->labelTwo] = MaximalNumberOfBlock;
			}
			
			if ( MatchLabelAndVariable.find(curEppBlock->labelTwo) == MatchLabelAndVariable.end() ) {
				auto secondLabelBlock = FindBlock(curEppBlock->labelTwo);
				
				secondLabelBlock++; // for insert GOTO
				secondLabelBlock++;
				
				// add function then
				int VariableForLabel = *EmptyVariablesIndexTable.begin();
				EmptyVariablesIndexTable.erase(EmptyVariablesIndexTable.begin());
				
				Block* NBlock = new Block();				
				NBlock->translatedBlock =  new string(string("GOTO #") + to_string(VariableForLabel));
				
				programFanuc.insert(secondLabelBlock, NBlock);
				
				MatchLabelAndVariable[curEppBlock->labelTwo] = VariableForLabel;
				
			}
			
			TranslateEppBlock(curBlock, MatchLabelAndVariable[curEppBlock->labelTwo], MatchLabelAndNumberOfBlock[curEppBlock->labelOne]);
			
			return 1;
		}
	}
	return 0;
}









extern "C" void* CreateURTBlock(char* value){
	Block* blockObject = new Block();

	/*create*/

	deque<Block*> *programFanuc = new deque<Block*>();

	programFanuc->push_back(blockObject);

	//cout << programFanuc->size() << endl;;

	return programFanuc;
}

extern "C" void* CreateDelayDeque(char* blockStr) {

	//cout << "CreateDefinedDequeForBlockString in process..." << endl;
	Block* blockObject = new Block();
	blockObject->translatedBlock = new string("G4 X");
	if (blockStr[0] == 'E')
	{
		*blockObject->translatedBlock += string("#") + to_string(GetVariableNCIndexForFanuc(blockStr));
	}
	else
		*blockObject->translatedBlock += blockStr;

	deque<Block*> *programFanuc = new deque<Block*>();

	programFanuc->push_back(blockObject);

	//cout << programFanuc->size() << endl;;

	return programFanuc;
}
