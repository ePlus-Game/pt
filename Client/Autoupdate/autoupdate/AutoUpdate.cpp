// AutoUpdate.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "AutoUpdateDlg.h"
#include <tlhelp32.h>
#include "GameGuid.h"
#include "KWin32.h"
#include "KPakList.h"
#include "KFilePath.h"
#include "KIniFile.h"
#include "Encrypter.h"
#include "KExServerDlg.h"   //<------EXVERSION

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define		WAKEUP_MESSAGE	"WakeUpFS2AutoRunMessage"

BOOL g_bExServerMode = FALSE;  //<------EXVERSION

/////////////////////////////////////////////////////////////////////////////
// CAutoUpdateApp

KPakList g_PakList;

BEGIN_MESSAGE_MAP(CAutoUpdateApp, CWinApp)
	//{{AFX_MSG_MAP(CAutoUpdateApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoUpdateApp construction

CAutoUpdateApp::CAutoUpdateApp()
	:m_bNeedReboot(FALSE),
	m_bEnterGame(FALSE),
	m_bInitialized(FALSE)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAutoUpdateApp object

CAutoUpdateApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAutoUpdateApp initialization
#define ERROR_STRING_MESSAGE_INIT_ERROR "消息初始化错误"
#define ERROR_STRING_MESSAGE_MORE_RUN   "请不要同时运行多个自动更新程序"

BOOL CAutoUpdateApp::InitInstance()
{

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	//得到路径
	InitWorkFolder();

	//随机的目的是为了随机形成服务器列表用
	srand((unsigned int)time(NULL));	

	m_RegisterMsg = RegisterWindowMessage(WAKEUP_MESSAGE);
	if(0 == m_RegisterMsg)
	{
		CString strWarning;
		strWarning.Format(ERROR_STRING_MESSAGE_INIT_ERROR);
		AfxMessageBox( (LPCTSTR)strWarning );

		return FALSE;
	}//endif

	//判断升级程序或者封神是否正在运行 ，如果不允许双开，可以把下面代码开放
/*	if ( IsFsGameRunning() )
	{
		CString strWarning;
		strWarning.Format(IDS_ERR_GAMEAPP_RUNNING);
		AfxMessageBox( (LPCTSTR)strWarning );
		return FALSE;
	}
*/
	if ( IsAnotherUpdateRunning() )
	{
		DWORD dwRecver = BSM_APPLICATIONS;
		BroadcastSystemMessage(BSF_IGNORECURRENTTASK,
			&dwRecver,
			m_RegisterMsg,
			NULL,
			NULL
			);

		CString strWarning;
		strWarning.Format(ERROR_STRING_MESSAGE_MORE_RUN);
		AfxMessageBox( (LPCTSTR)strWarning );

		return FALSE;
	}

// 	string strMessage;
// 	if (!IsValidInstance(strMessage))
// 	{
// 		AfxMessageBox(strMessage.c_str());
// 		return FALSE;	
// 	}

	ASSERT(!m_bInitialized);
	m_bInitialized = TRUE;
	 //<------EXVERSION
// 	if (FindNeedFileForEx())
// 	{
// 		CExServerDlg dlgOnec;
// 		int nResponse = dlgOnec.DoModal();
// 
// 		if (nResponse == IDCANCEL)
// 			return FALSE;
// 	}

	Reset();
	CAutoUpdateDlg dlg;
	dlg.SetMainApp(this);
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	return FALSE;
}

//初始化路径
BOOL CAutoUpdateApp::InitWorkFolder()
{
	char szModulePath[MAX_PATH + 1];
	::GetModuleFileName(NULL, szModulePath, (MAX_PATH + 1));
	
	char *pszOffset = NULL;
	pszOffset = strrchr(szModulePath, '\\');
    ASSERT(pszOffset);
	if (pszOffset)
		pszOffset[1] = '\0';
	g_SetRootPath(szModulePath);

	return TRUE;
}

//*****************************************************************
// *Description	: 是否有其他升级实例在运行
//*****************************************************************
BOOL CAutoUpdateApp::IsAnotherUpdateRunning()
{
	//因为自动更新更新自己完毕后，退出，重新启动时，会有瞬间的判断到自己还没有退出
	//采用延时的方法等待自己退出
	int nCount = 3;
	while (nCount > 0)
	{
		if (!FindAnotherUpdate())
			return FALSE;
		else
		{
			Sleep(50);
		}

		nCount --;
	}

	return TRUE;
}

//*****************************************************************
// *Description	: 是否游戏程序在运行
//*****************************************************************
BOOL CAutoUpdateApp::IsFsGameRunning()
{
	HANDLE hSemaphore = ::OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, FS2ONLINE_GUID);
	if (hSemaphore)
		::CloseHandle(hSemaphore);
	return hSemaphore != NULL;
}

//*****************************************************************
// *Description	: 检查运行环境
// *parameter	: strMessage 错误信息，返回FALSE时有效
// *Return      : TRUE--检查通过返回; FALSE--检查不通过
// *Comment     : 如果Game.exe在运行，或者Fsnline.exe已经有一个实例在运行，则检查不通过
//*****************************************************************
BOOL CAutoUpdateApp::IsValidInstance(string &strMessage)
{
	if (IsFsGameRunning())
	{
		CString strWarning;
		strWarning.Format(IDS_ERR_GAMEAPP_RUNNING);
		strMessage = (LPCSTR)strWarning;
		return FALSE;
	}
	if (IsAnotherUpdateRunning())
	{
		
		CString strWarning;
		strWarning.Format(IDS_ERR_AUTOUPDATE_RUNNING);
		strMessage = (LPCSTR)strWarning;
		return FALSE;
	}
	return TRUE;
}

BOOL OnExecute(LPCSTR szFile);

//*****************************************************************
// *Description	: 退出时是否打开指定URL
//*****************************************************************
BOOL CAutoUpdateApp::CanOpenUrlWhenExit()
{
	char szPath[MAX_PATH] = {0};
	::g_GetRootPath(szPath);
	::strcat(szPath, "\\");
	::strcat(szPath, "config.ini");
#ifndef TRADITIONAL_CHINESE
	return ::GetPrivateProfileInt("Client", "CanOpenUrlWhenExit", 1, szPath) != 0;
#else
	return ::GetPrivateProfileInt("Client", "CanOpenUrlWhenExit", 0, szPath) != 0;
#endif
}

int CAutoUpdateApp::ExitInstance()
{
	int	nRet = CWinApp::ExitInstance();
	
	if (m_bNeedReboot)
	{
		ASSERT(!m_strUpdateSelf.empty());
		OnExecute(m_strUpdateSelf.c_str());
	}
	else if (m_bEnterGame)
	{
		//更新逻辑修改 Edit by Brianyao
	/*	ASSERT(!m_strApplication.empty());
		OnExecute(m_strApplication.c_str());*/

	}
	else if (m_bInitialized && CanOpenUrlWhenExit())
	{
		// 手动关闭时打开指定连接
		//::ShellExecute(0, "open", K_KINGSOFT_URL, 0, 0, SW_SHOWNORMAL);
		//		CUrl().Open(K_KINGSOFT_URL);
	}
	return nRet;
}

void CAutoUpdateApp::DemandReboot()
{
	// 要求重新启动程序
	m_bNeedReboot = TRUE;
}

void CAutoUpdateApp::DemandEnterGame()
{
	m_bEnterGame = TRUE;
}

void CAutoUpdateApp::Reset()
{
	m_bNeedReboot = FALSE;
	m_bEnterGame = FALSE;
	m_strApplication = "";
	m_strUpdateSelf = "";
	m_strUpdateDir = "";
}

BOOL CAutoUpdateApp::FindAnotherUpdate()
{
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	ASSERT(handle);
	PROCESSENTRY32 RealInfo  = {0};	
	PROCESSENTRY32* info = &RealInfo;
    info->dwSize=sizeof(PROCESSENTRY32);
	int nCount = 0;
	if(Process32First(handle,info))
	{
		char aStr[MAX_PATH];
		GetFileTitle(info->szExeFile,aStr,MAX_PATH);
		CString aName = aStr;
		if(aName.CompareNoCase(K_JXONLINE_FILE_NAME_0) == 0 ||
			aName.CompareNoCase(K_JXONLINE_FILE_NAME_1) == 0)
		{
			nCount++;
		}
		while(Process32Next(handle,info)!=FALSE)
		{
			GetFileTitle(info->szExeFile,aStr,MAX_PATH);
			aName = aStr;
			if(aName.CompareNoCase(K_JXONLINE_FILE_NAME_0) == 0 ||
				aName.CompareNoCase(K_JXONLINE_FILE_NAME_1) == 0)
			{
				nCount++;
			}
		}
	}
	CloseHandle(handle);
	handle = NULL;
	return nCount > 1;
}
 //<------EXVERSION
BOOL CAutoUpdateApp::FindNeedFileForEx()
{
	BOOL  bResult = g_PakList.Open("\\package.ini");

	if (!bResult)
		return FALSE;

	KIniFile iniFile;

	if (!iniFile.Load("\\settings\\autoupdateex.ini"))
	{
		return FALSE;
	}
	
	HANDLE handle = ::CreateFile("Packageex.ini", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (INVALID_HANDLE_VALUE == handle)
		return FALSE;
	
	CloseHandle(handle);

	g_PakList.Close();
	
	return TRUE;
}
