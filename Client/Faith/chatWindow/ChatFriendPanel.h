#ifndef _CHAT_FIREND_PANEL_H
#define _CHAT_FRIEND_PANEL_H
#define _CHAT_PANEL_FRIEND_BUTTON_ID 500
#define _CHAT_PANEL_ENEMY_BUTTON_ID  501
#define _CHAT_PANEL_PINGBI_BUTTON_ID 502
#define _CHAT_PANEL_FRIEND_BUTTON_NUM 3

#define _CHAT_SRC_PATH              "ui/imagesets/image/"
#define _CHAT_SRC_X                 "x"
#define _CHAT_SRC_Y                 "y"
#define _CHAT_SRC_WIDTH             "width"
#define _CHAT_SRC_HEIGHT            "height"
#define _CHAT_SRC_CLIP              "isClip"
#define _CHAT_SRC_CLIP_COLOR_R      "clipColorR"
#define _CHAT_SRC_CLIP_COLOR_G      "clipColorG"
#define _CHAT_SRC_CLIP_COLOR_B      "clipColorB"
#define _CHAT_SRC_NORMAL             "normalSrc"
#define _CHAT_SRC_MOUSEOVER          "mouseOverSrc"
#define _CHAT_SRC_MOUSEDOWN          "mouseDownSrc"
#define _FRIEND_CONTROL_INI_NAME   "friendControl"
#define _FRIEND_CONTROL_INI_BK     "BKSrc"
#define _FRIEND_CONTROL_INI_FRIEND "friendControl-friend"
#define _FRIEND_CONTROL_INI_ENEMY  "friendControl-enemy"
#define _FRIEND_CONTROL_INI_PINGBI "friendControl-pingbi"
#define _FRIEND_CONTROL_IDX_FRIEND  0
#define _FIREND_CONTROL_IDX_ENEMY   1
#define _FRIEND_CONTROL_IDX_PINGBI  2
typedef class ChatFriendPanel
{
public:
	ChatFriendPanel();
	~ChatFriendPanel();
	static BOOL CALLBACK ChatFriendProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void ChatFriendProcessLClickButton(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void ChatFriendProcessMouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void ChatFriendProcessMouseLeave(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void ChatFriendLoadSource(HWND hwnd);
	void ChatFriendDrawButton(LPDRAWITEMSTRUCT lpdis);
	void ChatFriendCreate(HWND hParent);
	void ChatFriendAdjustWindow();
	void ChatFirendShow(BOOL isShow);
	HWND CHatFriendGetHandle() {return hFirendDlg;}
	PlayerInfoDlg* ChatFriendGetCurrentDlg() {return pCurrentDlg;}
	void      ChatFriendSetCurrentDlg(PlayerInfoDlg* pDlg){pCurrentDlg = pDlg;}
	ChatButton& ChatFriendGetButtonByIdx(int idx){return buttonList[idx];}
protected:
	HWND hFirendDlg;
	ChatButton buttonList[_CHAT_PANEL_FRIEND_BUTTON_NUM];
	int bkSrcIdx;
	int x ,y;
	int width,height;
	PlayerInfoDlg* pCurrentDlg;
public:
	int currentIdx;
}FRIENDPANEL,*LPFRIENDPANEL;
#endif