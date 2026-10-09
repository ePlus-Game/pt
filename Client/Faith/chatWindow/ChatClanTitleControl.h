#ifndef _CHATCLANTITLECONTROL_H_
#define _CHATCLANTITLECONTROL_H_
#endif

class PlayerListTitleControl;
class ChatClanInfoDlg;

typedef class ChatClanTitleControl : public PlayerListTitleControl
{
public:
	ChatClanTitleControl();
	~ChatClanTitleControl();
	void MouseMove(ChatClanInfoDlg & infoDlg, POINT & pos);
	void OnLButtonDBLCLK(ChatClanInfoDlg & infoDlg, POINT & pos);
}CHATCLANTITLECONTROL,*LPCHATCLANTITLECONTROL;