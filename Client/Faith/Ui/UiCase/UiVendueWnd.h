//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 12/19/2006 17:37
//      File_base        : UiVendueWnd
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 拍卖行界面
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UIUPDATETIP_H
#define UIUPDATETIP_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "TLRadioButton.h"
#include "TLStatic.h"
#include "AuctionComDef.h"
#include <string>
#include "TLEditbox.h"
#include "TLGameObject.h"
#include <map>

using namespace	std;

#define MAX_PAGE_NUM 3
#define MAX_BAR_NUM  7
#define MAX_PROFESSION_NUM 6

enum
{
	vendue_over_time_verylong = 0,
	vendue_over_time_long,
	vendue_over_time_middle,
	vendue_over_time_short,
	vendue_over_time_veryshort,
	vendue_over_time_count
};

class KUiVenduePage : public KUiWnd
{
	struct KItemBar 
	{
		KItemBar()
		{
			pPar = NULL;
			dwItemID = -1;
		}
		KItemBar( const KItemBar& rBar )
		{
			pPar = rBar.pPar;
			dwItemID = rBar.dwItemID;
		}
		CEGUI::Window* pPar;
		DWORD			dwItemID;
	};
	typedef std::vector<KItemBar> _itemlist;
public:
	KUiVenduePage();
	~KUiVenduePage();
public:
	void	initPage( int index );
	void	AddEvent() {};
	void	updateBar( int nIdx, void *pBuff );
	void	clearBarDate();
	void	clearSelectItem();
	bool	selectItem( const CEGUI::EventArgs& args );
	bool	onMouseMove(const CEGUI::EventArgs& e);
	bool	onMouseLeave(const CEGUI::EventArgs& e);
	bool	onMouseEnter(const CEGUI::EventArgs &e );
	bool	onObjectChange(const CEGUI::EventArgs &e );
	bool	showTip(TLGameObject* pGameObject);
	CEGUI::TLStaticText *getItemName( ) const
	{
		return m_pItemName;
	}

	CEGUI::TLStaticText *getItemLevel( ) const
	{
		return m_pItemLevel;
	}

	CEGUI::TLStaticText *getItemOwner( ) const
	{
		return m_pItemOwner;
	}

	CEGUI::TLStaticText *getItemPrice( ) const
	{
		return m_pItemPrice;
	}

	CEGUI::TLStaticText *getItemTime( ) const
	{
		return m_pItemTime;
	}

	int	getCurrentHeightItem() const
	{
		return m_iCurrentHeightItem;
	}

	CEGUI::Size getItemNameSize( void ) const
	{
		return m_ItemNameSize;
	}

	CEGUI::Size getItemLevelSize( void ) const
	{
		return m_ItemLevelSize;
	}

	CEGUI::Size getItemTimeSize( void ) const
	{
		return m_ItemTimeSize;
	}

	CEGUI::Size getItemOwnerSize( void ) const
	{
		return m_ItemOwnerSize;
	}

	CEGUI::Size getItemPriceSize( void ) const
	{
		return m_ItemPriceSize;
	}

	void	CancelCurHeight();

private:
	void	InitItemColumn( int index );
	void    InitItemSelectHandle( CEGUI::Window *pWindow, int iIndex );
	void	InitItemChildHandle( CEGUI::Window *pWindow, int iIndex, const char *pCtlName );

	void	LoadConfig();

	_itemlist	m_itemBarList;
	//likun
	int					m_iCurrentHeightItem;	//当前保持高亮的item索引
	CEGUI::TLStaticText *m_pItemName;
	CEGUI::TLStaticText *m_pItemLevel;
	CEGUI::TLStaticText *m_pItemOwner;
	CEGUI::TLStaticText *m_pItemPrice;
	CEGUI::TLStaticText *m_pItemTime;
	CEGUI::Size			m_ItemNameSize;
	CEGUI::Size			m_ItemLevelSize;
	CEGUI::Size			m_ItemOwnerSize;
	CEGUI::Size			m_ItemPriceSize;
	CEGUI::Size			m_ItemTimeSize;
	bool				m_showCompare;

	int					d_overTime[vendue_over_time_count];		// 单位分钟
};

class KUiVendueWnd : public KUiWndSingleton<KUiVendueWnd>
{
	enum  VENDUE_MESSAGES 
	{
		//各种要显示的提示信息
		NOT_VENDUE = 1,
		CANCLE_SALE,
		BUY_ONE_BY_PRICE,
		BUY_BY_PRICE,
		PRICE_NOT_ZERO,
		YES,
		NO,
		WRONG_CONDITION,
		SELECT_WARNING,
		GIVED_PRICE,
		STORAGE_PRICE_EXPLANE,
		COMPETE_PRICE_EXPLANE,
		STEADY_PRICE_EXPLANE,
		GOODS_NAME,
		GOODS_LEVEL,
		GOODS_OWNER,
		GOODS_PRICE,
		GOODS_TIME,
		EQUIPMENT_COMMON,
		EQUIPMENT_RARE,
		EQUIPMENT_SET,
		EQUIPMENT_EPIC,
		EQUIPMENT_ARTIFICIALONE,
		EQUIPMENT_ARTIFICIALTWO,
		EQUIPMENT_ARTIFICIALTHREE,
		OPERING_NOW,
		//来确定是取消拍卖还是购买物品的两个标志
		IS_CANCLE_SALE,
		IS_BUY_SOMETHING,
		EQUIPMENT_PROF_COMMON=32,
		EQUIPMENT_PROF_XF,
		EQUIPMENT_PROF_XT,
		EQUIPMENT_PROF_TS,
		EQUIPMENT_PROF_ZR,
		EQUIPMENT_PROF_SS,
		EQUIPMENT_PROF_YS,
		WARNING_MESSAGE,
	};
	friend KUiVenduePage;
	typedef std::vector<KUiWnd*> _commoditylist;
public:
	enum _pagetype
	{
		look_page,
		jingpai_page,
		sell_page,
	};

public:
    KUiVendueWnd( const CEGUI::String& id_name );
    ~KUiVendueWnd(								);

public:
	static	void Show( void );
	static  void Hide( void );
	virtual void Init( void );
	bool  isDigitStr( std::string digitStr )
	{
		std::string::iterator  iBegin = digitStr.begin();
		std::string::iterator  iEnd	  = digitStr.end();
		for ( ; iBegin < iEnd; iBegin++ )
		{
			if ( *iBegin < '0' || *iBegin > '9' )
			{
				return false;
			}
		}
		return true;
	}
	void	clearPageDate	( void					);
	inline	_pagetype	getCurPage() { return m_curPage; }

	static  int m_iCurrentBarNum;
private:
	void	setSelectItemID	( DWORD dwItemID		);
	void	onCreate		( UIMDLEvent& rEvent	);
	void	onRelease		( UIMDLEvent& rEvent	);
	void	onChange		( UIMDLEvent& rEvent	);

	void	initVendue		( void					);
	void	registerDataset	( void					);
	//void	clearPageDate	( void					);

	void	setPage			( int nPage				);
	void	updatePage		( int nIdx, void *pBuff );

	bool	searchItem( const CEGUI::EventArgs& args );
	void	search( AuctionComOper searchOper );

	bool	Sale( const CEGUI::EventArgs& args );
	bool	Buy( const CEGUI::EventArgs& args );
	bool	Prev( const CEGUI::EventArgs& args );
	bool	Next( const CEGUI::EventArgs& args );
	bool	handleExit( const CEGUI::EventArgs& args );
	bool	handleShowLook( const CEGUI::EventArgs& args );
	bool	handleShowPaimai( const CEGUI::EventArgs& args );
	bool	handleShowJingbiao( const CEGUI::EventArgs& args );
	bool	onLBDown(const CEGUI::EventArgs& e);
	bool	handleSetTime0(const CEGUI::EventArgs& e);
	bool	handleSetTime1(const CEGUI::EventArgs& e);
	bool	handleSetTime2(const CEGUI::EventArgs& e);
	void	hideAll( void );
	void	ShowLook( void );
	void	ShowPaimai( void );
	void	ShowJingpai( void );
	int		getYikouPrice();
	int		getJingpaiPrice();
	int		getBottomJingpaiPrice();

	void	clearYikouPrice();
	void	clearJingpaiPrice();
	void	clearBottomJingpaiPrice();

	bool	BuyOnce( const CEGUI::EventArgs& args );
	//likun defined
	bool	GetCtrlAndRegist( void );
	void	ShowMoneyValue( DWORD &dSubValue );

	bool	handleUseAble( const CEGUI::EventArgs& e );
	bool	CancleSale( const CEGUI::EventArgs& e );

	void	CopyJingPaiJia( String jing, String yin, String tong );

	void	SearchOperation( AuctionComOper oper, _pagetype pagetype );
	
private:
	void	LoadConfig();

	void    ShowJinPaiPageColumn( void );
	void    ShowSearchOrVenduePageColumn( void );
	void	ShowYiKouPageColumn( void );
	void	ClearAllColumnText( void );
	void	ShowRegComMsgBox( VENDUE_MESSAGES msg, VENDUE_MESSAGES buyOrsale );  //显示确认界面
	static	void	BuyOk( void );	//确定是否要购买界面
	static  void	CancelSale( void );  //确定是否要取消拍卖界面
	void	WarningMsg( void );

	void	UpdateTax( void );
	void	HideAllMenu();
		
//	bool	handleShown( const CEGUI::EventArgs& args );
//	bool	handleHidden( const CEGUI::EventArgs& args );
	bool	handleKeyDown( const CEGUI::EventArgs& args );
	bool	handleEquipQulity( const CEGUI::EventArgs& args );
	bool	handleEquipSelect( const CEGUI::EventArgs& args );
	bool    handleClickVendue( const CEGUI::EventArgs& args );

	bool	handleReq( const CEGUI::EventArgs& args );
	bool	handleRequestKind( const CEGUI::EventArgs& args );		// 物品分类：装备、消耗品、材料、其他

	bool	handleProfession( const CEGUI::EventArgs& args );		// 装备职业类型
	bool	handleProfessionSelect( const CEGUI::EventArgs& args );	// 装备职业类型选择：玄风、刑天、天师、真人、兽使、羿使

	bool	handleEquip( const CEGUI::EventArgs& args );
	bool	handleEquipmentKind( const CEGUI::EventArgs& args );	// 装备分类：武器、衣服、靴子……

	bool	handleUpdateTax( const CEGUI::EventArgs& args );
	
private:
	_commoditylist	m_commodityPageList;
	int				m_nPageCount;
	DWORD			m_curSelectItemID;
	int				m_nSaleTime;
	_pagetype		m_curPage;

	//likun defined
	CLIENT_BUYGOODS_REQDATA	d_buyInfo;  //要买东西的价格
	CLIENT_BUYGOODS_REQDATA d_selfBuy;
	CEGUI::TLRadioButton	*d_pUseAbleChckBtn;
	bool					d_bUseAble;
	CEGUI::Window			*d_pGoldCoin;
	CEGUI::Window			*d_pSillerCoin;
	CEGUI::Window			*d_pCopperCoin;
	CEGUI::TLEditbox		*d_pWeaponLevelLow;
	CEGUI::TLEditbox		*d_pWeaponLevelHeight;
	CEGUI::TLEditbox		*d_pBuyOnceGold;
	CEGUI::TLEditbox		*d_pBuyOnceSlive;
	CEGUI::TLEditbox		*d_pBuyOnceCopper;
	CEGUI::TLEditbox		*d_pBuyGold;
	CEGUI::TLEditbox		*d_pBuySlive;
	CEGUI::TLEditbox		*d_pBuyCopper;
	
	CEGUI::StaticText		*d_pTaxGold;
	CEGUI::StaticText		*d_pTaxSlive;
	CEGUI::StaticText		*d_pTaxCopper;

	CEGUI::TLEditbox		*d_pBottomGold;
	CEGUI::TLEditbox		*d_pBottomSlive;
	CEGUI::TLEditbox		*d_pBottomCopper;
	CEGUI::RadioButton		*d_pSaleTimeLimit0;
	CEGUI::RadioButton		*d_pSaleTimeLimit1;
	CEGUI::RadioButton		*d_pSaleTimeLimit2;
	//
	CEGUI::TLButton			*d_pEquipQulityBtn;
	BYTE					d_iEquipQulity;
	CEGUI::TLStaticImage	*d_pEquipMenu;
	CEGUI::TLButton			*d_pCommonEquipment;
	CEGUI::TLButton			*d_pRareEquipment;
	CEGUI::TLButton			*d_pSetEquipment;
	CEGUI::TLButton			*d_pEpicEquipment;
	CEGUI::TLButton			*d_pArtificialOne;
	//CEGUI::TLButton			*d_pArtificialTwo;
	//CEGUI::TLButton			*d_pArtificialThree;

	// 职业需求
	CEGUI::TLStaticImage	*d_pBrowse;

	CEGUI::TLButton			*d_pReqBtn;
	CEGUI::TLStaticImage	*d_pReqMenuBack;
	CEGUI::TLRadioButton	*d_pReqAllKind;
	CEGUI::TLRadioButton	*d_pReqEquipment;
	CEGUI::TLRadioButton	*d_pReqPotion;
	CEGUI::TLRadioButton	*d_pReqMaterial;
	CEGUI::TLRadioButton	*d_pReqGem;
	CEGUI::TLRadioButton	*d_pReqOthers;

	CEGUI::TLButton			*d_pEquipBtn;
	CEGUI::TLStaticImage	*d_pEquipMenuBack;
	CEGUI::TLRadioButton	*d_pEquipAllKind;
	CEGUI::TLRadioButton	*d_pEquipToukui;
	CEGUI::TLRadioButton	*d_pEquipYifu;
	CEGUI::TLRadioButton	*d_pEquipHujian;
	CEGUI::TLRadioButton	*d_pEquipXuezi;
	CEGUI::TLRadioButton	*d_pEquipWuqi;
	CEGUI::TLRadioButton	*d_pEquipZhuishi;
	CEGUI::TLRadioButton	*d_pEquipYupei;
	CEGUI::TLRadioButton	*d_pEquipJiezhi;
	CEGUI::TLRadioButton	*d_pEquipHuwan;

	// 种类需求在表中取得
	char					d_ReqKind[AUCTION_MAX_ITEM_TYPE_LEN];
	map<CEGUI::Window*, int>	d_KindMap;

	CEGUI::TLButton			*d_pProfessionBtn;
	CEGUI::TLStaticImage	*d_pProfessionMenu;
	CEGUI::TLButton			*d_pProfessionCommon;
	CEGUI::TLButton			*d_pProfessionXF;
	CEGUI::TLButton			*d_pProfessionXT;
	CEGUI::TLButton			*d_pProfessionTS;
	CEGUI::TLButton			*d_pProfessionZR;
	CEGUI::TLButton			*d_pProfessionSS;
	CEGUI::TLButton			*d_pProfessionYS;
	int						d_iProf;

	CEGUI::TLStaticText		*d_pKindTypeTxt;
	CEGUI::TLStaticText		*d_pQualityTypeTxt;
	CEGUI::TLStaticText		*d_pProfTypeTxt;
	CEGUI::TLStaticText		*d_pEquipmentTypeTxt;

	//时间限制
	int						d_iPrevTimeLimit[MAX_PAGE_NUM];
	int						d_iNextTimeLimit[MAX_PAGE_NUM];
	int						d_iSearchTimeLimit;

	String					d_curItemName;
	AuctionInfo				d_baseInfo;
};

#endif 
