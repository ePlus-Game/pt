// AutoUpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KWin32.h"
#include "AutoUpdate.h"
#include "AutoUpdateDlg.h"
#include "WndTool.h"
#include <io.h>
#include "KFilePath.h"
#include "GameOptionPanel.h"
#include "UpdateProcess.h"
#include "WndCommand.h"
#include "downloadtmp\bufsocket.h"
#include "DownLoadFile.h"
#include <afxinet.h>
#include "IEComCtrlSink.h"
#include "KPakList.h"
#include "KIniFile.h"
//#include "ServerList.h"
//#include "ServerListDlg.h"
//#include "UpdateTipDlg.h"
#include "ShareUIInfo.h"
#include "Encrypter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//显示网页的工作函数
#define DEFAULTLOCALWEB	"defaulut.mht"
#define DEFAULTWEBFILE	"http://www.kingsoft.com/"
#ifdef _DEBUG
	#define WEBFILE	"http://www.kingsoft.com/"
#else
	#define WEBFILE	"http://www.kingsoft.com/"
#endif

#define UPDATA_ADDRESS  "http://fs2.xoyo.com/"
//*********************************************************************
// macro : 计算某个CWnd成员变量在CUpdateDialog类对象中的偏移
//*********************************************************************
#define OFFSETOF_UPDATEDLG_CWND_MEMBER(Member)		\
OFFSETOF_MEMBER(CAutoUpdateDlg, CWnd, Member)
//*********************************************************************
// macro : 计算某个CHyperlinkStatic成员变量在CUpdateDialog类对象中的偏移
//*********************************************************************
#define OFFSETOF_UPDATEDLG_URL_MEMBER(Member)		\
OFFSETOF_MEMBER(CAutoUpdateDlg, CHyperlinkStatic, Member)

#define OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(Member)	\
OFFSETOF_MEMBER(CAutoUpdateDlg, CBmpButton, Member)

#define OFFSETOF_UPDATEDLG_URLBMPBTN_MEMBER(Member)		\
OFFSETOF_MEMBER(CAutoUpdateDlg, CURLBmpButton, Member)

#define OFFSETOF_UPDATEDLG_TRANSPARENTSTATIC_MEMBER(Member) \
OFFSETOF_MEMBER(CAutoUpdateDlg, CTransparentStatic, Member)


/////////////////////////////////////////////////////////////////////////////
// CAutoUpdateDlg dialog

CWndShell *g_pWndShell = NULL;

//Add By BrianYao in 4.5,2007
//左键点击,弹出DEFAULTWEBFILE,响应的Rect,This Might be edited when new BackGround Pictrue is used
#define LBUTTON_UP_NOTIFY_L 552
#define LBUTTON_UP_NOTIFY_R 595
#define LBUTTON_UP_NOTIFY_T 98
#define LBUTTON_UP_NOTIFY_D 136

#define	WM_OPEN_TIP_DIALOG		WM_USER + 110
#define WM_NOTIFY_CUR_SEL       WM_USER + 111

BMPButton CAutoUpdateDlg::m_bmpBtns[COUNT_BMPBUTTON + 1] = 
{		
	// 按钮：取消/进入游戏
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_btnPlay),   TRUE, {UI_BUTTON_START_X, UI_BUTTON_Y},  {UI_BUTTON_WIDTH,UI_BUTTON_HIGHT}, 
	RT_BITMAP, enumDRAW_USE_COLORKEY, IDB_BITMAP_PLAY_UP, IDB_BITMAP_PLAY_DOWN, IDB_BITMAP_PLAY_OVER, IDB_BITMAP_PLAY_DISALBE,
	UI_BUTTON_TRANS_COLOR,			

	// 按钮：进行更新
/*	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_btnUpdate),  TRUE, {UI_BUTTON_START_X+UI_BUTTON_WIDTH+UI_BUTTON_CX,UI_BUTTON_Y},  {UI_BUTTON_WIDTH,UI_BUTTON_HIGHT}, 
	RT_BITMAP, enumDRAW_USE_COLORKEY, IDB_BITMAP_DETAIL_UP, IDB_BITMAP_DETAIL_DOWN, IDB_BITMAP_DETAIL_OVER, IDB_BITMAP_DETAIL_DISABLE,
	UI_BUTTON_TRANS_COLOR,	*/

	// 按钮：游戏设置
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_btnSetting),	TRUE,  {UI_BUTTON_START_X+UI_BUTTON_WIDTH+UI_BUTTON_CX, UI_BUTTON_Y},  {UI_BUTTON_WIDTH,UI_BUTTON_HIGHT}, 	
	RT_BITMAP, enumDRAW_USE_COLORKEY, IDB_BITMAP_SETTING_UP, IDB_BITMAP_SETTING_DOWN, IDB_BITMAP_SETTING_OVER, IDB_BITMAP_SETTING_DISALBE,
	UI_BUTTON_TRANS_COLOR,			

	// 按钮: 退出
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_btnClose),	TRUE, {UI_BUTTON_START_X+2*UI_BUTTON_WIDTH+2*UI_BUTTON_CX, UI_BUTTON_Y},  {UI_BUTTON_WIDTH,UI_BUTTON_HIGHT}, 
	RT_BITMAP, enumDRAW_USE_COLORKEY,	IDB_BITMAP_CLOSE_UP, IDB_BITMAP_CLOSE_DOWN, IDB_BITMAP_CLOSE_OVER, IDB_BITMAP_CLOSE_DISALBE,
	UI_BUTTON_TRANS_COLOR,

	//按钮最小化
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_btnMinimize),	TRUE, UI_LITTLE_POS_MINI_POS,  UI_LITTLE_BUTTON_SIZE, 
	RT_BITMAP, enumDRAW_USE_COLORKEY,	IDB_BITMAP_MINIMIZE_UP, IDB_BITMAP_MINIMIZE_DOWN, IDB_BITMAP_MINIMIZE_OVER, IDB_BITMAP_MINIMIZE_OVER,
	UI_BUTTON_TRANS_COLOR,

	//按钮关闭
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_btnMinClose),	TRUE, UI_LITTLE_POS_CLOSE_POS,  UI_LITTLE_BUTTON_SIZE, 
	RT_BITMAP, enumDRAW_USE_COLORKEY,	IDB_BITMAP_MINCLOSE_UP, IDB_BITMAP_MINCLOSE_DOWN, IDB_BITMAP_MINCLOSE_OVER, IDB_BITMAP_MINCLOSE_OVER,
	UI_BUTTON_TRANS_COLOR,

	-1, {0},
};

WindowRect CAutoUpdateDlg::m_rectStaticCtl[COUNT_CONTROL + 1] =
{
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_updateMannerMsg), FALSE, UI_UPDATEMANNER_TIP_RECT,
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_stcProgressMsg), TRUE, UI_TIP_RECT,	// 文字：进度状态
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_prgOverallRate), TRUE, {CHANEL_SATART_X,CHANLE_OVERAL_Y, CHANEL_SATART_X+CHANEL_LEN, CHANLE_OVERAL_Y+CHANEL_WIDTH},	// 进度条：总体进度
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_prgCurrentRate), TRUE, {CHANEL_SATART_X, CHANLE_FILE_Y, CHANEL_SATART_X+CHANEL_LEN, CHANLE_FILE_Y+CHANEL_WIDTH},	// 进度条：当前进度
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_ShowCaption), TRUE, UI_CAPTION ,	// 窗口的caption
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_HstryLoginServList),TRUE,CURE_SEL_RECT,
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_CurrentServer),TRUE,CURE_SEL_RECT,
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_ctrlBrowser), TRUE, {10, 40, 30, 40},	// 窗口的caption
//OFFSETOF_UPDATEDLG_CWND_MEMBER(m_Picture), TRUE, {55, 215, 55 + 26, 215 + 20},	// 左侧的动画
};

UrlLink CAutoUpdateDlg::m_textStaticUrl[COUNT_LINKURL + 1] = 
{
#ifndef TRADITIONAL_CHINESE
	OFFSETOF_UPDATEDLG_URL_MEMBER(m_stcProgressMsg), UI_TIP_COLOR,  UI_TIP_COLOR, FALSE, 0, URL_NULL,		// 进度状态信息
//	OFFSETOF_UPDATEDLG_URL_MEMBER(m_updateMannerMsg), UI_TIP_COLOR, UI_TIP_COLOR, FALSE, 0, URL_NULL,
#else
	OFFSETOF_UPDATEDLG_URL_MEMBER(m_stcProgressMsg), UI_TIP_COLOR,  UI_TIP_COLOR, FALSE, 1, URL_NULL,		// 进度状态信息	
//	OFFSETOF_UPDATEDLG_URL_MEMBER(m_updateMannerMsg), UI_TIP_COLOR, UI_TIP_COLOR, FALSE, 1, URL_NULL,
#endif
//	OFFSETOF_UPDATEDLG_TRANSPARENTSTATIC_MEMBER(m_ShowCaption), COLOR_BLUE,  COLOR_BLUE, FALSE, 0, URL_NULL,		// 进度状态信息	

};

CAutoUpdateDlg::CAutoUpdateDlg(CWnd* pParent /*=NULL*/)
	: CBitmapDialog(CAutoUpdateDlg::IDD, pParent)
	,m_colorProgress(COLOR_DEEPRED)
	,m_bRefreshing(FALSE)
	,m_pProcess(NULL)
	,m_bOldButCanPlay(FALSE)
	//,m_blnShowDetail(FALSE)
	,m_WebSite(WEBFILE)
	,m_hUpdateLock(NULL)
	,m_CurrentSelServerName("服务器列表不可用")
	,m_UpdateState(enUpdate_Updating)
	,m_HasGetUpdateMode(false)
{
	//{{AFX_DATA_INIT(CAutoUpdateDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_bMoving = false;
	m_nCurX   = 0;
	m_nCurY   = 0;
	g_pWndShell = this;	
	m_bDoUrl = false;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pMyIESink = NULL;
}

CAutoUpdateDlg:: ~CAutoUpdateDlg()
{
	if (m_pProcess)
	{
		m_pProcess->Release();
		m_pProcess = NULL;
	}

	if ( m_pMyIESink )
	{
		delete m_pMyIESink;
		m_pMyIESink = NULL;
	}
	
	g_pWndShell = NULL;
}

void CAutoUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CBitmapDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoUpdateDlg)
//	DDX_Control(pDX, IDC_STATIC_BALL, m_Ball);
//	DDX_Control(pDX, IDC_STATIC_LEFTGIF2, m_Picture);
	DDX_Control(pDX, IDC_STATIC_MAINPROGRESS, m_prgOverallRate);
	DDX_Control(pDX, IDC_STATIC_FILEPROGRESS, m_prgCurrentRate);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnMinClose);
	DDX_Control(pDX, IDC_BUTTON_MINIMIZE, m_btnMinimize);
	DDX_Control(pDX, IDC_STATIC_SHOWCAPTION, m_ShowCaption);
//	DDX_Control(pDX,IDC_STATIC_CURRENT_SEL,m_CurrentServer);
//	DDX_Control(pDX, IDC_COMBO_SERVERLIST, m_HstryLoginServList);
//	DDX_Control(pDX, IDC_BTN_UPDATE, m_btnUpdate);
	DDX_Control(pDX, IDC_BTN_PLAY, m_btnPlay);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BTN_SETTING, m_btnSetting);
	DDX_Control(pDX, IDC_STATIC_PROGRESSMSG, m_stcProgressMsg);
	DDX_Control(pDX, IDC_EXPLORER1, m_ctrlBrowser);
//	DDX_Control(pDX, IDC_SERVER_TREE,m_TreeCtrl);
//	DDX_Control(pDX, IDC_STATIC_UPDATEMANNERTIP, m_updateMannerMsg);
	//}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CBkClrComboBox, CComboBox)
	//{{AFX_MSG_MAP(CBkClrComboBox)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HBRUSH CBkClrComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	static COLORREF	bkColor[] = 
	{
		RGB(229, 213, 189),
		RGB(239, 229, 214),
		RGB(206, 187, 168),
		RGB(155, 128, 105),
		RGB(211, 201, 186),
	};

	pDC->SetBkMode(TRANSPARENT);
 	HBRUSH hbr = ::CreateSolidBrush(bkColor[1]);
 	return hbr;   
}

BEGIN_MESSAGE_MAP(CAutoUpdateDlg, CBitmapDialog)
	//{{AFX_MSG_MAP(CAutoUpdateDlg)
	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDC_BTN_UPDATE, OnBtnUpdate)
	ON_BN_CLICKED(IDC_BTN_PLAY, OnBtnPlay)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_BTN_SETTING, OnBtnSetting)
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
//	ON_CBN_SELCHANGE(IDC_COMBO_SERVERLIST, OnHstryListSelChange)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_BUTTON_MINIMIZE, OnMinimize)	
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CAutoUpdateDlg, CDialog)
//{{AFX_EVENTSINK_MAP(CAutoUpdateDlg)
	ON_EVENT(CAutoUpdateDlg,IDC_EXPLORER1, DISPID_NAVIGATECOMPLETE, OnWebCompletion,VTS_BSTR)
	ON_EVENT(CAutoUpdateDlg, IDC_EXPLORER1, 250 /* BeforeNavigate2 */, OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	
//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()
/////////////////////////////////////////////////////////////////////////////
// CAutoUpdateDlg message handlers

BOOL CAutoUpdateDlg::OnInitDialog()
{
	CBitmapDialog::OnInitDialog();
    
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	ModifyStyle(WS_CAPTION, WS_MINIMIZEBOX, SWP_DRAWFRAME);

	InitUI();
	
	//首先创建详细信息窗体，将其隐藏
/*	m_dlgDetail.Create(IDD_DIALOG_DETAILINFO, this);
	m_dlgDetail.UpdateWindow();	
	m_dlgDetail.ShowWindow(SW_HIDE);
*/
	//把可以玩的Button先Disable Add by Yaojie
	CBufSocket::InitLibrary();

	if (!StartDownload())
		return FALSE;
	
	// TODO: Add extra initialization here
	IWebBrowser2* pWebBrowser2 = NULL;

	HRESULT hr = CoCreateInstance(CLSID_InternetExplorer,
										NULL,
										CLSCTX_INPROC_SERVER, //CLSCTX_LOCAL_SERVER, 
										IID_IWebBrowser2, 
										(void**)&pWebBrowser2);


	DWORD m_dwCookie = 0;
	if (SUCCEEDED(hr))
	{
		// Set up the event sink.
		//
		//		  LPUNKNOWN pUnkSink = GetIDispatch(FALSE);
		AfxConnectionAdvise(pWebBrowser2, 
			DIID_DWebBrowserEvents2,
			(IUnknown*)this,
			FALSE, &m_dwCookie);
		
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
/*
void CAutoUpdateDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	//主要用来显示背景图片
	CWndTool theWndTool(this);
	// 显示位图	
	RECT rect = 
	{
		0,
			0,
			634,
			485,
	};	
	
	CRect rctClient;
	GetClientRect(&rctClient);
	
	CDC memDC;
	CBitmap memBmp;
	memDC.CreateCompatibleDC(&dc);
	memBmp.CreateCompatibleBitmap(&dc, rctClient.Width(), rctClient.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&memBmp);
	
	CBrush bgBrush(GetSysColor(COLOR_MENU));//COLOR_WINDOW));
	CRect rctDraw(0, 0, rctClient.Width(), rctClient.Height());
	memDC.FillRect(rctDraw, &bgBrush);
	
	theWndTool.ShowPicture(&memDC, &m_picBackGround, &rect);
	
	dc.BitBlt(0, 0, rctClient.Width(), rctClient.Height(),
		&memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBmp);
	
}
*/
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutoUpdateDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAutoUpdateDlg::InitUI()
{
	//读取网页配置路径 Add By Brianyao 2007
	BOOL bResult;
 //<------EXVERSION
	if (!g_bExServerMode)
		bResult = g_PakList.Open("\\package.ini");
	else
		bResult = g_PakList.Open("\\packageex.ini");

	KIniFile	IniFile;
	
//	m_CurrentServer.SetWindowText(m_CurrentSelServerName);
  
	LPCSTR autoupdatestr = NULL;

	if (!g_bExServerMode)
		autoupdatestr = "\\settings\\autoupdate.ini";
	else
		autoupdatestr = "\\settings\\autoupdateex.ini";

	if (IniFile.Load(autoupdatestr))
	{
       char URL[MAX_PATH];
	   if(IniFile.GetString("main", "gameurl", "http://www.kingsoft.com/",URL, MAX_PATH))
	   {
	     m_WebSite=URL;
	   }//endif
	}//endif

	g_PakList.Close();
	//显示网页，开辟线程
	m_ctrlBrowser.SetWindowPos(this,0,0,0,0,SWP_SHOWWINDOW);
	m_ctrlBrowser.Navigate(m_WebSite, NULL, NULL, NULL, NULL);

	HANDLE hThread = NULL; 
	DWORD dwThreadId = 0;
	hThread = ::CreateThread( 
		NULL,                       
		0,                          
		ShowWebThreadFunc,          
		this,               
		0,                          
		&dwThreadId);               

	
	if ( hThread )
	{
		::CloseHandle( hThread );
		hThread = NULL;
	}

	EnableEasyMove(TRUE);
    SetBitmap(IDB_BITMAP_BKPICTURE);
    SetTransparentColor(UI_BK_TANS_COLORKEY);
	SetTransparent(TRUE);
	m_ctrlBrowser.ShowScrollBar(SB_BOTH,FALSE);
	m_ctrlBrowser.EnableScrollBar(SB_BOTH,ESB_DISABLE_BOTH);

	m_ctrlBrowser.SetWindowPos(this,BROWSER_X ,BROWSER_Y,BROWSER_CX,BROWSER_CY,SWP_SHOWWINDOW);


	//显示背景图片
//	BOOL bOK = m_picBackGround.Load(theApp.m_hInstance, MAKEINTRESOURCE(IDR_JPEG_BACKGROUND_FS), "JPEG");
	//ASSERT(bOK);
	
	//显示进度条
	// 设置进度条范围、颜色
	//m_prgOverallRate.SendMessage(0x0409, 0, m_colorProgress);
	//m_prgCurrentRate.SendMessage(0x0409, 0, m_colorProgress);
	//m_prgCurrentRate.SetRange(PROGRESS_MINRATE, PROGRESS_MAXRATE);
	//m_prgOverallRate.SetRange(PROGRESS_MINRATE, PROGRESS_MAXRATE);
	
	// 载入位图和图标资源	
	//m_prgOverallRate.SetBitmap(IDB_BITMAP_PROGRESS_BAR, RGB(0, 255, 0), IDB_BITMAP_PROGRESS_BLOCK, RGB(0, 255, 0));
	//m_prgOverallRate.SetClientRect(CRect(2, 1, 274, 18));
	//m_prgCurrentRate.SetBitmap(IDB_BITMAP_PROGRESS_BAR, RGB(0, 255, 0), IDB_BITMAP_PROGRESS_BLOCK, RGB(0, 255, 0));
	//m_prgCurrentRate.SetClientRect(CRect(2, 1, 274, 18));
	
	//Init the chanels
	m_prgOverallRate.SetBitmapChannel(IDB_BITMAP_MAINPROGRESSBACK, IDB_BITMAP_MAINPROGRESSFRONT, TRUE, RGB(255, 0, 255));
	//m_prgOverallRate.SetBitmapThumb(IDB_BITMAP_MAINPROGRESSTHUMB, IDB_BITMAP_MAINPROGRESSTHUMB, TRUE, RGB(255, 0, 255));
	m_prgOverallRate.DrawFocusRect( FALSE );
	m_prgOverallRate.SetRange(0, 100);
	m_prgOverallRate.SetPos(5);
	m_prgOverallRate.SetMargin(-50, 0, 0, 0 );
	m_prgOverallRate.SetPageSize(2);

	m_prgCurrentRate.SetBitmapChannel(IDB_BITMAP_MAINPROGRESSBACK, IDB_BITMAP_MAINPROGRESSFRONT, TRUE,RGB(255,0,255));
//	m_prgCurrentRate.SetBitmapThumb(IDB_BITMAP_MAINPROGRESSTHUMB, IDB_BITMAP_MAINPROGRESSTHUMB, TRUE, RGB(255, 0, 255));
	m_prgCurrentRate.DrawFocusRect( FALSE );
	m_prgCurrentRate.SetRange(0, 100);
	m_prgCurrentRate.SetPos(5);
	m_prgCurrentRate.SetMargin(-50, 0, 0, 0 );
	m_prgCurrentRate.SetPageSize(2);

	m_ShowCaption.SetCaptionColor(UI_FONT_COLOR);
//	m_CurrentServer.SetCaptionColor(CURE_SEL_COLOR );
	//显示按钮  Notice The CWndTool is a Tool class used to display things
	CWndTool theWndTool(this);
	theWndTool.ShowWindows(&m_rectStaticCtl[0]);
    theWndTool.ShowUrlCtls(&m_textStaticUrl[0]);	
	theWndTool.ShowBmpButtons(&m_bmpBtns[0]);

/*	m_Picture.MoveWindow(52, 218, 55 + 26, 215 + 20);
	m_Ball.MoveWindow(535, 478, 55 + 26, 215 + 20);
	if (m_Picture.Load(MAKEINTRESOURCE(IDR_LEFTGIF),_T("GIF")))
		m_Picture.Draw();
	if (m_Ball.Load(MAKEINTRESOURCE(IDR_BALL),_T("GIF")))
		m_Ball.Draw();
	m_Picture.ShowWindow( SW_HIDE );
	m_Ball.ShowWindow( SW_HIDE );*/
    //处理相关按扭的初始状态
	m_btnPlay.EnableWindow(FALSE);
//    m_btnSetting.EnableWindow(FALSE);
	m_btnClose.EnableWindow(FALSE);
	m_btnMinClose.ShowWindow(SW_HIDE);
//	m_btnMinClose.EnableWindow(FALSE);
//	m_btnUpdate.EnableWindow(FALSE);
	m_btnSetting.EnableWindow(TRUE);
//	m_btnMinm .ShowWindow(SW_HIDE);
}

/*void CAutoUpdateDlg::OnBtnUpdate() 
{
	if (m_hUpdateLock)
	{
		SetEvent(m_hUpdateLock);
		m_hUpdateLock=NULL;
	}//endif

//	m_btnUpdate.EnableWindow(FALSE);
	m_btnPlay.EnableWindow(FALSE);
}*/


#include "DlgCustomMsgBox.h"

void CAutoUpdateDlg::OnBtnPlay() 
{
	ASSERT(m_theApp && m_pProcess);

	if(enUpdate_Updating == m_UpdateState || enUpdate_Failed == m_UpdateState)
	{	
// 		DlgMsgInfo	msgInfo;
 
 //		if(enUpdate_Failed == m_UpdateState)
 //		{
 //			msgInfo.szMsg[0] = CANTUPDATE_DLGMSG_1;
// 			msgInfo.szMsg[1] = CANTUPDATE_DLGMSG_2;
// 			msgInfo.szMsg[2] = CANTUPDATE_DLGMSG_3;
// 			msgInfo.bMsg2Url = true;
// 		}
// 		else
// 		{
// 			msgInfo.szMsg[0] = ENTERGAMEONUPDATE_DLGMSG_1;
// 			msgInfo.szMsg[1] = ENTERGAMEONUPDATE_DLGMSG_2;
// 			msgInfo.szMsg[2] = ENTERGAMEONUPDATE_DLGMSG_3;
// 			msgInfo.bMsg2Url = false;
// 		}

		CDlgCustomMsgBox dlg;

		if( IDCANCEL == dlg.DoModal() )
			return;
	}

	if( g_Update_GetLock(0) )
	{
		g_Update_NextTime();
		g_Update_ReleaseLock();
	}
	else
	{
		return;
	}

	//更新逻辑修改：只下载不更新
	if(m_hUpdateLock)
	{
		SetEvent(m_hUpdateLock);
		m_hUpdateLock=NULL;
	}

//	m_btnPlay.EnableWindow(FALSE);
//	m_btnUpdate.EnableWindow(FALSE);
	
	//Do Server List Dlg
/*	CServerListDlg ServerListDlg;
	int res=ServerListDlg.DoModal();
    string  Address=ServerListDlg.GetTheServerResult();

    if (Address=="")
		return;
*/
// 	m_CurrentSelServerAddr = "192.168.0.1";
// 	ASSERT(m_CurrentSelServerAddr!="");
// 	m_theApp->m_strApplication = m_theApp->m_strApplication + " ";
// 	m_theApp->m_strApplication = m_theApp->m_strApplication + m_CurrentSelServerAddr;

//	string addtionalString(g_ServerList.GetCurrentSelect()->GetName());
//     string addtionalString = "8888";
// 	m_theApp->m_strApplication = m_theApp->m_strApplication + ":" + addtionalString;
	
//	g_ServerList.AddServerToHstryList( g_ServerList.GetCurrentSelect() );

	m_theApp->DemandEnterGame();
	
//Add By Brianyao 调用游戏主应用程序 Add by Brianyao
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	::GetStartupInfo(&si);
    ZeroMemory( &pi, sizeof(pi) );
	
	BOOL bRet = TRUE;
	
	char buffer[512];
 //<------EXVERSION
	if (g_bExServerMode)
	{
		g_GetRootPath(buffer);
		strcat(buffer, "\\FSOnline2Ex.exe");
	}
	else
		strcpy(buffer,m_theApp->m_strApplication.c_str());

    if( !CreateProcess( NULL, buffer, NULL,	NULL, FALSE, 0,	NULL, NULL,	&si, &pi)) {
		bRet = FALSE;
    }

//	g_ServerList.SaveCurSelToFile();
	
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
	
	ShowWindow(SW_HIDE);
	NotifyClose(IDOK);
}

void CAutoUpdateDlg::OnBtnClose() 
{
	ASSERT(m_pProcess);
    if (m_hUpdateLock)
	{
		SetEvent(m_hUpdateLock);
		m_hUpdateLock=NULL;
	}

	CString strQuiting;
	strQuiting.Format(IDS_STRING_QUITING);
	NotifyProcessStatus(strQuiting);
	NotifyOverallRate(0);
	NotifyCurrentRate(0);
	//Lucifer~yu[zhangjianyu] [2005-9-29] Add for 先隐藏在线升级对话框，然后再等待线呈运行完毕退出进程，
	//真是个取巧的好办法，这就是经验阿：）
	//begin------------------------------------------------------------------------
	ShowWindow( SW_HIDE );
	//end--------------------------------------------------------------------------
	
	
	// 界面更新过程中不可以取消，否则引起线程死锁
	if (m_pProcess)
		m_pProcess->ToQuit();
	while (IsRefreshing())
	{
		CWndCommand::FlushMessages(&theApp, NULL);
	}

	if (m_pProcess)
		m_pProcess->Cancel();

	NotifyClose(IDCANCEL);

	CBufSocket::CleanupLibrary();
	
	//CDialog::OnCancel();	
}

//// 进程结束
void CAutoUpdateDlg::NotifyClose(int nResult)
{
	this->PostMessage(
		CWndCommand::WM_USERAPPQUIT,
		(WPARAM)&CAutoUpdateDlg::OnNotifyClose,
		(LPARAM)nResult);
}

// 状态消息
void CAutoUpdateDlg::NotifyProcessStatus(LPCSTR pcszMessage)
{
	ASSERT(m_pProcess && pcszMessage);
	void *pParams[2] = {(void*)this, (void*)pcszMessage};
	if (m_pProcess)
		m_pProcess->Log(pcszMessage);
	CMD_HANDLER(this, &CAutoUpdateDlg::OnNotifyProgressStatus, pParams)
}

// 当前进度
void CAutoUpdateDlg::NotifyCurrentRate(int nRate)
{
	ASSERT(nRate >= 0 && nRate <= 100);
	SET_PROGRESS_RATE(this, &m_prgCurrentRate, nRate);
}

// 整体进度
void CAutoUpdateDlg::NotifyOverallRate(int nRate)
{
	ASSERT(nRate >= 0 && nRate <= 100);
	SET_PROGRESS_RATE(this, &m_prgOverallRate, nRate)
}

void CAutoUpdateDlg::NotifyImportant()
{
	m_stcProgressMsg.SetUnderLine(TRUE);
	m_stcProgressMsg.SetDefaultColor(RGB(255,0 , 0));
	m_stcProgressMsg.SetHyperlink(UPDATA_ADDRESS);
}

// 结果通知，TRUE-成功;FALSE-失败

void CAutoUpdateDlg::NotifyResult(BOOL bSuccess, BOOL bCanPlay)
{
	if(bSuccess && bCanPlay)
		m_UpdateState = enUpdate_Finished;
	else if(!bSuccess)
		m_UpdateState = enUpdate_Failed;

	CString strCaption;
	strCaption.Format(IDS_BTN_ENTER);
	SET_WINDOW_TEXT(this, &m_btnPlay, (LPCSTR)strCaption);

//	if("" == m_CurrentSelServerAddr)
//	{
//		HTREEITEM hRoot = m_TreeCtrl.GetRootItem();
	/*	HTREEITEM hLeafItem = hRoot;

		while(true)
		{
			if(NULL == m_TreeCtrl.GetChildItem(hLeafItem))
			{
				m_TreeCtrl.SelectItem(hLeafItem);
				break;
			}
			else
				hLeafItem = m_TreeCtrl.GetChildItem(hLeafItem);
		}*/
//	}

	if (!bSuccess)
	{
		//SHOW_WINDOW(this, &m_btnUpdate, SW_SHOW)
		//m_btnUpdate.EnableWindow(TRUE);
		m_btnPlay.EnableWindow(bCanPlay);
		
//		m_btnSetting.EnableWindow(bCanPlay);
		m_btnClose.EnableWindow(TRUE);
//      m_btnMinClose.EnableWindow(TRUE);
        m_btnMinClose.ShowWindow(SW_SHOW);
		m_bOldButCanPlay = bCanPlay;		
	}
	else
	{
		
		m_btnPlay.EnableWindow(TRUE);

		m_btnClose.EnableWindow(TRUE);
//		m_btnMinClose.EnableWindow(TRUE);
		m_btnMinClose.ShowWindow(SW_SHOW);
//		m_btnSetting.EnableWindow(TRUE);
		//	m_btnUpdate.EnableWindow(FALSE);
	}
}

// 版本号通知
void CAutoUpdateDlg::NotifyVersion(int nMajor, int nMinor)
{
	/*
	ASSERT(nMajor > 0 && nMinor >= 0);
	string strCaption = GetCaption(nMajor, nMinor);
	SET_WINDOW_TEXT(this, this, strCaption.c_str());
	*/
	
	string strCaption = GetCaption(nMajor, nMinor);
	SetWindowText(strCaption.c_str());
	static RECT rect = {0, 0, 0, 0};
	if (rect.left	== 0 &&
		rect.right  == 0 &&
		rect.top	== 0 &&
		rect.bottom == 0)
	{
		RECT *pRect = CWndTool(this).GetClientRect(
			static_cast<CWnd*>(&m_ShowCaption), m_rectStaticCtl);
		ASSERT(pRect);
		rect = *pRect;
	}

	m_ShowCaption.SetWindowText(strCaption.c_str());
	InvalidateRect(&rect, TRUE);
}

int CAutoUpdateDlg::Refresh(int nStatus, long lParam)
{
	ASSERT(m_pProcess);
	if (m_pProcess->IsToQuit())
		return TRUE;

	SetRefreshStatus(TRUE);
	
	LPDOWNLOADFILESTATUS pFileinfo = NULL;

	switch (nStatus)
	{
	case defUPDATE_STATUS_INITIALIZING:
		NotifyProcessStatus(U_CONNECTING_SERVER);
		NotifyOverallRate(20);
		break;
	case defUPDATE_STATUS_VERIFING:
		NotifyProcessStatus(U_VERIFY_ACCOUNT);
		NotifyOverallRate(40);
		break;
	case defUPDATE_RESULT_VERSION_NOT_ENOUGH:
		{
			//版本太旧，需要一次性手动升级包
			CString strTooOldVersion;
			strTooOldVersion.Format(IDS_STRING_TOOOLDVERSION);
			NotifyImportant();
			NotifyProcessStatus(strTooOldVersion);
			NotifyOverallRate(0);
			NotifyCurrentRate(0);
			NotifyResult(FALSE, TRUE);

			m_btnPlay.EnableWindow(TRUE);
//			m_btnUpdate.EnableWindow(FALSE);
			m_btnSetting.EnableWindow(FALSE);
			m_btnClose.EnableWindow(TRUE);
   //         m_btnMinClose.EnableWindow(TRUE);
			m_btnMinClose.ShowWindow(SW_SHOW);
			m_pProcess->ToQuit();
		}
		break;

	//增加非强制更新逻辑 Add Brianyao2007
	case defUPDATE_RESULT_CANPLAY_UPDATE_NEXTIME:
		{
			SetUpdateModeFlag(true);
	    	NotifyProcessStatus(MSG_UPDATE_NOTFORCE);
//			m_hUpdateLock=(HANDLE)lParam;
			/*
			m_UpdateTipDlg.SetLineMessage(0,"亲爱的用户：");
			m_UpdateTipDlg.SetLineMessage(1,"    最新版本为非强制更新模式，您可以选择直接进入游戏，系统会在");
			m_UpdateTipDlg.SetLineMessage(2,"您游戏时下载更新文件，下次启动时自动更新，您也可以选择等待更");
            m_UpdateTipDlg.SetLineMessage(3,"新，在下载完毕后进入游戏。");
			*/

//	    	PostMessage(WM_OPEN_TIP_DIALOG);

			m_btnPlay.EnableWindow(TRUE);
		}
        break;
	case defUPDATE_RESULT_NEED_UPDATE:
		{
			SetUpdateModeFlag(true);
//			m_hUpdateLock=(HANDLE)lParam;
			NotifyProcessStatus(MSG_UPDATE_FORCE);
			SetEvent( (HANDLE)lParam );
//			m_hUpdateLock=(HANDLE)lParam;
//			m_UpdateTipDlg.SetCanPlay(false);
//			PostMessage(WM_OPEN_TIP_DIALOG,1);
		}
		break;
	case defUPDATE_STATUS_PROCESSING_INDEX:
		NotifyProcessStatus(U_LOADING_UPDATE_INFO);
		NotifyOverallRate(60);
		break;
	case defUPDATE_STATUS_DOWNLOADING:
		NotifyProcessStatus(U_DOWNLOADING_FILE);
		NotifyOverallRate(80);
		break;
	case defUPDATE_STATUS_DOWNLOADING_FILE:
		NotifyOverallRate(80);
		pFileinfo = (LPDOWNLOADFILESTATUS)lParam;
		ASSERT(pFileinfo);
		if (pFileinfo)
		{
			CString strFile;
			strFile.Format(U_FILE_DOWNLOADING_STATUS, pFileinfo->strFileName, pFileinfo->dwFileSize / 1024, pFileinfo->dwFileDownloadedSize / 1024);
			int nRate = (int)(pFileinfo->dwFileDownloadedSize * 100.0 / pFileinfo->dwFileSize);
			NotifyProcessStatus((LPCSTR)strFile);
			NotifyCurrentRate(nRate);
			//通知子窗口
		//	::PostMessage(m_dlgDetail.GetSafeHwnd(), WM_ADDNEWFILE, (LPARAM)pFileinfo->strFileName, nRate); 

		}
		break;
	case defUPDATE_STATUS_UPDATING:
		NotifyProcessStatus(U_UPDATE_SYSTEM);
		NotifyOverallRate(100);
		m_btnPlay.EnableWindow(FALSE);
		m_btnClose.EnableWindow(FALSE);
//		m_btnMinClose.EnableWindow(FALSE);
		m_btnMinClose.ShowWindow(SW_HIDE);
//        m_btnUpdate.EnableWindow(FALSE);
		break;
	case defUPDATE_RESULT_UPDATE_SUCCESS:
		NotifyProcessStatus(U_UPDATE_FINISH);
		NotifyOverallRate(100);
		NotifyCurrentRate(100);
		NotifyResult(TRUE, TRUE);
		
//		m_btnSetting.EnableWindow(TRUE);
		m_btnClose.EnableWindow(TRUE);
//		m_btnMinClose.EnableWindow(TRUE);
		m_btnMinClose.ShowWindow(SW_SHOW);
//		m_btnMinimize.ShowWindow(SW_SHOW);
//		m_btnPlay.EnableWindow(TRUE);
//        m_btnUpdate.EnableWindow(FALSE);
		break;
	case defUPDATE_RESULT_VERSION_MORE:
		m_pProcess->ToQuit();
		break;
	case defUPDATE_RESULT_VERSION_LATEST:		//Modified by Fellow, 2003.11.13
		m_pProcess->ToQuit();
		NotifyProcessStatus(U_NEEDLESS_UPDATE);
		NotifyOverallRate(100);
		NotifyCurrentRate(100);
		NotifyResult(TRUE, TRUE);
		break;
	case defUPDATE_RESULT_RUNBEFORE:			//Modified by Fellow, 2003.12.9
		{//运行前程序
			char szRunBefore[MAX_PATH];
			strcpy(szRunBefore, (char *)lParam);
		}
		break;
	case defUPDATE_RESULT_RUNAFTER:				//Modified by Fellow, 2003.12.9
		{//运行后程序
			char szRunAfter[MAX_PATH];
			strcpy(szRunAfter, (char *)lParam);
		}
		break;
	case defUPDATE_RESULT_TOTALFILENUM:
		break;
	default:
		ASSERT(FALSE);
	}

	SetRefreshStatus(FALSE);

	return FALSE;
}

//DEL void CAutoUpdateDlg::OnSize(UINT nType, int cx, int cy) 
//DEL {
//DEL 	//RECT rect = {0, 0, DIALOG_WIDTH, DIALOG_HEIGHT};
//DEL 	//MoveWindow(&rect);
//DEL 	//CenterWindow();
//DEL 
//DEL 	CBitmapDialog::OnSize(nType, cx, cy);	
//DEL }

//显示网页的函数
DWORD WINAPI CAutoUpdateDlg::ShowWebThreadFunc(LPVOID lpParam)
{
	CAutoUpdateDlg *pThis = reinterpret_cast<CAutoUpdateDlg*>(lpParam);
	ASSERT(pThis);
	
	try
	{
		if (pThis)
		{
			pThis->_ShowWebThreadFunc();
		}
	}
	catch (...)
	{
		ASSERT(FALSE);
	}
	
	return 0;
}

DWORD WINAPI CAutoUpdateDlg::ShowMainWebThreadFunc(LPVOID lpParam)
{
	ShellExecute(0, "open", DEFAULTWEBFILE, 0, 0, SW_SHOWNORMAL);
	return 0;
}
	
int CAutoUpdateDlg::_ShowWebThreadFunc()
{
	CInternetSession session;
	CInternetFile* file = NULL;
	m_ctrlBrowser.SetSilent(true);
	try
	{
		// 试着连接到指定URL
		while (m_ctrlBrowser.GetBusy())
		{
			Sleep(1);
		}
		m_ctrlBrowser.SetWindowPos(this,BROWSER_X ,BROWSER_Y,BROWSER_CX,BROWSER_CY,SWP_SHOWWINDOW);
		m_bDoUrl = false;

	}
	catch (CInternetException* m_pException)
	{
		// 如果有错误的话，置文件为空
		m_bDoUrl = true;
		m_ctrlBrowser.MoveWindow(0,0,0,0);
		file = NULL; 
		m_pException->Delete();
		return 0;
	}
	return 0;
}

//从ftp(http上面下载一个文件)
BOOL CAutoUpdateDlg::GetFileFromUrl(const char *pUrl,
					BOOL bText,
					const char *pCachePathName)
{
	
	return FALSE;
}

void CAutoUpdateDlg::OnBtnSetting() 
{
	GameOptionPanel dlg;
	dlg.DoModal();
}

BOOL CAutoUpdateDlg::StartDownload()
{
	//启动下载进程函数
	ASSERT(!m_pProcess);
	m_pProcess = CUpdateProcess::CreateInstance();
	ASSERT(m_pProcess);
	if (!m_pProcess)
	{
		AfxMessageBox(IDS_STRING_FATALERR);
		return FALSE;
	}
	m_pProcess->Attach(this, this);
	// 显示标题
	ShowCaption(m_pProcess);
	// 执行升级
	m_pProcess->Run();
    //确保可以退出
	m_btnClose.EnableWindow(TRUE);
	m_btnMinClose.ShowWindow(SW_SHOW);
	m_btnMinClose.EnableWindow(TRUE);

	return TRUE;
}

void CAutoUpdateDlg::ShowCaption(CUpdateProcess *pProcess)
{
	ASSERT(pProcess);
	int nMajor = 0, nMinor = 0;
	pProcess->GetVersion(&nMajor, &nMinor);
	// 显示版本号
	string strCaption = GetCaption(nMajor, nMinor);
	SetWindowText(strCaption.c_str());
		static RECT rect = {0, 0, 0, 0};
	if (rect.left	== 0 &&
		rect.right  == 0 &&
		rect.top	== 0 &&
		rect.bottom == 0)
	{
 		RECT *pRect = CWndTool(this).GetClientRect(
 			static_cast<CWnd*>(&m_ShowCaption), m_rectStaticCtl);
 		ASSERT(pRect);
 		rect = *pRect;
	}
	m_ShowCaption.SetWindowText(strCaption.c_str());
	InvalidateRect(&rect, TRUE);
}

string CAutoUpdateDlg::GetCaption(int nMajor, int nMinor)
{
	CString strCaption;
	
	if (nMajor >= VERSION_CITYBATTLE)
		strCaption.Format(IDS_CAPTION_VERSION_3, nMajor, nMinor);
	else
		strCaption.Format(IDS_CAPTION_VERSION, nMajor, nMinor);
	
	return string((LPCSTR)strCaption);
}

//是否在界面更新过程中
BOOL CAutoUpdateDlg::IsRefreshing()
{
	m_csRefreshing.Lock();
	BOOL bRefreshing = m_bRefreshing;
	m_csRefreshing.Unlock();
	return bRefreshing;
}

//设置是否在界面更新过程中
void CAutoUpdateDlg::SetRefreshStatus(BOOL bRefreshing)
{
	m_csRefreshing.Lock();
	m_bRefreshing = bRefreshing;
	m_csRefreshing.Unlock();
}

void CAutoUpdateDlg::OnDestroy() 
{
	CBitmapDialog::OnDestroy();
	
	ASSERT(m_hIcon && m_theApp);	

	// 销毁图标句柄
	if (m_hIcon)
	{
		::DeleteObject(m_hIcon);
		m_hIcon = NULL;
	}	
}

void CAutoUpdateDlg::SetMainApp(CAutoUpdateApp *theApp)
{
	ASSERT(theApp);
	m_theApp = theApp;
}

// *********************************************************************
// function		: 实际的关闭消息处理函数
// *********************************************************************
void CAutoUpdateDlg::OnNotifyClose(WPARAM wParam, LPARAM lParam)
{	
	CAutoUpdateDlg *pDlg = (CAutoUpdateDlg*)wParam;
	ASSERT(pDlg && pDlg->m_pProcess);
	// 等待子线程结束，为了防止线程死锁，必须保持消息循环
	if (pDlg->m_pProcess)
	{
		while (pDlg->m_pProcess->IsActive())
		{
			CWndCommand::FlushMessages(&theApp, NULL);
			::Sleep(THREAD_WAITTIME);
		}
	}
	pDlg->EndDialog((int)lParam);	
}


//*********************************************************************
// function		: 实际的更新进度状态消息函数
//*********************************************************************
void CAutoUpdateDlg::OnNotifyProgressStatus(void *pParam)
{
	void **ppData = (void**)pParam;
	CAutoUpdateDlg *pDlg = (CAutoUpdateDlg*)ppData[0];
	LPCSTR pcszMessage = (LPCSTR)ppData[1];
	ASSERT(pDlg && pcszMessage);

	CHyperlinkStatic *pLink;
/*	if( pDlg->HasGetUpdateMode() )
		pLink = &pDlg->m_updateMannerMsg;
	else*/
		pLink = &pDlg->m_stcProgressMsg;
	ASSERT(pLink);
	pDlg->SetUpdateModeFlag(false);

	// 因为进度信息控件使用了背景图，所以对话框本身也要刷新
	static RECT rect = {0, 0, 0, 0};
	if (rect.left	== 0 &&
		rect.right  == 0 &&
		rect.top	== 0 &&
		rect.bottom == 0)
	{
		RECT *pRect = CWndTool(pDlg).GetClientRect(
			static_cast<CWnd*>(pLink), m_rectStaticCtl);
		ASSERT(pRect);
		rect = *pRect;
	}
	pLink->SetCaption(pcszMessage);
	pDlg->InvalidateRect(&rect, TRUE);
}

//服务器列表下载完毕的回调函数 AddBy Brianyao
/*void CAutoUpdateDlg::NotifyServerList(void)
{
	char szFile[MAX_PATH];
	strncpy(szFile, "UserData\\server.ini", sizeof(szFile));
	
	// 如果因为种种原因，到这一步的时候，server.ini是加密后的
	// 就会导致读不出来，无法显示server list，从而无法进入游戏
	// 这里提前判断一下
	if( !CanServListReadDirectly(szFile) )
	{
		IEncrypter *pTmp = GetXOREncrypter();
		pTmp->Unencrypt(szFile, szFile);
		delete pTmp;
	}

    BOOL res=g_ServerList.ReadListFile(szFile);
    ASSERT(res);
//	m_TreeCtrl.Initialize();
//	RefreshHstryServList();

	//避免服务器IP暴露
	IEncrypter * pEncrypter=GetXOREncrypter();
    BOOL bRes=pEncrypter->Encrypt(szFile, szFile);
	ASSERT(bRes);
	delete pEncrypter;

	m_btnClose.EnableWindow(TRUE);
	m_btnMinClose.EnableWindow(TRUE);
	m_btnMinClose.ShowWindow(SW_SHOW);
}
*/
/*void CAutoUpdateDlg::RefreshHstryServList()
{
	m_HstryLoginServList.ResetContent();

	const HistoryServerList &servList = g_ServerList.GetHistoryServList();

	for(int nServ = 0; nServ < servList.ServerCount; ++nServ)
	{
		m_HstryLoginServList.InsertString(nServ, servList.Server[nServ].ServerName);
	}

	m_HstryLoginServList.SetCurSel(0);
	OnHstryListSelChange();
}*/

LRESULT CAutoUpdateDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// 执行自定义命令
	LRESULT lResult = 0;
	switch (message)
	{
	// 自定义命令消息
	case CWndCommand::WM_USERCOMMAND:
		CWndCommand::OnMessage(wParam, lParam);
		break;
	// 自定义关闭消息
	case CWndCommand::WM_USERAPPQUIT:
		((fnOnApplicationQuit)wParam)((WPARAM)this, lParam);
		break;
	case WM_CHILDCLOSE:
		{
			OnChildClose();	
		}
		break;

/*	case WM_OPEN_TIP_DIALOG:
		{
			if (wParam==1)
				m_UpdateTipDlg.SetCanPlay(false);
			else
				m_UpdateTipDlg.SetCanPlay(true);
			
			//更改，只有强制更新才出对话框
			if(wParam)
			{
				m_UpdateTipDlg.DoModal();
			}//endif

			if(wParam==1) //强制更新
			{
				m_btnPlay.EnableWindow(FALSE);
			//	m_btnUpdate.EnableWindow(TRUE);
				if (m_hUpdateLock)
				{
					SetEvent(m_hUpdateLock);
					m_hUpdateLock=NULL;
				}//endif

			}//endif
            else
			{
				if (m_CurrentSelServerAddr!="")
					m_btnPlay.EnableWindow(TRUE);
//				m_btnUpdate.EnableWindow(TRUE);
			}//end else
			
//			m_btnSetting.EnableWindow(TRUE);
			m_btnClose.EnableWindow(TRUE);
//			m_btnMinClose.EnableWindow(TRUE);
			m_btnMinClose.ShowWindow(SW_SHOW);
		}
		break;
*/
 /*   case WM_NOTIFY_CUR_SEL://当前选择的服务器已经改变
		{
		  const CServerInfo * pInfo=g_ServerList.GetCurrentSelect();
		  if (pInfo)
		  {
            m_CurrentSelServerName=pInfo->GetParentRegion()->GetParentNet()->m_NetName+pInfo->GetParentRegion()->GetRegionName()+pInfo->GetName();
			m_CurrentSelServerAddr=pInfo->GetIP();
		
// 			static RECT rect = {0, 0, 0, 0};
// 			if (rect.left	== 0 &&
// 				rect.right  == 0 &&
// 				rect.top	== 0 &&
// 				rect.bottom == 0)
// 			{
//  				RECT *pRect = CWndTool(this).GetClientRect(
//  					static_cast<CWnd*>(&m_CurrentServer), m_rectStaticCtl);
//  				ASSERT(pRect);
//  				rect = *pRect;
// 			}
// 
// 			m_CurrentServer.SetWindowText(m_CurrentSelServerName);
//      	    InvalidateRect(&rect, TRUE);
		
		  }//endif
		}break;
*/
/*	case WM_NOTIFY_REFRESH_HSTRYLIST:
		{
//			RefreshHstryServList();
		}
		break;
*/
	// 系统消息
	default:
		lResult = CBitmapDialog::WindowProc(message, wParam, lParam);
	}

	if(message == m_theApp->m_RegisterMsg)
		ShowWindow(SW_SHOW);

	return lResult;
}

void CAutoUpdateDlg::OnMinimize()
{
	ShowWindow(SW_MINIMIZE);
}

void CAutoUpdateDlg::OnPostEraseBkgnd(CDC* pDC)
{
}

BOOL CAutoUpdateDlg::FindResource(const char *pszDefalutHtml)
{
	ASSERT(pszDefalutHtml);
	if (!pszDefalutHtml)
		return FALSE;

	if (_access(pszDefalutHtml, 00) != -1)
		return TRUE;

	HRSRC  hrsrc=::FindResource(NULL ,MAKEINTRESOURCE(IDR_MHT_DEFAULT),"MHT");

	HGLOBAL  hGlobal = NULL;
	HRSRC  hSource = NULL;
	LPVOID  lpVoid  = NULL;
	int   nSize   = 0;
	BOOL  bResult=FALSE;
	
	hSource = ::FindResource(NULL ,MAKEINTRESOURCE(IDR_MHT_DEFAULT),"MHT");;
	
	if(hSource == NULL)
	{
		return FALSE;
	}
	hGlobal = LoadResource(NULL, hSource);
	if(hGlobal == NULL)
	{ 
		return FALSE;
	}
	lpVoid = LockResource(hGlobal);
	if(lpVoid == NULL)
	{
		return FALSE;
	}
	
	nSize = (UINT)SizeofResource(NULL, hSource);

	BOOL blnRet =MakeDefaultMht((BYTE*)hGlobal, nSize, pszDefalutHtml);
		
	UnlockResource(hGlobal); // 16Bit Windows Needs This
	FreeResource(hGlobal); // 16Bit Windows Needs This (32Bit - Automatic Release)

	return blnRet;
}

BOOL CAutoUpdateDlg::MakeDefaultMht(BYTE *pBuffer, int nSize, const char *pszTempFileName)
{
	ASSERT(pszTempFileName);
	if (!pszTempFileName)
		return FALSE;

	BOOL bResult = FALSE;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	
	if(hGlobal == NULL)
	{
		return FALSE;
	}
	
	void* pData = GlobalLock(hGlobal);
	memcpy(pData, pBuffer, nSize);

	CFile file(pszTempFileName, CFile::modeCreate | CFile::modeWrite);
	file.Write(pData, nSize);
	file.Close();

	GlobalUnlock(hGlobal);
	FreeResource(hGlobal); 

	return TRUE;
}

void CAutoUpdateDlg::AdjustWindowPos()
{
//	if (!m_blnShowDetail)
//		return;

	CRect rect;
	this->GetWindowRect(&rect);
	
	CRect myrect;
//	m_dlgDetail.GetWindowRect(&myrect);
//	m_dlgDetail.MoveWindow(rect.right - 52, rect.top + 144, myrect.Width(), myrect.Height());
}

void CAutoUpdateDlg::OnMove(int x, int y) 
{
	CBitmapDialog::OnMove(x, y);
	
	AdjustWindowPos();
	
}


void CAutoUpdateDlg::OnChildClose()
{
//	m_blnShowDetail = FALSE;
}

void CAutoUpdateDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if ( m_bMoving )
	{
	CRect rcW;
	POINT point;
	GetWindowRect(rcW);

	//实现拖动时窗体跟着移动
    ::GetCursorPos(&point);			
//	MoveWindow(point.x - m_nCurX ,point.y - m_nCurY ,rcW.Width(),rcW.Height() ,true); 
	}
	CBitmapDialog::OnMouseMove(nFlags, point);
}

UINT CAutoUpdateDlg::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CDialog::OnNcHitTest(point);
}

void CAutoUpdateDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if ( (point.x > LBUTTON_UP_NOTIFY_L && point.x < LBUTTON_UP_NOTIFY_R) &&
     	 (point.y > LBUTTON_UP_NOTIFY_T && point.y < LBUTTON_UP_NOTIFY_D) )
	{
	//显示网页，开辟线程
		HANDLE hThread = NULL; 
		DWORD dwThreadId = 0;
		hThread = ::CreateThread( NULL,                       
								  0,                          
								  ShowMainWebThreadFunc,          
								  this,               
								  0,                          
								  &dwThreadId );               
	
		if ( hThread )
		{
			::CloseHandle( hThread );
			hThread = NULL;
		}	
	}
	if ( m_bMoving )
	{
		CRect rcW;
		POINT point;
		GetWindowRect(rcW);

    	//实现拖动时窗体跟着移动
        ::GetCursorPos(&point);			
		MoveWindow(point.x - m_nCurX ,point.y - m_nCurY ,rcW.Width(),rcW.Height() ,true);
		ReleaseCapture();
		m_bMoving = false;
	}
	if ( m_bDoUrl )
	{
	//显示网页，开辟线程
		HANDLE hThread = NULL; 
		DWORD dwThreadId = 0;
		hThread = ::CreateThread( NULL,                       
								  0,                          
								  ShowMainWebThreadFunc,          
								  this,               
								  0,                          
								  &dwThreadId );               
	
		if ( hThread )
		{
			::CloseHandle( hThread );
			hThread = NULL;
		}		
	}
//	CBitmapDialog::OnLButtonUp(nFlags, point);
}

void CAutoUpdateDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if ( !m_bMoving )
	{
		m_bMoving = true;
		SetCapture();
		m_nCurX = point.x;
		m_nCurY = point.y;
	}
//	CBitmapDialog::OnLButtonDown(nFlags, point);
}

void CAutoUpdateDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CDC *pDC = GetDC();
	CDC mdc;
	CBitmap bmBitmap1,bmBitmap2;
//	bmBitmap1.LoadBitmap();
//	bmBitmap2.LoadBitmap();
	mdc.CreateCompatibleDC(pDC);
	CBitmap *pOldBitmap = (CBitmap *)mdc.SelectObject (bmBitmap1);	
	CBitmapDialog::OnTimer(nIDEvent);
}

BOOL CAutoUpdateDlg::OnWebCompletion(const char * sbUrl)
{
	m_ctrlBrowser.SetWindowPos(this, BROWSER_X ,BROWSER_Y,BROWSER_CX,BROWSER_CY,SWP_SHOWWINDOW);
	return TRUE;
}

void CAutoUpdateDlg::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) 
{
	std::wstring strURL((unsigned short*)URL->pcVal);
	int nBegin	= strURL.find_first_of( L"^" );
	int nEnd	= strURL.find_last_of( L"^" );	
	if ( nEnd != -1 && nBegin != -1 )
	{
		strSelServerInfo = strURL.substr( nBegin + 1, nEnd - nBegin - 1 );
		*Cancel = TRUE;
	}
	
	return ;
}

BOOL CAutoUpdateDlg::PreTranslateMessage(MSG *pMsg)
{
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)	)
	{
		return TRUE;
	}//endif
	else
	{
		return CBitmapDialog::PreTranslateMessage(pMsg);
	}//end else
}

/*void CAutoUpdateDlg::OnHstryListSelChange()
{
	int nCurSel = m_HstryLoginServList.GetCurSel();

 	if(CB_ERR == nCurSel)
 		return;

	const HistoryServerList &hstryServList = g_ServerList.GetHistoryServList();

	HistoryServer hstryServer = hstryServList.Server[nCurSel];

	HTREEITEM hRoot = m_TreeCtrl.GetRootItem();
	SelectTargetItem(hRoot, hstryServer.NetName, hstryServer.RegionName, hstryServer.ServerName);

	HTREEITEM hSiblingItem = m_TreeCtrl.GetNextSiblingItem(hRoot);
	while(hSiblingItem)
	{
		SelectTargetItem(hSiblingItem, hstryServer.NetName, hstryServer.RegionName, hstryServer.ServerName);
		hSiblingItem = m_TreeCtrl.GetNextSiblingItem(hSiblingItem);
	}
}*/

/*void CAutoUpdateDlg::SelectTargetItem(HTREEITEM hItem, 
	const char *szNetName, 
	const char *szRegionName, 
	const char *szServerName
	)
{
	if(NULL == hItem)
		return;

	HTREEITEM hChildItem = m_TreeCtrl.GetChildItem(hItem);

	while(hChildItem)
	{
		SelectTargetItem(hChildItem, szNetName, szRegionName, szServerName);
		hChildItem = m_TreeCtrl.GetNextSiblingItem(hChildItem);
	}

	static TVITEM TempItem;
	TempItem.hItem = hItem;
	BOOL bSuc = m_TreeCtrl.GetItem(&TempItem);

	if (bSuc && TempItem.lParam!=0)
	{
		CServerInfo * pServer=(CServerInfo * )TempItem.lParam;

		if( 0 == strcmp(pServer->GetName(), szServerName) && 
			0 == strcmp(pServer->GetParentRegion()->GetRegionName(), szRegionName) &&
			0 == strcmp(pServer->GetParentRegion()->GetParentNet()->m_NetName, szNetName)
		  )
		  m_TreeCtrl.SelectItem(hItem);
	}
}*/

/*BOOL CAutoUpdateDlg::CanServListReadDirectly(const char *szFile)
{
	KIniFile File;
	BOOL bRet = File.Load(szFile);

    if(!bRet)
		return TRUE;

	int nNetListNum;
  
	if ( !File.GetInteger("List", "NetCount", 0, &nNetListNum) || nNetListNum == 0 )
 		return FALSE;
	else
		return TRUE;
}*/











