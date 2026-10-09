//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 04/15/2008 15:51
//      File_base        : TLIEWindow
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////


#include "TLIEWindow.h"
#include <comutil.h>
CComModule _Module; 
#include <string>

using std::string;

const utf8	TLIEWindow::WidgetTypeName[]		= "TaharezLook/IEWindow";
WNDPROC		TLIEWindow::d_oldProc = NULL;


TLIEWindow::TLIEWindow( const String& type, const String& name )
: StaticImage(type, name)
{
	d_hWnd = NULL;
	d_RefreshOnShow = false;
	d_MoveIEWindow = false;
	d_MovedEventSubscribed = false;

	d_ReNavigate = false;
	d_disableInput = false;
	d_hIEWnd = NULL;
	//d_oldProc = NULL;
	d_IEVisible = false;
}

TLIEWindow::~TLIEWindow()
{
}

Window* CEGUI::TLIEWindowFactory::createWindow( const String& name )
{
	return new TLIEWindow(d_type, name);	
}

void CEGUI::TLIEWindow::initialise()
{

	return ;
}

void CEGUI::TLIEWindow::onHidden( WindowEventArgs& e )
{
	StaticImage::onHidden(e);

	if ( !d_IEVisible )
	{
		return;
	}

	d_IEVisible = false;

	if ( d_hWnd != NULL )
	{
 		if ( d_RefreshOnShow )
 		{
 			::ShowWindow( d_hWnd, SW_HIDE );
 		}
 		else
 		{
			::ShowWindow( d_hWnd, SW_HIDE );
			_bstr_t str("about:blank");
			d_pWebBrowser->Navigate( str, NULL, NULL, NULL, NULL );
		}
	}	
}

void CEGUI::TLIEWindow::onShown( WindowEventArgs& e )
{
	StaticImage::onShown(e);

	if ( d_IEVisible )
	{
		return;
	}

	d_IEVisible = true;

	if ( !d_MovedEventSubscribed && d_MoveIEWindow )
	{
		Window* oriParent = this;
		Window* rootWnd = getRoot();
		while ( oriParent->getParent() != rootWnd )
		{
			oriParent = oriParent->getParent();
		}
		
		oriParent->subscribeEvent( 		
			Window::EventMoved,
			Event::Subscriber( &TLIEWindow::onParentMoved, this ) );

		d_MovedEventSubscribed = true;
	}

	if ( d_hWnd == NULL )
	{
		LPOLESTR   pStrBrowserID;   
		StringFromCLSID(IID_IWebBrowser2,&pStrBrowserID);   
		_bstr_t   bstrBrowser(pStrBrowserID);   
		CoTaskMemFree(pStrBrowserID);   

		int nParentX = 0;
		int nParentY = 0;
		Window* pParentWnd = getParent();
		if ( pParentWnd )
		{
			Rect unclippedRect = pParentWnd->getUnclippedPixelRect();
			nParentX = unclippedRect.d_left;
			nParentY = unclippedRect.d_top;

// 			nParentX = pParentWnd->getAbsolutePosition().d_x;
// 			nParentY = pParentWnd->getAbsolutePosition().d_y;
		}

		RECT WinRect;
		WinRect.left = getAbsolutePosition().d_x + nParentX;
		WinRect.top = getAbsolutePosition().d_y + nParentY;
		WinRect.right = WinRect.left + getSize(Absolute).d_width;
		WinRect.bottom = WinRect.top + getSize(Absolute	).d_height;

		d_hWnd = d_IEContainer.Create( 
			g_GetMainHWnd( ), 
			&WinRect, 
			LPCTSTR(bstrBrowser), WS_CHILD | WS_VISIBLE );

		if( d_hWnd != NULL )
		{
			HRESULT hr = d_IEContainer.QueryControl(
				/*__uuidof(IWebBrowser2)*/IID_IWebBrowser2,
				(void**)&d_pWebBrowser);

			if( hr == S_OK )
			{
				_bstr_t str(Utf8ToAnsi(d_text));
				d_pWebBrowser->Navigate( str, NULL, NULL, NULL, NULL );

				d_ReNavigate = false;
				
				if ( d_disableInput && ( NULL == d_oldProc ) )
				{
					beginUpdate();
				}
			}
		}
	}
	else
	{
		if ( d_RefreshOnShow )
		{
			if ( d_ReNavigate )
			{
				::ShowWindow( d_hWnd, SW_SHOW );
				_bstr_t str(Utf8ToAnsi( d_text ));
				d_pWebBrowser->Navigate( str, NULL, NULL, NULL, NULL );

				if ( d_disableInput )
				{
					beginUpdate();
				}

				d_ReNavigate = false;
			}
			else
			{
				::ShowWindow( d_hWnd, SW_SHOW );
				d_pWebBrowser->Refresh();
			}
		}
		else
		{
			::ShowWindow( d_hWnd, SW_SHOW );
			_bstr_t str(Utf8ToAnsi(d_text));
			d_pWebBrowser->Navigate( str, NULL, NULL, NULL, NULL );

			if ( d_disableInput )
			{
				beginUpdate();
			}
		}
	}
}

bool CEGUI::TLIEWindow::onParentMoved( const CEGUI::EventArgs& e )
{
	int nParentX = 0;
	int nParentY = 0;

	Window* oriParent = this;
	Window* rootWnd = getRoot();
	while ( oriParent->getParent() != rootWnd )
	{
		oriParent = oriParent->getParent();
	}

	if ( oriParent )
	{
		Rect unclippedRect = oriParent->getUnclippedPixelRect();
		nParentX = unclippedRect.d_left;
		nParentY = unclippedRect.d_top;
	}
		


// 	Window* pParentWnd = getParent();
// 	if ( pParentWnd )
// 	{
// 		Rect unclippedRect = pParentWnd->getUnclippedPixelRect();
// 		nParentX = unclippedRect.d_left;
// 		nParentY = unclippedRect.d_top;
// 	}
	
	RECT WinRect;
	WinRect.left = getAbsolutePosition().d_x + nParentX;
	WinRect.top = getAbsolutePosition().d_y + nParentY;
	WinRect.right = WinRect.left + getSize(Absolute).d_width;
	WinRect.bottom = WinRect.top + getSize(Absolute	).d_height;

	bool isValid = 
		( WinRect.left >= 0 ) && ( WinRect.left < g_GetScreenWidth() )
		&& ( WinRect.top >= 0 ) && ( WinRect.top <  g_GetScreenHeight() )
		&& ( WinRect.right >= 0 ) && ( WinRect.right < g_GetScreenWidth() )
		&& ( WinRect.bottom >= 0 ) && ( WinRect.bottom < g_GetScreenHeight() );

	if ( isValid )
	{
		Window* pParentWnd = getParent();
		if ( pParentWnd )
		{
			Rect unclippedRect = pParentWnd->getUnclippedPixelRect();
			nParentX = unclippedRect.d_left;
			nParentY = unclippedRect.d_top;
		}
		
		WinRect.left = getAbsolutePosition().d_x + nParentX;
		WinRect.top = getAbsolutePosition().d_y + nParentY;
		WinRect.right = WinRect.left + getSize(Absolute).d_width;
		WinRect.bottom = WinRect.top + getSize(Absolute	).d_height;

		
		MoveWindow( 
			d_hWnd, 
			WinRect.left, 
			WinRect.top, 
			WinRect.right - WinRect.left, 
			WinRect.bottom - WinRect.top, 
			TRUE );
	}

	return true;	
}

BOOL CALLBACK CEGUI::TLIEWindow::WindowProc( HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam )
{
	return DefWindowProc(hwnd, msg, wParam, lParam);		
}

void CEGUI::TLIEWindow::injectTimePulse( DWORD curUpdateTime )
{
	if ( d_pWebBrowser && d_IEVisible/*&& ( NULL == d_oldProc ) */)
	{
		VARIANT_BOOL vb;
		d_pWebBrowser->get_Busy( &vb );
		if ( vb == VARIANT_FALSE )
		{
			d_hIEWnd = GetLastChild( d_hWnd );
			char szIEClassName[128] = { 0 };
			GetClassName( d_hIEWnd, szIEClassName, sizeof( szIEClassName ) );
			if ( strcmp( szIEClassName, "Internet Explorer_Server" ) == 0 )
			{
				if ( GetWindowLong( d_hIEWnd, GWL_WNDPROC ) != ( long )IEWndProc )	//安全保护，防止重复设置窗口过程导致死循环
				{
					d_oldProc =  ( WNDPROC )::SetWindowLong( d_hIEWnd, GWL_WNDPROC, ( long )IEWndProc );
					stopUpdate();
				}
			}
		}
	}
	else
	{
		stopUpdate();
	}
}

LRESULT CALLBACK CEGUI::TLIEWindow::IEWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_RBUTTONDOWN: 
	case WM_RBUTTONUP: 
	case WM_LBUTTONDOWN: 
	case WM_LBUTTONUP: 
	case WM_RBUTTONDBLCLK: 
	case WM_LBUTTONDBLCLK:
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			return 0;
		}
		break;
	default:
		return CallWindowProc(
			d_oldProc,  
			hWnd,
			message,
			wParam,
			lParam );
	}
	
	return CallWindowProc(
		d_oldProc,  
		hWnd,
		message,
		wParam,
		lParam );
}

HWND GetLastChild( HWND hwndParent )
{
	HWND hResultWnd = hwndParent;
	while ( TRUE ) 
	{
		HWND hwndChild = ::GetWindow( hResultWnd, GW_CHILD );
		if ( hwndChild == NULL )
			return hResultWnd;
		hResultWnd = hwndChild;
	}
	return NULL;
}

