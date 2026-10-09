// UiInfoBar.cpp: implementation of the KUiInfoBar class.
//
//////////////////////////////////////////////////////////////////////

#include "UiInfoBar.h"
#include "../KMessageCentre.h"
#include <sstream>
#include <iomanip>
#include "cfs_filelogs.h"
#include "../UiConfigManager.h"

using namespace std;

//const float		g_Lag		= 400.0f;
const DWORD		g_MaxLagTime[5]	= { 1968, 1991, 1936, 2023, 1953 };

//////////////////////////////////////////////////////////////////////////
//				class KUiInfoBar_Ping
//////////////////////////////////////////////////////////////////////////
template<> 
KUiInfoBarPing* KUiWndSingleton<KUiInfoBarPing>::ms_Singleton	= NULL;

KUiInfoBarPing::KUiInfoBarPing( const CEGUI::String& id_name )
: KUiWndSingleton<KUiInfoBarPing>( id_name )
, m_pingInnerDelay( 111 )
{
	m_imgFast = NULL;
	m_imgNormal = NULL;
	m_imgSlow = NULL;
	m_stServerName = NULL;

	m_dwPing = 200;
	m_oldPingstate = 1;
	m_oldPing = 200;
	m_LagStateSpace = KUiCfgLoader::getSingleton().getInfoBarCfg().LagStateSpace;
}

KUiInfoBarPing::~KUiInfoBarPing()
{

}

void KUiInfoBarPing::Init()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		m_imgFast		=	static_cast< TLStaticImage* >( m_pThisWnd->getChild( "TaharezLook/InfoBar_Ping/imgFast" ) );
		m_imgNormal		=	static_cast< TLStaticImage* >( m_pThisWnd->getChild( "TaharezLook/InfoBar_Ping/imgNormal" ) );
		m_imgSlow		=	static_cast< TLStaticImage* >( m_pThisWnd->getChild( "TaharezLook/InfoBar_Ping/imgSlow" ) );
		m_stServerName	=	static_cast< TLStaticText* >( m_pThisWnd->getChild( "TaharezLook/InfoBar_Ping/stServerName" ) );

		m_imgFast->subscribeEvent(
			StaticImage::EventMouseEnters, 
			Event::Subscriber( &KUiInfoBarPing::window_MouseEnters, this ) );
		
		m_imgFast->subscribeEvent(
			StaticImage::EventMouseLeaves, 
			Event::Subscriber( &KUiInfoBarPing::window_MouseLeaves, this ) );

		m_imgNormal->subscribeEvent(
			StaticImage::EventMouseEnters, 
			Event::Subscriber( &KUiInfoBarPing::window_MouseEnters, this ) );

		m_imgNormal->subscribeEvent(
			StaticImage::EventMouseLeaves, 
			Event::Subscriber( &KUiInfoBarPing::window_MouseLeaves, this ) );

		m_imgSlow->subscribeEvent(
			StaticImage::EventMouseEnters, 
			Event::Subscriber( &KUiInfoBarPing::window_MouseEnters, this ) );

		m_imgSlow->subscribeEvent(
			StaticImage::EventMouseLeaves, 
			Event::Subscriber( &KUiInfoBarPing::window_MouseLeaves, this ) );

		m_stServerName->subscribeEvent(
			StaticText::EventMouseEnters, 
			Event::Subscriber( &KUiInfoBarPing::window_MouseEnters, this ) );

		m_stServerName->subscribeEvent(
			StaticText::EventMouseLeaves, 
			Event::Subscriber( &KUiInfoBarPing::window_MouseLeaves, this ) );

		UpdateNetInfo( m_oldPing );
	}
}

void KUiInfoBarPing::SetServerName( string& serverName )
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_stServerName )
	{
		ms_Singleton->m_stServerName->setText( AnsiToUtf8( serverName.c_str() ) );
	}
}

void KUiInfoBarPing::UpdateNetInfo( DWORD nPing )
{
	int i = g_Random( 5 );
	m_dwPing = g_MaxLagTime[i] > nPing ? nPing : g_MaxLagTime[i];

	int realPing = static_cast< int >( m_dwPing ) - m_pingInnerDelay;
	m_dwPing = realPing > 0 ? realPing : 1;
	
	
	RefreshPingImage( m_dwPing );
	RefreshPingTips( m_dwPing );

// #ifdef _DEBUG
// 	string loginfo = "Ping  = " + iToStr( nPing ) + "\r\n";
// 	CFS_FILELOGS::WriteLog( loginfo.c_str() );
// #endif	

	m_oldPing = m_dwPing;
}

void KUiInfoBarPing::RefreshPingImage( DWORD ping )
{
	if ( ( NULL != m_imgFast ) && ( NULL != m_imgNormal ) && ( NULL != m_imgSlow ) )
	{
		int pingstate = static_cast< int >( static_cast< float >( ping ) / static_cast< float >( m_LagStateSpace ) );
		
		//为了防止窗口刷新导致同级窗口以及父窗口随着一起刷新，在之前判断是否需要更新图片
		if( m_oldPingstate == pingstate )
		{
			return;
		}

		if ( pingstate > 2 )
		{
			pingstate = 2;
		}
		
		m_oldPingstate = pingstate;
		
		m_imgFast->hide();		
		m_imgNormal->hide();	
		m_imgSlow->hide();
		
		switch( pingstate )
		{
		case 0:
			{
				m_imgFast->show();
			}
			break;
		case 1:
			{
				m_imgNormal->show();
			}
			break;
		case 2:
			{
				m_imgSlow->show();
			}
			break;
		default:
			{
				m_imgNormal->show();
			}
			break;
		}
	}
}

void KUiInfoBarPing::RefreshPingTips( DWORD ping )
{
	if ( NULL != m_pThisWnd )
	{
		char pingMsg[COMMON_CLIENT_MSG_LEN_512];
		ZeroMemory( pingMsg, sizeof( pingMsg) );
		_snprintf( pingMsg, sizeof( pingMsg ), KMessageCentre::GetMessage( infobar_message, 0 ), ping );
		pingMsg[sizeof( pingMsg ) - 1] = 0;

		if ( m_oldPing != ping )
		{
			m_pThisWnd->setTooltipText( AnsiToUtf8( pingMsg ) );
		}
	}	
}

bool KUiInfoBarPing::window_MouseEnters( const CEGUI::EventArgs& e )
{
	//caol+ 2008/03/25 //处理鼠标滑动过快的情况下m_pThisWnd的MouseEnters消息被吃掉的问题
	const WindowEventArgs& wargs = static_cast< const WindowEventArgs& >( e );
	Window* tmpWnd = wargs.window;
	Window* parentWnd = tmpWnd->getParent();
	if ( NULL != parentWnd )
	{
		MouseEventArgs& margs = static_cast< MouseEventArgs& >( const_cast< EventArgs& > ( e ) );
		static_cast< TLStaticImage* >( parentWnd )->onMouseEnters( margs );
	}
	
	return true;
}

bool KUiInfoBarPing::window_MouseLeaves( const CEGUI::EventArgs& e )
{
	//caol+ 2008/03/25 //处理鼠标滑动过快的情况下m_pThisWnd的MouseLeaves消息被吃掉的问题
	const WindowEventArgs& wargs = static_cast< const WindowEventArgs& >( e );
	Window* tmpWnd = wargs.window;
	Window* parentWnd = tmpWnd->getParent();
	if ( NULL != parentWnd )
	{
		MouseEventArgs& margs = static_cast< MouseEventArgs& >( const_cast< EventArgs& > ( e ) );
		static_cast< TLStaticImage* >( parentWnd )->onMouseLeaves( margs );
	}
	
	return true;		
}



//////////////////////////////////////////////////////////////////////////
//				class KUiInfoBarTime
//////////////////////////////////////////////////////////////////////////
template<> 
KUiInfoBarTime* KUiWndSingleton<KUiInfoBarTime>::ms_Singleton	= NULL;

KUiInfoBarTime::KUiInfoBarTime( const CEGUI::String& id_name )
: KUiWndSingleton<KUiInfoBarTime>( id_name )
, s_breatheInterval( 1000 )
{
	m_stDate = NULL;
	m_stTime = NULL;
	m_curTick = 0;
	m_curFlipTick = 0;
	m_FlipInterval = 2000;
}

KUiInfoBarTime::~KUiInfoBarTime()
{
	
}

void KUiInfoBarTime::Init()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		m_stDate =  static_cast< TLStaticText* >( m_pThisWnd->getChild( "TaharezLook/InfoBar_Time/stDate" ) );
		m_stTime =  static_cast< TLStaticText* >( m_pThisWnd->getChild( "TaharezLook/InfoBar_Time/stTime" ) );

		m_stDate->subscribeEvent(
			StaticText::EventMouseEnters, 
			Event::Subscriber( &KUiInfoBarTime::window_MouseEnters, this ) );

		m_stDate->subscribeEvent(
			StaticText::EventMouseLeaves, 
			Event::Subscriber( &KUiInfoBarTime::window_MouseLeaves, this ) );

		m_stTime->subscribeEvent(
			StaticText::EventMouseEnters, 
			Event::Subscriber( &KUiInfoBarTime::window_MouseEnters, this ) );
		
		m_stTime->subscribeEvent(
			StaticText::EventMouseLeaves, 
			Event::Subscriber( &KUiInfoBarTime::window_MouseLeaves, this ) );


		//m_FlipInterval = atoi( KMessageCentre::GetMessage( infobar_message, 1 ) );
		m_FlipInterval = KUiCfgLoader::getSingleton().getInfoBarCfg().TimeFlipInterval;
		m_TimeTipFormatString = KMessageCentre::GetMessage( infobar_message, 1 );
		RefreshTime();
	}
}

void KUiInfoBarTime::Breathe()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->DoBreathe();
	}
}

void KUiInfoBarTime::DoBreathe()
{
	DWORD curTick = ::GetTickCount();
	
	if ( curTick - m_curTick > s_breatheInterval )
	{
		RefreshTime();
		m_curTick = curTick;
	}

	if ( curTick - m_curFlipTick > m_FlipInterval )
	{
		FlipTime();
		m_curFlipTick = curTick;
	}
}

void KUiInfoBarTime::FlipTime()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		if ( m_stDate->isVisible() )
		{
			m_stDate->hide();
			m_stTime->show();
		}
		else
		{
			m_stDate->show();
			m_stTime->hide();
		}
	}
}

void KUiInfoBarTime::RefreshTime()
{
	const time_t cur_t = time( NULL );
	tm* currentTime = localtime( &cur_t );
	
	string yearShow( KMessageCentre::GetMessage( time_message, 0 ) );
	string monShow( KMessageCentre::GetMessage( time_message, 1 ) );
	string dayShow( KMessageCentre::GetMessage( time_message, 2 ) );
	string hourShow( KMessageCentre::GetMessage( time_message, 3 ) );
	string minuteShow( KMessageCentre::GetMessage( time_message, 4 ) );
	
	ostringstream curDate;
	curDate<< currentTime->tm_year + 1900 << yearShow
		<< currentTime->tm_mon + 1 << monShow
		<< currentTime->tm_mday << dayShow;
	ms_Singleton->m_stDate->setText( AnsiToUtf8( curDate.str().c_str() ) );
	
	ostringstream curTime;
	curTime << setw( 2 ) << setfill( '0' ) << currentTime->tm_hour << hourShow
		<< setw( 2 ) << setfill( '0' ) << currentTime->tm_min << minuteShow;
	ms_Singleton->m_stTime->setText( AnsiToUtf8( curTime.str().c_str() ) );
	
	//刷新时间的Tips
	if ( m_TimeTipFormatString.length() > 0 )
	{
		char timeMsg[COMMON_CLIENT_MSG_LEN_512];
		int  msgLength = sizeof( timeMsg );
		ZeroMemory( timeMsg, msgLength );
		_snprintf( timeMsg, msgLength, m_TimeTipFormatString.c_str(), curDate.str().c_str(), curTime.str().c_str() );
		timeMsg[msgLength - 1] = 0;
		
		m_pThisWnd->setTooltipText( AnsiToUtf8( timeMsg ) );
	}	
}

bool KUiInfoBarTime::window_MouseEnters( const CEGUI::EventArgs& e )
{
	//caol+ 2008/03/25 //处理鼠标滑动过快的情况下m_pThisWnd的MouseEnters消息被吃掉的问题
	const WindowEventArgs& wargs = static_cast< const WindowEventArgs& >( e );
	Window* tmpWnd = wargs.window;
	Window* parentWnd = tmpWnd->getParent();
	if ( NULL != parentWnd )
	{
		MouseEventArgs& margs = static_cast< MouseEventArgs& >( const_cast< EventArgs& > ( e ) );
		static_cast< TLStaticImage* >( parentWnd )->onMouseEnters( margs );
	}
	
	return true;
}

bool KUiInfoBarTime::window_MouseLeaves( const CEGUI::EventArgs& e )
{
	//caol+ 2008/03/25 //处理鼠标滑动过快的情况下m_pThisWnd的MouseLeaves消息被吃掉的问题
	const WindowEventArgs& wargs = static_cast< const WindowEventArgs& >( e );
	Window* tmpWnd = wargs.window;
	Window* parentWnd = tmpWnd->getParent();
	if ( NULL != parentWnd )
	{
		MouseEventArgs& margs = static_cast< MouseEventArgs& >( const_cast< EventArgs& > ( e ) );
		static_cast< TLStaticImage* >( parentWnd )->onMouseLeaves( margs );
	}
	
	return true;		
}
