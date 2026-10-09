#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
//#include <string.h>
#include <list>
#include <string>
using namespace std;
#include "layoutinterface.h"
#include "GameDataDef.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/OnwerPlayerInfo.h"

#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/ChatControlPanel.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"

#include "ui/uicase/UiChatWindow.h"
#include "chatWindow/ChatResource.h"

PlayerInfoControl::PlayerInfoControl()
{
	state = 0;
	id = 0;
	posRect.x = 0;
	posRect.y  = 0;
	posRect.width = 0;
	posRect.height = 0;
	pSource = 0;
	enable = FALSE;
	fontLeftLineColor = 0;
	fontOnlineColor = 0;
	hParentHandle = 0;

	IsSelected = FALSE;

	nameItemWidth = 0 ;
	metierItemWidth = 0 ;
	levelItemWidth  = 0;



}
PlayerInfoControl::~PlayerInfoControl()
{

}

void PlayerInfoControl::PlayerControlDrawItem(HDC hdc,bool isUseDefaultFont)
{
//	HBITMAP hBitmap=0;
	COLORREF currentColor=0;
/*	switch(state)
	{
	case _PLAYER_CONTROL_STATE_NORMAL:
		currentColor = fontOnlineColor;
//		hBitmap = pSource->hNormalSrc;
		break;
	case _PLAYER_CONTROL_STATE_MOUSEOVER:
		currentColor = fontOnlineMouseOverColor;
//		hBitmap = pSource->hMouseOverSrc;
		break;
	case _PLAYER_CONTROL_STATE_MOUSEDOWN:
		currentColor = fontOnlineSelectColor;
//		hBitmap = pSource->hMouseDownSrc;
		break;
	case _PLAYER_CONTROL_STATE_LEFTLINE:
		currentColor = fontLeftLineColor;
//		hBitmap = pLeftSource->hNormalSrc;
		break;	
	case _PLAYER_CONTROL_STATE_LEFTLINE_SELECT:
		currentColor = fontLeftSelectColor;
//		hBitmap = pLeftSource->hMouseDownSrc;	
		break;
	}*/

	switch(state)
	{
	case _PLAYER_CONTROL_STATE_NORMAL:
		currentColor = fontOnlineColor;
		break;
	case _PLAYER_CONTROL_STATE_LEFTLINE:
		currentColor = fontLeftLineColor;
		break;
	}
	HDC hdc1 = CreateCompatibleDC(hdc);
	HBITMAP hTempBitmap = CreateCompatibleBitmap(hdc,posRect.width,posRect.height);
	HBITMAP hOldBitmap = SelectBitmap(hdc1,hTempBitmap);

	BitBlt(hdc1,0,0,posRect.width,posRect.height,hdc,posRect.x,posRect.y,SRCCOPY);

	HPEN hNewPen = NULL;
	HPEN hOldPen = NULL;
	if (IsSelected)
	{
		hNewPen = CreatePen(PS_SOLID, posRect.height + 10, fontLeftSelectColor);
		hOldPen = (HPEN)SelectObject(hdc1, (HGDIOBJ)hNewPen);
		MoveToEx(hdc1, 0, 2, NULL);
		LineTo(hdc1, posRect.width, 2);
	}

	DrawTextItem(hdc1,currentColor,0,0,*nameItemWidth,posRect.height,pszFontName,(char*)playerInfo.playerName,0,isUseDefaultFont,0);
	DrawTextItem(hdc1,currentColor,*nameItemWidth,0,*metierItemWidth,posRect.height,pszFontName,(char*)playerInfo.playerMetier,0,isUseDefaultFont,0);
	char level[16];
	if(playerInfo.playerLevel==0)
		wsprintf(level,"--");
	else
		wsprintf(level,"%d",playerInfo.playerLevel);
	DrawTextItem(hdc1,currentColor,
		        *nameItemWidth+*metierItemWidth,
				0,*levelItemWidth,posRect.height,
				pszFontName,level,0,isUseDefaultFont,0);

/*	DrawTextItem(hdc1,currentColor,
		        *nameItemWidth+*metierItemWidth+*levelItemWidth,
				0,*groupItemWidth,posRect.height,
				pszFontName,(char*)playerInfo.PlayerGroup,0,isUseDefaultFont,0);
	DrawTextItem(hdc1,currentColor,
		        *nameItemWidth+*metierItemWidth+*levelItemWidth+*groupItemWidth,
				0,*placeItemWidth,posRect.height,
				pszFontName,(char*)playerInfo.playerPlace,0,isUseDefaultFont,FALSE);*/
	BitBlt(hdc,posRect.x,posRect.y,posRect.width,posRect.height,hdc1,0,0,SRCCOPY);
	SelectBitmap(hdc1,hOldBitmap);
	SelectObject(hdc1, (HGDIOBJ)hOldPen);
	DeleteObject(hNewPen);
	DeleteObject(hTempBitmap);
	DeleteDC(hdc1);
}

void PlayerInfoControl::PlayerControlDrawItem()
{
	HBITMAP hBitmap;
	switch(state)
	{
	case _PLAYER_CONTROL_STATE_NORMAL:
		hBitmap = ChatResource::GetSingle().GetResource(pSource->normal_idx)->hBitmap;
		break;
	case _PLAYER_CONTROL_STATE_MOUSEOVER:
		hBitmap = ChatResource::GetSingle().GetResource(pSource->hover_idx)->hBitmap;
		break;
	case _PLAYER_CONTROL_STATE_MOUSEDOWN:
		hBitmap =ChatResource::GetSingle().GetResource(pSource->pushed_idx)->hBitmap;
		break;
	case _PLAYER_CONTROL_STATE_LEFTLINE:
		hBitmap = ChatResource::GetSingle().GetResource(pLeftSource->normal_idx)->hBitmap;
		break;
	}
	HDC hdc = GetWindowDC(hParentHandle);
	DrawBitmap(hdc,hBitmap,posRect.width,posRect.height,posRect.x,posRect.y);
	SetBkMode(hdc,TRANSPARENT);
//	SetTextColor(hdc,fontColor);
	TextOut(hdc,posRect.x,posRect.y,(CHAR*)playerInfo.playerName,strlen((CHAR*)playerInfo.playerName));
	ReleaseDC(hParentHandle,hdc);
}

void PlayerInfoControl::PlayerControlUpdate()
{
	RECT rc;
	rc.left = posRect.x;
	rc.top = posRect.y;
	rc.right = rc.left + posRect.width;
	rc.bottom = rc.top + posRect.height;
	InvalidateRect(hParentHandle,&rc,TRUE);
	UpdateWindow(hParentHandle);
}
void PlayerInfoControl::PlayerControlProcessLButtonBLCLK()
{
//	ChatControlPanel::ChatPanelGetPanel().currentPanel = 0;
	ChatButton* pButton = ChatControlPanel::ChatPanelGetPanel().panelButtonList[0];
	SendMessage(pButton->ChatWndGetHandle(),WM_LBUTTONDOWN,0,0);
	ILayout* pEdit = B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut;
//	pEdit->clearLayout();
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ClearText( );
	pEdit->SetText(_CHAT_EIDT_DEFAULT_STRING);
	pEdit->setSelection(0,0);
	DWORD dwNum = MultiByteToWideChar (CP_ACP, 0,(char*)playerInfo.playerName, -1, NULL, 0);
	WCHAR  *pName = new WCHAR[dwNum];
	WCHAR   name[256];
	MultiByteToWideChar( CP_ACP,0, (char*)playerInfo.playerName, -1, (unsigned short *)pName, dwNum );
	wsprintfW(name,L"%s%s%s",L"/",pName,L" ");
	delete [] pName;
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(TRUE);
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditInsertChar(name);
}
void PlayerInfoControl::PlayerControlProcessLButtonDown()
{
	if(state == _PLAYER_CONTROL_STATE_LEFTLINE)
	{
		state = _PLAYER_CONTROL_STATE_LEFTLINE_SELECT;
		IsSelected = TRUE;
	}
	else if(state == _PLAYER_CONTROL_STATE_LEFTLINE_SELECT)
	{
		state = _PLAYER_CONTROL_STATE_LEFTLINE;
		IsSelected = FALSE;
	}
	else if(state!=_PLAYER_CONTROL_STATE_MOUSEDOWN)
	{
		state = _PLAYER_CONTROL_STATE_MOUSEDOWN;
		IsSelected = TRUE;
	}
	else
	{
		state = _PLAYER_CONTROL_STATE_MOUSEOVER;
		IsSelected = FALSE;
	}
	PlayerControlUpdate();

}
void PlayerInfoControl::PlayerControlProcessMouseMove()
{
	if(state == _PLAYER_CONTROL_STATE_NORMAL)
	{
		state = _PLAYER_CONTROL_STATE_MOUSEOVER;
		PlayerControlUpdate();
	}
}

void PlayerInfoControl::PlayerControlSetArePartWidth(int *nameWidth,
													 int *metierWidth, 
													 int *levelWidth)
{
	nameItemWidth = nameWidth;
	metierItemWidth = metierWidth;
	levelItemWidth = levelWidth;
}
void PlayerInfoControl::PlayerControlSetSelected(BOOL selected,BOOL update)
{
	IsSelected = selected;
/*	if(selected)
	{
		if(playerInfo.isPlayerOnline)
			state = _PLAYER_CONTROL_STATE_MOUSEDOWN;
		else
			state = _PLAYER_CONTROL_STATE_LEFTLINE_SELECT;
	}
	else
	{
		if(playerInfo.isPlayerOnline)
		{
			state = _PLAYER_CONTROL_STATE_NORMAL;
		}
		else
			state = _PLAYER_CONTROL_STATE_LEFTLINE;
	}*/
	if (playerInfo.isPlayerOnline)
	{
		state = _PLAYER_CONTROL_STATE_NORMAL;
	}
	else
	{
		state = _PLAYER_CONTROL_STATE_LEFTLINE;
	}
	if(update)
		PlayerControlUpdate();
}
void PlayerInfoControl::PlayerControlProcessMouseLeave()
{
	if(state == _PLAYER_CONTROL_STATE_MOUSEOVER)
	{
		state = _PLAYER_CONTROL_STATE_NORMAL;
		PlayerControlUpdate();
	}
}
void PlayerInfoControl::PlayerControlCreate(int x,int y,int width,int height,int id,const char*font /* = 0 */)
{
	posRect.x = x;
	posRect.y = y;
	posRect.width = width;
	posRect.height = height;
	this->id = id;
}
void DrawTextItem(HDC hdc,COLORREF color,int x,int y,int width,int height,const char* fontName,const char* pText,COLORREF lineColor,BOOL isUseDefualFont,BOOL isDrawLine)
{
	int fontHeight = 0;
	int fontWeight = 0;
	if(isUseDefualFont)
	{
		fontHeight = ChatString::ChatStringGetString().chatDefualtFontHeight;
		fontWeight = FW_NORMAL;
	}
	else
	{
		fontHeight = height+1;
		fontWeight = FW_BOLD;
	}
	HFONT hFont = CreateFont(fontHeight,0,0,0,fontWeight,FALSE,FALSE,FALSE,ANSI_CHARSET,
		                     OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,
		                     ANTIALIASED_QUALITY,FF_ROMAN,fontName);
	int mode = 0;
	int oldColor = 0; 
	HFONT hOldFont = (HFONT)SelectObject(hdc,hFont);
	int drawX = 0;
	int drawY = 0;
	int length = strlen(pText);
	mode = SetBkMode(hdc,TRANSPARENT);
	oldColor = SetTextColor(hdc,color);
	char* showText = new char[length+1];
	memset(showText,0,length+1);
	memcpy(showText,pText,length);
	SIZE textSize ;
	GetTextExtentPoint32(hdc,showText,length,&textSize);
	RECT rc;
	rc.left = x+2;
	rc.right = x+width-2;
	if(isUseDefualFont)
		rc.top =  y+(height-textSize.cy)/2+1;
	else
		rc.top = y+(height-textSize.cy)/2;
	rc.bottom = y+(height+textSize.cy)/2;
	DrawTextEx(hdc,showText,-1,&rc,DT_CENTER|DT_VCENTER|DT_END_ELLIPSIS|DT_MODIFYSTRING,0);
	if(isDrawLine)
	{
		HPEN hPen = CreatePen(PS_SOLID,2,lineColor);
		HPEN hOldPen = (HPEN)SelectObject(hdc,hPen);
		MoveToEx(hdc,x+width,y+1,0);
		LineTo(hdc,x+width,y+height);
		SelectObject(hdc,hOldPen);
		DeleteObject(hPen);
	}
	SelectObject(hdc,hOldFont);
	DeleteObject(hFont);
	SetBkMode(hdc,mode);
	SetTextColor(hdc,oldColor);
	delete [] showText;
}