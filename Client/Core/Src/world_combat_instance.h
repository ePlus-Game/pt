#ifndef K_WORLD_COMBAT_INSTANCE_H
#define K_WORLD_COMBAT_INSTANCE_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 03/23/2008 12:18
//      File_base        : world_combat_instance
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 战场副本
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "GameDataDef.h"

#define MAX_COMBAT_ORG_NUM     200
#define MAX_COMBAT_ORG_ID      MAX_COMBAT_ORG_NUM
#define INVALID_COMBAT_ORG_ID  0
#define MAX_COMBAT_TOP 10
#define IsValidCombatID(x)    ( x > 0 && x <= MAX_COMBAT_ORG_ID)

#define MAX_COMBAT_PERSON_NUM  (0x7fffffff)                       //避免越界
#define MAX_COMBAT_SCORE_NUM   (0x7fffffff)                       //避免越界

typedef struct tagCombatOrgnize
{
	int nPersonNum;
	int nScore    ;

	tagCombatOrgnize(void):nPersonNum(0),nScore(0){}


}CombatOrgnize;
#define COMBAT_TOP10_BUFF_LEN (MAX_COMBAT_TOP - 1) * sizeof(COMBAT_TOP10_MEMBER_INFO) + sizeof(COMBAT_TOP10_INFO) //战场同步协议的buff最大长度

struct CombatTop10Data
{
	char protocolBuff[COMBAT_TOP10_BUFF_LEN];//战场排名同步的buff
	BOOL CompressSuccess;
};

typedef struct tagWorldCombatInstanceInfo
{
	CombatOrgnize  org[MAX_COMBAT_ORG_NUM];
	CombatTop10Data CombatTop10[MAX_SCORE_ORG_SYNC]; //战场排名同步的buff数组

	void ClearCombatTop10(){memset(CombatTop10, 0, sizeof(CombatTop10));}

}WordCombatInstanceInfo;

typedef struct tagPlayerCombatInfo
{
	int             nScore;

	tagPlayerCombatInfo(void):nScore(0)
	{/**/}

}PlayerCombatInfo;

#pragma	pack(push, 1)

typedef struct tagSubwordPlayerInfo
{
	BYTE         Orgnize;
	DWORD        NpcId;
	int          MapX;
	int          MapY;

}WORLD_PLAYER_INFO;

#pragma  pack(pop)

#ifdef _SERVER

#define MAX_SCORE_LEVEL                   200
#define WORLD_COMBAT_INFO_MAX_SETTINGS    "\\settings\\WorldCombatMaxScore.txt"
#define WORLD_COMBAT_INFO_KILLED_SETTINGS "\\settings\\WorldCombatKillScore.txt"

class KWorldCombatSetting
{
	unsigned long        m_ScoreLimited[MAX_SCORE_LEVEL];
	unsigned long        m_KillScore[MAX_SCORE_LEVEL];

public:
	 KWorldCombatSetting(void);
	~KWorldCombatSetting(void);

	void                 Init();
	int                  GetMaxScoreByLevel(const int nPlayerLevel);
	int                  GetKillScoreByLevel(const int nPlayerLevel);

public:
	static KWorldCombatSetting & Singleton(void);
};
#endif

#endif