#include "KEngine.h"
#include "KCore.h"
#include "KTabFile.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KItemGenerator.h"
#include "KSubWorldSet.h"
#include "KItem.h"
#include "KBuySell.h"
#include "ConfigManager.h"
#include "CoreUtil.h"
#include "pluspoint.h"

#ifdef _SERVER
#include "player_monitor.h"
#endif


#ifndef _STANDALONE
	#ifndef _SERVER
		#include "CoreShell.h"
        
	#endif
#endif

#define	SHOP_BOX_WIDTH		5
#define	SHOP_BOX_HEIGHT		10

KBuySell	BuySell;

KBuySell::KBuySell()
{
#ifndef _SERVER
	m_pShopRoom = NULL;
#endif
	m_Item = NULL;
	m_SellItem = NULL;
	m_Width = 0;
	m_Height = 0;
	m_MaxItem = 0;
}

KBuySell::~KBuySell()
{
#ifndef _SERVER
	if (m_pShopRoom)
	{
		delete m_pShopRoom;
		m_pShopRoom = NULL;
	}
#endif
	if (m_Item)
	{
		delete [] m_Item;
		m_Item = NULL;
	}
	if (m_SellItem)
	{
		for (int i = 0; i < m_Height; i++)
		{
			if (m_SellItem[i])
			{
				delete m_SellItem[i];
				m_SellItem[i] = NULL;
			}
		}
		delete m_SellItem;
		m_SellItem = NULL;
	}
	m_Width = 0;
	m_Height = 0;
	m_MaxItem = 0;
}

BOOL KBuySell::Init()
{
	KTabFile		GoodsFile;
	KTabFile		BuySellFile;
	
	if (!BuySellFile.Load(BUYSELL_FILE) || !GoodsFile.Load(GOODS_FILE))
		return FALSE;

	int nHeight = GoodsFile.GetHeight() - 1;
	int nWidth = GoodsFile.GetWidth() - 1;
	if (nWidth == 0 || nHeight == 0)
		return FALSE;

	m_Item = (KItem *)new KItem[nHeight];
	if (!m_Item)
		return FALSE;

	int	nItemGenre;
	int	nDetail;
	int	nParticular;
	int	nLevel;
	int	nItemCount = 1;
	int nSeries;
	int nPlusPrice = 0;
	BOOL bBangding = FALSE;

	//EquipGenItemParam GenParam;
	for (int k = 0; k < nHeight; k++)
	{
		GoodsFile.GetInteger(k + 2, 1, -1, &nItemGenre);
		GoodsFile.GetInteger(k + 2, 2, -1, &nDetail);
		GoodsFile.GetInteger(k + 2, 3, -1, &nParticular);
		GoodsFile.GetInteger(k + 2, 4, -1, &nSeries);
		GoodsFile.GetInteger(k + 2, 5, -1, &nLevel);
		GoodsFile.GetInteger(k + 2, 6, 0, &nPlusPrice);
		GoodsFile.GetInteger(k + 2, 7, FALSE, &bBangding);
		char plusPointStr[COMMON_CLIENT_MSG_LEN_256];
		GoodsFile.GetString(k + 2, 8, "", plusPointStr,sizeof(plusPointStr));

		std::map<int,DWORD>::iterator itPrice =  m_ItemPlusPrice.find( k );
		if ( itPrice == m_ItemPlusPrice.end() )
		{
			m_ItemPlusPrice[k] = nPlusPrice;
		}

		std::map<int,BOOL>::iterator itBangding =  m_ItemBangding.find( k );
		if ( itBangding == m_ItemBangding.end() )
		{
			m_ItemBangding[k] = bBangding;
		}

		std::map<int,std::string>::iterator itppLimit = m_ItemPlusPriceLimit.find(k);
		if ( itppLimit == m_ItemPlusPriceLimit.end() )
		{
			m_ItemPlusPriceLimit[k] = plusPointStr;
		}

		int nRet = g_ItemGen.Gen_Item( nItemGenre, nDetail, nParticular, nLevel, nItemCount, &m_Item[k] );
#ifndef _SERVER
		const KBASICPROP_ITEM* pItemTemplate = m_Item[k].GetItemTemplate();
		if ( pItemTemplate )
		{
			m_Item[k].RandomSocket( (char*)pItemTemplate->InlayDesc );
		}
#endif
		m_MaxItem++;
	}
	
	m_Height = BuySellFile.GetHeight() - 1;
	m_Width  = BuySellFile.GetWidth();
	
	if (m_Width == 0 || m_Height == 0)
		return FALSE;

	m_SellItem = (int **)new int*[m_Height];
	if (!m_SellItem)
		return FALSE;

	for (int i = 0; i < m_Height; i++)
	{
		m_SellItem[i] = NULL;
		m_SellItem[i] = (int *)new int[m_Width];
		if (!m_SellItem[i])
			return FALSE;

		int nPlusPointType = -1;
		BuySellFile.GetInteger(i + 2, 0 + 1, -1, &nPlusPointType);

		if (nPlusPointType != -1 && ( nPlusPointType < 0 || nPlusPointType >= MAX_PLUS_POINT_COUNT) )
		{
			_ASSERT(FALSE);
			return FALSE;
		}//endif
		
		std::map<int,int>::iterator itType = m_ItemPlusPointType.find(i);
		if ( itType == m_ItemPlusPointType.end() )
		{
			m_ItemPlusPointType[i] = nPlusPointType;
		}//endif
		else
		{
			_ASSERT(FALSE);
			return FALSE;
		}//end else

		for (int j = 0; j < m_Width; j++)
		{
			BuySellFile.GetInteger(i + 2, j + 2 , -1, &m_SellItem[i][j]);
			if (m_SellItem[i][j] == -1)
			{
				continue;
			}//endif

			_ASSERT(m_SellItem[i][j] > 0);		// 策划是从1开始的
			if (m_SellItem[i][j] > 0)
				m_SellItem[i][j] -= 1;			// 为了策划从1开始填表
		}//end for j

	}

#ifndef _SERVER
	if (!m_pShopRoom)
	{
		m_pShopRoom = new KInventory;
		m_pShopRoom->Init(SHOP_BOX_WIDTH, SHOP_BOX_HEIGHT);
	}
#endif
	return TRUE;
}

KItem* KBuySell::getItem(int shopIndex, int itemIndex)
{
	if(posCheck(shopIndex, itemIndex) == false)
		return NULL;

	int itemListIndex = m_SellItem[shopIndex][itemIndex];
	return &m_Item[itemListIndex];
}

int     KBuySell::GetPlusPointType(int shopIndex )
{
	std::map<int,int>::iterator itType = m_ItemPlusPointType.find(shopIndex);
	if ( itType != m_ItemPlusPointType.end() )
	{
		return itType->second;
	}//endif
	else
	{
		return -1;
	}//end else

}

DWORD	KBuySell::GetItemPlusPoint(int shopIndex, int itemIndex)
{
	if(posCheck(shopIndex, itemIndex) == false)
		return NULL;
	
	int itemListIndex = m_SellItem[shopIndex][itemIndex];

	DWORD nPlusPrice = 0;
	std::map<int,DWORD>::iterator it = m_ItemPlusPrice.find( itemListIndex );
	if ( it != m_ItemPlusPrice.end() )
	{
		nPlusPrice = m_ItemPlusPrice[itemListIndex];
	}

	return nPlusPrice;	
}

void KBuySell::GetItemPlusPointLimit(int shopIndex, int itemIndex, std::string& outString )
{
	if(posCheck(shopIndex, itemIndex) == false)
	{
		outString = "";
	}
	else
	{
		int itemListIndex = m_SellItem[shopIndex][itemIndex];
		
		std::map<int,std::string>::iterator it = m_ItemPlusPriceLimit.find( itemListIndex );
		if ( it != m_ItemPlusPriceLimit.end() )
		{
			outString = m_ItemPlusPriceLimit[itemListIndex];
		}
		else
		{
			outString = "";
		}
	}
}

KItem* KBuySell::GetItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_MaxItem || !m_Item)
		return NULL;

	return &m_Item[nIndex];
}

int KBuySell::GetItemIndex(int nShop, int nIndex)
{
	if (!m_SellItem || nShop < 0 || nShop >= m_Height || nIndex < 0 || nIndex >= m_Width)
		return -1;

	if (!m_SellItem[nShop])
		return -1;

	return m_SellItem[nShop][nIndex];
}


bool KBuySell::posCheck(int shopIndex, int itemIndex)
{
	if(shopIndex < 0 || shopIndex >= m_Height)
		return false;
	
	if (itemIndex < 0 || itemIndex >= m_Width)
		return false;
	
	int itemListIndex = m_SellItem[shopIndex][itemIndex];
	if (itemListIndex < 0 || itemListIndex >= m_MaxItem)
		return false;

	return true;
}

bool KBuySell::canBuy(int playerIndex, int shopIndex,  int itemIndex)
{
	if(playerIndex < 0 && playerIndex >= MAX_PLAYER)
	{
		_ASSERT(0);
		return false;
	}

	KPlayer& player = Player[playerIndex];
	KItemList& itemList = player.GetItemList();

	if(posCheck(shopIndex, itemIndex) == false)
		return false;

	if (shopIndex != player.m_BuyInfo.m_nBuyIdx)
	{
		g_DebugLog("BuySell: %s buy idx error!", Npc[player.GetNpcIndex()].Name);
		return false;
	}
	
	int itemListIndex = m_SellItem[shopIndex][itemIndex];
	KItem& shopItem = m_Item[itemListIndex];

#ifdef _SERVER
	int	nCityTaxRate = 0;
	int	nLordNpcIdx = Npc[player.m_nIndex].GetCurCityLordNpc();

	if(INVALID_WORLDLORDNPC_INDEX != nLordNpcIdx)
	{
		nCityTaxRate = Npc[nLordNpcIdx].m_UnaryAttrMgr[nuai_city_taxrate];
	}

	int nPrice = ComputePrice(shopItem.GetPrice(), nCityTaxRate);
#else
	int nPrice = shopItem.GetPrice();
#endif

	//金钱是否够
	if(itemList.GetEquipmentMoney() < nPrice)
		return false;
	
	//购买限制物品需要检查是否已经拥有该物品的最多实例
	if (shopItem.GetRestrictCount() > 0)
	{
		if (itemList.CountItem(shopItem.GetGenre(),
			shopItem.GetDetailType(),
			shopItem.GetParticular(),
			shopItem.GetLevel()) >= shopItem.GetRestrictCount())
		{
			return false;
		}
	}

	EXTRAINFOPLUS tagExtraPlus;
	tagExtraPlus.nItemGenre			= m_Item[itemListIndex].GetGenre();
	tagExtraPlus.nParticularType	= m_Item[itemListIndex].GetParticular();
	tagExtraPlus.nDetailType		= m_Item[itemListIndex].GetDetailType();
	tagExtraPlus.nMaxItem			= m_Item[itemListIndex].GetMaxItemCount();
	tagExtraPlus.nCurItem			= m_Item[itemListIndex].GetItemCount();
	tagExtraPlus.pCampareItem       = &m_Item[itemListIndex];

	if (!itemList.GetRoom(room_equipment)->itemCanPlace(&tagExtraPlus))
	{
		return false;
	}
	return true;
}

#ifdef _SERVER
bool KBuySell::buy(int playerIndex, int shopIndex, int itemIndex)
{
	if(canBuy(playerIndex, shopIndex, itemIndex) == false)
		return false;

	int itemListIndex = m_SellItem[shopIndex][itemIndex];
	
	int nItemIdx = ItemSet.Add(&m_Item[itemListIndex]);
	if (nItemIdx <= 0)
		return false;

	KItem& item = Item[nItemIdx];
	KPlayer& player = Player[playerIndex];
	
	if (player.m_ItemList.Add(nItemIdx, item_sync_type_buy))
	{
		int nCityTaxRate = 0;
		int nLordNpcIdx = Npc[player.m_nIndex].GetCurCityLordNpc();

		if(INVALID_WORLDLORDNPC_INDEX != nLordNpcIdx)
		{
			nCityTaxRate = Npc[nLordNpcIdx].m_UnaryAttrMgr[nuai_city_taxrate];
		}
		
		int payPrice = ComputePrice(m_Item[itemListIndex].GetPrice(), nCityTaxRate);

		if (player.Pay(payPrice))
		{	
			if(INVALID_WORLDLORDNPC_INDEX != nLordNpcIdx)
			{
				int nTax = ComputeTax(m_Item[itemListIndex].GetPrice(), nCityTaxRate) / 10000; //Notice : 金钱资源存最小单位为金
				if (nTax != 0)
				{
					Npc[nLordNpcIdx].AddUnaryAttr(nuai_lord_res0, nTax);
					Npc[nLordNpcIdx].SetDataChangedFlag(true);
				}//endif

			}//endif

			//统计：购买物品
			ItemTemplateId id;
			item.GetItemTemplateId(id);
			player.GetPlayerStatistic().AddItem(id, 1, item_count_type_buy);

			//日志：购买物品
			bool needLog = (payPrice >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount)) ||
				(item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
			if (needLog)
			{
				LogEventParam buyItemEvent;
				buyItemEvent.event = log_event_buy_item;
				buyItemEvent.param1 = player.GetGUID();
				buyItemEvent.param2 = item.GetGUID();
				item.GetItemTemplateId(buyItemEvent.param3.data, sizeof(buyItemEvent.param3.data) - 1);
				buyItemEvent.param4 = -payPrice;
				g_pLogSystem->Log(buyItemEvent);
			}
			
			if (g_PlayerMonitor.IsNeedRecord(playerIndex, player_action_buy_item))
			{
				RecordPlayerActionParam param;
				param.PlayerIndex = playerIndex;
				param.Action = player_action_buy_item;
				snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_BUY_ITEM, item.GetName());
				g_PlayerMonitor.RecordPlayerAction(param);
			}

			return true;
		}
		else
		{
			Player[playerIndex].m_ItemList.Remove(nItemIdx);
		}
	}
	
	ItemSet.Remove(nItemIdx);
	return false;
}

/*******************************************************************************
参数 nIdx 指游戏里Item数组的编号
*******************************************************************************/
BOOL KBuySell::Sell(int playerIndex, int shopIndex, int itemIndex)
{
	//检查三个参数
	if (playerIndex <= 0 || playerIndex >= MAX_PLAYER)
		return FALSE;

	if (shopIndex < 0 || shopIndex >= m_Height)
		return FALSE;

	if (itemIndex <= 0 || itemIndex >= MAX_ITEM)
		return FALSE;

	Player[playerIndex].tradeServerDoCanceTrade();

	KPlayer& player = Player[playerIndex];
	KItem& sellItem = Item[itemIndex];

	if (sellItem.IsLocked(Player[playerIndex].GetNetConnectIdx()) || !sellItem.CanSell())
		return FALSE;

	
	ItemPos pos;
	if (!player.GetItemList().GetItemPos(itemIndex,&pos))
		return FALSE;

	if (pos.nPlace != pos_equiproom)
		return FALSE;
	
// 	//检查是否为任务道具
// 	if (Item[itemIndex].GetGenre() == item_task && Item[itemIndex].GetLevel() != QK_Bag)
// 		return FALSE;

// 	if ( Item[itemIndex].GetGenre() == item_magicorscript && Item[itemIndex].GetParticular() == 145 )
// 	{
// 		return FALSE;
// 	}
	
	//检查玩家申请买卖的商店是否是当前正在交易的商店
	if (shopIndex != Player[playerIndex].m_BuyInfo.m_nBuyIdx)
	{
		g_DebugLog("BuySell: %s buy idx error!", Npc[Player[playerIndex].m_nIndex].Name);
		return FALSE;
	}

	//玩家售卖给系统价格=Y（填表）。
	//以上收买价格指当前耐久=最高耐久值上限状态下的出售价格。
	//当前耐久≠最高耐久上限时，
	//出售价格=Y*（当前耐久/最高耐久上限）
	int sellPrice = sellItem.GetSellPrice();
	if(sellItem.GetMaxDurability() != 0)
	{
		sellPrice = sellPrice * sellItem.GetDurability() / sellItem.GetMaxDurability();
	}

	int itemCount = sellItem.GetItemCount();
	if(itemCount <= 0)
		itemCount = 1;
	sellPrice *= itemCount;

	player.Earn(sellPrice);
	//删除该物品

	//统计：出售物品
	ItemTemplateId id;
	sellItem.GetItemTemplateId(id);
	player.GetPlayerStatistic().AddItem(id, itemCount, item_count_type_sell);
	
	//日志：出售物品	
	bool needLog = (sellPrice >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount) ||
		(sellItem.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level)));
	if (needLog)
	{
		LogEventParam sellItemEvent;
		sellItemEvent.event = log_event_sell_item;
		sellItemEvent.param1 = player.GetGUID();
		sellItemEvent.param2 = sellItem.GetGUID();
		sellItem.GetItemTemplateId(sellItemEvent.param3.data, sizeof(sellItemEvent.param3.data) - 1);
		sellItemEvent.param4 = sellPrice;
		g_pLogSystem->Log(sellItemEvent);
	}	

	if (g_PlayerMonitor.IsNeedRecord(playerIndex, player_action_sell_item))
	{
		RecordPlayerActionParam param;
		param.PlayerIndex = playerIndex;
		param.Action = player_action_sell_item;
		snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_SELL_ITEM, sellItem.GetName());
		g_PlayerMonitor.RecordPlayerAction(param);
	}
	
	player.GetItemList().Remove(itemIndex);
	ItemSet.Remove(itemIndex);
	return TRUE;
}
#endif

#ifndef _SERVER
void KBuySell::PaintItem(int nIdx, int nX, int nY)
{
	int nShop = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
	if (nShop < 0 || nShop >= m_Height)
		return;
	int nItemIdx = GetItemIndex(nShop, nIdx);

	int x = nX;
	int y = nY;

	KItem* pItem = GetItem(nItemIdx);

// 	if (pItem)
// 	{
// 		pItem->Paint(x, y, 0);
// 	}
}

void KBuySell::OpenSale(int nShop, int shopType)
{
	if (nShop < 0 || nShop >= m_Height)
		return;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx = nShop;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.shopType = shopType;
	CoreDataChanged(GDCNI_NPC_TRADE, shopType, TRUE);
	//自动买东西
	
}

void KBuySell::OpenHireShop()
{
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx = 1;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.shopType = ST_Hire;
	CoreDataChanged(GDCNI_NPC_TRADE, ST_Hire, TRUE);
}

void KBuySell::getPriceLayoutData(ItemPriceLayout* data)
{
	if(NULL == data)
		return;

	ConfigManager& cm = ConfigManager::Singleton();
	data->jinImage = cm.GetConfigurableDisplayStyle(style_money_image, 0);
	data->yinImage =cm.GetConfigurableDisplayStyle(style_money_image, 1);
	data->tongImage = cm.GetConfigurableDisplayStyle(style_money_image, 2);
	data->moneyNormalColor = cm.GetConfigurableDisplayStyle(style_money_text_color, 0);
	data->moneyNotEnoughColor= cm.GetConfigurableDisplayStyle(style_money_text_color, 1);
	data->font = cm.GetConfigurableDisplayStyle(style_money_font);
	data->repairText = cm.GetConfigurableDisplayStyle(style_money_text, 0);
	data->sellText = cm.GetConfigurableDisplayStyle(style_money_text, 1);
	data->buyText = cm.GetConfigurableDisplayStyle(style_money_text, 2);
}
#endif

#ifdef _SERVER
void KBuySell::OpenSale(int nPlayerIdx, int nShop, int shopType)
{
	if (nPlayerIdx <= 0 || nPlayerIdx > MAX_PLAYER)
	{
		return;
	}

	Player[nPlayerIdx].m_BuyInfo.shopType = shopType;
	Player[nPlayerIdx].m_BuyInfo.m_nBuyIdx = nShop;
	Player[nPlayerIdx].m_BuyInfo.m_SubWorldID = Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(
		&Player[nPlayerIdx].m_BuyInfo.m_nMpsX,
		&Player[nPlayerIdx].m_BuyInfo.m_nMpsY);

	int nCityTaxRate = 0;

	int nSubWorldIdx = Npc[Player[nPlayerIdx].m_nIndex].GetSubWorldIndex();
	int nCityLordNpcIdx = Npc[Player[nPlayerIdx].m_nIndex].GetCurCityLordNpc();

	if(INVALID_WORLDLORDNPC_INDEX != nCityLordNpcIdx)
	{
		nCityTaxRate = Npc[nCityLordNpcIdx].m_UnaryAttrMgr[nuai_city_taxrate];
	}

	SALE_BOX_SYNC saleSync;
	saleSync.ProtocolType = s2c_opensalebox;
	saleSync.nShopIndex = nShop;
	saleSync.shopType = shopType;
	saleSync.cityTaxRate = nCityTaxRate;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &saleSync, sizeof(SALE_BOX_SYNC));
}

void KBuySell::OpenHireShop(int playerIndex)
{
	if (!IsValidPlayer(playerIndex))
	{
		return;
	}

	Player[playerIndex].m_BuyInfo.shopType = ST_Hire;
	Player[playerIndex].m_BuyInfo.m_nBuyIdx = 1;
	Player[playerIndex].m_BuyInfo.m_SubWorldID = Npc[Player[playerIndex].m_nIndex].m_SubWorldIndex;
	Npc[Player[playerIndex].m_nIndex].GetMpsPos(
		&Player[playerIndex].m_BuyInfo.m_nMpsX,
		&Player[playerIndex].m_BuyInfo.m_nMpsY);

	SALE_BOX_SYNC openHire;
	openHire.ProtocolType = s2c_opensalebox;
	openHire.nShopIndex = -1;
	openHire.shopType = ST_Hire;
	openHire.cityTaxRate = -1;

	if (g_pServer != NULL)
	{
		g_pServer->PackDataToClient(Player[playerIndex].m_nNetConnectIdx, &openHire, sizeof(SALE_BOX_SYNC));
	}

	Player[playerIndex].GetUIServerState().SetUIState(player_ui_employ,player_ui_state_open);
}

#endif

bool KBuySell::canBuyByPlusPoint(int playerIndex, int shopIndex,  int itemIndex)
{
	if(playerIndex < 0 && playerIndex >= MAX_PLAYER)
	{
		_ASSERT(0);
		return false;
	}
	
	int nPlusPointIdx = GetPlusPointType(shopIndex);
	
	KPlayer& player = Player[playerIndex];
	KItemList& itemList = player.GetItemList();
	
	if(posCheck(shopIndex, itemIndex) == false)
		return false;
	
	if (shopIndex != player.m_BuyInfo.m_nBuyIdx)
	{
		g_DebugLog("BuySell: %s buy idx error!", Npc[player.GetNpcIndex()].Name);
		return false;
	}
	
	int itemListIndex = m_SellItem[shopIndex][itemIndex];
	KItem& shopItem = m_Item[itemListIndex];

	DWORD nPlusPrice = 0;
	std::map<int,DWORD>::iterator it = m_ItemPlusPrice.find( itemListIndex );
	if ( it != m_ItemPlusPrice.end() )
	{
		nPlusPrice = m_ItemPlusPrice[itemListIndex];
	}
	else
	{
		return false;
	}
	
	if ( nPlusPrice == 0 )
	{
		return false;
	}
	
	DWORD nPrice = nPlusPrice;
	
	//功勋值是否够
	if(player.GetPlusPoint( nPlusPointIdx ) < nPrice ) 
		return false;
	
	//购买限制物品需要检查是否已经拥有该物品的最多实例
	if (shopItem.GetRestrictCount() > 0)
	{
		if (itemList.CountItem(shopItem.GetGenre(),
			shopItem.GetDetailType(),
			shopItem.GetParticular(),
			shopItem.GetLevel()) >= shopItem.GetRestrictCount())
		{
			return false;
		}
	}

	//检测积分等级要求是否满足
	if ( !IsPlusPointOk(itemListIndex,playerIndex) )
	{
		return false;
	}
	
	EXTRAINFOPLUS tagExtraPlus;
	tagExtraPlus.nItemGenre			= m_Item[itemListIndex].GetGenre();
	tagExtraPlus.nParticularType	= m_Item[itemListIndex].GetParticular();
	tagExtraPlus.nDetailType		= m_Item[itemListIndex].GetDetailType();
	tagExtraPlus.nMaxItem			= m_Item[itemListIndex].GetMaxItemCount();
	tagExtraPlus.nCurItem			= m_Item[itemListIndex].GetItemCount();
	tagExtraPlus.pCampareItem       = &m_Item[itemListIndex];
	
	if (!itemList.GetRoom(room_equipment)->itemCanPlace(&tagExtraPlus))
	{
		return false;
	}
	return true;
}

bool KBuySell::IsPlusPointOk( int itemListIndex, int nPlayerIdx )
{
	std::map<int,std::string>::iterator itppLimit = m_ItemPlusPriceLimit.find(itemListIndex);
	if ( itppLimit == m_ItemPlusPriceLimit.end() )
	{
		return false;
	}
	
	int ppLimit[PLUS_POINT_LIMIT_COUNT];
	memset(ppLimit,0,sizeof(ppLimit));
	
	if ( m_ItemPlusPriceLimit[itemListIndex].empty() )
	{
		return false;
	}
	
	if ( strncmp(NO_PLUS_POINT_LIMIT,m_ItemPlusPriceLimit[itemListIndex].c_str(), sizeof(NO_PLUS_POINT_LIMIT) - 1 ) == 0 )
	{
		return true;
	}
	
	::sscanf(m_ItemPlusPriceLimit[itemListIndex].c_str(), "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", 
		&(ppLimit[0]), &(ppLimit[1]), &(ppLimit[2]), &(ppLimit[3]), &(ppLimit[4]), &(ppLimit[5]), 
		&(ppLimit[6]), &(ppLimit[7]), &(ppLimit[8]), &(ppLimit[9]),
		&(ppLimit[10]), &(ppLimit[11]), &(ppLimit[12]), &(ppLimit[13]), &(ppLimit[14]), &(ppLimit[15]), 
		&(ppLimit[16]), &(ppLimit[17]), &(ppLimit[18]), &(ppLimit[19]));
	
	if ( !IsValidPlayer(nPlayerIdx) )
	{
		return false;
	}
	
	for (int idx = 0; idx < PLUS_POINT_LIMIT_COUNT; ++idx )
	{
		int npp = Player[nPlayerIdx].GetPlusPoint(idx);
		if (ppLimit[idx] > 0 && npp < ppLimit[idx])
		{
			return false;
		}
	}
	//*/
	
	return true;
}


#ifdef _SERVER
bool KBuySell::buyByPlusPoint(int playerIndex, int shopIndex,  int itemIndex)
{
	int nPlusPointIdx = GetPlusPointType(shopIndex);;

	if(canBuyByPlusPoint(playerIndex, shopIndex, itemIndex) == false)
		return false;
	
	int itemListIndex = m_SellItem[shopIndex][itemIndex];

	DWORD nPlusPrice = 0;
	std::map<int,DWORD>::iterator it = m_ItemPlusPrice.find( itemListIndex );
	if ( it != m_ItemPlusPrice.end() )
	{
		nPlusPrice = m_ItemPlusPrice[itemListIndex];
	}
	else
	{
		return false;
	}
	
	if ( nPlusPrice == 0 )
	{
		return false;
	}
	
	int nItemIdx = ItemSet.Add(&m_Item[itemListIndex]);
	if (nItemIdx <= 0)
		return false;
	
	KItem& item = Item[nItemIdx];
	KPlayer& player = Player[playerIndex];

	std::map<int,BOOL>::iterator itBangding =  m_ItemBangding.find( itemListIndex );
	if ( itBangding != m_ItemBangding.end() )
	{
		if ( m_ItemBangding[itemListIndex] )
		{
			if ( !Item[nItemIdx].IsBind() )
			{
				Item[nItemIdx].SetBind(true);
			}				
		}
	}
	
	if ( item.GetIBItemType() == ib_item_timelimit )
		item.SetIBBuyDate(UNIX_TMIE_STAMP);

	if (player.m_ItemList.Add(nItemIdx, item_sync_type_buy))
	{
		//城市折价率不影响功勋商店
		DWORD payPrice = nPlusPrice;
		
		if (player.DecPlusPoint(nPlusPointIdx, nPlusPrice))
		{	
			//统计：购买物品
			ItemTemplateId id;
			item.GetItemTemplateId(id);
			player.GetPlayerStatistic().AddItem(id, 1, (ItemCountType)(item_count_type_buy_item_by_plus_point_0 + nPlusPointIdx));
			
			//日志：购买物品
			bool needLog = (payPrice >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_plus_point_amount)) ||
				(item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
			if (needLog)
			{
				LogEventParam buyItemEvent;
				buyItemEvent.event = (enumLogEvent)(log_event_player_buy_item_by_plus_point_0 + nPlusPointIdx);
				buyItemEvent.param1 = player.GetGUID();
				buyItemEvent.param2 = item.GetGUID();
				item.GetItemTemplateId(buyItemEvent.param3.data, sizeof(buyItemEvent.param3.data) - 1);
				buyItemEvent.param4 = -payPrice;
				g_pLogSystem->Log(buyItemEvent);
			}
			
			if (g_PlayerMonitor.IsNeedRecord(playerIndex, (enumPlayerAction)(player_buy_item_by_plus_point_0 + nPlusPointIdx)))
			{
				RecordPlayerActionParam param;
				param.PlayerIndex = playerIndex;
				param.Action = (enumPlayerAction)(player_buy_item_by_plus_point_0 + nPlusPointIdx);
				snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_BUY_ITEM, item.GetName());
				g_PlayerMonitor.RecordPlayerAction(param);
			}
			
			return true;
		}
		else
		{
			Player[playerIndex].m_ItemList.Remove(nItemIdx);
		}
	}
	
	ItemSet.Remove(nItemIdx);
	return false;	
}
#endif

