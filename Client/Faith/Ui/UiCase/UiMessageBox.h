//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/13/2006 11:20
//      File_base        : UiMessageBox
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUICOMMOMMSGBOX_H
#define KUICOMMOMMSGBOX_H

#include "..\UiCommon.h"
#include "CEGUI.h"

class KUiCommonMsgBox : public KUiWndSingleton<KUiCommonMsgBox>
{
public:
	KUiCommonMsgBox( const CEGUI::String& id_name );
	~KUiCommonMsgBox();
public:
	static void	OpenWindow( const char* strMessage		);
	static void	OpenWindow( int nMsgType, int nMsgCode	);
private:
	int				m_nDesireLoginStatus;	//!< Desire login status. 
	char			m_szMessage[256];		//!< Tip message.
	int				m_nMsgLen;				//!< Tip message length.
	int				m_nParam;				//!< Plus param 
};

#endif	