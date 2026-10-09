//xiehong - 2007-10-31

#include <vector>
#include <string>
#include "GameDataDef.h"

#ifndef _SERVER

using namespace std;

class MapNpcMgr
{
public:
	struct NpcInfo
	{
		int x;
		int y;
		string npcName;
		int npcId;
		int id;
		NpcInfo()
		{
			x = 0;
			y = 0;
			npcName = "";
			npcId = -1;
			id = -1;
		}
		NpcInfo(const NpcInfo& other)
		{
			x = other.x;
			y = other.y;
			npcName = other.npcName;
			npcId = other.npcId;
			id = other.id;
		}
		NpcInfo(int newX, int newY, string newNpcName, int newNpcId, int newId)
		{
			x = newX;
			y = newY;
			npcName = newNpcName;
			npcId = newNpcId;
			id = newId;
		}
	};

private:
	string	_curMapName;
	vector<NpcInfo> _npcList;
public:
	MapNpcMgr();
	~MapNpcMgr();

	static MapNpcMgr& getSingleton();
	vector<NpcInfo>& loadMap(char* mapName);
	NpcMapPos& findNpc(int npcIndex);
};

#endif