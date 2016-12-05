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
	
	auto lastRptBlockIter = FindLastTypedBlock(TB_RPT);
		
	while ( lastRptBlockIter != programFanuc.end() ) {
		
		int numberOfBlock = ++MaximalNumberOfBlock;
		int variableNumber = getUnusedFanucVariable();
		
		RptBlock *lastRptBlock = (RptBlock*)*lastRptBlockIter;
		lastRptBlock->type = TB_ORDINARY;
		*lastRptBlock->translatedBlock = "N" + to_string(numberOfBlock);
		
		Block *counterInitBlock = new Block();
		counterInitBlock->indentation = lastRptBlock->indentation;
		counterInitBlock->translatedBlock = new string("#" + to_string(variableNumber) + "=");
		*counterInitBlock->translatedBlock += IndetifyVariableOrNumber(lastRptBlock->counter);
					
		auto ErpBlockIter = programFanuc.insert(lastRptBlockIter, counterInitBlock);

		while ( (*ErpBlockIter)->type != TB_ERP /*&& ErpBlockIter != programFanuc.end()*/ ) // possible error
			ErpBlockIter ++;

		(*ErpBlockIter)->type = TB_ORDINARY;
		*(*ErpBlockIter)->translatedBlock = "IF [#" + to_string(variableNumber) + " GT 0] GOTO" + to_string(numberOfBlock);
		
		Block *decrementBlock = new Block();
		decrementBlock->indentation = (*(ErpBlockIter-1))->indentation;
		decrementBlock->translatedBlock = new string( "#" + to_string(variableNumber) + "=" +
													  "#" + to_string(variableNumber) + "-1" );
		
		programFanuc.insert(ErpBlockIter, decrementBlock);
		
		lastRptBlockIter = FindLastTypedBlock(TB_RPT);
		
	}
	
}

