#include "stdafx.h"
#include "KExServerDlg.h"
#include "UpdateProcess.h"
#include <windows.h>
#include "ShareUIInfo.h"
#include "KWin32.h"


//*********************************************************************
#define OFFSETOF_UPDATEDLG_CWND_MEMBER(Member)		\
OFFSETOF_MEMBER(CExServerDlg, CWnd, Member)

#define OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(Member)	\
OFFSETOF_MEMBER(CExServerDlg, CBmpButton, Member)

BMPButton CExServerDlg::m_bmpBtns[COUNT_BMPBUTTON + 1] = 
{		
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_contrNormal),   TRUE, EX_SER_NORMAL,  {UI_BUTTON_WIDTH,UI_BUTTON_HIGHT}, 
	RT_BITMAP, enumDRAW_USE_COLORKEY, IDB_BITMAP_NORMAL_UP, IDB_BITMAP_NORMAL_DOWN, IDB_BITMAP_NORMAL_OVER, IDB_BITMAP_NORMAL_DOWN,
	UI_BUTTON_TRANS_COLOR,			
		
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_contrExMode),	TRUE, EX_SER_EXMODE,  {UI_BUTTON_WIDTH,UI_BUTTON_HIGHT}, 
	RT_BITMAP, enumDRAW_USE_COLORKEY, IDB_BITMAP_EXSERVER_UP, IDB_BITMAP_EXSERVER_DOWN, IDB_BITMAP_EXSERVER_OVER, IDB_BITMAP_EXSERVER_DOWN,
	UI_BUTTON_TRANS_COLOR,

	//注意，这里的m_close的资源用的是主界面的close
	OFFSETOF_UPDATEDLG_BMPBTN_MEMBER(m_close),	TRUE, EX_SER_CLOSE,  UI_LITTLE_BUTTON_SIZE, 
	RT_BITMAP, enumDRAW_USE_COLORKEY, IDB_BITMAP_MINCLOSE_UP, IDB_BITMAP_MINCLOSE_DOWN, IDB_BITMAP_MINCLOSE_OVER, IDB_BITMAP_MINCLOSE_DOWN,
	UI_BUTTON_TRANS_COLOR,
	
	-1, {0},
};


CExServerDlg::CExServerDlg(CWnd* pParent /*=NULL*/)
	: CBitmapDialog(CExServerDlg::IDD, pParent)
{

}

CExServerDlg::~CExServerDlg()
{
}

void CExServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NORMAL, m_contrNormal);
	DDX_Control(pDX, IDC_EXMODE, m_contrExMode);
	DDX_Control(pDX, IDC_BUTTON_EXCLOSE, m_close);
}

BEGIN_MESSAGE_MAP(CExServerDlg, CBitmapDialog)
	ON_BN_CLICKED(IDC_NORMAL, OnNormal)
	ON_BN_CLICKED(IDC_EXMODE, OnExMode)	
	ON_BN_CLICKED(IDC_BUTTON_EXCLOSE, OnQuit)
END_MESSAGE_MAP()


void CExServerDlg::OnNormal()
{
	g_bExServerMode = FALSE;
	
	CDialog::OnOK();
}

void CExServerDlg::OnExMode()
{
	g_bExServerMode = TRUE;

	CDialog::OnOK();
}

void CExServerDlg::OnQuit()
{
	CDialog::OnCancel();
}

BOOL CExServerDlg::OnInitDialog()
{

	InitUI();

	return TRUE;
}

void CExServerDlg::InitUI()
{

	CDialog::OnInitDialog();

	UpdateData();

	ModifyStyle(WS_CAPTION, WS_MINIMIZEBOX, SWP_DRAWFRAME);
	EnableEasyMove(TRUE);
    SetBitmap(IDB_BITMAP_EX_BACKGROUNP);
    SetTransparentColor(UI_BK_TANS_COLORKEY);
	SetTransparent(TRUE);
	
	CWndTool theWndTool(this);
	theWndTool.ShowBmpButtons(&m_bmpBtns[0]);

	m_contrExMode.EnableWindow(TRUE);
	m_contrNormal.EnableWindow(TRUE);

}