
#include "UiRaid.h"
#include "Coreshell.h"
#include "UiPlayerMenu.h"
#include "UiUnitMenu.h"
#include "../UiCommon.h"
#include "../UiConfigManager.h"
#include "../UiSheetMgr.h"

extern iCoreShell*		g_pCoreShell;


KUiRaidPlayer::KUiRaidPlayer()
{
	_playerIndex = UI_RAID_INVALID_PLAYER_INDEX;
	
#ifndef _DEBUG
	try
	{
#endif
		static int namePrefix = 0;
		_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_RAID_PLAYER_WINDOW_NAME, iToString(namePrefix++));
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif
	
	_headImg = (TLStaticImage*)_thisWindow->getChild(_thisWindow->getName() + "/HeadImg");
	_playerName = (TLStaticText*)_thisWindow->getChild(_thisWindow->getName() + "/Name");
	_leadIcon = (TLStaticImage*)_thisWindow->getChild(_thisWindow->getName() + "/LeadIcon");
	_hoverFrameImg = (TLStaticImage*)_thisWindow->getChild(_thisWindow->getName() + "/HoverFrameImage");
	_pushedFrameImg = (TLStaticImage*)_thisWindow->getChild(_thisWindow->getName() + "/PushedFrameImage");
	_playerName->disable();
	_leadIcon->disable();
	_hoverFrameImg->disable();
	_pushedFrameImg->disable();

	_assistIcon = (TLButton*)_thisWindow->getChild(_thisWindow->getName() + "/AssistIcon");
	_assistIcon->subscribeEvent(Window::EventMouseButtonUp, Event::Subscriber(&KUiRaidPlayer::onAssistBtnDown, this));
// 	_assistIcon->subscribeEvent(Window::EventMouseEnters, Event::Subscriber(&KUiRaidPlayer::onMouseIn, this));
// 	_assistIcon->subscribeEvent(Window::EventMouseLeaves, Event::Subscriber(&KUiRaidPlayer::onMouseOut, this));
// 
// 	_headImg->subscribeEvent(Window::EventMouseEnters, Event::Subscriber(&KUiRaidPlayer::onMouseIn, this));
// 	_headImg->subscribeEvent(Window::EventMouseLeaves, Event::Subscriber(&KUiRaidPlayer::onMouseOut, this));
	
	_thisWindow->subscribeEvent(Window::EventMouseButtonDown, Event::Subscriber(&KUiRaidPlayer::onBDown, this));
	_thisWindow->subscribeEvent(Window::EventMouseButtonUp, Event::Subscriber(&KUiRaidPlayer::onBUp, this));
	_thisWindow->subscribeEvent(Window::EventMouseLeaves, Event::Subscriber(&KUiRaidPlayer::onMouseOut, this));
	_thisWindow->subscribeEvent(Window::EventMouseEnters, Event::Subscriber(&KUiRaidPlayer::onMouseIn, this));
}

KUiRaidPlayer::~KUiRaidPlayer()
{

}

Point KUiRaidPlayer::getPos()
{
	if(!_thisWindow)
	{
		return Point(0, 0);
	}

	return _thisWindow->getPosition(Absolute);
}

void KUiRaidPlayer::setPos(const Point& pos)
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->setPosition(Absolute, pos);
}

int KUiRaidPlayer::getHeight()
{
	if(!_thisWindow)
	{
		return 0;
	}

	return _thisWindow->getHeight(Absolute);
}

int KUiRaidPlayer::getWidth()
{
	if(!_thisWindow)
	{
		return 0;
	}

	return _thisWindow->getWidth(Absolute);
}

void KUiRaidPlayer::setParent(Window* parent)
{
	if(!_thisWindow)
	{
		return;
	}

	if(NULL == parent)
	{
		return;
	}

	parent->addChildWindow(_thisWindow);
}

void KUiRaidPlayer::show(int playerIndex)
{
	if(!_thisWindow)
	{
		return;
	}

	if(playerIndex < 0 || playerIndex >= UI_RAID_MAX_NUM)
	{
		return;
	}

	if(_playerIndex != playerIndex)
	{
		_playerIndex = playerIndex;
		setNormalState();
	}


	const KUiTeamMemberItem& player = KUiRaid::getSinglton().getPlayerByIndex(playerIndex);
	_playerName->setText(AnsiToUtf8(player.m_szName));
	
	const char* headImageText = getHeadImg(player.m_nSeries, player.m_nSkillSeries);
	if(headImageText != NULL)
	{
		const Image* headImage = getImage(headImageText);
		_headImg->setImage(headImage);
	}

	showLeadIcon(false);
	showAssistIcon(false, false);
	switch(player.m_Type)
	{
	case teammember_type_captain:
		{
			showLeadIcon(true);
		}
		break;
	case teammember_type_assistant:
		{
			showAssistIcon(true, true);
		}
		break;
	case teammember_type_normal:
		{
			if(KUiRaid::getSinglton().isLeader())
			{
				showAssistIcon(true, false);
			}
		}
		break;
	}

	_thisWindow->show();
}

void KUiRaidPlayer::setHoverState()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->setFrameEnabled(false);
	_thisWindow->setBackgroundEnabled(false);
	_hoverFrameImg->show();
	_pushedFrameImg->hide();
	
	float red, green, blue;
	if(_playerIndex < MAX_TEAM_MEMBER)
	{
		sscanf(KUiCfgLoader::getSingleton().getRaidCfg().topMemberHoverColor, "%f,%f,%f", &red, &green, &blue);
	}
	else
	{
		sscanf(KUiCfgLoader::getSingleton().getRaidCfg().hoverColor, "%f,%f,%f", &red, &green, &blue);
	}
	_playerName->setTextColours(colour(red / 255, green / 255, blue / 255));
}

void KUiRaidPlayer::setNormalState()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->setFrameEnabled(false);
	_thisWindow->setBackgroundEnabled(false);
	_hoverFrameImg->hide();
	_pushedFrameImg->hide();
	
	float red, green, blue;
	if(_playerIndex < MAX_TEAM_MEMBER)
	{
		sscanf(KUiCfgLoader::getSingleton().getRaidCfg().topMemberNormalColor, "%f,%f,%f", &red, &green, &blue);
	}
	else
	{
		sscanf(KUiCfgLoader::getSingleton().getRaidCfg().normalColor, "%f,%f,%f", &red, &green, &blue);
	}
	_playerName->setTextColours(colour(red / 255, green / 255, blue / 255));
}

void KUiRaidPlayer::setPushDownState()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->setFrameEnabled(false);
	_thisWindow->setBackgroundEnabled(true);
	_hoverFrameImg->hide();
	_pushedFrameImg->show();
	
	float red, green, blue;
	if(_playerIndex < MAX_TEAM_MEMBER)
	{
		sscanf(KUiCfgLoader::getSingleton().getRaidCfg().topMemberHoverColor, "%f,%f,%f", &red, &green, &blue);
	}
	else
	{
		sscanf(KUiCfgLoader::getSingleton().getRaidCfg().hoverColor, "%f,%f,%f", &red, &green, &blue);
	}
	_playerName->setTextColours(colour(red / 255, green / 255, blue / 255));
}

void KUiRaidPlayer::hide()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->hide();
}

bool KUiRaidPlayer::isVisible()
{
	if(!_thisWindow)
	{
		return false;
	}

	return _thisWindow->isVisible();
}

void KUiRaidPlayer::showLeadIcon(bool isLead)
{
	if(!_thisWindow)
	{
		return;
	}

	if(isLead)
	{
		_leadIcon->show();
	}
	else
	{
		_leadIcon->hide();
	}
}

void KUiRaidPlayer::showAssistIcon(bool show, bool isAssist)
{
	if(!_thisWindow)
	{
		return;
	}

	if(!show)
	{
		_assistIcon->hide();
		return;
	}

	_assistIcon->show();
	static const Image* noassistImage = _assistIcon->getNormalImage();
	static const Image* assistImage = _assistIcon->getDisabledImage();
	if(isAssist)
	{
		_assistIcon->setNormalImage(assistImage);
	}
	else
	{
		_assistIcon->setNormalImage(noassistImage);
	}
}

void KUiRaidPlayer::disable()
{
	_playerIndex = UI_RAID_INVALID_PLAYER_INDEX;
}

const char* KUiRaidPlayer::getPlayerName()
{
	static char* emptyString = "";
	if(!_thisWindow)
	{
		return NULL;
	}

	return Utf8ToAnsi(_playerName->getText());
}


int KUiRaidPlayer::getPlayerIndex()
{
	if(!_thisWindow)
	{
		return 0;
	}

	return _playerIndex;
}

bool KUiRaidPlayer::onBDown(const EventArgs& e)
{
	MouseEventArgs* event = (MouseEventArgs*)&e;

	if(LeftButton == event->button)
	{
		if(KUiRaidDragPlayer::getSinglton().isVisible())
		{
			KUiRaid::getSinglton().switchPlayer(_playerIndex, KUiRaidDragPlayer::getSinglton().getPlayerIndex());
			_lbDown = false;
			KUiRaidDragPlayer::getSinglton().hide();
		}
		else
		{
			//左键选择为当前目标
// 			const KUiTeamMemberItem& player = KUiRaid::getSinglton().getPlayerByIndex(_playerIndex);
// 			int npcIndex = g_pCoreShell->FindNpcIndexById(player.m_uId);
// 			if (npcIndex > 0)
// 			{
// 				g_pCoreShell->SelectNPC(npcIndex);
// 			}
			_lbDown = true;
		}
		KUiUnitMenu::getSingleton().hide();
	}
	else
	{
		KUiRaidDragPlayer::getSinglton().hide();

		Point pos = MouseCursor::getSingleton().getPosition();
		const KUiTeamMemberItem& playerInfo = KUiRaid::getSinglton().getPlayerByIndex(_playerIndex);

		KUiUnitMenu::UMNpcInfo npcInfo;
		npcInfo.NpcName = const_cast<char*>(playerInfo.m_szName);
		npcInfo.NpcId = playerInfo.m_uId;
		KUiUnitMenu::getSingleton().show(npcInfo, 
			UI_UNIT_MENU_WISPER_FLAG 
			| UI_UNIT_MENU_CAPTION_FLAG 
			| UI_UNIT_MENU_KICK_FLAG 
			| UI_UNIT_MENU_LEAVE_FLAG 
			| UI_UNIT_MENU_ADDFRIEND_FLAG,
			pos);
	}
	setPushDownState();
	return true;
}

bool KUiRaidPlayer::onBUp(const EventArgs& e)
{
	MouseEventArgs* event = (MouseEventArgs*)&e;
	if(LeftButton == event->button)
	{
		if(_lbDown && event->sysKeys & Shift)
		{
			String name = _playerName->getText();
			KUiRaidDragPlayer::getSinglton().show(getPlayerIndex());
		}
		else if(KUiRaidDragPlayer::getSinglton().isVisible())
		{
			KUiRaid::getSinglton().switchPlayer(_playerIndex, KUiRaidDragPlayer::getSinglton().getPlayerIndex());
			KUiRaidDragPlayer::getSinglton().hide();
		}
		_lbDown = false;
	}
	else
	{

	}
	setHoverState();
	return true;
}

bool KUiRaidPlayer::onMouseOut(const EventArgs& e)
{	
	if(_lbDown)
	{
		KUiRaidDragPlayer::getSinglton().show(getPlayerIndex());
	}
	_lbDown = false;

	//设置外观
	setNormalState();

	return true;
}

bool KUiRaidPlayer::onMouseIn(const EventArgs& e)
{
	//设置外观
	setHoverState();

	//
	KUiPlayerMenu::Hide();

	return false;
}

const char* KUiRaidPlayer::getHeadImg(int series, int skillSeries)
{
	if(series == 0)
	{
		if(skillSeries == 0)
		{
			return KUiCfgLoader::getSingleton().getRaidCfg().xingtianHeadImg;
		}
		else if(skillSeries == 1)
		{
			return KUiCfgLoader::getSingleton().getRaidCfg().xuanfengHeadImg;
		}
		else
		{
			return NULL;
		}
	}
	else if(series == 1)
	{
		if(skillSeries == 0)
		{
			return KUiCfgLoader::getSingleton().getRaidCfg().zhenrenHeadImg;
		}
		else if(skillSeries == 1)
		{
			return KUiCfgLoader::getSingleton().getRaidCfg().tianshiHeadImg;
		}
		else
		{
			return NULL;
		}
	}
	else if(series == 2)
	{
		if(skillSeries == 0)
		{
			return KUiCfgLoader::getSingleton().getRaidCfg().yishiHeadImg;
		}
		else if(skillSeries == 1)
		{
			return KUiCfgLoader::getSingleton().getRaidCfg().shoushiHeadImg;
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}

bool KUiRaidPlayer::onAssistBtnDown(const EventArgs& e)
{
	const KUiTeamMemberItem& memberInfo = KUiRaid::getSinglton().getPlayerByIndex(_playerIndex);
	if(memberInfo.m_Type == teammember_type_assistant)
	{
		g_pCoreShell->TeamOperation(TEAM_IO_DISMISS_ASSISTANT, memberInfo.m_uId, 0);
	}
	else
	{
		g_pCoreShell->TeamOperation(TEAM_IO_PROMOTE_ASSISTANT, memberInfo.m_uId, 0);
	}
	return false;
}
/*==============================================================*/

KUiRaid::KUiRaid()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_RAID_WINDOW_NAME_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_RAID_WINDOW_NAME);
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
	
	_thisWindow->subscribeEvent(TLStaticImage::EventNewFrame, 
		Event::Subscriber(&KUiRaid::onTimer, this));

	//teamlist
	_playerInfo = (TLStaticImage*)_thisWindow->getChild("TaharezLook/Raid/PlayerInfo");
	_playerPanel = (TLStaticImage*)_playerInfo->getChild("TaharezLook/Raid/PlayerInfo/PlayerPanel");
	_playerPanelMaxSize = _playerInfo->getHeight(Absolute);

	for(int i = 0; i < UI_RAID_MAX_NUM; ++i)
	{
		_playerPanel->addChildWindow(_player[i]._thisWindow);
	}

	//向上向下触发区域
	_upArea = (TLStaticImage*)_thisWindow->getChild("TaharezLook/Raid/UpTriggerArea");
	_upArea->subscribeEvent(TLStaticImage::EventMouseEnters, Event::Subscriber(&KUiRaid::onUpAreaIn, this));
	_upArea->subscribeEvent(TLStaticImage::EventMouseLeaves, Event::Subscriber(&KUiRaid::onUpAreaOut, this));

	_downArea = (TLStaticImage*)_thisWindow->getChild("TaharezLook/Raid/DownTriggerArea");
	_downArea->subscribeEvent(TLStaticImage::EventMouseEnters, Event::Subscriber(&KUiRaid::onDownAreaIn, this));
	_downArea->subscribeEvent(TLStaticImage::EventMouseLeaves, Event::Subscriber(&KUiRaid::onDownAreaOut, this));

	//滑动条
	_scrollBar = (TLVertScrollbar*)_playerInfo->getChild("TaharezLook/Raid/PlayerInfo/Scroll");
	_scrollBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
		Event::Subscriber(&KUiRaid::onScroll, this));
	_playerInfo->subscribeEvent(TLVertScrollbar::EventMouseWheel, Event::Subscriber(&KUiRaid::onWheelChanged, this));

	//关闭按钮
	_closeBtn = (TLButton*)_thisWindow->getChild("TaharezLook/Raid/Close");
	_closeBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiRaid::onClickCloseBtn, this));

	_thisWindow->hide();
	layoutPanel();
}

KUiRaid::~KUiRaid()
{
	
}

void KUiRaid::show()
{	
	if(!_thisWindow)
	{
		return;
	}

	freshTeamInfo();
//	layoutPanel();
	
 	System::getSingleton().getGUISheet()->removeChildWindow(_thisWindow);
 	System::getSingleton().getGUISheet()->addChildWindow(_thisWindow);
	_thisWindow->show();
}

void KUiRaid::freshTeamInfo()
{
	if(!_thisWindow)
	{
		return;
	}

	KUiPlayerTeam teamInfo;
	g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&teamInfo, NULL);

	_isLeader = teamInfo.bTeamLeader;
	
	_curMemberCount = g_pCoreShell->TeamOperation(TEAM_OI_MEMBER_INFO, (unsigned int)_teamInfo, 0);

//   	_teamInfo[0].m_nSeries = 0;
//   	_teamInfo[0].m_nSkillSeries = 0;
//   	_teamInfo[1].m_nSeries = 0;
//   	_teamInfo[1].m_nSkillSeries = 1;
//   	_teamInfo[2].m_nSeries = 1;
//   	_teamInfo[2].m_nSkillSeries = 0;
//   	_teamInfo[3].m_nSeries = 1;
//   	_teamInfo[3].m_nSkillSeries = 1;
//   	_teamInfo[4].m_nSeries = 2;
//   	_teamInfo[4].m_nSkillSeries = 0;
//   	_teamInfo[5].m_nSeries = 2;
//   	_teamInfo[5].m_nSkillSeries = 1;
//   	_teamInfo[5].m_Type = teammember_type_captain;
//   	_teamInfo[6].m_nSeries = 2;
//   	_teamInfo[6].m_nSkillSeries = 1;
//   	_teamInfo[6].m_Type = teammember_type_assistant;
//   	for(int w = 0; w < UI_RAID_MAX_NUM; w++)
//   	{
//   		sprintf(_teamInfo[w].m_szName, "xie%d", w + 1);
//   	}
//  	_curMemberCount = UI_RAID_MAX_NUM;

	for(int i = 0; i < _curMemberCount; ++i)
	{
		_player[i].show(i);
	}
	
	for(int j = _curMemberCount; j < UI_RAID_MAX_NUM; ++j)
	{
		_player[j].hide();
	}

	
	int playerItemWidth = _player[0].getWidth();
	int playerItemHeight = _player[0].getHeight();
	
	const KUiCfgLoader::RaidCfg& cfg = KUiCfgLoader::getSingleton().getRaidCfg();
	int panelHeight = playerItemHeight * (_curMemberCount + cfg.colCount - 1) / cfg.colCount;
	
	_playerPanel->setHeight(Absolute, panelHeight);
 
	if(_playerPanelMaxSize < panelHeight)
	{
		_scrollBar->setStepSize((float)playerItemHeight / (panelHeight - _playerPanelMaxSize));
		_scrollBar->show();
	}
	else
	{
		_scrollBar->hide();
	}
}

void KUiRaid::layoutPanel()
{
	if(!_thisWindow)
	{
		return;
	}

	const KUiCfgLoader::RaidCfg& cfg = KUiCfgLoader::getSingleton().getRaidCfg();

	int playerItemWidth = _player[0].getWidth();
	int playerItemHeight = _player[0].getHeight();
	Point pos(0, 0);
	for(int i = 0; i < UI_RAID_MAX_NUM; ++i)
	{
		_player[i].setPos(pos);
		pos.d_x += playerItemWidth;

		if(i % cfg.colCount == cfg.colCount - 1)
		{
			pos.d_x = 0;
			pos.d_y += playerItemHeight;
		}
	}

//	int panelWidth = playerItemWidth * cfg.colCount;
//	int panelHeight = playerItemHeight * (_curMemberCount + cfg.colCount - 1) / cfg.colCount;
	
//	_playerPanel->setWidth(Absolute, panelWidth);
// 	_scrollBar->setXPosition(Absolute, panelWidth);
// 	_playerInfo->setWidth(Absolute, panelWidth + _scrollBar->getWidth(Absolute));
// 	_upArea->setWidth(Absolute, panelWidth);
// 	_downArea->setWidth(Absolute, panelWidth);
//	_thisWindow->setWidth(Absolute, panelWidth + _scrollBar->getWidth(Absolute));

// 	_closeBtn->setXPosition(Absolute, _thisWindow->getWidth(Absolute) - _closeBtn->getWidth(Absolute));

//	_thisWindow->setRenderMode(true);
	_thisWindow->setRenderMode(false);
}

void KUiRaid::toggle()
{
	// 暂时禁用
	return;
	if(!_thisWindow)
	{
		return;
	}

	if(_thisWindow->isVisible())
	{
		hide();
	}
	else
	{
		show();
	}
}

void KUiRaid::hide()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->stopUpdate();
	_thisWindow->hide();
	KUiRaidDragPlayer::getSinglton().hide();
}

bool KUiRaid::isVisible()
{
	if(!_thisWindow)
	{
		return false;
	}

	return _thisWindow->isVisible();
}

bool KUiRaid::onClickCloseBtn(const EventArgs& arg)
{
	hide();
	return true;
}

bool KUiRaid::onWheelChanged(const EventArgs& arg)
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&arg;
	
	if(_scrollBar->isVisible())
	{
		_scrollBar->setScrollPosition(_scrollBar->getScrollPosition()
			- _scrollBar->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiRaid::onScroll(const EventArgs& arg)
{
	float scrollPos = _scrollBar->getScrollPosition();
	int panelHeight = _playerPanel->getHeight(Absolute);
	
	if(panelHeight < _playerPanelMaxSize)
	{
		return false;
	}

	int exceedSize = (panelHeight - _playerPanelMaxSize) * scrollPos;
	_playerPanel->setYPosition(Absolute, -exceedSize);
	return true;	
}
	
bool KUiRaid::onUpAreaIn(const EventArgs& args)
{
	if(!KUiRaidDragPlayer::getSinglton().isVisible())
	{
		return false;
	}
	_mouseAtUpArea = true;
	_thisWindow->beginUpdate();
	return false;
}

bool KUiRaid::onUpAreaOut(const EventArgs& args)
{
	_mouseAtUpArea = false;
	_thisWindow->stopUpdate();
	return false;
}

bool KUiRaid::onDownAreaIn(const EventArgs& args)
{
	if(!KUiRaidDragPlayer::getSinglton().isVisible())
	{
		return false;
	}
	_mouseAtDownArea = true;
	_thisWindow->beginUpdate();
	return false;
}

bool KUiRaid::onDownAreaOut(const EventArgs& args)
{
	_mouseAtDownArea = false;
	_thisWindow->stopUpdate();
	return false;
}

bool KUiRaid::onTimer(const EventArgs& args)
{
	FrameEventArgs* eventArg = (FrameEventArgs*)(&args);
// 	static ulong elaspe = 0;
// 	elaspe += eventArg->elapse;
// 	
// 	if(elaspe < 200.0f)
// 	{
// 		return true;
// 	}
// 	elaspe = 0.0f;

	//每200毫秒移动一个单元，移动的平滑程度取决于onTimer的调用频率
	if(!KUiRaidDragPlayer::getSinglton().isVisible())
	{
		return false;
	}
	if(_mouseAtUpArea)
	{
		float upOff = (float)_player[0].getHeight() / _curMemberCount * eventArg->elapse / 200.f;
		_scrollBar->setScrollPosition(_scrollBar->getScrollPosition() - upOff * _scrollBar->getStepSize());
	}
	else if(_mouseAtDownArea)
	{
		float upOff = (float)_player[0].getHeight() / _curMemberCount * eventArg->elapse / 200.f;
		_scrollBar->setScrollPosition(_scrollBar->getScrollPosition() + upOff * _scrollBar->getStepSize());
	}
	return true;
}

const KUiTeamMemberItem& KUiRaid::getPlayerByIndex(int playerIndex)
{
	return _teamInfo[playerIndex];
}

void KUiRaid::switchPlayer(int index1, int index2)
{
	if(!_thisWindow)
	{
		return;
	}

	if(index1 == index2)
	{
		return;
	}

	g_pCoreShell->TeamOperation(TEAM_OI_MOVE_INDEX, (unsigned int)index1, index2);
}
/*==================================================================*/


KUiRaidDragPlayer::KUiRaidDragPlayer()
{
	if(!_thisWindow)
	{
		return;
	}

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);
	_thisWindow->setRenderMode(true);

 	GlobalEventSet::getSingleton().subscribeEvent(Window::EventMouseMove, 
 		Event::Subscriber(&KUiRaidDragPlayer::onMouseMove, &KUiRaidDragPlayer::getSinglton()));
	GlobalEventSet::getSingleton().subscribeEvent(Window::EventMouseButtonDown, 
		Event::Subscriber(&KUiRaidDragPlayer::onMouseBDown, &KUiRaidDragPlayer::getSinglton()));

	_thisWindow->disable();
	
	_thisWindow->setAlpha((float)KUiCfgLoader::getSingleton().getRaidCfg().dragAlpha / 255);
	_assistIcon->hide();
	_leadIcon->hide();

	_thisWindow->setFrameEnabled(true);
	_thisWindow->setBackgroundEnabled(true);

	_thisWindow->setZLevel(Window::Top);

	hide();
}

KUiRaidDragPlayer::~KUiRaidDragPlayer()
{

}

void KUiRaidDragPlayer::moveToMouse()
{
	if(!_thisWindow)
	{
		return;
	}

	Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
 
 	
 	Rect area = _thisWindow->getUnclippedPixelRect();
 	Point pos = area.getPosition();
 
 	Point absPos = mousePos - Point(area.getWidth() / 3, area.getHeight() / 3);

 	_thisWindow->setPosition( Absolute, absPos );
}

bool KUiRaidDragPlayer::onMouseMove(const EventArgs& e)
{
 	if(!_thisWindow->isVisible())
	{
		return false;
	}
 
	moveToMouse();

	return true;
}

bool KUiRaidDragPlayer::onMouseBDown(const EventArgs& e)
{
	MouseEventArgs* event = (MouseEventArgs*)&e;

	if(LeftButton == event->button)
	{
		return false;
	}

 	if(!_thisWindow->isVisible())
	{
		return false;
	}
 
	hide();

	return true;
}

void KUiRaidDragPlayer::show(int playerIndex)
{
	if(!_thisWindow)
	{
		return;
	}

	KUiRaidPlayer::show(playerIndex);

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->removeChildWindow(_thisWindow);
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);

	_hoverFrameImg->hide();
	_pushedFrameImg->show();
	
	moveToMouse();
}