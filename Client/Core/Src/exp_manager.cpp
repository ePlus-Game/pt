//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-3-19
//      File_base        : exp_manager
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 经验管理器
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KNpcSet.h"
#include "ConfigManager.h"
#include "exp_manager.h"

ExpManager::ExpManager()
{
	memset(m_expDistributeFunction, 0, sizeof(m_expDistributeFunction));
}

ExpManager& ExpManager::Singleton()
{
	static ExpManager manager;
	return manager;
}

bool ExpManager::Init()
{
	for (int i = 0; i < MAX_LEVEL; i++)
	{
		m_expDistributeFunction[i] = 1;
	}

	KTabFile expDistributeTabFile;
	if (TRUE == expDistributeTabFile.Load(EXP_DISTRIBUTE_TABLE_FILE))
	{
		int rowCount = expDistributeTabFile.GetHeight();
		if (rowCount >= MAX_LEVEL)
		{
			rowCount = MAX_LEVEL;
			
			for (int row = 1; row <= rowCount; ++row)
			{
				expDistributeTabFile.GetInteger(row, 1, 1, &m_expDistributeFunction[row - 1]);
			}

			return true;
		}
	}

	return false;
}

void ExpManager::DistributeExp(int expNpcIndex, int expOwnerPlayerIndex)
{
	if (!IsValidNpc(expNpcIndex) || !IsValidPlayer(expOwnerPlayerIndex))
		return;

	KNpc& expNpc = Npc[expNpcIndex];
		
	int npcOriginalExp = expNpc.m_UnaryAttrMgr[nuai_experience];
	int npcOriginalSkillExp = expNpc.m_UnaryAttrMgr[nuai_skillexp];
	if (npcOriginalExp < 0)
		npcOriginalExp = 0;
	if (npcOriginalSkillExp < 0)
		npcOriginalSkillExp = 0;
	if (npcOriginalExp == 0 && npcOriginalSkillExp == 0)
		return;

	int npcExp = npcOriginalExp;
	int npcSkillExp = npcOriginalSkillExp;

	KPlayer& expOwner = Player[expOwnerPlayerIndex];
	
	if (expOwner.GetTeamInfo().IsInTeam())//组队中
	{
		KTeam* pTeam = expOwner.GetTeamInfo().GetTeam();
		if (pTeam != NULL)
		{
			if (pTeam->HasExp())//可以获得经验
			{
				if (pTeam->IsShareExp())//分享经验
				{
					int shareExpMemberCount = 0;
					bool shareExpMember[MAX_BIG_TEAM_MEMBER];//分享经验的队伍成员
					int maxShareExpRange = ConfigManager::Singleton().GetGlobalVariable(global_var_max_share_exp_range);
					
					//计算总计等级权值和确定分享经验的队伍成员
					int totalWeight = 0;
					int maxTeammemberCount = pTeam->GetMaxMemberCount();
					for (int teammemberLoopCount = 0; teammemberLoopCount < maxTeammemberCount; teammemberLoopCount++)
					{
						shareExpMember[teammemberLoopCount] = false;
						int memberPlayerIndex = pTeam->GetMemberPlayerIndex(teammemberLoopCount);
						if (IsValidPlayer(memberPlayerIndex))
						{
							if (Npc[Player[memberPlayerIndex].GetNpcIndex()].IsAlive()
								&& (NpcSet.GetDistance(expOwner.GetNpcIndex(), Player[memberPlayerIndex].GetNpcIndex()) <= maxShareExpRange))
							{
								totalWeight += GetExpDistributeWeight(Player[memberPlayerIndex].GetLevel());
								shareExpMember[teammemberLoopCount] = true;
								shareExpMemberCount++;
							}
						}
					}
										
					if (shareExpMemberCount == 0)
					{
						return;
					}
					//只有一个人能分享经验，当作一个人打怪考虑
					else if (shareExpMemberCount == 1)
					{
						expOwner.AddExp(npcExp, expNpc.GetLevel());
						expOwner.AddSkillExp(npcSkillExp, expNpc.GetLevel());
						return;
					}
					//根据分享经验的队伍成员数量进行一定的经验加成
					else if (shareExpMemberCount <= 3)
					{
						npcExp = npcExp * (3 + 2 * shareExpMemberCount) / 5;
						npcSkillExp = npcSkillExp *  (3 + 2 * shareExpMemberCount) / 5;
					}
					else
					{				
						npcExp = npcExp * 9 / 5;
						npcSkillExp = npcSkillExp * 9 / 5;
					}
					
					if (totalWeight > 0 && shareExpMemberCount > 1)
					{
						int maxExpPerMember = npcOriginalExp * 4 / 5;//单个队员可以获得的最多经验
						int maxSkillExpPerMember = npcOriginalSkillExp * 4 / 5;
						
						for (int teammemberLoopCount2 = 0; teammemberLoopCount2 < maxTeammemberCount; teammemberLoopCount2++)
						{
							int memberPlayerIndex = pTeam->GetMemberPlayerIndex(teammemberLoopCount2);
							if (IsValidPlayer(memberPlayerIndex))
							{
								if (shareExpMember[teammemberLoopCount2])
								{
									//根据队员的等级权值获得相应的经验
									int memberWeight = GetExpDistributeWeight(Player[memberPlayerIndex].GetLevel());
									int memberExp = memberWeight * npcExp / totalWeight;
									int memberSkillExp = memberWeight * npcSkillExp / totalWeight;
									
									//获得的经验必须限制在单个队员可以获得的最多经验
									if (memberExp > maxExpPerMember)
									{
										memberExp = maxExpPerMember;
									}
									
									if (memberSkillExp > maxSkillExpPerMember)
									{
										memberSkillExp = maxSkillExpPerMember;
									}
									
									Player[memberPlayerIndex].AddExp(memberExp, expNpc.GetLevel());
									Player[memberPlayerIndex].AddSkillExp(memberSkillExp, expNpc.GetLevel());
								}
							}
						}
					}
				}
				else//不分享经验
				{
					expOwner.AddExp(npcExp, expNpc.GetLevel());
					expOwner.AddSkillExp(npcSkillExp, expNpc.GetLevel());
				}
			}
		}			
	}
	else//非组队
	{
		expOwner.AddExp(npcExp, expNpc.GetLevel());
		expOwner.AddSkillExp(npcSkillExp, expNpc.GetLevel());
	}
}