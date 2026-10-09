
#include "UiTradeConfirmBox.h"
#include "UiPlayerState.h"
#include "UiShop.h"
#include "UiChatWindow.h"
#include "UiErrorMessageBox.h"
#include "../UiConfigManager.h"
#include "../UiConfigManager.h"
#include "Ui/UiCase/UiItemBox.h"

extern iCoreShell*		g_pCoreShell;

template<> 
KUiTradeConfirmBox* KUiWndSingleton<KUiTradeConfirmBox>::ms_Singleton	= NULL;

KUiTradeConfirmBox::KUiTradeConfirmBox(const CEGUI::String& id_name):
KUiWndSingleton<KUiTradeConfirmBox>( id_name )
{
	d_itemCount = 0;
}

KUiTradeConfirmBox::~KUiTradeConfirmBox()
{
}

void KUiTradeConfirmBox::getChild()
{
	String parentWndName = "TaharezLook/TradeConfirmBox";
	d_sell = (TLStaticImage*)m_pThisWnd->getChild(parentWndName + "/Sell");
	d_buy = (TLStaticImage*)m_pThisWnd->getChild(parentWndName + "/Buy");
	d_repair = (TLStaticImage*)m_pThisWnd->getChild(parentWndName + "/Repair");
	d_count = (TLEditbox*)m_pThisWnd->getChild(parentWndName + "/Count");
	d_increase = (TLButton*)m_pThisWnd->getChild(parentWndName + "/Increase");
	d_decrease = (TLButton*)m_pThisWnd->getChild(parentWndName + "/Decrease");

	d_jin = (TLStaticText*)m_pThisWnd->getChild(parentWndName + "/JinText");
	d_yin = (TLStaticText*)m_pThisWnd->getChild(parentWndName + "/YinText");
	d_tong = (TLStaticText*)m_pThisWnd->getChild(parentWndName + "/TongText");

	d_jinImg = m_pThisWnd->getChild(parentWndName + "/JinImage");
	d_yinImg = m_pThisWnd->getChild(parentWndName + "/YinImage");
	d_tongImg = m_pThisWnd->getChild(parentWndName + "/TongImage");

	d_plusPointTxt = (TLStaticText*)m_pThisWnd->getChild(parentWndName + "/PlusPointTxt");

	d_ok = (TLButton*)m_pThisWnd->getChild(parentWndName + "/Ok");
	d_cancel = (TLButton*)m_pThisWnd->getChild(parentWndName + "/Cancel");
	d_close = (TLButton*)m_pThisWnd->getChild(parentWndName + "/Close");
	d_name = (TLStaticText*)m_pThisWnd->getChild(parentWndName + "/Name");

	d_ok->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiTradeConfirmBox::onOk, this));
	d_cancel->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiTradeConfirmBox::onCancel, this));
	d_close->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiTradeConfirmBox::onCancel, this));

	d_count->subscribeEvent(TLEditbox::EventTextChanged, Event::Subscriber(&KUiTradeConfirmBox::onCountChanged, this));
	d_count->subscribeEvent(TLEditbox::EventKeyDown, Event::Subscriber(&KUiTradeConfirmBox::handleKeyDown, this));
	d_count->subscribeEvent(TLEditbox::EventMouseClick, Event::Subscriber(&KUiTradeConfirmBox::handleMouseClick, this));

	d_increase->subscribeEvent(TLButton::EventMouseButtonDown, Event::Subscriber(&KUiTradeConfirmBox::onIncreaseDown, this));
	d_increase->subscribeEvent(TLButton::EventMouseDoubleClick, Event::Subscriber(&KUiTradeConfirmBox::onIncreaseDown, this));
	d_increase->subscribeEvent(TLButton::EventNewFrame, Event::Subscriber(&KUiTradeConfirmBox::onIncreaseHover, this));
	d_increase->subscribeEvent(TLButton::EventMouseButtonUp, Event::Subscriber(&KUiTradeConfirmBox::onIncreaseUp, this));

	d_decrease->subscribeEvent(TLButton::EventMouseButtonDown, Event::Subscriber(&KUiTradeConfirmBox::onDecreaseDown, this));
	d_decrease->subscribeEvent(TLButton::EventMouseDoubleClick, Event::Subscriber(&KUiTradeConfirmBox::onDecreaseDown, this));
	d_decrease->subscribeEvent(TLButton::EventNewFrame, Event::Subscriber(&KUiTradeConfirmBox::onDecreaseHover, this));
	d_decrease->subscribeEvent(TLButton::EventMouseButtonUp, Event::Subscriber(&KUiTradeConfirmBox::onDecreaseUp, this));

	m_pThisWnd->subscribeEvent(TLButton::EventShown, Event::Subscriber(&KUiTradeConfirmBox::onShow, this));
	m_pThisWnd->subscribeEvent(TLButton::EventHidden, Event::Subscriber(&KUiTradeConfirmBox::onHide, this));
	m_pThisWnd ->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiTradeConfirmBox::handleKeyDown, this));
}

void KUiTradeConfirmBox::hideAll()
{
	d_sell->hide();
	d_buy->hide();
	d_repair->hide();
	d_count->hide();
	d_increase->hide();
	d_decrease->hide();
	d_itemCount = 1;
	d_cost = true;
	d_increasing = false;
	d_decreasing = false;
	d_mousePushDelayTime = UI_TRADE_COMFIRM_BOX_MAX_DELAY_TIME;
}

void KUiTradeConfirmBox::Init()
{
	getChild();
	d_itemCount = 1;
}

void KUiTradeConfirmBox::ShowRepairPrice()
{
	int price = d_aItemPrice * d_itemCount;
	
	int jin = price / 10000;
	int yin = price % 10000 / 100;
	int tong = price % 100;
	
	d_plusPointTxt->hide();
	
	d_jinImg->show();
	d_yinImg->show(); 
	d_tongImg->show();
	
	d_jin->show();
	d_yin->show();
	d_tong->show();
	d_jin->setText(iToString(jin));
	d_yin->setText(iToString(yin));
	d_tong->setText(iToString(tong));
	
	if(d_cost)
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
		d_ok->setEnabled(sumMoney >= price);
	}
	else
	{
		d_ok->enable();
	}
	if (d_itemCount > 0)
	{
		d_count->resetText(iToString(d_itemCount));
	}
	else
	{
		d_count->resetText(CEGUI::String(""));
	}
}

void KUiTradeConfirmBox::showPrice()
{
	int price = d_aItemPrice * d_itemCount;

	int jin = price / 10000;
	int yin = price % 10000 / 100;
	int tong = price % 100;

	d_plusPointTxt->hide();

	d_jinImg->show();
	d_yinImg->show(); 
	d_tongImg->show();

	d_jin->show();
	d_yin->show();
	d_tong->show();
	d_jin->setText(iToString(jin));
	d_yin->setText(iToString(yin));
	d_tong->setText(iToString(tong));
	
	if(d_cost)
	{
		int holdMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
		d_ok->setEnabled(holdMoney >= price);
	}
	else
	{
		d_ok->enable();
	}
	if (d_itemCount > 0)
	{
		d_count->resetText(iToString(d_itemCount));
	}
	else
	{
		d_count->resetText(CEGUI::String(""));
	}
	
}

void	KUiTradeConfirmBox::showPriceByPlusPoint()
{
	int price = d_aItemPrice * d_itemCount;
	
	d_jinImg->hide();
	d_yinImg->hide(); 
	d_tongImg->hide();

	d_jin->hide();
	d_yin->hide();
	d_tong->hide();
	int shopIndex = g_pCoreShell->GetGameData(GDI_GET_PLUS_POINT_INDEX_BY_SHOP_INDEX, 0, d_shopIdx);
	unsigned long holdMoney = g_pCoreShell->GetGameData(GDI_GET_PLUS_POINT, d_shopIdx, 0);

	if( KUiItemBox::getSingleton().GetInsteadSpecieIndex() != shopIndex )
	{
		char szPlusPoint[COMMON_CLIENT_MSG_LEN_128];
		sprintf( szPlusPoint, "%d %s", price, d_plusPointInfo.name.c_str() );
		d_plusPointTxt->show();
		d_plusPointTxt->setText(AnsiToUtf8(szPlusPoint));
		d_plusPointTxt->useLayout();
		d_plusPointTxt->getLayout()->clearLayout();
	}
	else
	{
		string daiBiJing( "set:daibi image:daibi-jin" );
		string daiBiYin( "set:daibi image:daibi-yin" );
		string daiBiTong( "set:daibi image:daibi-tong" );
		char tempText[COMMON_CLIENT_MSG_LEN_256] = "\0";
		char loText[MAX_TEXT_LEN] = "\0";
		char* moneyColor = NULL;
		moneyColor = (char*)KUiCfgLoader::getSingleton().getShopCfg().moneyTextColor;
		char* itemNameColor = (char*)KUiCfgLoader::getSingleton().getShopCfg().itemNameColor;
		char* moneyFont = (char*)KUiCfgLoader::getSingleton().getShopCfg().moneyFont;
		char* itemNameFont = (char*)KUiCfgLoader::getSingleton().getShopCfg().itemFont;
		
		strcat(loText, "<Layout width=100>");
		
		strcat(loText, "<Seg text-align=right>");
		if(price / 10000 > 0)
		{
			sprintf(tempText, "<Obj type=text %s vertical-align=center> %d</Obj>", moneyColor, price / 10000);
			strcat(loText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", daiBiJing.c_str() );
			strcat(loText, tempText);
		}
		
		if(price % 10000 / 100 > 0)
		{
			sprintf(tempText, "<Obj type=text %s vertical-align=center> %d</Obj>", moneyColor, price % 10000 / 100);
			strcat(loText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", daiBiYin.c_str() );
			strcat(loText, tempText);
		}
		
		if(price % 100 > 0)
		{
			sprintf(tempText, "<Obj type=text %s vertical-align=center> %d</Obj>", moneyColor, price % 100);
			strcat(loText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", daiBiTong.c_str() );
			strcat(loText, tempText);
		}
		
		
		strcat(loText, "</Seg>");
		
		strcat(loText, "</Layout>");

		d_plusPointTxt->setText( "" );
		d_plusPointTxt->useLayout();
		d_plusPointTxt->getLayout()->formatText(loText);
		d_plusPointTxt->getLayout()->SetText(loText);
		d_plusPointTxt->getLayout()->flashLayout();
		d_plusPointTxt->show();
	}

	if(d_cost)
	{
		
		d_ok->setEnabled(holdMoney >= price);
	}
	else
	{
		d_ok->enable();
	}
	if (d_itemCount > 0)
	{
		d_count->resetText(iToString(d_itemCount));
	}
	else
	{
		d_count->resetText(CEGUI::String(""));
	}
}

void KUiTradeConfirmBox::show(KObjAtContRegion* region)
{
	d_region = *region;
	int price = region->Obj.uId;
	hideAll();
	
	switch(KUiPlayerState::getSingleton().getState())
	{
	case KUiPlayerState::TRADE_NPC_BUY_SALE:
		{
			if(d_region.eContainer == UOC_NPC_SHOP)//从商店中买
			{
				d_buy->show();
				d_count->show();
				d_increase->show();
				d_decrease->show();
				d_aItemPrice = g_pCoreShell->GetGameData(GDI_SHOP_ITEM_PRICE, NULL, d_region.Obj.uId);
				char itemName[COMMON_CLIENT_MSG_LEN_32];
				g_pCoreShell->GetGameData(GDI_SHOP_ITEM_NAME, (UINT)itemName, d_region.Obj.uId);
				d_name->setText(AnsiToUtf8(itemName));
				
				d_cost = true;
				if (d_plusPointInfo.name != "" )
				{
					showPriceByPlusPoint();
				}
				else
				{
					showPrice();
				}
			}
			else if(d_region.eContainer == UOC_ITEM_TAKE_WITH)//卖
			{
				d_sell->show();
				d_aItemPrice = g_pCoreShell->GetGameData(GDI_GET_ITEM_SELL_PRICE_BY_ID, NULL, d_region.Obj.uId);
				char itemName[COMMON_CLIENT_MSG_LEN_32];
				g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_BY_ID, (UINT)itemName, d_region.Obj.uId);
				d_name->setText(AnsiToUtf8(itemName));
				
				d_cost = false;
				if (d_plusPointInfo.name != "" )
				{
					showPriceByPlusPoint();
				}
				else
				{
					showPrice();
				}
			}
		}
		break;
	case KUiPlayerState::TRADE_NPC_NORMAL_REPAIR:
		{
			d_repair->show();			
			d_aItemPrice = g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE, (UINT)false, d_region.Obj.uId);
			if(d_aItemPrice <= 0)
			{
				KUiChannelCentre::GetSingleton().toSysMsg(KUiCfgLoader::getSingleton().getShopCfg().noNeedRepairText);
				return;
			}
			char itemName[COMMON_CLIENT_MSG_LEN_32];
			int itemId = g_pCoreShell->GetGameData(GDI_GET_ITEM_ID_BY_INDEX, d_region.Obj.uId, NULL);
			g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_BY_ID, (UINT)itemName, itemId);
			d_name->setText(AnsiToUtf8(itemName));
			
			d_cost = true;
			ShowRepairPrice();
		}
		break;
	case KUiPlayerState::TRADE_NPC_SPECIAL_REPAIR:
		{
			d_repair->show();			
			d_aItemPrice = g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE, (UINT)true, d_region.Obj.uId);
			if(d_aItemPrice <= 0)
			{
				KUiChannelCentre::GetSingleton().toSysMsg(KUiCfgLoader::getSingleton().getShopCfg().noNeedRepairText);
				return;
			}
			char itemName[COMMON_CLIENT_MSG_LEN_32];
			int itemId = g_pCoreShell->GetGameData(GDI_GET_ITEM_ID_BY_INDEX, d_region.Obj.uId, NULL);
			g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_BY_ID, (UINT)itemName, itemId);
			d_name->setText(AnsiToUtf8(itemName));
			
			d_cost = true;
			ShowRepairPrice();
		}
		break;
	}
	
	Show();
	
	d_ok->show();
	d_cancel->show();
	
	adjustPos();
	d_count->activate();
}

void KUiTradeConfirmBox::show(KObjAtContRegion* region, int shopIdx, const PLUS_POINT_PARAM& ppt)
{
	d_region = *region;
	d_shopIdx = shopIdx;
	d_plusPointInfo.name = ppt.name;
	d_plusPointInfo.maxpluspoint = ppt.maxpluspoint;
	int price = region->Obj.uId;
	hideAll();

	switch(KUiPlayerState::getSingleton().getState())
	{
	case KUiPlayerState::TRADE_NPC_BUY_SALE:
		{
			if(d_region.eContainer == UOC_NPC_SHOP)//从商店中买
			{
				d_buy->show();
				d_count->show();
				d_increase->show();
				d_decrease->show();
				d_aItemPrice = g_pCoreShell->GetGameData(GDI_SHOP_ITEM_PRICE, NULL, d_region.Obj.uId);
				char itemName[COMMON_CLIENT_MSG_LEN_32];
				g_pCoreShell->GetGameData(GDI_SHOP_ITEM_NAME, (UINT)itemName, d_region.Obj.uId);
				d_name->setText(AnsiToUtf8(itemName));

				d_cost = true;
				if (d_plusPointInfo.name != "" )
				{
					showPriceByPlusPoint();
				}
				else
				{
					showPrice();
				}
			}
			else if(d_region.eContainer == UOC_ITEM_TAKE_WITH)//卖
			{
				d_sell->show();
				d_aItemPrice = g_pCoreShell->GetGameData(GDI_GET_ITEM_SELL_PRICE_BY_ID, NULL, d_region.Obj.uId);
				char itemName[COMMON_CLIENT_MSG_LEN_32];
				g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_BY_ID, (UINT)itemName, d_region.Obj.uId);
				d_name->setText(AnsiToUtf8(itemName));

				d_cost = false;
				if (d_plusPointInfo.name != "" )
				{
					showPriceByPlusPoint();
				}
				else
				{
					showPrice();
				}
			}
		}
		break;
	case KUiPlayerState::TRADE_NPC_NORMAL_REPAIR:
		{
			d_repair->show();			
			d_aItemPrice = g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE, (UINT)false, d_region.Obj.uId);
			if(d_aItemPrice <= 0)
			{
				KUiChannelCentre::GetSingleton().toSysMsg(KUiCfgLoader::getSingleton().getShopCfg().noNeedRepairText);
				return;
			}
			char itemName[COMMON_CLIENT_MSG_LEN_32];
			int itemId = g_pCoreShell->GetGameData(GDI_GET_ITEM_ID_BY_INDEX, d_region.Obj.uId, NULL);
			g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_BY_ID, (UINT)itemName, itemId);
			d_name->setText(AnsiToUtf8(itemName));

			d_cost = true;
			showPrice();
		}
		break;
	case KUiPlayerState::TRADE_NPC_SPECIAL_REPAIR:
		{
			d_repair->show();			
			d_aItemPrice = g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE, (UINT)true, d_region.Obj.uId);
			if(d_aItemPrice <= 0)
			{
				KUiChannelCentre::GetSingleton().toSysMsg(KUiCfgLoader::getSingleton().getShopCfg().noNeedRepairText);
				return;
			}
			char itemName[COMMON_CLIENT_MSG_LEN_32];
			int itemId = g_pCoreShell->GetGameData(GDI_GET_ITEM_ID_BY_INDEX, d_region.Obj.uId, NULL);
			g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_BY_ID, (UINT)itemName, itemId);
			d_name->setText(AnsiToUtf8(itemName));
			
			d_cost = true;
			showPrice();
		}
		break;
	}
	
	Show();

	d_ok->show();
	d_cancel->show();

	adjustPos();
	d_count->activate();
}

void KUiTradeConfirmBox::adjustPos()
{
	Point newPos;
	Rect mainWnd = m_pRootSheet->getRect(Absolute);
	Rect thisWnd = m_pThisWnd->getRect(Absolute);
	Point mouse = MouseCursor::getSingleton().getPosition();
	if(mouse.d_y + thisWnd.getHeight() > mainWnd.d_bottom)
	{
		newPos.d_y = mouse.d_y - thisWnd.getHeight() - 10.0f;
	}
	else
	{
		newPos.d_y = mouse.d_y + 10.0f;
	}

	if(mouse.d_x + thisWnd.getWidth() > mainWnd.d_right)
	{
		newPos.d_x = mouse.d_x - thisWnd.getWidth() - 10.0f;
	}
	else
	{
		newPos.d_x = mouse.d_x + 10.0f;
	}

	m_pThisWnd->setPosition(Absolute, newPos);
}

bool KUiTradeConfirmBox::onOk(const CEGUI::EventArgs& e)
{	
	switch(KUiPlayerState::getSingleton().getState())
	{
	case KUiPlayerState::TRADE_NPC_BUY_SALE:
		{
			if(d_region.eContainer == UOC_NPC_SHOP)//买
			{
				g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY, (unsigned int)d_region.Obj.uId, d_itemCount);
			}
			else if(d_region.eContainer == UOC_ITEM_TAKE_WITH)//卖
			{
				g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL,	(unsigned int)&d_region, 0);
			}
			playSound(KUiCfgLoader::getSingleton().getSoundEffectCfg().costMoney);
		}
		break;
	case KUiPlayerState::TRADE_NPC_NORMAL_REPAIR:
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR, (unsigned int)&d_region, false);
			playSound(KUiCfgLoader::getSingleton().getSoundEffectCfg().repair);
		}
		break;
	case KUiPlayerState::TRADE_NPC_SPECIAL_REPAIR:
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR, (unsigned int)&d_region, true);
			playSound(KUiCfgLoader::getSingleton().getSoundEffectCfg().repair);
		}
		break;
	}
	ms_Singleton->Hide();
	return true;
}

bool KUiTradeConfirmBox::onCancel(const CEGUI::EventArgs& e)
{
	ms_Singleton->Hide();
	return true;
}

bool KUiTradeConfirmBox::onIncreaseDown(const CEGUI::EventArgs& e)
{
	++d_itemCount;
	if(d_itemCount > UI_TRADE_COMFIRM_BOX_MAX_TRADE_ITEM_COUNT)
	{
		d_itemCount = UI_TRADE_COMFIRM_BOX_MAX_TRADE_ITEM_COUNT;
	}

	if (d_plusPointInfo.name != "" )
	{
		showPriceByPlusPoint();
	}
	else
	{
		showPrice();
	}

	d_mousePushDelayTime = UI_TRADE_COMFIRM_BOX_MAX_DELAY_TIME;
	d_increase->beginUpdate();
	d_increasing = true;
	return true;
}

bool KUiTradeConfirmBox::onIncreaseHover(const CEGUI::EventArgs& e)
{
	if(false == d_increasing)
		return true;

	if(d_mousePushDelayTime > 0)
	{
		--d_mousePushDelayTime;
		return true;
	}

	++d_itemCount;
	if(d_itemCount > UI_TRADE_COMFIRM_BOX_MAX_TRADE_ITEM_COUNT)
	{
		d_itemCount = UI_TRADE_COMFIRM_BOX_MAX_TRADE_ITEM_COUNT;
	}
	
	if (d_plusPointInfo.name != "" )
	{
		showPriceByPlusPoint();
	}
	else
	{
		showPrice();
	}
	return true;
}

bool KUiTradeConfirmBox::onIncreaseUp(const CEGUI::EventArgs& e)
{
	d_increase->stopUpdate();
	d_increasing = false;
	return true;
}

bool KUiTradeConfirmBox::onDecreaseDown(const CEGUI::EventArgs& e)
{
	--d_itemCount;
	if(d_itemCount < 1)
	{
		d_itemCount = 1;
	}
	if (d_plusPointInfo.name != "" )
	{
		showPriceByPlusPoint();
	}
	else
	{
		showPrice();
	}

	d_mousePushDelayTime = UI_TRADE_COMFIRM_BOX_MAX_DELAY_TIME;
	d_decrease->beginUpdate();
	d_decreasing = true;
	return true;
}

bool KUiTradeConfirmBox::onDecreaseHover(const CEGUI::EventArgs& e)
{
	if(false == d_decreasing)
		return true;

	if(d_mousePushDelayTime > 0)
	{
		--d_mousePushDelayTime;
		return true;
	}

	--d_itemCount;
	if(d_itemCount < 1)
	{
		d_itemCount = 1;
	}
	
	if (d_plusPointInfo.name != "" )
	{
		showPriceByPlusPoint();
	}
	else
	{
		showPrice();
	}
	return true;
}

bool KUiTradeConfirmBox::onDecreaseUp(const CEGUI::EventArgs& e)
{
	d_decrease->stopUpdate();
	d_decreasing = false;
	return true;
}

bool KUiTradeConfirmBox::onCountChanged(const CEGUI::EventArgs& e)
{
	String countString = d_count->getText();
	for(int i = 0; i < countString.length(); i++)
	{
		int num = countString[i];
		if(num < 48 || num > 57)
		{
			countString.erase(i, 1);
			i--;
			continue;
		}
	}

	int count = atoi(countString.c_str());

	if(count < 1)
	{
		count = 0;
		d_count->resetText(CEGUI::String(""));
		d_count->setCaratIndex(size_t(1));
	}
	if(count > UI_TRADE_COMFIRM_BOX_MAX_TRADE_ITEM_COUNT)
	{
		count = UI_TRADE_COMFIRM_BOX_MAX_TRADE_ITEM_COUNT;
		d_count->resetText(iToString(count));
	}

	d_itemCount = count;
	if (d_plusPointInfo.name != "" )
	{
		showPriceByPlusPoint();
	}
	else
	{
		showPrice();
	}

	return true;
}

bool KUiTradeConfirmBox::onShow(const CEGUI::EventArgs& e)
{
	m_pThisWnd->setModalState(true);
	return true;
}

bool KUiTradeConfirmBox::onHide(const CEGUI::EventArgs& e)
{
	d_increase->stopUpdate();
	d_decrease->stopUpdate();
	m_pThisWnd->setModalState(false);
	return true;
}

bool KUiTradeConfirmBox::handleKeyDown(const CEGUI::EventArgs& e)
{
	using namespace CEGUI;

    switch (static_cast<const KeyEventArgs&>(e).scancode)
    {
	case Key::Return:
		{
			onOk(e);
		}
		break;
	}
	return true;
}

bool KUiTradeConfirmBox::handleMouseClick(const CEGUI::EventArgs & e)
{
	d_itemCount = 0;
	d_count->resetText(CEGUI::String(""));
	d_count->setCaratIndex(size_t(1));
	if (d_plusPointInfo.name != "" )
	{
		showPriceByPlusPoint();
	}
	else
	{
		showPrice();
	}
	return true;
}