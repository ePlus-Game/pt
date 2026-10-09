//
//这个窗口类的目的是为了方便传递消息
//在下载类中，通过消息来告诉主进程，一个文件下载完毕了

#if !defined(AFX_MSGWND_H__34AED935_C812_4C6C_9D26_762562FC0BD5__INCLUDED_)
#define AFX_MSGWND_H__34AED935_C812_4C6C_9D26_762562FC0BD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgWnd window

class CMsgWnd : public CWnd
{
// Construction
public:
	CMsgWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMsgWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgWnd)
	afx_msg void OnDestroy();
	//}}AFX_MSG

    LRESULT OnDefaultMessage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGWND_H__34AED935_C812_4C6C_9D26_762562FC0BD5__INCLUDED_)
