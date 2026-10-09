// UpdateTipDlg.cpp : implementation file
//

#include "stdafx.h"
/*
#include "autoupdate.h"
//#include "UpdateTipDlg.h"
#include "ShareUIInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//UpdateTipDlg Setting goes here..............................................
/*
#define UTD_BEGIN_X 10
#define UTD_BEGIN_Y 10
#define UTD_LING_HIGHT 15
#define UTD_WORD_COLOR RGB(128,64,0)
#define UTD_FONT_FACE  "黑体"
*/

/////////////////////////////////////////////////////////////////////////////
// CUpdateTipDlg dialog

//*********************************************************************
// macro : 计算某个CWnd成员变量在CUpdateDialog类对象中的偏移
//*********************************************************************
/*
#define OFFSETOF_UPDATEDLG_CWND_MEMBER(Member)		\
OFFSETOF_MEMBER(CUpdateTipDlg, CWnd, Member)
//*********************************************************************
// macro : 计算某个CHyperlinkStatic成员变量在CUpdateDialog类对象中的偏移
//*********************************************************************
#define OFFSETOF_UPDATEDLG_URL_MEMBER(Member)		\
OFFSETOF_MEMBER(CUpdateTipDlg, CHyperlinkStatic, Member)

#define OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(Member)	\
OFFSETOF_MEMBER(CUpdateTipDlg, CBmpButton, Member)

#define OFFSETOF_UPDATEDLG_URLBMPBTN_MEMBER(Member)		\
OFFSETOF_MEMBER(CUpdateTipDlg, CURLBmpButton, Member)

#define OFFSETOF_UPDATEDLG_TRANSPARENTSTATIC_MEMBER(Member) \
OFFSETOF_MEMBER(CUpdateTipDlg, CTransparentStatic, Member)

//位图按钮的属性
BMPButton CUpdateTipDlg::m_bmpBtns[COUNT_BMPBUTTON + 1] = 
{
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_BtnOK), TRUE, UI_UTD_OK_POS  ,UI_UTD_SIZE, 
	RT_BITMAP, enumDRAW_USE_COLORKEY,	IDB_BUTTON_TIP_UP, IDB_BUTTON_TIP_DOWN, IDB_BUTTON_TIP_OVER, IDB_BUTTON_TIP_OVER,
	UI_UTD_COLOR_KEY,

	-1, {0},
};


CUpdateTipDlg::CUpdateTipDlg(CWnd* pParent /*=NULL*///)
//	: CBitmapDialog(CUpdateTipDlg::IDD, pParent)
//	,m_BackID(IDB_UPDATE_TIP)
/*
{
	//{{AFX_DATA_INIT(CUpdateTipDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CUpdateTipDlg::SetCanPlay(const bool bCanPlay)
{
	if (bCanPlay)
		m_BackID=IDB_UPDATE_TIP;
	else
	    m_BackID=IDB_UPDATE_TIP2;
}

void CUpdateTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateTipDlg)
    DDX_Control(pDX, IDC_TIP_OK, m_BtnOK);
		// NOTE: the ClassWizard will add DDX and DDV calls herezz
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdateTipDlg, CBitmapDialog)
	//{{AFX_MSG_MAP(CUpdateTipDlg)
	ON_BN_CLICKED(IDC_TIP_OK, OnTipOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateTipDlg message handlers

BOOL CUpdateTipDlg::OnInitDialog() 
{
	CBitmapDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUpdateTipDlg::InitUI()
{  
	
	CWndTool tool(this);
	tool.ShowBmpButtons(&m_bmpBtns[0]);

	EnableEasyMove(TRUE);
    SetBitmap(m_BackID);
    SetTransparentColor(RGB(255, 0, 255));
	SetTransparent(TRUE);
    SetLayOutStyle(LO_RESIZE);

}

/*
BOOL CUpdateTipDlg::OnEraseBkgnd(CDC *pDC)
{
	CBitmapDialog::OnEraseBkgnd(pDC);
    
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(UTD_WORD_COLOR);
	
    for (int i=0;i<MAX_TIP_LINE;i++)
	{
		if (!m_Message[i].IsEmpty())
			pDC->TextOut(UTD_BEGIN_X,UTD_BEGIN_Y+i*UTD_LING_HIGHT,m_Message[i]);
	}//endif


	return TRUE;
}
*/

/*
VOID CUpdateTipDlg::ClearAllMessage()
{
	for (int i=0;i<MAX_TIP_LINE;i++)
	{
		m_Message[i]="";
	}//endif
}

BOOL CUpdateTipDlg::SetLineMessage(const int index,const char * szMessage )
{
	if (index<MAX_TIP_LINE)
	{
		m_Message[index]=szMessage;
		return TRUE;
	}
	else 
		return FALSE;
}
*/

//void CUpdateTipDlg::OnTipOk() 
//{
	// TODO: Add your control notification handler code here
//	OnCancel();
//}
