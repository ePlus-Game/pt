//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/12/2006 14:08
//      File_base        : UiHealthGame
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UIHEALTHGAME_H
#define UIHEALTHGAME_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "TLVertScrollbar.h"

class KUiUpdateTip : public KUiWndSingleton<KUiUpdateTip>
{
public:
    KUiUpdateTip( const CEGUI::String& id_name );
    ~KUiUpdateTip(								);
public:
	virtual void Init( void );

private:
	bool						handleScroll	( const CEGUI::EventArgs& args		);
    bool						handleOk		( const CEGUI::EventArgs& args		);
	bool						handleCancel	( const CEGUI::EventArgs& args		);
	bool						handleKeyDown	( const CEGUI::EventArgs& args		);
	bool						handleOnShow	( const CEGUI::EventArgs& args		);

private:
	TLVertScrollbar*			d_scrollBar;
	TLStaticText*				d_text;
	TLButton*					d_acceptBtn;
};

#endif 
