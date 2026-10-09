// WndTool.cpp: implementation of the CWndTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autoupdate.h"
#include "WndTool.h"
#include "HyperlinkStatic.h"
#include "urlbmpbutton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndTool::CWndTool(void *pThis) : m_pThis(pThis)
{
	ASSERT(pThis);
}

CWndTool::~CWndTool()
{

}

//*********************************************************************
// function		: ShowWindows
// description	: 显示WindowRect数组中的所有控件窗口
// parameter	: pWndRects	窗口位置数组
// return		: void
//*********************************************************************
void CWndTool::ShowWindows(WindowRect *pWndRects)
{
	ASSERT(pWndRects);
	WindowRect *pRect = pWndRects;
	while (pRect->lWndOffset > 0)
	{
		CWnd *pWnd = MEMBER_ATOFFSET(CWnd, m_pThis, pRect->lWndOffset);
		pWnd->MoveWindow(&pRect->rect);
		if (!pRect->bShow)
			pWnd->ShowWindow(SW_HIDE);
		pRect++;
	}
}

//*********************************************************************
// function		: ShowBmpButtons
// description	: 显示WindowRect数组中的所有控件窗口
// parameter	: pWndRects	窗口位置数组
// return		: void
//*********************************************************************
void CWndTool::ShowBmpButtons(BMPButton *pBmpBtns)
{
	ASSERT(pBmpBtns);
	BMPButton *pBtns = pBmpBtns;
	while (pBtns->lWndOffset > 0)
	{
		CBmpButton *pWnd = MEMBER_ATOFFSET(CBmpButton, m_pThis, pBtns->lWndOffset);
		
		pWnd->MoveWindow(pBtns->ptPos.x, pBtns->ptPos.y, pBtns->size.cx, pBtns->size.cy);

		pWnd->SetDrawType(pBtns->byDrawType);		
		pWnd->LoadBitmaps(pBtns->dwUp, pBtns->dwDown, pBtns->dwOver,
			pBtns->dwDisalbe, pBtns->uTransColor, pBtns->lpType);
		
		if (!pBtns->bEnalbe)
			pWnd->EnableWindow(FALSE);
		pBtns++;
	}
}

void CWndTool::ShowUrlBmpButtons(URLBmpButton *pUrlBtns)
{
	ASSERT(pUrlBtns);
	URLBmpButton *pBtns = pUrlBtns;
	while (pBtns->lWndOffset > 0)
	{
		CURLBmpButton *pWnd = MEMBER_ATOFFSET(CURLBmpButton, m_pThis, pBtns->lWndOffset);
		pWnd->MoveWindow(pBtns->ptPos.x, pBtns->ptPos.y, pBtns->size.cx, pBtns->size.cy);
		if (!pBtns->bEnalbe)
			pWnd->EnableWindow(FALSE);
		pWnd->SetDrawType(pBtns->byDrawType);		
		pWnd->LoadBitmaps(pBtns->dwUp, pBtns->dwDown, pBtns->dwOver, 
			pBtns->dwDisalbe, pBtns->uTransColor, pBtns->lpType);
		pWnd->SetURL(pBtns->pcszURL);
		pBtns++;
	}
}

//*********************************************************************
// function		: ShowWindows
// description	: 设置超级联接控件的URL
// parameter	: pUrlCtls	控件数组
// return		: void
//*********************************************************************
void CWndTool::ShowUrlCtls(UrlLink *pUrlCtls)
{
	ASSERT(pUrlCtls);
	UrlLink *pUrl = pUrlCtls;
	while (pUrl->lWndOffset > 0)
	{
		CHyperlinkStatic *pLink = MEMBER_ATOFFSET(CHyperlinkStatic, m_pThis, pUrl->lWndOffset);
		pLink->SetHyperlink(pUrl->pcszCtlUrl);
		pLink->SetUnderLine(pUrl->bUnderLine);
		pLink->SetDefaultColor(pUrl->crDefault);
		pLink->SetOnMouseColor(pUrl->crOnMouse);
		pLink->SetFontSizeIncrement(pUrl->nFontIncrement);
		pLink->Invalidate(TRUE);
		pUrl++;
	}
}

//*********************************************************************
// function		: ShowPicture
// description	: 绘制图像
// parameter	: pDC
// parameter	: pPicture 图像对象
// parameter	: lLeft	  显示的最左位置
// parameter	: lTop	  显示的最上位置
// return		: BOOL
//*********************************************************************
BOOL CWndTool::ShowPicture(CDC *pDC, CPicture *pPicture, LPRECT pRect)
{
	ASSERT(pDC && pPicture && pRect);
	CDC cdc;
	CBitmap bmp;
	CPicture *pOldPic = NULL;
	cdc.CreateCompatibleDC(pDC);
	//bmp.CreateCompatibleBitmap(pDC, nWidth, nHeight);
	pPicture->Render(pDC, pRect);
	return TRUE;
}

//*********************************************************************
// function		: ShowBitmap
// description	: 绘制位图
// parameter	: pDC
// parameter	: pBitMap 位图对象
// parameter	: lLeft	  显示的最左位置
// parameter	: lTop	  显示的最上位置
// return		: void
//*********************************************************************
BOOL CWndTool::ShowBitmap(CDC *pDC, CBitmap *pBitMap, LONG lLeft, LONG lTop)
{
	ASSERT(pDC && pBitMap && pBitMap->m_hObject != 0);
	CDC cdc;
	CBitmap *pOldBitmap = NULL;
	cdc.CreateCompatibleDC(pDC);

	pOldBitmap = cdc.SelectObject(pBitMap);
	ASSERT(pOldBitmap);
	
	BITMAP bmp;
	pBitMap->GetBitmap(&bmp);
	
	BOOL bOK = pDC->BitBlt(
		lLeft,
		lTop,
		lLeft + bmp.bmWidth,
		lTop + bmp.bmHeight,
		&cdc,
		0,
		0,
		SRCCOPY);
	ASSERT(bOK);
	cdc.SelectObject(pOldBitmap);
	return bOK;
}

//*********************************************************************
// function		: 坐标是否位于位图范围内
// parameter	: x 起点的横坐标
// parameter	: y 起点的纵坐标
// parameter	: pBitmap 位图对象
// parameter	: pt	落点
// return		: 如果落点在区域内，返回TRUE，否则返回FALSE
//*********************************************************************
BOOL CWndTool::InBmpZoom(long x, long y, CBitmap *pBitmap, const CPoint &pt)
{
	ASSERT(pBitmap);
	BITMAP bmp;
	BOOL bInZoom = FALSE;
	pBitmap->GetBitmap(&bmp);
	if (pt.x >= x &&
		pt.x <= x + bmp.bmWidth &&
		pt.y >= y &&
		pt.y <= y + bmp.bmHeight)
	{
		bInZoom = TRUE;
	}
	return bInZoom;
}

//*********************************************************************
// function		: 获取子控件的窗口位置
// parameter	: pChildCwnd 子窗口对象
// parameter	: pRects	注册的窗口位置数组
// return		: RECT* 查找到的窗口位置结构指针，返回没有找到返回NULL
//*********************************************************************
RECT *CWndTool::GetClientRect(CWnd *pChildWnd, WindowRect *pRects)
{
	ASSERT(pRects && pChildWnd && m_pThis);
	RECT *pWndRect = NULL;
	if (NULL == pWndRect)
	{
		WindowRect *pTmpRect = pRects;
		while (!pWndRect && pTmpRect->lWndOffset > 0)
		{
			if ((char*)pChildWnd != (char*)m_pThis + pTmpRect->lWndOffset)
				pTmpRect++;
			else
				pWndRect = &pTmpRect->rect;
		}
	}
	ASSERT(pWndRect);
	return pWndRect;
}

