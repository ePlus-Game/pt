//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 04/15/2008 15:51
//      File_base        : TLIEWindow
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIIEWINDOW_H__A0D6CCAF_A85C_4B16_93D4_DE2790793FC2__INCLUDED_)
#define AFX_UIIEWINDOW_H__A0D6CCAF_A85C_4B16_93D4_DE2790793FC2__INCLUDED_

#include "TLStatic.h"
#include <exdisp.h>
#include  <atlbase.h> 
extern CComModule _Module;  
#include  <atlwin.h> 
#include "KWin32Wnd.h"

HWND GetLastChild( HWND hwndParent );

namespace CEGUI
{
class TLIEWindow : public StaticImage
{
public:
	TLIEWindow( const String& type, const String& name );
	virtual ~TLIEWindow();

	void initialise();
	void onShown(WindowEventArgs& e);
	void onHidden( WindowEventArgs& e );

	static BOOL CALLBACK WindowProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK IEWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	bool onParentMoved( const CEGUI::EventArgs& e );

	void setMoveIEWindow( bool flag )	{ d_MoveIEWindow = flag; };
	void setRefreshOnShown( bool flag ) { d_RefreshOnShow = flag; };
	void setDisableInput( bool flag )	{ d_disableInput = flag; };
	void setReNavigate( bool flag )	{ d_ReNavigate = flag; };
	
	virtual void injectTimePulse( DWORD curUpdateTime );

public:
	static const utf8	WidgetTypeName[];				//!< The unique typename of this widget
	static WNDPROC		d_oldProc;

private:
	CAxWindow			d_IEContainer;
	CComQIPtr<IWebBrowser2> d_pWebBrowser;
	HWND				d_hWnd;
	bool				d_RefreshOnShow;
	bool				d_MoveIEWindow;
	bool				d_MovedEventSubscribed;

	bool				d_ReNavigate;

	HWND				d_hIEWnd;
	bool				d_disableInput;

	bool				d_IEVisible;
};


class TAHAREZLOOK_API TLIEWindowFactory : public WindowFactory
{
public:
	TLIEWindowFactory(void) : WindowFactory(TLIEWindow::WidgetTypeName) { }
	~TLIEWindowFactory(void){}
public:
	Window*	createWindow(const String& name);
	virtual void	destroyWindow(Window* window)	 { if (window->getType() == d_type) delete window; }
};

}
#endif
