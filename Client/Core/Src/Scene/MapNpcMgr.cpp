

#include "KCore.h"
#include "MapNpcMgr.h"
#include "IQueryInfo.h"
#include "KNpcTemplate.h"

#ifndef _SERVER

MapNpcMgr::MapNpcMgr()
{
	
}

MapNpcMgr::~MapNpcMgr()
{
	
}

MapNpcMgr& MapNpcMgr::getSingleton()
{
	static MapNpcMgr singleton;
	return singleton;
}

vector<MapNpcMgr::NpcInfo>& MapNpcMgr::loadMap(char* mapName)
{
	if(_curMapName != mapName)
	{
		_curMapName = mapName;
		_npcList.clear();

		IQueryManager* mgr = NULL;
		CreateQueryManager(&mgr);

		IQueryResult* rst = NULL;
		int	retCode = mgr->QueryRequest(&rst, mapName, maps_query);
		if(retCode != query_succeed)
		{
			return _npcList;
		}

		if(!rst)
		{
			return _npcList;
		}

		int rstCount = 0;
		vector<int>* rstSet;
		QueryResultType rstType = map_id;
		rst->GetQueryResult(&rstCount, &rstSet, 0, &rstType);

		if(!rstCount)
		{
			return _npcList;
		}

		if(!rstSet)
		{
			return _npcList;
		}

		//int nMapPosCount = rstSet->size() < 6 ? rstSet->size() : 6;
		for(int i = 0; i < rstSet->size(); ++i)
		{
			KNpcTemplate aNpc;
			int npcId = (*rstSet)[i];
			if(npcId <= 0)
			{
				continue;
			}

			aNpc.InitNpcBaseData(npcId);
			int xPos = 0;
			int yPos = 0;
			int id = 0;
			int ret = sscanf(aNpc.m_nMapPos[0], "gt=pos id=%d id1=%d id2=%d", &id, &xPos, &yPos);
			if(ret != 3)
			{
				_ASSERT(0);
				continue;
			}
 			_npcList.push_back(MapNpcMgr::NpcInfo(xPos, yPos, aNpc.Name, npcId, id));
		}
	}	

	return _npcList;
}

NpcMapPos& MapNpcMgr::findNpc(int npcIndex)
{
	static NpcMapPos retNpc;
	static int oldNpcIndex = 0;
	if(oldNpcIndex == npcIndex)
	{
		return retNpc;
	}
	oldNpcIndex = npcIndex;

	KNpcTemplate aNpc;
	aNpc.InitNpcBaseData(npcIndex);//如果npcIndex无效，aNpc.m_nMapPos[0]将是一个空字符串
	
	int ret = sscanf(aNpc.m_nMapPos[0], "gt=pos id=%d id1=%d id2=%d", &retNpc.mapId, &retNpc.x, &retNpc.y);
	if(ret != 3)
	{
		retNpc.x = 0;
		retNpc.y = 0;
		_ASSERT(0);
	}
	
	return retNpc;
}

#endif