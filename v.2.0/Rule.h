#pragma once
#include "Common.h"
#include <functional>

namespace tinyYACC {
	//
	// Rule����Ҫ���ڼ�¼�﷨,����˵�ķ�.
	//
	class Rule
	{
	public:
		Rule(symbol start);
		~Rule();
	public:
		bool isEpsRule() const {				 // �����ж����������Ƿ��ǦŹ���
			return expansion_.size() == 0;
		}

		bool originEqualTo(symbol s) const {		// �ж�rule������Ƿ���s
			return s == origin_;
		}

		int findNthElem(size_t n) const {		// Ѱ��rule�Ƶ�ʽ�Ҳ�ĵ�N��Ԫ��
			assert(n < expansion_.size());
			return expansion_[n];
		}

		const vector<int>& expansion() const {
			return expansion_;
		}

		vector<symbol> subExpansion(size_t from, size_t len) const {
			auto start = expansion_.begin();
			return vector<symbol>(start + from, start + from + len);
		}

		shared_ptr<set<symbol>> subExpansionSymbols(size_t from, size_t len) const {
			auto start = expansion_.begin();
			return make_shared<set<symbol>>(start + from, start + from + len);
		}

		size_t expansionLength() const {  // ��ȡ�����Ҳ�ʽ�ӵĳ���
			return expansion_.size();
		}

		int origin() const {		// ��ȡ�������ķ��ս��
			return origin_;
		}


		void expansionAppend(int s) {
			expansion_.push_back(s);
		}

	public:
		bool operator==(const Rule& r);
		friend wostream& operator<<(wostream& os, const Rule& r);
	private:
		int origin_;					// ��߲���
		vector<int> expansion_;			// չ������
	};


	//
	// Item ���ڼ�¼һЩ״̬,˵ʵ��,Item�Ѿ��ǳ�������һ��ָ��,��ֻ�Ǽ�¼��rule�ĵ�ַ,Ȼ�󸽴���һ��pos,���Ը��ƵĿ���Ҳ������.
	//
	class Item
	{
	public:
		Item(const rulePtr&, int);
		~Item();

	private:
		rulePtr rule_;		// ���ڼ�¼�Ƶ�����
		int pos_;			// ���ڼ�¼Item�����Ѿ���������rule��expansions����һ����������

	public:
		friend wostream& operator<<(wostream& os, const Item &it);

		bool operator==(const Item& r) const {		// ���ڱȽ�����Item�Ƿ����
			return (rule_ == r.rule_) && (pos_ == r.pos_);
		}

		int getPos() const {
			return pos_;
		}

		//
		// ÿ��item����һ��hashֵ,���item��ȵĻ�,���ǵ�hashֵҲ���.
		//
		size_t hash() const {
			size_t h1 = std::hash<shared_ptr<Rule>>{}(rule_);
			size_t h2 = std::hash<int>{}(pos_);
			return h1 ^ (h2 << 1);
		}

		rulePtr getRule() {
			return rule_;
		}

		//
		// isSatisfied �����ж�pos�Ƿ���rule�����һ��λ��,Ҳ�����ж��ܷ��Լ��.
		//
		bool isSatisfied() const {
			return pos_ == rule_->expansionLength();
		}

		//
		// next��ȡrule_��Expansion��Բ�����ķ���,����s��,�ɹ�����true,���򷵻�false
		// �ٸ�����: E -> ab��C ,��C��Ӧ�ı�Ǽ���s��.
		//
		bool next(int& s) const {
			if (!isSatisfied()) {
				s = rule_->findNthElem(pos_);
				return true;
			}
			else
				return false;
		}

		//
		// advance��pos_ǰ��һ��,�޷�ǰ��ʱ,����false,���򷵻�true
		//
		bool advance() {
			if (!isSatisfied()) {
				pos_++;
				return true;
			}
			else
				return false;
		}

		bool operator<(const Item& r) const {	 // ������������ò���,���ǻ��Ƿ��������
			if (rule_ != r.rule_)
				return rule_ < r.rule_;
			else
				return pos_ < r.pos_;
		}

		Item(const Item& r) {
			this->rule_ = r.rule_;
			this->pos_ = r.pos_;
		}

	};
	typedef shared_ptr<Item> itemPtr;
}

