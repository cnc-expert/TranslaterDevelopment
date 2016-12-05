#include "translator.h"


using namespace std;


class JmpBlock : public Block {
	public:
		char* label_to_go;
};



extern "C" void ProcessJumpBlock() {
	
	auto JumpBlock = FindFrontTypedBlock(TB_JUMP);		
		
	while ( JumpBlock != programFanuc.end() ) {
		
		JmpBlock* JumpBlockObject = (JmpBlock *) *JumpBlock;
		
		if ( MatchLabelAndNumberOfBlock.find( JumpBlockObject->label_to_go ) == MatchLabelAndNumberOfBlock.end() ) {

			auto firstLabelBlock = FindLabeledBlock( JumpBlockObject->label_to_go );

			Block* NBlock = new Block();
			NBlock->indentation = (*firstLabelBlock)->indentation;
			NBlock->translatedBlock = new string(string("N") +
			                                     to_string(++MaximalNumberOfBlock) );
			                                 //  + " (" + JumpBlockObject->label_to_go + ")");

			programFanuc.insert(firstLabelBlock, NBlock);
			
			MatchLabelAndNumberOfBlock[ JumpBlockObject->label_to_go ] = MaximalNumberOfBlock;
		}
		
		*JumpBlockObject->translatedBlock += "GOTO" +
		                                     to_string( MatchLabelAndNumberOfBlock[ JumpBlockObject->label_to_go ] ) +
											 " (" + JumpBlockObject->label_to_go + ")";
		JumpBlockObject->type = TB_ORDINARY;
		
		JumpBlock = FindFrontTypedBlock(TB_JUMP);
	}	
}


extern "C" void* CreateBNCBlock(char* label) {
	
	JmpBlock* blockObject = new JmpBlock();
	blockObject->translatedBlock = new string("");

	blockObject->label_to_go = label;
	blockObject->type = TB_JUMP;

	deque<Block*> *programFanuc = new deque<Block*>();
	programFanuc->push_back(blockObject);

	return programFanuc;
}


extern "C" void* CreateJumpBlock(char* condition, char* arg1, char* arg2, char* label) {
	
	JmpBlock* blockObject = new JmpBlock();
	blockObject->translatedBlock = 
		new string("IF [" + IndetifyVariableOrNumber(arg1) + " " + condition + " " + IndetifyVariableOrNumber(arg2) + "] ");

	blockObject->label_to_go = label;
	blockObject->type = TB_JUMP;

	deque<Block*> *programFanuc = new deque<Block*>();
	programFanuc->push_back(blockObject);

	return programFanuc;
}
