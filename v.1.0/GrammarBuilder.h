#pragma once
#include "Common.h"

struct RuleDef;

//
// buildGrammar ���ڹ�����Grammar.
//
grammarPtr buildGrammar(wstring&, set<wstring>&, set<wstring>&, vector<RuleDef>&);

