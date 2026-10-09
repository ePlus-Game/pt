// TransparentStatic.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "TransparentStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransparentStatic

CTransparentStatic::CTransparentStatic() :
	m_colorCaption(RGB(33, 50, 35)), m_nFontIncrement(0)
{
}

CTransparentStatic::~CTransparentStatic()
{
}


BEGIN_MESSAGE_MAP(CTransparentStatic, CStatic)
	//{{AFX_MSG_MAP(CTransparentStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransparentStatic message handlers

void CTransparentStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (HFONT(m_fontCaption) != NULL || CreateFont(m_fontCaption))
	{
		CFont *pOldFont = (CFont*)dc.SelectObject(&m_fontCaption);
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(m_colorCaption);
		CString strCaption;
		GetWindowText(strCaption);
		dc.TextOut(0, 0, strCaption);
		dc.SelectObject(pOldFont);
	}
}

BOOL CTransparentStatic::CreateFont(CFont &font)
{
	ASSERT(HFONT(font) == NULL);
	BOOL bResult = FALSE;
	CFont* pParentFont = GetParent()->GetFont();
	ASSERT(pParentFont);
	if (pParentFont)
	{
		LOGFONT lf = {0};
		pParentFont->GetObject(sizeof(lf), &lf);
		lf.lfHeight += m_nFontIncrement;
		font.CreateFontIndirect(&lf);
		ASSERT(HFONT(font) != NULL);
		bResult = TRUE;
	}
	return bResult;
}

void CTransparentStatic::UpdateCtrl()
{
    CWnd* pParent = GetParent();
    CRect rect;
    
    GetWindowRect(rect);
    pParent->ScreenToClient(rect);
    rect.DeflateRect(0, 0);
    
	pParent->InvalidateRect(rect, TRUE);   
   // pParent->InvalidateRect(rect, FALSE);    
}

HBRUSH CTransparentStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    m_Brush.DeleteObject();
    
    if (m_BackColor == TRANS_BACK) {
        m_Brush.CreateStockObject(HOLLOW_BRUSH);
        pDC->SetBkMode(TRANSPARENT);
    }
    else {
        m_Brush.CreateSolidBrush(m_BackColor);
        pDC->SetBkColor(m_BackColor);
    }
    
    pDC->SetTextColor(m_colorCaption);
    
    return (HBRUSH)m_Brush;
}