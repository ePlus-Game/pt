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
#ifndef KUIDEATHBOX_H
#define KUIDEATHBOX_H

#include "..\UiCommon.h"
#include "CEGUI.h"

class KUiDeathBox : public KUiWndSingleton<KUiDeathBox>
{
public:
	KUiDeathBox( const CEGUI::String& id_name );
	~KUiDeathBox();
public:
	static void Show( void );
	void	    Init();
	bool        handleRenascence( const CEGUI::EventArgs& args );
	bool        handleLocalRevive( const CEGUI::EventArgs & args);
private:
	char			m_szMessage[256];		//!< Tip message.
	int				m_nMsgLen;				//!< Tip message length.
	TLButton *      m_LocalRevival;
};

#endif	