// DlgCustomMsgBox.cpp : implementation file
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "DlgCustomMsgBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCustomMsgBox dialog
//*********************************************************************
// macro : 计算某个CWnd成员变量在CUpdateDialog类对象中的偏移
//*********************************************************************
#define OFFSETOF_UPDATEDLG_CWND_MEMBER(Member)		\
OFFSETOF_MEMBER(CDlgCustomMsgBox, CWnd, Member)
//*********************************************************************
// macro : 计算某个CHyperlinkStatic成员变量在CUpdateDialog类对象中的偏移
//*********************************************************************
#define OFFSETOF_UPDATEDLG_URL_MEMBER(Member)		\
OFFSETOF_MEMBER(CDlgCustomMsgBox, CHyperlinkStatic, Member)

#define OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(Member)	\
OFFSETOF_MEMBER(CDlgCustomMsgBox, CBmpButton, Member)

#define OFFSETOF_UPDATEDLG_URLBMPBTN_MEMBER(Member)		\
OFFSETOF_MEMBER(CDlgCustomMsgBox, CURLBmpButton, Member)

#define OFFSETOF_UPDATEDLG_TRANSPARENTSTATIC_MEMBER(Member) \
OFFSETOF_MEMBER(CDlgCustomMsgBox, CTransparentStatic, Member)


//位图按钮的属性
BMPButton CDlgCustomMsgBox::m_bmpBtns[COUNT_BMPBUTTON + 1] = 
{
	//最小化按钮
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_buttonClose), TRUE, {482, 2},  {16, 16}, 
	RT_BITMAP, enumDRAW_USE_COLORKEY,	IDB_BITMAP_MINCLOSE_UP, IDB_BITMAP_MINCLOSE_DOWN, IDB_BITMAP_MINCLOSE_OVER, IDB_BITMAP_MINCLOSE_OVER,
	RGB(0, 255, 0),

	-1, {0},
};

WindowRect CDlgCustomMsgBox::m_rectStaticCtl[COUNT_CONTROL + 1] =
{
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_staticTitle), TRUE, {22, 36, 12+ 324, 25 + 19},	//窗口标题

	//提示信息相关
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_staticInfo1), TRUE, {55, 60, 80 + 324, 60 + 19},
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_staticInfo2), TRUE, {30, 80, 30 + 120, 80 + 19},	
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_staticUrl), TRUE, {30, 80, 30 + 120, 80 + 20},	
	//Lucifer~yu 2005-7-19 21:46 modify {202, 80, 202 + 324, 80 + 67} to {190, 80, 202 + 324, 80 + 67}
//	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_staticInfo3), TRUE, {150, 80, 202 + 324, 80 + 67},	
	
	//选择按钮相关
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_buttonOk), TRUE, {80, 156, 80 + 80, 156 + 33},	
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_buttonCancel), TRUE, {200, 156, 200 + 80, 156 + 33},	
};

//UrlLink CDlgCustomMsgBox::m_textStaticUrl[COUNT_LINKURL + 1] = 
//{
//	OFFSETOF_UPDATEDLG_URL_MEMBER(m_staticUrl), COLOR_BLUE,  COLOR_BLUE, TRUE, 0, CANTUPDATE_DLGMSG_2,	//超连接
//};


CDlgCustomMsgBox::CDlgCustomMsgBox(/*const DlgMsgInfo &msgInfo, */CWnd* pParent /*=NULL*/)
	: CBitmapDialog(CDlgCustomMsgBox::IDD, pParent)//, m_DlgMsgInfo(msgInfo)
{
	//{{AFX_DATA_INIT(CDlgCustomMsgBox)
	//}}AFX_DATA_INIT
}


void CDlgCustomMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCustomMsgBox)
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_buttonClose);
//	DDX_Control(pDX, IDC_STATIC_INFO1, m_staticInfo1);
	DDX_Control(pDX, IDOK, m_buttonOk);
	DDX_Control(pDX, IDCANCEL, m_buttonCancel);
//	DDX_Control(pDX, IDC_STATIC_URL, m_staticUrl);
//	DDX_Control(pDX, IDC_STATIC_INFO3, m_staticInfo3);
//	DDX_Control(pDX, IDC_STATIC_INFO2, m_staticInfo2);
//	DDX_Control(pDX, IDC_STATIC_TITLE, m_staticTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCustomMsgBox, CBitmapDialog)
	//{{AFX_MSG_MAP(CDlgCustomMsgBox)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCustomMsgBox message handlers

#define SETBUTTONCOLOR(btn, value)\
	btn.SetColor(CButtonST::BTNST_COLOR_FG_OUT, value);\
	btn.SetColor(CButtonST::BTNST_COLOR_FG_IN, value);\
	btn.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, value);

void CDlgCustomMsgBox::InitUI()
{
	ModifyStyle(WS_CAPTION, WS_MINIMIZEBOX, SWP_DRAWFRAME);
	EnableEasyMove(TRUE);                    
    SetBitmap(IDB_BITMAP_CUSTOMMSGBACKGROUPD);
    SetTransparentColor(RGB(255, 0, 255)); 
	SetTransparent(TRUE);

	m_buttonOk.SetIcon(IDI_ICON_OK, (int)BTNST_AUTO_DARKER);
	m_buttonOk.DrawTransparent();	

	m_buttonCancel.SetIcon(IDI_ICON_CANCEL, (int)BTNST_AUTO_DARKER);
	m_buttonCancel.DrawTransparent();

// 	m_staticInfo1.SetWindowText(m_DlgMsgInfo.szMsg[0]);
// 	m_staticInfo3.SetWindowText(m_DlgMsgInfo.szMsg[2]);
// 	
// 	if(m_DlgMsgInfo.bMsg2Url)
// 	{
// 		m_staticUrl.SetWindowText(m_DlgMsgInfo.szMsg[1]);
// 		m_textStaticUrl[0].pcszCtlUrl = (LPCSTR)m_DlgMsgInfo.szMsg[1];
// 		m_staticInfo2.ShowWindow(SW_HIDE);
// 	}
// 	else
// 	{
// 		m_staticInfo2.SetWindowText(m_DlgMsgInfo.szMsg[1]);
// 		m_staticUrl.ShowWindow(SW_HIDE);
// 	}
	
	CString strUrl;
//	m_staticUrl.GetWindowText(strUrl);
//	m_staticUrl.SetHyperlink(strUrl);

//	m_staticTitle.SetCaptionColor(RGB(255, 0, 0));

	CWndTool theWndTool(this);
	theWndTool.ShowWindows(&m_rectStaticCtl[0]);
	theWndTool.ShowBmpButtons(&m_bmpBtns[0]);
//	theWndTool.ShowUrlCtls(&m_textStaticUrl[0]);

	SETBUTTONCOLOR(m_buttonOk, RGB(239, 199, 140));
	SETBUTTONCOLOR(m_buttonCancel, RGB(239, 199, 140));

//	m_staticInfo1.SetCaptionColor(RGB(0, 0, 0));
//	m_staticInfo3.SetCaptionColor(RGB(0, 0, 0));;
//	m_staticInfo2.SetCaptionColor(RGB(0, 0, 0));;
}

BOOL CDlgCustomMsgBox::OnInitDialog() 
{
	CBitmapDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitUI();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgCustomMsgBox::OnPostEraseBkgnd(CDC* pDC)
{
	m_buttonOk.SetBk(pDC);
	m_buttonCancel.SetBk(pDC);
}

void CDlgCustomMsgBox::OnButtonClose() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}
