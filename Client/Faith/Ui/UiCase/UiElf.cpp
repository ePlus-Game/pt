//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/13/2007 11:15
//      File_base        : UiElf
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 帮助小精灵
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "UiElf.h"
#include "UiQueryWnd.h"
#include "UiSearchHelpWnd.h"
#include "..\KMessageCentre.h"
#include "../UiConfigManager.h"
#include "UiMapCentre.h"
#include "UiNpcNavigation.h"

using namespace CEGUI;

static unsigned int nElfChange = 0;

/************************************************************************/
/*               class KUiElfPopMenu                                    */
/************************************************************************/
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
		m_pThisWnd->getChild("TaharezLook/ElfPopMenu/ShowTask")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiElfPopMenu::handleShowTask, ms_Singleton));
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
	
	if ( (NULL != ms_Singleton) && (NULL != ms_Singleton->m_pThisWnd) )
	{
		char* text = NULL;
		if ( KUiSearchHelpWnd::IsVisible() )
		{
			text = KMessageCentre::GetMessage(elfpop_menu, 2);
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseAll")->setText(AnsiToUtf8(text));
		}
		else
		{
			text = KMessageCentre::GetMessage(elfpop_menu, 1);
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseAll")->setText(AnsiToUtf8(text));
		}
	}


// 	if ( KUiSearchHelpWnd::IsVisible() )
// 	{
// 		text = KMessageCentre::GetMessage(elfpop_menu, 4);
// 		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseSearchHelp")->setText(AnsiToUtf8(text));
// 	}
// 	else
// 	{
// 		text = KMessageCentre::GetMessage(elfpop_menu, 3);
// 		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseSearchHelp")->setText(AnsiToUtf8(text));
// 	}
// 
// 	if ( KUiQueryWnd::IsVisible() )
// 	{
// 		text = KMessageCentre::GetMessage(elfpop_menu, 2);
// 		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseQuery")->setText(AnsiToUtf8(text));
// 	} 
// 	else
// 	{
// 		text = KMessageCentre::GetMessage(elfpop_menu, 1);
// 		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ElfPopMenu/CloseQuery")->setText(AnsiToUtf8(text));
// 	}
}

void KUiElfPopMenu::Hide()
{
	KUiWndSingleton<KUiElfPopMenu>::Hide();
}

bool KUiElfPopMenu::handleCloseSearchHelp( const EventArgs& e )
{
// 	MouseEventArgs* mouse = (MouseEventArgs*)&e;
// 	if ( mouse->button == LeftButton )
// 	{
// 		if ( KUiSearchHelpWnd::IsVisible() )
// 		{
// 			KUiSearchHelpWnd::Hide();
// 		}
// 		else
// 		{
// 			KUiSearchHelpWnd::GetSingleton().ShowSearchHelp();
// 		}
// 		Hide();
// 	}
	KUiNpcNavigation::getSingleton().toggle();
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
		char* text = NULL;
		if ( KUiSearchHelpWnd::IsVisible() )
		{
			KUiSearchHelpWnd::Hide();
			KUiQueryWnd::Hide();
		}
		else
		{
			KUiSearchHelpWnd::Show();
			KUiQueryWnd::Show();
 		}

		Hide();
	}

	return true;
}

bool KUiElfPopMenu::handleShowTask( const EventArgs& e )
{
	KUiSearchHelpWnd::GetSingleton().ShowWhatICanDo();
	return true;
}

int KUiElfPopMenu::GetHeight()
{
	if ( NULL != ms_Singleton && NULL != m_pThisWnd )
	{
		return m_pThisWnd->getHeight(Absolute);
	}
	else
	{
		return 0;
	}
}

int KUiElfPopMenu::GetWidth()
{
	if ( NULL != ms_Singleton && NULL != m_pThisWnd )
	{
		return m_pThisWnd->getWidth(Absolute);
	}
	else
	{
		return 0;
	}
}
/************************************************************************/
/*						 class KUiElf                                   */
/************************************************************************/
template<> 
KUiElf* KUiWndSingleton<KUiElf>::ms_Singleton = NULL;

KUiElf::KUiElf(const CEGUI::String& id_name)
: KUiWndSingleton<KUiElf>( id_name )
, d_state(0)
, d_elfChangeTime(0)
, d_RandomHelpTime(0)
, d_pElf(NULL)
{
	for (int i = 0; i < ELF_STATE_COUNT; ++i)
	{
		char imageName[COMMON_CLIENT_MSG_LEN_32];
		sprintf(imageName, ELF_IMAGE_NAME, i);
		d_elfImageName[i] = imageName;
	}
}

KUiElf::~KUiElf()
{
}

void KUiElf::Init()
{
	d_elfChangeInterval = KUiCfgLoader::getSingleton().getElfCfg().elfChangeInterval;
	d_randomHelpInterval = KUiCfgLoader::getSingleton().getElfCfg().randomHelpInterval;
	d_RandomHelpTime = ::GetTickCount();
	getChild();
}

void KUiElf::getChild()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		// 初始化小精灵各种状态动画
		d_pElf = static_cast<StaticImage*>(m_pThisWnd);
		d_pElf->setCyc(true);
		d_pElf->setZLevel(Window::Top);
		d_pElf->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiElf::handleElfPopMenu, ms_Singleton) );
	}
}

void KUiElf::Show()
{
	KUiWndSingleton<KUiElf>::Show();
	if (ms_Singleton && ms_Singleton->d_pElf)
	{
		ms_Singleton->d_pElf->play();
		ms_Singleton->d_pElf->show();
		ms_Singleton->MoveToRightEdge();
	}
}

void KUiElf::Hide()
{
	KUiWndSingleton<KUiElf>::Hide();
}

void KUiElf::Breathe()
{
	// 改变小精灵状态
	if ( (::GetTickCount() - d_elfChangeTime) > d_elfChangeInterval )
	{
		d_elfChangeTime = ::GetTickCount();
		ChangeElf( g_Random(MAX_FREE_ELF_NUM) );
		//KUiSearchHelpWnd::GetSingleton().ShowSimpleHelp("simplehelp!");
		nElfChange++;
	}
// 
// 	if ( !KUiSearchHelpWnd::GetSingleton().IsSearchVisible() )
// 	{
// 		if ( (::GetTickCount() - d_RandomHelpTime) > d_randomHelpInterval )
// 		{
// 			KUiSearchHelpWnd::GetSingleton().ShowSimpleHelp(KMessageCentre::GetMessage(helpTip_message, g_Random(6)));
// 			d_RandomHelpTime = ::GetTickCount();
// 		}
// 	}
}

bool KUiElf::handleElfPopMenu(const EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	if ( /*arg->button == RightButton*/ true )
	{
		if ( KUiElfPopMenu::IsVisible() )
		{
			KUiElfPopMenu::Hide();
		}
		else
		{
			Point menuPos(
				MouseCursor::getSingleton().getPosition().d_x - KUiElfPopMenu::GetSingleton().GetWidth(),
				MouseCursor::getSingleton().getPosition().d_y - KUiElfPopMenu::GetSingleton().GetHeight());
			
			KUiElfPopMenu::GetSingleton().SetPosition(menuPos);
			KUiElfPopMenu::Show();
		}
	}
	else
	{
		return false;
	}

	return true;
}

void KUiElf::ChangeElf(int i)
{
	d_pElf->setImage(ELF_IMAGESET_NAME, d_elfImageName[i]);

	d_pElf->play();
	d_pElf->show();
}

void KUiElf::SetState(int state)
{
	d_state = state;
	d_elfChangeTime = ::GetTickCount();

	d_pElf->setImage(ELF_IMAGESET_NAME, d_elfImageName[state]);
	d_pElf->play();
	d_pElf->show();
}

void KUiElf::SetElfPos(const Point pos)
{
	if (d_pElf)
	{
		d_pElf->setPosition(pos);
	}
}

void KUiElf::MoveToRightEdge()
{
	if ( NULL != ms_Singleton && NULL != m_pThisWnd )
	{
		int selfWidth = m_pThisWnd->getWidth(Absolute);
		
		int rightEdge_X = g_GetScreenWidth();
		if ( KUiMiniMap::IsVisible() )
		{
			rightEdge_X = KUiMiniMap::GetSingleton().GetMiniMapWndPaintX();	
		}
		int selfPositionY = m_pThisWnd->getYPosition(Absolute);
		Point newPosition(rightEdge_X - selfWidth, selfPositionY);
		m_pThisWnd->setPosition(Absolute, newPosition);
	}
}
/********************************************************************
/*						class: KUiElfBtn
*********************************************************************/
template<>
KUiElfBtn* KUiWndSingleton<KUiElfBtn>::ms_Singleton = NULL;

bool KUiElfBtn::btnOpenElf_MouseClick( const CEGUI::EventArgs& args )
{
	if ( KUiElf::IsVisible() )
	{
		KUiElf::Hide();
	}
	else
	{
		KUiElf::Show();
	}
	return true;
}

void KUiElfBtn::Init()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ElfBtn/btnOpenElf")->subscribeEvent(Window::EventMouseClick, 
			Event::Subscriber(&KUiElfBtn::btnOpenElf_MouseClick, this));
	}	
}

KUiElfBtn::KUiElfBtn(const CEGUI::String& id_name)
: KUiWndSingleton<KUiElfBtn>(id_name)
{
}

KUiElfBtn::~KUiElfBtn()
{
}

void KUiElfBtn::Show()
{
	//
}