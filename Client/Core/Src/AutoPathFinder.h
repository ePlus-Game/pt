//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:3:12   13:46
//      File_base        : AutoPathFinder
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
#ifndef _AutoPathFinder_h
#define _AutoPathFinder_h
#ifdef _AUTO_ROBOT

#include <vector>
#include "AutoRobotComDef.h"
#include "AStarPathFinder.h"

using namespace std;

class AutoPathFinder
{
public:
	AutoPathFinder();
	bool	GoTo(int nDstCellX, int nCellDstY, int nSearchUnit = enSearchUnit_1by1Cell);
	bool    SafeGoto(int & nDstCellX,int & nCellDstY,int nSearchUnit = enSearchUnit_1by1Cell );
	bool	Active();
	bool	canGoto(int nDstCellX, int nDstCellY, int nSearchUnit = enSearchUnit_1by1Cell);
	void	StopAutoWalk();
	void	GetDestCellPos(int &nDstCellX, int &nDstCellY);

private:
	void	MapCordToCellCord(int nMpsX, int nMpsY, Coordinate	&pos);
	bool	IsInAutoWalk();

	bool	IsOnVerticalLine(const Coordinate &first, const Coordinate &next);
	bool	IsOnHorizontalLine(const Coordinate &first, const Coordinate &next);
	bool	IsOnDiagonalLine(const Coordinate &first, const Coordinate &next);
	const Coordinate*	GetNextCellInPath(const Coordinate &pos);

private:
	typedef	vector<Coordinate>	CoordinateArray;
	typedef CoordinateArray::reverse_iterator	CoordinateRIter;
	typedef	int (*PGetWalkCostFunc)(const char *pMapObstacle, 
		const Coordinate &srcPos, 
		const Coordinate &dstPos);
	typedef void (*PGetNewMapPosFunc)(const Coordinate &srcPathPos, 
		const Coordinate &nextPathPos, 
		Coordinate &newMapPos);

	int	m_SearchPathUnit;
	Coordinate	m_DstCellPos;
	Coordinate	m_PreMapPos;
	Coordinate	m_PreDstCellPos;
	PGetWalkCostFunc	m_GetWalkCostFunc[enSearchUnit_End];
	PGetNewMapPosFunc	m_GetNewMapPosFunc[enSearchUnit_End];
	CoordinateArray	m_FoundPath;
	AStarPathFinder	m_PathFinderPolicy;
};

inline void AutoPathFinder::StopAutoWalk()
{
	m_FoundPath.clear();
}

inline void	AutoPathFinder::MapCordToCellCord(int nMpsX, int nMpsY, Coordinate	&pos)
{
	pos.x = nMpsX / REGION_CELL_SIZE_X;
	pos.y = nMpsY / REGION_CELL_SIZE_Y;
}

inline bool	AutoPathFinder::IsInAutoWalk()
{
	return m_FoundPath.size() > 0;
}

inline bool AutoPathFinder::IsOnVerticalLine(const Coordinate &first, const Coordinate &next)
{
	return first.y == next.y;
}

inline 	bool AutoPathFinder::IsOnHorizontalLine(const Coordinate &first, const Coordinate &next)
{
	return first.x == next.x;
}

inline bool	AutoPathFinder::IsOnDiagonalLine(const Coordinate &first, const Coordinate &next)
{
	return 1 == abs(first.x - next.x) && 
		1 == abs(first.y - next.y);
}

inline void	AutoPathFinder::GetDestCellPos(int &nDstCellX, int &nDstCellY)
{
	if (IsInAutoWalk())
	{
		nDstCellX = m_DstCellPos.x;
		nDstCellY = m_DstCellPos.y;
	}
}

#endif	// #ifdef _AUTO_ROBOT
#endif	// #ifndef _AutoPathFinder_h