//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:4:17   17:20
//      File_base        : IBShop
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
#ifndef _IBShop_h
#define _IBShop_h

#include "IBShopComDef.h"
#include "IBShopUtil.h"

class IBShop
{
public:
	static IBShop& Singleton();
	~IBShop();

	void	LoadIBShopFromDBReq();
	void	LoadIBShopFromDBRet(int nDbOpeRst, IProcRet* pRet);
	void	LoadPanelFromDBReq();
	void	LoadPanelFromDBRet(int nDbOpeRst, IProcRet* pRet);
	void	LoadContentStyleFromDBReq();
	void	LoadContentStyleFromDBRet(int nDbOpeRst, IProcRet* pRet);
	void	LoadIBShopItemFromDBReq(int nShelfIdx);
	void	LoadIBShopItemFromDBRet(int nDbOpeRst, int nShelfIdx, IProcRet* pRet);

	int		GetShelfNum() { return m_ShelfCount; }
	int		GetPanelNum() { return m_PanelCount; }
	int		GetContentStyleNum() { return m_ContentStyleCount; }
	int		GetGoodsInShelf(int nShelfIdx, void *pOutBuf, int &nGoodsCount);
	
	int		GetShopVersion();
	void	SetShopVersion(DWORD dwVersion);
	int		GetPanelVersion();
	void	SetPanelVersion(DWORD dwVersion);
	int		GetContentStyleVersion();
	void	SetContentStyleVersion(DWORD dwVersion);
	
	inline IBGoodsShelf GetShelf(int nShelfIdx)
	{
		return m_GoodsShelfs[nShelfIdx];
	}

	inline Panel GetPanel(int nPanelIdx)
	{
		return m_Panels[nPanelIdx];
	}

	inline ContentStyle GetContentStyle(int nStyleIdx)
	{
		return m_ContentStyle[nStyleIdx];
	}

private:
	IBShop();
	// No copy operation
	IBShop(const IBShop &rhs);
	IBShop& operator=(const IBShop &rhs);

	void	ClearGoodsInShelf(int nShelfIdx);
	void	ClearAllGoodsInfo();

private:
	BYTE			m_ShelfCount;
	BYTE			m_PanelCount;
	BYTE			m_ContentStyleCount;

	DWORD			m_ShopVersion;
	DWORD			m_PanelVersion;
	DWORD			m_ContentStyleVersion;

	Panel			m_Panels[MAX_PANEL_NUM];
	ContentStyle	m_ContentStyle[MAX_CONTENT_STYLE_NUM];
	IBGoodsShelf	m_GoodsShelfs[MAX_SHELF_NUM];
	
};

inline IBShop::IBShop()
{
	m_ShelfCount = 0;
	m_ShopVersion = INVALID_SHOP_VERSION;
	m_PanelVersion = INVALID_PANEL_VERSION;
	m_ContentStyleVersion = INVALID_STYLE_VERSION;
	memset(&m_Panels, 0, sizeof(m_Panels));
	memset(&m_ContentStyle, 0, sizeof(m_ContentStyle));
	memset(&m_GoodsShelfs, 0, sizeof(m_GoodsShelfs));
}

inline int IBShop::GetShopVersion()
{
	return m_ShopVersion;
}

inline void IBShop::SetShopVersion(DWORD dwVersion)
{
	m_ShopVersion = dwVersion;
}

inline int IBShop::GetPanelVersion()
{
	return m_PanelVersion;
}

inline void IBShop::SetPanelVersion(DWORD dwVersion)
{
	m_PanelVersion = dwVersion;
}

inline int IBShop::GetContentStyleVersion()
{
	return m_ContentStyleVersion;
}

inline void IBShop::SetContentStyleVersion(DWORD dwVersion)
{
	m_ContentStyleVersion = dwVersion;
}

inline IBShop::~IBShop()
{
	ClearAllGoodsInfo();
}

#endif // #ifndef _IBShop_h