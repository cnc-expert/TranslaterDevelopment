/*
 * Fanuc variables handling.
 */

#include <set>
#include <map>



using namespace std;



static set<int> CreateEmptyVariablesIndexTable() {

	set<int> emptyVariablesTableTmp;

	for (int i = 100; i <= 199; i++) {
		emptyVariablesTableTmp.insert(i);
	}

	for (int i = 500; i <= 999; i++) {
		emptyVariablesTableTmp.insert(i);
	}

	return emptyVariablesTableTmp;
}



// Table containing only unused Fanuc variables.
static set<int> EmptyVariablesIndexTable = CreateEmptyVariablesIndexTable();



int getUnusedFanucVariable() {
	
	int unusedVariable = *EmptyVariablesIndexTable.begin();
	EmptyVariablesIndexTable.erase(EmptyVariablesIndexTable.begin());

	return unusedVariable;
}



int MatchinFanucVariableToNC(char* variableNC) {

	static map<char*, int> UsedVariableTable;

	if ( UsedVariableTable.find(variableNC) == UsedVariableTable.end() ) {

		UsedVariableTable[variableNC] = getUnusedFanucVariable();
	}

	return UsedVariableTable[variableNC];
}

