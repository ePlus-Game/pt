//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-9-1 16:55
//      File_base        : MagicAttribute
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
#include "MagicAttribute.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"

MagicAttrModifier	g_MagicAttrModifier;

MagicAttrModifier::MagicAttrModifier()
{
	for(int i = 0; i < magicattr_end; ++i)
	{
		m_MagicAttrFuncs[i] = NULL;
	}

	// Init needed value here

	// 人物物理，法术攻击
	m_MagicAttrFuncs[add_physdamagelow_v]     = &MagicAttrModifier::AddPhysDamageLow_v;
	m_MagicAttrFuncs[add_physdamagelow_p]     = &MagicAttrModifier::AddPhysDamageLow_p;
	m_MagicAttrFuncs[add_physdamagehight_v]   = &MagicAttrModifier::AddPhysDamageHight_v;
	m_MagicAttrFuncs[add_physdamagehight_p]   = &MagicAttrModifier::AddPhysDamageHight_p;
	m_MagicAttrFuncs[add_magicdamagelow_v]    = &MagicAttrModifier::AddMagicDamageLow_v;
	m_MagicAttrFuncs[add_magicdamagelow_p]    = &MagicAttrModifier::AddMagicDamageLow_p;
	m_MagicAttrFuncs[add_magicdamagehight_v]  = &MagicAttrModifier::AddMagicDamageHight_v;
	m_MagicAttrFuncs[add_magicdamagehight_p]  = &MagicAttrModifier::AddMagicDamageHight_p;

	// 八种攻击
	m_MagicAttrFuncs[add_farphysdamage_v]     = &MagicAttrModifier::AddFarPhysDamage_v;
	m_MagicAttrFuncs[add_farphysdamage_p]     = &MagicAttrModifier::AddFarPhysDamage_p;
	m_MagicAttrFuncs[add_nearphysdamage_v]    = &MagicAttrModifier::AddNearPhysDamage_v;
	m_MagicAttrFuncs[add_nearphysdamage_p]	  = &MagicAttrModifier::AddNearPhysDamage_p;
	m_MagicAttrFuncs[add_waterdamage_v]       = &MagicAttrModifier::AddWaterDamage_v;
	m_MagicAttrFuncs[add_waterdamage_p]		  = &MagicAttrModifier::AddWaterDamage_p;
	m_MagicAttrFuncs[add_firedamage_v]        = &MagicAttrModifier::AddFireDamage_v;
	m_MagicAttrFuncs[add_firedamage_p]        = &MagicAttrModifier::AddFireDamage_p;
	m_MagicAttrFuncs[add_winddamage_v]        = &MagicAttrModifier::AddWindDamage_v;
	m_MagicAttrFuncs[add_winddamage_p]        = &MagicAttrModifier::AddWindDamage_p;
	m_MagicAttrFuncs[add_thunderdamage_v]     = &MagicAttrModifier::AddThunderDamage_v;
	m_MagicAttrFuncs[add_thunderdamage_p]     = &MagicAttrModifier::AddThunderDamage_p;
	m_MagicAttrFuncs[add_shadowdamage_v]      = &MagicAttrModifier::AddShadowDamage_v;
	m_MagicAttrFuncs[add_shadowdamage_p]      = &MagicAttrModifier::AddShadowDamage_p;
	m_MagicAttrFuncs[add_poisondamage_v]      = &MagicAttrModifier::AddPoisonDamage_v;
	m_MagicAttrFuncs[add_poisondamage_p]      = &MagicAttrModifier::AddPoisonDamage_p;

	// 八种抗性
	m_MagicAttrFuncs[add_farphysdefendlow_v]    = &MagicAttrModifier::AddFarPhysDefendLow_v;
	m_MagicAttrFuncs[add_farphysdefendlow_p]    = &MagicAttrModifier::AddFarPhysDefendLow_p;
	m_MagicAttrFuncs[add_farphysdefendhight_v]  = &MagicAttrModifier::AddFarPhysDefendHight_v;
	m_MagicAttrFuncs[add_farphysdefendhight_p]  = &MagicAttrModifier::AddFarPhysDefendHight_p;
	m_MagicAttrFuncs[add_nearphysdefendlow_v]   = &MagicAttrModifier::AddNearPhysDefendLow_v;
	m_MagicAttrFuncs[add_nearphysdefendlow_p]   = &MagicAttrModifier::AddNearPhysDefendLow_p;
	m_MagicAttrFuncs[add_nearphysdefendhight_v] = &MagicAttrModifier::AddNearPhysDefendHight_v;
	m_MagicAttrFuncs[add_nearphysdefendhight_p] = &MagicAttrModifier::AddNearPhysDefendHight_p;
	m_MagicAttrFuncs[add_waterdefendlow_v]      = &MagicAttrModifier::AddWaterDefendLow_v;
	m_MagicAttrFuncs[add_waterdefendlow_p]      = &MagicAttrModifier::AddWaterDefendLow_p;
	m_MagicAttrFuncs[add_waterdefendhight_v]    = &MagicAttrModifier::AddWaterDefendHight_v;
	m_MagicAttrFuncs[add_waterdefendhight_p]    = &MagicAttrModifier::AddWaterDefendHight_p;
	m_MagicAttrFuncs[add_firedefendlow_v]       = &MagicAttrModifier::AddFireDefendLow_v;
	m_MagicAttrFuncs[add_firedefendlow_p]       = &MagicAttrModifier::AddFireDefendLow_p;
	m_MagicAttrFuncs[add_firedefendhight_v]     = &MagicAttrModifier::AddFireDefendHight_v;
	m_MagicAttrFuncs[add_firedefendhight_p]     = &MagicAttrModifier::AddFireDefendHight_p;
	m_MagicAttrFuncs[add_winddefendlow_v]       = &MagicAttrModifier::AddWindDefendLow_v;
	m_MagicAttrFuncs[add_winddefendlow_p]       = &MagicAttrModifier::AddWindDefendLow_p;
	m_MagicAttrFuncs[add_winddefendhight_v]     = &MagicAttrModifier::AddWindDefendHight_v;
	m_MagicAttrFuncs[add_winddefendhight_p]     = &MagicAttrModifier::AddWindDefendHight_p;
	m_MagicAttrFuncs[add_thunderdefendlow_v]    = &MagicAttrModifier::AddThunderDefendLow_v;
	m_MagicAttrFuncs[add_thunderdefendlow_p]    = &MagicAttrModifier::AddThunderDefendLow_p;
	m_MagicAttrFuncs[add_thunderdefendhight_v]  = &MagicAttrModifier::AddThunderDefendHight_v;
	m_MagicAttrFuncs[add_thunderdefendhight_p]  = &MagicAttrModifier::AddThunderDefendHight_p;
	m_MagicAttrFuncs[add_shadowdefendlow_v]	    = &MagicAttrModifier::AddShadowDefendLow_v;
	m_MagicAttrFuncs[add_shadowdefendlow_p]     = &MagicAttrModifier::AddShadowDefendLow_p;
	m_MagicAttrFuncs[add_shadowdefendhight_v]   = &MagicAttrModifier::AddShadowDefendHight_v;
	m_MagicAttrFuncs[add_shadowdefendhight_p]   = &MagicAttrModifier::AddShadowDefendHight_p;
	m_MagicAttrFuncs[add_poisondefendlow_v]     = &MagicAttrModifier::AddPoisonDefendLow_v;
	m_MagicAttrFuncs[add_poisondefendlow_p]     = &MagicAttrModifier::AddPoisonDefendLow_p;
	m_MagicAttrFuncs[add_poisondefendhight_v]   = &MagicAttrModifier::AddPoisonDefendHight_v;
	m_MagicAttrFuncs[add_poisondefendhight_p]   = &MagicAttrModifier::AddPoisonDefendHight_p;

	// 种类抗性
	m_MagicAttrFuncs[add_physdefendlow_v]        = &MagicAttrModifier::AddPhysDefendLow_v;
	m_MagicAttrFuncs[add_physdefendlow_p]        = &MagicAttrModifier::AddPhysDefendLow_p;
	m_MagicAttrFuncs[add_physdefendhight_v]      = &MagicAttrModifier::AddPhysDefendHight_v;
	m_MagicAttrFuncs[add_physdefendhight_p]      = &MagicAttrModifier::AddPhysDefendHight_p;
	m_MagicAttrFuncs[add_eightdiagdefendlow_v]   = &MagicAttrModifier::AddEightDiagDefendLow_v;
	m_MagicAttrFuncs[add_eightdiagdefendlow_p]   = &MagicAttrModifier::AddEightDiagDefendLow_p;
	m_MagicAttrFuncs[add_eightdiagdefendhight_v] = &MagicAttrModifier::AddEightDiagDefendHight_v;
	m_MagicAttrFuncs[add_eightdiagdefendhight_p] = &MagicAttrModifier::AddEightDiagDefendHight_p;
	m_MagicAttrFuncs[add_darkdefendlow_v]        = &MagicAttrModifier::AddDarkDefendLow_v;
	m_MagicAttrFuncs[add_darkdefendlow_p]        = &MagicAttrModifier::AddDarkDefendLow_p;
	m_MagicAttrFuncs[add_darkdefendhight_v]      = &MagicAttrModifier::AddDarkDefendHight_v;
	m_MagicAttrFuncs[add_darkdefendhight_p]      = &MagicAttrModifier::AddDarkDefendHight_p;

	// 人物其他属性
	m_MagicAttrFuncs[add_lifeuplimit_v]    = &MagicAttrModifier::AddLifeUpLimit_v;
	m_MagicAttrFuncs[add_lifeuplimit_p]    = &MagicAttrModifier::AddLifeUpLimit_p;
	m_MagicAttrFuncs[add_manauplimit_v]    = &MagicAttrModifier::AddManaUpLimit_v;
	m_MagicAttrFuncs[add_manauplimit_p]    = &MagicAttrModifier::AddManaUpLimit_p;
	m_MagicAttrFuncs[add_attackspeed_v]    = &MagicAttrModifier::AddAttackSpeed_v;
	m_MagicAttrFuncs[add_attackspeed_p]    = &MagicAttrModifier::AddAttackSpeed_p;
	m_MagicAttrFuncs[add_castspeed_v]      = &MagicAttrModifier::AddCastSpeed_v;
	m_MagicAttrFuncs[add_castspeed_p]      = &MagicAttrModifier::AddCastSpeed_p;
	m_MagicAttrFuncs[add_walkspeed_v]      = &MagicAttrModifier::AddWalkSpeed_v;
	m_MagicAttrFuncs[add_walkspeed_p]      = &MagicAttrModifier::AddWalkSpeed_p;
	m_MagicAttrFuncs[add_runspeed_v]       = &MagicAttrModifier::AddRunSpeed_v;
	m_MagicAttrFuncs[add_runspeed_p]       = &MagicAttrModifier::AddRunSpeed_p;
	m_MagicAttrFuncs[add_dexterity_v]      = &MagicAttrModifier::AddDexterity_v;
	m_MagicAttrFuncs[add_dexterity_p]      = &MagicAttrModifier::AddDexterity_p;
	m_MagicAttrFuncs[add_attackrating_v]   = &MagicAttrModifier::AddAttackRating_v;
	m_MagicAttrFuncs[add_attackrating_p]   = &MagicAttrModifier::AddAttackRating_p;
	m_MagicAttrFuncs[add_curlife_v]	       = &MagicAttrModifier::AddCurLife_v;
	m_MagicAttrFuncs[add_curlife_p]        = &MagicAttrModifier::AddCurLife_p;
	m_MagicAttrFuncs[add_curmana_v]        = &MagicAttrModifier::AddCurMana_v;
	m_MagicAttrFuncs[add_curmana_p]        = &MagicAttrModifier::AddCurMana_p;
	m_MagicAttrFuncs[add_physexplode_v]	   = &MagicAttrModifier::AddPhysExplode_v;
	m_MagicAttrFuncs[add_magicexplode_v]   = &MagicAttrModifier::AddMagicExplode_v;
	m_MagicAttrFuncs[add_body_b]		   = &MagicAttrModifier::AddBody_b;
	m_MagicAttrFuncs[add_body_v]		   = &MagicAttrModifier::AddBody_v;
	m_MagicAttrFuncs[add_body_p]		   = &MagicAttrModifier::AddBody_p;
	m_MagicAttrFuncs[add_nimbus_b]		   = &MagicAttrModifier::AddNimbus_b;
	m_MagicAttrFuncs[add_nimbus_v]		   = &MagicAttrModifier::AddNimbus_v;
	m_MagicAttrFuncs[add_nimbus_p]		   = &MagicAttrModifier::AddNimbus_p;
	m_MagicAttrFuncs[add_strength_b]	   = &MagicAttrModifier::AddStrength_b;
	m_MagicAttrFuncs[add_strength_v]	   = &MagicAttrModifier::AddStrength_v;
	m_MagicAttrFuncs[add_strength_p]	   = &MagicAttrModifier::AddStrength_p;
	m_MagicAttrFuncs[add_art_b]			   = &MagicAttrModifier::AddArt_b;
	m_MagicAttrFuncs[add_art_v]			   = &MagicAttrModifier::AddArt_v;
	m_MagicAttrFuncs[add_art_p]			   = &MagicAttrModifier::AddArt_p;

	// 后续增加
	m_MagicAttrFuncs[add_physdamage_v]	   = &MagicAttrModifier::AddPhysDamage_v;
	m_MagicAttrFuncs[add_physdamage_p]	   = &MagicAttrModifier::AddPhysDamage_p;
	m_MagicAttrFuncs[add_magicdamage_v]	   = &MagicAttrModifier::AddMagicDamage_v;
	m_MagicAttrFuncs[add_magicdamage_p]	   = &MagicAttrModifier::AddMagicDamage_p;

	m_MagicAttrFuncs[add_weightmax_v] = &MagicAttrModifier::AddWeightMax_v;
	m_MagicAttrFuncs[set_killer] = &MagicAttrModifier::SetKiller;
	m_MagicAttrFuncs[add_pkvalue_v] = &MagicAttrModifier::AddPkValue_v;
	m_MagicAttrFuncs[set_death_punish] = &MagicAttrModifier::SetDeathPunish;
	m_MagicAttrFuncs[set_canpickup] = &MagicAttrModifier::SetCanPickup;
	m_MagicAttrFuncs[add_hitrecover_v] = &MagicAttrModifier::AddHitRecover_v;
	m_MagicAttrFuncs[add_hitrecover_p] = &MagicAttrModifier::AddHitRecover_p;

	m_MagicAttrFuncs[add_physdefend_v] = &MagicAttrModifier::AddPhysDefend_v;
	m_MagicAttrFuncs[add_physdefend_p] = &MagicAttrModifier::AddPhysDefend_p;
	m_MagicAttrFuncs[add_eightdiagdefend_v] = &MagicAttrModifier::AddEightDiagDefend_v;
	m_MagicAttrFuncs[add_eightdiagdefend_p] = &MagicAttrModifier::AddEightDiagDefend_p;
	m_MagicAttrFuncs[add_darkdefend_v] = &MagicAttrModifier::AddDarkDefend_v;
	m_MagicAttrFuncs[add_darkdefend_p] = &MagicAttrModifier::AddDarkDefend_p;

	m_MagicAttrFuncs[add_lifebyuplimit_p] = &MagicAttrModifier::AddLifeByUplimit_p;
	m_MagicAttrFuncs[add_manabyuplimit_p] = &MagicAttrModifier::AddManaByUplimit_p;
}

void MagicAttrModifier::ModifyMagicAttr(int nNpcIdx, PMagicData pData)
{
	if(pData && pData->nMagicNo >= 0 && pData->nMagicNo < magicattr_end)
	{
		if(m_MagicAttrFuncs[pData->nMagicNo])
		{
			(this->*m_MagicAttrFuncs[pData->nMagicNo])(nNpcIdx, pData);
		}
	}
}

void MagicAttrModifier::AddPhysDamageLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_physics, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_physics, 1 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddPhysDamageLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_physics, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_physics, 1 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddPhysDamageHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_physics, idx_value_hight, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_physics, 16 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddPhysDamageHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_physics, idx_value_hight, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_physics, 16 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddMagicDamageLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_magic, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_magic, 1 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddMagicDamageLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_magic, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_magic, 1 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddMagicDamageHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_magic, idx_value_hight, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_magic, 16 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddMagicDamageHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_magic, idx_value_hight, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_magic, 16 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddFarPhysDamage_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_farphysics, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_farphysics, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddFarPhysDamage_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_farphysics, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_farphysics, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddNearPhysDamage_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_nearphysics, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_nearphysics, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddNearPhysDamage_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_nearphysics, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_nearphysics, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddWaterDamage_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_water, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_water, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddWaterDamage_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_water, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_water, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddFireDamage_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_fire, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_fire, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddFireDamage_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_fire, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_fire, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddWindDamage_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_wind, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_wind, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddWindDamage_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_wind, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_wind, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddThunderDamage_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_thunder, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_thunder, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddThunderDamage_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_thunder, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_thunder, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddShadowDamage_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_shadow, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_shadow, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddShadowDamage_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_shadow, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_shadow, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddPoisonDamage_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_poison, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_poison, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddPoisonDamage_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_poison, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_poison, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddFarPhysDefendLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_farphysics, idx_value_low, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddFarPhysDefendLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_farphysics, idx_value_low, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddFarPhysDefendHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_farphysics, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddFarPhysDefendHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_farphysics, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddNearPhysDefendLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_nearphysics, idx_value_low, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddNearPhysDefendLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_nearphysics, idx_value_low, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddNearPhysDefendHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_nearphysics, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddNearPhysDefendHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_nearphysics, idx_value_hight, idx_append_percent, pData->nVal);	
}

void MagicAttrModifier::AddWaterDefendLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_water, idx_value_low, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddWaterDefendLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_water, idx_value_low, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddWaterDefendHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_water, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddWaterDefendHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_water, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddFireDefendLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_fire, idx_value_low, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddFireDefendLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_fire, idx_value_low, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddFireDefendHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_fire, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddFireDefendHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_fire, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddWindDefendLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_wind, idx_value_low, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddWindDefendLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_wind, idx_value_low, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddWindDefendHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_wind, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddWindDefendHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_wind, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddThunderDefendLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_thunder, idx_value_low, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddThunderDefendLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_thunder, idx_value_low, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddThunderDefendHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_thunder, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddThunderDefendHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_thunder, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddShadowDefendLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_shadow, idx_value_low, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddShadowDefendLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_shadow, idx_value_low, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddShadowDefendHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_shadow, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddShadowDefendHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_shadow, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddPoisonDefendLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_poison, idx_value_low, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddPoisonDefendLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_poison, idx_value_low, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddPoisonDefendHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_poison, idx_value_hight, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddPoisonDefendHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_poison, idx_value_hight, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddPhysDefendLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_physics, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_physics, 1 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddPhysDefendLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_physics, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_physics, 1 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddPhysDefendHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_physics, idx_value_hight, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_physics, 16 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddPhysDefendHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_physics, idx_value_hight, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_physics, 16 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddEightDiagDefendLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_eightdiag, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_eightdiag, 1 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddEightDiagDefendLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_eightdiag, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_eightdiag, 1 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddEightDiagDefendHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_eightdiag, idx_value_hight, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_eightdiag, 16 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddEightDiagDefendHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_eightdiag, idx_value_hight, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_eightdiag, 16 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddDarkDefendLow_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_dark, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_dark, 1 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddDarkDefendLow_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_dark, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_dark, 1 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddDarkDefendHight_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_dark, idx_value_hight, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_dark, 16 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddDarkDefendHight_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_dark, idx_value_hight, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_dark, 16 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddLifeUpLimit_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_lifeuplimit, idx_append_value, pData->nVal);

	if(Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit] < Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curlife])
	{
		Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curlife, Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit]);
		Npc[nNpcIdx].SyncAttr(npc_attr_unary, nuai_curlife, 1, pData->bBroadCast);
	}

	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_lifeuplimit, 1 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddLifeUpLimit_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_lifeuplimit, idx_append_percent, pData->nVal);

	if(Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit] < Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curlife])
	{
		Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curlife, Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit]);
		Npc[nNpcIdx].SyncAttr(npc_attr_unary, nuai_curlife, 1, pData->bBroadCast);
	}

	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_lifeuplimit, 1 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddManaUpLimit_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_manauplimit, idx_append_value, pData->nVal);

	if(Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit] < Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curmana])
	{
		Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curmana, Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit]);
		Npc[nNpcIdx].SyncAttr(npc_attr_unary, nuai_curmana, 1, pData->bBroadCast);
	}

	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_manauplimit, 1 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddManaUpLimit_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_manauplimit, idx_append_percent, pData->nVal);

	if(Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit] < Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curmana])
	{
		Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curmana, Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit]);
		Npc[nNpcIdx].SyncAttr(npc_attr_unary, nuai_curmana, 1, pData->bBroadCast);
	}

	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_manauplimit, 1 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddAttackSpeed_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_attackspeed, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_attackspeed, 1 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddAttackSpeed_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_attackspeed, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_attackspeed, 1 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddCastSpeed_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_castspeed, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddCastSpeed_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_castspeed, idx_append_percent, pData->nVal);
}

void MagicAttrModifier::AddWalkSpeed_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_walkspeed, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_walkspeed, 1 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddWalkSpeed_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_walkspeed, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_walkspeed, 1 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddRunSpeed_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_runspeed, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_runspeed, 1 << idx_append_value, true);
}

void MagicAttrModifier::AddRunSpeed_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_runspeed, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_runspeed, 1 << idx_append_percent, true);
}

void MagicAttrModifier::AddAttackRating_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_vision, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_vision, 1 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddAttackRating_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_vision, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_vision, 1 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddDexterity_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_dexterity, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_dexterity, 1 << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddDexterity_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_dexterity, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_dexterity, 1 << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddPhysExplode_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_physexplode, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_physexplode, 1 << idx_append_value);
}

void MagicAttrModifier::AddMagicExplode_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_magicexplode, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_magicexplode, 1 << idx_append_value);
}

void MagicAttrModifier::AddCurLife_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddUnaryAttr(nuai_curlife, pData->nVal);

	if(Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curlife] > Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit])
	{
		Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curlife, Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit]);
	}
}

void MagicAttrModifier::AddCurLife_p(int nNpcIdx, PMagicData pData)
{
	int nAddedVal = (int)Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curlife] * pData->nVal / 100;
	Npc[nNpcIdx].AddUnaryAttr(nuai_curlife, nAddedVal);

	if(Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curlife] > Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit])
	{
		Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curlife, Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit]);
	}
}

void MagicAttrModifier::AddCurMana_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddUnaryAttr(nuai_curmana, pData->nVal);

	if(Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curmana] > Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit])
	{
		Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curmana, Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit]);
	}
	else if (Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curmana] < 0)
	{
		Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curmana, 0);
	}
}
void MagicAttrModifier::AddCurMana_p(int nNpcIdx, PMagicData pData)
{
	int nAddedVal = (int)Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curmana] * pData->nVal / 100;
	Npc[nNpcIdx].AddUnaryAttr(nuai_curmana, nAddedVal);

	if(Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curmana] > Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit])
	{
		Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curmana, Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit]);
	}
}

void MagicAttrModifier::AddPhysDamage_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_physics, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_physics, idx_value_hight, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_physics, (1 + 16) << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddPhysDamage_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_physics, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_physics, idx_value_hight, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_physics, (1 + 16) << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddMagicDamage_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_magic, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_magic, idx_value_hight, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_magic, (1 + 16) << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddMagicDamage_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_magic, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_damage_magic, idx_value_hight, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_damage_magic, (1 + 16) << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddBody_b(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_body];

	Npc[nNpcIdx].AddCompAttr(ncai_body, idx_base_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_body, 1 << idx_base_value, pData->bBroadCast);
	
	Npc[nNpcIdx].UpdateBodyEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddBody_v(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_body];

	Npc[nNpcIdx].AddCompAttr(ncai_body, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_body, 1 << idx_append_value, pData->bBroadCast);

	Npc[nNpcIdx].UpdateBodyEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddBody_p(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_body];

	Npc[nNpcIdx].AddCompAttr(ncai_body, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_body, 1 << idx_append_percent, pData->bBroadCast);

	Npc[nNpcIdx].UpdateBodyEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddNimbus_b(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_nimbus];

	Npc[nNpcIdx].AddCompAttr(ncai_nimbus, idx_base_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_nimbus, 1 << idx_base_value, pData->bBroadCast);

	Npc[nNpcIdx].UpdateNimbusEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddNimbus_v(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_nimbus];

	Npc[nNpcIdx].AddCompAttr(ncai_nimbus, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_nimbus, 1 << idx_append_value, pData->bBroadCast);

	Npc[nNpcIdx].UpdateNimbusEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddNimbus_p(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_nimbus];

	Npc[nNpcIdx].AddCompAttr(ncai_nimbus, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_nimbus, 1 << idx_append_percent, pData->bBroadCast);

	Npc[nNpcIdx].UpdateNimbusEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddStrength_b(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_strength];

	Npc[nNpcIdx].AddCompAttr(ncai_strength, idx_base_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_strength, 1 << idx_base_value, pData->bBroadCast);

	Npc[nNpcIdx].UpdateStrengthEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddStrength_v(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_strength];

	Npc[nNpcIdx].AddCompAttr(ncai_strength, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_strength, 1 << idx_append_value, pData->bBroadCast);

	Npc[nNpcIdx].UpdateStrengthEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddStrength_p(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_strength];

	Npc[nNpcIdx].AddCompAttr(ncai_strength, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_strength, 1 << idx_append_percent, pData->bBroadCast);

	Npc[nNpcIdx].UpdateStrengthEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddArt_b(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_art];

	Npc[nNpcIdx].AddCompAttr(ncai_art, idx_base_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_art, 1 << idx_base_value, pData->bBroadCast);

	Npc[nNpcIdx].UpdateArtEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddArt_v(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_art];

	Npc[nNpcIdx].AddCompAttr(ncai_art, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_art, 1 << idx_append_value, pData->bBroadCast);

	Npc[nNpcIdx].UpdateArtEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddArt_p(int nNpcIdx, PMagicData pData)
{
	int nOldVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_art];

	Npc[nNpcIdx].AddCompAttr(ncai_art, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_comp, ncai_art, 1 << idx_append_percent, pData->bBroadCast);

	Npc[nNpcIdx].UpdateArtEffect(nOldVal, pData->nVal, true, pData->bBroadCast);
}

void MagicAttrModifier::AddWeightMax_v(int nNpcIdx, PMagicData pData)
{
	if (Npc[nNpcIdx].IsPlayer())
	{
		KPlayer& player = Player[Npc[nNpcIdx].GetPlayerIdx()];
		player.SetWeightMax(player.GetWeightMax() + pData->nVal);
		player.SyncAttribute(attr_WeightMax);
	}
}

void MagicAttrModifier::SetKiller(int nNpcIdx, PMagicData pData)
{
	if (Npc[nNpcIdx].IsPlayer())
	{
		KPlayer& player = Player[Npc[nNpcIdx].GetPlayerIdx()];
		player.SetKiller(((1 == pData->nVal) ? true : false));		
	}	
}

void MagicAttrModifier::AddPkValue_v(int nNpcIdx, PMagicData pData)
{
	if (pData != NULL && IsValidNpc(nNpcIdx))
	{
		if (Npc[nNpcIdx].IsPlayer())
		{
			KPlayer& player = Player[Npc[nNpcIdx].GetPlayerIdx()];
			if (pData->nVal < 0)
			{
				int subWorldIndex = Npc[nNpcIdx].GetSubWorldIndex();
				if (subWorldIndex >= 0 && subWorldIndex < MAX_SUBWORLD)
				{
					int worldTemplateId = SubWorld[subWorldIndex].GetWorldTemplateId();
					WorldSetting * pSetting = g_SubWorldSet.GetWorldSetting(worldTemplateId);

					int PKReduceMutiple = 1;
					if (pSetting != NULL)
					{
						PKReduceMutiple = pSetting->m_nPKReduceMultiple;
					}
					if (PKReduceMutiple < 1)
					{
						PKReduceMutiple = 1;
					}
					int PKValue = player.GetPkValue() + pData->nVal * PKReduceMutiple;
					player.SetPkValue(PKValue);
				}
			}
			else
			{
				player.SetPkValue(player.GetPkValue() + pData->nVal);
			}
		}
	}
}

void MagicAttrModifier::SetDeathPunish(int nNpcIdx, PMagicData pData)
{
	if (Npc[nNpcIdx].IsPlayer())
	{
		KPlayer& player = Player[Npc[nNpcIdx].GetPlayerIdx()];
		player.SetDeathPunish(pData->nVal);
	}	
}

void MagicAttrModifier::SetCanPickup(int nNpcIdx, PMagicData pData)
{
	if (Npc[nNpcIdx].IsPlayer())
	{
		KPlayer& player = Player[Npc[nNpcIdx].GetPlayerIdx()];
		player.SetCanPickup(((TRUE == pData->nVal) ? true : false));
		player.SyncAttribute(attr_CanPickup);
	}	
}

void MagicAttrModifier::AddHitRecover_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_hitrecover, idx_append_value, pData->nVal);
}

void MagicAttrModifier::AddHitRecover_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddCompAttr(ncai_hitrecover, idx_append_percent, pData->nVal);	
}

void MagicAttrModifier::AddPhysDefend_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_physics, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_physics, idx_value_hight, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_physics, (1 + 16) << idx_append_value, pData->bBroadCast);	
}

void MagicAttrModifier::AddPhysDefend_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_physics, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_physics, idx_value_hight, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_physics, (1 + 16) << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddEightDiagDefend_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_eightdiag, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_eightdiag, idx_value_hight, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_eightdiag, (1 + 16) << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddEightDiagDefend_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_eightdiag, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_eightdiag, idx_value_hight, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_eightdiag, (1 + 16) << idx_append_percent, pData->bBroadCast);
}

void MagicAttrModifier::AddDarkDefend_v(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_dark, idx_value_low, idx_append_value, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_dark, idx_value_hight, idx_append_value, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_dark, (1 + 16) << idx_append_value, pData->bBroadCast);
}

void MagicAttrModifier::AddDarkDefend_p(int nNpcIdx, PMagicData pData)
{
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_dark, idx_value_low, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].AddRangeAttr(nrai_defend_dark, idx_value_hight, idx_append_percent, pData->nVal);
	Npc[nNpcIdx].SyncAttr(npc_attr_range, nrai_defend_dark, (1 + 16) << idx_append_percent, pData->bBroadCast);	
}

void MagicAttrModifier::AddLifeByUplimit_p(int nNpcIdx, PMagicData pData)
{
	int nAddVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit] * pData->nVal / 100;
	int nNewLife = Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curlife] + nAddVal;

	if(nNewLife < 0)
		nNewLife = 0;
	else if(nNewLife > Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit])
		nNewLife = Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit];

	Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curlife, nNewLife);
}

void MagicAttrModifier::AddManaByUplimit_p(int nNpcIdx, PMagicData pData)
{
	int nAddVal = Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit] * pData->nVal / 100;
	int nNewMana = Npc[nNpcIdx].m_UnaryAttrMgr[nuai_curmana] + nAddVal;

	if(nNewMana < 0)
		nNewMana = 0;
	else if(nNewMana > Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit])
		nNewMana = Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit];

	Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curmana, nNewMana);
}