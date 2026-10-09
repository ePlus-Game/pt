//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/09/2006 16:49
//      File_base        : UiLoginBg
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "Faith.h"
#include "UiLoginBg.h"
#include "KWin32Wnd.h"
#include "CoreUseNameDef.h"


using namespace CEGUI;
const unsigned int	KUiLoginBackGround::WebButtonID		= UI_LOGINBK_WEB;
const unsigned int	KUiLoginBackGround::RegistButtonID	= UI_LOGINBK_REGIST;
const unsigned int	KUiLoginBackGround::PayButtonID		= UI_LOGINBK_PAY;

template<> 
KUiLoginBackGround* KUiWndSingleton<KUiLoginBackGround>::ms_Singleton	= NULL;

KUiLoginBackGround::KUiLoginBackGround( const CEGUI::String& id_name ):
KUiWndSingleton<KUiLoginBackGround>( id_name )
{
	d_serverInfo = NULL;
	d_serverInfoBK = NULL;
}

KUiLoginBackGround::~KUiLoginBackGround()
{

}

void KUiLoginBackGround::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		// Do events wire-up
//		static_cast<StaticImage*>(ms_Singleton->m_pThisWnd)->getChild(WebButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiLoginBackGround::handleWeb, ms_Singleton));

//		static_cast<StaticImage*>(ms_Singleton->m_pThisWnd)->getChild(RegistButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiLoginBackGround::handleRegist, ms_Singleton));

//		static_cast<StaticImage*>(ms_Singleton->m_pThisWnd)->getChild(PayButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiLoginBackGround::handlePay, ms_Singleton));//*/

	//	ms_Singleton->m_pThisWnd->setRenderMode( false, 3 );
	//	ms_Singleton->m_pThisWnd->SetBottomWindow( );
		static_cast<StaticImage*>(ms_Singleton->m_pThisWnd)->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiLoginBackGround::handleKeyDown, ms_Singleton));

																					
		ms_Singleton->d_serverInfo = (TLStaticText*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/LoginBK/ServerInfo");
		d_serverInfoBK = static_cast< TLStaticImage* >( m_pThisWnd->getChild( "TaharezLook/LoginBK/ServerBK" ) );

		ms_Singleton->d_serverInfo->setText(AnsiToUtf8(g_ServerInfo));
	}
}

void	KUiLoginBackGround::Show( void )
{
	KUiWndSingleton<KUiLoginBackGround>::Show();
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		// load an image to use as a background
		if ( !CEGUI::ImagesetManager::getSingleton().isImagesetPresent( UI_LOGINBK_IMAGESET_NAME_CREATE_JS_0 ) )
		{

			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CREATE_JS_0, UI_LOGINBK_IMAGE_PATH_CREATE_JS_0 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CREATE_JS_1, UI_LOGINBK_IMAGE_PATH_CREATE_JS_1 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CREATE_DS_0, UI_LOGINBK_IMAGE_PATH_CREATE_DS_0 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CREATE_DS_1, UI_LOGINBK_IMAGE_PATH_CREATE_DS_1 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CREATE_YR_0, UI_LOGINBK_IMAGE_PATH_CREATE_YR_0 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CREATE_YR_1, UI_LOGINBK_IMAGE_PATH_CREATE_YR_1 );

			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_SEL_JS_1, UI_LOGINBK_IMAGE_PATH_SEL_JS_1 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_SEL_JS_0, UI_LOGINBK_IMAGE_PATH_SEL_JS_0 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_SEL_DS_1, UI_LOGINBK_IMAGE_PATH_SEL_DS_1 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_SEL_DS_0, UI_LOGINBK_IMAGE_PATH_SEL_DS_0 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_SEL_YR_1, UI_LOGINBK_IMAGE_PATH_SEL_YR_1 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_SEL_YR_0, UI_LOGINBK_IMAGE_PATH_SEL_YR_0 );

			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CLICK_JS_0, UI_LOGINBK_IMAGE_PATH_CLICK_JS_0 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CLICK_JS_1, UI_LOGINBK_IMAGE_PATH_CLICK_JS_1 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CLICK_DS_0, UI_LOGINBK_IMAGE_PATH_CLICK_DS_0 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CLICK_DS_1, UI_LOGINBK_IMAGE_PATH_CLICK_DS_1 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CLICK_YR_0, UI_LOGINBK_IMAGE_PATH_CLICK_YR_0 );
			ImagesetManager::getSingleton().createImagesetFromImageFile( UI_LOGINBK_IMAGESET_NAME_CLICK_YR_1, UI_LOGINBK_IMAGE_PATH_CLICK_YR_1 );
		}
	}

}

bool KUiLoginBackGround::handleWeb( const CEGUI::EventArgs& args )
{
//	::ShellExecute( NULL, "open", m_strRegister.c_str(), NULL, NULL, SW_SHOWNORMAL );
	return true;
}

bool KUiLoginBackGround::handleRegist( const CEGUI::EventArgs& args )
{
//	::ShellExecute( NULL, "open", m_strRegister.c_str(), NULL, NULL, SW_SHOWNORMAL );
	return true;
}

bool KUiLoginBackGround::handlePay( const CEGUI::EventArgs& args )
{
//	::ShellExecute( NULL, "open", m_strRegister.c_str(), NULL, NULL, SW_SHOWNORMAL );
	return true;
}

bool KUiLoginBackGround::handleKeyDown(const CEGUI::EventArgs& args)
{
    switch (static_cast<const KeyEventArgs&>(args).scancode)
    {
    case Key::Escape:
		if ( g_GetMainApp() )
		{
			g_GetMainApp()->StopApplication();
		}
        break;
    case Key::Return:
        break;
    default:
        return true;
    }

    return true;
}

void KUiLoginBackGround::SetServerInfo( CEGUI::String serverInfo )
{
	if ( NULL != d_serverInfo )
	{
		d_serverInfo->setText(serverInfo);
	}
}

void KUiLoginBackGround::ShowServerInfo()
{
	if ( NULL != d_serverInfo && NULL != d_serverInfoBK )
	{
		d_serverInfo->show();
		d_serverInfoBK->show();	
	}
}

void KUiLoginBackGround::HideServerInfo()
{
	if ( NULL != d_serverInfo && NULL != d_serverInfoBK )
	{
		d_serverInfo->hide();
		d_serverInfoBK->hide();
	}
}
	
