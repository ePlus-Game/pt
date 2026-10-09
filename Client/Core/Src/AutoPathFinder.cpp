//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:3:12   13:47
//      File_base        : AutoPathFinder
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

#include "AutoPathFinder.h"
#include "KPlayer.h"
#include "KSubWorld.h"
#include "MapObstacleMgr.h"
#include "AutoRobotComDef.h"
#include "AutoDialogNpc.h"
// debug
#ifdef _DEBUG
#include <time.h>
#endif
// debug

// 仅用于同一条直线上的相邻两个格子(以1*1个格子为单位搜索)
static int GetUnarySearchWalkCost(const char *pMapObstacle, 
	const Coordinate &srcPos, 
	const Coordinate &dstPos
	)
{
	MapObstacleMgr	&mgr = MapObstacleMgr::Singleton();
	unsigned char obstacle = mgr.GetCellObstacle(pMapObstacle, dstPos.x, dstPos.y);

	if(obstacle != 0)
		return WALKCOST_NOWAY;

	if(srcPos.x == dstPos.x || srcPos.y == dstPos.y)
		return WALKCOST_IN_NEARCELL;

	unsigned char obsHorizontal = INVALID_CELL_OBSTACLE;
	unsigned char obsVertiacl = INVALID_CELL_OBSTACLE;

	if(srcPos.x - 1 == dstPos.x)
	{
		if(srcPos.y - 1 == dstPos.y)
		{
			obsHorizontal = mgr.GetCellObstacle(pMapObstacle, srcPos.x - 1, srcPos.y);
			obsVertiacl = mgr.GetCellObstacle(pMapObstacle, srcPos.x, srcPos.y - 1);
		}
		else if(srcPos.y + 1 == dstPos.y)
		{
			obsHorizontal = mgr.GetCellObstacle(pMapObstacle, srcPos.x - 1, srcPos.y);
			obsVertiacl = mgr.GetCellObstacle(pMapObstacle, srcPos.x, srcPos.y + 1);
		}
	}
	else if(srcPos.x + 1 == dstPos.x)
	{
		if(srcPos.y - 1 == dstPos.y)
		{
			obsHorizontal = mgr.GetCellObstacle(pMapObstacle, srcPos.x + 1, srcPos.y);
			obsVertiacl = mgr.GetCellObstacle(pMapObstacle, srcPos.x, srcPos.y - 1);
		}
		else if(srcPos.y + 1 == dstPos.y)
		{
			obsHorizontal = mgr.GetCellObstacle(pMapObstacle, srcPos.x + 1, srcPos.y);
			obsVertiacl = mgr.GetCellObstacle(pMapObstacle, srcPos.x, srcPos.y + 1);
		}
	}
	else
	{
		_ASSERT(false);
		return WALKCOST_NOWAY;
	}
	

	if( (obsHorizontal != 0) || (obsVertiacl != 0) )
		return WALKCOST_NOWAY;
	else
		return WALKCOST_IN_DIAGONAL;
}

unsigned char GetObstacleInBinarySearch(const char *pMapObstacle, int x, int y)
{
	unsigned char obstacle = 0;
	MapObstacleMgr	&mgr = MapObstacleMgr::Singleton();

	Coordinate	cellPos;
	cellPos.x = x * 2;
	cellPos.y = y * 2;

	for(int nRow = 0; nRow < 2; ++nRow)
	{
		for(int nCol = 0; nCol < 2; ++nCol)
		{
			obstacle |= mgr.GetCellObstacle(pMapObstacle, cellPos.x + nRow, cellPos.y + nCol);
		}
	}

	return obstacle;
}

// 用于以2*2个个自为单位进行搜索
static int GetBinarySearchWalkCost(const char *pMapObstacle, 
	const Coordinate &srcPos, 
	const Coordinate &dstPos
	)
{
	unsigned char obstacle = GetObstacleInBinarySearch(pMapObstacle, dstPos.x, dstPos.y);

	if(obstacle != 0)
		return WALKCOST_NOWAY;

	if(srcPos.x == dstPos.x || srcPos.y == dstPos.y)
		return WALKCOST_IN_NEARCELL;

	unsigned char obsHorizontal = INVALID_CELL_OBSTACLE;
	unsigned char obsVertiacl = INVALID_CELL_OBSTACLE;

	if(srcPos.x - 1 == dstPos.x)
	{
		if(srcPos.y - 1 == dstPos.y)
		{
			obsHorizontal = GetObstacleInBinarySearch(pMapObstacle, srcPos.x - 1, srcPos.y);
			obsVertiacl = GetObstacleInBinarySearch(pMapObstacle, srcPos.x, srcPos.y - 1);
		}
		else if(srcPos.y + 1 == dstPos.y)
		{
			obsHorizontal = GetObstacleInBinarySearch(pMapObstacle, srcPos.x - 1, srcPos.y);
			obsVertiacl = GetObstacleInBinarySearch(pMapObstacle, srcPos.x, srcPos.y + 1);
		}
	}
	else if(srcPos.x + 1 == dstPos.x)
	{
		if(srcPos.y - 1 == dstPos.y)
		{
			obsHorizontal = GetObstacleInBinarySearch(pMapObstacle, srcPos.x + 1, srcPos.y);
			obsVertiacl = GetObstacleInBinarySearch(pMapObstacle, srcPos.x, srcPos.y - 1);
		}
		else if(srcPos.y + 1 == dstPos.y)
		{
			obsHorizontal = GetObstacleInBinarySearch(pMapObstacle, srcPos.x + 1, srcPos.y);
			obsVertiacl = GetObstacleInBinarySearch(pMapObstacle, srcPos.x, srcPos.y + 1);
		}
	}
	else
	{
		_ASSERT(false);
		return WALKCOST_NOWAY;
	}
	

	if( (obsHorizontal != 0) || (obsVertiacl != 0) )
		return WALKCOST_NOWAY;
	else
		return WALKCOST_IN_DIAGONAL;
}

// 
unsigned char GetObstacleInTernarySearch(const char *pMapObstacle, int x, int y)
{
	unsigned char obstacle = 0;
	MapObstacleMgr	&mgr = MapObstacleMgr::Singleton();

	Coordinate	cellPos;
	cellPos.x = x * 3;
	cellPos.y = y * 3;

	for(int nRow = 0; nRow < 3; ++nRow)
	{
		for(int nCol = 0; nCol < 3; ++nCol)
		{
			obstacle |= mgr.GetCellObstacle(pMapObstacle, cellPos.x + nRow, cellPos.y + nCol);
		}
	}

	return obstacle;
}

// 用于以3*3个个自为单位进行搜索
static int GetTernarySearchWalkCost(const char *pMapObstacle, 
	const Coordinate &srcPos, 
	const Coordinate &dstPos
	)
{
	unsigned char obstacle = GetObstacleInTernarySearch(pMapObstacle, dstPos.x, dstPos.y);

	if(obstacle != 0)
		return WALKCOST_NOWAY;

	if(srcPos.x == dstPos.x || srcPos.y == dstPos.y)
		return WALKCOST_IN_NEARCELL;

	unsigned char obsHorizontal = INVALID_CELL_OBSTACLE;
	unsigned char obsVertiacl = INVALID_CELL_OBSTACLE;

	if(srcPos.x - 1 == dstPos.x)
	{
		if(srcPos.y - 1 == dstPos.y)
		{
			obsHorizontal = GetObstacleInTernarySearch(pMapObstacle, srcPos.x - 1, srcPos.y);
			obsVertiacl = GetObstacleInTernarySearch(pMapObstacle, srcPos.x, srcPos.y - 1);
		}
		else if(srcPos.y + 1 == dstPos.y)
		{
			obsHorizontal = GetObstacleInTernarySearch(pMapObstacle, srcPos.x - 1, srcPos.y);
			obsVertiacl = GetObstacleInTernarySearch(pMapObstacle, srcPos.x, srcPos.y + 1);
		}
	}
	else if(srcPos.x + 1 == dstPos.x)
	{
		if(srcPos.y - 1 == dstPos.y)
		{
			obsHorizontal = GetObstacleInTernarySearch(pMapObstacle, srcPos.x + 1, srcPos.y);
			obsVertiacl = GetObstacleInTernarySearch(pMapObstacle, srcPos.x, srcPos.y - 1);
		}
		else if(srcPos.y + 1 == dstPos.y)
		{
			obsHorizontal = GetObstacleInTernarySearch(pMapObstacle, srcPos.x + 1, srcPos.y);
			obsVertiacl = GetObstacleInTernarySearch(pMapObstacle, srcPos.x, srcPos.y + 1);
		}
	}
	else
	{
		_ASSERT(false);
		return WALKCOST_NOWAY;
	}
	

	if( (obsHorizontal != 0) || (obsVertiacl != 0) )
		return WALKCOST_NOWAY;
	else
		return WALKCOST_IN_DIAGONAL;
}

void GetNewMapPosInUnarySearch(const Coordinate &srcPathPos, 
							   const Coordinate &nextPathPos,
							   Coordinate &newMapPos
							   )
{
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	Coordinate	mapPos;
	Npc[nPlayerNpcIdx].GetMpsPos(&mapPos.x, &mapPos.y);

	if(nextPathPos.x > srcPathPos.x)
		newMapPos.x = nextPathPos.x * REGION_CELL_SIZE_X + REGION_CELL_SIZE_X - 2;	
	else if(nextPathPos.x < srcPathPos.x)
		newMapPos.x = nextPathPos.x * REGION_CELL_SIZE_X + 1;
	else
		newMapPos.x = mapPos.x;

	if(nextPathPos.y > srcPathPos.y)
		newMapPos.y = nextPathPos.y * REGION_CELL_SIZE_Y + REGION_CELL_SIZE_Y - 2;
	else if(nextPathPos.y < srcPathPos.y)
		newMapPos.y = nextPathPos.y * REGION_CELL_SIZE_Y + 1;
	else
		newMapPos.y = mapPos.y;
}

void GetNewMapPosInBinarySearch(const Coordinate &srcPathPos, 
						 		const Coordinate &nextPathPos,
								Coordinate &newMapPos
							    )
{
	newMapPos.x = (nextPathPos.x * enSearchUnit_2by2Cell + 1) * REGION_CELL_SIZE_X;
	newMapPos.y = (nextPathPos.y * enSearchUnit_2by2Cell + 1) * REGION_CELL_SIZE_Y;			
}

void GetNewMapPosInTernarySearch(const Coordinate &srcPathPos, 
								 const Coordinate &nextPathPos,
								 Coordinate &newMapPos
							    )
{
	newMapPos.x = (nextPathPos.x * enSearchUnit_2by2Cell + 1) * REGION_CELL_SIZE_X 
			+ REGION_CELL_SIZE_X / 2;
	newMapPos.y = (nextPathPos.y * enSearchUnit_2by2Cell + 1) * REGION_CELL_SIZE_Y 
			+ REGION_CELL_SIZE_Y / 2;
}

AutoPathFinder::AutoPathFinder()
{
	m_SearchPathUnit = enSearchUnit_1by1Cell;

	memset(&m_GetWalkCostFunc, 0, sizeof(m_GetWalkCostFunc));
	m_GetWalkCostFunc[enSearchUnit_1by1Cell] = &GetUnarySearchWalkCost;
	m_GetWalkCostFunc[enSearchUnit_2by2Cell] = &GetBinarySearchWalkCost;
	m_GetWalkCostFunc[enSearchUnit_3by3Cell] = &GetTernarySearchWalkCost;

	memset(&m_GetNewMapPosFunc, 0, sizeof(m_GetNewMapPosFunc));
	m_GetNewMapPosFunc[enSearchUnit_1by1Cell] = &GetNewMapPosInUnarySearch;
	m_GetNewMapPosFunc[enSearchUnit_2by2Cell] = &GetNewMapPosInBinarySearch;
	m_GetNewMapPosFunc[enSearchUnit_3by3Cell] = &GetNewMapPosInTernarySearch;
}

bool AutoPathFinder::canGoto(int nDstCellX, int nDstCellY, int nSearchUnit)
{
	if(nSearchUnit <= enSearchUnit_Begin || nSearchUnit >= enSearchUnit_End)
		return false;
	
	m_SearchPathUnit = nSearchUnit;
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int	nMapId = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_SubWorldID;
	
	MapObstacleMgr	&obstacleMgr = MapObstacleMgr::Singleton();
	
	if( !obstacleMgr.IsCordinateInMap(nMapId, nDstCellX, nDstCellY) )
		return false;
	
	const char *pMapObstacle = obstacleMgr.GetMapObstacle(nMapId);
	
	if(NULL == pMapObstacle)
		return false;
	
	unsigned char dstCellObs = obstacleMgr.GetCellObstacle(pMapObstacle, nDstCellX, nDstCellY);
	
	if(dstCellObs != 0)
		return false;

	return true;
}

#define AUTORUN_LIN_PRE_RETRY_PERCENTAGE 1/2

bool AutoPathFinder::SafeGoto(int & nDstCellX,int & nCellDstY,int nSearchUnit /* = enSearchUnit_1by1Cell  */)
{
	StopAutoWalk();
	
	if(nSearchUnit <= enSearchUnit_Begin || nSearchUnit >= enSearchUnit_End)
		return false;
	
	m_SearchPathUnit = nSearchUnit;
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int	nMapId = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_SubWorldID;
	
	MapObstacleMgr	&obstacleMgr = MapObstacleMgr::Singleton();
	
	const char *pMapObstacle = obstacleMgr.GetMapObstacle(nMapId);
	
	if(NULL == pMapObstacle)
		return false;
	
	bool             bRightDest = true;

	if( !obstacleMgr.IsCordinateInMap(nMapId, nDstCellX, nCellDstY) )
		bRightDest = false;
	
	unsigned char dstCellObs = obstacleMgr.GetCellObstacle(pMapObstacle, nDstCellX, nCellDstY);
	
	if(dstCellObs != 0)
		bRightDest =  false;
	
	int	nSrcMapX;
	int	nSrcMapY;
	Npc[nPlayerNpcIdx].GetMpsPos(&nSrcMapX, &nSrcMapY);
	
	Coordinate	srcCellPos;
	MapCordToCellCord(nSrcMapX, nSrcMapY, srcCellPos);

	if (!bRightDest)
	{
		//Retry the right pos before search...
        //Noitce: The failed of AStare algorithm would cost lot! we gonna make it more safe to enable the dest pos is reachable
		int x1,y1,x2,y2,dx,dy,p,x,y,adx,ady,i;
		
		bool bXWidther = abs(nCellDstY-srcCellPos.y) < abs(nDstCellX-srcCellPos.x);

		if (bXWidther)
		{
			x1 = nDstCellX;
			y1 = nCellDstY;
			x2 = srcCellPos.x;
			y2 = srcCellPos.y;
		}//endif
		else
		{
			y1 = nDstCellX;
			x1 = nCellDstY;
			y2 = srcCellPos.x;
			x2 = srcCellPos.y;
		}//end else

		dx=abs(x2-x1);
		dy=abs(y2-y1);

		x=x1;
		y=y1;
			
		if((x2-x1)>=0)
			adx=1;
		else 
			adx=-1;

		if((y2-y1)>=0)
			ady=1;
		else 
			ady=-1;
		
		p = 2*dy-dx;

		bool bSearched = false;
		int  nTryDx    = dx;

		if ( nTryDx > 2 * REGION_CELL_HEIGHT)
			nTryDx     = nTryDx * AUTORUN_LIN_PRE_RETRY_PERCENTAGE;

		for( i= 1;i <= nTryDx ;i ++ )
		{
			x+=adx;
		
			if(p>0)
			{
				y+=ady;
				p=(p+2*dy-2*dx);
			}//endif
			else
			{
				p=p+2*dy;
			}//end else

			int nTestX = x;
			int nTestY = y;

			if (!bXWidther)
			{
				nTestX = y;
				nTestY = x;
			}

			if ( obstacleMgr.IsCordinateInMap(nMapId, nTestX, nTestY) )
			{
				unsigned char dstCellObs = obstacleMgr.GetCellObstacle(pMapObstacle, nTestX, nTestY);
				
				if(dstCellObs == 0)
				{
					bSearched = true;
					break;
				}//endif

			}//endif

		}//end for i

		if (bSearched)
		{
			if (bXWidther)
			{
				nDstCellX = x;
				nCellDstY = y;
			}//endif
			else
			{
				nCellDstY = x;
				nDstCellX = y;
			}//end else

		}//endif

	}//endif
	
	m_DstCellPos.x = nDstCellX;
	m_DstCellPos.y = nCellDstY;
	
	srcCellPos.x /= m_SearchPathUnit;
	srcCellPos.y /= m_SearchPathUnit;
	m_DstCellPos.x /= m_SearchPathUnit;
	m_DstCellPos.y /= m_SearchPathUnit;
	
	// debug
#ifdef _DEBUG
	clock_t	start = clock();
#endif
	// debug
	
	if( !m_PathFinderPolicy.FindPath(pMapObstacle, m_GetWalkCostFunc[m_SearchPathUnit], srcCellPos, m_DstCellPos) )
	{
	    Coordinate newDest;
		m_PathFinderPolicy.GetNearestCoordinate(newDest);

		if (!m_PathFinderPolicy.FindPath(pMapObstacle,m_GetWalkCostFunc[m_SearchPathUnit],srcCellPos,newDest))
		{
			return false;
		}//endif

		m_DstCellPos =  newDest;
		nDstCellX    =  newDest.x * m_SearchPathUnit;
		nCellDstY    =  newDest.y * m_SearchPathUnit;
		
	}//endif

	m_FoundPath = m_PathFinderPolicy.GetPath();
	
	// debug
#ifdef _DEBUG
	clock_t end = clock();
	
	CoordinateArray::reverse_iterator itIdx = m_FoundPath.rbegin();
	CoordinateArray::reverse_iterator itEnd = m_FoundPath.rend();
	
	CFS_FILELOGS::WriteDebugLog("%d\n", m_FoundPath.size());
	
	for(; itIdx != itEnd; ++itIdx)
		CFS_FILELOGS::WriteDebugLog("(%d, %d)\n", itIdx->x, itIdx->y);
	
	double rst = double(end - start);
	rst = rst / CLOCKS_PER_SEC;
	CFS_FILELOGS::WriteDebugLog("Time: %lf\n", rst);
	CFS_FILELOGS::WriteDebugLog("*************************************\n");
#endif
	// debug	
	
	m_PreDstCellPos = *m_FoundPath.rbegin();
	m_PreMapPos.x = nSrcMapX;
	m_PreMapPos.y = nSrcMapY;
	
	Active();

	return true;
}

bool AutoPathFinder::GoTo(int nDstCellX, int nDstCellY, int nSearchUnit /* = enSearchUnit_1by1Cell */)
{
	StopAutoWalk();

	if(nSearchUnit <= enSearchUnit_Begin || nSearchUnit >= enSearchUnit_End)
		return false;
	
	m_SearchPathUnit = nSearchUnit;
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int	nMapId = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_SubWorldID;

	MapObstacleMgr	&obstacleMgr = MapObstacleMgr::Singleton();

	if( !obstacleMgr.IsCordinateInMap(nMapId, nDstCellX, nDstCellY) )
		return false;

	const char *pMapObstacle = obstacleMgr.GetMapObstacle(nMapId);

	if(NULL == pMapObstacle)
		return false;

	unsigned char dstCellObs = obstacleMgr.GetCellObstacle(pMapObstacle, nDstCellX, nDstCellY);
	
	if(dstCellObs != 0)
		return false;

	int	nSrcMapX;
	int	nSrcMapY;
	Npc[nPlayerNpcIdx].GetMpsPos(&nSrcMapX, &nSrcMapY);

	m_DstCellPos.x = nDstCellX;
	m_DstCellPos.y = nDstCellY;

	Coordinate	srcCellPos;
	MapCordToCellCord(nSrcMapX, nSrcMapY, srcCellPos);

	srcCellPos.x /= m_SearchPathUnit;
	srcCellPos.y /= m_SearchPathUnit;
	m_DstCellPos.x /= m_SearchPathUnit;
	m_DstCellPos.y /= m_SearchPathUnit;

	// debug
#ifdef _DEBUG
	clock_t	start = clock();
#endif
	// debug

	if( !m_PathFinderPolicy.FindPath(pMapObstacle, m_GetWalkCostFunc[m_SearchPathUnit], srcCellPos, m_DstCellPos) )
		return false;
	
	m_FoundPath = m_PathFinderPolicy.GetPath();

	// debug
#ifdef _DEBUG
	clock_t end = clock();

	CoordinateArray::reverse_iterator itIdx = m_FoundPath.rbegin();
	CoordinateArray::reverse_iterator itEnd = m_FoundPath.rend();

	CFS_FILELOGS::WriteDebugLog("%d\n", m_FoundPath.size());

	for(; itIdx != itEnd; ++itIdx)
		CFS_FILELOGS::WriteDebugLog("(%d, %d)\n", itIdx->x, itIdx->y);
	
	double rst = double(end - start);
	rst = rst / CLOCKS_PER_SEC;
	CFS_FILELOGS::WriteDebugLog("Time: %lf\n", rst);
	CFS_FILELOGS::WriteDebugLog("*************************************\n");
#endif
	// debug	

	m_PreDstCellPos = *m_FoundPath.rbegin();
	m_PreMapPos.x = nSrcMapX;
	m_PreMapPos.y = nSrcMapY;
	Active();
	return true;
}

bool AutoPathFinder::Active()
{
	if( !IsInAutoWalk() )
		return false;
	
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	Coordinate	mapPos;
	Npc[nPlayerNpcIdx].GetMpsPos(&mapPos.x, &mapPos.y);

	Coordinate	cellPos;
	MapCordToCellCord(mapPos.x, mapPos.y, cellPos);

	cellPos.x /= m_SearchPathUnit;
	cellPos.y /= m_SearchPathUnit;
	
	if(cellPos == m_DstCellPos)
	{
		AutoDialogNpc::getSingleton().openDialogNpc();
		StopAutoWalk();
		return false;
	}
	else
	{
		const Coordinate	*pNextPos = NULL;


		
		if(cellPos == m_PreDstCellPos)
		{
			pNextPos = GetNextCellInPath(cellPos);
			if(pNextPos == NULL )
			{
				AutoDialogNpc::getSingleton().openDialogNpc();
				StopAutoWalk();
				return false;
			}
		}
		else
		{
			// 防止出现走到不在路径中的节点，而且不再走了
			if(mapPos == m_PreMapPos)
			{
				pNextPos = GetNextCellInPath(cellPos);

				if(NULL == pNextPos)
					pNextPos = &m_PreDstCellPos;
			}
		}

		m_PreMapPos = mapPos;
		
		if(pNextPos)
		{
			Coordinate	newMapPos;
			(*m_GetNewMapPosFunc[m_SearchPathUnit])(cellPos, *pNextPos, newMapPos);

			m_PreDstCellPos = *pNextPos;
			Npc[nPlayerNpcIdx].SendCommand(do_run, newMapPos.x, newMapPos.y);
			SendClientCmdRun(newMapPos.x, newMapPos.y);

			// debug
#ifdef _DEBUG
			CFS_FILELOGS::WriteDebugLog("SendCmd:  \
				CurPos(%d, %d)(%d, %d) \
				NextPos(%d, %d)(%d, %d)\n", 
				mapPos.x, mapPos.y, cellPos.x, cellPos.y,
				newMapPos.x, newMapPos.y, pNextPos->x, pNextPos->y);
#endif
			// debug
		}
	}
	return true;
}

const Coordinate*	AutoPathFinder::GetNextCellInPath(const Coordinate &pos)
{
	CoordinateRIter itPos;
	CoordinateRIter itEnd = m_FoundPath.rend();

	for(itPos = m_FoundPath.rbegin(); itPos != itEnd; ++itPos)
	{
		if(*itPos == pos)
			break;
	}

	if(itPos == itEnd || itPos + 1 == itEnd)
		return NULL;
	else
	{
		++itPos;
		Coordinate	*pPrePos = &(*itPos);

		if(itPos + 1 == itEnd)
			return pPrePos;

		// 寻找处于同一条直线上的最后一个格子
		if( IsOnHorizontalLine(*pPrePos, *(itPos + 1)) )
		{
			while(++itPos != itEnd)
			{
				if( IsOnHorizontalLine(*pPrePos, *itPos) )
					pPrePos = &(*itPos);
				else
					break;
			}
		}
		else if( IsOnVerticalLine(*pPrePos, *(itPos + 1)) )
		{
			while(++itPos != itEnd)
			{
				if( IsOnVerticalLine(*pPrePos, *itPos) )
					pPrePos = &(*itPos);
				else
					break;
			}
		}
		else if( IsOnDiagonalLine(*pPrePos, *(itPos + 1)) )
		{
			while(++itPos != itEnd)
			{
				if( IsOnDiagonalLine(*pPrePos, *itPos) )
					pPrePos = &(*itPos);
				else
					break;
			}
		}
		
		return pPrePos;
	}
}

#endif	// #ifdef _AUTO_ROBOT