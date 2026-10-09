 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/16/2006
//      File_base        : KUiShop
//      File_ext         : h
//      Author           : –ª„p
//      Description      : …ÃµÍΩÁ√Ê
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiShop_H
#define KUiShop_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "CoreShell.h"

#include "TLGameObject.h"
#include "TLButton.h"
#include "TLStatic.h"
#include "UiCommonGrid.h"

using namespace CEGUI;

#define SHOP_ITEM_PER_PAGE 14

class KUiShop : public KUiWndSingleton<KUiShop>
{
private:
	TLGameObject*	d_itemIcon[SHOP_ITEM_PER_PAGE];
	KUiCommonGrid	d_itemIconGrid[SHOP_ITEM_PER_PAGE];
	TLStaticText*	d_description[SHOP_ITEM_PER_PAGE];
	TLButton*		d_normalRepair;
	TLButton*		d_specialRepair;
	TLButton*		d_normalRepairAll;
	TLButton*		d_specialRepairAll;
	int				d_canRepairLevel;		//0=«Æ∂º–ﬁ 1=«Æπª–ﬁ∆’Õ® 2=«ÆπªÃÿ–ﬁ

	TLButton*		d_previous;
	TLButton*		d_next;
	TLButton*		d_close;

	int d_curPage;
	int d_pageCount;
	int d_itemCount;
	int d_shopIdx;
	PLUS_POINT_PARAM d_plusPointInfo;
	TLStaticText* d_Tittle;
	TLStaticText*	d_plusPointTxt;
	String		  d_defaultTittle;
	KObjAtContRegion* d_items;
	
private:
	void	getChild();
	void	getData();
	void	showCurPage();

	void	setItemImage(int itemShopIndex, TLGameObject::GameObject& obj);
	String	getEmptySpaceName();
	int		getRepairAllPrice(bool special);
	bool	genRepairAllPriceLayoutText(char* layoutText, bool special);
	void	flashRepairTip();

protected:
	bool IsInsteadSpecieShop(int curShopIndex);
	bool onDirectBuy(const EventArgs& e);

	bool onNormalRepair(const EventArgs& e);
	bool onSpecialRepair(const EventArgs& e);
	bool onNormalRepairAll(const EventArgs& e);
	bool onSpecialRepairAll(const EventArgs& e);
	bool onPrevious(const EventArgs& e);
	bool onNext(const EventArgs& e);
	bool onClose(const EventArgs& e);
	bool onWndHide(const EventArgs& e);

public:
	void Init();
	void show();
	int getWndWidth();
	void	showCurPageByPlusPoint();
	void	RepairAllItem();

    KUiShop( const String& id_name	);
    ~KUiShop(						);
};


#endif