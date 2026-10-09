
// includes
#include <exdisp.h> //For IWebBrowser2* and others
#include <exdispid.h>


class CAutoUpdateDlg;
class CWebBrowser2;
/////////////////////////////////////////////////////////////////////////////
// CIEComCtrlSink command target

class CIEComCtrlSink : public CCmdTarget
{
	DECLARE_DYNCREATE(CIEComCtrlSink)

	CIEComCtrlSink();           

public:
	virtual ~CIEComCtrlSink();

// properties
	// out parent we want to call functions in
protected:	
	CWebBrowser2*	m_pWebBrowser2;
	// member we use to keep track of the event sink to Advise and Unadvise it.
	DWORD m_dwCookie;	
	// counter to monitor number of requests to BeforeNavigate2 vs number of requests to DownloadBegin.
	int m_nPageCounter;
	// counter to monitor number of DownloadBegin and DownloadEnd calls.
	int m_nObjCounter;
	// variable to tell us whether a refresh request has started.	
	BOOL m_bIsRefresh;

	// member variables to remember the information we get from BeforeNavigate2 
	// but we don't get from DownloadBegin (Refresh) call.
	CString m_strUrl;	
	CString m_strHeaders;
	CString m_strPostData;

// methods
protected:
	// start capture of DWebBrowserEvents2 events
	BOOL MyAdviseSink( CWebBrowser2 *pWebBrowser );
	// stop capture of DWebBrowserEvents2 events
	BOOL UnAdviseSink();
	// load a URL
	BOOL Navigate2(CString strURL);

	// Fires before a navigation occurs in the given object 
	afx_msg void BeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR *url, VARIANT FAR *Flags, VARIANT FAR *TargetFrameName, VARIANT FAR *PostData, VARIANT FAR *Headers, VARIANT_BOOL* Cancel);
	// Fires when the document that is being navigated to reaches the READYSTATE_COMPLETE state
	afx_msg void DocumentComplete(IDispatch *pDisp,VARIANT *URL);
	// Fires when a navigation operation is beginning.
	afx_msg void DownloadBegin();
	// Fires when a navigation operation finishes, is halted, or fails.
	afx_msg void DownloadEnd();
	// browser is quiting so kill events
	afx_msg void OnQuit();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIEComCtrlSink)
	//}}AFX_VIRTUAL
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CIEComCtrlSink)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()

	friend class CAutoUpdateDlg;
};



