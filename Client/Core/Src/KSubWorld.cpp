#include "KCore.h" 
#include <math.h>
#include "KRegion.h"
#include "KMath.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KNpcSet.h"
#include "KObjSet.h"
#include "KPlayerSet.h"
#include "KSubWorldSet.h"
#ifndef _STANDALONE
#include "crtdbg.h"
#endif
#include "Scene/ObstacleDef.h"
#ifdef _SERVER
#include "CoreRelated.h"
#include "KWarInfoManager.h"
#include "ServerSocialUnitMgr.h"
#include "SocialUnit.h"
#endif
#ifndef _SERVER
#include "scene/KScenePlaceC.h"
#endif
#include "KSubWorld.h"

#include "KNpcTemplate.h"
KSubWorld SubWorld[MAX_SUBWORLD];

#ifdef _SERVER
//KGlobalMissionArray g_GlobalMissionArray;
#include "buff_man.h"
#include "ScriptFuns.h"
#endif

//////////////////////////////////////////////////////////////////////////

// #ifdef _SERVER
// int	KSubWorld::m_nMasterStatusNpcIndex = 0;	// 国王雕像Npc索引.
// #endif // _SERVER

#ifdef _SERVER
DWORD KSubWorld::s_RecycleCount = 0;
DWORD SpawnRecoder::s_SpawnCount = 0;
struct CombatTop10PlayerTempArray 
{
	int nPlayerIndex;
	int nscore;

	CombatTop10PlayerTempArray(void):nPlayerIndex(0),nscore(0){}

};
#endif


KSubWorld::KSubWorld()
{
	m_nIndex= INVALID_WORLD_INDEX;
	m_SubWorldID = INVALID_WORLD_ID;
	m_InstanceID = INVALID_INSTANCE_ID;
	m_ExpireTime = 0;
	
	m_Region = NULL;
	m_dwCurrentTime = 0;	

	for(int especialAreaIndex = 0; especialAreaIndex < MAX_ESAREA; especialAreaIndex++)
	{
		m_nEspecialArea[especialAreaIndex].nType = especial_area_none;
	}
	m_nAllType = especial_area_none;

	m_nSyncType        = especial_sync_type_normal;
	m_nMapChatInterval = DEFAULT_MAP_ROOM_CHAT_INTERVAL;

#ifdef _SERVER
	m_nSyncIntervalSec = MIN_SYNC_INTERVAL_SEC;
	m_State = world_state_init;
	m_LastPlayerExitTime = 0;
	m_nLordOfWorld		=	-1;
	m_nRobberOfWorld	=	-1;
	m_nPoolOfWorld      =   -1;
	m_nSubPoolOfWorld   =   -1;
	m_nGroup			=	-1;
	m_nCate				=	-1;
	
	memset( &m_SubLordOfWorld, -1, sizeof(m_SubLordOfWorld) );

	m_nPlayerCount = 0;	// 统计人数
	m_nWorldRegionWidth = 0;
	m_nWorldRegionHeight = 0;
	m_nIndexInUniverse = 0;
	
	memset(m_CustomVariable, 0, sizeof(m_CustomVariable));
	memset(m_CustomString, 0, sizeof(m_CustomString));
	for (int teamIndex = 0; teamIndex < MAX_WORLD_TEAM_COUNT; teamIndex++)
	{
		m_WorldTeam[teamIndex] = INVALID_TEAM_ID;
	}
	m_OwnerPlayerIndex = INVALID_PLAYER_INDEX;
	m_pIterateNode = NULL;
	m_IterateRegion = -1;
	m_SyncToWorldNpcs.clear();
	m_SyncListIndex = 0;
	m_NextSyncCounter = 0;

	for (int nFlag = 0; nFlag < MAX_MAP_FLAGS ; nFlag ++ )
	{
		m_MapFlag[nFlag] = 0;
	}//end for nFlag

#else
	m_nWorldRegionWidth  = 3;
	m_nWorldRegionHeight = 3;
	memset(m_ClientRegionIdx, 0, sizeof(this->m_ClientRegionIdx));
	memset(this->m_szMapPath, 0, sizeof(this->m_szMapPath));
    memset(&m_WorldCombatClientInfo, 0 ,sizeof(m_WorldCombatClientInfo));
#endif

	m_nTotalRegion    = m_nWorldRegionWidth * m_nWorldRegionHeight;
	m_IsCombatInstance= false;
	m_CombatScoreCalcType = PROGRAME_CALU;
}

KSubWorld::~KSubWorld()
{
	//TODO 施放对象
}

int KSubWorld::FindRegion(int nRegion)
{
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		if (m_Region[i].m_RegionID == nRegion)
			return i;
	}
	return -1;
}

int KSubWorld::FindFreeRegion(int nX, int nY)
{
	if (nX == 0 && nY == 0)
	{
		for (int i = 0; i < m_nTotalRegion; i++)
		{
			if (m_Region[i].m_RegionID == -1)
				return i;
		}
	}
	else
	{
		for (int i = 0; i < m_nTotalRegion; i++)
		{
			if (m_Region[i].m_RegionID == -1)
				return i;
			int nRegoinX = LOWORD(m_Region[i].m_RegionID);
			int nRegoinY = HIWORD(m_Region[i].m_RegionID);

			if ((nX - nRegoinX) * (nX - nRegoinX) + (nY - nRegoinY) * (nY - nRegoinY) > 2)	// 不在附近
				return i;
		}
	}
	return -1;
}

extern int nActiveRegionCount;

void KSubWorld::Activate()
{
	if (m_SubWorldID == INVALID_WORLD_ID)
		return;

#ifdef _SERVER
	switch(m_State)
	{
	case world_state_init:
		return;
	case world_state_ready_for_reuse:
		return;
	case world_state_idle:
		m_dwCurrentTime ++;
		CheckForExpire();
		return;
	case world_state_active:
		m_dwCurrentTime ++;
		if (CheckForExpire())
		{
			return;
		}
		break;
	default:
		return;
	}
#else
	m_dwCurrentTime ++;
#endif

	for (int i = 0; i < m_nTotalRegion; i++)
	{
		if (m_Region[i].IsActive())
		{
			m_Region[i].Activate();
			nActiveRegionCount++;
		}
	}

#ifdef _SERVER

    if ((m_dwCurrentTime % GAME_FPS) == 0)
    {
       m_clsWorldSpawner.Active(UNIX_TMIE_STAMP);
    }

    KIndexNode* pNode = (KIndexNode *)m_NoneRegionNpcList.GetHead();
	while(pNode)
	{
		Npc[pNode->m_nIndex].Activate();
		
		pNode = (KIndexNode *)pNode->GetNext();
	}

	if ( IsWorldCombatMap() ) //special case for combat map which the orgnization sematics is meanfor
	{
		WorldCombatMapActive();
	}//endif

	SyncTongWarInfo();

#define SYNC_TO_WORLD_NPC_PERIOD (GAME_FPS * 1)//世界同步NPC同步周期，即在这个时间内，所有的世界同步NPC都需要被同步一次

	//世界同步NPC
	//如下的算法使得同步基本分布在一个同步周期内
	const int syncToWorldNpcCount = m_SyncToWorldNpcs.size();	
	if (syncToWorldNpcCount > 0)
	{
		m_NextSyncCounter--;
		if (m_NextSyncCounter < 0)
		{
			m_NextSyncCounter = SYNC_TO_WORLD_NPC_PERIOD / syncToWorldNpcCount;
			const int syncStepSize = (syncToWorldNpcCount / SYNC_TO_WORLD_NPC_PERIOD) + 1;
			int syncCount = 0;
			while (syncCount < syncStepSize)
			{
				m_SyncListIndex = m_SyncListIndex % syncToWorldNpcCount;
				
				if (IsValidNpc(m_SyncToWorldNpcs[m_SyncListIndex]))
				{
					Npc[m_SyncToWorldNpcs[m_SyncListIndex]].SyncToWorld();
				}
				
				m_SyncListIndex++;
				syncCount++;
			}
		}
	}
#else
	
	NpcSet.ClearActivateFlagOfAllNpc();
	
#endif
	
}

int KSubWorld::GetDistance(int nRx1, int nRy1, int nRx2, int nRy2)
{
	//return (int)sqrt((float)((nRx1 - nRx2) * (nRx1 - nRx2) + (nRy1 - nRy2) * (nRy1 - nRy2)));
	//////////////////////////////////////////////////////////////////////////
	// 优化平方根表
	return g_GetDistance(nRx1, nRy1, nRx2, nRy2);
}

void KSubWorld::Map2Mps(int nR, int nX, int nY, int nDx, int nDy, int *nRx, int *nRy)
{
/*
#ifdef TOOLVERION
	*nRx = nX;
	*nRy = nY;
	return;
#endif
*/
#ifndef _SERVER
//	_ASSERT(nR >= 0 && nR < 9);
#endif
	//_ASSERT(nR >= 0);

	if (nR < 0 || nR >= m_nTotalRegion)
	{
		*nRx = 0;
		*nRy = 0;
		return;
	}

	int x, y;
	
	x = m_Region[nR].m_nRegionX;
	y = m_Region[nR].m_nRegionY;
	
	//x += nX * REGION_CELL_SIZE_X;
	//y += nY * REGION_CELL_SIZE_Y;
	x += nX * REGION_CELL_SIZE_X;
	y += nY * REGION_CELL_SIZE_Y;
	
	x += (nDx >> 10);
	y += (nDy >> 10);
	
	*nRx = x;
	*nRy = y;
}

void KSubWorld::Map2Mps(int nRx, int nRy, int nX, int nY, int nDx, int nDy, int *pnX, int *pnY)
{
	*pnX = (nRx * REGION_CELL_WIDTH + nX) * REGION_CELL_SIZE_X + (nDx >> 10);
	*pnY = (nRy * REGION_CELL_WIDTH + nY) * REGION_CELL_SIZE_Y + (nDy >> 10);
}

void KSubWorld::Mps2Map(int Rx, int Ry, int * nR, int * nX, int * nY, int *nDx, int * nDy)
{
	//if (REGION_CELL_SIZE_X == 0 || REGION_CELL_SIZE_Y == 0 || REGION_CELL_WIDTH == 0 || REGION_CELL_HEIGHT == 0)
	//	return;
	int x = Rx / REGION_PIXEL_WIDTH;//(REGION_CELL_WIDTH * REGION_CELL_SIZE_X);
	int	y = Ry / REGION_PIXEL_HEIGHT;//(REGION_CELL_HEIGHT * REGION_CELL_SIZE_Y);

	*nX = 0;
	*nY = 0;
	*nDx = 0;
	*nDy = 0;
#ifdef _SERVER
	// 非法的坐标
	if (x >= m_nWorldRegionWidth + m_nRegionBeginX || y >= m_nWorldRegionHeight + m_nRegionBeginY || x < m_nRegionBeginX || y < m_nRegionBeginY)
	{
		*nR = -1;
		return;
	}
#endif
	// 非法的坐标
#ifdef _SERVER
	*nR = GetRegionIndex(MAKELONG(x, y));
#else
	int nRegionID = MAKELONG(x, y);
	*nR = FindRegion(nRegionID);
	if (*nR == -1)
		return;
#endif
	if (*nR >= m_nTotalRegion)
	{
		*nR = -1;
		return;
	}

	x = Rx - m_Region[*nR].m_nRegionX;
	y = Ry - m_Region[*nR].m_nRegionY;

	*nX = x / REGION_CELL_SIZE_X;
	*nY = y / REGION_CELL_SIZE_Y;

	*nDx = (x - *nX * REGION_CELL_SIZE_X) << 10;
	*nDy = (y - *nY * REGION_CELL_SIZE_Y) << 10;
}

BYTE	KSubWorld::TestBarrier(int nMpsX, int nMpsY)
{
//	if (REGION_CELL_SIZE_X == 0 || REGION_CELL_SIZE_Y == 0 || REGION_CELL_WIDTH == 0 || REGION_CELL_HEIGHT == 0)
//		return 0xff;

	int x = nMpsX / REGION_PIXEL_WIDTH ; //(REGION_CELL_WIDTH * REGION_CELL_SIZE_X);
	int	y = nMpsY / REGION_PIXEL_HEIGHT ; //(REGION_CELL_HEIGHT * REGION_CELL_SIZE_Y);
#ifdef _SERVER
	// 非法的坐标
	if (x >= m_nWorldRegionWidth + m_nRegionBeginX || y >= m_nWorldRegionHeight + m_nRegionBeginY || x < m_nRegionBeginX || y < m_nRegionBeginY)
		return 0xff;
	int nRegion = GetRegionIndex(MAKELONG(x, y));
#else
	int nRegion = FindRegion(MAKELONG(x, y));
	if (nRegion == -1)
		return 0xff;
#endif
	if (nRegion >= m_nTotalRegion)
		return 0xff;

	x = nMpsX - m_Region[nRegion].m_nRegionX;
	y = nMpsY - m_Region[nRegion].m_nRegionY;

	int nCellX = x / REGION_CELL_SIZE_X;
	int nCellY = y / REGION_CELL_SIZE_Y;

	int nOffX = x - nCellX * REGION_CELL_SIZE_X;
	int nOffY = y - nCellY * REGION_CELL_SIZE_Y;

#ifndef _SERVER
	BYTE bRet = m_Region[nRegion].GetBarrier(nCellX, nCellY, nOffX, nOffY);
	if (bRet != Obstacle_NULL)
		return bRet;
	return 	(BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);
#endif
#ifdef _SERVER
	return m_Region[nRegion].GetBarrier(nCellX, nCellY, nOffX, nOffY);
#endif
}

BYTE KSubWorld::TestBarrier(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY)
{
	int nOldMapX = nMapX;
	int nOldMapY = nMapY;
	int nOldRegion = nRegion;

	nDx += nChangeX;
	nDy += nChangeY;

	if (nDx < 0)
	{
		nDx += (REGION_CELL_SIZE_X << 10);
		nMapX--;
	}
	else if (nDx >= (REGION_CELL_SIZE_X << 10))
	{
		nDx -= (REGION_CELL_SIZE_X << 10);
		nMapX++;
	}
	
	if (nDy < 0)
	{
		nDy += (REGION_CELL_SIZE_Y << 10);
		nMapY--;
	}
	else if (nDy >= (REGION_CELL_SIZE_Y << 10))
	{
		nDy -= (REGION_CELL_SIZE_Y << 10);
		nMapY++;
	}
	
	if (nMapX < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_LEFT] == -1)
			return 0xff;

		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_LEFT];
		nMapX += REGION_CELL_WIDTH;
	}
	else if (nMapX >= REGION_CELL_WIDTH)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_RIGHT] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_RIGHT];
		nMapX -= REGION_CELL_WIDTH;
	}
	
	if (nMapY < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_UP] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_UP];;
		nMapY += REGION_CELL_HEIGHT;
	}
	else if (nMapY >= REGION_CELL_HEIGHT)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_DOWN] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_DOWN];
		nMapY -= REGION_CELL_HEIGHT;
	}

	int nXf, nYf;
	nXf = (nDx >> 10);
	nYf = (nDy >> 10);

#ifndef _SERVER

	int nMpsX, nMpsY;
	Map2Mps(nRegion, nMapX, nMapY, nDx, nDy, &nMpsX, &nMpsY);
	BYTE bRet = m_Region[nRegion].GetBarrier(nMapX, nMapY, nXf, nYf);
	if (bRet != Obstacle_NULL)
		return bRet;
//	if (nMapX == nOldMapX && nMapY == nOldMapY && nRegion == nOldRegion)
//		return Obstacle_NULL;

	return (BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);
#else
//	if (nMapX == nOldMapX && nMapY == nOldMapY && nRegion == nOldRegion)
//		return 0;
	return m_Region[nRegion].GetBarrier(nMapX, nMapY, nXf, nYf);
#endif
}

BYTE KSubWorld::TestBarrierMin(int nRegion, int nMapX, int nMapY, int nDx, int nDy, int nChangeX, int nChangeY, BOOL bCheckNpc)
{
	int nOldMapX = nMapX;
	int nOldMapY = nMapY;
	int nOldRegion = nRegion;

	nDx += nChangeX;
	nDy += nChangeY;

	if (nDx < 0)
	{
		nDx += (REGION_CELL_SIZE_X << 10);
		nMapX--;
	}
	else if (nDx >= (REGION_CELL_SIZE_X << 10))
	{
		nDx -= (REGION_CELL_SIZE_X << 10);
		nMapX++;
	}
	
	if (nDy < 0)
	{
		nDy += (REGION_CELL_SIZE_Y << 10);
		nMapY--;
	}
	else if (nDy >= (REGION_CELL_SIZE_Y << 10))
	{
		nDy -= (REGION_CELL_SIZE_Y << 10);
		nMapY++;
	}
	
	if (nMapX < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_LEFT] == -1)
			return 0xff;

		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_LEFT];
		nMapX += REGION_CELL_WIDTH;
	}
	else if (nMapX >= REGION_CELL_WIDTH)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_RIGHT] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_RIGHT];
		nMapX -= REGION_CELL_WIDTH;
	}
	
	if (nMapY < 0)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_UP] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_UP];;
		nMapY += REGION_CELL_HEIGHT;
	}
	else if (nMapY >= REGION_CELL_HEIGHT)
	{
		if (m_Region[nRegion].m_nConnectRegion[DIR_DOWN] == -1)
			return 0xff;
		nRegion = m_Region[nRegion].m_nConnectRegion[DIR_DOWN];
		nMapY -= REGION_CELL_HEIGHT;
	}

#ifndef _SERVER

	// 20070509 by mixcooler
	// {{
	BYTE bRet = Obstacle_NULL;
	if (nMapX == nOldMapX && nMapY == nOldMapY && nRegion == nOldRegion)
	{
		bRet = m_Region[nRegion].GetBarrierMin(nMapX, nMapY, nDx, nDy, FALSE);
	}
	else
	{
		bRet = m_Region[nRegion].GetBarrierMin(nMapX, nMapY, nDx, nDy, bCheckNpc);
	}
	// }}
	if (bRet != Obstacle_NULL)
		return bRet;

	int nMpsX, nMpsY;
	Map2Mps(nRegion, nMapX, nMapY, nDx, nDy, &nMpsX, &nMpsY);
	return (BYTE)g_ScenePlace.GetObstacleInfoMin(nMpsX, nMpsY, nDx & 0x7fff, nDy & 0x7fff);
#else

	if (nMapX == nOldMapX && nMapY == nOldMapY && nRegion == nOldRegion)
	{
		return m_Region[nRegion].GetBarrierMin(nMapX, nMapY, nDx, nDy, FALSE);
	}

	return m_Region[nRegion].GetBarrierMin(nMapX, nMapY, nDx, nDy, bCheckNpc);
#endif
}

DWORD KSubWorld::GetTrap(int nMpsX, int nMpsY)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return 0;
	return m_Region[nRegion].GetTrap(nMapX, nMapY);
}

BYTE KSubWorld::GetBarrier(int nMpsX, int nMpsY)
{
#ifdef _SERVER
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	
	if (nRegion == -1)
		return 0xff;
	return m_Region[nRegion].GetBarrier(nMapX, nMapY, (nOffX >> 10), (nOffY) >> 10);
#else
	return (BYTE)g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY);
#endif
}

#ifdef _SERVER
#define SIEGEWORLDIDBEGIN 100
BOOL KSubWorld::LoadMap(int nId)
{
	KIniFile	IniFile;
	char		szPathName[FILE_NAME_LENGTH];
	char		szFileName[FILE_NAME_LENGTH];
	char		szMapName[FILE_NAME_LENGTH];
	char		szKeyName[32];
	
	m_SubWorldID = nId;
	IniFile.Load("settings/MapList.ini");
	
	sprintf(szKeyName, "%d", nId);
	IniFile.GetString("List", szKeyName, "", szPathName, sizeof(szPathName));
	szPathName[sizeof(szPathName) - 1] = 0;

	sprintf(szKeyName, "%d_name", nId);
	IniFile.GetString("List", szKeyName, "", szMapName, sizeof(szMapName));
	szMapName[sizeof(szMapName) - 1] = 0;

	sprintf(szFileName, "maps/%s.wor", szPathName);

	if (!IniFile.Load(szFileName))
		return FALSE;

//	m_nRegionWidth = 512 / 32;
//	REGION_CELL_HEIGHT = 1024 / 32;

	RECT	sRect;
	IniFile.GetRect("MAIN", "rect", &sRect);
	m_nRegionBeginX = sRect.left;
	m_nRegionBeginY = sRect.top;
	m_nWorldRegionWidth = sRect.right - sRect.left + 1;
	m_nWorldRegionHeight = sRect.bottom - sRect.top + 1;
	m_nTotalRegion = m_nWorldRegionWidth * m_nWorldRegionHeight;
	
//	m_nCellWidth = defLOGIC_CELL_WIDTH;
//	REGION_CELL_SIZE_Y = defLOGIC_CELL_HEIGHT;

	if (m_nTotalRegion <= 0 || m_nWorldRegionWidth <= 0 || m_nWorldRegionHeight <= 0)
		return FALSE;

	m_Region = new KRegion[m_nTotalRegion];

	int		nX, nY, nIdx;

	// 加载地图障碍，并连接各Region
	for (nY = 0; nY < m_nWorldRegionHeight; nY++)
	{
		for (nX = 0; nX < m_nWorldRegionWidth; nX++)
		{
			nIdx = nY * m_nWorldRegionWidth + nX;
			if (m_Region[nIdx].Load(nX + m_nRegionBeginX, nY + m_nRegionBeginY))
			{
				m_Region[nIdx].Init(REGION_CELL_WIDTH, REGION_CELL_HEIGHT);
				m_Region[nIdx].m_nIndex = nIdx;
			}
			for (int i = 0; i < 8; i++)
			{
				short nTmpX, nTmpY;
				nTmpX = (short)LOWORD(m_Region[nIdx].m_nConRegionID[i]) - m_nRegionBeginX;
				nTmpY = (short)HIWORD(m_Region[nIdx].m_nConRegionID[i]) - m_nRegionBeginY;
				if (nTmpX < 0 || nTmpY < 0 || nTmpX >= m_nWorldRegionWidth || nTmpY >= m_nWorldRegionHeight)
				{
					m_Region[nIdx].m_nConnectRegion[i] = -1;
					continue;
				}
				int nConIdx = nTmpY * m_nWorldRegionWidth + nTmpX;
				m_Region[nIdx].m_nConnectRegion[i] = nConIdx;
			}
		}
	}

	char	szPreCurPath[MAX_PATH] = { 0 };
	char	szRepairFilePath[MAX_PATH] = { 0 };
	
	sprintf(szRepairFilePath, "maps/%s", szPathName);
	g_GetFilePath(szPreCurPath);

	for (nY = 0; nY < m_nWorldRegionHeight; nY++)
	{
		for (nX = 0; nX < m_nWorldRegionWidth; nX++)
		{
			g_SetFilePath(szRepairFilePath);
			
			m_Region[nY * m_nWorldRegionWidth + nX].LoadObject(m_nIndex,nX + m_nRegionBeginX, nY + m_nRegionBeginY);
			g_SetFilePath(szPreCurPath);
		}
	}

	//Load Especial Area
#define KEYLEN	32
#define CONTENTLEN	256
	
	KIniFile	ESPAREIniFile;
	char		szAreaKey[KEYLEN];
	char		szAreaSec[KEYLEN];
	
	if (!ESPAREIniFile.Load("\\settings\\especialearea.ini"))
		return FALSE;
	
	int nType;
	int nCount;
	sprintf( szAreaSec, "%d", nId );
	ESPAREIniFile.GetInteger( szAreaSec, "alltype", -1, &nType );
	SetAllEspecialType( nType );

	ESPAREIniFile.GetInteger( szAreaSec, "group", -1, &m_nGroup );
	ESPAREIniFile.GetInteger( szAreaSec, "cate", -1, &m_nCate );
	
	int isWorldCombatInstance  = FALSE;
	ESPAREIniFile.GetInteger(szAreaSec,"IsCombatInstance",FALSE,&(isWorldCombatInstance));
	m_IsCombatInstance = (isWorldCombatInstance == TRUE);

	int nSyncType = especial_sync_type_normal;
	ESPAREIniFile.GetInteger(szAreaSec,"syncflag",especial_sync_type_normal,&(nSyncType));
	if (nSyncType < especial_sync_type_normal || nSyncType >= espacial_sync_type_num )
		nSyncType = especial_sync_type_normal ; 
	m_nSyncType = nSyncType;

	m_nSyncIntervalSec = MIN_SYNC_INTERVAL_SEC;
	ESPAREIniFile.GetInteger(szAreaSec,"syncinterval",MIN_SYNC_INTERVAL_SEC,&(m_nSyncIntervalSec));
	if (m_nSyncIntervalSec < MIN_SYNC_INTERVAL_SEC )
		m_nSyncIntervalSec = MIN_SYNC_INTERVAL_SEC;

	ESPAREIniFile.GetInteger(szAreaSec,"CombatScoreCalcType",PROGRAME_CALU, &(m_CombatScoreCalcType));


	m_nMapChatInterval         = DEFAULT_MAP_ROOM_CHAT_INTERVAL;
	int nNormalMapChatInterval = ConfigManager::Singleton().GetGlobalVariable(global_var_maproomchat_timeinterval);
	ESPAREIniFile.GetInteger(szAreaSec,"MapRoomChatInterval",nNormalMapChatInterval,&m_nMapChatInterval);
	if (m_nMapChatInterval < MIN_MAP_ROOM_CHAT_INTERVAL)
		m_nMapChatInterval = MIN_MAP_ROOM_CHAT_INTERVAL;

	char szFlagInfoSection[64] = "";
	for (int nFlag = 0; nFlag < MAX_MAP_FLAGS ; nFlag ++ )
	{
		sprintf(szFlagInfoSection,"Flage%d",nFlag);
		ESPAREIniFile.GetInteger(szAreaSec,szFlagInfoSection,0,&m_MapFlag[nFlag]);
	}//end for nFlag

	
	WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(nId);
	if (pSetting)
	{
		if (strlen(szMapName) == 0)
		{
			strncpy(pSetting->Name, szPathName, sizeof(szPathName));
		}
		else
		{
			strncpy(pSetting->Name, szMapName, sizeof(pSetting->Name));
		}
		
		pSetting->MapId = nId;

		int isInstance = FALSE;
		ESPAREIniFile.GetInteger(szAreaSec, "IsInstance", 0, &isInstance);				
		pSetting->IsInstance = (isInstance == TRUE);

#ifdef _SERVER
		ESPAREIniFile.GetInteger(szAreaSec, "ReduceMultiple", 1, &pSetting->m_nPKReduceMultiple);
#endif

		if (pSetting->IsInstance)
		{
			ESPAREIniFile.GetInteger(szAreaSec, "InstanceCountMin", 0, &(pSetting->InstanceCountMin));
			ESPAREIniFile.GetInteger(szAreaSec, "InstanceCountMax", 1, &(pSetting->InstanceCountMax));
			
			int idleTimeBeforeRecycleSeconds = 0;
			ESPAREIniFile.GetInteger(szAreaSec, "IdleTimeBeforeRecycle", 60, &(idleTimeBeforeRecycleSeconds));
			pSetting->IdleTimeBeforeRecycle = GAME_FPS * idleTimeBeforeRecycleSeconds;
			
			ESPAREIniFile.GetInteger(szAreaSec, "OfflineMode", 0, &(pSetting->OfflineMode));
			ESPAREIniFile.GetInteger(szAreaSec, "LifeTime", 0, (int*)(&(pSetting->LifeTime)));
			ESPAREIniFile.GetInteger(szAreaSec, "PlayerCountMax", MAX_BIG_TEAM_MEMBER, &(pSetting->PlayerCountMax));
			ESPAREIniFile.GetInteger(szAreaSec, "RequireLevelMin", 1, &(pSetting->RequireLevelMin));
			ESPAREIniFile.GetInteger(szAreaSec, "RequireLevelMax", MAX_LEVEL, &(pSetting->RequireLevelMax));
			ESPAREIniFile.GetInteger(szAreaSec, "RequireBuff", 0, &(pSetting->RequireBuff));
			
			ESPAREIniFile.GetInteger(szAreaSec, "EntryCount", 0, &(pSetting->EntryCount));
			for (int entryIndex = 0; entryIndex < pSetting->EntryCount; entryIndex++)
			{
				char keyName[32] = { 0 };
				sprintf(keyName, "Entry%dPosX", entryIndex + 1);
				ESPAREIniFile.GetInteger(szAreaSec, keyName, 0, &(pSetting->Entrys[entryIndex].PosX));
				sprintf(keyName, "Entry%dPosY", entryIndex + 1);
				ESPAREIniFile.GetInteger(szAreaSec, keyName, 0, &(pSetting->Entrys[entryIndex].PosY));
			}
			
			char requireItem[32] = { 0 };
			ESPAREIniFile.GetString(szAreaSec, "RequireItem", "", requireItem, sizeof(requireItem));
			if (strlen(requireItem) > 0)
			{
				::sscanf(requireItem, "%d,%d,%d,%d,%d,%d",
					&(pSetting->RequireItem.IDArray[0]),
					&(pSetting->RequireItem.IDArray[1]),
					&(pSetting->RequireItem.IDArray[2]),
					&(pSetting->RequireItem.IDArray[3]));
			}
			
			char permitProfession[32] = { 0 };
			ESPAREIniFile.GetString(szAreaSec, "PermitProfession", "", permitProfession, sizeof(permitProfession));
			if (strlen(permitProfession) > 0)
			{
				::sscanf(permitProfession, "%d,%d,%d,%d,%d,%d",
					&(pSetting->PermitProfession[0]),
					&(pSetting->PermitProfession[1]),
					&(pSetting->PermitProfession[2]),
					&(pSetting->PermitProfession[3]),
					&(pSetting->PermitProfession[4]),
					&(pSetting->PermitProfession[5]));
			}
			
			ESPAREIniFile.GetInteger(szAreaSec, "PermitMale", 1, &(pSetting->PermitMale));
			ESPAREIniFile.GetInteger(szAreaSec, "PermitFemale", 1, &(pSetting->PermitFemale));
			ESPAREIniFile.GetInteger(szAreaSec, "PermitSingle", 1, &(pSetting->PermitSingle));
			ESPAREIniFile.GetInteger(szAreaSec, "PermitTeam", 1, &(pSetting->PermitTeam));
			ESPAREIniFile.GetInteger(szAreaSec, "PermitBigTeam", 1, &(pSetting->PermitBigTeam));
			ESPAREIniFile.GetInteger(szAreaSec, "NeedOwner", 0, &(pSetting->NeedOwner));

			int nCanGainSroce = TRUE;
			ESPAREIniFile.GetInteger(szAreaSec, "CanGainSroce", TRUE, &nCanGainSroce);
			pSetting->CanGainSroce = (nCanGainSroce == TRUE);
#ifdef _SERVER
			WorldBuffInfo& buffInfo = pSetting->BuffInfo;
			for (int buffIndex = 0; buffIndex < MAX_WORLD_BUFF_COUNT; buffIndex++)
			{
				sprintf(szAreaKey, "EnterBuff%d", buffIndex + 1);
				ESPAREIniFile.GetInteger(szAreaSec, szAreaKey, -1, &(buffInfo.EnterBuff[buffIndex]));
				sprintf(szAreaKey, "ExitBuff%d", buffIndex + 1);
				ESPAREIniFile.GetInteger(szAreaSec, szAreaKey, -1, &(buffInfo.ExitBuff[buffIndex]));
			}
#endif
		}
	}
	
	if( nType == -1 )
	{
		ESPAREIniFile.GetInteger( szAreaSec, "count", 0, &nCount );
		for( int nLoopCount = 0; nLoopCount < nCount; nLoopCount++ )
		{
			char szContent[CONTENTLEN];
			sprintf( szAreaKey, "esa%02d", nLoopCount );
			ESPAREIniFile.GetString( 
				szAreaSec, 
				szAreaKey, 
				"", 
				szContent, 
				CONTENTLEN );
			
			int nLUX = -1;
			int nLUY = -1;
			int nRBX = -1;
			int nRBY = -1;
			int nType= -1;
			
			sscanf( szContent, "%d,%d,%d,%d,%d", 
				&nLUX, &nLUY, &nRBX, &nRBY, &nType );
			AddEspecialArea( nLUX, nLUY, nRBX, nRBY, nType );
		}
	}
	
	ESPAREIniFile.Clear( );

	// Add by Cooler -->
	// 2006-12-28 14:48
#ifdef _SERVER
	m_clsTransportList.Init(szPathName);
#endif
	// End add by Cooler <--

   	char szDataFilePath[MAX_PATH] = {0};
    sprintf(szDataFilePath, "\\maps\\%s\\spawn.dat",szPathName);
    m_clsWorldSpawner.Load(m_nIndex, szDataFilePath);

#ifdef _SERVER
	m_State = world_state_idle;
#endif

	CFS_FILELOGS::WriteDebugLog("Load map='%s' OK!\n", szPathName);

	return TRUE;
}

//--> Rocker 2004/09/29 重新按照国战地图类型进行加载不同的地图数据
BOOL KSubWorld::ReLoadMap(int nCityType)
{
	KIniFile	IniFile;
	char		szPathName[FILE_NAME_LENGTH];
	char		szFileName[FILE_NAME_LENGTH];
	char		mapName[FILE_NAME_LENGTH];

	// 查找对应表并找到对应的地图名称
	if(!IniFile.Load("\\settings\\city\\city_type_name.ini"))
		return FALSE;
	
	sprintf(szPathName, "%d", nCityType);
	IniFile.GetString(szPathName, "name", "0", mapName, FILE_NAME_LENGTH);

	sprintf(szFileName, "maps/%s.wor", mapName);

	if (!IniFile.Load(szFileName))
		return FALSE;
	
	RECT	sRect;
	IniFile.GetRect("MAIN", "rect", &sRect);
	m_nRegionBeginX = sRect.left;
	m_nRegionBeginY = sRect.top;
	m_nWorldRegionWidth = sRect.right - sRect.left + 1;
	m_nWorldRegionHeight = sRect.bottom - sRect.top + 1;
	
	if (m_nTotalRegion < m_nWorldRegionWidth * m_nWorldRegionHeight)
	{
		m_nTotalRegion = m_nWorldRegionWidth * m_nWorldRegionHeight;
		
		if (m_nTotalRegion <= 0 || m_nWorldRegionWidth <= 0 || m_nWorldRegionHeight <= 0)
			return FALSE;
		
		delete [] m_Region;
		m_Region = NULL;
		
		m_Region = new KRegion[m_nTotalRegion];
	}else
	{
		m_nTotalRegion = m_nWorldRegionWidth * m_nWorldRegionHeight;
	}

	int		nX, nY, nIdx;
	
	// 加载地图障碍，并连接各Region
	for (nY = 0; nY < m_nWorldRegionHeight; nY++)
	{
		for (nX = 0; nX < m_nWorldRegionWidth; nX++)
		{
			nIdx = nY * m_nWorldRegionWidth + nX;
			if (m_Region[nIdx].Load(nX + m_nRegionBeginX, nY + m_nRegionBeginY))
			{
				m_Region[nIdx].Init(REGION_CELL_WIDTH, REGION_CELL_HEIGHT);
				m_Region[nIdx].m_nIndex = nIdx;
			}
			for (int i = 0; i < 8; i++)
			{
				short nTmpX, nTmpY;
				nTmpX = (short)LOWORD(m_Region[nIdx].m_nConRegionID[i]) - m_nRegionBeginX;
				nTmpY = (short)HIWORD(m_Region[nIdx].m_nConRegionID[i]) - m_nRegionBeginY;
				if (nTmpX < 0 || nTmpY < 0 || nTmpX >= m_nWorldRegionWidth || nTmpY >= m_nWorldRegionHeight)
				{
					m_Region[nIdx].m_nConnectRegion[i] = -1;
					continue;
				}
				int nConIdx = nTmpY * m_nWorldRegionWidth + nTmpX;
				m_Region[nIdx].m_nConnectRegion[i] = nConIdx;
			}
		}
	}
	
	// Add Begin
	char	szRepairFilePath[MAX_PATH] = { 0 };
	char	szPreCurPath[MAX_PATH] = { 0 };

	g_GetFilePath(szPreCurPath);
	sprintf(szRepairFilePath, "maps/%s", mapName);
	// Add End

	for (nY = 0; nY < m_nWorldRegionHeight; nY++)
	{
		for (nX = 0; nX < m_nWorldRegionWidth; nX++)
		{
			// Commented Begin
			// g_SetFilePath(szPath);
			// Commented End

			// Add Begin
			g_SetFilePath(szRepairFilePath);
			// Add End

			m_Region[nY * m_nWorldRegionWidth + nX].LoadObject(m_nIndex,nX + m_nRegionBeginX, nY + m_nRegionBeginY);

			// Add Begin
			g_SetFilePath(szPreCurPath);
			// Add End
		}
	}

	return TRUE;
}
//<-- End

//<-- End

#endif

#ifndef _SERVER
//--> Rocker 2004/10/08 增加根据城市类型加载不同地图的功能
//BOOL KSubWorld::LoadMap(int nId, int nRegion)
BOOL KSubWorld::LoadMap(int nId, int nRegion, int nCityType, bool bSyncWorld )
//<-- End
{
	static int	nXOff[8] = {0, -1, -1, -1, 0,  1, 1, 1};
	static int	nYOff[8] = {1, 1,  0,  -1, -1, -1, 0, 1};
	KIniFile	IniFile;

	if (!m_Region)
	{
		m_Region = new KRegion[MAX_REGION];
	}

	if (nId != m_SubWorldID)
	{
		SubWorld[0].Close();
		g_ScenePlace.ClosePlace();

		char	szKeyName[32], szPathName[FILE_NAME_LENGTH];

		IniFile.Load("settings/MapList.ini");

		sprintf(szKeyName, "%d", nId);
		IniFile.GetString("List", szKeyName, "", szPathName, sizeof(szPathName));
		sprintf(m_szMapPath, "\\maps\\%s", szPathName);
		RECT	sRect;
		KIniFile ini;
		char szBuf[COMMON_CLIENT_MSG_LEN_64];
		sprintf(szBuf, "%s%s", m_szMapPath, ".wor");
		ini.Load( szBuf );
		ini.GetRect("MAIN", "rect", &sRect);
		m_nRegionBeginX = sRect.left;
		m_nRegionBeginY = sRect.top;
		m_nWorldRegionWidth = sRect.right - sRect.left + 1;
		m_nWorldRegionHeight = sRect.bottom - sRect.top + 1;

		g_ScenePlace.OpenPlace(nId, 0/*nCityType*/);
		m_SubWorldID = nId;

		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex = -1;
	}
	int nX = LOWORD(nRegion);
	int nY = HIWORD(nRegion);

	int nIdx = FindRegion(nRegion);

	if (nIdx < 0)
	{
		nIdx = m_ClientRegionIdx[0];

		if (m_Region[nIdx].Load(nX, nY))
		{
			m_Region[nIdx].m_nIndex = nIdx;
			m_Region[nIdx].Init(REGION_CELL_WIDTH, REGION_CELL_HEIGHT);
			m_Region[nIdx].LoadObject(0, nX, nY, m_szMapPath);
		}
	}

	if ( bSyncWorld )
	{
		gbSwitchPaintAlphaType = false;
		g_ScenePlace.SetFocusPosition(m_Region[nIdx].m_nRegionX, m_Region[nIdx].m_nRegionY, 0, bSyncWorld );
	}//*/

	

	m_ClientRegionIdx[0] = nIdx;

	for (int i = 0; i < 8; i++)
	{
		int nConIdx;
		nConIdx = FindRegion(m_Region[nIdx].m_nConRegionID[i]);

		if (nConIdx < 0)
		{
			nConIdx = FindFreeRegion(nX, nY);
			_ASSERT(nConIdx >= 0);

			if (m_Region[nConIdx].Load(nX + nXOff[i], nY + nYOff[i]))
			{
				m_Region[nConIdx].m_nIndex = nConIdx;
				m_Region[nConIdx].Init(REGION_CELL_WIDTH, REGION_CELL_HEIGHT);
				m_Region[nConIdx].LoadObject(0, nX + nXOff[i], nY + nYOff[i], m_szMapPath);
			}
			else
			{
				m_Region[nConIdx].m_nIndex = -1;
				m_Region[nConIdx].m_RegionID = -1;
				nConIdx = -1;
			}
		}
		m_ClientRegionIdx[i + 1] = nConIdx;
		m_Region[nIdx].m_nConnectRegion[i] = nConIdx;
	}

	if (m_Region[nIdx].m_nConnectRegion[0] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[7] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[4] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[3] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[5] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[0]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[7];
	}

	if (m_Region[nIdx].m_nConnectRegion[1] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[7] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[5] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[1]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[0];
	}

	if (m_Region[nIdx].m_nConnectRegion[2] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[1];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[7] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[5] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[2]].m_nConnectRegion[6] = nIdx;
	}

	if (m_Region[nIdx].m_nConnectRegion[3] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[2] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[3]].m_nConnectRegion[7] = nIdx;
	}

	if (m_Region[nIdx].m_nConnectRegion[4] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[0] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[1] = m_Region[nIdx].m_nConnectRegion[2];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[3];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[6] = m_Region[nIdx].m_nConnectRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[4]].m_nConnectRegion[7] = m_Region[nIdx].m_nConnectRegion[6];
	}

	if (m_Region[nIdx].m_nConnectRegion[5] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[1] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[3] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[4] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[5]].m_nConnectRegion[7] = -1;
	}

	if (m_Region[nIdx].m_nConnectRegion[6] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[0] = m_Region[nIdx].m_nConnectRegion[7];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[1] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[2] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[3] = m_Region[nIdx].m_nConnectRegion[4];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[5];
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[6]].m_nConnectRegion[7] = -1;
	}
	
	if (m_Region[nIdx].m_nConnectRegion[7] >= 0)
	{
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[0] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[1] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[2] = m_Region[nIdx].m_nConnectRegion[0];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[3] = nIdx;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[4] = m_Region[nIdx].m_nConnectRegion[6];
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[5] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[6] = -1;
		m_Region[m_Region[nIdx].m_nConnectRegion[7]].m_nConnectRegion[7] = -1;
	}


	//Load Especial Area
#define KEYLEN	32
#define CONTENTLEN	256
	
	KIniFile	ESPAREIniFile;
	char		szAreaKey[KEYLEN];
	char		szAreaSec[KEYLEN];
	
	if (!ESPAREIniFile.Load("\\settings\\especialearea.ini"))
		return FALSE;
	
	int nType;
	int nCount;
	sprintf( szAreaSec, "%d", nId );
	ESPAREIniFile.GetInteger( szAreaSec, "alltype", -1, &nType );
	SetAllEspecialType( nType );
	
	int isWorldCombatInstance  = FALSE;
	ESPAREIniFile.GetInteger(szAreaSec,"IsCombatInstance",FALSE,&(isWorldCombatInstance));
	m_IsCombatInstance = (isWorldCombatInstance == TRUE);
	
	int nSyncType = especial_sync_type_normal;
	ESPAREIniFile.GetInteger(szAreaSec,"syncflag",especial_sync_type_normal,&(nSyncType));
	if (nSyncType < especial_sync_type_normal || nSyncType >= espacial_sync_type_num )
		nSyncType = especial_sync_type_normal ; 
	
	m_nSyncType = nSyncType;

	ESPAREIniFile.GetInteger(szAreaSec,"CombatScoreCalcType",PROGRAME_CALU, &(m_CombatScoreCalcType));


	m_nMapChatInterval         = DEFAULT_MAP_ROOM_CHAT_INTERVAL;
	int nNormalMapChatInterval = ConfigManager::Singleton().GetGlobalVariable(global_var_maproomchat_timeinterval);
	ESPAREIniFile.GetInteger(szAreaSec,"MapRoomChatInterval",nNormalMapChatInterval,&m_nMapChatInterval);
	if (m_nMapChatInterval < MIN_MAP_ROOM_CHAT_INTERVAL)
		m_nMapChatInterval = MIN_MAP_ROOM_CHAT_INTERVAL;

	for (int n = 0; n < MAX_COMBAT_ORG_NUM; n ++)
	{
		char szOrgKey[32] = "";
		sprintf(szOrgKey,"Org_%d_Name",n +1 );
		m_WorldCombatClientInfo[n].szOrgName[0] = 0;
		ESPAREIniFile.GetString(szAreaSec, szOrgKey, "", m_WorldCombatClientInfo[n].szOrgName, sizeof(m_WorldCombatClientInfo[n].szOrgName));
	}//end for n

	char szOrgKey[32];

	memset(szOrgKey, 0, sizeof(szOrgKey));
	sprintf(szOrgKey, "LocalChannelFullName");
	ESPAREIniFile.GetString(szAreaSec, szOrgKey, "", m_MapChannelInfo.szChannelFullName, sizeof(m_MapChannelInfo.szChannelFullName));

	memset(szOrgKey, 0, sizeof(szOrgKey));
	sprintf(szOrgKey, "LocalChannelName");
	ESPAREIniFile.GetString(szAreaSec, szOrgKey, "", m_MapChannelInfo.szChannelName, sizeof(m_MapChannelInfo.szChannelName));
	
	memset(szOrgKey, 0, sizeof(szOrgKey));
	sprintf(szOrgKey, "TextColor");
	ESPAREIniFile.GetString(szAreaSec, szOrgKey, "255,255,255", m_MapChannelInfo.szColor, sizeof(m_MapChannelInfo.szColor));
	
	memset(szOrgKey, 0, sizeof(szOrgKey));
	sprintf(szOrgKey, "TextFont");
	ESPAREIniFile.GetString(szAreaSec, szOrgKey, "", m_MapChannelInfo.szFont, sizeof(m_MapChannelInfo.szFont));

	if( nType == -1 )
	{
		ESPAREIniFile.GetInteger( szAreaSec, "count", 0, &nCount );
		for( int nLoopCount = 0; nLoopCount < nCount; nLoopCount++ )
		{
			char szContent[CONTENTLEN];
			sprintf( szAreaKey, "esa%02d", nLoopCount );
			ESPAREIniFile.GetString( 
				szAreaSec, 
				szAreaKey, 
				"", 
				szContent, 
				CONTENTLEN );
			
			int nLUX = -1;
			int nLUY = -1;
			int nRBX = -1;
			int nRBY = -1;
			int nType= -1;
			
			sscanf( szContent, "%d,%d,%d,%d,%d", 
				&nLUX, &nLUY, &nRBX, &nRBY, &nType );
			AddEspecialArea( nLUX, nLUY, nRBX, nRBY, nType );
		}
	}
	
	ESPAREIniFile.Clear( );

	return TRUE;
}
#endif

void KSubWorld::LoadTrap()
{
}


#ifdef _SERVER
void KSubWorld::NpcChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;

	Npc[nIdx].SetActiveFlag(TRUE);
	if (nSrcRnidx == -1)
	{
		_ASSERT(nDesRnIdx >= 0);
		if (nDesRnIdx >= 0)
		{
			m_Region[nDesRnIdx].AddNpc(nIdx);
			Npc[nIdx].m_RegionIndex = nDesRnIdx;
		}
		return;
	}
	else if (nSrcRnidx == VOID_REGION)
	{
		Npc[nIdx].m_Node.Remove();
		Npc[nIdx].m_Node.Release();
	}
	else if (nSrcRnidx >= 0)
	{
		SubWorld[Npc[nIdx].m_SubWorldIndex].m_Region[nSrcRnidx].RemoveNpc(nIdx);
		if (nDesRnIdx != VOID_REGION)	// 不是加入到死亡重生链表
			Npc[nIdx].m_RegionIndex = -1;
	}

	if (nDesRnIdx >= 0)
	{
		m_Region[nDesRnIdx].AddNpc(nIdx);
		Npc[nIdx].m_RegionIndex = nDesRnIdx;
	}
	else if (nDesRnIdx == VOID_REGION)
	{
		KSpawnPoint_Runtime_Info* pSpawnInfo = Npc[nIdx].GetSpawnInfo();
		if (pSpawnInfo != NULL)
		{
			pSpawnInfo->m_uCount--;
			NpcSet.Remove(nIdx);
		}
		else
		{
            WorldSpawner_RunTimeInfo* pWorldSpawnInfo = Npc[nIdx].GetWorldSpawnInfo();
            if (pWorldSpawnInfo != NULL)
            {
                pWorldSpawnInfo->uMobCount--;
                NpcSet.Remove(nIdx);

				s_RecycleCount++;
            }
            else
            {
				//通知客户端删除
				NPC_REMOVE_SYNC	NetCommand;				
				NetCommand.ProtocolType = (BYTE)s2c_npcremove;
				NetCommand.ID = Npc[nIdx].m_dwID;				
				int nSubWorld = Npc[nIdx].m_SubWorldIndex;
				int nRegion = Npc[nIdx].m_RegionIndex;				
				if (nSubWorld >= 0 && nSubWorld <= MAX_SUBWORLD && nRegion >= 0 && nRegion <= SubWorld[nSubWorld].m_nTotalRegion)
				{	
					int nMaxCount = MAX_BROADCAST_COUNT_OPTIMIZED;
					SubWorld[nSubWorld].BroadCastRegion(&NetCommand, sizeof(NetCommand), nMaxCount, nRegion, Npc[nIdx].GetMapX(), Npc[nIdx].GetMapY());
				}

                m_NoneRegionNpcList.AddTail(&Npc[nIdx].m_Node);
                Npc[nIdx].m_Node.AddRef();
                Npc[nIdx].m_RegionIndex = nDesRnIdx;
            }
		}
	}
}
#endif

#ifndef _SERVER
void KSubWorld::NpcChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	int		nSrc, nDest;

	if (nSrcRnidx == -1)
	{
		nDest = SubWorld[0].FindRegion(nDesRnIdx);
		if (nDest < 0)
			return;
		m_Region[nDest].AddNpc(nIdx);
		Npc[nIdx].m_dwRegionID = m_Region[nDest].m_RegionID;
		Npc[nIdx].m_RegionIndex = nDest;
		return;
	}

	nSrc = SubWorld[0].FindRegion(nSrcRnidx);
	if (nSrc >= 0)
		SubWorld[0].m_Region[nSrc].RemoveNpc(nIdx);

	nDest = SubWorld[0].FindRegion(nDesRnIdx);

	KIndexNode *pNode = &Npc[nIdx].m_Node;
	if (nDest >= 0)
	{
		m_Region[nDest].AddNpc(nIdx);
		
		if (Player[CLIENT_PLAYER_INDEX].m_nIndex == nIdx)
			LoadMap(m_SubWorldID, m_Region[nDest].m_RegionID, 0, false);
		Npc[nIdx].m_dwRegionID = m_Region[nDest].m_RegionID;
		Npc[nIdx].m_RegionIndex = nDest;
	}
	else if (Player[CLIENT_PLAYER_INDEX].m_nIndex == nIdx)
	{
		LoadMap(m_SubWorldID, nDesRnIdx, 0, false);
		nDest = SubWorld[0].FindRegion(nDesRnIdx);
		if (nDest >= 0)
		{
			m_Region[nDest].AddNpc(nIdx);
			Npc[nIdx].m_dwRegionID = m_Region[nDest].m_RegionID;
			Npc[nIdx].m_RegionIndex = nDest;
		}
	}
}

void KSubWorld::RefreshWorldMapPlayerInfoCache(const int nNum, const WORLD_PLAYER_INFO * pInfos)
{
	if (pInfos && nNum <= MAX_GLOBAL_SYNC_PLAYER_NUM)
	{
		m_PlayerInfoCache.m_Num = nNum;
		memcpy(m_PlayerInfoCache.m_PlayerInfos,pInfos,nNum * sizeof(WORLD_PLAYER_INFO));
	}//endif
}

void KSubWorld::ClearWorldMapPlayerInfoCache()
{
	m_PlayerInfoCache.m_Num = 0;
	ZeroMemory(m_PlayerInfoCache.m_PlayerInfos,sizeof(m_PlayerInfoCache.m_PlayerInfos));
}

void KSubWorld::RefreshWarCommanderInfoCache(const int nNum, WAR_COMMANDER_INFO* pInfos)
{
	if ( nNum != 0 && nNum <= MAX_WAR_COMMANDER_NUM )
	{
		if (pInfos )
		{
			m_WarCommanderInfoCache.m_Num = nNum;
			memcpy(m_WarCommanderInfoCache.m_PlayerInfos, pInfos, nNum * sizeof(WAR_COMMANDER_INFO));
		}//endif
	}else if ( nNum == 0)
	{
		ClearWarCommanderInfoCache();
	}
}

void KSubWorld::ClearWarCommanderInfoCache()
{
	m_WarCommanderInfoCache.m_Num = 0;
	ZeroMemory(m_WarCommanderInfoCache.m_PlayerInfos, sizeof(m_WarCommanderInfoCache.m_PlayerInfos));
}
#endif

void KSubWorld::MissleChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
//	if (nIdx <= 0 || nIdx >= MAX_MISSLE)
//		return;
//
//	if (nSrcRnidx == -1)
//	{
//		m_Region[nDesRnIdx].AddMissle(nIdx);
//		return;
//	}
//
//	SubWorld[Missle[nIdx].m_nSubWorldId].m_Region[nSrcRnidx].RemoveMissle(nIdx);
//
//	if (nDesRnIdx == -1)
//	{
//		MissleSet.Remove(nIdx);
//		return;
//	}
//	
//	m_Region[nDesRnIdx].AddMissle(nIdx);
}

#ifdef _SERVER
void KSubWorld::PlayerChangeRegion(int nSrcRnidx, int nDesRnIdx, int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return;

	if (nSrcRnidx < 0)
		return;

	RemovePlayer(nSrcRnidx, nIdx);
	
	if (nDesRnIdx == -1)
	{
		//PlayerSet.PrepareRemove(Player[nIdx].m_nNetConnectIdx);
		if (g_pServer != NULL)
			g_pServer->ShutdownClient(Player[nIdx].m_nNetConnectIdx);
		return;
	}

	if (nDesRnIdx >= 0)
	{
		AddPlayer(nDesRnIdx, nIdx);
	}
/* 因为RemovePlayer和AddPlayer已经做了从链表中清除节点，修改周边九个REGION的引用计数的操作了
	KIndexNode* pNode = &Player[nIdx].m_Node;
	if (pNode->m_Ref > 0)
	{
		pNode->Remove();
		pNode->Release();
	}
	m_Region[nSrcRnidx].m_nActive--;
	if (m_Region[nSrcRnidx].m_nActive < 0)
		m_Region[nSrcRnidx].m_nActive = 0;
	
	for (int i = 0; i < 8; i++)
	{
		if (m_Region[nSrcRnidx].m_nConnectRegion[i] < 0)
			continue;
		m_Region[m_Region[nSrcRnidx].m_nConnectRegion[i]].m_nActive--;
		if (m_Region[m_Region[nSrcRnidx].m_nConnectRegion[i]].m_nActive < 0)
			m_Region[m_Region[nSrcRnidx].m_nConnectRegion[i]].m_nActive = 0;
	}

	if (pNode->m_Ref == 0)
	{
		m_Region[nDesRnIdx].m_PlayerList.AddTail(pNode);
		pNode->AddRef();
	}

	m_Region[nDesRnIdx].m_nActive++;
	for (i = 0; i < 8; i++)
	{
		if (m_Region[nDesRnIdx].m_nConnectRegion[i] < 0)
			continue;
		m_Region[m_Region[nDesRnIdx].m_nConnectRegion[i]].m_nActive++;
		if (m_Region[m_Region[nDesRnIdx].m_nConnectRegion[i]].m_nActive < 0)
			m_Region[m_Region[nDesRnIdx].m_nConnectRegion[i]].m_nActive = 0;
	}
*/
}
#endif

void KSubWorld::GetMps(int *nX, int *nY, int nSpeed, int nDir, int nMaxDir /* = 64 */)
{
	*nX += (g_DirCos(nDir, nMaxDir) * nSpeed) >> 10;
	*nY += (g_DirSin(nDir, nMaxDir) * nSpeed) >> 10;
}
#ifdef _SERVER
BOOL KSubWorld::SendSyncData(int nIdx, int nClient)
{
	WORLD_SYNC	WorldSync;
	WorldSync.ProtocolType = (BYTE)s2c_syncworld;
	WorldSync.Region = m_Region[Npc[nIdx].m_RegionIndex].m_RegionID;
	WorldSync.Frame = m_dwCurrentTime;
	WorldSync.SubWorld = (WORD)m_SubWorldID;
	DWORD expireLeftTime = (m_ExpireTime > UNIX_TMIE_STAMP) ? (m_ExpireTime - UNIX_TMIE_STAMP) : 0;
	WorldSync.ExpireLeftTime = expireLeftTime;

	if (g_pServer != NULL && SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&WorldSync, sizeof(WORLD_SYNC))))
	{
		return TRUE;
	}
	else
	{
		printf("player Packing world sync data failed...\n");
		return FALSE;
	}
	
	m_Region[Npc[nIdx].m_RegionIndex].SendSyncData( nClient, nIdx );
	
	return TRUE;
}
#endif

#ifndef _SERVER
void KSubWorld::LoadCell()
{
}
#endif

#ifndef _SERVER

void KSubWorld::Mps2Screen(int *Rx, int *Ry)
{
}

void KSubWorld::Screen2Mps(int *Rx, int *Ry)
{
}
#endif

#ifdef _SERVER
int KSubWorld::GetRegionIndex(int nRegionID)
{
	int nRet;
	short nX = (short)LOWORD(nRegionID) - m_nRegionBeginX;
	short nY = (short)HIWORD(nRegionID) - m_nRegionBeginY;

	if (nX < 0 || nY < 0 || nX >= m_nWorldRegionWidth || nY >= m_nWorldRegionHeight)
		return -1;

	nRet = nY * m_nWorldRegionWidth + nX;
	return nRet;
}
#endif

void KSubWorld::Close(bool notifyScript)
{
	if (m_SubWorldID == INVALID_WORLD_ID)
		return;

#ifdef _SERVER
	KickAllPlayer();

	//通知脚本
	if (notifyScript)
		ExecuteScript("\\script\\main.lua", "WorldClose", m_InstanceID, m_nIndex);
#endif

	if (m_Region)
	{
#ifdef _SERVER
		for (int i = 0; i < m_nTotalRegion; i++)
		{
			m_Region[i].Release();
		}
		delete[] m_Region;
		m_Region = NULL;
#else
		for (int i = 0; i < m_nTotalRegion; i++)
		{
			m_Region[i].Close();
		}
		ClearWarCommanderInfoCache();
#endif
	}
	
	m_nIndex = INVALID_WORLD_INDEX;
	m_SubWorldID = INVALID_WORLD_ID;
	m_InstanceID = INVALID_INSTANCE_ID;
	m_IsCombatInstance = false;
	m_nSyncType = especial_sync_type_normal ;
	m_nMapChatInterval = DEFAULT_MAP_ROOM_CHAT_INTERVAL;
	m_CombatScoreCalcType = PROGRAME_CALU;
	m_ExpireTime = 0;
	m_dwCurrentTime = 0;

#ifdef _SERVER
	m_nSyncIntervalSec = MIN_SYNC_INTERVAL_SEC;
	m_nWorldRegionWidth = 0;
	m_nWorldRegionHeight = 0;
	m_nTotalRegion = 0;
	m_nRegionBeginX = 0;
	m_nRegionBeginY = 0;
	m_OwnerPlayerIndex = INVALID_PLAYER_INDEX;
	memset(&m_OwnerSocialGUID, 0, sizeof(m_OwnerSocialGUID));
#endif
	for(int especialAreaIndex = 0; especialAreaIndex < MAX_ESAREA; especialAreaIndex++)
	{
		m_nEspecialArea[especialAreaIndex].nType = especial_area_none;
	}
	m_nAllType = especial_area_none;	

#ifdef _SERVER
	KIndexNode* pNode = NULL;
	KIndexNode* pTempNode = NULL;
	pNode = (KIndexNode *)m_NoneRegionNpcList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode*)pNode->GetNext();
		int npcIndex = pTempNode->m_nIndex;
		if (IsValidNpc(npcIndex))
		{
			Npc[npcIndex].m_RegionIndex = -1;
			Npc[npcIndex].m_Node.Remove();
			Npc[npcIndex].m_Node.Release();
			NpcSet.Remove(npcIndex);
		}
	}
#endif
		
#ifdef _SERVER
	m_nPlayerCount = 0;
	m_LastPlayerExitTime = 0;
	m_nIndexInUniverse = 0;
	m_State = world_state_init;
	m_clsWorldSpawner.Release();
	m_clsTransportList.Release();

	for (int teamIndex = 0; teamIndex < MAX_WORLD_TEAM_COUNT; teamIndex++)
	{
		int teamId = m_WorldTeam[teamIndex];
		if (teamId != INVALID_TEAM_ID)
			g_TeamSet.RemoveTeam(teamId);
	}

	m_SyncToWorldNpcs.clear();
	m_SyncListIndex = 0;
	m_NextSyncCounter = 0;
	
	for (int nCombatOrg = 0 ; nCombatOrg < MAX_COMBAT_ORG_NUM; nCombatOrg ++ )
	{
		m_WorldCombatInstanceInfo.org[nCombatOrg].nPersonNum = 0;
		m_WorldCombatInstanceInfo.org[nCombatOrg].nScore     = 0;
	}//end for CombatOrg

	for (int nFlag = 0; nFlag < MAX_MAP_FLAGS ; nFlag ++ )
	{
		m_MapFlag[nFlag] = 0;
	}//end for nFlag

#endif
}

#ifdef _SERVER
void KSubWorld::LoadObject(char* szPath, char* szFileName)
{
	if (!szFileName || !szFileName[0])
		return;

	KIniFile IniFile;
	// 加载世界中的动态物件（Npc、Object、Trap）

	//	特别注意：
	//	如果在今后的维护过程中，在后面的代码中增加了出口，请在出口前调用
	//  g_SetFilePath(szPreCurPath) 以把当前路径设回去
	//  chenshanglin 2005-11-17

	// add by chenshanglin on 2005-11-17 
	char	szPreCurPath[MAX_PATH] = { 0 };

	g_GetFilePath(szPreCurPath);
	// add end

	g_SetFilePath(szPath);
	
	int nObjNumber = 0;
	if (IniFile.Load(szFileName))
	{
		IniFile.GetInteger("MAIN", "elementnum", 0, &nObjNumber);
	}

	int nLength = strlen(szFileName);
	szFileName[nLength - 4] = 0;
	strcat(szPath, "\\");
	strcat(szPath, szFileName);

	char	szSection[32];
	char	szDataFile[32];

	for (int i = 0; i < nObjNumber; i++)
	{
		float	fPos[3];
		int		nPos[3];

		g_SetFilePath(szPath);
		sprintf(szSection, "%d", i);
		IniFile.GetFloat3(szSection, "groundoffset", fPos);
		nPos[0] = (int)(fPos[0] * 32);
		nPos[1] = (int)(fPos[1] * 32);
		nPos[2] = (int)fPos[2];

		szDataFile[0] = 0;
		IniFile.GetString(szSection, "event", "", szDataFile, sizeof(szDataFile));
		strcat(szDataFile, "_data.ini");
		
		KIniFile IniChange;
		int nType = 0, nLevel = 0;
		char	szName[32];
		
		if (!IniChange.Load(szDataFile))
			continue;
		
		IniChange.GetInteger("MAIN", "Type", 0, &nType);
		IniChange.GetInteger("MAIN", "Level", 4, &nLevel);
		IniChange.GetString("MAIN", "mapedit_templatesection", "", szName, sizeof(szName));
		
		switch(nType)
		{
		case kind_normal:		// 普通战斗npc
			{
				int nFindNo = g_NpcSetting.FindRow(szName);
				if (nFindNo == -1)
					continue;
				else
					nFindNo = nFindNo - 2;
				
				int nSettingInfo = MAKELONG(nLevel, nFindNo);
				NpcSet.Add(nSettingInfo, m_nIndex, nPos[0], nPos[1]);
			}
			break;
		case kind_player:
			break;
		case kind_partner:
			break;
		case kind_dialoger:		// 普通非战斗npc
			break;
		case kind_bird:			// 客户端only
			break;
		case kind_mouse:		// 客户端only
			break;
		default:
			break;
		}
	}

	g_SetFilePath(szPreCurPath);
}
#endif

void KSubWorld::AddPlayer(int nRegion, int nIdx)
{
	if (nRegion < 0 || nRegion >= m_nTotalRegion)
	{
		return;
	}

	if (m_Region[nRegion].AddPlayer(nIdx))
	{
#ifdef _SERVER
		++m_nPlayerCount;	// 统计人数
		m_State = world_state_active;
#endif
		m_Region[nRegion].m_nActive++;

		for (int i = 0; i < 8; i++)
		{
			int nConRegion = m_Region[nRegion].m_nConnectRegion[i];
			if (nConRegion == -1)
				continue;
			
			m_Region[nConRegion].m_nActive++;
		}
	}
}

void KSubWorld::RemovePlayer(int nRegion, int nIdx)
{
	if (nRegion < 0 || nRegion >= m_nTotalRegion)
		return;

	if (m_Region[nRegion].RemovePlayer(nIdx))
	{	
#ifdef _SERVER
		--m_nPlayerCount; // 统计人数

		//最后一个玩家离开，记录时间戳
		if (m_nPlayerCount == 0)
		{
			m_LastPlayerExitTime = m_dwCurrentTime;
			m_State = world_state_idle;
		}
#endif
			
		m_Region[nRegion].m_nActive--;		

		if (m_Region[nRegion].m_nActive < 0)
		{
			_ASSERT(0);
			m_Region[nRegion].m_nActive = 0;
		}
		
		for (int i = 0; i < 8; i++)
		{
			int nConRegion = m_Region[nRegion].m_nConnectRegion[i];
			if (nConRegion == -1)
				continue;
			
			m_Region[nConRegion].m_nActive--;

			if (m_Region[nConRegion].m_nActive < 0)
			{
				_ASSERT(0);
				m_Region[nConRegion].m_nActive = 0;
			}
		}
	}
}

void KSubWorld::GetFreeObjPos(POINT& pos)
{
	POINT	posLocal = pos;
	POINT	posTemp;
	int nLayer = 1;

//	if (CanPutObj(posLocal))
//		return;

	while(1)
	{
		for (int i = 0; i <= nLayer; i++)
		{
			posTemp.y = posLocal.y + i * 32;
			posTemp.x = posLocal.x + (nLayer - i) * 32;
			if (CanPutObj(posTemp))
			{
				pos = posTemp;
				return;
			}
			posTemp.y = posLocal.y + i * 32;
			posTemp.x = posLocal.x - (nLayer - i) * 32;
			if (CanPutObj(posTemp))
			{
				pos = posTemp;
				return;
			}
			posTemp.y = posLocal.y - i * 32;
			posTemp.x = posLocal.x + (nLayer - i) * 32;
			if (CanPutObj(posTemp))
			{
				pos = posTemp;
				return;
			}
			posTemp.y = posLocal.y - i * 32;
			posTemp.x = posLocal.x - (nLayer - i) * 32;
			if (CanPutObj(posTemp))
			{
				pos = posTemp;
				return;
			}
		}
		nLayer++;
		if (nLayer >= 10)
			break;
	}
	return;
}

BOOL KSubWorld::CanPutObj(POINT pos)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(pos.x, pos.y, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	if (nRegion >= 0 
		&& !m_Region[nRegion].GetBarrier(nMapX, nMapY, nOffX, nOffY) 
		&& !m_Region[nRegion].GetObj(nMapX, nMapY)) // lixuewu
		return TRUE;
	return FALSE;
}

#ifdef _SERVER
void KSubWorld::BroadCast(const char* pBuffer, size_t uSize)
{
	int nIdx = PlayerSet.GetFirstPlayer();
	while(nIdx)
	{
		if (Player[nIdx].m_nNetConnectIdx >= 0 
			&& Player[nIdx].m_nIndex > 0 
			&& Npc[Player[nIdx].m_nIndex].m_SubWorldIndex == m_nIndex)
		{
			if (g_pServer != NULL)
				g_pServer->PackDataToClient(Player[nIdx].m_nNetConnectIdx, pBuffer, uSize);
		}
		nIdx = PlayerSet.GetNextPlayer();
	}
}

int	KSubWorld::RevivalAllNpc()
{
	KIndexNode * pNode		= NULL;
	size_t	ulCount = 0;
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		KRegion * pCurRegion = &m_Region[i];
		pNode = (KIndexNode *)pCurRegion->m_NpcList.GetHead();
		
		while(pNode)
		{
			int nNpcIdx = pNode->m_nIndex;
			if (!Npc[nNpcIdx].IsPlayer())
			{
				Npc[nNpcIdx].ExecuteRevive();
			}
			pNode = (KIndexNode *)pNode->GetNext();	
		}
	}
	
	pNode = (KIndexNode*)m_NoneRegionNpcList.GetHead();
	
	while(pNode)
	{
		int nNpcIdx = pNode->m_nIndex;
		if (!Npc[nNpcIdx].IsPlayer())
		{
			Npc[nNpcIdx].m_Frames.nTotalFrame = 1;
			Npc[nNpcIdx].m_Frames.nCurrentFrame = 0;
			ulCount ++;
		}
		pNode = (KIndexNode *)pNode->GetNext();	
	}
	
	return ulCount;
}

int KSubWorld::FindNpcFromName(const char * szName)
{
	if (!szName || !szName[0])
		return 0;
	
	KIndexNode * pNode		= NULL;

	int nResult = 0;
	
	for (int i = 0; i < m_nTotalRegion; i++)
	{
		KRegion * pCurRegion = &m_Region[i];
		pNode = (KIndexNode *)pCurRegion->m_NpcList.GetHead();
		
		while(pNode)
		{
			int nNpcIdx = pNode->m_nIndex;
			if (!strcmp(Npc[nNpcIdx].Name, szName))
			{
				nResult = nNpcIdx;
				return nResult;
			}
			pNode = (KIndexNode *)pNode->GetNext();	
		}
	}	
	
	pNode = (KIndexNode*)m_NoneRegionNpcList.GetHead();
	
	while(pNode)
	{
		int nNpcIdx = pNode->m_nIndex;
		if (!strcmp(Npc[nNpcIdx].Name, szName))
		{
			return nNpcIdx;
		}
		pNode = (KIndexNode *)pNode->GetNext();	
	}
	
	return nResult;
}
//--> nRange表示从找到的Region的某个格子为中心, 周边的nRange圈需要
// 填满Trap 的脚本ID数据
void KSubWorld::SetTrap(DWORD dwTrapId, int nMpsX, int nMpsY, int nRange)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
		return;
	
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_CELL_WIDTH || nMapY >= REGION_CELL_HEIGHT)
	{
		return;
	}

	m_Region[nRegion].SetTrap(dwTrapId, nMapX, nMapY);
	// 暂时支持范围1
	nRange = 1;
	for (int i=1; i<=nRange; i++)
	{
		if (nMapX - i >= 0)
			m_Region[nRegion].SetTrap(dwTrapId, nMapX - i, nMapY);
		else
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_LEFT]].SetTrap(dwTrapId, REGION_CELL_WIDTH - i, nMapY);
		
		if (nMapX + i < REGION_CELL_WIDTH)
			m_Region[nRegion].SetTrap(dwTrapId, nMapX + i, nMapY);
		else
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_RIGHT]].SetTrap(dwTrapId, i-1 , nMapY);
		
		if (nMapY - i >= 0)
			m_Region[nRegion].SetTrap(dwTrapId, nMapX, nMapY - i);
		else
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_UP]].SetTrap(dwTrapId, nMapX, REGION_CELL_HEIGHT - i);
		
		if (nMapY + i < REGION_CELL_HEIGHT)
			m_Region[nRegion].SetTrap(dwTrapId, nMapX, nMapY + i);
		else
			m_Region[m_Region[nRegion].m_nConnectRegion[DIR_DOWN]].SetTrap(dwTrapId, nMapX, i-1);
	}
}	
//<-- End

#endif

int KSubWorld::AddEspecialArea( int nLUX, int nLUY, int nRBX, int nRBY, int nType )
{
	for( int nLoopCount = 0; nLoopCount < MAX_ESAREA; nLoopCount++ )
	{
		if( m_nEspecialArea[nLoopCount].nType == especial_area_none )
		{
			m_nEspecialArea[nLoopCount].nLUX	=	nLUX;
			m_nEspecialArea[nLoopCount].nLUY	=	nLUY;
			m_nEspecialArea[nLoopCount].nRBX	=	nRBX;
			m_nEspecialArea[nLoopCount].nRBY	=	nRBY;
			m_nEspecialArea[nLoopCount].nType	=	nType;
			break;
		}
	}

	return TRUE;
}

bool KSubWorld::IsWorldCombatMap()const
{
#ifdef _SERVER
	WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(GetWorldTemplateId());
	return (m_IsCombatInstance && pSetting && pSetting->IsInstance);
#else
	return m_IsCombatInstance;
#endif
}

int KSubWorld::GetEspecialAreaType( int nX, int nY )
{
	if( m_nAllType != especial_area_none )
		return m_nAllType;
	
	for( int nLoopCount = 0; nLoopCount < MAX_ESAREA; nLoopCount++ )
	{
		if( m_nEspecialArea[nLoopCount].nType != especial_area_none )
		{

			if( nX >= m_nEspecialArea[nLoopCount].nLUX * 32&&
				nY >= m_nEspecialArea[nLoopCount].nLUY * 32&&
				nX <= m_nEspecialArea[nLoopCount].nRBX * 32&&
				nY <= m_nEspecialArea[nLoopCount].nRBY * 32 )
				return m_nEspecialArea[nLoopCount].nType;
		}
	}

	return especial_area_none;
}

#ifdef _SERVER
BOOL KSubWorld::GetRandomTransPos(int &nTransX, int &nTransY, int group)
{
/*	int mapLeft = m_nRegionBeginX * REGION_PIXEL_WIDTH;
	int mapTop = m_nRegionBeginY * REGION_PIXEL_HEIGHT;	
	int mapWidth = m_nWorldRegionWidth * REGION_PIXEL_WIDTH;
	int mapHeight = m_nWorldRegionHeight * REGION_PIXEL_HEIGHT;
	
	for (int retryCount = 0; retryCount < RANDOM_TRANS_RETRY_TIMES; retryCount++)
	{		
		int posX = mapLeft + g_Random(mapWidth);
		int posY = mapTop + g_Random(mapHeight);

		if (TestBarrier(posX, posY) == 0)
		{
			nTransX = posX;
			nTransY = posY;
			return TRUE;
		}
	}

	return FALSE;*/
	if(m_clsTransportList.GetRandomTransPos(nTransX, nTransY, group))
	{
		nTransX += m_nRegionBeginX * REGION_PIXEL_WIDTH;
		nTransY += m_nRegionBeginY * REGION_PIXEL_HEIGHT;

		return TRUE;
	}

	return FALSE;
}
#endif


#ifdef _SERVER
void SpawnRecoder::Active(unsigned int uTimer)
{
    if (RunTimeInfo.uMobCount < CommonInfo.nMinCount
        && (uTimer - RunTimeInfo.uLastSpawnTime) > CommonInfo.nTimeInterval)
    {
        RunTimeInfo.uLastSpawnTime = uTimer;
        int nCount = (CommonInfo.nMaxCount - RunTimeInfo.uMobCount);
        for (int i = 0; i < nCount; i++)
        {
            int nLevel = CommonInfo.nMinLevel + g_Random(CommonInfo.nMaxLevel);
            const wRect& aRect = m_pSpawnRects->RandomGet();
            int nMapX = (g_Random(aRect.Right - aRect.Left) + aRect.Left) * 16;
            int nMapY = (g_Random(aRect.Bottom - aRect.Top) + aRect.Top) * 32;
            nMapX += SubWorld[m_uSubWorld].m_nRegionBeginX  * REGION_PIXEL_WIDTH;
            nMapY += SubWorld[m_uSubWorld].m_nRegionBeginY * REGION_PIXEL_HEIGHT;
            int nNpc = NpcSet.Add(MAKELONG(nLevel, CommonInfo.nNpcTemplateID), m_uSubWorld, nMapX, nMapY);
            if (nNpc > 0)
            {
                RunTimeInfo.uMobCount++;
                Npc[nNpc].m_pWorldSpawnInfo = &RunTimeInfo;

				s_SpawnCount++;
            }
            else
            {
				//记录日志：刷怪失败
				if (g_pLogSystem)
				{
					char spawnInfo[256] = { 0 };
					snprintf(spawnInfo, sizeof(spawnInfo), "SpawnRecoder::Active World=%u,NpcId=%d,Level=%d,PosX=%u,PosY=%u",
						m_uSubWorld,
						CommonInfo.nNpcTemplateID,
						nLevel,		
						nMapX,
						nMapY);
					spawnInfo[sizeof(spawnInfo) - 1] = 0;
					g_pLogSystem->SysDbgLog(spawnInfo, strlen(spawnInfo), sys_dbg_log_event_spawn_failure);
				}
				
				KSubWorldSet::s_bNeedBlanceSpawn = TRUE;
                return;
            }
        }
        
    }
}
#endif

#ifdef _SERVER
bool KSubWorld::CopyInstance(int worldIndex) const
{
	if (worldIndex < 0 || worldIndex >= MAX_SUBWORLD)
		return false;

	KSubWorld& world = SubWorld[worldIndex];
	world.m_nIndex = worldIndex;
	world.m_SubWorldID = m_SubWorldID;
	world.m_nPlayerCount = 0;
	world.m_LastPlayerExitTime = 0;
	memset(&(world.m_OwnerSocialGUID), 0, sizeof(world.m_OwnerSocialGUID));
	world.m_dwCurrentTime = 0;
	world.m_nWorldRegionWidth = m_nWorldRegionWidth;
	world.m_nWorldRegionHeight = m_nWorldRegionHeight;
	world.m_nTotalRegion = m_nTotalRegion;
	world.m_nRegionBeginX = m_nRegionBeginX;
	world.m_nRegionBeginY = m_nRegionBeginY;
	memcpy(world.m_nEspecialArea, m_nEspecialArea, sizeof(m_nEspecialArea));
	world.m_nAllType         = m_nAllType;
	world.m_IsCombatInstance = m_IsCombatInstance;
	world.m_nSyncType        = m_nSyncType;
	world.m_nMapChatInterval = m_nMapChatInterval;
	world.m_nGroup           = m_nGroup;
	world.m_nSyncIntervalSec = m_nSyncIntervalSec;
	world.m_CombatScoreCalcType = m_CombatScoreCalcType;
	
	if (world.m_nTotalRegion > 0)
	{
		world.m_Region = new KRegion[world.m_nTotalRegion];
		if (world.m_Region == NULL)
			return false;
		for (int i = 0; i < world.m_nTotalRegion; i++)
		{
			if (!m_Region[i].CopyInstance(&(world.m_Region[i]), worldIndex))
				return false;
		}
	}

	if (!m_clsWorldSpawner.CopyInstance(world.m_clsWorldSpawner, worldIndex))
		return false;	
	if (!m_clsTransportList.CopyInstance(world.m_clsTransportList))
		return false;

	memset(world.m_CustomVariable, 0, sizeof(world.m_CustomVariable));
	memset(world.m_CustomString, 0, sizeof(world.m_CustomString));
	for (int teamIndex = 0; teamIndex < MAX_WORLD_TEAM_COUNT; teamIndex++)
	{
		world.m_WorldTeam[teamIndex] = INVALID_TEAM_ID;
	}

    for (int nCombatInfo = 0; nCombatInfo < MAX_COMBAT_ORG_NUM; nCombatInfo ++)
	{
		world.m_WorldCombatInstanceInfo.org[nCombatInfo].nPersonNum = 0;
		world.m_WorldCombatInstanceInfo.org[nCombatInfo].nScore     = 0;
	}//end for nCombatInfo

	for (int nFlag = 0; nFlag < MAX_MAP_FLAGS ; nFlag ++ )
	{
		world.m_MapFlag[nFlag] = m_MapFlag[nFlag];
	}//end for nFlag

	world.m_OwnerPlayerIndex = INVALID_PLAYER_INDEX;
	world.m_State = world_state_idle;

	return true;
}
#endif

#ifdef _SERVER
bool RectList::CopyInstance(RectList& rectlist) const
{
	rectlist.uCount = uCount;
	if (uCount > 0)
	{
		rectlist.pRects = new wRect[uCount];
		if (rectlist.pRects == NULL)
			return false;
		if (pRects == NULL)
			return false;
		memcpy((void*)(rectlist.pRects), (void*)pRects, sizeof(wRect) * uCount);
	}
	
	return true;
}
#endif

#ifdef _SERVER
bool SpawnRecoder::CopyInstance(SpawnRecoder& record, int worldIndex) const
{
	memcpy(&(record.CommonInfo), &CommonInfo, sizeof(CommonInfo));
	memset(&(record.RunTimeInfo), 0, sizeof(record.RunTimeInfo));
	record.m_uSubWorld = worldIndex;
	if (m_pSpawnRects)
	{
		record.m_pSpawnRects = new RectList;
		if (record.m_pSpawnRects == NULL)
			return false;
		if (!m_pSpawnRects->CopyInstance(*(record.m_pSpawnRects)))
			return false;
	}

	return true;
}
#endif

#ifdef _SERVER
bool KWorldSpawner::CopyInstance(KWorldSpawner& worldSpawner, int worldIndex) const
{
	worldSpawner.m_uSpawnInfoCount = m_uSpawnInfoCount;
	if (m_uSpawnInfoCount > 0)
	{
		worldSpawner.m_pSpawnInfo = new SpawnRecoder[m_uSpawnInfoCount];
		if (worldSpawner.m_pSpawnInfo == NULL)
			return false;
		for (int i = 0; i < m_uSpawnInfoCount; i++)
		{
			if (!m_pSpawnInfo[i].CopyInstance(worldSpawner.m_pSpawnInfo[i], worldIndex))
				return false;
		}
	}

	return true;
}
#endif

#ifdef _SERVER
bool KSubWorld::PrepareForReuse()
{
	if (m_State == world_state_idle)
	{
		//清理，为重用做准备
		m_InstanceID = INVALID_INSTANCE_ID;
		m_ExpireTime = 0;
		m_OwnerPlayerIndex = INVALID_PLAYER_INDEX;
		memset(&m_OwnerSocialGUID, 0, sizeof(m_OwnerSocialGUID));

		for (int nCombatOrg = 0 ; nCombatOrg < MAX_COMBAT_ORG_NUM; nCombatOrg ++ )
		{
			m_WorldCombatInstanceInfo.org[nCombatOrg].nPersonNum = 0;
			m_WorldCombatInstanceInfo.org[nCombatOrg].nScore     = 0;
		}//end for CombatOrg

		m_State = world_state_ready_for_reuse;
		return true;
	}

	return false;
}

void KSubWorld::AddCombatInstanceOrgPerson(const int nOrgId,const int nNumberAdded)
{
	if (IsValidCombatID(nOrgId) && nNumberAdded > 0)
	{
		int nIndex        = nOrgId - 1;
		int nCurPersonNum = m_WorldCombatInstanceInfo.org[nIndex].nPersonNum;
		int nNewPersonNum = nCurPersonNum + nNumberAdded;
		
		if (nCurPersonNum >= MAX_COMBAT_PERSON_NUM )
			return ;

		if ( nNewPersonNum < 0 || nNewPersonNum < nCurPersonNum || nNewPersonNum > MAX_COMBAT_PERSON_NUM )
			nNewPersonNum  = MAX_COMBAT_PERSON_NUM;

		m_WorldCombatInstanceInfo.org[nIndex].nPersonNum = nNewPersonNum;
	}//endif
}

void KSubWorld::AddCombatInstanceOrgScore(const int nOrgId,const int nScoreAdded)
{
	if (IsValidCombatID(nOrgId) && nScoreAdded > 0)
	{
		int nIndex        = nOrgId -1;
		int nCurScore     = m_WorldCombatInstanceInfo.org[nIndex].nScore;
		int nNewScore     = nCurScore + nScoreAdded;
		
		if (nCurScore >= MAX_COMBAT_SCORE_NUM)
			return ; 

		if ( nNewScore < 0 || nNewScore < nCurScore || nNewScore > MAX_COMBAT_SCORE_NUM)
			nNewScore  = MAX_COMBAT_SCORE_NUM;

		m_WorldCombatInstanceInfo.org[nIndex].nScore = nNewScore;
	}//endif

}

void KSubWorld::DecCombatInstanceOrgScore(const int nOrgId, const int nScoreDeced)
{
	if ( IsValidCombatID(nOrgId) && nScoreDeced > 0)
	{
		int nOrgIndex = nOrgId - 1;
		int nCurScore = m_WorldCombatInstanceInfo.org[nOrgIndex].nScore;
		int nNewSocre = nCurScore - nScoreDeced;

		if (nNewSocre >= nCurScore)
			return ;

		if (nNewSocre < 0)
			nNewSocre = 0;

		m_WorldCombatInstanceInfo.org[nOrgIndex].nScore = nNewSocre;
	}
}

#endif

#ifdef _SERVER
bool KSubWorld::Reuse()
{
	if (m_State == world_state_ready_for_reuse)
	{
		//初始化，马上就要重用了
		m_dwCurrentTime = 0;
		m_LastPlayerExitTime = 0;
		
		m_State = world_state_idle;
		return true;
	}

	return false;
}
#endif

#ifdef _SERVER
bool KSubWorld::CanEnter(int playerIndex) const
{
	WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(GetWorldTemplateId());
	if (pSetting)
	{
		return pSetting->IsMatchRequirements(playerIndex, m_nIndex);		
	}

	return true;
}
#endif

#ifdef _SERVER
int KSubWorld::CreateWorldTeam(int index, int playerIndex)
{
	if (index >= 0 && index < MAX_WORLD_TEAM_COUNT)
	{
		int originalTeamId = m_WorldTeam[index];
		if (originalTeamId != INVALID_TEAM_ID)
		{
			g_TeamSet.RemoveTeam(originalTeamId);
			m_WorldTeam[index] = INVALID_TEAM_ID;
		}

		int createTeamId = g_TeamSet.CreateTeam(playerIndex);
		if (createTeamId >= 0)
		{
			m_WorldTeam[index] = createTeamId;
			KTeam* pTeam = g_TeamSet.GetTeam(createTeamId);
			if (pTeam)
			{
				pTeam->SetBindWorld(m_nIndex, index);
			}

			return createTeamId;
		}
	}

	return INVALID_TEAM_ID;
}
#endif

#ifdef _SERVER
void KSubWorld::SetOwner(int ownerPlayerIndex)
{
	m_OwnerPlayerIndex = ownerPlayerIndex;
	if (IsValidPlayer(ownerPlayerIndex))
	{
		Player[ownerPlayerIndex].GetInstanceInfo().SetInstanceId(GetWorldTemplateId(), GetInstanceId());
	}
}
#endif

#ifdef _SERVER
int KSubWorld::FirstPlayer()
{
	if (m_Region)
	{
		m_IterateRegion = -1;
		m_pIterateNode = NULL;
		while (!m_pIterateNode)
		{
			m_IterateRegion++;
			if (m_IterateRegion >= 0 && m_IterateRegion < m_nTotalRegion)
			{
				m_pIterateNode = (KIndexNode*)m_Region[m_IterateRegion].m_PlayerList.GetHead();
			}
			else
			{
				return 0;
			}
		}
		if (m_pIterateNode)
			return m_pIterateNode->m_nIndex;
	}

	return 0;
}
#endif

#ifdef _SERVER
int KSubWorld::NextPlayer()
{
	if (m_Region)
	{
		if (m_pIterateNode)
		{
			m_pIterateNode = (KIndexNode*)m_pIterateNode->GetNext();
			while (!m_pIterateNode)
			{
				m_IterateRegion++;
				if (m_IterateRegion >=0 && m_IterateRegion < m_nTotalRegion)
				{
					m_pIterateNode = (KIndexNode*)m_Region[m_IterateRegion].m_PlayerList.GetHead();
				}
				else
				{
					return 0;
				}
			}
			return m_pIterateNode->m_nIndex;
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
void KSubWorld::KickAllPlayer()
{
	if (m_Region)
	{
		int iterateRegion = 0;
		while (iterateRegion < m_nTotalRegion)
		{
			m_Region[iterateRegion].KickAllPlayer();
			iterateRegion++;
		}
	}
}
#endif

#ifdef _SERVER
bool KSubWorld::CheckForExpire()
{
	if (m_ExpireTime > 0 && m_ExpireTime < UNIX_TMIE_STAMP)
	{
		if (m_InstanceID != INVALID_INSTANCE_ID)
		{
			g_SubWorldSet.CloseInstance(m_nIndex);
		}
		else
		{
			Close();
		}
		
		return true;
	}
	else
	{
		return false;
	}
}

void KSubWorld::WorldCombatMapActive()
{
	int SyncCombatSortInterval = ConfigManager::Singleton().GetGlobalVariable(global_var_combat_top10_sort_interval);
	if (SyncCombatSortInterval < 30)
	{
		SyncCombatSortInterval = 30;
	}
	
	if (m_dwCurrentTime % (GAME_FPS * SyncCombatSortInterval) == 0 && IsWorldCombatMap() && CanGainScore() && GetCombatScoreCalcType() == PROGRAME_CALU)
	{
		FlushTop10Player();
	}//endif
	
	if ( m_nSyncIntervalSec < MIN_SYNC_INTERVAL_SEC)
		m_nSyncIntervalSec = MIN_SYNC_INTERVAL_SEC;

	
	if ( g_SubWorldSet.GetGameTime() % ( m_nSyncIntervalSec * GAME_FPS ) == 0 )
	{
		GlobalSyncPlayer();
	}//endif

}

#define        MAX_PACK_SIZE_PACKED       MAX_GLOBAL_SYNC_PLAYER_NUM * sizeof(WORLD_PLAYER_INFO) + sizeof(S2C_WORLD_PLAYER_INFO_SYNC) - 1
#define        MAX_COMPRESSED_BUFF_SIZE   5 * 1024

void KSubWorld::GlobalSyncPlayer()
{
	switch ( GetSyncType() )
	{
	case espacial_sync_type_same_org:	
		SyncPlayerByOrganize();
		break;
	case espacial_sync_type_all:
		SyncAllPlayer();
		break;
	default : break;	
	}//end switch
}

void KSubWorld::SyncPlayerByOrganize()
{
	static char pPackBuffer[MAX_PACK_SIZE_PACKED];
	static char pCompressionBuffer[MAX_COMPRESSED_BUFF_SIZE];
	
	for (int n = 0 ; n < MAX_COMBAT_ORG_NUM ; n++)
	{
		int              CombatOrgId =  n + 1;
		const CombatOrgnize * pInfo  =  GetCombatInstanceOrgInfo(CombatOrgId);

		if (pInfo && pInfo->nPersonNum != 0)
		{
			S2C_WORLD_PLAYER_INFO_SYNC * pInfoSync = (S2C_WORLD_PLAYER_INFO_SYNC *)pPackBuffer;
			pInfoSync->ProtocolType = s2c_world_player_info_sync;
			pInfoSync->Len          = sizeof(S2C_WORLD_PLAYER_INFO_SYNC) - 1 - 1;
			pInfoSync->InfoNum      = 0 ;
			
			WORLD_PLAYER_INFO * pInfo = (WORLD_PLAYER_INFO *)pInfoSync->Infos;
			
			for (int playerIndex = FirstPlayer(); playerIndex > 0; playerIndex = NextPlayer())
			{
				if (IsValidPlayer(playerIndex) && IsValidNpc(Player[playerIndex].m_nIndex) && Npc[Player[playerIndex].m_nIndex].m_WorldCombatOrg == CombatOrgId )
				{
					if (pInfoSync->InfoNum >= MAX_GLOBAL_SYNC_PLAYER_NUM)
						break;
					
					if (pInfoSync->Len + 1 >= MAX_PACK_SIZE_PACKED)
						break;
					
					int nNpcIndex   = Player[playerIndex].GetNpcIndex();
					pInfo->NpcId    = Npc[nNpcIndex].m_dwID;
					pInfo->Orgnize  = Npc[nNpcIndex].m_WorldCombatOrg;
					Npc[nNpcIndex].GetMpsPos(&(pInfo->MapX),&(pInfo->MapY));
					
					++ pInfo ;
					++ pInfoSync->InfoNum;
					pInfoSync->Len += sizeof(WORLD_PLAYER_INFO);
				}//endif
				
			}//end for playerIndex
			
			memcpy(pCompressionBuffer,pPackBuffer,3); //copy header only
			
			BYTE* pCompressBuff = (BYTE*)pCompressionBuffer + 3;
			unsigned int compressBuffLength = MAX_COMPRESSED_BUFF_SIZE - 3;
			BYTE* pCompressSrc = (BYTE*)pPackBuffer + 3;
			unsigned int compressSrcLength = (pInfoSync->Len + 1) - 3;
			
			lzo1x_1_compress(
				pCompressSrc,
				compressSrcLength,
				pCompressBuff,
				&compressBuffLength,
				wrkmem);
			
			if (compressBuffLength < MAX_COMPRESSED_BUFF_SIZE - 3) //Compressed successful
			{
				S2C_WORLD_PLAYER_INFO_SYNC * pFinalInfoSync = (S2C_WORLD_PLAYER_INFO_SYNC *)pCompressionBuffer;
				pFinalInfoSync->Len                         = 3 + compressBuffLength - 1 ;
				
				for (int playerIndex = FirstPlayer(); playerIndex > 0; playerIndex = NextPlayer())
				{
					if (IsValidPlayer(playerIndex) && IsValidNpc(Player[playerIndex].m_nIndex)  && Npc[Player[playerIndex].m_nIndex].m_WorldCombatOrg == CombatOrgId )
					{
						if (g_pServer != NULL)
							g_pServer->PackDataToClient(Player[playerIndex].m_nNetConnectIdx, pCompressionBuffer , pFinalInfoSync->Len + 1);
					}//endif
					
				}//end for playerIndex
				
			}//endif
		}
	}
}

void KSubWorld::SyncAllPlayer()
{
	static char pPackBuffer[MAX_PACK_SIZE_PACKED];
	static char pCompressionBuffer[MAX_COMPRESSED_BUFF_SIZE];

	S2C_WORLD_PLAYER_INFO_SYNC * pInfoSync = (S2C_WORLD_PLAYER_INFO_SYNC *)pPackBuffer;
	pInfoSync->ProtocolType = s2c_world_player_info_sync;
	pInfoSync->Len          = sizeof(S2C_WORLD_PLAYER_INFO_SYNC) - 1 - 1;
	pInfoSync->InfoNum      = 0 ;

	WORLD_PLAYER_INFO * pInfo = (WORLD_PLAYER_INFO *)pInfoSync->Infos;

	for (int playerIndex = FirstPlayer(); playerIndex > 0; playerIndex = NextPlayer())
	{
		if (IsValidPlayer(playerIndex) && IsValidNpc(Player[playerIndex].m_nIndex))
		{
			if (pInfoSync->InfoNum >= MAX_GLOBAL_SYNC_PLAYER_NUM)
				break;

			if (pInfoSync->Len + 1 >= MAX_PACK_SIZE_PACKED)
				break;

			int nNpcIndex   = Player[playerIndex].GetNpcIndex();
			pInfo->NpcId    = Npc[nNpcIndex].m_dwID;
			pInfo->Orgnize  = Npc[nNpcIndex].m_WorldCombatOrg;
			Npc[nNpcIndex].GetMpsPos(&(pInfo->MapX),&(pInfo->MapY));
		   
			++ pInfo ;
			++ pInfoSync->InfoNum;
			pInfoSync->Len += sizeof(WORLD_PLAYER_INFO);
		}//endif

	}//end for playerIndex
	
	memcpy(pCompressionBuffer,pPackBuffer,3); //copy header only

	BYTE* pCompressBuff = (BYTE*)pCompressionBuffer + 3;
	unsigned int compressBuffLength = MAX_COMPRESSED_BUFF_SIZE - 3;
	BYTE* pCompressSrc = (BYTE*)pPackBuffer + 3;
	unsigned int compressSrcLength = (pInfoSync->Len + 1) - 3;

	lzo1x_1_compress(
		pCompressSrc,
		compressSrcLength,
		pCompressBuff,
		&compressBuffLength,
		wrkmem);
	
	if (compressBuffLength < MAX_COMPRESSED_BUFF_SIZE - 3) //Compressed successful
	{
		S2C_WORLD_PLAYER_INFO_SYNC * pFinalInfoSync = (S2C_WORLD_PLAYER_INFO_SYNC *)pCompressionBuffer;
		pFinalInfoSync->Len                         = 3 + compressBuffLength - 1 ;

		for (int playerIndex = FirstPlayer(); playerIndex > 0; playerIndex = NextPlayer())
		{
			if (IsValidPlayer(playerIndex) && IsValidNpc(Player[playerIndex].m_nIndex))
			{
				if (g_pServer != NULL)
					g_pServer->PackDataToClient(Player[playerIndex].m_nNetConnectIdx, pCompressionBuffer , pFinalInfoSync->Len + 1);
			}//endif
			
		}//end for playerIndex

	}//endif

}

void KSubWorld::FlushTop10Player()
{
	m_WorldCombatInstanceInfo.ClearCombatTop10();

	for (int n = 0 ;n < MAX_SCORE_ORG_SYNC && n< MAX_COMBAT_ORG_NUM ; n++)
	{
		int CombatOrgId = n + 1;
		int used = 0; //记录数组中的填充个数
		const CombatOrgnize * pInfo  =  GetCombatInstanceOrgInfo(CombatOrgId);
		CombatTop10PlayerTempArray TempTop10Array[MAX_COMBAT_TOP];
		memset(TempTop10Array, 0, sizeof(TempTop10Array));
		
			
		if (pInfo && pInfo->nPersonNum != 0 && pInfo->nScore != 0)
		{//得到一个合法的阵营

			//排名部分
			for (int playerIndex = FirstPlayer(); playerIndex > 0; playerIndex = NextPlayer())
			{
				if (IsValidPlayer(playerIndex) && IsValidNpc(Player[playerIndex].m_nIndex) && Npc[Player[playerIndex].m_nIndex].m_WorldCombatOrg == CombatOrgId && Player[playerIndex].GetMyCombatScore() != 0)
				{//能进到这里的应该是一个合法的在战场中的玩家，而且他的阵营和当前计算阵营相同,而且其得分不为0，否者不参加排名。
					if (used < MAX_COMBAT_TOP)//前十还没满
					{
						if (used == 0)//第一个位置，直接插
						{
							TempTop10Array[used].nPlayerIndex = playerIndex;
							TempTop10Array[used].nscore       = Player[playerIndex].GetMyCombatScore();
						}
						else//非第一个位置，先移动比当前玩家分数小的玩家信息，再插入当前玩家信息
						{
							int i = used - 1;
							while (i >= 0 && Player[playerIndex].GetMyCombatScore() > TempTop10Array[i].nscore)
							{
								TempTop10Array[i + 1].nPlayerIndex = TempTop10Array[i].nPlayerIndex;
								TempTop10Array[i + 1].nscore = TempTop10Array[i].nscore;
									
								--i;
							}//end for while
								
							TempTop10Array[i + 1].nPlayerIndex = playerIndex;
							TempTop10Array[i + 1].nscore = Player[playerIndex].GetMyCombatScore();
						}
						
						++used;
					} 
					else//前十名已满，如果进行替换调整
					{
						int i = MAX_COMBAT_TOP - 1;
						if(Player[playerIndex].GetMyCombatScore() <= TempTop10Array[i].nscore)
							continue;
						
						i = MAX_COMBAT_TOP - 2;
						
						while ( i >= 0 && Player[playerIndex].GetMyCombatScore() > TempTop10Array[i].nscore)
						{
							TempTop10Array[i + 1].nPlayerIndex = TempTop10Array[i].nPlayerIndex;
							TempTop10Array[i + 1].nscore = TempTop10Array[i].nscore;
							--i;	
						}//end for while
						
						if(i >= -1)
						{
							TempTop10Array[i + 1].nPlayerIndex = playerIndex;
							TempTop10Array[i + 1].nscore = Player[playerIndex].GetMyCombatScore();
						}
						
					}//end for else		
				}//end for playerindex
			}//endfor
			//排名部分完


			//填充并压缩协议部分
			COMBAT_TOP10_INFO *pSyncComBatTopInfo = (COMBAT_TOP10_INFO*) m_WorldCombatInstanceInfo.CombatTop10[n].protocolBuff;
			pSyncComBatTopInfo->Protocol = s2c_world_combat_top10_info;
			pSyncComBatTopInfo->MemberCount = 0;
			pSyncComBatTopInfo->ProtocolSize = sizeof(COMBAT_TOP10_INFO) - 1 - sizeof(COMBAT_TOP10_MEMBER_INFO);
			
			int loopcount = 0;
			while (loopcount < used)
			{
				int playindex = TempTop10Array[pSyncComBatTopInfo->MemberCount].nPlayerIndex;
				if (IsValidPlayer(playindex) && IsValidNpc(Player[playindex].m_nIndex) && pSyncComBatTopInfo->MemberCount < MAX_COMBAT_TOP)
				{
					KNpc& npc = Npc[Player[playindex].m_nIndex];	
					BYTE sexInfo = npc.GetSex();
					BYTE classInfo = npc.GetSeries() + (Player[playindex].GetSkillSeries() + 1) * 3;
					
					pSyncComBatTopInfo->Member[pSyncComBatTopInfo->MemberCount].Level = npc.GetLevel();
					pSyncComBatTopInfo->Member[pSyncComBatTopInfo->MemberCount].ClassAndSexInfo = (sexInfo << 4) + classInfo;
					pSyncComBatTopInfo->Member[pSyncComBatTopInfo->MemberCount].Score  = Player[playindex].GetMyCombatScore();
					strncpy(pSyncComBatTopInfo->Member[pSyncComBatTopInfo->MemberCount].Name, Npc[Player[playindex].m_nIndex].Name, MAXSIZE_ROLENAME - 1);
					pSyncComBatTopInfo->Member[pSyncComBatTopInfo->MemberCount].Name[MAXSIZE_ROLENAME -1] = '\0';
					
					pSyncComBatTopInfo->ProtocolSize += sizeof(COMBAT_TOP10_MEMBER_INFO);
					pSyncComBatTopInfo->MemberCount++;	
				}
				
				++loopcount;
			}
			
			if(CompressProtocol((BYTE*)(pSyncComBatTopInfo), pSyncComBatTopInfo->ProtocolSize + 1, COMBAT_TOP10_BUFF_LEN))
				m_WorldCombatInstanceInfo.CombatTop10[n].CompressSuccess = TRUE;
			else
				m_WorldCombatInstanceInfo.CombatTop10[n].CompressSuccess = FALSE;
		//填充并压缩协议部分完

		}//endif
		
	}//endfor
}

bool KSubWorld::CanGainScore() const
{
	WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(GetWorldTemplateId());
	
	if (pSetting)
	{
		return pSetting->CanGainSroce;
	}

	return FALSE;
}

void KSubWorld::SetOrgScore(DWORD nOrgId, int Point)
{
	if (IsValidCombatID(nOrgId) && Point > 0)
	{
		int nIndex        = nOrgId -1;
		
		if ( Point < 0 || Point > MAX_COMBAT_SCORE_NUM)
			Point  = MAX_COMBAT_SCORE_NUM;
		
		m_WorldCombatInstanceInfo.org[nIndex].nScore = Point;
	}
}

void KSubWorld::SyncTongWarInfo()
{
	ConfigManager& cm = ConfigManager::Singleton();

	int SyncWarCommanderInterval = cm.GetGlobalVariable(global_var_tong_war_commander_sync_interval);
	if (SyncWarCommanderInterval < MIN_WAR_COMMANDER_INTERVAL)
	{
		SyncWarCommanderInterval = STD_WAR_COMMANDER_INTERVAL;
	}
	
	int SyncTongWarCommanderSwitch = cm.GetGlobalVariable(global_var_tong_war_commander_sync_switch);

	if ((SyncTongWarCommanderSwitch == 1) && (m_dwCurrentTime % (GAME_FPS * SyncWarCommanderInterval) == 0))
	{
		WarCommanderSync();
	}
	
}


#define        MAX_PACK_SIZE_PACKED_WARCOMMANDERINFO       (WAR_TOTAL_NORMAL_COMMANDER_NUM + 2) * sizeof(WAR_COMMANDER_INFO) + sizeof(S2C_WAR_COMMANDER_INFO_SYNC) - 1 
#define		   TEMP_COMMANDER_BUFF_SIZE                     WAR_SUB_COMMANDER_NUM * sizeof(WAR_COMMANDER_INFO) + MAX_PACK_SIZE_PACKED_WARCOMMANDERINFO

void KSubWorld::WarCommanderSync()
{
	static char pPackBuffer[MAX_PACK_SIZE_PACKED_WARCOMMANDERINFO];
	
	S2C_WAR_COMMANDER_INFO_SYNC* pInfoSync = (S2C_WAR_COMMANDER_INFO_SYNC* )pPackBuffer;
	pInfoSync->ProtocolType = s2c_war_commander_sync;	


	FSWarInfo* warinfo = GetGlobalWarInfoManager().GetRecordByMapId(m_SubWorldID);
	if (warinfo == NULL || warinfo->warState != FS_WAR_STATE_PROCESS)
	{
		return;
	}

	SocialUnit* pUnit[tongwarfangshu] = {0};
	pUnit[invader] = ServerSocialUnitMgr::Singleton().GetUnit(warinfo->invaderGUID, enSUTplId_Tong);
	pUnit[defender] = ServerSocialUnitMgr::Singleton().GetUnit(warinfo->defenderGUID, enSUTplId_Tong);
	
	for(int i = 0; i < tongwarfangshu; ++i)
	{
		if (pUnit[i] == NULL)
			continue;

		pInfoSync->Len              = sizeof(S2C_WAR_COMMANDER_INFO_SYNC) - 1 - 1;
		pInfoSync->InfoNum          = 0 ;
		WAR_COMMANDER_INFO * pInfo  = (WAR_COMMANDER_INFO *)pInfoSync->Infos;
		int nLMPlayerIndex = 0;
		int nSPlayerIndex  = 0;
		
		nLMPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(pUnit[i]->GetOwnerName());
		if (IsValidPlayer(nLMPlayerIndex))  //有可能开战了但是联盟长不在线or没取到联盟长的Index
		{
			int nLMNpcIndex = Player[nLMPlayerIndex].GetNpcIndex();
			if (IsValidNpc(nLMNpcIndex))
			{
				if (Npc[nLMNpcIndex].m_SubWorldIndex == m_nIndex)
				{
					FillDataFunc(pInfoSync, &pInfo, nLMNpcIndex, war_totalcommander);
				}
			}
		}
		
		SocialUnit::UnitIterator iterS;
		SocialUnit* pSTempUnit = NULL;
		while ((pSTempUnit = pUnit[i]->NextSubUnit(iterS)))  //yes, it is = not == :)
		{
			nSPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(pSTempUnit->GetOwnerName());
			if (IsValidPlayer(nSPlayerIndex) && nSPlayerIndex != nLMPlayerIndex)
			{
				int nSNpcIndex = Player[nSPlayerIndex].GetNpcIndex();
				if (IsValidNpc(nSNpcIndex))
				{
					if (Npc[nSNpcIndex].m_SubWorldIndex == m_nIndex)
					{
						FillDataFunc(pInfoSync, &pInfo, nSNpcIndex, war_normalcommander);
					}
				}
			}
		}
		
		bool nCompressSuccessed = CompressProtocol((BYTE* )pInfoSync, pInfoSync->Len + 1, TEMP_COMMANDER_BUFF_SIZE);
		
		if (nCompressSuccessed)
		{
			for (int nPlayerIterator = FirstPlayer(); IsValidPlayer(nPlayerIterator); nPlayerIterator = NextPlayer())
			{
				SocialUnit* pUnitPlayer = GetLeafUnit(nPlayerIterator, enSUTplId_Tong);
				if (pUnitPlayer)
				{
					SocialUnit* pUnitTong = GetUpNUnit(pUnitPlayer, pUnit[i]->GetLayer());
					if (pUnitTong && pUnitTong == pUnit[i])
					{
						g_pServer->PackDataToClient(Player[nPlayerIterator].m_nNetConnectIdx, (void* )pInfoSync, pInfoSync->Len + 1);
					}
				}
			}
		}
	}
}

void KSubWorld::FillDataFunc(S2C_WAR_COMMANDER_INFO_SYNC* pInfoSync, WAR_COMMANDER_INFO** ppInfo, int nNpcIndex, int nDuty)
{
	(*ppInfo)->dNpcId = Npc[nNpcIndex].m_dwID;
	(*ppInfo)->nDuty  = nDuty;
	Npc[nNpcIndex].GetMpsPos(&((*ppInfo)->nMpsX), &((*ppInfo)->nMpsY));
	
	++(*ppInfo);
	++(pInfoSync->InfoNum);
	pInfoSync->Len += sizeof(WAR_COMMANDER_INFO);
	
	return;
}


void KSubWorld::ClearWarMap()
{
	char pPackBuffer[TEMP_COMMANDER_BUFF_SIZE];
	ZeroMemory(pPackBuffer, sizeof(pPackBuffer));
	
	S2C_WAR_COMMANDER_INFO_SYNC * pInfoSync = (S2C_WAR_COMMANDER_INFO_SYNC *)pPackBuffer;
	pInfoSync->ProtocolType     = s2c_war_commander_sync;	
	pInfoSync->Len              = sizeof(S2C_WAR_COMMANDER_INFO_SYNC) - 1 - 1;
	pInfoSync->InfoNum          = 0 ;
	
	//压缩信息
	bool bCompressSuccess = CompressProtocol((BYTE*)pInfoSync, pInfoSync->Len + 1, TEMP_COMMANDER_BUFF_SIZE);	

	if (bCompressSuccess) //Compressed successful
	{
		for (int playerIndex = FirstPlayer(); playerIndex > 0; playerIndex = NextPlayer())
		{
			if (IsValidPlayer(playerIndex) && IsValidNpc(Player[playerIndex].m_nIndex)  && Npc[Player[playerIndex].m_nIndex].m_SubWorldIndex == m_nIndex )
			{
				if (g_pServer != NULL)
					g_pServer->PackDataToClient(Player[playerIndex].m_nNetConnectIdx, (void*)pInfoSync , pInfoSync->Len + 1);
			}//endif
		}//end for playerIndex
	}

}

#endif

int KSubWorld::GetCombatScoreCalcType() const
{
	return m_CombatScoreCalcType;
}

#ifdef _SERVER
void KSubWorld::SendCustomStringToPlayer(int strIndex, int playerIndex)
{
	if (NULL == g_pServer)
		return;
	
	char protocolBuff[WORLD_CUSTOM_STRING_PROTOCOL_BUFF];
	unsigned int buffSize = sizeof(protocolBuff);
	
	if (PrepareCustomStringProtocol(strIndex, protocolBuff, buffSize))
	{
		if (IsValidPlayer(playerIndex))
		{
			g_pServer->PackDataToClient(Player[playerIndex].GetNetConnectIdx(), (BYTE*)protocolBuff, buffSize);
		}
	}
}

void KSubWorld::SendCustomStringToAllPlayer(int strIndex)
{
	if (NULL == g_pServer)
		return;
	
	char protocolBuff[WORLD_CUSTOM_STRING_PROTOCOL_BUFF];
	unsigned int buffSize = sizeof(protocolBuff);
	
	if (PrepareCustomStringProtocol(strIndex, protocolBuff, buffSize))
	{
		for (int playerIndex = FirstPlayer(); playerIndex > 0; playerIndex = NextPlayer())
		{
			if (IsValidPlayer(playerIndex))
			{
				g_pServer->PackDataToClient(Player[playerIndex].GetNetConnectIdx(), (BYTE*)protocolBuff, buffSize);
			}
		}
	}
}

bool KSubWorld::PrepareCustomStringProtocol(int strIndex, char* protocolBuff, unsigned int& buffSize)
{
	if (strIndex >= 0 && strIndex < MAX_CUSTOM_STRING_COUNT && protocolBuff && buffSize >= WORLD_CUSTOM_STRING_PROTOCOL_BUFF)
	{
		memset(protocolBuff, 0, buffSize);
		
		WORLD_CUSTOME_STRING* pProtocol = (WORLD_CUSTOME_STRING*)protocolBuff;
		pProtocol->Protocol = s2c_world_custom_string;
		pProtocol->Length = sizeof(WORLD_CUSTOME_STRING) - 1;
		
		strncpy(pProtocol->CustomString, m_CustomString[strIndex], MAX_WORLD_CUSTOM_STRING_LENGTH);
		pProtocol->CustomString[MAX_WORLD_CUSTOM_STRING_LENGTH - 1] = 0;
		pProtocol->Length += strlen(pProtocol->CustomString);
		
		if (CompressProtocol((BYTE*)protocolBuff, pProtocol->Length + 1, buffSize))
		{
			buffSize = pProtocol->Length + 1;
			return true;
		}
	}

	return false;
}
#endif