#include "KCore.h"
#include "MyAssert.H"
#include "KItem.h"
#include "KItemSet.h"
#include "KNpc.h"
#include "KMath.h"
#include "KPlayer.h"
#include "KItemList.h"
#include "KItemChangeRes.h"
#include "KCompoundRule.h"
#ifdef WIN32
#include "KItemEnchaser.h"
#else
#include "KItemEnchaser.cpp"
#endif
#include "KItemGenerator.h"

#ifdef _SERVER
#include "IBLog.h"
#include "KObjSet.h"
#include "time.h"
#include "KPlayerSet.h"
#include "buff_man.h"
#include "Abrade_Table.h"
#include "player_monitor.h"
#include "CoreRelated.h"
#else

#include "CoreShell.h"
#include "networkinterface.h"

#endif

#include "KSubWorldSet.h"
#define	TRADE_DEBUG
#include "KNpcTemplate.h"
#include "KSubWorld.h"
#include "buff_tab.h"
#include "ArmorSet_Table.h"
#include "Yao_Table.h"
#include "talisman_manager.h"

int		g_nTradeColorCount;
const	char * g_szColorTbl[] = 
{
	"<color=blue>",
	"<color=yellow>",
	"<color=green>",
};

KBesetMap sBesetMap[UITP_NUM] =
{
	{0,0},		//UITP_ITEM
	{0,4},		//UITP_GOLD
	{1,4},		//UITP_WOOD
	{0,5},		//UITP_WATER
	{1,5},		//UITP_FIRE
	{0,6},		//UITP_EARTH
	{1,6},		//UITP_SPIRIT
	{0,7},		//UITP_LEVEL
	//--> Rocker 2004/08/14
	{1,7},		//附加的孔
	//<-- End
};

#ifdef _SERVER
#define TALISMAN_COOLDOWN_GROUP 10000//法宝CD组
#endif

KItemList::KItemList()
{
	m_PlayerIdx = 0;
	m_nListCurIdx = 0;
	//	m_bFirstCalWeight = true;
	m_nWeightTaken = 0;
	
	//--> Rocker 2004/08/03
	for (int i=0; i<itempart_num; i++)
	{
		mEquipInsurance[i].mInsuranceTimeStamp = 0;
	}
	//<-- End

#ifndef _SERVER
	m_bLockOperation = FALSE;
#endif
}

KItemList::~KItemList()
{
	
}

/*!*****************************************************************************
// Function		: KItemList::GetWeaponType
// Purpose		: 取得玩家装备的武器类型
// Return		: int 
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::GetWeaponLevel()
{
	const int nEquipIdx = m_EquipItem[itempart_weapon].nEquipIdx;
	if (nEquipIdx != 0 && !(m_EquipItem[itempart_weapon].IsMasked))
		return Item[nEquipIdx].GetLevel();
	else
		return -1;
}


void KItemList::GetWeaponDamage(int* nMin, int* nMax)
{
	int nWeaponIdx = m_EquipItem[itempart_weapon].nEquipIdx;
	if (nWeaponIdx != 0 && !(m_EquipItem[itempart_weapon].IsMasked))
	{
		int nMinDamage, nMaxDamage, nEnhance;
		int nDamageMinBase = 0;//Item[nWeaponIdx].m_aryBaseAttrib[0].nValue[0];
		int	nDamageMaxBase = 0;//Item[nWeaponIdx].m_aryBaseAttrib[1].nValue[0];
		nMinDamage = 0;
		nMaxDamage = 0;
		nEnhance = 0;
		for (int i = 0; i < 6; i++)
		{
//			switch(Item[nWeaponIdx].m_aryMagicAttrib[i].nAttribType)
//			{
//			case magic_weapondamagemin_v:
//				nMinDamage += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
//				break;
//			case magic_weapondamagemax_v:
//				nMaxDamage += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
//				break;
//			case magic_weapondamageenhance_p:
//				nEnhance += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
//				break;
//			default:
//				break;
//			}
		}
		*nMin = (nDamageMinBase + nMinDamage) * (100 + nEnhance) / 100;
		*nMax = (nDamageMaxBase + nMaxDamage) * (100 + nEnhance) / 100;
	}
	else	// 空手
	{
	int nSeries = Npc[Player[m_PlayerIdx].m_nIndex].m_Series;
//		*nMin = CBaseNumCalc::
//			GetPDamageViaStr(Player[m_PlayerIdx].m_nCurStrength, nSeries, TRUE);
//		*nMax = CBaseNumCalc::
//			GetPDamageViaStr(Player[m_PlayerIdx].m_nCurStrength, nSeries, FALSE);
	}
}

int KItemList::Add(int itemIndex, enumItemSyncType syncType)
{
	ItemPos pos;

	EXTRAINFOPLUS tagExtraPlus;
	tagExtraPlus.nItemGenre			= Item[itemIndex].GetGenre();
	tagExtraPlus.nParticularType	= Item[itemIndex].GetParticular();
	tagExtraPlus.nDetailType		= Item[itemIndex].GetDetailType();
	tagExtraPlus.nMaxItem			= Item[itemIndex].GetMaxItemCount();
	tagExtraPlus.nCurItem			= Item[itemIndex].GetItemCount();
	tagExtraPlus.pCampareItem       = &Item[itemIndex];

	if(SearchPosition(&pos, &tagExtraPlus) == false)
		return 0;

	return Add(itemIndex, pos.nPlace, pos.nX, pos.nY, NULL, syncType);
}

/******************************************************************************
// Function		: KItemList::Add
// Purpose		: 向ItemList中添加一个物品
// Return		: int 是否成功 
// Argumant		: int itemIndex
// Argumant		: int 容器号
// Argumant		: int 容器中的横坐标
// Argumant		: int 容器中的纵坐标
*****************************************************************************/
int KItemList::Add(int itemIndex, int place, int x, int y, int *pnOutIndex, enumItemSyncType syncType)
{
	bool check = (syncType != item_sync_type_init);

	if (itemIndex <= 0)
		return 0;
	
	if (Item[itemIndex].GetID() == 0)
		return 0;
	
	int listIndex = FindFree();
	if (!listIndex)
		return 0;

//ItemDebugLog Begin........................
#ifdef _SERVER
	bool bDump                  = false;
	int  nOldBelong             = -1;
	char szItemName[SZBUFLEN_0] = "";
	
	if (itemIndex > 0 && itemIndex < MAX_ITEM)
	{
		nOldBelong = Item[itemIndex].GetBelong();
		
		if (nOldBelong != -1)
		{
			bDump  = true; 
			strncpy(szItemName,Item[itemIndex].GetName(),sizeof(szItemName));
			szItemName[sizeof(szItemName) - 1] = 0;
		}//endif

	}//endif

	if (bDump)
	{
		char szDumpInfo[512] = "";
		snprintf(szDumpInfo,sizeof(szDumpInfo),"Add Itemlist:Index %d,Belong:%d,PlayerIndex:%d,Name:%s\n",itemIndex,nOldBelong,m_PlayerIdx,szItemName);
		szDumpInfo[sizeof(szDumpInfo) - 1] = 0;

		DumpInvalidItemOpeStack(true,szDumpInfo,4);
	}//end for bdump
#endif
//ItemDebugLog End.........................

#ifdef _SERVER
	//添加限制物品需要检查是否已经拥有该物品的最多实例
	if (check && Item[itemIndex].GetRestrictCount() > 0)
	{
		if (CountItem(Item[itemIndex].GetGenre(),
			Item[itemIndex].GetDetailType(),
			Item[itemIndex].GetParticular(),
			Item[itemIndex].GetLevel()) >= Item[itemIndex].GetRestrictCount())
		{
			return 0;
		}
	}
#endif

	//确定界面位置是否正确
	UIOBJECT_CONTAINER cont = corePos2ClientContainer((ITEM_POSITION)place);
	if(UOC_IDLE == cont)
		return 0;
	
	switch(place)
	{
	case pos_equip:
		{
			if (x < 0 || x >= itempart_num)
				return 0;
			
			if (m_EquipItem[x].nEquipIdx != 0)
				return 0;
			m_EquipItem[x].nEquipIdx = itemIndex;

			KNpc& aNpc = Npc[Player[m_PlayerIdx].m_nIndex];
			KLibOfBPT* pLibOfBPT = g_ItemGen.GetLibOfBPT( );
			const KBASICPROP_ITEM* pItemBP = NULL;

			if (check && !canEquip( itemIndex, GetEquipPlace( (EQUIPDETAILTYPE)Item[itemIndex].GetDetailType()) ) )
			{
				m_EquipItem[x].nEquipIdx = 0;
				return 0;
			}
			
#ifdef _SERVER
			if(!check && Item[itemIndex].GetGenre() != item_equip)
			{
				//记录系统日志
				char invalidEquipmentInfo[256] = { 0 };
				snprintf(invalidEquipmentInfo, sizeof(invalidEquipmentInfo), 
					"ItemList Init InvalidEquipment: PlayerName=\"%s\", ItemIndex=%d, ItemName=\"%s\"", 
					Player[m_PlayerIdx].GetPlayerName(), itemIndex, Item[itemIndex].GetName());
				invalidEquipmentInfo[sizeof(invalidEquipmentInfo) - 1] = 0;
				g_pLogSystem->SysDbgLog(invalidEquipmentInfo, strlen(invalidEquipmentInfo), sys_dbg_log_event_invalid_equipment);
			}
#endif

			switch(GetEquipPlace( (EQUIPDETAILTYPE)Item[itemIndex].GetDetailType() ))
			{
			case itempart_helm:
				
				pItemBP = pLibOfBPT->GetHelmRecord( Item[itemIndex].GetParticular() );
				aNpc.m_HelmType = pItemBP ? pItemBP->nItemRes : 0;
				aNpc.m_HelmPal	= g_ItemChangeRes.GetPal( BODY_PART_HELM, Item[itemIndex].GetParticular(), Item[itemIndex].GetLevel() );
				break;
			case itempart_armor:
				pItemBP = pLibOfBPT->GetArmorRecord( Item[itemIndex].GetParticular() );
				aNpc.m_ArmorType = pItemBP ? pItemBP->nItemRes : 0;
				aNpc.m_ArmorPal	 = g_ItemChangeRes.GetPal( BODY_PART_ARMOR, Item[itemIndex].GetParticular(), Item[itemIndex].GetLevel() );
				break;
			case itempart_weapon:
				pItemBP = pLibOfBPT->GetWeaponRecord( Item[itemIndex].GetParticular() );
				aNpc.m_WeaponType = pItemBP ? pItemBP->nItemRes : 0;
				aNpc.m_WeaponPal  = g_ItemChangeRes.GetPal( BODY_PART_WEAPON, Item[itemIndex].GetParticular(), Item[itemIndex].GetLevel() );
				break;
			case itempart_shoulder:
				pItemBP = pLibOfBPT->GetShoulderRecord( Item[itemIndex].GetParticular() );
				aNpc.m_ShoulderType = pItemBP ? pItemBP->nItemRes : 0;
				aNpc.m_ShoulderPal  = g_ItemChangeRes.GetPal( BODY_PART_SHOULDER, Item[itemIndex].GetParticular(), Item[itemIndex].GetLevel() );
				break;
			case itempart_cuff:
				pItemBP = pLibOfBPT->GetCuffRecord( Item[itemIndex].GetParticular() );
				aNpc.m_CuffType = pItemBP ? pItemBP->nItemRes : 0;
				aNpc.m_CuffPal  = g_ItemChangeRes.GetPal( BODY_PART_CUFF, Item[itemIndex].GetParticular(), Item[itemIndex].GetLevel() );
				break;
			case itempart_boots:
				pItemBP = pLibOfBPT->GetBootRecord( Item[itemIndex].GetParticular() );
				aNpc.m_BootType = pItemBP ? pItemBP->nItemRes : 0;
				aNpc.m_BootPal  = g_ItemChangeRes.GetPal( BODY_PART_BOOT, Item[itemIndex].GetParticular(), Item[itemIndex].GetLevel() );
				break;
			case itempart_horse:
				aNpc.m_HorseType = -1;
				aNpc.m_bRideHorse = FALSE;
				aNpc.m_HorsePal  = Default_PalIndex;
				break;
			default:
				break;
			}
		}
		break;
	default:
		//确定容器是否正确
		INVENTORY_ROOM itemRoom = corePos2coreRoom((ITEM_POSITION)place);
		if(itemRoom == room_num)
			return 0;
		if(m_Room[itemRoom].PlaceItem(x, y, itemIndex, m_PlayerIdx) == false)
			return 0;

		//PlaceItem有可能叠加，叠加的时候会把新生成的item删除，而改变原来物品的个数，
		//此时还用新物品的index就不对了，必须从新取得
		itemIndex = m_Room[itemRoom].FindItem(x, y);
		if(itemIndex <= 0)
			return 0;

		//特殊处理：当新增的物品是包裹扩展槽位时，根据物品属性加大包裹
		if(pos_itembox_extend == place)
		{
			m_Room[room_equipment].addSize(bagExtend(itemIndex, room_equipment));
			onBagSized(room_equipment);
		}
		if(pos_store_extend == place)
		{
			m_Room[room_repository].addSize(bagExtend(itemIndex, room_repository));
			onBagSized(room_repository);
		}
		break;
	}

	//在itemlist中加入新元素（如果不是叠加的话）
	if(FindSame(itemIndex) == 0)
	{
		m_Items[listIndex].nItemPrice = PlayerItem::INVALIDPRICE;
		m_Items[listIndex].nPlace = place;
		m_Items[listIndex].nX = x;
		m_Items[listIndex].nY = y;
		m_Items[listIndex].nIdx = itemIndex;
		
		m_FreeIdx.Remove(listIndex);
		m_UseIdx.Insert(listIndex);

		//ItemDebugLog Begin........................
        #ifdef _SERVER
		if (listIndex <= 0 || listIndex >= MAX_PLAYER_ITEM)
		{
			char szDumpInfo[512] = "";
			snprintf(szDumpInfo,sizeof(szDumpInfo),"Wrong ItemListIndex:%d,Name:%s\n",listIndex,Item[itemIndex].GetName());
			szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
			
			DumpInvalidItemOpeStack(true,szDumpInfo,4);
		}//end for bdump
        #endif
        //ItemDebugLog End.........................

		Item[itemIndex].SetBelong( m_PlayerIdx );
	}

	//更新其他关联信息
	switch(place)
	{
	case pos_equip:
		OnEquipChanged();
		break;
	case pos_equiproom:
		OnBagChanged(item_charm == Item[itemIndex].GetGenre());	
		break;
	}
	
	//从新取得itemIndex，因为叠加，可能与传入的不一样
	if(pnOutIndex != NULL)
	{
		*pnOutIndex = itemIndex;
	}

	KItem& item = Item[itemIndex];

#ifdef _SERVER	

	if ( item.isPickupBind()  && 
		!item.IsBind() )
	{
		item.SetBind(true);
		item.SyncAttribute(item_attr_isbind, Player[GetPlayerIndex()].GetNetConnectIdx());
	}

	ItemPos itPos;
	memset( &itPos, 0, sizeof(itPos) );
	itPos.nPlace = place;
	itPos.nX = x;
	itPos.nY = y;
	item.SyncItem( Player[m_PlayerIdx].m_nNetConnectIdx, itPos, syncType );

	if (item.IsTaskGiven())
	{
		item.SyncAttribute(item_attr_is_task_given, Player[GetPlayerIndex()].GetNetConnectIdx());
	}//endif

	item.SyncAttribute(item_attr_lockdate,Player[GetPlayerIndex()].GetNetConnectIdx());

	if (item.GetFlushTimes()) // Notice: FlushTimes Default Value is 0
		item.SyncAttribute(item_attr_flush_times,Player[GetPlayerIndex()].GetNetConnectIdx());
	
#endif
	
#ifndef _SERVER
	KObjAtContRegion	pInfo;
	
	pInfo.Region.h = x;
	pInfo.Region.v = y;
	pInfo.eContainer = cont;
	pInfo.Obj.uGenre = CGOG_ITEM;						//界面上装备和物品……所有物品都用CGOG_ITEM
	pInfo.Obj.uId = itemIndex;
	pInfo.Region.Height = Item[itemIndex].GetItemCount();	//物品数量 add by xiehong 2006-10-19
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 1);

	if (syncType != item_sync_type_split)
    {
        //物品同步消息
        char addItemMsg[256] = { 0 };
        char addItemMsgTemplate[256] = { 0 };
        switch(syncType)
        {
        case item_sync_type_gain:
            g_GetStringRes(sid_self_gain_item, addItemMsgTemplate, 256);
            break;
        case item_sync_type_pickup:
            g_GetStringRes(sid_self_pickup_item, addItemMsgTemplate, 256);
            break;
        case item_sync_type_trade:
            g_GetStringRes(sid_self_trade_item, addItemMsgTemplate, 256);
            break;
        case item_sync_type_buy:
			{
				bool needNotif = true;
				static ItemType lastBuyItem;
				static DWORD time = 0;
				if(GetTickCount() - time < 1000)
				{
					if(lastBuyItem.genre == Item[itemIndex].GetGenre()
						&& lastBuyItem.detail == Item[itemIndex].GetDetailType()
						&& lastBuyItem.particular == Item[itemIndex].GetParticular()
						&& lastBuyItem.level == Item[itemIndex].GetLevel())
					{
						needNotif = false;
					}
				}
				
				if(needNotif)
				{
					time = GetTickCount();
					lastBuyItem.genre = Item[itemIndex].GetGenre();
					lastBuyItem.detail = Item[itemIndex].GetDetailType();
					lastBuyItem.particular = Item[itemIndex].GetParticular();
					lastBuyItem.level = Item[itemIndex].GetLevel();
					g_GetStringRes(sid_self_buy_item, addItemMsgTemplate, 256);
				}
			}
            break;
        }
        sprintf(addItemMsg, addItemMsgTemplate, item.GetName());
        if (strlen(addItemMsg) > 0)
        {
            CoreDataChanged(GDCNI_ERROR_MESSAGE, (UINT)addItemMsg, 0);
        }	
    }
#endif

	#ifdef _DEBUG
	int nLoopIdx = 0;
	nLoopIdx = m_UseIdx.GetNext(nLoopIdx);
	g_DebugLog("[ITEM]Item Begin");
	while(nLoopIdx)
	{
		g_DebugLog("[ITEM]ItemListIdx:%d, Item:%d, ItemId:%d", nLoopIdx, m_Items[nLoopIdx].nIdx, Item[m_Items[nLoopIdx].nIdx].GetID());
		nLoopIdx = m_UseIdx.GetNext(nLoopIdx);
	}
	#endif
	
	return listIndex;
}


/*!*****************************************************************************
// Function		: KItemList::getItemOfTypeCount
// Purpose		: 得到某个容器内的某类物品的个数
// Return		: 个数总数 
// Argumant1	: 容器
// Argumant2	: 物品类别
// Argumant3	: 返回找到的物品列表（index）
// Comments		: 第三个参数不保证多线程情况下的正确性，它使用了公有空间，如果需要保存其内容，请copy到自己的空间
// Comments		: 第三个参数的最大个数为：MAX_PLAYER_ITEM
// Author		: xiehong 2007-9-19
*****************************************************************************/
int	KItemList::getItemOfTypeCount(ITEM_POSITION pos, ItemType type, int** itemIndexes)
{
	static int s_tempItemId[MAX_PLAYER_ITEM];	//作为返回内容

	int findItemCount = 0;
	int nextItem = 0;

	int itemIndex = 0;	//m_item的下标
	while(true)
	{
		itemIndex = m_UseIdx.GetNext(itemIndex);
		
		//找到尾了
		if (!itemIndex)
			break;

		if(m_Items[itemIndex].nPlace != pos)
		{
			continue;
		}

		int itemListIndex = m_Items[itemIndex].nIdx;
		KItem& item = Item[itemListIndex];
		
		if(Item[itemListIndex].GetGenre() == type.genre
			&& Item[itemListIndex].GetDetailType() == type.detail
			&& Item[itemListIndex].GetParticular() == type.particular
			&& Item[itemListIndex].GetLevel() == type.level)
		{
			findItemCount += Item[itemListIndex].GetItemCount();
			s_tempItemId[nextItem++] = itemListIndex;
		}
	}
	
	s_tempItemId[nextItem] = 0;

	*itemIndexes = s_tempItemId;

	return findItemCount;
}

/*!*****************************************************************************
// Function		: KItemList::removeItemOfTypeCount
// Purpose		: 删除某个容器内的某类物品的个数
// Return		: 是否删除成功（原子操作，只有删除和一个都没删两种情况） 
// Argumant1	: 容器
// Argumant2	: 物品类别
// Argumant3	: 个数
// Author		: xiehong 2007-9-19
*****************************************************************************/
bool KItemList::removeItemOfTypeCount(ITEM_POSITION pos, ItemType type, int count)
{
	int* indexes = NULL;
	int haveCount = getItemOfTypeCount(pos, type, &indexes);

	if(haveCount < count)
	{
		return false;
	}

	int indexOfIndexes = 0;

	while(indexes[indexOfIndexes])
	{
		int itemIndex = indexes[indexOfIndexes];
		KItem& item = Item[itemIndex];
		int removeCount = count < item.GetItemCount() ? count : item.GetItemCount();
		if(removeCount < item.GetItemCount())
		{
			Remove(itemIndex, removeCount);
		}
		else
		{
			Remove(itemIndex);
			ItemSet.Remove(itemIndex);
		}
		count -= removeCount;
		if(count <= 0)
		{
			return true;
		}
		++indexOfIndexes;
	}
	return false;
}

/*!*****************************************************************************
// Function		: KItemList::Remove
// Purpose		: 玩家失去一个装备
// Return		: int 
// Argumant		: int nGameIdx为游戏世界中道具数组的编号
// Argumant		: bool bSkillShortcut 快捷技能标志
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Remove(int nGameIdx, int nRemoveCount, bool syncToClient, bool removeFromItemSet, bool checkTrade,  bool bUseIbItem)
{
	if (!nGameIdx)
		return FALSE;
	
	int nIdx = FindSame(nGameIdx);
	
	if (!nIdx)
		return FALSE;
	
	//ItemDebugLog Begin........................
#ifdef _SERVER
	if (Item[nGameIdx].GetBelong() != m_PlayerIdx)
	{
		char szDumpInfo[512] = "";
		snprintf(szDumpInfo,sizeof(szDumpInfo),"InvalidRemove From ItemList:Index %d,Belong %d,PlayerIndex %d,ItemName:%s\n",nGameIdx,Item[nGameIdx].GetBelong(),m_PlayerIdx,Item[nGameIdx].GetName());
		szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
		DumpInvalidItemOpeStack(true,szDumpInfo,4);
	}//endif
#endif
    //ItemDebugLog End.........................

	ITEM_POSITION pos = (ITEM_POSITION)(m_Items[nIdx].nPlace);
	
#ifdef _SERVER

	bool needCancelTrade = false;
	//如果交易过程中删除了交易栏中的物品则直接取消交易
	if(pos_traderoom == pos && Player[m_PlayerIdx].m_cTrade.isTrading() && checkTrade)
	{
		needCancelTrade = true;
	}

	ITEM_REMOVE_SYNC	sRemove;
	sRemove.ProtocolType = s2c_removeitem;
	sRemove.m_ID = Item[nGameIdx].GetID();

	int nCurItemCount = Item[nGameIdx].GetItemCount();
	if ( nRemoveCount > 0 && nRemoveCount < nCurItemCount  )
	{
		int originalDestWeight = Item[nGameIdx].GetItemWeight();
		Item[nGameIdx].SetItemCount( nCurItemCount - nRemoveCount );
		m_Room[room_equipment].AddWeight(Item[nGameIdx].GetItemWeight() - originalDestWeight);
		
		if(pos_equiproom == pos)
		{
			OnBagChanged(item_charm == Item[nGameIdx].GetGenre());
		}//endif

		if (g_pServer != NULL)
		{
			Item[nGameIdx].SyncItemRefresh( Player[m_PlayerIdx].m_nNetConnectIdx );
		}//endif

		return TRUE;
	}


#endif
	
	switch(pos)
	{
	case pos_equip:
		{
			m_EquipItem[m_Items[nIdx].nX].nEquipIdx = 0;
			KNpc& aNpc = Npc[Player[m_PlayerIdx].GetNpcIndex()];
			switch(GetEquipPlace( (EQUIPDETAILTYPE)Item[nGameIdx].GetDetailType() ))
			{
			case itempart_helm:
				aNpc.m_HelmType = 0;
				aNpc.m_HelmPal = Default_PalIndex;
				break;
			case itempart_armor:
				aNpc.m_ArmorType = 0;
				aNpc.m_ArmorPal = Default_PalIndex;
				break;
			case itempart_weapon:
				aNpc.m_WeaponType = 0;
				aNpc.m_WeaponPal = Default_PalIndex;
				break;
			case itempart_shoulder:
				aNpc.m_ShoulderType = 0;
				aNpc.m_ShoulderPal = Default_PalIndex;
				break;
			case itempart_cuff:
				aNpc.m_CuffType = 0;
				aNpc.m_CuffPal	= Default_PalIndex;
				break;
			case itempart_boots:
				aNpc.m_BootType = 0;
				aNpc.m_BootPal = Default_PalIndex;
				break;
			}
			OnEquipChanged();
		}
		break;
	case pos_equiproom:
		m_Room[room_equipment].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY);
		break;
	case pos_repositoryroom:
		m_Room[room_repository].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY);
		break;
	case pos_traderoom:
		m_Room[room_trade].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY);
		break;
#ifndef _SERVER
	case pos_trade1:
		m_Room[room_trade1].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY
			);
		break;
#endif
#ifndef _SERVER
	case pos_pet_feed_box:
		m_Room[room_pet_feed].PickUpItem(nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY);		
		break;
	case pos_itembox_extend:
		m_Room[room_itembox_extend].PickUpItem(nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY);		
		break;
	case pos_store_extend:
		m_Room[room_store_extend].PickUpItem(nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY);		
		break;
#endif
	default:	
		break;
	}

#ifdef _SERVER
	if(syncToClient && g_pServer != NULL)
	{
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
	}
#endif

#ifndef _SERVER
	
	//特殊处理：当减少的物品是包裹扩展槽位时，根据物品属性减少包裹
	if(pos_itembox_extend == m_Items[nIdx].nPlace)
	{
		m_Room[room_equipment].addSize(-bagExtend(m_Items[nIdx].nIdx, room_equipment));
		onBagSized(room_equipment);
	}
	if(pos_store_extend == m_Items[nIdx].nPlace)
	{
		m_Room[room_repository].addSize(-bagExtend(m_Items[nIdx].nIdx, room_repository));
		onBagSized(room_repository);
	}
	
	// 客户端从玩家身上去除装备就应该从装备表中去除掉。
	
	KItem tmpItem;
	
	tmpItem = Item[m_Items[nIdx].nIdx];
	if (removeFromItemSet)
	{
		ItemSet.Remove(m_Items[nIdx].nIdx);
	}	
	
	// 界面处理
	KObjAtContRegion pInfo;
	
	pInfo.Obj.uGenre = CGOG_ITEM;
	pInfo.Obj.uId = m_Items[nIdx].nIdx;
	switch(pos)
	{ 
	case pos_equiproom:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_ITEM_TAKE_WITH;
		break;
	case pos_repositoryroom:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_STORE_BOX;
		break;
	case pos_equip:
		pInfo.Region.h = 0;
		pInfo.Region.v = m_Items[nIdx].nX;
		pInfo.eContainer = UOC_EQUIPTMENT;
		break;
	case pos_trade1:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_OTHER_TO_BE_TRADE;
		break;
	case pos_traderoom:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_TO_BE_TRADE;
		break;
	case pos_pet_feed_box:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_PET_FEED_BOX;
		break;
	}
	
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 0);

#endif
		
	m_Items[nIdx].nIdx = 0;
	m_Items[nIdx].nPlace = 0;
	m_Items[nIdx].nX = 0;
	m_Items[nIdx].nY = 0;
	m_Items[nIdx].nItemPrice = PlayerItem::INVALIDPRICE;
	m_FreeIdx.Insert(nIdx);
	m_UseIdx.Remove(nIdx);

	Item[nGameIdx].SetBelong( -1 );

	switch(pos)
	{
	case pos_equip:
		OnEquipChanged();
		break;
	case pos_equiproom:
		OnBagChanged(item_charm == Item[nGameIdx].GetGenre());
		break;
	}

#ifdef _SERVER

	if ( bUseIbItem )
	{
		int count = Item[nGameIdx].GetItemCount();
		if ( count == 1 )
		{
			Item[nGameIdx].SetItemCount(0);
		}
		if ( Item[nGameIdx].NeedIBUse() )
		{
			IBUse_Param ibuse_param;
			memset( &ibuse_param, 0, sizeof(ibuse_param) );
			
			ibuse_param.nItemGenre		= Item[nGameIdx].GetGenre();
			ibuse_param.nItemDetail		= Item[nGameIdx].GetDetailType();
			ibuse_param.nItemParticular = Item[nGameIdx].GetParticular();
			ibuse_param.IBGuid			= Item[nGameIdx].GetIBGuid();
			
			Player[m_PlayerIdx].UseIBItem( ibuse_param );
		}
		if ( Item[nGameIdx].GetGenre() == item_ib )
		{
			KIBLog::getSingleton().DelIBItem( use_delete, Item[nGameIdx].GetGUID(), GetPlayerIndex() );
		}
	}
	if(needCancelTrade)
	{
		Player[m_PlayerIdx].tradeServerDoCanceTrade();
	}
#endif

	return TRUE;
}

/*!*****************************************************************************
// Function		: KItemList::FindFree
// Purpose		: 查找可用空索引
// Return		: int 
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::FindFree()
{
	return m_FreeIdx.GetNext(0);
}

/*!*****************************************************************************
// Function		: KItemList::FindSame
// Purpose		: 
// Return		: int 
// Argumant		: int nGameIdx
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::FindSame(int nGameIdx)
{
	int nIdx = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;

		//堵漏 xiehong 2008-4-16
		if(nIdx < 0 || nIdx >= MAX_PLAYER_ITEM)
		{
			break;
		}

		if (m_Items[nIdx].nIdx == nGameIdx)
			return nIdx;
	}
	return 0;
}
/*!*****************************************************************************
// Function		: KItemList::Init
// Purpose		: 初始化玩家装备列表
// Return		: BOOL
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Init(int nPlayerIdx)
{
	m_PlayerIdx = nPlayerIdx;
	m_nBackHand = 0;
	// TODO: Maybe we can read size from ini file.
	
	int playerInitItemBoxSize = ConfigManager::Singleton().GetGlobalVariable(global_var_item_box_init_size);
#ifndef _SERVER
	onBagSized(room_equipment);	
#endif
	m_Room[room_equipment].Init(EQUIPMENT_ROOM_WIDTH, EQUIPMENT_ROOM_HEIGHT, playerInitItemBoxSize);
	m_Room[room_repository].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT, REPOSITORY_INIT_SPACE);
	m_Room[room_itembox_extend].Init(ITEMBOX_EXTEND_WIDTH, ITEMBOX_EXTEND_HEIGHT);
	m_Room[room_store_extend].Init(STORE_EXTEND_WIDTH, STORE_EXTEND_HEIGHT);

	m_Room[room_trade].Init(TRADE_ROOM_WIDTH, TRADE_ROOM_HEIGHT);
	
#ifdef _SERVER
	memset( &m_GroupCD, -1, sizeof(m_GroupCD) );
	m_BasicPropertyMonitor.Init();
	m_CharmMonitor.Init();
#else
	m_Room[room_trade1].Init(TRADE_ROOM_WIDTH, TRADE_ROOM_HEIGHT);		// 这个的大小必须与 room_trade 的大小一样
	m_Room[room_beset].Init(BESET_ROOM_WIDTH, BESET_ROOM_HEIGHT);
	//宠物喂养的物品
	m_Room[room_pet_feed].Init(PET_FEED_ROOM_WIDTH, PET_FEED_ROOM_HEIGHT);
	m_AbradeMonitor.Init();	
#endif
	ZeroMemory(m_EquipItem, sizeof(m_EquipItem));				// 玩家装备的道具（对应游戏世界中道具数组的索引）
	ZeroMemory(m_EquipOverdataState,sizeof(m_EquipOverdataState));
	ZeroMemory(m_Items, sizeof(m_Items));						// 玩家拥有的所有道具（包括装备着的和箱子里放的，对应游戏世界中道具数组的索引）
	m_nListCurIdx = 0;											// 用于 GetFirstItem 和 GetNextItem
	m_nWeightTaken = 0;											// Add by [Adt.X], checked bug. 2004.7.26
	m_FreeIdx.Init(MAX_PLAYER_ITEM);
	m_UseIdx.Init(MAX_PLAYER_ITEM);
	
	int i=0;
	for (i = MAX_PLAYER_ITEM - 1; i > 0 ; i--)
	{
		m_FreeIdx.Insert(i);
	}
	
	for (i=0; i<itempart_num; i++)
	{
		mEquipInsurance[i].mInsuranceTimeStamp = 0;
	}

	m_ArmorSetMonitor.Init();
	m_YaoMonitor.Init();
	m_TalismanMonitor.Init();
	
	memset( &m_ShortCut, -1, sizeof(m_ShortCut) );
	

	return TRUE;
}


//************************************
// Method:    CanEquip
// FullName:  KItemList::CanEquip
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: int equipItemIndex
// Parameter: int toEquipPos
//************************************
bool KItemList::canEquip(int equipItemIndex, ITEM_PART toEquipPos)
{
	if (m_PlayerIdx <= 0 || equipItemIndex <= 0 || equipItemIndex >= MAX_ITEM || Item[equipItemIndex].GetGenre() != item_equip)
		return false;
	
	//检查位置是否正确
	if (toEquipPos == itempart_unidentified || !equipPosCheck(equipItemIndex, toEquipPos))
	{
		return false;
	}

	KPlayer& player = Player[m_PlayerIdx];	
	KItem& item = Item[equipItemIndex];

	//检查职业
	if (!item.IsMatchProfessionRequirement(player))
		return false;

	//检查属性
	if (!item.IsMatchPropertyRequirement(player))
		return false;
	
	//检查等级
	if(!item.IsMatchLevelRequirement(player))
		return false;

	return true;
}

bool KItemList::canEquip(ItemPos* sourPos, ITEM_PART equipRoomPos)
{
	//要装备的物品在itemset中的索引
	int equipItemIndex = m_Room[corePos2coreRoom((ITEM_POSITION)sourPos->nPlace)].FindItem(sourPos->nX, sourPos->nY);
	if(equipItemIndex < 1)
		return false;
	return canEquip(equipItemIndex, equipRoomPos);
}

/*!*****************************************************************************
// Function		: KItemList::Equip
// Purpose		: 
// Return		: 
// Argumant		: 
// Argumant		: 
// Comments		:
// Author		: 
*****************************************************************************/
bool KItemList::Equip(const ItemPos* itemPos, ITEM_PART equipPos /* = itempart_unidentified */)
{	
	//得到要装备的物品的物品索引并检查是否有效
	INVENTORY_ROOM roomIndex = corePos2coreRoom((ITEM_POSITION)itemPos->nPlace);
	if(room_num == roomIndex)
	{
		return false;
	}

	int equipItemIndex = m_Room[roomIndex].FindItem(itemPos->nX, itemPos->nY);
	if (m_PlayerIdx <= 0 || equipItemIndex <= 0)
		return false;
	//itemPos对应的必须是个装备
	if(item_equip != Item[equipItemIndex].GetGenre())
		return false;
	
	//如果未指定装备位置，则自动判断该装备在哪个位置
	if (itempart_unidentified == equipPos)
	{
		equipPos = GetEquipPlace((EQUIPDETAILTYPE)Item[equipItemIndex].GetDetailType());
	}
// 	//判断该物品是否能够装备上
// 	if (!canEquip(equipItemIndex, equipPos))
// 	{
// 		return false;
// 	}

	//这是要装备物品在itemlist中的索引，由此来得到它在room里的x、y
	int sourListIndex = FindSame(equipItemIndex);
	//从物品栏拿起要交换到装备栏的物品
	int sourX = m_Items[sourListIndex].nX;
	int sourY = m_Items[sourListIndex].nY;
	if (!m_Room[room_equipment].PickUpItem(equipItemIndex, sourX, sourY))
		return false;
	
	int replaceItemIndex = m_EquipItem[equipPos].nEquipIdx;

	//如果该部位上已经装备了物品，则把该物品先换到刚才被拿起的物品所在的位置
	if(m_EquipItem[equipPos].nEquipIdx != 0)
	{
		//先把卸下的装备放在容器里
		int destEquipIndex = m_EquipItem[equipPos].nEquipIdx;
		m_Room[room_equipment].PlaceItem(sourX, sourY, destEquipIndex, m_PlayerIdx);
		//再改变ItemList到容器的索引
		int destListIndex = FindSame(destEquipIndex);
		m_Items[destListIndex].nPlace = pos_equiproom;
		m_Items[destListIndex].nX = sourX;
		m_Items[destListIndex].nY = sourY;
	}

#ifdef _SERVER
	//如果是装备绑定且尚未绑定，则需要设置为绑定
	if (Item[equipItemIndex].IsEquipBind() && !Item[equipItemIndex].IsBind())
	{
		Item[equipItemIndex].SetBind(true);
		Item[equipItemIndex].SyncAttribute(item_attr_isbind, Player[GetPlayerIndex()].GetNetConnectIdx());
	}
#endif

	//将物品装备到装备栏
	m_EquipItem[equipPos].nEquipIdx = equipItemIndex;
	m_EquipItem[equipPos].IsMasked = false;
	m_Items[sourListIndex].nPlace = pos_equip;
	m_Items[sourListIndex].nX = equipPos;
	m_Items[sourListIndex].nY = 0;

	// 换装
/*	KNpc& aNpc = Npc[Player[m_PlayerIdx].m_nIndex];
	const KItem& aItem = Item[equipItemIndex];
	aNpc.m_HelmType = aItem.GetRes( );
	aNpc.m_HelmPal = 0;//*/

	KNpc& aNpc = Npc[Player[m_PlayerIdx].m_nIndex];
	KLibOfBPT* pLibOfBPT = g_ItemGen.GetLibOfBPT( );
	const KBASICPROP_ITEM* pItemBP = NULL;

	switch(GetEquipPlace( (EQUIPDETAILTYPE)Item[equipItemIndex].GetDetailType() ))
	{
	case itempart_helm:
		pItemBP = pLibOfBPT->GetHelmRecord( Item[equipItemIndex].GetParticular() );
		aNpc.m_HelmType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_HelmPal	= g_ItemChangeRes.GetPal( BODY_PART_HELM, Item[equipItemIndex].GetParticular(), Item[equipItemIndex].GetLevel() );
		break;
	case itempart_armor:
		pItemBP = pLibOfBPT->GetArmorRecord( Item[equipItemIndex].GetParticular() );
		aNpc.m_ArmorType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_ArmorPal	 = g_ItemChangeRes.GetPal( BODY_PART_ARMOR, Item[equipItemIndex].GetParticular(), Item[equipItemIndex].GetLevel() );
		break;
	case itempart_weapon:
		pItemBP = pLibOfBPT->GetWeaponRecord( Item[equipItemIndex].GetParticular() );
		aNpc.m_WeaponType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_WeaponPal  = g_ItemChangeRes.GetPal( BODY_PART_WEAPON, Item[equipItemIndex].GetParticular(), Item[equipItemIndex].GetLevel() );
		break;
	case itempart_shoulder:
		pItemBP = pLibOfBPT->GetShoulderRecord( Item[equipItemIndex].GetParticular() );
		aNpc.m_ShoulderType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_ShoulderPal  = g_ItemChangeRes.GetPal( BODY_PART_SHOULDER, Item[equipItemIndex].GetParticular(), Item[equipItemIndex].GetLevel() );
		break;
	case itempart_cuff:
		pItemBP = pLibOfBPT->GetCuffRecord( Item[equipItemIndex].GetParticular() );
		aNpc.m_CuffType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_CuffPal  = g_ItemChangeRes.GetPal( BODY_PART_CUFF, Item[equipItemIndex].GetParticular(), Item[equipItemIndex].GetLevel() );
		break;
	case itempart_boots:
		pItemBP = pLibOfBPT->GetBootRecord( Item[equipItemIndex].GetParticular() );
		aNpc.m_BootType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_BootPal  = g_ItemChangeRes.GetPal( BODY_PART_BOOT, Item[equipItemIndex].GetParticular(), Item[equipItemIndex].GetLevel() );
		break;
	case itempart_horse:
		aNpc.m_HorseType = -1;
		aNpc.m_bRideHorse = FALSE;
		aNpc.m_HorsePal  = g_ItemChangeRes.GetPal( BODY_PART_HORSE, Item[equipItemIndex].GetParticular(), Item[equipItemIndex].GetLevel() );
		break;
	default:
		break;
	}

#ifdef _SERVER
	//发送给客户端
	PLAYER_MOVE_ITEM_SYNC moveItem;
	moveItem.ProtocolType = s2c_playermoveitem;
	moveItem.sourPlace	= itemPos->nPlace;
	moveItem.sourX		= itemPos->nX;
	moveItem.sourY		= itemPos->nY;
	moveItem.destPlace	= pos_equip;
	moveItem.destX		= equipPos;
	moveItem.destY		= -1;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&moveItem, sizeof(PLAYER_MOVE_ITEM_SYNC));	

	if (TalismanManager::Singleton().IsValidTalisman(equipItemIndex))
	{		
		MaskEquipment(itempart_talisman, true);

		KItem& talisman = Item[equipItemIndex];

		if (replaceItemIndex > 0)
		{
			DelGroupCoolDown(Item[replaceItemIndex].GetGroup());
		}
		
		AddGroupCoolDown(talisman.GetGroup(), talisman.GetTalismanCoolDown(), this, EnableTalismanCallback);
	}
#endif

	OnEquipChanged();

	OnBagChanged();

#ifndef _SERVER
	//发送给界面通知重新绘制物品
	KObjAtContRegion itemNewPos;
	itemNewPos.Region.h		= equipPos;
	itemNewPos.Region.v		= -1;
	itemNewPos.Obj.uGenre	= CGOG_ITEM;
	itemNewPos.Obj.uId		= equipItemIndex;
	itemNewPos.Region.Height= Item[equipItemIndex].GetItemCount();
	itemNewPos.nContainer	= UOC_EQUIPTMENT;
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (unsigned int)&itemNewPos, TRUE);
	int unEquipItemIndex = m_Room[corePos2coreRoom((ITEM_POSITION)itemPos->nPlace)].FindItem(sourX, sourY);
	itemNewPos.Region.h		= sourX;
	itemNewPos.Region.v		= sourY;
	itemNewPos.nContainer	= UOC_ITEM_TAKE_WITH;
	if(unEquipItemIndex > 0)
	{
		itemNewPos.Obj.uId		= unEquipItemIndex;
		itemNewPos.Region.Height= Item[unEquipItemIndex].GetItemCount();
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (unsigned int)&itemNewPos, TRUE);
	}
	else
	{	
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (unsigned int)&itemNewPos, FALSE);
	}	
#endif

	return true;
}

/*!*****************************************************************************
added by rocker 2004.3.2
检查装备是否安装在人身上
*****************************************************************************/
BOOL KItemList::IsItemInEquip(int nItemId) const
{
	for (int i=0; i<itempart_num; i++)
	{
		if (nItemId == Item[m_EquipItem[i].nEquipIdx].GetID())
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KItemList::HaveItemEquiped(int nItemDetail, int nItemParticular, int nItemLevel)
{
	for (int i=0; i<itempart_num; i++)
	{
		if (m_EquipItem[i].nEquipIdx > 0 && 
			m_EquipItem[i].nEquipIdx < MAX_ITEM &&
			(nItemDetail == Item[m_EquipItem[i].nEquipIdx].GetDetailType()) &&
			(nItemParticular == Item[m_EquipItem[i].nEquipIdx].GetParticular()) &&
			(nItemLevel == Item[m_EquipItem[i].nEquipIdx].GetLevel()))
		{
			return TRUE;
		}
	}
	return FALSE;
}

bool KItemList::unEquip(const ITEM_PART equipPos, const ItemPos* unEquipPos)
{
	//检查装备位置索引是否有效
	if(equipPos < 0 || equipPos > itempart_num)
		return false;
	//得到装备的物品索引并检查索引是否有效
	int equipIndex = m_EquipItem[equipPos].nEquipIdx;
	if(equipIndex < 1)
		return false;
	
	//检查目的位置是否已经存在物品
	//FindItem在位置信息不正确的时候返回－1，在找到物品时返回>0的数字，只有在位置正确但没有物品时返回0
	INVENTORY_ROOM unEquipRoom = corePos2coreRoom((ITEM_POSITION)unEquipPos->nPlace);
	int itemIndex = m_Room[unEquipRoom].FindItem(unEquipPos->nX, unEquipPos->nY);
	//如果卸装位置上已经有物品则可以看成把该物品装备到身上，走装备流程
	if(itemIndex > 0)
	{
		return Equip(unEquipPos, equipPos);
	}
	//如果位置不正确，则直接返回不成功
	else if(itemIndex == -1)
		return false;

	//先把装备从装备兰拿起来
	m_EquipItem[equipPos].nEquipIdx = 0;
	//再放在容器里
	m_Room[unEquipRoom].PlaceItem(unEquipPos->nX, unEquipPos->nY, equipIndex, m_PlayerIdx);
	//再改变itemlist中的反向索引
	int destListIndex = FindSame(equipIndex);
	m_Items[destListIndex].nPlace = unEquipPos->nPlace;
	m_Items[destListIndex].nX = unEquipPos->nX;
	m_Items[destListIndex].nY = unEquipPos->nY;

#ifdef _SERVER
	//发送给客户端
	PLAYER_MOVE_ITEM_SYNC moveItem;
	moveItem.ProtocolType = s2c_playermoveitem;
	moveItem.sourPlace	= pos_equip;
	moveItem.sourX		= equipPos;
	moveItem.sourY		= -1;
	moveItem.destPlace	= unEquipPos->nPlace;
	moveItem.destX		= unEquipPos->nX;
	moveItem.destY		= unEquipPos->nY;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&moveItem, sizeof(PLAYER_MOVE_ITEM_SYNC));

	if (TalismanManager::Singleton().IsValidTalisman(equipIndex))
	{		
		DelGroupCoolDown(Item[equipIndex].GetGroup());
	}

//	Item[equipIndex].ClearAllItemBuff( Npc[Player[m_PlayerIdx].m_nIndex].m_Index );
#endif

	KNpc& aNpc = Npc[Player[m_PlayerIdx].m_nIndex];
	KLibOfBPT* pLibOfBPT = g_ItemGen.GetLibOfBPT( );
	const KBASICPROP_ITEM* pItemBP = NULL;

	switch(GetEquipPlace( (EQUIPDETAILTYPE)Item[equipIndex].GetDetailType() ))
	{
	case itempart_helm:
		pItemBP = pLibOfBPT->GetHelmRecord( 0 );
		aNpc.m_HelmType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_HelmPal	= Default_PalIndex;
		break;
	case itempart_armor:
		pItemBP = pLibOfBPT->GetArmorRecord( 0 );
		aNpc.m_ArmorType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_ArmorPal	= Default_PalIndex;
		break;
	case itempart_weapon:
		pItemBP = pLibOfBPT->GetWeaponRecord( 0 );
		aNpc.m_WeaponType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_WeaponPal	= Default_PalIndex;
		break;
	case itempart_shoulder:
		pItemBP = pLibOfBPT->GetShoulderRecord( 0 );
		aNpc.m_ShoulderType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_ShoulderPal	= Default_PalIndex;
		break;
	case itempart_cuff:
		pItemBP = pLibOfBPT->GetCuffRecord( 0 );
		aNpc.m_CuffType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_CuffPal  = Default_PalIndex;
		break;
	case itempart_boots:
		pItemBP = pLibOfBPT->GetBootRecord( 0 );
		aNpc.m_BootType = pItemBP ? pItemBP->nItemRes : 0;
		aNpc.m_BootPal	= Default_PalIndex;
		break;
	case itempart_horse:
		aNpc.m_HorseType = -1;
		aNpc.m_bRideHorse = FALSE;
		aNpc.m_HorsePal	= Default_PalIndex;
		break;
	default:
		break;
	}

	OnEquipChanged();

	OnBagChanged();

#ifndef _SERVER
	//发送给界面通知重新绘制物品
	KObjAtContRegion itemNewPos;
	itemNewPos.Region.h		= unEquipPos->nX;
	itemNewPos.Region.v		= unEquipPos->nY;
	itemNewPos.Obj.uGenre	= CGOG_ITEM;
	itemNewPos.Obj.uId	= equipIndex;
	itemNewPos.Region.Height= Item[equipIndex].GetItemCount();
	itemNewPos.nContainer	= corePos2ClientContainer((ITEM_POSITION)unEquipPos->nPlace);
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (unsigned int)&itemNewPos, TRUE);
	itemNewPos.Region.h		= equipPos;
	itemNewPos.Region.v		= -1;
	itemNewPos.nContainer	= UOC_EQUIPTMENT;	
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (unsigned int)&itemNewPos, FALSE);	
#endif

	return true;
}

/*!*****************************************************************************
// Function		: KItemList::UnEquip
// Purpose		: 移除装备
// Return		: BOOL 
// Argumant		: int nIdx 游戏世界中的道具数组索引
// Comments		: 
// Author		: Spe
*****************************************************************************/
BOOL KItemList::UnEquip(int nIdx, int nPos/* = -1*/)
{
	int i = 0;
	if (m_PlayerIdx <= 0)
		return FALSE;
	
	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	if (nIdx <= 0)
		return FALSE;
	
	if (nPos <= 0)
	{
		for (i = 0; i < itempart_num; i++)
		{
			if (m_EquipItem[i].nEquipIdx == nIdx)
			{
				break;
			}
		}
		// 没有发现身上有这个装备
		if (i == itempart_num)
			return FALSE;
		
	}
	else
	{
		if (m_EquipItem[nPos].nEquipIdx != nIdx)	// 东西不对
			return FALSE;
		i = nPos;
	}
	// 移除该装备对NPC的属性调整
	//如果是马，要真的骑着才移去效果
	//if(i != itempart_horse || Npc[nNpcIdx].m_bRideHorse == TRUE)

	// 新的技能系统要求即使是下马状态仍然保持马的蓝色属性，所以，不管
	// 是不是真的骑，都进行属性调整
	if (!m_EquipItem[i].IsMasked || i == itempart_horse)
	{
		if(i == itempart_horse)
		{
			// 对马特殊处理
			//Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx], TRUE, TRUE);
		}
		else
		{
			//Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx]);
		}
	}
	// 这句话一定要放在上一句后，保证计算该装备激活的装备激活属性个数计算的正确性
	m_EquipItem[i].nEquipIdx = 0;
	m_EquipItem[i].IsMasked = false;
	// 换装

#ifdef _SERVER
	if(i == itempart_weapon)
	{
		KNpc &clsNpc = Npc[Player[m_PlayerIdx].m_nIndex];
		// Produce check
		if(clsNpc.m_tagProduceState.nProduceSpeed > 0)
		{
			clsNpc.ClearProduceState();
		}
	}
#endif
	return TRUE;
}

ITEM_PART KItemList::GetEquipPlace(EQUIPDETAILTYPE nType)
{
	ITEM_PART nRet = itempart_unidentified;
	switch(nType)
	{
	case equip_amulet:		//玉佩
		nRet = itempart_amulet;
		break;
	case equip_helm:		//头
		nRet = itempart_helm;
		break;
	case equip_pendant:		//披风
		nRet = itempart_pendant;
		break;
	case equip_weapon:	//武器
		nRet = itempart_weapon;
		break;
	case equip_armor:		//衣服
		nRet = itempart_armor;
		break;
	case equip_shoulder:		//腰带
		nRet = itempart_shoulder;
		break;
	case equip_ring:		//戒指
		nRet = itempart_ring;
		break;
	case equip_boots:		//鞋子
		nRet = itempart_boots;
		break;
	case equip_cuff:		//手镯
		nRet = itempart_cuff;
		break;
	case equip_talisman:	//法宝
		nRet = itempart_talisman;
		break;
	default:
		break;
	}
	return nRet;
}

/*!*****************************************************************************
// Function		: KItemList::Fit
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nIdx
// Argumant		: int nPlace
// Comments		:
// Author		: Spe
*****************************************************************************/
bool KItemList::equipPosCheck(int equipItemIndex, ITEM_PART toEquipPos)
{
	if(itempart_unidentified == toEquipPos)
		return false;
	bool bRet = false;
	if(Item[equipItemIndex].GetGenre() != item_equip)
		return false;
	switch(Item[equipItemIndex].GetDetailType())
	{
	case equip_weapon:
		if (toEquipPos == itempart_weapon)
			bRet = true;
		break;
	case equip_armor:
		if (toEquipPos == itempart_armor)
			bRet = true;
		break;
	case equip_shoulder:
		if (toEquipPos == itempart_shoulder)
			bRet = true;
		break;
	case equip_boots:
		if (toEquipPos == itempart_boots)
			bRet = true;
		break;
	case equip_amulet:
		if (toEquipPos == itempart_amulet)
			bRet = true;
		break;
	case equip_pendant:
		if (toEquipPos == itempart_pendant)
			bRet = true;
		break;
	case equip_helm:
		if (toEquipPos == itempart_helm)
			bRet = true;
		break;
	case equip_cuff:
		if (toEquipPos == itempart_cuff)
			bRet = true;
		break;
	case equip_ring:
		if (toEquipPos == itempart_ring)
			bRet = true;
		break;
	case equip_talisman:
		if (toEquipPos == itempart_talisman)
			bRet = true;
		break;
	}
	return bRet;
}

BOOL KItemList::EatMecidine(int nIdx, int nTargetIdx)
{
	if (nIdx <= 0)
		return FALSE;

	if (m_PlayerIdx <= 0)
		return FALSE;
	
	if (!CanUseItem(nIdx, nTargetIdx))
		return FALSE;

#ifdef _SERVER

	if ( Item[nIdx].IsMapArea(m_PlayerIdx) )
	{
		Item[nIdx].ItemErrCodeToClient( m_PlayerIdx, item_inlay_error_targetitem_rule );
		return FALSE;
	}

	Player[m_PlayerIdx].tradeServerDoCanceTrade();

	//使用前的重量
	int originalItemWeight = Item[nIdx].GetItemWeight();
	
	KItem oldItemTmp;
	oldItemTmp = Item[nIdx];
	oldItemTmp.SetBelong(-1);

	//使用物品
	BOOL bUseItemOk = Item[nIdx].UseItem( m_PlayerIdx, nIdx, nTargetIdx );
	BOOL bIsSameItem = (oldItemTmp.GetItemIndex() == Item[nIdx].GetItemIndex() && oldItemTmp.GetID() == Item[nIdx].GetID() );

	if (bIsSameItem)
	{
		//使用过后的重量
		int currentItemWeight = Item[nIdx].GetItemWeight();
		
		//改变负重
		int nTakeIndex = FindSame(nIdx);
		if(nTakeIndex > 0)
		{
			int nPlace = m_Items[nTakeIndex].nPlace;
			switch(nPlace)
			{
			case pos_equiproom:
				m_Room[room_equipment].AddWeight(currentItemWeight - originalItemWeight);
				break;
			default:
				break;
			}
		}
		
	}//endif
	
	if( oldItemTmp.GetCDTime( ) != -1 &&
		oldItemTmp.GetGroup( ) != -1 && 
		bUseItemOk)
		AddGroupCoolDown( oldItemTmp.GetGroup( ), oldItemTmp.GetCDTime( ) );

	KPlayer& player = Player[GetPlayerIndex()];

	//统计：使用物品
	ItemTemplateId templateId;
	oldItemTmp.GetItemTemplateId(templateId);
	player.GetPlayerStatistic().AddItem(templateId, 1, item_count_type_use);

	//日志：使用物品
	bool needLog = (oldItemTmp.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
	if (needLog)
	{
		LogEventParam useItemEvent;
		useItemEvent.event = log_event_use_item;
		useItemEvent.param1 = player.GetGUID();
		useItemEvent.param2 = oldItemTmp.GetGUID();	
		oldItemTmp.GetItemTemplateId(useItemEvent.param3.data, sizeof(useItemEvent.param3.data) - 1);
		g_pLogSystem->Log(useItemEvent);
	}

	if (g_PlayerMonitor.IsNeedRecord(player.GetPlayerIndex(), player_action_use_item))
	{
		RecordPlayerActionParam param;
		param.PlayerIndex = player.GetPlayerIndex();
		param.Action = player_action_use_item;
		snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_USE_ITEM, oldItemTmp.GetName());
		g_PlayerMonitor.RecordPlayerAction(param);
	}
	
	//如果是叠加的物品使用完了就将该物品Remove,否则就只减少其叠加的数量
	if (bIsSameItem && !Item[nIdx].CanntDisappear() )
	{
		if ( Item[nIdx].GetItemCount() )
		{
			OnBagChanged();		
			Item[nIdx].SyncItemRefresh( Player[m_PlayerIdx].m_nNetConnectIdx );
		}
		else
		{	
			if ( Item[nIdx].NeedIBUse() )
			{
				IBUse_Param ibuse_param;
				memset( &ibuse_param, 0, sizeof(ibuse_param) );
				
				ibuse_param.nItemGenre		= Item[nIdx].GetGenre();
				ibuse_param.nItemDetail		= Item[nIdx].GetDetailType();
				ibuse_param.nItemParticular = Item[nIdx].GetParticular();
				ibuse_param.IBGuid			= Item[nIdx].GetIBGuid();

				player.UseIBItem( ibuse_param );
			}

			if ( Item[nIdx].GetGenre() == item_ib )
			{
				KIBLog::getSingleton().DelIBItem( use_delete, Item[nIdx].GetGUID(), GetPlayerIndex() );
			}

			Remove(nIdx, 0, true, true, true, false);
			ItemSet.Remove(nIdx);
		}
	}
#endif
	
	if (Npc[Player[m_PlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		Npc[Player[m_PlayerIdx].m_nIndex].SendCommand(do_stand);
	}
	return TRUE;
}

#ifndef _SERVER
int KItemList::UseItem(int nIdx, int nTargetIdx )
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	
	int indexOfList = FindSame(nIdx);
	if (0 == indexOfList)
	{
		return 0;
	}
	
	int		nRet = 0;

	if ( Item[nIdx].IsMapArea(m_PlayerIdx) )
	{
		return FALSE;
	}

	switch(Item[nIdx].GetGenre())
	{
	case item_equip:
		nRet = REQUEST_EQUIP_ITEM;
		break;
	case item_target:
	case item_medicine:
	case item_horse:
	case item_ib:
		if (EatMecidine(nIdx, nTargetIdx))
			nRet = REQUEST_EAT_MEDICINE;
		break;
	default:
		break;
	}
	return nRet;
}
#endif

//#ifndef _SERVER
BOOL KItemList::SearchPosition(ItemPos* pPos, PEXTRAINFOPLUS pExtraInfo)
{
	if (NULL == pPos)
	{
		return FALSE;
	}
	
	POINT	pPt;
	
	if (m_Room[room_equipment].FindRoom(&pPt, pExtraInfo))
	{
		pPos->nPlace = pos_equiproom;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
		return TRUE;
	}
	return FALSE;
}

int	KItemList::SearchID(int nID)
{
	if (m_PlayerIdx <= 0)
		return 0;
	int nIdx = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;
		if (Item[m_Items[nIdx].nIdx].GetID() == (DWORD)nID)
			return m_Items[nIdx].nIdx;
	}
	return 0;
}

int	KItemList::GetItemIdx(int nID)
{
	if (m_PlayerIdx <= 0)
	{
		return 0;
	}

	int nIdx = 0;
	
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;

		if (Item[m_Items[nIdx].nIdx].GetID() == (DWORD)nID)
			return nIdx;
	}
	
	return 0;	
}

void KItemList::ExchangeMoney(int pos1, int pos2, int nMoney)
{
	if (pos1 < 0 || pos2 < 0 || pos1 > room_trade || pos2 > room_trade)
		return;
 
	//money一定要为正，xiehong 2007-9-20
	if(nMoney <= 0 || nMoney > m_Room[pos1].GetMoney())
	{
		return;
	}

	if (m_Room[pos1].AddMoney(-nMoney))		// 源位置能拿出这么多钱来
	{
		if (!m_Room[pos2].AddMoney(nMoney))	// 目的地能放不下去
		{
			m_Room[pos1].AddMoney(nMoney);	// 还原源位置的钱
			return;
		}
	}
	else
	{
		return;
	}
	
#ifndef _SERVER
	if (pos1 == room_equipment && pos2 == room_repository)
		SendClientCmdStoreMoney(0, nMoney);
	else if (pos1 == room_repository && pos2 == room_equipment)
		SendClientCmdStoreMoney(1, nMoney);
#endif
#ifdef _SERVER
	SendMoneySync();
#endif	
}

//----------------------------------------------------------------------
//	功能：得到物品栏和储物箱的总钱数
//----------------------------------------------------------------------
int KItemList::GetMoneyAmount()
{
	return (m_Room[room_equipment].GetMoney() + m_Room[room_repository].GetMoney());
}

//----------------------------------------------------------------------
//	功能：得到包裹里的的钱数
//----------------------------------------------------------------------
int KItemList::GetEquipmentMoney()
{
	return m_Room[room_equipment].GetMoney();
}

BOOL KItemList::AddMoney(int nRoom, int nMoney)
{
	if (nRoom < 0 || nRoom >= room_num)
		return FALSE;
	
	if ( !m_Room[nRoom].AddMoney(nMoney) )
		return FALSE;
	
#ifdef _SERVER
	SendMoneySync();
#endif
	
	return TRUE;
}

// BOOL KItemList::CostMoney(int nMoney)
// {
// 	if (nMoney > GetEquipmentMoney())
// 		return FALSE;
// 	
// 	if ( !m_Room[room_equipment].AddMoney(-nMoney) )
// 		return FALSE;
// 	
// #ifdef _SERVER
// 	SendMoneySync();
// #endif
// 	
// 	return TRUE;
// }

// BOOL KItemList::DecMoney(int nMoney)
// {
// 	if (nMoney < 0)
// 		return FALSE;
// 	
// 	if (nMoney > m_Room[room_equipment].GetMoney())
// 	{
// 		nMoney -= m_Room[room_equipment].GetMoney();
// 		SetRoomMoney(room_equipment, 0);
// 		if (nMoney > m_Room[room_repository].GetMoney())
// 			SetRoomMoney(room_repository, 0);
// 		else
// 			AddMoney(room_repository, -nMoney);
// 	}
// 	else
// 	{
// 		AddMoney(room_equipment, -nMoney);
// 	}
// 	
// #ifdef _SERVER
// 	SendMoneySync();
// #endif
// 	
// 	return TRUE;
// }

#ifdef _SERVER
//----------------------------------------------------------------------------------
//	功能：调用此接口必须保证传入的nMoney是一个有效数(正数且不超过所有钱数)(等待删除)
//----------------------------------------------------------------------------------
void	KItemList::TradeMoveMoney(int nMoney)
{
	//自己钱的处理
	m_Room[room_trade].SetMoney(nMoney);
//	SendMoneySync();自己的在客户端限制,不同步了,只要服务器端校验逻辑正确即可
	
	// 给对方发消息
	TRADE_MONEY_SYNC	sMoney;
	sMoney.ProtocolType = s2c_trademoneysync;
	sMoney.m_nMoney = nMoney;
	int oppositePlayerIndex = Player[m_PlayerIdx].m_cTrade.d_oppositePlayerIndex;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[oppositePlayerIndex].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(TRADE_MONEY_SYNC));
}

#endif

#ifdef _SERVER
//----------------------------------------------------------------------------------
//	功能：服务器发money同步信息给客户端
//----------------------------------------------------------------------------------
void	KItemList::SendMoneySync()
{
	PLAYER_MONEY_SYNC	sMoney;
	sMoney.ProtocolType = s2c_syncmoney;
	sMoney.m_nMoney1 = m_Room[room_equipment].GetMoney();
	sMoney.m_nMoney2 = m_Room[room_repository].GetMoney();
	sMoney.m_nMoney3 = m_Room[room_trade].GetMoney();
	
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(PLAYER_MONEY_SYNC));
}
#endif

void KItemList::SetMoney(int nMoney1, int nMoney2, int nMoney3)
{
	m_Room[room_equipment].SetMoney(nMoney1);
	m_Room[room_repository].SetMoney(nMoney2);
	m_Room[room_trade].SetMoney(nMoney3);
	
#ifndef _SERVER
	KObjAtContRegion	sMoney;
	sMoney.Obj.uGenre = CGOG_MONEY;
	sMoney.Obj.uId = nMoney1;
	sMoney.eContainer = UOC_ITEM_TAKE_WITH;
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sMoney, 1);
	sMoney.Obj.uId = nMoney2;
	sMoney.eContainer = UOC_STORE_BOX;
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sMoney, 1);
	sMoney.Obj.uId = nMoney3;
	sMoney.eContainer = UOC_TO_BE_TRADE;
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sMoney, 1);
#endif
}

void KItemList::SetRoomMoney(int nRoom, int nMoney)
{
	if (nRoom >= 0 && nRoom < room_num)
		m_Room[nRoom].SetMoney(nMoney);
}

void KItemList::exchangeItem(const ItemPos* sourPos, const ItemPos* destPos)
{
	//源宿端位置相同，不需要处理
	if(sourPos->nX == destPos->nX && sourPos->nY == destPos->nY && sourPos->nPlace == destPos->nPlace)
		return;
	
	if(exchangePrecheck(sourPos, destPos) == false)
		return;

	switch(sourPos->nPlace)
	{
	case pos_equiproom:
		{
			if(pos_equiproom == destPos->nPlace)			//物品栏到物品栏
			{
				if(doExchangeItem(room_equipment, sourPos, room_equipment, destPos) == false)
				{
					return;
				}
			}
			else if(pos_traderoom == destPos->nPlace)		//物品栏到交易栏
			{
				if(doExchangeItem(room_equipment, sourPos, room_trade, destPos) == false)
					return;
			}
			else if(pos_equip == destPos->nPlace)			//穿装
			{
				if ( Equip(sourPos, (ITEM_PART)destPos->nX) == false )
				{
					return;
				}
			}
			else if(pos_repositoryroom == destPos->nPlace)	//物品栏到储物箱
			{
				if ( doExchangeItem(room_equipment, sourPos, room_repository, destPos) == false )
					return;
			}
			else if(pos_itembox_extend == destPos->nPlace)	//物品栏到物品扩展栏
			{
				if(doExchangeItem(room_equipment, sourPos, room_itembox_extend, destPos) == false)
					return;
			}
			else if(pos_store_extend == destPos->nPlace)
			{
				if (doExchangeItem(room_equipment,sourPos,room_store_extend,destPos) == false)
					return;
			}
			else
			{
				return;
			}
		}
		break;
	case pos_traderoom:
		{
			if(pos_equiproom == destPos->nPlace)
			{
				if(doExchangeItem(room_trade, sourPos, room_equipment, destPos) == false)
					return;
			}
			else if(pos_traderoom == destPos->nPlace)
			{
				if(doExchangeItem(room_trade, sourPos, room_trade, destPos) == false)
					return;
			}
			else
			{
				return;
			}
		}
		break;
	case pos_equip:
		{
			if(pos_equiproom == destPos->nPlace)
			{
				if ( unEquip((ITEM_PART)sourPos->nX, destPos) == false )
				{
					return;
				}
			}
			else
			{
				return;
			}
		}
		break;
	case pos_repositoryroom:
		{
			if(pos_equiproom == destPos->nPlace)
			{
				if ( doExchangeItem(room_repository, sourPos, room_equipment, destPos) == false )
				{
					return;
				}
			}
			else if(pos_repositoryroom == destPos->nPlace)
			{
				if ( doExchangeItem(room_repository, sourPos, room_repository, destPos) == false )
				{
					return;
				}
			}
			else if(pos_store_extend == destPos->nPlace)
			{
				if ( doExchangeItem(room_repository, sourPos, room_store_extend, destPos)  == false )
				{
					return;
				}
			}
			else
			{
				return;
			}
		}
		break;
	case pos_itembox_extend:
		{
			if(pos_equiproom == destPos->nPlace)
			{
				if(doExchangeItem(room_itembox_extend, sourPos, room_equipment , destPos) == false)
					return;
			}
			else
			{
				return;
			}
		}
		break;
	case pos_store_extend:
		{
			if(pos_repositoryroom == destPos->nPlace)
			{
				if(doExchangeItem(room_store_extend, sourPos, room_repository , destPos) == false)
					return;
			}
			else
			{
				return;
			}
		}
		break;
	default:
		return;
	}

	onExchangeDone(sourPos, destPos);
}

bool KItemList::exchangePrecheck(const ItemPos* sourPos, const ItemPos* destPos)
{
	if(m_PlayerIdx <= 0)
		return false;

	//检查源端容器几个参数是否合法(非装备栏，装备在下面判断)
	if(sourPos->nPlace != 0)
	{
		INVENTORY_ROOM sourRoom = corePos2coreRoom((ITEM_POSITION)sourPos->nPlace);
		if(room_num == sourRoom)
		{
			return false;
		}
		int sourItemIndex = m_Room[sourRoom].FindItem(sourPos->nX, sourPos->nY);
		if(sourItemIndex <= 0)
		{
			return false;
		}//endif

		ItemPos       pos;
		if ( FALSE == GetItemPos(sourItemIndex,&pos) || ( pos.nPlace != sourPos->nPlace || pos.nX != sourPos->nX || pos.nY != sourPos->nY))
			return false;
	}

	//检查宿端容器几个参数是否合法(非装备栏，装备在下面判断)
	if(destPos->nPlace != 0)
	{
		INVENTORY_ROOM destRoom = corePos2coreRoom((ITEM_POSITION)destPos->nPlace);
		if(room_num == destRoom)
		{
			return false;
		}//endif
		int destItemIndex = m_Room[destRoom].FindItem(destPos->nX, destPos->nY);
		if(destItemIndex == -1)
		{
			return false;
		}//endif

		if (destItemIndex != 0)
		{
			ItemPos       pos;

			if ( FALSE == GetItemPos(destItemIndex,&pos) || ( pos.nPlace != destPos->nPlace || pos.nX != destPos->nX || pos.nY != destPos->nY))
				return    false;

		}//endif

	}

	//交易相关……begin
	//当交换发生在物品栏和交易栏之间时，双方必须处于交易状态
#ifdef _SERVER
	if(pos_traderoom == sourPos->nPlace || pos_traderoom == destPos->nPlace)
	{
		if(Player[m_PlayerIdx].m_cTrade.tradeBoxCanMove() == false)
			return false;
	}
#endif
	//如果是绑定物品则不允许把它放在交易栏上
	if(pos_traderoom == destPos->nPlace)
	{
		INVENTORY_ROOM room = corePos2coreRoom((ITEM_POSITION)sourPos->nPlace);
		if (room == room_num)
			return false;
		
		int itemIndex = m_Room[room].FindItem(sourPos->nX, sourPos->nY);
		if(itemIndex <= 0)
			return false;

		if(Item[itemIndex].IsLocked(Player[m_PlayerIdx].GetNetConnectIdx()) || !Item[itemIndex].CanExchange() || Item[itemIndex].IsBind())
			return false;
	}

	if(pos_traderoom == sourPos->nPlace)
	{
		INVENTORY_ROOM room = corePos2coreRoom((ITEM_POSITION)destPos->nPlace);
		if (room == room_num)
			return false;
		
		int itemIndex = m_Room[room].FindItem(destPos->nX, destPos->nY);
		//目标位置如果有物品，则必须是可以交易的
		if(itemIndex > 0)
		{
			if(Item[itemIndex].IsLocked(Player[m_PlayerIdx].GetNetConnectIdx()) || !Item[itemIndex].CanExchange() || Item[itemIndex].IsBind())
				return false;
		}
	}
	//交易相关……end

	//仓库相关
#ifdef _SERVER
	if (pos_repositoryroom == destPos->nPlace || pos_store_extend == destPos->nPlace || pos_repositoryroom == sourPos->nPlace || pos_store_extend == sourPos->nPlace )
	{
		if (!IsValidPlayer(m_PlayerIdx))
			return false;
		
		if (Player[m_PlayerIdx].GetUIServerState().GetUIState(player_ui_repository) != player_ui_state_open)
			return false;
	}//endif
#endif
	
	if(pos_repositoryroom == destPos->nPlace || pos_store_extend == destPos->nPlace )
	{
		INVENTORY_ROOM room = corePos2coreRoom((ITEM_POSITION)sourPos->nPlace);
		if (room == room_num)
			return false;
		
		int itemIndex = m_Room[room].FindItem(sourPos->nX, sourPos->nY);
		if(itemIndex <= 0)
			return false;

		if(Item[itemIndex].IsLocked( Player[m_PlayerIdx].GetNetConnectIdx(), false ))
			return false;
	}

	//包裹扩展槽位相关……begin
	//当交换发生在物品栏和扩展槽之间时，可能会发生包裹大小的减少，此时应该检查所要删除的位置（最后几个）上是否有物品
	int extendSize = 0;
	if(pos_equiproom == sourPos->nPlace && pos_itembox_extend == destPos->nPlace)
	{
		extendSize += bagExtend(m_Room[room_equipment].FindItem(sourPos->nX, sourPos->nY), room_equipment);
		//放入包裹扩展槽位的必须是个包裹类的物品（也就是说可以产生新格子的物品）
		if(extendSize <=0)//此处0可能是非包裹类物品或者位置上无东西，这种情况交换必须终止
			return false;
		extendSize -= bagExtend(m_Room[room_itembox_extend].FindItem(destPos->nX, destPos->nY), room_equipment);
		//如果发生交换后的格子比以前少，就必须判断少去的几个格子（从最后减）上是否有物品
		if(extendSize < 0 && !m_Room[room_equipment].isTailEmpty(-extendSize))
			return false;
	}
	if(pos_itembox_extend == sourPos->nPlace && pos_equiproom == destPos->nPlace)
	{
		extendSize -= bagExtend(m_Room[room_itembox_extend].FindItem(sourPos->nX, sourPos->nY), room_equipment);
		int destIndex = m_Room[room_equipment].FindItem(destPos->nX, destPos->nY);
		int destExtendSize = bagExtend(destIndex, room_equipment);
		if(destExtendSize <= 0 && destIndex > 0)//当要交换到扩展槽的物品不是一个包裹类物品时
			return false;
		extendSize += destExtendSize;
		
		//如果发生交换后的格子比以前少，就必须判断少去的几个格子（从最后减）上是否有物品
		//同时，将被交换到物品栏中的物品不能被放在减少的几个格子内
		if(extendSize < 0)
		{
			if(!m_Room[room_equipment].isTailEmpty(-extendSize))
				return false;
			
			if(destPos->nY * m_Room[room_equipment].getWidth() + destPos->nX 
				>= m_Room[room_equipment].getValidSize() + extendSize)
				return false;
		}
	}

	//如果交换发生在储物箱和储物箱扩展栏
	if(pos_repositoryroom == sourPos->nPlace && pos_store_extend == destPos->nPlace)
	{
		extendSize += bagExtend(m_Room[room_repository].FindItem(sourPos->nX, sourPos->nY), room_repository);
		//放入包裹扩展槽位的必须是个包裹类的物品（也就是说可以产生新格子的物品）
		if(extendSize <=0)//此处0可能是非包裹类物品或者位置上无东西，这种情况交换必须终止
			return false;
		extendSize -= bagExtend(m_Room[room_store_extend].FindItem(destPos->nX, destPos->nY), room_repository);
		//如果发生交换后的格子比以前少，就必须判断少去的几个格子（从最后减）上是否有物品
		if(extendSize < 0 && !m_Room[room_repository].isTailEmpty(-extendSize))
			return false;
	}

	if(pos_store_extend == sourPos->nPlace && pos_repositoryroom == destPos->nPlace)
	{
		extendSize -= bagExtend(m_Room[room_store_extend].FindItem(sourPos->nX, sourPos->nY), room_repository);
		int destIndex = m_Room[room_repository].FindItem(destPos->nX, destPos->nY);
		int destExtendSize = bagExtend(destIndex, room_repository);
		if(destExtendSize <= 0 && destIndex > 0)//当要交换到扩展槽的物品不是一个包裹类物品时
			return false;
		extendSize += destExtendSize;
		
		//如果发生交换后的格子比以前少，就必须判断少去的几个格子（从最后减）上是否有物品
		//同时，将被交换到物品栏中的物品不能被放在减少的几个格子内
		if(extendSize < 0)
		{
			if(!m_Room[room_repository].isTailEmpty(-extendSize))
				return false;
			
			if(destPos->nY * m_Room[room_repository].getWidth() + destPos->nX 
				>= m_Room[room_repository].getValidSize() + extendSize)
				return false;
		}
	}

	//交叉的情况：物品栏往储物箱扩展栏
	if (pos_equiproom == sourPos->nPlace && pos_store_extend == destPos->nPlace)
	{
		extendSize += bagExtend(m_Room[room_equipment].FindItem(sourPos->nX,sourPos->nY),room_repository);
		if (extendSize <= 0)
			return false;
		
		extendSize -= bagExtend(m_Room[room_store_extend].FindItem(destPos->nX, destPos->nY), room_repository);
		//如果发生交换后的格子比以前少，就必须判断少去的几个格子（从最后减）上是否有物品
		if(extendSize < 0 && !m_Room[room_repository].isTailEmpty(-extendSize))
			return false;
	}//endif

	//包裹扩展槽位相关……end
	
	//穿装脱装相关……begin
	if(pos_equiproom == sourPos->nPlace && pos_equip == destPos->nPlace)
	{
		//得到要装备的物品的物品索引并检查是否有效
		INVENTORY_ROOM room = corePos2coreRoom((ITEM_POSITION)sourPos->nPlace);
		if(room_num == room)
			return false;
		int equipItemIndex = m_Room[room].FindItem(sourPos->nX, sourPos->nY);
		if (m_PlayerIdx <= 0 || equipItemIndex <= 0)
			return false;

		//itemPos对应的必须是个装备
		if(item_equip != Item[equipItemIndex].GetGenre())
			return false;
		
		//如果未指定装备位置，则自动判断该装备在哪个位置
		ITEM_PART equipPos = (ITEM_PART)destPos->nX;
		if (itempart_unidentified == equipPos)
		{
			equipPos = GetEquipPlace((EQUIPDETAILTYPE)Item[equipItemIndex].GetDetailType());
		}
		//判断该物品是否能够装备上
		if (!canEquip(equipItemIndex, equipPos))
		{
			return false;
		}
	}

	if(pos_equip == sourPos->nPlace && pos_equiproom == destPos->nPlace)
	{
		//检查装备位置索引是否有效
		ITEM_PART equipPos = (ITEM_PART)sourPos->nX;
		if(equipPos < 0 || equipPos > itempart_num)
			return false;

		//得到装备的物品索引并检查索引是否有效
		int equipIndex = m_EquipItem[equipPos].nEquipIdx;
		if(equipIndex < 1)
			return false;
		
		//检查目的位置是否已经存在物品
		//FindItem在位置信息不正确的时候返回－1，在找到物品时返回>0的数字，只有在位置正确但没有物品时返回0
		INVENTORY_ROOM unEquipRoom = corePos2coreRoom((ITEM_POSITION)destPos->nPlace);
		if(room_num == unEquipRoom)
			return false;
		int itemIndex = m_Room[unEquipRoom].FindItem(destPos->nX, destPos->nY);
		//如果卸装位置上已经有物品则可以看成把该物品装备到身上，走装备流程
		if(itemIndex > 0)
		{
			//判断该物品是否能够装备上
			if (!canEquip(itemIndex, equipPos))
			{
				return false;
			}
		}
		else if(itemIndex == -1)
		{	//位置不正确
			return false;
		}
		
	}
	//穿装脱装相关……end

	return true;
}

void KItemList::onExchangeDone(const ItemPos* sourPos, const ItemPos* destPos)
{
#ifdef _SERVER
	//交易相关……begin
	//当交换发生在交易栏和其他容器之间的时候，一定是在和其他玩家进行交易，
	//如果交易成功，则进入该函数，在此把本方交易栏上的物品信息同步到对方客户端的对方交易栏（也就是对方看到的本方物品）中
	if(pos_traderoom == sourPos->nPlace)
		syncOppositeItem(sourPos);

	if(pos_traderoom == destPos->nPlace)
		syncOppositeItem(destPos);

	if(pos_traderoom == sourPos->nPlace || pos_traderoom == destPos->nPlace)
		Player[m_PlayerIdx].tradeServerSendUnlockToOpposite();
	//交易相关……end
#endif
	
	//包裹扩展栏相关……begin
	int extendSize = 0;
	//当交换发生在物品栏和物品栏扩展之间时，更改包裹大小并通知客户端
	if(pos_equiproom == sourPos->nPlace && pos_itembox_extend == destPos->nPlace)
	{
		extendSize += bagExtend(m_Room[room_itembox_extend].FindItem(destPos->nX, destPos->nY), room_equipment);
		extendSize -= bagExtend(m_Room[room_equipment].FindItem(sourPos->nX, sourPos->nY), room_equipment);
		m_Room[room_equipment].addSize(extendSize);
		onBagSized(room_equipment);
	}
	if(pos_itembox_extend == sourPos->nPlace && pos_equiproom == destPos->nPlace)
	{
		extendSize -= bagExtend(m_Room[room_equipment].FindItem(destPos->nX, destPos->nY), room_equipment);
		extendSize += bagExtend(m_Room[room_itembox_extend].FindItem(sourPos->nX, sourPos->nY), room_equipment);
		m_Room[room_equipment].addSize(extendSize);
		onBagSized(room_equipment);
	}
	//当交换发生在储物箱和储物箱扩展之间时，更改包裹大小并通知客户端
	if(pos_repositoryroom == sourPos->nPlace && pos_store_extend == destPos->nPlace)
	{
		extendSize += bagExtend(m_Room[room_store_extend].FindItem(destPos->nX, destPos->nY), room_repository);
		extendSize -= bagExtend(m_Room[room_repository].FindItem(sourPos->nX, sourPos->nY), room_repository);
		m_Room[room_repository].addSize(extendSize);
		onBagSized(room_repository);
	}
	if(pos_store_extend == sourPos->nPlace && pos_repositoryroom == destPos->nPlace)
	{
		extendSize -= bagExtend(m_Room[room_repository].FindItem(destPos->nX, destPos->nY), room_repository);
		extendSize += bagExtend(m_Room[room_store_extend].FindItem(sourPos->nX, sourPos->nY), room_repository);
		m_Room[room_repository].addSize(extendSize);
		onBagSized(room_repository);
	}
	//包裹扩展栏相关……end

	//交叉情况：物品栏到仓库扩展栏
	if(pos_equiproom == sourPos->nPlace && pos_store_extend == destPos->nPlace)
	{
		extendSize += bagExtend(m_Room[room_store_extend].FindItem(destPos->nX, destPos->nY), room_repository);
		extendSize -= bagExtend(m_Room[room_equipment].FindItem(sourPos->nX, sourPos->nY), room_repository);
		m_Room[room_repository].addSize(extendSize);
		onBagSized(room_repository);
	}//endif
}

bool KItemList::doExchangeItem(INVENTORY_ROOM sourBox, const ItemPos* sourPos, INVENTORY_ROOM destBox, const ItemPos* destPos)
{
	int sourIndex = m_Room[sourBox].FindItem(sourPos->nX, sourPos->nY);
	int destIndex = m_Room[destBox].FindItem(destPos->nX, destPos->nY);
	//源端必须有个东西
	if(sourIndex <= 0)
		return false;

	//宿端可以没有东西，此时把远端的东西直接放过去即可
	if(destIndex <= 0)
	{
		if (!m_Room[sourBox].PickUpItem(sourIndex, sourPos->nX, sourPos->nY))
			return false;
		if (!m_Room[destBox].PlaceItem(destPos->nX, destPos->nY, sourIndex, m_PlayerIdx))
		{
			// 放不到目的地，就要把东西放回去
			m_Room[sourBox].PlaceItem(sourPos->nX, sourPos->nY, sourIndex, m_PlayerIdx);
			return false;
		}
		int sourListIndex = FindSame(sourIndex);
		if (sourListIndex <= 0)
			return false;
		m_Items[sourListIndex].nPlace = destPos->nPlace;
		m_Items[sourListIndex].nX = destPos->nX;
		m_Items[sourListIndex].nY = destPos->nY;
	}
	else
	{
		//如果物品类型相同，改变物品的个数与负重即可
		if(Item[sourIndex].CanCombine(Item[destIndex]))
		{
			int totalCount = Item[sourIndex].GetItemCount() + Item[destIndex].GetItemCount();
			if(Item[destIndex].GetMaxItemCount() < 2)
			{
				return false;
			}
			else if(totalCount <= Item[destIndex].GetMaxItemCount())
			{
				int originalDestWeight = Item[destIndex].GetItemWeight();
				
				//加起来还没有最大个数多，则必须清空源端的格子
				Item[destIndex].SetItemCount(totalCount);
				Remove(sourIndex, 0, false);
				ItemSet.Remove(sourIndex);

				m_Room[destBox].AddWeight(Item[destIndex].GetItemWeight() - originalDestWeight);
			}
			else if(Item[destIndex].GetItemCount() == Item[destIndex].GetMaxItemCount())
			{
				int originalSourWeight = Item[sourIndex].GetItemWeight();
				int originalDestWeight = Item[destIndex].GetItemWeight();

				//当加起来的数量大于最大叠加数时，目标端的叠加数和最大叠加数相等，需要制造一个看起来像交换位置的现象
				Item[sourIndex].SetItemCount(Item[destIndex].GetMaxItemCount());
				Item[destIndex].SetItemCount(totalCount - Item[destIndex].GetMaxItemCount());

				m_Room[sourBox].AddWeight(Item[sourIndex].GetItemWeight() - originalSourWeight);
				m_Room[destBox].AddWeight(Item[destIndex].GetItemWeight() - originalDestWeight);
			}
			else
			{
				int originalSourWeight = Item[sourIndex].GetItemWeight();
				int originalDestWeight = Item[destIndex].GetItemWeight();

				//当加起来的数量大于最大叠加数时，目标端的叠加数小于最大叠加数
				Item[destIndex].SetItemCount(Item[destIndex].GetMaxItemCount());
				Item[sourIndex].SetItemCount(totalCount - Item[destIndex].GetMaxItemCount());

				m_Room[sourBox].AddWeight(Item[sourIndex].GetItemWeight() - originalSourWeight);
				m_Room[destBox].AddWeight(Item[destIndex].GetItemWeight() - originalDestWeight);
			}
		}
		else
		{
			//先把原来位置的物品清掉
			if (!m_Room[sourBox].PickUpItem(sourIndex, sourPos->nX, sourPos->nY))
				return false;
			if (!m_Room[destBox].PickUpItem(destIndex, destPos->nX, destPos->nY))
				return false;
			
			//宿端索引改为源端索引
			if (!m_Room[destBox].PlaceItem(destPos->nX, destPos->nY, sourIndex, m_PlayerIdx))
			{
				// 放不到目的地，就要把东西放回去
				m_Room[sourBox].PlaceItem(sourPos->nX, sourPos->nY, sourIndex, m_PlayerIdx);
				m_Room[destBox].PlaceItem(destPos->nX, destPos->nY, destIndex, m_PlayerIdx);
				return false;
			}
			//把源端索引改为宿端索引
			if (!m_Room[sourBox].PlaceItem(sourPos->nX, sourPos->nY, destIndex, m_PlayerIdx))
			{
				// 放不到目的地，就要把东西放回去
				m_Room[destBox].PlaceItem(destPos->nX, destPos->nY, destIndex, m_PlayerIdx);
				return false;
			}
			//交换m_itemList
			int sourListIndex = FindSame(sourIndex);
			int destListIndex = FindSame(destIndex);
			if (sourListIndex <= 0 || destListIndex <= 0)
				return false;
			m_Items[sourListIndex].nPlace = destPos->nPlace;
			m_Items[sourListIndex].nX = destPos->nX;
			m_Items[sourListIndex].nY = destPos->nY;
			m_Items[destListIndex].nPlace = sourPos->nPlace;
			m_Items[destListIndex].nX = sourPos->nX;
			m_Items[destListIndex].nY = sourPos->nY;
		}
	}

#ifdef _SERVER
	PLAYER_MOVE_ITEM_SYNC moveItem;
	moveItem.ProtocolType = s2c_playermoveitem;
	moveItem.destPlace	= destPos->nPlace;
	moveItem.destX		= destPos->nX;
	moveItem.destY		= destPos->nY;
	moveItem.sourPlace	= sourPos->nPlace;
	moveItem.sourX		= sourPos->nX;
	moveItem.sourY		= sourPos->nY;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&moveItem, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif

	//物品交换涉及到包裹的情况都需要通知包裹状态改变
	if ((room_equipment == sourBox && room_equipment != destBox)
		|| (room_equipment != sourBox && room_equipment == destBox))
	{
		bool isCharm = false;
		if (item_charm == Item[sourIndex].GetGenre())
			isCharm = true;
		if (destIndex > 0 && item_charm == Item[destIndex].GetGenre())
			isCharm = true;

		OnBagChanged(isCharm);
	}

#ifndef _SERVER
	//重新得到格子到ItemSet的索引
	sourIndex = m_Room[sourBox].FindItem(sourPos->nX, sourPos->nY);
	destIndex = m_Room[destBox].FindItem(destPos->nX, destPos->nY);
	KObjAtContRegion itemNewPos;
	itemNewPos.Region.h		= destPos->nX;
	itemNewPos.Region.v		= destPos->nY;
	itemNewPos.Obj.uGenre	= CGOG_ITEM;
	itemNewPos.Obj.uId	= destIndex;
	itemNewPos.Region.Height= Item[destIndex].GetItemCount();
	itemNewPos.nContainer	= corePos2ClientContainer((ITEM_POSITION)destPos->nPlace);
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (unsigned int)&itemNewPos, TRUE);
	itemNewPos.Region.h		= sourPos->nX;
	itemNewPos.Region.v		= sourPos->nY;
	itemNewPos.nContainer	= corePos2ClientContainer((ITEM_POSITION)sourPos->nPlace);
	if(sourIndex > 0)
	{
		itemNewPos.Obj.uGenre	= CGOG_ITEM; 
		itemNewPos.Obj.uId	= sourIndex;
		itemNewPos.Region.Height= Item[sourIndex].GetItemCount();
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (unsigned int)&itemNewPos, TRUE);
	}
	else
	{
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (unsigned int)&itemNewPos, FALSE);
	}
#endif
	//都运行到这里，恭喜你，交换成功啦！
	return true;
}

#ifdef _SERVER
bool KItemList::syncOppositeItem(const ItemPos* itemPos)
{
	int destPlayerIndex = Player[m_PlayerIdx].m_cTrade.d_oppositePlayerIndex;
	if(destPlayerIndex <= 0 )
		return false;
	
	int itemIndex = m_Room[room_trade].FindItem(itemPos->nX, itemPos->nY);
	if(itemIndex > 0)
	{
		ItemPos itPos;
		memset( &itPos, 0, sizeof(itPos) );
		itPos.nPlace = pos_trade1;
		itPos.nX = itemPos->nX;
		itPos.nY = itemPos->nY;
		Item[itemIndex].SyncItem( Player[destPlayerIndex].m_nNetConnectIdx, itPos, item_sync_type_normal );
	}
	else
	{
		char sendBuff[ITEM_SYNC_BUFF_LENGTH];
		memset(sendBuff, 0, sizeof(sendBuff));
		ITEM_SYNC* pItemSync = (ITEM_SYNC*)sendBuff;
		
		pItemSync->Protocol	= s2c_syncitem;
		pItemSync->Length = sizeof(ITEM_SYNC) - 1;
		pItemSync->m_ID = 0;
		pItemSync->m_btPlace = pos_trade1;
		pItemSync->m_btX = itemPos->nX;
		pItemSync->m_btY = itemPos->nY;
		
		if (CompressProtocol((BYTE*)sendBuff, pItemSync->Length + 1, sizeof(sendBuff)))
		{	
			if (g_pServer != NULL)
				g_pServer->PackDataToClient( Player[destPlayerIndex].m_nNetConnectIdx, (BYTE*)sendBuff, pItemSync->Length + 1 );
		}
	}

	return true;
}

bool KItemList::syncSelfItem(ITEM_POSITION place, int x, int y)
{
	//取得物品在itemset中的索引，装备栏做了特殊处理
	int itemIndex = 0;
	if(pos_equip == place)
		itemIndex = m_EquipItem[x].nEquipIdx;
	else
		itemIndex = m_Room[corePos2coreRoom(place)].FindItem(x, y);

	if(itemIndex > 0)
	{
		ItemPos itPos;
		memset( &itPos, 0, sizeof(itPos) );
		itPos.nPlace = place;
		itPos.nX = x;
		itPos.nY = y;
		Item[itemIndex].SyncItem( Player[m_PlayerIdx].m_nNetConnectIdx, itPos, item_sync_type_normal );
	}
	else
	{
		char sendBuff[ITEM_SYNC_BUFF_LENGTH];
		memset(sendBuff, 0, sizeof(sendBuff));
		ITEM_SYNC* pItemSync = (ITEM_SYNC*)sendBuff;
		
		pItemSync->Protocol	= s2c_syncitem;
		pItemSync->Length = sizeof(ITEM_SYNC) - 1;
		pItemSync->m_ID = 0;
		pItemSync->m_btPlace = place;
		pItemSync->m_btX = x;
		pItemSync->m_btY = y;
		
		if (CompressProtocol((BYTE*)sendBuff, pItemSync->Length + 1, sizeof(sendBuff)))
		{	
			if (g_pServer != NULL)
				g_pServer->PackDataToClient( Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)sendBuff, pItemSync->Length + 1 );
		}
	}

	return true;
}

bool KItemList::syncItemToOther(int playerIndex, int itemIndex)
{
	if(itemIndex <= 0 || itemIndex >= MAX_ITEM)
	{
		return false;
	}

	if(playerIndex <= 0 || playerIndex >= MAX_PLAYER)
	{
		return false;
	}

	if(NULL == g_pServer)
	{
		return false;
	}

	ItemPos itPos;
	memset( &itPos, 0, sizeof(itPos) );
	itPos.nPlace = pos_pet_feed_box;//使用pos_pet_feed_box来表示其他玩家同步过来的消息
	itPos.nX = -1;
	itPos.nY = -1;
	Item[itemIndex].SyncItem( Player[playerIndex].m_nNetConnectIdx, itPos, item_sync_type_normal );
	
	return true;
}
#endif

#ifdef _SERVER

void KItemList::splitItem(ItemPos* sourPos, ItemPos* destPos, int splitItemCount)
{
	if(sourPos->nX == destPos->nX && sourPos->nY == destPos->nY)
		return;
	switch(sourPos->nPlace)
	{
	case pos_equiproom:
		{
			if(pos_equiproom == destPos->nPlace)
			{
				doSplitItem(room_equipment, sourPos, room_equipment, destPos,  splitItemCount);
			}
			else if(pos_traderoom == destPos->nPlace)
			{
				return;//暂时不让把拆分宿端设为交易栏
				//doSplitItem(room_equipment, sourPos, room_trade, destPos,  splitItemCount);
			}
		}
		break;
	}
}

void KItemList::doSplitItem(INVENTORY_ROOM sourBox,  ItemPos* sourPos, 
						  INVENTORY_ROOM destBox, ItemPos* destPos, 
						  int moveItemCount)
{
	int sourIndex = m_Room[sourBox].FindItem(sourPos->nX, sourPos->nY);
	int destIndex = m_Room[destBox].FindItem(destPos->nX, destPos->nY);
	//源端必须有个东西
	if(sourIndex <= 0)
		return;

	if ( Item[sourIndex].IsIBCountTimelimitItem() )
	{
		return;
	}

	if ( destIndex > 0 && Item[destIndex].IsIBCountTimelimitItem() )
	{
		return;
	}

	if (Item[sourIndex].IsLockedByDate(Player[m_PlayerIdx].m_nNetConnectIdx))
	{
		return;
	}

	//检查拆分个数是否合理
	if(moveItemCount >= Item[sourIndex].GetItemCount() || moveItemCount <= 0 || Item[sourIndex].GetIBItemType() > -1 )
		return;
	
	//宿端必须没有不同类的东西
	if(destIndex > 0 && false == Item[sourIndex].IsSameParitcularItem(Item[destIndex]))
	{
		return;
	}

	//记录原来物品的重量
	int originalSourItemWeight = Item[sourIndex].GetItemWeight();
	
	//宿端有同类物品
	if(destIndex > 0)
	{		
		int originalDestItemWeight = Item[destIndex].GetItemWeight();

		//必须可以和目标物品合并
		if (!Item[sourIndex].CanCombine(Item[destIndex]))
			return;

		int totalCount = Item[sourIndex].GetItemCount() + Item[destIndex].GetItemCount();
		if(moveItemCount + Item[destIndex].GetItemCount() > Item[destIndex].GetMaxItemCount())
		{
			moveItemCount = Item[destIndex].GetMaxItemCount() - Item[destIndex].GetItemCount();
		}
		Item[sourIndex].SetItemCount(Item[sourIndex].GetItemCount() - moveItemCount);
		m_Room[sourBox].AddWeight(Item[sourIndex].GetItemWeight() - originalSourItemWeight);
		Item[destIndex].SetItemCount(Item[destIndex].GetItemCount() + moveItemCount);
		m_Room[destBox].AddWeight(Item[destIndex].GetItemWeight() - originalDestItemWeight);
	}
	else
	{
		int nFreeIndex = FindFree();
		if(!nFreeIndex)
		{
			return;
		}
		
		KItem* splitItem = &Item[sourIndex];
		int newItemIndex = ItemSet.Add(	splitItem->GetGenre(), 
										splitItem->GetDetailType(),
										splitItem->GetParticular(),
										splitItem->GetLevel(), 
										moveItemCount);
		
		if(newItemIndex <= 0)
		{
			return; 
		}

		//物品实例属性拷贝和物品个数设置
		TItemtransfersData transferData;
		Item[sourIndex].GetItemtransfersData(transferData);
		Item[newItemIndex].SetItemtransfersData(transferData);
		Item[newItemIndex].SetItemCount(moveItemCount);
		Item[newItemIndex].SetTaskGiven(Item[sourIndex].IsTaskGiven());
		Item[newItemIndex].SetFlushTimes(Item[sourIndex].GetFlushTimes());

		//重置源端物品个数和负重
		Item[sourIndex].SetItemCount(Item[sourIndex].GetItemCount() - moveItemCount);
		m_Room[sourBox].AddWeight(Item[sourIndex].GetItemWeight() - originalSourItemWeight);
		
		if (!Add(newItemIndex, coreRoom2corePos(destBox), destPos->nX, destPos->nY, NULL, item_sync_type_split))
		{
			ItemSet.Remove(newItemIndex);
			return;
		}		
	}

	OnBagChanged();
	
	Item[sourIndex].SyncItemRefresh(Player[m_PlayerIdx].m_nNetConnectIdx);
	
	if(destIndex > 0)
	{
		Item[destIndex].SyncItemRefresh(Player[m_PlayerIdx].m_nNetConnectIdx);
	}
}
#endif


INVENTORY_ROOM	KItemList::clientContainer2CoreRoom(UIOBJECT_CONTAINER cont)
{
	switch(cont)
	{
	case UOC_ITEMBOX_EXTEND:
		return room_itembox_extend;
		break;
	case UOC_STORE_EXTEND:
		return room_store_extend;
		break;
	case UOC_STORE_BOX:
		return room_repository;
		break;
	case UOC_ITEM_TAKE_WITH:
		return room_equipment;
		break;
	case UOC_TO_BE_TRADE:
		return room_trade;
		break;
	case UOC_OTHER_TO_BE_TRADE:
		return room_trade1;
		break;
	}
	return room_num;
}

UIOBJECT_CONTAINER KItemList::corePos2ClientContainer(ITEM_POSITION pos)
{
	switch(pos)
	{
	case pos_itembox_extend:
		return UOC_ITEMBOX_EXTEND;
		break;
	case pos_store_extend:
		return UOC_STORE_EXTEND;
		break;
	case pos_beset:
		return UOC_SMITH;
		break;
	case pos_pet_feed_box:
		return UOC_PET_FEED_BOX;
		break;
	case pos_repositoryroom:
		return UOC_STORE_BOX;
		break;
	case pos_equiproom:
		return UOC_ITEM_TAKE_WITH;
		break;
	case pos_equip:
		return UOC_EQUIPTMENT;
		break;
	case pos_traderoom:
		return UOC_TO_BE_TRADE;
		break;
	case pos_trade1:
		return UOC_OTHER_TO_BE_TRADE;
		break;
	}
	return UOC_IDLE;
}

INVENTORY_ROOM KItemList::corePos2coreRoom(ITEM_POSITION pos)
{
	switch(pos)
	{
	case pos_itembox_extend:
		return room_itembox_extend;
		break;
	case pos_store_extend:
		return room_store_extend;
		break;
	case pos_equiproom:
		return room_equipment;
		break;
	case pos_repositoryroom:
		return room_repository;
		break;
	case pos_traderoom:
		return room_trade;
		break;
	case pos_trade1:
		return room_trade1;
		break;
	case pos_beset:
		return room_beset;
		break;
	case pos_pet_feed_box:
		return room_pet_feed;
		break;
	}
	return room_num;
}

ITEM_POSITION KItemList::coreRoom2corePos(INVENTORY_ROOM room)
{
	switch(room)
	{
	case room_itembox_extend:
		return pos_itembox_extend;
		break;
	case room_store_extend:
		return pos_store_extend;
		break;
	case room_equipment:
		return pos_equiproom;
		break;
	case room_repository:
		return pos_repositoryroom;
		break;
	case room_trade:
		return pos_traderoom;
		break;
	case room_trade1:
		return pos_trade1;
		break;
	case room_beset:
		return pos_beset;
		break;
	case room_pet_feed:
		return pos_pet_feed_box;
		break;
	}
	return pos_equiproom;
}

#ifndef	_SERVER
//---------------------------------------------------------------------
//	功能：物品从一个地方直接移动到另一个地方，不经过鼠标这个中间过程
//---------------------------------------------------------------------
BOOL	KItemList::AutoMoveItem(ItemPos SrcPos,ItemPos DesPos)
{
	if (Player[this->m_PlayerIdx].CheckTrading())	// 如果正在交易
		return FALSE;
	
	BOOL	bMove = FALSE;
// 	int		nIdx, nListIdx;
// 	
// 	// 目前只支持从room_equipment到room_immediacy
// 	switch (SrcPos.nPlace)
// 	{
// 	case pos_equiproom:
// 		{
// 			switch (DesPos.nPlace)
// 			{
// 			}
// 		}
// 		break;
// 	}
// 	
// 	if (!bMove)
// 		return bMove;
// 	
// 	// 通知界面
// 	KObjAtContRegion sSrcInfo, sDestInfo;
// 	
// 	sSrcInfo.Obj.uGenre		= CGOG_ITEM;
// 	sSrcInfo.Obj.uId		= nIdx;
// 	sSrcInfo.Region.h		= SrcPos.nX;
// 	sSrcInfo.Region.v		= SrcPos.nY;
// 	sSrcInfo.eContainer		= UOC_ITEM_TAKE_WITH;
// 	
// 	sDestInfo.Obj.uGenre	= CGOG_ITEM;
// 	sDestInfo.Obj.uId		= nIdx;
// 	sDestInfo.Region.h		= DesPos.nX;
// 	sDestInfo.Region.v		= DesPos.nY;
// 	sDestInfo.eContainer	= UOC_SHORTCUT_BOX;
// 	
// //	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sSrcInfo, 0);
// //	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sDestInfo, 1);	
	
	return bMove;
}
#endif

#ifndef	_SERVER
//---------------------------------------------------------------------
//	功能：物品从一个地方直接移动到另一个地方，不经过鼠标这个中间过程
//---------------------------------------------------------------------
void	KItemList::MenuSetMouseItem()
{
	KObjAtContRegion	sInfo;
}
#endif

KItem* KItemList::GetItemFromPlace(ItemPos* pPos)
{
	int nItemIdx = 0;
	int nPlace = pPos->nPlace;
	int nX = pPos->nX;
	int nY = pPos->nY;
	switch(nPlace)
	{
	case pos_equiproom:
		nItemIdx = m_Room[room_equipment].FindItem(nX, nY);
		break;
	default:
		break;
	}
	if (nItemIdx <= 0)
		return NULL;
	return &Item[nItemIdx];
}

#ifdef _SERVER
BOOL KItemList::EatMecidine(int nPlace, int nX, int nY, int nTargetItemID, int nTargetPlace, int nTargetX, int nTargetY)
{
	ConfigManager& cm = ConfigManager::Singleton();
	int itemType = cm.GetGlobalVariable(global_var_item_lock_by_date_item_detail_type);
	KItem itemLock;
	BOOL bGen = g_ItemGen.Gen_Item( item_target, itemType, 0, 0, 1, &itemLock );
	int nTargetIdx = SearchID(nTargetItemID);
	if ( nPlace == 0xff &&
		nX == 0xff &&
		nY == 0xff &&
		nTargetPlace == 0xff &&
		nTargetX  == 0xff &&
		nTargetY  == 0xff && 
		nTargetIdx > 0 && 
		nTargetIdx < MAX_ITEM && 
		bGen == TRUE )
	{
		itemLock.UseItem( m_PlayerIdx, 0, nTargetIdx );
		return true;
	}

	int nItemIdx = 0;
	DWORD dwTargetId = 0; 
	switch(nPlace)
	{
	case pos_equiproom:
		{
			nItemIdx = m_Room[room_equipment].FindItem(nX, nY);
			if ( nItemIdx > 0 && nItemIdx < MAX_ITEM )
			{
				if ( Item[nItemIdx].IsPlayerTarget() )
				{
					dwTargetId = nTargetItemID;
				}
				else
				{
					int nTargetIdx = m_Room[room_equipment].FindItem(nTargetX, nTargetY);
					if (nTargetIdx > 0 && nTargetIdx < MAX_ITEM)
					{
						dwTargetId = Item[nTargetIdx].GetID();
					}
				}
				KPlayer& player = Player[m_PlayerIdx];
				if (CanUseItem(nItemIdx))
				{
					KItem& item = Item[nItemIdx];
					if (item.GetGenre() == item_medicine || item.GetGenre() == item_horse)
					{
						const ItemBasicBuff& basicBuff = item.GetBasicBuffID();
						for (int i = 0; i < ITEM_BUFF_COUNT; ++i )
						{
							bool canAdd = true;
							int nBuffTemplateID = basicBuff.BuffID[i];
							if ( nBuffTemplateID > 0 )
							{
								Buff* pBuff = CreateBuff( nBuffTemplateID );
								if( pBuff )
								{
									BUFF_ENV_PARAM Env;
									Env.nBuffSender		=	player.GetNpcIndex();
									Env.nBuffRecever	=	player.GetNpcIndex();
									Env.nBuffTempID		=	nBuffTemplateID;
									
									//前置条件
									if( !pBuff->PreCond( Env ) )
									{
										pBuff->ErrProc( Env );
										canAdd = false;
									}
									
									pBuff->Release( );
								}
								else
								{
									canAdd = false;
								}
							}
							
							if (!canAdd)
								return FALSE;
						}
					}

					DelayedAction useItemAction(m_PlayerIdx, delayed_action_use_item, Item[nItemIdx].GetActionTime(), delayed_action_msg_use_item);
					DelayedActionParamUseItem& param = useItemAction.GetUseItemParam();
					param.m_UseItemId = item.GetID();
					param.m_UseItemTargetId = dwTargetId;
					if (Player[m_PlayerIdx].GetActionDelayer().NewAction(useItemAction))
						return TRUE;
				}
			}
		}
		break;
	case pos_equip:
		ItemPos equipPos;
		equipPos.nPlace = nPlace;
		equipPos.nX = nX;
		equipPos.nY = nY;
		if(Equip(&equipPos))
			return TRUE;
		break;
	default:
		break;
	}
	
	return FALSE;
}
#endif

PlayerItem* KItemList::GetFirstItem()
{
	m_nListCurIdx = m_UseIdx.GetNext(0);
	return &m_Items[m_nListCurIdx];
}

PlayerItem* KItemList::GetNextItem()
{
	if ( !m_nListCurIdx )
		return NULL;
	m_nListCurIdx = m_UseIdx.GetNext(m_nListCurIdx);
	return &m_Items[m_nListCurIdx];
}

void	KItemList::ClearRoom(int nRoom)
{
	if (nRoom >= 0 && nRoom < room_num)
		this->m_Room[nRoom].Clear();
}

void    KItemList::ClearRoomItemOnly(int nRoom)
{
	if (nRoom >= 0 && nRoom < room_num)
		this->m_Room[nRoom].ClearItem();
}

void	KItemList::StartTrade()
{
	ClearRoom(room_trade);
	ClearRoom(room_trade1);
}

//注意，不作判断重复调用，会令到效果叠加
// 需要对上下马做一些特例
// 所有的上马或下马动作都必须经过这个函数
int     KItemList::SetItemMask(int nNpcIdx, int nPart, BOOL nMask, BOOL bOnOrOffHorse /* = FALSE */)
{
	int nRet = -1;
	if(nNpcIdx > 0 && nNpcIdx < MAX_NPC &&
		nPart >= 0 && nPart < itempart_num && m_EquipItem[nPart].nEquipIdx > 0)
	{
		int nIdx = m_EquipItem[nPart].nEquipIdx;
		if(nMask)
		{
			if (!m_EquipItem[nPart].IsMasked)
			{
				// 对上下马做特例
				// Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx]);
				//Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx], bOnOrOffHorse);

				m_EquipItem[nPart].IsMasked = true;
				// 如果有召唤兽移除属性 
//				if (Player[m_PlayerIdx].m_Creature.IsALive()) 
//				{
//					KNpc* pNpc = Player[m_PlayerIdx].m_Creature.GetCreatureNpc();
//					Item[nIdx].RemoveMagicAttribFromNPC(pNpc, bOnOrOffHorse);
//					RemoveGreenItemAttribFromCreature(nIdx);
//#ifdef _SERVER
//					if (itempart_weapon == nPart)
//					{
//						int nMinDamage =0; int nMaxDamage =0;
//						GetWeaponDamage(&nMinDamage, &nMaxDamage);
//						Player[m_PlayerIdx].m_Creature.UpDataWeaponDamage(-nMinDamage, -nMaxDamage);
//					}					
//					Player[m_PlayerIdx].m_Creature.UpDataEchoDamage();
//#endif
//				}

			}
		}
		else
		{
			if (m_EquipItem[nPart].IsMasked)
			{
				//Item[nIdx].ApplyMagicAttribToNPC(&Npc[nNpcIdx], bOnOrOffHorse);

				m_EquipItem[nPart].IsMasked = false;
//				if (Player[m_PlayerIdx].m_Creature.IsALive()) 
//				{
//					KNpc* pNpc = Player[m_PlayerIdx].m_Creature.GetCreatureNpc();
//
//					Item[nIdx].ApplyMagicAttribToNPC(pNpc, bOnOrOffHorse);
//					ApplyGreenItemAttribToCreature(nIdx);					
//#ifdef _SERVER
//					if (itempart_weapon == nPart)
//					{
//						int nMinDamage =0; int nMaxDamage =0;
//						GetWeaponDamage(&nMinDamage, &nMaxDamage);
//						Player[m_PlayerIdx].m_Creature.UpDataWeaponDamage(nMinDamage,nMaxDamage);
//					}					
//					Player[m_PlayerIdx].m_Creature.UpDataEchoDamage();
//#endif
//				}
			}
		}
		nRet = nMask;
	}
	return nRet;
}

/*!*****************************************************************************
// Function		: KItemList::RemoveAll
// Purpose		: 退出时清除所有的装备
// Return		: void
// Comments		: 会实际地从游戏世界中的道具数组中去掉
// Author		: Spe
*****************************************************************************/
void KItemList::RemoveAll()
{
	int nIdx = m_UseIdx.GetNext(0);
	int nIdx1 = 0;
	while(nIdx)
	{
		nIdx1 = m_UseIdx.GetNext(nIdx);
		int nGameIdx = m_Items[nIdx].nIdx;
		if (Remove(m_Items[nIdx].nIdx,  0, true, true, true, false) == FALSE)
		{
			m_UseIdx.Remove(nIdx);
			m_FreeIdx.Insert(nIdx);
		}
#ifdef _SERVER
		// 客户端在上面KItemList::Remove()已经做了ItemSet.Remove()
		else 
			ItemSet.Remove(nGameIdx);
#endif
		nIdx = nIdx1;
	}

#ifdef _SERVER
	//Clear Inventory
	for (int nRoomIndex = 0; nRoomIndex < room_num ; nRoomIndex ++ )
	{
		ClearRoomItemOnly(nRoomIndex);
	}//end for nRoomIndex
#endif
}

BOOL KItemList::IsTaskItemExist(int nDetailType)
{
	int nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;
		if (item_task != Item[nGameIdx].GetGenre())
			continue;
		if (nDetailType == Item[nGameIdx].GetDetailType())
		{
			return TRUE;
		}
	}
	return FALSE;
}

#ifdef _SERVER
int	KItemList::GetTaskItemNum(int nDetailType)
{
	int		nNo = 0;
	int		nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		if (item_task != Item[m_Items[nIdx].nIdx].GetGenre())
			continue;
		if (nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType())
		{
			nNo += Item[m_Items[nIdx].nIdx].GetItemCount();
		}
	}
	return nNo;
}

int	KItemList::GetTaskItemNumEx(int nDetailType)
{
	int		nNo = 0;
	int		nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		if (item_task != Item[m_Items[nIdx].nIdx].GetGenre())
			continue;
		if (nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType()
			&& m_Items[nIdx].nPlace == pos_equiproom)
		{
			nNo += Item[m_Items[nIdx].nIdx].GetItemCount();
		}
	}
	return nNo;
}
#endif

#ifdef _SERVER
BOOL KItemList::RemoveTaskItem(int nDetailType)
{
	int nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;
		if (item_task != Item[nGameIdx].GetGenre())
			continue;
		if (nDetailType == Item[nGameIdx].GetDetailType())
		{
			Remove(nGameIdx);
			ItemSet.Remove(nGameIdx);
			return TRUE;
		}
	}
	return FALSE;
}
#endif

#ifdef _SERVER
//--------------------------------------------------------------------------
//	功能：交易中把 trade room 中的 item 的 idx width height 信息写入 itemset 中的 m_psItemInfo 中去，返回值表明交易物品最好装备的类型号
//--------------------------------------------------------------------------
int	KItemList::GetTradeRoomItemInfo()
{
	_ASSERT(ItemSet.m_psItemInfo);
	memset(ItemSet.m_psItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
	
	int		nItemIdx, nXpos, nYpos, nPos;
	
	nItemIdx = 0;
	nXpos = 0;
	nYpos = 0;
	nPos = 0;

	int nBestItemType = 0;

	while (1)
	{
		nItemIdx = m_Room[room_trade].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx == 0)
			break;
		_ASSERT(nPos < TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
		
		ItemSet.m_psItemInfo[nPos].m_nIdx = nItemIdx;
		if(Item[nItemIdx].GetGenre() == ITEMCLASS_SAPPHIRE && Item[nItemIdx].GetDetailType() == ITEMDETAIL_SAPPHIRE || 
			Item[nItemIdx].GetGenre() == ITEMCLASS_YUANBAO && Item[nItemIdx].GetDetailType() == ITEMDETAIL_YUANBAO || 
			Item[nItemIdx].GetGenre() == ITEMCLASS_TONGQIAN && Item[nItemIdx].GetDetailType() == ITEMDETAIL_TONGQIAN)
		{
			nBestItemType = _LOGITEMTYPE_;
		}
		else
		{
	/*
			int nCurItemType = Item[nItemIdx].GetEquipMagicType();
				if(nCurItemType > nBestItemType)
				{
					nBestItemType = nCurItemType;
				}*/
	
		}
		nPos++;
	}
	
	// 从大到小排序
// 	TRADE_ITEM_INFO	sTemp;
// 	for (int i = nPos - 1; i >= 0; i--)
// 	{
// 		for (int j = 0; j < i; j++)
// 		{
// 			if (ItemSet.m_psItemInfo[j].m_nWidth * ItemSet.m_psItemInfo[j].m_nHeight < 
// 				ItemSet.m_psItemInfo[j + 1].m_nWidth * ItemSet.m_psItemInfo[j + 1].m_nHeight)
// 			{
// 				sTemp = ItemSet.m_psItemInfo[j];
// 				ItemSet.m_psItemInfo[j] = ItemSet.m_psItemInfo[j + 1];
// 				ItemSet.m_psItemInfo[j + 1] = sTemp;
// 			}
// 		}
// 	}

	return nBestItemType;
}

#endif
//等待删除（xiehong－2006年11月23日）
#ifdef _SERVER
//--------------------------------------------------------------------------
//	功能：交易中判断买进的物品能不能完全放进自己的物品栏
//--------------------------------------------------------------------------
BOOL	KItemList::TradeCheckCanPlace()
{
// 	LPINT	pnTempRoom;
// 	pnTempRoom = new int[EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT];
// 	memcpy(pnTempRoom, m_Room[room_equipment].m_pArray, sizeof(int) * EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT);
// 	
// 	int		nPos, i, j, nFind;
// 	for (nPos = 0; nPos < TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT; nPos++)
// 	{
// 		if (!ItemSet.m_psItemInfo[nPos].m_nIdx)
// 			break;
// 		nFind = 0;
// 		for (i = 0; i < EQUIPMENT_ROOM_HEIGHT ; i++)
// 		{
// 			for (j = 0; j < EQUIPMENT_ROOM_WIDTH ; j++)
// 			{
// 				if (pnTempRoom[i * EQUIPMENT_ROOM_WIDTH + j ] == 0)
// 				{
// 					// 数据处理
// 					ItemSet.m_psItemInfo[nPos].m_nX = j;
// 					ItemSet.m_psItemInfo[nPos].m_nY = i;
// 					pnTempRoom[i * EQUIPMENT_ROOM_WIDTH + j] = ItemSet.m_psItemInfo[nPos].m_nIdx;					
// 					nFind = 1;
// 					break;
// 				}
// 			}
// 			if (nFind)
// 				break;
// 		}
// 		if (!nFind)
// 		{
// 			delete []pnTempRoom;
// 			return FALSE;
// 		}
// 	}
// 	
// 	delete []pnTempRoom;
	return TRUE;
}


#endif

#ifdef _SERVER
//--------------------------------------------------------------------------
//	功能：判断一定长宽的物品能否放进物品栏 (为了服务器效率，本函数里面没有调用其他函数)
//--------------------------------------------------------------------------
BOOL	KItemList::CheckCanPlaceInEquipment(int *pnX, int *pnY)
{
#pragma message("maybe need fix to support pile by cooler")
	
	//
	POINT freePos;
	if(m_Room[room_equipment].FindRoom(&freePos))
	{
		*pnX = freePos.x;
		*pnY = freePos.y;
		return TRUE;
	}
	return FALSE;

// 	if (!pnX || !pnY)
// 		return FALSE;
// 	
// 	_ASSERT(m_Room[room_equipment].m_pArray);
// 	
// 	LPINT	pnTempRoom;
// 	int		i, j;
// 	
// 	pnTempRoom = m_Room[room_equipment].m_pArray;
// 	
// 	for (i = 0; i < EQUIPMENT_ROOM_HEIGHT ; i++)
// 	{
// 		for (j = 0; j < EQUIPMENT_ROOM_WIDTH ; j++)
// 		{
// 			if (pnTempRoom[i * EQUIPMENT_ROOM_WIDTH + j] == 0)
// 			{
// 				*pnX = j;
// 				*pnY = i;
// 				return TRUE;
// 			}
// 		}
// 	}
// 
// 	return FALSE;
}


bool KItemList::IsGroupCoolDown( int nGroup )
{
	for( int nLoopCount = 0; nLoopCount < MAX_REPOSITORY_ITEM; nLoopCount++ )
	{
		if( m_GroupCD[nLoopCount].nGroup == nGroup )
			return true;
	}

	return false;
}

void KItemList::AddGroupCoolDown( int nGroup, unsigned long ulTime, void* pParam/* = NULL */, PCoolDownCompleteCallback pCompleteCallback/* = NULL */ )
{
	for( int nLoopCount = 0; nLoopCount < MAX_REPOSITORY_ITEM; nLoopCount++ )
	{
		if( m_GroupCD[nLoopCount].nGroup == -1 )
		{
			SyncGroupCDAdd( nGroup, ulTime );
			m_GroupCD[nLoopCount].nGroup = nGroup;
			m_GroupCD[nLoopCount].ulCDTime = ulTime;
			m_GroupCD[nLoopCount].ulStartTime = UNIX_TMIE_STAMP;
			m_GroupCD[nLoopCount].pParam = pParam;
			m_GroupCD[nLoopCount].pComplete = pCompleteCallback;
			break;
		}
	}	
}

void KItemList::DelGroupCoolDown( int nGroup )
{
	for( int nLoopCount = 0; nLoopCount < MAX_REPOSITORY_ITEM; nLoopCount++ )
	{
		if( m_GroupCD[nLoopCount].nGroup == nGroup )
		{
			SyncGroupCDDel( m_GroupCD[nLoopCount].nGroup );
			m_GroupCD[nLoopCount].nGroup = -1;
			m_GroupCD[nLoopCount].ulCDTime = 0;
			m_GroupCD[nLoopCount].ulStartTime = 0;
			m_GroupCD[nLoopCount].pParam = NULL;
			m_GroupCD[nLoopCount].pComplete = NULL;
			break;
		}
	}
}

void KItemList::LoopGroupCoolDown( )
{
	unsigned long ulCurTime = UNIX_TMIE_STAMP;

	for( int nLoopCount = 0; nLoopCount < MAX_REPOSITORY_ITEM; nLoopCount++ )
	{
		if( m_GroupCD[nLoopCount].nGroup != -1 && 
			ulCurTime - m_GroupCD[nLoopCount].ulStartTime >= m_GroupCD[nLoopCount].ulCDTime )
		{
			SyncGroupCDDel( m_GroupCD[nLoopCount].nGroup );
			m_GroupCD[nLoopCount].nGroup = -1;
			m_GroupCD[nLoopCount].ulCDTime = 0;
			m_GroupCD[nLoopCount].ulStartTime = 0;
			PCoolDownCompleteCallback pComplete = m_GroupCD[nLoopCount].pComplete;
			if (pComplete != NULL)
			{
				(*pComplete)( m_GroupCD[nLoopCount].pParam );
			}
		}
	}
}

void KItemList::SyncGroupCDAdd( int nGroup, unsigned long ulTime )
{
	_ItemGroupCD_Add		IGAdd;
	IGAdd.Protocol			=	s2c_byte_extend;
	IGAdd.ProtocolExtend	=	s2c_ex_protocol_additemgroupcd;
	IGAdd.wProtocolSize		=	sizeof( _ItemGroupCD_Add ) - 1;
	IGAdd.nGroup			=	nGroup;
	IGAdd.ulCDTime			=	ulTime;
	
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(
			Player[m_PlayerIdx].m_nNetConnectIdx, 
			&IGAdd,
			sizeof(IGAdd) );
}

void KItemList::SyncGroupCDDel( int nGroup )
{
	_ItemGroupCD_Del		IGDel;
	IGDel.Protocol			=	s2c_byte_extend;
	IGDel.ProtocolExtend	=	s2c_ex_protocol_delitemgroupcd;
	IGDel.wProtocolSize		=	sizeof( _ItemGroupCD_Del ) - 1;
	IGDel.nGroup			=	nGroup;
	
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(
			Player[m_PlayerIdx].m_nNetConnectIdx, 
			&IGDel,
			sizeof(IGDel) );
}

#endif
//------------------------------------------------------------------------------
//	功能：在room_equipment中查找指定Genre和DetailType的物品，得到ItemIdx和位置
//------------------------------------------------------------------------------
BOOL	KItemList::FindSameParticularInEquipment(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY)
{
	return m_Room[room_equipment].FindSameParticularType(nGenre, nDetail, nParticular,  pnIdx, pnX, pnY);
}

BOOL	KItemList::FindSameParticularItem(int nGenre, int nDetail, int nParticular, int *pnIdx)
{
	int x, y;
	return m_Room[room_equipment].FindSameParticularType(nGenre, nDetail, nParticular, pnIdx, &x, &y);
}

BOOL	KItemList::FindLockedParticularItem(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY)
{
	return m_Room[room_equipment].FindLockedParticularItem(nGenre, nDetail, nParticular, pnIdx, pnX, pnY);
}

//等待删除（xiehong－2006-11-22）
BOOL	KItemList::FindNextSameparticularItem(int nIdx, int nGenre, int nDetail, int nParticular, int *pnIdx)
{
//	return m_Room[room_equipment].FindNextSameParticularType(nIdx, nGenre, nDetail, nParticular, pnIdx);
	return true;
}

void	KItemList::RemoveAllInOneRoom(int nRoom)
{
	if (nRoom < 0 || nRoom >= room_num)
		return;

	for ( int x = 0; x < m_Room[nRoom].getWidth(); ++x )
	{
		for ( int y = 0; y < m_Room[nRoom].getHeight(); ++y )
		{
			int nItemIdx = m_Room[nRoom].FindItem( x, y );
			if ( nItemIdx > 0 )
			{
				Remove(nItemIdx);
				ItemSet.Remove(nItemIdx);
			}
		}
	}

	ClearRoomItemOnly(nRoom);
}

#ifndef _SERVER
void KItemList::LockOperation()
{
	if (IsLockOperation())
	{
		_ASSERT(0);
		return;
	}
	m_bLockOperation = TRUE;
}
#endif

#ifndef _SERVER
void KItemList::UnlockOperation()
{
	if (!IsLockOperation())
	{
		return;
	}
	m_bLockOperation = FALSE;
}
#endif

#ifdef _SERVER

void KItemList::Abrade(AbradeMode mode)
{
	const AbradeInfo* pAbrade = AbradeTable::Singleton().GetAbrade(mode);
	_ASSERT(pAbrade != NULL);

	for (int i = 0; i < itempart_num; i++)
	{	
		if (m_EquipItem[i].nEquipIdx > 0 && pAbrade->Value[i] > 0)//装备存在而且需要磨损这个装备
		{			
			KItem& item = Item[m_EquipItem[i].nEquipIdx];
			int originalDur = item.GetDurability();
			int currentDur = 0;
			
			if (originalDur > 0)//这个装备可以磨损
			{
				if (pAbrade->ValueType == abrade_value_by_count)//按数值磨损
				{
					currentDur = item.Abrade(pAbrade->Value[i]);
				}
				else if (pAbrade->ValueType == abrade_value_by_percentage)//按比例磨损
				{
					currentDur = item.Abrade(((float)pAbrade->Value[i]) / 100);
				}
				
				if (currentDur < originalDur)//耐久发生变化
				{
					item.SyncAttribute(item_attr_durability, Player[m_PlayerIdx].GetNetConnectIdx());
					if (currentDur == 0)//磨损为0了，需要重新设定装备的效果
					{
						OnEquipChanged();
					}
				}
			}
		}
	}
}

void KItemList::AddShortCut( 
	DWORD dwPos, 
	DWORD dwID, 
	DWORD dwType )
{
	if( dwPos >= MAX_IMMEDIACY_ITEM )
		return;

	m_ShortCut[dwPos].nID = dwID;
	m_ShortCut[dwPos].nImmediacyType = dwType;
	m_ShortCut[dwPos].nPos = dwPos;
}

void KItemList::DelShortCut( 
	DWORD dwPos )
{
	if( dwPos >= MAX_IMMEDIACY_ITEM )
		return;
	m_ShortCut[dwPos].nID = -1;
	m_ShortCut[dwPos].nImmediacyType = -1;
	m_ShortCut[dwPos].nPos = -1;
}

void KItemList::SyncShortCut( )
{
	for( int nLoopCount = 0; nLoopCount < MAX_IMMEDIACY_ITEM; nLoopCount++ )
	{
		if( m_ShortCut[nLoopCount].nID == -1 )
			continue;

		_ShortCut_Add	SAdd;
		SAdd.Protocol			=	s2c_byte_extend;
		SAdd.ProtocolExtend		=	s2c_ex_protocol_shortcut_add;
		SAdd.wProtocolSize		=	sizeof( _ShortCut_Add ) - 1;
		SAdd.dwID				=	m_ShortCut[nLoopCount].nID;
		SAdd.dwPos				=	m_ShortCut[nLoopCount].nPos;
		SAdd.dwSCType			=	m_ShortCut[nLoopCount].nImmediacyType;
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(
				Player[m_PlayerIdx].m_nNetConnectIdx, 
				&SAdd,
				sizeof(SAdd) );
	}
}

KImmediacyParam* KItemList::GetShortCut( void ) const
{
	return (KImmediacyParam*)&m_ShortCut[0];
}

#endif

#ifndef _SERVER
//-------------------------------------------------------------------------------
//	功能：
//-------------------------------------------------------------------------------
int		KItemList::GetSameParticularItemNum(int nImmediatePos)
{
	int	nIdx = m_Room[room_equipment].FindItem(nImmediatePos, 0);
	return m_Room[room_equipment].CalcSameParticularType(Item[nIdx].GetGenre(), Item[nIdx].GetDetailType(), Item[nIdx].GetParticular()) + 1;
}

#endif

extern KItemGenerator *g_pItemGen;

#ifdef _SERVER
BOOL KItemList::DelNormalItemOnlyItemBox(int nItemClass,int nDetailType,int nParticualrType,int nLevel, bool checkLevelupTimes/* = true*/ )
{
	int nIdx;
	int nX;
	int nY;
    if(m_Room[room_equipment].FindValidIBItem(nItemClass, nDetailType, nParticualrType, nLevel, &nIdx, &nX, &nY, FALSE, checkLevelupTimes ) )
    {
        KItem& aItem = Item[nIdx];
        int originalItemWeight = aItem.GetItemWeight();
        unsigned int uItemCount = aItem.GetItemCount();
        if (uItemCount > 1 )
        {			
            aItem.SetItemCount(--uItemCount);
            m_Room[room_equipment].AddWeight(aItem.GetItemWeight() - originalItemWeight);
            aItem.SyncItemRefresh( Player[m_PlayerIdx].m_nNetConnectIdx );
        }
        else
        {
            Remove(nIdx);
            ItemSet.Remove(nIdx);				
        }
        return TRUE;
    }
	return FALSE;	
}

BOOL KItemList::DelNormalItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel)
{
	int nIdx;
	int nX;
	int nY;
    INVENTORY_ROOM checkRoom[2] = {room_equipment, room_repository};
    for (int i = 0; i < (sizeof(checkRoom)/sizeof(checkRoom[0])); i++)
    {
        if(m_Room[i].FindNormalItem(nItemClass, nDetailType, nParticualrType, nLevel, &nIdx, &nX, &nY, FALSE))
        {
            KItem& aItem = Item[nIdx];
            int originalItemWeight = aItem.GetItemWeight();
            unsigned int uItemCount = aItem.GetItemCount();
            if (uItemCount > 1 )
            {			
                aItem.SetItemCount(--uItemCount);
                m_Room[i].AddWeight(aItem.GetItemWeight() - originalItemWeight);
                aItem.SyncItemRefresh( Player[m_PlayerIdx].m_nNetConnectIdx );
            }
            else
            {
                Remove(nIdx);
                ItemSet.Remove(nIdx);				
            }
            return TRUE;
        }
    }
	return FALSE;
}

BOOL KItemList::DelValidIBItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel)
{
	int nIdx;
	int nX;
	int nY;
    //INVENTORY_ROOM checkRoom[2] = {room_equipment, room_repository};
    //for (int i = 0; i < (sizeof(checkRoom)/sizeof(checkRoom[0])); i++)
    //{
    if(m_Room[room_equipment].FindValidIBItem(nItemClass, nDetailType, nParticualrType, nLevel, &nIdx, &nX, &nY, FALSE))
    {
        KItem& aItem = Item[nIdx];
        int originalItemWeight = aItem.GetItemWeight();
        unsigned int uItemCount = aItem.GetItemCount();
        if (uItemCount > 1 )
        {			
            aItem.SetItemCount(--uItemCount);
            m_Room[room_equipment].AddWeight(aItem.GetItemWeight() - originalItemWeight);
            aItem.SyncItemRefresh( Player[m_PlayerIdx].m_nNetConnectIdx );
        }
        else
        {
            Remove(nIdx);
            ItemSet.Remove(nIdx);				
        }
        return TRUE;
    }
    //}
	return FALSE;
}

BOOL KItemList::DelNormalItemByID(int nItemID)
{
	int nIdx;
	int nX;
	int nY;
    INVENTORY_ROOM checkRoom[2] = {room_equipment, room_repository};
    for (int i = 0; i < (sizeof(checkRoom)/sizeof(checkRoom[0])); i++)
    {
        if(m_Room[i].FindNormalItemByID( nItemID, &nIdx, &nX, &nY, FALSE))
        {
            KItem& aItem = Item[nIdx];
            int originalItemWeight = aItem.GetItemWeight();
            unsigned int uItemCount = aItem.GetItemCount();
            if (uItemCount > 1 )
            {			
                aItem.SetItemCount(--uItemCount);
                m_Room[i].AddWeight(aItem.GetItemWeight() - originalItemWeight);
                aItem.SyncItemRefresh( Player[m_PlayerIdx].m_nNetConnectIdx );
            }
            else
            {
                Remove(nIdx);
                ItemSet.Remove(nIdx);				
            }
            return TRUE;
        }
    }
	return FALSE;
}

#endif

#ifdef _SERVER
BOOL KItemList::DelMagicItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel)
{
	int nIdx;
	int nX;
	int nY;
	
	if(m_Room[room_equipment].FindNormalItem(nItemClass, nDetailType, nParticualrType, nLevel, &nIdx, &nX, &nY, TRUE))
	{
		KItem& aItem = Item[nIdx];
		int originalItemWeight = aItem.GetItemWeight();
		unsigned int uItemCount = aItem.GetItemCount();
		if (uItemCount > 1 )
		{						
			aItem.SetItemCount(--uItemCount);
			m_Room[room_equipment].AddWeight(aItem.GetItemWeight() - originalItemWeight);
			aItem.SyncItemRefresh( Player[m_PlayerIdx].m_nNetConnectIdx );
		}
		else
		{
			Remove(nIdx);
			ItemSet.Remove(nIdx);				
		}

		return TRUE;
	}
	return FALSE;
}

int KItemList::DelStatckItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel, int nStatckCount )
{
	int nIdx;
	int nX;
	int nY;
	
	if(m_Room[room_equipment].FindNormalItem(
		nItemClass, nDetailType, nParticualrType, nLevel, &nIdx, &nX, &nY, FALSE))
	{
		KItem& aItem = Item[nIdx];
		int originalItemWeight = aItem.GetItemWeight();
		unsigned int uItemCount = aItem.GetItemCount();
		if ( uItemCount > nStatckCount )
		{			
			aItem.SetItemCount(uItemCount - nStatckCount);
			m_Room[room_equipment].AddWeight(aItem.GetItemWeight() - originalItemWeight);
			aItem.SyncItemRefresh( Player[m_PlayerIdx].m_nNetConnectIdx );

			return nStatckCount;
		}
		else
		{
			Remove(nIdx);
			ItemSet.Remove(nIdx);	
			return uItemCount;
		}
	}
	return 0;
}

#endif

#ifdef _SERVER
BOOL KItemList::WastePlaceItem(int nItemClass, int nItemDetail, 
							   int nItemParti, int nItemLevel, int nWasteCount)
{
	int nIdx = 0, nX = 0, nY = 0;
	
	int nLeftWaste = nWasteCount;
	
	while(nLeftWaste > 0)
	{
		if(m_Room[room_equipment].FindNormalItem(
			nItemClass, nItemDetail, nItemParti, nItemLevel, &nIdx, &nX, &nY))
		{
			m_Room[room_equipment].CheckItemValid(nIdx, m_PlayerIdx, nX, nY);

			int originalItemWeight = Item[nIdx].GetItemWeight();
			int nCurCount = Item[nIdx].GetItemCount();
			if(nLeftWaste >= nCurCount)
			{
				nLeftWaste -= nCurCount;
				Remove(nIdx);
				ItemSet.Remove(nIdx);
			}
			else
			{
				Item[nIdx].SetItemCount(nCurCount - nLeftWaste);
				m_Room[room_equipment].AddWeight(Item[nIdx].GetItemWeight() - originalItemWeight);				
				Item[nIdx].SyncItemRefresh( Player[m_PlayerIdx].m_nNetConnectIdx );

				nLeftWaste = 0;
			}
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

#endif

#ifdef _SERVER
// 把返回值由void改为int，表示被删除元宝个数
int KItemList::ClearAllInvalidItem()
{
	int nIdx = m_UseIdx.GetNext(0);
	int nIdx1 = 0;

	int	nDelTreasureNum = 0;

/*
	while(nIdx)
	{
		nIdx1 = m_UseIdx.GetNext(nIdx);
		
		if(Item[m_Items[nIdx].nIdx].IsItemValid() == false)
		{
			if(Remove(m_Items[nIdx].nIdx))
			{
				nDelTreasureNum++;
			}
		}

		nIdx = nIdx1;
	}*/


	return nDelTreasureNum;
}
#endif

#ifdef _SERVER
int KItemList::GetInvalidItemCount()
{
	int nIdx = m_UseIdx.GetNext(0);
	int nIdx1 = 0;

	int	nInvalidItemNum = 0;

/*
	while(nIdx)
	{
		nIdx1 = m_UseIdx.GetNext(nIdx);
		
		if(Item[m_Items[nIdx].nIdx].IsItemValid() == false)
		{
			nInvalidItemNum++;
		}

		nIdx = nIdx1;
	}*/


	return nInvalidItemNum;
}
#endif

bool KItemList::IsPosEmpty(int nPlace, int nX, int nY)
{
	bool bRet = false;

	switch(nPlace)
	{
	case pos_equip:
		if(nX >= 0 && nX < itempart_num)
		{
			bRet = (0 == m_EquipItem[nX].nEquipIdx);
		}
		break;

	case pos_equiproom:
		bRet = m_Room[room_equipment].isPosEmpty(nX, nY);
		break;

	case pos_repositoryroom:
		bRet = m_Room[room_repository].isPosEmpty(nX, nY);
		break;

	case pos_traderoom:
		bRet = m_Room[room_trade].isPosEmpty(nX, nY);
		break;

	case pos_trade1:
		bRet = m_Room[room_trade1].isPosEmpty(nX, nY);
		break;

	case pos_beset:
		bRet = m_Room[room_beset].isPosEmpty(nX, nY);
		break;

	case pos_pet_feed_box:
		bRet = m_Room[room_pet_feed].isPosEmpty(nX, nY);
		break;
	
	default:
		break;
	}

	return bRet;
}

void KItemList::CheckEquipmentExpireTime()
{
	bool bChanged = false;

	//--> Rocker 2004/08/03
	for (int i=0; i<itempart_num; i++)
	{
		BOOL bNewOverdataState = FALSE;

		int  nEquipItemIndex   = m_EquipItem[i].nEquipIdx;
		if (nEquipItemIndex > 0 && nEquipItemIndex < MAX_ITEM)
			 bNewOverdataState = Item[nEquipItemIndex].IsOverDate();

		if (bNewOverdataState!= m_EquipOverdataState[i])
			bChanged = true;

		m_EquipOverdataState[i] = bNewOverdataState;
	}//end for i

	if (bChanged)
		OnEquipChanged();
}

void KItemList::OnEquipChanged()
{
	int npcIndex = Player[m_PlayerIdx].GetNpcIndex();

#ifdef _SERVER
	//更新基本属性
	m_BasicPropertyMonitor.Update(npcIndex, m_EquipItem);
#else
	//更新装备磨损
	m_AbradeMonitor.Update(m_EquipItem);
#endif

	//更新套装
	m_ArmorSetMonitor.Update(npcIndex, m_EquipItem);

	//更新爻装
	m_YaoMonitor.Update(npcIndex, m_EquipItem);

	//更新法宝
	m_TalismanMonitor.Update(npcIndex, m_EquipItem);	
}

void KItemList::OnBagChanged(bool isCharm)
{
	KPlayer& player = Player[m_PlayerIdx];

#ifdef _SERVER
	if (isCharm)
	{
		m_CharmMonitor.Update(player.GetNpcIndex(), *this);
	}
#endif
	
	//检查是否超重
	player.CheckWeight();

#ifndef _SERVER
	//更新负重显示
	CoreDataChanged(GDCNI_PLAYER_WEIGHT_CHANGED, (unsigned int)player.GetWeightTaken(), player.GetWeightMax());
#endif
}

int KItemList::bagExtend(int itemIndex, INVENTORY_ROOM room)
{
	//如果不是包裹类物品，或者本来就没有物品，则返回0
	if(itemIndex <= 0)
		return 0;

	if(Item[itemIndex].GetGenre() != item_task || Item[itemIndex].GetLevel() != QK_Bag)
		return 0;
	
	return Item[itemIndex].GetItemQuality();
}

void KItemList::onBagSized(INVENTORY_ROOM room)
{
#ifndef _SERVER
	//更新格子数
	CoreDataChanged(GDCNI_PLAYER_BAG_SIZED, (unsigned int)room, m_Room[room].getValidSize());
#endif
}

bool KItemList::isEquipHaveItem( ITEM_PART nEquip )
{
	return m_EquipItem[nEquip].nEquipIdx > 0 ? true : false;
}

#ifdef _SERVER
void KItemList::EnableTalismanCallback( void* pParam )
{
	KItemList* pItemList = (KItemList*)pParam;

	if (pItemList != NULL)
	{
		pItemList->MaskEquipment(itempart_talisman, false);
		int npcIndex = Player[pItemList->GetPlayerIndex()].GetNpcIndex();
		pItemList->m_TalismanMonitor.Update(npcIndex, pItemList->m_EquipItem);		
	}
}
#endif

void KItemList::MaskEquipment(ITEM_PART itemPart, bool isMasked)
{
	if (itemPart >= 0 && itemPart < itempart_num)
	{
		m_EquipItem[itemPart].IsMasked = isMasked;
	}
}

bool KItemList::IsEquipmentMasked(ITEM_PART itemPart)
{
	if (itemPart >= 0 && itemPart < itempart_num)
	{
		return m_EquipItem[itemPart].IsMasked;
	}
	else
	{
		return false;
	}
}

void itemTarget( char* string, char pTargetItem[][COMMON_CLIENT_MSG_LEN_64] )
{
	const char seps[]   = ";";
	int i = 0;
	char* token = NULL; 
	token = strtok( string, seps );
	while( token != NULL )
	{
		strncpy( pTargetItem[i], token, COMMON_CLIENT_MSG_LEN_64 );
		token = strtok( NULL, seps );
		i++;
	}
}

bool KItemList::CanUseItem(int itemIndex, int targetItemIndex)
{
	if (m_PlayerIdx <= 0)
		return false;

	if (itemIndex <= 0)
		return false;

	KItem& item = Item[itemIndex];
	KPlayer& player = Player[m_PlayerIdx];

	if ( item.IsOverDate() )
	{
		return false;
	}

	//物品种类
	int itemGenre = item.GetGenre();	
	if (itemGenre != item_medicine
		&& itemGenre != item_horse
		&& itemGenre != item_target
		&& itemGenre != item_equip
		&& itemGenre != item_ib )
		return false;

#ifdef _SERVER
	if ( itemGenre == item_ib && IsValidPlayer(m_PlayerIdx) && Player[m_PlayerIdx].m_AntiEnthrall.GetCurState() != AntiEnthrall::enAntiEnthrall_Normal )
	{
		item.ItemErrCodeToClient(m_PlayerIdx,item_inlay_error_targetitem_rule);
		return false;
	}
#endif
	
	//等级要求
	if (!item.IsMatchLevelRequirement(player))
		return false;

	//职业要求
	if (!item.IsMatchProfessionRequirement(player))
		return false;
	
	//属性要求
	if (!item.IsMatchPropertyRequirement(player))
		return false;

	//判断是否可以对目标使用
	if (item.IsItemTarget() &&
		(itemGenre == item_target || (itemGenre == item_ib && !item.IsNoTarget() && item.IsItemTarget() ) )&&
		targetItemIndex > 0)
	{
		bool ret = false;
		char str[TARGETITEM_TARGET_COUNT][COMMON_CLIENT_MSG_LEN_64];
		ZeroMemory( str, sizeof(char)*COMMON_CLIENT_MSG_LEN_64 );
		char szTemp[SZBUFLEN_0] = {0};
		strncpy( szTemp, item.GetItemTemplate()->szBuffTarget, SZBUFLEN_0);
		itemTarget( szTemp, str );

		for ( int nIdx = 0; nIdx < TARGETITEM_TARGET_COUNT; ++nIdx )
		{
			if ( str[nIdx][0] == NULL )
			{
				continue;
			}
			int nGenre = 0; int nDetailType = 0;int nParticularType = 0; int nLevel = 0; int nCount = 0; int nQuality = 0; int nYao = 0;
			::sscanf(str[nIdx], "%d|%d|%d|%d|%d|%d|%d", 
				&(nGenre), &(nDetailType), &(nParticularType), &(nLevel), &(nCount), &(nQuality), &(nYao));
			KCompoundRules::KSrcItemDescriptor srcItemDesc;
			srcItemDesc.nItemGenre		= nGenre;
			srcItemDesc.nItemDetail		= nDetailType;
			srcItemDesc.nItemParticular	= nParticularType;
			srcItemDesc.nItemLevel		= nLevel;
			srcItemDesc.nItemCount		= nCount;
			srcItemDesc.nItemQuality	= nQuality;
			srcItemDesc.nItemYao		= nYao;
			srcItemDesc.nItemColor		= -1;
			if ( g_CompoundRule.CheckItem(&Item[targetItemIndex], srcItemDesc) )
			{
				ret = true;
				break;
			}
		}

		if ( !ret )
			return ret;
		
		//分期付款Target附加检测
		if (item.GetItemTemplate() && item.GetItemTemplate()->nMaxFlushTimes == -2 ) //使用的物品是分期付款性石头
		{
			if (Item[targetItemIndex].GetGenre() == item_ib
            #ifdef _SERVER
				|| Item[targetItemIndex].GetIBGuid() != 0
            #endif

				) //不可是IB物品
				return false;
			
			if (Item[targetItemIndex].GetIBBuyData() == 0 ) //不可是已经买断的
				return false;
			
			if ( !Item[targetItemIndex].GetItemTemplate() || Item[targetItemIndex].GetItemTemplate()->nMaxFlushTimes <= 0) //必须是国战分期付款物品
				return false;
		}//endif
		
	}

#ifdef _SERVER
	//冷却
	if( item.GetCDTime() != -1 &&
		item.GetGroup() != -1 && 
		IsGroupCoolDown(item.GetGroup()))
	{
		return false;
	}
#endif

	return true;
}

int KItemList::CountItem(int genre, int detail, int particular, int level)
{
	int itemCount = 0;
	int index = 0;
	while(1)
	{
		index = m_UseIdx.GetNext(index);
		if (!index)
			break;
		
		int itemIndex = m_Items[index].nIdx;
		if (itemIndex > 0)
		{
			KItem& item = Item[itemIndex];
			if (item.GetGenre() == genre
				&& item.GetDetailType() == detail
				&& item.GetParticular() == particular
				&& item.GetLevel() == level)
			{
				itemCount += item.GetItemCount();
			}
		}
	}

	return itemCount;
}

int KItemList::importantItem(KItem* item)
{
	if(item == NULL)
	{
		return 0;
	}

	if(item->GetItemTemplate() == NULL)
	{
		return 0;
	}

// 	if(item->GetQualityLabel() == quality_common && item->GetYaoID() == yao_invalid)
// 	{
// 		return 0;
// 	}

// 	if(item->GetGenre() == item_medicine)
// 	{
// 		return 0;
// 	}
	
	return 1;
}

#ifdef _SERVER
void KItemList::SetupEquipBuffToNpc(int npcIndex)
{
	if (!IsValidNpc(npcIndex))
		return;

	m_BasicPropertyMonitor.SetupCurrentBuff(npcIndex);
	m_ArmorSetMonitor.SetupCurrentBuff(npcIndex);
	m_YaoMonitor.SetupCurrentBuff(npcIndex);
	m_TalismanMonitor.SetupCurrentBuff(npcIndex);
}
#endif