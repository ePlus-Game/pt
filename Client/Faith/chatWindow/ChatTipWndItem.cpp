#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <list>
#include <vector>
using namespace std;
#include "layoutinterface.h"
#include "GameDataDef.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"

#include "loadSrcWnd/GDILoadBitmap.h"

#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/ChatWndProc.h"
#include "chatWindow/ChatResource.h"



ChatTipWndItem::ChatTipWndItem()
{
	pTipItemLay = 0;
}
ChatTipWndItem::~ChatTipWndItem()
{

	if(pTipItemLay)
	{
		pTipItemLay->clearLayout();
		pTipItemLay->Release();
		delete pTipItemLay;
	}
}
void ChatTipWndItem::ChatTipWndShow(BOOL bShow)
{
	isShow = bShow;
	if(bShow)
	{
		if(!IsWindowVisible(hTipItemWnd))
			ShowWindow(hTipItemWnd,SW_NORMAL);
	}
	else
	{
		if(IsWindowVisible(hTipItemWnd))
			ShowWindow(hTipItemWnd,SW_HIDE);
	}
}
BOOL CALLBACK ChatTipWndItem::ChatTipWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndCreate(hwnd);
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return 0;
	case WM_MOUSEMOVE:
		{
		//	if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->closeButton.ChatWndGetState()!=_CHAT_BUTTON_STATE_NORMAL)
			{
		//		B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->closeButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		//		B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->closeButton.ChatWndUpdate();
			}
			return FALSE;
		}
//	case WM_DRAWITEM:
//		{
//			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndDrawItem((LPDRAWITEMSTRUCT)lParam);
//		}
		return 0;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return 0;

	case WM_ERASEBKGND:
		{
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndProcessPaint((HDC)wParam);
		}
		return TRUE;
	case WM_COMMAND:
		{
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndClickButton(hwnd,wParam,lParam);
		}
		return 0;
	case WM_DESTROY:
		{
//			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->closeButton.ChatWndDeleteSrc();

//			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->pTipItemLay->Release();
		}
	}
	return FALSE;
}
void ChatTipWndItem::ChatTipWndAdjustPos()
{
	HWND hWnd = GetDesktopWindow();
	RECT rcDesk;
	GetClientRect(hWnd,&rcDesk);
	RECT  rcMainWnd;
	GetWindowRect(ChatMainDlg::hMainDlg,&rcMainWnd);
	int cx=0,cy=0;
	cx = rcMainWnd.left - width;
	if(cx<0)
		cx = 0;
	cy = y;
	if(cy+height>rcDesk.bottom-50)
		cy = rcDesk.bottom-height-50;
	RECT rc1;
	
	::MoveWindow(hTipItemWnd,cx,cy,width,height,TRUE);
	::MoveWindow(hShowItem,leftDist,topDist,width-leftDist-rightDist,height-topDist-endDist,TRUE);
	GetClientRect(hShowItem,&rc1);
	closeButton.ChatWndMove(rc1.right-closeButton.ChatWndGetRect().right,0,closeButton.ChatWndGetRect().right,closeButton.ChatWndGetRect().bottom,TRUE);
}
void ChatTipWndItem::ChatTipWndClickButton(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case CHAT_TIP_ITEM_BUTTON_ID:
		{
			if(HIWORD(wParam) == BN_CLICKED)
			{
				ChatTipWndShow(FALSE);
			}
			
		}
		return;
	}
}
void ChatTipWndItem::ChatTipWndDrawTip(HDC hdc)
{
	ChatResource& resource = ChatResource::GetSingle();
	RECT rc;
	GetClientRect(hShowItem,&rc);
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc,rc.right,rc.bottom);
	HBITMAP hOldBitmap = SelectBitmap(hdcBuffer,hBitmap);

	DrawBitmap(hdcBuffer,resource.GetResource(wndBitmapIdx)->hBitmap,width-leftDist-rightDist,height-topDist-endDist);
	if(pTipItemLay->isHaveContent())
	{
		((GDIRender*)B2ChatDialog::pItemTipRender)->hCurrentDC = hdcBuffer;
		LORect rect;
		rect.setPos(0,0);
		rect.setHeight(height-topDist-endDist);
		rect.setWidth(width-leftDist-rightDist);
		pTipItemLay->setClipper(rect);
		pTipItemLay->Render(0,0,0);
	}
	BitBlt(hdc,0,0,rc.right,rc.bottom,hdcBuffer,0,0,SRCCOPY);
	SelectBitmap(hdcBuffer,hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hdcBuffer);
}
void ChatTipWndItem::ChatTipWndDrawItem(LPDRAWITEMSTRUCT lpdis)
{
	return;
	ChatResource& resource = ChatResource::GetSingle();
	switch(lpdis->CtlID)
	{
	case CHAT_TIP_ITEM_SHOW_ID:
		{
			DrawBitmap(lpdis->hDC,resource.GetResource(wndBitmapIdx)->hBitmap,width-leftDist-rightDist,height-topDist-endDist);
			if(pTipItemLay->isHaveContent())
			{
				((GDIRender*)B2ChatDialog::pItemTipRender)->hCurrentDC = lpdis->hDC;
				LORect rect;
				rect.setPos(0,0);
				rect.setHeight(height-topDist-endDist);
				rect.setWidth(width-leftDist-rightDist);
				pTipItemLay->setClipper(rect);
				RECT rc;
				GetClientRect(lpdis->hwndItem,&rc);
				pTipItemLay->Render(0,0,0);

			}
		}
		return ;
	case CHAT_TIP_ITEM_BUTTON_ID:
		{
//			closeButton.ChatWndDrawItem(lpdis->hDC);
		}
		return;
	}
}
void ChatTipWndItem::ChatTipWndCreate(HWND hwnd)
{
#define TIP_WND_ITEM_NAME   "tipWndItem"
#define TIP_WND_ITEM_NAME_BUTTON "tipWndItemButton"
	KIniFile iniFile;
	B2ChatDialog::m_hDialog = hwnd;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	hShowItem = CreateWindowEx(0,"static" ,"",WS_CHILD|WS_VISIBLE|SS_OWNERDRAW|WS_CLIPCHILDREN|SS_NOTIFY ,0,0,0,0,hwnd,(HMENU)CHAT_TIP_ITEM_SHOW_ID,KWin32App::m_hInstance,0);
	iniFile.GetInteger("tipWndItem","leftDist",0,&leftDist);
	iniFile.GetInteger("tipWndItem","rightDist",0,&rightDist);
	iniFile.GetInteger("tipWndItem","topDist",0,&topDist);
	iniFile.GetInteger("tipWndItem","endDist",0,&endDist);
	iniFile.GetInteger(TIP_WND_ITEM_NAME,"leftBitmapIdx",0,&leftBitmapIdx);

	iniFile.GetInteger(TIP_WND_ITEM_NAME,"rightBitmapIdx",0,&rightBitmapIdx);
	iniFile.GetInteger(TIP_WND_ITEM_NAME,"topBitmapIdx",0,&topBitmapIdx);
	iniFile.GetInteger(TIP_WND_ITEM_NAME,"endBitmapIdx",0,&endBitmapIdx);
	iniFile.GetInteger(TIP_WND_ITEM_NAME,"leftTopBitmapIdx",0,&leftTopBitmapIdx);

	iniFile.GetInteger(TIP_WND_ITEM_NAME,"rightTopBitmapIdx",0,&rightTopBitmapIdx);
	iniFile.GetInteger(TIP_WND_ITEM_NAME,"leftEndBitmapIdx",0,&leftEndBitmapIdx);
	iniFile.GetInteger(TIP_WND_ITEM_NAME,"rightEndBitmapIdx",0,&rightEndBItmapIdx);

	iniFile.GetInteger(TIP_WND_ITEM_NAME,"WndSrcIdx",0,&wndBitmapIdx);


	int width ;
	int height ;
	iniFile.GetInteger(TIP_WND_ITEM_NAME_BUTTON,_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger(TIP_WND_ITEM_NAME_BUTTON,_CHAT_SRC_HEIGHT,0,&height);
	
	closeButton.ChatWndCreate(CHAT_TIP_ITEM_BUTTON_ID,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,hShowItem,"","button",0,0,width,height);
	int normal_idx = 0,hover_idx = 0;
	iniFile.GetInteger(TIP_WND_ITEM_NAME_BUTTON,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(TIP_WND_ITEM_NAME_BUTTON,"mouseOverSrcIdx",0,&hover_idx);
	closeButton.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	closeButton.SetWndProcessFun(ChatWndProcessFun::ProcessTipItemCloseButton);

	CreateLayout(&pTipItemLay,B2ChatDialog::pItemTipRender);
	iniFile.GetInteger(TIP_WND_ITEM_NAME,"fontHeight",0,&((GDIRender*)B2ChatDialog::pItemTipRender)->fontHeight);

	char font[FONT_SIZE]={0};
	iniFile.GetString(TIP_WND_ITEM_NAME,"font","",font,FONT_SIZE);
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
		strcpy(((GDIRender*)B2ChatDialog::pItemTipRender)->font,font);
		((GDIRender*)B2ChatDialog::pItemTipRender)->isUseDefualtFont = false;
	}
	else
	{
		strcpy(((GDIRender*)B2ChatDialog::pItemTipRender)->font,ChatString::ChatStringGetString().chatDefualtFont);
		((GDIRender*)B2ChatDialog::pItemTipRender)->isUseDefualtFont= true;
	}

	tipShowProc = (WNDPROC)SetWindowLong(hShowItem,GWL_WNDPROC,(LONG)(ChatWndProcessFun::ProcessTipShowControl));
}

LRESULT ChatTipWndItem::ChatTipColosButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{

	switch(msg)
	{
	case WM_MOUSEMOVE:
//		if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->closeButton.ChatWndGetState()!=_CHAT_BUTTON_STATE_MOUSEOVER)
		{
	//		B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->closeButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
	//		B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->closeButton.ChatWndUpdate();
		}
		return FALSE;

	}
	return CallWindowProc(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->closeButtonProc,hwnd,msg,wParam,lParam);
}
void ChatTipWndItem::ChatTipWndProcessPaint(HDC hdc)
{
	ChatResource&resource = ChatResource::GetSingle();
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hBitmapBuffer = CreateCompatibleBitmap(hdc,width,height);
	HBITMAP hOld = (HBITMAP)SelectObject(hdcBuffer,hBitmapBuffer);
	DrawBitmap(hdcBuffer,resource.GetResource(leftBitmapIdx)->hBitmap,leftDist,height-topDist-endDist,0,topDist);
	DrawBitmap(hdcBuffer,resource.GetResource(rightBitmapIdx)->hBitmap,rightDist,height-topDist-endDist,width-rightDist,topDist);
	DrawBitmap(hdcBuffer,resource.GetResource(topBitmapIdx)->hBitmap,width-rightDist-leftDist,topDist,leftDist,0);
	DrawBitmap(hdcBuffer,resource.GetResource(endBitmapIdx)->hBitmap,width-rightDist-leftDist,endDist,leftDist,height-endDist);
	DrawBitmap(hdcBuffer,resource.GetResource(leftTopBitmapIdx)->hBitmap,leftDist,topDist,0,0);
	DrawBitmap(hdcBuffer,resource.GetResource(leftEndBitmapIdx)->hBitmap,leftDist,endDist,0,height-endDist);
	DrawBitmap(hdcBuffer,resource.GetResource(rightTopBitmapIdx)->hBitmap,rightDist,topDist,width-rightDist,0);
	DrawBitmap(hdcBuffer,resource.GetResource(rightEndBItmapIdx)->hBitmap,rightDist,endDist,width-rightDist,height-endDist);
	DrawBitmap(hdcBuffer,resource.GetResource(wndBitmapIdx)->hBitmap,width-leftDist-rightDist,height-topDist-endDist,leftDist,topDist);
	BitBlt(hdc,0,0,width,height,hdcBuffer,0,0,SRCCOPY);
	SelectObject(hdcBuffer,hOld);
	DeleteObject(hBitmapBuffer);
	DeleteDC(hdcBuffer);

}