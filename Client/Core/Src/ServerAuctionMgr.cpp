//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006:12:20   14:57
//      File_base        : ServerAuctionMgr
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
#include "CoreRelated.h"
#include "ServerAuctionMgr.h"
#include "DBAucDataCenter.h"
#include "CoreUseNameDef.h"
#include "CoreUtil.h"
#include "player_monitor.h"

ServerAuctionMgr::PCLIENTREQPROC	ServerAuctionMgr::m_clientReqProc[enAuction_Num];
ServerAuctionMgr::PDBRETPROC		ServerAuctionMgr::m_dbOpeRetProc[enAucDBOpe_Num];

ServerAuctionMgr::ServerAuctionMgr()
{
	m_curDBOpe = enAucDBOpe_None;
	m_DBOpeAfterLock = enAucDBOpe_None;
	m_dwPrevSearchTime = 0;
	m_searchTimeInterval = ConfigManager::Singleton().GetGlobalVariable(global_var_auction_search_interval);
	
	memset(&m_clientReqProc, 0, sizeof(m_clientReqProc));
	memset(&m_dbOpeRetProc, 0, sizeof(m_dbOpeRetProc));

	m_clientReqProc[enAuction_SearchGoods] = &ServerAuctionMgr::SearchReq;
	m_clientReqProc[enAuction_SellGoods] = &ServerAuctionMgr::SellReq;
	m_clientReqProc[enAuction_BuyGoods] = &ServerAuctionMgr::BuyReq;
	m_clientReqProc[enAuction_Cancel] = &ServerAuctionMgr::CancelReq;

	m_dbOpeRetProc[enAucDBOpe_SearchGoods] = &ServerAuctionMgr::OnSearchRet;
	m_dbOpeRetProc[enAucDBOpe_LockGoods] = &ServerAuctionMgr::OnLockRet;
	m_dbOpeRetProc[enAucDBOpe_SellGoods] = &ServerAuctionMgr::OnSellRet;
	m_dbOpeRetProc[enAucDBOpe_UpdatePrice] = &ServerAuctionMgr::OnUpdateRet;
	m_dbOpeRetProc[enAucDBOpe_Cancel] = &ServerAuctionMgr::OnCancelRet;

}

void ServerAuctionMgr::ProcessProtocol(int nPlayerIdx, BYTE *pProtocolData, int nDataSize)
{
	// Only allow one db operation at the same time
	if(enAucDBOpe_None != m_curDBOpe)
	{
		MsgToClient(nPlayerIdx, enAucMsgCode_OpeOnDoing);
		return;
	}

	PVARLEN_PROTOCOL_HEADER	pProHeader = (PVARLEN_PROTOCOL_HEADER)pProtocolData;
	int						nSubProtocol = pProHeader->subProtocol;	

	_ASSERT(nSubProtocol > enAuction_None && nSubProtocol < enAuction_Num);

	
	if (!IsValidPlayer(nPlayerIdx))
		return;
	
	if ( Player[nPlayerIdx].GetUIServerState().GetUIState( player_ui_auction ) !=  player_ui_state_open)
		return;

	if(nSubProtocol > enAucDBOpe_None && nSubProtocol < enAuction_Num && m_clientReqProc[nSubProtocol])
	{
		(this->*m_clientReqProc[nSubProtocol])(nPlayerIdx, pProtocolData, nDataSize);
	}	
}

// Not move this function to header file, exist dependence problem
void ServerAuctionMgr::SearchReq(int nPlayerIdx, BYTE *pProtocolData, int nDataSize)
{
	DWORD dwCurTime = UNIX_TMIE_STAMP;
	
	if(dwCurTime - m_dwPrevSearchTime < m_searchTimeInterval)
	{
		MsgToClient(nPlayerIdx, enAucMsgCode_OpeOnDoing);
		return;
	}
	else
	{
		m_dwPrevSearchTime = dwCurTime;
	}

	C2S_SEARCHGOODS_REQ	*pSearchReq = (C2S_SEARCHGOODS_REQ*)pProtocolData;
	
	pSearchReq->s2dbSearchCond.filterCond.buyerName[MAXSIZE_ROLENAME-1]		= '\0';
	pSearchReq->s2dbSearchCond.filterCond.sellerName[MAXSIZE_ROLENAME-1]	= '\0';
	pSearchReq->s2dbSearchCond.filterCond.goodsName[MAXSIZE_ITEMNAME]		= '\0';
	pSearchReq->s2dbSearchCond.filterCond.itemType[AUCTION_MAX_ITEM_TYPE_LEN-1] = '\0';

	if (HasSqlKeyWord(pSearchReq->s2dbSearchCond.filterCond.goodsName, sizeof(pSearchReq->s2dbSearchCond.filterCond.goodsName)))
	{
		//避免SQL注入问题
		return;
	}

	DBAucDataCenter	&dbAucData = DBAucDataCenter::Singleton();
	dbAucData.SearchReq(nPlayerIdx, pSearchReq->s2dbSearchCond);

	m_curDBOpe = enAucDBOpe_SearchGoods;
}

void ServerAuctionMgr::SellReq(int nPlayerIdx, BYTE *pProtocolData, int nDataSize)
{
	C2S_SELLGOODS_REQ	*pSellReq = (C2S_SELLGOODS_REQ*)pProtocolData;

	if( !HasItemInEquipment(nPlayerIdx, pSellReq->recordData.itemId) )	
		return;

	if( !ItemCanTrade(nPlayerIdx, pSellReq->recordData.itemId) )
	{
		MsgToClient(nPlayerIdx, enAucMsgCode_ItemCantTrade);
		return;
	}

	int onePrice = (int)pSellReq->recordData.recordComData.onePrice;
	int curPrict = (int)pSellReq->recordData.recordComData.currentPrice;

	if (onePrice >= SELL_ITEM_MAX_PRICE || onePrice <= 0 || curPrict >= SELL_ITEM_MAX_PRICE ||
		curPrict <= 0 )
	{
		MsgToClient(nPlayerIdx, enAucMsgCode_InvalidPrice);
		return;
	}

	if(pSellReq->recordData.recordComData.onePrice < pSellReq->recordData.recordComData.currentPrice)
	{
		MsgToClient(nPlayerIdx, enAucMsgCode_InvalidPrice);
		return;
	}
	
	// check player's money is enough to pay admin tax
	int nTax = 0;
	
	// check time valid
	static ConfigManager& cfg = ConfigManager::Singleton();
	int maxTime = cfg.GetGlobalVariable(global_var_auction_long_time) * ONE_HOUR;
	if (pSellReq->recordData.recordComData.totalValidTime <= 0 ||
		pSellReq->recordData.recordComData.totalValidTime > maxTime)
		pSellReq->recordData.recordComData.totalValidTime = maxTime;

	if ( CheckSellTax(nPlayerIdx, pSellReq->recordData.recordComData.currentPrice,
		pSellReq->recordData.recordComData.totalValidTime, nTax) )
	{
		if (nTax >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
		{
			LogEventParam logParam;
			logParam.event = log_event_auction_pay_money;
			logParam.param1 = Player[nPlayerIdx].GetGUID();
			logParam.param4 = -nTax;
			g_pLogSystem->Log(logParam);
		}

		// pay admin tax
		DecMoney(nPlayerIdx, nTax, true);
	}
	else
		return;
	
	KPlayer& player = Player[nPlayerIdx];
	int itemIndex = player.GetItemList().SearchID(pSellReq->recordData.itemId);
	if (itemIndex > 0 && itemIndex < MAX_ITEM)
	{
		KItem& item = Item[itemIndex];
		
		//统计：拍卖物品
		ItemTemplateId id;
		item.GetItemTemplateId(id);
		player.GetPlayerStatistic().AddItem(id, item.GetItemCount(), item_count_type_auction);

		//日志：拍卖物品
		bool needLog = (item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
		if (needLog)
		{
			LogEventParam auctionItemEvent;
			auctionItemEvent.event = log_event_auction_item;
			auctionItemEvent.param1 = player.GetGUID();
			auctionItemEvent.param2 = item.GetGUID();
			item.GetItemTemplateId(auctionItemEvent.param3.data, sizeof(auctionItemEvent.param3.data) - 1);
			g_pLogSystem->Log(auctionItemEvent);
		}
		
		if (g_PlayerMonitor.IsNeedRecord(nPlayerIdx, player_action_auction_item))
		{
			RecordPlayerActionParam param;
			param.PlayerIndex = nPlayerIdx;
			param.Action = player_action_auction_item;
			snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_AUCTION_ITEM, item.GetName());
			g_PlayerMonitor.RecordPlayerAction(param);
		}
	}

	S2DB_SELLGOODS_REQ	s2dbReq;
	s2dbReq.recordComData = pSellReq->recordData.recordComData;
	GetItemName(nPlayerIdx, s2dbReq.itemName, sizeof(s2dbReq.itemName), pSellReq->recordData.itemId);
	s2dbReq.playerLvlReq = GetItemReqLevel(nPlayerIdx, pSellReq->recordData.itemId);
	s2dbReq.qualityLabel = GetQualityLabel(nPlayerIdx, pSellReq->recordData.itemId);
	strncpy(s2dbReq.sellerName, GetPlayerName(nPlayerIdx), sizeof(s2dbReq.sellerName));
	GetItemTransData(nPlayerIdx, s2dbReq.itemData, pSellReq->recordData.itemId);
	s2dbReq.playerGuid = GetPlayerGuid(nPlayerIdx);
	GetItemGuid(nPlayerIdx, pSellReq->recordData.itemId, s2dbReq.itemGuid);

	s2dbReq.factionReq = 0;
	int nFactionReq[enRoleType_Number * role_skillseries_count] = { 0 };
	Item[itemIndex].GetProfessionRequirement(&nFactionReq[0]);

	for(int nFacIdx = 0; nFacIdx < sizeof(nFactionReq) / sizeof(int); ++nFacIdx)
	{
		if(nFactionReq[nFacIdx] > 0)
			s2dbReq.factionReq |= (1 << nFacIdx);
	}

	s2dbReq.itemGenre		= Item[itemIndex].GetGenre();
	s2dbReq.itemDetail		= Item[itemIndex].GetDetailType();
	s2dbReq.itemParticular	= Item[itemIndex].GetParticular();
	s2dbReq.itemLevel		= Item[itemIndex].GetLevel();

	s2dbReq.sellerName[MAXSIZE_ROLENAME-1]					= '\0';
	s2dbReq.itemName[MAXSIZE_ITEMNAME]						= '\0';
	s2dbReq.playerGuid.data[32]								= '\0';
	s2dbReq.itemGuid.data[32]								= '\0';			
	s2dbReq.itemData.Guid.data[32]							= '\0';
	s2dbReq.itemData.szPlusInfo[COMMON_CLIENT_MSG_LEN_64-1] = '\0';

	// cache info and delete item from player
	m_curDBOpe = enAucDBOpe_SellGoods;
	m_curSellGoodsData.curPrice = pSellReq->recordData.recordComData.currentPrice;
	m_curSellGoodsData.onePrice = pSellReq->recordData.recordComData.onePrice;
	m_curSellGoodsData.itemData = s2dbReq.itemData;
	
	RemoveItem(nPlayerIdx, pSellReq->recordData.itemId);
	
	DBAucDataCenter	&dbAucData = DBAucDataCenter::Singleton();
	dbAucData.SellReq(nPlayerIdx, s2dbReq);
}

void ServerAuctionMgr::BuyReq(int nPlayerIdx, BYTE *pProcotolData, int nDataSize)
{
	C2S_BUYGOODS_REQ *pBuyReq = (C2S_BUYGOODS_REQ*)pProcotolData;

	int price = (int)pBuyReq->buyReqData.price;
	if (price < 0 || price >= SELL_ITEM_MAX_PRICE)
	{
		MsgToClient(nPlayerIdx, enAucMsgCode_InvalidPrice);
		return;
	}

	if( GetTotalMoney(nPlayerIdx) < pBuyReq->buyReqData.price )
	{
		MsgToClient(nPlayerIdx, enAucMsgCode_MoneyNotEnough);
		return;
	}

	m_curBuyGoodsData = pBuyReq->buyReqData;
	m_DBOpeAfterLock = enAucDBOpe_UpdatePrice;
	S2DBLock(nPlayerIdx, m_curBuyGoodsData);
}

void ServerAuctionMgr::CancelReq(int nPlayerIdx, BYTE *pProcotolData, int nDataSize)
{
	C2S_CANCELAUCTION_REQ *pCancelReq = (C2S_CANCELAUCTION_REQ*)pProcotolData;

	m_curBuyGoodsData = pCancelReq->goodsData;
	m_DBOpeAfterLock = enAucDBOpe_Cancel;
	S2DBLock(nPlayerIdx, pCancelReq->goodsData);
}

void ServerAuctionMgr::OnSearchRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet)
{
	m_curDBOpe = enAucDBOpe_None;

	if(!nDbOpeRst)
	{
		MsgToClient(nPlayerIdx, enAucMsgCode_DBOpeFailed);
		return;
	}

	const int			S2C_BUF_SIZE	= MAXRECORDS_PER_PAGE * 1024;
	int					nRecCount = pRet->GetRowCount();
	int					nColCount = pRet->GetColCount();

	_ASSERT(nRecCount * sizeof(SEARCH_DB_RETDATA) + sizeof(S2C_SEARCHGOODS_RET) <= S2C_BUF_SIZE);
	_ASSERT(nColCount == SEARCH_DB_RETDATA::ElemCount());

	if(nRecCount * sizeof(SEARCH_DB_RETDATA) + sizeof(S2C_SEARCHGOODS_RET) > S2C_BUF_SIZE)
		return;

	if(nColCount != SEARCH_DB_RETDATA::ElemCount())
		return;

	char					buf[S2C_BUF_SIZE];
	S2C_SEARCHGOODS_RET		*pS2cRet = (S2C_SEARCHGOODS_RET*)buf;
	SEARCH_DB_RETDATA		*pRecord = (SEARCH_DB_RETDATA*)pS2cRet->recordData.pRetData;

	pS2cRet->proHeader.protocol = s2c_auction_family;
	pS2cRet->proHeader.subProtocol = enAuction_SearchGoods;
	pS2cRet->recordData.retCount = 0;

	for(int nRow = 0; nRow < nRecCount && nRow < MAXRECORDS_PER_PAGE; ++nRow)
	{
		int		nCol = 0;

		pRet->GetData(nRow, nCol++, pRecord->recordId );

		pRet->GetData(nRow, nCol++, pRecord->sellerName, sizeof(pRecord->sellerName) );

		pRet->GetData(nRow, nCol++, pRecord->recordComData.currentPrice ) ;

		pRet->GetData(nRow, nCol++, pRecord->recordComData.onePrice ) ;

		pRet->GetData(nRow, nCol++, pRecord->recordComData.leftTime ) ;

		char* pItemData = NULL;
		int nSize = pRet->GetData(nRow, nCol++, &pItemData );

		TItemtransfersData	transData;
		KItem::GetItemtransfersData( &transData, pItemData, nSize );

		memcpy(&pRecord->itemData, &transData, sizeof(pRecord->itemData));
		//pRet->GetData(nRow, nCol++, (void*)&pRecord->itemData, sizeof(pRecord->itemData) );

		++(pS2cRet->recordData.retCount);
		++pRecord;

		if(pS2cRet->size() >= S2C_BUF_SIZE)
			break;
	}

	pS2cRet->proHeader.len = pS2cRet->size() - PROTOCOL_SIZE;
	
	//Compression begin...................................................................
	unsigned char szBuff[MAXRECORDS_PER_PAGE * 1024];
	unsigned int  nLen = MAXRECORDS_PER_PAGE * 1024;
	int                nOldSize = pS2cRet->recordData.size();
	unsigned char    * pData =  (unsigned char *)(&pS2cRet->recordData); 
	
	lzo1x_1_compress( 
		pData,
		nOldSize,
		szBuff,
		&nLen,
		wrkmem);
	
	if (nLen > MAXRECORDS_PER_PAGE * 1024 - sizeof(VARLEN_PROTOCOL_HEADER) )
		return;
	
	memcpy(pData,szBuff,nLen);          
	pS2cRet->proHeader.len = nLen + sizeof(VARLEN_PROTOCOL_HEADER) - PROTOCOL_SIZE;

	//Compression end.....................................................................

	SendDataToClient(nPlayerIdx, pS2cRet, pS2cRet->proHeader.len + PROTOCOL_SIZE);
}

void ServerAuctionMgr::OnSellRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet )
{
	m_curDBOpe = enAucDBOpe_None;

	if(!nDbOpeRst)
		return;

	S2C_SELLGOODS_RET	s2cRet;
	s2cRet.proHeader.protocol = s2c_auction_family;
	s2cRet.proHeader.subProtocol = enAuction_SellGoods;
	s2cRet.proHeader.len = sizeof(s2cRet) - PROTOCOL_SIZE;

	SendDataToClient(nPlayerIdx, &s2cRet, s2cRet.proHeader.len + PROTOCOL_SIZE);

}

void ServerAuctionMgr::OnLockRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet )
{
	m_curDBOpe = enAucDBOpe_None;
	int nNextDBOpe = m_DBOpeAfterLock;
	m_DBOpeAfterLock = enAucDBOpe_None;

	if(nDbOpeRst)
	{
		if(enAucDBOpe_UpdatePrice == nNextDBOpe)
			S2DBUpdatePrice(nPlayerIdx);
		else if(enAucDBOpe_Cancel == nNextDBOpe)
			S2DBCancelAuction(nPlayerIdx, m_curBuyGoodsData.recordId);
		else
			_ASSERT(false);

		return;
	}

	MsgToClient(nPlayerIdx, enAucMsgCode_GoodsOnLock);
}

void ServerAuctionMgr::OnUpdateRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet )
{
	m_curDBOpe = enAucDBOpe_None;

	if( !nDbOpeRst )
	{
		MsgToClient(nPlayerIdx, enAucMsgCode_DBOpeFailed);
		return;
	}

	int nRowCount = pRet->GetRowCount();
	int nColCount = pRet->GetColCount();

	NotifyClientRefresh(nPlayerIdx);

	int		nColDataSize;
	char	szSellerName[MAXSIZE_ROLENAME];

	nColDataSize = pRet->GetData(0, 0, szSellerName, sizeof(szSellerName));

	if(0 == nColDataSize)
	{
		// Bid succeed, but not buy the goods
		MsgToClient(nPlayerIdx, enAucMsgCode_BidSucceed);
	}
	else
	{
		// buy the goods
		NewMailNotify(nPlayerIdx, 1, CHAT_CHANNEL_NAME_SYSTEM);

		int		nSellerIdx = g_PlayerInfoToIndex.GetIndexByName(szSellerName);

		if(INVALID_PLAYER_INDEX != nSellerIdx)
			NewMailNotify(nSellerIdx, 1, CHAT_CHANNEL_NAME_SYSTEM);
	}

	char	szPreBidPlayerName[MAXSIZE_ROLENAME];
	nColDataSize = pRet->GetData(0, 1, szPreBidPlayerName, sizeof(szPreBidPlayerName));
	
	if(nColDataSize > 0)
	{
		int nPreBidPlayerIdx = g_PlayerInfoToIndex.GetIndexByName(szPreBidPlayerName);

		if(INVALID_PLAYER_INDEX != nPreBidPlayerIdx)
			MsgToClient(nPreBidPlayerIdx, enAucMsgCode_BidExceedByOther);
	}
}

void ServerAuctionMgr::OnCancelRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet )
{
	m_curDBOpe = enAucDBOpe_None;
	int nMsgCodeToClient = enAucMsgCode_DBOpeFailed;

	if(nDbOpeRst)
	{
		NewMailNotify(nPlayerIdx, 1, CHAT_CHANNEL_NAME_SYSTEM);
		nMsgCodeToClient = enAucMsgCode_CancelSucceed;
		NotifyClientRefresh(nPlayerIdx);
	}

	MsgToClient(nPlayerIdx, nMsgCodeToClient);
}

void ServerAuctionMgr::MsgToClient(int nPlayerIdx, int nMsgCode)
{
	S2C_AUCTION_MSG		msg;

	msg.proHeader.protocol = s2c_auction_family;
	msg.proHeader.subProtocol = enAuction_MsgToClient;
	msg.proHeader.len = sizeof(msg) - PROTOCOL_SIZE;
	msg.msgCode = nMsgCode;

	SendDataToClient(nPlayerIdx, &msg, msg.proHeader.len + PROTOCOL_SIZE);
}

void ServerAuctionMgr::S2DBUnLock(int nPlayerIdx, DWORD dwAuctionId)
{
	S2DB_UNLOCKGOODS_REQ	s2dbReq;
	s2dbReq.auctionId = dwAuctionId;
	s2dbReq.playerGuid = GetPlayerGuid(nPlayerIdx);

	DBAucDataCenter::Singleton().UnLockGoodsReq(nPlayerIdx, s2dbReq);
}

void ServerAuctionMgr::S2DBUpdatePrice(int nPlayerIdx)
{
	if( GetTotalMoney(nPlayerIdx) < m_curBuyGoodsData.price )
	{
		MsgToClient(nPlayerIdx, enAucMsgCode_GoodsOnLock);
		S2DBUnLock(nPlayerIdx, m_curBuyGoodsData.recordId);
		
		return;
	}

	m_curDBOpe = enAucDBOpe_UpdatePrice;

	if (m_curBuyGoodsData.price >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
	{
		LogEventParam logParam;
		logParam.event = log_event_auction_bid_pay_money;
		logParam.param1 = Player[nPlayerIdx].GetGUID();
		logParam.param4 = -m_curBuyGoodsData.price;
		g_pLogSystem->Log(logParam);
	}

	DecMoney(nPlayerIdx, m_curBuyGoodsData.price);

	S2DB_UPDATEPRICE_REQ	s2dbReq;
	strncpy(s2dbReq.buyerName, GetPlayerName(nPlayerIdx), sizeof(s2dbReq.buyerName));
	s2dbReq.playerGuid = GetPlayerGuid(nPlayerIdx);
	s2dbReq.buyGoodsData = m_curBuyGoodsData;

	DBAucDataCenter &dbAuc = DBAucDataCenter::Singleton();
	dbAuc.UpdatePrice(nPlayerIdx, s2dbReq);	
}

void ServerAuctionMgr::S2DBLock(int nPlayerIdx, const CLIENT_BUYGOODS_REQDATA &goodsData)
{
	m_curDBOpe = enAucDBOpe_LockGoods;	

	S2DB_LOCKGOODS_REQ s2dbReq;
	s2dbReq.buyReqData = goodsData;
	s2dbReq.playerGuid = GetPlayerGuid(nPlayerIdx);

	DBAucDataCenter &dbAucData = DBAucDataCenter::Singleton();
	dbAucData.LockGoodsReq(nPlayerIdx, s2dbReq);
}

void ServerAuctionMgr::S2DBCancelAuction(int nPlayerIdx, DWORD dwAuctionId)
{
	m_curDBOpe = enAucDBOpe_Cancel;

	S2DB_CANCELAUCTION_REQ s2dbReq;
	s2dbReq.auctionId = dwAuctionId;
	s2dbReq.playerGuid = GetPlayerGuid(nPlayerIdx);

	DBAucDataCenter::Singleton().CancelReq(nPlayerIdx, s2dbReq);
}

void ServerAuctionMgr::NotifyClientRefresh(int nPlayerIdx)
{
	S2C_BUYGOODS_RET	s2cRet;
	s2cRet.proHeader.protocol = s2c_auction_family;
	s2cRet.proHeader.subProtocol = enAuction_BuyGoods;
	s2cRet.proHeader.len = sizeof(s2cRet) - PROTOCOL_SIZE;
	SendDataToClient(nPlayerIdx, &s2cRet, s2cRet.proHeader.len + PROTOCOL_SIZE);	
}

bool ServerAuctionMgr::CheckSellTax(int nPlayerIdx, int curPrice, int validTime, int& tax)
{
	int nTaxPercent = 0;
	int nValidHour = validTime/ONE_HOUR;

	static ConfigManager& cfg = ConfigManager::Singleton();
	
	if (nValidHour >= cfg.GetGlobalVariable(global_var_auction_long_time))
	{
		nTaxPercent = cfg.GetGlobalVariable(global_var_auction_long_time_tax);
	} 
	else if (nValidHour >= cfg.GetGlobalVariable(global_var_auction_middle_time))
	{		
		nTaxPercent = cfg.GetGlobalVariable(global_var_auction_middle_time_tax);
	}
	else
	{
		nTaxPercent = cfg.GetGlobalVariable(global_var_auction_short_time_tax);
	}

	tax = (int)(curPrice*nTaxPercent/100);
	if (tax > GetTotalMoney(nPlayerIdx))
	{
		MsgToClient(nPlayerIdx, enAucMsgCode_MoneyNotEnough);
		return false;
	}

	return true;
}