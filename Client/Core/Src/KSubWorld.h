#ifndef	KWorldH
#define	KWorldH
#pragma warning( disable : 4786 )
#define	VOID_REGION -2
#include "KEngine.h"
#include "KRegion.h"

#ifdef _SERVER
#include "RandomTransport.h"
#include "npc_statistic.h"
#include "world_combat_instance.h"
#include "SocialUnit.h"
#endif


#define MAX_ESAREA	5
#define MAX_MAP_FLAGS 256
#define MIN_SYNC_INTERVAL_SEC 2
#define MAX_GLOBAL_SYNC_PLAYER_NUM 30
#define DEFAULT_MAP_ROOM_CHAT_INTERVAL 120
#define MIN_MAP_ROOM_CHAT_INTERVAL 1
#define WAR_TOTAL_NORMAL_COMMANDER_NUM 4
#define WAR_SUB_COMMANDER_NUM 9
#define MAX_WAR_COMMANDER_NUM 12
#define MIN_WAR_COMMANDER_INTERVAL 2
#define STD_WAR_COMMANDER_INTERVAL 5

enum
{
	especial_area_none = -1,
	especial_area_safe = 0,		//安全区
	especial_area_pk,			//切磋区
	especial_area_task,			//任务区
	especial_area_war,			//战争区
	especial_area_city,         //城市区

	especial_area_end
};
enum
{
	especial_sync_type_normal = 0,
	espacial_sync_type_same_org,
	espacial_sync_type_all,
	espacial_sync_type_num,
};

enum COMBAT_SCORE_CALU_TYPE
{
	NO_CALU = 0,	//不计算不显示
	PROGRAME_CALU, //程序计算的显示	
	SCRIPT_CALU,	//脚本计算的显示
};

enum
{
	war_totalcommander = 1,
	war_normalcommander,
	war_subcommander	

};

enum
{
	invader = 0,
	defender,
	tongwarfangshu
};
struct _EspecialArea {
	int nLUX;
	int nLUY;
	int nRBX;
	int nRBY;
	int nType;
};

//#ifdef _SERVER
//#define MAX_SUBWORLD_MISSIONCOUNT 10
//#define MAX_GLOBAL_MISSIONCOUNT 50
//typedef KMissionArray <KMission , MAX_TIMER_PERMISSION> KSubWorldMissionArray;
//typedef KMissionArray <KMission , MAX_GLOBAL_MISSIONCOUNT> KGlobalMissionArray;
//extern KGlobalMissionArray g_GlobalMissionArray;
//#endif

//-------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////

#ifdef _SERVER

struct WorldSpawner_RunTimeInfo
{
    unsigned int uMobCount;
    unsigned int uLastSpawnTime;
};
struct wRect 
{
    short Left;
    short Top;
    short Right;
    short Bottom;
};

class RectList
{
private:
    unsigned int uCount;
    wRect *pRects;
public:
    RectList():uCount(0),pRects(NULL)
    {
        
    }
    
    ~RectList()
    {
        Release();
    }
    
    void Release()
    {
        uCount = 0;
        if (pRects != NULL)
        {
            delete[] pRects;
        }
        pRects = NULL;
    }
    
    void Load(KFile &File)
    {
        Release();
        File.Read(&uCount, sizeof(uCount));
        pRects = new wRect[uCount];
        File.Read(pRects, uCount * sizeof(wRect));
    }
    
    
    const wRect& RandomGet(void) const
    {
        unsigned int uIdx = g_Random(uCount);
        return pRects[uIdx];
    }

	bool CopyInstance(RectList& list) const;
};

class SpawnRecoder
{
private:
    struct
    {
        int nNpcTemplateID;
        int nMinCount;
        int nMaxCount;
        int nMinLevel;
        int nMaxLevel;
        int nTimeInterval;
    } CommonInfo;
    
    WorldSpawner_RunTimeInfo RunTimeInfo;
    unsigned int m_uSubWorld;
    RectList*    m_pSpawnRects;
    public:
		static DWORD s_SpawnCount;
        SpawnRecoder()
        {
            memset(&CommonInfo, 0, sizeof(CommonInfo));
            memset(&RunTimeInfo, 0, sizeof(RunTimeInfo));
            m_pSpawnRects = NULL;
            m_uSubWorld = -1;
        }
        
        ~SpawnRecoder()
        {
            Release();
        }
        
        void Release()
        {
            if(m_pSpawnRects != NULL)
            {
                delete m_pSpawnRects;
            }
            m_pSpawnRects = NULL;
            m_uSubWorld = -1;
        }
        
        void Load(unsigned int uSubWorld, KFile& File)
        {
            Release();
            m_uSubWorld = uSubWorld;
            File.Read(&CommonInfo, sizeof(CommonInfo));
            m_pSpawnRects = new RectList;
            if (m_pSpawnRects != NULL)
            {
                m_pSpawnRects->Load(File);
            }
        }
        
        void Active(unsigned int uTimer);

		bool CopyInstance(SpawnRecoder& record, int worldIndex) const;
};

class KWorldSpawner
{
private:

    unsigned int m_uSpawnInfoCount;
    SpawnRecoder* m_pSpawnInfo;
public:
    KWorldSpawner():m_pSpawnInfo(NULL),m_uSpawnInfoCount(0)
    {
    }
    void Release()
    {
        if (m_pSpawnInfo != NULL)
        {
            delete[] m_pSpawnInfo;
        }
        m_pSpawnInfo = NULL;
        m_uSpawnInfoCount = 0;
    }
    void Load(unsigned int uSubWorld, const char* pStrFileName)
    {
        Release();
        KFile File;
        if (File.Open((char*)pStrFileName))
        {
            File.Read(&m_uSpawnInfoCount, sizeof(m_uSpawnInfoCount));
            m_pSpawnInfo = new SpawnRecoder[m_uSpawnInfoCount];
            if (m_pSpawnInfo != NULL)
            {
                for (unsigned int i = 0; i< m_uSpawnInfoCount; i++)
                {
                    m_pSpawnInfo[i].Load(uSubWorld, File);
                }
            }
        }
    }
    void Active(unsigned int uTimer)
    {
        for (unsigned int i = 0; i < m_uSpawnInfoCount; i++)
        {
            m_pSpawnInfo[i].Active(uTimer);
        }
    }

	bool CopyInstance(KWorldSpawner& worldSpawner, int worldIndex) const;
};

typedef std::vector<int> SyncToWorldNpcIndexArray;

#endif

#ifdef _SERVER
#define MAX_CUSTOM_VARIABLE_COUNT 10	//最大自定义变量个数
#define MAX_CUSTOM_STRING_COUNT 5		//最大自定义字符串个数
#define MAX_WORLD_TEAM_COUNT 5			//最大地图绑定队伍个数
#endif

enum enumWorldState
{
	world_state_init = 0,
	world_state_active,
	world_state_idle,
	world_state_ready_for_reuse,
};

class KSubWorld
{
public:	
	int			m_nIndex;//在SubWorld数组中的位置
	int			m_SubWorldID;//世界模板编号

#ifdef _SERVER
	enumWorldState m_State;
	int			m_nPlayerCount;			//玩家数量
	DWORD		m_LastPlayerExitTime;	//最后一个玩家离开的时刻
#endif

#ifdef _SERVER
	#define SUBLORD_COUNT 10

	int			m_nLordOfWorld;					//地图领主Npc Index
	int			m_nRobberOfWorld;				//地图攻击者Npc Index
	int			m_SubLordOfWorld[SUBLORD_COUNT];//地图领主Npc Index

	int         m_nPoolOfWorld;                 //地图分星池 NpcIndex
	int         m_nSubPoolOfWorld;              //地图分星池守护星 NpcIndex

	int			m_nGroup;
	int			m_nCate;
#endif

#ifdef _SERVER
	//KSubWorldMissionArray m_MissionArray;
	int			m_nIndexInUniverse;
#endif
	KRegion*	m_Region;
#ifndef _SERVER
	int			m_ClientRegionIdx[MAX_REGION];
	char		m_szMapPath[FILE_NAME_LENGTH];
	
#endif
	int			m_nWorldRegionWidth;			//	SubWorld里宽几个Region
	int			m_nWorldRegionHeight;			//	SubWorld里高几个Region
	int			m_nTotalRegion;					//	SubWorld里Region个数
//	int			m_nRegionWidth;					//	Region的格子宽度
//	int			m_nRegionHeight;				//	Region的格子高度
//	int			m_nCellWidth;					//	Cell的像素宽度
//	int			m_nCellHeight;					//	Cell的像素高度
	int			m_nRegionBeginX;				
	int			m_nRegionBeginY;
	DWORD		m_dwCurrentTime;				//	当前帧
	KList		m_NoneRegionNpcList;			//	不在地图上的NPC


	_EspecialArea	m_nEspecialArea[MAX_ESAREA];
	int				m_nAllType;

private:

#ifdef _SERVER
	CRandomTransport m_clsTransportList;
    KWorldSpawner   m_clsWorldSpawner;
	int m_CustomVariable[MAX_CUSTOM_VARIABLE_COUNT];	//自定义变量
	char m_CustomString[MAX_CUSTOM_STRING_COUNT][MAX_WORLD_CUSTOM_STRING_LENGTH];		//自定义字符串
	int m_WorldTeam[MAX_WORLD_TEAM_COUNT];				//地图绑定队伍
#endif

public:
	KSubWorld();
	~KSubWorld();
	void		Activate();
	void		GetFreeObjPos(POINT& pos);
	BOOL		CanPutObj(POINT pos);
	void		MissleChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	void		AddPlayer(int nRegion, int nIdx);
	void		RemovePlayer(int nRegion, int nIdx);
	void		Close(bool notifyScript = true);
	int			GetDistance(int nRx1, int nRy1, int nRx2, int nRy2);						// 像素级坐标
	void		Map2Mps(int nR, int nX, int nY, int nDx, int nDy, int *nRx, int *nRy);		// 格子坐标转像素坐标
	static void Map2Mps(int nRx, int nRy, int nX, int nY, int nDx, int nDy, int *pnX, int *pnY);		// 格子坐标转像素坐标
	void		Mps2Map(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy);	// 像素坐标转格子坐标
	void		GetMps(int *nX, int *nY, int nSpeed, int nDir, int nMaxDir = 64);			// 取得某方向某速度下一点的坐标
	BYTE		TestBarrier(int nMpsX, int nMpsY);
	BYTE		TestBarrier(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY);	// 检测下一点是否为障碍
	BYTE		TestBarrierMin(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY, BOOL bCheckNpc);	// 检测下一点是否为障碍
	BYTE		GetBarrier(int nMpsX, int nMpsY);											// 取得某点的障碍信息
	DWORD		GetTrap(int nMpsX, int nMpsY);
	void		MessageLoop();
	int			FindRegion(int RegionID);													// 找到某ID的Region的索引
	int			FindFreeRegion(int nX = 0, int nY = 0);
	int			AddEspecialArea( int nLUX, int nLUY, int nRBX, int nRBY, int nType );
	int			GetEspecialAreaType( int nX, int nY );
	void		SetAllEspecialType( int nType ){ m_nAllType = nType; }
	bool        IsWorldCombatMap(void)const;
	int         GetSyncType(void)const { return m_nSyncType ;}
	int         GetMapChatInterval(void)const{return m_nMapChatInterval;}
	int			GetCombatScoreCalcType() const;

#ifdef _SERVER
	int GetPlayerCount() const;
	bool IsFree() const;
	DWORD GetIdleTime() const;
	int			RevivalAllNpc();//将地图上所有的Npc包括已死亡的Npc全部恢复成原始状态
	void		BroadCast(const char* pBuffer, size_t uSize);
	inline void	BroadCastRegion(const void* pBuffer, size_t uSize, int &nMaxCount, int nRegionIndex, int nOX, int nOY);
	BOOL		LoadMap(int nIdx);
	
	BOOL		ReLoadMap(int nCityType);	// 重新按照地图名称进行加载
	
	//改变地图上某个坐标点的Trap, 如果dwTrapId=0则删除Trap, 如果不为0表示有trap
	void		SetTrap(DWORD dwTrapId, int nMpsX, int nMpsY, int nRange);
	void		LoadObject(char* szPath, char* szFile);
	void		NpcChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nNpcIdx);
	void		PlayerChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nObjIdx);
	BOOL		SendSyncData(int nIdx, int nClient);
	int			GetRegionIndex(int nRegionID);
	int			FindNpcFromName(const char * szName);
	//BOOL		LoadGround(KRegionPool* pAllocator, const char* szMapName);

	BOOL		IsCanRandomTrans();
	BOOL		GetRandomTransPos(int &nTransX, int &nTransY, int group = 0);
	
	int			GetGroup( )
				{ return m_nGroup; }

	int			GetCate( )
				{ return m_nCate; }

	int			GetLord( )
				{ return m_nLordOfWorld; }
	void		SetLord( int nLord )
				{ m_nLordOfWorld = nLord; }

	int			GetSubLord( int nIndex )
				{ return ( nIndex >= 0 && nIndex < SUBLORD_COUNT ) ? m_SubLordOfWorld[nIndex] : -1; }
	void		SetSubLord( int nIndex, int nSubLord )
				{ ( nIndex >= 0 && nIndex < SUBLORD_COUNT ) ? m_SubLordOfWorld[nIndex] = nSubLord : -1; }


	int			GetRobber( )
				{ return m_nRobberOfWorld; }
	void		SetRobber( int nRobber)
				{ m_nRobberOfWorld = nRobber; }

	void        SetPool(int nPool) 
	{           m_nPoolOfWorld = nPool;}

	int         GetPool()
	{           return m_nPoolOfWorld; }
	
	void        SetSubPool(int nSubPool)
	{           m_nSubPoolOfWorld = nSubPool;}

	int         GetSubPool()
	{           return m_nSubPoolOfWorld;    }


	bool IsHaveMapFlage(const int nIndex);
	int  GetCustomVariable(int varIndex) const;//得到自定义变量
	void SetCustomVariable(int varIndex, int varValue);//设置自定义变量
	void GetCustomString(int strIndex, char* outStrBuff, size_t strBuffSize) const;//得到自定义字符串
	bool SetCustomString(int strIndex, const char* inStrBuff, size_t strBuffSize);//设置自定义字符串
	void SendCustomStringToPlayer(int strIndex, int playerIndex);//向指定玩家发送
	void SendCustomStringToAllPlayer(int strIndex);//向地图上所有玩家发送
	int  GetWorldTeamCount() const;//得到地图绑定队伍数量
	int  GetWorldTeam(int index) const;//得到地图绑定队伍
	int  CreateWorldTeam(int index, int playerIndex);//创建地图绑定队伍
	void SetWorldTeam(int index, int teamId);//设置地图绑定队伍

	void GlobalSyncPlayer( void );
	void WorldCombatMapActive ( void );
	bool		CanGainScore() const;
	void		SetOrgScore(DWORD nOrgId, int Point);
	void		ClearWarMap();
private:
	void SyncAllPlayer ( void );
	void SyncPlayerByOrganize ( void );
	bool PrepareCustomStringProtocol(int strIndex, char* protocolBuff, unsigned int& buffSize);
public:

#endif
#ifndef _SERVER
	//BOOL		LoadMap(int nIdx, int nRegion);
	BOOL		LoadMap(int nIdx, int nRegion, int nCityType, bool bSyncWorld);
	void		NpcChangeRegion(int nSrcRegionIdx, int nDesRegionIdx, int nNpcIdx);
	//void		Paint();
	void		Mps2Screen(int *Rx, int *Ry);
	void		Screen2Mps(int *Rx, int *Ry);
#endif
private:
	void		LoadTrap();
public:
#ifndef _SERVER
	void		LoadCell();
#endif

public:
	DWORD GetInstanceId() const;//得到副本编号
	int GetWorldTemplateId() const;//得到世界模板编号
	void SetInstanceId(DWORD instanceId);//设置副本编号
	void SetExpireTime(DWORD unixTimeStamp);//设置过期时间
#ifdef _SERVER
	bool PrepareForReuse();//准备重用
	bool Reuse();//重用
	enumWorldState GetState() const;//得到状态
	bool CopyInstance(int worldIndex) const;//拷贝副本数据，并生成相关对象（用于副本生成）
	bool CanEnter(int playerIndex) const;//是否可以进入
	void SetOwner(int ownerPlayerIndex);//设置所有者
	int  GetOwner() const;//得到所有者
	void SetOwnerSocialGUID(const FSGUID& guid);//设置所有者（社会关系）
	FSGUID& GetOwnerSocialGUID();//得到所有者（社会关系）
	int  FirstPlayer();//第一个玩家
	int  NextPlayer();//下一个玩家
	void KickAllPlayer();//踢出所有玩家
	bool CheckForExpire();//检查是否过期
	bool IsInSyncToWorldList(int npcIndex) const;//是否在世界同步NPC列表中
	int  GetSyncToWorldListIndex(int npcIndex) const;//得到世界同步NPC列表的索引
	void AddToSyncToWorldList(int npcIndex);//添加到世界同步NPC列表
	void RemoveFromSyncToWorldList(int npcIndex);//从世界同步NPC列表删除
	const CombatOrgnize *  GetCombatInstanceOrgInfo(const int nOrgId); //Notice nOrgId~[1,MAX_COMBAT_ORG_NUM] return NULL if invalid param
	void                   AddCombatInstanceOrgPerson(const int nOrgId, const int nNumberAdded);
	void                   AddCombatInstanceOrgScore (const int nOrgId, const int nScoreAdded);
	void				   DecCombatInstanceOrgScore (const int nOrgId, const int nScoreDeced);
	WordCombatInstanceInfo& GetCombatInstanceInfo(){ return m_WorldCombatInstanceInfo;}
	void FlushTop10Player();
	void SyncTongWarInfo();
	void WarCommanderSync();

#endif
private:
	DWORD m_InstanceID;//副本编号
	DWORD m_ExpireTime;//过期时间
	bool  m_IsCombatInstance; //是否是战争地图
	int   m_CombatScoreCalcType; //战场积分计算类型
	int   m_nSyncType; //战场题图同步属性
	int   m_nMapChatInterval; //这个地图上 地图频道的聊天间隔

#ifdef _SERVER
	int m_OwnerPlayerIndex;//拥有者的PlayerIndex
	FSGUID m_OwnerSocialGUID;//拥有者的社会关系GUID
	int m_IterateRegion;//遍历的当前Region
	KIndexNode* m_pIterateNode;//遍历的当前节点
	SyncToWorldNpcIndexArray m_SyncToWorldNpcs;//世界同步NPC
	int m_SyncListIndex;
	int m_NextSyncCounter;
	WordCombatInstanceInfo     m_WorldCombatInstanceInfo;
	int                        m_nSyncIntervalSec;
	int                        m_MapFlag[MAX_MAP_FLAGS];
	
private:
	void FillDataFunc(S2C_WAR_COMMANDER_INFO_SYNC* pInfoSync, WAR_COMMANDER_INFO** ppInfo, int nNpcIndex, int nDuty);
#else

public:
	WorldCombatClientBaseInfo	m_WorldCombatClientInfo[MAX_COMBAT_ORG_NUM];
	MapChannelInfo				m_MapChannelInfo;
	typedef struct tagWorldMapPlayerInfo
	{
		int                    m_Num;
		WORLD_PLAYER_INFO      m_PlayerInfos[MAX_GLOBAL_SYNC_PLAYER_NUM];
        public:
			tagWorldMapPlayerInfo():m_Num(0){/**/}
	}WORLD_MAP_PLAYER_INFO_CACHE;

	WORLD_MAP_PLAYER_INFO_CACHE m_PlayerInfoCache;
	
	typedef struct tagWarMapCommanderInfo
	{
		int                    m_Num;
		WAR_COMMANDER_INFO      m_PlayerInfos[MAX_WAR_COMMANDER_NUM];
		public:
			tagWarMapCommanderInfo():m_Num(0){/**/}
	}WAR_MAP_COMMANDER_INFO_CACHE;

	WAR_MAP_COMMANDER_INFO_CACHE m_WarCommanderInfoCache;

public:
	void ClearWorldMapPlayerInfoCache(void);
	void RefreshWorldMapPlayerInfoCache(const int nNum, const WORLD_PLAYER_INFO * pInfos);
	void RefreshWarCommanderInfoCache(const int nNum, WAR_COMMANDER_INFO* pInfo);
	void ClearWarCommanderInfoCache(void);
#endif

public:
#ifdef _SERVER
	static DWORD s_RecycleCount;
#endif
};
 
#ifdef _SERVER

inline void KSubWorld::BroadCastRegion(const void* pBuffer, size_t uSize, int &nMaxCount, int nRegionIndex, int nOX, int nOY)
{
	const POINT POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};

	if(nRegionIndex < 0)
	{
		return;
	}
	
	KRegion& CurRegion = m_Region[nRegionIndex];
	CurRegion.BroadCast(pBuffer, uSize, nMaxCount, nOX, nOY);
	for (unsigned int i= 0; i < 8; i++)
	{
		const int nConnectRegion = CurRegion.m_nConnectRegion[i];
		if (nConnectRegion != -1)
		{
			m_Region[nConnectRegion].BroadCast(pBuffer, uSize, nMaxCount, nOX - POff[i].x, nOY - POff[i].y);
		}
	}
}

inline BOOL KSubWorld::IsCanRandomTrans()
{
	return m_clsTransportList.IsCanRandomTrans();
}

inline bool KSubWorld::IsFree() const
{
	return (m_nIndex == INVALID_WORLD_INDEX);
}

inline int KSubWorld::GetPlayerCount() const
{
	return m_nPlayerCount;
}

inline DWORD KSubWorld::GetIdleTime() const
{
	if (GetPlayerCount() == 0)
		return m_dwCurrentTime - m_LastPlayerExitTime;
	else
		return 0;
}

inline enumWorldState KSubWorld::GetState() const
{
	return m_State;
}

inline int KSubWorld::GetCustomVariable(int varIndex) const
{
	if (varIndex >= 0 && varIndex < MAX_CUSTOM_VARIABLE_COUNT)
		return m_CustomVariable[varIndex];
	else
		return 0;
}

inline bool KSubWorld::IsHaveMapFlage(const int nIndex)
{
	if (nIndex >=0 && nIndex < MAX_MAP_FLAGS)
		return (m_MapFlag[nIndex] > 0);
	else
		return false;
}

inline void KSubWorld::SetCustomVariable(int varIndex, int varValue)
{
	if (varIndex >= 0 && varIndex < MAX_CUSTOM_VARIABLE_COUNT)
		m_CustomVariable[varIndex] = varValue;
}

inline void KSubWorld::GetCustomString(int strIndex, char* outStrBuff, size_t strBuffSize) const
{
	if (strIndex >= 0 && strIndex < MAX_CUSTOM_STRING_COUNT && outStrBuff != NULL && strBuffSize > 0)
	{
		size_t size = strBuffSize < MAX_WORLD_CUSTOM_STRING_LENGTH ? strBuffSize : MAX_WORLD_CUSTOM_STRING_LENGTH;
		strncpy(outStrBuff, m_CustomString[strIndex], size);
	}
}

inline bool KSubWorld::SetCustomString(int strIndex, const char* inStrBuff, size_t strBuffSize)
{
	if (strIndex >= 0 && strIndex < MAX_CUSTOM_STRING_COUNT && inStrBuff != NULL && strBuffSize > 0)
	{
		size_t size = strBuffSize < MAX_WORLD_CUSTOM_STRING_LENGTH ? strBuffSize : MAX_WORLD_CUSTOM_STRING_LENGTH;
		strncpy(m_CustomString[strIndex], inStrBuff, size);
		m_CustomString[strIndex][MAX_WORLD_CUSTOM_STRING_LENGTH - 1] = 0;

		return true;
	}

	return false;
}

inline int KSubWorld::GetWorldTeamCount() const
{
	int teamCount = 0;
	for (int teamIndex = 0; teamIndex < MAX_WORLD_TEAM_COUNT; teamIndex++)
	{
		if (m_WorldTeam[teamIndex] != INVALID_TEAM_ID)
			teamCount++;
	}

	return teamCount;
}

inline int KSubWorld::GetWorldTeam(int index) const
{
	if (index >= 0 && index < MAX_WORLD_TEAM_COUNT)
		return m_WorldTeam[index];
	else
		return INVALID_TEAM_ID;
}

inline void KSubWorld::SetWorldTeam(int index, int teamId)
{
	if (index >= 0 && index < MAX_WORLD_TEAM_COUNT)
		m_WorldTeam[index] = teamId;
}

inline int KSubWorld::GetOwner() const
{
	return m_OwnerPlayerIndex;
}

inline bool KSubWorld::IsInSyncToWorldList(int npcIndex) const
{
	return (GetSyncToWorldListIndex(npcIndex) >= 0);
}

inline int KSubWorld::GetSyncToWorldListIndex(int npcIndex) const
{
	const int listSize = m_SyncToWorldNpcs.size();
	for (int listIndex = 0; listIndex < listSize; listIndex++)
	{
		if (m_SyncToWorldNpcs[listIndex] == npcIndex)
			return listIndex;
	}

	return -1;
}

inline void KSubWorld::AddToSyncToWorldList(int npcIndex)
{
	if (!IsInSyncToWorldList(npcIndex))
	{
		m_SyncToWorldNpcs.push_back(npcIndex);
	}
}

inline void KSubWorld::RemoveFromSyncToWorldList(int npcIndex)
{
	const int listIndex = GetSyncToWorldListIndex(npcIndex);
	if (listIndex >= 0)
	{
		SyncToWorldNpcIndexArray::iterator remove = m_SyncToWorldNpcs.begin() + listIndex;
		m_SyncToWorldNpcs.erase(remove);
	}
}

inline const CombatOrgnize * KSubWorld::GetCombatInstanceOrgInfo(const int nOrgId)
{
	if (IsValidCombatID(nOrgId))
	{
		int nIndex = nOrgId - 1;
		return &m_WorldCombatInstanceInfo.org[nIndex];
	}//endif

	return NULL;
}

inline void KSubWorld::SetOwnerSocialGUID(const FSGUID& guid)
{
	m_OwnerSocialGUID = guid;
}

inline FSGUID& KSubWorld::GetOwnerSocialGUID()
{
	return m_OwnerSocialGUID;
}
#endif

inline DWORD KSubWorld::GetInstanceId() const
{
	return m_InstanceID;
}

inline void KSubWorld::SetInstanceId(DWORD instanceId)
{
	m_InstanceID = instanceId;
}

inline int KSubWorld::GetWorldTemplateId() const
{
	return m_SubWorldID;
}

inline void KSubWorld::SetExpireTime(DWORD unixTimeStamp)
{
	m_ExpireTime = unixTimeStamp;
}

extern KSubWorld SubWorld[MAX_SUBWORLD];

#endif
