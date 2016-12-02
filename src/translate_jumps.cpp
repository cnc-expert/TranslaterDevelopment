#include "translator.h"


using namespace std;

extern "C" void ProcessJumpBlock() {
	
	auto JumpBlock = FindFrontTypedBlock(TB_JUMP);		
		
		while ( JumpBlock != programFanuc.end() ) {
			
			Block* JumpBlockObject = *JumpBlock;
			
			if ( MatchLabelAndNumberOfBlock.find( JumpBlockObject->label ) == MatchLabelAndNumberOfBlock.end() ) {

				Block* NBlock = new Block();
				NBlock->translatedBlock = new string(string("N") +
													 to_string(++MaximalNumberOfBlock) +
													 " (" + JumpBlockObject->label + ")");
				
				auto firstLabelBlock = FindLabeledBlock( JumpBlockObject->label );
				programFanuc.insert(firstLabelBlock, NBlock);
				
				MatchLabelAndNumberOfBlock[ JumpBlockObject->label ] = MaximalNumberOfBlock;
			}
			
			*JumpBlockObject->translatedBlock += "GOTO" + to_string( MatchLabelAndNumberOfBlock[ JumpBlockObject->label ] );
			
			JumpBlockObject->type = TB_ORDINARY;
			
			JumpBlock = FindFrontTypedBlock(TB_JUMP);
			
		}	
}

extern "C" void* CreateBNCBlock(char* label) {
	
	Block* blockObject = new Block();
	blockObject->translatedBlock = new string("");

	blockObject->label = label;
	blockObject->type = TB_JUMP;

	deque<Block*> *programFanuc = new deque<Block*>();

	programFanuc->push_back(blockObject);

	return programFanuc;
}

extern "C" void* CreateJumpBlock(char* condition, char* arg1, char* arg2, char* label) {
	
	Block* blockObject = new Block();
	blockObject->translatedBlock = 
		new string("IF [" + IndetifyVariableOrNumber(arg1) + " " + condition + " " + IndetifyVariableOrNumber(arg2) + "] ");

	blockObject->label = label;
	blockObject->type = TB_JUMP;

	deque<Block*> *programFanuc = new deque<Block*>();

	programFanuc->push_back(blockObject);

	return programFanuc;
}