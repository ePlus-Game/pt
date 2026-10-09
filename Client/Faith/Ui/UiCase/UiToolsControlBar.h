//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/10/2006 11:44
//      File_base        : UiToolsControlBar
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UITOOLSCONTROLBAR_H
#define UITOOLSCONTROLBAR_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "CoreShell.h"
#include "TLStatic.h"
#include "../UiConfigManager.h"
#include "TLButton.h"
#define MAX_SKILL_COUNT 8
using namespace CEGUI;

enum	MOUSE_CURRENT_STATUS
{
	MOUSE_FRIEND_STATUS,
	MOUSE_TRADE_STATUS,
	MOUSE_LOOK_EQUIPMENT,
	MOUSE_BLACK_LIST,
	MOUSE_TEAM_STATUS,
	MOUSE_CHAT_STATUS,
	MOUSE_FOLLOW_STATUS,
	MOUSE_NORMAL_STATUS,
	MOUSE_EDIT_STATUS,
};

class KUiMiniNaviation : public KUiWndSingleton<KUiMiniNaviation>
{
	enum MINITOOLBAR_STATUS_MESSAGE
	{
		BIND_SCREEN_MESSAGE = 1,
		BIND_TRADE_MESSAGE,
		BIND_LOOK_EQUIPMENT,
		BIND_TEAM_MESSAGE,
		BIND_CHAT_MESSAGE,
		BIND_FOLLOW_MESSAGE,
		BIND_ADD_FRIEND_MESSAGE,
	};
public:
	KUiMiniNaviation(  const CEGUI::String& id_name );
	~KUiMiniNaviation(								);
	void	Init( void );
	MOUSE_CURRENT_STATUS	getMiniNavMouseSatatus( void );
	void	setMiniNavMouseStatus( MOUSE_CURRENT_STATUS mouseStatus );
private:
    bool	handleFriend ( const CEGUI::EventArgs& args	);
    bool	handleTeam( const CEGUI::EventArgs& args );
	bool	handleLookEquipment( const CEGUI::EventArgs &args );
	bool	handleChat( const CEGUI::EventArgs &args );
	bool    handleFollow( const CEGUI::EventArgs &args );
	bool	handleBlack( const CEGUI::EventArgs &args );
	bool	handleTrade( const CEGUI::EventArgs &args );
private:
	static unsigned int FriendMgrID;
	TLButton	*d_pInfoBtn;
	TLButton	*d_pFriendBtn;
	TLButton	*d_pEquipmentBtn;
	TLButton	*d_pTeamBtn;
	TLButton	*d_pChatBtn;
	TLButton	*d_pFollowBtn;
	TLButton	*d_pTradeBtn;
	TLButton	*d_pBlackBtn;
	MOUSE_CURRENT_STATUS	d_miniMouseStatus;	
};


class KUiNaviation : public KUiWndSingleton<KUiNaviation>
{
public:
	KUiNaviation(  const CEGUI::String& id_name );
	~KUiNaviation(								);
	static void					Show					( void							);
	static void					Hide					( void							);
	void						Init					( void							);
	void						UpdateNetInfo			( DWORD nPing					);
	void						Breath();
	void						PickUpObject			( void							);
	void						ActiveButton			( int index						);
private:
    bool						handleTalisman			( const CEGUI::EventArgs& args	);
	bool						handleSkillManage		( const CEGUI::EventArgs& args	);
	bool						handleItem				( const CEGUI::EventArgs& args	);
	bool						handleRole				( const CEGUI::EventArgs& args	);
	
	bool						handleDisplayNetInfo	( const CEGUI::EventArgs& args	);
	bool						handleCloseNetInfo		( const CEGUI::EventArgs& args	);
	void						DisplayConnectionState	( int nPing						);
	bool						IsNewAbility();
	void						ShowAimationOfSkillBtn( bool show = false );
	bool						handleAimationDown			( const CEGUI::EventArgs& args	);
private:
	//static unsigned int			FriendMgrID;
	KUiPlayerBaseInfo			m_BaseInfo;
	KUiPlayerRuntimeInfo		m_RuntimeInfo;
	KUiPlayerAttribute			m_RuntimeAttribute;
	CEGUI::Window*				pImageFastNetSpeed;
	CEGUI::Window*				pImageNormalNetSpeed;
	CEGUI::Window*				pImageSlowNetSpeed;
	TLButton*					m_skillButton;
	TLStaticImage*				m_skillButtonAnimation;
	char						m_szNetInfo[MAX_TEXT_LEN];
	DWORD						m_dwPing;
	DWORD						m_currentSec;
	int							PlayTime;
	int							m_skillKindArray[MAX_SKILL_COUNT];
	int							m_skillArray[MAX_SKILL_COUNT];
	bool						m_isOpenAnimation;
	int							m_uncheckItem;
};


class KUiNaviationEx : public KUiWndSingleton<KUiNaviationEx>
{
	enum MINITOOLBAR_STATUS_MESSAGE_EX
	{
		NO_TONG_MESSAGE = 100,
	};
public:
	KUiNaviationEx(  const CEGUI::String& id_name );
	~KUiNaviationEx(								);
	static void					Show					( void							);
	void						Init					( void							);

	void						ActiveButton			( int index						);
private:
	bool						handleQuest				( const CEGUI::EventArgs& args	);
	bool						handleTong				( const CEGUI::EventArgs& args	);
	bool						handleFriend				( const CEGUI::EventArgs& args	);
	bool						handleOption			( const CEGUI::EventArgs& args	);

	void
	handleNoTongOper();

	int
	getTongRequireLevel();

	int
	getClientPlayerLevel();
	//bool						handleSearchHelp		( const CEGUI::EventArgs& args	);

private:
	static unsigned int			FriendMgrID;
	bool						m_bTongStatus;
	std::string					m_sNoTongMessage;
	int							m_nTongRequireLevel;
};

#endif
