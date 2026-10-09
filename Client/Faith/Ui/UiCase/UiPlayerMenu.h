//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2/21/2007 14:32
//      File_base        : UiPlayerMenu
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 把小雨的玩家菜单功能从targetface中拆出来
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef PLAYER_MENU_H
#define PLAYER_MENU_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "GlobalDef.h"

using namespace CEGUI;

enum PlayerMenuButtonName
{
	PlayerMenuPrivateChat = 0,
	PlayerMenuTrade,
	PlayerMenuFollow,
	PlayerMenuView,
	PlayerMenuAddFriend,
	PlayerMenuInvite,
	PlayerMenuApplicateTeam,
	PlayerMenuScreen,
	PlayerMenuReport,
	PlayerMenuShizu,
	PlayerMenuZhuhou,
	PlayerMenuJinYan,
	PlayerMenuKick,
	PlayerMenuDongjie,
	PlayerMenuDongjieAccount,
	PlayerMenuChuansong,
	PlayerMenuIP,
	PlayerMenuButtonCount
};

class KUiPlayerMenu : public KUiWndSingleton<KUiPlayerMenu>
{
public:
	KUiPlayerMenu(String wndType);
	~KUiPlayerMenu();

	static void ChuanSongByName();
	static void DongjiePlayerByName();
	static void JinyanPlayerByName();
	void Init();
	void show(char* name, int npcId, bool bPrivateState  = false);
	void show(char* name, int npcId, int teamID, bool bPrivateState = false); //bPrivateState 表示为蒙面模式
	void ShowByClickText(char * name, int npcId, bool bPrivateState = false);
	void setPos(Point pos);
	void SetReportText(char * text, size_t textLength);
	Rect getArea();
	int	 GetWinHeight();

protected:

	bool onChatClick( const EventArgs& args );
	bool onGroupInviteClick( const EventArgs& args );
	bool onTradeClick( const EventArgs& args );
	bool onViewClick( const EventArgs& args );
	bool onDetailClick( const EventArgs& args );
	bool onFollowClick( const EventArgs& args );
	bool onScreenClick( const EventArgs& args );
	bool onShizuInviteClick(const EventArgs& args);
	bool onZhuhouInviteClick(const EventArgs& args);
	bool onAddFriend(const EventArgs& args);
	bool onAppTeam(const EventArgs& args);
	bool onReport(const EventArgs& args);

	bool onKick(const EventArgs& args);
	bool onJinyan(const EventArgs& args);
	bool onDongjie(const EventArgs& args);
	bool onDongjieAccount(const EventArgs& args);
	bool onChuanSong(const EventArgs& args);
	bool onIp(const EventArgs& args);

	void RefreshPos(bool isDetail);
private:
	void getChild();

private:
	PushButton* d_chatBtn;
	PushButton* d_groupInviteBtn;
	PushButton* d_viewBtn;
	PushButton* d_tradeBtn;
	PushButton* d_detailBtn;
	PushButton* d_followBtn;
	PushButton* d_screenBtn;
	PushButton* d_pInviteShizu;
	PushButton* d_pInviteZhuhou;
	PushButton* d_addFriend;
	PushButton* d_appTeam;
	PushButton* d_report;

	PushButton* d_pKick;
	PushButton* d_pJinyan;
	PushButton* d_pDongjie;
	PushButton* d_pDongjieAccount;
	PushButton* d_pChuanSong;
	PushButton* d_pIp;

	PushButton * m_ButtonList[PlayerMenuButtonCount];

	char		d_reportText[COMMON_CLIENT_MSG_LEN_256];
	char		d_playerName[MAXSIZE_ROLENAME];
	int			d_playerNpcId;
	bool        d_PrivateState;
	int			m_iTargetTeamID;
};

#endif 
