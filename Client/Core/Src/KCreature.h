//////////////////////////////////////////////////////////////////////////
// 被召唤出来的东西
//

#ifndef _INC_KCREATURE_H_
#define _INC_KCREATURE_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//////////////////////////////////////////////////////////////////////////
// 记录召唤兽的基本信息

typedef struct SummonParam_tag
{
	int nSkillID;
	int nLevel;
	int nSummonerIdx;
	int nCreatureIdx;
	int nCropseAddDamageScale;
}SUMMONPARAM;

//////////////////////////////////////////////////////////////////////////
//

class KNpc;

class KCreature
{
	friend class KPlayer;
	
public:
	KCreature();
	// 还活这么?
	void Summon(const SUMMONPARAM& Param);
	BOOL inline IsALive(void) const 
	{
		return ((m_CreatureNpc != NULL));
	}
	inline KNpc* GetCreatureNpc(void){ return m_CreatureNpc; }
	// 是否已经召唤过了
	BOOL inline IsExist(int nSkillID) const
	{
		return (m_nSkillID == nSkillID);
	}
	
#ifdef _SERVER

	void AddSkillExp(void); //增加技能点数一次一点
	void SyncCreature(void);
	void ProcessAI(void);
	bool MarkToPlayer(int nNpcIdx);
	int	 GetMark() { return m_MarkNpcIdx; }
	void Reset();//重置

#endif
	void Dismiss(void);
	int m_nLastCanSummonTime;

#ifdef _SERVER
	void SetAIState(bool bOn);

	void SetCanUseSkillFlag(bool bFlag)
	{
		m_CanUseSkillFlag = bFlag;
	}
#endif

private:
#ifdef _SERVER
	bool IsTargetValid(int nTargetIdx);
	int	 SelectUsableSkill(int nTargetIdx);
	void ClearMark();
#endif

private:
	KNpc* m_CreatureNpc; //召唤兽(自己)
	KNpc* m_SummonerNpc; //召唤师(主人)

#ifdef _SERVER
	int m_nLevel;
	bool	m_IsAIOn;
	bool	m_CanUseSkillFlag;
	int	m_MarkNpcIdx;	// 拥有当前召唤兽标记的npc
#endif

public:
	int m_nSkillID; // 标示召唤兽类别(用什么技能召唤出来的)		
};

#ifdef _SERVER

inline void KCreature::SetAIState(bool bOn)
{
	m_IsAIOn = bOn;
}

#endif

#endif