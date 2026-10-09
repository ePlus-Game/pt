//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-9-4 16:20
//      File_base        : NpcSkillList
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
#include "NpcSkillList.h"
#include "SkillManager.h"
#include "KPlayer.h"
#include "MagicAttribute.h"
#include "KSkills.h"
#include "buff_alloc.h"
#ifndef _SERVER
	#include "CoreShell.h"
#endif


#define		SKILLINFO_ALLOCATE_GRANULARITY		(512)
#define		MAXCOUNT_SKILL_INFO					(1024 * SKILLINFO_ALLOCATE_GRANULARITY)

__allocator<NpcSkillInfo, SKILLINFO_ALLOCATE_GRANULARITY, MAXCOUNT_SKILL_INFO>	g_skillInfoAllco;

NpcSkillInfo*	AllocSkillInfo()
{
	return g_skillInfoAllco._alloc();
}

void FreeSkillInfo(NpcSkillInfo *ptr)
{
	g_skillInfoAllco._free(ptr);
}

const int NpcSkillInfo::DEF_COOLDOWN_TIME = 1000000;


NpcSkillList& NpcSkillList::operator= (const NpcSkillList &rhs)
{
	if(this != &rhs)
	{
		Clear();

		for(int nLoop = 0; nLoop < __max_skill_count; ++nLoop)
		{
			if(rhs.m_NpcSkillInfo[nLoop])
			{
				NpcSkillInfo *pCopy = AllocSkillInfo();

				_ASSERT(pCopy);
				if(NULL == pCopy)
					break;

				*pCopy = *rhs.m_NpcSkillInfo[nLoop];
				m_NpcSkillInfo[nLoop] = pCopy;
			}
		}
	}

	return *this;
}

void NpcSkillList::Clear()
{
	for(int nLoop = 0; nLoop < __max_skill_count; ++nLoop)
	{
		if(m_NpcSkillInfo[nLoop])
		{
			FreeSkillInfo(m_NpcSkillInfo[nLoop]);
			m_NpcSkillInfo[nLoop] = NULL;
		}
	}	

#ifndef _SERVER
	m_SwitchSkillId = INVALID_SKILL_ID;
#endif
}

bool NpcSkillList::LevelUpTo(int nSkillId, int nLevel)
{
	if( nLevel <= 0 || nLevel > g_SkillManager.GetMaxLevel(nSkillId) )
		return false;

	int nIdx = FindSkill(nSkillId);

	if(INVALID_SKILL_INDEX == nIdx)
		return false;

	if(NULL == m_NpcSkillInfo[nIdx])
		return false;

	if(nLevel <= m_NpcSkillInfo[nIdx]->m_SkillLevel)
		return false;

	bool bRet = false;

	if( g_SkillManager.IsValueSkill(nSkillId) )
	{
#ifdef _SERVER
		int	nOriLevel = m_NpcSkillInfo[nIdx]->m_SkillLevel;
#endif
		// 数值技能只需要在第一级的时候初始化一些东西
		if(1 == nLevel)
			AddSkill(nIdx, nSkillId, nLevel, g_SkillManager.GetSkillInitStatus(nSkillId));
		else
			m_NpcSkillInfo[nIdx]->m_SkillLevel = nLevel;

#ifdef _SERVER
			UpdateValueSkillEffect(nSkillId, nOriLevel, nLevel);
#endif
			bRet = true;
	}
	else if( g_SkillManager.IsSubSkill(nSkillId) )
	{
		int nNextSubId = g_SkillManager.GetSubSkillIdByLvl(nSkillId, nLevel);

		if(-1 != nNextSubId)
		{
			bRet = true;

			RepSkillsOnLevelup(nIdx, nNextSubId);

#ifdef _SERVER
			if(skill_status_usable == m_NpcSkillInfo[nIdx]->m_Status)
				CastPassiveSkill(nNextSubId, nLevel);
#endif
		}
	}
	else
	{
		bRet = false;
	}

#ifdef _SERVER
	if(bRet)
	{
		int nLevelUpBuff = g_SkillManager.GetLevelUpBuff(nSkillId);
		if(INVALID_SKILL_ID != nLevelUpBuff)
			BuffMgr::Singleton().AddNpcBuff(m_NpcIndex, m_NpcIndex, nLevelUpBuff);
		
		SkillLevelUpSync(nSkillId, nLevel);
	}
#endif

#ifdef _SERVER
	//日志：技能学习升级
	if (bRet)
	{
		if (Npc[m_NpcIndex].IsPlayer())
		{
			KPlayer& player = Player[Npc[m_NpcIndex].GetPlayerIdx()];

			LogEventParam skillLevelUpEent;
			skillLevelUpEent.event = log_event_skill_level_up;
			skillLevelUpEent.param1 = player.GetGUID();
			snprintf(skillLevelUpEent.param2.data, sizeof(skillLevelUpEent.param2.data), "%d", nSkillId);
			snprintf(skillLevelUpEent.param3.data, sizeof(skillLevelUpEent.param3.data), "%d", nLevel);
			skillLevelUpEent.param4 = player.GetOnlineTime();
			g_pLogSystem->Log(skillLevelUpEent);
		}
	}
#endif

	return bRet;
}

bool NpcSkillList::AddSkillEx(int nSkillId, int nLevel, int nStatus)
{
	if( !AddSkill(nSkillId, nLevel, nStatus) )
		return false;

	int nInvokeId = g_SkillManager.GetInvokeHideSkill(nSkillId);

	if(INVALID_SKILL_ID != nInvokeId)
		return AddSkill(nInvokeId,
			g_SkillManager.GetSubSkillLevel(nInvokeId),
			g_SkillManager.GetSkillInitStatus(nInvokeId)
			);
	else
		return true;
}

bool NpcSkillList::AddSkill(int nSkillId, int nLevel, int nStatus)
{
	if( !g_SkillManager.IsIdValid(nSkillId) )
		return false;

	if(nStatus <= skill_status_begin || nStatus >= skill_status_end)
		return false;

	if(INVALID_SKILL_INDEX != FindSkill(nSkillId) )
		return false;

	int	nEmptyPos = GetEmptyPos();

	if(INVALID_SKILL_INDEX == nEmptyPos)
		return false;

	m_NpcSkillInfo[nEmptyPos] = AllocSkillInfo();

	if(NULL == m_NpcSkillInfo[nEmptyPos])
	{
		_ASSERT(false);
		return false;
	}

	AddSkill(nEmptyPos, nSkillId, nLevel, nStatus);

	return true;
}

void NpcSkillList::AddSkill(int nIdx, int nSkillId, int nLevel, int nStatus)
{
	m_NpcSkillInfo[nIdx]->m_SkillId = nSkillId;
	m_NpcSkillInfo[nIdx]->m_SkillLevel = nLevel;
	m_NpcSkillInfo[nIdx]->m_Status = nStatus;
	m_NpcSkillInfo[nIdx]->m_PreStatus = nStatus;

	KSkill *pSkill = g_SkillManager.GetSkill(nSkillId, nLevel);

	if(pSkill)
	{
		m_NpcSkillInfo[nIdx]->m_CoolDownTime = pSkill->GetDelayPerCast();
		m_NpcSkillInfo[nIdx]->m_PreCoolDownTime = m_NpcSkillInfo[nIdx]->m_CoolDownTime;
	}	
}

#ifdef _SERVER
void NpcSkillList::UpdateValueSkillEffect(int nSkillId, int nOriLevel, int nCurLevel)
{
	MagicData	oriMagic;
	MagicData	curMagic;

	g_SkillManager.GetValueSkillMagicVal(nSkillId, nOriLevel, &oriMagic);
	g_SkillManager.GetValueSkillMagicVal(nSkillId, nCurLevel, &curMagic);


	MagicData	magic = curMagic;
	magic.nVal = curMagic.nVal - oriMagic.nVal;

	if(magic.nMagicNo != INVALID_ATTRIB)
		g_MagicAttrModifier.ModifyMagicAttr(m_NpcIndex, &magic);	
}
#endif

#ifdef _SERVER
void NpcSkillList::CastPassiveSkill(int nSkillId, int nLevel)
{
	KSkill *pSkill = g_SkillManager.GetSkill(nSkillId, nLevel);
		
	if(pSkill && SKILL_SS_PassivityNpcState == pSkill->GetSkillStyle())
		pSkill->Cast(m_NpcIndex, SKILL_SPT_TargetIndex, m_NpcIndex);
}
#endif

#ifdef _SERVER
void NpcSkillList::CastAllPassiveSkill()
{
	for(int i = 0; i < __max_skill_count; ++i)
	{
		if(NULL == m_NpcSkillInfo[i])
			continue;

		if(skill_status_usable == m_NpcSkillInfo[i]->m_Status)
		{
			KSkill *pSkill = g_SkillManager.GetSkill(m_NpcSkillInfo[i]->m_SkillId, 
				m_NpcSkillInfo[i]->m_SkillLevel);

			if( pSkill && (SKILL_SS_PassivityNpcState == pSkill->GetSkillStyle()) )
				pSkill->Cast(m_NpcIndex, SKILL_SPT_TargetIndex, m_NpcIndex);
		}
	}
}
#endif

#ifdef _SERVER
void NpcSkillList::CastAllValueSkill()
{
	for(int i = 0; i < __max_skill_count; ++i)
	{
		if(NULL == m_NpcSkillInfo[i])
			continue;

		if( skill_status_usable == m_NpcSkillInfo[i]->m_Status 
		    && g_SkillManager.IsValueSkill(m_NpcSkillInfo[i]->m_SkillId) )
			UpdateValueSkillEffect(m_NpcSkillInfo[i]->m_SkillId, 0, m_NpcSkillInfo[i]->m_SkillLevel);
	}
}
#endif

void NpcSkillList::SetComCoolTime(unsigned long iInterval)
{
	unsigned long dwTime=0;
	
#ifdef _SERVER
	dwTime = g_pController->GetTickCounter();
#else
	dwTime = GetTickCount();

	// 公共CD还没有调试
	//KSkill* pSkill = g_SkillManager.GetSkill( nSkillID );
	KItemGroupCD_C tagCD_C;
	//tagCD_C.Id			= nSkillID;
	//tagCD_C.nGroup		= pSkill->GetGroup();
	tagCD_C.nGroup		= 0;
	tagCD_C.ulCDTime	= iInterval/1000;
	tagCD_C.eType		= skill_common_coolingdown;
	if ( !g_ProtocolSimulationSet.IsExisting( itemgroupcd_dataset ) )
	{
		g_ProtocolSimulationSet.registerSimulation( itemgroupcd_dataset, new KItemGroupCDSimulation( itemgroupcd_dataset ) );
	}
	KItemGroupCDSimulation* pGroupCDSimulation = (KItemGroupCDSimulation*)g_ProtocolSimulationSet.getSimulation( itemgroupcd_dataset );
	if ( pGroupCDSimulation )
	{
		pGroupCDSimulation->AddGroupCD( tagCD_C );
	}
#endif
		
	m_CommonCoolDownTimer=dwTime+iInterval;
}

bool NpcSkillList::CanCast(int nSkillID)
{
	int nIdx = FindSkill(nSkillID);

	if(INVALID_SKILL_INDEX == nIdx)
		return false;

	if(NULL == m_NpcSkillInfo[nIdx])
		return false;

	if(m_NpcSkillInfo[nIdx]->m_Status != skill_status_usable)
		return false;

	DWORD dwTime;

#ifdef _SERVER
	dwTime = g_pController->GetTickCounter();
#else
	dwTime = GetTickCount();
#endif

	if (g_SkillManager.IsCommonCoolDown(nSkillID) && dwTime < m_CommonCoolDownTimer)
		return false;

	if(m_NpcSkillInfo[nIdx]->m_NextCastTime > dwTime)
		return false;

	return true;
}

#ifndef _SERVER
int	NpcSkillList::GetSubSkillIds(int nMailSkillId, int *pOutArray, int nArrayCapacity)
{
	if(NULL == pOutArray)
		return 0;

	if( !g_SkillManager.IsIdValid(nMailSkillId) )
		return 0;

	// 此处不要对技能系的合法性进行限制
	// 因为现在有一类通用技能，在没选系的时候也要显示
// 	if(Player[CLIENT_PLAYER_INDEX].m_SkillSeries <= role_skillseries_invalid || 
// 		Player[CLIENT_PLAYER_INDEX].m_SkillSeries >= role_skillseries_count)
// 		return 0;

	int nSkillCount = 0;

	for(int nSkillIdx = 0; nSkillIdx < __max_skill_count && nSkillCount < nArrayCapacity; ++nSkillIdx)
	{
		if(NULL == m_NpcSkillInfo[nSkillIdx])
			continue;

		if( g_SkillManager.IsMainSkill(m_NpcSkillInfo[nSkillIdx]->m_SkillId) )
			continue;

		if( g_SkillManager.GetMainSkillId(m_NpcSkillInfo[nSkillIdx]->m_SkillId) == nMailSkillId )
		{
			*pOutArray = m_NpcSkillInfo[nSkillIdx]->m_SkillId;
			++pOutArray;
			++nSkillCount;
		}
	}

	return nSkillCount;
}
#endif

#ifndef _SERVER
int	NpcSkillList::GetMainSkillIds(int *pOutArray, int nArrayCapacity)
{
	if(NULL == pOutArray)
		return 0;

	// 此处不要对技能系的合法性进行限制
	// 因为现在有一类通用技能，在没选系的时候也要显示
// 	if(Player[CLIENT_PLAYER_INDEX].m_SkillSeries <= role_skillseries_invalid || 
// 		Player[CLIENT_PLAYER_INDEX].m_SkillSeries >= role_skillseries_count)
// 		return 0;

	int	nSkillCount = 0;

	for(int nSkillIdx = 0; nSkillIdx < __max_skill_count && nSkillCount < nArrayCapacity; ++nSkillIdx)
	{
		if(NULL == m_NpcSkillInfo[nSkillIdx])
			continue;

		if( g_SkillManager.IsMainSkill(m_NpcSkillInfo[nSkillIdx]->m_SkillId) )
		{
			*pOutArray = m_NpcSkillInfo[nSkillIdx]->m_SkillId;
			++pOutArray;
			++nSkillCount;
		}
	}

	return nSkillCount;
}
#endif

#ifdef _SERVER
void NpcSkillList::SyncSkillInfo(int nIdx, int nSyncOpe)
{
	if( !IsIndexValid(nIdx) )
		return;

	if(NULL == m_NpcSkillInfo[nIdx])
		return;

	PLAYER_SKILLINFO_SYNC	syncInfo;

	// 这里没有加上 skill_ope_levelup 是因为服务器和客户端升级有个
	// 先后顺序的问题，而且升级可能会改变技能id，如果一方先升级
	// 然后将新的id传到另一方，另一方没有这个id，将会导致升级失败
	switch(nSyncOpe)
	{
	case skill_ope_chgstatus:
		syncInfo.PlusInfo.comInfo.Status = m_NpcSkillInfo[nIdx]->m_Status;
		break;

	case skill_ope_chgcdtime:
		syncInfo.PlusInfo.comInfo.CoolDownTime = m_NpcSkillInfo[nIdx]->m_CoolDownTime;
		break;

	case skill_ope_chgcost:
		syncInfo.PlusInfo.nCost = m_NpcSkillInfo[nIdx]->m_CostVal;
		break;

	case skill_ope_chgcastspeed:
		syncInfo.PlusInfo.nCastSpeed = m_NpcSkillInfo[nIdx]->m_CastSpeedEnhance;
		break;

	case skill_ope_cooldown:
	case skill_ope_clearcooldown:
		break;

	default:
		return;
	}

	syncInfo.ProtocolType	= s2c_playerskillinfo;
	syncInfo.Operation		= (BYTE)nSyncOpe;
	syncInfo.SkillId		= (WORD)m_NpcSkillInfo[nIdx]->m_SkillId;	

	if (g_pServer != NULL)
		g_pServer->PackDataToClient( Player[Npc[m_NpcIndex].GetPlayerIdx()].GetNetConnectIdx(),
								 &syncInfo,
								 sizeof(syncInfo)
							   );
}	
#endif

#ifdef _SERVER
void NpcSkillList::SkillLevelUpSync(int nSkillId, int nTargetLevel)
{
	PLAYER_SKILLINFO_SYNC	sync;

	sync.ProtocolType	= s2c_playerskillinfo;
	sync.Operation		= skill_ope_levelup;
	sync.SkillId		= nSkillId;
	sync.PlusInfo.comInfo.Level	= nTargetLevel;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient( Player[Npc[m_NpcIndex].GetPlayerIdx()].GetNetConnectIdx(),
								 &sync,
								 sizeof(sync)
							   );
}
#endif


void NpcSkillList::RepSkillsOnLevelup(int nCurIdx, int nTargetId)
{
	int nCurId = m_NpcSkillInfo[nCurIdx]->m_SkillId;
	int nCurInvokeId = g_SkillManager.GetInvokeHideSkill(nCurId);
	int nTargetInvokeId = g_SkillManager.GetInvokeHideSkill(nTargetId);

	if(INVALID_SKILL_ID != nCurInvokeId && INVALID_SKILL_ID != nTargetInvokeId)
	{
		int nCurInvokeIdx = FindSkill(nCurInvokeId);

		if(-1 != nCurInvokeIdx)
		{
			AddSkill(nCurInvokeIdx,
				nTargetInvokeId,
				g_SkillManager.GetSubSkillLevel(nTargetInvokeId),
				g_SkillManager.GetSkillInitStatus(nTargetInvokeId)
				);
		}
	}

	AddSkill(nCurIdx, 
		nTargetId, 
		g_SkillManager.GetSubSkillLevel(nTargetId), 
		g_SkillManager.GetSkillInitStatus(nTargetId)
		);

#ifndef _SERVER
		if( g_SkillManager.IsSameSubSkill(m_SwitchSkillId, nTargetId) )
			m_SwitchSkillId = nTargetId;
#endif		
}

#ifdef _SERVER
int NpcSkillList::SaveSkillData(BYTE *pOutBuf)
{
	int returnSize = 0;

	if(pOutBuf)
	{
		*pOutBuf = CUR_SKILL_VERSION;
//		BYTE* pSkillDataBuff = pOutBuf + 1;
		BYTE* pSkillDataBuff = pOutBuf + 2;
		*(pOutBuf + 1) = (BYTE)ConfigManager::Singleton().GetGlobalVariable(global_var_skill_logic_version);

		switch(CUR_SKILL_VERSION)
		{
		case skill_data_version_1:
			returnSize = SaveSkillDataVersion1(pSkillDataBuff);
			break;
		case skill_data_version_2:
			returnSize = SaveSkillDataVersion2(pSkillDataBuff);
			break;
		}

		if (returnSize > 0)
//			returnSize += 1;
			returnSize += 2;
	}

	return returnSize;
}
#endif

bool NpcSkillList::LoadSkillData(BYTE *pInBuf, int nSize)
{
	Clear();

	bool result = false;

	if(pInBuf && nSize >= 2)
	{
		BYTE dataVersion = *pInBuf;
// 		BYTE* pSkillDataBuff = pInBuf + 1;
// 		int skillDataSize = nSize - 1;
		BYTE* pSkillDataBuff = pInBuf + 2;		
		int skillDataSize = nSize - 2;

		switch(dataVersion)
		{
		case skill_data_version_1:
			result = LoadSkillDataVersion1(pSkillDataBuff, skillDataSize);
			break;
		case skill_data_version_2:
			result = LoadSkillDataVersion2(pSkillDataBuff, skillDataSize);
			break;
		}
		
#ifdef _SERVER
		if (result)
		{
			//如果技能逻辑版本发生变化，则需要根据技能配置表检查并修改初始技能，重新排列顺序
			BYTE logicVersion = *(pInBuf + 1);
			BYTE currentSkillLogicVersion = (BYTE)ConfigManager::Singleton().GetGlobalVariable(global_var_skill_logic_version);
			if (logicVersion != currentSkillLogicVersion)
			{
				Npc[m_NpcIndex].CheckInitSkills();
				//SortBySkillId();
			}
		}
#endif
	}

	return result;
}

bool NpcSkillList::IsClearCooling( int nIdx )
{
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	if(pInfo)
	{
		bool bRet = pInfo->m_bClearCooling;
		pInfo->m_bClearCooling = false;
		return bRet;
	}
	else
		return false;
}

bool NpcSkillList::IsCooling( int nIdx )
{
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	if(pInfo)
	{
		DWORD dwTime;
		
		#ifdef _SERVER
			dwTime = g_pController->GetTickCounter();
		#else
			dwTime = GetTickCount();
		#endif
		
		return pInfo->m_NextCastTime > dwTime;
	}
	else
		return false;
}

int	NpcSkillList::NextSkillIdx(Iterator &iter)
{
	for(int nLoop = iter.m_idx; nLoop < __max_skill_count; ++nLoop)
	{
		if(m_NpcSkillInfo[nLoop])
		{
			iter.m_idx = nLoop + 1;
			return nLoop;
		}
	}

	return INVALID_SKILL_INDEX;
}

#ifdef _SERVER
void NpcSkillList::NotifyAddSkill(int nSkillId, int nLevel, int nStatus)
{
	PLAYER_SKILLINFO_SYNC	sync;

	sync.ProtocolType	= s2c_playerskillinfo;
	sync.Operation		= skill_ope_addskill;
	sync.SkillId		= nSkillId;
	sync.PlusInfo.comInfo.Level	= nLevel;
	sync.PlusInfo.comInfo.Status = nStatus;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient( Player[Npc[m_NpcIndex].GetPlayerIdx()].GetNetConnectIdx(),
								 &sync,
								 sizeof(sync)
							   );
}
#endif

#ifdef _SERVER
void NpcSkillList::NotifyRemoveSkill(int nSkillId)
{
	PLAYER_SKILLINFO_SYNC	sync;

	sync.ProtocolType	= s2c_playerskillinfo;
	sync.Operation		= skill_ope_removeskill;
	sync.SkillId		= nSkillId;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient( Player[Npc[m_NpcIndex].GetPlayerIdx()].GetNetConnectIdx(),
								 &sync,
								 sizeof(sync)
							   );	
}
#endif

bool NpcSkillList::RemoveSkillEx(int nSkillId)
{
	if( !RemoveSkill(nSkillId) )	
		return false;

	int nInvokeId = g_SkillManager.GetInvokeHideSkill(nSkillId);

	if(INVALID_SKILL_ID != nInvokeId)
		return RemoveSkill(nInvokeId);
	else
		return true;
}

bool NpcSkillList::RemoveSkill(int nSkillId)
{
	int	nIdx = FindSkill(nSkillId);

	if( !IsIndexValid(nIdx) )
		return false;

	if(NULL == m_NpcSkillInfo[nIdx])
		return false;

	FreeSkillInfo(m_NpcSkillInfo[nIdx]);
	m_NpcSkillInfo[nIdx] = NULL;

	return true;
}

void NpcSkillList::ClearCoolDown(int nSkillId)
{
	int	nIdx = FindSkill(nSkillId);

	if( IsCooling(nIdx) )
	{

		NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

		if(pInfo)
		{
#ifdef _SERVER
			pInfo->m_NextCastTime = 0;
			SyncSkillInfo(nIdx, skill_ope_clearcooldown);
#else
			pInfo->m_NextCastTime = 0;
			pInfo->m_bClearCooling = true;
			KSkill* pSkill = g_SkillManager.GetSkill( nSkillId );
			KItemGroupCD_C tagCD_C;
			tagCD_C.Id			= nSkillId;
			tagCD_C.nGroup		= pSkill->GetGroup();
			tagCD_C.ulCDTime	= pSkill->GetDelayPerCast() / 1000;
			tagCD_C.eType		= skill_immediacy_type;
			if ( !g_ProtocolSimulationSet.IsExisting( itemgroupcd_dataset ) )
			{
				g_ProtocolSimulationSet.registerSimulation( itemgroupcd_dataset, new KItemGroupCDSimulation( itemgroupcd_dataset ) );
			}
			KItemGroupCDSimulation* pGroupCDSimulation = (KItemGroupCDSimulation*)g_ProtocolSimulationSet.getSimulation( itemgroupcd_dataset );
			if ( pGroupCDSimulation )
			{
				pGroupCDSimulation->DelGroupCD( tagCD_C );
				/* 删除多于操作
				pGroupCDSimulation->AddGroupCD( tagCD_C );
				int nNpcIdx = Player[CLIENT_PLAYER_INDEX].GetNpcIndex();
				int nSkillIdx = Npc[nNpcIdx].GetSkillList().FindSkill( nSkillId );
				if ( nSkillIdx > 0 )
				{
					bool bCooling = Npc[nNpcIdx].GetSkillList().IsCooling( nSkillIdx );
					if ( !bCooling )
					{
						pGroupCDSimulation->DelGroupCD( tagCD_C );
					}
				}//*/
			}

#endif
		}
	}
}


void NpcSkillList::CoolDown(int nSkillID)
{
	if( Npc[m_NpcIndex].IsPlayer() && g_SkillManager.IsCommonCoolDown(nSkillID) )
	{
		int nSkillSeries = Npc[m_NpcIndex].m_Series * role_skillseries_count + Player[Npc[m_NpcIndex].GetPlayerIdx()].GetSkillSeries();
		_ASSERT(nSkillSeries >= 0 && nSkillSeries <= 5);
		static ConfigManager &cfg = ConfigManager::Singleton();
		SetComCoolTime(cfg.GetGlobalVariable((enumGlobalVariable)(global_var_commoncooldown_interval_1+nSkillSeries)));
	}

	int nIdx = FindSkill(nSkillID);
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	if(pInfo)
	{
#ifdef _SERVER
		pInfo->m_NextCastTime = g_pController->GetTickCounter() + pInfo->m_CoolDownTime;
		SyncSkillInfo(nIdx, skill_ope_cooldown);
#else
		pInfo->m_NextCastTime = GetTickCount() + pInfo->m_CoolDownTime;

		//Client begin skill cd.
		KSkill* pSkill = g_SkillManager.GetSkill( nSkillID );
		KItemGroupCD_C tagCD_C;
		tagCD_C.Id			= nSkillID;
		tagCD_C.nGroup		= pSkill->GetGroup();
		tagCD_C.ulCDTime	= pSkill->GetDelayPerCast() / 1000;
		tagCD_C.eType		= skill_immediacy_type;
		if ( !g_ProtocolSimulationSet.IsExisting( itemgroupcd_dataset ) )
		{
			g_ProtocolSimulationSet.registerSimulation( itemgroupcd_dataset, new KItemGroupCDSimulation( itemgroupcd_dataset ) );
		}
		KItemGroupCDSimulation* pGroupCDSimulation = (KItemGroupCDSimulation*)g_ProtocolSimulationSet.getSimulation( itemgroupcd_dataset );
		if ( pGroupCDSimulation )
		{
			pGroupCDSimulation->AddGroupCD( tagCD_C );
		}
#endif
	}
}

int	NpcSkillList::GetCurSameSubSkillId(int nSkillId)
{
	for(int nSkillIdx = 0; nSkillIdx < __max_skill_count; ++nSkillIdx)
	{
		if(NULL == m_NpcSkillInfo[nSkillIdx])
			continue;
		
		if( g_SkillManager.IsSameSubSkill(nSkillId, m_NpcSkillInfo[nSkillIdx]->m_SkillId) )
			return m_NpcSkillInfo[nSkillIdx]->m_SkillId;
	}
	
	return INVALID_SKILL_ID;
}

#ifndef _SERVER
DWORD NpcSkillList::GetLeftCDTimeByIdx(int nIdx) const
{
	NpcSkillInfo *pInfo = GetSkillInfoByIdx(nIdx);

	if ( !pInfo || pInfo->m_NextCastTime == 0 )
		return 0;

	DWORD	dwCurTime = GetTickCount();
	DWORD	dwLeftCDTime = 0;

	if( pInfo->m_NextCastTime  <= dwCurTime)
		dwLeftCDTime = 0;
	else
	{
		dwLeftCDTime = pInfo->m_NextCastTime - dwCurTime;

		if(dwLeftCDTime > pInfo->m_CoolDownTime)
			dwLeftCDTime = pInfo->m_CoolDownTime;
	}

	dwLeftCDTime /= 1000;

	return dwLeftCDTime;
}
#endif

void NpcSkillList::SortBySkillId()
{
	for (int loop = 0; loop < __max_skill_count; ++loop)
	{
		const int end = __max_skill_count - 1 - loop;
		for (int index = 0; index < end; ++index)
		{
			if (NULL == m_NpcSkillInfo[index])
				continue;

			int index2 = index + 1;
			while (NULL == m_NpcSkillInfo[index2] && index2 < __max_skill_count - 1)
			{
				index2++;
			}

			if (NULL == m_NpcSkillInfo[index2])
				continue;

			if (m_NpcSkillInfo[index]->m_SkillId > m_NpcSkillInfo[index2]->m_SkillId)
			{
				NpcSkillInfo* pTempSkillInfo = m_NpcSkillInfo[index];
				m_NpcSkillInfo[index] = m_NpcSkillInfo[index2];
				m_NpcSkillInfo[index2] = pTempSkillInfo;
			}
		}
	}
}

bool NpcSkillList::LoadSkillDataVersion1(BYTE *pInBuf, int nSize)
{
	if(NULL == pInBuf)
		return false;

	int nSkillCount = nSize / sizeof(DBSkillDataVersion1);

	_ASSERT(nSkillCount <= __max_skill_count);	

	if(nSkillCount >= __max_skill_count)
		return false;

	DWORD	dwCurTime;

#ifndef _SERVER
	dwCurTime = GetTickCount();
#else
	dwCurTime = g_pController->GetTickCounter();

	DWORD	dwElapseOfflineTime = 0;

	if( Npc[m_NpcIndex].IsPlayer() )
	{
		dwElapseOfflineTime = UNIX_TMIE_STAMP - Player[Npc[m_NpcIndex].GetPlayerIdx()].m_dwLastOfflineTime;
		dwElapseOfflineTime *= 1000;
	}
#endif

	PDBSkillDataVersion1	pSkill = (PDBSkillDataVersion1)pInBuf;

	for(int i = 0; i < nSkillCount; ++i)
	{
		NpcSkillInfo *pInfo = AllocSkillInfo();

		_ASSERT(pInfo);
		if(NULL == pInfo)
			return false;

		m_NpcSkillInfo[i] = pInfo;
		m_NpcSkillInfo[i]->m_SkillId		 = pSkill[i].skillId;
		m_NpcSkillInfo[i]->m_SkillLevel		 = pSkill[i].skillLevel;	
		m_NpcSkillInfo[i]->m_Status			 = pSkill[i].status;
		m_NpcSkillInfo[i]->m_PreStatus		 = pSkill[i].status;

		KSkill *pInstSkill = g_SkillManager.GetSkill(pSkill[i].skillId, 1);

		if(NULL != pInstSkill)
		{
			m_NpcSkillInfo[i]->m_CoolDownTime	 = pInstSkill->GetDelayPerCast();
			m_NpcSkillInfo[i]->m_PreCoolDownTime = m_NpcSkillInfo[i]->m_CoolDownTime;
		}
		else
		{
			_ASSERT(false);
			return false;
		}		

		// 玩家的技能冷却时间下线也需要计时
		// 服务器端根据上次离线的时间重新计算剩余冷却时间
		DWORD	dwLeftCDTime = pSkill[i].leftCoolDownTime;

#ifdef _SERVER
		if(dwElapseOfflineTime >= dwLeftCDTime)
			dwLeftCDTime = 0;
		else
			dwLeftCDTime -= dwElapseOfflineTime;
#endif

		if(dwLeftCDTime > m_NpcSkillInfo[i]->m_CoolDownTime)
			m_NpcSkillInfo[i]->m_NextCastTime = dwCurTime + m_NpcSkillInfo[i]->m_CoolDownTime;
		else
			m_NpcSkillInfo[i]->m_NextCastTime = dwCurTime + dwLeftCDTime;
	}

	return true;
}

#ifdef _SERVER
int NpcSkillList::SaveSkillDataVersion1(BYTE *pOutBuf)
{
	int nSize = 0;
	int	nCount = 0;

	if(pOutBuf)
	{
		DWORD	dwCurTime = g_pController->GetTickCounter();
		PDBSkillDataVersion1 pSkill = (PDBSkillDataVersion1)pOutBuf;

		for(int i = 0; i < __max_skill_count; ++i)
		{
			if(NULL == m_NpcSkillInfo[i])
				continue;

			pSkill->skillId		= (WORD)m_NpcSkillInfo[i]->m_SkillId;
			pSkill->skillLevel	= (WORD)m_NpcSkillInfo[i]->m_SkillLevel;
			
			if(m_NpcSkillInfo[i]->m_NextCastTime > dwCurTime)
				pSkill->leftCoolDownTime = m_NpcSkillInfo[i]->m_NextCastTime - dwCurTime;
			else
				pSkill->leftCoolDownTime = 0;

			// 对于已经学会的技能，存盘的时候不能存当前状态，而应该
			// 存刚学会的时候的初始状态，因为Buff会改变这个状态
			// 而对于没有学会的技能，不用改变
			if(skill_status_active == m_NpcSkillInfo[i]->m_Status ||
				skill_status_usable == m_NpcSkillInfo[i]->m_Status)
				pSkill->status = (BYTE)g_SkillManager.GetSkillInitStatus(m_NpcSkillInfo[i]->m_SkillId);
			else
				pSkill->status = m_NpcSkillInfo[i]->m_Status;

			++pSkill;
			++nCount;
		}

		nSize = sizeof(DBSkillDataVersion1) * nCount;
	}

	return nSize;
}
#endif

bool NpcSkillList::LoadSkillDataVersion2(BYTE *pInBuf, int nSize)
{
	if(NULL == pInBuf)
		return false;

	int nSkillCount = nSize / sizeof(DBSkillData);

	_ASSERT(nSkillCount <= __max_skill_count);	

	if(nSkillCount >= __max_skill_count)
		return false;

	DWORD	dwCurTime;

#ifndef _SERVER
	dwCurTime = GetTickCount();
#else
	dwCurTime = g_pController->GetTickCounter();

	DWORD	dwElapseOfflineTime = 0;

	if( Npc[m_NpcIndex].IsPlayer() )
	{
		dwElapseOfflineTime = UNIX_TMIE_STAMP - Player[Npc[m_NpcIndex].GetPlayerIdx()].m_dwLastOfflineTime;
		dwElapseOfflineTime *= 1000;
	}
#endif

	PDBSkillData	pSkill = (PDBSkillData)pInBuf;

	for(int i = 0; i < nSkillCount; ++i)
	{
		NpcSkillInfo *pInfo = AllocSkillInfo();

		_ASSERT(pInfo);
		if(NULL == pInfo)
			return false;

		m_NpcSkillInfo[i] = pInfo;
		m_NpcSkillInfo[i]->m_SkillId		 = pSkill[i].skillId;
		m_NpcSkillInfo[i]->m_SkillLevel		 = pSkill[i].skillLevel;	
		m_NpcSkillInfo[i]->m_Status			 = pSkill[i].status;
		m_NpcSkillInfo[i]->m_PreStatus		 = pSkill[i].status;

		KSkill *pInstSkill = g_SkillManager.GetSkill(pSkill[i].skillId, 1);

		if(NULL != pInstSkill)
		{
			m_NpcSkillInfo[i]->m_CoolDownTime	 = pInstSkill->GetDelayPerCast();
			m_NpcSkillInfo[i]->m_PreCoolDownTime = m_NpcSkillInfo[i]->m_CoolDownTime;
		}
		else
		{
			_ASSERT(false);
			return false;
		}		

		// 玩家的技能冷却时间下线也需要计时
		// 服务器端根据上次离线的时间重新计算剩余冷却时间
		DWORD	dwLeftCDTime = pSkill[i].leftCoolDownTime;

#ifdef _SERVER
		if(dwElapseOfflineTime >= dwLeftCDTime)
			dwLeftCDTime = 0;
		else
			dwLeftCDTime -= dwElapseOfflineTime;
#endif

		if(dwLeftCDTime > m_NpcSkillInfo[i]->m_CoolDownTime)
			m_NpcSkillInfo[i]->m_NextCastTime = dwCurTime + m_NpcSkillInfo[i]->m_CoolDownTime;
		else
			m_NpcSkillInfo[i]->m_NextCastTime = dwCurTime + dwLeftCDTime;
	}

	return true;
}

#ifdef _SERVER
int NpcSkillList::SaveSkillDataVersion2(BYTE *pOutBuf)
{
	int nSize = 0;
	int	nCount = 0;

	if(pOutBuf)
	{
		DWORD	dwCurTime = g_pController->GetTickCounter();
		PDBSkillData pSkill = (PDBSkillData)pOutBuf;

		for(int i = 0; i < __max_skill_count; ++i)
		{
			if(NULL == m_NpcSkillInfo[i])
				continue;

			pSkill->skillId		= (WORD)m_NpcSkillInfo[i]->m_SkillId;
			pSkill->skillLevel	= (WORD)m_NpcSkillInfo[i]->m_SkillLevel;
			
			if(m_NpcSkillInfo[i]->m_NextCastTime > dwCurTime)
				pSkill->leftCoolDownTime = m_NpcSkillInfo[i]->m_NextCastTime - dwCurTime;
			else
				pSkill->leftCoolDownTime = 0;

			// 对于已经学会的技能，存盘的时候不能存当前状态，而应该
			// 存刚学会的时候的初始状态，因为Buff会改变这个状态
			// 而对于没有学会的技能，不用改变
			if(skill_status_active == m_NpcSkillInfo[i]->m_Status ||
				skill_status_usable == m_NpcSkillInfo[i]->m_Status)
				pSkill->status = (BYTE)g_SkillManager.GetSkillInitStatus(m_NpcSkillInfo[i]->m_SkillId);
			else
				pSkill->status = m_NpcSkillInfo[i]->m_Status;

			++pSkill;
			++nCount;
		}

		nSize = sizeof(DBSkillData) * nCount;
	}

	return nSize;
}
#endif