#include "KCore.h"
#include "KServerFuryMgr.h"
#include "ConfigManager.h"
#include "common_fury_def.h"
#include "KPlayer.h"
#include "buff_man.h"
#include "KSubWorldSet.h"

extern unsigned long UNIX_TMIE_STAMP;
#define SF_INVALID_PLAYER_INDEX -1
#define SF_DEFAULT_MIN_KILL_NUM  1 

KServerFuryMgr::KServerFuryMgr()
:m_PlayerIndex(SF_INVALID_PLAYER_INDEX),m_CurFuryExp(0)
,m_CurCheckTimeStamp(0),m_KilledInterval(0),m_CurKeepTimeStamp(0)
{/**/}

void KServerFuryMgr::Init(const int nPlayerIndex)
{
   m_PlayerIndex          = nPlayerIndex;
   m_CurFuryExp           = 0;
   m_CurCheckTimeStamp    = UNIX_TMIE_STAMP;
   m_CurKeepTimeStamp     = 0;
   m_KilledInterval       = 0;

   SyncFuryExpToClient();
}

void KServerFuryMgr::Release()
{
   m_PlayerIndex         = SF_INVALID_PLAYER_INDEX;
   m_CurFuryExp          = 0;
   m_CurCheckTimeStamp   = 0;
   m_CurKeepTimeStamp    = 0;
   m_KilledInterval      = 0;
}

#define SERVER_FURY_ACITVE_INTERVAL GAME_FPS

void KServerFuryMgr::Active()
{ 
	if (g_SubWorldSet.GetGameTime() % SERVER_FURY_ACITVE_INTERVAL == 0)
	{
		if (IsValidPlayer(m_PlayerIndex) )
		{
			KNpc &playerNpc = Npc[Player[m_PlayerIndex].GetNpcIndex()];
			
			if (!playerNpc.IsValid())
				return;
			
			ConfigManager &CMgr = ConfigManager::Singleton();
			
			//在角色使用爆魂技能时不会积攒爆魂.......................................
			BuffMgr & buffMgr = BuffMgr::Singleton();
			if(buffMgr.IsHaveBuff(Player[m_PlayerIndex].GetNpcIndex(),CMgr.GetGlobalVariable(global_var_fury_skill_id)))
			{
				return ;
			}//endif
			
			if (m_CurFuryExp != 100)  // Check state
			{
				if (UNIX_TMIE_STAMP - m_CurCheckTimeStamp >= CMgr.GetGlobalVariable(global_var_fury_check_interval))
				{
					if (m_KilledInterval >= CMgr.GetGlobalVariable(global_var_fury_check_kill_num)
						&& m_KilledInterval >= SF_DEFAULT_MIN_KILL_NUM)
					{
						m_CurFuryExp +=1;
						SyncFuryExpToClient();
						
						if (m_CurFuryExp == 100 )
						{
							//Get into the check state..........
							IntoKeepState();
						}//endif
						
					}//endif
					
					m_KilledInterval    = 0;
					m_CurCheckTimeStamp = UNIX_TMIE_STAMP;
				}//endif
				
			}//endif
			
		}//endif

	}//endif
}

void KServerFuryMgr::NpcKillingNotify()
{
    m_KilledInterval += 1;
}

void KServerFuryMgr::CleanFuryExp()
{
	m_CurFuryExp      = 0;
	IntoCheckState();
	SyncFuryExpToClient();	
	//********************************************************
	//可以在这里加入角色是否有爆魂技能的判断，如果有，需要删除
	//********************************************************
}

int KServerFuryMgr::GetFuryExp()
{
	return m_CurFuryExp;
}

void KServerFuryMgr::SetCurFuryExp(int nFury)
{
	if (nFury>=0 && nFury <=100)
	{
		bool bChanged = (m_CurFuryExp != nFury);

		m_CurFuryExp = nFury;
		SyncFuryExpToClient();

        if (nFury == 0 && bChanged)
		{
			IntoCheckState();
		}//endif

		if (nFury == 100 && bChanged)
		{
			IntoKeepState();
		}//endif	

	}//endif
}

void KServerFuryMgr::FuryExplode()
{
	if (m_CurFuryExp == 100)
	{
		m_CurFuryExp = 0;	
		//Add skill.......
		ConfigManager &CMgr = ConfigManager::Singleton();
		BuffMgr & buffMgr = BuffMgr::Singleton();
        buffMgr.AddNpcBuff(Player[m_PlayerIndex].GetNpcIndex(),Player[m_PlayerIndex].GetNpcIndex(),CMgr.GetGlobalVariable(global_var_fury_skill_id));
		IntoCheckState();		
		SyncFuryExpToClient();	
	}//endif
}

void KServerFuryMgr::IntoKeepState()
{
    m_CurKeepTimeStamp     =  UNIX_TMIE_STAMP;

	int nBeginBuffId = ConfigManager::Singleton().GetGlobalVariable(global_var_fury_full_buff);
	if (nBeginBuffId != 0)
	{
		int nNpcIndex = Player[m_PlayerIndex].GetNpcIndex();
		BuffMgr::Singleton().AddNpcBuff(nNpcIndex,nNpcIndex,nBeginBuffId);
	}//endif
						
}

void KServerFuryMgr::IntoCheckState()
{
	m_CurKeepTimeStamp  = 0;
	m_CurCheckTimeStamp = UNIX_TMIE_STAMP;
	m_KilledInterval    = 0;

	int nBeginBuffId = ConfigManager::Singleton().GetGlobalVariable(global_var_fury_end_buff);
	if (nBeginBuffId != 0)
	{
		int nNpcIndex = Player[m_PlayerIndex].GetNpcIndex();
		BuffMgr::Singleton().Singleton().AddNpcBuff(nNpcIndex,nNpcIndex,nBeginBuffId);
	}//endif						
}

void KServerFuryMgr::SyncFuryExpToClient()
{
	if (m_PlayerIndex != SF_INVALID_PLAYER_INDEX)
	{
		S2C_FURY_SYNC       furysync;
		furysync.ProtocolType     = s2c_fury_sync;
		furysync.SubProtocolType  = s2c_fury_exp_sync;
		furysync.nAdditionalParam = m_CurFuryExp;
		
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(Player[m_PlayerIndex].m_nNetConnectIdx, &furysync, sizeof(furysync));
	}//endif
}

void KServerFuryMgr::SyncFuryWarningToClient(int nNum)
{
	if (m_PlayerIndex!= SF_INVALID_PLAYER_INDEX)
	{
		S2C_FURY_SYNC       furysync;
		furysync.ProtocolType     = s2c_fury_sync;
		furysync.SubProtocolType  = s2c_fury_warning;
		furysync.nAdditionalParam = nNum;
		
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(Player[m_PlayerIndex].m_nNetConnectIdx, &furysync, sizeof(furysync));
	}//endif
}

KServerFuryMgr::~KServerFuryMgr()
{
	Release();
}