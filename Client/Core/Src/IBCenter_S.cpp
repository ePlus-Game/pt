//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:4:17   17:08
//      File_base        : IBCenter_S
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
#include "IBCenter_S.h"
#include "IBShopUtil.h"
#include "KItemGenerator.h"
#include "CoreRelated.h"
#include "IBLog.h"
#include "OnceIBItemMgr.h"


using namespace std;

IBCenter_S::CLIENTPROTPROC IBCenter_S::m_ClientProtProc[enIB_CSProt_End];

IBCenter_S& IBCenter_S::Singleton()
{
	static IBCenter_S	center;
	return center;
}

IBCenter_S::IBCenter_S()
: m_PanelCount(0)
//, m_preLoadPanelTime(0)
//, m_preLoadStyleTime(0)
, m_ContentStyleCount(0)
, m_CreditToTicketRate(0)
, m_PanelVersion(INVALID_PANEL_VERSION)
, m_ContentStyleVersion(INVALID_STYLE_VERSION)
{
	for (int nShopIdx = 0; nShopIdx < enSHOP_NUM; ++nShopIdx)
	{
		m_ShelfCount[nShopIdx]			= 0;
		m_ShopVersion[nShopIdx]		= INVALID_SHOP_VERSION;
		
		/*m_preLoadShopTime[nShopIdx]	= 0;
		for (int i = 0; i < MAX_SHELF_NUM; i++)
		{
			m_preLoadShelfTime[nShopIdx][i] = 0;
		}//*/
	}

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		m_preBuyTime[i] = 0;
	}

	memset(&m_Panels, 0, sizeof(m_Panels));
	memset(&m_ContentStyle, 0, sizeof(m_ContentStyle));
	memset(&m_GoodsShelfs, 0, sizeof(m_GoodsShelfs));
	memset(&m_ClientProtProc, 0, sizeof(m_ClientProtProc));

	memset(&m_PTicket, 0, sizeof(m_PTicket));

	BindProtocolFunc();
}

IBCenter_S::~IBCenter_S()
{
	ClearAllShop();
}

void IBCenter_S::Init()
{
	for (int nShopIdx = 0; nShopIdx < enSHOP_NUM; ++nShopIdx)
	{
		LoadIBShopFromDBReq(nShopIdx);
	}
	LoadPanelFromDBReq();
	LoadContentStyleFromDBReq();

	static ConfigManager &cfg = ConfigManager::Singleton();
	m_CreditToTicketRate = cfg.GetIBGlobalVariable(ib_global_var_credit_to_ticket_rate);
	cfg.GetIBTicketId(&m_PTicket.Genera,&m_PTicket.Detail,&m_PTicket.Particular,&m_PTicket.Level);
	
	if (m_CreditToTicketRate <= 0)
		m_CreditToTicketRate = 100;
}

void IBCenter_S::BindProtocolFunc()
{
	m_ClientProtProc[enIB_CSProt_LoadShelf]			= &IBCenter_S::LoadShelfReq;
	m_ClientProtProc[enIB_CSProt_LoadPanel]			= &IBCenter_S::LoadPanelReq;
	m_ClientProtProc[enIB_CSProt_LoadContentStyle]	= &IBCenter_S::LoadContentStyleReq;
	m_ClientProtProc[enIB_CSProt_LoadGoodsInShelf]	= &IBCenter_S::LoadGoodsInShelfReq;
	m_ClientProtProc[enIB_CSProt_BuyGoods]			= &IBCenter_S::BuyGoodsReq;
	m_ClientProtProc[enIB_CSProt_Chongzhi]			= &IBCenter_S::ChongZhi;
}

/*
void IBCenter_S::Active()
{

	DWORD	dwCurTime = UNIX_TMIE_STAMP;

	if(dwCurTime - m_preClearGoodsTime >= DEF_CLEARGOODS_TIMEINTERVAL)
	{
		m_preClearGoodsTime = dwCurTime;
	}

}
//*/

void IBCenter_S::ProcClientProtocol(int nPlayerIdx, BYTE *pMsg, int nSize)
{
	PVARLEN_PROTOCOL_HEADER	pHeader = (PVARLEN_PROTOCOL_HEADER)pMsg;

	_ASSERT(pHeader->subProtocol > enIB_CSProt_Begin && pHeader->subProtocol < enIB_CSProt_End);
	
	if(pHeader->subProtocol > enIB_CSProt_Begin && 
	   pHeader->subProtocol < enIB_CSProt_End	&& 
	   m_ClientProtProc[pHeader->subProtocol])
		(this->*m_ClientProtProc[pHeader->subProtocol])(nPlayerIdx, pMsg, nSize);
}

void IBCenter_S::ErrCodeToClient(int nPlayerIdx, int nErrCode, int nPlusCode )
{
	S2C_ERRCODE s2cSync;

	s2cSync.header.protocol = s2c_IB_family;
	s2cSync.header.subProtocol = enIB_CSProt_ErrCode;
	s2cSync.ErrCode = nErrCode;
	s2cSync.PlusCode = nPlusCode;
	s2cSync.header.len = sizeof(s2cSync) - PROTOCOL_SIZE;

	SendDataToClient(nPlayerIdx, &s2cSync, s2cSync.header.len + PROTOCOL_SIZE);
}

void IBCenter_S::LoadShelfReq(int nPlayerIdx, BYTE *pData, int nSize)
{
	C2S_LOADSHELF *pc2sReq = (C2S_LOADSHELF*)pData;
	int nShopIdx = pc2sReq->nShopIdx;
	
	if (!IBShopUtil::IsShopIndexValid(nShopIdx))
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_NoShop);
		return;
	}

	/*if (pc2sReq->IBShopVersion != INVALID_SHOP_VERSION)
	{
		if (!CheckOperationTime(nPlayerIdx, global_var_ibshop_timeinterval, m_preLoadShopTime[nShopIdx]))
			return;
	}//*/
	
	if (m_ShopVersion[nShopIdx] == INVALID_SHOP_VERSION || 
		m_ShelfCount[nShopIdx] <= 0)
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_OnLoading);
		return;
	}

	const int S2C_BUF_SIZE = sizeof(S2C_LOADSHELF) + \
		MAX_SHELF_NUM * sizeof(Load_IBShelf_Ret);
	char buf[S2C_BUF_SIZE];
	memset(buf, 0, sizeof(buf));

	S2C_LOADSHELF *ps2cRet = (S2C_LOADSHELF*)buf;
	Load_IBShelf_Ret *pShelf = (Load_IBShelf_Ret*)ps2cRet->Shelf;
	
	ps2cRet->header.protocol = s2c_IB_family;
	ps2cRet->header.subProtocol = enIB_CSProt_LoadShelf;
	ps2cRet->IBShopVersion = m_ShopVersion[nShopIdx];
	ps2cRet->nShopIdx = nShopIdx;

	if (pc2sReq->IBShopVersion == m_ShopVersion[nShopIdx])
	{
		ps2cRet->ShelfCount = 0;
	}
	else
	{
		ps2cRet->ShelfCount = m_ShelfCount[nShopIdx];
		for (int i = 0; i < ps2cRet->ShelfCount && i < MAX_SHELF_NUM; i++)
		{
			pShelf->ShelfIdx		= (BYTE)i;
			//pShelf->ShelfVersion	= m_GoodsShelfs[nShopIdx][i].Shelf.ShelfVersion;
			for (int j = 0; j < MAX_PANELCOUNT_PERSHELF; j++)
			{
				pShelf->PanelIdxList[j] = m_GoodsShelfs[nShopIdx][i].Shelf.PanelIdxList[j];
			}
			strncpy(pShelf->ShelfName, m_GoodsShelfs[nShopIdx][i].Shelf.ShelfName, sizeof(pShelf->ShelfName));
			pShelf++;
		}
	}

	ps2cRet->header.len = ps2cRet->size() - PROTOCOL_SIZE;

	if (CompressProtocol((BYTE*)ps2cRet, ps2cRet->header.len + PROTOCOL_SIZE, S2C_BUF_SIZE, sizeof(ps2cRet->header)))
	{
		SendDataToClient(nPlayerIdx, ps2cRet, ps2cRet->header.len + PROTOCOL_SIZE);
	}
}

void IBCenter_S::LoadPanelReq(int nPlayerIdx, BYTE *pData, int nSize)
{
	C2S_LOADPANEL *pc2sReq = (C2S_LOADPANEL*)pData;
	/*if (pc2sReq->PanelVersion != INVALID_PANEL_VERSION)
	{
		if (!CheckOperationTime(nPlayerIdx, 
			global_var_ibshop_panel_timeinterval, m_preLoadPanelTime))
			return;
	}//*/
	
	if (m_PanelVersion == INVALID_PANEL_VERSION ||
		m_PanelCount <= 0)
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_OnLoading);
		return;
	}

	const int S2C_BUF_SIZE = sizeof(S2C_LOADPANEL) + MAX_PANEL_NUM * sizeof(Panel);
	char buf[S2C_BUF_SIZE];
	
	S2C_LOADPANEL *ps2cRet = (S2C_LOADPANEL*)buf;
	Panel *pPanel = (Panel*)ps2cRet->panel;

	if ( pc2sReq->PanelVersion == m_PanelVersion )
	{
		ps2cRet->header.protocol = s2c_IB_family;
		ps2cRet->header.subProtocol = enIB_CSProt_LoadPanel;
		ps2cRet->PanelVersion = pc2sReq->PanelVersion;
		ps2cRet->PanelCount = 0;
		ps2cRet->header.len = ps2cRet->size() - PROTOCOL_SIZE;
	}
	else
	{
		ps2cRet->header.protocol = s2c_IB_family;
		ps2cRet->header.subProtocol = enIB_CSProt_LoadPanel;
		ps2cRet->PanelCount = m_PanelCount;
		ps2cRet->PanelVersion = m_PanelVersion;
		for (int i = 0; i < ps2cRet->PanelCount; i++)
		{
			*pPanel = m_Panels[i];
			pPanel++;
		}

		ps2cRet->header.len = ps2cRet->size() - PROTOCOL_SIZE;
	}
	
	if (CompressProtocol((BYTE*)ps2cRet, ps2cRet->header.len + PROTOCOL_SIZE, S2C_BUF_SIZE, sizeof(ps2cRet->header)))
	{
		SendDataToClient(nPlayerIdx, ps2cRet, ps2cRet->header.len + PROTOCOL_SIZE);
	}
}

void IBCenter_S::LoadContentStyleReq(int nPlayerIdx, BYTE *pData, int nSize)
{
	C2S_LOADCONTENTSTYLE *pc2sReq = (C2S_LOADCONTENTSTYLE*)pData;
	
	/*if (pc2sReq->ContentStyleVersion != INVALID_STYLE_VERSION)
	{
		if (!CheckOperationTime(nPlayerIdx, 
			global_var_ibshop_style_timeinterval, m_preLoadStyleTime))
			return;
	}//*/

	if (m_ContentStyleVersion == INVALID_STYLE_VERSION ||
		m_ContentStyleCount <= 0)
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_OnLoading);
		return;
	}

	const int S2C_BUF_SIZE = sizeof(S2C_LOADCONTENTSTYLE) + \
		MAX_CONTENT_STYLE_NUM * sizeof(ContentStyle);
	char buf[S2C_BUF_SIZE];

	S2C_LOADCONTENTSTYLE *ps2cRet = (S2C_LOADCONTENTSTYLE*)buf;
	ContentStyle *pContentStyle = (ContentStyle*)ps2cRet->contentStyle;

	if ( pc2sReq->ContentStyleVersion == m_ContentStyleVersion )
	{
		ps2cRet->header.protocol = s2c_IB_family;
		ps2cRet->header.subProtocol = enIB_CSProt_LoadContentStyle;
		ps2cRet->ContentStyleVersion = pc2sReq->ContentStyleVersion;
		ps2cRet->ContentStyleCount = 0;
		ps2cRet->header.len = ps2cRet->size() - PROTOCOL_SIZE;
	}
	else
	{
		ps2cRet->header.protocol = s2c_IB_family;
		ps2cRet->header.subProtocol = enIB_CSProt_LoadContentStyle;
		ps2cRet->ContentStyleCount = m_ContentStyleCount;
		ps2cRet->ContentStyleVersion = m_ContentStyleVersion;
		for (int i = 0; i < ps2cRet->ContentStyleCount; i++)
		{
			pContentStyle->StyleID = m_ContentStyle[i].StyleID;
			strncpy(pContentStyle->Content, m_ContentStyle[i].Content, 
				sizeof(pContentStyle->Content));
			pContentStyle++;
		}

		ps2cRet->header.len = ps2cRet->size() - PROTOCOL_SIZE;
	}
	
	if (CompressProtocol((BYTE*)ps2cRet, ps2cRet->header.len + PROTOCOL_SIZE, S2C_BUF_SIZE, sizeof(ps2cRet->header)))
	{
		SendDataToClient(nPlayerIdx, ps2cRet, ps2cRet->header.len + PROTOCOL_SIZE);
	}
}

void IBCenter_S::LoadGoodsInShelfReq(int nPlayerIdx, BYTE *pData, int nSize)
{
	C2S_LOADGOODSINSHELF *pc2sReq = (C2S_LOADGOODSINSHELF*)pData;
	int nShopIdx = pc2sReq->nShopIdx;
	int nShelfIdx = pc2sReq->ShelfIdx;
	
	if (!IBShopUtil::IsShopIndexValid(nShopIdx) || !IBShopUtil::IsShelfIdxValid(nShelfIdx))
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_NoShop);
		return;
	}

	/*if (pc2sReq->ShelfVersion != INVALID_SHELF_VERSION)
	{
		if (!CheckOperationTime(nPlayerIdx, 
			global_var_ibshop_shelf_timeinterval, m_preLoadShelfTime[nShopIdx][nShelfIdx]))
			return;
	}//*/

	if (m_ShopVersion[nShopIdx] == INVALID_SHOP_VERSION || 
		m_GoodsShelfs[nShopIdx][nShelfIdx].Shelf.ShelfVersion == INVALID_SHELF_VERSION ||
		m_GoodsShelfs[nShopIdx][nShelfIdx].GoodsCount == 0)
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_OnLoading);
		return;
	}


	const int S2C_BUF_SIZE = sizeof(S2C_LOADGOODSINSHELF) + \
		MAX_GOODSCOUNT_PERSHELF*sizeof(IBGoods);
	char buf[S2C_BUF_SIZE];

	S2C_LOADGOODSINSHELF *ps2cRet = (S2C_LOADGOODSINSHELF*)buf;
	IBGoods *pRecord = (IBGoods*)ps2cRet->Goods;

	if (pc2sReq->ShelfVersion == m_GoodsShelfs[nShopIdx][nShelfIdx].Shelf.ShelfVersion)
	{
		ps2cRet->header.protocol = s2c_IB_family;
		ps2cRet->header.subProtocol = enIB_CSProt_LoadGoodsInShelf;
		ps2cRet->GoodsCount = 0;
		ps2cRet->ShopIdx = (BYTE)nShopIdx;
		ps2cRet->ShelfIdx = (BYTE)nShelfIdx;
		ps2cRet->ShopVersion = m_ShopVersion[nShopIdx];
		ps2cRet->ShelfVersion = pc2sReq->ShelfVersion;
		ps2cRet->header.len = ps2cRet->size() - PROTOCOL_SIZE;

		if (CompressProtocol((BYTE*)ps2cRet, ps2cRet->header.len + PROTOCOL_SIZE, S2C_BUF_SIZE, sizeof(ps2cRet->header)))
		{
			SendDataToClient(nPlayerIdx, ps2cRet, ps2cRet->header.len + PROTOCOL_SIZE);
		}
		return;
	}

	int	nGoodsCount;
	int	nRet = GetGoodsInShelf(nShopIdx, nShelfIdx, pRecord, nGoodsCount);
	if(enIBShopErr_None != nRet)
	{
		ErrCodeToClient(nPlayerIdx, nRet);
	}
	else
	{		
		ps2cRet->header.protocol = s2c_IB_family;
		ps2cRet->header.subProtocol = enIB_CSProt_LoadGoodsInShelf;
		ps2cRet->ShopIdx = (BYTE)nShopIdx;
		ps2cRet->ShelfIdx = (BYTE)nShelfIdx;
		ps2cRet->GoodsCount = (short int)nGoodsCount;
		ps2cRet->ShopVersion = m_ShopVersion[nShopIdx];
		ps2cRet->ShelfVersion = m_GoodsShelfs[nShopIdx][nShelfIdx].Shelf.ShelfVersion;

		ps2cRet->header.len = ps2cRet->size() - PROTOCOL_SIZE;

		if (CompressProtocol((BYTE*)ps2cRet, ps2cRet->header.len + PROTOCOL_SIZE, S2C_BUF_SIZE, sizeof(ps2cRet->header)))
		{
			SendDataToClient(nPlayerIdx, ps2cRet, ps2cRet->header.len + PROTOCOL_SIZE);
		}
	}
}

//int IBCenter_S::CanBuy(int nPlayerIdx, IBGoods_Id Goods, DWORD nShelfVersion, int nShopIdx, int nShelfIdx)
int IBCenter_S::CanBuy(int nPlayerIdx, C2S_BUYGOODS* goods)
{
	int nShopIdx = goods->ShopIdx;
	int nShelfIdx = goods->ShelfIdx;

	if (goods->Goods.Genera != item_ib)
		return enIBShopErr_IsNotIBGoods;

	if (!IBShopUtil::IsShopIndexValid(nShopIdx))
		return enIBShopErr_NoShop;
	
	if (!IBShopUtil::IsShelfIdxValid(nShelfIdx))
		return enIBShopErr_NoShelf;

	if (m_GoodsShelfs[nShopIdx][nShelfIdx].Shelf.ShelfVersion != goods->ShelfVersion ||
		m_ShopVersion[nShopIdx] != goods->ShopVersion)
	{
		RefreshClientGoodsInShelf(nPlayerIdx, nShopIdx, nShelfIdx);
		return enIBShopErr_UpdateShelf;
	}

	return enIBShopErr_None;
}

void IBCenter_S::RefreshClientGoodsInShelf(int nPlayerIdx, int nShopIdx, int nShelfIdx)
{
	const int S2C_BUF_SIZE = sizeof(S2C_LOADGOODSINSHELF) + MAX_GOODSCOUNT_PERSHELF*sizeof(IBGoods);
	char buf[S2C_BUF_SIZE];

	S2C_LOADGOODSINSHELF *ps2cRet = (S2C_LOADGOODSINSHELF*)buf;
	IBGoods *pRecord = (IBGoods*)ps2cRet->Goods;

	int	nGoodsCount;
	int	nRet = GetGoodsInShelf(nShopIdx, nShelfIdx, pRecord, nGoodsCount);
	if(enIBShopErr_None != nRet)
	{
		ErrCodeToClient(nPlayerIdx, nRet);
	}
	else
	{		
		ps2cRet->header.protocol = s2c_IB_family;
		ps2cRet->header.subProtocol = enIB_CSProt_LoadGoodsInShelf;
		ps2cRet->GoodsCount = (short int)nGoodsCount;
		ps2cRet->ShelfIdx = (BYTE)nShelfIdx;
		ps2cRet->ShopVersion = m_ShopVersion[nShopIdx];
		ps2cRet->ShelfVersion = m_GoodsShelfs[nShopIdx][nShelfIdx].Shelf.ShelfVersion;

		ps2cRet->header.len = ps2cRet->size() - PROTOCOL_SIZE;
		SendDataToClient(nPlayerIdx, ps2cRet, ps2cRet->header.len + PROTOCOL_SIZE);
	}
}

void	IBCenter_S::ChongZhi(int nPlayerIdx, BYTE *pData, int nSize)
{
	if ( IsValidPlayer( nPlayerIdx ) )
	{
		Player[nPlayerIdx].ChongZhiLeftMoney();
	}
}

void IBCenter_S::BuyGoodsReqCore(int nPlayerIdx,BYTE * pData,int nSize)
{
	C2S_BUYGOODS *pc2sReq = (C2S_BUYGOODS*)pData;
	IBGoods_Id goods = pc2sReq->Goods;
	int cost = 0;

	KItem tmpItem;
	BOOL bOk = g_ItemGen.Gen_Item( goods.Genera, goods.Detail, goods.Particular, goods.Level, 1, &tmpItem );
	if (bOk == FALSE)
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_CannotFindGoods);
		return;
	}
	/*
	else if (tmpItem.GetIBUseCount() > 0)
	{
		pc2sReq->ReqNum = 1;
	}
	else if (pc2sReq->ReqNum > tmpItem.GetMaxItemCount())
	{
		// 购买上限为物品叠加数
		pc2sReq->ReqNum = tmpItem.GetMaxItemCount();
		ErrCodeToClient(nPlayerIdx, enIBShopErr_ErrReqBuyNum);
	}//*/
	// 购买数量检查 -----> end

	// 购买安全检查 -----> begin
	if( pc2sReq->bOnecItem )
	{
		//一次性ib道具地图判断
		if ( tmpItem.IsMapArea(nPlayerIdx) )
		{
			tmpItem.ItemErrCodeToClient( nPlayerIdx, item_inlay_error_targetitem_rule );
			return;
		}

		// 一次购买物品检查（包括还款）
		// 走通用购买流程
		cost = pc2sReq->Price;

		pc2sReq->ReqNum = 1;

		if ( GetMoneyType(pc2sReq->ShopIdx ) != jinshanbi )
		{
			ErrCodeToClient(nPlayerIdx, enIBShopErr_CannotFindGoods);
			return;
		}
		
		// 还款、保险安全判断
		if ( (goods.ID == TRADE_ID_RETURN_CREDIT || goods.ID == TRADE_ID_INSURANCE) && cost <= 0)
		{
			ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughJinShanBiReturnFailed);
			return;
		}
	
		// IB商店以外的一次购买物品安全判断
		// 还款例外，还款价格由用户提供
		if ( goods.ID != TRADE_ID_RETURN_CREDIT && goods.ID != TRADE_ID_INSURANCE &&
			 !KOnceIBItemMgr::GetSingleten().IsOkOnceItemParam( 
											goods.Genera,
											goods.Detail,
											goods.Particular,
											goods.Level,
											pc2sReq->Price ) )
		{
			ErrCodeToClient(nPlayerIdx, enIBShopErr_CannotFindGoods);
			return;
		}
	}
	else
	{
		// IB商店物品
		// 从商店中读取单价计算价格
		IBGoods* ibgoods = NULL;
		ibgoods = GetGoodsById(pc2sReq->Goods, pc2sReq->ShopIdx, pc2sReq->ShelfIdx);
		if (!ibgoods)
		{
			ErrCodeToClient(nPlayerIdx, enIBShopErr_CannotFindGoods);
			return;
		}
	
		if ( GetMoneyType(pc2sReq->ShopIdx ) == jinshanbi )
		{
			pc2sReq->ReqNum = 1;
		}

		KInventory* room = Player[nPlayerIdx].GetItemList().GetRoom(room_equipment);
		if ( room )
		{
			int emptyBagSize = room->getFreeSpaceCount();
			if (emptyBagSize <= 0 )
			{
				ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughSpace);
				return;
			}
			
			if(pc2sReq->ReqNum > emptyBagSize )
			{
				ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughSpace);
				return;
			}
		}
		else
		{
			ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughSpace);
			return;
		}

		cost = (int)(ibgoods->Info.Price * ibgoods->Info.Discount / 100) * pc2sReq->ReqNum;
		if (cost <= 0)
		{			
			ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughMoney);
			return;
		}

		if (pc2sReq->useTicket)
		{
			// 代金券购买流程
			int ticketReq = (ibgoods->Info.Price*ibgoods->Info.Discount*pc2sReq->ReqNum) / (100 *  m_CreditToTicketRate);

			int ticketReqPlus = ((ibgoods->Info.Price*ibgoods->Info.Discount*pc2sReq->ReqNum) /100) %  m_CreditToTicketRate;
			
			if ( ticketReqPlus > 0 )
			{
				ticketReq ++;
			}
	
			if (ticketReq <= 0)
			{
				ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughMoney);
				return;
			}

			PresentTicketBuy(nPlayerIdx, goods, pc2sReq->ReqNum, ticketReq);
			return;
		}

		// 判断商品是否是IB物品
		// 商店版本检查，如果不对立即刷新商店
		int ret = CanBuy(nPlayerIdx, pc2sReq);
		if ( ret != enIBShopErr_None )
		{
			ErrCodeToClient(nPlayerIdx, ret);
			return;
		}
	}
	// 购买安全检查 -----> end

	// 正常购买、还款流程	
	IBBuy_Param ibbuy_param;
	memset( &ibbuy_param, 0, sizeof(ibbuy_param) );
	ibbuy_param.eMoneyType		= GetMoneyType(pc2sReq->ShopIdx );
	ibbuy_param.nPrice			= cost;
	ibbuy_param.eIBItemType		= (enumIBItemType)tmpItem.GetIBItemType();
	ibbuy_param.nItemGenre		= tmpItem.GetGenre();
	ibbuy_param.nItemDetail		= tmpItem.GetDetailType();
	ibbuy_param.nItemParticular	= tmpItem.GetParticular();
	ibbuy_param.nItemCount		= pc2sReq->ReqNum;
	ibbuy_param.dwOverdueTime	= tmpItem.GetIBAvailabilityTime();
	ibbuy_param.bOnceItem		= pc2sReq->bOnecItem;

	KPlayer& player = Player[nPlayerIdx];
	int nBuyRet = player.BuyIBItem( ibbuy_param );		

	if (nBuyRet == not_allow_oper)
	{
		ErrCodeToClient(nPlayerIdx,enIBShopErr_OnLoading);
		return;
	}//endif

	if ( nBuyRet == not_enough_money )
	{
		if (pc2sReq->Goods.ID == TRADE_ID_RETURN_CREDIT)
		{
			ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughJinShanBiReturnFailed);
			return;
		}

		if (pc2sReq->Goods.ID == TRADE_ID_INSURANCE)
		{
			ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughMoney);
			return;
		}

		switch(pc2sReq->ShopIdx)
		{
		case enIB_SHOP:
			ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughJinShanBi);
			break;
		case enCREDIT_SHOP:
			ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughCreditPoint);
			break;
		case enPRESENT_SHOP:
			ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughPoint);
			break;
		default:
			ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughMoney);
			break;
		}
	}
}

void IBCenter_S::BuyGoodsReq(int nPlayerIdx, BYTE *pData, int nSize)
{
	DWORD curtime = UNIX_TMIE_STAMP;
	if ( curtime - m_preBuyTime[nPlayerIdx] <= 1)
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_RequestTooFast);
		return;
	}
	else
		m_preBuyTime[nPlayerIdx] = curtime;
	
	if (Player[nPlayerIdx].checkStoreBox())
	{
		return;
	}//endif

	C2S_BUYGOODS *pc2sReq      = (C2S_BUYGOODS*)pData;

	if ( GetMoneyType(pc2sReq->ShopIdx ) == money_type_count )
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_CannotFindGoods);
		return;
	}//endif
	
	// 购买数量检查 -----> begin
	if (pc2sReq->ReqNum == 0 || pc2sReq->ReqNum > MAX_BUY_COUNT )
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_ErrReqBuyNum);
		return;
	}//endif

	bool          bIsJinshanBi = GetMoneyType(pc2sReq->ShopIdx ) == jinshanbi;

	if (bIsJinshanBi) // Especially for JinShanbi
	{
		int nRealBuyNum = pc2sReq->ReqNum;

		if (!pc2sReq->bOnecItem)
		{
			//Precheck bag
			KInventory* room = Player[nPlayerIdx].GetItemList().GetRoom(room_equipment);
			if ( room )
			{
				int emptyBagSize = room->getFreeSpaceCount();
				
				if(nRealBuyNum > emptyBagSize )
				{
					ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughSpace);
					return;
				}//endif
			}//endif
			else
			{
				ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughSpace);
				return;
			}//end else
			
		}//endif
		
		pc2sReq->ReqNum = 1;

		for ( int nBuyLoop = 0; nBuyLoop < nRealBuyNum; nBuyLoop ++ )
		{
			BuyGoodsReqCore(nPlayerIdx,pData,nSize);
		}//end for nBuyLoop

	}//endif
	else
	{
		//Normal Buy
		BuyGoodsReqCore(nPlayerIdx,pData,nSize);
	}//end else

}

/*bool IBCenter_S::CheckOperationTime(int nPlayerIdx, enumGlobalVariable nOper, DWORD& preOperTime)
{
	static ConfigManager &cfg = ConfigManager::Singleton();
	DWORD dwTimeInterval = cfg.GetGlobalVariable(nOper);
	DWORD dwCurTime = UNIX_TMIE_STAMP;
	
	if(dwCurTime - preOperTime < dwTimeInterval)
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_RequestTooFast);
		return false;
	}
	else
		preOperTime = dwCurTime;
	
	return true;
}//*/


void IBCenter_S::LoadIBShopFromDBReq(int nShopIdx)
{
	if (!IBShopUtil::IsShopIndexValid(nShopIdx))
		return;
	
	_IBShopHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	m_ShopVersion[nShopIdx] = INVALID_SHOP_VERSION;
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IBShop;
	DBHeader.nShopIdx = nShopIdx;
	DBHeader.nIBShopID = ibshop_data_shelf;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_LOADSHELF );
	pParam->Push(nShopIdx);
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void IBCenter_S::LoadIBShopFromDBRet(int nDbOpeRst, int nShopIdx, IProcRet* pRet)
{
	if(!nDbOpeRst)
		return;
	
	int row = pRet->GetRowCount();
	int col = pRet->GetColCount();

	if (col < SHELF_ATTR_NUM + MAX_PANELCOUNT_PERSHELF || row <= 0)
		return;
	
	m_ShelfCount[nShopIdx] = (BYTE)row;
	for (int nRow = 0; nRow < m_ShelfCount[nShopIdx] && nRow < MAX_SHELF_NUM; nRow++)
	{
		int nCol = 0;
		int nIdx = 0;
		pRet->GetData(nRow, nCol++, nIdx);
		pRet->GetData(nRow, nCol++, m_GoodsShelfs[nShopIdx][nIdx].Shelf.ShelfName, 
			sizeof(m_GoodsShelfs[nShopIdx][nIdx].Shelf.ShelfName) );

		for (int i = 0; i < MAX_PANELCOUNT_PERSHELF; i++)
		{
			int PanelIdx = 0;
			pRet->GetData(nRow, nCol++, PanelIdx);
			m_GoodsShelfs[nShopIdx][nIdx].Shelf.PanelIdxList[i] = (BYTE)PanelIdx;
		}
	}

	//m_preLoadShopTime[nShopIdx] = 0;
	m_ShopVersion[nShopIdx] = UNIX_TMIE_STAMP;
	UpdateNotifyClient(ibshop_update_shelf, nShopIdx, 0);

	for (int i = 0; i < m_ShelfCount[nShopIdx] && i < MAX_SHELF_NUM; i++)
	{
		LoadIBShopItemFromDBReq(nShopIdx, i);
	}
}

void IBCenter_S::LoadPanelFromDBReq()
{
	_IBShopHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	m_PanelVersion = INVALID_PANEL_VERSION;
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IBShop;
	DBHeader.nIBShopID = ibshop_data_panel;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_LOADPANEL );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void IBCenter_S::LoadPanelFromDBRet(int nDbOpeRst,  IProcRet* pRet)
{
	if(!nDbOpeRst)
		return;

	int row = pRet->GetRowCount();
	int col = pRet->GetColCount();

	if (col < PANEL_ATTR_NUM || row <= 0)
		return;

	m_PanelCount = (BYTE)row;
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

		m_Panels[nRow].PanelID		= (BYTE)PanelID;
		m_Panels[nRow].PanelLeft	= (short)PanelLeft;
		m_Panels[nRow].PanelTop		= (short)PanelTop;
		m_Panels[nRow].PanelWidth	= (WORD)PanelWidth;
		m_Panels[nRow].PanelHeight	= (WORD)PanelHeight;
		m_Panels[nRow].PanelMarge	= (WORD)PanelMarge;
		m_Panels[nRow].CellWidth	= (WORD)CellWidth;
		m_Panels[nRow].CellHeight	= (WORD)CellHeight;
		m_Panels[nRow].TitleWidth	= (WORD)TitleWidth;
		m_Panels[nRow].TitleHeight	= (WORD)TitleHeight;
	}
		
	//m_preLoadPanelTime = 0;
	m_PanelVersion = UNIX_TMIE_STAMP;
	UpdateNotifyClient(ibshop_update_panel, 0, 0);
}

void IBCenter_S::LoadContentStyleFromDBReq()
{
	_IBShopHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	m_ContentStyleVersion = INVALID_STYLE_VERSION;
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IBShop;
	DBHeader.nIBShopID = ibshop_data_contentstyle;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_LOADCONTENTSTYLE );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void IBCenter_S::LoadContentStyleFromDBRet(int nDbOpeRst, IProcRet* pRet)
{
	if(!nDbOpeRst)
		return;

	int row = pRet->GetRowCount();
	int col = pRet->GetColCount();
	m_ContentStyleCount = (BYTE)row;

	if (col < CONTENTSTYLE_ATTR_NUM || row <= 0)
		return;

	for (int nRow = 0; nRow < m_ContentStyleCount && nRow < MAX_CONTENT_STYLE_NUM; nRow++)
	{
		int nCol = 0;
		int StyleID;
		pRet->GetData(nRow, nCol++, StyleID);
		pRet->GetData(nRow, nCol++, m_ContentStyle[nRow].Content, 
			sizeof(m_ContentStyle[nRow].Content));

		m_ContentStyle[nRow].StyleID = (BYTE)StyleID;
	}
	
	//m_preLoadStyleTime = 0;
	m_ContentStyleVersion = UNIX_TMIE_STAMP;
	UpdateNotifyClient(ibshop_update_contentstyle, 0, 0);
}

void IBCenter_S::LoadIBShopItemFromDBReq(int nShopIdx, int nShelfIdx)
{
	if (!IBShopUtil::IsShelfIdxValid(nShelfIdx) || !IBShopUtil::IsShopIndexValid(nShopIdx))
		return;

	_IBShopHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	m_GoodsShelfs[nShopIdx][nShelfIdx].Shelf.ShelfVersion = INVALID_SHELF_VERSION;
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IBShop;
	DBHeader.nShopIdx = nShopIdx;
	DBHeader.nIBShopID = ibshop_data_item;
	DBHeader.nIBShopShelfIdx = nShelfIdx;

	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_LOADITEM );
	pParam->Push(nShopIdx);
	pParam->Push(nShelfIdx);
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void IBCenter_S::LoadIBShopItemFromDBRet(int nDbOpeRst, int nShopIdx, int nShelfIdx, IProcRet* pRet)
{	
	if(!nDbOpeRst)
		return;

	int row = pRet->GetRowCount();
	int col = pRet->GetColCount();

	if (col < ITEM_ATTR_NUM || row <= 0)
		return;

	ClearGoodsInShelf(nShopIdx, nShelfIdx);
	for (int nRow = 0; nRow < row && nRow < MAX_GOODSCOUNT_PERSHELF; nRow++)
	{
		int nCol = 0;
		IBGoods	goods;
		int nPanelIndex, nIndexInPanel, nStyle;
		pRet->GetData(nRow, nCol++, nPanelIndex);
		pRet->GetData(nRow, nCol++, nIndexInPanel);
		pRet->GetData(nRow, nCol++, nStyle);
		pRet->GetData(nRow, nCol++, goods.Id.ID);
		pRet->GetData(nRow, nCol++, goods.Id.Genera);
		pRet->GetData(nRow, nCol++, goods.Id.Detail);
		pRet->GetData(nRow, nCol++, goods.Id.Particular);
		pRet->GetData(nRow, nCol++, goods.Id.Level);
		pRet->GetData(nRow, nCol++, goods.Info.Price);
		pRet->GetData(nRow, nCol++, goods.Info.Discount);
		pRet->GetData(nRow, nCol++, goods.Info.Status);
		pRet->GetData(nRow, nCol++, goods.Info.Label);

		goods.PanelIndex = nPanelIndex;
		goods.IndexInPanel = nIndexInPanel;
		goods.Style = nStyle;

		IBGoods_ListEntry *pListEntry = IBShopUtil::CreateGoodsListEntry(goods);

		if(NULL != pListEntry)
		{
			IBShopUtil::PushToGoodsListHead(pListEntry, m_GoodsShelfs[nShopIdx][nShelfIdx].pGoodsList);
			m_GoodsShelfs[nShopIdx][nShelfIdx].GoodsCount++;
		}
	}
	
	//m_preLoadShelfTime[nShopIdx][nShelfIdx] = 0;
	m_GoodsShelfs[nShopIdx][nShelfIdx].Shelf.ShelfVersion = UNIX_TMIE_STAMP;
	UpdateNotifyClient(ibshop_update_item, nShopIdx, nShelfIdx);
}

void IBCenter_S::ClearGoodsInShelf(int nShopIdx, int nShelfIdx)
{
	if (IBShopUtil::IsShopIndexValid(nShopIdx) && IBShopUtil::IsShelfIdxValid(nShelfIdx))
	{
		m_GoodsShelfs[nShopIdx][nShelfIdx].GoodsCount = 0;
		m_GoodsShelfs[nShopIdx][nShelfIdx].Shelf.ShelfVersion = INVALID_SHELF_VERSION;
		IBShopUtil::ClearGoodsList(m_GoodsShelfs[nShopIdx][nShelfIdx].pGoodsList);
	}
}

void IBCenter_S::ClearAllGoodsInfo(int nShopIdx)
{
	m_ShopVersion[nShopIdx] = INVALID_SHOP_VERSION;
	for(int nShelfIdx = 0; nShelfIdx < MAX_SHELF_NUM; ++nShelfIdx)
	{
		ClearGoodsInShelf(nShopIdx, nShelfIdx);
	}
}

void IBCenter_S::ClearAllShop()
{
	for (int nShopIdx = 0; nShopIdx < enSHOP_NUM; ++nShopIdx)
	{
		ClearAllGoodsInfo(nShopIdx);
	}
}

int	IBCenter_S::GetGoodsInShelf(int nShopIdx, int nShelfIdx, void *pOutBuf, int &nGoodsCount)
{
	if(NULL == pOutBuf)
	{
		nGoodsCount = 0;
		return enIBShopErr_Unknown;
	}

	if(m_GoodsShelfs[nShopIdx][nShelfIdx].GoodsCount > MAX_GOODSCOUNT_PERSHELF)
	{
		nGoodsCount = 0;
		return enIBShopErr_GoodsCountTooMany;
	}
	
	IBGoods	*pGoods = (IBGoods*)pOutBuf;
	IBGoods_ListEntry	*pEntry = m_GoodsShelfs[nShopIdx][nShelfIdx].pGoodsList;

	if (!pEntry)
	{
		nGoodsCount = 0;
		return enIBShopErr_CannotFindGoods;
	}

	while(pEntry)
	{
		*pGoods = pEntry->goods;
		++pGoods;
		pEntry = pEntry->pNextEntry;
	}

	nGoodsCount = m_GoodsShelfs[nShopIdx][nShelfIdx].GoodsCount;
	return enIBShopErr_None;
}

IBGoods* IBCenter_S::GetGoodsById(IBGoods_Id GoodsId, int nShopIdx, int nShelfIdx)
{
	if ( nShopIdx < 0 || nShopIdx >= enSHOP_NUM )
	{
		return NULL;
	}

	if ( nShelfIdx < 0 || nShelfIdx >= MAX_SHELF_NUM )
	{
		return NULL;
	}

	IBGoods_ListEntry	*pEntry = m_GoodsShelfs[nShopIdx][nShelfIdx].pGoodsList;

	while(NULL != pEntry)
	{
		if (pEntry->goods.Id.ID == GoodsId.ID)
		{
			return &pEntry->goods;
		}
		pEntry = pEntry->pNextEntry;
	}
	
	return NULL;
}

void IBCenter_S::PresentTicketBuy(int nPlayerIdx, IBGoods_Id goods, int num, int oneNeedTicket)
{
	KPlayer& player = Player[nPlayerIdx];
	int hashid = GenerateItemHashId(goods.Genera, goods.Detail, goods.Particular);

	//int nCount = player.m_ItemList.m_Room[room_equipment].HaveNormalItem(
	//	m_PTicket.Genera, m_PTicket.Detail, m_PTicket.Particular, m_PTicket.Level, FALSE);
	
	// 得到未过期的代金券数量
	int nCount = player.m_ItemList.m_Room[room_equipment].HaveValidIBItemNum(
		m_PTicket.Genera, m_PTicket.Detail, m_PTicket.Particular, m_PTicket.Level, FALSE);

	if (nCount < oneNeedTicket)
	{
		ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughTicket);
		return;
	}

	int needTicket = oneNeedTicket;
	while ( needTicket > 0 )
	{
		if (Player[nPlayerIdx].m_ItemList.DelValidIBItem(
			m_PTicket.Genera, m_PTicket.Detail, m_PTicket.Particular, m_PTicket.Level))
		{
			needTicket--;
		}
		else
		{
			ErrCodeToClient(nPlayerIdx, enIBShopErr_NoEnoughTicket);
			return;
		}
	}

	int nItemCount = num;
	while (num > 0)
	{
		int nItemIdx = ItemSet.Add(hashid, goods.Level, 1);
		if ( nItemIdx > 0 && nItemIdx < MAX_ITEM )
		{
			if (player.m_ItemList.Add(nItemIdx) == 0)
			{
				player.m_ItemList.Remove(nItemIdx);
				ItemSet.Remove( nItemIdx );
				ErrCodeToClient(nPlayerIdx, enIBShopErr_OpeBuyFailed);
			}
			else
			{
				Item[nItemIdx].SetIBBuyDate(UNIX_TMIE_STAMP);
				if ( !Item[nItemIdx].IsBind() )
				{
					Item[nItemIdx].SetBind(true);
					Item[nItemIdx].SyncAttribute(item_attr_isbind, player.GetNetConnectIdx());
				}
				Item[nItemIdx].SetIBGuid( 0 );
				Item[nItemIdx].SyncAttribute( item_attr_buytime, player.GetNetConnectIdx() );
				KIBLog::getSingleton().AddIBItem( 
										card_buy, 
										Item[nItemIdx].GetGUID(),
										player.GetPlayerIndex(), 
										Item[nItemIdx].GetGenerateItemHashId(),
										Item[nItemIdx].GetLevel(),
										(oneNeedTicket / nItemCount) <= 0 ? 1 : (oneNeedTicket / nItemCount) );	
  
				int nHashId = GenerateItemHashId(Item[nItemIdx].GetGenre(), Item[nItemIdx].GetDetailType(), Item[nItemIdx].GetParticular());
				ErrCodeToClient(player.GetPlayerIndex(), enIBShopErr_OnceItemUseOk, nHashId );
			}
		}
		num--;
	}

}

MoneyType	IBCenter_S::GetMoneyType( int ShopType )
{
	MoneyType type = money_type_count;
	switch( ShopType )
	{
	case enIB_SHOP:
		type = jinshanbi;
		break;
	case enCREDIT_SHOP:
		type = creditpoint;
		break;
	case enPRESENT_SHOP:
		type = point;
		break;
	default:
		break;
	}
	return type;
}

void	UpdateIBShopCallBack(const void* pCallBackParam, unsigned int uPassby)
{
	S2C_UPDATEIBSHOP *pParam = (S2C_UPDATEIBSHOP*)pCallBackParam;

	SendDataToClient(uPassby, pParam, pParam->header.len + PROTOCOL_SIZE);
}

void	IBCenter_S::UpdateNotifyClient(enIBShopUpdateType enType, int nShopIdx, int nShelfIdx)
{
	S2C_UPDATEIBSHOP s2cUpdate;
		
	s2cUpdate.header.protocol = s2c_IB_family;
	s2cUpdate.header.subProtocol = enIB_CSProt_UpdateIBShop;
	s2cUpdate.enType = enType;
	s2cUpdate.nShopIdx = nShopIdx;
	s2cUpdate.nShelfIdx = nShelfIdx;	
	s2cUpdate.header.len = sizeof(s2cUpdate) - PROTOCOL_SIZE;

	g_PlayerInfoToIndex.ForEach(UpdateIBShopCallBack, &s2cUpdate);
	// notify all online players
	//SendDataToClient(nPlayerIdx, &s2cUpdate, s2cUpdate.header.len + PROTOCOL_SIZE);
}

