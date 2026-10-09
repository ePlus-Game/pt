//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008/03/20
//      File_base        : UiCreditShop
//      File_ext         : h
//      Author           : Caolei
//      Description      : 信用商店界面
//
//      <Change_list>
//		1. 2008/03/20, 从CreditShop拷贝过来全部代码
//		2. 2008/03/21, 抽取公共实现到ShopImpBase
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UICREDITSHOP_H__ADC65DFA_B4D5_4BA5_B13E_F06950DE617A__INCLUDED_)
#define AFX_UICREDITSHOP_H__ADC65DFA_B4D5_4BA5_B13E_F06950DE617A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CEGUI.h"
#include "../uicommon.h"
#include "GameDataDef.h"
#include "IBShopComDef.h"
#include "UiIBShop.h"
#include "ShopImpBase.h"

using namespace std;
using namespace CEGUI;

/********************************************************************
/*						class: CreditShopNavigation
*********************************************************************/
class KUiCreditShopNavigation : public KUiWndSingleton<KUiCreditShopNavigation>
{
	
public:
    KUiCreditShopNavigation( const String& id_name );
    ~KUiCreditShopNavigation();
	
	static void		Show(void);
	void			Init(void);
	
	void			MoveToRightEdge();
	void			SetEnable(bool enable);
private:
	bool			btnOpenShop_MouseClick(const CEGUI::EventArgs& e);
	
private:
	PushButton*		m_btnOpenShop;	
};


/********************************************************************
/*						class: CreditShop
*********************************************************************/
class KUiCreditShop : public KUiWndSingleton<KUiCreditShop>
{
public:
    KUiCreditShop( const String& id_name );
    ~KUiCreditShop();

	static void		Show(void);
	static void		Hide(void);
	static void		BuyComfirm(void);
	void			Init(void);

	void			LoadShelf(BYTE* pBuff, int nShelfNum);
	void			LoadPointShopShelf(BYTE* pBuff, int nShelfNum);
	void			LoadPanel(BYTE* pBuff, int nCount);
	void			LoadStyle(BYTE* pBuff, int nCount);
	void			LoadGoodsInShelf(BYTE* pBuff, int nGoodsNum);
	void			LoadPointGoodsInShelf(BYTE* pBuff, int nGoodsNum);

	void			ClearShop();

	void	SetIBShopMessage(CommonStyle& style, String msg);

	void	SetJinShanBi(DWORD dwMoney);
	void	SetCredit(int credit);
	void	SetMaxCreditPoint(int maxCredit);
	void	SetPoint(int point);
	void	SetCreditStatus(int creditStatus);
	void	SetCreditReturnTime(DWORD date);
	void	RefreshVoucherCount();

	TLButton*	getBtnModifyPassword() { return imp->getBtnModifyPassword(); };
	//void	SetBuyResult( char* resultMessage );
private:
	KCreditShopImp*	imp;
private:
};


#endif // !defined(AFX_UICREDITSHOP_H__ADC65DFA_B4D5_4BA5_B13E_F06950DE617A__INCLUDED_)
