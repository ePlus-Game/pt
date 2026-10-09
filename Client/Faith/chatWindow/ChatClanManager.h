#ifndef _CHATCLANMANAGER_H_
#define _CHATCLANMANAGER_H_
#endif

class ChatClanManager : public ChatFriendPanelManager
{
public:
	ChatClanManager();
	~ChatClanManager();
	static ChatClanManager & GetManager();
	void ChatClanManagerInit(HWND hwnd);
	void ShowMemberPage(bool isShow);
	void HideClanDlg();
	void DestroyClanPanel();
	void UpdateMemberList();
	void ClearAll();
	void AddMemberButtonDown();
	void DeleteMemberButtonDown();
public:
	ChatClanPanel m_ClanPanel;
	ChatClanInfoDlg m_ClanInfoDlg;
	int m_iOnlineMemberNum;
	int m_iAllMemberNum;
};

class ChatLuedManager : ChatFriendPanelManager
{
public:
	ChatLuedManager();
	~ChatLuedManager();
	static ChatLuedManager & GetManager();
	void ChatLuedManagerInit(HWND hwnd);
	void ShowMemberPage(bool isShow);
	void HideLuedDlg();
	void DestroyClanPanel();
	void UpdateMemberList();
	void ClearAll();
	void AddClanButtonDown();
	void DeleteClanButtonDown();
public:
	ChatLuedPanel m_ClanPanel;
	ChatLuedInfoDlg m_LuedInfoDlg;
	int m_iOnlineMemberNum;
	int m_iAllMemberNum;
	HINTDLGDELETE  m_DeleteClanDlg;
	HINTDLGADD     m_AddClanDlg;
};