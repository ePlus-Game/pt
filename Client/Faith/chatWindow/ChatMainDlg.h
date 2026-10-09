#ifndef _CHAT_MAIN_DLG_H
#define _CHAT_MAIN_DLG_H

#include "layoutinterface.h"

#define _CHAT_SRC_PATH              "ui/imagesets/image/"
#define _CHAT_CFG_FILE              "UiSettings\\wndChatCfg.ini"
#define _CHAT_CFG_FILE_1024              "UiSettings\\wndChatCfg1024.ini"
#define USING_CHAT_WINDOW
#define Enable_Auto_Attack
#define _CHAT_SRC_X                 "x"
#define _CHAT_SRC_Y                 "y"
#define _CHAT_SRC_WIDTH             "width"
#define _CHAT_SRC_HEIGHT            "height"
#define _CHAT_SRC_CLIP              "isClip"
#define _CHAT_SRC_CLIP_COLOR_R      "clipColorR"
#define _CHAT_SRC_CLIP_COLOR_G      "clipColorG"
#define _CHAT_SRC_CLIP_COLOR_B      "clipColorB"
#define _CHAT_SRC_NORMAL             "normalSrc"
#define _CHAT_SRC_MOUSEOVER          "mouseOverSrc"
#define _CHAT_SRC_MOUSEDOWN          "mouseDownSrc"

#define CHAT_STRING_SIZE            64
#define _SYSCOLOR_CHANGE_TIME       200
#define _SYSCOLOR_CHANGE_TIME_ID    98
typedef struct  ChatString
{
	char  taskString[CHAT_STRING_SIZE];

	char  chSynthetizeName[CHAT_STRING_SIZE];
	char  chNearName[CHAT_STRING_SIZE];
	char  chWorldName[CHAT_STRING_SIZE];
	char  chSystemName[CHAT_STRING_SIZE];
	char  chPersonalName[CHAT_STRING_SIZE];
	char  chOrgName[CHAT_STRING_SIZE];
	char  chFightName[CHAT_STRING_SIZE];
	char  chMapName[CHAT_STRING_SIZE];

	char  chSizhuName[CHAT_STRING_SIZE];
	char  chZhuhouName[CHAT_STRING_SIZE];
	char  chTeamName[CHAT_STRING_SIZE];
	char  chGuojiaName[CHAT_STRING_SIZE];
	char  chBattlefield[CHAT_STRING_SIZE];

	char  titlePlayerName[CHAT_STRING_SIZE];
	char  titlePlayerMetier[CHAT_STRING_SIZE];
	char  titlePlayerLevel[CHAT_STRING_SIZE];
	char  titlePlayerGroup[CHAT_STRING_SIZE];
	char  titlePlayerPlace[CHAT_STRING_SIZE];

	char  personalShowIn[CHAT_STRING_SIZE];
	char  personalShowOut[CHAT_STRING_SIZE];

	char  autoGoInfo[CHAT_STRING_SIZE];
	char  chatDefualtFont[CHAT_STRING_SIZE];

	int   chatDefualtFontHeight;
	int   fontExtra;
	char  playerShowInfoZhuhou[CHAT_STRING_SIZE];
	char  playerShowInfoSizhu[CHAT_STRING_SIZE];
	char  playerNotHaveZhuhou[CHAT_STRING_SIZE];
	char  playerNotHaveSizhu[CHAT_STRING_SIZE];
	char  playerExpExtra[CHAT_STRING_SIZE];
	char  hideGameWnd[CHAT_STRING_SIZE];
	char  showGameWnd[CHAT_STRING_SIZE];
	char  pathHelpText[CHAT_STRING_SIZE];
	DWORD  pathHelpTextColor;
	DWORD  carriedTextColor;




	ChatString(){}
	~ChatString(){}
	void ChatStringLoadFronINI();
	static  ChatString& ChatStringGetString();
}CHATWNDSTRING,*LPCHATWNDSTRING;

void CALLBACK SystemColorChangedTimeProc(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime);
typedef class ChatMainDlg
{
public:
	ChatMainDlg();
	~ChatMainDlg(){}
	int  MainDlgInit();
	static BOOL CALLBACK MainDlgProc(HWND hwnd,UINT msg,WPARAM lParam,LPARAM wParam);
	static void MainDlgDeleteResource();
	static void MainDlgAdjustWindow();
	static void MainDlgShow(BOOL isShow);
	static void MainDlgShowWndChat(BOOL bShow);
	static void MainDlgDestroyAll();
	static void MainDlgCloseChannel();
	static void CheckFocus();
	static void InsertSystemMsg(ILayout* pLayout);
	static void InsertSystemMsg(const char* pText);
	static void RegisterLocalChannel();
	static void AjustMainWindow();
	static void UpdateFriendList();
	static void ReceiveFriendList();
public:
	static HWND hMainDlg;
//	static int  width;
//	static int  height;
	static int    mainWndSrcIdx;
	static BOOL   isShow;
	static NOTIFYICONDATA taskInfo;
	static BOOL     movingItself;
	static DWORD    attr;
	static bool     isCreate;
	static HCURSOR  hCursor;
	static HICON    hIcon;
}CHATMAINDLG,*LPCHATMAINDLG;
#endif