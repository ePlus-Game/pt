

#include "KEngine.h"
#include "KPakList.h"
#include "KMp3Music.h"
#include "CoreShell.h"
#include "ErrorCode.h"
#include "Ui/UiAdapter.h"

#ifdef _USEROBOT
#include "../Login/Login.h"
#include "RobotControl.h"
#endif
#include "KWin32.h"
#include "GlobalDef.h"
#include "Faith.h"
#include "KWin32Wnd.h"
#include "iRepresentShell.h"
#include "Login/Login.h"
#include "NetConnect/NetConnectAgent.h"
#include "Ui/UiCase/UiGameSpace.h"
#include "cfs_filelogs.h"
#include "UiMDLInterface.h"
#include "mdump.h"

#include <list>
using namespace std;
#include <commctrl.h>
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "chatWindow/ChatMainDlg.h"
#include "Resource.h"

#include "chatWindow/ChatControlPanel.h"

#include <windowsx.h>
#include "loadSrcWnd/GDILoadBitmap.h"

#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatFriendPanelManager.h"

#include "chatWindow/EntrustComputerDlg.h"

HWND B2ChatDialog::m_hDialog;
int B2ChatDialog::m_dialogWidth;
int B2ChatDialog::m_dialogHeight;
//GDIRender* B2ChatDialog::pGDIRender;
//ILayout*       B2ChatDialog::pLayout;
//ILayoutRender* B2ChatDialog::pLayOutRender;

ILayoutRender* B2ChatDialog::pEditRender;
ILayoutRender* B2ChatDialog::pFaceRender;
ILayoutRender* B2ChatDialog::pItemTipRender;

int B2ChatDialog::chatWndBkBitmapIdx;
WNDPROC  B2ChatDialog::wndEditProc;
CHATMANAGER B2ChatDialog::chatManager;
BOOL        B2ChatDialog::isShow;
int         B2ChatDialog::x;
int        B2ChatDialog::y;
B2ChatDialog::B2ChatDialog()
{
	B2ChatDialog::m_dialogHeight  = 0;
	B2ChatDialog::m_dialogWidth   = 0;
	B2ChatDialog::m_hDialog       = 0;
	chatWndBkBitmapIdx =0;
//	pGDIRender = 0;
//	pLayOutRender = 0;
	B2ChatDialog::isShow = TRUE;

}
B2ChatDialog::~B2ChatDialog()
{ 

}

void B2ChatDialog::ChatDialogShowPanel(bool show)
{
	if(show)
	{
		B2ChatDialog::ChatDialogShow(TRUE);
		ChatControlPanel::ChatPanelGetPanel().ChatPanelShow(TRUE);


		ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
		ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
		ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFirendShow(FALSE);
		ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoDlgShowDlg(FALSE);

		EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgShowDlg(FALSE);
	}
	else
	{
		B2ChatDialog::ChatDialogShow(FALSE);
		ShowWindow(B2ChatDialog::chatManager.ChatManagerGetUiComboBox().hDownDialg,SW_HIDE);
		B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
	}
}
void B2ChatDialog::ChatDialogShow(BOOL isShow)
{
	B2ChatDialog::isShow = isShow;
	if(isShow)
	{
		if(!IsWindowVisible(B2ChatDialog::m_hDialog))
		{
			ShowWindow(B2ChatDialog::m_hDialog,SW_NORMAL);
		}

	}
	else
	{
		if(IsWindowVisible(B2ChatDialog::m_hDialog))
		{
			ShowWindow(B2ChatDialog::m_hDialog,SW_HIDE);
		}
	}
}
void B2ChatDialog::ChatDialogAdjustWindow()
{
	RECT rc;
	GetWindowRect(ChatMainDlg::hMainDlg,&rc);
	RECT rClient;
	GetClientRect(ChatMainDlg::hMainDlg,&rClient);
	RECT rcLeft;
	GetClientRect(ChatControlPanel::ChatPanelGetPanel().hPanelDlg,&rcLeft);
//	int height = rClient.bottom;
//	int width = rClient.right-rcLeft.right;
	AdjustWindowRectEx(&rClient,GetWindowLong(ChatMainDlg::hMainDlg,GWL_STYLE),GetMenu(ChatMainDlg::hMainDlg)!=NULL,GetWindowExStyle(ChatMainDlg::hMainDlg));

	int dx = rClient.left;
	int dy = rClient.top;
//	B2ChatDialog::m_dialogHeight = height;
//	B2ChatDialog::m_dialogWidth = width;
	int x = rc.left - dx+B2ChatDialog::x;
	int y = rc.top-dy+B2ChatDialog::y;
	MoveWindow(B2ChatDialog::m_hDialog,x,y,B2ChatDialog::m_dialogWidth ,B2ChatDialog::m_dialogHeight,TRUE);
	int x1 = B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->x;
	int y1 = B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->y;
	int height1 = B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetControlRect().bottom;
	int width1  = B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetControlRect().right;
	B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndMove(x1,y1,width1,height1,TRUE);
	B2ChatDialog::chatManager.ChatManagerGetUiComboBox().UiComBoBoxAdjustDownDlg();
	
}
 void B2ChatDialog::ChatDialogDeleteResource()
{
}
int B2ChatDialog::ChatDialogInit(HWND hwnd)
{
#define _CHAT_PAGE   "chatPage"
	if((m_hDialog=CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_CHAT_NOT_MAIN_DLG),
		hwnd,
		(DLGPROC)B2ChatDialog::ChatDialogProc))==0)
		return 0;
	if(B2ChatDialog::isShow)
	{
		KIniFile iniFile;
		TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH]={0};
		GetCurrentDirectory(MAX_PATH,szPath);
		char szImagePathIndex[]=_CHAT_SRC_PATH;
		char szImagePath[MAX_PATH] = {0};
		if(g_GetScreenWidth() == 1024)
			strcpy(szPath,_CHAT_CFG_FILE_1024);
		else
			strcpy(szPath,_CHAT_CFG_FILE);
		iniFile.Load(szPath);
		iniFile.GetInteger(_CHAT_PAGE,"x",0,&B2ChatDialog::x);
		iniFile.GetInteger(_CHAT_PAGE,"y",0,&B2ChatDialog::y);
		iniFile.GetInteger(_CHAT_PAGE,"width",0,&B2ChatDialog::m_dialogWidth);
		iniFile.GetInteger(_CHAT_PAGE,"height",0,&B2ChatDialog::m_dialogHeight);
		
		B2ChatDialog::ChatDialogShow(FALSE);
	}

	return 1;
}
void B2ChatDialog::SendKeyDownMsgToMainWnd(UINT msg,WPARAM wParam ,LPARAM lParam)
{
	switch(msg)
	{
	case WM_KEYDOWN:
		{
				SendMessage(KWin32App::m_hMainWnd,msg,wParam,lParam);
				break;
		}
		break;
	}
}
LRESULT B2ChatDialog::ChatDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
#define _CHAT_PAGE "chatPage"
	static bool isClickScrollRect = false;
//	ChatMainDlg::CheckFocus();
	switch(msg)
	{

	case WM_INITDIALOG:
		{

			B2ChatDialog::pEditRender = new GDIRender;
			B2ChatDialog::pFaceRender = new GDIRender;	
			B2ChatDialog::pItemTipRender = new GDIRender;

			B2ChatDialog::chatManager.ChatManagerCreateWnd(hwnd);
			KIniFile iniFile;
			TCHAR  szPath[MAX_PATH]={0},szValue[MAX_PATH]={0};
			char szImagePath[MAX_PATH]=_CHAT_SRC_PATH;
			if(g_GetScreenWidth() == 1024)
				strcpy(szPath,_CHAT_CFG_FILE_1024);
			else
				strcpy(szPath,_CHAT_CFG_FILE);
			iniFile.Load(szPath);
			iniFile.GetInteger(_CHAT_PAGE,"BkSrcIdx",0,&B2ChatDialog::chatWndBkBitmapIdx);
			ShowWindow(hwnd,SW_SHOW);
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
			return FALSE;
		}
	case WM_PAINT:
		{
			return B2ChatDialog::chatManager.ChatManagerProcessPaint(hwnd,wParam,lParam);
		}
		return false;
	case WM_ERASEBKGND:
		{
			B2ChatDialog::chatManager.ChatManagerProcessPaint(hwnd,wParam,lParam);
		}
		return TRUE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
			
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);

		}
		return false ;
	case WM_LBUTTONDOWN:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);

			B2ChatDialog::chatManager.IsClickScrollBarDownOrUpBnt(pt);
			if (B2ChatDialog::chatManager.IsClickScrollBarRect(pt) == TRUE)
			{
				isClickScrollRect = true;
			}
			else
			{
				isClickScrollRect = false;
			}

			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);	
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
		}
		return 0;
	case WM_LBUTTONUP:
		{
			isClickScrollRect = false;
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			B2ChatDialog::chatManager.ClickScrollBar(pt);
		}
		return 0;
	case WM_DESTROY:

		B2ChatDialog::ChatDialogDeleteResource();
		{
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndDeleteSrc();
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndDeleteSrc();
			for(int i = 0; i < _CHAT_NORMAL_BUTTON_NUMBER;i++)
				B2ChatDialog::chatManager.normalButton[i].ChatWndDeleteSrc();
			B2ChatDialog::chatManager.showButton.ChatWndDeleteSrc();
			B2ChatDialog::chatManager.chatChannels.clear();
			HIMC hTempImc = ImmAssociateContext(B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndGetHandle(),B2ChatDialog::chatManager.ChatManagerGetEditBox()->hImc);
			ImmDestroyContext(hTempImc);
		}
		return 1;
	case WM_MOUSEMOVE:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);

			if (isClickScrollRect)
			{
				B2ChatDialog::chatManager.ProcessScrollBarRectMove(pt);
			}

			B2ChatDialog::chatManager.ChatManagerGetScrollBar()->ChatScrollBarProcessMouseMove(pt);
		}
		return 0;
	}
	return FALSE;
}

void B2ChatDialog::SwapChannel()
{
	B2ChatDialog::chatManager.SwapChannel();
}