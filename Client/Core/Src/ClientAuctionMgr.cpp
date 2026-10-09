//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006:12:20   12:28
//      File_base        : ClientAuctionMgr
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
#include "ClientAuctionMgr.h"

bool ClientAuctionMgr::Init()
{
	IUIMDL	*pMDLInterface = NULL;

	if( success_errorcode != GetMDLPtr(&pMDLInterface) )
		return false;

	IUIMDLDataset	*pDataSet = NULL;

	// create dataset for ui
	if( success_errorcode != pMDLInterface->queryDataSet(vendue_dataset, &pDataSet) )
	{
		if( success_errorcode != pMDLInterface->createDataSet(vendue_dataset) )
			return false;
	}

	// register my event handle to ui dataset
	if( success_errorcode != pMDLInterface->queryDataSet(vendue_operation, &pDataSet) )	
		return false;

	pDataSet->setEventHandle(this);

	return true;
}

void ClientAuctionMgr::onChange(UIMDLEvent& rEvent)
{
	_ASSERT(rEvent.pDataSet);

	if(update_umdl != rEvent.nOperation)
		return;
	
	UIMDLDatasetRecord &record =  rEvent.pDataSet->getDataRecord(rEvent.nRecordIndex);

	switch(record.nIndex)
	{
	case find_oper:
		SearchReq((const SEARCH_FILTER_COND*)record.pRecordData, enAucTurnPage_None);
		break;

	case next_oper:
		SearchReq((const SEARCH_FILTER_COND*)record.pRecordData, enAucTurnPage_Next);
		break;

	case prev_oper:
		SearchReq((const SEARCH_FILTER_COND*)record.pRecordData, enAucTurnPage_Pre);
		break;

	case buy_oper:
		BuyReq((const CLIENT_BUYGOODS_REQDATA*)record.pRecordData);
		break;

	case cancel_oper:
		CancelReq( (CLIENT_BUYGOODS_REQDATA*)record.pRecordData );
		break;

	case sale_oper:
		{
			int nItemIdx = ((CLIENT_SELLGOODS_REQ*)record.pRecordData)->itemId;
			((CLIENT_SELLGOODS_REQ*)record.pRecordData)->itemId = Item[nItemIdx].GetID();
			SellReq((const CLIENT_SELLGOODS_REQ*)record.pRecordData);
		}
		break;

	default:
		break;
	}
}

void ClientAuctionMgr::ProcessProtocol(BYTE *pProtocolData)
{
	PVARLEN_PROTOCOL_HEADER	pProHeader = (PVARLEN_PROTOCOL_HEADER)pProtocolData;
	
	switch(pProHeader->subProtocol)
	{
	case enAuction_SearchGoods:
		{
			S2C_SEARCHGOODS_RET	*pSearchRet = (S2C_SEARCHGOODS_RET*)pProtocolData;
			
			//Decompress begin...................................................................
			unsigned char szBuff[MAXRECORDS_PER_PAGE * 1024];
			unsigned int  nLen = MAXRECORDS_PER_PAGE * 1024;
			int                nOldSize =  pSearchRet->proHeader.len - sizeof(VARLEN_PROTOCOL_HEADER) + PROTOCOL_SIZE;
			unsigned char    * pData =  (unsigned char *)(&pSearchRet->recordData); 

			lzo1x_decompress(
				pData,
				nOldSize,
				(unsigned char *)szBuff,
				&nLen,
		        NULL);
			//............................................................................

			OnSearchRet((const DB2S_SEARCHGOODS_RET *) szBuff);
		}
		break;

	case enAuction_MsgToClient:
		{
			S2C_AUCTION_MSG	*pMsg = (S2C_AUCTION_MSG*)pProtocolData;
			OnAuctionMsg(pMsg->msgCode);
		}
		break;

	case enAuction_BuyGoods:
		{
			RefreshCurPage();
		}
		break;

	case enAuction_SellGoods:
		{
			RefreshCurPage();
		}
		break;

	default:
		break;
	}
}

void ClientAuctionMgr::GetAuctionBaseInfo(AuctionInfo* pInfo)
{
	*pInfo = m_auctionInfo; 
}

void ClientAuctionMgr::SearchReq(const SEARCH_FILTER_COND *pFilterCond, enAucTurnPageOpe enTurnPage)
{
	if (!CheckPreTime())
	{
		OnAuctionMsg(enAccMsgCod_OpeTooFast);
		return;
	}

	if(NULL == pFilterCond)
		return;

	if(enAucTurnPage_None == enTurnPage)
	{
		m_preSearchCond.startRecordOffset = 0;
	}
	else
	{
		// if search cond equal to previous cond and operation is turn page,
		// then change start offset, else reset start offset
		if( !memcmp(&m_preSearchCond.filterCond, pFilterCond, sizeof(SEARCH_FILTER_COND)) )
		{
			if(enAucTurnPage_Pre == enTurnPage)
				m_preSearchCond.startRecordOffset -= (MAXRECORDS_PER_PAGE + m_lastRetRecordCnt);
			else if(enAucTurnPage_Cur == enTurnPage)
				m_preSearchCond.startRecordOffset -= m_lastRetRecordCnt;

			if(m_preSearchCond.startRecordOffset <= 0)
				m_preSearchCond.startRecordOffset = 0;
		}
		else
		{
			m_preSearchCond.startRecordOffset = 0;
		}
	}

	m_preSearchCond.filterCond = *pFilterCond;
		
	C2S_SEARCHGOODS_REQ	c2sReq;
	c2sReq.proHeader.protocol = c2s_auction_family;
	c2sReq.proHeader.subProtocol = enAuction_SearchGoods;
	c2sReq.s2dbSearchCond = m_preSearchCond;
	c2sReq.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;

	SendDataToServer(&c2sReq, c2sReq.proHeader.len + PROTOCOL_SIZE);
}

void ClientAuctionMgr::SellReq(const CLIENT_SELLGOODS_REQ *pSellData)
{
	if (!CheckPreTime())
	{
		OnAuctionMsg(enAccMsgCod_OpeTooFast);
		return;
	}
	
	if(NULL == pSellData)
		return;

	if( !HasItemInEquipment(CLIENT_PLAYER_INDEX, pSellData->itemId) )
		return;

	if( !ItemCanTrade(CLIENT_PLAYER_INDEX, pSellData->itemId) )
	{
		OnAuctionMsg(enAucMsgCode_ItemCantTrade);
		return;
	}

	// check player's money is enough to pay tax
	
	C2S_SELLGOODS_REQ	c2sReq;
	c2sReq.proHeader.protocol = c2s_auction_family;
	c2sReq.proHeader.subProtocol = enAuction_SellGoods;
	c2sReq.recordData = *pSellData;
	c2sReq.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;

	SendDataToServer(&c2sReq, c2sReq.proHeader.len + PROTOCOL_SIZE);
}

void ClientAuctionMgr::BuyReq(const CLIENT_BUYGOODS_REQDATA *pBuyData)
{
	if (!CheckPreTime())
	{
		OnAuctionMsg(enAccMsgCod_OpeTooFast);
		return;
	}

	if(NULL == pBuyData)
		return;

	int nTotalMoney = GetTotalMoney(CLIENT_PLAYER_INDEX);

	if(nTotalMoney < pBuyData->price)
	{
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)g_szAuctionMsg[enAucMsgCode_MoneyNotEnough], 0);
		return;
	}

	C2S_BUYGOODS_REQ	c2sReq;
	c2sReq.proHeader.protocol = c2s_auction_family;
	c2sReq.proHeader.subProtocol = enAuction_BuyGoods;
	c2sReq.buyReqData = *pBuyData;
	c2sReq.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;

	SendDataToServer(&c2sReq, c2sReq.proHeader.len + PROTOCOL_SIZE);
}

void ClientAuctionMgr::OnSearchRet(const DB2S_SEARCHGOODS_RET *retData)
{
	m_preSearchCond.startRecordOffset += retData->retCount;
	m_lastRetRecordCnt = retData->retCount;
	
	IUIMDL			*pMDLInterface = NULL;
	IUIMDLDataset	*pDataSet = NULL;

	if( success_errorcode != GetMDLPtr(&pMDLInterface) )
		return;

	if(NULL == pMDLInterface)
		return;

	if( success_errorcode != pMDLInterface->queryDataSet(vendue_dataset, &pDataSet) )
		return;

	if(NULL == pDataSet)
		return;

	SEARCH_DB_RETDATA	*pSearchData = (SEARCH_DB_RETDATA*)&retData->pRetData;

	if(retData->retCount > 0)
	{
		SEARCH_DB_RETDATA tagSeach;
		ZeroMemory( &tagSeach, sizeof(SEARCH_DB_RETDATA) );

		int nDataSize = pDataSet->getRecordCount();
		if ( nDataSize == 0 )
		{
			for ( int i = 0; i < MAXRECORDS_PER_PAGE; ++i )
			{
				pDataSet->addDataRecord( &tagSeach, sizeof(SEARCH_DB_RETDATA));
			}
		}
		int nIdx = 0;
		for ( nIdx = 0; nIdx < retData->retCount; ++nIdx )
		{
			pDataSet->updateRecord( nIdx, &tagSeach, sizeof(SEARCH_DB_RETDATA));
		}
		for ( nIdx = 0; nIdx < retData->retCount; ++nIdx )
		{
			pDataSet->updateRecord( nIdx, pSearchData + nIdx, sizeof(SEARCH_DB_RETDATA));
		}

	}
	else
	{
		OnAuctionMsg(enAucMsgCode_NoRecord);
	}
}

void ClientAuctionMgr::OnAuctionMsg(int nMsgCode)
{
	if(nMsgCode > enAucMsgCode_Begin && nMsgCode < enAucMsgCode_Num)
	{
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)g_szAuctionMsg[nMsgCode], 0);
	}
}

void ClientAuctionMgr::CancelReq(const CLIENT_BUYGOODS_REQDATA *pReq)
{
	if (!CheckPreTime())
	{
		OnAuctionMsg(enAccMsgCod_OpeTooFast);
		return;
	}
	
	C2S_CANCELAUCTION_REQ	c2sReq;

	c2sReq.proHeader.protocol = c2s_auction_family;
	c2sReq.proHeader.subProtocol = enAuction_Cancel;
	c2sReq.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.goodsData = *pReq;

	SendDataToServer(&c2sReq, c2sReq.proHeader.len + PROTOCOL_SIZE);
}