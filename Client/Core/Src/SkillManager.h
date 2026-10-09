//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-9-5 10:31
//      File_base        : SkillManager
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
#ifndef _SkillManager_h
#define _SkillManager_h

#ifdef WIN32
#pragma	warning(disable:4786)
#endif

#include <map>
#include <vector>

using std::map;
using std::vector;

#include "SkillDef.h"
#include "MagicDef.h"

class KSkill;

class SkillManager
{
public:
	SkillManager();
	~SkillManager();

public:
	bool	Init();
	bool	IsSameSubSkill(int nSubId1, int nSubId2);
	bool    IsCommonCoolDown(int nSkillId);
	bool	IsMainSkill(int nSkillId);
	bool	IsSubSkill(int nSkillId);
	bool	IsHiddenSkill(int nSkillId);
	bool	IsValueSkill(int nSkillId);
	bool	IsPassiveLevelupSkill(int nSkillId);
	int		GetSkillInitStatus(int nSkillId);
	KSkill*  GetSkill(int nId, int nLevel);
	KSkill*  GetSkill(int skillId);
	int		GetMaxLevel(int nSkillId);
	bool	IsAddToListWhenInit(int nSkillId);

	const SkillChgCond*	GetSkillCond(int nSkillId, int nLevel) const;
	
	int		GetMainSkillIds(int nRoleType, int nRoleSkillSeries, int *pOutArray, int nArrayCapacity);
	int		GetInitSubSkillIds(int nMainSkillId, int *pOutArray, int nArrayCapacity);
	int		GetSubSkillIdByLvl(int nSkillId, int nLevel);
	int		GetSubSkillLevel(int nSkillId);
	bool	CanUpdateTo(int nNpcIdx, int nSkillId, int nLevel,bool bShow=false);
	bool	IsIdValid(int nSkillId) const;
	int		GetInvokeHideSkill(int nSkillId) const;
	
private:
	void	InitSkillChgCond();
	bool	LoadSkillRelation(const char *szFile);
	bool	LoadSkillSetting(const char *szFile);
	bool	LoadSkillChangeCond(const char *szFile);
	bool	IsSkillValid(int nSkillId, int nLevel) const;
	bool	IsLevelValid(int nLevel) const;
	KSkill*	InstanceSkill(int nSkillId, int nLevel);
	void	ClearInstancedSkills();
	void	StrToIntVec(char *szStr, const char *szTag, vector<int> &viRst);

#ifdef _SERVER
public:
	void	ConsumeUpdateCost(int nNpcIdx, int nSkillId, int nLevel);
	int		GetLevelUpBuff(int nSkillId);
	void	GetValueSkillMagicVal(int nSkillId, int nLevel, PMagicData pData);

private:
	bool	LoadValueSkillData(const char *szFile);
#endif

	// Functions used by client only
#ifndef _SERVER
public:
	int		GetMainSkillId(int nSubSkillId);	
	bool	IsSwitchSkill(int nSkillId);
	void    GenDesc(int nSkillId,int nLevel,char * lpBuff,int nBuffSize,bool bDisableStyle=false);
	void    GenDescWithSize(int nSkillId, int nLevel, char* lpBuff, int nBuffSize);
	void	GetSkill(const char* name);
	KSkill*	GetSkill(int skillId, int* nLevel);
#endif

private:
	typedef struct _tagSkillInfo
	{
		int		nTabFileRowNo;
		int		nSkillSeries;
		int		nInitStatus;
		int		nSubSkillLevel;
		int		nMaxLevel;
		int		IsAddToListWhenInit;
		int     IsCommonCoolDown;    //是否引起公共冷却
		int		SkillType;
		int		InvokeHideSkillId;

#ifdef _SERVER
		int		BuffIdOnLevelUp;
#endif

	} SkillInfo;

	typedef	vector<int>					SUBSKILLSCONT;
	typedef map<int, int>				MAINIDTOROWNO;
	typedef map<int, SUBSKILLSCONT>		MAINIDTOSUBIDS;
	typedef map<int, int>				MAINIDTOINDEX;
	typedef map<int, SUBSKILLSCONT>		SUBIDTONEXTIDS;

private:
	MAINIDTOSUBIDS		m_MainIdToSubIds;
	SUBIDTONEXTIDS		m_SubIdToNextIds;
#ifdef _SERVER
	MAINIDTOINDEX		m_ValueSkillIdToDataIdx;
#endif
	
	int					m_MainSkillIds[MAX_SKILL_SERIES][MAX_MAINSKILL_PER_SERIES];
	SkillInfo			m_SkillInfo[MAX_SKILL];
	KSkill				*m_pSkills[MAX_SKILL][MAX_SKILL_LEVEL];
	SkillChgCond		m_SkillChgCond[MAX_SKILL][MAX_SKILL_LEVEL];

#ifdef _SERVER
	MagicData			m_ValueSkillData[MAX_VALUE_SKILL][MAX_SKILL_LEVEL];
#endif
};

extern SkillManager		g_SkillManager;

inline SkillManager::SkillManager()
{
	memset(m_SkillInfo, 0, sizeof(m_SkillInfo));
	memset(m_pSkills, 0, sizeof(m_pSkills));
	memset(m_MainSkillIds, 0, sizeof(m_MainSkillIds));

#ifdef _SERVER
	memset(m_ValueSkillData, 0, sizeof(m_ValueSkillData));
#endif

	InitSkillChgCond();
}

inline void SkillManager::InitSkillChgCond()
{
	SkillChgCond cond = 
			{ 
				-1, -1, -1, -1, -1, -1, -1, -1, 
				{ -1, -1, -1, -1 },
				{ -1, -1, -1, -1 }
			};
 
	for(int i = 0; i < MAX_SKILL; ++i)
	{
		for(int j = 0; j < MAX_SKILL_LEVEL; ++j)
			m_SkillChgCond[i][j] = cond;
	}
}

inline bool	SkillManager::IsSameSubSkill(int nSubId1, int nSubId2)
{
	if( IsIdValid(nSubId1) && IsIdValid(nSubId2) )
	{
		return	  m_SkillInfo[nSubId1 - 1].nSkillSeries > 0
			   && m_SkillInfo[nSubId1 - 1].nSkillSeries == m_SkillInfo[nSubId2 - 1].nSkillSeries;
	}
	else
	{
		return false;
	}
}

inline bool SkillManager::IsMainSkill(int nSkillId)
{
	if( IsIdValid(nSkillId) )
	{
		return MAIN_SKILL_SERIES == m_SkillInfo[nSkillId - 1].nSkillSeries;
	}
	else
	{
		return false;
	}
}

inline bool SkillManager::IsCommonCoolDown(int nSkillId)
{
    if (IsIdValid(nSkillId))
	{
		return 1 == m_SkillInfo[nSkillId - 1].IsCommonCoolDown;
	}
	else
	{
		return false;
	}
}

inline bool SkillManager::IsHiddenSkill(int nSkillId)
{
	if( IsIdValid(nSkillId) )
		return HIDDEN_SKILL_SERIES == m_SkillInfo[nSkillId - 1].nSkillSeries;
	else
		return false;
}

inline int SkillManager::GetSkillInitStatus(int nSkillId)
{
	if( IsIdValid(nSkillId) )
	{
		return m_SkillInfo[nSkillId - 1].nInitStatus;
	}
	else
	{
		return skill_status_inactive;
	}
}

inline int	SkillManager::GetSubSkillLevel(int nSkillId)
{
	if( IsIdValid(nSkillId) )
		return m_SkillInfo[nSkillId - 1].nSubSkillLevel;
	else
		return INVALID_SKILL_LEVEL;	
}

inline bool	SkillManager::IsSubSkill(int nSkillId)
{
	if( IsIdValid(nSkillId) )
		return m_SkillInfo[nSkillId - 1].nSkillSeries > 0;
	else
		return false;
}

inline bool	SkillManager::IsValueSkill(int nSkillId)
{
	if( IsIdValid(nSkillId) )
		return VALUE_SKILL_TYPE == m_SkillInfo[nSkillId - 1].SkillType;
	else
		return false;
}

inline bool	SkillManager::IsPassiveLevelupSkill(int nSkillId)
{
	if( IsIdValid(nSkillId) )
		return PASSIVELEVELUP_SKILL_TYPE == m_SkillInfo[nSkillId - 1].SkillType;
	else
		return false;
}

inline const SkillChgCond*	SkillManager::GetSkillCond(int nSkillId, int nLevel) const
{
	if( IsSkillValid(nSkillId, nLevel) )
		return &m_SkillChgCond[nSkillId - 1][nLevel - 1];
	else
		return NULL;
}

inline bool	SkillManager::IsIdValid(int nSkillId) const
{
	return nSkillId > 0 && nSkillId <= MAX_SKILL;
}

inline bool SkillManager::IsLevelValid(int nLevel) const
{
	return nLevel > 0 && nLevel <= MAX_SKILL_LEVEL;
}

inline bool	SkillManager::IsSkillValid(int nSkillId, int nLevel) const
{
	if( IsIdValid(nSkillId) )
	{
		if(nLevel > 0 && nLevel <= m_SkillInfo[nSkillId - 1].nMaxLevel)
			return true;
	}
	
	return false;
}

inline int	SkillManager::GetMaxLevel(int nSkillId)
{
	if( IsIdValid(nSkillId) )
		return m_SkillInfo[nSkillId - 1].nMaxLevel;
	else
		return 0;
}

inline KSkill* SkillManager::GetSkill(int skillId)
{
	return GetSkill(skillId, 1);
}

inline bool	SkillManager::IsAddToListWhenInit(int nSkillId)
{
	if( IsIdValid(nSkillId) )
	{
		return 1 == m_SkillInfo[nSkillId - 1].IsAddToListWhenInit;
	}
	else
	{
		return false;
	}
}

#ifndef _SERVER
inline bool	SkillManager::IsSwitchSkill(int nSkillId)
{
	if( IsIdValid(nSkillId) )
		return SWITCH_SKILL_TYPE == m_SkillInfo[nSkillId - 1].SkillType;
	else
		return false;
}
#endif

#ifdef _SERVER
inline int SkillManager::GetLevelUpBuff(int nSkillId)
{
	if( IsIdValid(nSkillId) )
		return m_SkillInfo[nSkillId - 1].BuffIdOnLevelUp;
	else
		return INVALID_BUFF_ID;
}
#endif

inline int SkillManager::GetInvokeHideSkill(int nSkillId) const
{
	if( IsIdValid(nSkillId) )
		return m_SkillInfo[nSkillId - 1].InvokeHideSkillId;
	else
		return INVALID_SKILL_ID;
}

#endif // _SkillManager_h