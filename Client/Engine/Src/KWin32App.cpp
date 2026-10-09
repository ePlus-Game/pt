//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KWin32App.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	KWin32App Class
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KIme.h"
#include "KDebug.h"
#include "KMemBase.h"
#include "KStrBase.h"
#include "KWin32Wnd.h"
#include "KWin32App.h"
#ifndef NO_DIRECT_X
	#include "KDDraw.h"
#endif

FILE*			KWin32App::m_pSout			= NULL;
FILE*			KWin32App::m_pSerr			= NULL;
HINSTANCE		KWin32App::m_hInstance		= NULL;
KWin32App*		KWin32App::m_pWin32App		= NULL;
KWin32Frame*	KWin32App::m_pMainFrame		= NULL	;
HWND			KWin32App::m_hMainWnd		= NULL;
HWND			KWin32App::m_hMainDrawWnd	= NULL;
unsigned int	KWin32App::m_uScreenHeight  = 0;
unsigned int	KWin32App::m_uScreenWidth	= 0;
BOOL			KWin32App::m_bFullScreen	= FALSE;

KWin32App::KWin32App()
{
}

KWin32App::~KWin32App()
{
	if ( KWin32App::m_pMainFrame )
	{
		delete KWin32App::m_pMainFrame;
	}
}

BOOL KWin32App::CreateApplication( KWin32AppParam &rAppParam )
{
	if ( rAppParam.pMainFrame == NULL )
	{
		return FALSE;
	}

	KFrameParam tagFrameParam;
	tagFrameParam.bShowMouse = rAppParam.bShowMouse;
	tagFrameParam.bMultiGame = rAppParam.bMulti;
	tagFrameParam.uHoverTime = rAppParam.uMouseHoverTimeSetting;
	BOOL bOk = rAppParam.pMainFrame->RegisterClass( rAppParam.hInstance, rAppParam.szAppName, tagFrameParam );
	if ( !bOk )
	{
		return FALSE;
	}
	HWND hWnd = rAppParam.pMainFrame->CreateFrame( rAppParam.hInstance, rAppParam.uScreenWidth, rAppParam.uScreenHeight );
	if ( !hWnd )
	{
		return FALSE;
	}

	//初始化所有全局变量
	KWin32App::m_pWin32App		= this;
	KWin32App::m_hMainWnd		= hWnd;
	KWin32App::m_hMainDrawWnd	= hWnd;
	KWin32App::m_hInstance		= rAppParam.hInstance;
	KWin32App::m_pMainFrame		= rAppParam.pMainFrame;
	KWin32App::m_uScreenHeight	= rAppParam.uScreenHeight;
	KWin32App::m_uScreenWidth	= rAppParam.uScreenWidth;
	KWin32App::m_bFullScreen	= rAppParam.bFullScreen;

	MoveWindow(KWin32App::m_hMainWnd,-1024,-768,rAppParam.uScreenWidth,rAppParam.uScreenHeight,true);
	::ShowWindow( hWnd, SW_HIDE );

	/*::SetWindowPos( 
		KWin32App::m_hMainWnd,
		HWND_TOP, 
		-1024, 
		-768,
		rAppParam.uScreenWidth,
		rAppParam.uScreenHeight,
		NULL );//*/
	
	return rAppParam.pMainFrame->GameInit();
}

BOOL KWin32App::RunApplication()
{
	if ( m_pMainFrame == NULL )
	{
		return FALSE;
	}
	// 居中显示窗口
/*	RECT rc;
	::GetWindowRect(KWin32App::m_hMainWnd, &rc);
	RECT crc;
	::GetClientRect(KWin32App::m_hMainWnd, &crc);

	int nWndWidth	= rc.right - rc.left;
	int nWndHeight	= rc.bottom - rc.top;

	int nClientWidth	= crc.right - crc.left;
	int nClientHeight	= crc.bottom - crc.top;

	int w = nWndWidth	- nClientWidth	+ g_GetScreenWidth();
	int h = nWndHeight	- nClientHeight + g_GetScreenHeight();
	
	int nDesktopWidth	= ::GetSystemMetrics( SM_CXSCREEN );
	int nDesktopHeight	= ::GetSystemMetrics( SM_CYSCREEN );			

	::SetWindowPos( 
		KWin32App::m_hMainWnd,
		HWND_TOP, 
		(nDesktopWidth - w) / 2, 
		(nDesktopHeight - h) / 2,
		w, h, NULL );//*/

	RECT rc;
	rc.left = 0;
	rc.right= g_GetScreenWidth();
	rc.top = 0;
	rc.bottom = g_GetScreenHeight();
	AdjustWindowRectEx(&rc,GetWindowLong(g_GetMainHWnd() , GWL_STYLE),
		GetMenu(g_GetMainHWnd()) != NULL,
		GetWindowLong(g_GetMainHWnd(), GWL_EXSTYLE));
	RECT rcDesk;
	GetWindowRect(GetDesktopWindow(),&rcDesk);
	int cx = ((rcDesk.right - rcDesk.left)-g_GetScreenWidth())/2;
	int cy = ((rcDesk.bottom - rcDesk.top) - g_GetScreenHeight())/2;
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	if (!g_IsFullScreen())
		MoveWindow(g_GetMainHWnd(),cx,cy,width,height,true);
	else
		MoveWindow(g_GetMainHWnd(),0,0,g_GetScreenWidth(),g_GetScreenHeight(),true);

	::ShowWindow( KWin32App::m_hMainWnd, SW_SHOWNORMAL );

	m_pMainFrame->MessageLoop();

	return m_pMainFrame->GameExit();
}

void KWin32App::StopApplication()
{
	m_pMainFrame->DestroyFrame();
}



