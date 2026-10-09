#ifndef	KRegionH
#define	KRegionH

//-----------------------------------------------------------------------
#include "KEngine.h"
#include "GameDataDef.h"
#include "KObj.h"
#include "KNpcSet.h"
#include "KNpc.h"
#include "Scene/ObstacleDef.h"
//-----------------------------------------------------------------------
#ifdef _SERVER
#define	MAX_BROADCAST_COUNT				256
#define	MAX_BROADCAST_COUNT_OPTIMIZED	256
#define	MAX_BROADCAST_COUNT_MIN			256
#else
#define	MAX_REGION		9
#endif

#ifdef _SERVER

struct KSpawnPoint
{
	unsigned int	m_uSpawnType;
	unsigned int	m_nPositionX;
	unsigned int	m_nPositionY;
	unsigned int	m_uSpawnRange;
	unsigned int	m_uCountCount;
	unsigned int	m_uSpawnTime;
    unsigned int    m_nLvlParam;
	unsigned int	m_bManaged;
    unsigned int    m_uDir;
};

struct KSpawnPoint_Runtime_Info
{
	unsigned int m_uCount;
	unsigned int m_uNextSpawnTime;
};

class KSpawnPointList
{
	friend class	KRegion;
public:
	KSpawnPointList(class KRegion* pRegion);
	~KSpawnPointList();

	void Active();

	const KSpawnPoint& operator[](unsigned int uID) const
	{
		_ASSERT(uID < m_uCount);
		return m_pData[uID];
	}
	unsigned int Size(void) const
	{
		return m_uCount;
	}

	void Release();

	bool CopyInstance(KSpawnPointList& list, int worldIndex) const;//拷贝对象数据，并构造相关对象（用于副本生成）

private:
	BOOL LoadSpawnPoints(int nSubWorld, KPakFile* pFile, unsigned int dwDataSize);	
private:
	unsigned int m_uCount;
	KSpawnPoint* m_pData;
	KSpawnPoint_Runtime_Info* m_pInfo;
	//class KRegion* m_pRegion;
	unsigned int m_uSubworld;
};
#endif

class KRegion
{
	friend class	KSubWorld;
	friend class	KSpawnPointList;
public:
	int			m_nIndex;							// 地图索引
	int			m_RegionID;
	KList		m_NpcList;							// 人物列表
	KList		m_ObjList;							// 物件列表

#ifndef _SERVER
	KList		m_MissleList;						// 子弹列表
#endif

	KList		m_PlayerList;						// 玩家列表
	int			m_nConnectRegion[8];				// 相邻的地图索引
	int			m_nConRegionID[8];					// 相邻的地图ID
	int			m_nRegionX;							// 在世界中的位置X（象素点）
	int			m_nRegionY;							// 在世界中的位置Y（象素点）
#ifdef _SERVER
	KSpawnPointList m_SpawnPointList;
#endif
private:
#ifdef _SERVER
	// lixuewu 补充说明
	// 说明: 一个long的四个字节中
	// 最低一个字节 高四位表示 Obstacle_Type 低四位表示 Obstacle_Kind
	// 最高一个字节表示 阻挡造成的原因 Obstacle_Reason
	long		m_Obstacle[REGION_CELL_WIDTH][REGION_CELL_HEIGHT];	// 地图障碍信息表
	DWORD		m_dwTrap[REGION_CELL_WIDTH][REGION_CELL_HEIGHT];	// 地图trap信息表
#endif
	int			m_nNpcSyncCounter;					// 同步计数器
	int			m_nObjSyncCounter;
	int			m_nActive;							// 是否激活（是否有玩家在附近）
	int			m_nObjRef[REGION_CELL_WIDTH * REGION_CELL_HEIGHT];	// 格子上的OBJ
	BYTE		m_nNpcRef[REGION_CELL_WIDTH * REGION_CELL_HEIGHT];	// 格子上的NPC
public:
	KRegion();
	~KRegion();
	BOOL		Init(int nWidth, int nHeight);
	BOOL		Load(int nX, int nY);
#ifdef _SERVER
	// 载入服务器端地图上本region 的 object数据（包括npc、trap、box等）
	BOOL		LoadObject(int nSubWorld, int nX, int nY);
	// 载入服务器端地图上本 region 的障碍数据
	BOOL		LoadServerObstacle(KPakFile *pFile, DWORD dwDataSize);
	// 载入服务器端地图上本 region 的 trap 数据
	BOOL		LoadServerTrap(KPakFile *pFile, DWORD dwDataSize);
	// 载入服务器端地图上本 region 的 npc 数据
	BOOL		LoadServerNpc(int nSubWorld, KPakFile *pFile, DWORD dwDataSize);

	//-----------------------------------------------------------------------------
	// 暂时屏蔽，该功能不再使用。
	// 载入服务器端地图上本 region 的 obj 数据
	//BOOL		LoadServerObj(int nSubWorld, KPakFile *pFile, DWORD dwDataSize);
	//-----------------------------------------------------------------------------

	//拷贝Region静态数据，并构造相关对象（用于副本生成）
	bool		CopyInstance(KRegion* pRegion, int worldIndex) const;
#endif

#ifndef _SERVER
	// 载入客户端地图上本region 的 object数据（包括npc、box等）
	// 如果 bLoadNpcFlag == TRUE 需要载入 clientonly npc else 不载入
	BOOL		LoadObject(int nSubWorld, int nX, int nY, char *lpszPath);
	// 载入客户端地图上本 region 的 clientonlynpc 数据
	BOOL		LoadClientNpc(KPakFile *pFile, DWORD dwDataSize);
	// 载入客户端地图上本 region 的 clientonlyobj 数据
	BOOL		LoadClientObj(KPakFile *pFile, DWORD dwDataSize);
	// 载入障碍数据给小地图
	static void		LoadLittleMapData(int nX, int nY, char *lpszPath, BYTE *lpbtObstacle);
#endif
	void		Close();
	void		Activate();
	BYTE		GetBarrier(int MapX, int MapY, int nDx, int nDy);	//	地图高度

	// 按 像素点坐标 * 1024 的精度判断某个位置是否障碍
	// 参数：nGridX nGirdY ：本region格子坐标
	// 参数：nOffX nOffY ：格子内的偏移量(像素点 * 1024 精度)
	// 参数：bCheckNpc ：是否判断npc形成的障碍
	// 返回值：障碍类型(if 类型 == Obstacle_NULL 无障碍)
	BYTE		GetBarrierMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc);

	DWORD		GetTrap(int MapX, int MapY);						//	得到Trap编号
	inline BOOL		IsActive() 
	{
#ifdef _SERVER
		return m_nActive; 
#else
		return TRUE;
#endif
	};
	int			GetObj(unsigned int nMapX,unsigned int nMapY);
	BOOL		CanBuild(unsigned int nSubWorldIndex,unsigned int nMapX, unsigned int nMapY, unsigned int uW,unsigned int uH,BOOL bCheckTerrain);
	BOOL		AddNpcRef(int nIndex);
	BOOL		DecNpcRef(int nIndex);
	int			GetNpcRef(int nMapX, int nMapY);
	int			FindNpc(int nMapX, int nMapY, int nNpcIdx, int nRelation);
	int			FindNpcList(int nMapX, int nMapY, int nNpcIdx, int nRelation); // 原有版本的FindNpc
	int			FindEquip(int nMapX, int nMapY);
	int			FindObject(unsigned int nMapX,unsigned int nMapY);
	int			FindObject(int nObjID);
	void*		GetObjNode(int nIdx);
	int			SearchNpc(DWORD dwNpcID);		// 寻找本区域内是否有某个指定 id 的 npc (zroc add)
#ifdef _SERVER
	void		SendSyncData(int nClient, int nNpcIndex );
	void		BroadCast(const void *pBuffer, DWORD dwSize, int &nMaxCount, int nX, int nY);
	int			FindPlayer(DWORD dwId);
	int			FindPlayer(FSGUID& guid);
	BOOL		CheckPlayerIn(int nPlayerIdx);
	void		SetTrap(DWORD nTrapId, int nMapX, int nMapY);
	void		RemoveAllSpawnNpc();
#endif

	void		AddNpc(int nIdx);
	void		RemoveNpc(int nIdx);

#ifndef _SERVER
	void		AddMissle(int nIdx);
	void		RemoveMissle(int nIdx);
#endif

	void		AddObj(int nIdx);
	void		RemoveObj(int nIdx);
	BOOL		AddPlayer(int nIdx);
	BOOL		RemovePlayer(int nIdx);

	BOOL		TestObjBarrier(KObj& aObj, int nMapX, int nMapY);
	void		AddObjBarrier(KObj& aObj, int nMapX, int nMapY, enum Obstacle_Kind);
	void		ClearObjBarrier(KObj& aObj, int nMapX, int nMapY);

#ifdef _SERVER
	void Release();
	void KickAllPlayer();//踢出该区域中所有玩家（踢回重生点）
#endif
};

//--------------------------------------------------------------------------
//	Find Npc
//--------------------------------------------------------------------------
inline int KRegion::FindNpc(int nMapX, int nMapY, int nNpcIdx, int nRelation)
{

	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_NpcList.GetHead();
	
	while(pNode)
	{		
		if (IsValidNpc(pNode->m_nIndex) && Npc[pNode->m_nIndex].GetMapX() == nMapX && Npc[pNode->m_nIndex].GetMapY() == nMapY)
		{
			if (NpcSet.GetRelation(nNpcIdx, pNode->m_nIndex) & nRelation)
			{
				return pNode->m_nIndex;
			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}


//--------------------------------------------------------------------------
//	Find Npc in NpcList
// 原来版本的FindNpc 用于处理子弹
//--------------------------------------------------------------------------
inline int KRegion::FindNpcList(int nMapX, int nMapY, int nNpcIdx, int nRelation)
{

	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_NpcList.GetHead();
	
	while(pNode)
	{
		if (Npc[pNode->m_nIndex].GetMapX() == nMapX 
			&& Npc[pNode->m_nIndex].GetMapY() == nMapY)
		{
			if (NpcSet.GetRelation(nNpcIdx, pNode->m_nIndex) & nRelation)
			{
				if (nRelation != relation_enemy)
				{
					return pNode->m_nIndex;
				}
				else
				{
					if (Npc[pNode->m_nIndex].m_nSafeGuardLevel == 0)
					{
						return pNode->m_nIndex;
					}
					else
					{
						if(Npc[nNpcIdx].m_Kind == kind_normal)
						{
							if (Npc[pNode->m_nIndex].m_nSafeGuardLevel <= 1)
							{
								return pNode->m_nIndex;
							}
						}
					}
				}

			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

#endif
