#include "KCore.h"
#include "KItem.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "CoreRelated.h"
#ifndef WIN32
#define min(x,y) x<y?x:y
#endif

KItemSet	ItemSet;
/*!*****************************************************************************
// Function		: KItemSet::KItemSet
// Purpose		: 
// Return		: 
// Comments		:
// Author		: Spe
*****************************************************************************/
KItemSet::KItemSet()
{
	m_dwIDCreator = 100;
	m_nNumItems = 0;
	ZeroMemory(&m_sRepairParam, sizeof(REPAIR_ITEM_PARAM));
#ifdef _SERVER
	m_psItemInfo = NULL;
	m_psBackItemInfo = NULL;
#endif
}

KItemSet::~KItemSet()
{
#ifdef _SERVER
	if (m_psItemInfo)
		delete [] m_psItemInfo;
	m_psItemInfo = NULL;
	if (m_psBackItemInfo)
		delete [] m_psBackItemInfo;
	m_psBackItemInfo = NULL;
	m_psItemInfo = NULL;

#endif
}

/*!*****************************************************************************
// Function		: KItemSet::Init
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: Spe
*****************************************************************************/
void KItemSet::Init()
{
	m_FreeIdx.Init(MAX_ITEM);
	m_UseIdx.Init(MAX_ITEM);

	for (int i = MAX_ITEM - 1; i > 0; i--)
	{
		m_FreeIdx.Insert(i);
	}
#ifdef _SERVER
	if (m_psItemInfo)
		delete [] m_psItemInfo;
	m_psItemInfo = NULL;
	m_psItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
	memset(this->m_psItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
	if (m_psBackItemInfo)
		delete [] m_psBackItemInfo;
	m_psBackItemInfo = NULL;
	m_psBackItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
	memset(this->m_psBackItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);

#endif
}

/*!*****************************************************************************
// Function		: KItemSet::SearchID
// Purpose		: 
// Return		: int 
// Argumant		: DWORD dwID
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemSet::SearchID(DWORD dwID)
{
#ifdef _SERVER
	INDEXIDMAP::iterator it =  m_ItemUseIndex.find(dwID);
	if ( it != m_ItemUseIndex.end( ) )
		return it->second;
	else
		return 0;
#else
	int nIdx = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;
		if (Item[nIdx].GetID() == dwID)
			break;
	}
	return nIdx;
#endif
}

int KItemSet::Add(KItem* pItem)
{
	if ( pItem == NULL )
	{
		return -1;
	}

	int i = FindFree();

	if (!i)
		return 0;

	//ItemDebugLog Begin........................
#ifdef _SERVER
	if (Item[i].GetBelong() != -1)
	{
		int  nOnline = 0;
		if (IsValidPlayer(Item[i].GetBelong()))
			nOnline  = 1;

		char szDumpInfo[512] = "";
		snprintf(szDumpInfo,sizeof(szDumpInfo),"KItemSet::Add(KItem* pItem):Index %d,Belong:%d,Name:%s,IsOnline:%d\n",i,Item[i].GetBelong(),pItem->GetName(),nOnline);
		szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
		DumpInvalidItemOpeStack(false,szDumpInfo,4);
	}//endif
#endif
//ItemDebugLog End.........................
	
	Item[i] = *pItem;
	Item[i].SetItemIndex(i);
	Item[i].SetBelong(-1);

#ifdef _SERVER
	Item[i].SetOBJBelong( -1 );
	//物品GUID
	if (g_pController != NULL)
	{
		FSGUID guid;
		g_pController->GenGUID(guid.data, g_GuidPadding);
		Item[i].SetGUID(guid);
	}	
	SetID(i);
	m_ItemUseIndex[Item[i].GetID()] = i;
#endif
	m_FreeIdx.Remove(i);
	m_UseIdx.Insert(i);

	m_nNumItems++;
	return i;

}
/*!*****************************************************************************
// Function		: KItemSet::Add
// Purpose		: 
// Return		: int 数组编号
// Argumant		: int 道具类型（装备？药品？矿石？……）
// Argumant		: int 魔法等级（如对应于装备，就是一般装备，蓝色装备，亮金等……）
// Argumant		: int 五行属性
// Argumant		: int 等级
// Argumant		: int 幸运值
// Comments		:
// Author		: Spe
*****************************************************************************/
int	 KItemSet::Add(
	IN int nItemGenre, 
	IN int nDetail,
	IN int nParticular, 
	IN int nLevel, 
	IN int nItemCount )
{
	int nRet = FALSE;

	int i = FindFree();
	
	if (i == 0)
		return 0;

	KItem*	pItem = &Item[i];

	nRet = g_ItemGen.Gen_Item( 
		nItemGenre, 
		nDetail, 
		nParticular, 
		nLevel, 
		nItemCount,
		pItem );
	
	if (!nRet)
		return FALSE;
	
    //ItemDebugLog Begin........................
#ifdef _SERVER
	if (Item[i].GetBelong() != -1)
	{
		int  nOnline = 0;
		if (IsValidPlayer(Item[i].GetBelong()))
			nOnline  = 1;
		
		char szDumpInfo[512] = "";
		snprintf(szDumpInfo,sizeof(szDumpInfo),"KItemSet::Add(	IN int nItemGenre):Index %d,Belong:%d,Name:%s,IsOnline:%d\n",i,Item[i].GetBelong(),Item[i].GetName(),nOnline);
		szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
		DumpInvalidItemOpeStack(false,szDumpInfo,4);
	}//endif
#endif
    //ItemDebugLog End.........................

#ifdef _SERVER
	SetID(i);
	m_ItemUseIndex[Item[i].GetID()] = i;
#endif
	Item[i].SetItemIndex(i);

	m_FreeIdx.Remove(i);
	m_UseIdx.Insert(i);
	m_nNumItems++;
	return i;
}

int	KItemSet::Add(IN int nGenHashID, 
			      IN int nLevel,
				  IN int nItemCount )
{
	int nRet = FALSE;

	int i = FindFree();
	
	if (i == 0)
		return 0;

	int nItemGenre = 0; 
	int nDetail = 0; 
	int nParticular = 0;

	SpliteHashId(nGenHashID, nItemGenre, nDetail, nParticular );
	
	KItem*	pItem = &Item[i];

	nRet = g_ItemGen.Gen_Item( 
		nItemGenre, 
		nDetail, 
		nParticular, 
		nLevel, 
		nItemCount,
		pItem );
	
	if (!nRet)
		return FALSE;
	
	//ItemDebugLog Begin........................
#ifdef _SERVER
	if (Item[i].GetBelong() != -1)
	{
		int  nOnline = 0;
		if (IsValidPlayer(Item[i].GetBelong()))
			nOnline  = 1;
		
		char szDumpInfo[512] = "";
		snprintf(szDumpInfo,sizeof(szDumpInfo),"KItemSet::Add(IN int nGenHashID):Index %d,Belong:%d,Name:%s,IsOnline:%d\n",i,Item[i].GetBelong(),Item[i].GetName(),nOnline);
		szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
		DumpInvalidItemOpeStack(false,szDumpInfo,4);
	}//endif
#endif
    //ItemDebugLog End.........................

#ifdef _SERVER
	SetID(i);
	m_ItemUseIndex[Item[i].GetID()] = i;
#endif

	Item[i].SetItemIndex(i);

	m_FreeIdx.Remove(i);
	m_UseIdx.Insert(i);
	m_nNumItems++;
	return i;
}

/*!*****************************************************************************
// Function		: KItemSet::FindFree
// Purpose		: 
// Return		: int 
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemSet::FindFree()
{
	return m_FreeIdx.GetNext(0);
}

void KItemSet::Remove(IN int nIdx)
{
	if ( nIdx <= 0 || nIdx >= MAX_ITEM )
	{
		return;
	}

//ItemDebugLog Begin........................
#ifdef _SERVER
	if (Item[nIdx].GetBelong() != -1 || Item[nIdx].GetOBJBelong() != -1)
	{
		char szDumpInfo[512] = "";
		snprintf(szDumpInfo,sizeof(szDumpInfo),"Remove From ItemSet:Index %d,Belong:%d,ObjBelong:%d,Name:%s\n",nIdx,Item[nIdx].GetBelong(),Item[nIdx].GetOBJBelong(),Item[nIdx].GetName());
		szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
		DumpInvalidItemOpeStack(true,szDumpInfo,5);
	}//endif
#endif
//ItemDebugLog End.........................

#ifdef _SERVER
	m_ItemUseIndex.erase(Item[nIdx].GetID());
#endif
	Item[nIdx].Remove();
	
	m_UseIdx.Remove(nIdx);
	m_FreeIdx.Insert(nIdx);
	m_nNumItems--;
}

void KItemSet::SetID(IN int nIdx)
{
	Item[nIdx].SetID(m_dwIDCreator);
	m_dwIDCreator++;
}

#ifdef _SERVER
//---------------------------------------------------------------------------
//	功能：copy m_psItemInfo to m_psBackItemInfo
//---------------------------------------------------------------------------
void	KItemSet::BackItemInfo()
{
	_ASSERT(this->m_psItemInfo);
	_ASSERT(this->m_psBackItemInfo);
	if (!m_psItemInfo)
		return;
	if (!m_psBackItemInfo)
		m_psBackItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
	memcpy(m_psBackItemInfo, this->m_psItemInfo, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
}
#endif

int KItemSet::GetItemCount(IN int nItemGenre /* = -1 */)
{
	if (nItemGenre == -1)
		return m_nNumItems;
	else
		return 0;
}
