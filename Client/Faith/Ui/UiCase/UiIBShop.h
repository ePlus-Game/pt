 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 10/16/2007
//      File_base        : KUiIBShop
//      File_ext         : h
//      Author           : Lucien
//      Description      : IB…ÃµÍΩÁ√Ê
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _KUIIBSHOP_H_
#define _KUIIBSHOP_H_

#include "CEGUI.h"
#include "../uicommon.h"
#include "GameDataDef.h"
#include "IBShopComDef.h"
#include "TLGameObject.h"
#include "TLRadioButton.h"
#include "TLVertScrollbar.h"
#include <string>
#include "ShopImpBase.h"

using namespace std;
using namespace CEGUI;

string	Replace(string res, string find, string replace);

/********************************************************************
/*						class: IBNavigation
*********************************************************************/
class KUiIBNavigation : public KUiWndSingleton<KUiIBNavigation>
{

public:
    KUiIBNavigation( const String& id_name );
    ~KUiIBNavigation();

	static void		Show(void);
	void			Init(void);

	void			MoveToRightEdge();
	void			SetEnable(bool enable);
private:
	bool			btnOpenShop_MouseClick(const CEGUI::EventArgs& e);

private:
	PushButton*		m_btnOpenShop;	
};
class KShopImpBase;
/********************************************************************
/*						class: IBShop
*********************************************************************/
class KUiIBShop : public KUiWndSingleton<KUiIBShop>
{
public:
    KUiIBShop( const String& id_name );
    ~KUiIBShop();

	static void		Show(void);
	static void		Hide(void);
	static void		Breathe();
	static void		BuyComfirm(void);
	void			Init(void);

	void			LoadShelf(BYTE* pBuff, int nShelfNum);
	void			LoadPanel(BYTE* pBuff, int nCount);
	void			LoadStyle(BYTE* pBuff, int nCount);
	void			LoadGoodsInShelf(BYTE* pBuff, int nGoodsNum);

	void			ClearShop();

	void			SetIBShopMessage(CommonStyle& style, String msg);
	
	void	SetJinShanBi(DWORD dwMoney);
	void	SetCredit(int credit);
	void	SetMaxCreditPoint(int maxCredit);
	void	SetPoint(int point);
	void	SetCreditStatus(int creditStatus);
	void	SetCreditReturnTime(DWORD date);

	TLButton*	getBtnModifyPassword() { return imp->getBtnModifyPassword(); };
	//void	SetBuyResult( char* resultMessage );
	
private:
	KIBShopImp*		imp;
};

/********************************************************************
/*						class: IBQuantityInput
*********************************************************************/
class KUiIBQuantityInput : public KUiWndSingleton<KUiIBQuantityInput>
{

public:
    KUiIBQuantityInput( const String& id_name );
    ~KUiIBQuantityInput();

	static void		Show();
	static void		Hide();
	void			Init();
	
	void			SetQuantityInput_CallBack( void (KShopImpBase::*func)(int quantity), KShopImpBase* caller );
	void			SetItem( FIND_ITEMINDEX_PARAM& itemParam, int price, int shopIndex );
private:
	bool		btnOK_MouseClick( const CEGUI::EventArgs& e );
	bool		btnCancel_MouseClick( const CEGUI::EventArgs& e );
	bool		btnAdd_MouseClick( const CEGUI::EventArgs& e );
	bool		btnDec_MouseClick( const CEGUI::EventArgs& e );
	bool		edtQuantity_TextChanged( const CEGUI::EventArgs& e );
	void		ShowQuantity();
	void		(KShopImpBase::*QuantityInput_CallBack)(int quantity);
private:
	KShopImpBase*	m_caller;
	Editbox*		m_EdtQuantity;
	StaticText*		m_itemName;
	StaticText*		m_itemPrice;
	StaticText*		m_totalPrice;
	TLGameObject*	itemIcon;
	int				m_quantity;
	int				m_price;
	bool			m_isNumberOnly;
	std::string		m_unitShow;
	int				m_myMoney;
	int				m_maxMoney;
	enIBShopErrCode m_errCode;
	int				m_shopIndex;
	const int		m_maxCountLimit;
/*	bool			m_successBuy;*/
};

/********************************************************************
/*						class: RepayConfirm
*********************************************************************/
class KUiRepayConfirm : public KUiWndSingleton<KUiRepayConfirm>
{
public:
    KUiRepayConfirm( const String& id_name );
    ~KUiRepayConfirm();

	void		Init();
	void		SetCredits(int credits);
private:
	bool		btnOK_MouseClick( const CEGUI::EventArgs& e );
	bool		btnCancel_MouseClick( const CEGUI::EventArgs& e );
	int			m_credits;
};

/********************************************************************
/*						class: KUiIBShopResultMessage
*********************************************************************/
class KUiIBShopResultMessage : public KUiWndSingleton<KUiIBShopResultMessage>
{
public:
    KUiIBShopResultMessage( const String& id_name );
    ~KUiIBShopResultMessage();

	void		Init();
	static void Hide();
	void		SetResultMessage( char* resultMsg, int resultFlag );
	void		SetResultMessage( int resultFlag );
	void		SetResultMessage( char* resultMsg );
	void		SetReturnCreditMessage();
	void		SetBuySuccessMessage(IBGoods_Id* goods_id);
private:
	bool		btnOK_MouseClick( const CEGUI::EventArgs& e );
	bool		btnCancel_MouseClick( const CEGUI::EventArgs& e );
	bool		btnCharge_MouseClick( const CEGUI::EventArgs& e );
	bool		btnRepay_MouseClick( const CEGUI::EventArgs& e );
private:
	MultiLineEditbox*	m_txtResultMsg;
	PushButton* m_btnCharge;
	PushButton* m_btnRepay;
	PushButton* m_btnOK;
};

#endif