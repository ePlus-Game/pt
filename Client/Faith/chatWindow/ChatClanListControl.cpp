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
#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/ChatClanListControl.h"

ChatClanListControl::ChatClanListControl()
{
	memset(&m_playerData, 0, sizeof(TongPageData));
}

ChatClanListControl::~ChatClanListControl()
{
	
}

void ChatClanListControl::DrawItem(HWND hwnd, HDC hdc, bool isUseDefaultFont)
{
	COLORREF currentColor = 0;
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
	HPEN hNewPen = NULL;
	HPEN hOldPen = NULL;

	BitBlt(hdc1,0,0,posRect.width,posRect.height,hdc,posRect.x,posRect.y,SRCCOPY);
	if (IsSelected)
	{
		hNewPen = CreatePen(PS_SOLID, posRect.height + 10, fontLeftSelectColor);
		hOldPen = (HPEN)SelectObject(hdc1, (HGDIOBJ)hNewPen);
		MoveToEx(hdc1, 0, 2, NULL);
		LineTo(hdc1, posRect.width, 2);
	}
	DrawTextItem(hdc1, currentColor, 0, 0, *nameItemWidth, posRect.height,
		pszFontName, m_playerData.szName, 0, isUseDefaultFont, 0);

	char strMetier[16] = {0};

	DWORD dwMetire = m_playerData.nMetier;
	DWORD dwBaseMetire = (dwMetire & 0x000000000f);
	DWORD dwHiwordMetire = ((dwMetire & 0x000000f0)>>4);

	switch(dwBaseMetire)
	{
	case 0:
		{
			if (dwHiwordMetire == 0x0000000f)
			{
				sprintf(strMetier, ROLE_CAREER_JS);
			}
			else
			{
				if (dwHiwordMetire == 0)
				{
					sprintf(strMetier, ROLE_CAREER_JS_1);
				}
				else
				{
					sprintf(strMetier, ROLE_CAREER_JS_0);
				}
			}
		}
		break;
	case 1:
		{
			if (dwHiwordMetire == 0x0000000f)
			{
				sprintf(strMetier, ROLE_CAREER_DS);
			}
			else
			{
				if (dwHiwordMetire == 0)
				{
					sprintf(strMetier, ROLE_CAREER_DS_1);
				}
				else
				{
					sprintf(strMetier, ROLE_CAREER_DS_0);
				}
			}
		}
		break;
	case 2:
		{
			if (dwHiwordMetire == 0x0000000f)
			{
				sprintf(strMetier, ROLE_CAREER_YR);
			}
			else
			{
				if (dwHiwordMetire == 0)
				{
					sprintf(strMetier, ROLE_CAREER_YR_0);
				}
				else
				{
					sprintf(strMetier, ROLE_CAREER_YR_1);
				}
			}
		}
		break;
	default:
		{
			scanf(strMetier, "----");
		}
		break;
	}
	DrawTextItem(hdc1,currentColor,*nameItemWidth,0,*metierItemWidth,posRect.height,
		pszFontName,strMetier,0,isUseDefaultFont,0);
	char level[16];
	if(m_playerData.nLevel == 0)
		wsprintf(level,"--");
	else
		wsprintf(level, "%d", m_playerData.nLevel);
	DrawTextItem(hdc1, currentColor, *nameItemWidth + *metierItemWidth,	0, *levelItemWidth, posRect.height,
		pszFontName, level, 0, isUseDefaultFont, 0);

	BitBlt(hdc,posRect.x,posRect.y,posRect.width,posRect.height,hdc1,0,0,SRCCOPY);
	SelectBitmap(hdc1, hOldBitmap);
	SelectObject(hdc1, (HGDIOBJ)hOldPen);
	DeleteObject(hNewPen);
	DeleteObject(hTempBitmap);
	DeleteDC(hdc1);
}

void ChatClanListControl::SetSelected(BOOL selected, BOOL update)
{
	IsSelected = selected;
	if(m_playerData.bOnline)
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

TongPageData & ChatClanListControl::GetPlayerData()
{
	return m_playerData;
}

void ChatClanListControl::OnLButtonDBLCLK()
{
	ChatButton* pButton = ChatControlPanel::ChatPanelGetPanel().panelButtonList[0];
	SendMessage(pButton->ChatWndGetHandle(),WM_LBUTTONDOWN,0,0);
	ILayout* pEdit = B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut;
//	pEdit->clearLayout();
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ClearText( );
	pEdit->SetText(_CHAT_EIDT_DEFAULT_STRING);
	pEdit->setSelection(0,0);
	DWORD dwNum = MultiByteToWideChar (CP_ACP, 0,(char*)m_playerData.szName, -1, NULL, 0);
	WCHAR  *pName = new WCHAR[dwNum];
	WCHAR   name[256];
	MultiByteToWideChar( CP_ACP,0, (char*)m_playerData.szName, -1, (unsigned short *)pName, dwNum );
	wsprintfW(name,L"%s%s%s",L"/",pName,L" ");
	delete [] pName;
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(TRUE);
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditInsertChar(name);
}