// UiSysWindow.cpp: implementation of the KUiSysWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "TLSystemWindow.h"
#include "KWin32Wnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define KUI_SYS_WND_CLS		"Embed Window"

// type name for this widget
const utf8	TLSystemWindow::WidgetTypeName[]		= "TaharezLook/SystemWindow";

bool TLSystemWindow::m_windowClassRegistered = false;

TLSystemWindow::TLSystemWindow( const String& type, const String& name )
: StaticImage(type, name)
{

}

TLSystemWindow::~TLSystemWindow()
{
	if ( m_hWnd != NULL )
	{
		::DestroyWindow(m_hWnd);		
	}
}

LRESULT	 CALLBACK TLSystemWindow::SysWndProc(
	HWND hWnd, // handle to window 
	UINT message, // message identifier 
	WPARAM wParam, // first message parameter 
	LPARAM lParam // second message parameter ) 
	)
{
	return DefWindowProc(hWnd, message, wParam, lParam);	
}

void TLSystemWindow::setEnabled( bool setting )
{
	if ( NULL != m_hWnd )
	{
		::EnableWindow(m_hWnd, setting);

		StaticImage::setEnabled( setting );
	}
}

void TLSystemWindow::setPosition( MetricsMode mode, const Point& position )
{
	bool succ = ::SetWindowPos(
		m_hWnd, 
		HWND_TOP, 
		position.d_x,
		position.d_y, 
		getAbsoluteWidth(), 
		getAbsoluteHeight(),
		SWP_SHOWWINDOW);
	if ( succ )
	{
		StaticImage::setPosition(mode, position);
	}
}

void TLSystemWindow::setPosition( int dx, int dy )
{
	Point newPos;
	newPos.d_x = dx;
	newPos.d_y = dy;
	setPosition(Absolute, newPos);
}

void TLSystemWindow::setSize( MetricsMode mode, const Size& size )
{
	bool succ = ::SetWindowPos(
		m_hWnd, 
		HWND_TOP, 
		getAbsolutePosition().d_x,
		getAbsolutePosition().d_y, 
		size.d_width, 
		size.d_height,
		SWP_SHOWWINDOW);
	
	if ( succ )
	{
		StaticImage::setSize( mode, size );
	}
}

void TLSystemWindow::setSize( int width, int height )
{
	Size newSize;
	newSize.d_width = width;
	newSize.d_height = height;
	setSize( Absolute, newSize );
}

void TLSystemWindow::initialise( void )
{
	if ( !m_windowClassRegistered )
	{
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX); 
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)TLSystemWindow::SysWndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= g_GethInstance();
		wcex.hIcon			= NULL;
		wcex.hCursor		= NULL;
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= KUI_SYS_WND_CLS;
		wcex.hIconSm		= NULL;
		
		::RegisterClassEx(&wcex);

		m_windowClassRegistered = true;
	}

	int x = getAbsolutePosition().d_x;
		int y = getAbsolutePosition().d_y;
		int w = getAbsoluteWidth();
		int h = getAbsoluteHeight();

	m_hWnd = ::CreateWindow(
		KUI_SYS_WND_CLS,
		KUI_SYS_WND_CLS, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
		getAbsolutePosition().d_x,
		getAbsolutePosition().d_y,
		getAbsoluteWidth(),
		getAbsoluteHeight(),
		g_GetMainHWnd(),
		NULL,		
		g_GethInstance(), NULL);

	return;
}

Window* CEGUI::TLSystemWindowFactory::createWindow( const String& name )
{
	return new TLSystemWindow(d_type, name);	
}

void CEGUI::TLSystemWindow::onShown( WindowEventArgs& e )
{
	::ShowWindow(m_hWnd, SW_SHOW);
	bool succ = ::SetWindowPos(
		m_hWnd, 
		HWND_TOP, 
		getAbsolutePosition().d_x,
		getAbsolutePosition().d_y, 
		getAbsoluteWidth(), 
		getAbsoluteHeight(),
		SWP_SHOWWINDOW);
	::SetFocus(m_hWnd);
	StaticImage::onShown(e);
}
void CEGUI::TLSystemWindow::onHidden( WindowEventArgs& e )
{
	::ShowWindow(m_hWnd, SW_HIDE);
	::SetFocus(g_GetMainHWnd());
	StaticImage::onHidden(e);
}