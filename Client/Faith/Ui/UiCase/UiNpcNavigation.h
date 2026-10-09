#ifndef UI_NPC_NAVIGATION_H
#define UI_NPC_NAVIGATION_H

#define UI_NPC_NAVIGATION_WINDOW_PATH_1024 "uisettings/layouts1024/NpcNavigation.ls"
#define UI_NPC_NAVIGATION_WINDOW_PATH "uisettings/layouts/NpcNavigation.ls"
#define UI_NPC_NAVIGATION_INVALID_NPC_ID -1
#define UI_NPC_NAVIGATION_MAX_NPC_COUNT 300 
#define UI_NPC_NAVIGATION_TIP_TEXT_ID 310
#define UI_NAVIGATION_MAX_PLAYER_COUNT 200
#define MAX_TEAMMEMBER_COUNT 5

#include "CEGUI.h"
#include "TLStatic.h"
#include "TLVertScrollbar.h"
#include "TLTree.h"
#include "TLButton.h"
#include "GameDataDef.h"
#include "Ui/UiCase/UiChatWindow.h"

using namespace std;

class KUiNpcNavigation
{
	TLStaticImage*		_thisWindow;
	TLButton *			m_NpcButton;
	TLButton *			m_PlayerButton;
	//npc查询面板
	TLStaticImage*		_npcPanel;
	TLStaticImage*		_npcPanel_Clipper;
	TLTree*				_npcPanel_NameList;
	TLTree*				_npcPanel_PosList;
	TLVertScrollbar*	_npcPanel_Scroll;
	TLButton*			_npcPanel_Goto;

	vector<NpcMapInfo>	_npcInfo;
	TLTreeItem*			_npcItem[UI_NPC_NAVIGATION_MAX_NPC_COUNT];

	int					_npcId;

	//附近玩家查询面板
	TLStaticImage *		m_PlayerPanel;
	TLStaticImage *		m_PlayerClipper;
	TLTree *			m_PlayerList;
	TLVertScrollbar *	m_PlayerScroll;
	TLButton *			m_AddFriend;
	TLButton *			m_PrivateChat;
	TLButton *			m_Team;
	TLButton *			m_ViewItem;
	TLButton *			m_Follow;
	TLButton *			m_MoveToPlayer;

	TLTreeItem *		m_PlayerItem[UI_NAVIGATION_MAX_PLAYER_COUNT];
	char				m_SelectedPlayerName[COMMON_CLIENT_MSG_LEN_32];
	int					m_SelectedPlayerID;

	bool				m_isUsed;

private:
	void	load();
	void	loadNpcInfo();
	void	layoutNpcPanel();
	void	GetPlayerList();
	void	LayoutPlayerPanel();
	
	bool	gotoSelNpc();
	void	sortByName();
	bool	OnNpcButton(const EventArgs & args);
	bool	OnPlayerButton(const EventArgs & args);

	//npc面板
	bool	onGoto(const EventArgs& args);
	bool	selectNpc(const EventArgs& args);
	bool	selectNpcAndGo(const EventArgs& args);
	bool	onListWheelChanged(const EventArgs& args);
	bool	onScroll(const EventArgs& args);
	bool	onClose(const EventArgs& args);
	bool	onMHover(const EventArgs& args);
	bool	onMLeave(const EventArgs& args);
	//附近玩家面板
	bool	OnAddFriend(const EventArgs & args);
	bool	OnPrivateChat(const EventArgs & args);
	bool	OnTeam(const EventArgs & args);
	bool	OnViewItem(const EventArgs & args);
	bool	OnFollow(const EventArgs & args);
	bool	OnMoveToPlayer(const EventArgs &args);
	bool	OnListWhell(const EventArgs & args);
	bool	SelectPlayer(const EventArgs & args);
	bool	SelectPlayerAndGo(const EventArgs & args);
	bool	OnPlayerScroll(const EventArgs & args);
	bool	OnPlayerMHover(const EventArgs & args);
	bool	OnPlayerMLeave(const EventArgs & args);
public:
	KUiNpcNavigation();
	~KUiNpcNavigation();

	static KUiNpcNavigation& getSingleton();

	bool	isVisible();
	void	show();
	void	hide();
	void	toggle();

	static	char FirstLetter(string text);
};

#endif