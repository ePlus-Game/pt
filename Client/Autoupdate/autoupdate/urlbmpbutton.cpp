// urlbmpbutton.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "urlbmpbutton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CURLBmpButton
IMPLEMENT_DYNAMIC(CURLBmpButton, CButton)

CURLBmpButton::CURLBmpButton()
{
	m_strURL = _TT("");
	m_hHandCursor = ::LoadCursor(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_CUR_HAND));	
}

CURLBmpButton::~CURLBmpButton()
{
	
}



BEGIN_MESSAGE_MAP(CURLBmpButton, CBmpButton)
	//{{AFX_MSG_MAP(CURLBmpButton)
	ON_WM_SETCURSOR()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CURLBmpButton message handlers

BOOL CURLBmpButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if ( !m_strURL.IsEmpty() && m_hHandCursor != NULL )
	{
		::SetCursor(m_hHandCursor);
		return TRUE;
	}
	else
	{
		return CBmpButton::OnSetCursor(pWnd, nHitTest, message);
	}
}

void CURLBmpButton::SetURL(const CString &strURL)
{
	m_strURL = strURL;
}

void CURLBmpButton::OnClicked() 
{
	// TODO: Add your control notification handler code here
	if ( !m_strURL.IsEmpty() )
	{
		ShellExecute(0, "open", m_strURL, 0, 0, SW_SHOWNORMAL);
	}
}
