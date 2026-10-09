#ifndef _PAYSYS_H_
#define _PAYSYS_H_

#pragma	pack(push, 1)
//add by zuolizhi
/*
*	send area
*/
struct KGetAccountGameID : KAccountHead
{
	//DWORD dwIP;
	char szAccountName[32];
};

struct KFreezeAccount : KAccountHead
{
	char szAccountName[32];
	BYTE operate;
};

/*
*	return area
*/
//relay login return
struct KAccountRelayReturn : KAccountUserReturn
{
	short nGameID;
};
//query gameid return 
struct KGameIDAccountReturn : KAccountUserReturn
{
	short nGameID;
};
//freezeaccount return
struct KFreezeAccountReturn : KAccountUserReturn
{
	short nGameID;
};

struct KGetOnlinePlayerCount : KAccountHead
{
	int nGameID;
};

struct KGetOnlinePlayerCountReturn : KAccountHead
{
	int nGameID;
	int nReturn;
	int nPlayerCount;
};

enum rs2as
{
	rs2as_getonlineplayercount = 19,
	rs2as_freezeaccount = 47,
	rs2as_getaccountgameid	
};

enum as2rs
{
	as2rs_getonlineplayercount = 20,
	as2rs_freezeaccount = 47,
	as2rs_getaccountgameid
};

enum 
{
	account_lock = 0,
	account_unlock
};

#define SMALL_PACK_BUFFER_SIZE	(1024 * 4)
#define RELAYVERSION 0x4001

#pragma	pack(pop)
#endif