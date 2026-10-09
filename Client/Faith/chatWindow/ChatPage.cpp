#include <windows.h>
#include <commctrl.h>
#include <list>
#include <vector>
#include <string>
using std::list;
using std::vector;
using std::string;
#include "layoutinterface.h"
#include "chatWindow/faceDialog.h"
#include "GameDataDef.h"

#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "loadSrcWnd/GDILoadBitmap.h"

#include "chatWindow/ChatWndProc.h"

#include "ChatDataDef.h"
#include "chatWindow/ChatResource.h"

ChatPage::ChatPage()
{
	drawPoint.x = drawPoint .y = 0;
}
ChatPage::~ChatPage()
{
	pageButton.ChatWndKillTimer();
}
const Ui_Channel_Param& ChatPage::ChatPageGetChannel(int i)
{
	return channel[i];
}
void ChatPage::ChatPageRegistChannel(Ui_Channel_Param& channel)
{
	int size = this->channel.size();
	for(int i = 0;i < size;i++)
	{
		if(this->channel[i].dwChannelID == channel.dwChannelID)
			return ;
	}
	this->channel.push_back(channel);
}

void ChatPage::ChatPageCloseChannel(int channelID)
{

	int size = channel.size();
	for(int i = 0; i < size;i++)
	{
		if(channelID==channel[i].dwChannelID)
		{
			channel.erase(&channel[i]);
			return;
		}
	}
}

ChatButton& ChatPage::ChatPageGetButton() 
{
	return pageButton;
}
bool ChatPage::IsHavePersonalChannel()
{
	int size = channel.size();
	for(int i = 0; i < size; i++)
	{
		Ui_Channel_Param& chan = channel[i];
		if(chan.dwChannelID == COSE_ROOM_ID)
			return true;
	}
	return false;
}
void ChatPage::ChatPageSetShowWnd(ChatWnd* pInfo)
{
	pInfoWnd = pInfo;
}
POINT& ChatPage::ChatPageGetDrawPoint()
{

	return drawPoint;
}
void ChatPage::ChatPageShow()
{
	HWND hWnd = pInfoWnd->ChatWndGetHandle();
	if(chatWndRender.numberUsed>0)
	{
		chatWndRender.ChatWndRenderAutoScroll(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom-B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY);
	}
	InvalidateRect(hWnd,NULL,TRUE);
	UpdateWindow(hWnd);
}
void ChatPage::ChatPageCreatePageButton(HWND hwnd,const char*pagesName)
{
#define BUTTON_TEXT_INFO      "textInfo"
	////////////////////
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
//	GetCurrentDirectory(MAX_PATH,szPath);
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH]={0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	int x=0,y=0;
	int width=0;
	int height=0;
	BOOL isClip=0;
	int r=0,g=0,b=0;
	int id;
	char name[64] ={0};
	bool hideWindow = false;
	ChatWndProc   pFun = 0;
	if(strcmp(pagesName,ChatString::ChatStringGetString().chSynthetizeName)==0)
	{
		id = _CHAT_PAGE_BUTTON_ID_SYNTHESIS;
		pageID = _PAGE_ID_SYNTHESIS;
		strcpy(name,"synthetize");
		pFun = ChatWndProcessFun::ProcessSynthetizePageButtonFun;
	}
	else
	if(strcmp(pagesName,ChatString::ChatStringGetString().chNearName)==0)
	{
		id = _CHAT_PAGE_BUTTON_ID_NEAR;
		pageID = _PAGE_ID_NEAR;
		strcpy(name,"near");
		pFun = ChatWndProcessFun::ProcessNearPageButtonFun;
	}
	else
	if(strcmp(pagesName,ChatString::ChatStringGetString().chWorldName)==0)
	{
		id = _CHAT_PAGE_BUTTON_ID_WORLD;
		pageID = _PAGE_ID_WORLD;
		strcpy(name,"world");
		pFun = ChatWndProcessFun::ProcessWorldPageButtonFun;
	}
	else
	if(strcmp(pagesName,ChatString::ChatStringGetString().chSystemName)==0)
	{
		id = _CHAT_PAGE_BUTTON_ID_SYSTEM;
		pageID = _PAGE_ID_SYSTEM;
		strcpy(name,"system");
		hideWindow = true;

	}	
	else
	if(strcmp(pagesName,ChatString::ChatStringGetString().chPersonalName)==0)
	{
		id = _CHAT_PAGE_BUTTON_ID_PERSONAL;
		pageID = _PAGE_ID_PERSONAL;
		strcpy(name,"personal");
		pFun = ChatWndProcessFun::ProcessPersonalButtonFun;
	}
	else
	if(strcmp(pagesName,ChatString::ChatStringGetString().chOrgName)==0)
	{
		id = _CHAT_PAGE_BUTTON_ID_ORG;
		pageID = _PAGE_ID_ORG;
		strcpy(name,"org");
		pFun = ChatWndProcessFun::ProcessOrgPageButtonFun;
	}
	else
	if(strcmp(pagesName,ChatString::ChatStringGetString().chFightName)==0)
	{
		id = _CHAT_PAGE_BUTTON_ID_FIGHT;
		pageID = _PAGE_ID_FIGHT;
		strcpy(name,"fight");
		pFun = ChatWndProcessFun::ProcessFightPageButtonFun;
	}
	else
	{
		id = -1;
		pageID = -1;
	}
	iniFile.GetInteger(name,_CHAT_SRC_X,0,&x);
	iniFile.GetInteger(name,_CHAT_SRC_Y,0,&y);
	iniFile.GetInteger(name,_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger(name,_CHAT_SRC_HEIGHT,0,&height);

	iniFile.GetInteger(name,_CHAT_SRC_CLIP,0,&isClip);
	if(isClip==1)
	{
		iniFile.GetInteger(name,_CHAT_SRC_CLIP_COLOR_R,0,&r);
		iniFile.GetInteger(name,_CHAT_SRC_CLIP_COLOR_G,0,&g);
		iniFile.GetInteger(name,_CHAT_SRC_CLIP_COLOR_B,0,&b);

	}
	pageButton.ChatWndCreate(id,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,hwnd,"","button",x,y,width,height);
	SetClassLong(pageButton.ChatWndGetHandle(),GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
	///////////////////////////////////////////////////////////////////////////////////////////
	iniFile.GetString(name,"tooltipInfo","",szValue,MAX_PATH);
	if(szValue[0] != 0)
		pageButton.ChatWndTipCreate(TTS_NOPREFIX,szValue,100);
	//////////////////////////////////////////////////////////////////////////////////////////////
	int normal_idx = 0,hover_idx = 0,pushed_idx = 0;
	iniFile.GetInteger(name,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(name,"mouseOverSrcIdx",0,&hover_idx);
	iniFile.GetInteger(name,"mouseDownSrcIdx",0,&pushed_idx);
	pageButton.ChatWndSetResource(normal_idx,hover_idx,pushed_idx,-1);
	iniFile.GetInteger(name,"clipEnable",0,&isClip);
	if(isClip)
	{
		iniFile.GetString(name,"clipColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		ChatWnd::BitmapToRgn(ChatResource::GetSingle().GetResource(normal_idx)->hBitmap,pageButton.ChatWndGetRgn(),RGB(r,g,b));
		SetWindowRgn(pageButton.ChatWndGetHandle(),pageButton.ChatWndGetRgn(),TRUE);
	}
	char InfoText[256]={0};
	char font[256]={0};
	iniFile.GetString(name,BUTTON_TEXT_INFO,"",InfoText,256);
	if(InfoText[0]!=0)
	{
		pageButton.ChatWndSetText(InfoText);
		pageButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
	}
	iniFile.GetString(name,"fontColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	pageButton.ChatWndSetTextNormalColor(RGB(r,g,b));
	iniFile.GetString(name,"font","",font,256);
	TFONT tFont;
	strcpy(tFont.fontName,font);
	HDC hdc =CreateCompatibleDC(NULL);
	LOGFONT logFont;
	logFont.lfFaceName[0]=0;
	logFont.lfCharSet = DEFAULT_CHARSET;
	EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
	DeleteDC(hdc);
	if(tFont.isInSystem)
		pageButton.ChatWndSetFont(font);
	else
	{
		pageButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
		pageButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
	}
	this->name[0] = 0;
	strcpy(name,pagesName);
	if(pFun)
		pageButton.SetWndProcessFun(pFun);
	if(hideWindow)
		pageButton.ChatWndShow(SW_HIDE);


}