#pragma once
#include "Common.h"

//
// TokenDef�ǹ���Token�Ķ���.
//
struct TokenDef
{
	TokenDef(const wstring& name, const wstring& pattern) :
		mark(name), pattern(pattern)
	{}
	TokenDef() {}
	friend wostream& operator<<(wostream& os, TokenDef& tk) {
		os << tk.mark << L"	:" << tk.pattern;
		return os;
	}
	wstring mark;		// ��¼���Token������
	wstring pattern;	// ��¼��Ӧ��������ʽ
};

//
// RuleDef�ǹ��ڹ���Ķ���.
//
struct RuleDef {
	wstring origin;
	vector<wstring> expansions;
	friend wostream& operator<<(wostream& os, RuleDef& r) {
		os << r.origin << L"	:";
		for (auto exp : r.expansions) {
			os << L" " << exp;
		}
		return os;
	}
};


