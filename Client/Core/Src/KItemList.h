#ifndef	KItemListH
#define	KItemListH

#include "ItemCommonDef.h"
#include "KItem.h"
#include "KItemSet.h"
#include "KLinkArray.h"
#include "LinkStruct.h"
#include "GameDataDef.h"
#include "KInventory.h"
#include "Yao_Monitor.h"
#include "ArmorSet_Monitor.h"

#ifdef _SERVER
#include "BasicProperty_Monitor.h"
#include "charm_monitor.h"
#else
#include "Abrade_Monitor.h"
#endif

#include "talisman_monitor.h"

#define	MAX_ITEM_ACTIVE	2

#define	REQUEST_EQUIP_ITEM		1
#define	REQUEST_EAT_MEDICINE	2

#define TREASUREITEMCLASS		4
#define TREASUREDETAIL			46
#define TREASUREPARTI			1
#define TREASURELEVEL			1
#define TREASUREPILECOUNT		1

#define TREASUREDETAIL_NEW		157
#define TREASURE_OLD			0
#define TREASURE_NEW			2

#define COPPERCASHITEMCLASS		4
#define COPPERCASHDETAIL		47
#define COPPERCASHPARTI			1
#define COPPERCASHLEVEL			1
#define COPPERCASHPILECOUNT		100

#define _LOGITEMTYPE_			88888
#define ITEMCLASS_SAPPHIRE		3
#define ITEMDETAIL_SAPPHIRE		41
#define ITEMCLASS_YUANBAO		4
#define ITEMDETAIL_YUANBAO		46
#define ITEMCLASS_TONGQIAN		4
#define ITEMDETAIL_TONGQIAN		47

#define STORAGEBOXGUID			0xFFFF

enum
{
	enable_trade = 1,
	enable_gamble = 2,
};

struct KBesetMap
{
	BYTE				byX;
	BYTE				byY;
};

struct KEquipInsurance
{
	time_t				mInsuranceTimeStamp;		// 上一次保险的时间戳
};

typedef void (*PCoolDownCompleteCallback)(void*);

struct KItemGroupCD 
{
	KItemGroupCD()
	{
		nGroup = -1;
		ulCDTime = 0;
		ulStartTime = 0;
		pParam = NULL;
		pComplete = NULL;
	}

	int nGroup;
	unsigned long ulCDTime;
	unsigned long ulStartTime;
	void* pParam;
	PCoolDownCompleteCallback pComplete;
};

class KItemList
{	
	friend	class KPlayer;
	friend	class KProtocolProcess;
public:
	KItemList();
	~KItemList();
public:
	
#ifndef _SERVER
	AbradeMonitor&				GetAbradeMonitor() { return m_AbradeMonitor; }
#endif

	int							Init(int nIdx);
	KInventory*					GetRoom(INVENTORY_ROOM room)
	{
		if (room >= 0 && room < room_num)
			return &m_Room[room];
		else
			return NULL;
	}
	inline	int					GetEquipment(int nIdx);
	ArmorSetMonitor&			GetArmorSetMonitor();																						// 获得套装监视器
	YaoMonitor&					GetYaoMonitor();// 获得爻装监视器
	TalismanMonitor&			GetTalismanMonitor();// 获得法宝监视器
	int 						GetWeightTaken(void);																						// 获得当前携带物品重量
	inline int					GetSpecificItemIndexArraySize( int nRoomType, int nGenre, int nDetail, int nParticular );
	inline void					GetSpecificItemIndexArray( int nRoomType, int nGenre, int nDetail, int nParticular, int *pItemCountArray, int nArraySize );
	inline int					GetMarkPrice(int nObjIdx);
	inline BOOL					SetMarkPrice(int nObjIdx, int nPrice);
	inline int					GetBoxSize(INVENTORY_ROOM enBoxType, enBOXSIZETYPE enSizeType);
	ITEM_PART					GetEquipPlace(EQUIPDETAILTYPE nType);																					// 取得某类型装备应该放的位置
	bool						IsPosEmpty(int nPlace, int nX, int nY);
	BOOL						FindSameParticularItem(int nGenre, int nDetail, int nParticular, int *pnIdx);
	BOOL						FindSameParticularInEquipment(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY);	// 在room_equipment中查找指定Genre和DetailType的物品，得到ItemIdx和位置	
	BOOL						FindLockedParticularItem(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY);
	int							GetWeaponLevel();
	enWEAPONATTRTYPE			GetWeaponAttrType();																						// Get weapon attribute type, main using by cast skill
	void						GetWeaponDamage(int* nMin, int* nMax);																		// 取得武器的伤害
	int							Add(int itemIndex, int place, int x, int y, int *pnOutIndex = NULL, enumItemSyncType syncType = item_sync_type_normal);
	int							Add(int itemIndex, enumItemSyncType syncType = item_sync_type_normal);
	int							getItemOfTypeCount(ITEM_POSITION pos, ItemType type, int** itemIndexes);
	bool						removeItemOfTypeCount(ITEM_POSITION pos, ItemType type, int count);
	BOOL						Remove(int nIdx, int nRemoveCount = 0, bool syncToClient = true, bool removeFromItemSet = true, bool checkTrade = true, bool bUseIbItem = true);										// nIdx指游戏世界中道具数组的编号
	void						RemoveAll();
	bool						canEquip(int equipItemIndex, ITEM_PART equipRoomPos);																		// nIdx指游戏世界中道具数组的编号
	bool						canEquip(ItemPos* sourPos, ITEM_PART equipRoomPos);
	bool						Equip(const ItemPos* itemPos, ITEM_PART equipPos = itempart_unidentified);	
	bool						unEquip(const ITEM_PART equipPos, const ItemPos* unEquipPos);
	BOOL						UnEquip(int nIdx, int nPlace = -1);																			// nIdx指游戏世界中道具数组的编号
	BOOL						EatMecidine(int nIdx, int nTargetIdx);																						// nIdx指游戏世界中道具数组的编号
	PlayerItem*					GetFirstItem();
	PlayerItem*					GetNextItem();
	int							SearchID(int nID);
	int							GetItemIdx(int nID);
	inline int					GetGlobalIdx(int nIdx);
	void						ExchangeMoney(int nSrcRoom, int DesRoom, int nMoney);
	void                        CheckEquipmentExpireTime();

	void						exchangeItem(const ItemPos* sourPos, const ItemPos* destPos);
	bool						exchangePrecheck(const ItemPos* sourPos, const ItemPos* destPos);
	bool						doExchangeItem(INVENTORY_ROOM sourBox, const ItemPos* sourPos, INVENTORY_ROOM destBox, const ItemPos* destPos);
	void						onExchangeDone(const ItemPos* sourPos, const ItemPos* destPos);

	void						splitItem(ItemPos* sourPos, ItemPos* destPos, int splitItemCount);
	void						doSplitItem(INVENTORY_ROOM sourBox,  ItemPos* sourPos, INVENTORY_ROOM destBox, ItemPos* destPos, int moveItemCount);
	
	static INVENTORY_ROOM		clientContainer2CoreRoom(UIOBJECT_CONTAINER cont);
	static UIOBJECT_CONTAINER	corePos2ClientContainer(ITEM_POSITION pos);
	static INVENTORY_ROOM		corePos2coreRoom(ITEM_POSITION pos);
	static ITEM_POSITION		coreRoom2corePos(INVENTORY_ROOM room);
	
	int							GetMoneyAmount();																							// 得到物品栏和储物箱的总钱数
	int							GetEquipmentMoney();																						// 得到物品栏和储物箱的钱数
	BOOL						AddMoney(int nRoom, int nMoney);
//	BOOL						CostMoney(int nMoney);
//	BOOL						DecMoney(int nMoney);
	void						SetMoney(int nMoney1, int nMoney2, int nMoney3);
	void						SetRoomMoney(int nRoom, int nMoney);
	inline	int					GetMoney(int nRoom);																						// 取得空间的钱
	void						SetPlayerIdx(int nIdx);																						// 设定玩家索引
	void						ClearRoom(int nRoom);
	void                        ClearRoomItemOnly(int nRoom);
	void						StartTrade();
	int							SetItemMask(int nNpcIdx, int nPart, BOOL nMask, BOOL bOnOrOffHorse = FALSE);								//需要对上下马做一些特例,注意，仅在上马或下马的时候将bOnOrOffHorse置为TRUE
	KItem*						GetItemFromPlace(ItemPos* pPos);
	inline	BOOL				GetItemPos(int nGameIndex, ItemPos * pItemPos);
	BOOL						SearchPosition(ItemPos* pPos, PEXTRAINFOPLUS pExtraInfo = NULL);	
	void						OnEquipChanged();//穿上的装备发生变化
	void						OnBagChanged(bool isCharm = false);//包裹发生变化（参数isCharm：是否护身符）
	int							bagExtend(int itemIndex, INVENTORY_ROOM room);	//判断物品是否可扩展相应容器的大小
	void						onBagSized(INVENTORY_ROOM room);				//容器大小变化后的处理

	BOOL						HaveItemEquiped	(	int nItemDetail, int nItemParticular, int nItemLevel	);//某个装备是否正被穿在身上
	BOOL						IsTaskItemExist	(	int nDetailType		);//在人物身上的所有格子中查找是否有这个DetailType指定的任务物品 建议使用 IsExistItem代替
	inline int					IsExistItem		(	int nItemClass, int nDetailType, int nParticualrType, int nLevel	);//在人物身上的所有格子中查找是否有指定的道具
	inline int					IsExistItemIB		(	int nItemClass, int nDetailType, int nParticualrType, int nLevel	);//在人物身上的所有格子中查找是否有指定的道具
	inline int					HaveNormalItem	(	int nItemClass,int nDetailType,int nParticualrType,int nLevel	);//在道具栏的格子中查找是否有指定的道具
	inline int					HaveNormalItemByID(	DWORD dwItemID	);//在道具栏的格子中查找是否有指定的道具
	inline int					FindNormalItem	(	int nItemClass,int nDetailType,int nParticualrType,int nLevel );

	
	BOOL						IsItemInEquip	(	int nItemId		) const;
	bool						isEquipHaveItem	(	ITEM_PART nEquip	);
	void						MaskEquipment(ITEM_PART itemPart, bool isMasked);
	bool						IsEquipmentMasked(ITEM_PART itemPart);
	void						RemoveAllInOneRoom(int nRoom);
#ifdef	_SERVER
	bool						syncOppositeItem(const ItemPos* itemPos);
	bool						syncSelfItem(ITEM_POSITION place, int x, int y);
	static bool					syncItemToOther(int playerIndex, int itemIndex);
	BOOL						WastePlaceItem(int nItemClass, int nItemDetail,int nItemParti, int nItemLevel, int nWasteCount);
	void						SetEquipInsuranceTime(int equipPart, time_t second);
	void						SetEquipInsuranceTimeLast(int equipPart, time_t second);
	int							ClearAllInvalidItem();																						// 把返回值由void改为int，表示被删除的元宝个数
	int							GetInvalidItemCount();
	bool						sycnOppositeItem(ItemPos* itemPos);	
	void						TradeMoveMoney(int nMoney);																					// 调用此接口必须保证传入的nMoney是一个有效数(正数且不超过所有钱数)
	void						SendMoneySync();																							// 服务器发money同步信息给客户端
	int							GetTaskItemNum(int nDetailType);
	int							GetTaskItemNumEx(int nDetailType);
	BOOL						RemoveTaskItem(int nDetailType);
	int							GetTradeRoomItemInfo();																						// 交易中把 trade room 中的 item 的 idx width height 信息写入 itemset 中的 m_psItemInfo 中去，返回值表明交易物品最好装备的类型号
	BOOL						TradeCheckCanPlace();																						// 交易中判断买进的物品能不能完全放进自己的物品栏
	BOOL						CheckCanPlaceInEquipment(int *pnX, int *pnY);																// 判断一定长宽的物品能否放进物品栏
	BOOL						EatMecidine(int nPlace, int nX, int nY, int nTargetItemID, int nTargetPlace, int nTargetX, int nTargetY);																	// 吃什么地方的药
	inline BOOL					IsLockStorageBox();
	inline void					LockStorageBox(BOOL bLock = TRUE);
	inline int					GetFirstMatchNormalItem(int nItemClass,int nDetailType,int nParticualrType);
	inline int					GetFirstMatchMagictem(int nItemClass,int nDetailType,int nParticualrType);
	BOOL						DelNormalItemByID(int nItemID);
	BOOL						DelNormalItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel);
	BOOL						DelNormalItemOnlyItemBox(int nItemClass,int nDetailType,int nParticualrType,int nLevel, bool checkLevelupTimes = true );
	BOOL						DelValidIBItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel);
	BOOL						DelMagicItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel);
	int							DelStatckItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel, int nStatckCount );

	void						Abrade(AbradeMode mode);//装备磨损

	void						AddShortCut( DWORD dwPos, DWORD dwID, DWORD dwType );
	void						DelShortCut( DWORD dwPos );
	void						SyncShortCut( );
	KImmediacyParam*			GetShortCut( void ) const;
	bool						IsGroupCoolDown( int nGroup );
	void						AddGroupCoolDown( int nGroup, unsigned long ulTime, void* pParam = NULL, PCoolDownCompleteCallback pCompleteCallback = NULL );
	void						DelGroupCoolDown( int nGroup );
	void						LoopGroupCoolDown( );
	void						SyncGroupCDAdd( int nGroup, unsigned long ulTime );
	void						SyncGroupCDDel( int nGroup );	
	int							GetPlayerIndex();
	void						SetupEquipBuffToNpc(int npcIndex);//把装备BUFF添加到其他NPC上
#else
	int							UseItem(int nIdx, int nTargetIdx);																							// nIdx指游戏世界中道具数组的编号
	BOOL						AutoMoveItem(ItemPos SrcPos,ItemPos DesPos);
	void						MenuSetMouseItem();
	void						LockOperation();																							// 锁定客户端对装备的操作
	void						UnlockOperation();
	inline	BOOL				IsLockOperation();
	int							GetSameParticularItemNum(int nImmediatePos);																// 忘记什么
	void						ClearBesetRoom();																							// 在记忆中找寻一个
	BOOL						FindPlacePos(POINT* pPos);
#endif
	
private:
	int							FindFree();
	int							FindSame(int nGameIdx);																						// nGameIdx指游戏世界中道具数组的编号
	bool						equipPosCheck(int nIdx, ITEM_PART nPlace);	
	BOOL						FindNextSameparticularItem(int nIdx, int nGenre, int nDetail, int nParticular, int *pnIdx);

#ifdef _SERVER	
	static void					EnableTalismanCallback(void* pParam);
#endif

public:	
	KEquipState					m_EquipItem[itempart_num];																					// 玩家装备的道具（对应游戏世界中道具数组的索引）
	BOOL                        m_EquipOverdataState[itempart_num];
	PlayerItem					m_Items[MAX_PLAYER_ITEM];																					// 玩家拥有的所有道具（包括装备着的和箱子里放的，对应游戏世界中道具数组的索引）
	PlayerItem					m_sBackItems[MAX_PLAYER_ITEM];																				// 交易过程中 m_Items 的备份
	KLinkArray					m_FreeIdx;
	KLinkArray					m_UseIdx;
	int							m_nListCurIdx;																								// 用于 GetFirstItem 和 GetNextItem
	KEquipInsurance				mEquipInsurance[itempart_num];																				// 为安装每种装备的孔保险
	KInventory					m_Room[room_num];	
	KImmediacyParam				m_ShortCut[MAX_IMMEDIACY_ITEM];

#ifndef _SERVER
	KItem						m_clientTempItem[MAX_TEMP_ITEM];
#endif

#ifdef _SERVER
	KItemGroupCD				m_GroupCD[MAX_REPOSITORY_ITEM];
#else
	BOOL						m_bLockOperation;																						// 记忆
#endif

private:
	int							m_PlayerIdx;
	int							m_nBackHand;
	int							m_nWeightTaken;																								// 当前负重
	YaoMonitor					m_YaoMonitor;
	ArmorSetMonitor				m_ArmorSetMonitor;
#ifdef _SERVER
	BasicPropertyMonitor		m_BasicPropertyMonitor;
	CharmMonitor				m_CharmMonitor;
#else	
	AbradeMonitor				m_AbradeMonitor;	
#endif
	TalismanMonitor				m_TalismanMonitor;

	//储物箱的特殊情况（在服务器端要记录是否已经打开储物箱，没有打开是不允许交换物品的）
#ifdef _SERVER
private:
	bool						m_storeBoxOpened;
public:
	bool						isStoreBoxOpended(){	return m_storeBoxOpened;	}
	void						openStoreBox()		{	m_storeBoxOpened = true;	}
	void						closeStoreBox()		{	m_storeBoxOpened = false;	}	
#endif

public:
	bool CanUseItem(int itemIndex, int targetItemIndex = -1);//判断是否可以使用物品
	int CountItem(int genre, int detail, int particular, int level);//数出指定编号的物品的个数

	static int importantItem(KItem* item);
};

inline	int KItemList::GetEquipment(int nIdx)
{ 
	return m_EquipItem[nIdx].nEquipIdx; 
}

inline	int	KItemList::GetMoney(int nRoom)
{ 
	return m_Room[nRoom].GetMoney(); 
}	

inline int	KItemList::GetSpecificItemIndexArraySize( int nRoomType, int nGenre, int nDetail, int nParticular )
{
	int nRet = 0;
	for ( int x = 0; x < m_Room[nRoomType].getWidth(); ++x )
	{
		for ( int y = 0; y < m_Room[nRoomType].getHeight(); ++y )
		{
			int nItemIdx = m_Room[nRoomType].FindItem( x, y );
			if ( nItemIdx > 0 && Item[nItemIdx].GetParticular() == nParticular &&
				Item[nItemIdx].GetGenre() == nGenre && 
				Item[nItemIdx].GetDetailType() == nDetail )
			{
				++nRet; 
			}
		}
	}
	return nRet;
}

inline	void	KItemList::GetSpecificItemIndexArray( int nRoomType, int nGenre, int nDetail, int nParticular, int *pItemCountArray, int nArraySize )
{
	int nIdx = 0;
	if ( pItemCountArray == NULL )
	{
		return;
	}
	for ( int x = 0; x < m_Room[nRoomType].getWidth(); ++x )
	{
		for ( int y = 0; y < m_Room[nRoomType].getHeight(); ++y )
		{
			int nItemIdx = m_Room[nRoomType].FindItem( x, y );
			if ( nItemIdx > 0 &&
				Item[nItemIdx].GetParticular() == nParticular &&
				Item[nItemIdx].GetGenre() == nGenre && 
				Item[nItemIdx].GetDetailType() == nDetail )
			{
				if ( nIdx >= 0 && nIdx < nArraySize )
				{
					pItemCountArray[nIdx] = nItemIdx;
					++nIdx;
				}					
			}
		}
	}		
}

inline	int	KItemList::GetMarkPrice(int nObjIdx)
{
	int nIndex = FindSame(nObjIdx);
	if (nIndex)
		return m_Items[nIndex].nItemPrice;
	return PlayerItem::INVALIDPRICE;
}

inline	BOOL	KItemList::SetMarkPrice(int nObjIdx, int nPrice)
{
	int nIndex = FindSame(nObjIdx);
	if (nIndex)
	{
		m_Items[nIndex].nItemPrice = nPrice;
		return TRUE;
	}
	return FALSE;
}

inline	int	KItemList::GetBoxSize(INVENTORY_ROOM enBoxType, enBOXSIZETYPE enSizeType)
{
	return m_Room[enBoxType].GetInventorySize(enSizeType);
}

inline enWEAPONATTRTYPE KItemList::GetWeaponAttrType()// Get weapon attribute type, main using by cast skill
{
	if (m_EquipItem[itempart_weapon].nEquipIdx != 0 && !(m_EquipItem[itempart_weapon].IsMasked))
		return Item[m_EquipItem[itempart_weapon].nEquipIdx].GetWeaponAttrType();
	else
		return weaponattr_none;
}

inline int	KItemList::GetGlobalIdx(int nIdx)
{
	return m_Items[nIdx].nIdx;
}

inline int KItemList::FindNormalItem (int nItemClass,int nDetailType,int nParticualrType,int nLevel )
{
	int nX, nY, nIdx;

	m_Room[room_equipment].FindNormalItem(
		nItemClass, nDetailType, nParticualrType, nLevel, &nX, &nY, &nIdx );

	return nIdx;
}


inline int KItemList::HaveNormalItem(int nItemClass,int nDetailType,int nParticualrType,int nLevel)
{
	return m_Room[room_equipment].HaveNormalItem(nItemClass, nDetailType, nParticualrType, nLevel, FALSE);
}

inline int KItemList::HaveNormalItemByID(	DWORD dwItemID	)//在道具栏的格子中查找是否有指定的道具
{
	DWORD nItemID = SearchID(dwItemID);
	if ( nItemID > 0 && nItemID < MAX_ITEM  )
	{
		return HaveNormalItem( Item[nItemID].GetGenre(), Item[nItemID].GetDetailType(), Item[nItemID].GetParticular(), Item[nItemID].GetLevel() );
	}

	return FALSE;
}

inline	BOOL KItemList::GetItemPos(int nGameIndex, ItemPos * pItemPos)
{
	_ASSERT(pItemPos);
	if (!pItemPos)
		return FALSE;

	int nIndexInList = FindSame(nGameIndex);
	if (nIndexInList)
	{
		pItemPos->nPlace = m_Items[nIndexInList].nPlace;
		pItemPos->nX = m_Items[nIndexInList].nX;
		pItemPos->nY = m_Items[nIndexInList].nY;
		return TRUE;
	}
	return FALSE;
}

inline int KItemList::IsExistItem(int nItemClass, int nDetailType, int nParticualrType, int nLevel)
{
	int nCount = 0;

	nCount = m_Room[room_equipment].HaveNormalItem(nItemClass, nDetailType, nParticualrType, nLevel, FALSE);
	nCount += m_Room[room_repository].HaveNormalItem(nItemClass, nDetailType, nParticualrType, nLevel, FALSE);

	return nCount;
}

inline int KItemList::IsExistItemIB(int nItemClass, int nDetailType, int nParticualrType, int nLevel)
{
	int nCount = 0;
	
	nCount = m_Room[room_equipment].HaveNormalItemIB(nItemClass, nDetailType, nParticualrType, nLevel, FALSE);
	nCount += m_Room[room_repository].HaveNormalItemIB(nItemClass, nDetailType, nParticualrType, nLevel, FALSE);
	
	return nCount;
}


inline ArmorSetMonitor& KItemList::GetArmorSetMonitor()
{
	return m_ArmorSetMonitor;
}

inline YaoMonitor& KItemList::GetYaoMonitor()
{
	return m_YaoMonitor;
}

inline TalismanMonitor& KItemList::GetTalismanMonitor()
{
	return m_TalismanMonitor;
}

#ifndef _SERVER
	inline BOOL KItemList::FindPlacePos(POINT* pPos)
	{
		return m_Room[room_equipment].FindRoom(pPos);
	}

	inline	void	KItemList::ClearBesetRoom()
	{
		m_Room[room_beset].Clear();
	}

	inline	BOOL	KItemList::IsLockOperation()
	{ 
		return m_bLockOperation; 
	}

#else
	inline BOOL	KItemList::IsLockStorageBox()
	{
		return m_Room[room_repository].IsBoxLocked();
	}

	inline void		KItemList::LockStorageBox(BOOL bLock)
	{
		m_Room[room_repository].LockBox(bLock);
	}

	inline int KItemList::GetFirstMatchNormalItem(int nItemClass,int nDetailType,int nParticualrType)
	{
		return m_Room[room_equipment].GetFirstMatchItem(nItemClass, nDetailType, nParticualrType, FALSE);
	}

	inline int KItemList::GetFirstMatchMagictem(int nItemClass,int nDetailType,int nParticualrType)
	{
		return m_Room[room_equipment].GetFirstMatchItem(nItemClass, nDetailType, nParticualrType, TRUE);
	}

	inline int KItemList::GetPlayerIndex()
	{
		return m_PlayerIdx;
	}
#endif

#endif
