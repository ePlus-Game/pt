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
#include "chatWindow/ChatClanTitleControl.h"

#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "chatWindow/ChatClanPanel.h"
#include "chatWindow/ChatClanListControl.h"
#include "chatWindow/ChatClanInfoDlg.h"
#include "chatWindow/ChatClanManager.h"

ChatClanTitleControl::ChatClanTitleControl()
{

}

ChatClanTitleControl::~ChatClanTitleControl()
{

}

void ChatClanTitleControl::MouseMove(ChatClanInfoDlg & infoDlg, POINT & pos)
{
	if(dragState)
	{
		if(dragState == _TITLE_DRAG_NAMEITEM)
		{
			int dx = pos.x - dragCurrentPoint.x;
			if(dx>0)
			{
				if(metierItemWidth - dx>2)
				{
					metierItemWidth-= dx;
					nameItemWidth+= dx;
					ChatClanManager::GetManager().metierItemWidth -= dx;
					ChatClanManager::GetManager().nameItemWidth += dx;
				}
			}
			else
			{
				if(nameItemWidth+dx>2)
				{
					nameItemWidth+=dx;
					ChatClanManager::GetManager().nameItemWidth+=dx;
					metierItemWidth-=dx;
					ChatClanManager::GetManager().metierItemWidth-= dx;
				}
			}
			infoDlg.PlayerInfoDlgUpdateDrawArea();
			dragCurrentPoint.x = pos.x;
			dragCurrentPoint.y = pos.y;
			return;
		}

		if(dragState == _TITLE_DRAG_METIERITEM)
		{
			int dx = pos.x - dragCurrentPoint.x;
			if(dx>0)
			{
				if(levelItemWidth - dx>2)
				{
					levelItemWidth-= dx;
					metierItemWidth+= dx;
					ChatClanManager::GetManager().levelItemWidth -= dx;
					ChatClanManager::GetManager().metierItemWidth += dx;
				}
			}
			else
			{
				if(metierItemWidth+dx>2)
				{
					metierItemWidth+=dx;
					levelItemWidth-=dx;
					ChatClanManager::GetManager().metierItemWidth += dx;
					ChatClanManager::GetManager().levelItemWidth-=dx;
					
				}
			}
			infoDlg.PlayerInfoDlgUpdateDrawArea();
			dragCurrentPoint.x = pos.x;
			dragCurrentPoint.y = pos.y;
			return;
		}
		return;
	}
	if(pos.x<=posRect.x||pos.x>=posRect.x+posRect.width||
		pos.y<=posRect.y||pos.y>=posRect.y+posRect.height)
	{
		state&=~_TITLE_STATE_NAME_MOUSEOVE;
		state&= ~_TITLE_STATE_METIER_MOUSEOVER;
		state&= ~_TITLE_STATE_LEVEL_MOUSEOVER;
		state&= ~_TITLE_STATE_GROUP_MOUSEOVER;
		state&= ~_TITLE_STATE_PLACE_MOUSEOVER;
	}
	else
	{
		if(pos.x <= posRect.x + nameItemWidth)
		{
			if(state&_TITLE_STATE_NAME_MOUSEOVE)
				return;
			state|=_TITLE_STATE_NAME_MOUSEOVE;
			state&= ~_TITLE_STATE_METIER_MOUSEOVER;
			state&= ~_TITLE_STATE_LEVEL_MOUSEOVER;
			state&= ~_TITLE_STATE_GROUP_MOUSEOVER;
			state&= ~_TITLE_STATE_PLACE_MOUSEOVER;
		}
		else
		if(pos.x <= posRect.x + metierItemWidth + nameItemWidth)
		{
			if(state&_TITLE_STATE_METIER_MOUSEOVER)
				return;
			state|=_TITLE_STATE_METIER_MOUSEOVER;

			state&= ~_TITLE_STATE_NAME_MOUSEOVE;
			state&= ~_TITLE_STATE_LEVEL_MOUSEOVER;
			state&= ~_TITLE_STATE_GROUP_MOUSEOVER;
			state&= ~_TITLE_STATE_PLACE_MOUSEOVER;
		}
		else
		if(pos.x <= posRect.x + metierItemWidth + nameItemWidth + levelItemWidth)
		{
			if(state&_TITLE_STATE_LEVEL_MOUSEOVER)
				return ;
			state|= _TITLE_STATE_LEVEL_MOUSEOVER;

			state&= ~_TITLE_STATE_NAME_MOUSEOVE;
			state&= ~_TITLE_STATE_METIER_MOUSEOVER;
			state&= ~_TITLE_STATE_GROUP_MOUSEOVER;
			state&= ~_TITLE_STATE_PLACE_MOUSEOVER;

		}
	}
	TitleControlUpdate();
}

void ChatClanTitleControl::OnLButtonDBLCLK(ChatClanInfoDlg & infoDlg, POINT & pos)
{
/*	if(pos.x<=posRect.x||pos.x>=posRect.x+posRect.width||
		pos.y<=posRect.y||pos.y>=posRect.y+posRect.height)
	{
		return;
	}

	if(infoDlg.PlayerInfoDlgGetState()&_PLAYER_LIST_CONTROL_NORMAL_SHOW)
		infoDlg.PlayerInfoRemoveState(_PLAYER_LIST_CONTROL_NORMAL_SHOW);
	else
		infoDlg.PlayerInfoSetState(_PLAYER_LIST_CONTROL_NORMAL_SHOW);
	if(pos.x<=posRect.x+nameItemWidth)
	{
		infoDlg.PlayerInfoQSort(_PLAYER_LIST_SORT_BYNAME);
	}
	else
	if(pos.x<=posRect.x+metierItemWidth+nameItemWidth)
		infoDlg.PlayerInfoQSort(_PLAYER_LIST_SORT_BYMETIER);
	else
		infoDlg.PlayerInfoQSort(_PLAYER_LIST_SORT_BYLEVEL);*/

	infoDlg.PlayerInfoDlgUpdateDrawArea();
}