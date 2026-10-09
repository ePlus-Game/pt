//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/13/2006 23:43
//      File_base        : UiESCDlg
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "UiESCDlg.h"
#include "CoreShell.h"
#include "../../Login/Login.h"
#include "UiGameSetting.h"
#include "UiDelayQuit.h"
#include "UIDragItem.h"
#include "UiMapCentre.h"
#include "UiShortcutKeySetting.h"
#include "UiAutoConnect.h"
#include "../KMessageCentre.h"
#include "UiErrorMessageBox.h"
#include "UiComMsgBox.h"
#include "UiChatCentre.h"
#include "./UiFSBible.h"
#include "../UiConfigManager.h"

#include "UiHelpInfo.h"
#include "UiHire.h"

extern iCoreShell*		g_pCoreShell;

using namespace CEGUI;

const unsigned int	KUiExit::CancelButtonID	= UI_CANCEL_GAMESPACE;
const unsigned int	KUiExit::ExitButtonID	= UI_EXIT_GAMESPACE;

template<> 
KUiExit* KUiWndSingleton<KUiExit>::ms_Singleton	= NULL;

KUiExit::KUiExit( const CEGUI::String& id_name ):
KUiWndSingleton<KUiExit>( id_name )
{
}

KUiExit::~KUiExit()
{

}

void KUiExit::Init()
{
	// Do events wire-up
	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->d_pOptionBtn =static_cast<TLButton *>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/ExitGame/Operation"));
		ms_Singleton->d_skSettings =static_cast<TLButton *>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/ExitGame/SKSetting"));

		ms_Singleton->m_pThisWnd->getChild(CancelButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiExit::handleCancel, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild(ExitButtonID)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiExit::handleExit, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ExitGame/QuitToSelectRole")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiExit::handleQuitToSelectRole, ms_Singleton));
		ms_Singleton->d_pOptionBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiExit::handleOption, ms_Singleton));
		ms_Singleton->d_skSettings->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiExit::handleSKSetting, ms_Singleton));
		ms_Singleton->m_pThisWnd->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiExit::handleKeyDown, ms_Singleton));
		PushButton* pHelpButton = (PushButton*)ms_Singleton->m_pThisWnd->getChild(UIEXIT_GAME_HELP_NAME);
		pHelpButton->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&KUiExit::handleGameHelButton, ms_Singleton));
		
		PushButton* expHire = (PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/ExitGame/ExpHire");
		expHire->subscribeEvent(PushButton::EventMouseClick,Event::Subscriber(&KUiExit::onExpHire, ms_Singleton));
		PushButton* fighterHire = (PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/ExitGame/FigherHire");
		fighterHire->subscribeEvent(PushButton::EventMouseClick,Event::Subscriber(&KUiExit::onFighterHire, ms_Singleton));
	}
}

void KUiExit::Show( void )
{
	KUiWndSingleton<KUiExit>::Show();
	KUiDragItem::GetSingleton().initItem();
	if ( KUiSceneMap::getSinglton().isVisible() )
	{
		KUiSceneMap::getSinglton().hide();
	}
	if ( KUiBigMap::getSinglton().isVisible() )
	{
		KUiBigMap::getSinglton().hide();
	}
} 

bool KUiExit::handleExit( const CEGUI::EventArgs& args )
{
	KUiDeleyQuit::GetSingletonPtr()->SetQuitState( TOMAINBEGIN );
	KUiDeleyQuit::GetSingletonPtr()->Reset();
	KUiDeleyQuit::Show();
	//g_pCoreShell->OperationRequest( GOI_EXIT_GAME, 0, 0 );		
	//g_LoginLogic.NotifyDisconnect();
	Hide();
	return true;
}

bool KUiExit::handleCancel( const CEGUI::EventArgs& args )
{
	Hide();
	return true;
}

bool KUiExit::handleKeyDown(const CEGUI::EventArgs& args)
{
    using namespace CEGUI;

    switch (static_cast<const KeyEventArgs&>(args).scancode)
    {
    case Key::Return:
        break;
    default:
        return false;
    }
    return true;
}

bool KUiExit::handleOption( const CEGUI::EventArgs& args )
{
	KUiGameSetting::Show();
	return true;
}
bool KUiExit::handleGameHelButton(const CEGUI::EventArgs& args)
{
	KUiFSBible::getSingleton().ShowHelp();//zhangxin
	return true;
}
bool KUiExit::handleSKSetting( const CEGUI::EventArgs& args )
{
	KUiSKSetting::getSinglton().show();
	return true;
}

bool KUiExit::onExpHire(const CEGUI::EventArgs& args)
{
//	return true;
	if(g_pCoreShell->GetGameData( GDI_GET_EMPLOY_TIME, NULL, NULL ) <= 0)
	{
		char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_EMPLOYER_OUT_OF_EMPLOY_TIME);
		
		KUiComMsgBox::Show();
		KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(message));
		char	yesButton[COMMON_CLIENT_MSG_LEN_32];
		strcpy(yesButton, KMessageCentre::GetMessage( friend_message, KUiChatCentre::YES ));
		char	*noButton = KMessageCentre::GetMessage( friend_message, KUiChatCentre::NO );
		KUiComMsgBox::GetSingleton().setBtnName( AnsiToUtf8(yesButton), AnsiToUtf8(noButton));
		return true;
	}

	KUiHireConfigExp::Show();
	Hide();
	return true;
}

bool KUiExit::onFighterHire(const CEGUI::EventArgs& args)
{
//	return true;
	
	KUiHireConfigSalary::Show();
	Hide();
	return true;
}

bool KUiExit::handleQuitToSelectRole( const CEGUI::EventArgs& args )
{
	KUiDeleyQuit::GetSingleton().SetQuitState( TOSELECTROLE );
	KUiDeleyQuit::GetSingleton().Reset();
	KUiDeleyQuit::Show();
	return true;
}