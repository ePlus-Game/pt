// SvrLog.h: interface for the CSvrLog class.
// by Cooler liuyujun@263.net
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SVRLOG_H__FBB1E439_F910_494E_B395_BCF032D57B57__INCLUDED_)
#define AFX_SVRLOG_H__FBB1E439_F910_494E_B395_BCF032D57B57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BZFile.h"
#include <string>

typedef std::string BZString;

// Macro define region
#define FILESTATUS_CLOSE		0
#define FILESTATUS_OPEN			1

#define PATH_LOGFILES			"Logs\\"
#define SERVERLOG_SKIP			" -> "
#define SERVERLOG_ENTER			"\r\n"

class CSvrLog  
{
protected:
	static CBZFile m_fileSvrLog;
	static int m_nFileStatus;

	static void GetCurTime(BZString &strCurTime);
	static BOOL OpenLog();
	static void CloseLog();
	static void WriteData(LPCSTR pcLogMsg);

public:
	CSvrLog();
	virtual ~CSvrLog();

	static void WriteLog(LPCSTR pcLogMsg, 
		BOOL bNeedSeparator = FALSE);
	static void GetLogPathName(BZString &strPathName);
};

#endif // !defined(AFX_SVRLOG_H__FBB1E439_F910_494E_B395_BCF032D57B57__INCLUDED_)
