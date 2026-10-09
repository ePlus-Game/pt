//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/13/2007 22:08
//      File_base        : UiRoleExp
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 经验值
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "UiRoleExp.h"
#include <crtdbg.h>
#include "CoreShell.h"
#include "../KMessageCentre.h"

extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiRoleExp* KUiWndSingleton<KUiRoleExp>::ms_Singleton	= NULL;

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiRoleExp::KUiRoleExp( const CEGUI::String& id_name )
: KUiWndSingleton<KUiRoleExp>( id_name )
, m_pSkillExp(NULL)
, m_pSkillExpBar(NULL)
{
	m_pSkillExp				= NULL;
	m_pSkillExpBar			= NULL;

	m_pRewardExp			= NULL;
	m_pRewardExpBar			= NULL;

	m_pRewardTime			= NULL;
	m_pRewardTimeBar		= NULL;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiRoleExp::~KUiRoleExp()
{

}

void KUiRoleExp::Show( void )
{
	KUiWndSingleton<KUiRoleExp>::Show();
	KUiCastSkillExp::Show();
}

void KUiRoleExp::Hide( void )
{
	KUiCastSkillExp::Hide();
	KUiWndSingleton<KUiRoleExp>::Hide();	
}

void KUiRoleExp::Init()
{
	if ( m_pThisWnd )
	{
		m_pThisWnd->setZLevel(Window::Bottom);
		m_pSkillExp				= m_pThisWnd->getChild( "TaharezLook/RoleExp/SkillExp" );
		m_pSkillExpBar			= (ProgressBar*)m_pThisWnd->getChild( "TaharezLook/RoleExp/SkillExpProgBar" );
		m_pRoleExp				= m_pThisWnd->getChild( "TaharezLook/RoleExp/Exp" );
		m_pRoleExpBar			= (ProgressBar*)m_pThisWnd->getChild( "TaharezLook/RoleExp/ExpProgBar" );

		m_pRewardExp			= m_pThisWnd->getChild( "TaharezLook/RoleExp/RewardExp" );
		m_pRewardExpBar			= (ProgressBar*)m_pThisWnd->getChild( "TaharezLook/RoleExp/RewardExpProgBar" );
		m_pRewardTime			= m_pThisWnd->getChild( "TaharezLook/RoleExp/RewardTime" );
		m_pRewardTimeBar		= (ProgressBar*)m_pThisWnd->getChild( "TaharezLook/RoleExp/RewardTimeProgBar" );
	}
}

void KUiRoleExp::UpdateDataSkillExp(DWORD curSkillExp, DWORD fullSkillExp)
{
	if ( m_pThisWnd )
	{
		char szBuf[COMMON_CLIENT_MSG_LEN_32];
		sprintf( szBuf, "%u/%u", curSkillExp, fullSkillExp );
		m_pSkillExp->setText( AnsiToUtf8( szBuf ) );
		m_pSkillExpBar->setProgress( (float)curSkillExp/(float)fullSkillExp );
		char szTip[1024] = {0};
		sprintf( szTip, KMessageCentre::GetMessage( common_message, 6),curSkillExp,fullSkillExp );
		String strTip = AnsiToUtf8(szTip);
		m_pSkillExpBar->setTooltipText( strTip );


		KUiCastSkillExp::GetSingleton().UpdateData( curSkillExp, fullSkillExp );
	}
}

void KUiRoleExp::UpdateDataRoleExp(DWORD curRoleExp, DWORD fullRoleExp)
{
	if ( m_pThisWnd )
	{
		char szBuf[COMMON_CLIENT_MSG_LEN_32];
		sprintf( szBuf, "%u/%u", curRoleExp, fullRoleExp );
		m_pRoleExp->setText( AnsiToUtf8( szBuf ) );
		m_pRoleExpBar->setProgress( (float)curRoleExp/(float)fullRoleExp );
		char szTip[1024] = {0};
		sprintf( szTip, KMessageCentre::GetMessage( common_message, 5),curRoleExp,fullRoleExp );
		String strTip = AnsiToUtf8(szTip);
		m_pRoleExpBar->setTooltipText( strTip );
	}
}

void KUiRoleExp::UpdateRewardExp( /*DWORD curExp, DWORD fullExp*/ )
{
	InnerUpdateRewardExp();
	RefreshTip();
}

void KUiRoleExp::UpdateRewardTime( /*DWORD curExp, DWORD fullExp*/ )
{
	InnerUpdateRewardTime();
	RefreshTip();
}


void KUiRoleExp::InnerUpdateRewardExp( /*DWORD curExp, DWORD fullExp*/ )
{
	DWORD curExp = g_pCoreShell->GetGameData( GDI_EXP_INSURANCE_VALUE, 0, 0 );
	DWORD fullExp = g_pCoreShell->GetGameData( GDI_EXP_INSURANCE_VALUE_MAX, 0, 0 );
	RefreshExpBar( curExp, fullExp, m_pRewardExp, m_pRewardExpBar, KUiRoleExp::CM_REWARDEXP_EXCEED_LEVEL );	
}

void KUiRoleExp::InnerUpdateRewardTime( /*DWORD curExp, DWORD fullExp*/ )
{
	DWORD curExp = g_pCoreShell->GetGameData( GDI_QUEST_INSURNACE_VALUE, 0, 0 );
	DWORD fullExp = g_pCoreShell->GetGameData( GDI_QUEST_INSURANCE_VALUE_MAX, 0, 0 );
	RefreshExpBar( curExp, fullExp, m_pRewardTime, m_pRewardTimeBar, KUiRoleExp::CM_REWARDTIME_EXCEED_LEVEL, true );	
}

void KUiRoleExp::RefreshExpBar( DWORD curExp, DWORD fullExp, Window* expWnd, ProgressBar* expBar, MessageNum msgNum,  bool useFloat/* = false*/ )
{
	if ( ( NULL != m_pThisWnd ) && ( NULL != expWnd ) && ( NULL != expBar ) )
	{
		char szBuf[COMMON_CLIENT_MSG_LEN_32] = { 0 };
		if ( useFloat )
		{
			_snprintf( 
				szBuf, 
				sizeof( szBuf ), 
				"%.1f/%.1f", 
				static_cast< float >( curExp ) / 3600.0f,
				static_cast< float >( fullExp ) / 3600.0f );
		}
		else
		{
			_snprintf( szBuf, sizeof( szBuf ), "%u/%u", curExp, fullExp );
		}
		
		szBuf[sizeof( szBuf ) - 1] = 0;
		
		expWnd->setText( AnsiToUtf8( szBuf ) );
		
		if ( curExp > fullExp )
		{
			curExp = fullExp;
		}
		
		if ( fullExp != 0 )
		{
			expBar->setProgress( static_cast< float >( curExp ) / static_cast< float >( fullExp ) );
		}
		else
		{
			expBar->setProgress( 0.0f );
		}
		
		char szTip[COMMON_CLIENT_MSG_LEN_1024] = { 0 };
		if ( useFloat )
		{
			_snprintf( 
				szTip, sizeof( szTip ), 
				KMessageCentre::GetMessage( common_message, msgNum ), 
				static_cast< float >( curExp ) / 3600.0f, 
				static_cast< float >( fullExp ) / 3600.f );
		}
		else 
		{
			_snprintf( 
				szTip, sizeof( szTip ), 
				KMessageCentre::GetMessage( common_message, msgNum ), 
				curExp, fullExp );
		}
		szTip[sizeof( szTip ) - 1] = 0;
		
		String strTip = AnsiToUtf8( szTip );
		expBar->setTooltipText( strTip );
	}			
}

void KUiRoleExp::RefreshTip( /*int expRewardState, int timeRewardState*/ )
{
	bool bIsExpInsuranceValid = ( g_pCoreShell->GetGameData( GDI_EXP_INSURANCE_STATE, 0, 0 ) != 0 );
	bool bIsQuestInsuranceValid = ( g_pCoreShell->GetGameData( GDI_QUEST_INSURANCE_STATE, 0, 0 ) != 0 );
	
	if ( bIsExpInsuranceValid )
	{
		InnerUpdateRewardExp();
	}
	else
	{
		int expInsuranceLevel = g_pCoreShell->GetGameData( GDI_EXP_INSURANCE_ENABLE_LEVEL, 0, 0 );
		setTip( m_pRewardExpBar, KUiRoleExp::CM_REWARDEXP_UNDER_LEVEL, expInsuranceLevel );	
	}

	if ( bIsQuestInsuranceValid )
	{
		InnerUpdateRewardTime();		
	}
	else
	{
		int questInsuranceLevel = g_pCoreShell->GetGameData( GDI_QUEST_INSURANCE_ENABLE_LEVEL, 0, 0 );
		setTip( m_pRewardTimeBar, KUiRoleExp::CM_REWARDTIME_UNDER_LEVEL, questInsuranceLevel );
	}
}

void KUiRoleExp::setTip( ProgressBar* expBar, MessageNum msgNum, int minLevel )
{
	if ( NULL == expBar )
	{
		return;
	}

	char szTip[COMMON_CLIENT_MSG_LEN_1024] = { 0 };
	szTip[sizeof( szTip ) - 1] = 0;
	_snprintf( 
		szTip, sizeof( szTip ), 
		KMessageCentre::GetMessage( common_message, msgNum ), 	
		minLevel );
	szTip[sizeof( szTip ) - 1] = 0;
	
	String strTip = AnsiToUtf8( szTip );
	expBar->setTooltipText( strTip );		
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////
/////////

template<> 
KUiCastSkillExp* KUiWndSingleton<KUiCastSkillExp>::ms_Singleton	= NULL;

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiCastSkillExp::KUiCastSkillExp( const CEGUI::String& id_name )
: KUiWndSingleton<KUiCastSkillExp>( id_name )
{
	m_pCastExpSkillBtn		= NULL;
	m_pCastExpSkillCount	= NULL;
	m_pCastExpSkillDisplay	= NULL;
}

KUiCastSkillExp::~KUiCastSkillExp()
{

}

void KUiCastSkillExp::Show( void )
{
	KUiWndSingleton<KUiCastSkillExp>::Show();
}

void KUiCastSkillExp::Init()
{
	if ( m_pThisWnd )
	{
		m_pThisWnd->setZLevel(Window::Bottom);
		m_pCastExpSkillBtn		= (PushButton*)m_pThisWnd->getChild("TaharezLook/CastSkillExp/CastExpSkill");
		m_pCastExpSkillCount	= (StaticImage*)m_pThisWnd->getChild("TaharezLook/CastSkillExp/CastExpSkill/Count");
		m_pCastExpSkillDisplay	= (StaticImage*)m_pThisWnd->getChild("TaharezLook/CastSkillExp/CastExpSkill/Display");
		m_pCastExpSkillBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiCastSkillExp::handleCast, ms_Singleton));
	}
}

void KUiCastSkillExp::UpdateData(DWORD curSkillExp, DWORD fullSkillExp)
{
	if ( m_pThisWnd )
	{
		if ( m_pCastExpSkillBtn && m_pCastExpSkillDisplay && m_pCastExpSkillCount )
		{
			int nCurSpecialSkill = g_pCoreShell->GetGameData( GDI_GET_CUR_SPECIALSKILL_ID, NULL, NULL );
			KSkillInfo tagSkillInfo;
			ZeroMemory( &tagSkillInfo, sizeof(tagSkillInfo) );
			g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, NULL );
			if ( tagSkillInfo.nCostSkillExp > 0 )
			{
				int nExp = curSkillExp / tagSkillInfo.nCostSkillExp;
				if ( nExp <= 0 )
				{
					m_pCastExpSkillCount->stop();
					m_pCastExpSkillDisplay->stop();
				}
				else if ( nExp >= 1 && nExp <= 9 )
				{					
					m_pCastExpSkillCount->play( nExp, nExp, false );
					m_pCastExpSkillDisplay->setCyc( true );
					m_pCastExpSkillDisplay->play();
				}
				else
				{
					m_pCastExpSkillCount->play( 0,0, false );
					m_pCastExpSkillDisplay->setCyc( true );
					m_pCastExpSkillDisplay->play();
				}
			}
		}
	}
}

bool KUiCastSkillExp::handleCast(const CEGUI::EventArgs& args)
{
	int nCurSpecialSkill = g_pCoreShell->GetGameData( GDI_GET_CUR_SPECIALSKILL_ID, NULL, NULL );
	if ( nCurSpecialSkill > 0 )
	{
		int nCurSpecialSkill = g_pCoreShell->GetGameData( GDI_GET_CUR_SPECIALSKILL_ID, NULL, NULL );
		KSkillData skillData( nCurSpecialSkill, false, false );

		//该接口已经不再使用
		//g_pCoreShell->UseSkill( skillData );
	}
	return true;
}


/********************************************************************
/*						class: KUiRewardExpNotify
*********************************************************************/

template<> 
KUiRewardExpNotify* KUiWndSingleton<KUiRewardExpNotify>::ms_Singleton	= NULL;

KUiRewardExpNotify::KUiRewardExpNotify( const CEGUI::String& id_name )
: KUiWndSingleton<KUiRewardExpNotify>( id_name )
{
	m_stRewardExp = NULL;
	m_stRewardTime = NULL;
}

KUiRewardExpNotify::~KUiRewardExpNotify()
{
	
}

void KUiRewardExpNotify::Init()
{
	if ( ( NULL != ms_Singleton ) && ( NULL != m_pThisWnd ) )
	{
		m_stRewardExp = static_cast< StaticText* >( m_pThisWnd->getChild( "TaharezLook/RewardExpNotify/RewardExp" ) );	
		m_stRewardTime = static_cast< StaticText* >( m_pThisWnd->getChild( "TaharezLook/RewardExpNotify/RewardTime" ) );	

		m_pThisWnd->getChild( "TaharezLook/RewardExpNotify/btnOK" )->subscribeEvent(
			PushButton::EventClicked, 
			Event::Subscriber(&KUiRewardExpNotify::btnCancel_MouseClick, this ) );

		m_pThisWnd->getChild( "TaharezLook/RewardExpNotify/btnClose" )->subscribeEvent(
			PushButton::EventClicked, 
			Event::Subscriber(&KUiRewardExpNotify::btnCancel_MouseClick, this ) );
	}
}

bool KUiRewardExpNotify::btnCancel_MouseClick( const CEGUI::EventArgs& e )
{
	Hide();
	return true;
}

void KUiRewardExpNotify::Notify( DWORD rewardExp, DWORD rewardTime )
{
	if ( ( NULL != m_stRewardExp ) && ( NULL != m_stRewardTime ) )
	{
		if ( -1 != rewardTime )
		{
			m_stRewardTime->setText( iToString( rewardTime ) );
		}
		
		if ( -1 != rewardExp )
		{
			m_stRewardExp->setText( iToString( rewardExp ) );
		}
	}

	if ( ! IsVisible() )
	{
		Show();
	}
}

void KUiRewardExpNotify::Hide()
{
	if ( ( NULL != ms_Singleton ) && ( NULL != ms_Singleton->m_pThisWnd ) && ( NULL != ms_Singleton->m_stRewardTime ) && ( NULL != ms_Singleton->m_stRewardExp ) )
	{
		ms_Singleton->m_stRewardTime->setText( iToString( 0 ) );
		ms_Singleton->m_stRewardExp->setText( iToString( 0 ) );
	}

	KUiWndSingleton<KUiRewardExpNotify>::Hide();
}