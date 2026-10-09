//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/09/2008 
//      File_base        : UICharts.h
//      File_ext         : cpp
//      Author           : marryme (Chen Lin)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////


#include "UiRankButton.h"
//#include "CoreShell.h"
//#include "EmplomentDataDef.h"
//#include "../UiConfigManager.h"
#include "UiPointListCharts.h"
#include "UiMapCentre.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


template<> 
KUiRankButton* KUiWndSingleton< KUiRankButton >::ms_Singleton = NULL;

KUiRankButton::KUiRankButton( const CEGUI::String& id_name )
: KUiWndSingleton<KUiRankButton>( id_name )
, m_Button( NULL )
{}

KUiRankButton::~KUiRankButton()
{}

void KUiRankButton::Init()
{
	try
	{
		m_Button = static_cast< TLButton* >( ms_Singleton->m_pThisWnd->getChild( "TaharezLook/RankBtn/Btn" ) );
	}
	catch ( ... )
	{
		return ;
	}
	m_Button->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiRankButton::handleButtonEvent, ms_Singleton));

}

bool KUiRankButton::handleButtonEvent( const CEGUI::EventArgs& e )
{
	if( !KUiPointListCharts::GetSingleton().IsVisible() )
		KUiPointListCharts::Show();
	else
		KUiPointListCharts::Hide();
 	return true;
}

void KUiRankButton::MoveToRightEdge()
{
	if ( NULL != ms_Singleton && NULL != m_pThisWnd )
	{
		int selfWidth = m_pThisWnd->getWidth(Absolute);
		
		int rightEdge_X = g_GetScreenWidth();
		if ( KUiMiniMap::IsVisible() )
		{
			rightEdge_X = KUiMiniMap::GetSingleton().GetMiniMapWndPaintX();	
		}
		int selfPositionY = m_pThisWnd->getYPosition(Absolute);
		Point newPosition(rightEdge_X - selfWidth, selfPositionY);
		m_pThisWnd->setPosition(Absolute, newPosition);
	}
}