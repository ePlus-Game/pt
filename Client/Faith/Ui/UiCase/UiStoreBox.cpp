#include "UiStoreBox.h"
#include "UiItemBox.h"
#include "UiDragItem.h"
#include "UiPlayerState.h"
#include "UiSplitItemBox.h"
#include "UiErrorMessageBox.h"
#include "UiChatCentre.h"
#include "UiChatWindow.h"
#include "../KMessageCentre.h"
#include <crtdbg.h>
#include "../UiAdapter.h"
#include "UiComMsgBox.h"
#include "UiItemPassword.h"
#include "UiIBShop.h"
#include "UiCreditShop.h"

using namespace CEGUI;

extern iCoreShell*		g_pCoreShell;

KUiStoreBox::KUiStoreBox()
{
	_size = BAG_WIDTH * BAG_HEIGHT;
	_thisWindow = NULL;
	_isLoad = false;
	_lbDown = false;

	m_btnModifyPassword = NULL;
}

KUiStoreBox::~KUiStoreBox()
{

}

KUiStoreBox& KUiStoreBox::getSingleton()
{
	static KUiStoreBox singleton;
	return singleton;
}

void KUiStoreBox::loadUi()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_STORE_BOX_WND_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_STORE_BOX_WND_PATH);
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
	
	_thisWindow->setDummyWnd(false);
	_thisWindow->hide();

	char ctrlName[UI_STORE_BOX_WND_NAME_MAX_LEN];
	TLGameObject::GameObject itemObj;
	itemObj.d_gameobject = BACKGROUND_IMAGE;
	itemObj.d_type = TLGameObject::item;

	for(int i = 0; i < BAG_HEIGHT; i++)
	{
		for(int j = 0; j < BAG_WIDTH; j++)
		{
			sprintf(ctrlName, "%s/Item%d-%d", UI_STORE_BOX_WND_NAME, i + 1, j + 1);
			_item[i][j] = (TLGameObject*)_thisWindow->getChild(ctrlName);
			
			TLGameObject* thisGridCtrl = _item[i][j];
			//设置绘制信息
			thisGridCtrl->setObject(itemObj);

			//设置位置信息
			KObjAtContRegion* itemRegion = &_itemRegion[i][j];
			itemRegion->eContainer = UOC_STORE_BOX;
			itemRegion->Obj.uGenre = CGOG_NOTHING;
			itemRegion->Region.v = i;
			itemRegion->Region.h = j;
			itemRegion->Region.Width = 0;	//0表示挂位非物品
			itemRegion->Region.Height = 0;
			thisGridCtrl->setUserData(itemRegion);
			thisGridCtrl->subscribeEvent(TLGameObject::EventMouseButtonDown, Event::Subscriber(&KUiStoreBox::onBDown, this));
			thisGridCtrl->subscribeEvent(TLGameObject::EventMouseButtonUp, Event::Subscriber(&KUiStoreBox::onLBUp, this));
			thisGridCtrl->subscribeEvent(TLGameObject::EventMouseLeaves, Event::Subscriber(&KUiStoreBox::onLBLeave, this));

			//加入通用事件
			_itemGrid[i][j].setCtrl(thisGridCtrl);
			_itemGrid[i][j].addTip();
		}
	}

	for(int l = 0; l < EXTEND_HEIGHT; l++)
	{
		for(int k = 0; k < EXTEND_WIDTH; k++)
		{
			sprintf(ctrlName, "%s/SpecialItem%d", UI_STORE_BOX_WND_NAME, k + 1);
			_extendItem[l][k] = (TLGameObject*)_thisWindow->getChild(ctrlName);

			TLGameObject* thisCtrl = _extendItem[l][k];
			
			thisCtrl->setObject(itemObj);

			//设置位置信息
			KObjAtContRegion* itemRegion = &_extendItemRegion[l][k];
			itemRegion->eContainer = UOC_STORE_EXTEND;
			itemRegion->Obj.uGenre = CGOG_NOTHING;
			itemRegion->Region.h = k;
			itemRegion->Region.v = l;
			itemRegion->Region.Width = 0;	//0表示挂位非物品
			itemRegion->Region.Height = 0;
			thisCtrl->setUserData(itemRegion);
			thisCtrl->subscribeEvent(TLGameObject::EventMouseButtonDown, Event::Subscriber(&KUiStoreBox::onExtendPosBDown, this));
			thisCtrl->subscribeEvent(TLGameObject::EventMouseButtonUp, Event::Subscriber(&KUiStoreBox::onExtendLBUp, this));
			thisCtrl->subscribeEvent(TLGameObject::EventMouseLeaves, Event::Subscriber(&KUiStoreBox::onExtendLBLeave, this));
			
			//加入通用事件
			_extendItemGrid[l][k].setCtrl(thisCtrl);
			_extendItemGrid[l][k].addTip();
		}
	}

	//分页按钮
	for(int pageIndex = 0; pageIndex < PAGECOUNT; pageIndex++)
	{
		sprintf(ctrlName, "%s/Page%d", UI_STORE_BOX_WND_NAME, pageIndex + 1);
		_pageTab[pageIndex] = (TLRadioButton*)_thisWindow->getChild(ctrlName);
		_pageTab[pageIndex] ->subscribeEvent(TLRadioButton::EventSelectStateChanged, Event::Subscriber(&KUiStoreBox::onPageChanged, this));
	}

	String windowName = UI_STORE_BOX_WND_NAME;
	_jin		= (TLStaticText*)_thisWindow->getChild(windowName + "/Jin");
	_yin		= (TLStaticText*)_thisWindow->getChild(windowName + "/Yin");
	_tong		= (TLStaticText*)_thisWindow->getChild(windowName + "/Tong");
	_saveMoney	= (TLButton*)_thisWindow->getChild(windowName + "/SaveMoney");
	_getMoney	= (TLButton*)_thisWindow->getChild(windowName + "/GetMoney");

	_close		= (TLButton*)_thisWindow->getChild(windowName + "/Close");	
	_close->subscribeEvent(TLGameObject::EventClicked, Event::Subscriber(&KUiStoreBox::onClose, this));

	//金钱输入窗口……begin
	_moneyBox		= (TLStaticImage*)_thisWindow->getChild(windowName + "/Moneybox");
	_moneyBox->setDragMovingEnabled(false);

	_mbox_jin		= (TLEditbox*)_moneyBox->getChild(windowName + "/Moneybox/Jin");
	_mbox_yin		= (TLEditbox*)_moneyBox->getChild(windowName + "/Moneybox/Yin");
	_mbox_tong		= (TLEditbox*)_moneyBox->getChild(windowName + "/Moneybox/Tong");
	_mbox_ok		= (TLButton*)_moneyBox->getChild(windowName + "/Moneybox/Ok");
	_mbox_cancel	= (TLButton*)_moneyBox->getChild(windowName + "/Moneybox/Cancel");
	TLButton* mbox_close = (TLButton*)_moneyBox->getChild(windowName + "/Moneybox/Close");
	_moneyBox->subscribeEvent(TLStaticImage::EventShown, Event::Subscriber(&KUiStoreBox::onMboxShow, this));
	_moneyBox->subscribeEvent(TLStaticImage::EventHidden, Event::Subscriber(&KUiStoreBox::onMboxHide, this));
	_mbox_ok->subscribeEvent(TLGameObject::EventClicked, Event::Subscriber(&KUiStoreBox::onMboxOk, this));
	_mbox_cancel->subscribeEvent(TLGameObject::EventClicked, Event::Subscriber(&KUiStoreBox::onMboxCancel, this));
	_saveMoney->subscribeEvent(TLGameObject::EventClicked, Event::Subscriber(&KUiStoreBox::onMboxSaveMoney, this));
	_getMoney->subscribeEvent(TLGameObject::EventClicked, Event::Subscriber(&KUiStoreBox::onMboxGetMoney, this));
	_mbox_jin->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiStoreBox::handleKeyDown, this));
	_mbox_yin->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiStoreBox::handleKeyDown, this));
	_mbox_tong->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiStoreBox::handleKeyDown, this));
	_mbox_jin->subscribeEvent(Editbox::EventTextChanged, Event::Subscriber(&KUiStoreBox::onAddjustMoney, this));
	_mbox_yin->subscribeEvent(Editbox::EventTextChanged, Event::Subscriber(&KUiStoreBox::onAddjustMoney, this));
	_mbox_tong->subscribeEvent(Editbox::EventTextChanged, Event::Subscriber(&KUiStoreBox::onAddjustMoney, this));
	mbox_close->subscribeEvent(TLGameObject::EventClicked, Event::Subscriber(&KUiStoreBox::onMboxCancel, this));
	//金钱输入窗口……end

	_thisWindow->subscribeEvent(TLStaticImage::EventHidden, Event::Subscriber(&KUiStoreBox::onWndHide, this));
	m_strCreatePassword = _thisWindow->getChild( "TaharezLook/StoreBox/btnCreatePassword" )->getText();

	m_btnModifyPassword = static_cast< TLButton* >( _thisWindow->getChild( "TaharezLook/StoreBox/btnModifyPassword" ) ) ;
	m_strModifyPassword = m_btnModifyPassword->getText();

	m_btnModifyPassword->subscribeEvent(
		Window::EventClicked, 
		Event::Subscriber(&KUiStoreBox::btnModifyPassword_Clicked, this));

	Window* pBtnLockByDate = _thisWindow->getChild("TaharezLook/StoreBox/LockByDate");
	if ( pBtnLockByDate )
	{
		pBtnLockByDate->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiStoreBox::onClickLock, this));
	}
}

void KUiStoreBox::show()
{
	if(!_isLoad)
	{
		loadUi();
		_isLoad = true;
	}

	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->show();

	_curPage = 1;
	_pageTab[0]->setSelected(true);
	
	_saveMoney->enable();
	showPage();
	_moneyBox->hide();

	
	RefreshBtnModifyPasswordStates();

	KUiItemBox::getSingleton().show();
}

bool KUiStoreBox::isVisible()
{
	if(!_thisWindow)
	{
		return false;
	}

	if(!_isLoad)
	{
		return false;
	}
	return _thisWindow->isVisible();
}

void KUiStoreBox::hide()
{
	if(!_thisWindow)
	{
		return;
	}

	if(!_isLoad)
	{
		return;
	}
	_thisWindow->hide();
}

void KUiStoreBox::showPage()
{
	if(!_thisWindow)
	{
		return;
	}

	if(!_isLoad)
	{
		return;
	}

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
}

void KUiStoreBox::clearAGrid(TLGameObject* gridCtrl, bool disable)
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
	regionInfo->Region.Width = 0;//0表示挂位非物品
}

void KUiStoreBox::getData(void)
{
	if(!_thisWindow)
	{
		return;
	}

	if(!_isLoad)
	{
		return;
	}

	//得到储物箱金钱与物品
	KObjAtContRegion items[BAG_GRID_TOTAL_COUNT + 1];
	int count = g_pCoreShell->GetGameData(GDI_ITEM_IN_STORE_BOX, (unsigned int)items, BAG_GRID_TOTAL_COUNT + 1);
	
	if(!count)
	{
		_ASSERT(0);
	}
	//金钱
	if(count && items[0].Obj.uGenre == CGOG_MONEY)
	{
		int money = items->Obj.uId;
		int jin, yin, tong;
		sysMoneyToUiMoney(money, jin, yin, tong);
		_jin->setText(iToString(jin));
		_yin->setText(iToString(yin));
		_tong->setText(iToString(tong));
	}

	//显示当前包裹物品和金钱
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

	//物品
 	for(int index = 1; index < count; index++)
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

	//扩展背包栏
	for(int k = 0; k < EXTEND_HEIGHT; k++)
	{
		for(int l = 0; l < EXTEND_WIDTH; l++)
		{
			clearAGrid(_extendItem[k][l], false);
		}
	}

	KObjAtContRegion extend[BAG_EXTEND_GRID_TOTAL_COUNT];
	count = g_pCoreShell->GetGameData(GDI_STORE_EXTEND, (unsigned int)extend, BAG_EXTEND_GRID_TOTAL_COUNT);
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


bool KUiStoreBox::onBDown(const CEGUI::EventArgs& e)
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
	
	if(arg->button == LeftButton)
	{
		if ( KUiAdapter::GetMouseRes() == MOUSE_CURSOR_ITEM_LOCK_BY_DATA )
		{
			g_nLockItemByDateIdx = destPos.Obj.uId ;
			char itemName[COMMON_CLIENT_MSG_LEN_128];
			g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_WITH_COLOR_BY_INDEX, (UINT)itemName, g_nLockItemByDateIdx);
			KUiComMsgBox::GetSingleton().setModalStatus(true);
			KUiComMsgBox::GetSingleton().setComMsgPosition();
			KUiComMsgBox::Show();
			char yesString[COMMON_CLIENT_MSG_LEN_8];
			char noString[COMMON_CLIENT_MSG_LEN_8];
			strcpy(yesString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().yesString));
			strcpy(noString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().noString));
			KUiComMsgBox::GetSingleton().setBtnName((utf8*)yesString, (utf8*)noString);

			char deleteItemMsg[COMMON_CLIENT_MSG_LEN_256];


			if ( g_pCoreShell->GetGameData(GDI_IS_ITEM_LOCKED_BY_DATE, g_nLockItemByDateIdx, NULL) )
			{
				sprintf(deleteItemMsg, KMessageCentre::GetMessage(common_message, 2004), itemName);
			}
			else
			{
				sprintf(deleteItemMsg, KMessageCentre::GetMessage(common_message, 2003), itemName);
			}
			KUiComMsgBox::GetSingleton().setLayoutMsg(deleteItemMsg);
			
			KUiComMsgBox::GetSingleton().setFristBtnCallback(LockItemByDate);
			KUiAdapter::SetMouseRes(MOUSE_CURSOR_NORMAL);
			return true;
		}


		KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
		
		if(arg->sysKeys & Control && (KUiChatInputWnd::GetSingleton().isCurInput() || pRoom && pRoom->IsVisible()))
		{
			KUiItemHelper::sendItemLink(destPos);
		}
		else if(arg->sysKeys & Shift)
		{
			KUiSplitItemBox::getSingleton().open(destPos, destObjInfo);
		}
		else if(KUiPlayerState::getSingleton().getState() == KUiPlayerState::IDLE)
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
		if(KUiItemBox::getSingleton().isVisible())
		{
			moveToItemBox(destPos);
		}
		else
		{
			autoEquipExtendBag(destPos);
		}
	}
	
	return true;
}

bool KUiStoreBox::onLBUp(const EventArgs& e)
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

bool KUiStoreBox::onLBLeave(const EventArgs& e)
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

bool KUiStoreBox::onExtendPosBDown(const EventArgs& e)
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

bool KUiStoreBox::onExtendLBUp(const EventArgs& e)
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

bool KUiStoreBox::onExtendLBLeave(const EventArgs& e)
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


void KUiStoreBox::pickItem(KObjAtContRegion& destPos, TLGameObject::GameObject& destObjInfo, TLGameObject* destObj)
{
	if(!_thisWindow)
	{
		return;
	}

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

void KUiStoreBox::dropItem(KObjAtContRegion& destPos, TLGameObject::GameObject& destObjInfo, TLGameObject* destObj)
{
	if(!_thisWindow)
	{
		return;
	}

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

bool KUiStoreBox::autoEquipExtendBag(KObjAtContRegion& itemRegion)
{
	if(!_thisWindow)
	{
		return false;
	}

	if(false == _isLoad)
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

void KUiStoreBox::moveToItemBox(KObjAtContRegion& sourPos)
{
	if(!_thisWindow)
	{
		return;
	}

	if(false == _isLoad)
	{
		return;
	}

	KObjAtContRegion itemBoxRegion;
	itemBoxRegion.eContainer = UOC_ITEM_TAKE_WITH;
	int nRet = g_pCoreShell->OperationRequest(GOI_FIND_A_EMPTY_PLACE_OF_A_CONTAINER, (UINT)&itemBoxRegion, NULL);
	if(nRet <= 0)
	{
		char* message = KMessageCentre::GetMessage(storebox_error_message, BagFull);
		KUiChannelCentre::GetSingleton().toSysMsg(message);
		return;
	}
	
	nRet = g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (UINT)&sourPos, (int)&itemBoxRegion);
	if(nRet > 0)
	{
		KUiDragItem::GetSingleton().initItem();
	}
}

void KUiStoreBox::onItemChanged(KObjAtContRegion* pObj, int add)
{
	if(!_thisWindow)
	{
		return;
	}

	if(!_isLoad)
	{
		return;
	}

	TLGameObject::GameObject objInfo;
	if(pObj->Obj.uGenre == CGOG_MONEY)
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
		if(pObj->Obj.uGenre == CGOG_NOTHING)
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
		KObjAtContRegion* itemRegion = (KObjAtContRegion*)changedItem->getUserData();
		itemRegion->Obj.uGenre = CGOG_NOTHING;
		objInfo.d_gameobject = BACKGROUND_IMAGE;
		objInfo.d_type = TLGameObject::item;
		changedItem->setObject(objInfo);
	}
}

void KUiStoreBox::onItemExtendChanged(KObjAtContRegion* pObj, int add)
{
	if(!_thisWindow)
	{
		return;
	}

	if(!_isLoad)
	{
		return;
	}
	
	TLGameObject::GameObject objInfo;
	if(add)//如果是增加物品或更新物品
	{
		if(!pObj->Obj.uGenre)
			return;
		
		KObjAtContRegion* itemRegion = (KObjAtContRegion*)_extendItem[pObj->Region.v][pObj->Region.h]->getUserData();
		itemRegion->Obj = pObj->Obj;
		KUiItemHelper::setItemImage(pObj->Obj.uId, objInfo);
		objInfo.d_type = TLGameObject::item;
		objInfo.d_count = pObj->Region.Height;
		_extendItem[pObj->Region.v][pObj->Region.h]->setObject(objInfo);		
	}
	else//如果是减少物品
	{
		KObjAtContRegion* itemRegion = (KObjAtContRegion*)_extendItem[pObj->Region.v][pObj->Region.h]->getUserData();
		itemRegion->Obj.uGenre = CGOG_NOTHING;
		objInfo.d_gameobject = BACKGROUND_IMAGE;
		objInfo.d_type = TLGameObject::item;
		_extendItem[pObj->Region.v][pObj->Region.h]->setObject(objInfo);
	}
}

void KUiStoreBox::onBagSized(int newSize)
{
	_size = newSize;

	if(!_thisWindow)
	{
		return;
	}
	
	if(!_isLoad)
	{
		return;
	}

	//禁用没有格子的扩展栏
	int validPage = (_size - 1) / (BAG_HEIGHT * BAG_WIDTH) + 1;
	for(int i = 0; i < validPage; ++i)
	{
		_pageTab[i]->enable();
	}
	for(int j = validPage; j < PAGE_COUNT; ++j)
	{
		_pageTab[j]->disable();
	}
	showPage();
}

bool KUiStoreBox::onPageChanged(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);

	for(int i = 0; i < PAGE_COUNT; ++i)
	{
		if(_pageTab[i] != arg->window || _pageTab[i]->isSelected() == false)
			continue;

		_curPage = i + 1;
		showPage();
	}
	return true;
}

bool KUiStoreBox::onClickLock(const EventArgs& e)
{
	KUiAdapter::SetMouseRes(MOUSE_CURSOR_ITEM_LOCK_BY_DATA);
	return true;
}

bool KUiStoreBox::onClose(const CEGUI::EventArgs& e)
{
	_thisWindow->hide();
	return true;
}

bool KUiStoreBox::onMboxShow(const CEGUI::EventArgs& e)
{
	_moneyBox->setModalState(true);
	_mbox_jin->setText("");
	_mbox_yin->setText("");
	_mbox_tong->setText("");
	return true;
}

bool KUiStoreBox::onMboxHide(const CEGUI::EventArgs& e)
{
	_moneyBox->setModalState(false);
	return true;
}

bool KUiStoreBox::onMboxOk(const CEGUI::EventArgs& e)
{
	int jin		= atoi(_mbox_jin->getText().c_str());
	int yin		= atoi(_mbox_yin->getText().c_str());
	int tong	= atoi(_mbox_tong->getText().c_str());
	int money = jin * 10000 + yin * 100 + tong;
	if(money <= 0)
	{
		_moneyBox->hide();
		return false;
	}

	if(_isSaveMoney)
		g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX, true, money);
	else
		g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX, false, money);
	_moneyBox->hide();
	return false;
}

bool KUiStoreBox::onMboxCancel(const CEGUI::EventArgs& e)
{
	_moneyBox->hide();
	return false;
}

bool KUiStoreBox::onMboxSaveMoney(const CEGUI::EventArgs& e)
{
	_isSaveMoney = true;
	_moneyBox->show();
	_moneyBox->getChild("TaharezLook/StoreBox/Moneybox/quqiancunqian")->setText(_saveMoney->getText());
	return false;
}

bool KUiStoreBox::onMboxGetMoney(const CEGUI::EventArgs& e)
{
	_isSaveMoney = false;
	_moneyBox->show();
	_moneyBox->getChild("TaharezLook/StoreBox/Moneybox/quqiancunqian")->setText(_getMoney->getText());
	return false;
}

bool KUiStoreBox::onAddjustMoney(const CEGUI::EventArgs& e)
{
	WindowEventArgs* arg = (WindowEventArgs*)(&e);
	enum{
		Jin,
		Yin,
		Tong,
	}moneyType;

	if(_mbox_jin == arg->window)
	{
		moneyType = Jin;
	}
	else if(_mbox_yin == arg->window)
	{
		moneyType = Yin;
	}
	else if(_mbox_tong == arg->window)
	{
		moneyType = Tong;
	}
	else
	{
		return true;
	}
	String newCount = arg->window->getText();
	//判断是否数字，把非数字字符去掉
	for(int i = 0; i < newCount.length(); i++)
	{
		int num = newCount[i];
		if(num < 48 || num > 57)
		{
			newCount.erase(i, 1);
			i--;
			continue;
		}
	}

	//判断是否超过总数
	int maxMoney = 0;
	if(_isSaveMoney)//存钱，比较随身携带
	{
		maxMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	}
	else//取钱，比较存款
	{
		maxMoney = g_pCoreShell->GetGameData(GDI_PLAYER_STORE_MONEY, 0, 0);
	}
	int nCount = atoi(newCount.c_str());
	if(nCount <= 0)
	{
		((TLEditbox*)arg->window)->resetText(String(""));
		return true;
	}
	int jin = atoi(_mbox_jin->getText().c_str());
	int yin = atoi(_mbox_yin->getText().c_str());
	int tong = atoi(_mbox_tong->getText().c_str());
	switch(moneyType)
	{
	case Jin:
		if(nCount * 10000 + yin * 100 + tong > maxMoney)
			nCount = (maxMoney - yin * 100 - tong) / 10000;
		newCount = iToString(nCount);
		break;
	case Yin:
		if(nCount + yin * 100 + jin * 10000 > maxMoney)
			nCount = (maxMoney - jin * 10000 - tong) / 100;
		if(nCount > 99)
			nCount = 99;
		newCount = iToString(nCount);
		break;
	case Tong:
		if(nCount + yin * 100 + jin * 10000 > maxMoney)
			nCount = (maxMoney - jin * 10000 - yin * 100) % 100;
		if(nCount > 99)
			nCount = 99;
		newCount = iToString(nCount);
		break;
	}
	

	((TLEditbox*)arg->window)->resetText(newCount);
	return true;
}

bool KUiStoreBox::onWndHide(const CEGUI::EventArgs& e)
{
	_moneyBox->setModalState(false);
	return true;
}

pair<int, int> KUiStoreBox::uiPosToActPos(const pair<int, int>& uiPos)
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

pair<int, int> KUiStoreBox::actPosToUiPos(const pair<int, int>& actPos)
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

unsigned int KUiStoreBox::beginGroupCD(KItemGroupCD_C* groupCdInfo)
{
	if(!_thisWindow)
	{
		return 0;
	}

	if(!_isLoad)
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

unsigned int KUiStoreBox::endGroupCD(KItemGroupCD_C* pGroupCD)
{
	if(!_thisWindow)
	{
		return 0;
	}

	if(!_isLoad)
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

bool KUiStoreBox::btnModifyPassword_Clicked( const EventArgs& e )
{
	ShowPasswordDlg();
	return true;
}

void KUiStoreBox::RefreshBtnModifyPasswordStates()
{
	if ( NULL != m_btnModifyPassword )
	{
		if ( ! hasCreatedPassword() )
		{
			KUiIBShop::GetSingleton().getBtnModifyPassword()->setText( m_strCreatePassword );
			KUiCreditShop::GetSingleton().getBtnModifyPassword()->setText( m_strCreatePassword );
			m_btnModifyPassword->setText( m_strCreatePassword );
		}
		else
		{
			KUiIBShop::GetSingleton().getBtnModifyPassword()->setText( m_strModifyPassword );
			KUiCreditShop::GetSingleton().getBtnModifyPassword()->setText( m_strModifyPassword );
			m_btnModifyPassword->setText( m_strModifyPassword );
		}
	}
}

bool KUiStoreBox::hasCreatedPassword()
{
	return ( g_pCoreShell->GetGameData( GDI_GET_PASSWORD_STATE, NULL, NULL ) != 0 );
}

void KUiStoreBox::ShowPasswordDlg()
{
	if ( hasCreatedPassword() )
	{
		if ( ! KUiItemPassword_Modify::IsVisible() )
		{
			KUiItemPassword_Modify::Show();
		}
	}
	else
	{
		if ( ! KUiItemPassword_Create::IsVisible() )
		{
			KUiItemPassword_Create::Show();
		}
	}	
}