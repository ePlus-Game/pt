//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-26
//      File_base        : ai_controller
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : NPC控制器
//
//////////////////////////////////////////////////////////////////////

#ifndef _NPC_CONTROLLER_H_
#define _NPC_CONTROLLER_H_

#include "GameDataDef.h"
#include "ai_threat.h"

//NPC模式
enum enumAIMode
{	
	ai_mode_defend = 0,			//防御
	ai_mode_aggressive,			//主动
	ai_mode_passive,			//被动
	
	ai_mode_count
};

//NPC事件
enum enumNpcEvent
{
	NpcEvent_Revive = 0,	//重生
	NpcEvent_Death,			//死亡
	NpcEvent_BeUsedSkill,	//被施放技能

	NpcEvent_Count
};

//NPC控制器
class NpcController
{
public:

	NpcController();
	~NpcController();
	
	void Init(int npcIndex);//初始化
	void Active();//活动一次
	void OnEvent(enumNpcEvent npcEvent, void* eventParam);//发生事件

	ThreatMonitor& GetThreatMonitor();//得到威胁监视器

	bool IsActive() const;//得到是否激活
	void SetActive(bool active);//设置是否激活
	void SetForceTargetNpc(int npcIndex);//设置强制攻击的NPC
	void SetFollowNpc(int npcIndex, int followWaitTime);//设置要跟随的NPC	
	void SetSkillStrategyList(const PSkillStrategy skillStrategyList, int count);//设置技能策略列表
	void SetEnableReturn(bool enable);//设置启用返回
	void SetAggressive(bool aggressive);//设置具有攻击性
	void SetEnableChase(bool enable);//设置追击
	void SetWanderRestPercentage(int precentage);//设置漫步休息概率
	void SetStayAround(bool stayAround);//设置保持在出生点周围
	void SetWander(bool wander);//设置是否漫步
	void SetMode(enumAIMode mode);//设置AI模式

private:

	void SetState(enumNpcState state);//设置状态

	void ProcessStateGuard();
	void ProcessStateWander();
	void ProcessStateFollow();
	void ProcessStateCombat();
	void ProcessStateFlee();
	void ProcessStatePatrol();
	void ProcessStateTrapped();
	void ProcessStateReturn();
	void ProcessStateFollowWait();

	void ValidateEnemy();//检查敌人的合法性
	int GetTargetNpc();//取得当前攻击的目标
	int GetNearestNpc(int nRelation);//取得最近的NPC
	unsigned long GetCurrentTime();//取得当前时间
	int GetRandomEnemy();//取得随机目标
	int GetSkillStrategyCount();//得到技能策略计数
	const PSkillStrategy GetSkillStrategyList();//得到技能策略列表
	enumSkillUseableResult IsSkillUseable(int skillId, int targetNpcIndex);//判断技能是否可用
	void RefreshReturnTime();//刷新返回时间
	
	bool m_IsActive;//AI是否激活
	int m_NpcIndex;//所控制的NPC
	int m_TargetNpcIndex;//攻击的NPC
	int m_ForceTargetNpcIndex;//强制攻击的NPC
	int m_FollowNpcIndex;//跟随NPC的Index
	DWORD m_FollowNpcId;//跟随的NPC的Id
	FSGUID m_FollowPlayerGuid;//跟随的Player的Guid
	int m_FollowWaitTime;//跟随目标丢失等待时间
	enumNpcState m_NpcState;//所控制的NPC的状态
	ThreatMonitor m_ThreatMonitor;//威胁监视器
	int m_SkillStrategyCount;//技能策略计数
	SkillStrategy m_SkillStrategyList[MAX_SKILL_STRATEGY_LIST_LENGTH];//技能策略列表
	enumNpcState m_OriginalState;//原状态	
	int m_BirthPosX;//出生点位置X
	int m_BirthPosY;//出生点位置Y
	int m_OriginalPosX;//原位置X
	int m_OriginalPosY;//原位置Y
	unsigned long m_ReturnTime;//返回时刻
	unsigned long m_ForceInstantReturnTime;//强制返回时刻
	unsigned long m_NextWanderChangeTime;//下一次漫步改变时刻
	bool m_IsWanderRested;//是否漫步休息过
	int m_WanderRestPercentage;//漫步休息概率（%）
	enumAIMode m_Mode;//AI模式
	bool m_EnableReturn;//启用返回
	bool m_EnableChase;//启用追击
	bool m_IsWander;//是否漫步
	bool m_IsStayAround;//呆在在出生点周围
	unsigned long m_NpcReturnBuff;//NPC返回BUFF（用于返回状态结束时摘除）
	unsigned long m_NextAiTime;//下一次AI执行时间
};

inline void NpcController::SetActive(bool active)
{
	m_IsActive = active;

	if (!m_IsActive)
		m_ThreatMonitor.Reset();
}

inline bool NpcController::IsActive() const
{
	return m_IsActive;
}

inline const PSkillStrategy NpcController::GetSkillStrategyList()
{
	return m_SkillStrategyList;
}

inline int NpcController::GetSkillStrategyCount()
{
	return m_SkillStrategyCount;
}

inline ThreatMonitor& NpcController::GetThreatMonitor()
{
	return m_ThreatMonitor;
}

inline void NpcController::SetEnableReturn(bool enable)
{
	m_EnableReturn = enable;
}

inline void NpcController::SetAggressive(bool aggressive)
{
	m_Mode = (aggressive ? ai_mode_aggressive : ai_mode_defend);
}

inline void NpcController::SetEnableChase(bool enable)
{
	m_EnableChase = enable;
}

inline void NpcController::SetWanderRestPercentage(int precentage)
{
	m_WanderRestPercentage = precentage;
}

inline void NpcController::SetStayAround(bool stayAround)
{
	m_IsStayAround = stayAround;
}

inline void NpcController::SetWander(bool wander)
{
	m_IsWander = wander;
}

inline void NpcController::SetMode(enumAIMode mode)
{
	m_Mode = mode;
}

#endif// _NPC_CONTROLLER_H_