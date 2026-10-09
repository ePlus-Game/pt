//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008/03/20
//      File_base        : UiCreditShop
//      File_ext         : cpp
//      Author           : Caolei
//      Description      : 信用商店界面
//
//      <Change_list>
//		1. 2008/03/20, 从IBShop拷贝过来全部代码
//////////////////////////////////////////////////////////////////////

#include "CoreShell.h"
#include "UiCreditShop.h"
#include "UiComMsgBox.h"
#include "../KMessageCentre.h"
#include "../UiConfigManager.h"
#include "UiItemTip.h"
#include "time.h"
#include "UiMapCentre.h"

extern iCoreShell*	g_pCoreShell;

/********************************************************************
/*						class: CreditShop
*********************************************************************/
template<> 
KUiCreditShop* KUiWndSingleton<KUiCreditShop>::ms_Singleton	= NULL;

KUiCreditShop::KUiCreditShop(const CEGUI::String& id_name)
: KUiWndSingleton<KUiCreditShop>( id_name )
{
	imp = NULL;
	imp = new KCreditShopImp();
}

KUiCreditShop::~KUiCreditShop()
{
	if ( NULL != imp )
	{
		delete imp;
		imp = NULL;
	}
}

void KUiCreditShop::Init()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd)
	{
		imp->m_pThisWnd = ms_Singleton->m_pThisWnd;
		imp->m_pWindowManager = ms_Singleton->m_pWindowManager;
		imp->m_pRootSheet = ms_Singleton->m_pRootSheet;
		imp->Init("CreditShop");
	}
}

void KUiCreditShop::Show()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		g_pCoreShell->OperationRequest( GOI_IBSHOP_LOAD_SHELF_CATE, enPRESENT_SHOP, 0 );
		g_pCoreShell->OperationRequest( GOI_IBSHOP_LOAD_SHELF_CATE, enCREDIT_SHOP, 0 );
		
		ms_Singleton->imp->Show();
		KUiWndSingleton<KUiCreditShop>::Show();
	}
}

void KUiCreditShop::Hide()
{
	KUiWndSingleton<KUiCreditShop>::Hide();
}

void KUiCreditShop::SetIBShopMessage(CommonStyle& style, String msg)
{
	imp->SetIBShopMessage(style, msg);
}

void KUiCreditShop::LoadShelf(BYTE* pBuff, int nShelfNum)
{
	imp->LoadShelf(pBuff, nShelfNum);
}

void KUiCreditShop::LoadPanel(BYTE* pBuff, int nCount)
{
	imp->LoadPanel(pBuff, nCount);
}

void KUiCreditShop::LoadStyle(BYTE* pBuff, int nCount)
{
	imp->LoadStyle(pBuff, nCount);

}

void KUiCreditShop::LoadGoodsInShelf(BYTE* pBuff, int nGoodsNum)
{
	imp->LoadGoodsInShelf(pBuff, nGoodsNum);
}


void KUiCreditShop::BuyComfirm()
{
	ms_Singleton->imp->DoBuy();
}

void KUiCreditShop::LoadPointGoodsInShelf( BYTE* pBuff, int nGoodsNum )
{
	imp->LoadGoodsInShelf(pBuff, nGoodsNum);		
}

void KUiCreditShop::SetCredit( int credit )
{
	imp->SetCredit(credit);
}

void KUiCreditShop::SetMaxCreditPoint( int maxCredit )
{
	imp->SetMaxCreditPoint(maxCredit);
}

void KUiCreditShop::SetPoint( int point )
{
	imp->SetPoint(point);
}

void KUiCreditShop::SetCreditStatus( int creditStatus )
{
	imp->SetCreditStatus(creditStatus);
}

void KUiCreditShop::SetCreditReturnTime( DWORD date )
{
	imp->SetCreditReturnTime(date);
}

void KUiCreditShop::SetJinShanBi( DWORD dwMoney )
{
	imp->SetJinShanBi(dwMoney);
}

void KUiCreditShop::LoadPointShopShelf( BYTE* pBuff, int nShelfNum )
{
	imp->LoadPointShopShelf(pBuff, nShelfNum);
}

void KUiCreditShop::RefreshVoucherCount()
{
	imp->RefreshVoucherCount();	
}

void KUiCreditShop::ClearShop()
{
	imp->ClearAll();	
}


/********************************************************************
/*						class: CreditShopNavigation
*********************************************************************/

template<> 
KUiCreditShopNavigation* KUiWndSingleton<KUiCreditShopNavigation>::ms_Singleton	= NULL;

KUiCreditShopNavigation::KUiCreditShopNavigation( const String& id_name )
: KUiWndSingleton<KUiCreditShopNavigation>( id_name )
{
	m_btnOpenShop = NULL;
}

KUiCreditShopNavigation::~KUiCreditShopNavigation()
{
	
}

void KUiCreditShopNavigation::Show( void )
{
	KUiWndSingleton<KUiCreditShopNavigation>::Show();
	if ( NULL != ms_Singleton )
	{
		ms_Singleton->MoveToRightEdge();
	}	
}

void KUiCreditShopNavigation::Init( void )
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		m_btnOpenShop = static_cast<PushButton*>(m_pThisWnd->getChild("TaharezLook/CreditShopNavigation/ShopBtn"));
		if ( NULL != m_btnOpenShop )
		{
			m_btnOpenShop->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiCreditShopNavigation::btnOpenShop_MouseClick, this));
		}
	}
}

void KUiCreditShopNavigation::MoveToRightEdge()
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

void KUiCreditShopNavigation::SetEnable( bool enable )
{
	if ( NULL != ms_Singleton && NULL != m_pThisWnd && NULL != m_btnOpenShop)
	{
		m_btnOpenShop->setEnabled(enable);
	}
}

bool KUiCreditShopNavigation::btnOpenShop_MouseClick( const CEGUI::EventArgs& e )
{
	if( !KUiCreditShop::IsVisible() )
	{
		KUiCreditShop::Show();
	}
	else
	{
		KUiCreditShop::Hide();
	}
	return true;
}