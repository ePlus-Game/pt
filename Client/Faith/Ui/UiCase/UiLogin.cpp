//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/15/2006 13:05
//      File_base        : UiLogin
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KWin32Wnd.h"
#include "UiUpdateTip.h"
#include "UiLogin.h"
#include "UiSelPlayer.h"
#include "UiUpdateTip.h"
#include "UiLoginBg.h"
#include "KSG_MD5_String.h"
#include "TLButton.h"
#include "../../Login/Login.h"
#include "UiAutoConnect.h"
#include "TLCheckbox.h"
#include "TLEditbox.h"
#include "UiShortcutKeySetting.h"
#include "UiRaid.h"
#include "UiMovieFrame.h"
#include "TLListbox.h"
#include "UiMapCentre.h"
#include "UiWaitingMsg.h"
#include "UiServerList.h"
#include "UiComMsgBox.h"
#include "../KMessageCentre.h"
#include "process.h"

extern HANDLE g_updateEvent;
extern bool g_bShowServerList;

using namespace CEGUI;
using namespace std;

const unsigned int	KUiLogin::LoginButtonID	= UI_PASSWORD_LOGIN;
const unsigned int	KUiLogin::ExitButtonID	= UI_PASSWORD_BACK;
const string	    KeyPathName = "TaharezLook/Password/MiniKeyboard/Keybutton";
const int			MAX_USERNAME = 32;

extern bool			g_useServerList;

template<> 
KUiLogin* KUiWndSingleton<KUiLogin>::ms_Singleton	= NULL;

KUiLogin::KUiLogin( const CEGUI::String& id_name ):
 KUiWndSingleton<KUiLogin>( id_name )
,d_bMiniKeyBoard(false)
,d_bShiftDown(false)
,d_strUse("")
,d_strPass("")
,d_InputFocus(USERNAME)
,d_pUserName(NULL)
,d_pPassWord(NULL)
,m_hThread(NULL)
,m_ThreadId(0)
{

}

KUiLogin::~KUiLogin()
{
	if ( m_hThread )
	{
		if ( WAIT_OBJECT_0 != ::WaitForSingleObject( m_hThread, 10000 ) )
		{
			::TerminateThread( m_hThread, 0 );
		}
		::CloseHandle( m_hThread );
	}
	
	if ( g_updateEvent )
	{
		::CloseHandle( g_updateEvent );
		g_updateEvent = NULL;
	}
}

void KUiLogin::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setRenderMode( false, 3 );
//		ms_Singleton->m_pThisWnd->SetBottomWindow( );
		// Do events wire-up
		//ms_Singleton->m_pThisWnd->getChild("TaharezLook/RoleFace/18bg")->subscribeEvent(RadioButton::EventSelectStateChanged, Event::Subscriber(&KUiLogin::handle18, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild(LoginButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiLogin::handleLogin, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild(ExitButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiLogin::handleBack, ms_Singleton));
		
		//static_cast<TLEditbox*>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/Username" ))->setAscIICharacters(true);
		//static_cast<TLEditbox*>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/Password" ))->setAscIICharacters(true);
		
		
		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/memory" )->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiLogin::handleMemory, ms_Singleton));
		
		//keydown事件
		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/ActiveKey" )->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiLogin::handleKeyDown, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/Username" )->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiLogin::handleKeyDown, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/Password" )->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiLogin::handleKeyDown, ms_Singleton));
		ms_Singleton->m_pThisWnd->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiLogin::thisWnd_KeyDown, ms_Singleton));
		
		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/keyboard" )->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiLogin::handleMiniKeyDown, ms_Singleton));
//		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/Username" )->beginUpdate();
//		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/Password" )->beginUpdate();
		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/fanhui" )->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiLogin::btnFanhui_MouseClick, ms_Singleton));

//		m_pThisWnd->getChild( "TaharezLook/Password/Username" )->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiLogin::handleShow, ms_Singleton));
//		m_pThisWnd->getChild( "TaharezLook/Password/Username" )->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiLogin::handleHide, ms_Singleton));
//		m_pThisWnd->getChild( "TaharezLook/Password/Password" )->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiLogin::handleShow, ms_Singleton));
//		m_pThisWnd->getChild( "TaharezLook/Password/Password" )->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiLogin::handleHide, ms_Singleton));
// 		m_pThisWnd->getChild( "TaharezLook/Password/Username" )->beginUpdate();
// 		m_pThisWnd->getChild( "TaharezLook/Password/Password")->beginUpdate();
		m_pThisWnd->getChild( "TaharezLook/Password/Username" )->setAlpha(0.5f);
//l
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/Username")->subscribeEvent(Window::EventMouseButtonDown, Event::Subscriber(&KUiLogin::handleMouseBnDown, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/Password")->subscribeEvent(Window::EventMouseButtonDown, Event::Subscriber(&KUiLogin::handleMouseBnDown, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton1")->subscribeEvent(TLButton::EventClicked, 
											Event::Subscriber(&KUiLogin::handleKey1Down, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton2")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKey2Down, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton3")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKey3Down, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton4")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKey4Down, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton5")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKey5Down, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton6")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKey6Down, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton7")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKey7Down, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton8")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKey8Down, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton9")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKey9Down, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton0")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKey0Down, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttona")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyaDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonb")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeybDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonc")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeycDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttond")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeydDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttone")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyeDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonf")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyfDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttong")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeygDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonh")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyhDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttoni")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyiDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonj")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyjDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonk")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeykDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonl")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeylDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonm")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeymDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonn")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeynDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttono")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyoDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonp")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeypDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonq")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyqDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonr")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyrDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttons")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeysDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttont")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeytDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonu")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyuDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonv")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyvDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonw")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeywDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonx")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyxDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttony")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyyDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonz")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyzDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton`")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyTopDotDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton.")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyDotDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton-")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyPlusDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton=")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyEquelDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton\\")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyLbracketDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton/")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyRbracketDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton[")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyLslashDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton]")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyRslashDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton'")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyQuotesDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton,")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyCommaDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton;")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyColonDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/KeybuttonEnter")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyEnterDown, this));

		//PushButton *EnterKey =	static_cast<PushButton *>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/KeybuttonEnter"));	
		//EnterKey->getChild("TaharezLook/Password/MiniKeyboard/KeybuttonEntertxt")->subscribeEvent(TLButton::EventClicked,Event::Subscriber(&KUiLogin::handleKeyEnterDown, this));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/KeybuttonShift")->subscribeEvent(RadioButton::EventSelectStateChanged,
											Event::Subscriber(&KUiLogin::handleKeyShiftDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/KeybuttonCancel")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyCancelDown, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/KeybuttonBlank")->subscribeEvent(TLButton::EventClicked,
											Event::Subscriber(&KUiLogin::handleKeyBlankDown, this));

		//render add // 去掉18岁的东西
		Checkbox* p18Btn = (Checkbox*)m_pThisWnd->getChild("TaharezLook/Password/18btn");
		p18Btn->setSelected(true);
		p18Btn->hide();
		StaticImage* p18BtnBk = (StaticImage*)m_pThisWnd->getChild("TaharezLook/Password/18bg");
		p18BtnBk->hide();
		//end render aa
		
		//注册账号、忘记密码、游戏官网、账号充值按钮事件 zhangxin	
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/Register")->subscribeEvent(PushButton.EventClicked, Event::Subscriber(&KUiLogin::HandleIEClick, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/ForgetPwd")->subscribeEvent(PushButton.EventClicked, Event::Subscriber(&KUiLogin::HandleIEClick, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/GameWeb")->subscribeEvent(PushButton.EventClicked, Event::Subscriber(&KUiLogin::HandleIEClick, this));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/Recharge")->subscribeEvent(PushButton.EventClicked, Event::Subscriber(&KUiLogin::HandleIEClick, this));
		
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/ViewAgreement")->subscribeEvent(PushButton.EventClicked, Event::Subscriber(&KUiLogin::HandleViewClick, this));
		Checkbox* pagree = (Checkbox*)m_pThisWnd->getChild("TaharezLook/Password/Agree");
		pagree->setSelected(true);

		g_updateEvent = ::CreateEvent(NULL, TRUE, TRUE, NULL);
			
	}
}



void KUiLogin::Show(  bool bShowActiveKey  )
{
	KUiWndSingleton<KUiLogin>::Show();
	KUiLoginBackGround::GetSingleton().ShowServerInfo();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->setVisible(false);
	ms_Singleton->d_pUserName	= static_cast<Editbox *>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/Username" ));
	ms_Singleton->d_pUserName->activate();
	int nSelect = 0;
	char szUserName[MAX_USERNAME];
	KIniFile ini;
	ini.Load(CONFIG_INI);	
	ini.GetInteger( "LastLogin","SaveLastID",0, &nSelect );
	ini.GetString( "LastLogin","LastLoginID", "", szUserName, MAX_USERNAME );
	TLCheckbox* pBtn = (TLCheckbox*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/Password/memory");
	if ( nSelect )
	{
		pBtn->setSelected( true );
		ms_Singleton->d_pUserName->setText(AnsiToUtf8(szUserName));
		ms_Singleton->d_pUserName->setCaratIndex( strlen(szUserName) );
	}
	else
	{
		ms_Singleton->d_pUserName->setText("");
		pBtn->setSelected( false );
		ms_Singleton->d_pUserName->setCaratIndex( 0 );
	}

	ms_Singleton->d_pPassWord	= static_cast<Editbox *>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/Password" ));
	
	if ( bShowActiveKey )
	{
		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/ActiveKeyBK")->show();
		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/ActiveKey")->show();
		static_cast<Editbox *>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/ActiveKey"))->setCaratIndex( 0 );
		static_cast<Editbox *>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/ActiveKey"))->activate();
	}
	else
	{
		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/ActiveKeyBK")->hide();
		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Password/ActiveKey")->hide();
	}
}
/*
bool KUiLogin::handleShow( const CEGUI::EventArgs& args	)
{
	((WindowEventArgs*)&args)->window->beginUpdate();
	return true;
}

bool KUiLogin::handleHide( const CEGUI::EventArgs& args	)
{
	((WindowEventArgs*)&args)->window->stopUpdate();
	return true;
}
//*/
bool KUiLogin::handleMemory( const CEGUI::EventArgs& args )
{
	return true;
}

bool KUiLogin::handleLogin( const CEGUI::EventArgs& args )
{
	if ( ms_Singleton && m_pThisWnd )
	{
		Checkbox* pagree = (Checkbox*)m_pThisWnd->getChild("TaharezLook/Password/Agree");
		if ( pagree && !pagree->isSelected() )	////判断是否选中同意协议
		{
			KUiWaitingMsg::GetSingleton().SetLoginStatus( CI_MI_NO_AGREEMENT );
			return false;
		}

		Checkbox* p18Btn = (Checkbox*)m_pThisWnd->getChild("TaharezLook/Password/18btn");
		if ( p18Btn && p18Btn->isSelected() )
		{
			Login();
		}
		else
		{			
			KUiWaitingMsg::GetSingleton().SetLoginStatus( CI_MI_NO18 );
		}


	}	
    return true;
}

bool KUiLogin::handleBack( const CEGUI::EventArgs& args )
{
// 	KUiLogin::ToggleVisibility();
// 	KUiUpdateTip::ToggleVisibility();
// 	g_LoginLogic.NotifyDisconnect();
	Hide();
	g_LoginLogic.ReturnToIdleStatus();
	if ( g_GetMainApp() )
	{
		g_GetMainApp()->StopApplication();
	}
    return true;
}

bool KUiLogin::handleKeyDown(const CEGUI::EventArgs& args)
{
    using namespace CEGUI;

    switch (static_cast<const KeyEventArgs&>(args).scancode)
    {
    case Key::Return:
		{
			KSG_PASSWORD Password;
			String strAccount	= m_pThisWnd->getChild( "TaharezLook/Password/Username" )->getText();
			String strPassword	= m_pThisWnd->getChild( "TaharezLook/Password/Password")->getText();
			String strActiveKey	= m_pThisWnd->getChild( "TaharezLook/Password/ActiveKey")->getText();
			KSG_StringToMD5String( Password.szPassword, strPassword.c_str() );

			if ( !strAccount.empty() && !strPassword.empty() )
			{
				if ( ms_Singleton && m_pThisWnd )
				{
					Checkbox* pagree = (Checkbox*)m_pThisWnd->getChild("TaharezLook/Password/Agree");
					if ( pagree && !pagree->isSelected() )	//判断是否选中同意协议
					{
						KUiWaitingMsg::GetSingleton().SetLoginStatus( CI_MI_NO_AGREEMENT );
						return false;
					}

					Checkbox* p18Btn = (Checkbox*)m_pThisWnd->getChild("TaharezLook/Password/18btn");
					if ( p18Btn && p18Btn->isSelected() )
					{
						Login();
					}
					else
					{			
						KUiWaitingMsg::GetSingleton().SetLoginStatus( CI_MI_NO18 );
					}
				}
			}
			else
			{
				Editbox *pUsename	= (Editbox *)m_pThisWnd->getChild( "TaharezLook/Password/Username" );
				Editbox *pPassword	= (Editbox *)m_pThisWnd->getChild( "TaharezLook/Password/Password" );

				if ( pUsename && pPassword )
				{
					if ( pUsename->hasInputFocus() )
					{
						d_InputFocus = PASSWORD;
						pPassword->activate();
						pPassword->captureInput();
						int nCaratInd = pPassword->getCaratIndex();
						pPassword->setCaratIndex( nCaratInd );
					}
					else if ( pPassword->hasInputFocus() )
					{
						d_InputFocus = USERNAME;
						pUsename->activate();
						pUsename->captureInput();
						int nCaratInd = pUsename->getCaratIndex();
						pUsename->setCaratIndex( nCaratInd );
					}
					else
					{
						d_InputFocus = USERNAME;
						pUsename->activate();
						pUsename->captureInput();
						int nCaratInd = pUsename->getCaratIndex();
						pUsename->setCaratIndex( nCaratInd );
					}
				}
			}
		}
        break;
    case Key::Tab:
		{
			Editbox *pUsename	= (Editbox *)m_pThisWnd->getChild( "TaharezLook/Password/Username" );
			Editbox *pPassword	= (Editbox *)m_pThisWnd->getChild( "TaharezLook/Password/Password" );
			Editbox *pActiveKey	= (Editbox *)m_pThisWnd->getChild( "TaharezLook/Password/ActiveKey");
			
			if ( pUsename && pPassword && pActiveKey )
			{
				if ( pUsename->hasInputFocus() )
				{
					d_InputFocus = PASSWORD;
					pPassword->activate();
					pPassword->captureInput();
					int nCaratInd = pPassword->getCaratIndex();
					pPassword->setCaratIndex( nCaratInd );
				}
				else if ( pPassword->hasInputFocus() )
				{
					pActiveKey->activate();
					pActiveKey->captureInput();
					int nCaratInd = pActiveKey->getCaratIndex();
					pActiveKey->setCaratIndex( nCaratInd );
				}
				else if ( pActiveKey->hasInputFocus() )
				{
					d_InputFocus = USERNAME;
					pUsename->activate();
					pUsename->captureInput();
					int nCaratInd = pUsename->getCaratIndex();
					pUsename->setCaratIndex( nCaratInd );
				}
				else
				{
					d_InputFocus = USERNAME;
					pUsename->activate();
					pUsename->captureInput();
					int nCaratInd = pUsename->getCaratIndex();
					pUsename->setCaratIndex( nCaratInd );
				}
			}
		}
        break;
     default:
        return false;
    }
    return true;
}


bool	KUiLogin::handleMiniKeyDown( const CEGUI::EventArgs& args )
{
	//d_bMiniKeyBoard ? (d_bMiniKeyBoard = false) : (d_bMiniKeyBoard = true);
	d_bMiniKeyBoard = true;
	if ( m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->isVisible() )
		d_bMiniKeyBoard = false;

	m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->setVisible(d_bMiniKeyBoard);
	m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->setEnabled(true);
	return false;
}


bool	KUiLogin::handleMouseBnDown( const CEGUI::EventArgs& args )
{
	WindowEventArgs* scrollCtrl =(WindowEventArgs*)(&args);
	if ( d_pUserName == scrollCtrl->window )
	{
		d_InputFocus = USERNAME;
	}
	else
	{
		d_InputFocus = PASSWORD;
	}
	return false;
}

bool	KUiLogin::handleKey1Down( const CEGUI::EventArgs& args )
{
	return ShiftInput( "1", "!", d_bShiftDown );
}

bool	KUiLogin::handleKey2Down( const CEGUI::EventArgs& args )
{
	return ShiftInput( "2", "@", d_bShiftDown );
}

bool	KUiLogin::handleKey3Down( const CEGUI::EventArgs& args )
{
	return ShiftInput( "3", "#", d_bShiftDown );
}

bool	KUiLogin::handleKey4Down( const CEGUI::EventArgs& args )
{
	return ShiftInput( "4", "$", d_bShiftDown );	
}


bool	KUiLogin::handleKey5Down( const CEGUI::EventArgs& args )
{
	return ShiftInput( "5", "%", d_bShiftDown );	
}


bool	KUiLogin::handleKey6Down( const CEGUI::EventArgs& args )
{
	return ShiftInput( "6", "^", d_bShiftDown );	
}

bool	KUiLogin::handleKey7Down( const CEGUI::EventArgs& args )
{
	return ShiftInput( "7", "&", d_bShiftDown );	
}

bool	KUiLogin::handleKey8Down( const CEGUI::EventArgs& args )
{
	return ShiftInput( "8", "*", d_bShiftDown );
}

bool	KUiLogin::handleKey9Down( const CEGUI::EventArgs& args )
{
	return ShiftInput( "9", "(", d_bShiftDown );	
}

bool	KUiLogin::handleKey0Down( const CEGUI::EventArgs& args )
{
	return ShiftInput( "0", ")", d_bShiftDown );
}

bool	KUiLogin::handleKeyaDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "a", "A", d_bShiftDown );	
}

bool	KUiLogin::handleKeybDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "b", "B", d_bShiftDown );	
}

bool	KUiLogin::handleKeycDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "c", "C", d_bShiftDown );	
}

bool	KUiLogin::handleKeydDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "d", "D", d_bShiftDown );	
}

bool	KUiLogin::handleKeyeDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "e", "E", d_bShiftDown );	
}

bool	KUiLogin::handleKeyfDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "f", "F", d_bShiftDown );	
}

bool	KUiLogin::handleKeygDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "g", "G", d_bShiftDown );	
}

bool	KUiLogin::handleKeyhDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "h", "H", d_bShiftDown );	
}

bool	KUiLogin::handleKeyiDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "i", "I", d_bShiftDown );	
}

bool	KUiLogin::handleKeyjDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "j", "J", d_bShiftDown );	
}

bool	KUiLogin::handleKeykDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "k", "K", d_bShiftDown );	
}

bool	KUiLogin::handleKeylDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "l", "L", d_bShiftDown );	
}

bool	KUiLogin::handleKeymDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "m", "M", d_bShiftDown );	
}

bool	KUiLogin::handleKeynDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "n", "N", d_bShiftDown );	
}

bool	KUiLogin::handleKeyoDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "o", "O", d_bShiftDown );	
}

bool	KUiLogin::handleKeypDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "p", "P", d_bShiftDown );	
}

bool	KUiLogin::handleKeyqDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "q", "Q", d_bShiftDown );	
}

bool	KUiLogin::handleKeyrDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "r", "R", d_bShiftDown );	
}

bool	KUiLogin::handleKeysDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "s", "S", d_bShiftDown );	
}

bool	KUiLogin::handleKeytDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "t", "T", d_bShiftDown );
}

bool	KUiLogin::handleKeyuDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "u", "U", d_bShiftDown );	
}

bool	KUiLogin::handleKeyvDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "v", "V", d_bShiftDown );	
}

bool	KUiLogin::handleKeywDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "w", "W", d_bShiftDown );	
}

bool	KUiLogin::handleKeyxDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "x", "X", d_bShiftDown );	
}

bool	KUiLogin::handleKeyyDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "y", "Y", d_bShiftDown );	
}

bool	KUiLogin::handleKeyzDown( const CEGUI::EventArgs& args )
{
	return ShiftInput( "z", "Z", d_bShiftDown );	
}

bool	KUiLogin::handleKeyTopDotDown( const CEGUI::EventArgs& args	)
{
	return ShiftInput( "`", "~", d_bShiftDown );	
}

bool	KUiLogin::handleKeyDotDown( const CEGUI::EventArgs& args	)
{
	return ShiftInput( ".", ">", d_bShiftDown );	
}

bool	KUiLogin::handleKeyPlusDown( const CEGUI::EventArgs& args	)
{
	return ShiftInput( "-", "_", d_bShiftDown );	
}

bool	KUiLogin::handleKeyEquelDown( const CEGUI::EventArgs& args	)
{
	return ShiftInput( "=", "+", d_bShiftDown );	
}

bool	KUiLogin::handleKeyLbracketDown( const CEGUI::EventArgs& args	)
{
	return ShiftInput( "\\", "|", d_bShiftDown );	
}

bool	KUiLogin::handleKeyRbracketDown( const CEGUI::EventArgs& args	)
{
	return ShiftInput( "/", "?", d_bShiftDown );	
}

bool	KUiLogin::handleKeyLslashDown( const CEGUI::EventArgs& args	)
{
	return ShiftInput( "[", "{", d_bShiftDown );	
}
bool	KUiLogin::handleKeyRslashDown( const CEGUI::EventArgs& args	)
{
	return ShiftInput( "]", "}", d_bShiftDown );	
}

bool	KUiLogin::handleKeyQuotesDown( const CEGUI::EventArgs& args	)
{
	return ShiftInput( "'", "\"", d_bShiftDown );	
}

bool	KUiLogin::handleKeyCommaDown( const CEGUI::EventArgs& args	)
{
	return ShiftInput( ",", "<", d_bShiftDown );
}	


bool	KUiLogin::handleKeyColonDown( const CEGUI::EventArgs& args	)
{
	return ShiftInput( ";", ":", d_bShiftDown );
}

bool	KUiLogin::handleKeyEnterDown( const CEGUI::EventArgs& args	)
{
	EventArgs arg;
	handleLogin(arg);
	return true;
}

bool	KUiLogin::handleKeyShiftDown( const CEGUI::EventArgs& args	)
{
	d_bShiftDown ? (d_bShiftDown = false) : (d_bShiftDown = true);
	if ( d_bShiftDown == true ) 
	{
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton1")->setText("!");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton2")->setText("@");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton3")->setText("#");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton4")->setText("$");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton5")->setText("%");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton6")->setText("^");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton7")->setText("&");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton8")->setText("*");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton9")->setText("(");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton0")->setText(")");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttona")->setText("A");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonb")->setText("B");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonc")->setText("C");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttond")->setText("D");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttone")->setText("E");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonf")->setText("F");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttong")->setText("G");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonh")->setText("H");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttoni")->setText("I");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonj")->setText("J");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonk")->setText("K");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonl")->setText("L");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonm")->setText("M");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonn")->setText("N");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttono")->setText("O");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonp")->setText("P");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonq")->setText("Q");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonr")->setText("R");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttons")->setText("S");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttont")->setText("T");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonu")->setText("U");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonv")->setText("V");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonw")->setText("W");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonx")->setText("X");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttony")->setText("Y");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonz")->setText("Z");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton`")->setText("~");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton.")->setText(">");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton-")->setText("_");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton=")->setText("+");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton\\")->setText("|");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton/")->setText("?");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton[")->setText("{");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton]")->setText("}");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton'")->setText("\"");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton,")->setText("<");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton;")->setText(":");
	}
	else
	{
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton1")->setText("1");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton2")->setText("2");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton3")->setText("3");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton4")->setText("4");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton5")->setText("5");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton6")->setText("6");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton7")->setText("7");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton8")->setText("8");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton9")->setText("9");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton0")->setText("0");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttona")->setText("a");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonb")->setText("b");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonc")->setText("c");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttond")->setText("d");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttone")->setText("e");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonf")->setText("f");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttong")->setText("g");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonh")->setText("h");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttoni")->setText("i");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonj")->setText("j");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonk")->setText("k");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonl")->setText("l");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonm")->setText("m");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonn")->setText("n");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttono")->setText("o");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonp")->setText("p");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonq")->setText("q");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonr")->setText("r");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttons")->setText("s");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttont")->setText("t");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonu")->setText("u");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonv")->setText("v");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonw")->setText("w");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonx")->setText("x");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttony")->setText("y");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybuttonz")->setText("z");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton`")->setText("`");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton.")->setText(".");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton-")->setText("-");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton=")->setText("=");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton\\")->setText("\\");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton/")->setText("/");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton[")->setText("[");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton]")->setText("]");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton'")->setText("'");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton,")->setText(",");
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->getChild("TaharezLook/Password/MiniKeyboard/Keybutton;")->setText(";");
	}
	return false;
}

bool	KUiLogin::handleKeyCancelDown( const CEGUI::EventArgs& args	)
{
	string::size_type beginPos = 0;
	if ( d_InputFocus == USERNAME )
	{
		d_strUse = d_pUserName->getText().c_str();
		string::size_type endPos = d_strUse.size() - 1;
		d_strUse = d_strUse.substr( beginPos, endPos );
		d_pUserName->setAnsiText(d_strUse.c_str());
	}
	else
	{
		if ( d_InputFocus == PASSWORD )
		{
			d_strPass = d_pPassWord->getText().c_str();
			string::size_type endPos = d_strPass.size() - 1;
			d_strPass = d_strPass.substr( beginPos, endPos );
			d_pPassWord->setAnsiText(d_strPass.c_str());
		}
	}
	return true;
}

bool	KUiLogin::handleKeyBlankDown( const CEGUI::EventArgs& args	)
{
	return ( InputCharHelp(" ") );
}

bool	KUiLogin::InputCharHelp( const char *pChar )
{
	Editbox *temp = NULL;
	if ( (d_InputFocus == USERNAME) && ( d_InputFocus != PASSWORD ) )
	{
		temp = d_pUserName;
	}
	if ( (d_InputFocus != USERNAME) && ( d_InputFocus == PASSWORD ) )
	{
		temp = d_pPassWord;		
	}
	temp->activate();
	d_strUse = temp->getText().c_str();
	d_strUse.append(pChar);
	temp->setAnsiText( d_strUse.c_str() );
	temp->setCaratIndex(d_strUse.size());
	return true;
}

bool	KUiLogin::ShiftInput(const char *pNoShift, const char *pShift, bool bShiftDown)
{
	if ( bShiftDown )
	{
		InputCharHelp( pShift );
	}
	else
	{
		InputCharHelp( pNoShift );
	}
	return true;
}

string	KUiLogin::GetKeyName( const char *keyName)
{
	string sKeyName = "";
	sKeyName = KeyPathName + keyName;
	return sKeyName;
}

void KUiLogin::Login()
{
	KSG_PASSWORD Password;

	String strAccount	= m_pThisWnd->getChild( "TaharezLook/Password/Username" )->getText();
	String strPassword	= m_pThisWnd->getChild( "TaharezLook/Password/Password")->getText();
	String strActiveKey	= m_pThisWnd->getChild( "TaharezLook/Password/ActiveKey")->getText();

	KSG_StringToMD5String( Password.szPassword, strPassword.c_str() );		
	if ( !strAccount.empty() && !strPassword.empty() ) 
	{
		KUiWaitingMsg::GetSingleton().SetLoginParam(strAccount.c_str(), Password, strActiveKey.c_str());
		if( KUiWaitingMsg::GetSingleton().BeginLogin() )
		{
			KUiAutoConnect::GetSingletonPtr()->SetPassParam( strAccount.c_str(), strPassword.c_str() );
			TLCheckbox* pBtn = (TLCheckbox*)m_pThisWnd->getChild("TaharezLook/Password/memory");
			if ( pBtn )
			{
				if ( pBtn->isSelected() )
				{
					KIniFile ini;
					ini.Load(CONFIG_INI);	
					ini.WriteInteger( "LastLogin","SaveLastID", true );
					ini.WriteString( "LastLogin","LastLoginID", strAccount.c_str() );
					ini.Save(CONFIG_INI);
				}
				else
				{
					KIniFile ini;
					ini.Load(CONFIG_INI);	
					ini.WriteInteger( "LastLogin","SaveLastID", false );
					ini.WriteString( "LastLogin","LastLoginID", "" );
					ini.Save(CONFIG_INI);
				}
			}
		}
	}
	else
	{
		KUiWaitingMsg::GetSingleton().SetLoginStatus( CI_MI_ERROR_LOGIN_INPUT );		
		m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->setEnabled(false);
	}

	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->getChild( "TaharezLook/Password/ActiveKey")->setText("");
	}

	
}

void KUiLogin::AutoAccountValidate( const char* szAccName, const char* szPassword )
{
	assert ( szAccName && szPassword );

	KSG_PASSWORD Password;

#ifdef SWORDONLINE_USE_MD5_PASSWORD
	KSG_StringToMD5String( Password.szPassword, szPassword );		
#else
	strncpy(Password.szPassword, szPassword, sizeof(Password.szPassword));
	Password.szPassword[sizeof(Password.szPassword) - 1] = '\0';
#endif	

	if ( g_LoginLogic.LoginAccountValidate( szAccName, Password, "", true ) )
	{	
		g_LoginLogic.SetAccountValidate( true );
	}
	else
	{
		KUiAutoConnect::GetSingletonPtr()->ReStart();
	}
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->getChild( "TaharezLook/Password/ActiveKey")->setText("");
	}	
}

void	KUiLogin::SetKeyboardEnable( bool bEnable )
{
	m_pThisWnd->getChild("TaharezLook/Password/MiniKeyboard")->setEnabled(bEnable);
}

void	KUiLogin::SetPassWord( const char * text )
{
	if ( d_pPassWord )
	{
		d_pPassWord->setText(text);
	}
		
}

/************************************************************************/
/* 1.注册账号、忘记密码、游戏官网、账号充值 单击按钮事件
 * 2.方法打开IE游览器到指定网址
 * 3.调用ShellExecute方法调用IE 
 * zhangxin
/************************************************************************/
bool KUiLogin::HandleIEClick(const CEGUI::EventArgs& args) 
{
	WindowEventArgs* buttonArgs = (WindowEventArgs*)(&args);
	TLButton* wIe = static_cast<TLButton* >(buttonArgs->window);
	
	KIniFile ini;
	ini.Load(UI_CFG_STRING);//读取uicfg.ini文件配置
	
	char cUrl[COMMON_CLIENT_MSG_LEN_256];
	memset( cUrl, 0, sizeof(cUrl));

	if ("TaharezLook/Password/Register" == wIe->getName())
	{
		ini.GetString("WebUrl", "AccountReg", "", cUrl, sizeof(cUrl));	
	} 
	else if ("TaharezLook/Password/ForgetPwd" == wIe->getName()) 
	{
		ini.GetString("WebUrl", "ForgetPwd", "", cUrl, sizeof(cUrl));		
	} 
	else if ("TaharezLook/Password/GameWeb" == wIe->getName()) 
	{
		ini.GetString("WebUrl", "GameWeb", "", cUrl, sizeof(cUrl));
	} 
	else if ("TaharezLook/Password/Recharge" == wIe->getName()) 
	{
		ini.GetString("WebUrl", "Recharge", "", cUrl, sizeof(cUrl));
	} 
	else 
	{
		return false;
	}		

	ShellExecute(NULL, "open", cUrl, NULL, NULL, SW_SHOWNORMAL);  			
	return true;
}
bool KUiLogin::btnFanhui_MouseClick( const CEGUI::EventArgs& args )
{
	const MouseEventArgs& mouse_args = static_cast<const MouseEventArgs&>( args );
	if ( LeftButton == mouse_args.button )
	{
		return DoReturn();
	}
	else
	{
		return false;
	}
	
}


/************************************************************************/
/* 查看协议事件                                                                     
/************************************************************************/
bool KUiLogin::HandleViewClick(const CEGUI::EventArgs& args)
{
	KUiUpdateTip::GetSingleton().Show();

	return true;

}

bool KUiLogin::DoReturn()
{
	if ( g_useServerList && !KUiWaitingMsg::GetSingleton().IsConnectting() )
	{
		Hide();
		const char* szTip = KMessageCentre::GetMessage( login_error_message, CI_MI_NO_UPDATASERVERLIST );
		if ( szTip[0] != 0 )
		{
			KUiComMsgBox::GetSingleton().Show();
			KUiComMsgBox::GetSingleton().setModalStatus(true);
			KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(szTip));
			KUiComMsgBox::GetSingleton().setBtnName(AnsiToUtf8(CANNEL_RETURN));
			KUiComMsgBox::GetSingleton().setFristBtnCallback(ShowLocalServerList);

			DWORD dwRet = ::WaitForSingleObject( g_updateEvent, 0 );
			if ( m_hThread == NULL || dwRet == WAIT_OBJECT_0 )
			{
				m_hThread = (HANDLE) ::_beginthreadex(
											NULL,			// SD
											0,				// initial stack size
											DownLoadTempServerList,		// thread function
											ms_Singleton,			// thread argument
											0,				// creation option
											(unsigned*)&m_ThreadId);
				g_bShowServerList = true;
			}
		}
		else
		{
			KUiServerList::Show();
		}
		return true;
	}
	else
	{
		return false;
	}	
}

bool KUiLogin::thisWnd_KeyDown( const CEGUI::EventArgs& args )
{
	switch ( static_cast<const KeyEventArgs&>( args ).scancode )
	{
	case Key::Escape :
		{
			DoReturn();
		}
		break;
	 default:
		return false;
	}
	return true;	
}

void KUiLogin::Hide()
{
	KUiWndSingleton<KUiLogin>::Hide();
	KUiLoginBackGround::GetSingleton().HideServerInfo();
}