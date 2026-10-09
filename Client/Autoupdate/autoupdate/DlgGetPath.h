#if !defined(AFX_DLGGETPATH_H__0D8256E4_05E4_11D3_9FA0_00C0F019557F__INCLUDED_)
#define AFX_DLGGETPATH_H__0D8256E4_05E4_11D3_9FA0_00C0F019557F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgGetPath.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgGetPath dialog
#include "BitmapDialog.h"
#include "BtnST.h"
#include "WndTool.h"
#include "BmpButton.h"
#include "TransparentStatic.h"

class CDlgGetPath : public CBitmapDialog
{
// Construction
public:
	CDlgGetPath(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgGetPath)
	enum { IDD = IDD_GET_PATH };
	CTransparentStatic	m_staticTitle;
	CBmpButton	m_buttonMiniClose;
	CButtonST	m_btnCancel;
	CButtonST	m_btnOk;
	CTreeCtrl	m_Tree;
	//}}AFX_DATA

	//Quick and Nasty one liners.
	CString GetPath() const					{ return m_sPath; }
	void	SetPath( CString sPath )		{ m_sPath = sPath; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgGetPath)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	enum
	{
		COUNT_CONTROL = 3,	
		COUNT_BMPBUTTON = 1,
	};
	// Generated message map functions
	//{{AFX_MSG(CDlgGetPath)
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonMiniclose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//Operators
private:
	void InitUI();
	virtual void OnPostEraseBkgnd(CDC* pDC);
	CString GetItemPath( HTREEITEM hItem );
	bool	PopulateTree( CString sPath, HTREEITEM hParent );
	HTREEITEM InsertItem( HTREEITEM hParent, NETRESOURCE *const pNetResource, CString sText, int iImage, int iImageSelected = -1 );
//	bool	EnumNetwork( HTREEITEM hParent );

//Attributes
private:
	//位图按钮的个数	
	static BMPButton  m_bmpBtns[COUNT_BMPBUTTON + 1];
	//静态控件的窗口位置	
	static WindowRect m_rectStaticCtrl[COUNT_CONTROL + 1];
	CImageList	m_ImageListTree;					//Item in the Tree image list				
//	HTREEITEM	m_hNetworkRoot;						//Network neighbourhood root
	CString		m_sPath;							//Highlighted path
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGETPATH_H__0D8256E4_05E4_11D3_9FA0_00C0F019557F__INCLUDED_)
