//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:4:17   17:39
//      File_base        : IBShopComDef
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
#ifndef _IBShopComDef_h
#define _IBShopComDef_h

#include "cfs_fs2_savedef.h"
#include "cfs_db_interface.h"

class KItem;

#define		INVALID_SHOP_VERSION		0
#define		INVALID_PANEL_VERSION		0
#define		INVALID_STYLE_VERSION		0
#define		INVALID_SHELF_VERSION		0

#define		MAX_SHELF_NAME_LEN			16
#define		MAX_PANEL_TITLE_NAME_LEN	128
#define		MAX_CONTENT_TEXT_LEN		512

#define		MAX_PANELCOUNT_PERSHELF		5
#define		MAX_GOODSCOUNT_PERSHELF		64

#define		MAX_SHELF_NUM				10
#define		MAX_PANEL_NUM				30
#define		MAX_CONTENT_STYLE_NUM		10

#define		SHELF_ATTR_NUM				2
#define		PANEL_ATTR_NUM				11
#define		CONTENTSTYLE_ATTR_NUM		2
#define		ITEM_ATTR_NUM				12

#define		MAX_IBITEM_BUY_ONCE			100

#define		TRADE_ID_RETURN_CREDIT		-1
#define     TRADE_ID_INSURANCE          -2

#define		MAX_BUY_COUNT				99

enum
{
	ibshop_data_shelf = 1,
	ibshop_data_panel,
	ibshop_data_contentstyle,
	ibshop_data_item,
};

enum enIBShopUpdateType
{
	ibshop_update_shelf = 1,
	ibshop_update_panel,
	ibshop_update_contentstyle,
	ibshop_update_item,	
};

enum
{
	enIB_SHOP,
	enCREDIT_SHOP,
	enPRESENT_SHOP,
	enSHOP_NUM,
};

// 对应KPlayer.h中的 enumCreditState
enum 
{
	enCreditState_Disable,
	enCreditState_Good,
	enCreditState_Bad,
};

struct PlayerCreditInfo 
{
	unsigned char	uCreditState;
	DWORD			uReturnTime;
	int			uMinCredit[enSHOP_NUM];
	int			uMaxCredit[enSHOP_NUM];
};

struct _IBShopHeader : _DBProcHeader 
{
	int nIBShopID;
	int nShopIdx;
	int nIBShopShelfIdx;
};

enum enIB_CS_SubProtocol
{
	enIB_CSProt_Begin = -1,

	enIB_CSProt_LoadShelf,
	enIB_CSProt_LoadPanel,
	enIB_CSProt_LoadContentStyle,
	enIB_CSProt_LoadGoodsInShelf,
	enIB_CSProt_BuyGoods,
	enIB_CSProt_ErrCode,
	enIB_CSProt_UpdateIBShop,
	enIB_CSProt_Chongzhi,
	enIB_CSProt_End,
};

enum enIB_Shelf_Category
{
	enIB_ShelfCate_Begin = -1,

	enIB_ShelfCate_FirstPage,
	enIB_ShelfCate_SymbolPaper,
	enIB_ShelfCate_GodStone,
	enIB_ShelfCate_Activity,
	enIB_ShelfCate_Medicine,
	enIB_ShelfCate_Discount,
	enIB_ShelfCate_Other,

	enIB_ShelfCate_End,
};

enum enIB_Goods_Status
{
	enIB_Goods_Status_Begin = -1,

	enIB_Goods_Status_Normal,
	enIB_Goods_Status_Hot,
	enIB_Goods_Status_Recommend,
	enIB_Goods_Status_Discount,

	enIB_Goods_Status_End,
};

typedef struct _IBGoods_Id
{
	int	ID;
	int	Genera;
	int	Particular;
	int	Detail;
	int	Level;
	
} IBGoods_Id;

typedef struct _Client_Buy_Goods 
{
	IBGoods_Id goods;
	int		price;
	BYTE	number;
	BYTE	shopIdx;
	BYTE	shelfIdx;
	bool	useTicket;
	bool	bOnecItem;
} ClientBuyGoods;

typedef struct _IBGoods_NumericInfo
{
	int	Price;
	int Discount;
	int Status;
	int Label;

} IBGoods_NumericInfo;

typedef struct _IBGoods
{
	BYTE	PanelIndex;
	BYTE	IndexInPanel;
	BYTE	Style;
	IBGoods_Id			Id;
	IBGoods_NumericInfo	Info;

	_IBGoods()
	{}

	_IBGoods(const _IBGoods& rhs)
	{
		memcpy(this, &rhs, sizeof(_IBGoods));
	}
	
	_IBGoods& operator= (const _IBGoods& rhs)
	{
		memcpy(this, &rhs, sizeof(_IBGoods));
		return *this;
	}

} IBGoods;

typedef struct _IBGoods_ListEntry
{
	IBGoods		goods;
	_IBGoods_ListEntry	*pNextEntry;

} IBGoods_ListEntry;

typedef struct _ContentStyle
{
	BYTE StyleID;
	char Content[MAX_CONTENT_TEXT_LEN];

	_ContentStyle()
	{}

	_ContentStyle(const _ContentStyle& rhs)
	{
		memcpy(this, &rhs, sizeof(_ContentStyle));
	}

	_ContentStyle& operator= (const _ContentStyle& rhs)
	{
		memcpy(this, &rhs, sizeof(_ContentStyle));
		return *this;
	}

} ContentStyle;

typedef struct _Panel
{
	BYTE PanelID;
	short PanelLeft;
	short PanelTop;
	WORD PanelWidth;
	WORD PanelHeight;
	WORD PanelMarge;
	WORD CellWidth;
	WORD CellHeight;
	WORD TitleWidth;
	WORD TitleHeight;
	char TitleName[MAX_PANEL_TITLE_NAME_LEN];

	_Panel()
	{}

	_Panel(const _Panel& rhs)
	{
		memcpy(this, &rhs, sizeof(_Panel));
	}

	_Panel& operator= (const _Panel& rhs)
	{
		memcpy(this, &rhs, sizeof(_Panel));
		return *this;
	}

} Panel;

typedef struct _Load_IBShelf_Ret
{
	BYTE		ShelfIdx;
	DWORD		ShelfVersion;
	char		ShelfName[MAX_SHELF_NAME_LEN];
	BYTE		PanelIdxList[MAX_PANELCOUNT_PERSHELF];
	
} Load_IBShelf_Ret;

typedef struct _IBGoodsShelf
{
	short int GoodsCount;
	Load_IBShelf_Ret Shelf;
	IBGoods_ListEntry *pGoodsList;

} IBGoodsShelf;

#endif // #ifndef _IBShopComDef_h