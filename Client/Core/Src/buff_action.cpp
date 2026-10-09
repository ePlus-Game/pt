//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-28 17:00
//      File_base        : buff_action
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KNpc.h"
#include "buff_def.h"
#include "buff_action.h"
#include "buff_tab.h"
#include "buff_man.h"
#include "MagicAttribute.h"
#include "KSkills.h"
#include "SkillManager.h"
#include "KWin32.h"
#include "KCreature.h"
#include "ChatCenter_S.h"
#include "KSubWorld.h"
#include "ServerSocialUnitMgr.h"
#include "KPlayer.h"
#include "KSubWorldSet.h"
#include "SocialSerializer.h"
#include "npc_save.h"
#include "KNpcTemplate.h"
#include "KObjSet.h"
#include "ScriptFuns.h"
#include "KWarInfoManager.h"
#include "tong_war_manager.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

//=====================================================================================

int buff_none(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	return TRUE;
}

int buff_ismap(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );

		return ( Param[0] == SubWorld[nWorldIndex].m_SubWorldID );
	}

	return FALSE;
}
int buff_ismapgroup(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		return ( Param[0] == SubWorld[nWorldIndex].GetGroup( ) );
	}
	
	return FALSE;
}

int buff_ismapcate(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		return ( Param[0] == SubWorld[nWorldIndex].GetCate( ) );
	}
	
	return FALSE;
}

int buff_issowner( 
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 &&
		Env.nBuffRecever < MAX_NPC )
	{
		if( Npc[Env.nBuffRecever].IsPlayer( ) )
		{
			int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx( );
			RelationSet& RS = Player[nPlayerIndex].GetRelationSet( );

			RelationRecord* pRec = RS.GetRelationByTemplate( enSUTplId_Tong );

			if( pRec )
			{
				SocialUnit* pParent = pRec->pLeafUnit;
				
				while( pParent )
				{
					int nLayer = pParent->GetLayer( );

					if( nLayer == Param[0] &&
						pParent->IsOwner( Player[nPlayerIndex].GetPlayerName( ) ) )
						return TRUE;
					
					pParent = pParent->GetParent( );
				}
			}
		}
	}
	
	return FALSE;
}

int buff_ishavebuff( 
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{	
		return BuffMgr::Singleton( ).IsHaveBuff( Env.nBuffRecever, Param[0] );
	}

	return FALSE;
}

int buff_isequalpile( 
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{	
		return BuffMgr::Singleton( ).IsEqualPile( Env.nBuffRecever, Param[0], Param[1] );
	}
	
	return FALSE;
}

int buff_isworldlordhavebuff(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever >= 0 &&
		Env.nBuffRecever < MAX_NPC )	
	{
		int	nSubWorldIdx = Npc[Env.nBuffRecever].GetSubWorldIndex();
		int	nWorldLordIdx = SubWorld[nSubWorldIdx].GetLord();

		if (IsValidNpc(nWorldLordIdx))
		{
			return BuffMgr::Singleton().IsHaveBuff(nWorldLordIdx, Param[0]);
		}
	}

	return FALSE;
}


int buff_ishaveworldrobber( 
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{	
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		if( SubWorld[nWorldIndex].GetRobber( ) == INVALID_VALUE )
			return FALSE;
		else
			return TRUE;
	}

	return FALSE;
}


int buff_issamenpclord( 
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{

	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		const FSGUID *pRecvGuid = NULL;
		const FSGUID *pSendGuid = NULL;

		if( Npc[Env.nBuffRecever].IsPlayer() )
		{
			int nRecvPlayerIdx = Npc[Env.nBuffRecever].GetPlayerIdx();
			pRecvGuid = GetUnitGuid(nRecvPlayerIdx, enSUTplId_Tong, Param[0]);
		}
		else
		{
			pRecvGuid = &Npc[Env.nBuffRecever].GetLord();
		}

		if( Npc[Env.nBuffSender].IsPlayer() )
		{
			int nSendPlayerIdx = Npc[Env.nBuffSender].GetPlayerIdx();
			pSendGuid = GetUnitGuid(nSendPlayerIdx, enSUTplId_Tong, Param[0]);
		}
		else
		{
			pSendGuid = &Npc[Env.nBuffSender].GetLord();
		}

		if(NULL == pRecvGuid || NULL == pSendGuid)
			return FALSE;
		else
			return (*pRecvGuid) == (*pSendGuid);		
	}

	return FALSE;
}

int buff_issamenpcrobber( 
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		FSGUID SGUID = Npc[Env.nBuffSender].GetRobber( );
		
		if( Npc[Env.nBuffRecever].IsPlayer( ) )
		{
			int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx( );
			RelationSet& RS = Player[nPlayerIndex].GetRelationSet( );
			
			RelationRecord* pRec = RS.GetRelationByTemplate( enSUTplId_Tong );
			
			if( pRec )
			{
				SocialUnit* pParent = pRec->pLeafUnit;
				
				while( pParent )
				{
					const FSGUID& TGUID =  pParent->GetUnitGuid( );
					if( SGUID == TGUID )
						return TRUE;
					
					pParent = pParent->GetParent( );
				}
			}
		}
		else
		{
			const FSGUID& TGUID = Npc[Env.nBuffRecever].GetRobber( );
			
			if( SGUID == TGUID )
				return TRUE;
		}
	}
	
	return FALSE;
}

int buff_isworldowner(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		if( Npc[Env.nBuffRecever].IsPlayer( ) )
		{
			int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx( );
			RelationSet& RS = Player[nPlayerIndex].GetRelationSet( );
			
			RelationRecord* pRec = RS.GetRelationByTemplate( enSUTplId_Tong );
			
			if( pRec )
			{
				int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
				int nLord = SubWorld[nWorldIndex].GetLord( );
				
				if( nLord != INVALID_VALUE )
				{
					FSGUID LGUID = Npc[nLord].GetLord( );

					SocialUnit* pParent = pRec->pLeafUnit;
					
					while( pParent )
					{
						const FSGUID& TGUID =  pParent->GetUnitGuid( );

						if( LGUID == TGUID )
							return TRUE;
						
						pParent = pParent->GetParent( );
					}
				}
			}
		}
	}

	return FALSE;
}
//=====================================================================================

int buff_property_add(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		MagicData	MD;
		MD.nMagicNo = Param[0];
		MD.nVal		= Param[1] * Env.nBuffPileCount;
		
		g_MagicAttrModifier.ModifyMagicAttr(
			Env.nBuffRecever,
			&MD );

		return TRUE;
	}

	return FALSE;
}

int buff_addbufftorecver(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		BuffMgr &mgr = BuffMgr::Singleton();

		if( Env.nEventType )
		{
			if( Env.nEventRecever >= 0 && 
				Env.nEventRecever < MAX_NPC &&
				Env.nEventSender >= 0 &&
				Env.nEventSender < MAX_NPC )
			{
				mgr.AddNpcBuff(
					Env.nBuffRecever,
					Env.nEventRecever, 
					Param[0],
					FALSE );
				
				return TRUE;
			}
		}
		else
		{
			mgr.AddNpcBuff(
				Env.nBuffSender,
				Env.nBuffRecever, 
				Param[0],
				FALSE );
			
			return TRUE;
		}
	}
	
	return FALSE;
}

int buff_addbufftosender(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventSender >= 0 &&
		Env.nEventSender < MAX_NPC &&
		Env.nEventType )
	{
		BuffMgr &mgr = BuffMgr::Singleton();

		mgr.AddNpcBuff(
			Env.nEventRecever,
			Env.nEventSender, 
			Param[0],
			FALSE );
		
		return TRUE;
	}
	
	return FALSE;
}

int buff_decfromrecver(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		BuffMgr &mgr = BuffMgr::Singleton();
		
		mgr.DecBuffPile(
			Env.nBuffRecever,
			Param[0] );
		
		return TRUE;
	}
	
	return FALSE;
}

int buff_bsaddtoes(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param)
{
	if( Env.nEventSender >= 0 &&
		Env.nEventSender < MAX_NPC &&
		Env.nEventType )
	{
		BuffMgr &mgr = BuffMgr::Singleton();

		mgr.AddNpcBuff(
			Env.nBuffSender,
			Env.nEventSender,
			Param[0],
			FALSE );
		
		return TRUE;
	}
	
	return FALSE;
}

int buff_cast_skill(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		KSkill* pSkill = g_SkillManager.GetSkill( Param[0], 1 );
		if( pSkill )
		{
			int nSkillType = pSkill->GetAttackTargetType( ) & att_target_only ? SKILL_SPT_TargetIndex : 0;
			int nParam1	=	nSkillType;
			int nParam2	=	Env.nBuffRecever;
			
			if( pSkill->GetAttackTargetType( ) & att_target_only )
			{
				nParam1	=	nSkillType;
				nParam2	=	Env.nBuffRecever;
			}
			else
			{
				Npc[Env.nBuffSender].GetMpsPos(&nParam1, &nParam2);
			}
			
			if( pSkill->CanCastSkill( Env.nBuffSender, nParam1, nParam2 ) )
			{
				if( Npc[Env.nBuffSender].Cost(pSkill) )
				{
					for( int nLoopCount = 0; nLoopCount < Env.nBuffPileCount; nLoopCount++ )
						pSkill->Cast( Env.nBuffSender, nSkillType, Env.nBuffRecever );
					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
}

int buff_cast_skill_ext(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		KSkill* pSkill = g_SkillManager.GetSkill( Param[0], 1 );
		if( pSkill )
		{
			int nSkillType = pSkill->GetAttackTargetType( ) & att_target_only ? SKILL_SPT_TargetIndex : 0;
			int nParam1	=	nSkillType;
			int nParam2	=	Env.nBuffRecever;
			
			if( pSkill->GetAttackTargetType( ) & att_target_only )
			{
				nParam1	=	nSkillType;
				nParam2	=	Env.nBuffRecever;
			}
			else
			{
				Npc[Env.nBuffSender].GetMpsPos(&nParam1, &nParam2);
			}
			
			if( pSkill->CanCastSkill( Env.nBuffSender, nParam1, nParam2 ) )
			{
				if( Npc[Env.nBuffSender].Cost(pSkill) )
				{
					for( int nLoopCount = 0; nLoopCount < Env.nBuffPileCount; nLoopCount++ )
						pSkill->Cast( 
							Env.nBuffSender, 
							nSkillType, 
							Env.nBuffRecever, 
							0, 
							SKILL_SLT_Npc, 
							0, 
							Env.nBuffRecever );

					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
}

int buff_cast_skill_tag(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		KSkill* pSkill = g_SkillManager.GetSkill( Param[0], 1 );
		if( pSkill )
		{
			int nSkillType = pSkill->GetAttackTargetType( ) & att_target_only ? SKILL_SPT_TargetIndex : 0;
			int nParam1	=	nSkillType;
			int nParam2	=	Env.nBuffRecever;
			
			if( pSkill->GetAttackTargetType( ) & att_target_only )
			{
				nParam1	=	nSkillType;
				nParam2	=	Env.nBuffRecever;
			}
			else
			{
				Npc[Env.nBuffRecever].GetMpsPos(&nParam1, &nParam2);
			}
			
			if( pSkill->CanCastSkill( Env.nBuffRecever, nParam1, nParam2 ) )
			{
				if( Npc[Env.nBuffSender].Cost(pSkill) )
				{
					for( int nLoopCount = 0; nLoopCount < Env.nBuffPileCount; nLoopCount++ )
						pSkill->Cast( Env.nBuffRecever, nSkillType, Env.nBuffRecever );
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

int buff_enable_skill(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		int nIdx = Npc[Env.nBuffRecever].m_SkillList.FindSkill( Param[0] );
		Npc[Env.nBuffRecever].m_SkillList.ChangeStatus( 
		nIdx, Param[1] ? skill_status_usable : skill_status_active );
		return TRUE;
	}

	return FALSE;
}

int buff_enable_skill_g(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		NpcSkillList& SkillList = Npc[Env.nBuffRecever].GetSkillList( );
		NpcSkillList::Iterator iter;
		int		nSkillIdx;

		while( (nSkillIdx = SkillList.NextSkillIdx(iter)) != INVALID_SKILL_INDEX )
		{
			int nSkillID = SkillList.GetIdByIdx( nSkillIdx );
			int nLevel = SkillList.GetLevelByIdx( nSkillIdx );
			
			for( int nLoopCountS = 1; nLoopCountS <= nLevel; nLoopCountS++ )
			{
				KSkill* pSkill = g_SkillManager.GetSkill( nSkillID, nLoopCountS );
				
				if( pSkill &&
					pSkill->GetGroup( ) == Param[0] )
				{
					int nIdx = Npc[Env.nBuffRecever].m_SkillList.FindSkill( nSkillID );
					SkillList.ChangeStatus( nIdx, Param[1] ? skill_status_usable : skill_status_active );
				}
			}
		}
	}

	return TRUE;
}

int buff_enable_skill_c(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		NpcSkillList& SkillList = Npc[Env.nBuffRecever].GetSkillList( );
		NpcSkillList::Iterator iter;
		int		nSkillIdx;
		
		while( (nSkillIdx = SkillList.NextSkillIdx(iter)) != INVALID_SKILL_INDEX )
		{
			int nSkillID = SkillList.GetIdByIdx( nSkillIdx );
			int nLevel = SkillList.GetLevelByIdx( nSkillIdx );
			
			for( int nLoopCountS = 1; nLoopCountS <= nLevel; nLoopCountS++ )
			{
				KSkill* pSkill = g_SkillManager.GetSkill( nSkillID, nLoopCountS );
				
				if( pSkill &&
					pSkill->GetCategory( Param[1] ) == Param[0] )
				{
					int nIdx = Npc[Env.nBuffRecever].m_SkillList.FindSkill( nSkillID );
					SkillList.ChangeStatus( nIdx, Param[2] ? skill_status_usable : skill_status_active );
				}
			}
		}
	}
	
	return TRUE;
}

int buff_nomove(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		Npc[Env.nBuffRecever].NoMove( Param[0] );
		return TRUE;
	}

	return FALSE;
}

int buff_clearbuff(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		BuffMgr &mgr = BuffMgr::Singleton();
		
		mgr.ClearBuffByGroup( Env.nBuffRecever, Param[0] );
		
		return TRUE;
	}

	return FALSE;
}

int buff_clearbuff_c(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		BuffMgr &mgr = BuffMgr::Singleton();
		
		mgr.ClearBuffByCate( Env.nBuffRecever, Param[0], Param[1] );
		
		return TRUE;
	}
	
	return FALSE;
}

int buff_dorevive(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		if( Npc[Env.nBuffRecever].IsPlayer( ) )
		{
			int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx( );
			Player[nPlayerIndex].Revive( FALSE);
			return TRUE;
		}
	}
	return FALSE;
}

int buff_dodeath(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( IsValidNpc(Env.nBuffRecever) && Npc[Env.nBuffRecever].IsAlive() )
	{
		//		if( Npc[Env.nBuffRecever].IsPlayer( ) )
		{
			Npc[Env.nBuffRecever].m_UnaryAttrMgr.Set(nuai_curlife, 0);
			Npc[Env.nBuffRecever].SendCommand(do_death, 0, 0, 0);
			Npc[Env.nBuffRecever].GetController().SetActive(false);
			Npc[Env.nBuffRecever].SetProcessAI(TRUE);
			
		}//endif
		
		return TRUE;
	}
	return FALSE;
}

int buff_drawme(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		int nX,nY;
		Npc[Env.nBuffRecever].GetMpsPos( &nX, &nY );
		nX -= 2;
		nY -= 2;
		Npc[Env.nBuffSender].SetPos( nX, nY );

		return TRUE;
	}

	return FALSE;
}

int buff_cooldown_skill_g(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		NpcSkillList& SkillList = Npc[Env.nBuffRecever].m_SkillList;
		NpcSkillList::Iterator iter;
		int		nSkillIdx;

		while( (nSkillIdx = SkillList.NextSkillIdx(iter)) != INVALID_SKILL_INDEX )
		{
			int nSkillID = SkillList.GetIdByIdx( nSkillIdx );
			int nLevel = SkillList.GetLevelByIdx( nSkillIdx );
			
			for( int nLoopCountS = 1; nLoopCountS <= nLevel; nLoopCountS++ )
			{
				KSkill* pSkill = g_SkillManager.GetSkill( nSkillID, nLoopCountS );
				
				if( pSkill &&
					pSkill->GetGroup( ) == Param[0] )
				{
					Npc[Env.nBuffRecever].m_SkillList.ClearCoolDown( nSkillID );
				}
			}
		}
	}

	return FALSE;
}

int buff_cooldown_skill_c(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		NpcSkillList& SkillList = Npc[Env.nBuffRecever].m_SkillList;
		NpcSkillList::Iterator iter;
		int		nSkillIdx;
		
		while( (nSkillIdx = SkillList.NextSkillIdx(iter)) != INVALID_SKILL_INDEX )
		{
			int nSkillID = SkillList.GetIdByIdx( nSkillIdx );
			int nLevel = SkillList.GetLevelByIdx( nSkillIdx );
			
			for( int nLoopCountS = 1; nLoopCountS <= nLevel; nLoopCountS++ )
			{
				KSkill* pSkill = g_SkillManager.GetSkill( nSkillID, nLoopCountS );
				
				if( pSkill &&
					pSkill->GetCategory( Param[1] ) == Param[0]  )
				{
					Npc[Env.nBuffRecever].m_SkillList.ClearCoolDown( nSkillID );
				}
			}
		}
	}
	
	return FALSE;
}

int buff_killcreature(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		KCreature& aCreature = Player[Npc[Env.nBuffRecever].GetPlayerIdx( )].m_Creature;
		aCreature.Dismiss( );

		return TRUE;
	}
	
	return FALSE;
}

int buff_saytoall(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	g_ChatCenterS.SysMsgToAll(SYSMSG_TYPE_ID, (const BYTE*)&Param[0], sizeof(int));
	
	return FALSE;
}

int buff_saytochannel(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( (Env.nBuffRecever >= 0 && Env.nBuffRecever < MAX_NPC) && 
		(Param[0] >= SYSTEM_ROOM_ID && Param[0] <= COMBAT_INFO_ROOM_ID) &&
		(Param[1] > 0 && Param[1] <= 60000) )
	{
		g_ChatCenterS.ChatInRoomByStringID( Env.nBuffRecever, Param[0], Param[1] );
		return TRUE;
	}
	
	return FALSE;
}

int buff_addbufftoworldlord(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		int nLordIndex = SubWorld[nWorldIndex].GetLord( );
		
		BuffMgr &mgr = BuffMgr::Singleton();
	
		mgr.AddNpcBuff(
			Env.nBuffSender,
			nLordIndex, 
			Param[0],
			FALSE );

		if(INVALID_WORLDLORDNPC_INDEX != nLordIndex)
			Npc[nLordIndex].SetDataChangedFlag(true);

		return TRUE;
	}

	return FALSE;
}

int buff_addbufftoworldpool(
							BUFF_ENV_PARAM& Env,
							BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		int nPoolIndex = SubWorld[nWorldIndex].GetPool( );
		
		if (IsValidNpc(nPoolIndex))
		{
			BuffMgr &mgr = BuffMgr::Singleton();
			
			mgr.AddNpcBuff(
				Env.nBuffSender,
				nPoolIndex, 
				Param[0],
				FALSE );

			Npc[nPoolIndex].SetDataChangedFlag(true);
		}//endif
		else
		{
			_ASSERT(false);
		}
		
		return TRUE;
	}
	
	return FALSE;
}

int buff_addbufftoworldsubpool(
							BUFF_ENV_PARAM& Env,
							BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		int nWorldIndex   = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		int nSubPoolIndex = SubWorld[nWorldIndex].GetSubPool( );
		
		if (IsValidNpc(nSubPoolIndex))
		{
			BuffMgr &mgr = BuffMgr::Singleton();
			
			mgr.AddNpcBuff(
				Env.nBuffSender,
				nSubPoolIndex, 
				Param[0],
				FALSE );
			
			Npc[nSubPoolIndex].SetDataChangedFlag(true);
		}//endif
		else
		{
			_ASSERT(false);
		}
		
		return TRUE;
	}
	
	return FALSE;
}

int buff_addbufftoworldrobber(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );

		int nRobberIndex = SubWorld[nWorldIndex].GetRobber( );
		
		BuffMgr &mgr = BuffMgr::Singleton();
		
		mgr.AddNpcBuff(
			Env.nBuffSender,
			nRobberIndex, 
			Param[0],
			FALSE );

		if(INVALID_WORLDLORDNPC_INDEX != nRobberIndex)
			Npc[nRobberIndex].SetDataChangedFlag(true);
		
		return TRUE;
	}

	return FALSE;
}

int buff_addbufftosubworldlord(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		if( Param[1] == -1 )
		{
			int nSubLordIndex;
			BuffMgr &mgr = BuffMgr::Singleton();
			
			for( int nLoopCount = 0; nLoopCount < SUBLORD_COUNT; nLoopCount++ )
			{
				nSubLordIndex = SubWorld[nWorldIndex].GetSubLord( nLoopCount );
				
				mgr.AddNpcBuff(
					Env.nBuffSender,
					nSubLordIndex, 
					Param[0],
					FALSE );

				if(INVALID_WORLDLORDNPC_INDEX != nSubLordIndex)
					Npc[nSubLordIndex].SetDataChangedFlag(true);
			}
		}
		else
		{
			int nSubLordIndex = SubWorld[nWorldIndex].GetSubLord( Param[2] );
			
			BuffMgr &mgr = BuffMgr::Singleton();
			
			mgr.AddNpcBuff(
				Env.nBuffSender,
				nSubLordIndex, 
				Param[0],
				FALSE );

			if(INVALID_WORLDLORDNPC_INDEX != nSubLordIndex)
				Npc[nSubLordIndex].SetDataChangedFlag(true);
		}
		
		return TRUE;
	}

	return FALSE;
}

int buff_addbufftospecifyworldlord(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	// Param specification:
	// Param[0]: map Id
	// Param[1]: sublord index which buff add to, -1 means add to world lord
	// Param[2]: buff id

	int nWorldIndex = g_SubWorldSet.SearchWorld(Param[0]);

	if(-1 != nWorldIndex)
	{
		int nRecvNpcIdx = -1;

		if(-1 == Param[1])	
		{
			nRecvNpcIdx = SubWorld[nWorldIndex].GetLord( );
		}
		else if(Param[1] >= 0 && Param[1] < SUBLORD_COUNT)
		{
			nRecvNpcIdx = SubWorld[nWorldIndex].GetSubLord( Param[1] );
		}

		if(-1 != nRecvNpcIdx)
		{
			BuffMgr &mgr = BuffMgr::Singleton();

			mgr.AddNpcBuff(
				Env.nBuffSender,
				nRecvNpcIdx,
				Param[2],
				FALSE);

			Npc[nRecvNpcIdx].SetDataChangedFlag(true);

			return TRUE;
		}
	}

	return FALSE;
}

int buff_addblifnolwl(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	// if the city doesn't have owner, than add a buff to its lord
	// else do nothing

	int nWorldIndex = Npc[Env.nBuffSender].GetSubWorldIndex();
	int nWorldLordIndex = SubWorld[nWorldIndex].GetLord();

	if(INVALID_WORLDLORDNPC_INDEX != nWorldLordIndex)
	{
		const FSGUID &guid = Npc[nWorldLordIndex].GetLord();

		if( !IsGUIDValid(guid) )
		{
			BuffMgr &mgr = BuffMgr::Singleton();

			mgr.AddNpcBuff(
				nWorldLordIndex,
				nWorldLordIndex,
				Param[0],
				FALSE );

			return TRUE;
		}
	}

	return FALSE;
}

int buff_addbufftoBRL(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever < 0 || Env.nBuffRecever > MAX_NPC )
		return FALSE;
	
	//接受者为Player
	if( Npc[Env.nBuffRecever].IsPlayer( ) )
		return FALSE;
	
	int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
	
	int nRobIndex = SubWorld[nWorldIndex].GetRobber( );
	
	const FSGUID& LGUID = Npc[nRobIndex].GetLord( );
	
	ServerSocialUnitMgr& SMgr = ServerSocialUnitMgr::Singleton( );
	
	SocialUnit* pUnit = SMgr.GetUnit( LGUID );
	
	if( pUnit )
	{
		SocialUnitAttr& Attr = pUnit->GetUnitAttr(  );
		if( Attr.IsAttrHasData( enSUAttr_CityMap ) )
		{
			char* pData = NULL;
			int nSize=Attr.GetAttr( enSUAttr_CityMap, pData );
			if (nSize == sizeof(int))
			{
				int nMapID = *((int*)pData);
				int nRobWorldIndex = g_SubWorldSet.SearchWorld( nMapID );
				if( nRobWorldIndex != -1 )
				{
					int nRobWorldLordIndex = SubWorld[nRobWorldIndex].GetLord( );
					
					if( nRobWorldLordIndex != -1 )
					{
						BuffMgr &mgr = BuffMgr::Singleton();
						
						mgr.AddNpcBuff(
							Env.nBuffRecever,
							nRobWorldLordIndex, 
							Param[0],
							FALSE );
					}
				}
			}
		}
	}
	
	return FALSE;
}

int buff_swapnpclordrobber(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		bool IsRobberHasCity = false;
		FSGUID	robberGuid = Npc[Env.nBuffRecever].GetRobber();

		ServerSocialUnitMgr	&mgr = ServerSocialUnitMgr::Singleton();

		if( IsGUIDValid(robberGuid) )
		{
			SocialUnit *pCityOwnerUnit = mgr.GetUnit(robberGuid, enSUTplId_Tong);
			
			if(NULL != pCityOwnerUnit)
			{
				int nCityMapId = GetCityMapId(pCityOwnerUnit->GetUnitAttr());

				if(INVALID_WORLD_ID != nCityMapId)
				{
					int nMapIdx = g_SubWorldSet.SearchWorld(nCityMapId);
					IsRobberHasCity = (INVALID_WORLD_INDEX != nMapIdx);

					// 如果数据错乱，则清空
					if(INVALID_WORLD_INDEX == nMapIdx)
					{
						_ASSERT(false);
						pCityOwnerUnit->GetUnitAttr().DelAttr(enSUAttr_CityMap);
					}
				}
			}
		}

		// 如果进攻方没有城市，则占领该城市，同时从防守方清除该城市
		if( !IsRobberHasCity )
		{
			
			SocialUnit	*pUnitAttacker = mgr.GetUnit(robberGuid, enSUTplId_Tong);

			if(pUnitAttacker)
			{
				int	nWorldIdx = Npc[Env.nBuffRecever].m_SubWorldIndex;
				int	nMapId = SubWorld[nWorldIdx].m_SubWorldID;
				SocialUnitAttr	&attr = pUnitAttacker->GetUnitAttr();
				attr.AddAttr(enSUAttr_CityMap, (const char*)&nMapId, sizeof(nMapId));
				SocialSerializer::Singleton().UpdateAttrReq(-1, pUnitAttacker);
			}


			const FSGUID &guidDefender = Npc[Env.nBuffRecever].GetLord();

			if( IsGUIDValid(guidDefender) )
			{
				SocialUnit	*pUnitDefender = mgr.GetUnit(guidDefender, enSUTplId_Tong);

				if(pUnitDefender)
				{
					SocialUnitAttr	&attr = pUnitDefender->GetUnitAttr();
					attr.DelAttr(enSUAttr_CityMap);
					SocialSerializer::Singleton().UpdateAttrReq(-1, pUnitDefender);
				}
			}

			Npc[Env.nBuffRecever].SetLord(robberGuid);
		}

		FSGUID guid;
		Npc[Env.nBuffRecever].SetRobber( guid );
		Npc[Env.nBuffRecever].SetDataChangedFlag(true);
	}

	return FALSE;
}

int buff_setnpclord(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{

		if( Npc[Env.nBuffRecever].IsPlayer( ) )
			return FALSE;
		
		if( Param[0] )
		{
			FSGUID guid;
			
			if( Npc[Env.nBuffSender].IsPlayer( ) )
			{
				int nPlayerIndex = Npc[Env.nBuffSender].GetPlayerIdx( );
				RelationSet& RS = Player[nPlayerIndex].GetRelationSet( );
				
				RelationRecord* pRec = RS.GetRelationByTemplate( enSUTplId_Tong );
				
				if( pRec )
				{
					SocialUnit* pUp = GetUpNUnit( pRec->pLeafUnit, Param[1] );
					
					if( pUp )
					{
						const FSGUID& GUID = pUp->GetUnitGuid( );
						Npc[Env.nBuffRecever].SetLord( GUID );
						Npc[Env.nBuffRecever].SetDataChangedFlag(true);
					}
				}
			}
			else
			{
				const FSGUID& GUID = Npc[Env.nBuffSender].GetLord( );
				Npc[Env.nBuffRecever].SetLord( GUID );
				Npc[Env.nBuffRecever].SetDataChangedFlag(true);
			}
		}
		else
		{
			FSGUID guid;
			Npc[Env.nBuffRecever].SetLord( guid );
			Npc[Env.nBuffRecever].SetDataChangedFlag(true);
		}
		
		return TRUE;
	}

	return FALSE;
}

int buff_setnpcrobber(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		
		if( Npc[Env.nBuffRecever].IsPlayer( ) )
			return FALSE;
		
		if( Param[0] )
		{
			FSGUID guid;
			
			if( Npc[Env.nBuffSender].IsPlayer( ) )
			{
				int nPlayerIndex = Npc[Env.nBuffSender].GetPlayerIdx( );
				RelationSet& RS = Player[nPlayerIndex].GetRelationSet( );
				
				RelationRecord* pRec = RS.GetRelationByTemplate( enSUTplId_Tong );
				
				if( pRec )
				{
					SocialUnit* pUp = GetUpNUnit( pRec->pLeafUnit, Param[1] );
					
					if( pUp )
					{
						const FSGUID& GUID = pUp->GetUnitGuid( );
						Npc[Env.nBuffRecever].SetRobber( GUID );
						Npc[Env.nBuffRecever].SetDataChangedFlag(true);
					}
				}
			}
			else
			{
				const FSGUID& GUID = Npc[Env.nBuffSender].GetLord( );
				Npc[Env.nBuffRecever].SetRobber( GUID );
				Npc[Env.nBuffRecever].SetDataChangedFlag(true);
			}
		}
		else
		{
			FSGUID guid;
			Npc[Env.nBuffRecever].SetRobber( guid );
			Npc[Env.nBuffRecever].SetDataChangedFlag(true);
		}
		
		return TRUE;
	}
	
	return FALSE;
}

int buff_setworldlord(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		if( Param[0] )
		{
			SubWorld[nWorldIndex].SetLord( Env.nBuffRecever );
		}
		else
		{
			SubWorld[nWorldIndex].SetLord( -1 );
		}
		return TRUE;
	}

	return FALSE;
}

int buff_setworldPool(
					  BUFF_ENV_PARAM& Env,
					  BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		if( Param[0] )
		{
			SubWorld[nWorldIndex].SetPool( Env.nBuffRecever );
		}
		else
		{
			SubWorld[nWorldIndex].SetPool( -1 );
		}
		return TRUE;
	}
	
	return FALSE;
}


int buff_setworldSubPool(
					  BUFF_ENV_PARAM& Env,
					  BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		if( Param[0] )
		{
			SubWorld[nWorldIndex].SetSubPool( Env.nBuffRecever );
		}
		else
		{
			SubWorld[nWorldIndex].SetSubPool( -1 );
		}
		return TRUE;
	}
	
	return FALSE;
}



int buff_setworldrobber(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		if( Param[0] )
		{
			SubWorld[nWorldIndex].SetRobber( Env.nBuffRecever );
		}
		else
		{
			SubWorld[nWorldIndex].SetRobber( -1 );
		}
		return TRUE;
	}
	
	return FALSE;
}

int buff_setworldsublord(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		if( Param[0] )
		{
			SubWorld[nWorldIndex].SetSubLord( Param[1], Env.nBuffRecever );
		}
		else
		{
			SubWorld[nWorldIndex].SetSubLord( Param[1], -1 );
		}
		return TRUE;
	}

	return FALSE;
}

#define NPCTAG	"NPCNAMETAG"

int buff_addnpc(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		int nX,nY;
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		Npc[Env.nBuffRecever].GetMpsPos( &nX, &nY );
		
		// Param[0] -- ID Param[1] -- Level
		// Level ID
		int	nNpcIdxInfo = MAKELONG(Param[1], Param[0]);
		
//		nX += Param[3];
//		nY += Param[4];
//		if( SubWorld[nWorldIndex].TestBarrier( nX, nY ) )
//			return FALSE;

		for( int nLoopCount = 0; nLoopCount < Env.nBuffPileCount; nLoopCount++ )
		{
			int nNpcIdx = NpcSet.Add(
				nNpcIdxInfo, 
				nWorldIndex, 
				nX, 
				nY );
			
			if( nNpcIdx > 0 )
			{
				if( !strcmp( Npc[nNpcIdx].Name, NPCTAG ) )
					strcpy( Npc[nNpcIdx].Name, Npc[Env.nBuffSender].Name );

				Npc[nNpcIdx].NormalSync( );

				int nMode = Npc[nNpcIdx].m_UnaryAttrMgr[nuai_deathmode];
				nMode |= npc_deathmode_autodel;
				Npc[nNpcIdx].m_UnaryAttrMgr.Set( nuai_deathmode, nMode );

				nX += Param[2];
				nY += Param[3];
				Npc[nNpcIdx].SendCommand( do_run, nX, nY );

				const KNpcTemplate *pTemplate = Npc[nNpcIdx].GetTemplate();
				if( NULL != pTemplate && pTemplate->NeedSave() )
					Npc[nNpcIdx].SetDataChangedFlag(true);

				if( Npc[Env.nBuffSender].IsPlayer( ) )
				{
					int nPlayerIndex     = Npc[Env.nBuffSender].GetPlayerIdx( );
					RelationSet& RS      = Player[nPlayerIndex].GetRelationSet( );
					
					RelationRecord* pRec = RS.GetRelationByTemplate( enSUTplId_Tong );
					
					if( pRec )
					{
						SocialUnit* pParent = pRec->pLeafUnit;
						
						while( pParent )
						{
							int nLayer = pParent->GetLayer( );
							
							if( nLayer == Param[4] )
							{
								const FSGUID& GUID = pParent->GetUnitGuid( );
								Npc[nNpcIdx].SetLord( GUID );
							}
							
							pParent = pParent->GetParent( );
						}
					}
				}
				else
				{
					const FSGUID& GUID    = Npc[Env.nBuffSender].GetLord( );
					const FSGUID  Invalid ; 
					if (Param[4]>=enSULayer_Player && Param[4]<enSUTong_LayerNum && GUID!=Invalid)
					{
						SocialUnit * pUnit=ServerSocialUnitMgr::Singleton().GetUnit(GUID,enSUTplId_Tong);
						if (pUnit && pUnit->GetLayer()>=Param[4])
						{	
							while (pUnit && pUnit->GetLayer()!=Param[4])
							{
								pUnit = pUnit->GetParent();	
							}//end for while
							
							if (pUnit)
								Npc[nNpcIdx].SetLord( pUnit->GetUnitGuid() );	
						}//endif
					}//endif
				}
				return TRUE;
			}
			else
				return FALSE;
		}
	}
	return FALSE;
}

int buff_setpreventaddfriend( BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if (IsValidNpc(Env.nBuffRecever) && Npc[Env.nBuffRecever].m_Kind == kind_player)
	{
		int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if (IsValidPlayer(nPlayerIndex))
		{
			Player[nPlayerIndex].SetPreventAddFriend(Param[0]);
			return TRUE;
		}//endif

	}//endif

	return FALSE;
}


int buff_addobj(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender >= 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		KMapPos	Pos;
		Pos.nSubWorld = Npc[Env.nBuffRecever].m_SubWorldIndex;
		Pos.nRegion = Npc[Env.nBuffRecever].m_RegionIndex;
		Pos.nMapX = Npc[Env.nBuffRecever].GetMapX();
		Pos.nMapY = Npc[Env.nBuffRecever].GetMapY();
		Pos.nOffX =	Npc[Env.nBuffRecever].GetOffX();
		Pos.nOffY = Npc[Env.nBuffRecever].GetOffY();

		KObjItemInfo ObjInfo;
		int nObjId;
	
		if(0 == Param[0])
		{
			int nItemIdx = ItemSet.Add(
				Param[1],
				Param[2],
				Param[3],
				Param[4],
				1
				);

			if(nItemIdx <= 0)
				return FALSE;

			nObjId = nItemIdx;
			KItem &objItem = Item[nItemIdx];

			ObjInfo.m_nItemID = nItemIdx;
			ObjInfo.m_nMoneyNum = (objItem.GetMaxItemCount() == 0) ? 0 : objItem.GetItemCount();
			memcpy(ObjInfo.m_szName, objItem.GetName(), FILE_NAME_LENGTH );
			ObjInfo.m_szName[FILE_NAME_LENGTH - 1] = 0;
			ObjInfo.m_nColorID = objItem.GetQualityLabel();
			ObjInfo.m_nMovieFlag = 1;
			ObjInfo.m_nSoundFlag = 1;

			int nIndex = ObjSet.Add(objItem.GetObjIdx(), Pos, ObjInfo , -1);	

			if(nIndex > 0)
			{
				//ItemDebugLog Begin........................
                #ifdef _SERVER
				int nItemIndex = nItemIdx;
				if (nItemIndex > 0 && nItemIndex < MAX_ITEM && Item[nItemIndex].GetBelong() != -1)
				{
					int  nBelong         = Item[nItemIndex].GetBelong();
					char szDumpInfo[512] = "";
					snprintf(szDumpInfo,sizeof(szDumpInfo),"Buff Add:BuffID:%d,Index %d,Belong:%d,Name:%s\n",Env.nBuffTempID,nItemIndex,nBelong,Item[nItemIndex].GetName());
					szDumpInfo[sizeof(szDumpInfo) - 1] = 0;

					DumpInvalidItemOpeStack(false,szDumpInfo,4);
				}//endif
                #endif
                //ItemDebugLog End.........................

				//Object[nIndex].SetItemBelong(-1);
				
				//日志：系统掉落物品
				bool needLog = (objItem.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
				if (needLog)
				{
					LogEventParam systemDropItemEvent;
					systemDropItemEvent.event = log_event_system_drop_item;
					systemDropItemEvent.param2 = objItem.GetGUID();
					objItem.GetItemTemplateId(systemDropItemEvent.param3.data, sizeof(systemDropItemEvent.param3.data) - 1);
					g_pLogSystem->Log(systemDropItemEvent);
				}

				return TRUE;
			}
			else
				ItemSet.Remove(nItemIdx);
		}
		else
		{
			nObjId = Param[0];

			ObjInfo.m_nColorID = 1;
			ObjInfo.m_nItemID = 0;
			ObjInfo.m_nMoneyNum = 0;
			ObjInfo.m_nMovieFlag = 1;
			ObjInfo.m_nSoundFlag = 1;
			ObjInfo.m_nLauncher = Env.nBuffSender;
			memset(ObjInfo.m_szName, 0, sizeof(ObjInfo.m_szName));
			//strcpy( ObjInfo.m_szName, "trap" );

			int nIndex = ObjSet.Add(nObjId, Pos, ObjInfo);	

			if(nIndex > 0)
				return TRUE;
		}
	}
	
	return FALSE;
}

int buff_delnpc(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC && 
		!Env.nBeOPDec )
	{
		int nNpcIndex = Env.nBuffRecever;

		int nSubWorld = Npc[nNpcIndex].m_SubWorldIndex;
		int nRegion = Npc[nNpcIndex].m_RegionIndex;

		BuffMgr &mgr = BuffMgr::Singleton();
		mgr.ClearAllBuff( nNpcIndex, TRUE );
		
		SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(nNpcIndex);
		NpcSet.Remove(nNpcIndex);

	}
	return FALSE;
}

int buff_npcdrop(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		int nNpcIdx = Env.nBuffRecever;

		int nMode = Npc[nNpcIdx].m_UnaryAttrMgr[nuai_deathmode];
		if( !Param[0] )
		{
			nMode |= npc_deathmode_nodrop;
			Npc[nNpcIdx].m_UnaryAttrMgr.Set( nuai_deathmode, nMode );
		}
		else
		{
			nMode &= ~npc_deathmode_nodrop;
			Npc[nNpcIdx].m_UnaryAttrMgr.Set( nuai_deathmode, nMode );
		}

		return TRUE;
	}

	return FALSE;
}
//=====================================================================================

int buff_id_attribute(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;

	switch( Env.nEventType ) 
	{
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_delayskillout:
	case buff_event_type_finalskillout:
		{
			if( Param[0] == BUFF_INVALID )
				return TRUE;

			if( Env.nEventFormat & buff_event_format_skillid )
			{
				KSkill* pSkill = g_SkillManager.GetSkill( Env.nEvent, 1 );
				if( pSkill )
				{
					return ( pSkill->GetSkillId( ) == Param[0] );
				}
				else
					return FALSE;
			}
		}
		break;
	case buff_event_type_buffin:
	case buff_event_type_buffout:
		{
			if( Param[0] == BUFF_INVALID )
				return TRUE;
			
			if( Env.nEventFormat & buff_event_format_buffobj )
			{
				Buff* pBuff = (Buff*)Env.nEvent;
				return ( pBuff->GetBuffID() == Param[0] );

			}
			
			if( Env.nEventFormat & buff_event_format_buffid )
			{
				PBAT BAT = BuffTable::Singleton( ).GetBuff( Env.nEvent );
				if( BAT )
				{
					return ( BAT->nBuffID == Param[0] );
				}
				else
					return FALSE;
			}
		}
		break;
	case buff_event_type_damagein:
	case buff_event_type_damageout:
		{
			if( Param[0] == BUFF_INVALID )
				return TRUE;
			
			return ( Env.nEvent == Param[0] ); 
		}
		break;
	case buff_event_type_blood:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
		break;
	default:
		break;		
	}

	return FALSE;
}

int buff_group_attribute(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	if( Param[0] == BUFF_INVALID )
		return TRUE;
	
	switch( Env.nEventType ) 
	{
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_delayskillout:
	case buff_event_type_finalskillout:
		{
			if( Param[0] == BUFF_INVALID )
				return TRUE;
			
			if( Env.nEventFormat & buff_event_format_skillid )
			{
				KSkill* pSkill = g_SkillManager.GetSkill( Env.nEvent, 1 );
				if( pSkill )
				{
					return ( pSkill->GetGroup( ) == Param[0] );
				}
				else
					return FALSE;
			}
		}
		break;
	case buff_event_type_buffin:
	case buff_event_type_buffout:
		{
			if( Param[0] == BUFF_INVALID )
				return TRUE;
			
			if( Env.nEventFormat & buff_event_format_buffobj )
			{
				Buff* pBuff = (Buff*)Env.nEvent;
				return ( pBuff->GetGroup( ) == Param[0] );
			}
			
			if( Env.nEventFormat & buff_event_format_buffid )
			{
				PBAT BAT = BuffTable::Singleton( ).GetBuff( Env.nEvent );
				if( BAT )
				{
					return ( BAT->nGroup == Param[0] );
				}
				else
					return FALSE;
			}			
		}
		break;
	case buff_event_type_damagein:
	case buff_event_type_damageout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
	case buff_event_type_blood:
	default:
		break;		
	}
	
	return FALSE;
}

int buff_cate_attribute(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	if( Param[0] == BUFF_INVALID )
		return TRUE;
	
	switch( Env.nEventType ) 
	{
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_delayskillout:
	case buff_event_type_finalskillout:
		{
			if( Param[0] == BUFF_INVALID )
				return TRUE;
			
			if( Env.nEventFormat & buff_event_format_skillid )
			{
				KSkill* pSkill = g_SkillManager.GetSkill( Env.nEvent, 1 );
				if( pSkill )
				{
					return ( pSkill->GetCategory( Param[1] ) == Param[0] );
				}
				else
					return FALSE;
			}
		}
		break;
	case buff_event_type_buffin:
	case buff_event_type_buffout:
		{
			if( Param[0] == BUFF_INVALID )
				return TRUE;
			
			if( Env.nEventFormat & buff_event_format_buffobj )
			{
				Buff* pBuff = (Buff*)Env.nEvent;
				return ( pBuff->GetCategory( Param[1] ) == Param[0] );
			}
			
			if( Env.nEventFormat & buff_event_format_buffid )
			{
				PBAT BAT = BuffTable::Singleton( ).GetBuff( Env.nEvent );
				if( BAT )
				{
					return ( BAT->nCategory[Param[1]] == Param[0] );
				}
				else
					return FALSE;
			}
		}
		break;

	case buff_event_type_damagein:
	case buff_event_type_damageout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
	case buff_event_type_blood:
		break;
	default:
		break;		
	}
	
	return FALSE;
}

int buff_format_attribute(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	switch( Env.nEventType ) 
	{
	case buff_event_type_buffin:
	case buff_event_type_buffout:
		{
			if( Env.nEventFormat & Param[0] )
				return TRUE;

			return FALSE;
		}
		break;
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_damagein:
	case buff_event_type_damageout:
	case buff_event_type_blood:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
		break;
	default:
		break;		
	}

	return TRUE;
}

int buff_volume_attribute(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;

	if( Param[0] == BUFF_INVALID )
		return TRUE;
	
	switch( Env.nEventType )
	{
	case buff_event_type_blood:
		{
			if( Param[0] == BUFF_INVALID )
				return TRUE;

			int nNpcIndex = Env.nEventRecever;
			
			if( nNpcIndex < 0 || nNpcIndex > MAX_NPC )
				return FALSE;
			
			int nCurLife	= Npc[nNpcIndex].m_UnaryAttrMgr[nuai_curlife];
			int nLifeLimit	= Npc[nNpcIndex].m_CompAttrMgr[ncai_lifeuplimit];
			int nPercent	= nCurLife * 100 / nLifeLimit;
			int nPercentC	= ( nCurLife - Env.nEventValue ) * 100 / nLifeLimit;
			
			if( Param[0] > 0 && 
				( nPercentC > nPercent ) && 
				nPercent < Param[0] &&
				nPercentC >= Param[0] )
			{
				return TRUE;
			}
			
			int nValue = ( ~Param[0] ) + 1;
			
			if( Param[0] < 0 && 
				( nPercentC < nPercent ) &&
				nPercent > nValue &&
				nPercentC <= nValue )
			{
				return TRUE;
			}
			
		}
		break;
	case buff_event_type_mana:
		{
			if( Param[0] == BUFF_INVALID )
				return TRUE;
			
			int nNpcIndex = Env.nEventRecever;
			
			if( nNpcIndex < 0 || nNpcIndex > MAX_NPC )
				return FALSE;
			
			int nCurMana	= Npc[nNpcIndex].m_UnaryAttrMgr[nuai_curmana];
			int nManaLimit	= Npc[nNpcIndex].m_CompAttrMgr[ncai_manauplimit];
			int nPercent	= nCurMana * 100 / nManaLimit;
			int nPercentC	= ( nCurMana - Env.nEventValue ) * 100 / nManaLimit;
			
			if( Param[0] > 0 && 
				( nPercentC > nPercent ) && 
				nPercent < Param[0] &&
				nPercentC >= Param[0] )
			{
				return TRUE;
			}

			if( Param[0] == 0 && 
				( nPercentC < nPercent ) &&
				nPercent > Param[0] &&
				nPercentC <= Param[0] )
			{
				return TRUE;
			}
			
			if( Param[0] == 0 && 
				nPercentC == 0 &&
				nPercent == 0 &&
				Env.nEventValue >= nCurMana )
			{
				return TRUE;
			}

			int nValue = ( ~Param[0] ) + 1;
			
			if( Param[0] < 0 && 
				( nPercentC < nPercent ) &&
				nPercent > nValue &&
				nPercentC <= nValue )
			{
				return TRUE;
			}
			
		}
		break;
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_damagein:
	case buff_event_type_damageout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_buffin:
	case buff_event_type_buffout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
		break;
	default:
		break;	
	}
	
	return FALSE;
}

int buff_robber(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	if( Param[0] == BUFF_INVALID )
		return TRUE;
	
	switch( Env.nEventType )
	{
	case buff_event_type_blood:
	case buff_event_type_skillin:
	case buff_event_type_damagein:
		{
			if( Env.nEventSender < 0 || Env.nEventSender > MAX_NPC )
				return FALSE;

			if( Env.nBuffRecever < 0 || Env.nBuffRecever > MAX_NPC )
				return FALSE;

			if (Npc[Env.nEventSender].GetKind() == kind_creature) //前过滤宠物和雇佣兵
			{
					int nPlayerIdx = Npc[Env.nEventSender].GetSummonerIdx();

					if (!IsValidPlayer(nPlayerIdx))
						return FALSE;

					int nNpcIdx = Player[nPlayerIdx].m_nIndex;

					if (!IsValidNpc(nNpcIdx))
						return FALSE;

					Env.nEventSender = nNpcIdx;

			}
			//接受者为Player
			if( Npc[Env.nBuffRecever].IsPlayer( ) )
				return FALSE;

			const FSGUID& RGUID = Npc[Env.nBuffRecever].GetRobber( );

			if( Npc[Env.nEventSender].IsPlayer( ))
			{
				int nPlayerIndex = Npc[Env.nEventSender].GetPlayerIdx( );
				RelationSet& RS = Player[nPlayerIndex].GetRelationSet( );
				
				RelationRecord* pRec = RS.GetRelationByTemplate( enSUTplId_Tong );
				
				if( pRec )
				{
					SocialUnit* pLParent = pRec->pLeafUnit;
					
					while( pLParent )
					{
						const FSGUID& LGUID =  pLParent->GetUnitGuid( );
						
						if( LGUID != 0 && 
							RGUID != 0 &&
							RGUID == LGUID )
						{
							return TRUE;
						}
						
						pLParent = pLParent->GetParent( );
					}
				}
			}
			else
			{
				const FSGUID& LGUID = Npc[Env.nEventSender].GetLord( );

				if( LGUID != 0 && 
					RGUID != 0 &&
					RGUID == LGUID )
				{
					return TRUE;
				}
			}
		}
		break;
	case buff_event_type_skillout:
	case buff_event_type_damageout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_buffin:
	case buff_event_type_buffout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
		break;
	default:
		break;	
	}
	
	return FALSE;
}

int buff_lord(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	if( Param[0] == BUFF_INVALID )
		return TRUE;
	
	switch( Env.nEventType )
	{
	case buff_event_type_blood:
	case buff_event_type_skillin:
	case buff_event_type_damagein:
		{
			if( Env.nEventSender < 0 || Env.nEventSender > MAX_NPC )
				return FALSE;

			if( Env.nBuffRecever < 0 || Env.nBuffRecever > MAX_NPC )
				return FALSE;

			//接受者为Player
			if( Npc[Env.nBuffRecever].IsPlayer( ) )
				return FALSE;

			const FSGUID& RGUID = Npc[Env.nBuffRecever].GetLord( );

			if( Npc[Env.nEventSender].IsPlayer( ) )
			{
				int nPlayerIndex = Npc[Env.nEventSender].GetPlayerIdx( );
				RelationSet& RS = Player[nPlayerIndex].GetRelationSet( );
				
				RelationRecord* pRec = RS.GetRelationByTemplate( enSUTplId_Tong );
				
				if( pRec )
				{
					SocialUnit* pLParent = pRec->pLeafUnit;
					
					while( pLParent )
					{
						const FSGUID& LGUID =  pLParent->GetUnitGuid( );
						
						if( LGUID != 0 && 
							RGUID != 0 &&
							RGUID == LGUID )
						{
							return TRUE;
						}
						
						pLParent = pLParent->GetParent( );
					}
				}
			}
			else
			{
				const FSGUID& LGUID = Npc[Env.nEventSender].GetLord( );

				if( LGUID != 0 && 
					RGUID != 0 &&
					RGUID == LGUID )
				{
					return TRUE;
				}
			}
		}
		break;
	case buff_event_type_skillout:
	case buff_event_type_damageout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_buffin:
	case buff_event_type_buffout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
		break;
	default:
		break;	
	}
	
	return FALSE;
}

int buff_random(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	return ( g_Random( Param[0] ) < Param[1] );
}

//=====================================================================================

int buff_transid(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;

	switch( Env.nEventType )
	{
	case buff_event_type_skillin:
	case buff_event_type_skillout:
		{
			if( Env.nEventFormat & buff_event_format_skillid )
			{
				Env.nEvent = Param[0];
			}
		}
		break;
	case buff_event_type_buffin:
	case buff_event_type_buffout:
		{
			if( Env.nEventFormat & buff_event_format_buffid )
			{
				Env.nEvent = Param[0];
			}
		}
		break;
	case buff_event_type_damagein:
	case buff_event_type_damageout:
		{
			if( Env.nEventFormat & buff_event_format_damageid )
			{
				Env.nEvent = Param[0];
			}
		}
		break;
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
	case buff_event_type_blood:
		break;
	default:
		break;	
	}

	return FALSE;
}

int buff_transidbyos(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	switch( Env.nEventType )
	{
	case buff_event_type_skillin:
	case buff_event_type_skillout:
		{
			if( Env.nEventFormat & buff_event_format_skillid )
			{
				int nOffset = Env.nEvent > Param[0] ? Env.nEvent - Param[0] : 0 ;

				Env.nEvent = Param[1] + nOffset;
			}
		}
		break;
	case buff_event_type_buffin:
	case buff_event_type_buffout:
		{
			if( Env.nEventFormat & buff_event_format_buffid )
			{
				int nOffset = Env.nEvent > Param[0] ? Env.nEvent - Param[0] : 0 ;
				
				Env.nEvent = Param[1] + nOffset;
			}
		}
		break;
	case buff_event_type_damagein:
	case buff_event_type_damageout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
	case buff_event_type_blood:
		break;
	default:
		break;	
	}
	
	return FALSE;
}

int buff_transdamage(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	switch( Env.nEventType )
	{
	case buff_event_type_damagein:
	case buff_event_type_damageout:
		{
			if( Env.nEventFormat & buff_event_format_damageid )
			{
				if( Env.nEventValue != 0 )
					
				{
					if( Param[0] != -1 )
						Env.nEvent = Param[0];
					
					Env.nEventValue = Env.nEventValue * Param[1] / 100;
				}
				
				return TRUE;
			}
		}
		break;
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_buffin:
	case buff_event_type_buffout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
	case buff_event_type_blood:
		break;
	default:
		break;	
	}

	return FALSE;
}

int buff_transdamagebv(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{	
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	switch( Env.nEventType )
	{
	case buff_event_type_damagein:
	case buff_event_type_damageout:
		{
			if( Env.nEventFormat & buff_event_format_damageid )
			{
				if( Env.nEventValue != 0 )
				{
					int nCurLifeMax = Npc[Env.nEventSender].m_CompAttrMgr[ncai_lifeuplimit];
					int nCurLife = Npc[Env.nEventSender].m_UnaryAttrMgr[nuai_curlife];
					
					int BloodPercent = nCurLife * 100 / nCurLifeMax;
					Env.nEventValue = Env.nEventValue * ( BloodPercent * Param[0] / 100 + Param[1] ) / 100;
				}
				
				return TRUE;
			}
		}
		break;
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_buffin:
	case buff_event_type_buffout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
	case buff_event_type_blood:
		break;
	default:
		break;	
	}
	
	return FALSE;
}

int buff_killself(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	Env.nBuffCap	=	0;
	return TRUE;
}

int buff_reboundall(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	switch( Env.nEventType )
	{
	case buff_event_type_damagein:
	case buff_event_type_buffin:
		{
			Env.nEventRecever = Env.nEventSender;
		}
		break;
	case buff_event_type_damageout:
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_buffout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
	case buff_event_type_blood:
		break;
	default:
		break;	
	}	
	return FALSE;
}

int buff_rebounddamage_p(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	switch( Env.nEventType )
	{
	case buff_event_type_damagein:
		{
			if( Env.nEventValue != 0 )
			{
				Env.nEventRecever = Env.nEventSender;
				
				if( Param[0] != -1 )
					Env.nEvent = Param[0];
				
				Env.nEventValue = Env.nEventValue * Param[1] / 100;
			}
		}
		break;
	case buff_event_type_buffin:
	case buff_event_type_damageout:
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_buffout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
	case buff_event_type_blood:
		break;
	default:
		break;	
	}	
	return FALSE;
}

int buff_rebounddamage_v(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	switch( Env.nEventType )
	{
	case buff_event_type_damagein:
		{
			if( Env.nEventValue != 0 )
			{
				Env.nEventRecever = Env.nEventSender;
				
				if( Param[0] != -1 )
					Env.nEvent = Param[0];
				
				Env.nEventValue = Param[1];
			}
		}
		break;
	case buff_event_type_buffin:
	case buff_event_type_damageout:
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_buffout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
	case buff_event_type_blood:
		break;
	default:
		break;	
	}	
	return FALSE;
}

int buff_damagesorb(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	switch( Env.nEventType )
	{
	case buff_event_type_damagein:
		{
			if( Env.nEventValue != 0 &&
				Env.nEventRecever >= 0 &&
				Env.nEventRecever < MAX_NPC )
			{
				int nSorbValue = Env.nEventValue * Param[0] / 100;
				
				Env.nEventValue -= nSorbValue;
				
				if( Env.nEventValue < 0 )
					Env.nEventValue = 0;
				
				int nManaValue = nSorbValue * Param[1] / 100;
				
				int nCurMana = Npc[Env.nEventRecever].m_UnaryAttrMgr[nuai_curmana];
				
				nCurMana = nCurMana > nManaValue ? (nCurMana - nManaValue) : 0 ;
				
				Npc[Env.nEventRecever].m_UnaryAttrMgr.Set( nuai_curmana, nCurMana );
				
				if( nCurMana == 0 )
					Env.nBuffCap = 0;
				
				return TRUE;
			}
		}
		break;
	case buff_event_type_buffin:
	case buff_event_type_damageout:
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_buffout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
	case buff_event_type_blood:
		break;
	default:
		break;	
	}	
	return FALSE;
}

int buff_modifybuff_C(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
	{
		//outaction | effect_i op 

		BuffMgr& BMgr = BuffMgr::Singleton( );
		BMgr.ModifyBuffTByCate( Env.nBuffRecever, Param[1], Param[2], Param[0] );
	}
	else
	{
		//filter action
	}
	
	return FALSE;
}

int buff_modifybuff(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
	{
		//outaction | effect_i op
		BuffMgr& BMgr = BuffMgr::Singleton( );
		BMgr.ModifyBuffTByGroup( Env.nBuffRecever, Param[1], Param[0] );
	}
	else
	{
		//filter action
		if( Env.nEventFormat == buff_event_format_buffobj )
		{
			Buff *pTargetBuff = (Buff*)Env.nEvent;
			
			unsigned long ulPersist = pTargetBuff->GetCurPersist();
			ulPersist = ulPersist * Param[0] / 100;
			
			pTargetBuff->SetPersist(ulPersist);
			
			return TRUE;
		}
	}

	return FALSE;
}

int buff_setbufftime_G(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
	{
		//outaction | effect_i op
		BuffMgr& BMgr = BuffMgr::Singleton( );
		BMgr.SetBuffPersistByGroup( Env.nBuffRecever, Param[1], Param[0] );
	}
	
	return FALSE;
}

int buff_setbufftime_C(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
	{
		//outaction | effect_i op
		BuffMgr& BMgr = BuffMgr::Singleton( );
		BMgr.SetBuffPersistByCate( Env.nBuffRecever, Param[1], Param[2], Param[0] );
	}
	
	return FALSE;
}

int buff_addbufftime_G(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
	{
		//outaction | effect_i op
		BuffMgr& BMgr = BuffMgr::Singleton( );
		BMgr.AddBuffPersistByGroup( Env.nBuffRecever, Param[1], Param[0] );
	}
	
	return FALSE;
}

int buff_addbufftime_C(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
	{
		//outaction | effect_i op
		BuffMgr& BMgr = BuffMgr::Singleton( );
		BMgr.AddBuffPersistByCate( Env.nBuffRecever, Param[1], Param[2], Param[0] );
	}
	
	return FALSE;
}

int buff_addbufftimelimit_G(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
	{
		//outaction | effect_i op
		BuffMgr& BMgr = BuffMgr::Singleton( );
		BMgr.AddBuffPersistByGroupLimit( Env.nBuffRecever, Param[1], Param[0] );
	}
	
	return FALSE;
}

int buff_addbufftimelimit_C(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
	{
		//outaction | effect_i op
		BuffMgr& BMgr = BuffMgr::Singleton( );
		BMgr.AddBuffPersistByCateLimit( Env.nBuffRecever, Param[1], Param[2], Param[0] );
	}
	
	return FALSE;
}

int buff_modifybuffcap(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType != buff_event_type_none )
	{
		Env.nBuffCap += Param[0];
		return TRUE;
	}
	
	return FALSE;
}

int buff_transblood(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	switch( Env.nEventType )
	{
	case buff_event_type_blood:
		{
			if( Env.nEventValue != 0 )
			{
				Env.nEventValue = Env.nEventValue * Param[0] / 100;
			}
		}
		break;
	case buff_event_type_damagein:
	case buff_event_type_buffin:
	case buff_event_type_damageout:
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_buffout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
		break;
	default:
		break;	
	}	
	return FALSE;
}

int buff_setaiparam(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		NpcController& controller = Npc[Env.nBuffRecever].GetController();
		
		int paramId = Param[0];
		int paramValue = Param[1];
		switch(paramId)
		{
		case 0://Active
			controller.SetActive((TRUE == paramValue) ? true : false);
			break;		
		case 1://EnableChase
			controller.SetEnableChase((TRUE == paramValue) ? true : false);
			break;
		case 2://EnableReturn
			controller.SetEnableReturn((TRUE == paramValue) ? true : false);
			break;
		case 3://Aggressive
			controller.SetAggressive((TRUE == paramValue) ? true : false);
			break;		
		case 4://Wander
			controller.SetWander((TRUE == paramValue) ? true : false);
			break;
		case 5://StayAround
			controller.SetStayAround((TRUE == paramValue) ? true : false);
			break;
		case 6://WanderRestPrecentage
			controller.SetWanderRestPercentage(paramValue);
			break;
		default:
			//使用了没有意义的参数编号
			_ASSERT(false);
			break;
		}

		return TRUE;
	}

	return FALSE;
}

int buff_setaimode(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		NpcController& controller = Npc[Env.nBuffRecever].GetController();

		enum enumPredefinedAIMode//预定义AI模式（预定义的AI参数组合）
		{
			predefined_ai_mode_guard = 0,	//守卫
			predefined_ai_mode_dialoger,	//对话npc
			predefined_ai_mode_fresh_man,	//新手村怪
			predefined_ai_mode_normal_1,	//一般怪物
			predefined_ai_mode_normal_2,	//一般怪物
			predefined_ai_mode_passive,		//被动怪物
			predefined_ai_mode_terrified,	//恐惧状态
			predefined_ai_mode_frenzy,		//狂暴状态
			predefined_ai_mode_player,		//正常玩家
			
			predefined_ai_mode_count
		};

		static int predefinedAIMode[predefined_ai_mode_count][7] = {
			{ 1, 1, 1, 1, 0, 1, 50 },	//守卫
			{ 0, 0, 0, 0, 0, 0, 0 },	//对话npc
			{ 1, 1, 1, 0, 1, 1, 50 },	//新手村怪
			{ 1, 1, 0, 1, 1, 0, 50 },	//一般怪物
			{ 1, 1, 0, 1, 0, 0, 50 },	//一般怪物
			{ 1, 1, 0, 0, 0, 0, 50 },	//被动怪物
			{ 1, 0, 0, 2, 1, 0, 0 },	//恐惧状态
			{ 1, 1, 0, 1, 1, 0, 0 },	//狂暴状态
			{ 0, 0, 0, 0, 0, 0, 0 },	//正常玩家
		};
		
		int aiMode = Param[0];

		controller.SetActive((TRUE == predefinedAIMode[aiMode][0]) ? true : false);
		controller.SetEnableChase((TRUE == predefinedAIMode[aiMode][1]) ? true : false);
		controller.SetEnableReturn((TRUE == predefinedAIMode[aiMode][2]) ? true : false);
		controller.SetMode((enumAIMode)predefinedAIMode[aiMode][3]);
		controller.SetWander((TRUE == predefinedAIMode[aiMode][4]) ? true : false);
		controller.SetStayAround((TRUE == predefinedAIMode[aiMode][5]) ? true : false);
		controller.SetWanderRestPercentage(predefinedAIMode[aiMode][6]);

		return TRUE;
	}

	return FALSE;
}

int buff_block_clientcontrol(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
	{	
		int playerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if (playerIndex > 0)
		{			
			Player[playerIndex].SetBlockClientControl((TRUE == Param[0]) ? true : false);
			Player[playerIndex].SyncAttribute(attr_IsBlockClientControl);

			return TRUE;
		}
	}

	return FALSE;
}

int buff_new_item_owner(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if (Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC)
	{
		KNpc& receiver = Npc[Env.nBuffRecever];		
		if (receiver.GetKind() == kind_normal || receiver.GetKind() == kind_building)
		{
			bool setNoOwner = (Param[0]==-1 ? true : false);
			if (setNoOwner)
			{
				receiver.m_UnaryAttrMgr.Set(nuai_owner, 0);
				receiver.m_UnaryAttrMgr.Set(nuai_owner_team, -1);
			}
			else
			{
				if(Env.nBuffSender > 0 && Env.nBuffSender < MAX_NPC)
				{
					KNpc& npc = Npc[Env.nBuffSender];
					int ownerPlayerIndex = 0;					
					switch(npc.GetKind())
					{
					case kind_player:
						ownerPlayerIndex = npc.GetPlayerIdx();
						break;
					case kind_creature:
						ownerPlayerIndex = npc.GetSummonerIdx();
						break;
					case kind_employee:
						ownerPlayerIndex = npc.GetEmployerIdx();
						break;
					}

					if (IsValidPlayer(ownerPlayerIndex))
					{
						if (Player[ownerPlayerIndex].GetTeamInfo().IsInTeam())
						{
							int ownerTeamIndex = Player[ownerPlayerIndex].GetTeamInfo().GetTeamId();
							receiver.m_UnaryAttrMgr.Set(nuai_owner_team, ownerTeamIndex);
						}
						else
						{
							receiver.m_UnaryAttrMgr.Set(nuai_owner_team, -1);
						}

						receiver.m_UnaryAttrMgr.Set(nuai_owner, ownerPlayerIndex);

						return TRUE;
					}
				}
			}
		}
	}
	
	return FALSE;
}

int buff_can_be_item_owner(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if (Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC)
	{		
		KNpc& receiver = Npc[Env.nBuffRecever];
		if (receiver.GetKind() == kind_normal || receiver.GetKind() == kind_building)
		{
			if(Env.nEventSender > 0 && Env.nEventSender < MAX_NPC)
			{	
				KNpc& eventSender = Npc[Env.nEventSender];
				int checkPlayerIndex = 0;
				switch(eventSender.GetKind())
				{
				case kind_player:
					checkPlayerIndex = eventSender.GetPlayerIdx();
					break;
				case kind_creature:
					checkPlayerIndex = eventSender.GetSummonerIdx();
					break;
				case kind_employee:
					checkPlayerIndex = eventSender.GetEmployerIdx();
					break;
				}
				
				if (IsValidPlayer(checkPlayerIndex))
				{
					const int currentOwner = receiver.m_UnaryAttrMgr[nuai_owner];
					if (currentOwner == 0)
					{
						return TRUE;
					}
					else if (currentOwner == checkPlayerIndex)
					{
						return TRUE;
					}
					else if (Player[checkPlayerIndex].GetTeamInfo().IsInTeam())
					{
						if (Player[checkPlayerIndex].GetTeamInfo().GetTeamId() == receiver.m_UnaryAttrMgr[nuai_owner_team])
							return TRUE;
					}										
				}
			}
		}
	}

	return FALSE;
}

int buff_setpkmode(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
	{	
		Npc[Env.nBuffRecever].m_UnaryAttrMgr.Set(nuai_pkmode, Param[0]);

		if( Npc[Env.nBuffRecever].IsPlayer( ) )
			Npc[Env.nBuffRecever].SyncAttr(npc_attr_unary, nuai_pkmode, 0);
	}

	return TRUE;
}

int buff_newworld(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
		return Npc[Env.nBuffRecever].ChangeWorld( Param[0], Param[1] * 32 , Param[2] * 32 );

	return FALSE;
}

int buff_revworld(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Npc[Env.nBuffRecever].IsPlayer( ) )
	{
		int nPlayerIdx = Npc[Env.nBuffRecever].GetPlayerIdx( );
		if (Player[nPlayerIdx].TransferToRevivePos())
			return TRUE;
	}
	
	return FALSE;
}

int buff_isprof(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
	{
		if( Npc[Env.nBuffRecever].IsPlayer( ) )
		{
			return Npc[Env.nBuffRecever].GetSeries( ) == Param[0] &&
				Player[Npc[Env.nBuffRecever].GetPlayerIdx()].GetSkillSeries() == Param[1];
		}
	}

	return FALSE;
}
					 
int buff_issex(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
	{
		if( Npc[Env.nBuffRecever].IsPlayer( ) )
		{
			return Npc[Env.nBuffRecever].GetSex( ) == Param[0];
		}
	}
	
	return FALSE;
}

int buff_isplayer(
				  BUFF_ENV_PARAM& Env,
				  BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
	{
		return Npc[Env.nBuffRecever].IsPlayer( );
	}
	
	return FALSE;
}

int buff_isdeath(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
	{
		return Npc[Env.nBuffRecever].IsDeath( );
	}
	
	return FALSE;
}

int buff_islevel(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
	{
		return ( Npc[Env.nBuffRecever].GetLevel( ) == Param[0] );
	}
	
	return FALSE;
}


int buff_setlocal(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Param[0] >=0 && Param[0] < BUFF_MAX_PARAM )
		Env.LocalVar[Param[0]] = Param[1];

	return TRUE;
}

int buff_getlocal(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Param[0] >=0 && Param[0] < BUFF_MAX_PARAM )
		return Env.LocalVar[Param[0]];
	
	return FALSE;
}

//给地图领主添加资源
int buff_addres(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		int nLord = SubWorld[nWorldIndex].GetLord( );
		
		if( nLord != INVALID_VALUE )
		{
			if ( Param[0] == 0 )
				Param[1] = Param[1] / 10000;
			
			Npc[nLord].AddCityRes(Param[0], Param[1]);
		}
	}

	return FALSE;
}

int buff_decres(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		int nLord = SubWorld[nWorldIndex].GetLord( );
		
		if( nLord != INVALID_VALUE )
		{
			int nDecVal = Param[1];
			nDecVal = -nDecVal;

			if ( Param[0] == 0 )
				nDecVal = nDecVal / 10000;

			Npc[nLord].AddCityRes(Param[0], nDecVal);
		}
	}
	
	return FALSE;
}

//掠夺lord资源到robber
int buff_robres(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{	
	if( Env.nBuffRecever < 0 || Env.nBuffRecever > MAX_NPC )
		return FALSE;
	
	//接受者为Player
	if( Npc[Env.nBuffRecever].IsPlayer( ) )
		return FALSE;

	int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
	
	int nLordIndex = SubWorld[nWorldIndex].GetLord( );
	
	if( nLordIndex != Env.nBuffRecever )
		return FALSE;
	
	const FSGUID& RGUID = Npc[Env.nBuffRecever].GetRobber( );
	
	ServerSocialUnitMgr& SMgr = ServerSocialUnitMgr::Singleton( );

	SocialUnit* pUnit = SMgr.GetUnit( RGUID );

	if( pUnit )
	{
		SocialUnitAttr& Attr = pUnit->GetUnitAttr(  );
		if( Attr.IsAttrHasData( enSUAttr_CityMap ) )
		{
			char* pData = NULL;
			int nSize  = Attr.GetAttr( enSUAttr_CityMap, pData );
			if (nSize == sizeof (int))
			{
				int nMapID = *((int*)pData);
				int nRobWorldIndex = g_SubWorldSet.SearchWorld( nMapID );
				if( nRobWorldIndex != -1 )
				{
					int nRobWorldLordIndex = SubWorld[nRobWorldIndex].GetLord( );
					
					if( nRobWorldLordIndex != -1 )
					{
						int nType = Param[0] + nuai_lord_res0;
						int nValue = Npc[nLordIndex].m_UnaryAttrMgr[nType];
						int nRobValue = nValue * Param[1] / 100;
						
						if(nRobValue < 0)
							nRobValue = 0;
						
						if(nRobValue > nValue)
							nRobValue = nValue;
						
						Npc[nLordIndex].m_UnaryAttrMgr.Set( nType, nValue - nRobValue );
						Npc[nLordIndex].SetDataChangedFlag(true);
						
						nValue = Npc[nRobWorldLordIndex].m_UnaryAttrMgr[nType];
						Npc[nRobWorldLordIndex].m_UnaryAttrMgr.Set( nType, nValue + nRobValue );
						Npc[nRobWorldLordIndex].SetDataChangedFlag(true);
					}
				}
			}
		}
	}

	return FALSE;
}

int buff_additem(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Npc[Env.nBuffRecever].IsPlayer() )
	{

		if ( Param[4] <= 0 )
			return FALSE;
		
		int nIndex = 
		ItemSet.Add(
			Param[0], 
			Param[1], 
			Param[2], 
			Param[3], 
			Param[4] );

		if (nIndex <= 0)
		{
			return FALSE;
		}

		KItem& item = Item[nIndex];		
		int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx( );
		if( nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER )
		{
			KPlayer& player = Player[nPlayerIndex];
			int x, y;
			if (player.GetItemList().CheckCanPlaceInEquipment(&x, &y))
			{
				if (player.GetItemList().Add(nIndex, pos_equiproom, x, y, NULL, item_sync_type_gain))
				{
					//统计：系统增加物品（BUFF）
					ItemTemplateId templateId;
					item.GetItemTemplateId(templateId);
					player.GetPlayerStatistic().AddItem(templateId, item.GetItemCount(), item_count_type_system_add);

					//日志：系统增加物品（BUFF）
					bool needLog = (item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
					if (needLog)
					{
						LogEventParam systemAddItemEvent;
						systemAddItemEvent.event = log_event_system_add_item;
						systemAddItemEvent.param1 = player.GetGUID();
						systemAddItemEvent.param2 = item.GetGUID();
						systemAddItemEvent.param4 = item.GetItemCount();

						char itemTemplateId[32] = { 0 };
						item.GetItemTemplateId(itemTemplateId, sizeof(itemTemplateId));
						itemTemplateId[sizeof(itemTemplateId) - 1] = 0;
						snprintf(systemAddItemEvent.param3.data, sizeof(systemAddItemEvent.param3.data), "buff %s(%s)", item.GetName(), itemTemplateId);
						systemAddItemEvent.param3.data[sizeof(systemAddItemEvent.param3.data) - 1] = 0;

						g_pLogSystem->Log(systemAddItemEvent);
					}

					return TRUE;
				}
			}
		}
		
		ItemSet.Remove( nIndex );
	}

	return FALSE;
}

int buff_delitem(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Npc[Env.nBuffRecever].IsPlayer( ) )
	{
		int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx( );

		if ( nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER )
		{
			int nRet = 
			Player[nPlayerIndex].m_ItemList.DelStatckItem(
				Param[0], 
				Param[1], 
				Param[2], 
				Param[3],
				Param[4]);
			
			return nRet;
		}
	}

	return FALSE;
}

int buff_useitem(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Npc[Env.nBuffRecever].IsPlayer( ) )
	{
		int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx( );
		
		if ( nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER )
		{
			int nIdx =
			Player[nPlayerIndex].m_ItemList.FindNormalItem(
				Param[0], 
				Param[1], 
				Param[2], 
				Param[3] );

			if( nIdx != -1 )
			{
				Player[nPlayerIndex].m_ItemList.EatMecidine( nIdx, 0 );
			}
		}
	}
	return FALSE;
}

int buff_moditem(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Npc[Env.nBuffRecever].IsPlayer( ) )
	{
		int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx( );
		
		if ( nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER )
		{
			KPlayer& player = Player[nPlayerIndex];
			int nItemIdx = Env.LocalVar[0];
			if( nItemIdx >= 0 && nItemIdx < MAX_ITEM )
			{
				KItem& item = Item[nItemIdx];
				if( item.GetBelong( ) == nPlayerIndex )
				{
					switch( Param[0] ) 
					{
					case 0:	//modify action time
						item.SetActionTime( Param[1] );
						break;
					case 1: //modify bond state
						item.SetBind(Param[1] ? false : true);
						break;
					case 2: //修改锁定状态（例如：骑马时需要锁定马匹）
						{
							if (Param[1] == 1)
								item.Lock();
							else if (Param[1] == -1)
								item.Unlock();

							item.SyncAttribute(item_attr_islocked, player.GetNetConnectIdx());
						}
						break;
					case 3:
						{
							if (Param[1] > 0)
							{
								if ( Param[2] > 0 && !item.IsBind() )
								{
									item.ItemErrCodeToClient( nPlayerIndex, item_inlay_error_targetitem_rule );
									return FALSE;
								}
								DWORD dwLockTime = Param[1] * 86400;
								bool bOldState = item.IsLockedByDate(-1);
								DWORD dwOldDate = item.GetLockDate();
								if ( bOldState )
								{
									item.UnlockByDate( dwLockTime );
								}
								else
								{
									item.LockByDate( );
								}
								
								if ( dwOldDate != item.GetLockDate() )
								{
									item.SyncAttribute(item_attr_lockdate, player.GetNetConnectIdx());
								}//endif
								
							}//endif
						}
					case 4:
						{
							int nSocketCount = Param[1];
							if ( nSocketCount <= 0 || nSocketCount >  MAX_INLAY_COUNT )
							{
								return FALSE;
							}//endif 

							int nYao		= Param[2];
							if ( nYao != yao_yin && nYao != yao_yang )
							{
								return FALSE;
							}	
							
							if ( item.GetMaxSocketCount() == nSocketCount )
							{
								item.SetYaoID( nYao );
								item.SyncAttribute(item_attr_yao_id, player.GetNetConnectIdx());
								if ( nYao == yao_yang )
								{
									item.ItemErrCodeToClient( nPlayerIndex, item_inlay_ok_yang );
								}
								else
								{
									item.ItemErrCodeToClient( nPlayerIndex, item_inlay_ok_yin );
								}
							}
							else
							{
								item.ItemErrCodeToClient( nPlayerIndex, item_inlay_error_targetitem_rule );
							}
						}
						break;
					case 5:
						{
							int nLimitItemLevel = Param[1];
							int nRepairPresent = Param[2];
							
							if ( item.GetLevelRequirement() <= nLimitItemLevel && nLimitItemLevel > 0 &&
								nRepairPresent > 0 && nRepairPresent <= 100)
							{
								player.repairItemByItem(nItemIdx, nRepairPresent);
							}
							else
							{
								item.ItemErrCodeToClient( nPlayerIndex, item_inlay_error_repair_by_item );
							}
						}
						break;

					case 6: //国战分期付款
						{
							if (item.GetGenre() == item_ib || item.GetIBGuid() != 0) //不可是IB物品
								return FALSE;

							if (item.GetIBBuyData() == 0 ) //不可是已经买断的
								return FALSE;

							if ( !item.GetItemTemplate() || item.GetItemTemplate()->nMaxFlushTimes <= 0) //必须是国战分期付款物品
								return FALSE;

							if (item.GetIBItemType() > ib_item_now)
							{
								if ( item.GetFlushTimes() < item.GetItemTemplate()->nMaxFlushTimes - 1)
								{
									item.SetIBBuyDate(UNIX_TMIE_STAMP); // Refresh the overdate item
								}//endif
								else
								{
									item.SetIBBuyDate(0);               //Use forever
								}//endelse

								item.SetFlushTimes(item.GetFlushTimes() + 1);
								
								_ASSERT(item.GetFlushTimes() <= 255); // Notice One Byte only for DBSave


								item.SyncAttribute(item_attr_buytime,player.GetNetConnectIdx());
								item.SyncAttribute(item_attr_flush_times,player.GetNetConnectIdx());

								if (item.GetGenre()==item_equip)
									player.GetItemList().OnEquipChanged();

							}//endif

						}
						break;

					default:
						break;
					}
				}
			}
		}
	}
	return FALSE;
}

int buff_addskill(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		int nStatus = Param[2] ? skill_status_usable : skill_status_inactive;

		NpcSkillList& SL = Npc[Env.nBuffRecever].GetSkillList( );

		SL.AddSkillEx( Param[0], Param[1], nStatus );
		SL.NotifyAddSkill( Param[0], Param[1], nStatus );

		return TRUE;
	}
	return FALSE;
}

int buff_delskill(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		NpcSkillList& SL = Npc[Env.nBuffRecever].GetSkillList( );

		SL.RemoveSkillEx( Param[0] );
		SL.NotifyRemoveSkill( Param[0] );
		
		return TRUE;
	}
	return FALSE;
}

int buff_modskill(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		NpcSkillList& SL = Npc[Env.nBuffRecever].GetSkillList( );

		NpcSkillList::Iterator iter;
		int		nSkillIdx;
		
		while( (nSkillIdx = SL.NextSkillIdx(iter)) != INVALID_SKILL_INDEX )
		{
			int nSkillID = SL.GetIdByIdx( nSkillIdx );
			int nLevel = SL.GetLevelByIdx( nSkillIdx );

			KSkill* pSkill = g_SkillManager.GetSkill( nSkillID, nLevel );
			
			if( pSkill &&
				pSkill->GetGroup( ) == Param[0] )
			{
				int nValue = 0;

				switch( Param[1] ) 
				{
				case 0:
					{
						nValue = SL.GetExplodeProbByIdx( nSkillIdx );

						nValue += Param[2] * Env.nBuffPileCount;

						SL.ChangeExplodeProbByIdx( nSkillIdx, nValue );
					}	
					break;
				case 1:
					{
						nValue = SL.GetPrivateCostByIdx( nSkillIdx );

						nValue += Param[2] * Env.nBuffPileCount;

						SL.ChangeCostByIdx( nSkillIdx, nValue );
					}
					break;
				case 2:
					{
						SkillDamageInfo* pDI = SL.GetPrivateDamInfoByIdx( nSkillIdx );
						SkillDamageInfo* pComDam = pSkill->GetCommonDamageInfo();
						
						int nDamageIndex = Param[2];
						if( nDamageIndex > dot_none && 
							nDamageIndex < dot_end &&
							pDI )
						{
							// 伤害计算时会将技能原始的伤害(即pComDam)加上技能私有的伤害(即pDI)
							pDI[nDamageIndex].nVal += pComDam[nDamageIndex].nVal * ( Param[3] * Env.nBuffPileCount ) / 100;
							pDI[nDamageIndex].nNpcDamagePercent += Param[4] * Env.nBuffPileCount ;
						}
					}
					break;
				case 3:
					{
						nValue = SL.GetMaxAttackTargetByIdx( nSkillIdx );
						
						nValue += Param[2] * Env.nBuffPileCount;
						
						SL.ChgMaxAttackTargetByIdx( nSkillIdx, nValue );
					}
					break;
				case 4://编辑技能仇恨					
					{
						if (Param[3] == 0)//编辑仇恨值
						{
							SL.SetThreatValueByIdx(nSkillIdx, SL.GetThreatValueByIdx(nSkillIdx) + ( Param[2] * Env.nBuffPileCount ) );
						}
						else//编辑仇恨百分比
						{
							SL.SetThreatPercentageByIdx(nSkillIdx, SL.GetThreatPercentageByIdx(nSkillIdx) + ( Param[2] * Env.nBuffPileCount ) );
						}
					}
					break;

				case 5:
					{
						nValue = SL.GetExplodeDamageByIdx(nSkillIdx);
						nValue += Param[2] * Env.nBuffPileCount;
						SL.ChgExplodeDamageByIdx(nSkillIdx, nValue);
					}
					break;

				case 6:
					{
						nValue = SL.GetCastSpeedEnhanceByIdx(nSkillIdx);
						nValue += Param[2] * Env.nBuffPileCount;
						SL.ChgCastSpeedEnhanceByIdx(nSkillIdx, nValue);
					}
					break;

				default:
					break;
				}
			}
		}

		return TRUE;
	}
	return FALSE;
}


int buff_ridehorse(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Npc[Env.nBuffRecever].IsPlayer( ) )
	{
		Npc[Env.nBuffRecever].m_bRideHorse = Param[0];
		Npc[Env.nBuffRecever].m_HorseType = 0;

		if( Param[0] )
		{
			int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx( );
			
			if ( nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER )
			{
				int nItemIdx = Env.LocalVar[0];
				if( nItemIdx > 0 && nItemIdx < MAX_ITEM )
				{
					Npc[Env.nBuffRecever].m_HorseType = Item[nItemIdx].GetRes( );
					return TRUE;
				}
			}
		}
	}
	return FALSE; 
}

int buff_bloodvat(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Npc[Env.nBuffRecever].IsPlayer( ) &&
		!Npc[Env.nBuffRecever].IsDeath( ) &&
		Param[1] > 0 )
	{
		bool bUse = Param[0] > 0;
		int nAddBlood = Param[1];
		int nSkillID = Param[2];
		int nItemIdx = Env.LocalVar[0];
		int nPlayerIdx = Npc[Env.nBuffRecever].GetPlayerIdx();
		int nLifePercentage = Npc[Env.nBuffRecever].GetCurrentLifePercentage();
		if( nItemIdx >= 0 && nItemIdx < MAX_ITEM && IsValidPlayer(nPlayerIdx) && nLifePercentage < 70 )
		{
			int nCurDur = Item[nItemIdx].GetDurability();
			if ( nCurDur > 0 && bUse )
			{
				MagicData	MD;
				if ( nCurDur < nAddBlood )
				{
					nAddBlood = nCurDur;
				}
				MD.nMagicNo = add_curlife_v;
				MD.nVal		= nAddBlood;

				Npc[Env.nBuffRecever].SyncDamageInfo( Env.nBuffRecever, nAddBlood, COMBAT_INFO_HEAL_LIFE, nSkillID );
				g_MagicAttrModifier.ModifyMagicAttr( Env.nBuffRecever, &MD );
				Npc[Env.nBuffRecever].SyncAttr( npc_attr_unary, nuai_curlife, 0 );
				Item[nItemIdx].SetDurability( nCurDur - nAddBlood );				
				Item[nItemIdx].SyncAttribute( item_attr_durability, Player[nPlayerIdx].GetNetConnectIdx() );
				
				return TRUE;
			}
			
		}
	}
	return FALSE;
}

int buff_magicvat(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Npc[Env.nBuffRecever].IsPlayer( ) &&
		!Npc[Env.nBuffRecever].IsDeath( ) &&
		Param[1] > 0)
	{
		bool bUse = Param[0] > 0;
		int nAddMana = Param[1];
		int nSkillID = Param[2];
		int nItemIdx = Env.LocalVar[0];
		int nPlayerIdx = Npc[Env.nBuffRecever].GetPlayerIdx();
		int nManaPercentage = Npc[Env.nBuffRecever].GetCurrentManaPercentage();
		if( nItemIdx >= 0 && nItemIdx < MAX_ITEM && IsValidPlayer(nPlayerIdx) && nManaPercentage < 70 )
		{
			int nCurDur = Item[nItemIdx].GetDurability();
			if ( nCurDur > 0 && bUse )
			{
				MagicData	MD;
				if ( nCurDur < nAddMana )
				{
					nAddMana = nCurDur;
				}
				MD.nMagicNo = add_curmana_v;
				MD.nVal		= nAddMana;

				Npc[Env.nBuffRecever].SyncDamageInfo( Env.nBuffRecever, nAddMana, COMBAT_INFO_HEAL_MANA, nSkillID );
				g_MagicAttrModifier.ModifyMagicAttr( Env.nBuffRecever, &MD );
				Npc[Env.nBuffRecever].SyncAttr( npc_attr_unary, nuai_curmana, 0 );
				Item[nItemIdx].SetDurability( nCurDur - nAddMana );
				Item[nItemIdx].SyncAttribute( item_attr_durability, Player[nPlayerIdx].GetNetConnectIdx() );

				return TRUE;
			}
		}
	}
	return FALSE;
}

int buff_itemexp(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( IsValidNpc(Env.nBuffRecever) && IsValidPlayer(Npc[Env.nBuffRecever].GetPlayerIdx()))
	{
		bool isTurnOn = Param[0] > 0;
		int playerExpGainPercent = Param[1];
		int itemExpGainPercent = Param[2];
		int itemIndex = Env.LocalVar[0];
		KPlayer& player = Player[Npc[Env.nBuffRecever].GetPlayerIdx()];
		
		if(playerExpGainPercent >= 0 && itemExpGainPercent >= 0
			&& itemIndex > 0 && itemIndex < MAX_ITEM && Item[itemIndex].IsExpItem())
		{
			int currentItemIndex = 0;
			DWORD currentPlayerExpGainPercent = 0;
			DWORD currentItemExpGainPercent = 0;
			player.GetItemExpState(currentItemIndex, currentPlayerExpGainPercent, currentItemExpGainPercent);
			if (isTurnOn && currentItemIndex == 0)//开启
			{
				player.SetItemExpState(itemIndex, playerExpGainPercent, itemExpGainPercent);

				return TRUE;
			}
			else if (!isTurnOn && currentItemIndex > 0)//关闭
			{
				player.SetItemExpState(0, 0, 0);

				return TRUE;
			}
			else//错误
			{
				return FALSE;
			}
		}
	}

	return FALSE;
}

int buff_pushone(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender > 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		int nX1, nY1, nX2, nY2, nDir, nWantX, nWantY;
		
		Npc[Env.nBuffSender].GetMpsPos(&nX1, &nY1);
		Npc[Env.nBuffRecever].GetMpsPos(&nX2, &nY2);
		nDir = g_GetDirIndex(nX1, nY1, nX2, nY2);
		
		//往后退一步
		nWantX = nX2 + ((Param[0] * g_DirCos(nDir, 64)) >> 10);
		nWantY = nY2 + ((Param[0] * g_DirSin(nDir, 64)) >> 10);
		
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		if( SubWorld[nWorldIndex].TestBarrier( nWantX, nWantY ) )
			return FALSE;
		
		Npc[Env.nBuffRecever].SetPos( nWantX, nWantY );
	}

	return FALSE;
}

int buff_pullone(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && 
		Env.nBuffRecever < MAX_NPC &&
		Env.nBuffSender > 0 &&
		Env.nBuffSender < MAX_NPC )
	{
		int nX,nY;
		Npc[Env.nBuffSender].GetMpsPos( &nX, &nY );
		
		Npc[Env.nBuffRecever].SetPos( nX, nY );
		
		return TRUE;
	}

	return FALSE;
}

int buff_setpos(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
	{
		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );
		
		if( SubWorld[nWorldIndex].TestBarrier( Param[0], Param[1] ) )
			return FALSE;

		Npc[Env.nBuffRecever].SetPos( Param[0], Param[1] );
		return TRUE;
	}

	return FALSE;
}

int buff_offset(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nBuffRecever > 0 && Env.nBuffRecever < MAX_NPC )
	{
		int nX,nY;
		Npc[Env.nBuffRecever].GetMpsPos( &nX, &nY );

		int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );

		if( SubWorld[nWorldIndex].TestBarrier( Param[0]+ nX, Param[1] + nY ) )
			return FALSE;

		Npc[Env.nBuffRecever].SetPos( Param[0] + nX, Param[1] + nY );

		return TRUE;
	}

	return FALSE;
}

int buff_showmsg(
 	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever))
	{		
		int playerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if (IsValidPlayer(playerIndex))
		{
			KPlayer& player = Player[playerIndex];
			if (player.ShowPredefinedMsg(Param[0]))
				return TRUE;
		}
	}

	return FALSE;
}

int buff_follow(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever) && IsValidNpc(Env.nBuffSender))
	{
		Npc[Env.nBuffRecever].GetController().SetFollowNpc(Param[0] == 1 ? Env.nBuffSender : 0, Param[1]);

		return TRUE;
	}

	return FALSE;
}

int buff_setcreaturemark(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param)
{
	if( !IsValidNpc(Env.nBuffRecever) || !Npc[Env.nBuffRecever].IsPlayer() )
		return FALSE;

	if( !IsValidNpc(Env.nBuffSender) || !Npc[Env.nBuffSender].IsPlayer() )
		return FALSE;

// 	if(Env.nBuffSender == Env.nBuffRecever)
// 		return FALSE;

	int nSendPlayerIdx = Npc[Env.nBuffSender].GetPlayerIdx();

	if( Player[nSendPlayerIdx].m_Creature.IsALive() )
	{
		// 先退回上一个标记的宝宝
		int nRecvPlayerIdx = Npc[Env.nBuffRecever].GetPlayerIdx();
		Player[nRecvPlayerIdx].ReturnMarkCreature();

		return Player[nSendPlayerIdx].m_Creature.MarkToPlayer(Env.nBuffRecever);
		
	}

	return FALSE;	
}

int buff_setcreatureskillai(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if( IsValidNpc(Env.nBuffSender) )
	{
		if( Npc[Env.nBuffSender].IsPlayer() )
		{
			KCreature &creature = Player[Npc[Env.nBuffSender].GetPlayerIdx()].m_Creature;

			if( creature.IsALive() )
			{
				bool bFlag = Param[0] > 0 ? true : false;
				creature.SetCanUseSkillFlag(bFlag);
				return TRUE;
			}
		}
	}

	return FALSE;
}

int buff_ismybaby(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if( IsValidNpc(Env.nBuffSender) && IsValidNpc(Env.nBuffRecever) )
	{
		if( Npc[Env.nBuffSender].IsPlayer() && kind_creature == Npc[Env.nBuffRecever].m_Kind )
		{
			return Npc[Env.nBuffRecever].GetSummonerIdx() == Env.nBuffSender;
		}
	}

	return false;
}

int buff_add_exp(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever))
	{
		int playerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if(IsValidPlayer(playerIndex))
		{
			int addExp = Param[0];
			if (addExp > 0 && addExp <= MAX_ADD_EXP)
			{
				if (addExp >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_exp_amount) && g_pLogSystem)
				{
					LogEventParam addExpEvent;
					addExpEvent.event = log_event_buff_add_exp;
					addExpEvent.param1 = Player[playerIndex].GetGUID();
					addExpEvent.param4 = addExp;
					g_pLogSystem->Log(addExpEvent);
				}
				
				Player[playerIndex].QuestAddExp(addExp);
				return TRUE;
			}
		}
	}

	return FALSE;
}

int buff_modify_exp_percentage(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever))
	{
		int playerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if(IsValidPlayer(playerIndex))
		{
			KPlayer& player = Player[playerIndex];
			int newPercentage = player.GetExpPercentage() + Param[0];
			player.SetExpPercentage(newPercentage);
			return TRUE;
		}
	}

	return FALSE;
}

int buff_modify_quest_exp_percentage(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever))
	{
		int playerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if(IsValidPlayer(playerIndex))
		{
			KPlayer& player = Player[playerIndex];
			int newPercentage = player.GetQuestExpPercentage() + Param[0];
			player.SetQuestExpPercentage(newPercentage);
			return TRUE;
		}
	}

	return FALSE;
}

int buff_modify_skill_exp_percentage(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever))
	{
		int playerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if(IsValidPlayer(playerIndex))
		{
			KPlayer& player = Player[playerIndex];
			int newPercentage = player.GetSkillExpPercentage() + Param[0];
			player.SetSkillExpPercentage(newPercentage);
			return TRUE;
		}
	}

	return FALSE;
}

int buff_add_skill_exp(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever))
	{
		int playerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if(IsValidPlayer(playerIndex))
		{
			int addSkillExp = Param[0];
			if (addSkillExp > 0 && addSkillExp <= MAX_ADD_SKILL_EXP)
			{
				//防沉迷
				int nAntiEnthrallState = Player[playerIndex].m_AntiEnthrall.GetCurState();
				
				if(AntiEnthrall::enAntiEnthrall_Weariness == nAntiEnthrallState)
					addSkillExp /= AntiEnthrall::WEARINESS_EXP_SCALE;
				else if(AntiEnthrall::enAntiEnthrall_Insalubrity == nAntiEnthrallState)
					return TRUE ;
				
				Player[playerIndex].DirectAddSkillExp((DWORD)addSkillExp);

				return TRUE;
			}
		}
	}

	return FALSE;
}

int buff_same_team(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever) && IsValidNpc(Env.nBuffSender))
	{
		int senderPlayerIndex = 0;
		if (Npc[Env.nBuffSender].m_Kind == kind_player)
			senderPlayerIndex = Npc[Env.nBuffSender].GetPlayerIdx();
		else if (Npc[Env.nBuffSender].m_Kind == kind_creature)
			senderPlayerIndex = Npc[Env.nBuffSender].GetSummonerIdx();
		else if (Npc[Env.nBuffSender].m_Kind == kind_employee)
			senderPlayerIndex = Npc[Env.nBuffSender].GetEmployerIdx();

		int receiverPlayerIndex = 0;
		if (Npc[Env.nBuffRecever].m_Kind == kind_player)
			receiverPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		else if (Npc[Env.nBuffRecever].m_Kind == kind_creature)
			receiverPlayerIndex = Npc[Env.nBuffRecever].GetSummonerIdx();
		else if (Npc[Env.nBuffRecever].m_Kind == kind_employee)
			receiverPlayerIndex = Npc[Env.nBuffRecever].GetEmployerIdx();

		if(IsValidPlayer(senderPlayerIndex) && IsValidPlayer(receiverPlayerIndex))
		{
			if (senderPlayerIndex == receiverPlayerIndex)
			{
				return TRUE;
			}
			else
			{
				KPlayerTeam& senderTeamInfo = Player[senderPlayerIndex].GetTeamInfo();
				KPlayerTeam& receiverTeamInfo = Player[receiverPlayerIndex].GetTeamInfo();
				if (senderTeamInfo.IsInTeam() && receiverTeamInfo.IsInTeam() && senderTeamInfo.GetTeamId() == receiverTeamInfo.GetTeamId())
				{
					return TRUE;
				}
			}
		}
	}

	if(IsValidNpc(Env.nEventRecever) && IsValidNpc(Env.nEventSender))
	{
		int senderPlayerIndex = 0;
		if (Npc[Env.nBuffSender].m_Kind == kind_player)
			senderPlayerIndex = Npc[Env.nBuffSender].GetPlayerIdx();
		else if (Npc[Env.nBuffSender].m_Kind == kind_creature)
			senderPlayerIndex = Npc[Env.nBuffSender].GetSummonerIdx();
		else if (Npc[Env.nBuffSender].m_Kind == kind_employee)
			senderPlayerIndex = Npc[Env.nBuffSender].GetEmployerIdx();

		int receiverPlayerIndex = 0;
		if (Npc[Env.nBuffRecever].m_Kind == kind_player)
			receiverPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		else if (Npc[Env.nBuffRecever].m_Kind == kind_creature)
			receiverPlayerIndex = Npc[Env.nBuffRecever].GetSummonerIdx();
		else if (Npc[Env.nBuffRecever].m_Kind == kind_employee)
			receiverPlayerIndex = Npc[Env.nBuffRecever].GetSummonerIdx();

		if(IsValidPlayer(senderPlayerIndex) && IsValidPlayer(receiverPlayerIndex))
		{
			if (senderPlayerIndex == receiverPlayerIndex)
			{
				return TRUE;
			}
			else
			{
				KPlayerTeam& senderTeamInfo = Player[senderPlayerIndex].GetTeamInfo();
				KPlayerTeam& receiverTeamInfo = Player[receiverPlayerIndex].GetTeamInfo();
				if (senderTeamInfo.IsInTeam() && receiverTeamInfo.IsInTeam() && senderTeamInfo.GetTeamId() == receiverTeamInfo.GetTeamId())
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

int buff_match_level(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever) && IsValidNpc(Env.nBuffSender))
	{
		int senderLevel = Npc[Env.nBuffSender].GetLevel();
		int receiverLevel = Npc[Env.nBuffRecever].GetLevel();
		if (senderLevel * Param[0] + Param[2] <= receiverLevel &&
			senderLevel * Param[1] + Param[3] >= receiverLevel)
		{
			return TRUE;
		}
	}

	if(IsValidNpc(Env.nEventRecever) && IsValidNpc(Env.nEventSender))
	{
		int senderLevel = Npc[Env.nEventSender].GetLevel();
		int receiverLevel = Npc[Env.nEventRecever].GetLevel();
		if (senderLevel * Param[0] + Param[2] <= receiverLevel &&
			senderLevel * Param[1] + Param[3] >= receiverLevel)
		{
			return TRUE;
		}
	}

	return FALSE;
}

int buff_revivalpos(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nEventType == buff_event_type_none )
	{
		KNpc& npc = Npc[Env.nBuffRecever];
		if( npc.IsPlayer( ) )
		{
			KPlayer& player = Player[npc.GetPlayerIdx( )];
			int	nWorldIdx = npc.m_SubWorldIndex;
			int	nMapId = SubWorld[nWorldIdx].m_SubWorldID;

			if( Param[0] != 0 )
			{
				player.SetRevivalPos( nMapId, Param[0] );
			}
			else
			{
				int nRevivalID = g_SubWorldSet.GetRevivalID( nMapId );
				if( nRevivalID != 0 )
				player.SetRevivalPos( nMapId, nRevivalID );
			}

			return TRUE;
		}
	}
	return FALSE;
}

int buff_isespecialarea(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever))
	{
		return Npc[Env.nBuffRecever].IsInEspecialArea(Param[0]) ? TRUE : FALSE;
	}

	if(IsValidNpc(Env.nEventRecever))
	{
		return Npc[Env.nEventRecever].IsInEspecialArea(Param[0]) ? TRUE : FALSE;
	}

	return FALSE;
}

int buff_issenderespecialarea( 	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if (IsValidNpc(Env.nBuffSender))
	{
		return Npc[Env.nBuffSender].IsInEspecialArea(Param[0])?TRUE : FALSE;
	}//endif

	return FALSE;
}

int buff_visible_to_npc(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever))
	{
		Npc[Env.nBuffRecever].SetVisibleToNpcCount(Npc[Env.nBuffRecever].GetVisibleToNpcCount() + Param[0]);
		return TRUE;
	}

	return FALSE;
}

int buff_changethreat(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever) && IsValidNpc(Env.nBuffSender))
	{
		NpcController& controller = Npc[Env.nBuffRecever].GetController();
		if (controller.IsActive() && Npc[Env.nBuffSender].IsVisibleToNpc())
		{
			controller.GetThreatMonitor().ChangeEnemyThreat(Npc[Env.nBuffSender].GetId(), Env.nBuffSender, Param[0]);
			return TRUE;
		}
	}

	return FALSE;
}

int buff_random_trans(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nEventType == buff_event_type_none )
	{
		if(IsValidNpc(Env.nBuffRecever))
		{
			if (Npc[Env.nBuffRecever].RandomTrans())
				return TRUE;
		}
	}

	return FALSE;
}


int buff_player_change_world_to_sender(BUFF_ENV_PARAM& Env,BUFF_PARAM& Param )
{
	if( IsValidNpc(Env.nBuffRecever) && IsValidNpc(Env.nBuffSender) 
		&& Npc[Env.nBuffSender].m_Kind == kind_player && Npc[Env.nBuffRecever].m_Kind == kind_player)
	{
		int nBuffSenderWorldIndex = Npc[Env.nBuffSender].GetSubWorldIndex();
		int nSenderX = 0;
		int nSenderY = 0;

		Npc[Env.nBuffSender].GetMpsPos(&nSenderX,&nSenderY);

		if (nBuffSenderWorldIndex != INVALID_WORLD_INDEX && nBuffSenderWorldIndex < MAX_SUBWORLD)
		{
			if (SubWorld[nBuffSenderWorldIndex].GetInstanceId() == INVALID_INSTANCE_ID)
			{
				Npc[Env.nBuffRecever].ChangeWorld(
					SubWorld[nBuffSenderWorldIndex].m_SubWorldID,
					nSenderX,
					nSenderY,
					false);
			}//endif
			else
				return FALSE;

			return TRUE;
		}//endif

	}//endif
	
	return FALSE;
}

int buff_modify_player_weight_max( BUFF_ENV_PARAM& Env,BUFF_PARAM& Param )
{
	int npcIndex = 0;
	if (Env.nEvent == buff_event_type_none)
	{
		npcIndex = Env.nBuffRecever;
	}
	else
	{
		npcIndex = Env.nEventRecever;
	}//end else

	if (IsValidNpc(npcIndex))
	{
		int nPlayerIndex = Npc[npcIndex].GetPlayerIdx();
		if (IsValidPlayer(nPlayerIndex))
		{
			int  nAdded      = Param[0];
			if (nAdded != 0)
			{	
				Player[nPlayerIndex].AddWeightMax(nAdded,true);
				Player[nPlayerIndex].CheckWeight();
			}//endif
			
			return TRUE;
		}//endif
			
	}//endif

	return FALSE;
}

int buff_player_change_world_to_reciever(BUFF_ENV_PARAM& Env,BUFF_PARAM& Param )
{
	if( IsValidNpc(Env.nBuffRecever) && IsValidNpc(Env.nBuffSender) 
		&& Npc[Env.nBuffSender].m_Kind == kind_player && Npc[Env.nBuffRecever].m_Kind == kind_player)
	{
		int nBuffRecieverWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex();
		int nReciverX = 0;
		int nReciverY = 0;
		
		Npc[Env.nBuffRecever].GetMpsPos(&nReciverX,&nReciverY);
		
		if (nBuffRecieverWorldIndex != INVALID_WORLD_INDEX && nBuffRecieverWorldIndex < MAX_SUBWORLD)
		{
			if (SubWorld[nBuffRecieverWorldIndex].GetInstanceId() == INVALID_INSTANCE_ID)
			{
				Npc[Env.nBuffSender].ChangeWorld(
					SubWorld[nBuffRecieverWorldIndex].m_SubWorldID,
					nReciverX,
					nReciverY,
					false);
			}//endif
			else
				return FALSE;
			
			return TRUE;
		}//endif
		
	}//endif
	
	return FALSE;
}

int buff_set_private_state(BUFF_ENV_PARAM& Env,BUFF_PARAM& Param )
{
	if(IsValidNpc(Env.nBuffRecever) && Npc[Env.nBuffRecever].m_Kind == kind_player)
	{
		int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if (IsValidPlayer(nPlayerIndex))
		{	
			if (Param[0] == 1)
				Player[nPlayerIndex].SetComoflag(true);
			else
				Player[nPlayerIndex].SetComoflag(false);
			
			return TRUE;
		}//endif
		
	}//endif
	
	return FALSE;
}

int buff_change_dir(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if( Env.nEventType == buff_event_type_none )
	{
		if(IsValidNpc(Env.nBuffRecever))
		{
			return TRUE;
		}
	}

	return FALSE;
}

int buff_manatolife(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if(Env.nBuffRecever > 0 &&
		Env.nBuffRecever < MAX_NPC )
	{	
		int nCurMana = Npc[Env.nBuffRecever].m_UnaryAttrMgr[nuai_curmana];

		int nConsumeMana = nCurMana * Param[0] / 100;
		
		nCurMana = nCurMana > nConsumeMana ? (nCurMana - nConsumeMana) : 0 ;

		Npc[Env.nBuffRecever].m_UnaryAttrMgr.Set( nuai_curmana, nCurMana );
		

		
		int nCurLife = Npc[Env.nBuffRecever].m_UnaryAttrMgr[nuai_curlife];
		
		nCurLife += nConsumeMana * Param[1] / 100;

		int nMaxLife = Npc[Env.nBuffRecever].m_CompAttrMgr[ncai_lifeuplimit];

		nCurLife = nCurLife > nMaxLife ? nMaxLife : nCurLife ;
		
		Npc[Env.nBuffRecever].m_UnaryAttrMgr.Set( nuai_curlife, nCurLife );

		return TRUE;
	}

	return FALSE;
}

int buff_lifetoexplode(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if( Env.nEventType == buff_event_type_none )
		return FALSE;
	
	switch( Env.nEventType ) 
	{
	case buff_event_type_explodecalc:
		{
			int nCurLife = Npc[Env.nEventRecever].m_UnaryAttrMgr[nuai_curlife];			
			int nMaxLife = Npc[Env.nEventRecever].m_CompAttrMgr[ncai_lifeuplimit];
			int nLifePercent = nCurLife * 100 / nMaxLife;

			if( nLifePercent > Param[1] )
				Env.nEventValue += ( ( 100 - nLifePercent ) * Param[0] ) / 10;
			else
				Env.nEventValue += ( ( 100 - Param[1] ) * Param[0] ) / 10;
		}
		break;
	case buff_event_type_skillin:
	case buff_event_type_skillout:
	case buff_event_type_buffin:
	case buff_event_type_buffout:
	case buff_event_type_damagein:
	case buff_event_type_damageout:
	case buff_event_type_npcdeathin:
	case buff_event_type_npcdeathout:
	case buff_event_type_creaturedeath:
	case buff_event_type_explodeout:
	case buff_event_type_blood:
	default:
		break;		
	}
	
	return FALSE;
}

int buff_enter_instance(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	int npcIndex = Env.nBuffRecever;
	int worldTemplateId = Param[0];
	int entryIndex = Param[1] - 1;
	if (IsValidNpc(npcIndex) && worldTemplateId >= 0 && entryIndex >= 0)
	{
		int playerIndex = Npc[npcIndex].GetPlayerIdx();
		if (IsValidPlayer(playerIndex))
		{
			return g_SubWorldSet.EnterInstance(playerIndex, worldTemplateId, entryIndex);
		}
	}
	

	return FALSE;
}

int buff_visible_to_player(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param  )
{
	if(IsValidNpc(Env.nBuffRecever))
	{
		Npc[Env.nBuffRecever].SetVisibleToPlayerCount(Npc[Env.nBuffRecever].GetVisibleToPlayerCount() + Param[0]);
		return TRUE;
	}

	return FALSE;
}

int buff_larger_level(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{
	if (Env.nEventType == buff_event_type_none)
	{
		if (IsValidNpc(Env.nBuffRecever) && IsValidNpc(Env.nBuffSender))
		{
			if(Npc[Env.nBuffSender].GetLevel() >= Npc[Env.nBuffRecever].GetLevel())
				return TRUE;
		}
	}
	else
	{
		if (IsValidNpc(Env.nEventRecever) && IsValidNpc(Env.nEventSender))
		{
			if(Npc[Env.nEventSender].GetLevel() >= Npc[Env.nEventRecever].GetLevel())
				return TRUE;
		}
	}

	return FALSE;
}

int buff_exe_script(
	BUFF_ENV_PARAM& Env,
	BUFF_PARAM& Param )
{	
	if (IsValidNpc(Env.nBuffRecever))
	{
		KNpc& npc = Npc[Env.nBuffRecever];

		int scriptFuncId = Param[0];
		char scriptFuncName[32] = { 0 };
		snprintf(scriptFuncName, sizeof(scriptFuncName), "BuffFunction%d", scriptFuncId);
		
		int senderPlayerIndex = 0;
		if (IsValidNpc(Env.nBuffSender))
		{
			senderPlayerIndex = Npc[Env.nBuffSender].GetPlayerIdx();
			if (!IsValidPlayer(senderPlayerIndex))
			{
				senderPlayerIndex = 0;
			}
		}

		int playerIndex = npc.GetPlayerIdx();
		if (IsValidPlayer(playerIndex))
		{
			KPlayer& player = Player[playerIndex];
			
			if (player.ExecuteScript("\\script\\main.lua", scriptFuncName, Env.nBuffRecever))
				return TRUE;
		}
		else
		{
			if (ExecuteScript("\\script\\main.lua", scriptFuncName, Env.nBuffRecever, npc.GetSubWorldIndex()))
				return TRUE;
		}
	}

	return FALSE;
}

int buff_iswarcondvalid(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if( !IsValidNpc(Env.nBuffSender) )
		return FALSE;

	if( !Npc[Env.nBuffSender].IsPlayer() )
		return FALSE;
	
	int nPlayerIdx = Npc[Env.nBuffSender].GetPlayerIdx();

	if (!GetGlobalTongWarMgr().IsInitedAll())
	{
		ChatErrCodeToClient(nPlayerIdx, chat_err_pool_sys_busy);
		return FALSE; 
	}//endif

	BUFF_PARAM	newParam;
	BOOL ret = TRUE;

	if (IsValidPlayer(nPlayerIdx))
	{
		SocialUnit * pLeafUnit = GetLeafUnit(nPlayerIdx,enSUTplId_Tong);
		if (!pLeafUnit)
		{
			ChatErrCodeToClient(nPlayerIdx, chat_err_pool_sys_busy);
			return FALSE;
		}//endif

		SocialUnit * pTongUnit = GetUpNUnit(pLeafUnit,enSULayer_League);
		if (!pTongUnit)
		{
			ChatErrCodeToClient(nPlayerIdx, chat_err_war_mustbetongowner);
			return FALSE;	
		}//endif

		int nSubWorldIndex = Npc[Env.nBuffSender].GetSubWorldIndex();
		if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
			return FALSE;

		int nSubWorldTemplateID = SubWorld[nSubWorldIndex].GetWorldTemplateId();

		if (!GetGlobalTongWarMgr().IsTongWarMap(nSubWorldTemplateID) )
		{
			ChatErrCodeToClient(nPlayerIdx, chat_err_war_mustinwarmap);
			return FALSE;
		}//endif

		if ( GetGlobalWarInfoManager().ExistWarInfo(nSubWorldTemplateID) )
		{
			ChatErrCodeToClient(nPlayerIdx, chat_err_war_cityinwardeclared);
			return FALSE;
		}//endif

		
		KWarInfoManager & gWarInfoMgr = GetGlobalWarInfoManager();
		
		if (gWarInfoMgr.ExistWarInfo(pTongUnit->GetUnitGuid()))
		{
			ChatErrCodeToClient(nPlayerIdx, char_err_war_source_tong_inwar);
			return FALSE;
		}//endif

		int nLordIndex = SubWorld[nSubWorldIndex].GetLord();
		if (IsValidNpc(nLordIndex))
		{
			FSGUID Defender = Npc[nLordIndex].GetLord();
			if (Defender.data[0]!=0)
			{
				if (gWarInfoMgr.ExistWarInfo(Defender))
				{
					ChatErrCodeToClient(nPlayerIdx, char_err_war_dest_tong_inwar);
					return FALSE;
				}//endif

			}//endif

		}//endif

	}//endif

	newParam[0] = Param[0];
	if( !buff_ismapgroup(Env, newParam) )
	{
		ChatErrCodeToClient(nPlayerIdx, chat_err_war_mustinwarmap);
		return FALSE;
	}

	newParam[0] = Param[1];
	if( !buff_isespecialarea(Env, newParam) )
	{
		ChatErrCodeToClient(nPlayerIdx, chat_err_war_mustinspecarea);
		return FALSE;
	}

	if( buff_ishaveworldrobber(Env, newParam) )
	{
		ChatErrCodeToClient(nPlayerIdx, chat_err_war_cityinwardeclared);
		return FALSE;
	}

	if( buff_isworldowner(Env, newParam) )
	{
		ChatErrCodeToClient(nPlayerIdx, chat_err_war_nowartoowncity);
		return FALSE;
	}

	newParam[0] = Param[2];
	if( buff_isworldlordhavebuff(Env, newParam) )
	{
		ChatErrCodeToClient(nPlayerIdx, char_err_war_cityinwarprotected);
		return FALSE;
	}

	newParam[0] = Param[3];
	if( !buff_issowner(Env, newParam) )
	{
		ChatErrCodeToClient(nPlayerIdx, chat_err_war_mustbetongowner);
		return FALSE;
	}

	return ret;
}

int buff_is_in_my_city(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param
)
{
	if ( !IsValidNpc(Env.nBuffSender) )
		return FALSE;

	if ( !Npc[Env.nBuffSender].IsPlayer() )
		return FALSE;

	int nPlayerIndex = Npc[Env.nBuffSender].GetPlayerIdx();
	if ( !IsValidPlayer(nPlayerIndex) )
		return FALSE;

	SocialUnit* pLeafUnit = GetLeafUnit(nPlayerIndex, enSUTplId_Tong);
	if ( !pLeafUnit )
		return FALSE;
		
	SocialUnit* pUnit = GetUpNUnit(pLeafUnit, enSULayer_League);
	if ( !pUnit )
		return FALSE;

	int nMapID = GetCityMapId(pUnit->GetUnitAttr());
	if ( nMapID == INVALID_WORLD_ID )
		return FALSE;

	//占领的城市
	int CityMapSubWorldIndex = g_SubWorldSet.SearchWorld(nMapID);
	if ( CityMapSubWorldIndex < 0 || CityMapSubWorldIndex >= MAX_SUBWORLD )
		return FALSE;

	//当前所在的城市
	int nNpcInSubWorldIndex = Npc[Env.nBuffSender].GetSubWorldIndex();
	if ( nNpcInSubWorldIndex < 0 || nNpcInSubWorldIndex >= MAX_SUBWORLD )
		return FALSE;

	if ( CityMapSubWorldIndex != nNpcInSubWorldIndex )
		return FALSE;

	return TRUE;
}

int buff_set_pk_punish(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if(IsValidNpc(Env.nBuffRecever))
	{
		int playerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if (IsValidPlayer(playerIndex))
		{
			KPlayer& player = Player[playerIndex];
			int changeValue = Param[0];
			player.SetPkPunish(player.GetPkPunish() + changeValue);

			return TRUE;
		}
	}

	return FALSE;
}

int buff_set_death_punish(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if(IsValidNpc(Env.nBuffRecever))
	{
		int playerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if (IsValidPlayer(playerIndex))
		{
			KPlayer& player = Player[playerIndex];
			int changeValue = Param[0];
			player.SetDeathPunish(player.GetDeathPunish() + changeValue);

			return TRUE;
		}
	}

	return FALSE;
}

int buff_is_weekday(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	int weekday = Param[0];
	if (weekday >= 0 && weekday <= 6)
	{
		time_t currentTime;
		//time(&currentTime);
		currentTime = UNIX_TMIE_STAMP;
		tm* pLocalTime = localtime(&currentTime);
		if (pLocalTime && (pLocalTime->tm_wday == weekday))
			return TRUE;
	}

	return FALSE;
}

int buff_not_weekday(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	int weekday = Param[0];
	if (weekday >= 0 && weekday <= 6)
	{
		time_t currentTime;
		//time(&currentTime);
		currentTime = UNIX_TMIE_STAMP;
		tm* pLocalTime = localtime(&currentTime);
		if (pLocalTime && (pLocalTime->tm_wday != weekday))
			return TRUE;
	}

	return FALSE;
}

int buff_is_have_map_flag(
	BUFF_ENV_PARAM & Env,
	BUFF_PARAM     & Param)
{
	int npcIndex = 0;
	if (Env.nEvent == buff_event_type_none)
	{
		npcIndex = Env.nBuffRecever;
	}
	else
	{
		npcIndex = Env.nEventRecever;
	}
	
	if(IsValidNpc(npcIndex))
	{
		KNpc&         npc = Npc[npcIndex];
	    int   nWorldIndex = Npc[npcIndex].GetSubWorldIndex();
		if (nWorldIndex >= 0 && nWorldIndex < MAX_SUBWORLD)
		{
			int nFlageIndex = Param[0];
			return SubWorld[nWorldIndex].IsHaveMapFlage(nFlageIndex);
		}//endif

	}//endif

	return FALSE;
}

int buff_is_in_instance(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	int npcIndex = 0;
	if (Env.nEvent == buff_event_type_none)
	{
		npcIndex = Env.nBuffRecever;
	}
	else
	{
		npcIndex = Env.nEventRecever;
	}

	if(IsValidNpc(npcIndex))
	{
		KNpc& npc = Npc[npcIndex];
		WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(SubWorld[npc.GetSubWorldIndex()].GetWorldTemplateId());
		if (pSetting && pSetting->IsInstance)
			return TRUE;
	}

	return FALSE;
}

int buff_sync_to_world(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if (!IsValidNpc(Env.nBuffRecever))
		return FALSE;

	KNpc& npc = Npc[Env.nBuffRecever];
	npc.SetSyncToWorldMode(Param[0]);
	npc.SetSyncToWorldParam(0, Param[1]);
	npc.SetSyncToWorldParam(1, Param[2]);

	return TRUE;
}

int buff_onwardeclared(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if( !IsValidNpc(Env.nBuffSender) )
		return FALSE;

	if( !Npc[Env.nBuffSender].IsPlayer() )
		return FALSE;

	
	if (!GetGlobalTongWarMgr().IsInitedAll())
	{
		return FALSE; 
	}//endif

	int nPlayerIdx = Npc[Env.nBuffSender].GetPlayerIdx();
	
	int nSenderSubWorldIndex = Npc[Env.nBuffSender].GetSubWorldIndex();
	if (nSenderSubWorldIndex < 0 || nSenderSubWorldIndex >= MAX_SUBWORLD)
		return FALSE;
	
	int nSenderSubWorldTemplateID = SubWorld[nSenderSubWorldIndex].GetWorldTemplateId();


	SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIdx, enSUTplId_Tong);
	SocialUnit *pTongUnit = GetUpNUnit(pLeafUnit, enSULayer_League);

	if(NULL == pTongUnit)
		return FALSE;

	if (!GetGlobalTongWarMgr().IsTongWarMap(nSenderSubWorldTemplateID)
		|| GetGlobalWarInfoManager().ExistWarInfo(nSenderSubWorldTemplateID) 
		|| GetGlobalWarInfoManager().ExistWarInfo(pTongUnit->GetUnitGuid()))
		return FALSE;

	int nWorldIdx = Npc[Env.nBuffSender].GetSubWorldIndex();
	int nWorldLordIdx = SubWorld[nWorldIdx].GetLord();

	if(-1 == nWorldLordIdx)
		return FALSE;

	if (IsValidNpc(SubWorld[nWorldIdx].GetRobber()))
		return FALSE;
	else
	{	
		ConfigManager    & cm       = ConfigManager::Singleton();
		int nRobberTempID           = cm.GetGlobalVariable(global_var_tong_war_robber_tempID);
		int nX,nY;
		Npc[Env.nBuffSender].GetMpsPos( &nX, &nY );
		
		int	nNpcIdxInfo = MAKELONG(1, nRobberTempID);
		
		int nNpcIdx = NpcSet.Add(
			nNpcIdxInfo, 
			nWorldIdx, 
			nX , 
			nY );

		if( nNpcIdx > 0 )
		{
			//Debug for robber
			char infoStr[512];
			sprintf(infoStr, "Add Robber declare:MapId:%d,NpcIdx:%d,RegionIdx:%d", SubWorld[nWorldIdx].m_SubWorldID, nNpcIdx, Npc[nNpcIdx].m_RegionIndex);
			infoStr[511] = 0;
			GetGlobalTongWarMgr().DumpRobberInfo(infoStr, strlen(infoStr) + 1);


			Npc[nNpcIdx].NormalSync( );
			
			int nMode = Npc[nNpcIdx].m_UnaryAttrMgr[nuai_deathmode];
			nMode |= npc_deathmode_autodel;
			Npc[nNpcIdx].m_UnaryAttrMgr.Set( nuai_deathmode, nMode );
			
			Npc[nNpcIdx].SetLord(pTongUnit->GetUnitGuid());
			SubWorld[nWorldIdx].SetRobber(nNpcIdx);

			const KNpcTemplate *pTemplate = Npc[nNpcIdx].GetTemplate();
			if( NULL != pTemplate && pTemplate->NeedSave() )
				Npc[nNpcIdx].Save();

		}//endif
		else
			return FALSE;
		
	}//end else

    //宣战方保护
	SocialUnitAttr      & invaderAttr= pTongUnit->GetUnitAttr();
	if (invaderAttr.IsAttrHasData(enSUAttr_CityMap))
	{
		char* pData = NULL;
		int nSize = invaderAttr.GetAttr( enSUAttr_CityMap, pData );
		if (nSize == sizeof(int))
		{
			int invaderMapId    = *((int*)pData);
			int invaderMapIndex = g_SubWorldSet.SearchWorld(invaderMapId); 
			if (invaderMapIndex!=INVALID_WORLD_INDEX)
			{
				int nInvaderMapLord              = SubWorld[invaderMapIndex].GetLord();
				if (IsValidNpc(nInvaderMapLord))
				{
					ConfigManager       & cm         = ConfigManager::Singleton();
					int          nProtecteBuffId     = cm.GetGlobalVariable(global_var_tong_war_protect_buff);
					BuffMgr             & buffman    = BuffMgr::Singleton();
					if(!buffman.IsHaveBuff(nInvaderMapLord,nProtecteBuffId))
						buffman.AddNpcBuff(nInvaderMapLord,nInvaderMapLord,nProtecteBuffId);
				}//endif
				
			}//endif
			
		}//endif
		
	}//endif
	
	NotifyDeclareWarSucceed(nPlayerIdx);		

	FSWarInfo info;
	info.invaderGUID = pTongUnit->GetUnitGuid();
	info.defenderGUID = Npc[nWorldLordIdx].GetLord();
	info.mapID = SubWorld[nWorldIdx].m_SubWorldID;
	info.warState    = FS_WAR_STATE_NOTIFY;
	info.warDecTime  = UNIX_TMIE_STAMP;
	info.warProTime  = 0;

	return GetGlobalWarInfoManager().AddRecord(info);
}

int buff_TongWarStatueUsed(BUFF_ENV_PARAM& Env,
						   BUFF_PARAM& Param)
{
	if (IsValidNpc(Env.nBuffRecever))
	{
		StatueInfoMgr& sm = StatueInfoMgr::Singleton();
		sm.FillStatueUsedInfo(Param[0], Env.nBuffRecever);
	}//endif

	return TRUE;
}

int buff_PKValueCheck(BUFF_ENV_PARAM &Env,
					  BUFF_PARAM     &Param)
{
	if (IsValidNpc(Env.nBuffRecever))
	{
		int nPkValue = Param[0];
		int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();

		if (IsValidPlayer(nPlayerIndex))
		{
			int nPlayerPKValue = Player[nPlayerIndex].GetPkValue();

			if (nPlayerPKValue >= nPkValue)
				return TRUE;
		}
	}

	return FALSE;
}

int buff_SetFury(BUFF_ENV_PARAM &Env,
				 BUFF_PARAM &Param)
{
	if (IsValidNpc(Env.nBuffRecever) && Npc[Env.nBuffRecever].IsPlayer())
	{
		int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		if (IsValidPlayer(nPlayerIndex))
			Player[nPlayerIndex].GetFurySys().SetCurFuryExp(Param[0]);
	}//endif

	return TRUE;
}

int buff_AddFury(BUFF_ENV_PARAM &Env,
				 BUFF_PARAM     &Param)
{
	if (IsValidNpc(Env.nBuffRecever) && Npc[Env.nBuffRecever].IsPlayer())
	{
		int nPlayerIndex = Npc[Env.nBuffRecever].GetPlayerIdx();
		
		if (IsValidPlayer(nPlayerIndex))
		{
			int nCurrentFury = Player[nPlayerIndex].GetFurySys().GetFuryExp();
			int nAdded       = Param[0];
			
			int nNewExp      = nCurrentFury + nAdded;
			
			if  (nNewExp > 100 )
				nNewExp      = 100;
			
			if (nNewExp < 0)
				nNewExp      = 0;
			
			Player[nPlayerIndex].GetFurySys().SetCurFuryExp(nNewExp);

		}//endif

	}//endif

	return TRUE;
}

int buff_is_my_employer(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if( IsValidNpc(Env.nBuffSender) && IsValidNpc(Env.nBuffRecever) )
	{
		if( Npc[Env.nBuffSender].IsEmployee() && Npc[Env.nBuffRecever].IsPlayer() )
		{
			return ( Npc[Env.nBuffSender].GetEmployerIdx() == Npc[Env.nBuffRecever].GetPlayerIdx() );
		}
	}

	return false;
}

/*
int buff_onwarstart(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if( IsValidNpc(Env.nBuffSender) )
	{
		int nWorldIdx = Npc[Env.nBuffSender].GetSubWorldIndex();

		return GetGlobalWarInfoManager().ChgState(Npc[Env.nBuffSender].GetLord(),
			SubWorld[nWorldIdx].m_SubWorldID,
			FS_WAR_STATE_PROCESS);
	}

	return FALSE;
}
*/
/*
int buff_onwarend(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if( IsValidNpc(Env.nBuffRecever) )
	{
		int nWorldIdx = Npc[Env.nBuffRecever].GetSubWorldIndex();

		return GetGlobalWarInfoManager().DelRecord(Npc[Env.nBuffRecever].GetLord(),
			SubWorld[nWorldIdx].m_SubWorldID);
	}

	return FALSE;
}

int buff_warmsgtotong(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	static const char *WARMSG[] = 
		{
			MSG_WAR_NOTIFY_DECLARED,
			MSG_WAR_NOTIFY_BEFORESTART,
			MSG_WAR_NOTIFY_START
		};

	if( !IsValidNpc(Env.nBuffSender) )
		return FALSE;

	if(Param[0] < 0  || Param[0] > 2)
		return FALSE;

	char szMapName[MAXSIZE_WORLD_NAME];
	int nWorldIdx = Npc[Env.nBuffRecever].GetSubWorldIndex();

	if( !g_SubWorldSet.GetWorldNameFromID(SubWorld[nWorldIdx].m_SubWorldID, szMapName, sizeof(szMapName)) )
		return FALSE;

	szMapName[sizeof(szMapName) - 1] = 0;
	
	char szMsg[MAXSIZE_CHAT_MSG];
	int	nMsgLen;
	nMsgLen = snprintf(szMsg, sizeof(szMsg), WARMSG[Param[0]], szMapName);

	if(nMsgLen < 0 || nMsgLen >= sizeof(szMsg))
		return FALSE;

	const FSGUID &lordGuid = Npc[Env.nBuffSender].GetLord();
	SocialUnit *pUnit = ServerSocialUnitMgr::Singleton().GetUnit(lordGuid, enSUTplId_Tong);

	g_ChatCenterS.SysMsgToTong(pUnit, 
		SYSMSG_TYPE_STR, 
		(const BYTE*)&szMsg, 
		nMsgLen
		);
		
	return TRUE;
}

int buff_robreswithmsg(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if( !IsValidNpc(Env.nBuffRecever) )
		return FALSE;
	
	//接受者为Player
	if( Npc[Env.nBuffRecever].IsPlayer( ) )
		return FALSE;

	int nWorldIndex = Npc[Env.nBuffRecever].GetSubWorldIndex( );

	int nLordIndex = SubWorld[nWorldIndex].GetLord( );
	
	if( nLordIndex != Env.nBuffRecever )
		return FALSE;
	
	const FSGUID& RGUID = Npc[Env.nBuffRecever].GetRobber( );
	
	ServerSocialUnitMgr& SMgr = ServerSocialUnitMgr::Singleton( );

	SocialUnit* pUnit = SMgr.GetUnit( RGUID );

	if( pUnit )
	{
		int nMapId = GetCityMapId( pUnit->GetUnitAttr() );

		if(INVALID_WORLD_ID != nMapId)
		{
			int nRobWorldIndex = g_SubWorldSet.SearchWorld( nMapId );
			if( nRobWorldIndex != -1 )
			{
				int nRobWorldLordIndex = SubWorld[nRobWorldIndex].GetLord( );

				if( nRobWorldLordIndex != -1 )
				{
					int nType = Param[0] + nuai_lord_res0;
					int nValue = Npc[nLordIndex].m_UnaryAttrMgr[nType];
					int nRobValue = nValue * Param[1] / 100;
					
					if(nRobValue < 0)
						nRobValue = 0;

					if(nRobValue > nValue)
						nRobValue = nValue;

					Npc[nLordIndex].m_UnaryAttrMgr.Set( nType, nValue - nRobValue );
					Npc[nLordIndex].SetDataChangedFlag(true);
		
					nValue = Npc[nRobWorldLordIndex].m_UnaryAttrMgr[nType];
					Npc[nRobWorldLordIndex].m_UnaryAttrMgr.Set( nType, nValue + nRobValue );
					Npc[nRobWorldLordIndex].SetDataChangedFlag(true);

					int nTargetCityId = SubWorld[nWorldIndex].m_SubWorldID;
					const char *szTongName = GetUnitName( pUnit->GetUnitAttr() );
					NotifyRobRes(szTongName, nTargetCityId, Param[0], nRobValue);

					return TRUE;
				}
			}
		}
	}

	return FALSE;	
}
*/
/*
int buff_swaplrwithmsg(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param)
{
	if( Env.nBuffRecever >= 0 && 
		Env.nBuffRecever < MAX_NPC )
	{
		bool IsRobberHasCity = false;
		FSGUID	robberGuid = Npc[Env.nBuffRecever].GetRobber();

		ServerSocialUnitMgr	&mgr = ServerSocialUnitMgr::Singleton();

		if( IsGUIDValid(robberGuid) )
		{
			SocialUnit *pCityOwnerUnit = mgr.GetUnit(robberGuid, enSUTplId_Tong);
			
			if(NULL != pCityOwnerUnit)
			{
				int nCityMapId = GetCityMapId(pCityOwnerUnit->GetUnitAttr());

				if(INVALID_WORLD_ID != nCityMapId)
				{
					int nMapIdx = g_SubWorldSet.SearchWorld(nCityMapId);
					IsRobberHasCity = (INVALID_WORLD_INDEX != nMapIdx);

					// 如果数据错乱，则清空
					if(INVALID_WORLD_INDEX == nMapIdx)
					{
						_ASSERT(false);
						pCityOwnerUnit->GetUnitAttr().DelAttr(enSUAttr_CityMap);
					}
				}
			}
		}

		// 如果进攻方没有城市，则占领该城市，同时从防守方清除该城市
		if( !IsRobberHasCity )
		{
			
			SocialUnit	*pUnitAttacker = mgr.GetUnit(robberGuid, enSUTplId_Tong);

			if(pUnitAttacker)
			{
				int	nWorldIdx = Npc[Env.nBuffRecever].m_SubWorldIndex;
				int	nMapId = SubWorld[nWorldIdx].m_SubWorldID;
				SocialUnitAttr	&attr = pUnitAttacker->GetUnitAttr();
				attr.AddAttr(enSUAttr_CityMap, (const char*)&nMapId, sizeof(nMapId));
				SocialSerializer::Singleton().UpdateAttrReq(-1, pUnitAttacker);

				const char *szTongName = GetUnitName( pUnitAttacker->GetUnitAttr() );
				NotifyRobCity(szTongName, nMapId);
			}


			const FSGUID &guidDefender = Npc[Env.nBuffRecever].GetLord();

			if( IsGUIDValid(guidDefender) )
			{
				SocialUnit	*pUnitDefender = mgr.GetUnit(guidDefender, enSUTplId_Tong);

				if(pUnitDefender)
				{
					SocialUnitAttr	&attr = pUnitDefender->GetUnitAttr();
					attr.DelAttr(enSUAttr_CityMap);
					SocialSerializer::Singleton().UpdateAttrReq(-1, pUnitDefender);
				}
			}

			Npc[Env.nBuffRecever].SetLord(robberGuid);
		}

		FSGUID guid;
		Npc[Env.nBuffRecever].SetRobber( guid );
		Npc[Env.nBuffRecever].SetDataChangedFlag(true);
	}

	return FALSE;	
}
*/
/*
int buff_notifyrobberpos(
	BUFF_ENV_PARAM &Env,
	BUFF_PARAM &Param
	)
{
	if( !IsValidNpc(Env.nBuffRecever) )
		return FALSE;

	int nWorldIdx = Npc[Env.nBuffRecever].GetSubWorldIndex();
	int nRobberIdx = SubWorld[nWorldIdx].GetRobber();

	if(-1 == nRobberIdx)
		return FALSE;

	int	nMpsX, nMpsY;
	Npc[nRobberIdx].GetMpsPos(&nMpsX, &nMpsY);

	const FSGUID &lordGuid = Npc[Env.nBuffRecever].GetLord();
	SocialUnit *pUnit = ServerSocialUnitMgr::Singleton().GetUnit(lordGuid, enSUTplId_Tong);

	if(NULL == pUnit)
		return FALSE;

	char szMessage[MAXSIZE_CHAT_MSG];
	int nMsgLen;
	
	nMsgLen = snprintf(szMessage,
		sizeof(szMessage),
		MSG_WAR_ROBBERPOS_NOTIFY,
		nMpsX,
		nMpsY
		);

	if(nMsgLen > 0 && nMsgLen < sizeof(szMessage))
	{
		g_ChatCenterS.SysMsgToTong(pUnit,
			SYSMSG_TYPE_STR,
			(const BYTE*)szMessage,
			SYSTEM_ROOM_ID,
			MSG_SHOWTYPE_ROOM | MSG_SHOWTYPE_MIDDLESCREEN | MSG_SHOWTYPE_TOPSCREEN
			);

		return TRUE;
	}

	return FALSE;
}
*/

//=====================================================================================

_Buff_Action	g_BuffAction[] = 
{
	/*
	 *	pre condition
	 */
	{"None",				buff_none},
	{"IsMap",				buff_ismap},
	{"IsMapGroup",			buff_ismapgroup},							//地图分组
	{"IsMapCate",			buff_ismapcate},							//地图分类
	{"IsSOwner",			buff_issowner},
	{"IsHaveBuff",			buff_ishavebuff},
	{"IsEqualPile",			buff_isequalpile},	
	{"IsHaveWorldRobber",	buff_ishaveworldrobber},
	{"IsSameNpcLord",		buff_issamenpclord},
	{"IsSameNpcRobber",		buff_issamenpcrobber},
	{"IsWorldOwner",		buff_isworldowner},

	{"IsDeath",				buff_isdeath},					//判断是否死亡
	{"IsProf",				buff_isprof},					//判断职业
	{"IsSex",				buff_issex},					//判断性别
	{"IsPlayer",			buff_isplayer},					//判断是否玩家
	{"IsLevel",				buff_islevel },

	{"IsWorldLordHaveBuff", buff_isworldlordhavebuff},		//判断当前地图领主是否有buff	
	{"IsMyBaby",			buff_ismybaby},					//判断buff接收者是否为发送者的宝宝
	{"IsWarCondValid",		buff_iswarcondvalid},			//判断国战宣战条件是否满足
	{"IsInOwerCity",		buff_is_in_my_city},

	/*
	 *	Out Action
	 */

	{ "Add_V",				buff_property_add },			//改变NPC属性值
	{ "Add_B",				buff_addbufftorecver },			//给目标加一个BUFF
	{ "Add_BS",				buff_addbufftosender },			//给发送者加一个BUFF
	{ "Add_BSB",			buff_bsaddtoes },				//buff发送者给事件发送者加一个buff
	{ "Dec_B",				buff_decfromrecver },			//减少buff叠加数量
	
	
	{ "Cast_S",				buff_cast_skill },				//给目标Cast一个技能,主动输出以buff发送者为发送者,目标为目标,被动以过滤目标的发送者为发送者
	{ "Cast_SE",			buff_cast_skill_ext },			//给目标Cast一个技能,目标为发送中心,发送者为发送者	
	{ "Cast_ST",			buff_cast_skill_tag },			//给目标Cast一个技能
	{ "Enable_S",			buff_enable_skill },			//激活一个技能
	{ "Enable_SG",			buff_enable_skill_g },			//按组激活技能
	{ "NoMove",				buff_nomove },					//禁止某个NPC移动
	{ "Clear_B",			buff_clearbuff },				//清除某组Buff

	{ "Enable_SC",			buff_enable_skill_c },			//按类别激活技能
	{ "Clear_BC",			buff_clearbuff_c },				//清除某类别Buff
	{ "CD_SC",				buff_cooldown_skill_c },		//清除某类别技能CD时间

	{ "DoRevive",			buff_dorevive },				//复活一个玩家
	{ "DoDeath",			buff_dodeath },					//杀死一个玩家or Npc
	{ "DrawMe",				buff_drawme },					//将一个玩家传送到接受者附近 冲锋
	{ "CD_SG",				buff_cooldown_skill_g },		//清除某组技能CD时间
	{ "KillCreature",		buff_killcreature },			//杀死召唤兽
	{ "SaytoAll",			buff_saytoall },				//向全世界喊话
	{ "ShowMsg",			buff_showmsg },					//显示消息	
	{ "SaytoChannel",		buff_saytochannel },			//向除系统频道外的任意频道喊话

	{ "Add_BL",				buff_addbufftoworldlord },		//给接收者地图领主加BUFF
	{ "Add_BP",             buff_addbufftoworldpool },      //给地图分星池加BUFF
	{ "Add_BR",				buff_addbufftoworldrobber },	//给接收者地图子领主加BUFF
	{ "Add_BSL",			buff_addbufftosubworldlord },	//给接收者地图子领主加BUFF
	{ "Add_BSP",            buff_addbufftoworldsubpool },   //给接收者地图分星池的守护星加Buff
	{ "Swap_LR",			buff_swapnpclordrobber },		//用Npc攻击者替换占有者
	{ "Add_BSWL",			buff_addbufftospecifyworldlord}, // 给指定地图指定建筑加buff
	{ "AddBLIfNoLWL",		buff_addblifnolwl},				// 如果地图领主没有领主，则给地图领主加buff
	
	{ "NpcLord",			buff_setnpclord },				//添加、删除Npc占有者
	{ "NpcRobber",			buff_setnpcrobber },			//添加、删除Npc攻击者
	{ "WorldLord",			buff_setworldlord },			//添加、删除地图领主
	{ "WorldRobber",		buff_setworldrobber },			//添加、删除地图攻击者
	{ "WorldSubLord",		buff_setworldsublord },			//添加、删除地图子领主
	{ "WorldPool",          buff_setworldPool},             //添加、删除地图分星池
	{ "WorldSubPool",       buff_setworldSubPool},          //添加、删除地图分星池     
	{ "AddNpc",				buff_addnpc },					//直接添加Npc		如果添加者有社会关系，那么该NPC的lord为该社会关系
	//{ "DelNpc",				buff_delnpc },					//删除Npc
	{ "AddObj",				buff_addobj },					//添加Obj
	{ "PreventAddFriend",buff_setpreventaddfriend},

	{ "Add_BRL",			buff_addbufftoBRL },			//给接攻击者国家地图领主加BUFF
	{ "Add_Res",			buff_addres },					//添加资源或者减少
	{ "Dec_Res",			buff_decres },					//添加资源或者减少
	{ "Rob_Res",			buff_robres },					//掠夺占有方资源到攻击方

	{ "Add_Item",			buff_additem },					//添加物品
	{ "Del_Item",			buff_delitem },					//删除物品
	{ "Use_Item",			buff_useitem },					//使用物品

	{ "Mod_Item",			buff_moditem },					//编辑物品属性
	
	{ "Add_Skill",			buff_addskill },				//添加技能
	{ "Del_Skill",			buff_delskill },				//删除技能
	{ "Mod_Skill",			buff_modskill },				//编辑技能
	
	{ "Push_One",			buff_pushone },					//拉人
	{ "Pull_One",			buff_pullone },					//推人
	{ "SetPos",				buff_setpos },					//设置点
	{ "Offset",				buff_offset },					//设置偏移
	{ "NpcDrop",			buff_npcdrop },					//掉落配置
	

	
	{ "Set_Local",			buff_setlocal },				//设置局部变量
	{ "Get_Local",			buff_getlocal },				//获取局部变量

	{ "Set_Global",			NULL },							//设置全局变量
	{ "Get_Global",			NULL },							//获取全局变量

	{ "RideHorse",			buff_ridehorse },				//骑马	
	{ "BloodVat",			buff_bloodvat },
	{ "MagicVat",			buff_magicvat },	

	{ "Set_PKMode",			buff_setpkmode },				//设置PK模式
	{ "NewWorld",			buff_newworld },
	{ "RevWorld",			buff_revworld },
	{ "OnWarDeclared",		buff_onwardeclared },
	{ "TongWarStatueUsed",	buff_TongWarStatueUsed},
	{ "PKValueCheck",		buff_PKValueCheck},
	{ "SetFury",            buff_SetFury},                  //设置爆魂
	{ "AddFury",            buff_AddFury},
	{ "PRecChangeToSend",   buff_player_change_world_to_sender},//Reciever changeworld to sender world
	{ "PRecChangeToReciever",buff_player_change_world_to_reciever},//Sender changeWorld to reciever
	{ "ModifyPlayerMaxWeight",buff_modify_player_weight_max},
	{ "IsHaveMapFlage"      ,buff_is_have_map_flag},
	{ "SetPrivateState",    buff_set_private_state},
/*	{ "OnWarStart",			buff_onwarstart },
	{ "OnWarEnd",			buff_onwarend },
	{ "WarMsgToTong",		buff_warmsgtotong },
	{ "RobResWithMsg",		buff_robreswithmsg },
	{ "SwapLRWithMsg",		buff_swaplrwithmsg },
	{ "NotifyRobberPos",	buff_notifyrobberpos },
*/

	/*
	 *	Obj attribute
	 */

	{ "ID",					buff_id_attribute },
	{ "Group",				buff_group_attribute },
	{ "Cate",				buff_cate_attribute },
	{ "Volume",				buff_volume_attribute },
	{ "Format",				buff_format_attribute },
	{ "Robber",				buff_robber	},
	{ "Lord",				buff_lord },
	{ "Random",				buff_random },

	
	/*
	 *	Modify obj
	 */

	{"Trans_ID",			buff_transid},
	{"Trans_IDByOS",		buff_transidbyos},
	{"Trans_DM",			buff_transdamage},
	{"Trans_DMBV",			buff_transdamagebv},
	{"Kill_Self",			buff_killself},
	{"Rebound",				buff_reboundall},
	{"Rebound_DMP",			buff_rebounddamage_p},
	{"Rebound_DMV",			buff_rebounddamage_v},
	{"Sorb_DM",				buff_damagesorb},
	{"Trans_DMV",			buff_transdamage},
	{"Mod_BuffTime",		buff_modifybuff},
	{"Mod_BuffTime_C",		buff_modifybuff_C},
	{"Add_BuffTime_G",		buff_addbufftime_G},
	{"Add_BuffTime_C",		buff_addbufftime_C},
	{"Add_BuffTimeLimit_G",	buff_addbufftimelimit_G},
	{"Add_BuffTimeLimit_C",	buff_addbufftimelimit_C},
	{"Set_BuffTime_G",		buff_setbufftime_G},
	{"Set_BuffTime_C",		buff_setbufftime_C},
	{"Mod_BuffCap",			buff_modifybuffcap},
	{"Trans_Blood",			buff_transblood},
	{"Trans_ManaToLife",	buff_manatolife},
	{"Trans_LifeToExplode",	buff_lifetoexplode},	

	/*
	 * AI
	 */
	{"SetAIParam",			buff_setaiparam},				//设置AI参数
	{"BlockClientControl",	buff_block_clientcontrol},		//拦截客户端控制
	{"SetAIMode",			buff_setaimode},				//设置AI模式（预定义的AI参数组合）
	{"Follow",				buff_follow},					//跟随
	{"SetCreatureMark",		buff_setcreaturemark},			//设置召唤兽标记
	{"SetCreatureSkillAI",	buff_setcreatureskillai},		//设置召唤兽是否可以使用技能

	/*
	 * 怪物掉落物品属主
	 */
	{"NewItemOwner",		buff_new_item_owner},			//新的怪物掉落物品主人
	{"CanBeItemOwner",		buff_can_be_item_owner},		//是否可以成为物品主人

	{"AddExp",				buff_add_exp},					//增加经验
	{"AddSkillExp",			buff_add_skill_exp},			//增加技能经验
	{"SameTeam",			buff_same_team},				//是否同一个队伍
	{"MatchLevel",			buff_match_level},				//是否等级是否符合相应条件
	{"IsEspecialArea",		buff_isespecialarea},			//判断是否在指定的特殊区域
	{"IsSenderInEsArea",    buff_issenderespecialarea},     //判断buffsender 是否在指定特殊区域
	{"VisibleToNpc",		buff_visible_to_npc},			//设置是否可以被怪物AI发现
	{"ChangeThreat",		buff_changethreat},				//改变仇恨
	{"RevivalPos",			buff_revivalpos},				//设置人物重生点为当前地图重生点
	{"RandomTrans",			buff_random_trans},				//随机传送
	{"ChangeDir",			buff_change_dir},				//随机传送
	{"ExpPercentage",		buff_modify_exp_percentage},	//修改打怪经验获得百分比
	{"QuestExpPercentage",	buff_modify_quest_exp_percentage},	//修改任务经验获得百分比
	{"SkillExpPercentage",	buff_modify_skill_exp_percentage},	//修改蕴魂获得百分比

	{"EnterInstance",		buff_enter_instance},			//进入副本
	{"VisibleToPlayer",     buff_visible_to_player},		//设置是否可以被玩家发现（是否同步给其他玩家）
	{"LargerLevel",			buff_larger_level},				//发送者等级 >= 接受者等级
	{"ExeScript",			buff_exe_script},				//执行脚本
	{"PkPunish",			buff_set_pk_punish},			//设置是否计算PK值。参数一：-1-不计算PK值，1-计算PK值；可以叠加调用；初始默认计算PK值
	{"DeathPunish",			buff_set_death_punish},			//设置是否死亡掉装。参数一：-1-不死亡掉装，1-死亡掉装；可以叠加调用；初始默认死亡掉装
	{"IsWeekDay",			buff_is_weekday},				//是星期几，前置条件。参数一：0~6-星期日~星期六
	{"NotWeekDay",			buff_not_weekday},				//不是星期几，前置条件。参数一：0~6-星期日~星期六
	{"IsInInstance",		buff_is_in_instance},			//是否在副本中，前置条件。
	{"SyncToWorld",			buff_sync_to_world},			//设置世界同步
	{"IsMyEmployer",		buff_is_my_employer},			//判断是否是自己的雇主
	{"ItemExp",				buff_itemexp},					//设置物品存储经验状态
};

//=====================================================================================
int BuffAction::ParseOne( char* szAct, int nLogic )
{
	char szActName[MAX_BUFF_DESC]  = {0};
	int nLogicNot = effect_p_op_none;
	
	if( buff_str::CheckAct( szAct ) )
	{
		if( szAct[0] == NOTDELIMITERC )
		{
			//ClearChar( szAct, NOTDELIMITERC );
			nLogicNot = effect_p_op_not;
			sscanf( 
				szAct+1,
				PATTERN,
				szActName );
		}
		else
		{
			sscanf( 
				szAct,
				PATTERN,
				szActName );
		}

		if( m_Act[m_nCount].pAct = buff_str::FindAct( szActName ) )
		{
			strcpy( szAct, strstr( szAct, FUNLDELIMITER ) + 1 );

			int nPos = strlen(szAct);
			
			nPos ? szAct[nPos-1] = 0 : 0;

			char* szParam = buff_str::FindCloseComma( szAct );

			while( szParam &&
				m_Act[m_nCount].Param( ) < BUFF_MAX_PARAM )
			{
				if( m_Act[m_nCount].DParam = szParam )
				{
					m_Act[m_nCount].Param = 0;
					m_Act[m_nCount].nParamType |= 1 << m_Act[m_nCount].Param( );
				}
				else
				{
					m_Act[m_nCount].Param = ::atoi( szParam );
				}

				szParam = buff_str::FindCloseComma( NULL );
			}
			
			m_Act[m_nCount].nLogic		= nLogic;
			m_Act[m_nCount].nLogicNot	= nLogicNot;

			m_nCount++;
			
			return TRUE;
		}
		else
		{
			#ifdef _DEBUG
			printf( "Buff Action Not Found : %s\n", szAct );
			#endif
		}
	}

	return FALSE;
}

int	BuffAction::ParseAction( char* szAction )
{
	char szAct[MAX_BUFF_DESC]  = {0};
	int nActionLen	=	0;
	int nCurPos		=	0;
	int nCount		=	0;
	char* szFind	=	NULL;
	char* szCurPos	=	NULL;

	Clear( );
	//clear space or tab
	buff_str::ClearChar( szAction, 0x20, 0x09 );
	buff_str::ClearChar( szAction, '\r', '\n' );

	nActionLen	= strlen( szAction );

	while( nCurPos < nActionLen &&
			m_nCount < MAX_BUFFFUN )
	{
		szCurPos = szAction + nCurPos;

		nCount = 
		buff_str::FindDelimter( 
			szCurPos, 
			ANDDELIMITERC, 
			ORDELIMITERC );

		//and
		if( szCurPos[nCount] == ANDDELIMITERC )
		{
			strncpy( szAct, szCurPos, nCount );
			szAct[nCount] = 0;

			ParseOne( szAct, effect_p_op_and );
			
			//skip & |
			nCount++;
			
			nCurPos += nCount;
			continue;
		}

		//or
		if( szCurPos[nCount] == ORDELIMITERC )
		{
			strncpy( szAct, szCurPos, nCount );
			szAct[nCount] = 0;
			
			ParseOne( szAct, effect_p_op_or );
			
			//skip & |
			nCount++;
			
			nCurPos += nCount;
			continue;
		}

		//only one fun
		strcpy( szAct, szCurPos );
		nCount = strlen( szAct );

		ParseOne( szAct, effect_p_op_none );

		//finish
		nCount++;
		
		nCurPos += nCount;
	}

	return TRUE;
}

void BuffAction::CalcDyncParam( 
	int nIndex,
	BUFF_ENV_PARAM& Env )
{
	for( int nLoopCount = 0; nLoopCount < m_Act[nIndex].Param( ); nLoopCount++ )
	{
		if( m_Act[nLoopCount].nParamType & ( 1 << nLoopCount ) )
			m_Act[nLoopCount].Param[nLoopCount] = m_Act[nLoopCount].DParam( nLoopCount, Env );
	}
}

int BuffAction::operator( )( BUFF_ENV_PARAM& Env )
{
	int nRet	= m_nCount > 0 ? FALSE : TRUE;
	int nLogic	= effect_p_op_or;
	
	for( int nLoopCount = 0; nLoopCount < m_nCount; nLoopCount++ )
	{
		if( m_Act[nLoopCount].nParamType )
			CalcDyncParam( nLoopCount, Env );
		
		int nSubRet = 
		m_Act[nLoopCount].pAct( Env, m_Act[nLoopCount].Param );
		
		if( m_Act[nLoopCount].nLogicNot )
			nSubRet = !nSubRet;
		
		if( nLogic == effect_p_op_none )
			break;
		
		if( nLogic == effect_p_op_or )
		{
			if( !nRet )
				nRet = nSubRet;
		}
		
		if( nLogic == effect_p_op_and )
		{
			if( !nRet || !nSubRet )
				nRet = FALSE;
		}
		
		nLogic = m_Act[nLoopCount].nLogic;
	}
	
	return nRet;
}

//=====================================================================================

/*
*
	对应以下枚举请同时添加

	effect_p_type_skillin,
	
	  ..........

	effect_p_type_end
 */

char* g_PE_Type[] =
{
	"none",
	"SkillIn",
	"SkillOut",
	"DamageIn",
	"DamageOut",
	"NpcDeathIn",
	"NpcDeathOut",
	"BuffIn",
	"BuffOut",
	"CreatureDeath",
	"ExplodeOut",
	"Blood",
	"Mana",
	"ChgMap",
	"ExplodeCalc",
	"DelaySkillOut",
	"FinalSkillOut",

	"end"
};

char g_szAddivOp[] =
{
	'\x09',
	'*',
	'/',
	'+',
	'-'
};

BUFF_PARAM g_GlobalVar;
/*
 *	
 */

int buff_str::FindEffectEventType( char* szType )
{
	char szEventType[MAX_BUFF_DESC]  = {0};
	char szFindEventType[MAX_BUFF_DESC]  = {0};
	
	
	strcpy( szEventType, szType );
	buff_str::ConvLowerCase( szEventType );
	
	for( 
		int nLoopCount = 0; 
		nLoopCount < sizeof(g_PE_Type)/sizeof(char*); 
		nLoopCount++ )
	{
		strcpy( szFindEventType, g_PE_Type[nLoopCount] );
		buff_str::ConvLowerCase( szFindEventType );
		
		if( !strcmp( 
			szEventType, 
			szFindEventType ) )
		{
			return nLoopCount;
		}
	}
	
	return BUFF_INVALID;
}

void buff_str::ClearChar( 
	char* szAction, 
	char cOne, 
	char cTwo )
{
	int nPos = 0;
	int nScanPos = 0;
	
	while( szAction[nScanPos] )
	{
		szAction[nPos] = szAction[nScanPos];
		
		if( szAction[nPos] != cOne &&
			szAction[nPos] != cTwo )
			nPos++;
		
		nScanPos++;
	}
	
	szAction[nPos] = 0;
}

int	buff_str::FindDelimter( 
	char* szAction, 
	char cOne, 
	char cTwo )
{
	int nPos = 0;
	
	while( szAction[nPos] )
	{
		if( szAction[nPos] == cOne ||
			szAction[nPos] == cTwo )
			return nPos;
		
		nPos++;
	}
	
	return 0;
}

void buff_str::ConvLowerCase( 
	char* szAction )
{
	int nLoopCount = 0;
	
	while( szAction[nLoopCount] )
	{
		szAction[nLoopCount] = tolower( szAction[nLoopCount] );
		nLoopCount++;
	}
}

int	buff_str::CheckAct( 
	char* szAction )
{
	int nRet	= TRUE;
	int nPos	= 0;
	int nClose	= 0;
	
	while( szAction[nPos] )
	{
		if( szAction[nPos] < 0x21 ||
			szAction[nPos] > 0x7E )
		{
			nRet = 0;
			break;
		}
		
		if( szAction[nPos] == FUNLDELIMITERC )
		{
			nClose++;
			if( szAction[nPos+1] == FUNPDELIMITERC )
			{
				nRet = 0;
				break;
			}
		}
		
		if( szAction[nPos] == FUNRDELIMITERC )
			nClose--;
		
		if( szAction[nPos] == FUNPDELIMITERC &&
			szAction[nPos+1] == FUNRDELIMITERC)
		{
			nRet = 0;
			break;
		}
		
		nPos++;
	}
	
	if( nPos && 
		szAction[nPos-1] != FUNRDELIMITERC)
		nRet = 0;
	
	if( nRet && nClose )
		nRet = FALSE;
	
	#ifdef _DEBUG
	if( !nRet )
		printf( "Buff Check Action Syntax Error : %s\n", szAction );
	#endif

	return nRet;
}

PBUFFACTION	buff_str::FindAct( 
	char* szAction )
{
	char szInAct[MAX_BUFF_DESC]  = {0};
	char szFindAct[MAX_BUFF_DESC]  = {0};
	
	strcpy( szInAct, szAction );
	ConvLowerCase( szInAct );
	
	for( 
		int nLoopCount = 0; 
	nLoopCount < sizeof(g_BuffAction)/sizeof(_Buff_Action); 
	nLoopCount++ )
	{
		strcpy( szFindAct, g_BuffAction[nLoopCount].szName );
		ConvLowerCase( szFindAct );
		
		if( !strcmp( 
			szFindAct, 
			szInAct ) )
			return g_BuffAction[nLoopCount].BuffAction;
	}
	
	return NULL;
}

char* buff_str::FindCloseComma(
	char* szAction )
{
	static char szInBuff[100]  = {0};
	static int nPos	= 0;
	static int nPrevPos = 0;
	int nClose	= 0;
	
	if( szAction )
	{
		strcpy( szInBuff, szAction );
		nPos		= 0;
		nPrevPos	= 0;
	}
	
	nPrevPos = nPos;
	
	while( szInBuff[nPos] )
	{
		if( szInBuff[nPos] == FUNLDELIMITERC )
			nClose++;
		
		if( szInBuff[nPos] == FUNRDELIMITERC )
			nClose--;
		
		if( szInBuff[nPos] == FUNPDELIMITERC &&
			!nClose )
		{
			szInBuff[nPos] = 0;
			nPos++;
			return szInBuff + nPrevPos;
		}
		
		nPos++;
	}
	
	if( szInBuff[nPrevPos] )
		return szInBuff + nPrevPos;
	else
		return 0;
}

char* buff_str::FindAddiOp( 
	char* szAction, 
	int& nAddiOp )
{
	static char szInBuff[100]  = {0};
	static int nPos	= 0;
	static int nPrevPos = 0;
	static int nPrevAddiOp = 0;
	int nClose	= 0;
	
	if( szAction )
	{
		strcpy( szInBuff, szAction );
		nPos		= 0;
		nPrevPos	= 0;
		nPrevAddiOp	= 0;
	}
	
	nPrevPos = nPos;
	
	while( szInBuff[nPos] )
	{
		int nLoopCount = 0;
		while( g_szAddivOp[nLoopCount] )
		{
			if( szInBuff[nPos] == 
				g_szAddivOp[nLoopCount])
			{
				nAddiOp = nPrevAddiOp;
				nPrevAddiOp = nLoopCount;
				szInBuff[nPos] = 0;
				nPos++;
				return szInBuff + nPrevPos;
			}
			
			nLoopCount++;
		}
		
		nPos++;
	}
	
	nAddiOp = nPrevAddiOp;
	
	if( szInBuff[nPrevPos] )
		return szInBuff + nPrevPos;
	else
		return 0;
}

//=====================================================================================
