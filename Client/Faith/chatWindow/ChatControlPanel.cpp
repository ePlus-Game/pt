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
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatControlPanel.h"

#include "KWin32App.h"
#include "resource.h"
#include "chatWindow/ChatMainDlg.h"
#include "GameDataDef.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/chatPage.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatFriendPanelManager.h"

#include "loadSrcWnd/GDILoadBitmap.h"
#include "chatWindow/ChatWndProc.h"
#include "chatWindow/ChatResource.h"
#include "chatWindow/ChatClanPlayerData.h"
#include "chatWindow/ChatClanListControl.h"
#include "chatWindow/ChatClanTitleControl.h"
#include "chatWindow/ChatClanInfoDlg.h"
#include "chatWindow/ChatClanPanel.h"
#include "chatWindow/ChatClanManager.h"

#include "SocialComDef.h"

ChatControlPanel::ChatControlPanel()
{
	currentPanel = _CHAT_PANEL_CHAT_PANEL;
}
ChatControlPanel::~ChatControlPanel()
{
	while(!panelButtonList.empty())	
	{
		ChatButton* pButton = panelButtonList.back();
		panelButtonList.pop_back();
		delete pButton;	
	}

}
ChatControlPanel& ChatControlPanel::ChatPanelGetPanel()
{
	static ChatControlPanel panel;
	return panel;
}


void ChatControlPanel::ChatPanelInit(HWND hwnd)
{
#define _CHAT_BUTTON_TEXT  "textInfo"
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] ={0},szValue[MAX_PATH];
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);

	iniFile.Load(szPath);
	iniFile.GetInteger(_PANEL_CONTROL_INI_NAME,_CHAT_SRC_X,0,&x);
	iniFile.GetInteger(_PANEL_CONTROL_INI_NAME,_CHAT_SRC_Y,0,&y);
	iniFile.GetInteger(_PANEL_CONTROL_INI_NAME,_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger(_PANEL_CONTROL_INI_NAME,"height",0,&height);

	iniFile.GetInteger(_PANEL_CONTROL_INI_NAME,"BKSrcIdx",0,&bkBitmapIdx);

	int id = _CHAT_SYNTHES_PANEL_BUTTON_ID;
	int r=0,g=0,b=0,isClip=0;
	char* itemName[] = {_PANEL_CONTROL_INI_CHAT_PAGE, _PANEL_CONTROL_INI_FRIEND_PAGE,
		_PANEL_CONTROL_INI_TRUST_PAGE, _PANEL_CONTROL_INI_CLAN_PAGE, _PANEL_CONTROL_INI_LUED_PAGE};
	for(int i = 0; i < _CHAT_PANEL_BUTTON_NUMBER;i++)
	{
		int x,y,width,height;
		iniFile.GetInteger(itemName[i],_CHAT_SRC_X,0,&x);
		iniFile.GetInteger(itemName[i],_CHAT_SRC_Y,0,&y);
		iniFile.GetInteger(itemName[i],_CHAT_SRC_WIDTH,0,&width);
		iniFile.GetInteger(itemName[i],_CHAT_SRC_HEIGHT,0,&height);
		iniFile.GetInteger(itemName[i],_CHAT_SRC_CLIP,0,&isClip);

		if(isClip==1)
		{
			
			iniFile.GetInteger(itemName[i],_CHAT_SRC_CLIP_COLOR_R,0,&r);
			iniFile.GetInteger(itemName[i],_CHAT_SRC_CLIP_COLOR_G,0,&g);
			iniFile.GetInteger(itemName[i],_CHAT_SRC_CLIP_COLOR_B,0,&b);

		}
		ChatButton* pButton = new ChatButton;
		pButton->ChatWndCreate(id+i,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,hwnd,"","button",x,y,width,height);
		int normal_idx = 0,hover_idx = 0,pushed_idx = 0;
		iniFile.GetInteger(itemName[i],"normalSrcIdx",0,&normal_idx);
		iniFile.GetInteger(itemName[i],"mouseOverSrcIdx",0,&hover_idx);
		iniFile.GetInteger(itemName[i],"mouseDownSrcIdx",0,&pushed_idx);
		pButton->ChatWndSetResource(normal_idx,hover_idx,pushed_idx,-1);
		pButton->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		if(isClip)
		{
//			ChatWnd::BitmapToRgn(pSrc->hNormalSrc,pButton->ChatWndGetRgn(),RGB(r,g,b));
//			SetWindowRgn(pButton->ChatWndGetHandle(),pButton->ChatWndGetRgn(),TRUE); 
		}
		char textInfo[256]={0};
		iniFile.GetString(itemName[i],_CHAT_BUTTON_TEXT,"",textInfo,256);
		if(textInfo[0]!=0)
		{
			pButton->ChatWndSetText(textInfo);
			iniFile.GetString(itemName[i],"fontColor","",szValue,MAX_PATH);
			sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			pButton->ChatWndSetTextNormalColor(RGB(r,g,b));
			
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
				pButton->ChatWndSetFont(font);
			else
			{
				pButton->ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				pButton->ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
		BOOL isHaveTip = FALSE;
		iniFile.GetString(itemName[i],"tooltipInfo","",szValue,MAX_PATH);
		if(szValue[0]!=0)
			pButton->ChatWndTipCreate(TTS_NOPREFIX,szValue,200);
		panelButtonList.push_back(pButton);
//		buttonProc[i] = (WNDPROC)SetWindowLong(pButton->ChatWndGetHandle(),GWL_WNDPROC,(LONG)(ChatControlPanel::ChatPanelButtonProc));
	}
	panelButtonList[_CHAT_PANEL_CHAT_PANEL]->SetWndProcessFun(ChatWndProcessFun::ProcessChatInfoPageButton);
	panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->SetWndProcessFun(ChatWndProcessFun::ProcessFrindInfoPageButton);
	panelButtonList[_CHAT_PANEL_TRUST_PANEL]->SetWndProcessFun(ChatWndProcessFun::ProcessEntrustPageButton);
	panelButtonList[_CHAT_PANEL_CLAN_PANEL]->SetWndProcessFun(ChatWndProcessFun::ProcessClanPageButton);
	panelButtonList[_CHAT_PANEL_LUED_PANEL]->SetWndProcessFun(ChatWndProcessFun::ProcessLuedPageButton);
	
	ShowWindow( panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetHandle(), SW_HIDE );

	//panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
	panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);

	panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndMove(panelButtonList[1]->x+2,
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->y,
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().right-4,
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().bottom,true);

	panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndMove(panelButtonList[2]->x+2,
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->y,
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetRect().right-4,
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetRect().bottom,true);

	panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndMove(panelButtonList[3] ->x + 2,
		panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->y,
		panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndGetRect().right - 4,
		panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndGetRect().bottom, true);

	panelButtonList[_CHAT_PANEL_LUED_PANEL] ->ChatWndMove(panelButtonList[3] ->x + 2,
		panelButtonList[_CHAT_PANEL_LUED_PANEL] ->y,
		panelButtonList[_CHAT_PANEL_LUED_PANEL] ->ChatWndGetRect().right - 4,
		panelButtonList[_CHAT_PANEL_LUED_PANEL] ->ChatWndGetRect().bottom, true);
#ifndef Enable_Auto_Attack
	ShowWindow(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetHandle(),SW_HIDE);
#endif
}
BOOL ChatControlPanel::ChatPanelCreate(HWND hwnd)
{
	if((hPanelDlg =CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_PANEL_DLG),
		hwnd,
		(DLGPROC)ChatControlPanel::ChatPanelProc))==0)
		return FALSE;
	ChatPanelShow(FALSE);
	return TRUE;
}
void ChatControlPanel::ChatPanelAdjustWindow()
{
	RECT rc;
	GetWindowRect(ChatMainDlg::hMainDlg,&rc);
	RECT rClient;
	GetClientRect(ChatMainDlg::hMainDlg,&rClient);
//	height = rClient.bottom;
	AdjustWindowRectEx(&rClient,GetWindowStyle(ChatMainDlg::hMainDlg),
			               GetMenu(ChatMainDlg::hMainDlg)!=NULL,GetWindowExStyle(ChatMainDlg::hMainDlg));
	int dx = -rClient.left;
	int dy = -rClient.top;
	MoveWindow(hPanelDlg,rc.left+dx+x,rc.top+dy+y,width,height,TRUE);
}	

void ChatControlPanel::ChatPanelShowChatInfoPanel()
{

	ChatButton* pButton = panelButtonList[_CHAT_PANEL_CHAT_PANEL];
	if(pButton->ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN)
		return;
	pButton->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
	pButton->ChatWndMove(pButton->x,pButton->y,pButton->ChatWndGetRect().right,pButton->ChatWndGetRect().bottom,true);
	pButton->ChatWndUpdate();

	if(panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->y,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndUpdate();
	}

	if(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->y,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndUpdate();
	}

	if (panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->y,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndUpdate();
	}

	if (panelButtonList[_CHAT_PANEL_LUED_PANEL] ->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->y,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndUpdate();
	}

	currentPanel = _CHAT_PANEL_CHAT_PANEL;
	ChatMainDlg::MainDlgShowWndChat(TRUE);

}
void ChatControlPanel::ChatPanelShowFriendInfoPanel()
{
//	panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
//	panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndUpdate();


//	panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
//	panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndUpdate();

	if(panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN)
	{
		if(ChatFriendPanelManager::ChatFriendManagerGet().needUpdate)
		{
			ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerReceiveFriendList();
			ChatFriendPanelManager::ChatFriendManagerGet().needUpdate = false;
		}
		return;
	}
	panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
	panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->x,
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->y,
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().right,
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().bottom,true);
	panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndUpdate();	

	if(panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_CHAT_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->y,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetRect().bottom,true);
    	panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndUpdate();
	}
	if(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->y,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndUpdate();
	}
	
	if (panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->y,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndUpdate();
	}

	if (panelButtonList[_CHAT_PANEL_LUED_PANEL] ->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->y,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndUpdate();
	}

	currentPanel = _CHAT_PANEL_FRIEND_PANEL;
	ChatMainDlg::MainDlgShowWndChat(TRUE);
	ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerReceiveFriendList();

}
void ChatControlPanel::ChatPanelShowEntrustPanel()
{
	if(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN)
		return;
	panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
	panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x,
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->y,
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetRect().right,
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetRect().bottom,true);
	panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndUpdate();

//	panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
//	panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndUpdate();

//	panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
//	panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndUpdate();

	if(panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_CHAT_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->y,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndUpdate();
//	panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndUpdate();
	}
	if(panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->y,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndUpdate();
	}

	if (panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->y,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndUpdate();
	}

	if (panelButtonList[_CHAT_PANEL_LUED_PANEL] ->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->y,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndUpdate();
	}
					
	currentPanel=_CHAT_PANEL_TRUST_PANEL;
	ChatMainDlg::MainDlgShowWndChat(TRUE);

}

void ChatControlPanel::ChatPanelShowClanPanel()
{
	if(panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN)
		return;
	panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
	panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_CLAN_PANEL]->x,
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->y,
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndGetRect().right,
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndGetRect().bottom,true);
	panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndUpdate();	

	if(panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_CHAT_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->y,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetRect().bottom,true);
    	panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndUpdate();
	}

	if (panelButtonList[_CHAT_PANEL_FRIEND_PANEL] ->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->y,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndUpdate();
	}

	if(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->y,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndUpdate();
	}
	
	if (panelButtonList[_CHAT_PANEL_LUED_PANEL] ->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->y,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndUpdate();
	}

	currentPanel = _CHAT_PANEL_CLAN_PANEL;
	ChatMainDlg::MainDlgShowWndChat(TRUE);
}


void ChatControlPanel::ChatPanelShowLuedPanel()
{
	if(panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN)
		return;
	panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
	panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_CLAN_PANEL]->x,
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->y,
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndGetRect().right,
		panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndGetRect().bottom,true);
	panelButtonList[_CHAT_PANEL_LUED_PANEL]->ChatWndUpdate();	

	if(panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_CHAT_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->y,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetRect().bottom,true);
    	panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndUpdate();
	}

	if (panelButtonList[_CHAT_PANEL_FRIEND_PANEL] ->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->y,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndUpdate();
	}

	if(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->y,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndUpdate();
	}
	
	if (panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndGetState() != _CHAT_BUTTON_STATE_NORMAL)
	{
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndMove(panelButtonList[_CHAT_PANEL_TRUST_PANEL]->x+2,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->y,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndGetRect().right-4,
			panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndGetRect().bottom,true);
		panelButtonList[_CHAT_PANEL_CLAN_PANEL]->ChatWndUpdate();
	}

	currentPanel = _CHAT_PANEL_LUED_PANEL;
	ChatMainDlg::MainDlgShowWndChat(TRUE);
}

void ChatControlPanel::ChatPanelShow(BOOL show)
{
	if(show)
	{
		if(!IsWindowVisible(hPanelDlg))
			ShowWindow(hPanelDlg,SW_NORMAL);
	}
	else
	{
		if(IsWindowVisible(hPanelDlg))
			ShowWindow(hPanelDlg,SW_HIDE);
	}
}
void ChatControlPanel::ChatPanelDrawButton(LPDRAWITEMSTRUCT lpdis)
{
	int size = panelButtonList.size();
	for(int i = 0; i <size; i++)
	{
		ChatButton* pButton = panelButtonList[i];
		if(pButton->ChatWndGetID() == lpdis->CtlID)
		{
			pButton->ChatWndDrawItem(lpdis->hDC);
			return ;
		}
	}
}


LRESULT CALLBACK ChatControlPanel::ChatPanelButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			int id  = GetWindowLong(hwnd,GWL_ID);
			ChatControlPanel& panel = ChatControlPanel::ChatPanelGetPanel();
			if(panel.panelButtonList[panel.currentPanel]->ChatWndGetID()==id)
				return FALSE;
			switch(id)
			{
			case _CHAT_SYNTHES_PANEL_BUTTON_ID:
				{

					ChatButton* pButton = panel.panelButtonList[_CHAT_PANEL_CHAT_PANEL];
					pButton->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
					pButton->ChatWndUpdate();

					panel.panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndUpdate();

					panel.panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndUpdate();

					panel.panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndUpdate();

					panel.currentPanel = _CHAT_PANEL_CHAT_PANEL;
					ChatMainDlg::MainDlgShowWndChat(TRUE);
					SetFocus(hwnd);
					break;
				}
				return FALSE;
			case _CHAT_FRIEND_PANEL_BUTTON_ID:
				{
					panel.panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndUpdate();

					panel.panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndUpdate();

					panel.panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
					panel.panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndUpdate();

					panel.panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndUpdate();

					panel.currentPanel = _CHAT_PANEL_FRIEND_PANEL;
					ChatMainDlg::MainDlgShowWndChat(TRUE);
					ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerReceiveFriendList();
					SetFocus(hwnd);
					break;			
				}
			case _CHAT_TRUST_PANEL_BUTTON_ID:
				{
					panel.panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
					panel.panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndUpdate();

					panel.panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndUpdate();

					panel.panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndUpdate();
					
					panel.panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndUpdate();

					panel.currentPanel=_CHAT_PANEL_TRUST_PANEL;
					ChatMainDlg::MainDlgShowWndChat(TRUE);
					break;
				}
			case _CHAT_CLAN_PANEL_BUTTON_ID:
				{
					panel.panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
					panel.panelButtonList[_CHAT_PANEL_CLAN_PANEL] ->ChatWndUpdate();

					panel.panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_TRUST_PANEL]->ChatWndUpdate();

					panel.panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndUpdate();

					panel.panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					panel.panelButtonList[_CHAT_PANEL_FRIEND_PANEL]->ChatWndUpdate();

					panel.currentPanel=_CHAT_PANEL_CLAN_PANEL;
					ChatMainDlg::MainDlgShowWndChat(TRUE);
					break;
				}
			}
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatControlPanel& panel = ChatControlPanel::ChatPanelGetPanel();
			int size = panel.panelButtonList.size();
			int id = GetWindowLong(hwnd,GWL_ID);
			for(int i = 0; i <size; i++)
			{
				ChatButton* pButton  = panel.panelButtonList[i];
				if(id == pButton->ChatWndGetID())
				{
					if(pButton->ChatWndGetState()==_CHAT_BUTTON_STATE_NORMAL)
					{
						pButton->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
						pButton->ChatWndUpdate();
					}
				}
				else
				{
					if(pButton->ChatWndGetState()==_CHAT_BUTTON_STATE_MOUSEOVER)
					{
						pButton->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
						pButton->ChatWndUpdate();
					}
				}
			}
		}
		break;
	}
	WNDPROC proc=0;
	for(int i = 0; i <ChatControlPanel::ChatPanelGetPanel().panelButtonList.size();i++)
	{
		ChatButton* pButton = ChatControlPanel::ChatPanelGetPanel().panelButtonList[i];
		if(pButton->ChatWndGetHandle() == hwnd)
		{
			proc = ChatControlPanel::ChatPanelGetPanel().buttonProc[i];
			break ;
		}
	}
	return CallWindowProc(proc,hwnd,msg,wParam,lParam);
}
BOOL CALLBACK ChatControlPanel::ChatPanelProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		ChatControlPanel::ChatPanelGetPanel().ChatPanelInit(hwnd);
		SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			ChatControlPanel& panel = ChatControlPanel::ChatPanelGetPanel();
			DrawBitmap(hdc,ChatResource::GetSingle().GetResource(panel.bkBitmapIdx)->hBitmap,panel.width,panel.height);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			ChatControlPanel& panel = ChatControlPanel::ChatPanelGetPanel();
			DrawBitmap((HDC)wParam,ChatResource::GetSingle().GetResource(panel.bkBitmapIdx)->hBitmap,panel.width,panel.height);
		}
		return TRUE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		break;
	case WM_DRAWITEM:
		{
			ChatControlPanel& panel = ChatControlPanel::ChatPanelGetPanel();
			panel.ChatPanelDrawButton((LPDRAWITEMSTRUCT)lParam);
		}
		return FALSE;
	case WM_DESTROY:
		{
		
		}
		return TRUE;
	case WM_MOUSEMOVE:
		{
			ChatControlPanel& panel = ChatControlPanel::ChatPanelGetPanel();
			for(int i = 0; i< panel.panelButtonList.size();i++)
			{
				ChatButton* pButton = panel.panelButtonList[i];
				if(pButton->ChatWndGetState()!=_CHAT_BUTTON_STATE_MOUSEDOWN&&pButton->ChatWndGetState()==_CHAT_BUTTON_STATE_MOUSEOVER)
				{
					pButton->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					pButton->ChatWndUpdate();
				}
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(false);
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(false);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
		}
		return FALSE;
	}
	return FALSE;
}
