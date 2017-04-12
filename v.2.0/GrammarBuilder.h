#pragma once
#include "Common.h"

namespace tinyYACC {
	struct RuleDef;
	class SymbolTable;
	//
	// buildGrammar ���ڹ�����Grammar.
	//
	void buildSymbolTable(set<wstring>& terminal, set<wstring>& nonTerminal);
	grammarPtr buildGrammar(wstring& start, set<wstring>& terminal, set<wstring>& nonTerminal, vector<RuleDef>& rules);
}

