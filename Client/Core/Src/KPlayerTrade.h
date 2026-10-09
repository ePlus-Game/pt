//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerTrade.h
// Date:	2003.02.17
// Code:	边城浪子
// Desc:	Trade Class
//---------------------------------------------------------------------------

#ifndef KPLAYERTRADE_H
#define KPLAYERTRADE_H

class KTrade
{
public:
	//当玩家出于交易状态中的具体操作状态,TRADE_IDLE表示没有进行交易
	enum TradeState
	{
		TRADE_IDLE,
		TRADE_TRADING,
		TRADE_LOCK,
		TRADE_ENDTRADE,
	};
	//用来表示c2s_tradedecision协议的具体含义
	enum TradeMsgType
	{
		TRADE_MSG_SELF_LOCK,
		TRADE_MSG_SELF_UNLOCK,
		TRADE_MSG_OPPOSITE_LOCK,
		TRADE_MSG_SELF_END_TRADE,
		TRADE_MSG_OPPOSITE_END_TRADE,
		TRADE_MSG_OK,
		TRADE_MSG_CANCEL,
		TRADE_MSG_START_TRADE,
		TRADE_MSG_OPPOSITE_REFUSE,
		TRADE_MSG_OPPOSITE_BUSY,
	};

private:
	TradeState	d_tradeState;
public:
	int			d_oppositePlayerIndex;				// 服务器端记的是 player index 客户端记的是 npc id
	int			m_nTradeState;						// 是否已经点了ok 0 没有 1 点了

#ifndef _SERVER
	int			m_nBackEquipMoney;					// 交易开始时备份物品栏money
	int			m_nBackRepositoryMoney;				// 交易开始时备份储物箱money
	int			m_nTradeDestState;					// 客户端记录对方是否ok
#endif

public:
	KTrade();
	void		Release();

	TradeState	getState();
	void		setState(TradeState newState);
	void		initState();
	bool		isTrading();
	bool		tradeBoxCanMove();
};
#endif
