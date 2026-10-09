// TransTreeCtrl.cpp : implementation file
//

#include "stdafx.h"/*
#include "autoupdate.h"
//#include "TransTreeCtrl.h"
//#include "ServerList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransTreeCtrl

CTransTreeCtrl::CTransTreeCtrl()
{
	//Font Init
   	LOGFONT logFont;
	memset(&logFont,0,sizeof(LOGFONT));
	logFont.lfWeight=FW_EXTRALIGHT;
	logFont.lfQuality=PROOF_QUALITY;
	sprintf(logFont.lfFaceName,"黑体");
	HFONT m_hBkFont=CreateFontIndirect(&logFont);
    ASSERT(m_hBkFont!=NULL);
	m_BkFont.Attach(m_hBkFont);
	
	m_RootOffsetX = 10;
	m_RootOffsetY = 10;

}

CTransTreeCtrl::~CTransTreeCtrl()
{/*Do Nothing at all*///}
/*
void CTransTreeCtrl::Initialize()
{
	ASSERT(g_ServerList.IsReady());
	for (int iNet=0;iNet<g_ServerList.GetNetNum();iNet++)
	{
		NET_LIST * pList=g_ServerList.GetNetList(iNet);
		HTREEITEM  hNet=InsertItem(pList->m_NetName);
		SetItem(hNet,TVIF_PARAM,0,0,0,0,0,NULL);
		
		for (int iRegion=0;iRegion<pList->m_RegionNum;iRegion++)
		{
			HTREEITEM hRegion=InsertItem(pList->m_RegionInfos[iRegion].GetRegionName(),hNet);
			SetItem(hRegion,TVIF_PARAM,0,0,0,0,0,NULL);
			
			for (int iServer=0;iServer<pList->m_RegionInfos[iRegion].GetServerNum();iServer++)
			{
				
				HTREEITEM hServer=NULL;

				if (pList->m_RegionInfos[iRegion].GetServerInfo(iServer)->GetState()!="")
					hServer = InsertItem(pList->m_RegionInfos[iRegion].GetServerInfo(iServer)->GetName()+"("+pList->m_RegionInfos[iRegion].GetServerInfo(iServer)->GetState()+")",hRegion);
				else
					hServer = InsertItem(pList->m_RegionInfos[iRegion].GetServerInfo(iServer)->GetName(),hRegion);
				
				SetItem(hServer,TVIF_PARAM,0,0,0,0,0,(long)pList->m_RegionInfos[iRegion].GetServerInfo(iServer));
				
				//默认是选择第一个
				if( g_ServerList.GetCurrentSelect()==pList->m_RegionInfos[iRegion].GetServerInfo(iServer) )
					SelectItem(hServer);
				else if(0 == iNet && 0 == iRegion && 0 == iServer)
					SelectItem(hServer);
				
				//默认是选择第一个
				// 			  if (g_ServerList.GetCurrentSelect()==NULL && iNet==0 && iRegion==0 && iServer==0)
				// 			  {
				// 				  g_ServerList.SetCurrentSelect(pList->m_RegionInfos[iRegion].GetServerInfo(iServer));
				// 				  SelectItem(hServer);
				// 			  }//endif iserver==0
				// 			  else
				// 			  {
				// 				  if(g_ServerList.GetCurrentSelect()==pList->m_RegionInfos[iRegion].GetServerInfo(iServer))
				// 				  SelectItem(hServer);
				// 			  }//end else
				
				Expand(hServer,TVE_EXPAND); 
				
			}

		  Expand(hRegion,TVE_EXPAND); 
	  }//end for iRegion

	  Expand(hNet,TVE_EXPAND );
   }//end for i
   
   SetBkImage(IDB_BITMAP_TREEBACK);
   SetTextColor(RGB(0,0,0));
 //SetFont(&m_BkFont);
}

BEGIN_MESSAGE_MAP(CTransTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CTransTreeCtrl)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransTreeCtrl message handlers


BOOL CTransTreeCtrl::SetBkImage(UINT nIDResource)
{
	return SetBkImage( (LPCTSTR)nIDResource );
}

VOID CTransTreeCtrl::SetRootOffsetY(INT iY)
{
	m_RootOffsetY = iY;
}

VOID CTransTreeCtrl::SetRootOffsetX(INT iX)
{
	m_RootOffsetX = iX;
}

BOOL CTransTreeCtrl::SetBkImage(LPCTSTR lpszResourceName)
{
	// If this is not the first call then Delete GDI objects
	if( m_bitmap.m_hObject != NULL )
		m_bitmap.DeleteObject();
	if( m_pal.m_hObject != NULL )
		m_pal.DeleteObject();


	HBITMAP hBmp = (HBITMAP)::LoadImage( AfxGetInstanceHandle(),
			lpszResourceName, IMAGE_BITMAP, 0,0, LR_CREATEDIBSECTION );

	if( hBmp == NULL )
		return FALSE;

	m_bitmap.Attach( hBmp );
	BITMAP bm;
	m_bitmap.GetBitmap( &bm );
	m_cxBitmap = bm.bmWidth;
	m_cyBitmap = bm.bmHeight;


	// Create a logical palette for the bitmap
	DIBSECTION ds;
	BITMAPINFOHEADER &bmInfo = ds.dsBmih;
	m_bitmap.GetObject( sizeof(ds), &ds );

	int nColors = bmInfo.biClrUsed ? bmInfo.biClrUsed : 1 << bmInfo.biBitCount;

	// Create a halftone palette if colors > 256. 
	CClientDC dc(NULL);			// Desktop DC
	if( nColors > 256 )
		m_pal.CreateHalftonePalette( &dc );
	else
	{
		// Create the palette

		RGBQUAD *pRGB = new RGBQUAD[nColors];
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);

		memDC.SelectObject( &m_bitmap );
		::GetDIBColorTable( memDC, 0, nColors, pRGB );

		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;

		for( int i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = pRGB[i].rgbRed;
			pLP->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = pRGB[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}

		m_pal.CreatePalette( pLP );

		delete[] pLP;
		delete[] pRGB;
	}

	return TRUE;
}

void CTransTreeCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here	
	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);
	
	// Create a compatible memory DC 
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );
	
	// Select a compatible bitmap into the memory DC
	CBitmap bitmap, bmpImage;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	
	memDC.SelectObject( &bitmap );
	
	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );
	
	// Draw bitmap in the background if one has been set
	if( m_bitmap.m_hObject != NULL )
	{
		// Now create a mask
		CDC maskDC;
		maskDC.CreateCompatibleDC(&dc);
		CBitmap maskBitmap;
		
		// Create monochrome bitmap for the mask
		maskBitmap.CreateBitmap( rcClient.Width(), rcClient.Height(),
			1, 1, NULL );
		maskDC.SelectObject( &maskBitmap );
		memDC.SetBkColor( ::GetSysColor( COLOR_WINDOW ) );
		
		// Create the mask from the memory DC
		maskDC.BitBlt( 0, 0, rcClient.Width(), rcClient.Height(), &memDC,
			rcClient.left, rcClient.top, SRCCOPY );
		
		
		CDC tempDC;
		tempDC.CreateCompatibleDC(&dc);
		tempDC.SelectObject( &m_bitmap );
		
		CDC imageDC;
		CBitmap bmpImage;
		imageDC.CreateCompatibleDC( &dc );
		bmpImage.CreateCompatibleBitmap( &dc, rcClient.Width(),
			rcClient.Height() );
		imageDC.SelectObject( &bmpImage );
		
		if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE && m_pal.m_hObject != NULL )
		{
			dc.SelectPalette( &m_pal, FALSE );
			dc.RealizePalette();
			
			imageDC.SelectPalette( &m_pal, FALSE );
		}
		
		// Get x and y offset
		CRect rcRoot;
		GetItemRect( GetRootItem(), rcRoot, FALSE );
		rcRoot.left = -GetScrollPos( SB_HORZ ) ;
		
		// Draw bitmap in tiled manner to imageDC
		for( int i = rcRoot.left; i < rcClient.right; i += m_cxBitmap )
			for( int j = rcRoot.top; j < rcClient.bottom; j += m_cyBitmap )
		imageDC.BitBlt( 0, 0, m_cxBitmap, m_cyBitmap, &tempDC,
			0, 0, SRCCOPY );
		
		// Set the background in memDC to black. Using SRCPAINT with black and any other
		// color results in the other color, thus making black the transparent color
		memDC.SetBkColor(RGB(0,0,0));
		memDC.SetTextColor(RGB(255,255,255));
		memDC.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &maskDC,
			rcClip.left, rcClip.top, SRCAND);
		
		// Set the foreground to black. See comment above.
		imageDC.SetBkColor(RGB(255,255,255));
		imageDC.SetTextColor(RGB(0,0,0));
		imageDC.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &maskDC,
			rcClip.left, rcClip.top, SRCAND);
		
		// Combine the foreground with the background
		imageDC.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(),
			&memDC, rcClip.left, rcClip.top,SRCPAINT);
		
		// Draw the final image to the screen		
		dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(),
			&imageDC, rcClip.left, rcClip.top, SRCCOPY );
	}
	else
	{
		dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(),
			rcClip.Height(), &memDC,
			rcClip.left, rcClip.top, SRCCOPY );
	}
	


}

void CTransTreeCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bitmap.m_hObject != NULL )
		InvalidateRect(NULL);
	CTreeCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTransTreeCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bitmap.m_hObject != NULL )
		InvalidateRect(NULL);

	CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CTransTreeCtrl::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if( m_bitmap.m_hObject != NULL )
		InvalidateRect(NULL);
	
	*pResult = 0;
}


BOOL CTransTreeCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bitmap.m_hObject != NULL )
		return TRUE;
    
	return CTreeCtrl::OnEraseBkgnd(pDC);
}

void CTransTreeCtrl::OnClick(LPNMHDR pNMHDR, LRESULT *pResult)
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(&cursorPos);

	CPoint pt(cursorPos);
	unsigned int nFlag;
	HTREEITEM hItem = HitTest(pt, &nFlag);

	if( (NULL != hItem) && (TVHT_ONITEM & nFlag) )
	{
		ExpandOnClick(hItem);

		if( m_bitmap.m_hObject != NULL )
			InvalidateRect(NULL,FALSE);

	}//endif

	*pResult = 0;
}

void CTransTreeCtrl::ExpandOnClick(HTREEITEM hItem)
{
	HTREEITEM hChild=GetChildItem(hItem);

	while (hChild)
	{
		if( ItemHasChildren(hChild) )
			Expand(hChild,TVE_TOGGLE);
		
		hChild=GetNextItem(hChild,TVGN_NEXT);
	}

	Expand(hItem,TVE_TOGGLE);
}

void CTransTreeCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	static TVITEM TempItem;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hITEM=GetSelectedItem();
	TempItem.hItem=hITEM;
	BOOL bSuc=GetItem(&TempItem);
	if (bSuc && TempItem.lParam!=0)  //A Server Is Selected
	{
        CServerInfo * pServer=(CServerInfo * )TempItem.lParam;
		g_ServerList.SetCurrentSelect(pServer);  //Set The CurrentInfomation of Global CServerList
//		GetParent()->PostMessage(WM_NOTIFY_REFRESH_HSTRYLIST);
	}//endif
	else
	{
//         Expand(hITEM,TVE_TOGGLE);
// 		HTREEITEM hChild=GetChildItem(hITEM);
// 		if (hChild)
// 		{
// 			while (hChild)
// 			{
// 				Expand(hChild,TVE_TOGGLE);
// 				hChild=GetNextItem(hChild,TVGN_NEXT);
// 			}//end while
// 		}//endif

	}//end else

	CWnd * pWnd=GetParent();
	//Notify the AutoupdateDlg to Change the Current Selection of ServerListInfomation
    pWnd->PostMessage(WM_USER + 111);  //#define WM_NOTIFY_CUR_SEL WM_USER+111
	*pResult = 0;

}
*/