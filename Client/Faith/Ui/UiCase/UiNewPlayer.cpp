//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/15/2006 13:21
//      File_base        : UiNewPlayer
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KWin32Wnd.h"
#include "CoreShell.h"
#include "UiNewPlayer.h"
#include "UiSelPlayer.h"
#include "CoreUseNameDef.h"
#include "../KMessageCentre.h"
#include "../UiConfigManager.h"
#include "UiWaitingMsg.h"
#include "../UiElem/TLRadioButton.h"
#include "../UiElem/TLVertScrollbar.h"

extern iCoreShell*		g_pCoreShell;

using namespace CEGUI;

/************************************************************************/
/*                                                                      */
/************************************************************************/

const unsigned int	KUiNewPlayerInfo::CreateButtonID	= UI_NEWROLE_NEW;
const unsigned int	KUiNewPlayerInfo::CancelButtonID	= UI_NEWROLE_CANCEL;
const String Con_RolePortraiLs = "uisettings/layouts/RolePortrait.ls";

template<> 
KUiNewPlayerInfo* KUiWndSingleton<KUiNewPlayerInfo>::ms_Singleton	= NULL;

KUiNewPlayerInfo::KUiNewPlayerInfo( const CEGUI::String& id_name ):
KUiWndSingleton<KUiNewPlayerInfo>( id_name )
{
	m_pThisWnd = ms_Singleton->m_pWindowManager->loadWindowLayout( ms_Singleton->m_strPath );
	if ( m_pThisWnd )
	{
		// Do events wire-up
 		m_pThisWnd->getChild(CreateButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNewPlayerInfo::handleCreate, this));
		m_pThisWnd->getChild(CancelButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiNewPlayerInfo::handleBack, this));
		m_pThisWnd->getChild("TaharezLook/NewRoleInfo/RoleName")->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiNewPlayerInfo::handleKeyDown, this));
		m_pThisWnd->getChild( _TT("TaharezLook/NewRoleInfo/RoleName") )->beginUpdate();
		ZeroMemory( &m_pNewRole, sizeof(m_pNewRole));
	}
}

KUiNewPlayerInfo::~KUiNewPlayerInfo()
{
	
}

void KUiNewPlayerInfo::SetRolePortrait(int nPortait )
{
	m_pNewRole.byPortrait = nPortait;
}

void KUiNewPlayerInfo::SetRoleInfo( int nGenre, BYTE byAttribute )
{
	m_pNewRole.byGender			= nGenre;
	m_pNewRole.byAttribute		= byAttribute;
	m_pThisWnd->getChild("TaharezLook/NewRoleInfo/RoleName")->activate();
	switch( byAttribute )
	{
	case 0:
		{
			m_pNewRole.uNativePlaceId = 1;
		}
		break;
	case 1:
		{
			m_pNewRole.uNativePlaceId = 2;
		}
	    break;
	case 2:
		{
			m_pNewRole.uNativePlaceId = 3;
		}
	    break;
	}
}

void KUiNewPlayerInfo::SetRoleNameActive( void )
{
	m_pThisWnd->getChild("TaharezLook/NewRoleInfo/RoleName")->activate();
}

void KUiNewPlayerInfo::Show( void )
{
	KUiWndSingleton<KUiNewPlayerInfo>::Show();
	if (ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setRenderMode(false,3);
		ms_Singleton->m_pThisWnd->getChild( _TT("TaharezLook/NewRoleInfo/RoleName") )->setText("");
		ms_Singleton->m_pThisWnd->getChild( _TT("TaharezLook/NewRoleInfo/RoleName") )->activate();
		//ms_Singleton->m_pThisWnd->getChild( _TT("TaharezLook/NewRoleInfo/RoleName") )->captureInput();
		ms_Singleton->m_pThisWnd->getChild( _TT("TaharezLook/NewRoleInfo/RoleName") )->beginUpdate();
	}
}

void KUiNewPlayerInfo::Hide( void )
{
	//ms_Singleton->m_pThisWnd->getChild( _TT("TaharezLook/NewRoleInfo/RoleName") )->releaseInput();
	KUiWndSingleton<KUiNewPlayerInfo>::Hide();
	ms_Singleton->m_pThisWnd->getChild( _TT("TaharezLook/NewRoleInfo/RoleName") )->stopUpdate();
}

bool KUiNewPlayerInfo::handleCreate( const CEGUI::EventArgs& args )
{
	/*KIniFile iniFile;
	char	 cBeginLogin[COMMON_CLIENT_MSG_LEN_128] = "BFirstLogin";
	iniFile.Load(MAP_SETTING_FILE);*/

	String strNewName = m_pThisWnd->getChild( _TT("TaharezLook/NewRoleInfo/RoleName") )->getText();
	strncpy( m_pNewRole.szName, Utf8ToAnsi( strNewName ), COMMON_CLIENT_MSG_LEN_32 );

	if ( CherkInputInfo() )
	{
		g_LoginLogic.CreateRoleAtServer( &m_pNewRole );
		KUiSelPlayer::GetSingletonPtr()->setNewRoleName( m_pNewRole.szName );
		//likun  初始化第一次登陆时看到界面状态
		KIniFile iniFile;
		if(iniFile.Load(m_filename))
		{
			iniFile.WriteInteger(m_pNewRole.szName, ROLE_FIRST_LOGIN, 0 );
			iniFile.Save(m_filename);
		}
		return true;
	}
    return false;
}

bool	KUiNewPlayerInfo::handleKeyDown	( const CEGUI::EventArgs& args	)
{
    using namespace CEGUI;
    switch (static_cast<const KeyEventArgs&>(args).scancode)
    {
	case Key::Escape:
		if ( g_GetMainApp() )
		{
			//g_GetMainApp()->StopApplication();
			g_pCoreShell->OperationRequest( GOI_EXIT_GAME, 0, 0 );
			g_LoginLogic.ReturnToLogin();
		}
        break;
    case Key::Return:
		{
			if ( handleCreate(args) )
				return true;
		}
        break;
    default:
        return false;
    }

    return true;
}

bool KUiNewPlayerInfo::CherkInputInfo( void )
{
	int nLen = strlen( m_pNewRole.szName );
	if ( nLen < LOGIN_ROLE_NAME_MIN_LEN || nLen > LOGIN_ROLE_NAME_MAX_LEN )
	{
		KUiWaitingMsg::GetSingleton().SetLoginStatus( CI_MI_INVALID_LOGIN_INPUT2 );
		
		return false;
	}
	
	if ( !g_pCoreShell->IsNamePass( m_pNewRole.szName ) )
	{
		KUiWaitingMsg::GetSingleton().SetLoginStatus( CI_MI_INVALID_LOGIN_INPUT1 );
		
		return false;
	}

	return true;
} 


bool KUiNewPlayerInfo::handleBack( const CEGUI::EventArgs& args )
{
	if ( g_LoginLogic.GetRoleCount() > 0 )
	{
		KUiNewPlayer::Hide();
		KUiSelPlayer::Show();
		KUiSelPlayer::GetSingletonPtr()->handleFirstRole();

	}
	else
	{
		g_LoginLogic.ReturnToLogin();
	}

    return true;
}

/*void KUiNewPlayerInfo::SetRoleNum( int iNum )
{
	m_iRoleNum = iNum;
}*/

void KUiNewPlayerInfo::setFileName( char *iniFileName )
{
	strcpy(m_filename, iniFileName);
}
/************************************************************************/
/*                                                                      */
/************************************************************************/

const unsigned int	KUiNewPlayer::BackButtonID		= UI_NEWROLE_BACK;
template<> 
KUiNewPlayer* KUiWndSingleton<KUiNewPlayer>::ms_Singleton	= NULL;

KUiNewPlayer::KUiNewPlayer( const CEGUI::String& id_name )
: KUiWndSingleton<KUiNewPlayer>( id_name )
, d_pRoleImage(NULL)
, d_pClickRoleImage(NULL)
{
	d_curPortrait = 0;
}

KUiNewPlayer::~KUiNewPlayer()
{
	KUiNewPlayerInfo::DestroyWindow();
}




bool KUiNewPlayer::handleKeyDown(const CEGUI::EventArgs& args)
{
    using namespace CEGUI;

    switch (static_cast<const KeyEventArgs&>(args).scancode)
    {
    case Key::Escape:
		if ( g_GetMainApp() )
		{
			//g_GetMainApp()->StopApplication();
			g_pCoreShell->OperationRequest( GOI_EXIT_GAME, 0, 0 );
			g_LoginLogic.ReturnToLogin();
		}
        break;
    case Key::Return:
        break;
    default:
        return false;
    }

    return true;
}

void KUiNewPlayer::Init( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setZLevel(Window::SuperBottom);
		// Do events wire-up
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/JS1")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::HandleRoleA, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/JS0")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::HandleRoleB, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/DS1")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::HandleRoleC, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/DS0")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::HandleRoleD, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/YR1")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::HandleRoleE, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/YR0")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::HandleRoleF, ms_Singleton));

		ms_Singleton->d_roleFace = (TLStaticImage*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/DiyFaceBK")->getChild("TaharezLook/NewRole/DiyFace");

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/DiyFaceBK")->getChild("TaharezLook/NewRole/DiyFaceBtnLeft")
			->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiNewPlayer::HandleRoleSelectPortraitL, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/DiyFaceBK")->getChild("TaharezLook/NewRole/DiyFaceBtnRight")
			->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiNewPlayer::HandleRoleSelectPortraitR, ms_Singleton));

		ms_Singleton->m_pThisWnd->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiNewPlayer::handleKeyDown, ms_Singleton));

		d_pRoleImage		= static_cast<TLStaticImage*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/RoleCartoon"));
		d_pClickRoleImage	= static_cast<TLStaticImage*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/RoleCartoonAnim"));			
		initRolePortrait();
	}
}



void KUiNewPlayer::Show( void )
{
	KUiWndSingleton<KUiNewPlayer>::Show();

	if ( ms_Singleton->m_pThisWnd )
	{
		/*
		// load an image to use as a background
		try
		{
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_NEWROLEBK_IMAGESET_NAME, UI_SELROLEBK_IMAGE_PATH );
		}
		catch (...)
		{
				
		}

 		// set area rectangle
 		static_cast<StaticImage*>(ms_Singleton->m_pThisWnd)->setRect( Relative, Rect(0, 0, 1, 1) );
 		// disable frame and standard background
 		static_cast<StaticImage*>(ms_Singleton->m_pThisWnd)->setFrameEnabled( false );
 		static_cast<StaticImage*>(ms_Singleton->m_pThisWnd)->setBackgroundEnabled( false );
 		// set the background image
 		static_cast<StaticImage*>(ms_Singleton->m_pThisWnd)->setImage( UI_NEWROLEBK_IMAGESET_NAME, UI_FULL_IMAGESET );

		Window* pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/JSImage");
 		// set area rectangle
 		static_cast<StaticImage*>(pImage)->setRect(Relative, Rect(0, 0, 1, 1));
 		// disable frame and standard background
 		static_cast<StaticImage*>(pImage)->setFrameEnabled(false);
 		static_cast<StaticImage*>(pImage)->setBackgroundEnabled(true);
 		// set the background image
 		static_cast<StaticImage*>(pImage)->setImage( UI_LOGINBK_IMAGESET_NAME_JS, UI_FULL_IMAGESET );
		
		pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/DSImage");
 		// set area rectangle
 		static_cast<StaticImage*>(pImage)->setRect(Relative, Rect(0, 0, 1, 1));
 		// disable frame and standard background
 		static_cast<StaticImage*>(pImage)->setFrameEnabled(false);
 		static_cast<StaticImage*>(pImage)->setBackgroundEnabled(true);
 		// set the background image
 		static_cast<StaticImage*>(pImage)->setImage( UI_LOGINBK_IMAGESET_NAME_DS, UI_FULL_IMAGESET );

		pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/YRImage");
		// set area rectangle
 		static_cast<StaticImage*>(pImage)->setRect(Relative, Rect(0, 0, 1, 1));
 		// disable frame and standard background
 		static_cast<StaticImage*>(pImage)->setFrameEnabled(false);
 		static_cast<StaticImage*>(pImage)->setBackgroundEnabled(true);
 		// set the background image
 		static_cast<StaticImage*>(pImage)->setImage( UI_LOGINBK_IMAGESET_NAME_YR, UI_FULL_IMAGESET );
		//ms_Singleton->showSelRole( UI_LOGINBK_IMAGESET_NAME_CREATE_JS_0, UI_LOGINBK_IMAGE_PATH_CREATE_JS_0);
		//*/
	}

	g_pCoreShell->PlayUiLoginDisplayerNpc();

	KUiNewPlayerInfo::Hide();
	KUiNewPlayerInfo::Show();
	//KUiNewPlayerInfo::GetSingletonPtr()->SetRoleInfo( 0, 0 );
	//ms_Singleton->selectRole( 2, 1 );
	//ms_Singleton->selectRole( 1, 1 );
	//ms_Singleton->selectRole( 0, 1 );
	CEGUI::EventArgs arg;
	ms_Singleton->HandleRoleA(arg);
}

void KUiNewPlayer::Hide( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd && ms_Singleton->d_SelPortraitWnd)
	{
		ms_Singleton->d_pRoleImage->stop();
		ms_Singleton->d_SelPortraitWnd->hide();
	}
	g_pCoreShell->StopUiLoginDisplayerNpc();
	KUiWndSingleton<KUiNewPlayer>::Hide();
	KUiNewPlayerInfo::Hide();	
}

/************************************************************************/
/* 选择一个头像设置相关状态     
 * 1.KUiNewPlayerInfo.m_pNewRole.byPortrait  
 * 2.新建角色的头像图片设置
/************************************************************************/
void KUiNewPlayer::SelectPortrait( int nGender, int nPortrait )
{
	KUiNewPlayerInfo::GetSingleton().SetRolePortrait( nPortrait );
	KUiCfgLoader& cfg = KUiCfgLoader::getSingleton();
	std::string imageset;
	std::string image;
	if ( !nGender )
	{
		cfg.getManPortraitPath(nPortrait, imageset, image );
	}
	else
	{
		cfg.getWomanPortraitPath(nPortrait, imageset, image );
	}
	
	d_roleFace->setImage( imageset, image );
}

bool KUiNewPlayer::HandleRoleA( const CEGUI::EventArgs& args )
{
	KUiNewPlayerInfo::GetSingletonPtr()->SetRoleInfo( 0, 0 );
	selectRole( 0, 1 );
	showClickRole( UI_LOGINBK_IMAGESET_NAME_CLICK_JS_0, UI_LOGINBK_IMAGE_PATH_CLICK_JS_0 );
	showSelRole( UI_LOGINBK_IMAGESET_NAME_CREATE_JS_0, UI_LOGINBK_IMAGE_PATH_CREATE_JS_0);
	RadioButton* rb = static_cast<RadioButton*>(m_pThisWnd->getChild("TaharezLook/NewRole/JS1"));
    rb->setSelected(true);
	SelectPortrait(0,0);
	SelectAllPortrait(0);
	return true;
}

bool KUiNewPlayer::HandleRoleB( const CEGUI::EventArgs& args )
{
	KUiNewPlayerInfo::GetSingletonPtr()->SetRoleInfo( 1, 0 );
	selectRole( 0, 0 );
	showClickRole( UI_LOGINBK_IMAGESET_NAME_CLICK_JS_1, UI_LOGINBK_IMAGE_PATH_CLICK_JS_1 );
	showSelRole( UI_LOGINBK_IMAGESET_NAME_CREATE_JS_1, UI_LOGINBK_IMAGE_PATH_CREATE_JS_1 );
	RadioButton* rb = static_cast<RadioButton*>(m_pThisWnd->getChild("TaharezLook/NewRole/JS0"));
    rb->setSelected(true);
	SelectPortrait(1,0);
	SelectAllPortrait(1);
    return true;
}

bool KUiNewPlayer::HandleRoleC( const CEGUI::EventArgs& args )
{
	KUiNewPlayerInfo::GetSingletonPtr()->SetRoleInfo( 0, 1 );
	selectRole( 1, 1 );
	showClickRole( UI_LOGINBK_IMAGESET_NAME_CLICK_DS_0, UI_LOGINBK_IMAGE_PATH_CLICK_DS_0 );
	showSelRole( UI_LOGINBK_IMAGESET_NAME_CREATE_DS_0, UI_LOGINBK_IMAGE_PATH_CREATE_DS_0 );
	RadioButton* rb = static_cast<RadioButton*>(m_pThisWnd->getChild("TaharezLook/NewRole/DS1"));
    rb->setSelected(true);
	SelectPortrait(0,0);
	SelectAllPortrait(0);
    return true;
}

bool KUiNewPlayer::HandleRoleD( const CEGUI::EventArgs& args )
{
	KUiNewPlayerInfo::GetSingletonPtr()->SetRoleInfo( 1, 1 );
	selectRole( 1, 0 );
	showClickRole( UI_LOGINBK_IMAGESET_NAME_CLICK_DS_1, UI_LOGINBK_IMAGE_PATH_CLICK_DS_1 );
	showSelRole( UI_LOGINBK_IMAGESET_NAME_CREATE_DS_1, UI_LOGINBK_IMAGE_PATH_CREATE_DS_1 );
	
	RadioButton* rb = static_cast<RadioButton*>(m_pThisWnd->getChild("TaharezLook/NewRole/DS0"));
    rb->setSelected(true);
	SelectPortrait(1,0);
	SelectAllPortrait(1);
    return true;
}

bool KUiNewPlayer::HandleRoleE( const CEGUI::EventArgs& args )
{
	KUiNewPlayerInfo::GetSingletonPtr()->SetRoleInfo( 0, 2 );
	selectRole( 2, 1 );
	showClickRole( UI_LOGINBK_IMAGESET_NAME_CLICK_YR_0, UI_LOGINBK_IMAGE_PATH_CLICK_YR_0 );
	showSelRole( UI_LOGINBK_IMAGESET_NAME_CREATE_YR_0, UI_LOGINBK_IMAGE_PATH_CREATE_YR_0 );
	RadioButton* rb = static_cast<RadioButton*>(m_pThisWnd->getChild("TaharezLook/NewRole/YR1"));
    rb->setSelected(true);
	SelectPortrait(0,0);
	SelectAllPortrait(0);
    return true;
}

bool KUiNewPlayer::HandleRoleF( const CEGUI::EventArgs& args )
{
	KUiNewPlayerInfo::GetSingletonPtr()->SetRoleInfo( 1, 2 );
	selectRole( 2, 0 );
	showClickRole( UI_LOGINBK_IMAGESET_NAME_CLICK_YR_1, UI_LOGINBK_IMAGE_PATH_CLICK_YR_1 );
	showSelRole( UI_LOGINBK_IMAGESET_NAME_CREATE_YR_1, UI_LOGINBK_IMAGE_PATH_CREATE_YR_1 );
	RadioButton* rb = static_cast<RadioButton*>(m_pThisWnd->getChild("TaharezLook/NewRole/YR0"));
    rb->setSelected(true);
	SelectPortrait(1,0);
	SelectAllPortrait(1);
    return true;
}

void KUiNewPlayer::selectRole( int nRoleKind, int nGender )
{	
	Window *pImage = NULL;
	pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/DSImage");		
	pImage->hide();
	pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/JSImage");		
	pImage->hide();
	pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/YRImage");		
	pImage->hide();

	Window* pParent = m_pThisWnd->getChild( "TaharezLook/NewRole/RoleA" );
	if ( pParent )
	{
		pParent->setAlpha( 0.6f );
	}
	switch( nRoleKind )
	{
	case 0:
		{
			pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/JSImage");		
			pImage->show();
			Window* pParent = m_pThisWnd->getChild( "TaharezLook/NewRole/RoleA" );
			if ( pParent )
			{
				if ( nGender )
				{
					TLStaticText* pInfo = (TLStaticText*)pParent->getChild( "TaharezLook/NewRole/RoleA/Info0" );
					if ( pInfo )
					{
						pInfo->useLayout();
						//裁剪区域必须是相对底板的位置
						Rect textArea = pInfo->getUnclippedInnerRect();
						Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 						textArea.setPosition(posOff);

						LORect clipper;
						cerectToLorect(&textArea, &clipper);
						pInfo->getLayout()->setClipper(clipper);
						pInfo->getLayout()->SetText( KMessageCentre::GetMessage( role_description_message, 0 ) );
					}

					UI_DISPLAYER_NPC_SYNC npc;
					ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
					npc.m_Doing = do_stand;			
					npc.m_btKind = kind_player;				
					npc.NpcSettingIdx	= -1;
					npc.nLevel = 1;
					g_pCoreShell->SetUiLoginDisplayerNpc( &npc );
				}
				else
				{
					TLStaticText* pInfo = (TLStaticText*)pParent->getChild( "TaharezLook/NewRole/RoleA/Info0" );
					if ( pInfo )
					{
						pInfo->useLayout();
						//裁剪区域必须是相对底板的位置
						Rect textArea = pInfo->getUnclippedInnerRect();
						Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 						textArea.setPosition(posOff);

						LORect clipper;
						cerectToLorect(&textArea, &clipper);
						pInfo->getLayout()->setClipper(clipper);
						pInfo->getLayout()->SetText( KMessageCentre::GetMessage( role_description_message, 1 ) );
					}

					UI_DISPLAYER_NPC_SYNC npc;
					ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
					npc.m_Doing = do_stand;			
					npc.m_btKind = kind_player;				
					npc.NpcSettingIdx	= -4;
					npc.nLevel = 1;
					g_pCoreShell->SetUiLoginDisplayerNpc( &npc );

				}
				pParent->setAlpha( 0 );
			}
		}
		break;
	case 1:
		{
			pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/DSImage");		
			pImage->show();
			Window* pParent = m_pThisWnd->getChild( "TaharezLook/NewRole/RoleA" );
			if ( pParent )
			{
				if ( nGender )
				{
					TLStaticText* pInfo = (TLStaticText*)pParent->getChild( "TaharezLook/NewRole/RoleA/Info0" );
					if ( pInfo )
					{
						pInfo->useLayout();
						//裁剪区域必须是相对底板的位置
						Rect textArea = pInfo->getUnclippedInnerRect();
						Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 						textArea.setPosition(posOff);

						LORect clipper;
						cerectToLorect(&textArea, &clipper);
						pInfo->getLayout()->setClipper(clipper);
						pInfo->getLayout()->SetText( KMessageCentre::GetMessage( role_description_message, 2 ) );
					}

					UI_DISPLAYER_NPC_SYNC npc;
					ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
					npc.m_Doing = do_stand;			
					npc.m_btKind = kind_player;				
					npc.NpcSettingIdx	= -2;
					npc.nLevel = 1;
					g_pCoreShell->SetUiLoginDisplayerNpc( &npc );

				}
				else
				{
					TLStaticText* pInfo = (TLStaticText*)pParent->getChild( "TaharezLook/NewRole/RoleA/Info0" );
					if ( pInfo )
					{
						pInfo->useLayout();
						//裁剪区域必须是相对底板的位置
						Rect textArea = pInfo->getUnclippedInnerRect();
						Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 						textArea.setPosition(posOff);

						LORect clipper;
						cerectToLorect(&textArea, &clipper);
						pInfo->getLayout()->setClipper(clipper);
						pInfo->getLayout()->SetText( KMessageCentre::GetMessage( role_description_message, 3 ) );
					}
					UI_DISPLAYER_NPC_SYNC npc;
					ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
					npc.m_Doing = do_stand;			
					npc.m_btKind = kind_player;				
					npc.NpcSettingIdx	= -5;
					npc.nLevel = 1;
					g_pCoreShell->SetUiLoginDisplayerNpc( &npc );

				}
				pParent->setAlpha( 0 );
			}
		}
	    break;
	case 2:
		{
			pImage = ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/YRImage");		
			pImage->show();
			Window* pParent = m_pThisWnd->getChild( "TaharezLook/NewRole/RoleA" );
			if ( pParent )
			{
				if ( nGender )
				{
					TLStaticText* pInfo = (TLStaticText*)pParent->getChild( "TaharezLook/NewRole/RoleA/Info0" );
					if ( pInfo )
					{
						pInfo->useLayout();
						//裁剪区域必须是相对底板的位置
						Rect textArea = pInfo->getUnclippedInnerRect();
						Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 						textArea.setPosition(posOff);

						LORect clipper;
						cerectToLorect(&textArea, &clipper);
						pInfo->getLayout()->setClipper(clipper);
						pInfo->getLayout()->SetText( KMessageCentre::GetMessage( role_description_message, 4 ) );
					}
					UI_DISPLAYER_NPC_SYNC npc;
					ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
					npc.m_Doing = do_stand;			
					npc.m_btKind = kind_player;				
					npc.NpcSettingIdx	= -3;
					npc.nLevel = 1;
					g_pCoreShell->SetUiLoginDisplayerNpc( &npc );

				}
				else
				{
					TLStaticText* pInfo = (TLStaticText*)pParent->getChild( "TaharezLook/NewRole/RoleA/Info0" );
					if ( pInfo )
					{
						pInfo->useLayout();
						//裁剪区域必须是相对底板的位置
						Rect textArea = pInfo->getUnclippedInnerRect();
						Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 						textArea.setPosition(posOff);

						LORect clipper;
						cerectToLorect(&textArea, &clipper);
						pInfo->getLayout()->setClipper(clipper);
						pInfo->getLayout()->SetText( KMessageCentre::GetMessage( role_description_message, 5 ) );
					}
					UI_DISPLAYER_NPC_SYNC npc;
					ZeroMemory( &npc, sizeof(UI_DISPLAYER_NPC_SYNC) );
					npc.m_Doing = do_stand;			
					npc.m_btKind = kind_player;				
					npc.NpcSettingIdx	= -6;
					npc.nLevel = 1;
					g_pCoreShell->SetUiLoginDisplayerNpc( &npc );

				}
			}
			pParent->setAlpha( 0 );
		}
	    break;
	}
}

void KUiNewPlayer::showSelRole( const CEGUI::String& name, const CEGUI::String& path )
{

	//StaticImage* role =	static_cast<StaticImage*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/RoleCartoon"));
 	// set area rectangle
//	static_cast<StaticImage*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/RoleCartoon"))->setSize( Absolute, Size( 1024, 760 ) );
//	static_cast<StaticImage*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/NewRole/RoleCartoon"))->setPosition( Absolute, Point( 0, 8 ) );
	d_pRoleImage->setEnabled( false );

 	// disable frame and standard background
	d_pRoleImage->setFrameEnabled(false);
 	
	d_pRoleImage->setCyc( true );	
	// set the background image
 	d_pRoleImage->setImage( name, UI_FULL_IMAGESET );
	d_pClickRoleImage->setNextImage(d_pRoleImage);
	// play the cartoon
	d_pRoleImage->hide();
	//d_pRoleImage->play();
}

void KUiNewPlayer::showClickRole( const CEGUI::String& name, const CEGUI::String& path )
{
	d_pClickRoleImage->setEnabled(false);
	d_pClickRoleImage->setCyc(false);	
	d_pClickRoleImage->setImage( name, UI_FULL_IMAGESET );
	d_pClickRoleImage->play(true);
}

bool KUiNewPlayer::HandleRoleSelectPortraitL( const CEGUI::EventArgs& args	)
{

	d_SelPortraitWnd->show();
	d_SelPortraitWnd->setModalState(true);

	/*KUiCfgLoader& cfg = KUiCfgLoader::getSingleton();
	int faceCount = 0;
	if ( KUiNewPlayerInfo::GetSingleton().m_pNewRole.byGender == 0 )
	{
		faceCount = cfg.getManPortraitCount();
	}
	else
	{
		faceCount = cfg.getManPortraitCount();
	}	
	KUiNewPlayerInfo::GetSingleton().SetRoleNameActive();
	if ( d_curPortrait >= 0 && d_curPortrait < faceCount-1 )
	{
		d_curPortrait++;
		SelectPortrait( KUiNewPlayerInfo::GetSingleton().m_pNewRole.byGender, d_curPortrait );
	}*/

	return true;
}

bool KUiNewPlayer::HandleRoleSelectPortraitR( const CEGUI::EventArgs& args	)
{

	//d_SelPortraitWnd->hide();

	KUiCfgLoader& cfg = KUiCfgLoader::getSingleton();
	int faceCount = 0;
	if ( KUiNewPlayerInfo::GetSingleton().m_pNewRole.byGender == 0 )
	{
		faceCount = cfg.getManPortraitCount();
	}
	else
	{
		faceCount = cfg.getManPortraitCount();
	}
	KUiNewPlayerInfo::GetSingleton().SetRoleNameActive();
	if ( d_curPortrait > 0 && d_curPortrait < faceCount )
	{
		d_curPortrait--;
		SelectPortrait( KUiNewPlayerInfo::GetSingleton().m_pNewRole.byGender, d_curPortrait );
	}
	return true;

}


/************************************************************************/
/* 初始化选择角色头像窗口，注册点击单选钮事件
 * 装载窗口配置文件：RolePortrait.ls，创建选择头像窗口                                            
/************************************************************************/
void KUiNewPlayer::initRolePortrait() 
{
	d_SelPortraitWnd = ms_Singleton->m_pWindowManager->loadWindowLayout( Con_RolePortraiLs );
	assert( d_SelPortraitWnd );
	d_SelPortraitWnd->setRenderMode(true);	

	if(d_SelPortraitWnd) {
		ms_Singleton->m_pRootSheet->addChildWindow( d_SelPortraitWnd );
	}
	d_SelPortraitWnd->hide();	
	
	Window* Portraitclip = d_SelPortraitWnd->getChild("Clip_Image_2");
	Window* Portraitpanel = Portraitclip->getChild("Panel_Image_3");
	

	Portraitpanel->getChild("Portrait_Radio1-1")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));
	Portraitpanel->getChild("Portrait_Radio1-2")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));
	Portraitpanel->getChild("Portrait_Radio1-3")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));

	Portraitpanel->getChild("Portrait_Radio2-1")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));
	Portraitpanel->getChild("Portrait_Radio2-2")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));
	Portraitpanel->getChild("Portrait_Radio2-3")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));

	Portraitpanel->getChild("Portrait_Radio3-1")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));
	Portraitpanel->getChild("Portrait_Radio3-2")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));
	Portraitpanel->getChild("Portrait_Radio3-3")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));

	Portraitpanel->getChild("Portrait_Radio4-1")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));
	Portraitpanel->getChild("Portrait_Radio4-2")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));
	Portraitpanel->getChild("Portrait_Radio4-3")->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiNewPlayer::onClickPortrait, ms_Singleton));

	d_SelPortraitWnd->getChild("List_Scrollbar_2")->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiNewPlayer::onRolePortraitPanelScroll, this));
	Portraitpanel->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiNewPlayer::onRolePortraitMouseWheel, this));
	
	//SelectAllPortrait(KUiNewPlayerInfo::GetSingleton().m_pNewRole.byGender);
	SelectPortrait(0,0);
}

/************************************************************************/
/* 1.根据所选择的性别，设置选择头像窗口的头像列表图片
 * 2.当鼠标单击各个职业男女头像时，调用此方法更新头像图片列表
/************************************************************************/
void KUiNewPlayer::SelectAllPortrait( int nGender)
{
	Window* Portraitclip = d_SelPortraitWnd->getChild("Clip_Image_2");
	Window* Portraitpanel = Portraitclip->getChild("Panel_Image_3");

	TLStaticImage* pImage[COMMON_CLIENT_MSG_LEN_16];
	
	KUiCfgLoader& cfg = KUiCfgLoader::getSingleton();
	int faceCount = cfg.getManPortraitCount();
	
	std::string imageset;
	std::string image;
	
	char szFaceName[COMMON_CLIENT_MSG_LEN_32];
	memset(szFaceName, 0, sizeof(szFaceName));
	
	//初始化头像
	for (int i = 0; i < faceCount; i++)
	{
		sprintf(szFaceName, "Portrait_RadioFace%d", i+1);
		pImage[i] = static_cast<TLStaticImage*>(Portraitpanel->getChild(szFaceName));
		
		if ( KUiNewPlayerInfo::GetSingleton().m_pNewRole.byGender == 0 ) 
		{
			cfg.getManPortraitPath(i, imageset, image );		
		} 
		else 
		{
			cfg.getWomanPortraitPath(i, imageset, image );
		}
		pImage[i]->setImage(imageset, image);		
	}

	//
	for (int j = faceCount; j < 12; j++) {
		sprintf(szFaceName, "Portrait_RadioFace%d", j+1);
		pImage[j] = static_cast<TLStaticImage*>(Portraitpanel->getChild(szFaceName));

		if (pImage[j]) {			
			pImage[j]->setVisible(false);
			//pImage[j]->disable();
		}
	
	}

}

/************************************************************************/
/* 操作帮助内容鼠标垂直滚轮事件                                                                      
/************************************************************************/
bool KUiNewPlayer::onRolePortraitMouseWheel( const CEGUI::EventArgs& args ) 
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
	
	TLVertScrollbar* PortraitScroll = static_cast<TLVertScrollbar *>(d_SelPortraitWnd->getChild("List_Scrollbar_2"));

	if(PortraitScroll->isVisible())
	{ 
		PortraitScroll->setScrollPosition(PortraitScroll->getScrollPosition() - (PortraitScroll->getStepSize() * eventArgs->wheelChange));
	}
	return true;		
}

/************************************************************************/
/* 选择角色头像滚动条事件
/************************************************************************/
bool KUiNewPlayer::onRolePortraitPanelScroll( const CEGUI::EventArgs& args ) 
{
	Window* Portraitclip = d_SelPortraitWnd->getChild("Clip_Image_2");
	Window* Portraitpanel = Portraitclip->getChild("Panel_Image_3");
	TLVertScrollbar* PortraitScroll = static_cast<TLVertScrollbar *>(d_SelPortraitWnd->getChild("List_Scrollbar_2"));

	int clipHeight = Portraitclip->getHeight(Absolute);
	int panelHeight = Portraitpanel->getHeight(Absolute);
	float scrollPos = PortraitScroll->getScrollPosition();
	
	if(clipHeight < panelHeight) {
		int iscrpos = (panelHeight - clipHeight) * scrollPos;
		Portraitpanel->setYPosition(Absolute, -iscrpos);
	}
			
	return true;	
}


/**************************************************************************************/
/* 1.选择角色头像鼠标单击事件，根据选择的单选钮头像图片，设置对应的性别和头像号；
 * 2.顺序按照uicfg.ini中[ManPortrait]和[WomanPortrait]的顺序  
/***************************************************************************************/
bool KUiNewPlayer::onClickPortrait( const CEGUI::EventArgs& args ) 
{	
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLRadioButton* radio = (TLRadioButton*)(mouse->window);

	int iSex = KUiNewPlayerInfo::GetSingleton().m_pNewRole.byGender;

	if("Portrait_Radio1-1" == radio->getName()) 
	{
		SelectPortrait(iSex,0);			
	} 
	else if("Portrait_Radio1-2" == radio->getName()) 
	{
		SelectPortrait(iSex,1);
	} 
	else if("Portrait_Radio1-3" == radio->getName()) 
	{
		SelectPortrait(iSex,2);
	}
	else if("Portrait_Radio2-1" == radio->getName()) 
	{
		SelectPortrait(iSex,3);
	}
	else if("Portrait_Radio2-2" == radio->getName()) 
	{
		SelectPortrait(iSex,4);
	}
	else if("Portrait_Radio2-3" == radio->getName()) 
	{
		SelectPortrait(iSex,5);
	}
	
	d_SelPortraitWnd->setModalState(false);
	d_SelPortraitWnd->hide();
	return true;
}

