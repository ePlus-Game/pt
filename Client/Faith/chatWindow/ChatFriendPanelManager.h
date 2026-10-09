#ifndef _CHAT_FRIEND_PANEL_MANAGER_H
#define _CHAT_FRIEND_PANEL_MANAGER_H
#define _CHAT_PLAYER_INFO_FRIEND     0
#define _CHAT_PLAYER_INFO_ENEMY      1
#define _CHAT_PLAYER_INFO_PINGBI     2
#define _CHAT_PLAYER_INFO_ROOM       3
#define _CHAT_PLAYER_INFO_TEMP       4
#define _PLAYER_ITEM_CONTROL_FONT_SIZE 256

#define _PLAYER_ITEM_CONTROL_INI_NAME_WIDTH   "nameItemWidth"
#define _PLAYER_ITEM_CONTROL_INI_METIER_WIDTH "metierItemWidth"
#define _PLAYER_ITEM_CONTROL_INI_LEVEL_WIDTH  "levelItemWidth"
#define _PLAYER_ITEM_CONTROL_INI_GROUP_WIDTH  "groupItemWidth"
#define _PLAYER_ITEM_CONTROL_INI_PLACE_WIDTH  "placeItemWidth"
typedef class ChatFriendPanelManager
{
public:
	ChatFriendPanelManager();
	~ChatFriendPanelManager();
	static ChatFriendPanelManager& ChatFriendManagerGet();
	void ChatFriendManagerShowInfoDlg();
	void ChatFriendManagerHideInfoDlg();
	void ChatFriendManagerInit(HWND hwnd);
	void ChatFriendManagerAdd(PLAYERCONTROL*& pControls);
	void ChatFriendManagerAdd(ChatPlayerInfo& playerInfo);
	void ChatFriendManagerReceiveFriendList();
	void ChatFriendManagerClearControl();
	void ChatFriendManegerGetPlayerInfo(PlayerInfo& playerGameInfo);
	void ChatFriendManagerDestroyWindow();
	void ChatFriendManagerShowFriendPage(bool show);
public:
	ChatFriendPanel firendPanel;
	int controlWidth;
	int controlHeight ;
	LPPLAYERCONTROL ppControls[_CHAT_MAX_FRIENDS];
	int numberControls;
	char pszFont[_PLAYER_ITEM_CONTROL_FONT_SIZE];
	bool isUseDefaultFont;
	COLORREF onlineColor;
	COLORREF onlineMouseOverColor;
	COLORREF onlineSelectColor;
	COLORREF leftlineSelectColor;
	COLORREF leftlineColor;
	int nameItemWidth ;
	int metierItemWidth ;
	int levelItemWidth ;
	PlayerInfoDlg   playerInfoDlg[_PLAYER_INFO_PAGE_NUM];
	HINTDLGDELETE  deleteDlg;
	HINTDLGADD     addDlg;
	bool           chatWndListUpdata;
	//好友列表更新标志//
	bool needUpdate;
}FRIENDMANAGER,*LPFRIENDMANAGER;
#endif