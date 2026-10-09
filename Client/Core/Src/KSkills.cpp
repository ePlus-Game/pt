//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 10/24/2007 14:00
//      File_base        : KSkills
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "KSG_StringProcess.h"
#include "KSkills.h"
#include "KNpc.h"
#include "math.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KMath.h"
#include "KEngine.h"
#include "KTabFile.h"
#include "KTabFileCtrl.h"
#include "KPlayer.h"
#include "buff_man.h"
#include "ChatDataDef.h"
#include "CoreUtil.h"
#include "CoreRelated.h"
#ifndef _SERVER
	#include "coreshell.h"
	#include "iRepresentshell.h"
	#include "scene/KScenePlaceC.h"
	#include "KRepresentUnit.h"
	#include "imgref.h"
	#include "KOption.h"
	#include "SkillManager.h"
	#include "KMissle.h"
	#include "KMissleSet.h"
#else
	#include "SkillTargetFilter.h"
	#include "player_monitor.h"
#endif

KSkill::KSkill( void )
{
	m_nSkillCostType			= attrib_mana;
    m_nWaitTime					= 0;
	m_nDoHurt					= 100;
	m_SkillType					= skill_type_hostile;
	m_Threat					= 0;
	m_DamageNum					= 0;
	m_bCancelable					= 0;
	memset(m_DamageInfo, 0, sizeof(m_DamageInfo));
#ifndef _SERVER
	m_szManPreCastSoundFile[0]	= 0;
	m_szFMPreCastSoundFile[0]	= 0;
	m_szDesc[0]					= 0;
	m_nFlySkillId				=  0;
	m_nCollideSkillId			= 0;
	m_nVanishedSkillId			= 0;
#else
	m_CorBuffNum				= 0;
	m_IncorBuffNum				= 0;
#endif
}

KSkill::~KSkill( void )
{
}

BOOL KSkill::CanCastSkill(int nLauncher, int &nParam1, int &nParam2)  const 
{
	if ( (m_AttackTargetType & att_target_only) && nParam1 != SKILL_SPT_TargetIndex ) // lixuewu
		return FALSE;
		
	if (nParam1 == SKILL_SPT_TargetIndex)
	{
		if ( nParam2 <= 0 || nParam2 >= MAX_NPC) return FALSE;

		KNpc& targetNpc = Npc[nParam2];
		KNpc& launcherNpc = Npc[nLauncher];
		
		//PK低等级保护
		if ((nLauncher != nParam2) && (launcherNpc.IsPlayer() || launcherNpc.IsCreature()) && (targetNpc.IsPlayer() || targetNpc.IsCreature()))
		{
			int pkProtectLevel = ConfigManager::Singleton().GetGlobalVariable(global_var_pk_protect_level);
			if ((m_AttackTargetType & att_target_enemyplayer) && (launcherNpc.GetLevel() < pkProtectLevel || targetNpc.GetLevel() < pkProtectLevel))
				return FALSE;
		}

		if( Npc[nLauncher].IsAttackTarget(nParam2, m_AttackTargetType) )
			goto relationisvalid;

		return FALSE;
	}
	
relationisvalid:

	if (Npc[nLauncher].IsPlayer())
	{
		// 强制需要武器才能放技能
		int nWeaponType = GetWeaponSkill();
		int nCurWeaponType = Player[Npc[nLauncher].GetPlayerIdx()].GetItemList().GetWeaponLevel();
		if ( (nWeaponType > 0 && nWeaponType != nCurWeaponType) || 
			(!Player[Npc[nLauncher].GetPlayerIdx()].m_ItemList.isEquipHaveItem(itempart_weapon)) )
			return FALSE;

		int nPlayerIdx = 0;

#ifdef _SERVER
		nPlayerIdx	= Npc[nLauncher].GetPlayerIdx();
#else
		nPlayerIdx	= CLIENT_PLAYER_INDEX;
#endif

		if( IsCostItem() )
		{
			if( Player[nPlayerIdx].m_ItemList.HaveNormalItem(m_ItemKeys[0], m_ItemKeys[1], 
						m_ItemKeys[2], m_ItemKeys[3]) < m_ItemKeys[ITEM_KEY_NUM] )
				return FALSE;
		}

		if(m_bCheckTeammate)
		{
			if(Player[nPlayerIdx].GetTeamInfo().IsInTeam())
			{
				KTeam* pTeam = Player[nPlayerIdx].GetTeamInfo().GetTeam();
				if (pTeam != NULL)
				{
					if (pTeam->GetMemberCount() < m_TeammateNum)
					{
						return FALSE;
					}
				}				
			}				
		}

		// 检查攻击者和目标之间是否有阻挡
		if(m_AttackTargetType & att_target_only)
		{
			if( !CanTravTo(nLauncher, nParam2) )
				return FALSE;
		}

		if(SKILL_SS_Rush == m_eSkillStyle || SKILL_SS_AddFarTrap == m_eSkillStyle)
		{
			if( !CanTravToTargetPos(nLauncher, nParam1, nParam2) )
				return FALSE;
		}
		
		bool bCheckDistance = IsNeadCheckDistance();
		
		if (bCheckDistance)
		{
			if (nParam1 == SKILL_SPT_TargetIndex)
			{
				if( !IsInShootRange(nLauncher, nParam2) )
				{
//#ifdef _SERVER
//下面的代码原意是希望在位置不同步的情况下减少玩家打怪时的
//不适，但是可能会被恶意利用，所以删掉。
//  				if(Npc[nLauncher].IsPlayer())
// 					{
// 						int nX = 0;
// 						int nY = 0;
// 
// 						Npc[nLauncher].GetMpsPos( &nX, &nY);						
// 						Npc[nParam2].SendCommand( do_run, nX, nY);
// 					}
//#endif
					return FALSE;
				}
			}
			else if(SKILL_SS_AddFarTrap == m_eSkillStyle || SKILL_SS_Rush == m_eSkillStyle) 
			{
				if (nParam1 < 0 || nParam2 < 0) 
					return FALSE;
				
				int nSX = 0;
				int nSY = 0;
				Npc[nLauncher].GetMpsPos(&nSX, &nSY);
				int nDistance = g_GetDistance(nSX, nSY, nParam1, nParam2);
				
				//Modify by zuolizhi for AttackRadiusEnhance
				if (nDistance > GetAttackRadius() ) 
				{
					return FALSE;
				}
			}
		}
	}
	
	return TRUE;
}

BOOL KSkill::CastSummonSkill(KNpc &aNpc)
{
	if (!aNpc.IsPlayer())
	{
		return FALSE;
	}
	_ASSERT(aNpc.m_Kind == kind_player);

#ifndef _SERVER
	aNpc.m_DataRes.SetBlur(FALSE);
#else

	int nSkillIdx		=	aNpc.m_SkillList.FindSkill(m_nId);
	int nCreaterType	= 	aNpc.m_SkillList.GetSpecDataByIdx(nSkillIdx);
	
	if (nCreaterType <= 0)
	{
		nCreaterType = m_CreatureType;
	}
	
	// nCreatureType 表示召唤兽类型，对应npcs.txt中的索引(行号 - 2)
	// nCreatureLevel 表示召唤兽等级
	int	nCreatureLevel = aNpc.GetLevel();
	int nTemplateIdx = MAKELONG(nCreatureLevel, nCreaterType);
	const int nIndex = NpcSet.Add(nTemplateIdx,aNpc.m_SubWorldIndex,aNpc.m_RegionIndex,aNpc.GetMapX(),aNpc.GetMapY(), false);
	
	_ASSERT(nIndex > 0);
	if (nIndex <= 0 )
	{
		KCreature& aCreature = Player[aNpc.GetPlayerIdx()].m_Creature;
		aCreature.Dismiss();
		return 0;
	}
	
	Npc[nIndex].PolyMorph(nCreaterType, TRUE, 0, -1, 0);
				
	Npc[nIndex].m_Kind = kind_creature;
	Npc[nIndex].m_Doing = do_stand;
	Npc[nIndex].m_Level = nCreatureLevel;			
	KPlayer& aPlayer = Player[aNpc.GetPlayerIdx()];
	
	Npc[nIndex].SetSummonerIdx(aNpc.m_Index);
	strcpy(Npc[nIndex].Name,aNpc.Name);
	KCreature& aCreature = Player[aNpc.GetPlayerIdx()].m_Creature;

	const SUMMONPARAM SummonParam =
	{ 
		m_nId,
		nCreatureLevel,
		aNpc.m_Index, 
		nIndex,
		0
	};
	
	aCreature.Summon(SummonParam);
	Npc[nIndex].SendSyncData(Player[aNpc.GetPlayerIdx()].GetNetConnectIdx(), Player[aNpc.GetPlayerIdx()].m_nIndex);
#endif	
	
	aNpc.SendCommand(do_stand);
	if (aNpc.m_Frames.nTotalFrame == 0)
	{
		aNpc.SetProcessAI(TRUE);
	}	
	
	return TRUE;
}

/*!*****************************************************************************
// Function		: KSkill::Cast
// Purpose		: 发技能的统一接口
// Return		: 
// Argumant		: int nLauncher 发送者Id
// Argumant		: int nParam1   
// Argumant		: int nParam2
// Argumant		: int nWaitTime 发送的延迟时间
// Argumant		: eSkillLauncherType eLauncherType 发送者类型
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL	KSkill::Cast(int nLauncher, int nParam1, int nParam2, 
					 int nParam3, eSkillLauncherType eLauncherType, 
					 DWORD dwParentSkillID, int nPosNpc)
{
	//-----------------接口函数入口点，检测参数合法性-------------------------------
	
	//检查发送者是否符合要求
	if ((nLauncher < 0)||((nParam1 < 0)&&(nParam2 < 0)))
	{
		g_DebugLog("Skill::Cast(), nLauncher < 0 , Return False;"); 
		return FALSE; 
	}

	
	//校验参数的合法性
	switch(eLauncherType)
	{
	case SKILL_SLT_Npc:
		{
			if (MAX_NPC <= nLauncher)
				return FALSE;
			if (Npc[nLauncher].m_dwID < 0) 
				return FALSE;

			if (nParam1 == SKILL_SPT_TargetIndex)
			{
				if (nParam2 >= MAX_NPC) 
					return FALSE;
				
				if (
					(Npc[nParam2].m_Index <= 0)
					|| Npc[nLauncher].m_SubWorldIndex != Npc[nParam2].m_SubWorldIndex
					)
					return FALSE;
			}
		}
		break;
		
	case SKILL_SLT_Obj:
		{
			if (MAX_OBJECT <= nParam2) return FALSE;
			if (Object[nParam2].m_nDataID < 0) return FALSE;
		}
		break;
/*
#ifndef _SERVER
	case SKILL_SLT_Missle:
		{
			if (MAX_MISSLE <= nLauncher) 
				return FALSE;
			
			if (Missle[nLauncher].m_nMissleIdx < 0) 
				return FALSE;
			
			if (nParam1 == SKILL_SPT_TargetIndex)
			{
				if (nParam2 >= MAX_NPC) 
					return FALSE;
				
				if ((Npc[nParam2].m_Index <= 0) ||  Missle[nLauncher].m_nSubWorldId != Npc[nParam2].m_SubWorldIndex)
					return FALSE;
			}
		}
		break;
#endif
*/
	default:
		{
			return FALSE;
		}
	}
	
	//WaitTime不应该小于0，在当前设定上来说
	if (nParam3 < 0 ) 
	{
		g_DebugLog("Call Skill::Cast(), nWaitTime < 0 "); 
		nParam3 = 0;
	}
	
	//------------------------------------------------------------------------------

#ifdef _SERVER
	//统计玩家使用技能
	if (Npc[nLauncher].IsPlayer())
	{
		int playerIndex = Npc[nLauncher].GetPlayerIdx();
		Player[playerIndex].GetPlayerStatistic().UseSkill(m_nId);
		
		if (g_PlayerMonitor.IsNeedRecord(playerIndex, player_action_cast_skill))
		{
			RecordPlayerActionParam param;
			param.PlayerIndex = playerIndex;
			param.Action = player_action_cast_skill;
			snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_CAST_SKILL, m_nId);
			g_PlayerMonitor.RecordPlayerAction(param);
		}
	}
#endif// _SERVER

	// 召唤技比较特殊，放在前面过滤
	if(SKILL_SS_Summon == m_eSkillStyle)
	{
#ifdef _SERVER
		AppendBuff(nLauncher, nLauncher, buff_target_self);
#endif
		return CastSummonSkill(Npc[nLauncher]);
	}

	// 服务器端附加伤害，Buff, 陷阱等
#ifdef _SERVER

	switch(m_eSkillStyle)
	{
	case SKILL_SS_PassivityNpcState:
	case SKILL_SS_Active:
		// 单攻技能，有目标
		if( SKILL_SPT_TargetIndex == nParam1 
			&& (nParam2 > 0 && nParam2 < MAX_NPC)
			&& (m_AttackTargetType & att_target_only)
		  )
		{
			AttackTarget(nLauncher, nParam2);
		}
		break;

		// 近程群攻，以人物自己为中心
	case SKILL_SS_NearMultiAttack:
		if( !(m_AttackTargetType & att_target_only) )
		{
			int nSrcX, nSrcY;

			if( nPosNpc == -1 )
				Npc[nLauncher].GetMpsPos( &nSrcX, &nSrcY );
			else
				Npc[nPosNpc].GetMpsPos( &nSrcX, &nSrcY );

			AttackMultiTarget(nLauncher, Npc[nLauncher].m_SubWorldIndex, 
					Npc[nLauncher].m_RegionIndex, nSrcX, nSrcY);	
		}
		break;

		// 远程群攻，定点，现在的版本传入的是陷阱的index
	case SKILL_SS_FarMultiAttack:
		if( !(m_AttackTargetType & att_target_only) )
		{
			int nSrcX, nSrcY;

			Object[nParam2].GetMpsPos( &nSrcX, &nSrcY );

			AttackMultiTarget(nLauncher, Object[nParam2].m_nSubWorldID,
					Object[nParam2].m_nRegionIdx, nSrcX, nSrcY);
		}
		break;

	case SKILL_SS_AddFarTrap:
	case SKILL_SS_AddFixRangeTrap:
		{
			int nSrcX, nSrcY;
			Npc[nLauncher].GetMpsPos(&nSrcX, &nSrcY);
			int trapDir = g_GetDirIndex(nSrcX, nSrcY, nParam1, nParam2);
			BuffMgr::Singleton().AddTrap(m_TrapId, nLauncher, nParam1, nParam2, trapDir);
		}
		break;

	case SKILL_SS_AddNearTrap:
		BuffMgr::Singleton().AddTrap(m_TrapId, nLauncher);
		break;
		
	case SKILL_SS_RmTrap:
		BuffMgr::Singleton().RemoveTrap(nParam2);
		break;

	case SKILL_SS_Rush:
		Npc[nLauncher].SetPos(nParam1, nParam2);
		AttackTarget(nLauncher, nLauncher);
		break;

	case SKILL_SS_DIR:
		{
			int nSrcX, nSrcY;
			Npc[nLauncher].GetMpsPos( &nSrcX, &nSrcY );
			m_AttackDir = g_GetDirIdxForFindPath(nSrcX, nSrcY, nParam1, nParam2);
			AttackMultiTarget(nLauncher, Npc[nLauncher].m_SubWorldIndex, 
				Npc[nLauncher].m_RegionIndex, nSrcX, nSrcY);
		}
		break;

	case SKILL_SS_TargetDir:
		{
			if (SKILL_SPT_TargetIndex == nParam1 && IsValidNpc(nParam2))
			{
				int nSrcX, nSrcY;
				Npc[nLauncher].GetMpsPos( &nSrcX, &nSrcY );
				int destX, destY;
				Npc[nParam2].GetMpsPos( &destX, &destY );
				m_AttackDir = g_GetDirIdxForFindPath(nSrcX, nSrcY, destX, destY);
				AttackMultiTarget(nLauncher, Npc[nLauncher].m_SubWorldIndex, 
					Npc[nLauncher].m_RegionIndex, nSrcX, nSrcY);
			}
		}
		break;

		// 矩形区域群攻
	case SKILL_SS_Rectangle:
		if( !(m_AttackTargetType & att_target_only) )
		{
			if (eLauncherType == SKILL_SLT_Npc)
			{
				KNpc& launcher = Npc[nLauncher];
				int nSrcX, nSrcY;
				launcher.GetMpsPos( &nSrcX, &nSrcY );
				m_AttackDir = g_GetDirIdxForFindPath(nSrcX, nSrcY, nParam1, nParam2);
				AttackMultiTarget(nLauncher, launcher.m_SubWorldIndex, launcher.m_RegionIndex, nSrcX, nSrcY);
			} 
			else if (eLauncherType == SKILL_SLT_Obj)
			{
				KObj& obj = Object[nParam2];
				int nSrcX, nSrcY;
				obj.GetMpsPos(&nSrcX, &nSrcY);
				m_AttackDir = nParam1;
				AttackMultiTarget(nLauncher, obj.m_nSubWorldID, obj.m_nRegionIdx, nSrcX, nSrcY);
			}
		}
		break;

	default:
		break;
	}

#endif

	// 客户端利用子弹显示特效
#ifndef _SERVER

	// 远程群攻，现在的版本传入的是陷阱的 index，
	// 客户端播放特效需要将其转化成地图坐标
//	if(m_eSkillStyle == SKILL_SS_FarMultiAttack)
//		SubWorld[Object[nParam2].m_nSubWorldID].Map2Mps(Object[nParam2].m_nRegionIdx,
//													Object[nParam2].m_nMapX,
//													Object[nParam2].m_nMapY,
//													Object[nParam2].m_nOffX,
//													Object[nParam2].m_nOffY,
//													&nParam1,
//													&nParam2
//													);	



	if(m_eMisslesForm > SKILL_MF_NONE && m_eMisslesForm < SKILL_MF_COUNT)
	{
		CastMissles(nLauncher, nParam1, nParam2, nParam3, eLauncherType, dwParentSkillID);
	}
	
	// Add by chenshanglin on [2006-3-8 18:02]
	if(m_nStartSkillId > 0 && m_nEventSkillLevel > 0 && g_RandPercent(m_bStartEvent))
	// Add end
	{
		KSkill * pOrdinSkill = g_SkillManager.GetSkill(m_nStartSkillId, m_nEventSkillLevel);
		if (!pOrdinSkill) 
            return FALSE;
		
        pOrdinSkill->Cast(nLauncher, nParam1, nParam2, nParam3, eLauncherType);
	}

#endif
	
	return TRUE;	  
}

#ifdef _SERVER

void KSkill::AppendBuff(int nLauncher, int nTargetIdx, int nBuffTarget)
{
	BuffMgr &mgr = BuffMgr::Singleton();

	// 概率不相关的Buff，每个Buff独立计算自己的概率
	if(m_IncorBuffNum > 0)
	{
		for(int i = 0; i < m_IncorBuffNum; ++i)
		{
			if( !g_RandPercent(m_IncorelatedBuff[i].nPercent) )
				continue;

			if( nBuffTarget & m_IncorelatedBuff[i].nBuffTarget)
				mgr.AddNpcBuff(nLauncher, nTargetIdx, m_IncorelatedBuff[i].nBuffId );
		}
	}

	// 概率相关的Buff，这些Buff只能加其中的一个，而且肯定加上
	if(m_CorBuffNum > 0)
	{
		for(int i = 0; i < m_CorBuffNum; ++i)
		{
			if( i == m_CorBuffNum - 1)
			{
				// 前面的都没加上，那么做后一个就不用Random，直接加上
				if(nBuffTarget & m_CorelatedBuff[i].nBuffTarget)
					mgr.AddNpcBuff(nLauncher, nTargetIdx, m_CorelatedBuff[i].nBuffId );
			}
			else
			{
				if( !g_RandPercent(m_CorelatedBuff[i].nPercent) )
					continue;

				if(nBuffTarget & m_CorelatedBuff[i].nBuffTarget)
					mgr.AddNpcBuff(nLauncher, nTargetIdx, m_CorelatedBuff[i].nBuffId );
			
				break;
			}
		}
	}	
}

void KSkill::AttackTarget(int nLauncher, int nTargetIdx)
{
	Npc[nTargetIdx].BeUsedSkill(nLauncher, GetSkillType());

	if( !IsHitTheTarget(nLauncher, nTargetIdx) )
	{
		Npc[nTargetIdx].SyncDamageInfo(nLauncher, 0, COMBAT_INFO_DODGE, GetSkillId());
		return;
	}

	//filter skill in
	//===================================================
	BuffMgr& BM = BuffMgr::Singleton( );
	BUFF_ENV_PARAM Env;
	Env.nEventSender	=	nLauncher;
	Env.nEventRecever	=	nTargetIdx;
	Env.nEventType		=	buff_event_type_skillin;
	Env.nEventFormat	=	buff_event_format_skillid;
	Env.nEventRelation	=	buff_event_relation_recver;
	Env.nEvent			=	m_nId;
	BM.FilterEvent( Env );

	if( Env.nEvent <= 0 )
		return;
	//===================================================	

	AppendBuff(nLauncher, nLauncher, buff_target_self);
	AppendBuff(nLauncher, nTargetIdx, buff_target_target);

	AppendDamage(nLauncher, nTargetIdx);


	//filter delay skill out
	//===================================================
	Env.nEventSender	=	nLauncher;
	Env.nEventRecever	=	nTargetIdx;
	Env.nEventType		=	buff_event_type_delayskillout;
	Env.nEventFormat	=	buff_event_format_skillid;
	Env.nEventRelation	=	buff_event_relation_sender;
	Env.nEvent			=	m_nId;
	BM.FilterEvent( Env );
	//===================================================

	//filter final skill out
	//===================================================
	Env.nEventSender	=	nLauncher;
	Env.nEventRecever	=	nTargetIdx;
	Env.nEventType		=	buff_event_type_finalskillout;
	Env.nEventFormat	=	buff_event_format_skillid;
	Env.nEventRelation	=	buff_event_relation_sender;
	Env.nEvent			=	m_nId;
	BM.FilterEvent( Env );
	//===================================================
}
#endif

#ifdef _SERVER
void KSkill::AttackMultiTarget(int nLauncher, int nSubWorldIdx, int nSrcRgnIdx, int nSrcX, int nSrcY)
{
	// 防止有些 Buff 在玩家下线后仍放出技能
	if(nSubWorldIdx < 0 || nSrcRgnIdx < 0)
		return;

	int		nLeftTargetNum = m_MostAttackNum + Npc[nLauncher].m_SkillList.GetMaxAttackTarget(m_nId);
	KRegion &CurRegion = SubWorld[nSubWorldIdx].m_Region[nSrcRgnIdx];

	AppendBuff(nLauncher, nLauncher, buff_target_self);

	AttackTargetInRegion(nLauncher, CurRegion, nSrcX, nSrcY, nLeftTargetNum);

	for(int i = 0; i < 8 && (-1 == nLeftTargetNum || nLeftTargetNum > 0) ; ++i)
	{
		int nRegionIdx = CurRegion.m_nConnectRegion[i];

		if(-1 != nRegionIdx)
			AttackTargetInRegion(nLauncher, SubWorld[nSubWorldIdx].m_Region[nRegionIdx], nSrcX, nSrcY, nLeftTargetNum);	
	}

	//filter final skill out
	//===================================================
	BuffMgr& BM = BuffMgr::Singleton( );
	BUFF_ENV_PARAM Env;
	Env.nEventSender	=	nLauncher;
	Env.nEventRecever	=	nLauncher;
	Env.nEventType		=	buff_event_type_finalskillout;
	Env.nEventFormat	=	buff_event_format_skillid;
	Env.nEventRelation	=	buff_event_relation_sender;
	Env.nEvent			=	m_nId;
	BM.FilterEvent( Env );
	//===================================================
}
#endif

#ifdef _SERVER
void KSkill::AttackTargetInRegion(int nLauncher, KRegion &CurRegion, int nSrcX, int nSrcY, int &nLeftTargetNum)
{
	KIndexNode *pNode = (KIndexNode *)CurRegion.m_NpcList.GetHead();

	// -1 表示不用考虑目标数量
	while( pNode && (-1 == nLeftTargetNum || nLeftTargetNum > 0) )
	{
		int nNpcIdx = pNode->m_nIndex;
		int nRelation = NpcSet.GetRelation(nLauncher, nNpcIdx);

		pNode = (KIndexNode*)pNode->GetNext();

		if ( !IsInShootRange(nSrcX, nSrcY, nNpcIdx) )
			continue;

		if( !Npc[nLauncher].IsAttackTarget(nNpcIdx, m_AttackTargetType) )
			continue;		

		if(SKILL_SS_DIR == m_eSkillStyle
			|| SKILL_SS_TargetDir == m_eSkillStyle
			|| SKILL_SS_Rectangle == m_eSkillStyle)
		{
			DirSkillFilterParam passby;
			passby.nDir = m_AttackDir;
			memcpy(&passby.param, &m_TargetFilterInfo.param, sizeof(m_TargetFilterInfo.param));

			if( !SkillTargetFilter::Filter(m_TargetFilterInfo.nFilterType, 
				nLauncher,
				nSrcX,
				nSrcY,
				nNpcIdx, 
				&passby) 
				)
				continue;
		}
		
		Npc[nNpcIdx].BeUsedSkill(nLauncher, GetSkillType());

		if( !IsHitTheTarget(nLauncher, nNpcIdx) )
		{
			Npc[nNpcIdx].SyncDamageInfo(nLauncher, 0, COMBAT_INFO_DODGE, GetSkillId());
			continue;
		}
	
		if( !CanTravTo(nSrcX, nSrcY, nNpcIdx) )
			continue;

		//filter skill in
		//===================================================
		BuffMgr& BM = BuffMgr::Singleton( );
		BUFF_ENV_PARAM Env;
		Env.nEventSender	=	nLauncher;
		Env.nEventRecever	=	nNpcIdx;
		Env.nEventType		=	buff_event_type_skillin;
		Env.nEventFormat	=	buff_event_format_skillid;
		Env.nEventRelation	=	buff_event_relation_recver;
		Env.nEvent			=	m_nId;
		BM.FilterEvent( Env );
		
		if( Env.nEvent <= 0 )
			return;
		//===================================================	
		//

		AppendBuff(nLauncher, nNpcIdx, buff_target_target);
		AppendDamage(nLauncher, nNpcIdx);

		if(-1 != nLeftTargetNum)
			--nLeftTargetNum;

		//filter delay skill out
		//===================================================
		Env.nEventSender	=	nLauncher;
		Env.nEventRecever	=	nNpcIdx;
		Env.nEventType		=	buff_event_type_delayskillout;
		Env.nEventFormat	=	buff_event_format_skillid;
		Env.nEventRelation	=	buff_event_relation_sender;
		Env.nEvent			=	m_nId;
		BM.FilterEvent( Env );
		//===================================================
	}			
}
#endif

BOOL KSkill::CanTravTo(int nSubWorldIdx, int nStartX, int nStartY, int nEndX, int nEndY) const
{
	if (m_TravBarrierInfo == trav_barrier_all)
		return TRUE;

	// 防止客户端传入一个很大或很小的非法目标点，导致服务器计算过多
	if( abs(nStartX - nEndX) >= 1024 )
	{
		return FALSE;
	}

	if( abs(nStartY - nEndY) >= 1024 )
	{
		return FALSE;
	}

	const int  nStepLen = 16;

	int    nLenX = nEndX - nStartX;
	int    nLenY = nEndY - nStartY;
	float  fLen = qsqrt(nLenX * nLenX + nLenY * nLenY);
	int	   nStepX = int(nStepLen * nLenX / fLen);
	int	   nStepY = int(nStepLen * nLenY / fLen);

	if(0 == nLenX && 0 == nLenY)
		return TRUE;

	while(true)
	{
		int nRet = SubWorld[nSubWorldIdx].TestBarrier(nStartX, nStartY);
		nRet &= 0xf;
	
		switch(nRet)
		{
		case Obstacle_Normal:
			if( !(m_TravBarrierInfo & trav_barrier_normal) )
				return FALSE;

		case Obstacle_Fly:
			if( !(m_TravBarrierInfo & trav_barrier_fly) )
				return FALSE;

		case Obstacle_Jump:
			if( !(m_TravBarrierInfo & trav_barrier_jump) )
				return FALSE;

		case Obstacle_JumpFly:
			if( !(m_TravBarrierInfo & trav_barrier_jumpfly) )
				return FALSE;
		}

		nStartX += nStepX;
		nStartY += nStepY;

		if(nLenX > 0 && nStartX >= nEndX)
			break;

		if(nLenX < 0 && nStartX <= nEndX)
			break;

		if(nLenY > 0 && nStartY >= nEndY)
			break;

		if(nLenY < 0 && nStartY <= nEndY)
			break;
	}

	return TRUE;	
}

BOOL KSkill::CanTravTo(int nStartX, int nStartY, int nTarget) const
{
	if (m_TravBarrierInfo == trav_barrier_all)
		return TRUE;	

	int nEndX, nEndY;
	SubWorld[Npc[nTarget].m_SubWorldIndex].Map2Mps(Npc[nTarget].m_RegionIndex, 
										  		   Npc[nTarget].GetMapX(),
												   Npc[nTarget].GetMapY(),
												   Npc[nTarget].GetOffX(),
												   Npc[nTarget].GetOffY(),
												   &nEndX,
												   &nEndY
												  );	
	
	return CanTravTo(Npc[nTarget].m_SubWorldIndex, nStartX, nStartY, nEndX, nEndY);
}

BOOL KSkill::CanTravTo(int nLauncher, int nTarget) const
{
	if (Npc[nLauncher].m_SubWorldIndex != Npc[nTarget].m_SubWorldIndex)
		return FALSE;

	if (m_TravBarrierInfo == trav_barrier_all)
		return TRUE;

	int nStartX, nStartY;
	SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, 
									  				 Npc[nLauncher].GetMapX(),
													 Npc[nLauncher].GetMapY(),
													 Npc[nLauncher].GetOffX(),
													 Npc[nLauncher].GetOffY(),
													 &nStartX,
													 &nStartY
													);
	int nEndX, nEndY;
	SubWorld[Npc[nTarget].m_SubWorldIndex].Map2Mps(Npc[nTarget].m_RegionIndex, 
										  		   Npc[nTarget].GetMapX(),
												   Npc[nTarget].GetMapY(),
												   Npc[nTarget].GetOffX(),
												   Npc[nTarget].GetOffY(),
												   &nEndX,
												   &nEndY
												  );

	return CanTravTo(Npc[nLauncher].m_SubWorldIndex, nStartX, nStartY, nEndX, nEndY);
}

BOOL KSkill::CanTravToTargetPos(int nLauncher, int nDstX, int nDstY) const
{
	if(m_TravBarrierInfo == trav_barrier_all)
		return TRUE;

	int nStartX, nStartY;
	Npc[nLauncher].GetMpsPos(&nStartX, &nStartY);

	return CanTravTo(Npc[nLauncher].m_SubWorldIndex, nStartX, nStartY, nDstX, nDstY);
}

void KSkill::GetInfoFromTabFile(KITabFile *pSkillsSettingFile, int nRow)
{
	if (!pSkillsSettingFile || nRow < 0) 
		return;

	pSkillsSettingFile->GetInteger(nRow, "SkillId",				0, (int *)&m_nId,TRUE);
	pSkillsSettingFile->GetInteger(nRow, "HorseLimit",			0, (int *)&m_nHorseLimited, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ChildSkillNum",		0, &m_nChildSkillNum,TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MisslesForm",			0, (int *)&m_eMisslesForm, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "SkillStyle",			0, (int *)&m_eSkillStyle, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CharAnimId",			0, (int *)&m_nCharActionId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "IsPhysical",			0, &m_bIsPhysical, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "WeaponSkill",			0, &m_nWeaponSkill, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ByMissle",			0, &m_bByMissle, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ChildSkillId",		0, &m_nChildSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "SkillCostType",		0, (int *)&m_nSkillCostType, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CostValue",			0, &m_nCost, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "TimePerCast",			0, &m_nMinTimePerCast, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Param1",				0, &m_nValue1, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Param2",				0, &m_nValue2, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ChildSkillLevel",		0, &m_nChildSkillLevel, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "EventSkillLevel",		0, &m_nEventSkillLevel, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MslsGenerate",		0, (int *)&m_eMisslesGenerateStyle, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MslsGenerateData",	0, &m_nMisslesGenerateData, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "AttackRadius",		0, &m_nAttackRadius, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "StartSkillId",		0, &m_nStartSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "WaitTime",			0, &m_nWaitTime, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ClientSend",			0, &m_bClientSend, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "StopWhenMove",		0, &m_nInteruptTypeWhenMove, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "HeelAtParent",		0, (int *)&m_bHeelAtParent, TRUE );
	pSkillsSettingFile->GetInteger(nRow, "AttackTargetType",	att_target_none, &m_AttackTargetType, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Barrier",				trav_barrier_none, &m_TravBarrierInfo, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Group",				0, &m_SkillGroup, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Category",			0, &m_SkillCategory[0], TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Category1",			0, &m_SkillCategory[1], TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Category2",			0, &m_SkillCategory[2], TRUE);
	pSkillsSettingFile->GetInteger(nRow, "SummonNpcId",			0, &m_CreatureType, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "AddCriProb",			0, &m_AppendExplodeProb, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CriDamage",			0, &m_AppendExplodeDamage, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "DoHurt",				0, (int *)&m_nDoHurt, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CastSpeed",			0, &m_CastSpeedEnhance, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "RelativePosType",		0, (int *)&m_eMissleRelativePosType, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "IsCheckTeammate",		0, &m_bCheckTeammate, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "TeammateNum",			0, &m_TeammateNum, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "SkillType",			skill_type_hostile, &m_SkillType, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Threat",				0, &m_Threat, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "IsTalismanSkill",		0, &m_IsTalismanSkill, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CostSkillExp",		0, &m_nCostSkillExp, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "Cancelable",			0, &m_bCancelable, TRUE);
	

	LoadCostItemInfo(pSkillsSettingFile, nRow);
	m_eRelation = 0;
	if (m_AttackTargetType & att_target_enemy)
		m_eRelation |= relation_enemy;
	
	if (m_AttackTargetType & att_target_ally)
		m_eRelation |= relation_ally;
	
	if (m_AttackTargetType & att_target_self)
		m_eRelation |= relation_self;
	
#ifndef _SERVER
	
	pSkillsSettingFile->GetInteger(nRow, "DisplayID",			0, &m_nDisplayID, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "BaseSkill",			0, &m_bBaseSkill, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "FlyEvent",			0, &m_bFlyingEvent, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "StartEvent",			0, &m_bStartEvent, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CollideEvent",		0, &m_bCollideEvent, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "VanishedEvent",		0, &m_bVanishedEvent, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "FlySkillId",			0, &m_nFlySkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "VanishedSkillId",		0, &m_nVanishedSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "CollidSkillId",		0, &m_nCollideSkillId, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "FlyEventTime",		0, &m_nFlyEventTime, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MissleMoveKind",		MISSLE_MMK_Line, (int*)&m_eMissleMoveKind, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MissleSpeed",			5, &m_MissleSpeed, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MissleLifeTime",		GAME_FPS, &m_MissleLifeTime, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MissleHeight",		1, &m_MissleHeight, TRUE);
	pSkillsSettingFile->GetString(nRow, "SkillIcon",			"",m_szSkillIcon, sizeof(m_szSkillIcon));
	pSkillsSettingFile->GetString(nRow, "DescView",				"", m_szDesc, sizeof(m_szDesc), TRUE);
	pSkillsSettingFile->GetString(nRow, "SkillName",			"", m_szName, sizeof(m_szName) ,TRUE);
	pSkillsSettingFile->GetInteger(nRow, "LRSkill",				0, (int*)&m_eLRSkillInfo);
	pSkillsSettingFile->GetString(nRow, "PreCastSpr",			"", m_szPreCastEffectFile, 100);
	pSkillsSettingFile->GetString(nRow, "ManCastSnd",			"", m_szManPreCastSoundFile, 100);
	pSkillsSettingFile->GetString(nRow, "FMCastSnd",			"", m_szFMPreCastSoundFile, 100);

	pSkillsSettingFile->GetInteger(nRow, "ActionType",			0, (int*)&m_eActionType, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ActionBeginFrame",	0, &m_nBeginFrame, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "ActionEndFrame",		0, &m_nEndFrame, TRUE);
#else
	pSkillsSettingFile->GetInteger(nRow, "IsUseAR",				0, &m_bUseAttackRate, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "AddBaseDamage",		0, &m_bAddBaseDamage, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "MostAffNum",			-1, &m_MostAttackNum, TRUE);
	pSkillsSettingFile->GetInteger(nRow, "TrapId",				-1, &m_TrapId, TRUE);
	// 读取Buff相关数据
	LoadBuffInfo(pSkillsSettingFile, nRow);
	LoadTargetFilterInfo(pSkillsSettingFile, nRow);
#endif
	// 读取技能的伤害数据
	LoadSkillDamageInfo(pSkillsSettingFile, nRow);
}

void KSkill::LoadCostItemInfo(KITabFile *pSkillsSettingFile, int nRow)
{
	const int	BUF_SIZE = 64;
	char		buf[BUF_SIZE];

	pSkillsSettingFile->GetString(nRow, "CostItem", "", buf, BUF_SIZE, TRUE);

	if(0 == buf[0])
		m_ItemKeys[0] = -1;
	else
		StrToIntArray(buf, ",", m_ItemKeys, sizeof(m_ItemKeys) / sizeof(int));	
}

#ifdef _SERVER
void KSkill::LoadTargetFilterInfo(KITabFile *pSkillSettingFile, int nRow)
{
	static const char *szTag = ",";
	const int BUF_SIZE = 64;
	char szBuf[BUF_SIZE];

	memset(&m_TargetFilterInfo, 0, sizeof(m_TargetFilterInfo));
	pSkillSettingFile->GetString(nRow, "TargetFilterInfo", "", szBuf, BUF_SIZE, TRUE);

	if(0 != szBuf[0])
	{
		int nRet = StrToIntArray(szBuf, 
			szTag, 
			(int*)&m_TargetFilterInfo, 
			sizeof(m_TargetFilterInfo) / sizeof(int)
			);

		_ASSERT(nRet == sizeof(m_TargetFilterInfo) / sizeof(int));
	}
}
#endif

#ifdef _SERVER

void KSkill::LoadBuffInfo(KITabFile *pSkillSettingFile, int nRow)
{
	const int BUF_SIZE = 64;
	static const char *szTag = ",";

	char	szBuf[BUF_SIZE];
	int		aBuffId[MAX_BUFF_PER_SKILL];
	int		aBuffPercent[MAX_BUFF_PER_SKILL];
	int		aBuffTarget[MAX_BUFF_PER_SKILL];
	int		nBuffIdNum;
	int		nBuffPerNum;
	int		nBuffTargetNum;
	
	pSkillSettingFile->GetString(nRow, "IncorBuffId", "", szBuf, sizeof(szBuf));
	nBuffIdNum = StrToIntArray(szBuf, szTag, aBuffId, MAX_BUFF_PER_SKILL);
	pSkillSettingFile->GetString(nRow, "IncorBAvailRating", "", szBuf, sizeof(szBuf));
	nBuffPerNum = StrToIntArray(szBuf, szTag, aBuffPercent, MAX_BUFF_PER_SKILL);
	pSkillSettingFile->GetString(nRow, "IncorBTarget", "", szBuf, sizeof(szBuf));
	nBuffTargetNum = StrToIntArray(szBuf, szTag, aBuffTarget, MAX_BUFF_PER_SKILL);

	if(nBuffIdNum == nBuffPerNum && nBuffPerNum == nBuffTargetNum)
	{
		for(int i = 0; i < nBuffIdNum; ++i)
		{
			m_IncorelatedBuff[i].nBuffId     = aBuffId[i];
			m_IncorelatedBuff[i].nPercent    = aBuffPercent[i];
			m_IncorelatedBuff[i].nBuffTarget = aBuffTarget[i];
		}

		m_IncorBuffNum = nBuffIdNum;
	}
	else
	{
		_ASSERT(0);
	}

	pSkillSettingFile->GetString(nRow, "CorBuffId", "", szBuf, sizeof(szBuf));
	nBuffIdNum = StrToIntArray(szBuf, szTag, aBuffId, MAX_BUFF_PER_SKILL);
	pSkillSettingFile->GetString(nRow, "CorBAvailRating", "", szBuf, sizeof(szBuf));
	nBuffPerNum = StrToIntArray(szBuf, szTag, aBuffPercent, MAX_BUFF_PER_SKILL);
	pSkillSettingFile->GetString(nRow, "CorBTarget", "", szBuf, sizeof(szBuf));
	nBuffTargetNum = StrToIntArray(szBuf, szTag, aBuffTarget, MAX_BUFF_PER_SKILL);

	if(nBuffIdNum == nBuffPerNum && nBuffPerNum == nBuffTargetNum)
	{
#ifdef _DEBUG
			int nPerSum = 0;
			for(int j = 0; j < nBuffIdNum; ++j)
				nPerSum += aBuffPercent[j];

			_ASSERT(nBuffIdNum == 0 || 100 == nPerSum);
#endif

			for(int i= 0; i < nBuffIdNum; ++i)
			{
				m_CorelatedBuff[i].nBuffId     = aBuffId[i];
				m_CorelatedBuff[i].nPercent    = aBuffPercent[i];
				m_CorelatedBuff[i].nBuffTarget = aBuffTarget[i];		
			}

			m_CorBuffNum = nBuffIdNum;
	}
	else
	{
		_ASSERT(0);
	}
}
#endif

#ifdef _SERVER
void KSkill::AppendDamage(int nLauncher, int nTargetIdx)
{
	//防止多次死亡
	if (Npc[nTargetIdx].m_Doing == do_death)
		return;

	if(m_DamageNum > 0 || m_bAddBaseDamage)
	{
		int					aNpcDamage[dot_end];
		int					aNpcDefend[dot_end];
		OutputDamageInfo	aOutDamage[dot_end];

		Npc[nLauncher].GetNpcDamage(aNpcDamage);
		Npc[nTargetIdx].GetNpcDefend(aNpcDefend);
		CalcDamage(aNpcDamage, 
				   aNpcDefend, 
				   Npc[nLauncher].m_SkillList.GetPrivateDamInfo(m_nId), 
				   aOutDamage
				   );

		// 爆击计算 为了精确及方便策划填表，爆击率的分母是1000
		int		nExplodeProb;

		nExplodeProb = m_bIsPhysical ? Npc[nLauncher].CalcPhysExplode()
									 : Npc[nLauncher].CalcMagicExplode();
		nExplodeProb += m_AppendExplodeProb;
		nExplodeProb += Npc[nLauncher].m_SkillList.GetExplodeProb(m_nId);

		//filter explode calc
		//===================================================
		{{
		BuffMgr& BM = BuffMgr::Singleton( );
		BUFF_ENV_PARAM Env;
		Env.nEventSender	=	nLauncher;
		Env.nEventRecever	=	nTargetIdx;
		Env.nEventType		=	buff_event_type_explodecalc;
		Env.nEventFormat	=	buff_event_format_event;
		Env.nEventRelation	=	buff_event_relation_sender;
		Env.nEventValue		=	nExplodeProb;
		BM.FilterEvent( Env );
		nExplodeProb = Env.nEventValue;
		}}
		//===================================================	

		bool isCrit = false;//是否爆击

		if( (int)g_Random(1000) < nExplodeProb )
		{
			isCrit = true;
			int nAppendExplodeDam = m_AppendExplodeDamage + 
				Npc[nLauncher].m_SkillList.GetExplodeDamage(m_nId);

			for(int i = 0; i < dot_end; ++i)
				aOutDamage[i].nVal = aOutDamage[i].nVal * nAppendExplodeDam / 100;
				

			//filter explode out
			//===================================================
			BuffMgr& BM = BuffMgr::Singleton( );
			BUFF_ENV_PARAM Env;
			Env.nEventSender	=	nLauncher;
			Env.nEventRecever	=	nTargetIdx;
			Env.nEventType		=	buff_event_type_explodeout;
			Env.nEventFormat	=	buff_event_format_event;
			Env.nEventRelation	=	buff_event_relation_sender;
			BM.FilterEvent( Env );
			//===================================================	
		}
		//
		
		//buff filter----------------------------------------------------------
		BuffMgr& BM = BuffMgr::Singleton( );
		BUFF_ENV_PARAM Env;
		OutputDamageInfo	RBOutDamage[dot_end];
		memset( &RBOutDamage, 0, sizeof(RBOutDamage) );

		for( int nLoopCount = 0; nLoopCount < dot_end; nLoopCount++ )
		{

			//filter damage out
			//===================================================

			Env.nEventSender	=	nLauncher;
			Env.nEventRecever	=	nTargetIdx;
			Env.nEventType		=	buff_event_type_damageout;
			Env.nEventFormat	=	buff_event_format_damageid;
			Env.nEventRelation	=	buff_event_relation_sender;
			Env.nEvent			=	nLoopCount;
			Env.nEventValue		=	aOutDamage[nLoopCount].nVal;

			if( Env.nEventValue != 0 )
				BM.FilterEvent( Env );
			
			if( Env.nEvent != nLoopCount )
			{				
				aOutDamage[Env.nEvent].nVal += Env.nEventValue;
				
				aOutDamage[nLoopCount].nVal = 0;
			}
			else
				aOutDamage[nLoopCount].nVal = Env.nEventValue;

			//===================================================

			//filter damage in
			//===================================================
			Env.nEventSender	=	nLauncher;
			Env.nEventRecever	=	nTargetIdx;
			Env.nEventType		=	buff_event_type_damagein;
			Env.nEventFormat	=	buff_event_format_damageid;
			Env.nEventRelation	=	buff_event_relation_recver;
			Env.nEvent			=	nLoopCount;
			Env.nEventValue		=	aOutDamage[nLoopCount].nVal;

			if( Env.nEventValue != 0 )
				BM.FilterEvent( Env );
			
			if( Env.nEventRecever != nTargetIdx )
			{
				RBOutDamage[Env.nEvent].nVal = Env.nEventValue;
			}
			else
			{
				if( Env.nEvent != nLoopCount )
				{
					aOutDamage[Env.nEvent].nVal += Env.nEventValue;
					
					aOutDamage[nLoopCount].nVal = 0;
				}
				else
					aOutDamage[nLoopCount].nVal = Env.nEventValue;
			}
			
			//===================================================
		}

		Npc[nLauncher].ReceiveDamage(nTargetIdx, GetSkillId(), RBOutDamage, m_nDoHurt, isCrit);
		//-----------------------------------------------------------------------

		Npc[nTargetIdx].ReceiveDamage(nLauncher, GetSkillId(), aOutDamage, m_nDoHurt, isCrit);
	}	
}
#endif

void KSkill::LoadSkillDamageInfo(KITabFile *pSkillsSettingFile, int nRow)
{
	static const char *szTag = ",";
	const int BUF_SIZE = 64;

	char	szBuf[BUF_SIZE];
	int		arrayDT[dot_end];
	int		arrayDTT[dot_end];
	int		arrayVal[dot_end];
	int		arrayNpcPer[dot_end];

	pSkillsSettingFile->GetString(nRow, "DamageType", "", szBuf, sizeof(szBuf));
	int nDTNum = StrToIntArray(szBuf, szTag, arrayDT, dot_end);

	pSkillsSettingFile->GetString(nRow, "DamageTargetType", "", szBuf, sizeof(szBuf));
	int nDTTNum = StrToIntArray(szBuf, szTag, arrayDTT, dot_end);

	pSkillsSettingFile->GetString(nRow, "DamageVal", "", szBuf, sizeof(szBuf));
	int nValNum = StrToIntArray(szBuf, szTag, arrayVal, dot_end);

	pSkillsSettingFile->GetString(nRow, "NpcPercent", "", szBuf, sizeof(szBuf));
	int nNpcPercNum = StrToIntArray(szBuf, szTag, arrayNpcPer, dot_end);

	if( (nDTNum == nDTTNum) && (nDTTNum == nValNum) && (nValNum == nNpcPercNum) )
	{
		for(int i = 0; i < nDTNum; ++i)
		{
			if( arrayDT[i] < 0 || arrayDT[i] >= dot_end )
			{
				_ASSERT(0);
				return;
			}

			if( arrayDTT[i] != dtt_life && arrayDTT[i] != dtt_mana )
			{
				_ASSERT(0);
				return;
			}

			// 现在允许负的伤害
//			if( arrayNpcPer[i] < 0)
//			{
//				_ASSERT(0);
//				return;
//			}
			
			m_DamageInfo[arrayDT[i]].nTargetType = arrayDTT[i];
			m_DamageInfo[arrayDT[i]].nVal = arrayVal[i];
			m_DamageInfo[arrayDT[i]].nNpcDamagePercent = arrayNpcPer[i];
		}

		m_DamageNum = nDTNum;
	}
	else
	{
		_ASSERT(0);
	}
}

// add by chenshanglin on 2006-2-15 for new skill system
BOOL KSkill::IsHorseLimitSatisfied(int nLauncher) const
{
	//0表示不限制
	//1表示不可以骑马发该技能
	//2表示必须骑马发该技能
	if (m_nHorseLimited)
	{
		switch(m_nHorseLimited)
		{
		case 1:
			{
				if (Npc[nLauncher].m_bRideHorse)
				{
#ifndef _SERVER
					KSystemMessage Msg;
					Msg.byConfirmType = SMCT_NONE;
					Msg.byParamSize = 0;
					Msg.byPriority = 1;
					Msg.eType = SMT_NORMAL;
					strcpy_const(Msg.szMessage, MSG_CANT_RIDE);
//					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
#endif
					return FALSE;
				}
			}
			break;

		case 2:
			{
				if (!Npc[nLauncher].m_bRideHorse)
				{
#ifndef _SERVER
					KSystemMessage Msg;
					Msg.byConfirmType = SMCT_NONE;
					Msg.byParamSize = 0;
					Msg.byPriority = 1;
					Msg.eType = SMT_NORMAL;
					strcpy_const(Msg.szMessage, MSG_CANT_RIDE);
//					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
#endif
					return FALSE;
				}
			}
			break;
		default:
				return FALSE;
		}
	}
	
	return TRUE;
}
// add end

/*!*****************************************************************************
// Function		: KSkill::SetMissleGenerateTime
// Purpose		: 获得当前的子弹的实际产生时间
// Return		: void 
// Argumant		: Missle * pMissle
// Argumant		: int nNo
// Author		: RomanDou
// Comments		:
*****************************************************************************/
unsigned int KSkill::GetMissleGenerateTime(int nNo) const 
{
	
	switch(m_eMisslesGenerateStyle)
	{
	case SKILL_MGS_NULL:
		{
			return m_nWaitTime;
		}break;
		
	case SKILL_MGS_SAMETIME:
		{
			return  m_nWaitTime + m_nMisslesGenerateData;
		}break;
		
	case SKILL_MGS_ORDER:		
		{
			return  m_nWaitTime + nNo * m_nMisslesGenerateData;
		}break;
		
	case SKILL_MGS_RANDONORDER:	
		{
			if (g_Random(2) == 1) 
				return m_nWaitTime + nNo * m_nMisslesGenerateData + g_Random(m_nMisslesGenerateData);
			else 
				return m_nWaitTime + nNo * m_nMisslesGenerateData  - g_Random(m_nMisslesGenerateData / 2);
		}break;
		
	case SKILL_MGS_RANDONSAME:	
		{
			return  m_nWaitTime + g_Random(m_nMisslesGenerateData);
		}break;
		
	case SKILL_MGS_CENTEREXTENDLINE:
		{
			if (m_nChildSkillNum <= 1) return m_nWaitTime;
			int nCenter = m_nChildSkillNum / 2	;
			return m_nWaitTime + abs(nNo - nCenter) * m_nMisslesGenerateData ;
		}
	}
	return m_nWaitTime;
}

// 客户端特效相关函数

#ifndef _SERVER

void KSkill::PlayPreCastSound(BOOL bIsFeMale, int nX, int nY)  const 
{
	char * pSoundFile = NULL;
	
	if (!bIsFeMale)
		pSoundFile = (char *)m_szManPreCastSoundFile;
	else 
		pSoundFile = (char *)m_szFMPreCastSoundFile;
	
	int		nCenterX = 0, nCenterY = 0, nCenterZ = 0;
	
	// 获得屏幕中心点的地图坐标 not end
	g_ScenePlace.GetFocusPosition(nCenterX, nCenterY, nCenterZ);
	KCacheNode * pSoundNode = NULL;
	pSoundNode = (KCacheNode*) g_SoundCache.GetNode(pSoundFile, (KCacheNode*)pSoundNode);
	KWavSound * pWave = (KWavSound*)pSoundNode->m_lpData;
	if (pWave)
	{
		//pWave->Play((nX - nCenterX) * 5, (10000 - (abs(nX - nCenterX) + abs(nY - nCenterY))) * Option.GetSndVolume() / 100 - 10000, 0);
		
		float dist = sqrt((nX-nCenterX)*(nX-nCenterX)+(nY-nCenterY)*(nY-nCenterY));
		pWave->Play((nX-nCenterX)*10, Option.GetSndVolume(dist), 0);
	}
}

int KSkill::Param2PCoordinate(int nLauncher, int nParam1, int nParam2 , int *npPX, int *npPY, eSkillLauncherType eLauncherType)  const 
{
	
	int nTargetId = -1;
	if (eLauncherType == SKILL_SLT_Obj) return 0;
	
	switch(nParam1)
	{
	case SKILL_SPT_TargetIndex: //nParam2 参数指向某个Npc，或Obj的Index
		{
			nTargetId		= nParam2;
			KNpc& aNpc = Npc[nParam2];
			aNpc.GetMpsPos(npPX, npPY);
		}
		break;

	case SKILL_SPT_Direction://nParam 参数指向某个方向
		break;

	default://默认时, nParam1 与nParam2 为实际点坐标
		*npPX = nParam1;
		*npPY = nParam2;
		break;
	}
	
	if (*npPX < 0 || *npPY < 0)	
	{
		*npPX = 0;
		*npPY = 0;
	}
	
	return nTargetId;
}

/*!*****************************************************************************
// Function		: KSkill::Vanish
// Purpose		: 子弹生命结束时回调
// Return		: 
// Argumant		: KMissle* Missle
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KSkill::Vanish(KMissle * pMissle)  const 
{
	OnMissleEvent(Missle_VanishEvent, pMissle);
}

// Commented by chenshanglin on [2006-3-16 10:25]
// BOOL KSkill::OnMissleEvent(unsigned short usEvent, KMissle * pMissle)  const 
// Commented end
// Add by chenshanglin on [2006-3-16 10:25]
// nTargetIdx 用于碰撞事件中的必中技能
BOOL KSkill::OnMissleEvent(unsigned short usEvent, KMissle * pMissle, int nTargetIdx /* = -1 */)  const 
// Add end
{
	if (!pMissle) 
        return FALSE;

	int nLauncherIdx = pMissle->m_nLauncher;
	
    if (
		pMissle->m_nMissleIdx <= 0 
		|| pMissle->m_nMissleIdx >= MAX_MISSLE 
		|| nLauncherIdx <= 0
		|| nLauncherIdx >= MAX_NPC
		|| Npc[nLauncherIdx].m_Index <= 0
		)
        return FALSE;

	
	if (
		(!Npc[nLauncherIdx].IsMatch(pMissle->m_dwLauncherId)) 
		|| Npc[nLauncherIdx].m_SubWorldIndex != pMissle->m_nSubWorldId
		|| Npc[nLauncherIdx].m_RegionIndex < 0
		)
	{
		return FALSE;
	}
	
	int nEventSkillId = 0;
	int nEventSkillLevel = 0;
	switch(usEvent)
	{
	case Missle_FlyEvent:

		// Commented by chenshanglin on [2006-3-8 18:04]
		// if (!m_bFlyingEvent || m_nFlySkillId <= 0 || m_nEventSkillLevel <= 0)
		// Commented end
		
		// Commented by chenshanglin on [2006-3-15 10:10]
		// Add by chenshanglin on [2006-3-8 18:05]
		// if(m_nFlySkillId <= 0 || m_nEventSkillLevel <= 0 || !g_RandPercent(m_bFlyingEvent))
		// Add end
		// Commented end

		// Add by chenshanglin on [2006-3-15 10:11]
		// 在KMissle里面会先Random一次，这里不用在Random了
		if(m_nFlySkillId <= 0 || m_nEventSkillLevel <= 0)
		// Add end
			return FALSE;
		
		nEventSkillId = m_nFlySkillId ;
		nEventSkillLevel = m_nEventSkillLevel;
		break;
		
	case Missle_StartEvent:
		// Commented by chenshanglin on [2006-3-8 18:05]
		// if (!m_bStartEvent || m_nStartSkillId <= 0 || m_nEventSkillLevel <= 0)
		// Commented end
		
		// Commented by chenshanglin on [2006-3-15 10:11]
		// Add by chenshanglin on [2006-3-8 18:06]
		// if(m_nStartSkillId <= 0 || m_nEventSkillLevel <= 0 || !g_RandPercent(m_bStartEvent))
		// Add end
		// Commented end

		// Add by chenshanglin on [2006-3-15 10:11]
		// 在KMissle里面Random了一次，这里不再Random了
		if(m_nStartSkillId <= 0 || m_nEventSkillLevel <= 0)
		// Add end
			return FALSE;

		nEventSkillId = m_nStartSkillId ;
		nEventSkillLevel = m_nEventSkillLevel;
		break;
		
	case Missle_VanishEvent:
		// Commented by chenshanglin on [2006-3-8 18:06]
		// if (!m_bVanishedEvent || m_nVanishedSkillId <= 0 || m_nEventSkillLevel <= 0)
		// Commented end
		
		// Add by chenshanglin on [2006-3-8 18:07]
		// 在KMissle里面Random了一次，这里不再Random了
		if(m_nVanishedSkillId <= 0 || m_nEventSkillLevel <= 0)
		// Add end
			return FALSE;

		nEventSkillId = m_nVanishedSkillId ;
		nEventSkillLevel = m_nEventSkillLevel;
		break;
		
	case Missle_CollideEvent:
		// Commented by chenshanglin on [2006-3-8 18:08]
		// if (!m_bCollideEvent || m_nCollideSkillId <= 0 || m_nEventSkillLevel <= 0)
		// Commented end
		
		// Add by chenshanglin on [2006-3-8 18:08]
		// 在KMissle里面Random了一次，这里不再Random了
		if(m_nCollideSkillId <= 0 || m_nEventSkillLevel <= 0)
		// Add end
			return FALSE;

		nEventSkillId = m_nCollideSkillId;
		nEventSkillLevel = m_nEventSkillLevel;
		break;
	default:
		return FALSE;
	}
		
	int nDesPX = 0, nDesPY = 0;
	
	// changed by chenshanglin on 2006-3-23
	if (1 == m_bByMissle)
	{
		pMissle->GetMpsPos(&nDesPX, &nDesPY);
	}
	// Add by chenshanglin on [2006-3-24 17:38]
	else if(2 == m_bByMissle)
	{
		nDesPX = pMissle->m_nDesMapX;
		nDesPY = pMissle->m_nDesMapY;
	}
	// Add end
	else
	{
		Npc[nLauncherIdx].GetMpsPos(&nDesPX, &nDesPY);
	}

	KSkill * pOrdinSkill = (KSkill*)g_SkillManager.GetSkill(nEventSkillId, nEventSkillLevel);
	if (!pOrdinSkill) 
        return FALSE;
	
	BOOL bRetCode = FALSE;
	
	// changed by chenshanglin 
    if (1 == m_bByMissle)    //When Event
	{
		bRetCode = pOrdinSkill->CastMissles(pMissle->m_nMissleIdx, nDesPX, nDesPY, 0, SKILL_SLT_Missle);		
	}
	else
	{
		// Commented by chenshanglin on [2006-3-15 14:46]
// 		if (pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles)
// 		{   
//			bRetCode = pOrdinSkill->CastMissles(nLauncherIdx, nDesPX, nDesPY, 0, SKILL_SLT_Npc);
// 		}
		// Commented end

		// Add by chenshanglin on [2006-3-15 14:46]
		// 需要支持子弹及必中技能
		// bRetCode = pOrdinSkill->Cast(nLauncherIdx, SKILL_SPT_TargetIndex, nTargetIdx, 0, SKILL_SLT_Npc);
		// Add end

		// Add by chenshanglin on [2006-3-24 15:14]
		if(2 == m_bByMissle)
		{
			bRetCode = pOrdinSkill->CastMissles(pMissle->m_nMissleIdx, nDesPX, nDesPY, 0, SKILL_SLT_Missle, TRUE);
		}
		else if(0 == m_bByMissle)
		{
			bRetCode = pOrdinSkill->Cast(nLauncherIdx, SKILL_SPT_TargetIndex, nTargetIdx, 0, SKILL_SLT_Npc);
		}
		// Add end
	}
	
	return bRetCode;
}

/*!*****************************************************************************
// Function		: KSkill::FlyEvent
// Purpose		: 
// Return		: void 
// Argumant		: int nMissleId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KSkill::FlyEvent(KMissle * pMissle)  const 
{
	OnMissleEvent(Missle_FlyEvent, pMissle);
}

/*!*****************************************************************************
// Function		: KSkill::Collidsion
// Purpose		: 子弹被撞时回调
// Return		: 
// Argumant		: KMissle* Missle
// Comments		:
// Author		: RomanDou
*****************************************************************************/
// Commented by chenshanglin on [2006-3-16 10:20]
// void	KSkill::Collidsion(KMissle * pMissle)  const 
// Commented end
// Add by chenshanglin on [2006-3-16 10:21]
void	KSkill::Collidsion(KMissle * pMissle, int nTargetIdx)  const 
// Add end
{
	// Commented by chenshanglin on [2006-3-16 10:25]
	// OnMissleEvent(Missle_CollideEvent, pMissle);
	// Commented end
	
	// Add by chenshanglin on [2006-3-16 10:26]
	OnMissleEvent(Missle_CollideEvent, pMissle, nTargetIdx);
	// Add end
}

/*!*****************************************************************************
// Function		: KSkill::CastMissles
// Purpose		: 发送子弹技能
// Return		: 
// Argumant		: int nLauncher  发送者id
// Argumant		: int nParam1
// Argumant		: int nParam2
// Argumant		: int nWaitTime  延长时间
// Argumant		: eSkillLauncherType eLauncherType 发送者类型
// Comments		:
// Author		: RomanDou
*****************************************************************************/
// Commented by chenshanglin on [2006-3-27 11:17]
// BOOL	KSkill::CastMissles(int nLauncher, int nParam1, int nParam2, int nWaitTime, eSkillLauncherType eLauncherType, BOOL bAddExp, DWORD dwParentSkillID )  const 
// Commented end
// Add by chenshanglin on [2006-3-27 11:17]
BOOL KSkill::CastMissles(int nLauncher, int nParam1, int nParam2, int nWaitTime, eSkillLauncherType eLauncherType, BOOL bUseSpecialCode, DWORD dwParentSkillID )  const 
// Add end
{
	int nRegionId		=	0;
	int	nDesMapX		=	0;//地图坐标
	int nDesMapY		=	0;
	int nDesOffX		=	0;
	int nDesOffY		=	0;
	int nSrcOffX		=	0;
	int nSrcOffY		=	0;
	int nSrcPX			=	0;//点坐标
	int nSrcPY			=	0;
	int nDesPX			=	0;
	int nDesPY			=	0;
	int nDistance		=	0;
	int nDir			=	0;
	int nDirIndex		=	0;
	int nTargetId		=	-1;
	int nRefPX			=	0;
	int nRefPY			=	0;
	TOrdinSkillParam	SkillParam ;
	SkillParam.eLauncherType = SKILL_SLT_Npc;
	SkillParam.nParent = 0;
	SkillParam.eParentType = (eSkillLauncherType)0;
	SkillParam.nWaitTime = nWaitTime;
	SkillParam.nTargetId = 0;
	
	if (nLauncher <= 0) 
		return FALSE;
	
	switch(m_eMisslesForm)
	{
	/*
	火墙时，第一数字参数表示子弹之间的长度间隔
	X2  = X1 + N * SinA
	Y2  = Y2 - N * CosA
	*/
		
	case	SKILL_MF_Wall:			//墙形	多个子弹呈垂直方向排列，类式火墙状
		{
			//墙形魔法不可以只传方向
			if (nParam1 == SKILL_SPT_Direction) return FALSE;
			
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{	
					nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY,  SKILL_SLT_Npc);
				
					if (Npc[nLauncher].m_SubWorldIndex < 0) 
					{
						return FALSE;
					}
					
					// Modify by Cooler -->
					// 2005-7-12
					// SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
					SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].GetMapX(), Npc[nLauncher].GetMapY(), Npc[nLauncher].GetOffX(), Npc[nLauncher].GetOffY(), &nSrcPX, &nSrcPY);
					// End modify by Cooler <--
					
					nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
					nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
					nDir = nDir + MaxMissleDir / 4;
					if (nDir >= MaxMissleDir) 
						nDir -= MaxMissleDir;

					SkillParam.nLauncher = nLauncher;
					SkillParam.eLauncherType = eLauncherType;
					
					CastWall(&SkillParam , nDir, nDesPX, nDesPY);
				}	break;
			case SKILL_SLT_Obj:
				{
				}break;
			case SKILL_SLT_Missle:
				{
					KMissle * pMissle = &Missle[nLauncher];
					if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;

					// Modify by Cooler -->
					// 2005-7-12
					// SubWorld[Missle[nLauncher].m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);

					SubWorld[Missle[nLauncher].m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);

					// End modify by Cooler <--
					int nDir = pMissle->m_nDir + MaxMissleDir / 4;
					if (nDir >= MaxMissleDir) nDir -= MaxMissleDir;
					SkillParam.nLauncher = pMissle->m_nLauncher;
					SkillParam.nParent = nLauncher;
					//SkillParam.nParent = SKILL_SLT_Missle;
					SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
					CastWall(&SkillParam,  nDir, nRefPX, nRefPY);
				}break;
			}
		}break;
		
		
	case	SKILL_MF_Line:				//线形	多个子弹呈平行于玩家方向排列
		{
			
			if (nParam1 == SKILL_SPT_Direction)
			{
				switch(eLauncherType)
				{
				case SKILL_SLT_Npc:
					{
						// Modify by Cooler -->
						// 2005-7-12
						// SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);

						SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].GetMapX(), Npc[nLauncher].GetMapY(), Npc[nLauncher].GetOffX(), Npc[nLauncher].GetOffY(), &nSrcPX, &nSrcPY);

						// End modify by Cooler <--
						if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
						nDir = nParam2;
						SkillParam.nLauncher = nLauncher;
						SkillParam.eLauncherType = eLauncherType;
						SkillParam.nTargetId = nTargetId;
						CastLine(&SkillParam, nDir, nSrcPX,nSrcPY);
						
					}break;
				case SKILL_SLT_Obj:
					{
						
					}break;
				case SKILL_SLT_Missle:
					{
						KMissle * pMissle = &Missle[nLauncher];
						if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
						if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
						nDir = nParam2;

						// Modify by Cooler -->
						// 2005-7-12
						// SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
						SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
						// End modify by Cooler <--
						SkillParam.nLauncher = pMissle->m_nLauncher;
						SkillParam.nParent = nLauncher;
						SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
						CastWall(&SkillParam, nDir,  nRefPX, nRefPY);
					}break;
				}
				
			}
			else
			{
				switch(eLauncherType)
				{
				case SKILL_SLT_Npc:
					{
						nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY,  SKILL_SLT_Npc);
						// Modify by Cooler -->
						// 2005-7-12
						// SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);

						SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].GetMapX(), Npc[nLauncher].GetMapY(), Npc[nLauncher].GetOffX(), Npc[nLauncher].GetOffY(), &nSrcPX, &nSrcPY);					

						// End modify by Cooler <--
						nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
						nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
						SkillParam.nLauncher = nLauncher;
						SkillParam.eLauncherType = eLauncherType;
						SkillParam.nTargetId = nTargetId;
						if (m_nChildSkillNum == 1 && (KMissleSet::GetMissleTemplate(m_nChildSkillId)->m_eMoveKind == MISSLE_MMK_Line || KMissleSet::GetMissleTemplate(m_nChildSkillId)->m_eMoveKind == MISSLE_MMK_Parabola) ) 
						{
							if (nSrcPX == nDesPX && nSrcPY == nDesPY)		return FALSE ;
							nDistance = g_GetDistance(nSrcPX, nSrcPY, nDesPX, nDesPY);
							
							if (nDistance == 0 ) return FALSE;
							int		nYLength = nDesPY - nSrcPY;
							int		nXLength = nDesPX - nSrcPX;
							int		nSin = (nYLength << 10) / nDistance;	// 放大1024倍
							int		nCos = (nXLength << 10) / nDistance;
							
							if (abs(nSin) > 1024) 
								return FALSE;

							if (abs(nCos) > 1024) 
								return FALSE;
							
							
							CastExtractiveLineMissle(&SkillParam, nDir, nSrcPX, nSrcPY, nCos, nSin, nDesPX, nDesPY);
						}
						else
							CastLine(&SkillParam, nDir, nSrcPX,nSrcPY);
					}break;
				case SKILL_SLT_Obj:
					{
					}break;
				case SKILL_SLT_Missle:
					{
						KMissle * pMissle = &Missle[nLauncher];
						if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
						// Modify by Cooler -->
						// 2005-7-12
						// SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
						SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
						// End modify by Cooler <--
						SkillParam.nLauncher = pMissle->m_nLauncher;
						SkillParam.nParent = nLauncher;
						SkillParam.eParentType = eLauncherType;
						SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;

						// Add by chenshanglin on [2006-3-27 13:49]
						// 特例代码,控制子弹方向
						if(bUseSpecialCode)
						{
							nDirIndex	= g_GetDirIndex(nRefPX, nRefPY, nParam1, nParam2);
							nDir		= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
							CastLine(&SkillParam,  nDir,  nRefPX, nRefPY);	
						}
						else
						// Add end
						{
							CastLine(&SkillParam,  pMissle->m_nDir,  nRefPX, nRefPY);
						}
					}break;
				}
			}
		}
		break;
		
		//  数字参数一表示子弹之间的角度差，以64方向为准
		//  传来的X/Y参数为格子坐标
		
	case	SKILL_MF_Spread:				//散形	多个子弹呈一定的角度的发散状	
		{
			
			if (nParam1 == SKILL_SPT_Direction)
			{
				switch(eLauncherType)
				{
				case SKILL_SLT_Npc:
					{
						// Modify by Cooler -->
						// 2005-7-12
						// SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);
						SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].GetMapX(), Npc[nLauncher].GetMapY(), Npc[nLauncher].GetOffX(), Npc[nLauncher].GetOffY(), &nSrcPX, &nSrcPY);
						// End modify by Cooler <--
						if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
						nDir = nParam2;
						SkillParam.nLauncher = nLauncher;
						SkillParam.eLauncherType = eLauncherType;
						CastSpread(&SkillParam, nDir, nSrcPX,nSrcPY);
					}break;
				case SKILL_SLT_Obj:
					{
					}break;
				case SKILL_SLT_Missle:
					{
						KMissle * pMissle = &Missle[nLauncher];
						if (nParam2 > MaxMissleDir || nParam2 < 0) return FALSE;
						if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
						nDir = nParam2;
						// Modify by Cooler -->
						// 2005-7-12
						// SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);

						SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);

						// End modify by Cooler <--
						SkillParam.nLauncher = pMissle->m_nLauncher;
						SkillParam.nParent = nLauncher;
						SkillParam.eParentType = eLauncherType;
						SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
						CastSpread(&SkillParam, nDir,  nRefPX, nRefPY);
					}break;
				}
			}
			else
			{
				switch(eLauncherType)
				{
				case SKILL_SLT_Npc:
					{
						nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY, SKILL_SLT_Npc);		
						// Modify by Cooler -->
						// 2005-7-12
						// SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);

						SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].GetMapX(), Npc[nLauncher].GetMapY(), Npc[nLauncher].GetOffX(), Npc[nLauncher].GetOffY(), &nSrcPX, &nSrcPY);

						// End modify by Cooler <--
						nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
						nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
						SkillParam.nLauncher = nLauncher;
						SkillParam.eLauncherType = eLauncherType;
						SkillParam.nTargetId = nTargetId;
						
						if (m_nChildSkillNum == 1 && (KMissleSet::GetMissleTemplate(m_nChildSkillId)->m_eMoveKind == MISSLE_MMK_Line) ) 
						{
							if (nSrcPX == nDesPX && nSrcPY == nDesPY)		return FALSE ;
							nDistance = g_GetDistance(nSrcPX, nSrcPY, nDesPX, nDesPY);
							
							if (nDistance == 0 ) return FALSE;
							int		nYLength = nDesPY - nSrcPY;
							int		nXLength = nDesPX - nSrcPX;
							int		nSin = (nYLength << 10) / nDistance;	// 放大1024倍
							int		nCos = (nXLength << 10) / nDistance;

							if (abs(nSin) > 1024) 
								return FALSE;
							
							if (abs(nCos) > 1024) 
								return FALSE;

							CastExtractiveLineMissle(&SkillParam, nDir, nSrcPX, nSrcPY, nCos, nSin, nDesPX, nDesPY);
						}
						else
							// Commented by chenshanglin on [2006-3-27 14:53]
							// CastSpread(&SkillParam, nDir, nSrcPX, nSrcPY);
							// Commented end

							// Add by chenshanglin on [2006-3-27 14:53]
							CastSpread(&SkillParam, nDir, nSrcPX, nSrcPY, nDesPX, nDesPY);
							// Add end
							
					}break;
				case SKILL_SLT_Obj:
					{
						KObj* pObj = &Object[nParam2];

						int nSrcX,nSrcY;
						pObj->GetMpsPos( &nSrcX, &nSrcY );

						//SkillParam.nLauncher = pObj->m_nLauncher;

						CastSpread(&SkillParam , 0,  nSrcX, nSrcY);

					}break;
				case SKILL_SLT_Missle:
					{
						KMissle * pMissle = &Missle[nLauncher];
						if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) 
							return FALSE;
						// Modify by Cooler -->
						// 2005-7-12
						// SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);

						SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);

						// End modify by Cooler <--
						SkillParam.nLauncher = pMissle->m_nLauncher;
						SkillParam.nParent = nLauncher;
						SkillParam.eParentType = eLauncherType;
						SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
						CastSpread(&SkillParam ,pMissle->m_nDir,  nRefPX, nRefPY);
					}break;
				}
			}
			
		}break;
		
		
		//以当前点为圆点产生多个围扰的子弹
		//分成两种情况，一种为以原地为原心发出，另一种为以目标点为原心发出
		// 数字参数一表示 是否为原地发出
		
	case	SKILL_MF_Circle:				//圆形	多个子弹围成一个圈
		{
			if (nParam1 == SKILL_SPT_Direction) return FALSE;
			
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{
					nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2,  &nDesPX, &nDesPY, eLauncherType);
					// Modify by Cooler -->
					// 2005-7-12
					// SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);

					SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].GetMapX(), Npc[nLauncher].GetMapY(), Npc[nLauncher].GetOffX(), Npc[nLauncher].GetOffY(), &nSrcPX, &nSrcPY);

					// End modify by Cooler <--
					nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
					nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
					SkillParam.nLauncher = nLauncher;
					SkillParam.eLauncherType = eLauncherType;
					SkillParam.nTargetId = nTargetId;
					
					if (m_nValue1 == 0)
						CastCircle(&SkillParam, nDir, nSrcPX, nSrcPY, dwParentSkillID);
					else
						CastCircle(&SkillParam, nDir, nDesPX, nDesPY, dwParentSkillID);
				}break;
			case SKILL_SLT_Obj:
				{
					KObj* pObj = &Object[nParam2];
					
					int nSrcX,nSrcY;
					pObj->GetMpsPos( &nSrcX, &nSrcY );
					
					//SkillParam.nLauncher = pObj->m_nLauncher;
					
					CastCircle(&SkillParam , 0,  nSrcX, nSrcY, 0);
				}break;
			case SKILL_SLT_Missle:
				{
					KMissle * pMissle = &Missle[nLauncher];
					if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
					// Modify by Cooler -->
					// 2005-7-12
					// SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);

					SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);

					// End modify by Cooler <--
					SkillParam.nLauncher = pMissle->m_nLauncher;
					SkillParam.nParent = nLauncher;
					SkillParam.eParentType = eLauncherType;
					SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
					CastCircle(&SkillParam, pMissle->m_nDir,  nRefPX, nRefPY, dwParentSkillID);
				}break;
			}
			
		}break;
		
	case	SKILL_MF_Random:				//随机	多个子弹随机排放
		{
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{
					
				}break;
			case SKILL_SLT_Obj:
				{
					
				}break;
			case SKILL_SLT_Missle:
				{
					
				}break;
			}
		}
		break;
		
	case	SKILL_MF_AtTarget:				//定点	多个子弹根据
		{
			if (nParam1 == SKILL_SPT_Direction) return FALSE;	
			
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{
					nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2, &nDesPX, &nDesPY);
					nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
					nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
					SkillParam.nLauncher = nLauncher;
					SkillParam.eLauncherType = eLauncherType;
					SkillParam.nTargetId = nTargetId;
					CastZone(&SkillParam, nDir, nDesPX, nDesPY);
				}break;
			case SKILL_SLT_Obj:
				{
					
				}break;
			case SKILL_SLT_Missle:
				{
					KMissle * pMissle = &Missle[nLauncher];
					if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
					SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
					SkillParam.nLauncher = pMissle->m_nLauncher;
					SkillParam.nParent = nLauncher;
					SkillParam.eParentType = eLauncherType;
					SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
					CastZone(&SkillParam, pMissle->m_nDir, nRefPX, nRefPY);
				}break;
			}
		}break;
		
	case	SKILL_MF_AtFirer:				//本身	多个子弹停在玩家当前位置
		{
			if (nParam1 == SKILL_SPT_Direction) return FALSE;
			
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{
					SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].GetMapX(), Npc[nLauncher].GetMapY(), Npc[nLauncher].GetOffX(), Npc[nLauncher].GetOffY(), &nSrcPX, &nSrcPY);
					nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
					nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
					SkillParam.nLauncher = nLauncher;
					SkillParam.eLauncherType = eLauncherType;
					SkillParam.nTargetId = nTargetId;
					CastZone(&SkillParam,  nDir, nSrcPX, nSrcPY);
				}break;
			case SKILL_SLT_Obj:
				{
					
				}break;
			case SKILL_SLT_Missle:
				{
					KMissle * pMissle = &Missle[nLauncher];
					if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
					SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY , pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
					SkillParam.nLauncher = pMissle->m_nLauncher;
					SkillParam.nParent = nLauncher;
					SkillParam.eParentType = eLauncherType;
					SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
					CastZone(&SkillParam , pMissle->m_nDir, nRefPX, nRefPY);
				}break;
			}
			
		}break;
		
	case	SKILL_MF_Zone:
		{
			if (nParam1 == SKILL_SPT_Direction) return FALSE;
			
			switch(eLauncherType)
			{
			case SKILL_SLT_Npc:
				{
					nTargetId		= Param2PCoordinate(nLauncher,nParam1, nParam2,  &nDesPX, &nDesPY);
					// Modify by Cooler -->
					// 2005-7-12
					// SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].m_MapX, Npc[nLauncher].m_MapY, Npc[nLauncher].m_OffX, Npc[nLauncher].m_OffY, &nSrcPX, &nSrcPY);

					SubWorld[Npc[nLauncher].m_SubWorldIndex].Map2Mps(Npc[nLauncher].m_RegionIndex, Npc[nLauncher].GetMapX(), Npc[nLauncher].GetMapY(), Npc[nLauncher].GetOffX(), Npc[nLauncher].GetOffY(), &nSrcPX, &nSrcPY);

					// End modify by Cooler <--
					nDirIndex		= g_GetDirIndex(nSrcPX, nSrcPY, nDesPX, nDesPY);
					nDir			= g_DirIndex2Dir(nDirIndex, MaxMissleDir);
					SkillParam.nLauncher = nLauncher;
					SkillParam.eLauncherType = eLauncherType;
					SkillParam.nTargetId = nTargetId;
					CastZone(&SkillParam, nDir, nSrcPX, nSrcPY);
				}break;
			case SKILL_SLT_Obj:
				{
					
				}break;
			case SKILL_SLT_Missle:
				{
					KMissle * pMissle = &Missle[nLauncher];
					if (!Npc[pMissle->m_nLauncher].IsMatch(pMissle->m_dwLauncherId)) return FALSE;
					// Modify by Cooler -->
					// 2005-7-12
					// SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);
					SubWorld[pMissle->m_nSubWorldId].Map2Mps(pMissle->m_nRegionId, pMissle->m_nCurrentMapX, pMissle->m_nCurrentMapY, pMissle->m_nXOffset, pMissle->m_nYOffset, &nRefPX, &nRefPY);

					// End modify by Cooler <--
					SkillParam.nLauncher = pMissle->m_nLauncher;
					SkillParam.nParent = nLauncher;
					SkillParam.eParentType = eLauncherType;
					SkillParam.nTargetId = pMissle->m_nFollowNpcIdx;
					CastZone(&SkillParam, pMissle->m_nDir, nRefPX, nRefPY);
				}break;
			}
		}break;
	}
	return TRUE;
}



/*!*****************************************************************************
// Function		: KSkill::CastZone
// Purpose		: 
// Return		: int 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
//nValue1 = 0 表示矩形区域  nValue1 = 1 表示圆形区域
//nValue2 = 0 
int KSkill::CastZone(TOrdinSkillParam * pSkillParam , int nDir, int nRefPX, int nRefPY, BOOL bMustAttack, int nTargetIndex)  const 
{
	int nLauncher = pSkillParam->nLauncher;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int nCastMissleNum	= 0;
	int nBeginPX ;
	int nBeginPY ;
	if (m_nChildSkillNum == 1)
	{
		nBeginPX = nRefPX;
		nBeginPY = nRefPY;
	}
	else 
	{
		// lixuewu
		nBeginPX		= nRefPX - m_nChildSkillNum * REGION_CELL_SIZE_X / 2; //SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellWidth / 2;
		nBeginPY		= nRefPY - m_nChildSkillNum * REGION_CELL_SIZE_Y / 2; //SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellHeight / 2;
	}
	
	for (int i = 0; i < m_nChildSkillNum; i ++)
		for (int j = 0; j < m_nChildSkillNum; j ++)
		{
			if (m_bBaseSkill)
			{
				int nMissleIndex ;
				int nSubWorldId ; 
				
				nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
				
				if (m_nValue1 == 1)
					if ( ((i - m_nChildSkillNum / 2) * (i - m_nChildSkillNum / 2) + (j - m_nChildSkillNum / 2) * (j - m_nChildSkillNum / 2)) > (m_nChildSkillNum * m_nChildSkillNum / 4))			continue;
					
					
					if (nSubWorldId < 0)	goto exit;
					int nDesSubX = nBeginPX + j * REGION_CELL_SIZE_X;
					int nDesSubY = nBeginPY +  i * REGION_CELL_SIZE_Y;
					nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX , nDesSubY);
					
					if (nMissleIndex < 0)	continue;
					
					Missle[nMissleIndex].m_nDir				= nDir;
					Missle[nMissleIndex].m_nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
					CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
					Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
					if (pSkillParam->nTargetId > 0)
						Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
					Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
					Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
					Missle[nMissleIndex].m_nLauncher		= nLauncher;
					Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
					
					if (pSkillParam->nParent)
						Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
					else 
						Missle[nMissleIndex].m_nParentMissleIndex = 0;
					
					Missle[nMissleIndex].m_nCurSkillId			= m_nId;
					Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i * m_nChildSkillNum + j);
					Missle[nMissleIndex].m_nCurLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;
					Missle[nMissleIndex].m_nRefPX			= nDesSubX;
					Missle[nMissleIndex].m_nRefPY			= nDesSubY;
					
					if (Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Line|| Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_RollBack || Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Follow || Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Parabola)
					{
						Missle[nMissleIndex].m_nXFactor = g_DirCos(nDir, MaxMissleDir);
						Missle[nMissleIndex].m_nYFactor = g_DirSin(nDir, MaxMissleDir);
					}
					
					nCastMissleNum ++;
			}
			else
			{
				_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel > 0)	;
				KSkill * pOrdinSkill = g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
				if (pOrdinSkill) 
				{
					// lixueuw
					if (!pSkillParam->nParent)
						//nCastMissleNum += pOrdinSkill->Cast(nLauncher, nBeginPX + j * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellWidth , nBeginPY +  i * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellHeight, pSkillParam->nWaitTime + GetMissleGenerateTime(i * m_nChildSkillNum + j ), eLauncherType);
						nCastMissleNum += pOrdinSkill->Cast(nLauncher, nBeginPX + j * REGION_CELL_SIZE_X, nBeginPY +  i * REGION_CELL_SIZE_Y, pSkillParam->nWaitTime + GetMissleGenerateTime(i * m_nChildSkillNum + j ), eLauncherType);
					else 
						//nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nLauncher, nBeginPX + j * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellWidth , nBeginPY +  i * SubWorld[Npc[nLauncher].m_SubWorldIndex].m_nCellHeight, pSkillParam->nWaitTime + GetMissleGenerateTime(i * m_nChildSkillNum + j ), pSkillParam->eLauncherType);
						nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nLauncher, nBeginPX + j * REGION_CELL_SIZE_X, nBeginPY +  i * REGION_CELL_SIZE_Y, pSkillParam->nWaitTime + GetMissleGenerateTime(i * m_nChildSkillNum + j ), pSkillParam->eLauncherType);
				}
			}

		}
exit:	
	return nCastMissleNum;
}

/*!*****************************************************************************
// Function		: KSkill::CastLine
// Purpose		: 
// Return		: 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
// Value1 子弹之间的间距
// Value2 
int		KSkill::CastLine(TOrdinSkillParam *pSkillParam, int nDir, int nRefPX, int nRefPY)  const 
{
	int nLauncher = pSkillParam->nLauncher;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int	nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
	int nDesSubX		= 0;
	int nDesSubY		= 0;
	int nCastMissleNum	= 0;
	
	//子弹之间的间距
	int nMSDistanceEach = m_nValue1;
	
	//分别生成多少子弹
	for(int i = 0; i < m_nChildSkillNum; i++)
	{
		nDesSubX	= nRefPX + ((nMSDistanceEach * (i + 1) * g_DirCos(nDirIndex, MaxMissleDir) )>>10);
		nDesSubY	= nRefPY + ((nMSDistanceEach * (i + 1) * g_DirSin(nDirIndex, MaxMissleDir) )>>10);
		
		if (nDesSubX < 0 || nDesSubY < 0) 	continue;
		
		if (m_bBaseSkill)
		{
			int nMissleIndex ;
			int nSubWorldId ; 
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);
			
			if (nMissleIndex < 0)	continue;
			
			Missle[nMissleIndex].m_nDir				= nDir;
			Missle[nMissleIndex].m_nDirIndex		= nDirIndex;
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			if (pSkillParam->nTargetId > 0)
				Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			
			Missle[nMissleIndex].m_nCurSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nCurLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;	
			Missle[nMissleIndex].m_nRefPX			= nDesSubX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubY;
			if (Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Line|| Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_RollBack || Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Follow || Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Parabola)
			{
				Missle[nMissleIndex].m_nXFactor = g_DirCos(nDir, MaxMissleDir);
				Missle[nMissleIndex].m_nYFactor = g_DirSin(nDir, MaxMissleDir);
			}
			
			
			nCastMissleNum ++;
		}
		else
		{
			_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel > 0)	;
			KSkill * pOrdinSkill = g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), pSkillParam->eParentType);
				
			}
		}
		
	}
	
exit:	
	return nCastMissleNum;
}


int	KSkill::CastExtractiveLineMissle(TOrdinSkillParam* pSkillParam,  int nDir,int nSrcX, int nSrcY, int nXOffset, int nYOffset, int nDesX, int nDesY)  const 
{
	
	_ASSERT(pSkillParam);
	
	int nLauncher = pSkillParam->nLauncher;
	if (pSkillParam->eLauncherType != SKILL_SLT_Npc) return 0;	
	int	nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
	int nDesSubX		= 0;
	int nDesSubY		= 0;
	int nCastMissleNum	= 0;
	
	//分别生成多少子弹
	{
		
		if (m_bBaseSkill)
		{
			int nMissleIndex ;
			int nSubWorldId ; 
			
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			nMissleIndex = MissleSet.Add(nSubWorldId, nSrcX, nSrcY);
			
			if (nMissleIndex < 0)	goto exit;
			
			Missle[nMissleIndex].m_nDir				= nDir;
			Missle[nMissleIndex].m_nDirIndex		= nDirIndex;
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			
			if (Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Parabola)
			{
				int nLength = g_GetDistance(nSrcX, nSrcY, nDesX, nDesY);
				int nTime = nLength / Missle[nMissleIndex].m_nCurSpeed;
				Missle[nMissleIndex].m_nCurHeightSpeed	= Missle[nMissleIndex].GetMissleZAcceleration() * (nTime - 1) / 2;
				
			}
			
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			if (pSkillParam->nTargetId > 0)
				Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
		
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			
			Missle[nMissleIndex].m_nCurSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(0);
			Missle[nMissleIndex].m_nCurLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;	
			Missle[nMissleIndex].m_nRefPX			= nSrcX;
			Missle[nMissleIndex].m_nRefPY			= nSrcY;

			int nTempR = 0;
			int nTempMapX = 0;
			int nTempMapY = 0;
			int nTempOffsetX = 0;
			int nTempOffsetY = 0;

			Missle[nMissleIndex].m_bNeedReclaim = TRUE;
			int nLength = g_GetDistance(nSrcX, nSrcY, nDesX, nDesY);
			
			if (!Missle[nMissleIndex].m_nCurSpeed)
			{
				return 0;
			}

			Missle[nMissleIndex].m_nFirstReclaimTime = nLength / Missle[nMissleIndex].m_nCurSpeed + Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nEndReclaimTime = Missle[nMissleIndex].m_nFirstReclaimTime + REGION_CELL_SIZE_X / Missle[nMissleIndex].m_nCurSpeed + 2;

			if (Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Line|| Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_RollBack || Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Follow || Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Parabola)
			{
				Missle[nMissleIndex].m_nXFactor = nXOffset;
				Missle[nMissleIndex].m_nYFactor = nYOffset;
			}
			
			nCastMissleNum ++;
		}
		else
		{
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(0), pSkillParam->eLauncherType);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(0), pSkillParam->eParentType);
				
			}
		}
		
	}
	
exit:	
	return nCastMissleNum;		
}


/*!*****************************************************************************
// Function		: KSkill::CastWall
// Purpose		: Wall Magic 
// Return		: int 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
/*
m_nValue1 表示子弹之间的距离，单位像素点
*/
int KSkill::CastWall(TOrdinSkillParam * pSkillParam,  int nDir , int nRefPX , int nRefPY)  const 
{
	int nLauncher = pSkillParam->nLauncher;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	
	if (eLauncherType != SKILL_SLT_Npc) return 0;
	int	nDirIndex		= g_Dir2DirIndex(nDir, MaxMissleDir);
	int nDesSubX		= 0;
	int nDesSubY		= 0;
	int nCastMissleNum	= 0;
	
	
	//子弹之间的间距
	int nMSDistanceEach = m_nValue1;
	int nCurMSDistance	= -1 * nMSDistanceEach * m_nChildSkillNum / 2;
	
	//分别生成多少子弹
	for(int i = 0; i < m_nChildSkillNum; i++)
	{
		nDesSubX	= nRefPX + ((nCurMSDistance * g_DirCos(nDirIndex, MaxMissleDir)) >>10);
		nDesSubY	= nRefPY + ((nCurMSDistance * g_DirSin(nDirIndex, MaxMissleDir)) >>10);
		
		if (nDesSubX < 0 || nDesSubY < 0) 	continue;
		
		if (m_bBaseSkill)
		{
			int nMissleIndex ;
			int nSubWorldId ; 
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	
			{
				goto exit;
			}
			
			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);
			if (nMissleIndex < 0)	
			{
				continue;
			}

			if (m_nValue2)
			{
				int nDirTemp = nDir - MaxMissleDir / 4;
				if (nDirTemp < 0) nDirTemp += MaxMissleDir;
				Missle[nMissleIndex].m_nDir				= nDirTemp;
				Missle[nMissleIndex].m_nDirIndex = g_Dir2DirIndex(nDirTemp, 64);

			}
			else
			{
				Missle[nMissleIndex].m_nDir				= nDir;
				Missle[nMissleIndex].m_nDirIndex		= nDirIndex;
			}
			
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			
			if (pSkillParam->nTargetId > 0)
				Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
			
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			
			
			Missle[nMissleIndex].m_nCurSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nCurLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nRefPX			= nDesSubX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubY;
			
			if (Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Line|| Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_RollBack || Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Follow || Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Parabola)
			{
				Missle[nMissleIndex].m_nXFactor = g_DirCos(Missle[nMissleIndex].m_nDir, MaxMissleDir);
				Missle[nMissleIndex].m_nYFactor = g_DirSin(Missle[nMissleIndex].m_nDir, MaxMissleDir);
			}
			
			nCastMissleNum ++;
		}
		else
		{
			_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel > 0)	;
			KSkill * pOrdinSkill = g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubX, nDesSubY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubX, nDesSubY, pSkillParam->nWaitTime +  GetMissleGenerateTime(i), pSkillParam->eParentType);
			}
		}
		
		nCurMSDistance += nMSDistanceEach;
	}
	
exit:	
	return nCastMissleNum;
}


/*!*****************************************************************************
// Function		: KSkill::CastCircle
// Purpose		: 
// Return		: 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType  eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
// Value1  == 0 表示发送者为圆心产生圆，否则以目标点为圆心产生圆
int	 KSkill::CastCircle(TOrdinSkillParam * pSkillParam, int nDir, int nRefPX, int nRefPY, DWORD dwParentSkillID)  const 
{
	int nLauncher = pSkillParam->nLauncher;
	eSkillLauncherType  eLauncherType = pSkillParam->eLauncherType;
	if (eLauncherType != SKILL_SLT_Npc) return 0;	
	int nDesSubPX	= 0;
	int nDesSubPY	= 0;
	int nFirstStep	= m_nValue2;			//第一步的长度，子弹在刚发出去时离玩家的距离
	int nCurSubDir	= 0;
	int nDirPerNum  = 	MaxMissleDir / m_nChildSkillNum  ;
	int nCastMissleNum = 0;
	
	//分别生成多个子弹
	for(int i = 0; i < m_nChildSkillNum; i++)
	{
		int nCurSubDir	= nDir + nDirPerNum * i ;
		
		if (nCurSubDir < 0)
			nCurSubDir = MaxMissleDir + nCurSubDir;
		
		if (nCurSubDir >= MaxMissleDir)
			nCurSubDir -= MaxMissleDir;
		
		int nSinAB	= g_DirSin(nCurSubDir, MaxMissleDir);
		int nCosAB	= g_DirCos(nCurSubDir, MaxMissleDir);
		
		nDesSubPX	= nRefPX + ((nCosAB * nFirstStep) >> 10);
		nDesSubPY	= nRefPY + ((nSinAB * nFirstStep) >> 10);
		
		
		
		if (nDesSubPX < 0 || nDesSubPY < 0) 	continue;
		
		if (m_bBaseSkill)
		{
			int nMissleIndex ;
			int nSubWorldId ; 
			
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubPX, nDesSubPY);
			
			if (nMissleIndex < 0)	
			{
				continue;
			}
			
			Missle[nMissleIndex].m_nDir			= nCurSubDir;
			Missle[nMissleIndex].m_nDirIndex	= g_Dir2DirIndex(nCurSubDir, MaxMissleDir);
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			
			Missle[nMissleIndex].m_nFollowNpcIdx	= pSkillParam->nTargetId;
			
			if (pSkillParam->nTargetId > 0)
				Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;

			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			
			
			Missle[nMissleIndex].m_nCurSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nCurLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nRefPX			= nDesSubPX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubPY;
			
			if (Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Line|| Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_RollBack || Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Follow || Missle[nMissleIndex].GetMissleMoveKind() == MISSLE_MMK_Parabola)
			{
				Missle[nMissleIndex].m_nXFactor = g_DirCos(nCurSubDir, MaxMissleDir);
				Missle[nMissleIndex].m_nYFactor = g_DirSin(nCurSubDir, MaxMissleDir);
			}
			
			nCastMissleNum ++;
			
		}
		else
		{
			_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel > 0)	;
			KSkill * pOrdinSkill = g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum += pOrdinSkill->Cast(nLauncher, nDesSubPX, nDesSubPY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType);
				else
					nCastMissleNum += pOrdinSkill->Cast(pSkillParam->nParent, nDesSubPX, nDesSubPY, pSkillParam->nWaitTime + GetMissleGenerateTime(i), pSkillParam->eParentType);
			}
		}
		
	}
	
exit:	
	return nCastMissleNum;
}


/*!*****************************************************************************
// Function		: KSkill::CastSpread
// Purpose		: 
// Return		: 
// Argumant		: int nLauncher
// Argumant		: eSkillLauncherType eLauncherType
// Argumant		: int nDir
// Argumant		: int nRefPX
// Argumant		: int nRefPY
// Argumant		: int nWaitTime
// Argumant		: int nTargetId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
/*
Value1 每个子弹相差的角度单位
Value2 每一步的长度，第一步的长度，子弹在刚发出去时离玩家的距离
*/
// Commented by chenshanglin on [2006-3-27 14:50]
// int		KSkill::CastSpread(TOrdinSkillParam * pSkillParam, int nDir, int nRefPX, int nRefPY)  const 
// Commented end

// Add by chenshanglin on [2006-3-27 14:50]
// 增加两个特例参数，控制子弹的目标位置
int	KSkill::CastSpread(TOrdinSkillParam * pSkillParam, int nDir, int nRefPX, int nRefPY, int nDesPX /* = 0 */, int nDesPY /* = 0 */)  const 
// Add end
{
	int nLauncher = pSkillParam->nLauncher;
	eSkillLauncherType eLauncherType = pSkillParam->eLauncherType;
	if (eLauncherType != SKILL_SLT_Npc) 
		return 0;

	int nDesSubMapX		= 0;
	int nDesSubMapY		= 0;
	int nFirstStep		= m_nValue2;			//第一步的长度，子弹在刚发出去时离玩家的距离
	int nCurMSRadius	= m_nChildSkillNum / 2 ; 
	int nCurSubDir		= 0;
	int	nCastMissleNum  = 0;			//实际发送的Missle的数量
	
	// Sin A+B = SinA*CosB + CosA*SinB
	// Cos A+B = CosA*CosB - SinA*SinB
	// Sin A = nYFactor
	// Cos A = nXFactor
	
	int nDesSubX = 0;
	int nDesSubY = 0;
	int nXFactor = 0;
	int nYFactor = 0;

	if (pSkillParam->nTargetId > 0)
	{
		int nTargetId = pSkillParam->nTargetId;
		int nDistance = 0;
		
		int nDesX, nDesY;

		if (Npc[nTargetId].m_Index > 0 && Npc[nTargetId].m_SubWorldIndex >= 0) 
			SubWorld[Npc[nTargetId].m_SubWorldIndex].Map2Mps(Npc[nTargetId].m_RegionIndex, Npc[nTargetId].GetMapX(), Npc[nTargetId].GetMapY(), Npc[nTargetId].GetOffX(), Npc[nTargetId].GetOffY(), &nDesX, &nDesY);

		//////////////////////////////////////////////////////////////////////////
		// Fixed By Rocker 2004.03.22 优化平方根
		INTORFLOAT tmp;
		tmp.f = qsqrt((float)((nDesX - nRefPX)*(nDesX - nRefPX) +	(nDesY - nRefPY)*(nDesY - nRefPY)));
		tmp.f += bias.f;
		tmp.i -= bias.i;
		nDistance = tmp.i;
		//////////////////////////////////////////////////////////////////////////
		
		if (nDistance <= 0 ) nDistance = 1; // 防止除零错误

		nXFactor = ((nDesX - nRefPX)<<10) / nDistance;
		nYFactor = ((nDesY - nRefPY)<<10) / nDistance;
		
		nDesSubX = nRefPX + ((nXFactor * nFirstStep)>>10);
		nDesSubY = nRefPY + ((nYFactor * nFirstStep)>>10);
		
		if (nDesSubX < 0  || nDesSubY < 0 ) return 0;
	}
	
	int nTargetId = pSkillParam->nTargetId;
	
	//分别生成多个子弹
	for(int i = 0; i < m_nChildSkillNum; i++)
	{
		int nDSubDir	= m_nValue1 * nCurMSRadius; 
		nCurSubDir		= nDir - m_nValue1 * nCurMSRadius;
		
		
		if (nCurSubDir < 0)
			nCurSubDir = MaxMissleDir + nCurSubDir;
		
		if (nCurSubDir >= MaxMissleDir)
			nCurSubDir -= MaxMissleDir;
		
		int nSinAB	;
		int nCosAB	;
		
		if (nTargetId > 0)
		{
			nDSubDir	+= 48;
			if (nDSubDir >= MaxMissleDir)
				nDSubDir -= MaxMissleDir;
			//sin(a - b) = sinacosb - cosa*sinb
			//cos(a - b) = cosacoab + sinasinb
			nSinAB = (nYFactor * g_DirCos(nDSubDir, MaxMissleDir) - nXFactor * g_DirSin(nDSubDir, MaxMissleDir)) >> 10;
			nCosAB = (nXFactor * g_DirCos(nDSubDir, MaxMissleDir) + nYFactor * g_DirSin(nDSubDir , MaxMissleDir)) >> 10;
		}
		else
		{
			nSinAB = g_DirSin(nCurSubDir, MaxMissleDir);
			nCosAB = g_DirCos(nCurSubDir, MaxMissleDir);
		}
		
		nDesSubX	= nRefPX + ((nCosAB * nFirstStep) >> 10);
		nDesSubY	= nRefPY + ((nSinAB * nFirstStep) >> 10);
		
		if (nDesSubX < 0 || nDesSubY < 0) 	continue;
		
		if (m_bBaseSkill)
		{
			
			int nMissleIndex ;
			int nSubWorldId ; 
			nSubWorldId = Npc[nLauncher].m_SubWorldIndex;
			
			if (nSubWorldId < 0)	goto exit;
			
			nMissleIndex = MissleSet.Add(nSubWorldId, nDesSubX, nDesSubY);
			
			if (nMissleIndex < 0)	continue;
			
			Missle[nMissleIndex].m_nDir				= nCurSubDir;
			Missle[nMissleIndex].m_nDirIndex		= g_Dir2DirIndex(nCurSubDir, MaxMissleDir);
		
			// Commented by chenshanglin on [2006-3-24 17:56]
			// CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex);
			// Commented end

			// Add by chenshanglin on [2006-3-24 17:56]
			CreateMissle(nLauncher, m_nChildSkillId, nMissleIndex, nDesPX, nDesPY);	
			// Add end
		

			Missle[nMissleIndex].m_nFollowNpcIdx	= nTargetId;
			if (pSkillParam->nTargetId > 0)
				Missle[nMissleIndex].m_dwFollowNpcID	= Npc[pSkillParam->nTargetId].m_dwID;
			Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
			Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
			Missle[nMissleIndex].m_nLauncher		= nLauncher;
			Missle[nMissleIndex].m_dwLauncherId		= Npc[nLauncher].m_dwID;
			
			if (pSkillParam->nParent)
				Missle[nMissleIndex].m_nParentMissleIndex = pSkillParam->nParent;
			else 
				Missle[nMissleIndex].m_nParentMissleIndex = 0;
			
			Missle[nMissleIndex].m_nCurSkillId			= m_nId;
			Missle[nMissleIndex].m_nStartLifeTime	= pSkillParam->nWaitTime + GetMissleGenerateTime(i);
			Missle[nMissleIndex].m_nCurLifeTime		+= Missle[nMissleIndex].m_nStartLifeTime;
			Missle[nMissleIndex].m_nXFactor			= nCosAB;
			Missle[nMissleIndex].m_nYFactor			= nSinAB;
			Missle[nMissleIndex].m_nRefPX			= nDesSubX;
			Missle[nMissleIndex].m_nRefPY			= nDesSubY;
			
			nCastMissleNum ++;
		}
		else
		{
			_ASSERT(m_nChildSkillId > 0 && m_nChildSkillLevel > 0)	;
			KSkill * pOrdinSkill = g_SkillManager.GetSkill(m_nChildSkillId, m_nChildSkillLevel);
			if (pOrdinSkill) 
			{
				if (!pSkillParam->nParent)
					nCastMissleNum +=  pOrdinSkill->Cast(nLauncher,  nRefPX, nRefPY , pSkillParam->nWaitTime + GetMissleGenerateTime(i), eLauncherType);
				else
					nCastMissleNum +=  pOrdinSkill->Cast(pSkillParam->nParent,  nRefPX, nRefPY , pSkillParam->nWaitTime + GetMissleGenerateTime(i), pSkillParam->eParentType); 
			}
		}
		
		nCurMSRadius -- ;
	}

exit:	
	return nCastMissleNum;
}

/*!*****************************************************************************
// Function		: KSkill::CreateMissle
// Purpose		: 设置子弹的基本数据，以及该技能该等级下的对子弹信息的变动数据
//					设置用于数值计算的指针
// Return		: 
// Argumant		: int nChildSkillId
// Argumant		: int nMissleIndex
// Comments		:
// Author		: RomanDou
*****************************************************************************/
// Commented by chenshanglin on [2006-3-24 17:35]
// void	KSkill::CreateMissle(int nLauncher, int nChildSkillId, int nMissleIndex)  const 
// Commented end
void KSkill::CreateMissle(int nLauncher, int nChildSkillId, int nMissleIndex, int nDesPosX /* = 0 */, int nDesPosY /* = 0 */)  const 
{
	_ASSERT(nChildSkillId > 0 && nChildSkillId < MAX_MISSLESTYLE && nMissleIndex > 0);
	
	if (nLauncher <= 0) 
	{
		return ;
	}
	
	KMissle * pMissle = &Missle[nMissleIndex];
	pMissle->m_MissleRes.Init();	
	*pMissle = KMissleSet::GetMissleTemplate(nChildSkillId);//复制拷贝对象
	pMissle->m_bFlyEvent				= m_bFlyingEvent;
	pMissle->m_nFlyEventTime			= m_nFlyEventTime;
	pMissle->m_nMissleIdx				= nMissleIndex;
	pMissle->m_bClientSend				= m_bClientSend;
	pMissle->m_nInteruptTypeWhenMove	= m_nInteruptTypeWhenMove;
	pMissle->m_bHeelAtParent			= m_bHeelAtParent;
	pMissle->m_eRelativePosType			= m_eMissleRelativePosType;
	pMissle->m_nDesMapX					= nDesPosX;
	pMissle->m_nDesMapY					= nDesPosY;	
	if (pMissle->m_nInteruptTypeWhenMove)
	{
		Npc[nLauncher].GetMpsPos(&pMissle->m_nLauncherSrcPX, &pMissle->m_nLauncherSrcPY);
	}
	pMissle->m_eRelation				= m_eRelation;	
	pMissle->m_MissleRes.m_nMissleIdx	= nMissleIndex;
	pMissle->DoWait();	
	pMissle->m_eCurMoveKind				= m_eMissleMoveKind; 
	pMissle->m_nCurSpeed				= m_MissleSpeed; 
	pMissle->m_nCurLifeTime				= m_MissleLifeTime;
	//pMissle->m_nHeight		= m_MissleHeight;  
}

#endif

#ifdef _SERVER
void KSkill::CalcDamage(int *pNpcDamage, 
						int *pNpcDefend, 
						SkillDamageInfo *pSkillPrivateDam, 
						OutputDamageInfo *pRst
						)
{
	for(int i = 0; i < dot_end; ++i)
	{
		int nPrivateNpcPerc = pSkillPrivateDam ? pSkillPrivateDam[i].nNpcDamagePercent : 0;
		int nPrivateVal = pSkillPrivateDam ? pSkillPrivateDam[i].nVal : 0;

		pRst[i].nTargetType = m_DamageInfo[i].nTargetType;

		int nDamage = (m_DamageInfo[i].nVal + nPrivateVal + 
						pNpcDamage[i] * (m_DamageInfo[i].nNpcDamagePercent + nPrivateNpcPerc) / 100);

		int	nDefendDeno = pNpcDefend[i] + 200;

		if(nDefendDeno < 20)
			nDefendDeno = 20;

		if(dot_farphysics == i || dot_nearphysics == i)
		{
			int nDecDam = nDamage;
			int defendReduce = 50 - 10000 / nDefendDeno;
			if (defendReduce > 0)
				nDecDam -= defendReduce;

			if(nDecDam < 0)
				nDecDam = 0;
			
			pRst[i].nVal = nDecDam * 200 / nDefendDeno;
		}
		else
		{
			pRst[i].nVal = nDamage * 200 / nDefendDeno;
		}

		if(0 == pRst[i].nVal)
		{
			if(nDamage > 0)
				pRst[i].nVal = 1;
			else if(nDamage < 0)
				pRst[i].nVal = -1;
		}
	}
}
#endif
