//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/13/2007 9:50
//      File_base        : UiAutoConnect
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KWin32Wnd.h"
#include "CoreShell.h"
#include "UiAutoConnect.h"
#include "../UiAdapter.h"
#include "UiLogin.h"
#include "../KMessageCentre.h"
#include "GameDataDef.h"
#include "GlobalDef.h"
#include "KSG_MD5_String.h"
#include "UiNewPlayer.h"
#include "UiSelPlayer.h"

//#include "../../Faith.h"

extern iCoreShell*		g_pCoreShell;

using namespace CEGUI;

const int					DefaultTimeToNextConnect	= 20;
const int					DefaultConnectTimes			= -1;
const unsigned short		DefaultOverTimeLimit		= 10;
const char*					g_szPoint[6] = { ".", "..", "...", "....", ".....", "......"};

template<> 
KUiAutoConnect* KUiWndSingleton<KUiAutoConnect>::ms_Singleton	= NULL;

KUiAutoConnect::KUiAutoConnect( const CEGUI::String& id_name )
: KUiWndSingleton<KUiAutoConnect>( id_name )
, m_StaticText(NULL)
, m_AutoConnect(NULL)
, m_QuitGame(NULL)
, m_nRoleIndex(0)
, m_nTimeToConnect(DefaultTimeToNextConnect)
, m_nConnectTimes(DefaultConnectTimes)
, m_nOldConnectTimes(DefaultConnectTimes)
, m_nDisplayTimeControl(0)
, m_nDisplayTime(0)
, m_nOverTimeControl(0)
, m_nOverTime(0)
, m_nOverTimeLimit(DefaultOverTimeLimit)
, m_bStartConnect(false)
, m_bQuitGame(false)
{
	ZeroMemory( m_szUser, COMMON_CLIENT_MSG_LEN_32 );
	ZeroMemory( m_szPass, COMMON_CLIENT_MSG_LEN_32 );
}

KUiAutoConnect::~KUiAutoConnect()
{

}

void KUiAutoConnect::Show( void )
{
	KUiWndSingleton<KUiAutoConnect>::Show();
}


void KUiAutoConnect::Init( void	)
{	
	// 读取配置
	KIniFile iniFile;
	if ( iniFile.Load(MAP_SETTING_FILE) )
	{
		iniFile.GetInteger("AutoConnect", "TimeToNextConnect", DefaultTimeToNextConnect, &m_nTimeToConnect);
		iniFile.GetInteger("AutoConnect", "ConnectTimes", DefaultConnectTimes, &m_nConnectTimes);
		iniFile.GetInteger("AutoConnect", "OverTime", DefaultOverTimeLimit, &m_nOverTimeLimit);
		
		if ( m_nTimeToConnect < 5 || m_nTimeToConnect > 100 )
			m_nTimeToConnect = 15;

		if ( m_nConnectTimes < -1 )
			m_nConnectTimes = -1;
		
		m_nOldConnectTimes = m_nConnectTimes;
	}

	// 设置重连和退出事件
	if ( ms_Singleton->m_pThisWnd )
	{
		m_StaticText = ms_Singleton->m_pThisWnd->getChild("TaharezLook/AutoConnect/Text");

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/AutoConnect/AutoConnect")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiAutoConnect::handleAutoConnect, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/AutoConnect/QuitGame")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiAutoConnect::handleQuitGame, ms_Singleton));
		
		m_nDisplayTimeControl = GetTickCount();
	}
}

bool KUiAutoConnect::handleAutoConnect( const CEGUI::EventArgs& args )
{
	m_bStartConnect = true;
	g_LoginLogic.ReturnToIdleStatus();
	
	g_LoginLogic.SetAccountValidate(false);
	g_LoginLogic.SetSelectRole(false);

	if ( m_nConnectTimes == 0 )
	{
		const CEGUI::EventArgs args;
		handleQuitGame( args );
	}
	else if ( m_nConnectTimes != -1 && m_nConnectTimes > 0 )
	{
		m_nConnectTimes--;
	}

	KUiLogin::GetSingletonPtr()->AutoAccountValidate( m_szUser, m_szPass );

	return true;
}

bool KUiAutoConnect::handleQuitGame( const CEGUI::EventArgs& args )
{
	QuitGame();	
	return true;
}

void KUiAutoConnect::QuitGame( void )
{
	g_LoginLogic.NotifyDisconnect();
	
	m_pThisWnd->hide();
	g_LoginLogic.SetAccountValidate(true);
	g_LoginLogic.SetSelectRole(true);
	m_bQuitGame = true;
}

void  KUiAutoConnect::SetPassParam( const char* pUser, const char* pPass )
{
	if ( strcmp(m_szUser, pUser) == 0 && strcmp(m_szPass, pPass) == 0 )
		return;

	ZeroMemory( m_szUser, COMMON_CLIENT_MSG_LEN_32 );
	ZeroMemory( m_szPass, COMMON_CLIENT_MSG_LEN_32 );
	strncpy( m_szUser, pUser, COMMON_CLIENT_MSG_LEN_32 );
	strncpy( m_szPass, pPass, COMMON_CLIENT_MSG_LEN_32 );
	m_szUser[COMMON_CLIENT_MSG_LEN_32-1] = '\0';
	m_szPass[COMMON_CLIENT_MSG_LEN_32-1] = '\0';
}


void  KUiAutoConnect::DisplayAutoConnect( void )
{
	assert( m_nTimeToConnect > 0 && m_nTimeToConnect < 100 );
	
	if ( m_nConnectTimes == 0 )
	{
		const CEGUI::EventArgs args;
		handleQuitGame( args );
	}

	// 显示重连倒数记时
	if ( m_nDisplayTime < m_nTimeToConnect )
	{
		int tempTime = GetTickCount();

		if ( (tempTime - m_nDisplayTimeControl) < 1000 )
		{
			return;
		}
		else
		{
			if ( m_bStartConnect || m_bQuitGame )
			{
				// 开始处理
				ms_Singleton->m_StaticText->setAnsiText( KMessageCentre::GetMessage(auto_connect, 3) );
				m_bStartConnect = false;
				m_bQuitGame = false;
				return;
			}
			else 
			{	
				int timer = m_nTimeToConnect - m_nDisplayTime - 1;
				char temp[COMMON_CLIENT_MSG_LEN_64];
				sprintf( temp, KMessageCentre::GetMessage(auto_connect, 1), timer );

				ms_Singleton->m_StaticText->setText( AnsiToUtf8( temp ) );
				m_nDisplayTime++;
			}

			m_nDisplayTimeControl = tempTime;
		}
	}
	else if ( !m_bStartConnect && !m_bQuitGame )
	{
		const CEGUI::EventArgs args;
		handleAutoConnect( args );
	}
}

void KUiAutoConnect::ReStart()
{
	m_nDisplayTimeControl = GetTickCount();
	m_nDisplayTime			= 0;
	m_nOverTimeControl		= GetTickCount();
	m_nOverTime				= 0;
	m_bQuitGame				= false;
	m_bStartConnect			= false;
	g_LoginLogic.SetAccountValidate(false);
	g_LoginLogic.SetSelectRole(false);
}

void KUiAutoConnect::WaitAutoConnect()
{
	if ( m_nOverTime < m_nOverTimeLimit )
	{
		int tempTime = GetTickCount();
		if ( (tempTime - m_nOverTimeControl) < 1000 )
		{
			return;
		}
		else
		{
			m_nOverTimeControl = tempTime;
			m_nOverTime++;

			char* text = KMessageCentre::GetMessage(auto_connect, 4);
					
			int i = m_nOverTime % 5;
			CombineTextPoint( text, i );
		}
	}
	else
	{
		OverTime();
	}
}

void KUiAutoConnect::OverTime()
{
	ms_Singleton->m_StaticText->setAnsiText( KMessageCentre::GetMessage(auto_connect, 5) );

	Sleep(1000);

	ReStart();
}

void KUiAutoConnect::CombineTextPoint( char* str, int i )
{
	strcat( str, g_szPoint[i] );

	char temp[COMMON_CLIENT_MSG_LEN_128];
	strncpy( temp, str, COMMON_CLIENT_MSG_LEN_128 );
	temp[COMMON_CLIENT_MSG_LEN_128-1] = '\0';

	ms_Singleton->m_StaticText->setAnsiText( (char*)temp );
}

void KUiAutoConnect::Login( void )
{
	g_LoginLogic.ReturnToIdleStatus();
	assert ( m_szUser && m_szPass );

	KSG_PASSWORD Password;

#ifdef SWORDONLINE_USE_MD5_PASSWORD
	KSG_StringToMD5String( Password.szPassword, m_szPass );		
#else
	strncpy(Password.szPassword, m_szPass, sizeof(Password.szPassword));
	Password.szPassword[sizeof(Password.szPassword) - 1] = '\0';
#endif	

	g_LoginLogic.LoginAccountValidate( m_szUser, Password, m_szPass, true );
}