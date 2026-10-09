//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 06/08/2007 12:23
//      File_base        : mdump
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include <iostream>
#include <tchar.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include "dbghelp.h"
#include "Wininet.h"
#include "KIniFile.h"
#include "Process.h"
#include "KWin32Wnd.h"
#include "resource.h"
#include "mdump.h"
#include "GlobalDef.h"

static FilePutParam filePutParam;

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
                                    );

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			{
				::SetDlgItemText( hDlg,IDC_ERR_INFO, DUMP_DESC_0 );

				
				RECT rc;
				::GetWindowRect( hDlg, &rc );
				RECT rcDesk;
				GetWindowRect(GetDesktopWindow(),&rcDesk);
				int width = rc.right - rc.left;
				int height = rc.bottom - rc.top;
				int cx = ((rcDesk.right - rcDesk.left) - width)/2;
				int cy = ((rcDesk.bottom - rcDesk.top) - height)/2;


				MoveWindow(hDlg,cx,cy,width,height,true);

			}
			return TRUE;

		case WM_COMMAND:
			if ( LOWORD(wParam) == IDC_SEND_DUMP ) 
			{
				::EnableWindow( GetDlgItem( hDlg, IDC_SEND_DUMP ), FALSE );
				::SetDlgItemText( hDlg,IDC_ERR_INFO, DUMP_DESC_1 );
				PutFileToServer( filePutParam );
				::SetDlgItemText( hDlg,IDC_ERR_INFO, DUMP_DESC_2 );
				Sleep(1000);
				::EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}




int MiniDumper::DumpType;
PS_CRASHACTION_TYPE MiniDumper::CrashAction;
char MiniDumper::szFtpAddr[COMMON_CLIENT_MSG_LEN_256];
int	MiniDumper::nPort;
char MiniDumper::szUseName[COMMON_CLIENT_MSG_LEN_64];
char MiniDumper::szPassWord[COMMON_CLIENT_MSG_LEN_64];
char MiniDumper::szServerPath[COMMON_CLIENT_MSG_LEN_256];
int MiniDumper::nClientVersion;
char MiniDumper::szDumpPath[_MAX_PATH];

void CALLBACK InternetStatusCallback(
  HINTERNET hInternet,
  DWORD dwContext,
  DWORD dwInternetStatus,
  LPVOID lpvStatusInformation,
  DWORD dwStatusInformationLength
)
{
	switch(dwContext)   
	{   
	case   0:   //   Request   handle   
		  switch(dwInternetStatus)   
		  {   
		  case   INTERNET_STATUS_HANDLE_CREATED:   
				  {   
					  INTERNET_ASYNC_RESULT   *pRes   =   (INTERNET_ASYNC_RESULT   *)lpvStatusInformation;   
				  }   
				  break;   
		  case   INTERNET_STATUS_REQUEST_SENT:   
				  {   
					  ::MessageBox( NULL, DUMP_DESC_1, "FSOnline2", MB_OK + MB_ICONINFORMATION );
				  }   
				  break;   
		  case   INTERNET_STATUS_REQUEST_COMPLETE:   
				  {   
					 ::MessageBox( NULL, DUMP_DESC_2, "FSOnline2", MB_OK + MB_ICONINFORMATION );
					 if ( MiniDumper::szDumpPath[0] != 0 )
					 {
						 ::DeleteFile(MiniDumper::szDumpPath);
					 }
				  }   
				  break;   
		  case   INTERNET_STATUS_RECEIVING_RESPONSE:   
				  break;   
		  case   INTERNET_STATUS_RESPONSE_RECEIVED:   
				  {   
				  }   

		  }   

	}   

}

/************************************************************************/
/*                                                                      */
/************************************************************************/

bool PutFileToServer( FilePutParam putParam )
{
	if ( putParam.addr == NULL ||
		putParam.port == 0 ||
		putParam.username == NULL ||
		putParam.password == NULL ||
		putParam.locolfile == NULL || 
		putParam.serverfile == NULL )
	{
		return false;
	}

	if ( putParam.addr[0] == 0 ||
		putParam.username[0] == 0 ||
		putParam.password[0] == 0 ||
		putParam.locolfile[0] == 0 || 
		putParam.serverfile[0] == 0 )
	{
		return false;
	}

	HINTERNET hSession = ::InternetOpen(0, INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);
	if (!hSession) 
	{
		return false;
	}

	DWORD tmpTimeOut = 10000;
	BOOL bOption = ::InternetSetOption(hSession,INTERNET_OPTION_CONNECT_TIMEOUT,&tmpTimeOut,sizeof(tmpTimeOut));  
	if ( !bOption )
	{
		return false;
	}

	INTERNET_STATUS_CALLBACK pCallBack = ::InternetSetStatusCallback( hSession, (INTERNET_STATUS_CALLBACK)&InternetStatusCallback );
	if ( pCallBack == INTERNET_INVALID_STATUS_CALLBACK )
	{
		return false;
	}

	HINTERNET hService = ::InternetConnect( 
							hSession, 
							putParam.addr,
							putParam.port,
							putParam.username,
							putParam.password,
							INTERNET_SERVICE_FTP, 
							INTERNET_FLAG_PASSIVE, 
							0 );
	if (!hService) 
	{
		return false;
	}
	


	HINTERNET hFtpFile = ::FtpOpenFile(hService, putParam.serverfile,GENERIC_READ,FTP_TRANSFER_TYPE_BINARY, 0 );
	
	if ( !hFtpFile )
	{
		BOOL bRet = ::FtpPutFile(hService, putParam.locolfile, putParam.serverfile, FTP_TRANSFER_TYPE_BINARY, 0);		
		int err = ::GetLastError();
		::InternetCloseHandle( hFtpFile );
	}
	
	int nErr = ::GetLastError();							
	::InternetCloseHandle(hService);
	::InternetCloseHandle(hSession);

	::DeleteFile( putParam.locolfile );



	return true;
}



/************************************************************************/
/*                                                                      */
/************************************************************************/

MiniDumper::MiniDumper()
{
    // if this assert fires then you have two instances of MiniDumper
    // which is not allowed
    // Taken out
  
  // Default to a normal dump including only stack, segment, and load information
  DumpType=(int)MiniDumpNormal; 
  // Default to prompting wether to save a dump on crash
  CrashAction=PSCrashActionPrompt;

	memset( szFtpAddr, 0, sizeof(szFtpAddr) );
	nPort = 0;
	memset( szUseName, 0, sizeof(szUseName) );
	memset( szPassWord, 0, sizeof(szPassWord) );
	memset( szServerPath, 0, sizeof(szServerPath) );
	memset(szDumpPath, 0, sizeof(szDumpPath) );
	nClientVersion = 0;

    ::SetUnhandledExceptionFilter( TopLevelFilter );
}

void MiniDumper::LoadConfig( void )
{
	KIniFile versionIni;

//<-------EXVERSION
	if ( versionIni.Load( VERSION_CFG ) )
	{
		versionIni.GetInteger("Version", "Version", 0, &nClientVersion );
	}

	KIniFile dumpIni;

//<------EXVERSION
	if ( dumpIni.Load(AUTOUPDATE_INI) )
	{
		dumpIni.GetString("Dump", "Addr", "", szFtpAddr, sizeof(szFtpAddr) );
		dumpIni.GetInteger("Dump", "Port", 0, &nPort );
		dumpIni.GetString("Dump", "UserName", "", szUseName, sizeof(szUseName) );
		dumpIni.GetString("Dump", "PassWord", "", szPassWord, sizeof(szPassWord) );
		dumpIni.GetString("Dump", "ServerPath", "", szServerPath, sizeof(szServerPath) );
	}

	//DumpDialog::Create();
}

void MiniDumper::SetDumpType(PS_MINIDUMP_TYPE type)
{
  switch (type)
  {
    case PSMiniDumpWithDataSegs:
      DumpType=(int)MiniDumpWithDataSegs;
      break;
    case PSMiniDumpWithFullMemory:
      DumpType=(int)MiniDumpWithFullMemory;
      break;
    case PSMiniDumpWithHandleData:
      DumpType=(int)MiniDumpWithHandleData;
      break;
    case PSMiniDumpFilterMemory:
      DumpType=(int)MiniDumpFilterMemory;
      break;
    case PSMiniDumpScanMemory:
      DumpType=(int)MiniDumpScanMemory;
      break;
    default:
      DumpType=(int)MiniDumpNormal;
      break;
  }
}

PS_MINIDUMP_TYPE MiniDumper::GetDumpType()
{
  switch ((MINIDUMP_TYPE)DumpType)
  {
    case MiniDumpWithDataSegs:
      return PSMiniDumpWithDataSegs;
    case MiniDumpWithFullMemory:
      return PSMiniDumpWithFullMemory;
    case MiniDumpWithHandleData:
      return PSMiniDumpWithHandleData;
    case MiniDumpFilterMemory:
      return PSMiniDumpFilterMemory;
    case MiniDumpScanMemory:
      return PSMiniDumpScanMemory;
    default:
      return PSMiniDumpNormal;
  }
}

void MiniDumper::SetCrashAction(PS_CRASHACTION_TYPE action)
{
  CrashAction=action;
}

PS_CRASHACTION_TYPE MiniDumper::GetCrashAction()
{
  return CrashAction;
}


const char *MiniDumper::GetDumpTypeString()
{
  static char dumpstring[256];

  switch (CrashAction)
  {
    case PSCrashActionOff:
      strcpy(dumpstring,"Crash dumps are OFF");
      return dumpstring;
    case PSCrashActionAlways:
      strcpy(dumpstring,"Crash dumps will ALWAYS be generated.  ");
      break;
    default:
      strcpy(dumpstring,"You will be PROMPTED before a crash dump is generated.  ");
      break;
  }

  switch ((MINIDUMP_TYPE)DumpType)
  {
    case MiniDumpWithDataSegs:
      strcat(dumpstring,"Format is Detailed (Include data segments associated with modules at load time.  This includes global and static members but NOT the entire heap)");
      break;
    case MiniDumpWithFullMemory:
      strcat(dumpstring,"Format is Full (Include ALL process memory)");
      break;
    case MiniDumpWithHandleData:
      strcat(dumpstring,"Format is NTHandles (Like 'Normal' but include system handle information at the time of the crash - NT/2K/XP only)");
      break;
    case MiniDumpFilterMemory:
      strcat(dumpstring,"Format is Filter (Like 'Normal' but strip the stack and backtrace data down to only what's necessary for the raw function trace)");
      break;
    case MiniDumpScanMemory:
      strcat(dumpstring,"Format is Scan (Like 'Normal' but scan stack and backtrace data for module references and mark accordingly)");
      break;
    default:
      strcat(dumpstring,"Format is Normal (Stack and Backtrace information only)");
      break;
  }
  return dumpstring;
}


LONG MiniDumper::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
    // See if a crash dump should be generated at all
    if (CrashAction == PSCrashActionOff)
        return EXCEPTION_CONTINUE_SEARCH;
    else if (CrashAction == PSCrashActionIgnore)
        return EXCEPTION_EXECUTE_HANDLER;

    LONG retval = EXCEPTION_CONTINUE_SEARCH;

    // firstly see if dbghelp.dll is around and has the function we need
    // look next to the EXE first, as the one in System32 might be old 
    // (e.g. Windows 2000)
    HMODULE hDll = NULL;
    char szDbgHelpPath[_MAX_PATH];

    if (GetModuleFileName( NULL, szDbgHelpPath, _MAX_PATH ))
    {
        char *pSlash = _tcsrchr( szDbgHelpPath, '\\' );
        if (pSlash)
        {
            _tcscpy( pSlash+1, "DBGHELP.DLL" );
            hDll = ::LoadLibrary( szDbgHelpPath );
        }
    }

    if (hDll==NULL)
    {
        // load any version we can
        hDll = ::LoadLibrary( "DBGHELP.DLL" );
    }

    LPCTSTR szResult = NULL;

    if (hDll)
    {

    char exceptionNum[30];
        MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
        if (pDump)
        {
			  const char* exceptionType;
			  switch(pExceptionInfo->ExceptionRecord->ExceptionCode)
			  {
			  case EXCEPTION_ACCESS_VIOLATION:
				exceptionType = "EXCEPTION_ACCESS_VIOLATION";
				break;
			  case EXCEPTION_DATATYPE_MISALIGNMENT:
				exceptionType = "EXCEPTION_DATATYPE_MISALIGNMENT";
				break;
			  case EXCEPTION_BREAKPOINT:
				exceptionType = "EXCEPTION_BREAKPOINT";
				break;
			  case EXCEPTION_SINGLE_STEP:
				exceptionType = "EXCEPTION_SINGLE_STEP";
				break;
			  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
				exceptionType = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
				break;
			  case EXCEPTION_FLT_DENORMAL_OPERAND:
				exceptionType = "EXCEPTION_FLT_DENORMAL_OPERAND";
				break;
			  case EXCEPTION_FLT_DIVIDE_BY_ZERO:
				exceptionType = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
				break;
			  case EXCEPTION_FLT_INEXACT_RESULT:
				exceptionType = "EXCEPTION_FLT_INEXACT_RESULT";
				break;
			  case EXCEPTION_FLT_INVALID_OPERATION:
				exceptionType = "EXCEPTION_FLT_INVALID_OPERATION";
				break;
			  case EXCEPTION_FLT_OVERFLOW:
				exceptionType = "EXCEPTION_FLT_OVERFLOW";
				break;
			  case EXCEPTION_FLT_STACK_CHECK:
				exceptionType = "EXCEPTION_FLT_STACK_CHECK";
				break;
			  case EXCEPTION_FLT_UNDERFLOW:
				exceptionType = "EXCEPTION_FLT_UNDERFLOW";
				break;
			  case EXCEPTION_INT_DIVIDE_BY_ZERO:
				exceptionType = "EXCEPTION_INT_DIVIDE_BY_ZERO";
				break;
			  case EXCEPTION_INT_OVERFLOW:
				exceptionType = "EXCEPTION_INT_OVERFLOW";
				break;
			  case EXCEPTION_PRIV_INSTRUCTION:
				exceptionType = "EXCEPTION_PRIV_INSTRUCTION";
				break;
			  case EXCEPTION_IN_PAGE_ERROR:
				exceptionType = "EXCEPTION_IN_PAGE_ERROR";
				break;
			  case EXCEPTION_ILLEGAL_INSTRUCTION:
				exceptionType = "EXCEPTION_ILLEGAL_INSTRUCTION";
				break;
			  case EXCEPTION_NONCONTINUABLE_EXCEPTION:
				exceptionType = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
				break;
			  case EXCEPTION_STACK_OVERFLOW:
				exceptionType = "EXCEPTION_STACK_OVERFLOW";
				break;
			  case EXCEPTION_INVALID_DISPOSITION:
				exceptionType = "EXCEPTION_INVALID_DISPOSITION";
				break;
			  case EXCEPTION_GUARD_PAGE:
				exceptionType = "EXCEPTION_GUARD_PAGE";
				break;
			  case EXCEPTION_INVALID_HANDLE:
				exceptionType = "EXCEPTION_INVALID_HANDLE";
				break;
			  default:
				sprintf(exceptionNum, "Unknown code %X", pExceptionInfo->ExceptionRecord->ExceptionCode);
				exceptionType = exceptionNum;
			  }
			  //*/

            {
                char szScratch [_MAX_PATH];

                // work out a good place for the dump file
                if (!GetModuleFileName( NULL, szDumpPath, _MAX_PATH ))
                {
                    if (!GetTempPath( _MAX_PATH, szDumpPath ))
                        _tcscpy( szDumpPath, "c:\\temp\\" );
                }
                else
                {
                    char *pSlash = _tcsrchr( szDumpPath, '\\' );
                    *(pSlash + 1) = '\0';
                }

                // Format the message
				char szDumpFileName[COMMON_CLIENT_MSG_LEN_256];
				memset(szDumpFileName, 0, sizeof(szDumpFileName) );

				//Add the version
				char szVersion[COMMON_CLIENT_MSG_LEN_80];
                snprintf(szVersion, sizeof(szVersion), DUMP_VER, nClientVersion);
                strncat(szDumpFileName,szVersion, sizeof(szDumpFileName));//*/

                //Add the address
				char szAddr[COMMON_CLIENT_MSG_LEN_64];
                snprintf(szAddr, sizeof( szAddr), "_Addr%p", pExceptionInfo->ExceptionRecord->ExceptionAddress);
                strncat(szDumpFileName,szAddr,sizeof(szDumpFileName));//*/

                // Convert
                strncat( szDumpFileName, ".dmp", sizeof(szDumpFileName) );
				strncat( szDumpPath, szDumpFileName, sizeof(szDumpPath) );


                // create the file
                HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, NULL );

                if (hFile!=INVALID_HANDLE_VALUE)
                {
                    _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

                    ExInfo.ThreadId = ::GetCurrentThreadId();
                    ExInfo.ExceptionPointers = pExceptionInfo;
                    ExInfo.ClientPointers = NULL;

                    
                    // write the dump
                   BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, (MINIDUMP_TYPE)DumpType, &ExInfo, NULL, NULL );
                    if ( bOK )
                    {						
                        retval = EXCEPTION_EXECUTE_HANDLER;

						if ( szFtpAddr[0] != 0 &&
							nPort != 0 &&
							szUseName[0] != 0 &&
							szPassWord[0] != 0 &&
							nClientVersion != 0
							)
						{
							::CloseHandle(hFile);
							hFile = NULL;
							char szServerFilePath[COMMON_CLIENT_MSG_LEN_256];
							memset(szServerFilePath, 0, sizeof(szServerFilePath) );
							strncat( szServerFilePath, szServerPath, sizeof(szServerFilePath) );
							strncat( szServerFilePath, szDumpFileName, sizeof(szServerFilePath) );

							memset( &filePutParam, 0, sizeof(filePutParam) );
							filePutParam.addr = szFtpAddr;
							filePutParam.port = nPort;
							filePutParam.username = szUseName;
							filePutParam.password = szPassWord;
							filePutParam.locolfile = szDumpPath;
							filePutParam.serverfile = szServerFilePath;
						
							//DumpDialog::Show( begin_dump, filePutParam );

							::DialogBox(g_GethInstance(), (LPCTSTR)IDD_DUMP_MSG, g_GetDrawHWnd(), (DLGPROC)About);
						}
                    }
                    else
                    {
                        snprintf( szScratch, sizeof(szScratch), "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError() );
                        szResult = szScratch;
                    }
					if ( hFile != NULL )
					{
						::CloseHandle(hFile);
					}
					
                }
                else
                {
                    snprintf( szScratch, sizeof(szScratch), "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError() );
                    szResult = szScratch;
                }
            }
        }
        else
        {
            szResult = "DBGHELP.DLL too old";
        }
    }
    else
    {
        szResult = "DBGHELP.DLL not found";
    }

    //if (szResult)
    //    ::MessageBox( NULL, szResult, "FSOnline2", MB_OK + MB_ICONINFORMATION );

    return retval;
}
