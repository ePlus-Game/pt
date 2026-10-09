//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:3:12   14:27
//      File_base        : MapObstacleMgr
//      File_ext         : h
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
#ifndef _MapObstacleMgr_h
#define _MapObstacleMgr_h
#ifdef _AUTO_ROBOT

#include <map>
#include "AutoRobotComDef.h"

using namespace std;

class MapObstacleMgr
{
public:
	static MapObstacleMgr& Singleton();
	~MapObstacleMgr();

	const char*	GetMapObstacle(int nMapId);
	unsigned char	GetCellObstacle(const char *pMapObstacle, int nCellX, int nCellY);
	bool	IsCordinateInMap(int nMapId, int nCellX, int nCellY);
	bool	GetCellPos(int nMapId, float fXPosPercent, float fYPosPercent, int &nCellX, int &nCellY);

private:
	MapObstacleMgr() {};

	const char*	LoadObstacle(int nMapId);
	bool	IsObstacleLoad(int nMapId) const;

private:
	typedef struct _Rect
	{
		int	startRegionX;
		int	startRegionY;
		int	endRegionX;
		int	endRegionY;

	} Rect;

	typedef struct _ObstacleHeader
	{
		int	widthInRegion;
		int	heightInRegion;
		int	widthInCell;
		int	heightInCell;
		int	startCellX;
		int	startCellY;
		int endCellX;
		int endCellY;
		Rect	rc;

	} ObstacleHeader;

	typedef map<int, char*>	MapId2Obstacle;
	typedef map<int, bool>	MapId2LoadFlag;

private:
	MapId2Obstacle	m_MapId2Obstacle;
	MapId2LoadFlag	m_MapId2LoadFlag;
};

inline bool MapObstacleMgr::IsObstacleLoad(int nMapId) const
{
	return m_MapId2LoadFlag.find(nMapId) != m_MapId2LoadFlag.end();
}

inline unsigned char MapObstacleMgr::GetCellObstacle(const char *pMapObstacle, int nCellX, int nCellY)
{
	ObstacleHeader	*pHeader = (ObstacleHeader*)pMapObstacle;
	
	if(nCellX < pHeader->startCellX || nCellX > pHeader->endCellX)
		return INVALID_CELL_OBSTACLE;

	if(nCellY < pHeader->startCellY || nCellY > pHeader->endCellY)
		return INVALID_CELL_OBSTACLE;

	char	*pObstacle = (char*)pHeader + sizeof(ObstacleHeader);
	int		nCellIdx = (nCellY - pHeader->startCellY) * pHeader->widthInCell + nCellX - pHeader->startCellX;

	return	pObstacle[nCellIdx];
}

#endif	// #ifdef _AUTO_ROBOT
#endif // #ifndef _MapObstacleMgr_h
