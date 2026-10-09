#ifndef	KSubWorldSetH
#define	KSubWorldSetH

#include "KTimer.h"
#ifndef _SERVER
#include "KMapMusic.h"
#else
#include "KIniFile.h"
#include "KSubWorld.h"
#endif

#define ITEMVERSIONINI_SECTION1		"ItemVersion"
#define ITEMVERSIONINI_KEY11		"Version"

//世界运行时信息
struct WorldRuntimeInfo
{
	WorldRuntimeInfo()
	{
		LivingInstanceCount = 0;
		CreateInstanceCount = 0; 
		ReuseableInstanceCount = 0;
	}

	int LivingInstanceCount;
	int CreateInstanceCount;
	int ReuseableInstanceCount;
};

#define MAX_INSTANCE_ENTRY_COUNT 10		//最多副本入口数量

//世界入口信息
struct WorldEntryInfo
{
	WorldEntryInfo()
	{
		PosX = 0;
		PosY = 0;
	}

	int PosX;
	int PosY;
};

#define MAX_WORLD_BUFF_COUNT 5		//最多世界BUFF数量

//世界BUFF信息
typedef struct _WorldBuffInfo
{
	_WorldBuffInfo()
	{
		memset(EnterBuff, 0, sizeof(EnterBuff));
		memset(ExitBuff, 0, sizeof(ExitBuff));
	}

	int EnterBuff[MAX_WORLD_BUFF_COUNT];//进入BUFF
	int ExitBuff[MAX_WORLD_BUFF_COUNT];//离开BUFF
} WorldBuffInfo;

#ifdef _SERVER
//世界设定
typedef struct tagStatueUsedInfo
{
	int nMapTemplateId;
	int nNpcIndex;
public:
	tagStatueUsedInfo():nMapTemplateId(INVALID_WORLD_ID),nNpcIndex(INVALID_WORLD_INDEX)
	{/**/}

	void ClearData()
	{
		nNpcIndex = -1;
	}

}StatueUsedInfo;

class WorldSetting
{
public:
	WorldSetting();
	bool IsMatchRequirements(int playerIndex, int worldIndex = INVALID_WORLD_INDEX) const;//是否满足需求

	char Name[64];					//地图名称
	bool IsInstance;				//是否副本
	int MapId;						//地图编号
	int InstanceCountMin;			//副本最小数量
	int InstanceCountMax;			//副本最大数量
	DWORD IdleTimeBeforeRecycle;	//回收前的允许的空闲时间（即超过这个时间就会被回收）
	int OfflineMode;				//下线再上位置的模式
	DWORD LifeTime;					//生存时间（存在时间超过这个时间就会被关闭）

	int EntryCount;
	WorldEntryInfo Entrys[MAX_INSTANCE_ENTRY_COUNT];

	int PlayerCountMax;				//最大进入玩家数量
	int RequireLevelMin;			//需要最低等级
	int RequireLevelMax;			//需要最高等级
	EquipmentID RequireItem;		//需求物品
	int RequireBuff;				//需求BUFF
	int PermitProfession[6];		//允许职业
	int PermitMale;					//允许男性
	int PermitFemale;				//允许女性
	int PermitSingle;				//允许单人进入
	int PermitTeam;					//允许队伍进入
	int PermitBigTeam;				//允许大队伍进入
	int NeedOwner;					//需要拥有者（0:不需要/1:个人/2:氏族/3:国家）
	bool CanGainSroce;				//本地图（战场）上杀人是否能得分

	int	m_nPKReduceMultiple;		//PK值掉落倍速
	
	WorldBuffInfo BuffInfo;			//BUFF信息
	WorldRuntimeInfo RuntimeInfo;	//运行时信息
};
#endif

class KSubWorldSet
{
public:
	KSubWorldSet();
	BOOL Load(LPSTR szFileName);
	int SearchWorld(DWORD dwID);
	int SearchWorld(char* szWorldName);
	BOOL GetWorldNameFromID(DWORD dwID, char* szWorldName, size_t uBufLen);
	void MainLoop();
	int GetGameTime(){return m_nLoopRate;};
	void Close();

#ifdef _SERVER
	inline int GetLoadedMapCount(void) const;
	BOOL GetRevivalPosFromId(DWORD dwSubWorldId, int nRevivalId, POINT* pPos);
	int GetRevivalID( DWORD dwSubWorldId );
	bool NotifyMapIndex(int nMapId, int nIndexInUniverse);
#else	
	void SetPing(DWORD dwTimer);
	DWORD GetPing();
	void SetMusic(bool b);
	BOOL LoadMapList();
#endif

public:
	int m_nLoopRate;
#ifdef _SERVER
	static BOOL s_bNeedBlanceSpawn;
#else
	KMapMusic m_cMusic;
	static unsigned long s_uLastTime;
	static float s_fScale;
#endif

private:
	KTimer m_Timer;
	KIniFile m_MapListIni;
#ifdef _SERVER
	int m_nLaodedMapCount;
	KIniFile m_ReviveIniFile;
	DWORD m_NextSaveSpawnInfoTime;
	DWORD m_NextPingTime;
#else
	DWORD m_dwPing;
	bool m_bMusic;
#endif

public:
#ifdef _SERVER
	int CreateInstance(int worldTemplateId, int creatorPlayerIndex, DWORD lifeTime);//创建副本；返回：SubWorld序号
	int CloseInstance(int worldIndex);//关闭副本
	int GetInstance(DWORD instanceId);//得到副本；返回：SubWorld序号
	WorldSetting* GetWorldSetting(int worldTemplateId);//得到世界设定
	int EnterInstance(int playerIndex, int worldTemplateId, int entryIndex, DWORD lifeTime = 0, bool createIfNotExist = false);//进入副本（进入副本，如果指定需要创建就创建）
	int EnterInstanceByID(int playerIndex, DWORD instanceId, int entryIndex);//传送到指定ID的副本
	int CloseInstanceByID(DWORD instanceId);//关闭指定ID的副本
	DWORD GetEnterableInstanceID(int playerIndex, int worldTemplateId);//得到可进入的副本ID
	void PlayerOffLine(int playerIndex);//玩家下线
#endif

private:
#ifdef _SERVER
	int FindFreeInstanceSlot();//找到空闲的副本槽；返回：SubWorld序号
	int FindReuseableInstanceSlot(int worldTemplateId);//找到可重用的副本槽；返回：SubWorld序号
	void RecycleInstance();//回收副本
	void RecycleInstanceNotify(int worldIndex);

	WorldSetting m_WorldSetting[INSTANCE_SUBWORLD_START];
#endif

};

#ifdef _SERVER

inline int KSubWorldSet::GetLoadedMapCount(void) const
{
	return m_nLaodedMapCount;
}

inline WorldSetting* KSubWorldSet::GetWorldSetting(int worldTemplateId)
{
	if (worldTemplateId >= 0 && worldTemplateId < INSTANCE_SUBWORLD_START)
	{
		return &(m_WorldSetting[worldTemplateId]);
	}

	return NULL;
}

#else

inline void KSubWorldSet::SetPing(DWORD dwTimer)
{
	m_dwPing = dwTimer;
}

inline DWORD KSubWorldSet::GetPing()
{
	return m_dwPing;
}

inline void KSubWorldSet::SetMusic(bool b)
{
	m_bMusic = b;
}

#endif
extern KSubWorldSet g_SubWorldSet;

#ifdef _SERVER
#define MAX_WAR_MAP_NUM 4
class StatueInfoMgr
{
private:
	bool           m_bGlobalNpcLoaded;
	StatueUsedInfo m_StatueUsedInfoArray[MAX_WAR_MAP_NUM];
public:
	//Constructor and Destructor
	StatueInfoMgr();
	~StatueInfoMgr();

	static StatueInfoMgr& Singleton();


	void InitStatueUsedInfo();										//填MapID
	bool FillStatueUsedInfo(int nMapId, int nNpcIndex);				//buff调用，用于建立MapID和NpcIndex的对应关系
	void SendStatueInfoToClient(int nNpcIndex, int nPlayerIndex, DWORD dwtime, int nHasBuff);	//脚本调用
	void ReStoreStatue(int nMapId);
	void Breathe();
	void SetGlobalNpcLoaded();

	int  CanChangeLord(int nPlayerIndex);							//脚本调用，用于判断申请树立雕像的玩家的合法性
	void ForceClearStatue(int nMapId);

private://功能函数

	bool IsGlobalNpcLoaded();
	void DeleteStatue(int nNpcIndex, int iter);
	int  GetIterByMapId(int nMapId);
	bool HasStatue(int nMapId);
	bool IsStatueNeedClear(int iter);
	void ProcInvalidStatue();

	enum
	{
		not_have_purview = 0,
		database_busyness = -1,
		statue_already_establish = -2
	};
};

inline bool StatueInfoMgr::IsGlobalNpcLoaded()
{
	return m_bGlobalNpcLoaded;
}
#endif

#endif
