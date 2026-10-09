#include "KCore.h"
#include "world_combat_instance.h"

#ifdef _SERVER
#define MAX_SETTING_KEY_LEN 128

KWorldCombatSetting & KWorldCombatSetting::Singleton()
{
	static KWorldCombatSetting mgr;
	return mgr;
}

KWorldCombatSetting::KWorldCombatSetting()
{/**/}

KWorldCombatSetting::~KWorldCombatSetting()
{/**/}

void KWorldCombatSetting::Init(void)
{	
	KIniFile  kLimitFile;
	if (!kLimitFile.Load(WORLD_COMBAT_INFO_MAX_SETTINGS))
	{
		_ASSERT(false);
		return ;
	}//endif
	
	int nLastLimitedScore = 0;

	for (int nLevelScape = 0;nLevelScape < MAX_SCORE_LEVEL ; nLevelScape ++ )
	{
		char szKey[MAX_SETTING_KEY_LEN] = "";
		sprintf(szKey,"LevelLimited_%d",nLevelScape);
		
		int nScoreLimit = kLimitFile.GetInteger(szKey,"MaxScore",nLastLimitedScore,(int *)(&m_ScoreLimited[nLevelScape]));
		
		if ( nScoreLimit != 0 && m_ScoreLimited[nLevelScape] <= 0x7fffffff)
		{
			nLastLimitedScore = m_ScoreLimited[nLevelScape];
		}//endif
		else
		{
			//failed
			m_ScoreLimited[nLevelScape] = nLastLimitedScore;
		}
		
	}//end for nLevelScape
	
	KIniFile kKillFile;
	if (!kKillFile.Load(WORLD_COMBAT_INFO_KILLED_SETTINGS))
	{
		_ASSERT(false);
        return ;
	}//endif

	int nLastKill = 0;

	for (int nKillLoop = 0;nKillLoop < MAX_SCORE_LEVEL ; nKillLoop ++ )
	{
		char szKey[MAX_SETTING_KEY_LEN] = "";
		sprintf(szKey,"LevelKill_%d",nKillLoop);
		
		int nScoreRes   = kKillFile.GetInteger(szKey,"Score",nLastKill,(int *)(&m_KillScore[nKillLoop]));
		if ( nScoreRes != 0 && m_KillScore[nKillLoop] <= 0x7fffffff)
		{
		    nLastKill   = m_KillScore[nKillLoop];
		}//endif
		else
		{
			//failed
			m_KillScore[nKillLoop] = nLastKill;
		}
		
	}//end for nLevelScape

}

int KWorldCombatSetting::GetMaxScoreByLevel(const int nPlayerLevel)
{
	int nRet = 0;

	if (nPlayerLevel >= 0)
	{
		if (nPlayerLevel < MAX_SCORE_LEVEL)
		{
			nRet = m_ScoreLimited[nPlayerLevel];
		}//endif
		else
			nRet = m_ScoreLimited[MAX_SCORE_LEVEL - 1];

	}//endif

	return nRet;
}

int KWorldCombatSetting::GetKillScoreByLevel(const int nPlayerLevel)
{
	int nRet = 0;

	if (nPlayerLevel >= 0)
	{
		if (nPlayerLevel < MAX_SCORE_LEVEL)
		{
			nRet = m_KillScore[nPlayerLevel];
		}//endif
		else
			nRet = m_KillScore[MAX_SCORE_LEVEL - 1];
	}//endif
	
	return nRet;

}

#endif
