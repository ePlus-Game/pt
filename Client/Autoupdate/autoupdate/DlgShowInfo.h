#if !defined(AFX_DLGSHOWINFO_H__D08DFFDE_7B60_476F_8910_964323143C81__INCLUDED_)
#define AFX_DLGSHOWINFO_H__D08DFFDE_7B60_476F_8910_964323143C81__INCLUDED_

#include "TransparentStatic.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgShowInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgShowInfo dialog
#include "Afxmt.h"
#include <map>
#include "ListCtrlEx.h"
#include "BitmapDialog.h"
#include "bmpbutton.h"
#include "WndTool.h"
#include "BitmapListBox.h"

#define WM_CHILDCLOSE	WM_USER + 200
#define WM_ADDNEWFILE	WM_USER + 250

class CDlgShowInfo : public CBitmapDialog
{
// Construction
public:
	CDlgShowInfo(CWnd* pParent = NULL);   // standard constructor

	
// Dialog Data
	//{{AFX_DATA(CDlgShowInfo)
	enum { IDD = IDD_DIALOG_DETAILINFO };
	CTransparentStatic	m_staticTitle;
	CBmpButton	m_buttonMiniClose;
	CListCtrlEx	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgShowInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgShowInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnClickListStatus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonMiniclose();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void InitUI();
	void OnUpdateFileStatus(const char *pszFileName, int nRate);
	virtual	void OnPostEraseBkgnd(CDC* pDC);
private:
	int m_nCurRow;
	enum
	{
		COUNT_CONTROL = 2,	
		COUNT_BMPBUTTON = 1
	};
	//静态控件的窗口位置	
	static WindowRect m_rectStaticCtl[COUNT_CONTROL + 1];
	//位图按钮的个数	
	static BMPButton  m_bmpBtns[COUNT_BMPBUTTON + 1];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSHOWINFO_H__D08DFFDE_7B60_476F_8910_964323143C81__INCLUDED_)
