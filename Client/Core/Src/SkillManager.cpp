//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-9-5 11:27
//      File_base        : SkillManager
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
#include "SkillManager.h"
#include "KIniFile.h"
#include "KSkills.h"
#include "CoreUtil.h"
#include "CoreRelated.h"

#ifndef _SERVER
#include "CoreShell.h"
#include "QueryInfo.h"
#endif

SkillManager	g_SkillManager;

#define		SEPERATOR	","

SkillManager::~SkillManager()
{
	ClearInstancedSkills();
}

bool SkillManager::Init()
{
	memset(m_SkillInfo, 0, sizeof(m_SkillInfo));
	memset(m_MainSkillIds, 0, sizeof(m_MainSkillIds));
	m_MainIdToSubIds.clear();
#ifdef _SERVER
	memset(m_ValueSkillData, 0, sizeof(m_ValueSkillData));
	m_ValueSkillIdToDataIdx.clear();
#endif
	m_SubIdToNextIds.clear();
	ClearInstancedSkills();
	InitSkillChgCond();

	if( !LoadSkillSetting(SKILL_SETTING_FILE) )
		return false;

	if( !LoadSkillRelation(FILE_SKILLRELATION) )
		return false;

#ifdef _SERVER
	if( !LoadValueSkillData(FILE_MAINSKILL_VALUE) )
		return false;
#endif

	if( !LoadSkillChangeCond(FILE_SKILL_CHGCOND) )
		return false;

#ifndef _SERVER
	g_MisslesSetting.Clear();
	g_MisslesSetting.Load(MISSLES_SETTING_FILE);
#endif

	return true;
}

bool SkillManager::LoadSkillChangeCond(const char *szFile)
{
	KTabFile	tabFile;
	
	if( !tabFile.Load(szFile) )
		return false;

	int nSkillNum = tabFile.GetHeight() - 1;

	if(nSkillNum <= 0)
		return false;

	const	int COL_SIZE = 16;
	const	int VAL_SIZE = 256;
	char	szCol[COL_SIZE];
	char	szVal[VAL_SIZE];

	for(int i = 0; i < nSkillNum; ++i)
	{
		int	nSkillId;
		tabFile.GetInteger(i + 2, "SkillId", 0, &nSkillId);

		_ASSERT( IsIdValid(nSkillId) );

		if( !IsIdValid(nSkillId) )
			continue;

		for(int j = 0; j < MAX_SKILL_LEVEL; ++j)
		{
			sprintf(szCol, "%d", j + 1);
			tabFile.GetString(i + 2, szCol, "", szVal, VAL_SIZE);

			int *pVal = (int*)&m_SkillChgCond[nSkillId - 1][j];
			int nCapacity = sizeof(SkillChgCond) / sizeof(int);
			int nValCnt = StrToIntArray(szVal, SEPERATOR, pVal, nCapacity);

			_ASSERT(nValCnt == 0 || nValCnt == nCapacity);
		}
	}

	return true;
}

bool SkillManager::LoadSkillRelation(const char *szFile)
{
	KIniFile	iniFile;

	if( !iniFile.Load(szFile) )
		return false;

	static const int	MAX_VAL_SIZE = 256;
	char	sec[MAX_VAL_SIZE];
	char	val[MAX_VAL_SIZE];

	// 读取所有的主技能
	for(int i = 0; i < MAX_SKILL_SERIES; ++i)
	{
		sprintf(sec, "Series_%d", i);
		iniFile.GetString(sec, "MainSkillIds", "", val, MAX_VAL_SIZE);
		StrToIntArray(val, SEPERATOR, m_MainSkillIds[i], MAX_MAINSKILL_PER_SERIES);
	}

	for(int j = 0; j < MAX_SKILL_SERIES; ++j)
	for(int k = 0; k < MAX_MAINSKILL_PER_SERIES; ++k)
	{
		if(m_MainSkillIds[j][k] <= 0 && m_MainSkillIds[j][k] > MAX_SKILL)	
			continue;

		// 读取每个主技能对应的子技能
		sprintf(sec, "MainSkill_%d", m_MainSkillIds[j][k]);
		iniFile.GetString(sec, "SubSkillIds", "", val, MAX_VAL_SIZE);

		SUBSKILLSCONT firstSubIds;

		StrToIntVec(val, SEPERATOR, firstSubIds);
		m_MainIdToSubIds.insert( MAINIDTOSUBIDS::value_type(m_MainSkillIds[j][k], firstSubIds) );

		// 读取每个子技能对应的所有id
		int	nFirstSubIdsCnt = firstSubIds.size();

		for(int nFirstSubIdx = 0; nFirstSubIdx < nFirstSubIdsCnt; ++nFirstSubIdx)
		{
			sprintf(sec, "SubSkill_%d", firstSubIds[nFirstSubIdx]);
			iniFile.GetString(sec, "SkillIds", "", val, MAX_VAL_SIZE);

			SUBSKILLSCONT allSubIds;

			StrToIntVec(val, SEPERATOR, allSubIds);
			m_SubIdToNextIds.insert( SUBIDTONEXTIDS::value_type(firstSubIds[nFirstSubIdx], allSubIds) );
		}
	}
	
	return true;
}

#ifdef _SERVER
bool SkillManager::LoadValueSkillData(const char *szFile)
{
	KTabFile	tabFile;

	if( !tabFile.Load(szFile) )
	{
		return false;
	}

	int nSkillNum = tabFile.GetHeight() - 1;

	if(nSkillNum <= 0)
	{
		return false;
	}

	if (nSkillNum > MAX_VALUE_SKILL)
	{
		_ASSERT(0);
		nSkillNum = MAX_VALUE_SKILL;
	}

	for(int i = 0; i < nSkillNum; ++i)
	{
		int	 nSkillId;

		tabFile.GetInteger(i + 2, "id", INVALID_SKILL_ID, &nSkillId);

		if(INVALID_SKILL_ID == nSkillId)
		{
			_ASSERT(0);
			continue;
		}

		m_ValueSkillIdToDataIdx.insert( MAINIDTOINDEX::value_type(nSkillId, i) );

		for(int j = 1; j <= MAX_SKILL_LEVEL; ++j)
		{
			tabFile.GetInteger(i + 2, "type", INVALID_ATTRIB, &m_ValueSkillData[i][j - 1].nMagicNo);
			tabFile.GetInteger(i + 2, j + 2, 0, &m_ValueSkillData[i][j - 1].nVal);
		}
	}

	return true;
}
#endif

bool SkillManager::LoadSkillSetting(const char *szFile)
{
	g_OrdinSkillsSetting.Clear();

	if( !g_OrdinSkillsSetting.Load(szFile) )
	{
		return false;
	}

	int nSkillNum = g_OrdinSkillsSetting.GetHeight() - 1;

	if(nSkillNum <= 0)
	{
		return false;
	}

	for (int i = 0; i < nSkillNum; i++)
	{
		int nSkillId;
		int nInitialStatus;
		int nSeries;
		int nSubSkillLevel;
		int nMaxLevel;
		int	IsAddToListWhenInit;
		int IsCommonCoolDown;
		int	SkillType;
		int InvokeHideSkillId;

#ifdef _SERVER
		int BuffIdOnLevelUp;	
#endif
	
		g_OrdinSkillsSetting.GetInteger(i + 2, "SkillId", INVALID_SKILL_ID, &nSkillId);
		g_OrdinSkillsSetting.GetInteger(i + 2, "InitialStatus", skill_status_inactive, &nInitialStatus);
		g_OrdinSkillsSetting.GetInteger(i + 2, "SkillLevel", 1, &nSubSkillLevel);
		g_OrdinSkillsSetting.GetInteger(i + 2, "MaxLevel", 1, &nMaxLevel);
		g_OrdinSkillsSetting.GetInteger(i + 2, "AddToListInit", 0, &IsAddToListWhenInit);
		g_OrdinSkillsSetting.GetInteger(i + 2, "CommonCoolDown",0,&IsCommonCoolDown);
		g_OrdinSkillsSetting.GetInteger(i + 2, "IsSwitchSkill", 0, &SkillType);
		g_OrdinSkillsSetting.GetInteger(i + 2, "InvokeHideSkillId", INVALID_SKILL_ID, &InvokeHideSkillId);

#ifdef _SERVER
		g_OrdinSkillsSetting.GetInteger(i + 2, "LevelUpBuffId", INVALID_BUFF_ID, &BuffIdOnLevelUp);
#endif

		// -1 表示此技能是隐式技能，0 表示主技能，否则表示这个子技能第一级对应的技能的ID
		g_OrdinSkillsSetting.GetInteger(i + 2, "SkillSeries", -1, &nSeries);

	//	_ASSERT( IsIdValid(nSkillId) );
		//_ASSERT( IsLevelValid(nMaxLevel) );
	//	_ASSERT( IsLevelValid(nSubSkillLevel) );

		if( !IsIdValid(nSkillId) || !IsLevelValid(nMaxLevel) || !IsLevelValid(nSubSkillLevel) )
			continue;
		
		m_SkillInfo[nSkillId - 1].nTabFileRowNo = i + 2;
		m_SkillInfo[nSkillId - 1].nInitStatus = nInitialStatus;
		m_SkillInfo[nSkillId - 1].nSkillSeries = nSeries;
		m_SkillInfo[nSkillId - 1].nSubSkillLevel = nSubSkillLevel;
		m_SkillInfo[nSkillId - 1].nMaxLevel = nMaxLevel;
		m_SkillInfo[nSkillId - 1].IsAddToListWhenInit = IsAddToListWhenInit;
        m_SkillInfo[nSkillId - 1].IsCommonCoolDown =  IsCommonCoolDown;
		m_SkillInfo[nSkillId - 1].SkillType = SkillType;
		m_SkillInfo[nSkillId - 1].InvokeHideSkillId = InvokeHideSkillId;

#ifdef _SERVER
		m_SkillInfo[nSkillId - 1].BuffIdOnLevelUp = BuffIdOnLevelUp;
#endif
	}
	
	return true;	
}

void SkillManager::ClearInstancedSkills()
{
	for(int i = 0; i < MAX_SKILL; ++i)
	for(int j = 0; j < MAX_SKILL_LEVEL; ++j)
	{
		if(m_pSkills[i][j])
		{
			delete m_pSkills[i][j];
		}
	}
}

KSkill* SkillManager::GetSkill(int nId, int nLevel)
{
	if( !IsSkillValid(nId, nLevel) )
		return NULL;

	if( m_pSkills[nId - 1][nLevel - 1] )
		return m_pSkills[nId - 1][nLevel - 1];
	else
		return InstanceSkill(nId, nLevel);
}

#ifndef _SERVER
void  SkillManager::GetSkill(const char* name)
{
	for ( int nID = 0; nID < MAX_SKILL; ++nID )
	{
		int level = GetSubSkillLevel(nID);
		KSkill* pSkill = GetSkill(nID, level);
		if ( pSkill )
		{
			string res = pSkill->m_szName;
			string::size_type i = res.find( name );
			if ( i != string::npos )
				SkillsInfo::getSingleton().d_skillResult.d_vecSkill.push_back( pSkill );
		}
	}
}

KSkill* SkillManager::GetSkill(int skillId, int* nLevel)
{
	*nLevel = GetSubSkillLevel(skillId);
	return GetSkill(skillId, *nLevel);
}

#endif

KSkill* SkillManager::InstanceSkill(int nSkillId, int nLevel)
{
	KSkill	*pSkill = new KSkill;

	if(NULL == pSkill)
	{
		return NULL;
	}

	pSkill->GetInfoFromTabFile(&g_OrdinSkillsSetting, m_SkillInfo[nSkillId - 1].nTabFileRowNo);
	
	m_pSkills[nSkillId - 1][nLevel - 1] = pSkill;
	pSkill->SetCurLevel( nLevel );

	return pSkill;
}

#ifdef _SERVER
void SkillManager::GetValueSkillMagicVal(int nSkillId, int nLevel, PMagicData pData)
{
	MAINIDTOINDEX::iterator it = m_ValueSkillIdToDataIdx.find(nSkillId);

	if( it == m_ValueSkillIdToDataIdx.end() || nLevel <= 0 || nLevel > m_SkillInfo[nSkillId - 1].nMaxLevel)
	{
		pData->nMagicNo = INVALID_ATTRIB;
		pData->nVal = 0;
	}
	else
	{
		*pData = m_ValueSkillData[it->second][nLevel - 1];
	}
}
#endif

void SkillManager::StrToIntVec(char *szStr, const char *szTag, vector<int> &viRst)
{
	const char *pToken = strtok(szStr, szTag);

	while(pToken)
	{
		viRst.push_back( atoi(pToken) );
		pToken = strtok(NULL, szTag);
	}
}

int	SkillManager::GetMainSkillIds(int nRoleType, int nRoleSkillSeries, int *pOutArray, int nArrayCapacity)
{
	if(NULL == pOutArray)
		return 0;

	if(nRoleType < enRoleType_Knight || nRoleType >= enRoleType_Number)
		return 0;

	if(nRoleSkillSeries <= role_skillseries_invalid || nRoleSkillSeries >= role_skillseries_count)
		return 0;

	int nCount = 0;
	int nSeries = nRoleType * role_skillseries_count + nRoleSkillSeries;

	for(int i = 0; i < MAX_MAINSKILL_PER_SERIES && i < nArrayCapacity; ++i)
	{
		if(m_MainSkillIds[nSeries][i] > 0 && m_MainSkillIds[nSeries][i] <= MAX_SKILL)
		{
			pOutArray[i] = m_MainSkillIds[nSeries][i];
			++nCount;
		}
	}

	return nCount;
}

int SkillManager::GetInitSubSkillIds(int nMainSkillId, int *pOutArray, int nArrayCapacity)
{
	if(NULL == pOutArray)
		return 0;

	MAINIDTOSUBIDS::iterator itSubIdCont = m_MainIdToSubIds.find(nMainSkillId);

	if(m_MainIdToSubIds.end() == itSubIdCont)
		return 0;

	SUBSKILLSCONT::iterator itSubId = itSubIdCont->second.begin();
	SUBSKILLSCONT::iterator itSubIdEnd = itSubIdCont->second.end();

	int nCount = 0;

	for(; itSubId != itSubIdEnd && nCount < nArrayCapacity; ++itSubId)
	{
		if(*itSubId > 0 && *itSubId <= MAX_SKILL)
		{
			*pOutArray = *itSubId;
			++pOutArray;
			++nCount;
		}
	}

	return nCount;
}

int	SkillManager::GetSubSkillIdByLvl(int nSkillId, int nLevel)
{
	if( IsValueSkill(nSkillId) ) 
		return nSkillId;

	int	nNextId = -1;

	if(nSkillId > 0 && nSkillId <= MAX_SKILL)
	{
		int nSkillSeries = m_SkillInfo[nSkillId - 1].nSkillSeries;

		if(nSkillSeries > 0)
		{
			SUBIDTONEXTIDS::const_iterator itSubId = m_SubIdToNextIds.find(nSkillSeries);

			if( itSubId != m_SubIdToNextIds.end() )
			{
				if( nLevel >= 1 && nLevel <= itSubId->second.size() )
					nNextId = itSubId->second[nLevel - 1];				
			}
		}
	}

	return nNextId;
}

inline void	ShowErrMsg(const char *szMsg)
{
#ifndef _SERVER
	CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)szMsg, 0);	
#endif
}

bool SkillManager::CanUpdateTo(int nNpcIdx, int nSkillId, int nLevel,bool bShow/*=false*/)
{
	if(nNpcIdx <= 0)
		return false;

	if( !IsSkillValid(nSkillId, nLevel) )
		return false;

	// 由于同一子技能不同等级对应的技能id不同，升级条件表中以第一级ID
	// 和等级来作为所以，所以对于子技能，这里要将id转换为第一级id
	if( IsSubSkill(nSkillId) )
	{
		nSkillId = m_SkillInfo[nSkillId - 1].nSkillSeries;

		if( !IsSkillValid(nSkillId, nLevel) )
		{
			if (bShow)
				ShowErrMsg(MSG_SKILL_LVLUP_MAXLEVEL);
			return false;
		}
	}

	const SkillChgCond &cond = m_SkillChgCond[nSkillId - 1][nLevel - 1];

	// 依次检测各项条件是否满足

	if(-1 != cond.nPlayerLvl && Npc[nNpcIdx].m_Level < cond.nPlayerLvl)
	{
		if (bShow)
			ShowErrMsg(MSG_SKILL_LVLUP_REQPLAYERLVL);
		return false;
	}

	// 检查任务
	
	if(-1 != cond.nReqSkillId && -1 != cond.nReqSkillLvl)
	{
		int nSkillIdx = Npc[nNpcIdx].m_SkillList.FindSkill(cond.nReqSkillId);
		int nLvl = Npc[nNpcIdx].m_SkillList.GetLevelByIdx(nSkillIdx);

		if(nLvl < cond.nReqSkillLvl)
		{
			if (bShow)
				ShowErrMsg(MSG_SKILL_LVLUP_REQSKILL);
			return false;
		}
	}

	if(-1 != cond.nOwnMoney)
	{
		int nMoney = GetTotalMoney(Npc[nNpcIdx].GetPlayerIdx());

		if(nMoney < cond.nOwnMoney)
		{
			if (bShow)
				ShowErrMsg(MSG_OWNMONEY_NOTENOUGH);
			return false;
		}
	}

	if(-1 != cond.nOwnSkillExp)
	{
		DWORD nExp = Player[Npc[nNpcIdx].GetPlayerIdx()].GetSkillExp();

		if(nExp < cond.nOwnSkillExp)
		{
			if (bShow)
				ShowErrMsg(MSG_SKILL_LVLUP_REQOWNEXP);
			return false;
		}
	}

	bool	bNeedItem = false;

	for(int i = 0; i < ITEM_KEY_NUM; ++i)
	{
		if(-1 != cond.nOwnItemKey[i])
		{
			bNeedItem = true;
			break;
		}
	}

	if(bNeedItem)
	{
		int nPlayerIdx = Npc[nNpcIdx].GetPlayerIdx();
		int	nItemIdx;

		if( !Player[nPlayerIdx].m_ItemList.FindSameParticularItem(cond.nOwnItemKey[0],
				cond.nOwnItemKey[1], cond.nOwnItemKey[2], &nItemIdx) )
		{
			if (bShow)
				ShowErrMsg(MSG_SKILL_LVLUP_REQITEM);
			return false;
		}
	}

	return true;
}

#ifdef _SERVER
void SkillManager::ConsumeUpdateCost(int nNpcIdx, int nSkillId, int nLevel)
{
	if(nNpcIdx <= 0)
		return;

	if( !IsSkillValid(nSkillId, nLevel) )
		return;

	// 由于同一子技能不同等级对应的技能id不同，升级条件表中以第一级ID
	// 和等级来作为所以，所以对于子技能，这里要将id转换为第一级id
	if( IsSubSkill(nSkillId) )
	{
		nSkillId = m_SkillInfo[nSkillId - 1].nSkillSeries;

		if( !IsSkillValid(nSkillId, nLevel) )
			return;
	}

	const SkillChgCond &cond = m_SkillChgCond[nSkillId - 1][nLevel - 1];

	if(-1 != cond.nCostMoney)
	{
		if (Player[Npc[nNpcIdx].GetPlayerIdx()].Pay(cond.nCostMoney))
		{
			if (cond.nCostMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
			{
				LogEventParam logParam;
				logParam.event = log_event_skill_update_pay_money;
				logParam.param1 = Player[Npc[nNpcIdx].GetPlayerIdx()].GetGUID();
				logParam.param4 = -cond.nCostMoney;
				g_pLogSystem->Log(logParam);
			}
		}
	}

	if(-1 != cond.nCostSkillExp)
	{
		DWORD skillExp = Player[Npc[nNpcIdx].GetPlayerIdx()].GetSkillExp();
		skillExp -= cond.nCostSkillExp;
		Player[Npc[nNpcIdx].GetPlayerIdx()].SetSkillExp(skillExp);
		Player[Npc[nNpcIdx].GetPlayerIdx()].SyncAttribute(attr_SkillExp);
	}

	bool bCostItem = false;

	for(int i = 0; i < ITEM_KEY_NUM; ++i)
	{
		if(-1 != cond.nCostItemKey[i])
		{
			bCostItem = true;
			break;
		}
	}

	if(bCostItem)
	{
		int nItemIdx;

		Player[Npc[nNpcIdx].GetPlayerIdx()].m_ItemList.FindSameParticularItem(cond.nCostItemKey[0],
				cond.nCostItemKey[1], cond.nCostItemKey[2], &nItemIdx);
		Player[Npc[nNpcIdx].GetPlayerIdx()].m_ItemList.Remove(nItemIdx);
		ItemSet.Remove(nItemIdx);
	}
}
#endif

#ifndef _SERVER
int	SkillManager::GetMainSkillId(int nSubSkillId)
{
	if( IsMainSkill(nSubSkillId) )
		return nSubSkillId;

	if( !IsIdValid(nSubSkillId) )
		return INVALID_SKILL_ID;

	int nBaseSubSkillId = m_SkillInfo[nSubSkillId - 1].nSkillSeries;

	if(nBaseSubSkillId <= 0)
		return INVALID_SKILL_ID;

	MAINIDTOSUBIDS::const_iterator	itSubIdCont = m_MainIdToSubIds.begin();
	MAINIDTOSUBIDS::const_iterator	itSubIdContEnd = m_MainIdToSubIds.end();

	for(; itSubIdCont != itSubIdContEnd; ++itSubIdCont)
	{
		SUBSKILLSCONT::const_iterator	itSubId = (*itSubIdCont).second.begin();
		SUBSKILLSCONT::const_iterator	itSubIdEnd = (*itSubIdCont).second.end();

		for(; itSubId != itSubIdEnd; ++itSubId)
		{
			if(*itSubId == nBaseSubSkillId)
				return (*itSubIdCont).first;
		}
	}

	return INVALID_SKILL_ID;
}

#define MAX_SKILL_DESC_LEN 1024

void SkillManager::GenDesc(int nSkillId,int nLevel,char * lpBuff,int nBuffSize,bool bDisableStyle/*=false*/)
{  
	KSkill	*pSkill = GetSkill(nSkillId, nLevel > 0 ? nLevel : 1);

	if (NULL!=pSkill)
	{
		//Generate the szDesc automaticly...................................................
		ConfigManager& cm = ConfigManager::Singleton();
		char           szTempBuff[MAX_SKILL_DESC_LEN];
		
		char *         szDest=lpBuff;
		szDest[0]=0;

		unsigned long  dwSizeCost=1;
		unsigned long  dwSizeCurNeed=0;
		int            nStyle=0;
		
		if (bDisableStyle)
			nStyle=1;
		
		
		//Head
		const char * szHeadTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_head, 0);
		if (szHeadTemp!=NULL)
		{
			dwSizeCurNeed = strlen(szHeadTemp);
			
			if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
			{
				strcat(szDest,szHeadTemp);
				dwSizeCost+=dwSizeCurNeed;
				szDest+=dwSizeCurNeed;
			}//endif
			
		}

		//Name
		const char * szNameTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_name ,nStyle);
		if (szNameTemp!=NULL)
		{
			sprintf(szTempBuff,szNameTemp,pSkill->m_szName);
			
			dwSizeCurNeed = strlen(szTempBuff);
			
			if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
			{
				strcat(szDest,szTempBuff);
				dwSizeCost+=dwSizeCurNeed;
				szDest+=dwSizeCurNeed;
			}//endif
			
		}

		if (nLevel!=0 && !IsValueSkill(nSkillId) && !IsMainSkill(nSkillId))
		{
			bool  bIsSwitchSkill=g_SkillManager.IsValueSkill(nSkillId);
			//Level
			if (g_SkillManager.GetMaxLevel(nSkillId)>=1 && !bIsSwitchSkill)
			{
				
				const char * szLevelTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_level ,nStyle);
				if (szLevelTemp!=NULL)
				{
					sprintf(szTempBuff,szLevelTemp,nLevel);
					
					dwSizeCurNeed = strlen(szTempBuff);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,szTempBuff);
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
					
				}//endif
			}//endif
			
			//Distance
			if (
				(pSkill->GetSkillStyle()==0 || pSkill->GetSkillStyle()==6 || pSkill->GetSkillStyle()==9 )
				&& pSkill->GetAttackRadius()>0 && !bIsSwitchSkill
				)
			{
				
				const char * szDisTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_dis ,nStyle);
				if (szDisTemp!=NULL)
				{
					sprintf(szTempBuff,szDisTemp,pSkill->GetAttackRadius()/32);
					
					dwSizeCurNeed = strlen(szTempBuff);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,szTempBuff);
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
					
				}//endif
				
			}//endif
			
			//cost 
			if (pSkill->GetSkillCost()>0 && !bIsSwitchSkill)
			{ 
				const char * szCosTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_cos ,nStyle);
				
				if (szCosTemp!=NULL)
				{
					if (pSkill->GetSkillCostType()==0)
					{
						sprintf(szTempBuff,szCosTemp,cm.GetConfigurableDisplayStyle(style_skill_tip_cost_nor_string ,0),pSkill->GetSkillCost());
					}//endif
					else
					{
						sprintf(szTempBuff,szCosTemp,cm.GetConfigurableDisplayStyle(style_skill_tip_cost_nor_string ,1),pSkill->GetSkillCost());
					}
					dwSizeCurNeed = strlen(szTempBuff);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,szTempBuff);
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
					
				}//endif
			}//endif
			
			//Cool down time
			if (pSkill->GetDelayPerCast()>0 && !bIsSwitchSkill)
			{ 
				
				unsigned long dwTotalSecond=pSkill->GetDelayPerCast()/1000;
				unsigned long dwCoolSecond=dwTotalSecond%60;
				unsigned long dwCoolMinute=(dwTotalSecond/60)%60;
				unsigned long dwCoolHour=(dwTotalSecond/3600)%60;
				
				const char * szCoolTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_cool ,nStyle);
                if (szCoolTemp!=NULL)
				{
					char szSecond[16]="";
					char szMinite[16]="";
					char szHoure[16]="";
					
					if (dwCoolSecond && cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,0)!=NULL)
						sprintf(szSecond,"%d%s",dwCoolSecond,cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,0));
					
					if (dwCoolMinute && cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,1)!=NULL)
						sprintf(szMinite,"%d%s",dwCoolMinute,cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,1));
					
					if (dwCoolHour && cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,2)!=NULL)
						sprintf(szHoure,"%d%s",dwCoolHour,cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,2));
					
					char szTimeString[64];
					szTimeString[0]=0;
					
					strcat(szTimeString,szHoure);
					strcat(szTimeString,szMinite);
					strcat(szTimeString,szSecond);
					
					sprintf(szTempBuff,szCoolTemp,szTimeString);
					
					dwSizeCurNeed = strlen(szTempBuff);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,szTempBuff);
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
				}//endif
			}//endif
			
			//Desc view
			const char * szDescTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_desc ,nStyle);
			if (szDescTemp!=NULL)
			{
				sprintf(szTempBuff,szDescTemp,pSkill->m_szDesc);
				
				dwSizeCurNeed = strlen(szTempBuff);
				
				if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
				{
					strcat(szDest,szTempBuff);
					dwSizeCost+=dwSizeCurNeed;
					szDest+=dwSizeCurNeed;
				}//endif
				
			}//endif]

		}//endif
		else
		{
			if (!IsMainSkill(nSkillId) && !IsValueSkill(nSkillId))
			{
				const char * szHaventStudy=cm.GetConfigurableDisplayStyle(style_skill_tip_havent_study,0);
				if (szHaventStudy!=NULL)
				{
					dwSizeCurNeed=strlen(szHaventStudy);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,cm.GetConfigurableDisplayStyle(style_skill_tip_havent_study,0));
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
				}//endif
			}//endif
			else
			{
				dwSizeCurNeed=strlen("<Seg text-align=right float=wrap><Obj type=text> </Obj></Seg>");
				
				if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
				{
					strcat(szDest,"<Seg text-align=right float=wrap><Obj type=text> </Obj></Seg>");
					dwSizeCost+=dwSizeCurNeed;
					szDest+=dwSizeCurNeed;
				}//endif

				const char * szDescTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_desc ,nStyle);
				if (szDescTemp!=NULL)
				{
					sprintf(szTempBuff,szDescTemp,pSkill->m_szDesc);
					
					dwSizeCurNeed = strlen(szTempBuff);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,szTempBuff);
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
				}//endif
			}

		}
		
		//End Flag
		const char * szDescTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_end ,0);
		if (szDescTemp!=NULL)
		{
			
			dwSizeCurNeed = strlen(szDescTemp);
			
			if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
			{
				strcat(szDest,szDescTemp);
				dwSizeCost+=dwSizeCurNeed;
				szDest+=dwSizeCurNeed;
			}//endif
		}
		
		//End...............................................................................
	}
}


void SkillManager::GenDescWithSize(int nSkillId, int nLevel, char* lpBuff, int nBuffSize)
{  
	KSkill	*pSkill = GetSkill(nSkillId, nLevel > 0 ? nLevel : 1);

	if (NULL!=pSkill)
	{
		//Generate the szDesc automaticly...................................................
		ConfigManager& cm = ConfigManager::Singleton();
		char           szTempBuff[MAX_SKILL_DESC_LEN];		
		char *         szDest=lpBuff;

		unsigned long  dwSizeCost=1;
		unsigned long  dwSizeCurNeed=0;
		int            nStyle=0;

		//Name
		const char * szNameTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_name ,0);
		if (szNameTemp!=NULL)
		{
			sprintf(szTempBuff,szNameTemp,pSkill->m_szName);
			
			dwSizeCurNeed = strlen(szTempBuff);
			
			if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
			{
				strcat(szDest,szTempBuff);
				dwSizeCost+=dwSizeCurNeed;
				szDest+=dwSizeCurNeed;
			}//endif
			
		}

		if (nLevel!=0 && !IsValueSkill(nSkillId) && !IsMainSkill(nSkillId))
		{
			bool  bIsSwitchSkill=g_SkillManager.IsValueSkill(nSkillId);
			//Level
			if (g_SkillManager.GetMaxLevel(nSkillId)>=1 && !bIsSwitchSkill)
			{
				
				const char * szLevelTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_level ,0);
				if (szLevelTemp!=NULL)
				{
					sprintf(szTempBuff,szLevelTemp,nLevel);
					
					dwSizeCurNeed = strlen(szTempBuff);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,szTempBuff);
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
					
				}//endif
			}//endif
			
			//Distance
			if (
				(pSkill->GetSkillStyle()==0 || pSkill->GetSkillStyle()==6 || pSkill->GetSkillStyle()==9 )
				&& pSkill->GetAttackRadius()>0 && !bIsSwitchSkill
				)
			{
				
				const char * szDisTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_dis ,0);
				if (szDisTemp!=NULL)
				{
					sprintf(szTempBuff,szDisTemp,pSkill->GetAttackRadius()/32);
					
					dwSizeCurNeed = strlen(szTempBuff);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,szTempBuff);
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
					
				}//endif
				
			}//endif
			
			//cost 
			if (pSkill->GetSkillCost()>0 && !bIsSwitchSkill)
			{ 
				const char * szCosTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_cos ,0);
				
				if (szCosTemp!=NULL)
				{
					if (pSkill->GetSkillCostType()==0)
					{
						sprintf(szTempBuff,szCosTemp,cm.GetConfigurableDisplayStyle(style_skill_tip_cost_nor_string ,0),pSkill->GetSkillCost());
					}//endif
					else
					{
						sprintf(szTempBuff,szCosTemp,cm.GetConfigurableDisplayStyle(style_skill_tip_cost_nor_string ,1),pSkill->GetSkillCost());
					}
					dwSizeCurNeed = strlen(szTempBuff);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,szTempBuff);
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
					
				}//endif
			}//endif
			
			//Cool down time
			if (pSkill->GetDelayPerCast()>0 && !bIsSwitchSkill)
			{ 
				
				unsigned long dwTotalSecond=pSkill->GetDelayPerCast()/1000;
				unsigned long dwCoolSecond=dwTotalSecond%60;
				unsigned long dwCoolMinute=(dwTotalSecond/60)%60;
				unsigned long dwCoolHour=(dwTotalSecond/3600)%60;
				
				const char * szCoolTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_cool ,0);
                if (szCoolTemp!=NULL)
				{
					char szSecond[16]="";
					char szMinite[16]="";
					char szHoure[16]="";
					
					if (dwCoolSecond && cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,0)!=NULL)
						sprintf(szSecond,"%d%s",dwCoolSecond,cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,0));
					
					if (dwCoolMinute && cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,1)!=NULL)
						sprintf(szMinite,"%d%s",dwCoolMinute,cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,1));
					
					if (dwCoolHour && cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,2)!=NULL)
						sprintf(szHoure,"%d%s",dwCoolHour,cm.GetConfigurableDisplayStyle(style_skill_tip_time_nor_string ,2));
					
					char szTimeString[64];
					szTimeString[0]=0;
					
					strcat(szTimeString,szHoure);
					strcat(szTimeString,szMinite);
					strcat(szTimeString,szSecond);
					
					sprintf(szTempBuff,szCoolTemp,szTimeString);
					
					dwSizeCurNeed = strlen(szTempBuff);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,szTempBuff);
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
				}//endif
			}//endif
			
			//Desc view
			const char * szDescTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_desc ,0);
			if (szDescTemp!=NULL)
			{
				sprintf(szTempBuff,szDescTemp,pSkill->m_szDesc);
				
				dwSizeCurNeed = strlen(szTempBuff);
				
				if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
				{
					strcat(szDest,szTempBuff);
					dwSizeCost+=dwSizeCurNeed;
					szDest+=dwSizeCurNeed;
				}//endif
				
			}//endif]

		}//endif
		else
		{
			if (!IsMainSkill(nSkillId) && !IsValueSkill(nSkillId))
			{
				const char * szHaventStudy=cm.GetConfigurableDisplayStyle(style_skill_tip_havent_study,0);
				if (szHaventStudy!=NULL)
				{
					dwSizeCurNeed=strlen(szHaventStudy);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,cm.GetConfigurableDisplayStyle(style_skill_tip_havent_study,0));
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
				}//endif
			}//endif
			else
			{
				dwSizeCurNeed=strlen("<Seg text-align=right float=wrap><Obj type=text> </Obj></Seg>");
				
				if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
				{
					strcat(szDest,"<Seg text-align=right float=wrap><Obj type=text> </Obj></Seg>");
					dwSizeCost+=dwSizeCurNeed;
					szDest+=dwSizeCurNeed;
				}//endif

				const char * szDescTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_desc ,0);
				if (szDescTemp!=NULL)
				{
					sprintf(szTempBuff,szDescTemp,pSkill->m_szDesc);
					
					dwSizeCurNeed = strlen(szTempBuff);
					
					if ( dwSizeCost+dwSizeCurNeed< nBuffSize)
					{
						strcat(szDest,szTempBuff);
						dwSizeCost+=dwSizeCurNeed;
						szDest+=dwSizeCurNeed;
					}//endif
				}//endif
			}

		}		
		//End...............................................................................
	}
}
#endif