#include "nc.tab.h"
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <deque>
#include <algorithm>

using namespace std;

enum typeOfBlock {TB_EPP, TB_ORDINARY};

int CounterOfBlocks = 0; // counter of the temporary blocks' numbers
int MaximalNumberOfBlock = 0;

// map: lable -> temporary block number
map<char*, int> LabledBlocksTable;

map<char*, int> UsedVariableTable; // table containing the variables in use (type "set")


class Block {
	public:
		string* translatedBlock;
		int numberOfBlock;
		int numberOfBlockGoTo;
		enum typeOfBlock type;
	
	Block() {
		numberOfBlock = -1;
		numberOfBlockGoTo = -1;
		type = TB_ORDINARY;
	}
};

class EppBlock : public Block{
	public:
		char* labelOne;
		char* labelTwo;

	EppBlock() {
		numberOfBlock = CounterOfBlocks++;
		numberOfBlockGoTo = -1;
		type = TB_EPP;
	}
};

deque<Block> programFanuc;

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

// Fanuc Variable Range
// #100...199
// #500...999