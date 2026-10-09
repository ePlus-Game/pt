#ifndef _CHATCOMBOBOX_H_
#define _CHATCOMBOBOX_H_
#endif

#define _CLAN_DOWN_BNT_ID 1068
#define _CLAN_SELECTED_BNT_ID 1069
#define _MAX_CLAN_NUM 20
#define _MAX_CLAN_NAME 16
#define _MAX_FONT_SIZE 256

#define WM_DATA_REQUEST_SUCCEED (WM_USER + 10)
#define WM_CLAN_DATA_REQUEST_SUCCEED (WM_USER + 11)

typedef struct ChatClanInfo
{
	char m_ClanName[_MAX_CLAN_NAME];
	FSGUID m_ClanGuid;
	RECT m_posRect;
	BOOL m_blIsSelected;
//	BOOL m_blIsMouseHover;
	ChatClanInfo()
	{
		memset(m_ClanName, 0, _MAX_CLAN_NAME);
		memset(&m_posRect, 0, sizeof(RECT));
		m_blIsSelected = FALSE;
//		m_blIsMouseHover = FALSE;
	}
	int GetWidth()
	{
		return m_posRect.right - m_posRect.left;
	}
	int GetHeight()
	{
		return m_posRect.bottom - m_posRect.top;
	}
	void SetRect(int x, int y, int width, int height)
	{
		m_posRect.left = x;
		m_posRect.top = y;
		m_posRect.right = x + width;
		m_posRect.bottom = y + height;
	}
}CHATCLANINF, *LPCHATCLANINFO;

class ChatClanComboBox
{
public:
	ChatClanComboBox();
	~ChatClanComboBox();
	void LoadIniFile(HWND hParent);
	void AdjustDownDlg();
	void ShowDownDialog(bool show);
	void DrawItem(HDC hdc);
	void CLKOnDownDlg(POINT & point);
	void UpdateComboBox();
	void ClearAll();
	void UpdateDataList();
	LPCHATCLANINFO GetSelectedItem();
	static BOOL CALLBACK DownDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK DownDlgButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK SelectedButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static ChatClanComboBox & GetClanComboBox();
public:
	HWND m_hDownDialg;
	HWND m_hParent;
	BOOL m_isShowDownDlg;
	BOOL m_isUseDefaultFont;
	COLORREF m_fontColor;
	COLORREF m_mouseOverColor;
	ChatButton m_SelectedBnt;
	ChatButton m_DownBnt;
	ONRECT m_posRect;
	int m_iDlgSourceIdx;
	int m_iDlgSourceWidth;
	int m_iDlgSourceHeigth;
	int m_iInterval;
	int m_iInfoHeight;
	int m_iDrawPosX;
	int m_iDrawPosY;
	int m_iClanNum;
	char m_font[_MAX_FONT_SIZE];
	ChatScrollBar m_ScrollBar;
	LPCHATCLANINFO m_ClanList[_MAX_CLAN_NUM];
protected:

};