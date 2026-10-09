#include "KCore.h"
#include "KTabFile.h"
#include "KSmithShop.h"
#include "KPlayer.h"

#ifndef _STANDALONE
	#ifndef _SERVER
		#include "CoreShell.h"
	#endif
#endif

KSmithShop& KSmithShop::getSinglton()
{
	static KSmithShop smithShop;
	return smithShop;
}
	
KSmithShop::KSmithShop()
{

}

KSmithShop::~KSmithShop()
{	

}

bool KSmithShop::init()
{
	KTabFile file;
	
	if (!file.Load(SMITH_SHOP_FILE))
		return false;

	int height = file.GetHeight() - 1;
	int width = file.GetWidth() - 1;
	if (height == 0 || width == 0)
		return false;

	int shopId;
	SmithRuleInfo rule;
	for(int i = 0; i < height; ++i)
	{
		file.GetInteger	(i + 2, 1, -1, &shopId);
#ifndef _SERVER
		file.GetString	(i + 2, 2, "default", rule.type1, COMMON_CLIENT_MSG_LEN_32 );
		file.GetString	(i + 2, 3, "default", rule.type2, COMMON_CLIENT_MSG_LEN_32 );
		file.GetString	(i + 2, 4, "default", rule.type3, COMMON_CLIENT_MSG_LEN_32 );
		file.GetString	(i + 2, 5, "default", rule.type4, COMMON_CLIENT_MSG_LEN_32 );
#endif
		file.GetInteger	(i + 2, 6, -1, &rule.ruleId);
		if(!addNewRule(shopId, rule))
		{
			return false;
		}
	}

	return true;
}

#ifdef _SERVER

bool KSmithShop::addNewRule(int shopId, SmithRuleInfo& rule)
{
	if(shopId < 0 || shopId >= CORE_SMITH_SHOP_MAX_SHOP_COUNT)
	{
		return false;
	}

	SmithShop& shop = _shops[shopId];
	if(shop.rules.find(rule.ruleId) != shop.rules.end())
	{
		return false;
	}
	shop.rules[rule.ruleId] = rule;
	return true;
}

bool KSmithShop::isShopHaveTheRule(int shopId, int ruleId)
{
	KSmithShop::SmithShop* shop = getShopByIds(shopId);
	if(!shop)
	{
		return false;
	}

	if(shop->rules.find(ruleId) != shop->rules.end())
	{
		return true;
	}
	return false;
}

#else

bool KSmithShop::addNewRule(int shopId, SmithRuleInfo& rule)
{
	KSmithShop::SmithShop* shop = getShopByIds(shopId);
	if(!shop)
	{
		return false;
	}

	shop->rules.push_back(rule);
	return true;
}

bool KSmithShop::isShopHaveTheRule(int shopId, int ruleId)
{
	KSmithShop::SmithShop* shop = getShopByIds(shopId);
	if(!shop)
	{
		return false;
	}

	for(int ruleIndex = 0; ruleIndex < shop->rules.size(); ++ruleIndex)
	{
		if(shop->rules[ruleIndex].ruleId == ruleId)
			return true;
	}
	
	return false;
}

#endif

KSmithShop::SmithShop* KSmithShop::getShopByIds(int shopId)
{
	if(shopId < 0 || shopId >= CORE_SMITH_SHOP_MAX_SHOP_COUNT)
	{
		return NULL;
	}
	
	return &_shops[shopId];
}

#ifndef _SERVER

void KSmithShop::beginSmith(int shopId)
{
	if(shopId < 0 || shopId >= CORE_SMITH_SHOP_MAX_SHOP_COUNT)
	{
		return;
	}

	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nSmithShopIdx = shopId;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.shopType = ST_Smith;
	CoreDataChanged( GDCNI_OPEN_SMITH_SHOP, (unsigned int)shopId, NULL );
}

#else

void KSmithShop::beginSmith(int playerIndex, int shopId)
{
	if (playerIndex <= 0 || playerIndex > MAX_PLAYER)
	{
		return;
	}

	if(shopId < 0 || shopId >= CORE_SMITH_SHOP_MAX_SHOP_COUNT)
	{
		return;
	}

	Player[playerIndex].m_BuyInfo.shopType = ST_Smith;
	Player[playerIndex].m_BuyInfo.m_nSmithShopIdx = shopId;
	Player[playerIndex].m_BuyInfo.m_SubWorldID = Npc[Player[playerIndex].m_nIndex].m_SubWorldIndex;
	Npc[Player[playerIndex].m_nIndex].GetMpsPos(
		&Player[playerIndex].m_BuyInfo.m_nMpsX,
		&Player[playerIndex].m_BuyInfo.m_nMpsY);

	SALE_BOX_SYNC saleSync;
	saleSync.ProtocolType = s2c_opensalebox;
	saleSync.nShopIndex = shopId;
	saleSync.shopType = ST_Smith;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[playerIndex].m_nNetConnectIdx, &saleSync, sizeof(SALE_BOX_SYNC));
}
#endif