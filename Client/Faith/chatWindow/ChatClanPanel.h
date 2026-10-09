#ifndef _CHATCLANPANEL_H_
#define _CHATCLANPANEL_H_
#endif

#define _CHAT_CLAN_PANEL_INI_CLAN "chat-clan-panel-clan"
#define _CHAT_CLAN_PANEL_INI_LEUD "chat-clan-panel-leud"
#define _CHAT_CLAN_PANEL_INI_NATION "chat-clan-panel-nation"

#define _CLAN_INFO_DLG_CLAN 0
#define _CLAN_INFO_DLG_LUED 1
#define _CLAN_INFO_DLG_NATION 2

class ChatClanInfoDlg;
class ChatLuedInfoDlg;

class ChatClanPanel : public ChatFriendPanel
{
public:
	ChatClanPanel();
	~ChatClanPanel();
	void LoadSource(HWND hwnd);
	void CreatePanel(HWND hParent);
	ChatButton & GetButtonByIdx(int index) { return buttonList[index]; };
	static BOOL CALLBACK ChatClanProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
};

class ChatLuedPanel : public ChatFriendPanel
{
public:
	ChatLuedPanel();
	~ChatLuedPanel();
	void LoadSource(HWND hwnd);
	void CreatePanel(HWND hParent);
	ChatButton & GetButtonByIdx(int index) { return buttonList[index]; };
	static BOOL CALLBACK ChatLuedProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
};