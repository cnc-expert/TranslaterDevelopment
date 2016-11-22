/*
 * EPP three-letter-code processing.
 */


#include "translator.h"


using namespace std;



EppBlock::EppBlock() {
	numberOfBlock = CounterOfBlocks++;
	type = TB_EPP;
	labelOne = NULL;
	labelTwo = NULL;
}



// Mapping: Label -> fanuc variable for callback.
static map<char*, int> MatchLabelAndVariable;

// Mapping: LableTwo -> number of highest block in EPP pairs.
static map<char*, int> MatchLabels;



extern "C" void* CreateEPPBlock(char* labelOne,char*  labelTwo){
	EppBlock* blockObject = new EppBlock();
	blockObject->translatedBlock = new string("EppBlock");////delete

	blockObject->labelOne = labelOne;
	blockObject->labelTwo = labelTwo;

	deque<Block*> *programFanuc = new deque<Block*>();

	programFanuc->push_back(blockObject);

	return programFanuc;
}


static void SetMinimalBlockNumber(EppBlock *b) {

	if ( MatchLabels.find(b->labelTwo) == MatchLabels.end() ) {
		/* If pair for labelTwo does not exist: */

		MatchLabels[b->labelTwo] = LabledBlocksTable[b->labelOne];
	}
	else {
		/* If pair for labelTwo exists -
		   check if it might be greater than in current EppBlock 'b'. */

		if (MatchLabels[b->labelTwo] > LabledBlocksTable[b->labelOne]) {
			MatchLabels[b->labelTwo] = LabledBlocksTable[b->labelOne];
		}
	}
}



/* Find block by the label. */
static deque<Block*>::iterator FindLabeledBlock(char* label){
	int numBlock = LabledBlocksTable[label];
	for(auto curBlock = programFanuc.begin(); curBlock!= programFanuc.end(); curBlock++ ){

		if((*curBlock)->numberOfBlock == numBlock)
			return curBlock;
	}
	return programFanuc.end();
}



/* Find first occurrence block by it's type. */
static deque<Block*>::iterator FindFrontTypedBlock(enum typeOfBlock type){
	for(auto curBlock = programFanuc.begin(); curBlock!= programFanuc.end(); curBlock++ ){

		if( (*curBlock)->type == type ) {
			return curBlock;
		}
	}
	return programFanuc.end();
}



/* Substitue EPP block by three blocks:
   1) var with callback address,   2) GOTO block,   3) numbered block to return. */
static void TranslateFirstEppBlock(int variableNumber, int blockNumberToGo) {
	
	auto firstEppBlock = FindFrontTypedBlock(TB_EPP);
	(*firstEppBlock)->type = TB_ORDINARY;

	int numberOfBlockAfterEpp = ++MaximalNumberOfBlock;
	*(*firstEppBlock)->translatedBlock = "#" + to_string(variableNumber) + "=" + to_string(numberOfBlockAfterEpp);
	
	Block *b = new Block();				
	b->translatedBlock = new string( string("GOTO ") + to_string(blockNumberToGo) );
	firstEppBlock = programFanuc.insert(firstEppBlock+1, b);
	
	b = new Block();				
	b->translatedBlock = new string( string("N") + to_string(numberOfBlockAfterEpp) );
	programFanuc.insert(firstEppBlock+1, b);
	
}



/* Find first EPP block and process it.
   Returns 0 if there are no EPP blocks in fanucProgram. */
extern "C" int ProcessEppBlock() {

	auto firstEpp = FindFrontTypedBlock(TB_EPP);
	if (firstEpp == programFanuc.end()) {
		/* There is no unprocessed EPPs in programFanuc */

		return 0;
	}

	Block* tmpCurBlock = *firstEpp;
	EppBlock* curEppBlock = (EppBlock*)tmpCurBlock;

	/* Check the highest label */
	SetMinimalBlockNumber(curEppBlock);


	if ( MatchLabelAndNumberOfBlock.find(curEppBlock->labelOne) == MatchLabelAndNumberOfBlock.end() ) {

		Block* NBlock = new Block();
		NBlock->translatedBlock = new string(string("N") +
		                                     to_string(++MaximalNumberOfBlock) +
		                                     " (" + curEppBlock->labelOne + ")");
		
		auto firstLabelBlock = FindLabeledBlock(curEppBlock->labelOne);
		programFanuc.insert(firstLabelBlock, NBlock);
		
		MatchLabelAndNumberOfBlock[curEppBlock->labelOne] = MaximalNumberOfBlock;
	}


	if ( MatchLabelAndVariable.find(curEppBlock->labelTwo) == MatchLabelAndVariable.end() ) {
		
		Block* GotoBlock = new Block();				
		int VariableForLabel = getUnusedFanucVariable();
		GotoBlock->translatedBlock = new string(string("GOTO #") + to_string(VariableForLabel));
		
		auto secondLabelBlock = FindLabeledBlock(curEppBlock->labelTwo);
		programFanuc.insert(secondLabelBlock, GotoBlock);
		
		MatchLabelAndVariable[curEppBlock->labelTwo] = VariableForLabel;
	}


	// add function for addition of number block
	if ( MatchLabelAndNumberOfBlock.find(curEppBlock->labelTwo) == MatchLabelAndNumberOfBlock.end() ) {
		
		Block* NBlock = new Block();				
		NBlock->translatedBlock =  new string(string("N") +
		                                      to_string(++MaximalNumberOfBlock) +
		                                      " (" + curEppBlock->labelTwo + ")");
		
		auto secondLabelBlock = FindLabeledBlock(curEppBlock->labelTwo);
		programFanuc.insert(secondLabelBlock, NBlock);
		
		MatchLabelAndNumberOfBlock[curEppBlock->labelTwo] = MaximalNumberOfBlock;
	}
	
	TranslateFirstEppBlock(MatchLabelAndVariable[curEppBlock->labelTwo], MatchLabelAndNumberOfBlock[curEppBlock->labelOne]);
	
	return 1;
}

