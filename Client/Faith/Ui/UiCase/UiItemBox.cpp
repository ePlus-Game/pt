#include "UiStoreBox.h"
#include "UiDragItem.h"
#include "UiShop.h"
#include "UiPlayerState.h"
#include "UiSplitItemBox.h"
#include "UiTradeConfirmBox.h"
#include "UiItemBox.h"
#include "UiChatWindow.h"
#include "UiChatWindow.h"
#include "../KMessageCentre.h"
#include "UiShortcutWnd.h"
#include "UiShortcutPlusWnd.h"
#include "UiComMsgBox.h"
#include "GameDataDef.h"
#include "UiChatCentre.h"
#include "UiItemLockMgr.h"
#include <crtdbg.h>
#include "../UiAdapter.h"

#include "../UiSheetMgr.h"

using namespace CEGUI;

extern iCoreShell*		g_pCoreShell;

KObjAtContRegion KUiItemBox::_sellItemRegion;

KUiItemBox::KUiItemBox()
{
	_curPage = 1;
	_size = BAG_WIDTH * BAG_HEIGHT;
	
	_loaded = false;

	_lbDown = false;

	KUiItemCDTracker::getSingleton();

	m_InsteadSpecieGold = NULL;
	m_InsteadSpecieSilver = NULL;
	m_InsteadSpecieCopper = NULL;

	KIniFile iniFile;
	if (iniFile.Load(UI_CFG_STRING) == TRUE)
	{
		iniFile.GetInteger("InsteadSpecieIndex", "Index", 1, &m_InsteadIndex);
	}
}

KUiItemBox::~KUiItemBox()
{

}

bool KUiItemBox::isVisible()
{
	if(!_thisWindow)
	{
		return false;
	}

	if(!_loaded)
	{
		return false;
	}
	return _thisWindow->isVisible();
}

void KUiItemBox::hide()
{
	if(!_thisWindow)
	{
		return;
	}

	if(!_loaded)
	{
		return;
	}
	_thisWindow->hide();
}

void KUiItemBox::show()
{
	if(false == _loaded)
	{
		loadUi();
		_loaded = true;
	}

	if(!_thisWindow)
	{
		return;
	}

	_curPage = 1;
	_pageTab[0]->setSelected(true);

	_thisWindow->show();

	//caol- fix bug：打开背包后聊天输入窗口无法继续输入的bug
	//_thisWindow->moveToFront();
	
	showPage();
}

int KUiItemBox::GetInsteadSpecieIndex()
{
	return m_InsteadIndex;
}

void KUiItemBox::UpdateInsteadSpecie()
{
	unsigned long insteadSpecie = 0;
	if (g_pCoreShell != NULL)
	{
		insteadSpecie = g_pCoreShell->GetGameData(GDI_GET_INSTEAD_SPECIE, 0, m_InsteadIndex);
	}
	
	unsigned long gold = 0;
	unsigned long silver = 0;
	unsigned long copper = 0;
	InsteadSpecieToUiSpecie(insteadSpecie, gold, silver, copper);
	
	if (m_InsteadSpecieGold != NULL)
	{
		m_InsteadSpecieGold->setText(iToString(gold));
	}
	
	if (m_InsteadSpecieSilver != NULL)
	{
		m_InsteadSpecieSilver->setText(iToString(silver));
	}
	
	if (m_InsteadSpecieCopper != NULL)
	{
		m_InsteadSpecieCopper->setText(iToString(copper));
	}
}

void KUiItemBox::showPage()
{
	getData();
	
	//禁用没有格子的扩展栏
	int validPage = (_size - 1) / (BAG_HEIGHT * BAG_WIDTH) + 1;
	for(int i = 0; i < validPage; ++i)
	{
		_pageTab[i]->enable();
	}
	for(int j = validPage; j < PAGECOUNT; ++j)
	{
		_pageTab[j]->disable();
	}

	//打开物品栏或者切换页面的时候，更新锁定状态
	freshLockedItem();
}

void KUiItemBox::loadUi()
{	
#ifndef _DEBUG
	try
	{
#endif
	if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
	{
		_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_ITEM_BOX_WINDOW_PATH_1024);
	}
	else
	{
		_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_ITEM_BOX_WINDOW_PATH);
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
	_thisWindow->setDummyWnd(false);

	TLGameObject::GameObject itemObj;
	itemObj.d_gameobject = BACKGROUND_IMAGE;
	itemObj.d_type = TLGameObject::item;

	char ctrName[UI_ITEM_BOX_CTRL_NAME_LEN];

	sprintf(ctrName, "%s/LockedImage", UI_ITEM_BOX_WINDOW_NAME);
	TLStaticImage* lockedImageTemp = (TLStaticImage*)_thisWindow->getChild(ctrName);
	lockedImageTemp->hide();

	for(int i = 0; i < BAG_HEIGHT; i++)
	{
		for(int j = 0; j < BAG_WIDTH; j++)
		{
			sprintf(ctrName, "%s/Item%d-%d", UI_ITEM_BOX_WINDOW_NAME, i + 1, j + 1);
			_item[i][j] = (TLGameObject*)_thisWindow->getChild(ctrName);

			TLGameObject* thisGridCtrl = _item[i][j];
			//设置绘制信息
			thisGridCtrl->setObject(itemObj);

			//设置位置信息
			KObjAtContRegion* itemRegion = &_itemRegion[i][j];
			itemRegion->eContainer = UOC_ITEM_TAKE_WITH;
			itemRegion->Obj.uGenre = CGOG_NOTHING;
			itemRegion->Region.v = i;
			itemRegion->Region.h = j;
			itemRegion->Region.Width = 0;	//0表示挂位非物品
			itemRegion->Region.Height = 0;
			thisGridCtrl->setUserData(itemRegion);
			thisGridCtrl->subscribeEvent(TLGameObject::EventMouseButtonDown, Event::Subscriber(&KUiItemBox::onBDown, this));
			thisGridCtrl->subscribeEvent(TLGameObject::EventMouseButtonUp, Event::Subscriber(&KUiItemBox::onLBUp, this));
			thisGridCtrl->subscribeEvent(TLGameObject::EventMouseLeaves, Event::Subscriber(&KUiItemBox::onLBLeave, this));

			//加入通用事件
			_itemGrid[i][j].setCtrl(thisGridCtrl);
			_itemGrid[i][j].addTip();

			//物品锁定状态图片
			_lockedImage[i][j] = (TLStaticImage*)WindowManager::getSingleton().createWindow(TLStaticImage::WidgetTypeName);
			if(_lockedImage[i][j])
			{
				_lockedImage[i][j]->disable();
				_lockedImage[i][j]->setImage(lockedImageTemp->getImage());
				_lockedImage[i][j]->setWidth(Absolute, lockedImageTemp->getWidth(Absolute));
				_lockedImage[i][j]->setHeight(Absolute, lockedImageTemp->getHeight(Absolute));
				thisGridCtrl->addChildWindow(_lockedImage[i][j]);
			}
		}
	}

	for(int l = 0; l < EXTEND_HEIGHT; l++)
	{
		for(int k = 0; k < EXTEND_WIDTH; k++)
		{
			sprintf(ctrName, "%s/SpecialItem%d", UI_ITEM_BOX_WINDOW_NAME, k + 1);
			_extendItem[l][k] = (TLGameObject*)_thisWindow->getChild(ctrName);

			TLGameObject* thisCtrl = _extendItem[l][k];
			
			thisCtrl->setObject(itemObj);

			//设置位置信息
			KObjAtContRegion* itemRegion = &_extendItemRegion[l][k];
			itemRegion->eContainer = UOC_ITEMBOX_EXTEND;
			itemRegion->Obj.uGenre = CGOG_NOTHING;
			itemRegion->Region.h = k;
			itemRegion->Region.v = l;
			itemRegion->Region.Width = 0;	//0表示挂位非物品
			itemRegion->Region.Height = 0;
			thisCtrl->setUserData(itemRegion);
			thisCtrl->subscribeEvent(TLGameObject::EventMouseButtonDown, Event::Subscriber(&KUiItemBox::onExtendPosBDown, this));
			thisCtrl->subscribeEvent(TLGameObject::EventMouseButtonUp, Event::Subscriber(&KUiItemBox::onExtendLBUp, this));
			thisCtrl->subscribeEvent(TLGameObject::EventMouseLeaves, Event::Subscriber(&KUiItemBox::onExtendLBLeave, this));
			
			//加入通用事件
			_extendItemGrid[l][k].setCtrl(thisCtrl);
			_extendItemGrid[l][k].addTip();
		}
	}

	//分页按钮
	for(int pageIndex = 0; pageIndex < PAGECOUNT; pageIndex++)
	{
		sprintf(ctrName, "%s/Page%d", UI_ITEM_BOX_WINDOW_NAME, pageIndex + 1);
		_pageTab[pageIndex] = (TLRadioButton*)_thisWindow->getChild(ctrName);
		_pageTab[pageIndex] ->subscribeEvent(TLRadioButton::EventSelectStateChanged, Event::Subscriber(&KUiItemBox::onPageChanged, this));
	}

#ifndef _DEBUG
	try
	{
#endif
		String windowName = UI_ITEM_BOX_WINDOW_NAME;
		_jin		= (TLStaticText*)_thisWindow->getChild(windowName + "/Jin");
		_yin		= (TLStaticText*)_thisWindow->getChild(windowName + "/Yin");
		_tong		= (TLStaticText*)_thisWindow->getChild(windowName + "/Tong");

		_jinshanBi	= (TLButton*)_thisWindow->getChild(windowName + "/JinshanBi");

		_weight		= (TLStaticText*)_thisWindow->getChild(windowName + "/Weight");

		_close		= (TLButton*)_thisWindow->getChild(windowName + "/Close");
		_close->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiItemBox::onClose, this));

		m_InsteadSpecieGold		=	static_cast<TLStaticText *>(_thisWindow->getChild(windowName + "/InsteadSpecieTxt_Gold"));
		m_InsteadSpecieSilver	=	static_cast<TLStaticText *>(_thisWindow->getChild(windowName + "/InsteadSpecieTxt_Silver"));
		m_InsteadSpecieCopper	=	static_cast<TLStaticText *>(_thisWindow->getChild(windowName + "/InsteadSpecieTxt_Copper"));
#ifndef _DEBUG
	}
	catch (...)
	{
		_jin		= NULL;
		_yin		= NULL;
		_tong		= NULL;

		_jinshanBi	= NULL;

		_weight		= NULL;

		_close		= NULL;

		m_InsteadSpecieGold		= NULL;
		m_InsteadSpecieSilver	= NULL;
		m_InsteadSpecieCopper	= NULL;	
	}
#endif
}

void KUiItemBox::InsteadSpecieToUiSpecie(unsigned long insteadSpecie, unsigned long & gold, unsigned long & silver, unsigned long & copper)
{
	gold = insteadSpecie / 10000;
	silver = (insteadSpecie % 10000) / 100;
	copper = insteadSpecie % 100;
}

void KUiItemBox::clearAGrid(TLGameObject* gridCtrl, bool disable)
{
	if(!gridCtrl)
	{
		return;
	}
	
	//gameobj 数据
	TLGameObject::GameObject itemObj;
	itemObj.d_gameobject = BACKGROUND_IMAGE;
	itemObj.d_count = 0;

	if(disable)
	{
		itemObj.d_type = TLGameObject::idle;
		itemObj.d_state = TLGameObject::disableState;
		itemObj.d_count = 0;
	}
	else
	{
		itemObj.d_type = TLGameObject::item;
		itemObj.d_state = TLGameObject::normalState;
	}

	gridCtrl->setObject(itemObj);
	gridCtrl->setState(itemObj.d_state);

	//region 数据
	KObjAtContRegion* regionInfo = (KObjAtContRegion*)gridCtrl->getUserData();
	if(regionInfo == NULL)
	{
		_ASSERT(0);
		return;
	}

	regionInfo->Obj.uGenre = CGOG_NOTHING;
	regionInfo->Obj.uId = 0;

	regionInfo->Region.Height = 0;
	regionInfo->Region.Width = 0;	//0表示挂位非物品
}

void KUiItemBox::updatePropertys()
{
	if(!_thisWindow)
	{
		return;
	}

	//显示蕴魂
	int yunhun = g_pCoreShell->GetGameData(GDI_GET_CUR_SKILL_POINT, NULL, NULL);
	
	//显示金山币
	float jinshanBi = g_pCoreShell->GetGameData(GDI_PLAYER_JINSHANBI, NULL, NULL);
	float jinshanbiRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().jinshanbiRate);
	if(jinshanbiRate > 0)
	{
		jinshanBi = static_cast<float>(jinshanBi) / jinshanbiRate;
		char jinshanBiText[COMMON_CLIENT_MSG_LEN_64];
		sprintf(jinshanBiText, "%.2f", jinshanBi);
		_jinshanBi->setText(AnsiToUtf8(jinshanBiText));
	}
	else
	{
		_jinshanBi->setText("");
	}

	//显示负重
	int weightCur, weightMax;
	if ( !g_pCoreShell->GetGameData(GDI_PLAYER_WEIGHT_CURRENT, UINT(&weightCur), INT(&weightMax)) ) 
		return;
	_weight->setText(iToString(weightCur) + '/' + iToString(weightMax));

	//显示当前金钱数
	int money = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);

	UpdateInsteadSpecie();

	int jin, yin, tong;
	sysMoneyToUiMoney(money, jin, yin, tong);
	_jin->setText(iToString(jin));
	_yin->setText(iToString(yin));
	_tong->setText(iToString(tong));

	
};

void KUiItemBox::getData(void)
{
	if(false == _loaded)
	{
		return;
	}

	updatePropertys();

	//显示当前包裹物品，先把当前包裹清空
	int addIndex = (_curPage - 1) * BAG_HEIGHT * BAG_WIDTH;
	for(int i = 0; i < BAG_HEIGHT; i++)
	{
		for(int j = 0; j < BAG_WIDTH; j++)
		{
			//得到item在整个Invatory中的索引
			int itemIndex = i * BAG_WIDTH + j + addIndex;
			if(itemIndex >= _size)
			{
				clearAGrid(_item[i][j], true);
			}
			else
			{
				clearAGrid(_item[i][j], false);
			}
		}
	}
	
	//在core里查询要显示的物品
	KObjAtContRegion items[BAG_GRID_TOTAL_COUNT];
	int count = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, (unsigned int)items, BAG_GRID_TOTAL_COUNT);
	for (int index = 0; index < count; index++)
	{
		pair<int, int> uiPos = actPosToUiPos(pair<int, int>(items[index].Region.h, items[index].Region.v));
		if(-1 == uiPos.first)
			continue;
		
		KObjAtContRegion* pos = (KObjAtContRegion*)_item[uiPos.second][uiPos.first]->getUserData();
		pos->Obj.uGenre = CGOG_ITEM;
		pos->Obj.uId = items[index].Obj.uId;
		pos->Region.Height = items[index].Region.Height;
		
		TLGameObject::GameObject itemObj;
		KUiItemHelper::setItemImage(items[index].Obj.uId, itemObj);
		itemObj.d_type = TLGameObject::item;
		itemObj.d_count = items[index].Region.Height;
		itemObj.d_state = TLGameObject::normalState;
		
		FIND_ITEMINDEX_PARAM tagItemParam;
		g_pCoreShell->GetGameData(GDI_GET_ITEM_PARTICULAR, (unsigned int)&tagItemParam, pos->Obj.uId);
		itemObj.d_genre			= tagItemParam.nGenre;
		itemObj.d_detail		= tagItemParam.nDetail;
		itemObj.d_particular	= tagItemParam.nParticular;
		itemObj.d_level			= tagItemParam.nLevel;
		itemObj.d_group			= tagItemParam.nGroup;
		_item[uiPos.second][uiPos.first]->setObject(itemObj);
	}

	for(int k = 0; k < EXTEND_HEIGHT; k++)
	{
		for(int l = 0; l < EXTEND_WIDTH; l++)
		{
			clearAGrid(_extendItem[k][l], false);
		}
	}

	KObjAtContRegion extend[BAG_EXTEND_GRID_TOTAL_COUNT];
	count = g_pCoreShell->GetGameData(GDI_BAG_EXTEND, (unsigned int)extend, BAG_EXTEND_GRID_TOTAL_COUNT);
	for(int l = 0; l < count; l++)
	{
		TLGameObject::GameObject itemObj;
		KUiItemHelper::setItemImage(extend[l].Obj.uId, itemObj);
		itemObj.d_type = TLGameObject::item;
		itemObj.d_count = extend[l].Region.Height;
 		itemObj.d_state = TLGameObject::normalState;
		_extendItem[extend[l].Region.v][extend[l].Region.h]->setObject(itemObj);
		
		KObjAtContRegion* pos = (KObjAtContRegion*)_extendItem[extend[l].Region.v][extend[l].Region.h]->getUserData();
		pos->Obj.uGenre = CGOG_ITEM;
		pos->Obj.uId = extend[l].Obj.uId;
		pos->Region.Height = extend[l].Region.Height;
	}
}

bool KUiItemBox::onBDown(const EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	
	TLGameObject* destObj = (TLGameObject*)arg->window;

	KObjAtContRegion destPos = *(KObjAtContRegion*)destObj->getUserData();
	pair<int, int> actPos = uiPosToActPos(pair<int, int>(destPos.Region.h, destPos.Region.v));
	if(-1 == actPos.first)
		return true;

	if(KUiItemLockMgr::getSingleton().getLockState(destPos.Obj.uId))
	{
		return true;
	}

	destPos.Region.h = actPos.first;
	destPos.Region.v = actPos.second;

	TLGameObject::GameObject& destObjInfo = destObj->getObject();

	if(arg->button == LeftButton)
	{
		KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
		if(g_UseItem.uId > 0)
		{
			TLGameObject::GameObject tagGO;
			destObj->getObject( tagGO );
			
			if ( tagGO.d_state == TLGameObject::normalState ||
				tagGO.d_state == TLGameObject::hoverState || 
				tagGO.d_state == TLGameObject::pushedState )
			{
				useItem(&destPos);
			}
		}
		else if(arg->sysKeys & Control && (KUiChatInputWnd::GetSingleton().isCurInput() || pRoom && pRoom->IsVisible()))
		{
			KUiItemHelper::sendItemLink(destPos);
		}
		else if(arg->sysKeys & Shift)
		{
			KUiSplitItemBox::getSingleton().open(destPos, destObjInfo);
		}
		else if(KUiPlayerState::getSingleton().getState() == KUiPlayerState::TRADE_NPC_NORMAL_REPAIR
			|| KUiPlayerState::getSingleton().getState() == KUiPlayerState::TRADE_NPC_SPECIAL_REPAIR)
		{
			repairItem(&destPos);
		}
		else// if(KUiPlayerState::getSingleton().getState() == KUiPlayerState::IDLE)
		{
			TLGameObject* sourObj = KUiDragItem::GetSingleton().getObj();
			KObjAtContRegion* sourPos = (KObjAtContRegion*)sourObj->getUserData();

			if(sourPos->Obj.uGenre == CGOG_ITEM)
			{
				dropItem(destPos, destObjInfo, destObj);
				_lbDown = false;
			}
			else
			{
				destObj->releaseInput();
				_lbDown = true;
			}
		}
	}
	else if(arg->button == RightButton)
	{
		if(KUiStoreBox::getSingleton().isVisible())
		{
			moveToStoreBox(destPos);
		}
		else if(KUiShop::IsVisible())
		{
			sellItem(&destPos);
		}
		else if(autoEquipExtendBag(destPos) == false)
		{
			TLGameObject::GameObject tagGO;
			destObj->getObject(tagGO);
			
			if (tagGO.d_state == TLGameObject::normalState
				|| tagGO.d_state == TLGameObject::hoverState)
			{
				useItem(&destPos);
				destObj->intonate();
			}
		}
	}

// 	switch(KUiPlayerState::getSingleton().getState())
// 	{
// 	case KUiPlayerState::IDLE:
// 		{
// 			if(arg->button == LeftButton && g_UseItem.uId <= 0)
// 			{
// 				KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
// 				if((arg->sysKeys & Control) && KUiChatInputWnd::IsVisible() || (pRoom && pRoom->IsVisible() && (arg->sysKeys & Control)))
// 				{
// 					sendItemLink(destPos);
// 				}
// 				else if(arg->sysKeys & Shift)
// 				{
// 					KUiSplitItemBox::getSingleton().open(destPos, destObjInfo);
// 				}
// 				else
// 				{
// 					pickDropItem(destPos, destObjInfo, destObj);
// 				}
// 			}
// 			else if(!destObj->isLock())
// 			{
// 				if(KUiStoreBox::IsVisible())
// 				{
// 					moveToStoreBox(destPos);
// 				}
// 				else if(autoEquipExtendBag(destPos) == false && g_UseItem.uId <= 0 )
// 				{
// 					TLGameObject::GameObject tagGO;
// 					destObj->getObject( tagGO );
// 					
// 					
// 					if ( tagGO.d_state == TLGameObject::normalState ||
// 						tagGO.d_state == TLGameObject::hoverState )
// 					{
// 						useItem(&destPos);
// 						destObj->intonate();
// 					}
// 				}
// 				else if ( arg->button == LeftButton && g_UseItem.uId > 0 )
// 				{
// 					TLGameObject::GameObject tagGO;
// 					destObj->getObject( tagGO );
// 					
// 					
// 					if ( tagGO.d_state == TLGameObject::normalState ||
// 						 tagGO.d_state == TLGameObject::hoverState || 
// 						 tagGO.d_state == TLGameObject::pushedState )
// 					{
// 						useItem(&destPos);
// 					}
// 				}
// 			}
// 
// 		}
// 		break;
// 	case KUiPlayerState::TRADE_NPC_NORMAL_REPAIR:
// 	case KUiPlayerState::TRADE_NPC_SPECIAL_REPAIR:
// 		{
// 			if(destObjInfo.d_type != TLGameObject::item)
// 				return true;
// 			if(arg->button == RightButton && !destObj->isLock())
// 			{
// 				sellItem(&destPos);
// 			}
// 			if(arg->button == LeftButton)
// 			{
// 				repairItem(&destPos);
// 			}
// 		}
// 		break;
// 	case KUiPlayerState::TRADE_NPC_BUY_SALE:
// 		{
// 			if(arg->button == RightButton && !destObj->isLock())
// 			{
// 				sellItem(&destPos);
// 			}
// 			else if(arg->button == LeftButton)
// 			{
// 				if(arg->sysKeys & Control)
// 				{
// 					sendItemLink(destPos);
// 				}
// 				else if(arg->sysKeys & Shift && !destObj->isLock())
// 				{
// 					KUiSplitItemBox::getSingleton().open(destPos, destObjInfo);
// 				}
// 				else
// 				{
// 					pickDropItem(destPos, destObjInfo, destObj);
// 				}
// 			}
// 		}
// 		break;
// 	}
	return true;
}

bool KUiItemBox::onLBUp(const EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	
	TLGameObject* destObj = (TLGameObject*)arg->window;

	KObjAtContRegion destPos = *(KObjAtContRegion*)destObj->getUserData();
	pair<int, int> actPos = uiPosToActPos(pair<int, int>(destPos.Region.h, destPos.Region.v));
	if(-1 == actPos.first)
		return true;

	if(KUiItemLockMgr::getSingleton().getLockState(destPos.Obj.uId))
	{
		return true;
	}

	destPos.Region.h = actPos.first;
	destPos.Region.v = actPos.second;

	TLGameObject::GameObject& destObjInfo = destObj->getObject();

	if(arg->button == RightButton)
	{
		return false;
	}

	if(_lbDown)
	{
		pickItem(destPos, destObjInfo, destObj);
	}
	else
	{
		dropItem(destPos, destObjInfo, destObj);
	}

	_lbDown = false;
	return true;
}

bool KUiItemBox::onLBLeave(const EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	
	TLGameObject* destObj = (TLGameObject*)arg->window;

	KObjAtContRegion destPos = *(KObjAtContRegion*)destObj->getUserData();
	pair<int, int> actPos = uiPosToActPos(pair<int, int>(destPos.Region.h, destPos.Region.v));
	if(-1 == actPos.first)
		return true;

	destPos.Region.h = actPos.first;
	destPos.Region.v = actPos.second;

	TLGameObject::GameObject& destObjInfo = destObj->getObject();

	if(_lbDown)
	{
		pickItem(destPos, destObjInfo, destObj);
	}

	_lbDown = false;
	return true;
}

bool KUiItemBox::onExtendPosBDown(const EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	TLGameObject* destObj = (TLGameObject*)arg->window;

	KObjAtContRegion destPos = *(KObjAtContRegion*)destObj->getUserData();
	TLGameObject::GameObject& destObjInfo = destObj->getObject();

	TLGameObject* sourObj = KUiDragItem::GetSingleton().getObj();
	KObjAtContRegion* sourPos = (KObjAtContRegion*)sourObj->getUserData();
	
	if(sourPos->Obj.uGenre == CGOG_ITEM)
	{
		dropItem(destPos, destObjInfo, destObj);
		_lbDown = false;
	}
	else
	{
		_lbDown = true;
		destObj->releaseInput();
	}

	return true;
}

bool KUiItemBox::onExtendLBUp(const EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	TLGameObject* destObj = (TLGameObject*)arg->window;

	KObjAtContRegion destPos = *(KObjAtContRegion*)destObj->getUserData();
	TLGameObject::GameObject& destObjInfo = destObj->getObject();

	if(_lbDown)
	{
		pickItem(destPos, destObjInfo, destObj);
	}
	else
	{
		dropItem(destPos, destObjInfo, destObj);
	}

	_lbDown = false;

	return true;
}

bool KUiItemBox::onExtendLBLeave(const EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	TLGameObject* destObj = (TLGameObject*)arg->window;

	KObjAtContRegion destPos = *(KObjAtContRegion*)destObj->getUserData();
	TLGameObject::GameObject& destObjInfo = destObj->getObject();

	if(_lbDown)
	{
		pickItem(destPos, destObjInfo, destObj);
	}

	_lbDown = false;
	return true;
}

bool KUiItemBox::autoEquipExtendBag(KObjAtContRegion& itemRegion)
{	
	if(false == _loaded)
	{
		return false;
	}

	ItemType type;
	g_pCoreShell->GetGameData( GDI_GET_ITEM_TYPE_BY_INDEX, (unsigned int)&type, itemRegion.Obj.uId);

	if(type.genre != item_task || type.level != QK_Bag)
	{
		return false;
	}
	for(int i = 0; i < EXTEND_HEIGHT; ++i)
	{
		for(int j = 0; j < EXTEND_WIDTH; ++j)
		{
			KObjAtContRegion* extRegion = (KObjAtContRegion*)_extendItem[i][j]->getUserData();
			if(extRegion->Obj.uGenre != CGOG_NOTHING)
			{
				continue;
			}
			g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (unsigned int)&itemRegion, (int)extRegion);
			return true;
		}
	}

	return false;
}

void KUiItemBox::moveToStoreBox(KObjAtContRegion& sourPos)
{
	KObjAtContRegion itemBoxRegion;
	itemBoxRegion.eContainer = UOC_STORE_BOX;
	int nRet = g_pCoreShell->OperationRequest(GOI_FIND_A_EMPTY_PLACE_OF_A_CONTAINER, (UINT)&itemBoxRegion, NULL);
	if(nRet <= 0)
	{
		char* message = KMessageCentre::GetMessage(storebox_error_message, KUiStoreBox::StoreFull);
		KUiChannelCentre::GetSingleton().toSysMsg(message);
		return;
	}
	
	nRet = g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (UINT)&sourPos, (int)&itemBoxRegion);
	if(nRet > 0)
	{
		KUiDragItem::GetSingleton().initItem();
	}
}

void KUiItemBox::pickItem(KObjAtContRegion& destPos, TLGameObject::GameObject& destObjInfo, TLGameObject* destObj)
{
	TLGameObject* sourObj = KUiDragItem::GetSingleton().getObj();

	TLGameObject::GameObject& sourObjInfo = sourObj->getObject();
	KObjAtContRegion* sourPos = (KObjAtContRegion*)sourObj->getUserData();
	
	if(sourPos->Obj.uGenre != CGOG_NOTHING)
	{
		return;
	}

	if(destPos.Obj.uGenre == CGOG_NOTHING)
	{
		return;
	}
	
	sourObj->setObject(destObjInfo);
	*sourPos = destPos;
	sourObj->setCanDrag(true);
	
	ItemType type;
	g_pCoreShell->GetGameData(GDI_GET_ITEM_TYPE_BY_INDEX, (UINT)&type, destPos.Obj.uId);
	KUiItemHelper::playItemPickSoundEffect(type);
}

void KUiItemBox::dropItem(KObjAtContRegion& destPos, TLGameObject::GameObject& destObjInfo, TLGameObject* destObj)
{
	TLGameObject* sourObj = KUiDragItem::GetSingleton().getObj();

	TLGameObject::GameObject& sourObjInfo = sourObj->getObject();
	KObjAtContRegion* sourPos = (KObjAtContRegion*)sourObj->getUserData();

	if(sourPos->Obj.uGenre == CGOG_NOTHING)
	{
		return;
	}

	int nRet = g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (unsigned int)sourPos, (int)&destPos);
	if(nRet > 0)
	{
		KUiDragItem::GetSingleton().initItem();
		//playSound(KUiCfgLoader::getSingleton().getSoundEffectCfg().dropdown);
		
		ItemType type;
		g_pCoreShell->GetGameData(GDI_GET_ITEM_TYPE_BY_INDEX, (UINT)&type, destPos.Obj.uId);
		KUiItemHelper::playItemDropSoundEffect(type);
	}
}

void KUiItemBox::processSellItem()
{	
	g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL, (unsigned int)&_sellItemRegion, NULL);
}

void KUiItemBox::sellItem( KObjAtContRegion* itemRegion )
{
	ItemType type;
	int itemId = g_pCoreShell->GetGameData(GDI_GET_ITEM_ID_BY_INDEX, itemRegion->Obj.uId, NULL);
	g_pCoreShell->GetGameData(GDI_GET_ITEM_TYPE_BY_ID, (unsigned int)&type, itemId);
	int quality = g_pCoreShell->GetGameData(GDI_GET_ITEM_QUALITY_BY_TYPE, (unsigned int)&type, NULL);
	
	int AutoSellQuality = 1;
	int shopIdxTemp = g_pCoreShell->GetGameData(GDI_GET_SHOP_IDX, NULL, NULL);
	int currentShopIndex = g_pCoreShell->GetGameData(GDI_GET_PLUS_POINT_INDEX_BY_SHOP_INDEX, 0, shopIdxTemp);
	static int insteadSpecieIndex = KUiItemBox::getSingleton().GetInsteadSpecieIndex();
	if ( quality >= AutoSellQuality || ( insteadSpecieIndex == currentShopIndex ) )
	{
		_sellItemRegion = *itemRegion;

		char itemName[COMMON_CLIENT_MSG_LEN_128];
		g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_WITH_COLOR_BY_INDEX, (UINT)itemName, itemRegion->Obj.uId);

		char sellItemMsg[COMMON_CLIENT_MSG_LEN_256];
		sprintf(sellItemMsg, KUiCfgLoader::getSingleton().getCommonCfg().sellItemMsg, itemName);
		
		KUiComMsgBox::GetSingleton().setComMsgPosition();
		KUiComMsgBox::Show();
		char yesString[COMMON_CLIENT_MSG_LEN_8];
		char noString[COMMON_CLIENT_MSG_LEN_8];
		strcpy(yesString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().yesString));
		strcpy(noString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().noString));
		KUiComMsgBox::GetSingleton().setBtnName((utf8*)yesString, (utf8*)noString);
		KUiComMsgBox::GetSingleton().setLayoutMsg(sellItemMsg);

		KUiComMsgBox::GetSingleton().setFristBtnCallback(KUiItemBox::processSellItem);
	}
	else
	{
		g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL, (unsigned int)itemRegion, NULL);
	}
}

static KObjAtContRegion objRegin;

void EquipItem( void )
{
	g_pCoreShell->OperationRequest(GOI_USE_ITEM, (unsigned int)&objRegin, NULL);
	
	ItemType type;
	g_pCoreShell->GetGameData(GDI_GET_ITEM_TYPE_BY_INDEX, (UINT)&type, objRegin.Obj.uId);
	KUiItemHelper::playItemUsingSoundEffect(type);
}

void KUiItemBox::useItem( KObjAtContRegion* itemRegion )
{
	KItemInfo tagItemInfo;
	g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&tagItemInfo, itemRegion->Obj.uId );
	if ( tagItemInfo.bIsEquipBind && itemRegion && MOUSE_CURSOR_TARGETITEM != KUiAdapter::GetMouseRes() )
	{
		memcpy(&objRegin, itemRegion, sizeof(objRegin) );
		KUiComMsgBox::GetSingleton().setComMsgPosition();
		KUiComMsgBox::Show();
		char yesString[COMMON_CLIENT_MSG_LEN_8];
		char noString[COMMON_CLIENT_MSG_LEN_8];
		ZeroMemory(yesString, COMMON_CLIENT_MSG_LEN_8);
		ZeroMemory(noString, COMMON_CLIENT_MSG_LEN_8);

		strncpy(yesString, KUiCfgLoader::getSingleton().getCommonCfg().yesString, COMMON_CLIENT_MSG_LEN_8);
		strncpy(noString, KUiCfgLoader::getSingleton().getCommonCfg().noString, COMMON_CLIENT_MSG_LEN_8);
		KUiComMsgBox::GetSingleton().setBtnName(AnsiToUtf8(yesString), AnsiToUtf8(noString));

		char szBuff[COMMON_CLIENT_MSG_LEN_1024];
		sprintf( szBuff, "<Seg text-align=left float=wrap><Obj color=250,250,250>%s</Obj></Seg></Layout>", ITEM_CANNT_EQUIP );
		KUiComMsgBox::GetSingleton().setLayoutMsg(szBuff);
		
		KUiComMsgBox::GetSingleton().setFristBtnCallback(EquipItem);
	}
	else
	{
		g_pCoreShell->OperationRequest(GOI_USE_ITEM, (unsigned int)itemRegion, NULL);
		
		ItemType type;
		g_pCoreShell->GetGameData(GDI_GET_ITEM_TYPE_BY_INDEX, (UINT)&type, itemRegion->Obj.uId);
		KUiItemHelper::playItemUsingSoundEffect(type);
	}
}

void KUiItemBox::repairItem( KObjAtContRegion* itemRegion )
{	
	KUiTradeConfirmBox::GetSingleton().show(itemRegion);
}


void KUiItemBox::onItemChanged(KObjAtContRegion* pObj, int add)
{
	if(!_thisWindow)
	{
		return;
	}

	if(!_loaded)
	{
		return;
	}

	if(!_thisWindow->isVisible())
	{
		return;
	}
	TLGameObject::GameObject objInfo;
	if(pObj->Obj.uGenre == CGOG_MONEY)//如果改变的是自己放在交易面版上的钱
	{
		int money = pObj->Obj.uId;
		int jin, yin, tong;
		sysMoneyToUiMoney(money, jin, yin, tong);
		_jin->setText(iToString(jin));
		_yin->setText(iToString(yin));
		_tong->setText(iToString(tong));
		return;
	}
	
	pair<int, int> uiPos = actPosToUiPos(pair<int, int>(pObj->Region.h, pObj->Region.v));
	if(-1 == uiPos.first)
		return;

	TLGameObject* changedItem = _item[uiPos.second][uiPos.first];
	if(add)//如果是增加物品或更新物品
	{
		if(!pObj->Obj.uGenre)
			return;
		KObjAtContRegion* itemRegion = (KObjAtContRegion*)changedItem->getUserData();
		
		itemRegion->Obj = pObj->Obj;
		itemRegion->Region.Height = pObj->Region.Height;
		KUiItemHelper::setItemImage(pObj->Obj.uId, objInfo);
		objInfo.d_type = TLGameObject::item;
		objInfo.d_count = pObj->Region.Height;
		changedItem->setObject(objInfo);
	}
	else//如果是减少物品
	{
		KObjAtContRegion* temp = (KObjAtContRegion*)changedItem->getUserData();
		if ( temp->Obj.uId > 0 )
		{
			FIND_ITEMINDEX_PARAM tagItemInfo;
			g_pCoreShell->GetGameData( GDI_GET_ITEM_PARTICULAR, (unsigned int)&tagItemInfo, temp->Obj.uId );

			// 如果快捷栏有此物品，从新存此物品
			KUiShortcutWnd::GetSingletonPtr()->RefreshImmediacy( tagItemInfo );
			KUiShortcutPlusWnd::GetSingletonPtr()->RefreshImmediacy( tagItemInfo );
		}

		clearAGrid(changedItem, false);
	}

	freshLockedItem();
}

void KUiItemBox::onItemExtendChanged(KObjAtContRegion* pObj, int add)
{
	if(!_thisWindow)
	{
		return;
	}

	if(!_loaded)
	{
		return;
	}

	if(!_thisWindow->isVisible())
	{
		return;
	}

	TLGameObject::GameObject objInfo;
	TLGameObject* changedCtrl = _extendItem[pObj->Region.v][pObj->Region.h];
	if(add)	//如果是增加物品或更新物品
	{
		if(!pObj->Obj.uGenre)
			return;

		KObjAtContRegion* itemRegion = (KObjAtContRegion*)changedCtrl->getUserData();
		itemRegion->Obj = pObj->Obj;
		itemRegion->Region.Height = pObj->Region.Height;
		KUiItemHelper::setItemImage(pObj->Obj.uId, objInfo);
		objInfo.d_type = TLGameObject::item;
		objInfo.d_count = pObj->Region.Height;
		changedCtrl->setObject(objInfo);
	}
	else	//如果是减少物品
	{
		KObjAtContRegion* temp = (KObjAtContRegion*)changedCtrl->getUserData();
		if ( temp->Obj.uId > 0 )
		{
			FIND_ITEMINDEX_PARAM tagItemInfo;
			g_pCoreShell->GetGameData( GDI_GET_ITEM_PARTICULAR, (unsigned int)&tagItemInfo, temp->Obj.uId );

			//如果快捷栏有此物品，从新存此物品
			KUiShortcutWnd::GetSingletonPtr()->RefreshImmediacy( tagItemInfo );
			KUiShortcutPlusWnd::GetSingletonPtr()->RefreshImmediacy( tagItemInfo );
		}

		clearAGrid(changedCtrl, false);
	}
}

void KUiItemBox::onWeighChanged(int cur, int max)
{	
	if(!_thisWindow)
	{
		return;
	}

	if(!_loaded)
	{
		return;
	}

	if(!_thisWindow->isVisible())
	{
		return;
	}
	_weight->setText(iToString(cur) + '/' + iToString(max));
}

void KUiItemBox::onBagSized(int newSize)
{
	_size = newSize;
	
	if(!_thisWindow)
	{
		return;
	}
	
	if(!_loaded)
	{
		return;
	}

	if(!_thisWindow->isVisible())
	{
		return;
	}

	showPage();
}

void KUiItemBox::onJinShanBiChanged(int newCount)
{
	if(!_thisWindow)
	{
		return;
	}

	if(false == _loaded)
	{
		return;
	}
	_jinshanBi->setText(CEGUI::PropertyHelper::intToString(newCount));
}

bool KUiItemBox::onPageChanged(const EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);

	for(int i = 0; i < PAGECOUNT; ++i)
	{
		if(_pageTab[i] != arg->window)
			continue;
		_curPage = i + 1;
		showPage();
		break;
	}
	return true;
}

bool KUiItemBox::onClose(const EventArgs& e)
{
	_thisWindow->hide();
	return true;
}

pair<int, int> KUiItemBox::uiPosToActPos(const pair<int, int>& uiPos)
{
	if(uiPos.first < 0 || uiPos.first >= BAG_WIDTH 
		|| uiPos.second < 0 || uiPos.second >= BAG_HEIGHT)
	{
		return pair<int, int>(-1, -1);
	}

	pair<int, int> actPos;
	actPos.first = uiPos.first;
	actPos.second = uiPos.second + (_curPage - 1) * BAG_HEIGHT;
	return actPos;
}

pair<int, int> KUiItemBox::actPosToUiPos(const pair<int, int>& actPos)
{
	pair<int, int> uiPos;
	uiPos.first = actPos.first;
	uiPos.second = actPos.second - (_curPage - 1) * BAG_HEIGHT;

	if(uiPos.first < 0 || uiPos.first >= BAG_WIDTH 
		|| uiPos.second < 0 || uiPos.second >= BAG_HEIGHT)
	{
		uiPos.first = -1;
		uiPos.second = -1;
	}
	return uiPos;
}

unsigned int KUiItemBox::beginGroupCD(KItemGroupCD_C* groupCdInfo)
{
	if(!_thisWindow)
	{
		return 0;
	}

	if(!_loaded)
	{
		return 0;
	}

	for(int x = 0; x < BAG_WIDTH; ++x)
	{
		for (int y = 0; y < BAG_HEIGHT; ++y)
		{
			if(!_item[y][x])
			{
				_ASSERT(0);
				continue;
			}

			if(_item[y][x]->isEmpty())
			{
				continue;
			}

			KObjAtContRegion* regionInfo = (KObjAtContRegion*)_item[y][x]->getUserData();
			if(!regionInfo)
			{
				_ASSERT(0);
				continue;
			}
			int itemIndex = regionInfo->Obj.uId;
				
			KItemInfo itemInfo;
			ZeroMemory(&itemInfo, sizeof(KItemInfo));
			g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&itemInfo, itemIndex);
			if(itemInfo.nGroup <= 0)
			{
				continue;
			}
			
			if(itemInfo.nGroup != groupCdInfo->nGroup)
			{
				continue;
			}
			
			if (_item[y][x]->getState() == TLGameObject::normalState)
			{
				_item[y][x]->setCoolingTime(groupCdInfo->ulCDTime);
				_item[y][x]->intonate();
			}
		}
	}

	return 0;
}

unsigned int KUiItemBox::endGroupCD(KItemGroupCD_C* pGroupCD)
{
	if(!_thisWindow)
	{
		return 0;
	}

	if(!_loaded)
	{
		return 0;
	}

	for ( int x = 0; x < BAG_WIDTH; ++x)
	{
		for (int y = 0; y < BAG_HEIGHT; ++y)
		{
			if(!_item[y][x])
			{
				_ASSERT(0);
				continue;
			}

			if(_item[y][x]->isEmpty())
			{
				continue;
			}

			KObjAtContRegion* regionInfo = (KObjAtContRegion*)_item[y][x]->getUserData();
			if(!regionInfo)
			{
				_ASSERT(0);
				continue;
			}

			int itemIndex = regionInfo->Obj.uId;

			KItemInfo itemInfo;
			ZeroMemory(&itemInfo, sizeof(KItemInfo));
			g_pCoreShell->GetGameData(GDI_ITEM_INFO_INDEX, (unsigned int)&itemInfo, itemIndex);
			if(itemInfo.nGroup <= 0)
			{
				continue;
			}

			if(itemInfo.nGroup == pGroupCD->nGroup)
			{
				_item[y][x]->setState(TLGameObject::normalState);
			}
		}
	}
	return 0;
}

KUiItemBox& KUiItemBox::getSingleton()
{
	static KUiItemBox singleton;
	return singleton;
}

void KUiItemBox::freshLockedItem()
{
	if(!_thisWindow)
	{
		return;
	}

	for(int i = 0; i < BAG_HEIGHT; i++)
	{
		for(int j = 0; j < BAG_WIDTH; j++)
		{
			if(!_lockedImage[i][j])
			{
				continue;
			}

			_lockedImage[i][j]->hide();
			if(_itemRegion[i][j].Obj.uGenre == CGOG_NOTHING)
			{
				continue;
			}

			if(KUiItemLockMgr::getSingleton().getLockState(_itemRegion[i][j].Obj.uId))
			{
				_lockedImage[i][j]->show();
			}
		}
	}
}



KUiItemCDTracker::KUiItemCDTracker()
{
	initMDL();
}

KUiItemCDTracker::~KUiItemCDTracker()
{

}

KUiItemCDTracker& KUiItemCDTracker::getSingleton()
{
	static KUiItemCDTracker singleton;
	return singleton;
}

void KUiItemCDTracker::initMDL()
{
	GetMDLPtr(&_mdlMgr);

	IUIMDLDataset* pDataset = NULL;
	int nRet = _mdlMgr->queryDataSet( itemgroupcd_dataset, &pDataset );
	if(success_errorcode != nRet && dataset_areadycreated_errorcode != nRet)
	{
		_mdlMgr->createDataSet(itemgroupcd_dataset );
		_mdlMgr->queryDataSet(itemgroupcd_dataset, &pDataset);
	}
	pDataset->setEventHandle(this);
}

void KUiItemCDTracker::onCreate(UIMDLEvent& rEvent)
{

}

void KUiItemCDTracker::onRelease(UIMDLEvent& rEvent)
{

}

void KUiItemCDTracker::onChange(UIMDLEvent& rEvent)
{
	IUIMDLDataset* pIDataset = rEvent.pDataSet;
	if(!pIDataset)
	{
		return;
	}
	
	UIMDLDatasetRecord &rRecord = pIDataset->getDataRecord(rEvent.nRecordIndex);
	KItemGroupCD_C* pCD = (KItemGroupCD_C*)rRecord.pRecordData;
	if(!pCD)
	{
		return;
	}

	if(pCD->ulCDTime <= 0 && pCD->dwStartCount == 0)
	{
		KUiItemBox::getSingleton().endGroupCD(pCD);
	}
	else
	{
		KUiItemBox::getSingleton().beginGroupCD(pCD);
	}
}

//======================================================================================

void KUiItemHelper::setItemImage(int itemId, TLGameObject::GameObject& obj)
{
	KItemInfo tagItemInfo;
	g_pCoreShell->GetGameData(GDI_ITEM_INFO_INDEX, (unsigned int)&tagItemInfo, itemId);
	obj.d_gameobjectSet = tagItemInfo.szImageSet;
	obj.d_gameobject = tagItemInfo.szImage;
	obj.d_EdgeframeIdx = tagItemInfo.colour;
}

void KUiItemHelper::playItemPickSoundEffect(const ItemType& itemType)
{
	KBASICPROP_ITEM* itemInfo;
	g_pCoreShell->GetGameData(GDI_GET_ITEM_BASIC_INFO_BY_TYPE, (unsigned int)&itemInfo, (int)&itemType);
	playSound(itemInfo->SoundEffects[item_sound_effect_pickup]);
}

void KUiItemHelper::playItemDropSoundEffect(const ItemType& itemType)
{
	KBASICPROP_ITEM* itemInfo;
	g_pCoreShell->GetGameData(GDI_GET_ITEM_BASIC_INFO_BY_TYPE, (unsigned int)&itemInfo, (int)&itemType);
	playSound(itemInfo->SoundEffects[item_sound_effect_putdown]);
}

void KUiItemHelper::playItemUsingSoundEffect(const ItemType& itemType)
{
	KBASICPROP_ITEM* itemInfo;
	g_pCoreShell->GetGameData(GDI_GET_ITEM_BASIC_INFO_BY_TYPE, (unsigned int)&itemInfo, (int)&itemType);
	playSound(itemInfo->SoundEffects[item_sound_effect_use]);
}

void KUiItemHelper::sendItemLink(KObjAtContRegion &destPos)
{
	KItemInfo tagItemInfo;
	ZeroMemory( &tagItemInfo, sizeof(KItemInfo));
	g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&tagItemInfo, (int)destPos.Obj.uId );
	
	int itemId = g_pCoreShell->GetGameData( GDI_GET_ITEM_ID_BY_INDEX, (unsigned int)destPos.Obj.uId, NULL);
	
	int hashId = GenerateItemHashId(
		tagItemInfo.itemIdx.nGenre, 
		tagItemInfo.itemIdx.nDetail, 
		tagItemInfo.itemIdx.nParticular);
	
	LOElemInfo itemElem;
	wchar_t* itemContent = NULL;
	ansiToUnicode(tagItemInfo.szName, itemContent);
	wchar_t itemDescription[COMMON_CLIENT_MSG_LEN_64];
	itemDescription[0] = 0;
	wcscat(itemDescription, L"[");
	wcscat(itemDescription, itemContent);
	wcscat(itemDescription, L"]");
	
	itemElem.elemType = LO_TEXT;
	itemElem.isShowDes = true;
	itemElem.gameObj._objType = LO_GO_ITEM;
	itemElem.gameObj._objId[0] = hashId;
	itemElem.gameObj._objId[1] = tagItemInfo.itemIdx.nLevel;
	itemElem.gameObj._objId[2] = itemId;
	itemElem.content = itemContent;
	itemElem.description = itemDescription;
	KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
	if ( pRoom && pRoom->IsVisible() )
	{
		pRoom->write(itemElem);
	}
	else
	{
		KUiChatInputWnd::GetSingleton().write(itemElem);
		KUiChatInputWnd::GetSingleton().show();
	}
	
	delete[] itemContent;
	itemContent = NULL;
}
