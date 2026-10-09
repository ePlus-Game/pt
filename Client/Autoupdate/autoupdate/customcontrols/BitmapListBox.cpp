// BitmapListBox.cpp : implementation file
//

#include "stdafx.h"
#include "BitmapListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapListBox

CBitmapListBox::CBitmapListBox()
{
	m_brHollow.CreateStockObject(HOLLOW_BRUSH);	
	iSelectChange = FALSE;
	m_pbmpOldBk = NULL;
}

CBitmapListBox::~CBitmapListBox()
{
	if(m_bmpBk.GetSafeHandle())
		m_bmpBk.DeleteObject();
}


BEGIN_MESSAGE_MAP(CBitmapListBox, CListBox)
	//{{AFX_MSG_MAP(CBitmapListBox)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapListBox message handlers

//DEL void CBitmapListBox::OnSelchange() 
//DEL {
//DEL 	iSelectChange = TRUE;
//DEL 	Invalidate(FALSE);
//DEL }

HBRUSH CBitmapListBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_rgbText);
	return m_brHollow;	// TODO: Return a non-NULL brush if the parent's handler should not be called
}

void CBitmapListBox::SetTextColor(COLORREF rgb)
{
	m_rgbText = rgb;
}

void CBitmapListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	Invalidate();	
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CBitmapListBox::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	Invalidate();
	CListBox::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CBitmapListBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	if (m_bmpBk.GetSafeHandle())
	{
		CBitmap* pOldBitmap ;
		CRect rect;
		GetClientRect(rect);
	
		CDC dcMem;
		dcMem.CreateCompatibleDC(pDC);
		pOldBitmap = dcMem.SelectObject(&m_bmpBk);
		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOldBitmap);	
	}
		
	return CListBox::OnEraseBkgnd(pDC);
}

void CBitmapListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	Invalidate();	
	
	CListBox::OnLButtonDown(nFlags, point);
}


HANDLE DDBToDIB(CBitmap &bitmap, DWORD dwCompression, CPalette *pPal)
{
	BITMAP bm;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	DWORD dwLen;
	HANDLE hDIB;
	HANDLE handle;
	HDC hDC;
	HPALETTE hPal;	
	ASSERT(bitmap.GetSafeHandle());
	if(dwCompression==BI_BITFIELDS)
		return NULL;
	hPal=(HPALETTE)pPal->GetSafeHandle();
	if(hPal==NULL)
	{
		hPal=(HPALETTE)GetStockObject(DEFAULT_PALETTE);
	}
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	bi.biSize=sizeof(BITMAPINFOHEADER);
	bi.biWidth=bm.bmWidth;	
	bi.biHeight=bm.bmHeight;
	bi.biPlanes=1;
	bi.biBitCount=bm.bmPlanes*bm.bmBitsPixel;
	bi.biCompression=dwCompression;
	bi.biSizeImage=0;
	bi.biXPelsPerMeter=0;
	bi.biYPelsPerMeter=0;
	bi.biClrUsed=0;
	bi.biClrImportant=0;

	int nColors=(1<<bi.biBitCount);
//	if(nColors>256)
//		nColors=0;
	dwLen=bi.biSize+nColors*sizeof(RGBQUAD);
	
	hDC=GetDC(NULL);
	hPal=SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	hDIB=GlobalAlloc(GMEM_FIXED,dwLen);
	if(!hDIB)
	{
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi=(LPBITMAPINFOHEADER)hDIB;
	*lpbi=bi;

	GetDIBits(hDC,(HBITMAP)bitmap.GetSafeHandle(),0L,(DWORD)bi.biHeight,
				(LPBYTE)NULL,(LPBITMAPINFO)lpbi,(DWORD)DIB_RGB_COLORS);
	bi=*lpbi;
	if(bi.biSizeImage==0)
	{
		bi.biSizeImage=(((bi.biWidth*bi.biBitCount)+31)&~31)/8*bi.biHeight;
		if(dwCompression!=BI_RGB)
		{
			bi.biSizeImage=(bi.biSizeImage*3)/2;
		}		
	}

	dwLen+=bi.biSizeImage;
	if(handle=GlobalReAlloc(hDIB,dwLen,GMEM_MOVEABLE))
	{
		hDIB=handle;
	}
	else
	{
		GlobalFree(hDIB);
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi=(LPBITMAPINFOHEADER)hDIB;

	BOOL bGotBits=GetDIBits(hDC,(HBITMAP)bitmap.GetSafeHandle(),0L,
							(DWORD)bi.biHeight,
							(LPBYTE)lpbi+(bi.biSize+nColors*sizeof(RGBQUAD)),
							(LPBITMAPINFO)lpbi,
							(DWORD)DIB_RGB_COLORS);

	if(!bGotBits)
	{
		GlobalFree(hDIB);
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	ReleaseDC(NULL,hDC);
	return hDIB;
}

BOOL WriteDIB(LPTSTR szFile, HANDLE hDIB)
{
	BITMAPFILEHEADER hdr;
	LPBITMAPINFOHEADER lpbi;
	if(!hDIB)
	{
		return FALSE;
	}
	CFile file;
	if(!file.Open(szFile,CFile::modeWrite|CFile::modeCreate,NULL))
	{
		return FALSE;
	}

	lpbi=(LPBITMAPINFOHEADER)hDIB;
	int nColors=1<<lpbi->biBitCount;

	hdr.bfType=((WORD)('M'<<8)|'B');
	hdr.bfSize=GlobalSize(hDIB)+sizeof(hdr);
	hdr.bfReserved1=0;
	hdr.bfReserved2=0;
	hdr.bfOffBits=(DWORD)(sizeof(hdr)+lpbi->biSize+nColors*sizeof(RGBQUAD));

	file.Write(&hdr,sizeof(hdr));
	file.Write(lpbi,GlobalSize(hDIB));
	file.Close();
	return TRUE;
}


void CBitmapListBox::SetBK(CDC *pDC)
{
	if (m_bmpBk.GetSafeHandle())
		m_bmpBk.DeleteObject();

	CRect rect;
	GetClientRect(&rect);
	CRect rect1;
	GetWindowRect(rect1);
	GetParent()->ScreenToClient(rect1);

	CDC m_dcBKMem;
	m_dcBKMem.CreateCompatibleDC(pDC);
	m_bmpBk.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());

	m_pbmpOldBk = m_dcBKMem.SelectObject(&m_bmpBk);			
	m_dcBKMem.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, rect1.left, rect1.top, SRCCOPY);
/*
	HANDLE hDib;
	CPalette Pal;
	hDib=DDBToDIB(m_bmpBk,BI_RGB,&Pal);
	WriteDIB("text.bmp",hDib);
*/

}

//DEL void CBitmapListBox::OnSelchange() 
//DEL {
//DEL 	iSelectChange = TRUE;
//DEL 	Invalidate(FALSE);	
//DEL }
