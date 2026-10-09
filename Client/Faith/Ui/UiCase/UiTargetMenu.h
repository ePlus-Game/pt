//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 04/02/2007 10:52
//      File_base        : UiTargetMenu
//      File_ext         : cpp
//      Author           : likun
//      Description      : Ä¿±ê²Ëµ¥
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UITARGETMENU_H
#define UITARGETMENU_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "TLButton.h"
#include "GameDataDef.h"

class KUiTargeMenu :public KUiWndSingleton<KUiTargeMenu>
{
public:
	KUiTargeMenu( const CEGUI::String& id_name );
	~KUiTargeMenu();
	static  void  Show( void );
	static  void  SetWinPosition(const CEGUI::Point &pos );
	void	SetPlayer( const KUiPlayerItem &player);
	void	Init( void );
	float	GetWinHeight() const;
protected:
	bool	handleChat( const CEGUI::EventArgs &args );
	bool	handleFriend( const CEGUI::EventArgs &args );
	bool	handleTrade( const CEGUI::EventArgs &args );
	bool	handleFollow( const CEGUI::EventArgs &args );
	bool	handleEquip( const CEGUI::EventArgs &args );
	bool	handleBlaceList( const CEGUI::EventArgs &args );
	bool	handleGroup( const CEGUI::EventArgs &args );

private:
	void	InitRegBnt( void );
	CEGUI::TLButton	*d_pChatBtn;
	CEGUI::TLButton	*d_pFriendBtn;
	CEGUI::TLButton *d_pTradeBtn;
	CEGUI::TLButton *d_pFollowBtn;
	CEGUI::TLButton *d_pEquipBtn;
	CEGUI::TLButton *d_pBlackListBtn;
	CEGUI::TLButton *d_pPrentenBtn;
	KUiPlayerItem	d_player;
	KTargetInfo		d_targetInfo;
};
#endif