//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/26/2007 9:50
//      File_base        : UiDelComfirm
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 删除角色确认
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "UiDelComfirm.h"
#include "../KMessageCentre.h"
#include "../../Login/Login.h"
#include "KSG_MD5_String.h"
#include "UiSelPlayer.h"
#include "GameDataDef.h"

using namespace CEGUI;

template<>
KUiDelComfirm* KUiWndSingleton<KUiDelComfirm>::ms_Singleton = NULL;

KUiDelComfirm::KUiDelComfirm( const CEGUI::String& id_name )
: KUiWndSingleton<KUiDelComfirm>( id_name )
, m_StaticText(NULL)
, m_Password(NULL)
{
}

KUiDelComfirm::~KUiDelComfirm()
{
}

void KUiDelComfirm::Show( void )
{
	KUiWndSingleton<KUiDelComfirm>::Show();
}

void KUiDelComfirm::Init( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->setRenderMode(false,3);
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/DelComfirm/Comfirm")->subscribeEvent( PushButton::EventClicked, Event::Subscriber(&KUiDelComfirm::handleComfirm, ms_Singleton) );
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/DelComfirm/Cancel")->subscribeEvent( PushButton::EventClicked, Event::Subscriber(&KUiDelComfirm::handleCancel, ms_Singleton) );	
		m_StaticText = ms_Singleton->m_pThisWnd->getChild("TaharezLook/DelComfirm/Text");
		m_Password = ms_Singleton->m_pThisWnd->getChild( "TaharezLook/DelComfirm/Password" );
		m_StaticText->setAnsiText( KMessageCentre::GetMessage(delete_comfirm, 1) );

		//删除角色 zhangxin
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/DelComfirm/ForgetPwd2")->subscribeEvent( PushButton::EventClicked, Event::Subscriber(&KUiDelComfirm::handleForgetPwd2, ms_Singleton) );
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/DelComfirm/Close")->subscribeEvent( PushButton::EventClicked, Event::Subscriber(&KUiDelComfirm::handleClose, ms_Singleton) );

		m_StaticTextDetail = ms_Singleton->m_pThisWnd->getChild("TaharezLook/DelComfirm/TextDetail");	
		
		String delText = KMessageCentre::GetMessage(delete_comfirm, 1);
				
		int index = KUiSelPlayer::GetSingletonPtr()->GetRoleIndex();	//取得被选择角色的数组ID

		KRoleChiefInfo tagChiefInfo;
		memset( &tagChiefInfo, 0, sizeof( tagChiefInfo ));
		int nOk = g_LoginLogic.GetRoleInfo( index, &tagChiefInfo );		//取得角色信息
		
		char* delMsg = KMessageCentre::GetMessage(delete_comfirm, 3);	//取得string.ini配置

		if (delMsg) {

			char delText[COMMON_CLIENT_MSG_LEN_256];
			memset( delText, 0, sizeof(delText));

			sprintf(delText, delMsg, tagChiefInfo.uLevel, tagChiefInfo.szName);			
			m_StaticTextDetail->setAnsiText(delText);
		}

	}
}

/************************************************************************/
/*                                                                      
/************************************************************************/
bool KUiDelComfirm::handleForgetPwd2(const CEGUI::EventArgs& args)
{
	KIniFile ini;
	ini.Load(UI_CFG_STRING);										//读取uicfg.ini文件配置
	
	char cUrl[COMMON_CLIENT_MSG_LEN_256];
	memset( cUrl, 0, sizeof(cUrl));
	
	ini.GetString("WebUrl", "ForgetPwd", "", cUrl, sizeof(cUrl));	
	ShellExecute(NULL, "open", cUrl, NULL, NULL, SW_SHOWNORMAL);	//打开忘记密码网页  			

	return true;
}

/************************************************************************/
/*                                                                      
/************************************************************************/
bool KUiDelComfirm::handleClose(const CEGUI::EventArgs& args)
{
	this->Hide();
	return true;
}


bool KUiDelComfirm::handleComfirm( const CEGUI::EventArgs& args )
{
	int index = KUiSelPlayer::GetSingletonPtr()->GetRoleIndex();
	KSG_PASSWORD Password;
	String strPassword	= m_Password->getText();
	
	if ( strPassword.empty() )
	{
		m_StaticText->setAnsiText(KMessageCentre::GetMessage(delete_comfirm, 2));
		return false;
	}

#ifdef SWORDONLINE_USE_MD5_PASSWORD
	KSG_StringToMD5String( Password.szPassword, strPassword.c_str() );		
#else
	strncpy(Password.szPassword, strPassword.c_str(), sizeof(Password.szPassword));
	Password.szPassword[sizeof(Password.szPassword) - 1] = '\0';
#endif
	
	if ( !g_LoginLogic.DeleteRoleFromServer( index, Password ) )
	{
		m_StaticText->setAnsiText(KMessageCentre::GetMessage(delete_comfirm, 2));
		return false;
	}

	m_Password->setAnsiText("");

	Hide();
	return true;
}

bool KUiDelComfirm::handleCancel( const CEGUI::EventArgs& args )
{
	m_StaticText->setAnsiText(KMessageCentre::GetMessage(delete_comfirm, 1));
	m_Password->setAnsiText("");
	Hide();
	return false;
}