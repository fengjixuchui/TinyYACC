#pragma once
#include "Common.h"

//
// GrammarBuilder ���ڹ�����Grammar.
//
class GrammarBuilder
{
public:
	GrammarBuilder();
	~GrammarBuilder();
public:
	grammarPtr buildGrammar(set<wstring>&, set<wstring>&, vector<RuleDef>&);
};

