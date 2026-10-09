//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/30/2006 11:44
//      File_base        : UiState
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KWin32Wnd.h"
#include "UiHelpCentre.h"
#include "CoreUseNameDef.h"
#include "CoreShell.h"
extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiHelpCentre* KUiWndSingleton<KUiHelpCentre>::ms_Singleton	= NULL;

KUiHelpCentre::KUiHelpCentre():
KUiWndSingleton<KUiHelpCentre>( "uisettings/layouts/ClientHand.ls" )
{
}

KUiHelpCentre::~KUiHelpCentre()
{

}

void KUiHelpCentre::SetDragObject( TLGameObject::GameObject& rGO )
{
	ms_Singleton->m_pClientHand->setObject( rGO );
}

void KUiHelpCentre::GetDragObject( const TLGameObject::GameObject& rGO )
{
	ms_Singleton->m_pClientHand->clear();
}

void KUiHelpCentre::Show( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		try
		{
			ms_Singleton->m_pClientHand = static_cast<TLGameObject*>(ms_Singleton->m_pWindowManager->getWindow("TaharezLook/ClientHand"));
		}
		catch ( ... )
		{
			ms_Singleton->m_pClientHand = static_cast<TLGameObject*>(ms_Singleton->m_pWindowManager->loadWindowLayout("uisettings/layouts/ClientHand.ls"));
		}
		
		ms_Singleton->m_pClientHand->SetHand( true );
	}
}

