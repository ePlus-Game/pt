//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/28/2007 9:50
//      File_base        : UiDelayQuit
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 退出延时
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "UiDelayQuit.h"
#include "CoreShell.h"
#include "Login/Login.h"
#include "../KMessageCentre.h"
#include "UiAutoConnect.h"
#include "UiChangeMapWnd.h"
#include "UiHire.h"

extern iCoreShell*	g_pCoreShell;
const int			DefineQuitTime = 20;

template<>
KUiDeleyQuit* KUiWndSingleton<KUiDeleyQuit>::ms_Singleton = NULL;

KUiDeleyQuit::KUiDeleyQuit( const CEGUI::String& id_name )
: KUiWndSingleton<KUiDeleyQuit>( id_name )
, m_pStaticText(NULL)
, m_nDisplayTimeControl(0)
, m_nDisplayTime(0)
, m_nTimeToQuit(0)
, m_bQuitGame(false)
, m_bState(TOMAINBEGIN)
, _timerAct(true)
{
}

KUiDeleyQuit::~KUiDeleyQuit()
{
}

void KUiDeleyQuit::Show()
{
	KUiWndSingleton<KUiDeleyQuit>::Show();
}

void KUiDeleyQuit::Init()
{
	// 读取配置
	KIniFile iniFile;
	if ( iniFile.Load(MAP_SETTING_FILE) )
	{
		iniFile.GetInteger("QuitDelay", "QuitTime", DefineQuitTime, &m_nTimeToQuit);
		
		if ( m_nTimeToQuit <= 0 || m_nTimeToQuit > 100 )
			m_nTimeToQuit = 15;
	}

	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		m_pStaticText = ms_Singleton->m_pThisWnd->getChild("TaharezLook/DelayQuit/Text");
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/DelayQuit/Quit")->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiDeleyQuit::handleQuit, ms_Singleton) );
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/DelayQuit/Cancel")->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiDeleyQuit::handleCancel, ms_Singleton) );
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/DelayQuit/btnHireConfig")->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiDeleyQuit::btnHireConfig, ms_Singleton) );
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/DelayQuit/btnHireSalary")->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiDeleyQuit::btnHireSalary, ms_Singleton) );
	}
}

void KUiDeleyQuit::DisplayDelay()
{
	// 如果在安全区域
	if (!_timerAct)
	{
		m_nDisplayTimeControl = GetTickCount();
		return;
	}

	// 如果在非安全区域
	if ( m_nDisplayTime < m_nTimeToQuit )
	{
		int tempTime = GetTickCount();

		if ( (tempTime - m_nDisplayTimeControl) < 1000 )
		{
			return;
		}
		else
		{
			if ( m_bQuitGame )
			{
				// 开始处理
				m_bQuitGame = false;
				return;
			}
			else 
			{	
				char text[COMMON_CLIENT_MSG_LEN_32];
				ZeroMemory( text, COMMON_CLIENT_MSG_LEN_32 );
				switch( m_bState )
				{
				case TOMAINBEGIN:
					{
						sprintf( text, KMessageCentre::GetMessage(quit_delay, 1), (m_nTimeToQuit - m_nDisplayTime - 1) );
					}
					break;
				case TOSELECTROLE:
					{
						sprintf( text, KMessageCentre::GetMessage(quit_delay, 2), (m_nTimeToQuit - m_nDisplayTime - 1) );
					}
					break;
				default:
					break;
				}

				m_pStaticText->setAnsiText( text );
				m_nDisplayTime++;
			}

			m_nDisplayTimeControl = tempTime;
		}
	}
	else if ( !m_bQuitGame )
	{
		switch( m_bState )
		{
		case TOMAINBEGIN:
			Quit();
			break;
		case TOSELECTROLE:
			QuitToSelectRole();
		    break;
		default:
		    break;
		}
	}
}

bool KUiDeleyQuit::handleQuit( const CEGUI::EventArgs& args )
{
	if ( ((const MouseEventArgs&)args).button == LeftButton )
	{
		switch( m_bState )
		{
		case TOMAINBEGIN:
			Quit();
			break;
		case TOSELECTROLE:
			QuitToSelectRole();
		    break;
		default:
		    break;
		}
		return true;
	}
	return false;
}

bool KUiDeleyQuit::handleCancel( const CEGUI::EventArgs& args )
{
	if (((const MouseEventArgs&)args).button == LeftButton)
	{
		m_pThisWnd->hide();
		return true;
	}
	return false;
}

void KUiDeleyQuit::Reset()
{
	m_bQuitGame = false;
	m_nDisplayTime = 0;
}

void KUiDeleyQuit::Quit()
{	
	g_LoginLogic.NotifyDisconnect();
	
	if ( m_pThisWnd )
		m_pThisWnd->hide();

	m_bQuitGame = true;
}

void KUiDeleyQuit::QuitToSelectRole()
{
	Quit();
	// 和服务器断开连接需要一段时间
	// 直接SLEEP再连接
	Sleep(2000);
	KUiChangeMapWnd::GetSingleton().show( false, defaultMap );
	KUiAutoConnect::GetSingleton().Login();
}

bool KUiDeleyQuit::btnHireConfig( const CEGUI::EventArgs& args )
{
	if ( !KUiHireConfigExp::IsVisible() )
	{
		KUiHireConfigExp::Show();

		setTimerAct(false);
		return true;
	}
	else
	{
		return false;	
	}
}

bool KUiDeleyQuit::btnHireSalary( const CEGUI::EventArgs& args )
{
	if ( !KUiHireConfigSalary::IsVisible() )
	{
		KUiHireConfigSalary::Show();
		
		setTimerAct(false);
		return true;
	}
	else
	{
		return false;	
	}
}




