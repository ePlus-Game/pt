#include "KWin32App.h"

#include <commctrl.h>

#include <vector>
using std::vector;
#include "layoutinterface.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/LookFriendInfoPopDlg.h"

#include "resource.h"
#include "KIniFile.h"
#include "chatWindow/ChatResource.h"

#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/chatWnd.h"
#include "CoreShell.h"
#include "Ui/UiCase/UiTeamList.h"
#include "ui/UiCase/UiChatWindow.h"
#include "ui/KMessageCentre.h"
#include "chatWindow/ChatWndProc.h"

extern iCoreShell*		g_pCoreShell;

LookFriendInfoDlg::LookFriendInfoDlg()
{
	hDlg = 0;
	width = 0;
}
LookFriendInfoDlg::~LookFriendInfoDlg()
{

}
BOOL LookFriendInfoDlg::CreateDlg(HWND hParent,DlgProcessFun processFun)
{
	hDlg = CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_LOOK_FRIEND_INFO_POP),hParent,
		(DLGPROC)processFun);
	if(hDlg == 0)
		return  FALSE;

	LoadIniSrc(hDlg);
	return TRUE;
	
}
void  LookFriendInfoDlg::LoadIniSrc(HWND hParent)
{

	KIniFile iniFile;

//	char  szFileName[] = CHAT_INI_FILE_NAME;
	char  szVale[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		iniFile.Load(_CHAT_CFG_FILE_1024);
	else
		iniFile.Load(_CHAT_CFG_FILE);

	iniFile.GetInteger("lookInfoDlg","width",0,&width);
	int x = 0,y=0,width=0,height = 0;
	int r = 0,g = 0,b=0;
	iniFile.GetInteger("talkPersonalBtn","x",0,&x);
	iniFile.GetInteger("talkPersonalBtn","y",0,&y);
	iniFile.GetInteger("talkPersonalBtn","width",0,&width);
	iniFile.GetInteger("talkPersonalBtn","height",0,&height);
	talkPersonalBtn.ChatWndCreate(TALK_PERSONAL_BTN_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	char infoText[256] = {0};
	iniFile.GetString("talkPersonalBtn","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		talkPersonalBtn.ChatWndSetText(infoText);
		talkPersonalBtn.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("talkPersonalBtn","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		talkPersonalBtn.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("talkPersonalBtn","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			talkPersonalBtn.ChatWndSetFont(font);
		else
		{
			talkPersonalBtn.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			talkPersonalBtn.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	int normal_id = 0,hover_id=0,disable_id=0;
	iniFile.GetInteger("talkPersonalBtn","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("talkPersonalBtn","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("talkPersonalBtn","disableStateSrcID",0,&disable_id);
	
	talkPersonalBtn.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("talkPersonalBtn","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		talkPersonalBtn.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	talkPersonalBtn.SetWndProcessFun(ChatWndProcessFun::ProcessLookInfoPersonalButton);

	///////////////////////////////////////////////////////////////
	iniFile.GetInteger("makeTeamBtn","x",0,&x);
	iniFile.GetInteger("makeTeamBtn","y",0,&y);
	iniFile.GetInteger("makeTeamBtn","width",0,&width);
	iniFile.GetInteger("makeTeamBtn","height",0,&height);
	makeTeamBtn.ChatWndCreate(MAKE_TEAM_BTN_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	iniFile.GetString("makeTeamBtn","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		makeTeamBtn.ChatWndSetText(infoText);
		makeTeamBtn.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("makeTeamBtn","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		makeTeamBtn.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("makeTeamBtn","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			makeTeamBtn.ChatWndSetFont(font);
		else
		{
			makeTeamBtn.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			makeTeamBtn.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("makeTeamBtn","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("makeTeamBtn","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("makeTeamBtn","disableStateSrcID",0,&disable_id);
	makeTeamBtn.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("makeTeamBtn","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		makeTeamBtn.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	makeTeamBtn.SetWndProcessFun(ChatWndProcessFun::ProcessLookInfoMakeTeamButton);
	////////////////////////////////////////////////////////

	iniFile.GetInteger("deleteMemberBtn","x",0,&x);
	iniFile.GetInteger("deleteMemberBtn","y",0,&y);
	iniFile.GetInteger("deleteMemberBtn","width",0,&width);
	iniFile.GetInteger("deleteMemberBtn","height",0,&height);
	deleteMemberBtn.ChatWndCreate(DELETE_MEMBER_BTN_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	iniFile.GetString("deleteMemberBtn","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		deleteMemberBtn.ChatWndSetText(infoText);
		deleteMemberBtn.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("deleteMemberBtn","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		deleteMemberBtn.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("deleteMemberBtn","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			deleteMemberBtn.ChatWndSetFont(font);
		else
		{
			deleteMemberBtn.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			deleteMemberBtn.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("deleteMemberBtn","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("deleteMemberBtn","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("deleteMemberBtn","disableStateSrcID",0,&disable_id);
	deleteMemberBtn.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("deleteMemberBtn","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		deleteMemberBtn.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	deleteMemberBtn.SetWndProcessFun(ChatWndProcessFun::ProcessLookInfoDeleteButton);
	/////////////////////////////////////////////////
	////////////////////////////////////
	iniFile.GetInteger("lookInfoBtn","x",0,&x);
	iniFile.GetInteger("lookInfoBtn","y",0,&y);
	iniFile.GetInteger("lookInfoBtn","width",0,&width);
	iniFile.GetInteger("lookInfoBtn","height",0,&height);
	lookInfoBtn.ChatWndCreate(LOOK_INFO_BTN_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	iniFile.GetString("lookInfoBtn","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		lookInfoBtn.ChatWndSetText(infoText);
		lookInfoBtn.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("lookInfoBtn","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		lookInfoBtn.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("lookInfoBtn","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			lookInfoBtn.ChatWndSetFont(font);
		else
		{
			lookInfoBtn.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			lookInfoBtn.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("lookInfoBtn","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("lookInfoBtn","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("lookInfoBtn","disableStateSrcID",0,&disable_id);
	lookInfoBtn.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("lookInfoBtn","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		lookInfoBtn.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	lookInfoBtn.SetWndProcessFun(ChatWndProcessFun::ProcessLookInfoLookButton);

	//////////////////////////////////////////////////////
	iniFile.GetInteger("pingbiInfoBtn","x",0,&x);
	iniFile.GetInteger("pingbiInfoBtn","y",0,&y);
	iniFile.GetInteger("pingbiInfoBtn","width",0,&width);
	iniFile.GetInteger("pingbiInfoBtn","height",0,&height);
	pingBiBtn.ChatWndCreate(PINGBI_BTN_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	iniFile.GetString("pingbiInfoBtn","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		pingBiBtn.ChatWndSetText(infoText);
		pingBiBtn.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("pingbiInfoBtn","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		pingBiBtn.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("pingbiInfoBtn","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			pingBiBtn.ChatWndSetFont(font);
		else
		{
			pingBiBtn.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			pingBiBtn.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("pingbiInfoBtn","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("pingbiInfoBtn","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("pingbiInfoBtn","disableStateSrcID",0,&disable_id);
	pingBiBtn.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("pingbiInfoBtn","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		pingBiBtn.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	pingBiBtn.SetWndProcessFun(ChatWndProcessFun::ProcessLookInfoPingBi);
	////////////////////////////////////////////////////
	iniFile.GetInteger("friendInfoBtn","x",0,&x);
	iniFile.GetInteger("friendInfoBtn","y",0,&y);
	iniFile.GetInteger("friendInfoBtn","width",0,&width);
	iniFile.GetInteger("friendInfoBtn","height",0,&height);
	friendInfoBtn.ChatWndCreate(FRIEND_INFO_BTN_ID,
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_OWNERDRAW,hParent,
		"","button",x,y,width,height);
	iniFile.GetString("friendInfoBtn","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		friendInfoBtn.ChatWndSetText(infoText);
		friendInfoBtn.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("friendInfoBtn","fontColor","",szVale,MAX_PATH);
		sscanf(szVale,"%d,%d,%d",&r,&g,&b);
		friendInfoBtn.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("friendInfoBtn","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			friendInfoBtn.ChatWndSetFont(font);
		else
		{
			friendInfoBtn.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			friendInfoBtn.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetInteger("friendInfoBtn","normalStateSrcID",0,&normal_id);
	iniFile.GetInteger("friendInfoBtn","hoverSTateSrcID",0,&hover_id);
	iniFile.GetInteger("friendInfoBtn","disableStateSrcID",0,&disable_id);
	friendInfoBtn.ChatWndSetResource(normal_id,hover_id,-1,disable_id);
	iniFile.GetString("friendInfoBtn","tooltipInfo","",szVale,MAX_PATH);
	if(szVale[0] != 0)
		friendInfoBtn.ChatWndTipCreate(TTS_NOPREFIX,szVale,100);
	friendInfoBtn.SetWndProcessFun(ChatWndProcessFun::ProcessLookInfoFriendButton);

}

LookFriendInfoDlg& LookFriendInfoDlg::GetSingle()
{
	static LookFriendInfoDlg lookInfoDlg;
	return lookInfoDlg;
}

void LookFriendInfoDlg::ProcessTalkPersonalLButtonDown()
{
	if(pPlayerControl == 0||
		talkPersonalBtn.ChatWndGetState()==_CHAT_BUTTON_STATE_DISABLE)
		return;
	pPlayerControl->PlayerControlProcessLButtonBLCLK();
	Hide();
}
void LookFriendInfoDlg::ProcessMakeTeamLButtonDown()
{
	if(pPlayerControl == 0||
		makeTeamBtn.ChatWndGetState()==_CHAT_BUTTON_STATE_DISABLE)
	{
		return;
	}
	KUiPlayerItem tagPlayer;
	strcpy(tagPlayer.Name,(char*)pPlayerControl->PlayerControlGetPlayerInfo().playerName);
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
	Hide();
}
void LookFriendInfoDlg::ProcessPingBiLButtonDown()
{
	if(pPlayerControl == 0||
		pingBiBtn.ChatWndGetState()==_CHAT_BUTTON_STATE_DISABLE)
		return;
	g_pCoreShell->OperationRequest( GOI_CHAT_ADD_BLACK_LIST, (unsigned int)pPlayerControl->PlayerControlGetPlayerInfo().playerName, NULL );
	Hide();
}
void LookFriendInfoDlg::ProcessFriendInfoLButtonDown()
{
	if(pPlayerControl == 0||
		friendInfoBtn.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
		return;
	g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)(pPlayerControl->PlayerControlGetPlayerInfo().playerName), 1 );
	Hide();
}
void LookFriendInfoDlg::ProcessDeleteLButtonDown()
{
	if(pPlayerControl == 0)
	{
		Hide();
		return;
	}

	g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, (UINT)pPlayerControl->PlayerControlGetPlayerInfo().playerName, NULL );
	pPlayerControl = 0;
	Hide();
	
}
void LookFriendInfoDlg::AdjustWindow()
{
	POINT pt;
	GetCursorPos(&pt);
	int height = talkPersonalBtn.ChatWndGetRect().bottom+deleteMemberBtn.ChatWndGetRect().bottom
		+lookInfoBtn.ChatWndGetRect().bottom+makeTeamBtn.ChatWndGetRect().bottom + pingBiBtn.ChatWndGetRect().bottom+
		friendInfoBtn.ChatWndGetRect().bottom;
	MoveWindow(hDlg,pt.x,pt.y,width,height,true);
}
void LookFriendInfoDlg::Show()
{
	SetWindowPos(hDlg,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE);
	ShowWindow(hDlg,SW_NORMAL);
	SetFocus(hDlg);
}
void LookFriendInfoDlg::Hide()
{
	ShowWindow(hDlg,SW_HIDE);
}
void LookFriendInfoDlg::ProcessKillFocus()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hDlg,&pt);
	RECT rc;
	GetClientRect(hDlg,&rc);
	if(!IsInRect(pt,rc))
	{
		Hide();
	}
}
