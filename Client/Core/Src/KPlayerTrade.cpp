//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerTrade.cpp
// Date:	2003.02.17
// Code:	±ß³ÇÀË×Ó
// Desc:	Trade Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KPlayerTrade.h"
KTrade::KTrade()
{
	Release();
}

void	KTrade::Release()
{
	d_oppositePlayerIndex			= -1;
	m_nTradeState			= 0;
#ifndef _SERVER
	m_nBackEquipMoney		= 0;
	m_nBackRepositoryMoney	= 0;
	m_nTradeDestState		= 0;
#endif
}

KTrade::TradeState KTrade::getState()
{
	return d_tradeState;
}

void KTrade::setState(TradeState newState)
{
	d_tradeState = newState;
}

void KTrade::initState()
{
	d_oppositePlayerIndex = -1;
	d_tradeState = KTrade::TRADE_IDLE;
}


bool KTrade::isTrading()
{
	return (d_tradeState != KTrade::TRADE_IDLE);
}

bool KTrade::tradeBoxCanMove()
{
	if(KTrade::TRADE_TRADING == d_tradeState)
		return true;
	return false;
}
