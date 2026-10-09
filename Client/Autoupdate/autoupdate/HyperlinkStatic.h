#if !defined(AFX_HYPERLINKSTATIC_H__32A71426_1315_407C_9D90_A484C5589D80__INCLUDED_)
#define AFX_HYPERLINKSTATIC_H__32A71426_1315_407C_9D90_A484C5589D80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HyperlinkStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHyperlinkStatic window

class CHyperlinkStatic : public CStatic
{
// Construction
public:
	CHyperlinkStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHyperlinkStatic)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHyperlinkStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHyperlinkStatic)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	//*********************************************************************
	// function : 设置超级连接
	//*********************************************************************
	void SetHyperlink(CString strUrl);
	//*********************************************************************
	// function : 设置显示文本
	//*********************************************************************
	void SetCaption(CString strCaption);
	//*********************************************************************
	// function : 设置缺省颜色
	//*********************************************************************
	void SetDefaultColor(COLORREF color)
	{
		m_crDefault = color;
		m_crCurrent = color;
	}
	//*********************************************************************
	// function : 设置鼠标覆盖颜色
	//*********************************************************************
	void SetOnMouseColor(COLORREF color)
	{ m_crOnMouse = color; }
	//*********************************************************************
	// function : 修改字体大小
	//*********************************************************************
	void SetFontSizeIncrement(int nIncrement)
	{
		m_nFontIncrement = nIncrement;
	}
	//*********************************************************************
	// function : 设置是否有下划线
	//*********************************************************************
	void SetUnderLine(BOOL bUnderLine)
	{
		m_bUnderLine = bUnderLine;
	}
	//*********************************************************************
	// function : 获取文本尺寸
	//*********************************************************************
	CSize GetCaptionSize();
private:
	CString _strCaption, _strHyperlink;
	CFont _fontCaption;
	CSize _sizeCaption;
	bool _bCreateFont, _bMouseInControl, _bGetCaptionSize;
	HCURSOR	_hHandCursor, _hArrowCursor;

	void CreateFont(CFont *pFont, LONG lFontIncrement);
	void DrawText(CDC* pDC, COLORREF color);
	void SetCaptionSize();
	bool InCaptionRange(CPoint &point);
	//*********************************************************************
	// function : 设置制定文本在某个字体下的尺寸
	//*********************************************************************
	CSize GetTextSize(CFont *pFont, const CString &strText);
private:
	int		 m_nFontIncrement;
	BOOL	 m_bUnderLine;
	COLORREF m_crDefault;		// 缺省字体颜色
	COLORREF m_crOnMouse;		// 鼠标覆盖时的颜色
	COLORREF m_crCurrent;		// 当前字体
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HYPERLINKSTATIC_H__32A71426_1315_407C_9D90_A484C5589D80__INCLUDED_)
