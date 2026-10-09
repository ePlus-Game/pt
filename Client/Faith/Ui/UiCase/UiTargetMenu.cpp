//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 04/02/2007 10:52
//      File_base        : UiTargetMenu
//      File_ext         : cpp
//      Author           : likun
//      Description      : Ä¿±ê²Ëµ¥
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "UiTargetMenu.h"
#include "UiTargetEquipment.h"
#include "Coreshell.h"
#include "UiTradeBox.h"
#include "UiChatWindow.h"
#include "UiItemBox.h"
#include "..\KMessageCentre.h"
#include "UiErrorMessageBox.h"
#include "UiTeamList.h"
using namespace CEGUI;

extern iCoreShell*		g_pCoreShell;

template<> 
KUiTargeMenu* KUiWndSingleton<KUiTargeMenu>::ms_Singleton	= NULL;

KUiTargeMenu::KUiTargeMenu( const CEGUI::String& id_name )
: KUiWndSingleton<KUiTargeMenu>( id_name )
, d_pChatBtn(NULL)
, d_pFriendBtn(NULL)
, d_pTradeBtn(NULL)
, d_pFollowBtn(NULL)
, d_pEquipBtn(NULL)
, d_pBlackListBtn(NULL)
, d_pPrentenBtn(NULL)
{}

KUiTargeMenu::~KUiTargeMenu()
{}

void	KUiTargeMenu::Init( void )
{
	InitRegBnt();
}

void	KUiTargeMenu::Show( void )
{
	KUiWndSingleton<KUiTargeMenu>::Show();
}


void	KUiTargeMenu::SetWinPosition( const CEGUI::Point &pos )
{
	ms_Singleton->m_pThisWnd->setPosition(Absolute, pos);
}


void	KUiTargeMenu::SetPlayer( const KUiPlayerItem &player)
{
	if ( g_pCoreShell )
	{
		g_pCoreShell->GetGameData(GDI_GET_PLAYER_INFO_BY_NPCID, (UINT)(&d_targetInfo), player.uId);
	}
}

void	KUiTargeMenu::InitRegBnt( void )
{
	if( ms_Singleton != NULL && ms_Singleton->m_pThisWnd != NULL)
	{
		d_pChatBtn		= static_cast<TLButton *>(m_pThisWnd->getChild( "TaharezLook/TargetMenu/Chat" ));
		d_pFriendBtn	= static_cast<TLButton *>(m_pThisWnd->getChild( "TaharezLook/TargetMenu/Friend" ));
		d_pTradeBtn		= static_cast<TLButton *>(m_pThisWnd->getChild( "TaharezLook/TargetMenu/Trade" ));
		d_pFollowBtn	= static_cast<TLButton *>(m_pThisWnd->getChild( "TaharezLook/TargetMenu/FollowSomeOne" ));
		d_pEquipBtn		= static_cast<TLButton *>(m_pThisWnd->getChild( "TaharezLook/TargetMenu/Equip" ));
		d_pBlackListBtn	= static_cast<TLButton *>(m_pThisWnd->getChild( "TaharezLook/TargetMenu/blackList" ));
		d_pPrentenBtn	= static_cast<TLButton *>(m_pThisWnd->getChild( "TaharezLook/TargetMenu/Prentice" ));

		d_pChatBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiTargeMenu::handleChat, this ));
		d_pFriendBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiTargeMenu::handleFriend, this));
		d_pTradeBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiTargeMenu::handleTrade, this ));
		d_pFollowBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiTargeMenu::handleFollow, this ));
		d_pEquipBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiTargeMenu::handleEquip, this ));
		d_pBlackListBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiTargeMenu::handleBlaceList, this ));
		d_pPrentenBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiTargeMenu::handleGroup, this ));
	}
}

bool	KUiTargeMenu::handleChat( const CEGUI::EventArgs &args )
{
	if (!d_targetInfo.nPrivateState )
	{
		KUiChatInputWnd::GetSingleton().clearText();
		
		KUiChatInputWnd::GetSingleton().write("/");
		
		KUiChatInputWnd::GetSingleton().write(d_targetInfo.strName);
		KUiChatInputWnd::GetSingleton().write(" ");
		
		KUiChatInputWnd::GetSingleton().show();	Hide();
	}//endif

	return true;
}

bool	KUiTargeMenu::handleFriend( const CEGUI::EventArgs &args )
{
	if (!d_targetInfo.nPrivateState )
	{
		g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)(d_targetInfo.strName), CHAT::GROUPID_NONE );
		Hide();
	}//endif
	
	return true;
}

bool	KUiTargeMenu::handleTrade( const CEGUI::EventArgs &args )
{
	if ( g_pCoreShell && d_targetInfo.nIndex != -1 )
	{
		g_pCoreShell->TradeApplyStart(d_targetInfo.nId);
	}
	Hide();
	return true;
}

bool	KUiTargeMenu::handleFollow( const CEGUI::EventArgs &args )
{
	if ( g_pCoreShell && d_targetInfo.nIndex != -1 )
	{
		g_pCoreShell->OperationRequest(GOI_FOLLOW_SOMEONE, (unsigned int)d_targetInfo.nId, NULL );
	}
	Hide();
	return true;
}

bool	KUiTargeMenu::handleEquip( const CEGUI::EventArgs &args )
{
	if (!d_targetInfo.nPrivateState )
	{
		if ( g_pCoreShell && d_targetInfo.nIndex != -1 )
		{
			KUiTargetEquipment::GetSingleton().SetSelectedPlayerID( d_targetInfo.nId );
			g_pCoreShell->OperationRequest(GOI_VIEW_PLAYERITEM, (UINT)d_targetInfo.nId, NULL);
		}
		Hide();
	}//endif
	return true;
}

bool	KUiTargeMenu::handleBlaceList( const CEGUI::EventArgs &args )
{
	if (!d_targetInfo.nPrivateState )
	{
		if ( g_pCoreShell )
		{
			g_pCoreShell->OperationRequest( GOI_CHAT_ADD_BLACK_LIST, (unsigned int)d_targetInfo.strName, NULL );
		}
		Hide();
	}//endif
	return true;
}

bool	KUiTargeMenu::handleGroup( const CEGUI::EventArgs &args )
{
	KUiPlayerItem tagPlayer;
	if ( d_targetInfo.strName )
	{
		strncpy( tagPlayer.Name, d_targetInfo.strName, CLIENT_NAME_AND_TITLE_MAX + 1);
	}
	tagPlayer.nData = 0;
	tagPlayer.nIndex = d_targetInfo.nIndex;
	tagPlayer.nParam = 0;
	tagPlayer.uId = d_targetInfo.nId;


	KUiPlayerTeam	TeamInfo;
	TeamInfo.cNumMember = 0;
	g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
	if ( ((int)(TeamInfo.cNumMember)) <= MAX_TEAMMEMBER_COUNT )
	{
		if (TeamInfo.cNumMember == 0)
		{
			g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
		}

		g_pCoreShell->TeamOperation( TEAM_OI_INVITE, (unsigned int)&tagPlayer, NULL );
	}
	else
	{
		char *msg = KMessageCentre::GetMessage(team_message, 1);
		KUiChannelCentre::GetSingleton().toSysMsg(msg);
	}
	m_pThisWnd->hide();
	return true;
}

float	KUiTargeMenu::GetWinHeight() const
{
	return m_pThisWnd->getSize(Absolute).d_height;
}