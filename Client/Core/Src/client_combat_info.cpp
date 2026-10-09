//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-3-9
//      File_base        : client_combat_info
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 战斗信息（客户端）
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "ChatCenter_C.h"
#include "KSkills.h"
#include "ConfigManager.h"
#include "client_combat_info.h"

#define DAMAGE_INFO_TOTAL_FRAME 100//伤害信息最大总桢数
#define MAX_DAMAGE_INFO_TEXT_WIDTH 50
#define DAMAGE_INFO_TEXT_CHAR_WIDTH 10
#define DAMAGE_INFO_TEXT_HEIGHT 30
#define DAMAGE_INFO_TEXT_RANDOM_X 150
#define DAMAGE_INFO_TEXT_RANDOM_Y 100
#define DAMAGE_INFO_TEXT_BASIC_OFFSET_Y 200
#define COMBATINFO_NORMAL_CONFIG_FILE "\\settings\\combatinfo_normal.txt"
#define COMBATINFO_CRIT_CONFIG_FILE "\\settings\\combatinfo_crit.txt"

struct NormalDamageFrameInfo
{
	int PosX;
	int PosY;
	int Alpha;
};

struct CritDamageFrameBasicInfo
{
	int PosX;
	int PosY;
	int Alpha;
	int Frame;
};

extern iRepresentShell * g_pRepresentShell;
static KRUImage s_NormalImage;
static KRUImage s_HealImage;
static KRUImage s_DamageImage;
static KRUImage	s_CritDamageImage[10];
static KRUImage	s_CritLabelImage;
static KRUImage	s_MissImage;
static KRUImage	s_DodgeImage;
static KRUImage	s_AbsorbImage;
static KRUImage s_ScoreImage;

static int s_CritDamageTextWidth[3] = { 10, 13, 16 };
static int s_NormalDamageFrameCount = 0;
static NormalDamageFrameInfo s_NormalDamageFrameInfos[DAMAGE_INFO_TOTAL_FRAME] = { 0 };
static int s_CritDamageFrameCount = 0;
static CritDamageFrameBasicInfo s_CritDamageFrameBasicInfos[DAMAGE_INFO_TOTAL_FRAME] = { 0 };

static char s_CombatInfoUnknownTarget[256];
static char s_CombatInfoUnknownSkill[256];
static char s_CombatInfoTargetYou[256];
static char s_CombatInfoDamageCritical[256];
static char s_CombatInfoHealCritical[256];
static char s_CombatInfoDamageLifeText[4][256];
static char s_CombatInfoDamageManaText[4][256];
static char s_CombatInfoHealLifeText[4][256];
static char s_CombatInfoHealManaText[4][256];
static char s_CombatInfoDodgeText[4][256];
static char s_CombatInfoAbsorbLifeText[4][256];
static char s_CombatInfoAbsorbManaText[4][256];
static char s_CombatInfoScoreGet[256];
static char s_CombatInfoBasicMsgText[256];
static char s_CombatInfoSkillNameTemplate[256];
static char s_CombatInfoCasterNameTemplate[256];
static char s_CombatInfoTargetNameTemplate[256];

void InitCombatInfoResources()
{	
	s_CombatInfoUnknownTarget[0] = 0;
	s_CombatInfoUnknownSkill[0] = 0;
	s_CombatInfoTargetYou[0] = 0;
	s_CombatInfoDamageCritical[0] = 0;
	s_CombatInfoHealCritical[0] = 0;
	
	memset(s_CombatInfoDamageLifeText, 0, sizeof(s_CombatInfoDamageLifeText));
	memset(s_CombatInfoDamageManaText, 0, sizeof(s_CombatInfoDamageManaText));
	memset(s_CombatInfoHealLifeText, 0, sizeof(s_CombatInfoHealLifeText));
	memset(s_CombatInfoHealManaText, 0, sizeof(s_CombatInfoHealManaText));
	memset(s_CombatInfoDodgeText, 0, sizeof(s_CombatInfoDodgeText));
	memset(s_CombatInfoAbsorbLifeText, 0, sizeof(s_CombatInfoAbsorbLifeText));
	memset(s_CombatInfoAbsorbManaText, 0, sizeof(s_CombatInfoAbsorbManaText));
	memset(s_CombatInfoScoreGet,0,sizeof(s_CombatInfoScoreGet));	
	
	s_CombatInfoBasicMsgText[0] = 0;	
	s_CombatInfoSkillNameTemplate[0] = 0;
	s_CombatInfoCasterNameTemplate[0] = 0;
	s_CombatInfoTargetNameTemplate[0] = 0;
	s_CombatInfoScoreGet[0]= 0 ;

	g_GetStringRes(sid_unknown_target , s_CombatInfoUnknownTarget, 256);
	g_GetStringRes(sid_unknown_skill , s_CombatInfoUnknownSkill, 256);
	g_GetStringRes(sid_target_you , s_CombatInfoTargetYou, 256);
	g_GetStringRes(sid_damage_critical , s_CombatInfoDamageCritical, 256);
	g_GetStringRes(sid_heal_critical , s_CombatInfoHealCritical, 256);

	g_GetStringRes(sid_damage_self_life , s_CombatInfoDamageLifeText[0], 256);
	g_GetStringRes(sid_damage_target_life , s_CombatInfoDamageLifeText[1], 256);
	g_GetStringRes(sid_damage_self_life_critical , s_CombatInfoDamageLifeText[2], 256);
	g_GetStringRes(sid_damage_target_life_critical , s_CombatInfoDamageLifeText[3], 256);

	g_GetStringRes(sid_damage_self_mana , s_CombatInfoDamageManaText[0], 256);
	g_GetStringRes(sid_damage_target_mana , s_CombatInfoDamageManaText[1], 256);
	g_GetStringRes(sid_damage_self_mana_critical , s_CombatInfoDamageManaText[2], 256);
	g_GetStringRes(sid_damage_target_mana_critical , s_CombatInfoDamageManaText[3], 256);

	g_GetStringRes(sid_heal_self_life , s_CombatInfoHealLifeText[0], 256);
	g_GetStringRes(sid_heal_target_life , s_CombatInfoHealLifeText[1], 256);
	g_GetStringRes(sid_heal_self_life_critical , s_CombatInfoHealLifeText[2], 256);
	g_GetStringRes(sid_heal_target_life_critical , s_CombatInfoHealLifeText[3], 256);

	g_GetStringRes(sid_heal_self_mana , s_CombatInfoHealManaText[0], 256);
	g_GetStringRes(sid_heal_target_mana , s_CombatInfoHealManaText[1], 256);
	g_GetStringRes(sid_heal_self_mana_critical , s_CombatInfoHealManaText[2], 256);
	g_GetStringRes(sid_heal_target_mana_critical , s_CombatInfoHealManaText[3], 256);

	g_GetStringRes(sid_dodge_self , s_CombatInfoDodgeText[0], 256);
	g_GetStringRes(sid_dodge_target , s_CombatInfoDodgeText[1], 256);
	g_GetStringRes(sid_dodge_self_critical , s_CombatInfoDodgeText[2], 256);
	g_GetStringRes(sid_dodge_target_critical , s_CombatInfoDodgeText[3], 256);

	g_GetStringRes(sid_absorb_self_life , s_CombatInfoAbsorbLifeText[0], 256);
	g_GetStringRes(sid_absorb_target_life , s_CombatInfoAbsorbLifeText[1], 256);
	g_GetStringRes(sid_absorb_self_life_critical , s_CombatInfoAbsorbLifeText[2], 256);
	g_GetStringRes(sid_absorb_target_life_critical , s_CombatInfoAbsorbLifeText[3], 256);

	g_GetStringRes(sid_absorb_self_mana , s_CombatInfoAbsorbManaText[0], 256);
	g_GetStringRes(sid_absorb_target_mana , s_CombatInfoAbsorbManaText[1], 256);
	g_GetStringRes(sid_absorb_self_mana_critical , s_CombatInfoAbsorbManaText[2], 256);
	g_GetStringRes(sid_absorb_target_mana_critical , s_CombatInfoAbsorbManaText[3], 256);	

	g_GetStringRes(sid_combat_info_basic , s_CombatInfoBasicMsgText, 256);
	g_GetStringRes(sid_combat_info_skill_name , s_CombatInfoSkillNameTemplate, 256);
	g_GetStringRes(sid_combat_info_caster_name , s_CombatInfoCasterNameTemplate, 256);
	g_GetStringRes(sid_combat_info_target_name , s_CombatInfoTargetNameTemplate, 256);
	
	g_GetStringRes(sid_combat_info_score_get,s_CombatInfoScoreGet,256);

	KTabFile configFile;
	configFile.Load(COMBATINFO_NORMAL_CONFIG_FILE);
	s_NormalDamageFrameCount = configFile.GetHeight() - 1;
	if (s_NormalDamageFrameCount > 0 && s_NormalDamageFrameCount <= DAMAGE_INFO_TOTAL_FRAME)
	{
		for (int normalFrameIndex = 0; normalFrameIndex < s_NormalDamageFrameCount; normalFrameIndex++)
		{
			int row = normalFrameIndex + 2;
			NormalDamageFrameInfo& info = s_NormalDamageFrameInfos[normalFrameIndex];
			configFile.GetInteger(row, 1, 0, &(info.PosX));
			configFile.GetInteger(row, 2, 0, &(info.PosY));
			configFile.GetInteger(row, 3, 0, &(info.Alpha));
		}		
	}
	else
	{
		s_NormalDamageFrameCount = 0;
	}
	configFile.Clear();

	configFile.Load(COMBATINFO_CRIT_CONFIG_FILE);
	s_CritDamageFrameCount = configFile.GetHeight() - 1;
	if (s_CritDamageFrameCount > 0 && s_CritDamageFrameCount <= DAMAGE_INFO_TOTAL_FRAME)
	{
		for (int critFrameIndex = 0; critFrameIndex < s_CritDamageFrameCount; critFrameIndex++)
		{
			int row = critFrameIndex + 2;
			CritDamageFrameBasicInfo& info = s_CritDamageFrameBasicInfos[critFrameIndex];
			configFile.GetInteger(row, 1, 0, &(info.PosX));
			configFile.GetInteger(row, 2, 0, &(info.PosY));
			configFile.GetInteger(row, 3, 0, &(info.Alpha));
			configFile.GetInteger(row, 4, 0, &(info.Frame));
		}
	}
	else
	{
		s_CritDamageFrameCount = 0;
	}
	configFile.Clear();

 	int i;
// 
// 	for (i = 0; i < DAMAGE_INFO_TOTAL_FRAME; i++)
// 	{
// 		s_NormalDamageFrameInfos[i].PosX = 0;
// 	}
// 	for (i = 0; i < 7; i++)
// 	{		
// 		s_NormalDamageFrameInfos[i].PosY = 4;
// 		s_NormalDamageFrameInfos[i].Alpha = 0;
// 	}
// 	for (i = 7; i < 14; i++)
// 	{
// 		s_NormalDamageFrameInfos[i].PosY = 3;
// 		s_NormalDamageFrameInfos[i].Alpha = -5;
// 	}
// 	for (i = 14; i < 35; i++)
// 	{
// 		s_NormalDamageFrameInfos[i].PosY = 2;
// 		s_NormalDamageFrameInfos[i].Alpha = -4;
// 	}
// 	for (i = 35; i < DAMAGE_INFO_TOTAL_FRAME; i++)
// 	{
// 		s_NormalDamageFrameInfos[i].PosY = 1;
// 		s_NormalDamageFrameInfos[i].Alpha = -3;
// 	}

// 	for (i = 0; i < DAMAGE_INFO_TOTAL_FRAME; i++)
// 	{
// 		s_CritDamageFrameBasicInfos[i].PosX = 0;
// 		s_CritDamageFrameBasicInfos[i].PosY = 0;
// 		s_CritDamageFrameBasicInfos[i].Alpha = 0;
// 		s_CritDamageFrameBasicInfos[i].Frame = 2;
// 	}
// 	//跳起（向上移动，变大）
// 	s_CritDamageFrameBasicInfos[0].PosY = 0;
// 	s_CritDamageFrameBasicInfos[1].PosY = 0;
// 	s_CritDamageFrameBasicInfos[2].PosY = 20;
// 	s_CritDamageFrameBasicInfos[3].PosY = 20;
// 	s_CritDamageFrameBasicInfos[4].PosY = 20;
// 	s_CritDamageFrameBasicInfos[5].PosY = 0;
// 	s_CritDamageFrameBasicInfos[0].Frame = 0;
// 	s_CritDamageFrameBasicInfos[1].Frame = 0;
// 	s_CritDamageFrameBasicInfos[2].Frame = 1;
// 	s_CritDamageFrameBasicInfos[3].Frame = 1;
// 	s_CritDamageFrameBasicInfos[4].Frame = 1;
// 	s_CritDamageFrameBasicInfos[5].Frame = 2;
// 	//晃动
// 	s_CritDamageFrameBasicInfos[7].PosX = 5;
// 	s_CritDamageFrameBasicInfos[8].PosX = -5;
// 	s_CritDamageFrameBasicInfos[9].PosX = -5;
// 	s_CritDamageFrameBasicInfos[10].PosX = 5;	
//  	s_CritDamageFrameBasicInfos[7].PosY = -5;
//  	s_CritDamageFrameBasicInfos[8].PosY = 5;
//  	s_CritDamageFrameBasicInfos[9].PosY = 5;
//  	s_CritDamageFrameBasicInfos[10].PosY = -5;
// 	//向上消失
// 	for (i = 30; i < DAMAGE_INFO_TOTAL_FRAME; i++)
// 	{
// 		s_CritDamageFrameBasicInfos[i].PosY = 1;		
// 		s_CritDamageFrameBasicInfos[i].Alpha = -6;
// 	}

	s_NormalImage.Color.Color_b.a = 255;
	s_NormalImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	s_NormalImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	s_NormalImage.nType = 1;
	sprintf(s_NormalImage.szImage, "%s", "\\spr\\strike.spr");
	
	s_HealImage.Color.Color_b.a = 255;
	s_HealImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	s_HealImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	s_HealImage.nType = 1;
	sprintf(s_HealImage.szImage, "%s", "\\spr\\heal.spr");

	s_DamageImage.Color.Color_b.a = 255;
	s_DamageImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	s_DamageImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	s_DamageImage.nType = 1;
	sprintf(s_DamageImage.szImage, "%s", "\\spr\\damage.spr");

	s_CritLabelImage.Color.Color_b.a = 255;
	s_CritLabelImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	s_CritLabelImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	s_CritLabelImage.nType = 1;
	sprintf(s_CritLabelImage.szImage, "%s", "\\spr\\crit_10.spr");

	s_MissImage.Color.Color_b.a = 255;
	s_MissImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	s_MissImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	s_MissImage.nType = 1;
	sprintf(s_MissImage.szImage, "%s", "\\spr\\miss.spr");

	s_DodgeImage.Color.Color_b.a = 255;
	s_DodgeImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	s_DodgeImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	s_DodgeImage.nType = 1;
	sprintf(s_DodgeImage.szImage, "%s", "\\spr\\dodge.spr");

	s_AbsorbImage.Color.Color_b.a = 255;
	s_AbsorbImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	s_AbsorbImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	s_AbsorbImage.nType = 1;
	sprintf(s_AbsorbImage.szImage, "%s", "\\spr\\absorb.spr");

	s_ScoreImage.Color.Color_b.a = 255;
	s_ScoreImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	s_ScoreImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	s_ScoreImage.nType = 1;
	sprintf(s_ScoreImage.szImage, "%s", "\\spr\\scoreget.spr");
	

	char strBuff[128];
	memset(s_CritDamageImage, 0, sizeof(s_CritDamageImage));
	for (i = 0; i < 10; i++)
	{
		KRUImage& critImage = s_CritDamageImage[i];
		critImage.Color.Color_b.a = 255;
		critImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		critImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		critImage.nType = 1;		
		sprintf(strBuff, "\\spr\\crit_%d.spr", i);
		strcpy(critImage.szImage, strBuff);
	}
}

ClientCombatInfoShower::ClientCombatInfoShower()
{
	Init(0);
}

void ClientCombatInfoShower::Init(int npcIndex)
{
	m_NpcIndex = npcIndex;
	memset(m_CombatInfoList, 0, sizeof(m_CombatInfoList));
	m_PosHead = 0;
	m_PosTail = 0;
	m_Count = 0;
	m_IsOn = true;	
	m_IsSelf = (GetClientPlayer().GetNpcIndex() == npcIndex);
}

void ClientCombatInfoShower::TurnOn()
{
	m_IsOn = true;
}

void ClientCombatInfoShower::TurnOff()
{
	m_IsOn = false;
}

void ClientCombatInfoShower::AddInfo(int caster, int damage, int spell, COMBAT_INFO_TYPE damageType, bool isCrit)
{	
	ShowInfoInMessageBox(caster, damage, spell, damageType, isCrit);
	ShowInfoAboveNpc(caster, damage, spell, damageType, isCrit);
}

void ClientCombatInfoShower::ShowInfoInMessageBox(int caster, int damage, int spell, COMBAT_INFO_TYPE damageType, bool isCrit)
{
	bool isSelf = (GetClientPlayer().GetNpcIndex() == m_NpcIndex);//是否为对本人造成的战斗信息
	
	if (caster > 0 && caster < MAX_NPC && damageType == COMBAT_INFO_DAMAGE_LIFE && isSelf &&
		(Npc[caster].m_Kind == kind_player || Npc[caster].m_Kind == kind_creature || Npc[caster].m_Kind == kind_employee))
	{
		CoreDataChanged(GDCNI_PLAYER_ATTACK_NOTIFY, NULL, NULL);
	}

	//在系统消息栏显示战斗信息	
	if (true)
	{
		ConfigManager& cm = ConfigManager::Singleton();

		char msgBuff[1024] = { 0 };
		char casterName[256] = { 0 };		
		char receiverName[256] = { 0 };
		char skillName[256] = { 0 };

		strcpy(casterName, s_CombatInfoUnknownTarget);
		strcpy(receiverName, s_CombatInfoUnknownTarget);
		strcpy(skillName, s_CombatInfoUnknownSkill);

		if (caster > 0 && caster < MAX_NPC)
		{
			sprintf(casterName, s_CombatInfoCasterNameTemplate, ((GetClientPlayer().GetNpcIndex() == caster) ? s_CombatInfoTargetYou : Npc[caster].Name));
		}

		sprintf(receiverName, s_CombatInfoTargetNameTemplate, (isSelf ? s_CombatInfoTargetYou : Npc[m_NpcIndex].Name));

		KSkill* pSkill = g_SkillManager.GetSkill(spell);
		if (pSkill != NULL)
		{
			sprintf(skillName, s_CombatInfoSkillNameTemplate, pSkill->m_szName);
		}		
		
		int styleParam = 0;
		if (isSelf)
		{
			styleParam = (isCrit ? 2 : 0);
		}
		else
		{
			styleParam = (isCrit ? 3 : 1);
		}		
		
		switch(damageType)
		{
		case COMBAT_INFO_DAMAGE_LIFE:							
			sprintf(msgBuff, s_CombatInfoDamageLifeText[styleParam],
				casterName,
				skillName,
				receiverName, 
				damage,
				(isCrit ? s_CombatInfoDamageCritical : "")
			);
			break;
		case COMBAT_INFO_HEAL_LIFE:
			sprintf(msgBuff, s_CombatInfoHealLifeText[styleParam],
				casterName, 
				skillName, 
				receiverName, 
				damage,
				(isCrit ? s_CombatInfoHealCritical : "")
			);
			break;
		case COMBAT_INFO_DAMAGE_MANA:
			sprintf(msgBuff, s_CombatInfoDamageManaText[styleParam],
				casterName,
				skillName,
				receiverName, 
				damage,
				(isCrit ? s_CombatInfoDamageCritical : "")
			);
			break;
		case COMBAT_INFO_HEAL_MANA:
			sprintf(msgBuff, s_CombatInfoHealManaText[styleParam],
				casterName, 
				skillName, 
				receiverName, 
				damage,
				(isCrit ? s_CombatInfoHealCritical : "")
			);
			break;
		case COMBAT_INFO_DODGE:
			sprintf(msgBuff, s_CombatInfoDodgeText[styleParam],
				receiverName,
				casterName, 
				skillName
			);
			break;
		case COMBAT_INFO_ABSORB_LIFE:
			sprintf(msgBuff, s_CombatInfoAbsorbLifeText[styleParam],
				receiverName,
				casterName, 
				skillName
			);
			break;
		case COMBAT_INFO_ABSORB_MANA:
			sprintf(msgBuff, s_CombatInfoAbsorbManaText[styleParam],
				receiverName,
				casterName, 
				skillName
			);
			break;
		case COMBAT_INFO_SCORE_GET:
			sprintf(msgBuff,s_CombatInfoScoreGet,damage);
			break;

		default:
			_ASSERT(false);
			break;
		}

		char uiMsgBuff[1024] = { 0 };
		sprintf(uiMsgBuff, s_CombatInfoBasicMsgText, msgBuff);

		CoreDataChanged(GDCNI_APPEND_MESSAGE, (unsigned int)uiMsgBuff, COMBAT_INFO_ROOM_ID);
	}
}

void ClientCombatInfoShower::ShowInfoAboveNpc(int caster, int damage, int spell, COMBAT_INFO_TYPE damageType, bool isCrit)
{
	bool isSelf = (GetClientPlayer().GetNpcIndex() == m_NpcIndex);//是否为对本人造成的战斗信息

	if (damageType == COMBAT_INFO_DAMAGE_LIFE ||
		damageType == COMBAT_INFO_HEAL_LIFE ||
		damageType == COMBAT_INFO_DODGE ||
		damageType == COMBAT_INFO_ABSORB_LIFE ||
		damageType == COMBAT_INFO_ABSORB_MANA || 
		damageType == COMBAT_INFO_SCORE_GET )
	{
		if (m_Count < MAX_COMBAT_INFO && m_NpcIndex > 0)
		{
			int posNew = (m_PosHead + m_Count) % MAX_COMBAT_INFO;
			COMBAT_INFO& info = m_CombatInfoList[posNew];		
			info.Damage = damage;
			info.Spell = spell;
			info.DamageType = damageType;
			info.IsCrit = isCrit;
			info.Frame = 0;
			info.Alpha = 255;
			
			if (isSelf)
			{
				info.PosX = 0;
				info.PosY = 0;						
			}
			else
			{
				//如果是爆击
				if (isCrit)
				{
					//随即决定爆击伤害信息的显示位置
					info.PosX = (DAMAGE_INFO_TEXT_RANDOM_X * rand() / RAND_MAX) - (DAMAGE_INFO_TEXT_RANDOM_X / 2);
					info.PosY = (DAMAGE_INFO_TEXT_RANDOM_Y * rand() / RAND_MAX);
				}
				else
				{
					info.PosX = 0;
					info.PosY = 0;
					
					//			//如果前面一个伤害信息的还没有来得及上移
					// 			if (m_Count > 0)
					// 			{
					// 				int perviousInfoIndex = (posNew - 1 + MAX_COMBAT_INFO) % MAX_COMBAT_INFO;
					// 				COMBAT_INFO& perviousInfo = m_CombatInfoList[perviousInfoIndex];
					// 				if (perviousInfo.PosY < DAMAGE_INFO_TEXT_HEIGHT)
					// 				{
					// 					int offset = DAMAGE_INFO_TEXT_HEIGHT - perviousInfo.PosY;
					// 
					// 					//新的伤害信息紧接着排在前面的信息下面
					// 					int perviousInfoLoopCount = perviousInfoIndex;
					// 					while (perviousInfoLoopCount != m_PosHead)
					// 					{
					// 						perviousInfoLoopCount = (perviousInfoLoopCount > 0) ? (perviousInfoLoopCount - 1) : (MAX_COMBAT_INFO - 1);
					// 						m_CombatInfoList[perviousInfoLoopCount].PosY += offset;
					// 					}
					// 					m_CombatInfoList[m_PosHead].PosY += offset;
					// 				}
					// 			}		
				}	
			}
			
			m_Count++;
		}
	}	
}

void ClientCombatInfoShower::Draw(int baseHeight)
{	
	if (m_NpcIndex > 0)
	{
		KNpc& npc = Npc[m_NpcIndex];
		unsigned long currentTime = time(NULL);		
		int npcMapPosX, npcMapPosY;
		npc.GetMpsPos(&npcMapPosX, &npcMapPosY);

		for(int i = 0; i < m_Count; i++)
		{
			COMBAT_INFO& info = m_CombatInfoList[(m_PosHead + i) % MAX_COMBAT_INFO];
			
			int totalFrame = info.IsCrit ? s_CritDamageFrameCount : s_NormalDamageFrameCount;
			if (info.Frame < totalFrame)
			{
				if ((GetClientPlayer().GetNpcIndex() == m_NpcIndex))//本人的战斗信息
				{
					KRUImage* pImage = NULL;
					switch(info.DamageType)
					{
					case COMBAT_INFO_DAMAGE_LIFE:							
						pImage = &s_DamageImage;
						break;
					case COMBAT_INFO_HEAL_LIFE:
						pImage = &s_HealImage;
						break;
					case COMBAT_INFO_DODGE:
						pImage = &s_DodgeImage;
						break;
					case COMBAT_INFO_ABSORB_LIFE:
						pImage = &s_AbsorbImage;
						break;
					case COMBAT_INFO_ABSORB_MANA:
						pImage = &s_AbsorbImage;
						break;
					case COMBAT_INFO_SCORE_GET:
						pImage = &s_ScoreImage;
						break;
					default:
						_ASSERT(false);
						break;
					}
					if (pImage != NULL)
					{
						NormalDamageFrameInfo& frameInfo = s_NormalDamageFrameInfos[info.Frame];
						info.PosX = info.PosX + frameInfo.PosX;
						info.PosY = info.PosY + frameInfo.PosY;
						info.Alpha = info.Alpha + frameInfo.Alpha;
						pImage->oPosition.nX = npcMapPosX + info.PosX;
						pImage->oPosition.nY = npcMapPosY - baseHeight - DAMAGE_INFO_TEXT_BASIC_OFFSET_Y - info.PosY;
						pImage->Color.Color_b.a = info.Alpha;
						pImage->uImage = 0;						
						
						if (
							info.DamageType == COMBAT_INFO_DAMAGE_LIFE ||
							info.DamageType == COMBAT_INFO_HEAL_LIFE  )
						{
							int damage = info.Damage;						
							do {
								pImage->nFrame = (unsigned short)(damage % 10);
								damage /= 10;
								pImage->oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
								g_pRepresentShell->DrawPrimitives(1,pImage, RU_T_IMAGE,FALSE);
							} while (damage > 0);
							
							pImage->nFrame = 10;							
							pImage->oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
							g_pRepresentShell->DrawPrimitives(1,pImage, RU_T_IMAGE,FALSE);
						}
						else if (
							info.DamageType == COMBAT_INFO_DODGE ||
							info.DamageType == COMBAT_INFO_ABSORB_LIFE ||
							info.DamageType == COMBAT_INFO_ABSORB_MANA )
						{
							pImage->nFrame = 0;
							g_pRepresentShell->DrawPrimitives(1,pImage, RU_T_IMAGE,FALSE);
						}
						else if (info.DamageType == COMBAT_INFO_SCORE_GET)
						{
							int damage = info.Damage;						
							do {
								pImage->nFrame = (unsigned short)(damage % 10);
								damage /= 10;
								pImage->oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
								g_pRepresentShell->DrawPrimitives(1,pImage, RU_T_IMAGE,FALSE);
							} while (damage > 0);

						}

					}
				}
				else//本人对其他人造成的战斗信息
				{
					if (!info.IsCrit)
					{
						KRUImage* pImage = NULL;
						switch(info.DamageType)
						{
						case COMBAT_INFO_DAMAGE_LIFE:							
							pImage = &s_NormalImage;
							break;
						case COMBAT_INFO_HEAL_LIFE:
							pImage = &s_HealImage;
							break;
						case COMBAT_INFO_DODGE:
							pImage = &s_MissImage;
							break;
						case COMBAT_INFO_ABSORB_LIFE:
							pImage = &s_AbsorbImage;
							break;
						case COMBAT_INFO_ABSORB_MANA:
							pImage = &s_AbsorbImage;
							break;
						case COMBAT_INFO_SCORE_GET:
							pImage = &s_ScoreImage;
							break;

						default:
							_ASSERT(false);
							break;
						}
						if (pImage != NULL)
						{
							NormalDamageFrameInfo& frameInfo = s_NormalDamageFrameInfos[info.Frame];
							info.PosX = info.PosX + frameInfo.PosX;
							info.PosY = info.PosY + frameInfo.PosY;
							info.Alpha = info.Alpha + frameInfo.Alpha;
							pImage->oPosition.nX = npcMapPosX + info.PosX;
							pImage->oPosition.nY = npcMapPosY - baseHeight - DAMAGE_INFO_TEXT_BASIC_OFFSET_Y - info.PosY;
							pImage->Color.Color_b.a = info.Alpha;
							pImage->uImage = 0;						
							
							if (
								info.DamageType == COMBAT_INFO_DAMAGE_LIFE || 
								info.DamageType == COMBAT_INFO_HEAL_LIFE)
							{
								int damage = info.Damage;						
								do {
									pImage->nFrame = (unsigned short)(damage % 10);
									damage /= 10;
									pImage->oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
									g_pRepresentShell->DrawPrimitives(1,pImage, RU_T_IMAGE,FALSE);
								} while (damage > 0);
								
								pImage->nFrame = 10;							
								pImage->oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
								g_pRepresentShell->DrawPrimitives(1,pImage, RU_T_IMAGE,FALSE);
							}
							else if (
								info.DamageType == COMBAT_INFO_DODGE ||
								info.DamageType == COMBAT_INFO_ABSORB_LIFE ||
								info.DamageType == COMBAT_INFO_ABSORB_MANA)
							{
								pImage->nFrame = 0;
								g_pRepresentShell->DrawPrimitives(1,pImage, RU_T_IMAGE,FALSE);
							}
						}
					}
					else
					{
						CritDamageFrameBasicInfo& frameBasicInfo = s_CritDamageFrameBasicInfos[info.Frame];
						
						info.PosX = info.PosX + frameBasicInfo.PosX;
						info.PosY = info.PosY + frameBasicInfo.PosY;
						info.Alpha = info.Alpha + frameBasicInfo.Alpha;
						
						s_CritLabelImage.nFrame = frameBasicInfo.Frame;
						s_CritLabelImage.oPosition.nX = npcMapPosX + info.PosX + s_CritDamageTextWidth[frameBasicInfo.Frame];
						s_CritLabelImage.oPosition.nY = npcMapPosY - baseHeight - DAMAGE_INFO_TEXT_BASIC_OFFSET_Y - info.PosY;
						s_CritLabelImage.Color.Color_b.a = info.Alpha;	
						s_CritLabelImage.uImage = 0;
						g_pRepresentShell->DrawPrimitives(1,&s_CritLabelImage, RU_T_IMAGE,FALSE);
						
						int damage = info.Damage;
						int i = 0;
						do {
							KRUImage& image = s_CritDamageImage[(unsigned short)(damage % 10)];
							
							image.nFrame = frameBasicInfo.Frame;
							image.oPosition.nX = npcMapPosX + info.PosX - s_CritDamageTextWidth[frameBasicInfo.Frame] * i;
							image.oPosition.nY = npcMapPosY - baseHeight - DAMAGE_INFO_TEXT_BASIC_OFFSET_Y - info.PosY;
							image.Color.Color_b.a = info.Alpha;	
							image.uImage = 0;
							
							damage /= 10;							
							g_pRepresentShell->DrawPrimitives(1,&image, RU_T_IMAGE,FALSE);
							i++;
						} while (damage > 0);
					}
				}
			
				info.Frame++;
			}
			else
			{
				m_PosHead = (m_PosHead + 1) % MAX_COMBAT_INFO;
				m_Count--;
			}
		}
	}
}