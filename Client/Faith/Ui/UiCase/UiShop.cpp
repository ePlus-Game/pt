
#include "UiShop.h"
#include "UiPlayerState.h"
#include "UiItemBox.h"
#include "UiTradeConfirmBox.h"
#include "UiEquipment.h"
#include "../UiAdapter.h"
#include "../UiConfigManager.h"
#include "Ui/KMessageCentre.h"

using namespace CEGUI;

extern iCoreShell*		g_pCoreShell;

template<> 
KUiShop* KUiWndSingleton<KUiShop>::ms_Singleton	= NULL;

KUiShop::KUiShop(const CEGUI::String& id_name):
KUiWndSingleton<KUiShop>( id_name )
{
	d_curPage = 0;
	d_pageCount = 0;
	d_items = NULL;
}

KUiShop::~KUiShop()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		//自己的物品栏
		delete[] d_items;
	}

}

void KUiShop::getChild()
{
	String parentWndName = "TaharezLook/Shop";
	TLGameObject::GameObject itemObj;
	itemObj.d_gameobject = getEmptySpaceName();
	itemObj.d_type = TLGameObject::idle;

	//自己的物品栏
	for(int i = 0; i < SHOP_ITEM_PER_PAGE; i++)
	{	
		String itemNum = iToString(i + 1);
		d_itemIcon[i] = (TLGameObject*)m_pThisWnd->getChild(parentWndName + "/Item" + itemNum);
		//设置格子通y用属性
		d_itemIconGrid[i].setCtrl(d_itemIcon[i]);
		d_itemIconGrid[i].addTip();
		
		d_description[i] = (TLStaticText*)m_pThisWnd->getChild(parentWndName + "/Description" + itemNum);
//		d_description[i]->setFont("SongTi-10");
		
		//设置显示图片
		d_itemIcon[i]->setObject(itemObj);
		
		//设置位置信息
		d_itemIcon[i]->setUserData(NULL);

		d_itemIcon[i]->subscribeEvent(TLGameObject::EventMouseClick, Event::Subscriber(&KUiShop::onDirectBuy, this));
	}

	d_normalRepair		= (TLButton*)m_pThisWnd->getChild(parentWndName + "/NormalRepair");
	d_specialRepair		= (TLButton*)m_pThisWnd->getChild(parentWndName + "/SpecialRepair");
	d_normalRepairAll	= (TLButton*)m_pThisWnd->getChild(parentWndName + "/NormalRepairAll");
	d_specialRepairAll	= (TLButton*)m_pThisWnd->getChild(parentWndName + "/SpecialRepairAll");
	d_previous			= (TLButton*)m_pThisWnd->getChild(parentWndName + "/Previous");
	d_next				= (TLButton*)m_pThisWnd->getChild(parentWndName + "/Next");
	d_close				= (TLButton*)m_pThisWnd->getChild(parentWndName + "/Close");
	d_Tittle			= (TLStaticText*)m_pThisWnd->getChild("TaharezLook/Shop/ShopUI");
	if ( d_Tittle )
	{
		d_defaultTittle = d_Tittle->getText();		
	}
	d_plusPointTxt	    = (TLStaticText*)m_pThisWnd->getChild("TaharezLook/Shop/PlusMoney");
	d_normalRepair->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiShop::onNormalRepair, this));
	d_specialRepair->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiShop::onSpecialRepair, this));
	d_normalRepairAll->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiShop::onNormalRepairAll, this));
	d_specialRepairAll->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiShop::onSpecialRepairAll, this));

	d_previous->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiShop::onPrevious, this));
	d_next->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiShop::onNext, this));
	d_previous->subscribeEvent(PushButton::EventMouseDoubleClick, Event::Subscriber(&KUiShop::onPrevious, this));
	d_next->subscribeEvent(PushButton::EventMouseDoubleClick, Event::Subscriber(&KUiShop::onNext, this));

	d_close->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiShop::onClose, this));
	m_pThisWnd->subscribeEvent(TLStaticImage::EventHidden, Event::Subscriber(&KUiShop::onWndHide, this));
}

bool KUiShop::IsInsteadSpecieShop(int curShopIndex)
{
	int insteadSpecieIndex = KUiItemBox::getSingleton().GetInsteadSpecieIndex();
	return (curShopIndex == insteadSpecieIndex);
}

bool KUiShop::onDirectBuy(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	TLGameObject* clickedObj = (TLGameObject*)arg->window;
	
	KObjAtContRegion* pos = (KObjAtContRegion*)clickedObj->getUserData();
	if(NULL == pos || pos->Obj.uId >= d_itemCount)
	{
		return true;
	}

	//先清空状态
	KUiPlayerState::getSingleton().setState(KUiPlayerState::TRADE_NPC_BUY_SALE);
	KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );

	if(arg->button == RightButton)
	{
		//右键直接买
		g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY, (unsigned int)pos->Obj.uId, 1);
	}
	else
	{
		//左键弹出确认框
		KUiTradeConfirmBox::GetSingleton().show(pos, d_shopIdx, d_plusPointInfo);
	}

	return true;
}

bool KUiShop::onNormalRepair(const CEGUI::EventArgs& e)
{
	if(KUiPlayerState::getSingleton().getState() == KUiPlayerState::TRADE_NPC_BUY_SALE
		|| KUiPlayerState::getSingleton().getState() == KUiPlayerState::TRADE_NPC_SPECIAL_REPAIR)
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_REPAIR );
		KUiPlayerState::getSingleton().setState(KUiPlayerState::TRADE_NPC_NORMAL_REPAIR);
	}
	else if(KUiPlayerState::getSingleton().getState() == KUiPlayerState::TRADE_NPC_NORMAL_REPAIR)
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
		KUiPlayerState::getSingleton().setState(KUiPlayerState::TRADE_NPC_BUY_SALE);
	}
	return true;
}

bool KUiShop::onSpecialRepair(const CEGUI::EventArgs& e)
{
	if(KUiPlayerState::getSingleton().getState() == KUiPlayerState::TRADE_NPC_BUY_SALE
		|| KUiPlayerState::getSingleton().getState() == KUiPlayerState::TRADE_NPC_NORMAL_REPAIR)
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_REPAIR_PLUS );
		KUiPlayerState::getSingleton().setState(KUiPlayerState::TRADE_NPC_SPECIAL_REPAIR);
	}
	else if(KUiPlayerState::getSingleton().getState() == KUiPlayerState::TRADE_NPC_SPECIAL_REPAIR)
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
		KUiPlayerState::getSingleton().setState(KUiPlayerState::TRADE_NPC_BUY_SALE);
	}
	return true;
}

bool KUiShop::onNormalRepairAll(const CEGUI::EventArgs& e)
{
	if(d_canRepairLevel == 0)
		return true;

	g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR_ALL, NULL, false);
	
	d_specialRepairAll->disable();
	d_normalRepairAll->disable();
	return true;
}

bool KUiShop::onSpecialRepairAll(const CEGUI::EventArgs& e)
{
	if(d_canRepairLevel <= 1)
	{
		char * errorMessage = KMessageCentre::GetMessage(not_enough_money_for_repair, 0);
		if (errorMessage != NULL && errorMessage[0] != 0 && g_pCoreShell != NULL)
		{
			g_pCoreShell->OperationRequest(GOI_AUTO_REPAIR_ERROR_MSG, (unsigned int)errorMessage, 0);
		}
		return true;
	}

	g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR_ALL, NULL, true);

	d_specialRepairAll->disable();
	d_normalRepairAll->disable();
	return true;
}


bool KUiShop::onPrevious(const CEGUI::EventArgs& e)
{
	d_curPage--;
	if (d_plusPointInfo.name != "" )
	{
		showCurPageByPlusPoint();
	}
	else
	{
		showCurPage();
	}
	
	return true;
}

bool KUiShop::onNext(const CEGUI::EventArgs& e)
{
	d_curPage++;
	if (d_plusPointInfo.name != "" )
	{
		showCurPageByPlusPoint();
	}
	else
	{
		showCurPage();
	}

	return true;
}

void KUiShop::getData()
{
	d_shopIdx = g_pCoreShell->GetGameData(GDI_GET_SHOP_IDX, NULL, NULL);
	g_pCoreShell->GetGameData( GDI_GET_PLUS_POINT_TEMPLATE, d_shopIdx, (int)&d_plusPointInfo);
	unsigned long money = g_pCoreShell->GetGameData(GDI_GET_PLUS_POINT, d_shopIdx, NULL);
	if (d_plusPointInfo.name != "" )
	{
		String shopName = AnsiToUtf8(d_plusPointInfo.name.c_str());
		d_Tittle->setText(shopName+d_defaultTittle);
		char szPlusPoint[COMMON_CLIENT_MSG_LEN_128];
		sprintf( szPlusPoint, "%s: %u/%u",d_plusPointInfo.name.c_str(), money, d_plusPointInfo.maxpluspoint );

		if (d_plusPointTxt != NULL)
		{
			if (!IsInsteadSpecieShop(g_pCoreShell->GetGameData(GDI_GET_PLUS_POINT_INDEX_BY_SHOP_INDEX, 0, d_shopIdx)))
			{
				d_plusPointTxt->show();
			}
			else
			{
				d_plusPointTxt->hide();
			}
		}
		d_plusPointTxt->setText(AnsiToUtf8(szPlusPoint));
		int currentShopIndex = g_pCoreShell->GetGameData( GDI_GET_PLUS_POINT_INDEX_BY_SHOP_INDEX, 0, d_shopIdx );
		if( !IsInsteadSpecieShop( currentShopIndex ) )
		{
			d_normalRepair->hide();	
			d_specialRepair->hide();	
			d_normalRepairAll->hide();
			d_specialRepairAll->hide();
		}
		else
		{
			d_plusPointTxt->hide();
			
			d_normalRepair->show();	
			d_specialRepair->show();	
			d_normalRepairAll->show();
			d_specialRepairAll->show();
		}
	}
	else
	{
		d_Tittle->setText(d_defaultTittle);
		d_plusPointTxt->hide();

		d_normalRepair->show();	
		d_specialRepair->show();	
		d_normalRepairAll->show();
		d_specialRepairAll->show();
	}
	d_itemCount = g_pCoreShell->GetGameData(GDI_TRADE_NPC_ITEM, 0, 0); 
	d_curPage = 1;
	d_pageCount = 1;
	if (d_itemCount == 0)
		return;

	if(d_items != NULL)
		delete[] d_items;

	d_items = new KObjAtContRegion[d_itemCount];
	if (d_items)
	{
		g_pCoreShell->GetGameData(GDI_TRADE_NPC_ITEM, (unsigned int)d_items, d_itemCount);
		d_pageCount = (d_itemCount - 1) / SHOP_ITEM_PER_PAGE + 1;
	}
}

void KUiShop::showCurPage()
{
	if (d_curPage <= 0 && d_curPage > d_pageCount && !d_items)
		return;

	if(d_curPage == 1)
		d_previous->disable();
	else
		d_previous->enable();

	int curPageItemCount;
	if (d_curPage == d_pageCount)
	{
		curPageItemCount = d_itemCount - (d_curPage - 1) * SHOP_ITEM_PER_PAGE;
		d_next->disable();
	}
	else
	{
		d_next->enable();
		curPageItemCount = SHOP_ITEM_PER_PAGE;
	}
	
	int money = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, NULL, NULL);
	char itemName[COMMON_CLIENT_MSG_LEN_128];
	//画有物品的格子
	for(int i = 0; i < curPageItemCount; i++)
	{
		KObjAtContRegion* curItem = &d_items[(d_curPage - 1) * SHOP_ITEM_PER_PAGE + i];
		curItem->Obj.uGenre = CGOG_NPCSELLITEM;
		curItem->eContainer = UOC_NPC_SHOP;
		curItem->bPlusShop = false;
		d_itemIcon[i]->setUserData(curItem);

		TLGameObject::GameObject itemObj;
		setItemImage(i + SHOP_ITEM_PER_PAGE * (d_curPage - 1), itemObj);
		itemObj.d_type = TLGameObject::idle;
		d_itemIcon[i]->setObject(itemObj);

		int price = g_pCoreShell->GetGameData(GDI_SHOP_ITEM_PRICE, NULL, curItem->Obj.uId);
		if(0 == price)
		{
			continue;
		}
		if(g_pCoreShell->GetGameData(GDI_SHOP_ITEM_NAME, (unsigned int)itemName, curItem->Obj.uId) == 0)
		{
			continue;
		}
		int jin = price / 10000;
		int yin = price % 10000 / 100;
		int tong = price % 100;

		char tempText[COMMON_CLIENT_MSG_LEN_256] = "\0";
		char loText[MAX_TEXT_LEN] = "\0";
		char* moneyColor = NULL;
		if(price > money)
		{
			moneyColor = (char*)KUiCfgLoader::getSingleton().getShopCfg().moneyNotEnoughTextColor;
		}
		else
		{
			moneyColor = (char*)KUiCfgLoader::getSingleton().getShopCfg().moneyTextColor;
		}
		char* itemNameColor = (char*)KUiCfgLoader::getSingleton().getShopCfg().itemNameColor;
		char* moneyFont = (char*)KUiCfgLoader::getSingleton().getShopCfg().moneyFont;
		char* itemNameFont = (char*)KUiCfgLoader::getSingleton().getShopCfg().itemFont;

		strcat(loText, "<Layout width=100>");

		sprintf(tempText, "<Seg text-align=left float=wrap><Obj type=text font-family=%s color=%s vertical-align=center>%s</Obj></Seg>",
			itemNameFont, itemNameColor, itemName);
		strcat(loText, tempText);

		strcat(loText, "<Seg text-align=left>");
		if(jin > 0)
		{
			sprintf(tempText, "<Obj type=text vertical-align=center font-family=%s color=%s>%d </Obj>", moneyFont, moneyColor, jin);
			strcat(loText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", KUiCfgLoader::getSingleton().getJinImagePath());
			strcat(loText, tempText);
		}
		
		if(yin > 0)
		{
			sprintf(tempText, "<Obj type=text vertical-align=center font-family=%s color=%s>%d </Obj>", moneyFont, moneyColor, yin);
			strcat(loText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", KUiCfgLoader::getSingleton().getYinImagePath());
			strcat(loText, tempText);
		}
		
		if(tong > 0 || 0 == jin && 0 == yin)
		{
			sprintf(tempText, "<Obj type=text vertical-align=center font-family=%s color=%s>%d </Obj>", moneyFont, moneyColor, tong);
			strcat(loText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", KUiCfgLoader::getSingleton().getTongImagePath());
			strcat(loText, tempText);
		}
		strcat(loText, "</Seg>");
		
		strcat(loText, "</Layout>");
		
		d_description[i]->useLayout();
		d_description[i]->getLayout()->SetText(loText);
		d_description[i]->getLayout()->flashLayout();
		d_description[i]->fitLayoutSize();
		d_description[i]->show();
		d_itemIcon[i]->show();
	}
	//画空格子
	for(int j = curPageItemCount; j < SHOP_ITEM_PER_PAGE; j++)
	{
// 		d_itemIcon[j]->setUserData(NULL);
// 		TLGameObject::GameObject itemObj;
// 		itemObj.d_gameobject = getEmptySpaceName();
// 		itemObj.d_type = TLGameObject::idle;
// 		d_itemIcon[j]->setObject(itemObj);
		d_itemIcon[j]->hide();
		d_description[j]->hide();
	}
}

void KUiShop::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->getChild();
	}
}

void KUiShop::show()
{
	KUiWndSingleton<KUiShop>::Show();
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		KUiPlayerState::getSingleton().setState(KUiPlayerState::TRADE_NPC_BUY_SALE);
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );

		ms_Singleton->getData();
		if (d_plusPointInfo.name != "" )
		{
			ms_Singleton->showCurPageByPlusPoint();
		}
		else
		{
			ms_Singleton->showCurPage();
		}
		
		KUiItemBox::getSingleton().show();
		if(KUiEquipment::IsVisible())
			KUiEquipment::Show();

		flashRepairTip();
	}
}

bool KUiShop::onWndHide(const EventArgs& e)
{
	KUiPlayerState::getSingleton().setState(KUiPlayerState::IDLE);
	KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );

	if(d_items != NULL)
	{
		delete[] d_items;
		d_items = NULL;
	}
	
	KUiWndSingleton<KUiShop>::Hide();

	if(KUiEquipment::IsVisible())
		KUiEquipment::Show();

	return true;
}

int KUiShop::getWndWidth()
{
	return m_pThisWnd->getWidth(Absolute);
}

bool KUiShop::onClose(const CEGUI::EventArgs& e)
{
	Hide();
	return true;
}

void KUiShop::setItemImage(int itemShopIndex, TLGameObject::GameObject& obj)
{
	KItemInfo tagItemInfo;
	g_pCoreShell->GetGameData(GDI_ITEM_INFO_SHOP, (unsigned int)&tagItemInfo, itemShopIndex);
	obj.d_gameobjectSet = tagItemInfo.szImageSet;
	obj.d_gameobject = tagItemInfo.szImage;
	obj.d_EdgeframeIdx = tagItemInfo.colour;
}

String KUiShop::getEmptySpaceName()
{
	return BACKGROUND_IMAGE;
}

void KUiShop::RepairAllItem()
{
	if(d_canRepairLevel <= 1)
	{
		char * errorMessage = KMessageCentre::GetMessage(not_enough_money_for_repair, 0);
		if (errorMessage != NULL && errorMessage[0] != 0 && g_pCoreShell != NULL)
		{
			g_pCoreShell->OperationRequest(GOI_AUTO_REPAIR_ERROR_MSG, (unsigned int)errorMessage, 0);
		}
		return;
	}
	
	g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR_ALL, NULL, true);
	
	d_specialRepairAll->disable();
	d_normalRepairAll->disable();
}

void KUiShop::flashRepairTip()
{	
	int holdMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	unsigned long insteadSpecieCount = g_pCoreShell->GetGameData(GDI_GET_INSTEAD_SPECIE, 0, KUiItemBox::getSingleton().GetInsteadSpecieIndex());
	unsigned long sumMoney = 0;
	if (holdMoney >= 0)
	{
		sumMoney = holdMoney + insteadSpecieCount;
	}
	else
	{
		sumMoney = insteadSpecieCount;
	}

	int specialPrice = getRepairAllPrice(true);
	int normalPrice = getRepairAllPrice(false);

	d_specialRepairAll->enable();
	d_normalRepairAll->enable();
	if(specialPrice == 0)
	{
		d_specialRepairAll->disable();
	}
	if(normalPrice == 0)
	{
		d_normalRepairAll->disable();
	}

	d_canRepairLevel = 0;
	if(sumMoney >= normalPrice)
	{
		d_canRepairLevel++;
	}
	if(sumMoney >= specialPrice)
	{
		d_canRepairLevel++;
	}


	static char layoutText[COMMON_CLIENT_MSG_LEN_1024];
	memset(layoutText, 0, COMMON_CLIENT_MSG_LEN_1024);
	
	genRepairAllPriceLayoutText(layoutText, true);
	d_specialRepairAll->setTooltipText(AnsiToUtf8(layoutText));
	
	genRepairAllPriceLayoutText(layoutText, false);
	d_normalRepairAll->setTooltipText(AnsiToUtf8(layoutText));
}

int	KUiShop::getRepairAllPrice(bool special)
{
	return g_pCoreShell->GetGameData(GDI_REPAIR_ALL_ITEM_PRICE, NULL, special);
}

bool KUiShop::genRepairAllPriceLayoutText(char* layoutText, bool special)
{
	int holdMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	unsigned long insteadSpecieCount = g_pCoreShell->GetGameData(GDI_GET_INSTEAD_SPECIE, 0, KUiItemBox::getSingleton().GetInsteadSpecieIndex());
	unsigned long sumMoney = 0;
	if (holdMoney >= 0)
	{
		sumMoney = holdMoney + insteadSpecieCount;
	}
	else
	{
		sumMoney = insteadSpecieCount;
	}

	ItemPriceLayout data;
	g_pCoreShell->GetGameData(GDI_ITEM_PRICE_LAYOUT_DATA, (unsigned int)&data, NULL);
	
	const char* moneyColor = data.moneyNormalColor;
	int price = getRepairAllPrice(special);
	bool enough = true;
	if(price > sumMoney)
	{
		enough = false;
		moneyColor = data.moneyNotEnoughColor;
	}
	
	char tempText[COMMON_CLIENT_MSG_LEN_128];

	const char* tipTitleText = NULL;
	if(special)
	{
		tipTitleText = KUiCfgLoader::getSingleton().getShopCfg().specialRepairTipTileText;
	}
	else
	{
		tipTitleText = KUiCfgLoader::getSingleton().getShopCfg().normalRepairTipTileText;
	}

	sprintf(layoutText, 
		"<Layout width=160 margin-left=10 margin-right=10>"
		"<Seg text-align=left float=wrap>"
		"<Obj type=text color=%s font-family=%s>%s</Obj>"
		"</Seg>", 
		KUiCfgLoader::getSingleton().getShopCfg().repairTipTileTextColor,
		KUiCfgLoader::getSingleton().getShopCfg().repairTipTileTextFont,
		tipTitleText);
	
	strcat(layoutText, "<Seg text-align=right float=wrap>");
	
	bool showMoney = false;
	if(price / 10000 > 0)
	{
		sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price / 10000);
		strcat(layoutText, tempText);
		sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", data.jinImage);
		strcat(layoutText, tempText);
		showMoney = true;
	}
	
	if(price % 10000 / 100 > 0)
	{
		sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price % 10000 / 100);
		strcat(layoutText, tempText);
		sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", data.yinImage);
		strcat(layoutText, tempText);
		showMoney = true;
	}
	
	if(price % 100 > 0 || showMoney == false)
	{
		sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price % 100);
		strcat(layoutText, tempText);
		sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", data.tongImage);
		strcat(layoutText, tempText);
	}
	
	strcat(layoutText, "</Seg>");
	strcat(layoutText, "</Layout>");

	return enough;
}

void KUiShop::showCurPageByPlusPoint()
{
	if ( d_plusPointInfo.name == "" )
	{
		return;
	}

	if (d_curPage <= 0 && d_curPage > d_pageCount && !d_items)
		return;

	if(d_curPage == 1)
		d_previous->disable();
	else
		d_previous->enable();

	int curPageItemCount;
	if (d_curPage == d_pageCount)
	{
		curPageItemCount = d_itemCount - (d_curPage - 1) * SHOP_ITEM_PER_PAGE;
		d_next->disable();
	}
	else
	{
		d_next->enable();
		curPageItemCount = SHOP_ITEM_PER_PAGE;
	}
	
	unsigned long money = g_pCoreShell->GetGameData(GDI_GET_PLUS_POINT, d_shopIdx, NULL);

	char szPlusPoint[COMMON_CLIENT_MSG_LEN_128];
	sprintf( szPlusPoint, "%s: %u/%u",d_plusPointInfo.name.c_str(), money, d_plusPointInfo.maxpluspoint );
	if (d_plusPointTxt != NULL)
	{
		if (!IsInsteadSpecieShop(g_pCoreShell->GetGameData(GDI_GET_PLUS_POINT_INDEX_BY_SHOP_INDEX, 0, d_shopIdx)))
		{
			d_plusPointTxt->show();
		}
		else
		{
			d_plusPointTxt->hide();
		}
	}
	d_plusPointTxt->setText(AnsiToUtf8(szPlusPoint));

	char itemName[COMMON_CLIENT_MSG_LEN_128];
	ItemPriceLayout data;
	g_pCoreShell->GetGameData(GDI_ITEM_PRICE_LAYOUT_DATA, (unsigned int)&data, NULL);
	string daiBiJing( "set:daibi image:daibi-jin" );
	string daiBiYin( "set:daibi image:daibi-yin" );
	string daiBiTong( "set:daibi image:daibi-tong" );
	//画有物品的格子
	for(int i = 0; i < curPageItemCount; i++)
	{
		KObjAtContRegion* curItem = &d_items[(d_curPage - 1) * SHOP_ITEM_PER_PAGE + i];
		curItem->Obj.uGenre = CGOG_NPCSELLITEM;
		curItem->eContainer = UOC_NPC_SHOP;
		curItem->bPlusShop = true;
		d_itemIcon[i]->setUserData(curItem);

		TLGameObject::GameObject itemObj;
		setItemImage(i + SHOP_ITEM_PER_PAGE * (d_curPage - 1), itemObj);
		itemObj.d_type = TLGameObject::idle;
		d_itemIcon[i]->setObject(itemObj);

		int price = g_pCoreShell->GetGameData(GDI_SHOP_ITEM_PRICE, NULL, curItem->Obj.uId);
		if(0 == price)
		{
			continue;
		}
		if(g_pCoreShell->GetGameData(GDI_SHOP_ITEM_NAME, (unsigned int)itemName, curItem->Obj.uId) == 0)
		{
			continue;
		}

		char tempText[COMMON_CLIENT_MSG_LEN_256] = "\0";
		char loText[MAX_TEXT_LEN] = "\0";
		char* moneyColor = NULL;
		if(price > money)
		{
			moneyColor = (char*)KUiCfgLoader::getSingleton().getShopCfg().moneyNotEnoughTextColor;
		}
		else
		{
			moneyColor = (char*)KUiCfgLoader::getSingleton().getShopCfg().moneyTextColor;
		}
		char* itemNameColor = (char*)KUiCfgLoader::getSingleton().getShopCfg().itemNameColor;
		char* moneyFont = (char*)KUiCfgLoader::getSingleton().getShopCfg().moneyFont;
		char* itemNameFont = (char*)KUiCfgLoader::getSingleton().getShopCfg().itemFont;

		strcat(loText, "<Layout width=100>");

		sprintf(tempText, "<Seg text-align=left float=wrap><Obj type=text font-family=%s color=%s vertical-align=center>%s</Obj></Seg>",
			itemNameFont, itemNameColor, itemName);
		strcat(loText, tempText);

		strcat(loText, "<Seg text-align=left>");
		int currentShopIndex = g_pCoreShell->GetGameData( GDI_GET_PLUS_POINT_INDEX_BY_SHOP_INDEX, 0, d_shopIdx );
		if( !IsInsteadSpecieShop( currentShopIndex ) )
		{
			if(price > 0 )
			{
				
				sprintf(tempText, "<Obj type=text vertical-align=center font-family=%s color=%s>%d %s</Obj>", moneyFont, moneyColor, price, d_plusPointInfo.name.c_str());
				strcat(loText, tempText);
				//sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", KUiCfgLoader::getSingleton().getTongImagePath());
				//strcat(loText, tempText);
			}
		}
		else
		{
			bool showMoney = false;
			if(price / 10000 > 0)
			{
				sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price / 10000);
				strcat(loText, tempText);
				sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", daiBiJing.c_str() );
				strcat(loText, tempText);
				showMoney = true;
			}
			
			if(price % 10000 / 100 > 0)
			{
				sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price % 10000 / 100);
				strcat(loText, tempText);
				sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", daiBiYin.c_str() );
				strcat(loText, tempText);
				showMoney = true;
			}
			
			if(price % 100 > 0 || showMoney == false)
			{
				sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price % 100);
				strcat(loText, tempText);
				sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", daiBiTong.c_str() );
				strcat(loText, tempText);
			}
		}
		
		strcat(loText, "</Seg>");
		
		strcat(loText, "</Layout>");
		
		d_description[i]->useLayout();
		d_description[i]->getLayout()->SetText(loText);
		d_description[i]->getLayout()->flashLayout();
		d_description[i]->fitLayoutSize();
		d_description[i]->show();
		d_itemIcon[i]->show();
	}
	//画空格子
	for(int j = curPageItemCount; j < SHOP_ITEM_PER_PAGE; j++)
	{
// 		d_itemIcon[j]->setUserData(NULL);
// 		TLGameObject::GameObject itemObj;
// 		itemObj.d_gameobject = getEmptySpaceName();
// 		itemObj.d_type = TLGameObject::idle;
// 		d_itemIcon[j]->setObject(itemObj);
		d_itemIcon[j]->hide();
		d_description[j]->hide();
	}
}