#include "GrammarBuilder.h"
#include "Defs.h"
#include "Symbol.h"
#include "Rule.h"
#include "Grammar.h"

grammarPtr buildGrammar(wstring& s, set<wstring>& terminal, set<wstring>& nonTerminal, vector<RuleDef>& rules) {
	map<wstring, symbolPtr> syms;
	// ���ȹ��������еķ���
	symbolPtr start = make_shared<Symbol>(Symbol::NonTerminal, L"#start");
	symbolPtr end = make_shared<Symbol>(Symbol::Terminal, L"#end");
	for (auto it = terminal.begin(); it != terminal.end(); ++it) {
		syms[*it] = make_shared<Symbol>(Symbol::Terminal, *it);
	}
	for (auto it = nonTerminal.begin(); it != nonTerminal.end(); ++it) {
		syms[*it] = make_shared<Symbol>(Symbol::NonTerminal, *it);
	}
	// ��һ���ķ��������ķ�,��ν�����ķ�,�������Լ���ӵ�һ���ķ�.��ӵ��ķ���Ӱ��ԭ���ķ�,���ǻ�����Ǻ����Ĵ����������.
	rulePtr r0 = make_shared<Rule>(start);
	r0->expansionAppend(syms[s]);
	r0->expansionAppend(end);
	grammarPtr g = make_shared<Grammar>(start); // start��������������Ŀ�ʼ����
	g->appendRule(r0);
	// �����ķ�
	for (auto it = rules.begin(); it != rules.end(); ++it) {
		rulePtr r = make_shared<Rule>(syms[it->origin]);
		for (auto elem : it->expansions) {
			r->expansionAppend(syms[elem]);
		}
		g->appendRule(r);
	}
	return g;
}
