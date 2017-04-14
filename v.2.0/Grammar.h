#pragma once
#include "Common.h"
#include "Rule.h"

namespace tinyYACC {
	class Grammar
	{
		typedef set<symbol> symbolSet;
	public:
		class iterator
		{
		public:
			friend class Grammar;
			iterator() {}

			/* ������֮����бȽ� */
			bool operator==(const iterator& rhs) const {
				return it_ == rhs.it_;
			}
			bool operator!=(const iterator& rhs) const {
				return it_ != rhs.it_;
			}
			rulePtr& operator*() {
				/* �����б�Ҫ˵��һ��,it_�ǵ���������, *it_��rulePtr����, **it_��Rule���� */
				return *it_;
			}
			iterator& operator++ () {
				it_++;
				return *this;
			}
			iterator& operator--() {
				--it_;
				return *this;
			}
		private:
			vector<rulePtr>::iterator it_;
			iterator(vector<rulePtr>::iterator& it) :
				it_(it)
			{}
		};
	public:
		Grammar(symbol);
		~Grammar();
	private:
		Grammar() {}
	public:
		int start_;										// ��ʼ���� 
	private:
		set<symbol> symbols_;							// ���ڼ�¼���еķ���,�����ս���ͷ��ս��
		map<symbol, symbolSet> follow_;					// ���ڼ�¼���е�follow����
		map<symbol, symbolSet> first_;					// ���ڼ�¼���е�first����
		set<symbol> nullable_;							// ���ڼ�¼���ս���Ƿ��ܹ������մ�
		vector<rulePtr> rules_;							// ���ڼ�¼����
	public:
		shared_ptr<vector<rulePtr>> findRules(int l);	// ͨ��l,Ѱ����lΪ��ͷ��rule
		friend wostream& operator<< (wostream& os, const Grammar& g);
		void appendRule(const rulePtr& r) {
			rules_.push_back(r);
		}
		iterator begin() {
			return iterator(rules_.begin());
		}
		iterator end() {
			return iterator(rules_.end());
		}

		const vector<rulePtr>& getAllRules() {
			return rules_;
		}

		void calculateSets();
		void printSets() const;

		shared_ptr<Rule> startRule() {
			return (*findRules(start_))[0];
		}

		const set<int>& follow(symbol s) {
			assert(follow_.find(s) != follow_.end());
			return follow_[s];
		}

		const set<int>& first(symbol s) {
			assert(first_.find(s) != first_.end());
			return first_[s];
		}

	private:
		void getAllSymbols();
		static bool updateSet(set<symbol>& t, const set<symbol>& s);
		static bool updateSet(set<symbol>& t, symbol s);
	};
}

