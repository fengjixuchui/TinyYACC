#include "Lexer.h"
#include "Defs.h"
#include "SymbolTable.h"

namespace tinyYACC {

	//
	// printToken �������Token��ֵ.
	//
	wostream& operator<<(wostream& os, const Token& tk)
	{
		os << tk.content << L"[" << g_symbolTable[tk.mark] << L"]" << L"(" << tk.line << L")" << endl;
		return os;
	}

	Lexer::Lexer(const vector<TokenDef>& tokens, const vector<wstring>& ignoreTokens)
	{
		// ����ǳ��п����׳��쳣, �����ڹ��캯�����׳��쳣ͨ���ǲ��������,
		// �����ڽ���Lexer����֮ǰ,��ȷ�������tokens�������.
		for (auto it = tokens.begin(); it != tokens.end(); ++it) {
			regexs_.push_back(wregex(it->pattern));
			marks_.push_back(g_symbolTable[it->name]);  // ���ʷ���nameת��Ϊint
		}
		for (auto sym : ignoreTokens)
		ignore_.insert(g_symbolTable[sym]);
	}

	Lexer::~Lexer()
	{
	}

	//
	// next�������ڻ�ȡ��һ��Ԫ��,�������ĵ����Ԫ��.
	//
	Token Lexer::next()
	{
		if (cache_.size() != 0) {
			auto tk = cache_.front();
			cache_.pop_front();
			return tk;
		}

		if (offset_ == stream_.length()) // �����ַ����Ѿ������ĵ���
			return Token(g_symbolTable.endMark, line_, L"");

		std::wsmatch mc;
		bool wrong = true;
		// ʹ�ø���������ʽȥƥ��
		size_t	line = line_;
		size_t i = 0;
		for (; i < regexs_.size(); ++i) {
			bool finded = std::regex_search(stream_.cbegin() + offset_, stream_.cend(), mc, regexs_[i]);
			if (finded && mc.position() == 0 && mc.length() != 0) {
				wrong = false;
				offset_ += mc.length();
				bool over = offset_ == stream_.length();
				int type = marks_[i];
				wstring matchStr = mc.str();
				if (matchStr.find(L'\n') != matchStr.npos) { // new line
					line_ += countNL(matchStr); 
				}
				if (ignore_.find(type) != ignore_.end()) { // should be ignored!
					if (!over)
						return next();
					else
						return Token(g_symbolTable.endMark, line,  L"");
				}
				break;
			}
		}
		if (wrong) { // ��ô��������ʽ,��Ȼû��һ��ƥ��,Ҳ������,�������޷�ƥ����ַ�.
			GrammarError error = { line, L"�޷�ʶ����ַ�" };
			throw error;
		}
		return Token(marks_[i], line, mc.str());
	}

	int Lexer::countNL(const wstring& str) {
		int res = 0;
		for (auto s : str) {
			if (L'\n' == s)
				res++;
		}
		return res;
	}
	//
	// peekToken �����������token,ֻ��͵��һ��.
	//
	Token Lexer::peek() {
		if (cache_.size() == 0) {
			cache_.push_back(next());
		}
		return cache_.front();
	}


	//
	// checkTokens���ڼ��token�Ķ����Ƿ�������.
	//
	void checkTokens(const vector<TokenDef>& tokens) {
		set<wstring> count;
		for (auto it = tokens.begin(); it != tokens.end(); ++it) {
			if (count.find(it->name) != count.end()) {
				// һ��Token�����˶��ض���
				GeneralError error;
				error.msg = it->name + L"�����˶��ض���!";
				throw error;
			}
			else
				count.insert(it->name);
			try
			{
				wregex(it->pattern);
			}
			catch (...) {
				GeneralError error;
				error.msg = it->name + L"�Ķ���\"" + it->pattern + L"\"�����˴���!";
				throw error;
			}
		}
	}
}
