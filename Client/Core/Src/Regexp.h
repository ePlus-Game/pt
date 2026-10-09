// Linux transplant by Cooler 2004-01-02 liuyujun@263.net
//////////////////////////////////////////////////////////////////////

#ifndef __REGEXP_H__
#define __REGEXP_H__

#include "KWin32.h"
#include <string>
#define KString		std::string

// At this project, not supported MBCS
#define LPTSTR		LPSTR
#define LPCTSTR		LPCSTR
#define TCHAR		char
#define _TUCHAR		unsigned char

#ifndef _T
#define _T(x)		x
#endif

#define _tcslen		strlen
#define _tcsstr		strstr
#define _tcschr		strchr
#define _tcsncmp	strncmp
#define _tcsspn		strspn
#define _tcscspn	strcspn
#define _tcsncpy	strncpy

class regexp;

class Regexp
{
public:
	enum { NSUBEXP = 10 };

	Regexp();
	Regexp( LPCTSTR exp, BOOL iCase = 0 );
	Regexp( const Regexp &r );
	~Regexp();

	const Regexp & operator=( const Regexp & r );
	const KString operator[]( unsigned int i ) const;

	bool Match( const TCHAR * s );
	int SubStrings() const;	
	int SubStart( unsigned int i ) const;
	int SubLength( unsigned int i ) const;
	KString GetReplaceString( LPCTSTR source ) const;

	KString GetErrorString() const;
	bool CompiledOK() const;

#if defined( _RE_DEBUG )
	void Dump();
#endif

private:
	const TCHAR * string;	/* used to return substring offsets only */
	mutable KString m_szError;
	regexp * rc;

	void ClearErrorString() const;
	int safeIndex( unsigned int i ) const;
};

#endif
