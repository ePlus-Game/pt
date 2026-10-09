#ifndef _CHAT_TIP_WND_H
#define _CHAT_TIP_WND_H
#include "UiMDLInterface.h"
#define _CHAT_WND_BUTTON_NUM 10

#define _CHAT_TIP_PERSONAL_BUTTON_ID  200
#define _CHAT_TIP_TRADE_BUTTON_ID     201
#define _CHAT_TIP_TEAM_BUTTON_ID      202
#define _CHAT_TIP_FOLLOW_BUTTON_ID    203
#define _CHAT_TIP_ARM_BUTTON_ID       204
#define _CHAT_TIP_HIDE_BUTTON_ID      205
#define _CHAT_TIP_FRIEND_BUTTON_ID    206
#define _CHAT_TIP_SIZHU_BUTTON_ID     207
#define _CHAT_TIP_ZHUHOU_BUTTON_ID    208
#define _CHAT_TIP_SHENGQING_JIARU_ID  209

#define _CHAT_TIP_SHIZU_BUTTON_INDEX	7
#define _CHAT_TIP_ZHUHOU_BUTTON_INDEX	8

typedef class ChatTipWnd
{
public:
	ChatTipWnd();
	~ChatTipWnd();
	static BOOL CALLBACK ChatTipWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void   ChatTipProcessCreate(HWND hwnd);
	void   ChatTipShow(BOOL bShow);
	BOOL   ChatTipIsShow() const {return isShow;}
	void   ChatTipProcessDrawItem(LPDRAWITEMSTRUCT lpdis);
	void   ChatTipAdjustWindow(HWND hwnd);
	static LRESULT CALLBACK  ChatTipChildButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	ChatButton* ChatTipGetButton() { return buttons;}
	void    ChatTipClickPersonalButton();
	void    ChatTipClickTradeButton();
	void    ChatTipClickTeamButton();
	void    ChatTipClickFollowButton();
	void    ChatTipClickArmButton();
	void    ChatTipClickHideButton();
	void    ChatTipClickFriendButton();
	void    ChatTipClickSizhuButton();
	void    ChatTipClickZhuhouButton();
	void    ChatTipClickJoinTeamButton();
	WNDPROC    ChatTipGetButtonPoc(int i ) {return buttonsProc[i];}
public:
	HWND hWndHandle;
	LOElemInfo playerInfo;
protected:
	BOOL isShow;
	ChatButton   buttons[_CHAT_WND_BUTTON_NUM];
	WNDPROC      buttonsProc[_CHAT_WND_BUTTON_NUM];
	IUIMDL* pUiMdl;
}CHATTIPWND,*LPCHATTIPWND;
#endif