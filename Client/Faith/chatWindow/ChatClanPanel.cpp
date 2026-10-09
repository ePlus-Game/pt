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
#include "UiMDLInterface.h"
#include "Ui/UiMDLDataset.h"
#include "chatWindow/ChatClanPanel.h"

#include "chatWindow/ChatResource.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "ChatClanListControl.h"
#include "chatWindow/ChatClanTitleControl.h"
#include "chatWindow/ChatClanInfoDlg.h"
#include "chatWindow/ChatClanManager.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatControlPanel.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "chatWindow/ChatWndProc.h"

ChatClanPanel::ChatClanPanel()
{

}

ChatClanPanel::~ChatClanPanel()
{
	
}

void ChatClanPanel::CreatePanel(HWND hParent)
{
	if((hFirendDlg =CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_CONTROL),
		hParent,
		(DLGPROC)ChatClanPanel::ChatClanProc))==0)
		return;
	ChatFirendShow(FALSE);
}

void ChatClanPanel::LoadSource(HWND hwnd)
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
	int r = 0;
	int g = 0;
	int b = 0;
	int isClip = 0;
/*	char* itemName[] = {_CHAT_CLAN_PANEL_INI_CLAN,_CHAT_CLAN_PANEL_INI_LEUD,_CHAT_CLAN_PANEL_INI_NATION};
	for(int i = 0; i < _CHAT_PANEL_FRIEND_BUTTON_NUM - 1;i++)
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
	
//	buttonList[_CLAN_INFO_DLG_CLAN].SetWndProcessFun(ChatWndProcessFun::ProcessClanListButton);
//	buttonList[_CLAN_INFO_DLG_LUED].SetWndProcessFun(ChatWndProcessFun::ProcessLuedListButton);
//	buttonList[_CLAN_INFO_DLG_NATION].SetWndProcessFun(ChatWndProcessFun::ProcessNationListButton);
//	buttonList[_CLAN_INFO_DLG_CLAN].ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
//	currentIdx = _FRIEND_CONTROL_IDX_FRIEND;*/
}

BOOL CALLBACK ChatClanPanel::ChatClanProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		
	case WM_INITDIALOG:
		ChatClanManager::GetManager().m_ClanPanel.LoadSource(hwnd);
		SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			ChatClanPanel& clanPanel = ChatClanManager::GetManager().m_ClanPanel;
			HDC hdc = BeginPaint(hwnd,&ps);
			DrawBitmap(hdc,ChatResource::GetSingle().GetResource(clanPanel.bkSrcIdx)->hBitmap, clanPanel.width, clanPanel.height);
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
			ChatClanPanel& clanPanel = ChatClanManager::GetManager().m_ClanPanel;
			DrawBitmap((HDC)wParam,ChatResource::GetSingle().GetResource(clanPanel.bkSrcIdx)->hBitmap,clanPanel.width,clanPanel.height);
		}
		return TRUE;
	case WM_DESTROY:
		{
			
		}
		return false;
	}
	return FALSE;
}

ChatLuedPanel::ChatLuedPanel()
{

}

ChatLuedPanel::~ChatLuedPanel()
{
	
}

void ChatLuedPanel::CreatePanel(HWND hParent)
{
	if((hFirendDlg =CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_CONTROL),
		hParent,
		(DLGPROC)ChatLuedPanel::ChatLuedProc))==0)
		return;
	ChatFirendShow(FALSE);
}

void ChatLuedPanel::LoadSource(HWND hwnd)
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
	int r = 0;
	int g = 0;
	int b = 0;
	int isClip = 0;
/*	char* itemName[] = {_CHAT_CLAN_PANEL_INI_CLAN,_CHAT_CLAN_PANEL_INI_LEUD,_CHAT_CLAN_PANEL_INI_NATION};
	for(int i = 0; i < _CHAT_PANEL_FRIEND_BUTTON_NUM - 1;i++)
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
	
//	buttonList[_CLAN_INFO_DLG_CLAN].SetWndProcessFun(ChatWndProcessFun::ProcessClanListButton);
//	buttonList[_CLAN_INFO_DLG_LUED].SetWndProcessFun(ChatWndProcessFun::ProcessLuedListButton);
//	buttonList[_CLAN_INFO_DLG_NATION].SetWndProcessFun(ChatWndProcessFun::ProcessNationListButton);
//	buttonList[_CLAN_INFO_DLG_CLAN].ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
//	currentIdx = _FRIEND_CONTROL_IDX_FRIEND;*/
}

BOOL CALLBACK ChatLuedPanel::ChatLuedProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		
	case WM_INITDIALOG:
		ChatLuedManager::GetManager().m_ClanPanel.LoadSource(hwnd);
		SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			ChatLuedPanel& clanPanel = ChatLuedManager::GetManager().m_ClanPanel;
			HDC hdc = BeginPaint(hwnd,&ps);
			DrawBitmap(hdc,ChatResource::GetSingle().GetResource(clanPanel.bkSrcIdx)->hBitmap, clanPanel.width, clanPanel.height);
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
			ChatLuedPanel& clanPanel = ChatLuedManager::GetManager().m_ClanPanel;
			DrawBitmap((HDC)wParam,ChatResource::GetSingle().GetResource(clanPanel.bkSrcIdx)->hBitmap,clanPanel.width,clanPanel.height);
		}
		return TRUE;
	case WM_DESTROY:
		{
			
		}
		return false;
	}
	return FALSE;
}