#include <windows.h>
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
#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatFriendPanelManager.h"

#include "chatWindow/ChatClanTitleControl.h"
#include "chatWindow/ChatClanListControl.h"
#include "chatWindow/ChatClanPanel.h"
#include "chatWindow/ChatClanInfoDlg.h"
#include "chatWindow/ChatClanPlayerData.h"
#include "chatWindow/ChatClanManager.h"

#include "chatWindow/ChatPage.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatControlPanel.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "chatWindow/ChatWndProc.h"
#include "chatWindow/EntrustComputerDlg.h"
#include "chatWindow/ChatClanComboBox.h"

ChatClanManager::ChatClanManager()
: m_iOnlineMemberNum(0)
, m_iAllMemberNum(0)
{

}

ChatClanManager::~ChatClanManager()
{

}

ChatClanManager & ChatClanManager::GetManager()
{
	static ChatClanManager m_manager;
	return m_manager;
}

void ChatClanManager::ChatClanManagerInit(HWND hwnd)
{
	KIniFile  iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);

	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_CHAT_SRC_WIDTH,0,&controlWidth);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_CHAT_SRC_HEIGHT,0,&controlHeight);


	char font[FONT_SIZE]={0};
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_LIST_INI_SRC_FONT,"",font,FONT_SIZE);
	TFONT tFont;
	strcpy(tFont.fontName,font);
	HDC hdc =CreateCompatibleDC(NULL);
	LOGFONT logFont;
	logFont.lfFaceName[0]=0;
	logFont.lfCharSet = DEFAULT_CHARSET;
	EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
	DeleteDC(hdc);
	if(tFont.isInSystem)
	{
		isUseDefaultFont = false;
		strcpy(pszFont,font);
	}
	else
	{
		isUseDefaultFont = true;
	    strcpy(pszFont,ChatString::ChatStringGetString().chatDefualtFont);
	}

	int r,g,b;
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"onlineFontColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	onlineColor = RGB(r,g,b);
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"onlineFontHoverColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	onlineMouseOverColor = RGB(r,g,b);
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"onlineFontSelectColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	onlineSelectColor = RGB(r,g,b);

	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"leftlinefontColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	leftlineColor = RGB(r,g,b);
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"leftlinefontSelect","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	leftlineSelectColor = RGB(r,g,b);

	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_ITEM_CONTROL_INI_NAME_WIDTH,0,&nameItemWidth);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_ITEM_CONTROL_INI_LEVEL_WIDTH,0,&levelItemWidth);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_ITEM_CONTROL_INI_METIER_WIDTH,0,&metierItemWidth);

	m_ClanInfoDlg.CreateClanInfoDlg(hwnd, ChatWndProcessFun::ProcessClanProc);
	m_ClanInfoDlg.LoadSRC(m_ClanInfoDlg.hDlg);
	
	addDlg.ChatHintDlgCreate(B2ChatDialog::m_hDialog, ChatWndProcessFun::AddMemberDlgProc);
	addDlg.LoadSource_Clan();
	deleteDlg.ChatHintDlgCreate(B2ChatDialog::m_hDialog, ChatWndProcessFun::DeleteMemberDlgProc);
	deleteDlg.LoadSource_Clan();

	m_ClanPanel.CreatePanel(m_ClanInfoDlg.hDlg);
}

void ChatClanManager::ShowMemberPage(bool isShow)
{	
	if(isShow)
	{
		ChatClanManager::GetManager().m_ClanPanel.ChatFirendShow(TRUE);
		ChatClanManager::GetManager().m_ClanInfoDlg.PlayerInfoDlgShowDlg(TRUE);
		ChatControlPanel::ChatPanelGetPanel().ChatPanelShow(TRUE);

		B2ChatDialog::ChatDialogShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
	}
	else
	{
		ChatClanManager::GetManager().m_ClanPanel.ChatFirendShow(FALSE);
		ChatClanManager::GetManager().m_ClanInfoDlg.PlayerInfoDlgShowDlg(FALSE);
	}
}

void ChatClanManager::HideClanDlg()
{
	m_ClanInfoDlg.PlayerInfoDlgShowDlg(FALSE);
}

void ChatClanManager::DestroyClanPanel()
{
	DestroyWindow(m_ClanInfoDlg.hDlg);
	DestroyWindow(ChatControlPanel::ChatPanelGetPanel().hPanelDlg);
	DestroyWindow(m_ClanPanel.CHatFriendGetHandle());
}

void ChatClanManager::UpdateMemberList()
{
	TongPageData * playerData = ChatClanPlayerData::GetPlayerData().m_DataList;

	ChatClanListControl * pListControl = NULL;

	m_ClanInfoDlg.ClearAll();
	for (int i = 0; i < ChatClanPlayerData::GetPlayerData().m_iPlayerNum; i++)
	{
		pListControl = new ChatClanListControl;
		if (pListControl != NULL)
		{
			pListControl ->PlayerControlCreate(0, 0, controlWidth, controlHeight, 0, 0);
			pListControl ->PlayerControlSetFont(pszFont);
			pListControl ->PlayerControlSetArePartWidth(&nameItemWidth, &metierItemWidth, &levelItemWidth);
			pListControl ->PlayerControlSetOnlineFontColor(onlineColor);
			pListControl ->PlayerControlSetOnlineFontSelectColor(onlineSelectColor);
			pListControl ->PlayerControlSetOnlineFontMouseOverColor(onlineMouseOverColor);
			pListControl ->PlayerControlSetLeftlineFontSelectColor(leftlineSelectColor);
			pListControl ->PlayerControlSetLeftlineFontColor(leftlineColor);
			pListControl ->PlayerControlSetParent(m_ClanInfoDlg.hDlg);
			if (playerData[i].bOnline)
			{
				pListControl ->PlayerControlSetState(_PLAYER_CONTROL_STATE_NORMAL);
			}
			else
			{
				pListControl ->PlayerControlSetState(_PLAYER_CONTROL_STATE_LEFTLINE);
			}

			memcpy(&pListControl ->m_playerData, &playerData[i], sizeof(TongPageData));
			m_ClanInfoDlg.UpdateMemberList(pListControl);
		}
	}
}

void ChatClanManager::ClearAll()
{
	m_ClanInfoDlg.ClearAll();
}

void ChatClanManager::AddMemberButtonDown()
{
	ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
	if(ChatClanManager::GetManager().addDlg.ChatHintDlgIsShow())
	{
		BringWindowToTop(ChatClanManager::GetManager().addDlg.hDlg);
		return;
	}
	ChatClanManager::GetManager().addDlg.ChatHintDlgShow(TRUE);
}

void ChatClanManager::DeleteMemberButtonDown()
{
	ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
	if (ChatClanManager::GetManager().deleteDlg.ChatHintDlgIsShow())
	{
		BringWindowToTop(ChatClanManager::GetManager().deleteDlg.hDlg);
		return;
	}

	for (int i = 0; i < m_ClanInfoDlg.m_iOnlineMemberNum; i++)
	{
		if (m_ClanInfoDlg.m_OnlineMemberList[i] ->PlayerControlIsSelected())
		{
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(TRUE);
			break;
		}
	}
	
}

ChatLuedManager::ChatLuedManager()
: m_iOnlineMemberNum(0)
, m_iAllMemberNum(0)
{

}

ChatLuedManager::~ChatLuedManager()
{

}

ChatLuedManager & ChatLuedManager::GetManager()
{
	static ChatLuedManager m_manager;
	return m_manager;
}

void ChatLuedManager::ChatLuedManagerInit(HWND hwnd)
{
	KIniFile  iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);

	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_CHAT_SRC_WIDTH,0,&controlWidth);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_CHAT_SRC_HEIGHT,0,&controlHeight);


	char font[FONT_SIZE]={0};
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_LIST_INI_SRC_FONT,"",font,FONT_SIZE);
	TFONT tFont;
	strcpy(tFont.fontName,font);
	HDC hdc =CreateCompatibleDC(NULL);
	LOGFONT logFont;
	logFont.lfFaceName[0]=0;
	logFont.lfCharSet = DEFAULT_CHARSET;
	EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
	DeleteDC(hdc);
	if(tFont.isInSystem)
	{
		isUseDefaultFont = false;
		strcpy(pszFont,font);
	}
	else
	{
		isUseDefaultFont = true;
	    strcpy(pszFont,ChatString::ChatStringGetString().chatDefualtFont);
	}

	int r,g,b;
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"onlineFontColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	onlineColor = RGB(r,g,b);
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"onlineFontHoverColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	onlineMouseOverColor = RGB(r,g,b);
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"onlineFontSelectColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	onlineSelectColor = RGB(r,g,b);

	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"leftlinefontColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	leftlineColor = RGB(r,g,b);
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"leftlinefontSelect","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	leftlineSelectColor = RGB(r,g,b);

	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_ITEM_CONTROL_INI_NAME_WIDTH,0,&nameItemWidth);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_ITEM_CONTROL_INI_LEVEL_WIDTH,0,&levelItemWidth);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_ITEM_CONTROL_INI_METIER_WIDTH,0,&metierItemWidth);


	m_LuedInfoDlg.CreateClanInfoDlg(hwnd, ChatWndProcessFun::ProcessLuedProc);
	m_LuedInfoDlg.LoadSRC(m_LuedInfoDlg.hDlg);
	
	m_AddClanDlg.ChatHintDlgCreate(B2ChatDialog::m_hDialog, ChatWndProcessFun::AddClanDlgProc);
	m_AddClanDlg.LoadSource_Lued();
	m_DeleteClanDlg.ChatHintDlgCreate(B2ChatDialog::m_hDialog, ChatWndProcessFun::DeleteClanDlgProc);
	m_DeleteClanDlg.LoadSource_Lued();

	m_ClanPanel.CreatePanel(m_LuedInfoDlg.hDlg);
}

void ChatLuedManager::ShowMemberPage(bool isShow)
{	
	if(isShow)
	{
		ChatLuedManager::GetManager().m_ClanPanel.ChatFirendShow(TRUE);
		ChatLuedManager::GetManager().m_LuedInfoDlg.PlayerInfoDlgShowDlg(TRUE);
		ChatControlPanel::ChatPanelGetPanel().ChatPanelShow(TRUE);

		B2ChatDialog::ChatDialogShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
	}
	else
	{
		ChatLuedManager::GetManager().m_LuedInfoDlg.PlayerInfoDlgShowDlg(FALSE);
		ChatLuedManager::GetManager().m_ClanPanel.ChatFirendShow(FALSE);
	}
}

void ChatLuedManager::HideLuedDlg()
{
	m_LuedInfoDlg.PlayerInfoDlgShowDlg(FALSE);
}

void ChatLuedManager::DestroyClanPanel()
{
	DestroyWindow(m_LuedInfoDlg.hDlg);
}

void ChatLuedManager::UpdateMemberList()
{
	TongPageData * playerData = ChatClanPlayerData::GetPlayerData().m_DataList;

	ChatClanListControl * pListControl = NULL;

	m_LuedInfoDlg.ClearAll();
	for (int i = 0; i < ChatClanPlayerData::GetPlayerData().m_iPlayerNum; i++)
	{
		pListControl = new ChatClanListControl;
		if (pListControl != NULL)
		{
			pListControl ->PlayerControlCreate(0, 0, controlWidth, controlHeight, 0, 0);
			pListControl ->PlayerControlSetFont(pszFont);
			pListControl ->PlayerControlSetArePartWidth(&nameItemWidth, &metierItemWidth, &levelItemWidth);
			pListControl ->PlayerControlSetOnlineFontColor(onlineColor);
			pListControl ->PlayerControlSetOnlineFontSelectColor(onlineSelectColor);
			pListControl ->PlayerControlSetOnlineFontMouseOverColor(onlineMouseOverColor);
			pListControl ->PlayerControlSetLeftlineFontSelectColor(leftlineSelectColor);
			pListControl ->PlayerControlSetLeftlineFontColor(leftlineColor);
			pListControl ->PlayerControlSetParent(m_LuedInfoDlg.hDlg);
			if (playerData[i].bOnline)
			{
				pListControl ->PlayerControlSetState(_PLAYER_CONTROL_STATE_NORMAL);
			}
			else
			{
				pListControl ->PlayerControlSetState(_PLAYER_CONTROL_STATE_LEFTLINE);
			}

			memcpy(&pListControl ->m_playerData, &playerData[i], sizeof(TongPageData));
			m_LuedInfoDlg.UpdateMemberList(pListControl);
		}
	}
}

void ChatLuedManager::ClearAll()
{
	m_LuedInfoDlg.ClearAll();
}

void ChatLuedManager::AddClanButtonDown()
{
	ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgShow(FALSE);
	if(ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgIsShow())
	{
		BringWindowToTop(ChatLuedManager::GetManager().m_AddClanDlg.hDlg);
		return;
	}
	ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgShow(TRUE);
}

void ChatLuedManager::DeleteClanButtonDown()
{
	ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgShow(FALSE);
	if (ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgIsShow())
	{
		BringWindowToTop(ChatLuedManager::GetManager().m_DeleteClanDlg.hDlg);
		return;
	}

	LPCHATCLANINFO * pInfoList = ChatClanComboBox::GetClanComboBox().m_ClanList;
	for (int i = 0; i < ChatClanComboBox::GetClanComboBox().m_iClanNum; i++)
	{
		if (pInfoList[i] ->m_blIsSelected)
		{
			ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgShow(TRUE);
		}
	}
}