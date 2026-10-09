// SvrLog.cpp: implementation of the CSvrLog class.
// by Cooler liuyujun@263.net
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "SvrLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

CBZFile CSvrLog::m_fileSvrLog;
int CSvrLog::m_nFileStatus = FILESTATUS_CLOSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSvrLog::CSvrLog()
{

}

CSvrLog::~CSvrLog()
{

}

void CSvrLog::WriteLog(LPCSTR pcLogMsg, BOOL bNeedSeparator)
{
	if(pcLogMsg == NULL)
	{
		return;
	}

	BOOL bOpenSuccess = TRUE;

	bOpenSuccess = OpenLog();

	BZString strWriteMsg;
	if(bOpenSuccess)
	{
		GetCurTime(strWriteMsg);
		strWriteMsg += SERVERLOG_SKIP;
		strWriteMsg += pcLogMsg;
		strWriteMsg += SERVERLOG_ENTER;

		WriteData(strWriteMsg.data());
		if(bNeedSeparator)
		{
			WriteData(SERVERLOG_ENTER);
		}

		CloseLog();
	}
}

void CSvrLog::GetCurTime(BZString &strCurTime)
{
	SYSTEMTIME curDateTime;
	::GetLocalTime(&curDateTime);
	char szDataTime[64] = {0};
	sprintf(szDataTime, "%04d-%02d-%02d %02d:%02d:%02d", 
		curDateTime.wYear, curDateTime.wMonth, curDateTime.wDay, 
		curDateTime.wHour, curDateTime.wMinute, curDateTime.wSecond);
	strCurTime = szDataTime;
}

BOOL CSvrLog::OpenLog()
{
	BOOL bRet = TRUE;

	if(m_nFileStatus == FILESTATUS_CLOSE)
	{
		BZString strLogPathName;
		GetLogPathName(strLogPathName);

		BOOL bOpenSuccess = 
			m_fileSvrLog.Open(strLogPathName.data(), 
				CBZFile::modeReadWrite);
		if(!bOpenSuccess)
		{
			bOpenSuccess = m_fileSvrLog.Open(strLogPathName.data(), 
				CBZFile::modeCreate | 
				CBZFile::modeReadWrite);
		}

		if(bOpenSuccess)
		{
			m_fileSvrLog.SeekToEnd();
			m_nFileStatus = FILESTATUS_OPEN;
		}
		else
		{
			bRet = bOpenSuccess;
		}
	}

	return bRet;
}

void CSvrLog::CloseLog()
{
	if(m_nFileStatus == FILESTATUS_OPEN)
	{
		m_fileSvrLog.Close();
		m_nFileStatus = FILESTATUS_CLOSE;
	}
}

void CSvrLog::WriteData(LPCSTR pcLogMsg)
{
	if(pcLogMsg == NULL)
	{
		return;
	}

	if(m_nFileStatus == FILESTATUS_OPEN)
	{
		try
		{
			m_fileSvrLog.Write(pcLogMsg, 
				strlen(pcLogMsg));
		}
		catch( ... )
		{
			// Do nothing
		}
	}
}

void CSvrLog::GetLogPathName(BZString &strPathName)
{
	char szPathName[_MAX_PATH];

	::GetModuleFileName(NULL, szPathName, _MAX_PATH);
	char drive[3];
	char dir[_MAX_PATH];
	_splitpath(szPathName, drive, dir, NULL, NULL);
	strPathName = drive;
	strPathName = strPathName + dir;

	strPathName += PATH_LOGFILES;
	CreateDirectory(strPathName.data(), NULL);
	SYSTEMTIME curDateTime;
	::GetLocalTime(&curDateTime);
	char szFileName[MAX_PATH] = {0};
	sprintf(szFileName, "BZ%04d%02d%02d.txt", 
		curDateTime.wYear, curDateTime.wMonth, curDateTime.wDay);
	strPathName += szFileName;
}
