//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/08/2006 11:12
//      File_base        : UiPKFilter
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : PK◊¥Ã¨…Ë÷√≤Àµ•
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "UiPKFilter.h"
#include "CoreUseNameDef.h"
#include "Coreshell.h"
#include "UiRoleFace.h"

extern iCoreShell*		g_pCoreShell;

using namespace CEGUI;

template<> 
KUiPKFilter* KUiWndSingleton<KUiPKFilter>::ms_Singleton	= NULL;

KUiPKFilter::KUiPKFilter( const CEGUI::String& id_name ):
KUiWndSingleton<KUiPKFilter>( id_name )
{
}

KUiPKFilter::~KUiPKFilter()
{
	
}

bool	KUiPKFilter::handlePK0( const CEGUI::EventArgs& args )
{
	SetPKState( pk_peace );
	KUiRoleFace::SetData( pk_peace );
	return true;
}

bool	KUiPKFilter::handlePK1( const CEGUI::EventArgs& args )
{
	SetPKState( pk_team );
	KUiRoleFace::SetData( pk_team );
	return true;
}

bool	KUiPKFilter::handlePK2( const CEGUI::EventArgs& args )
{
	SetPKState( pk_gens );
	KUiRoleFace::SetData( pk_gens );
	return true;
}

bool	KUiPKFilter::handlePK3( const CEGUI::EventArgs& args )
{
	SetPKState( pk_friendevil );
	KUiRoleFace::SetData( pk_friendevil );
	return true;
}

bool	KUiPKFilter::handlePK4( const CEGUI::EventArgs& args )
{
	SetPKState( pk_whole );
	KUiRoleFace::SetData( pk_whole );
	return true;
}

bool	KUiPKFilter::handlePK5( const CEGUI::EventArgs& args )
{
	SetPKState( pk_guard );
	KUiRoleFace::SetData( pk_guard );
	return true;
}

bool	KUiPKFilter::handlePK6( const CEGUI::EventArgs& args )
{
	SetPKState( pk_monster );
	KUiRoleFace::SetData( pk_monster );
	return true;
}

bool	KUiPKFilter::handlePK7( const CEGUI::EventArgs& args )
{
	SetPKState( pk_tong );
	KUiRoleFace::SetData( pk_tong );
	return true;
}

bool KUiPKFilter::handlePK8( const CEGUI::EventArgs& args )
{
	SetPKState( pk_league );
	KUiRoleFace::SetData( pk_league );
	return true;	
}

void	KUiPKFilter::SetPKState( int nPK )
{
	g_pCoreShell->OperationRequest( GOI_PK_SETTING, nPK, NULL );
	Hide();
}

void	KUiPKFilter::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK0")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiPKFilter::handlePK0, ms_Singleton) ) ;
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK1")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiPKFilter::handlePK1, ms_Singleton) ) ;
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK2")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiPKFilter::handlePK2, ms_Singleton) ) ;
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK3")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiPKFilter::handlePK3, ms_Singleton) ) ;
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK4")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiPKFilter::handlePK4, ms_Singleton) ) ;
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK5")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiPKFilter::handlePK5, ms_Singleton) ) ;
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK6")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiPKFilter::handlePK6, ms_Singleton) ) ;
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK7")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiPKFilter::handlePK7, ms_Singleton) ) ;
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK8")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiPKFilter::handlePK8, ms_Singleton) ) ;

		ms_Singleton->d_pkIndex[pk_peace]		=	(PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK0");
		ms_Singleton->d_pkIndex[pk_team]		=	(PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK1");
		ms_Singleton->d_pkIndex[pk_gens]		=	(PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK2");
		ms_Singleton->d_pkIndex[pk_friendevil]	=	(PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK3");
		ms_Singleton->d_pkIndex[pk_whole]		=	(PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK4");
		ms_Singleton->d_pkIndex[pk_guard]		=	(PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK5");
		ms_Singleton->d_pkIndex[pk_monster]		=	(PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK6");
		ms_Singleton->d_pkIndex[pk_tong]		=	(PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/PKFilter/PK7");
	}
}

void	KUiPKFilter::Show( void )
{
	KUiWndSingleton<KUiPKFilter>::Show();
}

void	KUiPKFilter::UpdatePKState( int nPK )
{
	/*ms_Singleton->d_pkIndex[pk_peace]->setEnabled( true );
	ms_Singleton->d_pkIndex[pk_team]->setEnabled( true );
	ms_Singleton->d_pkIndex[pk_society]->setEnabled( true );
	ms_Singleton->d_pkIndex[pk_friendevil]->setEnabled( true );
	ms_Singleton->d_pkIndex[pk_whole]->setEnabled( true );
	ms_Singleton->d_pkIndex[pk_guard]->setEnabled( true );
	ms_Singleton->d_pkIndex[pk_monster]->setEnabled( true );
	ms_Singleton->d_pkIndex[nPK]->setEnabled( false );*/
}

