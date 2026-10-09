// ShopImpBase.cpp: implementation of the ShopImpBase class.
//
//////////////////////////////////////////////////////////////////////

#include "ShopImpBase.h"
#include "CoreShell.h"
#include "UiComMsgBox.h"
#include "../KMessageCentre.h"
#include "../UiConfigManager.h"
#include "UiItemTip.h"
#include "time.h"
#include "UiMapCentre.h"
#include "UiCreditShop.h"
#include "math.h"
#include <sstream>
#include <iomanip>
#include "UiIEWindow.h"
#include "UiErrorMessageBox.h"
#include "UiQuestionWindow.h"
#include "UiItemPassword.h"
#include "UiStoreBox.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

const char*	szIBFrameImageName[pos_count] = { 
	"IBshangdiannei_zuoshang", 
	"IBshangdiannei_youshang",
	"IBshangdiannei_zuoxia",
	"IBshangdiannei_youxia",
	"IBshangdiannei_zuo",
	"IBshangdiannei_shang",
	"IBshangdiannei_you",
	"IBshangdiannei_xia" };

#define IBSHOP_DEFAULT_FRAME_IMAGESET		"IBshangdiannei"
#define IBSHOP_DEFAULT_FRAME_CENTER_IMAGE	"IBshangdiannei_zhong"
#define IBGOODS_TEMPLATE_FILENAME			"uisettings/layouts/IBItemTemplate.ls"
#define IBAD_TEMPLATE_FILENAME			"uisettings/layouts/IBADTemplate.ls"

const int IBShopMsgStartIndex = 100;
const int MaxInt = static_cast<unsigned int>(~0) >> 1;

extern iCoreShell*	g_pCoreShell;

/********************************************************************
/*						class: KShopImpBase 的实现
*********************************************************************/

KShopImpBase::KShopImpBase()
: d_curShelf(0)
, d_curPanel(0)
, d_curGoodsIdx(0)
, d_pageCount(0)
, d_itemCount(0)
, d_topMsg(NULL)
, d_buy(NULL)
, d_charge(NULL)
, d_close(NULL)
, d_shelfWnd(NULL)
, d_help(NULL)
, d_helpBtn(NULL)
, d_helpText(NULL)
, d_pScroll(NULL)
, d_shopIndex(enIB_SHOP)
, d_accountInfo_Name(NULL)
, d_accountInfo_Level(NULL)
, d_accountInfo_Jinshanbi(NULL)
, d_accountInfo_Credit(NULL)
, d_accountInfo_MaxCredit(NULL)
, d_accountInfo_RepayTime(NULL)
, d_accountInfo_ConsumeAward(NULL)
, d_introduce(NULL)
, d_btnAccountInfo(NULL)
, d_repayRemind_txtRepayCount(NULL)
, d_repayRemind_txtRepayTime(NULL)
, d_repayRemind(NULL)
, d_accountInfo(NULL)
, d_accountInfo_CreditStatus(NULL)
, m_curView(0)
, m_maxViewCount(0)
, m_txtViewCount(NULL)
, m_btnNextView(NULL)
, m_btnPrevView(NULL)
, m_prevShopIndex(enIB_SHOP)
, m_PointPanelLoadCount(0)
{
	for (int i = 0; i < MAX_SHELF_NUM; i++)
	{
		d_shelf[i] = NULL;
	}
	for (i = 0; i < MAX_PANELCOUNT_PERSHELF; i++)
	{
		d_panel[i] = NULL;
		d_panelTitleBG[i] = NULL;
		d_panelTitle[i] = NULL;
	}
	for (i = 0; i < MAX_PANELCOUNT_PERSHELF; i++)
	{
		for (int j = 0; j < MAX_GOODSCOUNT_PERSHELF; j++)
		{
			d_item[i][j] = NULL;
		}
	}
	for (i = 0; i < pos_count; i++)
	{
		d_pFrameImage[i] = NULL;
	}	

	m_btnModifyPassword = NULL;
}

KShopImpBase::~KShopImpBase()
{
	
}

void KShopImpBase::Init( const CEGUI::String& shopName )
{
	m_shopName = shopName;
	m_wndName = "TaharezLook/" + m_shopName + "/";

	for (int i = 0; i < MAX_SHELF_NUM; i++)
	{
		char szName[COMMON_CLIENT_MSG_LEN_256];
		ZeroMemory(szName, COMMON_CLIENT_MSG_LEN_256);
		
		CEGUI::String pageName = m_wndName + "Pagebtn_%d";
		sprintf( szName, pageName.c_str(), i );
		d_shelf[i] = static_cast<TLRadioButton*>(m_pThisWnd->getChild(AnsiToUtf8(szName)));
		d_shelf[i]->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KShopImpBase::handleChangeShelf, this));
	}

	for (i = 0; i < pos_count; i++)
	{
		d_pFrameImage[i] = &ImagesetManager::getSingleton().getImageset(IBSHOP_DEFAULT_FRAME_IMAGESET)->getImage(szIBFrameImageName[i]);
	}
	
	d_shelfWnd = static_cast<TLStaticImage*>(m_pThisWnd->getChild(m_wndName + "Shelf"));
// 	d_shelfWnd->subscribeEvent(TLVertScrollbar::EventMouseWheel, 
// 		Event::Subscriber(&KShopImpBase::handleMouseWheel, this));
	d_shelfSize = d_shelfWnd->getSize(Absolute);


	d_topMsg = static_cast<TLStaticText*>(m_pThisWnd->getChild(m_wndName + "IBShopMessage"));
	//d_topMsg->setRenderMode(true);
	//d_topMsg->setRollSpeedH(50);

	d_pScroll = static_cast<TLVertScrollbar*>(m_pThisWnd->getChild(m_wndName + "Scrollbar"));
	d_pScroll->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
		Event::Subscriber(&KShopImpBase::handleScroll, this));
// 	d_pScroll->subscribeEvent(TLVertScrollbar::EventMouseWheel, 
// 		Event::Subscriber(&KShopImpBase::handleMouseWheel, this));

	//购买按钮不能有效，否则handlebuy出问题
// 	d_buy = static_cast<TLButton*>(m_pThisWnd->getChild(m_wndName + "Buybtn"));
// 	d_buy->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KShopImpBase::handleBuy, this));

	d_helpBtn = static_cast<TLRadioButton*>(m_pThisWnd->getChild(m_wndName + "Helpbtn"));
	d_helpBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KShopImpBase::handleHelp, this));
	d_help = m_pThisWnd->getChild(m_wndName + "HelpWnd");

	Window* firstBtn = m_pThisWnd->getChild(m_wndName + "Pagebtn_0");
	d_defaultFirstBtnPos = Point(firstBtn->getPosition(Absolute).d_x, firstBtn->getPosition(Absolute).d_y);

	d_close = static_cast<TLButton*>(m_pThisWnd->getChild(m_wndName + "Close"));
	d_close->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KShopImpBase::handleClose, this));

	d_helpText = static_cast<TLStaticText*>(d_help->getChild(m_wndName + "HelpWnd/Text"));
	d_helpText->setText("");

	//临时屏蔽帮助界面
// 	char helpLayoutText[COMMON_CLIENT_MSG_LEN_1024];
// 	ZeroMemory(helpLayoutText, sizeof(helpLayoutText));

// 	sprintf(helpLayoutText, "<Layout w=%d>%s", 
// 		(int)(d_helpText->getAbsoluteWidth()), KMessageCentre::GetMessage(ibshop_message, 1));
// 	d_helpText->useLayout();
// 	d_helpText->getLayout()->SetText(helpLayoutText);
// 	d_helpText->getLayout()->flashLayout();

	//caol+ 2008/03/19
	d_accountInfo = d_shelfWnd->getChild(m_wndName + "Shelf/AccountInfo");
	
	d_charge = static_cast<TLButton*>(d_accountInfo->getChild(m_wndName + "Shelf/AccountInfo/btnCharge"));
	d_charge->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KShopImpBase::handleCharge, this));
	
	//caol+ 2008/03/21 初始化还款提醒界面的控件
	d_repayRemind = d_shelfWnd->getChild(m_wndName + "Shelf/RepayRemind");
	d_repayRemind->getChild(m_wndName + "Shelf/RepayRemind/btnRepay")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KShopImpBase::btnRepay_MouseClick, this));
	d_repayRemind_txtRepayTime = static_cast<StaticText*>(d_repayRemind->getChild(m_wndName + "Shelf/RepayRemind/txtRepayTime"));
	d_repayRemind_txtRepayCount = static_cast<StaticText*>(d_repayRemind->getChild(m_wndName + "Shelf/RepayRemind/txtRepayCount"));

	d_btnAccountInfo = static_cast<RadioButton*>(m_pThisWnd->getChild(m_wndName + "btnAccountInfo"));
	d_btnAccountInfo->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KShopImpBase::btnAccountInfo_MouseClick, this));
	d_introduce = static_cast<TLStaticText*>(m_pThisWnd->getChild(m_wndName + "Introduce"));

	d_accountInfo->getChild(m_wndName + "Shelf/AccountInfo/btnRepay")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KShopImpBase::btnRepay_MouseClick, this));
	
	//初始化帐户信息页面中的控件
	d_accountInfo_Name = static_cast<StaticText*>(d_accountInfo->getChild(m_wndName + "Shelf/AccountInfo/Name"));
	d_accountInfo_Level = static_cast<StaticText*>(d_accountInfo->getChild(m_wndName + "Shelf/AccountInfo/Level"));
	d_accountInfo_Jinshanbi = static_cast<StaticText*>(d_accountInfo->getChild(m_wndName + "Shelf/AccountInfo/Jinshanbi"));
	d_accountInfo_Credit = static_cast<StaticText*>(d_accountInfo->getChild(m_wndName + "Shelf/AccountInfo/Credit"));
	d_accountInfo_MaxCredit = static_cast<StaticText*>(d_accountInfo->getChild(m_wndName + "Shelf/AccountInfo/MaxCredit"));
	d_accountInfo_RepayTime = static_cast<StaticText*>(d_accountInfo->getChild(m_wndName + "Shelf/AccountInfo/RepayTime"));
	d_accountInfo_ConsumeAward = static_cast<StaticText*>(d_accountInfo->getChild(m_wndName + "Shelf/AccountInfo/ConsumeAward"));
	d_accountInfo_CreditStatus = static_cast<StaticText*>(d_accountInfo->getChild(m_wndName + "Shelf/AccountInfo/CreditStatus"));

	m_txtViewCount = static_cast<StaticText*>(m_pThisWnd->getChild(m_wndName + "txtView"));
	m_btnNextView = static_cast<PushButton*>(m_pThisWnd->getChild(m_wndName + "btnNextView"));
	m_btnPrevView = static_cast<PushButton*>(m_pThisWnd->getChild(m_wndName + "btnPrevView"));

	m_btnPrevView->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KShopImpBase::btnPrevView_MouseClick, this));
	m_btnNextView->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KShopImpBase::btnNextView_MouseClick, this));

	m_pThisWnd->setZLevel(Window::Bottom);

// 	m_pThisWnd->getChild( m_wndName + "btnCreatePassword" )->subscribeEvent(
// 		PushButton::EventClicked, 
// 		Event::Subscriber( &KShopImpBase::btnCreatePassword_Clicked, this ) );

	m_btnModifyPassword = static_cast< TLButton* >( m_pThisWnd->getChild( m_wndName + "btnModifyPassword" ) );
	m_btnModifyPassword->subscribeEvent(
		PushButton::EventClicked, 
		Event::Subscriber( &KShopImpBase::btnModifyPassword_Clicked, this ) );
	
	RefreshAccountInfo();
}

void KShopImpBase::showRepayRemind()
{
	if ( (NULL != d_shelf) && (NULL != d_shelf[0]) && (0 == d_curShelf) && ( enPRESENT_SHOP != d_shopIndex ) )
	{
		d_repayRemind->setVisible(true);
		d_repayRemind->setZLevel(Window::SuperTop);
		d_repayRemind->moveToFront();

		m_txtViewCount->setVisible(false);
		m_btnNextView->setVisible(false);
		m_btnPrevView->setVisible(false);
	}
	else
	{
		d_repayRemind->setVisible(false);
	}

	int credits = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enCREDIT_SHOP, NULL);

	PlayerCreditInfo creditInfo;
	if ( g_pCoreShell->GetGameData(GDI_PLAYER_CREDIT_INFO, (unsigned int)&creditInfo, NULL) > 0)
	{
		RefreshRepayRemindPanel(credits, creditInfo.uReturnTime, creditInfo.uCreditState);
	}
}


bool KShopImpBase::handleChangeShelf( const CEGUI::EventArgs& e )
{
	WindowEventArgs* wargs = (WindowEventArgs *)(&e);
	Window *tmpWnd = wargs->window;

	int oldShelfNum = d_curShelf;
	int newShelfNum = 0;
	for(int i = 0; i < MAX_SHELF_NUM; i++)
	{
		if (tmpWnd == d_shelf[i])
		{
			newShelfNum = i;
			break;
		}
	}
	
	if ( (oldShelfNum != newShelfNum) || (m_prevShopIndex != d_shopIndex) )
	{
		ClearShelf();
		d_curShelf = newShelfNum;
		g_pCoreShell->OperationRequest( GOI_IBSHOP_LOAD_SHELF, d_curShelf, d_shopIndex);
		
		m_txtViewCount->setVisible(true);
		m_btnNextView->setVisible(true);
		m_btnPrevView->setVisible(true);
		
		showRepayRemind();
		UpdateIntroduce();
		return true;
	}
	else
	{
		return false;
	}
}

// bool KShopImpBase::handleMouseWheel( const CEGUI::EventArgs& e )
// {
// 	MouseEventArgs* eventArgs = (MouseEventArgs*)&e;
// 
// 	if(d_pScroll->isVisible())
// 	{
// 		d_pScroll->setScrollPosition( d_pScroll->getScrollPosition()
// 									  - d_pScroll->getStepSize() * eventArgs->wheelChange );
// 	}
// 	return true;	
// }

bool KShopImpBase::handleScroll( const CEGUI::EventArgs& e )
{
	float scrollPos = d_pScroll->getScrollPosition();
	int curPanelPos = - (m_curView * m_scrollerPageSize) + m_yAxisOffset;
	//float offset_y = m_scrollerPageSize;//(d_shelfSize.d_height - d_shelfWnd->getHeight(Absolute)) * (scrollPos / static_cast<float>(m_scrollerMaxPosition));
	
	for (int i = 0; i < MAX_PANELCOUNT_PERSHELF; i++)
	{
		if (d_panel[i] && d_panel[i]->isVisible())
		{
			Panel tempPanel;
			if (!GetPanelByID(d_panelID[d_curShelf][i], tempPanel))
				break;
			
			int height = tempPanel.PanelTop;
			Point p = d_panel[i]->getPosition(Absolute);
			d_panel[i]->setPosition(Absolute, Point(p.d_x, curPanelPos));
		}
	}

// 	for ( int j = 0; j < MAX_PANELCOUNT_PERSHELF; j++ )
// 	{
// 		for ( int k = 0; k <  MAX_GOODSCOUNT_PERSHELF; k++ )
// 		{
// 			if ( NULL ==  d_item[j][k] )
// 			{
// 				break;
// 			}
// 			else
// 			{
// 				SetItemClipper(*d_item[j][k]);
// 			}
// 		}
// 	}

	return true;	
}

bool KShopImpBase::btnBuy_MouseClick( const CEGUI::EventArgs& e )
{
	handleClickItem(e);

	if (d_curShelf >= 0 && d_curShelf < MAX_SHELF_NUM &&
		d_curPanel >= 0 && d_curPanel < MAX_PANELCOUNT_PERSHELF &&
		d_curGoodsIdx >= 0 && d_curGoodsIdx < MAX_GOODSCOUNT_PERSHELF )
	{
		IBGoods_Id ibitem = d_itemInfo[d_curPanel][d_curGoodsIdx].Id;
		FIND_ITEMINDEX_PARAM itemidx;
		itemidx.nGenre = ibitem.Genera;
		itemidx.nDetail = ibitem.Detail;
		itemidx.nParticular = ibitem.Particular;
		itemidx.nLevel = ibitem.Level;

		int curPrice = int(d_itemInfo[d_curPanel][d_curGoodsIdx].Info.Price * d_itemInfo[d_curPanel][d_curGoodsIdx].Info.Discount / 100);

		KUiIBQuantityInput::GetSingleton().SetItem(itemidx, curPrice, d_shopIndex);
		KUiIBQuantityInput::GetSingleton().SetQuantityInput_CallBack(KShopImpBase::GetQuantity_CallBackHandler, this);
 		KUiIBQuantityInput::Show();
	}

	return true; 
}

bool KShopImpBase::handleHelp( const CEGUI::EventArgs& e )
{
	ClearShelf();
	d_curShelf = -1;
	if (!d_help->isVisible())
		d_help->show();

	d_introduce->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 400)));
	m_txtViewCount->setVisible(false);
	m_btnNextView->setVisible(false);
	m_btnPrevView->setVisible(false);
	
	return true;	
}

bool KShopImpBase::handleClose( const CEGUI::EventArgs& e )
{
	Hide();
	return true;	
}

bool KShopImpBase::handleCharge( const CEGUI::EventArgs& e )
{
	//ShellExecute(NULL, "open", "http://www.kingsoft.com", NULL, NULL, SW_SHOWNORMAL);

	//Edit by DarkMagic(DuanMu)

	if ( KUiQuestionWindow::IsVisible() )
	{
		char* tipmsg = KMessageCentre::GetMessage( ibshop_message, 71 );
		if ( NULL != tipmsg )
		{
			KUiErrorMessageBox::GetSingleton().AddMessage( AnsiToUtf8( tipmsg ) );
		}

		return false;
	}

	if (KUiCfgLoader::getSingleton().getIBShopCfg().isUseIE != 0)
	{
		char _url[COMMON_CLIENT_MSG_LEN_256];
		_url[0] = 0;
		
		KIniFile ini;	
		ini.Load(UI_CFG_STRING);//读取uicfg.ini文件配置
		ini.GetString("WebUrl", "Recharge", "", _url, sizeof(_url));
		_url[COMMON_CLIENT_MSG_LEN_256 - 1] = 0;
		if (strlen(_url) == 0)
		{
			return false;
		}
		ShellExecute(NULL, "open", _url, NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	else
	{
		KUiIEWindow::Show();
	}

	return true;
}

bool KShopImpBase::btnAccountInfo_MouseClick( const CEGUI::EventArgs& e )
{
	d_curShelf = -1;
	ClearShelf();
	if ( NULL != d_accountInfo && !d_accountInfo->isVisible())
	{
		RefreshAccountInfo();
		d_accountInfo->show();
		d_accountInfo->moveToFront();
		d_introduce->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 401)));

		m_txtViewCount->setVisible(false);
		m_btnNextView->setVisible(false);
		m_btnPrevView->setVisible(false);
	}
	return true;
}

void KShopImpBase::Purchase()
{
	if (d_curShelf >= 0 && d_curShelf < MAX_SHELF_NUM &&
		d_curPanel >= 0 && d_curPanel < MAX_PANELCOUNT_PERSHELF &&
		d_curGoodsIdx >= 0 && d_curGoodsIdx < MAX_GOODSCOUNT_PERSHELF )
	{
		IBGoods_Id ibitem = d_itemInfo[d_curPanel][d_curGoodsIdx].Id;
		FIND_ITEMINDEX_PARAM tagItemIdx;
		tagItemIdx.nGenre = ibitem.Genera;
		tagItemIdx.nDetail = ibitem.Detail;
		tagItemIdx.nParticular = ibitem.Particular;
		tagItemIdx.nLevel = ibitem.Level;
		
		char itemName[COMMON_CLIENT_MSG_LEN_128];
		ZeroMemory(itemName, COMMON_CLIENT_MSG_LEN_128);
		g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_WITH_COLOR_BY_INDEXPARAM
			, (unsigned int)&tagItemIdx, (unsigned int)itemName);
		
		DoBuy();

		//ShowConfirmDlg( itemName );
	}	
}

bool KShopImpBase::handleClickItem(const CEGUI::EventArgs& e)
{	
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
//	ClearSelectedGoods();

	for (int nPanel = 0; nPanel < MAX_PANELCOUNT_PERSHELF; nPanel++)
	{
		if (d_panel[nPanel] && d_panel[nPanel]->isVisible())
		{
			for (int nGoodsIdx = 0; nGoodsIdx < MAX_GOODSCOUNT_PERSHELF; nGoodsIdx++)
			{
				if (d_item[nPanel][nGoodsIdx] && 
					d_item[nPanel][nGoodsIdx]->isVisible())
				{
/*					String str = d_item[nPanel][nGoodsIdx]->getName() + "/ibitemhover";*/
					
					if (arg->window->getParent() == d_item[nPanel][nGoodsIdx] ||
						arg->window == d_item[nPanel][nGoodsIdx])
					{
// 						d_item[nPanel][nGoodsIdx]->getChild(str)->setVisible(true);
// 						d_item[nPanel][nGoodsIdx]->getChild(str)->setZLevel(Window::Top);
						
// 						KUiItemTip::GetSingleton().show(
// 							Utf8ToAnsi(d_item[nPanel][nGoodsIdx]->getTooltipText()), 
// 							d_item[nPanel][nGoodsIdx]->getUnclippedInnerRect(), KUiItemTip::BottomLeft);

						d_curPanel = nPanel;
						d_curGoodsIdx = nGoodsIdx;
						return true;
					}
				}
			}
		}		
	}

	return true;
}


string KShopImpBase::Parse(IBGoods pGoods, string res, TLStaticImage& itemImg)
{
	// 得到物品信息
	KItemInfo tagItemInfo;
	FIND_ITEMINDEX_PARAM tagItemIdx;
	ZeroMemory(&tagItemInfo, sizeof(KItemInfo));
	ZeroMemory(&tagItemIdx, sizeof(FIND_ITEMINDEX_PARAM));

	tagItemIdx.nGenre		= pGoods.Id.Genera;
	tagItemIdx.nDetail		= pGoods.Id.Detail;
	tagItemIdx.nParticular	= pGoods.Id.Particular;
	tagItemIdx.nLevel		= pGoods.Id.Level;
	g_pCoreShell->GetGameData( GDI_GET_IBITEM_BIG_IMAGE, (unsigned int)&tagItemIdx, (int)&tagItemInfo );

	float discount = (float)pGoods.Info.Discount / 100.0f;
	int oldPrice = pGoods.Info.Price;
	int newPrice = (int)(pGoods.Info.Discount * oldPrice / 100);
	char szDiscount[COMMON_CLIENT_MSG_LEN_32];
	char szOldPrice[COMMON_CLIENT_MSG_LEN_32];
	char szNewPrice[COMMON_CLIENT_MSG_LEN_32];
	ZeroMemory(szDiscount, COMMON_CLIENT_MSG_LEN_32);
	ZeroMemory(szOldPrice, COMMON_CLIENT_MSG_LEN_32);
	ZeroMemory(szNewPrice, COMMON_CLIENT_MSG_LEN_32);
	
	sprintf(szDiscount, "%.1f", discount);
	if ( enIB_SHOP == d_shopIndex )
	{
		//IB商店的货币要除以一个uicfg中的配置的比率
		float jinshanbiRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().jinshanbiRate);
		sprintf(szOldPrice, "%.2f",  static_cast<float>(oldPrice) / jinshanbiRate);
		sprintf(szNewPrice, "%.2f", static_cast<float>(newPrice) / jinshanbiRate);
	}
	else if ( enCREDIT_SHOP == d_shopIndex )
	{
		float creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
		sprintf(szOldPrice, "%.2f",  static_cast<float>(oldPrice) / creditRate);
		sprintf(szNewPrice, "%.2f", static_cast<float>(newPrice) / creditRate);
	}
	else
	{
		sprintf(szOldPrice, "%d", oldPrice);
		sprintf(szNewPrice, "%d", newPrice);
	}

	string str("");
	for ( int i = 0; i < MAX_CONTENT_STYLE_NUM; i++ )
	{
		if( d_contentStyle[i].StyleID == pGoods.Style )
		{
			str = d_contentStyle[i].Content;
			break;
		}
	}
	
	if ( str.length() <= 0 )
		str = res;
	
	if ( str.length() != 0 )
	{
		//设定宽度
		ostringstream layoutHeader;
		layoutHeader<<"<Layout w="<<itemImg.getWidth(Absolute)<<">";
		str = layoutHeader.str() + str;

		if ( -1 != str.find("#ad") )
		{
			// #ad = 广告，advertisement
			str = Replace(str, "#ad", "<Obj t=pic>set:IBADImage_");
			// #ae =  广告图片结束 advertisement end
			str = Replace(str, "#ae", " image:full_image</Obj>");
			// #at =  广告语 advertisement text
			str = Replace(str, "#at", KMessageCentre::GetMessage(ibshop_message, 16));
			itemImg.getChild( itemImg.getName() + "/btnVoucherBuy")->setVisible(false);
			itemImg.getChild( itemImg.getName() + "/btnBuy")->setVisible(false);
			itemImg.getChild( itemImg.getName() + "/ibitemhover")->setVisible(false);
			//广告不需要Tooltop
			itemImg.setTooltip(NULL);
		}
		else
		{
			itemImg.subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KShopImpBase::handleClickItem, this));

			//取得IB物品的物品图标，同时取得IB物品的状态图标，如“热卖”等
			ostringstream imgLayout;
			imgLayout<<"<Obj t=pic "<<KMessageCentre::GetMessage(ibshop_message, 22)<<pGoods.Info.Status
				<<".spr "<<" >set:"<<tagItemInfo.szImageSet<<" image:"<<tagItemInfo.szImage<<"</Obj>";
			str = Replace(str, "#ii", imgLayout.str().c_str());

			//取得物品名字下面的划线图片
			str = Replace(str, "#li", "<Obj t=pic>set:IBGoodsUnderline image:full_image</Obj>");

			//添加原价上面的划线
			str = Replace(str, "#bi", KMessageCentre::GetMessage(ibshop_message, 17));
			str = Replace(str, "#name", tagItemInfo.szName);
			str = Replace(str, "#op", szOldPrice);
			str = Replace(str, "#np", szNewPrice);
		}
		
		str = str + "</Layout>";
	}
	return	str;
}

void KShopImpBase::UpdateScrollBar()
{
// 	Size clip = d_shelfWnd->getSize(Absolute);
// 	d_pScroll->setScrollPosition(0);
// 	if (clip.d_height >= d_shelfSize.d_height)
// 	{
// 		d_pScroll->hide();
// 	}
// 	else
// 	{
// 		float renderHeight = d_shelfWnd->getHeight(Absolute);
// 		float totleHeight = d_shelfSize.d_height;
// 		float step = renderHeight/(2 * (totleHeight - renderHeight));
// 		d_pScroll->setStepSize(step);
// 		d_pScroll->show();
// 	}
}

bool KShopImpBase::GetPanelByID(int panelID, Panel& panel)
{
	for (int i = 0; i < MAX_PANEL_NUM; ++i)
	{
		if (d_panelStyle[i].PanelID == panelID)
		{
			panel = d_panelStyle[i];
			return true;
		}
	}
	return false;
}


void KShopImpBase::AddPanelsToShelf(IBGoods_ListEntry* pGoods, int nGoodsNum, int nPanelIdx)
{
	//Panel生成设置
	int nPanelMarge;
	int nPanelID;
		
	if ( enPRESENT_SHOP == d_shopIndex )
	{
		nPanelID = d_panelID[MAX_SHELF_NUM - 1][nPanelIdx];
	}
	else
	{
		nPanelID = d_panelID[d_curShelf][nPanelIdx];
	}

	if (nPanelID > 0 && nPanelID < MAX_PANEL_NUM)
	{
		Panel tempPanel;
		TLStaticImage*	pPanelImage = NULL;
		TLStaticImage*	pTitleBG = NULL;
		TLStaticText*	pTitle = NULL;

		if ( NULL == GetPanelByID(nPanelID, tempPanel) )
			return;
		
		if ( NULL == d_panel[nPanelIdx] )
		{
			char szPanelName[COMMON_CLIENT_MSG_LEN_64];
			char szPanelTitleBG[COMMON_CLIENT_MSG_LEN_64];
			char szPanelTitle[COMMON_CLIENT_MSG_LEN_64];
			ZeroMemory(szPanelName, COMMON_CLIENT_MSG_LEN_64);
			ZeroMemory(szPanelTitle, COMMON_CLIENT_MSG_LEN_64);
			ZeroMemory(szPanelTitleBG, COMMON_CLIENT_MSG_LEN_64);
			
			CEGUI::String sTemp;
			sTemp = m_wndName + "Shelf/panel_%d";
			sprintf( szPanelName, sTemp.c_str(), nPanelIdx );
			sTemp = m_wndName + "Shelf/panel_%d/image";
			sprintf( szPanelTitleBG, sTemp.c_str(), nPanelIdx );
			sTemp = m_wndName + "Shelf/panel_%d/image/panelTitle_%d";
			sprintf( szPanelTitle, sTemp.c_str(), nPanelIdx );

			d_panel[nPanelIdx] = static_cast<TLStaticImage*>(
				m_pWindowManager->createWindow("TaharezLook/StaticImage", szPanelName));
			d_panelTitleBG[nPanelIdx] = static_cast<TLStaticImage*>(
				m_pWindowManager->createWindow("TaharezLook/StaticImage", szPanelTitleBG));
			d_panelTitle[nPanelIdx] = static_cast<TLStaticText*>(
				m_pWindowManager->createWindow("TaharezLook/StaticText", szPanelTitle));
		}
		
		nPanelMarge = tempPanel.PanelMarge;
		pPanelImage = d_panel[nPanelIdx];
		pTitleBG = d_panelTitleBG[nPanelIdx];
		pTitle = d_panelTitle[nPanelIdx];

		//Panel设置
		pPanelImage->setImage(IBSHOP_DEFAULT_FRAME_IMAGESET, IBSHOP_DEFAULT_FRAME_CENTER_IMAGE);
		pPanelImage->setPosition(Absolute, Point(tempPanel.PanelLeft, tempPanel.PanelTop));
		m_yAxisOffset = tempPanel.PanelTop;
		pPanelImage->setSize(Absolute, Size(tempPanel.PanelWidth, tempPanel.PanelHeight));
// 		pPanelImage->setFrameImages(d_pFrameImage[pos_topleft],
// 			d_pFrameImage[pos_topright], d_pFrameImage[pos_bottomleft],
// 			d_pFrameImage[pos_bottomright], d_pFrameImage[pos_left],
// 			d_pFrameImage[pos_top], d_pFrameImage[pos_right], d_pFrameImage[pos_bottom]);
// 		pPanelImage->setFrameEnabled(true);
		//pPanelImage->setZLevel(Window::Bottom);
		
		//Panel标题设置
		pTitleBG->setImage(IBSHOP_DEFAULT_FRAME_IMAGESET, IBSHOP_DEFAULT_FRAME_CENTER_IMAGE);
		pTitleBG->setSize(Absolute, Size(tempPanel.TitleWidth, tempPanel.TitleHeight));
		pTitleBG->setPosition(Absolute, Point(nPanelMarge, nPanelMarge));
		pTitleBG->setFrameImages(d_pFrameImage[pos_topleft],
			d_pFrameImage[pos_topright], d_pFrameImage[pos_bottomleft],
			d_pFrameImage[pos_bottomright], d_pFrameImage[pos_left],
			d_pFrameImage[pos_top], d_pFrameImage[pos_right], d_pFrameImage[pos_bottom]);
		pTitleBG->setFrameEnabled(true);

		pTitle->setSize(Absolute, Size(tempPanel.TitleWidth, tempPanel.TitleHeight));
		pTitle->setPosition(Absolute, Point(0, 0));
		pTitle->useLayout();
		pTitle->getLayout()->formatText(tempPanel.TitleName);
		pTitle->getLayout()->SetText(tempPanel.TitleName);
				
// 		LORect clipper;
// 		Rect textArea = d_shelfWnd->getUnclippedPixelRect();
// 		textArea.setPosition(d_shelfWnd->getPosition(Absolute));
// 		cerectToLorect(&textArea, &clipper);
// 		pTitle->getLayout()->setClipper(clipper);
		pTitleBG->addChildWindow(pTitle);
		pTitle->setPosition(Absolute, Point(0, 0));
		pPanelImage->addChildWindow(pTitleBG);
		d_shelfWnd->addChildWindow(pPanelImage);

		LORect pannelclipper;
		Rect textArea = pPanelImage->getUnclippedPixelRect();
		Point itemPos(textArea.d_left, textArea.d_top);
		Point wndPos = m_pThisWnd->getPosition(Absolute);  //todo 这里提一个成员m_wndPos;
		Point clipperPos;
		clipperPos.d_x = itemPos.d_x - wndPos.d_x;
		clipperPos.d_y = itemPos.d_y - wndPos.d_y;
		textArea.setPosition(clipperPos);
		textArea.d_right -= d_pFrameImage[pos_right]->getWidth();
		textArea.d_bottom -= d_pFrameImage[pos_bottom]->getHeight();
		cerectToLorect(&textArea, &pannelclipper);
		pTitle->getLayout()->setClipper(pannelclipper);

		pTitle->show();
		pTitleBG->show();
		pPanelImage->show();
		int nPanelMarge = tempPanel.PanelMarge;
		//Panel添加Goods
		Point LeftTop;
		int width = tempPanel.TitleWidth + nPanelMarge * 2 + tempPanel.CellWidth;
		
		if (width > tempPanel.PanelWidth)
		{
			LeftTop = Point(nPanelMarge, nPanelMarge * 2 + tempPanel.TitleHeight);
		}
		else
		{
			LeftTop = Point(nPanelMarge * 2 + tempPanel.TitleWidth, nPanelMarge);
		}

		int nPanelGoodsIdx = 0;

		for (int i = 0; i < nGoodsNum && i < MAX_GOODSCOUNT_PERSHELF; i++)
		{
			if ( NULL != pGoods )
			{
				if (pGoods->goods.PanelIndex == nPanelIdx)
				{
					AddGoodsToPanel(pGoods, LeftTop, tempPanel, nPanelIdx, nPanelGoodsIdx/*, clipper*/);
				}
				pGoods = pGoods->pNextEntry;
			}
			else
			{
				return;
			}
		}

		//设置页面大小
		d_shelfSize.d_height = max(d_shelfSize.d_height, tempPanel.PanelTop + tempPanel.PanelHeight);
		showRepayRemind();

// 		if ( enPRESENT_SHOP == d_shopIndex )
// 		{
// 			m_maxViewCount = 1;
// 			m_curView = 0;
// 			d_pScroll->setScrollPosition(0);
// 		}
// 		else
//		{
			//caol+ 2008/03/26 翻页功能
			m_maxViewCount = CalculateViewCount();

			m_scrollerPageSize = d_shelfWnd->getHeight(Absolute);
			m_scrollerDocumentSize = m_maxViewCount * m_scrollerPageSize;
			m_scrollerMaxPosition= m_scrollerDocumentSize - m_scrollerPageSize; 
			d_pScroll->setPageSize(m_scrollerPageSize);
			d_pScroll->setDocumentSize(m_scrollerDocumentSize);
			m_curView = 0;
			d_pScroll->setScrollPosition(0);

			//add by marryme 2008/10/28
			//In order to caculate the move distance
			if( d_panel[nPanelIdx] && d_item && d_item[nPanelIdx][0])
			{
				int itemHeight		= d_item[ nPanelIdx ][ 0 ]->getAbsoluteHeight();
				int panelHeight		= d_shelfWnd->getHeight( Absolute );
				m_scrollerPageSize	= ( ( int )panelHeight / ( itemHeight + nPanelMarge ) ) * ( itemHeight + nPanelMarge );
			}
//		}
 		RefreshView();
	}
}

void KShopImpBase::AddGoodsToPanel(IBGoods_ListEntry* pGoods, Point leftTop, Panel panel, 
								int nPanelIdx, int &nPanelGoodsIdx/*, LORect clipper*/)
{
	TLStaticImage* tempItem = NULL;
	m_bNeedScribeEvent = false;
	if ( NULL == d_item[nPanelIdx][nPanelGoodsIdx] )
	{
		// 添加一个IB物品
		char szName[COMMON_CLIENT_MSG_LEN_128];
		ZeroMemory(szName, COMMON_CLIENT_MSG_LEN_128);
		
		CEGUI::String sTemp = m_wndName +  "Shelf/panel_%d/item_%d";
		sprintf(szName, sTemp.c_str(), nPanelIdx, nPanelGoodsIdx);
		d_item[nPanelIdx][nPanelGoodsIdx] = static_cast<TLStaticImage*>(m_pWindowManager->loadWindowLayout(
			IBGOODS_TEMPLATE_FILENAME, szName, "", NULL, NULL, true));

		m_bNeedScribeEvent = true;
		d_panel[nPanelIdx]->addChildWindow( d_item[nPanelIdx][nPanelGoodsIdx] );
	}

	tempItem = d_item[nPanelIdx][nPanelGoodsIdx];

	// IB物品显示的位置和大小
	int nPanelMarge = panel.PanelMarge;
	int col = (int)((panel.PanelWidth - leftTop.d_x) / panel.CellWidth);
	Point absPoint;
	int offsetX = nPanelGoodsIdx % col;
	int offsetY = (int)(nPanelGoodsIdx / col);
	absPoint.d_x = leftTop.d_x + offsetX * nPanelMarge + offsetX * panel.CellWidth;
	absPoint.d_y = leftTop.d_y + offsetY * nPanelMarge + offsetY * panel.CellHeight;

	if ( NULL != pGoods )
	{
		ShowItem(pGoods->goods, tempItem, panel, absPoint);
	}

	d_itemInfo[nPanelIdx][nPanelGoodsIdx] = pGoods->goods;
	
	nPanelGoodsIdx++;
	pGoods = pGoods->pNextEntry;
}

void KShopImpBase::ClearShelf()
{
	ClearSelectedGoods();
	HideAllGoods();
	HideAllPanel();
	d_help->hide();
	d_accountInfo->hide();
	d_repayRemind->hide();
	d_shelfSize = Size(0, 0);	
}

void KShopImpBase::ClearAllShelfCate()
{
	for (int i = 0; i < MAX_SHELF_NUM; i++)
	{
		if (d_shelf[i])
			d_shelf[i]->hide();
	}
}

void KShopImpBase::ClearSelectedGoods()
{
	d_curPanel = -1;
	d_curGoodsIdx = -1;
	for (int nPanel = 0; nPanel < MAX_PANELCOUNT_PERSHELF; nPanel++)
	{
		if (d_panel[nPanel] && d_panel[nPanel]->isVisible())
		{
			for (int nPanelGoodsIdx = 0; nPanelGoodsIdx < MAX_GOODSCOUNT_PERSHELF; nPanelGoodsIdx++)
			{
				if (d_item[nPanel][nPanelGoodsIdx] && 
					d_item[nPanel][nPanelGoodsIdx]->isVisible())
				{
					String str = d_item[nPanel][nPanelGoodsIdx]->getName() + "/ibitemhover";
					d_item[nPanel][nPanelGoodsIdx]->getChild(str)->setVisible(false);
				}
			}
		}		
	}
}

void KShopImpBase::HideAllPanel()
{
	for (int i = 0; i < MAX_PANELCOUNT_PERSHELF; i++)
	{
		if (d_panel[i])
			d_panel[i]->hide();
	}
}

void KShopImpBase::HideAllGoods()
{
	for (int nPanel = 0; nPanel < MAX_PANELCOUNT_PERSHELF; nPanel++)
	{
		if (d_panel[nPanel] && d_panel[nPanel]->isVisible())
		{
			for (int nPanelGoodsIdx = 0; nPanelGoodsIdx < MAX_GOODSCOUNT_PERSHELF; nPanelGoodsIdx++)
			{
				if (d_item[nPanel][nPanelGoodsIdx] && 
					d_item[nPanel][nPanelGoodsIdx]->isVisible())
				{
					d_item[nPanel][nPanelGoodsIdx]->hide();
				}
			}
		}		
	}
}

void KShopImpBase::SetIBShopMessage(CommonStyle& style, String msg)
{
	String fontName = AnsiToUtf8(style.font);
	if(FontManager::getSingleton().isFontPresent(fontName))
	{
		d_topMsg->setFont(fontName);
	}
	d_topMsg->setRollSpeedH(style.speed);
	d_topMsg->setTextColours(style.color);
	d_topMsg->setText(msg);	
}

void KShopImpBase::LoadShelf( BYTE* pBuff, int nShelfNum )
{
	if (!pBuff)
		return;

	ClearAllShelfCate();
	Load_IBShelf_Ret* pShelf = (Load_IBShelf_Ret*)pBuff;
	
	Point pCurButtonPosition;
	int curBtn_x = d_defaultFirstBtnPos.d_x;

	for ( int i = 0; i < nShelfNum && i < MAX_SHELF_NUM; i++ )
	{
		int nShelfIdx = pShelf->ShelfIdx;
		pCurButtonPosition = Point(curBtn_x, d_defaultFirstBtnPos.d_y);

		d_shelf[nShelfIdx]->show();
		d_shelf[nShelfIdx]->setPosition(Absolute, pCurButtonPosition);
		d_shelf[nShelfIdx]->setText(AnsiToUtf8(pShelf->ShelfName));
		
		for (int j = 0; j < MAX_PANELCOUNT_PERSHELF; j++)
		{
			d_panelID[nShelfIdx][j] = pShelf->PanelIdxList[j];
		}
		curBtn_x += d_shelf[nShelfIdx]->getWidth(Absolute);
		pShelf++;
	}

	SetDefaultButtonPos(curBtn_x);
	g_pCoreShell->OperationRequest( GOI_IBSHOP_LOAD_SHELF, d_curShelf, d_shopIndex);	
}

void KShopImpBase::LoadPanel( BYTE* pBuff, int nCount )
{
	Panel* pPanel = (Panel*)pBuff;
	for ( int i = 0; i < nCount && i < MAX_PANEL_NUM; i++ )
	{
		d_panelStyle[i] = *pPanel;
		pPanel++;
	}
}

void KShopImpBase::LoadStyle( BYTE* pBuff, int nCount )
{
	ContentStyle* pContentStyle = (ContentStyle*)pBuff;
	for ( int i = 0; i < nCount && i < MAX_CONTENT_STYLE_NUM; i++ )
	{
		d_contentStyle[i] = *pContentStyle;
		pContentStyle++;
	}
}

void KShopImpBase::LoadGoodsInShelf( BYTE* pBuff, int nGoodsNum )
{
	ClearShelf();
	for ( int nPanelIdx = 0; nPanelIdx < MAX_PANELCOUNT_PERSHELF; nPanelIdx++ )
	{
		IBGoods_ListEntry* pGoodsRet = (IBGoods_ListEntry*)pBuff;
		if (!pGoodsRet || nGoodsNum == 0)
			return;

		//Panel生成设置
		AddPanelsToShelf(pGoodsRet, nGoodsNum, nPanelIdx);
	}

	//UpdateScrollBar();	
}

void KShopImpBase::DoBuy()
{
	if ( -1 == d_curQuantity  )
	{
		return;
	}
	int nPanel = d_curPanel;
	int nGoodsIdx = d_curGoodsIdx;
	IBGoods_Id ibitem = d_itemInfo[nPanel][nGoodsIdx].Id;

	ClientBuyGoods goods;
	goods.goods = ibitem;
	goods.number = d_curQuantity;
	goods.price = 0;
	goods.shelfIdx = d_curShelf;
	goods.shopIdx = d_shopIndex;
	goods.useTicket = false;
	goods.bOnecItem = false;

	g_pCoreShell->OperationRequest(GOI_IBSHOP_BUY, (unsigned int)&goods, NULL);	
}

void KShopImpBase::Hide()
{
	if ( m_pRootSheet && m_pThisWnd )
	{
		m_pRootSheet->removeChildWindow( m_pThisWnd );
		m_pThisWnd->hide();
	}	
}

bool KShopImpBase::btnRepay_MouseClick( const CEGUI::EventArgs& e )
{
	//显示信用点数
	int credits = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enCREDIT_SHOP, NULL);
	if ( credits >= 0 )
	{
		KUiRepayConfirm::GetSingleton().SetCredits(credits);
		KUiRepayConfirm::Show();
	}
	else
	{
		char comfirmString[COMMON_CLIENT_MSG_LEN_8];
		strcpy(comfirmString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().comfirmString));
		KUiComMsgBox::GetSingleton().setComMsgPosition();
		KUiComMsgBox::Show();
		KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 57)));
		KUiComMsgBox::GetSingleton().setBtnName((utf8*)comfirmString);
	}

	return true;	
}

void KShopImpBase::RefreshAccountInfo()
{
	//显示角色名
	KUiPlayerBaseInfo baseInfo;
	ZeroMemory(&baseInfo, sizeof(KUiPlayerBaseInfo));
	g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&baseInfo, NULL );
	d_accountInfo_Name->setText(AnsiToUtf8(baseInfo.Name));
	
	//显示角色等级
	KUiPlayerAttribute attr;
	ZeroMemory(&attr, sizeof(KUiPlayerAttribute));
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&attr, NULL);
	d_accountInfo_Level->setText(iToString(attr.nLevel));

	//显示金山币
	int jinshanBi = g_pCoreShell->GetGameData(GDI_PLAYER_JINSHANBI, NULL, NULL);
	char szJinshanbiShow[COMMON_CLIENT_MSG_LEN_32];
	float jinshanbiRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().jinshanbiRate);
	_snprintf(szJinshanbiShow, COMMON_CLIENT_MSG_LEN_32, "%.2f", static_cast<float>(jinshanBi) / jinshanbiRate);
	d_accountInfo_Jinshanbi->setText(AnsiToUtf8(szJinshanbiShow));

	//显示信用点数
	int credits = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enCREDIT_SHOP, NULL);
	float creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
	char szCreditShow[COMMON_CLIENT_MSG_LEN_32];
	ZeroMemory(szCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
	sprintf(szCreditShow, "%.2f",  static_cast<float>(credits) / creditRate);
	d_accountInfo_Credit->setText(AnsiToUtf8(szCreditShow));

	//显示消费积分
	int comsumeAward = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enPRESENT_SHOP, NULL);
	d_accountInfo_ConsumeAward->setText(iToString(comsumeAward));

	PlayerCreditInfo creditInfo;
	if ( g_pCoreShell->GetGameData(GDI_PLAYER_CREDIT_INFO, (unsigned int)&creditInfo, NULL) > 0 )
	{
		if ( enCreditState_Disable == creditInfo.uCreditState )
		{
			d_accountInfo_RepayTime->setText(AnsiToUtf8("------------"));
		}
		else
		{
			//显示还款日期
			tm* returnTime = localtime((const long*)&creditInfo.uReturnTime);
			d_accountInfo_RepayTime->setText(AnsiToUtf8(FormatDateTime(*returnTime).c_str()));
		}

		//显示最大信用点数
		//显示信用点数
		creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
		char szMaxCreditShow[COMMON_CLIENT_MSG_LEN_32];
		ZeroMemory(szMaxCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
		sprintf(szMaxCreditShow, "%.2f",  static_cast<float>(creditInfo.uMaxCredit[enCREDIT_SHOP]) / creditRate);
		d_accountInfo_MaxCredit->setText(AnsiToUtf8(szMaxCreditShow));

		SetCreditStatus(creditInfo.uCreditState);
	}
	else
	{
		d_accountInfo_RepayTime->setText("");
		d_accountInfo_MaxCredit->setText("");
	}
}

void KShopImpBase::UpdateIntroduce()
{
	if ( d_curShelf >= 0 && d_curShelf < MAX_SHELF_NUM )
	{
		d_introduce->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, (d_shopIndex + 1) * 100 + d_curShelf)));
	}
}

void KShopImpBase::Show()
{
	g_pCoreShell->OperationRequest( GOI_IBSHOP_LOAD_SHELF, d_curShelf, d_shopIndex);
	UpdateIntroduce();
	RefreshAccountInfo();
	RefreshView(0);
}

void KShopImpBase::GetQuantity_CallBackHandler( int quantity )
{
 	d_curQuantity = quantity;
 	Purchase();
}

// void KShopImpBase::ShowConfirmDlg( char* itemName )
// {
// 
// }

void KShopImpBase::SetItemButton( TLStaticImage* item )
{
	if ( m_bNeedScribeEvent )
	{
		item->getChild(item->getName() + "/btnBuy")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KShopImpBase::btnBuy_MouseClick, this));
		item->getChild(item->getName() + "/btnBuy")->subscribeEvent(StaticImage::EventMouseEnters, Event::Subscriber(&KShopImpBase::btnBuy_MouseEnters, this));
		item->getChild(item->getName() + "/btnBuy")->subscribeEvent(StaticImage::EventMouseLeaves, Event::Subscriber(&KShopImpBase::btnBuy_MouseLeaves, this));
		item->getChild(item->getName() + "/btnVoucherBuy")->subscribeEvent(StaticImage::EventMouseEnters, Event::Subscriber(&KShopImpBase::btnBuy_MouseEnters, this));
		item->getChild(item->getName() + "/btnVoucherBuy")->subscribeEvent(StaticImage::EventMouseLeaves, Event::Subscriber(&KShopImpBase::btnBuy_MouseLeaves, this));
	}

	item->getChild(item->getName() + "/btnBuy")->setVisible(true);
	item->getChild(item->getName() + "/btnBuy")->setZLevel(Window::SuperTop);
	item->getChild(item->getName() + "/btnVoucherBuy")->setVisible(false);
	Point pt;
	pt.d_x = item->getWidth(Absolute) - item->getChild(item->getName() + "/btnBuy")->getWidth(Absolute) - d_pFrameImage[pos_right]->getWidth();
	pt.d_y = item->getHeight(Absolute) - item->getChild(item->getName() + "/btnBuy")->getHeight(Absolute) - d_pFrameImage[pos_bottom]->getHeight();
	item->getChild(item->getName() + "/btnBuy")->setPosition(Absolute, pt);
}

void KShopImpBase::SetCredit( int credit )
{
	float creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
	char szCreditShow[COMMON_CLIENT_MSG_LEN_32];
	ZeroMemory(szCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
	sprintf(szCreditShow, "%.2f",  static_cast<float>(credit) / creditRate);

	d_accountInfo_Credit->setText(AnsiToUtf8(szCreditShow));	

	PlayerCreditInfo creditInfo;
	if ( g_pCoreShell->GetGameData(GDI_PLAYER_CREDIT_INFO, (unsigned int)&creditInfo, NULL) > 0 )
	{
		RefreshRepayRemindPanel(credit, creditInfo.uReturnTime, creditInfo.uCreditState);
	}
}

void KShopImpBase::SetMaxCreditPoint( int maxCredit )
{
	float creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
	char szMaxCreditShow[COMMON_CLIENT_MSG_LEN_32];
	ZeroMemory(szMaxCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
	sprintf(szMaxCreditShow, "%.2f",  static_cast<float>(maxCredit) / creditRate);
	d_accountInfo_MaxCredit->setText(AnsiToUtf8(szMaxCreditShow));
}

void KShopImpBase::SetPoint( int point )
{
	//消费积分
	d_accountInfo_ConsumeAward->setText(iToString(point));
}

void KShopImpBase::SetCreditStatus( int creditStatus )
{
	switch ( creditStatus )
	{
	case enCreditState_Disable:
		d_accountInfo_CreditStatus->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 60)));
		break;
	case enCreditState_Good:
		d_accountInfo_CreditStatus->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 61)));
		break;
	case enCreditState_Bad:
		d_accountInfo_CreditStatus->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 62)));
		break;
	default :
		d_accountInfo_CreditStatus->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 63)));
		break;
	}

	//d_accountInfo_CreditStatus->setText(iToString(creditStatus));
}

void KShopImpBase::SetCreditReturnTime( DWORD date )
{
	tm* returnTime = localtime((const long*)&date);

	d_accountInfo_RepayTime->setText(AnsiToUtf8(FormatDateTime(*returnTime).c_str()));

	int credit = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enCREDIT_SHOP, NULL);

	PlayerCreditInfo creditInfo;
	if ( g_pCoreShell->GetGameData(GDI_PLAYER_CREDIT_INFO, (unsigned int)&creditInfo, NULL) > 0 )
	{
		RefreshRepayRemindPanel(credit, date, creditInfo.uCreditState);
	}
}

void KShopImpBase::SetJinShanBi( DWORD dwMoney )
{
	char szJinshanbiShow[COMMON_CLIENT_MSG_LEN_32];
	float jinshanbiRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().jinshanbiRate);
	_snprintf(szJinshanbiShow, COMMON_CLIENT_MSG_LEN_32, "%.2f", static_cast<float>(dwMoney) / jinshanbiRate);
	d_accountInfo_Jinshanbi->setText(AnsiToUtf8(szJinshanbiShow));
}

void KShopImpBase::RefreshRepayRemindPanel( int credit, DWORD repayDate, int creditStatus )
{
	 //显示还款金额
	tm* returnTime = localtime((const long*)&repayDate);
	tm	curReturnTime;
	curReturnTime.tm_year = returnTime->tm_year;
	curReturnTime.tm_mon = returnTime->tm_mon;
	curReturnTime.tm_yday = returnTime->tm_yday;
	curReturnTime.tm_mday = returnTime->tm_mday;
	curReturnTime.tm_hour = returnTime->tm_hour;
	curReturnTime.tm_min = returnTime->tm_min;

	time_t cur_t = time(NULL);
	tm* currentTime = localtime(&cur_t);
	if ( ( currentTime->tm_year == curReturnTime.tm_year ) &&  ( returnTime->tm_yday - curReturnTime.tm_yday > 10 ) && ( credit > 0 ) )
	{
		d_repayRemind_txtRepayCount->setTextColours(colour(1.0f, 0, 0, 1.0f));
	}
	else
	{
		d_repayRemind_txtRepayCount->setTextColours(colour(1.0f, 1.0f, 1.0f, 1.0f));
	}
	ostringstream creditShow;
	
	float creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
	char szCreditShow[COMMON_CLIENT_MSG_LEN_32];
	ZeroMemory(szCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
	sprintf(szCreditShow, "%.2f",  static_cast<float>(credit) / creditRate);

	creditShow<<KMessageCentre::GetMessage(ibshop_message, 14)<<szCreditShow;
	d_repayRemind_txtRepayCount->setText(AnsiToUtf8(creditShow.str().c_str()));

	//显示还款日期
	if ( creditStatus > 0 )
	{
		d_repayRemind_txtRepayTime->setText(AnsiToUtf8(FormatDateTime(curReturnTime).c_str()));
	}
	else
	{
		d_repayRemind_txtRepayTime->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 12)));
	}	
}

void KShopImpBase::SetItemClipper( TLStaticImage& item )
{
	TLStaticText* itemInfo = static_cast<TLStaticText*>(item.getChild(item.getName() + "/iteminfo"));

	LORect pannelclipper;
	Rect textArea = d_shelfWnd->getUnclippedPixelRect();
	Point itemPos(textArea.d_left, textArea.d_top);
	Point wndPos = m_pThisWnd->getPosition(Absolute);  //todo 这里提一个成员m_wndPos;
	Point clipperPos;
	clipperPos.d_x = itemPos.d_x - wndPos.d_x;
	clipperPos.d_y = itemPos.d_y - wndPos.d_y;
	textArea.setPosition(clipperPos);
// 	textArea.d_right -= d_pFrameImage[pos_right]->getWidth();
// 	textArea.d_bottom -= d_pFrameImage[pos_bottom]->getHeight();
	cerectToLorect(&textArea, &pannelclipper);
	itemInfo->useLayout();
	itemInfo->getLayout()->setClipper(pannelclipper);	
}

void KShopImpBase::ShowItem( IBGoods& goods, TLStaticImage* item, Panel& panel, Point& position )
{
	// IB物品显示的位置和大小
	int nPanelMarge = panel.PanelMarge;

	//TODO 这里的框架应该开放配置
	item->setSize(Absolute, Size(panel.CellWidth, panel.CellHeight));
	item->setPosition(Absolute, position);
	item->show();
	item->setFrameEnabled(true);
	item->setImage(IBSHOP_DEFAULT_FRAME_IMAGESET, IBSHOP_DEFAULT_FRAME_CENTER_IMAGE);
	item->setFrameImages(d_pFrameImage[pos_topleft],
			d_pFrameImage[pos_topright], d_pFrameImage[pos_bottomleft],
			d_pFrameImage[pos_bottomright], d_pFrameImage[pos_left],
			d_pFrameImage[pos_top], d_pFrameImage[pos_right], d_pFrameImage[pos_bottom]);

	SetItemButton(item);

	string strStyle("");
	for (int i = 0; i < MAX_CONTENT_STYLE_NUM; i++)
	{
		if(d_contentStyle[i].StyleID == goods.Style)
		{
			strStyle = d_contentStyle[i].Content;
			break;
		}
	}

	bool isAD = false;
	if ( strStyle.length() > 0 )
	{
		isAD = (-1 != strStyle.find("#ad"));
		if ( !isAD )
		{
			// IB物品提示信息
			KItemInfo tagItemInfo;
			FIND_ITEMINDEX_PARAM tagItemIdx;
			ZeroMemory(&tagItemInfo, sizeof(KItemInfo));
			ZeroMemory(&tagItemIdx, sizeof(FIND_ITEMINDEX_PARAM));
			
			tagItemIdx.nGenre		= goods.Id.Genera;
			tagItemIdx.nDetail		= goods.Id.Detail;
			tagItemIdx.nParticular	= goods.Id.Particular;
			tagItemIdx.nLevel		= goods.Id.Level;
			g_pCoreShell->GetGameData( /*GDI_ITEM_INFO_PARTICULAR*/GDI_GET_IBITEM_BIG_IMAGE, (unsigned int)&tagItemIdx, (int)&tagItemInfo );
			item->setTooltipText(AnsiToUtf8(tagItemInfo.szToolTip));
		}
	}

	// IB物品信息
	string str = KMessageCentre::GetMessage(ibshop_message, 0);
	string text = Parse(goods, str, *item);
	char* layoutText = (char*)text.c_str();

	TLStaticText* itemInfo = static_cast<TLStaticText*>(item->getChild(item->getName() + "/iteminfo"));
	itemInfo->setPosition(Absolute, Point(nPanelMarge, nPanelMarge));
	itemInfo->setSize(Absolute, item->getSize(Absolute));
	itemInfo->useLayout();
	itemInfo->getLayout()->formatText(layoutText);
	itemInfo->getLayout()->SetText(layoutText);
	itemInfo->getLayout()->flashLayout();
	itemInfo->fitLayoutSize(true);
	//itemInfo->getLayout()->setClipper(clipper);

	SetItemClipper(*item);

	if (  d_shopIndex == enCREDIT_SHOP && !isAD )
	{
		//得到该物品是否可以用代金券购买，从而控制用代金券购买的按钮是否显示
		FIND_ITEMINDEX_PARAM itemVoucherInfo;
		itemVoucherInfo.nGenre = goods.Id.Genera;
		itemVoucherInfo.nDetail = goods.Id.Detail;
		itemVoucherInfo.nParticular = goods.Id.Particular;
		itemVoucherInfo.nLevel = goods.Id.Level;
		ITEM_IB_BUY_TYPE ibBuyType = g_pCoreShell->GetGameData(GDI_BUY_IBITEM_MONEY_TYPE, (unsigned int)&itemVoucherInfo, NULL);
		bool canUseTicket = (ibBuyType & IIBT_TICKET) != 0;
		item->getChild(item->getName() + "/btnVoucherBuy")->setVisible(canUseTicket);
		item->getChild(item->getName() + "/btnVoucherBuy")->setZLevel(Window::SuperTop);
		item->getChild(item->getName() + "/btnVoucherBuy")->moveToFront();
	}
	else
	{
		item->getChild(item->getName() + "/btnVoucherBuy")->setVisible(false);
	}

	String itemcontent = item->getName() + "/iteminfo";
	String itemhover = item->getName() + "/ibitemhover";
	item->getChild(itemcontent)->setEnabled(false);
	item->getChild(itemhover)->setEnabled(false);
	item->getChild(itemhover)->setSize(Absolute, Size(panel.CellWidth, panel.CellHeight));
}

bool KShopImpBase::btnNextView_MouseClick( const CEGUI::EventArgs& e )
{
	if ( m_curView < m_maxViewCount - 1 )
	{
		m_curView++;
		RefreshView(2);
		return true;
	}
	else
		return false;
}

bool KShopImpBase::btnPrevView_MouseClick( const CEGUI::EventArgs& e )
{
	if ( m_curView > 0 )
	{
		m_curView--;
		RefreshView(1);
		return true;
	}
	else
		return false;
}

void KShopImpBase::RefreshView( int oper )
{
	int step = d_shelfWnd->getHeight(Absolute);
	switch (oper)
	{
	case 0:		//不需要滚动
		
		break;
	case 1:		//向上滚动
		{
			d_pScroll->setScrollPosition( 
			d_pScroll->getScrollPosition() - step);
		}
		break;
	case 2:		//向下滚动
		{	
			d_pScroll->setScrollPosition( 
			d_pScroll->getScrollPosition() + step);
		}
		break;
	}
	
	ostringstream viewCountShow;
	viewCountShow<< (((m_curView + 1) > m_maxViewCount) ? m_maxViewCount : (m_curView + 1)) <<"/"<<m_maxViewCount;
	m_txtViewCount->setText(AnsiToUtf8(viewCountShow.str().c_str()));
}

bool KShopImpBase::btnBuy_MouseEnters( const CEGUI::EventArgs& e )
{
	//caol+ 2008/03/25 处理滑动过快的情况下，pannle的OnMouseEnters事件不触发的问题
	WindowEventArgs* wargs = (WindowEventArgs *)(&e);
	Window *tmpWnd = wargs->window;
	MouseEventArgs& mea = static_cast<CEGUI::MouseEventArgs&>(const_cast<CEGUI::EventArgs&>(e));
	static_cast<TLStaticImage*>(tmpWnd->getParent())->onMouseEnters(mea);
	return true;
}

bool KShopImpBase::btnBuy_MouseLeaves( const CEGUI::EventArgs& e )
{
	//caol+ 2008/03/25 处理滑动过快的情况下，pannle的OnMouseEnters事件不触发的问题
	WindowEventArgs* wargs = (WindowEventArgs *)(&e);
	Window *tmpWnd = wargs->window;
	MouseEventArgs& mea = static_cast<CEGUI::MouseEventArgs&>(const_cast<CEGUI::EventArgs&>(e));
	static_cast<TLStaticImage*>(tmpWnd->getParent())->onMouseLeaves(mea);
	return true;
}

void KShopImpBase::SetDefaultButtonPos( int current_x )
{
	Point pCurButtonPosition = Point(current_x, d_defaultFirstBtnPos.d_y);
	d_btnAccountInfo->setPosition(Absolute, pCurButtonPosition);
	pCurButtonPosition.d_x += d_btnAccountInfo->getWidth(Absolute);
	d_helpBtn->setPosition(Absolute, pCurButtonPosition);	
}

int KShopImpBase::CalculateViewCount()
{
	int curMax_Y = 0, curMin_Y = MaxInt;
	for ( int i = 0; i < MAX_PANELCOUNT_PERSHELF; i++ )
	{
		if ( NULL != d_panel && NULL != d_panel[i] )
		{
			if ( (d_panel[i]->getYPosition(Absolute) + d_panel[i]->getHeight(Absolute)) > curMax_Y )
			{
				curMax_Y = d_panel[i]->getYPosition(Absolute) + d_panel[i]->getHeight(Absolute);
			}
			if ( d_panel[i]->getYPosition(Absolute) < curMin_Y )
			{
				curMin_Y = d_panel[i]->getYPosition(Absolute);
			}
			/*currentMaxHeight = d_panel[i]->getHeight(Absolute) > currentMaxHeight ? d_panel[i]->getHeight(Absolute) : currentMaxHeight;*/
		}
	}

	int totalHeight = curMax_Y - curMin_Y + 1;
	int shelfHeight = d_shelfWnd->getHeight(Absolute);
	int viewCount = totalHeight / shelfHeight + ((totalHeight % shelfHeight) != 0);   // d_shelfWnd->getHeight(Absolute);
	return viewCount;
}

void KShopImpBase::SetBuyResult( char* resultMessage )
{
	
}

std::string KShopImpBase::FormatDateTime( tm& returnTime )
{
	std::string yearShow(KMessageCentre::GetMessage(time_message, 0));
	std::string monShow(KMessageCentre::GetMessage(time_message, 1));
	std::string dayShow(KMessageCentre::GetMessage(time_message, 2));
	std::string hourShow(KMessageCentre::GetMessage(time_message, 3));
	std::string minuteShow(KMessageCentre::GetMessage(time_message, 4));
	std::ostringstream repayTime;
	repayTime<<returnTime.tm_year + 1900<<yearShow
		<<returnTime.tm_mon + 1<<monShow
		<<returnTime.tm_mday<<dayShow<<"  "
		<<setw(2)<<setfill('0')<<returnTime.tm_hour<<hourShow
		<<setw(2)<<setfill('0')<<returnTime.tm_min<<minuteShow;
	
	return repayTime.str();
}

void KShopImpBase::ClearAll()
{
	ClearShelf();
	m_curView = 0;
	m_maxViewCount = 1;
	RefreshView();

	int i = 0, j = 0;

	for ( i = 0; i < MAX_SHELF_NUM; i++ )
	{
		for ( int j = 0; j < MAX_PANELCOUNT_PERSHELF; j++ )
		{
			d_panelID[i][j] = -1;
		}
	}

	for ( i = 0; i < MAX_PANEL_NUM; i++ )
	{
		ZeroMemory( &d_panelStyle[i], sizeof(Panel) ); 
		d_panelStyle[i].PanelID = -2;
	}

	for ( i = 0; i < MAX_CONTENT_STYLE_NUM; i++ )
	{
		ZeroMemory( &d_contentStyle[i], sizeof(ContentStyle) );
		d_contentStyle[i].StyleID = -3;
	}
}


bool KShopImpBase::btnModifyPassword_Clicked( const CEGUI::EventArgs& e )
{
	KUiStoreBox::getSingleton().ShowPasswordDlg();
	
	return true;
}

/********************************************************************
/*						class: KCreditShopImp
*********************************************************************/

void KCreditShopImp::Init( const CEGUI::String& shopName )
{
	KShopImpBase::Init(shopName);
	//积分商店
	d_pointShop = m_pThisWnd->getChild("TaharezLook/CreditShop/PointShop");

	//积分商店按钮
	d_btnPointShop = static_cast<TLRadioButton*>(m_pThisWnd->getChild("TaharezLook/CreditShop/btnPointShop"));
	d_btnPointShop->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KCreditShopImp::btnPointShop_MouseClick,  this));

	d_txtVoucherTitle = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/CreditShop/VoucherTitle"));
	d_txtVoucher = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/CreditShop/Voucher"));

	d_txtCreditTitle = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/CreditShop/CreditTitle"));
	d_txtCredit = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/CreditShop/Credit"));

	d_ConsumeAward = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/CreditShop/ConsumeAward"));
	d_ConsumeAwardTitle = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/CreditShop/ConsumeAwardTitle"));
	
	d_VoucherImage = static_cast<StaticImage*>(m_pThisWnd->getChild("TaharezLook/CreditShop/VoucherImage"));
}

void KCreditShopImp::ClearShelf()
{
	KShopImpBase::ClearShelf();
	d_pointShop->hide();
}

bool KCreditShopImp::btnPointShop_MouseClick( const CEGUI::EventArgs& e )
{
	m_PointPanelLoadCount = 0;
	ClearShelf();
	d_shopIndex = enPRESENT_SHOP;
	d_curShelf = 0;
	g_pCoreShell->OperationRequest( GOI_IBSHOP_LOAD_SHELF, 0, enPRESENT_SHOP);
	UpdateIntroduce();
	RefreshBottomBar();

	m_txtViewCount->setVisible(true);
	m_btnNextView->setVisible(true);
	m_btnPrevView->setVisible(true);
	return true;
}

bool KCreditShopImp::handleChangeShelf( const CEGUI::EventArgs& e )
{
	m_prevShopIndex = d_shopIndex;
	d_shopIndex = enCREDIT_SHOP;
	RefreshBottomBar();
	return KShopImpBase::handleChangeShelf(e);
}

void KCreditShopImp::DoBuy()
{
	if ( -1 ==  d_curQuantity)
	{
		d_useTicket = false;
		return;
	}
	int nPanel = d_curPanel;
	int nGoodsIdx = d_curGoodsIdx;
	IBGoods_Id ibitem = d_itemInfo[nPanel][nGoodsIdx].Id;

	ClientBuyGoods goods;
	goods.goods = ibitem;
	goods.number = d_curQuantity;
	goods.price = 0;
	goods.shelfIdx = d_curShelf;
	goods.shopIdx = d_shopIndex;
	goods.useTicket = d_useTicket;
	goods.bOnecItem = false;

	g_pCoreShell->OperationRequest(GOI_IBSHOP_BUY, (unsigned int)&goods, NULL);	
	d_useTicket = false;
}

KCreditShopImp::KCreditShopImp()
: KShopImpBase()
, d_useTicket(false)
, d_txtCredit(NULL)
, d_txtVoucher(NULL)
, d_pointShop(NULL) 
, d_btnPointShop(NULL)
, d_ConsumeAward(NULL)
, d_ConsumeAwardTitle(NULL)
, d_txtCreditTitle(NULL)
, d_txtVoucherTitle(NULL)
, d_VoucherImage(NULL)
{
	d_shopIndex = enCREDIT_SHOP;
}

KCreditShopImp::~KCreditShopImp()
{
	
}

void KCreditShopImp::Show()
{
	KShopImpBase::Show();	

	PlayerCreditInfo creditInfo;
	if ( g_pCoreShell->GetGameData(GDI_PLAYER_CREDIT_INFO, (unsigned int)&creditInfo, NULL) > 0 )
	{
		//显示信用点数
		int credits = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enCREDIT_SHOP, NULL);

		float creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
		char szCreditShow[COMMON_CLIENT_MSG_LEN_32];
		char szMaxCreditShow[COMMON_CLIENT_MSG_LEN_32];
		ZeroMemory(szCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
		ZeroMemory(szMaxCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
		sprintf(szCreditShow, "%.2f",  static_cast<float>(credits) / creditRate);
		sprintf(szMaxCreditShow, "%.2f",  static_cast<float>(creditInfo.uMaxCredit[enCREDIT_SHOP]) / creditRate);
		
		ostringstream creditShow;
		creditShow<<szCreditShow<<"/"<<szMaxCreditShow;
		d_txtCredit->setText(AnsiToUtf8(creditShow.str().c_str()));
	}

	RefreshBottomBar();
}

void KCreditShopImp::ShowCredit()
{
	//显示信用点数
	int credits = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enCREDIT_SHOP, NULL);
	d_accountInfo_Credit->setText(iToString(credits));

	PlayerCreditInfo creditInfo;
	if ( g_pCoreShell->GetGameData(GDI_PLAYER_CREDIT_INFO, (unsigned int)&creditInfo, NULL) > 0 )
	{
		//显示还款日期
		ostringstream repayTime;
		repayTime<<credits<<"/"<<creditInfo.uMaxCredit[enCREDIT_SHOP]<<KMessageCentre::GetMessage(ibshop_message, 10);

		//显示最大信用点数
		d_txtCredit->setText(repayTime.str());
	}	
}

bool KCreditShopImp::btnVoucherBuy_MouseClick( const CEGUI::EventArgs& e )
{
	d_useTicket = true;
	int oldShopIndex = d_shopIndex;
	d_shopIndex = -1;
	bool res = KShopImpBase::btnBuy_MouseClick(e);
	d_shopIndex = oldShopIndex;
	return res;
}

void KCreditShopImp::SetItemButton( TLStaticImage* item )
{
	KShopImpBase::SetItemButton(item);
	if ( m_bNeedScribeEvent )
	{
		item->getChild(item->getName() + "/btnVoucherBuy")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KCreditShopImp::btnVoucherBuy_MouseClick, this));
	}
	
	Point pt;
	pt.d_x = item->getChild(item->getName() + "/btnBuy")->getXPosition(Absolute) - item->getChild(item->getName() + "/btnVoucherBuy")->getWidth(Absolute);
	pt.d_y = item->getChild(item->getName() + "/btnBuy")->getYPosition(Absolute);
	item->getChild(item->getName() + "/btnVoucherBuy")->setPosition(Absolute, pt);
	//TODO superTop是不是会有问题？不过hoverImage的是Top，所以这里只能用SuperTop
	item->getChild(item->getName() + "/btnVoucherBuy")->setZLevel(Window::SuperTop);
	item->getChild(item->getName() + "/btnVoucherBuy")->setVisible(true);
	item->getChild(item->getName() + "/btnVoucherBuy")->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 21)));

	switch(d_shopIndex)
	{
	case enCREDIT_SHOP:
		{
			item->getChild(item->getName() + "/btnBuy")->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 18)));
		}
	    break;
	case enPRESENT_SHOP:
		item->getChild(item->getName() + "/btnBuy")->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 19)));
	    break;
	default:
	    break;
	}
}

void KCreditShopImp::RefreshBottomBar()
{
	bool isPresentShop = (enPRESENT_SHOP == d_shopIndex);

	d_ConsumeAwardTitle->setVisible(isPresentShop);
	d_ConsumeAward->setVisible(isPresentShop);
	
	d_txtCreditTitle->setVisible(!isPresentShop);
	d_txtCredit->setVisible(!isPresentShop);

	d_txtVoucherTitle->setVisible(!isPresentShop);
	d_txtVoucher->setVisible(!isPresentShop);
	d_VoucherImage->setVisible(!isPresentShop);


	if ( isPresentShop )
	{
		//显示消费积分
		int comsumeAward = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enPRESENT_SHOP, NULL);
		d_ConsumeAward->setText(iToString(comsumeAward));
	}
	else
	{	
		//刷新代金券数量
		int ticketcount = g_pCoreShell->GetGameData(GDI_PLAYER_PRESENT_TICKET_COUNT, NULL, NULL);
		d_txtVoucher->setText(iToString(ticketcount));
	}

	RefreshView(0);
}

bool KCreditShopImp::btnAccountInfo_MouseClick( const CEGUI::EventArgs& e )
{
	d_shopIndex = enCREDIT_SHOP;
	RefreshBottomBar();
	return KShopImpBase::btnAccountInfo_MouseClick(e);
}

bool KCreditShopImp::handleHelp( const CEGUI::EventArgs& e )
{
	d_shopIndex = enCREDIT_SHOP;
	RefreshBottomBar();
	return KShopImpBase::handleHelp(e);
}

void KCreditShopImp::SetCredit( int credit )
{
	KShopImpBase::SetCredit(credit);
	
	PlayerCreditInfo creditInfo;
	if ( g_pCoreShell->GetGameData(GDI_PLAYER_CREDIT_INFO, (unsigned int)&creditInfo, NULL) > 0 )
	{
		//显示信用点数
		float creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
		char szCreditShow[COMMON_CLIENT_MSG_LEN_32];
		char szMaxCreditShow[COMMON_CLIENT_MSG_LEN_32];
		ZeroMemory(szCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
		ZeroMemory(szMaxCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
		sprintf(szCreditShow, "%.2f",  static_cast<float>(credit) / creditRate);
		sprintf(szMaxCreditShow, "%.2f",  static_cast<float>(creditInfo.uMaxCredit[enCREDIT_SHOP]) / creditRate);

		ostringstream creditShow;
		creditShow<<szCreditShow<<"/"<<szMaxCreditShow;
		d_txtCredit->setText(AnsiToUtf8(creditShow.str().c_str()));
	}
}

void KCreditShopImp::SetMaxCreditPoint( int maxCredit )
{
	KShopImpBase::SetMaxCreditPoint(maxCredit);
	int credit = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enCREDIT_SHOP, NULL);
	//显示信用点数
	float creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
	char szCreditShow[COMMON_CLIENT_MSG_LEN_32];
	char szMaxCreditShow[COMMON_CLIENT_MSG_LEN_32];
	ZeroMemory(szCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
	ZeroMemory(szMaxCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
	sprintf(szCreditShow, "%.2f",  static_cast<float>(credit) / creditRate);
	sprintf(szMaxCreditShow, "%.2f",  static_cast<float>(maxCredit) / creditRate);
	
	ostringstream creditShow;
	creditShow<<szCreditShow<<"/"<<szMaxCreditShow;
	d_txtCredit->setText(AnsiToUtf8(creditShow.str().c_str()));
}

void KCreditShopImp::SetPoint( int point )
{
	KShopImpBase::SetPoint(point);
	d_ConsumeAward->setText(iToString(point));
}

void KCreditShopImp::SetDefaultButtonPos( int current_x )
{
	Point p = Point(current_x, d_defaultFirstBtnPos.d_y);
	d_btnPointShop->setPosition(Absolute, p);
	p.d_x += d_btnPointShop->getWidth(Absolute);
	d_btnAccountInfo->setPosition(Absolute, p);
	p.d_x += d_btnAccountInfo->getWidth(Absolute);
	d_helpBtn->setPosition(Absolute, p);		
}

void KCreditShopImp::LoadPointShopShelf( BYTE* pBuff, int nShelfNum )
{
	if ( NULL == pBuff )
		return;

	Load_IBShelf_Ret* pShelf = (Load_IBShelf_Ret*)pBuff;
	int nShelfIdx = MAX_SHELF_NUM - 1;
	for (int j = 0; j < MAX_PANELCOUNT_PERSHELF; j++)
	{
		d_panelID[nShelfIdx][j] = pShelf->PanelIdxList[j];
	}
	g_pCoreShell->OperationRequest( GOI_IBSHOP_LOAD_SHELF, d_curShelf, d_shopIndex);
}

bool KCreditShopImp::btnBuy_MouseClick( const CEGUI::EventArgs& e )
{
	d_useTicket = false;
	return KShopImpBase::btnBuy_MouseClick(e);
}

void KCreditShopImp::RefreshVoucherCount()
{
	//刷新代金券数量
	int ticketcount = g_pCoreShell->GetGameData(GDI_PLAYER_PRESENT_TICKET_COUNT, NULL, NULL);
	d_txtVoucher->setText(iToString(ticketcount));
}
/********************************************************************
/*						class: KIBShopImp
*********************************************************************/

KIBShopImp::KIBShopImp()
: KShopImpBase()
, d_pJinshanbiTxt(NULL)
, d_jinshanbi(0)
, m_currentSec(0)
{
	d_shopIndex		= enIB_SHOP;
	m_yAxisOffset	= 0;
}

KIBShopImp::~KIBShopImp()
{
	
}

void KIBShopImp::Init( const CEGUI::String& shopName )
{
	KShopImpBase::Init(shopName);

	d_jinshanbi = g_pCoreShell->GetGameData(GDI_PLAYER_JINSHANBI, 0, 0);
	d_pJinshanbiTxt = static_cast<TLStaticText*>(m_pThisWnd->getChild("TaharezLook/IBShop/Jinshanbi"));
	SetJinShanBi(d_jinshanbi);

	static_cast<PushButton*>(m_pThisWnd->getChild("TaharezLook/IBShop/btnCharge"))->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KIBShopImp::handleCharge, this));
}

bool KIBShopImp::handleCharge( const CEGUI::EventArgs& e )
{
	return KShopImpBase::handleCharge(e);
}

void KIBShopImp::SetJinShanBi( DWORD dwMoney )
{
	KShopImpBase::SetJinShanBi(dwMoney);
	char szJinshanbiShow[COMMON_CLIENT_MSG_LEN_32];
	float jinshanbiRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().jinshanbiRate);
	_snprintf(szJinshanbiShow, COMMON_CLIENT_MSG_LEN_32, "%.2f", static_cast<float>(dwMoney) / jinshanbiRate);
	d_pJinshanbiTxt->setText(AnsiToUtf8(szJinshanbiShow));
}

void KIBShopImp::Show()
{
	KShopImpBase::Show();
	d_jinshanbi = g_pCoreShell->GetGameData(GDI_PLAYER_JINSHANBI, 0, 0);
	SetJinShanBi(d_jinshanbi);
}

void KIBShopImp::SetItemButton( TLStaticImage* item )
{
	KShopImpBase::SetItemButton(item);
	item->getChild(item->getName() + "/btnBuy")->setText(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 20)));
}


void KIBShopImp::SeperateBuy()
{
	if ( m_goods.size() > 0 )
	{
		// 购买时间
		DWORD curTime = ::GetTickCount();
		if (curTime - m_currentSec < 2000)
		{
			return;
		}
		else
		{
			m_currentSec = curTime;

			int curGoodsNum = m_goods[m_goods.size() - 1].number;
			m_goods[m_goods.size() - 1].number = 1;
			
			int ret = g_pCoreShell->OperationRequest(GOI_IBSHOP_BUY, (unsigned int)&(m_goods[m_goods.size() - 1]), NULL);	
 			
// 			{
// 				m_goods[m_goods.size() - 1].number = curGoodsNum;
// 			}
// 			else
			if ( enIBShopErr_None == ret )
 			{
				curGoodsNum--;
				if ( curGoodsNum  <= 0 )
				{
					m_goods.pop_back();
				}
				else
				{
					m_goods[m_goods.size() - 1].number = curGoodsNum;
				}
			}
			else
			{
				//购买有错误就清空该物品，不再购买
				m_goods.pop_back();
			}
		}
	}
}

// void KIBShopImp::DoBuy()
// {
// 	if ( -1 == d_curQuantity  )
// 	{
// 		return;
// 	}
// 	int nPanel = d_curPanel;
// 	int nGoodsIdx = d_curGoodsIdx;
// 	IBGoods_Id ibitem = d_itemInfo[nPanel][nGoodsIdx].Id;
// 	
// 	ClientBuyGoods goods;
// 	goods.goods = ibitem;
// 	goods.number = d_curQuantity;
// 	goods.price = 0;
// 	goods.shelfIdx = d_curShelf;
// 	goods.shopIdx = d_shopIndex;
// 	goods.useTicket = false;
// 	goods.bOnecItem = false;
// 
// 	if ( m_goods.size() == 0 )
// 	{
// 		m_currentSec = 0;
// 	}
// 	
// 	m_goods.push_back(goods);
// }

// bool KIBShopImp::btnBuy_MouseClick( const CEGUI::EventArgs& e )
// {
// 	if ( m_goods.size() > 0 )
// 	{
// 		KUiErrorMessageBox::GetSingleton().AddMessage(
// 			AnsiToUtf8( KMessageCentre::GetMessage( ibshop_message, 70) ) );
// 		return false;
// 	}
// 	else
// 	{
// 		return KShopImpBase::btnBuy_MouseClick( e );
// 	}
// 		
//}