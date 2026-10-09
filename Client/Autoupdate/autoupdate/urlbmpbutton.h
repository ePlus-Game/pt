#if !defined(AFX_URLBMPBUTTON_H__5D056C4F_1E1F_4E60_8E9A_5407135ED929__INCLUDED_)
#define AFX_URLBMPBUTTON_H__5D056C4F_1E1F_4E60_8E9A_5407135ED929__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// urlbmpbutton.h : header file
//

#include "bmpbutton.h"
/////////////////////////////////////////////////////////////////////////////
// CURLBmpButton window

class CURLBmpButton : public CBmpButton
{
	DECLARE_DYNAMIC(CURLBmpButton)
// Construction
public:
	CURLBmpButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CURLBmpButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetURL(const CString& strURL);
	virtual ~CURLBmpButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CURLBmpButton)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnClicked();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
private:
	CString m_strURL;
	HCURSOR m_hHandCursor;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_URLBMPBUTTON_H__5D056C4F_1E1F_4E60_8E9A_5407135ED929__INCLUDED_)
