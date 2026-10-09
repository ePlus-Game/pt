// bmpbutton.cpp : implementation file
//

#include "stdafx.h"
#include "autoupdate.h"
#include "bmpbutton.h"
#include "winuser.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpButton
IMPLEMENT_DYNAMIC(CBmpButton, CButton)

extern bool TransparentBltU(
     HDC dcDest,         // handle to Dest DC
     int nXOriginDest,   // x-coord of destination upper-left corner
     int nYOriginDest,   // y-coord of destination upper-left corner
     int nWidthDest,     // width of destination rectangle
     int nHeightDest,    // height of destination rectangle
     HDC dcSrc,          // handle to source DC
     int nXOriginSrc,    // x-coord of source upper-left corner
     int nYOriginSrc,    // y-coord of source upper-left corner
     int nWidthSrc,      // width of source rectangle
     int nHeightSrc,     // height of source rectangle
     UINT crTransparent  // color to make transparent
  );
CBmpButton::CBmpButton()
{
	m_bMouseOnButton = FALSE;
	m_nDrawType = enumDRAW_USE_NORMAL;
}

CBmpButton::~CBmpButton()
{
}


BEGIN_MESSAGE_MAP(CBmpButton, CButton)
	//{{AFX_MSG_MAP(CBmpButton)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()	
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpButton message handlers


// LoadBitmaps will load in one, two, three or all four bitmaps
// returns TRUE if all specified images are loaded
BOOL CBmpButton::LoadBitmaps(LPCTSTR lpszBitmapResource,
	LPCTSTR lpszBitmapResourceSel, LPCTSTR lpszBitmapResourceOver,
	LPCTSTR lpszBitmapResourceDisabled,
	UINT uTransColor,
	LPCTSTR lpType)
{
	// delete old bitmaps (if present)
	m_bitmap.DeleteObject();
	m_bitmapSel.DeleteObject();
	m_bitmapOver.DeleteObject();
	m_bitmapDisabled.DeleteObject();
	
	m_uTransColor = uTransColor;
	if (!m_bitmap.LoadBitmap(lpszBitmapResource))
	{		
		TRACE0(_TT("Failed to load bitmap for normal image.\n"));
		return FALSE;   // need this one image
	}	
	BOOL bAllLoaded = TRUE;
	if (lpszBitmapResourceSel != NULL)
	{
		if (!m_bitmapSel.LoadBitmap(lpszBitmapResourceSel))
		{
			TRACE0(_TT("Failed to load bitmap for selected image.\n"));
			bAllLoaded = FALSE;		
		}
	}
	if (lpszBitmapResourceOver != NULL)
	{
		if (!m_bitmapOver.LoadBitmap(lpszBitmapResourceOver))
			bAllLoaded = FALSE;
	}
	if (lpszBitmapResourceDisabled != NULL)
	{
		if (!m_bitmapDisabled.LoadBitmap(lpszBitmapResourceDisabled))
			bAllLoaded = FALSE;
	}

	return bAllLoaded;
}

// SizeToContent will resize the button to the size of the bitmap
void CBmpButton::SizeToContent()
{
	ASSERT(m_bitmap.m_hObject != NULL);
	CSize bitmapSize;
	BITMAP bmInfo;
	VERIFY(m_bitmap.GetObject(sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
	VERIFY(SetWindowPos(NULL, -1, -1, bmInfo.bmWidth, bmInfo.bmHeight,
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
}

// Autoload will load the bitmap resources based on the text of
//  the button
// Using suffices "U", "D", "F" and "X" for up/down/focus/disabled
BOOL CBmpButton::AutoLoad(UINT nID, CWnd* pParent)
{
	// first attach the CBmpButton to the dialog control
	if (!SubclassDlgItem(nID, pParent))
		return FALSE;

	CString buttonName;
	GetWindowText(buttonName);
	ASSERT(!buttonName.IsEmpty());      // must provide a title

	LoadBitmaps(buttonName + _TT("U"), buttonName + _TT("D"),
	  buttonName + _TT("F"), buttonName + _TT("X"));

	// we need at least the primary
	if (m_bitmap.m_hObject == NULL)
		return FALSE;

	// size to content
	SizeToContent();
	return TRUE;
}

// Draw the appropriate bitmap
void CBmpButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	// must have at least the first bitmap loaded before calling DrawItem
	ASSERT(m_bitmap.m_hObject != NULL);     // required

	// use the main bitmap for up, the selected bitmap for down
	CDIB* pBitmap = &m_bitmap;
	UINT state = lpDIS->itemState;
	if ((state & ODS_SELECTED) && m_bitmapSel.m_hObject != NULL)
		pBitmap = &m_bitmapSel;	
	else if ((state & ODS_DISABLED) && m_bitmapDisabled.m_hObject != NULL)
		pBitmap = &m_bitmapDisabled;   // last image for disabled
	else if ( m_bMouseOnButton && m_bitmapOver.m_hObject != NULL)
		pBitmap = &m_bitmapOver;						// 
	
	// draw the whole button
	CRect rect;
	rect.CopyRect(&lpDIS->rcItem);
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	BITMAP	bmpInfo;
	pBitmap->GetBitmap(&bmpInfo);
	int nWidth = min(rect.Width(), bmpInfo.bmWidth);
	int nHeight = min(rect.Height(), bmpInfo.bmHeight);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	memDC.SelectObject(pBitmap);

	switch ( m_nDrawType )
	{
		case enumDRAW_USE_COLORKEY:
			{
				TransparentBltU(pDC->GetSafeHdc(), rect.left, rect.top, nWidth, nHeight, 
					memDC.GetSafeHdc(), 0, 0, nWidth, nHeight, m_uTransColor);
			}
			break;
		case enumDRAW_USE_ALPHA:
			{
				CWnd *pParentWnd = GetParent();
				
				CRect rtParentRect;
				GetWindowRect(&rtParentRect);
				pParentWnd->ScreenToClient(&rtParentRect);
				
				
				CDC * pParentDC = pParentWnd->GetDC();
				CDibDC dibDC;				
				
				dibDC.CreateCompatibleDC(pParentDC);	
				
				CBitmap tmpBmp;
				tmpBmp.CreateCompatibleBitmap(pParentDC, rect.Width(), rect.Height());
				
				dibDC.SelectObject(tmpBmp);	
				
				dibDC.BitBlt(0, 0, rect.Width(), rect.Height(), 
					pParentDC, rtParentRect.left, rtParentRect.top, SRCCOPY);
				
				
				dibDC.AlphaBlend(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0);
				
				pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
					&dibDC, 0, 0, SRCCOPY);
			}
			break;
		default:	//enumDRAW_USE_NORMAL
			{
				pDC->BitBlt(rect.left, rect.top, nWidth, nHeight, &memDC, 0, 0, SRCCOPY);
			}
			break;
	}
	
	
	
}

/////////////////////////////////////////////////////////////////////////////
// CBmpButton diagnostics
#ifdef _DEBUG
void CBmpButton::AssertValid() const
{
	CButton::AssertValid();

	m_bitmap.AssertValid();
	m_bitmapSel.AssertValid();
	m_bitmapOver.AssertValid();
	m_bitmapDisabled.AssertValid();
}

void CBmpButton::Dump(CDumpContext& dc) const
{
	CButton::Dump(dc);

	dc << "m_bitmap = " << (UINT)m_bitmap.m_hObject;
	dc << "\nm_bitmapSel = " << (UINT)m_bitmapSel.m_hObject;
	dc << "\nm_bitmapOver = " << (UINT)m_bitmapOver.m_hObject;
	dc << "\nm_bitmapDisabled = " << (UINT)m_bitmapDisabled.m_hObject;

	dc << "\n";
}
#endif

BOOL CBmpButton::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CButton::OnEraseBkgnd(pDC);
	return FALSE;
}

void CBmpButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWnd*				wndUnderMouse = NULL;
	CWnd*				wndActive = this;
	TRACKMOUSEEVENT		csTME;
	CButton::OnMouseMove(nFlags, point);

	ClientToScreen(&point);
	wndUnderMouse = WindowFromPoint(point);
	
	//wndActive = GetActiveWindow();

	if (wndUnderMouse && wndUnderMouse->m_hWnd == m_hWnd )//&& wndActive)
	{
		if ( m_bMouseOnButton == FALSE )
		{
			m_bMouseOnButton = TRUE;

			Invalidate();

			csTME.cbSize = sizeof(csTME);
			csTME.dwFlags = TME_LEAVE;
			csTME.hwndTrack = m_hWnd;
			::_TrackMouseEvent(&csTME);
		} // if
	} 
	else 
	{
		CancelHover();
	}

}

LRESULT CBmpButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	CancelHover();
	return 0;
} // End of OnMouseLeave

void CBmpButton::CancelHover()
{
	// Only for flat buttons	
	if (m_bMouseOnButton)
	{
		m_bMouseOnButton = FALSE;
		Invalidate();
	} // if
	
} // End of CancelHover


bool TransparentBltU(
     HDC dcDest,         // handle to Dest DC
     int nXOriginDest,   // x-coord of destination upper-left corner
     int nYOriginDest,   // y-coord of destination upper-left corner
     int nWidthDest,     // width of destination rectangle
     int nHeightDest,    // height of destination rectangle
     HDC dcSrc,          // handle to source DC
     int nXOriginSrc,    // x-coord of source upper-left corner
     int nYOriginSrc,    // y-coord of source upper-left corner
     int nWidthSrc,      // width of source rectangle
     int nHeightSrc,     // height of source rectangle
     UINT crTransparent  // color to make transparent
  )
{
     if (nWidthDest < 1) return false;
     if (nWidthSrc < 1) return false;
     if (nHeightDest < 1) return false;
     if (nHeightSrc < 1) return false;

     HDC dc = CreateCompatibleDC(NULL);
     HBITMAP bitmap = CreateBitmap(nWidthSrc, nHeightSrc, 1, GetDeviceCaps(dc,
                                                              BITSPIXEL), NULL);

     if (bitmap == NULL)
     {
         DeleteDC(dc);    
         return false;
     }

     HBITMAP oldBitmap = (HBITMAP)SelectObject(dc, bitmap);

     if (!BitBlt(dc, 0, 0, nWidthSrc, nHeightSrc, dcSrc, nXOriginSrc,
                                                         nYOriginSrc, SRCCOPY))
     {
         SelectObject(dc, oldBitmap); 
         DeleteObject(bitmap);        
         DeleteDC(dc);                
         return false;
     }

     HDC maskDC = CreateCompatibleDC(NULL);
     HBITMAP maskBitmap = CreateBitmap(nWidthSrc, nHeightSrc, 1, 1, NULL);

     if (maskBitmap == NULL)
     {
         SelectObject(dc, oldBitmap); 
         DeleteObject(bitmap);        
         DeleteDC(dc);                
         DeleteDC(maskDC);            
         return false;
     }

     HBITMAP oldMask =  (HBITMAP)SelectObject(maskDC, maskBitmap);

     SetBkColor(maskDC, RGB(0,0,0));
     SetTextColor(maskDC, RGB(255,255,255));
     if (!BitBlt(maskDC, 0,0,nWidthSrc,nHeightSrc,NULL,0,0,BLACKNESS))
     {
         SelectObject(maskDC, oldMask); 
         DeleteObject(maskBitmap);      
         DeleteDC(maskDC);              
         SelectObject(dc, oldBitmap);   
         DeleteObject(bitmap);          
         DeleteDC(dc);                  
         return false;
     }

     SetBkColor(dc, crTransparent);
     BitBlt(maskDC, 0,0,nWidthSrc,nHeightSrc,dc,0,0,SRCINVERT);

     SetBkColor(dc, RGB(0,0,0));
     SetTextColor(dc, RGB(255,255,255));
     BitBlt(dc, 0,0,nWidthSrc,nHeightSrc,maskDC,0,0,SRCAND);

     HDC newMaskDC = CreateCompatibleDC(NULL);
     HBITMAP newMask;
     newMask = CreateBitmap(nWidthDest, nHeightDest, 1,
                                    GetDeviceCaps(newMaskDC, BITSPIXEL), NULL);

     if (newMask == NULL)
     {
         SelectObject(dc, oldBitmap);
         DeleteDC(dc);
         SelectObject(maskDC, oldMask);
         DeleteDC(maskDC);
          DeleteDC(newMaskDC);
         DeleteObject(bitmap);     
         DeleteObject(maskBitmap); 
         return false;
     }

     SetStretchBltMode(newMaskDC, COLORONCOLOR);
     HBITMAP oldNewMask = (HBITMAP) SelectObject(newMaskDC, newMask);
     StretchBlt(newMaskDC, 0, 0, nWidthDest, nHeightDest, maskDC, 0, 0,
                                               nWidthSrc, nHeightSrc, SRCCOPY);

     SelectObject(maskDC, oldMask);
     DeleteDC(maskDC);
     DeleteObject(maskBitmap); 

     HDC newImageDC = CreateCompatibleDC(NULL);
     HBITMAP newImage = CreateBitmap(nWidthDest, nHeightDest, 1,
                                    GetDeviceCaps(newMaskDC, BITSPIXEL), NULL);

     if (newImage == NULL)
     {
         SelectObject(dc, oldBitmap);
         DeleteDC(dc);
         DeleteDC(newMaskDC);
         DeleteObject(bitmap);     
         return false;
     }

     HBITMAP oldNewImage = (HBITMAP)SelectObject(newImageDC, newImage);
     StretchBlt(newImageDC, 0, 0, nWidthDest, nHeightDest, dc, 0, 0, nWidthSrc,
                                                          nHeightSrc, SRCCOPY);

     SelectObject(dc, oldBitmap);
     DeleteDC(dc);
     DeleteObject(bitmap);     

     BitBlt( dcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
                                                      newMaskDC, 0, 0, SRCAND);

     BitBlt( dcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
                                                   newImageDC, 0, 0, SRCPAINT);

     SelectObject(newImageDC, oldNewImage);
     DeleteDC(newImageDC);
     SelectObject(newMaskDC, oldNewMask);
     DeleteDC(newMaskDC);
     DeleteObject(newImage);   
     DeleteObject(newMask);    

     return true;
}

