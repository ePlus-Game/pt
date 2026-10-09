//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:4:18   19:38
//      File_base        : IBCenter_C
//      File_ext         : cpp
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "IBCenter_C.h"
#include "CoreRelated.h"
#include "IBShopProtocol.h"
#include "IBShopUtil.h"
#include "IBShopComDef.h"
#include "KItemGenerator.h"
#include "OnceIBItemMgr.h"

IBCenter_C::PPROTFUNC IBCenter_C::m_ServerProtFunc[enIB_CSProt_End];

IBCenter_C& IBCenter_C::Singleton()
{
	static IBCenter_C	center;
	return center;
}

void IBCenter_C::Init()
{
	m_ContentStyleVersion = INVALID_STYLE_VERSION;
	m_PanelVersion = INVALID_PANEL_VERSION;
	ClearAllShop();
}

void IBCenter_C::ClearAllShop()
{
	for (int nShopIndex = 0; nShopIndex < enSHOP_NUM; ++nShopIndex)
	{
		m_IBShopVersion[nShopIndex] = INVALID_SHOP_VERSION;
		ClearAllGoods(nShopIndex);
	}
}

void IBCenter_C::ClearGoodsInShelf(int nShopIdx, int nShelfIdx)
{
	if ( IBShopUtil::IsShopIndexValid(nShopIdx) && IBShopUtil::IsShelfIdxValid(nShelfIdx) )
	{
		m_GoodsShelf[nShopIdx][nShelfIdx].GoodsCount = 0;
		m_GoodsShelf[nShopIdx][nShelfIdx].Shelf.ShelfVersion = INVALID_SHELF_VERSION;
		IBShopUtil::ClearGoodsList(m_GoodsShelf[nShopIdx][nShelfIdx].pGoodsList);
	}
}

inline void IBCenter_C::ClearAllGoods(int nShopIndex)
{
	if (!IBShopUtil::IsShopIndexValid(nShopIndex))
		return;
	
	for(int nShelf = 0; nShelf < MAX_SHELF_NUM; ++nShelf)
	{
		ClearGoodsInShelf(nShopIndex, nShelf);
	}
}

void IBCenter_C::BindProtocolFunc()
{
	m_ServerProtFunc[enIB_CSProt_LoadShelf]			= &IBCenter_C::OnLoadShelf;
	m_ServerProtFunc[enIB_CSProt_LoadPanel]			= &IBCenter_C::OnLoadPanel;
	m_ServerProtFunc[enIB_CSProt_LoadContentStyle]	= &IBCenter_C::OnLoadContentStyle;
	m_ServerProtFunc[enIB_CSProt_LoadGoodsInShelf]	= &IBCenter_C::OnLoadGoodsInShelf;
	m_ServerProtFunc[enIB_CSProt_ErrCode]			= &IBCenter_C::OnErrCode;
	m_ServerProtFunc[enIB_CSProt_UpdateIBShop]		= &IBCenter_C::OnUpdateIBShop;
}

void IBCenter_C::ProcessServerProtocol(const void *pMsg)
{
	VARLEN_PROTOCOL_HEADER	*pVarHeader = (VARLEN_PROTOCOL_HEADER*)pMsg;

	if( IBShopUtil::IsCSProtValid(pVarHeader->subProtocol) )
	{
		if(m_ServerProtFunc[pVarHeader->subProtocol])
			(this->*m_ServerProtFunc[pVarHeader->subProtocol])(pMsg);
	}
	else
		_ASSERT(false);
}

void IBCenter_C::LoadShelfReq(int nShopIdx)
{
	if (m_IBShopVersion[nShopIdx] != INVALID_SHOP_VERSION &&
		!CheckOperationTime(global_var_ibshop_timeinterval, m_preLoadShopTime[nShopIdx]))
	{
		unsigned int uDataId = GDCNI_IBSHOP_SHELF;
		switch(nShopIdx)
		{
		case enIB_SHOP:
			uDataId = GDCNI_IBSHOP_SHELF;
			break;
		case enCREDIT_SHOP:
			uDataId = GDCNI_CREDITSHOP_SHELF;
			break;
		case enPRESENT_SHOP:
			uDataId = GDCNI_POINTSHOP_SHELF;
		    break;
		default:
		    break;
		}
		CoreDataChanged(uDataId, NULL, NULL);
		return;
	}
	C2S_LOADSHELF c2sReq;
	c2sReq.header.protocol = c2s_IB_family;
	c2sReq.header.subProtocol = enIB_CSProt_LoadShelf;
	c2sReq.nShopIdx = nShopIdx;
	c2sReq.IBShopVersion = m_IBShopVersion[nShopIdx];
	c2sReq.header.len = sizeof(c2sReq) - PROTOCOL_SIZE;

// 	// 积分现在是一个页面，单独请求
// 	if (enPRESENT_SHOP == nShopIdx)
// 	{
//		LoadGoodsInShelfReq(nShopIdx, 0);
// 		return;
// 	}

	SendDataToServer(&c2sReq, c2sReq.header.len + PROTOCOL_SIZE);
}

void IBCenter_C::LoadPanelReq()
{
	if (m_PanelVersion != INVALID_PANEL_VERSION &&
		!CheckOperationTime(global_var_ibshop_panel_timeinterval, m_preLoadPanelTime))
		return;

	C2S_LOADPANEL c2sReq;
	c2sReq.header.protocol = c2s_IB_family;
	c2sReq.header.subProtocol = enIB_CSProt_LoadPanel;
	c2sReq.PanelVersion = m_PanelVersion;
	c2sReq.header.len = sizeof(c2sReq) - PROTOCOL_SIZE;

	SendDataToServer(&c2sReq, c2sReq.header.len + PROTOCOL_SIZE);
}

void IBCenter_C::LoadContentStyleReq()
{
	if (m_ContentStyleVersion != INVALID_STYLE_VERSION &&
		!CheckOperationTime(global_var_ibshop_style_timeinterval, m_preLoadStyleTime))
		return;

	C2S_LOADCONTENTSTYLE c2sReq;
	c2sReq.header.protocol = c2s_IB_family;
	c2sReq.header.subProtocol = enIB_CSProt_LoadContentStyle;
	c2sReq.ContentStyleVersion = m_ContentStyleVersion;
	c2sReq.header.len = sizeof(c2sReq) - PROTOCOL_SIZE;

	SendDataToServer(&c2sReq, c2sReq.header.len + PROTOCOL_SIZE);
}

void IBCenter_C::LoadGoodsInShelfReq(int nShopIdx, int nShelfIdx)
{
	if( IBShopUtil::IsShelfIdxValid(nShelfIdx) && IBShopUtil::IsShopIndexValid(nShopIdx) )
	{
		if (m_GoodsShelf[nShopIdx][nShelfIdx].Shelf.ShelfVersion != INVALID_SHELF_VERSION &&
			!CheckOperationTime(global_var_ibshop_shelf_timeinterval, m_preLoadShelfTime[nShopIdx][nShelfIdx]))
		{
			unsigned int uDataId = GDCNI_IBSHOP_GOODS;
			switch(nShopIdx)
			{
			case enIB_SHOP:
				uDataId = GDCNI_IBSHOP_GOODS;
				break;
			case enCREDIT_SHOP:
				uDataId = GDCNI_CREDITSHOP_GOODS;
				break;
			case enPRESENT_SHOP:
				uDataId = GDCNI_POINTSHOP_GOODS;
				break;
			default:
				break;
			}
			
			CoreDataChanged(uDataId, (unsigned int)m_GoodsShelf[nShopIdx][nShelfIdx].pGoodsList, 
				m_GoodsShelf[nShopIdx][nShelfIdx].GoodsCount);
			return;
		}

		C2S_LOADGOODSINSHELF c2sReq;
		c2sReq.header.protocol = c2s_IB_family;
		c2sReq.header.subProtocol = enIB_CSProt_LoadGoodsInShelf;
		c2sReq.header.len = sizeof(c2sReq) - PROTOCOL_SIZE;
		c2sReq.nShopIdx = nShopIdx;
		c2sReq.ShelfIdx = (BYTE)nShelfIdx;
		c2sReq.ShelfVersion = (DWORD)m_GoodsShelf[nShopIdx][nShelfIdx].Shelf.ShelfVersion;

		SendDataToServer(&c2sReq, c2sReq.header.len + PROTOCOL_SIZE);
	}
}

int IBCenter_C::CheckBuyItem(ClientBuyGoods goods, int& needBagSize, int& lastBuyNum, int& maxItemCount)
{
	// 购买时间
	DWORD curTime = ::GetTickCount();
	if (curTime - m_preBuyReqTime < 1000)
		return enIBShopErr_RequestTooFast;
	else
		m_preBuyReqTime = curTime;

	if (goods.number <= 0 || goods.number > MAX_IBITEM_BUY_ONCE)
		return enIBShopErr_ErrReqBuyNum;
	
	KItem item;
	if(g_ItemGen.Gen_Item( goods.goods.Genera, goods.goods.Detail, goods.goods.Particular, goods.goods.Level, 1, &item ))
		maxItemCount = item.GetMaxItemCount();
	else
		return enIBShopErr_GoodsInvalid;

	if (maxItemCount <= 0)
		return enIBShopErr_GoodsInvalid;

// 	if (item.GetIBUseCount() > 0)
// 		goods.number = 1;
	
	int emptyBagSize = GetClientPlayer().m_ItemList.GetBoxSize(room_equipment, enBoxEmptySize);
	if (emptyBagSize <= 0 && !goods.bOnecItem)
		return enIBShopErr_NoEnoughSpace;
	
	needBagSize = goods.number / maxItemCount;
	lastBuyNum = goods.number % maxItemCount;
	if (lastBuyNum > 0)
		needBagSize++;

	if(needBagSize > emptyBagSize && !goods.bOnecItem)
		return enIBShopErr_NoEnoughSpace;
	
	if (needBagSize > 1)
	{
// 		C2S_BUYGOODS c2sReq;
// 		c2sReq.header.protocol = c2s_IB_family;
// 		c2sReq.header.subProtocol = enIB_CSProt_BuyGoods;
// 		c2sReq.ShopIdx = goods.shopIdx;
// 		c2sReq.ShopVersion = m_IBShopVersion[goods.shopIdx];
// 		c2sReq.Goods = goods.goods;
// 		c2sReq.ShelfIdx = (BYTE)goods.shelfIdx;
// 		c2sReq.ShelfVersion = m_GoodsShelf[goods.shopIdx][goods.shelfIdx].Shelf.ShelfVersion;
// 		c2sReq.Price = goods.price;
// 		c2sReq.ReqNum = goods.number;
// 		c2sReq.useTicket = goods.useTicket;
// 		c2sReq.header.len = sizeof(c2sReq) - PROTOCOL_SIZE;
// 		c2sReq.bOnecItem =goods.bOnecItem;
// 
// 		SendDataToServer(&c2sReq, c2sReq.header.len + PROTOCOL_SIZE);
		lastBuyNum = goods.number;
		return enIBShopErr_None/*enIBShopErr_ErrReqBuyNum*/;
	}

	if (needBagSize == 1 && lastBuyNum == 0)
		lastBuyNum = maxItemCount;

	return enIBShopErr_None;
}

//void IBCenter_C::BuyGoodsReq(IBGoods_Id Goods, int nShopIdx, int nShelf)
int IBCenter_C::BuyGoodsReq(ClientBuyGoods goods)
{
	int needBagSize, lastBuyNum, maxItemCount;
	int err = CheckBuyItem(goods, needBagSize, lastBuyNum, maxItemCount);
	if (err != enIBShopErr_None)
	{
		const char *szErrMsg = IBShopUtil::GetErrMsg(err);
		if ( NULL != szErrMsg )
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)szErrMsg, 0);
		return err;
	}

	/*if (needBagSize > 1)
	{
		// 限时暂时不能买多组物品
		for (int i = 0; i < (needBagSize-1); ++i)
		{
			C2S_BUYGOODS c2sReq;
			c2sReq.header.protocol = c2s_IB_family;
			c2sReq.header.subProtocol = enIB_CSProt_BuyGoods;
			c2sReq.ShopIdx = goods.shopIdx;
			c2sReq.ShopVersion = m_IBShopVersion[goods.shopIdx];
			c2sReq.Goods = goods.goods;
			c2sReq.ShelfIdx = (BYTE)goods.shelfIdx;
			c2sReq.ShelfVersion = m_GoodsShelf[goods.shopIdx][goods.shelfIdx].Shelf.ShelfVersion;
			c2sReq.Price = goods.price;
			c2sReq.ReqNum = maxItemCount;
			c2sReq.useTicket = goods.useTicket;
			c2sReq.header.len = sizeof(c2sReq) - PROTOCOL_SIZE;
			c2sReq.bOnecItem =goods.bOnecItem;

			SendDataToServer(&c2sReq, c2sReq.header.len + PROTOCOL_SIZE);
		}
	}//*/
	
	C2S_BUYGOODS c2sReq;
	c2sReq.header.protocol = c2s_IB_family;
	c2sReq.header.subProtocol = enIB_CSProt_BuyGoods;
	c2sReq.ShopIdx = goods.shopIdx;
	c2sReq.ShopVersion = m_IBShopVersion[goods.shopIdx];
	c2sReq.Goods = goods.goods;
	c2sReq.ShelfIdx = (BYTE)goods.shelfIdx;
	c2sReq.ShelfVersion = m_GoodsShelf[goods.shopIdx][goods.shelfIdx].Shelf.ShelfVersion;
	c2sReq.Price = goods.price;
	c2sReq.ReqNum = lastBuyNum;
	c2sReq.useTicket = goods.useTicket;
	c2sReq.header.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.bOnecItem =goods.bOnecItem;

	SendDataToServer(&c2sReq, c2sReq.header.len + PROTOCOL_SIZE);
	if ( g_pClient )
	{
		g_pClient->FlushData();
	}	

	return enIBShopErr_None;
}

void	IBCenter_C::ChongZhi( void )
{
	if ( Player[CLIENT_PLAYER_INDEX].ChongZhiLeftMoney() )
	{
		C2S_CHONGZHI c2sReq;
		c2sReq.header.protocol = c2s_IB_family;
		c2sReq.header.subProtocol = enIB_CSProt_Chongzhi;
		c2sReq.header.len = sizeof(c2sReq) - PROTOCOL_SIZE;
		SendDataToServer(&c2sReq, c2sReq.header.len + PROTOCOL_SIZE);
	}
}

void IBCenter_C::OnErrCode(const void *pMsg)
{
	S2C_ERRCODE *ps2cErrCode = (S2C_ERRCODE*)pMsg;
	if (  ps2cErrCode->ErrCode == enIBShopErr_OnceItemUseOk )
	{
		ClientBuyGoods retGoods;

		KOnceIBItemMgr::GetSingleten().GetOnceItemParam(local_revive, retGoods);
		int nReviveHashId = GenerateItemHashId(retGoods.goods.Genera, retGoods.goods.Detail, retGoods.goods.Particular);

		KOnceIBItemMgr::GetSingleten().GetOnceItemParam(return_money, retGoods);
		int nReturnHashId = GenerateItemHashId(retGoods.goods.Genera, retGoods.goods.Detail, retGoods.goods.Particular);
		
		if ( nReviveHashId  == ps2cErrCode->PlusCode )
		{
			CoreDataChanged(GDCNI_DEATH_CLOSE, 0, 0);
		}
		else if ( nReturnHashId == ps2cErrCode->PlusCode )
		{
			CoreDataChanged(GDCNI_RETURN_CREDIT_SUCCESS, 0, 0);
		}
		else
		{
			SpliteHashId( ps2cErrCode->PlusCode, retGoods.goods.Genera, retGoods.goods.Detail, retGoods.goods.Level );
			CoreDataChanged(GDCNI_IBITEM_BUY_SUCCESS, reinterpret_cast<unsigned int>(&retGoods.goods), 0);
		}
	}
	else
	{
		const char *szErrMsg = IBShopUtil::GetErrMsg(ps2cErrCode->ErrCode);
			
		if(NULL != szErrMsg)
		{
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)szErrMsg, 0);
			CoreDataChanged(GDCNI_IBCENTER_ERROR_MESSAGE, (unsigned int)szErrMsg, ps2cErrCode->ErrCode);
		}
	}
}

void IBCenter_C::OnUpdateIBShop(const void *pMsg)
{
	S2C_UPDATEIBSHOP *ps2cUpdate = (S2C_UPDATEIBSHOP*)pMsg;
	switch(ps2cUpdate->enType)
	{
	case ibshop_update_shelf:
		m_IBShopVersion[ps2cUpdate->nShopIdx] = INVALID_SHOP_VERSION;
		break;
	case ibshop_update_panel:
		m_PanelVersion = INVALID_PANEL_VERSION;
		break;
	case ibshop_update_contentstyle:
		m_ContentStyleVersion = INVALID_STYLE_VERSION;
	    break;
	case ibshop_update_item:
		m_GoodsShelf[ps2cUpdate->nShopIdx][ps2cUpdate->nShelfIdx].Shelf.ShelfVersion = INVALID_SHELF_VERSION;
	    break;
	default:
	    break;
	}
}

void IBCenter_C::OnLoadShelf(const void *pMsg)
{
	const int S2C_BUF_SIZE = sizeof(S2C_LOADSHELF) + \
	MAX_SHELF_NUM * sizeof(Load_IBShelf_Ret);
	char buf[S2C_BUF_SIZE];
	S2C_LOADSHELF *ps2cRet = (S2C_LOADSHELF*)buf;
	if (!DecompressProtocol((const BYTE*)pMsg, (BYTE*)buf, sizeof(buf), sizeof(ps2cRet->header)))
		return;
	
	Load_IBShelf_Ret *pRecord = (Load_IBShelf_Ret*)ps2cRet->Shelf;
	int nShopIdx = ps2cRet->nShopIdx;

	if (!IBShopUtil::IsShopIndexValid(nShopIdx) ||
		ps2cRet->IBShopVersion == INVALID_SHOP_VERSION ||
		ps2cRet->ShelfCount <= 0 ||
		m_IBShopVersion[nShopIdx] == ps2cRet->IBShopVersion)
		return;

	m_ShelfCount[nShopIdx] = ps2cRet->ShelfCount;
	m_IBShopVersion[nShopIdx] = ps2cRet->IBShopVersion;

	for (int i = 0; i < m_ShelfCount[nShopIdx]; i++)
	{
		int nShelfIdx = pRecord->ShelfIdx;
		if (!IBShopUtil::IsShelfIdxValid(nShelfIdx))
			return;

		strncpy(m_GoodsShelf[nShopIdx][nShelfIdx].Shelf.ShelfName, pRecord->ShelfName, 
			sizeof(m_GoodsShelf[nShopIdx][nShelfIdx].Shelf.ShelfName));

		for (int nPanelIdx = 0; nPanelIdx < MAX_PANELCOUNT_PERSHELF; nPanelIdx++)
		{
			m_GoodsShelf[nShopIdx][nShelfIdx].Shelf.PanelIdxList[nPanelIdx] = pRecord->PanelIdxList[nPanelIdx];
		}
		pRecord++;
	}

	if ( m_ShelfCount > 0 )
	{
		unsigned int uDataId = GDCNI_IBSHOP_SHELF;
		switch(ps2cRet->nShopIdx)
		{
		case enIB_SHOP:
			uDataId = GDCNI_IBSHOP_SHELF;
			break;
		case enCREDIT_SHOP:
			uDataId = GDCNI_CREDITSHOP_SHELF;
			break;
		case enPRESENT_SHOP:
			uDataId = GDCNI_POINTSHOP_SHELF;
		    break;
		default:
			uDataId = GDCNI_IBSHOP_SHELF;
		    break;
		}
		CoreDataChanged(uDataId, (unsigned int)ps2cRet->Shelf, m_ShelfCount[nShopIdx]);
	}
}

void IBCenter_C::OnLoadPanel(const void *pMsg)
{
	const int S2C_BUF_SIZE = sizeof(S2C_LOADPANEL) + MAX_PANEL_NUM * sizeof(Panel);
	char buf[S2C_BUF_SIZE];
	S2C_LOADPANEL *ps2cRet = (S2C_LOADPANEL*)buf;
	if (!DecompressProtocol((const BYTE*)pMsg, (BYTE*)buf, sizeof(buf), sizeof(ps2cRet->header)))
		return;
	
	Panel *pRecord = (Panel*)ps2cRet->panel;

	if (m_PanelVersion == ps2cRet->PanelVersion)
		return;

	m_PanelCount = ps2cRet->PanelCount;
	m_PanelVersion = ps2cRet->PanelVersion;

	if (m_PanelCount > 0)
	{
		CoreDataChanged(GDCNI_IBSHOP_PANEL, 
			(unsigned int)pRecord, ps2cRet->PanelCount);
	}
}

void IBCenter_C::OnLoadContentStyle(const void *pMsg)
{
	const int S2C_BUF_SIZE = sizeof(S2C_LOADCONTENTSTYLE) + \
		MAX_CONTENT_STYLE_NUM * sizeof(ContentStyle);
	char buf[S2C_BUF_SIZE];	
	S2C_LOADCONTENTSTYLE *ps2cRet = (S2C_LOADCONTENTSTYLE*)buf;
	DecompressProtocol((const BYTE*)pMsg, (BYTE*)buf, sizeof(buf), sizeof(ps2cRet->header));

	ContentStyle *pRecord = (ContentStyle*)ps2cRet->contentStyle;

	if (m_ContentStyleVersion == ps2cRet->ContentStyleVersion)
		return;

	m_ContentStyleCount = ps2cRet->ContentStyleCount;
	m_ContentStyleVersion = ps2cRet->ContentStyleVersion;

	if (m_ContentStyleCount > 0)
	{
		CoreDataChanged(GDCNI_IBSHOP_CONTENTSTYLE, 
			(unsigned int)pRecord, ps2cRet->ContentStyleCount);
	}
}

void IBCenter_C::OnLoadGoodsInShelf(const void *pMsg)
{
	const int S2C_BUF_SIZE = sizeof(S2C_LOADGOODSINSHELF) + \
		MAX_GOODSCOUNT_PERSHELF*sizeof(IBGoods);
	char buf[S2C_BUF_SIZE];	
	S2C_LOADGOODSINSHELF *ps2cRet = (S2C_LOADGOODSINSHELF*)buf;
	if (!DecompressProtocol((const BYTE*)pMsg, (BYTE*)buf, sizeof(buf), sizeof(ps2cRet->header)))
		return;

	IBGoods *pRecord = (IBGoods*)ps2cRet->Goods;
	int nShopIdx = ps2cRet->ShopIdx;
	int nShelfIdx = ps2cRet->ShelfIdx;

	if(!IBShopUtil::IsShelfIdxValid(nShelfIdx) || 
		!IBShopUtil::IsShopIndexValid(nShopIdx) ||
		ps2cRet->ShopVersion == INVALID_SHOP_VERSION ||
		ps2cRet->ShelfVersion == INVALID_SHELF_VERSION)
		return;

	m_IBShopVersion[nShopIdx] = ps2cRet->ShopVersion;
	if (m_GoodsShelf[nShopIdx][nShelfIdx].Shelf.ShelfVersion == ps2cRet->ShelfVersion)
	{
		unsigned int uDataId = GDCNI_IBSHOP_GOODS;
		switch(nShopIdx)
		{
		case enIB_SHOP:
			uDataId = GDCNI_IBSHOP_GOODS;
			break;
		case enCREDIT_SHOP:
			uDataId = GDCNI_CREDITSHOP_GOODS;
			break;
		case enPRESENT_SHOP:
			uDataId = GDCNI_POINTSHOP_GOODS;
			break;
		default:
			break;
		}
		
		CoreDataChanged(uDataId, (unsigned int)m_GoodsShelf[nShopIdx][nShelfIdx].pGoodsList, 
			m_GoodsShelf[nShopIdx][nShelfIdx].GoodsCount);
		return;
	}
	else
	{
		//版本不同同步数据
		ClearGoodsInShelf(nShopIdx, nShelfIdx);

		m_GoodsShelf[nShopIdx][nShelfIdx].GoodsCount = ps2cRet->GoodsCount;
		m_GoodsShelf[nShopIdx][nShelfIdx].Shelf.ShelfVersion = ps2cRet->ShelfVersion;
		
		for(int nGoods = 0; nGoods < m_GoodsShelf[nShopIdx][nShelfIdx].GoodsCount; ++nGoods)
		{
			IBGoods_ListEntry *pGoodsEntry = IBShopUtil::CreateGoodsListEntry(*pRecord);

			if(NULL != pGoodsEntry)
			{
				IBShopUtil::PushToGoodsListHead(pGoodsEntry, m_GoodsShelf[nShopIdx][nShelfIdx].pGoodsList);
			}
			pRecord++;
		}
	}
	
	if ( ps2cRet->GoodsCount > 0 )
	{
		unsigned int uDataId;
		switch(ps2cRet->ShopIdx)
		{
		case enIB_SHOP:
			uDataId = GDCNI_IBSHOP_GOODS;
			break;
		case enCREDIT_SHOP:
			uDataId = GDCNI_CREDITSHOP_GOODS;
			break;
		case enPRESENT_SHOP:
			uDataId = GDCNI_POINTSHOP_GOODS;
		    break;
		default:
		    break;
		}
		CoreDataChanged(uDataId, (unsigned int)m_GoodsShelf[nShopIdx][nShelfIdx].pGoodsList, 
			m_GoodsShelf[nShopIdx][nShelfIdx].GoodsCount);
	}
}

bool IBCenter_C::CheckOperationTime(enumGlobalVariable nOper, DWORD& preOperTime)
{
	static ConfigManager &cfg = ConfigManager::Singleton();
	DWORD dwTimeInterval = cfg.GetGlobalVariable(nOper);
	DWORD dwCurTime = ::GetTickCount();
	
	if(dwCurTime - preOperTime < (dwTimeInterval*1000))
		return false;
	else
		preOperTime = dwCurTime;
	
	return true;
}

