// ProgressCtrlST.cpp: implementation of the CProgressCtrlST class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProgressCtrlST.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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
  );

CProgressCtrlST::CProgressCtrlST() : m_rectClient(0, 0, 0, 0)
{	
	m_nBlockSpace = 2;
}

CProgressCtrlST::~CProgressCtrlST()
{
}


BEGIN_MESSAGE_MAP(CProgressCtrlST, CProgressCtrl)
	//{{AFX_MSG_MAP(CProgressCtrlST)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressCtrlST message handlers




void CProgressCtrlST::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	// Restore Parent's Image
	//PaintBk(&dc);

	//Paint Progress Bar

	PaintProgressBar(&dc);
	// Do not call CProgressCtrl::OnPaint() for painting messages
}

void CProgressCtrlST::PaintBk(CDC * pDC)
{
	CClientDC clDC(GetParent());
}

void CProgressCtrlST::PaintProgressBar(CDC * pDC)
{
	// Paint the ProgressBar Background
	ASSERT(pDC != NULL);
	CRect rc;
	HDC dcMem;
	GetClientRect(&rc);
	int nClientWidth = rc.Width();
	int nClientHeight = rc.Height();
	
	dcMem = ::CreateCompatibleDC(pDC->m_hDC);
	HBITMAP oldBmp = (HBITMAP)::SelectObject(dcMem, m_bmpProgressBk.m_hObject);
		
	int nWidth = min(m_nProgressBarWidth, nClientWidth);
	int nHeight = min(m_nProgressBarHeight, nClientHeight);
	

	TransparentBltU(pDC->m_hDC, 0, 0, nWidth, nHeight, 
		dcMem, 0, 0, nWidth, nHeight, m_uBKTransColor);
	
	// Paint the blocks
	int nLow, nHigh;
	GetRange(nLow, nHigh);
	int nPos = GetPos();
	int nBlockNum = ( m_rectClient.Width() + m_nBlockWidth + m_nBlockSpace ) * (nPos - nLow) / 
		( (m_nBlockWidth + m_nBlockSpace) * (nHigh - nLow));
	int nX = m_rectClient.left, nY = m_rectClient.top;
	SelectObject(dcMem, m_bmpBlock);
	int nBlockWidth = m_nBlockWidth;
	for ( int i = 0; i < nBlockNum; ++i, nX += (m_nBlockWidth + m_nBlockSpace) )
	{
		if ( nX + m_nBlockWidth > m_rectClient.right )
		{
			nBlockWidth -= nX + m_nBlockWidth - m_rectClient.right;			
		}
		TransparentBltU(pDC->m_hDC, nX, nY, nBlockWidth, m_nBlockHeight, 
			dcMem, 0, 0, nBlockWidth, m_nBlockHeight, m_uBlockTransColor);
	}
	SelectObject(dcMem, oldBmp);
	DeleteDC(dcMem);
	// Paint the Blocks
}


void CProgressCtrlST::SetBitmap(LPCTSTR progressbarBkBmp, UINT bkTransColor, LPCTSTR blockBmp, UINT blockTransColor)
{
	HBITMAP hBmp = NULL;
	BITMAP	bmpInfo;

	hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), progressbarBkBmp, IMAGE_BITMAP, 0, 0, 0);
	if ( hBmp == NULL )
	{
		hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), progressbarBkBmp, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	if ( hBmp != NULL )
	{
		m_bmpProgressBk.Attach(hBmp);
		m_bmpProgressBk.GetBitmap(&bmpInfo);
		m_nProgressBarWidth = bmpInfo.bmWidth;
		m_nProgressBarHeight = bmpInfo.bmHeight;
		m_uBKTransColor = bkTransColor;
	}	

	hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), blockBmp, IMAGE_BITMAP, 0, 0, 0);
	if ( hBmp == NULL )
	{
		hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), blockBmp, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	if ( hBmp != NULL )
	{
		m_bmpBlock.Attach(hBmp);
		m_bmpBlock.GetBitmap(&bmpInfo);
		m_nBlockWidth = bmpInfo.bmWidth;
		m_nBlockHeight = bmpInfo.bmHeight;
		m_uBlockTransColor = blockTransColor;
	}
}

void CProgressCtrlST::SetBitmap(UINT progressbarBkBmp, UINT bkTransColor, UINT blockBmp, UINT blockTransColor)
{
	HBITMAP hBmp = NULL;
	BITMAP	bmpInfo;
	hBmp = (HBITMAP)::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(progressbarBkBmp));
	if ( hBmp != NULL )
	{		
		m_bmpProgressBk.Attach(hBmp);
		m_bmpProgressBk.GetBitmap(&bmpInfo);
		m_nProgressBarWidth = bmpInfo.bmWidth;
		m_nProgressBarHeight = bmpInfo.bmHeight;
		m_uBKTransColor = bkTransColor;

	}

	hBmp = (HBITMAP)::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(blockBmp));
	if ( hBmp != NULL )
	{
		m_bmpBlock.Attach(hBmp);

		m_bmpBlock.GetBitmap(&bmpInfo);
		m_nBlockWidth = bmpInfo.bmWidth;
		m_nBlockHeight = bmpInfo.bmHeight;
		m_uBlockTransColor = blockTransColor;
	}

}
/*
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
*/
