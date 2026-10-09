#ifndef KUiPlayerState_H
#define KUiPlayerState_H


#include "CEGUI.h"

using namespace CEGUI;

class KUiPlayerState : public Singleton<KUiPlayerState>
{
public:
	KUiPlayerState()
	{
		d_state = IDLE;
	}
	enum PLAYER_STATUS
	{
		IDLE,
		TRADE_NPC_BUY_SALE,			//正在与NPC交易，该NPC可以买卖
		TRADE_NPC_NORMAL_REPAIR,	//普通修理
		TRADE_NPC_SPECIAL_REPAIR,	//特殊修理
	};
	void setState(PLAYER_STATUS newState)
	{	
		d_state = newState; 
	};
	PLAYER_STATUS getState()
	{
		return d_state;
	};
private:
	PLAYER_STATUS d_state;
};

#endif