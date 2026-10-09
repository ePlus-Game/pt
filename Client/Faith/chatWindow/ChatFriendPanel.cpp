#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <vector>
#include <list>
#include <string>
using std::string;
using std::list;
using std::vector;
#include "layoutinterface.h"
#include "GameDataDef.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "KWin32App.h"
#include "resource.h"
#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatControlPanel.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "loadSrcWnd/GDILoadBitmap.h"



#include "chatWindow/ChatTipWndItem.h"

#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "chatWindow/ChatWndProc.h"
#include "chatWindow/ChatResource.h"

ChatFriendPanel::ChatFriendPanel()
{
	bkSrcIdx = 0;
	currentIdx = 0;
}
ChatFriendPanel::~ChatFriendPanel()
{

}
void ChatFriendPanel::ChatFriendCreate(HWND hParent)
{
	if((hFirendDlg =CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_CONTROL),
		hParent,
		(DLGPROC)ChatFriendPanel::ChatFriendProc))==0)
		return ;
	ChatFirendShow(FALSE);
}
BOOL CALLBACK ChatFriendPanel::ChatFriendProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		
	case WM_INITDIALOG:
		ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendLoadSource(hwnd);
		SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		return FALSE;
//	case WM_DRAWITEM:
//		ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendDrawButton((LPDRAWITEMSTRUCT)lParam);
//		return FALSE;
//	case WM_COMMAND:
//		ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendProcessLClickButton(hwnd,wParam,lParam);
//		return FALSE;
//	case WM_MOUSEMOVE:
//		ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendProcessMouseMove(hwnd,wParam,lParam);
//		return FALSE;
//	case WM_MOUSELEAVE:
//		ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendProcessMouseLeave(hwnd,wParam,lParam);
//		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			ChatFriendPanel& friendPanel = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel;
			HDC hdc = BeginPaint(hwnd,&ps);
			DrawBitmap(hdc,ChatResource::GetSingle().GetResource(friendPanel.bkSrcIdx)->hBitmap,friendPanel.width,friendPanel.height);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			SetFocus(hwnd);
		}
		return false;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			ChatFriendPanel& friendPanel = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel;
			DrawBitmap((HDC)wParam,ChatResource::GetSingle().GetResource(friendPanel.bkSrcIdx)->hBitmap,friendPanel.width,friendPanel.height);
		}
		return TRUE;
	case WM_DESTROY:
		{
			
		}
		return false;
	}
	return FALSE;
}
/*
void ChatFriendPanel::ChatFriendDrawButton(LPDRAWITEMSTRUCT lpdis)
{
	for(int i = 0; i < _CHAT_PANEL_FRIEND_BUTTON_NUM; i++)
	{
		ChatButton& button = buttonList[i];
		if(lpdis->CtlID == pButton->ChatWndGetID())
		{
			pButton->ChatWndDrawItem(lpdis->hDC);
			return ;
		}
	}
		
}*/
void ChatFriendPanel::ChatFriendProcessLClickButton(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	
/*	int size = buttonList.size();
	int id = LOWORD(wParam);
	ChatButton* pButton=0;
	if(HIWORD(wParam)==BN_CLICKED)
	{
		for(int i = 0; i < size; i++)
		{
			if(buttonList[i]->ChatWndGetID() == id)
			{
				pButton = buttonList[i];
			}
			else
			{
				if(buttonList[i]->ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN)
				{
					buttonList[i]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					buttonList[i]->ChatWndUpdate();
				}
			}
		}
		if(pButton == 0)
			return;
		if(pButton->ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN)
			return;
		pButton->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
		pButton->ChatWndUpdate();
		ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerHideInfoDlg();
		ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
		ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
		switch(id)
		{
		case _CHAT_PANEL_FRIEND_BUTTON_ID:
			pCurrentDlg->PlayerInfoClearSelect(FALSE);
			pCurrentDlg = &ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_CHAT_PLAYER_INFO_FRIEND];
			break;
		case _CHAT_PANEL_ENEMY_BUTTON_ID:
			pCurrentDlg->PlayerInfoClearSelect(FALSE);
			pCurrentDlg = &ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_CHAT_PLAYER_INFO_ENEMY];
			break;
		case _CHAT_PANEL_PINGBI_BUTTON_ID:
			pCurrentDlg->PlayerInfoClearSelect(FALSE);
			pCurrentDlg = &ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_CHAT_PLAYER_INFO_PINGBI];
			break;
/*		case _CHAT_PANEL_ROOM_BUTTON_ID:
			pCurrentDlg->PlayerInfoClearSelect(FALSE);
			pCurrentDlg = &ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_CHAT_PLAYER_INFO_ROOM];
			break;
		case _CHAT_PANEL_TEMP_BUTTON_ID:
			pCurrentDlg->PlayerInfoClearSelect(FALSE);
			pCurrentDlg = &ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_CHAT_PLAYER_INFO_TEMP];
			break;
			
		}
		pCurrentDlg->PlayerInfoDlgShowDlg(TRUE);
//		SetFocus( pCurrentDlg->hDlg);
		
	}*/

}
void ChatFriendPanel::ChatFriendProcessMouseLeave(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
/*	POINT mouse_pt;
	POINT pt;
	GetCursorPos(&mouse_pt);
	RECT btn_rect;
	int size = buttonList.size();
	for(int i = 0; i<size;i++)
	{
		pt.x = mouse_pt.x;
		pt.y = mouse_pt.y;
		ChatButton* button = buttonList[i]; 
		GetClientRect(button->ChatWndGetHandle(),&btn_rect);
		ScreenToClient(button->ChatWndGetHandle(),&pt);
		if(IsInRect(pt,btn_rect))
		{
			if(button->ChatWndGetState() !=_CHAT_BUTTON_STATE_MOUSEDOWN)
			{
				button->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button->ChatWndUpdate();
				return;
			}
		}
	}*/
}

void ChatFriendPanel::ChatFriendProcessMouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
/*	TRACKMOUSEEVENT tme;
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_HOVER|TME_LEAVE;
	tme.dwHoverTime=1000;
	tme.hwndTrack=hwnd;
	_TrackMouseEvent(&tme);
	
	
	int size = buttonList.size();
	for(int i = 0;i<size; i++)
	{
		ChatButton* button = buttonList[i];
		if(button->ChatWndGetState() ==_CHAT_BUTTON_STATE_MOUSEOVER)
		{
			button->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			button->ChatWndUpdate();
			return ;
			
		}
	}*/

}
void ChatFriendPanel::ChatFirendShow(BOOL isShow)
{
	if(isShow)
	{
		if(!IsWindowVisible(hFirendDlg))
			ShowWindow(hFirendDlg,SW_NORMAL);
	}
	else
	{
		if(IsWindowVisible(hFirendDlg))
			ShowWindow(hFirendDlg,SW_HIDE);
	}
}
void ChatFriendPanel::ChatFriendAdjustWindow()
{
	
	RECT rc;
	GetWindowRect(ChatMainDlg::hMainDlg,&rc);
	RECT rClient;
	GetClientRect(ChatMainDlg::hMainDlg,&rClient);
//	RECT rcLeft;
//	GetClientRect(ChatControlPanel::ChatPanelGetPanel().hPanelDlg,&rcLeft);
//	width = rClient.right-rcLeft.right;
	AdjustWindowRectEx(&rClient,GetWindowLong(ChatMainDlg::hMainDlg,GWL_STYLE),GetMenu(ChatMainDlg::hMainDlg)!=NULL,GetWindowExStyle(ChatMainDlg::hMainDlg));

	int dx = rClient.left;
	int dy = rClient.top;
//	int x = rc.left - dx+rcLeft.right;//B2ChatDialog::x;
//	int y = rc.top-dy;//B2ChatDialog::y;
	int x = this->x - dx+rc.left;
	int y = this->y - dy+rc.top;
	MoveWindow(hFirendDlg,x,y,width ,height,TRUE);
}


void ChatFriendPanel::ChatFriendLoadSource(HWND hwnd)
{
#define _CHAT_WND_BUTTON_TEXT "textInfo"
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = { 0},szValue[MAX_PATH]= {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	iniFile.GetInteger(_FRIEND_CONTROL_INI_NAME,_CHAT_SRC_X,0,&x);
	iniFile.GetInteger(_FRIEND_CONTROL_INI_NAME,_CHAT_SRC_Y,0,&y);
	iniFile.GetInteger(_FRIEND_CONTROL_INI_NAME,_CHAT_SRC_HEIGHT,0,&height);
	iniFile.GetInteger(_FRIEND_CONTROL_INI_NAME,_CHAT_SRC_WIDTH,0,&width);

	iniFile.GetInteger(_FRIEND_CONTROL_INI_NAME,"BKSrcIdx",0,&bkSrcIdx);
	int id = _CHAT_PANEL_FRIEND_BUTTON_ID;
	int r=0,g=0,b=0,isClip=0;
	char* itemName[] = {_FRIEND_CONTROL_INI_FRIEND,_FRIEND_CONTROL_INI_ENEMY,_FRIEND_CONTROL_INI_PINGBI};
	for(int i = 0; i < _CHAT_PANEL_FRIEND_BUTTON_NUM;i++)
	{
		int x,y,width,height;
		iniFile.GetInteger(itemName[i],_CHAT_SRC_X,0,&x);
		iniFile.GetInteger(itemName[i],_CHAT_SRC_Y,0,&y);
		iniFile.GetInteger(itemName[i],_CHAT_SRC_HEIGHT,0,&height);
		iniFile.GetInteger(itemName[i],_CHAT_SRC_WIDTH,0,&width);
		iniFile.GetInteger(itemName[i],_CHAT_SRC_CLIP,0,&isClip);
		buttonList[i].ChatWndCreate(id+i,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,hwnd,"","button",x,y,width,height);
		int normal_idx = 0,hover_idx = 0,pushed_idx=0;
		iniFile.GetInteger(itemName[i],"normalSrcIdx",0,&normal_idx);
		iniFile.GetInteger(itemName[i],"mouseOverSrcIdx",0,&hover_idx);
		iniFile.GetInteger(itemName[i],"mouseDownSrcIdx",0,&pushed_idx);
		buttonList[i].ChatWndSetResource(normal_idx,hover_idx,pushed_idx,-1);
		buttonList[i].ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		if(isClip)
		{
			iniFile.GetString(itemName[i],"clipColor",0,szValue,MAX_PATH);
			sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			ChatWnd::BitmapToRgn(ChatResource::GetSingle().GetResource(normal_idx)->hBitmap,buttonList[i].ChatWndGetRgn(),RGB(r,g,b));
		    SetWindowRgn(buttonList[i].ChatWndGetHandle(),buttonList[i].ChatWndGetRgn(),TRUE);
		}
		char textInfo[256]={0};
		iniFile.GetString(itemName[i],_CHAT_WND_BUTTON_TEXT,"",textInfo,256);
		if(textInfo[0]!=0)
		{
			buttonList[i].ChatWndSetText(textInfo);
			buttonList[i].ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
			iniFile.GetString(itemName[i],"fontColor","",szValue,MAX_PATH);
			sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			buttonList[i].ChatWndSetTextNormalColor(RGB(r,g,b));

			char font[FONT_SIZE]={0};
			iniFile.GetString(itemName[i],"font","",font,FONT_SIZE);
			TFONT tFont;
			strcpy(tFont.fontName,font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0]=0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
				buttonList[i].ChatWndSetFont(font);
			else
			{
				buttonList[i].ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				buttonList[i].ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
			
		}
	}
	buttonList[_FRIEND_CONTROL_IDX_FRIEND].SetWndProcessFun(ChatWndProcessFun::ProcessFriendListButton);
	buttonList[_FIREND_CONTROL_IDX_ENEMY].SetWndProcessFun(ChatWndProcessFun::processEnemyListButton);
	buttonList[_FRIEND_CONTROL_IDX_PINGBI].SetWndProcessFun(ChatWndProcessFun::processPingbiListButton);
	buttonList[_FRIEND_CONTROL_IDX_FRIEND].ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
	currentIdx = _FRIEND_CONTROL_IDX_FRIEND;

}