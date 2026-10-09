#ifndef _CHAT_DIALOG_H
#define _CHAT_DIALOG_H
typedef class B2ChatDialog
{
public:
	B2ChatDialog();
	~B2ChatDialog();
	int  ChatDialogInit(HWND hwnd);
	static LRESULT CALLBACK ChatDialogProc(HWND hwnd,UINT msg,WPARAM lParam,LPARAM wParam);
	static void ChatDialogDeleteResource();
	static void  ChatDialogAdjustWindow();
	static void ChatDialogShow(BOOL isShow);
	static void ChatDialogShowPanel(bool show);
	static void SendKeyDownMsgToMainWnd(UINT msg,WPARAM wParam ,LPARAM lParam);
	static void SwapChannel();
public:
	static HWND m_hDialog;
	static int m_dialogWidth;
	static int m_dialogHeight;
	static ILayoutRender* pEditRender;
	static ILayoutRender* pFaceRender;
	static ILayoutRender* pItemTipRender;
	static int       chatWndBkBitmapIdx;
	static WNDPROC  wndEditProc;
//	static WNDPROC  wndInfoProc;
	static CHATMANAGER chatManager;
	static BOOL        isShow;
	static int         x;
	static  int        y;
}CHATDLG,*LPCHATDLG;
#endif