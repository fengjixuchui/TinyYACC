#pragma once
#include "Common.h"
#include <regex>
#include "SymbolTable.h"

namespace tinyYACC {
	struct TokenDef;

	struct Token {
		int mark;
		size_t line;
		wstring content;
		Token(int mark, size_t line, const wstring& content) :
			mark(mark), line(line) , content(content)
		{}
		// ����������캯����ĳЩʱ��Ҳ���õõ���!
		Token(int mark) :
			mark(mark), line(0)
		{}
		friend wostream& operator<<(wostream& os, const Token& tk);
	};

	//
	// Lexer��һ���ʷ�������.
	//
	class Lexer
	{
	public:
		Lexer(const vector<TokenDef>&, const vector<wstring>&);
		~Lexer();
	private:
		vector<wregex> regexs_;
		vector<int> marks_;
		set<int> ignore_;
		wstring stream_;
		size_t offset_;					// ���ڼ�¼�Ѿ��������˵�λ��
		size_t line_;					// ��¼�ı����ڵ���,��Ҳ�ǳ���Ҫ
		list<Token> cache_;
	public:
		void setStream(const wstring& stream) {
			offset_ = 0;
			line_ = 1;
			stream_ = stream;
		}
		Token next();
		Token peek();
	private: 
		int countNL(const wstring&);
	};

	void checkTokens(const vector<TokenDef>& tokens);
}