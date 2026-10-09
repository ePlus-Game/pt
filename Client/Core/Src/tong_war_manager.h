#ifndef TONG_WAR_MANAGER_H
#define TONG_WAR_MANAGER_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/08/2007 16:54
//      File_base        : tong_war_manager
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 国战逻辑固化
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWarInfoManager.h"
#include "ServerSocialUnitMgr.h"
#include <map>

enum enWarMode
{
	nocity_vs_nocity_mode = 1,
	nocity_vs_city_mode,
	city_vs_nocity_rob_mode,
	city_vs_nocity_change_mode,
	city_vs_city_rob_mode,
	city_vs_city_change_mode,
};

enum enWarRResult
{
	war_result_invader_win  = 1,
	war_result_defender_win,
};

class KTongWarManager
{
	//Base struct used by tong war system............................................................................................................................
	typedef struct tagStateEnviroment //Logic state machie enviroment variable type
	{
		SocialUnit * pInvader ;
		SocialUnit * pDefender;
        int          nMapWorldIndex;
		int          nMapLordNpcIndex;
		int          nMapRobberNpcIndex;

		tagStateEnviroment():pInvader(NULL),pDefender(NULL),
			nMapWorldIndex(-1),nMapLordNpcIndex(-1),nMapRobberNpcIndex(-1){/**/}
	}StateEnviroment;

	typedef struct tagWarChangeFactor //Logic state path
	{
       	bool (KTongWarManager::*ChgCondTester)(FSWarInfo * pWarInfo ,StateEnviroment * pSE); //Condition to get to the next state
		void (KTongWarManager::*ChgAct       )(FSWarInfo * pWarInfo ,StateEnviroment * pSE); //Action when condition is true execute when change into next state
		
		tagWarChangeFactor():ChgCondTester(NULL),ChgAct(NULL)
		{/*Do nothing at all*/}

		bool  IsChangeValid(void)const {return (ChgCondTester!=NULL);}

	}WarChangeFactor;

	typedef struct tagMapWarLoadCondition
	{
		bool         m_IsLordSocialUnitLoaded;
		bool         m_IsRobberSocialUnitLoaded;
		bool         m_IsMapLordLoaded;
		bool         m_HasRobber;
		bool         m_IsRobberLoaded;
		bool         m_IsSubLordLoaded;

		bool         m_IsLordLoading;
		bool         m_IsRobberLoading;
		bool         m_IsLordSocialUnitLoading;
		bool         m_IsRobberSocialUnitLoading;
		bool		 m_IsSubLordLoading;
		
		tagMapWarLoadCondition():m_IsMapLordLoaded(false),m_IsRobberSocialUnitLoaded(false),m_IsLordSocialUnitLoaded(false),m_HasRobber(false),
			m_IsRobberLoaded(false),m_IsLordLoading(false),m_IsRobberLoading(false),m_IsRobberSocialUnitLoading(false),m_IsLordSocialUnitLoading(false),m_IsSubLordLoaded(false),m_IsSubLordLoading(false)
		{/**/}
	}MapWarLoadCondition;

	typedef std::map<int,MapWarLoadCondition> WarInfoLoadFlag;
	
	//Private member .........................................................................................................................................................................
    //State machie used 
	WarChangeFactor                       m_StateChgFactor[FS_WAR_TOTAL_STATE_NUM][FS_WAR_TOTAL_STATE_NUM];                                                     //状态机
	bool                                  (KTongWarManager::*m_CheckStateEnviroment[FS_WAR_TOTAL_STATE_NUM])(FSWarInfo * pWarInfo ,StateEnviroment * lpOutSE);  //状态的环境判断与异常处理函数
	//Base condition info flag
	bool                                  m_GlobalNpcLoadReady;
	WarInfoLoadFlag                       m_WarMapLoadFlag;
	//Others 
	int                                   m_CurTimeStamp;
	bool                                  m_IsInitedAll;

public:
    KTongWarManager(void);
	~KTongWarManager(void);
public:
	void InitWarMapSetting(void);
	bool IsTongWarMap(const int nMapID);
public:  
	void Breathe(void);
	bool IsInitedAll(void)const;
public:
	//Event Notify Fuctions
	void OnNpcSaveLoadComplete(void);
	void OnTongLoaded(const FSGUID & guid);
	void DumpRobberInfo(const char* pStr, int nStrSize);
private: 
	//state change Condition Tester
	bool TestFromNotifyToProcess(FSWarInfo * pWarInfo,StateEnviroment * pSE);
	bool TestFromNotifyToEnd(FSWarInfo * pWarInfo,StateEnviroment * pSE);
	bool TestFromProcessToEnd(FSWarInfo * pWarInfo,StateEnviroment * pSE);
	bool TestFromEndToInvalid(FSWarInfo * pWarInfo,StateEnviroment * pSE);
    
	//State change action 
    void ChangeFromNotifyToProcess(FSWarInfo * pWarInfo,StateEnviroment * pSE);
	void ChangeFromNotifyToEnd(FSWarInfo * pWarInfo,StateEnviroment * pSE);
	void ChangeFromProcessToEnd(FSWarInfo * pWarInfo,StateEnviroment * pSE);
	void ChangeFromEndToInvalid(FSWarInfo * pWarInfo,StateEnviroment * pSE);

	//Enviroment and exeption handler
	bool EnviromentCheckInNotify(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE);
	bool EnviromentCheckInProcess(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE);
	bool EnviromentCheckInEnd(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE);
	bool EnviromentCheckInInvalid(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE);
private:
	//Assistant functions.
	void BindStateMachie(void);
	bool CheckWholeWorldState(void);
	bool CheckMapConditionLoadState(WarInfoLoadFlag::iterator & it);
	void JurgeWinFailed(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE);
	void RobRes(const int nLordNpcIndex,const int nRoberNpcIndex,const int nMapId,const char * szTongName = NULL);
	void GainCity(int nCityMapId,int nCityWorldIndex,SocialUnit * pInvader);
	void RobCity(int nCityMapId,int nCityWorldIndex,SocialUnit * pInvader,SocialUnit * pDefender);
	void ChangeCity(const int nOldCityID,const int nNewCityID,SocialUnit * pInvader,SocialUnit * pDefender);
	void InvaderTongWarMsgNotify(int nMapID ,SocialUnit * pInvaderUnit ,const char * szWarMsg);
	void DefenderTongWarMsgNotify(int nMapID,SocialUnit * pDefenderUnit,SocialUnit * pInvaderUnit ,const char * szWarMsg);
	void ClearMapWarState(FSWarInfo * pWarInfo);
	void CheckMapLordAndSocialUnit(int nMapID,int nSubWorldIndex);
	void CheckRemoveRobber(int iSubWorldIndex,int nNpcIndex);
    void CheckAddProtectBuff(SocialUnit * pInvader);
	void CheckRemoveProtectBuff(SocialUnit * pInvader);
	bool CheckAddWorldRobber(int iSubWorldIndex,FSGUID & invaderGUID);
	void NotifyClientClearMap(StateEnviroment* lpOutSE);

	void FillMapInfo(int nRobberMapId, int nDefenerMapId, int& ScriptParam);
	void FillWarStateInfo(int nWarResult, int nWarMode, int& ScriptParam);
private:
	//Log Functions..
    void DumpInvalidWarInfo(FSWarInfo * pWarInfo);
	void DumpTongWarStateChange(FSWarInfo * pWarInfo,int oldstate,int newstate);
	void DumpInvalidMapLord(int nMapID,const FSGUID & guid);
	void DumpWarMapComplete(void);
};

KTongWarManager & GetGlobalTongWarMgr(void);

#endif