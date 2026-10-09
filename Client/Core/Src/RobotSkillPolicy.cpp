//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:3:9   11:51
//      File_base        : RobotSkillPolicy
//      File_ext         : cpp
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#include "KCore.h"

#ifdef _AUTO_ROBOT

#include <algorithm>
#include "RobotSkillPolicy.h"
#include "KPlayer.h"
#include "KSkills.h"

using namespace std;

bool CmpSkillInfo(const RobotSkillPolicy::RobotSkillInfo &lhs, const RobotSkillPolicy::RobotSkillInfo &rhs)
{
	if(lhs.totalDamage > rhs.totalDamage)
		return true;
	else if(lhs.totalDamage < rhs.totalDamage)
		return false;
	else
		return lhs.attackRadius > rhs.attackRadius;
}

bool RobotSkillPolicy::Initialize()
{
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	NpcSkillList&	skillList = Npc[nPlayerNpcIdx].m_SkillList;

	int	nSkillIdx;
	NpcSkillList::Iterator	skillIter;

	while( INVALID_SKILL_INDEX != (nSkillIdx = skillList.NextSkillIdx(skillIter)) )
	{
		RobotSkillInfo	info;
		info.skillId = skillList.GetIdByIdx(nSkillIdx);

		KSkill	*pSkill = g_SkillManager.GetSkill(info.skillId);

		if(pSkill)
		{
			SkillDamageInfo	*pDamInfo = pSkill->GetCommonDamageInfo();
			info.totalDamage = CalcTotalDamage(pDamInfo);
			info.attackRadius = pSkill->GetAttackRadius();

			m_RobotSkillInfo.push_back(info);
		}
	}

	sort(m_RobotSkillInfo.begin(), m_RobotSkillInfo.end(), CmpSkillInfo);

	return true;
}

int	RobotSkillPolicy::GetUsableSkill(int nNearestEnemyIdx)
{
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	NpcSkillList	&skillList = Npc[nPlayerNpcIdx].m_SkillList;

	int nSkillId = skillList.GetSwitchSkillId();

	if(INVALID_SKILL_ID != nSkillId)
	{
		if( IsSkillCanUse(nPlayerNpcIdx, nNearestEnemyIdx, nSkillId) )
			return nSkillId;
	}

	nSkillId = Npc[nPlayerNpcIdx].GetNormalSkillId();

	if( IsSkillCanUse(nPlayerNpcIdx, nNearestEnemyIdx, nSkillId) )
		return nSkillId;
	else
		return INVALID_SKILL_ID;

// 	RobotSkillInfoCont::const_iterator	itSkillInfo;
// 	RobotSkillInfoCont::const_iterator	itSkillInfoEnd = m_RobotSkillInfo.end();
// 
// 	for(itSkillInfo = m_RobotSkillInfo.begin(); itSkillInfo < itSkillInfoEnd; ++itSkillInfo)
// 	{
// 		if( !skillList.CanCast(itSkillInfo->skillId) )
// 			continue;
// 
// 		if( (itSkillInfo->totalDamage <= 0) 
// 			&& (itSkillInfo->skillId != Npc[nPlayerNpcIdx].GetNormalSkillId()) )
// 			continue;
// 
// 		KSkill	*pSkill = g_SkillManager.GetSkill(itSkillInfo->skillId);
// 
// 		if(NULL == pSkill)
// 			continue;
// 
// 		if( pSkill->GetAttackTargetType() & att_target_only )
// 		{
// 			int	nSkillParam1 = SKILL_SPT_TargetIndex;
// 			if( pSkill->CanCastSkill(nPlayerNpcIdx, nSkillParam1, nNearestEnemyIdx) )
// 				return itSkillInfo->skillId;
// 		}
// 		else
// 		{
// 			// 近程群攻在CanCastSkill中不会检测攻击距离
// 			if( SKILL_SS_NearMultiAttack == pSkill->GetSkillStyle() )
// 			{
// 				int	nAttackRadius = pSkill->GetAttackRadius();
// 				int	nDistanceSquare = NpcSet.GetDistanceSquare(nPlayerNpcIdx, nNearestEnemyIdx);
// 
// 				if(nAttackRadius * nAttackRadius < nDistanceSquare)
// 					continue;
// 			}
// 
// 			int	nX, nY;
// 			Npc[nNearestEnemyIdx].GetMpsPos(&nX, &nY);
// 			if( pSkill->CanCastSkill(nPlayerNpcIdx, nX, nY) )
// 				return itSkillInfo->skillId;
// 		}
// 	}

//	return INVALID_SKILL_ID;
}

int RobotSkillPolicy::CalcTotalDamage(const SkillDamageInfo *pDamInfo)
{
	int	nTotalDamage = 0;

	for(int nDamIdx = 0; nDamIdx < dot_end; ++nDamIdx)
		nTotalDamage += pDamInfo[nDamIdx].nVal;	

	return nTotalDamage;
}

bool RobotSkillPolicy::IsSkillCanUse(int nLauncher, int nTarget, int nSkillId)
{
	KSkill	*pSkill = g_SkillManager.GetSkill(nSkillId);

	if(NULL == pSkill)
		return false;

	if( pSkill->GetAttackTargetType() & att_target_only )
	{
		int	nSkillParam1 = SKILL_SPT_TargetIndex;

		if( Npc[nLauncher].m_SkillList.CanCast(nSkillId) &&
			Npc[nLauncher].Cost(pSkill, TRUE) &&
			pSkill->CanCastSkill(nLauncher, nSkillParam1, nTarget) 
		  )
			return true;
	}
	else
	{
		// 近程群攻在CanCastSkill中不会检测攻击距离
		if( SKILL_SS_NearMultiAttack == pSkill->GetSkillStyle() )
		{
			int	nAttackRadius = pSkill->GetAttackRadius();
			int	nDistanceSquare = NpcSet.GetDistanceSquare(nLauncher, nTarget);

			if(nAttackRadius * nAttackRadius < nDistanceSquare)
				return false;
		}

		int	nX, nY;
		Npc[nTarget].GetMpsPos(&nX, &nY);

		if(	Npc[nLauncher].m_SkillList.CanCast(nSkillId) &&
			Npc[nLauncher].Cost(pSkill, TRUE) &&
			pSkill->CanCastSkill(nLauncher, nX, nY) 
		  )
			return true;
	}	

	return false;
}

#endif // #ifdef _AUTO_ROBOT