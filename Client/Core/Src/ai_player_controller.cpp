//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007-07-28
//      File_base        : ai_player_controller
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : Player控制器
//
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#include "SkillDef.h"
#include "KSkills.h"
#include "CoreShell.h"
#include "KSubWorld.h"
#include "ai_player_controller.h"
#include "KWin32Wnd.h"
#include "AutoRobotMgr.h"

#define DIALOG_NPC_RANGE 200
#define SEND_GOTO_CMD_DELAY_FRAME 3
#define SEND_NEXT_SKILL_DELAY_FRAME 3
#define NORMAL_AI_FRAME_DELAY 4
#define MIN_KEEP_DISTANCE 20

#ifndef MAX_FOLLOW_DISTANCE
#define	MAX_FOLLOW_DISTANCE	48
#endif

PlayerController::PlayerController()
{
}

PlayerController::~PlayerController()
{
}

PlayerController& PlayerController::Singleton()
{
	static PlayerController controller;
	return controller;
}

void PlayerController::Init()
{
	m_PlayerState = player_state_none;
	m_SelectedSkillId = INVALID_SKILL_ID;
	m_FollowAttackNpcIndex = 0;
	m_FollowAttackNpcId = 0;
	m_FollowNpcIndex = 0;
	m_FollowNpcId = 0;
	m_PickUpObjectIndex = 0;
	m_PickUpObjectId = 0;
	m_NextAiTime = 0;
	m_InRequiredRange = false;

	m_NpcIndex = GetClientPlayer().GetNpcIndex();	
}

void PlayerController::ChangeWorld()
{
	m_PlayerState = player_state_none;
	m_FollowAttackNpcIndex = 0;
	m_FollowAttackNpcId = 0;
	m_FollowNpcIndex = 0;
	m_FollowNpcId = 0;
	m_PickUpObjectIndex = 0;
	m_PickUpObjectId = 0;
	m_NextAiTime = 0;
	
	m_NpcIndex = GetClientPlayer().GetNpcIndex();
}

void PlayerController::Active()
{
	if (m_NextAiTime > GetCurrentTime())
		return;

	if (GetClientPlayer().IsBlockClientControl())
		return;

	switch(m_PlayerState)
	{
	case player_state_none:
		ProcessNormal();
		break;
	case player_state_follow_npc:
		ProcessFollowNpc();
		break;
	case player_state_follow_attack:
		ProcessFollowAttack();
		break;
	case player_state_follow_dialog:
		ProcessFollowDialog();
		break;
	case player_state_pickup_object:
		ProcessPickupObject();
		break;
	}
}

void PlayerController::Stop()
{
	StopAutoAttack();

	m_PlayerState = player_state_none;	
	m_FollowAttackNpcIndex = 0;
	m_FollowAttackNpcId = 0;
	m_FollowNpcIndex = 0;
	m_FollowNpcId = 0;
	m_PickUpObjectIndex = 0;
	m_PickUpObjectId = 0;
	m_InRequiredRange = false;
	m_KeepRangeMax = 0;
	m_KeepRangeMin = 0;
	m_NextSkillRequiredDistance = 0;
}

void PlayerController::SetSelectedSkill(int skillId)
{
	m_SelectedSkillId = skillId;

	if(RUN_SKILL_ID == skillId)
	{
		StopAutoAttack();
		return;
	}

	if (m_FollowAttackNpcIndex > 0)
	{
		int targetPosX = 0;
		int targetPosY = 0;
		KNpc& target = Npc[m_FollowAttackNpcIndex];
		target.GetMpsPos(&targetPosX, &targetPosY);
		
		CastSkillParam skillParam;
		int requiredDistance = 0;
		if (GetSkillParam(m_NpcIndex, m_SelectedSkillId, m_FollowAttackNpcIndex, targetPosX, targetPosY, false, &skillParam, &requiredDistance))
		{
			if (skill_useable_result_ok != CheckSkillRequirement(m_NpcIndex, skillParam.SkillId, m_FollowAttackNpcIndex, skillParam.Param1, skillParam.Param2, false, true))
			{
				StopAutoAttack();
				return;
			}

			if (m_KeepRangeMin == 0 || m_KeepRangeMax > requiredDistance)
			{
				if (requiredDistance == 0)
				{
					m_KeepRangeMin = 0;
				}
				else
				{
					m_KeepRangeMin = requiredDistance - ConfigManager::Singleton().GetGlobalVariable(global_var_ai_player_follow_attack_range_reduce);
					if (m_KeepRangeMin < 0)
					{
						m_KeepRangeMin = MIN_KEEP_DISTANCE;
					}
				}
				m_KeepRangeMax = requiredDistance;
			}
			
			StartAutoAttack(skillParam);
		}
		else
		{
			StopAutoAttack();
		}
	}

	CoreDataChanged(GDCNI_REFRESH_SELECTED_SKILL, NULL, 0);
}

void PlayerController::ProcessNormal()
{
	if (m_NextSkill.SkillId != INVALID_SKILL_ID)
	{
		KNpc& self = Npc[m_NpcIndex];
		if (m_NextSkill.Param1 == SKILL_SPT_TargetIndex)
		{
			int targetNpcIndex = NpcSet.SearchID((DWORD)m_NextSkill.Param2);
			if (targetNpcIndex > 0)
			{
				int distance = NpcSet.GetDistance(m_NpcIndex, targetNpcIndex);
				if (m_KeepRangeMin == 0 || distance <= m_KeepRangeMin)
				{
					SendNextSkill(m_NextSkill);
					m_NextSkill.SkillId = INVALID_SKILL_ID;
				}
				else
				{
					int destX, destY;
					Npc[targetNpcIndex].GetMpsPos(&destX, &destY);
					GoTo(destX, destY);

					DelayAi(SEND_GOTO_CMD_DELAY_FRAME);
				}
			}
		}
		else
		{
			int distance = sqrt(NpcSet.GetDistanceSquare(m_NextSkill.Param1, m_NextSkill.Param2, m_NpcIndex));
			if (m_KeepRangeMin == 0 || m_NextSkillRequiredDistance == 0 || distance <= m_KeepRangeMin)
			{
				SendNextSkill(m_NextSkill);
				m_NextSkill.SkillId = INVALID_SKILL_ID;
			}
			else
			{
				GoTo(m_NextSkill.Param1, m_NextSkill.Param2);

				DelayAi(SEND_GOTO_CMD_DELAY_FRAME);
			}
		}
	}
}

void PlayerController::FollowAttack(int npcIndex)
{
	if (IsValidNpc(npcIndex) && m_SelectedSkillId != RUN_SKILL_ID)
	{
		int targetPosX = 0;
		int targetPosY = 0;
		KNpc& target = Npc[npcIndex];
		target.GetMpsPos(&targetPosX, &targetPosY);
		
		CastSkillParam skillParam;
		int requiredDistance = 0;
		if (GetSkillParam(m_NpcIndex, m_SelectedSkillId, npcIndex, targetPosX, targetPosY, false, &skillParam, &requiredDistance))
		{
			if (skill_useable_result_ok != CheckSkillRequirement(m_NpcIndex, skillParam.SkillId, npcIndex, skillParam.Param1, skillParam.Param2, false, true))
				return;

			m_FollowAttackNpcIndex = npcIndex;
			m_FollowAttackNpcId = Npc[npcIndex].GetId();
			
			if (requiredDistance == 0)
			{
				m_KeepRangeMin = 0;
			}
			else
			{
				m_KeepRangeMin = requiredDistance - ConfigManager::Singleton().GetGlobalVariable(global_var_ai_player_follow_attack_range_reduce);
				if (m_KeepRangeMin < 0)
				{
					m_KeepRangeMin = MIN_KEEP_DISTANCE;
				}
			}
			m_KeepRangeMax = requiredDistance;
			
			StartAutoAttack(skillParam);
		}
	}
}

void PlayerController::FollowNpc(int npcIndex)
{
	Stop();

	if (IsValidNpc(npcIndex))
	{
		m_FollowNpcIndex = npcIndex;
		m_FollowNpcId    = Npc[npcIndex].GetId();
		m_PlayerState    = player_state_follow_npc;
	}//endif
}

void PlayerController::FollowDialog(int npcIndex)
{
	Stop();

	if (IsValidNpc(npcIndex))
	{
		m_FollowDialogNpcIndex = npcIndex;
		m_FollowDialogNpcId = Npc[npcIndex].GetId();
		m_PlayerState = player_state_follow_dialog;
	}
}

void PlayerController::PickupObject(int objectIndex)
{
	if (m_PlayerState == player_state_pickup_object && m_PickUpObjectIndex == objectIndex && m_PickUpObjectId == Object[objectIndex].m_nID )
			return;

	Stop();

	if (objectIndex > 0 && objectIndex < MAX_OBJECT)
	{
		m_PickUpObjectIndex = objectIndex;
		m_PickUpObjectId = Object[objectIndex].m_nID;
		m_PlayerState = player_state_pickup_object;
		ProcessPickupObject();
	}	
}

void PlayerController::ProcessFollowNpc()
{
	if ( !IsValidNpc(m_FollowNpcIndex) || Npc[m_FollowNpcIndex].GetId() != m_FollowNpcId)
	{
	     Stop();
		 return ;
	}//endif

	int distance = NpcSet.GetDistance(m_FollowNpcIndex, m_NpcIndex);
	
	int nDesX, nDesY;
	if (distance > MAX_FOLLOW_DISTANCE)
	{
		Npc[m_FollowNpcIndex].GetMpsPos(&nDesX, &nDesY);
		Npc[m_NpcIndex].SendCommand(do_run, nDesX, nDesY);			
		SendClientCmdRun(nDesX, nDesY);
		DelayAi(NORMAL_AI_FRAME_DELAY);
	}
}

void PlayerController::ProcessFollowAttack()
{
	if (m_FollowAttackNpcIndex <= 0)
		return;
	
	KNpc& target = Npc[m_FollowAttackNpcIndex];
	if (target.GetId() != m_FollowAttackNpcId)
	{
		Stop();
		return;
	}
	
	KNpc& self = Npc[m_NpcIndex];
	int distance = NpcSet.GetDistance(m_NpcIndex, m_FollowAttackNpcIndex);

	if (m_NextSkill.SkillId != INVALID_SKILL_ID)
	{
		if (m_NextSkillRequiredDistance == 0 || distance <= m_NextSkillRequiredDistance)
		{
			SendNextSkill(m_NextSkill);
			m_NextSkill.SkillId = INVALID_SKILL_ID;

			DelayAi(SEND_NEXT_SKILL_DELAY_FRAME);
			return;
		}
	}

	if (m_KeepRangeMin > 0 && distance > (m_InRequiredRange ? m_KeepRangeMax : m_KeepRangeMin))
	{
		m_InRequiredRange = false;

		int targetX, targetY;
		target.GetMpsPos(&targetX, &targetY);
		GoTo(targetX, targetY);

// 		int selfX, selfY;		
// 		self.GetMpsPos(&selfX, &selfY);
// 		double xDist = selfX - targetX;
// 		double yDist = selfY - targetY;		
// 		if (xDist != 0 || yDist != 0)
// 		{
// 			int xSpace = (int)((double)m_KeepRangeMin * sqrt(xDist * xDist / (xDist * xDist + yDist * yDist)));
// 			int ySpace = (int)((double)m_KeepRangeMin * sqrt(yDist * yDist / (xDist * xDist + yDist * yDist)));
// 			int destX = targetX + ((xDist > 0) ? xSpace : -xSpace);
// 			int destY = targetY + ((yDist > 0) ? ySpace : -ySpace);
// 			SendClientCmdRun(destX, destY);
// 		}
		
		DelayAi(SEND_GOTO_CMD_DELAY_FRAME);
	}
	else
	{
		if (!m_InRequiredRange)
		{
			self.SendCommand(do_stand);
			self.SendC2SPosSync();
		}

		m_InRequiredRange = true;
	}
}

void PlayerController::ProcessFollowDialog()
{
	if (m_FollowDialogNpcIndex <= 0)
		return;

	KNpc& target = Npc[m_FollowDialogNpcIndex];

	if (target.GetId() != m_FollowDialogNpcId)
	{
		Stop();
		return;
	}

	int distance = NpcSet.GetDistance(m_NpcIndex, m_FollowDialogNpcIndex);
	if (distance < DIALOG_NPC_RANGE)
	{
		//TODO 命令NPC停下，避免靠的太近

		GetClientPlayer().DialogNpc(m_FollowDialogNpcIndex);		
		Stop();
	}
	else
	{
		int destX, destY;
		target.GetMpsPos(&destX, &destY);
		GoTo(destX, destY);
		DelayAi(NORMAL_AI_FRAME_DELAY);
	}
}

void PlayerController::ProcessPickupObject()
{
	if (m_PickUpObjectIndex <= 0)
		return;

	KObj& object = Object[m_PickUpObjectIndex];

	if (object.m_nID != m_PickUpObjectId)
	{
		Stop();
		return;
	}

	int selfPosX, selfPosY, objectPosX, objectPosY;
	Npc[m_NpcIndex].GetMpsPos(&selfPosX, &selfPosY);
	object.GetMpsPos(&objectPosX, &objectPosY);

	if ((selfPosX - objectPosX) * (selfPosX - objectPosX) + (selfPosY - objectPosY) * (selfPosY - objectPosY) < PLAYER_PICKUP_CLIENT_DISTANCE * PLAYER_PICKUP_CLIENT_DISTANCE)
	{
		//TODO 命令NPC停下

		GetClientPlayer().PickUpObj(m_PickUpObjectIndex);
		Stop();
	}
	else
	{
		GoTo(objectPosX, objectPosY);
		DelayAi(NORMAL_AI_FRAME_DELAY);
	}
}

bool PlayerController::CanUseSkill()
{
	int doing = Npc[m_NpcIndex].m_Doing;
	return (doing == do_stand || doing == do_none || doing == do_hurt || doing == do_run);
}

void PlayerController::CastSkill(int skillId, int param1, int param2)
{
	if(SKILL_SPT_TargetIndex == param1)
		SendClientCmdSkill(skillId, param1, Npc[param2].m_dwID);
	else
		SendClientCmdSkill(skillId, param1, param2);

//	Npc[m_NpcIndex].SendCommand(do_skill, skillId, param1, param2);
}

void PlayerController::GoTo(int destX, int destY)
{
	Npc[m_NpcIndex].SendCommand(do_run, destX, destY);
	SendClientCmdRun(destX, destY);
}

unsigned long PlayerController::GetCurrentTime()
{
	return SubWorld[Npc[m_NpcIndex].m_SubWorldIndex].m_dwCurrentTime;
}

bool PlayerController::IsCastSelf(int relation, int skillTargetType)
{
	if (att_target_self & skillTargetType)
	{
		if (relation_enemy == relation)
		{
			if (!(att_target_enemynpc & skillTargetType) && !(att_target_enemyplayer & skillTargetType))
				return true;
		}
		else if (relation_ally == relation)
		{
			if (!(att_target_allynpc & skillTargetType) && !(att_target_allyplayer & skillTargetType))
				return true;
		}
		else if (relation_dialog == relation)
		{
			return true;
		}
	}

	return false;
}

void PlayerController::StartAutoAttack(CastSkillParam& skillParam)
{
	m_PlayerState = player_state_follow_attack;	

	SELECT_SKILL selectSkill;
	selectSkill.Protocol = c2s_select_skill;
	selectSkill.SkillID = skillParam.SkillId;
	selectSkill.SkillParam1 = skillParam.Param1;
	selectSkill.SkillParam2 = skillParam.Param2;

	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID, &selectSkill, sizeof(selectSkill));
}

void PlayerController::StopAutoAttack()
{
	if (m_PlayerState != player_state_follow_attack)
		return;

	SELECT_SKILL selectSkill;
	selectSkill.Protocol = c2s_select_skill;
	selectSkill.SkillID = INVALID_SKILL_ID;
	selectSkill.SkillParam1 = 0;
	selectSkill.SkillParam2 = 0;
	
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID, &selectSkill, sizeof(selectSkill));
}

void PlayerController::SendNextSkill(CastSkillParam& skillParam)
{
	NPC_SKILL_COMMAND skillCmd;	
	skillCmd.ProtocolType = c2s_npcskill;
	skillCmd.nSkillID = skillParam.SkillId;
	skillCmd.nMpsX = skillParam.Param1;
	skillCmd.nMpsY = skillParam.Param2;

	Npc[m_NpcIndex].SendCommand(do_stand);

	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID, &skillCmd, sizeof(skillCmd));	
}

void PlayerController::SetNextSkill(int skillId, int mouseX, int mouseY, bool targetSelf)
{
	if(RUN_SKILL_ID == skillId)
	{
		GoTo(mouseX, mouseY);
		return;
	}

	int targetNpcIndex = Npc[m_NpcIndex].GetTargetNpc();
	if (m_FollowAttackNpcIndex > 0 && targetNpcIndex <= 0)
		targetNpcIndex = m_FollowAttackNpcIndex;

	CastSkillParam skillParam;
	if (GetSkillParam(m_NpcIndex, skillId, targetNpcIndex, mouseX, mouseY, targetSelf, &skillParam, &m_NextSkillRequiredDistance))
	{
		if (skill_useable_result_ok != CheckSkillRequirement(m_NpcIndex, skillParam.SkillId, targetNpcIndex, skillParam.Param1, skillParam.Param2, targetSelf, true))
			return;

		m_NextSkill = skillParam;
		if (m_KeepRangeMin == 0 || (m_NextSkillRequiredDistance > 0 && m_KeepRangeMax > m_NextSkillRequiredDistance))
		{
			if (m_NextSkillRequiredDistance == 0)
			{
				m_KeepRangeMin = 0;
			}
			else
			{
				m_KeepRangeMin = m_NextSkillRequiredDistance - ConfigManager::Singleton().GetGlobalVariable(global_var_ai_player_follow_attack_range_reduce);
				if (m_KeepRangeMin < 0)
				{
					m_KeepRangeMin = MIN_KEEP_DISTANCE;
				}
			}
			m_KeepRangeMax = m_NextSkillRequiredDistance;
		}
	}
}

bool PlayerController::GetSkillParam(int selfNpcIndex, int skillId, int targetNpcIndex, int mouseX, int mouseY, bool targetSelf, CastSkillParam* pSkillParam, int* pRequiredDistance)
{
	if (pSkillParam == NULL || selfNpcIndex <= 0)
		return false;

	KNpc& self = Npc[selfNpcIndex];

	if (skillId == INVALID_SKILL_ID)
	{
		switch(self.GetSeries())
		{
		case 0:
			skillId = KNIGHT_NORMALSKILL_ID;
			break;
		case 1:
			skillId = ENCHANTER_NORMALSKILL_ID;
			break;
		case 2:
			skillId = MONSTROUS_NORMAILSKILL_ID;
			break;
		}
	}

	skillId = self.GetSkillList().GetCurSameSubSkillId(skillId);

	KSkill* pSkill = g_SkillManager.GetSkill(skillId);
	if (NULL != pSkill)
	{
		int attackRadius = pSkill->GetAttackRadius();
		int requredDistance = 0;
		int skillTargetType = pSkill->GetAttackTargetType();
		
		if (att_target_only & skillTargetType)//单攻
		{
			pSkillParam->Param1 = SKILL_SPT_TargetIndex;

			if (IsValidNpc(targetNpcIndex))
			{
				int	relation = NpcSet.GetRelation(selfNpcIndex, targetNpcIndex);
				SkillType skillType = pSkill->GetSkillType();
				if (targetSelf || IsCastSelf(relation, skillTargetType))
				{
					pSkillParam->Param2 = Npc[selfNpcIndex].GetId();
				}
				else
				{
					pSkillParam->Param2 = Npc[targetNpcIndex].GetId();
				}
			}
			else
			{
				pSkillParam->Param2 = Npc[selfNpcIndex].GetId();
			}
			requredDistance = attackRadius;
		}
		else//群攻
		{
			switch(pSkill->GetSkillStyle())
			{
			case SKILL_SS_AddNearTrap:
			case SKILL_SS_NearMultiAttack:
				{
					pSkillParam->Param1 = g_GetScreenWidth() / 2;
					pSkillParam->Param2 = g_GetScreenHeight() / 2;
					requredDistance = 0;
				}
				break;
			case SKILL_SS_AddFixRangeTrap:
				{
					int destX, destY;
					int selfX, selfY;		
					self.GetMpsPos(&selfX, &selfY);
					double xDist = mouseX - selfX;
					double yDist = mouseY - selfY;
					if (xDist != 0 || yDist != 0)
					{
						int xSpace = (int)((double)attackRadius * sqrt(xDist * xDist / (xDist * xDist + yDist * yDist)));
						int ySpace = (int)((double)attackRadius * sqrt(yDist * yDist / (xDist * xDist + yDist * yDist)));
						destX = selfX + ((xDist > 0) ? xSpace : -xSpace);
						destY = selfY + ((yDist > 0) ? ySpace : -ySpace);
					}

					pSkillParam->Param1 = destX;
					pSkillParam->Param2 = destY;
					requredDistance = 0;
				}
				break;
			case SKILL_SS_Rectangle:
				{
					pSkillParam->Param1 = mouseX;
					pSkillParam->Param2 = mouseY;
					requredDistance = 0;
				}
				break;
			default:
				{
					pSkillParam->Param1 = mouseX;
					pSkillParam->Param2 = mouseY;
					requredDistance = attackRadius;
				}
				break;
			}
		}

		if (pRequiredDistance)
		{
			*pRequiredDistance = requredDistance;
		}

		pSkillParam->SkillId = skillId;

		return true;
	}

	return false;
}

void PlayerController::DelayAi(unsigned long delayFrame)
{
	m_NextAiTime = GetCurrentTime() + delayFrame;
}

enumSkillUseableResult PlayerController::CheckSkillRequirement(int selfNpcIndex, int skillId, int targetNpcIndex, int mouseX, int mouseY, bool targetSelf, bool showError)
{
	enumSkillUseableResult result = CheckSkillRequirement(selfNpcIndex, skillId, targetNpcIndex, mouseX, mouseY, targetSelf);
	if (showError)
	{
		switch(result)
		{
		case skill_useable_result_unknown:
			break;
		case skill_useable_result_ok:
			break;		
		case skill_useable_result_out_of_range:
			break;
		case skill_useable_result_invalid_skill:
			break;
		case skill_useable_result_no_mana:
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)MSG_NPC_NO_MANA, 0);
			break;
		case skill_useable_result_no_life:
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)MSG_NPC_NO_LIFE, 0);
			break;
		case skill_useable_result_no_skill_exp:
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)MSG_NPC_NO_SKILLEXP, 0);
			break;
		case skill_useable_result_in_cd:
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)MSG_SKILL_IN_CD, 0);
			break;
		case skill_useable_result_invalid_target:
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)MSG_SKILL_INVALID_TARGET, 0);
			break;
		case skill_useable_result_no_target:
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)MSG_SKILL_NOTARGET, 0);
			break;
		case skill_useable_result_pk_protection:
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)MSG_SKILL_PK_PROTECTION, 0);
			break;
		case skill_useable_result_no_required_weapon:
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)MSG_SKILL_NOWEAPON, 0);
			break;
		case skill_useable_result_no_required_item:
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)MSG_SKILL_NEED_ITEM, 0);
			break;

// 			const char *szItemName = GetItemName(m_ItemKeys[0], m_ItemKeys[1], m_ItemKeys[2], m_ItemKeys[3]);
// 			if(NULL == szItemName)
// 				szItemName = "";
// 			char	szNeedItemMsg[256];
// 			snprintf(szNeedItemMsg, sizeof(szNeedItemMsg), MSG_SKILL_NEED_ITEM, szItemName);
// 			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)szNeedItemMsg, 0);

		case skill_useable_result_behind_barrier:
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)MSG_TARGET_BEHIND_BARRIER, 0);
			AutoRobotMgr::Singleton().ResetTargetNpc();
			break;
		default:
			break;
		}
	}

	return result;
}

enumSkillUseableResult PlayerController::CheckSkillRequirement(int selfNpcIndex, int skillId, int targetNpcIndex, int mouseX, int mouseY, bool targetSelf)
{
	if (selfNpcIndex <= 0)
		return skill_useable_result_unknown;

	//判断技能是否存在
	KSkill* pSkill = g_SkillManager.GetSkill(skillId);
	if (NULL == pSkill)
		return skill_useable_result_invalid_skill;

	KNpc& self = Npc[selfNpcIndex];
	NpcSkillList& skillList = self.GetSkillList();
	int attackRadius = pSkill->GetAttackRadius();
	int skillTargetType = pSkill->GetAttackTargetType();

	//判断技能是否已经冷却
	if (!skillList.CanCast(skillId))
		return skill_useable_result_in_cd;
	
	KPlayer& selfPlayer = Player[self.GetPlayerIdx()];
	
	//需要指定的武器才能放技能
	int requiredWeaponType = pSkill->GetWeaponSkill();
	int curWeaponType = selfPlayer.GetItemList().GetWeaponLevel();	
	if (!selfPlayer.GetItemList().isEquipHaveItem(itempart_weapon)
		|| (requiredWeaponType > 0 && requiredWeaponType != curWeaponType))
		return skill_useable_result_no_required_weapon;
	
	//需要消耗物品
	if(pSkill->IsCostItem())
	{
		int itemKeys[ITEM_KEY_NUM + 1];
		pSkill->GetItemKeys(itemKeys);
		if( selfPlayer.GetItemList().HaveNormalItem(itemKeys[0], itemKeys[1], itemKeys[2], itemKeys[3]) < itemKeys[ITEM_KEY_NUM] )
			return skill_useable_result_no_required_item;
	}

	//判断技能消耗是否足够
	enumSkillUseableResult costResult = skill_useable_result_unknown;
	if (FALSE == self.Cost(pSkill, TRUE, &costResult))
		return costResult;

	//根据技能类型作进一步判断
	if (att_target_only & skillTargetType)//单攻
	{
		if (IsValidNpc(targetNpcIndex))
		{
			int	relation = NpcSet.GetRelation(selfNpcIndex, targetNpcIndex);
			SkillType skillType = pSkill->GetSkillType();
			
			if (targetSelf || IsCastSelf(relation, skillTargetType))//对自身施放的技能
			{
				if (!(att_target_self & skillTargetType))
				{
					return skill_useable_result_invalid_target;
				}
			}
			else//对他人施放的技能
			{				
				KNpc& targetNpc = Npc[targetNpcIndex];
				
				//对死亡玩家施放
				if(skillTargetType & att_target_deathplayer)
				{
					if(kind_player == targetNpc.m_Kind && kind_player == self.m_Kind && targetNpc.m_Doing == do_revive)
						return skill_useable_result_ok;
					else
						return skill_useable_result_invalid_target;
				}
				
				//根据双方关系判断
				switch(relation)
				{			
				case relation_enemy:
					if(kind_player == targetNpc.m_Kind)
					{
						if(!(skillTargetType & att_target_enemyplayer))
							return skill_useable_result_invalid_target;
					}
					else
					{
						if(!(skillTargetType & att_target_enemynpc || skillTargetType & att_target_enemyobj))
							return skill_useable_result_invalid_target;
					}
					break;					
				case relation_ally:
					if(kind_player ==targetNpc.m_Kind)
					{
						if(!(skillTargetType & att_target_allyplayer))
							return skill_useable_result_invalid_target;
					}
					else
					{
						if(!(skillTargetType & att_target_allynpc || skillTargetType & att_target_allyobj))
							return skill_useable_result_invalid_target;
					}
					break;
				case relation_dialog:
					return skill_useable_result_invalid_target;
				default:
					break;
				}

				//PK保护
				if ((selfNpcIndex != targetNpcIndex) && (self.IsPlayer() || self.IsCreature() || self.IsEmployee()) && (targetNpc.IsPlayer() || targetNpc.IsCreature() || targetNpc.IsEmployee()))
				{
					int pkProtectLevel = ConfigManager::Singleton().GetGlobalVariable(global_var_pk_protect_level);
					if ((skillTargetType & att_target_enemyplayer) && (self.GetLevel() < pkProtectLevel || targetNpc.GetLevel() < pkProtectLevel))
						return skill_useable_result_pk_protection;
				}

				//是否有阻挡
				if (!pSkill->CanTravTo(selfNpcIndex, targetNpcIndex))
				{
					return skill_useable_result_behind_barrier;
				}
			}
		}
		else
		{
			if (!(att_target_self & skillTargetType))
			{
				return skill_useable_result_no_target;
			}
		}
	}
	else//群攻
	{
		switch(pSkill->GetSkillStyle())
		{
		case SKILL_SS_AddNearTrap:
		case SKILL_SS_NearMultiAttack:
			{
			}
			break;
		default:
			{
				//是否有阻挡
				if (!pSkill->CanTravToTargetPos(selfNpcIndex, mouseX, mouseY))
				{
					return skill_useable_result_behind_barrier;
				}
			}
			break;
		}
	}

	return skill_useable_result_ok;
}