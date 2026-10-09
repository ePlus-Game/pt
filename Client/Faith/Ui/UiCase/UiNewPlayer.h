//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/15/2006 13:21
//      File_base        : UiNewPlayer
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UINEWPLAYER_H
#define UINEWPLAYER_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "../../Login/Login.h"

class KUiNewPlayer;

class KUiNewPlayerInfo : public KUiWndSingleton<KUiNewPlayerInfo>
{
	friend KUiNewPlayer;
public:
	KUiNewPlayerInfo( const CEGUI::String& id_name	);
	~KUiNewPlayerInfo(								);

public:
	static void					Show			( void								);
	static void					Hide			( void								);
	//void						SetRoleNum		( int iNum							);
	void						setFileName		( char *iniFileName					);

	void						SetRoleNameActive( void								);
private:
	bool						handleKeyDown	( const CEGUI::EventArgs& args		);
	bool						CherkInputInfo	( void								);
	void						SetRoleInfo		( int nGenre, BYTE byAttribute		);
	void						SetRolePortrait	( int nPortait						);
    bool						handleCreate	( const CEGUI::EventArgs& args		);
	bool						handleBack		( const CEGUI::EventArgs& args		);	
	
private:
	static const unsigned int	CreateButtonID;
	static const unsigned int	CancelButtonID;
	KRoleChiefInfo				m_pNewRole;
	//likun 已有角色数
	int							m_iRoleNum;
	char						m_filename[COMMON_CLIENT_MSG_LEN_128];
};

class KUiNewPlayer : public KUiWndSingleton<KUiNewPlayer>
{
public:
	KUiNewPlayer( const CEGUI::String& id_name	);
	~KUiNewPlayer(								);

public:
	static void					Show			( void								);
	static void					Hide			( void								);
	virtual void				Init			( void								);

private:
	bool						handleKeyDown	( const CEGUI::EventArgs& args		);
	bool						HandleRoleA		( const CEGUI::EventArgs& args		);
	bool						HandleRoleB		( const CEGUI::EventArgs& args		);
	bool						HandleRoleC		( const CEGUI::EventArgs& args		);
	bool						HandleRoleD		( const CEGUI::EventArgs& args		);
	bool						HandleRoleE		( const CEGUI::EventArgs& args		);
	bool						HandleRoleF		( const CEGUI::EventArgs& args		);
	void						selectRole		( int nRoleKind, int nGender		);
	
	void						showSelRole		( const CEGUI::String& name, const CEGUI::String& path );
	void						SelectPortrait	( int nGender, int nPortrait );
	bool						HandleRoleSelectPortraitL( const CEGUI::EventArgs& args		);
	bool						HandleRoleSelectPortraitR( const CEGUI::EventArgs& args		);
	void						showClickRole	( const CEGUI::String& name, const CEGUI::String& path );
	
	void						initRolePortrait();
	bool						onClickPortrait( const CEGUI::EventArgs& args );
	bool						onRolePortraitPanelScroll( const CEGUI::EventArgs& args ) ;
	bool						onRolePortraitMouseWheel( const CEGUI::EventArgs& args ) ;
	void						SelectAllPortrait( int nGender);

private:
	static const unsigned int	BackButtonID;
	TLStaticImage*				d_roleFace;
	int							d_curPortrait;

	TLStaticImage*				d_pRoleImage;
	TLStaticImage*				d_pClickRoleImage;
		
	Window*						d_SelPortraitWnd;
};

#endif
