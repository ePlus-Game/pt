//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/27/2007 9:50
//      File_base        : UiLevelUp
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 登陆等待
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "UiLevelUp.h"
#include "UiLevelUpInfo.h"
#include "UiItemTip.h"
#include "GameDataDef.h"
#include "..\KMessageCentre.h"
#include "CoreShell.h"

extern iCoreShell* g_pCoreShell;

template<>
KUiLevelUp* KUiWndSingleton<KUiLevelUp>::ms_Singleton = NULL;

KUiLevelUp::KUiLevelUp( const CEGUI::String& id_name )
: KUiWndSingleton<KUiLevelUp>( id_name )
, m_pImage(NULL)
{
	ZeroMemory( m_szLevelUp, MAX_TEXT_LEN );
}

KUiLevelUp::~KUiLevelUp()
{
}

void KUiLevelUp::Show()
{
	KUiWndSingleton<KUiLevelUp>::Show();
}

void KUiLevelUp::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setRenderMode( false, 3 );
		m_pImage = static_cast<StaticImage*>(ms_Singleton->m_pThisWnd);
		ms_Singleton->m_pThisWnd->subscribeEvent( StaticImage::EventMouseEnters, Event::Subscriber(&KUiLevelUp::handleMouseEntres, ms_Singleton) );
		ms_Singleton->m_pThisWnd->subscribeEvent( StaticImage::EventMouseLeaves, Event::Subscriber(&KUiLevelUp::handleMouseLeaves, ms_Singleton) );
		ms_Singleton->m_pThisWnd->subscribeEvent( StaticImage::EventMouseClick, Event::Subscriber(&KUiLevelUp::handleMouseClicks, ms_Singleton) );
	}
}

bool KUiLevelUp::handleMouseEntres( const CEGUI::EventArgs& args )
{
	//Point p(-50, -50);
	//rect = m_pImage->getRect(Absolute).offset( p ); 
	Rect rect = m_pImage->getRect(Absolute);
	KUiItemTip::GetSingleton();
	KUiItemTip::GetSingleton().show( m_szLevelUp, rect, KUiItemTip::Top);

	return true;
}

bool KUiLevelUp::handleMouseLeaves( const CEGUI::EventArgs& args )
{
	KUiItemTip::Hide();
	return true;
}

bool KUiLevelUp::handleMouseClicks( const CEGUI::EventArgs& args )	
{
	KUiLevelUpInfo::Show();
	KUiItemTip::Hide();
	m_pThisWnd->hide();
	return true;
}	

void KUiLevelUp::GetPlayerInfo( const char* szName, const int level )
{
	ZeroMemory(m_szLevelUp, MAX_TEXT_LEN);
	sprintf( m_szLevelUp, " <Layout width=250><Seg text-align=left float=wrap><Obj color=0,255,0 font-family=stzhongs-9>%s</Obj><Obj color=250,100,0 font-family=stzhongs-9>%s</Obj> ", 
			 KMessageCentre::GetMessage(levelup_info, 1), szName);

	char szlevel[3];
	strcat( m_szLevelUp, "<Obj color=0,255,0 font-family=stzhongs-9>");
	strcat( m_szLevelUp, KMessageCentre::GetMessage(levelup_info, 2) );
	strcat( m_szLevelUp, "</Obj>" );
	strcat( m_szLevelUp, "<Obj color=250,100,0 font-family=stzhongs-9>");
	strcat( m_szLevelUp, itoa(level, szlevel, 10) );
	strcat( m_szLevelUp, "</Obj>" );
	strcat( m_szLevelUp, "<Obj color=0,255,0 font-family=stzhongs-9>");
	strcat( m_szLevelUp, KMessageCentre::GetMessage(levelup_info, 3) );
	strcat( m_szLevelUp, "</Obj></Seg></Layout>" );
}