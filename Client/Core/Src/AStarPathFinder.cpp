//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:3:16   11:20
//      File_base        : AStarPathFinder
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

#include "AStarPathFinder.h"
#include "SocialAllocator.h"

#define	PATHFINDER_NODE_GRANULARITY		512
#define	PATHFINDER_NODE_MAXCOUNT		(2048 * 2048)
#define DEFAULT_PATHNODE_COUNT			512

__simpleallocator<sizeof(AStarPathFinder::NodeInfo), 
	PATHFINDER_NODE_GRANULARITY, 
	PATHFINDER_NODE_MAXCOUNT>	g_PathFinderAlloc;

bool GreaterFScore(const AStarPathFinder::NodeInfo *pLhs, const AStarPathFinder::NodeInfo *pRhs)
{
	return pLhs->FScore > pRhs->FScore;
}

AStarPathFinder::~AStarPathFinder()
{
	Clear();
}

bool AStarPathFinder::FindPath(const char *pObstacleInfo, 
		PGetWalkCost pFunc, 
		const Coordinate &srcPos,
		const Coordinate &dstPos
		)
{
	if(NULL == pObstacleInfo || NULL == pFunc)
		return false;

	Clear();

	m_OpenNodeHeap.reserve(PATHFINDER_NODE_GRANULARITY);
	m_FoundPath.reserve(DEFAULT_PATHNODE_COUNT);

	m_SrcPos = srcPos;
	m_DstPos = dstPos;
	m_FuncGetWalkCost = pFunc;
	m_MapObstacle = pObstacleInfo;
	
	NodeInfo	*pInfo = (NodeInfo*)g_PathFinderAlloc._alloc();
	if(NULL == pInfo)
		return false;

	pInfo->GScore = 0;
	pInfo->HScore = GetHScore(srcPos, dstPos);
	pInfo->FScore = pInfo->GScore + pInfo->HScore;
	pInfo->pos = srcPos;
	pInfo->parentPos.x = _null_pos_x;
	pInfo->parentPos.y = _null_pos_y;

	m_OpenNodeHeap.push_back(pInfo);
	m_OpenNodeInfo.insert(MapPos2NodeInfo::value_type(pInfo->pos, pInfo));

	m_NearestNode = *pInfo;

	if(srcPos == dstPos)
		ConstructPath(*pInfo);
	else
		FindPathByAStar();	

	return IsPathFound();	
}

void AStarPathFinder::FindPathByAStar()
{
	const NodeInfo	*pInfo = NULL;
	
	while( !IsFoundFinished() )
	{
		pInfo = GetMinFScoreNode();

		if(NULL == pInfo)
		{
			m_IsFoundFinished = true;
			break;
		}

		m_OpenNodeInfo.erase(pInfo->pos);
		m_CloseNodeInfo.insert(MapPos2NodeInfo::value_type(pInfo->pos, (NodeInfo*)pInfo) );

		for(int nRow = -1; nRow < 2; ++nRow)
		{
			for(int nCol = -1; nCol < 2; ++nCol)
			{
				if(0 == nRow && 0 == nCol)
					continue;

				if( !IsFoundFinished() )		
				{
					Coordinate	newPos;
					newPos.x = pInfo->pos.x + nCol;
					newPos.y = pInfo->pos.y + nRow;
					ProcessNode(newPos, *pInfo);
				}
			}
		}
	}
}

void AStarPathFinder::ProcessNode(const Coordinate &newPos, const NodeInfo &parentNode)
{
	int	nWalkCost = (*m_FuncGetWalkCost)(m_MapObstacle, parentNode.pos, newPos);

	// 找到目标点
	if(newPos == m_DstPos)
	{
		if(WALKCOST_NOWAY != nWalkCost)
		{
			NodeInfo	*pInfo = (NodeInfo*)g_PathFinderAlloc._alloc();
			pInfo->pos = newPos;
			pInfo->parentPos = parentNode.pos;

			ConstructPath(*pInfo);
		}
		else
			ConstructPath(parentNode);

		return;
	}

	if(WALKCOST_NOWAY == nWalkCost)
		return;

	if( IsInCloseNodeSet(newPos) )
		return;

	if( !IsInOpenNodeSet(newPos) )
	{
		NodeInfo	*pInfo = (NodeInfo*)g_PathFinderAlloc._alloc();		
		pInfo->pos = newPos;
		pInfo->parentPos = parentNode.pos;
		pInfo->HScore = GetHScore(newPos, m_DstPos);
		pInfo->GScore = parentNode.GScore + nWalkCost;
		pInfo->FScore = pInfo->GScore + pInfo->HScore;

		if (pInfo->HScore < m_NearestNode.HScore)
		{
			m_NearestNode = *pInfo;
		}//endif

		m_OpenNodeInfo.insert(MapPos2NodeInfo::value_type(pInfo->pos, pInfo));
		m_OpenNodeHeap.push_back(pInfo);
		push_heap(m_OpenNodeHeap.begin(), m_OpenNodeHeap.end(), GreaterFScore);
	}
	else
	{
		NodeInfo	*pInfo = GetNodeInfo(m_OpenNodeInfo, newPos);
		_ASSERT(pInfo);

		if(pInfo)
		{
			if(pInfo->GScore > parentNode.GScore + nWalkCost)
			{
				pInfo->GScore = parentNode.GScore + nWalkCost;
				pInfo->FScore = pInfo->GScore + pInfo->HScore;
				pInfo->parentPos = parentNode.pos;
			}
		}
	}
}

void AStarPathFinder::ConstructPath(const NodeInfo &dstNodeInfo)
{
	m_FoundPath.push_back(dstNodeInfo.pos);
	const NodeInfo *pInfo = &dstNodeInfo;

	while( pInfo = GetNodeInfo(m_CloseNodeInfo, pInfo->parentPos) )
	{
		m_FoundPath.push_back(pInfo->pos);

		if(pInfo->pos == m_SrcPos)
			break;
	}

	m_IsFoundFinished = true;
}

const AStarPathFinder::NodeInfo* AStarPathFinder::GetMinFScoreNode() 
{
	if( m_OpenNodeHeap.size() <= 0 )
		return NULL;

	pop_heap(m_OpenNodeHeap.begin(), m_OpenNodeHeap.end(), GreaterFScore);
	const NodeInfo *pInfo = m_OpenNodeHeap.back();
	m_OpenNodeHeap.pop_back();

	return pInfo;
}

void AStarPathFinder::Clear()
{
	m_FoundPath.clear();
	m_IsFoundFinished = false;

	map<Coordinate, NodeInfo*>::iterator itNode;
	map<Coordinate, NodeInfo*>::iterator itNodeEnd = m_CloseNodeInfo.end();

	for(itNode = m_CloseNodeInfo.begin(); itNode != itNodeEnd; ++itNode)
		g_PathFinderAlloc._free(itNode->second);

	itNodeEnd = m_OpenNodeInfo.end();
	for(itNode = m_OpenNodeInfo.begin(); itNode != itNodeEnd; ++itNode)
		g_PathFinderAlloc._free(itNode->second);

	m_OpenNodeHeap.clear();
	m_CloseNodeInfo.clear();
	m_OpenNodeInfo.clear();
}

#endif	// #ifdef _AUTO_ROBOT