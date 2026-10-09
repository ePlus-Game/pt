// UpdateDLL.h : main header file for the UPDATEDLL DLL
//

#if !defined(AFX_UPDATEDLL_H__A3D11B44_BB0A_4695_BD1B_CD93A1F0F616__INCLUDED_)
#define AFX_UPDATEDLL_H__A3D11B44_BB0A_4695_BD1B_CD93A1F0F616__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "UpdateExport.h"
#include "DownNotify.h"
#include "MsgWnd.h"
#include "ProcessIndex.h"

/////////////////////////////////////////////////////////////////////////////
// CUpdateDLLApp
// See UpdateDLL.cpp for the implementation of this class
//
#define DOWNINDEX_STEPINDEX                 1
#define DOWNINDEX_STEPOVER					2
#define DOWNINDEX_STEPCHKSNHTTP             3
#define DOWNINDEX_STEPCHKSNUDP              4

extern HANDLE	g_hUpdateMutex;

class CUpdateDLLApp : public CWinApp
					, public CDownNotify
{	
public:
	CUpdateDLLApp();
	virtual ~CUpdateDLLApp();

	int Init(const KUPDATE_SETTING& UpdateSetting);
    int UnInit();
	int CancelDownload();

	//合法用户的验证，游戏升级过程中不需要改步
	//只保留其接口，去除其实现，若需要增加请参考旧版本的Update.dll
	int UserVerify();	
    int CheckNeedUpdate();  
    int Download();
    int Update();

    BOOL DownDispatch(const MSG *pMsg);
	//Add by Brianyao
	//If CanUpdate :When the Game is Entered ,Can't Update !
	BOOL GetCanUpdate(void)const;
	VOID ReSetCanUpdate(void);
	BOOL GetServerMode() 	//<------EXVERSION
	{
		return m_bExServerMode;
	}
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateDLLApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CUpdateDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    FN_UPDATE_CALLBACK *m_pfnCallBackProc;
private:
	//下载文件的状态处理函数
	virtual ULONG OnStatusFileDowned(PDOWNLOADSTATUS pDownStatus);    
	//下载结果的处理函数
    virtual ULONG OnDownResult(ULONG ulOverResult);
	//步骤控制函数
	int ProcessNextStep(int nDownStep);	
	//下载下一个文件的函数
	int DownNextItem(BOOL nRedownCurrent = FALSE);

	BOOL RegisterMyWindow();
	BOOL InitWinSock();
	BOOL InitPaths();
	BOOL LoadUpdateData(const KUPDATE_SETTING& UpdateSetting);
	
	BOOL SetDownloadProxy(const char *lpszHostUrl);
 //<------EXVERSION
	
private:
	CMsgWnd m_wndMessage;			//接受消息的自定义窗口
	int m_nResultCode;				//返回结果，因为下载过程通过消息传递，所以定义为全局
    HANDLE m_hExitNotify;			//下载结束事件，主进程开辟下载线程，受到该事件表示下载完毕
	int m_nInitWSA;					//winsock是否初始化成功
	int m_nInitFlag;				//是否成功进行初始化
	int m_nProcessStep;				//当前下载的步骤
	CDownloadFile m_DownloadFile;	//下载主管
	KUPDATE_ITEM *m_pCurDownItem;	//当前下载的节点
	int m_nCurEnableResume;
	CString m_strIndexTempFile;
	BOOL m_bExServerMode; //<------EXVERSION

	//强制更新修改 Add by Brianyao 2007
	//如果游戏进入了，把这个位置设置成FALSE 默认TRUE
	LONG m_bUpdate;
};

extern CUpdateDLLApp g_theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEDLL_H__A3D11B44_BB0A_4695_BD1B_CD93A1F0F616__INCLUDED_)
