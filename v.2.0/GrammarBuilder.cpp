#include "GrammarBuilder.h"
#include "Defs.h"
#include "Rule.h"
#include "Grammar.h"
#include "SymbolTable.h"
#include "LALRParserTable.h"
#include "GrammarLoader.h"

namespace tinyYACC {
	//
	// buildSymbolTable ����һ��Symbol Table.
	//
	void buildSymbolTable(set<wstring>& terminal, set<wstring>& nonTerminal)
	{
		g_symbolTable.clear();
		for (auto it = terminal.begin(); it != terminal.end(); ++it) {
			g_symbolTable.appendType(*it, true);
		}
		for (auto it = nonTerminal.begin(); it != nonTerminal.end(); ++it) {
			g_symbolTable.appendType(*it, false);
		}
	}
	
	
	grammarPtr buildGrammar(wstring& s, set<wstring>& terminal, set<wstring>& nonTerminal, vector<RuleDef>& rules) {
		// ���ȹ��������еķ���, ����g_typeMapping�����.
		// ��һ���ķ��������ķ�,��ν�����ķ�,�������Լ���ӵ�һ���ķ�.��ӵ��ķ���Ӱ��ԭ���ķ�,���ǻ�����Ǻ����Ĵ����������.
		rulePtr r0 = make_shared<Rule>(g_symbolTable.startMark);
		r0->expansionAppend(g_symbolTable[s]);
		r0->expansionAppend(g_symbolTable.endMark);
		grammarPtr g = make_shared<Grammar>(g_symbolTable.startMark); // start��������������Ŀ�ʼ����
		g->appendRule(r0);
		// �����ķ�
		for (auto it = rules.begin(); it != rules.end(); ++it) {
			rulePtr r = make_shared<Rule>(g_symbolTable[it->origin]);
			for (auto elem : it->expansions) {
				r->expansionAppend(g_symbolTable[elem]);
			}
			g->appendRule(r);
		}
		return g;
	}

	//
	// buildParsingTable �����Parsing Table.
	//
	shared_ptr<LALRParserTable> buildParsingTable(CollectDefsAndRules& coll) {
		buildSymbolTable(coll.terminal_, coll.nonTerminal_);
		auto grammar = buildGrammar(coll.start_[0], coll.terminal_, coll.nonTerminal_, coll.rules_);
		shared_ptr<LALRParserTable> table = make_shared<LALRParserTable>(*grammar);
		table->computerLookAhead();
		return table;
	}
}
