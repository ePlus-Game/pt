#if !defined(AFX_DLGCUSTOMMSGBOX_H__499AFA4B_0BA4_4D75_925C_E7D0D971E3B3__INCLUDED_)
#define AFX_DLGCUSTOMMSGBOX_H__499AFA4B_0BA4_4D75_925C_E7D0D971E3B3__INCLUDED_

#include "BitmapDialog.h"
#include "BtnST.h"
#include "TransparentStatic.h"
#include "BitmapDialog.h"
#include "HyperlinkStatic.h"
#include "bmpbutton.h"
#include "WndTool.h"

/*
	因为MessageBox对话框需要底图，而且每一个message对话框的大小不是固定的
	为减少底图数量，同时自动更新使用Messagebox的地方较少，所以采用定制的方式
	www.codeproject上面有一个功能十分强大的类，如果将来扩充可以考虑采用那个类
*/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCustomMsgBox.h : header file
//

typedef struct _DlgMsgInfo
{
	const char *szMsg[3];
	bool	bMsg2Url;

} DlgMsgInfo;

/////////////////////////////////////////////////////////////////////////////
// CDlgCustomMsgBox dialog

class CDlgCustomMsgBox : public CBitmapDialog
{
// Construction
public:
	CDlgCustomMsgBox(/*const DlgMsgInfo &msgInfo,*/ CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCustomMsgBox)
	enum { IDD = IDD_DIALOG_CUSTOMDIALOG };
	CBmpButton	m_buttonClose;
//	CTransparentStatic	m_staticInfo1;
	CButtonST	m_buttonOk;
	CButtonST	m_buttonCancel;
//	CHyperlinkStatic	m_staticUrl;
//	CTransparentStatic	m_staticInfo3;
//	CTransparentStatic	m_staticInfo2;
//	CTransparentStatic	m_staticTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCustomMsgBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCustomMsgBox)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void InitUI();
	virtual void OnPostEraseBkgnd(CDC* pDC);
private:
	enum
	{
		COUNT_CONTROL = 2,	
		COUNT_BMPBUTTON = 1,
		COUNT_LINKURL = 0,
	};
	//静态控件的窗口位置	
	static WindowRect m_rectStaticCtl[COUNT_CONTROL + 1];
	//位图按钮的个数	
	static BMPButton  m_bmpBtns[COUNT_BMPBUTTON + 1];
	
//	static UrlLink m_textStaticUrl[COUNT_LINKURL + 1];

//	DlgMsgInfo	m_DlgMsgInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCUSTOMMSGBOX_H__499AFA4B_0BA4_4D75_925C_E7D0D971E3B3__INCLUDED_)
