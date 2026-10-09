#ifndef _PLAYER_INFO_DLG_H
#define _PLAYER_INFO_DLG_H
#define _PLAYER_INFO_PAGE_NUM 3

#define _PLAYER_INFO_DLG_FRIEND 0
#define _PLAYER_INFO_DLG_ENEMY  1
#define _PLAYER_INFO_DLG_PINGBI 2

#define _PLAYER_ADD_BUTTON_ID   700
#define _PLAYER_DELETE_BUTTON_ID 1105
//隐藏/显示按钮和切换按钮ID
#define _HIDE_SHOW_BNT_ID 1113
#define _CHANGE_WND_ID 1120

#define _CHAT_HINT_OK_BUTTON_ID  703
#define _CHAT_HINT_NO_BUTTON_ID  704
#define _CHAT_HINT_ADD_OK_BUTTON_ID 705
#define _CHAT_HINT_ADD_NO_BUTTON_ID 706
#define _CHAT_HINT_EDIT_ID          707
#define _CHAT_SHOW_LEFT_BUTTON_ID   708

#define _CHAT_MAX_FRIENDS           200


#define _CHAT_HINT_DELETE_DLG_INI_NAME     "deleteTipWnd"
#define _CHAT_HINT_DELETE_DLG_BK_ITEM      "BkSrc"
#define _CHAT_HINT_DELETE_DLG_OK_ITEM      "deleteTipWnd-ok"
#define _CHAT_HINT_DELETE_DLG_NO_ITEM      "deleteTipWnd-no"
#define _CHAT_HINT_DELETE_MEMBER_INI_TXT   "deleteMemberTipWnd"
#define _CHAT_HINT_DELETE_CLAN_INI_TXT     "deleteClanTipWnd"

#define _CHAT_HINT_ADD_INI_NAME    "addTipWnd"
#define _CHAT_HINT_ADD_DLG_BK_ITEM  "BkSrc"
#define _CHAT_HINT_ADD_DLG_OK_ITEM  "addTipWnd-ok"
#define _CHAT_HINT_ADD_DLG_NO_ITEM  "addTipWnd-no"
#define _CHAT_HINT_ADD_DLG_GET_ITEM "addTipWnd-in"

#define _CHAT_HINT_ADD_MEMBER_INI_TXT "addMemberTipWnd"
#define _CHAT_HINT_ADD_CLAN_INI_TXT   "addClanTipWnd"

#define _PLAYER_LIST_INI_SRC_NAME  "friendList-source"
#define _PLAYER_LIST_INI_SRC_BK    "BkSrc"
#define _PLAYER_LIST_INI_SRC_DRAW_X "rcX"
#define _PLAYER_LIST_INI_SRC_DRAW_Y "rcY"
#define _PLAYER_LIST_INI_SRC_DRAW_WIDTH "rcWidth"
#define _PLAYER_LIST_INI_SRC_DRAW_HEIGHT "rcHeight"
#define _PLAYER_LIST_INI_SRC_DRAW_INTERMISSION "intermission"
#define _PLAYER_LIST_INI_SRC_FRIEND_NAME "friendList"
#define _PLAYER_LIST_INI_SRC_FRIEND_BK   "DrawAreaBitmap"
#define _PLAYER_LIST_INI_SRC_DELETE     "friendButton-delete"
#define _PLAYER_LIST_INI_SRC_ADD        "friendButton-add"
#define _PLAYER_LIST_INI_SRC_LEFTLINE   "friendButton-showLeft"
#define _PLAYER_LIST_INI_SRC_LEFTBK "leftBKSrc"
#define _PLAYER_LIST_INI_SRC_LEFTBKSELECT  "leftBkSelectSrc"
#define _PLAYER_LIST_INI_SRC_FONT          "font"
#define _PLAYER_LIST_INI_SRC_ONLINE_FONT_R "onlinefontColorR"
#define _PLAYER_LIST_INI_SRC_ONLINE_FONT_G "onlinefontColorG"
#define _PLAYER_LIST_INI_SRC_ONLINE_FONT_B "onlinefontColorB"

//配置文件字段
#define _PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT "friendButton_Hide_Show"
#define _PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT "friendButton_Change_Wnd"

#define _PLAYER_LIST_INI_SRC_ONLINE_FONT_MOUSEOVER_R   "onlinefontMouseOverColorR"
#define _PLAYER_LIST_INI_SRC_ONLINE_FONT_MOUSEOVER_G   "onlinefontMouseOverColorG"
#define _PLAYER_LIST_INI_SRC_ONLINE_FONT_MOUSEOVER_B   "onlinefontMouseOverColorB"

#define _PLAYER_LIST_INI_SRC_ONLINE_FONT_SELECT_R      "onlinefontSelectColorR"
#define _PLAYER_LIST_INI_SRC_ONLINE_FONT_SELECT_G      "onlinefontSelectColorG"
#define _PLAYER_LIST_INI_SRC_ONLINE_FONT_SELECT_B      "onlinefontSelectColorB"


#define _PLAYER_LIST_INI_SRC_LEFT_FONT_R "leftlinefontColorR"
#define _PLAYER_LIST_INI_SRC_LEFT_FONT_G "leftlinefontColorG"
#define _PLAYER_LIST_INI_SRC_LEFT_FONT_B "leftlinefontColorB"

#define _PLAYER_LIST_INI_SRC_LEFT_FONT_SELECT_R     "leftlinefontSelectR"
#define _PLAYER_LIST_INI_SRC_LEFT_FONT_SELECT_G     "leftlinefontSelectG"
#define _PLAYER_LIST_INI_SRC_LEFT_FONT_SELECT_B     "leftlinefontSelectB"

class PlayerInfoDlg;
typedef class ChatHintDeletePlayerDlg
{
public:
	ChatHintDeletePlayerDlg();
	~ChatHintDeletePlayerDlg();
	void ChatHintDlgProcessOnButton(HWND hwnd ,WPARAM wParam,LPARAM lParam);
	void OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void ChatHintDlgProcessDrawButton(LPDRAWITEMSTRUCT lpdis);
	void ChatHintDlgProcessPaint(HDC hdc );
	void ChatHintDlgLoadSource();
	void LoadSource_Clan();
	void LoadSource_Lued();
	BOOL ChatHintDlgCreate(HWND hParent);
	BOOL ChatHintDlgCreate(HWND hParent, DLGPROC func);
	void ChatHintDlgAdjustWindow(HWND hwnd);
	static BOOL CALLBACK ChatHintDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void ChatHintDlgProcessMouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void ChatHintDlgProcessMouseLeave(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void ChatHintDlgShow(BOOL bShow);
	BOOL ChatHintDlgIsShow(){return isShow;}
	HWND hDlg;
public:
	int bkBitmapIdx;
	ChatButton okButton;
	ChatButton noButton;
	int width ,height;
	char wndText[256];
	char infoText[256];
	char font[256];
	bool isUseDefaultFont;
	int x,y;
	BOOL isShow;
	int divY;
	HRGN hrgn;
}HINTDLGDELETE,*LPHINTDLGDELETE;

typedef class ChatHintAddPlayerDlg
{
public:
	ChatHintAddPlayerDlg();
	~ChatHintAddPlayerDlg();
	void ChatHintDlgProcessOnButton(HWND hwnd ,WPARAM wParam,LPARAM lParam);
	void OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void ChatHintDlgProcessDrawButton(LPDRAWITEMSTRUCT lpdis);
	void ChatHintDlgProcessPaint(HDC hdc );
	void ChatHintDlgLoadSource();
	void LoadSource_Clan();
	void LoadSource_Lued();
	BOOL ChatHintDlgCreate(HWND hParent);
	BOOL ChatHintDlgCreate(HWND hParent, DLGPROC func);
	void ChatHintDlgAdjustWindow(HWND hwnd);
	static BOOL CALLBACK ChatHintDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void ChatHintDlgProcessMouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void ChatHintDlgProcessMouseLeave(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void ChatHintDlgShow(BOOL bShow);
	BOOL ChatHintDlgIsShow(){return isShow;}
	HWND hDlg;
public:
	int bkBitmapIdx;
	int editBkIdx;
	HBRUSH  hEditBrush;
	HFONT   hEditFont;
	HRGN    hRgn;
	ChatButton okButton;
	ChatButton noButton;
	int width ,height;
	int x,y;
	HWND hEdit;
	int editWidth;
	int editHeight;
	int editX;
	int editY;
	int divY;
	int divX1;
	int divY1;
	int bkMaskX;
	int bkMaskY;

	char font[256];
	char wndText[256];
	char infoText[256];
	bool isUseDefaultFont;
	
	BOOL isShow;
}HINTDLGADD,*LPHINTDLGADD;



#define _TITLE_FONT_NAME_SIZE    256

#define _TITLE_INI_ITEM_NAME   "friendList-title"
#define _TITLE_INI_X           "x"
#define _TITLE_INI_Y           "y"
#define _TITLE_INT_WIDTH       "width"
#define _TITLE_INI_HEIGHT      "height"
#define _TITLE_INI_NAME_WIDTH  "nameItemWidth"
#define _TITLE_INI_METIER_WIDTH "metierItemWidth"
#define _TITLE_INI_LEVEL_WIDTH  "levelItemWidth"
#define _TITLE_INI_GROUP_WIDTH  "groupItemWidth"
#define _TITLE_INI_PLACE_WIDTH  "placeItemWidth"
#define _TITLE_INI_FONT          "font"
#define _TITLE_INI_FONT_NORMAL_COLOR_R "normalFontColor_r"
#define _TITLE_INI_FONT_NORMAL_COLOR_G "normalFontColor_g"
#define _TITLE_INI_FONT_NORMAL_COLOR_B "normalFontColor_b"

#define _TITLE_INI_FONT_MOUSEOVER_COLOR_R "mouseOverFontColor_r"
#define _TITLE_INI_FONT_MOUSEOVER_COLOR_G "mouseOverFontColor_g"
#define _TITLE_INI_FONT_MOUSEOVER_COLOR_B "mouseOverFontColor_b"

#define _TITLE_STATE_NAME_MOUSEOVE       1
#define _TITLE_STATE_METIER_MOUSEOVER    2
#define _TITLE_STATE_LEVEL_MOUSEOVER     4
#define _TITLE_STATE_GROUP_MOUSEOVER     8
#define _TITLE_STATE_PLACE_MOUSEOVER     16


#define _TITLE_DRAG_NAMEITEM             1
#define _TITLE_DRAG_METIERITEM           2
#define _TITLE_DRAG_LEVELITEM            3
#define _TITLE_DRAG_GROUPITEM            4
#define _TITLE_DRAG_PLACEITEM            5



typedef class PlayerListTitleControl
{
public:
	PlayerListTitleControl();
	~PlayerListTitleControl();
public:
	void TitleControlInit();
	void TitleControlDrawItem(HDC hdc);
	void TitleControlUpdate();
	void TitleControlProcessMouseMove(PlayerInfoDlg& infoDlg,POINT& pos);
	void TitleControlProcessLButtonDBLCLK(PlayerInfoDlg& infoDlg,POINT& pos);
	void TitleControlProcessLButtonUp();
	void TitleControlProcessLButtonDown(PlayerInfoDlg& infoDlg,POINT& pt);
	void TitleControlSetParentHandle(HWND hwnd){hParentHandle = hwnd;}
	void TitleContrilSetDragState(int drag) { dragState = drag;}
	int TitleControlGetDragState() const { return dragState;}
	POINT TitleControlGetDragPoint() const { return dragCurrentPoint;}
protected:
	///////控件的状态//////
	int state;
	int dragState;
	POINT dragCurrentPoint;
	//////////////
	////控件的大小和位置//////
	ONRECT posRect;
	int nameItemWidth;
	int metierItemWidth;
	int levelItemWidth ;
//	int groupItemWidth ;
//	int placeItemWidth ;
	char pszFont[_TITLE_FONT_NAME_SIZE];
	bool isUseDefualFont;
	COLORREF fontNormalColor;
	COLORREF fontMouseOverColor;
	COLORREF lineColor;
	//////////////////
	HWND hParentHandle;
}TITLECONTROL,*LPTITLECONTROL;
/////////////////////////////////

#define _PLAYER_LIST_SORT_BYNAME   0
#define _PLAYER_LIST_SORT_BYMETIER 1
#define _PLAYER_LIST_SORT_BYLEVEL  2
#define _PLAYER_LIST_SORT_BYGROUP  3
#define _PLAYER_LIST_SORT_BYPLACE  4

#define _PLAYER_LIST_CONTROL_SHOW_LEFT 1
#define _PLAYER_LIST_CONTROL_NORMAL_SHOW 2


/////////////////////////
#define _PLAYER_INFO_INI_ITEM_NAME   "friendList"
#define _PLAYER_INFO_INI_ITEM_

////////////////////////
typedef class PlayerInfoDlg
{
public:
	PlayerInfoDlg();
	~PlayerInfoDlg();
	static BOOL CALLBACK PlayerInfoDlfProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void PlayerInfoDlgAdjustWnd();
	void PlayerInfoDlgProcessPaint(HDC hdc);
	void PlayerInfoDlfCreate(HWND hParent,DlgProcessFun pFun);
	void PlayerInfoDlgLoadSRC(HWND hwnd);
	void PlayerInfoDlgProcessMouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void PlayerInfoDlgProcessMouseLButtonDown(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void PlayerInfoDlgProcessMouseLButtonUp(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void PlayerInfoDlgShowDlg(BOOL bShow);
	void PlayerInfoDlgAdd(LPPLAYERCONTROL pControl);
	void PlayerInfoDlgProcessDrawButton(LPDRAWITEMSTRUCT lpdis);
	void PlayerInfoDlgProcessMouseLeave(HWND hwnd ,WPARAM wParam,LPARAM lParam);
	void PlayerInfoDlgProcessMouseWheel(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void PlayerInfoDlgProcessBTNCommand(HWND hwnd ,WPARAM wParam,LPARAM lParam);
	void PlayerInfoDlgDeletePlayers();
	void PlayerInfoDlgProcessClearControl();
	void PlayerInfoDlgUpdate();
	void PlayerInfoDlgUpdateDrawArea();
	void PlayerInfoQSort(int method);
	void PlayerInfoAddPlayers(const char* name);
	void PlayerInfoProcessLButtonDBCLK(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void PlayerInfoMakeMixAllPlayers();
	LPPLAYERCONTROL PlayerInfoGetControlByPoint(POINT& pt);
	void PlayerInfoSetState(int flag) {state|=flag;}
	void PlayerInfoRemoveState(int flag){ state&=~flag;}
	void PlayerInfoClearSelect(BOOL update);
 	void PlayerInfoProcessDeleteButtonDown();
	void PlayerInfoProcessAddButtonDown();
	void PlayerInfoProcessShowLeftDown();
	static LRESULT CALLBACK PlayerDeleteButtonProc(HWND hwnd,UINT mgs,WPARAM wParam,LPARAM lParam);
	static LRESULT CALLBACK PlayerAddButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static LRESULT CALLBACK PlayerShowLeftButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static int PlayersQsortByName(const void* p1,const void* p2);
	static int PlayersQsortByMetier(const void* p1,const void* p2);
	static int PlayersQsortByLevel(const void* p1,const void* p2);
	static int PlayersQsortByGroup(const void* p1,const void* p2);
	static int PlayersQsortByPlace(const void* p1,const void* p2);
	int PlayerInfoDlgGetState() const {return state;}
	CHATSCROLLBAR& PlayerInfoDlgGetScrollBar()  { return scroll_bar;}
public:
	int state;
	int x;
	int y;
	int width;
	int height;
	int drawPointX,drawPointY;
	int drawAreaX,drawAreaY;
	int drawAreaWidth,drawAreaHeight;
	int intermission;
	HWND hDlg;
	int bitmapIdx;
	int drawAreaBKBitmapIdx;
	BOOL    isShow;
	LPPLAYERCONTROL  pOnlinePlayers[_CHAT_MAX_FRIENDS];
	int numberOnlinePlayers;
	int numberLeftLinePlayers;
	LPPLAYERCONTROL  pLeftLinePlayers[_CHAT_MAX_FRIENDS];
	LPPLAYERCONTROL  pAllPlayers[_CHAT_MAX_FRIENDS];

	int            drawIndex ;
	ChatButton     addPlayerButton;
	ChatButton     deletePlayerButton;
	ChatButton     showLeftButton;
	ChatButton     m_wndHideShowBnt;
	ChatButton     m_wndChangeBnt;
	WNDPROC        deleteButtonProc;
	WNDPROC        addButtonProc;
	WNDPROC        showLeftButtonProc;
	PlayerListTitleControl titleControl;

	CHATSCROLLBAR     scroll_bar;

	int               allFriendsLength;
}PLAYERINFODLG,*LPPLAYERINFODLG;

#endif