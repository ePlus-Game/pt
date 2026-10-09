//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 11/10/2006 17:54
//      File_base        : KItem
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi) Lucifer~yu (Zhang jian yu)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef	KItemH
#define	KItemH

#include <vector>
#include "KNpc.h"
#include "KTabFile.h"
#include "KBasPropTbl.h"
#include "GameDataDef.h"
#include "ItemCommonDef.h"
#include "kitemsocketset.h"

#define LOCK_BY_DATA_SIGN  0xffffffff

#ifdef _SERVER
#define MAX_ITEM 200000
#else

#include "KRepresentUnit.h"
#define MAX_ITEM 512

#endif

#define PLAYER_TARGET "-2|-2|-2|-2|-2|-2|-2"
#define NO_TARGET "-3|-3|-3|-3|-3|-3|-3"
#define TARGETITEM_TARGET_COUNT 6
#define	MAX_ADD_TALISMAN_POTENTIAL		1000000000		//添加法宝蕴魂上限
#define	MAX_TALISMAN_POTENTIAL			3000000000		//法宝蕴魂上限


#define MAPAREA_TARGET_COUNT 6
#define NO_MAPAREA_LIMIT "-1|-1|-1|-1|-1|-1"

class KNpc;
class KPlayer;
class KIniFile;

enum enumItemAttribute
{
	item_attr_durability,			//当前耐久
	item_attr_max_durability,		//耐久上限
	item_attr_stack_count,			//堆放数量
	item_attr_yao_id,				//爻属性（阳爻，阴爻）
	item_attr_upgrade_count,		//升级次数
	item_attr_talisman_potential,	//法宝蕴魂
	item_attr_talisman_enchase,		//法宝镶嵌
	item_attr_isbind,				//是否已绑定
	item_attr_islocked,				//是否锁定
	item_attr_buytime,
	item_attr_credit_flag,          //信贷标识
	item_attr_step,         
	item_attr_mapid,        
	item_attr_mapx,         
	item_attr_mapy,         
	item_attr_is_task_given,        //是否是任务给于
	item_attr_lockdate,        //是否是任务给于
	item_attr_upgrade_type,		//升级次数
	item_attr_upgrade_buffid,		//升级次数
	item_attr_flush_times,      //国战装备租用次数
	item_attr_count
};

enum enumItemUseType
{
	item_normal_use,
	item_step_use,
	item_cannt_del_use,
};

class KItem
{
public:
	KItem();
	~KItem();
	KItem( const KItem& rItem );

public:
	bool					IsMapArea( int nPlayerIdx );
	inline void				SetStep( BYTE nStep );
	inline int				GetStep( void );
	inline void				SetPosInfo( int mapID, int mapX, int mapY );
	inline void				GetPosInfo( int& mapID, int& mapX, int& mapY );
	inline void				SetItemIndex( int index );
	inline int				GetItemIndex( void );
	inline bool				IsNormalUseItem( void );
	inline bool				IsStepUseItem( void );
	inline bool				CanntDisappear( void );
	bool					IsInitWeapon( void ) const;
	int						GetItemQuality( void ) const;//得到物品品质
	ITEM_QUALITY_LABEL		GetQualityLabel( void ) const;//获取物品品质标签
	bool					CanDiscard( void ) const;//是否可以丢弃（销毁）
	bool					CanSell( void ) const;//是否可以出售（卖给系统商人）
	bool					CanExchange( void ) const;//是否可以交易（玩家之间交易和拍卖行出售）
	enumItemDeathDropType	GetDeathDropType( void ) const;//得到物品死亡掉落类型
	bool					IsUnique( void ) const;//是否唯一
	int						GetRestrictCount( void ) const;//限制拥有数量
	bool					IsBind( void ) const;//是否绑定
	void					SetBind( bool bind );//设置是否绑定
	BOOL					CanBeRepaired( void );
	int						GetDropFlag( void ) const;
	enWEAPONATTRTYPE		GetWeaponAttrType( void ) const;
	ennWEAPONPOSITIONTPE	GetWeaponPosReq( void ) const;
	int 					GetItemCount( void ) const;//得到重叠个数
	int 					GetMaxItemCount( void ) const;//得到最大重叠个数
	void					SetItemCount( int itemCount );//设置重叠个数	
	int						GetItemWeight( void ) const;// 获得物品重量.
	void					SetID( DWORD dwID );
	DWORD					GetID( void ) const;
	EQUIPDETAILTYPE			GetDetailType( void ) const;
	ITEMGENRE				GetGenre( void ) const;
	int						GetParticular( void ) const;
	int						GetLevel( void ) const;
	int			 			GetPrice( void ) const;
	int						GetSellPrice( void ) const;
	char*					GetName( void ) const;
	int						GetObjIdx( void ) const;
	int						GetRes( void ) const;
	int						getRepairPrice(bool special = false) const;	
	int						GetGroup( void ) const;
	int						GetCDTime( void ) const;
	void					Remove( void );
	const ItemBasicBuff&	GetBasicBuffID( void ) const;		
	const WORD*				GetCompBuffTemplateSet( void ) const;
	void					SetCompBuffTemplateSet( const WORD* pCompBuffTemplateSet, int nSize ) const;
	BOOL					UseItem( int nPlayerIdx, int nItemIdx, int nTagetIdx );
	void					SetLevelupTimes( int nTimes );
	void					SetLevelupType( int nType );
	void					SetItemTemplate( const KBASICPROP_ITEM* pItemTemplate );
	KBASICPROP_ITEM*		GetItemTemplate() const;
	bool					IsSameParitcularItem( const KItem &DesItem ) const;
	bool					IsSameParitcularItem( int genre, int detailType, int particularType ) const;
	bool					IsBuffSwitch( void );
	
	void					SetLockDate( DWORD lockDate );
	DWORD					GetLockDate( void );

	void                    SetFlushTimes ( int nTimes );
	int                     GetFlushTimes ( void )const;

	bool					IsLockedByDate( int nConnectIdx );
	inline void				LockByDate( void );//锁定
	inline void				UnlockByDate( DWORD nLockTime );

	bool					IsLocked( int nConnectIdx, bool bDate = true );
	void					Lock();//锁定
	void					Unlock();//解锁定
	int						GetLockCount( void ) const;//得到锁定计数
	void					SetLockCount( int lockCount );//设置锁定计数
	int						GetActionTime() const;
	void					SetActionTime(int time);
	bool					CanCombine(const KItem& compareItem);//是否可以和目标物品合并
	bool					IsEquipBind() const;//是否装备绑定
	//IB相关
	bool					IsIBCountTimelimitItem( void );
	int						GetIBItemType( void );
	int						GetIBAvailabilityTime( void );
	inline void				SetIBUseCount( int count );
	inline int				GetIBCurUseCount( void );
	inline int				GetIBUseCount( void );
	int						GetGenerateItemHashId();
	void					SetIBBuyDate( DWORD dwData );
	DWORD					GetIBBuyData( void );	
	bool					NeedIBUse( void );
	bool					IsOverDate( void );
	bool					IsExpItem( void );//是否是经验物品
#ifdef _SERVER
	INT64					GetIBGuid( void );
	void					SetIBGuid( INT64 ibGuid );	
#endif
	

	//Target item 特有方法
	inline	bool			IsNoTarget( void ) const;
	inline bool				IsPlayerTarget( void ) const;
	inline bool				IsItemTarget( void ) const;
	//装备耐久相关
	int						GetMaxDurability( void ) const;
	void					SetMaxDurability( int maxDurability );
	void					SetDurability( IN const int nDur );
	int						GetDurability( void );
	int						Abrade( int count );//磨损（即耐久减少）（按数值），返回剩余耐久
	int						Abrade( float rate );//磨损（即耐久减少）（按比例），返回剩余耐久
	bool					IsBroken();//是否已经损坏
	//爻装套装相关
	int						GetArmorSetID( void ) const;//获得套装编号
	int						GetYaoID( void ) const;//获得爻属性编号
	void					SetYaoID( int yaoID );//设置爻属性编号
	int						GetYaoLevel( void ) const;//获得爻属性等级	
	int						GetYaoAddOn(int index) const;//获得爻装附加属性
	void					SetYaoAddOn(int index, int yaoAddOnBuffID);//设置爻装附加属性
	// 合成相关
	BYTE					GetLevelupTimes( void ) const;
	int						GetLevelupType( void ) const;
	void					AddCompoundBuff( int nCompoundType, int nLevelUpType, int nCompoundBuffTID );
	void					SetCompoundBuff( int nCompoundType, int nCompoundBuffTID );
	int						GetCompoundBuff( int nCompoundType ) const;
	void					ClearCompoundBuff( int nCompoundType );
	inline void				SetPlusInfo( char* szPlusInfo );
	inline char*			getPlusInfo( void );
	// 镶嵌相关
	int						RandomSocket( char* socketDesc );
	int						InlaySocket( const KBASICPROP_ITEM* itemTemplate, short buffID );
	inline void				CreateSocket( void );
	inline void				ClearSocketSet( void );
	inline void				SetSocketSet( const InlayStuff& inlay );
	inline void				SetInlayBaseBuffSet( short* inLayBaseBuff );
	inline void				SetInlayYaoBuffSet( short* inLayYaoBuff );
	inline void				SetInlaySpecialBuffSet( short* inLaySpecialBuff );
	inline void				GetInlayStuffBySocketIdx( int socketIdx, InlayStuff& stuff );
	inline short*			GetInlayBaseBuffSet( void );
	inline short*			GetInlayYaoBuffSet( void );
	inline short*			GetInlaySpecialBuffSet( void );
	inline int				GetUseSocketCount( void );
	inline int				GetMaxSocketCount( void );
	//装备需求相关
	bool					IsMatchProfessionRequirement(KPlayer& player);
	bool					IsMatchPropertyRequirement(KPlayer& player, bool* requirementMatched = NULL);
	bool					IsMatchLevelRequirement(KPlayer& player);
	int						GetProfessionRequirement(int* requirement);
	int						GetPropertyRequirement(int* requirement);
	int						GetLevelRequirement();
	void					GetItemtransfersData(TItemtransfersData& rData );
	void					SetItemtransfersData( const TItemtransfersData& rData );

#ifdef _SERVER
	static int				GetItemtransfersData( TItemtransfersData* pData, const char* pItemData, int dataSize );
	static void				ParseItemtransfersDataVersion1( TItemtransfersData* pData, const char* pItemData, int dataSize );
#endif

	//法宝相关
	int						GetTalismanLevel() const;//获得法宝等级
	DWORD					GetTalismanPotential() const;//获得法宝蕴魂
	void					SetTalismanPotential(DWORD potential);//设置法宝蕴魂
	void					AddTalismanPotential(DWORD potentialAdded);//添加法宝蕴魂
	DWORD					GetTalismanPotentialLimit() const;//获得法宝蕴魂上限
	int						GetTalismanEnchase(int index) const;//获得法宝镶嵌
	void					SetTalismanEnchase(int index, int enchaseId);//设置法宝镶嵌	
	int						GetTalismanCoolDown() const;//获得法宝冷却时间
	int						GetTalismanId() const;//获得法宝编号
	int						GetTalismanBuff() const;//获得法宝BUFF
	int						GetEnchaseId() const;//获得内丹编号
	int						GetEnchaseLevel() const;//获得内丹等级

	int						GetBelong(  )
							{ return m_nBelongIndex; }
	void					SetBelong( int nIndex )
							{ m_nBelongIndex = nIndex; }

#ifdef _SERVER
	int                     GetOBJBelong() { return m_ObjBelongIdx; }
	void                    SetOBJBelong(int nIndex) { m_ObjBelongIdx = nIndex; }
#endif

	bool					isDefaultItem();
	bool					isPickupBind();
	ITEM_IB_BUY_TYPE        GetAvailableIBBuyType(void);

	DWORD                   GetCreditFlag(void);
	void                    SetCreditFlag(const DWORD dwFlag);

	BOOL                    IsTaskGiven(void)const;
	void                    SetTaskGiven(BOOL bGiven);

#ifdef _SERVER
	void					SyncItem( int nNetConnectIdx, ItemPos& pos, enumItemSyncType syncType );
	void					SyncItemRefresh( int nNetConnectIdx );
	void					SetGenTime( DWORD dwTime );
	DWORD					GetGenTime( void ) const;
	void					SyncAttribute(enumItemAttribute attr, int connectionIndex);//同步属性
	int						GetLogLevel() const;
	const FSGUID&			GetGUID() const;
	void					SetGUID(const FSGUID& guid);
	void					SyncTalismanEnchase(int connectionIndex);
	void					GetItemTemplateId(ItemTemplateId& templateId) const;
	int						GetItemTemplateId(char* pTemplateIdStr, size_t buffSize) const;
	int						PrintItemInfo(char* pInfoStr, size_t buffSize) const;
	void					ItemErrCodeToClient(int nPlayerIdx, int nErrCode);
#else
	int						GetTargetWeaponType( int nIdx );
	char*					GetNameWithColor();
	void					GetDesc( char* descBuff ,bool bIsShopSpecial = false);
	static void				getEquipCompareTitalLayoutStyle( char* layoutText );
	const char*				GetImageSetFile( void ) const;
	const char*				GetImageFile( void ) const;
	void					RecvAttributeSync(BYTE attr, DWORD val);//收到属性同步
	void					RecvTalismanEnchaseSync(int* enchaseSet);
#endif

private:
#ifdef _SERVER	
	int						CastSkill( int nSkillID, int nPlayerIdx, int nTargetIdx );
	int						ExecuteScript(  int nPlayerIdx, const char* szScript );
#endif
	
private:
	KBASICPROP_ITEM*		m_pItemTemplate;							//基本属模板（从表里读出来的固定数据）
	enWEAPONATTRTYPE		m_enItemAttrType;							//暂时不需要
	ennWEAPONPOSITIONTPE	m_enItemPosReq;								//暂时不需要
	DWORD					m_GenerateTime;								//生成时间
	DWORD					m_ID;										//独立的ID，用于客户端与服务器端的交流	
	int						m_CurrentDurability;						//当前耐久度
	int						m_MaxDurability;							//耐久度上限
	int						m_ItemStackCount;							//叠放个数
	int						m_YaoID;									//爻属性编号
	int						m_YaoAddOnBuffIDSet[YAO_ADDON_BUFF_COUNT];	//爻装附加属性buff集合
	WORD					m_CompoundBuffIDSet[COMPOUND_COUNT];		//道具合成后添加的buff数组
	int						m_UpgradeType;								//道具升级类型
	int						m_UpgradeCount;								//道具升级次数
	char					m_szPlusInfo[ITEM_PLUS_INFO_LEN];
	DWORD					m_TalismanPotential;						//法宝当前蕴魂
	int						m_TalismanEnchaseSet[TM_HOLE_NUM];			//法宝镶嵌内丹集合
	bool					m_IsBind;									//是否已绑定
	int						m_LockCount;								//锁定计数（大于0时锁定，等于0时不锁定）
	int						m_ActionTime;								//动作时间
	int						m_nBelongIndex;								//物品所属NpcIndex
	KItemSocketSet			m_socketSet;										 // 镶嵌槽位
	short					m_InlayBaseBuffSet[MAX_INLAY_COUNT][ITEM_BUFF_COUNT];//灵石附加buff
	short					m_InlayYaoBuffSet[MAX_ITEM_INLAY_YAO_EFFECT_COUNT];//灵石爻附加buff
	short					m_InlaySpecialBuffSet[MAX_SPECIALEFFECT_COUNT];//灵石组合附加buff
	DWORD					m_dwIBBuyDate;
	DWORD                   m_CreditFlag;                                  //信贷标识
	DWORD					m_dwLockDate;									//锁定日期
	int                     m_FlushTimes;                                   //国战装备被充值的次数
	int						m_Index;
	int						m_MapID;
	int						m_MapX;
	int						m_MapY;
	BYTE					m_Step;
	BOOL                    m_IsTaskGiven;
#ifdef _SERVER
	FSGUID					m_GUID;										//GUID
	INT64					m_IBGUID;									// IBGUID
	int                     m_ObjBelongIdx;
#else 
	char					m_itemName[384];	
#endif
};

extern KItem *Item;

inline	bool KItem::IsPlayerTarget( void ) const
{
	if ( m_pItemTemplate && strcmp( m_pItemTemplate->szBuffTarget, PLAYER_TARGET ) == 0 )
	{
		return true;
	}
	return false;
}

inline	bool KItem::IsNoTarget( void ) const
{
	if ( m_pItemTemplate && strcmp( m_pItemTemplate->szBuffTarget, NO_TARGET ) == 0 )
	{
		return true;
	}
	return false;
}

inline	bool KItem::IsItemTarget( void ) const
{
	if ( IsPlayerTarget() )
	{
		return false;
	}
	return true;
}

inline const ItemBasicBuff& KItem::GetBasicBuffID() const
{
	return m_pItemTemplate->BasicBuff;
}

inline void KItem::SetItemTemplate( const KBASICPROP_ITEM* pItemTemplate ) 
{ 
	if ( pItemTemplate == NULL )
	{
		return;
	}
	m_pItemTemplate = (KBASICPROP_ITEM*)pItemTemplate; 
	m_enItemAttrType	= weaponattr_shortweapon;
	m_enItemPosReq		= weaponposreq_none;
	m_GenerateTime		= 0;					
	m_ID				= 0;							
	m_CurrentDurability	= 0;					
	m_MaxDurability		= 0;						
	m_ItemStackCount	= 0;					
	m_YaoID				= 0;							
	m_UpgradeCount		= 0;
	m_UpgradeType		= -1;
	m_szPlusInfo[0]		= 0;
	m_TalismanPotential = 0;
	m_IsBind			= false;
	::memset(m_TalismanEnchaseSet, -1, sizeof(m_TalismanEnchaseSet));
	::memset(m_YaoAddOnBuffIDSet, 0, sizeof(m_YaoAddOnBuffIDSet));
	::memset(m_CompoundBuffIDSet, 0, sizeof(m_CompoundBuffIDSet));
	::memset(m_InlayBaseBuffSet, 0, sizeof(m_InlayBaseBuffSet) );
	::memset(m_InlayYaoBuffSet, 0, sizeof(m_InlayYaoBuffSet) );
	::memset(m_InlaySpecialBuffSet, 0, sizeof(m_InlaySpecialBuffSet) );
#ifdef _SERVER
	RandomSocket(m_pItemTemplate->InlayDesc);
#endif	
}

inline KBASICPROP_ITEM* KItem::GetItemTemplate() const
{
	return m_pItemTemplate;
}

inline int KItem::GetArmorSetID() const
{
	return m_pItemTemplate->nEquiSetID;
}

inline int	KItem::GetYaoID() const
{
	return m_YaoID;
}

inline void KItem::SetYaoID(int yaoID)
{
	m_YaoID = yaoID;
}

inline int	KItem::GetYaoLevel() const
{
	return m_pItemTemplate->nQuality;
}

inline int KItem::GetYaoAddOn(int index) const
{
	if (index >= 0 && index < YAO_ADDON_BUFF_COUNT)
	{
		return m_YaoAddOnBuffIDSet[index];
	}
	else
	{
		return 0;
	}
}

inline void KItem::SetYaoAddOn(int index, int yaoAddOnBuffID)
{
	if (index >= 0 && index < YAO_ADDON_BUFF_COUNT)
	{
		m_YaoAddOnBuffIDSet[index] = yaoAddOnBuffID;
	}
}

inline BYTE KItem::GetLevelupTimes() const
{
	return m_UpgradeCount;
}

inline int KItem::GetLevelupType() const
{
	return m_UpgradeType;
}

inline void KItem::AddCompoundBuff(int nCompoundType, int nLevelUpType, int nCompoundBuffTID)
{
	m_CompoundBuffIDSet[nCompoundType] = nCompoundBuffTID;
	if ( nCompoundType == COMPOUND_LEVELUP )
	{
		m_UpgradeType = nLevelUpType;
		++m_UpgradeCount;
	}
}

inline void	KItem::SetCompoundBuff( int nCompoundType, int nCompoundBuffTID )
{
	if ( nCompoundType >= 0 && nCompoundType < COMPOUND_COUNT )
	{
		m_CompoundBuffIDSet[nCompoundType] = nCompoundBuffTID;
	}	
}


inline int KItem::GetCompoundBuff(int nCompoundType) const
{
	return m_CompoundBuffIDSet[nCompoundType];
}

inline void KItem::ClearCompoundBuff(int nCompoundType)
{
	if ( nCompoundType == COMPOUND_LEVELUP )
	{
		m_UpgradeCount	= 0;
		m_UpgradeType	= -1;
	}
	m_CompoundBuffIDSet[nCompoundType] = 0;
}

inline int KItem::GetMaxDurability() const
{
	return m_MaxDurability;
}

inline void KItem::SetMaxDurability( int maxDurability )
{
	if (maxDurability >= 0)
	{
		m_MaxDurability = maxDurability;
		if (m_CurrentDurability > m_MaxDurability)
		{
			SetDurability(m_MaxDurability);
		}
	}	
}

inline void	KItem::SetStep( BYTE nStep )
{
	m_Step = nStep;
}

inline int	KItem::GetStep( void )
{
	return m_Step;
}

inline void	KItem::SetPosInfo( int mapID, int mapX, int mapY )
{
	m_MapID =  mapID;
	m_MapX	=  mapX;
	m_MapY	=  mapY;
}

inline void	KItem::GetPosInfo( int& mapID, int& mapX, int& mapY )
{
	mapID = m_MapID;
	mapX = m_MapX;
	mapY = m_MapY;
}


inline void KItem::SetItemIndex( int index )
{
	m_Index = index;
}

inline int	KItem::GetItemIndex( void )
{
	return m_Index;
}

inline void KItem::SetDurability( IN const int nDur )
{
	if (nDur >= 0 && nDur <= m_MaxDurability)
	{
		m_CurrentDurability = nDur;
	}
}

inline int KItem::GetDurability()
{
	return m_CurrentDurability;
}

inline bool	KItem::IsNormalUseItem( void )
{
	if ( m_pItemTemplate && (m_pItemTemplate->UseType == item_normal_use || m_pItemTemplate->UseType == -1) )
	{
		return true;
	}
	else
	{
		return false;
	}
}


inline bool	KItem::IsStepUseItem( void )
{
	if ( m_pItemTemplate && m_pItemTemplate->UseType == item_step_use )
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool KItem::CanntDisappear( void )
{
	if ( m_pItemTemplate && m_pItemTemplate->nItemGenre == item_ib && m_pItemTemplate->UseType == item_cannt_del_use )
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool KItem::IsInitWeapon( void ) const
{
	if ( m_pItemTemplate == NULL )
	{
		return false;
	}

	if ( m_pItemTemplate->nItemGenre == item_equip && 
		m_pItemTemplate->nDetailType == equip_weapon  )
	{
		if ( strcmp( m_pItemTemplate->szReqPro, "1.1.0.0.0.0" ) == 0 ||
			strcmp( m_pItemTemplate->szReqPro, "0.0.1.1.0.0" ) == 0 ||
			strcmp( m_pItemTemplate->szReqPro, "0.0.0.0.1.1" ) == 0 )
		{
			return true;
		}
	}

	return false;
}

inline int KItem::GetItemQuality() const
{
	return m_pItemTemplate->nQuality;
}

inline enWEAPONATTRTYPE KItem::GetWeaponAttrType() const
{
	return m_enItemAttrType;
}

inline ennWEAPONPOSITIONTPE KItem::GetWeaponPosReq() const
{
	return m_enItemPosReq;
}

inline int KItem::GetItemCount() const
{
	return m_ItemStackCount;
}

inline int KItem::GetMaxItemCount() const
{
	return m_pItemTemplate->nStack;
}

inline void KItem::SetItemCount( int itemCount )
{
	m_ItemStackCount = itemCount;
}

inline int KItem::GetItemWeight() const
{
	if (GetMaxItemCount() > 1)
		return m_pItemTemplate->nItemWeight * GetItemCount();
	else
		return m_pItemTemplate->nItemWeight;
}

inline void KItem::SetID( DWORD dwID )
{
	m_ID = dwID;
}

inline DWORD KItem::GetID() const
{
	return m_ID;
}

inline EQUIPDETAILTYPE	KItem::GetDetailType() const
{
	return m_pItemTemplate->nDetailType;
}

inline ITEMGENRE KItem::GetGenre() const
{
	return m_pItemTemplate->nItemGenre;
}

inline int KItem::GetParticular() const
{
	return m_pItemTemplate->nParticularType;
}

inline int KItem::GetLevel() const
{
	return m_pItemTemplate->nLevel;
}

inline int KItem::GetPrice() const
{
	return m_pItemTemplate->nPrice;
}

inline int KItem::GetSellPrice() const
{
	return m_pItemTemplate->nSellPrice;
}

inline char* KItem::GetName() const
{
	return (char *)m_pItemTemplate->szName;
}

inline int KItem::GetObjIdx() const
{
	return m_pItemTemplate->nObjIdx;
}

inline int KItem::GetRes() const
{
	return m_pItemTemplate->nItemRes;
}

inline int KItem::GetGroup() const
{
	return m_pItemTemplate->nGroup;
}

inline int KItem::GetCDTime() const
{
	return m_pItemTemplate->nCDTime ;
}

inline ITEM_QUALITY_LABEL KItem::GetQualityLabel() const
{
	return m_pItemTemplate->nColor;
}

inline const WORD* KItem::GetCompBuffTemplateSet() const
{
	return m_CompoundBuffIDSet;
}

inline void KItem::SetCompBuffTemplateSet( const WORD* pCompBuffTemplateSet, int nSize ) const
{
	if (pCompBuffTemplateSet == NULL )
		return;

	memcpy((void*)m_CompoundBuffIDSet, (void*)pCompBuffTemplateSet, nSize);
}

inline void KItem::SetLevelupTimes( int nTimes )
{
	m_UpgradeCount = nTimes;
}

inline void KItem::SetLevelupType( int nType )
{
	m_UpgradeType = nType;
}

inline bool KItem::IsBroken()
{
	return (GetDurability() == 0);
}

inline void	KItem::SetPlusInfo( char* szPlusInfo )
{
	strncpy(m_szPlusInfo, szPlusInfo, ITEM_PLUS_INFO_LEN);
	m_szPlusInfo[ITEM_PLUS_INFO_LEN - 1] = 0;
}

inline char*	KItem::getPlusInfo( void )
{
	return m_szPlusInfo;
}

inline void	KItem::CreateSocket( void )
{
	m_socketSet.CreateSocket();
}

inline void	KItem::ClearSocketSet( void )
{
	::memset(m_InlayBaseBuffSet, 0, sizeof(m_InlayBaseBuffSet) );
	::memset(m_InlayYaoBuffSet, 0, sizeof(m_InlayYaoBuffSet) );
	::memset(m_InlaySpecialBuffSet, 0, sizeof(m_InlaySpecialBuffSet) );
	m_socketSet.DestroySocket();
}


inline void	KItem::SetSocketSet( const InlayStuff& inlay )
{
	m_socketSet.CreateSocket();
	m_socketSet.SetInlayStuffBySocketIdx( m_socketSet.GetUseSocketCount(), inlay );
}

inline void	KItem::SetInlayBaseBuffSet( short* inLayBaseBuff )
{
	if ( inLayBaseBuff )
	{
		memcpy( m_InlayBaseBuffSet, inLayBaseBuff, sizeof(m_InlayBaseBuffSet) );
	}
}

inline void	KItem::SetInlayYaoBuffSet( short* inLayYaoBuff )
{
	if ( inLayYaoBuff )
	{
		memcpy( m_InlayYaoBuffSet, inLayYaoBuff, sizeof(m_InlayYaoBuffSet) );
	}
}

inline void	KItem::SetInlaySpecialBuffSet( short* inLaySpecialBuff )
{
	if ( inLaySpecialBuff )
	{
		memcpy( m_InlaySpecialBuffSet, inLaySpecialBuff, sizeof(m_InlaySpecialBuffSet) );
	}
}

inline void	KItem::GetInlayStuffBySocketIdx( int socketIdx, InlayStuff& stuff )
{
	m_socketSet.GetInlayStuffBySocketIdx( socketIdx, stuff);
}

inline short* KItem::GetInlayBaseBuffSet(void)
{
	return (short*)m_InlayBaseBuffSet;
}

inline short* KItem::GetInlayYaoBuffSet(void)
{
	return (short*)m_InlayYaoBuffSet;
}

inline short* KItem::GetInlaySpecialBuffSet(void)
{
	return (short*)m_InlaySpecialBuffSet;
}

inline int	KItem::GetUseSocketCount( void )
{
	return m_socketSet.GetUseSocketCount();
}

inline int	KItem::GetMaxSocketCount( void )
{
	return m_socketSet.GetSocketCount();		
}

inline bool	KItem::IsBuffSwitch( void )
{
	return m_pItemTemplate->btBuffSwitch > 0 ? true: false;
}

inline bool KItem::IsEquipBind() const
{
	return m_pItemTemplate->EquipBind > 0 ? true : false;
}

inline bool KItem::IsIBCountTimelimitItem( void ) 
{
	if ( m_pItemTemplate == NULL )
	{
		return false;
	}
	if ( GetGenre() == item_ib &&
		GetIBItemType() == ib_item_time_count_limit )
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool	KItem::IsOverDate( void )
{
	DWORD dwCurTime = 0;
#ifdef _SERVER
	dwCurTime =	UNIX_TMIE_STAMP;
#else
	dwCurTime = ::time( NULL );
#endif
	if ( m_pItemTemplate && 
		GetIBItemType() > ib_item_now && 
		dwCurTime - GetIBBuyData() >= GetIBAvailabilityTime() && 
		dwCurTime >  GetIBBuyData() && GetIBBuyData() != 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline bool KItem::NeedIBUse( void )
{
#ifdef _SERVER
	if ( m_pItemTemplate && 
		GetIBItemType() == ib_item_time_count_limit && 
		GetIBUseCount() > 0 &&
		GetItemCount() == 0 && 
		GetCreditFlag() == 0 &&
		GetIBGuid() != 0)
	{
		return true;
	}
	else
#endif
	{
		return false;
	}
}

inline int	KItem::GetIBItemType()
{
	if ( m_pItemTemplate == NULL )
	{
		return 0;
	}
	return m_pItemTemplate->IBType;
}

inline int	KItem::GetIBAvailabilityTime()
{
	if ( m_pItemTemplate == NULL )
	{
		return 0;
	}
	return m_pItemTemplate->IBLiveTime;
}

inline	void	KItem::SetIBUseCount( int count )
{
	if ( m_pItemTemplate && 
		m_pItemTemplate->nItemGenre == item_ib &&
		m_pItemTemplate->IBType	== ib_item_time_count_limit &&
		m_pItemTemplate->IBUseCount > 0 )
	{
		m_ItemStackCount = count;
	}
}

inline int	KItem::GetIBCurUseCount( void )
{
	if ( m_pItemTemplate && 
		m_pItemTemplate->nItemGenre == item_ib &&
		m_pItemTemplate->IBType	== ib_item_time_count_limit &&
		m_pItemTemplate->IBUseCount > 0 )
	{
		GetItemCount();
	}
	return 0;
}

inline int	KItem::GetIBUseCount()
{
	if ( m_pItemTemplate == NULL )
	{
		return 0;
	}

	return m_pItemTemplate->IBUseCount;
}

inline int KItem::GetGenerateItemHashId()
{
	if ( m_pItemTemplate == NULL )
	{
		return 0;
	}
	return 	GenerateItemHashId( m_pItemTemplate->nItemGenre, m_pItemTemplate->nDetailType, m_pItemTemplate->nParticularType );
}

inline void	KItem::SetIBBuyDate( DWORD dwData )
{
	if ( GetIBItemType() > 0  )
	{
		m_dwIBBuyDate = dwData;
	}	
	else
	{
		m_dwIBBuyDate = 0;
	}
}

inline bool KItem::isPickupBind()
{
	if ( m_pItemTemplate )
	{
		return m_pItemTemplate->PickupBind > 0 ? true : false;
	}
	else
	{
		return 0;
	}
}


inline DWORD KItem::GetIBBuyData( void )
{
	return m_dwIBBuyDate;
}

inline bool KItem::IsExpItem( void )
{
	return (TRUE == m_pItemTemplate->IsExpItem);
}

#ifdef _SERVER

inline INT64 KItem::GetIBGuid( void )
{
	return m_IBGUID;
}

inline void KItem::	SetIBGuid( INT64 ibGuid )
{
	m_IBGUID = ibGuid;
}

#endif

inline int KItem::GetTalismanLevel() const
{
	return m_pItemTemplate->nLevel;
}

inline DWORD KItem::GetTalismanPotential() const
{
	return m_TalismanPotential;
}

inline void KItem::SetTalismanPotential(DWORD potential)
{
	m_TalismanPotential = potential;
}

inline void KItem::AddTalismanPotential(DWORD potentialAdded)
{
	if (potentialAdded == 0 || potentialAdded > MAX_ADD_TALISMAN_POTENTIAL || m_TalismanPotential > MAX_TALISMAN_POTENTIAL)
		return;

	DWORD tempPotential = m_TalismanPotential + potentialAdded;
	DWORD potentialLimit = GetTalismanPotentialLimit();
	m_TalismanPotential = (tempPotential < potentialLimit) ? tempPotential : potentialLimit;

	if (m_TalismanPotential > MAX_TALISMAN_POTENTIAL)
		m_TalismanPotential = MAX_TALISMAN_POTENTIAL;
}

inline DWORD KItem::GetTalismanPotentialLimit() const
{
	return m_pItemTemplate->Potential;
}

inline int KItem::GetTalismanEnchase(int index) const
{
	if (index >= 0 && index < TM_HOLE_NUM)
	{
		return m_TalismanEnchaseSet[index];
	}
	else
	{
		return -1;
	}
}

inline void KItem::SetTalismanEnchase(int index, int enchaseId)
{
	if (index >= 0 && index < TM_HOLE_NUM)
	{
		m_TalismanEnchaseSet[index] = enchaseId;
	}
}

inline int KItem::GetTalismanBuff() const
{
	return m_pItemTemplate->TalismanBuff;
}

inline int KItem::GetEnchaseId() const
{
	return m_pItemTemplate->TalismanBuff;
}

inline int KItem::GetEnchaseLevel() const
{
	return m_pItemTemplate->nLevel;
}

inline int KItem::GetTalismanCoolDown() const
{
	return m_pItemTemplate->nCDTime;
}

inline int KItem::GetTalismanId() const
{
	return m_pItemTemplate->nItemRes;
}

inline void	KItem::SetLockDate( DWORD lockDate )
{
	m_dwLockDate = lockDate;
}
inline 	DWORD	KItem::GetLockDate( void )
{
	return m_dwLockDate;
}

inline void KItem::SetFlushTimes(int nTimes )
{
	m_FlushTimes = nTimes;
}

inline int KItem::GetFlushTimes()const
{
	return m_FlushTimes;
}

inline bool KItem::IsLocked( int nConnectIdx, bool bDate )
{
	if ( bDate )
	{
		return m_LockCount > 0 || IsLockedByDate( nConnectIdx );
	}
	else
	{
		return m_LockCount > 0;
	}	
}

inline void KItem::LockByDate( void )
{
#ifdef _SERVER
	m_dwLockDate = LOCK_BY_DATA_SIGN;
#endif	
}

inline void  KItem::UnlockByDate( DWORD nLockTime )
{
#ifdef _SERVER
	DWORD curDate = UNIX_TMIE_STAMP;
	if ( (m_dwLockDate == 0|| m_dwLockDate == LOCK_BY_DATA_SIGN) && curDate + nLockTime > curDate )
	{
		m_dwLockDate = curDate + nLockTime;
	}//endif
#endif	
}

inline void KItem::Lock()
{
	m_LockCount++;
}

inline void KItem::Unlock()
{
	m_LockCount--;
}

inline int KItem::GetLockCount( void ) const
{
	return m_LockCount;
}

inline void KItem::SetLockCount( int lockCount )
{
	m_LockCount = lockCount;
}

inline int KItem::GetActionTime() const
{
	return m_ActionTime;
}

inline void	KItem::SetActionTime(int time)
{
	m_ActionTime = time;
}

inline bool KItem::IsUnique( void ) const
{
	return (GetRestrictCount() == 1);
}

inline bool KItem::CanDiscard( void ) const
{
	return m_pItemTemplate->btCanDiscard ? true : false;
}

inline bool KItem::CanSell( void ) const
{
	return m_pItemTemplate->CanSell ? true : false;
}

inline bool KItem::CanExchange( void ) const
{
	return ( m_pItemTemplate->btCanExchage  && m_ItemStackCount > 0 && !IsTaskGiven() && m_pItemTemplate->nMaxFlushTimes <= 0 ) ? true : false;
}

inline enumItemDeathDropType KItem::GetDeathDropType( void ) const
{
	return (enumItemDeathDropType)m_pItemTemplate->DeathDropType;
}

inline int KItem::GetRestrictCount( void ) const
{
	return m_pItemTemplate->RestrictCount;
}

inline bool KItem::IsBind( void ) const
{
	return m_IsBind;
}

inline void KItem::SetBind( bool bind )
{
	m_IsBind = bind;
}

#ifdef _SERVER

inline void KItem::SetGenTime( DWORD dwTime )
{
	m_GenerateTime = dwTime;
}

inline DWORD KItem::GetGenTime() const
{
	return m_GenerateTime;
}

inline int KItem::GetLogLevel() const
{
	return m_pItemTemplate->nLogCode;
}

inline const FSGUID& KItem::GetGUID() const
{
	return m_GUID;
}

inline void KItem::SetGUID(const FSGUID& guid)
{
	memcpy(&m_GUID, &guid, sizeof(FSGUID));
}

inline void KItem::GetItemTemplateId(ItemTemplateId& templateId) const
{
	templateId.IDArray[0] = GetGenre();
	templateId.IDArray[1] = GetDetailType();
	templateId.IDArray[2] = GetParticular();
	templateId.IDArray[3] = GetLevel();
}

inline int KItem::GetItemTemplateId(char* pTemplateIdStr, size_t buffSize) const
{
	if (pTemplateIdStr)
	{
		return snprintf(pTemplateIdStr, buffSize, "%d,%d,%d,%d", GetGenre(), GetDetailType(), GetParticular(), GetLevel());
	}
	else
	{
		return 0;
	}
}

inline int 
KItem::PrintItemInfo( char* pInfoStr, size_t buffSize ) const
{
	if ( pInfoStr )
	{
		int printResult = snprintf(
			pInfoStr, 
			buffSize, 
			"%d,%d,%d,%d %d %d", 
			GetGenre(), 
			GetDetailType(), 
			GetParticular(), 
			GetLevel(), 
			GetLevelupTimes(), 
			GetYaoID() );

		pInfoStr[ buffSize - 1 ] = 0;
		return printResult;
	}
	else
	{
		return 0;
	}
}

#else

inline const char* KItem::GetImageSetFile() const
{
	return m_pItemTemplate->szImageSetName;
}

inline const char* KItem::GetImageFile() const
{
	return m_pItemTemplate->szImageName;
}

#endif

#endif
