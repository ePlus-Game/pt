//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/08/2007 19:08
//      File_base        : UiBeginHelp
//      File_ext         : h
//      Author           : likun
//      Description      : 玩家第一次登陆后出现的UI帮助界面
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "UiBeginHelp.h"
#include "CoreUseNameDef.h"
#include "KIniFile.h"

using namespace CEGUI;

template<>
KUiBeginHelp *KUiWndSingleton<KUiBeginHelp>::ms_Singleton = NULL;

KUiBeginHelp::KUiBeginHelp( const CEGUI::String& id_name )
: KUiWndSingleton<KUiBeginHelp>(id_name)
{
	
}

KUiBeginHelp::~KUiBeginHelp()
{

}

void KUiBeginHelp::Show()
{
	if ( !ms_Singleton->d_bFirstLogin && 
		 ms_Singleton != NULL && 
		 ms_Singleton->m_pThisWnd != NULL )
	{
		KUiWndSingleton<KUiBeginHelp>::Show();
		KIniFile iniFile;
		iniFile.Load(ms_Singleton->d_fileName);
		iniFile.WriteInteger(ms_Singleton->d_roleName, ROLE_FIRST_LOGIN, 1);
		ms_Singleton->d_bFirstLogin = true;
		iniFile.Save(ms_Singleton->d_fileName);
	}
}

void KUiBeginHelp::Init()
{
	m_pThisWnd->setRenderMode(true);
	m_pThisWnd->subscribeEvent(	Window::EventMouseClick, 
								Event::Subscriber( &KUiBeginHelp::handleClick, this));
}

void KUiBeginHelp::setbRoleFirstLogin( bool bFirstLogin, 
									   const char *fileName, 
									   const char *roleName )
{ 
	d_bFirstLogin = bFirstLogin; 

	if ( fileName == NULL || roleName == NULL )
		return;

	strcpy( d_fileName, fileName );
	strcpy( d_roleName, roleName);
}

bool KUiBeginHelp::handleClick(const CEGUI::EventArgs& args)
{
	Hide();
	return true;
}

void KUiBeginHelp::RepeatShow( void )
{
	setAlwayTop(true);
	KUiWndSingleton<KUiBeginHelp>::Show();
}

void KUiBeginHelp::setAlwayTop(bool bTop)
{
	if ( m_pThisWnd )
	{
		m_pThisWnd->setZLevel(Window::Top);
	}
}