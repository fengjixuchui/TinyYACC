#include "LALRParser.h"
#include "Grammar.h"
#include "Rule.h"

namespace tinyYACC {
	LALRParser::LALRParser(Grammar &g) :
		g_(g)
	{
	}

	LALRParser::~LALRParser()
	{
	}

	//
	// computerLookAhead ���ڼ���ǰ������,ͬʱ����״̬.
	//
	void LALRParser::computerLookAhead()
	{
		// ���ȴ��ķ���start���ų���,����һ��stat
		g_.calculateSets(); // �������з��ŵ�first��, folllow��,�Լ�nullable��
		list<Item> its;
		bool exists = false;				// ���ڼ�¼״̬�Ƿ���ʹ�����
		queue<size_t> notHandled;			// ���ڴ洢��Щ��δ�������״̬�ı�־

		its.push_back(Item(g_.startRule(), 0));
		start_ = expandRule(its, exists);	// �õ���һ��״̬
		notHandled.push(start_);

		while (!notHandled.empty()) {
			// ȡ��һ����δ���������stat
			size_t stat = notHandled.front(); notHandled.pop();
			shared_ptr<map<symbol, list<Action>>> lookahead = make_shared<map<symbol, list<Action>>>();
			list<Item> satisfied; // satisfied���ڼ�¼stat�п��Թ�Լ��item

			// ��״̬���Item���з���,�������Թ�Լ����,�Լ�ͨ��ĳ��Item�е�next��Ӧ��symbol����ͬ����
			// ����ÿ��״̬�ı��,��status_��ȡ��״̬(status_[stat])
			auto group = classify(status_[stat], satisfied);

			for (auto it : satisfied) {
				// ����ʹ���������Լ
				for (auto sym : g_.follow(it.getRule()->origin())) {
					appendAction(*lookahead, sym, makeAction(Action::reduce, it.getRule()));
				}
			}
			// ���������ڸ��ַ��Ž��б���
			for (auto &pr : *group) {
				int sym = pr.first;
				list<Item>& its = pr.second;

				for (auto &it : its) {
					// ������Ա�֤����Խ��
					//assert(it.advance() != false);
					it.advance();
				}
				size_t stat = expandRule(its, exists); // ������չ״̬

				// ��ǰ�����ڹ�,Ҳ�����¼���Ľڵ�,��Ҫ����
				if (!exists) notHandled.push(stat);

				// appendAction����action������Ǹ�����ĳ���,������Ȳ�Ϊ1,��ʾ������������Ҫִ��
				// ����Ȼ�����˴���
				if (appendAction(*lookahead, sym, makeAction(Action::shift, stat)) != 1) {
					GeneralError error;
					error.msg = L"�ƽ���Լ��ͻ : " + g_symbolTable[sym] + L"!";
					throw error;
				}
			}

			// �������������Ļ�,��������д��table��
			table_[stat] = map<int, Action>();
			for (auto &pr : *lookahead) {
				int sym = pr.first;
				table_[stat][sym] = *pr.second.begin();
			}
		}
		// �����Parsing table֮��,status_û�����κ��ô�
		status_.clear();
	}

	//
	// expandRule ������չ״̬,������һ��״̬�ı��,������״̬֮ǰ�Ѿ����ڹ���,exists
	// �趨Ϊtrue,�����趨Ϊfalse
	// 
	size_t LALRParser::expandRule(list<Item>& items, bool& exists)
	{
		set<int> visited;				// ���ڼ�¼�Ѿ����ʹ��Ľڵ�
		statusPtr stat = make_shared<Status>();
		int sym;

		for (auto &it : items) {
			stat->insertItem(it);
		}

		// ����֮����ѡ��list��Ϊ����,��Ϊ��ʹ��list�ĺ������Ԫ��,���ĵ�����Ҳ����ʧЧ
		for (auto &it : items) {
			if (it.next(sym) && !g_symbolTable.isTerminal(sym)) { // ��ʼ��չ
				if (visited.find(sym) == visited.end()) {
					visited.insert(sym);
					auto rules = g_.findRules(sym);
					for (auto &r : *rules) {
						auto newItem = Item(r, 0);
						stat->insertItem(newItem);
						items.push_back(newItem);
					}
				}
			}
		}
		// tofix: ��Ȼ���ʺ�С��С,����hash�����п��ܻ��ͻ
		// ��û�и��õİ취�������ظ���״̬��?
		size_t h = StatusHash{}(*stat);		// ����hashֵ
		if (status_.find(h) == status_.end()) { // ��¼����״̬
			status_[h] = stat;
			exists = false;
		}
		else exists = true;
		//wcout << *stat << endl;
		// ����ֻ��Ҫ����һ��Ψһ��hashֵ����,�����������stat
		return h;
	}

	//
	// classify ���ڽ�status�е�һЩ���Թ�Լ��item����lst��,���ڲ����Թ�Լ����
	// ��item����һ��Ҫ�������ַ���Ϊkey,��key��ͬ��item�Ƿ���һ��list��,��Ϊvalue
	// ������һ���ֵ䷵�ػ���
	// 
	shared_ptr<map<int, list<Item>>> LALRParser::classify(statusPtr& s, list<Item>& lst)
	{
		shared_ptr<map<symbol, list<Item>>> res = make_shared<map<symbol, list<Item>>>();
		for (auto &it : *s->items) {
			// �ɹ�Լ���
			int next;
			if (it.next(next)) {
				if (res->find(next) != res->end()) {
					(*res)[next].push_back(it); 
				}
				else {
					(*res)[next] = list<Item>();
					(*res)[next].push_back(it);
				}
			}
			else { // it���item���Թ�Լ��(ֻ�г�����������E -> abC����������ſ��ܷ���false)
				lst.push_back(it);
			}
		}
		return res;
	}

	//
	// printStats ��������������Status_��.
	//
	void LALRParser::printStats()
	{
		for (auto &pr : status_) {
			wcout << L"lable :" << pr.first << endl;
			wcout << (*pr.second) << endl;
		}
	}

	LALRParser::Action LALRParser::makeAction(Action::action act, size_t target)
	{
		assert(act == Action::shift);
		return Action(Action::shift, target);
	}

	// 
	// makeAction ʹ�ù���r���й�Լ
	// 
	LALRParser::Action LALRParser::makeAction(Action::action act, rulePtr& r)
	{
		assert(act == Action::reduce);
		reducePool_.push_back(r);
		return Action(Action::reduce, reducePool_.size() - 1);
	}

	//
	// appendAction ������Ӧ��ִ�еĲ�����ӵ�list��,���շ���list�ĳ���.
	// 
	size_t LALRParser::appendAction(map<int, list<Action>>& lookahead, int sym, Action &act)
	{
		if (lookahead.find(sym) != lookahead.end()) {
			lookahead[sym].push_back(act);
			return lookahead[sym].size(); 
		}
		lookahead[sym] = list<Action>();
		lookahead[sym].push_back(act);
		return 1;
	}
}