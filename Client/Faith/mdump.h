//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 06/08/2007 12:23
//      File_base        : mdump
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef MDUMP_H
#define MDUMP_H

#include "CoreUseNameDef.h"


/* Planeshift enums for minidump types.
 *  Redefined and translated so the caller doesn't have to find/include the real dbghelp.h header
 */
typedef enum _PS_MINIDUMP_TYPE {
    PSMiniDumpNormal         = 0x0000,
    PSMiniDumpWithDataSegs   = 0x0001,
    PSMiniDumpWithFullMemory = 0x0002,
    PSMiniDumpWithHandleData = 0x0004,
    PSMiniDumpFilterMemory   = 0x0008,
    PSMiniDumpScanMemory     = 0x0010
} PS_MINIDUMP_TYPE;

typedef enum {
    PSCrashActionOff = 0,
    PSCrashActionPrompt,
    PSCrashActionAlways,
    PSCrashActionIgnore
} PS_CRASHACTION_TYPE;

struct  FilePutParam
{
	char* addr;
	int port;
	char* username;
	char* password;
	char* locolfile;
	char* serverfile;
};

bool PutFileToServer( FilePutParam putParam );

class MiniDumper
{
public:
	MiniDumper();
	~MiniDumper() {};

public:
	static LONG WINAPI	TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );
	void				LoadConfig( void );
	void				SetDumpType(PS_MINIDUMP_TYPE type);
	PS_MINIDUMP_TYPE	GetDumpType();
	void				SetCrashAction(PS_CRASHACTION_TYPE action);
	PS_CRASHACTION_TYPE GetCrashAction();
	const char*			GetDumpTypeString();

public:
	static int DumpType; // really a MINIDUMP_TYPE enum, but stored as an int
	static PS_CRASHACTION_TYPE CrashAction;
	static char szFtpAddr[COMMON_CLIENT_MSG_LEN_256];
	static int	nPort;
	static char szUseName[COMMON_CLIENT_MSG_LEN_64];
	static char szPassWord[COMMON_CLIENT_MSG_LEN_64];
	static char szServerPath[COMMON_CLIENT_MSG_LEN_256];
	static char szDumpPath[_MAX_PATH];
	static int nClientVersion;
	//static HANDLE hPutEvent;
};


#endif // #ifndef MDUMP_H

