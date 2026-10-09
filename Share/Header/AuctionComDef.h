//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006:12:20   11:37
//      File_base        : AuctionComDef
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
#ifndef _AuctionComDef_h
#define _AuctionComDef_h

#include "GlobalDef.h"
#include "ItemCommonDef.h"
#include "GameDataDef.h"

#define		MAXRECORDS_PER_PAGE			7
#define		DEFAULT_BITUP_PERCENT		5
#define		DEFAULT_ADMINTAX_PERCENT	5
#define		DEFAULT_ADMINTAX			20
#define		INVALID_RECORD_ID			0
#define		AUCTION_MAX_ITEM_TYPE_LEN	64

// Valid time of goods on sale, seconds
#define		ONE_HOUR					3600
#define		AUCTION_VALIDTIME_ONEDAY	24 * ONE_HOUR	// seconds
#define		AUCTION_VALIDTIME_TWODAY	48 * ONE_HOUR
#define		AUCTION_VALIDTIME_THREEDAY	72 * ONE_HOUR

// subprotocol definition
enum enAuctionSubProtocol
{
	enAuction_None = -1,

	enAuction_SearchGoods,
	enAuction_SellGoods,
	enAuction_BuyGoods,
	enAuction_MsgToClient,
	enAuction_Cancel,

	enAuction_Num,
};

enum enAuctionDBOpe
{
	enAucDBOpe_None = -1,

	enAucDBOpe_SearchGoods,
	enAucDBOpe_SellGoods,
	enAucDBOpe_LockGoods,
	enAucDBOpe_UpdatePrice,
	enAucDBOpe_Cancel,

	enAucDBOpe_Num,
};

enum enAucTurnPageOpe
{
	enAucTurnPage_None,
	enAucTurnPage_Next,
	enAucTurnPage_Pre,
	enAucTurnPage_Cur,
};

enum enVendueTime
{
	vendue_time_long = 0,
	vendue_time_middle,
	vendue_time_short,
	vendue_time_count
};

#pragma  pack(push,1)
// common data structure definition
typedef struct _Search_Filter_Cond
{
	BYTE	factionReq;
	BYTE	qualityLabel;
	WORD	levelReqHight;
	WORD	levelReqLow;
	char	itemType[AUCTION_MAX_ITEM_TYPE_LEN];
	char	goodsName[MAXSIZE_ITEMNAME + 1];
	char	sellerName[MAXSIZE_ROLENAME];
	char	buyerName[MAXSIZE_ROLENAME];
		
} SEARCH_FILTER_COND;

typedef struct _Record_Common_Data
{
	DWORD	currentPrice;
	DWORD	onePrice;

	union
	{
		DWORD	totalValidTime;		// seconds, used save new record to db
		DWORD	leftTime;			// seconds, used when search record from db
	};

	static int ElemCount()
	{
		return 3;
	}

} RECORD_COMMON_DATA;

typedef struct _Search_DB_RetData
{
	RECORD_COMMON_DATA		recordComData;
	DWORD					recordId;
	char					sellerName[MAXSIZE_ROLENAME];
	TItemtransfersDataBase	itemData;

	static int ElemCount()
	{
		return	3 + RECORD_COMMON_DATA::ElemCount();
	}

} SEARCH_DB_RETDATA;

typedef struct _Client_SellGoods_Req
{
	RECORD_COMMON_DATA	recordComData;
	int					itemId;

} CLIENT_SELLGOODS_REQ;

typedef struct _Client_BuyGoods_ReqData
{
	DWORD	recordId;
	DWORD	price;	

} CLIENT_BUYGOODS_REQDATA;

typedef struct _Server_SellReq_Cache
{
	DWORD	curPrice;
	DWORD	onePrice;
	TItemtransfersData	itemData;

} SERVER_SELLREQ_CACHE;

// protocol data definition

// search goods
typedef struct _s2db_SearchGoods_Cond
{
	int					startRecordOffset;
	SEARCH_FILTER_COND	filterCond;

} S2DB_SEARCHGOODS_COND;

typedef struct _c2s_SearchGoods_Req
{
	VARLEN_PROTOCOL_HEADER		proHeader;
	S2DB_SEARCHGOODS_COND		s2dbSearchCond;

} C2S_SEARCHGOODS_REQ;

typedef struct _db2s_SearchGoods_Ret
{
	WORD	retCount;
	char	pRetData[1];		// array of SEARCH_DB_RETDATA

	DWORD size() const
	{
		if(0 == retCount)
			return sizeof(_db2s_SearchGoods_Ret);
		else
			return sizeof(retCount) + retCount * sizeof(SEARCH_DB_RETDATA);
	}

	_db2s_SearchGoods_Ret()
	{
		retCount = 0;
	}

	_db2s_SearchGoods_Ret& operator= (const _db2s_SearchGoods_Ret &rhs)
	{
		memcpy(this, &rhs, rhs.size());
		return *this;
	}

private:
	_db2s_SearchGoods_Ret(const _db2s_SearchGoods_Ret &rhs);

} DB2S_SEARCHGOODS_RET;

typedef struct _s2c_SearchGoods_Ret
{
	VARLEN_PROTOCOL_HEADER		proHeader;
	DB2S_SEARCHGOODS_RET		recordData;

	DWORD size()
	{
		return sizeof(proHeader) + recordData.size();
	}

	_s2c_SearchGoods_Ret() {};

private:
	_s2c_SearchGoods_Ret(const _s2c_SearchGoods_Ret &rhs);
	_s2c_SearchGoods_Ret& operator= (const _s2c_SearchGoods_Ret &rhs);

} S2C_SEARCHGOODS_RET;

// buy goods
typedef struct _s2db_UpdatePrice_Req
{
	CLIENT_BUYGOODS_REQDATA	buyGoodsData;
	char	buyerName[MAXSIZE_ROLENAME];
	FSGUID	playerGuid;

} S2DB_UPDATEPRICE_REQ;

typedef struct _s2db_LockGoods_Req
{
	CLIENT_BUYGOODS_REQDATA		buyReqData;
	FSGUID	playerGuid;

} S2DB_LOCKGOODS_REQ;

typedef struct _s2db_UnlockGoods_Req
{
	DWORD	auctionId;
	FSGUID	playerGuid;

} S2DB_UNLOCKGOODS_REQ;

typedef struct _c2s_BuyGoods_Req
{
	VARLEN_PROTOCOL_HEADER		proHeader;
	CLIENT_BUYGOODS_REQDATA		buyReqData;	

} C2S_BUYGOODS_REQ;

typedef struct _s2c_BuyGoods_Ret
{
	VARLEN_PROTOCOL_HEADER		proHeader;

} S2C_BUYGOODS_RET;

// cancel auction
typedef struct _c2s_CancelAuction_Req
{
	VARLEN_PROTOCOL_HEADER	proHeader;
	CLIENT_BUYGOODS_REQDATA	goodsData;

} C2S_CANCELAUCTION_REQ;

typedef struct _s2db_CancelAuction_Req
{
	DWORD	auctionId;
	FSGUID	playerGuid;

} S2DB_CANCELAUCTION_REQ;

// sell goods
typedef struct _s2db_SellGoods_Req
{
	RECORD_COMMON_DATA			recordComData;
	char						itemName[MAXSIZE_ITEMNAME + 1];
	char						sellerName[MAXSIZE_ROLENAME];
	TItemtransfersData			itemData;	
	FSGUID						playerGuid;
	FSGUID						itemGuid;
	int							itemGenre;
	int							itemDetail;
	int							itemParticular;
	int							itemLevel;
	BYTE						factionReq;			// lowest 0-5 bit used for mask
	BYTE						qualityLabel;
	int							playerLvlReq;

} S2DB_SELLGOODS_REQ;

typedef struct _c2s_SellGoods_Req
{
	VARLEN_PROTOCOL_HEADER		proHeader;
	CLIENT_SELLGOODS_REQ		recordData;
	
} C2S_SELLGOODS_REQ;

typedef struct _s2c_SellGoods_Ret
{
	VARLEN_PROTOCOL_HEADER		proHeader;

} S2C_SELLGOODS_RET;

// Message to client

typedef struct _s2c_Auction_Msg
{
	VARLEN_PROTOCOL_HEADER		proHeader;
	BYTE						msgCode;

} S2C_AUCTION_MSG;

enum AuctionComOper
{
	find_oper,
	sale_oper,
	buy_oper,
	cancel_oper,
	prev_oper,
	next_oper,

};

struct AuctionInfo 
{
	int	time[vendue_time_count];
	int	tax[vendue_time_count];
	int bidPercent;
};
#pragma pack (pop)

#endif