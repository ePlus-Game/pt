// UiIEWindow.cpp: implementation of the KUiIEWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "UiIEWindow.h"
#include "CoreShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

template<> 
KUiIEWindow* KUiWndSingleton<KUiIEWindow>::ms_Singleton	= NULL;

KUiIEWindow::KUiIEWindow(const CEGUI::String& id_name)
: KUiWndSingleton<KUiIEWindow>( id_name )
{
}

KUiIEWindow::~KUiIEWindow()
{
}

void KUiIEWindow::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->subscribeEvent(StaticImage::EventKeyDown, Event::Subscriber(&KUiIEWindow::handleKeyDown, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/IEWnd/IE")->subscribeEvent(StaticImage::EventKeyDown, Event::Subscriber(&KUiIEWindow::handleKeyDown, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/IEWnd/Close")->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiIEWindow::btnClose_MouseClick, ms_Singleton));
	}
}

void	KUiIEWindow::Show( void )
{
	KUiWndSingleton<KUiIEWindow>::Show();
}

void	KUiIEWindow::Hide( void )
{
	KUiWndSingleton<KUiIEWindow>::Hide();
	if ( g_pCoreShell )
	{
		g_pCoreShell->OperationRequest( GOI_IBSHOP_CHONGZHI, NULL, NULL );
	}
}

bool KUiIEWindow::handleKeyDown(const CEGUI::EventArgs& args)
{
    using namespace CEGUI;
	
    switch (static_cast<const KeyEventArgs&>(args).scancode)
    {
    case Key::Escape:
		{
			KUiIEWindow::Hide();
		}
		break;
	}
	return true;
}

bool KUiIEWindow::btnClose_MouseClick( const CEGUI::EventArgs& args )
{
	KUiIEWindow::Hide();
	return true;	
}
