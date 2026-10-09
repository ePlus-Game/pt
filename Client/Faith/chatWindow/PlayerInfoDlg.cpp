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


#include "chatWindow/ChatControlPanel.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "resource.h"

#include "CoreShell.h"
#include "loadSrcWnd/GDILoadBitmap.h"

#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/chatManager.h"
#include "ChatWindow/chatDialog.h"
#include "ChatDataDef.h"
#include "chatWindow/ChatWndProc.h"
#include "chatWindow/ChatResource.h"
using namespace CHAT;
extern iCoreShell * g_pCoreShell;

#include "chatWindow/ChatClanTitleControl.h"
#include "chatWindow/ChatClanListControl.h"
#include "chatWindow/ChatClanInfoDlg.h"
#include "chatWindow/ChatClanPanel.h"
#include "chatWindow/ChatClanManager.h"





////////////////////////////////////////
ChatHintDeletePlayerDlg::ChatHintDeletePlayerDlg()
{

	bkBitmapIdx = 0;
	divY  = 0;
	memset(wndText,0,256);
	memset(infoText,0,256);
	memset(font,0,256);
	isUseDefaultFont = 0;
	hrgn = 0;
	
}
ChatHintDeletePlayerDlg::~ChatHintDeletePlayerDlg()
{

}
BOOL ChatHintDeletePlayerDlg::ChatHintDlgCreate(HWND hParent)
{
	if((hDlg =CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_TIP_DLG),
		hParent,
		(DLGPROC)ChatHintDeletePlayerDlg::ChatHintDlgProc))==0)
		return FALSE;
	return TRUE;
}

BOOL ChatHintDeletePlayerDlg::ChatHintDlgCreate(HWND hParent, DLGPROC func)
{
	if((hDlg =CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_TIP_DLG),
		hParent,
		func))==0)
		return FALSE;
	return TRUE;
}

void ChatHintDeletePlayerDlg::LoadSource_Lued()
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	BOOL isClip=0;
	int r=0,g=0,b=0;
	CHATWNDSRC* pSrc = 0;
	iniFile.Load(szPath);
	iniFile.GetInteger(_CHAT_HINT_DELETE_MEMBER_INI_TXT,_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger(_CHAT_HINT_DELETE_MEMBER_INI_TXT,_CHAT_SRC_HEIGHT,0,&height);
	iniFile.GetInteger(_CHAT_HINT_DELETE_MEMBER_INI_TXT,"BkSrcIdx",0,&bkBitmapIdx);
	ChatWnd::BitmapToRgn(ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,hrgn,RGB(255,255,255));
	SetWindowRgn(hDlg,hrgn,TRUE);
	
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_X,0,&okButton.x);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_Y,0,&okButton.y);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&okButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&okButton.ChatWndGetControlRect().bottom));

	okButton.ChatWndCreate(_CHAT_HINT_OK_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",okButton.x,okButton.y,okButton.ChatWndGetRect().right,okButton.ChatWndGetRect().bottom);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP,0,&isClip);
	if(isClip==1)
	{
		iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP_COLOR_R,0,&r);
		iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP_COLOR_G,0,&g);
		iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP_COLOR_B,0,&b);
	}
	int normal_idx = 0,hover_idx = 0,pushed_idx = 0,disable_idx = 0;
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	okButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char okTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_DELETE_DLG_OK_ITEM,"textInfo","",okTextInfo,256);
	if(okTextInfo[0]!=0)
	{
		okButton.ChatWndSetText(okTextInfo);
		okButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_OK_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		okButton.ChatWndSetTextNormalColor(RGB(r,g,b));

		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_OK_ITEM,"font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			okButton.ChatWndSetFont(font);
		else
		{
			okButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			okButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}

	}

	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_X,0,&noButton.x);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_Y,0,&noButton.y);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&noButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&noButton.ChatWndGetControlRect().bottom));
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_CLIP,0,&isClip);
	noButton.ChatWndCreate(_CHAT_HINT_NO_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",noButton.x,noButton.y,noButton.ChatWndGetRect().right,noButton.ChatWndGetRect().bottom);


	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	noButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char noTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_DELETE_DLG_NO_ITEM,"textInfo","",noTextInfo,256);
	if(noTextInfo[0]!=0)
	{
		noButton.ChatWndSetText(noTextInfo);
		noButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_NO_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		noButton.ChatWndSetTextNormalColor(RGB(r,g,b));

		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_NO_ITEM,"font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			noButton.ChatWndSetFont(font);
		else
		{
			noButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			noButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	iniFile.GetString(_CHAT_HINT_DELETE_CLAN_INI_TXT,"wndText","",wndText,256);
	iniFile.GetString(_CHAT_HINT_DELETE_CLAN_INI_TXT,"infoText","",infoText,256);
	iniFile.GetInteger(_CHAT_HINT_DELETE_MEMBER_INI_TXT,"divY",0,&divY);

	char font[FONT_SIZE]={0};
	iniFile.GetString(_CHAT_HINT_DELETE_MEMBER_INI_TXT,"font","",font,FONT_SIZE);
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
		strcpy(this->font,font);
		this->isUseDefaultFont = false;
	}
	else
	{
		strcpy(this->font,ChatString::ChatStringGetString().chatDefualtFont);
		this->isUseDefaultFont = true;
	}
}

void ChatHintDeletePlayerDlg::LoadSource_Clan()
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	BOOL isClip=0;
	int r=0,g=0,b=0;
	CHATWNDSRC* pSrc = 0;
	iniFile.Load(szPath);
	iniFile.GetInteger(_CHAT_HINT_DELETE_MEMBER_INI_TXT,_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger(_CHAT_HINT_DELETE_MEMBER_INI_TXT,_CHAT_SRC_HEIGHT,0,&height);
	iniFile.GetInteger(_CHAT_HINT_DELETE_MEMBER_INI_TXT,"BkSrcIdx",0,&bkBitmapIdx);
	ChatWnd::BitmapToRgn(ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,hrgn,RGB(255,255,255));
	SetWindowRgn(hDlg,hrgn,TRUE);
	
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_X,0,&okButton.x);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_Y,0,&okButton.y);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&okButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&okButton.ChatWndGetControlRect().bottom));

	okButton.ChatWndCreate(_CHAT_HINT_OK_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",okButton.x,okButton.y,okButton.ChatWndGetRect().right,okButton.ChatWndGetRect().bottom);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP,0,&isClip);
	if(isClip==1)
	{
		iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP_COLOR_R,0,&r);
		iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP_COLOR_G,0,&g);
		iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP_COLOR_B,0,&b);
	}
	int normal_idx = 0,hover_idx = 0,pushed_idx = 0,disable_idx = 0;
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	okButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char okTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_DELETE_DLG_OK_ITEM,"textInfo","",okTextInfo,256);
	if(okTextInfo[0]!=0)
	{
		okButton.ChatWndSetText(okTextInfo);
		okButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_OK_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		okButton.ChatWndSetTextNormalColor(RGB(r,g,b));

		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_OK_ITEM,"font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			okButton.ChatWndSetFont(font);
		else
		{
			okButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			okButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}

	}

	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_X,0,&noButton.x);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_Y,0,&noButton.y);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&noButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&noButton.ChatWndGetControlRect().bottom));
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_CLIP,0,&isClip);
	noButton.ChatWndCreate(_CHAT_HINT_NO_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",noButton.x,noButton.y,noButton.ChatWndGetRect().right,noButton.ChatWndGetRect().bottom);


	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	noButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char noTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_DELETE_DLG_NO_ITEM,"textInfo","",noTextInfo,256);
	if(noTextInfo[0]!=0)
	{
		noButton.ChatWndSetText(noTextInfo);
		noButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_NO_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		noButton.ChatWndSetTextNormalColor(RGB(r,g,b));

		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_NO_ITEM,"font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			noButton.ChatWndSetFont(font);
		else
		{
			noButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			noButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	iniFile.GetString(_CHAT_HINT_DELETE_MEMBER_INI_TXT,"wndText","",wndText,256);
	iniFile.GetString(_CHAT_HINT_DELETE_MEMBER_INI_TXT,"infoText","",infoText,256);
	iniFile.GetInteger(_CHAT_HINT_DELETE_MEMBER_INI_TXT,"divY",0,&divY);

	char font[FONT_SIZE]={0};
	iniFile.GetString(_CHAT_HINT_DELETE_MEMBER_INI_TXT,"font","",font,FONT_SIZE);
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
		strcpy(this->font,font);
		this->isUseDefaultFont = false;
	}
	else
	{
		strcpy(this->font,ChatString::ChatStringGetString().chatDefualtFont);
		this->isUseDefaultFont = true;
	}
}

void ChatHintDeletePlayerDlg::ChatHintDlgLoadSource()
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	BOOL isClip=0;
	int r=0,g=0,b=0;
	CHATWNDSRC* pSrc = 0;
	iniFile.Load(szPath);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_INI_NAME,_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_INI_NAME,_CHAT_SRC_HEIGHT,0,&height);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_INI_NAME,"BkSrcIdx",0,&bkBitmapIdx);
	ChatWnd::BitmapToRgn(ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,hrgn,RGB(255,255,255));
	SetWindowRgn(hDlg,hrgn,TRUE);
	
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_X,0,&okButton.x);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_Y,0,&okButton.y);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&okButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&okButton.ChatWndGetControlRect().bottom));

	okButton.ChatWndCreate(_CHAT_HINT_OK_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",okButton.x,okButton.y,okButton.ChatWndGetRect().right,okButton.ChatWndGetRect().bottom);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP,0,&isClip);
	if(isClip==1)
	{
		iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP_COLOR_R,0,&r);
		iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP_COLOR_G,0,&g);
		iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,_CHAT_SRC_CLIP_COLOR_B,0,&b);
	}
	int normal_idx = 0,hover_idx = 0,pushed_idx = 0,disable_idx = 0;
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_OK_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	okButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char okTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_DELETE_DLG_OK_ITEM,"textInfo","",okTextInfo,256);
	if(okTextInfo[0]!=0)
	{
		okButton.ChatWndSetText(okTextInfo);
		okButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_OK_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		okButton.ChatWndSetTextNormalColor(RGB(r,g,b));

		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_OK_ITEM,"font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			okButton.ChatWndSetFont(font);
		else
		{
			okButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			okButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}

	}

	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_X,0,&noButton.x);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_Y,0,&noButton.y);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&noButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&noButton.ChatWndGetControlRect().bottom));
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,_CHAT_SRC_CLIP,0,&isClip);
	noButton.ChatWndCreate(_CHAT_HINT_NO_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",noButton.x,noButton.y,noButton.ChatWndGetRect().right,noButton.ChatWndGetRect().bottom);


	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_NO_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	noButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char noTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_DELETE_DLG_NO_ITEM,"textInfo","",noTextInfo,256);
	if(noTextInfo[0]!=0)
	{
		noButton.ChatWndSetText(noTextInfo);
		noButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_NO_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		noButton.ChatWndSetTextNormalColor(RGB(r,g,b));

		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_DELETE_DLG_NO_ITEM,"font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			noButton.ChatWndSetFont(font);
		else
		{
			noButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			noButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	iniFile.GetString(_CHAT_HINT_DELETE_DLG_INI_NAME,"wndText","",wndText,256);
//	iniFile.GetString(_CHAT_HINT_DELETE_DLG_INI_NAME,"font","",font,256);
	iniFile.GetString(_CHAT_HINT_DELETE_DLG_INI_NAME,"infoText","",infoText,256);
	iniFile.GetInteger(_CHAT_HINT_DELETE_DLG_INI_NAME,"divY",0,&divY);

	char font[FONT_SIZE]={0};
	iniFile.GetString(_CHAT_HINT_DELETE_DLG_INI_NAME,"font","",font,FONT_SIZE);
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
		strcpy(this->font,font);
		this->isUseDefaultFont = false;
	}
	else
	{
		strcpy(this->font,ChatString::ChatStringGetString().chatDefualtFont);
		this->isUseDefaultFont = true;
	}
}
BOOL CALLBACK ChatHintDeletePlayerDlg::ChatHintDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG :
		{
//			ChatButton* pButton = &(ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.noButton);
//			pButton->ChatWndCreate(_CHAT_HINT_NO_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
	//			                   hwnd,"","button",pButton->x,pButton->y,pButton->ChatWndGetRect().right,pButton->ChatWndGetRect().bottom);
//			pButton  = &(ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.okButton);
//			pButton->ChatWndCreate(_CHAT_HINT_OK_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
	//			                   hwnd,"","button",pButton->x,pButton->y,pButton->ChatWndGetRect().right,pButton->ChatWndGetRect().bottom);
		
//			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgAdjustWindow(hwnd);
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_DRAWITEM:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgProcessDrawButton((LPDRAWITEMSTRUCT)lParam);
		}
		return FALSE;
	case WM_COMMAND:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgProcessOnButton(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgProcessMouseMove(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgProcessMouseLeave(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgProcessPaint((HDC)wParam);

		}
		return TRUE;
	case WM_DESTROY:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.okButton.ChatWndDeleteSrc();
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.noButton.ChatWndDeleteSrc();
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.wndText[0] = 0;
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.infoText[0] = 0;
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.font[0] = 0;
		}
		return FALSE;
	}
	return FALSE;
}
void ChatHintDeletePlayerDlg::ChatHintDlgAdjustWindow(HWND hwnd)
{
	PlayerInfoDlg* pDlg = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg();
	RECT rc;
	GetWindowRect(pDlg->hDlg,&rc);
	POINT pt;
	pt.x = rc.left;
	pt.y = rc.top;
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	POINT drawPos;
	drawPos.x = pt.x+width/2-this->width/2;
	drawPos.y = pt.y+height/2-this->height/2;
	MoveWindow(hDlg,drawPos.x,drawPos.y,this->width,this->height,TRUE);
}
void ChatHintDeletePlayerDlg::ChatHintDlgProcessDrawButton(LPDRAWITEMSTRUCT lpdis)
{
	switch(lpdis->CtlID)
	{
	case _CHAT_HINT_NO_BUTTON_ID:
		noButton.ChatWndDrawItem(lpdis->hDC);
		return;
	case _CHAT_HINT_OK_BUTTON_ID:
		okButton.ChatWndDrawItem(lpdis->hDC);
		return;
	}
}

void ChatHintDeletePlayerDlg::OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	switch(id)
	{
	case _CHAT_HINT_OK_BUTTON_ID:
		{
			if(HIWORD(wParam) == BN_CLICKED)
			{
				if (ChatClanManager::GetManager().m_ClanInfoDlg.hDlg == hwnd)
				{
					ChatClanManager::GetManager().m_ClanInfoDlg.DeleteMember(NULL);
				}
				
				else if (ChatLuedManager::GetManager().m_LuedInfoDlg.hDlg == hwnd)
				{
					ChatLuedManager::GetManager().m_LuedInfoDlg.DeleteMember(NULL);
				}
				ChatHintDlgShow(FALSE);
			}
		}
		return;
	case _CHAT_HINT_NO_BUTTON_ID:
		{
			if(HIWORD(wParam)==BN_CLICKED)
			{
				ChatHintDlgShow(FALSE);
			}
		}
		return;
	}
}

void ChatHintDeletePlayerDlg::ChatHintDlgProcessOnButton(HWND hwnd ,WPARAM wParam,LPARAM lParam)
{
	int id = LOWORD(wParam);
	switch(id)
	{
	case _CHAT_HINT_OK_BUTTON_ID:
		{
			if(HIWORD(wParam) == BN_CLICKED)
			{
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoDlgDeletePlayers();
				ChatHintDlgShow(FALSE);
			}
		}
		return;
	case _CHAT_HINT_NO_BUTTON_ID:
		{
			if(HIWORD(wParam)==BN_CLICKED)
			{
				ChatHintDlgShow(FALSE);
			}
		}
		return;
	}
}

void ChatHintDeletePlayerDlg::ChatHintDlgProcessPaint(HDC hdc )
{
	ChatHintDlgAdjustWindow(hDlg);
	HDC hdc1 = CreateCompatibleDC(hdc);
	BITMAP bm;
	GetObject(ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,sizeof(bm),&bm);
	HBITMAP hBitmap= CreateCompatibleBitmap(hdc,bm.bmWidth,bm.bmHeight);
	HBITMAP hOld = (HBITMAP)SelectObject(hdc1,hBitmap);
	DrawBitmap(hdc1,ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,bm.bmWidth,bm.bmHeight);
	SetBkMode(hdc1,TRANSPARENT);
	int fontHeight = 0;
	int fontWeight = 0;
	if(isUseDefaultFont)
	{
		fontHeight = ChatString::ChatStringGetString().chatDefualtFontHeight;
		fontWeight = FW_NORMAL;
		
	}
	else
	{
		fontHeight = divY-2;
		fontWeight = FW_BOLD;
	}
	HFONT hFont = CreateFont(fontHeight,0,0,0,fontWeight,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_ROMAN,font);
	HFONT hOldFont = (HFONT)SelectObject(hdc1,hFont);
	DWORD oldColor = SetTextColor(hdc1,RGB(128,64,0));
	RECT rc;
	rc.left = 0;
	rc.right= bm.bmWidth;
	rc.top = 2;
	rc.bottom = divY - 1;
	DrawText(hdc1,wndText - 1,-1,&rc,DT_CENTER|DT_NOCLIP|DT_VCENTER);
	rc.left = 0;
	rc.right = bm.bmWidth;
	rc.top = divY+(bm.bmHeight-divY)/3;
	rc.bottom = rc.top+divY;
	DrawText(hdc1,infoText,-1,&rc,DT_CENTER|DT_NOCLIP|DT_VCENTER);
	SetBkMode(hdc1,OPAQUE);
	SetTextColor(hdc1,oldColor);
	StretchBlt(hdc,0,0,width,height,hdc1,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);

	SelectObject(hdc1,hOldFont);
	SelectObject(hdc1,hOld);

	DeleteObject(hFont);
	DeleteObject(hBitmap);
	DeleteDC(hdc1);
	
}
void ChatHintDeletePlayerDlg::ChatHintDlgShow(BOOL bShow)
{
	if(bShow)
	{
		BringWindowToTop(hDlg);
		if(!IsWindowVisible(hDlg))
		{
			ShowWindow(hDlg,SW_NORMAL);
			isShow = bShow;
		}
	}
	else
	{
		if(IsWindowVisible(hDlg))
		{
			ShowWindow(hDlg,SW_HIDE);
			isShow = bShow;
		}
	}
}
void ChatHintDeletePlayerDlg::ChatHintDlgProcessMouseLeave(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	POINT pt;
	GetCursorPos(&pt);
	POINT ptClient;
	ptClient.x = pt.x;
	ptClient.y = pt.y;
	ScreenToClient(noButton.ChatWndGetHandle(),&ptClient);
	if(IsInRect(ptClient,noButton.ChatWndGetRect()))
	{
		if(noButton.ChatWndGetState() == _CHAT_BUTTON_STATE_NORMAL)
		{
			noButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
			noButton.ChatWndUpdate();
		}
		return;
	}

	ptClient.x = pt.x;
	ptClient.y = pt.y;
	ScreenToClient(okButton.ChatWndGetHandle(),&ptClient);
	if(IsInRect(ptClient,okButton.ChatWndGetRect()))
	{
		if(okButton.ChatWndGetState() == _CHAT_BUTTON_STATE_NORMAL)
		{
			okButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
			okButton.ChatWndUpdate();
		}
		return;
	}

}
void ChatHintDeletePlayerDlg::ChatHintDlgProcessMouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_HOVER|TME_LEAVE;
	tme.dwHoverTime=1000;
	tme.hwndTrack=hDlg;
	_TrackMouseEvent(&tme);
	if(noButton.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEOVER)
	{
		noButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		noButton.ChatWndUpdate();
	}
	if(okButton.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEOVER)
	{
		okButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		okButton.ChatWndUpdate();
	}
}
ChatHintAddPlayerDlg::ChatHintAddPlayerDlg()
{
	isShow = FALSE;
	isUseDefaultFont = FALSE;
	hRgn = 0;
	hEditFont = 0;
}
ChatHintAddPlayerDlg::~ChatHintAddPlayerDlg()
{

	if(hEditBrush)
		DeleteObject(hEditBrush);
//	if(hBkBitmapMask)
//		DeleteObject(hBkBitmapMask);
	if(hRgn)
		DeleteObject(hRgn);
	if(hEditFont)
		DeleteObject(hEditFont);
}
void ChatHintAddPlayerDlg::ChatHintDlgAdjustWindow(HWND hwnd)
{
	PlayerInfoDlg* pDlg = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg();
	RECT rc;
	GetWindowRect(pDlg->hDlg,&rc);
	POINT pt;
	pt.x = rc.left;
	pt.y = rc.top;
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	POINT drawPos;
	drawPos.x = pt.x+width/2-this->width/2;
	drawPos.y = pt.y+height/2-this->height/2;
	MoveWindow(hDlg,drawPos.x,drawPos.y,this->width,this->height,TRUE);
}
BOOL ChatHintAddPlayerDlg::ChatHintDlgCreate(HWND hParent)
{
	if(( hDlg = CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_TIP_DLG),
		hParent,
		(DLGPROC)ChatHintAddPlayerDlg::ChatHintDlgProc))==0)
		return FALSE;
	return TRUE;
}

BOOL ChatHintAddPlayerDlg::ChatHintDlgCreate(HWND hParent, DLGPROC func)
{
	if(( hDlg = CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_TIP_DLG),
		hParent,
		func))==0)
		return FALSE;
	return TRUE;
}


void ChatHintAddPlayerDlg::ChatHintDlgLoadSource()
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH]= {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH]={0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	int isClip;
	int r,g,b;
	CHATWNDSRC* pSrc = 0;
	iniFile.Load(szPath);
	iniFile.GetInteger(_CHAT_HINT_ADD_INI_NAME,_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger(_CHAT_HINT_ADD_INI_NAME,_CHAT_SRC_HEIGHT,0,&height);

	iniFile.GetInteger(_CHAT_HINT_ADD_INI_NAME,"BkSrcIdx",0,&bkBitmapIdx);
	ChatWnd::BitmapToRgn(ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,hRgn,RGB(255,255,255));
	SetWindowRgn(this->hDlg,hRgn,TRUE);
	
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_X,0,&okButton.x);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_Y,0,&okButton.y);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&okButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&okButton.ChatWndGetControlRect().bottom));
	okButton.ChatWndCreate(_CHAT_HINT_ADD_OK_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",okButton.x,okButton.y,okButton.ChatWndGetRect().right,okButton.ChatWndGetRect().bottom);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_CLIP,0,&isClip);

	int normal_idx = 0,hover_idx = 0,pushed_idx = 0,disable_idx = 0;
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	okButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char okTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_ADD_DLG_OK_ITEM,"textInfo","",okTextInfo,256);
	if(okTextInfo[0]!=0)
	{
		okButton.ChatWndSetText(okTextInfo);
		okButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_ADD_DLG_OK_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		okButton.ChatWndSetTextNormalColor(RGB(r,g,b));


		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_ADD_DLG_OK_ITEM,"font","",font,256);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			okButton.ChatWndSetFont(font);
		else
		{
			okButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			okButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_X,0,&noButton.x);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_Y,0,&noButton.y);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&noButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&noButton.ChatWndGetControlRect().bottom));


	noButton.ChatWndCreate(_CHAT_HINT_ADD_NO_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",noButton.x,noButton.y,noButton.ChatWndGetRect().right,noButton.ChatWndGetRect().bottom);


	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	noButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char noTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_ADD_DLG_NO_ITEM,"textInfo","",noTextInfo,256);
	if(noTextInfo[0]!=0)
	{
		noButton.ChatWndSetText(noTextInfo);
		noButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_ADD_DLG_NO_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		noButton.ChatWndSetTextNormalColor(RGB(r,g,b));


		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_ADD_DLG_NO_ITEM,"font","",font,256);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			noButton.ChatWndSetFont(font);
		else
		{
			noButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			noButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_X,0,&editX);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_Y,0,&editY);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_WIDTH,0,&editWidth);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_HEIGHT,0,&editHeight);
	


	
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,"BkSrcIdx",0,&editBkIdx);
	hEditBrush  = CreatePatternBrush(ChatResource::GetSingle().GetResource(editBkIdx)->hBitmap);

	iniFile.GetInteger(_CHAT_HINT_ADD_INI_NAME,"divY",0,&divY);
	iniFile.GetInteger(_CHAT_HINT_ADD_INI_NAME,"divY1",0,&divY1);
	iniFile.GetInteger(_CHAT_HINT_ADD_INI_NAME,"divX1",0,&divX1);
	iniFile.GetInteger(_CHAT_HINT_ADD_INI_NAME,"bkX",0,&bkMaskX);
	iniFile.GetInteger(_CHAT_HINT_ADD_INI_NAME,"bkY",0,&bkMaskY);
	iniFile.GetString(_CHAT_HINT_ADD_INI_NAME,"wndText","",wndText,256);

	iniFile.GetString(_CHAT_HINT_ADD_INI_NAME,"infoText","",infoText,256);

	///////////////////
	hEdit = CreateWindowEx(0,"edit","",WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|ES_AUTOHSCROLL,
		editX,editY,editWidth,editHeight,hDlg,(HMENU)_CHAT_HINT_EDIT_ID,KWin32App::m_hInstance,0);
	ChatHintDlgAdjustWindow(hDlg);
	RECT rc;
	GetClientRect(hEdit,&rc);
	HDC hdc = CreateCompatibleDC(NULL);
	hEditFont = CreateFont(0,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,ChatFriendPanelManager::ChatFriendManagerGet().addDlg.font);
	SelectObject(hdc,hEditFont);
	TEXTMETRIC tm;
	GetTextMetrics(hdc,&tm);
	DeleteObject(hEditFont);
	int fontHeight = ChatString::ChatStringGetString().chatDefualtFontHeight;
	hEditFont = CreateFont(16,0,0,0,600,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_ROMAN,ChatFriendPanelManager::ChatFriendManagerGet().addDlg.font);
	DeleteDC(hdc);
	SendMessage(hEdit,WM_SETFONT,(WPARAM)hEditFont,1);
//	iniFile.GetString(_CHAT_HINT_ADD_INI_NAME,"bkSrcMask","",szValue,256);

	

//	memset(szImagePath,0,MAX_PATH);
//	strcpy(szImagePath,szImagePathIndex);
//	strcat(szImagePath,szValue);
//	hBkBitmapMask = LoadBitmapFromFile(szImagePath);


	char font[FONT_SIZE]={0};
	iniFile.GetString(_CHAT_HINT_ADD_INI_NAME,"font","",font,256);
	TFONT tFont;
	strcpy(tFont.fontName,font);
	HDC hdc1 =CreateCompatibleDC(NULL);
	LOGFONT logFont;
	logFont.lfFaceName[0]=0;
	logFont.lfCharSet = DEFAULT_CHARSET;
	EnumFontFamiliesEx(hdc1,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
	DeleteDC(hdc1);
	if(tFont.isInSystem)
	{
		this->isUseDefaultFont = false;
		strcpy(this->font,font);
	}
	else
	{
		strcpy(this->font,ChatString::ChatStringGetString().chatDefualtFont);
		this->isUseDefaultFont = true;
	}


}

void ChatHintAddPlayerDlg::LoadSource_Lued()
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH]= {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH]={0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	int isClip;
	int r,g,b;
	CHATWNDSRC* pSrc = 0;
	iniFile.Load(szPath);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,_CHAT_SRC_HEIGHT,0,&height);

	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"BkSrcIdx",0,&bkBitmapIdx);
	ChatWnd::BitmapToRgn(ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,hRgn,RGB(255,255,255));
	SetWindowRgn(this->hDlg,hRgn,TRUE);
	
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_X,0,&okButton.x);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_Y,0,&okButton.y);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&okButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&okButton.ChatWndGetControlRect().bottom));
	okButton.ChatWndCreate(_CHAT_HINT_ADD_OK_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",okButton.x,okButton.y,okButton.ChatWndGetRect().right,okButton.ChatWndGetRect().bottom);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_CLIP,0,&isClip);

	int normal_idx = 0,hover_idx = 0,pushed_idx = 0,disable_idx = 0;
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	okButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char okTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_ADD_DLG_OK_ITEM,"textInfo","",okTextInfo,256);
	if(okTextInfo[0]!=0)
	{
		okButton.ChatWndSetText(okTextInfo);
		okButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_ADD_DLG_OK_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		okButton.ChatWndSetTextNormalColor(RGB(r,g,b));


		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_ADD_DLG_OK_ITEM,"font","",font,256);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			okButton.ChatWndSetFont(font);
		else
		{
			okButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			okButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_X,0,&noButton.x);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_Y,0,&noButton.y);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&noButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&noButton.ChatWndGetControlRect().bottom));


	noButton.ChatWndCreate(_CHAT_HINT_ADD_NO_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",noButton.x,noButton.y,noButton.ChatWndGetRect().right,noButton.ChatWndGetRect().bottom);


	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	noButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char noTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_ADD_DLG_NO_ITEM,"textInfo","",noTextInfo,256);
	if(noTextInfo[0]!=0)
	{
		noButton.ChatWndSetText(noTextInfo);
		noButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_ADD_DLG_NO_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		noButton.ChatWndSetTextNormalColor(RGB(r,g,b));


		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_ADD_DLG_NO_ITEM,"font","",font,256);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			noButton.ChatWndSetFont(font);
		else
		{
			noButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			noButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_X,0,&editX);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_Y,0,&editY);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_WIDTH,0,&editWidth);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_HEIGHT,0,&editHeight);
	


	
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,"BkSrcIdx",0,&editBkIdx);
	hEditBrush  = CreatePatternBrush(ChatResource::GetSingle().GetResource(editBkIdx)->hBitmap);

	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"divY",0,&divY);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"divY1",0,&divY1);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"divX1",0,&divX1);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"bkX",0,&bkMaskX);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"bkY",0,&bkMaskY);
	iniFile.GetString(_CHAT_HINT_ADD_CLAN_INI_TXT,"wndText","",wndText,256);

	iniFile.GetString(_CHAT_HINT_ADD_CLAN_INI_TXT,"infoText","",infoText,256);

	///////////////////
	hEdit = CreateWindowEx(0,"edit","",WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|ES_AUTOHSCROLL,
		editX, editY, editWidth, editHeight, hDlg,(HMENU)_CHAT_HINT_EDIT_ID,KWin32App::m_hInstance,0);
	ChatHintDlgAdjustWindow(hDlg);
	RECT rc;
	GetClientRect(hEdit,&rc);
	HDC hdc = CreateCompatibleDC(NULL);
	hEditFont = CreateFont(0, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, ChatClanManager::GetManager().addDlg.font);
	SelectObject(hdc,hEditFont);
	TEXTMETRIC tm;
	GetTextMetrics(hdc,&tm);
	DeleteObject(hEditFont);
	int fontHeight = ChatString::ChatStringGetString().chatDefualtFontHeight;
	hEditFont = CreateFont(16, 0, 0, 0, 600, FALSE, FALSE, FALSE, ANSI_CHARSET,	OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_ROMAN, ChatClanManager::GetManager().addDlg.font);
	DeleteDC(hdc);
	SendMessage(hEdit,WM_SETFONT,(WPARAM)hEditFont,1);

	char font[FONT_SIZE]={0};
	iniFile.GetString(_CHAT_HINT_ADD_MEMBER_INI_TXT, "font", "", font, 256);
	TFONT tFont;
	strcpy(tFont.fontName, font);
	HDC hdc1 =CreateCompatibleDC(NULL);
	LOGFONT logFont;
	logFont.lfFaceName[0]=0;
	logFont.lfCharSet = DEFAULT_CHARSET;
	EnumFontFamiliesEx(hdc1, &logFont, (FONTENUMPROC)EnumFontProc, (LPARAM)&tFont, 0);
	DeleteDC(hdc1);
	if(tFont.isInSystem)
	{
		this->isUseDefaultFont = false;
		strcpy(this->font,font);
	}
	else
	{
		strcpy(this->font,ChatString::ChatStringGetString().chatDefualtFont);
		this->isUseDefaultFont = true;
	}
}

void ChatHintAddPlayerDlg::LoadSource_Clan()
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH]= {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH]={0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	int isClip;
	int r,g,b;
	CHATWNDSRC* pSrc = 0;
	iniFile.Load(szPath);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,_CHAT_SRC_HEIGHT,0,&height);

	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"BkSrcIdx",0,&bkBitmapIdx);
	ChatWnd::BitmapToRgn(ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,hRgn,RGB(255,255,255));
	SetWindowRgn(this->hDlg,hRgn,TRUE);
	
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_X,0,&okButton.x);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_Y,0,&okButton.y);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&okButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&okButton.ChatWndGetControlRect().bottom));
	okButton.ChatWndCreate(_CHAT_HINT_ADD_OK_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",okButton.x,okButton.y,okButton.ChatWndGetRect().right,okButton.ChatWndGetRect().bottom);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,_CHAT_SRC_CLIP,0,&isClip);

	int normal_idx = 0,hover_idx = 0,pushed_idx = 0,disable_idx = 0;
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_OK_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	okButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char okTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_ADD_DLG_OK_ITEM,"textInfo","",okTextInfo,256);
	if(okTextInfo[0]!=0)
	{
		okButton.ChatWndSetText(okTextInfo);
		okButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_ADD_DLG_OK_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		okButton.ChatWndSetTextNormalColor(RGB(r,g,b));


		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_ADD_DLG_OK_ITEM,"font","",font,256);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			okButton.ChatWndSetFont(font);
		else
		{
			okButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			okButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_X,0,&noButton.x);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_Y,0,&noButton.y);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_WIDTH,0,(int*)(&noButton.ChatWndGetControlRect().right));
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,_CHAT_SRC_HEIGHT,0,(int*)(&noButton.ChatWndGetControlRect().bottom));


	noButton.ChatWndCreate(_CHAT_HINT_ADD_NO_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
				                   hDlg,"","button",noButton.x,noButton.y,noButton.ChatWndGetRect().right,noButton.ChatWndGetRect().bottom);


	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_NO_ITEM,"mouseOverSrcIdx",0,&hover_idx);
	noButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char noTextInfo[256]={0};
	iniFile.GetString(_CHAT_HINT_ADD_DLG_NO_ITEM,"textInfo","",noTextInfo,256);
	if(noTextInfo[0]!=0)
	{
		noButton.ChatWndSetText(noTextInfo);
		noButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_CHAT_HINT_ADD_DLG_NO_ITEM,"fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		noButton.ChatWndSetTextNormalColor(RGB(r,g,b));


		char font[FONT_SIZE]={0};
		iniFile.GetString(_CHAT_HINT_ADD_DLG_NO_ITEM,"font","",font,256);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			noButton.ChatWndSetFont(font);
		else
		{
			noButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			noButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_X,0,&editX);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_Y,0,&editY);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_WIDTH,0,&editWidth);
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,_CHAT_SRC_HEIGHT,0,&editHeight);
	


	
	iniFile.GetInteger(_CHAT_HINT_ADD_DLG_GET_ITEM,"BkSrcIdx",0,&editBkIdx);
	hEditBrush  = CreatePatternBrush(ChatResource::GetSingle().GetResource(editBkIdx)->hBitmap);

	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"divY",0,&divY);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"divY1",0,&divY1);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"divX1",0,&divX1);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"bkX",0,&bkMaskX);
	iniFile.GetInteger(_CHAT_HINT_ADD_MEMBER_INI_TXT,"bkY",0,&bkMaskY);
	iniFile.GetString(_CHAT_HINT_ADD_MEMBER_INI_TXT,"wndText","",wndText,256);

	iniFile.GetString(_CHAT_HINT_ADD_MEMBER_INI_TXT,"infoText","",infoText,256);

	///////////////////
	hEdit = CreateWindowEx(0,"edit","",WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|ES_AUTOHSCROLL,
		editX, editY, editWidth, editHeight, hDlg,(HMENU)_CHAT_HINT_EDIT_ID,KWin32App::m_hInstance,0);
	ChatHintDlgAdjustWindow(hDlg);
	RECT rc;
	GetClientRect(hEdit,&rc);
	HDC hdc = CreateCompatibleDC(NULL);
	hEditFont = CreateFont(0, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, ChatClanManager::GetManager().addDlg.font);
	SelectObject(hdc,hEditFont);
	TEXTMETRIC tm;
	GetTextMetrics(hdc,&tm);
	DeleteObject(hEditFont);
	int fontHeight = ChatString::ChatStringGetString().chatDefualtFontHeight;
	hEditFont = CreateFont(16, 0, 0, 0, 600, FALSE, FALSE, FALSE, ANSI_CHARSET,	OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_ROMAN, ChatClanManager::GetManager().addDlg.font);
	DeleteDC(hdc);
	SendMessage(hEdit,WM_SETFONT,(WPARAM)hEditFont,1);

	char font[FONT_SIZE]={0};
	iniFile.GetString(_CHAT_HINT_ADD_MEMBER_INI_TXT, "font", "", font, 256);
	TFONT tFont;
	strcpy(tFont.fontName, font);
	HDC hdc1 =CreateCompatibleDC(NULL);
	LOGFONT logFont;
	logFont.lfFaceName[0]=0;
	logFont.lfCharSet = DEFAULT_CHARSET;
	EnumFontFamiliesEx(hdc1, &logFont, (FONTENUMPROC)EnumFontProc, (LPARAM)&tFont, 0);
	DeleteDC(hdc1);
	if(tFont.isInSystem)
	{
		this->isUseDefaultFont = false;
		strcpy(this->font,font);
	}
	else
	{
		strcpy(this->font,ChatString::ChatStringGetString().chatDefualtFont);
		this->isUseDefaultFont = true;
	}
}

void ChatHintAddPlayerDlg::ChatHintDlgProcessDrawButton(LPDRAWITEMSTRUCT lpdis)
{
	switch(lpdis->CtlID)
	{
	case _CHAT_HINT_ADD_NO_BUTTON_ID:
		noButton.ChatWndDrawItem(lpdis->hDC);
		return;
	case _CHAT_HINT_ADD_OK_BUTTON_ID:
		okButton.ChatWndDrawItem(lpdis->hDC);
		return;
	}
}
void ChatHintAddPlayerDlg::ChatHintDlgShow(BOOL bShow)
{
	SetWindowText(hEdit,0);
	isShow = bShow;
	if(isShow)
	{
		BringWindowToTop(hDlg);
		if(!IsWindowVisible(hDlg))
		{
			ShowWindow(hDlg,SW_NORMAL);
		}
	}
	else
	{
		if(IsWindowVisible(hDlg))
			ShowWindow(hDlg,SW_HIDE);
	}
}
void ChatHintAddPlayerDlg::ChatHintDlgProcessMouseLeave(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	POINT pt;
	GetCursorPos(&pt);
	POINT ptClient;
	ptClient.x = pt.x;
	ptClient.y = pt.y;
	ScreenToClient(noButton.ChatWndGetHandle(),&ptClient);
	if(IsInRect(ptClient,noButton.ChatWndGetRect()))
	{
		if(noButton.ChatWndGetState() == _CHAT_BUTTON_STATE_NORMAL)
		{
			noButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
			noButton.ChatWndUpdate();
		}
		return;
	}

	ptClient.x = pt.x;
	ptClient.y = pt.y;
	ScreenToClient(okButton.ChatWndGetHandle(),&ptClient);
	if(IsInRect(ptClient,okButton.ChatWndGetRect()))
	{
		if(okButton.ChatWndGetState() == _CHAT_BUTTON_STATE_NORMAL)
		{
			okButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
			okButton.ChatWndUpdate();
		}
		return;
	}
}

void ChatHintAddPlayerDlg::ChatHintDlgProcessMouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_HOVER|TME_LEAVE;
	tme.dwHoverTime=1000;
	tme.hwndTrack=hDlg;
	_TrackMouseEvent(&tme);
	if(noButton.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEOVER)
	{
		noButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		noButton.ChatWndUpdate();
	}
	if(okButton.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEOVER)
	{
		okButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		okButton.ChatWndUpdate();
	}
}

void ChatHintAddPlayerDlg::OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	switch(id)
	{
	case _CHAT_HINT_ADD_OK_BUTTON_ID:
		{
			if(HIWORD(wParam) == BN_CLICKED)
			{
				char name[20] = {0};
				GetWindowText(hEdit,name,20);
				name[19] = 0;
				if (hwnd == ChatClanManager::GetManager().m_ClanInfoDlg.hDlg)
				{
					ChatClanManager::GetManager().m_ClanInfoDlg.AddMember(name, ChatClanManager::GetManager().m_ClanInfoDlg.hDlg);
				}
				else if (hwnd == ChatLuedManager::GetManager().m_LuedInfoDlg.hDlg)
				{
					ChatLuedManager::GetManager().m_LuedInfoDlg.AddMember(name, ChatLuedManager::GetManager().m_LuedInfoDlg.hDlg);
				}
				ChatHintDlgShow(FALSE);
			}
		}
		return;
	case _CHAT_HINT_ADD_NO_BUTTON_ID:
		{
			if(HIWORD(wParam)==BN_CLICKED)
			{
				ChatHintDlgShow(FALSE);
			}
		}
		return;
	}
}

void ChatHintAddPlayerDlg::ChatHintDlgProcessOnButton(HWND hwnd ,WPARAM wParam,LPARAM lParam)
{
	int id = LOWORD(wParam);
	switch(id)
	{
	case _CHAT_HINT_ADD_OK_BUTTON_ID:
		{
			if(HIWORD(wParam) == BN_CLICKED)
			{
				char name[20] = {0};
				GetWindowText(hEdit,name,20);
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoAddPlayers(name);
				ChatHintDlgShow(FALSE);
			}
		}
		return;
	case _CHAT_HINT_ADD_NO_BUTTON_ID:
		{
			if(HIWORD(wParam)==BN_CLICKED)
			{
				ChatHintDlgShow(FALSE);
			}
		}
		return;
	}
}

void ChatHintAddPlayerDlg::ChatHintDlgProcessPaint(HDC hdc )
{
//	ChatHintDlgAdjustWindow(hDlg);
	HDC hdc1 = CreateCompatibleDC(hdc);
	BITMAP bm;
	GetObject(ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,sizeof(bm),&bm);
	HBITMAP hBitmap= CreateCompatibleBitmap(hdc,bm.bmWidth,bm.bmHeight);
	HBITMAP hOld = (HBITMAP)SelectObject(hdc1,hBitmap);
	DrawBitmap(hdc1,ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,bm.bmWidth,bm.bmHeight);
//	BITMAP bmMask;
//	GetObject(hBkBitmapMask,sizeof(BITMAP),&bmMask);
//	DrawBitmap(hdc1,hBkBitmapMask,bmMask.bmWidth,bmMask.bmHeight,bkMaskX,bkMaskY);
	
	SetBkMode(hdc1,TRANSPARENT);
	int fontHeight = 0;
	int fontWeight = 0;
	RECT rc,rc1;
	if(isUseDefaultFont)
	{
		fontHeight = ChatString::ChatStringGetString().chatDefualtFontHeight;
		fontWeight = FW_NORMAL;
		rc.left = 0;
		rc.right= bm.bmWidth;
		rc.top = 4;
		rc.bottom = divY;

		rc1.left = 0;
		rc1.right = bm.bmWidth;
		rc1.top = divY+(bm.bmHeight-divY)/10+4;
		rc1.bottom = rc1.top+divY+1;
	}
	else
	{
		fontHeight = divY-2;
		fontWeight = FW_BOLD;
		rc.left = 0;
		rc.right= bm.bmWidth;
		rc.top = 1;
		rc.bottom = divY;
		rc1.left = 0;
		rc1.right = bm.bmWidth;
		rc1.top = divY+(bm.bmHeight-divY)/10+1;
		rc1.bottom = rc1.top+divY;
	}
	HFONT hFont = CreateFont(fontHeight,0,0,0,fontWeight,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_ROMAN,font);
	HFONT hOldFont = (HFONT)SelectObject(hdc1,hFont);
	DWORD oldColor = SetTextColor(hdc1,RGB(128,64,0));
	DrawText(hdc1,wndText,-1,&rc,DT_CENTER|DT_NOCLIP|DT_VCENTER);
	DrawText(hdc1,infoText,-1,&rc1,DT_CENTER|DT_NOCLIP|DT_VCENTER);
	SetBkMode(hdc1,OPAQUE);
	SetTextColor(hdc1,oldColor);
	StretchBlt(hdc,0,0,width,height,hdc1,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	SelectObject(hdc1,hOldFont);
	SelectObject(hdc1,hOld);
	DeleteObject(hFont);
	
	DeleteObject(hBitmap);
	DeleteDC(hdc1);
}
BOOL CALLBACK ChatHintAddPlayerDlg::ChatHintDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
//	static HFONT hFont;
	switch(msg)
	{
	case WM_INITDIALOG :
		{
		//	ChatButton* pButton = &(ChatFriendPanelManager::ChatFriendManagerGet().addDlg.noButton);
	//		pButton->ChatWndCreate(_CHAT_HINT_ADD_NO_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
		//		                   hwnd,"","button",pButton->x,pButton->y,pButton->ChatWndGetRect().right,pButton->ChatWndGetRect().bottom);
//			pButton  = &(ChatFriendPanelManager::ChatFriendManagerGet().addDlg.okButton);
	//		pButton->ChatWndCreate(_CHAT_HINT_ADD_OK_BUTTON_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|BS_NOTIFY|BS_OWNERDRAW,
			//	                   hwnd,"","button",pButton->x,pButton->y,pButton->ChatWndGetRect().right,pButton->ChatWndGetRect().bottom);
			
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);

		}
		return FALSE;
	case WM_DRAWITEM:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgProcessDrawButton((LPDRAWITEMSTRUCT)lParam);
		}
		return FALSE;
	case WM_COMMAND:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgProcessOnButton(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgProcessMouseMove(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgProcessMouseLeave(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgProcessPaint((HDC)wParam);

		}
		return TRUE;
	case WM_DESTROY:
		{
//			DeleteObject(hFont);
			DeleteObject(ChatFriendPanelManager::ChatFriendManagerGet().addDlg.hEditBrush);
//			DeleteObject(ChatFriendPanelManager::ChatFriendManagerGet().addDlg.hBkBitmapMask);
		}
		break;
	case WM_CTLCOLOREDIT:
		{
			switch(GetWindowLong((HWND)lParam,GWL_ID))
			{
			case _CHAT_HINT_EDIT_ID:
				{
					SetBkMode((HDC)wParam,TRANSPARENT);
					SetTextColor((HDC)wParam,RGB(255,230,20));
					return (BOOL)ChatFriendPanelManager::ChatFriendManagerGet().addDlg.hEditBrush;
				}
				break;
			}
		}
		return FALSE;
	}
	return FALSE;
}
/////////////////////////////////////////


PlayerInfoDlg::PlayerInfoDlg()
{
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	bitmapIdx = 0;
	hDlg = 0;
	isShow = FALSE;
	numberLeftLinePlayers = 0;
	numberOnlinePlayers = 0;

	drawPointX = 0;
	drawPointY = 0;
	drawAreaWidth = 0;
	drawAreaHeight = 0;
	intermission = 0;
	drawAreaX = 0;
	drawAreaY = 0;
	drawIndex = 0;

	drawAreaBKBitmapIdx = 0;
	state = _PLAYER_LIST_CONTROL_SHOW_LEFT|_PLAYER_LIST_CONTROL_NORMAL_SHOW;
	allFriendsLength = 0;
}
PlayerInfoDlg::~PlayerInfoDlg()
{
}

void PlayerInfoDlg::PlayerInfoClearSelect(BOOL update)
{
	for(int i = 0; i < numberOnlinePlayers;i++)
	{
		if(pOnlinePlayers[i]->PlayerControlIsSelected())
			pOnlinePlayers[i]->PlayerControlSetSelected(FALSE,update);
	}
	for(i = 0;i < numberLeftLinePlayers;i++)
	{
		if(pLeftLinePlayers[i]->PlayerControlIsSelected())
			pLeftLinePlayers[i]->PlayerControlSetSelected(FALSE,update);
	}
}
void PlayerInfoDlg::PlayerInfoDlgShowDlg(BOOL bShow)
{
	isShow = bShow;
	if(isShow)
	{
		if(!IsWindowVisible(hDlg))
			ShowWindow(hDlg,SW_NORMAL);
	}
	else
	{
		if(IsWindowVisible(hDlg))
			ShowWindow(hDlg,SW_HIDE);
	}
}

LPPLAYERCONTROL PlayerInfoDlg::PlayerInfoGetControlByPoint(POINT& pt)
{
	int i = 0;
	for( i = 0 ;i < numberOnlinePlayers;i++)
	{
		const ONRECT& onwerRc = pOnlinePlayers[i]->PlayerControlGetPosRect();
		RECT rc;
		rc.left = onwerRc.x;
		rc.right = onwerRc.x + onwerRc.width;
		rc.top = onwerRc.y;
		rc.bottom = onwerRc.y + onwerRc.height;
		if(IsInRect(pt,rc))
		{
			return pOnlinePlayers[i];
		}
	}

	for(i = 0; i < numberLeftLinePlayers;i++)
	{
		const ONRECT& onwerRc = pLeftLinePlayers[i]->PlayerControlGetPosRect();
		RECT rc;
		rc.left = onwerRc.x;
		rc.right = onwerRc.x + onwerRc.width;
		rc.top = onwerRc.y;
		rc.bottom = onwerRc.y + onwerRc.height;
		if(IsInRect(pt,rc))
		{
			return pLeftLinePlayers[i];
		}

	}
	return NULL;
}
void PlayerInfoDlg::PlayerInfoMakeMixAllPlayers()
{
	memset(pAllPlayers,0,sizeof(LPPLAYERCONTROL)*_CHAT_MAX_FRIENDS);
	int num = 0;
	for(;num < numberOnlinePlayers ; num++)
		pAllPlayers[num] = pOnlinePlayers[num];
	for(int i = 0;i<numberLeftLinePlayers;i++,num++)
		pAllPlayers[num] = pLeftLinePlayers[i];
}
void PlayerInfoDlg::PlayerInfoProcessLButtonDBCLK(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	titleControl.TitleControlProcessLButtonDBLCLK(*this,pt);
	ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(false);
	ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(false);
	int index = 0,i = 0;
	if(state&_PLAYER_LIST_CONTROL_SHOW_LEFT)
	{
		if(state&_PLAYER_LIST_CONTROL_NORMAL_SHOW)
		{
			if(drawIndex>=numberOnlinePlayers)
			{
				for(i  = 0,index = drawIndex - numberOnlinePlayers; index<numberLeftLinePlayers;index++,i++)
				{
					const ONRECT& onwerRc = pLeftLinePlayers[index]->PlayerControlGetPosRect();
					RECT rc;
					rc.left = onwerRc.x;
					rc.right = rc.left+ onwerRc.width;
					rc.top = onwerRc.y;
					rc.bottom = onwerRc.y + onwerRc.height;
					if(IsInRect(pt,rc))
					{
						pLeftLinePlayers[index]->PlayerControlProcessLButtonBLCLK();
						return ;
					}
					if(pLeftLinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
					{
						break;
					}
					
				}
			}
			else
			{
				for(i = 0,index = drawIndex;index < numberOnlinePlayers ;index++,i++)
				{
					const ONRECT& onwerRc = pOnlinePlayers[index]->PlayerControlGetPosRect();
					RECT rc;
					rc.left = onwerRc.x;
					rc.right = rc.left+ onwerRc.width;
					rc.top = onwerRc.y;
					rc.bottom = onwerRc.y + onwerRc.height;
					if(IsInRect(pt,rc))
					{
						pOnlinePlayers[index]->PlayerControlProcessLButtonBLCLK();
						return ;
					}
					if( pOnlinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
					{
						return;
					}
				}
				for(i= 0,index = 0;index <numberLeftLinePlayers;index++,i++)
				{
					const ONRECT& onwerRc = pLeftLinePlayers[index]->PlayerControlGetPosRect();
					RECT rc;
					rc.left = onwerRc.x;
					rc.right = rc.left+ onwerRc.width;
					rc.top = onwerRc.y;
					rc.bottom = onwerRc.y + onwerRc.height;
					if(IsInRect(pt,rc))
					{
						pLeftLinePlayers[index]->PlayerControlProcessLButtonBLCLK();
						return ;
					}
					if(pLeftLinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
					{
						return;
					}
				}
				

			}
		}
		else
		{
			int numbers = numberOnlinePlayers+numberLeftLinePlayers;
			for(index =0,i = drawIndex; i < numbers;i++,index++)
			{
				const ONRECT& onwerRc = pAllPlayers[i]->PlayerControlGetPosRect();
				RECT rc;
				rc.left = onwerRc.x;
				rc.right = rc.left+ onwerRc.width;
				rc.top = onwerRc.y;
				rc.bottom = onwerRc.y + onwerRc.height;
				if(IsInRect(pt,rc))
				{
						pAllPlayers[i]->PlayerControlProcessLButtonBLCLK();
						return ;
				}
				if(pAllPlayers[i]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
				{
						return;
				}
			}
		}
	}
	else
	{
		if(drawIndex >= numberOnlinePlayers)
			return;
		for(i = 0,index = drawIndex;index < numberOnlinePlayers ;index++,i++)
		{
			const ONRECT& onwerRc = pOnlinePlayers[index]->PlayerControlGetPosRect();
			RECT rc;
			rc.left = onwerRc.x;
			rc.right = rc.left+ onwerRc.width;
			rc.top = onwerRc.y;
			rc.bottom = onwerRc.y + onwerRc.height;
			if(IsInRect(pt,rc))
			{
				pOnlinePlayers[index]->PlayerControlProcessLButtonBLCLK();
				return ;
			}
			if( pOnlinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
			{
						return;
			}

		}
		
	}
}

void PlayerInfoDlg::PlayerInfoDlgProcessMouseLButtonUp(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	titleControl.TitleControlProcessLButtonUp();
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	if(scroll_bar.ChatScrollBarIsSetCapture())
	{
		scroll_bar.ChatScrollBarReleaseCapture();
		ReleaseCapture();
	}
	titleControl.TitleControlProcessMouseMove(*this,pt);
}
void PlayerInfoDlg::PlayerInfoDlgProcessMouseLButtonDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(false);
	ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(false);
	if(scroll_bar.ChatScrollBarProcessClickUpButton(pt))
	{
		if(drawIndex > 0)
			drawIndex--;
		if(drawIndex<0)
			drawIndex = 0;
		PlayerInfoDlgUpdateDrawArea();
		return;
		
	}
	else
	if(scroll_bar.ChatScrollBarProcessClickDownButton(pt))
	{
		if(state&_PLAYER_LIST_CONTROL_SHOW_LEFT)
		{
			if(drawIndex==numberLeftLinePlayers+numberOnlinePlayers-1)
				return;
			drawIndex++;
			if(drawIndex>numberLeftLinePlayers+numberOnlinePlayers-1)
				drawIndex = numberOnlinePlayers+numberLeftLinePlayers-1;
		}
		else
		{
			if(drawIndex == numberOnlinePlayers-1)
				return;
			drawIndex++;
			if(drawIndex > numberOnlinePlayers-1)
				drawIndex = numberOnlinePlayers -1;
		}
		PlayerInfoDlgUpdateDrawArea();
		return;
	}
	titleControl.TitleControlProcessLButtonDown(*this,pt);
	if(scroll_bar.ChatScrollBarProcessClickScrollBar(pt))
		return;
	int index = 0,i = 0;
	if(state&_PLAYER_LIST_CONTROL_SHOW_LEFT)
	{
		if(state&_PLAYER_LIST_CONTROL_NORMAL_SHOW)
		{
			if(drawIndex>=numberOnlinePlayers)
			{
				for(i  = 0,index = drawIndex - numberOnlinePlayers; index<numberLeftLinePlayers;index++,i++)
				{
					const ONRECT& onwerRc = pLeftLinePlayers[index]->PlayerControlGetPosRect();
					RECT rc;
					rc.left = onwerRc.x;
					rc.right = rc.left+ onwerRc.width;
					rc.top = onwerRc.y;
					rc.bottom = onwerRc.y + onwerRc.height;
					if(IsInRect(pt,rc))
					{
						pLeftLinePlayers[index]->PlayerControlSetSelected(TRUE, TRUE);
			//			pLeftLinePlayers[index]->PlayerControlProcessLButtonDown();
			//			return ;
					}
					else
					{
						pLeftLinePlayers[index]->PlayerControlSetSelected(FALSE, TRUE);
					}
					if(pLeftLinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
					{
						break;
					}
					
				}
			}
			else
			{
				for(i = 0,index = drawIndex;index < numberOnlinePlayers ;index++,i++)
				{
					const ONRECT& onwerRc = pOnlinePlayers[index]->PlayerControlGetPosRect();
					RECT rc;
					rc.left = onwerRc.x;
					rc.right = rc.left+ onwerRc.width;
					rc.top = onwerRc.y;
					rc.bottom = onwerRc.y + onwerRc.height;
					if(IsInRect(pt,rc))
					{
						pOnlinePlayers[index]->PlayerControlSetSelected(TRUE, TRUE);
			//			pOnlinePlayers[index]->PlayerControlProcessLButtonDown();
			//			return ;
					}
					else
					{
						pOnlinePlayers[index]->PlayerControlSetSelected(FALSE, TRUE);
					}
					if( pOnlinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
					{
						return;
					}
				}
				for(i= 0,index = 0;index <numberLeftLinePlayers;index++,i++)
				{
					const ONRECT& onwerRc = pLeftLinePlayers[index]->PlayerControlGetPosRect();
					RECT rc;
					rc.left = onwerRc.x;
					rc.right = rc.left+ onwerRc.width;
					rc.top = onwerRc.y;
					rc.bottom = onwerRc.y + onwerRc.height;
					if(IsInRect(pt,rc))
					{
						pLeftLinePlayers[index]->PlayerControlSetSelected(TRUE, TRUE);
			//			pLeftLinePlayers[index]->PlayerControlProcessLButtonDown();
			//			return ;
					}
					else
					{
						pLeftLinePlayers[index]->PlayerControlSetSelected(FALSE, TRUE);
					}
					if(pLeftLinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
					{
						return;
					}
				}
				

			}
		}
		else
		{
			int numbers = numberOnlinePlayers+numberLeftLinePlayers;
			for(index =0,i = drawIndex; i < numbers;i++,index++)
			{
				const ONRECT& onwerRc = pAllPlayers[i]->PlayerControlGetPosRect();
				RECT rc;
				rc.left = onwerRc.x;
				rc.right = rc.left+ onwerRc.width;
				rc.top = onwerRc.y;
				rc.bottom = onwerRc.y + onwerRc.height;
				if(IsInRect(pt,rc))
				{
					pAllPlayers[i]->PlayerControlSetSelected(TRUE, TRUE);
		//			pAllPlayers[i]->PlayerControlProcessLButtonDown();
		//			return ;
				}
				else
				{
					pAllPlayers[i]->PlayerControlSetSelected(FALSE, TRUE);
				}
				if(pAllPlayers[i]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
				{
						return;
				}
			}
		}
	}
	else
	{
		if(drawIndex >= numberOnlinePlayers)
			return;
		for(i = 0,index = drawIndex;index < numberOnlinePlayers ;index++,i++)
		{
			const ONRECT& onwerRc = pOnlinePlayers[index]->PlayerControlGetPosRect();
			RECT rc;
			rc.left = onwerRc.x;
			rc.right = rc.left+ onwerRc.width;
			rc.top = onwerRc.y;
			rc.bottom = onwerRc.y + onwerRc.height;
			if(IsInRect(pt,rc))
			{
				pOnlinePlayers[index]->PlayerControlSetSelected(TRUE, TRUE);
	//			pOnlinePlayers[index]->PlayerControlProcessLButtonDown();
	//			return ;
			}
			else
			{
				pOnlinePlayers[index]->PlayerControlSetSelected(FALSE, TRUE);
			}
			if( pOnlinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
			{
				return;
			}

		}
		
	}
	

}
void PlayerInfoDlg::PlayerInfoDlgDeletePlayers()
{
	BOOL isDelete = FALSE;
	for(int i  = 0;i<numberOnlinePlayers;i++)
	{
		if(pOnlinePlayers[i]->PlayerControlIsSelected())
		{
			g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, (UINT)pOnlinePlayers[i]->PlayerControlGetPlayerInfo().playerName, NULL );
			isDelete = TRUE;
		}
	}
	for(int j = 0; j < numberLeftLinePlayers;j++)
	{
		if(pLeftLinePlayers[j]->PlayerControlIsSelected())
		{
			g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, (UINT)pLeftLinePlayers[j]->PlayerControlGetPlayerInfo().playerName, NULL );
			isDelete = TRUE;
		}
	}
}
void PlayerInfoDlg::PlayerInfoDlgProcessBTNCommand(HWND hwnd ,WPARAM wParam,LPARAM lParam)
{
	
	int id = LOWORD(wParam);
	switch(id)
	{
	case _PLAYER_DELETE_BUTTON_ID:
		{
			
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(false);
			if(ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgIsShow())
			{
				BringWindowToTop(ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.hDlg);
				return;
			}
			if(HIWORD(wParam) == BN_CLICKED)
			{
				
				for(int i  = 0;i<numberOnlinePlayers;i++)
				{
					if(pOnlinePlayers[i]->PlayerControlIsSelected())
					{
						ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(TRUE);
						return;
					}
				}
				for(int j = 0; j < numberLeftLinePlayers;j++)
				{
					if(pLeftLinePlayers[j]->PlayerControlIsSelected())
					{

						ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(TRUE);

						return;
					}
				}
				
			}
		}
		return ;
	case _PLAYER_ADD_BUTTON_ID:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(false);
			if(ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgIsShow())
			{
				BringWindowToTop(ChatFriendPanelManager::ChatFriendManagerGet().addDlg.hDlg);
				return;
			}
			if(HIWORD(wParam) == BN_CLICKED)
			{
				ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(TRUE);		
			}
		}
		return;
	case _CHAT_SHOW_LEFT_BUTTON_ID:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(false);
		    ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(false);
			if(HIWORD(wParam)== BN_CLICKED)
			{
				PlayerInfoDlg* pDlg = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg();
				if(pDlg->showLeftButton.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN)
				{
					pDlg->showLeftButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
					state&=~_PLAYER_LIST_CONTROL_SHOW_LEFT;
					PlayerInfoQSort(_PLAYER_LIST_SORT_BYNAME);
					int controlHeight = ChatFriendPanelManager::ChatFriendManagerGet().controlHeight;
					int height = 0;
					int iter = pDlg->intermission;
					for(int i = 0; i < pDlg->numberOnlinePlayers;i++)
					{
						height += controlHeight;
						height += iter;
					}
					pDlg->scroll_bar.ChatScrollBarSetScrolls(height,controlHeight+iter);
					pDlg->scroll_bar.ChatScrolBarUpdate();
					PlayerInfoDlgUpdateDrawArea();
					
				}
				else
				{
					pDlg->showLeftButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
					state|=_PLAYER_LIST_CONTROL_SHOW_LEFT;
					state|=_PLAYER_LIST_CONTROL_NORMAL_SHOW;
					PlayerInfoQSort(_PLAYER_LIST_SORT_BYNAME);
					int controlHeight = ChatFriendPanelManager::ChatFriendManagerGet().controlHeight;
					int height = 0;
					int iter = pDlg->intermission;
					for(int i = 0; i < pDlg->numberOnlinePlayers;i++)
					{
						height += controlHeight;
						height += iter;
					}
					for(i = 0; i < pDlg->numberLeftLinePlayers;i++)
					{
						height+=controlHeight;
						height += iter;
					}
					pDlg->scroll_bar.ChatScrollBarSetScrolls(height,controlHeight+iter);
					pDlg->scroll_bar.ChatScrolBarUpdate();
					PlayerInfoDlgUpdateDrawArea();
					
				}
				pDlg->showLeftButton.ChatWndUpdate();
			}
		}
		return;
	}
}
void PlayerInfoDlg::PlayerInfoDlgProcessClearControl()
{
	for(int i = 0; i < numberOnlinePlayers ; i ++)
		pOnlinePlayers[i] = 0;
	numberOnlinePlayers = 0;
	for(int j = 0; j <numberLeftLinePlayers;j++)
		pLeftLinePlayers[i] = 0;
	numberLeftLinePlayers = 0;
	allFriendsLength = 0;
}

void PlayerInfoDlg::PlayerInfoDlgProcessMouseWheel(HWND hwnd,WPARAM wParam,LPARAM lParam)
{

	if(numberLeftLinePlayers+numberOnlinePlayers == 0)
		return ;
	short zDelta = HIWORD(wParam);
	if(zDelta >= 120)
	{
		if(drawIndex ==  0)
			return ;
		drawIndex--;
		if(drawIndex<0)
			drawIndex = 0;
		else
			scroll_bar.ChatScrocllBarMoveUp();
	}
	else
	{
		if(state&_PLAYER_LIST_CONTROL_SHOW_LEFT)
		{
			if(drawIndex==numberLeftLinePlayers+numberOnlinePlayers-1)
				return;
			drawIndex++;
			if(drawIndex>numberLeftLinePlayers+numberOnlinePlayers-1)
				drawIndex = numberOnlinePlayers+numberLeftLinePlayers-1;
			else
				scroll_bar.ChatScrollBarMoveDown();
		}
		else
		{
			if(numberOnlinePlayers == 0)
				return;
			if(drawIndex == numberOnlinePlayers-1)
				return;
			drawIndex++;
			if(drawIndex > numberOnlinePlayers-1)
				drawIndex = numberOnlinePlayers -1;
			else
				scroll_bar.ChatScrollBarMoveDown();
		}
	}
	this->PlayerInfoDlgUpdateDrawArea();
}
void PlayerInfoDlg::PlayerInfoDlgProcessMouseLeave(HWND hwnd ,WPARAM wParam,LPARAM lParam)
{


}
void PlayerInfoDlg::PlayerInfoDlgProcessMouseMove(HWND hwnd,WPARAM wParam,LPARAM lParam)
{

	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	int index = 0;
/*	for(;index < numberOnlinePlayers;index++)
	{
		const ONRECT& onwerRc = pOnlinePlayers[index]->PlayerControlGetPosRect();
		RECT rc;
		rc.left = onwerRc.x;
		rc.right = rc.left+ onwerRc.width;
		rc.top = onwerRc.y;
		rc.bottom = onwerRc.y + onwerRc.height;
		if(IsInRect(pt,rc))
		{
			pOnlinePlayers[index]->PlayerControlProcessMouseMove();
		}
		else
		{
			pOnlinePlayers[index]->PlayerControlProcessMouseLeave();
		}
		
	}*/
	titleControl.TitleControlProcessMouseMove(*this,pt);
	switch(scroll_bar.ChatScrollBarProcessMouseMove(pt))
	{
	case -1:
		if(drawIndex > 0)
			drawIndex--;
		if(drawIndex<0)
			drawIndex = 0;
		PlayerInfoDlgUpdateDrawArea();
		return;
	case 1:
		if(state&_PLAYER_LIST_CONTROL_SHOW_LEFT)
		{
			if(drawIndex==numberLeftLinePlayers+numberOnlinePlayers-1)
				return;
			drawIndex++;
			if(drawIndex>numberLeftLinePlayers+numberOnlinePlayers-1)
				drawIndex = numberOnlinePlayers+numberLeftLinePlayers-1;
		}
		else
		{
			if(drawIndex == numberOnlinePlayers-1)
				return;
			drawIndex++;
			if(drawIndex > numberOnlinePlayers-1)
				drawIndex = numberOnlinePlayers -1;
		}
		PlayerInfoDlgUpdateDrawArea();
		return;
		
	}

}
void PlayerInfoDlg::PlayerInfoProcessDeleteButtonDown()
{
	ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(false);
	if(ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgIsShow())
	{
		BringWindowToTop(ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.hDlg);
				return;
	}
				
	for(int i  = 0;i<numberOnlinePlayers;i++)
	{
		if(pOnlinePlayers[i]->PlayerControlIsSelected())
		{
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(TRUE);
			return;
		}
	}
	for(int j = 0; j < numberLeftLinePlayers;j++)
	{
		if(pLeftLinePlayers[j]->PlayerControlIsSelected())
		{

			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(TRUE);

			return;
		}
	}
}
void PlayerInfoDlg::PlayerInfoProcessAddButtonDown()
{
	ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(false);
	if(ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgIsShow())
	{
				BringWindowToTop(ChatFriendPanelManager::ChatFriendManagerGet().addDlg.hDlg);
				return;
	}
	{
		ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(TRUE);		
	}
}

void PlayerInfoDlg::PlayerInfoProcessShowLeftDown()
{
	ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(false);
	ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(false);
	PlayerInfoDlg* pDlg = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg();
	if(pDlg->showLeftButton.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN)
	{
		pDlg->showLeftButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
		state&=~_PLAYER_LIST_CONTROL_SHOW_LEFT;
		PlayerInfoQSort(_PLAYER_LIST_SORT_BYNAME);
		int controlHeight = ChatFriendPanelManager::ChatFriendManagerGet().controlHeight;
		int height = 0;
		int iter = pDlg->intermission;
		for(int i = 0; i < pDlg->numberOnlinePlayers;i++)
		{
						height += controlHeight;
						height += iter;
		}
		pDlg->scroll_bar.ChatScrollBarSetScrolls(height,controlHeight+iter);
		pDlg->scroll_bar.ChatScrolBarUpdate();
		PlayerInfoDlgUpdateDrawArea();
			
	}
	else
	{
		pDlg->showLeftButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
		state|=_PLAYER_LIST_CONTROL_SHOW_LEFT;
		state|=_PLAYER_LIST_CONTROL_NORMAL_SHOW;
		PlayerInfoQSort(_PLAYER_LIST_SORT_BYNAME);
		int controlHeight = ChatFriendPanelManager::ChatFriendManagerGet().controlHeight;
		int height = 0;
		int iter = pDlg->intermission;
		for(int i = 0; i < pDlg->numberOnlinePlayers;i++)
		{
			height += controlHeight;
			height += iter;
		}
		for(i = 0; i < pDlg->numberLeftLinePlayers;i++)
		{
			height+=controlHeight;
			height += iter;
		}
		pDlg->scroll_bar.ChatScrollBarSetScrolls(height,controlHeight+iter);
		pDlg->scroll_bar.ChatScrolBarUpdate();
		PlayerInfoDlgUpdateDrawArea();
					
	}
	pDlg->showLeftButton.ChatWndUpdate();
}

LRESULT CALLBACK PlayerInfoDlg::PlayerShowLeftButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatFriendPanelManager& panelManager= ChatFriendPanelManager::ChatFriendManagerGet();
	PlayerInfoDlg* pDlg = 0;
	for(int i = 0; i <_PLAYER_INFO_PAGE_NUM;i++)
	{
		if(panelManager.playerInfoDlg[i].hDlg == hwnd)
		{
			pDlg = &panelManager.playerInfoDlg[i];
			break;
		}
	}
	if(pDlg == 0)
		return FALSE;
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		{
			pDlg->PlayerInfoProcessShowLeftDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);

		}
		return FALSE;

	}

	return CallWindowProc(pDlg->showLeftButtonProc,hwnd,msg,wParam,lParam);

}
BOOL CALLBACK PlayerInfoDlg::PlayerInfoDlfProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatFriendPanelManager& panelManager= ChatFriendPanelManager::ChatFriendManagerGet();
	int id =0;
	PlayerInfoDlg* pDlg = 0;
	for(int i = 0; i <_PLAYER_INFO_PAGE_NUM;i++)
	{
		if(panelManager.playerInfoDlg[i].hDlg == hwnd)
		{
			id = i;	
			pDlg = &panelManager.playerInfoDlg[i];
			break;
		}
	}
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_PAINT:
		{
			ShowWindow(hwnd,SW_HIDE);
			return FALSE;
			PAINTSTRUCT ps;
			HDC  hdc = BeginPaint(hwnd,&ps);
			pDlg->PlayerInfoDlgProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			pDlg->PlayerInfoDlgProcessPaint((HDC)wParam);
		}
		return TRUE;
	case WM_MOUSEMOVE:
		{
			pDlg->PlayerInfoDlgProcessMouseMove(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			pDlg->PlayerInfoDlgProcessMouseLButtonDown(hwnd,wParam,lParam);
			SetFocus(hwnd);
		}
		return FALSE;
	case WM_LBUTTONUP:
		{
			pDlg->PlayerInfoDlgProcessMouseLButtonUp(hwnd,wParam,lParam);
			SetFocus(ChatControlPanel::ChatPanelGetPanel().hPanelDlg);
		}
		return FALSE;
	case WM_DRAWITEM:
		{
			pDlg->PlayerInfoDlgProcessDrawButton((LPDRAWITEMSTRUCT)lParam);	
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			pDlg->PlayerInfoDlgProcessMouseLeave(hwnd ,wParam,lParam);
		}
		return FALSE;
	case WM_MOUSEWHEEL:
		{
			pDlg->PlayerInfoDlgProcessMouseWheel(hwnd , wParam , lParam);
		}
		return FALSE;
	case WM_COMMAND:
		{
			pDlg->PlayerInfoDlgProcessBTNCommand(hwnd,wParam,lParam);

		}
		return FALSE;
	case WM_LBUTTONDBLCLK:
		{
			
			pDlg->PlayerInfoProcessLButtonDBCLK(hwnd,wParam,lParam);
	//		SetFocus(hwnd);
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_DESTROY:
		{
		
		
		}
		return FALSE;

	}
	return FALSE;
}
void PlayerInfoDlg::PlayerInfoDlgAdjustWnd()
{
	RECT rc;
	GetWindowRect(ChatMainDlg::hMainDlg,&rc);
	RECT rClient;
	GetClientRect(ChatMainDlg::hMainDlg,&rClient);
	AdjustWindowRectEx(&rClient,GetWindowStyle(ChatMainDlg::hMainDlg),
			               GetMenu(ChatMainDlg::hMainDlg)!=NULL,GetWindowExStyle(ChatMainDlg::hMainDlg));
	int dx = rClient.left;
	int dy = rClient.top;
	int x = this->x-dx+rc.left;
	int y = this->y-dy+rc.top;
	MoveWindow(hDlg,x,y,width,height,TRUE);
}

int  PlayerInfoDlg::PlayersQsortByName(const void* p1,const void* p2)
{
	LPPLAYERCONTROL pControl1 = *((LPPLAYERCONTROL*)p1);
	LPPLAYERCONTROL pControl2 = *((LPPLAYERCONTROL*)p2);
	if(strcmp((CHAR*)pControl1->PlayerControlGetPlayerInfo().playerName,(CHAR*)pControl2->PlayerControlGetPlayerInfo().playerName)>=0)
		return 1;
	return -1;
}
int PlayerInfoDlg::PlayersQsortByMetier(const void* p1,const void* p2)
{
	LPPLAYERCONTROL pControl1 = *((LPPLAYERCONTROL*)p1);
	LPPLAYERCONTROL pControl2 = *((LPPLAYERCONTROL*)p2);
	if(strcmp((CHAR*)pControl1->PlayerControlGetPlayerInfo().playerMetier,(CHAR*)pControl2->PlayerControlGetPlayerInfo().playerMetier)>=0)
		return 1;
	return -1;
}
int PlayerInfoDlg::PlayersQsortByLevel(const void* p1,const void* p2)
{
	LPPLAYERCONTROL pControl1 = *((LPPLAYERCONTROL*)p1);
	LPPLAYERCONTROL pControl2 = *((LPPLAYERCONTROL*)p2);
//	if(pControl1->PlayerControlGetPlayerInfo().playerLevel == 0)
//		return -1;
	if(pControl1->PlayerControlGetPlayerInfo().playerLevel>pControl2->PlayerControlGetPlayerInfo().playerLevel)
		return 1;
	return -1;
}

int PlayerInfoDlg::PlayersQsortByGroup(const void* p1,const void* p2)
{
	LPPLAYERCONTROL pControl1 = *((LPPLAYERCONTROL*)p1);
	LPPLAYERCONTROL pControl2 = *((LPPLAYERCONTROL*)p2);
//	if(strcmp((CHAR*)pControl1->PlayerControlGetPlayerInfo().PlayerGroup,(CHAR*)pControl2->PlayerControlGetPlayerInfo().PlayerGroup)>=0)
	//	return 1;
	return -1;
}
int PlayerInfoDlg::PlayersQsortByPlace(const void* p1,const void* p2)
{
	LPPLAYERCONTROL pControl1 = *((LPPLAYERCONTROL*)p1);
	LPPLAYERCONTROL pControl2 = *((LPPLAYERCONTROL*)p2);
	if(strcmp((CHAR*)pControl1->PlayerControlGetPlayerInfo().playerPlace,(CHAR*)pControl2->PlayerControlGetPlayerInfo().playerPlace)>=0)
		return 1;
	return -1;
}
void PlayerInfoDlg::PlayerInfoQSort(int method)
{
	if(state&_PLAYER_LIST_CONTROL_SHOW_LEFT)
	{
		if(state&_PLAYER_LIST_CONTROL_NORMAL_SHOW)
		{
			if(numberOnlinePlayers>0)
			{
				if(method == _PLAYER_LIST_SORT_BYNAME)
					qsort(pOnlinePlayers,numberOnlinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByName);
				else
				if(method == _PLAYER_LIST_SORT_BYMETIER)
					qsort(pOnlinePlayers,numberOnlinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByMetier);
				else
				if(method == _PLAYER_LIST_SORT_BYLEVEL)
					qsort(pOnlinePlayers,numberOnlinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByLevel);
				else
				if(method == _PLAYER_LIST_SORT_BYGROUP)
					qsort(pOnlinePlayers,numberOnlinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByGroup);
				else
					qsort(pOnlinePlayers,numberOnlinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByPlace);
			}
			if(numberLeftLinePlayers>0)
			{
				if(method == _PLAYER_LIST_SORT_BYNAME)
					qsort(pLeftLinePlayers,numberLeftLinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByName);
				else
				if(method == _PLAYER_LIST_SORT_BYMETIER)
					qsort(pLeftLinePlayers,numberLeftLinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByMetier);
				else
				if(method == _PLAYER_LIST_SORT_BYLEVEL)
					qsort(pLeftLinePlayers,numberLeftLinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByLevel);
				else
				if(method == _PLAYER_LIST_SORT_BYGROUP)
					qsort(pLeftLinePlayers,numberLeftLinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByGroup);
				else
					qsort(pLeftLinePlayers,numberLeftLinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByPlace);
			}
			drawIndex = 0;
			return;
		}
		else
		{
			if(numberLeftLinePlayers+numberOnlinePlayers>0)
			{
				PlayerInfoMakeMixAllPlayers();
				if(method == _PLAYER_LIST_SORT_BYNAME)
					qsort(pAllPlayers,numberOnlinePlayers+numberLeftLinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByName);
				else
				if(method == _PLAYER_LIST_SORT_BYMETIER)
					qsort(pAllPlayers,numberOnlinePlayers+numberLeftLinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByMetier);
				else
				if(method == _PLAYER_LIST_SORT_BYLEVEL)
					qsort(pAllPlayers,numberOnlinePlayers+numberLeftLinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByLevel);
				else
				if(method == _PLAYER_LIST_SORT_BYGROUP)
					qsort(pAllPlayers,numberOnlinePlayers+numberLeftLinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByGroup);
				else
					qsort(pAllPlayers,numberOnlinePlayers+numberLeftLinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByPlace);
				
			}
			drawIndex = 0;
			return;			
		}
	}
	else
	{
		if(numberOnlinePlayers>0)
			{
				if(method == _PLAYER_LIST_SORT_BYNAME)
					qsort(pOnlinePlayers,numberOnlinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByName);
				else
				if(method == _PLAYER_LIST_SORT_BYMETIER)
					qsort(pOnlinePlayers,numberOnlinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByMetier);
				else
				if(method == _PLAYER_LIST_SORT_BYLEVEL)
					qsort(pOnlinePlayers,numberOnlinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByLevel);
				else
				if(method == _PLAYER_LIST_SORT_BYGROUP)
					qsort(pOnlinePlayers,numberOnlinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByGroup);
				else
					qsort(pOnlinePlayers,numberOnlinePlayers,sizeof(LPPLAYERCONTROL),PlayerInfoDlg::PlayersQsortByPlace);
			}
		drawIndex = 0;
		return;
	}
	


}
void PlayerInfoDlg::PlayerInfoDlgProcessPaint(HDC hdc)
{
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hTemp = CreateCompatibleBitmap(hdc,width,height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcBuffer,hTemp);
	DrawBitmap(hdcBuffer,ChatResource::GetSingle().GetResource(bitmapIdx)->hBitmap,width,height);
	DrawBitmap(hdcBuffer,ChatResource::GetSingle().GetResource(drawAreaBKBitmapIdx)->hBitmap,drawAreaWidth ,drawAreaHeight,drawAreaX,drawAreaY);
	int index = 0;
	int controlHeight = ChatFriendPanelManager::ChatFriendManagerGet().controlHeight;
	int i ;
	bool isUseDefaultFont = ChatFriendPanelManager::ChatFriendManagerGet().isUseDefaultFont;
	if(ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgIsShow())
		BringWindowToTop(ChatFriendPanelManager::ChatFriendManagerGet().addDlg.hDlg);
	if(ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgIsShow())
		BringWindowToTop(ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.hDlg);
	if(state&_PLAYER_LIST_CONTROL_SHOW_LEFT)
	{
		if(state&_PLAYER_LIST_CONTROL_NORMAL_SHOW)
		{
			if(drawIndex>=numberOnlinePlayers)
			{
				for(i  = 0,index = drawIndex - numberOnlinePlayers; index<numberLeftLinePlayers;index++,i++)
				{
					pLeftLinePlayers[index]->PlayerControlGetPosRectControl().x = drawPointX;
					pLeftLinePlayers[index]->PlayerControlGetPosRectControl().y = drawPointY + (controlHeight+intermission)*i;
					int rectHieght = pLeftLinePlayers[index]->PlayerControlGetPosRect().height;
					if(pLeftLinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission - rectHieght)
					{
						titleControl.TitleControlDrawItem(hdcBuffer);
						scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
						BitBlt(hdc,0,0,width,height,hdcBuffer,0,0,SRCCOPY);
						SelectObject(hdcBuffer,hOldBitmap);
						DeleteObject(hTemp);
						DeleteDC(hdcBuffer);
						return ;
					}
					pLeftLinePlayers[index]->PlayerControlDrawItem(hdcBuffer,isUseDefaultFont);
				}
			}
			else
			{
				for(i = 0,index = drawIndex;index < numberOnlinePlayers ;index++,i++)
				{
					pOnlinePlayers[index]->PlayerControlGetPosRectControl().x = drawPointX;
					pOnlinePlayers[index]->PlayerControlGetPosRectControl().y = drawPointY + (controlHeight+intermission)*i;
					int rectHeight = pOnlinePlayers[index]->PlayerControlGetPosRect().height;
					if(pOnlinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission - rectHeight)
					{
						titleControl.TitleControlDrawItem(hdcBuffer);
						scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
						BitBlt(hdc,0,0,width,height,hdcBuffer,0,0,SRCCOPY);
						SelectObject(hdcBuffer,hOldBitmap);
						DeleteObject(hTemp);
						DeleteDC(hdcBuffer);
						return;
					}
					pOnlinePlayers[index]->PlayerControlDrawItem(hdcBuffer,isUseDefaultFont);
				}
				int drawY = pOnlinePlayers[numberOnlinePlayers-1]->PlayerControlGetPosRect().y+intermission+controlHeight;
				for(i= 0,index = 0;index <numberLeftLinePlayers;index++,i++)
				{
					pLeftLinePlayers[index]->PlayerControlGetPosRectControl().x = drawPointX;
					pLeftLinePlayers[index]->PlayerControlGetPosRectControl().y = drawY + (controlHeight+intermission)*i;
					int rectHeight = pLeftLinePlayers[index]->PlayerControlGetPosRect().height;
					if(pLeftLinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission - rectHeight)
					{
						titleControl.TitleControlDrawItem(hdcBuffer);
						scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
						BitBlt(hdc,0,0,width,height,hdcBuffer,0,0,SRCCOPY);
						SelectObject(hdcBuffer,hOldBitmap);
						DeleteObject(hTemp);
						DeleteDC(hdcBuffer);
						return ;
					}
					pLeftLinePlayers[index]->PlayerControlDrawItem(hdcBuffer,isUseDefaultFont);
				}
			}
		}
		else
		{
			int numbers = numberOnlinePlayers+numberLeftLinePlayers;
			for(index =0,i = drawIndex; i < numbers;i++,index++)
			{
				pAllPlayers[i]->PlayerControlGetPosRectControl().x = drawPointX;
				pAllPlayers[i]->PlayerControlGetPosRectControl().y = drawPointY + (controlHeight+intermission)*index;
				int rectHeight = pAllPlayers[i]->PlayerControlGetPosRect().height;
				if(pAllPlayers[i]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission - rectHeight)
				{
					titleControl.TitleControlDrawItem(hdcBuffer);
					scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
					BitBlt(hdc,0,0,width,height,hdcBuffer,0,0,SRCCOPY);
					SelectObject(hdcBuffer,hOldBitmap);
					DeleteObject(hTemp);
					DeleteDC(hdcBuffer);
					return ;
				}
			pAllPlayers[i]->PlayerControlDrawItem(hdcBuffer,isUseDefaultFont);
			}
		}
	}
	else
	{
		if(drawIndex>=numberOnlinePlayers)
		{
			titleControl.TitleControlDrawItem(hdcBuffer);
			scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
			BitBlt(hdc,0,0,width,height,hdcBuffer,0,0,SRCCOPY);
			SelectObject(hdcBuffer,hOldBitmap);
			DeleteObject(hTemp);
			DeleteDC(hdcBuffer);
			return;
		}
		else
		{
			for(i = 0,index = drawIndex;index < numberOnlinePlayers ;index++,i++)
			{
				pOnlinePlayers[index]->PlayerControlGetPosRectControl().x = drawPointX;
				pOnlinePlayers[index]->PlayerControlGetPosRectControl().y = drawPointY + (controlHeight+intermission)*i;
				int rectHeight = pOnlinePlayers[index]->PlayerControlGetPosRect().height;
				if(pOnlinePlayers[index]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission - rectHeight)
				{
					    titleControl.TitleControlDrawItem(hdcBuffer);
						scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
						BitBlt(hdc,0,0,width,height,hdcBuffer,0,0,SRCCOPY);
						SelectObject(hdcBuffer,hOldBitmap);
						DeleteObject(hTemp);
						DeleteDC(hdcBuffer);
						return;
				}
				pOnlinePlayers[index]->PlayerControlDrawItem(hdcBuffer,isUseDefaultFont);
			}
		}
			
	}
	scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
	titleControl.TitleControlDrawItem(hdcBuffer);
	BitBlt(hdc,0,0,width,height,hdcBuffer,0,0,SRCCOPY);
	SelectObject(hdcBuffer,hOldBitmap);
	DeleteObject(hTemp);
	DeleteDC(hdcBuffer);
}

void PlayerInfoDlg::PlayerInfoDlfCreate(HWND hParent,DlgProcessFun pFun)
{
	if((hDlg =CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_LIST_DLG),
		hParent,
		(DLGPROC)pFun))==0)
		return ;
	PlayerInfoDlgShowDlg(FALSE);
}



void PlayerInfoDlg::PlayerInfoDlgLoadSRC(HWND hwnd)
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	iniFile.GetInteger(_PLAYER_INFO_INI_ITEM_NAME,_CHAT_SRC_X,0,&x);
	iniFile.GetInteger(_PLAYER_INFO_INI_ITEM_NAME,_CHAT_SRC_Y,0,&y);
	iniFile.GetInteger(_PLAYER_INFO_INI_ITEM_NAME,"width",0,&width);
	iniFile.GetInteger(_PLAYER_INFO_INI_ITEM_NAME,"height",0,&height);
	iniFile.GetInteger(_PLAYER_INFO_INI_ITEM_NAME,"BkSrcIdx",0,&bitmapIdx);


	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_CHAT_SRC_X,0,&drawPointX);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_CHAT_SRC_Y,0,&drawPointY);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_LIST_INI_SRC_DRAW_X,0,&drawAreaX);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_LIST_INI_SRC_DRAW_Y,0,&drawAreaY);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_LIST_INI_SRC_DRAW_WIDTH,0,&drawAreaWidth);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_LIST_INI_SRC_DRAW_HEIGHT,0,&drawAreaHeight);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_LIST_INI_SRC_DRAW_INTERMISSION,0,&intermission);

	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_FRIEND_NAME,"drawAreaBitmapIdx",0,&drawAreaBKBitmapIdx);
	{
		int x ,y,width,height,isClip;
		int r,g,b;
		int normal_idx = 0,hover_idx=0,pushed_idx=0;
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_DELETE,_CHAT_SRC_X,0,&x);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_DELETE,_CHAT_SRC_Y,0,&y);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_DELETE,_CHAT_SRC_WIDTH,0,&width);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_DELETE,_CHAT_SRC_HEIGHT,0,&height);

		deletePlayerButton.ChatWndCreate(_PLAYER_DELETE_BUTTON_ID,WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
			hwnd,"","button",x,y,width,height);

//		deleteButtonProc = (WNDPROC)SetWindowLong(deletePlayerButton.ChatWndGetHandle(),GWL_WNDPROC,(LONG)(PlayerInfoDlg::PlayerDeleteButtonProc));
		
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_DELETE,"normalSrcIdx",0,&normal_idx);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_DELETE,"mouseOverSrcIdx",0,&hover_idx);
		deletePlayerButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_DELETE,_CHAT_SRC_CLIP,0,&isClip);
		iniFile.GetString(_PLAYER_LIST_INI_SRC_DELETE,"tooltipInfo","",szValue,MAX_PATH);
	    if(szValue[0] != 0)
		   deletePlayerButton.ChatWndTipCreate(TTS_NOPREFIX,szValue,100);
		char deleteTextInfo[256]={0};
		iniFile.GetString(_PLAYER_LIST_INI_SRC_DELETE,"textInfo","",deleteTextInfo,256);
		if(deleteTextInfo[0]!=0)
		{
			deletePlayerButton.ChatWndSetText(deleteTextInfo);
			deletePlayerButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
			iniFile.GetString(_PLAYER_LIST_INI_SRC_DELETE,"fontColor","",szValue,MAX_PATH);
			sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			deletePlayerButton.ChatWndSetTextNormalColor(RGB(r,g,b));

			char font[FONT_SIZE]={0};
			iniFile.GetString(_PLAYER_LIST_INI_SRC_DELETE,"font","",font,FONT_SIZE);
			TFONT tFont;
			strcpy(tFont.fontName,font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0]=0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
				deletePlayerButton.ChatWndSetFont(font);
			else
			{
				deletePlayerButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				deletePlayerButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
		deletePlayerButton.SetWndProcessFun(ChatWndProcessFun::ProcessDeleteFriendListButton);

		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_ADD,_CHAT_SRC_X,0,&x);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_ADD,_CHAT_SRC_Y,0,&y);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_ADD,_CHAT_SRC_WIDTH,0,&width);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_ADD,_CHAT_SRC_HEIGHT,0,&height);

		addPlayerButton.ChatWndCreate(_PLAYER_ADD_BUTTON_ID,WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
			hwnd,"","button",x,y,width,height);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_ADD,"normalSrcIdx",0,&normal_idx);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_ADD,"mouseOverSrcIdx",0,&hover_idx);
		addPlayerButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
		iniFile.GetString(_PLAYER_LIST_INI_SRC_ADD,"tooltipInfo","",szValue,MAX_PATH);
	    if(szValue[0] != 0)
		   addPlayerButton.ChatWndTipCreate(TTS_NOPREFIX,szValue,100);
		char addTextInfo[256]={0};
		iniFile.GetString(_PLAYER_LIST_INI_SRC_ADD,"textInfo","",addTextInfo,256);
		if(addTextInfo[0]!=0)
		{
			addPlayerButton.ChatWndSetText(addTextInfo);
			addPlayerButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
			iniFile.GetString(_PLAYER_LIST_INI_SRC_ADD,"fontColor","",szValue,MAX_PATH);
			sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			addPlayerButton.ChatWndSetTextNormalColor(RGB(r,g,b));

			char font[FONT_SIZE]={0};
			iniFile.GetString(_PLAYER_LIST_INI_SRC_ADD,"font","",font,FONT_SIZE);
			TFONT tFont;
			strcpy(tFont.fontName,font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0]=0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
				addPlayerButton.ChatWndSetFont(font);
			else
			{
				addPlayerButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				addPlayerButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
		addPlayerButton.SetWndProcessFun(ChatWndProcessFun::ProcessAddFriendListButton);

		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_LEFTLINE,_CHAT_SRC_X,0,&x);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_LEFTLINE,_CHAT_SRC_Y,0,&y);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_LEFTLINE,_CHAT_SRC_WIDTH,0,&width);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_LEFTLINE,_CHAT_SRC_HEIGHT,0,&height);

		showLeftButton.ChatWndCreate(_CHAT_SHOW_LEFT_BUTTON_ID,WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
			hwnd,"","button",x,y,width,height);
		
		
//		showLeftButtonProc = (WNDPROC)SetWindowLong(showLeftButton.ChatWndGetHandle(),GWL_WNDPROC,(LONG)(PlayerInfoDlg::show));
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_LEFTLINE,"normalSrcIdx",0,&normal_idx);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_LEFTLINE,"mouseOverSrcIdx",0,&hover_idx);
		iniFile.GetInteger(_PLAYER_LIST_INI_SRC_LEFTLINE,"mouseDownSrcIdx",0,&pushed_idx);
		showLeftButton.ChatWndSetResource(normal_idx,hover_idx,pushed_idx,-1);
		//////////////////////////////////

		char showLeftTextInfo[256]={0};
		iniFile.GetString(_PLAYER_LIST_INI_SRC_LEFTLINE,"textInfo","",showLeftTextInfo,256);
		if(showLeftTextInfo[0]!=0)
		{
			showLeftButton.ChatWndSetText(showLeftTextInfo);
			showLeftButton.ChatWndSetAttr(_CHAT_WND_ATTR_CHECKED_BUTTON);
			iniFile.GetString(_PLAYER_LIST_INI_SRC_LEFTLINE,"fontColor","",szValue,MAX_PATH);
			sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			showLeftButton.ChatWndSetTextNormalColor(RGB(r,g,b));

			char font[FONT_SIZE]={0};
			iniFile.GetString(_PLAYER_LIST_INI_SRC_LEFTLINE,"font","",font,FONT_SIZE);
			TFONT tFont;
			strcpy(tFont.fontName,font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0]=0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
				showLeftButton.ChatWndSetFont(font);
			else
			{
				showLeftButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				showLeftButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
		
		
		showLeftButton.SetWndProcessFun(ChatWndProcessFun::ProcessShowLeftButton);
		showLeftButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
		showLeftButton.ChatWndUpdate();

		
	}
	titleControl.TitleControlInit();
	titleControl.TitleControlSetParentHandle(hwnd);
	scroll_bar.ChatScrollBarLoadIniCtg();
	scroll_bar.ChatScrollBarSetParent(hwnd);

	//初始化button
	//读取配置信息
	int button_Pos_X = 0;
	int button_Pos_Y = 0;
	int button_Width = 0;
	int button_Height = 0;
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_X,0,&button_Pos_X);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_Y,0,&button_Pos_Y);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_WIDTH,0,&button_Width);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_HEIGHT,0,&button_Height);

	//创建button
	
	m_wndHideShowBnt.ChatWndCreate(_HIDE_SHOW_BNT_ID, WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
		hwnd, "", "button", button_Pos_X, button_Pos_Y, button_Width, button_Height);
	//读取资源
	int normal_idx = 0;
	int hover_idx = 0;
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,"mouseOverSrcIdx",0,&hover_idx);
	m_wndHideShowBnt.ChatWndSetResource(normal_idx,hover_idx,-1,-1);

	//判断是否有字体
	char textInfo[256]={0};
	iniFile.GetString(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,"textInfo","",textInfo,256);
	if(textInfo[0]!=0)
	{
		m_wndHideShowBnt.ChatWndSetText(textInfo);
		m_wndHideShowBnt.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,"fontColor","",szValue,MAX_PATH);

		int r = 0;
		int g = 0;
		int b = 0;
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		m_wndHideShowBnt.ChatWndSetTextNormalColor(RGB(r,g,b));

		char font[FONT_SIZE]={0};
		iniFile.GetString(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,"font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_wndHideShowBnt.ChatWndSetFont(font);
		else
		{
			m_wndHideShowBnt.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_wndHideShowBnt.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	//设置button消息函数
	m_wndHideShowBnt.SetWndProcessFun(ChatWndProcessFun::ProcessFriendHideShowButton);
	m_wndHideShowBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
	m_wndHideShowBnt.ChatWndUpdate();

	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_X,0,&button_Pos_X);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_Y,0,&button_Pos_Y);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_WIDTH,0,&button_Width);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_HEIGHT,0,&button_Height);

	m_wndChangeBnt.ChatWndCreate(_CHANGE_WND_ID, WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
		hwnd, "", "button", button_Pos_X, button_Pos_Y, button_Width, button_Height);

	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,"mouseOverSrcIdx",0,&hover_idx);
	m_wndChangeBnt.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	memset(textInfo, 0, 256);
	iniFile.GetString(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,"textInfo","",textInfo,256);
	if(textInfo[0]!=0)
	{
		m_wndChangeBnt.ChatWndSetText(textInfo);
		m_wndChangeBnt.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,"fontColor","",szValue,MAX_PATH);

		int r = 0;
		int g = 0;
		int b = 0;
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		m_wndChangeBnt.ChatWndSetTextNormalColor(RGB(r,g,b));

		char font[FONT_SIZE]={0};
		iniFile.GetString(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,"font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_wndChangeBnt.ChatWndSetFont(font);
		else
		{
			m_wndChangeBnt.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_wndChangeBnt.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	m_wndChangeBnt.SetWndProcessFun(ChatWndProcessFun::ProcessFriendChangeButton);
	m_wndChangeBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
	m_wndChangeBnt.ChatWndUpdate();
}
void PlayerInfoDlg::PlayerInfoDlgProcessDrawButton(LPDRAWITEMSTRUCT lpdis)
{
	
}
void PlayerInfoDlg::PlayerInfoDlgAdd(LPPLAYERCONTROL pControl)
{
	ChatPlayerInfo& playerInfo = pControl->PlayerControlGetPlayerInfo();
	pControl->PlayerControlGetPosRectControl().x = drawPointX;
	if(playerInfo.isPlayerOnline)
	{
		pControl->PlayerControlSetState(_PLAYER_CONTROL_STATE_NORMAL);
		pOnlinePlayers[numberOnlinePlayers] = pControl;
		if(numberOnlinePlayers!=0)
			pControl->PlayerControlGetPosRectControl().y=pOnlinePlayers[numberOnlinePlayers -1]->PlayerControlGetPosRectControl().y+
			pOnlinePlayers[numberOnlinePlayers-1]->PlayerControlGetPosRectControl().height;
		else
		{
			pControl->PlayerControlGetPosRectControl().y = drawPointY;
		}
		allFriendsLength += intermission+ChatFriendPanelManager::ChatFriendManagerGet().controlHeight;
		numberOnlinePlayers++;
	}
	else
	{
		pControl->PlayerControlSetState(_PLAYER_CONTROL_STATE_LEFTLINE);
		pLeftLinePlayers[numberLeftLinePlayers] = pControl;
		if(numberLeftLinePlayers!=0)
			pControl->PlayerControlGetPosRectControl().y=pLeftLinePlayers[numberLeftLinePlayers -1]->PlayerControlGetPosRectControl().y+
			pLeftLinePlayers[numberLeftLinePlayers-1]->PlayerControlGetPosRectControl().height;
		else
		{
			if(numberOnlinePlayers !=0)
			{
				pControl->PlayerControlGetPosRectControl().y = pOnlinePlayers[numberOnlinePlayers-1]->PlayerControlGetPosRect().y +
				pOnlinePlayers[numberOnlinePlayers-1]->PlayerControlGetPosRect().height;
			}
			else
			{
				pControl->PlayerControlGetPosRectControl().y = drawPointY;
			}
		}
		allFriendsLength += intermission+ChatFriendPanelManager::ChatFriendManagerGet().controlHeight;
		numberLeftLinePlayers++;
	}

}

void PlayerInfoDlg::PlayerInfoDlgUpdateDrawArea()
{
	RECT rc;
	rc.left = drawAreaX;
	rc.top = drawAreaY;
	rc.right = rc.left + drawAreaWidth;
	rc.bottom = rc.top + drawAreaHeight;
	InvalidateRect(hDlg,&rc,TRUE);
	UpdateWindow(hDlg);
}
void PlayerInfoDlg::PlayerInfoDlgUpdate()
{

	InvalidateRect(hDlg,NULL,TRUE);
	UpdateWindow(hDlg);
}

void PlayerInfoDlg::PlayerInfoAddPlayers(const char* name)
{

	if(hDlg == ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_CHAT_PLAYER_INFO_FRIEND].hDlg)
		g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)name, CHAT::GROUPID_NONE );
	else
	if(hDlg == ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_CHAT_PLAYER_INFO_ENEMY].hDlg)
		g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)name, CHAT::GROUPID_ENEMY );
	else
	if(hDlg == ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_CHAT_PLAYER_INFO_PINGBI].hDlg)
		g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)name, CHAT::GROUPID_BLACK );
	else
	if(hDlg == ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_CHAT_PLAYER_INFO_TEMP].hDlg)
		g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)name, CHAT::GROUPID_TEMP );
}


PlayerListTitleControl::PlayerListTitleControl()
{
	///////控件的状态//////
	state = 0;
	//////////////
	////控件的大小和位置//////
	posRect.height = posRect.height=posRect.x = posRect.y = 0;
	nameItemWidth = 0;
	metierItemWidth = 0;
	levelItemWidth  = 0;
//	groupItemWidth = 0;
//	placeItemWidth  = 0;
	pszFont[0] = 0;
	fontNormalColor = 0;
	fontMouseOverColor = 0;
	//////////////////
	hParentHandle = 0;
	dragState = 0;
	dragCurrentPoint.x = 0;
	dragCurrentPoint.y = 0;
	isUseDefualFont = false;
}

PlayerListTitleControl::~PlayerListTitleControl()
{
}
void PlayerListTitleControl::TitleControlInit()
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_X,0,&posRect.x);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_Y,0,&posRect.y);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INT_WIDTH,0,&posRect.width);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_HEIGHT,0,&posRect.height);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_NAME_WIDTH,0,&nameItemWidth);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_METIER_WIDTH,0,&metierItemWidth);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_LEVEL_WIDTH,0,&levelItemWidth);
//	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_GROUP_WIDTH,0,&groupItemWidth);
//	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_PLACE_WIDTH,0,&placeItemWidth);
//	iniFile.GetString(_TITLE_INI_ITEM_NAME,_TITLE_INI_FONT ,"",szValue,MAX_PATH);

//	strcpy(pszFont,szValue);

	char font[FONT_SIZE]={0};
	iniFile.GetString(_TITLE_INI_ITEM_NAME,"font","",font,FONT_SIZE);
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
		isUseDefualFont  = false;
		strcpy(pszFont,font);
	}
	else
	{
	  strcpy(pszFont,ChatString::ChatStringGetString().chatDefualtFont);
	  isUseDefualFont = true;
	}

	int r ,g,b;
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_FONT_NORMAL_COLOR_R,0,&r);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_FONT_NORMAL_COLOR_G,0,&g);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_FONT_NORMAL_COLOR_B,0,&b);

	fontNormalColor = RGB(r,g,b);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_FONT_MOUSEOVER_COLOR_R,0,&r);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_FONT_MOUSEOVER_COLOR_R,0,&g);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,_TITLE_INI_FONT_MOUSEOVER_COLOR_R,0,&b);

	fontMouseOverColor = RGB(r,g,b);

	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,"lineFontColor_r",0,&r);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,"lineFontColor_g",0,&g);
	iniFile.GetInteger(_TITLE_INI_ITEM_NAME,"lineFontColor_b",0,&b);
	lineColor = RGB(r,g,b);



}

void PlayerListTitleControl::TitleControlUpdate()
{
	RECT rc;
	rc.left = posRect.x;
	rc.top = posRect.y;
	rc.right = rc.left + posRect.width;
	rc.bottom = rc.top + posRect.height;
	InvalidateRect(hParentHandle,&rc,TRUE);
	UpdateWindow(hParentHandle);
}

void PlayerListTitleControl::TitleControlProcessLButtonDown(PlayerInfoDlg& infoDlg,POINT& pos)
{
	if(pos.x<=posRect.x||pos.x>=posRect.x+posRect.width||
		pos.y<=posRect.y||pos.y>=posRect.y+posRect.height)
	{
		return;
	}
	if(pos.x<= posRect.x+nameItemWidth)
		dragState = _TITLE_DRAG_NAMEITEM;
	else
	if(pos.x<=posRect.x+metierItemWidth+nameItemWidth)
		dragState = _TITLE_DRAG_METIERITEM;
	else
//	if(pos.x <= posRect.x+metierItemWidth+nameItemWidth+levelItemWidth)
		dragState = _TITLE_DRAG_LEVELITEM;
/*	else
	if(pos.x <= posRect.x+metierItemWidth+nameItemWidth+levelItemWidth+groupItemWidth)
		dragState = _TITLE_DRAG_GROUPITEM;
	else
		dragState = _TITLE_DRAG_PLACEITEM;*/

	dragCurrentPoint.x = pos.x;
	dragCurrentPoint.y = pos.y;

	SetCapture(infoDlg.hDlg);
	
}

void PlayerListTitleControl::TitleControlProcessLButtonUp()
{
	if(dragState)
	{
		ReleaseCapture();
		dragState = 0;
	}
}
void PlayerListTitleControl::TitleControlProcessLButtonDBLCLK(PlayerInfoDlg& infoDlg,POINT& pos)
{
	if(pos.x<=posRect.x||pos.x>=posRect.x+posRect.width||
		pos.y<=posRect.y||pos.y>=posRect.y+posRect.height)
	{
		return;
	}
	///////////////////////////////
	if(infoDlg.PlayerInfoDlgGetState()&_PLAYER_LIST_CONTROL_NORMAL_SHOW)
		infoDlg.PlayerInfoRemoveState(_PLAYER_LIST_CONTROL_NORMAL_SHOW);
	else
		infoDlg.PlayerInfoSetState(_PLAYER_LIST_CONTROL_NORMAL_SHOW);
	if(pos.x<=posRect.x+nameItemWidth)
	{
		infoDlg.PlayerInfoQSort(_PLAYER_LIST_SORT_BYNAME);
	}
	else
	if(pos.x<=posRect.x+metierItemWidth+nameItemWidth)
		infoDlg.PlayerInfoQSort(_PLAYER_LIST_SORT_BYMETIER);
	else
//	if(pos.x <= posRect.x+metierItemWidth+nameItemWidth+levelItemWidth)
		infoDlg.PlayerInfoQSort(_PLAYER_LIST_SORT_BYLEVEL);
//	else
/*	if(pos.x <= posRect.x+metierItemWidth+nameItemWidth+levelItemWidth+groupItemWidth)
		infoDlg.PlayerInfoQSort(_PLAYER_LIST_SORT_BYGROUP);
	else
		infoDlg.PlayerInfoQSort(_PLAYER_LIST_SORT_BYPLACE);*/

	infoDlg.PlayerInfoDlgUpdateDrawArea();



}



void PlayerListTitleControl::TitleControlProcessMouseMove(PlayerInfoDlg& infoDlg,POINT& pos)
{
	if(dragState)
	{
		if(dragState == _TITLE_DRAG_NAMEITEM)
		{
			int dx = pos.x - dragCurrentPoint.x;
			if(dx>0)
			{
				if(metierItemWidth - dx>2)
				{
					metierItemWidth-= dx;
					nameItemWidth+= dx;
					ChatFriendPanelManager::ChatFriendManagerGet().metierItemWidth-=dx;
					ChatFriendPanelManager::ChatFriendManagerGet().nameItemWidth+=dx;
					
				}
			}
			else
			{
				if(nameItemWidth+dx>2)
				{
					nameItemWidth+=dx;
					ChatFriendPanelManager::ChatFriendManagerGet().nameItemWidth+=dx;
					metierItemWidth-=dx;
					ChatFriendPanelManager::ChatFriendManagerGet().metierItemWidth-= dx;
				}
			}
			infoDlg.PlayerInfoDlgUpdateDrawArea();
			dragCurrentPoint.x = pos.x;
			dragCurrentPoint.y = pos.y;
			return;
		}

		if(dragState == _TITLE_DRAG_METIERITEM)
		{
			int dx = pos.x - dragCurrentPoint.x;
			if(dx>0)
			{
				if(levelItemWidth - dx>2)
				{
					levelItemWidth-= dx;
					metierItemWidth+= dx;
					ChatFriendPanelManager::ChatFriendManagerGet().levelItemWidth-=dx;
					ChatFriendPanelManager::ChatFriendManagerGet().metierItemWidth+=dx;
				}
			}
			else
			{
				if(metierItemWidth+dx>2)
				{
					metierItemWidth+=dx;
					levelItemWidth-=dx;
					ChatFriendPanelManager::ChatFriendManagerGet().metierItemWidth+=dx;
					ChatFriendPanelManager::ChatFriendManagerGet().levelItemWidth-=dx;
					
				}
			}
			infoDlg.PlayerInfoDlgUpdateDrawArea();
			dragCurrentPoint.x = pos.x;
			dragCurrentPoint.y = pos.y;
			return;
		}

		return;

	}
	if(pos.x<=posRect.x||pos.x>=posRect.x+posRect.width||
		pos.y<=posRect.y||pos.y>=posRect.y+posRect.height)
	{
		state&=~_TITLE_STATE_NAME_MOUSEOVE;
		state&= ~_TITLE_STATE_METIER_MOUSEOVER;
		state&= ~_TITLE_STATE_LEVEL_MOUSEOVER;
		state&= ~_TITLE_STATE_GROUP_MOUSEOVER;
		state&= ~_TITLE_STATE_PLACE_MOUSEOVER;
	}
	else
	{
		if(pos.x<=posRect.x+nameItemWidth)
		{
			if(state&_TITLE_STATE_NAME_MOUSEOVE)
				return;
			state|=_TITLE_STATE_NAME_MOUSEOVE;
			state&= ~_TITLE_STATE_METIER_MOUSEOVER;
			state&= ~_TITLE_STATE_LEVEL_MOUSEOVER;
			state&= ~_TITLE_STATE_GROUP_MOUSEOVER;
			state&= ~_TITLE_STATE_PLACE_MOUSEOVER;
		}
		else
		if(pos.x<=posRect.x+metierItemWidth+nameItemWidth)
		{
			if(state&_TITLE_STATE_METIER_MOUSEOVER)
				return;
			state|=_TITLE_STATE_METIER_MOUSEOVER;

			state&= ~_TITLE_STATE_NAME_MOUSEOVE;
			state&= ~_TITLE_STATE_LEVEL_MOUSEOVER;
			state&= ~_TITLE_STATE_GROUP_MOUSEOVER;
			state&= ~_TITLE_STATE_PLACE_MOUSEOVER;
		}
		else
		if(pos.x <= posRect.x+metierItemWidth+nameItemWidth+levelItemWidth)
		{
			if(state&_TITLE_STATE_LEVEL_MOUSEOVER)
				return ;
			state|= _TITLE_STATE_LEVEL_MOUSEOVER;

			state&= ~_TITLE_STATE_NAME_MOUSEOVE;
			state&= ~_TITLE_STATE_METIER_MOUSEOVER;
			state&= ~_TITLE_STATE_GROUP_MOUSEOVER;
			state&= ~_TITLE_STATE_PLACE_MOUSEOVER;

		}
	}
	TitleControlUpdate();
}


void PlayerListTitleControl::TitleControlDrawItem(HDC hdc)
{
	COLORREF color = 0;
	if(state&_TITLE_STATE_NAME_MOUSEOVE)
		color = fontMouseOverColor;
	else
		color = fontNormalColor;
	DrawTextItem(hdc,color,posRect.x,posRect.y,nameItemWidth,posRect.height,pszFont,ChatString::ChatStringGetString().titlePlayerName,lineColor,isUseDefualFont);

	if(state&_TITLE_STATE_METIER_MOUSEOVER)
		color = fontMouseOverColor;
	else
		color = fontNormalColor;
	DrawTextItem(hdc,color,posRect.x+nameItemWidth,posRect.y,metierItemWidth,posRect.height,pszFont,ChatString::ChatStringGetString().titlePlayerMetier,lineColor,isUseDefualFont);

	if(state&_TITLE_STATE_LEVEL_MOUSEOVER)
		color = fontMouseOverColor;
	else
		color = fontNormalColor;
	DrawTextItem(hdc,color,posRect.x+nameItemWidth+metierItemWidth,
		         posRect.y,levelItemWidth,posRect.height,pszFont,ChatString::ChatStringGetString().titlePlayerLevel,lineColor,isUseDefualFont,FALSE);
}
