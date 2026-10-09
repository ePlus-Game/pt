//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 18:12:2006   14:07
//      File_base        : ServerAuctionMgr
//      File_ext         : h
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

#ifndef _ServerAuctionMgr_h
#define _ServerAuctionMgr_h

#include "AuctionComDef.h"
#include "cfs_db_interface.h"

#define SELL_ITEM_MAX_PRICE 100000000

class ServerAuctionMgr
{
public:
	ServerAuctionMgr();
	void	ProcessProtocol(int nPlayerIdx, BYTE *pProtocolData, int nDataSize);
	void	ProcessDBRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet );
	void	PlayerOffLine(int nPlayerIdx);

private:
	// Process db operation ret
	void	OnSearchRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	OnLockRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	OnSellRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	OnUpdateRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	OnCancelRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet);

	// Process operation req from client
	void	SearchReq(int nPlayerIdx, BYTE *pProcotolData, int nDataSize);
	void	SellReq(int nPlayerIdx, BYTE *pProcotolData, int nDatasize);
	void	BuyReq(int nPlayerIdx, BYTE *pProcotolData, int nDataSize);
	void	CancelReq(int nPlayerIdx, BYTE *pProcotolData, int nDataSize);

	// server request to db
	void	S2DBUnLock(int nPlayerIdx, DWORD dwAuctionId);
	void	S2DBUpdatePrice(int nPlayerIdx);
	void	S2DBCancelAuction(int nPlayerIdx, DWORD dwAuctionId);
	void	S2DBLock(int nPlayerIdx, const CLIENT_BUYGOODS_REQDATA &goodsData);

private:
	// Assistant fuctions
	void	MsgToClient(int nPlayerIdx, int nMsgCode);
	void	NotifyClientRefresh(int nPlayerIdx);

	bool	CheckSellTax(int nPlayerIdx, int curPrice, int validTime, int& tax);

private:
	ServerAuctionMgr(const ServerAuctionMgr &rhs);
	ServerAuctionMgr& operator= (const ServerAuctionMgr &rhs);

private:
	SERVER_SELLREQ_CACHE		m_curSellGoodsData;
	CLIENT_BUYGOODS_REQDATA		m_curBuyGoodsData;
	enAuctionDBOpe				m_curDBOpe;
	enAuctionDBOpe				m_DBOpeAfterLock;

	typedef void (ServerAuctionMgr::*PCLIENTREQPROC)(int nPlayerIdx, BYTE *pReqData, int nDataSize);
	typedef void (ServerAuctionMgr::*PDBRETPROC)(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet);

	static	PCLIENTREQPROC		m_clientReqProc[enAuction_Num];
	static	PDBRETPROC			m_dbOpeRetProc[enAucDBOpe_Num];	

	DWORD						m_dwPrevSearchTime;
	WORD						m_searchTimeInterval;
};

inline void ServerAuctionMgr::ProcessDBRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet )
{
//	_ASSERT(m_curDBOpe > enAucDBOpe_None && m_curDBOpe < enAuction_Num);

	if(m_curDBOpe > enAucDBOpe_None && m_curDBOpe < enAuction_Num && m_dbOpeRetProc[m_curDBOpe])
		(this->*m_dbOpeRetProc[m_curDBOpe])(nDbOpeRst, nPlayerIdx, pRet);
}

inline void ServerAuctionMgr::PlayerOffLine(int nPlayerIdx)
{
	S2DBUnLock(nPlayerIdx, INVALID_RECORD_ID);

	m_curDBOpe = enAucDBOpe_None;
	m_DBOpeAfterLock = enAucDBOpe_None;
}

#endif