// UpdateProcess.cpp: implementation of the CUpdateProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autoupdate.h"
#include "UpdateProcess.h"
#include <string>
#include "KWin32.h"
#include "KPakList.h"
#include "KIniFile.h"
#include "KFilePath.h"
#include "EditionSelector.h"
#include <afxinet.h>
#include "DownLoadFile.h"
#include "Encrypter.h"

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const char CUpdateProcess::szEOF[] = {'\r', '\n'};
#define UPDATELOG_DIR			"logs"			// 升级日志目录
#define UPDATELOG_FILE			"updatelog.txt"	// 升级日志文件
#define SYMBOL_LOGBEGIN			"Update LOG begin..."
#define SYMBOL_LOGEND			"Update LOG end."

extern UPDATEA_INIT g_Update_Init;
extern UPDATE_UNINIT g_Update_UnInit;
extern UPDATE_START g_Update_Start;
extern UPDATE_START g_Update_Cancel;
extern UPDATE_NEXTIME g_Update_NextTime;
extern UPDATE_GETLOCK	g_Update_GetLock;
extern UPDATE_RELEASELOCK	g_Update_ReleaseLock;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUpdateProcess::CUpdateProcess() : 
	m_nCurrentHost(-1),
	m_bEnableRun(FALSE),
	m_bToQuit(FALSE),
	m_pCallBack(NULL),
	m_pOwnerWnd(NULL),
	m_bActive(FALSE)
{
}

//*********************************************************************
// function : 只能执行一次的初始化
// comment	: 相当于FinalConstruct
//*********************************************************************
BOOL CUpdateProcess::InitOnce()
{
	BOOL bResult = FALSE;
	try
	{
		char szFullPath[MAX_PATH] = {0};
		::g_GetRootPath(szFullPath);
		string strFullPath = szFullPath;
		strFullPath += "\\"UPDATELOG_DIR;
		::CreateDirectory(strFullPath.c_str(), NULL);
		strFullPath.append("\\");
		strFullPath.append(UPDATELOG_FILE);
		m_logFile.Open(strFullPath.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate);
		ASSERT(m_logFile.m_hFile != CFile::hFileNull);
		m_logFile.SeekToEnd();
		Log(CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"), SYMBOL_LOGBEGIN);
		ASSERT(!g_hModule);
		g_hModule = LoadLibrary("UpdateDLL.dll");
		ASSERT(g_hModule);
		if (g_hModule)
		{
			g_Update_Init = (UPDATEA_INIT)GetProcAddress(g_hModule, "Update_Init");
			g_Update_UnInit = (UPDATE_UNINIT)GetProcAddress(g_hModule, "Update_UnInit");
			g_Update_Start = (UPDATE_START)GetProcAddress(g_hModule, "Update_Start");
			g_Update_Cancel = (UPDATE_START)GetProcAddress(g_hModule, "Update_Cancel");
			g_Update_NextTime = (UPDATE_NEXTIME)GetProcAddress(g_hModule,"Update_NextTime");
			g_Update_GetLock = (UPDATE_GETLOCK)GetProcAddress(g_hModule, "GetUpdateLock");
			g_Update_ReleaseLock = (UPDATE_RELEASELOCK)GetProcAddress(g_hModule, "ReleaseUpdateLock");
		}
		bResult = TRUE;
	}
	catch (...)
	{}
	return bResult;
}

CUpdateProcess::~CUpdateProcess()
{
	ASSERT(m_logFile.m_hFile != CFile::hFileNull);
	if (m_logFile.m_hFile != CFile::hFileNull)
	{
		Log(CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"), SYMBOL_LOGEND);
		Log("\r\n");
		m_logFile.Close();
	}
	if (g_hModule)
	{
      FreeLibrary(g_hModule);
	  g_hModule = NULL;
	}
}

//*********************************************************************
// function : 记录日志信息
//*********************************************************************
void CUpdateProcess::Log(LPCSTR pcszMessage)
{
	ASSERT(pcszMessage && m_logFile.m_hFile != CFile::hFileNull);
	if (m_logFile.m_hFile != CFile::hFileNull)
	{
		m_logFile.Write(pcszMessage, ::strlen(pcszMessage));
		m_logFile.Write(szEOF, sizeof(szEOF));
		m_logFile.Flush();
	}
}

//*********************************************************************
// function : 记录日志信息
//*********************************************************************
void CUpdateProcess::Log(LPCSTR pcszPrefix, LPCSTR pcszMessage)
{
	ASSERT(pcszMessage && pcszPrefix && m_logFile.m_hFile != CFile::hFileNull);
	m_logFile.Write(pcszPrefix, ::strlen(pcszPrefix));
	m_logFile.Write(" : ", 1);
	m_logFile.Write(pcszMessage, ::strlen(pcszMessage));
	m_logFile.Write(szEOF, sizeof(szEOF));
}

struct TServerDirDownParam
{
	char* szDownSite;
	char* szDirPath;
	bool bIsReceivingDirFile;
	bool bResult;
};

BOOL OnExecute(LPCSTR szFile)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	::GetStartupInfo(&si);
    ZeroMemory( &pi, sizeof(pi) );
	
	char buffer[256];
	
	strcpy(buffer, szFile);
	
	BOOL bRet = TRUE;
	
    if( !CreateProcess( NULL, buffer, NULL,	NULL, FALSE, 0,	NULL, NULL,	&si, &pi)) {
		bRet = FALSE;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
	
	return bRet;
}

void DisplayErrorInfo(string& ErrorInfo) {
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	MessageBox( NULL, ErrorInfo.c_str(),(LPCTSTR)lpMsgBuf , MB_OK |MB_ICONWARNING );
	LocalFree( lpMsgBuf );
}

BOOL g_bLog = FALSE;

UPDATEA_INIT g_Update_Init = NULL;
UPDATE_UNINIT g_Update_UnInit = NULL;
UPDATE_START g_Update_Start = NULL;
UPDATE_START g_Update_Cancel = NULL;
UPDATE_NEXTIME g_Update_NextTime = NULL;
UPDATE_GETLOCK g_Update_GetLock = NULL;
UPDATE_RELEASELOCK g_Update_ReleaseLock = NULL;

extern CWndShell *g_pWndShell;
// ********************************************************************
// function : 界面更新回调函数，直接在这里显示状态
// ********************************************************************
int __stdcall RefreshStatus(int nCurrentStatus, long lParam)
{
	ASSERT(g_pWndShell);
	return g_pWndShell->Refresh(nCurrentStatus, lParam);
}

HMODULE g_hModule = NULL;

UINT CUpdateProcess::AutoUpdateDLL(LPCSTR pcszSite)
{
	KUPDATE_SETTING UpdateSet;
    int nRetCode = 0;
    CString sText;

    UpdateSet.bAutoTryNextHost = false;
    UpdateSet.bUseVerify = false;
    UpdateSet.bUseFastestHost = false;
    UpdateSet.pfnCallBackProc = &RefreshStatus;
    UpdateSet.ProxySetting.bUpdateAuth = true; 
    UpdateSet.ProxySetting.nHostPort = 0;
    UpdateSet.ProxySetting.nProxyMethod = PROXY_METHOD_DIRECT;
    strcpy(UpdateSet.szVerifyInfo, "102400-010999-106075-054738");
    UpdateSet.ProxySetting.szHostAddr[0] = '\0';
    UpdateSet.ProxySetting.szPassword[0] = '\0';
    UpdateSet.ProxySetting.szUserName[0] = '\0';
	
    
	char szModulePath[MAX_PATH + 1];
	::GetModuleFileName(NULL, szModulePath, (MAX_PATH + 1));
	char *pszOffset = NULL;
	pszOffset = strrchr(szModulePath, '\\');
    ASSERT(pszOffset);
	strcpy(UpdateSet.szMainExecute, pszOffset + 1);
    pszOffset[1] = '\0';
    strcpy(UpdateSet.szDownloadPath, szModulePath);
	strcat(UpdateSet.szDownloadPath, "Update\\");
    strcpy(UpdateSet.szUpdatePath, szModulePath);
	 //<------EXVERSION
	UpdateSet.nUpdateMode = 0;
	UpdateSet.bExServerMode = g_bExServerMode;

	strncpy(UpdateSet.szUpdateSite, pcszSite, MAX_PATH);
	
	CString strVersion = szModulePath;
	
	LPCSTR version = "Version.cfg";
	if (g_bExServerMode)
		version = "Versionex.cfg";

	UpdateSet.nVersion = ::GetPrivateProfileInt(
		"Version",
		"Version",
		0,
		strVersion + version);

	UpdateSet.nMajorVersion = ::GetPrivateProfileInt(
		"Version",
		"MajorVersion",
		0,
		strVersion + version);
	
	UpdateSet.bLog = g_bLog;
	if (g_Update_Init == NULL || g_Update_UnInit == NULL || g_Update_Start == NULL ||
		g_Update_GetLock == NULL || g_Update_ReleaseLock == NULL
	   )
	{
		goto Exit0;
	}

	// 检测是否取消
	if (m_kernel.IsCanceled())
	{
		nRetCode = defUPDATE_RESULT_CANCEL;
		goto Exit0;
	}

	nRetCode = g_Update_Init(UpdateSet);
	Sleep(10);
    if (nRetCode == defUPDATE_RESULT_INIT_FAILED)
        goto Exit0;
	
	// 设置内核标志，决定取消升级的时候是否调用g_Update_Cancel()
	m_kernel.InKernel(TRUE);
	// 检测是否取消
	if (m_kernel.IsCanceled())
	{
		nRetCode = defUPDATE_RESULT_CANCEL;
	}
	else
	{
		try { nRetCode = g_Update_Start(); } catch (...) {}
	}
	m_kernel.InKernel(FALSE);

    g_Update_UnInit();

Exit0:
	UpdateSet.nVersion = ::GetPrivateProfileInt("Version", "Version", 0, strVersion + version); //<------EXVERSION
    return nRetCode;
}

unsigned long CUpdateProcess::ProcessCRC(HANDLE hFile)
{
	unsigned long dwCRC=0;
	unsigned long word=0;
	unsigned long dwSizeReaded=0;

	while(ReadFile(hFile,&word,1,&dwSizeReaded,NULL) && dwSizeReaded!=0)
	{
		word=word-'0';
		dwCRC=dwCRC*10+word;
	}

	return dwCRC;
}

bool  CUpdateProcess::CheckCRC(const char * szFileName,const unsigned long dwMatchCRC)
{
	CFile File;
    CString szCRC;
    char *pvBuffer = NULL;
    int nRetCode   = 0;
    unsigned uResult    = 0;
    
    if (File.Open(szFileName, CFile::modeRead | CFile::shareDenyNone, NULL))
    {
        int nLen = File.GetLength();
        
        if (nLen > 0)
        {
            pvBuffer = new char[nLen + 1];
            if (pvBuffer != NULL)
            {
                nRetCode = File.Read(pvBuffer, nLen);
                if (nRetCode == nLen)
                    uResult = CRC32(0, pvBuffer, nLen);
            }
        }
        
        File.Close();
    }
    
    delete [] pvBuffer;
    
    return (uResult==dwMatchCRC);
}


BOOL CUpdateProcess::ServerStateDownThread()
{
	//删除本地的旧版本
	char szServerPath[MAX_PATH];
	g_GetRootPath(szServerPath);
	strcat(szServerPath, "\\UserData\\");
	g_CreatePath(szServerPath);
	strcat(szServerPath, "server.ini");
	strcat(szServerPath, ".crc");
//	DeleteFile(szServerPath);
	//下载服务器列表的网址获取
	char URL[MAX_PATH];
 //<------EXVERSION
	BOOL bResult;

	if (!g_bExServerMode)
		bResult = g_PakList.Open("\\package.ini");
	else
		bResult = g_PakList.Open("\\packageex.ini");

	if (!bResult)
		return bResult;   //可能在这里返回false

	KIniFile	IniFile;
	//<------EXVERSION
	LPCSTR autoupdatestr = NULL;
	
	if (!g_bExServerMode)
		autoupdatestr = "\\settings\\autoupdate.ini";
	else
		autoupdatestr = "\\settings\\autoupdateex.ini";
	
	if (IniFile.Load(autoupdatestr))
	{
		IniFile.GetString("main", "serverurl", "",URL, MAX_PATH);
	}//endif

	g_PakList.Close();

	unsigned long dwCRCLocal=0;
	//判断本地是否有CRC文件
	HANDLE hCRC=CreateFile(szServerPath,GENERIC_READ,FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hCRC!=INVALID_HANDLE_VALUE)
	{
		dwCRCLocal=ProcessCRC(hCRC);
		CloseHandle(hCRC);
		DeleteFile(szServerPath);
	}//end else

	bool bFixedDownServer = ('\0' == URL[0]) ? false : true;

	int nUpdateIndex = 0;
	const char* pSeverInfo = NULL;
	IEncrypter * pEncrypter=GetXOREncrypter();
	while ((pSeverInfo = GetNextUpdateServer(nUpdateIndex)) != NULL)
	{
		if(!bFixedDownServer)
		{
			_snprintf(URL, sizeof(URL), "%s%s", pSeverInfo, "index.ini");
		}

		strcat(URL,".crc");       //CRC的地址

		//Addition ServerState File Download first
		//Because no matter wheather the downloading is succsessful the process is goint right way
		char szStateURL[MAX_PATH];
		char szStateLocal[MAXPATH];
    
		g_GetRootPath(szStateLocal);
		strcat(szStateLocal, "\\UserData\\");
		g_CreatePath(szStateLocal);
		strcat(szStateLocal, "state.ini");

		sprintf(szStateURL,URL);
		int iStateLen=strlen(szStateURL);
		szStateURL[iStateLen-13]=0;
		strcat(szStateURL,"state.ini");
		DownLoadFromPath(szStateURL,szStateLocal);
    
		//下载服务器上的 CRC
		bResult=DownLoadFromPath(URL,szServerPath);
		if (bResult)
		{
			//计算服务器上的CRC
			hCRC=CreateFile(szServerPath,GENERIC_READ,FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			ASSERT(hCRC!=INVALID_HANDLE_VALUE);
			
			unsigned long dwNewCRC=ProcessCRC(hCRC);
			CloseHandle(hCRC);
			
			//还原路径
			int lenLocal=strlen(szServerPath);
			szServerPath[lenLocal-4]=0;
			int lenServer=strlen(URL);
			URL[lenServer-4]=0;
			
			//只有当服务器上的和本地不同的时候才下载ServerList
			if (dwNewCRC!=dwCRCLocal )
			{
				bResult=DownLoadFromPath(URL,szServerPath);
				bResult=pEncrypter->Encrypt(szServerPath,szServerPath);
			}//endif
			else//服务器上的和本地CRC相同
			{
				//否则先解码本地文件Check CRC
				bResult=pEncrypter->Unencrypt(szServerPath,szServerPath);		
			
				if (bResult)
				{
					//再一次效验CRC，避免ServerList文件出错
					//因为用户有可能在解码的时候终止程序（断电等），那样,如果不检查就永远不会更新ServerList了	
					bool bRight=CheckCRC(szServerPath,dwNewCRC);
					if (!bRight)
					{		
						bResult=DownLoadFromPath(URL,szServerPath);
						pEncrypter->Encrypt(szServerPath, szServerPath);
						break;
					}
					else
					{
						pEncrypter->Encrypt(szServerPath, szServerPath);
						break;
					}
				}
				else
				{
					//本地Serverlist不存在（有可能被删了）
					bResult=DownLoadFromPath(URL,szServerPath);
					pEncrypter->Encrypt(szServerPath, szServerPath);
					break;
				}
				
			}//end else

			// 只要这个ftp能连接上，后面的ftp就不继续尝试了
			break;
		}
		else
			continue;

		if(bFixedDownServer)
			break;
	}

	// 注意: 调用了GetNextUpdateServer后
	// 一定要在 return之前一定要将m_nCurrentHost置为0
	m_nCurrentHost = 0;

	delete pEncrypter;
	return bResult;
}

UINT CUpdateProcess::ServerDirDownThread(LPVOID p)
{
	ASSERT(m_pCallBack);
	TServerDirDownParam* pParam = (TServerDirDownParam*)p;

	if (!IsToQuit())
		m_pCallBack->NotifyProcessStatus(U_DOWNLOAD_FOLDER_LIST_FILE);

	char szServerPath[MAX_PATH] = {0};
	_snprintf(szServerPath, MAX_PATH - 1, "%s%s", pParam->szDownSite, INIFILE_UPDATE);

	pParam->bIsReceivingDirFile = true;

	/*
	BOOL blnRet = CDownLoadFile::GetFileByFtp(szServerPath, pParam->szDirPath, TRUE,
		CONNECTION_TIMEOUT, CONNECTION_TIMEOUT, CONNECTION_TIMEOUT);
	*/

	BOOL blnRet = DownLoadFromPath(szServerPath,pParam->szDirPath);

	pParam->bResult = ((blnRet == TRUE) ? true : false);

	return blnRet;
}

//*********************************************************************
// function : 删除以前升级遗留的文件
//*********************************************************************
void CUpdateProcess::DeleteOldFiles()
{
	char szFile[MAX_PATH];
	char szPath[MAX_PATH];
	g_GetRootPath(szFile);
	strcat(szFile, "\\Update\\");
	int		nFilePathLen = strlen(szFile);
	memcpy(szPath, szFile, nFilePathLen);
	strcpy(szPath + nFilePathLen, "*.tmp");
	
	WIN32_FIND_DATA		FindData;
	memset(&FindData, 0, sizeof(FindData));
	HANDLE	hFindHandle = ::FindFirstFile(szPath, &FindData);
	if (hFindHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			strcpy(szFile + nFilePathLen, FindData.cFileName);
			DeleteFile(szFile);
		} while(FindNextFile(hFindHandle, &FindData));
		FindClose(hFindHandle);
		hFindHandle = NULL;
	}
}

//add by zuolizhi
void CUpdateProcess::StartDownLoadBackGround()
{
	typedef DWORD (WINAPI CUpdateProcess::*CLASS_ROUTINE)();
	typedef DWORD (WINAPI *THREAD_ROUTINE)(VOID*);

	DWORD	dwThreadID;
	HANDLE	ThreadHandle;
	THREAD_ROUTINE ThreadRoutine;
	CLASS_ROUTINE ClassRoutine;
	
	ClassRoutine = DownLoadBackGroundProc;
	
	_asm
	{
		mov eax,ClassRoutine
		mov ThreadRoutine,eax
	}

	ThreadHandle =
	CreateThread(NULL,NULL,ThreadRoutine,this,NULL,&dwThreadID);

	CloseHandle(ThreadHandle);
}

DWORD WINAPI CUpdateProcess::DownLoadBackGroundProc()
{
	BOOL a = m_bActive;
	return 0;
}

//*********************************************************************
// function : 升级主函数，被子线程调用
//*********************************************************************
DWORD WINAPI CUpdateProcess::UpdateProcess(LPVOID pParam)
{
	ASSERT(pParam );
	int ret = 0;
	CUpdateProcess *pProcess = (CUpdateProcess*)pParam;
	BOOL bOK = pProcess->ServerStateDownThread();
	//Lucifer~yu 2005-7-29 Dell
//	ASSERT(bOK && pProcess->m_pCallBack && pProcess->m_pOwnerWnd);

	CWndCallBack *pCallBack = pProcess->m_pCallBack;
	ASSERT(pCallBack);
    
	UPDATE_LOG(pProcess, bOK ? U_DOWNLOAD_SVR_STAT_SUCC : U_DOWNLOAD_SVR_STAT_FAIL)
    
	//Addd by Brianyao 2007
	if (bOK)
	{
  //     pCallBack->NotifyServerList();
	}//endif
		
	int nUpdateIndex = 0;
	const char* pSeverInfo = NULL;
	while ((pSeverInfo = pProcess->GetNextUpdateServer(nUpdateIndex)) != NULL)
	{
		UPDATE_PREFIX_LOG(pProcess, U_CONNECT_DOWNLOAD_SVR, pSeverInfo)
		Sleep(10);
		CString strMsg;
		strMsg.Format(U_TRY_UPDATE_SERVER, nUpdateIndex);
		pCallBack->NotifyProcessStatus((LPCSTR)strMsg);

		CString strDownSite = pSeverInfo;

		bool bIsOK = false;
		//-----删除以前升级失败/出错可能留下临时文件----
		pProcess->DeleteOldFiles();
		//----------------------------------------------

		char szDirPath[MAX_PATH];
		g_GetRootPath(szDirPath);
		strcat(szDirPath, "\\UserData\\");
		g_CreatePath(szDirPath);
		strcat(szDirPath, INIFILE_UPDATE);
		DeleteFile(szDirPath);

		if (theApp.GetUpdateDir().empty())
		{
			TServerDirDownParam ThreadParam;
			ThreadParam.szDownSite = (char*)pSeverInfo;
			ThreadParam.szDirPath = szDirPath;
			ThreadParam.bIsReceivingDirFile = false;
			ThreadParam.bResult = false;
			pProcess->ServerDirDownThread(&ThreadParam);
			bIsOK = ThreadParam.bResult;
		}

		if (pProcess->m_kernel.IsCanceled())
		{
			bIsOK = FALSE;
		}
		else if (theApp.GetUpdateDir().empty() && bIsOK)
		{//update.ini下载成功，处理列表
			bIsOK = false;
			string strDirectory;
			string strApplication;
			if (CEditionSelector(pProcess->m_pOwnerWnd, szDirPath).GetEdition(strDirectory, strApplication))
			{
				ASSERT(!strDirectory.empty() && !strApplication.empty());
				strDownSite += CString(strDirectory.c_str()) + "/";
				char szRootPath[MAX_PATH];
				g_GetRootPath(szRootPath);
				string strAppPath;
				strAppPath.append(szRootPath);
				strAppPath.append("\\");
				strAppPath.append(strApplication);
				// 设置运行程序名字和目录名字
				theApp.SetApplication(strAppPath.c_str());
				theApp.SetUpdateDir(strDirectory.c_str());
				bIsOK = true;
			}
		}
		else if (!theApp.GetUpdateDir().empty())
		{//已经有选择
			strDownSite += "/";
			strDownSite += theApp.GetUpdateDir().c_str();
			strDownSite += "/";
			bIsOK = true;
		}
	
		// 检测用户是否取消
		if (pProcess->m_kernel.IsCanceled())
		{
			ret = defUPDATE_RESULT_CANCEL;
		}
		else if(bIsOK)
		{
			UPDATE_LOG(pProcess, L_CONNECT_SERVER_SUCC)
			ret = pProcess->AutoUpdateDLL(strDownSite);
		}
		else
		{
			UPDATE_LOG(pProcess, L_CANT_READ_DATA)
			//读取不到数据
			ret = defUPDATE_RESULT_DOWNLOAD_FAILED;
		}

		char szFormat[MAX_PATH];
		::_snprintf(szFormat, MAX_PATH, "AutoUpdateDLL return code %d", ret);
		UPDATE_LOG(pProcess, szFormat)
		if (ret >= 0 && (ret == defUPDATE_RESULT_INIT_FAILED || ret == defUPDATE_RESULT_DOWNLOAD_INDEX_FAILED ||
			ret == defUPDATE_RESULT_PROCESS_INDEX_FAILED || ret == defUPDATE_RESULT_CONNECT_SERVER_FAILED ||
			ret == defUPDATE_RESULT_DOWNLOAD_FAILED) || ret == defUPDATE_RESULT_UPDATE_FAILED)
		{
			continue;
		}
		else if (ret == defUPDATE_RESULT_CANCEL)
		{
			UPDATE_LOG(pProcess, L_CANCEL_DOWNLOAD)
			pProcess->ToQuit();
			break;
		}
		else if(ret == 100)
		{
			pProcess->ToQuit();
			break;
		}
		else if(ret == 101)
		{
			//		g_pAutoupdateDlg->EndDialog(IDCANCEL);
			pProcess->ToQuit();
			break;
		}
		else if(ret == 102)
		{
			pProcess->ToQuit();
			break;
		}
		else if (ret == defUPDATE_RESULT_UPDATESELF_SUCCESS)
		{
			UPDATE_LOG(pProcess, L_DOWN_AUTOUPDATE_SUCC)
			pCallBack->NotifyProcessStatus(U_TO_RESTART_SELF);
			pCallBack->NotifyOverallRate(100);
			pCallBack->NotifyCurrentRate(100);
			pCallBack->NotifyResult(TRUE, TRUE);
			// 在此写入文件，避免下次还要选择升级服务器
			pProcess->m_clsUpdateSelection.SetLastSelection(theApp.GetUpdateDir(), theApp.GetApplication());
/*
			char szModulePath[MAX_PATH + 1];
			g_GetRootPath(szModulePath);
			CString strUpdateDirIni = szModulePath;
			strUpdateDirIni += "\\Update\\";
			strUpdateDirIni += INIFILE_UPDATEDIR;
			WritePrivateProfileString("UpdateDir", "Dir", theApp.GetUpdateDir().c_str(), strUpdateDirIni);
			WritePrivateProfileString("UpdateDir", "Application", theApp.GetApplication().c_str(), strUpdateDirIni);
*/
			pProcess->ToQuit();
			theApp.DemandReboot();
			pCallBack->NotifyClose(IDOK);
		}
		else if (ret == defUPDATE_RESULT_NOT_UPDATE_FILE )
		{
			pCallBack->NotifyProcessStatus(U_NEEDLESS_UPDATE);
			pCallBack->NotifyOverallRate(100);
			pCallBack->NotifyCurrentRate(100);
			pCallBack->NotifyResult(TRUE, TRUE);

			pProcess->ToQuit();
		}//end else
		else
		{
			UPDATE_LOG(pProcess, L_UPDAE_SUCC)
			pCallBack->NotifyProcessStatus(U_UPDATE_FINISH);
			pCallBack->NotifyOverallRate(100);
			pCallBack->NotifyCurrentRate(100);
			pCallBack->NotifyResult(TRUE, TRUE);

			pProcess->ToQuit();
		}//end else

		// 版本号变更
		int nMajor = 0;
		int nMinor = 0;
		pProcess->GetVersion(&nMajor, &nMinor);
		ASSERT(nMajor >= 0 && nMinor >= 0);
		pCallBack->NotifyVersion(nMajor, nMinor);
		ret = 0;
		break;
	}

	if (!pSeverInfo && !pProcess->IsToQuit())
	{
		UPDATE_LOG(pProcess, U_NO_AVAILABLE_SERVER)
		pCallBack->NotifyImportant();
		pCallBack->NotifyProcessStatus(U_FAILT_BUT_CAN_TRY_2);
		pCallBack->NotifyResult(FALSE, TRUE);
		pCallBack->NotifyOverallRate(0);
		pCallBack->NotifyCurrentRate(0);
	}
	pProcess->Activate(FALSE);
	return ret;
}

void CUpdateProcess::Run()
{
	ASSERT(!m_bActive);
	BOOL bResult = InitAutoUpdate();
	ASSERT(bResult);
	if (bResult)
	{
		DWORD dwThread = 0;
		HANDLE hThread = ::CreateThread(
			NULL,
			0,
			CUpdateProcess::UpdateProcess,
			this,
			0,
			&dwThread);
		ASSERT(hThread);
		Activate(TRUE);
		CloseHandle(hThread);
	}

	//add by zuolizhi 2004/09/02
	StartDownLoadBackGround();
}

//*********************************************************************
// function : 初始化升级选项
//*********************************************************************
void CUpdateProcess::InitializeSelection()
{
	// 读取上次保存的升级目录选项后，立即置空
	char szPath[MAX_PATH];
	::g_GetRootPath(szPath);
	m_clsUpdateSelection.Initialize(szPath);
	string strDir, strApp;
	m_clsUpdateSelection.GetLastSelection(strDir, strApp);
	m_clsUpdateSelection.DelLastSelection();
	theApp.SetUpdateDir(strDir.c_str());
	if (!strApp.empty())
	{
		theApp.SetApplication(strApp.c_str());
	}
	else
	{
		CString strApplication;
		strApplication.Format(IDS_STR_DEFAULTAPP);
		ASSERT(!strApplication.IsEmpty());
		strApp.append(szPath);
		strApp.append("\\");
		strApp.append((LPCSTR)strApplication);
		theApp.SetApplication(strApp.c_str());
	}
}

//*********************************************************************
// function : 初始化
//*********************************************************************
BOOL CUpdateProcess::InitAutoUpdate()
{ //<------EXVERSION
	BOOL bResult;

	if (!g_bExServerMode)
		bResult = g_PakList.Open("\\package.ini");
	else
		bResult = g_PakList.Open("\\packageex.ini");

	KIniFile	IniFile;
	LPCSTR autoupdatestr = NULL;
	
	if (!g_bExServerMode)
		autoupdatestr = "\\settings\\autoupdate.ini";
	else
		autoupdatestr = "\\settings\\autoupdateex.ini";
	
	if (!IniFile.Load(autoupdatestr))
		return FALSE;
	
	char run[MAX_PATH];
	IniFile.GetString("main", "game", "FSOnline2.exe", run, MAX_PATH);
	
	char szModulePath[MAX_PATH + 1];
	g_GetRootPath(szModulePath);
	CString strPath = szModulePath;
	strPath += "\\";
	theApp.SetUpdateself(strPath + "\\Update\\" + "UpdateSelf.DAT");
	m_strSiteList = strPath + "\\Update\\" + "SiteList.ini";
	m_nCurrentHost = -1;
	m_strHosts.RemoveAll();
	
	// 初始化升级目录选项
	InitializeSelection();
    
	//This Code bellow mignt not be used! first the site_ini might in a package
	//that fopen can't find. The second,Update\SitList.ini is not exist in a 
	//Un packed virsion because we use the ftplist to Find The host.  
	//Comment by Brianyao 2007

	FILE *site_ini = fopen(m_strSiteList, "r");
	if (site_ini)
	{
		char line[100];
		while(fgets(line, 100, site_ini) != NULL)
		{
			if(m_nCurrentHost == -1)
			{
				m_nCurrentHost = atoi(line);
				if(m_nCurrentHost == -1) m_nCurrentHost = 0;
			}
			else 
			{
				int last = strlen(line) - 1;
				if(line[last] == '\n')
				{
					line[last--] = 0;
				}
				if(line[last] == '\r')
				{
					line[last] = 0;
				}
				m_strHosts.Add(line);
			}
		}
		fclose(site_ini);
		remove(m_strSiteList);
		IniFile.GetInteger("main", "log", 0, &g_bLog);
		g_PakList.Close();
		if(m_nCurrentHost >= m_strHosts.GetSize()) m_nCurrentHost = 0;
		return (m_strHosts.GetSize() >= 0);
	}//end of 	if (site_ini)	

	char szSite[MAX_PATH];
	int n = 1;

	CString strSite;
	CStringArray strHosts;
	do {
		szSite[0] = 0;
		strSite.Format("ftpsite%d", n);
		IniFile.GetString("main", strSite, "", szSite, MAX_PATH);
		if (szSite[0] != 0)
			strHosts.Add(szSite);
		else
			break;
		n++;
	} while(szSite[0] != 0);
	
	if (strHosts.GetSize() > 0)
	{
		srand( (unsigned)time( NULL ) );		
		int nStart = rand() % strHosts.GetSize();
		int i;
		for (i = nStart; i < strHosts.GetSize(); i++)
		{
			m_strHosts.Add(strHosts[i]);
		}
		for (i = 0; i < nStart; i++)
		{
			m_strHosts.Add(strHosts[i]);
		}
		ASSERT(m_strHosts.GetSize() == strHosts.GetSize());
		m_nCurrentHost = 0;

		//m_nConnectionStep = ConnectionProgress/(m_strHosts.GetSize());
	}
	else
		m_nCurrentHost = -1;

	//Del By Brianyao 2007 We always need the log when updating...
    //	IniFile.GetInteger("main", "log", 0, &g_bLog);
	g_PakList.Close();

	return (m_nCurrentHost >= 0);
}

LPCSTR CUpdateProcess::GetNextUpdateServer(int& nCurrent)
{
	if ((!IsToQuit()) && m_nCurrentHost >= 0 && m_nCurrentHost < m_strHosts.GetSize())
	{
		nCurrent = (m_nCurrentHost++);
		return ((LPCSTR)m_strHosts[nCurrent]);
	}
	nCurrent = 0;
	return NULL;
}

void CUpdateProcess::Retry() 
{
	//重试连接
	m_nCurrentHost = 0;
	theApp.SetUpdateself("");
	m_bEnableRun = FALSE;

	Run();
}

//*********************************************************************
// function : 取消升级
// return	: void
//*********************************************************************
void CUpdateProcess::Cancel()
{
	// 必须在内核升级过程中才可以调用g_Update_Cancel()
	ASSERT(g_Update_Cancel);
	m_kernel.Cancel();
	m_kernel.LockKernel(TRUE);
	if (g_Update_Cancel && m_kernel.InKernel())
		g_Update_Cancel();
	m_kernel.LockKernel(FALSE);

	// 因为是取消操作，所以删除记录升级目录的文件，使下次启动升级程序时可以重新选择升级目录
	m_clsUpdateSelection.DelLastSelection();
}

//*********************************************************************
// function : 获取版本号
// parameter: pnMajor 主版本号
// parameter: pnMinor 副版本号
// return void
//*********************************************************************
void CUpdateProcess::GetVersion(int *pnMajor, int *pnMinor)
{
	ASSERT(pnMajor && pnMinor);
	char aCfgPath[MAX_PATH];
	g_GetRootPath(aCfgPath);
	if (!g_bExServerMode) //<------EXVERSION
		::strcat(aCfgPath, "\\Version.cfg");
	else
		::strcat(aCfgPath, "\\Versionex.cfg");

	int nMajor = ::GetPrivateProfileInt("Version", "MajorVersion", 0, aCfgPath);
	int nMinor = ::GetPrivateProfileInt("Version", "Version", 0, aCfgPath);
	
	//nMajor -= 1;
	
	if (nMajor < 0) 
		nMajor = 0;

// 	if(nMinor <= 122)
// 		nMinor = 0;
// 	else
// 		nMinor -= 122;

	*pnMajor = nMajor;
	*pnMinor = nMinor;
}

//*********************************************************************
// function : 要求退出
//*********************************************************************
void CUpdateProcess::ToQuit()
{
	m_sec.Lock();
	m_bToQuit = TRUE;
	m_sec.Unlock();
}

//*********************************************************************
// function : 是否要退出
//*********************************************************************
BOOL CUpdateProcess::IsToQuit()
{
	m_sec.Lock();
	BOOL bToQuit = m_bToQuit;
	m_sec.Unlock();
	return bToQuit;
}

//*********************************************************************
// function : 是否正在运行
//*********************************************************************
BOOL CUpdateProcess::IsActive()
{
	m_sec.Lock();
	BOOL bActive = m_bActive;
	m_sec.Unlock();
	return bActive;
}

//*********************************************************************
// function : 设置是否正在运行
//*********************************************************************
void CUpdateProcess::Activate(BOOL bActive)
{
	m_sec.Lock();
	m_bActive = bActive;
	m_sec.Unlock();
}

//*********************************************************************
// function	: 复位
//*********************************************************************
void CUpdateProcess::Reset()
{
	m_nCurrentHost = 0;
	m_clsUpdateSelection.Reset();
	theApp.Reset();
	m_bEnableRun = FALSE;
	ASSERT(!m_kernel.InKernel());
	m_kernel.Reset();
	m_bToQuit = FALSE;
}

//*********************************************************************
// function : 创建对象实例
//*********************************************************************
CUpdateProcess *CUpdateProcess::CreateInstance()
{
	CUpdateProcess *pProcess = new CUpdateProcess;
	ASSERT(pProcess);
	if (!pProcess->InitOnce())
	{
		pProcess->Release();
		pProcess = NULL;
	}
	return pProcess;
}

//CRC........................................................................................................
typedef unsigned __int32 CRC_UINT32;
static const CRC_UINT32 CRC_Table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

unsigned CUpdateProcess::CRC32(unsigned CRC, const void *pvBuf, int nLen)
{
    unsigned RetCode = 0;

    if (!pvBuf)
        return RetCode;

    __asm mov esi, [pvBuf]
    __asm mov ecx, [nLen]
    __asm mov eax, [CRC]
    __asm shr ecx, 3
    __asm xor eax, 0xffffffff

    __asm test ecx, ecx
    __asm jz CRC_Last7Bytes
    
    __asm push ebp
    __asm mov ebp, ecx

    __asm xor edx, edx
    __asm xor ecx, ecx

    __asm align 4

CRC_Loop1:
    // eax = CRC
    // edx first TableIndex
    // ecx second TableIndex

    // Process 4 bytes
    __asm mov ebx, [esi]
    __asm add esi, 4
    
    __asm mov dl, bl
    __asm mov cl, bh
    __asm xor dl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[edx * 4]
    __asm shr ebx, 16
    __asm xor eax, edi

    __asm xor cl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[ecx * 4]
    __asm mov dl, bl
    __asm xor eax, edi

    __asm xor dl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[edx * 4] 
    __asm mov cl, bh
    __asm xor eax, edi

    __asm mov ebx, [esi]
    __asm xor cl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[ecx * 4]
    __asm add esi, 4
    __asm xor eax, edi
    
    __asm mov dl, bl
    __asm mov cl, bh
    __asm xor dl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[edx * 4]
    __asm shr ebx, 16
    __asm xor eax, edi

    __asm xor cl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[ecx * 4]
    __asm mov dl, bl
    __asm xor eax, edi

    __asm xor dl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[edx * 4] 
    __asm mov cl, bh
    __asm xor eax, edi

    __asm xor cl, al
    __asm shr eax, 8
    __asm xor eax, CRC_Table[ecx * 4]

    __asm dec ebp
    __asm jnz CRC_Loop1

    __asm pop ebp

CRC_Last7Bytes:

    __asm mov ecx, [nLen]

    __asm xor edx, edx
    __asm and ecx, 0x7
    __asm jz CRC_Exit

CRC_Loop2:

    __asm mov dl, [esi]
    __asm inc esi
    __asm xor dl, al
    __asm shr eax, 8
    __asm xor eax, CRC_Table[edx * 4]

    __asm dec ecx
    __asm jnz CRC_Loop2

CRC_Exit:

    __asm xor eax, 0xffffffff
    __asm mov [RetCode], eax

    return RetCode;
}


BOOL CUpdateProcess::DownLoadFromPath(const char * szPath,const char * szDestPath)
{
	if (szPath==NULL)
		return FALSE;

	if (szPath[0]=='h' || szPath[0]=='H')
		return CDownLoadFile::GetFileByHttp(szPath,szDestPath, CONNECTION_TIMEOUT, CONNECTION_TIMEOUT, CONNECTION_TIMEOUT);
	else if (szPath[0]=='F'|| szPath[0]=='f')
		return CDownLoadFile::GetFileByFtp(szPath,szDestPath, CONNECTION_TIMEOUT, CONNECTION_TIMEOUT, CONNECTION_TIMEOUT);
    
	return FALSE;
}


















