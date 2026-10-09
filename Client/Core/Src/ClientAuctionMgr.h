//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006:12:20   11:54
//      File_base        : ClientAuctionMgr
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

#ifndef _ClientAuctionMgr_h
#define _ClientAuctionMgr_h

#include "AuctionComDef.h"
#include "KSimulation.h"

class ClientAuctionMgr : public IProtocolSimulation
{
public:
	ClientAuctionMgr();

	// Inherit from base interface
	virtual void Breathe() {};
	virtual void onCreate(UIMDLEvent& rEvent) {};
	virtual void onRelease(UIMDLEvent& rEvent) {};
	virtual void onChange(UIMDLEvent& rEvent);

public:
	bool	Init();
	void	ProcessProtocol(BYTE *pProtocolData);
	void	GetAuctionBaseInfo(AuctionInfo* pInfo);

private:
	void	SearchReq(const SEARCH_FILTER_COND *pFilterCond, enAucTurnPageOpe enTurnPage);
	void	SellReq(const CLIENT_SELLGOODS_REQ	*pSellData);
	void	BuyReq(const CLIENT_BUYGOODS_REQDATA *pBuyData);
	void	CancelReq(const CLIENT_BUYGOODS_REQDATA *pReq);

	void	OnSearchRet(const DB2S_SEARCHGOODS_RET * retData);
	void	RefreshCurPage();
	void	OnAuctionMsg(int nMsgCode);
	
	bool	CheckPreTime();
private:
	ClientAuctionMgr(const ClientAuctionMgr &rhs);
	ClientAuctionMgr& operator= (const ClientAuctionMgr &rhs);

private:
	int						m_lastRetRecordCnt;
	S2DB_SEARCHGOODS_COND	m_preSearchCond;

	AuctionInfo				m_auctionInfo;
	DWORD					m_preOperationTime;
};

inline ClientAuctionMgr::ClientAuctionMgr()
{
	m_lastRetRecordCnt = 0;
	memset(&m_preSearchCond, 0, sizeof(m_preSearchCond));
	
	static ConfigManager &cfg = ConfigManager::Singleton();
	m_auctionInfo.time[vendue_time_long] = cfg.GetGlobalVariable(global_var_auction_long_time);
	m_auctionInfo.time[vendue_time_middle] = cfg.GetGlobalVariable(global_var_auction_middle_time);
	m_auctionInfo.time[vendue_time_short] = cfg.GetGlobalVariable(global_var_auction_short_time);
	m_auctionInfo.tax[vendue_time_long] = cfg.GetGlobalVariable(global_var_auction_long_time_tax);
	m_auctionInfo.tax[vendue_time_middle] = cfg.GetGlobalVariable(global_var_auction_middle_time_tax);
	m_auctionInfo.tax[vendue_time_short] = cfg.GetGlobalVariable(global_var_auction_short_time_tax);
	m_auctionInfo.bidPercent = cfg.GetGlobalVariable(global_var_auction_bid_percent);
}

inline void ClientAuctionMgr::RefreshCurPage()
{
	// Refresh current page records
	Sleep(300);
	SearchReq(&m_preSearchCond.filterCond, enAucTurnPage_Cur);
}

inline bool	ClientAuctionMgr::CheckPreTime()
{
	DWORD curTime = ::GetTickCount();
	if (curTime - m_preOperationTime < 300)
		return false;
	else
		m_preOperationTime = curTime;

	return true;
}

#endif