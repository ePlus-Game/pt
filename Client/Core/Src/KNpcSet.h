#ifndef	KNpcSetH
#define	KNpcSetH

#include "KLinkArray.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "GameDataDef.h"

#ifndef _SERVER

#define		MAX_NPC_REQUEST		512
#define		MAX_INSTANT_SOUND	30
#define		INVALIDE_COUNT		0xFFFFFFFF

class	KInstantSpecial
{
	typedef char SPRFILENAME[FILE_NAME_LENGTH];
	typedef char SNDFILENAME[FILE_NAME_LENGTH];
public:
	KInstantSpecial();
	~KInstantSpecial();

public:
	void				GetSprName		( unsigned int nNo, char *lpszName,unsigned int nLength	) ;
	void				PlaySound		( unsigned int nNo										);
private:
	void				LoadSprName		( void													);
	void				LoadSoundName	( void													);

private:
	SPRFILENAME*		m_SprNames;
	unsigned int		m_nSprNameCount;
	SNDFILENAME*		m_SndNames;
	unsigned int		m_nSndNameCount;
	static KCacheNode*	s_pSndNode;
};

#endif

typedef struct
{
	DWORD				dwRequestId;
	DWORD				dwRequestTime;
	RequestNpcAddon		Addon;
} RequestNpc;

typedef struct
{
#ifndef _SERVER
	int					nStandFrame[2];
	int					nWalkFrame[2];
#endif
	int					nRunFrame[2];
	int					nWalkSpeed;
	int					nRunSpeed;
	int					nAttackFrame;
	int					nCastFrame;
	int					nHurtFrame;
} PlayerBaseValue;

struct PKRelationEntry
{
	int Condition;
	int Match;
	int Result;
};

#ifndef _SERVER
struct SyncToWorldNpcInfo
{
	DWORD NpcId;
	int Mode;
	int Params[MAX_SYNC_TO_WORLD_PARAM_COUNT];
	char Name[32];
	DWORD PosX;
	DWORD PosY;
};

typedef std::vector<SyncToWorldNpcInfo> SyncToWorldNpcInfoArray;
#endif

#define MAX_PK_RELATION_ENTRY 32

class KNpcSet
{
public:
	KNpcSet();

public:
	void			Init();
	int				GetNpcCount(int nKind = -1, int nCamp = -1);
	int				SearchName(LPSTR szName);
	int				SearchID(DWORD dwID);
	int				SearchNameID(DWORD dwID);
	BOOL			IsNpcExist(int nIdx, DWORD dwId);
	int				Add(int nNpcSetingIdxInfo, int nSubWorld, int nRegion, int nMapX, int nMapY, bool bPlayer, int nOffX = 0, int nOffY = 0);
	int				Add(int nNpcSetingIdxInfo, int nSubWorld, int nMpsX, int nMpsY, bool bPlayer = false);
	int				Add(int nSubWorld, void* pNpcInfo, bool bPlayer = false);
	bool			IsPKModeCanSwitch(int nNpcIdx, PK_MODE mode);

#ifdef _SERVER
	void			Remove(int nIdx, BOOL bPlayer = FALSE);
#else
	void			Remove(int nIdx, bool bServerRemove, BOOL bPlayer = FALSE);
#endif	
	void			RemoveAll();

	NPC_RELATION	GetRelation(int nIdx1, int nIdx2);
	
	static int		GetDistance(int nIdx1, int nIdx2);
	static int		GetDistance(int nSrcX, int nSrcY, int nTargetIdx);
	static int		GetDistanceSquare(int nIdx1, int nIdx2);
	static int		GetDistanceSquare(int nSrcX, int nSrcY, int nTargetIdx);
#ifndef _SERVER
	static int		GetDistanceByMousePt(int nMouseX, int nMouseY, int nTargetIdx);
#endif	
	int				GetNextIdx(int nIdx);

	void			ClearActivateFlagOfAllNpc();	// 把所有npc的 bActivateFlag 设为 FALSE (每次游戏循环处理所有npc的activate之前做这个处理)
	void			LoadPlayerBaseValue(LPSTR szFile);
	inline	int		GetPlayerWalkSpeed() { return m_cPlayerBaseValue.nWalkSpeed; };
	inline	int		GetPlayerRunSpeed() { return m_cPlayerBaseValue.nRunSpeed; };
	inline	int		GetPlayerAttackFrame() { return m_cPlayerBaseValue.nAttackFrame; };
	inline	int		GetPlayerCastFrame() { return m_cPlayerBaseValue.nCastFrame; };
	inline	int		GetPlayerHurtFrame() { return m_cPlayerBaseValue.nHurtFrame; };
	inline	int		GetPlayerRunFrame(BOOL bMale)
	{
		if (bMale)
			return m_cPlayerBaseValue.nRunFrame[0];
		else
			return m_cPlayerBaseValue.nRunFrame[1];
	};

#ifndef _SERVER
	void			SetUiLoginDisplayerNpc(  UI_DISPLAYER_NPC_SYNC* NpcSync  );
	int				SearchPet(int nClientID);
	int				GetPlayerStandFrame(BOOL bMale) 
	{ 
		if (bMale)
			return m_cPlayerBaseValue.nStandFrame[0];
		else
			return m_cPlayerBaseValue.nStandFrame[1];
	};
	int				GetPlayerWalkFrame(BOOL bMale)
	{
		if (bMale)
			return m_cPlayerBaseValue.nWalkFrame[0];
		else
			return m_cPlayerBaseValue.nWalkFrame[1];
	};
	
	BOOL			IsNpcRequestExist(DWORD	dwID);
	void			InsertNpcRequest(DWORD dwID);
	RequestNpcAddon* GetRequestAddon(DWORD dwID);
	void			RemoveNpcRequest(DWORD dwID);
	int				GetRequestIndex(DWORD dwID);
	// 添加一个客户端npc（需要设定ClientNpcID）
	int				AddClientNpc(int nTemplateID, int nRegionX, int nRegionY, int nMpsX, int nMpsY, int nNo);
	// 从npc数组中寻找属于某个region的 client npc ，添加进去
	void			InsertNpcToRegion(int nRegionIdx);
	// 查找某个ClientID的npc是否存在
	int				SearchClientID(KClientNpcID sClientID);
	// 某座标上精确查找Npc，客户端专用
	int				SearchNpcAt(int nX, int nY, int nRelation, int nRange, bool bSearchSelf = false , bool bNoPlayer = false);
	void			CheckBalance();
	int				GetAroundPlayerForTeamInvite(KUiPlayerItem *pList, int nCount);	// 获得周围玩家列表(用于队伍邀请列表)
	void			GetAroundOpenCaptain(int nCamp);		// 获得周围同阵营的已开放队伍队长列表
	int				GetAroundPlayer(KUiPlayerItem *pList, int nCount);	// 获得周围玩家列表(用于列表)

	// 设定是否全部显示玩家的名字  bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
	void			SetShowNameFlag(BOOL bFlag);
	// 判断是否全部显示玩家的名字  返回值 TRUE 显示，FALSE 不显示
	BOOL			CheckShowName();
	// 设定是否全部显示玩家的聊天  bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
	void			SetShowChatFlag(BOOL bFlag);
	// 判断是否全部显示玩家的聊天  返回值 TRUE 显示，FALSE 不显示
	BOOL			CheckShowChat();
	// 设定是否全部显示玩家的血  bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
	void			SetShowLifeFlag(BOOL bFlag);
	// 判断是否全部显示玩家的血  返回值 TRUE 显示，FALSE 不显示
	BOOL			CheckShowLife();
	// 设定是否全部显示玩家的内力  bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
	void			SetShowManaFlag(BOOL bFlag);
	// 判断是否全部显示玩家的内力  返回值 TRUE 显示，FALSE 不显示
	BOOL			CheckShowMana();
		
	void			setShowPlayer(BOOL bFlag);

	BOOL			isShowPlayer();

	int GetSyncToWorldNpcCount() const;//得到世界同步NPC数量
	bool ExistSyncToWorldNpc(DWORD npcId) const;//是否存在世界同步NPC
	int GetSyncToWorldNpcIndex(DWORD npcId) const;//得到世界同步NPC的ListIndex
	SyncToWorldNpcInfo* GetSyncToWorldNpcInfo(int listIndex);//得到世界同步NPC的信息
	void AddSyncToWorldNpc(SyncToWorldNpcInfo& info);//添加世界同步NPC
	void RemoveSyncToWorldNpc(int listIndex);//删除世界同步NPC
	void RemoveAllSyncToWorldNpcs();//删除所有的世界同步NPC
	void RemoveAllNpcExceptClient();
	void ForceRemoveAllNpcHeadInfo();
	int  GetNpcByName(const char* name);
#else
	BOOL			SyncNpc(DWORD dwID, int nPlayerIdx);
#endif
private:
	void			SetID(int m_nIndex);
	int				FindFree(bool bPlayer);
	bool			GenPKRelationTbl();
	PK_TARGET_COND	GetPKCond(int nLauncher, int nTarget);
public:
	PK_TARGET_COND	GetSUCond(int nLauncher, int nTarget);
#ifndef _SERVER
public:
	void			SceneProcessNpc();

#endif

public:
	PlayerBaseValue		m_cPlayerBaseValue;					// 玩家标准数据

#ifdef _SERVER
	int					m_nPKDamageRate;					// PK时伤害乘一个系数
	int					m_nFactionPKFactionAddPKValue;		// 三大阵营之间PK，等级差太大时PK者PK值增加
	int					m_nKillerPKFactionAddPKValue;		// 杀手与三大阵营PK，等级差太大时PK者PK值增加
	int					m_nEnmityAddPKValue;				// 仇杀时PK者PK值增加
	int					m_nBeKilledAddPKValue;				// 被PK致死着PK值增加，应该是个负数
	int					m_nLevelDistance;					// 等级差多少算是PK新手
#else
	KInstantSpecial		m_cInstantSpecial;
#endif
private:
	PKRelationEntry m_PKRelationTbl[pk_mode_num][MAX_PK_RELATION_ENTRY];
	unsigned char m_PKModeAccesser[pk_mode_num];

	DWORD				m_dwIDCreator;						//	游戏世界中的ID计数器
	KLinkArray			m_FreeIdx;							//	可用表
	KLinkArray			m_UseIdx;							//	已用表
#ifdef _SERVER
	INDEXIDMAP			m_NpcUseIndex;						//  已用NPC索引
#endif
	// Add by Cooler -->
	// 2006-5-26 15:49
	KLinkArray			m_FreePlayerIdx;
	KLinkArray			m_UsePlayerIdx;
	// End add by Cooler <--

#ifndef _SERVER
	enum
	{
		PATE_CHAT = 0x01,
		PATE_NAME = 0x02,
		PATE_LIFE = 0x04,
		PATE_MANA = 0x08,
	};

	enum NPC_PAINT_FLAG
	{
		SHOW_NPC	= 0x01,
		SHOW_PLAYER = 0x02,
	};

	int					m_nNpcPaitFlag;
	int					m_nShowPateFlag;					// 是否全部显示玩家的名字在头顶上 zroc add
	RequestNpc			m_RequestNpc[MAX_NPC_REQUEST];		//	向服务器申请的ID表
	KLinkArray			m_RequestFreeIdx;					//	向服务器申请可用表
	KLinkArray			m_RequestUseIdx;					//	向服务器申请空闲表

	SyncToWorldNpcInfoArray m_SyncToWorldNpcs;				//世界同步NPC信息
#endif

#ifdef _SERVER
public:
	int GetFreeNpcSlotCount();
	int GetUsedNpcSlotCount();
	DWORD m_NpcSlotNotFoundCount;
#endif
};

// modify by Freeway Chen in 2003.7.14
// 确定两个NPC之间的战斗关系

extern KNpcSet NpcSet;

inline bool KNpcSet::IsPKModeCanSwitch(int nNpcIdx, PK_MODE mode)
{
	bool bRet = false;

	if(Npc[nNpcIdx].m_Kind == kind_player)
	{
		bRet = m_PKModeAccesser[mode] & pkm_accesser_player ? true : false;;
	}
	else
	{
		bRet = m_PKModeAccesser[mode] & pkm_accesser_npc ? true : false;;
	}

	return bRet;
}

#ifdef _SERVER
inline int KNpcSet::GetFreeNpcSlotCount()
{
	return m_FreeIdx.GetCount();
}

inline int KNpcSet::GetUsedNpcSlotCount()
{
	return m_UseIdx.GetCount();
}
#endif

#ifndef _SERVER
inline int KNpcSet::GetSyncToWorldNpcCount() const
{
	return m_SyncToWorldNpcs.size();
}

inline bool KNpcSet::ExistSyncToWorldNpc(DWORD npcId) const
{
	return (GetSyncToWorldNpcIndex(npcId) >= 0);
}

inline int KNpcSet::GetSyncToWorldNpcIndex(DWORD npcId) const
{
	const int size = m_SyncToWorldNpcs.size();
	for (int index = 0; index < size; index++)
	{
		if (m_SyncToWorldNpcs[index].NpcId == npcId)
		{
			return index;
		}
	}
	
	return -1;
}

inline SyncToWorldNpcInfo* KNpcSet::GetSyncToWorldNpcInfo(int listIndex)
{
	if (listIndex >= 0 && listIndex < m_SyncToWorldNpcs.size())
	{
		return &m_SyncToWorldNpcs[listIndex];
	}
	else
	{
		return NULL;
	}
}

inline void KNpcSet::RemoveSyncToWorldNpc(int listIndex)
{
	if (listIndex >= 0 && listIndex < m_SyncToWorldNpcs.size())
	{
		SyncToWorldNpcInfoArray::iterator remove = m_SyncToWorldNpcs.begin() + listIndex;
		m_SyncToWorldNpcs.erase(remove);
	}
}

inline void KNpcSet::RemoveAllSyncToWorldNpcs()
{
	m_SyncToWorldNpcs.clear();
}

inline void KNpcSet::AddSyncToWorldNpc(SyncToWorldNpcInfo& info)
{
	m_SyncToWorldNpcs.push_back(info);
}
#endif

#endif
