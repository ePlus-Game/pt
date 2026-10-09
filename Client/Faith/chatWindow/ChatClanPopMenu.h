#ifndef _CHATCLANPOPMENU_H_
#define _CHATCLANPOPMENU_H_
#endif

#define _CLAN_POP_ADD_FRIEND_BNT_ID         886
#define _CLAN_POP_INVITE_TEAM_BNT_ID        887
#define _CLAN_POP_PRIVATE_CHAT_BNT_ID       888
#define _CLAN_POP_PARTICULAR_INFO_BNT_ID    889
#define _CLAN_POP_DEMISE_BNT_ID             890
#define _CLAN_POP_FIRE_BNT_ID               891

class ChatClanPopMenu
{
public:
	ChatClanPopMenu();
	~ChatClanPopMenu();
public:
	ChatButton & GetAddFriendButton() { return m_AddFriend; };
	ChatButton & GetInviteTeamButton() { return m_InviteTeam; };
	ChatButton & GetPrivateChatButton() { return m_PrivateChat; };
	ChatButton & GetParticulInfoButton() { return m_ParticularInfo; };
	ChatButton & GetDemiseButton() { return m_Demise; };
	ChatButton & GetFireButton() {return m_Fire; };
	HWND GetDlgItemHandle() { return m_hDlg;};
	BOOL CreateMenu(HWND hParent, DLGPROC processFun);
public:
	void LoadIniSrc(HWND hParent);
	void AddFrienButtonDown();
	void InviteTeamButtonDown();
	void PrivateChatButtonDown();
	void ParticularInfoButtonDown();
	void DemiseButtonDown();
	void FireButtonDown();
	void ProcessKillFocus();
	void AdjustWindow();
	void ShowMenu();
	void HideMenu();
	void SetClanListControl(ChatClanListControl * pControl) { pPlayerControl = pControl; }
	static ChatClanPopMenu & GetMenu();
public:
	ChatButton m_AddFriend;
	ChatButton m_InviteTeam;
	ChatButton m_PrivateChat;
	ChatButton m_ParticularInfo;
	ChatButton m_Demise;
	ChatButton m_Fire;
	HWND m_hDlg;
	HWND m_hParent;
	int m_iMenuWidth;
	ChatClanListControl * pPlayerControl;
};

#define _LUED_POP_ADD_FRIEND_BNT_ID         892
#define _LUED_POP_INVITE_TEAM_BNT_ID        893
#define _LUED_POP_PRIVATE_CHAT_BNT_ID       894
#define _LUED_POP_PATRICULAR_INFO_BNT_ID    895
#define _LUED_POP_DEMISE_BNT_ID             896
#define _LUED_POP_FORBID_CHAT_BNT_ID        897
#define _LUED_POP_UNFORBID_CHAT_BNT_ID      898

class ChatLuedPopMenu
{
public:
	ChatLuedPopMenu();
	~ChatLuedPopMenu();
public:
	ChatButton & GetAddFriendButton() { return m_AddFriend; };
	ChatButton & GetInviteTeamButton() { return m_InviteTeam; };
	ChatButton & GetPrivateChatButton() { return m_PrivateChat; };
	ChatButton & GetParticulInfoButton() { return m_ParticularInfo; };
	ChatButton & GetDemiseButton() { return m_Demise; };
	ChatButton & GetForbidChatButton() { return m_ForbidChat;};
	ChatButton & GetUnforbidChatButton() {return m_UnforbidChat;};
	HWND GetDlgItemHandle() { return m_hDlg;};
	BOOL CreateMenu(HWND hParent, DLGPROC processFun);
public:
	void LoadIniSrc(HWND hParent);
	void AddFrienButtonDown();
	void InviteTeamButtonDown();
	void PrivateChatButtonDown();
	void ParticularInfoButtonDown();
	void DemiseButtonDown();
	void ForbidChatButtonDown();
	void UnforbidChatButtonDown();
	void ProcessKillFocus();
	void AdjustWindow();
	void ShowMenu();
	void HideMenu();
	void SetClanListControl(ChatClanListControl * pControl) { pPlayerControl = pControl; }
	static ChatLuedPopMenu & GetMenu();
public:
	ChatButton m_AddFriend;
	ChatButton m_InviteTeam;
	ChatButton m_PrivateChat;
	ChatButton m_ParticularInfo;
	ChatButton m_Demise;
	ChatButton m_ForbidChat;
	ChatButton m_UnforbidChat;
	HWND m_hDlg;
	HWND m_hParent;
	int m_iMenuWidth;
	ChatClanListControl * pPlayerControl;
};