//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:4:19   14:42
//      File_base        : IBShopProtocol
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
#ifndef _IBShopProtocol_h
#define _IBShopProtocol_h

#include "GlobalDef.h"
#include "IBShopComDef.h"

//---------------- client to/from game server protocol structure ---------------------------

#pragma pack(push, 1)

typedef struct _c2s_chongzhi
{
	VARLEN_PROTOCOL_HEADER header;
	
} C2S_CHONGZHI;

typedef struct _c2s_LoadShelf
{
	VARLEN_PROTOCOL_HEADER header;
	BYTE	nShopIdx;	
	DWORD	IBShopVersion;

} C2S_LOADSHELF;

typedef struct _c2s_LoadPanel
{
	VARLEN_PROTOCOL_HEADER header;
	DWORD PanelVersion;

} C2S_LOADPANEL;

typedef struct _c2s_LoadContentStyle
{
	VARLEN_PROTOCOL_HEADER header;
	DWORD ContentStyleVersion;

} C2S_LOADCONTENTSTYLE;

typedef struct _c2s_LoadGoodsInShelf
{
	VARLEN_PROTOCOL_HEADER header;
	BYTE nShopIdx;
	BYTE ShelfIdx;
	DWORD ShelfVersion;

} C2S_LOADGOODSINSHELF;

typedef struct _c2s_BuyGoods
{
	VARLEN_PROTOCOL_HEADER header;
	DWORD		ShopVersion;
	DWORD		ShelfVersion;
	BYTE		ShopIdx;
	BYTE		ShelfIdx;
	WORD		ReqNum;
	IBGoods_Id	Goods;
	bool		useTicket;
	int			Price;
	bool		bOnecItem;

} C2S_BUYGOODS;

typedef struct _s2c_LoadShelf
{
	VARLEN_PROTOCOL_HEADER header;
	DWORD IBShopVersion;
	BYTE nShopIdx;
	BYTE ShelfCount;
	char Shelf[1];

	DWORD size() const
	{
		if (ShelfCount == 0)
			return sizeof(_s2c_LoadShelf);
		else
			return sizeof(_s2c_LoadShelf) + ShelfCount * sizeof(Load_IBShelf_Ret) - 1;
	}

} S2C_LOADSHELF;

typedef struct _s2c_LoadPanel
{
	VARLEN_PROTOCOL_HEADER header;
	DWORD PanelVersion;
	BYTE PanelCount;
	char panel[1];

	DWORD size() const
	{
		if (PanelCount == 0)
			return sizeof(_s2c_LoadPanel);
		else
			return sizeof(_s2c_LoadPanel) + PanelCount * sizeof(Panel) - 1;
	}

} S2C_LOADPANEL;

typedef struct _s2c_LoadContentStyle
{
	VARLEN_PROTOCOL_HEADER header;
	DWORD ContentStyleVersion;
	BYTE ContentStyleCount;
	char contentStyle[1];

	DWORD size() const
	{
		if (ContentStyleCount == 0)
			return sizeof(_s2c_LoadContentStyle);
		else
			return sizeof(_s2c_LoadContentStyle) + ContentStyleCount * sizeof(ContentStyle) - 1;
	}

} S2C_LOADCONTENTSTYLE;

typedef struct _s2c_LoadGoodsInShelf
{
	VARLEN_PROTOCOL_HEADER header;
	BYTE ShopIdx;
	BYTE ShelfIdx;
	DWORD ShopVersion;
	DWORD ShelfVersion;
	short int GoodsCount;
	char Goods[1];

	DWORD size() const
	{
		if ( GoodsCount == 0 )
			return sizeof(_s2c_LoadGoodsInShelf);
		else
			return sizeof(_s2c_LoadGoodsInShelf) + GoodsCount * sizeof(IBGoods) - 1;
	}

} S2C_LOADGOODSINSHELF;

typedef struct _s2c_ErrCode
{
	VARLEN_PROTOCOL_HEADER header;
	int ErrCode;
	int PlusCode;
} S2C_ERRCODE;

typedef struct _s2c_UpdateIBShop
{
	VARLEN_PROTOCOL_HEADER header;
	enIBShopUpdateType	enType;
	BYTE				nShopIdx;
	BYTE				nShelfIdx;

} S2C_UPDATEIBSHOP;

#pragma pack(pop)

#endif