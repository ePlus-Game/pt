#if !defined(AFX_BMPBUTTON_H__BB4EE3C8_BBB9_4E51_9D53_B6A443AE7FF0__INCLUDED_)
#define AFX_BMPBUTTON_H__BB4EE3C8_BBB9_4E51_9D53_B6A443AE7FF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// bmpbutton.h : header file
//
#include "Dib.h"

/////////////////////////////////////////////////////////////////////////////
// CBmpButton window
enum
{
	enumDRAW_USE_NORMAL = 0,
	enumDRAW_USE_COLORKEY,
	enumDRAW_USE_ALPHA,
};
class CBmpButton : public CButton
{
	DECLARE_DYNAMIC(CBmpButton)
// Construction
public:
	CBmpButton();
	virtual ~CBmpButton();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpButton)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL LoadBitmaps(UINT nIDBitmapResource, 
		UINT nIDBitmapResourceSel = 0, 
		UINT nIDBitmapResourceOver = 0, 
		UINT nIDBitmapResourceDisabled = 0, 
		UINT uTransColor = 0, 
		LPCTSTR lpType = RT_BITMAP)
	{
		return LoadBitmaps(MAKEINTRESOURCE(nIDBitmapResource),
		MAKEINTRESOURCE(nIDBitmapResourceSel),
		MAKEINTRESOURCE(nIDBitmapResourceOver),
		MAKEINTRESOURCE(nIDBitmapResourceDisabled), 
		uTransColor,
		lpType);
	}
	
	BOOL LoadBitmaps(LPCTSTR lpszBitmapResource,
			LPCTSTR lpszBitmapResourceSel = NULL,
			LPCTSTR lpszBitmapResourceOver = NULL,
			LPCTSTR lpszBitmapResourceDisabled = NULL,
			UINT uTransColor = 0,
			LPCTSTR lpType = RT_BITMAP);
	
	BOOL AutoLoad(UINT nID, CWnd* pParent);
	void SetTransColor(UINT uTransColor)
	{
		m_uTransColor = uTransColor;
	};
	void SetDrawType(int nType)
	{
		m_nDrawType = min(nType, enumDRAW_USE_ALPHA);
		m_nDrawType = max(nType, enumDRAW_USE_NORMAL);
	};
// Operations
	void SizeToContent();

// Implementation:
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	// all bitmaps must be the same size
	CDIB	m_bitmap;           // normal image (REQUIRED)
	CDIB	m_bitmapSel;        // selected image (OPTIONAL)
	CDIB	m_bitmapOver;      // focused but not selected (OPTIONAL)
	CDIB	m_bitmapDisabled;   // disabled bitmap (OPTIONAL)
	UINT	m_uTransColor;		// color key for Transparent
	int		m_nDrawType;
	BOOL	m_bMouseOnButton;
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	void	CancelHover();
	// Generated message map functions
protected:
	//{{AFX_MSG(CBmpButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPBUTTON_H__BB4EE3C8_BBB9_4E51_9D53_B6A443AE7FF0__INCLUDED_)
