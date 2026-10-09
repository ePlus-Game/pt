//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007年3月22日
//      File_base        : 
//      File_ext         : h
//      Author           : xiehong
//      Description      : tip描述生成
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "UiTipGenerator.h"
#include "CoreShell.h"
#include "AuctionComDef.h"
#include "..\UiCommon.h"
#include "UiPlayerState.h"
#include "UiFSbible_QuestData.h"
#include "..\UiConfigManager.h"
#include "UiItemBox.h"
extern iCoreShell*		g_pCoreShell;

KUiTipGenerator::KUiTipGenerator()
{
	sprintf(d_headText, "<Layout width=%d margin-top=%d margin-left=%d margin-right=%d margin-bottom=%d>", 
		KUiCfgLoader::getSingleton().getTipData().windowWidth,
		KUiCfgLoader::getSingleton().getTipData().topMargin,
		KUiCfgLoader::getSingleton().getTipData().leftMargin,
		KUiCfgLoader::getSingleton().getTipData().RightMargin,
		KUiCfgLoader::getSingleton().getTipData().bottomMargin);
}

char* KUiTipGenerator::genLayoutDes(TipObject& tipObj)
{
	//商店的id是在core里维护的，所以同一个商品index可能是不同商店
	//if(d_lastObject == tipObj && tipObj.type != ShopItem)
	//{
	//	return d_layoutText;
	//}

	d_lastObject = tipObj;
	d_layoutText[0] = 0;

	strcat(d_layoutText, d_headText);

	switch(tipObj.type)
	{
	case MyItem:
		{
			g_pCoreShell->GetGameData(GDI_MY_ITEM_LAYOUT_DESC, (unsigned int)d_layoutText, d_lastObject.ids[0]);
		}
		break;
	case OppositeItem:
		{
			g_pCoreShell->GetGameData(GDI_OPPOSITE_ITEM_LAYOUT_DESC, (unsigned int)d_layoutText, d_lastObject.ids[0]);
		}
		break;
	case MyGua:
		{
			g_pCoreShell->GetGameData(GDI_MY_GUA_LAYOUT_DESC, (unsigned int)d_layoutText, d_lastObject.ids[0]);
		}
		break;
	case OppositeGua:
		{
			g_pCoreShell->GetGameData(GDI_OPPOSITE_GUA_LAYOUT_DESC, (unsigned int)d_layoutText, d_lastObject.ids[0]);
		}
		break;
	case InsideBall:
		{
			TM_HOLE_POS holePos;
			holePos.talismanId	= tipObj.ids[0];
			holePos.holeIndex	= tipObj.ids[1];
			g_pCoreShell->GetGameData(GDI_INSIDE_BALL_LAYOUT_DESC, (unsigned int)d_layoutText, (int)&holePos);
		}
		break;
	case LinkedItem:
		{
			FIND_ITEMINDEX_PARAM itemType;
			itemType.nGenre			= tipObj.ids[0];
			itemType.nDetail		= tipObj.ids[1];
			itemType.nParticular	= tipObj.ids[2];
			itemType.nLevel			= tipObj.ids[3];
			itemType.nGroup			= tipObj.ids[4];
			g_pCoreShell->GetGameData(GDI_LINKED_ITEM_LAYOUT_DESC, (unsigned int)d_layoutText, (int)&itemType);
		}
		break;
	case VendueItem:
		{
			int itemId = tipObj.ids[0];

			//得到中间层管理对象
			IUIMDL*	m_pUiMDLManager;
			GetMDLPtr( &m_pUiMDLManager );
			
			//得到拍卖行数据集
			IUIMDLDataset* pIDataset = NULL;
			int nErr = m_pUiMDLManager->queryDataSet( vendue_dataset, &pIDataset );
			if ( nErr != success_errorcode )
			{
				break;
			}

			//从数据集中按指定id查询数据
			UIMDLDatasetRecord& rRecord = pIDataset->findDataRecord( _vendueFindItemFromMDL, &itemId );
			SEARCH_DB_RETDATA* pSearchData = (SEARCH_DB_RETDATA*)rRecord.pRecordData;
			if(pSearchData == NULL)
			{
				break;
			}
			TItemtransfersData itemInfo;
			ZeroMemory(&itemInfo, sizeof(itemInfo));
			memcpy(&itemInfo, &pSearchData->itemData, sizeof(pSearchData->itemData));

			g_pCoreShell->GetGameData(GDI_VENDUE_ITEM_LAYOUT_DESC, (unsigned int)d_layoutText, (int)&itemInfo);
		}
		break;
	case PlusPointShopItem:
	case ShopItem:
		{
			g_pCoreShell->GetGameData(GDI_SHOP_ITEM_LAYOUT_DESC, (unsigned int)d_layoutText, d_lastObject.ids[0]);
		}
		break;
	case QuestIcon:
		{
			int rewardTypeId = tipObj.ids[0];
			KRewardType* reward = KUiFSBibleQuestData::getSingleton().getRewardTypeById(rewardTypeId);
			if(reward)
			{
				strcat(d_layoutText, reward->tip);
			}
		}
		break;
	default:
		{
			strcpy(d_layoutText, 
				"<Seg text-align=center >"
					"<Obj type=text color=255,0,0>error: nothing to show!</Obj>"
				"</Seg>");
		}
		break;
	}
	if (PlusPointShopItem == tipObj.type)
	{
		addItemPlusPointPrice(tipObj);
	}
	else
	{
		addItemPrice(tipObj);
	}

	strcat(d_layoutText, "</Layout>");		
	d_layoutText[LAYOUT_TEXT_MAX_LEN - 1] = 0;
	return d_layoutText;
}

char* KUiTipGenerator::genCompareLayoutDes(TipObject& tipObj)
{
	d_compareLayoutText[0] = 0;

	//先得到该物品对应的装备位置
	int equipPos = itempart_unidentified;
	switch(tipObj.type)
	{
	case MyItem:
		{
			equipPos = g_pCoreShell->GetGameData(GDI_MY_ITEM_EQUIP_POS, NULL, d_lastObject.ids[0]);
		}
		break;
	case OppositeItem:
		{
			equipPos = g_pCoreShell->GetGameData(GDI_OPPOSITE_EQUIP_POS, NULL, d_lastObject.ids[0]);
		}
		break;
	case LinkedItem:
		{
			FIND_ITEMINDEX_PARAM itemType;
			itemType.nGenre			= tipObj.ids[0];
			itemType.nDetail		= tipObj.ids[1];
			itemType.nParticular	= tipObj.ids[2];
			itemType.nLevel			= tipObj.ids[3];
			itemType.nGroup			= tipObj.ids[4];
			equipPos = g_pCoreShell->GetGameData(GDI_LINKED_EQUIP_POS, NULL, (int)&itemType);
		}
		break;
	case VendueItem:
		{
			int itemId = tipObj.ids[0];

			//得到中间层管理对象
			IUIMDL*	m_pUiMDLManager;
			GetMDLPtr( &m_pUiMDLManager );
			
			//得到拍卖行数据集
			IUIMDLDataset* pIDataset = NULL;
			int nErr = m_pUiMDLManager->queryDataSet( vendue_dataset, &pIDataset );
			if ( nErr != success_errorcode )
			{
				break;
			}

			//从数据集中按指定id查询数据
			UIMDLDatasetRecord& rRecord = pIDataset->findDataRecord( _vendueFindItemFromMDL, &itemId );
			SEARCH_DB_RETDATA* pSearchData = (SEARCH_DB_RETDATA*)rRecord.pRecordData;
			if(pSearchData == NULL)
			{
				break;
			}
			
			TItemtransfersData itemInfo;
			ZeroMemory(&itemInfo, sizeof(itemInfo));
			memcpy(&itemInfo, &pSearchData->itemData, sizeof(pSearchData->itemData));

			equipPos = g_pCoreShell->GetGameData(GDI_VENDUE_EQUIP_POS, NULL, (int)&itemInfo);
		}
		break;
	case ShopItem:
		{
			equipPos = g_pCoreShell->GetGameData(GDI_SHOP_EQUIP_POS, NULL, d_lastObject.ids[0]);
		}
		break;
	default:
		{
			equipPos = itempart_unidentified;
		}
		break;
	}

	
	if(itempart_unidentified != equipPos)
	{
		int itemIndex = equipPos = g_pCoreShell->GetGameData(GDI_EQUIP_INDEX_BY_EQUIP_POS, NULL, equipPos);
		
		if(itemIndex != 0)
		{
			strcpy(d_compareLayoutText, d_headText);
			g_pCoreShell->GetGameData(GDI_EQUIP_COMPARE_TITLE_LAYOUT_DATA, (unsigned int)d_compareLayoutText, NULL);
			g_pCoreShell->GetGameData(GDI_MY_ITEM_LAYOUT_DESC, (unsigned int)d_compareLayoutText, itemIndex);
			strcat(d_compareLayoutText, "</Layout>");	
		}
	}
	
	return d_compareLayoutText;
}

void KUiTipGenerator::addItemPlusPointPrice(TipObject& tipObj)
{
	if(tipObj.type == PlusPointShopItem)
	{
		int shopIdx = g_pCoreShell->GetGameData(GDI_GET_SHOP_IDX, NULL, NULL);
		unsigned long holdMoney = g_pCoreShell->GetGameData(GDI_GET_PLUS_POINT, shopIdx, 0);
		int price = 0;
		ItemPriceLayout data;
		g_pCoreShell->GetGameData(GDI_ITEM_PRICE_LAYOUT_DATA, (unsigned int)&data, NULL);
		
		const char* moneyColor = data.moneyNormalColor;
		price = g_pCoreShell->GetGameData(GDI_SHOP_ITEM_PRICE, NULL, tipObj.ids[0]);
		if(price > holdMoney)
		{
			moneyColor = data.moneyNotEnoughColor;
		}
		
		char tempText[COMMON_CLIENT_MSG_LEN_128];
		
		strcat(d_layoutText, "<Seg text-align=right float=wrap>");
		
		sprintf(tempText, "<Obj type=text %s %s vertical-align=center>%s</Obj>", moneyColor, data.font, data.buyText);
		strcat(d_layoutText, tempText);
		bool showMoney = false;
		int shopIdxTemp = g_pCoreShell->GetGameData(GDI_GET_SHOP_IDX, NULL, NULL);
		int currentShopIndex = g_pCoreShell->GetGameData(GDI_GET_PLUS_POINT_INDEX_BY_SHOP_INDEX, 0, shopIdxTemp);
		static int insteadSpecieIndex = KUiItemBox::getSingleton().GetInsteadSpecieIndex();
		if( currentShopIndex != insteadSpecieIndex )
		{
			if(price> 0)
			{
				PLUS_POINT_PARAM plusPointInfo;
				g_pCoreShell->GetGameData( GDI_GET_PLUS_POINT_TEMPLATE, shopIdx, (int)&plusPointInfo);
				sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d %s</Obj>", moneyColor, data.font, price, plusPointInfo.name.c_str());
				strcat(d_layoutText, tempText);
				showMoney = true;
			}
		}
		else
		{
			string daiBiJing( "set:daibi image:daibi-jin" );
			string daiBiYin( "set:daibi image:daibi-yin" );
			string daiBiTong( "set:daibi image:daibi-tong" );
			if(price> 0)
			{
				if(price / 10000 > 0)
				{
					sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price / 10000);
					strcat(d_layoutText, tempText);
					sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", daiBiJing.c_str());
					strcat(d_layoutText, tempText);
					showMoney = true;
				}
				
				if(price % 10000 / 100 > 0)
				{
					sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price % 10000 / 100);
					strcat(d_layoutText, tempText);
					sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", daiBiYin.c_str() );
					strcat(d_layoutText, tempText);
					showMoney = true;
				}
				
				if(price % 100 > 0 || showMoney == false)
				{
					sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price % 100);
					strcat(d_layoutText, tempText);
					sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", daiBiTong.c_str() );
					strcat(d_layoutText, tempText);
				}
			}	
		}
			
		
		strcat(d_layoutText, "</Seg>");
	}
}

void KUiTipGenerator::addItemPrice(TipObject& tipObj)
{
	if(tipObj.type == MyItem && KUiPlayerState::getSingleton().getState() != KUiPlayerState::IDLE)
	{
		int holdMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
		int price = 0;
		ItemPriceLayout data;
		g_pCoreShell->GetGameData(GDI_ITEM_PRICE_LAYOUT_DATA, (unsigned int)&data, NULL);
		
		
		const char* moneyColor = data.moneyNormalColor;
		const char* headText = data.repairText;
		if(KUiPlayerState::getSingleton().getState() == KUiPlayerState::TRADE_NPC_BUY_SALE)
		{
			price = g_pCoreShell->GetGameData(GDI_SHOP_ITEM_SELL_PRICE, NULL, tipObj.ids[0]);
			if(price != -1)
			{
				price *= tipObj.count;
			}
			headText = data.sellText;
		}
		else if(KUiPlayerState::getSingleton().getState() == KUiPlayerState::TRADE_NPC_NORMAL_REPAIR)
		{
			price = g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE, (UINT)false, tipObj.ids[0]);
			if(price > holdMoney)
			{
				moneyColor = data.moneyNotEnoughColor;
			}
		}
		else if(KUiPlayerState::getSingleton().getState() == KUiPlayerState::TRADE_NPC_SPECIAL_REPAIR)
		{
			price = g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE, (UINT)true, tipObj.ids[0]);
			if(price > holdMoney)
			{
				moneyColor = data.moneyNotEnoughColor;
			}
		}

		//-1表示不可修理或不可出售
		if(-1 == price)
		{
			return;
		}
		
		char tempText[COMMON_CLIENT_MSG_LEN_128];
		
		strcat(d_layoutText, "<Seg text-align=right float=wrap>");
		
		sprintf(tempText, "<Obj type=text %s %s vertical-align=center>%s</Obj>", moneyColor, data.font, headText);
		strcat(d_layoutText, tempText);
		bool showMoney = false;
		if(price / 10000 > 0)
		{
			sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price / 10000);
			strcat(d_layoutText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", data.jinImage);
			strcat(d_layoutText, tempText);
			showMoney = true;
		}
		
		if(price % 10000 / 100 > 0)
		{
			sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price % 10000 / 100);
			strcat(d_layoutText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", data.yinImage);
			strcat(d_layoutText, tempText);
			showMoney = true;
		}
		
		if(price % 100 > 0 || showMoney == false)
		{
			sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price % 100);
			strcat(d_layoutText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", data.tongImage);
			strcat(d_layoutText, tempText);
		}
		strcat(d_layoutText, "</Seg>");
	}

	if(tipObj.type == ShopItem)
	{
		int holdMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
		int price = 0;
		ItemPriceLayout data;
		g_pCoreShell->GetGameData(GDI_ITEM_PRICE_LAYOUT_DATA, (unsigned int)&data, NULL);
		
		const char* moneyColor = data.moneyNormalColor;
		price = g_pCoreShell->GetGameData(GDI_SHOP_ITEM_PRICE, NULL, tipObj.ids[0]);
		if(price > holdMoney)
		{
			moneyColor = data.moneyNotEnoughColor;
		}
		
		char tempText[COMMON_CLIENT_MSG_LEN_128];
		
		strcat(d_layoutText, "<Seg text-align=right float=wrap>");
		
		sprintf(tempText, "<Obj type=text %s %s vertical-align=center>%s</Obj>", moneyColor, data.font, data.buyText);
		strcat(d_layoutText, tempText);
		bool showMoney = false;
		if(price / 10000 > 0)
		{
			sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price / 10000);
			strcat(d_layoutText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", data.jinImage);
			strcat(d_layoutText, tempText);
			showMoney = true;
		}
		
		if(price % 10000 / 100 > 0)
		{
			sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price % 10000 / 100);
			strcat(d_layoutText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", data.yinImage);
			strcat(d_layoutText, tempText);
			showMoney = true;
		}
		
		if(price % 100 > 0 || showMoney == false)
		{
			sprintf(tempText, "<Obj type=text %s %s vertical-align=center> %d</Obj>", moneyColor, data.font, price % 100);
			strcat(d_layoutText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", data.tongImage);
			strcat(d_layoutText, tempText);
		}
		
		strcat(d_layoutText, "</Seg>");
	}
}