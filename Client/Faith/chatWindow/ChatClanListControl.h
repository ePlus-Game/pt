#ifndef _CHATCLANLISTCONTROL_H_
#define _CHATCLANLISTCONTROL_H_
#endif

class PlayerInfoControl;

typedef class ChatClanListControl : public PlayerInfoControl
{
public:
	ChatClanListControl();
	~ChatClanListControl();
	void DrawItem(HWND hwnd, HDC hdc, bool isUseDefaultFont);
	void SetSelected(BOOL selected, BOOL update);
	TongPageData & GetPlayerData();
	void OnLButtonDBLCLK();
public:
	TongPageData m_playerData;
}CHATCLANLISTCONGTRL, *LPCHATCLANLISTCONTROL;