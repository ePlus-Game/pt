#if !defined(AFX_DIRSELECTDLG_H__301D1233_72E2_4E95_91C7_370053AA6409__INCLUDED_)
#define AFX_DIRSELECTDLG_H__301D1233_72E2_4E95_91C7_370053AA6409__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirSelectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirSelectDlg dialog

#include <map>
#include <string>
using namespace std;
#include "BitmapDialog.h"
#include "BtnST.h"
#include "bmpbutton.h"
#include "TransparentStatic.h"
#include "WndTool.h"
#include "BitmapListBox.h"

class CDirSelectDlg : public CBitmapDialog
{
// Construction
public:
	CDirSelectDlg(CWnd* pParent = NULL, int nIndex = 0);   // standard constructor
	~CDirSelectDlg();
// Dialog Data
	//{{AFX_DATA(CDirSelectDlg)
	enum { IDD = IDD_DIRSELECTDLG_DIALOG };
	CTransparentStatic	m_staticShowInfo;
	CBmpButton	m_btnMiniClose;
	CButtonST	m_btnCancel;
	CButtonST	m_btnOK;
	CTransparentStatic	m_DirSelectTitle;	
	CBitmapListBox	m_ctlEntryList;
	CString		m_strDescription;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDirSelectDlg)
	afx_msg void OnSelchangeListDir();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListDir();
	afx_msg void OnButtonMiniclose();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	struct ListEntry
	{
		string strDownName;		// 名字
		string strDownDir;		// 目录
		string strDescription;	// 描述
		string strApplication;	// 程序名字
		ListEntry(LPCSTR pcszName, LPCSTR pcszDir, LPCSTR pcszDescription, LPCSTR pcszApplication) :
			strDownName(pcszName),
			strDownDir(pcszDir),
			strDescription(pcszDescription),
			strApplication(pcszApplication)
		{}
		ListEntry()
		{}
	};
	//　获取当前选项，成功返回TRUE，否则FALSE
	BOOL GetSelection(ListEntry &tagEntry);
	// 增加列表项
	void AddSelection(LPCSTR pcszName, LPCSTR pcszDir, LPCSTR pcszDescription, LPCSTR pcszApplication);
	// 增加列表项
	void AddSelection(const ListEntry &entry);
private:
	void Clear();
	void InitUI();
	virtual void CDirSelectDlg::OnPostEraseBkgnd(CDC* pDC);
	enum
	{
		COUNT_CONTROL = 5,	
		COUNT_BMPBUTTON = 1
	};
	//静态控件的窗口位置	
	static WindowRect m_rectStaticCtl[COUNT_CONTROL + 1];
	//位图按钮的个数	
	static BMPButton  m_bmpBtns[COUNT_BMPBUTTON + 1];
private:
	map<int, ListEntry*> m_mapEntries;	// 可选择的版本列表
	int					 m_nListIndex;	// 选项索引
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRSELECTDLG_H__301D1233_72E2_4E95_91C7_370053AA6409__INCLUDED_)
