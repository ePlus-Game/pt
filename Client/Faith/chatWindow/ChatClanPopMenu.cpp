#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <zmouse.h>
#include <list>
#include <vector>
#include <string>
using namespace std;
#include "layoutinterface.h"
#include "GameDataDef.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatClanListControl.h"
#include "chatWindow/ChatClanPopMenu.h"

#include "resource.h"
#include "chatWindow/ChatWndProc.h"
#include "CoreShell.h"
#include "Ui/UiCase/UiTeamList.h"
#include "ui/UiCase/UiChatWindow.h"
#include "ui/KMessageCentre.h"

#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatClanPanel.h"
#include "chatWindow/ChatClanTitleControl.h"
#include "chatWindow/ChatClanInfoDlg.h"
#include "chatWindow/ChatClanPlayerData.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "chatWindow/ChatClanManager.h"
#include "SocialComDef.h"
#include "chatWindow/ChatClanComboBox.h"

extern iCoreShell * g_pCoreShell;

ChatClanPopMenu::ChatClanPopMenu()
{
	m_hDlg = 0;
	m_iMenuWidth = 0;
	m_hParent = 0;
}
ChatClanPopMenu::~ChatClanPopMenu()
{

}
BOOL ChatClanPopMenu::CreateMenu(HWND hParent, DLGPROC processFun)
{
	m_hDlg = CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_LOOK_FRIEND_INFO_POP), hParent,
		(DLGPROC)processFun);
	if(m_hDlg == 0)
		return  FALSE;

	m_hParent = hParent;
	LoadIniSrc(m_hDlg);
	return TRUE;
	
}

void  ChatClanPopMenu::LoadIniSrc(HWND hParent)
{

	KIniFile iniFile;

	char  szVale[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		iniFile.Load(_CHAT_CFG_FILE_1024);
	else
		iniFile.Load(_CHAT_CFG_FILE);

	iniFile.GetInteger("lookInfoDlg","width",0,&m_iMenuWidth);
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	char infoText[256] = {0};

	iniFile.GetInteger("addFriendBnt","x",0,&x);
	iniFile.GetInteger("addFriendBnt","y",0,&y);
	iniFile.GetInteger("addFriendBnt","width",0,&width);
	iniFile.GetInteger("addFriendBnt","height",0,&height);
	iniFile.GetString("addFriendBnt","textInfo","",infoText,256);

	m_AddFriend.ChatWndCreate(_CLAN_POP_ADD_FRIEND_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	
	if(infoText[0]!=0)
	{
		m_AddFriend.ChatWndSetText(infoText);
		m_AddFriend.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("addFriendBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_AddFriend.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("addFriendBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc = CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0] = 0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_AddFriend.ChatWndSetFont(font);
		else
		{
			m_AddFriend.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_AddFriend.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	int normal_id = 0;
	int hover_id = 0;
	int disable_id = 0;
	iniFile.GetInteger("addFriendBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("addFriendBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("addFriendBnt","disableStateSrcID",0,&disable_id);
	
	m_AddFriend.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("addFriendBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_AddFriend.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_AddFriend.SetWndProcessFun(ChatWndProcessFun::ClanPopMenuAddFrienBntProc);

	///////////////////////////////////////////////////////////////
	iniFile.GetInteger("inviteTeamBnt","x",0,&x);
	iniFile.GetInteger("inviteTeamBnt","y",0,&y);
	iniFile.GetInteger("inviteTeamBnt","width",0,&width);
	iniFile.GetInteger("inviteTeamBnt","height",0,&height);
	iniFile.GetString("inviteTeamBnt","textInfo","",infoText,256);

	m_InviteTeam.ChatWndCreate(_CLAN_POP_INVITE_TEAM_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	
	if(infoText[0]!=0)
	{
		m_InviteTeam.ChatWndSetText(infoText);
		m_InviteTeam.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("inviteTeamBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_InviteTeam.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("inviteTeamBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_InviteTeam.ChatWndSetFont(font);
		else
		{
			m_InviteTeam.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_InviteTeam.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("inviteTeamBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("inviteTeamBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("inviteTeamBnt","disableStateSrcID",0,&disable_id);
	m_InviteTeam.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("inviteTeamBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_InviteTeam.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_InviteTeam.SetWndProcessFun(ChatWndProcessFun::ClanPopMenuInviteTeamBntProc);
	////////////////////////////////////////////////////////

	iniFile.GetInteger("privateChatBnt","x",0,&x);
	iniFile.GetInteger("privateChatBnt","y",0,&y);
	iniFile.GetInteger("privateChatBnt","width",0,&width);
	iniFile.GetInteger("privateChatBnt","height",0,&height);
	
	m_PrivateChat.ChatWndCreate(_CLAN_POP_PRIVATE_CHAT_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	iniFile.GetString("privateChatBnt","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		m_PrivateChat.ChatWndSetText(infoText);
		m_PrivateChat.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("privateChatBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_PrivateChat.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("privateChatBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_PrivateChat.ChatWndSetFont(font);
		else
		{
			m_PrivateChat.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_PrivateChat.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("privateChatBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("privateChatBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("privateChatBnt","disableStateSrcID",0,&disable_id);
	m_PrivateChat.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("privateChatBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_PrivateChat.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_PrivateChat.SetWndProcessFun(ChatWndProcessFun::ClanPopMenuPrivateChatBntProc);

	/////////////////////////////////////////////////
	////////////////////////////////////
	iniFile.GetInteger("particularInfoBnt","x",0,&x);
	iniFile.GetInteger("particularInfoBnt","y",0,&y);
	iniFile.GetInteger("particularInfoBnt","width",0,&width);
	iniFile.GetInteger("particularInfoBnt","height",0,&height);
	iniFile.GetString("particularInfoBnt","textInfo","",infoText,256);

	m_ParticularInfo.ChatWndCreate(_CLAN_POP_PARTICULAR_INFO_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	if(infoText[0]!=0)
	{
		m_ParticularInfo.ChatWndSetText(infoText);
		m_ParticularInfo.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("particularInfoBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_ParticularInfo.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("particularInfoBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_ParticularInfo.ChatWndSetFont(font);
		else
		{
			m_ParticularInfo.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_ParticularInfo.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("particularInfoBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("particularInfoBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("particularInfoBnt","disableStateSrcID",0,&disable_id);
	m_ParticularInfo.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("particularInfoBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_ParticularInfo.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_ParticularInfo.SetWndProcessFun(ChatWndProcessFun::ClanPopMenuParticularInfoBntProc);

	//////////////////////////////////////////////////////
	iniFile.GetInteger("demiseBnt","x",0,&x);
	iniFile.GetInteger("demiseBnt","y",0,&y);
	iniFile.GetInteger("demiseBnt","width",0,&width);
	iniFile.GetInteger("demiseBnt","height",0,&height);
	iniFile.GetString("demiseBnt","textInfo","",infoText,256);

	m_Demise.ChatWndCreate(_CLAN_POP_DEMISE_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	
	if(infoText[0]!=0)
	{
		m_Demise.ChatWndSetText(infoText);
		m_Demise.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("demiseBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_Demise.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("demiseBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_Demise.ChatWndSetFont(font);
		else
		{
			m_Demise.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_Demise.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("demiseBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("demiseBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("demiseBnt","disableStateSrcID",0,&disable_id);
	m_Demise.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("demiseBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_Demise.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_Demise.SetWndProcessFun(ChatWndProcessFun::ClanPopMenuDemiseBntProc);

	////////////////////////////////////////////////////
	iniFile.GetInteger("fireBnt","x",0,&x);
	iniFile.GetInteger("fireBnt","y",0,&y);
	iniFile.GetInteger("fireBnt","width",0,&width);
	iniFile.GetInteger("fireBnt","height",0,&height);
	iniFile.GetString("fireBnt","textInfo","",infoText,256);

	m_Fire.ChatWndCreate(_CLAN_POP_FIRE_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	
	if(infoText[0]!=0)
	{
		m_Fire.ChatWndSetText(infoText);
		m_Fire.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("fireBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_Fire.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("fireBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_Fire.ChatWndSetFont(font);
		else
		{
			m_Fire.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_Fire.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("fireBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("fireBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("fireBnt","disableStateSrcID",0,&disable_id);
	m_Fire.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("fireBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_Fire.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_Fire.SetWndProcessFun(ChatWndProcessFun::ClanPopMenuFireBntProc);

}

ChatClanPopMenu & ChatClanPopMenu::GetMenu()
{
	static ChatClanPopMenu popMenu;
	return popMenu;
}

void ChatClanPopMenu::AddFrienButtonDown()
{
if(pPlayerControl == 0||
		m_AddFriend.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
		return;
	g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)(pPlayerControl ->m_playerData.szName), 1 );
	HideMenu();
}

void ChatClanPopMenu::InviteTeamButtonDown()
{
	if(pPlayerControl == 0||
		m_InviteTeam.ChatWndGetState()==_CHAT_BUTTON_STATE_DISABLE)
	{
		return;
	}
	KUiPlayerItem tagPlayer;
	strcpy(tagPlayer.Name,(char*)pPlayerControl ->m_playerData.szName);
	tagPlayer.nData = 0;
	tagPlayer.nIndex = 0;
	tagPlayer.nParam = 0;
	
	KUiPlayerTeam	TeamInfo;
	TeamInfo.cNumMember = 0;
	g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
	if ( ((int)TeamInfo.cNumMember) <= MAX_TEAMMEMBER_COUNT )
	{
		if (TeamInfo.cNumMember == 0)
		{
			g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
		}
		
		g_pCoreShell->TeamOperation( TEAM_OI_INVITE_BY_NAME, (unsigned int)&tagPlayer, NULL );
	}
	else
	{
		char *msg = KMessageCentre::GetMessage(team_message, 1);
		KUiChannelCentre::GetSingleton().toSysMsg(msg);
	}
	HideMenu();
}

void ChatClanPopMenu::PrivateChatButtonDown()
{
	if(pPlayerControl == 0||
		m_PrivateChat.ChatWndGetState()==_CHAT_BUTTON_STATE_DISABLE)
		return;
	pPlayerControl->OnLButtonDBLCLK();
	HideMenu();
}

void ChatClanPopMenu::ParticularInfoButtonDown()
{
	if(pPlayerControl == 0||
		m_ParticularInfo.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
		return;
	g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)(pPlayerControl ->m_playerData.szName), 1 );
	HideMenu();
}

void ChatClanPopMenu::FireButtonDown()
{
	ChatClanPlayerData::GetPlayerData().DeleteMember(pPlayerControl ->m_playerData.guid, ChatClanManager::GetManager().m_ClanInfoDlg.hDlg, enSULayer_Gens);
	ChatClanPlayerData::GetPlayerData().RequestDataList(ChatClanManager::GetManager().m_ClanInfoDlg.hDlg, enSULayer_Gens);
	HideMenu();
}

void ChatClanPopMenu::DemiseButtonDown()
{
	if (pPlayerControl == NULL ||
		m_Demise.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
		return;
	m_Demise.ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
	ChatClanPlayerData::GetPlayerData().Demise(pPlayerControl ->m_playerData.guid, enSULayer_Gens);
	HideMenu();
}

void ChatClanPopMenu::AdjustWindow()
{
	POINT pt;
	GetCursorPos(&pt);
	int height = m_AddFriend.ChatWndGetRect().bottom +
		m_InviteTeam.ChatWndGetRect().bottom +
		m_PrivateChat.ChatWndGetRect().bottom +
		m_ParticularInfo.ChatWndGetRect().bottom + 
		m_Demise.ChatWndGetRect().bottom +
		m_Fire.ChatWndGetRect().bottom;
	MoveWindow(m_hDlg, pt.x, pt.y, m_iMenuWidth, height, true);
}

void ChatClanPopMenu::ShowMenu()
{
	SetWindowPos(m_hDlg,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE);
	ShowWindow(m_hDlg,SW_NORMAL);
	SetFocus(m_hDlg);
}

void ChatClanPopMenu::HideMenu()
{
	ShowWindow(m_hDlg,SW_HIDE);
}

void ChatClanPopMenu::ProcessKillFocus()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hDlg,&pt);
	RECT rc;
	GetClientRect(m_hDlg,&rc);
	if(!IsInRect(pt,rc))
	{
		HideMenu();
	}
}

ChatLuedPopMenu::ChatLuedPopMenu()
{
	m_hDlg = 0;
	m_iMenuWidth = 0;
}
ChatLuedPopMenu::~ChatLuedPopMenu()
{

}
BOOL ChatLuedPopMenu::CreateMenu(HWND hParent, DLGPROC processFun)
{
	m_hDlg = CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_LOOK_FRIEND_INFO_POP), hParent,
		(DLGPROC)processFun);
	if(m_hDlg == 0)
		return  FALSE;

	m_hParent = hParent;
	LoadIniSrc(m_hDlg);
	return TRUE;
	
}

void  ChatLuedPopMenu::LoadIniSrc(HWND hParent)
{

	KIniFile iniFile;

	char  szVale[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		iniFile.Load(_CHAT_CFG_FILE_1024);
	else
		iniFile.Load(_CHAT_CFG_FILE);

	iniFile.GetInteger("lookInfoDlg","width",0,&m_iMenuWidth);
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	char infoText[256] = {0};

	iniFile.GetInteger("addFriendBnt","x",0,&x);
	iniFile.GetInteger("addFriendBnt","y",0,&y);
	iniFile.GetInteger("addFriendBnt","width",0,&width);
	iniFile.GetInteger("addFriendBnt","height",0,&height);
	iniFile.GetString("addFriendBnt","textInfo","",infoText,256);

	m_AddFriend.ChatWndCreate(_LUED_POP_ADD_FRIEND_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	
	if(infoText[0]!=0)
	{
		m_AddFriend.ChatWndSetText(infoText);
		m_AddFriend.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("addFriendBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_AddFriend.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("addFriendBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc = CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0] = 0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_AddFriend.ChatWndSetFont(font);
		else
		{
			m_AddFriend.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_AddFriend.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	int normal_id = 0;
	int hover_id = 0;
	int disable_id = 0;
	iniFile.GetInteger("addFriendBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("addFriendBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("addFriendBnt","disableStateSrcID",0,&disable_id);
	
	m_AddFriend.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("addFriendBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_AddFriend.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_AddFriend.SetWndProcessFun(ChatWndProcessFun::LuedPopMenuAddFrienBntProc);

	///////////////////////////////////////////////////////////////
	iniFile.GetInteger("inviteTeamBnt","x",0,&x);
	iniFile.GetInteger("inviteTeamBnt","y",0,&y);
	iniFile.GetInteger("inviteTeamBnt","width",0,&width);
	iniFile.GetInteger("inviteTeamBnt","height",0,&height);
	iniFile.GetString("inviteTeamBnt","textInfo","",infoText,256);

	m_InviteTeam.ChatWndCreate(_LUED_POP_INVITE_TEAM_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	
	if(infoText[0]!=0)
	{
		m_InviteTeam.ChatWndSetText(infoText);
		m_InviteTeam.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("inviteTeamBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_InviteTeam.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("inviteTeamBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_InviteTeam.ChatWndSetFont(font);
		else
		{
			m_InviteTeam.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_InviteTeam.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("inviteTeamBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("inviteTeamBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("inviteTeamBnt","disableStateSrcID",0,&disable_id);
	m_InviteTeam.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("inviteTeamBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_InviteTeam.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_InviteTeam.SetWndProcessFun(ChatWndProcessFun::LuedPopMenuInviteTeamBntProc);
	////////////////////////////////////////////////////////

	iniFile.GetInteger("privateChatBnt","x",0,&x);
	iniFile.GetInteger("privateChatBnt","y",0,&y);
	iniFile.GetInteger("privateChatBnt","width",0,&width);
	iniFile.GetInteger("privateChatBnt","height",0,&height);
	
	m_PrivateChat.ChatWndCreate(_LUED_POP_PRIVATE_CHAT_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	iniFile.GetString("privateChatBnt","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		m_PrivateChat.ChatWndSetText(infoText);
		m_PrivateChat.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("privateChatBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_PrivateChat.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("privateChatBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_PrivateChat.ChatWndSetFont(font);
		else
		{
			m_PrivateChat.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_PrivateChat.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("privateChatBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("privateChatBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("privateChatBnt","disableStateSrcID",0,&disable_id);
	m_PrivateChat.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("privateChatBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_PrivateChat.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_PrivateChat.SetWndProcessFun(ChatWndProcessFun::LuedPopMenuPrivateChatBntProc);

	/////////////////////////////////////////////////
	////////////////////////////////////
	iniFile.GetInteger("particularInfoBnt","x",0,&x);
	iniFile.GetInteger("particularInfoBnt","y",0,&y);
	iniFile.GetInteger("particularInfoBnt","width",0,&width);
	iniFile.GetInteger("particularInfoBnt","height",0,&height);
	iniFile.GetString("particularInfoBnt","textInfo","",infoText,256);

	m_ParticularInfo.ChatWndCreate(_LUED_POP_PATRICULAR_INFO_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	if(infoText[0]!=0)
	{
		m_ParticularInfo.ChatWndSetText(infoText);
		m_ParticularInfo.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("particularInfoBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_ParticularInfo.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("particularInfoBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_ParticularInfo.ChatWndSetFont(font);
		else
		{
			m_ParticularInfo.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_ParticularInfo.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("particularInfoBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("particularInfoBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("particularInfoBnt","disableStateSrcID",0,&disable_id);
	m_ParticularInfo.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("particularInfoBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_ParticularInfo.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_ParticularInfo.SetWndProcessFun(ChatWndProcessFun::LuedPopMenuParticularInfoBntProc);

	//////////////////////////////////////////////////////
	iniFile.GetInteger("demiseBnt","x",0,&x);
	iniFile.GetInteger("demiseBnt","y",0,&y);
	iniFile.GetInteger("demiseBnt","width",0,&width);
	iniFile.GetInteger("demiseBnt","height",0,&height);
	iniFile.GetString("demiseBnt","textInfo","",infoText,256);

	m_Demise.ChatWndCreate(_LUED_POP_DEMISE_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	
	if(infoText[0]!=0)
	{
		m_Demise.ChatWndSetText(infoText);
		m_Demise.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("demiseBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_Demise.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("demiseBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_Demise.ChatWndSetFont(font);
		else
		{
			m_Demise.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_Demise.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("demiseBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("demiseBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("demiseBnt","disableStateSrcID",0,&disable_id);
	m_Demise.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("demiseBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_Demise.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_Demise.SetWndProcessFun(ChatWndProcessFun::LuedPopMenuDemiseBntProc);

	////////////////////////////////////////////////////
	iniFile.GetInteger("forbitChatBnt","x",0,&x);
	iniFile.GetInteger("forbitChatBnt","y",0,&y);
	iniFile.GetInteger("forbitChatBnt","width",0,&width);
	iniFile.GetInteger("forbitChatBnt","height",0,&height);
	iniFile.GetString("forbitChatBnt","textInfo","",infoText,256);

	m_ForbidChat.ChatWndCreate(_LUED_POP_FORBID_CHAT_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	
	if(infoText[0]!=0)
	{
		m_ForbidChat.ChatWndSetText(infoText);
		m_ForbidChat.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("forbitChatBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_ForbidChat.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("fireBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_ForbidChat.ChatWndSetFont(font);
		else
		{
			m_ForbidChat.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_ForbidChat.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("forbitChatBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("forbitChatBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("forbitChatBnt","disableStateSrcID",0,&disable_id);
	m_ForbidChat.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("forbitChatBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_ForbidChat.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_ForbidChat.SetWndProcessFun(ChatWndProcessFun::LuedPopMenuForbidBntProc);

	///////////////////////////////////////////////////////////////
	iniFile.GetInteger("unforbitChatBnt","x",0,&x);
	iniFile.GetInteger("unforbitChatBnt","y",0,&y);
	iniFile.GetInteger("unforbitChatBnt","width",0,&width);
	iniFile.GetInteger("unforbitChatBnt","height",0,&height);
	iniFile.GetString("unforbitChatBnt","textInfo","",infoText,256);

	m_UnforbidChat.ChatWndCreate(_LUED_POP_FORBID_CHAT_BNT_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	
	if(infoText[0]!=0)
	{
		m_UnforbidChat.ChatWndSetText(infoText);
		m_UnforbidChat.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("unforbitChatBnt","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		m_UnforbidChat.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("unforbitChatBnt","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_UnforbidChat.ChatWndSetFont(font);
		else
		{
			m_UnforbidChat.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_UnforbidChat.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("unforbitChatBnt","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("unforbitChatBnt","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("unforbitChatBnt","disableStateSrcID",0,&disable_id);
	m_UnforbidChat.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("unforbitChatBnt","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		m_UnforbidChat.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	m_UnforbidChat.SetWndProcessFun(ChatWndProcessFun::LuedPopMenuUnforbidBntProc);	
}

ChatLuedPopMenu & ChatLuedPopMenu::GetMenu()
{
	static ChatLuedPopMenu popMenu;
	return popMenu;
}

void ChatLuedPopMenu::AddFrienButtonDown()
{
	if(pPlayerControl == 0||
		m_AddFriend.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
		return;
	g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)(pPlayerControl ->m_playerData.szName), 1 );
	HideMenu();
}

void ChatLuedPopMenu::InviteTeamButtonDown()
{
	if(pPlayerControl == 0||
		m_InviteTeam.ChatWndGetState()==_CHAT_BUTTON_STATE_DISABLE)
	{
		return;
	}
	KUiPlayerItem tagPlayer;
	strcpy(tagPlayer.Name,(char*)pPlayerControl ->m_playerData.szName);
	tagPlayer.nData = 0;
	tagPlayer.nIndex = 0;
	tagPlayer.nParam = 0;
	
	KUiPlayerTeam	TeamInfo;
	TeamInfo.cNumMember = 0;
	g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
	if ( ((int)TeamInfo.cNumMember) <= MAX_TEAMMEMBER_COUNT )
	{
		if (TeamInfo.cNumMember == 0)
		{
			g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
		}
		
		g_pCoreShell->TeamOperation( TEAM_OI_INVITE_BY_NAME, (unsigned int)&tagPlayer, NULL );
	}
	else
	{
		char *msg = KMessageCentre::GetMessage(team_message, 1);
		KUiChannelCentre::GetSingleton().toSysMsg(msg);
	}
	HideMenu();
}

void ChatLuedPopMenu::PrivateChatButtonDown()
{
	if(pPlayerControl == 0||
		m_PrivateChat.ChatWndGetState()==_CHAT_BUTTON_STATE_DISABLE)
		return;
	pPlayerControl->OnLButtonDBLCLK();
	HideMenu();
}

void ChatLuedPopMenu::ParticularInfoButtonDown()
{
	if(pPlayerControl == 0||
		m_ParticularInfo.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
		return;
	g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)(pPlayerControl ->m_playerData.szName), 1 );
	HideMenu();
}

void ChatLuedPopMenu::ForbidChatButtonDown()
{
	ChatClanPlayerData::GetPlayerData().ForbidChat(pPlayerControl ->m_playerData.guid, enSULayer_Tong);
	FSGUID * guid = NULL;
	if (ChatClanComboBox::GetClanComboBox().GetSelectedItem() != NULL)
	{
		guid = &ChatClanComboBox::GetClanComboBox().GetSelectedItem() ->m_ClanGuid;
		return;
	}
	ChatClanPlayerData::GetPlayerData().RequestDataList(m_hParent, enSULayer_Gens, guid);
	HideMenu();
}

void ChatLuedPopMenu::DemiseButtonDown()
{
	if (pPlayerControl == NULL ||
		m_Demise.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
		return;
	m_Demise.ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
	ChatClanPlayerData::GetPlayerData().Demise(pPlayerControl ->m_playerData.guid, enSULayer_Tong);
	HideMenu();
}

void ChatLuedPopMenu::UnforbidChatButtonDown()
{
	ChatClanPlayerData::GetPlayerData().UnforbidChat(pPlayerControl ->m_playerData.guid, enSULayer_Tong);
	FSGUID * guid = NULL;
	if (ChatClanComboBox::GetClanComboBox().GetSelectedItem() != NULL)
	{
		guid = &ChatClanComboBox::GetClanComboBox().GetSelectedItem() ->m_ClanGuid;
		return;
	}
	ChatClanPlayerData::GetPlayerData().RequestDataList(m_hParent, enSULayer_Gens, guid);
	HideMenu();
}

void ChatLuedPopMenu::AdjustWindow()
{
	POINT pt;
	GetCursorPos(&pt);
	int height = m_AddFriend.ChatWndGetRect().bottom +
		m_InviteTeam.ChatWndGetRect().bottom +
		m_PrivateChat.ChatWndGetRect().bottom +
		m_ParticularInfo.ChatWndGetRect().bottom + 
		m_Demise.ChatWndGetRect().bottom +
		m_ForbidChat.ChatWndGetRect().bottom;
	MoveWindow(m_hDlg, pt.x, pt.y, m_iMenuWidth, height, true);
}

void ChatLuedPopMenu::ShowMenu()
{
	SetWindowPos(m_hDlg,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE);
	ShowWindow(m_hDlg,SW_NORMAL);
	SetFocus(m_hDlg);
}

void ChatLuedPopMenu::HideMenu()
{
	ShowWindow(m_hDlg,SW_HIDE);
}

void ChatLuedPopMenu::ProcessKillFocus()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hDlg,&pt);
	RECT rc;
	GetClientRect(m_hDlg,&rc);
	if(!IsInRect(pt,rc))
	{
		HideMenu();
	}
}