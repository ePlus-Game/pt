//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:3:16   11:19
//      File_base        : AStarPathFinder
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
#ifndef _AStarPathFinder_h
#define _AStarPathFinder_h
#ifdef _AUTO_ROBOT

#include <vector>
#include <map>
#include <algorithm>
#include "AutoRobotComDef.h"

using namespace std;

class AStarPathFinder
{
public:
	typedef int (*PGetWalkCost)(const char *pObstacleInfo, 
		const Coordinate &srcPos, 
		const Coordinate &dstPos
		);

	typedef struct _NodeInfo
	{
		short int	FScore;
		short int	HScore;
		short int	GScore;
		Coordinate	pos;
		Coordinate	parentPos;

		bool operator< (const _NodeInfo &rhs) const
		{
			return pos < rhs.pos;
		}

		bool operator== (const _NodeInfo &rhs) const
		{
			return pos == rhs.pos;
		}

	} NodeInfo;

private:
	enum
	{
		_null_pos_x = -1,
		_null_pos_y = -1,
		_max_fscore = 0x7fffffff,
	};

	typedef vector<Coordinate>	CoordinateArray;
	typedef map<Coordinate, NodeInfo*>	MapPos2NodeInfo;

public:
	bool	FindPath(const char *pObstacleInfo, 
		PGetWalkCost pFunc, 
		const Coordinate &srcPos,
		const Coordinate &dstPos
		);

	~AStarPathFinder();
	const vector<Coordinate>&	GetPath() const;
	void    GetNearestCoordinate(Coordinate & nearestPos);

private:
	void	FindPathByAStar();
	void	ConstructPath(const NodeInfo &dstNodeInfo);
	void	ProcessNode(const Coordinate &newPos, const NodeInfo &parentNode);

	void	Clear();
	bool	IsFoundFinished() const;
	const NodeInfo*	GetMinFScoreNode();
	bool	IsInCloseNodeSet(const Coordinate &newPos) const;
	bool	IsInOpenNodeSet(const Coordinate &newPos) const;
	NodeInfo* GetNodeInfo(MapPos2NodeInfo &infoSet, const Coordinate &pos);
	bool	IsPathFound() const;
	int		GetHScore(const Coordinate &srcPos, const Coordinate &dstPos);

private:
	const char	*m_MapObstacle;
	PGetWalkCost	m_FuncGetWalkCost;

	Coordinate	m_SrcPos;
	Coordinate	m_DstPos;
	bool		m_IsFoundFinished;
	vector<NodeInfo*>	m_OpenNodeHeap;	
	MapPos2NodeInfo	m_CloseNodeInfo;
	MapPos2NodeInfo	m_OpenNodeInfo;	
	CoordinateArray	m_FoundPath;

	NodeInfo        m_NearestNode;
};

inline bool AStarPathFinder::IsFoundFinished() const
{
	return m_IsFoundFinished;
}

inline bool AStarPathFinder::IsInCloseNodeSet(const Coordinate &newPos) const
{
	return m_CloseNodeInfo.find(newPos) != m_CloseNodeInfo.end();
}

inline bool	AStarPathFinder::IsInOpenNodeSet(const Coordinate &newPos) const
{
	return m_OpenNodeInfo.find(newPos) != m_OpenNodeInfo.end();
}

inline AStarPathFinder::NodeInfo* AStarPathFinder::GetNodeInfo(MapPos2NodeInfo &infoSet, 
															   const Coordinate &pos)
{
	MapPos2NodeInfo::iterator itInfo = infoSet.find(pos);
	return itInfo != infoSet.end() ? itInfo->second : NULL;
}

inline bool AStarPathFinder::IsPathFound() const
{
	return m_FoundPath.size() > 0;
}

inline const vector<Coordinate>& AStarPathFinder::GetPath() const
{
	return m_FoundPath;
}

inline int AStarPathFinder::GetHScore(const Coordinate &srcPos, const Coordinate &dstPos)
{
	return (abs(dstPos.x - srcPos.x) + abs(dstPos.y - srcPos.y)) * 10;	
}

inline void AStarPathFinder::GetNearestCoordinate(Coordinate & nearestPos)
{
	nearestPos.x = m_NearestNode.pos.x;
	nearestPos.y = m_NearestNode.pos.y;
}

#endif // #ifdef _AUTO_ROBOT
#endif // #ifndef _AStarPathFinder_h