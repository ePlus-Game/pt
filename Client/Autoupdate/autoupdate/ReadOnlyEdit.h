#if !defined(AFX_READONLYEDIT_H__237EB2C6_98DA_4F2D_BFD3_D46E1F2AFA4B__INCLUDED_)
#define AFX_READONLYEDIT_H__237EB2C6_98DA_4F2D_BFD3_D46E1F2AFA4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReadOnlyEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyEdit window

class CReadOnlyEdit : public CEdit
{
// Construction
public:
	CReadOnlyEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReadOnlyEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CReadOnlyEdit();

	void SetTextColor(COLORREF rgb);
	void SetBackColor(COLORREF rgb);
	// Generated message map functions
protected:
	//{{AFX_MSG(CReadOnlyEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COLORREF m_crText;
	COLORREF m_crBackGnd;
	//background brush
	CBrush m_brBackGnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READONLYEDIT_H__237EB2C6_98DA_4F2D_BFD3_D46E1F2AFA4B__INCLUDED_)
