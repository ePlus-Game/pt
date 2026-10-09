#ifndef _CHAT_CONTROL_PANEL_H
#define _CHAT_CONTROL_PANEL_H

#define _CHAT_SYNTHES_PANEL_BUTTON_ID 400
#define _CHAT_FRIEND_PANEL_BUTTON_ID  401
#define _CHAT_TRUST_PANEL_BUTTON_ID   402
#define _CHAT_CLAN_PANEL_BUTTON_ID    403

#define _CHAT_PANEL_BUTTON_NUMBER    5
#define _CHAT_PANEL_CHAT_PANEL       0
#define _CHAT_PANEL_FRIEND_PANEL     1
#define _CHAT_PANEL_TRUST_PANEL      2
#define _CHAT_PANEL_CLAN_PANEL       3
#define _CHAT_PANEL_LUED_PANEL       4

#define _PANEL_CONTROL_INI_NAME  "controlPage"
#define _PANEL_CONTROL_INI_BK    "BKSrc"
#define _PANEL_CONTROL_INI_CHAT_PAGE "controlPage-chat"
#define _PANEL_CONTROL_INI_FRIEND_PAGE "controlPage-friend"
#define _PANEL_CONTROL_INI_TRUST_PAGE  "controlPage-autoAttack"
#define _PANEL_CONTROL_INI_CLAN_PAGE  "controlPage-clan"
#define _PANEL_CONTROL_INI_LUED_PAGE  "controlPage-lued"

typedef class ChatControlPanel
{
public:
	ChatControlPanel();
	~ChatControlPanel();
	void ChatPanelInit(HWND hwnd);
	BOOL ChatPanelCreate(HWND hwnd);
	void ChatPanelAdjustWindow();
	void ChatPanelDrawButton(LPDRAWITEMSTRUCT lpdis);
	void ChatPanelShow(BOOL show);
	void ChatPanelShowChatInfoPanel();
	void ChatPanelShowFriendInfoPanel();
	void ChatPanelShowEntrustPanel();
	void ChatPanelShowClanPanel();
	void ChatPanelShowLuedPanel();
	static BOOL CALLBACK ChatPanelProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static LRESULT CALLBACK ChatPanelButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static ChatControlPanel& ChatPanelGetPanel();
public:
	vector<ChatButton*> panelButtonList;
	WNDPROC   buttonProc[_CHAT_PANEL_BUTTON_NUMBER];
	HWND   hPanelDlg;
	int x;
	int y;
	int width;
	int height;
	int bkBitmapIdx;
	int currentPanel;
}CONTROLPANEL,*LPCONTROLPANEL;
#endif