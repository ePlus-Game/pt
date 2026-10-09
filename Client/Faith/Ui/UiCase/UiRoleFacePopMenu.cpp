//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/09/2007 19:05
//      File_base        : KUiHeadToolBar
//      File_ext         : cpp
//      Author           : likun
//      Description      : rolofaceµ¯³ö²Ëµ¥
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "UiRoleFacePopMenu.h"

using namespace CEGUI;

extern iCoreShell* g_pCoreShell;

template<> 
KUiRoleFacePopMenu* KUiWndSingleton<KUiRoleFacePopMenu>::ms_Singleton	= NULL;

KUiRoleFacePopMenu::KUiRoleFacePopMenu(const CEGUI::String& id_name)
: KUiWndSingleton<KUiRoleFacePopMenu>(id_name)
, m_pExpSet(NULL)
, m_pLeaveTeam(NULL)
, m_pCreateRaid(NULL)
, m_pOpenAutoAccept(NULL)
, m_pCloseAutoAccept(NULL)
{
	
}


KUiRoleFacePopMenu::~KUiRoleFacePopMenu()
{

}

void KUiRoleFacePopMenu::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		m_pLeaveTeam   = static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/RoleFaceTeamPopMenu/Leave"));
		m_pExpSet	   = static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/RoleFaceTeamPopMenu/Exp"));
		m_pCreateRaid  = static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/RoleFaceTeamPopMenu/CreateRaid"));
		m_pOpenAutoAccept = static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/RoleFaceTeamPopMenu/OpenAutoAccept"));
		m_pCloseAutoAccept = static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/RoleFaceTeamPopMenu/CloseAutoAccept"));
		ms_Singleton->m_pThisWnd->setRenderMode( false, 3 );
		
		m_pLeaveTeam->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiRoleFacePopMenu::handleLeaveTeamBtn, this));
		m_pExpSet->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiRoleFacePopMenu::handleExpSet, this));
		m_pCreateRaid->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiRoleFacePopMenu::handleCreateRaid, this));
		m_pOpenAutoAccept->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiRoleFacePopMenu::handleOpenAutoAccept, this));
		m_pCloseAutoAccept->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiRoleFacePopMenu::handleCloseAutoAccept, this));
	}
}

void KUiRoleFacePopMenu::Show()
{
	if (ms_Singleton->m_pCreateRaid != NULL)
	{
		KUiPlayerTeam TeamInfo;
		TeamInfo.bAutoAcceptApply = false;
		g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)(&TeamInfo), NULL );
		if (TeamInfo.bTeamLeader)
		{
			ms_Singleton->m_pOpenAutoAccept->enable();
			ms_Singleton->m_pCloseAutoAccept->enable();
			ms_Singleton->m_pCreateRaid->enable();
		}
		else
		{
			ms_Singleton->m_pOpenAutoAccept->disable();
			ms_Singleton->m_pCloseAutoAccept->disable();
			ms_Singleton->m_pCreateRaid->disable();
		}

		KUiWndSingleton<KUiRoleFacePopMenu>::Show();

		if (TeamInfo.bAutoAcceptApply)
		{
			ms_Singleton->m_pCloseAutoAccept->show();
			ms_Singleton->m_pOpenAutoAccept->hide();
		}
		else
		{
			ms_Singleton->m_pOpenAutoAccept->show();
			ms_Singleton->m_pCloseAutoAccept->hide();
		}
	}
	else
	{
		KUiWndSingleton<KUiRoleFacePopMenu>::Show();
	}
}

bool	KUiRoleFacePopMenu::handleExpSet(const CEGUI::EventArgs &args)
{
	return true;
}

bool	KUiRoleFacePopMenu::handleLeaveTeamBtn(const CEGUI::EventArgs &args )
{
	if ( g_pCoreShell != NULL && m_pThisWnd != NULL )
	{
		g_pCoreShell->TeamOperation( TEAM_OI_LEAVE, NULL, NULL );
		m_pThisWnd->hide();
	}
	return true;
}


void	KUiRoleFacePopMenu::SetPosition(Point pos)
{
	ms_Singleton->m_pThisWnd->setPosition(Absolute,pos);
}

bool KUiRoleFacePopMenu::handleCreateRaid(const EventArgs & args)
{
	if ( g_pCoreShell )
	{
		g_pCoreShell->TeamOperation(TEAM_OI_OPEN_BIG_TEAM_MODE, NULL, NULL );
		m_pThisWnd->hide();
	}
	return true;
}

bool KUiRoleFacePopMenu::handleOpenAutoAccept(const EventArgs & args)
{
	if (g_pCoreShell != NULL)
	{
		g_pCoreShell->TeamOperation(TEAM_IO_SET_AUTO_ACCEPT_APPLY, 0, true);
	}
	m_pThisWnd->hide();
	return true;
}

bool KUiRoleFacePopMenu::handleCloseAutoAccept(const EventArgs & args)
{
	if (g_pCoreShell != NULL)
	{
		g_pCoreShell->TeamOperation(TEAM_IO_SET_AUTO_ACCEPT_APPLY, 0, false);
	}
	m_pThisWnd->hide();
	return true;
}
