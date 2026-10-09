//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:4:18   19:37
//      File_base        : IBCenter_C
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
#ifndef _IBCenter_C_h
#define _IBCenter_C_h

#include "IBShopComDef.h"
#include "ConfigManager.h"

class IBCenter_C
{
public:
	static IBCenter_C& Singleton();
	~IBCenter_C();
	
	void	Init();
	void	ProcessServerProtocol(const void *pMsg);

	void	LoadShelfReq(int nShopIdx);
	void	LoadPanelReq();
	void	LoadContentStyleReq();
	void	LoadGoodsInShelfReq(int nShopIdx, int nShelfIdx);
	//void	BuyGoodsReq(IBGoods_Id Goods, int nShopIdx, int nShelf);
	int		BuyGoodsReq(ClientBuyGoods goods);
	void	ChongZhi( void );	
private:
	IBCenter_C();

	// Process protocol functions
	void	OnLoadShelf(const void *pMsg);
	void	OnLoadPanel(const void *pMsg);
	void	OnLoadContentStyle(const void *pMsg);
	void	OnLoadGoodsInShelf(const void *pMsg);
	void	OnErrCode(const void *pMsg);
	void	OnUpdateIBShop(const void *pMsg);

	// Assistant functions
	void	BindProtocolFunc();
	void	ClearAllShop();
	void	ClearAllGoods(int nShopIndex);
	void	ClearGoodsInShelf(int nShopIdx, int nShelfIdx);
	bool	CheckOperationTime(enumGlobalVariable nOper, DWORD& preOperTime);
	int		CheckBuyItem(ClientBuyGoods goods, int& needBagSize, int& lastBuyNum, int& maxItemCount);

private:
	// No copy operation
	IBCenter_C(const IBCenter_C &rhs);
	IBCenter_C& operator=(const IBCenter_C &rhs);

	typedef void (IBCenter_C::*PPROTFUNC)(const void *pMsg);

private:
	BYTE			m_ShelfCount[enSHOP_NUM];
	BYTE			m_PanelCount;
	BYTE			m_ContentStyleCount;

	DWORD			m_preLoadShopTime[enSHOP_NUM];
	DWORD			m_preLoadShelfTime[enSHOP_NUM][MAX_SHELF_NUM];
	DWORD			m_preLoadPanelTime;
	DWORD			m_preLoadStyleTime;

	DWORD			m_preBuyReqTime;

	DWORD			m_IBShopVersion[enSHOP_NUM];
	DWORD			m_PanelVersion;
	DWORD			m_ContentStyleVersion;

	IBGoodsShelf	m_GoodsShelf[enSHOP_NUM][MAX_SHELF_NUM];

	static PPROTFUNC m_ServerProtFunc[enIB_CSProt_End];
};

inline IBCenter_C::IBCenter_C()
: m_PanelCount(0)
, m_ContentStyleCount(0)
, m_preLoadPanelTime(0)
, m_preLoadStyleTime(0)
, m_preBuyReqTime(0)
, m_PanelVersion(INVALID_SHOP_VERSION)
, m_ContentStyleVersion(INVALID_SHOP_VERSION)
{
	for (int nShopIdx = 0; nShopIdx < enSHOP_NUM; ++nShopIdx)
	{
		m_ShelfCount[nShopIdx] = 0;
		m_preLoadShopTime[nShopIdx] = 0;
		m_IBShopVersion[nShopIdx] = INVALID_SHOP_VERSION;
		for (int i = 0; i < MAX_SHELF_NUM; i++)
		{
			m_preLoadShelfTime[nShopIdx][i] = 0;
		}
	}

	memset(&m_GoodsShelf, 0, sizeof(m_GoodsShelf));
	memset(&m_ServerProtFunc, 0, sizeof(m_ServerProtFunc));
	BindProtocolFunc();
}

inline IBCenter_C::~IBCenter_C()
{
	ClearAllShop();
}

#endif