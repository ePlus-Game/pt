// HyperlinkStatic.cpp : implementation file
//

#include "stdafx.h"
#include "HyperlinkStatic.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHyperlinkStatic

CHyperlinkStatic::CHyperlinkStatic() :
	m_nFontIncrement(0)
{
	_strCaption = _strHyperlink = _TT("");	 
	_bMouseInControl = _bCreateFont = _bGetCaptionSize = false;

	_hHandCursor = ::LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_CUR_HAND));
	_hArrowCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));
	ASSERT(_hHandCursor && _hArrowCursor);

	// 缺省属性
	m_bUnderLine = TRUE;			// 有下划线
	m_crDefault = RGB(0, 0, 255);	// 蓝色
	m_crOnMouse = m_crDefault;		// 鼠标覆盖时，颜色不变
	m_crCurrent = m_crDefault;		// 当前字体蓝色
}

CHyperlinkStatic::~CHyperlinkStatic()
{
}

BEGIN_MESSAGE_MAP(CHyperlinkStatic, CStatic)
	//{{AFX_MSG_MAP(CHyperlinkStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperlinkStatic message handlers

void CHyperlinkStatic::SetHyperlink(CString strHyperlink)
{
	_strHyperlink = strHyperlink;
}

void CHyperlinkStatic::SetCaption(CString strCaption)
{
	_strCaption = strCaption;
	_bGetCaptionSize = false;
	Invalidate(TRUE);
}

void CHyperlinkStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!_bGetCaptionSize)
		SetCaptionSize();
	if (!_strHyperlink.IsEmpty() && InCaptionRange(point))
		ShellExecute(0, "open", _strHyperlink, 0, 0, SW_SHOWNORMAL);
	CStatic::OnLButtonDown(nFlags, point);
}

void CHyperlinkStatic::DrawText(CDC* pDC, COLORREF color)
{
	if (NULL == pDC)
		return;

	CFont *pOldFont = (CFont*)pDC->SelectObject(&_fontCaption);
	int nBkMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF nOldColor = pDC->SetTextColor(color);

	pDC->TextOut(0, 0, _strCaption);

	pDC->SelectObject(pOldFont);
	pDC->SetBkMode(nBkMode);
	pDC->SetTextColor(nOldColor);
}

void CHyperlinkStatic::OnPaint() 
{
	if (!_fontCaption.m_hObject)
	{
		CreateFont(&_fontCaption, m_nFontIncrement);
		_bCreateFont = TRUE;
	}

	CPaintDC dc(this);
//	CRect rctClient;
//	this->GetClientRect(&rctClient);
//
//	CDC memDC;
//	CBitmap memBmp;
//	memDC.CreateCompatibleDC(&dc);
//	memBmp.CreateCompatibleBitmap(&dc, rctClient.Width(), rctClient.Height());
//	CBitmap* pOldBmp = memDC.SelectObject(&memBmp);
//	CBrush bgBrush(0xffffff);
//	CRect rctDraw(0, 0, rctClient.Width(), rctClient.Height());
//	memDC.FillRect(rctDraw, &bgBrush);
	
	DrawText(&dc, m_crCurrent);

//	dc.BitBlt(0, 0, rctClient.Width(), rctClient.Height(),
//				&memDC, 0, 0, SRCCOPY);

//	memDC.SelectObject(pOldBmp);
/*
	if ( _bCreateFont == false )
		CreateFont();
	CPaintDC dc(this);
	CFont *pOldFont = (CFont*)dc.SelectObject(&_fontCaption);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_tagFontColor);
	dc.TextOut(0, 0, _strCaption);
	dc.SelectObject(pOldFont);
*/
}

void CHyperlinkStatic::OnDestroy() 
{
	CStatic::OnDestroy();
	_fontCaption.DeleteObject();
}

void CHyperlinkStatic::PreSubclassWindow() 
{
	ModifyStyle(0, SS_NOTIFY, TRUE);
	GetWindowText(_strCaption);
	_bGetCaptionSize = FALSE;
	CStatic::PreSubclassWindow();
}

LRESULT CHyperlinkStatic::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	_bMouseInControl = false;
	// 恢复光标
	if (!_strHyperlink.IsEmpty())
		::SetCursor(_hArrowCursor);	
	// 恢复字体颜色
	if (m_crCurrent != m_crDefault)
	{
		m_crCurrent = m_crDefault;
		Invalidate(TRUE);
	}
	return 0;
}

void CHyperlinkStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( _bMouseInControl == false ) {
		//Track the mouse leave event
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
        tme.hwndTrack = GetSafeHwnd();
        tme.dwFlags = TME_LEAVE;        
		_TrackMouseEvent(&tme);
		_bMouseInControl = true;
	}
	else {
		if (!_bGetCaptionSize)
			SetCaptionSize();
		// 显示手形光标
		if (!_strHyperlink.IsEmpty())
			::SetCursor(_hHandCursor);//(InCaptionRange(point))?_hHandCursor:_hArrowCursor);
		// 改变字体颜色
		if (m_crCurrent != m_crOnMouse)
		{
			m_crCurrent = m_crOnMouse;
			Invalidate(TRUE);
		}
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CHyperlinkStatic::CreateFont(CFont *pFont, LONG lFontIncrement)
{
	CFont* pFontParent = GetParent()->GetFont();	
	ASSERT(pFontParent);

	LOGFONT lf;
	pFontParent->GetObject(sizeof(lf), &lf);
	
	lf.lfUnderline = m_bUnderLine;
	lf.lfHeight += lFontIncrement;
	pFont->CreateFontIndirect(&lf);
}

//*********************************************************************
// function : 获取制定文本在某个字体下的尺寸
//*********************************************************************
CSize CHyperlinkStatic::GetTextSize(CFont *pFont, const CString &strText)
{
	ASSERT(pFont);
	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(pFont);
	CSize size = dc.GetTextExtent(strText);
	dc.SelectObject(pOldFont);
	return size;
}

//*********************************************************************
// function : 设置制定文本在某个字体下的尺寸
//*********************************************************************
void CHyperlinkStatic::SetCaptionSize()
{
	if (!_bCreateFont)
	{
		CreateFont(&_fontCaption, m_nFontIncrement);
		_bCreateFont = TRUE;
	}
	if (!_bGetCaptionSize)
	{
		_sizeCaption = GetTextSize(&_fontCaption, _strCaption);
		_bGetCaptionSize = TRUE;
	}
}

//*********************************************************************
// function : 获取文本尺寸
//*********************************************************************
CSize CHyperlinkStatic::GetCaptionSize()
{
	if (!_bGetCaptionSize)
		SetCaptionSize();
	return _sizeCaption;
}

bool CHyperlinkStatic::InCaptionRange(CPoint &point)
{
	if ( _bGetCaptionSize == false )
		return false;
	return (( point.x >= 0 )&&( point.x < _sizeCaption.cx ) &&
			( point.y >= 0 )&&( point.y < _sizeCaption.cy ));
}

int CHyperlinkStatic::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CreateFont(&_fontCaption, m_nFontIncrement);
	_bCreateFont = TRUE;
	return 0;
}
