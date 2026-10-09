 //////////////////////////////////////////////////////////////////////////
//
//
#include "KCore.h"
#include "KCreature.h"
#include "KMath.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KProtocol.h"
#include "KSubWorldSet.h"
#include "buff_man.h"
#include "SkillManager.h"
#include "KSkills.h"

#ifndef _SERVER
#include "Coreshell.h"
#endif

#ifdef _SERVER
// 为方便修改代码
#define AI_FOLLOW_PRESERVE m_CreatureNpc->m_AiParam[0] // 与召唤者保持的距离
//#define AI_CHGMARK_DISTANCE  m_CreatureNpc->m_AiParam[1] // 主人与标记者之间的距离上限
#define AI_FOLLOW_TRANSPOT m_CreatureNpc->m_AiParam[2] // 与召唤者保持的距离
#define AI_CREATURE_TYPE m_CreatureNpc->m_AiParam[3] // 宝宝类型，0治疗 1攻击
//#define AI_USE_SKILL_1 m_CreatureNpc->m_AiParam[3] // 施用技能的概率
//#define AI_USE_SKILL_2 m_CreatureNpc->m_AiParam[4] // 施用技能的概率
//#define AI_USE_SKILL_3 m_CreatureNpc->m_AiParam[5] // 施用技能的概率
//#define AI_USE_SKILL_4 m_CreatureNpc->m_AiParam[6] // 施用技能的概率

#define	CREATURE_TYPE_CURE		0
#define CREATURE_TYPE_ATTACK	1
#define	MAX_TARGET_DISTANCE		1024
//////////////////////////////////////////////////////////////////////////
// 千分比随机
static inline BOOL Random_permillage(unsigned int nPermillage)
{
	return(g_Random(1000) < nPermillage);
}
//////////////////////////////////////////////////////////////////////////
#endif


//////////////////////////////////////////////////////////////////////////
// 构造
KCreature::KCreature()
{
	m_CreatureNpc = NULL; //召唤兽(自己)
	m_SummonerNpc = NULL; //召唤师(主人)

	m_nSkillID = 0;
	m_nLastCanSummonTime = 0;
#ifdef _SERVER
	m_nLevel = 0;
	m_MarkNpcIdx = 0;
#endif
#ifdef _SERVER
	m_IsAIOn = true;
#endif
}
//////////////////////////////////////////////////////////////////////////
//

void KCreature::Summon(const SUMMONPARAM& Param)
{
	
	m_SummonerNpc = &Npc[Param.nSummonerIdx];
	m_CreatureNpc = &Npc[Param.nCreatureIdx];

	m_CreatureNpc->SetTarget(type_npc, 0);

#ifdef _SERVER	
	m_nLevel = Param.nLevel;
//	m_CreatureNpc->SetActiveSkill(0);
	m_CreatureNpc->SetSummonerIdx(m_SummonerNpc->m_Index);
	m_nSkillID = Param.nSkillID;

	//Player[m_SummonerNpc->GetPlayerIdx()].m_ItemList.ApplayAttribToCreature();

	//添加装备BUFF
	Player[m_SummonerNpc->GetPlayerIdx()].GetItemList().SetupEquipBuffToNpc(m_CreatureNpc->m_Index);

	m_CreatureNpc->m_UnaryAttrMgr.Set(nuai_curlife, m_CreatureNpc->m_CompAttrMgr[ncai_lifeuplimit]);

//	GetEchoDamage(&m_nMinDamage, &m_nMaxDamage, &m_nAR);		
	SyncCreature();

	m_MarkNpcIdx = Param.nSummonerIdx;
	m_CanUseSkillFlag = true;

	//召唤兽的名字颜色与其主人保持同步
	int summerTitleColor = m_SummonerNpc->m_UnaryAttrMgr[nuai_titlecolor];
	m_CreatureNpc->SetNpcTitleColor(summerTitleColor);

#else
	int playerIndex = m_SummonerNpc->GetPlayerIdx();
	if(playerIndex == 0)
	{
		return;
	}

	if(playerIndex == CLIENT_PLAYER_INDEX)
	{
		char petName[COMMON_CLIENT_MSG_LEN_128];
		KSkill* pSkill = g_SkillManager.GetSkill(Param.nSkillID);
		if (pSkill != NULL)
		{
			sprintf(petName, "%s", pSkill->m_szName);
		}
		else
		{
			sprintf(petName, "%s", m_SummonerNpc->Name);
		}
		int petNpcIndex = Param.nCreatureIdx;
		CoreDataChanged(GDCNI_PET_CALLED_OUT, (unsigned int)petName, petNpcIndex);
	}
#endif
}

#ifndef _SERVER
//////////////////////////////////////////////////////////////////////////
// 收回
void KCreature::Dismiss(void)
{
	if(m_SummonerNpc)
	{
		int playerIndex = m_SummonerNpc->GetPlayerIdx();
		if(playerIndex == 0)
		{
			return;
		}
		
		if(playerIndex == CLIENT_PLAYER_INDEX)
		{
			CoreDataChanged(GDCNI_PET_RELEASE, 0, 0);
		}
	}

	if (m_CreatureNpc != NULL)
	{
		m_CreatureNpc->PolyMorph(-1,1,0,-1,0);
	}
	if ( m_SummonerNpc )
	{
		m_SummonerNpc->SetHeadInfoChanged( true );
	}
	m_CreatureNpc = NULL;	
	m_nSkillID = 0;
	m_SummonerNpc = NULL;
	m_nLastCanSummonTime = 0;

	return;	
}
#else
//////////////////////////////////////////////////////////////////////////
// 收回
void KCreature::Dismiss(void)
{
	if (m_CreatureNpc && m_SummonerNpc)
	{
		//filter npc death
		//===================================================
		BuffMgr& BM = BuffMgr::Singleton( );
		BUFF_ENV_PARAM Env;
		Env.nEventFormat	=	buff_event_format_event;
		Env.nEventSender	=	m_SummonerNpc->m_Index;
		Env.nEventRecever	=	m_CreatureNpc->m_Index;

		Env.nEventType		=	buff_event_type_npcdeathin;
		Env.nEventRelation	=	buff_event_relation_recver;
		BM.FilterEvent( Env );
		//===================================================

		BM.ClearAllBuff(m_CreatureNpc->m_Index);

		const int nCreatureIdx = m_CreatureNpc->m_Index;
		const int nSubWorld = m_CreatureNpc->m_SubWorldIndex;
		const int nRegion = m_CreatureNpc->m_RegionIndex;
		if(nRegion >= 0)
		{
			SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(nCreatureIdx);
		}

		// 不要将这一行移到后面去
		ClearMark();

		NpcSet.Remove(nCreatureIdx);

		m_nSkillID = 0;
		SyncCreature();	
		m_SummonerNpc = NULL;
		m_CreatureNpc = NULL;
	}
}
#endif

#ifdef _SERVER
void KCreature::SyncCreature(void)
{
	if (m_CreatureNpc != NULL)
	{
		CREATURE_SYNC aCreatureSync;
		aCreatureSync.ProtocolType = s2c_sync_creature;
		aCreatureSync.wSkillID = m_nSkillID;
		aCreatureSync.wMaxLife = m_CreatureNpc->m_CompAttrMgr[ncai_lifeuplimit];
		aCreatureSync.wCurrentLife = m_CreatureNpc->m_UnaryAttrMgr[nuai_curlife];
		
		// add by chenshanglin on 2006-2-21 for new skill system
		aCreatureSync.wCreatureSkillID = m_CreatureNpc->m_ActiveSkillID;
		// add end

		if (NULL != m_SummonerNpc) 
		{
			const int nPlayerIdx = m_SummonerNpc->GetPlayerIdx();

			if (nPlayerIdx > 0)
			{
				if (g_pServer != NULL)
					g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx,&aCreatureSync,sizeof(aCreatureSync));
			}
		}
	}
}
#endif

#ifdef _SERVER
//////////////////////////////////////////////////////////////////////////
// 依据当前命令处理行为
void KCreature::ProcessAI(void)
{
	_ASSERT(m_SummonerNpc);
	_ASSERT(m_CreatureNpc);
	_ASSERT( IsValidNpc(m_MarkNpcIdx) );

	if ( m_CreatureNpc == NULL || 
		m_SummonerNpc == NULL || 
		!IsValidNpc(m_MarkNpcIdx) )
	{
		Dismiss();
		return;
	}

	if ((m_CreatureNpc->m_Doing == do_revive)||(m_CreatureNpc->m_Doing == do_death))
	{
		Dismiss();
		return;
	}

	if( !m_IsAIOn )
		return;

	// 计算距离	
	int	nDistance = NpcSet.GetDistance(m_SummonerNpc->m_Index, m_CreatureNpc->m_Index);
	
	if(nDistance > AI_FOLLOW_TRANSPOT)
	{
		int x, y;
		m_SummonerNpc->GetMpsPos(&x, &y);
		m_CreatureNpc->SetPos(x, y);	
		MarkToPlayer(m_SummonerNpc->m_Index);
	}
	else
	{
		int	nTarget;

		if(AI_CREATURE_TYPE == CREATURE_TYPE_CURE)
			nTarget = m_MarkNpcIdx;
		else
		{
			nTarget = m_CreatureNpc->GetTargetNpc();
			if (!IsTargetValid(nTarget))
				m_CreatureNpc->SetTarget(type_npc, 0);
		}

		if( m_CanUseSkillFlag && IsTargetValid(nTarget) )
		{
			if (do_death == Npc[nTarget].m_Doing || do_revive == Npc[nTarget].m_Doing)
				m_CreatureNpc->SetTarget(type_npc, 0);
			else
			{
				int nSkillIdx = SelectUsableSkill(nTarget);

				if(INVALID_SKILL_INDEX != nSkillIdx)
				{
					m_CreatureNpc->SetActiveSkill(nSkillIdx);
					int nDistanceEnemy =  NpcSet.GetDistance(m_CreatureNpc->m_Index, nTarget); 

					if (nDistanceEnemy < m_CreatureNpc->m_CompAttrMgr[ncai_attackradius]) // 可以打了
					{
						m_CreatureNpc->SendCommand(do_skill, m_CreatureNpc->m_ActiveSkillID, -1, nTarget);
					}
					else
					{
						if(nDistance < MAX_TARGET_DISTANCE)
						{
							int nDesX, nDesY;
							Npc[nTarget].GetMpsPos(&nDesX, &nDesY);
							m_CreatureNpc->SendCommand(do_run, nDesX, nDesY);
						}
					}
				}
			}			
		}
		else
		{
			nDistance = NpcSet.GetDistance(m_MarkNpcIdx, m_CreatureNpc->m_Index);
				
			if(nDistance > AI_FOLLOW_PRESERVE)
			{
				int x, y;
				Npc[m_MarkNpcIdx].GetMpsPos(&x, &y);
				m_CreatureNpc->SendCommand(do_run, x, y);
			}
		}
	}
}
#endif

#ifdef _SERVER
bool KCreature::MarkToPlayer(int nNpcIdx)
{
	if( IsValidNpc(nNpcIdx) && Npc[nNpcIdx].IsPlayer() )
	{
		if(NULL != m_CreatureNpc && NULL != m_SummonerNpc)
		{
			ClearMark();

			m_MarkNpcIdx = nNpcIdx;
			m_CreatureNpc->SetTarget(type_npc, 0);

			if(m_MarkNpcIdx != m_SummonerNpc->m_Index)
			{
				int nPlayerIdx = Npc[m_MarkNpcIdx].GetPlayerIdx();
				Player[nPlayerIdx].MarkToCreature(m_CreatureNpc->m_Index);
			}

			return true;
		}
	}
	
	return false;
}

void KCreature::ClearMark()
{
	if( IsValidNpc(m_MarkNpcIdx) && Npc[m_MarkNpcIdx].IsPlayer() )	
	{
		if(m_MarkNpcIdx != m_SummonerNpc->m_Index)
		{
			int nMarkPlayer = Npc[m_MarkNpcIdx].GetPlayerIdx();
			Player[nMarkPlayer].ClearMarkCreature();
		}
	}

	m_MarkNpcIdx = 0;
	m_CreatureNpc->SetTarget(type_npc, 0);
}	

#endif

#ifdef _SERVER
bool KCreature::IsTargetValid(int nTargetIdx)
{
	if( IsValidNpc(nTargetIdx) )
	{
		int nRelation = NpcSet.GetRelation(m_MarkNpcIdx, nTargetIdx);

		if(CREATURE_TYPE_CURE == AI_CREATURE_TYPE)
			return (nRelation & relation_ally) || (nRelation & relation_self);
		else if(CREATURE_TYPE_ATTACK == AI_CREATURE_TYPE)
			return (nRelation & relation_enemy) ? true : false;
	}

	return false;
}
#endif

#ifdef _SERVER
int KCreature::SelectUsableSkill(int nTargetIdx)
{
	NpcSkillList &skillList = m_CreatureNpc->m_SkillList;

	for(int nSkillIdx = 0; nSkillIdx < 2; ++nSkillIdx)
	{
		int nSkillId = skillList.GetIdByIdx(nSkillIdx);
	
		if(INVALID_SKILL_ID == nSkillId)
			continue;
		
		if( !skillList.CanCast(nSkillId) )
			continue;

		// 有可能是群攻技能，下面的判断就不要了
// 		int nLevel = skillList.GetLevelByIdx(nSkillIdx);
// 		KSkill *pSkill = g_SkillManager.GetSkill(nSkillId, nLevel);
// 
// 		if(NULL == pSkill)		
// 			continue;
// 		
// 		int nParam1 = SKILL_SPT_TargetIndex;
// 		int nParam2 = nTargetIdx;
// 		if( !pSkill->CanCastSkill(m_CreatureNpc->m_Index, nParam1, nParam2) )
// 			continue;

		return nSkillIdx;
	}

	return INVALID_SKILL_INDEX;
}
#endif

#ifdef _SERVER
void KCreature::Reset()
{
	if (m_SummonerNpc)
	{
		MarkToPlayer(m_SummonerNpc->m_Index);
	}
}
#endif














