#ifndef PLAYER_SHOW_INFO
#define PLAYER_SHOW_INFO
#include "GameDataDef.h"
#include <map>
using std::map;
class PlayerShowInfo
{
public:
	
	PlayerShowInfo();
	~PlayerShowInfo();
	void  GetBaseInfo(PlayerInfo& playerInfo);
	void  AddItem(PlayerInfo& info);
	static PlayerShowInfo& GetSingle();
private:
	typedef std::map<DWORD ,PlayerInfo> playerList;
	void  ShowInfoInText(PlayerInfo* info);

	playerList  playerInfoList;
};
#endif