#include "Lexer.h"
#include "TokenDef.h"

Lexer::Lexer(vector<TokenDef>& tokens, vector<wstring>& ignoreTokens) {
	// ����ǳ��п����׳��쳣, �����ڹ��캯�����׳��쳣�ǲ��������,�����ڽ���Lexer����֮ǰ,��ȷ�������tokens�������.
	for (auto it = tokens.begin(); it != tokens.end(); ++it) {
		regexs_.push_back(wregex(it->pattern));
		names_.push_back(it->name);
	}
	ignore_.insert(ignoreTokens.begin(), ignoreTokens.end());
}

Lexer::~Lexer()
{
}

/*
 * lex��ʼ�Խ����ı�.
 */
Token Lexer::nextToken()
{
	if (cache_.size() != 0) {
		auto tk = cache_.front();
		cache_.pop_front();
		return tk;
	}

	if (offset_ == stream_.length()) // �����ַ����Ѿ������ĵ���
		return Token(L"#", line_, column_, L"");

	std::wsmatch mc;
	bool wrong = true;
	/* ʹ�ø���������ʽȥƥ�� */
	int	line = line_;
	int	column = column_;
	size_t i = 0;
	for (; i < regexs_.size(); ++i) {
		bool finded = std::regex_search(stream_.cbegin() + offset_, stream_.cend(), mc, regexs_[i]);
		if (finded && mc.position() == 0 && mc.length() != 0) {
			wrong = false;
			offset_ += mc.length();
			column_ += mc.length();
			bool over = offset_ == stream_.length();
			wstring type = names_[i];
			if (type == L"NL") { // new line
				line_ += 1;
				column_ = 1;
			}
			if (ignore_.find(type) != ignore_.end()) { // should be ignored!
				if (!over)
					return nextToken();
				else
					return Token(L"end", line, column, L"");
			}
			break;
		}
	}
	if (wrong) { /* ��ô��������ʽ,��Ȼû��һ��ƥ��,Ҳ������,�������޷�ƥ����ַ�. */
		throw GrammarError();
	}
	return Token(names_[i], line, column, mc.str());
}

/*
 * peekToken �����������token,ֻ��͵��һ��
 */
Token Lexer::peekToken() {
	if (cache_.size() == 0) {
		cache_.push_back(nextToken());
	}
	 return cache_.front();
}

/*
 * checkTokens���ڼ��token�Ķ����Ƿ�������,
 */
void checkTokens(vector<TokenDef>& tokens) {
	set<wstring> count;
	try
	{
		for (auto it = tokens.begin(); it != tokens.end(); ++it) {
			if (count.find(it->name) != count.end()) {
				/* һ��Token�����˶��ض��� */
				throw GrammarError();
			}
			else
				count.insert(it->name);
			wregex(it->pattern);
		}
	}
	catch (regex_error& e)
	{
		e.what();
		throw GrammarError();
	}
}

