/**********************************************************************
** Description : 透明的CStatic控件类
** FileName    : TransparentStatic.h
** Author      : wangbin
** Datetime    : 2004-05-10 11:00
** Comment     :
**********************************************************************/
#if !defined(AFX_TRANSPARENTSTATIC_H__85ADCC1E_E377_4FA3_9851_F3914FF086B0__INCLUDED_)
#define AFX_TRANSPARENTSTATIC_H__85ADCC1E_E377_4FA3_9851_F3914FF086B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransparentStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransparentStatic window
#define TRANS_BACK -1

class CTransparentStatic : public CStatic
{
// Construction
public:
	CTransparentStatic();

// Attributes
public:

// Operations
public:
	// 获取文本颜色，确省为RGB(33, 50, 35)
	inline COLORREF GetCaptionColor() const
	{
		return m_colorCaption;
	}
	// 设置文本颜色
	inline void SetCaptionColor(COLORREF color)
	{
		m_colorCaption = color;
	}
	// 改变字体大小
	void SetFontSizeIncrement(int nIncrement)
	{
		m_nFontIncrement = nIncrement;
	}

	void SetBackColor(COLORREF col)
	{
		m_BackColor = col;
		UpdateCtrl();      
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransparentStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTransparentStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTransparentStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
private:
	BOOL CreateFont(CFont &font);	// 创建字体属性
    void UpdateCtrl();
private:
    CBrush   m_Brush;
	CFont	 m_fontCaption;		// 文本字体属性
	COLORREF m_colorCaption;	// 文本颜色
	int		 m_nFontIncrement;	// 字体大小增量
	COLORREF m_BackColor;		//  背景的颜色
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSPARENTSTATIC_H__85ADCC1E_E377_4FA3_9851_F3914FF086B0__INCLUDED_)
