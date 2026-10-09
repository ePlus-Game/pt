//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/13/2007 11:15
//      File_base        : UiQueryWnd
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : ²éÑ¯¿ò
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "UiQueryWnd.h"
#include "UiSearchHelpWnd.h"
#include <string>

using namespace CEGUI;

template<> 
KUiQueryWnd* KUiWndSingleton<KUiQueryWnd>::ms_Singleton	= NULL;

KUiQueryWnd::KUiQueryWnd(const CEGUI::String& id_name)
: KUiWndSingleton<KUiQueryWnd>( id_name )
, d_pSearchText(NULL)
, d_pAll(NULL)
, d_pItem(NULL)
, d_pSkill(NULL)
, d_pQuest(NULL)
, d_queryType(faintness_query)
{
}

KUiQueryWnd::~KUiQueryWnd()
{
}

void KUiQueryWnd::Init()
{
	getChild();
}

void KUiQueryWnd::getChild()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->getChild("TaharezLook/QueryWnd/SearchComfirm")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiQueryWnd::handleSearch, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/QueryWnd/Close")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiQueryWnd::handleClose, ms_Singleton));

		d_pSearchText = static_cast<TLEditbox*>(m_pThisWnd->getChild("TaharezLook/QueryWnd/SearchContent"));
		d_pSearchText->subscribeEvent(TLEditbox::EventKeyDown, Event::Subscriber(&KUiQueryWnd::handleKeyDown, ms_Singleton));
		
		d_pAll		= static_cast<TLRadioButton*>( m_pThisWnd->getChild("TaharezLook/QueryWnd/All") );
		d_pItem		= static_cast<TLRadioButton*>( m_pThisWnd->getChild("TaharezLook/QueryWnd/Item") );
		d_pSkill	= static_cast<TLRadioButton*>( m_pThisWnd->getChild("TaharezLook/QueryWnd/Skill") );
		d_pQuest	= static_cast<TLRadioButton*>( m_pThisWnd->getChild("TaharezLook/QueryWnd/Quest") );
		d_pLevel	= static_cast<TLRadioButton*>( m_pThisWnd->getChild("TaharezLook/QueryWnd/Level") );
		d_pNpc		= static_cast<TLRadioButton*>( m_pThisWnd->getChild("TaharezLook/QueryWnd/Npc") );

		d_pAll->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiQueryWnd::handleSelectType, ms_Singleton));
		d_pItem->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiQueryWnd::handleSelectType, ms_Singleton));
		d_pSkill->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiQueryWnd::handleSelectType, ms_Singleton));
		d_pQuest->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiQueryWnd::handleSelectType, ms_Singleton));
		d_pLevel->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiQueryWnd::handleSelectType, ms_Singleton));
		d_pNpc->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiQueryWnd::handleSelectType, ms_Singleton));

		m_pThisWnd->getChild("TaharezLook/QueryWnd/Close")->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiQueryWnd::btnClose_MouseClick, ms_Singleton) );
	}
}

void KUiQueryWnd::Show()
{
	KUiWndSingleton<KUiQueryWnd>::Show();
}

void KUiQueryWnd::Hide()
{
	KUiWndSingleton<KUiQueryWnd>::Hide();
}

bool KUiQueryWnd::handleSearch( const EventArgs& e )
{
	SearchContent	content;
	String searchText = d_pSearchText->getText();
	if ( searchText == "" )
		return false;
	
	content.id = INVALID_CONTENT_ID;
	content.text = searchText;
	content.queryType = d_queryType;
	content.queryResultType = format_string;

	KUiSearchHelpWnd::GetSingleton().QueryRequest( content );

	return true;
}

bool KUiQueryWnd::handleClose( const EventArgs& e )
{
//	Hide();
	return true;
}

bool KUiQueryWnd::handleSelectType( const EventArgs& e )
{
	WindowEventArgs*wargs = (WindowEventArgs *)(&e);
	Window *tmpWnd = wargs->window;
	
	if ( tmpWnd == NULL )
	{
		return false;
	}
	else if ( tmpWnd == d_pAll )
	{
		d_queryType = faintness_query;
	}
	else if ( tmpWnd == d_pQuest )
	{
		d_queryType = quest_query;
	} 
	else if ( tmpWnd == d_pSkill )
	{
		d_queryType = skill_query;
	} 
	else if ( tmpWnd == d_pItem )
	{
		d_queryType = item_query;
	}
	else if ( tmpWnd == d_pLevel )
	{
		d_queryType = player_query;
	}
	else if ( tmpWnd == d_pNpc )
	{
		d_queryType = npc_query;
	}
	
	return true;
}

bool KUiQueryWnd::handleKeyDown( const EventArgs& e )
{
    switch (static_cast<const KeyEventArgs&>(e).scancode)
    {
    case Key::Return:
		{
			handleSearch(e);
		}
		break;
	default:
		return true;
	}
	return true;
}

bool KUiQueryWnd::btnClose_MouseClick( const EventArgs& e )
{
	Hide();
	KUiSearchHelpWnd::GetSingleton().Hide();
	return true;
}
