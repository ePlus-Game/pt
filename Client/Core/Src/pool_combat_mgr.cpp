#include "KCore.h"
#include "pool_combat_mgr.h"
#include "ConfigManager.h"
#include "KSubWorld.h"
#include "ChatCenter_S.h"
#include "KSubWorldSet.h"
#include "KNpcTemplate.h"
#include "buff_man.h"
#include "npc_save.h"
#include "ILogDevice.h"
#include "social_recruit_svr.h"
 
#define  MAX_POOL_MAP_NUM       16
#define  MAP_SETTING_FILE_PATH "/settings/tongwar.ini"

KPoolCombatMgr::KPoolCombatMgr()
:m_CurTimeStamp(UNIX_TMIE_STAMP),
m_GlobalNpcLoadReady(false),m_IsInitedAll(false),m_InvaderLoading(false)
{
	BindStateMachie();
}

KPoolCombatMgr::~KPoolCombatMgr()
{
	/*Do Nothing at all*/
}

void KPoolCombatMgr::BindStateMachie()
{
	m_StateChgFactor[FS_POOL_COMBAT_STATE_PROCESS][FS_POOL_COMBAT_STATE_END].ChgCondTester    = &KPoolCombatMgr::TestFromProcessToEnd;
    m_StateChgFactor[FS_POOL_COMBAT_STATE_PROCESS][FS_POOL_COMBAT_STATE_END].ChgAct           = &KPoolCombatMgr::ChangeFromProcessToEnd;

    m_StateChgFactor[FS_POOL_COMBAT_STATE_END][FS_POOL_COMBAT_STATE_INVALID].ChgCondTester    = &KPoolCombatMgr::TestFromEndToInvalid;
    m_StateChgFactor[FS_POOL_COMBAT_STATE_END][FS_POOL_COMBAT_STATE_INVALID].ChgAct           = &KPoolCombatMgr::ChangeFromEndToInvalid;

    m_CheckStateEnviroment[FS_POOL_COMBAT_STATE_PROCESS] = &KPoolCombatMgr::EnviromentCheckInProcess;
	m_CheckStateEnviroment[FS_POOL_COMBAT_STATE_END]     = &KPoolCombatMgr::EnviromentCheckInEnd;
	m_CheckStateEnviroment[FS_POOL_COMBAT_STATE_INVALID] = &KPoolCombatMgr::EnviromentCheckInInvalid;
}

bool KPoolCombatMgr::CheckWholeWorldState()
{
	if (!GetPoolCombatInfoManager().IsInited())
		return false;
	
	if (!m_GlobalNpcLoadReady)
        return false;
	
	//Check for pool and pool's lord
	bool bAllSuc = true;
	
	WarInfoLoadFlag::iterator  it = m_WarMapLoadFlag.begin();
	while (it!=m_WarMapLoadFlag.end())
	{
		if (!CheckMapConditionLoadState(it))
		{
			bAllSuc = false;
		}//endif
		else
		{
			_ASSERT(it->second.m_IsMapPoolLoaded && it->second.m_IsPoolSocialUnitLoaded );
		}//end else
		
		++it;
	}//end for it
	
	if (bAllSuc)
	{
		//Check Invader record
		bool bAllInvaderReady     = true;
		ServerSocialUnitMgr & mgr = ServerSocialUnitMgr::Singleton();
		
		KPoolCombatInfoManager::SelfIterator it;
		PoolCombatInfo   *                   pCurInfo = GetPoolCombatInfoManager().NextRecord(it);
		
		while (pCurInfo)
		{
			FSGUID invaderGUID = pCurInfo->invaderGUID;	
			SocialUnit * pUnit = mgr.GetUnit(invaderGUID,enSUTplId_Tong);
			if (!pUnit)
			{
				if (!CheckLoadState(invaderGUID))
				{
					bAllInvaderReady = false;
					
					if (!m_InvaderLoading)
					{
						SocialSerializer & ss = SocialSerializer::Singleton();
						ss.LoadTreeUpReq(-1,enSUTplId_Tong,invaderGUID);
					}//endif
					
				}//endif
				
			}//endif
			
			pCurInfo = GetPoolCombatInfoManager().NextRecord(it);
		}//end for while
		
		m_InvaderLoading = true;
		
		if (bAllInvaderReady)
		{
			ServerSocialUnitMgr::Singleton().OnMapPoolLoadReady();
			
			WarInfoLoadFlag::iterator iter = m_WarMapLoadFlag.begin();
			while (iter!=m_WarMapLoadFlag.end())
			{
				int iWorldIndex = g_SubWorldSet.SearchWorld(iter->first);
				
				if (iWorldIndex!=INVALID_WORLD_INDEX)
					CheckMapPoolAndSocialUnit(iter->first,iWorldIndex);
				
				++iter;
			}//end for it
			
			DumpWarMapComplete();
			m_IsInitedAll = true;

            #ifdef _DEBUG
			CFS_FILELOGS::WriteDebugLog("PoolWarMap InitedComplete All.\n");
            #endif

			return true;
		}//endif

	}//endif
	
	return false;
}

void KPoolCombatMgr::InitWarMapSetting()
{
	KIniFile kWarFile;
	if(kWarFile.Load(MAP_SETTING_FILE_PATH))
	{
       for (int i = 1;i<=MAX_POOL_MAP_NUM ; i++)
	   {
		   char szKeyName[32];
		   snprintf(szKeyName,sizeof(szKeyName),"map%d",i);
		   szKeyName[31]=0;

		   int  iWordMapId = -1;
		   kWarFile.GetInteger("PoolWarMapID",szKeyName,-1,&iWordMapId);

		   if (iWordMapId!=INVALID_WORLD_ID)
		   {
               WarInfoLoadFlag::iterator  it = m_WarMapLoadFlag.find(iWordMapId);
			   if (it == m_WarMapLoadFlag.end())
			   {
				   MapPoolLoadingCondition                   initCond;
				   pair<WarInfoLoadFlag::iterator, bool>     ret;
				   ret = m_WarMapLoadFlag.insert( WarInfoLoadFlag::value_type(iWordMapId, initCond) );
				   _ASSERT(ret.second);
				   
				   it=ret.first;
			   }//endif

		   }//endif

	   }//end for i

	}//endif
}

bool KPoolCombatMgr::CheckMapConditionLoadState(WarInfoLoadFlag::iterator &  it)
{
	if (it == m_WarMapLoadFlag.end())
		return false;

	ServerSocialUnitMgr& gSocial =	ServerSocialUnitMgr::Singleton();

	int iSubwordIndex = g_SubWorldSet.SearchWorld(it->first);
	if (iSubwordIndex ==INVALID_WORLD_INDEX)
		return false ;

	//Check pool Load state
	if (!it->second.m_IsMapPoolLoaded)
	{
        int iPoolNpc = SubWorld[iSubwordIndex].GetPool();

        if (IsValidNpc(iPoolNpc))
		{
            if (Npc[iPoolNpc].GetLoadSaveState()==npc_load_save_state_loaded)
			{
				it->second.m_IsMapPoolLoaded = true;

				if (Npc[iPoolNpc].GetRobber().data[0]!=0 || Npc[iPoolNpc].GetCurrentLifePercentage()<=5 )
				{
					if (!GetPoolCombatInfoManager().ExistRecord(it->first))
					{
						FSGUID invalid;
						Npc[iPoolNpc].SetRobber(invalid);
						Npc[iPoolNpc].SetCurrentLifePercentage(Npc[iPoolNpc].GetCurrentLifePercentage() + 30);
					}//endif

				}//endif

			}//endif
			else
			{
				if (!it->second.m_IsPoolLoading)
				{
					NpcSave::LoadNpc(iPoolNpc);
					it->second.m_IsPoolLoading = true;
				}//endif

			}//end else

		}//endif
	
	}//endif 

	if (!it->second.m_IsMapPoolLoaded)
		return false;

    //Check pool socialunit state
	if (!it->second.m_IsPoolSocialUnitLoaded)
	{
		if (Npc[SubWorld[iSubwordIndex].GetPool()].GetLord().data[0]!=0)
		{
            SocialUnit * pSocialUnit = gSocial.GetUnit(Npc[SubWorld[iSubwordIndex].GetPool()].GetLord());
			
			if (pSocialUnit)
			{
                it->second.m_IsPoolSocialUnitLoaded = true;
			    it->second.m_IsPoolSocialUnitLoading= true;

			}//end if
			else
			{
                if (!it->second.m_IsPoolSocialUnitLoading)
				{
                    SocialSerializer & ss = SocialSerializer::Singleton();
					ss.LoadTreeUpReq(-1,enSUTplId_Tong,Npc[SubWorld[iSubwordIndex].GetPool()].GetLord());
                    it->second.m_IsPoolSocialUnitLoading = true;
				}//endif

			}//end else

		}//endif
		else
		{
			it->second.m_IsPoolSocialUnitLoaded = true;
			it->second.m_IsPoolSocialUnitLoading= true;
		}//end else

	}//endif

	if (!it->second.m_IsPoolSocialUnitLoaded)
		return false;
	
	//subunit load state
	int nSubPoolIndex = SubWorld[iSubwordIndex].GetSubPool();
	if (IsValidNpc(nSubPoolIndex))
	{
		it->second.m_IsHasSubPool    = true;
		if (!it->second.m_IsSubPoolLoaded)
		{
			if (Npc[nSubPoolIndex].GetLoadSaveState() == npc_load_save_state_loaded)
			{
				it->second.m_IsSubPoolLoaded = true;
				
				FSGUID                                  subPoolRober = Npc[nSubPoolIndex].GetRobber();
				KPoolCombatInfoManager                 & gPoolInfoMgr= GetPoolCombatInfoManager();
				KPoolCombatInfoManager::SelfIterator            iter;
				PoolCombatInfo      *                           pCur = gPoolInfoMgr.NextRecord(iter);

				bool                                            bValid = false;

				while ( pCur)
				{
					if (pCur->mapID == it->first && pCur->invaderGUID == subPoolRober && pCur->warState != FS_POOL_COMBAT_STATE_INVALID)
					{
						bValid = true;
						break;
					}//endif

					pCur = gPoolInfoMgr.NextRecord(iter);
				}//end for while

				if (!bValid)
				{
			        it->second.m_IsHasSubPool    = false;
					
					Npc[nSubPoolIndex].m_UnaryAttrMgr.Set(nuai_curlife, 0);
					Npc[nSubPoolIndex].SendCommand(do_death,0,0,0);
					Npc[nSubPoolIndex].GetController().SetActive(false);
					Npc[nSubPoolIndex].SetProcessAI(TRUE);
             		
					SubWorld[iSubwordIndex].SetSubPool(-1);
				}//endif

			}//endif
			else
			{
				it->second.m_IsSubPoolLoaded = false;
				if (!it->second.m_IsSubPoolLoading)
				{
					NpcSave::LoadNpc(nSubPoolIndex);
					it->second.m_IsSubPoolLoading = true;
				}//endif

			}//end else

		}//endif	

	}//endif
	else
	{
		it->second.m_IsHasSubPool    = false;
		it->second.m_IsSubPoolLoaded = true;
		it->second.m_IsSubPoolLoading= true;
	}//end else

	if (!it->second.m_IsSubPoolLoaded )
		return false;

	return true;
}

bool KPoolCombatMgr::IsInitedAll()const
{
	return m_IsInitedAll;
}

bool KPoolCombatMgr::IsPoolCombatMap(const int nMapID)
{
	WarInfoLoadFlag::iterator  it = m_WarMapLoadFlag.find(nMapID);
	if (it == m_WarMapLoadFlag.end())
		return false;
	else 
		return true;
}

void KPoolCombatMgr::OnNpcSaveLoadComplete()
{
    m_GlobalNpcLoadReady = true;
}

void KPoolCombatMgr::OnTongLoaded(const FSGUID & guid)
{
	WarInfoLoadFlag::iterator  it = m_WarMapLoadFlag.begin();
	while (it!=m_WarMapLoadFlag.end())
	{
		int nSubwordIndex = g_SubWorldSet.SearchWorld(it->first);
		if (nSubwordIndex!=INVALID_WORLD_INDEX)
		{ 
            int iPoolNpc = SubWorld[nSubwordIndex].GetPool();
			if (IsValidNpc(iPoolNpc))
			{
				if (Npc[iPoolNpc].GetLord()==guid)
					it->second.m_IsPoolSocialUnitLoaded = true;
			}//endif

		}//endif
		
		++it;
	}//end for it

	m_SocialLoadedRec.push_back(guid);
}

void KPoolCombatMgr::EnHanceANewCombat(const FSGUID & guid,int nMapID)
{
	int  nSubWorldIndex      = g_SubWorldSet.SearchWorld(nMapID);
	if (INVALID_WORLD_INDEX == nSubWorldIndex)
	{
		_ASSERT(false);
		return;
	}//endif

	int  nPoolIndex          = SubWorld[nSubWorldIndex].GetPool();
	if (!IsValidNpc(nPoolIndex))
	{
		_ASSERT(false);
		return ;
	}//endif
	
	if (guid.data[0] == 0 )
	{
		_ASSERT(false);
		return ;
	}//endif
	
	//1.加出守护星..........................................................................
	
	ConfigManager    & cm       = ConfigManager::Singleton();
	int nSubPoolId              = cm.GetGlobalVariable(global_var_pool_combat_npc_id);
	int nSubPoolLevel           = cm.GetGlobalVariable(global_var_pool_combat_npc_level);
	int nOffsetX                = cm.GetGlobalVariable(global_var_pool_combat_npc_offset_x);
	int nOffsetY                = cm.GetGlobalVariable(global_var_pool_combat_npc_offset_y);
	int nX,nY;
	
	Npc[nPoolIndex].GetMpsPos( &nX, &nY );
	
	int	nNpcIdxInfo = MAKELONG(nSubPoolLevel, nSubPoolId);
	
	int nNpcIdx = NpcSet.Add(
		nNpcIdxInfo, 
		nSubWorldIndex, 
		nX + nOffsetX, 
		nY + nOffsetY);

	if (!IsValidNpc(nNpcIdx))
	{
		_ASSERT(false);
		return ;
	}//endif
	
	Npc[nNpcIdx].NormalSync( );
	
	int nMode = Npc[nNpcIdx].m_UnaryAttrMgr[nuai_deathmode];
	nMode |= npc_deathmode_autodel;
	Npc[nNpcIdx].m_UnaryAttrMgr.Set( nuai_deathmode, nMode );
	
	const KNpcTemplate *pTemplate = Npc[nNpcIdx].GetTemplate();

	Npc[nNpcIdx].SetLord(Npc[nPoolIndex].GetLord());
	Npc[nNpcIdx].SetRobber(guid);
	
	if( NULL != pTemplate && pTemplate->NeedSave() )
		Npc[nNpcIdx].Save();

	//2.SetSubPool
	SubWorld[nSubWorldIndex].SetSubPool(nNpcIdx);

	PoolCombatInfo             info;
	info.invaderGUID         = guid;
	info.mapID               = nMapID;
	info.warProTime          = 0;
	info.warState            = FS_POOL_COMBAT_STATE_PROCESS;
	
	bool bRes = GetPoolCombatInfoManager().AddRecord(info);
	_ASSERT(bRes);

	if (bRes)
	{
		WarBeginNotify(nMapID,guid);
	}//endif

}

void KPoolCombatMgr::Breathe()
{
	//Check Load state
	if (!m_IsInitedAll)
	{
        CheckWholeWorldState();
		return;
	}//endif
	
	if ( UNIX_TMIE_STAMP - m_CurTimeStamp >= 1 )   //一秒种执行一次
	{
		KPoolCombatInfoManager    & gPoolInfoMgr=GetPoolCombatInfoManager();
	    ServerSocialUnitMgr       & gSocialMgr  =ServerSocialUnitMgr::Singleton();

		KPoolCombatInfoManager::SelfIterator    iter;
		PoolCombatInfo                     *    pCurInfo = gPoolInfoMgr.NextRecord(iter);
		
		while (pCurInfo)
		{
			_ASSERT( pCurInfo->warState < FS_POOL_COMBAT_TOTAL_STATE_NUM && pCurInfo->warState>=FS_POOL_COMBAT_STATE_INVALID );
			
			if (pCurInfo->warState>=FS_POOL_COMBAT_TOTAL_STATE_NUM || pCurInfo->warState < FS_POOL_COMBAT_STATE_INVALID)
			{
				pCurInfo = gPoolInfoMgr.NextRecord(iter);
				continue;
			}//endif
			
			//1. 确认当前状态的运行环境,如果运行环境不正确说明数据库存取有问题，
			//   首先会做一定的修复，如果无法修复则直接废弃这条记录.
			
			StateEnviroment se;
			
			if (!(this->*(m_CheckStateEnviroment[pCurInfo->warState]))(pCurInfo,&se))
			{
				pCurInfo = gPoolInfoMgr.NextRecord(iter);
				continue;
			}//endif
			
			//2.状态机转换判断
			
			for (int state = FS_POOL_COMBAT_STATE_INVALID; state < FS_POOL_COMBAT_TOTAL_STATE_NUM ; state ++ )
			{
				//2.1 判断当前状态到state状态是否满足条件 :
				if ( m_StateChgFactor[pCurInfo->warState][state].ChgCondTester!=NULL &&
					(this->*(m_StateChgFactor[pCurInfo->warState][state].ChgCondTester))(pCurInfo,&se) )
				{
					
					//2.2 满足条件则做转换操作 :
					if (m_StateChgFactor[pCurInfo->warState][state].ChgAct!=NULL)
						(this->*(m_StateChgFactor[pCurInfo->warState][state].ChgAct))(pCurInfo,&se);
					
                    DumpTongWarStateChange(pCurInfo,pCurInfo->warState,state);
					
					pCurInfo->warState = state;
					gPoolInfoMgr.NotifyToSaveDB();
					
				}//endif
				
			}//end for state
			
			pCurInfo = gPoolInfoMgr.NextRecord(iter);
			
		}//end for while
		
		m_CurTimeStamp = UNIX_TMIE_STAMP;
	}//endif
}

#define BOSS_AREA_ADJUST 40

void KPoolCombatMgr::InvaderTongWarMsgNotify(int nMapID,SocialUnit * pInvaderUnit,const char * szWarMsg)
{
	_ASSERT(pInvaderUnit && szWarMsg);
	
	if (pInvaderUnit==NULL || szWarMsg == NULL)	
		return ;

	char szMapName[MAXSIZE_WORLD_NAME];
	
	if( !g_SubWorldSet.GetWorldNameFromID(nMapID, szMapName, sizeof(szMapName)) )
		return ;
	
	szMapName[sizeof(szMapName) - 1] = 0;
	
	char szMsg[MAXSIZE_CHAT_MSG];
	int	nMsgLen;
	nMsgLen = snprintf(szMsg, sizeof(szMsg), szWarMsg, szMapName);
	
	szMsg[MAXSIZE_CHAT_MSG-1] =0;

	if(nMsgLen < 0 || nMsgLen >= sizeof(szMsg))
		return;
	
	g_ChatCenterS.SysMsgToTong(pInvaderUnit, 
		SYSMSG_TYPE_STR, 
		(const BYTE*)&szMsg, 
		nMsgLen
		);
}

void KPoolCombatMgr::WarBeginNotify(int nMapID,const FSGUID & guid)
{
	ServerSocialUnitMgr &   mgr = ServerSocialUnitMgr::Singleton();
	SocialUnit     *pTargetUnit = mgr.GetUnit(guid);
	if (pTargetUnit == NULL)
	{
		_ASSERT(false);
		return;
	}//endif
	
	const char *szUnitName = GetUnitName( pTargetUnit->GetUnitAttr() );
	
	if(NULL == szUnitName)
		return;
	
	int  nWorldId = nMapID;
	char szWorldName[MAXSIZE_WORLD_NAME];
	g_SubWorldSet.GetWorldNameFromID(nWorldId, szWorldName, sizeof(szWorldName));
	szWorldName[MAXSIZE_WORLD_NAME - 1] = 0;

	char szMsg[256];
	int nLen = snprintf(szMsg, sizeof(szMsg), MSG_POOL_DECLAREWAR, szUnitName, szWorldName);
	
	if(nLen < 0 || nLen >= sizeof(szMsg))
	{
		nLen = 0;
		szMsg[nLen] = 0;
	}
	
	g_ChatCenterS.SysMsgToAll(SYSMSG_TYPE_STR, 
		(const BYTE*)szMsg, 
		nLen,
		SYSTEM_ROOM_ID,
		MSG_SHOWTYPE_ROOM | MSG_SHOWTYPE_MIDDLESCREEN | MSG_SHOWTYPE_TOPSCREEN
	);
}

void KPoolCombatMgr::DefenderTongWarMsgNotify(int nMapID,SocialUnit * pDefender,SocialUnit * pInvaderUnit ,const char * szWarMsg)
{
	_ASSERT(pDefender && pInvaderUnit && szWarMsg);
	
	if (pDefender==NULL|| pInvaderUnit ==NULL || szWarMsg == NULL)	
		return ;
	
	SocialUnitAttr & attr = pInvaderUnit->GetUnitAttr();
	if (!attr.IsAttrHasData(enSUAttr_UnitName))
		return ;

	char * szUnitName=NULL;
	int    nNameSize = attr.GetAttr(enSUAttr_UnitName,szUnitName); 

	if (szUnitName == NULL || nNameSize == 0)
		return ;

	char szMapName[MAXSIZE_WORLD_NAME];
	
	if( !g_SubWorldSet.GetWorldNameFromID(nMapID, szMapName, sizeof(szMapName)) )
		return ;
	
	szMapName[sizeof(szMapName) - 1] = 0;
	
	char szMsg[MAXSIZE_CHAT_MSG];
	int	nMsgLen;
	
	nMsgLen = snprintf(szMsg, sizeof(szMsg),szWarMsg, szUnitName,szMapName);
	
	szMsg[MAXSIZE_CHAT_MSG-1]=0;

	if(nMsgLen < 0 || nMsgLen >= sizeof(szMsg))
		return;
	
	g_ChatCenterS.SysMsgToTong(pDefender, 
		SYSMSG_TYPE_STR, 
		(const BYTE*)&szMsg, 
		nMsgLen
	);
}

bool KPoolCombatMgr::TestFromProcessToEnd(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	_ASSERT(pWarInfo && pWarInfo->warState == FS_POOL_COMBAT_STATE_PROCESS );
    BuffMgr & buffman              = BuffMgr::Singleton();

	//1.诸侯解散
	if (lpOutSE->pInvader==NULL)
	{
        return true;
	}//endif

	ConfigManager    & cm        = ConfigManager::Singleton();
	int         nLifeDeathPercent= cm.GetGlobalVariable(global_var_tong_war_death_percent);
	int         nInvincibilityBuf= cm.GetGlobalVariable(global_var_tong_war_Invincibility_buff);

	//2.分星池被打到死亡界限
    if (Npc[lpOutSE->nMapPoolNpcIndex].GetCurrentLifePercentage()<=nLifeDeathPercent)
	{
		FSGUID    invalidguid;
		Npc[lpOutSE->nMapPoolNpcIndex].SetRobber(invalidguid);      //使它不再受伤害
		return true;
	}//endif

	//3.战争时间判断
	int nWarProcessMaxTime = cm.GetGlobalVariable(global_var_pool_combat_process_t);
	if (pWarInfo->warProTime >= nWarProcessMaxTime)
		return true;

	//5.战争时的内部Breathe
	pWarInfo->warProTime+=1;
	if (pWarInfo->warProTime % 600 == 0) //十分钟存储一下时间
		GetPoolCombatInfoManager().NotifyToSaveDB();

	//当子领主被打掉的时候才让振诸侯木被打
	bool      bSetRober            = true;
	int       nSubPool             = SubWorld[lpOutSE->nMapWorldIndex].GetSubPool();

	int       nSubPoolCombatValidTime = cm.GetGlobalVariable(global_var_pool_combat_npc_against_time);
	if (nSubPoolCombatValidTime <= 0) //Forbid invalid settings  
		nSubPoolCombatValidTime = nWarProcessMaxTime;

	//与守护星争斗时间
	if (IsValidNpc(nSubPool) && Npc[nSubPool].GetCurrentLifePercentage() > cm.GetGlobalVariable(global_var_pool_combat_npc_alert_per) && pWarInfo->warProTime >= nSubPoolCombatValidTime)
		return true;

	if (IsValidNpc(nSubPool) && Npc[nSubPool].GetCurrentLifePercentage()>nLifeDeathPercent)
	{
		bSetRober            = false;
	}//endif

	if (bSetRober && Npc[lpOutSE->nMapPoolNpcIndex].GetRobber() != lpOutSE->pInvader->GetUnitGuid())
	{
		Npc[lpOutSE->nMapPoolNpcIndex].SetRobber(lpOutSE->pInvader->GetUnitGuid());
		Npc[lpOutSE->nMapPoolNpcIndex].Save();

    }//endif
	

	return false;
}

void KPoolCombatMgr::JurgeWinFailed(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	bool bInvaderWin=false;
	
	if (lpOutSE->pInvader)
	{
		ConfigManager    & cm         = ConfigManager::Singleton();
		int         nLifeDeathPercent = cm.GetGlobalVariable(global_var_tong_war_death_percent);
		//分星池
		if (Npc[lpOutSE->nMapPoolNpcIndex].GetCurrentLifePercentage()<=nLifeDeathPercent)	
		{
			bInvaderWin = true;
		}//endif
		
	}//endif
	
	if (bInvaderWin)
	{
		_ASSERT(lpOutSE->pInvader);
		//攻击方胜利
        SocialUnitAttr & attr = lpOutSE->pInvader->GetUnitAttr();
		
		if (lpOutSE->pDefender == NULL)
		{
			//首次攻城战
			GainPool(pWarInfo->mapID,lpOutSE->nMapWorldIndex,lpOutSE->pInvader);
		}//endif
		else
		{
			//夺城战争
			RobPool(pWarInfo->mapID,lpOutSE->nMapWorldIndex,lpOutSE->pInvader,lpOutSE->pDefender);
			DefenderTongWarMsgNotify(pWarInfo->mapID,lpOutSE->pDefender,lpOutSE->pInvader,MSG_WAR_DEFENDER_POOL_LOSE);
		}//end else
		
        InvaderTongWarMsgNotify(pWarInfo->mapID,lpOutSE->pInvader,MSG_WAR_POOL_INVADER_WIN);
		ProtectPool(lpOutSE->nMapWorldIndex);
	}//endif
	else
	{
        if (lpOutSE->pInvader)
		{
            InvaderTongWarMsgNotify(pWarInfo->mapID,lpOutSE->pInvader,MSG_WAR_INVADER_POOL_LOSS);
		}//endif

		if (lpOutSE->pDefender && lpOutSE->pInvader)
		{
            DefenderTongWarMsgNotify(pWarInfo->mapID,lpOutSE->pDefender,lpOutSE->pInvader,MSG_WAR_POOL_DEFENDER_WIN);
		}//endf

	}//end for else

}

void KPoolCombatMgr::GainPool(int nPoolMapId,int nPoolWorldIndex,SocialUnit * pInvader)
{
    _ASSERT(pInvader);
	
	if(pInvader)
	{
		SocialUnitAttr	&attr = pInvader->GetUnitAttr();
		attr.AddAttr(enSUAttr_PoolMap, (const char*)&nPoolMapId, sizeof(nPoolMapId));
		SocialSerializer::Singleton().UpdateAttrReq(-1, pInvader);
        
		//如果宕在这里，战争记录也停在ProcessState,那么初始化的时候，在SocialUnitMgr::OnMapLordLoadReady 时，会
		//把这个属性去掉.重走正常流程。

		int nPoolNpcIndex = SubWorld[nPoolWorldIndex].GetPool();
		_ASSERT(IsValidNpc(nPoolNpcIndex));

		Npc[nPoolNpcIndex].SetLord(pInvader->GetUnitGuid());
		Npc[nPoolNpcIndex].Save();

		int nSubPoolIndex = SubWorld[nPoolWorldIndex].GetSubPool();
		if (IsValidNpc(nSubPoolIndex))
		{
			Npc[nSubPoolIndex].SetLord(pInvader->GetUnitGuid());
			Npc[nSubPoolIndex].Save();
		}//endif

	}//endif
	
}

void KPoolCombatMgr::RobPool(int nPoolMapId,int nPoolWorldIndex,SocialUnit * pInvader,SocialUnit * pDefender)
{
	_ASSERT(pInvader && pDefender);
    if(pInvader && pDefender)
	{
		SocialUnitAttr	&invaderAttr = pInvader->GetUnitAttr();
		invaderAttr.AddAttr(enSUAttr_PoolMap, (const char*)&nPoolMapId, sizeof(nPoolMapId));
		SocialSerializer::Singleton().UpdateAttrReq(-1, pInvader);
		
		//如果宕在这里，战争记录也停在ProcessState,那么初始化的时候，在SocialUnitMgr::OnMapLordLoadReady 时，会
		//把这个属性去掉.重走正常流程。

		int nPoolNpcIndex = SubWorld[nPoolWorldIndex].GetPool();
		_ASSERT(IsValidNpc(nPoolNpcIndex));
		
		Npc[nPoolNpcIndex].SetLord(pInvader->GetUnitGuid());
		Npc[nPoolNpcIndex].Save();

		int nSubPoolIndex = SubWorld[nPoolWorldIndex].GetSubPool();
		if (IsValidNpc(nSubPoolIndex))
		{
			Npc[nSubPoolIndex].SetLord(pInvader->GetUnitGuid());
			Npc[nSubPoolIndex].Save();
		}//endif

		//如果宕在这里，defender的Attr会在SocialUnitMgr::OnMapLordLoadReady 时去掉，
		//重走正常流程时,会当作无效记录

		SocialUnitAttr	&defenderAttr = pDefender->GetUnitAttr();
		defenderAttr.DelAttr(enSUAttr_PoolMap);
		SocialSerializer::Singleton().UpdateAttrReq(-1, pDefender);
	}//endif
	
}

void KPoolCombatMgr::ChangeFromProcessToEnd(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	_ASSERT(pWarInfo && pWarInfo->warState == FS_POOL_COMBAT_STATE_PROCESS);	
	JurgeWinFailed(pWarInfo,lpOutSE);
}

bool KPoolCombatMgr::TestFromEndToInvalid(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	return true;
}

void KPoolCombatMgr::ChangeFromEndToInvalid(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	_ASSERT(pWarInfo && pWarInfo->warState == FS_POOL_COMBAT_STATE_END && lpOutSE->nMapWorldIndex!=INVALID_WORLD_INDEX);

	ClearMapWarState(pWarInfo);
}

bool KPoolCombatMgr::EnviromentCheckInProcess(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	_ASSERT(pWarInfo && lpOutSE);

	bool                      bValid     = true;
	//1.Check SocialUnit...
	ServerSocialUnitMgr     & gSocialMgr = ServerSocialUnitMgr::Singleton();
	
	lpOutSE->pInvader       = gSocialMgr.GetUnit(pWarInfo->invaderGUID);
	lpOutSE->nMapWorldIndex = g_SubWorldSet.SearchWorld(pWarInfo->mapID);
	
	//2.Check World id ...
	if (lpOutSE->nMapWorldIndex == INVALID_WORLD_INDEX)
	{
		//Exception!
		_ASSERT(false);
		pWarInfo->warState =  FS_POOL_COMBAT_STATE_INVALID;
		bValid             =  false;
	}//endif
    else
	{
		//3.Pool Npc Index...
		lpOutSE->nMapPoolNpcIndex = SubWorld[lpOutSE->nMapWorldIndex].GetPool();
	
		if (!IsValidNpc(lpOutSE->nMapPoolNpcIndex))
		{
			//Exception!
			_ASSERT(false);
			pWarInfo->warState = FS_POOL_COMBAT_STATE_INVALID;
			bValid             =  false;
		}//endif
		else
		{
			//4.Defender SocialUnit
			lpOutSE->pDefender = gSocialMgr.GetUnit(Npc[lpOutSE->nMapPoolNpcIndex].GetLord(),enSUTplId_Tong);
		}//endif
		
		//Check Pool Attr
		if (bValid && lpOutSE->pInvader)
		{
			int iInvaderMap = GetPoolMapId(lpOutSE->pInvader->GetUnitAttr());
			if (iInvaderMap != INVALID_WORLD_ID )
			{
				//Exception!
				_ASSERT(false);
				pWarInfo->warState = FS_POOL_COMBAT_STATE_INVALID;
				bValid             =  false;

				CheckClearPoolState(lpOutSE->nMapWorldIndex);
			}//endif
			
		}//endif

	}//endif
	
	//Check and add subpool，DBError ,need fix
	if (bValid)
	{
		int nSubPool = SubWorld[lpOutSE->nMapWorldIndex].GetSubPool();
		if (Npc[lpOutSE->nMapPoolNpcIndex].GetRobber().data[0]==0)
		{
			if (!IsValidNpc(nSubPool))
			{
				ConfigManager    & cm       = ConfigManager::Singleton();
				int nSubPoolId              = cm.GetGlobalVariable(global_var_pool_combat_npc_id);
				int nSubPoolLevel           = cm.GetGlobalVariable(global_var_pool_combat_npc_level);
				int nOffsetX                = cm.GetGlobalVariable(global_var_pool_combat_npc_offset_x);
				int nOffsetY                = cm.GetGlobalVariable(global_var_pool_combat_npc_offset_y);
				int nX,nY;
				
				Npc[lpOutSE->nMapPoolNpcIndex].GetMpsPos( &nX, &nY );
				
				int	nNpcIdxInfo = MAKELONG(nSubPoolLevel, nSubPoolId);
				
				int nNpcIdx = NpcSet.Add(
					nNpcIdxInfo, 
					lpOutSE->nMapWorldIndex, 
					nX + nOffsetX, 
					nY + nOffsetY);
				
				if (!IsValidNpc(nNpcIdx))
				{
					_ASSERT(false);
					return false;
				}//endif
				
				Npc[nNpcIdx].NormalSync( );
				
				int nMode = Npc[nNpcIdx].m_UnaryAttrMgr[nuai_deathmode];
				nMode |= npc_deathmode_autodel;
				Npc[nNpcIdx].m_UnaryAttrMgr.Set( nuai_deathmode, nMode );
				
				const KNpcTemplate *pTemplate = Npc[nNpcIdx].GetTemplate();
				
				Npc[nNpcIdx].SetLord(Npc[lpOutSE->nMapWorldIndex].GetLord());
				Npc[nNpcIdx].SetRobber(lpOutSE->pInvader->GetUnitGuid());
				
				if( NULL != pTemplate && pTemplate->NeedSave() )
					Npc[nNpcIdx].Save();
				
				//2.SetSubPool
				SubWorld[lpOutSE->nMapWorldIndex].SetSubPool(nNpcIdx);
				
			}//endif
			else
			{
				if (Npc[nSubPool].GetLord().data[0] == 0)
				{
					Npc[nSubPool].SetLord(Npc[lpOutSE->nMapPoolNpcIndex].GetLord());
				}//endif

				if (Npc[nSubPool].GetRobber() != pWarInfo->invaderGUID)
				{
					Npc[nSubPool].SetRobber(pWarInfo->invaderGUID);
				}//endif

			}//end else

		}//endif

	}//endif
	
	if (pWarInfo->warState==FS_POOL_COMBAT_STATE_INVALID)
		DumpInvalidWarInfo(pWarInfo);
		
	return bValid;
}

bool KPoolCombatMgr::EnviromentCheckInEnd(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE)
{
    _ASSERT(pWarInfo && lpOutSE);
	
	bool                      bValid     = true;
	
	ServerSocialUnitMgr     & gSocialMgr = ServerSocialUnitMgr::Singleton();
	
	lpOutSE->pInvader       = gSocialMgr.GetUnit(pWarInfo->invaderGUID);
	lpOutSE->nMapWorldIndex = g_SubWorldSet.SearchWorld(pWarInfo->mapID);
	
	if (lpOutSE->nMapWorldIndex == INVALID_WORLD_INDEX)
	{
		//Exception!
		_ASSERT(false);
		pWarInfo->warState =  FS_POOL_COMBAT_STATE_INVALID;
		bValid             =  false;
	}//endif
	else
	{
		lpOutSE->nMapPoolNpcIndex = SubWorld[lpOutSE->nMapWorldIndex].GetPool();
		
		if (!IsValidNpc(lpOutSE->nMapPoolNpcIndex))
		{
			//Exception!
			_ASSERT(false);
			pWarInfo->warState = FS_POOL_COMBAT_STATE_INVALID;
			bValid             =  false;
		}//endif

	}//end else

	if (pWarInfo->warState==FS_POOL_COMBAT_STATE_INVALID)
		DumpInvalidWarInfo(pWarInfo);

	return bValid;
}

bool KPoolCombatMgr::EnviromentCheckInInvalid(PoolCombatInfo * pWarInfo,StateEnviroment * lpOutSE)
{
    _ASSERT(pWarInfo && lpOutSE);

	int iSubWordIndex = g_SubWorldSet.SearchWorld(pWarInfo->mapID);
	if (iSubWordIndex == INVALID_WORLD_INDEX)
	{
		return false;
	}//endif

	CheckClearPoolState(iSubWordIndex);

    return true;
}

void KPoolCombatMgr::ClearMapWarState(PoolCombatInfo * pWarInfo)
{
	_ASSERT(pWarInfo);

	//恢复城市  ..................................................................
	int nSubWorldIndex= g_SubWorldSet.SearchWorld(pWarInfo->mapID);
	if (INVALID_WORLD_INDEX == nSubWorldIndex)
		return;

	//去掉地图Pool的Rober.并恢血.........................................................
	int iPoolNpcIndex = SubWorld[nSubWorldIndex].GetPool();
	if (IsValidNpc(iPoolNpcIndex))
	{
		FSGUID    guid;
		int nCurLifePercentage = Npc[iPoolNpcIndex].GetCurrentLifePercentage();
		int nNewLifePercentage = nCurLifePercentage +30;
		if (nNewLifePercentage > 100)
			nNewLifePercentage = 100;

		Npc[iPoolNpcIndex].SetCurrentLifePercentage(nNewLifePercentage);
	    Npc[iPoolNpcIndex].SetRobber(guid);
		Npc[iPoolNpcIndex].Save();
	}//endif
	
	//去掉SubPool..........................................................................
	int nSubPool = SubWorld[nSubWorldIndex].GetSubPool();
	if (IsValidNpc(nSubPool))
	{
		Npc[nSubPool].m_UnaryAttrMgr.Set(nuai_curlife, 0);
		Npc[nSubPool].SendCommand(do_death,0,0,0);
		Npc[nSubPool].GetController().SetActive(false);
		Npc[nSubPool].SetProcessAI(TRUE);
	}//endif

	SubWorld[nSubWorldIndex].SetSubPool(-1);
}

#define MAX_INVALID_INFO_LEN 1024

void KPoolCombatMgr::DumpInvalidWarInfo(PoolCombatInfo * pWarInfo)
{
    _ASSERT(pWarInfo);   
    _ASSERT(false);
	
	if (pWarInfo)
	{
		char szLogString[MAX_INVALID_INFO_LEN];
		char szInvader[34];
		
		if (pWarInfo->invaderGUID.data[0]!=0)
			memcpy(szInvader,pWarInfo->invaderGUID.data,33);
		else
			sprintf(szInvader,"NoSocialUnit");
		
		szInvader[33]  =0;
		szLogString[0] = 0;

		snprintf(szLogString,sizeof(szLogString),"Invalid PoolCombat clear:mapID %d,InvaderGUID: %s ",pWarInfo->mapID,szInvader);
        szLogString[MAX_INVALID_INFO_LEN -1] = 0;
		
		int nSize      =strlen(szLogString);
		g_pLogSystem->SysDbgLog(szLogString,nSize,sys_dbg_log_event_pool_combat);
	}//endif

}

void KPoolCombatMgr::DumpTongWarStateChange(PoolCombatInfo * pWarInfo,int oldstate,int newstate)
{
    _ASSERT(pWarInfo);   
	
	if (pWarInfo)
	{
		char szLogString[MAX_INVALID_INFO_LEN];
		char szInvader[34];
		
		if (pWarInfo->invaderGUID.data[0]!=0)
			memcpy(szInvader,pWarInfo->invaderGUID.data,33);
		else
			sprintf(szInvader,"NoSocialUnit");
		
		szInvader[33]  =0;
		szLogString[0] = 0;
		
		snprintf(szLogString,sizeof(szLogString),"PoolWar StateChange:mapID %d,InvaderGUID: %s ChangeFrom %d to %d."
			,pWarInfo->mapID,szInvader,oldstate,newstate);

        szLogString[MAX_INVALID_INFO_LEN -1] = 0;
		
		int nSize      =strlen(szLogString);
		g_pLogSystem->SysDbgLog(szLogString,nSize,sys_dbg_log_event_pool_combat);

	}//endif

}

void KPoolCombatMgr::DumpInvalidMapPool(int nMapID,const FSGUID & guid)
{
	_ASSERT(false);   
	//Log to DB
	char szLogString[MAX_INVALID_INFO_LEN];
	char szUnitGUID[34];
	
	memcpy(szUnitGUID,guid.data,33);
	szUnitGUID[33]=0;
	
	snprintf(szLogString,sizeof(szLogString),"Invalid Pool,GUID:%s MapId:%d",szUnitGUID,nMapID);
	szLogString[MAX_INVALID_INFO_LEN -1] = 0;
	
	int nSize      =strlen(szLogString);
	
	g_pLogSystem->SysDbgLog(szLogString,nSize,sys_dbg_log_event_pool_combat);
}
 
void KPoolCombatMgr::DumpWarMapComplete()
{
	char szLogString[MAX_INVALID_INFO_LEN];
	snprintf(szLogString,sizeof(szLogString),"Pool War Map Init Ready!");
	szLogString[MAX_INVALID_INFO_LEN -1] = 0;
	int nSize      =strlen(szLogString);
	g_pLogSystem->SysDbgLog(szLogString,nSize,sys_dbg_log_event_pool_combat);
}

bool KPoolCombatMgr::CheckLoadState(const FSGUID & guid)
{
	std::list<FSGUID>::iterator it = m_SocialLoadedRec.begin();
	while (it != m_SocialLoadedRec.end())
	{
		if (*it == guid)
			return true;

		++it;
	}//end for while

	return false;
}

void KPoolCombatMgr::ProtectPool(const int nSubWorldIndex)
{
	if ( nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
	{
		int nPoolNpcIndex  = SubWorld[nSubWorldIndex].GetPool();
		int nProtectBuffID = ConfigManager::Singleton().GetGlobalVariable(global_var_pool_combat_success_buff);
		
		if (IsValidNpc(nPoolNpcIndex) && nProtectBuffID )
		{
			BuffMgr & buffman              = BuffMgr::Singleton();
			buffman.AddNpcBuff(nPoolNpcIndex,nPoolNpcIndex,nProtectBuffID);
		}//endif

	}//endif

}

void KPoolCombatMgr::CheckClearPoolState(const int nSubWorldIndex)
{
	int  nPool        = SubWorld[nSubWorldIndex].GetPool();
	int  nSubPool     = SubWorld[nSubWorldIndex].GetSubPool();
	bool bClearRobber = (IsValidNpc(nPool) && Npc[nPool].GetRobber().data[0]!=0);
	bool bDelSubPool  = (IsValidNpc(nSubPool) && Npc[nSubPool].m_Doing!= do_death && Npc[nSubPool].m_Doing!= do_revive);
	int  nMapID       = SubWorld[nSubWorldIndex].m_SubWorldID;

	if (bClearRobber || bDelSubPool)
	{	
		KPoolCombatInfoManager::SelfIterator it;
		PoolCombatInfo   *                   pCurInfo = GetPoolCombatInfoManager().NextRecord(it);
		
		while (pCurInfo)
		{
			if (pCurInfo->mapID == nMapID || pCurInfo->warState != FS_POOL_COMBAT_STATE_INVALID)
			{
				bClearRobber = false;
				bDelSubPool  = false;
			}//endif
			
			pCurInfo = GetPoolCombatInfoManager().NextRecord(it);
		}//end for while
		
	}//endif
	
	if (bClearRobber)
	{
		_ASSERT(false);
		FSGUID guid;
		Npc[nPool].SetRobber(guid);
		Npc[nPool].Save();
	}//endif
	
	if (bDelSubPool)
	{
		_ASSERT(false);
		Npc[nSubPool].m_UnaryAttrMgr.Set(nuai_curlife, 0);
		Npc[nSubPool].SendCommand(do_death,0,0,0);
		Npc[nSubPool].GetController().SetActive(false);
		Npc[nSubPool].SetProcessAI(TRUE);
		SubWorld[nSubWorldIndex].SetSubPool(-1);
	}//endif
}

void KPoolCombatMgr::CheckMapPoolAndSocialUnit(int nMapID,int iSubwordIndex)
{
	ServerSocialUnitMgr& gSocial     = ServerSocialUnitMgr::Singleton();
	int                  nPoolIndex = SubWorld[iSubwordIndex].GetPool();

	_ASSERT(IsValidNpc(nPoolIndex));

	const FSGUID &       lordGUID    = Npc[nPoolIndex].GetLord();

	if (lordGUID.data[0]!=0)
	{ 
		bool         bReset    = true;
		SocialUnit * pLordUnit = gSocial.GetUnit(lordGUID);
		if (pLordUnit)
		{
			SocialUnitAttr  & attr    = pLordUnit->GetUnitAttr();
			if (GetPoolMapId(attr)==nMapID)
			{
				bReset = false;
			}//endif
			
		}//endif
		
		if (bReset)
		{
			_ASSERT(false);
			//可能诸侯在占领城后宕机，造成数据存储不对.
			DumpInvalidMapPool(nMapID,lordGUID);
			
			FSGUID           invalid;
			Npc[nPoolIndex].SetLord(invalid);
			Npc[nPoolIndex].Save();

		}//endif

	}//endif
}

KPoolCombatMgr & GetGlobalPoolCombatMgr(void)
{
	static KPoolCombatMgr g_PoolWarMgr;
	return g_PoolWarMgr;
}