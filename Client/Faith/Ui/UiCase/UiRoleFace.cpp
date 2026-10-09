//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/14/2006 19:05
//      File_base        : KUiHeadToolBar
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 人物头像现实界面（血条等)
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "UiRoleFace.h"
#include "UiChatCentre.h"
#include "UiStudySkillManage.h"
#include "UiPKFilter.h"
#include "..\KMessageCentre.h"
#include "UiRoleFacePopMenu.h"
#include "UiErrorMessageBox.h"
#include "UiRoleExp.h"
#include "../UiConfigManager.h"

const unsigned short				s_dwPingPerSecond	= 0;


extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiRoleFace* KUiWndSingleton<KUiRoleFace>::ms_Singleton	= NULL;

PK_MODE KUiRoleFace::s_Flag = pk_peace;

KUiRoleFace::KUiRoleFace( const CEGUI::String& id_name )
: KUiWndSingleton<KUiRoleFace>( id_name )
, m_pTeamLeader(NULL)
, m_pRoleName(NULL)
, m_pRoleBlood(NULL)
, m_pRoleMagic(NULL)
, m_pRoleBloodBar(NULL)
, m_pRoleMagicBar(NULL)
, m_pDisplay(NULL)
, m_pDisplayLevel(NULL)
, m_pPkButton(NULL)
, m_sPkName(NULL)
, m_bPKStatus(false)
, m_pPKNotifyImg(NULL)
{

}

KUiRoleFace::~KUiRoleFace()
{

}

void KUiRoleFace::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setZLevel(Window::SuperBottom);
		m_pTeamLeader		= ms_Singleton->m_pThisWnd->getChild("TaharezLook/RoleFace/RoleFaceFront")->getChild("TaharezLook/RoleFace/RoleFaceFront/Leader");
		m_pRoleName			= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/RoleFace/RoleFaceFront" )->getChild( "TaharezLook/RoleFace/RoleFaceFront/RoleName" );
		m_pRoleBlood		= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/RoleFace/RoleBlood" );
		m_pRoleMagic		= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/RoleFace/RoleMagic" );
		m_pRoleBlood->setZLevel(Window::Top);
		m_pRoleMagic->setZLevel(Window::Top);

		m_pRoleBloodBar		= (ProgressBar*)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/RoleFace/RoleBloodProgBar" );
		m_pRoleMagicBar		= (ProgressBar*)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/RoleFace/RoleMagicProgBar" );
		m_pDisplay = ms_Singleton->m_pThisWnd->getChild( "TaharezLook/RoleFace/RoleFaceFront" )->getChild( "TaharezLook/RoleFace/RoleFaceFront/RoleFace" );
		m_pDisplayLevel = ms_Singleton->m_pThisWnd->getChild( "TaharezLook/RoleFace/RoleFaceFront" )->getChild( "TaharezLook/RoleFace/RoleFaceFront/RoleLevel" );
		m_pPkButton = static_cast<TLButton *>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/RoleFace/PK0"));
		m_pPKNotifyImg = static_cast<TLStaticImage*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/RoleFace/Bg"));
		
		m_pThisWnd->subscribeEvent(StaticImage::EventMouseClick, Event::Subscriber(&KUiRoleFace::handlePopMenu, this));
		m_pTeamLeader->subscribeEvent(StaticImage::EventMouseClick, Event::Subscriber(&KUiRoleFace::handlePopMenu, this));
		m_pRoleName->subscribeEvent(StaticImage::EventMouseClick, Event::Subscriber(&KUiRoleFace::handlePopMenu, this));
		m_pDisplay->subscribeEvent(StaticImage::EventMouseClick, Event::Subscriber(&KUiRoleFace::handlePopMenu, this));
		m_pDisplayLevel->subscribeEvent(StaticImage::EventMouseClick, Event::Subscriber(&KUiRoleFace::handlePopMenu, this));
		m_pPkButton->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiRoleFace::handlePkButton, this));
		m_pRoleBlood->subscribeEvent(StaticText::EventMouseClick, Event::Subscriber(&KUiRoleFace::handlePopMenu, this));
		m_pRoleMagic->subscribeEvent(StaticText::EventMouseClick, Event::Subscriber(&KUiRoleFace::handlePopMenu, this));
		m_pRoleBloodBar->subscribeEvent(ProgressBar::EventMouseClick, Event::Subscriber(&KUiRoleFace::handlePopMenu, this));
		m_pRoleMagicBar->subscribeEvent(ProgressBar::EventMouseClick, Event::Subscriber(&KUiRoleFace::handlePopMenu, this));
		
		char PKName[256];
		int	 PKValue = 0;
		g_pCoreShell->GetGameData( GDI_GET_PK_INFO, (unsigned int)PKName, PKValue );
		if ( PKValue >= pk_peace && PKValue < pk_mode_num)
		{
			SetData((PK_MODE)PKValue);
		}

		m_pPKNotifyImg->disable();
		m_pPKNotifyImg->setZLevel(Window::Top);
	}
}

void KUiRoleFace::Show()
{
	KUiWndSingleton<KUiRoleFace>::Show();
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->UpdateData();
	}
}

void KUiRoleFace::Hide()
{
	KUiWndSingleton<KUiRoleFace>::Hide();
}

unsigned int KUiRoleFace::UpdateData( void ) 
{
	int	SkillKindArray[MAX_SKILL_COUNT];
	int nSkillKindCount = g_pCoreShell->GetGameData( GDI_SKILL_KIND_LIST, (unsigned int)SkillKindArray, MAX_SKILL_COUNT );
	KSkillInfo tagSkillInfo;
	memset( &tagSkillInfo, 0, sizeof( tagSkillInfo ) ); 
	
	g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, SkillKindArray[nSkillKindCount - 1] );	
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_INFO, (unsigned int)&ms_Singleton->m_RuntimeInfo, NULL );
	g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&ms_Singleton->m_BaseInfo, NULL );
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&ms_Singleton->m_RuntimeAttribute, NULL );
	g_pCoreShell->TeamOperation( TEAM_OI_GD_INFO, (unsigned int)&(ms_Singleton->m_PlayerTeam), 0);

	if ( ms_Singleton->m_PlayerTeam.nTeamServerID >= 0 && ms_Singleton->m_PlayerTeam.bTeamLeader )
	{
		ms_Singleton->m_pTeamLeader->show();
	}
	else
	{
		ms_Singleton->m_pTeamLeader->hide();
	}
	char szBuf[COMMON_CLIENT_MSG_LEN_32];
	if ( ms_Singleton->m_pRoleName && ms_Singleton->m_pRoleBlood && ms_Singleton->m_pRoleMagic 
		 && ms_Singleton->m_pRoleName && ms_Singleton->m_pRoleBlood && ms_Singleton->m_pRoleMagic
		 && ms_Singleton->m_pPkButton )
	{
		ms_Singleton->m_pRoleName->setText( AnsiToUtf8( ms_Singleton->m_BaseInfo.Name ) );
		sprintf( szBuf, "%d/%d", ms_Singleton->m_RuntimeInfo.nLife, ms_Singleton->m_RuntimeInfo.nLifeFull );
		ms_Singleton->m_pRoleBlood->setText( AnsiToUtf8( szBuf ) );
		ms_Singleton->m_pRoleBloodBar->setProgress( (float)ms_Singleton->m_RuntimeInfo.nLife/ms_Singleton->m_RuntimeInfo.nLifeFull);
		sprintf( szBuf, "%d/%d", ms_Singleton->m_RuntimeInfo.nMana, ms_Singleton->m_RuntimeInfo.nManaFull );
		ms_Singleton->m_pRoleMagic->setText( AnsiToUtf8( szBuf ) );
		ms_Singleton->m_pRoleMagicBar->setProgress( (float)ms_Singleton->m_RuntimeInfo.nMana/ms_Singleton->m_RuntimeInfo.nManaFull);
		KUiRoleExp::GetSingletonPtr()->UpdateDataSkillExp( tagSkillInfo.nSkillPoint, tagSkillInfo.nSkillMaxPoint );
		KUiRoleExp::GetSingletonPtr()->UpdateDataRoleExp( (float)ms_Singleton->m_RuntimeInfo.nExperience, ms_Singleton->m_RuntimeInfo.nExperienceFull );
	}

	String strRoleFaceImageSetName;
	String strRoleFaceImageName;

	KUiCfgLoader& cfg = KUiCfgLoader::getSingleton();
	std::string imageset;
	std::string image;
	if ( !g_pCoreShell->GetGameData( GDI_PLAYER_IS_MALE, NULL, NULL ) )
	{
		cfg.getMidWomanPortraitPath(ms_Singleton->m_BaseInfo.nPortrait, imageset, image );
	}
	else
	{
		cfg.getMidManPortraitPath(ms_Singleton->m_BaseInfo.nPortrait, imageset, image );
	}
	
	strRoleFaceImageSetName = imageset.c_str();
	strRoleFaceImageName = image.c_str();

 	static_cast<StaticImage*>(ms_Singleton->m_pDisplay)->setImage( strRoleFaceImageSetName, strRoleFaceImageName );	
	sprintf( szBuf, "%d", ms_Singleton->m_RuntimeAttribute.nLevel );
	ms_Singleton->m_pDisplayLevel->setText( AnsiToUtf8( szBuf ) );

	return 0;
}

bool	KUiRoleFace::handlePkButton( const EventArgs &args )
{
	if ( !m_bPKStatus && !KUiPKFilter::GetSingleton().IsVisible())
	{
		KUiPKFilter::Show();
		m_bPKStatus = true;
		return true;
	}
	else
	{
		if ( KUiPKFilter::GetSingleton().IsVisible() )
		{
			KUiPKFilter::Hide();
			m_bPKStatus = false;
			return false;
		}
		else
		{
			KUiPKFilter::Show();
			m_bPKStatus = true;
			return true;
		}
	}
}

void	KUiRoleFace::SetData( PK_MODE pkFlag )
{
	char *status = NULL;
	switch( pkFlag )
	{
		case pk_peace:
			{
				status = KMessageCentre::GetMessage( pk_status_message, 1);
				break;
			}
		case pk_team:
			{
				status = KMessageCentre::GetMessage( pk_status_message, 2);
				break;
			}
		case pk_gens:
			{
				status = KMessageCentre::GetMessage( pk_status_message, 3);
				break;
			}
		case pk_friendevil:
			{
				status = KMessageCentre::GetMessage( pk_status_message, 4);
				break;
			}
		case pk_whole:
			{
				status = KMessageCentre::GetMessage( pk_status_message, 5);
				break;
			}
		case pk_guard:
			{
				status = KMessageCentre::GetMessage( pk_status_message, 6);
				break;
			}
		case pk_tong:
			{
				status = KMessageCentre::GetMessage( pk_status_message, 7);
				break;
			}
		case pk_league:
			{
				status = KMessageCentre::GetMessage( pk_status_message, 8);
				break;
			}
		default:
				break;
	}
	if(status != NULL)
	{
		ms_Singleton->m_pPkButton->setText(AnsiToUtf8(status));
	}
	s_Flag = pkFlag;
}



bool	KUiRoleFace::handlePopMenu(const CEGUI::EventArgs &args )
{
	MouseEventArgs* pArgs = (MouseEventArgs*)&args;
	if ( pArgs->button == RightButton )
	{
		KUiPlayerTeam TeamInfo;
		g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)(&TeamInfo), NULL );
		if (TeamInfo.nTeamServerID >= 0)
		{
			MouseEventArgs* pArgs = (MouseEventArgs*)&args;
			Point parentP = m_pThisWnd->getPosition(Absolute);
			Point pos = MouseCursor::getSingleton().getPosition();
			if ( pArgs && pArgs->button == RightButton )
			{
				KUiRoleFacePopMenu::SetPosition(pos - parentP);
				KUiRoleFacePopMenu::Show();	
			}
		}
	}
	return false;
}

Rect	KUiRoleFace::getArea()
{
	return m_pThisWnd->getUnclippedPixelRect();	
}

void	KUiRoleFace::PlayerAttackNotify()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd && m_pPKNotifyImg)
	{
		m_pPKNotifyImg->setCycCount(3);
		if (!m_pPKNotifyImg->isPlaying())
		{
			m_pPKNotifyImg->play(true);
		}
	}
}