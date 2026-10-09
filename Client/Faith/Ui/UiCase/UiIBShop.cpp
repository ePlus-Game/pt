//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 10/16/2007
//      File_base        : KUiIBShop
//      File_ext         : cpp
//      Author           : Lucien
//      Description      : IB商店界面
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "CoreShell.h"
#include "UiIBShop.h"
#include "UiMapCentre.h"
#include "UiCreditShop.h"
#include "math.h"
#include "../KMessageCentre.h"
#include "../UiConfigManager.h"
#include "UiComMsgBox.h"
#include "../UiConfigManager.h"
#include "UiErrorMessageBox.h"
#include "UiIEWindow.h"
#include "UiQuestionWindow.h"

extern iCoreShell*	g_pCoreShell;

/********************************************************************
/*						class: IBNavigation
*********************************************************************/
template<> 
KUiIBNavigation* KUiWndSingleton<KUiIBNavigation>::ms_Singleton	= NULL;

KUiIBNavigation::KUiIBNavigation(const CEGUI::String& id_name)
: KUiWndSingleton<KUiIBNavigation>( id_name )
, m_btnOpenShop(NULL)
{
}

KUiIBNavigation::~KUiIBNavigation()
{
}

void KUiIBNavigation::Show()
{
	KUiWndSingleton<KUiIBNavigation>::Show();
	if ( NULL != ms_Singleton )
	{
		ms_Singleton->MoveToRightEdge();
	}
}

void KUiIBNavigation::Init()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		m_btnOpenShop = static_cast<PushButton*>(m_pThisWnd->getChild("TaharezLook/IBNavigation/IBShopBtn"));
		if ( NULL != m_btnOpenShop )
		{
			m_btnOpenShop->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiIBNavigation::btnOpenShop_MouseClick, this));
		}
	}
}

bool KUiIBNavigation::btnOpenShop_MouseClick(const CEGUI::EventArgs& e)
{
	if( !KUiIBShop::IsVisible() )
	{
		KUiIBShop::Show();
	}
	else
	{
		KUiIBShop::Hide();
	}	

// 	if( !KUiCreditShop::IsVisible() )
// 	{
// 		KUiCreditShop::Show();
// 	}
// 	else
// 	{
// 		KUiCreditShop::Hide();
// 	}	

	return true;
}

void KUiIBNavigation::MoveToRightEdge()
{
	if ( NULL != ms_Singleton && NULL != m_pThisWnd )
	{
		int selfWidth = m_pThisWnd->getWidth(Absolute);

		int rightEdge_X = g_GetScreenWidth();
		if ( KUiMiniMap::IsVisible() )
		{
			rightEdge_X = KUiMiniMap::GetSingleton().GetMiniMapWndPaintX();	
		}
		int selfPositionY = m_pThisWnd->getYPosition(Absolute);
		Point newPosition(rightEdge_X - selfWidth, selfPositionY);
		m_pThisWnd->setPosition(Absolute, newPosition);
	}
}

void KUiIBNavigation::SetEnable( bool enable )
{
	if ( NULL != ms_Singleton && NULL != m_pThisWnd && NULL != m_btnOpenShop)
	{
		m_btnOpenShop->setEnabled(enable);
	}
}

/********************************************************************
/*						class: IBShop
*********************************************************************/
template<> 
KUiIBShop* KUiWndSingleton<KUiIBShop>::ms_Singleton	= NULL;

KUiIBShop::KUiIBShop(const CEGUI::String& id_name)
: KUiWndSingleton<KUiIBShop>( id_name )
{
	imp = NULL;
	imp = new KIBShopImp();
}

KUiIBShop::~KUiIBShop()
{
	if ( NULL != imp )
	{
		delete imp;
		imp = NULL;
	}
}

void KUiIBShop::Init()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd && NULL != imp)
	{
		imp->m_pThisWnd = ms_Singleton->m_pThisWnd;
		imp->m_pWindowManager = ms_Singleton->m_pWindowManager;
		imp->m_pRootSheet = ms_Singleton->m_pRootSheet;
		imp->Init("IBShop");
	}
}

void KUiIBShop::Show()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd)
	{
		g_pCoreShell->OperationRequest( GOI_IBSHOP_LOAD_SHELF_CATE, ms_Singleton->imp->d_shopIndex, 0 );

		ms_Singleton->imp->Show();
		KUiWndSingleton<KUiIBShop>::Show();
	}
}

void KUiIBShop::Hide()
{
	KUiWndSingleton<KUiIBShop>::Hide();
}

void KUiIBShop::SetIBShopMessage(CommonStyle& style, String msg)
{
	imp->SetIBShopMessage(style, msg);
}

void KUiIBShop::SetJinShanBi(DWORD dwMoney)
{
	imp->SetJinShanBi(dwMoney);
}

void KUiIBShop::LoadShelf(BYTE* pBuff, int nShelfNum)
{
	imp->LoadShelf(pBuff, nShelfNum);
}

void KUiIBShop::LoadPanel(BYTE* pBuff, int nCount)
{
	imp->LoadPanel(pBuff, nCount);
}

void KUiIBShop::LoadStyle(BYTE* pBuff, int nCount)
{
	imp->LoadStyle(pBuff, nCount);
}

void KUiIBShop::LoadGoodsInShelf(BYTE* pBuff, int nGoodsNum)
{
	imp->LoadGoodsInShelf(pBuff, nGoodsNum);
}

void KUiIBShop::BuyComfirm()
{
	ms_Singleton->imp->DoBuy();
}

void KUiIBShop::SetCredit( int credit )
{
	imp->SetCredit(credit);
}

void KUiIBShop::SetMaxCreditPoint( int maxCredit )
{
	imp->SetMaxCreditPoint(maxCredit);
}

void KUiIBShop::SetPoint( int point )
{
	imp->SetPoint(point);
}

void KUiIBShop::SetCreditStatus( int creditStatus )
{
	imp->SetCreditStatus(creditStatus);
}

void KUiIBShop::SetCreditReturnTime( DWORD date )
{
	imp->SetCreditReturnTime(date);
}

void KUiIBShop::Breathe()
{
	if ( (NULL != ms_Singleton) && (NULL != ms_Singleton->imp) )
	{
		ms_Singleton->imp->SeperateBuy();
	}
}

void KUiIBShop::ClearShop()
{
	imp->ClearAll();
}
// void KUiIBShop::SetBuyResult( char* resultMessage )
// {
// 	imp->SetBuyResult(resultMessage);
// }

string Replace(string res, string find, string replace)
{
	if (find.length() > 0)
	{
		string::size_type pos = res.find(find);

		if (pos != string::npos)
		{
			res.replace(pos, find.length(), replace);
		}
	}
	return res;
}

/********************************************************************
/*						class: IBQuantityInput
*********************************************************************/

template<> 
KUiIBQuantityInput* KUiWndSingleton<KUiIBQuantityInput>::ms_Singleton	= NULL;


void KUiIBQuantityInput::Show()
{
	KUiWndSingleton<KUiIBQuantityInput>::Show();
	if ( NULL != ms_Singleton )
	{
		ms_Singleton->ShowQuantity();
	}
}

void KUiIBQuantityInput::Init()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->getChild("TaharezLook/IBQuantityInput/btnOK")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiIBQuantityInput::btnOK_MouseClick, this));
		m_pThisWnd->getChild("TaharezLook/IBQuantityInput/btnCancel")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiIBQuantityInput::btnCancel_MouseClick, this));
		m_pThisWnd->getChild("TaharezLook/IBQuantityInput/btnClose")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiIBQuantityInput::btnCancel_MouseClick, this));
		
		m_pThisWnd->getChild("TaharezLook/IBQuantityInput/btnAdd")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiIBQuantityInput::btnAdd_MouseClick, this));
		m_pThisWnd->getChild("TaharezLook/IBQuantityInput/btnDec")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiIBQuantityInput::btnDec_MouseClick, this));
		
		
		m_EdtQuantity = static_cast<Editbox*>(m_pThisWnd->getChild("TaharezLook/IBQuantityInput/EdtQuantity"));
		m_EdtQuantity->subscribeEvent(Editbox::EventTextChanged, Event::Subscriber(&KUiIBQuantityInput::edtQuantity_TextChanged, this));
		m_isNumberOnly = m_EdtQuantity->isNumberOnly();
		
		KObjAtContRegion* newObjInfo = new KObjAtContRegion();
		newObjInfo->Obj.uGenre = CGOG_NOTHING;
		itemIcon = static_cast<TLGameObject*>(m_pThisWnd->getChild("TaharezLook/IBQuantityInput/ItemIcon"));
		itemIcon->setUserData(newObjInfo);

		m_itemName = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/IBQuantityInput/ItemName"));

		m_itemPrice = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/IBQuantityInput/ItemPrice"));
		m_totalPrice = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/IBQuantityInput/ItemTotalPrice"));
	}
}

KUiIBQuantityInput::KUiIBQuantityInput( const String& id_name )
: KUiWndSingleton<KUiIBQuantityInput>( id_name )
, m_quantity(1)
, m_caller(NULL)
, QuantityInput_CallBack(NULL)
, m_EdtQuantity(NULL)
, m_itemName(NULL)
, itemIcon(NULL)
, m_itemPrice(NULL)
, m_totalPrice(NULL)
, m_price(0)
, m_maxCountLimit(99)
/*, m_successBuy(false)*/
{
	
}

KUiIBQuantityInput::~KUiIBQuantityInput()
{
	if ( NULL != itemIcon )
	{
		KObjAtContRegion* region = static_cast<KObjAtContRegion*>(itemIcon->getUserData());
		if ( NULL != region )
		{
			delete region;
			region = NULL;
		}
	}
}

void KUiIBQuantityInput::SetItem( FIND_ITEMINDEX_PARAM& itemParam, int price, int shopIndex )
{
	m_quantity = 1;
	m_price = price;
	m_shopIndex = shopIndex;
/*	m_successBuy = false;*/
	//int myMoney = 0;

	switch(shopIndex)
	{
	case enIB_SHOP:
		{
			m_unitShow = KMessageCentre::GetMessage(ibshop_message, 13);
			m_myMoney = g_pCoreShell->GetGameData(GDI_PLAYER_JINSHANBI, NULL, NULL);
			m_maxMoney = g_pCoreShell->GetGameData(GDI_MAX_BUY_LIMIT_BY_MONEY_TYPE, enIB_SHOP, NULL);
			m_errCode = enIBShopErr_NoEnoughJinShanBi;
		}
		break;
	case enCREDIT_SHOP:
		{
			m_unitShow = KMessageCentre::GetMessage(ibshop_message, 10);
			m_errCode = enIBShopErr_NoEnoughCreditPoint;
			PlayerCreditInfo creditInfo;
			if ( g_pCoreShell->GetGameData(GDI_PLAYER_CREDIT_INFO, (unsigned int)&creditInfo, NULL) > 0 )
			{
				//显示最大信用点数
				int credits = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enCREDIT_SHOP, NULL);
				
				int maxCredits = creditInfo.uMaxCredit[enCREDIT_SHOP];
				m_myMoney = maxCredits - credits;
			}
			m_maxMoney = g_pCoreShell->GetGameData(GDI_MAX_BUY_LIMIT_BY_MONEY_TYPE, enCREDIT_SHOP, NULL);
		}
	    break;
	case enPRESENT_SHOP:
		{
			m_unitShow = KMessageCentre::GetMessage(ibshop_message, 10);
			m_myMoney = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enPRESENT_SHOP, NULL);
			m_maxMoney = g_pCoreShell->GetGameData(GDI_MAX_BUY_LIMIT_BY_MONEY_TYPE, enPRESENT_SHOP, NULL);
			m_errCode = enIBShopErr_NoEnoughPoint;
		}
		
		break;
	case -1:   //代金券购买
		{
			m_unitShow = KMessageCentre::GetMessage(ibshop_message, 11);
			m_myMoney = g_pCoreShell->GetGameData(GDI_PLAYER_PRESENT_TICKET_COUNT, NULL, NULL);
			int maxCredit = g_pCoreShell->GetGameData(GDI_MAX_BUY_LIMIT_BY_MONEY_TYPE, enCREDIT_SHOP, NULL);
			int rate = g_pCoreShell->GetGameData(GDI_CREDIT_TO_TICKET_RATE, NULL, NULL);
			m_maxMoney = maxCredit / rate;
			m_errCode = enIBShopErr_NoEnoughTicket;
		}
		break;
	default:
		m_unitShow = "";
	    break;
	}

	KItemInfo itemInfo;	
 	g_pCoreShell->GetGameData(/*GDI_ITEM_INFO_PARTICULAR*/GDI_GET_IBITEM_BIG_IMAGE, (unsigned int)&itemParam, (int)&itemInfo);

	TLGameObject::GameObject goInfo;
	goInfo.d_type			= TLGameObject::item;
	goInfo.d_gameobjectSet	= AnsiToUtf8( itemInfo.szImageSet );
	goInfo.d_gameobject		= AnsiToUtf8( itemInfo.szImage );
	goInfo.d_count			= 1;
	itemIcon->setObject( goInfo );
	itemIcon->setZLevel(Window::SuperTop);
	itemIcon->show();
	Image image = ImagesetManager::getSingleton().getImageset(itemInfo.szImageSet)->getImage(itemInfo.szImage);
	itemIcon->setSize(Absolute, image.getSize());
	itemIcon->setTooltipText(AnsiToUtf8(itemInfo.szToolTip));

	KObjAtContRegion* objInfo = static_cast<KObjAtContRegion*>(itemIcon->getUserData());
	objInfo->Obj.uGenre = CGOG_ICON;
	objInfo->Region.h = itemParam.nGenre;
	objInfo->Region.v = itemParam.nDetail;
	objInfo->Region.Width = itemParam.nParticular;
	objInfo->Region.Height = itemParam.nLevel;
	
	m_itemName->setText(AnsiToUtf8(itemInfo.szName));
	char szPriceShow[COMMON_CLIENT_MSG_LEN_32];

	if ( enIB_SHOP == m_shopIndex )
	{
		float jinshanbiRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().jinshanbiRate);
		_snprintf(szPriceShow, COMMON_CLIENT_MSG_LEN_32, "%.2f%s", static_cast<float>(m_price) / jinshanbiRate, m_unitShow.c_str());	
	}
	else if ( enCREDIT_SHOP == m_shopIndex )
	{
		float creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
		_snprintf(szPriceShow, COMMON_CLIENT_MSG_LEN_32, "%.2f%s", static_cast<float>(m_price) / creditRate, m_unitShow.c_str());	
	}
	else if ( -1 == m_shopIndex )
	{
		int rate = g_pCoreShell->GetGameData(GDI_CREDIT_TO_TICKET_RATE, NULL, NULL);
		int totalTicket = m_price / rate + (m_price % rate != 0);
		_snprintf(szPriceShow, COMMON_CLIENT_MSG_LEN_32, "%d%s", totalTicket, m_unitShow.c_str());
	}
	else
	{
		_snprintf(szPriceShow, COMMON_CLIENT_MSG_LEN_32, "%d%s", m_price, m_unitShow.c_str());
	}
	
	//CEGUI::String show = iToString(m_price) + m_unitShow;
	
	m_itemPrice->setText(AnsiToUtf8(szPriceShow));
}

bool KUiIBQuantityInput::btnOK_MouseClick( const CEGUI::EventArgs& e )
{
	//购买数量验证
	if ( m_quantity <= 0 || m_quantity > m_maxCountLimit)
	{
		Hide();
		return false;
	}

	//代金券购买的情况下计算所需代金券数量
	int totalPrice = m_quantity * m_price;
	if ( -1 == m_shopIndex )
	{
		int rate = g_pCoreShell->GetGameData(GDI_CREDIT_TO_TICKET_RATE, NULL, NULL);
		totalPrice = totalPrice / rate + (totalPrice % rate != 0);
	}

	//处理物品的价格超过用户当前可透支信用点数的情况
	//以及玩家处于冻结状态的问题
	if ( enCREDIT_SHOP == m_shopIndex )
	{
		PlayerCreditInfo creditInfo;
		if ( g_pCoreShell->GetGameData(GDI_PLAYER_CREDIT_INFO, (unsigned int)&creditInfo, NULL) > 0 )
		{
			//显示最大信用点数
			//int credits = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enCREDIT_SHOP, NULL);
			
			int maxCredits = creditInfo.uMaxCredit[enCREDIT_SHOP];

			if ( enCreditState_Disable == creditInfo.uCreditState  )
			{
				int creditLevel = g_pCoreShell->GetGameData(GDI_CREDIT_LEVEL_LIMIT, 0, 0);
				char level_waring[COMMON_CLIENT_MSG_LEN_64] = "";
				_snprintf(level_waring, COMMON_CLIENT_MSG_LEN_64 * sizeof(char), KMessageCentre::GetMessage(ibshop_message, 66), creditLevel);
				level_waring[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
				KUiIBShopResultMessage::GetSingleton().SetResultMessage(level_waring);
// 				if ( NULL != QuantityInput_CallBack && NULL != m_caller)
// 				{
// 					(m_caller->*QuantityInput_CallBack)(-1);
// 				}
				return false;
			}
			else if ( enCreditState_Bad == creditInfo.uCreditState )
			{
				KUiIBShopResultMessage::GetSingleton().SetResultMessage(KMessageCentre::GetMessage(ibshop_message, 65));
// 				if ( NULL != QuantityInput_CallBack && NULL != m_caller)
// 				{
// 					(m_caller->*QuantityInput_CallBack)(-1);
// 				}
				return false;
			}
			else if ( totalPrice > maxCredits )
			{
				KUiIBShopResultMessage::GetSingleton().SetResultMessage(KMessageCentre::GetMessage(ibshop_message, 64));
// 				if ( NULL != QuantityInput_CallBack && NULL != m_caller)
// 				{
// 					(m_caller->*QuantityInput_CallBack)(-1);
// 				}
				return false;
			}
		}
	}

	//处理现金不足的情况
	if ( totalPrice > m_myMoney )
	{
		if ( enCREDIT_SHOP == m_shopIndex && m_myMoney <= 0)
		{
			KUiIBShopResultMessage::GetSingleton().SetResultMessage(KMessageCentre::GetMessage(ibshop_message, 58));
		}
		else
		{
			KUiIBShopResultMessage::GetSingleton().SetResultMessage(m_errCode);
		}

// 		if ( NULL != QuantityInput_CallBack && NULL != m_caller)
// 		{
// 			(m_caller->*QuantityInput_CallBack)(-1);
// 		}
		return false;
	}

	//处理背包空间不足的情况
// 	KObjAtContRegion itemBoxRegion;
// 	itemBoxRegion.eContainer = UOC_ITEM_TAKE_WITH;
// 	int nRet = g_pCoreShell->OperationRequest(GOI_FIND_A_EMPTY_PLACE_OF_A_CONTAINER, (UINT)&itemBoxRegion, NULL);
	
// 	if ( nRet <= 0 )
// 	{
// 		//背包空间不足的提示
// 		KUiIBShopResultMessage::GetSingleton().SetResultMessage(KMessageCentre::GetMessage(ibshop_message, 54), enIBShopErr_Unknown);
// 		return false;
// 	}

	//处理消费金额超过最大数量的情况
	if ( totalPrice > m_maxMoney )
	{
		KUiIBShopResultMessage::GetSingleton().SetResultMessage(KMessageCentre::GetMessage(ibshop_message, 56), enIBShopErr_Unknown);
// 		if ( NULL != QuantityInput_CallBack && NULL != m_caller)
// 		{
// 			(m_caller->*QuantityInput_CallBack)(-1);
// 		}
		return false;
	}
	
	if ( NULL != QuantityInput_CallBack && NULL != m_caller)
	{
/*		m_successBuy = true;*/
		Hide();
		(m_caller->*QuantityInput_CallBack)(m_quantity);
	}

	return true;
}

bool KUiIBQuantityInput::btnCancel_MouseClick( const CEGUI::EventArgs& e )
{
	Hide();
	return true;
}

bool KUiIBQuantityInput::btnAdd_MouseClick( const CEGUI::EventArgs& e )
{
	if ( m_EdtQuantity->getMaxTextLength() > 0 && (m_quantity < m_maxCountLimit) && (m_quantity <  static_cast<int>(pow(10, m_EdtQuantity->getMaxTextLength())) - 1) )
	{
		m_quantity++;
		ShowQuantity();
		return true;
	}
	else
	{
		return false;
	}
}

bool KUiIBQuantityInput::btnDec_MouseClick( const CEGUI::EventArgs& e )
{
	if ( m_quantity > 1 )
	{
		m_quantity--;
		ShowQuantity();
		return true;
	}
	else
	{
		return false;
	}
}

void KUiIBQuantityInput::ShowQuantity()
{
	if ( NULL != m_EdtQuantity )
	{
		//显示数量
		CEGUI::String show = iToString(m_quantity);
		m_EdtQuantity->setText(show);

		//显示总价
		int total = m_quantity * m_price;
		char szPriceShow[COMMON_CLIENT_MSG_LEN_32];
		if ( enIB_SHOP == m_shopIndex )
		{
			float jinshanbiRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().jinshanbiRate);
			_snprintf(szPriceShow, COMMON_CLIENT_MSG_LEN_32, "%.2f%s", static_cast<float>(total) / jinshanbiRate, m_unitShow.c_str());	
		}
		else if ( enCREDIT_SHOP == m_shopIndex )
		{
			float creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
			_snprintf(szPriceShow, COMMON_CLIENT_MSG_LEN_32, "%.2f%s", static_cast<float>(total) / creditRate, m_unitShow.c_str());	
		}
		else if ( -1 == m_shopIndex )
		{
			int rate = g_pCoreShell->GetGameData(GDI_CREDIT_TO_TICKET_RATE, NULL, NULL);
			int totalTicket = total / rate + (total % rate != 0);
			_snprintf(szPriceShow, COMMON_CLIENT_MSG_LEN_32, "%d%s", totalTicket, m_unitShow.c_str());
		}
		else
		{
			_snprintf(szPriceShow, COMMON_CLIENT_MSG_LEN_32, "%d%s", total, m_unitShow.c_str());
		}
		
		//show = iToString(m_quantity * m_price)  + m_unitShow;
		m_totalPrice->setText(AnsiToUtf8(szPriceShow));
	}
}

bool KUiIBQuantityInput::edtQuantity_TextChanged( const CEGUI::EventArgs& e )
{
	if ( NULL != m_EdtQuantity && m_isNumberOnly)
	{
		m_quantity = atoi(m_EdtQuantity->getText().c_str());
		if ( m_quantity < 0 )
		{
			m_quantity = 0;
		}
		ShowQuantity();
		return true;
	}
	else
	{
		return false;
	}
}

void KUiIBQuantityInput::SetQuantityInput_CallBack( void (KShopImpBase::*func)(int quantity), KShopImpBase* caller )
{
	QuantityInput_CallBack = func;
	m_caller = caller;
}

void KUiIBQuantityInput::Hide()
{
	KUiWndSingleton<KUiIBQuantityInput>::Hide();

	if ( KUiIBShopResultMessage::GetSingleton().IsVisible() )
	{
		KUiIBShopResultMessage::GetSingleton().Hide();
	}
	if ( KUiRepayConfirm::GetSingleton().IsVisible() )
	{
		KUiRepayConfirm::GetSingleton().Hide();
	}

// 	if ( NULL != ms_Singleton )
// 	{
// 		if ( NULL != ms_Singleton->QuantityInput_CallBack && NULL != ms_Singleton->m_caller && !ms_Singleton->m_successBuy)
// 		{
// 			(ms_Singleton->m_caller->*(ms_Singleton->QuantityInput_CallBack))(-1);
// 		}
// 	}
}

/********************************************************************
/*						class: RepayConfirm
*********************************************************************/

template<> 
KUiRepayConfirm* KUiWndSingleton<KUiRepayConfirm>::ms_Singleton	= NULL;

void KUiRepayConfirm::Init()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->getChild("TaharezLook/RepayConfirm/btnOK")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiRepayConfirm::btnOK_MouseClick, this));
		m_pThisWnd->getChild("TaharezLook/RepayConfirm/btnCancel")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiRepayConfirm::btnCancel_MouseClick, this));
		m_pThisWnd->getChild("TaharezLook/RepayConfirm/btnClose")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiRepayConfirm::btnCancel_MouseClick, this));	
	}
}

bool KUiRepayConfirm::btnOK_MouseClick( const CEGUI::EventArgs& e )
{
	Hide();

	//取得角色等级
// 	KUiPlayerAttribute attr;
// 	ZeroMemory(&attr, sizeof(KUiPlayerAttribute));
// 	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&attr, NULL);
// 	int playerLevel = attr.nLevel;

	PlayerCreditInfo creditInfo;
	if ( g_pCoreShell->GetGameData(GDI_PLAYER_CREDIT_INFO, (unsigned int)&creditInfo, NULL) > 0 )
	{
		//显示最大信用点数
		//int credits = g_pCoreShell->GetGameData(GDI_PLAYER_MONEY_INFO, enCREDIT_SHOP, NULL);
		
		//int maxCredits = creditInfo.uMaxCredit[enCREDIT_SHOP];
		if ( enCreditState_Disable == creditInfo.uCreditState )
		{
			int creditLevel = g_pCoreShell->GetGameData(GDI_CREDIT_LEVEL_LIMIT, 0, 0);
			char level_waring[COMMON_CLIENT_MSG_LEN_64] = "";
			_snprintf(level_waring, COMMON_CLIENT_MSG_LEN_64 * sizeof(char), KMessageCentre::GetMessage(ibshop_message, 66), creditLevel);
			level_waring[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
			KUiIBShopResultMessage::GetSingleton().SetResultMessage(level_waring);
			return false;
		}
		else if ( (enCreditState_Disable != creditInfo.uCreditState) && (0 >= m_credits) )
		{
			KUiIBShopResultMessage::GetSingleton().SetResultMessage(KMessageCentre::GetMessage(ibshop_message, 67));
			return false;		
		}
	}


// 	int creditLevel = g_pCoreShell->GetGameData(GDI_CREDIT_LEVEL_LIMIT, 0, 0);
// 	if ( playerLevel < creditLevel )
// 	{
// 		char level_waring[COMMON_CLIENT_MSG_LEN_64] = "";
// 		_snprintf(level_waring, COMMON_CLIENT_MSG_LEN_64 * sizeof(char), KMessageCentre::GetMessage(ibshop_message, 66), creditLevel);
// 		level_waring[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
// 		KUiIBShopResultMessage::GetSingleton().SetResultMessage(level_waring);
// 		return false;
// 	}


	//判断金山币是否足够还款
	int jinshanBi = g_pCoreShell->GetGameData(GDI_PLAYER_JINSHANBI, NULL, NULL);
	if ( jinshanBi < m_credits )
	{
		KUiIBShopResultMessage::GetSingleton().SetResultMessage(KMessageCentre::GetMessage(ibshop_message, 59));
		return false;
	}

// 	if ( m_credits < 0 )
// 	{
// 		//异常情况
// 		return false;
// 	}
	
	IBGoods_Id ibitem ;
	ibitem.ID = TRADE_ID_RETURN_CREDIT;
	ibitem.Detail = 0;
	ibitem.Genera = 0;
	ibitem.Level = 0;
	ibitem.Particular = 0;
	
	ClientBuyGoods goods;
	goods.goods = ibitem;
	goods.number = 1;
	goods.price = m_credits;
	goods.shelfIdx = 0;
	goods.shopIdx = 0;
	goods.useTicket = 0;
	goods.bOnecItem = true;

	//调用接口
	g_pCoreShell->OperationRequest(GOI_IBSHOP_BUY, (unsigned int)&goods, 0);

	return true;	
}

bool KUiRepayConfirm::btnCancel_MouseClick( const CEGUI::EventArgs& e )
{
	Hide();
	return true;	
}

KUiRepayConfirm::KUiRepayConfirm( const String& id_name )
: KUiWndSingleton<KUiRepayConfirm>( id_name )
{
	
}

KUiRepayConfirm::~KUiRepayConfirm()
{
	
}

void KUiRepayConfirm::SetCredits(int credits)
{
	m_credits = credits;

	float creditRate = static_cast<float>(KUiCfgLoader::getSingleton().getIBShopCfg().creditRate);
	char szCreditShow[COMMON_CLIENT_MSG_LEN_32];
	ZeroMemory(szCreditShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32);
	sprintf(szCreditShow, "%.2f",  static_cast<float>(m_credits) / creditRate);
	m_pThisWnd->getChild("TaharezLook/RepayConfirm/edtRepayPoint")->setText(AnsiToUtf8(szCreditShow));
}

template<> 
KUiIBShopResultMessage* KUiWndSingleton<KUiIBShopResultMessage>::ms_Singleton	= NULL;

void KUiIBShopResultMessage::Init()
{
	if ( NULL != ms_Singleton && NULL != m_pThisWnd )
	{	
		m_txtResultMsg = static_cast<MultiLineEditbox*>(m_pThisWnd->getChild("TaharezLook/IBShopResultMessage/Message"));
		m_btnOK = static_cast<PushButton*>(m_pThisWnd->getChild("TaharezLook/IBShopResultMessage/btnOK"));
		m_btnRepay = static_cast<PushButton*>(m_pThisWnd->getChild("TaharezLook/IBShopResultMessage/btnRepay"));
		m_btnCharge = static_cast<PushButton*>(m_pThisWnd->getChild("TaharezLook/IBShopResultMessage/btnCharge"));
		
		m_pThisWnd->getChild("TaharezLook/IBShopResultMessage/btnCancel")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiIBShopResultMessage::btnCancel_MouseClick, this));
		m_pThisWnd->getChild("TaharezLook/IBShopResultMessage/btnClose")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiIBShopResultMessage::btnCancel_MouseClick, this));	
		
		m_btnOK->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiIBShopResultMessage::btnOK_MouseClick, this));
		m_btnCharge->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiIBShopResultMessage::btnCharge_MouseClick, this));	
		m_btnRepay->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiIBShopResultMessage::btnRepay_MouseClick, this));
	}
}

KUiIBShopResultMessage::KUiIBShopResultMessage( const String& id_name )
: KUiWndSingleton<KUiIBShopResultMessage>( id_name )
, m_txtResultMsg(NULL)
, m_btnCharge(NULL)
, m_btnRepay(NULL)
, m_btnOK(NULL)
{
	
}

KUiIBShopResultMessage::~KUiIBShopResultMessage()
{
	
}

void KUiIBShopResultMessage::SetResultMessage( char* resultMsg, int resultFlag )
{
// 	if ( strlen(resultMsg) <= 0 )
// 	{
// 		switch ( resultFlag )
// 		{
// 		case enIBShopErr_NoEnoughJinShanBi:
// 			resultMsg = KMessageCentre::GetMessage(ibshop_message, 50);
// 			break;
// 		case enIBShopErr_NoEnoughCreditPoint:
// 			resultMsg = KMessageCentre::GetMessage(ibshop_message, 51);
// 			break;
// 		case enIBShopErr_NoEnoughPoint:
// 			resultMsg = KMessageCentre::GetMessage(ibshop_message, 52);
// 			break;
// 		case enIBShopErr_NoEnoughTicket:
// 			resultMsg = KMessageCentre::GetMessage(ibshop_message, 53);
// 			break;
// 		default:
// 			resultMsg = KMessageCentre::GetMessage(ibshop_message, 53);
// 			break;
// 		}
// 	};

	m_txtResultMsg->setText(AnsiToUtf8(resultMsg));
	
	if ( enIBShopErr_NoEnoughCreditPoint == resultFlag )
	{
		//显示立即还款按钮
		m_btnOK->hide();
		m_btnCharge->hide();
		m_btnRepay->show();
	}
	else if ( enIBShopErr_NoEnoughJinShanBi == resultFlag )
	{
		m_btnOK->hide();
		m_btnCharge->show();
		m_btnRepay->hide();
	}
	else
	{
		m_btnOK->show();
		m_btnCharge->hide();
		m_btnRepay->hide();
	}
	KUiWndSingleton<KUiIBShopResultMessage>::Show();
}

void KUiIBShopResultMessage::SetResultMessage( int resultFlag )
{
	char* resultMsg = "";
	switch ( resultFlag )
	{
	case enIBShopErr_NoEnoughJinShanBi:
		resultMsg = KMessageCentre::GetMessage(ibshop_message, 50);
		break;
	case enIBShopErr_NoEnoughCreditPoint:
		resultMsg = KMessageCentre::GetMessage(ibshop_message, 51);
		break;
	case enIBShopErr_NoEnoughPoint:
		resultMsg = KMessageCentre::GetMessage(ibshop_message, 52);
		break;
	case enIBShopErr_NoEnoughTicket:
		resultMsg = KMessageCentre::GetMessage(ibshop_message, 53);
		break;
	default:
		resultMsg = KMessageCentre::GetMessage(ibshop_message, 55);
		break;
	}

	SetResultMessage(resultMsg, resultFlag);
}

void KUiIBShopResultMessage::SetResultMessage( char* resultMsg )
{
	SetResultMessage(resultMsg, enIBShopErr_None);
}
bool KUiIBShopResultMessage::btnCharge_MouseClick( const CEGUI::EventArgs& e )
{
	if ( KUiQuestionWindow::IsVisible() )
	{
		char* tipmsg = KMessageCentre::GetMessage( ibshop_message, 71 );
		if ( NULL != tipmsg )
		{
			KUiErrorMessageBox::GetSingleton().AddMessage( AnsiToUtf8( tipmsg ) );
		}
		
		return false;
	}

	//Edit by DarkMagic(DuanMu)
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
	//ShellExecute(NULL, "open", "http://www.kingsoft.com", NULL, NULL, SW_SHOWNORMAL);
	KUiIEWindow::Show();
	return true;
}

bool KUiIBShopResultMessage::btnRepay_MouseClick( const CEGUI::EventArgs& e )
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

bool KUiIBShopResultMessage::btnOK_MouseClick( const CEGUI::EventArgs& e )
{
	Hide();
	return true;
}

bool KUiIBShopResultMessage::btnCancel_MouseClick( const CEGUI::EventArgs& e )
{
	Hide();
	return true;
}

void KUiIBShopResultMessage::Hide()
{
	KUiWndSingleton<KUiIBShopResultMessage>::Hide();
	if ( KUiIBQuantityInput::GetSingleton().IsVisible() )
	{
		KUiIBQuantityInput::GetSingleton().Hide();
	}
	if ( KUiRepayConfirm::GetSingleton().IsVisible() )
	{
		KUiRepayConfirm::GetSingleton().Hide();
	}
}

void KUiIBShopResultMessage::SetBuySuccessMessage( IBGoods_Id* goods_id )
{
	FIND_ITEMINDEX_PARAM itemidx;
	itemidx.nGenre = goods_id->Genera;
	itemidx.nDetail = goods_id->Detail;
	itemidx.nParticular = goods_id->Particular;
	itemidx.nLevel = goods_id->Level;

	KItemInfo itemInfo;	
	ZeroMemory(&itemInfo, sizeof(KItemInfo));
 	g_pCoreShell->GetGameData(/*GDI_ITEM_INFO_PARTICULAR*/GDI_GET_IBITEM_BIG_IMAGE, (unsigned int)&itemidx, (int)&itemInfo);
	
	char messageShow[COMMON_CLIENT_MSG_LEN_32] = { 0 };
	_snprintf(messageShow, sizeof(char) * COMMON_CLIENT_MSG_LEN_32, KMessageCentre::GetMessage(ibshop_message, 69), itemInfo.szName);
	messageShow[COMMON_CLIENT_MSG_LEN_32 - 1] = 0;
	KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(messageShow));
}

void KUiIBShopResultMessage::SetReturnCreditMessage()
{
	KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(KMessageCentre::GetMessage(ibshop_message, 68)));
}