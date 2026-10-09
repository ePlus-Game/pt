// IEComCtrlSink.cpp : implementation file
//

#include "stdafx.h"
//#include "IERefreshSampleDlg.h"
#include "IEComCtrlSink.h"
#include <Mshtmdid.h>
#include <Afxctl.h>
#include "shlwapi.h"
#include "AutoUpdateDlg.h"
#include "webbrowser2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIEComCtrlSink

IMPLEMENT_DYNCREATE(CIEComCtrlSink, CCmdTarget)

CIEComCtrlSink::CIEComCtrlSink()
{
	// enable OLE
	EnableAutomation();
	CoInitialize(NULL);

	// member we use to keep track of the event sink to Advise and Unadvise it.
	m_dwCookie = 0;
	// counter to monitor number of requests to BeforeNavigate2 vs number of requests to DownloadBegin.
	m_nPageCounter = 0;
	// counter to monitor number of DownloadBegin and DownloadEnd calls.
	m_nObjCounter = 0;
	// variable to tell us whether a refresh request has started.
	m_bIsRefresh = false;
}

CIEComCtrlSink::~CIEComCtrlSink()
{
	HRESULT hr = 0;
	// kill event sink
	UnAdviseSink();

	// unload OLE
	CoUninitialize();
}

// using MFC CCmdTarget to catch DWebBrowserEvents2 events
BEGIN_MESSAGE_MAP(CIEComCtrlSink, CCmdTarget)
	//{{AFX_MSG_MAP(CIEComCtrlSink)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// these are the events we want to monitor. 
// The events are explained below above their function implementations.
BEGIN_DISPATCH_MAP(CIEComCtrlSink, CCmdTarget)
	DISP_FUNCTION_ID(CIEComCtrlSink, "OnQuit",DISPID_ONQUIT,OnQuit,VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CIEComCtrlSink, "BeforeNavigate2",DISPID_BEFORENAVIGATE2,BeforeNavigate2,
					 VT_EMPTY, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	DISP_FUNCTION_ID(CIEComCtrlSink, "DocumentComplete",DISPID_DOCUMENTCOMPLETE,DocumentComplete,
					 VT_EMPTY, VTS_DISPATCH VTS_PVARIANT)				 
	DISP_FUNCTION_ID(CIEComCtrlSink, "DownloadBegin",DISPID_DOWNLOADBEGIN,DownloadBegin,VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CIEComCtrlSink, "DownloadEnd",DISPID_DOWNLOADCOMPLETE,DownloadEnd,VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

// start capture of DWebBrowserEvents2 events
BOOL CIEComCtrlSink::MyAdviseSink(  CWebBrowser2 *pWebBrowser  )
{
	m_pWebBrowser2 = pWebBrowser;
	if ( m_pWebBrowser2 == NULL )
	{
		return FALSE;
	}
	return TRUE;
}

// stop capture of DWebBrowserEvents2 events
BOOL CIEComCtrlSink::UnAdviseSink()
{
	BOOL bOK = TRUE;
	// kill event sink if cookie is not 0
	if(m_dwCookie != 0)
	{
		LPUNKNOWN pUnkSink = GetIDispatch(FALSE);
		bOK = AfxConnectionUnadvise((LPUNKNOWN)m_pWebBrowser2, DIID_DWebBrowserEvents2, pUnkSink, FALSE, m_dwCookie);
		m_dwCookie = 0;
	}
	return bOK;
}

// browser is quiting so kill events
void CIEComCtrlSink::OnQuit()
{
	UnAdviseSink();	
}

// Fires before a navigation occurs in the given object 
void CIEComCtrlSink::BeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR *url, VARIANT FAR *Flags, 
			VARIANT FAR *TargetFrameName, VARIANT FAR *PostData, VARIANT FAR *Headers, VARIANT_BOOL* Cancel)
{
	int i = i * 0 ;
	return;
	//Lucifer~yu(zhangjianyu) 06/15/2006 Modify
	//Begin-------------------------------------------------------------------
// 	// add page counter so we can compare in DownloadBegin function
// 	m_nPageCounter ++;
// 
// 	// capture relevant information on the URL etc we are about to load.
// 	CString strHeaders = (BSTR)Headers->bstrVal;
// 	CString strUrl = (BSTR)url->bstrVal;	
// 	CString strPostData;
// 
// 	// do we have post data?
// 	if (PostData != NULL && PostData->vt == (VT_VARIANT|VT_BYREF) && PostData->pvarVal->vt != VT_EMPTY )
// 	{		
// 		char *szTemp = NULL, *szPostData = NULL;
// 		long plLbound, plUbound;
// 		
// 		SAFEARRAY *parrTemp = PostData->pvarVal->parray;
// 		SafeArrayAccessData(parrTemp,(void HUGEP **)&szTemp);
// 		
// 		SafeArrayGetLBound(parrTemp , 1, &plLbound);
// 		SafeArrayGetUBound(parrTemp , 1, &plUbound);
// 		
// 		szPostData = new char[plUbound - plLbound + 2];
// 	    StrCpyN(szPostData, szTemp, plUbound - plLbound + 1);
// 		szPostData[plUbound-plLbound] = '\0';
// 		SafeArrayUnaccessData(parrTemp);
// 					
// 		strPostData = szPostData;
// 		delete[] szPostData;
// 	}
// 
// 	// get web browser Dispatch to see if this is the top level URL call
// 	LPDISPATCH lpWBDisp;
// 	m_pWebBrowser2->QueryInterface(IID_IDispatch, (void**)&lpWBDisp);
// 	if (pDisp == lpWBDisp && !m_bIsRefresh)
// 	{
// 		// Top-level Window object, so store URL for a refresh request
// 		m_strHeaders = strHeaders;
// 		m_strUrl = strUrl;
// 		m_strPostData = strPostData;
// 
// 		// have our parent class use this information in some way....
// 		m_pParent->LookUpURL(strUrl,strPostData,strHeaders);			
// 	}
//	lpWBDisp->Release();		
	//End---------------------------------------------------------------------
}

// Fires when the document that is being navigated to reaches the READYSTATE_COMPLETE state
void CIEComCtrlSink::DocumentComplete(IDispatch *pDisp,VARIANT *URL)
{
// 	// decrease page counter
// 	m_nPageCounter --;
// 
// 	CString strURL = (BSTR)URL->bstrVal;
// 
// 	// get web browser Dispatch to see if this is the top level URL call
// 	LPDISPATCH lpWBDisp;
// 	m_pWebBrowser2->QueryInterface(IID_IDispatch, (void**)&lpWBDisp);
// 	if (pDisp == lpWBDisp )
// 	{
// 		//Lucifer~yu(zhangjianyu) 06/15/2006 Modify
// 		//Begin-------------------------------------------------------------------
// 		/*
// 		// Top-level Window object, so document has been loaded
// 		// have our parent class use this information in some way....
// 		m_pParent->DisplayDocComplete(strURL);	
// 		*/
// 		//End---------------------------------------------------------------------
// 
// 	}
//	lpWBDisp->Release();
}

// Fires when a navigation operation is beginning.
void CIEComCtrlSink::DownloadBegin()
{
	//Lucifer~yu(zhangjianyu) 06/15/2006 Modify
	//Begin-------------------------------------------------------------------
	/*
	// if page counter is Zero then we know the Refresh button has been hit...
	if(m_nPageCounter == 0)
	{
		m_bIsRefresh = true;
		// have our parent class use this information in some way....
		m_pParent->LookUpRefreshURL(m_strUrl,m_strPostData,m_strHeaders);	
	}
	// count number of calls to DownloadBegin
	m_nObjCounter ++;
	*/
	//End---------------------------------------------------------------------
}

// Fires when a navigation operation finishes, is halted, or fails.
void CIEComCtrlSink::DownloadEnd()
{
	//Lucifer~yu(zhangjianyu) 06/15/2006 Modify
	//Begin-------------------------------------------------------------------
	/*
	// decrease counter to compare with DownloadBegin
	m_nObjCounter --;

	// if m_nObjCounter is Zero and we are in Refresh mode we know that the refreshed page has loaded.
	if(m_bIsRefresh && m_nObjCounter == 0)
	{
		// have our parent class use this information in some way....
		m_pParent->DisplayDocCompleteRefresh(m_strUrl);	
		m_bIsRefresh = false;
	}
	*/
	//End---------------------------------------------------------------------

}

// load a web page
BOOL CIEComCtrlSink::Navigate2(CString strURL)
{
	HRESULT hr = S_OK;

// 	DWORD dwFlags = 0;
// 	LPCTSTR lpszTargetFrameName = NULL;
// 	LPCTSTR lpszHeaders = NULL;
// 	LPVOID lpvPostData = NULL;
// 	DWORD dwPostDataLen = 0;
// 	COleSafeArray vPostData;
// 
// 	// convert CString to a COleVariant
// 	COleVariant vaURL(strURL);
// 
// 	// we need to add 1 to the counter because we are calling Navigate2()
// 	// therefore BeforeNavigate2() will not be called
// 	m_nPageCounter ++;
// 	m_nObjCounter ++;
// 
//     hr = m_pWebBrowser2->Navigate2(vaURL, COleVariant((long) dwFlags, VT_I4), COleVariant(lpszTargetFrameName, VT_BSTR), 
//                             vPostData, COleVariant(lpszHeaders, VT_BSTR));

	return (hr == S_OK);
}

