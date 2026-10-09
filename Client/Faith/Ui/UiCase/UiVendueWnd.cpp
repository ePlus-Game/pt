//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 12/19/2006 17:37
//      File_base        : UiVendueWnd
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "UiVendueWnd.h"

#include "KWin32.h"
#include "UiUpdateTip.h"
#include "UiLogin.h"
#include "KWin32Wnd.h"
#include "../../Login/Login.h"
#include "TLGameObject.h"
#include "CoreShell.h"
#include "UiDragItem.h"
#include "UiItemTip.h"
#include "UiTipGenerator.h"
#include "../KMessageCentre.h"
#include "UiErrorMessageBox.h"
#include "CEGUI.h"
#include "UiComMsgBox.h"
#include "../UiConfigManager.h"
#include "UiChatWindow.h"

extern iCoreShell*		g_pCoreShell;

#define VENDUE_PAGE_TOP_OFFSET			20
#define VENDUE_PAGE_LEFT_OFFSET			180
#define BAR_OFFSET						5

#define	UI_VENDUE_BG					"paimaisuopaimai"
#define	UI_VENDUE_BG_IMAGE_PATH_JS		"\\Ui\\imagesets\\StaticImage\\ItemVendue\\VendueList.spr"
#define	UI_VENDUE_BG_WIDTH				661
#define	UI_VENDUE_BG_HEIGHT				288
#define	UI_VENDUE_BG_X					16
#define	UI_VENDUE_BG_Y					41

#define	UI_VENDUE_BG_LONG				"paimaisuo"
#define	UI_VENDUE_BG_LONG_IMAGE_PATH_JS	"\\Ui\\imagesets\\StaticImage\\ItemVendue\\VendueShowBar.spr"
#define	UI_VENDUE_BG_WIDTH_LONG			661
#define	UI_VENDUE_BG_HEIGHT_LONG		288
#define	UI_VENDUE_BG_LONG_X				16
#define	UI_VENDUE_BG_LONG_Y				41

#define	UI_VENDUE_BG_JINGBIAO			"paimaisuojingbiao"

#define ITEM_COLUMN_HEIGHT				36
#define ITEM_COLUMN_JIPAI_NAME_WIDTH	154
#define ITEM_COLUMN_JIPAI_TIME_WIDTH	70
#define ITEM_COLUMN_JIPAI_LEVEL_WIDTH	76
#define ITEM_COLUMN_JIPAI_OWNER_WIDTH	128
#define ITEM_COLUMN_JIPAI_PRICE_WIDTH	214
#define INCREASE_GOLD					10

#define MAX_VENDUE_TIME_LIMIT			48
#define ONE_MINUTE						60
#define	TIME_LIMIT						500
using namespace CEGUI;



/************************************************************************/
/*                                                                      */
/************************************************************************/

KUiVenduePage::KUiVenduePage()
: m_pItemName(NULL)
, m_pItemLevel(NULL)
, m_pItemOwner(NULL)
, m_pItemPrice(NULL)
, m_pItemTime(NULL)
, m_ItemNameSize( 0, 0 )
, m_ItemLevelSize( 0, 0 )
, m_ItemTimeSize( 0, 0 )
, m_ItemOwnerSize( 0, 0 )
, m_ItemPriceSize( 0, 0 )
, m_showCompare(false)
{

}

KUiVenduePage::~KUiVenduePage()
{
	//likun
	if ( m_pItemName != NULL)
	{
		WindowManager::getSingleton().destroyWindow(m_pItemName);
	}

	if ( m_pItemLevel != NULL )
	{
		WindowManager::getSingleton().destroyWindow(m_pItemLevel);
	}

	if ( m_pItemOwner != NULL )
	{
		WindowManager::getSingleton().destroyWindow(m_pItemOwner);
	}

	if ( m_pItemPrice != NULL )
	{
		WindowManager::getSingleton().destroyWindow(m_pItemPrice);
	}
}

void KUiVenduePage::initPage( int index )
{
	LoadConfig();
	InitItemColumn(index);
	char szName[COMMON_CLIENT_MSG_LEN_256];
	for ( int nIdx = 0; nIdx < MAXRECORDS_PER_PAGE; ++nIdx )
	{
		sprintf( szName, "%s_%d_", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );
		Window* pGO = m_pWindowManager->loadWindowLayout( UI_ITEMVENDUEBAR, szName, "", NULL, NULL, true  );
		if ( pGO )
		{
			Point absPoint;
			Size  absSize	= m_pThisWnd->getAbsoluteSize(); 
			absPoint.d_x = 0; //VENDUE_PAGE_LEFT_OFFSET;
			absPoint.d_y = nIdx * (GAMEOBJECT_WIDTH_MID + BAR_OFFSET) + VENDUE_PAGE_TOP_OFFSET;
			absSize.d_height = GAMEOBJECT_WIDTH_MID + BAR_OFFSET + 1;
			absSize.d_width = UI_VENDUE_BG_WIDTH - 15;
			m_pThisWnd->addChildWindow( pGO );
			pGO->setSize( Absolute, absSize );
			pGO->setPosition( Absolute, absPoint );
			pGO->setZLevel(Window::Top);
			pGO->subscribeEvent(StaticImage::EventMouseClick, Event::Subscriber(&KUiVenduePage::selectItem, this));
			KItemBar Bar;
			Bar.pPar = pGO;
			Bar.dwItemID = -1;
			m_itemBarList.push_back( Bar );
			sprintf( szName, "%d", nIdx );
			pGO->setUserString("idx", szName );
			sprintf( szName, "%s_%d_TaharezLook/itemvenduebar/itemicon", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );
			pGO->getChild(szName)->subscribeEvent(TLGameObject::EventMouseEnters, Event::Subscriber(&KUiVenduePage::onMouseEnter, this));
			pGO->getChild(szName)->subscribeEvent(TLGameObject::EventObjectChanged, Event::Subscriber(&KUiVenduePage::onObjectChange, this));
			pGO->getChild(szName)->subscribeEvent(TLGameObject::EventMouseMove, Event::Subscriber(&KUiVenduePage::onMouseMove, this));
			pGO->getChild(szName)->subscribeEvent(TLGameObject::EventMouseLeaves, Event::Subscriber(&KUiVenduePage::onMouseLeave, this));
			InitItemSelectHandle(pGO,nIdx);
		}
	}	
}

void KUiVenduePage::LoadConfig()
{
	d_overTime[vendue_over_time_verylong]	= KUiCfgLoader::getSingleton().getAuctionCfg().timeVeryLong;
	d_overTime[vendue_over_time_long]		= KUiCfgLoader::getSingleton().getAuctionCfg().timeLong;
	d_overTime[vendue_over_time_middle]		= KUiCfgLoader::getSingleton().getAuctionCfg().timeMiddle;
	d_overTime[vendue_over_time_short]		= KUiCfgLoader::getSingleton().getAuctionCfg().timeShort;
	d_overTime[vendue_over_time_veryshort]	= KUiCfgLoader::getSingleton().getAuctionCfg().timeVeryShort;
}

void KUiVenduePage::updateBar( int nIdx, void *pBuff )
{
	SEARCH_DB_RETDATA* pRecord  = (SEARCH_DB_RETDATA*)pBuff;
	char szName[COMMON_CLIENT_MSG_LEN_128];
	sprintf( szName, "%s_%d_TaharezLook/itemvenduebar", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );
	Window* pGO = m_pThisWnd->getChild( szName );
	if ( pRecord && pGO )
	{
		sprintf( szName, "%s_%d_TaharezLook/itemvenduebar/itemicon", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );
		
		TLGameObject* pGameObj = (TLGameObject*)pGO->getChild(szName);
		KItemInfo tagItemInfo;
		ZeroMemory( &tagItemInfo, sizeof( KItemInfo ) );
		FIND_ITEMINDEX_PARAM tagItemIdx;
		ZeroMemory( &tagItemIdx, sizeof(FIND_ITEMINDEX_PARAM) );

		tagItemIdx.nGenre = pRecord->itemData.igenre;
		tagItemIdx.nDetail	= pRecord->itemData.idetailtype;
		tagItemIdx.nParticular	= pRecord->itemData.iparticulartype;
		tagItemIdx.nLevel	= pRecord->itemData.ilevel;	
	
		g_pCoreShell->GetGameData( GDI_ITEM_INFO_PARTICULAR, (unsigned int)&tagItemIdx, (int)&tagItemInfo );
		if ( pRecord->recordComData.currentPrice != 0 ||
			pRecord->recordComData.onePrice != 0)
		{
			TLGameObject::GameObject tagGO;
			tagGO.d_type = TLGameObject::item;		
			tagGO.d_gameobjectSet = AnsiToUtf8( tagItemInfo.szImageSet );
			tagGO.d_gameobject	= AnsiToUtf8( tagItemInfo.szImage );
			tagGO.d_count		= pRecord->itemData.nItemCount;
			tagGO.d_EdgeframeIdx	= tagItemInfo.colour;
			pGameObj->setObject( tagGO );
			pGameObj->setTooltipText( tagItemInfo.szToolTip );

			// 物品名称
			char szBuff[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szName, "%s_%d_TaharezLook/itemvenduebar/itemname", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );		
			TLStaticText* pName = static_cast<TLStaticText*>(pGO->getChild(szName));
			char tempName[COMMON_CLIENT_MSG_LEN_128];
			//pGO->getChild(szName)->setText( AnsiToUtf8( tagItemInfo.szName ));
			ZeroMemory(tempName, COMMON_CLIENT_MSG_LEN_128);
			g_pCoreShell->GetGameData( GDI_GET_ITEM_NAME_WITH_COLOR_BY_INDEXPARAM, (unsigned int)&tagItemIdx, (unsigned int)tempName );
			char itemName[COMMON_CLIENT_MSG_LEN_256];
			ZeroMemory(itemName, COMMON_CLIENT_MSG_LEN_128);
			strcpy(itemName, "<Layout width=100><Seg text-align=left float=wrap>");
			strcat(itemName, tempName);
			strcat(itemName, "</Seg></Layout>");
			pName->useLayout();
			pName->getLayout()->SetText(itemName);
			pName->getLayout()->flashLayout();
			
			// 时间
			sprintf( szName, "%s_%d_TaharezLook/itemvenduebar/itemlefttime", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx);
			int	iLeftTime = static_cast<int>(pRecord->recordComData.totalValidTime)/ONE_MINUTE;
			char *message = NULL;
			if ( iLeftTime > d_overTime[vendue_over_time_long] /*&& iLeftTime <= d_overTime[vendue_over_time_verylong]*/)
			{
				message = KMessageCentre::GetMessage(vendue_message, 27);
			}
			else if ( iLeftTime > d_overTime[vendue_over_time_middle] &&
					  iLeftTime <= d_overTime[vendue_over_time_long] )
			{
				message = KMessageCentre::GetMessage(vendue_message, 28);
			}
			else if ( iLeftTime > d_overTime[vendue_over_time_short] &&
					  iLeftTime <= d_overTime[vendue_over_time_middle] )
			{
				message = KMessageCentre::GetMessage(vendue_message, 29);
			}
			else if ( iLeftTime > d_overTime[vendue_over_time_veryshort] &&
					  iLeftTime <= d_overTime[vendue_over_time_short] )
			{
				message = KMessageCentre::GetMessage(vendue_message, 30);
			}
			else if ( iLeftTime <= d_overTime[vendue_over_time_veryshort] )
			{
				message = KMessageCentre::GetMessage(vendue_message, 31);
			}
			TLStaticText* pTime = static_cast<TLStaticText*>(pGO->getChild(szName));
			pTime->setText( AnsiToUtf8( const_cast<char *>(message) ));
			
			// 等级
			sprintf( szName, "%s_%d_TaharezLook/itemvenduebar/itemlevel", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx);
			if ( tagItemInfo.iReqLevel < 0 )
			{
				tagItemInfo.iReqLevel = 0;
			}
			TLStaticText* pLevel = static_cast<TLStaticText*>(pGO->getChild(szName));
			pLevel->setText( AnsiToUtf8( itoa( tagItemInfo.iReqLevel, szBuff, 10 )));

			// 卖者姓名
			sprintf( szName, "%s_%d_TaharezLook/itemvenduebar/itemowner", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx);
			TLStaticText* pSeller = static_cast<TLStaticText*>(pGO->getChild(szName));
			pSeller->setText( AnsiToUtf8( pRecord->sellerName ));

			// 一口价
			sprintf(szName, "%s_%d_TaharezLook/itemvenduebar/itemprice", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx);
			Window* pPriceWin = pGO->getChild(szName);
			int yihouj, yihouy, yihout;
			sysMoneyToUiMoney( pRecord->recordComData.onePrice, yihouj, yihouy, yihout );
			sprintf( szName, "%s/yikouj", Utf8ToAnsi( pPriceWin->getName() ), nIdx );
			pPriceWin->getChild( szName )->setText( iToString(yihouj) );
			sprintf( szName, "%s/yikouy", Utf8ToAnsi( pPriceWin->getName() ), nIdx );
			pPriceWin->getChild( szName )->setText( iToString(yihouy) );
			sprintf( szName, "%s/yikout", Utf8ToAnsi( pPriceWin->getName() ), nIdx );
			pPriceWin->getChild( szName )->setText( iToString(yihout) );

			// 当前价格
			int jingpaij, jingpaiy, jingpait;
			sysMoneyToUiMoney( pRecord->recordComData.currentPrice, jingpaij, jingpaiy, jingpait );
			sprintf( szName, "%s/jingpaij", Utf8ToAnsi( pPriceWin->getName() ), nIdx );
			pPriceWin->getChild( szName )->setText( iToString(jingpaij) );
			sprintf( szName, "%s/jingpaiy", Utf8ToAnsi( pPriceWin->getName() ), nIdx );
			pPriceWin->getChild( szName )->setText( iToString(jingpaiy) );
			sprintf( szName, "%s/jingpait", Utf8ToAnsi( pPriceWin->getName() ), nIdx );
			pPriceWin->getChild( szName )->setText( iToString(jingpait) );

			KItemBar Bar;
			Bar.pPar = pGO;
			Bar.dwItemID = pRecord->recordId;
			m_itemBarList[nIdx] = Bar;
			KUiVendueWnd::m_iCurrentBarNum++;
			pGO->show();
		
			// 放置单位位置
			Point	pos(0, 0);
			Size	size(0, 0);
			const int	nHoverLength = 15;
			sprintf(szName, "%s_%d_TaharezLook/itemvenduebarhover", Utf8ToAnsi(m_pThisWnd->getName()), nIdx);
			Window*	pHover = pGO->getChild(szName);

			Point absPoint	= pGO->getAbsolutePosition();
			Size  absSize(UI_VENDUE_BG_WIDTH - 15, GAMEOBJECT_WIDTH_MID + BAR_OFFSET + 1);
			absPoint.d_x = 0;

			if (KUiVendueWnd::GetSingleton().getCurPage() == KUiVendueWnd::jingpai_page)
			{
				pos.d_x = GAMEOBJECT_WIDTH_MID/2 - 3;
				size = pHover->getSize(Absolute);
				size.d_width = UI_VENDUE_BG_WIDTH - pos.d_x - nHoverLength;
				pHover->setSize(Absolute, size);
				pHover->setPosition(Absolute, pos);

				pos = pGameObj->getPosition(Absolute);
				pos.d_x = GAMEOBJECT_WIDTH_MID/2;
				pGameObj->setPosition(Absolute, pos);
				
				pos = pName->getPosition(Absolute);
				pos.d_x = GAMEOBJECT_WIDTH_MID/2 + GAMEOBJECT_WIDTH_MID + 5;
				pName->setPosition(Absolute, pos);

				pos = pLevel->getPosition(Absolute);
				pos.d_x = ITEM_COLUMN_JIPAI_NAME_WIDTH + \
					ITEM_COLUMN_JIPAI_LEVEL_WIDTH/2 - \
					pLevel->getWidth(Absolute)/2;
				pLevel->setPosition(Absolute, pos);

				pos = pTime->getPosition(Absolute);
				pos.d_x = ITEM_COLUMN_JIPAI_NAME_WIDTH + \
					ITEM_COLUMN_JIPAI_LEVEL_WIDTH + \
					ITEM_COLUMN_JIPAI_TIME_WIDTH/2 - \
					pTime->getWidth(Absolute)/2;
				pTime->setPosition(Absolute, pos);

				pos = pSeller->getPosition(Absolute);
				pos.d_x = ITEM_COLUMN_JIPAI_NAME_WIDTH + \
					ITEM_COLUMN_JIPAI_LEVEL_WIDTH + \
					ITEM_COLUMN_JIPAI_TIME_WIDTH + \
					ITEM_COLUMN_JIPAI_OWNER_WIDTH/2 - \
					pSeller->getWidth(Absolute)/2;
				pSeller->setPosition(Absolute, pos);

				pos = pPriceWin->getPosition(Absolute);
				pos.d_x = ITEM_COLUMN_JIPAI_NAME_WIDTH + \
					ITEM_COLUMN_JIPAI_LEVEL_WIDTH + \
					ITEM_COLUMN_JIPAI_TIME_WIDTH + \
					ITEM_COLUMN_JIPAI_OWNER_WIDTH + \
					ITEM_COLUMN_JIPAI_PRICE_WIDTH/2 - \
					pPriceWin->getWidth(Absolute)/2;
				pPriceWin->setPosition(Absolute, pos);				
			}
			else
			{
				int offset_x = 3;
				absPoint.d_x = VENDUE_PAGE_LEFT_OFFSET - offset_x;
				absSize.d_width = UI_VENDUE_BG_WIDTH - VENDUE_PAGE_LEFT_OFFSET - 15;
				
				pos.d_x = 0;
				size = pHover->getSize(Absolute);
				size.d_width = absSize.d_width - offset_x;
				pHover->setSize(Absolute, size);
				pHover->setPosition(Absolute, pos);

				pos = pGameObj->getPosition(Absolute);
				pos.d_x = offset_x;
				pGameObj->setPosition(Absolute, pos);
				
				pos = pName->getPosition(Absolute);
				pos.d_x = offset_x + pGameObj->getWidth(Absolute) + 2;
				pName->setPosition(Absolute, pos);

				pos = pLevel->getPosition(Absolute);
				pos.d_x = offset_x + pGameObj->getWidth(Absolute) + \
					pName->getWidth(Absolute);
				pLevel->setPosition(Absolute, pos);

				pos = pTime->getPosition(Absolute);
				pos.d_x = offset_x + pGameObj->getWidth(Absolute) + \
					pName->getWidth(Absolute) + pLevel->getWidth(Absolute);
				pTime->setPosition(Absolute, pos);

				pos = pSeller->getPosition(Absolute);
				pos.d_x = offset_x + pGameObj->getWidth(Absolute) + \
					pName->getWidth(Absolute) + pLevel->getWidth(Absolute) + \
					pTime->getWidth(Absolute);
				pSeller->setPosition(Absolute, pos);

				pos = pPriceWin->getPosition(Absolute);
				pos.d_x = offset_x + pGameObj->getWidth(Absolute) + \
					pName->getWidth(Absolute) + pLevel->getWidth(Absolute) + \
					pTime->getWidth(Absolute) +	pSeller->getWidth(Absolute);
				pPriceWin->setPosition(Absolute, pos);
			}

			pGO->setSize( Absolute, absSize );
			pGO->setPosition( Absolute, absPoint );
		}
	}
}

void KUiVenduePage::clearBarDate()
{
	_itemlist::iterator it = m_itemBarList.begin();
	while ( it != m_itemBarList.end() )
	{
		(*it).pPar->hide();
		(*it).dwItemID = -1;
		++it;
	}
}

void KUiVenduePage::clearSelectItem()
{
	for ( int nIdx = 0; nIdx < MAXRECORDS_PER_PAGE; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_128];
		sprintf( szName, "%s_%d_TaharezLook/itemvenduebar", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );
		Window* pItem = m_pThisWnd->getChild( szName );

		sprintf( szName, "%s_%d_TaharezLook/itemvenduebarhover", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );
		pItem->getChild(szName)->setVisible( false );
	}
}

bool	KUiVenduePage::selectItem( const CEGUI::EventArgs& args )
{
	MouseEventArgs* pEvent = (MouseEventArgs*)&args;
	for ( int nIdx = 0; nIdx < MAXRECORDS_PER_PAGE; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_128];
		sprintf( szName, "%s_%d_TaharezLook/itemvenduebar", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );
		Window* pGO = m_pThisWnd->getChild( szName );
		sprintf( szName, "%s_%d_TaharezLook/itemvenduebar/itemicon", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );		
		TLGameObject* pGameObj = (TLGameObject*)pGO->getChild(szName);

		String fchildName = pGO->getName();
		String fparentName = pEvent->window->getParent()->getName();
		if ( pGO != pEvent->window && fchildName != fparentName )
		{
			sprintf( szName, "%s_%d_TaharezLook/itemvenduebarhover", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );
			pGO->getChild(szName)->setVisible( false );
		}
		else
		{
			sprintf( szName, "%s_%d_TaharezLook/itemvenduebarhover", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );
			pGO->getChild(szName)->setVisible( true );
			pGO->getChild(szName)->setEnabled( false );
			m_iCurrentHeightItem = nIdx;
		}
		pGameObj->setZLevel(Window::Top);
		pGameObj->show();
	}	

	if ( pEvent )
	{
		Window* pBar = pEvent->window;
		if ( pBar )
		{
			CEGUI::String str = pBar->getUserString( "idx" );
			int nIdx = atoi( Utf8ToAnsi(str) );
			KUiVendueWnd::GetSingletonPtr()->setSelectItemID( m_itemBarList[nIdx].dwItemID );
			
			// 得到竞拍价
			char szName[COMMON_CLIENT_MSG_LEN_128];
			ZeroMemory(szName, COMMON_CLIENT_MSG_LEN_128);	
			sprintf( szName, "%s_%d_TaharezLook/itemvenduebar", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx );
			Window* pGO = m_pThisWnd->getChild( szName );

			sprintf(szName, "%s_%d_TaharezLook/itemvenduebar/itemprice", Utf8ToAnsi( m_pThisWnd->getName() ), nIdx);
			Window* pPriceWin = pGO->getChild(szName);
			sprintf( szName, "%s/jingpaij", Utf8ToAnsi(pPriceWin->getName()) );
			String jing = pPriceWin->getChild( szName )->getText();
			sprintf( szName, "%s/jingpaiy", Utf8ToAnsi(pPriceWin->getName()) );
			String yin = pPriceWin->getChild( szName )->getText();
			sprintf( szName, "%s/jingpait", Utf8ToAnsi(pPriceWin->getName()) );
			String tong = pPriceWin->getChild( szName )->getText();

			KUiVendueWnd::GetSingletonPtr()->CopyJingPaiJia(jing, yin, tong);

		}
	}
	return true;
}

bool	KUiVenduePage::onMouseMove(const CEGUI::EventArgs& e)
{
	m_showCompare = true;
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	TLGameObject* destObj = (TLGameObject*)arg->window;
	if ( destObj )
	{
		showTip(destObj);
	}
	return true;
}

bool	KUiVenduePage::onMouseLeave(const CEGUI::EventArgs& e)
{
	m_showCompare = false;
	KUiItemTip::Hide();
	return true;
}

bool	KUiVenduePage::onMouseEnter(const CEGUI::EventArgs &e )
{
	m_showCompare = true;
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	TLGameObject* destObj = (TLGameObject*)arg->window;
	if ( destObj )
	{
		showTip(destObj);
	}
	return true;
}

bool	KUiVenduePage::onObjectChange(const CEGUI::EventArgs &e )
{
	return true;
}

bool	KUiVenduePage::showTip(TLGameObject* pGameObject)
{
	if ( NULL == pGameObject )
		return false;

	KUiTipGenerator::TipObject tipObj;
	Window* pBar = pGameObject->getParent();
	if ( pBar )
	{
	
		CEGUI::String str = pBar->getUserString( "idx" );
		int nIdx = atoi( Utf8ToAnsi(str) );
		
		//获取排版字符串
		tipObj.type = KUiTipGenerator::VendueItem;
		tipObj.ids[0] = m_itemBarList[nIdx].dwItemID;
	}

	char* layoutText = KUiTipGenerator::getSinglton().genLayoutDes(tipObj);
	KUiItemTip::GetSingleton();
	KUiItemTip::GetSingleton().show(layoutText, pGameObject->getUnclippedInnerRect());

	if(m_showCompare)
	{
		char* compareLayoutText = KUiTipGenerator::getSinglton().genCompareLayoutDes(tipObj);
		KUiItemTip::GetSingleton().showCompare(compareLayoutText);
	}
	return true;
}

void	KUiVenduePage::InitItemColumn( int index )
{
	//likun
	char szName[COMMON_CLIENT_MSG_LEN_256];
	//增加本页中物品名列名
	sprintf( szName, "%s%s", Utf8ToAnsi( m_pThisWnd->getName() ),  "/itemname");
	m_pItemName = reinterpret_cast<TLStaticText *>( WindowManager::getSingleton().getWindow(szName));
	if ( m_pItemName != NULL )
	{
		m_pThisWnd->addChildWindow(m_pItemName);
	}
	m_ItemNameSize = Size( m_pItemName->getWidth(Absolute), m_pItemName->getHeight(Absolute) );
	//增加本页中物品等级列名
	sprintf( szName, "%s%s", Utf8ToAnsi( m_pThisWnd->getName() ),  "/itemlevel");
	m_pItemLevel = reinterpret_cast<TLStaticText *>( WindowManager::getSingleton().getWindow(szName));
	if ( m_pItemLevel != NULL )
	{
		m_pThisWnd->addChildWindow(m_pItemLevel);
	}
	m_ItemLevelSize = Size( m_pItemLevel->getWidth(Absolute), m_pItemLevel->getHeight(Absolute)) ;
	//增加本页中物品时效列名
	sprintf( szName, "%s%s", Utf8ToAnsi( m_pThisWnd->getName() ),  "/itemlefttime");
	m_pItemTime = reinterpret_cast<TLStaticText *>( WindowManager::getSingleton().getWindow(szName));
	if ( m_pItemTime != NULL )
	{
		m_pThisWnd->addChildWindow(m_pItemTime);
	}
	m_ItemTimeSize = Size( m_pItemTime->getWidth(Absolute), m_pItemTime->getHeight(Absolute)) ;

	//增加本页中物品拥有者列名
	sprintf( szName, "%s%s", Utf8ToAnsi( m_pThisWnd->getName() ),  "/itemowner");
	m_pItemOwner = reinterpret_cast<TLStaticText *>( WindowManager::getSingleton().getWindow(szName));
	if ( m_pItemOwner != NULL )
	{
		m_pThisWnd->addChildWindow(m_pItemOwner);
	}
	m_ItemOwnerSize = Size( m_pItemOwner->getWidth(Absolute), m_pItemOwner->getHeight(Absolute)) ;

	//增加本页中物品价格列名
	sprintf( szName, "%s%s", Utf8ToAnsi( m_pThisWnd->getName() ),  "/itemprice");
	m_pItemPrice = reinterpret_cast<TLStaticText *>( WindowManager::getSingleton().getWindow(szName));
	if ( m_pItemPrice != NULL )
	{
		m_pThisWnd->addChildWindow(m_pItemPrice);
	}
	m_ItemPriceSize = Size( m_pItemPrice->getWidth(Absolute), m_pItemPrice->getHeight(Absolute)) ;
}

//likun 初始化itembar中所有控件被点中时触发的事件
void    KUiVenduePage::InitItemSelectHandle( CEGUI::Window *pWindow, int iIndex )
{
	pWindow->subscribeEvent(TLStaticImage::EventMouseClick, Event::Subscriber(&KUiVenduePage::selectItem, this));
	/*InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemname" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/yikoujtxt" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/yikouj_img" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/yikouy_img" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/yikout_img" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/yikouj" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/yikouy" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/yikout" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/jingpaitxt" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/jingpaij_img" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/jingpaiy_img" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/jingpait_img" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/jingpaij" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/jingpaiy" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/itemprice/jingpait" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/venduername" );
	InitItemChildHandle( pWindow, iIndex, "TaharezLook/itemvenduebar/vendueditemlv" );//*/
}

//likun 初始化itembar中一个控件被点中时触发的事件
void	KUiVenduePage::InitItemChildHandle( CEGUI::Window *pWindow, int iIndex, const char *pCtlName )
{
	char szName[COMMON_CLIENT_MSG_LEN_128];
	char szUseName[COMMON_CLIENT_MSG_LEN_128];
	sprintf( szName, "%s_%d_%s", Utf8ToAnsi( m_pThisWnd->getName() ), iIndex, pCtlName );
	pWindow->getChild(szName)->subscribeEvent(TLStaticImage::EventMouseClick, Event::Subscriber(&KUiVenduePage::selectItem, this));
	sprintf( szUseName, "%d", iIndex );
	pWindow->getChild(szName)->setUserString("idx", szUseName );
}

//取消当前高亮的item项
void	KUiVenduePage::CancelCurHeight()
{
	char szName[COMMON_CLIENT_MSG_LEN_128];
	sprintf( szName, "%s_%d_TaharezLook/itemvenduebar", Utf8ToAnsi( m_pThisWnd->getName() ), m_iCurrentHeightItem );
	Window* pGO = m_pThisWnd->getChild( szName );
	sprintf( szName, "%s_%d_TaharezLook/itemvenduebarhover", Utf8ToAnsi( m_pThisWnd->getName() ), m_iCurrentHeightItem );
	pGO->getChild(szName)->setVisible( false );
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
int KUiVendueWnd::m_iCurrentBarNum = 0;

template<> 
KUiVendueWnd* KUiWndSingleton<KUiVendueWnd>::ms_Singleton	= NULL;

KUiVendueWnd::KUiVendueWnd( const CEGUI::String& id_name )
: KUiWndSingleton<KUiVendueWnd>( id_name )
, d_pUseAbleChckBtn(NULL)
, d_bUseAble(false)
, d_pGoldCoin(NULL)
, d_pSillerCoin(NULL)
, d_pCopperCoin(NULL)
, d_pWeaponLevelHeight(NULL)
, d_pWeaponLevelLow(NULL)
, d_pSaleTimeLimit0(NULL)
, d_pSaleTimeLimit1(NULL)
, d_pSaleTimeLimit2(NULL)
, d_pBuyOnceGold(NULL)
, d_pBuyOnceSlive(NULL)
, d_pBuyOnceCopper(NULL)
, d_pBuyGold(NULL)
, d_pBuySlive(NULL)
, d_pBuyCopper(NULL)
, d_pEquipQulityBtn(NULL)
, d_pEquipMenu(NULL)
, d_pCommonEquipment(NULL)
, d_pRareEquipment(NULL)
, d_pSetEquipment(NULL)
, d_pEpicEquipment(NULL)
, d_pArtificialOne(NULL)
//, d_pArtificialTwo(NULL)
//, d_pArtificialThree(NULL)
, d_pBrowse(NULL)
, d_pReqAllKind(NULL)
, d_pReqEquipment(NULL)
, d_pReqPotion(NULL)
, d_pReqMaterial(NULL)
, d_pReqGem(NULL)
, d_pReqOthers(NULL)
, d_pEquipAllKind(NULL)
, d_pEquipToukui(NULL)
, d_pEquipYifu(NULL)
, d_pEquipHujian(NULL)
, d_pEquipXuezi(NULL)
, d_pEquipWuqi(NULL)
, d_pEquipZhuishi(NULL)
, d_pEquipYupei(NULL)
, d_pEquipJiezhi(NULL)
, d_pEquipHuwan(NULL)
, d_pProfessionBtn(NULL)
, d_pProfessionMenu(NULL)
, d_pProfessionCommon(NULL)
, d_pProfessionXF(NULL)
, d_pProfessionXT(NULL)
, d_pProfessionTS(NULL)
, d_pProfessionZR(NULL)
, d_pProfessionSS(NULL)
, d_pProfessionYS(NULL)
, d_iProf(-1)
, d_pKindTypeTxt(NULL)
, d_pQualityTypeTxt(NULL)
, d_pProfTypeTxt(NULL)
, d_pEquipmentTypeTxt(NULL)
, d_iEquipQulity(0)
, d_iSearchTimeLimit(0)
, d_pEquipBtn(NULL)
, d_pEquipMenuBack(NULL)
, d_pTaxGold(NULL)
, d_pTaxSlive(NULL)
, d_pTaxCopper(NULL)
{
	m_curSelectItemID	= -1;
	m_nPageCount		= 1;
	m_nSaleTime			= ONE_HOUR;
	ZeroMemory( &d_buyInfo, sizeof( CLIENT_BUYGOODS_REQDATA ) );
	ZeroMemory( d_ReqKind, sizeof(AUCTION_MAX_ITEM_TYPE_LEN) );
	for ( int i = 0; i < MAX_PAGE_NUM; i++ )
	{
		d_iPrevTimeLimit[i] = 0;
		d_iNextTimeLimit[i] = 0;
	}
	
	d_baseInfo.bidPercent = 0;
	for ( i = 0; i < vendue_time_count; i++ )
	{
		d_baseInfo.tax[i] = 0;
	}
}

KUiVendueWnd::~KUiVendueWnd()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		TLGameObject* pGO = (TLGameObject*)m_pThisWnd->getChild("TaharezLook/itemvendueshop/itemicon");
		if( pGO )
		{
			delete pGO->getUserData();
		}
		_commoditylist::iterator it = m_commodityPageList.begin();
		while ( it != m_commodityPageList.end() )
		{
			if ( *it )
			{
				delete (*it);
			}
			
			it++;
		}
	}
}

void KUiVendueWnd::LoadConfig()
{
	g_pCoreShell->GetGameData(GDI_GET_AUCTION_BASE_INFO, (unsigned int)&d_baseInfo, NULL);
	m_nSaleTime	= d_baseInfo.time[vendue_time_short]*ONE_HOUR;
	
	char tempTime[COMMON_CLIENT_MSG_LEN_32];
	char* hour = KMessageCentre::GetMessage(login_error_message, 27);
	
	ZeroMemory(tempTime, COMMON_CLIENT_MSG_LEN_32);
	sprintf(tempTime, "%d%s", d_baseInfo.time[vendue_time_short], hour);
	d_pSaleTimeLimit0->setText(AnsiToUtf8(tempTime));
	
	ZeroMemory(tempTime, COMMON_CLIENT_MSG_LEN_32);
	sprintf(tempTime, "%d%s", d_baseInfo.time[vendue_time_middle], hour);
	d_pSaleTimeLimit1->setText(AnsiToUtf8(tempTime));
	
	ZeroMemory(tempTime, COMMON_CLIENT_MSG_LEN_32);
	sprintf(tempTime, "%d%s", d_baseInfo.time[vendue_time_long], hour);
	d_pSaleTimeLimit2->setText(AnsiToUtf8(tempTime));

}

void KUiVendueWnd::Init( void )
{
	try
	{
		if ( ms_Singleton && ms_Singleton->m_pThisWnd )
		{
			TLGameObject* pGO = (TLGameObject*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/itemicon");
			if( pGO )
			{
				KObjAtContRegion* region = new KObjAtContRegion();
				region->eContainer = UOC_EQUIPTMENT;
				region->Region.v = 0;
				pGO->setUserData(region);
			}
		// Do events wire-up
			ms_Singleton->initVendue();

			d_pBrowse = static_cast<TLStaticImage *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/Browse"));
			GetCtrlAndRegist();
		
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/find")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiVendueWnd::searchItem, ms_Singleton));
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikou")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiVendueWnd::BuyOnce, ms_Singleton));
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpai")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiVendueWnd::Buy, ms_Singleton));
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/beginpaimai")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiVendueWnd::Sale, ms_Singleton));
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/cancelpaimai")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiVendueWnd::CancleSale, ms_Singleton));
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/Prev")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiVendueWnd::Prev, ms_Singleton));
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/Next")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiVendueWnd::Next, ms_Singleton));
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/Close")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiVendueWnd::handleExit, ms_Singleton));
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/lookbtn")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleShowLook, ms_Singleton));
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingbiaobtn")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleShowJingbiao, ms_Singleton));
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/paimaibtn")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleShowPaimai, ms_Singleton));
			ms_Singleton->m_pThisWnd->getChild("TaharezLook/itemvendueshop/itemicon")->subscribeEvent(TLGameObject::EventMouseButtonDown, Event::Subscriber(&KUiVendueWnd::onLBDown, ms_Singleton));

			//m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaitxt")->setTooltipText(AnsiToUtf8(KMessageCentre::GetMessage(vendue_message, COMPETE_PRICE_EXPLANE)));
			//m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikoujtxt")->setTooltipText(AnsiToUtf8(KMessageCentre::GetMessage(vendue_message, STEADY_PRICE_EXPLANE)));
			//m_pThisWnd->getChild("TaharezLook/itemvendueshop/taxtxt")->setTooltipText(AnsiToUtf8(KMessageCentre::GetMessage(vendue_message, STORAGE_PRICE_EXPLANE)));

			d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/itemname")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			d_pWeaponLevelLow->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			d_pWeaponLevelHeight->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaij")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaiy")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpait")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaij")->subscribeEvent(Editbox::EventKeyUp, Event::Subscriber(&KUiVendueWnd::handleUpdateTax, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaiy")->subscribeEvent(Editbox::EventKeyUp, Event::Subscriber(&KUiVendueWnd::handleUpdateTax, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpait")->subscribeEvent(Editbox::EventKeyUp, Event::Subscriber(&KUiVendueWnd::handleUpdateTax, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpaij")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpaiy")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpait")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouj")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouy")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikout")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiVendueWnd::handleKeyDown, ms_Singleton));
			
			//加入装备品质下拉列表框 likun 
			m_pThisWnd->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleClickVendue, ms_Singleton));
			d_pEquipQulityBtn = static_cast<TLButton *>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/EquipmentQulityBtn"));
			d_pEquipMenu	  = static_cast<TLStaticImage *>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/EquipmentQulity"));
			d_pCommonEquipment= static_cast<TLButton *>(d_pEquipMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentQulity/Common"));
			d_pRareEquipment  = static_cast<TLButton *>(d_pEquipMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentQulity/Rare"));
			d_pSetEquipment   = static_cast<TLButton *>(d_pEquipMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentQulity/Set"));
			d_pEpicEquipment  = static_cast<TLButton *>(d_pEquipMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentQulity/Epic"));
			d_pArtificialOne  = static_cast<TLButton *>(d_pEquipMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentQulity/ArtificialOne"));
			//d_pArtificialTwo  = static_cast<TLButton *>(d_pEquipMenu->getChild("TaharezLook/itemvendueshop/EquipmentQulity/ArtificialTwo"));
			//d_pArtificialThree= static_cast<TLButton *>(d_pEquipMenu->getChild("TaharezLook/itemvendueshop/EquipmentQulity/ArtificialThree"));
			
			d_pEquipQulityBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipQulity, ms_Singleton));
			d_pCommonEquipment->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipSelect, ms_Singleton));
			d_pRareEquipment->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipSelect, ms_Singleton));
			d_pSetEquipment->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipSelect, ms_Singleton));
			d_pEpicEquipment->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipSelect, ms_Singleton));
			d_pArtificialOne->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipSelect, ms_Singleton));
			//d_pArtificialTwo->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipSelect, ms_Singleton));
			//d_pArtificialThree->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipSelect, ms_Singleton));

			d_pReqBtn = static_cast<TLButton *>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/ReqBtn"));
			d_pReqBtn->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiVendueWnd::handleReq, ms_Singleton));
			d_pReqMenuBack = static_cast<TLStaticImage *>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/ReqMenu"));;
			if ( d_pReqMenuBack )
			{
				d_pReqAllKind	= static_cast<TLRadioButton *>(d_pReqMenuBack->getChild("TaharezLook/itemvendueshop/Browse/All"));
				d_pReqEquipment = static_cast<TLRadioButton *>(d_pReqMenuBack->getChild("TaharezLook/itemvendueshop/Browse/Equipment"));
				d_pReqPotion	= static_cast<TLRadioButton *>(d_pReqMenuBack->getChild("TaharezLook/itemvendueshop/Browse/Potion"));
				d_pReqMaterial	= static_cast<TLRadioButton *>(d_pReqMenuBack->getChild("TaharezLook/itemvendueshop/Browse/Material"));
				d_pReqGem		= static_cast<TLRadioButton *>(d_pReqMenuBack->getChild("TaharezLook/itemvendueshop/Browse/Gem"));
				d_pReqOthers	= static_cast<TLRadioButton *>(d_pReqMenuBack->getChild("TaharezLook/itemvendueshop/Browse/Others"));

				d_pReqAllKind->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleRequestKind, ms_Singleton));
				d_pReqEquipment->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleRequestKind, ms_Singleton));
				d_pReqPotion->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleRequestKind, ms_Singleton));
				d_pReqMaterial->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleRequestKind, ms_Singleton));
				d_pReqGem->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleRequestKind, ms_Singleton));
				d_pReqOthers->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleRequestKind, ms_Singleton));

				d_KindMap[d_pReqAllKind]	= enAuction_Kind_All;
				d_KindMap[d_pReqEquipment]	= enAuction_Kind_Equipment;
				d_KindMap[d_pReqPotion]		= enAuction_Kind_Potion;
				d_KindMap[d_pReqMaterial]	= enAuction_Kind_Material;
				d_KindMap[d_pReqGem]		= enAuction_Kind_Gem;
				d_KindMap[d_pReqOthers]		= enAuction_Kind_Others;

				d_pReqMenuBack->hide();
			}

			d_pEquipBtn = static_cast<TLButton *>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/EquipmentTypeBtn"));
			d_pEquipBtn->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiVendueWnd::handleEquip, ms_Singleton));
			d_pEquipMenuBack = static_cast<TLStaticImage *>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/EquipmentType"));
			
			if ( d_pEquipMenuBack )
			{
				d_pEquipMenuBack->hide();
				d_pEquipAllKind = static_cast<TLRadioButton *>(d_pEquipMenuBack->getChild("TaharezLook/itemvendueshop/Browse/EquipmentType/AllKind"));
				d_pEquipToukui = static_cast<TLRadioButton *>(d_pEquipMenuBack->getChild("TaharezLook/itemvendueshop/Browse/EquipmentType/Toukui"));
				d_pEquipYifu = static_cast<TLRadioButton *>(d_pEquipMenuBack->getChild("TaharezLook/itemvendueshop/Browse/EquipmentType/Yifu"));
				d_pEquipHujian = static_cast<TLRadioButton *>(d_pEquipMenuBack->getChild("TaharezLook/itemvendueshop/Browse/EquipmentType/Hujian"));
				d_pEquipXuezi = static_cast<TLRadioButton *>(d_pEquipMenuBack->getChild("TaharezLook/itemvendueshop/Browse/EquipmentType/Xuezi"));
				d_pEquipWuqi = static_cast<TLRadioButton *>(d_pEquipMenuBack->getChild("TaharezLook/itemvendueshop/Browse/EquipmentType/Wuqi"));
				d_pEquipZhuishi = static_cast<TLRadioButton *>(d_pEquipMenuBack->getChild("TaharezLook/itemvendueshop/Browse/EquipmentType/Zhuishi"));
				d_pEquipYupei = static_cast<TLRadioButton *>(d_pEquipMenuBack->getChild("TaharezLook/itemvendueshop/Browse/EquipmentType/Yupei"));
				d_pEquipJiezhi = static_cast<TLRadioButton *>(d_pEquipMenuBack->getChild("TaharezLook/itemvendueshop/Browse/EquipmentType/Jiezhi"));
				d_pEquipHuwan = static_cast<TLRadioButton *>(d_pEquipMenuBack->getChild("TaharezLook/itemvendueshop/Browse/EquipmentType/Huwan"));
							
				d_pEquipAllKind->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipmentKind, ms_Singleton));
				d_pEquipToukui->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipmentKind, ms_Singleton));
				d_pEquipYifu->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipmentKind, ms_Singleton));
				d_pEquipHujian->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipmentKind, ms_Singleton));
				d_pEquipXuezi->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipmentKind, ms_Singleton));
				d_pEquipWuqi->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipmentKind, ms_Singleton));
				d_pEquipZhuishi->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipmentKind, ms_Singleton));
				d_pEquipYupei->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipmentKind, ms_Singleton));
				d_pEquipJiezhi->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipmentKind, ms_Singleton));
				d_pEquipHuwan->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleEquipmentKind, ms_Singleton));
				
				d_KindMap[d_pEquipAllKind]	= enAuction_Kind_Equipment;
				d_KindMap[d_pEquipToukui]	= enAuction_Kind_Helm;
				d_KindMap[d_pEquipYifu]		= enAuction_Kind_Armor;
				d_KindMap[d_pEquipHujian]	= enAuction_Kind_Shoulder;
				d_KindMap[d_pEquipXuezi]	= enAuction_Kind_Boots;
				d_KindMap[d_pEquipWuqi]		= enAuction_Kind_Weapon;
				d_KindMap[d_pEquipZhuishi]	= enAuction_Kind_Pendant;
				d_KindMap[d_pEquipYupei]	= enAuction_Kind_Amulet;
				d_KindMap[d_pEquipJiezhi]	= enAuction_Kind_Ring;
				d_KindMap[d_pEquipHuwan]	= enAuction_Kind_Cuff;
			}

			d_pProfessionBtn	= static_cast<TLButton *>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/EquipmentProfBtn"));
			d_pProfessionMenu	= static_cast<TLStaticImage *>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/EquipmentProf"));
			d_pProfessionCommon	= static_cast<TLButton *>(d_pProfessionMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentProf/Common"));
			d_pProfessionXF		= static_cast<TLButton *>(d_pProfessionMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentProf/XF"));
			d_pProfessionXT		= static_cast<TLButton *>(d_pProfessionMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentProf/XT"));
			d_pProfessionTS		= static_cast<TLButton *>(d_pProfessionMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentProf/TS"));
			d_pProfessionZR		= static_cast<TLButton *>(d_pProfessionMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentProf/ZR"));
			d_pProfessionSS		= static_cast<TLButton *>(d_pProfessionMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentProf/SS"));
			d_pProfessionYS		= static_cast<TLButton *>(d_pProfessionMenu->getChild("TaharezLook/itemvendueshop/Browse/EquipmentProf/YS"));
			
			d_pProfessionBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleProfession, ms_Singleton));
			d_pProfessionCommon->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleProfessionSelect, ms_Singleton));
			d_pProfessionXF->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleProfessionSelect, ms_Singleton));
			d_pProfessionXT->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleProfessionSelect, ms_Singleton));
			d_pProfessionTS->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleProfessionSelect, ms_Singleton));
			d_pProfessionZR->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleProfessionSelect, ms_Singleton));
			d_pProfessionSS->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleProfessionSelect, ms_Singleton));
			d_pProfessionYS->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleProfessionSelect, ms_Singleton));

			d_pEquipBtn->disable();
			d_pProfessionBtn->disable();

			d_pKindTypeTxt = static_cast<TLStaticText*>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/ItemType"));
			d_pQualityTypeTxt = static_cast<TLStaticText*>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/ItemQuality"));
			d_pProfTypeTxt = static_cast<TLStaticText*>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/ProfType"));
			d_pEquipmentTypeTxt = static_cast<TLStaticText*>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/EquipType"));

			ms_Singleton->registerDataset();
			ShowSearchOrVenduePageColumn();

			IUIMDLDataset* pIDataset = NULL;
			int nErr = ms_Singleton->m_pUiMDLManager->queryDataSet( vendue_dataset, &pIDataset );
			if ( nErr == success_errorcode )
			{
				pIDataset->setEventHandle( ms_Singleton );
				ms_Singleton->clearPageDate();
				KUiWndSingleton<KUiVendueWnd>::Show();

				//初始化显示金钱数量
				DWORD dSubValue = 0x00000000;
				ShowMoneyValue( dSubValue );
			}
			
			LoadConfig();
		}
	}
	catch (...)
	{
		// to do
	}
}


void	KUiVendueWnd::Show()
{
	KUiWndSingleton<KUiVendueWnd>::Show();

	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->clearPageDate();
		ms_Singleton->clearBottomJingpaiPrice();
		ms_Singleton->setSelectItemID(-1);
		int iGold = 0, iSiller = 0, iCopper = 0;
		int money = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
		sysMoneyToUiMoney(money, iGold, iSiller, iCopper);
		ms_Singleton->d_pGoldCoin->setText(iToString(iGold));
		ms_Singleton->d_pSillerCoin->setText(iToString(iSiller));
		ms_Singleton->d_pCopperCoin->setText(iToString(iCopper));
		ms_Singleton->m_pThisWnd->setZLevel(Window::Bottom);
	}
}

void	KUiVendueWnd::initVendue( void )
{
	IUIMDLDataset* pIDataset = NULL;
	int nErr = m_pUiMDLManager->queryDataSet( vendue_dataset, &pIDataset );
	if ( nErr == success_errorcode )
	{
		for ( int nIdx = 0; nIdx < m_nPageCount; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_256];	
			sprintf( szName, "%d_VenduePage", nIdx );
			m_commodityPageList.push_back( new KUiVenduePage );
			m_commodityPageList[nIdx]->CreateWnd( UI_ITEMVENDUEPAGE,szName );
			sprintf( szName, "%s%s", szName, "TaharezLook/funcpage" );
			Window* pPage = m_pWindowManager->getSingleton().getWindow( szName );
			pPage->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleClickVendue, ms_Singleton));
			m_pThisWnd->addChildWindow( pPage );
			pPage->setZLevel( Window::Bottom );
			((KUiVenduePage*)(m_commodityPageList[nIdx]))->initPage( nIdx );
		}	
	}

}

void KUiVendueWnd::registerDataset( void )
{
	IUIMDLDataset* pIDataset = NULL;
	int nErr = m_pUiMDLManager->queryDataSet( vendue_operation, &pIDataset );
	if ( nErr == success_errorcode )
	{
		//find_oper
		SEARCH_FILTER_COND tagSearch;
		ZeroMemory( &tagSearch, sizeof(SEARCH_FILTER_COND) );
		pIDataset->addDataRecord( &tagSearch, sizeof(SEARCH_FILTER_COND) );
		//sale_oper
		CLIENT_SELLGOODS_REQ tagSale;
		ZeroMemory( &tagSale, sizeof(CLIENT_SELLGOODS_REQ) );
		pIDataset->addDataRecord( &tagSale,sizeof( CLIENT_SELLGOODS_REQ) );		
		//buy_oper
		CLIENT_BUYGOODS_REQDATA tagBuy;
		ZeroMemory( &tagBuy, sizeof(CLIENT_BUYGOODS_REQDATA) );
		pIDataset->addDataRecord( &tagBuy, sizeof(CLIENT_BUYGOODS_REQDATA) );
		//cancel_oper
		CLIENT_BUYGOODS_REQDATA tagCancel;
		ZeroMemory( &tagCancel, sizeof(CLIENT_BUYGOODS_REQDATA));
		pIDataset->addDataRecord( &tagCancel, sizeof(CLIENT_BUYGOODS_REQDATA));
		int nOper = 0;
		//prev_oper
		pIDataset->addDataRecord( &nOper, sizeof(SEARCH_FILTER_COND) );
		//next_oper
		pIDataset->addDataRecord( &nOper, sizeof(SEARCH_FILTER_COND) );
	}
}

void	KUiVendueWnd::updatePage( int nIdx, void *pBuff )
{
	int nPageIdx = nIdx / MAXRECORDS_PER_PAGE;
	int nBarIdx = nIdx % MAXRECORDS_PER_PAGE;
	((KUiVenduePage*)(m_commodityPageList[nPageIdx]))->updateBar( nBarIdx, pBuff );
}

void	KUiVendueWnd::setPage( int nPage )
{
	_commoditylist::iterator it = m_commodityPageList.begin();
	while ( it != m_commodityPageList.end() )
	{
		(*it)->Hide();
	}
	m_commodityPageList[nPage]->Show();
}

void	KUiVendueWnd::clearPageDate	( void )
{
	_commoditylist::iterator it = m_commodityPageList.begin();
	while ( it != m_commodityPageList.end() )
	{
		((KUiVenduePage*)(*it))->clearBarDate();
		((KUiVenduePage*)(*it))->clearSelectItem();
		it++;
	}
}

void	KUiVendueWnd::setSelectItemID( DWORD dwItemID )
{
	m_curSelectItemID = dwItemID;
}

void	KUiVendueWnd::onCreate( UIMDLEvent& rEvent	)
{

}

void	KUiVendueWnd::onRelease( UIMDLEvent& rEvent	)
{

}

void	KUiVendueWnd::onChange( UIMDLEvent& rEvent	)
{
	IUIMDLDataset* pIDataset = rEvent.pDataSet;
	if ( pIDataset )
	{
		UIMDLDatasetRecord &rRecord = pIDataset->getDataRecord( rEvent.nRecordIndex );
		updatePage( rEvent.nRecordIndex, rRecord.pRecordData );		
	}
}

bool	KUiVendueWnd::searchItem( const CEGUI::EventArgs& args )
{
	int tmpTime = GetTickCount();
	if ( tmpTime - d_iSearchTimeLimit > TIME_LIMIT )
	{
		search( find_oper );
		d_iSearchTimeLimit = GetTickCount();
	}
	else
	{
		char *message = KMessageCentre::GetMessage(vendue_message, OPERING_NOW);
		KUiChannelCentre::GetSingleton().toSysMsg(message);
	}

	return true;
}

void	KUiVendueWnd::search( AuctionComOper searchOper )
{
	IUIMDLDataset* pIDataset = NULL;
	int nErr = m_pUiMDLManager->queryDataSet( vendue_operation, &pIDataset );
	if ( nErr == success_errorcode )
	{
		clearPageDate();
		SEARCH_FILTER_COND tagSearch;
		ZeroMemory( &tagSearch, sizeof( SEARCH_FILTER_COND ) );
		CEGUI::String str = d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/itemname")->getText();
		if ( str != String("") )
		{
			strncpy( tagSearch.goodsName, Utf8ToAnsi( str ), MAXSIZE_ITEMNAME + 1 );
		}
		else
		{
			ZeroMemory( tagSearch.goodsName, MAXSIZE_ITEMNAME );
			tagSearch.goodsName[MAXSIZE_ITEMNAME+1] = 0;
		}
		WORD iWeaponLow	= static_cast<WORD>(atoi(d_pWeaponLevelLow->getText().c_str()));
		WORD iWeaponHeight = static_cast<WORD>(atoi(d_pWeaponLevelHeight->getText().c_str()));
		//物品等级限定
		tagSearch.levelReqLow   = iWeaponLow;
		tagSearch.levelReqHight = iWeaponHeight;
		if ( tagSearch.levelReqHight <= 0 )
		{
			tagSearch.levelReqHight = -1;
		}
		else if ( tagSearch.levelReqHight < tagSearch.levelReqLow )
		{
			tagSearch.levelReqLow = 0;
		}

		//物品类型限定
		ZeroMemory(tagSearch.itemType, AUCTION_MAX_ITEM_TYPE_LEN);
		strncpy(tagSearch.itemType, d_ReqKind, AUCTION_MAX_ITEM_TYPE_LEN);

		//物品品质限定
		if ( d_iEquipQulity == 0 )
		{
			tagSearch.qualityLabel = -1;
		}
		else
		{
			tagSearch.qualityLabel	= d_iEquipQulity;
		}
		
		//物品职业限定
		tagSearch.factionReq	= d_iProf;

		//物品可用限定
		if ( d_bUseAble )
		{
			KUiPlayerBaseInfo	baseInfo;
			KUiPlayerAttribute	runtimeAttribute;
			g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&baseInfo, NULL );
			g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&runtimeAttribute, NULL );
			
			if ( tagSearch.levelReqHight != -1 )
			{
				tagSearch.levelReqHight = min( tagSearch.levelReqHight, runtimeAttribute.nLevel );
			}
			
			switch( runtimeAttribute.nSeries )
			{
			case 0:
				{
					switch( baseInfo.nSkillType )
					{
					case -1:
						tagSearch.factionReq = 1|2;
						break;
					case 0:
						tagSearch.factionReq = 1;
						break;
					case 1:
						tagSearch.factionReq = 2;
					    break;
					default:
					    break;
					}
				}
				break;
			case 1:
				{
					switch( baseInfo.nSkillType )
					{
					case -1:
						tagSearch.factionReq = 4|8;
						break;
					case 0:
						tagSearch.factionReq = 4;
						break;
					case 1:
						tagSearch.factionReq = 8;
					    break;
					default:
					    break;
					}
				}
				break;
			case 2:
				{
					switch( baseInfo.nSkillType )
					{
					case -1:
						tagSearch.factionReq = 16|32;
						break;
					case 0:
						tagSearch.factionReq = 16;
						break;
					case 1:
						tagSearch.factionReq = 32;
					    break;
					default:
					    break;
					}					
				}
			    break;
			default:
			    break;
			}			
		}

		m_iCurrentBarNum = 0;
		pIDataset->updateRecord( searchOper, &tagSearch, sizeof(SEARCH_FILTER_COND)  );
	}
}

bool	KUiVendueWnd::Sale( const CEGUI::EventArgs& args )
{
	TLGameObject* pGO = (TLGameObject*)m_pThisWnd->getChild( "TaharezLook/itemvendueshop/itemicon" );
	if ( pGO && pGO->getGameObjectType() == TLGameObject::item )
	{
		clearPageDate();
		IUIMDLDataset* pIDataset = NULL;
		int nErr = m_pUiMDLManager->queryDataSet( vendue_operation, &pIDataset );
		if ( nErr == success_errorcode )
		{
			CLIENT_SELLGOODS_REQ tagSell;
			ZeroMemory( &tagSell, sizeof( CLIENT_SELLGOODS_REQ ) );
			KObjAtContRegion* itemRegion = (KObjAtContRegion*)pGO->getUserData();
			tagSell.itemId	= itemRegion->Obj.uId;

			KItemInfo tagItemInfo;
			g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&tagItemInfo, tagSell.itemId );
			if (!tagItemInfo.bVendue)
			{
				pGO->clear();
				pGO->show();
				clearYikouPrice();
				clearJingpaiPrice();
				clearBottomJingpaiPrice();
				char *message = KMessageCentre::GetMessage(vendue_message, NOT_VENDUE);
				KUiChannelCentre::GetSingleton().toSysMsg(message);
				return false;
			}
			
			DWORD	currentPrice				= getJingpaiPrice();
			DWORD	OnePrice					= getYikouPrice();

			if ( currentPrice == 0 || OnePrice == 0 )
			{
				pGO->clear();
				pGO->show();
				clearYikouPrice();
				clearJingpaiPrice();
				clearBottomJingpaiPrice();
				char *message = KMessageCentre::GetMessage(vendue_message, PRICE_NOT_ZERO);
				KUiChannelCentre::GetSingleton().toSysMsg(message);
				return false;
			}
			tagSell.recordComData.currentPrice		= getJingpaiPrice();
			tagSell.recordComData.onePrice			= OnePrice;
			tagSell.recordComData.totalValidTime	= m_nSaleTime;
			m_iCurrentBarNum = 0;
			pIDataset->updateRecord( sale_oper, &tagSell, sizeof(CLIENT_SELLGOODS_REQ) );
			pGO->clear();
			pGO->show();
			clearYikouPrice();
			clearJingpaiPrice();
			clearBottomJingpaiPrice();
		}
	}
	return true;
}

bool	KUiVendueWnd::Buy( const CEGUI::EventArgs& args )
{
	//clearPageDate();
	IUIMDLDataset* pIDataset = NULL;
	IUIMDLDataset* pIOper = NULL;
	int nErr = m_pUiMDLManager->queryDataSet( vendue_dataset, &pIDataset );
	int nErrOper = m_pUiMDLManager->queryDataSet( vendue_operation, &pIOper );
	if ( nErr == success_errorcode && nErrOper == success_errorcode && m_curSelectItemID != -1 )
	{
		ZeroMemory( &d_buyInfo, sizeof( CLIENT_BUYGOODS_REQDATA ));
		d_buyInfo.recordId = m_curSelectItemID;
		UIMDLDatasetRecord& rRecord = pIDataset->findDataRecord( _vendueFindItemFromMDL, &d_buyInfo.recordId );
		if ( rRecord.nIndex >= 0 )
		{
			SEARCH_DB_RETDATA*  pData = (SEARCH_DB_RETDATA*)rRecord.pRecordData;
			
			if ( !pData )
				return false;

			// 得到物品名称
			KItemInfo tagItemInfo;
			ZeroMemory( &tagItemInfo, sizeof( KItemInfo ) );
			FIND_ITEMINDEX_PARAM tagItemIdx;
			ZeroMemory( &tagItemIdx, sizeof(FIND_ITEMINDEX_PARAM) );

			tagItemIdx.nGenre = pData->itemData.igenre;
			tagItemIdx.nDetail	= pData->itemData.idetailtype;
			tagItemIdx.nParticular	= pData->itemData.iparticulartype;
			tagItemIdx.nLevel	= pData->itemData.ilevel;		
			g_pCoreShell->GetGameData( GDI_ITEM_INFO_PARTICULAR, (unsigned int)&tagItemIdx, (int)&tagItemInfo );

			char itemName[COMMON_CLIENT_MSG_LEN_64];
			g_pCoreShell->GetGameData( GDI_GET_ITEM_NAME_WITH_COLOR_BY_INDEXPARAM, (unsigned int)&tagItemIdx, (unsigned int)itemName );
			d_curItemName = AnsiToUtf8(itemName);
			
			int iPrice = getBottomJingpaiPrice();
			if ( iPrice == 0 )
			{
				if (((pData->recordComData.currentPrice + INCREASE_GOLD) < pData->recordComData.onePrice))
				{
					d_buyInfo.price	= pData->recordComData.currentPrice + INCREASE_GOLD;
				}
				else
				{
					d_buyInfo.price = pData->recordComData.onePrice;
					ShowRegComMsgBox( BUY_ONE_BY_PRICE, IS_BUY_SOMETHING );
					return true;
				}
			}
			else
			{
				if ( (iPrice == pData->recordComData.currentPrice &&
					  iPrice == pData->recordComData.onePrice) || 
					 (iPrice > pData->recordComData.currentPrice  &&
					 (iPrice < pData->recordComData.onePrice   || 
					  iPrice == pData->recordComData.onePrice)) )
				{
					d_buyInfo.price	= getBottomJingpaiPrice();
				}
				else if ( iPrice > pData->recordComData.onePrice )
				{
					d_buyInfo.price	= pData->recordComData.onePrice;
					ShowRegComMsgBox( BUY_ONE_BY_PRICE, IS_BUY_SOMETHING );
					return true;
				}
				else
				{
					char *message = KMessageCentre::GetMessage(vendue_message, WRONG_CONDITION);
					KUiChannelCentre::GetSingleton().toSysMsg(message);
					return false;
				}
			}
			//显示竞拍价购买确认界面
			ShowRegComMsgBox(BUY_BY_PRICE, IS_BUY_SOMETHING);
			//clearPageDate();
		}
	}
	else
	{
		char *message = KMessageCentre::GetMessage(vendue_message, SELECT_WARNING);
		KUiChannelCentre::GetSingleton().toSysMsg(message);
		
		return false;
	}
	return true;
}

bool	KUiVendueWnd::BuyOnce( const CEGUI::EventArgs& args )
{
	//clearPageDate();
	IUIMDLDataset* pIDataset = NULL;
	IUIMDLDataset* pIOper = NULL;
	int nErr = m_pUiMDLManager->queryDataSet( vendue_dataset, &pIDataset );
	int nErrOper = m_pUiMDLManager->queryDataSet( vendue_operation, &pIOper );
	if ( nErr == success_errorcode && nErrOper == success_errorcode && m_curSelectItemID != -1 )
	{
		ZeroMemory( &d_buyInfo, sizeof( CLIENT_BUYGOODS_REQDATA ));
		d_buyInfo.recordId = m_curSelectItemID;
		UIMDLDatasetRecord& rRecord = pIDataset->findDataRecord( _vendueFindItemFromMDL, &d_buyInfo.recordId );
		if ( rRecord.nIndex >= 0 )
		{
			SEARCH_DB_RETDATA*  pData = (SEARCH_DB_RETDATA*)rRecord.pRecordData;
			if ( pData )
			{
				d_buyInfo.price = pData->recordComData.onePrice;

				// 得到物品名称
				KItemInfo tagItemInfo;
				ZeroMemory( &tagItemInfo, sizeof( KItemInfo ) );
				FIND_ITEMINDEX_PARAM tagItemIdx;
				ZeroMemory( &tagItemIdx, sizeof(FIND_ITEMINDEX_PARAM) );

				tagItemIdx.nGenre = pData->itemData.igenre;
				tagItemIdx.nDetail	= pData->itemData.idetailtype;
				tagItemIdx.nParticular	= pData->itemData.iparticulartype;
				tagItemIdx.nLevel	= pData->itemData.ilevel;		
				g_pCoreShell->GetGameData( GDI_ITEM_INFO_PARTICULAR, (unsigned int)&tagItemIdx, (int)&tagItemInfo );

				char itemName[COMMON_CLIENT_MSG_LEN_64];
				g_pCoreShell->GetGameData( GDI_GET_ITEM_NAME_WITH_COLOR_BY_INDEXPARAM, (unsigned int)&tagItemIdx, (unsigned int)itemName );
				d_curItemName = AnsiToUtf8(itemName);

				//显示一口价购买确认界面
				ShowRegComMsgBox( BUY_ONE_BY_PRICE, IS_BUY_SOMETHING );
			}
		}
	}
	else
	{
		char *message = KMessageCentre::GetMessage(vendue_message, SELECT_WARNING);
		KUiChannelCentre::GetSingleton().toSysMsg(message);
		return false;
	}
	return true;
}

bool	KUiVendueWnd::Prev( const CEGUI::EventArgs& args )
{
	SearchOperation(prev_oper, m_curPage);
	return true;
}

bool	KUiVendueWnd::Next( const CEGUI::EventArgs& args )
{
	//if ( m_iCurrentBarNum >= MAX_BAR_NUM )
	{
		SearchOperation(next_oper, m_curPage);
	}
	return true;
}

void	KUiVendueWnd::SearchOperation( AuctionComOper oper, _pagetype pagetype )
{
	//操作时间检测
	/*int tmpTime = GetTickCount();
	switch(oper)
	{
	case find_oper:
		{
			if (tmpTime - d_iSearchTimeLimit <= TIME_LIMIT)
			{				
				char *message = KMessageCentre::GetMessage(vendue_message, OPERING_NOW);
				KUiChannelCentre::GetSingleton().toSysMsg(message);
				return;
			}
			else
				d_iSearchTimeLimit = ::GetTickCount();
		}
		break;
	case next_oper:
		{
			if (tmpTime - d_iNextTimeLimit[pagetype] <= TIME_LIMIT)
			{				
				char *message = KMessageCentre::GetMessage(vendue_message, OPERING_NOW);
				KUiChannelCentre::GetSingleton().toSysMsg(message);
				return;
			}
			else
				d_iNextTimeLimit[pagetype] = GetTickCount();
		}
		break;
	case prev_oper:
		{
			if (tmpTime - d_iPrevTimeLimit[pagetype] <= TIME_LIMIT)
			{				
				char *message = KMessageCentre::GetMessage(vendue_message, OPERING_NOW);
				KUiChannelCentre::GetSingleton().toSysMsg(message);
				return;
			}
			else
				d_iPrevTimeLimit[pagetype] = GetTickCount();
		}
	    break;
	default:
	    break;
	}//*/
	
	//执行操作
	switch(pagetype)
	{
	case look_page:
		{
			search( oper );
			ShowLook();
		}
		break;
	case jingpai_page:
		{
			IUIMDLDataset* pIDataset = NULL;
			int nErr = m_pUiMDLManager->queryDataSet( vendue_operation, &pIDataset );
			if ( nErr == success_errorcode )
			{
				m_iCurrentBarNum = 0;
				clearPageDate();
				SEARCH_FILTER_COND tagSearch;
				ZeroMemory( &tagSearch, sizeof( SEARCH_FILTER_COND ) );
				tagSearch.levelReqHight	= -1;
				tagSearch.qualityLabel	= -1;
				tagSearch.factionReq	= -1;
				strncpy(tagSearch.itemType, "-1,-1,-1,-1", AUCTION_MAX_ITEM_TYPE_LEN);
				KUiPlayerBaseInfo tagInfo;
				g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&tagInfo, NULL );
				strncpy( tagSearch.buyerName, tagInfo.Name, sizeof(tagSearch.buyerName) );
				tagSearch.buyerName[MAXSIZE_ROLENAME-1] = 0;
				pIDataset->updateRecord( oper, &tagSearch, sizeof(SEARCH_FILTER_COND)  );
			}
			ShowJingpai();
		}
		break;
	case sell_page:
		{
			IUIMDLDataset* pIDataset = NULL;
			int nErr = m_pUiMDLManager->queryDataSet( vendue_operation, &pIDataset );
			if ( nErr == success_errorcode )
			{
				m_iCurrentBarNum = 0;
				clearPageDate();
				SEARCH_FILTER_COND tagSearch;
				ZeroMemory( &tagSearch, sizeof( SEARCH_FILTER_COND ) );
				tagSearch.levelReqHight	= -1;
				tagSearch.qualityLabel	= -1;
				tagSearch.factionReq	= -1;
				strncpy(tagSearch.itemType, "-1,-1,-1,-1", AUCTION_MAX_ITEM_TYPE_LEN);
				KUiPlayerBaseInfo tagInfo;
				g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&tagInfo, NULL );
				strncpy( tagSearch.sellerName, tagInfo.Name, sizeof(tagSearch.sellerName) );
				tagSearch.sellerName[MAXSIZE_ROLENAME-1] = 0;
				pIDataset->updateRecord( oper, &tagSearch, sizeof(SEARCH_FILTER_COND)  );
			}
			ShowPaimai();
		}
	    break;
	default:
	    break;
	}
}

bool	KUiVendueWnd::handleExit( const CEGUI::EventArgs& args )
{
	TLGameObject* pGO = (TLGameObject*)m_pThisWnd->getChild( "TaharezLook/itemvendueshop/itemicon" );
	if ( pGO != NULL )
	{
		pGO->clear();
		pGO->show();
		clearYikouPrice();
		clearJingpaiPrice();
		clearBottomJingpaiPrice();
	}
	Hide();
    return true;
}

void	KUiVendueWnd::ShowLook( void )
{
	d_pUseAbleChckBtn->show();
	if ( !CEGUI::ImagesetManager::getSingleton().isImagesetPresent( UI_VENDUE_BG ) )
	{
		CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile( UI_VENDUE_BG, UI_VENDUE_BG_IMAGE_PATH_JS );
		CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile( UI_VENDUE_BG_LONG, UI_VENDUE_BG_LONG_IMAGE_PATH_JS );
	}

	char szName[COMMON_CLIENT_MSG_LEN_256];
	sprintf( szName, "%d_VenduePage", 0 );
	sprintf( szName, "%s%s", szName, "TaharezLook/funcpage" );
	Window* pImage = ms_Singleton->m_pThisWnd->getChild(szName);
	static_cast<StaticImage*>(pImage)->hide();

 	// set area rectangle
	Size size;
	size.d_width	= UI_VENDUE_BG_WIDTH;
	size.d_height	= UI_VENDUE_BG_HEIGHT;

	Point pos;
	pos.d_x			= UI_VENDUE_BG_X;
	pos.d_y			= UI_VENDUE_BG_Y;
	static_cast<StaticImage*>(pImage)->setPosition( Absolute, pos );
	static_cast<StaticImage*>(pImage)->setSize( Absolute, size );
 	// disable frame and standard background
 	static_cast<StaticImage*>(pImage)->setFrameEnabled(false);
 	static_cast<StaticImage*>(pImage)->setBackgroundEnabled(false);
 	// set the background image
 	static_cast<StaticImage*>(pImage)->setImage( UI_VENDUE_BG_LONG, UI_FULL_IMAGESET );
	static_cast<StaticImage*>(pImage)->show();

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/Prev" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/Next" )->show();
	d_pWeaponLevelLow->show();
	d_pWeaponLevelHeight->show();
	m_curPage = look_page;
	hideAll();
	//ShowSearchOrVenduePageColumn();

	d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/itemname")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/find")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikou")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpai")->show();
	
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxj_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxy_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxt_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxtxt" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxj" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxy" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxt" )->hide();

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaij_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaiy_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpait_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaitxt" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaij" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaiy" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpait" )->hide();

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouj_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouy_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikout_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikoujtxt" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouj" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouy" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikout" )->hide();

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaij_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaiy_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpait_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaitxt" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaij" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaiy" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpait" )->show();
}

void	KUiVendueWnd::ShowPaimai( void )
{
	d_pUseAbleChckBtn->hide();
	m_curPage = sell_page;
	hideAll();
	//ShowSearchOrVenduePageColumn();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/Prev" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/Next" )->show();
	
	//拍卖页面显示
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/itemicon")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/vendueTime")->show();
	d_pSaleTimeLimit0->show();
	d_pSaleTimeLimit1->show();
	d_pSaleTimeLimit2->show();
	d_pWeaponLevelLow->hide();
	d_pWeaponLevelHeight->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouj_img")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouy_img")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikout_img")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaij_img")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaiy_img")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpait_img")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouj")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouy")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikout")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaij")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaiy")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpait")->show();

/*
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/taxj_img")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/taxy_img")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/taxt_img")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/taxj")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/taxy")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/taxt")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/taxtxt")->show();		
//*/
		
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/beginpaimai")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/cancelpaimai")->show();	

	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaitxt")->show();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikoujtxt")->show();

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaij_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaiy_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpait_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaitxt" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaij" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaiy" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpait" )->hide();
}


void	KUiVendueWnd::ShowJingpai( void )
{
	m_curPage = jingpai_page;
	//ShowJinPaiPageColumn();
	hideAll();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/Prev" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/Next" )->show();
	d_pWeaponLevelLow->hide();
	d_pWeaponLevelHeight->hide();
	d_pUseAbleChckBtn->hide();
	if ( NULL != d_pEquipQulityBtn && NULL != d_pEquipMenu )
	{
		d_pEquipQulityBtn->hide();
		d_pEquipMenu->hide();
	}

	if ( NULL != d_pBrowse )
	{
		d_pBrowse->hide();
	}

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaij_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaiy_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpait_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaitxt" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaij" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpaiy" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/buttomjingpait" )->hide();

}

bool	KUiVendueWnd::handleShowLook( const CEGUI::EventArgs& args )
{
	if ( NULL != d_pEquipQulityBtn && NULL != d_pEquipMenu )
	{
		d_pEquipQulityBtn->show();
		d_pEquipMenu->hide();
	}

	if ( NULL != d_pBrowse && NULL != d_pProfessionMenu )
	{
		d_pBrowse->show();
	}
	
	clearBottomJingpaiPrice();
	setSelectItemID(-1);
	ShowLook();
	ShowSearchOrVenduePageColumn();
	return true;
}

bool	KUiVendueWnd::handleShowPaimai( const CEGUI::EventArgs& args )
{
	char szName[COMMON_CLIENT_MSG_LEN_256];
	sprintf( szName, "%d_VenduePage", 0 );
	sprintf( szName, "%s%s", szName, "TaharezLook/funcpage" );

	Window* pImage = ms_Singleton->m_pThisWnd->getChild(szName);
	static_cast<StaticImage*>(pImage)->hide();

 	// set area rectangle
	Size size;
	size.d_width	= UI_VENDUE_BG_WIDTH;
	size.d_height	= UI_VENDUE_BG_HEIGHT;

	Point pos;
	pos.d_x			= UI_VENDUE_BG_X;
	pos.d_y			= UI_VENDUE_BG_Y;
	static_cast<StaticImage*>(pImage)->setPosition( Absolute, pos );
	static_cast<StaticImage*>(pImage)->setSize( Absolute, size );
 	// disable frame and standard background
 	static_cast<StaticImage*>(pImage)->setFrameEnabled(false);
 	static_cast<StaticImage*>(pImage)->setBackgroundEnabled(false);
 	// set the background image
 	static_cast<StaticImage*>(pImage)->setImage( UI_VENDUE_BG, UI_FULL_IMAGESET );
	static_cast<StaticImage*>(pImage)->show();

	if ( NULL != d_pEquipQulityBtn && NULL != d_pEquipMenu )
	{
		d_pEquipQulityBtn->hide();
		d_pEquipMenu->hide();
	}

	if ( NULL != d_pBrowse )
	{
		d_pBrowse->hide();
	}

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxj_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxy_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxt_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxtxt" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxj" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxy" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxt" )->show();

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaij_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaiy_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpait_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaitxt" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaij" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaiy" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpait" )->show();

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouj_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouy_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikout_img" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikoujtxt" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouj" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouy" )->show();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikout" )->show();

	SearchOperation(find_oper, sell_page);
	ShowSearchOrVenduePageColumn();
	return true;
}

bool	KUiVendueWnd::handleShowJingbiao( const CEGUI::EventArgs& args )
{

	char szName[COMMON_CLIENT_MSG_LEN_256];
	sprintf( szName, "%d_VenduePage", 0 );
	sprintf( szName, "%s%s", szName, "TaharezLook/funcpage" );

	Window* pImage = ms_Singleton->m_pThisWnd->getChild(szName);
	Size size;
	size.d_width	= UI_VENDUE_BG_WIDTH_LONG;
	size.d_height	= UI_VENDUE_BG_HEIGHT_LONG;
 	static_cast<StaticImage*>(pImage)->setSize( Absolute, size );

	Point pos;
	pos.d_x			= UI_VENDUE_BG_LONG_X;
	pos.d_y			= UI_VENDUE_BG_LONG_Y;
	static_cast<StaticImage*>(pImage)->setPosition( Absolute, pos );
 	// disable frame and standard background
 	static_cast<StaticImage*>(pImage)->setFrameEnabled(false);
 	static_cast<StaticImage*>(pImage)->setBackgroundEnabled(false);
 	// set the background image
	//static_cast<StaticImage*>(pImage)->setImage( UI_VENDUE_BG_LONG, UI_FULL_IMAGESET );
 	static_cast<StaticImage*>(pImage)->setImage( UI_VENDUE_BG_JINGBIAO, UI_FULL_IMAGESET );

	if ( NULL != d_pEquipQulityBtn && NULL != d_pEquipMenu )
	{
		d_pEquipQulityBtn->hide();
		d_pEquipMenu->hide();
	}

	if ( NULL != d_pBrowse )
	{
		d_pBrowse->hide();
	}

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxj_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxy_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxt_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxtxt" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxj" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxy" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxt" )->hide();

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaij_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaiy_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpait_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaitxt" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaij" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpaiy" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/jingpait" )->hide();

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouj_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouy_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikout_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikoujtxt" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouj" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikouy" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/yikout" )->hide();

	SearchOperation(find_oper, jingpai_page);	
	ShowJinPaiPageColumn();
	return true;
}

void	KUiVendueWnd::hideAll( void )
{
	clearPageDate();
	d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/itemname")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/find")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikou")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpai")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/itemicon")->hide();
	//隐藏拍卖时间限制
	d_pSaleTimeLimit0->hide();
	d_pSaleTimeLimit1->hide();
	d_pSaleTimeLimit2->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/vendueTime")->hide();

	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouj_img")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouy_img")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikout_img")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaij_img")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaiy_img")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpait_img")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouj")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouy")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikout")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaij")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaiy")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpait")->hide();

	m_pThisWnd->getChild("TaharezLook/itemvendueshop/beginpaimai")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/cancelpaimai")->hide();
/*
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxj_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxy_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxt_img" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxtxt" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxj" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxy" )->hide();
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxt" )->hide();
//*/
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaitxt")->hide();
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikoujtxt")->hide();
		
}

bool KUiVendueWnd::handleSetTime0(const CEGUI::EventArgs& e)
{
	m_nSaleTime	= d_baseInfo.time[vendue_time_short]*ONE_HOUR;
	return true;
}

bool KUiVendueWnd::handleSetTime1(const CEGUI::EventArgs& e)
{
	m_nSaleTime	= d_baseInfo.time[vendue_time_middle]*ONE_HOUR;
	return true;
}

bool KUiVendueWnd::handleSetTime2(const CEGUI::EventArgs& e)
{
	m_nSaleTime	= d_baseInfo.time[vendue_time_long]*ONE_HOUR;
	return true;
}

bool KUiVendueWnd::onLBDown(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	if(arg->button != LeftButton)
		return false;

	TLGameObject *pGO = static_cast<TLGameObject*>(arg->window);
	if ( pGO )
	{
		TLGameObject* destObj,* sourObj;
		TLGameObject::GameObject destObjInfo, sourObjInfo;
		//KObjAtContRegion* destRegion, * sourRegion;
		destObj = pGO;	
		destObj->getObject(destObjInfo);
		sourObj = KUiDragItem::GetSingleton().getObj();
		KObjAtContRegion* pItem = (KObjAtContRegion*)sourObj->getUserData();
		sourObj->getObject(sourObjInfo);
		sourObj->clear();
		if(sourObjInfo.d_type == TLGameObject::idle)
		{
			if(destObjInfo.d_type == TLGameObject::item)
			{
				sourObj->setObject(destObjInfo);
				sourObj->setCanDrag(true);
			}
			pGO->clear();
			pGO->show();
		}
		else
		{
			// 拍卖行中物品的TIP
			KItemInfo tagItemInfo;
			g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&tagItemInfo, pItem->Obj.uId );
			if ( tagItemInfo.bIsBind )
			{
				char layoutText[COMMON_CLIENT_MSG_LEN_1024];
				sprintf(layoutText, "<Seg float=wrap><Obj type=text c=ffff0000>[%s]%s</Obj></Seg>", CHAT_CHANNEL_NAME_SYSTEM, ITEM_CANNT_VEN);
				KUiChannelCentre::GetSingleton().recvCustomMessage(SYSTEM_ROOM_ID, layoutText);
				KUiDragItem::GetSingleton().initItem();
			}
			else
			{
				pGO->clear();
				sourObjInfo.d_EdgeframeIdx = tagItemInfo.colour;
				pGO->setObject( sourObjInfo );
				pGO->setTooltipText( AnsiToUtf8( tagItemInfo.szToolTip ) );
				//KObjAtContRegion temp = *((KObjAtContRegion*)(sourObj->getUserData()));
				*((KObjAtContRegion*)(pGO->getUserData())) = *((KObjAtContRegion*)(sourObj->getUserData()));
				pGO->setType( TLGameObject::item );

				KUiDragItem::GetSingleton().initItem();
			}

		}
	}
	return true;
}

int	KUiVendueWnd::getYikouPrice()
{
	int j, y, t;
	j = atoi( Utf8ToAnsi( d_pBuyOnceGold->getText() ) );
	y = atoi( Utf8ToAnsi( d_pBuyOnceSlive->getText() ) );
	t = atoi( Utf8ToAnsi( d_pBuyOnceCopper->getText() ) );
	return uiMoneyToSysMoney( j, y, t );
}

int	KUiVendueWnd::getJingpaiPrice()
{
	int j, y, t;
	j = atoi( Utf8ToAnsi( d_pBuyGold->getText() ) );
	y = atoi( Utf8ToAnsi( d_pBuySlive->getText() ) );
	t = atoi( Utf8ToAnsi( d_pBuyCopper->getText() ) );
	return uiMoneyToSysMoney( j, y, t );
}

int	KUiVendueWnd::getBottomJingpaiPrice()
{
	int j, y, t;
	j = atoi( Utf8ToAnsi( d_pBottomGold->getText() ) );
	y = atoi( Utf8ToAnsi( d_pBottomSlive->getText() ) );
	t = atoi( Utf8ToAnsi( d_pBottomCopper->getText() ) );
	return uiMoneyToSysMoney( j, y, t );
}

void KUiVendueWnd::clearYikouPrice()
{
	d_pBuyOnceGold->setText("");
	d_pBuyOnceSlive->setText("");
	d_pBuyOnceCopper->setText("");
}

void KUiVendueWnd::clearJingpaiPrice()
{
	d_pBuyGold->setText("");
	d_pBuySlive->setText("");
	d_pBuyCopper->setText("");
	
	d_pTaxGold->setText(AnsiToUtf8("0"));
	d_pTaxSlive->setText(AnsiToUtf8("0"));
	d_pTaxCopper->setText(AnsiToUtf8("0"));
}

void KUiVendueWnd::clearBottomJingpaiPrice()
{
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpaij")->setText("");
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpaiy")->setText("");
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpait")->setText("");
}

//likun defined
//获取并注册控件窗口
bool KUiVendueWnd::GetCtrlAndRegist( void )
{
	bool result = false;
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		d_pUseAbleChckBtn	= static_cast<CEGUI::TLRadioButton *>(d_pBrowse->getChild( "TaharezLook/itemvendueshop/Browse/useable" ));
		d_pUseAbleChckBtn->subscribeEvent( TLRadioButton::EventMouseClick, Event::Subscriber(&KUiVendueWnd::handleUseAble, this ));
		d_pGoldCoin			= static_cast<CEGUI::Window *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/bodyj"));
		d_pSillerCoin		= static_cast<CEGUI::Window *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/bodyy"));
		d_pCopperCoin		= static_cast<CEGUI::Window *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/bodyt"));
		
		d_pWeaponLevelLow   = static_cast<CEGUI::TLEditbox *>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/WeaponLevelLow"));
		d_pWeaponLevelHeight= static_cast<CEGUI::TLEditbox *>(d_pBrowse->getChild("TaharezLook/itemvendueshop/Browse/WeaponLevelHeigh"));
		d_pWeaponLevelLow->setIsOnlyNumber(true);
		d_pWeaponLevelHeight->setIsOnlyNumber(true);
		
		d_pSaleTimeLimit0	= static_cast<CEGUI::RadioButton *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/TimeLimit0"));
		d_pSaleTimeLimit1  = static_cast<CEGUI::RadioButton *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/TimeLimit1"));
		d_pSaleTimeLimit2	= static_cast<CEGUI::RadioButton *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/TimeLimit2"));
		
		d_pSaleTimeLimit0->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiVendueWnd::handleSetTime0, ms_Singleton));
		d_pSaleTimeLimit1->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiVendueWnd::handleSetTime1, ms_Singleton));
		d_pSaleTimeLimit2->subscribeEvent(RadioButton::EventMouseButtonDown, Event::Subscriber(&KUiVendueWnd::handleSetTime2, ms_Singleton));
		d_pSaleTimeLimit0->subscribeEvent(RadioButton::EventMouseButtonUp, Event::Subscriber(&KUiVendueWnd::handleUpdateTax, ms_Singleton));
		d_pSaleTimeLimit1->subscribeEvent(RadioButton::EventMouseButtonUp, Event::Subscriber(&KUiVendueWnd::handleUpdateTax, ms_Singleton));
		d_pSaleTimeLimit2->subscribeEvent(RadioButton::EventMouseButtonUp, Event::Subscriber(&KUiVendueWnd::handleUpdateTax, ms_Singleton));
				
		d_pSaleTimeLimit0->setSelected(true);

		d_pBuyOnceGold = reinterpret_cast<CEGUI::TLEditbox *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouj"));
		d_pBuyOnceSlive = reinterpret_cast<CEGUI::TLEditbox *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikouy"));
		d_pBuyOnceCopper = reinterpret_cast<CEGUI::TLEditbox *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/yikout"));
		d_pBuyOnceGold->setIsOnlyNumber(true);
		d_pBuyOnceSlive->setIsOnlyNumber(true);
		d_pBuyOnceCopper->setIsOnlyNumber(true);

		d_pBuyGold =  reinterpret_cast<CEGUI::TLEditbox *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaij"));
		d_pBuySlive = reinterpret_cast<CEGUI::TLEditbox *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpaiy")) ;
		d_pBuyCopper = reinterpret_cast<CEGUI::TLEditbox *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/jingpait")) ;
		d_pBuyGold->setIsOnlyNumber(true);
		d_pBuySlive->setIsOnlyNumber(true);		
		d_pBuyCopper->setIsOnlyNumber(true);
		
		d_pTaxGold = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/taxj"));
		d_pTaxSlive = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/taxy"));
		d_pTaxCopper = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/taxt"));

		d_pBottomGold = reinterpret_cast<CEGUI::TLEditbox *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpaij"));
		d_pBottomSlive = reinterpret_cast<CEGUI::TLEditbox *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpaiy"));
		d_pBottomCopper = reinterpret_cast<CEGUI::TLEditbox *>(m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpait"));
		d_pBottomGold->setIsOnlyNumber(true);
		d_pBottomSlive->setIsOnlyNumber(true);
		d_pBottomCopper->setIsOnlyNumber(true);
/*
		d_pGoldCoin->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiVendueWnd::handleShown, ms_Singleton));
		d_pGoldCoin->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiVendueWnd::handleHidden, ms_Singleton));
		d_pSillerCoin->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiVendueWnd::handleShown, ms_Singleton));
		d_pSillerCoin->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiVendueWnd::handleHidden, ms_Singleton));
		d_pCopperCoin->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiVendueWnd::handleShown, ms_Singleton));
		d_pCopperCoin->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiVendueWnd::handleHidden, ms_Singleton));
//*/
		result = true;
	}
	return result;
}

//显示金钱数量 
void KUiVendueWnd::ShowMoneyValue( DWORD &dSubValue )
{
	int iGold = 0, iSiller = 0, iCopper = 0;
	int money = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	money -= dSubValue;

	if ( money < 0 )
	{
		ms_Singleton->ShowLook();
		return;
	}
	
	sysMoneyToUiMoney(money, iGold, iSiller, iCopper);
	d_pGoldCoin->setText(iToString(iGold));
	d_pSillerCoin->setText(iToString(iSiller));
	d_pCopperCoin->setText(iToString(iCopper));
	ms_Singleton->ShowLook();
}

//响应“可用物品“的多选框控件的鼠标单击事件
bool KUiVendueWnd::handleUseAble( const CEGUI::EventArgs& e )
{
	d_bUseAble ? d_bUseAble = false : d_bUseAble = true;
	d_pUseAbleChckBtn->setSelected( d_bUseAble );
	return true;
}


bool KUiVendueWnd::CancleSale( const CEGUI::EventArgs& e )
{
	//clearPageDate();
	IUIMDLDataset* pIDataset = NULL;
	IUIMDLDataset* pIOper = NULL;
	int nErr = m_pUiMDLManager->queryDataSet( vendue_dataset, &pIDataset );
	int nErrOper = m_pUiMDLManager->queryDataSet( vendue_operation, &pIOper );
	if ( nErr == success_errorcode && nErrOper == success_errorcode && m_curSelectItemID != -1 )
	{
		ZeroMemory( &d_buyInfo, sizeof( CLIENT_BUYGOODS_REQDATA ));
		d_buyInfo.recordId = m_curSelectItemID;
		UIMDLDatasetRecord& rRecord = pIDataset->findDataRecord( _vendueFindItemFromMDL, &d_buyInfo.recordId );
		if ( rRecord.nIndex >= 0 )
		{
			SEARCH_DB_RETDATA*  pData = (SEARCH_DB_RETDATA*)rRecord.pRecordData;
			d_buyInfo.price = pData->recordComData.onePrice;
			ShowRegComMsgBox( CANCLE_SALE, IS_CANCLE_SALE );
		}
	}
	else
	{
		char *message = KMessageCentre::GetMessage(vendue_message, SELECT_WARNING);
		KUiChannelCentre::GetSingleton().toSysMsg(message);
		return false;
	}
	return true;
}

// 把竞拍价填到下方的竞拍价中
void	KUiVendueWnd::CopyJingPaiJia( String jing, String yin, String tong )
{
	int njing	= atoi(Utf8ToAnsi(jing))*10000;
	int nyin	= atoi(Utf8ToAnsi(yin))*100;
	int ntong	= atoi(Utf8ToAnsi(tong));
	int nMoney = njing + nyin + ntong;

	int nIncrease = (int)(nMoney*d_baseInfo.bidPercent/100);
	if (nIncrease < 1)
		nMoney += 1;
	else
		nMoney += nIncrease;

	sysMoneyToUiMoney(nMoney, njing, nyin, ntong);
	
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpaij")->setText(iToString(njing));
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpaiy")->setText(iToString(nyin));
	m_pThisWnd->getChild("TaharezLook/itemvendueshop/buttomjingpait")->setText(iToString(ntong));
}

//显示jingpai页的column的名字
void	KUiVendueWnd::ShowJinPaiPageColumn( void )
{
	Point pos;
	Size  size;
	int index = static_cast<int>(KUiVendueWnd::look_page);
	if ( m_commodityPageList[index] == NULL )
	{
		return;
	}
	if ((static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemName() != NULL )
	{
		pos.d_x = 0;//VENDUE_PAGE_LEFT_OFFSET;
		pos.d_y = BAR_OFFSET-6;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemName()->setPosition( Absolute, pos );
		size.d_height = ITEM_COLUMN_HEIGHT;
		size.d_width  = ITEM_COLUMN_JIPAI_NAME_WIDTH;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemName()->setSize( Absolute, size );
		char *message = KMessageCentre::GetMessage(vendue_message, GOODS_NAME);
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemName()->setText( AnsiToUtf8(message) );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemName()->setVisible(true);
	}
	
	if ((static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevel() != NULL)
	{
		//pos.d_x = VENDUE_PAGE_LEFT_OFFSET +  
		pos.d_x = ITEM_COLUMN_JIPAI_NAME_WIDTH;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevel()->setPosition( Absolute, pos );
		size.d_width = ITEM_COLUMN_JIPAI_LEVEL_WIDTH;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevel()->setSize( Absolute, size );
		char *message = KMessageCentre::GetMessage(vendue_message, GOODS_LEVEL);
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevel()->setText( AnsiToUtf8(message) );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevel()->setVisible(true);
	}

	if ((static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTime() != NULL)
	{
		//pos.d_x = VENDUE_PAGE_LEFT_OFFSET +  
		pos.d_x = ITEM_COLUMN_JIPAI_NAME_WIDTH + 
					ITEM_COLUMN_JIPAI_LEVEL_WIDTH;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTime()->setPosition( Absolute, pos );
		size.d_width = ITEM_COLUMN_JIPAI_TIME_WIDTH;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTime()->setSize( Absolute, size );
		char *message = KMessageCentre::GetMessage(vendue_message, GOODS_TIME);
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTime()->setText( AnsiToUtf8(message) );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTime()->setVisible(true);
	}

	if ((static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner() != NULL)
	{
		//pos.d_x = VENDUE_PAGE_LEFT_OFFSET +  
		pos.d_x = ITEM_COLUMN_JIPAI_NAME_WIDTH + 
				  ITEM_COLUMN_JIPAI_LEVEL_WIDTH +
				  ITEM_COLUMN_JIPAI_TIME_WIDTH;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner()->setPosition( Absolute, pos );
		size.d_width = ITEM_COLUMN_JIPAI_OWNER_WIDTH;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner()->setSize(Absolute, size);
		char *message = KMessageCentre::GetMessage(vendue_message, GOODS_OWNER);
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner()->setText( AnsiToUtf8(message) );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner()->setVisible(true);
	}

	if ( (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice() != NULL )
	{
		//pos.d_x = VENDUE_PAGE_LEFT_OFFSET +  
		pos.d_x = ITEM_COLUMN_JIPAI_LEVEL_WIDTH + 
				  ITEM_COLUMN_JIPAI_NAME_WIDTH + 
				  ITEM_COLUMN_JIPAI_TIME_WIDTH +
				  ITEM_COLUMN_JIPAI_OWNER_WIDTH;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice()->setPosition( Absolute, pos );
		size.d_width = ITEM_COLUMN_JIPAI_PRICE_WIDTH;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice()->setSize(Absolute, size);
		char *message = KMessageCentre::GetMessage(vendue_message, GOODS_PRICE);
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice()->setText( AnsiToUtf8(message) );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice()->setVisible(true);
	}
}

//清空所有列名
void	KUiVendueWnd::ClearAllColumnText( void )
{
	for ( int i = 0; i < 1; i++ )
	{
		KUiVendueWnd::_pagetype index = static_cast<KUiVendueWnd::_pagetype>(i);
		KUiWnd *temp = m_commodityPageList[i];
		if ( m_commodityPageList[i] != NULL && 
			 (static_cast<KUiVenduePage *>(m_commodityPageList[i]))->getItemName() != NULL )
		{
			(static_cast<KUiVenduePage *>(m_commodityPageList[i]))->getItemName()->setVisible(false);
		}

		if ( m_commodityPageList[i] != NULL && 
			 (static_cast<KUiVenduePage *>(m_commodityPageList[i]))->getItemLevel() != NULL )
		{
			(static_cast<KUiVenduePage *>(m_commodityPageList[i]))->getItemLevel()->setVisible(false);
		}

		if ( m_commodityPageList[i] != NULL && 
			 (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner() != NULL )
		{
			(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner()->setVisible(false);
		}

		if ( m_commodityPageList[i] != NULL && 
			 (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice() != NULL )
		{
			(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice()->setVisible(false);
		}
	}
	return;
}

//确定要竞拍
void  KUiVendueWnd::BuyOk( void )
{
	m_iCurrentBarNum = 0;
	IUIMDLDataset* pIOper = NULL;
	int nErrOper = ms_Singleton->m_pUiMDLManager->queryDataSet( vendue_operation, &pIOper );
	if ( nErrOper == success_errorcode )
	{
		//当竞标价购买物品后金钱数目改变更新显示
		ms_Singleton->ShowMoneyValue(ms_Singleton->d_buyInfo.price);

		ms_Singleton->clearPageDate();
		pIOper->updateRecord( buy_oper, &(ms_Singleton->d_buyInfo), sizeof(CLIENT_BUYGOODS_REQDATA)  );
		ms_Singleton->m_curSelectItemID = -1;
		ZeroMemory(&ms_Singleton->d_buyInfo, sizeof( CLIENT_BUYGOODS_REQDATA ));
		((KUiVenduePage*)(ms_Singleton->m_commodityPageList[0]))->CancelCurHeight();
		ms_Singleton->clearYikouPrice();
		ms_Singleton->clearJingpaiPrice();
		ms_Singleton->clearBottomJingpaiPrice();
		ms_Singleton->WarningMsg();
	}
}


//显示确定界面
void  KUiVendueWnd::ShowRegComMsgBox( VENDUE_MESSAGES msg, VENDUE_MESSAGES buyOrsale )
{
	m_iCurrentBarNum = 0;
	KUiComMsgBox::GetSingleton().setModalStatus(true);
	KUiComMsgBox::Show();
	char warnMsg[COMMON_CLIENT_MSG_LEN_512];
	ZeroMemory(warnMsg, COMMON_CLIENT_MSG_LEN_512);

	switch(msg)
	{
	case BUY_ONE_BY_PRICE:
	case BUY_BY_PRICE:
		{
			int jing=0, yin=0, tong=0;
			sysMoneyToUiMoney(d_buyInfo.price, jing, yin, tong);
			sprintf(warnMsg, KMessageCentre::GetMessage(vendue_message, msg), jing, yin, tong, Utf8ToAnsi(d_curItemName));
			KUiComMsgBox::GetSingletonPtr()->setLayoutMsg(warnMsg);
		}
	    break;
	default:
		sprintf(warnMsg, KMessageCentre::GetMessage(vendue_message, msg));
		KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(warnMsg));
	    break;
	}

	char	yesButton[COMMON_CLIENT_MSG_LEN_32];
	strcpy(yesButton, KMessageCentre::GetMessage( vendue_message, YES ));
	char	*noButton = KMessageCentre::GetMessage( vendue_message, NO );
	KUiComMsgBox::GetSingleton().setBtnName( AnsiToUtf8(yesButton), AnsiToUtf8(noButton));
	switch(buyOrsale)
	{
	case IS_BUY_SOMETHING:
		KUiComMsgBox::GetSingleton().setFristBtnCallback(KUiVendueWnd::BuyOk);
		break;
	case IS_CANCLE_SALE:
		KUiComMsgBox::GetSingleton().setFristBtnCallback(KUiVendueWnd::CancelSale);
		break;
	default:
		break;
	}
}

//显示jingpai页的column的名字
void	KUiVendueWnd::ShowSearchOrVenduePageColumn( void )
{
	Point pos;
	Size  size;
	const XOffSet = 5;
	int index = static_cast<int>(KUiVendueWnd::look_page);
	if ( m_commodityPageList[index] == NULL )
	{
		return;
	}
	if ((static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemName() != NULL )
	{
		pos.d_x = VENDUE_PAGE_LEFT_OFFSET - XOffSet;
		pos.d_y = BAR_OFFSET-6;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemName()->setPosition( Absolute, pos );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemName()->setSize( Absolute,  (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemNameSize());
		char *message = KMessageCentre::GetMessage(vendue_message, GOODS_NAME);
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemName()->setText( AnsiToUtf8(message) );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemName()->setVisible(true);
	}
	
	if ((static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevel() != NULL)
	{
		pos.d_x = VENDUE_PAGE_LEFT_OFFSET - XOffSet +  
			      (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemNameSize().d_width;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevel()->setPosition( Absolute, pos );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevel()->setSize( Absolute, (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevelSize() );
		char *message = KMessageCentre::GetMessage(vendue_message, GOODS_LEVEL);
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevel()->setText( AnsiToUtf8(message) );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevel()->setVisible(true);
	}

	if ((static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTime() != NULL)
	{
		pos.d_x = VENDUE_PAGE_LEFT_OFFSET - XOffSet +  
			      (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemNameSize().d_width+
				  (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevelSize().d_width;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTime()->setPosition( Absolute, pos );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTime()->setSize( Absolute, (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTimeSize() );
		char *message = KMessageCentre::GetMessage(vendue_message, GOODS_TIME);
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTime()->setText( AnsiToUtf8(message) );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTime()->setVisible(true);
	}

	if ((static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner() != NULL)
	{
		pos.d_x = VENDUE_PAGE_LEFT_OFFSET - XOffSet +  
			      (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemNameSize().d_width+
				  (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevelSize().d_width +
				  (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTimeSize().d_width ;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner()->setPosition( Absolute, pos );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner()->setSize(Absolute, (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwnerSize() );
		char *message = KMessageCentre::GetMessage(vendue_message, GOODS_OWNER);
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner()->setText( AnsiToUtf8(message) );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwner()->setVisible(true);
	}

	if ( (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice() != NULL )
	{
		pos.d_x = VENDUE_PAGE_LEFT_OFFSET - XOffSet +  
			      (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemNameSize().d_width+
				  (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemLevelSize().d_width +
				  (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemTimeSize().d_width +
				  (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemOwnerSize().d_width ;
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice()->setPosition( Absolute, pos );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice()->setSize(Absolute, (static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPriceSize());
		char *message = KMessageCentre::GetMessage(vendue_message, GOODS_PRICE);
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice()->setText( AnsiToUtf8(message) );
		(static_cast<KUiVenduePage *>(m_commodityPageList[index]))->getItemPrice()->setVisible(true);
	}
	
}
/*
bool	KUiVendueWnd::handleShown( const CEGUI::EventArgs& args )
{
	((WindowEventArgs*)&args)->window->beginUpdate();
	return true;
}

bool	KUiVendueWnd::handleHidden( const CEGUI::EventArgs& args )
{
	((WindowEventArgs*)&args)->window->stopUpdate();
	return true;
}
//*/
bool	KUiVendueWnd::handleKeyDown( const CEGUI::EventArgs& args )
{
	return true;
}

bool	KUiVendueWnd::handleUpdateTax( const CEGUI::EventArgs& args )
{
	UpdateTax();
	return true;
}

//确定要取消拍买
void  KUiVendueWnd::CancelSale( void )
{
	m_iCurrentBarNum = 0;
	IUIMDLDataset* pIOper = NULL;
	int nErrOper = ms_Singleton->m_pUiMDLManager->queryDataSet( vendue_operation, &pIOper );
	if ( nErrOper == success_errorcode )
	{
		ms_Singleton->clearPageDate();
		pIOper->updateRecord( cancel_oper, &(ms_Singleton->d_buyInfo), sizeof(CLIENT_BUYGOODS_REQDATA)  );
		ms_Singleton->m_curSelectItemID = -1;
		ZeroMemory(&ms_Singleton->d_buyInfo, sizeof( CLIENT_BUYGOODS_REQDATA ));
		((KUiVenduePage*)(ms_Singleton->m_commodityPageList[0]))->CancelCurHeight();
		ms_Singleton->clearYikouPrice();
		ms_Singleton->clearJingpaiPrice();
		ms_Singleton->clearBottomJingpaiPrice();
		//ms_Singleton->ShowMoneyValue(ms_Singleton->d_buyInfo.price);
		ms_Singleton->WarningMsg();
	}
}

//弹出装备品质下拉列表框
bool  KUiVendueWnd::handleEquipQulity( const CEGUI::EventArgs& args )
{
	if ( NULL != d_pEquipMenu && !d_pEquipMenu->isVisible())
	{
		HideAllMenu();
		d_pEquipMenu->moveToFront();
		d_pEquipMenu->show();
	}
	else
		d_pEquipMenu->hide();

	return true;
}


bool  KUiVendueWnd::handleEquipSelect( const CEGUI::EventArgs& args )
{
	WindowEventArgs*wargs = (WindowEventArgs *)(&args);
	Window *tmpWnd = wargs->window;
	if ( tmpWnd == NULL )
	{
		return false;
	}
	if ( NULL != d_pEquipQulityBtn && NULL != d_pEquipMenu && NULL != d_pQualityTypeTxt)
	{
		if ( tmpWnd == d_pCommonEquipment && NULL != d_pCommonEquipment )
		{
			d_iEquipQulity = quality_common;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_COMMON);
			d_pQualityTypeTxt->setText(AnsiToUtf8(msg));
		}

		if ( tmpWnd == d_pRareEquipment )
		{
			d_iEquipQulity = quality_rare;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_RARE);
			d_pQualityTypeTxt->setText(AnsiToUtf8(msg));
		}

		if ( tmpWnd == d_pSetEquipment )
		{
			d_iEquipQulity = quality_set;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_SET);
			d_pQualityTypeTxt->setText(AnsiToUtf8(msg));
		}

		if ( tmpWnd == d_pEpicEquipment )
		{
			d_iEquipQulity = quality_epic;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_EPIC);
			d_pQualityTypeTxt->setText(AnsiToUtf8(msg));
		}

		if ( tmpWnd == d_pArtificialOne )
		{
			d_iEquipQulity = quality_artificial_1;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_ARTIFICIALONE);
			d_pQualityTypeTxt->setText(AnsiToUtf8(msg));
		}
/*
		if ( tmpWnd == d_pArtificialTwo )
		{
			d_iEquipQulity = quality_artificial_2;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_ARTIFICIALTWO);
			d_pQualityTypeTxt->setText(AnsiToUtf8(msg));
		}

		if ( tmpWnd == d_pArtificialThree )
		{
			d_iEquipQulity = quality_artificial_3;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_ARTIFICIALTHREE);
			d_pQualityTypeTxt->setText(AnsiToUtf8(msg));
		}
//*/
		d_pEquipMenu->hide();
	}

	return false;
}

bool KUiVendueWnd:: handleReq( const CEGUI::EventArgs& args )
{
	if ( NULL != d_pReqMenuBack && !d_pReqMenuBack->isVisible() )
	{
		HideAllMenu();
		d_pReqMenuBack->moveToFront();
		d_pReqMenuBack->show();
	}
	else
		d_pReqMenuBack->hide();

	return true;
}

bool KUiVendueWnd::handleEquip( const CEGUI::EventArgs& args )
{
	if ( NULL != d_pEquipMenuBack && !d_pEquipMenuBack->isVisible() )
	{
		HideAllMenu();
		d_pEquipMenuBack->moveToFront();
		d_pEquipMenuBack->show();
	}
	else
		d_pEquipMenuBack->hide();

	return true;
}


//弹出装备职业下拉列表框
bool  KUiVendueWnd::handleProfession( const CEGUI::EventArgs& args )
{
	if ( NULL != d_pProfessionMenu && !d_pProfessionMenu->isVisible())
	{
		HideAllMenu();
		d_pProfessionMenu->moveToFront();
		d_pProfessionMenu->show();
	}
	else
		d_pProfessionMenu->hide();

	return true;
}


bool  KUiVendueWnd::handleProfessionSelect( const CEGUI::EventArgs& args )
{
	WindowEventArgs*wargs = (WindowEventArgs *)(&args);
	Window *tmpWnd = wargs->window;
	if ( tmpWnd == NULL )
	{
		return false;
	}
	if ( NULL != d_pProfessionBtn && NULL != d_pProfessionMenu && NULL != d_pProfTypeTxt)
	{
		if ( tmpWnd == d_pProfessionCommon && NULL != d_pProfessionCommon )
		{
			d_iProf = -1;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_PROF_COMMON);
			d_pProfTypeTxt->setText(AnsiToUtf8(msg));
		}
		else if ( tmpWnd == d_pProfessionXF && NULL != d_pProfessionXF )
		{
			d_iProf = 1;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_PROF_XF);
			d_pProfTypeTxt->setText(AnsiToUtf8(msg));
		}
		else if ( tmpWnd == d_pProfessionXT && NULL != d_pProfessionXT )
		{
			d_iProf = 2;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_PROF_XT);
			d_pProfTypeTxt->setText(AnsiToUtf8(msg));
		}
		else if ( tmpWnd == d_pProfessionTS && NULL != d_pProfessionTS )
		{
			d_iProf = 8;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_PROF_TS);
			d_pProfTypeTxt->setText(AnsiToUtf8(msg));
		}
		else if ( tmpWnd == d_pProfessionZR && NULL != d_pProfessionZR )
		{
			d_iProf = 4;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_PROF_ZR);
			d_pProfTypeTxt->setText(AnsiToUtf8(msg));
		}
		else if ( tmpWnd == d_pProfessionSS && NULL != d_pProfessionSS )
		{
			d_iProf = 16;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_PROF_SS);
			d_pProfTypeTxt->setText(AnsiToUtf8(msg));
		}
		else if ( tmpWnd == d_pProfessionYS && NULL != d_pProfessionYS )
		{
			d_iProf = 32;
			char *msg = KMessageCentre::GetMessage(vendue_message, EQUIPMENT_PROF_YS);
			d_pProfTypeTxt->setText(AnsiToUtf8(msg));
		}

		d_pProfessionMenu->hide();
	}

	return false;
}

bool	KUiVendueWnd::handleRequestKind( const CEGUI::EventArgs& args )
{
	WindowEventArgs *wargs = (WindowEventArgs *)(&args);
	Window *tmpWnd = wargs->window;
	int nKind = 0;
	
	if (!tmpWnd || nKind < 0 || nKind >= enAuction_Kind_Count)
		return false;

	nKind = d_KindMap[tmpWnd];
	
	if (nKind == enAuction_Kind_Equipment)
	{	
		d_pEquipBtn->enable();
		d_pProfessionBtn->enable();
	}
	else
	{
		d_pEquipBtn->disable();
		d_pProfessionBtn->disable();
	}

	if ( d_pKindTypeTxt && d_pReqMenuBack )
	{
		d_pKindTypeTxt->setText( tmpWnd->getText() );		
		d_pReqMenuBack->hide();
	}

	ZeroMemory(d_ReqKind, AUCTION_MAX_ITEM_TYPE_LEN);
	strncpy(d_ReqKind, KUiCfgLoader::getSingleton().getAuctionCfg().type[nKind], sizeof(d_ReqKind));

	return true;
}

bool	KUiVendueWnd::handleEquipmentKind( const CEGUI::EventArgs& args )
{
	WindowEventArgs *wargs = (WindowEventArgs *)(&args);
	Window *tmpWnd = wargs->window;
	int nKind = 0;
	
	if (!tmpWnd || nKind < 0 || nKind >= enAuction_Kind_Count)
		return false;

	nKind = d_KindMap[tmpWnd];

	if ( d_pEquipmentTypeTxt && d_pEquipMenuBack )
	{
		d_pEquipmentTypeTxt->setText( tmpWnd->getText() );		
		d_pEquipMenuBack->hide();
	}
	
	ZeroMemory(d_ReqKind, AUCTION_MAX_ITEM_TYPE_LEN);
	strncpy(d_ReqKind, KUiCfgLoader::getSingleton().getAuctionCfg().type[nKind], sizeof(d_ReqKind));

	return false;
}

//单击拍卖行界面的事件
bool    KUiVendueWnd::handleClickVendue( const CEGUI::EventArgs& args )
{
	WindowEventArgs *temp = (WindowEventArgs *)(&args);
	if ( NULL != d_pEquipMenu  && d_pEquipMenu->isVisible() && temp->window != d_pEquipQulityBtn )
	{
		d_pEquipMenu->hide();
	}
	
	if ( NULL != d_pProfessionMenu  && d_pProfessionMenu->isVisible() && temp->window != d_pProfessionBtn )
	{
		d_pProfessionMenu->hide();
	}

	return true;
}

void	KUiVendueWnd::WarningMsg()
{
	char *message = KMessageCentre::GetMessage(vendue_message, WARNING_MESSAGE);
	KUiChannelCentre::GetSingleton().toSysMsg(message);
}

void	KUiVendueWnd::UpdateTax()
{
	int vendueTime = vendue_time_short;
	if (m_nSaleTime	== d_baseInfo.time[vendue_time_short]*ONE_HOUR)
		vendueTime = vendue_time_short;
	else if (m_nSaleTime == d_baseInfo.time[vendue_time_middle]*ONE_HOUR)
		vendueTime = vendue_time_middle;
	else
		vendueTime = vendue_time_long;

	float taxPercent = (float)(d_baseInfo.tax[vendueTime])/100.0f;
	int nJingpaijia = getJingpaiPrice();
	int nTax = (int)(nJingpaijia*taxPercent+0.5);
	
	int jing, yin, tong;
	sysMoneyToUiMoney( nTax, jing, yin, tong );

	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxj" )->setText(iToString(jing));
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxy" )->setText(iToString(yin));
	m_pThisWnd->getChild( "TaharezLook/itemvendueshop/taxt" )->setText(iToString(tong));
}

void	KUiVendueWnd::HideAllMenu()
{
	d_pEquipMenu->hide();
	d_pEquipMenuBack->hide();
	d_pReqMenuBack->hide();
	d_pProfessionMenu->hide();
}

void KUiVendueWnd::Hide( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		TLGameObject* pGO = (TLGameObject*)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/itemvendueshop/itemicon" );
		if ( pGO != NULL )
		{
			pGO->clear();
			pGO->show();
			ms_Singleton->clearYikouPrice();
			ms_Singleton->clearJingpaiPrice();
			ms_Singleton->clearBottomJingpaiPrice();
			ms_Singleton->m_pThisWnd->deactivate();
		}
	}
	
	KUiWndSingleton<KUiVendueWnd>::Hide();
}
