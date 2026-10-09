#if !defined(AFX_UPDATETIPDLG_H__0976E3F9_0136_435B_B438_D128DF263B43__INCLUDED_)
#define AFX_UPDATETIPDLG_H__0976E3F9_0136_435B_B438_D128DF263B43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UpdateTipDlg.h : header file
//

#include"BitmapDialog.h"

//////////////////////////////////////////////////////////////////////
// CUpdateTipDlg dialog
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-04-13 02:43
//      File_base        : TransTreeCtrl.h
//      File_ext         : .h
//      Author           : Brianyao(yaojie)
//      Description      : 强制更新的提示界面
//
//////////////////////////////////////////////////////////////////////

#define MAX_TIP_LINE 4
#include "WndTool.h"
#include "BmpButton.h"

class CUpdateTipDlg : public CBitmapDialog
{
 // CString        m_Message[MAX_TIP_LINE];  //Max Tip Line Need
	UINT           m_BackID;
    CBmpButton     m_BtnOK;
	enum
	{
		COUNT_BMPBUTTON = 1
	};

	static BMPButton  m_bmpBtns[COUNT_BMPBUTTON + 1];

// Construction
public:
	CUpdateTipDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUpdateTipDlg)
	enum { IDD = IDD_ENTER_TIP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateTipDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
/*
public:
	VOID    ClearAllMessage(void);
    BOOL    SetLineMessage(const int index,const char * szMessage );
*/
public:
    VOID    SetCanPlay(const bool bCanPlay);
protected:

	// Generated message map functions
	//{{AFX_MSG(CUpdateTipDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTipOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void    InitUI();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATETIPDLG_H__0976E3F9_0136_435B_B438_D128DF263B43__INCLUDED_)
