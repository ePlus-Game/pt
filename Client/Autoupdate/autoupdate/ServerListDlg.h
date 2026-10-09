#if !defined(AFX_SERVERLISTDLG_H__0B9EE15F_4CB2_4C42_B6E0_860597FF5F6D__INCLUDED_)
#define AFX_SERVERLISTDLG_H__0B9EE15F_4CB2_4C42_B6E0_860597FF5F6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CServerListDlg dialog

class CServerListDlg : public CDialog
{
// Construction
public:
	CServerListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CServerListDlg)
	enum { IDD = IDD_SEVERLIST };
	CListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
    CString  GetTheServerResult(void)const;
protected:

	// Generated message map functions
	//{{AFX_MSG(CServerListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList();
	afx_msg void OnListOk();
	afx_msg void OnListCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_ServerAddr;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERLISTDLG_H__0B9EE15F_4CB2_4C42_B6E0_860597FF5F6D__INCLUDED_)
