//////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-7-25 11:05
//      File_base        : cfs_filelogs
//      File_ext         : h
//      Author           : Cooler(liuyujun@263.net)
//      Description      : FSOnline series file logs writer
//
//      <Change_list>
//
//      Example:
//      {
//      Change_datetime  : year-month-day hour:minute
//      Change_by        : changed by who
//      Change_purpose   : change reason
//      }
//////////////////////////////////////////////////////////////////////////

#ifndef _CFS__FILELOGS___H____
#define _CFS__FILELOGS___H____

// Macro define region
#define FILESTATUS_CLOSE			0
#define FILESTATUS_OPEN				1

#define MAXSIZE_CURTIME				64
#define MAXSIZE_PATHNAME			512
#define MAXSIZE_MSGBUF				(1024*8)
#ifdef _WIN32
#define SUBDIRNAME_LOG				"logs\\"
#else
#define SUBDIRNAME_LOG				"./logs/"
#endif
#define SERVERLOG_SKIP				" -> "
#define SERVERLOG_ENTER				"\r\n"

#define LOGLEADNAME_BOOT			"boot"
#define LOGLEADNAME_DEBUG			"debug"

class CFS_FILELOGS  
{
public:
	CFS_FILELOGS();
	virtual ~CFS_FILELOGS();

	static void WriteLog(const char *pcLogMsg, ...);
	static void WriteDebugLog(const char *pcLogMsg, ...);

private:
	static FILE *m_fileSvrLog;
	static int m_nFileStatus;

	static void WriteLogInner(const char *pcLogMsg, 
		const char *pcLogLead, 
		unsigned char bKeepOpen = 0, 
		unsigned char bNeedSeparator = 0);
	static void CloseLog();

	static int OpenLog(const char *pcLogLead);
	static void WriteData(const char *pcLogMsg);
};

#endif // _CFS__FILELOGS___H____
