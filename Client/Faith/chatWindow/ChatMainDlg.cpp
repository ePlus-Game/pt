#include <windows.h>
#include <commctrl.h>
#include <list>
#include <string>
#include <vector>
using std::vector;
using  std::string;
using std::list;
#include "Resource.h"
#include "KWin32App.h"
#include <windowsx.h>
#include "layoutinterface.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "GameDataDef.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "chatWindow/ChatControlPanel.h"
#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "loadSrcWnd/GDILoadBitmap.h"
#include "ui/UiCase/UiChatWindow.h"

#include "chatWindow/EntrustComputerDlg.h"
#include "chatWindow/ChatResource.h"
#include "chatWindow/ChatWndProc.h"
#include "chatWindow/LookFriendInfoPopDlg.h"
#include "chatWindow/ChatMiniMap.h"
#include "chatWindow/ChatPlayerBaseInfoDlg.h"

#include "Ui/UiCase/UiMapCentre.h"
#include "chatWindow/ChatClanPanel.h"
#include "chatWindow/ChatClanListControl.h"
#include "chatWindow/ChatClanTitleControl.h"
#include "chatWindow/ChatClanInfoDlg.h"
#include "chatWindow/ChatClanManager.h"
#include "chatWindow/ChatClanPopMenu.h"
#include "chatWindow/ChatClanComboBox.h"
#include "chatWindow/ChatClanAnnouncementDlg.h"

HWND ChatMainDlg::hMainDlg;
int ChatMainDlg::mainWndSrcIdx;
BOOL    ChatMainDlg::isShow;
NOTIFYICONDATA ChatMainDlg::taskInfo;
BOOL  ChatMainDlg::movingItself = FALSE;
DWORD ChatMainDlg::attr = 0;
HCURSOR ChatMainDlg::hCursor = 0;
HICON   ChatMainDlg::hIcon = 0;
ChatString& ChatString::ChatStringGetString()
{
	static ChatString chatString;
	return chatString;
}
void ChatString::ChatStringLoadFronINI()
{
	KIniFile iniFile;
	if(g_GetScreenWidth() == 1024)
		iniFile.Load(_CHAT_CFG_FILE_1024);
	else
		iniFile.Load(_CHAT_CFG_FILE);
	taskString[0]=0;
	char tempBuffer[256]={0};
	iniFile.GetString("ChatString","taskString","",taskString,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHSynthetize","",chSynthetizeName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHNear","",chNearName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHWorld","",chWorldName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHSystem","",chSystemName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHPersonal","",chPersonalName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHOrg","",chOrgName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHFight","",chFightName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHMap","",chMapName,CHAT_STRING_SIZE);

	iniFile.GetString("ChatString","CHSizhu","",chSizhuName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHTeam","",chTeamName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHGuojia","",chGuojiaName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHZhuhou","",chZhuhouName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","CHBattlefield","",chBattlefield,CHAT_STRING_SIZE);

	iniFile.GetString("ChatString","playerName","",titlePlayerName,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","playerMetier","",titlePlayerMetier,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","playerLevel","",titlePlayerLevel,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","playerGroup","",titlePlayerGroup,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","playerPlace","",titlePlayerPlace,CHAT_STRING_SIZE);

	iniFile.GetString("ChatString","chatPersonalShowIn","",personalShowIn,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","chatPersonalShowOut","",personalShowOut,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","autoGoInfo","",autoGoInfo,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","chatDefualtFont","",chatDefualtFont,CHAT_STRING_SIZE);
	iniFile.GetInteger("ChatString","chatDefualtFontHeight",0,&chatDefualtFontHeight);
	iniFile.GetInteger("ChatString","fontExtra",0,&fontExtra);
	iniFile.GetString("ChatString","playerShowInfo","",playerShowInfoZhuhou,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","playerShowInfo1","",playerShowInfoSizhu,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","playerNotHaveZhuhou","",playerNotHaveZhuhou,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","playerNotHaveSizhu","",playerNotHaveSizhu,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","playerExpExtra","",playerExpExtra,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","hideGameWnd","",hideGameWnd,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","showGameWnd","",showGameWnd,CHAT_STRING_SIZE);
	iniFile.GetString("ChatString","pathHelpTextColor","",tempBuffer,256);
	int r = 0,g=0,b=0;
	sscanf(tempBuffer,"%d,%d,%d",&r,&g,&b);
	pathHelpTextColor = (r<<16)|(g<<8)|(b);
	iniFile.GetString("ChatString","carriedTextColor","",tempBuffer,256);
	sscanf(tempBuffer,"%d,%d,%d",&r,&g,&b);
	carriedTextColor = (r<<16)|(g<<8)|(b);
	iniFile.GetString("ChatString","pathHelpText","",pathHelpText,CHAT_STRING_SIZE);






}
ChatMainDlg::ChatMainDlg()
{
	ChatMainDlg::mainWndSrcIdx = 0;
	 ChatMainDlg::isShow = FALSE;
}
int ChatMainDlg::MainDlgInit()
{
	RECT rc,rc1;
	GetWindowRect(KWin32App::m_hMainWnd,&rc);
	GetClientRect(KWin32App::m_hMainWnd,&rc1);
	int hdetal = rc.bottom-rc.top - rc1.bottom;
	if((ChatMainDlg::hMainDlg =CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_CHAT_DIALOG),
		KWin32App::m_hMainWnd,
		(DLGPROC)ChatMainDlg::MainDlgProc))==0)
		return 0;
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	int width = 0;
	int height = 0;
	RECT rcMainExtra;
	GetClientRect(ChatMainDlg::hMainDlg,&rcMainExtra);
	AdjustWindowRectEx(&rcMainExtra,
		GetWindowLong(ChatMainDlg::hMainDlg,GWL_STYLE),
		GetMenu(ChatMainDlg::hMainDlg)!=NULL,GetWindowExStyle(ChatMainDlg::hMainDlg));

	iniFile.GetInteger("MAINWND",_CHAT_SRC_WIDTH,0,&width);
	
	height = rc.bottom - rc.top-rc1.bottom;
	height += g_GetScreenHeight();
	width -= rcMainExtra.left;
	MoveWindow(ChatMainDlg::hMainDlg,0,0,width,height,false);
	iniFile.GetInteger("MAINWND","bkIdx",0,&ChatMainDlg::mainWndSrcIdx);

	taskInfo.cbSize=(DWORD)sizeof(NOTIFYICONDATA); 
	taskInfo.hWnd=ChatMainDlg::hMainDlg;
	taskInfo.uID=IDI_ICON; 
	taskInfo.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ; 
	taskInfo.uCallbackMessage=WM_SHOWTASK;
	taskInfo.hIcon=LoadIcon(KWin32App::m_hInstance,MAKEINTRESOURCE(IDI_ICON)); 
	strcpy(taskInfo.szTip,ChatString::ChatStringGetString().taskString);

	ChatPlayerBaseInfoDlg::GetSingle().Create(ChatMainDlg::hMainDlg);
	ChatPlayerBaseInfoDlg::GetSingle().AdjustWindow();
	ChatPlayerBaseInfoDlg::GetSingle().SetTimer();
//	ChatPlayerBaseInfoDlg::GetSingle().Show();
	return 1;

}
void ChatMainDlg::MainDlgShowWndChat(BOOL bShow)
{
	ChatMainDlg::isShow = bShow;
	ChatMainDlg::MainDlgShow(bShow);
	if (!bShow)
	{
		ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
		ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
		ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
		ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
		ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgShow(FALSE);
		ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgShow(FALSE);
		ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
	}
	ChatControlPanel::ChatPanelGetPanel().ChatPanelShow(bShow);
	bShow?ChatMiniMap::GetSingle().Show():ChatMiniMap::GetSingle().Hide();
	bShow?ChatPlayerBaseInfoDlg::GetSingle().Show():ChatPlayerBaseInfoDlg::GetSingle().Hide();
	if(ChatControlPanel::ChatPanelGetPanel().currentPanel == _CHAT_PANEL_CHAT_PANEL)
	{
		B2ChatDialog::ChatDialogShowPanel(bShow == FALSE ? false : true);
		ChatClanManager::GetManager().ShowMemberPage(false);
		ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
		ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
		ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
		ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
		ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgShow(FALSE);
		ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgShow(FALSE);
		ChatLuedManager::GetManager().ShowMemberPage(FALSE);
		ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
	}
	else if(ChatControlPanel::ChatPanelGetPanel().currentPanel == _CHAT_PANEL_FRIEND_PANEL)
	{
		ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerShowFriendPage(bShow == FALSE ? false : true);
		ChatClanManager::GetManager().ShowMemberPage(false);
		ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
		ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
		ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgShow(FALSE);
		ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgShow(FALSE);
		ChatLuedManager::GetManager().ShowMemberPage(FALSE);
		ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
	}
	else if(ChatControlPanel::ChatPanelGetPanel().currentPanel == _CHAT_PANEL_TRUST_PANEL)
	{
		if(bShow)
		{
			B2ChatDialog::ChatDialogShowPanel(FALSE);
			ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerShowFriendPage(false);
			ChatClanManager::GetManager().ShowMemberPage(false);
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
			ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
			ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgShow(FALSE);
			ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgShow(FALSE);
			ChatLuedManager::GetManager().ShowMemberPage(FALSE);
			ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
		}
		EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgShowDlg(bShow == FALSE ? false : true);
	}
	else if (ChatControlPanel::ChatPanelGetPanel().currentPanel == _CHAT_PANEL_CLAN_PANEL)
	{
		ChatClanManager::GetManager().ShowMemberPage(bShow == FALSE ? false : true);
		ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerShowFriendPage(FALSE);
		EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgShowDlg(FALSE);
		ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
		ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
		ChatLuedManager::GetManager().ShowMemberPage(FALSE);
		ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
	}
	else if (ChatControlPanel::ChatPanelGetPanel().currentPanel == _CHAT_PANEL_LUED_PANEL)
	{
		ChatLuedManager::GetManager().ShowMemberPage(bShow == FALSE ? FALSE : TRUE);
		ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerShowFriendPage(false);
		ChatClanManager::GetManager().ShowMemberPage(false);
		EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgShowDlg(FALSE);
		ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
		ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
		ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
		ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
		ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
	}
}

void ChatMainDlg::MainDlgCloseChannel()
{
	ChatMainDlg::MainDlgShowWndChat(false);
	B2ChatDialog::chatManager.ChatManagerClearInfoWnd();
	for(int i = 0; i < B2ChatDialog::chatManager.chatChannels.size();)
	{
		Ui_Channel_Param& channel = B2ChatDialog::chatManager.chatChannels[i];
		if(strcmp(ChatString::ChatStringGetString().chTeamName,channel.szChannelName) == 0 ||
			strcmp(ChatString::ChatStringGetString().chSizhuName,channel.szChannelName) == 0||
			strcmp(ChatString::ChatStringGetString().chZhuhouName,channel.szChannelName) == 0||
			strcmp(ChatString::ChatStringGetString().chGuojiaName,channel.szChannelName) == 0||
			strcmp(ChatString::ChatStringGetString().chMapName,channel.szChannelName) == 0)
		{
			if(B2ChatDialog::chatManager.ChatManagerChannelClose(channel.dwChannelID))
			{
				i = 0;
				continue;
			}

		}
		i++;
	}
	ChatUiComboBox& uiComboBox = B2ChatDialog::chatManager.ChatManagerGetUiComboBox();
	uiComboBox.currentSelected = NEAR_CHANNALES_TALK;
	uiComboBox.selectItemButton.ChatWndSetText(uiComboBox.downDialgButtons[uiComboBox.currentSelected].ChatWndGetText());
	uiComboBox.selectItemButton.ChatWndSetFont(uiComboBox.downDialgButtons[uiComboBox.currentSelected].ChatWndGetFontName());
	uiComboBox.selectItemButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
	if(uiComboBox.downDialgButtons[uiComboBox.currentSelected].ChatWndGetAttr()&_CHAT_WND_ATTR_USE_DEFUALT_FONT)
		uiComboBox.selectItemButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		const char* colorText = KUiChanMgr::getSinglton().getChanColor(LOCAL_ROOM_ID, false);
		LOColor color(colorText);
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->setColor(color);
}
void ChatMainDlg::MainDlgDestroyAll()
{
	DestroyWindow(B2ChatDialog::chatManager.ChatManagerGetUiComboBox().hDownDialg);
	DestroyWindow(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->hTipItemWnd);
	DestroyWindow(B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->hWndHandle);
	DestroyWindow(B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogGetHandle());
	DestroyWindow(B2ChatDialog::m_hDialog);
	ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerDestroyWindow();
	ChatClanManager::GetManager().ClearAll();
	ChatClanManager::GetManager().DestroyClanPanel();

	DestroyWindow(ChatMainDlg::hMainDlg);
}
void CALLBACK SystemColorChangedTimeProc(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime)
{
	BOOL bHide = false;
	RECT rc;
	GetClientRect(GetDesktopWindow(),&rc);
	RECT extraWndRect;
	GetWindowRect(ChatMainDlg::hMainDlg,&extraWndRect);
	int height = extraWndRect.right - extraWndRect.left;
	if(rc.right - KWin32App::m_uScreenWidth<height)
		bHide = true;
	if(KWin32App::m_bFullScreen||bHide)
	{
		if(IsWindowVisible(ChatMainDlg::hMainDlg))
		{
			ChatMainDlg::MainDlgShowWndChat(FALSE);
			KUiChannelCentre::Show();
		}
	}
}

void ChatMainDlg::ReceiveFriendList()
{
	if(ChatFriendPanelManager::ChatFriendManagerGet().needUpdate == true)
	{
		ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerReceiveFriendList();
	}
}
void ChatMainDlg::UpdateFriendList()
{
//	ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerClearControl();
	ChatFriendPanelManager::ChatFriendManagerGet().needUpdate = true;
}
void ChatMainDlg::AjustMainWindow()
{
	/////////////////////
	RECT rect;
	HWND hDest = GetDesktopWindow();
	GetClientRect(hDest,&rect);
	RECT gameRect;
	RECT chatExtra;
	GetWindowRect(KWin32App::m_hMainWnd,&gameRect);
	GetWindowRect(ChatMainDlg::hMainDlg,&chatExtra);
	int gameWindowWidth = gameRect.right -gameRect.left;
	int gameWindowHeight = gameRect.bottom - gameRect.top;
	int chatExtraWindowWidth = chatExtra.right - chatExtra.left;
	int chatExtraWindowHeight = chatExtra.bottom - chatExtra.top;
	int gameWindowX = (rect.right - gameWindowWidth -chatExtraWindowWidth)/2;
	int gameWindowY = (rect.bottom - gameWindowHeight)/2;

	MoveWindow(KWin32App::m_hMainWnd,gameWindowX,gameWindowY,gameWindowWidth,gameWindowHeight,true);
	MoveWindow(ChatMainDlg::hMainDlg,gameWindowX+gameWindowWidth,gameWindowY,chatExtraWindowWidth,chatExtraWindowHeight,true);
}
void ChatMainDlg::RegisterLocalChannel()
{
	Ui_Channel_Param channel;
	strcpy(channel.szChannelName,ChatString::ChatStringGetString().chPersonalName);
	channel.dwChannelID = COSE_ROOM_ID;
	B2ChatDialog::chatManager.ChatManagerRegistChannel(channel);
	
	channel.dwChannelID = COMBAT_INFO_ROOM_ID;
	sprintf(channel.szChannelName, ChatString::ChatStringGetString().chFightName);
	B2ChatDialog::chatManager.ChatManagerRegistChannel(channel);
}
void ChatMainDlg::InsertSystemMsg(ILayout* pLayout)
{
	B2ChatDialog::chatManager.ChatClientInsertSystemMsg(pLayout);
}
void ChatMainDlg::InsertSystemMsg(const char* pText)
{
	B2ChatDialog::chatManager.ChatClientInsertSystemMsg(pText,false);
}
void ChatMainDlg::CheckFocus()
{
	if(!IsIconic(KWin32App::m_hMainWnd)&&IsWindowVisible(KWin32App::m_hMainWnd))
	{
		KWin32Frame::s_minisized = false;
		POINT pt;
		GetCursorPos(&pt);
		HWND hTemp = WindowFromPoint(pt);
		if(hTemp == KWin32App::m_hMainWnd&&GetFocus()!=KWin32App::m_hMainWnd)
		{
			if(B2ChatDialog::chatManager.ChatManagerGetEditBox()->focus)
			{
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->focus = false;
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditKillCursor();
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->showCarat(false,false);
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
			}
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			SetFocus(hTemp);
		}
		else
		if(hTemp != KWin32App::m_hMainWnd)
		{
			if(GetFocus()!=KWin32App::m_hMainWnd)
				return;
			if(IsWindowVisible(ChatMainDlg::hMainDlg) == false)
				return;
			if(ChatControlPanel::ChatPanelGetPanel().panelButtonList[_CHAT_PANEL_CHAT_PANEL]->ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN)
			{
				ScreenToClient(ChatMainDlg::hMainDlg,&pt);
				RECT rc;
				GetClientRect(ChatMainDlg::hMainDlg,&rc);
				if(IsInRect(pt,rc))
				{
					B2ChatDialog::chatManager.ChatManagerGetEditBox()->focus = true;
					B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(true);
					B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->showCarat(true,false);
					B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
				}
			}
		}
	}
	

}
BOOL CALLBACK ChatMainDlg::MainDlgProc(HWND hwnd,UINT msg,WPARAM lParam,LPARAM wParam)
{
	static   UINT   s_uTaskbarRestart;
	
//	CheckFocus();
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			
			///注册SHELL重建消息////////////////////
			s_uTaskbarRestart   =   RegisterWindowMessage(TEXT("TaskbarCreated")); 
			InitCommonControls();
			ChatString::ChatStringGetString().ChatStringLoadFronINI();
			ChatResource::GetSingle().LoadSrc();
			B2ChatDialog B2Dlg;
			B2Dlg.ChatDialogInit(hwnd);
			ChatControlPanel& panel = ChatControlPanel::ChatPanelGetPanel();
			panel.ChatPanelCreate(hwnd);
			ChatFriendPanelManager& friendManager = ChatFriendPanelManager::ChatFriendManagerGet();
			friendManager.ChatFriendManagerInit(hwnd);
			LookFriendInfoDlg::GetSingle().CreateDlg(hwnd,ChatWndProcessFun::LookFriendInfoDlgProc);
			SetTimer(ChatMainDlg::hMainDlg,_SYSCOLOR_CHANGE_TIME_ID,_SYSCOLOR_CHANGE_TIME,(TIMERPROC)SystemColorChangedTimeProc);
			ChatMainDlg::hCursor = LoadCursorFromFile("data/normal.pak");
			SetCursor(hCursor);
			SetClassLong(hwnd,   GCL_HCURSOR,   (LONG)ChatMainDlg::hCursor);//
			ShowCursor(TRUE);
			EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgInitFromCtfIni(hwnd);
			ChatMainDlg::hIcon = LoadIcon(KWin32App::m_hInstance,MAKEINTRESOURCE(IDI_ICON));
			SendMessage(hwnd,WM_SETICON,true,(LPARAM)ChatMainDlg::hIcon);
//			int width = KUiMiniMap::GetSingleton().GetMiniMapWndWidth();
//			int height = KUiMiniMap::GetSingleton().GetMiniMapWndHeight();
//			int paintX = KUiMiniMap::GetSingleton().GetMiniMapWndPaintX();
//			int paintY = KUiMiniMap::GetSingleton().GetMiniMapWndPaintY();
			ChatMiniMap::GetSingle().Create(hwnd,ChatWndProcessFun::ChatMiniMapDlgProc,0,0);
		//	ChatMiniMap::GetSingle().GetSingle().CreateSurface();
//			ChatMiniMap::GetSingle().SetPaintPos(paintX,paintY);
			SetTimer(ChatMiniMap::GetSingle().GetHandle(),UPDATA_TIMER_ID,UPDATA_DT_TIME,(TIMERPROC)(ChatWndProcessFun::ChatMiniMapUpataTimeProc));
			
			//初始化氏族面板
			ChatClanManager::GetManager().ChatClanManagerInit(hwnd);
			ChatLuedManager::GetManager().ChatLuedManagerInit(hwnd);
			ChatClanAnnouncementDlg::GetDlg().LoadResource(hwnd);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			RECT rc;
			GetClientRect(hwnd,&rc);
			DrawBitmap(hdc,ChatResource::GetSingle().GetResource(ChatMainDlg::mainWndSrcIdx)->hBitmap,rc.right,rc.bottom);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			RECT rc;
			GetClientRect(hwnd,&rc);
			DrawBitmap((HDC)wParam,ChatResource::GetSingle().GetResource(ChatMainDlg::mainWndSrcIdx)->hBitmap,rc.right,rc.bottom);
		}
		return TRUE;
	case WM_ACTIVATEAPP:
		{
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
		}
		return FALSE;
	case WM_DISPLAYCHANGE:
		{
			ChatResource::GetSingle().Reset();
		}
		return FALSE;
	case WM_DESTROY:
		{
			KillTimer(B2ChatDialog::m_hDialog,_SYSCOLOR_CHANGE_TIME_ID);
		//	ChatMainDlg::MainDlgDeleteResource();
			DestroyCursor(ChatMainDlg::hCursor);
			if(ChatMainDlg::hIcon)
				DeleteObject(ChatMainDlg::hIcon);
		}
		return TRUE;

	case WM_SYSCOMMAND:
		{
			if((lParam)==SC_MINIMIZE)
			{
				SendMessage(hwnd,WM_SYSCOMMAND,SC_RESTORE,0);
				SendMessage(hwnd,WM_CLOSE,0,0);
				return TRUE;
			}	
		}
		return false;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		break;
	case WM_SIZE:
		{
			if(lParam == SIZE_MINIMIZED)
			{
				SendMessage(hwnd,WM_CLOSE,0,0);
				return FALSE;
			}
		}
	case WM_MOVING:
		{
			B2ChatDialog::ChatDialogAdjustWindow();
			EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgAdjustWindow();
			ChatControlPanel::ChatPanelGetPanel().ChatPanelAdjustWindow();
			ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendAdjustWindow();
			ChatClanManager::GetManager().m_ClanPanel.ChatFriendAdjustWindow();
			ChatLuedManager::GetManager().m_ClanPanel.ChatFriendAdjustWindow();
			for(int i = 0; i <_PLAYER_INFO_PAGE_NUM; i++ )
			{
				ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[i].PlayerInfoDlgAdjustWnd();
			}
			ChatClanManager::GetManager().m_ClanInfoDlg.PlayerInfoDlgAdjustWnd();
			ChatLuedManager::GetManager().m_LuedInfoDlg.PlayerInfoDlgAdjustWnd();
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgAdjustWindow(0);
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgAdjustWindow(0);
			ChatClanManager::GetManager().addDlg.ChatHintDlgAdjustWindow(0);
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgAdjustWindow(0);
			ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgAdjustWindow(0);
			ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgAdjustWindow(0);
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			ChatClanAnnouncementDlg::GetDlg().AdjustWindow();
			ChatClanComboBox::GetClanComboBox().AdjustDownDlg();
			RECT rc,rc1;
			if(!IsIconic(KWin32App::m_hMainWnd))
			{
				GetWindowRect(hwnd,&rc);
				GetWindowRect(KWin32App::m_hMainWnd,&rc1);
				
				int x = rc.left-(rc1.right-rc1.left);
				int y = rc.top;
				MoveWindow(KWin32App::m_hMainWnd,x,y,rc1.right-rc1.left,rc1.bottom-rc1.top,true);
			}
			ChatMiniMap::GetSingle().AdjustWindow();
			ChatPlayerBaseInfoDlg::GetSingle().AdjustWindow();
		}
		return TRUE;
	case WM_MOVE:
		{
			B2ChatDialog::ChatDialogAdjustWindow();
			EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgAdjustWindow();
			ChatControlPanel::ChatPanelGetPanel().ChatPanelAdjustWindow();
			ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendAdjustWindow();
			ChatClanManager::GetManager().m_ClanPanel.ChatFriendAdjustWindow();
			ChatLuedManager::GetManager().m_ClanPanel.ChatFriendAdjustWindow();
			for(int i = 0; i <_PLAYER_INFO_PAGE_NUM; i++ )
			{
				ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[i].PlayerInfoDlgAdjustWnd();
			}
			ChatClanManager::GetManager().m_ClanInfoDlg.PlayerInfoDlgAdjustWnd();
			ChatLuedManager::GetManager().m_LuedInfoDlg.PlayerInfoDlgAdjustWnd();
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgAdjustWindow(0);
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgAdjustWindow(0);
			ChatClanManager::GetManager().addDlg.ChatHintDlgAdjustWindow(0);
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgAdjustWindow(0);
			ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgAdjustWindow(0);
			ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgAdjustWindow(0);
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			ChatMiniMap::GetSingle().AdjustWindow();
			ChatPlayerBaseInfoDlg::GetSingle().AdjustWindow();
			ChatClanAnnouncementDlg::GetDlg().AdjustWindow();
			ChatClanComboBox::GetClanComboBox().AdjustDownDlg();
			RECT rc,rc1;
			if(!IsIconic(KWin32App::m_hMainWnd))
			{
				GetWindowRect(hwnd,&rc);
				GetWindowRect(KWin32App::m_hMainWnd,&rc1);
				
				int x = rc.left-(rc1.right-rc1.left);
				int y = rc.top;
				MoveWindow(KWin32App::m_hMainWnd,x,y,rc1.right-rc1.left,rc1.bottom-rc1.top,true);
			}

		}
		return FALSE;
	case WM_CLOSE:
		{
			ChatMainDlg::MainDlgShowWndChat(FALSE);
			if(IsWindowVisible(KWin32App::m_hMainWnd))
			{
				KUiChannelCentre::Show();
				KUiMiniMap::Show();
				KUiChannelCentre::GetSingleton().showSystemFrame(true);
			}
		}
		return FALSE;
	case WM_SHOWTASK:
		{
			if(lParam!=IDI_ICON)
				return FALSE;
			switch(wParam)
			{
			case WM_LBUTTONDBLCLK:
				{
					ChatMainDlg::MainDlgShowWndChat(TRUE);
					SetForegroundWindow(ChatMainDlg::hMainDlg);
				}
				return FALSE;
			case WM_LBUTTONDOWN:
				{
				//	if(IsWindowVisible(ChatMainDlg::hMainDlg))
				//	{
				//		BringWindowToTop(ChatMainDlg::hMainDlg);
				//	}
					ChatMainDlg::MainDlgShowWndChat(TRUE);
					SetForegroundWindow(ChatMainDlg::hMainDlg);
				}
				return FALSE;
			}
		}
		return FALSE;
	case WM_SYSCOLORCHANGE:
		{
			BOOL bHide = false;
			RECT rc;
			GetClientRect(GetDesktopWindow(),&rc);
			RECT extraWndRect;
			GetWindowRect(ChatMainDlg::hMainDlg,&extraWndRect);
			int height = extraWndRect.right - extraWndRect.left;
			if(rc.right - KWin32App::m_uScreenWidth<height)
				bHide = true;
			if(KWin32App::m_bFullScreen||bHide)
			{
				if(IsWindowVisible(ChatMainDlg::hMainDlg))
				{
					ChatMainDlg::MainDlgShowWndChat(FALSE);
					KUiChannelCentre::Show();
				}
			}
		}	
		break;
	default:
		if(msg == s_uTaskbarRestart)
			Shell_NotifyIcon(NIM_ADD,&ChatMainDlg::taskInfo);		
	}
	return FALSE;
}

void ChatMainDlg::MainDlgDeleteResource()
{
/*	if(ChatMainDlg::hMainSrcBK)
	{
		DeleteObject(hMainSrcBK);
		hMainSrcBK = 0;
	}*/
}
void ChatMainDlg::MainDlgShow(BOOL isShow)
{
	if(isShow)
	{
		if (!IsWindowVisible(ChatMainDlg::hMainDlg))
		{
			ShowWindow(ChatMainDlg::hMainDlg,SW_NORMAL);
		}
	}
	else
	{
		if(IsWindowVisible(ChatMainDlg::hMainDlg))
			ShowWindow(ChatMainDlg::hMainDlg,SW_HIDE);
	}
}
void ChatMainDlg::MainDlgAdjustWindow()
{

}
