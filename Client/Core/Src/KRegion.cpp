#include "KCore.h"
#include "KNpc.h"
#include "KNpcSet.h"

#ifndef _SERVER
#include "KMissleSet.h"
#endif

#include "KObjSet.h"
#include "KPlayerSet.h"
#include "KPlayer.h"

#ifndef _SERVER
#include "KMissle.h"
#endif

#include "KObj.h"
#include "KSubWorld.h"
#include "KMath.h"
#ifdef _SERVER
#include "KNpcTemplate.h"
#endif
#include "Scene/SceneDataDef.h"
#include "KRegion.h"
#include "MyAssert.H"
#include "KSubWorldSet.h"

#ifndef _SERVER
#include "scene/KScenePlaceC.h"
#include "KDirtyNpcSet.h"
#endif

#ifdef _SERVER
#include "npc_save.h"
#endif

//lixuewu 临近区域表
static const int OffSetTable[3][3] = 
{
	{-1 , 4, 0},
	{2 , 3, 1},
	{6 , 5, 7},
};

#ifdef _SERVER 
KRegion::KRegion():m_SpawnPointList(this) 
#else
KRegion::KRegion()
#endif
{
	m_nIndex		= -1;
	m_RegionID		= -1;
	m_nActive		= 0;
	m_nNpcSyncCounter = 0;
	m_nObjSyncCounter = 0;
	ZeroMemory(m_nConnectRegion, sizeof(m_nConnectRegion));
#ifdef _SERVER
	memset(m_dwTrap, 0, sizeof(m_dwTrap));
#endif
}

KRegion::~KRegion()
{
}

BOOL KRegion::Init(int nWidth, int nHeight)
{
	ZeroMemory(m_nNpcRef, sizeof(m_nNpcRef));
	ZeroMemory(m_nObjRef, sizeof(m_nObjRef));
	return TRUE;
}

BOOL KRegion::Load(int nX, int nY)
{
#ifndef _SERVER
	Close();
#endif

	m_RegionID = MAKELONG(nX, nY);

	m_nRegionX = nX * REGION_PIXEL_WIDTH;
	m_nRegionY = nY * REGION_PIXEL_HEIGHT;

	// 下方
	m_nConRegionID[0] = MAKELONG(nX, nY + 1);
	// 左下方
	m_nConRegionID[1] = MAKELONG(nX - 1, nY + 1);
	// 左方
	m_nConRegionID[2] = MAKELONG(nX - 1, nY);
	// 左上方
	m_nConRegionID[3] = MAKELONG(nX - 1, nY - 1);
	// 上方
	m_nConRegionID[4] = MAKELONG(nX, nY - 1);
	// 右上方
	m_nConRegionID[5] = MAKELONG(nX + 1, nY - 1);
	// 右方
	m_nConRegionID[6] = MAKELONG(nX + 1, nY);
	// 右下方
	m_nConRegionID[7] = MAKELONG(nX + 1, nY + 1);

	return TRUE;
}

#ifdef _SERVER
//----------------------------------------------------------------------
//	功能：载入服务器端地图上本region 的 object数据（包括npc、trap、box等）
//	注意：使用此函数之前必须保证当前路径是本地图路径
//----------------------------------------------------------------------
BOOL KRegion::LoadObject(int nSubWorld, int nX, int nY)
{
	KPakFile	cData;
	char		szFilePath[80];
	char		szFile[80];

	g_GetFilePath(szFile);

	sprintf(szFilePath, "\\%sv_%03d", szFile, nY);

	sprintf(szFile, "%s\\%03d_%s", szFilePath, nX, REGION_COMBIN_FILE_NAME_SERVER);
	if (cData.Open(szFile))
	{
		DWORD	dwHeadSize;
		DWORD	dwMaxElemFile = 0;
		KCombinFileSection	sElemFile[REGION_ELEM_FILE_COUNT];

		if (cData.Size() < sizeof(DWORD) + sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT)
		{
			ZeroMemory(m_Obstacle, sizeof(m_Obstacle));
			goto gotoCLOSE;
		}
		cData.Read(&dwMaxElemFile, sizeof(DWORD));
		if (dwMaxElemFile > REGION_ELEM_FILE_COUNT)
		{
			cData.Read(sElemFile, sizeof(sElemFile));
			cData.Seek(sizeof(KCombinFileSection) * (dwMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
		}
		else
		{
			cData.Read(sElemFile, sizeof(sElemFile));
		}
		dwHeadSize = sizeof(DWORD) + sizeof(KCombinFileSection) * dwMaxElemFile;

		// 载入障碍数据
		cData.Seek(dwHeadSize + sElemFile[REGION_OBSTACLE_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerObstacle(&cData, sElemFile[REGION_OBSTACLE_FILE_INDEX].uLength);

		// 载入trap信息
		cData.Seek(dwHeadSize + sElemFile[REGION_TRAP_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerTrap(&cData, sElemFile[REGION_TRAP_FILE_INDEX].uLength);

		// 载入npc数据
		cData.Seek(dwHeadSize + sElemFile[REGION_NPC_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadServerNpc(nSubWorld, &cData, sElemFile[REGION_NPC_FILE_INDEX].uLength);

		//----------------------------------------------------------------------------------
		// 暂时屏蔽，该功能不再使用。
		// 载入obj数据
		//cData.Seek(dwHeadSize + sElemFile[REGION_OBJ_FILE_INDEX].uOffset, FILE_BEGIN);
		//LoadServerObj(nSubWorld, &cData, sElemFile[REGION_OBJ_FILE_INDEX].uLength);
		//----------------------------------------------------------------------------------

gotoCLOSE:
		cData.Close();
	}
	else
	{
		LoadServerObstacle(NULL, 0);
		LoadServerTrap(NULL, 0);
	}
	
	return TRUE;
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	功能：载入客户端地图上本region 的 object数据（包括npc、box等）
//	如果 bLoadNpcFlag == TRUE 需要载入 clientonly npc else 不载入
//----------------------------------------------------------------------
BOOL KRegion::LoadObject(int nSubWorld, int nX, int nY, char *lpszPath)
{
	char	szPath[FILE_NAME_LENGTH], szFile[FILE_NAME_LENGTH];

	if (!lpszPath || !lpszPath[0] || strlen(lpszPath) >= FILE_NAME_LENGTH)
		return FALSE;
	sprintf(szPath, "\\%s\\v_%03d", lpszPath, nY);

	// 载入npc数组中位于本地的 client npc
	NpcSet.InsertNpcToRegion(this->m_nIndex);

	KPakFile	cData;
	sprintf(szFile, "%s\\%03d_%s", szPath, nX, REGION_COMBIN_FILE_NAME_CLIENT);
	if (cData.Open(szFile))
	{
		DWORD	dwHeadSize;
		DWORD	dwMaxElemFile = 0;
		KCombinFileSection	sElemFile[REGION_ELEM_FILE_COUNT];

		if (cData.Size() < sizeof(DWORD) + sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT)
			goto gotoCLOSE;
		cData.Read(&dwMaxElemFile, sizeof(DWORD));
		if (dwMaxElemFile > REGION_ELEM_FILE_COUNT)
		{
			cData.Read(sElemFile, sizeof(sElemFile));
			cData.Seek(sizeof(KCombinFileSection) * (dwMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
		}
		else
		{
			cData.Read(sElemFile, sizeof(sElemFile));
		}
		dwHeadSize = sizeof(DWORD) + sizeof(KCombinFileSection) * dwMaxElemFile;

		// 载入npc数据
		cData.Seek(dwHeadSize + sElemFile[REGION_NPC_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadClientNpc(&cData, sElemFile[REGION_NPC_FILE_INDEX].uLength);

		// 载入obj数据
		cData.Seek(dwHeadSize + sElemFile[REGION_OBJ_FILE_INDEX].uOffset, FILE_BEGIN);
		LoadClientObj(&cData, sElemFile[REGION_OBJ_FILE_INDEX].uLength);

gotoCLOSE:
		cData.Close();
	}

	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	功能：载入服务器端地图上本 region 的障碍数据
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerObstacle(KPakFile *pFile, DWORD dwDataSize)
{
    if (pFile != NULL && dwDataSize == sizeof(this->m_Obstacle))
    {
        pFile->Read((LPVOID)m_Obstacle, sizeof(m_Obstacle));
    }
    else
    {
        //            memset(this->m_Obstacle, 0, sizeof(this->m_Obstacle));
        for (int x =0; x < REGION_CELL_WIDTH; x++)
        {
            for(int y =0; y < REGION_CELL_HEIGHT; y++)
            {
                m_Obstacle[x][y] = (((Obstacle_Full & 0x0f) << 4) | (Obstacle_Normal & 0x0f));
            }
        }
    }
	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	功能：载入服务器端地图上本 region 的 trap 数据
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerTrap(KPakFile *pFile, DWORD dwDataSize)
{
	memset(m_dwTrap, 0, sizeof(m_dwTrap));
	if (!pFile || dwDataSize < sizeof(KTrapFileHead))
		return FALSE;

	KTrapFileHead	sTrapFileHead;
	KSPTrap			sTrapCell;
	int				i, j;

	pFile->Read(&sTrapFileHead, sizeof(KTrapFileHead));
	if (sTrapFileHead.uNumTrap * sizeof(KSPTrap) + sizeof(KTrapFileHead) != dwDataSize)
		return FALSE;
	for (i = 0; i < sTrapFileHead.uNumTrap; i++)
	{
		pFile->Read(&sTrapCell, sizeof(KSPTrap));
		if (sTrapCell.cY >= REGION_CELL_HEIGHT || sTrapCell.cX + sTrapCell.cNumCell - 1 >= REGION_CELL_WIDTH)
			continue;
		for (j = 0; j < sTrapCell.cNumCell; j++)
		{
			m_dwTrap[sTrapCell.cX + j][sTrapCell.cY] = sTrapCell.uTrapId;
		}
	}

	return TRUE;
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------
//	功能：载入服务器端地图上本 region 的 npc 数据
//----------------------------------------------------------------------
BOOL	KRegion::LoadServerNpc(int nSubWorld, KPakFile *pFile, DWORD dwDataSize)
{
	return	m_SpawnPointList.LoadSpawnPoints(nSubWorld, pFile, dwDataSize);
}
#endif

//#ifdef _SERVER
//----------------------------------------------------------------------
//	功能：载入服务器端地图上本 region 的 obj 数据
//----------------------------------------------------------------------
// BOOL	KRegion::LoadServerObj(int nSubWorld, KPakFile *pFile, DWORD dwDataSize)
// {
// 	return ObjSet.ServerLoadRegionObj(nSubWorld, pFile, dwDataSize);
// }
//#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	功能：载入客户端地图上本 region 的 clientonlynpc 数据
//----------------------------------------------------------------------
BOOL	KRegion::LoadClientNpc(KPakFile *pFile, DWORD dwDataSize)
{
	if (!pFile || dwDataSize < sizeof(KNpcFileHead))
		return FALSE;

	KNpcFileHead	sNpcFileHead;
	KSPNpc			sNpcCell;
	DWORD			i;
	KClientNpcID	sTempID;
	int				nNpcNo;

	pFile->Read(&sNpcFileHead, sizeof(KNpcFileHead));
	for (i = 0; i < sNpcFileHead.uNumNpc; i++)
	{
		pFile->Read(&sNpcCell, sizeof(KSPNpc));
		sTempID.m_dwRegionID = m_RegionID;
		sTempID.m_nNo = i;
		nNpcNo = NpcSet.SearchClientID(sTempID);
		if (nNpcNo == 0)
		{
			int nIdx = NpcSet.AddClientNpc(sNpcCell.nTemplateID, LOWORD(m_RegionID), HIWORD(m_RegionID), sNpcCell.nPositionX, sNpcCell.nPositionY, i);
			if (nIdx > 0)
			{
				Npc[nIdx].SendCommand(do_stand);
			}
		}
	}

	return TRUE;
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	功能：载入客户端地图上本 region 的 clientonlyobj 数据
//----------------------------------------------------------------------
BOOL	KRegion::LoadClientObj(KPakFile *pFile, DWORD dwDataSize)
{
	return ObjSet.ClientLoadRegionObj(pFile, dwDataSize);
}
#endif

#ifndef _SERVER
//----------------------------------------------------------------------
//	功能：载入障碍数据给小地图
//----------------------------------------------------------------------
void	KRegion::LoadLittleMapData(int nX, int nY, char *lpszPath, BYTE *lpbtObstacle)
{
	if (!lpbtObstacle)
		return;

	char	szPath[FILE_NAME_LENGTH], szFile[FILE_NAME_LENGTH];
	int		i, j;

	if (!lpszPath || !lpszPath[0] || strlen(lpszPath) >= FILE_NAME_LENGTH)
		return ;
	sprintf(szPath, "\\%s\\v_%03d", lpszPath, nY);

	KPakFile	cData;
	long		nTempTable[REGION_CELL_WIDTH][REGION_CELL_HEIGHT];	// 地图障碍信息表

	sprintf(szFile, "%s\\%03d_%s", szPath, nX, REGION_COMBIN_FILE_NAME_CLIENT);
	if (cData.Open(szFile))
	{
		DWORD	dwHeadSize;
		DWORD	dwMaxElemFile = 0;
		KCombinFileSection	sElemFile[REGION_ELEM_FILE_COUNT];

		if (cData.Size() < sizeof(DWORD) + sizeof(KCombinFileSection) * REGION_ELEM_FILE_COUNT)
		{
			ZeroMemory(nTempTable, sizeof(nTempTable));
		}
		else
		{
			cData.Read(&dwMaxElemFile, sizeof(DWORD));
			if (dwMaxElemFile > REGION_ELEM_FILE_COUNT)
			{
				cData.Read(sElemFile, sizeof(sElemFile));
				cData.Seek(sizeof(KCombinFileSection) * (dwMaxElemFile - REGION_ELEM_FILE_COUNT), FILE_CURRENT);
			}
			else
			{
				cData.Read(sElemFile, sizeof(sElemFile));
			}

			if (sElemFile[REGION_OBSTACLE_FILE_INDEX].uLength == sizeof(nTempTable))
			{
				dwHeadSize = sizeof(DWORD) + sizeof(KCombinFileSection) * dwMaxElemFile;
				cData.Seek(dwHeadSize + sElemFile[REGION_OBSTACLE_FILE_INDEX].uOffset, FILE_BEGIN);
				cData.Read(nTempTable, sizeof(nTempTable));
			}
			else
			{
				ZeroMemory(nTempTable, sizeof(nTempTable));
			}
		}

		cData.Close();
	}
	else
	{
		sprintf(szFile, "%03d_%s", nX, REGION_OBSTACLE_FILE);
		if (cData.Open(szFile))
			cData.Read((LPVOID)nTempTable, sizeof(nTempTable));
		else
			ZeroMemory(nTempTable, sizeof(nTempTable));
		cData.Close();
	}
	
	for (i = 0; i < REGION_CELL_HEIGHT; i++)
	{
		for (j = 0; j < REGION_CELL_WIDTH; j++)
		{
			lpbtObstacle[i * REGION_CELL_WIDTH + j] = (BYTE)nTempTable[j][i];
		}
	}
}
#endif

void KRegion::Activate()
{
#ifdef _SERVER
	m_SpawnPointList.Active();
#endif

	const int SYNC_FACTOR = 3;

	KIndexNode *pNode = NULL;
	KIndexNode *pTmpNode = NULL;
	int	nCounter = 0;
	bool sendNpcSync = (m_nNpcSyncCounter % SYNC_FACTOR == 0);
	int sendNpcSyncCount = m_nNpcSyncCounter / SYNC_FACTOR;

	pNode = (KIndexNode *)m_NpcList.GetHead();

	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();

		int nNpcIdx = pNode->m_nIndex;
#ifdef _SERVER
		if (sendNpcSync && (nCounter == sendNpcSyncCount))
		{
			// 发送同步信号
			Npc[nNpcIdx].NormalSync();
		}
		nCounter++;
#endif
		Npc[nNpcIdx].Activate();

		pNode = pTmpNode;		
	}
	m_nNpcSyncCounter++;
	if (m_nNpcSyncCounter > m_NpcList.GetNodeCount() * SYNC_FACTOR)
	{
		m_nNpcSyncCounter = 0;
	}

	nCounter = 0;
	bool sendObjSync = (m_nObjSyncCounter % SYNC_FACTOR == 0);
	int sendObjSyncCount = m_nObjSyncCounter / SYNC_FACTOR;

	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();
#ifdef _SERVER
		if (sendObjSync && (nCounter == sendObjSyncCount))
		{
			Object[pNode->m_nIndex].SyncState();
		}
		nCounter++;
#endif
#ifndef _SERVER
		if ( pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_OBJECT )
		{
#endif
			Object[pNode->m_nIndex].Activate();
#ifndef _SERVER
		}
#endif	
		pNode = pTmpNode;
	}
	m_nObjSyncCounter++;
	if (m_nObjSyncCounter > m_ObjList.GetNodeCount() * SYNC_FACTOR)
	{
		m_nObjSyncCounter = 0;
	}

#ifdef _SERVER	
	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();
		Player[pNode->m_nIndex].Active();
		pNode = pTmpNode;		
	}
#endif

#ifndef _SERVER
	pNode = (KIndexNode *)m_MissleList.GetHead();
	while(pNode)
	{
		pTmpNode = (KIndexNode *)pNode->GetNext();
		Missle[pNode->m_nIndex].Activate();		
		pNode = pTmpNode;
	}

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex == m_nIndex)	// 是Player所在的Region
	{
		Player[CLIENT_PLAYER_INDEX].Active();
	}
#endif
}


void KRegion::AddNpc(int nIdx)
{
	if (nIdx > 0 && nIdx < MAX_NPC)
	{
		// changed by chenshanglin for game video on 2005-10-16
		// 因为 NPC 随即走动，回放录像时有可能和原来不一样，如：原来
		// NPC走出了当前区域，它的计数变为0，过一会儿走进来了，这里就会
		// 增加NPC，但回放时可能这个NPC并没有走出去，但下行包过来后，
		// 仍然会增加NPC，这时候就会导致下面的断言失败
		
			_ASSERT(Npc[nIdx].m_Node.m_Ref == 0);
		// changed end

		if (Npc[nIdx].m_Node.m_Ref == 0)
		{
			m_NpcList.AddTail(&Npc[nIdx].m_Node);
			Npc[nIdx].m_Node.AddRef();
			if (Npc[nIdx].m_Kind == kind_building)
			{
				m_nActive++;
			}
		}
	}
}

void KRegion::RemoveNpc(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;

	DecNpcRef(nIdx);
	//_ASSERT(Npc[nIdx].m_Node.m_Ref > 0);
	if (Npc[nIdx].m_Node.m_Ref > 0)
	{
		Npc[nIdx].m_Node.Remove();
		Npc[nIdx].m_Node.Release();
		if (Npc[nIdx].m_Kind == kind_building)
		{
			m_nActive--;
		}
	}
#ifndef _SERVER
	Npc[nIdx].RemoveRes();
#endif
}

#ifndef _SERVER
void KRegion::AddMissle(int nId)
{
	if (nId > 0 && nId < MAX_MISSLE)
	{
			_ASSERT(Missle[nId].m_Node.m_Ref == 0);


		if (Missle[nId].m_Node.m_Ref == 0)
		{
			m_MissleList.AddTail(&Missle[nId].m_Node);
			Missle[nId].m_Node.AddRef();
		}
	}
}

void KRegion::RemoveMissle(int nId)
{
	if (nId > 0 && nId < MAX_MISSLE)
	{
		_ASSERT(Missle[nId].m_Node.m_Ref > 0);
		if (Missle[nId].m_Node.m_Ref > 0)
		{
			Missle[nId].m_Node.Remove();
			Missle[nId].m_Node.Release();
		}
	}
}
#endif	// #ifndef _SERVER

void KRegion::AddObj(int nIdx)
{
	KIndexNode *pNode = NULL;
	
	KObj& anObj = Object[nIdx];
	anObj.m_Node.m_nIndex = nIdx;
	m_ObjList.AddTail(&(anObj.m_Node));

	const unsigned int nMapX = anObj.m_nMapX;
	const unsigned int nMapY = anObj.m_nMapY;

	if (nMapX < REGION_CELL_WIDTH && nMapY < REGION_CELL_HEIGHT)
	{
		const unsigned int uPos = nMapY * REGION_CELL_WIDTH + nMapX;
		if (m_nObjRef[uPos] == 0) m_nObjRef[uPos] = nIdx;
	}
}

void KRegion::RemoveObj(int nIdx)
{
	KObj& anObj = Object[nIdx];
	anObj.m_Node.Remove();

	const unsigned int nMapX = anObj.m_nMapX;
	const unsigned int nMapY = anObj.m_nMapY;
	
	if (nMapX < REGION_CELL_WIDTH && nMapY < REGION_CELL_HEIGHT)
	{
		const unsigned int uPos = nMapY * REGION_CELL_WIDTH + nMapX;
		if (nIdx == m_nObjRef[uPos]) m_nObjRef[uPos] = 0;

		if (anObj.m_nKind == Obj_Kind_House_Entry ||
			anObj.m_nKind == Obj_Kind_Furniture)
		{
			this->ClearObjBarrier(anObj, nMapX, nMapY);
		}
	}
}

DWORD KRegion::GetTrap(int nMapX, int nMapY)
{
#ifdef _SERVER
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_CELL_WIDTH || nMapY >= REGION_CELL_HEIGHT)
	{
		return 0;
	}
	return m_dwTrap[nMapX][nMapY];
#else
	return 0;
#endif
}

BYTE KRegion::GetBarrier(int nMapX, int nMapY, int nDx, int nDy)
{
//	_ASSERT(nMapX >= 0 && nMapX < REGION_CELL_WIDTH && nMapY >= 0 && nMapY < REGION_CELL_HEIGHT);
#ifdef _SERVER	
	const long lInfo = m_Obstacle[nMapX][nMapY];
	long lRet = lInfo & 0x0000000f;

	if (lRet != Obstacle_NULL)
	{
		const long lType = (lInfo >> 4) & 0x0000000f;
		switch(lType)
		{
		case Obstacle_LT:
			if (nDx + nDy > 32)
				lRet = Obstacle_NULL;
			break;
		case Obstacle_RT:
			if (nDx < nDy)
				lRet = Obstacle_NULL;
			break;
		case Obstacle_LB:
			if (nDx > nDy)
				lRet = Obstacle_NULL;
			break;
		case Obstacle_RB:
			if (nDx + nDy < 32)
				lRet = Obstacle_NULL;
			break;
		default:
			break;
		}
		return lRet;
	}
#endif
// 	const unsigned int nPos = nMapY * REGION_CELL_WIDTH + nMapX;
// 	if (m_nNpcRef[nPos] > 0)
// 	{
// 		return Obstacle_Fly;
// 	}
	return Obstacle_NULL;
}

BOOL	KRegion::TestObjBarrier(KObj& aObj, int nMapX, int nMapY)
{
	if ((nMapX < REGION_CELL_WIDTH)&&(nMapY < REGION_CELL_HEIGHT))
	{
		const int nX1 = aObj.m_nObstacleW;
		const int nX2 = -nX1;
		for(int x = nX2 ;x < nX1; x++)
		{
			const int nY1 = aObj.m_nObstacleH;
			const int nY2 = -nY1;
			for(int y = nY2; y < nY1; y++)
			{
				unsigned int nOffSetX = 0;
				unsigned int nOffSetY = 0;
				int nNewMapX = nMapX + (x - y);
				int nNewMapY = nMapY + (x + y)+1;
				if (nNewMapX < 0) 
				{
					nOffSetX = 1;
					nNewMapX += REGION_CELL_WIDTH;
				}
				else if (nNewMapX >= REGION_CELL_WIDTH)
				{
					nOffSetX = 2;
					nNewMapX -= REGION_CELL_WIDTH;
				}
				
				if (nNewMapY < 0) 
				{
					nOffSetY = 1;
					nNewMapY += REGION_CELL_HEIGHT;
				}
				else if (nNewMapY >= REGION_CELL_HEIGHT)
				{
					nOffSetY = 2;
					nNewMapY -= REGION_CELL_HEIGHT;
				}
				
				const int uConnectRegionIdx = OffSetTable[nOffSetX][nOffSetY];
				int nNewRegion = m_nIndex;
				if (uConnectRegionIdx >= 0)
				{
					nNewRegion = m_nConnectRegion[uConnectRegionIdx];
				}
				if (nNewRegion >= 0)
				{		
					//ToDo: Test Npc on this region.
					KIndexNode* pNpc = (KIndexNode*)SubWorld[aObj.m_nSubWorldID].m_Region[nNewRegion].m_NpcList.GetHead();
					for (;pNpc != 0; pNpc = (KIndexNode*)pNpc->GetNext())
					{
						if (Npc[pNpc->m_nIndex].GetMapX() == nNewMapX && Npc[pNpc->m_nIndex].GetMapY() == nNewMapY)
						{
							return FALSE;
						}
					}
#ifdef _SERVER
					int Obstacle = SubWorld[aObj.m_nSubWorldID].m_Region[nNewRegion].m_Obstacle[nNewMapX][nNewMapY] & 0x0000000f;
#else
					int nMpsX, nMpsY;
					SubWorld[0].Map2Mps(nNewRegion, nNewMapX, nNewMapY, 0, 0, &nMpsX, &nMpsY);
					int Obstacle = g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY) & 0x0000000f;
#endif
					if (Obstacle != Obstacle_NULL)
					{
						return FALSE;
					}

					if (y > nY2 && x > nX2)
					{
						nNewMapY = nMapY + (x + y);
						if (nNewMapY < 0) 
						{
							nOffSetY = 1;
							nNewMapY += REGION_CELL_HEIGHT;
						}
						else if (nNewMapY >= REGION_CELL_HEIGHT)
						{
							nOffSetY = 2;
							nNewMapY -= REGION_CELL_HEIGHT;
						}
						else
						{
							nOffSetY = 0;
						}
						
						const int uConnectRegionIdx = OffSetTable[nOffSetX][nOffSetY];
						int nNewRegion = m_nIndex;
						if (uConnectRegionIdx >= 0)
						{
							nNewRegion = m_nConnectRegion[uConnectRegionIdx];
						}
						if (nNewRegion >= 0)
						{
							//ToDo: Test Npc on this region.
							KIndexNode* pNpc = (KIndexNode*)SubWorld[aObj.m_nSubWorldID].m_Region[nNewRegion].m_NpcList.GetHead();
							for (;pNpc != 0; pNpc = (KIndexNode*)pNpc->GetNext())
							{
								if (Npc[pNpc->m_nIndex].GetMapX() == nNewMapX && Npc[pNpc->m_nIndex].GetMapY() == nNewMapY)
								{
									return FALSE;
								}
							}

#ifdef _SERVER
							int Obstacle = SubWorld[aObj.m_nSubWorldID].m_Region[nNewRegion].m_Obstacle[nNewMapX][nNewMapY] & 0x0000000f;
#else
							int nMpsX, nMpsY;
							SubWorld[0].Map2Mps(nNewRegion, nNewMapX, nNewMapY, 0, 0, &nMpsX, &nMpsY);
							int Obstacle = g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY) & 0x0000000f;
#endif
							if (Obstacle != Obstacle_NULL)
							{
								return FALSE;
							}
						}
					}
				}
			}
		}
	}
	return TRUE;
}

void  KRegion::AddObjBarrier(KObj& aObj, int nMapX, int nMapY, Obstacle_Kind kind)
{
	if ((nMapX < REGION_CELL_WIDTH)&&(nMapY < REGION_CELL_HEIGHT))
	{
		if (aObj.m_nObstacleH <= 0 || aObj.m_nObstacleW <=0 )
		{
			// 不设阻挡.			
			return;
		}

		const int nX1 = aObj.m_nObstacleW;
		const int nX2 = -nX1;
		for(int x = nX2 ;x < nX1; x++)
		{
			const int nY1 = aObj.m_nObstacleH;
			const int nY2 = -nY1;
			for(int y = nY2; y < nY1; y++)
			{
				unsigned int nOffSetX = 0;
				unsigned int nOffSetY = 0;
				int nNewMapX = nMapX + (x - y);
				int nNewMapY = nMapY + (x + y)+1;
				if (nNewMapX < 0) 
				{
					nOffSetX = 1;
					nNewMapX += REGION_CELL_WIDTH;
				}
				else if (nNewMapX >= REGION_CELL_WIDTH)
				{
					nOffSetX = 2;
					nNewMapX -= REGION_CELL_WIDTH;
				}
				
				if (nNewMapY < 0) 
				{
					nOffSetY = 1;
					nNewMapY += REGION_CELL_HEIGHT;
				}
				else if (nNewMapY >= REGION_CELL_HEIGHT)
				{
					nOffSetY = 2;
					nNewMapY -= REGION_CELL_HEIGHT;
				}
				
				const int uConnectRegionIdx = OffSetTable[nOffSetX][nOffSetY];
				int nNewRegion = m_nIndex;
				if (uConnectRegionIdx >= 0)
				{
					nNewRegion = m_nConnectRegion[uConnectRegionIdx];
				}
				if (nNewRegion >= 0)
				{
#ifdef _SERVER
					long& nObstacle = SubWorld[aObj.m_nSubWorldID].m_Region[nNewRegion].m_Obstacle[nNewMapX][nNewMapY];					
					nObstacle = (Obstacle_Full << 4) + kind;
#else 
					int nMpsX, nMpsY;
					SubWorld[0].Map2Mps(nNewRegion, nNewMapX, nNewMapY, 0, 0, &nMpsX, &nMpsY);
					g_ScenePlace.AddObstacle(nMpsX, nMpsY, kind);					
#endif
				}

				if (y > nY2 && x > nX2)
				{
					nNewMapY = nMapY + (x + y);
					if (nNewMapY < 0) 
					{
						nOffSetY = 1;
						nNewMapY += REGION_CELL_HEIGHT;
					}
					else if (nNewMapY >= REGION_CELL_HEIGHT)
					{
						nOffSetY = 2;
						nNewMapY -= REGION_CELL_HEIGHT;
					}
					else
					{
						nOffSetY = 0;
					}
					
					const int uConnectRegionIdx = OffSetTable[nOffSetX][nOffSetY];
					int nNewRegion = m_nIndex;
					if (uConnectRegionIdx >= 0)
					{
						nNewRegion = m_nConnectRegion[uConnectRegionIdx];
					}
					if (nNewRegion >= 0)
					{
#ifdef _SERVER
						long& nObstacle = SubWorld[aObj.m_nSubWorldID].m_Region[nNewRegion].m_Obstacle[nNewMapX][nNewMapY];					
						nObstacle = (Obstacle_Full << 4) + kind;
#else 
						int nMpsX, nMpsY;
						SubWorld[0].Map2Mps(nNewRegion, nNewMapX, nNewMapY, 0, 0, &nMpsX, &nMpsY);
						g_ScenePlace.AddObstacle(nMpsX, nMpsY, kind);					
#endif

					}
				}
			}
		}	
	}	
}

void KRegion::ClearObjBarrier(KObj& aObj, int nMapX, int nMapY)
{
	if ((nMapX < REGION_CELL_WIDTH)&&(nMapY < REGION_CELL_HEIGHT))
	{
		if (aObj.m_nObstacleH <= 0 || aObj.m_nObstacleW <=0 )
		{
			// 不设阻挡.			
			return;
		}

		const int nX1 = aObj.m_nObstacleW;
		const int nX2 = -nX1;
		for(int x = nX2 ;x < nX1; x++)
		{
			const int nY1 = aObj.m_nObstacleH;
			const int nY2 = -nY1;
			for(int y = nY2; y < nY1; y++)
			{
				unsigned int nOffSetX = 0;
				unsigned int nOffSetY = 0;
				int nNewMapX = nMapX + (x - y);
				int nNewMapY = nMapY + (x + y)+1;
				if (nNewMapX < 0) 
				{
					nOffSetX = 1;
					nNewMapX += REGION_CELL_WIDTH;
				}
				else if (nNewMapX >= REGION_CELL_WIDTH)
				{
					nOffSetX = 2;
					nNewMapX -= REGION_CELL_WIDTH;
				}
				
				if (nNewMapY < 0) 
				{
					nOffSetY = 1;
					nNewMapY += REGION_CELL_HEIGHT;
				}
				else if (nNewMapY >= REGION_CELL_HEIGHT)
				{
					nOffSetY = 2;
					nNewMapY -= REGION_CELL_HEIGHT;
				}
				
				const int uConnectRegionIdx = OffSetTable[nOffSetX][nOffSetY];
				int nNewRegion = m_nIndex;
				if (uConnectRegionIdx >= 0)
				{
					nNewRegion = m_nConnectRegion[uConnectRegionIdx];
				}
				if (nNewRegion >= 0)
				{
#ifdef _SERVER
					long& nObstacle = SubWorld[aObj.m_nSubWorldID].m_Region[nNewRegion].m_Obstacle[nNewMapX][nNewMapY];					
					nObstacle = Obstacle_NULL;
#else 
					int nMpsX, nMpsY;
					SubWorld[0].Map2Mps(nNewRegion, nNewMapX, nNewMapY, 0, 0, &nMpsX, &nMpsY);
					g_ScenePlace.ClearObstacle(nMpsX, nMpsY);					
#endif
				}

				if (y > nY2 && x > nX2)
				{
					nNewMapY = nMapY + (x + y);
					if (nNewMapY < 0) 
					{
						nOffSetY = 1;
						nNewMapY += REGION_CELL_HEIGHT;
					}
					else if (nNewMapY >= REGION_CELL_HEIGHT)
					{
						nOffSetY = 2;
						nNewMapY -= REGION_CELL_HEIGHT;
					}
					else
					{
						nOffSetY = 0;
					}
					
					const int uConnectRegionIdx = OffSetTable[nOffSetX][nOffSetY];
					int nNewRegion = m_nIndex;
					if (uConnectRegionIdx >= 0)
					{
						nNewRegion = m_nConnectRegion[uConnectRegionIdx];
					}
					if (nNewRegion >= 0)
					{
#ifdef _SERVER
						long& nObstacle = SubWorld[aObj.m_nSubWorldID].m_Region[nNewRegion].m_Obstacle[nNewMapX][nNewMapY];					
						nObstacle = Obstacle_NULL;
#else 
						int nMpsX, nMpsY;
						SubWorld[0].Map2Mps(nNewRegion, nNewMapX, nNewMapY, 0, 0, &nMpsX, &nMpsY);
						g_ScenePlace.ClearObstacle(nMpsX, nMpsY);					
#endif
					}
				}
			}
		}	
	}		
}

//----------------------------------------------------------------------------
//	功能：按 像素点坐标 * 1024 的精度判断某个位置是否障碍
//	参数：nGridX nGirdY ：本region格子坐标
//	参数：nOffX nOffY ：格子内的偏移量(像素点 * 1024 精度)
//	参数：bCheckNpc ：是否判断npc形成的障碍
//	返回值：障碍类型(if 类型 == Obstacle_NULL 无障碍)
//----------------------------------------------------------------------------
BYTE	KRegion::GetBarrierMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc)
{
//	_ASSERT(0 <= nGridX && nGridX < REGION_CELL_WIDTH && 0 <= nGridY && nGridY < REGION_CELL_HEIGHT);
	if (nGridX < 0 || nGridX >= REGION_CELL_WIDTH || nGridY < 0 || nGridY >= REGION_CELL_HEIGHT)
		return Obstacle_Normal;

#ifdef _SERVER
	const long lInfo = m_Obstacle[nGridX][nGridY];
	long lRet = lInfo & 0x0000000f;

	if (lRet != Obstacle_NULL)
	{
		const long lType = (lInfo >> 4) & 0x0000000f;
		switch(lType)
		{
		case Obstacle_LT:
			if (nOffX + nOffY > 32 * 1024)
				lRet = Obstacle_NULL;
			break;
		case Obstacle_RT:
			if (nOffX < nOffY)
				lRet = Obstacle_NULL;
			break;
		case Obstacle_LB:
			if (nOffX > nOffY)
				lRet = Obstacle_NULL;
			break;
		case Obstacle_RB:
			if (nOffX + nOffY < 32 * 1024)
				lRet = Obstacle_NULL;
			break;
		default:
			break;
		}
		return lRet;
	}
#endif
 	if (bCheckNpc)
 	{
 		const unsigned int nPos = nGridY * REGION_CELL_WIDTH + nGridX;
 		if (m_nNpcRef[nPos] > 0)
 		{
 			return Obstacle_Fly;
		}
 	}
	return Obstacle_NULL;
}



BOOL KRegion::AddPlayer(int nIdx)
{
	if (nIdx > 0 && nIdx < MAX_PLAYER)
	{
		_ASSERT(Player[nIdx].m_Node.m_Ref == 0);
		if (Player[nIdx].m_Node.m_Ref == 0)
		{
			m_PlayerList.AddTail(&Player[nIdx].m_Node);
			Player[nIdx].m_Node.AddRef();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KRegion::RemovePlayer(int nIdx)
{
	if (nIdx > 0 && nIdx < MAX_PLAYER)
	{
		if (Player[nIdx].m_Node.m_Ref > 0)
		{
			Player[nIdx].m_Node.Remove();
			Player[nIdx].m_Node.Release();
			return TRUE;
		}
	}
	return FALSE;
}

//-------------------------------------------------------------------------
//	功能：寻找本区域内是否有某个指定 id 的 npc
//-------------------------------------------------------------------------
int		KRegion::SearchNpc(DWORD dwNpcID)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		const int nIndex = pNode->m_nIndex;
		if (nIndex > 0 && nIndex < MAX_NPC)
		{
			if (Npc[nIndex].m_dwID == dwNpcID)
				return nIndex;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	

	return 0;
}


int KRegion::FindObject(unsigned int nMapX,unsigned int nMapY)
{
	if (nMapX < REGION_CELL_WIDTH && nMapY < REGION_CELL_HEIGHT)
	{
		return m_nObjRef[nMapY * REGION_CELL_WIDTH + nMapX];
	}
	return 0;
}

int KRegion::FindObject(int nObjID)
{
	KIndexNode *pNode = NULL;
	
	pNode = (KIndexNode *)m_ObjList.GetHead();
	
	while(pNode)
	{
		if (Object[pNode->m_nIndex].m_nID == nObjID)
		{
			return pNode->m_nIndex;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return 0;
}

int KRegion::FindEquip(int nMapX, int nMapY)
{

	const int nFindIndex = m_nObjRef[nMapY * REGION_CELL_WIDTH + nMapX];
	if ( nFindIndex != 0)
	{
		if (Obj_Kind_Item == Object[nFindIndex].m_nKind)
		{
			return nFindIndex;
		}
	}
	return 0;
	
}

#ifdef	_SERVER
void KRegion::SendSyncData(int nClient, int nNpcIndex)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		Npc[pNode->m_nIndex].SendSyncData( nClient, nNpcIndex );
		pNode = (KIndexNode *)pNode->GetNext();
	}
	
	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		Npc[Player[pNode->m_nIndex].GetPlayerIndex()].SendSyncData( nClient, nNpcIndex );
		pNode = (KIndexNode *)pNode->GetNext();
	}
}

void KRegion::BroadCast(const void* pBuffer, DWORD dwSize, int &nMaxCount, int nOX, int nOY)
{
#define	MAX_SYNC_RANGE	32 * 32
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode && nMaxCount > 0)
	{
		_ASSERT(pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER);
		const int nPlayerIndex = pNode->m_nIndex;
//		if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER) // 增加边界值检查 lixuewu			
		{
			const KPlayer& aPlayer = Player[nPlayerIndex];
			const int nNpcIndex = aPlayer.m_nIndex;
			const int nDX = Npc[nNpcIndex].GetMapX() - nOX;
			const int nDY = Npc[nNpcIndex].GetMapY() - nOY;
			if (aPlayer.m_nNetConnectIdx >= 0 
//					&& (nDX * nDX + nDY * nDY) <= MAX_SYNC_RANGE
				&& aPlayer.m_bSleepMode == FALSE)
			{
				if (g_pServer != NULL)
					g_pServer->PackDataToClient(aPlayer.m_nNetConnectIdx, (BYTE*)pBuffer, dwSize);
				nMaxCount--;
			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
}

//---------------------------------------------------------------------
// 查找该Region中NpcID为dwId的Player索引
//---------------------------------------------------------------------
int KRegion::FindPlayer(DWORD dwId)
{
	KIndexNode *pNode = NULL;
	int	nRet = -1;

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		const int nPlayerIndex = pNode->m_nIndex;
		if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
		{
			const int nIndex = Player[nPlayerIndex].m_nIndex;
			if (nIndex > 0 && nIndex < MAX_NPC)
			{
				if (Npc[nIndex].m_dwID == dwId)
				{
					nRet = nPlayerIndex;
					break;
				}
			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
	return nRet;
}

int KRegion::FindPlayer(FSGUID& guid)
{
	KIndexNode *pNode = NULL;
	int	nRet = -1;

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		const int nPlayerIndex = pNode->m_nIndex;
		if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
		{
			if (Player[nPlayerIndex].GetGUID() == guid)
			{
				nRet = nPlayerIndex;
				break;
			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}

	return nRet;
}

//---------------------------------------------------------------------
// 查找该Region中是否有索引为 nPlayerIdx 的玩家
//---------------------------------------------------------------------
BOOL	KRegion::CheckPlayerIn(int nPlayerIdx)
{
	if (nPlayerIdx <= 0)
		return FALSE;

	KIndexNode *pNode = NULL;
	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		if (pNode->m_nIndex == nPlayerIdx)
			return TRUE;
		pNode = (KIndexNode *)pNode->GetNext();
	}
	return FALSE;
}

//--> Rocker 2004/10/09
void	KRegion::SetTrap(DWORD nTrapId, int nMapX, int nMapY)
{
	if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_CELL_WIDTH || nMapY >= REGION_CELL_HEIGHT)
	{
		return;
	}
	m_dwTrap[nMapX][nMapY] = nTrapId;
}
//<-- End

void KRegion::RemoveAllSpawnNpc()
{
	if (m_nActive <= 0) 
	{
		KIndexNode* pTempNode = NULL;
		KIndexNode * pNode = (KIndexNode *)m_NpcList.GetHead();
		while(pNode)
		{
			pTempNode = pNode;
			pNode = (KIndexNode *)pNode->GetNext();
			if (pTempNode->m_nIndex)
			{
				KSpawnPoint_Runtime_Info* pSpawnInfo = Npc[pTempNode->m_nIndex].GetSpawnInfo();				
				if (pSpawnInfo != NULL)
				{
					pSpawnInfo->m_uCount--;
					RemoveNpc(pTempNode->m_nIndex);
					NpcSet.Remove(pTempNode->m_nIndex);
				}
                else
                {
                    WorldSpawner_RunTimeInfo* pWorldSpawnInfo = Npc[pTempNode->m_nIndex].GetWorldSpawnInfo();
                    if (pWorldSpawnInfo != NULL)
                    {
                        pWorldSpawnInfo->uMobCount--;
                        RemoveNpc(pTempNode->m_nIndex);
                        NpcSet.Remove(pTempNode->m_nIndex);
                    }
                }
			}
		}
	}
}

#endif


void* KRegion::GetObjNode(int nIdx)
{
	KIndexNode *pNode = NULL;

	pNode = (KIndexNode *)m_ObjList.GetHead();

	while(pNode)
	{
		if (pNode->m_nIndex == nIdx)
		{
			break;
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}
	return pNode;
}

void KRegion::Close()		// 清除Region中的几个链表（所指向的内容没有被清除）
{
	KIndexNode* pNode = NULL;
	KIndexNode* pTempNode = NULL;

	ZeroMemory(m_nNpcRef, sizeof(m_nNpcRef));
	ZeroMemory(m_nObjRef, sizeof(m_nObjRef));

	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
#ifndef _SERVER	
		Npc[pTempNode->m_nIndex].DelInfo();
#endif
		Npc[pTempNode->m_nIndex].m_RegionIndex = -1;
		RemoveNpc(pTempNode->m_nIndex);
#ifdef _SERVER
		NpcSet.Remove(pTempNode->m_nIndex);
#endif
	}

#ifndef _SERVER
	pNode = (KIndexNode *)m_MissleList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
		MissleSet.Remove(pTempNode->m_nIndex);
		Missle[pTempNode->m_nIndex].m_nRegionId = -1;
		pTempNode->Remove();
		pTempNode->Release();
	}
#endif

	// 同时清除 clientonly 类型的 obj ---- zroc add
	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
#ifndef _SERVER
		Object[pTempNode->m_nIndex].Remove(FALSE);
#endif
		ObjSet.Remove(pTempNode->m_nIndex); // TODO:
		Object[pTempNode->m_nIndex].m_nRegionIdx = -1;
	}

	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
		pTempNode->Remove();
		pTempNode->Release();
	}

	m_RegionID	= -1;
	m_nIndex	= -1;
	m_nActive	= 0;
	memset(m_nConnectRegion, -1, sizeof(m_nConnectRegion));
	memset(m_nConRegionID, -1, sizeof(m_nConRegionID));

#ifdef _SERVER
	m_SpawnPointList.Release();
#endif
}


//************************************************************************
// 增加Npc阻挡计数
// nMapX,nMapY 在Region中的格子坐标
// nIndex 在Npc数组中的位置
// 为了简化只进行一次传递,对象障碍大小必须小于REGION_CELL_WIDTH*REGION_CELL_HEIGHT
//************************************************************************
BOOL KRegion::AddNpcRef(int nIndex)
{	
	KNpc& aNpc = Npc[nIndex];
	unsigned int nMapX=aNpc.GetMapX();
	unsigned int nMapY=aNpc.GetMapY();
	if ((nMapX < REGION_CELL_WIDTH)&&(nMapY < REGION_CELL_HEIGHT))
	{
		if (aNpc.m_bRegionRefAdded == TRUE || (aNpc.m_Kind != kind_building  && aNpc.m_Kind != kind_normal))
		{
			return FALSE;
		}
        const unsigned int uPos = nMapX + nMapY * REGION_CELL_WIDTH;
        if (m_nNpcRef[uPos] != 255)
        {
    		aNpc.m_bRegionRefAdded = TRUE;
            m_nNpcRef[uPos]++;
        }
        return TRUE;
	}
	return FALSE;
}


int KRegion::GetNpcRef(int nMapX, int nMapY)
{
	return m_nNpcRef[nMapY * REGION_CELL_WIDTH + nMapX];
}

BOOL KRegion::DecNpcRef(int nIndex)
{
	KNpc& aNpc = Npc[nIndex];
	unsigned int nMapX=aNpc.GetMapX();
	unsigned int nMapY=aNpc.GetMapY();
	if ((nMapX < REGION_CELL_WIDTH)&&(nMapY < REGION_CELL_HEIGHT))
	{
		if (aNpc.m_bRegionRefAdded == FALSE || (aNpc.m_Kind != kind_building  && aNpc.m_Kind != kind_normal))
		{
			return FALSE;
		}

        const unsigned int uPos = nMapX + nMapY * REGION_CELL_WIDTH;
        if (m_nNpcRef[uPos] > 0)
        {
    		aNpc.m_bRegionRefAdded = FALSE;
            m_nNpcRef[uPos]--;
        }
        return TRUE;
	}
	return FALSE;
}


int KRegion::GetObj(unsigned int nMapX,unsigned int nMapY)
{
	if (nMapX < REGION_CELL_WIDTH && nMapY < REGION_CELL_HEIGHT)
	{
		return m_nObjRef[nMapY * REGION_CELL_WIDTH + nMapX];
	}
	return 0;
}

//************************************************************************
// 检查指定位置的制定范围内是否有障碍
// uMapX,uMapY 中心位置
// uW,uH 检查范围,不支持跨越2个以上区域
//************************************************************************
BOOL KRegion::CanBuild(unsigned int nSubWorldIndex,unsigned int nMapX, unsigned int nMapY, unsigned int uW,unsigned int uH,BOOL bCheckTerrain)
{
	if ((nMapX < REGION_CELL_WIDTH)&&(nMapY < REGION_CELL_HEIGHT))
	{
		const int nX1 = uW;
		const int nX2 = -nX1;
		for(int x = nX2 ;x < nX1; x++)
		{
			const int nY1 = uH;
			const int nY2 = -nY1;
			for(int y = nY2; y < nY1; y++)
			{
				unsigned int nOffSetX = 0;
				unsigned int nOffSetY = 0;
				int nNewMapX = nMapX + (x - y);
				int nNewMapY = nMapY + (x + y)+1;
				if (nNewMapX < 0) 
				{
					nOffSetX = 1;
					nNewMapX += REGION_CELL_WIDTH;
				}
				else if (nNewMapX >= REGION_CELL_WIDTH)
				{
					nOffSetX = 2;
					nNewMapX -= REGION_CELL_WIDTH;
				}
				
				if (nNewMapY < 0) 
				{
					nOffSetY = 1;
					nNewMapY += REGION_CELL_HEIGHT;
				}
				else if (nNewMapY >= REGION_CELL_HEIGHT)
				{
					nOffSetY = 2;
					nNewMapY -= REGION_CELL_HEIGHT;
				}
				
				const int uConnectRegionIdx = OffSetTable[nOffSetX][nOffSetY];
				int nNewRegion = m_nIndex;
				if (uConnectRegionIdx >= 0)
				{
					nNewRegion = m_nConnectRegion[uConnectRegionIdx];
				}
				if (nNewRegion >= 0)
				{
					BYTE *pBuffer = SubWorld[nSubWorldIndex].m_Region[nNewRegion].m_nNpcRef;
					const unsigned int uPos = nNewMapY * REGION_CELL_WIDTH + nNewMapX;
					_ASSERT(uPos <= REGION_CELL_WIDTH * REGION_CELL_HEIGHT);
#ifdef _SERVER
					int Obstacle = SubWorld[nSubWorldIndex].m_Region[nNewRegion].m_Obstacle[nNewMapX][nNewMapY] & 0x0000000f;
#else
					int nMpsX, nMpsY;
					SubWorld[0].Map2Mps(nNewRegion, nNewMapX, nNewMapY, 0, 0, &nMpsX, &nMpsY);
					int Obstacle = g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY) & 0x0000000f;
#endif
				    BOOL bTerrain = bCheckTerrain?(Obstacle != Obstacle_JumpFly):(Obstacle == Obstacle_Normal);
					if ((pBuffer[uPos] != 0) || bTerrain)
						return FALSE;

					if (y > nY2 && x > nX2)
					{
						nNewMapY = nMapY + (x + y);
						if (nNewMapY < 0) 
						{
							nOffSetY = 1;
							nNewMapY += REGION_CELL_HEIGHT;
						}
						else if (nNewMapY >= REGION_CELL_HEIGHT)
						{
							nOffSetY = 2;
							nNewMapY -= REGION_CELL_HEIGHT;
						}
						else
						{
							nOffSetY = 0;
						}
						
						const int uConnectRegionIdx = OffSetTable[nOffSetX][nOffSetY];
						int nNewRegion = m_nIndex;
						if (uConnectRegionIdx >= 0)
						{
							nNewRegion = m_nConnectRegion[uConnectRegionIdx];
						}
						if (nNewRegion >= 0)
						{
							BYTE *pBuffer = SubWorld[nSubWorldIndex].m_Region[nNewRegion].m_nNpcRef;					
							const unsigned int uPos = nNewMapY * REGION_CELL_WIDTH + nNewMapX;
							_ASSERT(uPos <= REGION_CELL_WIDTH * REGION_CELL_HEIGHT);
#ifdef _SERVER
							int Obstacle = SubWorld[nSubWorldIndex].m_Region[nNewRegion].m_Obstacle[nNewMapX][nNewMapY] & 0x0000000f;
#else
							int nMpsX, nMpsY;
							SubWorld[0].Map2Mps(nNewRegion, nNewMapX, nNewMapY, 0, 0, &nMpsX, &nMpsY);
							int Obstacle = g_ScenePlace.GetObstacleInfo(nMpsX, nMpsY) & 0x0000000f;
#endif
							BOOL bTerrain = bCheckTerrain?(Obstacle != Obstacle_JumpFly):(Obstacle == Obstacle_Normal);
							if ((pBuffer[uPos] != 0) || bTerrain) return FALSE;
						}
					}
				}
			}
		}
	}
	return TRUE;
}

#ifdef _SERVER
bool KRegion::CopyInstance(KRegion* pRegion, int worldIndex) const
{
	if (pRegion != NULL)
	{
		pRegion->m_nIndex = m_nIndex;
		pRegion->m_RegionID = m_RegionID;
		pRegion->m_nActive = 0;
		pRegion->m_nNpcSyncCounter = 0;
		pRegion->m_nObjSyncCounter = 0;
		ZeroMemory(pRegion->m_nNpcRef, sizeof(pRegion->m_nNpcRef));
		ZeroMemory(pRegion->m_nObjRef, sizeof(pRegion->m_nObjRef));
		memcpy((void*)pRegion->m_nConnectRegion, (void*)m_nConnectRegion, sizeof(m_nConnectRegion));
		memcpy((void*)pRegion->m_nConRegionID, (void*)m_nConRegionID, sizeof(m_nConRegionID));
		pRegion->m_nRegionX = m_nRegionX;
		pRegion->m_nRegionY = m_nRegionY;
		memcpy((void*)(pRegion->m_Obstacle), (void*)m_Obstacle, sizeof(m_Obstacle));
		memcpy((void*)(pRegion->m_dwTrap), (void*)m_dwTrap, sizeof(m_dwTrap));
		if (!m_SpawnPointList.CopyInstance(pRegion->m_SpawnPointList, worldIndex))
			return false;

		return true;
	}

	return false;
}
#endif

#ifdef _SERVER

KSpawnPointList::KSpawnPointList(KRegion* pRegion):m_uCount(0),
											m_pData(NULL),
											m_pInfo(NULL),
//											m_pRegion(pRegion),
											m_uSubworld()
{
}

KSpawnPointList::~KSpawnPointList()
{
	if (m_pData != NULL)
		delete[] m_pData;
	if (m_pInfo != NULL)
		delete[] m_pInfo;
}



BOOL KSpawnPointList::LoadSpawnPoints(int nSubWorld ,KPakFile* pFile, unsigned int dwDataSize)
{
	if (!pFile || dwDataSize < sizeof(KNpcFileHead))
		return FALSE;


	KNpcFileHead	sNpcFileHead;
	KSPNpc			sNpcCell;

	int nNpcIndex	= 0; 
	
	pFile->Read(&sNpcFileHead, sizeof(KNpcFileHead));
	
	m_pData = new KSpawnPoint[sNpcFileHead.uNumNpc];
	if (m_pData != NULL)
	{
		m_pInfo = new KSpawnPoint_Runtime_Info[sNpcFileHead.uNumNpc];
		if (m_pInfo != NULL)
		{
			m_uCount = sNpcFileHead.uNumNpc;
			memset(m_pData, 0, sizeof(KSpawnPoint)* m_uCount);
			memset(m_pInfo, 0, sizeof(KSpawnPoint_Runtime_Info)* m_uCount);


			for (int i = 0; i < sNpcFileHead.uNumNpc; i++)
			{
				pFile->Read(&sNpcCell, sizeof(KSPNpc));
				KSpawnPoint& aSpawnPoint = m_pData[i];
				aSpawnPoint.m_nPositionX = sNpcCell.nPositionX;
				aSpawnPoint.m_nPositionY = sNpcCell.nPositionY;
				aSpawnPoint.m_uSpawnType = MAKELONG(sNpcCell.nLevel, sNpcCell.nTemplateID);
				aSpawnPoint.m_uCountCount = sNpcCell.nCount;
                aSpawnPoint.m_nLvlParam = sNpcCell.cLvlParam;
				aSpawnPoint.m_uDir      = sNpcCell.nDir;
				unsigned int nNpcTemplateId = sNpcCell.nTemplateID;
				KNpcTemplate *pNpcTemplate = g_pNpcTemplate[nNpcTemplateId][0];
				if (pNpcTemplate == NULL)
				{
					pNpcTemplate = new KNpcTemplate;
					pNpcTemplate->InitNpcBaseData(sNpcCell.nTemplateID);
					pNpcTemplate->m_NpcSettingIdx = sNpcCell.nTemplateID;
					pNpcTemplate->m_bHaveLoadedFromTemplate = TRUE;
					g_pNpcTemplate[nNpcTemplateId][0] = pNpcTemplate;
				}

				aSpawnPoint.m_uSpawnTime = pNpcTemplate->m_ReviveFrame;
				aSpawnPoint.m_uSpawnRange = sNpcCell.nRadius;
				aSpawnPoint.m_bManaged	= sNpcCell.bRecycle;
				if (!aSpawnPoint.m_bManaged)
				{
					nNpcIndex = NpcSet.Add(nSubWorld, &sNpcCell);
                    if (nNpcIndex > 0)
                    {
                        Npc[nNpcIndex].m_UnaryAttrMgr.Set(nuai_dir,sNpcCell.nDir);
                    }
				}
			}
				
			m_uSubworld = nSubWorld;
			return TRUE;
			
		}
		else
		{
			delete[] m_pData;
			m_pData = NULL;
		}
	}
	
	return FALSE;	
}

void KSpawnPointList::Release()
{
	m_uCount = 0;
	if (m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}
	if (m_pInfo)
	{
		delete[] m_pInfo;
		m_pInfo = NULL;
	}
	m_uSubworld = INVALID_WORLD_INDEX;
}

bool KSpawnPointList::CopyInstance(KSpawnPointList& list, int worldIndex) const
{
	list.m_uCount = m_uCount;
	list.m_uSubworld = m_uSubworld;
	list.m_pData = NULL;
	list.m_pInfo = NULL;

	if (m_uCount > 0)
	{	
		list.m_pData = new KSpawnPoint[list.m_uCount];
		list.m_pInfo = new KSpawnPoint_Runtime_Info[list.m_uCount];		
		if (list.m_pData == NULL || list.m_pInfo == NULL)
		{
			if (list.m_pData)
			{
				delete[] list.m_pData;
				list.m_pData = NULL;
			}
			if (list.m_pInfo)
			{
				delete[] list.m_pInfo;
				list.m_pInfo = NULL;
			}

			return false;
		}
		
		memcpy(list.m_pData, m_pData, list.m_uCount * sizeof(KSpawnPoint));
		memset(list.m_pInfo, 0, list.m_uCount * sizeof(KSpawnPoint_Runtime_Info));
		
		for (int i = 0; i < list.m_uCount; i++)
		{
			KSpawnPoint& spawnPoint = list.m_pData[i];
			if (!spawnPoint.m_bManaged)
			{
				KSPNpc npcInfo;
				npcInfo.nPositionX = spawnPoint.m_nPositionX;
				npcInfo.nPositionY = spawnPoint.m_nPositionY;
				npcInfo.nTemplateID = HIWORD(spawnPoint.m_uSpawnType);
				npcInfo.nLevel = LOWORD(spawnPoint.m_uSpawnType);					
				
				int npcIndex = NpcSet.Add(worldIndex, &npcInfo);
				if (npcIndex > 0)
				{
					Npc[npcIndex].m_UnaryAttrMgr.Set(nuai_dir, spawnPoint.m_uDir);
				}
			}
		}
	}
	
	return true;
}

void KSpawnPointList::Active()
{
	for(unsigned int i = 0; i < m_uCount; i++ )
	{
		const KSpawnPoint& aSpawnPoint = m_pData[i];
		if (aSpawnPoint.m_bManaged)
		{
			KSpawnPoint_Runtime_Info& aInfo =  m_pInfo[i];
			unsigned int uShouldSpawn = aSpawnPoint.m_uCountCount - aInfo.m_uCount;
			if (uShouldSpawn > 0)
			{
				if(aInfo.m_uNextSpawnTime == 1)
				{
					aInfo.m_uNextSpawnTime = SubWorld[m_uSubworld].m_dwCurrentTime + aSpawnPoint.m_uSpawnTime;
				}
				else if(aInfo.m_uNextSpawnTime <= SubWorld[m_uSubworld].m_dwCurrentTime)
				{
					aInfo.m_uNextSpawnTime = 1;
					for ( unsigned int n = 0; n < uShouldSpawn ; n ++)
					{
                        int range = g_Random(aSpawnPoint.m_uSpawnRange);
                        int offsety = g_Random(range);
                        int offsetx = qsqrt((range * range) - (offsety * offsety));
                        if(g_RandPercent(50))
                        {
                            offsety = -offsety;
                        }
                        if(g_RandPercent(50))
                        {
                            offsetx = -offsetx;
                        }
                    	int nNpcSettingIdx = (short)HIWORD(aSpawnPoint.m_uSpawnType);
	                    int nLevel = LOWORD(aSpawnPoint.m_uSpawnType) + g_Random(aSpawnPoint.m_nLvlParam) ;
						int nNpc = NpcSet.Add(MAKELONG(nLevel, nNpcSettingIdx), m_uSubworld, aSpawnPoint.m_nPositionX + offsetx * REGION_CELL_WIDTH, aSpawnPoint.m_nPositionY + offsety * REGION_CELL_HEIGHT);
						if (nNpc > 0)
						{
                            Npc[nNpc].m_UnaryAttrMgr.Set(nuai_dir, aSpawnPoint.m_uDir);
							aInfo.m_uCount++;
							Npc[nNpc].m_pSpawnInfo = &aInfo;
						}
						else
						{
							//记录日志：刷怪失败
							if (g_pLogSystem)
							{
								char spawnInfo[256] = { 0 };
								snprintf(spawnInfo, sizeof(spawnInfo), "KSpawnPointList::Active World=%u,NpcId=%d,Level=%d,PosX=%u,PosY=%u",
									m_uSubworld,
									nNpcSettingIdx,
									nLevel,		
									aSpawnPoint.m_nPositionX,
									aSpawnPoint.m_nPositionY);
								spawnInfo[sizeof(spawnInfo) - 1] = 0;
								g_pLogSystem->SysDbgLog(spawnInfo, strlen(spawnInfo), sys_dbg_log_event_spawn_failure);
							}

							KSubWorldSet::s_bNeedBlanceSpawn = TRUE;
							return;
						}
					}
				}
			}
		}
	}
}

#endif

#ifdef _SERVER
void KRegion::Release()
{
	KIndexNode* pNode = NULL;
	KIndexNode* pTempNode = NULL;

	//m_NpcList
	pNode = (KIndexNode *)m_NpcList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode*)pNode->GetNext();
		int npcIndex = pTempNode->m_nIndex;
		if (IsValidNpc(npcIndex))
		{
			KNpc& npc = Npc[npcIndex];			
			if (npc.IsPlayer())
			{
				int playerIndex = npc.GetPlayerIdx();
				if (IsValidPlayer(playerIndex))
				{
					if (npc.m_Doing == do_revive || npc.m_Doing == do_death)
					{
						Player[playerIndex].Revive(0);
					}
					
					if (FALSE == Player[playerIndex].TransferToRememberPos())
					{
						Player[playerIndex].TransferToRevivePos();
					}
				}
			}
			else
			{
				npc.m_RegionIndex = -1;
				RemoveNpc(npcIndex);
				NpcSet.Remove(npcIndex);
			}
		}
	}

	//m_ObjList
	pNode = (KIndexNode *)m_ObjList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode*)pNode->GetNext();
		int objectIndex = pTempNode->m_nIndex;
		if (objectIndex >= 0)
		{
			Object[objectIndex].m_nRegionIdx = -1;
			ObjSet.Remove(pTempNode->m_nIndex);
		}
	}

	//m_PlayerList
	pNode = (KIndexNode *)m_PlayerList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode *)pNode->GetNext();
		pTempNode->Remove();
		pTempNode->Release();
	}
	
	//m_SpawnPointList
	m_SpawnPointList.Release();
}
#endif

#ifdef _SERVER
void KRegion::KickAllPlayer()
{
	KIndexNode* pTempNode = NULL;
	KIndexNode* pNode = (KIndexNode*)m_PlayerList.GetHead();
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KIndexNode*)pNode->GetNext();
		int playerIndex = pTempNode->m_nIndex;
		if (IsValidPlayer(playerIndex))
		{
			int npcIndex = Player[playerIndex].GetNpcIndex();
			if (IsValidNpc(npcIndex) && Npc[npcIndex].IsPlayer() && (Npc[npcIndex].m_Doing == do_revive || Npc[npcIndex].m_Doing == do_death))
			{
				Player[playerIndex].Revive(0);
			}
			
			if (FALSE == Player[playerIndex].TransferToRememberPos())
			{
				Player[playerIndex].TransferToRevivePos();
			}
		}
	}
}
#endif