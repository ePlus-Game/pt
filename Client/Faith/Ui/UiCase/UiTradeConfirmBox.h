 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/09/2006
//      File_base        : KUiTradeConfirmBox
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 交易确认对话框
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiTradeConfirmBox_H
#define KUiTradeConfirmBox_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "CoreShell.h"
#include "TLButton.h"
#include "TLStatic.h"
#include "TLEditbox.h"

using namespace CEGUI;

#define UI_TRADE_COMFIRM_BOX_MAX_TRADE_ITEM_COUNT 100
#define UI_TRADE_COMFIRM_BOX_MAX_DELAY_TIME 6

class KUiTradeConfirmBox : public KUiWndSingleton<KUiTradeConfirmBox>
{
public:	


private:
	TLStaticImage*	d_sell;
	TLStaticImage*	d_buy;
	TLStaticImage*	d_repair;
	TLEditbox*		d_count;
	TLButton*		d_increase;
	TLButton*		d_decrease;
	TLStaticText*	d_jin;
	TLStaticText*	d_yin;
	TLStaticText*	d_tong;
	TLStaticText*	d_name;
	TLButton*		d_ok;
	TLButton*		d_cancel;
	TLButton*		d_close;

	KObjAtContRegion d_region;

	colour			normal;						//正常情况下的金币数颜色
	colour			notEnough;					//当金钱不够时的颜色
	
	int				d_aItemPrice;
	int				d_itemCount;
	bool			d_cost;

	bool			d_increasing;
	bool			d_decreasing;

	int				d_mousePushDelayTime;
	int d_shopIdx;
	PLUS_POINT_PARAM d_plusPointInfo;
	TLStaticText*	d_plusPointTxt;
	Window*				d_jinImg;
	Window*				d_yinImg; 
	Window*				d_tongImg;
	void	getChild();
	void	hideAll();
	void	ShowRepairPrice();
	void	showPrice();
	void	showPriceByPlusPoint();
protected:
	bool onCancel(const CEGUI::EventArgs& e);
	bool onShow(const CEGUI::EventArgs& e);
	bool onHide(const CEGUI::EventArgs& e);
	bool onIncreaseUp(const CEGUI::EventArgs& e);
	bool onIncreaseHover(const CEGUI::EventArgs& e);
	bool onIncreaseDown(const CEGUI::EventArgs& e);
	bool onDecreaseUp(const CEGUI::EventArgs& e);
	bool onDecreaseHover(const CEGUI::EventArgs& e);
	bool onDecreaseDown(const CEGUI::EventArgs& e);
	bool onCountChanged(const CEGUI::EventArgs& e);
	bool handleKeyDown(const CEGUI::EventArgs& e);
	bool handleMouseClick(const CEGUI::EventArgs & e);
public:	
	void Init();
	void show(KObjAtContRegion* region);
	void show(KObjAtContRegion* region, int shopIdx, const PLUS_POINT_PARAM& ppt );
	void adjustPos();
	bool onOk(const CEGUI::EventArgs& e);
    KUiTradeConfirmBox( const CEGUI::String& id_name	);
    ~KUiTradeConfirmBox(								);
	
};


#endif