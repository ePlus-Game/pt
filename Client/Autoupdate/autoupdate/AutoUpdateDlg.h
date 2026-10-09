// AutoUpdateDlg.h : header file
//
//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES

#include "ProgressCtrlST.h"
#include "bmpbutton.h"
#include "AutoUpdate.h"
//#include "TransTreeCtrl.h"

#if !defined(AFX_AUTOUPDATEDLG_H__EAC109CF_6E87_44E2_96A2_A5C6EA3EF84C__INCLUDED_)
#define AFX_AUTOUPDATEDLG_H__EAC109CF_6E87_44E2_96A2_A5C6EA3EF84C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAutoUpdateDlg dialog
#include "webbrowser2.h"
#include "WndCallBack.h"
#include "HyperlinkStatic.h"
#include "Picture.h"
#include "WndTool.h"
#include <Afxmt.h>
#include "BitmapDialog.h"
#include "DlgShowInfo.h"
#include "TransparentStatic.h"
#include "BitmapSlider.h"
#include "PictureEx.h"
#include <exdispid.h>
#include <afxdisp.h>
#include <string>
#include "ServerListDlg.h"
//#include "UpdateTipDlg.h"
// 空连接
#define URL_NULL				""

enum UIParam
{
		PROGRESS_MINRATE = 0,	// 最小进度
		PROGRESS_MAXRATE = 100,	// 最大进度
		DIALOG_WIDTH	 = 640,	// 对话框宽度
#ifndef TRADITIONAL_CHINESE
		DIALOG_HEIGHT	 = 510,	// 对话框高度
#else
		DIALOG_HEIGHT	 = 514,	// 对话框高度
#endif
//		COUNT_CONTROL	 = 6,	// 静态控件的数量
		COUNT_CONTROL    = 5,
		COUNT_LINKURL	 = 1,	// 超级链接的数量
//		COUNT_BMPBUTTON	 = 6,	// 位图按钮的个数
		COUNT_BMPBUTTON  = 5,
		COUNT_URLBTN	 = 0,	// 位图URL按钮的个数
};

class CUpdateProcess;
class CIEComCtrlSink;

class CBkClrComboBox : public CComboBox
{
protected:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};


class CAutoUpdateDlg : public CBitmapDialog, public CWndCallBack, public CWndShell
{
// Construction
public:
	std::wstring strSelServerInfo;
	CAutoUpdateDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CAutoUpdateDlg();
	
// Dialog Data
	//{{AFX_DATA(CAutoUpdateDlg)
	enum { IDD = IDD_AUTOUPDATE_DIALOG };
//	CPictureEx	m_Picture;
//	CPictureEx  m_Ball;
	CBitmapSlider	m_prgOverallRate;
	CBitmapSlider	m_prgCurrentRate;
	CBmpButton	m_btnMinClose;
	CBmpButton	m_btnMinimize;
	CTransparentStatic	m_ShowCaption;
//	CTransparentStatic  m_CurrentServer;    //目前选择的服务器
//	CBkClrComboBox	m_HstryLoginServList;
//	CBmpButton	m_btnUpdate;     //进行更新按钮 Edit by Brianyao2007
	CBmpButton	m_btnPlay;
	CBmpButton	m_btnClose;
	CBmpButton	m_btnSetting;
	CHyperlinkStatic	m_stcProgressMsg;
//	CHyperlinkStatic	m_updateMannerMsg;
	CWebBrowser2	    m_ctrlBrowser;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoUpdateDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
public:
	void SetMainApp(CAutoUpdateApp *theApp);

	virtual BOOL PreTranslateMessage(MSG *pMsg);
	
	//回调函数相关
	virtual void NotifyClose(int nResult);					// 进程结束
	virtual void NotifyProcessStatus(LPCSTR pcszMessage);	// 状态消息
	virtual void NotifyCurrentRate(int nRate);				// 当前进度
	virtual void NotifyOverallRate(int nRate);				// 整体进度
	virtual void NotifyResult(BOOL bSuccess, BOOL bCanPlay);// 结果通知，TRUE-成功;FALSE-失败
	virtual void NotifyVersion(int nMajor, int nMinor);		// 版本号通知
//	virtual void NotifyServerList(void);                    // 服务器列表更新 Add by Brianyao 2007
	virtual void NotifyImportant();							//设置显示字体等
	virtual int Refresh(int nStatus, long lParam);			//界面更新		

	bool	HasGetUpdateMode()
	{
		return m_HasGetUpdateMode;
	}

	void	SetUpdateModeFlag(bool bFlag)
	{
		m_HasGetUpdateMode = bFlag;
	}
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAutoUpdateDlg)
	virtual BOOL OnInitDialog();
	afx_msg HCURSOR OnQueryDragIcon();
//	afx_msg void OnBtnUpdate();
	afx_msg void OnBtnPlay();
	afx_msg void OnBtnClose();
	afx_msg void OnBtnSetting();
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMinimize();
//	afx_msg void OnHstryListSelChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated message map functions
	//{{AFX_MSG(CAutoUpdateDlg)
	afx_msg BOOL OnWebCompletion(const char * sbUrl);
	afx_msg void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) ;
	//}}AFX_MSG
	DECLARE_EVENTSINK_MAP()
private:
	//初始化界面的函数
	void InitUI();
	//启动下载线程
	BOOL StartDownload();
	
	//显示网页的线程
	static DWORD WINAPI ShowWebThreadFunc(LPVOID lpParam);
	static DWORD WINAPI ShowMainWebThreadFunc(LPVOID lpParam);
	int _ShowWebThreadFunc();
	
	//下载函数，使用update.dll中提供的函数下载一个文件
	BOOL GetFileFromUrl( const char *pUrl, BOOL bText, const char *pCachePathName);

	//界面更新相关
	//是否在界面更新过程中
	BOOL IsRefreshing();
	//设置是否在界面更新过程中
	void SetRefreshStatus(BOOL bRefreshing);

	//辅助函数
	//实际的关闭消息处理函数
	static void OnNotifyClose(WPARAM wParam, LPARAM lParam);
	//实际的更新进度状态消息函数	
	static void OnNotifyProgressStatus(void *pParam);

	//显示标题
	void ShowCaption(CUpdateProcess *pProcess);
	//得到标题
	string GetCaption(int nMajor, int nMinor);

	//设置窗体上的控件为透明
	virtual void OnPostEraseBkgnd(CDC* pDC);

	//根据资源形成默认的网页
	BOOL FindResource(const char *pszDefalutHtml);
	BOOL MakeDefaultMht(BYTE *pBuffer, int nSize, const char *pszTempFileName);

//	void RefreshHstryServList();
/*	void SelectTargetItem(HTREEITEM hItem, 
		const char *szNetName, 
		const char *szRegionName, 
		const char *szServerName
		);
*/
	//详细列表文件信息
	void AdjustWindowPos();
	void OnChildClose();

	BOOL CanServListReadDirectly(const char *szFile);

private:
	enum {
		THREAD_WAITTIME = 100,
	};	// 等待线程事件，单位是毫秒
	typedef void (*fnOnApplicationQuit)(WPARAM, LPARAM);

	//界面相关变量
	CPicture  m_picBackGround;
	//静态控件的窗口位置	
	static WindowRect m_rectStaticCtl[COUNT_CONTROL + 1];
	//位图按钮的个数	
	static BMPButton  m_bmpBtns[COUNT_BMPBUTTON + 1];
	//URL控件的链接内容
	static UrlLink m_textStaticUrl[COUNT_LINKURL + 1];
	// 进度条进度快的颜色
	COLORREF		m_colorProgress;
	BOOL			m_bRefreshing;		// 界面更新过程中
	
	//升级对象
	CUpdateProcess	*m_pProcess;
	
	// 界面更新标志临界区
	CCriticalSection m_csRefreshing;
	//客户端不是最新但是可以玩
	BOOL m_bOldButCanPlay;	

	//主程序
	CAutoUpdateApp	*m_theApp;	

	//详细列表窗口相关
/*
	CDlgShowInfo m_dlgDetail;
	BOOL		 m_blnShowDetail;
*/
	//文件总数
	int			m_nTotalNum;
	bool        m_bMoving;
	int         m_nCurX;
	int         m_nCurY;
	bool        m_bDoUrl;
	CIEComCtrlSink*		m_pMyIESink;
	//Add by Brianyao 2007
	//官网地址:默认是http://www.kingsoft.com/ ，具体设置是本地Settings\AutoUpdate.ini
	CString       m_WebSite;
	string        m_CurrentSelServerName;
	string        m_CurrentSelServerAddr;
	HANDLE        m_hUpdateLock;                //强制更新的锁
//	CUpdateTipDlg m_UpdateTipDlg;
//	CTransTreeCtrl m_TreeCtrl;                  //服务器列表

	enum	enUpdateState
	{
		enUpdate_Updating,
		enUpdate_Failed,
		enUpdate_Finished,
	};

	enUpdateState	m_UpdateState;

	bool		m_HasGetUpdateMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOUPDATEDLG_H__EAC109CF_6E87_44E2_96A2_A5C6EA3EF84C__INCLUDED_)
