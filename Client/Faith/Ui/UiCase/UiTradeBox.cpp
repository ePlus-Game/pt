
#include "UiTradeBox.h"
#include "UiDragItem.h"
#include "../KMessageCentre.h"
#include "UiItemBox.h"
#include "UiChatWindow.h"
#include "UiErrorMessageBox.h"
#include "../UiConfigManager.h"

#include "chatWindow/ChatCharContainer.h"
#include "ChatWindow/chatWnd.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"

using namespace CEGUI;

extern iCoreShell*		g_pCoreShell;

template<> 
KUiTradeBox* KUiWndSingleton<KUiTradeBox>::ms_Singleton	= NULL;

KUiTradeBox::KUiTradeBox(const CEGUI::String& id_name):
KUiWndSingleton<KUiTradeBox>( id_name )
{
	for(int i = 0; i < TRADE_BOX_HEIGHT; i++)
	{
		for(int j = 0; j < TRADE_BOX_WIDTH; j++)
		{
			d_selfItem[i][j] = NULL;
		}
	}
	for(int k = 0; k < TRADE_BOX_HEIGHT; k++)
	{
		for(int l = 0; l < TRADE_BOX_WIDTH; l++)
		{			
			d_oppositeItem[k][l] = NULL;
		}
	}
	m_bIsItem = false;
	m_iItemMum = 0;
	m_bIsTrading = false;
}

KUiTradeBox::~KUiTradeBox()
{
	for(int i = 0; i < TRADE_BOX_HEIGHT; i++)
	{
		for(int j = 0; j < TRADE_BOX_WIDTH; j++)
		{
			if(d_selfItem[i][j] && d_selfItem[i][j]->getUserData())
			{
				delete d_selfItem[i][j]->getUserData();
			}
		}
	}
	for(int k = 0; k < TRADE_BOX_HEIGHT; k++)
	{
		for(int l = 0; l < TRADE_BOX_WIDTH; l++)
		{
			if(d_oppositeItem[k][l] && d_oppositeItem[k][l]->getUserData())
			{
				delete d_oppositeItem[k][l]->getUserData();
			}
		}
	}
}

void KUiTradeBox::Init()
{
	getChild();
	clear();
}

void KUiTradeBox::show(char* oppoNameText)
{
	m_iItemMum = 0;
	KUiWndSingleton<KUiTradeBox>::Show();
	clear();
	m_bIsTrading = true;
	//设置当前两个人物的名称
	TLStaticText* selfName = (TLStaticText*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/TradeBox/SelfName");
	KUiPlayerBaseInfo baseInfo;
	baseInfo.Name[0] = 0;
	g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&baseInfo, NULL );
	selfName->setText(AnsiToUtf8(baseInfo.Name));
	
	if(oppoNameText == NULL)
		return;

	TLStaticText* oppoName = (TLStaticText*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/TradeBox/OppositeName");
	oppoName->setText(AnsiToUtf8(oppoNameText));
	d_lockImage->setVisible(false);

	//同时打开物品栏
	KUiItemBox::getSingleton().show();
	
	char message[COMMON_CLIENT_MSG_LEN_128];
	sprintf(message, KUiCfgLoader::getSingleton().getTradeCfg().beginTradeMsg, oppoNameText);
	printSystemMessage(message);

	if (m_bIsItem)
	{
		m_bIsItem = false;
		TLGameObject* destObj = d_selfItem[0][0];
		TLGameObject::GameObject destObjInfo = destObj->getObject();
		KObjAtContRegion* destRegion = (KObjAtContRegion*)destObj->getUserData();
		
		TLGameObject* sourObj = KUiDragItem::GetSingleton().getObj();
		TLGameObject::GameObject sourObjInfo = sourObj->getObject();
		KObjAtContRegion* sourRegion = (KObjAtContRegion*)sourObj->getUserData();
		
		if(sourObjInfo.d_type == TLGameObject::idle)
		{
			if(destObjInfo.d_type == TLGameObject::item)
			{
				sourObj->setObject(destObjInfo);
				*sourRegion = *destRegion;
				sourObj->setCanDrag(true);
			}
		}
		else
		{
			int itemIndex = sourRegion->Obj.uId;
			int canTrade = g_pCoreShell->GetGameData( GDI_ITEM_CAN_TRADE, NULL, itemIndex);
			if(!canTrade)
			{
				KUiChannelCentre::GetSingleton().toSysMsg(KUiCfgLoader::getSingleton().getTradeCfg().tradeForbiddenMsg);
			}
			else
			{
				int nRet = g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (unsigned int)sourRegion, (int)destRegion);
				KUiDragItem::GetSingleton().initItem();
			}
		}
	}
}

void KUiTradeBox::getChild()
{
	if(NULL == m_pThisWnd)
	{
		return;
	}

	String parentWndName = "TaharezLook/TradeBox";
	TLGameObject::GameObject itemObj;
	itemObj.d_gameobject = BACKGROUND_IMAGE;
	itemObj.d_type = TLGameObject::idle;

	//自己的物品栏
	for(int i = 0; i < TRADE_BOX_HEIGHT; i++)
	{
		for(int j = 0; j < TRADE_BOX_WIDTH; j++)
		{
			char itemNum[COMMON_CLIENT_MSG_LEN_64];
			sprintf(itemNum, "%d-%d", i + 1, j + 1);
			d_selfItem[i][j] = (TLGameObject*)m_pThisWnd->getChild(parentWndName + "/SelfItem" + itemNum);
			
			TLGameObject* thisObj = d_selfItem[i][j];
			d_selfItemGrid[i][j].setCtrl(thisObj);
			d_selfItemGrid[i][j].addTip();
			
			//设置显示图片
			thisObj->setObject(itemObj);
			
			//设置位置信息
			KObjAtContRegion* oldRegion = (KObjAtContRegion*)thisObj->getUserData();
			if(oldRegion != NULL)
			{
				delete oldRegion;
			}
			KObjAtContRegion* itemRegion = new KObjAtContRegion();
			itemRegion->eContainer = UOC_TO_BE_TRADE;
			itemRegion->Region.h = j;
			itemRegion->Region.v = i;
			itemRegion->Region.Width = 0;//0表示物品非挂位
			itemRegion->Region.Height = 0;
			itemRegion->Obj.uGenre = CGOG_NOTHING;
			itemRegion->Obj.uId = 0;
			thisObj->setUserData(itemRegion);
			thisObj->subscribeEvent(TLGameObject::EventMouseButtonUp, Event::Subscriber(&KUiTradeBox::onLBUp, this));
		}
	}

	//对方的物品
	for(int k = 0; k < TRADE_BOX_HEIGHT; k++)
	{
		for(int l = 0; l < TRADE_BOX_WIDTH; l++)
		{
			char itemNum[COMMON_CLIENT_MSG_LEN_64];
			sprintf(itemNum, "%d-%d", k + 1, l + 1);
			d_oppositeItem[k][l] = (TLGameObject*)m_pThisWnd->getChild(parentWndName + "/OppositeItem" + itemNum);
			
			TLGameObject* thisObj = d_oppositeItem[k][l];
			d_oppositeItemGrid[k][l].setCtrl(thisObj);
			d_oppositeItemGrid[k][l].addTip();
			
			//设置显示图片
			thisObj->setObject(itemObj);
			
			//设置位置信息
			KObjAtContRegion* oldRegion = (KObjAtContRegion*)thisObj->getUserData();
			if(oldRegion != NULL)
			{
				delete oldRegion;
			}
			KObjAtContRegion* itemRegion = new KObjAtContRegion();
			itemRegion->eContainer = UOC_OTHER_TO_BE_TRADE;
			itemRegion->Region.h = l;
			itemRegion->Region.v = k;
			itemRegion->Region.Width = 0;//0表示物品非挂位
			itemRegion->Region.Height = 0;
			itemRegion->Obj.uGenre = CGOG_NOTHING;
			itemRegion->Obj.uId = 0;
			thisObj->setUserData(itemRegion);	
		}
	}

	d_selfJin		= (TLEditbox*)m_pThisWnd->getChild(parentWndName + "/SelfJin");
	d_selfYin		= (TLEditbox*)m_pThisWnd->getChild(parentWndName + "/SelfYin");
	d_selfTong		= (TLEditbox*)m_pThisWnd->getChild(parentWndName + "/SelfTong");

	d_selfJin->subscribeEvent(Editbox::EventTextChanged, Event::Subscriber(&KUiTradeBox::onSelfMoneyChange, this));
	d_selfYin->subscribeEvent(Editbox::EventTextChanged, Event::Subscriber(&KUiTradeBox::onSelfMoneyChange, this));
	d_selfTong->subscribeEvent(Editbox::EventTextChanged, Event::Subscriber(&KUiTradeBox::onSelfMoneyChange, this));

	d_selfJin->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiTradeBox::onKeyDown, this));
	d_selfYin->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiTradeBox::onKeyDown, this));
	d_selfTong->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiTradeBox::onKeyDown, this));

	d_selfLock		= (TLButton*)m_pThisWnd->getChild(parentWndName + "/Lock");
	d_selfTrade		= (TLButton*)m_pThisWnd->getChild(parentWndName + "/Trade");
	d_selfCancel	= (TLButton*)m_pThisWnd->getChild(parentWndName + "/Cancel");

	d_selfLock->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiTradeBox::onLock, this));
	d_selfTrade->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiTradeBox::onTrade, this));
	d_selfCancel->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiTradeBox::onCancel, this));

	//对方交易栏控件
	d_oppositeJin	= (TLEditbox*)m_pThisWnd->getChild(parentWndName + "/OppositeJin");
	d_oppositeYin	= (TLEditbox*)m_pThisWnd->getChild(parentWndName + "/OppositeYin");
	d_oppositeTong	= (TLEditbox*)m_pThisWnd->getChild(parentWndName + "/OppositeTong");

	d_oppositeLock	= (TLButton*)m_pThisWnd->getChild(parentWndName + "/OppositeLock");
	d_oppositeTrade	= (TLButton*)m_pThisWnd->getChild(parentWndName + "/OppositeTrade");
	d_oppositeCancel= (TLButton*)m_pThisWnd->getChild(parentWndName + "/OppositeCancel");

	//交易信息
	d_tradeInfo		= (TLEditbox*)m_pThisWnd->getChild(parentWndName + "/TradeInfo");

	//关闭按钮
	d_closeBtn		= (TLButton*)m_pThisWnd->getChild(parentWndName + "/Close");
	d_closeBtn->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiTradeBox::onCancel, this));

	//交易窗口
	m_pThisWnd->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiTradeBox::onHide, this));

	d_lockImage = m_pThisWnd->getChild("TaharezLook/TradeBox/TradeLocked");
}

void KUiTradeBox::clear()
{
	TLGameObject::GameObject itemObj;
	itemObj.d_gameobject = BACKGROUND_IMAGE;
	itemObj.d_type = TLGameObject::idle;

	//自己的物品栏
	for(int i = 0; i < TRADE_BOX_HEIGHT; i++)
	{
		for(int j = 0; j < TRADE_BOX_WIDTH; j++)
		{
			TLGameObject* thisObj = d_selfItem[i][j];
			if(NULL == thisObj)
			{
				continue;
			}

			thisObj->setObject(itemObj);
						
			KObjAtContRegion* itemRegion = (KObjAtContRegion*)thisObj->getUserData();
			itemRegion->Region.Width = 0;//0表示物品非挂位
			itemRegion->Region.Height = 0;
			itemRegion->Obj.uGenre = CGOG_NOTHING;
			itemRegion->Obj.uId = 0;	
		}
	}

	//对方的物品
	for(int k = 0; k < TRADE_BOX_HEIGHT; k++)
	{
		for(int l = 0; l < TRADE_BOX_WIDTH; l++)
		{
			TLGameObject* thisObj = d_oppositeItem[k][l];
			if(NULL == thisObj)
			{
				continue;
			}

			thisObj->setObject(itemObj);
						
			KObjAtContRegion* itemRegion = (KObjAtContRegion*)thisObj->getUserData();
			itemRegion->Region.Width = 0;//0表示物品非挂位
			itemRegion->Region.Height = 0;
			itemRegion->Obj.uGenre = CGOG_NOTHING;
			itemRegion->Obj.uId = 0;	
		}
	}
	
	d_selfJin->resetText(String("0"));
	d_selfYin->resetText(String("0"));
	d_selfTong->resetText(String("0"));

	d_selfJin->enable();
	d_selfYin->enable();
	d_selfTong->enable();

	d_selfLock->enable();
	d_selfTrade->disable();
	d_selfCancel->enable();
	
	d_oppositeJin->setText("0");
	d_oppositeYin->setText("0");
	d_oppositeTong->setText("0");

	d_oppositeJin->disable();
	d_oppositeYin->disable();
	d_oppositeTong->disable();

	d_oppositeLock->enable();
	d_oppositeTrade->disable();
	d_oppositeCancel->disable();

	char* message = KMessageCentre::GetMessage(trade_box_message, ui_trade_both_unlock);
	d_tradeInfo->setText(AnsiToUtf8(message));
}

void KUiTradeBox::setItemImage(int itemId, TLGameObject::GameObject& obj)
{
	KItemInfo tagItemInfo;
	g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&tagItemInfo, itemId );
	obj.d_gameobjectSet = tagItemInfo.szImageSet;
	obj.d_gameobject = tagItemInfo.szImage;
	obj.d_EdgeframeIdx = tagItemInfo.colour;
}

bool KUiTradeBox::onLBUp(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	if(arg->button != LeftButton)
		return true;

	//判断是否可以继续放物品上去
	if(d_selfLock->isDisabled())
		return true;

	TLGameObject* destObj = (TLGameObject*)arg->window;
	TLGameObject::GameObject destObjInfo = destObj->getObject();
	KObjAtContRegion* destRegion = (KObjAtContRegion*)destObj->getUserData();

	TLGameObject* sourObj = KUiDragItem::GetSingleton().getObj();
	TLGameObject::GameObject sourObjInfo = sourObj->getObject();
	KObjAtContRegion* sourRegion = (KObjAtContRegion*)sourObj->getUserData();

	if(sourObjInfo.d_type == TLGameObject::idle)
	{
		if(destObjInfo.d_type == TLGameObject::item)
		{
			sourObj->setObject(destObjInfo);
			*sourRegion = *destRegion;
			sourObj->setCanDrag(true);
		}
	}
	else//手上拿着的东西为item或者其他东西
	{
		int itemIndex = sourRegion->Obj.uId;
		int canTrade = g_pCoreShell->GetGameData( GDI_ITEM_CAN_TRADE, NULL, itemIndex);
		if(!canTrade)
		{
			KUiChannelCentre::GetSingleton().toSysMsg(KUiCfgLoader::getSingleton().getTradeCfg().tradeForbiddenMsg);
		}
		else
		{
			int nRet = g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (unsigned int)sourRegion, (int)destRegion);
			KUiDragItem::GetSingleton().initItem();
		}
	}
	return true;
}

bool KUiTradeBox::onSelfMoneyChange(const CEGUI::EventArgs& e)
{
	WindowEventArgs* window = (WindowEventArgs*)&e;
	
	String jinString = d_selfJin->getText();
	for(int i = 0; i < jinString.length(); i++)
	{
		int num = jinString[i];
		if(num < 48 || num > 57)
		{
			jinString.erase(i, 1);
			i--;
			continue;
		}
	}
	int jin = atoi(jinString.c_str());
	
	String yinString = d_selfYin->getText();
	for(int j = 0; j < yinString.length(); j++)
	{
		int num = yinString[j];
		if(num < 48 || num > 57)
		{
			yinString.erase(j, 1);
			j--;
			continue;
		}
	}
	int yin = atoi(yinString.c_str());
	
	String tongString = d_selfTong->getText();
	for(int k = 0; k < tongString.length(); k++)
	{
		int num = tongString[k];
		if(num < 48 || num > 57)
		{
			tongString.erase(k, 1);
			k--;
			continue;
		}
	}
	int tong = atoi(tongString.c_str());
	
	if(yin > 99)
		yin = 99;
	if(yin < 0)
		yin = 0;
	if(tong > 99)
		tong = 99;
	if(tong < 0)
		tong = 0;
	if(jin > 9999)
	{
		jin = 9999;
	}
	
	int tradeMoney = uiMoneyToSysMoney(jin, yin, tong);
	int maxMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	
	if(tradeMoney > maxMoney || tradeMoney < 0)
	{
		tradeMoney = maxMoney;
	}
	
	sysMoneyToUiMoney(tradeMoney, jin, yin, tong);
	if(jinString.length() != 0)
		d_selfJin->resetText(iToString(jin));
	if(yinString.length() != 0)
		d_selfYin->resetText(iToString(yin));
	if(tongString.length() != 0)
		d_selfTong->resetText(iToString(tong));
	g_pCoreShell->OperationRequest(GOI_TRADE_CHANGE_MONEY, tradeMoney, 0);

	d_oppositeTrade->disable();
	d_oppositeLock->enable();
	return true;
}

void KUiTradeBox::onSelfItemChanged(KObjAtContRegion* changedObj, int add)
{
	if(NULL == m_pThisWnd || m_pThisWnd->isVisible() == false)
	{
		return;
	}

	TLGameObject::GameObject objInfo;
	if(changedObj->Obj.uGenre == CGOG_MONEY)//如果改变的是自己放在交易面版上的钱
	{
		int money = changedObj->Obj.uId;
		int jin = 0;
		int yin = 0;
		int tong = 0;
		sysMoneyToUiMoney(money, jin, yin, tong);
		d_selfJin->resetText(iToString(jin));
		d_selfYin->resetText(iToString(yin));
		d_selfTong->resetText(iToString(tong));
	}
	else if(add)//如果是增加物品
	{
		if(CGOG_ITEM != changedObj->Obj.uGenre)
			return;
		
		KObjAtContRegion* itemRegion = (KObjAtContRegion*)d_selfItem[changedObj->Region.v][changedObj->Region.h]->getUserData();
		itemRegion->Obj = changedObj->Obj;
		itemRegion->Region.Height = changedObj->Region.Height;

		setItemImage(changedObj->Obj.uId, objInfo);
		objInfo.d_type = TLGameObject::item;
		objInfo.d_count = changedObj->Region.Height;
		d_selfItem[changedObj->Region.v][changedObj->Region.h]->setObject(objInfo);
		m_iItemMum++;
	}
	else//如果是减少物品
	{
		KObjAtContRegion* itemRegion = (KObjAtContRegion*)d_selfItem[changedObj->Region.v][changedObj->Region.h]->getUserData();
		itemRegion->Obj.uGenre = CGOG_NOTHING;
		itemRegion->Obj.uId = 0;
		itemRegion->Region.Height = 0;

		objInfo.d_gameobject = BACKGROUND_IMAGE;
		objInfo.d_type = TLGameObject::idle;
		d_selfItem[changedObj->Region.v][changedObj->Region.h]->setObject(objInfo);
		m_iItemMum--;
	}
	d_oppositeLock->enable();
	d_oppositeTrade->disable();
}

void KUiTradeBox::onOppositeMoneyChanged(int money)
{
	int jin = 0;
	int yin = 0;
	int tong = 0;
	sysMoneyToUiMoney(money, jin, yin, tong);
	d_oppositeJin->setText(iToString(jin));
	d_oppositeYin->setText(iToString(yin));
	d_oppositeTong->setText(iToString(tong));
}

void KUiTradeBox::onOppositeItemChanged(KObjAtContRegion *pObj, int bAdd)
{
	if (pObj->Obj.uGenre == CGOG_MONEY)
	{
		int money = pObj->Obj.uId;
		int jin = 0;
		int yin = 0;
		int tong = 0;
		sysMoneyToUiMoney(money, jin, yin, tong);
		d_selfJin->setText(iToString(jin));
		d_selfYin->setText(iToString(yin));
		d_selfTong->setText(iToString(tong));
	}
	else
	{	
		if (bAdd)
		{	
			KObjAtContRegion* itemRegion = (KObjAtContRegion*)d_oppositeItem[pObj->Region.v][pObj->Region.h]->getUserData();
			itemRegion->Obj = pObj->Obj;
			itemRegion->Region.Height = pObj->Region.Height;

			TLGameObject::GameObject itemInfo;
			setItemImage(pObj->Obj.uId, itemInfo);
			itemInfo.d_type = TLGameObject::item;
			itemInfo.d_count = pObj->Region.Height;
			d_oppositeItem[pObj->Region.v][pObj->Region.h]->setObject(itemInfo);
		}
		else
		{
			KObjAtContRegion* itemRegion = (KObjAtContRegion*)d_oppositeItem[pObj->Region.v][pObj->Region.h]->getUserData();
			itemRegion->Obj.uGenre = CGOG_NOTHING;
			itemRegion->Obj.uId = 0;
			itemRegion->Region.Height = 0;

			TLGameObject::GameObject itemInfo;
			itemInfo.d_gameobject = BACKGROUND_IMAGE;
			itemInfo.d_type = TLGameObject::idle;
			d_oppositeItem[pObj->Region.v][pObj->Region.h]->setObject(itemInfo);
		}
	}
}

bool KUiTradeBox::onLock(const CEGUI::EventArgs& e)
{
	g_pCoreShell->OperationRequest(GOI_TRADE_LOCK, 0, 0);
	return true;
}

bool KUiTradeBox::onTrade(const CEGUI::EventArgs& e)
{
	g_pCoreShell->OperationRequest(GOI_TRADE, 0, 0);
	return true;
}

bool KUiTradeBox::onHide(const CEGUI::EventArgs& e)
{
	cancelTrade();
	return true;
}

bool KUiTradeBox::onCancel(const CEGUI::EventArgs& e)
{
	Hide();
	return true;
}

void KUiTradeBox::cancelTrade()
{
	g_pCoreShell->OperationRequest(GOI_TRADE_CANCEL, 0, 0);
	// 锁定交易图标
	d_lockImage->setVisible(false);
	m_bIsTrading = false;
//	printSystemMessage(KUiCfgLoader::getSingleton().getTradeCfg().cancelTradeMsg);
}

void KUiTradeBox::lock(bool self)
{
	if(self)
	{
		d_selfLock->disable();
		d_selfJin->disable();
		d_selfYin->disable();
		d_selfTong->disable();
		if(d_oppositeLock->isDisabled() == true)
		{
			d_selfTrade->enable();
			d_oppositeTrade->enable();
			char* message = KMessageCentre::GetMessage(trade_box_message, ui_trade_both_lock);
			d_tradeInfo->setText(AnsiToUtf8(message));
			// 锁定交易图标
			d_lockImage->setVisible(true);
			printSystemMessage(message);
		}
		else
		{
			char* message = KMessageCentre::GetMessage(trade_box_message, ui_trade_you_lock_opposite_unlock);
			d_tradeInfo->setText(AnsiToUtf8(message));
			printSystemMessage(message);
		}
	}
	else
	{
		d_oppositeLock->disable();
		if(true == d_selfLock->isDisabled())
		{
			char* message = KMessageCentre::GetMessage(trade_box_message, ui_trade_both_lock);
			d_tradeInfo->setText(AnsiToUtf8(message));
			printSystemMessage(message);

			d_oppositeTrade->enable();
			d_selfTrade->enable();
			// 锁定交易图标
			d_lockImage->setVisible(true);
		}
		else
		{
			char* message = KMessageCentre::GetMessage(trade_box_message, ui_trade_opposite_lock);
			d_tradeInfo->setText(AnsiToUtf8(message));
			printSystemMessage(message);
		}
	}
}

void KUiTradeBox::unlock()
{
	char* message = KMessageCentre::GetMessage(trade_box_message, ui_trade_both_unlock);
	d_tradeInfo->setText(AnsiToUtf8(message));
//	printSystemMessage(message);

	d_selfLock->enable();
	d_selfTrade->disable();

	d_oppositeLock->enable();
	d_oppositeTrade->disable();
	
	d_selfJin->enable();
	d_selfYin->enable();
	d_selfTong->enable();
				
	// 锁定交易图标
	d_lockImage->setVisible(false);
}

void KUiTradeBox::endTrade(bool self)
{
	if(self)
	{
		char* message = KMessageCentre::GetMessage(trade_box_message, ui_trade_you_end_trade);
		d_tradeInfo->setText(AnsiToUtf8(message));
		printSystemMessage(message);

		d_selfTrade->disable();
	}
	else
	{
		char* message = KMessageCentre::GetMessage(trade_box_message, ui_trade_opposite_end_trade);
		d_tradeInfo->setText(AnsiToUtf8(message));
		printSystemMessage(message);

		d_oppositeTrade->disable();
	}
	m_bIsTrading = false;
}

bool KUiTradeBox::onKeyDown( const CEGUI::EventArgs& e )
{
    switch (static_cast<const KeyEventArgs&>(e).scancode)
    {
	case Key::F1:
	case Key::F2:
	case Key::F3:	
	case Key::F4:
	case Key::F5:
	case Key::F6:	
	case Key::F7:
	case Key::F8:
	case Key::F9:	
	case Key::F10:
	case Key::F11:
	case Key::F12:
		return false;
    default:
        return true;
    }
}

void KUiTradeBox::printSystemMessage(const char* msg)
{
	char layoutText[COMMON_CLIENT_MSG_LEN_1024];
	sprintf(layoutText, "<Seg float=wrap><Obj type=text c=cfcf>%s</Obj></Seg>", msg);
	KUiChannelCentre::GetSingleton().recvCustomMessage(SYSTEM_ROOM_ID, layoutText);
	B2ChatDialog::chatManager.ChatClientInsertSystemMsg(msg);
}
	
void KUiTradeBox::printOppoPickDropItem(bool pickdrop, int itemIndex)
{
	char itemName[COMMON_CLIENT_MSG_LEN_64];
	g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_BY_INDEX, (unsigned int)itemName, itemIndex );

	ItemType type;
	g_pCoreShell->GetGameData(GDI_GET_ITEM_TYPE_BY_INDEX, (unsigned int)&type, itemIndex );

	int itemId;
	itemId = g_pCoreShell->GetGameData(GDI_GET_ITEM_ID_BY_INDEX, itemIndex, 0);

	int itemHashId = GenerateItemHashId(type.genre, type.detail, type.particular);

	char layoutText[COMMON_CLIENT_MSG_LEN_1024];
	char tempText[COMMON_CLIENT_MSG_LEN_64];
	memset(layoutText, 0, COMMON_CLIENT_MSG_LEN_1024);
	memset(tempText, 0, COMMON_CLIENT_MSG_LEN_64);

	strcpy(layoutText, "<Seg float=wrap>");
	if(pickdrop)
	{
		strcat(layoutText, "<Obj color=255,0,0>");
		strcat(layoutText, KUiCfgLoader::getSingleton().getTradeCfg().oppositePickMsg);
	}
	else
	{
		strcat(layoutText, "<Obj color=0,255,255>");
		strcat(layoutText, KUiCfgLoader::getSingleton().getTradeCfg().oppositeDropMsg);
	}
	strcat(layoutText, "</Obj>");
	strcat(layoutText, "<Obj show-des=true gotype=item des=[");
	strcat(layoutText, itemName);
	strcat(layoutText, "] goid=");
	sprintf(tempText, "%d", itemHashId);
	strcat(layoutText, tempText);
	strcat(layoutText, " goid1=");
	sprintf(tempText, "%d", type.level);
	strcat(layoutText, tempText);
	strcat(layoutText, " goid2=");
	sprintf(tempText, "%d", itemId);
	strcat(layoutText, tempText);
	
	int color = g_pCoreShell->GetGameData( GDI_GET_ITEM_QUALITY_BY_TYPE, (unsigned int)&type, NULL );
	
	strcat(layoutText, " color=");
	strcat(layoutText, KUiChanMgr::getSinglton().getItemColor(color));
	strcat(layoutText, " font-family=");
	strcat(layoutText, KUiChanMgr::getSinglton().getItemFont());
	strcat(layoutText, ">");
	strcat(layoutText, itemName);
	strcat(layoutText, "</Obj></Seg>");

	KUiChannelCentre::GetSingleton().recvCustomMessage(SYSTEM_ROOM_ID, layoutText);
}

void KUiTradeBox::SetIsItem(bool isItem)
{
	m_bIsItem = isItem;
}

bool KUiTradeBox::isVisible()
{
	return KUiWndSingleton<KUiTradeBox>::IsVisible();
}

void KUiTradeBox::AddItemByClickPlayer()
{
	if(d_selfLock->isDisabled())
		return;
	
	if (m_iItemMum >= 10)
	{
		return;
	}

	TLGameObject* destObj = NULL;
	TLGameObject::GameObject destObjInfo;
	for (int i = 0; i < 10; i++)
	{
		destObj = (TLGameObject*)d_selfItem[i / 5][i % 5];
		destObjInfo = destObj->getObject();
		if (destObjInfo.d_type == TLGameObject::idle)
		{
			break;
		}
	}

	KObjAtContRegion* destRegion = (KObjAtContRegion*)destObj->getUserData();
	
	TLGameObject* sourObj = KUiDragItem::GetSingleton().getObj();
	TLGameObject::GameObject sourObjInfo = sourObj->getObject();
	KObjAtContRegion* sourRegion = (KObjAtContRegion*)sourObj->getUserData();
	
	if(sourObjInfo.d_type == TLGameObject::idle)
	{
		if(destObjInfo.d_type == TLGameObject::item)
		{
			sourObj->setObject(destObjInfo);
			*sourRegion = *destRegion;
			sourObj->setCanDrag(true);
		}
	}
	else//手上拿着的东西为item或者其他东西
	{
		int itemIndex = sourRegion->Obj.uId;
		int canTrade = g_pCoreShell->GetGameData( GDI_ITEM_CAN_TRADE, NULL, itemIndex);
		if(!canTrade)
		{
			KUiChannelCentre::GetSingleton().toSysMsg(KUiCfgLoader::getSingleton().getTradeCfg().tradeForbiddenMsg);
		}
		else
		{
			int nRet = g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (unsigned int)sourRegion, (int)destRegion);
			KUiDragItem::GetSingleton().initItem();
		}
	}
}

bool KUiTradeBox::IsTrading()
{
	return m_bIsTrading;
}