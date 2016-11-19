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



static deque<Block*>::iterator FindBlock(char* label){
	int numBlock = LabledBlocksTable[label];
	for(auto curBlock = programFanuc.begin(); curBlock!= programFanuc.end(); curBlock++ ){

		if((*curBlock)->numberOfBlock == numBlock)
			return curBlock;
	}
	return programFanuc.end();
}



static deque<Block*>::iterator FindFrontEppBlock(){
	for(auto curBlock = programFanuc.begin(); curBlock!= programFanuc.end(); curBlock++ ){

		if( (*curBlock)->type == TB_EPP ) {
			return curBlock;
		}
	}
	return programFanuc.end();
}



static void TranslateEppBlock(deque<Block*>::iterator currentBlock, int variableNumber, int blockNumberToGo) {
	
	int numberOfBlockAfterEpp = ++MaximalNumberOfBlock;
	
	*(*currentBlock)->translatedBlock = "#" + to_string(variableNumber) + "=" + to_string(numberOfBlockAfterEpp);
	
	Block *b = new Block();				
	b->translatedBlock = new string( string("GOTO ") + to_string(blockNumberToGo) );
	programFanuc.insert(currentBlock+1, b);
	
	b = new Block();				
	b->translatedBlock = new string( string("N") + to_string(numberOfBlockAfterEpp) );
	programFanuc.insert(currentBlock+2, b);
	
}



/* Find first EPP block and process it.
   Returns 0 if no EPP block in fanucProgram. */
extern "C" int ProcessEppBlock() {

	auto firstEpp = FindFrontEppBlock();
	if (firstEpp == programFanuc.end()) {
		/* There is no unprocessed EPP in programFanuc */

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
		
		auto firstLabelBlock = FindBlock(curEppBlock->labelOne);
		programFanuc.insert(firstLabelBlock, NBlock);
		
		MatchLabelAndNumberOfBlock[curEppBlock->labelOne] = MaximalNumberOfBlock;
	}


	if ( MatchLabelAndVariable.find(curEppBlock->labelTwo) == MatchLabelAndVariable.end() ) {
		
		Block* GotoBlock = new Block();				
		int VariableForLabel = getUnusedFanucVariable();
		GotoBlock->translatedBlock = new string(string("GOTO #") + to_string(VariableForLabel));
		
		auto secondLabelBlock = FindBlock(curEppBlock->labelTwo);
		programFanuc.insert(secondLabelBlock, GotoBlock);
		
		MatchLabelAndVariable[curEppBlock->labelTwo] = VariableForLabel;
	}


	// add function for addition of number block
	if ( MatchLabelAndNumberOfBlock.find(curEppBlock->labelTwo) == MatchLabelAndNumberOfBlock.end() ) {
		
		Block* NBlock = new Block();				
		NBlock->translatedBlock =  new string(string("N") +
		                                      to_string(++MaximalNumberOfBlock) +
		                                      " (" + curEppBlock->labelTwo + ")");
		
		auto secondLabelBlock = FindBlock(curEppBlock->labelTwo);
		programFanuc.insert(secondLabelBlock, NBlock);
		
		MatchLabelAndNumberOfBlock[curEppBlock->labelTwo] = MaximalNumberOfBlock;
	}
	
	firstEpp = FindFrontEppBlock(); /* Bug was here: modifing the deque object discards all previously stated iterators? */
	(*firstEpp)->type = TB_ORDINARY;
	TranslateEppBlock(firstEpp, MatchLabelAndVariable[curEppBlock->labelTwo], MatchLabelAndNumberOfBlock[curEppBlock->labelOne]);
	
	return 1;
}

