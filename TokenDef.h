#pragma once
#include "Common.h"
/*
 * TokenDef�ǹ���Token�Ķ���.
 */
struct TokenDef
{
	TokenDef(const wstring& name, const wstring& pattern) :
		name(name), pattern(pattern)
	{}
	wstring name;		/* ��¼���Token������ */
	wstring pattern;	/* ��¼��Ӧ��������ʽ */
};


