 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/12/2006
//      File_base        : KUiTradeBox
//      File_ext         : h
//      Author           : Ð»ãp
//      Description      : ½»Ò×
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiTradeBox_H
#define KUiTradeBox_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "CoreShell.h"

#include "TLGameObject.h"
#include "TLButton.h"
#include "TLEditbox.h"
#include "UiCommonGrid.h"

using namespace CEGUI;
#define TRADE_BOX_WIDTH 5
#define TRADE_BOX_HEIGHT 2

class KUiTradeBox : public KUiWndSingleton<KUiTradeBox>
{
	TLGameObject*	d_selfItem[TRADE_BOX_HEIGHT][TRADE_BOX_WIDTH];
	KUiCommonGrid	d_selfItemGrid[TRADE_BOX_HEIGHT][TRADE_BOX_WIDTH];

	TLGameObject*	d_oppositeItem[TRADE_BOX_HEIGHT][TRADE_BOX_WIDTH];
	KUiCommonGrid	d_oppositeItemGrid[TRADE_BOX_HEIGHT][TRADE_BOX_WIDTH];

	TLEditbox*		d_selfJin;
	TLEditbox*		d_selfYin;
	TLEditbox*		d_selfTong;
	
	TLEditbox*		d_oppositeJin;
	TLEditbox*		d_oppositeYin;
	TLEditbox*		d_oppositeTong;

	TLEditbox*		d_tradeInfo;
	TLButton*		d_selfLock;
	TLButton*		d_selfTrade;
	TLButton*		d_selfCancel;
	TLButton*		d_oppositeLock;
	TLButton*		d_oppositeTrade;
	TLButton*		d_oppositeCancel;

	TLButton*		d_closeBtn;

	CEGUI::Window*	d_lockImage;
	int m_iItemMum;

	bool m_bIsItem;
	bool m_bIsTrading;

	void getChild();
	void setItemImage(int itemId, TLGameObject::GameObject& obj);
	void clear();
protected:
	virtual bool onLBUp(const CEGUI::EventArgs& e); 
	virtual bool onSelfMoneyChange(const CEGUI::EventArgs& e);
	virtual bool onLock(const CEGUI::EventArgs& e);
	virtual bool onTrade(const CEGUI::EventArgs& e);
	virtual bool onCancel(const CEGUI::EventArgs& e);
	virtual bool onHide(const CEGUI::EventArgs& e);
	virtual bool onKeyDown(const CEGUI::EventArgs& e);
public:
	void printSystemMessage(const char* msg);
	void printOppoPickDropItem(bool pickdrop, int itemIndex);

	enum UI_TRADE_MSG
	{
		ui_trade_you_lock_opposite_unlock = 1,
		ui_trade_both_lock,
		ui_trade_opposite_lock,
		ui_trade_both_unlock,
		ui_trade_you_end_trade,
		ui_trade_opposite_end_trade,
		ui_trade_opposite_busy,
		ui_trade_refuse,
		ui_trade_request,
	};
	void Init();
	void show(char* oppoNameText);
	void onSelfItemChanged(KObjAtContRegion* pObj, int add);
	void onOppositeItemChanged(KObjAtContRegion* pObj, int bAdd);
	void lock(bool self);
	void unlock();
	void endTrade(bool self);
	void onOppositeMoneyChanged(int money);
	void cancelTrade();
	void SetIsItem(bool isItem);
	bool isVisible();
	void AddItemByClickPlayer();
	bool IsTrading();

    KUiTradeBox( const CEGUI::String& id_name	);
    ~KUiTradeBox(								);
};


#endif