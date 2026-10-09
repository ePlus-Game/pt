//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-26
//      File_base        : ai_controller
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : NPC控制器
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "ai_controller.h"
#include "GameDataDef.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "KSkills.h"
#include "KMath.h"
#include "KSubWorldSet.h"
#include "ConfigManager.h"

NpcController::NpcController()
{
}

NpcController::~NpcController()
{
}

void NpcController::Init(int npcIndex)
{
	if (npcIndex > 0)
	{
		m_NpcIndex = npcIndex;
		m_SkillStrategyCount = 0;
		memset(m_SkillStrategyList, 0, sizeof(m_SkillStrategyList));
	}
	
	m_IsActive = false;	
	m_NpcState = npc_state_guard;
	m_OriginalState = npc_state_guard;	
	m_TargetNpcIndex = 0;
	m_ForceTargetNpcIndex = 0;
	m_FollowNpcIndex = 0;
	m_FollowNpcId = 0;
	memset(&m_FollowPlayerGuid, 0, sizeof(m_FollowPlayerGuid));
	m_FollowWaitTime = 0;
	m_ReturnTime = 0;
	m_NextWanderChangeTime = 0;
	m_ForceInstantReturnTime = 0;
	m_BirthPosX = 0;
	m_BirthPosY = 0;
	m_OriginalPosX = 0;
	m_OriginalPosY = 0;	
	m_ThreatMonitor.Reset();
	m_IsWanderRested = false;
	m_WanderRestPercentage = 50;	
	m_EnableReturn = false;
	m_EnableChase = true;
	m_IsWander = false;
	m_IsStayAround = false;
	m_NpcReturnBuff = 0;
	m_Mode = ai_mode_defend;
	m_NextAiTime = 0;	
}

void NpcController::Active()
{
	if (!m_IsActive)
		return;

	if (!IsValidNpc(m_NpcIndex))
		return;

	KNpc& npc = Npc[m_NpcIndex];

	//只有在没有敌人的情况下才需要省略一些AI处理
	if (m_ThreatMonitor.GetEnemyCount() == 0)
	{
		//判断是否应该进行一次新的AI处理
		unsigned long currentTime = GetCurrentTime();
		if (m_NextAiTime > currentTime)
		{
			return;
		}
		else
		{
			m_NextAiTime = currentTime + ConfigManager::Singleton().GetGlobalVariable(global_var_next_ai_delay);
		}
	}

	//根据NPC状态进行相应的AI处理
	switch(m_NpcState)
	{
	case npc_state_none:
		break;
	case npc_state_guard:
		ProcessStateGuard();
		break;	
	case npc_state_follow:
		ProcessStateFollow();
		break;
	case npc_state_combat:
		ProcessStateCombat();
		break;
	case npc_state_flee:
		ProcessStateFlee();
		break;
	case npc_state_patrol:
		ProcessStatePatrol();
		break;
	case npc_state_trapped:
		ProcessStateTrapped();
		break;
	case npc_state_return:
		ProcessStateReturn();
		break;
	case npc_state_follow_wait:
		ProcessStateFollowWait();
		break;
	default:
		break;
	}
}

void NpcController::OnEvent(enumNpcEvent npcEvent, void* eventParam)
{
	switch(npcEvent)
	{
	case NpcEvent_BeUsedSkill:
		if (m_NpcState == npc_state_combat)
		{
			RefreshReturnTime();
		}
		break;
	}
}

int NpcController::GetTargetNpc()
{
	if (m_ForceTargetNpcIndex > 0)
	{
		m_TargetNpcIndex = m_ForceTargetNpcIndex;
	}
	else if (m_ThreatMonitor.GetEnemyCount() > 0)
	{
		int enemyCount = m_ThreatMonitor.GetEnemyCount();
		for(int enemyListLoopCount = 0; enemyListLoopCount < enemyCount; enemyListLoopCount++)
		{
			const PNpcThreatInfo pThreatInfo = m_ThreatMonitor.GetEnemyThreatInfo(enemyListLoopCount);
			if (pThreatInfo != NULL)
			{
				if (!pThreatInfo->IsHidden)
				{
					if (m_TargetNpcIndex == 0)
					{
						m_TargetNpcIndex = pThreatInfo->NpcIndex;
					}
					else
					{
						if (pThreatInfo->NpcIndex != m_TargetNpcIndex)//如果威胁列表第一个可见的敌人不是当前目标，则需要计算是否OT
						{
							const PNpcThreatInfo pTargetThreatInfo = m_ThreatMonitor.GetEnemyThreatInfo(m_ThreatMonitor.GetEnemyListIndex(Npc[m_TargetNpcIndex].GetId()));
							if (pTargetThreatInfo != NULL)
							{
								//OT了
								if (pThreatInfo->Threat >= pTargetThreatInfo->Threat * ConfigManager::Singleton().GetGlobalVariable(global_var_over_threat_factor) / 100)
								{
									m_TargetNpcIndex = pThreatInfo->NpcIndex;
								}
							}
						}
					}

					break;
				}
			}
		}
	}
	else
	{
		m_TargetNpcIndex = 0;
	}

	return m_TargetNpcIndex;
}

void NpcController::SetForceTargetNpc(int npcIndex)
{
	m_ForceTargetNpcIndex = npcIndex;
}

void NpcController::SetFollowNpc(int npcIndex, int followWaitTime)
{
	if (IsValidNpc(npcIndex))
	{
		m_FollowNpcIndex = npcIndex;
		m_FollowNpcId = Npc[npcIndex].GetId();
		if (Npc[npcIndex].IsPlayer())
		{
			int playerIndex = Npc[npcIndex].GetPlayerIdx();
			if (IsValidPlayer(playerIndex))
			{
				m_FollowPlayerGuid = Player[playerIndex].GetGUID();
			}
		}
		m_FollowWaitTime = followWaitTime;

		SetState(npc_state_follow);
	}
	else
	{
		m_FollowNpcIndex = 0;
		m_FollowNpcId = 0;
		memset(&m_FollowPlayerGuid, 0, sizeof(m_FollowPlayerGuid));
		m_FollowWaitTime = 0;

		SetState(npc_state_guard);
	}
}

void NpcController::SetState(enumNpcState state)
{
	m_NpcState = state;
}

void NpcController::ProcessStateGuard()
{
	KNpc& npc = Npc[m_NpcIndex];

	unsigned long currentTime = GetCurrentTime();

	if (m_BirthPosX == 0 && m_BirthPosY == 0)
	{
		npc.GetMpsPos(&m_BirthPosX, &m_BirthPosY);
	}

	if (ai_mode_aggressive == m_Mode)//如果富有攻击性，则会主动寻找附近的敌人
	{
		int nearestEnemyIndex = GetNearestNpc(relation_enemy);
		if (nearestEnemyIndex > 0)//发现敌人
		{	
			m_ThreatMonitor.AddEnemy(Npc[nearestEnemyIndex].GetId(), nearestEnemyIndex);//加入威胁列表	
		}
	}

	if (m_ThreatMonitor.GetEnemyCount() > 0)
	{
		if (ai_mode_passive == m_Mode)
		{
			m_ThreatMonitor.Reset();
		}
		else
		{
			m_ReturnTime = currentTime + ConfigManager::Singleton().GetGlobalVariable(global_var_npc_return_time);//决定返回时间
			npc.GetMpsPos(&m_OriginalPosX, &m_OriginalPosY);//记录返回地点
			m_OriginalState = m_NpcState;
			
			SetState(npc_state_combat);//进入战斗状态
			return;
		}
	}
	
	if (m_IsWander)//需要漫步
	{
		if (m_NextWanderChangeTime <= currentTime)//漫步需要变向
		{
			int waitTime = 0;
			
			//随机决定是否要休息，如果已经休息过，则必定需要改变方向前进
			if (FALSE == g_RandPercent(m_WanderRestPercentage) || m_IsWanderRested)
			{
				m_IsWanderRested = false;
				int wanderRange = ConfigManager::Singleton().GetGlobalVariable(global_var_npc_wander_range);
				
				//随机决定这次变向的地点
				int posXChange = g_Random(wanderRange * 2) - wanderRange;
				int posYChange = g_Random(wanderRange * 2) - wanderRange;

				int nextPosX = 0;
				int nextPosY = 0;
								
				if (m_IsStayAround)
				{
					//需要保持在出生点周围，偏移原点应该是出生点
					nextPosX = m_BirthPosX + posXChange;
					nextPosY = m_BirthPosY + posYChange;					
				}
				else
				{
					//偏移原点是当前位置
					int currentPosX, currentPosY;
					npc.GetMpsPos(&currentPosX, &currentPosY);
					nextPosX = currentPosX + posXChange;
					nextPosY = currentPosY + posYChange;
				}

				//估算行动时间
				int runspeed = npc.m_CompAttrMgr[ncai_runspeed];
				if (runspeed <= 0)
					runspeed = 1;
				int walkTime = (int)qsqrt(NpcSet.GetDistanceSquare(nextPosX, nextPosY, m_NpcIndex)) / runspeed;
				waitTime = walkTime;

				//走吧
				npc.SendCommand(do_run, nextPosX, nextPosY);
			}
			else//需要休息
			{
				//一旦休息过，下一次必定需要改变方向前进
				m_IsWanderRested  = true;

				//随机休息时间（基本值+范围随机值）
				waitTime = ConfigManager::Singleton().GetGlobalVariable(global_var_base_next_wander_change_time) + g_Random(ConfigManager::Singleton().GetGlobalVariable(global_var_max_next_wander_change_time));
			}

			//决定下一次变向的时间
			m_NextWanderChangeTime = currentTime + waitTime;
		}
	}
}

void NpcController::ProcessStateFollow()
{
	if (m_NpcIndex <= 0 || m_FollowNpcIndex <= 0)
		return;

	KNpc& selfNpc = Npc[m_NpcIndex];
	KNpc& followNpc = Npc[m_FollowNpcIndex];

	ConfigManager& cm = ConfigManager::Singleton();

	int distance = NpcSet.GetDistance(m_NpcIndex, m_FollowNpcIndex);

	if (followNpc.GetId() != m_FollowNpcId//已经不是以前跟随的那个NPC了
		|| followNpc.GetSubWorldIndex() != selfNpc.GetSubWorldIndex()//已经不在这个地图上了
		|| !followNpc.IsAlive()//死咯
		|| distance > cm.GetGlobalVariable(global_var_ai_npc_follow_max_range)//距离太远了
		)
	{
		m_FollowNpcIndex = 0;
		m_FollowNpcId = 0;

		//脱离Follow状态，进入FollowWait状态
		if (m_FollowPlayerGuid.data[0] > 0 && m_FollowWaitTime > 0)
		{
			selfNpc.SetExpire(UNIX_TMIE_STAMP + m_FollowWaitTime);
			SetState(npc_state_follow_wait);
		}
		else
		{
			SetState(npc_state_guard);
		}

		return;
	}
	
	if (distance > cm.GetGlobalVariable(global_var_ai_npc_follow_min_range))//距离还不够接近
	{
		//进一步接近
		int followX, followY;
		followNpc.GetMpsPos(&followX, &followY);
		selfNpc.SendCommand(do_run, followX, followY);
	}
	else//距离够近了
	{
		//休息一会儿
		selfNpc.SendCommand(do_stand, 0, 0);
	}
}

void NpcController::ProcessStateCombat()
{
	KNpc& selfNpc = Npc[m_NpcIndex];	
	
	ValidateEnemy();

	unsigned long currentTime = GetCurrentTime();
	
	int targetNpcIndex = GetTargetNpc();
	selfNpc.SetTarget(type_npc, targetNpcIndex);
	if (targetNpcIndex > 0)//有目标
	{
		KNpc& targetNpc = Npc[targetNpcIndex];
		
		if (m_ReturnTime > currentTime)//追击时间没到
		{
			int distance = NpcSet.GetDistance(m_NpcIndex, targetNpcIndex);			
			int targetPosX, targetPosY, selfPosX, selfPosY;
			targetNpc.GetMpsPos(&targetPosX, &targetPosY);
			selfNpc.GetMpsPos(&selfPosX, &selfPosY);
			
			const int skillStrategyCount = GetSkillStrategyCount();
			const PSkillStrategy skillStrategyList = GetSkillStrategyList();
			
			int useableSkillId = -1;
			bool needToBeCloser = false;
			int skillStrategyLoopCount = 0;
			int useableSkillStrategyWeight = 0;
			int totalWeight = 0;
			
			for (skillStrategyLoopCount = 0; skillStrategyLoopCount < skillStrategyCount; skillStrategyLoopCount++)
			{
				int skillId = skillStrategyList[skillStrategyLoopCount].SkillId;
				enumSkillUseableResult skillUseableResult = IsSkillUseable(skillId, targetNpcIndex);
				enumSkillStrategyMode skillStrategyMode = skillStrategyList[skillStrategyLoopCount].Mode;
				
				if (skill_strage_mode_positive == skillStrategyMode)//主动
				{
					if (skill_useable_result_ok == skillUseableResult)
					{
						//就用这个了
						useableSkillId = skillId;
						break;
					}
					else if (skill_useable_result_out_of_range == skillUseableResult)
					{
						//太远了，追
						needToBeCloser = true;
						break;
					}
					else if (skill_useable_result_in_cd == skillUseableResult)
					{
						//还没好，等等吧
						break;
					}
				}
				else if (skill_strage_mode_neutral == skillStrategyMode)//中立
				{
					//把所有需要随即运算的中立技能集中起来运算，并选择一个执行
					int i = skillStrategyLoopCount;
					int totalNeutralSkillWeight = 0;
					while(i < skillStrategyCount && skill_strage_mode_neutral == skillStrategyList[i].Mode)
					{
						totalNeutralSkillWeight += skillStrategyList[i].Weight;
						i++;
					}
					
					int randomWeight = g_Random(totalNeutralSkillWeight);
					int currentNeutralSkillWeight = 0;
					for (int j = skillStrategyLoopCount; j < i; j++)
					{
						currentNeutralSkillWeight += skillStrategyList[j].Weight;
						if (randomWeight <= currentNeutralSkillWeight)
						{
							if (skill_useable_result_ok == IsSkillUseable(skillStrategyList[j].SkillId, targetNpcIndex))
							{
								useableSkillId = skillStrategyList[j].SkillId;
							}							
							break;
						}
					}					
					
					if (useableSkillId > 0)
					{
						break;
					}
				}
				else if (skill_strage_mode_passive == skillStrategyMode)//被动
				{
					if (skill_useable_result_ok == skillUseableResult)
					{
						//就用这个了
						useableSkillId = skillId;
						break;
					}
				}
			}
			
			if (useableSkillId > 0)//找到一个可以使用的技能
			{
				KSkill* pSkill = g_SkillManager.GetSkill(useableSkillId);
				if (pSkill != NULL)
				{
					//根据不同的技能类型使用技能
					switch(pSkill->GetSkillStyle())
					{				
					case SKILL_SS_PassivityNpcState:
					case SKILL_SS_Active:
						if (pSkill->GetAttackTargetType() == (att_target_only | att_target_self))
						{
							selfNpc.SendCommand(do_skill, useableSkillId, -1, m_NpcIndex);
						}
						else
						{
							selfNpc.SendCommand(do_skill, useableSkillId, -1, targetNpcIndex);
						}						
						break;					
					case SKILL_SS_NearMultiAttack:
					case SKILL_SS_AddNearTrap:
						selfNpc.SendCommand(do_skill, useableSkillId, selfPosX, selfPosY);
						break;
					case SKILL_SS_AddFarTrap:						
						selfNpc.SendCommand(do_skill, useableSkillId, targetPosX, targetPosY);
						break;
					default:
						_ASSERT(false);//使用到了不支持的技能
						break;
					}
					
					RefreshReturnTime();
				}
				
				return;
			}
			
			if (m_EnableChase && needToBeCloser)//需要靠近目标
			{				
				if (distance > ConfigManager::Singleton().GetGlobalVariable(global_var_min_npc_follow_range))//距离还不够接近，还可以进一步接近
				{
					//向目标靠近
					int targetX, targetY;
					targetNpc.GetMpsPos(&targetX, &targetY);
					selfNpc.SendCommand(do_run, targetX, targetY);
					
					return;
				}
			}
			
			//当前没有任何技能可以使用（也无法解决）
			//不管这个家伙了，我看还是逃跑吧

			return;
		}	
	}
	
	//其他任何情况都，不知道干什么了，回去吧
	m_ThreatMonitor.Reset();//清空威胁列表
	m_TargetNpcIndex = 0;//取消目标
	if (m_EnableReturn)//启用了返回
	{
		//添加返回BUFF
		int npcReturnBuff = ConfigManager::Singleton().GetGlobalVariable(global_var_buff_npc_return);
		if (npcReturnBuff > 0)
		{
			m_NpcReturnBuff = BuffMgr::Singleton().AddNpcBuff(
				m_NpcIndex, m_NpcIndex, npcReturnBuff );
			
			_ASSERT(m_NpcReturnBuff);
		}
		
		SetState(npc_state_return);//进入返回状态
		return;
	}
	else//没有启用返回
	{
		SetState(m_OriginalState);//进入初始状态
		return;
	}	
}

void NpcController::ProcessStateFlee()
{
	KNpc& npc = Npc[m_NpcIndex];
}

void NpcController::ProcessStatePatrol()
{
	KNpc& npc = Npc[m_NpcIndex];
}

void NpcController::ProcessStateTrapped()
{
	KNpc& npc = Npc[m_NpcIndex];
}

void NpcController::ProcessStateReturn()
{
	KNpc& npc = Npc[m_NpcIndex];

	unsigned long currentTime = GetCurrentTime();

	if (m_ForceInstantReturnTime == 0)//刚进入返回状态
	{
		//设置强制立即返回时间
		m_ForceInstantReturnTime = currentTime + ConfigManager::Singleton().GetGlobalVariable(global_var_force_instant_return_delay);
	}
	else//进入返回状态一定时间了
	{
		//强制立即返回时间到了
		if (m_ForceInstantReturnTime <= currentTime)
		{
			m_ForceInstantReturnTime = 0;
			npc.SetPos(m_OriginalPosX, m_OriginalPosY);//立刻把我传送回去
			if (m_NpcReturnBuff > 0)
			{
				BuffMgr::Singleton().ClearBuffByID(m_NpcIndex, m_NpcReturnBuff);
			}	
			SetState(m_OriginalState);//进入初始状态
			return;
		}
	}

	int currentPosX, currentPosY;
	npc.GetMpsPos(&currentPosX, &currentPosY);	
	int returnOriginalPosRange = ConfigManager::Singleton().GetGlobalVariable(global_var_min_npc_follow_range);
	if (abs(currentPosX - m_OriginalPosX) < returnOriginalPosRange
		&& abs(currentPosY - m_OriginalPosY) < returnOriginalPosRange)//差不多到初始地点了吧
	{
		m_ForceInstantReturnTime = 0;
		if (m_NpcReturnBuff > 0)
		{
			BuffMgr::Singleton().ClearBuffByID(m_NpcIndex, m_NpcReturnBuff);
		}		
		SetState(m_OriginalState);//进入初始状态
		return;
	}
	else
	{		
		npc.SendCommand(do_run, m_OriginalPosX, m_OriginalPosY);//赶紧跑回去
	}	
}

void NpcController::ProcessStateFollowWait()
{
	KNpc& npc = Npc[m_NpcIndex];
	if (m_FollowPlayerGuid.data[0] > 0)
	{
		//在所在Region内查找自己跟随的玩家
		int playerIndex = SubWorld[npc.m_SubWorldIndex].m_Region[npc.m_RegionIndex].FindPlayer(m_FollowPlayerGuid);
		if (playerIndex > 0)
		{
			//找到原来跟随的玩家了，继续跟随
			m_FollowNpcIndex = Player[playerIndex].GetNpcIndex();
			m_FollowNpcId = Npc[m_FollowNpcIndex].GetId();

			npc.SetExpire(0);
			SetState(npc_state_follow);
		}
	}
}

int NpcController::GetNearestNpc(int nRelation)
{
	KNpc& npc = Npc[m_NpcIndex];
	int nRangeX = npc.m_CompAttrMgr[ncai_visionradius];
	int	nRangeY = nRangeX;
	const int nSubWorld = npc.m_SubWorldIndex;
	const int nRegion = npc.m_RegionIndex;
	int	nMapX = npc.GetMapX();
	int	nMapY = npc.GetMapY();
	int	nRet;
	int	nRMx, nRMy, nSearchRegion;

	if (nSubWorld < 0 || nSubWorld >= MAX_SUBWORLD || SubWorld[nSubWorld].m_Region == NULL)
		return 0;

	const int totalRegion = SubWorld[nSubWorld].m_nTotalRegion;
	if (nRegion < 0 || nRegion >= SubWorld[nSubWorld].m_nTotalRegion)
		return 0;

	nRangeX = nRangeX / REGION_CELL_SIZE_X;
	nRangeY = nRangeY / REGION_CELL_SIZE_Y;	

	// 检查视野范围内的格子里的NPC
	for (int i = 0; i < nRangeX; i++)	// i, j由0开始而不是从-range开始是要保证Nearest
	{
		for (int j = 0; j < nRangeY; j++)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH ;
			}
			if (nSearchRegion < 0 || nSearchRegion >= totalRegion)
				continue;

			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion < 0 || nSearchRegion >= totalRegion)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_NpcIndex, nRelation);
			if (nRet > 0 && Npc[nRet].IsVisibleToNpc())
				return nRet;
			
			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH;
			}
			if (nSearchRegion < 0 || nSearchRegion >= totalRegion)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion < 0 || nSearchRegion >= totalRegion)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_NpcIndex, nRelation);
			if (nRet > 0 && Npc[nRet].IsVisibleToNpc())
				return nRet;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH;
			}
			if (nSearchRegion < 0 || nSearchRegion >= totalRegion)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion < 0 || nSearchRegion >= totalRegion)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_NpcIndex, nRelation);
			if (nRet > 0 && Npc[nRet].IsVisibleToNpc())
				return nRet;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;			
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH;
			}
			if (nSearchRegion < 0 || nSearchRegion >= totalRegion)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion < 0 || nSearchRegion >= totalRegion)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_NpcIndex, nRelation);
			if (nRet > 0 && Npc[nRet].IsVisibleToNpc())
				return nRet;
		}
	}
	return 0;
}

unsigned long NpcController::GetCurrentTime()
{
	return SubWorld[Npc[m_NpcIndex].m_SubWorldIndex].m_dwCurrentTime;
}

int NpcController::GetRandomEnemy()
{
	const PNpcThreatInfo pThreatInfo = m_ThreatMonitor.GetEnemyThreatInfo(g_Random(m_ThreatMonitor.GetEnemyCount()));
	if (pThreatInfo != NULL)
	{
		return pThreatInfo->NpcIndex;			
	}
	else
	{
		return 0;
	}
}

enumSkillUseableResult NpcController::IsSkillUseable(int skillId, int targetNpcIndex)
{
	KSkill* pSkill = g_SkillManager.GetSkill(skillId, 1);
	if(pSkill != NULL)
	{
		KNpc& selfNpc = Npc[m_NpcIndex];

		int nSkillType = pSkill->GetAttackTargetType( ) & att_target_only ? SKILL_SPT_TargetIndex : 0;
		int nParam1	=	nSkillType;
		int nParam2	=	targetNpcIndex;
		int attackTargetType = pSkill->GetAttackTargetType( );

		if( attackTargetType & att_target_only )
		{
			if (attackTargetType == (att_target_only | att_target_self))
			{
				nParam1	=	nSkillType;
				nParam2	=	m_NpcIndex;
			}
			else
			{
				nParam1	=	nSkillType;
				nParam2	=	targetNpcIndex;
			}
			
			if (FALSE == selfNpc.IsAttackTarget(nParam2, attackTargetType))
				return skill_useable_result_invalid_target;
		}
		else
		{
			selfNpc.GetMpsPos(&nParam1, &nParam2);
		}
		
		if( pSkill->CanCastSkill( m_NpcIndex, nParam1, nParam2 ) )
		{			
			int distance = NpcSet.GetDistance(m_NpcIndex, targetNpcIndex);
			if (pSkill->GetAttackRadius() == 0 || pSkill->GetAttackRadius() >= distance)
			{
				if( Npc[m_NpcIndex].Cost(pSkill, TRUE))
				{
					int skillIndex = selfNpc.GetSkillList().FindSkill(skillId);
					if (selfNpc.GetSkillList().IsCooling(skillIndex))
					{
						return skill_useable_result_in_cd;
					}
					else
					{
						return skill_useable_result_ok;
					}							
				}
				else
				{
					return skill_useable_result_no_mana;
				}
			}
			else
			{
				return skill_useable_result_out_of_range;
			}
		}
	}
	
	return skill_useable_result_unknown;
}

void NpcController::SetSkillStrategyList(const PSkillStrategy skillStrategyList, int count)
{
	m_SkillStrategyCount = 0;
	memset(m_SkillStrategyList, 0, sizeof(m_SkillStrategyList));

	if (skillStrategyList != NULL && count > 0 && count <= MAX_SKILL_STRATEGY_LIST_LENGTH)
	{
		m_SkillStrategyCount = count;
		memcpy(m_SkillStrategyList, skillStrategyList, sizeof(SkillStrategy) * count);
	}	
}

void NpcController::ValidateEnemy()
{
	KNpc& selfNpc = Npc[m_NpcIndex];

	int enemyCount = m_ThreatMonitor.GetEnemyCount();
	for(int enemyLoopCount = 0; enemyLoopCount < enemyCount; enemyLoopCount++)
	{
		const PNpcThreatInfo pThreatInfo = m_ThreatMonitor.GetEnemyThreatInfo(enemyLoopCount);
		if (pThreatInfo != NULL)
		{
			KNpc& enemyNpc = Npc[pThreatInfo->NpcIndex];
			if (enemyNpc.GetId() != pThreatInfo->NpcId//原来的那个NPC已经不存在了
				|| enemyNpc.GetSubWorldIndex() != selfNpc.GetSubWorldIndex()//已经不在这个地图上了
				|| !enemyNpc.IsVisibleToNpc()//已经不可见了
				|| !enemyNpc.IsAlive()//已经死了
				|| (NpcSet.GetDistance(m_NpcIndex, pThreatInfo->NpcIndex) > ConfigManager::Singleton().GetGlobalVariable(global_var_valid_enemy_distance))//超出有效敌人距离
				)
			{
				if (pThreatInfo->NpcIndex == m_TargetNpcIndex)
				{
					m_TargetNpcIndex = 0;					
				}

				if (pThreatInfo->NpcIndex == m_ForceTargetNpcIndex)
				{
					if (m_TargetNpcIndex == m_ForceTargetNpcIndex)
					{
						m_TargetNpcIndex = 0;
					}
					m_ForceTargetNpcIndex = 0;
				}

				m_ThreatMonitor.RemoveEnemy(pThreatInfo->NpcId);
				break;
			}
		}
	}
}

void NpcController::RefreshReturnTime()
{
	m_ReturnTime = GetCurrentTime() + ConfigManager::Singleton().GetGlobalVariable(global_var_npc_return_time);//更新返回时间
}
