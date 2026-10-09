#include "KWin32.h"

#include "ChatDataDef.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <stdio.h>
#include <list>
#include <vector>
#include <string>
using std::string;
using std::list;
using std::vector;
#include "Ui/UiCase/UiTipGenerator.h"
#include "CoreShell.h"
#include "ui/UiCommon.h"
#include "GameDataDef.h"
#include "layoutinterface.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatCharContainer.h"
#include "Ui/UiCase/UiChatWindow.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "Ui/UiCase/UiErrorMessageBox.h"
#include "loadSrcWnd/GDILoadBitmap.h"
#include "resource.h"
#include "chatWindow/ChatWndProc.h"
#include "ChatResource.h"
#include "chatWindow/PlayerShowInfo.h"
#include "ui/UiCase/UiPathHelp.h"
using namespace ClientMapInfo;
extern iCoreShell*							g_pCoreShell			;
ChatWndSRC::ChatWndSRC()
{
	normal_idx = -1;
	hover_idx = -1;
	pushed_idx = -1;
	disable_idx = -1;
}
ChatWndSRC::~ChatWndSRC()
{
	
}
void ChatWndSRC::SetSrcIdx(int normal_id /* = -1 */,int hove_id /* = -1 */,int push_id /* = -1 */,int disable_id /* =-1 */)
{
	normal_idx = normal_id;
	hover_idx = hove_id;
	pushed_idx = push_id;
	disable_idx = disable_id;
}

HRGN   ChatWnd::BitmapToRgn(HBITMAP hBitmap,HRGN& hRgn,COLORREF color)
{
	BITMAP bm;
	HDC hdc;
	hdc = CreateCompatibleDC(NULL);
	HBITMAP hOld = (HBITMAP)SelectObject(hdc,hBitmap);
	GetObject(hBitmap,sizeof(bm),&bm);
	HRGN tempRgn;
	hRgn = CreateRectRgn(0,0,0,0);
	for(int y = 0;y < bm.bmHeight;y++)
	{
		int xStart = 0,xEnd = 0;
		while((xStart < bm.bmWidth)&&(GetPixel(hdc,xStart,y) == color))
			xStart++;
		if(xStart >= bm.bmWidth) continue;
		xEnd = xStart;
		while((xEnd < bm.bmWidth)&&(GetPixel(hdc,xEnd,y) !=color))
			xEnd++;
		tempRgn = CreateRectRgn(xStart,y,xEnd,y+1);
		CombineRgn(hRgn,hRgn,tempRgn,RGN_OR);
		DeleteObject(tempRgn);	

	}
	SelectObject(hdc,hOld);
	DeleteDC(hdc);
	return hRgn;
}
void CALLBACK ButtonFlashTimerProc(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime)
{
	DWORD id = timer_id - _BUTTON_ID_TO_TIMER_ID;
	ChatWnd* pWnd = 0;
	switch(id)
	{
	case _CHAT_PAGE_BUTTON_ID_SYNTHESIS:
	case _CHAT_PAGE_BUTTON_ID_NEAR:
	case _CHAT_PAGE_BUTTON_ID_SYSTEM:
	case _CHAT_PAGE_BUTTON_ID_WORLD:
	case _CHAT_PAGE_BUTTON_ID_PERSONAL:
    case _CHAT_PAGE_BUTTON_ID_ORG:
	case _CHAT_PAGE_BUTTON_ID_FIGHT:
		{
			int numberPage = B2ChatDialog::chatManager.ChatManagerGetPageNumber();
			for(int i = 0;i<numberPage;i++)
			{
				ChatButton* pButton = &B2ChatDialog::chatManager.chatPages[i]->ChatPageGetButton();
				if(pButton->ChatWndGetID() ==id)
				{
					pWnd = pButton;
					break;
				}
			}
		}
		break;
	case _CHAT_BUTTON_ID_TOP:
		{
			pWnd = &B2ChatDialog::chatManager.normalButton[_TOP_BUTTON_INDEX];
		}
		break;
	case _CHAT_BUTTON_ID_UP:
		{
			pWnd = &B2ChatDialog::chatManager.normalButton[_UP_BUTTON_INDEX];

		}
		break;
	case _CHAT_BUTTON_ID_DOWN:
		{
			pWnd = &B2ChatDialog::chatManager.normalButton[_DOWN_BUTTON_INDEX];
		}
		break;
	case _CHAT_BUTTON_ID_END:
		{
			pWnd = &B2ChatDialog::chatManager.normalButton[_END_BUTTON_INDEX];
		}
		break;
	}
	float currentTimef = (float)currentTime/1000.0f;
	if(currentTimef - pWnd->startAllFlashTime>pWnd->flashAllTime)
	{
		pWnd->ChatWndKillTimer();
		pWnd->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		pWnd->ChatWndUpdate();
		return;
	}
	float dTime = currentTimef - pWnd->startFlashTime;
	if(dTime>=pWnd->flashTime)
	{
		if(pWnd->ChatWndGetState()==_CHAT_BUTTON_STATE_NORMAL)
			pWnd->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
		else
			pWnd->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		pWnd->ChatWndUpdate();
		pWnd->startFlashTime = currentTimef;
	}
}
ChatWnd::ChatWnd()
{
	hChatRgn     =   0;
	hChatWndHandle = 0;
	hChatWndID     = 0;
	iChatWndState  = 0;
	pTextInfo      = 0;
	isFlashing    = false;
	flashTime     = 0.0f;
	startFlashTime = 0.0f;
	farTimerProc = ButtonFlashTimerProc;
	timerID      = 0;
	chatWndattr = 0;
	flashAllTime = _FLASH_ALL_TIME;
	startAllFlashTime = 0.0f;
	hControlParent = 0;
	tipEnable = false;
	tipMaxWidth = 0;
	hTipWnd = 0;
	processFun = 0;

	
}
ChatWnd::~ChatWnd()
{

	if(pTextInfo)
	{
		delete [] pTextInfo;
		pTextInfo= 0;
	}
	if(hChatRgn)
		DeleteObject(hChatRgn);
	ChatWndKillTimer();  
	
}
BOOL ChatWnd::ChatWndTipCreate(long style,char* text,int maxWidth,const TCHAR* titleText /* = 0 */)
{
	if(hChatWndHandle == 0)
		return false;
	if((hTipWnd = CreateWindowEx(0,TOOLTIPS_CLASS,"",WS_POPUP|style,
		CW_USEDEFAULT,CW_USEDEFAULT
		,CW_USEDEFAULT,CW_USEDEFAULT,0,(HMENU)0,KWin32App::m_hInstance,0))==0)
		return FALSE;

	TOOLINFO toolInfo;
	toolInfo.cbSize = sizeof(TOOLINFO);
	toolInfo.uFlags = TTF_IDISHWND | TTF_TRANSPARENT | TTF_TRACK | TTF_ABSOLUTE;
	toolInfo.hinst = KWin32App::m_hInstance;
	toolInfo.hwnd = hChatWndHandle;
	toolInfo.uId = (UINT)hChatWndHandle;
	toolInfo.lpszText = text;
	toolInfo.rect.bottom = toolInfo.rect.top = toolInfo.rect.left = toolInfo.rect.right = 0;
	SendMessage(hTipWnd,TTM_ADDTOOL,0,(LPARAM)&toolInfo);
	SendMessage(hTipWnd,TTM_SETMAXTIPWIDTH,0,maxWidth);
	tipEnable = true;
	return TRUE;
	

}

void  ChatWnd::SetWndProcessFun(ChatWndProc pProcessFun)
{
	processFun = (WNDPROC)SetWindowLong(hChatWndHandle,GWL_WNDPROC,(LONG)pProcessFun);
}
BOOL ChatWnd::ChatWndCreate(int id,long style,
							HWND parent,
							const TCHAR* text,
							const TCHAR* className, 
							int x/* =0 */,int y/* =0 */,
							int width/* =0 */,
							int height/* =0 */)
{
	hChatWndID = id;
	if((hChatWndHandle = CreateWindowEx(0,className,text,style,x,y,
		width,height,parent,(HMENU)id,KWin32App::m_hInstance,0))==0)
	{
		DWORD err = GetLastError();
		return FALSE;
	}
	ChatWnd::x = x;
	ChatWnd::y = y;
	timerID = _BUTTON_ID_TO_TIMER_ID+id; 
	GetClientRect(hChatWndHandle,&rect);
	hControlParent = parent;
	return TRUE;
}
void ChatWnd::ChatWndSetTimer()
{
	if(isFlashing==TRUE)
		return;
	SetTimer(hChatWndHandle,timerID,_TIMER_PROCESS_TIME,farTimerProc);
	startFlashTime = (float)GetTickCount()/1000.0f;
	isFlashing = TRUE;
	startAllFlashTime = startFlashTime;
}
void ChatWnd::ChatWndKillTimer()
{
	if(isFlashing)
		KillTimer(hChatWndHandle,timerID);
	isFlashing = FALSE;
}
const RECT& ChatWnd::ChatWndGetRect() const
{
	return rect;
}
HWND ChatWnd::ChatWndGetHandle() const
{
	return hChatWndHandle;
}

HRGN& ChatWnd::ChatWndGetRgn()
{
	return hChatRgn;
}
int ChatWnd::ChatWndGetID() const
{
	return hChatWndID;
}
void ChatWnd::ChatWndSetResource(int normal_idx,int hover_idx,int pushed_idx,int diable_idx)
{
	wndSrc.SetSrcIdx(normal_idx,hover_idx,pushed_idx,diable_idx);
}
int ChatWnd::ChatWndGetState() const
{
	return iChatWndState;
}
void ChatWnd::ChatWndMove(int x,int y,int width,int height,bool redraw)
{
	MoveWindow(hChatWndHandle,x,y,width,height,redraw);
//	rect.left = rect.top = 0;
//	rect.right = width;
//	rect.bottom = height;
//	ChatWnd::x = x;
//	ChatWnd::y = y;

}

void ChatWnd::ChatWndSetText(const char* pText)
{
	if(pText == 0)
		return;
	int len = strlen(pText);
	if(pTextInfo)
		delete [] pTextInfo;
	pTextInfo = new char[len+1];
	strcpy(pTextInfo,pText);
	chatWndattr |= _CHAT_WND_ATTR_HAVE_TEXT;
	pTextInfo[len] = 0;
}
void ChatWnd::ChatWndSetState(int state)
{
	iChatWndState = state;
}
void ChatWnd::ChatWndShow(int flags)
{
	ShowWindow(hChatWndHandle,flags);
}
void ChatWnd::ChatWndShowTip(BOOL bShow)
{
	TOOLINFO toolInfo ;
	toolInfo.cbSize = sizeof(TOOLINFO);
	toolInfo.uFlags = TTF_IDISHWND;
	toolInfo .hwnd = hChatWndHandle;
	toolInfo.uId = (UINT)hChatWndHandle;

	SendMessage(hTipWnd,TTM_TRACKACTIVATE,(WPARAM)bShow,(LPARAM)&toolInfo);
	
}
void ChatWnd::ChatWndUpdate()
{
	InvalidateRect(hChatWndHandle,NULL,TRUE);
	UpdateWindow(hChatWndHandle);
}
void ChatWnd::ChatWndUpdataTipText(char* text)
{
	if(text == 0)
		return;
	TOOLINFO toolInfo;
	toolInfo.cbSize = sizeof(TOOLINFO);
	toolInfo.uFlags = TTF_IDISHWND;
	toolInfo.hwnd = hChatWndHandle;
	toolInfo.hinst = KWin32App::m_hInstance;
	toolInfo.lpszText = text;
	toolInfo.uId = (UINT)hChatWndHandle;
	SendMessage(hTipWnd,TTM_UPDATETIPTEXT,0,(LPARAM)&toolInfo);
}
void ChatWnd::ChatWndSetTipPos()
{
	POINT pt;
	GetCursorPos(&pt);
	RECT rc;
	GetClientRect(hTipWnd,&rc);
	RECT rDesk;
	GetClientRect(GetDesktopWindow(),&rDesk);
	if(pt.x+rc.right>rDesk.right)
		pt.x= rDesk.right - rc.right;
	else
	if(pt.x < rDesk.left)
		pt.x = rDesk.left;
	if(pt.y + rc.bottom > rDesk.bottom)
		pt.y = rDesk.bottom - rc.bottom;
	else
	if(pt.y < rc.left)
		pt.y = rc.left;
//	ScreenToClient(hChatWndHandle,&pt);
	SendMessage(hTipWnd,TTM_TRACKPOSITION,0,(LPARAM)MAKELPARAM(pt.x,pt.y));
	//////////////////////////////////////////////////////////////////////////
	SetWindowPos(hTipWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE);
}

/////////////////////
ChatButton::ChatButton():
ChatWnd()
{
}
BOOL ChatButton::ChatWndCreate(int id,long style,HWND parent,const TCHAR* text,const TCHAR* className, int x/* =0 */,int y/* =0 */,int width/* =0 */,int height/* =0 */)
{
	return ChatWnd::ChatWndCreate(id,style,parent,text,className,x,y,width,height);
}
LPCHATWND ChatButton::ChatWndGetChatWndPointer()
{
	
	return (LPCHATWND)this;
}
ChatButton::~ChatButton()
{
	normalColor = 0;
}
void ChatButton::ChatWndDrawItem(HDC hdc)
{
	
	HBITMAP hBitmap = 0;
	ChatResource& resource = ChatResource::GetSingle();
	switch(iChatWndState)
	{
	case _CHAT_BUTTON_STATE_NORMAL:
			if(wndSrc.normal_idx!=-1)
				hBitmap = resource.GetResource(wndSrc.normal_idx)->hBitmap;
		break;
	case _CHAT_BUTTON_STATE_MOUSEOVER:
		if(wndSrc.hover_idx!=-1)
			hBitmap = resource.GetResource(wndSrc.hover_idx)->hBitmap;
		break;
	case _CHAT_BUTTON_STATE_MOUSEDOWN:
		if(wndSrc.pushed_idx!=-1)
			hBitmap = resource.GetResource(wndSrc.pushed_idx)->hBitmap;
		break;
	case _CHAT_BUTTON_STATE_DISABLE:
		if(wndSrc.disable_idx!=-1)
			hBitmap = resource.GetResource(wndSrc.disable_idx)->hBitmap;
		break;

	}
	RECT rect;
	GetClientRect(hChatWndHandle,&rect);
	HDC hdc1 = CreateCompatibleDC(hdc);
	HBITMAP hTemp = CreateCompatibleBitmap(hdc,rect.right,rect.bottom);
	HBITMAP hOld = (HBITMAP)SelectObject(hdc1,hTemp);
	DrawBitmap(hdc1,hBitmap,rect.right,rect.bottom);
	if(chatWndattr&_CHAT_WND_ATTR_CHECKED_BUTTON)
	{
		BitBlt(hdc,0,0,rect.right,rect.bottom,hdc1,0,0,SRCCOPY);
		SelectObject(hdc1,hOld);
		DeleteObject(hTemp);
	    DeleteDC(hdc1);
		if((chatWndattr&_CHAT_WND_ATTR_HAVE_TEXT)&&pTextInfo&&strlen(pTextInfo))
		{
			HDC hParentDC = GetWindowDC(hControlParent);
			int mode = SetBkMode(hParentDC,TRANSPARENT);
			int fontHeight = 0;
			int fontWeight = 0;
			if(chatWndattr&_CHAT_WND_ATTR_USE_DEFUALT_FONT)
			{
				fontHeight = ChatString::ChatStringGetString().chatDefualtFontHeight;
				fontWeight = FW_NORMAL;
			}
			else
			{
				fontHeight = rect.bottom;
				fontWeight = FW_BOLD;
			}
			HFONT hFont = CreateFont(fontHeight,0,0,0,fontWeight,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_ROMAN,font);
			HFONT hOldFont = (HFONT)SelectObject(hParentDC,hFont);
			int color = SetTextColor(hParentDC,normalColor);
						SIZE textSize;
			int len = strlen(pTextInfo);
			GetTextExtentPoint32(hParentDC,pTextInfo,len,&textSize);
			int drawX = ChatWnd::x + rect.right+1;
			int drawY = ChatWnd::y + rect.bottom/2-textSize.cy/2;
			RECT rc;
			rc.left = drawX ;
			rc.top = drawY;
			rc.right = rc.left +textSize.cx;
			rc.bottom = rc.top + textSize.cy;
			InvalidateRect(hControlParent,&rc,false);
			UpdateWindow(hControlParent);
			TextOut(hParentDC,drawX,drawY,pTextInfo,strlen(pTextInfo));
			SetTextColor(hParentDC,color);
			SelectObject(hParentDC,hOldFont);
			SetBkMode(hParentDC,mode);
			DeleteObject(hFont);
			ReleaseDC(hControlParent,hParentDC);
			
		}
		return;

	}
	if((chatWndattr&_CHAT_WND_ATTR_HAVE_TEXT)&&pTextInfo&&strlen(pTextInfo))
	{
		int mode = SetBkMode(hdc1,TRANSPARENT);
		if(chatWndattr&_CHAT_WND_ATTR_TEXT_H)
		{
			int height = 0;
			int weight = 0;
			if(chatWndattr&_CHAT_WND_ATTR_USE_DEFUALT_FONT)
			{
				height = ChatString::ChatStringGetString().chatDefualtFontHeight;
				weight = FW_NORMAL;
			}
			else
			{
				height = rect.bottom;
				weight = FW_BOLD;
			}
			HFONT hFont = CreateFont(height,0,0,0,weight,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_ROMAN,font);
			HFONT hOldFont = (HFONT)SelectObject(hdc1,hFont);
			int color = SetTextColor(hdc1,normalColor);
			SIZE textSize;
			int len = strlen(pTextInfo);
			GetTextExtentPoint32(hdc1,pTextInfo,len,&textSize);
			RECT  rc;
			rc.left = (rect.right-textSize.cx)/2;
			rc.right = rc.left+textSize.cx;
			rc.top = (rect.bottom-textSize.cy)/2;
			rc.bottom = rc.top + textSize.cy;
			DrawTextEx(hdc1,pTextInfo,-1,&rc,DT_CENTER|DT_VCENTER,0);
			SetBkMode(hdc1,mode);
			BitBlt(hdc,0,0,rect.right,rect.bottom,hdc1,0,0,SRCCOPY);
			SelectObject(hdc1,hOld);
			SelectObject(hdc1,hOldFont);			
			DeleteObject(hFont);
			DeleteObject(hTemp);
			DeleteDC(hdc1);
			return;
		}
		else
		{
			wchar_t* pWchar = 0;
			ansiToUnicode(pTextInfo,pWchar);
			wchar_t* pNewFont = 0;
			ansiToUnicode(font,pNewFont);
			int height = 0;
			int weight = 0;
			int len = wcslen(pWchar);
			if(chatWndattr&_CHAT_WND_ATTR_USE_DEFUALT_FONT)
			{
				height = ChatString::ChatStringGetString().chatDefualtFontHeight;
				weight = FW_NORMAL;
			}
			else
			{
				height = 16;
				weight = FW_BOLD;
			}

			HFONT hFont = CreateFontW(height,0,0,0,weight,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,pNewFont);
			HFONT hOldFont = (HFONT)SelectObject(hdc1,hFont);
			SIZE textSize ;
			GetTextExtentPoint32W(hdc1,pWchar,len,&textSize);
			int allHeight = len*textSize.cy;
			int color = SetTextColor(hdc1,normalColor);
			for(int i = 0; i<len;i++)
			{
				int charSize[1];
				int x = 0,y=0;
				GetCharWidthW(hdc1,pWchar[i],pWchar[i],charSize);
				RECT rc;
				rc.left = (rect.right - charSize[0])/2;
				rc.right = rc.left+charSize[0];
				rc.top =  (rect.bottom-allHeight)/2+i*textSize.cy;
				rc.bottom = rc.top + textSize.cy;
				DrawTextExW(hdc1,pWchar+i,1,&rc,DT_CENTER|DT_VCENTER,0);

			}
			BitBlt(hdc,0,0,rect.right,rect.bottom,hdc1,0,0,SRCCOPY);
			SelectObject(hdc1,hOld);
			SetTextColor(hdc1,color);
			SetBkMode(hdc1,mode);
			SelectObject(hdc1,hOldFont);
			DeleteObject(hFont);
		    DeleteObject(hTemp);
			DeleteDC(hdc1);
			delete [] pWchar;
			delete [] pNewFont;
			return;
		}
	}
	BitBlt(hdc,0,0,rect.right,rect.bottom,hdc1,0,0,SRCCOPY);
	SelectObject(hdc1,hOld);
	DeleteObject(hTemp);
	DeleteDC(hdc1);
	return;
}

void ChatButton::SetEnableColor()
{
	ChatWndSetTextNormalColor(m_EnableColor);
}

void ChatButton::SetDisableColor()
{
	ChatWndSetTextNormalColor(m_DisableColor);
}

ChatInfoWnd::ChatInfoWnd():
ChatWnd()
{
//	isPressControl = FALSE;
	isScroll = TRUE;
}
ChatInfoWnd::~ChatInfoWnd()
{
	
}


void ChatInfoWnd::ChatInfoWndSetFocus(BOOL focus)
{
	this->focus = focus;
	SetFocus(this->hChatWndHandle);
}
void ChatInfoWnd::ChatInfoWndClearChatInfo()
{
//	if(B2ChatDialog::pLayout)
//	{
//		B2ChatDialog::pLayout->clearLayout();
//	}
}
void ChatInfoWnd::ChatWndMakePlayerBaseInfo(char* buffer)
{
	if(buffer == 0)
		return ;
	KUiPlayerBaseInfo playerBaseInfo;
	g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO,(unsigned int)(&playerBaseInfo),NULL);
	KUiPlayerAttribute playerAttribute;
	g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE,(unsigned int)(&playerAttribute),NULL);
	if(playerBaseInfo.Name[0]==0)
		return;
	sprintf(buffer,"%s %s%d",playerBaseInfo.Name,ChatString::ChatStringGetString().titlePlayerLevel,playerAttribute.nLevel);
	char szZhiye[64] = {0};
	if(playerBaseInfo.nSkillType < 0)
	{
		switch(playerAttribute.nSeries)
		{
		case 0:
			{
				strcpy(szZhiye,ROLE_CAREER_JS);
			}
			break;
		case 1:
			{
				strcpy(szZhiye,ROLE_CAREER_DS);
			}
			break;
		case 2:
			{
				strcpy(szZhiye,ROLE_CAREER_YR);
			}
			break;
		default:
			{
				strcpy(szZhiye,ROLE_CAREER_JS);
			}
		}
	}
	else
	{
		switch(playerAttribute.nSeries)
		{
		case 0:
			{
				if(playerBaseInfo.nSkillType)
					strcpy(szZhiye,ROLE_CAREER_JS_0);
				else
					strcpy(szZhiye,ROLE_CAREER_JS_1);
			}
			break;
		case 1:
			{
				if(playerBaseInfo.nSkillType)
					strcpy(szZhiye,ROLE_CAREER_DS_0);
				else
					strcpy(szZhiye,ROLE_CAREER_DS_1);
			}
			break;
		case 2:
			{
				if(playerBaseInfo.nSkillType)
					strcpy(szZhiye,ROLE_CAREER_YR_0);
				else
					strcpy(szZhiye,ROLE_CAREER_YR_1);
			}
			break;
		default:
			{
				if(playerBaseInfo.nSkillType)
					strcpy(szZhiye,ROLE_CAREER_JS_0);
				else
					strcpy(szZhiye,ROLE_CAREER_JS_1);
			}
		}
	}
	strcat(buffer,szZhiye);
	strcat(buffer," ");

	char szShehui[256] = {0};
//	if()



}
void ChatInfoWnd::ChatWndProcessClickText(POINT& pt,int flags)
{
	int currentPageIndex = B2ChatDialog::chatManager.ChatManagerGetCurrentPage();
	ChatPage* pCurrentPage = B2ChatDialog::chatManager.ChatManagerGetPage(currentPageIndex);
	LOElemInfo info;
	if(pCurrentPage->chatWndRender.ChatWndRenderGetElement(pt.x,pt.y,info) == FALSE)
	{
		if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
		{
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
		}
		B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(true);
		return ;
	}

	bool handled = false;

	switch(info.gameObj._objType)
	{
	case LO_GO_PLAYER:
		{
			if(flags&_CLICK_FLAG_RBUTTON)
			{
				char* name = NULL;
				unicodeToAnsi(info.content.get(), name);
				
				KUiPlayerBaseInfo tagRoleInfo;
				g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&tagRoleInfo, NULL );
				
				if ( 0 == strcmp(tagRoleInfo.Name, name) )
				{
					delete [] name;
					return ;
				}
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->playerInfo = info;
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipAdjustWindow(B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->hWndHandle);
				if(B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipIsShow())
				{
					B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
					B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(TRUE);
				}
				else
					B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(TRUE);
				delete [] name;
				B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(true);

			}
			else
			if(flags&_CLICK_FLAG_SHIFT)
			{	
				PlayerInfo playerInfo;
				memset(&playerInfo,0,sizeof(PlayerInfo));
				char* name = NULL;
				unicodeToAnsi(info.content.get(), name);
				strcpy(playerInfo.szName,name);
				PlayerShowInfo::GetSingle().AddItem(playerInfo);
				delete [] name;	
				B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(true);
			}
			else
			{
				ILayout* pEdit = B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut;
//				pEdit->clearLayout();
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ClearText( );
				pEdit->SetText(_CHAT_EIDT_DEFAULT_STRING);
				pt.x = 0;
				pEdit->setSelection(0,0);
				WCHAR name[256] = {0};
				wsprintfW(name,L"%s%s%s",L"/",info.content.get(),L" ");
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(TRUE);
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditInsertChar(name);
			}
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			return;
		}
	case LO_GO_CHANNEL:
		{
		    B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(TRUE);
			int chanId = info.gameObj._objId[0];
			switch(chanId)
			{
			case LOCAL_ROOM_ID:
				{
					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().currentSelected = NEAR_CHANNALES_TALK;
					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetText(
						B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[NEAR_CHANNALES_TALK].ChatWndGetText());
					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetAllAttr(
						B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[NEAR_CHANNALES_TALK].ChatWndGetAttr());
					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndUpdate();
					if(!B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditIsCloseInfo())
					{
						const char* colorText = KUiChanMgr::getSinglton().getChanColor(LOCAL_ROOM_ID, false);
						LOColor color(colorText);
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->setColor(color);

						B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
					}
				}
				break;
			case GLOBAL_ROOM_ID:
				{

					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().currentSelected = WORLD_CHANNALES_TALK;
					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetText(
						B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[WORLD_CHANNALES_TALK].ChatWndGetText());
					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetAllAttr(
						B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[WORLD_CHANNALES_TALK].ChatWndGetAttr());
					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndUpdate();
					if(!B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditIsCloseInfo())
					{
						const char* colorText = KUiChanMgr::getSinglton().getChanColor(GLOBAL_ROOM_ID, false);
						LOColor color(colorText);
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->setColor(color);
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
					}
				}
				break;
			case SYSTEM_ROOM_ID:
				{

					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().currentSelected = SYSTEM_CHANNALS_TALK;
					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetText(
						B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[SYSTEM_CHANNALS_TALK].ChatWndGetText());
					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetAllAttr(
						B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[SYSTEM_CHANNALS_TALK].ChatWndGetAttr());
					B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndUpdate();
					if(!B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditIsCloseInfo())
					{
						const char* colorText = KUiChanMgr::getSinglton().getChanColor(SYSTEM_ROOM_ID, false);
						LOColor color(colorText);
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->setColor(color);
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
					}
				}
				break;
			default:
				{
					int numberChannels = B2ChatDialog::chatManager.chatChannels.size();
					for(int index = 0; index < numberChannels; index++)
					{
						Ui_Channel_Param channel = B2ChatDialog::chatManager.chatChannels[index];
						if(channel.dwChannelID == chanId)
						{
							if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chTeamName) == 0)
							{
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().currentSelected = TEAM_CHANNALES_TALK;
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetText(
									B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[TEAM_CHANNALES_TALK].ChatWndGetText());
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetAllAttr(
						             B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[TEAM_CHANNALES_TALK].ChatWndGetAttr());
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndUpdate();
								if(!B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditIsCloseInfo())
								{
									const char* colorText = KUiChanMgr::getSinglton().getChanColor(channel.dwChannelID, false);
									LOColor color(colorText);
									B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->setColor(color);
									B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
								}
							}
							else
							if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chGuojiaName) == 0)
							{
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().currentSelected = GUOJIA_CHANNALES_TALK;
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetText(
									B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[GUOJIA_CHANNALES_TALK].ChatWndGetText());
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetAllAttr(
						             B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[GUOJIA_CHANNALES_TALK].ChatWndGetAttr());
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndUpdate();
								if(!B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditIsCloseInfo())
								{
									const char* colorText = KUiChanMgr::getSinglton().getChanColor(channel.dwChannelID, false);
									LOColor color(colorText);
									B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->setColor(color);
									B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
								}
							}
							else
							if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chSizhuName) == 0)
							{
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().currentSelected = SHIZU_CHANNALES_TALK;
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetText(
									B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[SHIZU_CHANNALES_TALK].ChatWndGetText());
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetAllAttr(
						             B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[SHIZU_CHANNALES_TALK].ChatWndGetAttr());
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndUpdate();
								if(!B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditIsCloseInfo())
								{
									const char* colorText = KUiChanMgr::getSinglton().getChanColor(channel.dwChannelID, false);
									LOColor color(colorText);
									B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->setColor(color);
									B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
								}
							}
							else
							if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chZhuhouName) == 0)
							{
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().currentSelected = ZHUHOU_CHANNALES_TALK;
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetText(
									B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[ZHUHOU_CHANNALES_TALK].ChatWndGetText());
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetAllAttr(
						             B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[ZHUHOU_CHANNALES_TALK].ChatWndGetAttr());
								B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndUpdate();
								if(!B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditIsCloseInfo())
								{
									const char* colorText = KUiChanMgr::getSinglton().getChanColor(channel.dwChannelID, false);
									LOColor color(colorText);
									B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->setColor(color);
									B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
								}
							}
						}
					}
				}

			}
			handled = true;	
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(true);
		}
		return;
	case LO_GO_ITEM:
		{
			if(flags&_CLICK_FLAG_RBUTTON)
				return ;
			if(flags&_CLICK_FLAG_CONTROL)
			{
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(TRUE);
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->chatEditInsertElem(info);


			}	
			else
			{ 
				KUiTipGenerator::TipObject tipObj;
				tipObj.type = KUiTipGenerator::LinkedItem;
				SpliteHashId(info.gameObj._objId[0], tipObj.ids[0], tipObj.ids[1], tipObj.ids[2]);
				tipObj.ids[3]	= info.gameObj._objId[1];
				tipObj.ids[4]	= info.gameObj._objId[2];

				char* layoutDes = KUiTipGenerator::getSinglton().genLayoutDes(tipObj);
//				char text[10000] = "<Layout width = 100><Seg text-align=left><Obj>abc\n</Obj></Seg></Layout>";

				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->pTipItemLay->formatText(layoutDes);
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->pTipItemLay->clearLayout();
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->pTipItemLay->SetText(layoutDes);
				HDC hdc  = GetWindowDC(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->hShowItem);
//				SetTextCharacterExtra(hdc,ChatString::ChatStringGetString().fontExtra);
				((GDIRender*)B2ChatDialog::pItemTipRender)->hCurrentDC = hdc;
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->pTipItemLay->flashLayout();
				LORect lRc = B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->pTipItemLay->getRenderArea(false);
				ReleaseDC(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->hShowItem,hdc);
				lRc.setHeight(lRc.getHeight()+B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->topDist+B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->endDist+10);
				lRc.setWidth(lRc.getWidth()+B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->leftDist+B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->rightDist+10);
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->width = lRc.getWidth();
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->height = lRc.getHeight();
				POINT pt;
				::GetCursorPos(&pt);
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->x = pt.x;
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->y = pt.y;
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->closeButton.ChatWndUpdate();
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndAdjustPos();
				if(!B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
					B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(TRUE);
				InvalidateRect(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->hShowItem,NULL,FALSE);
				UpdateWindow(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->hShowItem);

			}
			handled = true;
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(true);
			return ;
		}
	case LO_GO_POSITION:
		{
			if(flags&_CLICK_FLAG_CONTROL)
			{
			
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(TRUE);
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->chatEditInsertElem(info);

			}
			else
			{	
				KUiSceneTimeInfo mapInfo = { 0 };
				
				g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );
				mapInfo.szSceneName[COMMON_CLIENT_MSG_LEN_32 - 1] = 0;
				
				if(info.gameObj._objId[3]!=1)
				{
					if(info.gameObj._objId[0] == mapInfo.nSceneId)
					{
						g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)info.gameObj._objId[1], (int)info.gameObj._objId[2] * 2);
					}
					else
					{
						KUiChannelCentre::GetSingleton().toSysMsg(ChatString::ChatStringGetString().autoGoInfo);
					}
				}
				else
				{
					const AutoMapInfo* pMapInfo = AutoGoBack::Singleton().GetMapInfo(info.gameObj._objId[0]);
					if(pMapInfo == 0)
						return;
					if(strcmp(pMapInfo->mapName,mapInfo.szSceneName) == 0)
					{
						g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)info.gameObj._objId[1], (int)info.gameObj._objId[2] * 2);
					}
					else
					{
						KUiChannelCentre::GetSingleton().toSysMsg(ChatString::ChatStringGetString().autoGoInfo);
					}
				}
			}
			
		}
		if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
		{
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
		}
		B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(true);
		return;// 
	default:
		{
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(true);

		}

	}	
}

LRESULT CALLBACK ChatInfoWnd::ChatInfoWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatInfoWnd* pInfo = B2ChatDialog::chatManager.ChatManagerGetInfoWnd();
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		{
			int flags = 0;
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			if(wParam&MK_CONTROL)
				flags|=_CLICK_FLAG_CONTROL;
			pInfo->ChatWndProcessClickText(pt,flags);
			SetFocus(hwnd);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			int flags = _CLICK_FLAG_RBUTTON;
			if(wParam&MK_SHIFT)
				flags|= _CLICK_FLAG_SHIFT;
			pInfo->ChatWndProcessClickText(pt,flags);

		}
		break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		break;
	}
	return FALSE;
//	return CallWindowProc(B2ChatDialog::wndInfoProc,hwnd,msg,wParam,lParam);
}
void ChatInfoWnd::ChatWndDrawItem(HDC hdc)
{
	
	RECT rect;
	GetClientRect(hChatWndHandle,&rect);
	HDC hdc1 = CreateCompatibleDC(hdc);
//	SetTextCharacterExtra(hdc1,2);

	BITMAP bm;
	ChatResource& resource = ChatResource::GetSingle();
	GetObject(resource.GetResource(wndSrc.normal_idx)->hBitmap,sizeof(bm),&bm);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc,rect.right,rect.bottom);
	HBITMAP hOld1 = (HBITMAP)SelectObject(hdc1,hBitmap);
	DrawBitmap(hdc1,resource.GetResource(wndSrc.normal_idx)->hBitmap,rect.right,rect.bottom);
	int currentPage = B2ChatDialog::chatManager.ChatManagerGetCurrentPage();
	ChatPage* page = B2ChatDialog::chatManager.ChatManagerGetPage(currentPage);
	page->chatWndRender.ChatWndRenderItself(hdc1);
	BitBlt(hdc,0,0,rect.right,rect.bottom,hdc1,0,0,SRCCOPY);
	SelectObject(hdc1,hOld1);
	DeleteObject(hBitmap);
	DeleteDC(hdc1);
}

LPCHATWND ChatInfoWnd::ChatWndGetChatWndPointer()
{
	return (LPCHATWND)this;
}



ChatEditBox::ChatEditBox():
ChatWnd()
{
	pt.x  = pt.y = 0;
	pt.x  = 0;
	pt.y  = 0;
	focus = FALSE;
	bCtlPushed = FALSE;
	mouseExtra = 0;
	channelIndex = 0;
	pEditLayOut = 0;
	hImc = 0;
	lastSendTime = 0.0f;
	isCloseInfo = false;
	showCursor = true;
}
ChatEditBox::~ChatEditBox()
{
	if(pEditLayOut)
	{
		pEditLayOut->clearLayout();
		pEditLayOut->Release();
		delete pEditLayOut;
	}
//	ImmDestroyContext(hImc);
	
}
void   ChatEditBox::ChatEditSetCursorFlash()
{
	SetTimer(hChatWndHandle,_CHAT_WND_EDIT_CURSOR_FLASH_TIME_ID,_CHAT_WND_EDIT_CURSOR_FLASH_TIME,(TIMERPROC)ChatWndProcessFun::EditCursorFlashProc);
	showCursor = true;
}
void ChatEditBox::ChatEditKillCursor()
{
	KillTimer(hChatWndHandle,_CHAT_WND_EDIT_CURSOR_FLASH_TIME_ID);
	showCursor = true;
}
bool   ChatEditBox::ChatEditIsCloseInfo()
{
	LOElemInfo* elemList;
	
	int elemCount = pEditLayOut->getElemList(elemList);
	if(elemCount <= 0)
	{
		delete [] elemList;
		return false;
	}
	const wchar_t* firstContent = elemList[0].content.get();
	wchar_t cozeName[COMMON_CLIENT_MSG_LEN_64];
	cozeName[0] = 0;
	swscanf(firstContent, L"/%[^ ] %*[^\0]", cozeName);
	int nameLen = wcslen(cozeName);
	int textLen = wcslen(firstContent);
	
	if(cozeName[0] != 0 && (nameLen + 2 <= textLen || elemCount > 1))
	{
		delete [] elemList;
		return true;
	}
	delete [] elemList;
	return false;
}
void ChatEditBox::ChatEditWndFashColor()
{
	//判断是否私聊，如果是则改变其颜色，否则显示当前颜色
	if(ChatEditIsCloseInfo())
	{
		pEditLayOut->setColor(KUiChanMgr::getSinglton().getChanColor(COSE_ROOM_ID, false));
		isCloseInfo = true;
	}
	else
	{
		int item = B2ChatDialog::chatManager.ChatManagerGetUiComboBox().currentSelected;
		ChatEditSetChannelColor(item);
	}
}


void ChatEditBox::ChatEditSetChannelColor(int channelID)
{
	if(channelID == WORLD_CHANNALES_TALK)
	{
		const char* colorText = KUiChanMgr::getSinglton().getChanColor(GLOBAL_ROOM_ID, false);
		LOColor color(colorText);
		pEditLayOut->setColor(color);
		ChatWndUpdate();
	}
	else
	if(channelID == SYSTEM_CHANNALS_TALK)
	{
		const char* colorText = KUiChanMgr::getSinglton().getChanColor(SYSTEM_ROOM_ID, false);
		LOColor color(colorText);
		pEditLayOut->setColor(color);
		ChatWndUpdate();
	}
	else
	if(channelID == NEAR_CHANNALES_TALK)
	{
		const char* colorText = KUiChanMgr::getSinglton().getChanColor(LOCAL_ROOM_ID, false);
		LOColor color(colorText);
		pEditLayOut->setColor(color);
		ChatWndUpdate();
	}
	else
	if(channelID == TEAM_CHANNALES_TALK)
	{
		int numberChannels = B2ChatDialog::chatManager.chatChannels.size();
		Ui_Channel_Param channel = B2ChatDialog::chatManager.chatChannels[0];
		for(int i  = 0 ; i < numberChannels; i++)
		{
			channel = B2ChatDialog::chatManager.chatChannels[i];
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chTeamName) == 0)
			{
				const char* colorText = KUiChanMgr::getSinglton().getChanColor(channel.dwChannelID, false);
				LOColor color(colorText);
				pEditLayOut->setColor(color);
				ChatWndUpdate();
				break;
			}
		}
	}
	else
	if(channelID == SHIZU_CHANNALES_TALK)
	{
		int numberChannels = B2ChatDialog::chatManager.chatChannels.size();
		Ui_Channel_Param channel = B2ChatDialog::chatManager.chatChannels[0];
		for(int i  = 0 ; i < numberChannels; i++)
		{
			channel = B2ChatDialog::chatManager.chatChannels[i];
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chSizhuName) == 0)
			{
				const char* colorText = KUiChanMgr::getSinglton().getChanColor(channel.dwChannelID, false);
				LOColor color(colorText);
				pEditLayOut->setColor(color);
				ChatWndUpdate();
				break;
			}
		}
	}
	else
	if(channelID == GUOJIA_CHANNALES_TALK)
	{
		int numberChannels = B2ChatDialog::chatManager.chatChannels.size();
		Ui_Channel_Param channel = B2ChatDialog::chatManager.chatChannels[0];
		for(int i  = 0 ; i < numberChannels; i++)
		{
			channel = B2ChatDialog::chatManager.chatChannels[i];
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chGuojiaName) == 0)
			{
				const char* colorText = KUiChanMgr::getSinglton().getChanColor(channel.dwChannelID, false);
				LOColor color(colorText);
				pEditLayOut->setColor(color);
				ChatWndUpdate();
				break;

			}
		}
	}
	else
	if(channelID == ZHUHOU_CHANNALES_TALK)
	{
		int numberChannels = B2ChatDialog::chatManager.chatChannels.size();
		Ui_Channel_Param channel = B2ChatDialog::chatManager.chatChannels[0];
		for(int i  = 0 ; i < numberChannels; i++)
		{
			channel = B2ChatDialog::chatManager.chatChannels[i];
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chZhuhouName) == 0)
			{
				const char* colorText = KUiChanMgr::getSinglton().getChanColor(channel.dwChannelID, false);
				LOColor color(colorText);
				pEditLayOut->setColor(color);
				ChatWndUpdate();
				break;
			}
		}
	}
	else
	if(channelID == MAP_CHANNALES_TALK)
	{
		int numberChannels = B2ChatDialog::chatManager.chatChannels.size();
		Ui_Channel_Param channel = B2ChatDialog::chatManager.chatChannels[0];
		for(int i  = 0 ; i < numberChannels; i++)
		{
			channel = B2ChatDialog::chatManager.chatChannels[i];
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chMapName) == 0)
			{
				MapChannelInfo channelInfo;
				char colorText[COMMON_CLIENT_MSG_LEN_32];
				if (KUiChanMgr::getSinglton().getChanInfo(MAP_ROOM_ID, channelInfo))
				{
					memset(colorText, 0, sizeof(colorText));
					strcpy(colorText, channelInfo.szColor);
				}
				else
				{
					memset(colorText, 0, sizeof(colorText));
					strcpy(colorText, KUiChanMgr::getSinglton().getChanColor(channel.dwChannelID, false));
				}
				LOColor color(colorText);
				pEditLayOut->setColor(color);
				ChatWndUpdate();
				break;
			}
		}

	}
};

void    ChatEditBox::ChatEditWndProcessChangeChannel(bool moveup)
{
	int index = 0;
	ChatUiComboBox& uiComboBox = B2ChatDialog::chatManager.ChatManagerGetUiComboBox();
	if(moveup)
	{
		if(uiComboBox.currentSelected == 0)
			return;
		int uIndex = uiComboBox.currentSelected;
		while(uIndex >=0)
		{
			uIndex--;
			if(uiComboBox.downDialgButtonEnable[uIndex] == true)
			{
				uiComboBox.currentSelected = uIndex;
				break;
			}
		}
		if(uIndex<0)
			return;
		
	}
	else
	{
		if(uiComboBox.currentSelected == NUMBER_CHANNALS_CAN_TALK-1)
			return;
		int uIndex = uiComboBox.currentSelected;
		while(uIndex<= NUMBER_CHANNALS_CAN_TALK-1)
		{
			uIndex++;
			if(uiComboBox.downDialgButtonEnable[uIndex]== true)
			{
				uiComboBox.currentSelected =uIndex;
				break;
			}
		}
		if(uIndex>NUMBER_CHANNALS_CAN_TALK-1)
			return;
	}
	index = uiComboBox.currentSelected;
	uiComboBox.selectItemButton.ChatWndSetText(uiComboBox.downDialgButtons[index].ChatWndGetText());
	uiComboBox.selectItemButton.ChatWndSetAllAttr(uiComboBox.downDialgButtons[index].ChatWndGetAttr());
	uiComboBox.selectItemButton.ChatWndUpdate();
	if(ChatEditIsCloseInfo())
		return;
	ChatEditSetChannelColor(index);
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(TRUE);
}
void    ChatEditBox::ChatEditDeleteReturnCharW(wchar_t* pText)
{
	if(pText == 0)
		return ;
	int len  = wcslen(pText);
	if(len == 0)
		return;
	wchar_t* pTempText = new wchar_t[len+1];
	memcpy(pTempText,pText,len);
	pTempText[len] = 0;
	memset(pText,0,len);
	int i = 0,j = 0;
	WCHAR info[] =L"\n";
	while(i < len)
	{
		if(pTempText[i] != info[0])
		{
			pText[j] = pTempText[i];
			j++;
		}
		i++;
	}
	delete [] pTempText;
}
void     ChatEditBox::ChatEditDelteReturnChar(char* pText)
{
	if(pText == 0)
		return ;
	int len  = strlen(pText);
	if(len == 0)
		return;
	char* pTempText = new char[len+1];
	memcpy(pTempText,pText,len);
	pTempText[len] = 0;
	memset(pText,0,len);
	int i = 0,j = 0;
	while(i < len)
	{
		if(pTempText[i] != '\n')
		{
			pText[j] = pTempText[i];
			j++;
		}
		i++;
	}
	delete [] pTempText;
}


void 
ChatEditBox::ClearText( )
{
	HDC hRenderDC = GetWindowDC( hChatWndHandle );

	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = hRenderDC;

	pEditLayOut->clearLayout( );

	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = NULL;

	ReleaseDC( hChatWndHandle, hRenderDC );

}
void   ChatEditBox::ChatEditGetElemListForClipbord(char* pText,vector<LOElemInfo*>* pElemInfoList)
{
	if(pText == 0)
		return;
	int subLen = strlen(_CHAT_WND_H_SUBTEXT_FOR_CLIPBORD);
	int length = strlen(pText);
	int eFaceLen = strlen(_CHAT_WND_H_SUBTEXT_END_FOR_CLIPBORD);
	if(length<0)
		return;
	LOElemInfo* pElemInfo = 0;
	if(length<=subLen)
	{
		if(ChatEditBox::ChatEditGetElemForClipbord(pText,0,length,false,pElemInfo))
			pElemInfoList->push_back(pElemInfo);
		return;
	}
	char* pCurrentText = pText;
	char* pSubText = 0;
	char* pFaceEndText = 0;
	while(pCurrentText!=0&&(pSubText=strstr(pCurrentText,_CHAT_WND_H_SUBTEXT_FOR_CLIPBORD))!=0&&
		  (pFaceEndText = strstr(pSubText,_CHAT_WND_H_SUBTEXT_END_FOR_CLIPBORD))!=0)
	{
		int imageLen = pFaceEndText - pSubText;
		if(imageLen>0)
		{
			int textLen = (pSubText - pCurrentText);
			if(ChatEditBox::ChatEditGetElemForClipbord(pCurrentText,0,textLen,false,pElemInfo))
				pElemInfoList->push_back(pElemInfo);
			int length1 = pFaceEndText - pSubText+eFaceLen;
			if(ChatEditBox::ChatEditGetElemForClipbord(pSubText,0,length1,true,pElemInfo))
				pElemInfoList->push_back(pElemInfo);
			pCurrentText = pFaceEndText+eFaceLen;
			continue;
		}
		int tempLen = pFaceEndText - pCurrentText;
		if(ChatEditBox::ChatEditGetElemForClipbord(pCurrentText,0,tempLen+eFaceLen,false,pElemInfo))
			pElemInfoList->push_back(pElemInfo);
		pCurrentText = pFaceEndText+eFaceLen;
	}
	if(ChatEditBox::ChatEditGetElemForClipbord(pCurrentText,0,strlen(pCurrentText),false,pElemInfo))
		pElemInfoList->push_back(pElemInfo);

}
bool   ChatEditBox::ChatEditGetElemForClipbord(char* pText,int pos,int length,bool isImage,LOElemInfo*&pElemInfo)
{
	//此函数为剪贴板设置的，为了能复制表情，在用完pElemInfo后别忘记删除//
	if(!isImage)
	{
		if(pText == 0)
			return false;
		int textLength = strlen(pText);
		if(textLength == 0)
			return false;
		if(pos>=textLength||length == 0||pos<0)
		{
			return false;
		}
		if(pos+length>textLength)
		{
			length = textLength - pos; 
		}
		const char* pCurrentText = pText + pos;
		char* pLayoutText = new char[length+1];
		memset(pLayoutText,0,length+1);
		strncpy(pLayoutText,pCurrentText,length);
		pElemInfo = new LOElemInfo;
		wchar_t* unicodeText = NULL;
	    ansiToUnicode(pLayoutText, unicodeText);
		pElemInfo->elemType = LO_TEXT;
		pElemInfo->content = unicodeText;
		pElemInfo->description = unicodeText;
		pElemInfo->vAlign = LO_VA_BOTTOM;
		pElemInfo->isShowDes = false;
		delete [] unicodeText;
		delete [] pLayoutText;
	}
	else
	{
		if(pText == 0)
			return false;
		int textLength = strlen(pText);
		if(textLength == 0)
			return false;
		if(length == 0)
		{
			return false;
		}
		int id = 0;
		int count = sscanf(pText,_CHAT_WND_H_FACE_ID_FORMAT,&id);
		if(count !=1)
		{
			pElemInfo = new LOElemInfo;
			char* pContent = new char[length+1];
			pContent[length] = 0;
			strncpy(pContent,pText,length);
			wchar_t * pUnicode = NULL;
			ansiToUnicode(pContent,pUnicode);
			delete[] pContent;
			pElemInfo->elemType = LO_TEXT;
			pElemInfo->content = pUnicode;
			pElemInfo->description = pUnicode;
			pElemInfo->vAlign = LO_VA_BOTTOM;
			pElemInfo->isShowDes = false;
			delete [] pUnicode;
		}
		else
		{
			pElemInfo = new LOElemInfo;
			pElemInfo->elemType = LO_IMAGE;
			pElemInfo->gameObj._objType = LO_GO_FACE;
			pElemInfo->gameObj._objId[0] = id;
			pElemInfo->vAlign = LO_VA_BOTTOM;
			const vector<KUiCfgLoader::FaceCfgData>& faceData = KUiCfgLoader::getSingleton().getFaceData().faceList;
			for(int i = 0; i < faceData.size(); ++i)
			{
				if(faceData[i].index == id)
				{
					wchar_t* pUnicode = NULL;
					ansiToUnicode(faceData[i].image,pUnicode);
					pElemInfo->content = pUnicode;
					delete [] pUnicode;
					break;
				}
			}

		}		
	}
	return true;
}
void    ChatEditBox::ChatEditInsertBordClipText(char* pText)
{

	vector<LOElemInfo*> elemInfoList;
	ChatEditBox::ChatEditGetElemListForClipbord(pText,&elemInfoList);
	if(!elemInfoList.empty())
	{
		int size = elemInfoList.size();
		for(int i = 0; i < size; i++)
		{
			LOElemInfo* pElemInfo = elemInfoList[i];
			chatEditInsertElem(*pElemInfo);
		}
	}

	///清除///
	while (!elemInfoList.empty())
	{
		LOElemInfo* pElemInfo = elemInfoList.back();
		elemInfoList.pop_back();
		delete pElemInfo;
		pElemInfo = 0;
	}
	elemInfoList.clear();


}
void     ChatEditBox::ChatEditWndProcessProcessHotKey(WPARAM wParam,LPARAM lParam)
{
	switch(wParam)
	{
	case _CHAT_EDIT_CPY_DELETE_HOT_KEY:
	case _CHAT_EDIT_CPY_HOT_KEY:
		{
			if(OpenClipboard(0))
			{
				if(EmptyClipboard())
				{
					char* ansiText = 0;
					int len = GetLayoutSelectedText(pEditLayOut,ansiText);
					if(len == 0)
					{
						CloseClipboard();
						return ;
					}
					HGLOBAL hData = (char*)GlobalAlloc(GMEM_MOVEABLE, len + 1);
					if(NULL == hData)
					{
						delete [] ansiText;
						CloseClipboard();
						return;
					}
					char* pszData = (char*)GlobalLock(hData);
				    if(NULL == pszData)
					{
						delete [] ansiText;
						CloseClipboard();
						break;
					}
					memcpy(pszData, ansiText, len);
					pszData[len] = 0;
					GlobalUnlock(hData);
					SetClipboardData(CF_TEXT, hData);
					delete [] ansiText;
				}
				CloseClipboard();
			}
			if(wParam == _CHAT_EDIT_CPY_HOT_KEY)
				break;
			ChatEditWndProcessDeleteChar(false);
			
		}
		break;
	case _CHAT_EDIT_PAST_HOT_KEY:
		{
			if(OpenClipboard(0))
			{

				HGLOBAL hData = 0;//GetClipboardData(CF_OEMTEXT);
				UINT format = 0; // 从第一种格式值开始枚举
				while(format = EnumClipboardFormats(format))
				{
					if(format == CF_TEXT)
					{
						hData = GetClipboardData(CF_TEXT);
						if(NULL == hData)
						{
							CloseClipboard();
							return;
						}
						char* data = (char*)GlobalLock(hData);
						int len = strlen(data);
						char* pText = new char[len+1];
						memcpy(pText,data,len);
						pText[len] = 0;
						ChatEditDelteReturnChar(pText);
						GlobalUnlock(hData);				
						CloseClipboard();
						ChatEditWndProcessDeleteChar(false);
/*						wchar_t* unicodeText = NULL;
						ansiToUnicode(pText, unicodeText);
						LOElemInfo newElem;
						newElem.elemType = LO_TEXT;
						newElem.content = unicodeText;
						newElem.description = unicodeText;
						newElem.vAlign = LO_VA_BOTTOM;
						newElem.isShowDes = false;
						chatEditInsertElem(newElem);
						delete[] unicodeText;
						unicodeText = NULL;*/
						ChatEditInsertBordClipText(pText);
						delete [] pText;
						return ;
					}
					else
					if(format == CF_UNICODETEXT)
					{
						hData = GetClipboardData(CF_UNICODETEXT);
						if(hData == 0)
						{
							CloseClipboard();
							return;
						}
						wchar_t* data = (wchar_t*)GlobalLock(hData);
						ChatEditWndProcessDeleteChar(false);
						LOElemInfo newElem;
						newElem.elemType = LO_TEXT;
						newElem.content = data;
						newElem.description = data;
						newElem.vAlign = LO_VA_BOTTOM;
						newElem.isShowDes = false;
						chatEditInsertElem(newElem);
						GlobalUnlock(hData);				
						CloseClipboard();
						return ;
					}
				}
				CloseClipboard();

							
			}
		}
		return ;
/*	case _CHAT_EDIT_FLIP_CTFMON_HOT_KEY:
		{
			if(focus)
			{
				HKL hCurrentKL = GetKeyboardLayout(0);
				char currentIMEDes[_CHAT_EDIT_CTFMON_DES_LENGTH] = {0};
				ImmGetDescription(hCurrentKL,currentIMEDes,_CHAT_EDIT_CTFMON_DES_LENGTH);
				int numberKL = GetKeyboardLayoutList(0,0);
				if(numberKL==0||numberKL == 1)
				{
						return;
				}
				HKL* systemIMES = new HKL[numberKL];
				GetKeyboardLayoutList(numberKL,systemIMES);
				for(int i = 0; i < numberKL; i++)
				{
					char enumIME[_CHAT_EDIT_CTFMON_DES_LENGTH]= {0};
					ImmGetDescription(systemIMES[i],enumIME,_CHAT_EDIT_CTFMON_DES_LENGTH);
					if(strcmp(currentIMEDes,enumIME) == 0)
					{
						if(i == numberKL-1)
							i = 0;
						else
						{
							enumIME[0] = 0;
							ImmGetDescription(systemIMES[i+1],enumIME,_CHAT_EDIT_CTFMON_DES_LENGTH);
							if(strcmp(currentIMEDes,enumIME) == 0)
							{
								continue;
							}
							i++;
						}
						ActivateKeyboardLayout(systemIMES[i],0);
						delete [] systemIMES;
						return ;
					}
				}
				delete[] systemIMES;
			}
		}
		return;*/
	}
}
/*
LRESULT  ChatEditBox::ChatEditProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_KEYUP:
		{
			switch(wParam)
			{
			case VK_SHIFT:
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->bCtlPushed = FALSE;
				return 0;
			}
			
		}
		break;
	case WM_LBUTTONUP:
		{
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessLButtonUp(wParam,lParam);
		
			return 0;
		}
	case WM_LBUTTONDOWN:
		{
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndProcessLButtonDown(wParam ,lParam);

			return 0;
		}
	case WM_SETFOCUS:
		{ 
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->focus = TRUE;
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->showCarat(TRUE,TRUE);
		
		}
		return 0;
	case WM_HOTKEY:
		{
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessProcessHotKey(wParam,lParam);
			return 0;
		}
	case WM_KILLFOCUS:
		{
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->focus = FALSE;
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->showCarat(FALSE,FALSE);
		}
		return 0;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc =  BeginPaint(hwnd,&ps);
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return 0;
	case WM_MOUSEMOVE:
		{
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessMouseMove(wParam,lParam);
			return 0;
		}
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);

		}
		return FALSE;
	case WM_KEYDOWN:
		{
			if(GetFocus()!=hwnd)
			{
				B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
				return FALSE;
			}
			switch(wParam)
			{
			case VK_SHIFT:
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->bCtlPushed = TRUE;
				return 0;
			case VK_LEFT:
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessCaretLeftMove();
				return 0;
			case VK_RIGHT:
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessCaretRightMove();
				return 0;
			case VK_BACK:
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessDeleteChar(true);
				return 0;
			case VK_RETURN:
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessReturn();
				return 0;
			case VK_DELETE:
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessKeyDelete();
				return 0;
			case VK_NEXT:
				{
					B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessChangeChannel(false);
				}
				return 0;
			case VK_PRIOR:
				{
					B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessChangeChannel(true);
				}
				return 0;
					
			case 'C':
				{
					if(GetKeyState(VK_CONTROL)&0x8000)
					{
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessProcessHotKey(_CHAT_EDIT_CPY_HOT_KEY,0);
					}
				}
				return 0;
			case 'V':
				{
					if(GetKeyState(VK_CONTROL)&0x8000)
					{
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessProcessHotKey(_CHAT_EDIT_PAST_HOT_KEY,0);
					}
				}
				return 0;
			case 'X':
				{
					if(GetKeyState(VK_CONTROL)&0x8000)
					{
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessProcessHotKey(_CHAT_EDIT_CPY_DELETE_HOT_KEY,0);
					}
				}
				return 0;
			case VK_UP:
				{
					if(KUiChatInputWnd::GetSingleton().haveCachedMessage(true))
					{
						ChatEditBox* pEdit = B2ChatDialog::chatManager.ChatManagerGetEditBox();
						pEdit->pEditLayOut->clearLayout();
						pEdit->pEditLayOut->SetText(_CHAT_EIDT_DEFAULT_STRING);
						KUiChatInputWnd::GetSingleton().prevMessage(pEdit->pEditLayOut);
						LOElemInfo* pElem = 0;
						int numbers = pEdit->pEditLayOut->getElemList(pElem);
						if(numbers<=0)
						{
							delete [] pElem;
							return false;
						}
						pEdit->pEditLayOut->clearLayout();
						pEdit->pEditLayOut->SetText(_CHAT_EIDT_DEFAULT_STRING);
						pEdit->pEditLayOut->flashLayout();
						pEdit->pt.x = 2;
						pEdit->pEditLayOut->setSelection(0,0);
						pEdit->pEditLayOut->showCarat(1,1);
						for(int i = 0; i < numbers ; i ++)
						{
							pEdit->chatEditInsertElem(pElem[i]);
						}
						pEdit->ChatWndUpdate();
						delete [] pElem;
						
					}
				}
				return 0;
			case VK_DOWN:
				{
					if(KUiChatInputWnd::GetSingleton().haveCachedMessage(false))
					{
						ChatEditBox* pEdit = B2ChatDialog::chatManager.ChatManagerGetEditBox();
						pEdit->pEditLayOut->clearLayout();
						pEdit->pEditLayOut->SetText(_CHAT_EIDT_DEFAULT_STRING);
						KUiChatInputWnd::GetSingleton().nextMessage(pEdit->pEditLayOut);
						LOElemInfo* pElem = 0;
						int numbers = pEdit->pEditLayOut->getElemList(pElem);
						if(numbers<=0)
						{
							delete [] pElem;
							return false;
						}
						pEdit->pEditLayOut->clearLayout();
						pEdit->pEditLayOut->SetText(_CHAT_EIDT_DEFAULT_STRING);
						pEdit->pEditLayOut->flashLayout();
						pEdit->pt.x = 2;
						pEdit->pEditLayOut->setSelection(0,0);
						pEdit->pEditLayOut->showCarat(1,1);
						for(int i = 0; i < numbers ; i ++)
						{
							pEdit->chatEditInsertElem(pElem[i]);
						}
						pEdit->ChatWndUpdate();
						delete [] pElem;
						
					}
				}
				return 0;
			}
		}
		break;
	case WM_CHAR:
	case WM_IME_CHAR:
		{
			WCHAR   wChar[2] = { 0};
			if(wParam >= 0x80)
			{
				char rChar[4] = {0};
				rChar[0] = ((wParam>>8)&0xff);
				rChar[1] = ((wParam&0xff));
				rChar[2] = ((wParam>>24)&0xff);
				rChar[3] = ((wParam>>16)&0xff);
				MultiByteToWideChar( CP_ACP, 0, rChar, -1, (unsigned short *)wChar, sizeof(WCHAR) );
			}
			else
			{
				if(wParam < 0x20)
					break;
				char tempChar[2]= {0};
				tempChar[0] = wParam;
				MultiByteToWideChar( CP_ACP, 0, tempChar, -1, (unsigned short *)wChar, sizeof(WCHAR) );
			}
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditInsertChar(wChar);
	//		MessageBox(NULL,"","",MB_OK);
			return 0;
			
		}
		return 0;
	}
	return CallWindowProc(B2ChatDialog::wndEditProc,hwnd,msg,wParam,lParam);
}*/
BOOL ChatEditBox::ChatWndCreate(int id,long style,HWND parent,const TCHAR* text,
								const TCHAR* className, int x,int y,int width,int height)
{
	if(ChatWnd::ChatWndCreate(id,style,parent,text,className,x,y,width,height)== FALSE)
		return FALSE;
	HDC hdc = GetWindowDC(hChatWndHandle);
	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = hdc;
	CreateLayout(&pEditLayOut,B2ChatDialog::pEditRender);

	LORect rc;
	rc.setPos(dx,0);
	rc.setWidth(width-2*dx);
	rc.setHeight(height);
	pEditLayOut->setClipper(rc);
	pEditLayOut->SetText(_CHAT_EIDT_DEFAULT_STRING);
	ReleaseDC(hChatWndHandle,hdc);
	((GDIRender*)B2ChatDialog::pEditRender) ->hCurrentDC = 0;
	ChatWndUpdate();
	HIMC  hImc0 = ImmCreateContext();
	hImc = ImmAssociateContext(hChatWndHandle,hImc0);
//	RegisterHotKey(hChatWndHarawndle,_CHAT_EDIT_CPY_HOT_KEY,MOD_CONTROL,(UINT)'C');
//	RegisterHotKey(hChatWndHandle,_CHAT_EDIT_FLIP_CTFMON_HOT_KEY,MOD_CONTROL|MOD_SHIFT,(UINT)0);
	SetClassLong(hChatWndHandle,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
	
	return TRUE;
}
void ChatEditBox::ChatEditWndProcessCaretRightMove()
{
	int start= 0,end = 0;
	pEditLayOut->getSelection(start,end);
	HDC hdc = GetWindowDC(hChatWndHandle);
	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = hdc;
	if(start!=end&&!bCtlPushed)
	{
		pEditLayOut->setSelection(end,end);
		((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = 0;
		ReleaseDC(hChatWndHandle,hdc);
		ChatWndUpdate();
		return ;
	}
	if(end > pEditLayOut->getWordCount())
	{
		((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = 0;
		ReleaseDC(hChatWndHandle,hdc);
		return ;
	}
	end++;
	LOPoint pos = pEditLayOut->getPosAtWordIndex(end);
	pos.x += pt.x;
	if(pos.x+dx-rect.right>0)
		pt.x -= pos.x+dx-rect.right;
	if(bCtlPushed)
	{
		pEditLayOut->setSelection(start,end);
		pEditLayOut->showCarat(TRUE,FALSE);
	}
	else
		pEditLayOut->setSelection(end,end);
	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = 0;
	ReleaseDC(hChatWndHandle,hdc);
	ChatWndUpdate();
}

void ChatEditBox::ChatEditWndProcessCaretLeftMove()
{
	int start = 0,end = 0;
	pEditLayOut->getSelection(start,end);
	HDC hdc = GetWindowDC(hChatWndHandle);
	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = hdc;
	if(start!=end&&!bCtlPushed)
	{
		pEditLayOut->setSelection(end,end);
		((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = 0;
		ReleaseDC(hChatWndHandle,hdc);
		ChatWndUpdate();
		return ;

	}
	if(end == 0)
	{
		((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = 0;
		ReleaseDC(hChatWndHandle,hdc);
		return ;
	}
	end--;
	LOPoint pos = pEditLayOut->getPosAtWordIndex(end);
	pos.x += pt.x;
	if(pos.x<dx)
		pt.x -= pos.x;
	if(bCtlPushed)
	{
		pEditLayOut->setSelection(start,end);
		pEditLayOut->showCarat(TRUE,FALSE);
	}
	else
		pEditLayOut->setSelection(end,end);
	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = 0;
	ReleaseDC(hChatWndHandle,hdc);
	ChatWndUpdate();


}

void ChatEditBox::ChatEditWndProcessReturn()
{	

	LOElemInfo* pElem = 0;
	int cont  = pEditLayOut->getElemList(pElem);
	if(cont <=0)
	{
		delete [] pElem;
		return;
	}
	bool bGM = g_pCoreShell->GetGameData(GDI_IS_GM, NULL, NULL) > 0 ? true : false;
	if( !bGM )
	{
		if(B2ChatDialog::chatManager.ChatManagerGetUiComboBox().currentSelected == WORLD_CHANNALES_TALK && isCloseInfo == false)
		{
			if(lastSendTime!=0.0f)
			{
				int intervalTime = 0;
				g_pCoreShell->GetGameData(GDI_MAP_CHANNEL_TIME, (unsigned int)&intervalTime, 0);
				float currentTime = (float)GetTickCount()/1000.0f;
				if(currentTime - lastSendTime < intervalTime)
				{
					KUiChatInputWnd::GetSingleton().canSay(SYSTEM_ROOM_ID);
					B2ChatDialog::chatManager.ChatClientInsertSystemMsg(MSG_CHAT_TOOFAST);
//					pEditLayOut->clearLayout();
					B2ChatDialog::chatManager.ChatManagerGetEditBox()->ClearText( );
					pEditLayOut->SetText(_CHAT_EIDT_DEFAULT_STRING);
					pEditLayOut->flashLayout();
					pt.x = dx;
					pEditLayOut->setSelection(0,0);
					pEditLayOut->showCarat(1,1);
					SetFocus(hChatWndHandle);
					ChatWndUpdate();
					return ;
				}
			}
			lastSendTime = (float)GetTickCount()/1000.0f;
		}
	}
	KUiChatInputWnd::GetSingleton().cacheMessage(pElem,cont);
	delete [] pElem ;
	pElem = 0;
	B2ChatDialog::chatManager.ChatManagerSendMessageToServer();
//	pEditLayOut->clearLayout();
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ClearText( );
	pEditLayOut->SetText(_CHAT_EIDT_DEFAULT_STRING);
	pEditLayOut->flashLayout();
	pt.x = dx;
	pEditLayOut->setSelection(0,0);
	pEditLayOut->showCarat(1,1);
	SetFocus(hChatWndHandle);
	ChatWndUpdate();
	if(isCloseInfo)
	{
		ChatEditSetChannelColor(B2ChatDialog::chatManager.ChatManagerGetUiComboBox().currentSelected);
		isCloseInfo = false;
	}
}
void ChatEditBox::ChatEditWndProcessDeleteChar(bool isLeft)
{
	int start=0,end = 0;
	pEditLayOut->getSelection(start,end);
	HDC hdc = GetWindowDC(hChatWndHandle);
	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = hdc;
	if(end == start)
	{
		if(isLeft)
		{
			if(end==0)
			{
				((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = 0;
				ReleaseDC(hChatWndHandle,hdc);
				return ;
			}
			int tempWidth = 0;
			if(pt.x<dx)
			{
				int w1 = pEditLayOut->getPosAtWordIndex(start-1).x;
				int w2 = pEditLayOut->getPosAtWordIndex(start).x;
				tempWidth = w2 - w1;
			}
			pEditLayOut->setSelection(start-1,end);
			pEditLayOut->eraseSelection();
			pt.x +=tempWidth;
			if(pt.x > dx)
				pt.x =dx;
		}
	}
	else
	{
		int tempWidth = 0;
		if(end > start)
		{
			int w1 = pEditLayOut->getPosAtWordIndex(start).x;
			int w2 = pEditLayOut->getPosAtWordIndex(end).x;
			tempWidth = w2-w1;
		}
		pEditLayOut->eraseSelection();
		pt.x+=tempWidth;
		if(pt.x>dx)
			pt.x-=tempWidth;

	}
	ReleaseDC(hChatWndHandle,hdc);
	ChatWndUpdate();

}

void ChatEditBox::ChatEditWndProcessLButtonUp(WPARAM wParam,LPARAM lParam)
{
	ReleaseCapture();
	mouseExtra = 0;
	SetFocus(hChatWndHandle);
}
void ChatEditBox::ChatEditWndProcessMouseMove(WPARAM wParam,LPARAM lParam)
{
	if(GetFocus()!=hChatWndHandle)
		return ;
	if(wParam&MK_LBUTTON)
	{
		HDC hdc = GetWindowDC(hChatWndHandle);
		int start= 0;
		int end = 0;
		((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = hdc;
		pEditLayOut->getSelection(start,end);
		LOPoint mousePt;
		mousePt.x = LOWORD(lParam);
		mousePt.y = HIWORD(lParam);
		bool isOutLeft =FALSE;
		bool isOutRight = FALSE;
		if(mousePt.x >2000)
		{
			mousePt.x = dx;
			isOutLeft = TRUE;
		}
		else
		if(mousePt.x >rect.right)
		{
			isOutRight = TRUE;
			mousePt.x = rect.right + dx;
		}
		mousePt.x -= pt.x;
		int indexPos = pEditLayOut->wordIndexAtPixel(mousePt.x,10);
		pEditLayOut->setSelection(start,indexPos);
		if(isOutLeft)
		{
			if(pt.x<0&&mouseExtra<0)
			{
				int pos = pEditLayOut->getPosAtWordIndex(indexPos).x;
				pos += pt.x;
				pt.x -= pos;
				mouseExtra = _CHAT_EDIT_MOUSE_EXTRA;
			}
			else
			{
				mouseExtra--;
			}
			

		}
		else
		if(isOutRight)
		{			
			if(mouseExtra<0&&indexPos<pEditLayOut->getWordCount())
			{
				int pos = pEditLayOut->getPosAtWordIndex(indexPos+1).x;
				pos+=pt.x;
				pt.x-= pos+dx-rect.right;
				pEditLayOut->setSelection(start,indexPos+2);
				mouseExtra = _CHAT_EDIT_MOUSE_EXTRA;
			}
			else
			{
				mouseExtra--;
				pEditLayOut->setSelection(start,end);
			}
		}
		ReleaseDC(hChatWndHandle,hdc);
		ChatWndUpdate();
	}

}
void ChatEditBox::ChatEditWndShowText()
{
	int startIndex = 0;
	int endIndex = 0;
	pEditLayOut->getSelection(startIndex, endIndex);
	
	int caratPosXInLayout = pEditLayOut->getPosAtWordIndex(endIndex).x;
	int inputBoxWidth = rect.right-6;//d_inputBox->getUnclippedPixelRect().getWidth();
	int inputBoxHeight = rect.bottom;//d_inputBox->getUnclippedPixelRect().getHeight();
	int layoutOffX = pt.x;
	
	int layoutHeight = pEditLayOut->getRenderArea(false).getHeight();
	if(layoutHeight == 0)
	{
		layoutHeight = 14;//特殊处理，当没有内容的时候则给定一个排版高度
	}
	if(caratPosXInLayout + layoutOffX < 0)
	{
		pt.x = -caratPosXInLayout;
		pt.y = (inputBoxHeight - layoutHeight)/2;
	}
	else if(caratPosXInLayout + layoutOffX > inputBoxWidth)
	{
		pt.x = inputBoxWidth - caratPosXInLayout;
		pt.y = (inputBoxHeight - layoutHeight)/2;
	}
	else
	{
		pt.x = layoutOffX;
		pt.y= (inputBoxHeight - layoutHeight)/2;
	}
}
void ChatEditBox::ChatEditWndSetFocus(BOOL bFocus)
{
//	focus = bFocus;
	if(bFocus)
	{
		SetFocus(hChatWndHandle);
		pEditLayOut->showCarat(TRUE,TRUE);
	}
	else
	{
		pEditLayOut->showCarat(FALSE,FALSE);
	}
}
void ChatEditBox::ChatWndProcessLButtonDown(WPARAM wParam ,LPARAM lParam)
{
	SetCapture(hChatWndHandle);
	////估计时间////
	mouseExtra = _CHAT_EDIT_MOUSE_EXTRA;
	ChatEditWndSetFocus(TRUE);
	LOElemInfo* info;
	int num = pEditLayOut->getElemList(info);
	if(num!=0)
	{
		HDC hdc = GetWindowDC(hChatWndHandle);
		((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = hdc;
		LOPoint mousePos;
		mousePos.x = LOWORD(lParam);
		mousePos.y = HIWORD(lParam);
		///修正鼠标位置////
		mousePos.x -=pt.x;
		int indexPos = pEditLayOut->wordIndexAtPixel(mousePos.x,mousePos.y);
		pEditLayOut->setSelection(indexPos+1,indexPos+1);
		ReleaseDC(hChatWndHandle,hdc);
	}		
	ChatWndUpdate();
//	B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(FALSE);
	delete [] info;
}
void ChatEditBox::chatEditInsertElem(LOElemInfo& Info)
{
	if(LO_GO_ITEM == Info.gameObj._objType 
		&& KUiChatInputWnd::layoutGOCount(pEditLayOut, LO_GO_ITEM) >= MAXSIZE_SYNC_ITEM_COUNT)
	{
		return;
	}
	Info.vAlign = LO_VA_BOTTOM;
	HDC hdc = GetWindowDC(hChatWndHandle);
	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = hdc;
	pEditLayOut->insertElem(Info);
	pEditLayOut->flashLayout();
	int start = 0;
	int end = 0;
	pEditLayOut->getSelection(start,end);
	LOPoint lopt = pEditLayOut->getPosAtWordIndex(end);
	lopt.x +=pt.x;
	if(lopt.x+dx>rect.right)
	{
		pt.x-=lopt.x+dx-rect.right;
	}
	ChatEditWndFashColor();
	ReleaseDC(hChatWndHandle,hdc);
	ChatWndUpdate();
}
void ChatEditBox::ChatEditInsertChar(WCHAR* wChar)
{
	LOElemInfo info ;
	info.vAlign = LO_VA_BOTTOM;
	info.content.set(wChar);
    HDC hdc = GetWindowDC(hChatWndHandle);
	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = hdc;
	pEditLayOut->insertElem(info);
	int start = 0;
	int end = 0;
	pEditLayOut->getSelection(start,end);
	LOPoint lopt = pEditLayOut->getPosAtWordIndex(end);
	lopt.x +=pt.x;
	if(lopt.x>rect.right-dx)
	{
		pt.x-=lopt.x+dx-rect.right;
	}
	ChatEditWndFashColor();
	ReleaseDC(hChatWndHandle,hdc);
	ChatWndUpdate();
}
void ChatEditBox::ChatWndDrawItem(HDC hdc)
{

	HDC hdc2 = CreateCompatibleDC(hdc);
	HBITMAP hBitmap2 = CreateCompatibleBitmap(hdc,rect.right,rect.bottom);
	HBITMAP hOld2 = (HBITMAP)SelectObject(hdc2,hBitmap2);
	DrawBitmap(hdc2,ChatResource::GetSingle().GetResource(wndSrc.normal_idx)->hBitmap,rect.right,rect.bottom,0,0);

	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC =hdc2;
	pEditLayOut->flashLayout();
	ChatEditWndShowText();
	pEditLayOut->Render(pt.x,pt.y,0);
	((GDIRender*)B2ChatDialog::pEditRender)->hCurrentDC = 0;
	StretchBlt(hdc,0,0,rect.right,rect.bottom,hdc2,0,0,rect.right,rect.bottom,SRCCOPY);
	SelectObject(hdc2,hOld2);
	DeleteObject(hBitmap2);
	DeleteDC(hdc2);


}
LPCHATWND ChatEditBox::ChatWndGetChatWndPointer()
{
	return (LPCHATWND)this;
}



void ChatEditBox::ChatEditWndProcessKeyHome()
{
	pt.x = 2;
	pEditLayOut->setSelection(0,0);
	pEditLayOut->showCarat(true,false);
	ChatWndUpdate();
}
void ChatEditBox::ChatEditWndProcessKeyEnd()
{
	int width = pEditLayOut->getRenderArea(true).getWidth();
	int wordCount = pEditLayOut->getWordCount();
	pEditLayOut->setSelection(wordCount,wordCount);
	pEditLayOut->showCarat(true,false);
	if(width < rect.right-4)
	{
		ChatWndUpdate();
		return;
	}
	int dWidth = width - rect.right - 4;
	pt.x = 2- dWidth;
	ChatWndUpdate();

}

void    ChatEditBox::ChatEditWndProcessKeyDelete()
{
	int start=0;
	int end = 0;
	pEditLayOut->getSelection(start,end);
	if(start != end)
		ChatEditWndProcessDeleteChar(false);
	
}


ChatScrollBar::ChatScrollBar()
{
	scrollType = 0;
	scrollBarX = 0;
	scrollBarY = 0;
	scrollBarWidth = 0;
	scrollBarHeight = 0;
	scrollBarUpWidth = 0;
	scrollBarUpHeight = 0;
	scrollBarDownWidth = 0;
	scrollBarDownHeight = 0;
	scrollLength = 0;
//    scrollCurrentPersent = 0;
	hScrollParent = 0;
	
}

ChatScrollBar::~ChatScrollBar()
{

}

void ChatScrollBar::ChatScrollBarSetScrolls(int allLength,int oneLength)
{
	scrollBarRectPos = 0;
	if(allLength==0)
	{
		oneLineLength = 0;
		return;
	}
	float height = (float)scrollLength*(float)oneLength/(float)allLength;
	oneLineLength = height;
}

BOOL ChatScrollBar::ChatScrollBarProcessClickUpButton(const POINT& pt)
{
	if(oneLineLength == 0)
		return FALSE;
	RECT rc;
	rc.left = scrollBarX;
	rc.right = rc.left+scrollBarUpWidth;
	rc.top = scrollBarY;
	rc.bottom = scrollBarY+scrollBarUpHeight;
	if(IsInRect(pt,rc))
	{
		scrollBarRectPos-=oneLineLength;
		if(scrollBarRectPos<0)
			scrollBarRectPos = 0;
		ChatScrolBarUpdate();
		return TRUE;
	}
	return FALSE;
}

BOOL ChatScrollBar::ChatScrollBarProcessClickScrollBar(const POINT& pt)
{
	if(oneLineLength == 0)
		return false;
	RECT rc;
	rc.left = scrollBarX;
	rc.right = rc.left + scrollLengthRectWidth;
	rc.top  = scrollBarY + scrollBarUpHeight + scrollBarRectPos;
	rc.bottom = rc.top + scrollBarRectHeight;
	if(IsInRect(pt,rc))
	{
		if(!isSetCapture)
		{
			SetCapture(hScrollParent);
			isSetCapture = TRUE;
			POINT pt;
		    GetCursorPos(&pt);
		    ScreenToClient(hScrollParent,&pt);
			currentMousePos = pt;
			scrollBarDt      = 0;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL ChatScrollBar::ChatScrollBarProcessClickDownButton(const POINT& pt)
{
	if(oneLineLength == 0)
		return FALSE;
	RECT rc;
	rc.left = scrollBarX;
	rc.right = rc.left + scrollBarDownWidth;
	rc.top  = scrollBarY + scrollBarUpHeight + scrollLength;
	rc.bottom = rc.top + scrollBarDownHeight;
	if(IsInRect(pt,rc))
	{
		scrollBarRectPos+=oneLineLength;
		if(scrollBarRectPos>scrollLength -scrollBarRectHeight )
			scrollBarRectPos = scrollLength -scrollBarRectHeight;
		ChatScrolBarUpdate();
		return TRUE;
	}
	return FALSE;
}

int ChatScrollBar::ChatScrollBarProcessMouseMove(const POINT& pt)
{
	RECT rc,rc1,rc2;
	rc.left = scrollBarX;
	rc.right = rc.left+scrollBarUpWidth;
	rc.top = scrollBarY;
	rc.bottom = scrollBarY+scrollBarUpHeight;

	rc1.left = scrollBarX;
	rc1.right = rc1.left+scrollBarRectWidth;
	rc1.top = scrollBarY+scrollBarUpHeight+scrollBarRectPos;
	rc1.bottom = rc1.top+scrollBarRectHeight;

	rc2.left = scrollBarX;
	rc2.right = rc2.left+scrollBarRectWidth;
	rc2.top = scrollBarY+scrollBarUpHeight+scrollLength;
	rc2.bottom = rc2.top+scrollBarRectHeight;
	if(isSetCapture)
	{
		if(oneLineLength==0)
			return FALSE;
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hScrollParent,&pt);
		int dist = pt.y - currentMousePos.y;
		scrollBarDt+=dist;
		currentMousePos.x = pt.x;
		currentMousePos.y = pt.y;
		int dt = scrollBarDt;
		if(dt<0)
			dt = -dt;
		int num = dt/oneLineLength;
		if(num > 0)
		{
			if(scrollBarDt>0)
			{
				scrollBarRectPos = scrollBarRectPos + dist+(scrollBarDt - num*oneLineLength);
			    if(scrollBarRectPos > scrollLength - scrollBarRectHeight)
					scrollBarRectPos = scrollLength - scrollBarRectHeight;
				scrollBarDt = 0;
				ChatScrolBarUpdate();
				return 1;
			}
			else
			{
				scrollBarRectPos = scrollBarRectPos+dist - (scrollBarDt + num*oneLineLength);
				if(scrollBarRectPos<0) 
					scrollBarRectPos = 0;
				scrollBarDt = 0;
				ChatScrolBarUpdate();
				return -1;

			}
		}
		else
		{
			scrollBarRectPos +=dist;
			if(scrollBarRectPos<0) scrollBarRectPos = 0;
			if(scrollBarRectPos > scrollLength - scrollBarRectHeight)
				scrollBarRectPos = scrollLength - scrollBarRectHeight;
			ChatScrolBarUpdate();
			return 0;
		}
	}
	if(IsInRect(pt,rc))
	{
		if(scrollBarUpState == SCROLLBAR_STATE_NORMAL)
		{
			scrollBarUpState   = SCROLLBAR_STATE_MOUSEOVER;
			scrollBarDownSate   = SCROLLBAR_STATE_NORMAL;
			scrollBarRectState = SCROLLBAR_STATE_NORMAL	;			
			ChatScrolBarUpdate();
			return 0;
		}
	}
	else
	if(IsInRect(pt,rc1))
	{
		if(scrollBarRectState == SCROLLBAR_STATE_NORMAL)
		{
				scrollBarUpState   = SCROLLBAR_STATE_NORMAL ;
				scrollBarDownSate   = SCROLLBAR_STATE_NORMAL;
				scrollBarRectState = SCROLLBAR_STATE_MOUSEOVER	;
				ChatScrolBarUpdate();
				return 0;
		}
	}
	else
	if(IsInRect(pt,rc2))
	{
		if(scrollBarDownSate == SCROLLBAR_STATE_NORMAL)
		{
			scrollBarUpState   = SCROLLBAR_STATE_NORMAL ;
			scrollBarDownSate   = SCROLLBAR_STATE_MOUSEOVER ;
			scrollBarRectState = SCROLLBAR_STATE_NORMAL	;
			ChatScrolBarUpdate();
			return 0;
		}
	}
	else
	{
		if(scrollBarUpState != SCROLLBAR_STATE_NORMAL||
			scrollBarDownSate!=SCROLLBAR_STATE_NORMAL||
			scrollBarRectState != SCROLLBAR_STATE_NORMAL)
		{
			scrollBarUpState   = SCROLLBAR_STATE_NORMAL;
			scrollBarDownSate   = SCROLLBAR_STATE_NORMAL;
			scrollBarRectState = SCROLLBAR_STATE_NORMAL	;
			ChatScrolBarUpdate();
		}
	}
	return 0;
	
}

void ChatScrollBar::ChatScrolBarUpdate()
{
	RECT rc;
	rc.left = scrollBarX;
	rc.top = scrollBarY;
	rc.right = rc.left + scrollBarWidth;
	rc.bottom = rc.top + scrollBarHeight;
	InvalidateRect(hScrollParent,&rc,TRUE);
	UpdateWindow(hScrollParent);
}

#define SCROLLBAR_X                   "x"
#define SCROLLBAR_Y                   "y"
#define SCROLLBAR_WIDTH               "width"
#define SCROLLBAR_HEIGHT              "height"
#define SCROLLBAR_DOWN_WIDTH          "down_width"
#define SCROLLBAR_DOWN_HEIGHT         "down_height"
#define SCROLLBAR_UP_WIDTH            "up_width"
#define SCROLLBAR_UP_HEIGHT           "up_height"
#define SCROLLBAR_BAR_WIDTH           "scroll_bar_width"
#define SCROLLBAR_BAR_HEIGHT          "scroll_bar_height"

#define SCROLLBAR_UP_SRC_NORMAL       "up_part_normal_src"
#define SCROLLBAR_UP_SRC_MOUSEOVER    "up_part_mouseOver_src"
#define SCROLLBAR_UP_SRC_MOUSEDOWN    "up_part_mouseDown_src"

#define SCROLLBAR_DOWN_SRC_NORMAL       "down_part_normal_src"
#define SCROLLBAR_DOWN_SRC_MOUSEOVER    "down_part_mouseOver_src"
#define SCROLLBAR_DOWN_SRC_MOUSEDOWN    "down_part_mouseDown_src"

#define SCROLLBAR_BAR_SRC_NORMAL       "scroll_bar_normal_src"
#define SCROLLBAR_BAR_SRC_MOUSEOVER    "scroll_bar_mouseOver_src"
#define SCROLLBAR_BAR_SRC_MOUSEDOWN    "scroll_bar_mouseDown_src"
#define SCROLLBAR_RECT_SRC             "scroll_rect_src"

#define SCROLLBAR_RECT_WIDTH           "scroll_bar_rect_Width"

#define SCROLLBAR_INI_NAME             "friendScrollBar"
void ChatScrollBar::ChatScrollBarLoadIniCtg()
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
//	GetCurrentDirectory(MAX_PATH,szPath);
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,SCROLLBAR_X,0,&scrollBarX);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,SCROLLBAR_Y,0,&scrollBarY);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,SCROLLBAR_WIDTH,0,&scrollBarWidth);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,SCROLLBAR_HEIGHT,0,&scrollBarHeight);


	iniFile.GetInteger(SCROLLBAR_INI_NAME,SCROLLBAR_UP_WIDTH,0,&scrollBarUpWidth);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,SCROLLBAR_UP_HEIGHT,0,&scrollBarUpHeight);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,SCROLLBAR_DOWN_WIDTH,0,&scrollBarDownWidth);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,SCROLLBAR_DOWN_HEIGHT,0,&scrollBarDownHeight);

	iniFile.GetInteger(SCROLLBAR_INI_NAME,SCROLLBAR_BAR_WIDTH,0,&scrollBarRectWidth);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,SCROLLBAR_BAR_HEIGHT,0,&scrollBarRectHeight);

	iniFile.GetInteger(SCROLLBAR_INI_NAME,SCROLLBAR_RECT_WIDTH,0,&scrollLengthRectWidth);
	int normal_idx = 0,hover_idx = 0,pushed_idx = 0,disable_idx = 0;
	iniFile.GetInteger(SCROLLBAR_INI_NAME,"up_part_normal_idx",0,&normal_idx);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,"up_part_mouseOver_idx",0,&hover_idx);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,"up_part_mouseDown_idx",0,&pushed_idx);
	scrollUpPart.SetSrcIdx(normal_idx,hover_idx,pushed_idx,-1);



	iniFile.GetInteger(SCROLLBAR_INI_NAME,"down_part_normal_idx",0,&normal_idx);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,"down_part_mouseOver_idx",0,&hover_idx);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,"down_part_mouseDown_idx",0,&pushed_idx);
	scrollDownPart.SetSrcIdx(normal_idx,hover_idx,pushed_idx,-1);

	iniFile.GetInteger(SCROLLBAR_INI_NAME,"scroll_bar_normal_src",0,&normal_idx);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,"scroll_bar_mouseOver_src",0,&hover_idx);
	iniFile.GetInteger(SCROLLBAR_INI_NAME,"scroll_bar_mouseDown_src",0,&pushed_idx);
	scrollBarPart.SetSrcIdx(normal_idx,hover_idx,pushed_idx,-1);

	iniFile.GetInteger(SCROLLBAR_INI_NAME,"scroll_rect_idx",0,&normal_idx);
	scrollRectPart.SetSrcIdx(normal_idx,-1,-1,-1);

	scrollBarRectPos = 0;
	scrollBarUpState = 0;
	scrollBarDownSate = 0;
	scrollBarRectState = 0;
	scrollLength = scrollBarHeight - scrollBarUpHeight - scrollBarDownHeight;
	ChatScrollBarSetScrolls(0,0);
}
void ChatScrollBar::ChatScrollBarSetParent(HWND hParent)
{ 
	hScrollParent = hParent;
}
void ChatScrollBar::ChatScrollBarMoveDown()
{
	if(oneLineLength == 0)
		return ;
	scrollBarRectPos += oneLineLength;
	if(scrollBarRectPos > scrollLength -oneLineLength)
		scrollBarRectPos = scrollLength - oneLineLength;
	ChatScrolBarUpdate();
}
void ChatScrollBar::ChatScrocllBarMoveUp()
{
	if(oneLineLength == 0)
		return ;
	scrollBarRectPos -= oneLineLength;
	if(scrollBarRectPos < 0)
		scrollBarRectPos = 0;
	ChatScrolBarUpdate();
}
void ChatScrollBar::ChatScrollBarDrawItem(HDC hdc)
{
	ChatResource& resource = ChatResource::GetSingle();
	HDC hdc1 = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc,scrollBarWidth,scrollBarHeight);
	HBITMAP hOld = (HBITMAP)SelectObject(hdc1,hBitmap);
	BitBlt(hdc1,0,0,scrollBarWidth,scrollBarHeight,hdc,scrollBarX,scrollBarY,SRCCOPY);
	HBITMAP hUpBitmap = 0;
	HBITMAP hDownBitmap = 0;
	HBITMAP hBarRectBitmap = 0;
	HBITMAP hscrollRectBitmap = resource.GetResource(scrollRectPart.normal_idx)->hBitmap;
	int x = (scrollBarWidth - scrollLengthRectWidth)/2;
	DrawBitmap(hdc1,hscrollRectBitmap,scrollLengthRectWidth,scrollLength,x,scrollBarUpHeight);
	if(scrollBarUpState == SCROLLBAR_STATE_NORMAL)
		hUpBitmap = resource.GetResource(scrollUpPart.normal_idx)->hBitmap;
	else
	if(scrollBarUpState == SCROLLBAR_STATE_MOUSEOVER)
		hUpBitmap = resource.GetResource(scrollUpPart.hover_idx)->hBitmap;
	else
		hUpBitmap = resource.GetResource(scrollUpPart.pushed_idx)->hBitmap;
	////////////////////////////////////////////////
	if(scrollBarDownSate == SCROLLBAR_STATE_NORMAL)
		hDownBitmap = resource.GetResource(scrollDownPart.normal_idx)->hBitmap;
	else
	if(scrollBarDownSate == SCROLLBAR_STATE_MOUSEOVER)
		hDownBitmap = resource.GetResource(scrollDownPart.hover_idx)->hBitmap;
	else
		hDownBitmap = resource.GetResource(scrollDownPart.pushed_idx)->hBitmap;
	/////////////////////////////////////////////
	if(scrollBarRectState == SCROLLBAR_STATE_NORMAL)
		hBarRectBitmap = resource.GetResource(scrollBarPart.normal_idx)->hBitmap;
	else
	if(scrollBarRectState == SCROLLBAR_STATE_MOUSEDOWN)
		hBarRectBitmap = resource.GetResource(scrollBarPart.pushed_idx)->hBitmap;
	else
		hBarRectBitmap = resource.GetResource(scrollBarPart.hover_idx)->hBitmap;

	DrawBitmap(hdc1,hUpBitmap,scrollBarUpWidth,scrollBarUpHeight,0,0);
	DrawBitmap(hdc1,hDownBitmap,scrollBarDownWidth,scrollBarDownHeight,0,scrollBarUpHeight+scrollLength);
	int cx = (scrollLengthRectWidth - scrollBarRectWidth)/2;
	DrawBitmap(hdc1,hBarRectBitmap,scrollBarRectWidth,scrollBarRectHeight,cx,scrollBarRectPos+scrollBarUpHeight);
	BitBlt(hdc,scrollBarX,scrollBarY,scrollBarWidth,scrollBarHeight,hdc1,0,0,SRCCOPY);
	SelectObject(hdc1,hOld);
	DeleteObject(hBitmap);
	DeleteDC(hdc1);

}


ChatUiComboBox::ChatUiComboBox()
{
	for(int i = 0; i < NUMBER_CHANNALS_CAN_TALK; i++)
	{
		downDialgButtonEnable[i] = false;
	}
}
ChatUiComboBox::~ChatUiComboBox()
{

}

bool ChatUiComboBox::UiComboBoxLoadIniFile(HWND hParent)
{
	KIniFile iniFile;
	CHAR  szPath[MAX_PATH] = { 0},szValue[MAX_PATH]= {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	int x=0,y=0,width=0,height=0,r=0,g=0,b=0;
	iniFile.GetInteger("UiComboBox","buttonX",0,&x);
	iniFile.GetInteger("UiComboBox","buttonY",0,&y);
	iniFile.GetInteger("UiComboBox","buttonWidth",0,&width);
	iniFile.GetInteger("UiComboBox","buttonHeight",0,&height);
	selectItemButton.ChatWndCreate(_COMBOBOX_SELECTED_BUTTON_ID,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,
		                            hParent,"","button",x,y,width,height);
	
	int normal_idx = 0,hover_idx = 0;
	iniFile.GetInteger("UiComboBox","normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger("UiComboBox","mouseOverSrcIdx",0,&hover_idx);
	selectItemButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);

	iniFile.GetString("UiComboBox","fontColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	selectItemButton.ChatWndSetTextNormalColor(RGB(r,g,b));
	iniFile.GetString("UiComboBox","tooltipInfo","",szValue,MAX_PATH);
	if(szValue[0]!=0)
		selectItemButton.ChatWndTipCreate(TTS_NOPREFIX,szValue,200);


	selectItemButton.SetWndProcessFun(ChatWndProcessFun::ProcessChannelChangeButton);

	hDownDialg = CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_CHANNALE_DLG),
		hParent,
		(DLGPROC)ChatUiComboBox::UiComboBoxDownDlgProc);
	char* itemName[]={"UiComboBox-system","UiComboBox-zhuhou","UiComboBox-guojia","UiComboBox-sizhu","UiComboBox-team","UiComboBox-world","UiComboBox-near","UiComboBox-map"};
	int id = _COMBOBOX_DOWN_DIALOG_SYS_BUTTON_ID;
	for(int i = 0;i<NUMBER_CHANNALS_CAN_TALK;i++)
	{
		iniFile.GetInteger(itemName[i],"x",0,&x);
		iniFile.GetInteger(itemName[i],"y",0,&y);
		iniFile.GetInteger(itemName[i],"width",0,&width);
		iniFile.GetInteger(itemName[i],"height",0,&height);
		downDialgButtons[i].ChatWndCreate(id,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,hDownDialg,"","button",x,y,width,height);
		int normal_idx = 0,hover_idx = 0;
		iniFile.GetInteger(itemName[i],"normalSrcIdx",0,&normal_idx);
		iniFile.GetInteger(itemName[i],"mouseOverSrcIdx",0,&hover_idx);
		downDialgButtons[i].ChatWndSetResource(normal_idx,hover_idx,-1,-1);
		char textInfo[256]={0};
		iniFile.GetString(itemName[i],"textInfo","",textInfo,256);
		if(textInfo[0]!=0)
		{
			if (i == MAP_CHANNALES_TALK)
			{
				if (g_pCoreShell)
				{
					if (g_pCoreShell->GetGameData( GDI_IS_PLAYER_IN_COMBAT_WORLD, NULL, NULL ))
					{
						downDialgButtons[i].ChatWndSetText(ChatString::ChatStringGetString().chBattlefield);
					}
					else
					{
						downDialgButtons[i].ChatWndSetText(textInfo);
					}
				}
			}
			else
			{
				downDialgButtons[i].ChatWndSetText(textInfo);
			}
			downDialgButtons[i].ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
			iniFile.GetString(itemName[i],"fontColor","",szValue,MAX_PATH);
			sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			downDialgButtons[i].ChatWndSetTextNormalColor(RGB(r,g,b));

			char font[FONT_SIZE]={0};
			iniFile.GetString(itemName[i],"font","",font,256);
			TFONT tFont;
			strcpy(tFont.fontName,font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0]=0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
				downDialgButtons[i].ChatWndSetFont(font);
			else
			{
				downDialgButtons[i].ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				downDialgButtons[i].ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
		id++;
		buttonsProc[i] = (WNDPROC)SetWindowLong(downDialgButtons[i].ChatWndGetHandle(),GWL_WNDPROC,(LONG)(ChatUiComboBox::UiComBoBoxDownDlgButtonProc));
	}
	downDialgButtonEnable[SYSTEM_CHANNALS_TALK] = true;
	downDialgButtonEnable[WORLD_CHANNALES_TALK] = true;
	downDialgButtonEnable[NEAR_CHANNALES_TALK] = true;
	currentSelected = NEAR_CHANNALES_TALK;
	selectItemButton.ChatWndSetText(downDialgButtons[currentSelected].ChatWndGetText());
	selectItemButton.ChatWndSetFont(downDialgButtons[currentSelected].ChatWndGetFontName());
	selectItemButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
	if(downDialgButtons[currentSelected].ChatWndGetAttr()&_CHAT_WND_ATTR_USE_DEFUALT_FONT)
		selectItemButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
	const char* colorText = KUiChanMgr::getSinglton().getChanColor(LOCAL_ROOM_ID, false);
	LOColor color(colorText);
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->setColor(color);
	return true;
	
}
void ChatUiComboBox::UiComBoBoxAdjustDownDlg()
{
	RECT rc;
	GetWindowRect(selectItemButton.ChatWndGetHandle(),&rc);
	int height = 0;
	for(int i = 0; i < NUMBER_CHANNALS_CAN_TALK;i++)
	{
		if(downDialgButtonEnable[i])
		{
			downDialgButtons[i].y = height;
			downDialgButtons[i].ChatWndMove(downDialgButtons[i].x,height,downDialgButtons[i].ChatWndGetRect().right,downDialgButtons[i].ChatWndGetRect().bottom,true);
			height+=downDialgButtons[i].ChatWndGetRect().bottom;
			ShowWindow(downDialgButtons[i].ChatWndGetHandle(),SW_NORMAL);
		}
		else
			ShowWindow(downDialgButtons[i].ChatWndGetHandle(),FALSE);
	}
	int width = downDialgButtons[0].ChatWndGetRect().right;
	int x =rc.left;
	int y = rc.top-height;
	MoveWindow(hDownDialg,x,y,width,height,TRUE);
}
void ChatUiComboBox::UiComBoBoxProcessClickButton()
{
	if(!isShowDownDlg)
	{
		UiComBoBoxAdjustDownDlg();
		ShowWindow(hDownDialg,SW_NORMAL);
		isShowDownDlg = true;
	}
	else
	{
		ShowWindow(hDownDialg,SW_HIDE);
		isShowDownDlg = false;
	}
}
void ChatUiComboBox::UiComBoBoxDrawDownDlgButton(WPARAM wParam,LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
	switch(lpdis->CtlID)
	{
	case _COMBOBOX_DOWN_DIALOG_TEAM_BUTTON_ID:
		if(downDialgButtonEnable[TEAM_CHANNALES_TALK])
			downDialgButtons[TEAM_CHANNALES_TALK].ChatWndDrawItem(lpdis->hDC);
		return;
	case _COMBOBOX_DOWN_DIALOG_SHIZU_BUTTON_ID:
		if(downDialgButtonEnable[SHIZU_CHANNALES_TALK])
		    downDialgButtons[SHIZU_CHANNALES_TALK].ChatWndDrawItem(lpdis->hDC);
		return;
	case _COMBOBOX_DOWN_DIALOG_GUOJIA_BUTTON_ID:
		if(downDialgButtonEnable[GUOJIA_CHANNALES_TALK])
			downDialgButtons[GUOJIA_CHANNALES_TALK].ChatWndDrawItem(lpdis->hDC);
		return;
	case _COMBOBOX_DOWN_DIALOG_ZHUHOU_BUTTON_ID:
		if(downDialgButtonEnable[ZHUHOU_CHANNALES_TALK])
			downDialgButtons[ZHUHOU_CHANNALES_TALK].ChatWndDrawItem(lpdis->hDC);
		return;
	case _COMBOBOX_DOWN_DIALOG_SYS_BUTTON_ID:
		if(downDialgButtonEnable[SYSTEM_CHANNALS_TALK])
			downDialgButtons[SYSTEM_CHANNALS_TALK].ChatWndDrawItem(lpdis->hDC);
		return;
	case _COMBOBOX_DOWN_DIALOG_WORLD_BUTTON_ID:
		if(downDialgButtonEnable[WORLD_CHANNALES_TALK])
			downDialgButtons[WORLD_CHANNALES_TALK].ChatWndDrawItem(lpdis->hDC);
		return;
	case _COMBOBOX_DOWN_DIALOG_NEAR_BUTTON_ID:
		if(downDialgButtonEnable[NEAR_CHANNALES_TALK])
			downDialgButtons[NEAR_CHANNALES_TALK].ChatWndDrawItem(lpdis->hDC);
		return;

	case _COMBOBOX_DOWN_DIALOG_MAP_BUTTON_ID:
		if(downDialgButtonEnable[MAP_CHANNALES_TALK])
			downDialgButtons[MAP_CHANNALES_TALK].ChatWndDrawItem(lpdis->hDC);
		return;

	}
}
LRESULT ChatUiComboBox::UiComboBoxDownDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{

	switch(msg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_INITDIALOG:
		SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		return FALSE;
	case WM_DRAWITEM:
		B2ChatDialog::chatManager.ChatManagerGetUiComboBox().UiComBoBoxDrawDownDlgButton(wParam,lParam);
		return FALSE;
	case WM_DESTROY:
		{
//			selectItemButton.ChatWndDeleteSrc();
			for(int i = 0; i < NUMBER_CHANNALS_CAN_TALK;i++)
				B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[i].ChatWndDeleteSrc();
		}
		return FALSE;
	}
	return FALSE;
}
LRESULT ChatUiComboBox::UiComBoBoxDownDlgButtonProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int index = 0;
	ChatButton* pButton=0;
	for(int i = 0 ; i < NUMBER_CHANNALS_CAN_TALK;i++)
	{
		if(hWnd == B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[i].ChatWndGetHandle()&&
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtonEnable[i] == true)
		{
			index = i;
			pButton = B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons+i;
			break;
		}
	}
	if(pButton == 0)
		return FALSE;
	switch(msg)
	{

	case WM_MOUSEMOVE:
		{
			if(pButton->ChatWndGetState() !=_CHAT_BUTTON_STATE_MOUSEOVER)
			{
				pButton->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				pButton->ChatWndUpdate();
			}
			for(int i = 0;i < NUMBER_CHANNALS_CAN_TALK;i++)
			{
				if(i!=index)
				{
					if(B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[i].ChatWndGetState()!=_CHAT_BUTTON_STATE_NORMAL)
					{
						B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[i].ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
						B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[i].ChatWndUpdate();
					}
				}
			}
			return FALSE;
		}
	case WM_LBUTTONDOWN:
		{
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().currentSelected = index;
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetText(
				B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[index].ChatWndGetText());
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndSetAllAttr(
				B2ChatDialog::chatManager.ChatManagerGetUiComboBox().downDialgButtons[index].ChatWndGetAttr());
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton.ChatWndUpdate();
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().UiComBoBoxProcessClickButton();
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(TRUE);
			if(B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditIsCloseInfo())
				return false;
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditSetChannelColor(index);
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	}

	return CallWindowProc(B2ChatDialog::chatManager.ChatManagerGetUiComboBox().buttonsProc[index],hWnd,msg,wParam,lParam);
}


