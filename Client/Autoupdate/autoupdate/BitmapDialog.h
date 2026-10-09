/*
	带有背景图片的窗口基类
	函数的调用顺序为:
	LoadBitmap;
	SetTransColor;
	SetTransparent;
	如果需要支btmap以外的图片格式，请先将其转为bmp
*/

#ifndef _BITMAPDIALOG_H_
#define _BITMAPDIALOG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BitmapDialog.h : header file
//

enum LayOutStyle
{
    LO_DEFAULT,
    LO_TILE,    // Tile the background picture
    LO_CENTER,  // Center the background picture
    LO_STRETCH, // Stretch the background picture to the dialog window size
    LO_RESIZE   // Resize the dialog so that it just fits the background 
};

/////////////////////////////////////////////////////////////////////////////
// CBitmapDialog dialog

class CBitmapDialog : public CDialog
{
// Construction
public:

	CBitmapDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL,
		LPCTSTR lpszResourceName = NULL, UINT nIDResource = 0,
		LPCTSTR lpszFilename = NULL, CBitmap *pBitmap = NULL);

	CBitmapDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL,
		LPCTSTR lpszResourceName = NULL, UINT nIDResource = 0,
		LPCTSTR lpszFilename = NULL, CBitmap *pBitmap = NULL);

	CBitmapDialog(LPCTSTR lpszResourceName = NULL, UINT nIDResource = 0,
		LPCTSTR lpszFilename = NULL, CBitmap *pBitmap = NULL);

	// Destructor (just release the bitmap)
	virtual ~CBitmapDialog() { ReleaseBitmap (); }

	// Bitmap
	BOOL LoadBitmap(LPCTSTR lpszResourceName,
		LPCTSTR lpszFilename);						// Load from resource or file
	BOOL SetBitmap(UINT nIDResource);				// Load from resource
	BOOL CopyBitmapFrom(CBitmap *pBitmap);	// Copy of user defined
	void SetBitmap(CBitmap *pBitmap);			// User defined
	void ReleaseBitmap();

	// Transparency
	void SetTransparentColor(COLORREF col);
	void SetTransparent(BOOL bTransparent);
	 
	// Static control transparency
	void SetStaticTransparent (BOOL bTransparent) { m_bStaticTransparent = bTransparent; }

	// Clicking anywhere moves
    void EnableEasyMove(BOOL bMove) { m_bClickAnywhereMove = bMove; }
	
	//Set the Dlialog style
	void SetLayOutStyle(const LayOutStyle& style) { m_LayOutStyle = style;  }

// Dialog Data
	//{{AFX_DATA(CBitmapDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapDialog)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBitmapDialog)
	afx_msg BOOL OnEraseBkgnd (CDC *pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg UINT OnNcHitTest(CPoint point);
	DECLARE_MESSAGE_MAP()
	
private:
	// Common constructor
	void Constructor (LPCTSTR lpszResourceName, UINT nIDResource,
		LPCTSTR lpszFilename, CBitmap *pBitmap);

	void	MakeWindowRgn();
	void	MakeBitmapRgn();
	//根据窗口的属性得到要绘制窗口的实际大小
	void	GetDialogSize(int nDiagW, int nDiagH,
						int nBitmapW, int nBitmapH,
						int& nW, int& nH);

	//主要在子类里面处理该函数，以便子窗口容易处理透明
	virtual void OnPostEraseBkgnd(CDC* pDC);

private:
	// Transparency
	BOOL		m_bTransparent;
	BOOL		m_bStaticTransparent;
	HBRUSH		m_brushHollow;
	COLORREF	m_colTrans;

	// Clicking anywhere moves
	BOOL		m_bClickAnywhereMove;

	// Bitmap and its DC
	BOOL	m_bBitmapCreated, m_bBitmapExists;
	CBitmap	*m_bmBitmap;
	int		m_nBitmapW;
	int		m_nBitmapH;

	//style
	LayOutStyle m_LayOutStyle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPDIALOG_H__A76F9E74_DF43_11D4_AE27_4854E828E6FD__INCLUDED_)
