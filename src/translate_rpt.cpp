#include "translator.h"
#include <stack>

using namespace std;

static stack< pair<int, int> > VariablesAndBlockNumbers;

static deque<Block*>::iterator FindLastTypedBlock(enum typeOfBlock type){
	for(auto curBlock = programFanuc.end()-1; curBlock != programFanuc.begin(); curBlock-- ){
		
		if( (*curBlock)->type == type ) {
			return curBlock;
		}
	}
	
 	if( ( *programFanuc.begin() )->type == type ) {
			return programFanuc.begin();
	}
		
	return programFanuc.end();
}

extern "C" void* CreateERPDeque() {
	
	deque<Block*>* dequeTmp = new deque<Block*>();
	
	Block *blockTmp = new Block();
	blockTmp->type = TB_ERP;
	blockTmp->translatedBlock = new string("ErpBlock");
	
	dequeTmp->push_back(blockTmp);
	
	return dequeTmp;
	
}

extern "C" void* CreateRPTDeque(char* counter) {
	
	deque<Block*>* dequeTmp = new deque<Block*>();
	
	RptBlock *blockTmp = new RptBlock();
	blockTmp->type = TB_RPT;
	blockTmp->counter = counter;
	blockTmp->translatedBlock = new string("RptBlock");
	
	dequeTmp->push_back(blockTmp);
	
	return dequeTmp;
	
}

extern "C" void ProcessRptBlock() {
	
	auto lastRptBlock = FindLastTypedBlock(TB_RPT);
		
		while ( lastRptBlock != programFanuc.end() ) {
			
			int numberOfBlock = ++MaximalNumberOfBlock;
			int variableNumber = getUnusedFanucVariable();
			
			(*lastRptBlock)->type = TB_ORDINARY;
			*(*lastRptBlock)->translatedBlock = "N" + to_string(numberOfBlock);
			
			Block *blockTmp = new Block();
			blockTmp->translatedBlock = new string("#" + to_string(variableNumber) + "=");
			char* counter = ( (RptBlock*)(*lastRptBlock) )->counter;
			
			*blockTmp->translatedBlock += IndetifyVariableOrNumber(counter);
						
			auto ErpBlock = programFanuc.insert(lastRptBlock, blockTmp);
			
			while ( (*ErpBlock)->type != TB_ERP /*&& ErpBlock != programFanuc.end()*/ ) // possible error
				ErpBlock ++;
			
			*(*ErpBlock)->translatedBlock = "IF [#" + to_string(variableNumber) + " GT 0] GOTO" + to_string(numberOfBlock);
			(*ErpBlock)->type = TB_ORDINARY;
			
			Block *decrementBlock = new Block();
			decrementBlock->translatedBlock = 
				new string( "#" + to_string(variableNumber) + "=#" + to_string(variableNumber) + "-1" );
			
			programFanuc.insert(ErpBlock, decrementBlock);
			
			lastRptBlock = FindLastTypedBlock(TB_RPT);
			
			
		}
		
}

