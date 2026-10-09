//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 03/22/2008 12:14
//      File_base        : IBLog
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _ib_long_h_
#define _ib_long_h_

#include "IMoney.h"

enum IBLogType
{
	jinshanbi_onetime_buy,
	jinshanbi_buy,
	jinshanbi_buy_wait,
	creditpoint_buy,
	point_buy,
	card_buy,

	gm_creditpoint_add,
	gm_point_add,
	creditpoint_add,
	point_add,
	card_add,

	jinshanbi_pay,
	creditpoint_pay,
	point_pay,
	card_pay,

	ib_trade_exchange,
	mail_auction_exchange,

	buy_ok_add_no_delete,
	onetime_use_delete,
	use_delete,
	iblogtype_count,

	recommender_reward_card,

	/*
	gm_buy,
	shop_buy,


	throwaway_delete,
	itemleveup_delete,
	itemmake_delete,
	//*/
};

#define IB_LOG_STRING_LEN 64

class KIBLog
{
private:
	KIBLog() {}
public:
	~KIBLog() {}
public:
	// 获取IB Log实体
	static KIBLog& getSingleton( void );
	//添加（购买）IB物品：
	void AddIBItem( int type, const FSGUID& ibItemGuid, int playerIndex, int itemHashId, int itemLevel, int price );
	//转移（交易）IB物品：
	void TransferIBItem( int type, const FSGUID& ibItemGuid, int playerIndex, int newOwerIndex );
	void TransferIBItem( int type, const FSGUID& ibItemGuid, int newOwerIndex );
	//删除IB物品：
	void DelIBItem( int type, const FSGUID& ibItemGuid, int playerIndex );
	//充值IB货币
	void AddIBMoney( int type,int playerIndex, int price ,const FSGUID * pGUID = NULL); //Guid needed for ticket
private:	
	// 检测IB Log 类型
	bool IsOkIBLogType( int type );
	//记录IB物品行为：
	void LogIbItem( int type, const FSGUID& ibItemGuid, int playerIndex, int newOwerIndex );
	// IB Log类型转换到通用Log类型
	int IBLogToCommonLog( int type );
};

#endif