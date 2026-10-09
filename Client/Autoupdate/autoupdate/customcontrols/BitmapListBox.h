#ifndef _BITMAPLISTBOX_H_
#define _BITMAPLISTBOX_H_
	
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitmapListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBitmapListBox window

class CBitmapListBox : public CListBox
{
// Construction
public:
	CBitmapListBox();

// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetBK(CDC *pDC);
	void SetTextColor(COLORREF rgb = RGB(255, 255, 0));
	virtual ~CBitmapListBox();
	BOOL iSelectChange;
protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CBitmapListBox)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	CBrush m_brHollow;

	DECLARE_MESSAGE_MAP()

private:
	CBitmap		m_bmpBk;
	CBitmap*	m_pbmpOldBk;
	COLORREF	m_rgbText;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // end of _BITMAPLISTBOX_H_
