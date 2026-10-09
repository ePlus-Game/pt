#ifndef _CHATCLANINFODLG_H_
#define _CHATCLANINFODLG_H_
#endif

#define _CHAT_CLAN_INFO_DLG_BUTTON_NUM        3
#define _CHAT_CLAN_INFO_DLG_MODIFY_BULLETIN   0
#define _CHAT_CLAN_INFO_DLG_ADD_MEMBER        1
#define _CHAT_CLAN_INFO_DLG_DELETE_MEMBER     2

#define _CHAT_CLAN_INFO_DLG_INI_MODIFY_BULLETIN   "clan_info_dlg_modify_bulletin"
#define _CHAT_CLAN_INFO_DLG_INI_ADD_MEMBER        "clan_info_dlg_add_member"
#define _CHAT_CLAN_INFO_DLG_INI_DELETE_MEMBER     "clan_info_dlg_delete_memeber"

#define _CHAT_CLAN_INFO_DLG_INI_MODIFY_BULLETIN_ID   2000
#define _CHAT_CLAN_INFO_DLG_INI_ADD_MEMBER_ID        2001
#define _CHAT_CLAN_INFO_DLG_INI_DELETE_MEMBER_ID     2002

#define _CLAN_MAX_MEMBER_NUM 10

class ClanInfoDlg : public PlayerInfoDlg
{
public:
	virtual void CreateClanInfoDlg(HWND hParent,DlgProcessFun pFun) = 0;
	virtual void ClearAll() = 0;
	virtual void UpdateMemberList(ChatClanListControl * listControl) = 0;
	virtual void LoadSRC(HWND hwnd) = 0;
	virtual void PaintDLG(HDC hdc) = 0;
	virtual void AddMember(const char * name, HWND callWnd) = 0;
	virtual void DeleteMember(HWND callWnd) = 0;
	virtual void ClearSelected(BOOL update) = 0;
	virtual void OnLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnMouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnLButtonUp(HWND hwnd, WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnLButtonDBLCLK(HWND hwnd, WPARAM wParam, LPARAM lParam) = 0;
	virtual ChatClanListControl * GetControlByPoint(POINT& pt) = 0;
public:
	ChatClanTitleControl m_titleControl;
	ChatClanListControl * m_OnlineMemberList[_CLAN_MAX_MEMBER_NUM];
	ChatClanListControl * m_LeftMemberList[_CLAN_MAX_MEMBER_NUM];
	ChatButton m_ButtenList[_CHAT_CLAN_INFO_DLG_BUTTON_NUM];
	int m_iOnlineMemberNum;
	int m_iLeftMemberNum;
};

class ChatClanInfoDlg : public ClanInfoDlg
{
public:
	ChatClanInfoDlg();
	~ChatClanInfoDlg();
	void CreateClanInfoDlg(HWND hParent,DlgProcessFun pFun);
	void ClearAll();
	void UpdateMemberList(ChatClanListControl * listControl);
	void LoadSRC(HWND hwnd);
	void PaintDLG(HDC hdc);
	void AddMember(const char * name, HWND callWnd);
	void DeleteMember(HWND callWnd);
	void ClearSelected(BOOL update);
	void OnLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void OnMouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void OnLButtonDBLCLK(HWND hwnd, WPARAM wParam, LPARAM lParam);
	ChatClanListControl * GetControlByPoint(POINT& pt);
};

#define _CHAT_LUED_INFO_DLG_INI_ADD_CLAN          "clan_info_dlg_add_clan"
#define _CHAT_LUED_INFO_DLG_INI_DELETE_CLAN       "clan_info_dlg_delete_clan"

#define _CHAT_LUED_INFO_DLG_INI_MODIFY_BULLETIN_ID 2003
#define _CHAT_LUED_INFO_DLG_INI_ADD_CLAN_ID        2004
#define _CHAT_LUED_INFO_DLG_INI_DELETE_CLAN_ID     2005

#define _CHAT_LUED_INFO_DLG_MODIFY_BULLETIN  0
#define _CHAT_LUED_INFO_DLG_ADD_CLAN         1
#define _CHAT_LUED_INFO_DLG_DELETE_CLAN      2


class ChatLuedInfoDlg : public ChatClanInfoDlg
{
public:
	ChatLuedInfoDlg();
	~ChatLuedInfoDlg();
	void CreateLuedInfoDlg(HWND hParent,DlgProcessFun pFun);
	void ClearAll();
	void UpdateMemberList(ChatClanListControl * listControl);
	void LoadSRC(HWND hwnd);
	void PaintDLG(HDC hdc);
	void AddMember(const char * name, HWND callWnd);
	void DeleteMember(HWND callWnd);
	void ClearSelected(BOOL update);
	void OnLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void OnMouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void OnLButtonUp(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void OnLButtonDBLCLK(HWND hwnd, WPARAM wParam, LPARAM lParam);
	ChatClanListControl * GetControlByPoint(POINT& pt);
};
