// ShopImpBase.h: interface for the :
//		KShopImpBase	class
//		KIBShopImp		class
//		KCreditShopImp	class
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHOPQQ_H__B57D1109_DA36_47EF_BE08_8FF9216106CE__INCLUDED_)
#define AFX_SHOPQQ_H__B57D1109_DA36_47EF_BE08_8FF9216106CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CEGUI.h"
#include "../uicommon.h"
#include "GameDataDef.h"
#include "IBShopComDef.h"
#include "TLGameObject.h"
#include "TLRadioButton.h"
#include "TLVertScrollbar.h"
#include "../KMessageCentre.h"
#include <string>

using namespace std;
using namespace CEGUI;

enum
{
	pos_topleft = 0, 
	pos_topright,
	pos_bottomleft,
	pos_bottomright,
	pos_left,
	pos_top,
	pos_right,
	pos_bottom,
	pos_count,
};

/********************************************************************
/*						class: KShopImpBase
*********************************************************************/
class KShopImpBase
{
public:
	KShopImpBase();
	virtual	~KShopImpBase();
	virtual void	Init( const CEGUI::String& shopName/*, CEGUI::Window* pWnd*/ );
	void			showRepayRemind();
	void			SetIBShopMessage(CommonStyle& style, String msg);

	virtual	void	SetJinShanBi(DWORD dwMoney);
	virtual void	SetCredit(int credit);
	virtual void	SetMaxCreditPoint(int maxCredit);
	virtual void	SetPoint(int point);
	virtual	void	SetCreditStatus(int creditStatus);
	virtual void	SetCreditReturnTime(DWORD date);
	void	SetBuyResult( char* resultMessage );

	void	LoadShelf(BYTE* pBuff, int nShelfNum);
	void			LoadPanel(BYTE* pBuff, int nCount);
	void			LoadStyle(BYTE* pBuff, int nCount);
	void			LoadGoodsInShelf(BYTE* pBuff, int nGoodsNum);
	void			Purchase();
	virtual void	DoBuy();
	
	virtual void	Show();
	void			GetQuantity_CallBackHandler( int quantity );

	void			ClearAll();

	TLButton*		getBtnModifyPassword() { return m_btnModifyPassword; };
protected:
	virtual bool	btnBuy_MouseClick(const CEGUI::EventArgs& e);
	bool			btnBuy_MouseEnters(const CEGUI::EventArgs& e);
	bool			btnBuy_MouseLeaves(const CEGUI::EventArgs& e);
	void			AddGoodsToPanel(IBGoods_ListEntry* pGoods, Point leftTop, Panel panel, 
									int nPanel, int &nPanelGoodsIdx/*, LORect clipper*/);
	virtual void	ClearShelf();
	virtual bool	handleChangeShelf(const CEGUI::EventArgs& e);
	virtual bool	handleCharge(const CEGUI::EventArgs& e);
	virtual void	UpdateIntroduce();
/*	virtual void	ShowConfirmDlg( char* itemName );*/
	virtual	void    SetItemButton( TLStaticImage* item );
	virtual	bool	btnAccountInfo_MouseClick(const CEGUI::EventArgs& e);
	virtual bool	handleHelp(const CEGUI::EventArgs& e);
	virtual void	SetDefaultButtonPos( int current_x );
	void			RefreshView(int oper = 0);

private:
//	bool			handleMouseWheel(const CEGUI::EventArgs& e);
	bool			handleScroll(const CEGUI::EventArgs& e);
	bool			handleClose(const CEGUI::EventArgs& e);
	bool			handleClickItem(const CEGUI::EventArgs& e);
	bool			btnPrevView_MouseClick( const CEGUI::EventArgs& e );
	bool			btnNextView_MouseClick( const CEGUI::EventArgs& e );
	bool			btnRepay_MouseClick(const CEGUI::EventArgs& e);
/*	bool			btnCreatePassword_Clicked(const CEGUI::EventArgs& e);*/
	bool			btnModifyPassword_Clicked(const CEGUI::EventArgs& e);

	string			Parse(IBGoods pGoods, string res, TLStaticImage& itemImg);
//	string			ParseAD(IBGoods pGoods, string res);
	bool			GetPanelByID(int panelID, Panel& panel);
	void			UpdateScrollBar();
	void			AddPanelsToShelf(IBGoods_ListEntry* pGoods, int nGoodsNum, int nPanelIdx);
	void			ClearAllShelfCate();
	void			ClearSelectedGoods();
	void			HideAllPanel();
	void			HideAllGoods();
	void			Hide();
	void			RefreshAccountInfo();
	void			RefreshRepayRemindPanel(int credit, DWORD returnDate, int creditStatus );
	void			SetItemClipper(TLStaticImage& item);
	void			ShowItem( IBGoods& goods, TLStaticImage* item, Panel& panel, Point& position);
	
	int				CalculateViewCount();
	std::string		FormatDateTime(tm& returnTime);
	
public:
	//IBShop继承来的成员，必须在IBShop的构造函数或者Init中赋值
	CEGUI::Window*			m_pThisWnd;
	CEGUI::WindowManager*	m_pWindowManager;
	CEGUI::Window*			m_pRootSheet;
	int						d_shopIndex;
protected:
	//IBShop本身的成员
	TLStaticText*		d_topMsg;
	TLButton*			d_buy;
	TLButton*			d_charge;
	TLButton*			d_close;
	Window*				d_help;
	TLRadioButton*		d_helpBtn;
	TLStaticText*		d_helpText;
	TLStaticImage*		d_shelfWnd;
	TLVertScrollbar*	d_pScroll;

	TLRadioButton*		d_shelf[MAX_SHELF_NUM];
	TLStaticImage*		d_panel[MAX_PANELCOUNT_PERSHELF];
	TLStaticImage*		d_panelTitleBG[MAX_PANELCOUNT_PERSHELF];
	TLStaticText*		d_panelTitle[MAX_PANELCOUNT_PERSHELF];
	TLStaticImage*		d_item[MAX_PANELCOUNT_PERSHELF][MAX_GOODSCOUNT_PERSHELF];
	IBGoods				d_itemInfo[MAX_PANELCOUNT_PERSHELF][MAX_GOODSCOUNT_PERSHELF];
	
	int					d_panelID[MAX_SHELF_NUM][MAX_PANELCOUNT_PERSHELF];
	Panel				d_panelStyle[MAX_PANEL_NUM];
	ContentStyle		d_contentStyle[MAX_CONTENT_STYLE_NUM];
	
	int					d_curShelf;
	int					d_curPanel;
	int					d_curGoodsIdx;
	int					d_pageCount;
	int					d_itemCount;
	
	Size				d_shelfSize;
	Point				d_defaultFirstBtnPos;

	const Image*		d_pFrameImage[pos_count];

	//caol+ 2008/03/19
	Window*				d_accountInfo;
	Window*				d_repayRemind;
	StaticText*			d_repayRemind_txtRepayTime;
	StaticText*			d_repayRemind_txtRepayCount;
	
	RadioButton*		d_btnAccountInfo;
	TLStaticText*		d_introduce;
	int					d_curQuantity;

	//个人帐户信息控件
	StaticText*			d_accountInfo_Name;
	StaticText*			d_accountInfo_Level;
	StaticText*			d_accountInfo_Jinshanbi;
	StaticText*			d_accountInfo_Credit;
	StaticText*			d_accountInfo_MaxCredit;
	StaticText*			d_accountInfo_RepayTime;
	StaticText*			d_accountInfo_ConsumeAward;
	StaticText*			d_accountInfo_CreditStatus;

	StaticText*			m_txtViewCount;
	PushButton*			m_btnNextView;
	PushButton*			m_btnPrevView;
 	bool				m_bNeedScribeEvent;
	int					m_prevShopIndex;

	int					m_PointPanelLoadCount;
	int					m_yAxisOffset;
	//密码修改按钮
	TLButton*			m_btnModifyPassword;
private:
	CEGUI::String		m_wndName;
	CEGUI::String		m_shopName;
	
	//用于商品的分页显示
	int					m_curView;
	int					m_maxViewCount;
	int					m_scrollerPageSize;
	int					m_scrollerDocumentSize;
	int					m_scrollerMaxPosition;
};

/********************************************************************
/*						class: KCreditShopImp
*********************************************************************/
class KCreditShopImp: public KShopImpBase
{
public:
	KCreditShopImp();
	virtual ~KCreditShopImp();

	void	DoBuy();
	void	Init( const CEGUI::String& shopName );
	void	Show();
/*	void	LoadShelf(BYTE* pBuff, int nShelfNum);*/
	void	LoadPointShopShelf(BYTE* pBuff, int nShelfNum);

	void	SetCredit(int credit);
	void	SetMaxCreditPoint(int maxCredit);
	void	SetPoint(int point);
	void	RefreshVoucherCount();
	//void	SetCreditStatus(int creditStatus);
	//void	SetCreditReturnTime(DWORD date);
protected:
	bool	btnVoucherBuy_MouseClick( const CEGUI::EventArgs& e );
	bool    btnAccountInfo_MouseClick( const CEGUI::EventArgs& e );
	bool	btnBuy_MouseClick( const CEGUI::EventArgs& e );
	bool	handleChangeShelf(const CEGUI::EventArgs& e);
	bool	handleHelp( const CEGUI::EventArgs& e );

	void	RefreshBottomBar();
	void	ClearShelf();
	void	ShowCredit();
/*	void	ShowConfirmDlg( char* itemName );*/
	void    SetItemButton( TLStaticImage* imgItem );
	void	SetDefaultButtonPos( int current_x );
private:
	bool	btnPointShop_MouseClick(const CEGUI::EventArgs& e);

private:
	StaticText*			d_ConsumeAward;
	StaticText*			d_ConsumeAwardTitle;
	//下面的显示
	StaticText*			d_txtCredit;
	StaticText*			d_txtCreditTitle;
	StaticText*			d_txtVoucher;
	StaticText*			d_txtVoucherTitle;
	StaticImage*		d_VoucherImage;
	Window*				d_pointShop;
	TLRadioButton*		d_btnPointShop;
	bool				d_useTicket;
};


/********************************************************************
/*						class: KIBShopImp
*********************************************************************/
class KIBShopImp: public KShopImpBase
{
public:
	KIBShopImp();
	virtual ~KIBShopImp();

	void	SetJinShanBi(DWORD dwMoney);
	void	Init( const CEGUI::String& shopName );
	void	Show();
	void	SeperateBuy();
// 	void	DoBuy();
// 	bool	btnBuy_MouseClick( const CEGUI::EventArgs& e );
protected:
	bool	handleCharge( const CEGUI::EventArgs& e );
	/*void	ShowConfirmDlg( char* itemName );*/
	void	SetItemButton( TLStaticImage* item );

	
	vector<ClientBuyGoods> m_goods;

private:
	TLStaticText*		d_pJinshanbiTxt;
	int					d_jinshanbi;
	int					m_currentSec;
};

#endif // !defined(AFX_SHOPQQ_H__B57D1109_DA36_47EF_BE08_8FF9216106CE__INCLUDED_)
