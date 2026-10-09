#ifndef KInventoryH
#define	KInventoryH

#define KInventoryH_Max_Money 20 * 10000 * 100 * 100
#include "KItem.h"

// Add by Cooler 2004-5-18
// Begin -->
enum enBOXSIZETYPE
{
	enBoxTotalSize = 0,
	enBoxUsedSize,
	enBoxEmptySize,
};
// End <--

// Add by Cooler 2004-8-2
// Begin -->
enum enPLACEITEMRET
{
	enPlaceNone = 0, 
	enPlaceOnly, 
	enPlacePile, 
	enPlacePileLeft, 
	enPlaceSwap, 
};
// End <--

// Add by Cooler 2004-8-2
// Begin -->
typedef struct tagEXTRAINFOPLUS
{
	int nItemGenre;
	int nDetailType;
	int nParticularType;
	int nMaxItem;
	int nCurItem;
	KItem * pCampareItem;

	tagEXTRAINFOPLUS():pCampareItem(NULL),nItemGenre(0),nDetailType(0),nParticularType(0),nMaxItem(0),nCurItem(0)
	{/**/}

}EXTRAINFOPLUS, *PEXTRAINFOPLUS;
// End <--

class KInventory
{
private:
	int*	m_pArray;
	int		m_nMoney;
	int		m_nWidth;
	int		m_nHeight;
	int		m_nTotalSize;
	int		m_validSize;
	int		d_initSize;
	int		m_nUsedSize;
	BOOL	m_locked;
	int		m_nWeight;    //该容器内物品的总重量、只在放入物品和拿出物品时会改变

public:
	int		getWeight()		const{	return m_nWeight;	}
	int		getTotalSize()	const{	return m_nTotalSize;}
	int		getValidSize()	const{	return m_validSize; }
	int		getUsedSize()	const{	return m_nUsedSize; }
	int		getWidth()		const{	return m_nWidth;	}
	int		getHeight()		const{	return m_nHeight;	}
	void	AddWeight(int nDelta);		// 当装入或取出物品时改变储物箱容物总重
	int		GetMoney()		const{	return m_nMoney;	}
	BOOL	AddMoney(int nMoney);
	bool	addSize(int add);
	BOOL	SetMoney(int nMoney);
	int		GetItem(int pos);
	bool	itemCanPlace(PEXTRAINFOPLUS pExtraInfo);	//判断某种类型的物品是否可以放入容器中

	KInventory();
    ~KInventory();

	BOOL	Init(int width, int height, int validSpace = -1);
	void	Release();
	void	Clear();
	void    ClearItem();

	BOOL	PlaceItem(int nXpos, int nYpos, int nIdx, int nPlayerIndex);
	BOOL	PickUpItem(int nIdx, int nX, int nY);
	BOOL	HoldItem(int nIdx, int nPlayerIndex);

	int		FindItem(int nX, int nY);
	BOOL	FindRoom(POINT* pPos, PEXTRAINFOPLUS pExtraInfo = NULL);
	POINT	findEmptySpace();

	int		GetNextItem(int nStartIdx, int nXpos, int nYpos, int *pX, int *pY);
	
	int		CalcSameParticularType(int nGenre, int nDetail, int nParticular);
	// 输入物品类型和具体类型和详细类别，察看Inventory里面有没有相同的物品，输出位置和编号
	BOOL	FindSameParticularType(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY);
	BOOL	FindLockedParticularItem(int nGenre, int nDetail, int nParticular, int* pnIdx, int *pnX, int *pnY);
	
	bool	isPosEmpty(int nX, int nY);				//该位置是否为空
	bool	isRangeEmpty(int x, int y, int len);	//该位置以后的位置是否为空
	bool	isTailEmpty(int len);					//最后的len个位置是否为空

	bool	checkPosValid(const int& x, const int& y);	//检查输入的x、y是否有意义
#ifndef _SERVER
	//商店相关(需要整理)
	BOOL	ShopPlaceItem(int nXpos, int nYpos, int nIdx, int nPlayerIndex, 
		enPLACEITEMRET *penRet = NULL, BOOL bFreeItemList = TRUE, BOOL bSyncMode = FALSE);
	BOOL	ShopFindRoom(POINT* pPos, PEXTRAINFOPLUS pExtraInfo = NULL);
#endif


	
	int GetInventorySize(enBOXSIZETYPE enBoxSizeType)
	{
		switch(enBoxSizeType)
		{
		case enBoxTotalSize:
			return m_validSize;
		case enBoxUsedSize:
			return m_nUsedSize;
		case enBoxEmptySize:
			return (m_validSize > m_nUsedSize ? m_validSize - m_nUsedSize : 0);
		default:
			return 0;
		}
	}

	int GetFirstMatchItem(int nItemClass, int nDetailType, int nParticualrType, BOOL bCheckMagic = FALSE) const
	{
		int	nRet = 0;
		if (m_pArray != NULL )
		{
			int	*pArray = m_pArray;
			const unsigned int nSize = m_nWidth * m_nHeight;
			for (unsigned int i = 0; i < nSize; i++, pArray++)
			{
				if (*pArray > 0)
				{
					KItem& aItem = Item[*pArray];
					if (aItem.GetGenre() == nItemClass &&
						aItem.GetDetailType() == nDetailType &&
						aItem.GetParticular() == nParticualrType &&
						aItem.GetLevelupTimes() == 0 )
					{
						return aItem.GetLevel();
					}
				}

			}
		}
		return 0;
	}
	// 返回某种物品的个数
	int HaveNormalItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel, BOOL bCheckMagic = FALSE) const
	{
		int	nRet = 0;
		if (m_pArray != NULL )
		{
			int	*pArray = m_pArray;
			const unsigned int nSize = m_nWidth * m_nHeight;
			for (unsigned int i = 0; i < nSize; i++, pArray++)
			{
				if (*pArray > 0)
				{
					KItem& aItem = Item[*pArray];
					if (aItem.GetGenre() == nItemClass &&
						aItem.GetDetailType() == nDetailType &&
						aItem.GetParticular() == nParticualrType )
					{
						nRet+=aItem.GetItemCount();
					}
				}

			}
		}
		return nRet;
	}

	int HaveNormalItemIB(int nItemClass,int nDetailType,int nParticualrType,int nLevel, BOOL bCheckMagic = FALSE) const
	{
		int	nRet = 0;
		if (m_pArray != NULL )
		{
			int	*pArray = m_pArray;
			const unsigned int nSize = m_nWidth * m_nHeight;
			for (unsigned int i = 0; i < nSize; i++, pArray++)
			{
				if (*pArray > 0)
				{
					KItem& aItem = Item[*pArray];
					if (aItem.GetGenre() == nItemClass &&
						aItem.GetDetailType() == nDetailType &&
						aItem.GetParticular() == nParticualrType &&
						!aItem.IsOverDate() )
					{
						nRet+=aItem.GetItemCount();
					}
				}
				
			}
		}
		return nRet;
	}
	// 返回未过期IB物品的个数
	int HaveValidIBItemNum(int nItemClass,int nDetailType,int nParticualrType,int nLevel, BOOL bCheckMagic = FALSE) const
	{
		if (nItemClass != item_ib)
			return 0;

		int nRet = 0;
		if (m_pArray != NULL )
		{
			int	*pArray = m_pArray;
			const unsigned int nSize = m_nWidth * m_nHeight;
			for (unsigned int i = 0; i < nSize; i++, pArray++)
			{
				if (*pArray > 0)
				{
					KItem& aItem = Item[*pArray];
					if (aItem.GetGenre() == nItemClass &&
						aItem.GetDetailType() == nDetailType &&
						aItem.GetParticular() == nParticualrType )
					{
						if (!aItem.IsOverDate())
							nRet+=aItem.GetItemCount();
					}
				}

			}
		}
		return nRet;
	}	
	// 查找第一次出现的某种物品的Idx 和 位置
	BOOL FindNormalItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel,int *pnIdx, int *pnX, int *pnY, BOOL bCheckMagic = FALSE) const
	{
		if (!m_pArray)
			return FALSE;
		if (!pnIdx || !pnX || !pnY)
			return FALSE;

		int		*pArray = m_pArray;
		int		i, nSize = m_nWidth * m_nHeight;

		for (i = 0; i < nSize; i++, pArray++)
		{
			if (*pArray > 0)
			{
				KItem& aItem = Item[*pArray];
				if (aItem.GetGenre() == nItemClass &&
					aItem.GetDetailType() == nDetailType &&
					aItem.GetParticular() == nParticualrType &&
					( aItem.GetLevel() == nLevel || nLevel == -1 ) &&
					aItem.GetLevelupTimes() == 0 )
				{
					*pnIdx = *pArray;
					*pnX = i % m_nWidth;
					*pnY = i / m_nWidth;
					return TRUE;
				}
			}
		}
		*pnIdx = -1;
		return FALSE;
	}
	// 查找某种未过期IB物品的Idx 和 位置
	BOOL FindValidIBItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel,int *pnIdx, int *pnX, int *pnY, BOOL bCheckMagic = FALSE, bool checkLevelUpTimes = true ) const
	{
		if (!m_pArray)
			return FALSE;
		if (!pnIdx || !pnX || !pnY)
			return FALSE;

		int		*pArray = m_pArray;
		int		i, nSize = m_nWidth * m_nHeight;

		for (i = 0; i < nSize; i++, pArray++)
		{
			if (*pArray > 0)
			{
				KItem& aItem = Item[*pArray];
				bool isLevelupTimesValid = !checkLevelUpTimes || aItem.GetLevelupTimes() == 0;

				if (aItem.GetGenre() == nItemClass &&
					aItem.GetDetailType() == nDetailType &&
					aItem.GetParticular() == nParticualrType &&
					( aItem.GetLevel() == nLevel || nLevel == -1 ) &&
					isLevelupTimesValid &&
					!aItem.IsOverDate() )
				{
					*pnIdx = *pArray;
					*pnX = i % m_nWidth;
					*pnY = i / m_nWidth;
					return TRUE;
				}
			}
		}
		*pnIdx = -1;
		return FALSE;
	}

	BOOL FindNormalItemByID(int nItemID,int *pnIdx, int *pnX, int *pnY, BOOL bCheckMagic = FALSE) const
	{
		if (!m_pArray)
			return FALSE;
		if (!pnIdx || !pnX || !pnY)
			return FALSE;

		int		*pArray = m_pArray;
		int		i, nSize = m_nWidth * m_nHeight;

		for (i = 0; i < nSize; i++, pArray++)
		{
			if (*pArray > 0)
			{
				KItem& aItem = Item[*pArray];
				if (aItem.GetID() == nItemID &&
					!aItem.IsOverDate())
				{
					*pnIdx = *pArray;
					*pnX = i % m_nWidth;
					*pnY = i / m_nWidth;
					return TRUE;
				}
			}
		}
		*pnIdx = -1;
		return FALSE;
	}

	BOOL IsBoxLocked()
	{
#ifdef _SERVER
		return m_locked;
#else
		return FALSE;
#endif
	}

	void LockBox(BOOL bLock = TRUE)
	{
		m_locked = bLock;
	}
	// End <--
	
	int getFreeSpaceCount();
	int getUsedSpaceCount();



//以下等待删除（xiehong）
#ifdef _SERVER
	BOOL	CheckItemValid(int nIndex, int nPlayerIndex, int nX, int nY);
#endif

	
	BOOL IsExistItem(int nItemClass, int nDetailType, 
		int nParticularType, int nItemAttribute, int nLevel, 
		int &nItemID, int &nPosX, int &nPosY);

#ifndef _SERVER
	void	UpdateWeight(int nUpdateVal);
#endif
};

inline int KInventory::GetItem(int pos)
{
	if (pos >= 0 && pos < m_validSize)
	{
		return m_pArray[pos];
	}
	else
	{
		return 0;
	}
}

#endif //KInventoryH
