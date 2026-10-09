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
#include "chatWindow/ChatClanTitleControl.h"
#include "UiMDLInterface.h"
#include "Ui/UiMDLDataset.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatClanInfoDlg.h"

#include "chatWindow/ChatWndProc.h"
#include "chatWindow/ChatResource.h"
#include "chatWindow/ChatClanPlayerData.h"

#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatClanPanel.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "chatWindow/ChatClanManager.h"
#include "resource.h"
#include "chatWindow/ChatClanComboBox.h"
#include "SocialComDef.h"

ChatClanInfoDlg::ChatClanInfoDlg()
{
	m_iOnlineMemberNum = 0;
	m_iLeftMemberNum = 0;
}

ChatClanInfoDlg::~ChatClanInfoDlg()
{

}

void ChatClanInfoDlg::CreateClanInfoDlg(HWND hParent, DlgProcessFun pFun)
{
	if((hDlg =CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_LIST_DLG),
		hParent,
		(DLGPROC)pFun)) == 0)
	{
		DWORD err = GetLastError();
		return ;
	}
	PlayerInfoDlgShowDlg(FALSE);
}

void ChatClanInfoDlg::LoadSRC(HWND hwnd)
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

	//创建button
	int button_Pos_X = 0;
	int button_Pos_Y = 0;
	int button_Width = 0;
	int button_Height = 0;

	int normal_idx = 0;
	int hover_idx = 0;

	//读取资源
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_X,0,&button_Pos_X);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_Y,0,&button_Pos_Y);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_WIDTH,0,&button_Width);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_HEIGHT,0,&button_Height);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,"mouseOverSrcIdx",0,&hover_idx);
	//创建button
	if (!m_wndHideShowBnt.ChatWndCreate(_HIDE_SHOW_BNT_ID, WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
		hwnd, "", "button", button_Pos_X, button_Pos_Y, button_Width, button_Height))
	{
		DWORD err = GetLastError();
	}
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
	m_wndHideShowBnt.SetWndProcessFun(ChatWndProcessFun::ProcessClanHideShowButton);
	m_wndHideShowBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
	m_wndHideShowBnt.ChatWndUpdate();

	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_X,0,&button_Pos_X);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_Y,0,&button_Pos_Y);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_WIDTH,0,&button_Width);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_HEIGHT,0,&button_Height);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,"mouseOverSrcIdx",0,&hover_idx);

	if (!m_wndChangeBnt.ChatWndCreate(_CHANGE_WND_ID, WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
		hwnd, "", "button", button_Pos_X, button_Pos_Y, button_Width, button_Height))
	{
		DWORD err = GetLastError();
	}
	

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

	m_wndChangeBnt.SetWndProcessFun(ChatWndProcessFun::ProcessClanChangeButton);
	m_wndChangeBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
	m_wndChangeBnt.ChatWndUpdate();

	m_titleControl.TitleControlInit();
	m_titleControl.TitleControlSetParentHandle(hwnd);
	scroll_bar.ChatScrollBarLoadIniCtg();
	scroll_bar.ChatScrollBarSetParent(hwnd);

	char *itemName[] = {_CHAT_CLAN_INFO_DLG_INI_MODIFY_BULLETIN, _CHAT_CLAN_INFO_DLG_INI_ADD_MEMBER, _CHAT_CLAN_INFO_DLG_INI_DELETE_MEMBER};
	int itemID[] = {_CHAT_CLAN_INFO_DLG_INI_MODIFY_BULLETIN_ID, _CHAT_CLAN_INFO_DLG_INI_ADD_MEMBER_ID, _CHAT_CLAN_INFO_DLG_INI_DELETE_MEMBER_ID};
	for (int i = 0; i < _CHAT_CLAN_INFO_DLG_BUTTON_NUM; i++)
	{
		iniFile.GetInteger(itemName[i], "x", 0, &button_Pos_X);
		iniFile.GetInteger(itemName[i], "y", 0, &button_Pos_Y);
		iniFile.GetInteger(itemName[i], "height", 0, &button_Height);
		iniFile.GetInteger(itemName[i], "width", 0, &button_Width);
		iniFile.GetInteger(itemName[i], "normalSrcIdx", 0, &normal_idx);
		iniFile.GetInteger(itemName[i], "mouseOverSrcIdx", 0, &hover_idx);

		if (!m_ButtenList[i].ChatWndCreate(itemID[i], WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
			hwnd, "", "button", button_Pos_X, button_Pos_Y, button_Width, button_Height))
		{
			DWORD err = GetLastError();
		}
		m_ButtenList[i].ChatWndSetResource(normal_idx, hover_idx, -1, normal_idx);
		memset(textInfo, 0, 256);
		iniFile.GetString(itemName[i], "textInfo", "", textInfo, 256);
		if (textInfo[0] != 0)
		{
			m_ButtenList[i].ChatWndSetText(textInfo);
			m_ButtenList[i].ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
			iniFile.GetString(itemName[i], "normalColor", "", szValue, MAX_PATH);

			int fontColor_r = 0;
			int fontColor_g = 0;
			int fontColor_b = 0;
			sscanf(szValue, "%d,%d,%d", &fontColor_r, &fontColor_g, &fontColor_b);
			m_ButtenList[i].ChatWndSetTextNormalColor(RGB(fontColor_r, fontColor_g, fontColor_b));
			m_ButtenList[i].m_EnableColor = RGB(fontColor_r, fontColor_g, fontColor_b);

			iniFile.GetString(itemName[i], "disableColor", "", szValue, MAX_PATH);
			sscanf(szValue, "%d,%d,%d", &fontColor_r, &fontColor_g, &fontColor_b);
			m_ButtenList[i].m_DisableColor = RGB(fontColor_r, fontColor_g, fontColor_b);

			char font[FONT_SIZE] = {0};
			iniFile.GetString(itemName[i], "font", "", font, FONT_SIZE);
			TFONT tFont;
			strcpy(tFont.fontName, font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0] = 0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc, &logFont, (FONTENUMPROC)EnumFontProc, (LPARAM)&tFont, 0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
			{
				m_ButtenList[i].ChatWndSetFont(font);
			}
			else
			{
				m_ButtenList[i].ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				m_ButtenList[i].ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
		m_ButtenList[i].ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
	}

	m_ButtenList[_CHAT_CLAN_INFO_DLG_MODIFY_BULLETIN].SetWndProcessFun(ChatWndProcessFun::ProcessClanModifyBulletinButton);
	m_ButtenList[_CHAT_CLAN_INFO_DLG_ADD_MEMBER].SetWndProcessFun(ChatWndProcessFun::ProcessClanAddMemberButton);
	m_ButtenList[_CHAT_CLAN_INFO_DLG_DELETE_MEMBER].SetWndProcessFun(ChatWndProcessFun::ProcessClanDeleteMemberButton);
}

void ChatClanInfoDlg::PaintDLG(HDC hdc)
{
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hTemp = CreateCompatibleBitmap(hdc,width,height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcBuffer,hTemp);
	DrawBitmap(hdcBuffer,ChatResource::GetSingle().GetResource(bitmapIdx)->hBitmap,width,height);
	DrawBitmap(hdcBuffer,ChatResource::GetSingle().GetResource(drawAreaBKBitmapIdx)->hBitmap,drawAreaWidth ,drawAreaHeight,drawAreaX,drawAreaY);
	int index = 0;
	int controlHeight = ChatClanManager::GetManager().controlHeight;
	bool isUseDefaultFont = ChatClanManager::GetManager().isUseDefaultFont;
	if(ChatClanManager::GetManager().addDlg.ChatHintDlgIsShow())
		BringWindowToTop(ChatClanManager::GetManager().addDlg.hDlg);
	if(ChatClanManager::GetManager().deleteDlg.ChatHintDlgIsShow())
		BringWindowToTop(ChatClanManager::GetManager().deleteDlg.hDlg);
	for (int i = 0; i <m_iOnlineMemberNum; i++)
	{
		m_OnlineMemberList[i] ->PlayerControlGetPosRectControl().x = drawPointX;
		m_OnlineMemberList[i] ->PlayerControlGetPosRectControl().y = drawPointY + (controlHeight + intermission) * i;
		if (m_OnlineMemberList[i] ->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
		{
			m_titleControl.TitleControlDrawItem(hdcBuffer);
			scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
			BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);
			SelectObject(hdcBuffer,hOldBitmap);
			DeleteObject(hTemp);
			DeleteDC(hdcBuffer);
			return ;
		}
		m_OnlineMemberList[i] ->DrawItem(hDlg, hdcBuffer, isUseDefaultFont);
	/*	{
		else
			{
				m_LeftMemberList[m_iOnlineMemberNum - i] ->PlayerControlGetPosRectControl().x = drawPointX;
				m_LeftMemberList[m_iOnlineMemberNum - i] ->PlayerControlGetPosRectControl().y = drawPointY + (controlHeight + intermission) * i;
				if (m_LeftMemberList[m_iOnlineMemberNum - i] ->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
				{
					m_titleControl.TitleControlDrawItem(hdcBuffer);
					scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
					BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);
					SelectObject(hdcBuffer,hOldBitmap);
					DeleteObject(hTemp);
					DeleteDC(hdcBuffer);
					return ;
				}
				m_LeftMemberList[m_iOnlineMemberNum - i] ->DrawItem(hdcBuffer, isUseDefaultFont);
			}
		}*/
	}

	scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
	m_titleControl.TitleControlDrawItem(hdcBuffer);
	BitBlt(hdc,0,0,width,height,hdcBuffer,0,0,SRCCOPY);
	SelectObject(hdcBuffer,hOldBitmap);
	DeleteObject(hTemp);
	DeleteDC(hdcBuffer);
}

void ChatClanInfoDlg::UpdateMemberList(ChatClanListControl * listControl)
{
	if (m_iOnlineMemberNum < _CLAN_MAX_MEMBER_NUM)
	{
		m_OnlineMemberList[m_iOnlineMemberNum] = listControl;
		m_iOnlineMemberNum++;
	}
/*	if (listControl ->m_playerData.bOnline)
	{
		if (m_iOnlineMemberNum < TONGMEMBER_MAX_NUM)
		{
			m_OnlineMemberList[m_iOnlineMemberNum] = listControl;
			m_iOnlineMemberNum++;
		}
	}
	else
	{
		if (m_iLeftMemberNum < TONGMEMBER_MAX_NUM)
		{
			m_LeftMemberList[m_iLeftMemberNum] = listControl;
			m_iLeftMemberNum++;
		}
	}
	*/
}

void ChatClanInfoDlg::ClearAll()
{
	for (int i = 0; i < m_iOnlineMemberNum; i++)
	{
		delete m_OnlineMemberList[i];
		m_OnlineMemberList[i] = NULL;
	}

	for (i = 0; i < m_iLeftMemberNum; i++)
	{
		delete m_LeftMemberList[i];
		m_LeftMemberList[i] = NULL;
	}
	m_iLeftMemberNum = 0;
	m_iOnlineMemberNum = 0;
}

void ChatClanInfoDlg::OnLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	if(scroll_bar.ChatScrollBarProcessClickUpButton(pt))
	{
		if(drawIndex > 0)
			drawIndex--;
		if(drawIndex<0)
			drawIndex = 0;
		PlayerInfoDlgUpdateDrawArea();
		return;
	}
	else if(scroll_bar.ChatScrollBarProcessClickDownButton(pt))
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
	if(scroll_bar.ChatScrollBarProcessClickScrollBar(pt))
		return;
	for(int i = 0; i < m_iOnlineMemberNum; i++)
	{
		const ONRECT& onwerRc = m_OnlineMemberList[i]->PlayerControlGetPosRect();
		RECT rc;
		rc.left = onwerRc.x;
		rc.right = rc.left+ onwerRc.width;
		rc.top = onwerRc.y;
		rc.bottom = onwerRc.y + onwerRc.height;
		if(IsInRect(pt,rc))
		{
			m_OnlineMemberList[i] ->SetSelected(TRUE, TRUE);
		}
		else
		{
			m_OnlineMemberList[i] ->SetSelected(FALSE, TRUE);
		}
		if(m_OnlineMemberList[i]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
		{
			break;
		}
	}
}

ChatClanListControl * ChatClanInfoDlg::GetControlByPoint(POINT& pt)
{
	for(int i = 0 ;i < m_iOnlineMemberNum; i++)
	{
		const ONRECT& onwerRc = m_OnlineMemberList[i]->PlayerControlGetPosRect();
		RECT rc;
		rc.left = onwerRc.x;
		rc.right = onwerRc.x + onwerRc.width;
		rc.top = onwerRc.y;
		rc.bottom = onwerRc.y + onwerRc.height;
		if(IsInRect(pt,rc))
		{
			return m_OnlineMemberList[i];
		}
	}
	return NULL;
}

void ChatClanInfoDlg::AddMember(const char * name, HWND callWnd)
{
	if(hDlg == ChatClanManager::GetManager().m_ClanInfoDlg.hDlg)
	{
		ChatClanPlayerData::GetPlayerData().AddMember(name, callWnd, enSULayer_Gens);
	}
}

void ChatClanInfoDlg::DeleteMember(HWND callWnd)
{
	BOOL isDelete = FALSE;
	for(int i = 0;i<m_iOnlineMemberNum;i++)
	{
		if(m_OnlineMemberList[i] ->PlayerControlIsSelected())
		{
			ChatClanPlayerData::GetPlayerData().DeleteMember(m_OnlineMemberList[i] ->m_playerData.guid, hDlg, enSULayer_Gens);
			ChatClanPlayerData::GetPlayerData().RequestDataList(hDlg, enSULayer_Gens);
			break;
		}
	}
}

void ChatClanInfoDlg::ClearSelected(BOOL update)
{
	for(int i = 0; i < m_iOnlineMemberNum;i++)
	{
		if(m_OnlineMemberList[i] ->PlayerControlIsSelected())
			m_OnlineMemberList[i] ->SetSelected(FALSE, update);
	}
}

void ChatClanInfoDlg::OnMouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	m_titleControl.MouseMove(*this,pt);
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

void ChatClanInfoDlg::OnLButtonUp(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	m_titleControl.TitleControlProcessLButtonUp();
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	if(scroll_bar.ChatScrollBarIsSetCapture())
	{
		scroll_bar.ChatScrollBarReleaseCapture();
		ReleaseCapture();
	}
	m_titleControl.MouseMove(*this,pt);
}

void ChatClanInfoDlg::OnLButtonDBLCLK(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	m_titleControl.OnLButtonDBLCLK(*this,pt);
	ChatClanManager::GetManager().addDlg.ChatHintDlgShow(false);
	ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(false);

	for(int i = 0; i < m_iOnlineMemberNum; i++)
	{
		const ONRECT& onwerRc = m_OnlineMemberList[i]->PlayerControlGetPosRect();
		RECT rc;
		rc.left = onwerRc.x;
		rc.right = rc.left+ onwerRc.width;
		rc.top = onwerRc.y;
		rc.bottom = onwerRc.y + onwerRc.height;
		if(IsInRect(pt,rc))
		{
			m_OnlineMemberList[i] ->OnLButtonDBLCLK();
			return;
		}
		if(m_OnlineMemberList[i]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
		{
			break;
		}
	}
}

ChatLuedInfoDlg::ChatLuedInfoDlg()
{
	m_iOnlineMemberNum = 0;
	m_iLeftMemberNum = 0;
}

ChatLuedInfoDlg::~ChatLuedInfoDlg()
{

}

void ChatLuedInfoDlg::CreateLuedInfoDlg(HWND hParent, DlgProcessFun pFun)
{
	if((hDlg =CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_FRIEND_LIST_DLG),
		hParent,
		(DLGPROC)pFun)) == 0)
	{
		DWORD err = GetLastError();
		return ;
	}
	PlayerInfoDlgShowDlg(FALSE);
}

void ChatLuedInfoDlg::LoadSRC(HWND hwnd)
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

	//创建button
	int button_Pos_X = 0;
	int button_Pos_Y = 0;
	int button_Width = 0;
	int button_Height = 0;

	int normal_idx = 0;
	int hover_idx = 0;

	//读取资源
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_X,0,&button_Pos_X);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_Y,0,&button_Pos_Y);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_WIDTH,0,&button_Width);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,_CHAT_SRC_HEIGHT,0,&button_Height);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_HIDE_SHOW_BNT_TEXT,"mouseOverSrcIdx",0,&hover_idx);
	//创建button
	if (!m_wndHideShowBnt.ChatWndCreate(_HIDE_SHOW_BNT_ID, WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
		hwnd, "", "button", button_Pos_X, button_Pos_Y, button_Width, button_Height))
	{
		DWORD err = GetLastError();
	}
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
	m_wndHideShowBnt.SetWndProcessFun(ChatWndProcessFun::LuedHideShowButton);
	m_wndHideShowBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
	m_wndHideShowBnt.ChatWndUpdate();

	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_X,0,&button_Pos_X);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_Y,0,&button_Pos_Y);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_WIDTH,0,&button_Width);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,_CHAT_SRC_HEIGHT,0,&button_Height);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,"normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_CHANGE_WND_TEXT,"mouseOverSrcIdx",0,&hover_idx);

	if (!m_wndChangeBnt.ChatWndCreate(_CHANGE_WND_ID, WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
		hwnd, "", "button", button_Pos_X, button_Pos_Y, button_Width, button_Height))
	{
		DWORD err = GetLastError();
	}
	

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

	m_wndChangeBnt.SetWndProcessFun(ChatWndProcessFun::LuedChangeButton);
	m_wndChangeBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
	m_wndChangeBnt.ChatWndUpdate();

	m_titleControl.TitleControlInit();
	m_titleControl.TitleControlSetParentHandle(hwnd);
	scroll_bar.ChatScrollBarLoadIniCtg();
	scroll_bar.ChatScrollBarSetParent(hwnd);

	char * itemName[] = {_CHAT_CLAN_INFO_DLG_INI_MODIFY_BULLETIN, _CHAT_LUED_INFO_DLG_INI_ADD_CLAN, _CHAT_LUED_INFO_DLG_INI_DELETE_CLAN};
	int itemID[] = {_CHAT_LUED_INFO_DLG_INI_MODIFY_BULLETIN_ID, _CHAT_LUED_INFO_DLG_INI_ADD_CLAN_ID, _CHAT_LUED_INFO_DLG_INI_DELETE_CLAN_ID};
	for (int i = 0; i < _CHAT_CLAN_INFO_DLG_BUTTON_NUM; i++)
	{
		iniFile.GetInteger(itemName[i], "x", 0, &button_Pos_X);
		iniFile.GetInteger(itemName[i], "y", 0, &button_Pos_Y);
		iniFile.GetInteger(itemName[i], "height", 0, &button_Height);
		iniFile.GetInteger(itemName[i], "width", 0, &button_Width);
		iniFile.GetInteger(itemName[i], "normalSrcIdx", 0, &normal_idx);
		iniFile.GetInteger(itemName[i], "mouseOverSrcIdx", 0, &hover_idx);

		if (!m_ButtenList[i].ChatWndCreate(itemID[i], WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
			hwnd, "", "button", button_Pos_X, button_Pos_Y, button_Width, button_Height))
		{
			DWORD err = GetLastError();
		}
		m_ButtenList[i].ChatWndSetResource(normal_idx, hover_idx, -1, normal_idx);
		memset(textInfo, 0, 256);
		iniFile.GetString(itemName[i], "textInfo", "", textInfo, 256);
		if (textInfo[0] != 0)
		{
			m_ButtenList[i].ChatWndSetText(textInfo);
			m_ButtenList[i].ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
			iniFile.GetString(itemName[i], "normalColor", "", szValue, MAX_PATH);

			int fontColor_r = 0;
			int fontColor_g = 0;
			int fontColor_b = 0;
			sscanf(szValue, "%d,%d,%d", &fontColor_r, &fontColor_g, &fontColor_b);
			m_ButtenList[i].ChatWndSetTextNormalColor(RGB(fontColor_r, fontColor_g, fontColor_b));
			m_ButtenList[i].m_EnableColor = RGB(fontColor_r, fontColor_g, fontColor_b);

			iniFile.GetString(itemName[i], "disableColor", "", szValue, MAX_PATH);
			sscanf(szValue, "%d,%d,%d", &fontColor_r, &fontColor_g, &fontColor_b);
			m_ButtenList[i].m_DisableColor = RGB(fontColor_r, fontColor_g, fontColor_b);

			char font[FONT_SIZE] = {0};
			iniFile.GetString(itemName[i], "font", "", font, FONT_SIZE);
			TFONT tFont;
			strcpy(tFont.fontName, font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0] = 0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc, &logFont, (FONTENUMPROC)EnumFontProc, (LPARAM)&tFont, 0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
			{
				m_ButtenList[i].ChatWndSetFont(font);
			}
			else
			{
				m_ButtenList[i].ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				m_ButtenList[i].ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
		m_ButtenList[i].ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
	}

	m_ButtenList[_CHAT_LUED_INFO_DLG_MODIFY_BULLETIN].SetWndProcessFun(ChatWndProcessFun::LuedModifyBulletinButtonProc);
	m_ButtenList[_CHAT_LUED_INFO_DLG_ADD_CLAN].SetWndProcessFun(ChatWndProcessFun::LuedAddClanButtonProc);
	m_ButtenList[_CHAT_LUED_INFO_DLG_DELETE_CLAN].SetWndProcessFun(ChatWndProcessFun::LuedDeleteClanButtonProc);

	ChatClanComboBox::GetClanComboBox().LoadIniFile(hDlg);
}

void ChatLuedInfoDlg::PaintDLG(HDC hdc)
{
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hTemp = CreateCompatibleBitmap(hdc,width,height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcBuffer,hTemp);
	DrawBitmap(hdcBuffer,ChatResource::GetSingle().GetResource(bitmapIdx)->hBitmap,width,height);
	DrawBitmap(hdcBuffer,ChatResource::GetSingle().GetResource(drawAreaBKBitmapIdx)->hBitmap,drawAreaWidth ,drawAreaHeight,drawAreaX,drawAreaY);
	int index = 0;
	int controlHeight = ChatClanManager::GetManager().controlHeight;
	bool isUseDefaultFont = ChatClanManager::GetManager().isUseDefaultFont;
	if(ChatClanManager::GetManager().addDlg.ChatHintDlgIsShow())
		BringWindowToTop(ChatClanManager::GetManager().addDlg.hDlg);
	if(ChatClanManager::GetManager().deleteDlg.ChatHintDlgIsShow())
		BringWindowToTop(ChatClanManager::GetManager().deleteDlg.hDlg);

	for (int i = 0; i <m_iOnlineMemberNum; i++)
	{
		m_OnlineMemberList[i] ->PlayerControlGetPosRectControl().x = drawPointX;
		m_OnlineMemberList[i] ->PlayerControlGetPosRectControl().y = drawPointY + (controlHeight + intermission) * i;
		if (m_OnlineMemberList[i] ->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
		{
			m_titleControl.TitleControlDrawItem(hdcBuffer);
			scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
			BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);
			SelectObject(hdcBuffer,hOldBitmap);
			DeleteObject(hTemp);
			DeleteDC(hdcBuffer);
			return ;
		}
		m_OnlineMemberList[i] ->DrawItem(hDlg, hdcBuffer, isUseDefaultFont);
	/*	{
		else
			{
				m_LeftMemberList[m_iOnlineMemberNum - i] ->PlayerControlGetPosRectControl().x = drawPointX;
				m_LeftMemberList[m_iOnlineMemberNum - i] ->PlayerControlGetPosRectControl().y = drawPointY + (controlHeight + intermission) * i;
				if (m_LeftMemberList[m_iOnlineMemberNum - i] ->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
				{
					m_titleControl.TitleControlDrawItem(hdcBuffer);
					scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
					BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);
					SelectObject(hdcBuffer,hOldBitmap);
					DeleteObject(hTemp);
					DeleteDC(hdcBuffer);
					return ;
				}
				m_LeftMemberList[m_iOnlineMemberNum - i] ->DrawItem(hdcBuffer, isUseDefaultFont);
			}
		}*/
	}
	scroll_bar.ChatScrollBarDrawItem(hdcBuffer);
	m_titleControl.TitleControlDrawItem(hdcBuffer);
	BitBlt(hdc,0,0,width,height,hdcBuffer,0,0,SRCCOPY);
	SelectObject(hdcBuffer,hOldBitmap);
	DeleteObject(hTemp);
	DeleteDC(hdcBuffer);
}

void ChatLuedInfoDlg::UpdateMemberList(ChatClanListControl * listControl)
{
	if (m_iOnlineMemberNum < _CLAN_MAX_MEMBER_NUM)
	{
		m_OnlineMemberList[m_iOnlineMemberNum] = listControl;
		m_iOnlineMemberNum++;
	}
/*	if (listControl ->m_playerData.bOnline)
	{
		if (m_iOnlineMemberNum < TONGMEMBER_MAX_NUM)
		{
			m_OnlineMemberList[m_iOnlineMemberNum] = listControl;
			m_iOnlineMemberNum++;
		}
	}
	else
	{
		if (m_iLeftMemberNum < TONGMEMBER_MAX_NUM)
		{
			m_LeftMemberList[m_iLeftMemberNum] = listControl;
			m_iLeftMemberNum++;
		}
	}
	*/
}

void ChatLuedInfoDlg::ClearAll()
{
	for (int i = 0; i < m_iOnlineMemberNum; i++)
	{
		delete m_OnlineMemberList[i];
		m_OnlineMemberList[i] = NULL;
	}

	for (i = 0; i < m_iLeftMemberNum; i++)
	{
		delete m_LeftMemberList[i];
		m_LeftMemberList[i] = NULL;
	}
	m_iLeftMemberNum = 0;
	m_iOnlineMemberNum = 0;
}

void ChatLuedInfoDlg::OnLButtonDown(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	if(scroll_bar.ChatScrollBarProcessClickUpButton(pt))
	{
		if(drawIndex > 0)
			drawIndex--;
		if(drawIndex<0)
			drawIndex = 0;
		PlayerInfoDlgUpdateDrawArea();
		return;
	}
	else if(scroll_bar.ChatScrollBarProcessClickDownButton(pt))
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
	if(scroll_bar.ChatScrollBarProcessClickScrollBar(pt))
		return;
	for(int i = 0; i < m_iOnlineMemberNum; i++)
	{
		const ONRECT& onwerRc = m_OnlineMemberList[i]->PlayerControlGetPosRect();
		RECT rc;
		rc.left = onwerRc.x;
		rc.right = rc.left+ onwerRc.width;
		rc.top = onwerRc.y;
		rc.bottom = onwerRc.y + onwerRc.height;
		if(IsInRect(pt,rc))
		{
			m_OnlineMemberList[i] ->SetSelected(TRUE, TRUE);
		}
		else
		{
			m_OnlineMemberList[i] ->SetSelected(FALSE, TRUE);
		}
		if(m_OnlineMemberList[i]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
		{
			break;
		}
	}
}

ChatClanListControl * ChatLuedInfoDlg::GetControlByPoint(POINT& pt)
{
	for(int i = 0 ;i < m_iOnlineMemberNum; i++)
	{
		const ONRECT& onwerRc = m_OnlineMemberList[i]->PlayerControlGetPosRect();
		RECT rc;
		rc.left = onwerRc.x;
		rc.right = onwerRc.x + onwerRc.width;
		rc.top = onwerRc.y;
		rc.bottom = onwerRc.y + onwerRc.height;
		if(IsInRect(pt,rc))
		{
			return m_OnlineMemberList[i];
		}
	}
	return NULL;
}

void ChatLuedInfoDlg::AddMember(const char * name, HWND callWnd)
{
	if(hDlg == ChatLuedManager::GetManager().m_LuedInfoDlg.hDlg)
	{
		ChatClanPlayerData::GetPlayerData().AddMember(name, callWnd, enSULayer_Tong);
	}
}

void ChatLuedInfoDlg::DeleteMember(HWND callWnd)
{
	BOOL isDelete = FALSE;
	LPCHATCLANINFO * pInfoList = ChatClanComboBox::GetClanComboBox().m_ClanList;
	for (int i = 0; i < ChatClanComboBox::GetClanComboBox().m_iClanNum; i++)
	{
		if (pInfoList[i] ->m_blIsSelected)
		{
			ChatClanPlayerData::GetPlayerData().DeleteMember(pInfoList[i] ->m_ClanGuid, hDlg, enSULayer_Tong);
			break;
		}
	}
	if (ChatClanPlayerData::GetPlayerData().InitInterface())
	{
		ChatClanPlayerData::GetPlayerData().RequestDataList(ChatClanComboBox::GetClanComboBox().m_hDownDialg, enSULayer_Tong);
	}
}

void ChatLuedInfoDlg::ClearSelected(BOOL update)
{
	for(int i = 0; i < m_iOnlineMemberNum;i++)
	{
		if(m_OnlineMemberList[i] ->PlayerControlIsSelected())
			m_OnlineMemberList[i] ->SetSelected(FALSE, update);
	}
}

void ChatLuedInfoDlg::OnMouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	m_titleControl.MouseMove(*this,pt);
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

void ChatLuedInfoDlg::OnLButtonUp(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	m_titleControl.TitleControlProcessLButtonUp();
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	if(scroll_bar.ChatScrollBarIsSetCapture())
	{
		scroll_bar.ChatScrollBarReleaseCapture();
		ReleaseCapture();
	}
	m_titleControl.MouseMove(*this,pt);
}

void ChatLuedInfoDlg::OnLButtonDBLCLK(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	m_titleControl.OnLButtonDBLCLK(*this,pt);
	ChatClanManager::GetManager().addDlg.ChatHintDlgShow(false);
	ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(false);

	for(int i = 0; i < m_iOnlineMemberNum; i++)
	{
		const ONRECT& onwerRc = m_OnlineMemberList[i]->PlayerControlGetPosRect();
		RECT rc;
		rc.left = onwerRc.x;
		rc.right = rc.left+ onwerRc.width;
		rc.top = onwerRc.y;
		rc.bottom = onwerRc.y + onwerRc.height;
		if(IsInRect(pt,rc))
		{
			m_OnlineMemberList[i] ->OnLButtonDBLCLK();
			return;
		}
		if(m_OnlineMemberList[i]->PlayerControlGetPosRect().y > drawAreaY + drawAreaHeight - intermission)
		{
			break;
		}
	}
}
