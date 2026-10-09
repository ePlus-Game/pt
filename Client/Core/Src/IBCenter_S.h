//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:4:17   17:04
//      File_base        : IBCenter_S
//      File_ext         : h
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
#ifndef _IBCenter_S_h
#define _IBCenter_S_h

//#include "IBShop.h"
#include "IBShopProtocol.h"
#include "ConfigManager.h"
#include "IMoney.h"

#include <map>

class IBCenter_S
{
public:
	static IBCenter_S& Singleton();
	~IBCenter_S();

	void	Init();
	//void	Active();

	// Request from Client
	void	ProcClientProtocol(int nPlayerIdx, BYTE *pMsg, int nSize);

	// Send request to DB
	void	LoadIBShopFromDBReq(int nShopIdx);
	void	LoadPanelFromDBReq();
	void	LoadContentStyleFromDBReq();
	void	LoadIBShopItemFromDBReq(int nShopIdx, int nShelfIdx);

	// Recieve data from DB
	void	LoadIBShopFromDBRet(int nDbOpeRst, int nShopIdx, IProcRet* pRet);
	void	LoadPanelFromDBRet(int nDbOpeRst, IProcRet* pRet);
	void	LoadContentStyleFromDBRet(int nDbOpeRst, IProcRet* pRet);
	void	LoadIBShopItemFromDBRet(int nDbOpeRst, int nShopIdx, int nShelfIdx, IProcRet* pRet);
	void	ErrCodeToClient(int nPlayerIdx, int nErrCode, int nPlusCode = 0);
	
private:
	IBCenter_S();
	// No copy operation
	IBCenter_S(const IBCenter_S &rhs);
	IBCenter_S& operator=(const IBCenter_S &rhs);

	void		BindProtocolFunc();

	// Request from Client
	void		LoadShelfReq(int nPlayerIdx, BYTE *pData, int nSize);
	void		LoadPanelReq(int nPlayerIdx, BYTE *pData, int nSize);
	void		LoadContentStyleReq(int nPlayerIdx, BYTE *pData, int nSize);
	void		LoadGoodsInShelfReq(int nPlayerIdx, BYTE *pData, int nSize);
	void		BuyGoodsReq(int nPlayerIdx, BYTE *pData, int nSize);
	void        BuyGoodsReqCore(int nPlayerIdx,BYTE * pData,int nSize);
	void		RefreshClientGoodsInShelf(int nPlayerIdx, int nShopIdx, int nShelfIdx);
	void		ChongZhi(int nPlayerIdx, BYTE *pData, int nSize);

	// Update Notify
	void		UpdateNotifyClient(enIBShopUpdateType enType, int nShopIdx, int nShelfIdx);

	// Assist Func
	//bool		CheckOperationTime(int nPlayerIdx, enumGlobalVariable nOper, DWORD& preOperTime);
	//int		CanBuy(int nPlayerIdx, IBGoods_Id Goods, DWORD nShelfVersion, int nShopIdx, int nShelfIdx);
	int			CanBuy(int nPlayerIdx, C2S_BUYGOODS* goods);
	
	int			GetGoodsInShelf(int nShopIdx, int nShelfIdx, void *pOutBuf, int &nGoodsCount);
	IBGoods*	GetGoodsById(IBGoods_Id GoodsId, int nShopIdx, int nShelfIdx);

	void		PresentTicketBuy(int nPlayerIdx, IBGoods_Id goods, int num, int oneNeedTicket);
	
	void		ClearAllShop();
	void		ClearAllGoodsInfo(int nShopIdx);
	void		ClearGoodsInShelf(int nShopIdx, int nShelfIdx);
	
	MoneyType	GetMoneyType( int ShopType );

private:
	typedef void (IBCenter_S::*CLIENTPROTPROC)(int nPlayerIdx, BYTE *pData, int nSize);

private:
	static CLIENTPROTPROC	m_ClientProtProc[enIB_CSProt_End];
	
	// Request time control
	//DWORD			m_preLoadShopTime[enSHOP_NUM];
	//DWORD			m_preLoadShelfTime[enSHOP_NUM][MAX_SHELF_NUM];
	//DWORD			m_preLoadPanelTime;
	//DWORD			m_preLoadStyleTime;
	//DWORD			m_preClearGoodsTime[enSHOP_NUM];

	// Version control
	DWORD			m_ShopVersion[enSHOP_NUM];
	DWORD			m_PanelVersion;
	DWORD			m_ContentStyleVersion;
	
	// Shop data
	BYTE			m_ShelfCount[enSHOP_NUM];
	BYTE			m_PanelCount;
	BYTE			m_ContentStyleCount;
	
	Panel			m_Panels[MAX_PANEL_NUM];
	ContentStyle	m_ContentStyle[MAX_CONTENT_STYLE_NUM];
	IBGoodsShelf	m_GoodsShelfs[enSHOP_NUM][MAX_SHELF_NUM];
	
	IBGoods_Id		m_PTicket;
	int				m_CreditToTicketRate;
	
	std::map<int, DWORD>	m_preBuyTime;
	
};


#endif // #ifndef _IBCenter_S_h