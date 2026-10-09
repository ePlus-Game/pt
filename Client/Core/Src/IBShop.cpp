//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:4:17   17:27
//      File_base        : IBShop
//      File_ext         : cpp
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "IBShop.h"
#include "KItemGenerator.h"
#include "IBShopUtil.h"
#include "KCore.h"

IBShop& IBShop::Singleton()
{
	static IBShop	shop;
	return shop;
}

void IBShop::LoadIBShopFromDBReq()
{
	_IBShopHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IBShop;
	DBHeader.nIBShopID = ibshop_data_shelf;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_LOADSHELF );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void IBShop::LoadIBShopFromDBRet(int nDbOpeRst, IProcRet* pRet)
{
	if(!nDbOpeRst)
		return;
	
	int row = pRet->GetRowCount();
	int col = pRet->GetColCount();
	m_ShelfCount = (BYTE)row;

	if (col < SHELF_ATTR_NUM)
		return;

	for (int nRow = 0; nRow < m_ShelfCount && nRow < MAX_SHELF_NUM; nRow++)
	{
		int nCol = 0;
		int nIdx = 0;
		pRet->GetData(nRow, nCol++, nIdx);
		pRet->GetData(nRow, nCol++, m_GoodsShelfs[nIdx].Shelf.ShelfName, 
			sizeof(m_GoodsShelfs[nIdx].Shelf.ShelfName) );
		for (int i = 0; i < MAX_PANELCOUNT_PERSHELF; i++)
		{
			int PanelIdx = 0;
			pRet->GetData(nRow, nCol++, PanelIdx);
			m_GoodsShelfs[nIdx].Shelf.PanelIdxList[i] = PanelIdx;
		}
	}

	m_ShopVersion = UNIX_TMIE_STAMP;

	for (int i = 0; i < m_ShelfCount; i++)
	{
		LoadIBShopItemFromDBReq(i);
	}
}

void IBShop::LoadPanelFromDBReq()
{
	_IBShopHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IBShop;
	DBHeader.nIBShopID = ibshop_data_panel;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_LOADPANEL );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void IBShop::LoadPanelFromDBRet(int nDbOpeRst,  IProcRet* pRet)
{
	if(!nDbOpeRst)
		return;

	int row = pRet->GetRowCount();
	int col = pRet->GetColCount();
	m_PanelCount = (BYTE)row;

	if (col < PANEL_ATTR_NUM)
		return;

	for (int nRow = 0; nRow < m_PanelCount && nRow < MAX_PANEL_NUM; nRow++)
	{
		int nCol = 0;
		int PanelID, PanelLeft, PanelTop, PanelWidth, PanelHeight, PanelMarge;
		int CellWidth, CellHeight, TitleWidth, TitleHeight;
		pRet->GetData(nRow, nCol++, PanelID);
		pRet->GetData(nRow, nCol++, PanelLeft);
		pRet->GetData(nRow, nCol++, PanelTop);
		pRet->GetData(nRow, nCol++, PanelWidth);
		pRet->GetData(nRow, nCol++, PanelHeight);
		pRet->GetData(nRow, nCol++, PanelMarge);
		pRet->GetData(nRow, nCol++, CellWidth);
		pRet->GetData(nRow, nCol++, CellHeight);
		pRet->GetData(nRow, nCol++, TitleWidth);
		pRet->GetData(nRow, nCol++, TitleHeight);
		pRet->GetData(nRow, nCol++, m_Panels[nRow].TitleName, 
			sizeof(m_Panels[nRow].TitleName));

		m_Panels[nRow].PanelID		= PanelID;
		m_Panels[nRow].PanelLeft	= PanelLeft;
		m_Panels[nRow].PanelTop		= PanelTop;
		m_Panels[nRow].PanelWidth	= PanelWidth;
		m_Panels[nRow].PanelHeight	= PanelHeight;
		m_Panels[nRow].PanelMarge	= PanelMarge;
		m_Panels[nRow].CellWidth	= CellWidth;
		m_Panels[nRow].CellHeight	= CellHeight;
		m_Panels[nRow].TitleWidth	= TitleWidth;
		m_Panels[nRow].TitleHeight	= TitleHeight;
	}

	m_PanelVersion = UNIX_TMIE_STAMP;
}

void IBShop::LoadContentStyleFromDBReq()
{
	_IBShopHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IBShop;
	DBHeader.nIBShopID = ibshop_data_contentstyle;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_LOADCONTENTSTYLE );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void IBShop::LoadContentStyleFromDBRet(int nDbOpeRst, IProcRet* pRet)
{
	if(!nDbOpeRst)
		return;

	int row = pRet->GetRowCount();
	int col = pRet->GetColCount();
	m_ContentStyleCount = (BYTE)row;

	if (col < CONTENTSTYLE_ATTR_NUM)
		return;

	for (int nRow = 0; nRow < m_ContentStyleCount && nRow < MAX_CONTENT_STYLE_NUM; nRow++)
	{
		int nCol = 0;
		int StyleID;
		pRet->GetData(nRow, nCol++, StyleID);
		pRet->GetData(nRow, nCol++, m_ContentStyle[nRow].Content, 
			sizeof(m_ContentStyle[nRow].Content));
		m_ContentStyle[nRow].StyleID = StyleID;
	}

	m_ContentStyleVersion = UNIX_TMIE_STAMP;
}

void IBShop::LoadIBShopItemFromDBReq(int nShelfIdx)
{
	if (!IBShopUtil::IsShelfIdxValid(nShelfIdx))
		return;

	_IBShopHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IBShop;
	DBHeader.nIBShopID = ibshop_data_item;
	DBHeader.nIBShopShelfIdx = nShelfIdx;

	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_LOADITEM );
	pParam->Push(nShelfIdx);
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void IBShop::LoadIBShopItemFromDBRet(int nDbOpeRst, int nShelfIdx, IProcRet* pRet)
{	
	if(!nDbOpeRst)
		return;

	int row = pRet->GetRowCount();
	int col = pRet->GetColCount();

	if (col < ITEM_ATTR_NUM)
		return;

	ClearGoodsInShelf(nShelfIdx);

	for (int nRow = 0; nRow < row && nRow < MAX_GOODSCOUNT_PERSHELF; nRow++)
	{
		int nCol = 0;
		IBGoods	goods;
		pRet->GetData(nRow, nCol++, goods.PanelIndex);
		pRet->GetData(nRow, nCol++, goods.IndexInPanel);
		pRet->GetData(nRow, nCol++, goods.Style);
		pRet->GetData(nRow, nCol++, goods.Id.ID);
		pRet->GetData(nRow, nCol++, goods.Id.Genera);
		pRet->GetData(nRow, nCol++, goods.Id.Detail);
		pRet->GetData(nRow, nCol++, goods.Id.Particular);
		pRet->GetData(nRow, nCol++, goods.Id.Level);
		pRet->GetData(nRow, nCol++, goods.Info.Price);
		pRet->GetData(nRow, nCol++, goods.Info.Discount);
		pRet->GetData(nRow, nCol++, goods.Info.Status);
		pRet->GetData(nRow, nCol++, goods.Info.Label);

		IBGoods_ListEntry *pListEntry = IBShopUtil::CreateGoodsListEntry(goods);	

		if(NULL != pListEntry)
		{
			IBShopUtil::PushToGoodsListHead(pListEntry, m_GoodsShelfs[nShelfIdx].pGoodsList);
			m_GoodsShelfs[nShelfIdx].GoodsCount++;
		}
	}

	m_GoodsShelfs[nShelfIdx].Shelf.ShelfVersion = UNIX_TMIE_STAMP;

}

void IBShop::ClearGoodsInShelf(int nShelfIdx)
{
	if (IBShopUtil::IsShelfIdxValid(nShelfIdx))
	{
		m_GoodsShelfs[nShelfIdx].GoodsCount = 0;
		m_GoodsShelfs[nShelfIdx].Shelf.ShelfVersion = INVALID_SHELF_VERSION;
		IBShopUtil::ClearGoodsList(m_GoodsShelfs[nShelfIdx].pGoodsList);
		m_GoodsShelfs[nShelfIdx].pGoodsList = NULL;
	}
}

void IBShop::ClearAllGoodsInfo()
{	
	m_ShopVersion = INVALID_SHOP_VERSION;
	for(int nShelfIdx = 0; nShelfIdx < MAX_SHELF_NUM; ++nShelfIdx)
	{
		ClearGoodsInShelf(nShelfIdx);
	}
}

int	IBShop::GetGoodsInShelf(int nShelfIdx, void *pOutBuf, int &nGoodsCount)
{
	if(NULL == pOutBuf)
	{
		nGoodsCount = 0;
		return enIBShopErr_UnInit;
	}

	if(m_GoodsShelfs[nShelfIdx].GoodsCount > MAX_GOODSCOUNT_PERSHELF)
	{
		nGoodsCount = 0;
		return enIBShopErr_GoodsCountTooMany;
	}
	
	if (m_GoodsShelfs[nShelfIdx].GoodsCount <= 0)
	{
		nGoodsCount = 0;
		return enIBShopErr_CannotFindGoods;
	}

	IBGoods	*pGoods = (IBGoods*)pOutBuf;
	IBGoods_ListEntry	*pEntry = m_GoodsShelfs[nShelfIdx].pGoodsList;

	while(NULL != pEntry)
	{
		*pGoods = pEntry->goods;
		++pGoods;
		pEntry = pEntry->pNextEntry;
	}

	nGoodsCount = m_GoodsShelfs[nShelfIdx].GoodsCount;

	return enIBShopErr_None;
}



