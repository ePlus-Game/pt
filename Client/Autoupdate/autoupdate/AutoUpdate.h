// AutoUpdate.h : main header file for the AUTOUPDATE application
//

#if !defined(AFX_AUTOUPDATE_H__A5EF2DE4_22B5_480F_9221_77762C876328__INCLUDED_)
#define AFX_AUTOUPDATE_H__A5EF2DE4_22B5_480F_9221_77762C876328__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <string>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CAutoUpdateApp:
// See AutoUpdate.cpp for the implementation of this class
//

#define WM_NOTIFY_REFRESH_HSTRYLIST	WM_USER + 112

class CAutoUpdateDlg;
class CAutoUpdateApp : public CWinApp
{
public:
	CAutoUpdateApp();
	
	void Reset();
	void DemandReboot();	// 要求重新启动程序
	void DemandEnterGame(); // 已经进入游戏
	//设置要下载程序的名称
	void SetApplication(LPCSTR pcszApplication)
	{
		ASSERT(pcszApplication && ::strlen(pcszApplication));
		m_strApplication = pcszApplication;
	}
	string GetApplication()
	{
		return m_strApplication;
	}
	void SetUpdateself(LPCSTR pcszUpdateself)
	{
		ASSERT(pcszUpdateself);
		m_strUpdateSelf = pcszUpdateself;
	}
	void SetUpdateDir(LPCSTR pcszUpdateDir)
	{
		ASSERT(pcszUpdateDir);
		m_strUpdateDir = pcszUpdateDir;
	}
	string GetUpdateDir()
	{
		return m_strUpdateDir;
	}
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoUpdateApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
	
	// Implementation
	
	//{{AFX_MSG(CAutoUpdateApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
private:
	//*****************************************************************
	// *Description	: 检查运行环境
	// *parameter	: strMessage 错误信息，返回FALSE时有效
	// *Return      : TRUE--检查通过返回; FALSE--检查不通过
	// *Comment     : 如果Game.exe在运行，或者FSOnline.exe已经有一个实例在运行，则检查不通过
	//*****************************************************************
	BOOL IsValidInstance(string &strMessage);
	//*****************************************************************
	// *Description	: 退出时是否打开指定URL
	//*****************************************************************
	BOOL CanOpenUrlWhenExit();
	//*****************************************************************
	// *Description	: 是否有其他升级实例在运行
	//*****************************************************************
	BOOL IsAnotherUpdateRunning();
	//*****************************************************************
	// *Description	: 是否游戏程序在运行
	//*****************************************************************
	BOOL IsFsGameRunning();	
	//初始化路径
	BOOL InitWorkFolder();
private:
	BOOL FindAnotherUpdate();
	BOOL FindNeedFileForEx();	 //<------EXVERSION
	BOOL m_bNeedReboot;
	BOOL m_bEnterGame;
	BOOL m_bInitialized;	// 初始化成功
	string m_strUpdateSelf;
	string m_strUpdateDir;

public:
	string m_strApplication;
	UINT m_RegisterMsg;
};

extern CAutoUpdateApp theApp;
extern BOOL g_bExServerMode;  //<------EXVERSION

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOUPDATE_H__A5EF2DE4_22B5_480F_9221_77762C876328__INCLUDED_)
