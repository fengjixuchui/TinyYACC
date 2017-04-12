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
		g_.calculateSets();
		list<Item> its;
		bool exists = false;				// ���ڼ�¼״̬�Ƿ���ʹ�����
		queue<size_t> notHandled;			// ���ڴ洢��Щ��δ�������״̬�ı�־

		its.push_back(Item(g_.startRule(), 0));
		start_ = expandRule(its, exists);	// �õ���һ��״̬
		notHandled.push(start_);

		while (!notHandled.empty()) {
			// ȡ��һ����δ���������stat
			size_t stat = notHandled.front(); notHandled.pop();
			shared_ptr<map<symbol, shared_ptr<list<Action>>>> lookahead = make_shared<map<symbol, shared_ptr<list<Action>>>>();
			list<Item> satisfied;

			// ��״̬���Item���з���,�������Թ�Լ����,�Լ�ͨ��ĳ��Item�е�next��Ӧ��symbol����ͬ����
			auto group = classify(status_[stat], satisfied);

			for (auto it : satisfied) {
				// ����ʹ���������Լ
				for (auto sym : g_.follow(it.getRule()->origin())) {
					appendAction(*lookahead, sym, makeAction(Action::reduce, it.getRule()));
				}
			}
			// ���������ڸ��ַ��Ž��б���
			for (auto pr : *group) {
				int sym = pr.first;
				shared_ptr<list<Item>> its = pr.second;

				for (auto i = its->begin(); i != its->end(); ++i) {
					// ������Ա�֤����Խ��
					assert((*i).advance() != false);
				}
				size_t stat = expandRule(*its, exists); // ������չ״̬

				// ��ǰ�����ڹ�,Ҳ�����¼���Ľڵ�,��Ҫ����
				if (!exists) {
					notHandled.push(stat);
				}

				if (appendAction(*lookahead, sym, makeAction(Action::shift, stat)) != 1) {
					GeneralError error;
					error.msg = L"�ƽ���Լ��ͻ : " + g_symbolTable[sym] + L"!";
					throw error;
				}
			}

			// �������������Ļ�,��������д��table��
			table_[stat] = map<int, Action>();
			for (auto pr : *lookahead) {
				int sym = pr.first;
				table_[stat][sym] = *pr.second->begin();
			}
		}
		// �����Parsing table֮��,status_û�����κ��ô�
		status_.clear();
	}

	size_t LALRParser::expandRule(list<Item>& items, bool& exists)
	{
		set<int> visited;				// ���ڼ�¼�Ѿ����ʹ��Ľڵ�
		statusPtr stat = make_shared<Status>();
		int sym;

		for (auto it : items) {
			stat->insertItem(it);
		}

		// ����֮����ѡ��list��Ϊ����,��Ϊ��ʹ��list�ĺ������Ԫ��,���ĵ�����Ҳ����ʧЧ
		for (auto it : items) {
			if (it.next(sym) && !g_symbolTable.isTerminal(sym)) { // ��ʼ��չ
				if (visited.find(sym) == visited.end()) {
					visited.insert(sym);
					auto rules = g_.findRules(sym);
					for (auto r : *rules) {
						auto newItem = Item(r, 0);
						stat->insertItem(newItem);
						items.push_back(newItem);
					}
				}
			}
		}
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


	shared_ptr<map<int, shared_ptr<list<Item>>>> LALRParser::classify(statusPtr& s, list<Item>& lst)
	{
		shared_ptr<map<symbol, shared_ptr<list<Item>>>> res = make_shared<map<symbol, shared_ptr<list<Item>>>>();
		for (auto it : *s->items) {
			// �ɹ�Լ���
			int next;
			if (it.next(next)) {
				if (res->find(next) != res->end()) {
					(*res)[next]->push_back(it);
				}
				else {
					(*res)[next] = make_shared<list<Item>>();
					(*res)[next]->push_back(it);
				}
			}
			else {
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
		for (auto pr : status_) {
			wcout << L"lable :" << pr.first << endl;
			wcout << (*pr.second) << endl;
		}
	}

	LALRParser::Action LALRParser::makeAction(Action::action act, size_t target)
	{
		assert(act == Action::shift);
		return Action(Action::shift, target);
	}

	LALRParser::Action LALRParser::makeAction(Action::action act, rulePtr& r)
	{
		assert(act == Action::reduce);
		reducePool_.push_back(r);
		size_t len = reducePool_.size() - 1;
		return Action(Action::reduce, reducePool_.size() - 1);
	}

	size_t LALRParser::appendAction(map<int, shared_ptr<list<Action>>>& lookahead, int sym, Action &act)
	{
		if (lookahead.find(sym) != lookahead.end()) {
			lookahead[sym]->push_back(act);
			return lookahead[sym]->size();
		}
		lookahead[sym] = make_shared<list<Action>>();
		lookahead[sym]->push_back(act);
		return 1;
	}
}