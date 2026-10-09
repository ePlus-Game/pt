//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/13/2007 11:15
//      File_base        : UiElfPopMenu
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : °ïÖúÐ¡¾«Áé²Ëµ¥
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32Wnd.h"
#include "GameDataDef.h"
#include "UiElfPopMenu.h"
#include "UiElf.h"
#include "UiQueryWnd.h"
#include "UiSearchHelpWnd.h"
#include "../KMessageCentre.h"

using namespace CEGUI;

template<> 
KUiElfPopMenu* KUiWndSingleton<KUiElfPopMenu>::ms_Singleton	= NULL;

KUiElfPopMenu::KUiElfPopMenu(const CEGUI::String& id_name)
: KUiWndSingleton<KUiElfPopMenu>( id_name )
{
}

KUiElfPopMenu::~KUiElfPopMenu()
{
}

void KUiElfPopMenu::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseQuery")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiElfPopMenu::handleCloseQuery, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseSearchHelp")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiElfPopMenu::handleCloseSearchHelp, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseAll")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiElfPopMenu::handleCloseAll, ms_Singleton));
	}
}

void KUiElfPopMenu::SetPosition(Point pos)
{
	if ( ms_Singleton )
	{
		if ( (pos.d_x+m_pThisWnd->getWidth(Absolute)) > g_GetScreenWidth() )
		{
			pos.d_x = pos.d_x - m_pThisWnd->getWidth(Absolute);
			ms_Singleton->m_pThisWnd->setPosition(Absolute, pos);
		}
		else
		{
			ms_Singleton->m_pThisWnd->setPosition(Absolute, pos);
		}
	}
}

void KUiElfPopMenu::Show()
{
	KUiWndSingleton<KUiElfPopMenu>::Show();
	
	char* text = NULL;
	if ( KUiSearchHelpWnd::IsVisible() )
	{
		text = KMessageCentre::GetMessage(elfpop_menu, 4);
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseSearchHelp")->setText(AnsiToUtf8(text));
	}
	else
	{
		text = KMessageCentre::GetMessage(elfpop_menu, 3);
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseSearchHelp")->setText(AnsiToUtf8(text));
	}
	if ( KUiQueryWnd::IsVisible() )
	{
		text = KMessageCentre::GetMessage(elfpop_menu, 2);
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseQuery")->setText(AnsiToUtf8(text));
	} 
	else
	{
		text = KMessageCentre::GetMessage(elfpop_menu, 1);
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseQuery")->setText(AnsiToUtf8(text));
	}
}

void KUiElfPopMenu::Hide()
{
	KUiWndSingleton<KUiElfPopMenu>::Hide();
}

bool KUiElfPopMenu::handleCloseSearchHelp( const EventArgs& e )
{
	MouseEventArgs* mouse = (MouseEventArgs*)&e;
	if ( mouse->button == LeftButton )
	{
		if ( KUiSearchHelpWnd::IsVisible() )
		{
			KUiSearchHelpWnd::Hide();
		}
		else
		{
			KUiSearchHelpWnd::Show();
		}
		Hide();
	}

	return true;
}

bool KUiElfPopMenu::handleCloseQuery( const EventArgs& e )
{	
	MouseEventArgs* mouse = (MouseEventArgs*)&e;
	if ( mouse->button == LeftButton )
	{
		if ( KUiQueryWnd::IsVisible() )
		{
			KUiQueryWnd::Hide();
		}
		else
		{
			KUiQueryWnd::Show();
		}
		Hide();
	}

	return true;
}

bool KUiElfPopMenu::handleCloseAll( const EventArgs& e )
{
	MouseEventArgs* mouse = (MouseEventArgs*)&e;
	if ( mouse->button == LeftButton )
	{
		KUiElf::Hide();
		KUiSearchHelpWnd::Hide();
		KUiQueryWnd::Hide();
		Hide();
	}

	return true;
}