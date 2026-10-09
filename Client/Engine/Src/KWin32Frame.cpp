//-------------------------------------------------------------
//	Purpose   :	 
//	Filename  :	 KWin32Frame.cpp
//	Author    :	 Lucifer~yu (Zhang jian yu)
//	CreateTime:	 05/22/2006
//-------------------------------------------------------------
#include "KWin32.h"
#include "KWin32Wnd.h"
#include "KWin32Frame.h"
#include "KIme.h"
#include "KColors.h"

#ifndef NO_DIRECT_X
	#include "KDDraw.h"
#endif

BOOL			KWin32Frame::m_bMouseInWindow			= FALSE;
BOOL			KWin32Frame::m_bActive					= TRUE;
BOOL			KWin32Frame::m_bShowMouse				= TRUE;
BOOL			KWin32Frame::m_bMultiGame				= TRUE;
unsigned int	KWin32Frame::m_nLastMousePos			= 0;
unsigned int	KWin32Frame::m_uLastMouseStatus			= 0;
unsigned int	KWin32Frame::m_uMouseHoverStartTime		= MOUSE_EVENT_NONE;
unsigned int	KWin32Frame::m_uMouseHoverTimeSetting	= 0;

bool			KWin32Frame::s_minisized	= true;
KWin32Frame::KWin32Frame()
{
}

KWin32Frame::~KWin32Frame()
{

}

BOOL KWin32Frame::RegisterClass( HINSTANCE hInstance, TCHAR* szAppName, KFrameParam& rFrameParam  )
{
	kstrcpy( m_szClass, szAppName );
	kstrcpy( m_szTitle, szAppName );

	WNDCLASS wc;
	
	wc.style			= CS_DBLCLKS;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= NULL;
	wc.cbWndExtra		= NULL;
	wc.hInstance		= hInstance;
	wc.hIcon			= ::LoadIcon( hInstance, MAKEINTATOM( SWORD_ICON ) );
	wc.hCursor			= NULL;
	wc.hbrBackground	= (HBRUSH)::GetStockObject( NULL_BRUSH );
	wc.lpszMenuName 	= NULL;
	wc.lpszClassName	= m_szClass;

	//初始化成员变量
	m_bShowMouse			 = rFrameParam.bShowMouse;
	m_bMultiGame			 = rFrameParam.bMultiGame;
	m_uMouseHoverTimeSetting = rFrameParam.uHoverTime;

	return ::RegisterClass( &wc );
}

HWND KWin32Frame::CreateFrame( HINSTANCE hInstance, int uScreenW, int uScreenH )
{
	RECT r ={0,0, uScreenW, uScreenH};
	AdjustWindowRect(&r, WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX, FALSE);
	HWND hWnd = ::CreateWindowEx(
				WS_EX_APPWINDOW,
				m_szClass,
				m_szTitle,
				WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX,
				NULL,
				NULL,
				r.right - r.left,
				r.bottom - r.top,
				NULL,
				NULL,
				hInstance,		
				NULL);

	if ( !hWnd )
	{
		return NULL;
	}

	return hWnd;
}

int KWin32Frame::HandleInput( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

void KWin32Frame::SetFocus()
{
	return;
}

LRESULT KWin32Frame::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam	)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		if ( KWin32App::m_pMainFrame )
		{
			KWin32App::m_pMainFrame->HandleInput( uMsg, wParam, lParam );
			return true;
		}
		break;
	case WM_DESTROY:
		{
			::PostQuitMessage( 0 );
		}
		break;
	case WM_SETCURSOR:
		if ( m_bShowMouse == FALSE && m_bActive &&	LOWORD(lParam) == HTCLIENT )
		{
			::SetCursor( NULL );
			return TRUE;
		}
        else
        {
			if ( KWin32App::m_pMainFrame )
			{
				KWin32App::m_pMainFrame->HandleInput( uMsg, wParam, lParam );
			}
            return TRUE;
        }
		break;
	case WM_IME_CHAR:
		{
			if ( KWin32App::m_pMainFrame )
			{
				return KWin32App::m_pMainFrame->HandleInput( uMsg, wParam, lParam );
			}
		}
		break;
// 	case WM_INPUTLANGCHANGEREQUEST:
// 	case WM_IME_NOTIFY:
// 		if ( g_pIme && g_pIme->WndMsg( hWnd,uMsg,wParam,lParam ) )
// 		{
// 			return 0;
// 		}
// 		break;
	case WM_ACTIVATEAPP:
		{
			m_bActive = (BOOL)wParam;
#ifndef NO_DIRECT_X
			if ( m_bActive && g_pDirectDraw )
			{
				g_pDirectDraw->RestoreSurface();
			}
#endif
   			if ( KWin32App::m_pMainFrame )
			{
				return KWin32App::m_pMainFrame->HandleInput( uMsg, wParam, lParam );
			}
		}
		break;
    case WM_SYSCOMMAND:
        if ( wParam == SC_KEYMENU )
        {
            return 0;
        }
		break;
	case WM_KILLFOCUS:
		{
			if ( KWin32App::m_pMainFrame )
				return KWin32App::m_pMainFrame->HandleInput( uMsg, wParam, lParam );
		}
		break;
	case WM_SETFOCUS:
		{
			if ( KWin32App::m_pMainFrame )
			{
				KWin32App::m_pMainFrame->SetFocus();
				return KWin32App::m_pMainFrame->HandleInput( uMsg, wParam, lParam );
			}
		}
		break;
	case WM_COPYDATA:
		{
			if ( KWin32App::m_pMainFrame )
			{
				return KWin32App::m_pMainFrame->HandleInput( uMsg, wParam, lParam );
			}
		}
		break;    
	case WM_SIZE:
		{
			if(wParam == 1)
			{
				s_minisized = true;
			}
			else if(wParam == 0)
			{
				s_minisized = false;
			}
		}
	case WM_DISPLAYCHANGE:
    case WM_MOVE:
    case WM_MOVING:
    case WM_SIZING:
    case WM_WINDOWPOSCHANGED:
    case WM_WINDOWPOSCHANGING:
	case WM_MOUSELEAVE:
	case WM_SYSCOLORCHANGE:
		{
			if ( KWin32App::m_pMainFrame )
			{
				KWin32App::m_pMainFrame->HandleInput( uMsg, wParam, lParam );
			}
		}
		break;
	case WM_GETMINMAXINFO:
		{
			MINMAXINFO *pMinmaxinfo			= (MINMAXINFO*)lParam;
			pMinmaxinfo->ptMaxTrackSize.x	= 3000;
			pMinmaxinfo->ptMaxTrackSize.y	= 3000;
		}
		break;
	default:
		if ( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST )
		{
			if ( KWin32App::m_pMainFrame )
			{
				KWin32App::m_pMainFrame->HandleInput( uMsg, wParam, lParam );
			}
		}
		else if ( uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST )
		{
			m_uLastMouseStatus		= wParam;
			m_nLastMousePos			= lParam;
			m_uMouseHoverStartTime	= MOUSE_EVENT_HAPPEND;
			if ( KWin32App::m_pMainFrame )
			{
				KWin32App::m_pMainFrame->HandleInput( uMsg, wParam, lParam );
			}
		}
		else
		{
			//unknown message
		}
		break;
	}
	return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
}

void KWin32Frame::DestroyFrame()
{
	::DestroyWindow( KWin32App::m_hMainWnd );
}

void KWin32Frame::MouseLeaves()
{
    if (m_bMouseInWindow)
    {
        m_bMouseInWindow = false;
        ::ShowCursor(true);
    }
}

void KWin32Frame::GenerateMsgHoverMsg()
{
	if ( m_uMouseHoverStartTime == MOUSE_HOVER_MSG_SENT )
	{
		static char cCounter = 0;
		if ( (++cCounter) == 7 )
		{
			cCounter = 0;
			if ( KWin32App::m_pMainFrame )
			{
				KWin32App::m_pMainFrame->HandleInput( WM_MOUSEHOVER, m_uLastMouseStatus, m_nLastMousePos );
			}
		}
	}
	else if ( m_uMouseHoverStartTime >= MOUSE_HOVER_START_TIME_MIN )
	{
		unsigned int	nCurrentTime = ::timeGetTime();
		if ( ( nCurrentTime - m_uMouseHoverStartTime ) >= m_uMouseHoverTimeSetting )
		{
			if ( KWin32App::m_pMainFrame )
			{
				KWin32App::m_pMainFrame->HandleInput( WM_MOUSEHOVER, m_uLastMouseStatus, m_nLastMousePos );
			}
			m_uMouseHoverStartTime = MOUSE_HOVER_MSG_SENT;
		}
	}
	else if ( m_uMouseHoverStartTime == MOUSE_EVENT_HAPPEND )
	{
		m_uMouseHoverStartTime = MOUSE_EVENT_NONE;
	}
	else if ( m_uMouseHoverTimeSetting )
	{
		m_uMouseHoverStartTime = ::timeGetTime();
		if ( m_uMouseHoverStartTime < MOUSE_HOVER_START_TIME_MIN )
		{
			m_uMouseHoverStartTime = MOUSE_HOVER_START_TIME_MIN;
		}
	}
	else
	{
		//error m_uMouseHoverStartTime.
	}
}

void KWin32Frame::MessageLoop()
{
	MSG	tagMsg;
	BOOL bRet;

	for( ; ; )
	{
		bRet = ::PeekMessage(&tagMsg, NULL, 0, 0, PM_REMOVE);
		if ( bRet )
		{
            if ( tagMsg.message == WM_QUIT )
			{
                return;
			}
			::TranslateMessage( &tagMsg );
			::DispatchMessage( &tagMsg );

			if (!g_IsFullScreen())
			{
				RECT  rcCur;
				GetWindowRect(KWin32App::m_hMainWnd ,&rcCur);
				
				int nDesktopWidth	= ::GetSystemMetrics( SM_CXSCREEN );
				int nDesktopHeight	= ::GetSystemMetrics( SM_CYSCREEN );	
				
				RECT rc;
				rc.left   = 0;
				rc.right  = KWin32App::m_uScreenWidth;
				rc.top    = 0;
				rc.bottom = KWin32App::m_uScreenHeight;
				
				AdjustWindowRectEx(&rc,GetWindowLong(KWin32App::m_hMainWnd, GWL_STYLE),GetMenu(KWin32App::m_hMainWnd) != NULL,GetWindowLong(KWin32App::m_hMainWnd, GWL_EXSTYLE));

				int nMinimalYPos = rcCur.top + ::GetSystemMetrics(SM_CYCAPTION);

				int nFinalWidth = rc.right  - rc.left;
				int nFinalHight = rc.bottom - rc.top;
				
				if (rcCur.right - rcCur.left < nFinalWidth || rcCur.bottom - rcCur.top < nFinalHight || rcCur.top >= nDesktopHeight || rcCur.bottom < 0 || rcCur.left >= nDesktopWidth || rcCur.right < 5 || nMinimalYPos < 0)
				{
					// 居中显示窗口
					::MoveWindow(KWin32App::m_hMainWnd,(nDesktopWidth - nFinalWidth) /2 , (nDesktopHeight - nFinalHight) /2
						,nFinalWidth,nFinalHight,TRUE);
				}//endif
				
			}//endif

		}
 		else if ( m_bActive || m_bMultiGame )
		{
			GenerateMsgHoverMsg( );
			if ( !GameLoop() )
			{
				::DestroyWindow( KWin32App::m_hMainWnd );
			}
		}
		else
		{
			::WaitMessage();
		}
	}
}

BOOL KWin32Frame::GameInit()
{
	return TRUE;
}

BOOL KWin32Frame::GameLoop()
{
	::WaitMessage();
	return TRUE;
}

BOOL KWin32Frame::GameExit()
{
	return TRUE;
}




