#ifndef ENTRUST_COMPUTER_HEAD
#define ENTRUST_COMPUTER_HEAD


#define _ENTRUST_TITLE_BUTTON_ID   1678
#define _ENTRUST_OK_BUTTON_ID      1679

#define _ENTRUST_AUTOATTACK_MAIN_BUTTON_ID 1770
#define _ENTRUST_AUTOATTACK_ENEMY_LEVEL_HIGHER_BUTTON_ID 1771
#define _ENTRUST_AUTOATTACK_ENEMY_LEVEL_LOWER_BUTTON_ID 1772
#define _ENTRUST_AUTOATTACK_GO_HOME_BUTTON_ID   1773
#define _ENTRUST_AUTOATTACK_ONLY_NORMAL_SET_BUTTON_ID 1774

#define _ENTRUST_AUTOPICKUP_MAIN_BUTTON_ID  2770
#define _ENTRUST_AUTOPICKUP_DEAR_FIRST_BUTTON_ID 2771
#define _ENTRUST_AUTOPICKUP_OVER_BLUE_BUTTON_ID  2772
#define _ENTRUST_AUTOPICKUP_OVER_GREEN_BUTTON_ID 2773
#define _ENTRUST_AUTOPICKUP_MEDICINE_BUTTON_ID 2774

#define _ENTRUST_AUTOUSEITEM_MAIN_BUTTON_ID       3770
#define _ENTRUST_AUTOUSEITEM_HP_60_BUTTON_ID      3771
#define _ENTRUST_AUTOUSEITEM_HP_40_BUTTON_ID      3772
#define _ENTRUST_AUTOUSEITEM_OWNER_BUTTON_ID      3773
#define _ENTRUST_AUTOUSEITEM_MP_20_BUTTON_ID      3774 

#define _ENTRUST_HIDE_SHOW_WND_BNT_ID 3775
#define _ENTRUST_CHANGE_WND_BNT_ID 3776
#define _ENTRUST_HIDE_SHOW_WND_BNT_TEXT "entrust_hide_show_button"
#define _ENTRUST_CHANGE_WND_BNT_TEXT "entrust_change_wnd_button"

#define _ENTRUST_AUTOPICKUP_CHILD_BUTTONS_NUMBERS	4
#define _ENTRUST_AUTOATTACK_CHILD_BUTTONS_NUMBERS	4
#define _ENTRUST_AUTOUSEITEM_CHILD_BUTTONS_NUMBERS	4

#define _ENTRUST_AUTOATTACK_ENEMY_LEVEL_HIGHER_BUTTON_INDEX 0
#define _ENTRUST_AUTOATTACK_ENEMY_LEVEL_LOWER_BUTTON_INDEX 1
#define _ENTRUST_AUTOATTACK_GO_HOME_BUTTON_INDEX 2
#define _ENTRUST_AUTOATTACK_ONLY_NORMAL_SET_BUTTON_INDEX 3

#define _ENTRUST_AUTOPICKUP_DEAR_FIRST_BUTTON_INDEX		0
#define _ENTRUST_AUTOPICKUP_OVER_BLUE_BUTTON_INDEX		1
#define _ENTRUST_AUTOPICKUP_OVER_GREEN_BUTTON_INDEX		2
#define _ENTRUST_AUTOPICKUP_MEDICINE_BUTTON_INDEX		3

#define _ENTRUST_AUTOUSEITEM_HP_60_BUTTON_INDEX   0
#define _ENTRUST_AUTOUSEITEM_HP_40_BUTTON_INDEX   1
#define _ENTRUST_AUTOUSEITEM_OWNER_BUTTON_INDEX   2
#define _ENTRUST_AUTOUSEITEM_MP_40_BUTTON_INDEX   3


#define _ENTRUST_PAGE_NAME_INI "entrustPage"

struct MutexButtonsIndex
{
	MutexButtonsIndex()
	{
		numberButtons = 0;
	}
	void MutexIndexAddIndex(int index)
	{
		if(index < 0)
			return;
		int size = buttonIndexList.size();
		for(int i = 0; i < size ;i++)
		{
			if(buttonIndexList[i] == index)
				return;
		}
		buttonIndexList.push_back(index);
		numberButtons++;
	}
	int numberButtons;
	vector<int> buttonIndexList;
};
typedef class CheckedButtonGroup
{
public:
	CheckedButtonGroup();
	~CheckedButtonGroup();
public:
	bool CheckedButtonDrawItem(LPDRAWITEMSTRUCT lpdis);
	bool CheckedButtonProcessLButtonDown(WPARAM wParam ,LPARAM lParam);
	void CheckedButtonUpdate();
	void CheckedButtonDrawTextBk(HDC hParentDC);
	void CheckedButtonSetLastSelected();
	void CheckedButtonSetAutoAttack(bool enable);

//	bool CheckedButtonProcessMouseMove(WPARAM wParam,LPARAM lParam);
//	bool CheckedButtonProcessMouseLeave(WPARAM wParam,LPARAM lParam);
public:
public:
	ChatButton               mainButton;
	bool                     mainButtonEnable;
	vector<ChatButton*>      childButtonList;
	bool                     lastMainButtonEnable;
	vector<bool>             lastChildButtonEnabel;
	vector<bool>              childButtonEnabel;
	vector<MutexButtonsIndex> mutexIndexList;
} CHECKBUTTON,*LPCHECKBUTTON;
typedef class EntrustComputerDlg
{
public:
	EntrustComputerDlg();
	~EntrustComputerDlg();
public:
	static EntrustComputerDlg& EntrustDlgGetSingleton();
public:
	void   EntrustDlgInitFromCtfIni(HWND hParent);
	void   EntrustDlgAutoAtackIni(HWND hParent);
	void   EntrustDlgAutoPickUpIni(HWND hParent);
	void   EntrustDlgAutoUseItemIni(HWND hParent);
	void   EntrustDlgLoadSrc();
	void   EntrustDlgShowDlg(bool show);
	static BOOL CALLBACK EntrustDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void   EntrustDlgAdjustWindow();
	void   EntrustDlgDrawItem(LPDRAWITEMSTRUCT lpdis);
	void   EntrustDlgProcessMouseMove(WPARAM wParam,LPARAM lParam);
	void   EntrustDlgProcessMouseLeave(WPARAM wParam,LPARAM lParam);
	void   EntrustDlgProcessCommand(WPARAM wParam,LPARAM lParam);
	void   EntrustDlgProcessPaint(HDC hdc);
	void   EntrustDlgProcessPushOkButton(WPARAM wParam,LPARAM lParam);
protected:
	void   EntrustDlgProcessAutoAttack();
	void   EntrustDlgProcessAutoPickUp();
	void   EntrustDlgProcessAutoUseItem();
public:
	int   dlgBkBitmapIdx;
	HWND      hDlg;
	int       dlgPosX;
	int       dlgPosY;
	int       dlgWidth;
	int       dlgHeight;
	ChatButton titleButton;
	ChatButton okButton;
	ChatButton m_wndHideShowBnt;
	ChatButton m_wndChangeWndBnt;
	CheckedButtonGroup autoAttackGroup;
	CheckedButtonGroup autoPickUpGroup;
	CheckedButtonGroup autoUseItemGroup;
}ENTRUSTCPUDLG,*LPENTRUSTCPUDLG;
#endif
