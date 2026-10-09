#ifndef TONG_POOL_COMBAT_MANAGER_H
#define TONG_POOL_COMBAT_MANAGER_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 12/10/2007 12:17
//      File_base        : pool_combat_mgr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 分星池逻辑
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "pool_combat_info_mgr.h"
#include "ServerSocialUnitMgr.h"
#include <map>
#include <list>

class KPoolCombatMgr
{
	//Base struct used by tong war system............................................................................................................................
	typedef struct tagStateEnviroment //Logic state machie enviroment variable type
	{
		SocialUnit * pInvader ;
		SocialUnit * pDefender;
        int          nMapWorldIndex;
		int          nMapPoolNpcIndex;

		tagStateEnviroment():pInvader(NULL),pDefender(NULL),
			nMapWorldIndex(-1),nMapPoolNpcIndex(-1){/**/}
	}StateEnviroment;

	typedef struct tagWarChangeFactor //Logic state path
	{
       	bool (KPoolCombatMgr::*ChgCondTester)(PoolCombatInfo * pWarInfo ,StateEnviroment * pSE); //Condition to get to the next state
		void (KPoolCombatMgr::*ChgAct       )(PoolCombatInfo * pWarInfo ,StateEnviroment * pSE); //Action when condition is true execute when change into next state
		
		tagWarChangeFactor():ChgCondTester(NULL),ChgAct(NULL)
		{/*Do nothing at all*/}

		bool  IsChangeValid(void)const {return (ChgCondTester!=NULL);}

	}WarChangeFactor;

	typedef struct tagMapPoolLoadingCondition
	{
		bool         m_IsPoolSocialUnitLoaded;
		bool         m_IsMapPoolLoaded;

		bool         m_IsHasSubPool;
		bool         m_IsSubPoolLoaded;
		bool         m_IsSubPoolLoading;


		bool         m_IsPoolLoading;
		bool         m_IsPoolSocialUnitLoading;
		
		tagMapPoolLoadingCondition():m_IsMapPoolLoaded(false),m_IsPoolSocialUnitLoaded(false),
			                         m_IsPoolLoading(false),m_IsPoolSocialUnitLoading(false),m_IsHasSubPool(false),
									 m_IsSubPoolLoaded(false),m_IsSubPoolLoading(false)
		{/**/}
	}MapPoolLoadingCondition;

	typedef std::map<int,MapPoolLoadingCondition> WarInfoLoadFlag;
	
	//Private member .........................................................................................................................................................................
    //State machie used 
	WarChangeFactor                       m_StateChgFactor[FS_POOL_COMBAT_TOTAL_STATE_NUM][FS_POOL_COMBAT_TOTAL_STATE_NUM];                                                     //状态机
	bool                                  (KPoolCombatMgr::*m_CheckStateEnviroment[FS_POOL_COMBAT_TOTAL_STATE_NUM])(PoolCombatInfo * pWarInfo ,StateEnviroment * lpOutSE);  //状态的环境判断与异常处理函数
	//Base condition info flag
	bool                                  m_GlobalNpcLoadReady;
	bool                                  m_InvaderLoading;
	WarInfoLoadFlag                       m_WarMapLoadFlag;
	std::list<FSGUID>                     m_SocialLoadedRec;
	//Others 
	int                                   m_CurTimeStamp;
	bool                                  m_IsInitedAll;

public:
    KPoolCombatMgr(void);
	~KPoolCombatMgr(void);
public:
	void InitWarMapSetting(void);
	void Breathe(void);
	void EnHanceANewCombat(const FSGUID & guid,int nMapID);
	bool IsInitedAll(void)const;
	bool IsPoolCombatMap(const int nMapID);
public:
	//Event Notify Fuctions
	void OnNpcSaveLoadComplete(void);
	void OnTongLoaded(const FSGUID & guid);
private: 
	//state change Condition Tester
	bool TestFromProcessToEnd(PoolCombatInfo * pWarInfo,StateEnviroment * pSE);
	bool TestFromEndToInvalid(PoolCombatInfo * pWarInfo,StateEnviroment * pSE);
    
	//State change action 
	void ChangeFromProcessToEnd(PoolCombatInfo * pWarInfo,StateEnviroment * pSE);
	void ChangeFromEndToInvalid(PoolCombatInfo * pWarInfo,StateEnviroment * pSE);

	//Enviroment and exeption handler
	bool EnviromentCheckInProcess(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE);
	bool EnviromentCheckInEnd(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE);
	bool EnviromentCheckInInvalid(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE);
private:
	//Assistant functions.
	void BindStateMachie(void);
	bool CheckWholeWorldState(void);
	bool CheckMapConditionLoadState(WarInfoLoadFlag::iterator & it);
	void JurgeWinFailed(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE);
	void GainPool(int nPoolMapId,int nPoolWorldIndex,SocialUnit * pInvader);
	void RobPool(int nPoolMapId,int nPoolWorldIndex,SocialUnit * pInvader,SocialUnit * pDefender);
	void InvaderTongWarMsgNotify(int nMapID ,SocialUnit * pInvaderUnit ,const char * szWarMsg);
	void DefenderTongWarMsgNotify(int nMapID,SocialUnit * pDefenderUnit,SocialUnit * pInvaderUnit ,const char * szWarMsg);
    void WarBeginNotify(int nMapID,const FSGUID & guid);
	void ClearMapWarState(PoolCombatInfo * pWarInfo);
	void CheckMapPoolAndSocialUnit(int nMapID,int nSubWorldIndex);
	bool CheckLoadState(const FSGUID & guid);
	void CheckClearPoolState(const int nSubWorldIndex);
	void ProtectPool(const int nSubWorldIndex);
private:
	//Log Functions..
    void DumpInvalidWarInfo(PoolCombatInfo * pWarInfo);
	void DumpTongWarStateChange(PoolCombatInfo * pWarInfo,int oldstate,int newstate);
	void DumpInvalidMapPool(int nMapID,const FSGUID & guid);
	void DumpWarMapComplete(void);
};

KPoolCombatMgr & GetGlobalPoolCombatMgr(void);

#endif