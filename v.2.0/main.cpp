#include "Common.h"
#include "Grammar.h"
#include "Rule.h"
#include "GrammarLoader.h"
#include "Lexer.h"
#include "LALRParserTable.h"
#include "GrammarBuilder.h"
#include <regex>
using namespace std;
using namespace tinyYACC;


int main() {
	locale loc("chs");
	wcout.imbue(loc);
	GrammarLoader loader;
	// nd�Ǹ����ķ�1.df���ɵ�ast.������ɳɹ�,nd��Ϊ��,������ӡ������ԭ��,ndΪ��.
	grammarNodePtr nd = parseGrammar(loader, L"1.df");
	if (nd) {
		// printTree�����ɵ�ast�����graphviz�ű�,������graphviz�鿴.
		printTree(*nd);
		// collectDefsAndules��nd���Ѽ�Rule,token�Ķ���,������Щ��Ϣ,���ǿ�����������ķ���Parsing Table.
		// ����Parser,����Ҫ�Լ�д,���ڱ�����Ĺ��ܷ�Χ֮��.
		collectDefsAndRules(*nd);
	}
	getchar();
	return 0;
}