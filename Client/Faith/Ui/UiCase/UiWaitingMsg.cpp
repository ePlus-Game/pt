//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/23/2007 9:50
//      File_base        : UiWaitingMsg
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 登陆等待
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KWin32Wnd.h"
#include "CoreShell.h"
#include "UiWaitingMsg.h"
#include "../UiAdapter.h"
#include "../KMessageCentre.h"
#include "UiChangeMapWnd.h"
#include "UiAutoConnect.h"
#include "process.h"
#include "UiNewPlayer.h"
#include "UiSelPlayer.h"

#include "../../Faith.h"

using namespace CEGUI;

const int DefaultOverTimeLimit = 16;

#define DEFAULT_STR_TRY_TIME 10

template<> 
KUiWaitingMsg* KUiWndSingleton<KUiWaitingMsg>::ms_Singleton	= NULL;

KUiWaitingMsg::KUiWaitingMsg( const CEGUI::String& id_name )
: KUiWndSingleton<KUiWaitingMsg>( id_name )
, m_StaticText(NULL)
,m_loop(0)
,m_errorMsg(false)
,m_bConnectting(false)
,m_bConnectSwitch(false)
,m_hThread(NULL)
,m_bModel(true)
,m_bQuiting(false)
{
	memset(m_AutoConnectText, 0, sizeof(m_AutoConnectText));
}

KUiWaitingMsg::~KUiWaitingMsg()
{
	QuitMsg();
}

void KUiWaitingMsg::Init( void	)
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setRenderMode( false, 3 );		
		m_StaticText = ms_Singleton->m_pThisWnd->getChild("TaharezLook/WaitConnect/Text");
		if ( m_StaticText )
		{
			m_StaticText->disable();
		}
		m_pThisWnd->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiWaitingMsg::handleKeyDown, ms_Singleton));
		m_pThisWnd->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiWaitingMsg::onShow, ms_Singleton));
		m_pThisWnd->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiWaitingMsg::onHide, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/WaitConnect/Close")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiWaitingMsg::onClose, ms_Singleton));
		m_timeBegin = 3;
		m_timeEnd = 5;
		m_bNeedActiveKey = false;
	}
}

bool KUiWaitingMsg::handleKeyDown(const CEGUI::EventArgs& args)
{
    using namespace CEGUI;
	
    switch (static_cast<const KeyEventArgs&>(args).scancode)
    {
    case Key::Return:
		{
			if ( ms_Singleton && ms_Singleton->m_pThisWnd )
			{
				ms_Singleton->m_pThisWnd->setModalState(true);
			}
			EndLogin();
			QuitMsg();
			if ( KUiNewPlayer::IsVisible() ||
				KUiSelPlayer::IsVisible() )
			{
				return true;
			}
			g_LoginLogic.NotifyDisconnect( false, m_bNeedActiveKey );
		}
        break;
    default:
        return false;
    }
	
    return true;
}

bool KUiWaitingMsg::onShow( const CEGUI::EventArgs& args )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setModalState(m_bModel);
		ms_Singleton->m_pThisWnd->setZLevel(Window::SuperTop);
	}
	return true;
}

bool KUiWaitingMsg::onHide( const CEGUI::EventArgs& args )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setModalState(false);
	}
	return true;
}

bool KUiWaitingMsg::onClose( const CEGUI::EventArgs& args )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setModalState(true);
	}
	EndLogin();
	QuitMsg();
	if ( KUiNewPlayer::IsVisible() ||
		KUiSelPlayer::IsVisible() )
	{
		return true;
	}
	g_LoginLogic.NotifyDisconnect( false, m_bNeedActiveKey );
	return true;
}

void KUiWaitingMsg::ShowMessage( const char* msg, bool bModel )
{
	m_bModel = bModel;
	m_errorMsg = true;
	KUiChangeMapWnd::GetSingleton().EndLoading();
	KUiWaitingMsg::Show();
	
	if ( ms_Singleton && ms_Singleton->m_StaticText)
	{
		ms_Singleton->m_StaticText->setAnsiText( msg );
	}
}

bool KUiWaitingMsg::IsQuiting( void )
{
	return m_bQuiting;
}

void KUiWaitingMsg::SetLoginStatus( LOGIN_BG_INFO_MSG_INDEX eIndex	)
{
	m_errorMsg = true;
	KUiChangeMapWnd::GetSingleton().EndLoading();
	KUiWaitingMsg::Show();

	const char* szTip = KMessageCentre::GetMessage( login_error_message, eIndex );
	
	if (szTip && ms_Singleton && ms_Singleton->m_StaticText)
	{
		ms_Singleton->m_StaticText->setAnsiText( szTip );
	}
}

void  KUiWaitingMsg::SetLoginParam( const char* pAccount, const KSG_PASSWORD& crPassword, const char* pActiveKey )
{
	m_username = pAccount;
	memcpy(&m_crPassword, &crPassword, sizeof(m_crPassword) );
	m_activekey = pActiveKey;
}

bool KUiWaitingMsg::BeginLogin( void )
{
	if ( !m_bConnectting && !m_hThread )
	{
		m_bConnectSwitch = true;
		m_bConnectting = true;
	   m_hThread = (HANDLE) _beginthread(
			LoadThreadLogin,
			0,
			ms_Singleton );
	   KUiWaitingMsg::Show();
	   char* text = KMessageCentre::GetMessage(auto_connect, 4);
	   if ( text && ms_Singleton && ms_Singleton->ms_Singleton )
	   {
		   	char temp[COMMON_CLIENT_MSG_LEN_128];
			snprintf( temp, COMMON_CLIENT_MSG_LEN_128, "%s,%s", text, g_szPoint[0] );
			temp[COMMON_CLIENT_MSG_LEN_128-1] = 0;
			ms_Singleton->m_StaticText->setAnsiText( (char*)temp );
	   }	   
	}	
	return true;
}

void KUiWaitingMsg::EndLogin( void )
{
	m_bConnectSwitch = false;
	m_bConnectting = false;
	m_hThread = 0;
}

void KUiWaitingMsg::WaitConnect( void )
{
	if ( m_AutoConnectText[0] == 0 )
	{
		char* text = KMessageCentre::GetMessage(auto_connect, 4);
		if ( text == NULL )
		{
			return;
		}
		strncpy( m_AutoConnectText, text, sizeof(m_AutoConnectText) );
	}

	switch( g_LoginLogic.GetStatus() )
	{
	case LL_S_ACCOUNT_CONFIRMING:
		CombineTextPoint( m_AutoConnectText );
	    break;
	case LL_S_WAIT_ROLE_LIST:
		CombineTextPoint( m_AutoConnectText );
	    break;
	case LL_S_WAIT_INPUT_ACCOUNT:
		CombineTextPoint( m_AutoConnectText );
		break;
	case LL_S_ROLE_LIST_READY:
	case LL_S_CREATING_ROLE:
	case LL_S_DELETING_ROLE:
	case LL_S_ENTERING_GAME:
	case LL_S_IN_GAME:
	case LL_S_DROPLINE:
	case LL_S_IDLE:
		if ( !m_errorMsg && !IsConnectting())
		{
			QuitMsg();
		}
	    break;
	default:
		if ( !m_errorMsg && !IsConnectting())
		{
			QuitMsg();
		}
	    break;
	}
}

void KUiWaitingMsg::QuitMsg( void )
{
	m_bQuiting = true;
	while( m_bConnectting )
	{
		Sleep(1);
	}

	m_errorMsg = false;

	if ( ms_Singleton && ms_Singleton->m_StaticText )
	{
		ms_Singleton->m_StaticText->setText("");
	}
	Hide();
	m_bQuiting = false;
}


void KUiWaitingMsg::LoadThreadLogin(void* pParam)
{
	KUiWaitingMsg* pWaitingWnd = (KUiWaitingMsg*)pParam;
	if ( pWaitingWnd == NULL )
	{
		return;
	}
	try
	{
		while (	pWaitingWnd->m_bConnectSwitch &&
			!g_LoginLogic.LoginAccountValidate( 
			pWaitingWnd->m_username.c_str(),
			pWaitingWnd->m_crPassword,
			pWaitingWnd->m_activekey.c_str() ) )
		{
			DWORD dwWaitTime = pWaitingWnd->GetRandomTime() * 1000;

			Sleep(dwWaitTime);
		}
	}
	catch ( ... )
	{
		//有异常直接退出
	}

	if ( pWaitingWnd )
	{
		pWaitingWnd->EndLogin();
	}	
	::_endthread();
}

bool KUiWaitingMsg::IsConnectting( void )
{
	if ( ms_Singleton && ms_Singleton->m_bConnectting )
	{
		return true;
	}
	return false;
}

void KUiWaitingMsg::CombineTextPoint( char* str )
{
	// 开始游戏等待进入
	KUiWaitingMsg::Show();

	static int s_time = 0;
	if ( ::GetTickCount() - s_time >= 1000 )
	{
		s_time = ::GetTickCount();
		if ( m_loop <= 5 && m_loop >= 0 )
		{
			char temp[COMMON_CLIENT_MSG_LEN_128];
			snprintf( temp, COMMON_CLIENT_MSG_LEN_128, "%s,%s", str, g_szPoint[m_loop] );
			temp[COMMON_CLIENT_MSG_LEN_128-1] = 0;
			ms_Singleton->m_StaticText->setAnsiText( (char*)temp );
			m_loop++;
		}
		else
		{
			m_loop = 0;
		}
	}
}

DWORD KUiWaitingMsg::GetRandomTime( void )
{
	int time = 0;
	time = m_timeBegin + g_Random( m_timeEnd - m_timeBegin );
	return time;
}