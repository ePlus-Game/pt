#include "UiNpcNavigation.h"
#include "../UiSheetMgr.h"
#include "CoreShell.h"
#include "UiItemTip.h"
#include "../KMessageCentre.h"
// sort的定义 
#include <algorithm> 

using namespace std;

extern iCoreShell* g_pCoreShell;

KUiNpcNavigation::KUiNpcNavigation()
{
	_npcId = UI_NPC_NAVIGATION_INVALID_NPC_ID;
	for(int i = 0; i < UI_NPC_NAVIGATION_MAX_NPC_COUNT; ++i)
	{
		_npcItem[i] = new TLTreeItem("");
	}

	m_SelectedPlayerName[0] = 0;
	m_SelectedPlayerID = UI_NPC_NAVIGATION_INVALID_NPC_ID;
	for (i = 0; i < UI_NAVIGATION_MAX_PLAYER_COUNT; i++)
	{
		m_PlayerItem[i] = new TLTreeItem("");
	}

	m_isUsed = true;
	load();
}

KUiNpcNavigation::~KUiNpcNavigation()
{
	for(int i = 0; i < UI_NPC_NAVIGATION_MAX_NPC_COUNT; ++i)
	{
		delete _npcItem[i];
		_npcItem[i] = NULL;
	}

	for (i = 0; i < UI_NAVIGATION_MAX_PLAYER_COUNT; i++)
	{
		delete m_PlayerItem[i];
		m_PlayerItem[i] = NULL;
	}
}

KUiNpcNavigation& KUiNpcNavigation::getSingleton()
{
	static KUiNpcNavigation singleton;
	return singleton;
}

void KUiNpcNavigation::load()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_NPC_NAVIGATION_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_NPC_NAVIGATION_WINDOW_PATH);
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);
	_thisWindow->hide();

	//npc查询
	_npcPanel = (TLStaticImage*)_thisWindow->getChild("TaharezLook/NpcNavigation/NpcPanel");
	_npcPanel_Clipper = (TLStaticImage*)_npcPanel->getChild("TaharezLook/NpcNavigation/NpcPanel/Clipper");
	_npcPanel_NameList = (TLTree*)_npcPanel_Clipper->getChild("TaharezLook/NpcNavigation/NpcPanel/Clipper/NameList");
	_npcPanel_NameList->subscribeEvent(Tree::TR_EventSelectionChanged, Event::Subscriber(&KUiNpcNavigation::selectNpc, this));
	_npcPanel_NameList->subscribeEvent(Tree::TR_EventDoubleClick, Event::Subscriber(&KUiNpcNavigation::selectNpcAndGo, this));
	_npcPanel_NameList->subscribeEvent(Tree::EventMouseWheel, Event::Subscriber(&KUiNpcNavigation::onListWheelChanged, this));

	_npcPanel_Scroll = (TLVertScrollbar*)_npcPanel->getChild("TaharezLook/NpcNavigation/NpcPanel/Scroll");
	_npcPanel_Scroll->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiNpcNavigation::onScroll, this));

	_npcPanel_Goto = (TLButton*)_npcPanel->getChild("TaharezLook/NpcNavigation/NpcPanel/Goto");
	_npcPanel_Goto->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiNpcNavigation::onGoto, this));

	TLButton* closeBtn = (TLButton*)_thisWindow->getChild("TaharezLook/NpcNavigation/Close");
	closeBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiNpcNavigation::onClose, this));

	_npcPanel_NameList->subscribeEvent(Window::EventMouseMove, Event::Subscriber(&KUiNpcNavigation::onMHover, this));
	_npcPanel_NameList->subscribeEvent(Window::EventMouseLeaves, Event::Subscriber(&KUiNpcNavigation::onMLeave, this));

	_npcPanel->setZLevel(Window::Bottom);

	//附近玩家
	if (m_isUsed)
	{
		m_NpcButton = (TLButton *)_thisWindow ->getChild("TaharezLook/NpcNavigation/NpcButton");
		m_NpcButton ->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiNpcNavigation::OnNpcButton, this));

		m_PlayerButton = (TLButton *)_thisWindow ->getChild("TaharezLook/NpcNavigation/PlayerButton");
		m_PlayerButton ->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiNpcNavigation::OnPlayerButton, this));

		m_PlayerPanel = (TLStaticImage *)_thisWindow ->getChild("TaharezLook/NpcNavigation/PlayerPanel");

		m_AddFriend = (TLButton *)m_PlayerPanel ->getChild("TaharezLook/NpcNavigation/PlayerPanel/AddFriend");
		m_AddFriend ->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiNpcNavigation::OnAddFriend, this));

		m_PrivateChat = (TLButton *)m_PlayerPanel ->getChild("TaharezLook/NpcNavigation/PlayerPanel/PrivateChat");
		m_PrivateChat ->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiNpcNavigation::OnPrivateChat, this));

		m_Team = (TLButton *)m_PlayerPanel ->getChild("TaharezLook/NpcNavigation/PlayerPanel/Team");
		m_Team ->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiNpcNavigation::OnTeam, this));

		m_ViewItem = (TLButton *)m_PlayerPanel ->getChild("TaharezLook/NpcNavigation/PlayerPanel/ViewItem");
		m_ViewItem ->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiNpcNavigation::OnViewItem, this));

		m_Follow = (TLButton *)m_PlayerPanel ->getChild("TaharezLook/NpcNavigation/PlayerPanel/Follow");
		m_Follow ->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiNpcNavigation::OnFollow, this));

		m_MoveToPlayer = (TLButton *)m_PlayerPanel ->getChild("TaharezLook/NpcNavigation/PlayerPanel/MoveToPlayer");
		m_MoveToPlayer ->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiNpcNavigation::OnMoveToPlayer, this));

		m_PlayerClipper = (TLStaticImage *)m_PlayerPanel ->getChild("TaharezLook/NpcNavigation/PlayerPanel/PlayerClipper");

		m_PlayerScroll = (TLVertScrollbar *)m_PlayerPanel ->getChild("TaharezLook/NpcNavigation/PlayerPanel/Scroll");
		m_PlayerScroll ->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiNpcNavigation::OnPlayerScroll, this));

		m_PlayerList = (TLTree *)m_PlayerClipper ->getChild("TaharezLook/NpcNavigation/PlayerPanel/PlayerClipper/PlayerList");
		m_PlayerList ->subscribeEvent(Tree::TR_EventSelectionChanged, Event::Subscriber(&KUiNpcNavigation::SelectPlayer, this));
		m_PlayerList ->subscribeEvent(Tree::TR_EventDoubleClick, Event::Subscriber(&KUiNpcNavigation::SelectPlayerAndGo, this));
		m_PlayerList ->subscribeEvent(Tree::EventMouseWheel, Event::Subscriber(&KUiNpcNavigation::OnListWhell, this));

		m_PlayerPanel ->setZLevel(Window::Bottom);
		m_PlayerPanel ->hide();
	}
}

void KUiNpcNavigation::loadNpcInfo()
{
	if(!_thisWindow)
	{
		return;
	}

	static char mapName[COMMON_CLIENT_MSG_LEN_64];

	KUiSceneTimeInfo mapInfo;
	g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL);
	mapInfo.szSceneName[sizeof(mapInfo.szSceneName) - 1] = 0;

	if(!strcmp(mapInfo.szSceneName, mapName))
	{
		return;
	}

	//进入一个新地图后第一次打开
	_npcInfo.clear();
	strcpy(mapName, mapInfo.szSceneName);

	g_pCoreShell->GetGameData(GDI_NPC_LIST_OF_MAP, (unsigned int)&_npcInfo, (int)mapInfo.szSceneName);
		
	sortByName();

	_npcPanel_NameList->removeAllItem();
	for(int i = 0; i < _npcInfo.size(); ++i)
	{
		char npcPos[COMMON_CLIENT_MSG_LEN_64] = {0};
		sprintf(npcPos, "%s  (%d, %d)", _npcInfo[i].idAndName.name, _npcInfo[i].x, _npcInfo[i].y);
		npcPos[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
		_npcItem[i]->setText(AnsiToUtf8(npcPos));
		_npcItem[i]->setID(_npcInfo[i].idAndName.id);
		_npcItem[i]->setAutoDeleted(false);
		_npcPanel_NameList->addItem(_npcItem[i]);
	}
	
	if(_npcPanel_NameList->getItemCount())
	{
		_npcId = _npcItem[0]->getID();
	}
	else
	{
		_npcId = UI_NPC_NAVIGATION_INVALID_NPC_ID;
	}

	_npcPanel_Scroll->setScrollPosition(0);
}

void KUiNpcNavigation::layoutNpcPanel()
{
	if(!_thisWindow)
	{
		return;
	}

	int clipperHeight	= _npcPanel_Clipper->getHeight(Absolute);
	int actHeight		= _npcPanel_NameList->getTreeTotalItemsHeigh();

	if(actHeight <= clipperHeight)
	{
		_npcPanel_Scroll->hide();
	}
	else
	{
		_npcPanel_Scroll->show();
		int treeItemCount = _npcPanel_NameList->getItemCount();
		float step = ((float)actHeight / treeItemCount) / (actHeight - clipperHeight);
		_npcPanel_Scroll->setStepSize(step);
	}
}

bool KUiNpcNavigation::gotoSelNpc()
{
	if(!_thisWindow)
	{
		return false;
	}

	if(UI_NPC_NAVIGATION_INVALID_NPC_ID == _npcId)
	{
		return false;
	}

	NpcMapPos pos;
	g_pCoreShell->GetGameData(GDI_GET_NPC_POS_BY_TABLE_INDEX, (UINT)&pos, _npcId);
	g_pCoreShell->OperationRequest(GOI_SET_AUTO_DIALOG_NPC, _npcId, NULL);
	g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)pos.x, (int)pos.y * 2);

	return true;
}

bool KUiNpcNavigation::selectNpc(const EventArgs& args)
{
	TreeEventArgs* treeEvent = (TreeEventArgs*)&args;
	TreeItem* treeItem = treeEvent->treeItem;

	if(treeItem == NULL)
	{
		return false;
	}

	_npcId = treeItem->getID();

	return true;
}

bool KUiNpcNavigation::selectNpcAndGo(const EventArgs& args)
{
	TreeEventArgs* treeEvent = (TreeEventArgs*)&args;
	TreeItem* treeItem = treeEvent->treeItem;

	if(treeItem == NULL)
	{
		return false;
	}

	_npcId = treeItem->getID();
	
	gotoSelNpc();
	return true;
}

bool KUiNpcNavigation::onListWheelChanged(const EventArgs& args)
{	
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
	
	if(_npcPanel_Scroll->isVisible())
	{
		_npcPanel_Scroll->setScrollPosition(_npcPanel_Scroll->getScrollPosition()
			- _npcPanel_Scroll->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiNpcNavigation::onScroll(const EventArgs& args)
{
	float scrollPos = _npcPanel_Scroll->getScrollPosition();
	int clipperHeight	= _npcPanel_Clipper->getHeight(Absolute);
	int actHeight		= _npcPanel_NameList->getTreeTotalItemsHeigh();
	int exceedSize = (actHeight - clipperHeight) * scrollPos;
	_npcPanel_NameList->setYPosition(Absolute, -exceedSize);
	return true;
}

bool KUiNpcNavigation::onGoto(const EventArgs& args)
{
	gotoSelNpc();
	return true;
}

bool KUiNpcNavigation::onClose(const EventArgs& args)
{
	hide();
	return true;
}

bool KUiNpcNavigation::onMHover(const EventArgs& args)
{
	MouseEventArgs* ma = (MouseEventArgs*)&args;

	if(_npcPanel_NameList->getItemAtCurMouse())
	{
		Point pos = ma->position;
		Rect windowArea = _thisWindow->getUnclippedInnerRect();
		pos.d_x = windowArea.getPosition().d_x + windowArea.getWidth();

		char* message = KMessageCentre::GetMessage(common_message, UI_NPC_NAVIGATION_TIP_TEXT_ID);
		KUiItemTip::GetSingleton().show(message, Rect(pos, Size(0, 0)), KUiItemTip::Right);
	}
	else
	{
		KUiItemTip::Hide();
	}

	return true;
}

bool KUiNpcNavigation::onMLeave(const EventArgs& args)
{
	KUiItemTip::Hide();
	return true;
}

bool KUiNpcNavigation::isVisible()
{
	if(_thisWindow)
	{
		return _thisWindow->isVisible();
	}

	return false;
}

void KUiNpcNavigation::show()
{
	if(_thisWindow)
	{
		_thisWindow->show();
		if (m_isUsed)
		{
			m_PlayerPanel ->hide();
			_npcPanel ->show();
		}
		loadNpcInfo();
		layoutNpcPanel();
	}
}

void KUiNpcNavigation::hide()
{
	if(_thisWindow)
		_thisWindow->hide();	
}

void KUiNpcNavigation::toggle()
{
	if(!_thisWindow)
	{
		return;
	}
	
	if(_thisWindow->isVisible())
		hide();
	else
		show();
}

bool cmpNpcName(const NpcMapInfo& npcInfo1, const NpcMapInfo& npcInfo2)
{
 	char firstLetter1 = KUiNpcNavigation::FirstLetter(npcInfo1.idAndName.name);
 	char firstLetter2 = KUiNpcNavigation::FirstLetter(npcInfo2.idAndName.name);
 
 	if(firstLetter1 < firstLetter2)
 	{
 		return true;
 	}
	return false;
}

void KUiNpcNavigation::sortByName()
{
	std::sort(_npcInfo.begin(), _npcInfo.end(), cmpNpcName);
}

char KUiNpcNavigation::FirstLetter(string text)
{
	char letter = '0';
	
	return letter;
}

bool KUiNpcNavigation::OnAddFriend(const EventArgs & args)
{
	if (m_SelectedPlayerName[0] == 0)
	{
		return false;
	}

	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)m_SelectedPlayerName, 1);
	}
	return true;
}

bool KUiNpcNavigation::OnPrivateChat(const EventArgs & args)
{
	KUiChatInputWnd::GetSingleton().clearText();
	KUiChatInputWnd::GetSingleton().write("/");
	KUiChatInputWnd::GetSingleton().write(m_SelectedPlayerName);
	KUiChatInputWnd::GetSingleton().write(" ");
	KUiChatInputWnd::GetSingleton().show();

	return true;
}

bool KUiNpcNavigation::OnTeam(const EventArgs & args)
{
	if (m_SelectedPlayerName[0] == 0)
	{
		return false;
	}

	KUiPlayerItem tagPlayer;
	strcpy(tagPlayer.Name, m_SelectedPlayerName);
	tagPlayer.nData = 0;
	tagPlayer.nIndex = 0;
	tagPlayer.nParam = 0;
	tagPlayer.uId = m_SelectedPlayerID;
	
	KUiPlayerTeam	TeamInfo;
	TeamInfo.cNumMember = 0;
	g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
	if ( ((int)TeamInfo.cNumMember) <= MAX_TEAMMEMBER_COUNT )
	{
		if (TeamInfo.cNumMember == 0)
		{
			g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
		}
		if (g_pCoreShell)
		{
			g_pCoreShell->TeamOperation( TEAM_OI_INVITE_BY_NAME, (unsigned int)&tagPlayer, NULL );
		}
	}
	else
	{
		char *msg = KMessageCentre::GetMessage(team_message, 1);
		KUiChannelCentre::GetSingleton().toSysMsg(msg);
	}
	return true;
}

bool KUiNpcNavigation::OnViewItem(const EventArgs & args)
{
	if (m_SelectedPlayerID == UI_NPC_NAVIGATION_INVALID_NPC_ID)
	{
		return false;
	}

	if (g_pCoreShell)
	{
		g_pCoreShell ->OperationRequest(GOI_VIEW_PLAYERITEM, (unsigned int)m_SelectedPlayerID, NULL);
	}
	return true;
}

bool KUiNpcNavigation::OnFollow(const EventArgs & args)
{
	if (m_SelectedPlayerID == UI_NPC_NAVIGATION_INVALID_NPC_ID)
	{
		return false;
	}

	if (g_pCoreShell)
	{
		g_pCoreShell ->OperationRequest(GOI_FOLLOW_SOMEONE, (unsigned int)m_SelectedPlayerID, NULL);
	}
	return true;
}

bool KUiNpcNavigation::OnMoveToPlayer(const EventArgs &args)
{
	if (m_SelectedPlayerName[0] == 0)
	{
		return false;
	}

	pair<int, int> pos;
	if (g_pCoreShell)
	{
		g_pCoreShell->GetGameData(GDI_GET_PLAYER_POS, (unsigned int)&pos, (int)m_SelectedPlayerName);
		g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)pos.first, (int)pos.second * 2);
	}
	return true;
}

bool KUiNpcNavigation::OnListWhell(const EventArgs & args)
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
	
	if(m_PlayerScroll ->isVisible())
	{
		m_PlayerScroll ->setScrollPosition(m_PlayerScroll ->getScrollPosition()
			- m_PlayerScroll ->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiNpcNavigation::SelectPlayer(const EventArgs & args)
{
	TreeEventArgs* treeEvent = (TreeEventArgs*)&args;
	TreeItem* treeItem = treeEvent->treeItem;

	if(treeItem == NULL)
	{
		return false;
	}

	m_SelectedPlayerID = treeItem ->getID();
	m_SelectedPlayerName[0] = 0;
	strcpy(m_SelectedPlayerName, Utf8ToAnsi(treeItem ->getText()));
	return true;
}

bool KUiNpcNavigation::SelectPlayerAndGo(const EventArgs & args)
{
	TreeEventArgs* treeEvent = (TreeEventArgs*)&args;
	TreeItem* treeItem = treeEvent->treeItem;

	if(treeItem == NULL)
	{
		return false;
	}

	String _playerName;
	_playerName = treeItem ->getText();

	m_SelectedPlayerName[0] = 0;
	strcpy(m_SelectedPlayerName, Utf8ToAnsi(_playerName));

	if (m_SelectedPlayerName[0] == 0)
	{
		return false;
	}

	pair<int, int> pos;
	if (g_pCoreShell)
	{
		g_pCoreShell->GetGameData(GDI_GET_PLAYER_POS, (UINT)&pos, (int)&m_SelectedPlayerName);
		g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)pos.first, (int)pos.second * 2);
	}
	return true;
}

bool KUiNpcNavigation::OnPlayerScroll(const EventArgs & args)
{
	float scrollPos = m_PlayerScroll->getScrollPosition();
	int clipperHeight	= m_PlayerClipper->getHeight(Absolute);
	int actHeight		= m_PlayerList->getTreeTotalItemsHeigh();
	int exceedSize = (actHeight - clipperHeight) * scrollPos;
	m_PlayerList->setYPosition(Absolute, -exceedSize);
	return true;
}

bool KUiNpcNavigation::OnPlayerMHover(const EventArgs & args)
{
	MouseEventArgs* ma = (MouseEventArgs*)&args;

	if(m_PlayerList->getItemAtCurMouse())
	{
/*		Point pos = ma->position;
		Rect windowArea = _thisWindow->getUnclippedInnerRect();
		pos.d_x = windowArea.getPosition().d_x + windowArea.getWidth();

		char* message = KMessageCentre::GetMessage(common_message, UI_NPC_NAVIGATION_TIP_TEXT_ID);
		KUiItemTip::GetSingleton().show(message, Rect(pos, Size(0, 0)), KUiItemTip::Right);*/
	}
	else
	{
		KUiItemTip::Hide();
	}
	return true;
}

bool KUiNpcNavigation::OnPlayerMLeave(const EventArgs & args)
{
	KUiItemTip::Hide();
	return true;
}

bool KUiNpcNavigation::OnNpcButton(const EventArgs & args)
{
	m_PlayerPanel ->hide();
	_npcPanel ->show();
	return true;
}

bool KUiNpcNavigation::OnPlayerButton(const EventArgs & args)
{
	_npcPanel ->hide();
	GetPlayerList();
	LayoutPlayerPanel();
	m_PlayerPanel ->show();
	return true;
}

void KUiNpcNavigation::GetPlayerList()
{
	if(!_thisWindow)
	{
		return;
	}

	m_PlayerList ->removeAllItem();
	_npcInfo.clear();

	g_pCoreShell->GetGameData(GDI_GET_NEARBY_PLAYER, (unsigned int)&_npcInfo, NULL);
	
	for (int i = 0; i < _npcInfo.size(); i++)
	{
		m_PlayerItem[i] ->setText(AnsiToUtf8(_npcInfo[i].idAndName.name));
		m_PlayerItem[i] ->setID(_npcInfo[i].idAndName.id);
		m_PlayerItem[i] ->setAutoDeleted(false);
		m_PlayerList ->addItem(m_PlayerItem[i]);
	}

	m_SelectedPlayerName[0] = 0;

	m_PlayerScroll ->setScrollPosition(0);
}

void KUiNpcNavigation::LayoutPlayerPanel()
{
	if(!_thisWindow)
	{
		return;
	}

	int clipperHeight	= m_PlayerClipper->getHeight(Absolute);
	int actHeight		= m_PlayerList->getTreeTotalItemsHeigh();

	if(actHeight <= clipperHeight)
	{
		m_PlayerScroll ->hide();
	}
	else
	{
		m_PlayerScroll ->show();
		int treeItemCount = m_PlayerList->getItemCount();
		float step = ((float)actHeight / treeItemCount) / (actHeight - clipperHeight);
		m_PlayerScroll->setStepSize(step);
	}
}
