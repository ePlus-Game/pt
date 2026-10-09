//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:3:12   14:29
//      File_base        : MapObstacleMgr
//      File_ext         : cpp
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#ifdef _AUTO_ROBOT

#include "GlobalDef.h"
#include "MapObstacleMgr.h"
#include "GameDataDef.h"
#include "KSubWorldSet.h"
#include "KSubWorld.h"

#define		OBSTACLE_FILE_PREFIX	"%s\\obstacle.bin"

MapObstacleMgr& MapObstacleMgr::Singleton()
{
	static MapObstacleMgr mgr;

	return mgr;
}

MapObstacleMgr::~MapObstacleMgr()
{
	MapId2Obstacle::iterator itObstacle;
	MapId2Obstacle::iterator itObsEnd = m_MapId2Obstacle.end();

	for(itObstacle = m_MapId2Obstacle.begin(); itObstacle != itObsEnd; ++itObstacle)
	{
		if(itObstacle->second)
			delete [] itObstacle->second;
	}
}

const char* MapObstacleMgr::LoadObstacle(int nMapId)
{
	int	nSubWorldIdx = g_SubWorldSet.SearchWorld(nMapId);

	if(-1 == nSubWorldIdx)
		return NULL;

	char	szObsFilePath[MAX_PATH];
	snprintf(szObsFilePath, 
		sizeof(szObsFilePath), 
		OBSTACLE_FILE_PREFIX, 
		SubWorld[nSubWorldIdx].m_szMapPath
		);

	KPakFile	obsPakFile;

	if( !obsPakFile.Open(szObsFilePath) )
		return NULL;

	int	nSize = obsPakFile.Size();
	char	*pObstacleBuf = new char[nSize + sizeof(ObstacleHeader) - sizeof(Rect)];
	ObstacleHeader *pHeader = (ObstacleHeader*)pObstacleBuf;

	int nReadSize = obsPakFile.Read(&pHeader->rc, nSize);

	if(nReadSize != nSize)
	{
		_ASSERT(false);
		delete [] pObstacleBuf;
		pObstacleBuf = NULL;
	}
	else
	{
		pHeader->widthInRegion = pHeader->rc.endRegionX - pHeader->rc.startRegionX + 1;
		pHeader->heightInRegion = pHeader->rc.endRegionY - pHeader->rc.startRegionY + 1;
		pHeader->widthInCell = pHeader->widthInRegion * REGION_CELL_WIDTH;
		pHeader->heightInCell = pHeader->heightInRegion * REGION_CELL_HEIGHT;
		pHeader->startCellX = pHeader->rc.startRegionX * REGION_CELL_WIDTH;
		pHeader->startCellY = pHeader->rc.startRegionY * REGION_CELL_HEIGHT;
		pHeader->endCellX = pHeader->rc.endRegionX * REGION_CELL_WIDTH;
		pHeader->endCellY = pHeader->rc.endRegionY * REGION_CELL_HEIGHT;
		m_MapId2Obstacle.insert( MapId2Obstacle::value_type(nMapId, pObstacleBuf) );
	}

	obsPakFile.Close();
	return pObstacleBuf;
}

const char*	MapObstacleMgr::GetMapObstacle(int nMapId)
{
	if( !IsObstacleLoad(nMapId) )
	{
		m_MapId2LoadFlag.insert( MapId2LoadFlag::value_type(nMapId, true) );
		return LoadObstacle(nMapId);
	}
	else
	{
		MapId2Obstacle::const_iterator itObstacle = m_MapId2Obstacle.find(nMapId);
		
		if( itObstacle == m_MapId2Obstacle.end() )
			return NULL;
		else
			return itObstacle->second;
	}
}

bool MapObstacleMgr::IsCordinateInMap(int nMapId, int nCellX, int nCellY)
{
	const char *pMapObstacle = GetMapObstacle(nMapId);

	if(pMapObstacle)
	{
		ObstacleHeader	*pHeader = (ObstacleHeader*)pMapObstacle;

		return (nCellX >= pHeader->startCellX && nCellX <= pHeader->endCellX) &&
			(nCellY >= pHeader->startCellY && nCellY <= pHeader->endCellY);
	}
	else
		return false;
}

bool MapObstacleMgr::GetCellPos(int nMapId, float fXPosPercent, float fYPosPercent, int &nCellX, int &nCellY)
{
	const char *pMapObstacle = GetMapObstacle(nMapId);

	if(NULL == pMapObstacle)
		return false;

	const ObstacleHeader *pHeader = (ObstacleHeader*)pMapObstacle;
	
	nCellX = pHeader->startCellX + pHeader->widthInCell * fXPosPercent;
	nCellY = pHeader->startCellY + pHeader->heightInCell * fYPosPercent;

	return true;
}

#endif	// #ifdef _AUTO_ROBOT
