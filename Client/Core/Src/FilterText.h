// Linux transplant by Cooler 2004-01-02 liuyujun@263.net
//////////////////////////////////////////////////////////////////////

#ifndef __FILTERTEXT_H__
#define __FILTERTEXT_H__

#include "Regexp.h"
#include <vector>

const TCHAR leadchar_common = '=';
const TCHAR leadchar_advance = '+';
const TCHAR leadchar_ignore = '-';
const TCHAR leadchar_insensitive = '@';

class CTextFilter
{
public:

	CTextFilter() { m_nCommon = 0; }
	~CTextFilter() {};
	virtual BOOL AddExpression(LPCTSTR szExp);
	virtual BOOL Clearup();
	virtual BOOL IsTextPass(LPCTSTR text);

#ifndef _SERVER
	virtual BOOL ReplaceInvalidText(LPCTSTR text, TCHAR saver);
#endif

private:
	typedef std::vector<Regexp>	CExpArray;

	enum {HASHSIZE = 0x01 << (sizeof(_TUCHAR) * 8)};
	typedef _TUCHAR	HASHINDEXTYPE;

	HASHINDEXTYPE Char2HashIdx(TCHAR ch) {return HASHINDEXTYPE(ch);}

	CExpArray m_HashEntry[HASHSIZE];
	CExpArray m_AdvExps;
	int m_nCommon;

private:
	static const TCHAR* NextChar(const TCHAR* p)
	{
//		ASSERT(p && p[0]);
#ifdef _UNICODE
		return p + 1;
#else
		return p + (p[0] < 0 && p[1] < 0 ? 2 : 1);
#endif
	}
};

extern CTextFilter g_TextFilter;
extern CTextFilter g_ChatTxtFilter;

#ifndef _SERVER
extern CTextFilter g_UserChatFilter;
extern CTextFilter g_ChatRecvFilter;
#endif

#endif // __FILTERTEXT_H__
