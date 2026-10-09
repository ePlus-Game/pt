#ifndef _CHATCLANANNOUNCEMENTDLG_H_
#define _CHATCLANANNOUNCEMENTDLG_H_
#endif

#define _CHAT_SRC_PATH              "ui/imagesets/image/"
#define _CHAT_CFG_FILE              "UiSettings\\wndChatCfg.ini"
#define _CHAT_CFG_FILE_1024         "UiSettings\\wndChatCfg1024.ini"

#define _DLG_RESOURCE_INI                    "announcementDlg"
#define _DLG_RESOURCE_INI_WIDTH_TXT          "width"
#define _DLG_RESOURCE_INI_HEIGHT_TXT         "height"
#define _DLG_RESOURCE_INI_EDIT_HEIGHT_TXT    "editHeight"
#define _DLG_RESOURCE_INI_DLG_BKSRCID_TXT    "dlgBKSrcIdx"
#define _DLG_RESOURCE_INI_EDIT_BCSRCID_TXT   "editBCSrcIdx"

#define _DLG_OK_BNT_INI         "ancmtDlgOKBnt"
#define _DLG_CANCEL_BNT_INI     "ancmtDlgCancelBnt"

#define _DLG_OK_BNT_ID          1070
#define _DLG_CANCEL_BNT_ID      1071
#define _DLG_EDIT_ID            1072
#define _UPDATE_TIMER_ID        1073

typedef class ChatClanAnnouncementDlg
{
public:
	ChatClanAnnouncementDlg();
	~ChatClanAnnouncementDlg();
	BOOL CreateDlg(HWND hParent);
	BOOL CreateDlg(HWND hParent, DLGPROC func);
	void LoadResource(HWND hParent);
	void AdjustWindow();
	void ShowDlg(BOOL isShow);
	void DrawDlg(HDC hdc);
	void Release();
	void OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void OnMouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void OnMouseLeave(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void DrawButton(LPDRAWITEMSTRUCT lpdis);
	void ClearBrush();
	static ChatClanAnnouncementDlg & GetDlg();
	static BOOL CALLBACK AnnouncementDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	ChatButton m_okBnt;
	ChatButton m_cancelBnt;
	RECT m_DlgPos;
	HFONT m_hFont;
	HBRUSH m_hBrush;
	HWND m_hDlg;
	HWND m_hEditWnd;
	HWND m_hParent;
	BOOL m_blIsShow;
	int m_iLayerID;
	int m_iDlgWidth;
	int m_iDLgHeight;
	int m_iEditHeight;
	int m_iDlgBKSrcIdx;
	int m_iEditBKSrcIdx;
}CHATCLANANNOUNCEMENTDLG, *LPCHATCLANANNOUNCEMENTDLG;