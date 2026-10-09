//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/26/2006 21:24
//      File_base        : UiSelPlayer
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KWin32Wnd.h"
#include "CoreShell.h"
#include "KIniFile.h"
#include "UiSelPlayer.h"
#include "UiLoginBg.h"
#include "UiLogin.h"
#include "UiNewPlayer.h"
#include "CoreUseNameDef.h"
#include "KSG_MD5_String.h"
#include "UiBeginHelp.h"
#include "UiDelComfirm.h"
#include ".\..\UiGlobalEvent.h"
#include "../UiConfigManager.h"
#include "UiChangeMapWnd.h"
#include "UiWaitingMsg.h"
#include "../KMessageCentre.h"
#include "../../NetConnect/NetConnectAgent.h"

extern iCoreShell*		g_pCoreShell;


using namespace CEGUI;

const unsigned int	KUiSelPlayer::CreateButtonID = UI_SELROLE_NEW;
const unsigned int	KUiSelPlayer::EnterButtonID = UI_SELROLE_LOGIN;
const unsigned int	KUiSelPlayer::DeleteButtonID = UI_SELROLE_DELETE;
const unsigned int	KUiSelPlayer::ExitButtonID = UI_SELROLE_BACK;


/************************************************************************/
/*                                                                      */
/************************************************************************/

KUiRoleItem::KUiRoleItem()
{
	d_window = NULL;
	d_roleFace = NULL;
	d_roleFaceBg = NULL;
	d_roleFaceFront = NULL;
	d_roleInfo = NULL;
	d_roleName = NULL;
	d_roleLeve = NULL;
	d_roleLastMap = NULL;
	d_roleDestoryTime = NULL;
	d_roleFreeze = NULL;
	d_roleForbid = NULL;
	d_SelPlayerWnd = NULL;
	d_index = -1;
}

void KUiRoleItem::loadWnd( int nIdx, KUiSelPlayer* pWnd  )
{
	d_SelPlayerWnd = pWnd;
	d_index = nIdx;

	char szPlusName[COMMON_CLIENT_MSG_LEN_64];
	snprintf(szPlusName, COMMON_CLIENT_MSG_LEN_64,  "%d_", nIdx);
	szPlusName[COMMON_CLIENT_MSG_LEN_64-1] = 0;	

	WindowManager& wm = WindowManager::getSingleton();
	d_window = wm.loadWindowLayout( "\\uisettings\\layouts\\rolelistitem.ls", szPlusName );

	String windName = d_window->getName();
	if ( d_window && d_SelPlayerWnd )
	{
		d_roleFace				= d_window->getChild(  windName + "/RoleFace" );
		d_roleFaceBg			= d_window->getChild(  windName + "/RoleFaceBg" );
		d_roleFaceFront			= (RadioButton*)d_window->getChild(  windName + "/RoleFaceFront" );
		d_roleInfo				= d_window->getChild(  windName + "/RoleInfo" );
		if ( d_roleInfo )
		{
			String infoName		= d_roleInfo->getName();
			d_roleName			= d_roleInfo->getChild(  infoName + "/RoleName" );
			d_roleLeve			= d_roleInfo->getChild(  infoName + "/RoleLeve" );
			d_roleLastMap		= d_roleInfo->getChild(  infoName + "/LastMap" );
			d_roleDestoryTime	= d_roleInfo->getChild(  infoName + "/DestoryTime" );
			d_roleFreeze		= (Checkbox*)d_roleInfo->getChild(  infoName + "/Freeze" );
			d_roleForbid		= (Checkbox*)d_roleInfo->getChild(  infoName + "/Forbid" );
		}
		d_window->setWantsMultiClickEvents( true );
		d_window->setWantsMultiClickEvents( true );
		d_window->subscribeEvent(StaticImage::EventMouseDoubleClick, Event::Subscriber(&KUiRoleItem::handleDCRole, this));
		if ( d_roleFaceFront )
		{
			d_roleFaceFront->subscribeEvent(RadioButton::EventMouseDoubleClick, Event::Subscriber(&KUiRoleItem::handleDCRole, this));
			d_roleFaceFront->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiRoleItem::handleRole, this));
		}
		
		d_SelPlayerWnd->m_pRoleListWnd->addChildWindow( d_window );
		d_window->setPosition( Absolute, CEGUI::Point( d_window->getXPosition(Absolute), d_window->getHeight(Absolute) * d_index) );
	}
}


void KUiRoleItem::setRole( KRoleChiefInfo* pRole )
{
	if ( pRole == NULL )
	{
		return;
	}

	//设置人物头像背景
	char szMetier[5];
	if ( d_roleFaceBg )
	{
		String strRoleFaceImageSetName = "xuanzejuesetu";
		String strRoleFaceImageName = "jiashinan";
		
		
		switch ( pRole->byAttribute )  
		{
		case 0:
			snprintf( szMetier, 5, "%s", ROLE_CAREER_JS );
			if ( pRole->byGender )
			{
				strRoleFaceImageName = "jiashinv";
			}
			else
			{
				strRoleFaceImageName = "jiashinan";
			}
			break;
		case 1:
			snprintf( szMetier, 5, "%s", ROLE_CAREER_DS );
			if ( pRole->byGender )
			{
				strRoleFaceImageName = "daoshinv";
			}
			else
			{
				strRoleFaceImageName = "daoshinan";
			}
			break;
		case 2:
			snprintf( szMetier, 5, "%s", ROLE_CAREER_YR );
			if ( pRole->byGender )
			{
				strRoleFaceImageName = "yirennv";
			}
			else
			{
				strRoleFaceImageName = "yirennan";
			}
			break;
		default:
			return;
		}		

		if ( !strRoleFaceImageSetName.empty() && !strRoleFaceImageName.empty() )
		{
			static_cast<StaticImage*>(d_roleFaceBg)->setFrameEnabled( false );
			static_cast<StaticImage*>(d_roleFaceBg)->setBackgroundEnabled( false );
			// set the background image
			static_cast<StaticImage*>(d_roleFaceBg)->setImage( strRoleFaceImageSetName, strRoleFaceImageName );
			static_cast<StaticImage*>(d_roleFaceBg)->setEnabled( false );
			static_cast<StaticImage*>(d_roleFaceBg)->setVisible( true );
		}
	}

	//设置高亮
	if ( d_roleFaceFront )
	{
		d_roleFaceFront->enable();
		d_roleFaceFront->setSelected(true);
	}

	//Role name.
	if ( d_roleName )
	{
		d_roleName->setAnsiText( pRole->szName );
	}

	//Level and Metier information.
	szMetier[4] = 0;
	if ( d_roleLeve )
	{
		char szLevelMetier[COMMON_CLIENT_MSG_LEN_64];
		snprintf( szLevelMetier, COMMON_CLIENT_MSG_LEN_64,  MSG_LEVEL_SELROLEWND, pRole->uLevel, szMetier );
		d_roleLeve->setAnsiText( szLevelMetier );
	}

	//Last map information.
	if ( d_roleLastMap )
	{
		KIniFile mapFile;
		if ( mapFile.Load( "\\settings\\maplist.ini" ) )
		{
			char szLastMapKey[COMMON_CLIENT_MSG_LEN_64];
			char szLastMap[COMMON_CLIENT_MSG_LEN_64];
			itoa( pRole->dwLastMapID, szLastMapKey, 10 );
			mapFile.GetString( "List", szLastMapKey, "", szLastMap, sizeof( szLastMap ) );
			if ( szLastMap[0] == 0 )
			{
				d_roleLastMap->setAnsiText( "----------" );
			}
			else
			{
				d_roleLastMap->setAnsiText( szLastMap );
			}
		}
	}

	//Destory Time information.
	if ( d_roleDestoryTime )
	{		
		int nDay = pRole->dwWillDestoryTime/86400;
		if ( nDay <= 30 && nDay >= 0 )
		{
			char szTime[256];
			if ( nDay == 0 )
			{				
				sprintf( szTime, AUTO_DESTROY_TODAY );
			}
			else
			{
				sprintf( szTime, AUTO_DESTROY, nDay );
			}
			
			d_roleDestoryTime->setAnsiText( szTime );
			
			const char* szTip = KMessageCentre::GetMessage( login_error_message, CI_MI_WAITING_ROLEDELETE );
			if ( szTip )
			{
				char szMessage[COMMON_CLIENT_MSG_LEN_256];
				snprintf( szMessage,COMMON_CLIENT_MSG_LEN_256, szTip, pRole->szName );
				KUiWaitingMsg::GetSingleton().ShowMessage( szMessage );
			}			
		}
		else
		{
			d_roleDestoryTime->setAnsiText( "" );
		}
	}

	//Freeze information.
	if ( d_roleFaceFront )
	{
		if ( pRole->byFreeze )
		{
			d_roleFaceFront->disable();
		}
		else
		{
			d_roleFaceFront->enable();
		}
	}		
	if ( d_roleFreeze )
	{
		if ( pRole->byFreeze )
		{
			d_roleFreeze->setSelected( true );
			d_roleFreeze->show();
		}
		else
		{
			d_roleFreeze->setSelected( false );
			d_roleFreeze->hide();
		}
	}

	//Forbid information
	if ( d_roleForbid )
	{
		if ( pRole->byForbid )
		{
			d_roleForbid->setSelected( true );
			d_roleForbid->show();
		}
		else
		{
			d_roleForbid->setSelected( false );
			d_roleForbid->hide();
		}
	}
}

void KUiRoleItem::clearRole( void )
{
	// clear 设置人物头像背景
	if ( d_roleFaceBg )
	{
		static_cast<StaticImage*>(d_roleFaceBg)->setFrameEnabled( false );
		static_cast<StaticImage*>(d_roleFaceBg)->setBackgroundEnabled( false );
		// set the background image
		static_cast<StaticImage*>(d_roleFaceBg)->setEnabled( false );
		static_cast<StaticImage*>(d_roleFaceBg)->hide();
	}

	//设置高亮
	if ( d_roleFaceFront )
	{
		d_roleFaceFront->enable();
		d_roleFaceFront->setSelected(false);
	}


	//clear role name.
	if ( d_roleName )
	{
		d_roleName->setText( "" );
	}
	
	if ( d_roleLeve )
	{
		d_roleLeve->setText( "" );
	}

	//Last map information.
	if ( d_roleLastMap )
	{
		d_roleLastMap->setText( "" );
	}

	//clear destory Time information.
	if ( d_roleDestoryTime )
	{		
		d_roleDestoryTime->setText( "" );
	}
	
	//Freeze information.
	if ( d_roleFreeze )
	{
		d_roleFreeze->setSelected( false );
		d_roleFreeze->hide();
	}
	
	//clear forbid information
	if ( d_roleForbid )
	{
		d_roleForbid->setSelected( false );
		d_roleForbid->hide();
	}
	
}

bool	KUiRoleItem::handleRole( const CEGUI::EventArgs& args		)
{
	if ( d_SelPlayerWnd == NULL )
	{
		return true;
	}

	d_SelPlayerWnd->clearRadioBtn();

	if ( d_roleFaceFront )
	{
		d_roleFaceFront->setSelected( true );
	}
	d_SelPlayerWnd->m_nSelRoleIdx = d_index;
	KRoleChiefInfo tagChiefInfo;
	ZeroMemory( &tagChiefInfo, sizeof( tagChiefInfo) );
	if ( g_LoginLogic.GetRoleInfo( d_index, &tagChiefInfo ) )
	{
		if ( tagChiefInfo.dwEmployLeftTime > 0 && tagChiefInfo.dwEmployLeftTime < 0xfffffffd )
		{
			const char* szTip = KMessageCentre::GetMessage( login_error_message, CI_MI_ENGAGE_TIP_1 );
			if ( szTip )
			{
				char szMessage[COMMON_CLIENT_MSG_LEN_256];
				DWORD dwLeftTime = (tagChiefInfo.dwEmployLeftTime / 60) != 0 ? (tagChiefInfo.dwEmployLeftTime / 60) : 1;
				snprintf( szMessage,COMMON_CLIENT_MSG_LEN_256, szTip, tagChiefInfo.szName, dwLeftTime );
				KUiWaitingMsg::GetSingleton().ShowMessage( szMessage, false );
			}
		}
		if ( tagChiefInfo.dwEmployLeftTime == -2 )
		{
			const char* szTip = KMessageCentre::GetMessage( login_error_message, CI_MI_ENGAGE_TIP_2 );
			if ( szTip )
			{
				char szMessage[COMMON_CLIENT_MSG_LEN_256];
				snprintf( szMessage,COMMON_CLIENT_MSG_LEN_256, szTip, tagChiefInfo.szName );
				KUiWaitingMsg::GetSingleton().ShowMessage( szMessage, false );
			}
		}

		d_SelPlayerWnd->selectRole( tagChiefInfo.byAttribute );
	}
	else
	{
		d_SelPlayerWnd->clearRole(d_index);
		d_SelPlayerWnd->m_nSelRoleIdx = -1;
	}
	KUiCfgLoader& cfg = KUiCfgLoader::getSingleton();
	std::string imageset;
	std::string image;
	if ( tagChiefInfo.byGender )
	{
		cfg.getWomanPortraitPath(tagChiefInfo.byPortrait, imageset, image );
	}
	else
	{
		cfg.getManPortraitPath(tagChiefInfo.byPortrait, imageset, image );
	}
	Window* pFaceBk = d_SelPlayerWnd->m_pThisWnd->getChild( "TaharezLook/SelRole/DiyFaceBK" );
	if ( pFaceBk )
	{
		TLStaticImage* pFace = (TLStaticImage*)pFaceBk->getChild("TaharezLook/SelRole/DiyFace");
		if ( pFace )
		{
			pFace->setFrameEnabled( false );
 			pFace->setBackgroundEnabled( false );
 			pFace->setImage( imageset.c_str(), image.c_str() );
			pFace->setEnabled( false );
			pFace->setVisible( true );
		}
	}

	Window *pDisplay = d_SelPlayerWnd->m_pThisWnd->getChild( "TaharezLook/SelRole/Display" );
	String name;
	if ( pDisplay )
	{
		
		switch ( tagChiefInfo.byAttribute )  
		{
		case 0:
			if ( tagChiefInfo.byGender )
			{
				name = UI_LOGINBK_IMAGESET_NAME_SEL_JS_1;
				UI_DISPLAYER_NPC_SYNC npc;
				ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
				npc.m_Doing			= do_stand;			
				npc.m_btKind		= kind_player;				
				npc.NpcSettingIdx	= -4;
				npc.nLevel			= tagChiefInfo.uLevel;
				npc.nWeapon			= tagChiefInfo.nWeapon;
				npc.nHelm			= tagChiefInfo.nHelm;
				npc.nArmor			= tagChiefInfo.nArmor;			
				npc.nShoulder		= tagChiefInfo.nShoulder;
				npc.nCuff			= tagChiefInfo.nCuff;
				npc.nBoot			= tagChiefInfo.nBoot;
				npc.nHorse			= tagChiefInfo.nHorse;
				npc.bRideHorse		= tagChiefInfo.bRideHorse;
				g_pCoreShell->SetUiLoginDisplayerNpc( &npc );
			}
			else
			{
				name = UI_LOGINBK_IMAGESET_NAME_SEL_JS_0;
				UI_DISPLAYER_NPC_SYNC npc;
				ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
				npc.m_Doing = do_stand;			
				npc.m_btKind = kind_player;				
				npc.NpcSettingIdx	= -1;
				npc.nLevel			= tagChiefInfo.uLevel;
				npc.nWeapon			= tagChiefInfo.nWeapon;
				npc.nHelm			= tagChiefInfo.nHelm;
				npc.nArmor			= tagChiefInfo.nArmor;			
				npc.nShoulder		= tagChiefInfo.nShoulder;
				npc.nCuff			= tagChiefInfo.nCuff;
				npc.nBoot			= tagChiefInfo.nBoot;
				npc.nHorse			= tagChiefInfo.nHorse;
				npc.bRideHorse		= tagChiefInfo.bRideHorse;
				g_pCoreShell->SetUiLoginDisplayerNpc( &npc );
			}
			break;
		case 1:
			if ( tagChiefInfo.byGender )
			{
				name = UI_LOGINBK_IMAGESET_NAME_SEL_DS_1;
				UI_DISPLAYER_NPC_SYNC npc;
				ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
				npc.m_Doing = do_stand;			
				npc.m_btKind = kind_player;				
				npc.NpcSettingIdx	= -5;
				npc.nLevel			= tagChiefInfo.uLevel;
				npc.nWeapon			= tagChiefInfo.nWeapon;
				npc.nHelm			= tagChiefInfo.nHelm;
				npc.nArmor			= tagChiefInfo.nArmor;			
				npc.nShoulder		= tagChiefInfo.nShoulder;
				npc.nCuff			= tagChiefInfo.nCuff;
				npc.nBoot			= tagChiefInfo.nBoot;
				npc.nHorse			= tagChiefInfo.nHorse;
				npc.bRideHorse		= tagChiefInfo.bRideHorse;
				g_pCoreShell->SetUiLoginDisplayerNpc( &npc );
			}
			else
			{
				name = UI_LOGINBK_IMAGESET_NAME_SEL_DS_0;
				UI_DISPLAYER_NPC_SYNC npc;
				ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
				npc.m_Doing = do_stand;			
				npc.m_btKind = kind_player;				
				npc.NpcSettingIdx	= -2;
				npc.nLevel			= tagChiefInfo.uLevel;
				npc.nWeapon			= tagChiefInfo.nWeapon;
				npc.nHelm			= tagChiefInfo.nHelm;
				npc.nArmor			= tagChiefInfo.nArmor;			
				npc.nShoulder		= tagChiefInfo.nShoulder;
				npc.nCuff			= tagChiefInfo.nCuff;
				npc.nBoot			= tagChiefInfo.nBoot;
				npc.nHorse			= tagChiefInfo.nHorse;
				npc.bRideHorse		= tagChiefInfo.bRideHorse;
				g_pCoreShell->SetUiLoginDisplayerNpc( &npc );
			}
			break;
		case 2:
			if ( tagChiefInfo.byGender )
			{
				name = UI_LOGINBK_IMAGESET_NAME_SEL_YR_1;
				UI_DISPLAYER_NPC_SYNC npc;
				ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
				npc.m_Doing = do_stand;			
				npc.m_btKind = kind_player;				
				npc.NpcSettingIdx	= -6;
				npc.nLevel			= tagChiefInfo.uLevel;
				npc.nWeapon			= tagChiefInfo.nWeapon;
				npc.nHelm			= tagChiefInfo.nHelm;
				npc.nArmor			= tagChiefInfo.nArmor;			
				npc.nShoulder		= tagChiefInfo.nShoulder;
				npc.nCuff			= tagChiefInfo.nCuff;
				npc.nBoot			= tagChiefInfo.nBoot;
				npc.nHorse			= tagChiefInfo.nHorse;
				npc.bRideHorse		= tagChiefInfo.bRideHorse;
				g_pCoreShell->SetUiLoginDisplayerNpc( &npc );
			}
			else
			{
				name = UI_LOGINBK_IMAGESET_NAME_SEL_YR_0;
				UI_DISPLAYER_NPC_SYNC npc;
				ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
				npc.m_Doing = do_stand;			
				npc.m_btKind = kind_player;				
				npc.NpcSettingIdx	= -3;
				npc.nLevel			= tagChiefInfo.uLevel;
				npc.nWeapon			= tagChiefInfo.nWeapon;
				npc.nHelm			= tagChiefInfo.nHelm;
				npc.nArmor			= tagChiefInfo.nArmor;			
				npc.nShoulder		= tagChiefInfo.nShoulder;
				npc.nCuff			= tagChiefInfo.nCuff;
				npc.nBoot			= tagChiefInfo.nBoot;
				npc.nHorse			= tagChiefInfo.nHorse;
				npc.bRideHorse		= tagChiefInfo.bRideHorse;
				g_pCoreShell->SetUiLoginDisplayerNpc( &npc );
			}
			break;
		default:
			return false;
		}
		static_cast<StaticImage*>(pDisplay)->setImage( name, UI_FULL_IMAGESET );
		static_cast<StaticImage*>(pDisplay)->setCyc( true );
		static_cast<StaticImage*>(pDisplay)->play();
	}
	return true;
}

bool	KUiRoleItem::handleDCRole( const CEGUI::EventArgs& args		)
{
	if ( d_SelPlayerWnd == NULL )
	{
		return true;
	}
	d_SelPlayerWnd->m_nSelRoleIdx = d_index;
	KRoleChiefInfo tagChiefInfo;
	ZeroMemory(&tagChiefInfo, sizeof(tagChiefInfo));
	if ( g_LoginLogic.GetRoleInfo( d_SelPlayerWnd->m_nSelRoleIdx, &tagChiefInfo ) )
	{
		//KUiBeginHelp::GetSingleton().setRoleIdx( m_nSelRoleIdx );
		d_SelPlayerWnd->IsFirstLogin( tagChiefInfo.szName );
		g_LoginLogic.SelectRoleLoginGame( d_SelPlayerWnd->m_nSelRoleIdx );
	}
	else
	{
		d_SelPlayerWnd->clearRole(d_index);
		d_SelPlayerWnd->m_nSelRoleIdx = -1;
	}

	return true;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

template<> 
KUiSelPlayer* KUiWndSingleton<KUiSelPlayer>::ms_Singleton	= NULL;

KUiSelPlayer::KUiSelPlayer( const CEGUI::String& id_name ):
KUiWndSingleton<KUiSelPlayer>( id_name )
{
	ZeroMemory(m_filename, COMMON_CLIENT_MSG_LEN_128);
	ZeroMemory(m_newRoleName, COMMON_CLIENT_MSG_LEN_32);
	m_nSelRoleIdx = -1;
	m_roleCurCount = 0;
	m_pRoleListWnd = NULL;
	m_pRoleListWndCliper = NULL;
	m_proleListScrol = NULL;
	m_roleListbox = NULL;
	m_roleListboxBG = NULL;
}

KUiSelPlayer::~KUiSelPlayer()
{

}

void KUiSelPlayer::Init( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
 		static_cast<StaticImage*>(ms_Singleton->m_pThisWnd)->setRect( Relative, Rect(0, 0, 1, 1) );
 		static_cast<StaticImage*>(ms_Singleton->m_pThisWnd)->setFrameEnabled( false );
 		static_cast<StaticImage*>(ms_Singleton->m_pThisWnd)->setBackgroundEnabled( false );

		// Do events wire-up
		// set the background image
		ms_Singleton->m_pThisWnd->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiSelPlayer::handleKeyDown, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild(CreateButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiSelPlayer::handleCreate, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild(ExitButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiSelPlayer::handleBack, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild(EnterButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiSelPlayer::handleEntry, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild(DeleteButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiSelPlayer::handleDelete, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SelRole/liebiao")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiSelPlayer::handleShowList, ms_Singleton));
		m_roleListboxBG = ms_Singleton->m_pThisWnd->getChild( "TaharezLook/SelRole/jueseliebiao" );
		if ( m_roleListboxBG )
		{
			m_roleListboxBG->disable();
		}
		m_roleListbox = (TLListbox*)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/SelRole/RoleListBoxList" );
		if ( m_roleListbox )
		{
			m_roleListbox->subscribeEvent(Listbox::EventDoubleClickListItem,  Event::Subscriber(&KUiSelPlayer::handleDListBoxClick, ms_Singleton));
			m_roleListbox->subscribeEvent(Listbox::EventMouseClick,  Event::Subscriber(&KUiSelPlayer::handleListBoxClick, ms_Singleton));
		}
		ms_Singleton->m_pRoleListWndCliper = ms_Singleton->m_pThisWnd->getChild( "TaharezLook/SelRole/RoleListCliper" );
		ms_Singleton->m_pRoleListWnd = ms_Singleton->m_pRoleListWndCliper->getChild( "TaharezLook/SelRole/RoleList" );
		if ( ms_Singleton->m_pRoleListWnd )
		{
			m_pRoleListWnd->setZLevel(CEGUI::Window::SuperTop);
		}
		ms_Singleton->m_proleListScrol = (CEGUI::TLVertScrollbar*)ms_Singleton->m_pRoleListWndCliper->getChild("TaharezLook/SelRole/RoleList/RoleListScrollbar");
		if ( m_proleListScrol)
		{
			m_proleListScrol->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiSelPlayer::handleTreeScroll, this));
		}		
		
		for ( int nIdx = 0; nIdx < MAX_PLAYER_IN_ACCOUNT; nIdx++ )
		{
			m_roleItem[nIdx].loadWnd( nIdx, ms_Singleton );
		}
	}
}

void KUiSelPlayer::Show( void )
{
	KUiWndSingleton<KUiSelPlayer>::Show();
	if (ms_Singleton&&ms_Singleton->m_pThisWnd)
	{
		g_pCoreShell->PlayUiLoginDisplayerNpc();
		ms_Singleton->m_pThisWnd->activate();
		KUiChangeMapWnd::GetSingleton().EndLoading();
	}
}

void KUiSelPlayer::Hide( void )
{
	if ( ms_Singleton == NULL )
	{
		return;
	}
	if ( ms_Singleton->m_pThisWnd == NULL )
	{
		return;
	}
	TLStaticImage* pDisplay = (TLStaticImage*)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/SelRole/Display" );
	if (pDisplay)
	{
		pDisplay->stop();
	}
	if ( g_pCoreShell )
	{
		g_pCoreShell->PlayUiLoginDisplayerNpc();
	}
	
	KUiWndSingleton<KUiSelPlayer>::Hide();
}

void KUiSelPlayer::setRoleList( KRoleList* pRoleList )
{
	if ( ms_Singleton == NULL )
	{
		return;
	}

	if ( pRoleList == NULL )
	{
		return;
	}

	if ( pRoleList->size( ) == 0 )
	{
		return;
	}

	if ( m_proleListScrol == NULL )
	{
		return;
	}

	
	if ( m_roleListbox == NULL || 
		m_roleListboxBG == NULL )
	{
		return;
	}

	if ( m_pRoleListWnd == NULL )
	{
		return;
	}

	int selRole = -1;
	if ( ms_Singleton->m_pThisWnd )
	{
		for ( int n = 0; n < MAX_PLAYER_IN_ACCOUNT; ++n )
		{
			clearRole( n );
		}
		
		m_roleListbox->resetList();

		m_roleCurCount = MAX_PLAYER_IN_ACCOUNT >= pRoleList->size() ? pRoleList->size() : MAX_PLAYER_IN_ACCOUNT; 
		
		for ( int nIdx = 0; nIdx < m_roleCurCount; ++nIdx )
		{
			setRole( nIdx, (*pRoleList)[nIdx] );
			if ( !strcmp( (*pRoleList)[nIdx]->szName, m_newRoleName ) )
				selRole = nIdx;
		}
	}

	if ( selRole != -1 )
	{
		CEGUI::WindowEventArgs args(m_roleItem[selRole].getFront());
		m_roleItem[selRole].handleRole( args );
		ZeroMemory(m_newRoleName, COMMON_CLIENT_MSG_LEN_32);
	}
	else
	{
		CEGUI::WindowEventArgs args(m_roleItem[0].getFront());
		m_roleItem[0].handleRole( args );
		
		m_roleListbox->setItemSelectState( static_cast< unsigned int >( 0 ), true );
		CEGUI::WindowEventArgs args_rolelist( m_roleListbox );
		handleListBoxClick( args_rolelist );
	}

	if ( m_roleCurCount <= 2  )
	{
		m_proleListScrol->hide();
		m_roleListbox->hide();
		m_roleListboxBG->hide();
	}
	else
	{
		m_proleListScrol->show();
		m_roleListboxBG->show();
		m_roleListbox->show();

	}

	m_pRoleListWnd->setSize( Absolute, Size( m_roleItem[0].getWidth(), m_roleItem[0].getHeight() * m_roleCurCount ) );
}

void KUiSelPlayer::setRole( int nIdx, KRoleChiefInfo* pRole	)
{
	if ( nIdx < 0 || nIdx >=  MAX_PLAYER_IN_ACCOUNT )
	{
		return;
	}

	if ( pRole == NULL )
	{
		return;
	}

	if ( m_roleListbox == NULL )
	{
		return;
	}

	String strRoleImageName;

	switch ( pRole->byAttribute )  
	{
	case 0:
		if ( pRole->byGender )
		{
			strRoleImageName = UI_LOGINBK_IMAGESET_NAME_SEL_JS_1;
		}
		else
		{
			strRoleImageName = UI_LOGINBK_IMAGESET_NAME_SEL_JS_0;
		}
		break;
	case 1:
		if ( pRole->byGender )
		{
			strRoleImageName = UI_LOGINBK_IMAGESET_NAME_SEL_DS_1;
		}
		else
		{
			strRoleImageName = UI_LOGINBK_IMAGESET_NAME_SEL_DS_0;
		}
		break;
	case 2:
		if ( pRole->byGender )
		{
			strRoleImageName = UI_LOGINBK_IMAGESET_NAME_SEL_YR_1;
		}
		else
		{
			strRoleImageName = UI_LOGINBK_IMAGESET_NAME_SEL_YR_0;
		}
		break;
	default:
		return;
	}

	selectRole( pRole->byAttribute );
	
	Window *pDisplay = m_pThisWnd->getChild( "TaharezLook/SelRole/Display" );

 	// disable frame and standard background
 	static_cast<StaticImage*>(pDisplay)->setFrameEnabled( false );
 	static_cast<StaticImage*>(pDisplay)->setBackgroundEnabled( false );
 	// set the background image
 	static_cast<StaticImage*>(pDisplay)->setImage( strRoleImageName, UI_FULL_IMAGESET );
	static_cast<StaticImage*>(pDisplay)->setEnabled( false );
	static_cast<StaticImage*>(pDisplay)->setVisible( true );

	


	KUiCfgLoader& cfg = KUiCfgLoader::getSingleton();
	std::string imageset;
	std::string image;
	if ( pRole->byGender )
	{
		cfg.getWomanPortraitPath(pRole->byPortrait, imageset, image );
	}
	else
	{
		cfg.getManPortraitPath(pRole->byPortrait, imageset, image );
	}
	
	Window* pFaceBk = m_pThisWnd->getChild( "TaharezLook/SelRole/DiyFaceBK" );
	if ( pFaceBk )
	{
		TLStaticImage* pFace = (TLStaticImage*)pFaceBk->getChild("TaharezLook/SelRole/DiyFace");
		if ( pFace )
		{
			pFace->setFrameEnabled( false );
 			pFace->setBackgroundEnabled( false );
 			pFace->setImage( imageset.c_str(), image.c_str() );
			pFace->setEnabled( false );
			pFace->setVisible( true );
		}
	}
	
	m_roleItem[nIdx].setRole( pRole );


	// add to listbox
	char szMetier[5];
	switch ( pRole->byAttribute )  
	{
	case 0:
		snprintf( szMetier, 5, "%s", ROLE_CAREER_JS );
		break;
	case 1:
		snprintf( szMetier, 5, "%s", ROLE_CAREER_DS );
		break;
	case 2:
		snprintf( szMetier, 5, "%s", ROLE_CAREER_YR );
		break;
	default:
		return;
	}		
		
	char szLevelMetier[COMMON_CLIENT_MSG_LEN_64];
	snprintf( szLevelMetier, sizeof(szLevelMetier),  MSG_LEVEL_SELROLEWND, pRole->uLevel, szMetier );
	szLevelMetier[COMMON_CLIENT_MSG_LEN_64-1] = 0;

	char szRoleItem[COMMON_CLIENT_MSG_LEN_256];
	snprintf( szRoleItem, sizeof(szRoleItem), "%s %s", pRole->szName, szLevelMetier );
	szRoleItem[COMMON_CLIENT_MSG_LEN_256 - 1] = 0;

	ListboxTextItem* roleItem = new ListboxTextItem(AnsiToUtf8(szRoleItem), nIdx );
	if ( roleItem )
	{
		roleItem->setSelectionBrushImage( String("txtbg"), String("txtbg10") );
		m_roleListbox->addItem( roleItem );	
	}

}

void KUiSelPlayer::clearRole(int nIdx )
{
	if ( nIdx < 0 || nIdx >= MAX_PLAYER_IN_ACCOUNT )
	{
		return;
	}
	Window *pDisplay = m_pThisWnd->getChild( "TaharezLook/SelRole/Display" );
	if ( pDisplay )
	{
		static_cast<StaticImage*>(pDisplay)->hide();
	}
	m_roleItem[nIdx].clearRole();
}

bool KUiSelPlayer::handleDelete ( const CEGUI::EventArgs& args		)
{

	KRoleChiefInfo tagChiefInfo;
	memset( &tagChiefInfo, 0, sizeof( tagChiefInfo ));
	int nOk = g_LoginLogic.GetRoleInfo( m_nSelRoleIdx, &tagChiefInfo );
	if ( tagChiefInfo.bTongMember )
	{
		g_LoginLogic.CallConnectInfoBox(CI_MI_TONGMEMBER);
		return true;
	}

	if ( tagChiefInfo.dwEmployLeftTime != -1 )
	{
		g_LoginLogic.CallConnectInfoBox(CI_MI_ENGAGE_CANNT_DEL_ROLE);
		return true;
	}

	if ( tagChiefInfo.dwWillDestoryTime != -1 )
	{
		g_LoginLogic.CallConnectInfoBox(CI_MI_DELING_CANNT_DEL_ROLE);
		return true;
	}

	KUiDelComfirm::Show();
	//删除角色时删除相应配置
	KIniFile iniFile;
	iniFile.Load(m_filename);
	
	if ( nOk )
	{
		iniFile.EraseSection(tagChiefInfo.szName);
		iniFile.Save(m_filename);
	}

	return true;
}

bool KUiSelPlayer::handleEntry	 ( const CEGUI::EventArgs& args		)
{
	if ( m_nSelRoleIdx != -1 )
	{
		KRoleChiefInfo tagChiefInfo;
		if (g_LoginLogic.GetRoleInfo( m_nSelRoleIdx, &tagChiefInfo ))
		{
			IsFirstLogin(tagChiefInfo.szName);
			g_LoginLogic.SelectRoleLoginGame( m_nSelRoleIdx );
		}
		//KUiBeginHelp::GetSingleton().setRoleIdx(m_nSelRoleIdx);
	}
	return true;	
}

bool KUiSelPlayer::handleCreate( const CEGUI::EventArgs& args )
{
	if ( g_LoginLogic.GetRoleCount() >= MAX_CREATE_PLAYER_IN_ACCOUNT )
	{
		KUiWaitingMsg::GetSingleton().SetLoginStatus( CI_MI_FULLROLE );
		
		return true;
	}

	if ( g_LoginLogic.IsAnswerRight() )
	{
		Hide();
		KUiNewPlayer::Show();
		g_NetConnectAgent.RegisterMsgTargetObject( s2c_byte_extend,		NULL				);
	}
	else if ( g_LoginLogic.IsCanCreateRole() )
	{
		KUiWaitingMsg::GetSingleton().SetLoginStatus( CI_MI_ANSWER_OVERTIME );
	}
	else
	{
		g_LoginLogic.RequestQuestion();
	}
	
    return true;
}

bool KUiSelPlayer::handleBack( const CEGUI::EventArgs& args )
{
	g_LoginLogic.ReturnToLogin();
    return true;
}

bool KUiSelPlayer::handleShowList(const CEGUI::EventArgs& args)
{
	if ( m_roleListbox && m_roleListboxBG )
	{
		if (m_roleListbox->isVisible())
		{
			m_roleListbox->hide();
		}
		else
		{
			m_roleListbox->show();			
		}
		
		if ( m_roleListboxBG->isVisible() )
		{
			m_roleListboxBG->hide();
		}
		else
		{
			m_roleListboxBG->show();
		}
	}

    return true;	
}

bool KUiSelPlayer::handleKeyDown(const CEGUI::EventArgs& args)
{
    using namespace CEGUI;

    switch (static_cast<const KeyEventArgs&>(args).scancode)
    {
    case Key::Escape:
		{
			// 退到开始画面
			g_pCoreShell->OperationRequest( GOI_EXIT_GAME, 0, 0 );
			g_LoginLogic.ReturnToLogin();
		}
        break;
    case Key::Return:
		if ( m_nSelRoleIdx != -1 )
		{
			//KUiBeginHelp::GetSingleton().setRoleIdx( m_nSelRoleIdx );
			KRoleChiefInfo tagChiefInfo;
			if (g_LoginLogic.GetRoleInfo( m_nSelRoleIdx, &tagChiefInfo ))
			{
				IsFirstLogin(tagChiefInfo.szName);
				g_LoginLogic.SelectRoleLoginGame( m_nSelRoleIdx );
			}
			g_LoginLogic.SelectRoleLoginGame( m_nSelRoleIdx );
		}
        break;
	case Key::Tab:
		{
			static int nSel = 0;
			if ( nSel >= 0  && nSel < m_roleCurCount )
			{
				m_roleItem[nSel].handleRole( args );
				++nSel;
			}
			else
			{
				nSel = 0;
				m_roleItem[nSel].handleRole( args );
			}
			
		}
		break;
    default:
        return false;
    }
	
    return true;
}

void KUiSelPlayer::selectRole( int nRoleKind )
{
	if ( nRoleKind != 0 && nRoleKind != 1 && nRoleKind != 2 )
	{
		return;
	}
	Window *pImage = NULL;
	pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/SelRole/DSImage");		
	pImage->hide();
	pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/SelRole/JSImage");		
	pImage->hide();
	pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/SelRole/YRImage");		
	pImage->hide();
	switch( nRoleKind )
	{
	case 0:
		{
			pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/SelRole/JSImage");		
			pImage->show();
		}
		break;
	case 1:
		{
			pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/SelRole/DSImage");		
			pImage->show();
		}
	    break;
	case 2:
		{
			pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/SelRole/YRImage");		
			pImage->show();
		}
	    break;
	}
}


//likun
void KUiSelPlayer::setFileName( char *iniFileName )
{
	strcpy(m_filename, iniFileName);
}

//根据角色名判断是否是第一次登陆
void KUiSelPlayer::IsFirstLogin( const char *roleName )
{
	int iFirstLogin = 0;
	KIniFile iniFile;
	iniFile.Load(m_filename);
	if ( roleName != NULL)
	{
		iniFile.GetInteger( roleName, ROLE_FIRST_LOGIN, 0, &iFirstLogin );
		//KUiBeginHelp::GetSingleton().setbRoleFirstLogin( static_cast<bool>(iFirstLogin), m_filename, roleName );
		KUIGlobalEvent::getSingleton().setLoginInfo( m_filename, roleName, !iFirstLogin );
	}
}

void KUiSelPlayer::setNewRoleName( char *newName )
{
	if ( newName )
	{
		memcpy( m_newRoleName, newName, COMMON_CLIENT_MSG_LEN_32 );
	}
}

bool KUiSelPlayer::handleFirstRole( void )	
{
	EventArgs arg;
	ZeroMemory( &arg, sizeof(arg) );
	m_roleItem[0].handleRole(arg);
	return 0;
}

void KUiSelPlayer::clearRadioBtn( void )
{
	for ( int nIdx = 0; nIdx < m_roleCurCount; ++nIdx )
	{
		RadioButton* btn = (RadioButton* )m_roleItem[nIdx].getFront();
		if ( btn )
		{
			btn->setSelected( false );
		}
	}
}

bool	KUiSelPlayer::handleListBoxClick( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pWinEvent = (WindowEventArgs*)&args;
	if ( pWinEvent )
	{
		ListboxItem* lItem = static_cast<Listbox*>(pWinEvent->window)->getFirstSelectedItem();
		
		if (lItem )
		{
			int nSel = lItem->getID();
			if (nSel >= 0 && nSel <  m_roleCurCount )
			{
				m_roleItem[nSel].handleRole(args);
				if ( m_proleListScrol && m_roleCurCount > 0 )
				{
					float pos = nSel;
					if ( nSel == 0 )
					{
						m_proleListScrol->setScrollPosition(0.0f);
					}
					else if ( nSel == m_roleCurCount - 1 )
					{
						m_proleListScrol->setScrollPosition(1.0f);
					}
					else
					{
						pos = pos / m_roleCurCount;
						m_proleListScrol->setScrollPosition(pos);
					}
				}
			}
		}
	}

	return true;
}

bool	KUiSelPlayer::handleDListBoxClick( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pWinEvent = (WindowEventArgs*)&args;
	if ( pWinEvent )
	{
		ListboxItem* lItem = static_cast<Listbox*>(pWinEvent->window)->getFirstSelectedItem();
		
		if (lItem )
		{
			int nSel = lItem->getID();
			if (nSel >= 0 && nSel <  MAX_PLAYER_IN_ACCOUNT )
			{
				m_roleItem[nSel].handleDCRole(args);
			}
		}
	}
	return true;
}


bool KUiSelPlayer::handleTreeScroll( const CEGUI::EventArgs& args )
{
	WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;
	float sparef = m_pRoleListWnd->getHeight(Absolute);
	float clipper = m_pRoleListWndCliper->getHeight(Absolute);
	float yPos = 0;
	if (scrollCtrl->window == m_proleListScrol)
	{	
		float scrollPos = m_proleListScrol->getScrollPosition();
		
		if ( (sparef > clipper ) )
		{
			yPos = (sparef - clipper) * scrollPos;
			if ( yPos >= 0 )
			{
				Point pos;
				pos.d_x = m_pRoleListWnd->getPosition(Absolute).d_x;
				pos.d_y = 0 - yPos;
				m_pRoleListWnd->setPosition( Absolute, pos );
			}

		}
		else
		{
			m_pRoleListWnd->setPosition( Absolute, Point(0, 0));
		}
	}
	return true;
}