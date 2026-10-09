#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <vector>
using std::vector;
#include "KWin32App.h"
#include "KIniFile.h"
#include "layoutinterface.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatClanAnnouncementDlg.h"

#include "KWin32Wnd.h"
#include "resource.h"
#include "chatWindow/ChatMainDlg.h"
#include "GDIRender.h"
#include "SocialComDef.h"
#include "chatWindow/ChatResource.h"
#include "chatWindow/ChatClanPlayerData.h"
#include "GameDataDef.h"

ChatClanAnnouncementDlg::ChatClanAnnouncementDlg()
{
	m_hEditWnd = 0;
	m_hDlg = 0;
}

ChatClanAnnouncementDlg::~ChatClanAnnouncementDlg()
{

}

BOOL ChatClanAnnouncementDlg::CreateDlg(HWND hParent)
{
	if ((m_hDlg = CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_TIP_DLG), hParent,
		(DLGPROC)ChatClanAnnouncementDlg::AnnouncementDlgProc)) != NULL)
	{
		hParent = hParent;
		return TRUE;
	}
	return FALSE;
}

BOOL ChatClanAnnouncementDlg::CreateDlg(HWND hParent, DLGPROC func)
{
	if ((m_hDlg = CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_TIP_DLG), hParent, func)) != NULL)
	{
		hParent = hParent;
		return TRUE;
	}
	return FALSE;
}

void ChatClanAnnouncementDlg::LoadResource(HWND hParent)
{
	if (m_hDlg == NULL)
	{
		CreateDlg(hParent);
	}

	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0};
	TCHAR szValue[MAX_PATH] = {0};
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	
	iniFile.GetInteger(_DLG_RESOURCE_INI, _DLG_RESOURCE_INI_WIDTH_TXT, 0, &m_iDlgWidth);
	iniFile.GetInteger(_DLG_RESOURCE_INI, _DLG_RESOURCE_INI_HEIGHT_TXT, 0, &m_iDLgHeight);
	iniFile.GetInteger(_DLG_RESOURCE_INI, _DLG_RESOURCE_INI_EDIT_HEIGHT_TXT, 0, &m_iEditHeight);
	iniFile.GetInteger(_DLG_RESOURCE_INI, _DLG_RESOURCE_INI_DLG_BKSRCID_TXT, 0, &m_iDlgBKSrcIdx);
	iniFile.GetInteger(_DLG_RESOURCE_INI, _DLG_RESOURCE_INI_EDIT_BCSRCID_TXT, 0, &m_iEditBKSrcIdx);

	AdjustWindow();
	GetWindowRect(m_hDlg, &m_DlgPos);
	
	m_hEditWnd = CreateWindowEx(0, "edit", "", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | ES_LEFT | ES_MULTILINE | ES_WANTRETURN,
		5, 10, m_iDlgWidth - 10, m_iEditHeight, m_hDlg,	(HMENU)_DLG_EDIT_ID, KWin32App::m_hInstance, NULL);
	if (m_hEditWnd == NULL)
	{
		int err = GetLastError();
	}
	m_hBrush = CreatePatternBrush(ChatResource::GetSingle().GetResource(m_iEditBKSrcIdx) ->hBitmap);

	m_hFont = CreateFont(ChatString::ChatStringGetString().chatDefualtFontHeight, 0, 0, 0, FW_NORMAL,
		FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
		ChatString::ChatStringGetString().chatDefualtFont);
	SendMessage(m_hEditWnd, WM_SETFONT, (WPARAM)m_hFont, TRUE);

	int bntWidht = 0;
	int bntHeight = 0;
	int bntPosX = 0;
	int bntPosY = 0;
	int bntNormalSrcIdx = 0;
	int bntMouseOverSrcIdx = 0;
	int bntFontColorR = 0;
	int bntFontColorG = 0;
	int bntFontColorB = 0;
	char textInfo[MAX_PATH] = {0};

	iniFile.GetInteger(_DLG_OK_BNT_INI, "x", 0, &bntPosX);
	iniFile.GetInteger(_DLG_OK_BNT_INI, "y", 0, &bntPosY);
	iniFile.GetInteger(_DLG_OK_BNT_INI, "width", 0, &bntWidht);
	iniFile.GetInteger(_DLG_OK_BNT_INI, "height", 0, &bntHeight);
	iniFile.GetInteger(_DLG_OK_BNT_INI, "normalSrcIdx", 0, &bntNormalSrcIdx);
	iniFile.GetInteger(_DLG_OK_BNT_INI, "mouseOverSrcIdx", 0, &bntMouseOverSrcIdx);

	m_okBnt.ChatWndCreate(_DLG_OK_BNT_ID, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | BS_NOTIFY | BS_OWNERDRAW,
		m_hDlg, "", "button", bntPosX, bntPosY, bntWidht, bntHeight);
	m_okBnt.ChatWndSetResource(bntNormalSrcIdx, bntMouseOverSrcIdx, -1, -1);

	iniFile.GetString(_DLG_OK_BNT_INI, "textInfo", "", textInfo, MAX_PATH);
	if (textInfo[0] != 0)
	{
		m_okBnt.ChatWndSetText(textInfo);
		m_okBnt.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
	}
	iniFile.GetString(_DLG_OK_BNT_INI, "font", "", textInfo, MAX_PATH);
	if (textInfo[0] != 0)
	{
		TFONT tFont;
		strcpy(tFont.fontName, textInfo);
		HDC hdc = CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0] = 0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_okBnt.ChatWndSetFont(textInfo);
		else
		{
			m_okBnt.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_okBnt.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetString(_DLG_OK_BNT_INI, "fontColor", "", textInfo, MAX_PATH);
	if (textInfo[0] != 0)
	{
		sscanf(textInfo, "%d,%d,%d", &bntFontColorR, &bntFontColorG, &bntFontColorB);
		m_okBnt.ChatWndSetTextNormalColor(RGB(bntFontColorR, bntFontColorG, bntFontColorB));
	}
	m_okBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);

	iniFile.GetInteger(_DLG_CANCEL_BNT_INI, "x", 0, &bntPosX);
	iniFile.GetInteger(_DLG_CANCEL_BNT_INI, "y", 0, &bntPosY);
	iniFile.GetInteger(_DLG_CANCEL_BNT_INI, "width", 0, &bntWidht);
	iniFile.GetInteger(_DLG_CANCEL_BNT_INI, "height", 0, &bntHeight);
	iniFile.GetInteger(_DLG_CANCEL_BNT_INI, "normalSrcIdx", 0, &bntNormalSrcIdx);
	iniFile.GetInteger(_DLG_CANCEL_BNT_INI, "mouseOverSrcIdx", 0, &bntMouseOverSrcIdx);

	m_cancelBnt.ChatWndCreate(_DLG_CANCEL_BNT_ID, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | BS_NOTIFY | BS_OWNERDRAW,
		m_hDlg, "", "button", bntPosX, bntPosY, bntWidht, bntHeight);
	m_cancelBnt.ChatWndSetResource(bntNormalSrcIdx, bntMouseOverSrcIdx, -1, -1);

	iniFile.GetString(_DLG_CANCEL_BNT_INI, "textInfo", "", textInfo, MAX_PATH);
	if (textInfo[0] != NULL)
	{
		m_cancelBnt.ChatWndSetText(textInfo);
		m_cancelBnt.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
	}
	iniFile.GetString(_DLG_CANCEL_BNT_INI, "font", "", textInfo, MAX_PATH);
	if (textInfo[0] != NULL)
	{
		TFONT tFont;
		strcpy(tFont.fontName, textInfo);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0] = 0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_cancelBnt.ChatWndSetFont(textInfo);
		else
		{
			m_cancelBnt.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_cancelBnt.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	iniFile.GetString(_DLG_CANCEL_BNT_INI, "fontColor", "", textInfo, MAX_PATH);
	if (textInfo[0] != NULL)
	{
		sscanf(textInfo, "%d,%d,%d", &bntFontColorR, &bntFontColorG, &bntFontColorB);
		m_cancelBnt.ChatWndSetTextNormalColor(RGB(bntFontColorR, bntFontColorG, bntFontColorB));
	}

	m_cancelBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
}

void ChatClanAnnouncementDlg::AdjustWindow()
{
	RECT rc;
	GetWindowRect(ChatMainDlg::hMainDlg, &rc);
	MoveWindow(m_hDlg, rc.left + 50, rc.top + 100, m_iDlgWidth, m_iDLgHeight, TRUE);
}

ChatClanAnnouncementDlg & ChatClanAnnouncementDlg::GetDlg()
{
	static ChatClanAnnouncementDlg dlg;
	return dlg;
}

void ChatClanAnnouncementDlg::ShowDlg(BOOL isShow)
{
	m_blIsShow = isShow;
	if (isShow)
	{
		ShowWindow(m_hDlg, SW_SHOW);
	}
	else
	{
		ShowWindow(m_hDlg, SW_HIDE);
	}
}

void ChatClanAnnouncementDlg::DrawDlg(HDC hdc)
{
	HDC hTempDC = CreateCompatibleDC(hdc);
	if (hTempDC == NULL)
	{
		return;
	}
	HBITMAP hNewBitmap = CreateCompatibleBitmap(hdc, m_iDlgWidth, m_iDLgHeight);
	if (hNewBitmap == NULL)
	{
		return;
	}
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hTempDC, hNewBitmap);
	if (hOldBitmap == NULL || hOldBitmap == HGDI_ERROR)
	{
		return;
	}
	DrawBitmap(hTempDC, ChatResource::GetSingle().GetResource(m_iDlgBKSrcIdx)->hBitmap, m_iDlgWidth, m_iDLgHeight);
	BitBlt(hdc, 0, 0, m_iDlgWidth, m_iDLgHeight, hTempDC, 0, 0, SRCCOPY);
	SelectObject(hTempDC, hOldBitmap);
	DeleteObject(hNewBitmap);
	DeleteObject(hTempDC);
}

void ChatClanAnnouncementDlg::Release()
{
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
	}
	if (m_hBrush != NULL)
	{
		DeleteObject(m_hBrush);
	}
}

void ChatClanAnnouncementDlg::OnMouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_HOVER|TME_LEAVE;
	tme.dwHoverTime = 1000;
	tme.hwndTrack = m_hDlg;
	_TrackMouseEvent(&tme);
	if(m_cancelBnt.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEOVER)
	{
		m_cancelBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		m_cancelBnt.ChatWndUpdate();
	}
	if(m_okBnt.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEOVER)
	{
		m_okBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		m_okBnt.ChatWndUpdate();
	}
}

void ChatClanAnnouncementDlg::OnMouseLeave(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	GetCursorPos(&pt);
	POINT ptClient;
	ptClient.x = pt.x;
	ptClient.y = pt.y;
	ScreenToClient(m_cancelBnt.ChatWndGetHandle(), &ptClient);
	if(IsInRect(ptClient, m_cancelBnt.ChatWndGetRect()))
	{
		if(m_cancelBnt.ChatWndGetState() == _CHAT_BUTTON_STATE_NORMAL)
		{
			m_cancelBnt.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
			m_cancelBnt.ChatWndUpdate();
		}
		return;
	}

	ptClient.x = pt.x;
	ptClient.y = pt.y;
	ScreenToClient(m_okBnt.ChatWndGetHandle(),&ptClient);
	if(IsInRect(ptClient, m_okBnt.ChatWndGetRect()))
	{
		if(m_okBnt.ChatWndGetState() == _CHAT_BUTTON_STATE_NORMAL)
		{
			m_okBnt.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
			m_okBnt.ChatWndUpdate();
		}
		return;
	}
}

void ChatClanAnnouncementDlg::DrawButton(LPDRAWITEMSTRUCT lpdis)
{
	switch(lpdis->CtlID)
	{
	case _DLG_OK_BNT_ID:
		m_okBnt.ChatWndDrawItem(lpdis->hDC);
		return;
	case _DLG_CANCEL_BNT_ID:
		m_cancelBnt.ChatWndDrawItem(lpdis->hDC);
		return;
	}
}

void ChatClanAnnouncementDlg::OnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	int id = 0;
	id = LOWORD(wParam);
	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
		{
			if(id == _DLG_OK_BNT_ID)
			{
				char text[COMMON_CLIENT_MSG_LEN_512];
				GetWindowText(m_hEditWnd, text, COMMON_CLIENT_MSG_LEN_512);
				text[COMMON_CLIENT_MSG_LEN_512 - 1] = 0;
				ChatClanPlayerData::GetPlayerData().ModifyAnnoucement(text, m_iLayerID);
				ChatClanPlayerData::GetPlayerData().GetAnnoucement(m_iLayerID);
				ShowDlg(FALSE);
			}
			else if (id == _DLG_CANCEL_BNT_ID)
			{
				ShowDlg(FALSE);
			}
		}
		return;
	case EN_CHANGE:
		{
			if(id == _DLG_EDIT_ID)
			{
				InvalidateRect(m_hEditWnd, NULL, TRUE);
				UpdateWindow(m_hEditWnd);
			}
		}
		return;
	}
}

void ChatClanAnnouncementDlg::ClearBrush()
{
	if (m_hBrush == NULL)
	{
		return;
	}
	DeleteObject(m_hBrush);
	m_hBrush = CreatePatternBrush(ChatResource::GetSingle().GetResource(m_iEditBKSrcIdx) ->hBitmap);
}

BOOL CALLBACK ChatClanAnnouncementDlg::AnnouncementDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CTLCOLOREDIT:
		{
			switch(GetWindowLong((HWND)lParam, GWL_ID))
			{
			case _DLG_EDIT_ID:
				{
					SetBkMode((HDC)wParam, TRANSPARENT);
					SetTextColor((HDC)wParam, RGB(255, 255, 255));
					return (BOOL)ChatClanAnnouncementDlg::GetDlg().m_hBrush;
				}
				break;
			}
		}
		return FALSE;
	case WM_INITDIALOG:
		{
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_DESTROY:
		{
			ChatClanAnnouncementDlg::GetDlg().Release();
		}
		return FALSE;
	case WM_DRAWITEM:
		{
			ChatClanAnnouncementDlg::GetDlg().DrawButton((LPDRAWITEMSTRUCT)lParam);
		}
		return FALSE;
	case WM_COMMAND:
		{
			ChatClanAnnouncementDlg::GetDlg().OnCommand(hwnd, wParam, lParam);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatClanAnnouncementDlg::GetDlg().OnMouseMove(hwnd, wParam, lParam);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatClanAnnouncementDlg::GetDlg().OnMouseLeave(hwnd, wParam, lParam);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			ChatClanAnnouncementDlg::GetDlg().DrawDlg((HDC)wParam);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			ChatClanAnnouncementDlg::GetDlg().DrawDlg(hdc);
			EndPaint(hwnd, &ps);
		}
		return FALSE;
	case WM_SHOWWINDOW:
		{
			if (ChatClanAnnouncementDlg::GetDlg().m_iLayerID == enSULayer_Gens)
			{
				SetWindowText(ChatClanAnnouncementDlg::GetDlg().m_hEditWnd, ChatClanPlayerData::GetPlayerData().m_ClanAnnoucement);
			}
			else if (ChatClanAnnouncementDlg::GetDlg().m_iLayerID ==enSULayer_Tong)
			{
				SetWindowText(ChatClanAnnouncementDlg::GetDlg().m_hEditWnd, ChatClanPlayerData::GetPlayerData().m_LuedAnnoucement);
			}
		}
		return FALSE;
	}
	return FALSE;
}
