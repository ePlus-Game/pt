#include <windows.h>
#include <windowsx.h>
#include <zmouse.h>
#include <tchar.h>
#include <commctrl.h>
#include <vector>
#include <list>
#include "CoreShell.h"
#include "loadSrcWnd/GDILoadBitmap.h"
#include "SocialComDef.h"

extern iCoreShell*							g_pCoreShell;

using namespace std;

///////////////////

//////////////////
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

#include "Ui/UiCase/UiChatWindow.h"
#include "ChatDataDef.h"
#include "Ui/UiCase/UiBubble.h"
#include "resource.h"
using namespace CHAT;

#include "KIniFile.h"
#include "chatWindow/ChatWndProc.h"
#include "chatWindow/ChatResource.h"
#include "cfs_filelogs.h"

bool         ChatManager::isCurrrent = false;
bool         ChatManager::needUpdate = false;
DWORD        ChatManager::updataTime = 500;
DWORD        ChatManager::startUpdateTime = 0;;
ChatManager::ChatManager()
{
	currentPageIndex = 0;
	isShowMainWnd = TRUE;
	
//	isFlash
}
ChatManager::~ChatManager()
{
	while (!chatPages.empty())
	{
		ChatPage* pTemp = chatPages.back();
		chatPages.pop_back();
		delete pTemp;
	}
}
//////////////////////////////////////////////////////////////////////////////
LRESULT ChatManager::ChatManagerProcessButtonCommand(HWND hwnd,WPARAM wParam,LPARAM lParam)
{

	return 0;
}
LRESULT ChatManager::ChatManagerProcessMButtonEvent(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}
LRESULT ChatManager::ChatManagerProcessMouseHover(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	POINT mouse_pt;
	POINT pt;
	GetCursorPos(&mouse_pt);
	RECT btn_rect;
	int size = chatPages.size();
	for(int i = 0; i<size;i++)
	{
		pt.x = mouse_pt.x;
		pt.y = mouse_pt.y;
		ChatButton& button = chatPages[i]->ChatPageGetButton(); 
		GetClientRect(button.ChatWndGetHandle(),&btn_rect);
		ScreenToClient(button.ChatWndGetHandle(),&pt);
		if(IsInRect(pt,btn_rect))
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShow(TRUE);
			}
		}
	}

	return FALSE;

}
LRESULT ChatManager::ChatManagerProcessMouseLeave(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	POINT mouse_pt;
	POINT pt;
	GetCursorPos(&mouse_pt);
	RECT btn_rect;
	int size = chatPages.size();
	for(int i = 0; i<size;i++)
	{
		pt.x = mouse_pt.x;
		pt.y = mouse_pt.y;
		ChatButton& button = chatPages[i]->ChatPageGetButton(); 
		GetClientRect(button.ChatWndGetHandle(),&btn_rect);
		ScreenToClient(button.ChatWndGetHandle(),&pt);
		if(IsInRect(pt,btn_rect))
		{
			if(button.ChatWndGetState() !=_CHAT_BUTTON_STATE_MOUSEDOWN&&
				button.isFlashing==FALSE)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
				return 0;
			}
		}
	}
	for(int j = 0; j< _CHAT_NORMAL_BUTTON_NUMBER;j++)
	{
		pt.x = mouse_pt.x;
		pt.y = mouse_pt.y;
		GetClientRect(normalButton[j].ChatWndGetHandle(),&btn_rect);
		ScreenToClient(normalButton[j].ChatWndGetHandle(),&pt);
		if(IsInRect(pt,btn_rect))
		{
			if(normalButton[j].ChatWndGetState() !=_CHAT_BUTTON_STATE_MOUSEDOWN&&
				normalButton[j].isFlashing==FALSE)
			{
				normalButton[j].ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				normalButton[j].ChatWndUpdate();
				return 0;
			}
		}
	}
	pt.x = mouse_pt.x;
	pt.y = mouse_pt.y;
	GetClientRect(showButton.ChatWndGetHandle(),&btn_rect);
	ScreenToClient(showButton.ChatWndGetHandle(),&pt);
	if(IsInRect(pt,btn_rect))
	{
		if(showButton.ChatWndGetState() !=_CHAT_BUTTON_STATE_MOUSEDOWN&&
			showButton.isFlashing == FALSE)
		{
				showButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				showButton.ChatWndUpdate();
				return 0;
		}
	}
	pt.x = mouse_pt.x;
	pt.y = mouse_pt.y;
	GetClientRect(uiComboBox.selectItemButton.ChatWndGetHandle(),&btn_rect);
	ScreenToClient(uiComboBox.selectItemButton.ChatWndGetHandle(),&pt);
	if(IsInRect(pt,btn_rect))
	{
		if(uiComboBox.selectItemButton.ChatWndGetState() !=_CHAT_BUTTON_STATE_MOUSEDOWN&&
			uiComboBox.selectItemButton.isFlashing==FALSE)
		{
				uiComboBox.selectItemButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				uiComboBox.selectItemButton.ChatWndUpdate();
				return 0;
		}
	}
	return 0;
}

LRESULT ChatManager::ChatManagerProcessMouseMove(HWND hwnd,WPARAM wParm,LPARAM lParam)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_HOVER|TME_LEAVE;
	tme.dwHoverTime=1;
	tme.hwndTrack=B2ChatDialog::m_hDialog;
	_TrackMouseEvent(&tme);


	int size = chatPages.size();
	for(int i = 0;i<size; i++)
	{
		ChatButton& button = chatPages[i]->ChatPageGetButton();
		if(button.ChatWndGetState() ==_CHAT_BUTTON_STATE_MOUSEOVER&&button.isFlashing==FALSE)
		{
			button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			button.ChatWndUpdate();
			return 0;

		}
	}
	for(int j = 0;j<_CHAT_NORMAL_BUTTON_NUMBER;j++)
	{
		if(normalButton[j].ChatWndGetState() ==_CHAT_BUTTON_STATE_MOUSEOVER&&normalButton[j].isFlashing==FALSE)
		{
			normalButton[j].ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			normalButton[j].ChatWndUpdate();
			return 0;
			
		}
	}
	if(showButton.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEOVER&&
		showButton.isFlashing == FALSE)
	{
		showButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		showButton.ChatWndUpdate();
		return 0;
	}
	if(uiComboBox.selectItemButton.ChatWndGetState()==_CHAT_BUTTON_STATE_MOUSEOVER&&
		uiComboBox.selectItemButton.isFlashing == FALSE)
	{
		uiComboBox.selectItemButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		uiComboBox.selectItemButton.ChatWndUpdate();
		return 0;
	}
	return 0;
}


LRESULT ChatManager::ChatManagerProcessOnwerDraw(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
	int length = chatPages.size();
	for(int i = 0; i < length;i++)
	{
		ChatButton& button = chatPages[i]->ChatPageGetButton();
		if(button.ChatWndGetID() ==lpdis->CtlID)
		{
			button.ChatWndDrawItem(lpdis->hDC);
			return 0;
		}
	}
	for(int j = 0;j<_CHAT_NORMAL_BUTTON_NUMBER;j++)
	{
		if(normalButton[j].ChatWndGetID()== lpdis->CtlID)
		{
			normalButton[j].ChatWndDrawItem(lpdis->hDC);
			return 0;
		}
	}
	if(lpdis->CtlID == _CHAT_EDIT_CHAT_ID)
	{
		edit.ChatWndDrawItem(lpdis->hDC);
		return 0;
	}
	if(lpdis->CtlID == _CHAT_STATIC_PAGE_ID)
	{
		infoWnd.ChatWndDrawItem(lpdis->hDC);
		return 0;
	}
	if(lpdis->CtlID == _CHAT_SHOWFACE_BUTTON)
	{
		showButton.ChatWndDrawItem(lpdis->hDC);
		return 0;
	}
	if(lpdis->CtlID==uiComboBox.selectItemButton.ChatWndGetID())
	{
		uiComboBox.selectItemButton.ChatWndDrawItem(lpdis->hDC);
	}
	return 0;
}


LRESULT ChatManager::ChatManagerProcessMouseWheel(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	int zDelta = HIWORD(wParam);
	if(zDelta ==120)
	{
//		if(chatPages[currentPageIndex]->ChatPageGetDrawPoint().y>=0)///pages[currentPage].wndDrawPoint.y >= 0)
//			return 0;
		if(chatPages[currentPageIndex]->chatWndRender.infoWndLayOut[0].pt.y>infoWnd.renderY)
		{
			chatPages[currentPageIndex]->chatWndRender.infoWndLayOut[0].pt.y = infoWnd.renderY;
			return 0;
		}
		chatPages[currentPageIndex]->chatWndRender.ChatWndRenderScroll(GDIRender::wordSize);
//		chatPages[currentPageIndex]->ChatPageGetDrawPoint().y+=10;
		if(infoWnd.ChatInfoWndIsScroll())
		{
			infoWnd.ChatInfoWndDisableScroll();
		}
	}
	else
	{
//		if(infoWnd.ChatWndGetRect().bottom-25>=B2ChatDialog::pLayout->getRenderArea().getHeight()+chatPages[currentPageIndex]->ChatPageGetDrawPoint().y)
//		{
//			infoWnd.ChatInfoWndEnableScroll();
//			normalButton[_DOWN_BUTTON_INDEX].ChatWndKillTimer();
///			return 0;
//		}
		if(infoWnd.ChatWndGetRect().bottom - infoWnd.renderY>=chatPages[currentPageIndex]->chatWndRender.allHeight+chatPages[currentPageIndex]->chatWndRender.infoWndLayOut[0].pt.y)
		{
			infoWnd.ChatInfoWndEnableScroll();
			normalButton[_DOWN_BUTTON_INDEX].ChatWndKillTimer();
			return 0;
		}
		if(infoWnd.ChatInfoWndIsScroll())
		{
			infoWnd.ChatInfoWndDisableScroll();
		}
//		chatPages[currentPageIndex]->ChatPageGetDrawPoint().y-=10;
		chatPages[currentPageIndex]->chatWndRender.ChatWndRenderScroll(-GDIRender::wordSize);
	}
	infoWnd.ChatWndUpdate();
	return 0;
}
LRESULT ChatManager::ChatManagerProcessPaint(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd,&ps);
	HDC hdc1 = CreateCompatibleDC(hdc);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc1,ChatResource::GetSingle().GetResource(B2ChatDialog::chatWndBkBitmapIdx)->hBitmap);
	BITMAP bm;
	GetObject(ChatResource::GetSingle().GetResource(B2ChatDialog::chatWndBkBitmapIdx)->hBitmap,sizeof(bm),&bm);
	m_ScrollBar.ChatScrollBarDrawItem(hdc1);
	StretchBlt(hdc,0,0,B2ChatDialog::m_dialogWidth,
		B2ChatDialog::m_dialogHeight,hdc1,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	SelectObject(hdc1,hOldBitmap);
	DeleteDC(hdc1);
	EndPaint(hwnd,&ps);
	return 0;
}
LRESULT ChatManager::ChatManagerProcessKeyDown(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	return 0;
}

void ChatManager::ChatManagerSendMessageToServer()
{
	LOElemInfo* elemList = 0;
	int item = uiComboBox.currentSelected;
	int size = chatChannels.size();
	int channalIndex =0 ;
	switch(item)
	{
	case SYSTEM_CHANNALS_TALK:
		{
			for(channalIndex  = 0; channalIndex < size; channalIndex++)
			{
				if(chatChannels[channalIndex].dwChannelID == SYSTEM_ROOM_ID )
					break;
			}
			break;
		}
	case WORLD_CHANNALES_TALK:
		{
			for(channalIndex  = 0; channalIndex < size; channalIndex++)
			{
				if(chatChannels[channalIndex].dwChannelID == GLOBAL_ROOM_ID )
					break;
			}
			break;
		}
	case NEAR_CHANNALES_TALK:
		{
			for(channalIndex  = 0; channalIndex < size; channalIndex++)
			{
				if(chatChannels[channalIndex].dwChannelID == LOCAL_ROOM_ID )
					break;
			}
			break;
		}
	case TEAM_CHANNALES_TALK:
		{
			for(channalIndex  = 0; channalIndex < size; channalIndex++)
			{
				if(strcmp(chatChannels[channalIndex].szChannelName,ChatString::ChatStringGetString().chTeamName)==0)
					break;
			}
			break;
		}
	case SHIZU_CHANNALES_TALK:
		{
			for(channalIndex  = 0; channalIndex < size; channalIndex++)
			{
				if(strcmp(chatChannels[channalIndex].szChannelName,ChatString::ChatStringGetString().chSizhuName)==0)
					break;
			}
			break;
		}
	case GUOJIA_CHANNALES_TALK:
		{
			for(channalIndex  = 0; channalIndex < size; channalIndex++)
			{
				if(strcmp(chatChannels[channalIndex].szChannelName,ChatString::ChatStringGetString().chGuojiaName)==0)
					break;
			}
			break;
		}
	case ZHUHOU_CHANNALES_TALK:
		{
			for(channalIndex  = 0; channalIndex < size; channalIndex++)
			{
				if(strcmp(chatChannels[channalIndex].szChannelName,ChatString::ChatStringGetString().chZhuhouName)==0)
					break;
			}
			break;
		}
	case MAP_CHANNALES_TALK:
		{
			for(channalIndex  = 0; channalIndex < size; channalIndex++)
			{
				if(strcmp(chatChannels[channalIndex].szChannelName,ChatString::ChatStringGetString().chMapName)==0)
					break;
			}
			break;

		}
		break;

	}
	int channelID = chatChannels[channalIndex].dwChannelID;
	int elemNumbers = B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->getElemList(elemList);
	if(elemNumbers<=0)
		return ;
	strMsg[0] = 0;
	int textLength = 0;
	char textHead[COMMON_CLIENT_MSG_LEN_64] = {0};

	for (int i = 0; i < elemNumbers; i++)
	{
		if(elemList[i].gameObj._objType == LO_GO_ITEM)
			sprintf(textHead,"<I=%d|%d|%d|",
				elemList[i].gameObj._objId[0],
				elemList[i].gameObj._objId[1],
				elemList[i].gameObj._objId[2]);
		else
		if(elemList[i].gameObj._objType == LO_GO_NOTHING)
			strcpy(textHead,"<N=");
		else
		if(elemList[i].gameObj._objType == LO_GO_FACE)
			sprintf(textHead,"<F=%d",elemList[i].gameObj._objId[0]);
		else
		if(elemList[i].gameObj._objType == LO_GO_POSITION)
			sprintf(textHead,"<P=%d|%d|%d|%d|",
			        elemList[i].gameObj._objId[0],
					elemList[i].gameObj._objId[1],
					elemList[i].gameObj._objId[2],
					elemList[i].gameObj._objId[3]);
		if((textLength+=strlen(textHead))>LAYOUT_TEXT_MAX_LEN)
			break;
		char* ansiText = 0;
		int ansiTextLen = 0;
		if(elemList[i].isShowDes)
			ansiTextLen = unicodeToAnsi(elemList[i].description.get(),ansiText);
		else
			ansiTextLen = unicodeToAnsi(elemList[i].content.get(),ansiText);
		for(int j = 0; j < ansiTextLen; j++)
		{
			if(ansiText[j] == '<')
				ansiText[j] =1;
			if(ansiText[j] == '>')
				ansiText[j] = 2;
		}
		if((textLength+=ansiTextLen)>LAYOUT_TEXT_MAX_LEN)
		{
			delete [] ansiText;
			break;
		}
		strcat(strMsg,textHead);
		if(elemList[i].gameObj._objType != LO_GO_FACE)
			strcat(strMsg,ansiText);
		strcat(strMsg,">");
		delete [] ansiText;
		ansiText = 0;
	}

	char cozeName[COMMON_CLIENT_MSG_LEN_64];
	cozeName[0] = 0;
	sscanf(strMsg, "<N=/%[^ ] %[^\0]", cozeName, strMsg + 3);
	
	if(cozeName[0] != 0)
	{
		strMsg[0] = '<';
		strMsg[1] = 'N';
		strMsg[2] = '=';
		if(strlen(strMsg) > 4)
		{
			g_pCoreShell->OperationRequest( GOI_SEND_CHAT_DATE_P2P, (UINT)cozeName, (int)strMsg);
		}
	}
	else
	{
		g_pCoreShell->OperationRequest( GOI_SEND_CHAT_DATE_P2R, (UINT)channelID, (int)strMsg);
	}
	delete [] elemList;
}

ChatPage* ChatManager::ChatManagerGetPage(int index)
{
	int size = chatPages.size();
	
	for(int i = 0; i < size;i++)
	{
		ChatPage* pPage = chatPages[i];
		if(pPage->ChatPageGetID() == index)
			return pPage;
	}
	return NULL;
}
int ChatManager::ChatManagerGetCurrentPage() const
{
	return currentPageIndex;
}
ChatEditBox* ChatManager::ChatManagerGetEditBox()
{
	return &edit;
}
ChatInfoWnd* ChatManager::ChatManagerGetInfoWnd()
{
	return &infoWnd;
}

ChatManagerScrollBar * ChatManager::ChatManagerGetScrollBar()
{
	return &m_ScrollBar;
}

int ChatManager::ChatManagerRecMsgAtPage(int chanID)
{

	return 0;
}


void         ChatManager::ChatManagerChannelEnable(int channelID)
{
	int numberChannel = chatChannels.size();
	for(int j = 0; j < numberChannel;j++)
	{
		Ui_Channel_Param & channel = chatChannels[j];
		if(channel.dwChannelID == channelID)
		{
			chatChannelsClose[j] = false;
		}
	}
}

void ChatManager::ChangeTextColor(char * segText, const char * color, int chanId, bool bGM)
{
	if (bGM)
	{
		strcat(segText, KUiChanMgr::getSinglton().getChanColor(chanId, bGM));
	}
	else
	{
		strcat(segText, color);
	}
}

void ChatManager::ChangeTextFont(char * segText, const char * font, int chanId, bool bGM)
{
	if (bGM)
	{
		strcat(segText, KUiChanMgr::getSinglton().getChanFont(chanId, bGM));
	}
	else
	{
		strcat(segText, font);
	}
}

void ChatManager::ClickScrollBar(const POINT & pt)
{
	if (m_ScrollBar.IsClickScrollBar(pt))
	{
		ProcessScrollBarRectMove(pt);
	}
}

void ChatManager::ProcessScrollBarRectMove(const POINT & pt)
{
	ChatPage* pCurrentPage = ChatManagerGetPage(currentPageIndex);
	if (pCurrentPage == NULL)
	{
		return;
	}

	if (pCurrentPage->chatWndRender.numberUsed > 0)
	{
		if((pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed - 1].pt.y +
			pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed - 1].renderHeight) < (infoWnd.renderRect.bottom - infoWnd.renderY))
			return;
	}
	else
	{
		return;
	}

	m_ScrollBar.MoveBarRect(pt);
	if(edit.focus)
	{
		edit.ChatEditWndSetFocus(FALSE);
		edit.ChatWndUpdate();
	}

	float NewRectPos		 = m_ScrollBar.GetBarRectPos();
	float rectHeight		 = m_ScrollBar.GetBarRectHeight();
	float scrollLength		 = m_ScrollBar.ChatScrollBarGetScrollLength();
	float layoutAllHeight	 = pCurrentPage->chatWndRender.allHeight;
	float layoutRenderHeight = infoWnd.renderRect.bottom - infoWnd.renderY;

	int pos = (int)(NewRectPos * (layoutAllHeight - layoutRenderHeight) / (scrollLength - rectHeight));

	pCurrentPage->chatWndRender.ChatWndRenderSetRenderPos(-pos);
	infoWnd.ChatInfoWndDisableScroll();

	if (m_ScrollBar.GetBarRectPos() <= 0)
	{
		pCurrentPage->chatWndRender.ChatWndRenderSetRenderPos(infoWnd.renderY);
		infoWnd.ChatInfoWndDisableScroll();
	}
	else if (m_ScrollBar.GetBarRectPos() >= (m_ScrollBar.ChatScrollBarGetScrollLength() - m_ScrollBar.GetBarRectHeight()))
	{
		pCurrentPage->chatWndRender.ChatWndRenderSetRenderPos(-(pCurrentPage->chatWndRender.allHeight - infoWnd.renderRect.bottom));
		infoWnd.ChatInfoWndEnableScroll();
	}

	if(pCurrentPage->chatWndRender.infoWndLayOut[0].pt.y > infoWnd.renderY)
	{
		pCurrentPage->chatWndRender.ChatWndRenderScroll(infoWnd.renderY);
		infoWnd.ChatInfoWndDisableScroll();
	}

	if(itemInfo.ChatTipWndIsShow())
	{
		itemInfo.ChatTipWndShow(FALSE);
	}
	faceDialog.FaceDialogShow(FALSE);
	playerRelate.ChatTipShow(FALSE);
		uiComboBox.uiComboBoxShowDownDialog(false);
	infoWnd.ChatWndUpdate();
}

bool ChatManager::IsClickScrollBarRect(const POINT & pt)
{
	return m_ScrollBar.IsClickRect(pt);
}

bool ChatManager::IsClickScrollBarDownOrUpBnt(const POINT & pt)
{
	bool ret = false;
	if (m_ScrollBar.ChatScrollBarProcessClickDownButton(pt))
	{
		if(edit.focus)
		{
			edit.ChatEditWndSetFocus(FALSE);
			edit.ChatWndUpdate();
		}
		ChatPage* pCurrentPage = ChatManagerGetPage(currentPageIndex);
		if (pCurrentPage == NULL)
		{
			return false;
		}

		if(pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed - 1].pt.y +
			pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed - 1].renderHeight < infoWnd.ChatWndGetRect().bottom - infoWnd.renderY)
			return false;
		pCurrentPage->chatWndRender.ChatWndRenderScroll(-GDIRender::wordSize);
		if(pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed - 1].pt.y +
			pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed - 1].renderHeight < infoWnd.ChatWndGetRect().bottom - infoWnd.renderY)
		{
			pCurrentPage->chatWndRender.ChatWndRenderScroll(infoWnd.ChatWndGetRect().bottom - pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed - 1].pt.y -
				pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed - 1].renderHeight);
			infoWnd.ChatInfoWndEnableScroll();
			pCurrentPage->chatWndRender.ChatWndRenderAutoScroll(infoWnd.ChatWndGetRect().bottom - infoWnd.renderY);
		}
		infoWnd.ChatWndUpdate();
		if(itemInfo.ChatTipWndIsShow())
		{
			itemInfo.ChatTipWndShow(FALSE);
		}
		faceDialog.FaceDialogShow(FALSE);
		playerRelate.ChatTipShow(FALSE);
		uiComboBox.uiComboBoxShowDownDialog(false);
		ret = true;
	}
	else if (m_ScrollBar.ChatScrollBarProcessClickUpButton(pt))
	{
		if(edit.focus)
		{
			edit.ChatEditWndSetFocus(FALSE);
			edit.ChatWndUpdate();
		}
		ChatPage* pCurrentPage = ChatManagerGetPage(currentPageIndex);

		if (pCurrentPage == NULL)
		{
			return false;
		}
		pCurrentPage->chatWndRender.ChatWndRenderScroll(GDIRender::wordSize);
		if(pCurrentPage->chatWndRender.infoWndLayOut[0].pt.y > infoWnd.renderY)
		{
			pCurrentPage->chatWndRender.ChatWndRenderScroll(-(pCurrentPage->chatWndRender.infoWndLayOut[0].pt.y) + infoWnd.renderY);
		}

		infoWnd.ChatWndUpdate();
		infoWnd.ChatInfoWndDisableScroll();
		if(itemInfo.ChatTipWndIsShow())
		{
			itemInfo.ChatTipWndShow(FALSE);
		}
		faceDialog.FaceDialogShow(FALSE);
		playerRelate.ChatTipShow(FALSE);
		uiComboBox.uiComboBoxShowDownDialog(false);
		ret = true;
	}

	if (ret)
	{
		RecalculateScroll();
	}
	return ret;
}

void ChatManager::SwapChannel()
{
	MapChannelInfo channelInfo;
	if (!KUiChanMgr::getSinglton().getChanInfo(MAP_ROOM_ID, channelInfo))
	{
		if (strcmp(uiComboBox.selectItemButton.ChatWndGetText(), uiComboBox.downDialgButtons[MAP_CHANNALES_TALK].ChatWndGetText()) == 0)
		{
			uiComboBox.selectItemButton.ChatWndSetText(ChatString::ChatStringGetString().chMapName);
			uiComboBox.selectItemButton.ChatWndUpdate();
		}
		uiComboBox.downDialgButtons[MAP_CHANNALES_TALK].ChatWndSetText(ChatString::ChatStringGetString().chMapName);
		uiComboBox.downDialgButtons[MAP_CHANNALES_TALK].ChatWndUpdate();
	}
	else
	{
		if (strcmp(uiComboBox.selectItemButton.ChatWndGetText(), uiComboBox.downDialgButtons[MAP_CHANNALES_TALK].ChatWndGetText()) == 0)
		{
			uiComboBox.selectItemButton.ChatWndSetText(channelInfo.szChannelName);
			uiComboBox.selectItemButton.ChatWndUpdate();
		}
		uiComboBox.downDialgButtons[MAP_CHANNALES_TALK].ChatWndSetText(channelInfo.szChannelName);
		uiComboBox.downDialgButtons[MAP_CHANNALES_TALK].ChatWndUpdate();
	}
}

void           ChatManager::ChatManagerChannelCloseOne(int channelID)
{
	int numberChannel = chatChannels.size();
	for(int j = 0; j < numberChannel;j++)
	{
		Ui_Channel_Param & channel = chatChannels[j];
		if(channel.dwChannelID == channelID)
		{
			chatChannelsClose[j] = true;
		}
	}
}
bool            ChatManager::ChatManagerChannelClose(int channelID)
{
	int numberPage = chatPages.size();
	for(int i = 0; i < numberPage; i++)
	{
		chatPages[i]->ChatPageCloseChannel(channelID);
	}
	for(int j = 0; j < chatChannels.size();j++)
	{
		Ui_Channel_Param & channel = chatChannels[j];
		if(channel.dwChannelID == channelID)
		{
			if(strcmp(channel.szChannelName,uiComboBox.selectItemButton.ChatWndGetText())==0)
			{
				uiComboBox.currentSelected = NEAR_CHANNALES_TALK;
				uiComboBox.selectItemButton.ChatWndSetText(uiComboBox.downDialgButtons[uiComboBox.currentSelected].ChatWndGetText());
				uiComboBox.selectItemButton.ChatWndSetFont(uiComboBox.downDialgButtons[uiComboBox.currentSelected].ChatWndGetFontName());
				uiComboBox.selectItemButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
				if(uiComboBox.downDialgButtons[uiComboBox.currentSelected].ChatWndGetAttr()&_CHAT_WND_ATTR_USE_DEFUALT_FONT)
					uiComboBox.selectItemButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
				const char* colorText = KUiChanMgr::getSinglton().getChanColor(LOCAL_ROOM_ID,false);
				LOColor color(colorText);
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->setColor(color);
			}
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chGuojiaName) == 0)
			{
				uiComboBox.downDialgButtonEnable[GUOJIA_CHANNALES_TALK] = false;
				uiComboBox.UiComBoBoxAdjustDownDlg();
			}
			else
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chNearName) == 0)
			{
				uiComboBox.downDialgButtonEnable[NEAR_CHANNALES_TALK] = false;
				uiComboBox.UiComBoBoxAdjustDownDlg();
			}
			else
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chSizhuName) == 0)
			{
				uiComboBox.downDialgButtonEnable[SHIZU_CHANNALES_TALK] = false;
				uiComboBox.UiComBoBoxAdjustDownDlg();
			}
			else
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chSystemName) == 0)
			{
				uiComboBox.downDialgButtonEnable[SYSTEM_CHANNALS_TALK] = false;
				uiComboBox.UiComBoBoxAdjustDownDlg();
			}
			else
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chTeamName) == 0)
			{
				uiComboBox.downDialgButtonEnable[TEAM_CHANNALES_TALK] = false;
				uiComboBox.UiComBoBoxAdjustDownDlg();
			}
			else
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chWorldName)==0)
			{
				uiComboBox.downDialgButtonEnable[WORLD_CHANNALES_TALK] = false;
				uiComboBox.UiComBoBoxAdjustDownDlg();
			}
			else
			if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chZhuhouName) == 0)
			{
				uiComboBox.downDialgButtonEnable[ZHUHOU_CHANNALES_TALK] = false;
				uiComboBox.UiComBoBoxAdjustDownDlg();
			}
			else
			if(strcmp(ChatString::ChatStringGetString().chMapName,channel.szChannelName) == 0)
			{
				uiComboBox.downDialgButtonEnable[MAP_CHANNALES_TALK] = false;
				uiComboBox.UiComBoBoxAdjustDownDlg();
			}
			else
			{
				continue;
			}
			chatChannels.erase(&chatChannels[j]);
			chatChannelsClose.erase(&chatChannelsClose[j]);
			return true;

		}
	}
	return false;
}
void ChatManager::ChatManagerClearInfoWnd()
{
	int size = chatPages.size();
	for(int i = 0; i < size; i ++)
	{
		ChatPage* pPage = chatPages[i];
		pPage->chatWndRender.ChatWndRenderClear();
		pPage->chatWndRender.numberUsed = 0;
		pPage->chatWndRender.allHeight = 0;
	}
}		
const char* ChatManager::ChatManagerGetChannelName(int channelID)
{

	return 0;
}
void ChatManager::ChatManageTextToLoelem(const char* text, char* segText, int chanId, bool bGM)
{
	if(NULL == text || NULL == segText)
	{
		return;
	}

	int textLen = strlen(text);

	if(text[0] != '<' || text[textLen - 1] != '>')
	{
		return;
	}

	std::vector<char> formatTexts[COMMON_CLIENT_MSG_LEN_256];
	int index = 0;
	for(int i = 0; i < textLen; ++i)
	{
		if(text[i] == '<')
		{
			continue;
		}
		else if(text[i] == '>')
		{
			formatTexts[index].push_back(0);
			index++;
			continue;
		}
		else if(text[i] == 1)
		{
			formatTexts[index].push_back('<');
		}
		else if(text[i] == 2)
		{
			formatTexts[index].push_back('>');
		}
		else
		{
			formatTexts[index].push_back(text[i]);
		}
	}

	char tempText[COMMON_CLIENT_MSG_LEN_32];
	char msgText[COMMON_CLIENT_MSG_LEN_512];
	msgText[0] = 0;
	for(int j = 0; j < index; ++j)
	{
		memset(msgText, 0, COMMON_CLIENT_MSG_LEN_512);

		char* curText = &formatTexts[j][0];
		if(formatTexts[j][0] == 'I')
		{
			int id[3] = {0, 0, 0};
			int retCode = sscanf(curText, "I=%d|%d|%d|%[^\0]", &id[0], &id[1], &id[2], msgText);
			if(retCode != 4)
			{
				continue;
			}
			strcat(segText, "<Obj type=text vertical-align=bottom ");
			strcat(segText, " show-des=true gotype=item des=");
			strcat(segText, msgText);
			strcat(segText, " goid=");
			sprintf(tempText, "%d", id[0]);
			strcat(segText, tempText);
			strcat(segText, " goid1=");
			sprintf(tempText, "%d", id[1]);
			strcat(segText, tempText);
			strcat(segText, " goid2=");
			sprintf(tempText, "%d", id[2]);
			strcat(segText, tempText);
			
			ItemType type;
			SpliteHashId(id[0], type.genre, type.detail, type.particular);
			type.level = id[1];
			int color = g_pCoreShell->GetGameData( GDI_GET_ITEM_QUALITY_BY_TYPE, (unsigned int)&type, NULL );

			strcat(segText, " color=");
			strcat(segText, KUiChanMgr::getSinglton().getItemColor(color));
			strcat(segText, " font-family=");
			strcat(segText, KUiChanMgr::getSinglton().getItemFont());
			strcat(segText, ">");
			strcat(segText, msgText);
			strcat(segText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'N')
		{
			int retCode = sscanf(curText, "N=%[^\0]", msgText);
			if(retCode != 1)
			{
				continue;
			}
			strcat(segText, "<Obj type=text vertical-align=bottom color=");

			MapChannelInfo channelInfo;
			if(KUiChanMgr::getSinglton().getChanInfo(chanId, channelInfo) && chanId == MAP_ROOM_ID)
			{
				ChangeTextColor(segText, channelInfo.szColor, chanId, bGM);
				strcat(segText, " font-family=");
				ChangeTextFont(segText, channelInfo.szFont, chanId, bGM);	
			}
			else
			{
				ChangeTextColor(segText, KUiChanMgr::getSinglton().getChanColor(chanId, bGM), chanId, bGM);
				strcat(segText, " font-family=");
				ChangeTextFont(segText, KUiChanMgr::getSinglton().getChanFont(chanId, bGM), chanId, bGM);
			}
			strcat(segText, ">");
			strcat(segText, msgText);
			strcat(segText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'F')
		{
			int id;
			int retCode = sscanf(curText, "F=%d", &id);
			if(retCode != 1)
			{
				continue;
			}
			sprintf(tempText, "%d", id);

			strcat(segText, "<Obj type=pic vertical-align=bottom color=");
			strcat(segText, KUiChanMgr::getSinglton().getChanColor(chanId, bGM));

			strcat(segText, " gotype=face goid=");
			
			strcat(segText, " color=");
			strcat(segText, KUiChanMgr::getSinglton().getChanColor(chanId, bGM));

			//根据goid得到内容和描述
			char imagePath[COMMON_CLIENT_MSG_LEN_64] = {0};
			char description[COMMON_CLIENT_MSG_LEN_32] = {0};
			const KUiCfgLoader::FacePanelCfgData& facePanelCfg  = KUiCfgLoader::getSingleton().getFaceData();
			for(int i = 0; i < facePanelCfg.faceList.size(); ++i)
			{
				if(facePanelCfg.faceList[i].index == id)
				{
					strcpy(imagePath, facePanelCfg.faceList[i].image);
					strcpy(description, facePanelCfg.faceList[i].description);
					break;
				}
			}
			strcat(segText, " des=");
			strcat(segText, description);
			strcat(segText, ">");
			strcat(segText, imagePath);
			strcat(segText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'P')
		{
			int id[4] = {0, 0, 0,0};
			int retCode = sscanf(curText, "P=%d|%d|%d|%d|%[^\0]", &id[0], &id[1], &id[2],&id[3], msgText);
			if(retCode != 5)
			{
				continue;
			}
			strcat(segText, "<Obj type=text vertical-align=bottom color=");
			strcat(segText, KUiChanMgr::getSinglton().getChanColor(chanId, bGM));

			strcat(segText, " show-des=true gotype=pos des=");
			strcat(segText, msgText);
			strcat(segText, " goid=");
			sprintf(tempText, "%d", id[0]);
			strcat(segText, tempText);
			strcat(segText, " goid1=");
			sprintf(tempText, "%d", id[1]);
			strcat(segText, tempText);
			strcat(segText, " goid2=");
			sprintf(tempText, "%d", id[2]);
			strcat(segText, tempText);

			strcat(segText, " goid3=");
			sprintf(tempText, "%d", id[3]);
			strcat(segText, tempText);

			
			strcat(segText, " color=");
			strcat(segText, KUiCfgLoader::getSingleton().getPosLinkCfg().color);
			strcat(segText, " font-family=");
			strcat(segText, KUiCfgLoader::getSingleton().getPosLinkCfg().font);
			strcat(segText, ">");
			strcat(segText, msgText);
			strcat(segText, "</Obj>");
		}
		else
		{
			continue;
		}
	}
}

void CALLBACK InfoWndUpdate(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime)
{
	if(hwnd == B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetHandle())
	{
		DWORD dt = currentTime - ChatManager::startUpdateTime;
		if(dt>=_CHAT_NEED_UPDATE_TIMER_DT*2)
		{
			ChatManager::startUpdateTime = 0;
			ChatManager::isCurrrent = FALSE;
/*			HDC hdc = GetWindowDC(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetHandle());
			HDC hOld = B2ChatDialog::pGDIRender->hCurrentDC;
			B2ChatDialog::pGDIRender->hCurrentDC = hdc;
			B2ChatDialog::pLayout->SetText(B2ChatDialog::chatManager.ChatManagerGetPage(B2ChatDialog::chatManager.currentPageIndex)->ChatPageGetContainer()->CharContainerGetTextBuffer());
//			B2ChatDialog::pLayout->flashLayout();
			B2ChatDialog::pGDIRender->hCurrentDC = hOld;
			ReleaseDC(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetHandle(),hdc);
			int dy = B2ChatDialog::pLayout->getRenderArea().getHeight() + 
				B2ChatDialog::chatManager.ChatManagerGetPage(B2ChatDialog::chatManager.currentPageIndex)->ChatPageGetDrawPoint().y 
				- B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom;

			if(dy>0)
			{
				if(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndIsScroll())
					B2ChatDialog::chatManager.ChatManagerGetPage(B2ChatDialog::chatManager.currentPageIndex)->ChatPageGetDrawPoint().y-=dy;
				else
					B2ChatDialog::chatManager.normalButton[_DOWN_BUTTON_INDEX].ChatWndSetTimer();
			}*/
			B2ChatDialog::chatManager.ChatManagerGetPage(B2ChatDialog::chatManager.currentPageIndex)->chatWndRender.ChatWndRenderAutoScroll(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom - B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY);
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndUpdate();
			B2ChatDialog::chatManager.RecalculateScroll();
			KillTimer(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID);
		}
	}
}
void ChatManager::ChatManagerReceiveCustomMsg(int channelID,char* msg)
{
	int channelSize = chatChannels.size();
	for(int k = 0; k < channelSize; k ++)
	{
		Ui_Channel_Param& channel = chatChannels[k];
		if(channel.dwChannelID == channelID)
		{
			if(chatChannelsClose[k] == true)
				return ;
			break;
		}
	}
	int numberPage = chatPages.size();
	bool update = false;
	for(int i = 0;i<numberPage;i++)
	{
		int numberChannel = chatPages[i]->ChatPageGetChannelNumbers();
		for(int j = 0; j < numberChannel; j++)
		{
			if(channelID == chatPages[i]->ChatPageGetChannel(j).dwChannelID)
			{
				chatPages[i]->chatWndRender.ChatWndRenderPushBack(msg);
				if(currentPageIndex == chatPages[i]->ChatPageGetID())
				{
					if(isCurrrent == false)
					{
						isCurrrent = TRUE;
						startUpdateTime = GetTickCount();
						SetTimer(infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
					}
					else
						return;


				}
				else
				if(chatPages[i]->IsHavePersonalChannel()&&channelID == COSE_ROOM_ID)
					chatPages[i]->ChatPageGetButton().ChatWndSetTimer();
			}
		}
	}
}
void   ChatManager::ChatClientInsertPlayerInfoMsg(const char* pBuffer)
{
	if(pBuffer == 0)
		return;
	int len = strlen(pBuffer);
	if(len == 0)
		return;
	int channelsize = chatChannels.size();
	int channelID = -100;
	for(int chId = 0; chId < channelsize; chId ++)
	{
		Ui_Channel_Param& channel = chatChannels[chId];
		if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chSystemName) == 0)
		{
			channelID = channel.dwChannelID;
			break;
		}
	}
	if(channelID ==-100)
		return;
	char* segText = strMsg;
	segText[0] = 0;
	char tempText[256] = {0};
	strcat(segText, "<Seg float=wrap>");

	strcat(segText,"<Obj type=text vertical-align=bottom color=");
	strcat(segText, KUiChanMgr::getSinglton().getChanColor(channelID, false));
	strcat(segText,">");
	strcat(segText,pBuffer);
	strcat(segText,"</Obj>");
	strcat(segText,"</Seg>");
	int pageSize = chatPages.size();
	bool update = false;
	for(int pageIndex = 0;pageIndex < pageSize;pageIndex++)
	{
		int channelSize = chatPages[pageIndex]->ChatPageGetChannelNumbers();
		for(int channelIndex = 0; channelIndex < channelSize; channelIndex++)
		{
			if(chatPages[pageIndex]->ChatPageGetChannel(channelIndex).dwChannelID == channelID)
			{
				ChatPage* pPage = chatPages[pageIndex];
				pPage->chatWndRender.ChatWndRenderPushBack(segText);
				if(currentPageIndex == pageIndex)
				{
					if(isCurrrent==false)
					{
						isCurrrent = TRUE;
						startUpdateTime = GetTickCount();
						SetTimer(infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
					}
				}
				else
					pPage->ChatPageGetButton().ChatWndSetTimer();
			}
		}
	}

}
void    ChatManager::ChatClientInsertSystemMsg(const char* pBuffer,bool isImage /* = false */)
{
	/*"<Seg text-align=left >"
//  					"<Obj type=text color=255,34,56 font-family=LiBian-16>abcdefg\n谢鉷hijkl\nmnopqr\nstuv\nwxyz</Obj>"
//  					"<Obj type=text font-family=SongTi-10 color=255,255,56>abcdef\ng谢鉷hij\nklmnopq\nrstuvw\nxyz</Obj>"
//  					"</Seg>"
//  					"<Seg text-align=center float=none>"
//  					"<Obj type=pic>set:bagua image:bagua1_normal</Obj>"*/
	if(pBuffer == 0)
		return;
	int len = strlen(pBuffer);
	if(len == 0)
		return;
	int channelsize = chatChannels.size();
	int channelID = -100;
	for(int chId = 0; chId < channelsize; chId ++)
	{
		Ui_Channel_Param& channel = chatChannels[chId];
		if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chSystemName) == 0)
		{
			channelID = channel.dwChannelID;
			break;
		}
	}
	if(channelID ==-100)
		return;
	char* segText = strMsg;
	segText[0] = 0;
	char tempText[256] = {0};
	strcat(segText, "<Seg float=wrap>");

	strcat(segText, "<Obj type=text vertical-align=bottom show-des=true color=");
	strcat(segText, KUiChanMgr::getSinglton().getChanColor(channelID,false));
	strcat(segText, " font-family=");
	strcat(segText, KUiChanMgr::getSinglton().getChanFont(channelID, false));
	strcat(segText, " gotype=chan goid=");
	sprintf(tempText, "%d", channelID);
	strcat(segText, tempText);
	strcat(segText, " des=[");
	strcat(segText, ChatString::ChatStringGetString().chSystemName);
	strcat(segText, "] ></Obj>");

	if(isImage)
	{
		strcat(segText,"<Obj type=pic>");
		strcat(segText,pBuffer);
		strcat(segText,"</Obj>");
	}
	else
	{
		strcat(segText,"<Obj type=text vertical-align=bottom color=");
		strcat(segText, KUiChanMgr::getSinglton().getChanColor(channelID, false));
		strcat(segText,">");
		strcat(segText,pBuffer);
		strcat(segText,"</Obj>");
	}
	strcat(segText,"</Seg>");
	int pageSize = chatPages.size();
	bool update = false;
	for(int pageIndex = 0;pageIndex < pageSize;pageIndex++)
	{
		int channelSize = chatPages[pageIndex]->ChatPageGetChannelNumbers();
		for(int channelIndex = 0; channelIndex < channelSize; channelIndex++)
		{
			if(chatPages[pageIndex]->ChatPageGetChannel(channelIndex).dwChannelID == channelID)
			{
				ChatPage* pPage = chatPages[pageIndex];
				pPage->chatWndRender.ChatWndRenderPushBack(segText);
				if(currentPageIndex == pageIndex)
				{
					if(isCurrrent==false)
					{
						isCurrrent = TRUE;
						startUpdateTime = GetTickCount();
						SetTimer(infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
					}
				}
				else
					pPage->ChatPageGetButton().ChatWndSetTimer();
			}
		}
	}
}

void ChatManager::RecalculateScroll()
{
	ChatPage * pPage = ChatManagerGetPage(currentPageIndex);
	if (pPage == NULL)
	{
		return;
	}

	int scrollLength	 = m_ScrollBar.ChatScrollBarGetScrollLength();
	int barRectHeight	 = m_ScrollBar.GetBarRectHeight();

	int renderHeight = pPage->chatWndRender.allHeight - (infoWnd.renderRect.bottom - infoWnd.renderY);
	if (renderHeight < 0)
	{
		renderHeight = barRectHeight;
	}

	m_ScrollBar.ChatScrollBarSetScrolls(renderHeight, m_ScrollBar.GetBarRectHeight());

	int renderPosY = -(pPage->chatWndRender.infoWndLayOut[0].pt.y - infoWnd.renderY);
	int lastLinePosY = 0;
	if (pPage->chatWndRender.numberUsed > 0)
	{
		lastLinePosY = pPage->chatWndRender.infoWndLayOut[pPage->chatWndRender.numberUsed - 1].pt.y;
	}
	else
	{
		lastLinePosY = infoWnd.renderY;
	}

	int rectPos = 0;
	int layoutRenderHeight = 0;
	if (pPage->chatWndRender.numberUsed > 0)
	{
		layoutRenderHeight = pPage->chatWndRender.infoWndLayOut[pPage->chatWndRender.numberUsed - 1].renderHeight;
	}
	else
	{
		layoutRenderHeight = 0;
	}
	if ((lastLinePosY + layoutRenderHeight) < (infoWnd.renderRect.bottom - infoWnd.renderY))
	{
		rectPos = scrollLength - barRectHeight;
	}
	else
	{
		rectPos = (int)((float)renderPosY * (float)(scrollLength - barRectHeight) / (float)renderHeight);
	}
	if (rectPos < 0)
	{
		rectPos = 0;
	}
	else if (rectPos > (scrollLength - barRectHeight))
	{
		rectPos = scrollLength - barRectHeight;
	}

	int oneLineLength = m_ScrollBar.GetOneLineLength();
	if (rectPos > (scrollLength - barRectHeight - oneLineLength))
	{
		infoWnd.ChatInfoWndEnableScroll();
	}

	m_ScrollBar.SetBarRectPos(rectPos);
	m_ScrollBar.ChatScrolBarUpdate();
}

void ChatManager::ChatClientInsertSystemMsg(ILayout* pLayout)
{
	int channelsize = chatChannels.size();
	int channelID = -100;
	for(int chId = 0; chId < channelsize; chId ++)
	{
		Ui_Channel_Param& channel = chatChannels[chId];
		if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chSystemName) == 0)
		{
			channelID = channel.dwChannelID;
			break;
		}
	}
	if(channelID ==-100)
		return;
	char* segText = strMsg;
	segText[0] = 0;
	char tempText[256] = {0};
	DWORD color = ChatString::ChatStringGetString().pathHelpTextColor;
	sprintf(segText, "<Seg float=wrap color=%d,%d,%d><Obj>%s</Obj></Seg>",color>>16,(color>>8)&0xff,(color&0xff),ChatString::ChatStringGetString().pathHelpText);

	int pageSize = chatPages.size();
	bool update = false;
	for(int pageIndex = 0;pageIndex < pageSize;pageIndex++)
	{
		int channelSize = chatPages[pageIndex]->ChatPageGetChannelNumbers();
		for(int channelIndex = 0; channelIndex < channelSize; channelIndex++)
		{
			if(chatPages[pageIndex]->ChatPageGetChannel(channelIndex).dwChannelID == channelID)
			{
				ChatPage* pPage = chatPages[pageIndex];
				pPage->chatWndRender.ChatWndRenderCopyLayoutText(pLayout,segText);
				if(currentPageIndex == pageIndex)
				{
					if(isCurrrent==false)
					{
						isCurrrent = TRUE;
						startUpdateTime = GetTickCount();
						SetTimer(infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
					}
				}
				else
					pPage->ChatPageGetButton().ChatWndSetTimer();
			}
		}
	}
}
void ChatManager::ChatManagerReceiveCozeMsg(BYTE* byBuffer)
{

	PCHATMSG_BY_NAME cozeMsg = (PCHATMSG_BY_NAME)byBuffer;
	
	char tempText[COMMON_CLIENT_MSG_LEN_32];
	
	char* segText = strMsg;
	
	segText[0] = 0;
	strcat(segText, "<Seg float=wrap>");
	//名称
	if(cozeMsg->name != NULL && cozeMsg->name[0] != 0)
	{
		strcat(segText, "<Obj type=text vertical-align=bottom show-des=true color=");
		strcat(segText, KUiChanMgr::getSinglton().getChanColor(COSE_ROOM_ID, false));
		strcat(segText, " font-family=");
		strcat(segText, KUiChanMgr::getSinglton().getPlayerNameFont());
		strcat(segText, " gotype=player goid=");
		sprintf(tempText, "%d", cozeMsg->npcId);
		strcat(segText, tempText);
		strcat(segText, " des=");
		
		if(cozeMsg->isRecive)
		{
			strcat(segText, "[");
			strcat(segText, cozeMsg->name);
			char buffer[256]={"]"};
			strcat(buffer,ChatString::ChatStringGetString().personalShowIn);
			strcat(buffer,":>");
			strcat(segText, buffer);
			KUiChatInputWnd::GetSingleton().setLastSender(cozeMsg->name);
			playSound(KUiCfgLoader::getSingleton().getSoundEffectCfg().recvNewCoze);
		}
		else
		{
			char buffer[256]={0};
			strcpy(buffer,ChatString::ChatStringGetString().personalShowOut);
			strcat(buffer,"[");
			strcat(segText, buffer);
			strcat(segText, cozeMsg->name);
			strcat(segText, "]:>");
			KUiChatInputWnd::GetSingleton().setLastReciver(cozeMsg->name);
		}
		strcat(segText, cozeMsg->name);
		strcat(segText, "</Obj>");
	}
	ChatManager::ChatManageTextToLoelem((const char*)&cozeMsg->msg, segText, COSE_ROOM_ID, false);
	
	strcat(segText, "</Seg>");

	int pageSize = chatPages.size();
	bool update = false;
	int channelID = 0;
	for(int i = 0 ;i <chatChannels.size();i++)
	{
		if(strcmp(chatChannels[i].szChannelName,ChatString::ChatStringGetString().chPersonalName)==0)
		{
			if(chatChannelsClose[i] == true)
				return;
			channelID = chatChannels[i].dwChannelID;
			break;
		}
	}
	for(int pageIndex = 0;pageIndex < pageSize;pageIndex++)
	{
		int channelSize = chatPages[pageIndex]->ChatPageGetChannelNumbers();
		for(int channelIndex = 0; channelIndex < channelSize; channelIndex++)
		{
			if(chatPages[pageIndex]->ChatPageGetChannel(channelIndex).dwChannelID == channelID)
			{
				ChatPage* pPage = chatPages[pageIndex];
				pPage->chatWndRender.ChatWndRenderPushBack(segText);
				if(currentPageIndex == pPage->ChatPageGetID())
				{
					if(isCurrrent==false)
					{
						isCurrrent = TRUE;
						startUpdateTime = GetTickCount();
						SetTimer(infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
					}
				}
				else
				if(pPage->IsHavePersonalChannel()&&channelID == COSE_ROOM_ID)
					pPage->ChatPageGetButton().ChatWndSetTimer();
			}
		}
	}
}
void ChatManager::ChatManagerReceiveChatMsg(int channelID ,BYTE* msg)
{
	int channelSize = chatChannels.size();
	Ui_Channel_Param* pChannel = NULL;
	for(int i = 0; i < channelSize; i ++)
	{
		Ui_Channel_Param& channel = chatChannels[i];
		if(channel.dwChannelID == channelID)
		{
			if(chatChannelsClose[i] == true)
				return ;
			pChannel = &chatChannels[i];
			break;
		}
	}
	PCHATROOMMSG_TO_SOMEONE pChatMsg = (PCHATROOMMSG_TO_SOMEONE)msg;
	if(LOCAL_ROOM_ID == channelID)
	{
		KUiBubbleManager::getSington().showBubble((char*)&pChatMsg->msg, _Bubble::NpcHeadPop ,pChatMsg->senderPlayerId, pChatMsg->gm);
	}
	char tempText[COMMON_CLIENT_MSG_LEN_32]={0};
	char* pSegText = strMsg;
	pSegText[0] = 0;
	strcat(pSegText,"<Seg float=wrap>");

	char chanName[COMMON_CLIENT_MSG_LEN_32];
	memset(chanName, 0, sizeof(chanName));
	KUiChanMgr::getSinglton().getChanNameById(channelID, chanName, sizeof(chanName));//ChatManagerGetChannelName(channelID);
	if(chanName!=0&&chanName[0] !=0)
	{
		strcat(pSegText, "<Obj type=text vertical-align=bottom show-des=true color=");

		MapChannelInfo mapInfo;
		if (channelID == MAP_ROOM_ID && KUiChanMgr::getSinglton().getChanInfo(MAP_ROOM_ID, mapInfo))
		{
			ChangeTextColor(pSegText, mapInfo.szColor, channelID, pChatMsg->gm);
			strcat(pSegText, " font-family=");
			ChangeTextFont(pSegText, mapInfo.szFont, channelID, pChatMsg->gm);
		}
		else
		{
			ChangeTextColor(pSegText, KUiChanMgr::getSinglton().getChanColor(channelID, pChatMsg->gm ), channelID, pChatMsg->gm);
			strcat(pSegText, " font-family=");
			ChangeTextFont(pSegText, KUiChanMgr::getSinglton().getChanFont(channelID, pChatMsg->gm), channelID, pChatMsg->gm);
		}
		strcat(pSegText, " gotype=chan goid=");
		sprintf(tempText, "%d", channelID);
		strcat(pSegText, tempText);
		strcat(pSegText, " des=[");
		strcat(pSegText, chanName);
		strcat(pSegText, "]></Obj>");
	}
	if( pChannel && (strcmp(pChannel->szChannelName, ChatString::ChatStringGetString().chZhuhouName) == 0
		|| strcmp(pChannel->szChannelName, ChatString::ChatStringGetString().chSizhuName) == 0
		|| strcmp(pChannel->szChannelName, ChatString::ChatStringGetString().chGuojiaName) == 0))
	{
		if(enSULayer_Gens == pChatMsg->unitRank)
		{
			strcat(pSegText, "<Obj t=text v-a=bottom color=");
			strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().zhuzhangColor);
			strcat(pSegText, " f-f=");
			strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().zhuzhangFont);
			strcat(pSegText, ">[");
			strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().zhuzhang);
			strcat(pSegText, "]</Obj>");
		}
		else if(enSULayer_Tong == pChatMsg->unitRank)
		{
			strcat(pSegText, "<Obj t=text v-a=bottom color=");
			strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().houzhuColor);
			strcat(pSegText, " f-f=");
			strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().houzhuFont);
			strcat(pSegText, ">[");
			strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().houzhu);
			strcat(pSegText, "]</Obj>");
		}
		else if(enSULayer_League == pChatMsg->unitRank)
		{
			strcat(pSegText, "<Obj t=text v-a=bottom color=");
			strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().leagueColor);
			strcat(pSegText, " f-f=");
			strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().leagueFont);
			strcat(pSegText, ">[");
			strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().leagueLeader);
			strcat(pSegText, "]</Obj>");
		}
	}
	if(pChatMsg->senderName != NULL && pChatMsg->senderName[0] != 0)
	{
		strcat(pSegText, "<Obj type=text vertical-align=bottom show-des=true color=");
		if ( pChatMsg->gm == 0 )
		{
			strcat(pSegText, KUiChanMgr::getSinglton().getPlayerNameColor());
		}
		else
		{
			strcat(pSegText, KUiChanMgr::getSinglton().getChanNameColor(SYSTEM_ROOM_ID, false));
		}
		
		strcat(pSegText, " font-family=");
		if ( pChatMsg->gm == 0 )
		{
			strcat(pSegText, KUiChanMgr::getSinglton().getPlayerNameFont());
		}
		else
		{
			strcat(pSegText, KUiChanMgr::getSinglton().getChanNameFont(SYSTEM_ROOM_ID, false));
		}
		
		strcat(pSegText, " gotype=player goid=");
		sprintf(tempText, "%d", pChatMsg->senderPlayerId);
		strcat(pSegText, tempText);
		strcat(pSegText, " des=[");
		strcat(pSegText, pChatMsg->senderName);
		strcat(pSegText, "]:>");
		strcat(pSegText, pChatMsg->senderName);
		strcat(pSegText, "</Obj>");

		if(SYSTEM_ROOM_ID != channelID  && pChatMsg->gm == 0 )
		{
			if(KUiCfgLoader::getSingleton().getChannelData().personalText[0] != 0 && pChatMsg->senderPlayerId != INVALID_PLAYER_INDEX)
			{
				strcat(pSegText, "<Obj t=text v-a=bottom s-d=true color=");
				strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().personalTextColor);
				strcat(pSegText, " f-f=");
				strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().personalTextFont);
				strcat(pSegText, " d=");
				strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().personalText);
				strcat(pSegText, "></Obj>");
			}
		}
		else
		{
			if(KUiCfgLoader::getSingleton().getChannelData().officialText[0] != 0)
			{
				strcat(pSegText, "<Obj t=text v-a=bottom s-d=true color=");
				strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().officialTextColor);
				strcat(pSegText, " f-f=");
				strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().officialTextFont);
				strcat(pSegText, " d=");
				strcat(pSegText, KUiCfgLoader::getSingleton().getChannelData().officialText);
				strcat(pSegText, "></Obj>");
			}
		}
	}

	if(pChatMsg->msg[0] == '<')
	{
		ChatManageTextToLoelem((const char*)&pChatMsg->msg, pSegText, channelID, pChatMsg->gm);
	}
	else	//系统消息可能不带标签
	{
		static char sysMsg[COMMON_CLIENT_MSG_LEN_1024] = {0};
		sprintf(sysMsg, "<N=%s>", (const char*)&pChatMsg->msg);
		ChatManageTextToLoelem(sysMsg, pSegText, channelID, pChatMsg->gm);
	}
	strcat(pSegText,"</Seg>");
	pSegText[LAYOUT_TEXT_MAX_LEN] = 0;
	int pageSize = chatPages.size();
	bool update = false;
	for(int pageIndex = 0;pageIndex < pageSize;pageIndex++)
	{
		int channelSize = chatPages[pageIndex]->ChatPageGetChannelNumbers();
		for(int channelIndex = 0; channelIndex < channelSize; channelIndex++)
		{
			if(chatPages[pageIndex]->ChatPageGetChannel(channelIndex).dwChannelID == channelID)
			{
				ChatPage* pPage = chatPages[pageIndex];
//				pPage->ChatPageInsertSeg(pSegText);
				pPage->chatWndRender.ChatWndRenderPushBack(pSegText);
				if(currentPageIndex == pPage->ChatPageGetID())
				{
					if(isCurrrent==false)
					{
						isCurrrent = TRUE;
						startUpdateTime = GetTickCount();
						SetTimer(infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
					}
					else
						return;
				}
				else
				if(pPage->IsHavePersonalChannel()&&channelID == COSE_ROOM_ID)
      				pPage->ChatPageGetButton().ChatWndSetTimer();

			}
		}
	}
}
void ChatManager::ChatManagerRegistChannel(Ui_Channel_Param& channel)
{
	const KUiCfgLoader::ChannelCfgData& chanCfg = KUiCfgLoader::getSingleton().getChannelData();
	for(int i = 0; i < chatChannels.size();i++)
	{
		if(channel.dwChannelID == chatChannels[i].dwChannelID)
			return;
	}
	chatChannels.push_back(channel);
	chatChannelsClose.push_back(false);
	if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chSizhuName) ==0)
	{
		uiComboBox.downDialgButtonEnable[SHIZU_CHANNALES_TALK]=true;
		uiComboBox.UiComBoBoxAdjustDownDlg();
	}
	else
	if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chTeamName)==0)
	{
		uiComboBox.downDialgButtonEnable[TEAM_CHANNALES_TALK] = true;
		uiComboBox.UiComBoBoxAdjustDownDlg();
	}
	else
	if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chGuojiaName)==0)
	{
		uiComboBox.downDialgButtonEnable[GUOJIA_CHANNALES_TALK]=true;
		uiComboBox.UiComBoBoxAdjustDownDlg();
	}
	else
	if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chZhuhouName) ==0)
	{
		uiComboBox.downDialgButtonEnable[ZHUHOU_CHANNALES_TALK]=true;
		uiComboBox.UiComBoBoxAdjustDownDlg();
	}
	else
	if(strcmp(channel.szChannelName,ChatString::ChatStringGetString().chMapName) == 0)
	{
		uiComboBox.downDialgButtonEnable[MAP_CHANNALES_TALK]=true;
		uiComboBox.UiComBoBoxAdjustDownDlg();
	}


	//查找到定制了该频道的页面，然后向找到的页面注册该频道
	for(int j = 0; j < chanCfg.framesCfg.size(); ++j)
	{
		const KUiCfgLoader::ChatFrameCfg& frameCfg = chanCfg.framesCfg[j];
		for(int k = 0; k < frameCfg.channelName.size(); ++k)
		{
			const string& chanRequired = frameCfg.channelName[k];
			if(chanRequired == string(channel.szChannelName))
			{
				chatPages[j]->ChatPageRegistChannel(channel);
				break;
			}
		}
	}
	if(channel.dwChannelID == SYSTEM_ROOM_ID)
	{
		chatPages[_PAGE_ID_SYSTEM]->ChatPageRegistChannel(channel);	
	}

}
BOOL ChatManager::ChatManagerCreateWnd(HWND hwnd)
{
#define _CHAT_BUTTON_TEXT    "textInfo"
	KIniFile iniFile;
	B2ChatDialog::m_hDialog = hwnd;
	TCHAR  szPath[MAX_PATH],szValue[MAX_PATH];
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);

	iniFile.Load(szPath);

	TCHAR* szNormalButtonItem[] ={"TOP","UP","DOWN","END","closeWnd","close"};
	int x,y,width,height;
	BOOL isClip=0;
	int r=0,g=0,b=0;
	int id = _CHAT_BUTTON_ID_TOP;
	for(int i = 0;i<_CHAT_NORMAL_BUTTON_NUMBER;i++)
	{
		iniFile.GetInteger(szNormalButtonItem[i],_CHAT_SRC_X,0,&x);
		iniFile.GetInteger(szNormalButtonItem[i],_CHAT_SRC_Y,0,&y);
		iniFile.GetInteger(szNormalButtonItem[i],_CHAT_SRC_WIDTH,0,&width);
		iniFile.GetInteger(szNormalButtonItem[i],_CHAT_SRC_HEIGHT,0,&height);
		iniFile.GetInteger(szNormalButtonItem[i],_CHAT_SRC_CLIP,0,&isClip);
		normalButton[i].ChatWndCreate(id+i,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY ,hwnd,"","button",x,y,width,height);
		iniFile.GetString(szNormalButtonItem[i],"tooltipInfo","",szValue,MAX_PATH);
	    if(szValue[0] != 0)
		   normalButton[i].ChatWndTipCreate(TTS_NOPREFIX,szValue,100);
		SetClassLong(normalButton[i].ChatWndGetHandle(),GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		int normal_idx = 0,hover_idx=0,pushed_idx=0;
		iniFile.GetInteger(szNormalButtonItem[i],"normalSrcIdx",0,&normal_idx);
		iniFile.GetInteger(szNormalButtonItem[i],"mouseOverSrcIdx",0,&hover_idx);
		iniFile.GetInteger(szNormalButtonItem[i],"mouseDownSrcIdx",0,&pushed_idx);
		normalButton[i].ChatWndSetResource(normal_idx,hover_idx,pushed_idx,-1);
		normalButton[i].ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		if(isClip)
		{
		}
		char infoText[256] = {0};
		iniFile.GetString(szNormalButtonItem[i],_CHAT_BUTTON_TEXT,"",infoText,256);
		if(infoText[0]!=0)
		{
			normalButton[i].ChatWndSetText(infoText);
			normalButton[i].ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
			iniFile.GetString(szNormalButtonItem[i],"fontColor","",szValue,MAX_PATH);
			sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			normalButton[i].ChatWndSetTextNormalColor(RGB(r,g,b));
			char font[FONT_SIZE]={0};
			iniFile.GetString(szNormalButtonItem[i],"font","",font,FONT_SIZE);
			TFONT tFont;
			strcpy(tFont.fontName,font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0]=0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
				normalButton[i].ChatWndSetFont(font);
			else
			{
				normalButton[i].ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				normalButton[i].ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
	}
	if(!normalButton[_CLOSEWND_BUTTON_INDEX].ChatWndTipEnable())
	{
		normalButton[_CLOSEWND_BUTTON_INDEX].ChatWndTipCreate(TTS_NOPREFIX,ChatString::ChatStringGetString().hideGameWnd,100);
	}
	
	//////////////////////////
	normalButton[_CLOSE_BUTTON_INDEX].SetWndProcessFun(ChatWndProcessFun::ProcessChangeChatButton);
	normalButton[_CLOSEWND_BUTTON_INDEX].SetWndProcessFun(ChatWndProcessFun::ProcessHideGameWndButton);
	normalButton[_UP_BUTTON_INDEX].SetWndProcessFun(ChatWndProcessFun::ProcessChatInfoUpButton);
	normalButton[_DOWN_BUTTON_INDEX].SetWndProcessFun(ChatWndProcessFun::ProcessChatInfoDownButton);
	normalButton[_END_BUTTON_INDEX].SetWndProcessFun(ChatWndProcessFun::ProcessChatInfoEndButton);
	normalButton[_TOP_BUTTON_INDEX].SetWndProcessFun(ChatWndProcessFun::ProcessChatInfoTopButton);
////////////////////////////////////////////////////

	iniFile.GetInteger("PAGESTATIC",_CHAT_SRC_X,0,&x);
	iniFile.GetInteger("PAGESTATIC",_CHAT_SRC_Y,0,&y);
	iniFile.GetInteger("PAGESTATIC",_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger("PAGESTATIC",_CHAT_SRC_HEIGHT,0,&height);	
	infoWnd.ChatWndCreate(_CHAT_STATIC_PAGE_ID,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|SS_OWNERDRAW|SS_NOTIFY ,hwnd,"","static",x,y,width,height);
	SetClassLong(infoWnd.ChatWndGetHandle(),GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
	
	int normal_idx = 0,hover_idx = 0,pushed_idx = 0;
	iniFile.GetInteger("PAGESTATIC","srcIdx",0,&normal_idx);
	infoWnd.ChatWndSetResource(normal_idx,normal_idx,normal_idx,normal_idx);
	iniFile.GetInteger("PAGESTATIC","renderX",0,&infoWnd.renderX);
	iniFile.GetInteger("PAGESTATIC","renderY",0,&infoWnd.renderY);
	infoWnd.renderRect.left = infoWnd.renderX;
	infoWnd.renderRect.right = infoWnd.ChatWndGetRect().right -infoWnd.renderX;
	infoWnd.renderRect.top = infoWnd.renderY;
	infoWnd.renderRect.bottom = infoWnd.ChatWndGetRect().bottom - infoWnd.renderY;
	infoWnd.SetWndProcessFun(ChatWndProcessFun::ProcessChatInfoWnd);
//	B2ChatDialog::wndInfoProc = (WNDPROC)SetWindowLong(infoWnd.ChatWndGetHandle(),GWL_WNDPROC,(LONG)(ChatInfoWnd::ChatInfoWndProc));
	int size = KUiCfgLoader::getSingleton().getChannelData().framesCfg.size();
	for(int j = 0; j<size;j++)
	{
		ChatPage* pPage = new ChatPage;
		pPage->ChatPageCreatePageButton(hwnd,KUiCfgLoader::getSingleton().getChannelData().framesCfg[j].frameName.c_str());
		pPage->ChatPageSetShowWnd(&infoWnd);
		pPage->chatWndRender.ChatWndRenderCreate();
		pPage->chatWndRender.hWnd = infoWnd.ChatWndGetHandle();
		pPage->chatWndRender.width = infoWnd.renderRect.right - infoWnd.renderRect.left;
		chatPages.push_back(pPage);
	}
	Ui_Channel_Param channel;
	strcpy(channel.szChannelName,ChatString::ChatStringGetString().chPersonalName);
	channel.dwChannelID = COSE_ROOM_ID;
	ChatManagerRegistChannel(channel);
	
	channel.dwChannelID = COMBAT_INFO_ROOM_ID;
	sprintf(channel.szChannelName, ChatString::ChatStringGetString().chFightName);
	ChatManagerRegistChannel(channel);
	
	
	chatPages[0]->ChatPageGetButton().ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);


	int fontHeight = 0;
	iniFile.GetInteger("PAGESTATIC","fontHeight",0,&fontHeight);
//	fontHeight = B2ChatDialog::pGDIRender->fontHeight;
	size = chatPages.size();
	for(i = 0 ; i < size; i++)
	{
		ChatPage* pPage = chatPages[i];
		for(int j = 0; j < MAX_CHAT_INFO_SEG; j++)
		{
			((GDIRender*)(pPage->chatWndRender.infoWndLayOut[j].pRender))->fontHeight = fontHeight;
		}
	}

	char font[FONT_SIZE]={0};
    iniFile.GetString("PAGESTATIC","font","",font,FONT_SIZE);
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
//		strcpy(B2ChatDialog::pGDIRender->font,font);
		size = chatPages.size();
		for(i = 0 ; i < size; i++)
		{
			ChatPage* pPage = chatPages[i];
			for(int j = 0; j < MAX_CHAT_INFO_SEG; j++)
			{
				strcpy(((GDIRender*)(pPage->chatWndRender.infoWndLayOut[j].pRender))->font,font);
				((GDIRender*)(pPage->chatWndRender.infoWndLayOut[j].pRender))->isUseDefualtFont = false;
			}
		}
	}
	else
	{
//		strcpy(B2ChatDialog::pGDIRender->font,ChatString::ChatStringGetString().chatDefualtFont);

		size = chatPages.size();
		for(i = 0 ; i < size; i++)
		{
			ChatPage* pPage = chatPages[i];
			for(int j = 0; j < MAX_CHAT_INFO_SEG; j++)
			{
				strcpy(((GDIRender*)(pPage->chatWndRender.infoWndLayOut[j].pRender))->font,ChatString::ChatStringGetString().chatDefualtFont);
				((GDIRender*)(pPage->chatWndRender.infoWndLayOut[j].pRender))->isUseDefualtFont = true;
			}
		}
	}

	iniFile.GetInteger("face",_CHAT_SRC_X,0,&x);
	iniFile.GetInteger("face",_CHAT_SRC_Y,0,&y);
	iniFile.GetInteger("face",_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger("face",_CHAT_SRC_HEIGHT,0,&height);

	iniFile.GetInteger("face",_CHAT_SRC_CLIP,0,&isClip);
	if(isClip==1)
	{
//		iniFile.GetInteger("face",_CHAT_SRC_CLIP_COLOR_R,0,&r);
//		iniFile.GetInteger("face",_CHAT_SRC_CLIP_COLOR_G,0,&g);
//		iniFile.GetInteger("face",_CHAT_SRC_CLIP_COLOR_B,0,&b);

	}
	showButton.ChatWndCreate(_CHAT_SHOWFACE_BUTTON,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY ,hwnd,"","button",x,y,width,height);
	
	iniFile.GetString("face","tooltipInfo","",szValue,MAX_PATH);
	  if(szValue[0] != 0)
		   showButton.ChatWndTipCreate(TTS_NOPREFIX,szValue,100);
	SetClassLong(showButton.ChatWndGetHandle(),GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
//	int normal_idx = 0,hover_idx=0,pushed_idx=0;
	iniFile.GetInteger("face","normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger("face","mouseOverSrcIdx",0,&hover_idx);
	iniFile.GetInteger("face","mouseDownSrcIdx",0,&pushed_idx);
	showButton.ChatWndSetResource(normal_idx,hover_idx,pushed_idx,-1);
	showButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
	showButton.SetWndProcessFun(ChatWndProcessFun::ProcessShowFaceButton);
	if(isClip)
	{
//		ChatWnd::BitmapToRgn(pSrc1->hNormalSrc,normalButton[i].ChatWndGetRgn(),RGB(r,g,b));
//		SetWindowRgn(showButton.ChatWndGetHandle(),showButton.ChatWndGetRgn(),TRUE);
	}

	iniFile.GetInteger("edit",_CHAT_SRC_X,0,&x);
	iniFile.GetInteger("edit",_CHAT_SRC_Y,0,&y);
	iniFile.GetInteger("edit",_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger("edit",_CHAT_SRC_HEIGHT,0,&height);
	iniFile.GetInteger("edit","dx",0,&edit.dx);
	edit.pt.x = edit.dx;

	edit.ChatWndCreate(_CHAT_EDIT_CHAT_ID,WS_CHILD|WS_VISIBLE|ES_READONLY,hwnd,"","edit",x,y,width,height);
	
	iniFile.GetInteger("edit","srcIdx",0,&normal_idx);
	edit.ChatWndSetResource(normal_idx,normal_idx,normal_idx,normal_idx);
	iniFile.GetInteger("edit","fontHeight",0.0f,&((GDIRender*)B2ChatDialog::pEditRender)->fontHeight);

	char font1[FONT_SIZE]={0};
    iniFile.GetString("edit","font","",font1,FONT_SIZE);
	TFONT tFont1;
	strcpy(tFont1.fontName,font1);
	HDC hdc1 =CreateCompatibleDC(NULL);
	LOGFONT logFont1;
	logFont1.lfFaceName[0]=0;
	logFont1.lfCharSet = DEFAULT_CHARSET;
	EnumFontFamiliesEx(hdc1,&logFont1,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont1,0);
	DeleteDC(hdc1);
	if(tFont1.isInSystem)
	{
		strcpy(((GDIRender*)B2ChatDialog::pEditRender)->font,font1);
		((GDIRender*)B2ChatDialog::pEditRender)->isUseDefualtFont = false;
	}
	else
	{
		strcpy(((GDIRender*)B2ChatDialog::pEditRender)->font,ChatString::ChatStringGetString().chatDefualtFont);
		((GDIRender*)B2ChatDialog::pEditRender)->isUseDefualtFont = true;
	}

	
//	B2ChatDialog::wndEditProc = (WNDPROC)SetWindowLong(edit.ChatWndGetHandle(),GWL_WNDPROC,(LONG)(CHATEDITBOX::ChatEditProc));
	edit.SetWndProcessFun(ChatWndProcessFun::ProcessInputEditWnd);
	
	
	uiComboBox.UiComboBoxLoadIniFile(hwnd);
	faceDialog.hFaceDlg = CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_CHAT_FACE),
		hwnd,
		(DLGPROC)FaceDialog::FaceDialogProc);
	faceDialog.FaceDialogShow(FALSE);
	playerRelate.hWndHandle = CreateDialog(KWin32App::m_hInstance,MAKEINTRESOURCE(IDD_PLAYER_RELATE),hwnd,
		(DLGPROC)ChatTipWnd::ChatTipWndProc);
	playerRelate.ChatTipShow(FALSE);
	itemInfo.hTipItemWnd = CreateDialog(KWin32App::m_hInstance,MAKEINTRESOURCE(IDD_TIP_ITEM),hwnd,
		(DLGPROC)ChatTipWndItem::ChatTipWndProc);
	itemInfo.ChatTipWndShow(FALSE);

	m_ScrollBar.ChatScrollBarLoadIniCtg();
	m_ScrollBar.ChatScrollBarSetParent(hwnd);
	int scrollLength = m_ScrollBar.ChatScrollBarGetScrollLength();
	m_ScrollBar.SetBarRectPos(scrollLength);
//	
	return TRUE;
}
void ChatManager::ChatManagerSetCurrentPage(int pageIndex)
{

}

ChatManagerScrollBar::ChatManagerScrollBar()
{

}

ChatManagerScrollBar::~ChatManagerScrollBar()
{

}

void ChatManagerScrollBar::ChatScrollBarLoadIniCtg()
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
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, CHAT_MANAGER_SCROLLBAR_X, 0, &scrollBarX);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, CHAT_MANAGER_SCROLLBAR_Y, 0, &scrollBarY);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, CHAT_MANAGER_SCROLLBAR_WIDTH, 0, &scrollBarWidth);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, CHAT_MANAGER_SCROLLBAR_HEIGHT, 0, &scrollBarHeight);

	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, CHAT_MANAGER_SCROLLBAR_UP_WIDTH, 0, &scrollBarUpWidth);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, CHAT_MANAGER_SCROLLBAR_UP_HEIGHT, 0, &scrollBarUpHeight);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, CHAT_MANAGER_SCROLLBAR_DOWN_WIDTH, 0, &scrollBarDownWidth);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, CHAT_MANAGER_SCROLLBAR_DOWN_HEIGHT, 0, &scrollBarDownHeight);

	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, CHAT_MANAGER_SCROLLBAR_BAR_WIDTH, 0, &scrollBarRectWidth);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, CHAT_MANAGER_SCROLLBAR_BAR_HEIGHT, 0, &scrollBarRectHeight);

	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, CHAT_MANAGER_SCROLLBAR_RECT_WIDTH, 0, &scrollLengthRectWidth);
	int normal_idx = 0,hover_idx = 0,pushed_idx = 0,disable_idx = 0;
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, "up_part_normal_idx", 0, &normal_idx);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, "up_part_mouseOver_idx", 0, &hover_idx);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, "up_part_mouseDown_idx", 0, &pushed_idx);
	scrollUpPart.SetSrcIdx(normal_idx, hover_idx, pushed_idx, -1);

	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, "down_part_normal_idx", 0, &normal_idx);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, "down_part_mouseOver_idx", 0, &hover_idx);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, "down_part_mouseDown_idx", 0, &pushed_idx);
	scrollDownPart.SetSrcIdx(normal_idx, hover_idx, pushed_idx, -1);

	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, "scroll_bar_normal_src", 0, &normal_idx);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, "scroll_bar_mouseOver_src", 0, &hover_idx);
	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, "scroll_bar_mouseDown_src", 0, &pushed_idx);
	scrollBarPart.SetSrcIdx(normal_idx, hover_idx, pushed_idx, -1);

	iniFile.GetInteger(CHAT_MANAGER_SCROLLBAR_INI_NAME, "scroll_rect_idx", 0, &normal_idx);
	scrollRectPart.SetSrcIdx(normal_idx, -1, -1, -1);

	scrollBarRectPos = 0;
	scrollBarUpState = 0;
	scrollBarDownSate = 0;
	scrollBarRectState = 0;
	scrollLength = scrollBarHeight - scrollBarUpHeight - scrollBarDownHeight;
	ChatScrollBarSetScrolls(100,10);
}

void ChatManagerScrollBar::ChatScrollBarSetScrolls(int allLength, int oneLength)
{
	if(allLength==0)
	{
		oneLineLength = 0;
		return;
	}
	float height = (float)scrollLength * (float)oneLength / (float)allLength;
	oneLineLength = height;
}

int ChatManagerScrollBar::GetBarRectPos()
{
	return scrollBarRectPos;
}

int ChatManagerScrollBar::GetOneLineLength()
{
	return oneLineLength;
}

bool ChatManagerScrollBar::IsClickScrollBar(const POINT & pt)
{
	RECT scrollRect;
	scrollRect.left = scrollBarX;
	scrollRect.right = scrollRect.left + scrollBarRectWidth;
	scrollRect.top = scrollBarY + scrollBarUpHeight;
	scrollRect.bottom = scrollRect.top + scrollLength;
	
	if (!IsInRect(pt, scrollRect) || IsClickRect(pt))
	{
		return false;
	}

	return true;
}

void ChatManagerScrollBar::SetBarRectPos(int pos)
{
	if (pos + scrollBarRectHeight >= scrollLength)
	{
		scrollBarRectPos = scrollLength - scrollBarRectHeight;
	}
	else if (pos <= 0)
	{
		scrollBarRectPos = 0;
	}
	else
	{
		scrollBarRectPos = pos;
	}
}

int ChatManagerScrollBar::GetBarRectHeight()
{
	return scrollBarRectHeight;
}



bool ChatManagerScrollBar::IsClickRect(const POINT & pt)
{
	RECT rect;
	rect.left = scrollBarX;
	rect.right = rect.left + scrollBarRectWidth;
	rect.top = scrollBarY + scrollBarUpHeight + scrollBarRectPos;
	rect.bottom = rect.top + scrollBarRectHeight;

	return IsInRect(pt, rect);
}

int ChatManagerScrollBar::MoveBarRect(const POINT & pt)
{
	if(oneLineLength == 0)
		return FALSE;

	if (pt.y < (scrollBarY + scrollBarUpHeight))
	{
		scrollBarRectPos = 0;
		ChatScrolBarUpdate();
		return 0;
	}
	if (pt.y > (scrollBarY + scrollLength))
	{
		scrollBarRectPos = scrollLength - scrollBarRectHeight;
		ChatScrolBarUpdate();
		return 0;
	}

	int distance = (pt.y - scrollBarY - scrollBarUpHeight) - scrollBarRectPos;
	scrollBarRectPos += distance;

	if (scrollBarRectPos < 0)
	{
		scrollBarRectPos = 0;
	}
	else if (scrollBarRectPos > scrollLength - scrollBarRectHeight)
	{
		scrollBarRectPos = scrollLength - scrollBarRectHeight;
	}

	ChatScrolBarUpdate();
	return distance;
}
