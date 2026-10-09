#include "KCore.h"
#include "KPlayer.h"
#include "exp_insruance.h"
#include "KSubWorldSet.h"
#include "CoreRelated.h"

//KExpQuestInsuraceSetting ........................................................................
KExpQuestInsuraceSetting::KExpQuestInsuraceSetting()
:m_EnableExpManually(true),m_EnableQuesetManually(true),m_MaxOfflineReward(0),m_ExpOnlineRewardTime(0),m_SettingsLoaded(false)
,m_ExpEnableLevel(MAX_EXP_QUEST_INSRUANCE_LEVEL),m_QuestEnableLevel(MAX_EXP_QUEST_INSRUANCE_LEVEL)
{
	ZeroMemory(m_MaxExpReward,sizeof(m_MaxExpReward));
	ZeroMemory(m_ExpInsuranceFunc,sizeof(m_ExpInsuranceFunc));
	ZeroMemory(m_QuestInsuranceFunc,sizeof(m_QuestInsuranceFunc));
}

KExpQuestInsuraceSetting::~KExpQuestInsuraceSetting()
{ /*Do Nothing at all*/ }


#define  MAX_SECTION_NAME 128

bool KExpQuestInsuraceSetting::Load()
{
	KIniFile  kSetting;
	
	if (!kSetting.Load( EXP_QUEST_INSURANCE_SETTING_PATH ))
	{
		return FALSE;
	}//endif
	
	kSetting.GetInteger("CommonSettings","MaxOfflineReward",0,&m_MaxOfflineReward);
	
	if ( m_MaxOfflineReward < 0 )
		 m_MaxOfflineReward = 0;

	if ( m_MaxOfflineReward >= MAX_INT_VALUE )
		 m_MaxOfflineReward =  MAX_INT_VALUE - 1;
	
	kSetting.GetInteger("CommonSettings","ExpRewardOnlineGiveTime",0,&m_ExpOnlineRewardTime);
	
	if (m_ExpOnlineRewardTime < 0 )
		m_ExpOnlineRewardTime = 0;

	if (m_ExpOnlineRewardTime >= 24)
		m_ExpOnlineRewardTime = 23;

	kSetting.GetInteger("CommonSettings","ExpInsuranceEnableLevel", MAX_EXP_QUEST_INSRUANCE_LEVEL ,&m_ExpEnableLevel);
	
	if ( m_ExpEnableLevel < 0 || m_ExpEnableLevel > MAX_EXP_QUEST_INSRUANCE_LEVEL)
	{
		 m_ExpEnableLevel = MAX_EXP_QUEST_INSRUANCE_LEVEL; 
	}//endif
	
	
	kSetting.GetInteger("CommonSettings","QuestInsuranceEnableLevel", MAX_EXP_QUEST_INSRUANCE_LEVEL ,&m_QuestEnableLevel);
	
	if ( m_QuestEnableLevel < 0 || m_QuestEnableLevel > MAX_EXP_QUEST_INSRUANCE_LEVEL)
	{
		m_QuestEnableLevel = MAX_EXP_QUEST_INSRUANCE_LEVEL; 
	}//endif
	

	kSetting.GetString("CommonSettings","ExpInsruanceScriptFunc","",m_ExpInsuranceFunc,sizeof(m_ExpInsuranceFunc));
	m_ExpInsuranceFunc[sizeof(m_ExpInsuranceFunc) - 1] = 0;

	kSetting.GetString("CommonSettings","QuestInsuranceScriptFunc","",m_QuestInsuranceFunc,sizeof(m_QuestInsuranceFunc));
	m_QuestInsuranceFunc[sizeof(m_QuestInsuranceFunc) - 1] = 0;
	
	char szSectionName[MAX_SECTION_NAME] = "";
	int  nLastAvailableRewardValue       = 0;

	for (int n = 1; n <= MAX_EXP_QUEST_INSRUANCE_LEVEL ; n ++ )
	{
		int nValue = 0;
		sprintf(szSectionName,"%d",n);
		kSetting.GetInteger("LevelMaxExpReward",szSectionName,0,&nValue);

		if (nValue == 0)
			nValue = nLastAvailableRewardValue;

		if ( nValue < 0 )
		 	 nValue = 0;

		if ( nValue >= MAX_ADD_EXP)
			 nValue  = MAX_ADD_EXP - 1;

		m_MaxExpReward[ n - 1]    = nValue;
		nLastAvailableRewardValue = nValue;
	}//end for n

	m_SettingsLoaded = true;

	return TRUE;
}

void KExpQuestInsuraceSetting::SetQuestEnable(bool bEnable )
{
	m_EnableQuesetManually  = bEnable;
}

void KExpQuestInsuraceSetting::SetExpEnable( bool bEnable )
{
	m_EnableExpManually     = bEnable;
}

bool KExpQuestInsuraceSetting::IsExpEnabled() const 
{
	return m_SettingsLoaded && m_EnableExpManually;
}

bool KExpQuestInsuraceSetting::IsQuesetEnabled() const 
{
	return m_SettingsLoaded && m_EnableQuesetManually;
}

int  KExpQuestInsuraceSetting::GetMaxExpRewardByLevel(const int nLevel)const
{
	if ( nLevel > 0 && nLevel <= MAX_EXP_QUEST_INSRUANCE_LEVEL )
	{
		return m_MaxExpReward[nLevel - 1];
	}//endif
	else
	{
		if (nLevel > MAX_EXP_QUEST_INSRUANCE_LEVEL )
			return m_MaxExpReward[MAX_EXP_QUEST_INSRUANCE_LEVEL - 1];		

		return 0;
	}//else
	
}

char * KExpQuestInsuraceSetting::GetExpInsuranceFuncName()
{
	return m_ExpInsuranceFunc;
}

char * KExpQuestInsuraceSetting::GetQuestInsuranceFuncName()
{
	return m_QuestInsuranceFunc;
}

int KExpQuestInsuraceSetting::GetExpOnlineRewardTime()const
{
	return m_ExpOnlineRewardTime;
}

int KExpQuestInsuraceSetting::GetExpEnableLevel()const
{
	return m_ExpEnableLevel;
}

int KExpQuestInsuraceSetting::GetQuestEnableLevel()const
{
	return m_QuestEnableLevel;
}

int KExpQuestInsuraceSetting::GetMaxOfflineReward( )const
{
	return m_MaxOfflineReward;
}

KExpQuestInsuraceSetting & KExpQuestInsuraceSetting::Singleton()
{
	static KExpQuestInsuraceSetting settings;
	return settings;
}


//KExpInsuraceMgr ...........................................................................
#ifdef _SERVER

#define EXP_INSRUANCE_CHECK_INTERVAL 60 * GAME_FPS

KExpInsuranceMgr::KExpInsuranceMgr( )
{
	Release();
}

KExpInsuranceMgr::~KExpInsuranceMgr()
{/*Do Nothing*/}

void KExpInsuranceMgr::Init(const int nPlayerIndex )
{
	m_PlayerIndex = nPlayerIndex;
}

void KExpInsuranceMgr::PlayerOnline( void)
{
	if (!KExpQuestInsuraceSetting::Singleton().IsExpEnabled())
		return ;

	if (!IsValidPlayer(m_PlayerIndex))
	{
		return;
	}//endif
	
	if (!IsEnterInsuraceState() && GetPlayerLevel(m_PlayerIndex) >= KExpQuestInsuraceSetting::Singleton().GetExpEnableLevel())
		ReEnterInsuraceState();

	SyncRewardState();
	SyncRewardExp();
	CheckGiveRewardExp();
}

void KExpInsuranceMgr::Release()
{
	m_ExpCache       = 0;
	m_LastRewardTime = 0;
	m_PlayerIndex    = INVALID_PLAYER_INDEX; 
}

void KExpInsuranceMgr::SetData(const int nExpCache,const DWORD dwLastRewardTime)
{
	m_ExpCache         = nExpCache;
	m_LastRewardTime   = dwLastRewardTime;
}

void KExpInsuranceMgr::GetData(int & nExpCache , DWORD & dwLastRewardTime )
{
	nExpCache         =  m_ExpCache;
	dwLastRewardTime  =  m_LastRewardTime;
}

int  KExpInsuranceMgr::GetCurRewardExp() const 
{
	return m_ExpCache;
}

void KExpInsuranceMgr::ContributeExp(const int nExp )
{
	if (!KExpQuestInsuraceSetting::Singleton().IsExpEnabled() || !IsEnterInsuraceState())
		return ;

	if (nExp <= 0 )
		return ;

	if (!IsValidPlayer(m_PlayerIndex))
		return ;

	int nCurrent      = m_ExpCache;
	int nLevelExpMax  = KExpQuestInsuraceSetting::Singleton().GetMaxExpRewardByLevel(Player[m_PlayerIndex].GetLevel());
	
	if ( nCurrent >= nLevelExpMax || nCurrent < 0)
		return;

	int nConributeExp = CalcRewardExp( nExp );
	if (nConributeExp <= 0)
		return ;

	int nFinalExp     = nCurrent + nConributeExp;
	if (nFinalExp <= 0 || nFinalExp > nLevelExpMax ) // too big 
	{
		nFinalExp     = nLevelExpMax;
	}//endif
	
	ChangeExpCache(nFinalExp);
}

void KExpInsuranceMgr::SyncRewardState() const
{
	if (!IsValidPlayer(m_PlayerIndex))
		return ;
	
	S2C_EXP_INSRUANCE rewardCacheNotify;
	rewardCacheNotify.Protocol       = s2c_byte_extend;
	rewardCacheNotify.ProtocolExtend = s2c_ex_protocol_exp_insurance;
	rewardCacheNotify.SubProtocol    = s2c_exp_insurance_sync_state;
	rewardCacheNotify.wProtocolSize  = sizeof(S2C_EXP_INSRUANCE) - 1;
	rewardCacheNotify.Data           = IsEnterInsuraceState();
	
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(),&rewardCacheNotify,sizeof(rewardCacheNotify));

}

void KExpInsuranceMgr::SyncRewardExp() const 
{
	if (!IsValidPlayer(m_PlayerIndex))
		return ;

	S2C_EXP_INSRUANCE rewardCacheNotify;
	rewardCacheNotify.Protocol       = s2c_byte_extend;
	rewardCacheNotify.ProtocolExtend = s2c_ex_protocol_exp_insurance;
	rewardCacheNotify.SubProtocol    = s2c_exp_insurance_sync_reward;
	rewardCacheNotify.wProtocolSize  = sizeof(S2C_EXP_INSRUANCE) - 1;
	rewardCacheNotify.Data         = m_ExpCache;

	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(),&rewardCacheNotify,sizeof(rewardCacheNotify));
}

void KExpInsuranceMgr::NotifyLevelUp(const int nLevel )
{
	if (!KExpQuestInsuraceSetting::Singleton().IsExpEnabled())
		return ;

	if (!IsValidPlayer(m_PlayerIndex))
		return;

	if ( nLevel == KExpQuestInsuraceSetting::Singleton().GetExpEnableLevel())
		ReEnterInsuraceState();
}

void KExpInsuranceMgr::Active()
{
	if (!KExpQuestInsuraceSetting::Singleton().IsExpEnabled() || !IsEnterInsuraceState())
		return ;

	if (!IsValidPlayer(m_PlayerIndex))
		return ;

	int  nGameTime = g_SubWorldSet.GetGameTime();
	if ( nGameTime % EXP_INSRUANCE_CHECK_INTERVAL == 0)
	{
		CheckGiveRewardExp();
	}//endif

}

bool KExpInsuranceMgr::IsEnterInsuraceState()const
{
	return m_LastRewardTime != 0;
}

void KExpInsuranceMgr::ReEnterInsuraceState()
{
	if (!IsValidPlayer(m_PlayerIndex))
		return;

	if ( m_LastRewardTime != 0)
		return;

	ChangeExpRewardTime( UNIX_TMIE_STAMP);
	ChangeExpCache(0);

	S2C_EXP_INSRUANCE rewardCacheNotify;
	rewardCacheNotify.Protocol       = s2c_byte_extend;
	rewardCacheNotify.ProtocolExtend = s2c_ex_protocol_exp_insurance;
	rewardCacheNotify.SubProtocol    = s2c_exp_insurance_enter_notify;
	rewardCacheNotify.wProtocolSize  = sizeof(S2C_EXP_INSRUANCE) - 1;
	rewardCacheNotify.Data           = m_ExpCache;
	
	if (g_pServer)
			g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(),&rewardCacheNotify,sizeof(rewardCacheNotify));

}

void KExpInsuranceMgr::LeaveInsuranceState()
{
	ChangeExpCache(0);
	ChangeExpRewardTime(0);
}

void KExpInsuranceMgr::ChangeExpCache(int nExp )
{
	if ( nExp != m_ExpCache)
	{
		m_ExpCache = nExp;
		SyncRewardExp();
	}//endif
}

void KExpInsuranceMgr::ChangeExpRewardTime(DWORD dwTime)
{
	if (dwTime != m_LastRewardTime)
	{
		m_LastRewardTime = dwTime;
		SyncRewardState();
	}//endif

}

void KExpInsuranceMgr::CheckGiveRewardExp()
{
	if (!KExpQuestInsuraceSetting::Singleton().IsExpEnabled() || !IsEnterInsuraceState())
		return;
		
	if (!IsValidPlayer(m_PlayerIndex))
		return;
	
	//Time check here............................................................................................
	//Current data..
	time_t tCurentTime= UNIX_TMIE_STAMP;
	tm   * time       = localtime(&tCurentTime);
	if    (time == NULL)
		   return;

	tm     current;
	memcpy(&current,time,sizeof(current));

	//Last data..
	time              = localtime((long *)&m_LastRewardTime);
	if    (time == NULL)
		   return;
	tm     last;
	memcpy(&last,time,sizeof(last));

    if ( UNIX_TMIE_STAMP >= m_LastRewardTime + 3600 * 24 || (last.tm_mday != current.tm_mday && current.tm_hour >= KExpQuestInsuraceSetting::Singleton().GetExpOnlineRewardTime()))
	{
		if (m_ExpCache > 0 && m_ExpCache < MAX_ADD_EXP)
		{
			if (m_ExpCache >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_exp_amount) && g_pLogSystem)
			{
				LogEventParam addExpEvent;
				addExpEvent.event  = log_event_exp_insurance_add_reward;
				addExpEvent.param1 = Player[m_PlayerIndex].GetGUID();
				addExpEvent.param4 = m_ExpCache;
				g_pLogSystem->Log(addExpEvent);
			}//endif
			
			S2C_EXP_INSRUANCE rewardCacheNotify;
			rewardCacheNotify.Protocol       = s2c_byte_extend;
			rewardCacheNotify.ProtocolExtend = s2c_ex_protocol_exp_insurance;
			rewardCacheNotify.SubProtocol    = s2c_exp_insurance_fetch_reward;
			rewardCacheNotify.wProtocolSize  = sizeof(S2C_EXP_INSRUANCE) - 1;
			rewardCacheNotify.Data           = m_ExpCache;
			
			if (g_pServer)
				g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(),&rewardCacheNotify,sizeof(rewardCacheNotify));
			
			Player[m_PlayerIndex].DirectAddExp(m_ExpCache);
			
		}
		
		ChangeExpCache(0);
		m_LastRewardTime = UNIX_TMIE_STAMP;
	}//endif

}

int KExpInsuranceMgr::CalcRewardExp(int nExp )const
{
	if (!IsValidPlayer(m_PlayerIndex))
		return 0 ;

	if (!Player[m_PlayerIndex].ExecuteScript("\\script\\main.lua",KExpQuestInsuraceSetting::Singleton().GetExpInsuranceFuncName(),nExp,1))
		return 0 ;

	return Player[m_PlayerIndex].m_nScriptResult;
}


//QuestInsuranceMgr........................................................................

KQuestInsuranceMgr::KQuestInsuranceMgr()
{
	Release();
}

KQuestInsuranceMgr::~KQuestInsuranceMgr()
{/*Do Nothing ata ll*/}

void KQuestInsuranceMgr::Init(const int nPlayerIndex )
{
	m_PlayerIndex = nPlayerIndex;
}

void KQuestInsuranceMgr::Release()
{
	m_PlayerIndex       = INVALID_PLAYER_INDEX;
	m_OfflineTimeCache  = 0;
	m_Enabled           = false;
}

void KQuestInsuranceMgr::PlayerOnline()
{
	if (!KExpQuestInsuraceSetting::Singleton().IsQuesetEnabled())
		return ;

	if (!IsValidPlayer(m_PlayerIndex))
	{
		return ;
	}//endif
	
	if (!IsEnterInsuraceState() && GetPlayerLevel(m_PlayerIndex) >= KExpQuestInsuraceSetting::Singleton().GetQuestEnableLevel())
		ReEnterInsuraceState();
	
	SyncQuestInsuranceState();
	SyncOfflineTimeCache();
	OnlineReward();
}

void KQuestInsuranceMgr::SetData(const int nOffLineCache,const bool bEnable)
{
	m_OfflineTimeCache = nOffLineCache;
	m_Enabled          = bEnable;
}

void KQuestInsuranceMgr::GetData(int & nOffLineCache , bool & bEnable)
{
	nOffLineCache      = m_OfflineTimeCache;
	bEnable            = m_Enabled;
}

void KQuestInsuranceMgr::NotifyLevelUp(const int nLevel )
{
	if (!KExpQuestInsuraceSetting::Singleton().IsQuesetEnabled())
		return ;

	if (!IsValidPlayer(m_PlayerIndex))
		return;
	
	if ( nLevel == KExpQuestInsuraceSetting::Singleton().GetQuestEnableLevel())
		ReEnterInsuraceState();
}

bool KQuestInsuranceMgr::IsEnterInsuraceState() const 
{
	return m_Enabled;
}

int  KQuestInsuranceMgr::GetOfflineTimeCache() const
{
	return m_OfflineTimeCache;
}

void KQuestInsuranceMgr::AddOfflineTimeCache( int nTime )
{
	if ( nTime <= 0)
		return;

	if (!KExpQuestInsuraceSetting::Singleton().IsQuesetEnabled() || !IsEnterInsuraceState())
		return;

	int nCurrent = m_OfflineTimeCache;
	int nMax     = KExpQuestInsuraceSetting::Singleton().GetMaxOfflineReward();

	if (nCurrent < 0 || nCurrent >= nMax)
		return;

	int  nNewOfflineCache      = m_OfflineTimeCache + nTime;
	if ( nNewOfflineCache <= 0 || nNewOfflineCache > nMax ) // too big
	{
		nNewOfflineCache       = nMax;
	}//endif
	
	ChangeOfflineTimeCache(nNewOfflineCache);

}

void KQuestInsuranceMgr::ReEnterInsuraceState()
{
	if ( IsEnterInsuraceState())
		return;
	
	if (!IsValidPlayer(m_PlayerIndex))
		return;

	ChangeRewardState( true );
	ChangeOfflineTimeCache( 0 ) ;

	S2C_QUEST_INSURANCE rewardCacheNotify;
	rewardCacheNotify.Protocol       = s2c_byte_extend;
	rewardCacheNotify.ProtocolExtend = s2c_ex_protocol_quest_insurnace;
	rewardCacheNotify.SubProtocol    = s2c_quest_insurance_enter_notify;
	rewardCacheNotify.wProtocolSize  = sizeof(S2C_QUEST_INSURANCE) - 1;
	rewardCacheNotify.Data           = m_OfflineTimeCache;
	
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(),&rewardCacheNotify,sizeof(rewardCacheNotify));

}

void KQuestInsuranceMgr::LeaveInsuranceState()
{
	ChangeRewardState( false );
	ChangeOfflineTimeCache( 0 ) ;
}


void KQuestInsuranceMgr::ChangeOfflineTimeCache(const int nTime)
{
	if (nTime != m_OfflineTimeCache)
	{
		m_OfflineTimeCache = nTime;
		SyncOfflineTimeCache();
	}//endif

}

void KQuestInsuranceMgr::ChangeRewardState(bool bEnable )
{
	if ( bEnable != m_Enabled)
	{
		m_Enabled = bEnable;
		SyncQuestInsuranceState();
	}//endif

}

void KQuestInsuranceMgr::SyncOfflineTimeCache()
{
	if (!IsValidPlayer(m_PlayerIndex))
		return;

	S2C_QUEST_INSURANCE rewardCacheNotify;
	rewardCacheNotify.Protocol       = s2c_byte_extend;
	rewardCacheNotify.ProtocolExtend = s2c_ex_protocol_quest_insurnace;
	rewardCacheNotify.SubProtocol    = s2c_quest_insurance_sync_reward;
	rewardCacheNotify.wProtocolSize  = sizeof(S2C_QUEST_INSURANCE) - 1;
	rewardCacheNotify.Data           = m_OfflineTimeCache;
	
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(),&rewardCacheNotify,sizeof(rewardCacheNotify));
}

void KQuestInsuranceMgr::SyncQuestInsuranceState()
{
	if (!IsValidPlayer(m_PlayerIndex))
		return;

	S2C_QUEST_INSURANCE rewardCacheNotify;
	rewardCacheNotify.Protocol       = s2c_byte_extend;
	rewardCacheNotify.ProtocolExtend = s2c_ex_protocol_quest_insurnace;
	rewardCacheNotify.SubProtocol    = s2c_quest_insurance_sync_state;
	rewardCacheNotify.wProtocolSize  = sizeof(S2C_QUEST_INSURANCE) - 1;
	rewardCacheNotify.Data           = m_Enabled;
	
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(),&rewardCacheNotify,sizeof(rewardCacheNotify));
}

#define INVALID_OFFLINE_INTERVAL 60

void KQuestInsuranceMgr::OnlineReward()
{
	if (!KExpQuestInsuraceSetting::Singleton().IsQuesetEnabled() || !IsEnterInsuraceState())
		return;

	if (!IsValidPlayer(m_PlayerIndex))
		return;

	DWORD dwLastPlayingTime = Player[m_PlayerIndex].m_dwLastOfflineTime;
	if ( UNIX_TMIE_STAMP < dwLastPlayingTime)
		return; //data invalid!

	int   nRealOffLine      = (int)(UNIX_TMIE_STAMP - dwLastPlayingTime);
	int   nRewardOffline    = m_OfflineTimeCache;

	if ( nRewardOffline <= 0)
		return; // data invalid

	if ( nRealOffLine < INVALID_OFFLINE_INTERVAL && nRealOffLine >= 0)
		return ;

	int  nRewardExp        = 0;
	int  nDelOfflineCache  = 0;

	if ( nRealOffLine >= 0 ) 
	{
		if (Player[m_PlayerIndex].ExecuteScript2Param(	g_FileName2Id("\\script\\main.lua"),KExpQuestInsuraceSetting::Singleton().GetQuestInsuranceFuncName(),1,nRealOffLine,nRewardOffline))
		{
			nRewardExp       = Player[m_PlayerIndex].m_nScriptResult;
		}//endif
		else
			nRewardExp   = 0;
		
		nDelOfflineCache = nRealOffLine;
	}//endif
	else
	{
		//Offline time too long
		nRewardExp       = 0;
		nDelOfflineCache = m_OfflineTimeCache ;
	}//end else
	
	//1.GiveReward Exp
	if ( nRewardExp > 0 && nRewardExp < MAX_ADD_EXP )
	{
		if (nRewardExp >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_exp_amount) && g_pLogSystem)
		{
			LogEventParam addExpEvent;
			addExpEvent.event  = log_event_quest_insurance_add_reward;
			addExpEvent.param1 = Player[m_PlayerIndex].GetGUID();
			addExpEvent.param4 = nRewardExp;
			g_pLogSystem->Log(addExpEvent);
		}//endif
		
		S2C_QUEST_INSURANCE rewardCacheNotify;
		rewardCacheNotify.Protocol       = s2c_byte_extend;
		rewardCacheNotify.ProtocolExtend = s2c_ex_protocol_quest_insurnace;
		rewardCacheNotify.SubProtocol    = s2c_quest_insurance_fetch_reward;
		rewardCacheNotify.wProtocolSize  = sizeof(S2C_QUEST_INSURANCE) - 1;
		rewardCacheNotify.Data           = nRewardExp;
		
		if (g_pServer)
			g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(),&rewardCacheNotify,sizeof(rewardCacheNotify));
		
		Player[m_PlayerIndex].DirectAddExp(nRewardExp);

	}//endif

	//2.Del current reward offline cache
	if ( nDelOfflineCache > 0)
	{		
		int nNewOfflineCache   = 0;
		if (nDelOfflineCache  >= m_OfflineTimeCache)
			nNewOfflineCache   = 0;
		else
			nNewOfflineCache   = m_OfflineTimeCache - nDelOfflineCache;
		
		ChangeOfflineTimeCache(nNewOfflineCache);
	}//endif
	
}

#endif