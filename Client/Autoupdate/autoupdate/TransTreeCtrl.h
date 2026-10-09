#if !defined(AFX_TRANSTREECTRL_H__95FB0196_AC73_4F9C_97D9_3DBF81073DBA__INCLUDED_)
#define AFX_TRANSTREECTRL_H__95FB0196_AC73_4F9C_97D9_3DBF81073DBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransTreeCtrl window
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-04-13 02:43
//      File_base        : TransTreeCtrl.h
//      File_ext         : .h
//      Author           : Brianyao(yaojie)
//      Description      : The Tree List at the left of the AutoupdateDlg
//
//////////////////////////////////////////////////////////////////////

class CTransTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CTransTreeCtrl();

// Attributes
public:
    void  Initialize();    //Do It when the g_ServerList Is Ready!
	//Tips:This function Do the Data Initialization of the TreeView from the Server.ini
	//Downloaded from the HttpServer.
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTransTreeCtrl();
	//Bitmap Settting at the Left
    BOOL SetBkImage(UINT nIDResource);
    BOOL SetBkImage(LPCTSTR lpszResourceName);
	VOID SetRootOffsetX(INT iX);
	VOID SetRootOffsetY(INT iY);
	// Generated message map functions

protected:
	void ExpandOnClick(HTREEITEM hItem);

protected:
	//{{AFX_MSG(CTransTreeCtrl)
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(LPNMHDR pNMHDR, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CPalette m_pal;
    CBitmap m_bitmap;
	int m_cxBitmap, m_cyBitmap;
	CFont m_BkFont;
	int m_RootOffsetX;
    int m_RootOffsetY;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSTREECTRL_H__95FB0196_AC73_4F9C_97D9_3DBF81073DBA__INCLUDED_)
