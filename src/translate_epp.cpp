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

// Mapping: labelTwo -> labelOne in EPP pairs.
static map<char*, char*> MatchLabels;



extern "C" void* CreateEPPBlock(char* labelOne, char* labelTwo){
	EppBlock* blockObject = new EppBlock();
	blockObject->translatedBlock = new string("EppBlock");

	blockObject->labelOne = labelOne;
	blockObject->labelTwo = labelTwo;

	deque<Block*> *programFanuc = new deque<Block*>();

	programFanuc->push_back(blockObject);

	return programFanuc;
}


/* Setting up 'MatchLabels': for any label 'labelTwo' find the highest 'labelOne'. */
static void SetMinimalBlockNumber(EppBlock *b) {

	if ( MatchLabels.find(b->labelTwo) == MatchLabels.end() ) {
		/* If pair for labelTwo does not exist: */

		MatchLabels[b->labelTwo] = b->labelOne;
	}
	else {
		/* If pair for labelTwo exists -
		   check if it might be greater than in current EppBlock 'b'. */

		if (LabledBlocksTable[MatchLabels[b->labelTwo]] > LabledBlocksTable[b->labelOne]) {
			MatchLabels[b->labelTwo] = b->labelOne;
		}
	}
}



/* Find block by the label. */
deque<Block*>::iterator FindLabeledBlock(char* label) {
	int numBlock = LabledBlocksTable[label];
	for(auto curBlock = programFanuc.begin(); curBlock!= programFanuc.end(); curBlock++ ){

		if((*curBlock)->numberOfBlock == numBlock)
			return curBlock;
	}
	return programFanuc.end();
}



/* Find first block by it's type. */
deque<Block*>::iterator FindFrontTypedBlock(enum typeOfBlock type){
	for(auto curBlock = programFanuc.begin(); curBlock!= programFanuc.end(); curBlock++ ){

		if( (*curBlock)->type == type ) {
			return curBlock;
		}
	}
	return programFanuc.end();
}


/* Initialize of callback variable for labels between 'labelOne' and 'labelTwo'.
   To prevent unexpected jumpings while going from 'labelOne' to 'labelTwo'. */
static void InitializeIntermediateLabels() {

	auto firstEppBlockIter = FindFrontTypedBlock(TB_UNINIT_EPP);
	
	while ( firstEppBlockIter != programFanuc.end() ) {
		
		EppBlock* firstEppBlock = (EppBlock*)*firstEppBlockIter;
		firstEppBlock->type = TB_ORDINARY;
		deque<Block*> tmpDeque;
		
		for ( auto intermediateBlockIter = FindLabeledBlock(firstEppBlock->labelOne) + 1;
		      (*intermediateBlockIter)->numberOfBlock != LabledBlocksTable[firstEppBlock->labelTwo];
		      intermediateBlockIter++ ) {
			
			if( (*intermediateBlockIter)->label != NULL && 
					MatchLabelAndVariable.find((*intermediateBlockIter)->label) != MatchLabelAndVariable.end() ) {
				
				Block *b = new Block();
				b->indentation = firstEppBlock->indentation;
				b->translatedBlock = new string(string("#") +
				                                to_string(MatchLabelAndVariable[(*intermediateBlockIter)->label] ) +
				                                string("=") +
				                                to_string(MatchLabelAndNumberOfBlock[(*intermediateBlockIter)->label] ) );

				tmpDeque.push_front(b);
				
			}	
			
		}
		
		
		while ( !tmpDeque.empty() ) {
		
			firstEppBlockIter = programFanuc.insert( firstEppBlockIter, tmpDeque.front() );
			tmpDeque.pop_front();
		
		}
		
		firstEppBlockIter = FindFrontTypedBlock(TB_UNINIT_EPP);
	}
	
}

/* Substitue EPP block by three blocks:
   1) var with callback address,   2) GOTO block,   3) numbered block to return. */
static void TranslateFirstEppBlock(char *labelOne, char *labelTwo) {

	int variableNumber = MatchLabelAndVariable[labelTwo];
	int blockNumberToGo = MatchLabelAndNumberOfBlock[labelOne];
	
	auto firstEppBlock = FindFrontTypedBlock(TB_EPP);
	(*firstEppBlock)->type = TB_UNINIT_EPP; //   uninitialized epp_block
	
	
	int numberOfBlockAfterEpp = ++MaximalNumberOfBlock;
	*(*firstEppBlock)->translatedBlock = "#" + to_string(variableNumber) + "=" + to_string(numberOfBlockAfterEpp);
	
	Block *b = new Block();				
	b->indentation = (*firstEppBlock)->indentation;
	b->translatedBlock = new string( string("GOTO ") +
	                                 to_string(blockNumberToGo) +
									 " (" + labelOne + " -> " + labelTwo + ")");
	firstEppBlock = programFanuc.insert(firstEppBlock+1, b);
	
	b = new Block();
	b->indentation = (*firstEppBlock)->indentation;
	b->translatedBlock = new string( string("N") + to_string(numberOfBlockAfterEpp) );
	programFanuc.insert(firstEppBlock + 1, b);
	
}


/* New variable initialization block before 'labelOne':
   initialize of callback variable by default address (corresponding to 'labelTwo')
   for linear program execution. */
static void InitializeEppBlocks() {

	for (auto currentLabel = MatchLabels.begin(); currentLabel != MatchLabels.end(); currentLabel++ ) {
		char *firstLabel = (*currentLabel).second;
		while (MatchLabels.find(firstLabel) != MatchLabels.end() ) {
			firstLabel = MatchLabels[firstLabel];
		}
		
		auto firstLabelIter = FindLabeledBlock(firstLabel);
		char *secondLabel = (*currentLabel).first;

		Block* initBlock = new Block();				
		initBlock->indentation = (*(firstLabelIter - 1))->indentation;
		initBlock->translatedBlock =  new string(string("#") +
		                                      to_string(MatchLabelAndVariable[secondLabel]) +
											  string("=") 
											  + to_string(MatchLabelAndNumberOfBlock[secondLabel]) );
		
		programFanuc.insert(firstLabelIter - 1, initBlock);								  
		
	}	
}


/* Find first EPP block and process it.
   Returns 0 if there are no EPP blocks in fanucProgram. */
extern "C" int ProcessEppBlock() {

	auto firstEpp = FindFrontTypedBlock(TB_EPP);
	if (firstEpp == programFanuc.end()) {
		/* There is no unprocessed EPPs in programFanuc. */

		InitializeEppBlocks();
		InitializeIntermediateLabels();
		return 0;
	}

	Block* tmpCurBlock = *firstEpp;
	EppBlock* curEppBlock = (EppBlock*)tmpCurBlock;

	/* Check the highest label. */
	SetMinimalBlockNumber(curEppBlock);

	/* Insert block number before 'labelOne'. */
	if ( MatchLabelAndNumberOfBlock.find(curEppBlock->labelOne) == MatchLabelAndNumberOfBlock.end() ) {

		auto firstLabelBlock = FindLabeledBlock(curEppBlock->labelOne);

		Block* NBlock = new Block();
		NBlock->indentation = (*firstLabelBlock)->indentation;
		NBlock->translatedBlock = new string(string("N") +
		                                     to_string(++MaximalNumberOfBlock) );
		                                //   + " (" + curEppBlock->labelOne + ")");
		
		programFanuc.insert(firstLabelBlock, NBlock);
		
		MatchLabelAndNumberOfBlock[curEppBlock->labelOne] = MaximalNumberOfBlock;
	}

	/* Insert GOTO block before 'labelTwo'. */
	if ( MatchLabelAndVariable.find(curEppBlock->labelTwo) == MatchLabelAndVariable.end() ) {
		
		auto secondLabelBlock = FindLabeledBlock(curEppBlock->labelTwo);

		Block* gotoBlock = new Block();				
		int VariableForLabel = getUnusedFanucVariable();
		gotoBlock->indentation = (*secondLabelBlock)->indentation;
		gotoBlock->translatedBlock = new string(string("GOTO #") + to_string(VariableForLabel));
		
		programFanuc.insert(secondLabelBlock, gotoBlock);
		
		MatchLabelAndVariable[curEppBlock->labelTwo] = VariableForLabel;
	}


	// add function for addition of number block
	/* Insert block number before 'labelTwo' for possibility of linear execution. */
	if ( MatchLabelAndNumberOfBlock.find(curEppBlock->labelTwo) == MatchLabelAndNumberOfBlock.end() ) {

		auto secondLabelBlock = FindLabeledBlock(curEppBlock->labelTwo);

		Block* NBlock = new Block();				
		NBlock->indentation = (*secondLabelBlock)->indentation;
		NBlock->translatedBlock =  new string(string("N") +
		                                      to_string(++MaximalNumberOfBlock) );
		                                 //   + " (" + curEppBlock->labelTwo + ")");
		
		programFanuc.insert(secondLabelBlock, NBlock);
		
		MatchLabelAndNumberOfBlock[curEppBlock->labelTwo] = MaximalNumberOfBlock;
	}
	
	TranslateFirstEppBlock(curEppBlock->labelOne, curEppBlock->labelTwo);
	
	return 1;
}

