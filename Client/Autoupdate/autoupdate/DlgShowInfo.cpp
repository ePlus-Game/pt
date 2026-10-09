// DlgShowInfo.cpp : implementation file
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "DlgShowInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgShowInfo dialog

//*********************************************************************
// macro : 计算某个CWnd成员变量在CDlgShowInfo类对象中的偏移
//*********************************************************************
#define OFFSETOF_UPDATEDLG_CWND_MEMBER(Member)		\
OFFSETOF_MEMBER(CDlgShowInfo, CWnd, Member)
//*********************************************************************
// macro : 计算某个CHyperlinkStatic成员变量在CDlgShowInfo类对象中的偏移
//*********************************************************************
#define OFFSETOF_UPDATEDLG_URL_MEMBER(Member)		\
OFFSETOF_MEMBER(CDlgShowInfo, CHyperlinkStatic, Member)

#define OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(Member)	\
OFFSETOF_MEMBER(CDlgShowInfo, CBmpButton, Member)

#define OFFSETOF_UPDATEDLG_URLBMPBTN_MEMBER(Member)		\
OFFSETOF_MEMBER(CDlgShowInfo, CURLBmpButton, Member)

#define OFFSETOF_UPDATEDLG_TRANSPARENTSTATIC_MEMBER(Member) \
OFFSETOF_MEMBER(CDlgShowInfo, CTransparentStatic, Member)


//位图按钮的属性
BMPButton CDlgShowInfo::m_bmpBtns[COUNT_BMPBUTTON + 1] = 
{
	//最小化按钮
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_buttonMiniClose), TRUE, {182, 18},  {16, 16}, 
	RT_BITMAP, enumDRAW_USE_COLORKEY,	IDB_BITMAP_MINCLOSE_UP, IDB_BITMAP_MINCLOSE_DOWN, IDB_BITMAP_MINCLOSE_OVER, IDB_BITMAP_MINCLOSE_OVER,
	RGB(0, 255, 0),

	-1, {0},
};

WindowRect CDlgShowInfo::m_rectStaticCtl[COUNT_CONTROL + 1] =
{
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_staticTitle), TRUE, {58, 600, 5 + 324, 23 + 19},	//标题按钮
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_list), TRUE, {14, 54, 188, 396},	//listctrl
};

CDlgShowInfo::CDlgShowInfo(CWnd* pParent /*=NULL*/)
	: CBitmapDialog(CDlgShowInfo::IDD, pParent)
	, m_nCurRow(-1)
{
	//{{AFX_DATA_INIT(CDlgShowInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgShowInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgShowInfo)
	DDX_Control(pDX, IDC_STATIC_TITLE, m_staticTitle);
	DDX_Control(pDX, IDC_BUTTON_MINICLOSE, m_buttonMiniClose);
	DDX_Control(pDX, IDC_LIST_STATUS, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgShowInfo, CBitmapDialog)
	//{{AFX_MSG_MAP(CDlgShowInfo)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_STATUS, OnClickListStatus)
	ON_BN_CLICKED(IDC_BUTTON_MINICLOSE, OnButtonMiniclose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgShowInfo message handlers

BOOL CDlgShowInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_TRACKSELECT);
	InitUI();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgShowInfo::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	::PostMessage(this->GetParent()->GetSafeHwnd(), WM_CHILDCLOSE, 0, 0);

	this->ShowWindow(SW_HIDE);
}

void CDlgShowInfo::InitUI()
{
	m_list.Init();

	CRect rect;
	m_list.GetWindowRect(&rect);

	int nWidth = rect.Width() - 27;
#ifdef _SPLITHEADER_				//分多个列进行显示
	int colwidths[3] = { 2, 3, 2};	// sixty-fourths

	TCHAR *	lpszHeaders[] = { U_DETAILHEADER_FILENAME,
							  U_DETAILHEADER_PROCESS,
							  U_DETAILHEADER_STATUS,
							  NULL
							};
	
	for (int i=0; i<=3; i++)
	{
		if (!lpszHeaders[i])
			break;

		m_list.InsertColumn(i, lpszHeaders[i], LVCFMT_CENTER, nWidth * colwidths[i] / 8);
	}
#else
#if ( !defined FS_LANG || FS_LANG == 0 )
	m_list.InsertColumn(0, "详细信息", LVCFMT_CENTER, nWidth );
#elif ( FS_LANG == 1 )
	m_list.InsertColumn(0, "冈灿獺", LVCFMT_CENTER, nWidth );
#elif ( FS_LANG == 2 )
	m_list.InsertColumn(0, "详细信息", LVCFMT_CENTER, nWidth );
#endif
#endif

	ModifyStyle(WS_CAPTION, WS_MINIMIZEBOX, SWP_DRAWFRAME);	               
    SetBitmap(IDB_BITMAP_DETAILINFOBACKGROUND);
    SetTransparentColor(RGB(255, 0, 255));
	EnableEasyMove(TRUE); 
	SetLayOutStyle(LO_RESIZE);
	SetTransparent(TRUE);

	m_staticTitle.SetCaptionColor(RGB(255, 255,255));

	CWndTool theWndTool(this);
	theWndTool.ShowWindows(&m_rectStaticCtl[0]);
	theWndTool.ShowBmpButtons(&m_bmpBtns[0]);
	m_list.SetBitmaps(IDB_COLUMNHEADER_START,IDB_COLUMNHEADER_SPAN,IDB_COLUMNHEADER_END);
}

LRESULT CDlgShowInfo::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
	case WM_ADDNEWFILE:
		OnUpdateFileStatus((char*)(wParam), (int)lParam);
		break;
	default:
		return CDialog::WindowProc(message, wParam, lParam);
		break;
	}

	return 0;
}

void CDlgShowInfo::OnUpdateFileStatus(const char *pszFileName, int nRate)
{
	int nCount = m_list.GetItemCount();
	BOOL blnExit = FALSE;
	int i = -1;

	for (i=0; i < nCount; i++)
	{
		CString strFileName = m_list.GetItemText(i, 0);
		if (-1 != strFileName.Find(pszFileName))
		{
			blnExit = TRUE;
			break;
		}
	}

#ifdef _SPLITHEADER_				//分多个列进行显示
	CString strFormat;
	strFormat.Format("%d %%", nRate);

	if (blnExit)
	{		
		m_list.SetItemText(i, 1, strFormat);
		if (nRate == 100)
			m_list.SetItemText(i, 2, U_DETAILHEADER_STATUS_COMPLETE);
	}
	else
	{
		m_list.InsertItem(nCount, pszFileName);
		m_list.SetItemText(nCount, 1, strFormat);
		m_list.SetItemText(nCount, 2, U_DETAILHEADER_STATUS_DOWNING);
	}
#else
	CString strFormat;
	strFormat.Format("%s %d%%", pszFileName, nRate);
	if(nRate!= 0)
		int a = 10;

	if (blnExit)
	{		
		m_list.SetItemText(i, 0, strFormat);
	}
	else
	{
		m_list.InsertItem(nCount, strFormat);
	}
#endif
}

void CDlgShowInfo::OnClickListStatus(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	*pResult = 0;
}

void CDlgShowInfo::OnButtonMiniclose() 
{
	OnClose();
}

void CDlgShowInfo::OnOK() 
{
	// TODO: Add extra validation here
	OnClose();
}

void CDlgShowInfo::OnCancel() 
{
	OnClose();
}

void CDlgShowInfo::OnPostEraseBkgnd(CDC* pDC)
{
}
