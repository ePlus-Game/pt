// MsgWnd.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateDLL.h"
#include "MsgWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgWnd

CMsgWnd::CMsgWnd()
{
}

CMsgWnd::~CMsgWnd()
{
}


BEGIN_MESSAGE_MAP(CMsgWnd, CWnd)
	//{{AFX_MSG_MAP(CMsgWnd)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_DOWNNOTIFY_DEFAULT, OnDefaultMessage)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMsgWnd message handlers

BOOL CMsgWnd::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::PreTranslateMessage(pMsg);
}

void CMsgWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

LRESULT CMsgWnd::OnDefaultMessage(WPARAM wParam, LPARAM lParam)
{
	return g_theApp.DownDispatch(GetCurrentMessage());
}

