// BitmapDialog.cpp : implementation file
//
// Created by David Forrester, January 1, 2001
// Feel free to use this code in any way you want.

#include "stdafx.h"
#include "BitmapDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CBitmapDialog::Constructor(LPCTSTR lpszResourceName, UINT nIDResource,
								   LPCTSTR lpszFilename, CBitmap *pBitmap)
{
	m_bTransparent = FALSE;			// No transparency
	m_bStaticTransparent = TRUE;	// Static controls are transparent
	m_bBitmapCreated = FALSE;		// Don't automatically release the bitmap
	m_bBitmapExists = FALSE;		// IS there a bitmap?
	m_bClickAnywhereMove = FALSE;	// Clicking anywhere moves
	
	// Create a hollow brush used in making static controls transparent
	m_brushHollow = (HBRUSH) GetStockObject (HOLLOW_BRUSH);

	// Load a bitmap from a resource name
	if (lpszResourceName != NULL)
	{
		LoadBitmap (lpszResourceName, NULL);
	}
	// Load a bitmap from a resource ID
	else if (nIDResource != 0)
	{
		SetBitmap (nIDResource);
	}
	// Use the passed bitmap
	else if (pBitmap != 0)
	{
		// NOTE: The user is responsible for handling the bitmap
		SetBitmap (pBitmap);
	}
	// else: No bitmap has been created yet

	m_nBitmapW = 0;
	m_nBitmapH = 0;


	//Resize the dialog
	m_LayOutStyle = LO_RESIZE;	

}

CBitmapDialog::CBitmapDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd,
							  LPCTSTR lpszResourceName, UINT nIDResource,
							  LPCTSTR lpszFilename, CBitmap *pBitmap)
	: CDialog(lpszTemplateName, pParentWnd)
{
	Constructor(lpszResourceName, nIDResource, lpszFilename, pBitmap);
}

CBitmapDialog::CBitmapDialog(UINT nIDTemplate, CWnd* pParentWnd,
							  LPCTSTR lpszResourceName, UINT nIDResource,
							  LPCTSTR lpszFilename, CBitmap *pBitmap)
	: CDialog(nIDTemplate, pParentWnd)
{
	Constructor(lpszResourceName, nIDResource, lpszFilename, pBitmap);
}

CBitmapDialog::CBitmapDialog(LPCTSTR lpszResourceName, UINT nIDResource,
							  LPCTSTR lpszFilename, CBitmap *pBitmap)
	: CDialog()
{
	Constructor(lpszResourceName, nIDResource, lpszFilename, pBitmap);
}

BOOL CBitmapDialog::LoadBitmap(LPCTSTR lpszResourceName, LPCTSTR lpszFilename)
{
	ASSERT(lpszResourceName && lpszFilename);

	// Release the bitmap if it was created
	ReleaseBitmap();

	if (lpszResourceName != NULL)
	{
		// Load the bitmap from a resource
		m_bmBitmap = new CBitmap;
		if (!m_bmBitmap->LoadBitmap(lpszResourceName))
			return FALSE;

		// Automatically delete the object
		m_bBitmapCreated = TRUE;
		m_bBitmapExists = TRUE;
	}
	else
	{
		// Load the bitmap from a file
		HBITMAP hbm = (HBITMAP) LoadImage(NULL, lpszFilename, IMAGE_BITMAP, 0, 0,
			LR_LOADFROMFILE|LR_CREATEDIBSECTION);
		if (hbm == NULL) return FALSE;

		// Get the CBitmap object
		CopyBitmapFrom(CBitmap::FromHandle(hbm));
	}

	return TRUE;
}

BOOL CBitmapDialog::SetBitmap(UINT nIDResource)
{
	// Release the bitmap if it was created
	ReleaseBitmap();
	
	// Load the bitmap
	m_bmBitmap = new CBitmap;
	if (!m_bmBitmap->LoadBitmap(nIDResource))
		return FALSE;

	// Automatically delete the object
	m_bBitmapCreated = TRUE;
	m_bBitmapExists = TRUE;

	// Make the window transparent if needed
	//MakeWindowRgn ();

	return TRUE;
}

BOOL CBitmapDialog::CopyBitmapFrom(CBitmap *pBitmap)
{
	ASSERT(pBitmap);

	// Release the bitmap if it was created
	ReleaseBitmap();

	// Get the bitmap information
	BITMAP bmSrc;
	pBitmap->GetBitmap(&bmSrc);

	// Get a DC to the source bitmap
	CDC dcSrc;
	dcSrc.CreateCompatibleDC(NULL);
	CBitmap *bmSrcOld = dcSrc.SelectObject (pBitmap);

	// Create a new bitmap
	m_bmBitmap = new CBitmap;
	if (!m_bmBitmap->CreateCompatibleBitmap(&dcSrc, bmSrc.bmWidth, bmSrc.bmHeight))
		return FALSE;

	// Get a DC to the destination bitmap
	CDC dcDst;
	dcDst.CreateCompatibleDC (NULL);
	CBitmap *bmDstOld = dcDst.SelectObject (m_bmBitmap);

	// Copy the bitmap
	dcDst.BitBlt(0, 0, bmSrc.bmWidth, bmSrc.bmHeight, &dcSrc, 0, 0, SRCCOPY);

	// Release
	dcSrc.SelectObject(bmSrcOld);
	dcDst.SelectObject(bmDstOld);
	dcSrc.DeleteDC();
	dcDst.DeleteDC();

	// Automatically delete the object
	m_bBitmapCreated = TRUE;
	m_bBitmapExists = TRUE;

	return TRUE;
}

void CBitmapDialog::SetBitmap(CBitmap *pBitmap)
{
	// Release the bitmap if it was created
	ReleaseBitmap();

	// Set the bitmap
	m_bmBitmap = pBitmap;

	// The bitmap exists, but was not created
	m_bBitmapExists = TRUE;
}

void CBitmapDialog::ReleaseBitmap()
{
	// Make sure that the bitmap was created using LoadBitmap or CopyBitmapFrom
	if (m_bBitmapCreated)
	{
		// Delete the bitmap
		m_bmBitmap->DeleteObject();
		delete m_bmBitmap;

		// The bitmap has not been created yet
		m_bBitmapCreated = FALSE;
	}

	m_bBitmapExists = FALSE;
}

void CBitmapDialog::SetTransparent (BOOL bTransparent)
{
	m_bTransparent = bTransparent;
	MakeWindowRgn ();
}

void CBitmapDialog::SetTransparentColor(COLORREF col)
{
	m_colTrans = col;
}

//----------------------------------------------------------------
// CBitmapDialog :: MakeWindowRgn - makes a window region from
//  the bitmap and uses it if on transparent mode.

void CBitmapDialog::MakeWindowRgn()
{
	if (!m_bTransparent)
	{
		// Set the window region to the full window
		CRect rc;
		GetWindowRect(rc);
		CRgn rgn;
		rgn.CreateRectRgn(0, 0, rc.Width(), rc.Height());
		SetWindowRgn(rgn, TRUE);
	}
	else
	{
		MakeBitmapRgn();
	}
}

//目前只用到LO_STRETCH和LO_RESIZE两种情况
void CBitmapDialog::GetDialogSize(int nDiagW, int nDiagH,
						int nBitmapW, int nBitmapH,
						int& nW, int& nH)
{
	switch (m_LayOutStyle)
	{
	case LO_DEFAULT:
		nW = min(nDiagW, nBitmapW);
		nH = min(nDiagH, nBitmapH);
		break;
	case LO_TILE:
		nW = max(nDiagW, nBitmapW);
		nH = max(nDiagH, nBitmapH);
		break;
	case LO_CENTER:
	case LO_STRETCH:
		nW = nDiagW;
		nH = nDiagH;		
		break;
	case LO_RESIZE:
		nW = nBitmapW;
		nH = nBitmapH;
		break;
	default:
		break;
	}
}

void CBitmapDialog::MakeBitmapRgn()
{
	// Set the region to the window rect minus the client rect
	CRect rcWnd;
	GetWindowRect(rcWnd);

	CRgn rgn;
	rgn.CreateRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom);

	CRect rcClient;
	GetClientRect(rcClient);
	
	CRgn rgnClient;
	rgnClient.CreateRectRgn(rcWnd.left, rcWnd.top, rcWnd.right,
		rcWnd.bottom);

	// Subtract rgnClient from rgn
	rgn.CombineRgn(&rgn, &rgnClient, RGN_XOR);

	// Get a DC for the bitmap
	CDC dcImage;
	dcImage.CreateCompatibleDC(NULL);
	CBitmap *pOldBitmap = dcImage.SelectObject(m_bmBitmap);

	// Get the bitmap for width and height information
	BITMAP bm;
	m_bmBitmap->GetBitmap(&bm);
	m_nBitmapW = bm.bmWidth;
	m_nBitmapH = bm.bmHeight;

	int width;
	int height;
	GetDialogSize(rcClient.Width(), rcClient.Height(), m_nBitmapW, m_nBitmapH, width, height);
	
	RECT rect = {0, 0, width, height};
	MoveWindow(&rect);
	CenterWindow();

	// Use RLE (run-length) style because it goes faster.
	// Row start is where the first opaque pixel is found.  Once
	// a transparent pixel is found, a line region is created.
	// Then row_start becomes the next opaque pixel.
	int row_start;

	// Go through all rows
	for (int y=0; y<height; y++)
	{
		// Start looking at the beginning
		row_start = 0;

		// Go through all columns
		for (int x=0; x<width; x++)
		{
			// If this pixel is transparent
			if (dcImage.GetPixel(x, y) == m_colTrans)
			{
				// If we haven't found an opaque pixel yet, keep searching
				if (row_start == x) row_start ++;
				else
				{
					// We have found the start (row_start) and end (x) of
					// an opaque line.  Add it to the region.
					CRgn rgnAdd;
					rgnAdd.CreateRectRgn(rcClient.left+row_start,
						rcClient.top+y, rcClient.left+x, rcClient.top+y+1);
					rgn.CombineRgn(&rgn, &rgnAdd, RGN_OR);
					row_start = x+1;
				}
			}
		}

		// If the last pixel is still opaque, make a region.
		if (row_start != x)
		{
			CRgn rgnAdd;
			rgnAdd.CreateRectRgn(rcClient.left+row_start, rcClient.top+y,
				rcClient.left+x, rcClient.top+y+1);
			rgn.CombineRgn(&rgn, &rgnAdd, RGN_OR);
		}
	}
	
	SetWindowRgn(rgn, TRUE);
}

BEGIN_MESSAGE_MAP(CBitmapDialog, CDialog)
	//{{AFX_MSG_MAP(CBitmapDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


BOOL CBitmapDialog::OnEraseBkgnd(CDC *pDC)
{
	// If no bitmap is loaded, behave like a normal dialog box
	if (!m_bBitmapExists)
		return CDialog::OnEraseBkgnd(pDC);

	// Get the client rectangle of the window
	CRect rect;
	GetClientRect(rect);

	// Get a DC for the bitmap
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *pOldBitmap = dc.SelectObject (m_bmBitmap);

	if (m_LayOutStyle == LO_DEFAULT || m_LayOutStyle == LO_RESIZE)
    {
        pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
    }
    else if (m_LayOutStyle == LO_TILE)
    {
        int ixOrg, iyOrg;

        for (iyOrg = 0; iyOrg < rect.Height(); iyOrg += m_nBitmapH)
        {
            for (ixOrg = 0; ixOrg < rect.Width(); ixOrg += m_nBitmapW)
            {
                pDC->BitBlt (ixOrg, iyOrg, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
            }
        }
    }
    else if (m_LayOutStyle == LO_CENTER)
    {
        int ixOrg = (rect.Width() - m_nBitmapW) / 2;
        int iyOrg = (rect.Height() - m_nBitmapH) / 2;
        
        pDC->BitBlt(ixOrg, iyOrg, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
    }
    else if (m_LayOutStyle == LO_STRETCH)
    {
        pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, m_nBitmapW, m_nBitmapH, SRCCOPY);
    }
    
   	// Release
	dc.SelectObject(pOldBitmap);
	dc.DeleteDC();

	OnPostEraseBkgnd(pDC);

	// Return value: Nonzero if it erases the background.
	return TRUE;
}

//主要在子类里面处理该函数，以便子窗口容易处理透明
void CBitmapDialog::OnPostEraseBkgnd(CDC* pDC)
{

}

//----------------------------------------------------------------
// CBitmapDialog :: OnCtlColor - set the colors for controls

HBRUSH CBitmapDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here

	// Make static controls transparent
	if (m_bStaticTransparent && nCtlColor == CTLCOLOR_STATIC)
	{
		// Make sure that it's not a slider control
		char lpszClassName[256];
		GetClassName (pWnd->m_hWnd, lpszClassName, 255);
		if (strcmp (lpszClassName, TRACKBAR_CLASS) == 0)
			return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

		pDC->SetBkMode(TRANSPARENT);
		return m_brushHollow;
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

UINT CBitmapDialog::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    if (m_bClickAnywhereMove)
		return HTCAPTION;
	else
		return CDialog::OnNcHitTest(point);
}
