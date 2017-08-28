#pragma once
class Grammar;
#include "Status.h"
#include "Rule.h"
#include "SymbolTable.h"


namespace tinyYACC {
	//
	// LALRParser��֮ǰ��LR(1)������ͬ,��Ϊ���и��Ӽ���Ч��ʵ�ַ���.
	//
	class LALRParser
	{
	public:
		LALRParser(Grammar&);
		~LALRParser();
	public:
		struct Action {
			enum action { shift, reduce };  // һ�����ֶ���,һ�����ƽ� shift,һ���ǹ�Լreduce
			action act;
			size_t index;					// ���ڴ洢״̬status
			
			Action(const action act, size_t index) :
				act(act), index(index)
			{}

			Action() {}

			Action(const Action& a) {
				if (&a == this) return;
				act = a.act;
				index = a.index;
			}
		};
	private:
		Grammar& g_;
		map<size_t, statusPtr> status_;
		// ʹ��vector���洢�ƽ�ʱʹ�õĹ���
		vector<rulePtr> reducePool_;
		map<size_t, map<int, Action>> table_; // size_t���״̬, int��Ƿ���, Action��Ƕ���
	public:
		size_t start_;		// ���ڼ�¼��ʼ״̬���±�
	public:
		void printStats();
		void computerLookAhead();
		Action queryAction(size_t state, int sym) {
			assert(table_.find(state) != table_.end());
			if (table_[state].find(sym) == table_[state].end()) {
				GeneralError error;
				error.msg = L"�޷�Ϊ" + g_symbolTable.queryType(sym) + L"��ѯ��Action!";
				throw error;
			}
			return table_[state][sym];
		}

		rulePtr queryRule(size_t idx) {
			assert(idx < reducePool_.size());
			return reducePool_[idx];
		}

	private:
		Action makeAction(Action::action, size_t);
		Action makeAction(Action::action, rulePtr&);
		static size_t appendAction(map<int, list<Action>>&, int sym, Action&);
		size_t expandRule(list<Item>&, bool&);
		shared_ptr<map<int, list<Item>>> classify(statusPtr& s, list<Item>&);
	};
}

