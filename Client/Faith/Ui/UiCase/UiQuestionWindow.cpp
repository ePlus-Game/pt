// UiIEWindow.cpp: implementation of the KUiQuestionWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "UiQuestionWindow.h"
#include "CoreShell.h"
#include "UiChatWindow.h"
#include "UiIEWindow.h"
#include "GameDataDef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

template<> 
KUiQuestionWindow* KUiWndSingleton<KUiQuestionWindow>::ms_Singleton	= NULL;

KUiQuestionWindow::KUiQuestionWindow(const CEGUI::String& id_name)
: KUiWndSingleton<KUiQuestionWindow>( id_name )
{
	m_pQuestionWnd = NULL;
	m_currentSec = 0;
}

KUiQuestionWindow::~KUiQuestionWindow()
{
}

void KUiQuestionWindow::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setRenderMode( false, 3 );

		m_pQuestionWnd = static_cast< TLQuestionWindow* >( m_pThisWnd->getChild( "TaharezLook/UiQuestionWindow/Container" ) );
		
		m_pQuestionWnd->subscribeEvent(
			Window::EventAnswerCommitted, 
			Event::Subscriber( &KUiQuestionWindow::wndQuestion_AnswerCommited, this ) );

		m_pQuestionWnd->subscribeEvent(
			Window::EventAnserTimeUp, 
			Event::Subscriber( &KUiQuestionWindow::wndQuestion_AnswerCommited, this ) );
		
		ms_Singleton->m_pThisWnd->setZLevel( Window::SuperSurperTop );
		
		m_pThisWnd->getChild( "TaharezLook/UiQuestionWindow/btnClose" )->subscribeEvent(
			PushButton::EventClicked, 
			Event::Subscriber( &KUiQuestionWindow::BtnClose_Click, this ) );
	}
}

void	KUiQuestionWindow::Show( void )
{
	KUiIEWindow::Hide();
	KUiWndSingleton<KUiQuestionWindow>::Show();
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setModalState(true);
	}
}

void	KUiQuestionWindow::Hide( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setModalState(false);
	}
	if ( ( NULL != ms_Singleton ) && ( NULL != ms_Singleton->m_pQuestionWnd ) )
	{
		ms_Singleton->m_pQuestionWnd->SendEmptyAnswer();
	}
	KUiWndSingleton<KUiQuestionWindow>::Hide();
}

void KUiQuestionWindow::Toggle()
{
	if( NULL != m_pThisWnd )
	{
		if( m_pThisWnd->isVisible() )
		{
			Hide();
		}
		else
		{
			Show();
		}
	}	
}

void KUiQuestionWindow::ShowQuestion( unsigned int uParam, int nParam )
{
	bool bCreateFileSuccessed = false;

	UIQuestionData* questionData = (UIQuestionData*) uParam;
	if ( questionData && m_pQuestionWnd )
	{
		m_pQuestionWnd->setTotalTime( questionData->Timeout );
		bCreateFileSuccessed = m_pQuestionWnd->RefreshFile( uParam );
	}

	if ( KUiQuestionWindow::IsVisible() )
	{
		KUiQuestionWindow::Hide();
	}

	if ( bCreateFileSuccessed )
	{
		KUiQuestionWindow::Show();
	}
}

bool KUiQuestionWindow::BtnClose_Click( const EventArgs& args )
{
	Hide();
	return true;
}

bool KUiQuestionWindow::wndQuestion_AnswerCommited( const EventArgs& args )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setModalState(false);
	}
	KUiWndSingleton<KUiQuestionWindow>::Hide();	
	return true;
}