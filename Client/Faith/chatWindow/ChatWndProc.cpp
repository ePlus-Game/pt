#include "KWin32App.h"
#include "chatWindow/ChatWndProc.h"

#include "layoutinterface.h"
#include "GameDataDef.h"
#include "Ui/UiCase/UiChatWindow.h"
#include <vector>
using std::vector;
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
#include <commctrl.h>


#include "chatWindow/ChatControlPanel.h"


#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "chatWindow/LookFriendInfoPopDlg.h"
#include "chatWindow/ChatMiniMap.h"
#include "chatWindow/ChatPlayerBaseInfoDlg.h"
#include "Ui/UiCase/UiMapCentre.h"
#include "chatWindow/ChatResource.h"
#include "chatWindow/EntrustComputerDlg.h"

#include "chatWindow/ChatClanPanel.h"
#include "chatWindow/ChatClanListControl.h"
#include "chatWindow/ChatClanTitleControl.h"
#include "chatWindow/ChatClanInfoDlg.h"
#include "chatWindow/ChatClanManager.h"
#include "SocialComDef.h"
#include "Ui/UiMDLDataset.h"
#include "chatWindow/ChatClanPlayerData.h"
#include "chatWindow/ChatClanPopMenu.h"
#include "chatWindow/ChatClanComboBox.h"
#include "chatWindow/ChatClanAnnouncementDlg.h"

using namespace ChatWndProcessFun;
#include "CoreShell.h"

extern iCoreShell*		g_pCoreShell;

LRESULT CALLBACK ChatWndProcessFun::ProcessPersonalButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_PERSONAL)->ChatPageGetButton();
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_PERSONAL)->ChatPageGetButton();
			int state = button.ChatWndGetState();
			button.ChatWndShowTip(FALSE);
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
			

		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_PERSONAL)->ChatPageGetButton();
			button.ChatWndSetTipPos();
			button.ChatWndShowTip(TRUE);

		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_PERSONAL)->ChatPageGetButton();
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			
			int state = button.ChatWndGetState();
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam ,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatManager& manager = B2ChatDialog::chatManager;
			if(manager.ChatManagerGetCurrentPage() == _PAGE_ID_PERSONAL)
			{
				return FALSE;
			}
			int currentIdx = manager.ChatManagerGetCurrentPage();
			ChatPage* pPage = manager.ChatManagerGetPage(currentIdx);
			ChatButton& pageButton = pPage->ChatPageGetButton();
			pageButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			pageButton.ChatWndUpdate();
			ChatPage* pCurrentPage = manager.ChatManagerGetPage(_PAGE_ID_PERSONAL);
			ChatButton& currentButton = pCurrentPage->ChatPageGetButton();
			currentButton.ChatWndKillTimer();
			currentButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
			currentButton.ChatWndUpdate();
			manager.currentPageIndex = _PAGE_ID_PERSONAL;
			manager.ChatManagerGetInfoWnd()->ChatInfoWndEnableScroll();
			manager.normalButton[_PAGE_ID_PERSONAL].ChatWndKillTimer();
			pCurrentPage->chatWndRender.ChatWndRenderAutoScroll(manager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom-manager.ChatManagerGetInfoWnd()->renderY);
			manager.ChatManagerGetInfoWnd()->ChatWndUpdate();
			if(manager.ChatManagerGetEditBox()->focus)
			{
				manager.ChatManagerGetEditBox()->ChatEditWndSetFocus(FALSE);
				manager.ChatManagerGetEditBox()->ChatWndUpdate();
			}
			if(!manager.ChatManagerGetInfoWnd()->ChatInfoWndGetFocus())
			{
				manager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(TRUE);
			}
			if(manager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				manager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			SetTimer(manager.infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
			manager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			manager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			manager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
		}
		return FALSE;
	case WM_DESTROY:
		{
			return FALSE;
		}
	}
	ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_PERSONAL)->ChatPageGetButton();
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}
LRESULT CALLBACK ChatWndProcessFun::ProcessFightPageButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_FIGHT)->ChatPageGetButton();
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_FIGHT)->ChatPageGetButton();
			int state = button.ChatWndGetState();
			button.ChatWndShowTip(FALSE);
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_FIGHT)->ChatPageGetButton();
			button.ChatWndSetTipPos();
			button.ChatWndShowTip(TRUE);

		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_FIGHT)->ChatPageGetButton();
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			
			int state = button.ChatWndGetState();
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam ,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatManager& manager = B2ChatDialog::chatManager;
			if(manager.ChatManagerGetCurrentPage() == _PAGE_ID_FIGHT)
			{
				return FALSE;
			}
			int currentIdx = manager.ChatManagerGetCurrentPage();
			ChatPage* pPage = manager.ChatManagerGetPage(currentIdx);
			ChatButton& pageButton = pPage->ChatPageGetButton();
			pageButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			pageButton.ChatWndUpdate();
			ChatPage* pCurrentPage = manager.ChatManagerGetPage(_PAGE_ID_FIGHT);
			ChatButton& currentButton = pCurrentPage->ChatPageGetButton();
			currentButton.ChatWndKillTimer();
			currentButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
			currentButton.ChatWndUpdate();
			manager.currentPageIndex = _PAGE_ID_FIGHT;
			manager.ChatManagerGetInfoWnd()->ChatInfoWndEnableScroll();
			manager.normalButton[_PAGE_ID_FIGHT].ChatWndKillTimer();
			pCurrentPage->chatWndRender.ChatWndRenderAutoScroll(manager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom - manager.ChatManagerGetInfoWnd()->renderY);
			manager.ChatManagerGetInfoWnd()->ChatWndUpdate();
			if(manager.ChatManagerGetEditBox()->focus)
			{
				manager.ChatManagerGetEditBox()->ChatEditWndSetFocus(FALSE);
				manager.ChatManagerGetEditBox()->ChatWndUpdate();
			}
			if(!manager.ChatManagerGetInfoWnd()->ChatInfoWndGetFocus())
			{
				manager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(TRUE);
			}
			if(manager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				manager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			SetTimer(manager.infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
			manager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			manager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			manager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
		}
		return FALSE;
	case WM_DESTROY:
		{
			return FALSE;
		}
	}
	ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_FIGHT)->ChatPageGetButton();
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}
LRESULT CALLBACK ChatWndProcessFun::ProcessOrgPageButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_ORG)->ChatPageGetButton();
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_ORG)->ChatPageGetButton();
			int state = button.ChatWndGetState();
			button.ChatWndShowTip(FALSE);
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
			

		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_ORG)->ChatPageGetButton();
			button.ChatWndSetTipPos();
			button.ChatWndShowTip(TRUE);

		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_ORG)->ChatPageGetButton();
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			
			int state = button.ChatWndGetState();
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam ,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatManager& manager = B2ChatDialog::chatManager;
			if(manager.ChatManagerGetCurrentPage() == _PAGE_ID_ORG)
			{
				return FALSE;
			}
			int currentIdx = manager.ChatManagerGetCurrentPage();
			ChatPage* pPage = manager.ChatManagerGetPage(currentIdx);
			ChatButton& pageButton = pPage->ChatPageGetButton();
			pageButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			pageButton.ChatWndUpdate();
			ChatPage* pCurrentPage = manager.ChatManagerGetPage(_PAGE_ID_ORG);
			ChatButton& currentButton = pCurrentPage->ChatPageGetButton();
			currentButton.ChatWndKillTimer();
			currentButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
			currentButton.ChatWndUpdate();
			manager.currentPageIndex = _PAGE_ID_ORG;
			manager.ChatManagerGetInfoWnd()->ChatInfoWndEnableScroll();
			manager.normalButton[_PAGE_ID_ORG].ChatWndKillTimer();
			pCurrentPage->chatWndRender.ChatWndRenderAutoScroll(manager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom - manager.ChatManagerGetInfoWnd()->renderY);
			manager.ChatManagerGetInfoWnd()->ChatWndUpdate();
			if(manager.ChatManagerGetEditBox()->focus)
			{
				manager.ChatManagerGetEditBox()->ChatEditWndSetFocus(FALSE);
				manager.ChatManagerGetEditBox()->ChatWndUpdate();
			}
			if(!manager.ChatManagerGetInfoWnd()->ChatInfoWndGetFocus())
			{
				manager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(TRUE);
			}
			if(manager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				manager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			SetTimer(manager.infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
			manager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			manager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			manager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
		}
		return FALSE;
	case WM_DESTROY:
		{
			return FALSE;
		}
	}
	ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_ORG)->ChatPageGetButton();
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}
LRESULT CALLBACK ChatWndProcessFun::ProcessNearPageButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_NEAR)->ChatPageGetButton();
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_NEAR)->ChatPageGetButton();
			int state = button.ChatWndGetState();
			button.ChatWndShowTip(FALSE);
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
			

		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_NEAR)->ChatPageGetButton();
			button.ChatWndSetTipPos();
			button.ChatWndShowTip(TRUE);

		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_NEAR)->ChatPageGetButton();
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			
			int state = button.ChatWndGetState();
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam ,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatManager& manager = B2ChatDialog::chatManager;
			if(manager.ChatManagerGetCurrentPage() == _PAGE_ID_NEAR)
			{
				return FALSE;
			}
			int currentIdx = manager.ChatManagerGetCurrentPage();
			ChatPage* pPage = manager.ChatManagerGetPage(currentIdx);
			ChatButton& pageButton = pPage->ChatPageGetButton();
			pageButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			pageButton.ChatWndUpdate();
			ChatPage* pCurrentPage = manager.ChatManagerGetPage(_PAGE_ID_NEAR);
			ChatButton& currentButton = pCurrentPage->ChatPageGetButton();
			currentButton.ChatWndKillTimer();
			currentButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
			currentButton.ChatWndUpdate();
			manager.currentPageIndex = _PAGE_ID_NEAR;
			manager.ChatManagerGetInfoWnd()->ChatInfoWndEnableScroll();
			manager.normalButton[_PAGE_ID_NEAR].ChatWndKillTimer();
			pCurrentPage->chatWndRender.ChatWndRenderAutoScroll(manager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom - manager.ChatManagerGetInfoWnd()->renderY);
			manager.ChatManagerGetInfoWnd()->ChatWndUpdate();
			if(manager.ChatManagerGetEditBox()->focus)
			{
				manager.ChatManagerGetEditBox()->ChatEditWndSetFocus(FALSE);
				manager.ChatManagerGetEditBox()->ChatWndUpdate();
			}
			if(!manager.ChatManagerGetInfoWnd()->ChatInfoWndGetFocus())
			{
				manager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(TRUE);
			}
			if(manager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				manager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			SetTimer(manager.infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
			manager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			manager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			manager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
		}
		return FALSE;
	case WM_DESTROY:
		{
			return FALSE;
		}
	}
	ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_NEAR)->ChatPageGetButton();
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

LRESULT CALLBACK  ChatWndProcessFun::ProcessSynthetizePageButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	///获取这个窗口的
	
	switch(msg)
	{
	case WM_PAINT:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_SYNTHESIS)->ChatPageGetButton();
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_SYNTHESIS)->ChatPageGetButton();
			int state = button.ChatWndGetState();
			button.ChatWndShowTip(FALSE);
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
			

		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_SYNTHESIS)->ChatPageGetButton();
			button.ChatWndSetTipPos();
			button.ChatWndShowTip(TRUE);

		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_SYNTHESIS)->ChatPageGetButton();
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			
			int state = button.ChatWndGetState();
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam ,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatManager& manager = B2ChatDialog::chatManager;
			if(manager.ChatManagerGetCurrentPage() == _PAGE_ID_SYNTHESIS)
			{
				return FALSE;
			}
			int currentIdx = manager.ChatManagerGetCurrentPage();
			ChatPage* pPage = manager.ChatManagerGetPage(currentIdx);
			ChatButton& pageButton = pPage->ChatPageGetButton();
			pageButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			pageButton.ChatWndUpdate();
			ChatPage* pCurrentPage = manager.ChatManagerGetPage(_PAGE_ID_SYNTHESIS);
			ChatButton& currentButton = pCurrentPage->ChatPageGetButton();
			currentButton.ChatWndKillTimer();
			currentButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
			currentButton.ChatWndUpdate();
			manager.currentPageIndex = _PAGE_ID_SYNTHESIS;
			manager.ChatManagerGetInfoWnd()->ChatInfoWndEnableScroll();
			manager.normalButton[_PAGE_ID_SYNTHESIS].ChatWndKillTimer();
			pCurrentPage->chatWndRender.ChatWndRenderAutoScroll(manager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom - manager.ChatManagerGetInfoWnd()->renderY);
			manager.ChatManagerGetInfoWnd()->ChatWndUpdate();
			if(manager.ChatManagerGetEditBox()->focus)
			{
				manager.ChatManagerGetEditBox()->ChatEditWndSetFocus(FALSE);
				manager.ChatManagerGetEditBox()->ChatWndUpdate();
			}
			if(!manager.ChatManagerGetInfoWnd()->ChatInfoWndGetFocus())
			{
				manager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(TRUE);
			}
			if(manager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				manager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			SetTimer(manager.infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
			manager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			manager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			manager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
		}
		return FALSE;
	case WM_DESTROY:
		{
			return FALSE;
		}
	}
	ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_SYNTHESIS)->ChatPageGetButton();
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}
LRESULT CALLBACK  ChatWndProcessFun::ProcessWorldPageButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	///获取这个窗口的
	
	switch(msg)
	{
	case WM_PAINT:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_WORLD)->ChatPageGetButton();
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_WORLD)->ChatPageGetButton();
			int state = button.ChatWndGetState();
			button.ChatWndShowTip(FALSE);
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_WORLD)->ChatPageGetButton();
			button.ChatWndSetTipPos();
			button.ChatWndShowTip(TRUE);

		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam ,lParam);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_WORLD)->ChatPageGetButton();
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			
			int state = button.ChatWndGetState();
			if(state== _CHAT_BUTTON_STATE_MOUSEDOWN||
				state == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatManager& manager = B2ChatDialog::chatManager;
			if(manager.ChatManagerGetCurrentPage() == _PAGE_ID_WORLD)
			{
				return FALSE;
			}
			int currentIdx = manager.ChatManagerGetCurrentPage();
			ChatPage* pPage = manager.ChatManagerGetPage(currentIdx);
			ChatButton& pageButton = pPage->ChatPageGetButton();
			pageButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			pageButton.ChatWndUpdate();
			ChatPage* pCurrentPage = manager.ChatManagerGetPage(_PAGE_ID_WORLD);
			ChatButton& currentButton = pCurrentPage->ChatPageGetButton();
			currentButton.ChatWndKillTimer();
			currentButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
			currentButton.ChatWndUpdate();
			manager.currentPageIndex = _PAGE_ID_WORLD;
			manager.ChatManagerGetInfoWnd()->ChatInfoWndEnableScroll();
			manager.normalButton[_DOWN_BUTTON_INDEX].ChatWndKillTimer();
			pCurrentPage->chatWndRender.ChatWndRenderAutoScroll(manager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom - manager.ChatManagerGetInfoWnd()->renderY);
			manager.ChatManagerGetInfoWnd()->ChatWndUpdate();
			if(manager.ChatManagerGetEditBox()->focus)
			{
				manager.ChatManagerGetEditBox()->ChatEditWndSetFocus(FALSE);
				manager.ChatManagerGetEditBox()->ChatWndUpdate();
			}
			if(!manager.ChatManagerGetInfoWnd()->ChatInfoWndGetFocus())
			{
				manager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(TRUE);
			}
			if(manager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				manager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			SetTimer(manager.infoWnd.ChatWndGetHandle(),_CHAT_NEED_UPDATE_TIMER_ID,_CHAT_NEED_UPDATE_TIMER_DT,InfoWndUpdate);
			manager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			manager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			manager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
		}
		return FALSE;
	case WM_DESTROY:
		{
			return FALSE;
		}
	}
	ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetPage(_PAGE_ID_WORLD)->ChatPageGetButton();
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessChatInfoWnd(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			int flags = 0;
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			if(wParam&MK_CONTROL)
				flags|=_CLICK_FLAG_CONTROL;
			else
			if(wParam&MK_SHIFT)
				flags|= _CLICK_FLAG_SHIFT;
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndProcessClickText(pt,flags);

//			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndSetFocus(true);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
//			SetFocus(hwnd);

		}
		return FALSE;
	case WM_RBUTTONDOWN:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			int flags = _CLICK_FLAG_RBUTTON;
			
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndProcessClickText(pt,flags);

		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_MOUSEWHEEL:
		{
			int zDelta = HIWORD(wParam);
			ChatPage* pCurrentPage = B2ChatDialog::chatManager.ChatManagerGetPage(B2ChatDialog::chatManager.currentPageIndex);
			if(zDelta ==120)
			{
				if(pCurrentPage->chatWndRender.infoWndLayOut[0].pt.y>=B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY)
				{
					pCurrentPage->chatWndRender.infoWndLayOut[0].pt.y = B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY;
					return FALSE;
				}
				pCurrentPage->chatWndRender.ChatWndRenderScroll(GDIRender::wordSize);
				if(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndIsScroll())
				{
					B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndDisableScroll();
				}
			}
			else
			{
				if(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom - B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY>=
					pCurrentPage->chatWndRender.allHeight+pCurrentPage->chatWndRender.infoWndLayOut[0].pt.y)
				{
					B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndEnableScroll();
					B2ChatDialog::chatManager.normalButton[_DOWN_BUTTON_INDEX].ChatWndKillTimer();
					return FALSE;
				}
				if(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndIsScroll())
				{
					B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndDisableScroll();
				}
				pCurrentPage->chatWndRender.ChatWndRenderScroll(-GDIRender::wordSize);
			}
			B2ChatDialog::chatManager.RecalculateScroll();
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndUpdate();

		}
		return FALSE;
	case WM_LBUTTONUP:
		{
			SendMessage(B2ChatDialog::chatManager.ChatManagerGetScrollBar()->ChatScrollBarGetParentWnd(), WM_LBUTTONUP, wParam, lParam);
		}
		return 0;
	case WM_MOUSEMOVE:
		{
			SendMessage(B2ChatDialog::chatManager.ChatManagerGetScrollBar()->ChatScrollBarGetParentWnd(), WM_MOUSEMOVE, wParam, lParam);
		}
		return 0;
	}
	ChatWnd* pWnd = B2ChatDialog::chatManager.ChatManagerGetInfoWnd();
	return CallWindowProc(pWnd->ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK  ChatWndProcessFun::ProcessChangeChatButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			B2ChatDialog::chatManager.normalButton[_CLOSE_BUTTON_INDEX].ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.normalButton[_CLOSE_BUTTON_INDEX];
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
	case WM_MOUSEHOVER:
		{
			ChatButton& button = B2ChatDialog::chatManager.normalButton[_CLOSE_BUTTON_INDEX];
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
			
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.normalButton[_CLOSE_BUTTON_INDEX];
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(FALSE);
			}
			int state = button.ChatWndGetState();
			if(state  == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatMainDlg::MainDlgShowWndChat(FALSE);	
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			if(!IsWindowVisible(KWin32App::m_hMainWnd))
			{
				RECT destRect;
				GetClientRect(GetDesktopWindow(),&destRect);
				RECT gameWindow;
				GetWindowRect(KWin32App::m_hMainWnd,&gameWindow);
				int gameWindowWidth = gameWindow.right - gameWindow.left;
				int gameWindowHeight = gameWindow.bottom - gameWindow.top;
				int gameWindowX = (destRect.right - gameWindowWidth)/2;
				int gameWindowY = (destRect.bottom - gameWindowHeight)/2;
				MoveWindow(KWin32App::m_hMainWnd,gameWindowX,gameWindowY,gameWindowWidth,gameWindowHeight,true);
				ShowWindow(KWin32App::m_hMainWnd,SW_NORMAL);
			}
			KUiChannelCentre::Show();
			KUiMiniMap::Show();
			Shell_NotifyIcon(NIM_DELETE,&ChatMainDlg::taskInfo);
			BringWindowToTop(KWin32App::m_hMainWnd);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
			KUiChannelCentre::GetSingleton().showSystemFrame(true);
			KWin32Frame::s_minisized = false;
			
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;

	}
	ChatButton& button = B2ChatDialog::chatManager.normalButton[_CLOSE_BUTTON_INDEX];
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}


LRESULT CALLBACK ChatWndProcessFun::ProcessHideGameWndButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			B2ChatDialog::chatManager.normalButton[_CLOSEWND_BUTTON_INDEX].ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.normalButton[_CLOSEWND_BUTTON_INDEX];
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=500;
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
	case WM_MOUSEHOVER:
		{
			ChatButton& button = B2ChatDialog::chatManager.normalButton[_CLOSEWND_BUTTON_INDEX];
			if(button.ChatWndTipEnable())
			{
				if(IsWindowVisible(KWin32App::m_hMainWnd))
				{
					button.ChatWndUpdataTipText(ChatString::ChatStringGetString().hideGameWnd);
				}
				else
					button.ChatWndUpdataTipText(ChatString::ChatStringGetString().showGameWnd);
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
			
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatButton& button = B2ChatDialog::chatManager.normalButton[_CLOSEWND_BUTTON_INDEX];
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(FALSE);
			}
			int state = button.ChatWndGetState();
			if(state  == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(B2ChatDialog::chatManager.isShowMainWnd)
			{
					Shell_NotifyIcon(NIM_ADD,&ChatMainDlg::taskInfo);
					ShowWindow(KWin32App::m_hMainWnd,SW_HIDE);
					KWin32Frame::s_minisized = true;
					B2ChatDialog::chatManager.isShowMainWnd = FALSE;
			}else
			{
				Shell_NotifyIcon(NIM_DELETE,&ChatMainDlg::taskInfo);
				ShowWindow(KWin32App::m_hMainWnd,SW_NORMAL);
				RECT rc;
				::GetWindowRect(KWin32App::m_hMainWnd,&rc);
				RECT rc1;
				::GetWindowRect(ChatMainDlg::hMainDlg,&rc1);
				HWND hDesk = GetDesktopWindow();
				RECT destRc;
				GetClientRect(hDesk,&destRc);
				int widthMain = rc.right - rc.left;
				int widthMain1 = rc1.right - rc1.left;
				int heightMain = rc.bottom - rc.top;
				int heightMain1 = rc1.bottom - rc.top;
				int x = (destRc.right-(widthMain+widthMain1))/2;
				int y = (destRc.bottom - (heightMain))/2;
				MoveWindow(KWin32App::m_hMainWnd,x,y,widthMain,heightMain,TRUE);
				MoveWindow(ChatMainDlg::hMainDlg,x+widthMain,y,widthMain1,heightMain1,TRUE);
				KWin32Frame::s_minisized = false;
				B2ChatDialog::chatManager.isShowMainWnd = TRUE;
			}
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;

	}
	ChatButton& button = B2ChatDialog::chatManager.normalButton[_CLOSEWND_BUTTON_INDEX];
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}


LRESULT CALLBACK ChatWndProcessFun::ProcessShowFaceButton(HWND hwnd,UINT msg,WPARAM wParam ,LPARAM lParam)
{
	ChatButton& button = B2ChatDialog::chatManager.showButton;
	switch(msg)
	{
	case WM_PAINT:
		{
			
			PAINTSTRUCT ps;
			HDC hdc  = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogIsShow())
				B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			else
			{
				B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogAdjustWindow(B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->hFaceDlg);
				B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(TRUE);
			}
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessChatInfoTopButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = B2ChatDialog::chatManager.normalButton[_TOP_BUTTON_INDEX];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(B2ChatDialog::chatManager.ChatManagerGetEditBox()->focus)
			{
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(FALSE);
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
			}
			ChatPage* pCurrentPage = B2ChatDialog::chatManager.ChatManagerGetPage(B2ChatDialog::chatManager.currentPageIndex);
			pCurrentPage->chatWndRender.ChatWndRenderScroll(-pCurrentPage->chatWndRender.infoWndLayOut[0].pt.y+B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY);
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndUpdate();
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndDisableScroll();
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			B2ChatDialog::chatManager.RecalculateScroll();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessChatInfoEndButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = B2ChatDialog::chatManager.normalButton[_END_BUTTON_INDEX];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(B2ChatDialog::chatManager.ChatManagerGetEditBox()->focus)
			{
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(FALSE);
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
			}
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndEnableScroll();
			ChatPage* pCurrentPage = B2ChatDialog::chatManager.ChatManagerGetPage(B2ChatDialog::chatManager.currentPageIndex);
			pCurrentPage->chatWndRender.ChatWndRenderAutoScroll(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom - B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY);
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndUpdate();
			B2ChatDialog::chatManager.normalButton[_DOWN_BUTTON_INDEX].ChatWndKillTimer();
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			B2ChatDialog::chatManager.RecalculateScroll();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

LRESULT CALLBACK ChatWndProcessFun::ProcessChatInfoUpButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = B2ChatDialog::chatManager.normalButton[_UP_BUTTON_INDEX];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(B2ChatDialog::chatManager.ChatManagerGetEditBox()->focus)
			{
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(FALSE);
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
			}
			ChatPage* pCurrentPage = B2ChatDialog::chatManager.ChatManagerGetPage(B2ChatDialog::chatManager.currentPageIndex);
//			int size = GDIRender::wordSize;
//			if(size < )
			pCurrentPage->chatWndRender.ChatWndRenderScroll(GDIRender::wordSize);
			if(pCurrentPage->chatWndRender.infoWndLayOut[0].pt.y>B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY)
				pCurrentPage->chatWndRender.ChatWndRenderScroll(-pCurrentPage->chatWndRender.infoWndLayOut[0].pt.y+B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY);
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndUpdate();
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndDisableScroll();
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			B2ChatDialog::chatManager.RecalculateScroll();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessChatInfoDownButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = B2ChatDialog::chatManager.normalButton[_DOWN_BUTTON_INDEX];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(B2ChatDialog::chatManager.ChatManagerGetEditBox()->focus)
			{
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(FALSE);
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
			}
			ChatPage* pCurrentPage = B2ChatDialog::chatManager.ChatManagerGetPage(B2ChatDialog::chatManager.currentPageIndex);
			if(pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed-1].pt.y +
				pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed-1].renderHeight<B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom-B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY)
				return FALSE;
			pCurrentPage->chatWndRender.ChatWndRenderScroll(-GDIRender::wordSize);
			if(pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed-1].pt.y+
				pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed-1].renderHeight<B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom-B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY)
			{
				pCurrentPage->chatWndRender.ChatWndRenderScroll(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom-pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed-1].pt.y-
					pCurrentPage->chatWndRender.infoWndLayOut[pCurrentPage->chatWndRender.numberUsed-1].renderHeight);
				B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndEnableScroll();
				pCurrentPage->chatWndRender.ChatWndRenderAutoScroll(B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndGetRect().bottom - B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY);
			}
			B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatWndUpdate();
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			B2ChatDialog::chatManager.RecalculateScroll();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessTipShowControl(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndDrawTip(hdc);
			EndPaint(hwnd,&ps);
		}
	}
	return CallWindowProc(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->tipShowProc,hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessTipItemCloseButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->closeButton;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndAdjustPos();
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessChannelChangeButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = B2ChatDialog::chatManager.ChatManagerGetUiComboBox().selectItemButton;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().UiComBoBoxProcessClickButton();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessInputEditWnd(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
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
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditSetCursorFlash();
		
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
			B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditKillCursor();
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
			case VK_HOME:
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessKeyHome();
				return FALSE;
			case VK_END:
				B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndProcessKeyEnd();
				return FALSE;
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
//						pEdit->pEditLayOut->clearLayout();
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->ClearText( );
						pEdit->pEditLayOut->SetText(_CHAT_EIDT_DEFAULT_STRING);
						KUiChatInputWnd::GetSingleton().prevMessage(pEdit->pEditLayOut);
						LOElemInfo* pElem = 0;
						int numbers = pEdit->pEditLayOut->getElemList(pElem);
						if(numbers<=0)
						{
							delete [] pElem;
							return false;
						}
//						pEdit->pEditLayOut->clearLayout();
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->ClearText( );
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
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->ClearText( );
//						pEdit->pEditLayOut->clearLayout();
						pEdit->pEditLayOut->SetText(_CHAT_EIDT_DEFAULT_STRING);
						KUiChatInputWnd::GetSingleton().nextMessage(pEdit->pEditLayOut);
						LOElemInfo* pElem = 0;
						int numbers = pEdit->pEditLayOut->getElemList(pElem);
						if(numbers<=0)
						{
							delete [] pElem;
							return false;
						}
//						pEdit->pEditLayOut->clearLayout();
						B2ChatDialog::chatManager.ChatManagerGetEditBox()->ClearText( );
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
			return 0;
			
		}
		return 0;
	}
	return CallWindowProc(B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessChatInfoPageButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton* button = ChatControlPanel::ChatPanelGetPanel().panelButtonList[_CHAT_PANEL_CHAT_PANEL];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button->ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button->ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			int state = button->ChatWndGetState();
			if(state == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button->ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			if(button->ChatWndTipEnable())
			{
				button->ChatWndSetTipPos();
				button->ChatWndShowTip(FALSE);
			}
			int state = button->ChatWndGetState();
			if(state  == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button->ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button->ChatWndTipEnable())
			{
				button->ChatWndSetTipPos();
				button->ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatControlPanel::ChatPanelGetPanel().ChatPanelShowChatInfoPanel();
		}
		return FALSE;
	}
	return CallWindowProc(button->ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessFrindInfoPageButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton* button = ChatControlPanel::ChatPanelGetPanel().panelButtonList[_CHAT_PANEL_FRIEND_PANEL];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button->ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button->ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			int state = button->ChatWndGetState();
			if(state == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button->ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			if(button->ChatWndTipEnable())
			{
				button->ChatWndSetTipPos();
				button->ChatWndShowTip(FALSE);
			}
			int state = button->ChatWndGetState();
			if(state  == _CHAT_BUTTON_STATE_DISABLE ||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button->ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button->ChatWndTipEnable())
			{
				button->ChatWndSetTipPos();
				button->ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatControlPanel::ChatPanelGetPanel().ChatPanelShowFriendInfoPanel();
		}
		return FALSE;
	}
	return CallWindowProc(button->ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

LRESULT CALLBACK ChatWndProcessFun::ProcessEntrustPageButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton* button = ChatControlPanel::ChatPanelGetPanel().panelButtonList[_CHAT_PANEL_TRUST_PANEL];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button->ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button->ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			int state = button->ChatWndGetState();
			if(state == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button->ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			if(button->ChatWndTipEnable())
			{
				button->ChatWndSetTipPos();
				button->ChatWndShowTip(FALSE);
			}
			int state = button->ChatWndGetState();
			if(state  == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button->ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button->ChatWndTipEnable())
			{
				button->ChatWndSetTipPos();
				button->ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatControlPanel::ChatPanelGetPanel().ChatPanelShowEntrustPanel();
		}
		return FALSE;
	}
	return CallWindowProc(button->ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

LRESULT CALLBACK ChatWndProcessFun::ProcessClanPageButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton* button = ChatControlPanel::ChatPanelGetPanel().panelButtonList[_CHAT_PANEL_CLAN_PANEL];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button->ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button->ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			int state = button->ChatWndGetState();
			if(state == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button->ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			if(button->ChatWndTipEnable())
			{
				button->ChatWndSetTipPos();
				button->ChatWndShowTip(FALSE);
			}
			int state = button->ChatWndGetState();
			if(state  == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button->ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button->ChatWndTipEnable())
			{
				button->ChatWndSetTipPos();
				button->ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatControlPanel::ChatPanelGetPanel().ChatPanelShowClanPanel();
		}
		return FALSE;
	}
	return CallWindowProc(button->ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessLuedPageButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton* button = ChatControlPanel::ChatPanelGetPanel().panelButtonList[_CHAT_PANEL_LUED_PANEL];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button->ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button->ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			int state = button->ChatWndGetState();
			if(state == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button->ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			if(button->ChatWndTipEnable())
			{
				button->ChatWndSetTipPos();
				button->ChatWndShowTip(FALSE);
			}
			int state = button->ChatWndGetState();
			if(state  == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button->ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button->ChatWndTipEnable())
			{
				button->ChatWndSetTipPos();
				button->ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatControlPanel::ChatPanelGetPanel().ChatPanelShowLuedPanel();
		}
		return FALSE;
	}
	return CallWindowProc(button->ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessFriendListButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetButtonByIdx(_FRIEND_CONTROL_IDX_FRIEND);
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
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
			if(state  == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(button.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN||
				button.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(button.ChatWndGetState() != _CHAT_BUTTON_STATE_MOUSEDOWN )
			{
				ChatButton& tempButton = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetButtonByIdx(ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.currentIdx);
				tempButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			//	tempButton.ChatWndMove(tempButton.x+2,tempButton.y,tempButton.ChatWndGetRect().right-4,tempButton.ChatWndGetRect().bottom,false);
				tempButton.ChatWndUpdate();
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.currentIdx = _CHAT_PLAYER_INFO_FRIEND;
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
			//	button.ChatWndMove(button.x,button.y,button.ChatWndGetRect().right,button.ChatWndGetRect().bottom,false);
				button.ChatWndUpdate();
				ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerHideInfoDlg();
				ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoClearSelect(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendSetCurrentDlg(&ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_CHAT_PLAYER_INFO_FRIEND]);
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoDlgShowDlg(TRUE);

			}
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

LRESULT CALLBACK ChatWndProcessFun::processEnemyListButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetButtonByIdx(_FIREND_CONTROL_IDX_ENEMY);
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
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
			if(state  == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(button.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN||
				button.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(button.ChatWndGetState() != _CHAT_BUTTON_STATE_MOUSEDOWN )
			{
				ChatButton& tempButton = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetButtonByIdx(ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.currentIdx);
				tempButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		//		tempButton.ChatWndMove(tempButton.x+2,tempButton.y,tempButton.ChatWndGetRect().right-4,tempButton.ChatWndGetRect().bottom,false);
				tempButton.ChatWndUpdate();
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.currentIdx = _FIREND_CONTROL_IDX_ENEMY;

				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
		//		button.ChatWndMove(button.x,button.y,button.ChatWndGetRect().right,button.ChatWndGetRect().bottom,false);
				button.ChatWndUpdate();
				ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerHideInfoDlg();
				ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoClearSelect(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendSetCurrentDlg(&ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_FIREND_CONTROL_IDX_ENEMY]);
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoDlgShowDlg(TRUE);

			}
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

LRESULT CALLBACK ChatWndProcessFun::processPingbiListButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetButtonByIdx(_FRIEND_CONTROL_IDX_PINGBI);
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
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
			if(state  == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(button.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN||
				button.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(button.ChatWndGetState() != _CHAT_BUTTON_STATE_MOUSEDOWN )
			{
				ChatButton& tempButton = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetButtonByIdx(ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.currentIdx);
				tempButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		//		tempButton.ChatWndMove(tempButton.x+2,tempButton.y,tempButton.ChatWndGetRect().right-4,tempButton.ChatWndGetRect().bottom,false);
				tempButton.ChatWndUpdate();
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.currentIdx = _FRIEND_CONTROL_IDX_PINGBI;

				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
			//	button.ChatWndMove(button.x,button.y,button.ChatWndGetRect().right,button.ChatWndGetRect().bottom,false);
				button.ChatWndUpdate();
				ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerHideInfoDlg();
				ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoClearSelect(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendSetCurrentDlg(&ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_FRIEND_CONTROL_IDX_PINGBI]);
				ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoDlgShowDlg(TRUE);

			}
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

BOOL CALLBACK   ChatWndProcessFun::FriendDlgProc(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam)
{
	PlayerInfoDlg& infoDlg= ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_PLAYER_INFO_DLG_FRIEND];
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_PAINT:
		{
	//		ShowWindow(hwnd,SW_HIDE);
			PAINTSTRUCT ps;
			HDC  hdc = BeginPaint(hwnd,&ps);
			infoDlg.PlayerInfoDlgProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			infoDlg.PlayerInfoDlgProcessPaint((HDC)wParam);
		}
		return TRUE;
	case WM_MOUSEMOVE:
		{
			infoDlg.PlayerInfoDlgProcessMouseMove(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			infoDlg.PlayerInfoDlgProcessMouseLButtonDown(hwnd,wParam,lParam);
			SetFocus(hwnd);
		}
		return FALSE;
	case WM_LBUTTONUP:
		{
			infoDlg.PlayerInfoDlgProcessMouseLButtonUp(hwnd,wParam,lParam);
			SetFocus(ChatControlPanel::ChatPanelGetPanel().hPanelDlg);
		}
		return FALSE;
	case WM_MOUSEWHEEL:
		{
			infoDlg.PlayerInfoDlgProcessMouseWheel(hwnd , wParam , lParam);
		}
		return FALSE;
	case WM_LBUTTONDBLCLK:
		{
			
			infoDlg.PlayerInfoProcessLButtonDBCLK(hwnd,wParam,lParam);
	//		SetFocus(hwnd);
		}
		return FALSE;
	case WM_RBUTTONUP:
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->hDlg,&pt);
			LPPLAYERCONTROL pControl = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoGetControlByPoint(pt);
			if(pControl==0)
				return FALSE;
			pControl ->PlayerControlSetSelected(TRUE, TRUE);
			if(pControl->PlayerControlGetPlayerInfo().isPlayerOnline == false)
			{
				LookFriendInfoDlg::GetSingle().GetTalkPersonalBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				LookFriendInfoDlg::GetSingle().GetLookInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				LookFriendInfoDlg::GetSingle().GetMakeTeamBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			}
			else
			{
				LookFriendInfoDlg::GetSingle().GetTalkPersonalBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				LookFriendInfoDlg::GetSingle().GetLookInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				LookFriendInfoDlg::GetSingle().GetMakeTeamBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			}
			LookFriendInfoDlg::GetSingle().GetDeleteBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			LookFriendInfoDlg::GetSingle().GetPingBiInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			LookFriendInfoDlg::GetSingle().GetFriendInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			LookFriendInfoDlg::GetSingle().SetPlayerProcessControl(pControl);
			LookFriendInfoDlg::GetSingle().AdjustWindow();
			LookFriendInfoDlg::GetSingle().Show();
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
	case WM_SHOWWINDOW:
		{

		}
		return FALSE;

	}

	return FALSE;
}

BOOL CALLBACK   ChatWndProcessFun::EnemyDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	PlayerInfoDlg& infoDlg= ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_PLAYER_INFO_DLG_ENEMY];
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC  hdc = BeginPaint(hwnd,&ps);
			infoDlg.PlayerInfoDlgProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			infoDlg.PlayerInfoDlgProcessPaint((HDC)wParam);
		}
		return TRUE;
	case WM_MOUSEMOVE:
		{
			infoDlg.PlayerInfoDlgProcessMouseMove(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			infoDlg.PlayerInfoDlgProcessMouseLButtonDown(hwnd,wParam,lParam);
			SetFocus(hwnd);
		}
		return FALSE;
	case WM_LBUTTONUP:
		{
			infoDlg.PlayerInfoDlgProcessMouseLButtonUp(hwnd,wParam,lParam);
			SetFocus(ChatControlPanel::ChatPanelGetPanel().hPanelDlg);
		}
		return FALSE;
	case WM_MOUSEWHEEL:
		{
			infoDlg.PlayerInfoDlgProcessMouseWheel(hwnd , wParam , lParam);
		}
		return FALSE;
	case WM_LBUTTONDBLCLK:
		{
			
			infoDlg.PlayerInfoProcessLButtonDBCLK(hwnd,wParam,lParam);
	//		SetFocus(hwnd);
		}
		return FALSE;
	case WM_RBUTTONUP:
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->hDlg,&pt);
			LPPLAYERCONTROL pControl = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoGetControlByPoint(pt);
			if(pControl==0)
				return FALSE;
			LookFriendInfoDlg::GetSingle().GetDeleteBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			LookFriendInfoDlg::GetSingle().GetLookInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			LookFriendInfoDlg::GetSingle().GetTalkPersonalBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			if(pControl->PlayerControlGetPlayerInfo().isPlayerOnline == false)
			{
				LookFriendInfoDlg::GetSingle().GetMakeTeamBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			}
			else
			{
				LookFriendInfoDlg::GetSingle().GetMakeTeamBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			}
			LookFriendInfoDlg::GetSingle().GetPingBiInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			LookFriendInfoDlg::GetSingle().GetFriendInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			LookFriendInfoDlg::GetSingle().SetPlayerProcessControl(pControl);
			LookFriendInfoDlg::GetSingle().AdjustWindow();
			LookFriendInfoDlg::GetSingle().Show();
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

BOOL CALLBACK   ChatWndProcessFun::PingBiDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	PlayerInfoDlg& infoDlg= ChatFriendPanelManager::ChatFriendManagerGet().playerInfoDlg[_PLAYER_INFO_DLG_PINGBI];
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC  hdc = BeginPaint(hwnd,&ps);
			infoDlg.PlayerInfoDlgProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			infoDlg.PlayerInfoDlgProcessPaint((HDC)wParam);
		}
		return TRUE;
	case WM_MOUSEMOVE:
		{
			infoDlg.PlayerInfoDlgProcessMouseMove(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			infoDlg.PlayerInfoDlgProcessMouseLButtonDown(hwnd,wParam,lParam);
			SetFocus(hwnd);
		}
		return FALSE;
	case WM_LBUTTONUP:
		{
			infoDlg.PlayerInfoDlgProcessMouseLButtonUp(hwnd,wParam,lParam);
			SetFocus(ChatControlPanel::ChatPanelGetPanel().hPanelDlg);
		}
		return FALSE;
	case WM_RBUTTONUP:
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->hDlg,&pt);
			LPPLAYERCONTROL pControl = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoGetControlByPoint(pt);
			if(pControl==0)
				return FALSE;
			LookFriendInfoDlg::GetSingle().GetDeleteBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			LookFriendInfoDlg::GetSingle().GetLookInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			LookFriendInfoDlg::GetSingle().GetTalkPersonalBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			if(pControl->PlayerControlGetPlayerInfo().isPlayerOnline == false)
			{
				LookFriendInfoDlg::GetSingle().GetMakeTeamBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			}
			else
			{
				LookFriendInfoDlg::GetSingle().GetMakeTeamBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			}
			LookFriendInfoDlg::GetSingle().GetPingBiInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			LookFriendInfoDlg::GetSingle().GetFriendInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			LookFriendInfoDlg::GetSingle().SetPlayerProcessControl(pControl);
			LookFriendInfoDlg::GetSingle().AdjustWindow();
			LookFriendInfoDlg::GetSingle().Show();
		}
		return FALSE;
	case WM_MOUSEWHEEL:
		{
			infoDlg.PlayerInfoDlgProcessMouseWheel(hwnd , wParam , lParam);
		}
		return FALSE;
	case WM_LBUTTONDBLCLK:
		{
			
			infoDlg.PlayerInfoProcessLButtonDBCLK(hwnd,wParam,lParam);
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

LRESULT CALLBACK ChatWndProcessFun::ProcessDeleteFriendListButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->deletePlayerButton;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoProcessDeleteButtonDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessAddFriendListButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->addPlayerButton;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoProcessAddButtonDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

LRESULT CALLBACK ChatWndProcessFun::ProcessShowLeftButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->showLeftButton;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
		/*	if(state)
			if(state != _CHAT_BUTTON_STATE_MOUSEOVER)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}*/
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
		/*	if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}*/
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoProcessShowLeftDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

void CALLBACK ChatWndProcessFun::EditCursorFlashProc(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime)
{
	if(B2ChatDialog::chatManager.ChatManagerGetEditBox()->showCursor)
	{
		B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->showCarat(false,false);
		B2ChatDialog::chatManager.ChatManagerGetEditBox()->showCursor = false;
		B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
	}
	else
	{
		B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut->showCarat(true,false);
		B2ChatDialog::chatManager.ChatManagerGetEditBox()->showCursor = true;
		B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
	}
}

LRESULT CALLBACK ChatWndProcessFun::ProcessLookInfoPersonalButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = LookFriendInfoDlg::GetSingle().GetTalkPersonalBtn();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			LookFriendInfoDlg::GetSingle().ProcessTalkPersonalLButtonDown();
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			LookFriendInfoDlg::GetSingle().ProcessKillFocus();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessLookInfoMakeTeamButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = LookFriendInfoDlg::GetSingle().GetMakeTeamBtn();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			LookFriendInfoDlg::GetSingle().ProcessKillFocus();
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			LookFriendInfoDlg::GetSingle().ProcessMakeTeamLButtonDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

LRESULT CALLBACK ChatWndProcessFun::ProcessLookInfoDeleteButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = LookFriendInfoDlg::GetSingle().GetDeleteBtn();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			LookFriendInfoDlg::GetSingle().ProcessKillFocus();
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			LookFriendInfoDlg::GetSingle().ProcessDeleteLButtonDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessLookInfoLookButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = LookFriendInfoDlg::GetSingle().GetLookInfoBtn();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			LookFriendInfoDlg::GetSingle().ProcessKillFocus();
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
//			LookFriendInfoDlg::GetSingle().ProcessDeleteLButtonDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

LRESULT CALLBACK ChatWndProcessFun::ProcessLookInfoPingBi(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = LookFriendInfoDlg::GetSingle().GetPingBiInfoBtn();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			LookFriendInfoDlg::GetSingle().ProcessKillFocus();
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			LookFriendInfoDlg::GetSingle().ProcessPingBiLButtonDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessLookInfoFriendButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = LookFriendInfoDlg::GetSingle().GetFriendInfoBtn();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			LookFriendInfoDlg::GetSingle().ProcessKillFocus();
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			LookFriendInfoDlg::GetSingle().ProcessFriendInfoLButtonDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

BOOL CALLBACK ChatWndProcessFun::LookFriendInfoDlgProc(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{

		}
		return TRUE;
	case WM_KILLFOCUS:
		{
			LookFriendInfoDlg::GetSingle().ProcessKillFocus();
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK ChatWndProcessFun::ChatMiniMapDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			ChatMiniMap::GetSingle().ProcessPaint(hwnd,hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			ChatMiniMap::GetSingle().ProcessPaint(hwnd,(HDC)wParam);
		}
		return true;
	case WM_LBUTTONDOWN:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			int flags = 0;
			if(wParam&MK_CONTROL)
				flags = CONTROL_PUSHED;
			ChatMiniMap::GetSingle().ProcessLButtonDown(pt,flags);
		}
		return FALSE;
	case WM_DESTROY:
		{
			KillTimer(hwnd,UPDATA_TIMER_ID);
		}
		return FALSE;
	}
	return FALSE;
}

VOID CALLBACK ChatWndProcessFun::ChatMiniMapUpataTimeProc(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime)
{
	if(ChatMiniMap::GetSingle().GetUptata())
	{
		InvalidateRect(hwnd,NULL,TRUE);
		UpdateWindow(hwnd);
		ChatMiniMap::GetSingle().UpdateCmpt();
	}
}

LRESULT CALLBACK ChatWndProcessFun::ProcessMiniMapPathFindBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatMiniMap::GetSingle().GetPathFindBtn();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatMiniMap::GetSingle().ProcessPathFindBtnDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

LRESULT CALLBACK ChatWndProcessFun::ProcessMiniMapKeyJinglinBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatMiniMap::GetSingle().GetKeyJinglinBtn();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatMiniMap::GetSingle().ProcessKeyJinglinBtnDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);


}

LRESULT CALLBACK ChatWndProcessFun::ProcessMiniMapCurrentMapBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatMiniMap::GetSingle().GetCurrentWordBtn();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatMiniMap::GetSingle().ProcessCurrentMapBtnDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

LRESULT CALLBACK ChatWndProcessFun::ProcessMiniMapBigWordMapBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatMiniMap::GetSingle().GetBigWordMapBtn();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatMiniMap::GetSingle().ProcessBigWodMapBtnDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);


}

LRESULT CALLBACK ChatWndProcessFun::ProcessMiniMapShowPlayerBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatMiniMap::GetSingle().GetShowPlayer();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatMiniMap::GetSingle().ProcessPlayerShpwBtnDown();
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);


}

LRESULT CALLBACK ChatWndProcessFun::ProcessMiniMapFindTeamBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatMiniMap::GetSingle().GetFindTeamBtn( );
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatMiniMap::GetSingle().ProcessFindTeamBtnDown( );
		}
		return FALSE;
	}
	
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

BOOL CALLBACK ChatWndProcessFun::ChatPlayerBaseInfoDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			ChatPlayerBaseInfoDlg::GetSingle().ProcessPaint(hwnd,hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			ChatPlayerBaseInfoDlg::GetSingle().ProcessPaint(hwnd,(HDC)wParam);

		}
		return TRUE;
	}
	return FALSE;
}

void CALLBACK ChatWndProcessFun::ChatPlayerBaseInfoDlgTimerProc(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime)
{
	ChatPlayerBaseInfoDlg& playerBaseDlg = ChatPlayerBaseInfoDlg::GetSingle();
	g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE,(unsigned int)&ChatPlayerBaseInfoDlg::GetSingle().GetAttribute(),NULL);
	g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO,(unsigned int)&ChatPlayerBaseInfoDlg::GetSingle().GetBaseInfo(),NULL);

	g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO,(unsigned int)&ChatPlayerBaseInfoDlg::GetSingle().GetRunTimeInfo(),NULL);

	g_pCoreShell->GetGameData(GDI_GET_SOCIETY,(unsigned int)&ChatPlayerBaseInfoDlg::GetSingle().GetSocielTY(),NULL);
	InvalidateRect(hwnd,NULL,TRUE);
	UpdateWindow(hwnd);
	
}

LRESULT CALLBACK ChatWndProcessFun:: ProcessPlayerLifeShowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatPlayerBaseInfoDlg::GetSingle().GetLifeShowControl();
	switch(msg)
	{
	case WM_PAINT:
		{
			ChatResource& resource = ChatResource::GetSingle();
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
//			button.ChatWndDrawItem(hdc);
			ChatPlayerBaseInfoDlg::GetSingle().DrawLifeShowControl(hdc,resource.GetResource(button.ChatWndGetResource().normal_idx)->hBitmap,
				resource.GetResource(button.ChatWndGetResource().hover_idx)->hBitmap);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
/*			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);*/
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
		/*	if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(FALSE);
			}*/
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
		/*	if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}*/
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessPlayerManaShowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatPlayerBaseInfoDlg::GetSingle().GetManaShowControl();
	switch(msg)
	{
	case WM_PAINT:
		{
			ChatResource& resource = ChatResource::GetSingle();
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
//			button.ChatWndDrawItem(hdc);
			ChatPlayerBaseInfoDlg::GetSingle().DrawManaShowControl(hdc,resource.GetResource(button.ChatWndGetResource().normal_idx)->hBitmap,
				resource.GetResource(button.ChatWndGetResource().hover_idx)->hBitmap);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
/*			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);*/
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
	/*		if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(FALSE);
			}*/
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
	/*		if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}*/
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessPlayerExpShowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatButton& button = ChatPlayerBaseInfoDlg::GetSingle().GetExpShowControl();
	switch(msg)
	{
	case WM_PAINT:
		{
			ChatResource& resource = ChatResource::GetSingle();
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
//			button.ChatWndDrawItem(hdc);
			ChatPlayerBaseInfoDlg::GetSingle().DrawExpShowControl(hdc,resource.GetResource(button.ChatWndGetResource().normal_idx)->hBitmap,
				resource.GetResource(button.ChatWndGetResource().hover_idx)->hBitmap);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=200;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			if(button.ChatWndTipEnable())
			{
				
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(FALSE);
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				if(IsWindowVisible(button.ChatWndGetTipHandle())==false)
				{
					ChatPlayerBaseInfoDlg::GetSingle().UpdateExpShowTip();
					button.ChatWndSetTipPos();
					button.ChatWndShowTip(TRUE);
				}
				
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	}

	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);

}

//委托界面切换按钮
LRESULT CALLBACK ChatWndProcessFun::ProcessEntrustChangeButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = EntrustComputerDlg::EntrustDlgGetSingleton().m_wndChangeWndBnt;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=500;
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
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatMainDlg::MainDlgShowWndChat(FALSE);	
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			if(!IsWindowVisible(KWin32App::m_hMainWnd))
			{
				RECT destRect;
				GetClientRect(GetDesktopWindow(),&destRect);
				RECT gameWindow;
				GetWindowRect(KWin32App::m_hMainWnd,&gameWindow);
				int gameWindowWidth = gameWindow.right - gameWindow.left;
				int gameWindowHeight = gameWindow.bottom - gameWindow.top;
				int gameWindowX = (destRect.right - gameWindowWidth)/2;
				int gameWindowY = (destRect.bottom - gameWindowHeight)/2;
				MoveWindow(KWin32App::m_hMainWnd,gameWindowX,gameWindowY,gameWindowWidth,gameWindowHeight,true);
				ShowWindow(KWin32App::m_hMainWnd,SW_NORMAL);
			}
			KUiChannelCentre::Show();
			KUiMiniMap::Show();
			Shell_NotifyIcon(NIM_DELETE,&ChatMainDlg::taskInfo);
			BringWindowToTop(KWin32App::m_hMainWnd);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
			ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
			KUiChannelCentre::GetSingleton().showSystemFrame(true);
			KWin32Frame::s_minisized = false;
			
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;

	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}
//委托界面隐藏/显示按钮
LRESULT CALLBACK ChatWndProcessFun::ProcessEntrustHideShowButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = EntrustComputerDlg::EntrustDlgGetSingleton().m_wndHideShowBnt;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=500;
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
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				if(IsWindowVisible(KWin32App::m_hMainWnd))
				{
					button.ChatWndUpdataTipText(ChatString::ChatStringGetString().hideGameWnd);
				}
				else
					button.ChatWndUpdataTipText(ChatString::ChatStringGetString().showGameWnd);
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
			else
			{
				if(IsWindowVisible(KWin32App::m_hMainWnd))
				{
					button.ChatWndTipCreate(TTS_NOPREFIX, ChatString::ChatStringGetString().hideGameWnd, 100);
				}
				else
				{
					button.ChatWndTipCreate(TTS_NOPREFIX, ChatString::ChatStringGetString().showGameWnd, 100);
				}
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(B2ChatDialog::chatManager.isShowMainWnd)
			{
					Shell_NotifyIcon(NIM_ADD,&ChatMainDlg::taskInfo);
					ShowWindow(KWin32App::m_hMainWnd,SW_HIDE);
					KWin32Frame::s_minisized = true;
					B2ChatDialog::chatManager.isShowMainWnd = FALSE;
			}else
			{
				Shell_NotifyIcon(NIM_DELETE,&ChatMainDlg::taskInfo);
				ShowWindow(KWin32App::m_hMainWnd,SW_NORMAL);
				RECT rc;
				::GetWindowRect(KWin32App::m_hMainWnd,&rc);
				RECT rc1;
				::GetWindowRect(ChatMainDlg::hMainDlg,&rc1);
				HWND hDesk = GetDesktopWindow();
				RECT destRc;
				GetClientRect(hDesk,&destRc);
				int widthMain = rc.right - rc.left;
				int widthMain1 = rc1.right - rc1.left;
				int heightMain = rc.bottom - rc.top;
				int heightMain1 = rc1.bottom - rc.top;
				int x = (destRc.right-(widthMain+widthMain1))/2;
				int y = (destRc.bottom - (heightMain))/2;
				MoveWindow(KWin32App::m_hMainWnd,x,y,widthMain,heightMain,TRUE);
				MoveWindow(ChatMainDlg::hMainDlg,x+widthMain,y,widthMain1,heightMain1,TRUE);
				KWin32Frame::s_minisized = false;
				B2ChatDialog::chatManager.isShowMainWnd = TRUE;
			}
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;

	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}
//好友界面切换按钮
LRESULT CALLBACK ChatWndProcessFun::ProcessFriendChangeButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg() ->m_wndChangeBnt;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=500;
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
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatMainDlg::MainDlgShowWndChat(FALSE);	
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			if(!IsWindowVisible(KWin32App::m_hMainWnd))
			{
				RECT destRect;
				GetClientRect(GetDesktopWindow(),&destRect);
				RECT gameWindow;
				GetWindowRect(KWin32App::m_hMainWnd,&gameWindow);
				int gameWindowWidth = gameWindow.right - gameWindow.left;
				int gameWindowHeight = gameWindow.bottom - gameWindow.top;
				int gameWindowX = (destRect.right - gameWindowWidth)/2;
				int gameWindowY = (destRect.bottom - gameWindowHeight)/2;
				MoveWindow(KWin32App::m_hMainWnd,gameWindowX,gameWindowY,gameWindowWidth,gameWindowHeight,true);
				ShowWindow(KWin32App::m_hMainWnd,SW_NORMAL);
			}
			KUiChannelCentre::Show();
			KUiMiniMap::Show();
			Shell_NotifyIcon(NIM_DELETE,&ChatMainDlg::taskInfo);
			BringWindowToTop(KWin32App::m_hMainWnd);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
			ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
			KUiChannelCentre::GetSingleton().showSystemFrame(true);
			KWin32Frame::s_minisized = false;
			
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;

	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}
//好友界面隐藏/显示按钮
LRESULT CALLBACK ChatWndProcessFun::ProcessFriendHideShowButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg() ->m_wndHideShowBnt;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=500;
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
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				if(IsWindowVisible(KWin32App::m_hMainWnd))
				{
					button.ChatWndUpdataTipText(ChatString::ChatStringGetString().hideGameWnd);
				}
				else
					button.ChatWndUpdataTipText(ChatString::ChatStringGetString().showGameWnd);
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
			else
			{
				if(IsWindowVisible(KWin32App::m_hMainWnd))
				{
					button.ChatWndTipCreate(TTS_NOPREFIX, ChatString::ChatStringGetString().hideGameWnd, 100);
				}
				else
				{
					button.ChatWndTipCreate(TTS_NOPREFIX, ChatString::ChatStringGetString().showGameWnd, 100);
				}
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(B2ChatDialog::chatManager.isShowMainWnd)
			{
					Shell_NotifyIcon(NIM_ADD,&ChatMainDlg::taskInfo);
					ShowWindow(KWin32App::m_hMainWnd,SW_HIDE);
					KWin32Frame::s_minisized = true;
					B2ChatDialog::chatManager.isShowMainWnd = FALSE;
			}else
			{
				Shell_NotifyIcon(NIM_DELETE,&ChatMainDlg::taskInfo);
				ShowWindow(KWin32App::m_hMainWnd,SW_NORMAL);
				RECT rc;
				::GetWindowRect(KWin32App::m_hMainWnd,&rc);
				RECT rc1;
				::GetWindowRect(ChatMainDlg::hMainDlg,&rc1);
				HWND hDesk = GetDesktopWindow();
				RECT destRc;
				GetClientRect(hDesk,&destRc);
				int widthMain = rc.right - rc.left;
				int widthMain1 = rc1.right - rc1.left;
				int heightMain = rc.bottom - rc.top;
				int heightMain1 = rc1.bottom - rc.top;
				int x = (destRc.right-(widthMain+widthMain1))/2;
				int y = (destRc.bottom - (heightMain))/2;
				MoveWindow(KWin32App::m_hMainWnd,x,y,widthMain,heightMain,TRUE);
				MoveWindow(ChatMainDlg::hMainDlg,x+widthMain,y,widthMain1,heightMain1,TRUE);
				KWin32Frame::s_minisized = false;
				B2ChatDialog::chatManager.isShowMainWnd = TRUE;
			}
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;

	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

//氏族面板切换	
LRESULT CALLBACK ChatWndProcessFun::ProcessClanListButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
/*
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			_TrackMouseEvent(&tme);

			int state = button.ChatWndGetState();
			if(state == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
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
			if(state  == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(button.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN||
				button.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(button.ChatWndGetState() != _CHAT_BUTTON_STATE_MOUSEDOWN )
			{
				ChatButton& tempButton = ChatClanManager::GetManager().m_ClanPanel.GetButtonByIdx(ChatClanManager::GetManager().m_ClanPanel.currentIdx);
				tempButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				tempButton.ChatWndUpdate();
				ChatClanManager::GetManager().m_ClanPanel.currentIdx = _CLAN_INFO_DLG_CLAN;
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
				button.ChatWndUpdate();
				ChatClanManager::GetManager().HideCurrentDlg();
				ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
				ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
				ChatClanManager::GetManager().m_pCurrentDlg ->ClearSelected(FALSE);
				ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
				ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
				ChatClanManager::GetManager().SetCurrnetDlg(ChatClanManager::GetManager().m_pInfoDlg[_CLAN_INFO_DLG_CLAN]);
				ChatClanManager::GetManager().m_pCurrentDlg ->PlayerInfoDlgShowDlg(TRUE);
			}
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);*/
	return FALSE;
}

LRESULT CALLBACK ChatWndProcessFun::ProcessLuedListButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
/*	ChatButton & button = ChatClanManager::GetManager().m_ClanPanel.GetButtonByIdx(_CLAN_INFO_DLG_LUED);
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
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
			if(state  == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(button.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN||
				button.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(button.ChatWndGetState() != _CHAT_BUTTON_STATE_MOUSEDOWN )
			{
				ChatButton& tempButton = ChatClanManager::GetManager().m_ClanPanel.GetButtonByIdx(ChatClanManager::GetManager().m_ClanPanel.currentIdx);
				tempButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				tempButton.ChatWndUpdate();
				ChatClanManager::GetManager().m_ClanPanel.currentIdx = _CLAN_INFO_DLG_LUED;
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
				button.ChatWndUpdate();
				ChatClanManager::GetManager().HideCurrentDlg();
				ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
				ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
				ChatClanManager::GetManager().m_pCurrentDlg ->ClearSelected(FALSE);
				ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
				ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
				ChatClanManager::GetManager().SetCurrnetDlg(ChatClanManager::GetManager().m_pInfoDlg[_CLAN_INFO_DLG_LUED]);
				ChatClanManager::GetManager().m_pCurrentDlg ->PlayerInfoDlgShowDlg(TRUE);
			}
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);*/
	return FALSE;
}

LRESULT CALLBACK ChatWndProcessFun::ProcessNationListButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
/*	ChatButton & button = ChatClanManager::GetManager().m_ClanPanel.GetButtonByIdx(_CLAN_INFO_DLG_NATION);
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
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
			if(state  == _CHAT_BUTTON_STATE_DISABLE||
				state == _CHAT_BUTTON_STATE_MOUSEDOWN)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(button.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN||
				button.ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(button.ChatWndGetState() != _CHAT_BUTTON_STATE_MOUSEDOWN )
			{
					ChatButton& tempButton = ChatClanManager::GetManager().m_ClanPanel.GetButtonByIdx(ChatClanManager::GetManager().m_ClanPanel.currentIdx);
				tempButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				tempButton.ChatWndUpdate();
				ChatClanManager::GetManager().m_ClanPanel.currentIdx = _CLAN_INFO_DLG_NATION;
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
				button.ChatWndUpdate();
				ChatClanManager::GetManager().HideCurrentDlg();
				ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
				ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
				ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
				ChatClanManager::GetManager().m_pCurrentDlg ->ClearSelected(FALSE);
				ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
				ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
				ChatClanManager::GetManager().SetCurrnetDlg(ChatClanManager::GetManager().m_pInfoDlg[_CLAN_INFO_DLG_NATION]);
				ChatClanManager::GetManager().m_pCurrentDlg ->PlayerInfoDlgShowDlg(TRUE);
			}
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);*/
	return FALSE;
}

//氏族分页
BOOL CALLBACK ChatWndProcessFun::ProcessClanProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatClanInfoDlg * infoDlg = (ChatClanInfoDlg *)&ChatClanManager::GetManager().m_ClanInfoDlg;
	if (infoDlg == NULL)
		return FALSE;
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			ChatClanPopMenu::GetMenu().CreateMenu(hwnd, ChatWndProcessFun::ClanPopMenuProc);
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC  hdc = BeginPaint(hwnd,&ps);
			infoDlg ->PaintDLG(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			infoDlg ->PaintDLG((HDC)wParam);
		}
		return TRUE;
	case WM_MOUSEMOVE:
		{
			infoDlg ->OnMouseMove(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			infoDlg ->OnLButtonDown(hwnd, wParam, lParam);
			SetFocus(hwnd);
		}
		return FALSE;
	case WM_LBUTTONUP:
		{
			infoDlg ->OnLButtonUp(hwnd,wParam,lParam);
			SetFocus(ChatControlPanel::ChatPanelGetPanel().hPanelDlg);
		}
		return FALSE;
	case WM_MOUSEWHEEL:
		{
			infoDlg ->PlayerInfoDlgProcessMouseWheel(hwnd , wParam , lParam);
		}
		return FALSE;
	case WM_LBUTTONDBLCLK:
		{
			infoDlg ->PlayerInfoProcessLButtonDBCLK(hwnd,wParam,lParam);
			SetFocus(hwnd);
		}
		return FALSE;
	case WM_RBUTTONUP:
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(infoDlg ->hDlg, &pt);
			infoDlg ->ClearSelected(TRUE);
			ChatClanListControl * pControl = NULL;
			pControl = infoDlg ->GetControlByPoint(pt);
			if (pControl == NULL)
			{
				return FALSE;
			}

			SocietyInfoIndex tagSocietyIdx;
			tagSocietyIdx.TemplateId       = enSUTplId_Tong;
			tagSocietyIdx.Layer            = enSULayer_Gens;
			tagSocietyIdx.Operation        = enSUO_ChangeOwner;

			pControl ->SetSelected(TRUE, TRUE);
			ChatClanPopMenu::GetMenu().SetClanListControl(pControl);
			ChatClanPopMenu::GetMenu().GetAddFriendButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			if (pControl ->m_playerData.bOnline)
			{
				ChatClanPopMenu::GetMenu().GetInviteTeamButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				ChatClanPopMenu::GetMenu().GetPrivateChatButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				ChatClanPopMenu::GetMenu().GetParticulInfoButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);

				if (g_pCoreShell ->GetGameData(GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL))
					ChatClanPopMenu::GetMenu().GetDemiseButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				else
					ChatClanPopMenu::GetMenu().GetDemiseButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			}
			else
			{
				ChatClanPopMenu::GetMenu().GetInviteTeamButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				ChatClanPopMenu::GetMenu().GetPrivateChatButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				ChatClanPopMenu::GetMenu().GetParticulInfoButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				ChatClanPopMenu::GetMenu().GetDemiseButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			}

			tagSocietyIdx.Operation        = enSUO_RemoveSubUnit;
			if (g_pCoreShell ->GetGameData(GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL))
				ChatClanPopMenu::GetMenu().GetFireButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			else
				ChatClanPopMenu::GetMenu().GetFireButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			ChatClanPopMenu::GetMenu().AdjustWindow();
			ChatClanPopMenu::GetMenu().ShowMenu();
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
			infoDlg ->ClearAll();
		}
		return FALSE;
	case WM_SHOWWINDOW:
		{
			if (!IsWindowVisible(hwnd))
			{
				ChatClanManager::GetManager().ClearAll();
				if (ChatClanPlayerData::GetPlayerData().InitInterface())
				{
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_ADD_MEMBER].ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_ADD_MEMBER].SetDisableColor();
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_MODIFY_BULLETIN].ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_MODIFY_BULLETIN].SetDisableColor();
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_DELETE_MEMBER].ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_DELETE_MEMBER].SetDisableColor();
					ChatClanPlayerData::GetPlayerData().RequestDataList(hwnd, enSULayer_Gens);
				}
			}
		}
		return FALSE;
	case WM_DATA_REQUEST_SUCCEED:
		{
			ChatClanManager::GetManager().UpdateMemberList();
			ChatClanPlayerData::GetPlayerData().GetAnnoucement(enSULayer_Gens);

			SocietyInfoIndex tagSocietyIdx;
			tagSocietyIdx.TemplateId       = enSUTplId_Tong;
			tagSocietyIdx.Layer            = enSULayer_Gens;
			tagSocietyIdx.Operation        = enSUO_AddSubUnit;

			if (g_pCoreShell ->GetGameData(GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL))
			{
				infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_ADD_MEMBER].ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_ADD_MEMBER].SetEnableColor();
			}

			tagSocietyIdx.Operation        = enSUO_PubAnnouncement;
			if (g_pCoreShell ->GetGameData(GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL))
			{
				infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_MODIFY_BULLETIN].ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_MODIFY_BULLETIN].SetEnableColor();
			}

			tagSocietyIdx.Operation        = enSUO_RemoveSubUnit;
			if (g_pCoreShell ->GetGameData(GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL))
			{
				infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_DELETE_MEMBER].ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_DELETE_MEMBER].SetEnableColor();
			}

			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}
		return FALSE;
	}

	return FALSE;
}

BOOL CALLBACK ChatWndProcessFun::ProcessLuedProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ChatLuedInfoDlg * infoDlg = (ChatLuedInfoDlg *)&ChatLuedManager::GetManager().m_LuedInfoDlg;
	if (infoDlg == NULL)
		return FALSE;
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			ChatLuedPopMenu::GetMenu().CreateMenu(hwnd, ChatWndProcessFun::LuedPopMenuProc);
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC  hdc = BeginPaint(hwnd,&ps);
			infoDlg ->PaintDLG(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			infoDlg ->PaintDLG((HDC)wParam);
		}
		return TRUE;
	case WM_MOUSEMOVE:
		{
			infoDlg ->OnMouseMove(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			infoDlg ->OnLButtonDown(hwnd, wParam, lParam);
			ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
			SetFocus(hwnd);
		}
		return FALSE;
	case WM_LBUTTONUP:
		{
			infoDlg ->OnLButtonUp(hwnd,wParam,lParam);
			SetFocus(ChatControlPanel::ChatPanelGetPanel().hPanelDlg);
		}
		return FALSE;
	case WM_MOUSEWHEEL:
		{
			infoDlg ->PlayerInfoDlgProcessMouseWheel(hwnd , wParam , lParam);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatClanComboBox::GetClanComboBox().ShowDownDialog(FALSE);
		}
		return FALSE;
	case WM_LBUTTONDBLCLK:
		{
			infoDlg ->PlayerInfoProcessLButtonDBCLK(hwnd,wParam,lParam);
			ChatClanAnnouncementDlg::GetDlg().ShowDlg(FALSE);
			SetFocus(hwnd);
		}
		return FALSE;
	case WM_RBUTTONUP:
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(infoDlg ->hDlg, &pt);
			infoDlg ->ClearSelected(TRUE);
			ChatClanListControl * pControl = NULL;
			pControl = infoDlg ->GetControlByPoint(pt);
			if (pControl == NULL)
			{
				return FALSE;
			}

			SocietyInfoIndex tagSocietyIdx;
			tagSocietyIdx.TemplateId       = enSUTplId_Tong;
			tagSocietyIdx.Layer            = enSULayer_Tong;
			tagSocietyIdx.Operation        = enSUO_ChangeOwner;

			pControl ->SetSelected(TRUE, TRUE);
			ChatLuedPopMenu::GetMenu().SetClanListControl(pControl);
			ChatLuedPopMenu::GetMenu().GetAddFriendButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			if (pControl ->m_playerData.bOnline)
			{
				ChatLuedPopMenu::GetMenu().GetInviteTeamButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				ChatLuedPopMenu::GetMenu().GetPrivateChatButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				ChatLuedPopMenu::GetMenu().GetParticulInfoButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);

				if (g_pCoreShell ->GetGameData(GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL))
					ChatLuedPopMenu::GetMenu().GetDemiseButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				else
					ChatLuedPopMenu::GetMenu().GetDemiseButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			}
			else
			{
				ChatLuedPopMenu::GetMenu().GetInviteTeamButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				ChatLuedPopMenu::GetMenu().GetPrivateChatButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				ChatLuedPopMenu::GetMenu().GetParticulInfoButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				ChatLuedPopMenu::GetMenu().GetDemiseButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			}

			tagSocietyIdx.Operation = enSUO_ForbidChat;
			if (g_pCoreShell ->GetGameData(GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL))
			{
				if (!pControl ->m_playerData.bPreventChatState[1])
				{
					ShowWindow(ChatLuedPopMenu::GetMenu().GetForbidChatButton().ChatWndGetHandle(), TRUE);
					ChatLuedPopMenu::GetMenu().GetForbidChatButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					ShowWindow(ChatLuedPopMenu::GetMenu().GetUnforbidChatButton().ChatWndGetHandle(), FALSE);
				}
				else
				{
					ShowWindow(ChatLuedPopMenu::GetMenu().GetForbidChatButton().ChatWndGetHandle(), FALSE);
					ShowWindow(ChatLuedPopMenu::GetMenu().GetUnforbidChatButton().ChatWndGetHandle(), TRUE);
					ChatLuedPopMenu::GetMenu().GetUnforbidChatButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				}
			}
			else
			{
				ChatLuedPopMenu::GetMenu().GetForbidChatButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				ShowWindow(ChatLuedPopMenu::GetMenu().GetUnforbidChatButton().ChatWndGetHandle(), FALSE);
			}

			tagSocietyIdx.Operation = enSUO_UnForbidChat;
			if (g_pCoreShell ->GetGameData(GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL))
			{
				if (!pControl ->m_playerData.bPreventChatState[1])
				{
					ShowWindow(ChatLuedPopMenu::GetMenu().GetForbidChatButton().ChatWndGetHandle(), TRUE);
					ChatLuedPopMenu::GetMenu().GetForbidChatButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					ShowWindow(ChatLuedPopMenu::GetMenu().GetUnforbidChatButton().ChatWndGetHandle(), FALSE);
				}
				else
				{
					ShowWindow(ChatLuedPopMenu::GetMenu().GetForbidChatButton().ChatWndGetHandle(), FALSE);
					ShowWindow(ChatLuedPopMenu::GetMenu().GetUnforbidChatButton().ChatWndGetHandle(), TRUE);
					ChatLuedPopMenu::GetMenu().GetUnforbidChatButton().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				}
			}
			else
			{
				ChatLuedPopMenu::GetMenu().GetForbidChatButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				ChatLuedPopMenu::GetMenu().GetUnforbidChatButton().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			}

			ChatLuedPopMenu::GetMenu().AdjustWindow();
			ChatLuedPopMenu::GetMenu().ShowMenu();
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
			infoDlg ->ClearAll();
		}
		return FALSE;
	case WM_SHOWWINDOW:
		{
			if (!IsWindowVisible(hwnd))
			{
				ChatLuedManager::GetManager().ClearAll();
				if (ChatClanPlayerData::GetPlayerData().InitInterface())
				{
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_ADD_MEMBER].ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_ADD_MEMBER].SetDisableColor();
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_MODIFY_BULLETIN].ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_MODIFY_BULLETIN].SetDisableColor();
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_DELETE_MEMBER].ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
					infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_DELETE_MEMBER].SetDisableColor();
					ChatClanPlayerData::GetPlayerData().RequestDataList(ChatClanComboBox::GetClanComboBox().m_hDownDialg, enSULayer_Tong);
				}
			}
			else
			{
				ChatClanComboBox::GetClanComboBox().ClearAll();
				ChatClanAnnouncementDlg::GetDlg().ShowDlg(FALSE);
			}
		}
		return FALSE;
	case WM_DATA_REQUEST_SUCCEED:
		{
			ChatLuedManager::GetManager().UpdateMemberList();
			ChatClanPlayerData::GetPlayerData().GetAnnoucement(enSULayer_Tong);
			SocietyInfoIndex tagSocietyIdx;
			tagSocietyIdx.TemplateId       = enSUTplId_Tong;
			tagSocietyIdx.Layer            = enSULayer_Tong;
			tagSocietyIdx.Operation        = enSUO_AddSubUnit;

			if (g_pCoreShell ->GetGameData(GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL))
			{
				infoDlg ->m_ButtenList[_CHAT_LUED_INFO_DLG_ADD_CLAN].ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				infoDlg ->m_ButtenList[_CHAT_LUED_INFO_DLG_ADD_CLAN].SetEnableColor();
			}

			tagSocietyIdx.Operation        = enSUO_PubAnnouncement;
			if (g_pCoreShell ->GetGameData(GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL))
			{
				infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_MODIFY_BULLETIN].ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				infoDlg ->m_ButtenList[_CHAT_CLAN_INFO_DLG_MODIFY_BULLETIN].SetEnableColor();
			}

			tagSocietyIdx.Operation        = enSUO_RemoveSubUnit;
			if (g_pCoreShell ->GetGameData(GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL))
			{
				infoDlg ->m_ButtenList[_CHAT_LUED_INFO_DLG_DELETE_CLAN].ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				infoDlg ->m_ButtenList[_CHAT_LUED_INFO_DLG_DELETE_CLAN].SetEnableColor();
			}

			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}
	}

	return FALSE;
}

BOOL CALLBACK ChatWndProcessFun::ProcessNationProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
/*	ChatClanInfoDlg * infoDlg= (ChatClanInfoDlg *)ChatClanManager::GetManager().m_pInfoDlg[_CLAN_INFO_DLG_NATION];
	if (infoDlg == NULL)
		return FALSE;
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_PAINT:
		{
	//		ShowWindow(hwnd,SW_HIDE);
			PAINTSTRUCT ps;
			HDC  hdc = BeginPaint(hwnd,&ps);
			infoDlg ->PaintDLG(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
//			infoDlg.PlayerInfoDlgProcessPaint((HDC)wParam);
		}
		return TRUE;
	case WM_MOUSEMOVE:
		{
//			infoDlg.PlayerInfoDlgProcessMouseMove(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
//			infoDlg.PlayerInfoDlgProcessMouseLButtonDown(hwnd,wParam,lParam);
			SetFocus(hwnd);
		}
		return FALSE;
	case WM_LBUTTONUP:
		{
//			infoDlg.PlayerInfoDlgProcessMouseLButtonUp(hwnd,wParam,lParam);
			SetFocus(ChatControlPanel::ChatPanelGetPanel().hPanelDlg);
		}
		return FALSE;
	case WM_MOUSEWHEEL:
		{
//			infoDlg.PlayerInfoDlgProcessMouseWheel(hwnd , wParam , lParam);
		}
		return FALSE;
	case WM_LBUTTONDBLCLK:
		{
//			infoDlg.PlayerInfoProcessLButtonDBCLK(hwnd,wParam,lParam);
	//		SetFocus(hwnd);
		}
		return FALSE;
	case WM_RBUTTONUP:
		{
/*			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->hDlg,&pt);
			LPPLAYERCONTROL pControl = ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoGetControlByPoint(pt);
			if(pControl==0)
				return FALSE;
			if(pControl->PlayerControlGetPlayerInfo().isPlayerOnline == false)
			{
				LookFriendInfoDlg::GetSingle().GetTalkPersonalBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				LookFriendInfoDlg::GetSingle().GetLookInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
				LookFriendInfoDlg::GetSingle().GetMakeTeamBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			}
			else
			{
				LookFriendInfoDlg::GetSingle().GetTalkPersonalBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				LookFriendInfoDlg::GetSingle().GetLookInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				LookFriendInfoDlg::GetSingle().GetMakeTeamBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			}
			LookFriendInfoDlg::GetSingle().GetDeleteBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			LookFriendInfoDlg::GetSingle().GetPingBiInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			LookFriendInfoDlg::GetSingle().GetFriendInfoBtn().ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			LookFriendInfoDlg::GetSingle().SetPlayerProcessControl(pControl);
			LookFriendInfoDlg::GetSingle().AdjustWindow();
			LookFriendInfoDlg::GetSingle().Show();
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
			infoDlg ->ClearAll();
		}
		return FALSE;

	}
*/
	return FALSE;
}

LRESULT CALLBACK ChatWndProcessFun::ProcessClanChangeButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanManager::GetManager().m_ClanInfoDlg.m_wndChangeBnt;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=500;
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
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatMainDlg::MainDlgShowWndChat(FALSE);	
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			if(!IsWindowVisible(KWin32App::m_hMainWnd))
			{
				RECT destRect;
				GetClientRect(GetDesktopWindow(),&destRect);
				RECT gameWindow;
				GetWindowRect(KWin32App::m_hMainWnd,&gameWindow);
				int gameWindowWidth = gameWindow.right - gameWindow.left;
				int gameWindowHeight = gameWindow.bottom - gameWindow.top;
				int gameWindowX = (destRect.right - gameWindowWidth)/2;
				int gameWindowY = (destRect.bottom - gameWindowHeight)/2;
				MoveWindow(KWin32App::m_hMainWnd,gameWindowX,gameWindowY,gameWindowWidth,gameWindowHeight,true);
				ShowWindow(KWin32App::m_hMainWnd,SW_NORMAL);
			}
			KUiChannelCentre::Show();
			KUiMiniMap::Show();
			Shell_NotifyIcon(NIM_DELETE,&ChatMainDlg::taskInfo);
			BringWindowToTop(KWin32App::m_hMainWnd);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
			ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
			KUiChannelCentre::GetSingleton().showSystemFrame(true);
			KWin32Frame::s_minisized = false;
			
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;

	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessClanHideShowButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanManager::GetManager().m_ClanInfoDlg.m_wndHideShowBnt;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=500;
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
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				if(IsWindowVisible(KWin32App::m_hMainWnd))
				{
					button.ChatWndUpdataTipText(ChatString::ChatStringGetString().hideGameWnd);
				}
				else
					button.ChatWndUpdataTipText(ChatString::ChatStringGetString().showGameWnd);
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
			else
			{
				if(IsWindowVisible(KWin32App::m_hMainWnd))
				{
					button.ChatWndTipCreate(TTS_NOPREFIX, ChatString::ChatStringGetString().hideGameWnd, 100);
				}
				else
				{
					button.ChatWndTipCreate(TTS_NOPREFIX, ChatString::ChatStringGetString().showGameWnd, 100);
				}
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(B2ChatDialog::chatManager.isShowMainWnd)
			{
				Shell_NotifyIcon(NIM_ADD,&ChatMainDlg::taskInfo);
				ShowWindow(KWin32App::m_hMainWnd,SW_HIDE);
				KWin32Frame::s_minisized = true;
				B2ChatDialog::chatManager.isShowMainWnd = FALSE;
			}else
			{
				Shell_NotifyIcon(NIM_DELETE,&ChatMainDlg::taskInfo);
				ShowWindow(KWin32App::m_hMainWnd,SW_NORMAL);
				RECT rc;
				::GetWindowRect(KWin32App::m_hMainWnd,&rc);
				RECT rc1;
				::GetWindowRect(ChatMainDlg::hMainDlg,&rc1);
				HWND hDesk = GetDesktopWindow();
				RECT destRc;
				GetClientRect(hDesk,&destRc);
				int widthMain = rc.right - rc.left;
				int widthMain1 = rc1.right - rc1.left;
				int heightMain = rc.bottom - rc.top;
				int heightMain1 = rc1.bottom - rc.top;
				int x = (destRc.right-(widthMain+widthMain1))/2;
				int y = (destRc.bottom - (heightMain))/2;
				MoveWindow(KWin32App::m_hMainWnd,x,y,widthMain,heightMain,TRUE);
				MoveWindow(ChatMainDlg::hMainDlg,x+widthMain,y,widthMain1,heightMain1,TRUE);
				KWin32Frame::s_minisized = false;
				B2ChatDialog::chatManager.isShowMainWnd = TRUE;
			}
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;

	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}
//氏族面板按钮
LRESULT CALLBACK ChatWndProcessFun::ProcessClanModifyBulletinButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanManager::GetManager().m_ClanInfoDlg.m_ButtenList[_CHAT_CLAN_INFO_DLG_MODIFY_BULLETIN];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER|TME_LEAVE;
			tme.dwHoverTime = 500;
			tme.hwndTrack = button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			
			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_MOUSEOVER)
			{	
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			int buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			if (IsWindowVisible(ChatClanAnnouncementDlg::GetDlg().m_hDlg))
			{
				ChatClanAnnouncementDlg::GetDlg().ShowDlg(FALSE);
			}
			else
			{
				ChatClanAnnouncementDlg::GetDlg().m_iLayerID = enSULayer_Gens;
				ChatClanAnnouncementDlg::GetDlg().ShowDlg(TRUE);
			}
			
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessClanAddMemberButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanManager::GetManager().m_ClanInfoDlg.m_ButtenList[_CHAT_CLAN_INFO_DLG_ADD_MEMBER];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER|TME_LEAVE;
			tme.dwHoverTime = 500;
			tme.hwndTrack = button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);

			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_MOUSEOVER)
			{	
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			ChatClanManager::GetManager().AddMemberButtonDown();
		}
	case WM_MOUSEHOVER:
		{
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ProcessClanDeleteMemberButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanManager::GetManager().m_ClanInfoDlg.m_ButtenList[_CHAT_CLAN_INFO_DLG_DELETE_MEMBER];
	switch(msg)
	{
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
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			ChatClanManager::GetManager().DeleteMemberButtonDown();
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();

			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER|TME_LEAVE;
			tme.dwHoverTime = 500;
			tme.hwndTrack = button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);

			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_MOUSEOVER)
			{	
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

//添加氏族成员对话框消息处理函数
BOOL CALLBACK ChatWndProcessFun::AddMemberDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG :
		{
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_DRAWITEM:
		{
			ChatClanManager::GetManager().addDlg.ChatHintDlgProcessDrawButton((LPDRAWITEMSTRUCT)lParam);
		}
		return FALSE;
	case WM_COMMAND:
		{
			ChatClanManager::GetManager().addDlg.OnCommand(ChatClanManager::GetManager().m_ClanInfoDlg.hDlg, wParam, lParam);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatClanManager::GetManager().addDlg.ChatHintDlgProcessMouseMove(hwnd, wParam, lParam);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatClanManager::GetManager().addDlg.ChatHintDlgProcessMouseLeave(hwnd, wParam, lParam);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			ChatClanManager::GetManager().addDlg.ChatHintDlgProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			ChatClanManager::GetManager().addDlg.ChatHintDlgProcessPaint((HDC)wParam);

		}
		return TRUE;
	case WM_DESTROY:
		{
			DeleteObject(ChatClanManager::GetManager().addDlg.hEditBrush);
		}
		break;
	case WM_CTLCOLOREDIT:
		{
			switch(GetWindowLong((HWND)lParam,GWL_ID))
			{
			case _CHAT_HINT_EDIT_ID:
				{
					SetBkMode((HDC)wParam, TRANSPARENT);
					SetTextColor((HDC)wParam, RGB(255, 230, 20));
					return (BOOL)ChatClanManager::GetManager().addDlg.hEditBrush;
				}
				break;
			}
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK ChatWndProcessFun::DeleteMemberDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG :
		{
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_DRAWITEM:
		{
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgProcessDrawButton((LPDRAWITEMSTRUCT)lParam);
		}
		return FALSE;
	case WM_COMMAND:
		{
			ChatClanManager::GetManager().deleteDlg.OnCommand(ChatClanManager::GetManager().m_ClanInfoDlg.hDlg,wParam,lParam);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgProcessMouseMove(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgProcessMouseLeave(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgProcessPaint((HDC)wParam);

		}
		return TRUE;
	case WM_DESTROY:
		{
			ChatClanManager::GetManager().deleteDlg.wndText[0] = 0;
			ChatClanManager::GetManager().deleteDlg.infoText[0] = 0;
			ChatClanManager::GetManager().deleteDlg.font[0] = 0;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK ChatWndProcessFun::ClanPopMenuProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{

		}
		return TRUE;
	case WM_KILLFOCUS:
		{
			ChatClanPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return FALSE;
}

LRESULT CALLBACK ChatWndProcessFun::ClanPopMenuAddFrienBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanPopMenu::GetMenu().GetAddFriendButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatClanPopMenu::GetMenu().AddFrienButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatClanPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ClanPopMenuInviteTeamBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanPopMenu::GetMenu().GetInviteTeamButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatClanPopMenu::GetMenu().InviteTeamButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatClanPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ClanPopMenuPrivateChatBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanPopMenu::GetMenu().GetPrivateChatButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatClanPopMenu::GetMenu().PrivateChatButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatClanPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ClanPopMenuParticularInfoBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanPopMenu::GetMenu().GetParticulInfoButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatClanPopMenu::GetMenu().ParticularInfoButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatClanPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ClanPopMenuDemiseBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanPopMenu::GetMenu().GetDemiseButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatClanPopMenu::GetMenu().DemiseButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatClanPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::ClanPopMenuFireBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatClanPopMenu::GetMenu().GetFireButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatClanPopMenu::GetMenu().FireButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatClanPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

BOOL CALLBACK ChatWndProcessFun::LuedPopMenuProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{

		}
		return TRUE;
	case WM_KILLFOCUS:
		{
			ChatLuedPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return FALSE;
}

LRESULT CALLBACK ChatWndProcessFun::LuedPopMenuAddFrienBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedPopMenu::GetMenu().GetAddFriendButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatLuedPopMenu::GetMenu().AddFrienButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatLuedPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::LuedPopMenuInviteTeamBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedPopMenu::GetMenu().GetInviteTeamButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatLuedPopMenu::GetMenu().InviteTeamButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatLuedPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::LuedPopMenuPrivateChatBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedPopMenu::GetMenu().GetPrivateChatButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatLuedPopMenu::GetMenu().PrivateChatButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatClanPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::LuedPopMenuParticularInfoBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedPopMenu::GetMenu().GetParticulInfoButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatLuedPopMenu::GetMenu().ParticularInfoButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatLuedPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::LuedPopMenuDemiseBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedPopMenu::GetMenu().GetDemiseButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatLuedPopMenu::GetMenu().DemiseButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatLuedPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::LuedPopMenuForbidBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedPopMenu::GetMenu().GetForbidChatButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			int state = button.ChatWndGetState();
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);
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
			int state = button.ChatWndGetState();
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(FALSE);
			}
			if(state  == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatLuedPopMenu::GetMenu().ForbidChatButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatClanPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::LuedPopMenuUnforbidBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedPopMenu::GetMenu().GetUnforbidChatButton();
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			int state = button.ChatWndGetState();
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=1000;
			tme.hwndTrack=button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);
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
			int state = button.ChatWndGetState();
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(FALSE);
			}
			if(state  == _CHAT_BUTTON_STATE_DISABLE)
				return FALSE;
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatLuedPopMenu::GetMenu().UnforbidChatButtonDown();
		}
		return FALSE;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_KILLFOCUS:
		{
			ChatClanPopMenu::GetMenu().ProcessKillFocus();
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::LuedModifyBulletinButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedManager::GetManager().m_LuedInfoDlg.m_ButtenList[_CHAT_LUED_INFO_DLG_MODIFY_BULLETIN];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER|TME_LEAVE;
			tme.dwHoverTime = 500;
			tme.hwndTrack = button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);
			
			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_MOUSEOVER)
			{	
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			int buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			if (IsWindowVisible(ChatClanAnnouncementDlg::GetDlg().m_hDlg))
			{
				ChatClanAnnouncementDlg::GetDlg().ShowDlg(FALSE);
			}
			else
			{
				ChatClanAnnouncementDlg::GetDlg().m_iLayerID = enSULayer_Tong;
				ChatClanAnnouncementDlg::GetDlg().ShowDlg(TRUE);
			}
			
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::LuedAddClanButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedManager::GetManager().m_LuedInfoDlg.m_ButtenList[_CHAT_LUED_INFO_DLG_ADD_CLAN];
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_MOUSEDOWN)
			{
				return FALSE;
			}
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER|TME_LEAVE;
			tme.dwHoverTime = 500;
			tme.hwndTrack = button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);

			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_MOUSEOVER)
			{	
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_MOUSEDOWN)
			{
				return FALSE;
			}
			else if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			ChatLuedManager::GetManager().AddClanButtonDown();
		}
	case WM_MOUSEHOVER:
		{
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_MOUSEDOWN)
			{
				return FALSE;
			}
			else if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::LuedDeleteClanButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedManager::GetManager().m_LuedInfoDlg.m_ButtenList[_CHAT_LUED_INFO_DLG_DELETE_CLAN];
	switch(msg)
	{
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
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_MOUSEDOWN)
			{
				return FALSE;
			}
			else if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			ChatLuedManager::GetManager().DeleteClanButtonDown();
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_MOUSEDOWN)
			{
				return FALSE;
			}

			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER|TME_LEAVE;
			tme.dwHoverTime = 500;
			tme.hwndTrack = button.ChatWndGetHandle();
			_TrackMouseEvent(&tme);

			if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_MOUSEOVER)
			{	
				button.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_MOUSEHOVER:
		{
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			int buttonState = 0;
			buttonState = button.ChatWndGetState();
			if (buttonState == _CHAT_BUTTON_STATE_MOUSEDOWN)
			{
				return FALSE;
			}
			else if (buttonState == _CHAT_BUTTON_STATE_DISABLE)
			{
				return FALSE;
			}
			else if (buttonState != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;
	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

BOOL CALLBACK ChatWndProcessFun::AddClanDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG :
		{
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_DRAWITEM:
		{
			ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgProcessDrawButton((LPDRAWITEMSTRUCT)lParam);
		}
		return FALSE;
	case WM_COMMAND:
		{
			ChatLuedManager::GetManager().m_AddClanDlg.OnCommand(ChatLuedManager::GetManager().m_LuedInfoDlg.hDlg, wParam, lParam);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgProcessMouseMove(hwnd, wParam, lParam);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgProcessMouseLeave(hwnd, wParam, lParam);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			ChatLuedManager::GetManager().m_AddClanDlg.ChatHintDlgProcessPaint((HDC)wParam);

		}
		return TRUE;
	case WM_DESTROY:
		{
			DeleteObject(ChatLuedManager::GetManager().m_AddClanDlg.hEditBrush);
		}
		break;
	case WM_CTLCOLOREDIT:
		{
			switch(GetWindowLong((HWND)lParam,GWL_ID))
			{
			case _CHAT_HINT_EDIT_ID:
				{
					SetBkMode((HDC)wParam, TRANSPARENT);
					SetTextColor((HDC)wParam, RGB(255, 230, 20));
					return (BOOL)ChatLuedManager::GetManager().m_AddClanDlg.hEditBrush;
				}
				break;
			}
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK ChatWndProcessFun::DeleteClanDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG :
		{
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return FALSE;
	case WM_DRAWITEM:
		{
			ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgProcessDrawButton((LPDRAWITEMSTRUCT)lParam);
		}
		return FALSE;
	case WM_COMMAND:
		{
			ChatLuedManager::GetManager().m_DeleteClanDlg.OnCommand(ChatLuedManager::GetManager().m_LuedInfoDlg.hDlg,wParam,lParam);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgProcessMouseMove(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_MOUSELEAVE:
		{
			ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgProcessMouseLeave(hwnd,wParam,lParam);
		}
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			ChatLuedManager::GetManager().m_DeleteClanDlg.ChatHintDlgProcessPaint((HDC)wParam);

		}
		return TRUE;
	case WM_DESTROY:
		{
			ChatLuedManager::GetManager().m_DeleteClanDlg.wndText[0] = 0;
			ChatLuedManager::GetManager().m_DeleteClanDlg.infoText[0] = 0;
			ChatLuedManager::GetManager().m_DeleteClanDlg.font[0] = 0;
		}
		return FALSE;
	}
	return FALSE;
}

LRESULT CALLBACK ChatWndProcessFun::LuedChangeButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedManager::GetManager().m_LuedInfoDlg.m_wndChangeBnt;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=500;
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
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			ChatMainDlg::MainDlgShowWndChat(FALSE);	
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			if(!IsWindowVisible(KWin32App::m_hMainWnd))
			{
				RECT destRect;
				GetClientRect(GetDesktopWindow(),&destRect);
				RECT gameWindow;
				GetWindowRect(KWin32App::m_hMainWnd,&gameWindow);
				int gameWindowWidth = gameWindow.right - gameWindow.left;
				int gameWindowHeight = gameWindow.bottom - gameWindow.top;
				int gameWindowX = (destRect.right - gameWindowWidth)/2;
				int gameWindowY = (destRect.bottom - gameWindowHeight)/2;
				MoveWindow(KWin32App::m_hMainWnd,gameWindowX,gameWindowY,gameWindowWidth,gameWindowHeight,true);
				ShowWindow(KWin32App::m_hMainWnd,SW_NORMAL);
			}
			KUiChannelCentre::Show();
			KUiMiniMap::Show();
			Shell_NotifyIcon(NIM_DELETE,&ChatMainDlg::taskInfo);
			BringWindowToTop(KWin32App::m_hMainWnd);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			ChatFriendPanelManager::ChatFriendManagerGet().addDlg.ChatHintDlgShow(FALSE);
			ChatFriendPanelManager::ChatFriendManagerGet().deleteDlg.ChatHintDlgShow(FALSE);
			ChatClanManager::GetManager().addDlg.ChatHintDlgShow(FALSE);
			ChatClanManager::GetManager().deleteDlg.ChatHintDlgShow(FALSE);
			KUiChannelCentre::GetSingleton().showSystemFrame(true);
			KWin32Frame::s_minisized = false;
			
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;

	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}

LRESULT CALLBACK ChatWndProcessFun::LuedHideShowButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatButton & button = ChatLuedManager::GetManager().m_LuedInfoDlg.m_wndHideShowBnt;
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			button.ChatWndDrawItem(hdc);
			EndPaint(hwnd,&ps);
		}
		return FALSE;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize=sizeof(TRACKMOUSEEVENT);
			tme.dwFlags=TME_HOVER|TME_LEAVE;
			tme.dwHoverTime=500;
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
	case WM_MOUSEHOVER:
		{
			if(button.ChatWndTipEnable())
			{
				if(IsWindowVisible(KWin32App::m_hMainWnd))
				{
					button.ChatWndUpdataTipText(ChatString::ChatStringGetString().hideGameWnd);
				}
				else
					button.ChatWndUpdataTipText(ChatString::ChatStringGetString().showGameWnd);
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
			}
			else
			{
				if(IsWindowVisible(KWin32App::m_hMainWnd))
				{
					button.ChatWndTipCreate(TTS_NOPREFIX, ChatString::ChatStringGetString().hideGameWnd, 100);
				}
				else
				{
					button.ChatWndTipCreate(TTS_NOPREFIX, ChatString::ChatStringGetString().showGameWnd, 100);
				}
				button.ChatWndSetTipPos();
				button.ChatWndShowTip(TRUE);
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
			if(state != _CHAT_BUTTON_STATE_NORMAL)
			{
				button.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				button.ChatWndUpdate();
			}
		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			if(B2ChatDialog::chatManager.isShowMainWnd)
			{
				Shell_NotifyIcon(NIM_ADD,&ChatMainDlg::taskInfo);
				ShowWindow(KWin32App::m_hMainWnd,SW_HIDE);
				KWin32Frame::s_minisized = true;
				B2ChatDialog::chatManager.isShowMainWnd = FALSE;
			}else
			{
				Shell_NotifyIcon(NIM_DELETE,&ChatMainDlg::taskInfo);
				ShowWindow(KWin32App::m_hMainWnd,SW_NORMAL);
				RECT rc;
				::GetWindowRect(KWin32App::m_hMainWnd,&rc);
				RECT rc1;
				::GetWindowRect(ChatMainDlg::hMainDlg,&rc1);
				HWND hDesk = GetDesktopWindow();
				RECT destRc;
				GetClientRect(hDesk,&destRc);
				int widthMain = rc.right - rc.left;
				int widthMain1 = rc1.right - rc1.left;
				int heightMain = rc.bottom - rc.top;
				int heightMain1 = rc1.bottom - rc.top;
				int x = (destRc.right-(widthMain+widthMain1))/2;
				int y = (destRc.bottom - (heightMain))/2;
				MoveWindow(KWin32App::m_hMainWnd,x,y,widthMain,heightMain,TRUE);
				MoveWindow(ChatMainDlg::hMainDlg,x+widthMain,y,widthMain1,heightMain1,TRUE);
				KWin32Frame::s_minisized = false;
				B2ChatDialog::chatManager.isShowMainWnd = TRUE;
			}
			if(B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndIsShow())
			{
				B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
			}
			B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
			B2ChatDialog::chatManager.ChatManagerGetUiComboBox().uiComboBoxShowDownDialog(false);
			
		}
		return FALSE;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);
		}
		return FALSE;

	}
	return CallWindowProc(button.ChatWndGetProcessFun(),hwnd,msg,wParam,lParam);
}