//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KWin32Wnd.h.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Window Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#ifndef NO_DIRECT_X
	#include "KDDraw.h"
#endif
#include "KWin32Wnd.h"


//---------------------------------------------------------------------------
// 函数:	GetMainHWnd
// 功能:	取得主窗口句柄
// 参数:	void
// 返回:	主窗口句柄(有可能为NULL)
//---------------------------------------------------------------------------
HWND g_GetMainHWnd(void)
{
	return KWin32App::m_hMainWnd;
}
//---------------------------------------------------------------------------
// 函数:	SetMainHWnd
// 功能:	设置主窗口句柄
// 参数:	hWnd	主窗口句柄
// 返回:	void
//---------------------------------------------------------------------------
void g_SetMainHWnd(HWND hWnd)
{
	KWin32App::m_hMainWnd = hWnd;
}
//---------------------------------------------------------------------------
// 函数:	GetDrawHWnd
// 功能:	取得绘图窗口句柄
// 参数:	void
// 返回:	窗口句柄(有可能为NULL)
//---------------------------------------------------------------------------
HWND g_GetDrawHWnd(void)
{
	return KWin32App::m_hMainDrawWnd;
}
//---------------------------------------------------------------------------
// 函数:	SetDrawHWnd
// 功能:	设置绘图窗口句柄
// 参数:	hWnd	窗口句柄
// 返回:	void
//---------------------------------------------------------------------------
void g_SetDrawHWnd(HWND hWnd)
{
	KWin32App::m_hMainDrawWnd = hWnd;
}
//---------------------------------------------------------------------------
// 函数:	Get Client Rect
// 功能:	取得窗口客户坐标矩形
// 参数:	lpRect	矩形区域
// 返回:	void
//---------------------------------------------------------------------------
void g_GetClientRect(LPRECT lpRect)
{
#ifndef NO_DIRECT_X
	if (g_pDirectDraw->GetScreenMode() == FULLSCREEN)
	{
		lpRect->left = 0;
		lpRect->top = 0;
		lpRect->right = g_pDirectDraw->GetScreenWidth();
		lpRect->bottom = g_pDirectDraw->GetScreenHeight();
	}
	else
#endif
	{
		GetClientRect(KWin32App::m_hMainDrawWnd, lpRect);
	}
}
//---------------------------------------------------------------------------
// 函数:	Client To Screen
// 功能:	客户坐标－屏幕坐标
// 参数:	lpRect
// 返回:	void
//---------------------------------------------------------------------------
void g_ClientToScreen(LPRECT lpRect)
{
#ifndef NO_DIRECT_X
	if (g_pDirectDraw->GetScreenMode() == WINDOWMODE)
#endif
	{
		ClientToScreen(KWin32App::m_hMainDrawWnd, (LPPOINT)lpRect);
		ClientToScreen(KWin32App::m_hMainDrawWnd, (LPPOINT)lpRect + 1);
	}
}
//---------------------------------------------------------------------------
// 函数:	Screen To Client
// 功能:	屏幕坐标－客户坐标
// 参数:	lpRect
// 返回:	void
//---------------------------------------------------------------------------
void g_ScreenToClient(LPRECT lpRect)
{
#ifndef NO_DIRECT_X
	if (g_pDirectDraw->GetScreenMode() == WINDOWMODE)
#endif
	{
		ScreenToClient(KWin32App::m_hMainDrawWnd, (LPPOINT)lpRect);
		ScreenToClient(KWin32App::m_hMainDrawWnd, (LPPOINT)lpRect + 1);
	}
}
//---------------------------------------------------------------------------

bool g_bHighQuality = false;

void   g_SetFontQuality	( bool bHighQuality )
{
	g_bHighQuality = bHighQuality;
}

bool    g_IsHighFontQuality()
{
	return g_bHighQuality;
}

bool g_bShowBorder = false;

void	g_SetFontBorder	( bool bBorder )
{
	g_bShowBorder = bBorder;
}

bool	g_IsFontWithBorder()
{
	return g_bShowBorder;
}

void g_ScreenToClient(LPPOINT lpPoint)
{
#ifndef NO_DIRECT_X
	if (g_pDirectDraw->GetScreenMode() == WINDOWMODE)
#endif
		ScreenToClient(KWin32App::m_hMainDrawWnd, lpPoint);
}

UINT g_GetScreenHeight()
{
	return KWin32App::m_uScreenHeight;
}

UINT g_GetScreenWidth()
{
	return KWin32App::m_uScreenWidth;
}

void g_SetScreenHeight( UINT uHeight )
{
	KWin32App::m_uScreenHeight = uHeight;
}

void g_SetScreenWidth( UINT uWidth )
{
	KWin32App::m_uScreenWidth = uWidth;
}

void g_SetFullScreen( BOOL bFullWindow )
{
	KWin32App::m_bFullScreen = bFullWindow;
}

BOOL g_IsFullScreen()
{
	return KWin32App::m_bFullScreen;
}

KWin32App* g_GetMainApp()
{
	return KWin32App::m_pWin32App;
}

KWin32Frame* g_GetMainWnd()
{
	return KWin32App::m_pMainFrame;
}

void g_SetMainApp( KWin32App* pMainApp )
{
	KWin32App::m_pWin32App = pMainApp;
}
void g_SetMainWnd( KWin32Frame* pMainFrame )
{
	KWin32App::m_pMainFrame = pMainFrame;
}

HINSTANCE g_GethInstance()
{
	return KWin32App::m_hInstance;
}
