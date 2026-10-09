// DirSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "DirSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//*********************************************************************
// macro : 计算某个CWnd成员变量在CUpdateDialog类对象中的偏移
//*********************************************************************
#define OFFSETOF_UPDATEDLG_CWND_MEMBER(Member)		\
OFFSETOF_MEMBER(CDirSelectDlg, CWnd, Member)
//*********************************************************************
// macro : 计算某个CHyperlinkStatic成员变量在CUpdateDialog类对象中的偏移
//*********************************************************************
#define OFFSETOF_UPDATEDLG_URL_MEMBER(Member)		\
OFFSETOF_MEMBER(CDirSelectDlg, CHyperlinkStatic, Member)

#define OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(Member)	\
OFFSETOF_MEMBER(CDirSelectDlg, CBmpButton, Member)

#define OFFSETOF_UPDATEDLG_URLBMPBTN_MEMBER(Member)		\
OFFSETOF_MEMBER(CDirSelectDlg, CURLBmpButton, Member)

#define OFFSETOF_UPDATEDLG_TRANSPARENTSTATIC_MEMBER(Member) \
OFFSETOF_MEMBER(CDirSelectDlg, CTransparentStatic, Member)


//位图按钮的属性
BMPButton CDirSelectDlg::m_bmpBtns[COUNT_BMPBUTTON + 1] = 
{
	//最小化按钮
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_btnMiniClose), TRUE, {420, 3},  {16, 16}, 
	RT_BITMAP, enumDRAW_USE_COLORKEY,	IDB_BITMAP_MINCLOSE_UP, IDB_BITMAP_MINCLOSE_DOWN, IDB_BITMAP_MINCLOSE_OVER, IDB_BITMAP_MINCLOSE_OVER,
	RGB(0, 255, 0),

	-1, {0},
};

WindowRect CDirSelectDlg::m_rectStaticCtl[COUNT_CONTROL + 1] =
{
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_ctlEntryList), TRUE, {35, 158, 35 + 318, 158 + 58},	//listbox选择框	
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_staticShowInfo), TRUE, {33, 42, 33 + 200, 42 + 11},	//详细信息
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_DirSelectTitle), TRUE, {25, 22, 25 + 20, 22 + 19},		//窗口标题	
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_btnOK), TRUE, {33, 200, 33 + 80, 200 + 30},			//确定按钮	
	OFFSETOF_UPDATEDLG_CWND_MEMBER(m_btnCancel), TRUE, {290, 200, 290 + 80, 200 + 30},		//取消按钮	
};


/////////////////////////////////////////////////////////////////////////////
// CDirSelectDlg dialog

CDirSelectDlg::CDirSelectDlg(CWnd* pParent, int nIndex)
	: CBitmapDialog(CDirSelectDlg::IDD, pParent), m_nListIndex(nIndex)
{
	ASSERT(nIndex >= 0);
	//{{AFX_DATA_INIT(CDirSelectDlg)
	m_strDescription = _TT("");
	//}}AFX_DATA_INIT
}

CDirSelectDlg::~CDirSelectDlg()
{
	Clear();
}

void CDirSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CBitmapDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirSelectDlg)
	DDX_Control(pDX, IDC_STATIC_DESCRIPTION, m_staticShowInfo);
	DDX_Control(pDX, IDC_BUTTON_MINICLOSE, m_btnMiniClose);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_DirSelectTitle);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_LIST_DIR, m_ctlEntryList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDirSelectDlg, CBitmapDialog)
	//{{AFX_MSG_MAP(CDirSelectDlg)
	ON_LBN_SELCHANGE(IDC_LIST_DIR, OnSelchangeListDir)
	ON_LBN_DBLCLK(IDC_LIST_DIR, OnDblclkListDir)
	ON_BN_CLICKED(IDC_BUTTON_MINICLOSE, OnButtonMiniclose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDirSelectDlg::OnSelchangeListDir() 
{
	// 描述信息同步变化
	m_nListIndex = m_ctlEntryList.GetCurSel();
	ASSERT(m_nListIndex >= 0);
	ListEntry *pEntry = m_mapEntries[m_nListIndex];
	ASSERT(pEntry);
	//m_editDescription.SetWindowText(pEntry->strDescription.c_str());
	SetDlgItemText(IDC_STATIC_DESCRIPTION, pEntry->strDescription.c_str());
	m_staticShowInfo.SetBackColor(TRANS_BACK);

	// 因为进度信息控件使用了背景图，所以对话框本身也要刷新
	static RECT rect = {0, 0, 0, 0};
	if (rect.left	== 0 &&
		rect.right  == 0 &&
		rect.top	== 0 &&
		rect.bottom == 0)
	{
		RECT *pRect = CWndTool(this).GetClientRect(
			static_cast<CWnd*>(&m_ctlEntryList), m_rectStaticCtl);
		ASSERT(pRect);
		rect = *pRect;
	}

	InvalidateRect(&rect, TRUE);
	
	//m_btnOK.EnableWindow();
}

//---------------------------------------------------------------------
//  function: 增加列表项
//---------------------------------------------------------------------
void CDirSelectDlg::AddSelection(LPCSTR pcszName, LPCSTR pcszDir, LPCSTR pcszDescription, LPCSTR pcszApplication)
{
	ASSERT(pcszName && pcszDir && pcszDescription && pcszApplication);
	ListEntry *pEntry = new ListEntry(
		pcszName,
		pcszDir,
		pcszDescription,
		pcszApplication);
	ASSERT(pEntry);
	int nIndex = m_mapEntries.size();
	m_mapEntries[nIndex] = pEntry;	
}

//---------------------------------------------------------------------
//  function: 增加列表项
//---------------------------------------------------------------------
void CDirSelectDlg::AddSelection(const ListEntry &entry)
{
	AddSelection(
		entry.strDownName.c_str(),
		entry.strDownDir.c_str(),
		entry.strDescription.c_str(),
		entry.strApplication.c_str());
}

BOOL CDirSelectDlg::OnInitDialog() 
{
	CBitmapDialog::OnInitDialog();
	if (m_mapEntries.size() != 0)
		m_nListIndex = 0;
	// 加载版本选项
	for (int i = 0; i < m_mapEntries.size(); i++)
	{
		ASSERT(m_mapEntries.find(i) != m_mapEntries.end());
		int nIndex = m_ctlEntryList.AddString(m_mapEntries[i]->strDownName.c_str());
		ASSERT(nIndex == i);
	}
	// 设置标题
	CString strCaption;
	strCaption.Format(IDS_SELECT_EDITION);
	SetWindowText(strCaption);
	// 设置默认选项
	if (m_ctlEntryList.SetCurSel(m_nListIndex) != LB_ERR)
	{
		OnSelchangeListDir();
		m_btnOK.EnableWindow(TRUE);
	}
	else
	{
		m_btnOK.EnableWindow(FALSE);
	}
	
	InitUI();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//---------------------------------------------------------------------
//  function: 获取当前的选项数据
//  return	: TRUE-成功；FALSE-失败
//---------------------------------------------------------------------
BOOL CDirSelectDlg::GetSelection(ListEntry &tagEntry)
{
	BOOL bResult = FALSE;
	ASSERT(m_nListIndex >= 0);
	if (m_nListIndex >= 0)
	{
		ASSERT(m_mapEntries.find(m_nListIndex) != m_mapEntries.end());
		ListEntry *pEntry = m_mapEntries[m_nListIndex];
		ASSERT(pEntry);
		tagEntry = *pEntry;
		bResult = TRUE;
	}
	return bResult;
}

//---------------------------------------------------------------------
//  function: 清除内存中的数据结构
//---------------------------------------------------------------------
void CDirSelectDlg::Clear()
{
	map<int, ListEntry*>::const_iterator it = m_mapEntries.begin();
	for (; it != m_mapEntries.end(); it++)
	{
		ListEntry *pEntry = it->second;
		ASSERT(pEntry);
		delete pEntry;
	}
	m_mapEntries.clear();

}

void CDirSelectDlg::OnDblclkListDir() 
{
	// TODO: Add your control notification handler code here
	OnSelchangeListDir();
	EndDialog(IDOK);
}

void CDirSelectDlg::OnButtonMiniclose() 
{
	CBitmapDialog::OnCancel();	
}

void CDirSelectDlg::InitUI()
{
	ModifyStyle(WS_CAPTION, WS_MINIMIZEBOX, SWP_DRAWFRAME);                    
    SetBitmap(IDB_BITMAP_VERSIONSELECTBACKGROUD);
    SetTransparentColor(RGB(255, 0, 255));
	EnableEasyMove(TRUE);
	SetTransparent(TRUE);

	m_DirSelectTitle.SetCaptionColor(RGB(255, 255, 255));
	m_staticShowInfo.SetCaptionColor(RGB(0, 0, 0));

	CWndTool theWndTool(this);
	theWndTool.ShowWindows(&m_rectStaticCtl[0]);
	theWndTool.ShowBmpButtons(&m_bmpBtns[0]);

	m_ctlEntryList.SetTextColor(RGB(0, 0, 0));

	m_btnOK.SetIcon(IDI_ICON_OK, (int)BTNST_AUTO_DARKER);
	m_btnOK.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(255, 255, 255));
	m_btnOK.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
	m_btnOK.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255));
	m_btnOK.DrawTransparent();	

	m_btnCancel.SetIcon(IDI_ICON_CANCEL, (int)BTNST_AUTO_DARKER);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(255, 255, 255));
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255));
	m_btnCancel.DrawTransparent();	
}

void CDirSelectDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CBitmapDialog::OnCancel();
}

void CDirSelectDlg::OnPostEraseBkgnd(CDC* pDC)
{
	m_ctlEntryList.SetBK(pDC);
	m_btnCancel.SetBk(pDC);
	m_btnOK.SetBk(pDC);
}
