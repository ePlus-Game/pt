//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/13/2006 23:43
//      File_base        : UiESCDlg
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UIESCDLG_H
#define UIESCDLG_H

#include "../UiCommon.h"
#include "TLButton.h"
#include "CEGUI.h"

///render add
#define UIEXIT_GAME_HELP_NAME  "TaharezLook/ExitGame/GameHelp"
//end
class KUiExit : public KUiWndSingleton<KUiExit>
{
public:
	KUiExit(  const CEGUI::String& id_name );
	~KUiExit(								);
	
public:
	void	Init();
	static void KUiExit::Show( void );

private:
    bool						handleExit	 ( const CEGUI::EventArgs& args		);
	bool						handleCancel ( const CEGUI::EventArgs& args		);	
	bool						handleKeyDown( const CEGUI::EventArgs& args		);
	bool						handleOption ( const CEGUI::EventArgs& args     );
	bool						handleSKSetting ( const CEGUI::EventArgs& args  );
	bool						handleQuitToSelectRole( const CEGUI::EventArgs& args );
	bool                        handleGameHelButton(const CEGUI::EventArgs& args);
	bool                        onExpHire(const CEGUI::EventArgs& args);
	bool                        onFighterHire(const CEGUI::EventArgs& args);
	
private:
	static const unsigned int	ExitButtonID;
	static const unsigned int	CancelButtonID;
	CEGUI::TLButton				*d_pOptionBtn;
	CEGUI::TLButton				*d_skSettings;
};


#endif