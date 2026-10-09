//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/16/2006 15:18
//      File_base        : UiGame
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "KWin32Wnd.h"
#include "UiGameSpace.h"
#include "coreshell.h"
#include "../ShortcutKey.h"
#include "UiMapCentre.h"
#include "..\UiAdapter.h"
#include "CEGUISystem.h"
#include "UiDragItem.h"
#include "UiChangeMapWnd.h"

#ifndef GET_X_LPARAM
	#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
	#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

#define LB_AUTO_RUN_TICKCOUNT 3000

extern iCoreShell*			g_pCoreShell;
KUiGameSpace				g_WndGameSpace;

KUiGameSpace::KUiGameSpace( void )
{
	d_lButtonDownBeginTick = 0;
	d_lButtonDownTickCount = 0;
}

KUiGameSpace::~KUiGameSpace( void )
{
	
}

/************************************************************************/
/*				Handle the game space message                           */
/************************************************************************/
int KUiGameSpace::handleInput( unsigned int uMsg, unsigned int uParam, int nParam )
{
	if (GetKeyState(VK_MENU) & 0x8000)
		KShortcutKeyCentre::ms_bAltPressed = true;
	else
		KShortcutKeyCentre::ms_bAltPressed = false;

	switch( uMsg )
	{
    case WM_KILLFOCUS:
		{
			// 停止音乐
			//if ( g_pCoreShell )
			//	g_pCoreShell->SetMusic(false);
		}
		break;
	case WM_SETFOCUS:
		{
			// 开始音乐
			//if ( g_pCoreShell )
			//	g_pCoreShell->SetMusic(true);
		}
		break;
	case WM_KEYDOWN:
		{
			if ( uParam == VK_SHIFT)
			{
				BOOL bShowObjName = g_pCoreShell->GetGameData( GDI_SHOW_ITEMS_NAME, NULL, NULL );
				g_pCoreShell->OperationRequest( GOI_SHOW_GAMESPACE_ITEM_NAME, TRUE, !bShowObjName );
			}
			
			if ( uMsg == WM_KEYDOWN || (uMsg == WM_KEYUP && uParam == VK_SNAPSHOT) )
			{
				if (LOWORD(nParam) == 0)
				{
					return 0;
				}
				int nModifier = 0;
				if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					nModifier |= HOTKEYF_CONTROL;
				}
				if (GetKeyState(VK_SHIFT) & 0x8000)
				{
					nModifier |= HOTKEYF_SHIFT;
				}
				if (GetKeyState(VK_MENU) & 0x8000)
				{
					nModifier |= HOTKEYF_ALT;
				}
				KShortcutKeyCentre::HandleKeyInput(uParam, nModifier);
			}
		}
		break;
	case WM_KEYUP:
		{
			if ( uMsg == WM_KEYDOWN || (uMsg == WM_KEYUP && uParam == VK_SNAPSHOT) )
			{
				if (LOWORD(nParam) == 0)
				{
					return 0;
				}
				int nModifier = 0;
				if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					nModifier |= HOTKEYF_CONTROL;
				}
				if (GetKeyState(VK_SHIFT) & 0x8000)
				{
					nModifier |= HOTKEYF_SHIFT;
				}
				if (GetKeyState(VK_MENU) & 0x8000)
				{
					nModifier |= HOTKEYF_ALT;
				}
				KShortcutKeyCentre::HandleKeyInput(uParam, nModifier);
			}
		}
		break;
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		{
			if (uMsg == WM_SYSKEYDOWN || (uMsg == WM_SYSKEYUP && uParam == VK_MENU))
			{
				int nModifier = 0;
				if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					nModifier |= HOTKEYF_CONTROL;
				}
				if (GetKeyState(VK_SHIFT) & 0x8000)
				{
					nModifier |= HOTKEYF_SHIFT;
				}
				if (GetKeyState(VK_MENU) & 0x8000)
				{
					nModifier |= HOTKEYF_ALT;
				}
				KShortcutKeyCentre::HandleKeyInput( uParam, nModifier );
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int nModifier = 0;
			if (uParam & MK_CONTROL)
				nModifier |= HOTKEYF_CONTROL;

			if (uParam & MK_SHIFT)
				nModifier |= HOTKEYF_SHIFT;

			if (GetKeyState(VK_MENU) & 0x8000)
			{
				nModifier |= HOTKEYF_ALT;
				if (g_pCoreShell)
				{
					g_pCoreShell->GotoWhere( GET_X_LPARAM(nParam),  GET_Y_LPARAM(nParam), 0 );
					g_pCoreShell->DrawMovePosition( GET_X_LPARAM(nParam),  GET_Y_LPARAM(nParam) );
					closeUiWnd(GAMESPACE_CLICKED);
				}
			}
			else
			{
				KShortcutKeyCentre::HandleMouseInput(uMsg == WM_LBUTTONDOWN ? VK_LBUTTON : VK_RBUTTON, nModifier, GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));//*/
			}
			
		}
		break;	
	case WM_RBUTTONDOWN:
		{
			if ( KShortcutKeyCentre::ms_nAutoRunMode == KShortcutKeyCentre::enAutoRun )
			{
				d_lButtonDownBeginTick = 0;
				d_lButtonDownTickCount = 0;
				KShortcutKeyCentre::ms_nAutoRunMode = KShortcutKeyCentre::enNone;
			}
			else
			{
				d_lButtonDownBeginTick = ::GetTickCount();
				d_lButtonDownTickCount = 0;
			}

			int nModifier = 0;
			if (uParam & MK_CONTROL)
				nModifier |= HOTKEYF_CONTROL;

			if (uParam & MK_SHIFT)
				nModifier |= HOTKEYF_SHIFT;

			if (GetKeyState(VK_MENU) & 0x8000)
				nModifier |= HOTKEYF_ALT;

			KShortcutKeyCentre::HandleMouseInput(uMsg == WM_LBUTTONDOWN ? VK_LBUTTON : VK_RBUTTON, nModifier, GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		}
		break;	
	case WM_MOUSEMOVE:
	case WM_MOUSEHOVER:
		{
			bool bLButtonDown = false;
			if ( (uParam & MK_LBUTTON ) && g_pCoreShell->GetGameData(GDI_IS_STALL, -1, 0) == 0)
			{
				bLButtonDown = true;
				if (d_lButtonDownTickCount >= LB_AUTO_RUN_TICKCOUNT)
				{
					d_lButtonDownBeginTick = 0;
					d_lButtonDownTickCount = 0;
					KShortcutKeyCentre::ms_nAutoRunMode = KShortcutKeyCentre::enAutoRun;
				}
				else if (d_lButtonDownTickCount < LB_AUTO_RUN_TICKCOUNT)
				{
					d_lButtonDownTickCount = ::GetTickCount();
					KShortcutKeyCentre::ms_nAutoRunMode = KShortcutKeyCentre::enNone;
				}
			}
			else 
			{
				if ( KShortcutKeyCentre::ms_nAutoRunMode != KShortcutKeyCentre::enAutoRun )
				{
					KShortcutKeyCentre::ms_nAutoRunMode = KShortcutKeyCentre::enNone;
					KShortcutKeyCentre::ms_bLBtnPressed = false;
					KShortcutKeyCentre::ms_nLBtnPressedCounter = 0;
				}
				
				if (uParam & MK_RBUTTON)
				{
					KShortcutKeyCentre::HandleMouseInput(0, VK_RBUTTON, GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
				}
			}
			KShortcutKeyCentre::MouseMove( GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam), bLButtonDown);
		}
		break;
	case WM_MOUSEWHEEL:
		if (g_pCoreShell)
		{
			d_lButtonDownBeginTick = 0;
			d_lButtonDownTickCount = 0;
			KShortcutKeyCentre::ms_nAutoRunMode = KShortcutKeyCentre::enNone;

			int zDelta = GET_Y_LPARAM(uParam);
			int nPos = (-zDelta / WHEEL_DELTA);
			if (nPos > 0)
				g_pCoreShell->Turn(0);
			else if (nPos < 0)
				g_pCoreShell->Turn(1);
		}
		break;
	}
	return 0;
}

/************************************************************************/
/*				Paint the game space                                    */
/************************************************************************/
void KUiGameSpace::PaintWindow()
{
	g_pCoreShell->DrawGameSpace();
	/*
	if ( KShortcutKeyCentre::ms_nAutoRunMode == KShortcutKeyCentre::enAutoRun )
	{
		if (g_pCoreShell)
		{
			g_pCoreShell->OperationRequest( GOI_SET_FOLLOW_ATTACK, false, NULL );
			POINT tagPoint;
			::GetCursorPos( &tagPoint );
			g_pCoreShell->GotoWhere(tagPoint.x, tagPoint.y, 0);
		}
	}//*/
}
void KUiGameSpace::PaintUiEffect()
{
	g_pCoreShell->DrawUiEffect();
}
void KUiGameSpace::BreatheWindow( void )
{
	g_pCoreShell->BreatheGameSpace();	
}

