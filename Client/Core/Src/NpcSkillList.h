//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-9-4 15:14
//      File_base        : NpcSkillList
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
#ifndef _NpcSkillList_h
#define	_NpcSkillList_h

#include "SkillDef.h"

class NpcSkillInfo
{
public:
	NpcSkillInfo()
	{
		Clear();
	}

	inline void Clear()
	{
		m_SkillId = INVALID_SKILL_ID;	
		m_NextCastTime = 0;
		m_SpecialData = 0;
		m_CoolDownTime = DEF_COOLDOWN_TIME;
		m_bClearCooling = false;

		m_CostVal = 0;
		m_ExplodeProb = 0;
		m_MaxAttackTarget = 0;
		m_ExplodeDamage = 0;
		m_CastSpeedEnhance = 0;
		memset(&m_DamageInfo, 0, sizeof(m_DamageInfo));

		m_ThreatValue = 0;
		m_ThreatPercentage = 100;
	}

	int				m_SkillId;
	int				m_SkillLevel;
	int				m_Status;
	int				m_PreStatus;
	int				m_CoolDownTime;
	int				m_PreCoolDownTime;
	DWORD			m_NextCastTime;
	int				m_SpecialData;		// 对于召唤技，用于召唤兽的外观
	short int		m_CostVal;
	short int		m_ExplodeProb;
	short int		m_ExplodeDamage;
	short int		m_MaxAttackTarget;
	short int		m_CastSpeedEnhance;
	SkillDamageInfo	m_DamageInfo[dot_end];
	int				m_ThreatValue;			//仇恨值
	int				m_ThreatPercentage;		//仇恨获得百分比
	

	bool		m_bClearCooling;

	static const int DEF_COOLDOWN_TIME;
};

NpcSkillInfo*	AllocSkillInfo();
void			FreeSkillInfo(NpcSkillInfo *ptr);

class NpcSkillList
{
public:
	class Iterator
	{
	public:
		friend class NpcSkillList;
		Iterator() : m_idx(0) {};

	private:
		int	m_idx;
	};

	NpcSkillList()
	{
		memset(&m_NpcSkillInfo, 0, sizeof(m_NpcSkillInfo));

		m_CommonCoolDownTimer=0;   //Invalid value

#ifndef _SERVER
		m_SwitchSkillId = INVALID_SKILL_ID;
#endif
	}

	~NpcSkillList();

	NpcSkillList(const NpcSkillList &rhs)
	{
		*this = rhs;
	}

	NpcSkillList& operator= (const NpcSkillList &rhs);

	bool	AddSkillEx(int nSkillId, int nLevel, int nStatus);
	bool	LevelUpTo(int nSkillId, int nLevel);
	
	int		FindSkill(int nSkillId) const;	
	void	SetNpcIdx(int nIdx);
	bool	CanCast(int nSkillID);
	void	Clear();
	bool	LoadSkillData(BYTE	*pInBuf, int nSize);

	int		GetIdByIdx(int nIdx) const;
	int		GetLevelByIdx(int nIdx) const;
	void	CoolDown(int nSkillID);
	void	ClearCoolDown(int nSkillId);
	int		GetStatusByIdx(int nIdx) const;
	int		GetSpecDataByIdx(int nIdx) const;
	void	ChangeStatus(int nIdx, int nStatus);
	void	RestoreStatus(int nIdx);
	void	ChangeCollDownTime(int nIdx, int nNewTime);
	void	RestoreCoolDownTime(int nIdx);
	bool	IsCooling( int nIdx );
	bool	IsClearCooling( int nIdx );	
	int		NextSkillIdx(Iterator &iter);
	bool	RemoveSkillEx(int nSkillId);

	int		GetPrivateCost(int nSkillId);
	int		GetPrivateCostByIdx(int nIdx);
	void	ChangeCost(int nSkillId, int nNewCost);
	void	ChangeCostByIdx(int nIdx, int nNewCost);	

	int		GetCastSpeedEnhance(int nSkillId);
	int		GetCastSpeedEnhanceByIdx(int nIdx);
	void	ChgCastSpeedEnhance(int nSkillId, int nVal);
	void	ChgCastSpeedEnhanceByIdx(int nIdx, int nVal);

	void    SetComCoolTime(unsigned long iInterval);         //设置全局冷却时间	
	int		GetCurSameSubSkillId(int nSkillId);

	void	SortBySkillId();//按照技能ID排序
	
private:
	bool	AddSkill(int nSkillId, int nLevel, int nStatus);
	void	AddSkill(int nIdx, int nSkillId, int nLevel, int nStatus);
	bool	RemoveSkill(int nSkillId);
	void	RepSkillsOnLevelup(int nCurIdx, int nTargetId);
	bool	IsIndexValid(int nIdx) const;
	int		GetEmptyPos() const;
	NpcSkillInfo*	GetSkillInfoByIdx(int nIdx) const;

	bool	LoadSkillDataVersion1(BYTE *pInBuf, int nSize);
	bool	LoadSkillDataVersion2(BYTE *pInBuf, int nSize);
#ifdef _SERVER
	int		SaveSkillDataVersion1(BYTE *pOutBuf);
	int		SaveSkillDataVersion2(BYTE *pOutBuf);
#endif

#ifndef _SERVER
public:
	int		GetSubSkillIds(int nMailSkillId, int *pOutArray, int nArrayCapacity);
	int		GetMainSkillIds(int *pOutArray, int nArrayCapacity);
	void	SwitchSkill(int nSkillId);
	int		GetSwitchSkillId();
	DWORD	GetLeftCDTimeByIdx(int nIdx) const;
#endif

#ifdef _SERVER
public:
	void	SyncSkillInfo(int nIdx, int nOperaton);
	void	CastAllPassiveSkill();
	void	CastAllValueSkill();
	int		SaveSkillData(BYTE	*pOutBuf);
	void	NotifyAddSkill(int nSkillId, int nLevel, int nStatus);
	void	NotifyRemoveSkill(int nSkillId);

	SkillDamageInfo* GetPrivateDamInfo(int nSkillId);
	SkillDamageInfo* GetPrivateDamInfoByIdx(int nIdx);

	int		GetExplodeProb(int nSkillId);
	int		GetExplodeProbByIdx(int nIdx);
	void	ChangeExplodeProbByIdx(int nIdx, int nVal);

	int		GetMaxAttackTarget(int nSkillId);
	int		GetMaxAttackTargetByIdx(int nIdx);
	void	ChgMaxAttackTargetByIdx(int nIdx, int nVal);
	
	int		GetExplodeDamage(int nSkillId);
	int		GetExplodeDamageByIdx(int nIdx);
	void	ChgExplodeDamageByIdx(int nIdx, int nVal);

	int		GetThreatValueByIdx(int nIdx);
	int		GetThreatPercentageByIdx(int nIdx);
	void	SetThreatValueByIdx(int nIdx, int newValue);
	void	SetThreatPercentageByIdx(int nIdx, int newPercentage);

protected:
	void	SkillLevelUpSync(int nSkillId, int nTargetLevel);
	void	UpdateValueSkillEffect(int nSkillId, int nOriLevel, int nCurLevel);
	void	CastPassiveSkill(int nSkillId, int nLevel);

#endif

private:
	int				m_NpcIndex;
	NpcSkillInfo	*m_NpcSkillInfo[MAX_NPCSKILL];

	enum {	__max_skill_count = MAX_NPCSKILL };

	unsigned long   m_CommonCoolDownTimer;            //全局CoolDown计时
#ifndef _SERVER
	int    	        m_SwitchSkillId;
#endif	
};

inline NpcSkillList::~NpcSkillList()
{
	Clear();
}

inline bool	NpcSkillList::IsIndexValid(int nIdx) const
{
	return nIdx >= 0 && nIdx < __max_skill_count;
}

inline void NpcSkillList::SetNpcIdx(int nIdx)
{
	m_NpcIndex = nIdx;
}

inline int	NpcSkillList::FindSkill(int nSkillId) const
{
	for(int nLoop = 0; nLoop < __max_skill_count; ++nLoop)
	{
		if(m_NpcSkillInfo[nLoop])
		{
			if(m_NpcSkillInfo[nLoop]->m_SkillId == nSkillId)
				return nLoop;
		}
	}

	return INVALID_SKILL_INDEX;
}

inline int NpcSkillList::GetIdByIdx(int nIdx) const
{
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	return pInfo ? pInfo->m_SkillId : INVALID_SKILL_ID;
}

inline int NpcSkillList::GetLevelByIdx(int nIdx) const
{
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	return pInfo ? pInfo->m_SkillLevel : INVALID_SKILL_LEVEL;
}

inline int	NpcSkillList::GetStatusByIdx(int nIdx) const
{
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	return pInfo ? pInfo->m_Status : skill_status_inactive;
}

inline int NpcSkillList::GetSpecDataByIdx(int nIdx) const
{
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	return pInfo ? pInfo->m_SpecialData : DEFAULT_SKILL_SPECDATA;
}

inline void NpcSkillList::ChangeStatus(int nIdx, int nStatus)
{
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	if(pInfo)
	{
		if(nStatus > skill_status_begin && nStatus < skill_status_end)
		{
			pInfo->m_Status = nStatus;

#ifdef _SERVER
			SyncSkillInfo(nIdx, skill_ope_chgstatus);
#endif
		}
	}
}

inline void	NpcSkillList::RestoreStatus(int nIdx)
{
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	if(pInfo)
	{
		pInfo->m_Status = pInfo->m_PreStatus;

#ifdef _SERVER
		SyncSkillInfo(nIdx, skill_ope_chgstatus);
#endif
	}
}

inline void NpcSkillList::ChangeCollDownTime(int nIdx, int nNewTime)
{
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	if(pInfo)
	{
		pInfo->m_PreCoolDownTime = pInfo->m_CoolDownTime;
		pInfo->m_CoolDownTime = nNewTime;

#ifdef _SERVER
		SyncSkillInfo(nIdx, skill_ope_chgcdtime);
#endif
	}
}

inline void NpcSkillList::RestoreCoolDownTime(int nIdx)
{
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	if(pInfo)
	{
		pInfo->m_CoolDownTime = pInfo->m_PreCoolDownTime;

#ifdef _SERVER
		SyncSkillInfo(nIdx, skill_ope_chgcdtime);
#endif
	}
}

inline NpcSkillInfo* NpcSkillList::GetSkillInfoByIdx(int nIdx) const
{
	if( IsIndexValid(nIdx) )
		return m_NpcSkillInfo[nIdx];
	else
		return NULL;
}

inline int	NpcSkillList::GetEmptyPos() const
{
	for(int nLoop = 0; nLoop < __max_skill_count; ++nLoop)
	{
		if(NULL == m_NpcSkillInfo[nLoop])
			return nLoop;
	}

	return INVALID_SKILL_INDEX;
}

inline int NpcSkillList::GetPrivateCost(int nSkillId)
{
	int	nIdx = FindSkill(nSkillId);

	return GetPrivateCostByIdx(nIdx);
}

inline int NpcSkillList::GetPrivateCostByIdx(int nIdx)
{
	if( IsIndexValid(nIdx) && m_NpcSkillInfo[nIdx] )
		return m_NpcSkillInfo[nIdx]->m_CostVal;
	else
		return 0;	
}

#ifdef _SERVER
inline SkillDamageInfo* NpcSkillList::GetPrivateDamInfo(int nSkillId)
{
	int nIdx = FindSkill(nSkillId);
	return GetPrivateDamInfoByIdx(nIdx);
}
#endif

#ifdef _SERVER
inline SkillDamageInfo* NpcSkillList::GetPrivateDamInfoByIdx(int nIdx)
{
	if( IsIndexValid(nIdx) && m_NpcSkillInfo[nIdx] )
		return m_NpcSkillInfo[nIdx]->m_DamageInfo;
	else
		return NULL;
}
#endif

#ifdef _SERVER
inline void NpcSkillList::ChangeExplodeProbByIdx(int nIdx, int nVal)
{
	if( IsIndexValid(nIdx) && m_NpcSkillInfo[nIdx])
		m_NpcSkillInfo[nIdx]->m_ExplodeProb = nVal;
}
#endif

#ifdef _SERVER
inline int	NpcSkillList::GetExplodeProb(int nSkillId)
{
	int nIdx = FindSkill(nSkillId);
	return GetExplodeProbByIdx(nIdx);
}
#endif

#ifdef _SERVER
inline int	NpcSkillList::GetExplodeProbByIdx(int nIdx)
{
	if( IsIndexValid(nIdx) && m_NpcSkillInfo[nIdx])
		return m_NpcSkillInfo[nIdx]->m_ExplodeProb;
	else
		return 0;	
}
#endif

#ifdef _SERVER
inline void	NpcSkillList::ChgMaxAttackTargetByIdx(int nIdx, int nVal)
{
	if( IsIndexValid(nIdx) && m_NpcSkillInfo[nIdx])
		m_NpcSkillInfo[nIdx]->m_MaxAttackTarget = nVal;
}
#endif

#ifdef _SERVER
inline int NpcSkillList::GetMaxAttackTarget(int nSkillId)
{
	int	nIdx = FindSkill(nSkillId);
	return GetMaxAttackTargetByIdx(nIdx);
}
#endif

#ifdef _SERVER
inline int NpcSkillList::GetMaxAttackTargetByIdx(int nIdx)
{
	if( IsIndexValid(nIdx) && m_NpcSkillInfo[nIdx])
		return m_NpcSkillInfo[nIdx]->m_MaxAttackTarget;
	else
		return 0;
}
#endif

inline void NpcSkillList::ChangeCost(int nSkillId, int nNewCost)
{
	int nIdx = FindSkill(nSkillId);
	ChangeCostByIdx(nIdx, nNewCost);
}

#ifndef _SERVER
inline void NpcSkillList::SwitchSkill(int nSkillId)
{
	if(m_SwitchSkillId == nSkillId)
		m_SwitchSkillId = INVALID_SKILL_ID;
	else
		m_SwitchSkillId = nSkillId;
}

inline int NpcSkillList::GetSwitchSkillId()
{
	return m_SwitchSkillId;
}
#endif 

#ifdef _SERVER
inline int NpcSkillList::GetThreatValueByIdx(int nIdx)
{
	return m_NpcSkillInfo[nIdx]->m_ThreatValue;
}

inline int NpcSkillList::GetThreatPercentageByIdx(int nIdx)
{
	return m_NpcSkillInfo[nIdx]->m_ThreatPercentage;
}

inline void NpcSkillList::SetThreatValueByIdx(int nIdx, int newValue)
{
	m_NpcSkillInfo[nIdx]->m_ThreatValue = newValue;
}

inline void	NpcSkillList::SetThreatPercentageByIdx(int nIdx, int newPercentage)
{
	m_NpcSkillInfo[nIdx]->m_ThreatPercentage = newPercentage;
}
#endif

#ifdef _SERVER
inline int NpcSkillList::GetExplodeDamage(int nSkillId)
{
	int nIdx = FindSkill(nSkillId);
	return GetExplodeDamageByIdx(nIdx);
}

inline int	NpcSkillList::GetExplodeDamageByIdx(int nIdx)
{
	if( IsIndexValid(nIdx) && m_NpcSkillInfo[nIdx])
		return m_NpcSkillInfo[nIdx]->m_ExplodeDamage;
	else
		return 0;
}

inline void NpcSkillList::ChgExplodeDamageByIdx(int nIdx, int nVal)
{
	if( IsIndexValid(nIdx) && m_NpcSkillInfo[nIdx])
		m_NpcSkillInfo[nIdx]->m_ExplodeDamage = nVal;
}
#endif

inline int	NpcSkillList::GetCastSpeedEnhance(int nSkillId)
{
	int nIdx = FindSkill(nSkillId);
	return GetCastSpeedEnhanceByIdx(nIdx);
}

inline int	NpcSkillList::GetCastSpeedEnhanceByIdx(int nIdx)
{
	if( IsIndexValid(nIdx) && m_NpcSkillInfo[nIdx] )	
		return m_NpcSkillInfo[nIdx]->m_CastSpeedEnhance;
	else
		return 0;
}

inline void NpcSkillList::ChangeCostByIdx(int nIdx, int nNewCost)
{
	if( IsIndexValid(nIdx) && m_NpcSkillInfo[nIdx] )
	{
		m_NpcSkillInfo[nIdx]->m_CostVal = nNewCost;

	#ifdef _SERVER
		SyncSkillInfo(nIdx, skill_ope_chgcost);
	#endif	
	}
}

inline void	NpcSkillList::ChgCastSpeedEnhance(int nSkillId, int nVal)
{
	int nIdx = FindSkill(nSkillId);
	ChgCastSpeedEnhanceByIdx(nIdx, nVal);
}

inline void NpcSkillList::ChgCastSpeedEnhanceByIdx(int nIdx, int nVal)
{
	if( IsIndexValid(nIdx) && m_NpcSkillInfo[nIdx] )
	{
		m_NpcSkillInfo[nIdx]->m_CastSpeedEnhance = nVal;

#ifdef _SERVER
		SyncSkillInfo(nIdx, skill_ope_chgcastspeed);
#endif
	}
}

#endif // #ifdef _NpcSkillList_h