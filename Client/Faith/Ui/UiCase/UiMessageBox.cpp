///////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/13/2006 11:20
//      File_base        : UiMessageBox
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "kwin32.h"
#include "UiMessageBox.h"

using namespace CEGUI;

template<> 
KUiCommonMsgBox* KUiWndSingleton<KUiCommonMsgBox>::ms_Singleton	= NULL;

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiCommonMsgBox::KUiCommonMsgBox( const CEGUI::String& id_name ):
KUiWndSingleton<KUiCommonMsgBox>( id_name )
{
	m_nDesireLoginStatus	= -1;			
	m_nMsgLen				= 0;			
	m_nParam				= 0;			
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiCommonMsgBox::~KUiCommonMsgBox()
{

}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void KUiCommonMsgBox::OpenWindow( const char* szMessage )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd && szMessage )
	{
		ms_Singleton->m_pThisWnd->setAnsiText( szMessage );
		ms_Singleton->Show();
	}
}

