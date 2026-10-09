//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-9-1 16:22
//      File_base        : MagicAttribute
//      File_ext         : h
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
#ifndef _MagicAttribute_h
#define _MagicAttribute_h

#include "MagicDef.h"

// Define magic attributes used by boff and skill
// Suffix meaning: 
// b	base   value, add value permanently，can't be clear
// v	append value
// p	append percent

// Note: Don't change the order of these attributes.
//       Add new attribute on the last.

enum enMagicAttrNo
{
	attr_no_invalid,
	add_lifeuplimit_b,					// 加生命上限的基础数值
	add_lifeuplimit_v,					// 加生命上限，数值
	add_lifeuplimit_p,					// 加生命上限，百分比
	add_curlife_v,						// 增加当前生命，数值
	add_manauplimit_b,					// 增加法力上限的基础数值
	add_manauplimit_v,					// 增加法力上限，数值
	add_manauplimit_p,					// 增加法力上限，百分比
	add_curmana_v,						// 增加当前法力
	add_liferenewspeed_b,				// 增加生命回复速度的基础数值
	add_liferenewspeed_v,				// 增加生命恢复速度，数值
	add_liferenewspeed_p,				// 增加生命恢复速度，百分比
	add_manarenewspeed_b,				// 增加法力恢复速度的基础数值
	add_manarenewspeed_v,				// 增加法力恢复速度，数值		
	add_manarenewspeed_p,				// 增加法力恢复速度，百分比
	add_body_b,							// 增加体质基础数值
	add_body_v,							// 增加体质，数值
	add_body_p,							// 增加体质，百分比
	add_nimbus_b,						// 增加法力基础数值
	add_nimbus_v,						// 增加灵气，数值
	add_nimbus_p,						// 增加灵气，百分比
	add_strength_b,						// 增加力量基础数值
	add_strength_v,						// 增加力量，数值
	add_strength_p,						// 增加力量，百分比
	add_art_b,							// 增加法术基础数值
	add_art_v,							// 增加法术，数值
	add_art_p,							// 增加法术，百分比
	add_attackrating_v,					// 增加命中率
	add_dexterity_v,					// 增加闪避率
	add_physexplode_v,					// 增加物理爆击率(注意爆击率对精度要求比较高，这里的分母是1000)
	add_magicexplode_v,					// 增加法术爆击率(分母是1000)
	add_walkspeed_v,					// 增加走路速度，数值
	add_walkspeed_p,					// 增加走路速度，百分比
	add_runspeed_v,						// 增加跑步速度，数值
	add_runspeed_p,						// 增加跑步速度，百分比
	add_attackspeed_v,					// 增加攻击速度，数值
	add_attackspeed_p,					// 增加攻击速度，百分比
	add_castspeed_v,					// 增加施法速度，数值
	add_castspeed_p,					// 增加施法速度，百分比
	add_attackrating_p,					// 增加命中率，百分比
	add_dexterity_p,					// 增加闪避率，百分比
	add_curlife_p,						// 增加当前生命，百分比
	add_curmana_p,						// 增加当前法力，百分比

	// 人物物理，法术攻击
	add_physdamagelow_v,				// 增加人物的物理攻击下限，数值
	add_physdamagelow_p,				// 增加人物的物理攻击下限，百分比
	add_physdamagehight_v,				// ...上限
	add_physdamagehight_p,				// ...上限
	add_magicdamagelow_v,				// 增加人物的法术攻击下限，数值
	add_magicdamagelow_p,				// 增加人物的法术攻击下限，百分比
	add_magicdamagehight_v,				// ...上限
	add_magicdamagehight_p,				// ...上限

	// 八种攻击
	add_farphysdamage_v,				// 增加远程物理攻击，数值
	add_farphysdamage_p,				// 增加远程物理攻击，百分比 
	add_nearphysdamage_v,				// 增加近程物理攻击，数值
	add_nearphysdamage_p,				// 增加近程物理攻击，百分比
	add_waterdamage_v,					// 增加水攻击，数值
	add_waterdamage_p,					// 增加水攻击，百分比
	add_firedamage_v,					// 增加火攻击，数值
	add_firedamage_p,					// 增加火攻击，百分比
	add_winddamage_v,					// 增加风攻击，数值
	add_winddamage_p,					// 增加风攻击，百分比
	add_thunderdamage_v,				// 增加雷攻击，数值
	add_thunderdamage_p,				// 增加雷攻击，百分比
	add_shadowdamage_v,					// 增加阴攻击，数值
	add_shadowdamage_p,					// 增加阴攻击，百分比
	add_poisondamage_v,					// 增加毒攻击，数值
	add_poisondamage_p,					// 增加毒攻击，百分比

	// 八种抗性
	add_farphysdefendlow_v,				// 增加远程物理抗性下限，数值
	add_farphysdefendlow_p,				// 增加远程物理抗性下限，百分比
	add_farphysdefendhight_v,			// 增加远程物理抗性上限，数值
	add_farphysdefendhight_p,			// 增加远程物理抗性上限，百分比
	add_nearphysdefendlow_v,			// 增加近程物理抗性下限，数值
	add_nearphysdefendlow_p,			// 增加近程物理抗性下限，百分比
	add_nearphysdefendhight_v,			// 增加近程物理抗性上限，数值
	add_nearphysdefendhight_p,			// 增加近程物理抗性上限，百分比
	add_waterdefendlow_v,				// 增加水抗性下限，数值
	add_waterdefendlow_p,				// 增加水抗性下限，百分比
	add_waterdefendhight_v,				// 增加水抗性上限，数值
	add_waterdefendhight_p,				// 增加水抗性上限，百分比
	add_firedefendlow_v,				// 增加火炕性下限，数值
	add_firedefendlow_p,				// 增加火炕性下限，百分比
	add_firedefendhight_v,				// 增加火炕性上限，数值
	add_firedefendhight_p,				// 增加火炕性上限，百分比
	add_winddefendlow_v,				// 增加风抗性下限，数值
	add_winddefendlow_p,				// 增加风抗性下限，百分比
	add_winddefendhight_v,				// 增加风抗性上限，数值
	add_winddefendhight_p,				// 增加风抗性上限，百分比
	add_thunderdefendlow_v,				// 增加雷抗性下限，数值
	add_thunderdefendlow_p,				// 增加雷抗性下限，百分比
	add_thunderdefendhight_v,			// 增加雷抗性上限，数值
	add_thunderdefendhight_p,			// 增加雷抗性上限，百分比
	add_shadowdefendlow_v,				// 增加阴抗性下限，数值
	add_shadowdefendlow_p,				// 增加阴抗性下限，百分比
	add_shadowdefendhight_v,			// 增加阴抗性上限，数值
	add_shadowdefendhight_p,			// 增加阴抗性上限，百分比
	add_poisondefendlow_v,				// 增加毒抗性下限，数值
	add_poisondefendlow_p,				// 增加毒抗性下限，百分比
	add_poisondefendhight_v,			// 增加毒抗性上限，数值
	add_poisondefendhight_p,			// 增加毒抗性上限，百分比

	// 种类抗性
	add_physdefendlow_v,				// 增加物理抗性下限，数值
	add_physdefendlow_p,				// 增加物理抗性下限，百分比
	add_physdefendhight_v,				// 增加物理抗性上限，数值
	add_physdefendhight_p,				// 增加物理抗性上限，百分比
	add_eightdiagdefendlow_v,			// 增加八卦抗性下限，数值
	add_eightdiagdefendlow_p,			// 增加八卦抗性下限，百分比
	add_eightdiagdefendhight_v,			// 增加八卦抗性上限，数值
	add_eightdiagdefendhight_p,			// 增加八卦抗性上限，百分比
	add_darkdefendlow_v,				// 增加玄冥抗性下限，数值
	add_darkdefendlow_p,				// 增加玄冥抗性下限，百分比
	add_darkdefendhight_v,				// 增加玄冥抗性上限，数值
	add_darkdefendhight_p,				// 增加玄冥抗性上限，百分比

	// 后续增加
	add_physdamage_v,					// 增加物理攻击，数值，上下限都加
	add_physdamage_p,					// 增加物理攻击，百分比，上下限都加
	add_magicdamage_v,					// 增加法术攻击，数值，上下限都加
	add_magicdamage_p,					// 增加法术攻击，百分比，上下限都加

	// Add new magic number here
	add_weightmax_v,					// 增加负重上限
	set_killer,							// 设置是否为“杀手”
	add_pkvalue_v,						// 增加PK值，数值
	set_death_punish,					// 设置是否死亡惩罚
	set_canpickup,						// 设置是否能拾取物品
	add_hitrecover_v,					// 增加受伤回复速度，附加数值
	add_hitrecover_p,					// 增加受伤回复速度，百分比

	add_physdefend_v,					// 增加物理抗性上限及下限，数值
	add_physdefend_p,					// 增加物理抗性上线及下限，百分比
	add_eightdiagdefend_v,				// 增加八卦抗性上限及下限，数值
	add_eightdiagdefend_p,				// 增加八卦抗性上限及下限，百分比
	add_darkdefend_v,					// 增加玄冥抗性的上限及下限，数值
	add_darkdefend_p,					// 增加玄冥抗性的上限及下限，百分比

	add_lifebyuplimit_p,				// 按照当前生命上限的百分比来增加生命
	add_manabyuplimit_p,				// 按照当前法力上限的百分比来增加法力
	
	magicattr_end
	
};

class MagicAttrModifier
{
public:
	MagicAttrModifier();
	
	void ModifyMagicAttr(int nNpcIdx, PMagicData pData);

private:
	// 物理攻击，魔法攻击
	void	AddPhysDamageLow_v(int nNpcIdx, PMagicData pData);
	void	AddPhysDamageLow_p(int nNpcIdx, PMagicData pData);
	void	AddPhysDamageHight_v(int nNpcIdx, PMagicData pData);
	void	AddPhysDamageHight_p(int nNpcIdx, PMagicData pData);
	void	AddMagicDamageLow_v(int nNpcIdx, PMagicData pData);
	void	AddMagicDamageLow_p(int nNpcIdx, PMagicData pData);
	void	AddMagicDamageHight_v(int nNpcIdx, PMagicData pData);
	void	AddMagicDamageHight_p(int nNpcIdx, PMagicData pData);

	// 八种攻击
	void	AddFarPhysDamage_v(int nNpcIdx, PMagicData pData);
	void	AddFarPhysDamage_p(int nNpcIdx, PMagicData pData);
	void	AddNearPhysDamage_v(int nNpcIdx, PMagicData pData);
	void	AddNearPhysDamage_p(int nNpcIdx, PMagicData pData);
	void	AddWaterDamage_v(int nNpcIdx, PMagicData pData);
	void	AddWaterDamage_p(int nNpcIdx, PMagicData pData);
	void	AddFireDamage_v(int nNpcIdx, PMagicData pData);
	void	AddFireDamage_p(int nNpcIdx, PMagicData pData);
	void	AddWindDamage_v(int nNpcIdx, PMagicData pData);
	void	AddWindDamage_p(int nNpcIdx, PMagicData pData);
	void	AddThunderDamage_v(int nNpcIdx, PMagicData pData);
	void	AddThunderDamage_p(int nNpcIdx, PMagicData pData);
	void	AddShadowDamage_v(int nNpcIdx, PMagicData pData);
	void	AddShadowDamage_p(int nNpcIdx, PMagicData pData);
	void	AddPoisonDamage_v(int nNpcIdx, PMagicData pData);
	void	AddPoisonDamage_p(int nNpcIdx, PMagicData pData);
	
	// 八种抗性
	void	AddFarPhysDefendLow_v(int nNpcIdx, PMagicData pData);
	void	AddFarPhysDefendLow_p(int nNpcIdx, PMagicData pData);
	void	AddFarPhysDefendHight_v(int nNpcIdx, PMagicData pData);
	void	AddFarPhysDefendHight_p(int nNpcIdx, PMagicData pData);
	void	AddNearPhysDefendLow_v(int nNpcIdx, PMagicData pData);
	void	AddNearPhysDefendLow_p(int nNpcIdx, PMagicData pData);
	void	AddNearPhysDefendHight_v(int nNpcIdx, PMagicData pData);
	void	AddNearPhysDefendHight_p(int nNpcIdx, PMagicData pData);
	void	AddWaterDefendLow_v(int nNpcIdx, PMagicData pData);
	void	AddWaterDefendLow_p(int nNpcIdx, PMagicData pData);
	void	AddWaterDefendHight_v(int nNpcIdx, PMagicData pData);
	void	AddWaterDefendHight_p(int nNpcIdx, PMagicData pData);
	void	AddFireDefendLow_v(int nNpcIdx, PMagicData pData);
	void	AddFireDefendLow_p(int nNpcIdx, PMagicData pData);
	void	AddFireDefendHight_v(int nNpcIdx, PMagicData pData);
	void	AddFireDefendHight_p(int nNpcIdx, PMagicData pData);
	void	AddWindDefendLow_v(int nNpcIdx, PMagicData pData);
	void	AddWindDefendLow_p(int nNpcIdx, PMagicData pData);
	void	AddWindDefendHight_v(int nNpcIdx, PMagicData pData);
	void	AddWindDefendHight_p(int nNpcIdx, PMagicData pData);
	void	AddThunderDefendLow_v(int nNpcIdx, PMagicData pData);
	void	AddThunderDefendLow_p(int nNpcIdx, PMagicData pData);
	void	AddThunderDefendHight_v(int nNpcIdx, PMagicData pData);
	void	AddThunderDefendHight_p(int nNpcIdx, PMagicData pData);
	void	AddShadowDefendLow_v(int nNpcIdx, PMagicData pData);
	void	AddShadowDefendLow_p(int nNpcIdx, PMagicData pData);
	void	AddShadowDefendHight_v(int nNpcIdx, PMagicData pData);
	void	AddShadowDefendHight_p(int nNpcIdx, PMagicData pData);
	void	AddPoisonDefendLow_v(int nNpcIdx, PMagicData pData);
	void	AddPoisonDefendLow_p(int nNpcIdx, PMagicData pData);
	void	AddPoisonDefendHight_v(int nNpcIdx, PMagicData pData);
	void	AddPoisonDefendHight_p(int nNpcIdx, PMagicData pData);

	// 种类抗性
	void	AddPhysDefendLow_v(int nNpcIdx, PMagicData pData);
	void	AddPhysDefendLow_p(int nNpcIdx, PMagicData pData);
	void	AddPhysDefendHight_v(int nNpcIdx, PMagicData pData);
	void	AddPhysDefendHight_p(int nNpcIdx, PMagicData pData);
	void	AddEightDiagDefendLow_v(int nNpcIdx, PMagicData pData);
	void	AddEightDiagDefendLow_p(int nNpcIdx, PMagicData pData);
	void	AddEightDiagDefendHight_v(int nNpcIdx, PMagicData pData);
	void	AddEightDiagDefendHight_p(int nNpcIdx, PMagicData pData);
	void	AddDarkDefendLow_v(int nNpcIdx, PMagicData pData);
	void	AddDarkDefendLow_p(int nNpcIdx, PMagicData pData);
	void	AddDarkDefendHight_v(int nNpcIdx, PMagicData pData);
	void	AddDarkDefendHight_p(int nNpcIdx, PMagicData pData);
	
	// 人物其他属性
	void	AddLifeUpLimit_v(int nNpcIdx, PMagicData pData);
	void	AddLifeUpLimit_p(int nNpcIdx, PMagicData pData);
	void	AddManaUpLimit_v(int nNpcIdx, PMagicData pData);
	void	AddManaUpLimit_p(int nNpcIdx, PMagicData pData);
	void	AddAttackSpeed_v(int nNpcIdx, PMagicData pData);
	void	AddAttackSpeed_p(int nNpcIdx, PMagicData pData);
	void	AddCastSpeed_v(int nNpcIdx, PMagicData pData);
	void	AddCastSpeed_p(int nNpcIdx, PMagicData pData);
	void	AddWalkSpeed_v(int nNpcIdx, PMagicData pData);
	void	AddWalkSpeed_p(int nNpcIdx, PMagicData pData);
	void	AddRunSpeed_v(int nNpcIdx, PMagicData pData);
	void	AddRunSpeed_p(int nNpcIdx, PMagicData pData);
	void	AddAttackRating_v(int nNpcIdx, PMagicData pData);
	void	AddAttackRating_p(int nNpcIdx, PMagicData pData);
	void	AddDexterity_v(int nNpcIdx, PMagicData pData);
	void	AddDexterity_p(int nNpcIdx, PMagicData pData);
	void	AddDeadAttack_v(int nNpcIdx, PMagicData pData);
	void	AddDeadAttack_p(int nNpcIdx, PMagicData pData);
	void	AddCurLife_v(int nNpcIdx, PMagicData pData);
	void	AddCurLife_p(int nNpcIdx, PMagicData pData);
	void	AddCurMana_v(int nNpcIdx, PMagicData pData);
	void	AddCurMana_p(int nNpcIdx, PMagicData pData);
	void	AddPhysExplode_v(int nNpcIdx, PMagicData pData);
	void	AddMagicExplode_v(int nNpcIdx, PMagicData pData);
	void	AddBody_b(int nNpcIdx, PMagicData pData);
	void	AddBody_v(int nNpcIdx, PMagicData pData);
	void	AddBody_p(int nNpcIdx, PMagicData pData);
	void	AddNimbus_b(int nNpcIdx, PMagicData pData);
	void	AddNimbus_v(int nNpcIdx, PMagicData pData);
	void	AddNimbus_p(int nNpcIdx, PMagicData pData);
	void	AddStrength_b(int nNpcIdx, PMagicData pData);
	void	AddStrength_v(int nNpcIdx, PMagicData pData);
	void	AddStrength_p(int nNpcIdx, PMagicData pData);
	void	AddArt_b(int nNpcIdx, PMagicData pData);
	void	AddArt_v(int nNpcIdx, PMagicData pData);
	void	AddArt_p(int nNpcIdx, PMagicData pData);

	// 后续增加
	void	AddPhysDamage_v(int nNpcIdx, PMagicData pData);
	void	AddPhysDamage_p(int nNpcIdx, PMagicData pData);
	void	AddMagicDamage_v(int nNpcIdx, PMagicData pData);
	void	AddMagicDamage_p(int nNpcIdx, PMagicData pData);
	
	void	AddWeightMax_v(int nNpcIdx, PMagicData pData);
	void	SetKiller(int nNpcIdx, PMagicData pData);
	void	AddPkValue_v(int nNpcIdx, PMagicData pData);
	void	SetDeathPunish(int nNpcIdx, PMagicData pData);
	void	SetCanPickup(int nNpcIdx, PMagicData pData);
	void	AddHitRecover_v(int nNpcIdx, PMagicData pData);
	void	AddHitRecover_p(int nNpcIdx, PMagicData pData);

	void	AddPhysDefend_v(int nNpcIdx, PMagicData pData);
	void	AddPhysDefend_p(int nNpcIdx, PMagicData pData);
	void	AddEightDiagDefend_v(int nNpcIdx, PMagicData pData);
	void	AddEightDiagDefend_p(int nNpcIdx, PMagicData pData);
	void	AddDarkDefend_v(int nNpcIdx, PMagicData pData);
	void	AddDarkDefend_p(int nNpcIdx, PMagicData pData);

	void	AddLifeByUplimit_p(int nNpcIdx, PMagicData pData);
	void	AddManaByUplimit_p(int nNpcIdx, PMagicData pData);

private:
	typedef	void (MagicAttrModifier::*PATTRFUNC)(int nNpcIdx, PMagicData pData);
	
	PATTRFUNC	m_MagicAttrFuncs[magicattr_end];
};

extern MagicAttrModifier	g_MagicAttrModifier;

#endif