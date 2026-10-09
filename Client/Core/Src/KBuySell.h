#ifndef	KBuySellH
#define	KBuySellH

#include <map>

#define  NO_PLUS_POINT_LIMIT "-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1"
#define  PLUS_POINT_LIMIT_COUNT 20

class KItem;

class KInventory;

class KBuySell
{
private:
	std::map<int,DWORD> m_ItemPlusPrice;
	std::map<int,BOOL> m_ItemBangding;
	std::map<int,std::string> m_ItemPlusPriceLimit;
	std::map<int,int>         m_ItemPlusPointType;

	int**			m_SellItem;
	KItem*			m_Item;
	int				m_Width;
	int				m_Height;
	int				m_MaxItem;


	bool			posCheck(int shopIndex, int itemIndex);
public:
	KBuySell();
	~KBuySell();
	BOOL			Init();
	int				GetWidth() { return m_Width; }
	int				GetHeight() { return m_Height; }
	KItem*			GetItem(int nIndex);//µÈ´ýÉ¾³ý
	KItem*			getItem(int shopIndex, int itemIndex);
	int				GetItemIndex(int nShop, int nIndex);//µÈ´ýÉ¾³ý

public:
	DWORD			GetItemPlusPoint(int shopIndex, int itemIndex);
	int             GetPlusPointType(int shopIndex );
	bool			IsPlusPointOk( int itemListIndex, int nPlayerIdx );
	void			GetItemPlusPointLimit(int shopIndex, int itemIndex, std::string& outString );
	bool			canBuy(int playerIndex, int shopIndex,  int itemIndex);
	bool			canBuyByPlusPoint(int playerIndex, int shopIndex,  int itemIndex);
#ifdef _SERVER
	void			OpenSale(int nPlayerIdx, int nShop, int shopType);
	void			OpenHireShop(int playerIndex);
	bool			buy(int playerIndex, int shopIndex,  int itemIndex);
	bool			buyByPlusPoint(int playerIndex, int shopIndex,  int itemIndex);
	BOOL			Sell(int nPlayerIdx, int nBuy, int nIdx);
#endif
#ifndef _SERVER
	KInventory*		m_pShopRoom;
	void			OpenSale(int nShop, int nCashType);
	void			OpenHireShop();
	void			PaintItem(int nIdx, int nX, int nY);
	void			getPriceLayoutData(ItemPriceLayout* data);
#endif
};

extern KBuySell	BuySell;
#endif
