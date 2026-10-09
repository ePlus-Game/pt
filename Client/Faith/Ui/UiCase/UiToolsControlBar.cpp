/////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/10/2006 11:46
//      File_base        : UiToolsControlBar
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "UiToolsControlBar.h"
#include "UiChatCentre.h"
#include "UiMailCentre.h"
#include "UiStudySkillManage.h"
#include "UiPKFilter.h"
#include "UiQuestManage.h"
#include "UiItemBox.h"
#include "UiEquipment.h"
#include "UiTongManager.h"
#include "UiCastBar.h"
#include "UiHelpInfo.h"
#include "UiMapCentre.h"
#include "UiGameSetting.h"
#include "UiItemTip.h"
#include "..\KMessageCentre.h"
#include "UiSearchHelpWnd.h"
#include "../UiAdapter.h"
#include "UiChatWindow.h"
#include "UiESCDlg.h"
#include "UiTalisman.h"
#include "UiTeamViewer.h"
#include "UiFSBible.h"
#include "UiStudySkillManage.h"
#include "UiTongRecruitCentre.h"
extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

const float		g_Lag		= 400.0f;
// 限制显示延迟大小
const int		g_MaxLagTime[5]	= { 1768, 1691, 1836, 2023, 1953 };

const int BreathTrequency = 1000;

/************************************************************************/
/*                                                                      */
/************************************************************************/
unsigned int KUiMiniNaviation::FriendMgrID = UI_FRIEND_MGR;

template<> 
KUiMiniNaviation* KUiWndSingleton<KUiMiniNaviation>::ms_Singleton	= NULL;

KUiMiniNaviation::KUiMiniNaviation( const CEGUI::String& id_name )
: KUiWndSingleton<KUiMiniNaviation>( id_name )
{
	// Do events wire-up
//	m_pThisWnd->getChild(FriendMgrID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiMiniNaviation::handleFriend, this));
}

KUiMiniNaviation::~KUiMiniNaviation()
{

}

bool KUiMiniNaviation::handleFriend( const CEGUI::EventArgs& args )
{

	KUiAdapter::SetMouseRes( MOUSE_CURSOR_ADDFRIEND );
	setMiniNavMouseStatus( MOUSE_FRIEND_STATUS );
	char *message = KMessageCentre::GetMessage( minitoolbar_message, BIND_ADD_FRIEND_MESSAGE);
	KUiChannelCentre::GetSingleton().toSysMsg(message);
    return true;
}

void KUiMiniNaviation::Init( void )
{
	if ( ms_Singleton != NULL && ms_Singleton->m_pThisWnd != NULL )
	{
		m_pThisWnd->setZLevel(Window::Bottom);
		d_pInfoBtn		= reinterpret_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/MiniNavigation/Info"));
		d_pTradeBtn		= reinterpret_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/MiniNavigation/Coin"));
		d_pFriendBtn	= reinterpret_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/MiniNavigation/Friend"));
		d_pEquipmentBtn	= reinterpret_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/MiniNavigation/Eye"));
		d_pTeamBtn		= reinterpret_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/MiniNavigation/Unnamed0"));
		d_pChatBtn		= reinterpret_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/MiniNavigation/Unnamed1"));
		d_pFollowBtn	= reinterpret_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/MiniNavigation/Unnamed2"));

		d_pInfoBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiMiniNaviation::handleBlack, this ));
		d_pTradeBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiMiniNaviation::handleTrade, this ));
		d_pFriendBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiMiniNaviation::handleFriend, this ));
		d_pEquipmentBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiMiniNaviation::handleLookEquipment, this ));
		d_pTeamBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiMiniNaviation::handleTeam, this ));
		d_pChatBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiMiniNaviation::handleChat, this ));
		d_pFollowBtn->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiMiniNaviation::handleFollow, this ));
	}
}

//拖入黑名单
bool	KUiMiniNaviation::handleBlack( const CEGUI::EventArgs &args )
{
	/*	
	KUiAdapter::SetMouseRes(MOUSE_CURSOR_SCREEN);
	setMiniNavMouseStatus( MOUSE_BLACK_LIST );
	char *message = KMessageCentre::GetMessage( minitoolbar_message, BIND_SCREEN_MESSAGE);
	KUiChannelCentre::GetSingleton().toSysMsg(message);
	//*/

	KUiTeamViewer::ToggleVisibility();
	
	return true;
}

//和别的玩家交易
bool	KUiMiniNaviation::handleTrade( const CEGUI::EventArgs &args )
{
	KUiAdapter::SetMouseRes(MOUSE_CURSOR_TRADE);
	setMiniNavMouseStatus( MOUSE_TRADE_STATUS );
	char *message = KMessageCentre::GetMessage( minitoolbar_message, BIND_TRADE_MESSAGE);
	KUiChannelCentre::GetSingleton().toSysMsg(message);
	return true;
}


//查看某玩家的装备
bool	KUiMiniNaviation::handleLookEquipment( const CEGUI::EventArgs &args )
{
	KUiAdapter::SetMouseRes(MOUSE_CURSOR_VIEW);
	setMiniNavMouseStatus( MOUSE_LOOK_EQUIPMENT );
	char *message = KMessageCentre::GetMessage( minitoolbar_message, BIND_LOOK_EQUIPMENT);
	KUiChannelCentre::GetSingleton().toSysMsg(message);
	return true;
}

//组队
bool	KUiMiniNaviation::handleTeam( const CEGUI::EventArgs& args )
{
	KUiAdapter::SetMouseRes(MOUSE_CURSOR_TEAM);
	setMiniNavMouseStatus( MOUSE_TEAM_STATUS );
	char *message = KMessageCentre::GetMessage( minitoolbar_message, BIND_TEAM_MESSAGE);
	KUiChannelCentre::GetSingleton().toSysMsg(message);
	return true;
}

//私聊
bool	KUiMiniNaviation::handleChat( const CEGUI::EventArgs &args )
{
	KUiAdapter::SetMouseRes( MOUSE_CURSOR_CHAT );
	setMiniNavMouseStatus( MOUSE_CHAT_STATUS );
	char *message = KMessageCentre::GetMessage( minitoolbar_message, BIND_CHAT_MESSAGE);
	KUiChannelCentre::GetSingleton().toSysMsg(message);
	return true;
}

//跟随
bool	KUiMiniNaviation::handleFollow( const CEGUI::EventArgs &args )
{
	KUiAdapter::SetMouseRes( MOUSE_CURSOR_FOLLOW );
	setMiniNavMouseStatus( MOUSE_FOLLOW_STATUS );
	char *message = KMessageCentre::GetMessage( minitoolbar_message, BIND_FOLLOW_MESSAGE);
	KUiChannelCentre::GetSingleton().toSysMsg(message);
	return true;
}

MOUSE_CURRENT_STATUS	KUiMiniNaviation::getMiniNavMouseSatatus( void )
{
	return d_miniMouseStatus;
}

void	KUiMiniNaviation::setMiniNavMouseStatus( MOUSE_CURRENT_STATUS mouseStatus )
{
	d_miniMouseStatus = mouseStatus;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/

//unsigned int KUiNaviation::FriendMgrID = UI_FRIEND_MGR;

template<> 
KUiNaviation* KUiWndSingleton<KUiNaviation>::ms_Singleton	= NULL;

KUiNaviation::KUiNaviation( const CEGUI::String& id_name )
: KUiWndSingleton<KUiNaviation>( id_name )
, pImageFastNetSpeed( NULL )
, pImageNormalNetSpeed( NULL )
, pImageSlowNetSpeed( NULL )
//, m_bTongStatus(false)
{
	ms_Singleton->m_dwPing = 0;
	ZeroMemory( m_szNetInfo, MAX_TEXT_LEN );
	m_currentSec = 0;
	m_skillButton = NULL;
	m_skillButtonAnimation = NULL;
	PlayTime = 1000;;
	ZeroMemory( m_skillKindArray, MAX_SKILL_COUNT * sizeof( int ) );
	ZeroMemory( m_skillArray, MAX_SKILL_COUNT * sizeof( int ) );
	m_isOpenAnimation = false;
	m_uncheckItem = 0;
}

KUiNaviation::~KUiNaviation()
{
	pImageFastNetSpeed = NULL;
	pImageNormalNetSpeed = NULL;
	pImageSlowNetSpeed = NULL;
	m_skillButton = NULL;
	m_skillButtonAnimation = NULL;
}

void KUiNaviation::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->setZLevel(Window::Bottom);
		// Do events wire-up
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/ItemMgr")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviation::handleItem, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_JS_1")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviation::handleRole, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_JS_0")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviation::handleRole, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_DS_1")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviation::handleRole, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_DS_0")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviation::handleRole, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_YR_1")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviation::handleRole, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_YR_0")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviation::handleRole, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/SkillMgr")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviation::handleSkillManage, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/TalismanMgr")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviation::handleTalisman, ms_Singleton));
		
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/FastNetSpeed")->subscribeEvent(StaticImage::EventMouseEnters, Event::Subscriber(&KUiNaviation::handleDisplayNetInfo, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/NormalNetSpeed")->subscribeEvent(StaticImage::EventMouseEnters, Event::Subscriber(&KUiNaviation::handleDisplayNetInfo, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/SlowNetSpeed")->subscribeEvent(StaticImage::EventMouseEnters, Event::Subscriber(&KUiNaviation::handleDisplayNetInfo, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/FastNetSpeed")->subscribeEvent(StaticImage::EventMouseLeaves, Event::Subscriber(&KUiNaviation::handleCloseNetInfo, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/NormalNetSpeed")->subscribeEvent(StaticImage::EventMouseLeaves, Event::Subscriber(&KUiNaviation::handleCloseNetInfo, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/SlowNetSpeed")->subscribeEvent(StaticImage::EventMouseLeaves, Event::Subscriber(&KUiNaviation::handleCloseNetInfo, ms_Singleton));

		pImageFastNetSpeed		= ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/FastNetSpeed");
		pImageNormalNetSpeed	= ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/NormalNetSpeed");
		pImageSlowNetSpeed		= ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/SlowNetSpeed");	

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_JS_1")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_JS_0")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_DS_1")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_DS_0")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_YR_1")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_YR_0")->hide();
		m_skillButton = static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/SkillMgr"));
		m_skillButtonAnimation = static_cast<TLStaticImage*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/SkillMgr")->getChild("TaharezLook/Navigation/SkillMgr/Animation"));
		m_skillButtonAnimation->subscribeEvent( RadioButton::EventMouseClick, Event::Subscriber(&KUiNaviation::handleAimationDown, ms_Singleton));
		PlayTime		= KUiCfgLoader::getSingleton().getPlayTime();
		m_isOpenAnimation = KUiCfgLoader::getSingleton().getAnimationOpenStatus();
		m_uncheckItem =  KUiCfgLoader::getSingleton().getUnCheckSkill();
	}
}

void KUiNaviation::Show( void )
{
	KUiWndSingleton<KUiNaviation>::Show();

	g_pCoreShell->GetGameData( GDI_PLAYER_RT_INFO, (unsigned int)&ms_Singleton->m_RuntimeInfo, NULL );
	g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&ms_Singleton->m_BaseInfo, NULL );
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&ms_Singleton->m_RuntimeAttribute, NULL );
	
	String strRoleFaceImageName;
	switch ( ms_Singleton->m_RuntimeAttribute.nSeries )  
	{
	case 0:
		if ( g_pCoreShell->GetGameData( GDI_PLAYER_IS_MALE, NULL, NULL ) )
		{
			strRoleFaceImageName = "_JS_1";
		}
		else
		{
			strRoleFaceImageName = "_JS_0";
		}
		break;
	case 1:
		if ( g_pCoreShell->GetGameData( GDI_PLAYER_IS_MALE, NULL, NULL ) )
		{
			strRoleFaceImageName = "_DS_1";
		}
		else
		{
			strRoleFaceImageName = "_DS_0";
		}
		break;
	case 2:
		if ( g_pCoreShell->GetGameData( GDI_PLAYER_IS_MALE, NULL, NULL ))
		{
			strRoleFaceImageName = "_YR_1";
		}
		else
		{
			strRoleFaceImageName = "_YR_0";
		}
		break;
	}	
	ms_Singleton->m_pThisWnd->getChild(AnsiToUtf8("TaharezLook/Navigation/RoleMgr") + strRoleFaceImageName)->show(); 
		
	KUiWndSingleton<KUiNaviation>::Show();
}

void KUiNaviation::Hide( void )
{
	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_JS_1")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_JS_0")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_DS_1")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_DS_0")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_YR_1")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_YR_0")->hide();
		KUiWndSingleton<KUiNaviation>::Hide();
	}
}

/*
bool KUiNaviation::handleFriend( const CEGUI::EventArgs& args )
{
	if(KUiChatCentre::GetSingleton().IsVisible() == false)
	{
		if(KUiSceneMap::getSinglton().isVisible())
		{
			KUiSceneMap::getSinglton().hide();
		}
		KUiChatCentre::Show();
	}
	else
		KUiChatCentre::Hide();
	return true;
}

bool KUiNaviation::handleSkillManage( const CEGUI::EventArgs& args )
{
	if(KUiSkillManage::GetSingleton().IsVisible() == false)
	{
		if(KUiSceneMap::getSinglton().isVisible())
		{
			KUiSceneMap::getSinglton().hide();
		}
		KUiSkillManage::Show();
	}
	else
		KUiSkillManage::Hide();
	return true;
}

bool KUiNaviation::handlePKFilter( const CEGUI::EventArgs& args )
{
	if(KUiPKFilter::GetSingleton().IsVisible() == false)
		KUiPKFilter::Show();
	else
		KUiPKFilter::Hide();
	return true;
}
/*
bool	KUiNaviation::handleQuest( const CEGUI::EventArgs& args	)
{
	if(KUiQuestManage::IsVisible() == false)
	{
		if(KUiSceneMap::getSinglton().isVisible())
		{
			KUiSceneMap::getSinglton().hide();
		}
		KUiQuestManage::GetSingleton().show();
	}
	else
		KUiQuestManage::GetSingleton().hide();
	return true;
}//*/

bool	KUiNaviation::handleItem( const CEGUI::EventArgs& args	)
{
	if(KUiItemBox::getSingleton().isVisible() == false)
	{
		if(KUiSceneMap::getSinglton().isVisible())
		{
			KUiSceneMap::getSinglton().hide();
		}
		KUiItemBox::getSingleton().show();
	}
	else
		KUiItemBox::getSingleton().hide();

	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/ItemMgr"))->deactivateImage();

	return true;
}

bool	KUiNaviation::handleRole( const CEGUI::EventArgs& args	)
{
// 	KUiSceneMap::Show();
// 	return true;

	if(KUiEquipment::GetSingleton().IsVisible() == false)
	{
		if(KUiSceneMap::getSinglton().isVisible())
		{
			KUiSceneMap::getSinglton().hide();
		}
		KUiEquipment::Show();
	}
	else
		KUiEquipment::Hide();

	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_JS_1"))->deactivateImage();
	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_JS_0"))->deactivateImage();
	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_DS_1"))->deactivateImage();
	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_DS_0"))->deactivateImage();
	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_YR_1"))->deactivateImage();
	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/RoleMgr_YR_0"))->deactivateImage();

	return true;
}

bool KUiNaviation::handleTalisman( const CEGUI::EventArgs& args )
{
	// 暂时屏蔽法宝功能
	return true;
	if(KUiTalisman::getSingleton().isVisible() == false)
	{
		if(KUiSceneMap::getSinglton().isVisible())
		{
			KUiSceneMap::getSinglton().hide();
		}
		KUiTalisman::getSingleton().show();
	}
	else
	{
		KUiTalisman::getSingleton().hide();
	}

	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/TalismanMgr"))->deactivateImage();
		
	return true;
}

bool KUiNaviation::handleSkillManage( const CEGUI::EventArgs& args )
{
	if(KUiStudySkillManage::GetSingleton().IsVisible() == false)
	{
		if(KUiSceneMap::getSinglton().isVisible())
		{
			KUiSceneMap::getSinglton().hide();
		}
		KUiStudySkillManage::Show( true );
	}
	else
		KUiStudySkillManage::Hide();

	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/SkillMgr"))->deactivateImage();
	return true;
}

/*
bool	KUiNaviation::handTong( const CEGUI::EventArgs& args )
{
	SocietyInfoIndex tagSocietyIdx;
	tagSocietyIdx.TemplateId	= enSUTplId_Tong;
	int nTopLayer = 0;
	g_pCoreShell->GetGameData( GDI_GET_SOCIETY_PLAYER, (unsigned int)&tagSocietyIdx, (int)&nTopLayer );
	if ( nTopLayer > 0 )
	{
		//暂时这样处理，没有找到更好的方法
		if ( !m_bTongStatus )
		{
			m_bTongStatus = true;
			KUiTongManager::Show();
		}
		else
		{
			if(KUiTongManager::GetSingleton().IsVisible() == false)
			{
				if(KUiSceneMap::getSinglton().isVisible())
				{
					KUiSceneMap::getSinglton().hide();
				}
				KUiTongManager::Show();
			}
			else
				KUiTongManager::Hide();
		}
		return true;
	}
	else
	{
		char *message = KMessageCentre::GetMessage(tong_operation_message, 24);
		KUiChannelCentre::GetSingleton().toSysMsg(message);
		return false;
	}
}//*/

// LSL
void KUiNaviation::UpdateNetInfo(DWORD nPing)
{
	int i = g_Random(5);
	
	m_dwPing = g_MaxLagTime[i] > nPing ? nPing : g_MaxLagTime[i];

	if ( pImageFastNetSpeed && pImageNormalNetSpeed && pImageSlowNetSpeed )
	{
		int pingstate = (int)( (float)m_dwPing/g_Lag);
		
		//为了防止窗口刷新导致同级窗口以及父窗口随着一起刷新，在之前判断是否需要更新图片
		int static oldPingstate = 1000;
		if(oldPingstate == pingstate)
		{
			return;
		}
		oldPingstate = pingstate;
		pImageFastNetSpeed->hide();		
		pImageNormalNetSpeed->hide();	
		pImageSlowNetSpeed->hide();
		switch( pingstate )
		{
		case 0:
			{
				pImageFastNetSpeed->show();
			}
			break;
		case 1:
			{
				pImageNormalNetSpeed->show();
			}
			break;
		case 2:
			{
				pImageSlowNetSpeed->show();
			}
			break;
		default:
			{
				pImageSlowNetSpeed->show();
			}
			break;
		}
	}

}

void KUiNaviation::PickUpObject()
{
	static_cast<TLButton*>( ms_Singleton->m_pThisWnd->getChild("TaharezLook/Navigation/ItemMgr") )->activeImage();
}

void KUiNaviation::ActiveButton( int index )
{
	String str("TaharezLook/Navigation/");
	String temp;

	switch(index)
	{
	case 0:
		{
			temp = "RoleMgr";
			String strRoleFaceImageName;
			switch ( ms_Singleton->m_RuntimeAttribute.nSeries )  
			{
			case 0:
				if ( g_pCoreShell->GetGameData( GDI_PLAYER_IS_MALE, NULL, NULL ) )
				{
					strRoleFaceImageName = "_JS_1";
				}
				else
				{
					strRoleFaceImageName = "_JS_0";
				}
				break;
			case 1:
				if ( g_pCoreShell->GetGameData( GDI_PLAYER_IS_MALE, NULL, NULL ) )
				{
					strRoleFaceImageName = "_DS_1";
				}
				else
				{
					strRoleFaceImageName = "_DS_0";
				}
				break;
			case 2:
				if ( g_pCoreShell->GetGameData( GDI_PLAYER_IS_MALE, NULL, NULL ))
				{
					strRoleFaceImageName = "_YR_1";
				}
				else
				{
					strRoleFaceImageName = "_YR_0";
				}
				break;
			}	
			temp = temp+strRoleFaceImageName;
		}
		break;
	case 1:
		temp = "ItemMgr";
		break;
	case 2:
		temp = "SkillMgr";
	    break;
	case 3:
		temp = "FriendMgr";
	    break;
	default:
	    return;
	}
	
	str = str+temp;
	static_cast<TLButton*>( ms_Singleton->m_pThisWnd->getChild(str) )->activeImage(10);

}

bool KUiNaviation::IsNewAbility()
{
	KUiPlayerAttribute playerInfo;
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, ( unsigned int )&playerInfo , 0 );
	KSkillInfo tagSkillInfo;
	int  nSkillKindCount = 0;
	int	 skillCount = 0;
	nSkillKindCount = g_pCoreShell->GetGameData( GDI_SKILL_KIND_LIST, (unsigned int)m_skillKindArray, MAX_SKILL_COUNT );

	for ( int i = 0; i < nSkillKindCount; ++i )
	{
		if( i == m_uncheckItem - 1 )
			continue;
		int nParam = 0;
		nParam |= MAX_SKILL_COUNT << 16;
		nParam |= m_skillKindArray[i]; 
		skillCount = g_pCoreShell->GetGameData( GDI_SKILL_LIST, (unsigned int)m_skillArray, nParam );
		for ( int t = 0; t < skillCount; ++t )
		{
			g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, m_skillArray[t] );
			if ( tagSkillInfo.pSkillCond && playerInfo.nLevel >= tagSkillInfo.pSkillCond->nPlayerLvl )
			{
				return true;
			}
		}
	}
	return false; 
}
void KUiNaviation::ShowAimationOfSkillBtn( bool show /*= false*/  )
{
	if ( !m_skillButton || !m_skillButtonAnimation )
		return;
	if( show )
	{
		m_skillButtonAnimation->setCycCount( PlayTime );
		m_skillButtonAnimation->show();
		m_skillButtonAnimation->play();
	}
	else
	{
		m_skillButtonAnimation->hide();
		m_skillButtonAnimation->stop();
	}
}

bool  KUiNaviation::handleAimationDown( const CEGUI::EventArgs& args	)
{
	if ( !m_skillButton || !m_skillButtonAnimation )
		return false;
	WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
	if( !eventArgs )
		return false;
	WindowEventArgs eventObj( *eventArgs );
	
	eventObj.window = m_skillButton;
	m_skillButton->fireEvent( PushButton::EventClicked, eventObj );
	return true; 
}

void KUiNaviation::Breath()
{
	DWORD curTime = ::GetTickCount();
	if ( curTime - m_currentSec < BreathTrequency )
		return ;
	m_currentSec = curTime;
	bool isNew = IsNewAbility();
	if( m_isOpenAnimation && !KUiStudySkillManage::GetSingleton().IsVisible() && isNew )
		ShowAimationOfSkillBtn( true );
	else
		ShowAimationOfSkillBtn();
}
bool KUiNaviation::handleDisplayNetInfo(const CEGUI::EventArgs& args )
{
	DisplayConnectionState(m_dwPing);
	
	KUiItemTip::GetSingleton();
	KUiItemTip::GetSingleton().show( m_szNetInfo, pImageSlowNetSpeed->getRect(Absolute).offset(m_pThisWnd->getPosition(Absolute)), KUiItemTip::Top);
	return true;
}

bool KUiNaviation::handleCloseNetInfo( const CEGUI::EventArgs& args )
{
	KUiItemTip::Hide();
	return true;
}

void KUiNaviation::DisplayConnectionState(int nPing)
{
	// NetInfo
	if ( m_szNetInfo )
	{
		ZeroMemory( m_szNetInfo, MAX_TEXT_LEN );
	}

	const KUiCfgLoader::NetInfoData& netInfoCfg = KUiCfgLoader::getSingleton().getNetInfoData();
	sprintf( m_szNetInfo, "<Layout width=%d><Seg text-align=center float=wrap><Obj color=%s font-family=%s>",
			 netInfoCfg.maxWidth, netInfoCfg.NetInfoColor, netInfoCfg.NetInfoFont);

	switch( (int)((float)nPing/g_Lag) )
	{
	case 0:
		{
			strcat( m_szNetInfo, KMessageCentre::GetMessage(connection_state_message, 2) );
			break;
		}
	case 1:
		{
			strcat( m_szNetInfo, KMessageCentre::GetMessage(connection_state_message, 3) );
			break;
		}
	case 2:
		{
			strcat( m_szNetInfo, KMessageCentre::GetMessage(connection_state_message, 4) );
			break;
		}
	default:
		{
			strcat( m_szNetInfo, KMessageCentre::GetMessage(connection_state_message, 4) );
			break;
		}
	}

	char* temp = KMessageCentre::GetMessage(connection_state_message, 6);
	strcat( m_szNetInfo, temp );
	sprintf( temp, "%d", nPing );
	strcat( m_szNetInfo, temp );
	temp = KMessageCentre::GetMessage(connection_state_message, 7);
	strcat( m_szNetInfo, temp );
	
	// DateInfo
	time_t tval;   
    struct tm *now;   
    
    // Get current date and time   
    tval = time(NULL);   
    now = localtime(&tval);
	char szDate[128];
    sprintf(szDate, "\t\n%4d-%d-%02d  %d:%02d:%02d", now->tm_year+1900, 
			now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	strcat(m_szNetInfo, szDate);
	
	strcat(m_szNetInfo , "</Obj></Seg></Layout>");

}

/************************************************************************/
/*                                                                      */
/************************************************************************/
unsigned int KUiNaviationEx::FriendMgrID = UI_FRIEND_MGR;

template<> 
KUiNaviationEx* KUiWndSingleton<KUiNaviationEx>::ms_Singleton	= NULL;

KUiNaviationEx::KUiNaviationEx( const CEGUI::String& id_name )
: KUiWndSingleton<KUiNaviationEx>( id_name )
, m_bTongStatus(false)
{
	m_nTongRequireLevel = 0;
}

KUiNaviationEx::~KUiNaviationEx()
{
}

void KUiNaviationEx::Show( void )
{
	KUiWndSingleton<KUiNaviationEx>::Show();
}

void KUiNaviationEx::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->setZLevel(Window::Bottom);
		
		m_nTongRequireLevel			= getTongRequireLevel();
		string sRequireLevel		= iToStr( m_nTongRequireLevel );
		std::string sFormatTemplate = KMessageCentre::GetMessageSafe( minitoolbar_message, NO_TONG_MESSAGE );
		char szCueMsg[ COMMON_CLIENT_MSG_LEN_512 ] = { 0 };
		_snprintf( szCueMsg, COMMON_CLIENT_MSG_LEN_512, sFormatTemplate.c_str(),  sRequireLevel.c_str() );
		szCueMsg[ sizeof( szCueMsg ) - 1 ] = 0;
		m_sNoTongMessage = szCueMsg;

		// Do events wire-up
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NavigationEx/SystemMgr")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviationEx::handleOption, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NavigationEx/FriendMgr")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviationEx::handleFriend, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NavigationEx/TaskMgr")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviationEx::handleQuest, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NavigationEx/TongMgr")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNaviationEx::handleTong, ms_Singleton));
		//ms_Singleton->m_pThisWnd->getChild("TaharezLook/NavigationEx/ElfMgr")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiNaviationEx::handleHelp, ms_Singleton));
	}
}

void	KUiNaviationEx::ActiveButton( int index )
{
	String str("TaharezLook/NavigationEx/");
	String temp;
	switch(index)
	{
	case 4:
		temp = "TaskMgr";
		break;
	case 5:
		temp = "TongMgr";
		break;
	case 6:
		temp = "FriendMgr";
	    break;
	case 7:
		temp = "SystemMgr";
	    break;
	default:
		return;
	}

	str = str+temp;
	static_cast<TLButton*>( ms_Singleton->m_pThisWnd->getChild(str) )->activeImage(10);
}

bool	KUiNaviationEx::handleTong( const CEGUI::EventArgs& args )
{
	SocietyInfoIndex tagSocietyIdx;
	tagSocietyIdx.TemplateId	= enSUTplId_Tong;
	int nTopLayer = 0;
	g_pCoreShell->GetGameData( GDI_GET_SOCIETY_PLAYER, (unsigned int)&tagSocietyIdx, (int)&nTopLayer );
	if ( nTopLayer > 0 )
	{
		//暂时这样处理，没有找到更好的方法
		if ( !m_bTongStatus )
		{
			m_bTongStatus = true;
			KUiTongManager::Show();
		}
		else
		{
			if(KUiTongManager::GetSingleton().IsVisible() == false)
			{
				if(KUiSceneMap::getSinglton().isVisible())
				{
					KUiSceneMap::getSinglton().hide();
				}
				KUiTongManager::Show();
			}
			else
				KUiTongManager::Hide();
		}
		static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/NavigationEx/TongMgr"))->deactivateImage();
		return true;
	}
	else
	{
// 		char *message = KMessageCentre::GetMessage(tong_operation_message, 24);
// 		KUiChannelCentre::GetSingleton().toSysMsg(message);
// 		return false;

		//caolei modified 2008.12.1
		//氏族界面相关优化
		handleNoTongOper();
		return false;
	}
}

void 
KUiNaviationEx::handleNoTongOper()
{
	int playerLevel = getClientPlayerLevel();
	if ( m_nTongRequireLevel <= 0 || playerLevel <= 0 )
		return;
	
	if ( m_nTongRequireLevel > playerLevel )
	{
		//KUiChannelCentre::GetSingleton().toSysMsg( m_sNoTongMessage.c_str() );
		KUiChannelCentre::GetSingleton().recvCustomMessageConst( SYSTEM_ROOM_ID,m_sNoTongMessage.c_str() );
	}
	else
	{
		//打开氏族招募面板
		KUiTongRecruitCentre::Show();
	}
}

bool	KUiNaviationEx::handleQuest( const CEGUI::EventArgs& args )
{
	/*if(KUiQuestManage::IsVisible() == false)
	{
		if(KUiSceneMap::getSinglton().isVisible())
		{
			KUiSceneMap::getSinglton().hide();
		}
		KUiQuestManage::GetSingleton().show();
	}
	else
	KUiQuestManage::GetSingleton().hide();//*/
	
	KUiFSBible::getSingleton().toggleQuest();
	
	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/NavigationEx/TaskMgr"))->deactivateImage();
	return true;
}

bool	KUiNaviationEx::handleFriend( const CEGUI::EventArgs& args )
{
	if(KUiChatCentre::GetSingleton().IsVisible() == false)
	{
		if(KUiSceneMap::getSinglton().isVisible())
		{
			KUiSceneMap::getSinglton().hide();
		}
		KUiChatCentre::Show();
	}
	else
		KUiChatCentre::Hide();
	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/NavigationEx/FriendMgr"))->deactivateImage();
	return true;
}

bool	KUiNaviationEx::handleOption( const CEGUI::EventArgs& args )
{
	if ( KUiExit::IsVisible()	)
	{
		KUiExit::Hide();
	}
	else
	{
		KUiExit::Show();
	}
	static_cast<TLButton*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/NavigationEx/SystemMgr"))->deactivateImage();
	return true;
}

int 
KUiNaviationEx::getTongRequireLevel()
{
	SocietyInfoIndex infoIndexParam;
	infoIndexParam.TemplateId	= enSUTplId_Tong;
	infoIndexParam.Layer		= enSULayer_Gens;
	infoIndexParam.Operation	= enSUO_AddSubUnit;
	return g_pCoreShell->GetGameData( GDI_GET_SOCIETY_REQUIRE_LEVEL, reinterpret_cast< unsigned int >( &infoIndexParam ), NULL );
}

int 
KUiNaviationEx::getClientPlayerLevel()
{
	KUiPlayerAttribute playerAttribute;
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, reinterpret_cast< unsigned int >( &playerAttribute ), NULL );
	return playerAttribute.nLevel;
}

/*bool KUiNaviationEx::handleSearchHelp( const CEGUI::EventArgs& args	)
{
	if ( !KUiGameSetting::GetSingletonPtr()->GetShowHelpElf() )
	{
		return false;
	}

	if(KUiSearchHelpWnd::GetSingleton().IsSearchVisible() == false)
	{
		KUiSearchHelpWnd::GetSingleton().ShowSearchHelp();
	}
	else
	{
		KUiSearchHelpWnd::Hide();
	}
	return true;
}//*/
