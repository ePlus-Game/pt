//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007-07-28
//      File_base        : ai_player_controller
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : Player控制器
//
//////////////////////////////////////////////////////////////////////

#ifndef _PLAYER_CONTROLLER_H_
#define _PLAYER_CONTROLLER_H_

#include "KNpc.h"

//玩家状态
enum enumPlayerState
{
	player_state_none = 0,
	player_state_follow_npc,
	player_state_follow_attack,
	player_state_follow_dialog,
	player_state_pickup_object,	
};

//Player控制器（客户端）
class PlayerController
{
public:

	PlayerController();
	~PlayerController();
	static PlayerController& Singleton();//单件

	void Init();//初始化
	void Active();//活动一次
	void ChangeWorld();//切换世界

	void Stop();//停止
	void UseSelectedSkill(int mouseX, int mouseY, bool targetSelf);//使用一次选中技能
	void FollowAttack(int npcIndex);//跟随攻击
	void FollowNpc(int npcIndex);//跟随
	void FollowDialog(int npcIndex);//跟随对话
	void PickupObject(int objectIndex);//拾取物品
	void SetSelectedSkill(int skillId);//设置选中的技能
	int GetSelectedSkill() const;//得到选中的技能
	void SetNextSkill(int skillId, int mouseX, int mouseY, bool targetSelf);//设置下次施放的技能

private:

	void ProcessNormal();
	void ProcessFollowNpc();
	void ProcessFollowAttack();
	void ProcessFollowDialog();
	void ProcessPickupObject();
	
	bool CanUseSkill();//是否可以使用任何技能	
	void CastSkill(int skillId, int param1, int param2);
	void GoTo(int destX, int destY);
	unsigned long GetCurrentTime();//取得当前时间	
	void StartAutoAttack(CastSkillParam& skillParam);//开始自动攻击
	void StopAutoAttack();//停止自动攻击
	void SendNextSkill(CastSkillParam& skillParam);//发送施放下一个技能的指令
	void DelayAi(unsigned long delayFrame);//延迟AI
	static bool IsCastSelf(int relation, int skillTargetType);//是否应该对自己施法
	static bool GetSkillParam(int selfNpcIndex, int skillId, int targetNpcIndex, int mouseX, int mouseY, bool targetSelf, CastSkillParam* pSkillParam, int* pRequiredDistance);
	static enumSkillUseableResult CheckSkillRequirement(int selfNpcIndex, int skillId, int targetNpcIndex, int mouseX, int mouseY, bool targetSelf, bool showError);
	static enumSkillUseableResult CheckSkillRequirement(int selfNpcIndex, int skillId, int targetNpcIndex, int mouseX, int mouseY, bool targetSelf);

	unsigned long m_NextAiTime;
	int m_NpcIndex;
	enumPlayerState m_PlayerState;
	int m_FollowAttackNpcIndex;
	DWORD m_FollowAttackNpcId;
	int   m_FollowNpcIndex;
	DWORD m_FollowNpcId;
	int m_SelectedSkillId;
	CastSkillParam m_NextSkill;
	int m_PickUpObjectIndex;
	DWORD m_PickUpObjectId;
	int m_FollowDialogNpcIndex;
	DWORD m_FollowDialogNpcId;
	int m_KeepRangeMin;
	int m_KeepRangeMax;
	int m_NextSkillRequiredDistance;
	bool m_InRequiredRange;
};

inline int PlayerController::GetSelectedSkill() const
{
	return m_SelectedSkillId;
}

inline void PlayerController::UseSelectedSkill(int mouseX, int mouseY, bool targetSelf)
{
	SetNextSkill(m_SelectedSkillId, mouseX, mouseY, targetSelf);
}

#endif// _PLAYER_CONTROLLER_H_