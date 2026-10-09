//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/12/2006 14:10
//      File_base        : UiHealthGame
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "UiUpdateTip.h"
#include "UiLogin.h"
#include "KWin32Wnd.h"
#include "../../Login/Login.h"

using namespace CEGUI;

template<> 
KUiUpdateTip* KUiWndSingleton<KUiUpdateTip>::ms_Singleton	= NULL;

KUiUpdateTip::KUiUpdateTip( const CEGUI::String& id_name ):
KUiWndSingleton<KUiUpdateTip>( id_name )
{
}


KUiUpdateTip::~KUiUpdateTip()
{

}

void KUiUpdateTip::Init( void )
{
	m_pThisWnd->setRenderMode( false, 3 );

	m_pThisWnd->getChild("TaharezLook/UpdateTip/Login")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiUpdateTip::handleOk, this));

    m_pThisWnd->getChild("TaharezLook/UpdateTip/Exit")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiUpdateTip::handleCancel, this));


	d_text = (TLStaticText* )m_pThisWnd->getChild( "TaharezLook/UpdateTip/Text" );
	if ( d_text )
	{
		d_text->useLayout();
		//裁剪区域必须是相对底板的位置
		Rect textArea = d_text->getUnclippedInnerRect();
		Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 		textArea.setPosition(posOff);

		LORect clipper;
		cerectToLorect(&textArea, &clipper);
		d_text->getLayout()->setClipper(clipper);
		d_text->getLayout()->SetText( Utf8ToAnsi( d_text->getText() ) );
		d_text->setText("");
	}

	d_scrollBar = (TLVertScrollbar*)m_pThisWnd->getChild("TaharezLook/UpdateTip/Scrollbar");
	d_scrollBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
		Event::Subscriber(&KUiUpdateTip::handleScroll, this));
	
	m_pThisWnd->subscribeEvent(StaticImage::EventShown, 
		Event::Subscriber(&KUiUpdateTip::handleOnShow, this));

	d_acceptBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/UpdateTip/Login");

	m_pThisWnd->setModalState( true );
}

bool KUiUpdateTip::handleScroll( const CEGUI::EventArgs& args )
{
	if ( d_text )
	{
		int layoutHeight = d_text->getLayout()->getRenderArea().getHeight();
		int clipperHeight = d_text->getHeight(Absolute) 
			- d_text->getTopFrameHeight() - d_text->getBottomFrameHeight();
		
		if(layoutHeight <= clipperHeight)
		{
			d_acceptBtn->enable();
			return true;
		}

		float scrollPos = d_scrollBar->getScrollPosition();
		
		int yPos = (layoutHeight - clipperHeight) * scrollPos;

		d_text->setLayoutOffset(0, -yPos + d_text->getTopFrameHeight());
		if ( scrollPos == 1 )
		{
			d_acceptBtn->enable();
		}
	}
	return true;
}

bool KUiUpdateTip::handleOk( const CEGUI::EventArgs& args )
{
	KIniFile ini;
//<----EXVERSION
	if ( ini.Load( VERSION_CFG ) )
	{
		char curVersion[32];
		int nMajorVersion = 0;
		int nVersion = 0;
		int nNeedVersion = 0;
		ini.GetInteger( "Version", "MajorVersion", 0, &nMajorVersion );
		ini.GetInteger( "Version", "Version", 0, &nVersion );
		ini.GetInteger( "Version", "NeedVersion", 0, &nNeedVersion );
		sprintf( curVersion, "%d,%d,%d", nMajorVersion, nVersion, nNeedVersion );

		KIniFile ini2;
		if ( ini2.Load( CONFIG_INI ) )
		{	
			ini2.WriteString("LastLogin",LAST_VERSION, curVersion );
			ini2.Save( CONFIG_INI );
		}
	}

	Hide();
	m_pThisWnd->setModalState( false );
	return true;
}

bool KUiUpdateTip::handleCancel( const CEGUI::EventArgs& args )
{
	if ( g_GetMainApp() )
	{
		g_GetMainApp()->StopApplication();
	}
    return true;
}

bool KUiUpdateTip::handleKeyDown(const CEGUI::EventArgs& args)
{
    using namespace CEGUI;

    switch (static_cast<const KeyEventArgs&>(args).scancode)
    {
    case Key::Escape:
        break;
    case Key::Return:
        break;
    default:
        return false;
    }

    return true;
}

bool KUiUpdateTip::handleOnShow(const CEGUI::EventArgs& args)
{

	if ( d_acceptBtn )
	{
		d_acceptBtn->disable();
		d_scrollBar->setScrollPosition(0);
	}
	return true;
}