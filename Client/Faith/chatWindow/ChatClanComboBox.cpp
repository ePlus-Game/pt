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
#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/ChatClanComboBox.h"

#include "resource.h"
#include "layoutinterface.h"
#include "GameDataDef.h"

#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"

#include "chatWindow/ChatResource.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatClanPlayerData.h"
#include "SocialComDef.h"

ChatClanComboBox::ChatClanComboBox()
: m_hDownDialg(0)
, m_hParent(0)
, m_isShowDownDlg(FALSE)
, m_iDlgSourceIdx(0)
, m_iDlgSourceWidth(0)
, m_iDlgSourceHeigth(0)
, m_iClanNum(0)
, m_iInterval(0)
, m_iInfoHeight(0)
, m_isUseDefaultFont(FALSE)
{
	m_font[0] = 0;
}

ChatClanComboBox::~ChatClanComboBox()
{

}

void ChatClanComboBox::LoadIniFile(HWND hParent)
{
	KIniFile iniFile;
	CHAR  szPath[MAX_PATH] = { 0},szValue[MAX_PATH]= {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	int x = 0;
	int y = 0;
	int bntWidth = 0;
	int bntHeight = 0;
	int r = 0;
	int g = 0;
	int b = 0;

	iniFile.GetInteger("clan_combo_box", "downBntX", 0, &x);
	iniFile.GetInteger("clan_combo_box", "downBntY", 0, &y);
	iniFile.GetInteger("clan_combo_box", "downWidth", 0, &bntWidth);
	iniFile.GetInteger("clan_combo_box", "bntHeight", 0, &bntHeight);
	m_DownBnt.ChatWndCreate(_CLAN_DOWN_BNT_ID,
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
		hParent, "", "button", x, y, bntWidth, bntHeight);

	int bntNormalSrcIdx = -1;
	int bntMouseDownSrcIdx = -1;
	iniFile.GetInteger("clan_combo_box", "bntNormalSrcIdx", -1, &bntNormalSrcIdx);
	iniFile.GetInteger("clan_combo_box", "bntMouseDownSrcIdx", -1, &bntMouseDownSrcIdx);
	m_DownBnt.ChatWndSetResource(bntNormalSrcIdx, bntMouseDownSrcIdx, bntMouseDownSrcIdx, -1);
	m_DownBnt.SetWndProcessFun(ChatClanComboBox::DownDlgButtonProc);

	iniFile.GetInteger("clan_combo_box", "selectedBntX", 0, &x);
	iniFile.GetInteger("clan_combo_box", "selectedBntY", 0, &y);
	iniFile.GetInteger("clan_combo_box", "selectedWidth", 0, &bntWidth);
	iniFile.GetInteger("clan_combo_box", "bntHeight", 0, &bntHeight);
	m_SelectedBnt.ChatWndCreate(_CLAN_SELECTED_BNT_ID,
		WS_CHILD | BS_OWNERDRAW | WS_VISIBLE,
		hParent, "", "button", x, y, bntWidth, bntHeight);

	iniFile.GetInteger("clan_combo_box", "selBntNormanSrcIdx", 0, &bntNormalSrcIdx);
	m_SelectedBnt.ChatWndSetResource(bntNormalSrcIdx, -1, -1, -1);
	
	iniFile.GetString("clan_combo_box", "fontColor", "", szValue, 256);
	sscanf(szValue, "%d,%d,%d", &r, &g, &b);
	m_SelectedBnt.ChatWndSetTextNormalColor(RGB(r, g, b));

	m_fontColor = RGB(r, g, b);

	iniFile.GetString("clan_combo_box", "font", "", m_font, 256);
	m_SelectedBnt.ChatWndSetText("");
	m_SelectedBnt.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
	if (m_font[0] != 0)
	{	
		TFONT tFont;
		strcpy(tFont.fontName,m_font);
		HDC hdc = CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0] = 0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc, &logFont, (FONTENUMPROC)EnumFontProc, (LPARAM)&tFont, 0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
		{
			m_SelectedBnt.ChatWndSetFont(m_font);
			m_isUseDefaultFont = FALSE;
		}
		else
		{
			m_SelectedBnt.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_SelectedBnt.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			if (strlen(ChatString::ChatStringGetString().chatDefualtFont) < _MAX_FONT_SIZE)
			{
				memset(m_font, 0, _MAX_FONT_SIZE);
				strncpy(m_font, ChatString::ChatStringGetString().chatDefualtFont, strlen(ChatString::ChatStringGetString().chatDefualtFont));
			}
			else
			{
				memset(m_font, 0, _MAX_FONT_SIZE);
				strncpy(m_font, ChatString::ChatStringGetString().chatDefualtFont, _MAX_FONT_SIZE - 1);
				m_font[_MAX_FONT_SIZE - 1] = 0;
			}
			m_isUseDefaultFont = TRUE;
		}
	}
	m_SelectedBnt.SetWndProcessFun(ChatClanComboBox::SelectedButtonProc);
	m_SelectedBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
	m_SelectedBnt.ChatWndUpdate();

	m_hDownDialg = CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_LOOK_FRIEND_INFO_POP),
		hParent, ChatClanComboBox::DownDlgProc);
	m_hParent = hParent;

	iniFile.GetInteger("clan_combo_box", "selectedBntX", 0, &m_posRect.x);
	iniFile.GetInteger("clan_combo_box", "selectedBntY", 0, &m_posRect.y);
	iniFile.GetInteger("clan_combo_box", "dlgWidth", 0, &m_posRect.width);
	iniFile.GetInteger("clan_combo_box", "dlgHeight", 0, &m_posRect.height);
	iniFile.GetInteger("clan_combo_box", "interval", 0, &m_iInterval);
	iniFile.GetInteger("clan_combo_box", "infoHeight", 0, &m_iInfoHeight);

	iniFile.GetInteger("clan_resource", "width", 0, &m_iDlgSourceWidth);
	iniFile.GetInteger("clan_resource", "height", 0, &m_iDlgSourceHeigth);
	iniFile.GetInteger("clan_resource", "drawAreaBitmapIdx", 0, &m_iDlgSourceIdx);

	iniFile.GetString("clan_combo_box", "mouseOverColor", "", szValue, 256);
	sscanf(szValue, "%d,%d,%d", &r, &g, &b);
	m_mouseOverColor = RGB(r, g, b);
	
	m_iDrawPosX = 0;
	m_iDrawPosY = 2;
	
	AdjustDownDlg();
	ShowDownDialog(FALSE);

//	m_ScrollBar.ChatScrollBarLoadIniCtg();
}

void ChatClanComboBox::AdjustDownDlg()
{
	RECT rc;
	GetWindowRect(m_SelectedBnt.ChatWndGetHandle(), &rc);
	MoveWindow(m_hDownDialg, rc.left, rc.top - m_posRect.height, m_posRect.width, m_posRect.height, TRUE);
}

void ChatClanComboBox::ShowDownDialog(bool show)
{
	m_isShowDownDlg = show;
	if(show)
	{
		AdjustDownDlg();
		ShowWindow(m_hDownDialg,SW_SHOW);
	}
	else
	{
		ShowWindow(m_hDownDialg,SW_HIDE);
	}
}

ChatClanComboBox & ChatClanComboBox::GetClanComboBox()
{
	static ChatClanComboBox comboBox;
	return comboBox;
}

void ChatClanComboBox::DrawItem(HDC hdc)
{
	HDC hTempDC = CreateCompatibleDC(hdc);
	if (hTempDC == NULL)
	{
		return;
	}
	HBITMAP hNewBitmap = CreateCompatibleBitmap(hdc, m_iDlgSourceWidth, m_iDlgSourceHeigth);
	if (hNewBitmap == NULL)
	{
		return;
	}
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hTempDC, hNewBitmap);
	if (hOldBitmap == NULL || hOldBitmap == HGDI_ERROR)
	{
		return;
	}

	DrawBitmap(hTempDC, ChatResource::GetSingle().GetResource(m_iDlgSourceIdx)->hBitmap,
		m_iDlgSourceWidth, m_iDlgSourceHeigth,
		0, 0);

	HPEN hNewPen = NULL;
	HPEN hOldPen = NULL;

	for (int i = 0; i < m_iClanNum; i++)
	{
		m_ClanList[i] ->SetRect(m_iDrawPosX, m_iDrawPosY + (m_iInfoHeight + m_iInterval) * i,
			m_iDlgSourceWidth - 2, m_iInfoHeight);

/*		if (m_ClanList[i] ->m_blIsMouseHover)
		{
			hNewPen = CreatePen(PS_SOLID, m_ClanList[i] ->GetHeight() + 10, m_mouseOverColor);
			hOldPen = (HPEN)SelectObject(hTempDC, (HGDIOBJ)hNewPen);
			MoveToEx(hTempDC, m_ClanList[i] ->m_posRect.left, m_ClanList[i] ->m_posRect.top, NULL);
			LineTo(hTempDC, m_ClanList[i] ->m_posRect.right, m_ClanList[i] ->m_posRect.top);
		}*/

		DrawTextItem(hTempDC, m_fontColor, m_ClanList[i] ->m_posRect.left, m_ClanList[i] ->m_posRect.top,
			m_ClanList[i] ->GetWidth(), m_ClanList[i] ->GetHeight(),
			m_font, m_ClanList[i] ->m_ClanName, m_fontColor, m_isUseDefaultFont, FALSE);
	}

	BitBlt(hdc, 0, 0, m_iDlgSourceWidth, m_iDlgSourceHeigth, hTempDC, 0, 0, SRCCOPY);
	SelectObject(hTempDC, (HGDIOBJ)hOldPen);
	SelectObject(hTempDC, (HGDIOBJ)hOldBitmap);
	DeleteObject(hNewPen);
	DeleteObject(hNewBitmap);
	DeleteObject(hTempDC);
}

void ChatClanComboBox::CLKOnDownDlg(POINT & point)
{
	for (int i = 0; i < m_iClanNum; i++)
	{
		if (IsInRect(point, m_ClanList[i] ->m_posRect))
		{
			m_ClanList[i] ->m_blIsSelected = TRUE;
			ChatClanPlayerData::GetPlayerData().RequestDataList(m_hParent, enSULayer_Gens, &m_ClanList[i] ->m_ClanGuid);
		}
		else
		{
			m_ClanList[i] ->m_blIsSelected = FALSE;
		}
	}
	ShowDownDialog(FALSE);
	UpdateComboBox();
}

void ChatClanComboBox::UpdateComboBox()
{
	for (int i = 0; i < m_iClanNum; i++)
	{
		if (m_ClanList[i] ->m_blIsSelected)
		{
			m_SelectedBnt.ChatWndSetText(m_ClanList[i] ->m_ClanName);
			if (m_isUseDefaultFont)
			{
				m_SelectedBnt.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
			m_SelectedBnt.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
			m_SelectedBnt.ChatWndUpdate();
			break;
		}
	}
	m_DownBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
	m_DownBnt.ChatWndUpdate();
}

void ChatClanComboBox::ClearAll()
{
	for (int i = 0; i < m_iClanNum; i++)
	{
		delete[] m_ClanList[i];
		m_ClanList[i] = NULL;
	}
	m_iClanNum = 0;

	m_SelectedBnt.ChatWndSetText("");
	if (m_isUseDefaultFont)
	{
		m_SelectedBnt.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
	}
	m_SelectedBnt.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
	m_SelectedBnt.ChatWndUpdate();
}

void ChatClanComboBox::UpdateDataList()
{
	TongPageData * pDataList = NULL;
	pDataList = ChatClanPlayerData::GetPlayerData().m_DataList;
	for (int i = 0; i < ChatClanPlayerData::GetPlayerData().m_iPlayerNum; i++)
	{
		m_ClanList[m_iClanNum] = new ChatClanInfo;
		strcpy(m_ClanList[i] ->m_ClanName, pDataList[i].szName);
		memcpy(&m_ClanList[i] ->m_ClanGuid, &pDataList[i].guid, sizeof(FSGUID));
		m_iClanNum++;
	}
	if (m_ClanList[0] != NULL)
	{
		m_ClanList[0] ->m_blIsSelected = TRUE;
		m_SelectedBnt.ChatWndSetText(m_ClanList[0] ->m_ClanName);
		if (m_isUseDefaultFont)
		{
			m_SelectedBnt.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
		m_SelectedBnt.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		m_SelectedBnt.ChatWndUpdate();
		if (ChatClanPlayerData::GetPlayerData().InitInterface())
		{
			ChatClanPlayerData::GetPlayerData().RequestDataList(m_hParent, enSULayer_Gens, &m_ClanList[0] ->m_ClanGuid);
		}
	}
	UpdateComboBox();
}

LPCHATCLANINFO ChatClanComboBox::GetSelectedItem()
{
	for (int i = 0; i <m_iClanNum; i++)
	{
		if (m_ClanList[i] ->m_blIsSelected)
		{
			return m_ClanList[i];
		}
	}
	return NULL;
}

BOOL CALLBACK ChatClanComboBox::DownDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			if (hdc != NULL)
			{
				ChatClanComboBox::GetClanComboBox().DrawItem(hdc);
			}
			EndPaint(hwnd, &ps);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			POINT point;
			point.x = GET_X_LPARAM(lParam);
			point.y = GET_Y_LPARAM(lParam);
			ChatClanComboBox::GetClanComboBox().CLKOnDownDlg(point);
		}
		return FALSE;
	case WM_CLAN_DATA_REQUEST_SUCCEED:
		{
			ChatClanComboBox::GetClanComboBox().ClearAll();
			ChatClanComboBox::GetClanComboBox().UpdateDataList();
		}
		return FALSE;
	case WM_DESTROY:
		{
			ChatClanComboBox::GetClanComboBox().ClearAll();
		}
	}
	return FALSE;
}

LRESULT ChatClanComboBox::DownDlgButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton & button = ChatClanComboBox::GetClanComboBox().m_DownBnt;
	switch(msg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);

			int state = button.ChatWndGetState();
			if(state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(FALSE);
			}
			int state = button.ChatWndGetState();
			if(state  == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if (state == _CHAT_BUTTON_STATE_MOUSEDOWN)
			{
				return FALSE;
			}
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			int state = button.ChatWndGetState();
			if (state == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			if (IsWindowVisible(ChatClanComboBox::GetClanComboBox().m_hDownDialg))
			{
				ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			}
			else
			{
				ChatClanComboBox::GetClanComboBox().ShowDownDialog(TRUE);
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
			}
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(), hwnd, msg, wParam, lParam);
}

LRESULT ChatClanComboBox::SelectedButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanComboBox::GetClanComboBox().m_SelectedBnt;
	switch (msg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(), hwnd, msg, wParam, lParam);
}
