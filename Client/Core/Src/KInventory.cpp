#include "KCore.h"
#include "KItem.h"
#include "MyAssert.H"
#include "KInventory.h"
#include "KPlayer.h"
#include "KItemSet.h"

KInventory::KInventory()
{
	m_pArray = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	// Add by Cooler 2004-5-18
	// Begin -->
	m_nTotalSize = 0;
	m_nUsedSize = 0;
	// End <--
	// Add by Cooler 2004-8-11
	// Begin -->
	m_locked = FALSE;
	// End <--
	// Add by [Adt.X], 2004-8-22
	m_nWeight = 0;
	// End
	m_nMoney = 0;

}

KInventory::~KInventory()
{
    Release();
}

BOOL KInventory::Init(int width, int height, int validSpace)
{
	//声请空间
	if (m_pArray)
		delete [] m_pArray;

	m_pArray = new int[width * height];
	if (!m_pArray)
		return FALSE;

	ZeroMemory(m_pArray, sizeof(int) * width * height);
	
	//设置基本数据信息
	m_nWidth = width;
	m_nHeight = height;
	m_nTotalSize = width * height;
	if(-1 == validSpace)
		m_validSize = width * height;
	else
		m_validSize = validSpace;
	d_initSize = m_validSize;

	m_nUsedSize = 0;
	m_locked = FALSE;
	m_nWeight = 0;
	
	return TRUE;
}

bool KInventory::addSize(int add)
{
	if(m_validSize + add > m_nTotalSize || m_validSize + add < d_initSize)
	{
		_ASSERT(0);
		return false;
	}
	
	m_validSize += add;
	if(m_validSize > m_nTotalSize)
		m_validSize = m_nTotalSize;
	return true;
}

void KInventory::AddWeight(int nDelta) // 当装入或取出物品时改变储物箱容物总重
{
	if (m_nWeight + nDelta < 0) 
	{
		return;
	}
	m_nWeight += nDelta;
}

void KInventory::Release()
{
	if (m_pArray)
	{
		delete []m_pArray;
	}
	m_pArray = NULL;
	this->m_nWidth = 0;
	this->m_nHeight = 0;
	this->m_nMoney = 0;

	m_nTotalSize = 0;
	m_nUsedSize = 0;
	m_locked = FALSE;
	m_nWeight = 0;
}

void KInventory::Clear()
{
	if (m_pArray)
		memset(m_pArray, 0, sizeof(int) * m_nWidth * m_nHeight);
	m_nMoney    = 0;
}

void KInventory::ClearItem()
{
	if (m_pArray)
		memset(m_pArray, 0, sizeof(int) * m_nWidth * m_nHeight);
	
	m_nUsedSize = 0;
}

BOOL KInventory::SetMoney(int nMoney)
{ 	
	if(nMoney > KInventoryH_Max_Money)
	{
		nMoney = KInventoryH_Max_Money;
	}

	if (nMoney < 0) 
		return FALSE; 
	m_nMoney = nMoney; 
	return TRUE; 
}

BOOL KInventory::PlaceItem(int nXpos, int nYpos, int index, int nPlayerIndex)
{
	if(!m_pArray)
		return FALSE;

	//检查x、y索引是否正确
	if(checkPosValid(nXpos, nYpos) == false)
		return FALSE;
	
	//检查物品是否存在
	if(index <= 0 || index > MAX_ITEM || Item[index].GetID() <= 0)
		return FALSE;

// 	//容器锁定时，不允许操作
// 	if(IsBoxLocked())
// 		return FALSE;
	
	int arrayIndex = nYpos * m_nWidth + nXpos;
	int nOldIdx = m_pArray[arrayIndex];
	
	int addWeight = Item[index].GetItemWeight();

	//当指定位置上已经存在物品时，则先看是否能够叠加
	if(nOldIdx > 0)
	{
		//如果不是相同类型物品，则放置物品失败
		if(Item[nOldIdx].IsSameParitcularItem(Item[index]) == false)
		{
			return FALSE;
		}
		//如果是相同物品，但不能叠加，则放置物品失败
		int maxItemCount = Item[nOldIdx].GetMaxItemCount();
		if(maxItemCount < 2)
		{
			return FALSE;
		}
		//可以叠加，但超过了叠加上限，也不能放下
		if(maxItemCount < Item[nOldIdx].GetItemCount() + Item[index].GetItemCount())
		{
			return FALSE;
		}

		//叠加
		Item[nOldIdx].SetItemCount(Item[nOldIdx].GetItemCount() + Item[index].GetItemCount());

		Player[nPlayerIndex].m_ItemList.Remove(index);
		ItemSet.Remove(index);
	}
	else
	{
		//如果该位置上没有物品，则直接把该位置的索引更新即可
		++m_nUsedSize;
		m_pArray[arrayIndex] = index;
	}	
	
	//更新负重和使用大小
	AddWeight(addWeight);

	return TRUE;
}

#ifndef _SERVER
BOOL KInventory::ShopPlaceItem(int nXpos, int nYpos, int nIdx, int nPlayerIndex, 
			enPLACEITEMRET *penRet, BOOL bFreeItemList, BOOL bSyncMode)
{
	
	// Add by Cooler 2004-8-11
	// Begin -->
	if(IsBoxLocked())
	{
		return FALSE;
	}
	// End <--

	//int nEachItemWeight = Item[nIdx].GetItemWeight();

	int nOldIdx = 0;
	enPLACEITEMRET enRet = enPlaceNone;

	if(!m_pArray || nIdx <= 0 ||
		nXpos < 0 || nYpos < 0 || nXpos > m_nWidth || nYpos > m_nHeight)
	{
		enRet = enPlaceNone;
		goto placeexport;
	}

	if(!nOldIdx)
	{
		nOldIdx = m_pArray[nYpos * m_nWidth + nXpos];
	}
	else
	{
		if(nOldIdx != m_pArray[nXpos * m_nWidth + nXpos])
		{
			// Not the same item
			enRet = enPlaceNone;					
		}					
		goto placeexport;
	}

	enRet = enPlaceOnly;
	// Set Item Idx to Inventory
	m_pArray[nXpos * m_nWidth + nXpos] = nIdx;

	// Add by Cooler 2004-5-18
	// Begin -->
	++m_nUsedSize;
	// End <--



placeexport:

	if(penRet != NULL)
	{
		*penRet = enRet;
	}

	if(enRet == enPlaceNone)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
#endif

// End <--

BOOL KInventory::HoldItem(int nIdx, int nPlayerIndex)
{
	int i, j;
	for (i = 0; i < m_nWidth; i++)
	{
		for (j = 0; j < m_nHeight; j++)
		{
			if (PlaceItem(i, j, nIdx, nPlayerIndex))
				return TRUE;
		}
	}
	return FALSE;
}

BOOL	KInventory::PickUpItem(int nIdx, int nX, int nY)
{
// 	if(IsBoxLocked())
// 	{
// 		return FALSE;
// 	}
	if (checkPosValid(nX, nY) == false)
		return FALSE;

	if (m_pArray[nY * m_nWidth + nX] != nIdx)
	{
		_ASSERT(0);
		return FALSE;
	}

	m_pArray[nY * m_nWidth + nX] = 0;

	--m_nUsedSize ;

	AddWeight(-Item[nIdx].GetItemWeight());
	return TRUE;
}


int	KInventory::FindItem(int nX, int nY)
{
	if (!m_pArray)
		return -1;
	if (checkPosValid(nX, nY) == false)
		return -1;

	int	nPos = nY * m_nWidth + nX;
	int	*pArray = &m_pArray[nPos];
	if (*pArray <= 0)
		return 0;
	int	nIdx = *pArray;

	return nIdx;
}

bool KInventory::itemCanPlace(PEXTRAINFOPLUS pExtraInfo)
{
	if(NULL == pExtraInfo)
		return false;
	
	//找到空位置或者可叠加位置都可以
	int limitLen = m_validSize < m_nTotalSize ? m_validSize : m_nTotalSize;
	for(int i = 0; i < limitLen; ++i)
	{
		if(m_pArray[i] <= 0)
			return true;
		
		int itemIndex = m_pArray[i];
		if(Item[itemIndex].GetMaxItemCount() < 1)
			continue;
		
		if(pExtraInfo->pCampareItem && !pExtraInfo->pCampareItem->CanCombine(Item[itemIndex]))
			continue;

		if(	Item[itemIndex].GetGenre()		== pExtraInfo->nItemGenre && 
			Item[itemIndex].GetParticular()	== pExtraInfo->nParticularType && 
			Item[itemIndex].GetDetailType()	== pExtraInfo->nDetailType && 
			Item[itemIndex].GetMaxItemCount() - (Item[itemIndex].GetItemCount() + pExtraInfo->nCurItem) >= 0)
		{
			return true;
		}
	}
	
	return false;
}

BOOL KInventory::FindRoom(POINT* pPos, PEXTRAINFOPLUS pExtraInfo)
{
	if (!pPos)
		return FALSE;

	int limitLen = m_validSize < m_nTotalSize ? m_validSize : m_nTotalSize;

	//当指定了物品类型且该物品可以叠加时，尽量找到可以叠加的位置
	if(NULL != pExtraInfo && pExtraInfo->nMaxItem > 1 && pExtraInfo->nCurItem > 0)
	{
		for(int i = 0; i < limitLen; ++i)
		{
			if(m_pArray[i] <= 0)
				continue;
			
			int itemIndex = m_pArray[i];
			KItem* pDstItem = &Item[itemIndex];
			if(pDstItem->GetMaxItemCount() < 1 || pDstItem->GetIBBuyData() > 0 )
				continue;

			if (pExtraInfo->pCampareItem && !pExtraInfo ->pCampareItem->CanCombine( Item[itemIndex] ))
				continue;
			
			if(	pDstItem->GetGenre()		== pExtraInfo->nItemGenre && 
				pDstItem->GetParticular()	== pExtraInfo->nParticularType && 
				pDstItem->GetDetailType()	== pExtraInfo->nDetailType && 
				pDstItem->GetMaxItemCount()	== pExtraInfo->nMaxItem &&
				pDstItem->GetItemCount() > 0 &&
				pDstItem->GetMaxItemCount() - (pDstItem->GetItemCount() + pExtraInfo->nCurItem) >= 0)
			{
				pPos->x = i % m_nWidth;
				pPos->y = i / m_nWidth;
				return TRUE;
			}
		}
	}

	int reqSpaceCount = 1;
	if( pExtraInfo && pExtraInfo->nMaxItem > 0)
	{
		reqSpaceCount = (pExtraInfo->nCurItem - 1) / pExtraInfo->nMaxItem + 1;
	}
	
	//未指定物品或者该物品不能叠加，或者寻找叠加物品失败时，查找一个空位置
	for(int i = 0; i < limitLen; ++i)
	{
		if (m_pArray[i] > 0)
			continue;
		pPos->x = i % m_nWidth;
		pPos->y = i / m_nWidth;
		if(reqSpaceCount > 1)
			--reqSpaceCount;
		else
			return TRUE;
	}
	
	//没有任何合适的位置
	pPos->x = -1;
	pPos->y = -1;
	return FALSE;
}

#ifndef _SERVER
BOOL KInventory::ShopFindRoom(POINT* pPos, PEXTRAINFOPLUS pExtraInfo)
{
	if (!pPos)
		return FALSE;

	int m, nSize = m_nWidth * m_nHeight;
	for(m = 0; m < nSize; m ++)
	{
		if (m_pArray[m] == 0)
		{
			pPos->x = m % m_nWidth;
			pPos->y = m / m_nWidth;
			return TRUE;
		}
	}
	pPos->x = 0;
	pPos->y = 0;
	return FALSE;
}
#endif



BOOL KInventory::AddMoney(int nMoney)
{
	if (m_nMoney + nMoney < 0)
		return FALSE;

	//xiehong 2008-3-28 金钱设置上限
	if( m_nMoney + nMoney > KInventoryH_Max_Money)
	{
		return FALSE;
	}

	m_nMoney += nMoney;
	return TRUE;
}

int	KInventory::GetNextItem(int nStartIdx, int startX, int startY, int* findX, int* findY)
{
	if (!m_pArray)
		return 0;

	//检查开始索引是否越界，以及传入参数是否有效
	int arrayIndex = startY * m_nHeight + startX;
	int endIndex = m_validSize < m_nTotalSize ? m_validSize : m_nTotalSize;
	if (arrayIndex < 0 || arrayIndex >= endIndex || !findX || !findY)
		return 0;

	for(int i = arrayIndex; i < endIndex; ++i)
	{
		//当没有物品或者为指定物品时查找下一个
		if(m_pArray[i] <= 0 || m_pArray[i] == nStartIdx)
			continue;

		*findX = i % m_nWidth;
		*findY = i / m_nWidth;
		return m_pArray[i];
	}

	//没找到
	return 0;
}

POINT KInventory::findEmptySpace()
{
	POINT pos;
	pos.x = -1;
	pos.y = -1;
	
	if (!m_pArray)
		return pos;
	
	for(int y = 0 ; y < m_nHeight; y++)
	{
		for(int x = 0; x < m_nWidth; x++)
		{
			int arrayIndex = y * m_nWidth + x;
			
			if(arrayIndex >= m_validSize)
			{
				return pos;
			}
			if(0 == m_pArray[arrayIndex])
			{
				pos.x = x;
				pos.y = y;
				return pos;
			}
		}
	}
	return pos;
}

int	KInventory::CalcSameParticularType(int nGenre, int nDetail, int nParticular)
{
	if (!m_pArray)
		return 0;
	
	int endIndex = m_validSize < m_nTotalSize ? m_validSize : m_nTotalSize;
	int totalCount = 0;
	for (int i = 0; i < endIndex; i++)
	{
		int itemIndex = m_pArray[i];

		if(itemIndex <= 0)
			continue;
		
		if (Item[itemIndex].GetGenre() == nGenre 
			&& Item[itemIndex].GetDetailType() == nDetail 
			&& Item[itemIndex].GetParticular() == nParticular)
			totalCount += Item[itemIndex].GetItemCount();
	}

	return totalCount;
	
// 	int		nNum = 0;
// 	int		nCurIdx = 0;
// 	int		nSize = m_nWidth * m_nHeight;
// 	int		*pArray = m_pArray;
// 	for (int i = 0; i < nSize; i++)
// 	{
// 		if (*pArray <= 0)
// 		{
// 			pArray++;
// 			continue;
// 		}
// 		if (nCurIdx == *pArray)
// 		{
// 			pArray++;
// 			continue;
// 		}
// 
// 		nCurIdx = *pArray;
// 		if (Item[nCurIdx].GetGenre() == nGenre && Item[nCurIdx].GetDetailType() == nDetail && Item[nCurIdx].GetParticular() == nParticular)
// 			nNum++;
// 		
// 		pArray++;
// 	}
// 
// 	return nNum;
}
//---------------------------------------------------------------------------------
// 功能：输入物品类型和具体类型，察看Inventory里面有没有相同的物品，输出位置和编号
//---------------------------------------------------------------------------------
BOOL	KInventory::FindSameParticularType(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY)
{
	if (!m_pArray)
		return FALSE;

	if (!pnIdx || !pnX || !pnY)
		return FALSE;
	
	_ASSERT(m_validSize <= m_nTotalSize);
	int endIndex = m_validSize < m_nTotalSize ? m_validSize : m_nTotalSize;
	
	int totalCount = 0;
	for (int i = 0; i < endIndex; i++)
	{
		int itemIndex = m_pArray[i];

		if(itemIndex <= 0)
			continue;
		
		if (Item[itemIndex].GetGenre() == nGenre 
			&& Item[itemIndex].GetDetailType() == nDetail 
			&& Item[itemIndex].GetParticular() == nParticular)
		{	
			*pnIdx = itemIndex;
			*pnX = i % m_nWidth;
			*pnY = i / m_nWidth;
			return TRUE;
		}
	}

	//没找到
	*pnIdx = -1;
	*pnX = -1;
	*pnY = -1;
	return FALSE;

// 	if (!m_pArray)
// 		return FALSE;
// 	if (!pnIdx || !pnX || !pnY)
// 		return FALSE;
// 
// 	int		*pArray = m_pArray;
// 	int		i, nSize = m_nWidth * m_nHeight;
// 
// 	for (i = 0; i < nSize; i++, pArray++)
// 	{
// 		if (*pArray <= 0)
// 			continue;
// 		if (Item[*pArray].GetGenre() == nGenre && Item[*pArray].GetDetailType() == nDetail && Item[*pArray].GetParticular() == nParticular)
// 		{
// 			*pnIdx = *pArray;
// 			*pnX = i % m_nWidth;
// 			*pnY = i / m_nWidth;
// 			return TRUE;
// 		}
// 	}
// 	*pnIdx = -1;
// 	return FALSE;
}

BOOL KInventory::FindLockedParticularItem(int nGenre, int nDetail, int nParticular, int* pnIdx, int *pnX, int *pnY)
{
	if (!m_pArray)
		return FALSE;

	if (!pnIdx || !pnX || !pnY)
		return FALSE;
	
	_ASSERT(m_validSize <= m_nTotalSize);
	int endIndex = m_validSize < m_nTotalSize ? m_validSize : m_nTotalSize;
	
	int totalCount = 0;
	for (int i = 0; i < endIndex; i++)
	{
		int itemIndex = m_pArray[i];

		if(itemIndex <= 0)
			continue;
		
		if (Item[itemIndex].GetGenre() == nGenre 
			&& Item[itemIndex].GetDetailType() == nDetail 
			&& Item[itemIndex].GetParticular() == nParticular
			&& Item[itemIndex].IsLocked(-1, false))
		{	
			*pnIdx = itemIndex;
			*pnX = i % m_nWidth;
			*pnY = i / m_nWidth;
			return TRUE;
		}
	}

	//没找到
	*pnIdx = -1;
	*pnX = -1;
	*pnY = -1;
	return FALSE;
}

//************************************
// Method:    getEmptySpace
// FullName:  KInventory::getEmptySpace
// Access:    public 
// Returns:   int
// Qualifier: 得到包裹剩下的空格子数
//************************************
int KInventory::getFreeSpaceCount()
{
	if (!m_pArray)
		return 0;
	
	int freeSpace = 0;
	for (int y = 0; y < m_nHeight; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			int arrayIndex = y * m_nWidth + x;
			
			if(arrayIndex >= m_validSize)
				return freeSpace;

			if(0 == m_pArray[arrayIndex])
				freeSpace++;
		}
	}
	return freeSpace;
}


//************************************
// Method:    getUsedSpaceCount
// FullName:  KInventory::getUsedSpaceCount
// Access:    public 
// Returns:   int
// Qualifier: 得到已经使用的格子个数
//************************************
int KInventory::getUsedSpaceCount()
{
	return m_nHeight * m_nWidth - getFreeSpaceCount();
}


//************************************
// Method:    IsPosEmpty
// FullName:  KInventory::IsPosEmpty
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: int nX
// Parameter: int nY
//************************************
bool KInventory::isPosEmpty(int x, int y)
{
	if(!m_pArray)
		return false;

	if(checkPosValid(x, y) == false)
		return false;

	if(0 == m_pArray[x * m_nWidth + y])
		return true;
	
	return false;
}

bool KInventory::isRangeEmpty(int x, int y, int len)
{
	if(checkPosValid(x, y) == false)
		return false;


	for(int i = 0; i < len; ++i)
	{
		if(isPosEmpty(x, y) == false)
			return false;
		++x;
		if(x >= m_nWidth)
		{
			x = 0;
			++y;
		}
	}
	return true;
}

bool KInventory::isTailEmpty(int len)
{
	if(len < 1)
		return false;
	
	for(int i = m_validSize - len; i < m_validSize; ++i)
	{
		if(m_pArray[i] > 0)
			return false;
	}
	return true;
}

bool KInventory::checkPosValid(const int& x, const int& y)
{
	//检查横坐标
	if(x >= m_nWidth || x < 0)
		return false;

	int endIndex = m_validSize < m_nTotalSize ? m_validSize : m_nTotalSize;
	int arrayIndex = y * m_nWidth + x;

	//检查索引（相当于检查了纵坐标）
	if(arrayIndex >= endIndex || arrayIndex < 0)
		return false;
	return true;
}

//以下等待删除（xiehong）

#ifndef _SERVER
void KInventory::UpdateWeight(int nUpdateVal)
{
	m_nWeight = nUpdateVal;
}
#endif


#ifdef _SERVER
BOOL KInventory::CheckItemValid(int nIndex, int nPlayerIndex, int nX, int nY)
{
	return TRUE;
}
#endif

BOOL KInventory::IsExistItem(int nItemClass, int nDetailType, 
			int nParticularType, int nItemAttribute, int nLevel, 
			int &nItemID, int &nPosX, int &nPosY)
{
	int i = 0, nSize = m_nWidth * m_nHeight;

	int *pArray = m_pArray;
	for(i = 0; i < nSize; i++, pArray++)
	{
		if(*pArray > 0)
		{
			KItem& aItem = Item[*pArray];

			if (aItem.GetGenre() == nItemClass && 
				aItem.GetDetailType() == nDetailType && 
				aItem.GetParticular() == nParticularType && 
				aItem.GetLevel() == nLevel )
			{
				nItemID = pArray[0];
				nPosX = i % m_nWidth;
				nPosY = i / m_nWidth;
				
				return TRUE;
			}
		}
	}

	nItemID = -1;
	nPosX = -1;
	nPosY = -1;

	return FALSE;
}
