#include "KCore.h"
#include "tong_war_manager.h"
#include "ConfigManager.h"
#include "KSubWorld.h"
#include "ChatCenter_S.h"
#include "KSubWorldSet.h"
#include "KNpcTemplate.h"
#include "buff_man.h"
#include "npc_save.h"
#include "ILogDevice.h"
#include "social_recruit_svr.h"
#include "KNpc.h"
#include "ScriptFuns.h"
#include "keconomysys.h"
 
#define  MAX_WAR_MAP_NUM                    4
#define  MAP_SETTING_FILE_PATH              "/settings/tongwar.ini"
#define  FS_WAR_NOTIFY_PROCESS_MIN_INTERVAL 24 * 3600

/*********************************************************************************************/
/* Notice:CityAttr 已经改为不存盘的属性，如果改回存盘属性，需要在改变的时候立即UpdateAttrReq */
/*********************************************************************************************/

KTongWarManager::KTongWarManager()
:m_CurTimeStamp(UNIX_TMIE_STAMP),
m_GlobalNpcLoadReady(false),m_IsInitedAll(false)
{
	BindStateMachie();
}

KTongWarManager::~KTongWarManager()
{
	/*Do Nothing at all*/
}

void KTongWarManager::BindStateMachie()
{
    m_StateChgFactor[FS_WAR_STATE_NOTIFY][FS_WAR_STATE_PROCESS].ChgCondTester = &KTongWarManager::TestFromNotifyToProcess;
    m_StateChgFactor[FS_WAR_STATE_NOTIFY][FS_WAR_STATE_PROCESS].ChgAct        = &KTongWarManager::ChangeFromNotifyToProcess;

	m_StateChgFactor[FS_WAR_STATE_NOTIFY][FS_WAR_STATE_END].ChgCondTester     = &KTongWarManager::TestFromNotifyToEnd;
    m_StateChgFactor[FS_WAR_STATE_NOTIFY][FS_WAR_STATE_END].ChgAct            = &KTongWarManager::ChangeFromNotifyToEnd;

	m_StateChgFactor[FS_WAR_STATE_PROCESS][FS_WAR_STATE_END].ChgCondTester    = &KTongWarManager::TestFromProcessToEnd;
    m_StateChgFactor[FS_WAR_STATE_PROCESS][FS_WAR_STATE_END].ChgAct           = &KTongWarManager::ChangeFromProcessToEnd;

    m_StateChgFactor[FS_WAR_STATE_END][FS_WAR_STATE_INVALID].ChgCondTester    = &KTongWarManager::TestFromEndToInvalid;
    m_StateChgFactor[FS_WAR_STATE_END][FS_WAR_STATE_INVALID].ChgAct           = &KTongWarManager::ChangeFromEndToInvalid;

    m_CheckStateEnviroment[FS_WAR_STATE_NOTIFY]  = &KTongWarManager::EnviromentCheckInNotify;
    m_CheckStateEnviroment[FS_WAR_STATE_PROCESS] = &KTongWarManager::EnviromentCheckInProcess;
	m_CheckStateEnviroment[FS_WAR_STATE_END]     = &KTongWarManager::EnviromentCheckInEnd;
	m_CheckStateEnviroment[FS_WAR_STATE_INVALID] = &KTongWarManager::EnviromentCheckInInvalid;
}

bool KTongWarManager::CheckWholeWorldState()
{
	if (!GetGlobalWarInfoManager().IsInited())
		return false;
	
	if (!m_GlobalNpcLoadReady)
        return false;
	
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
			_ASSERT(it->second.m_IsMapLordLoaded && it->second.m_IsLordSocialUnitLoaded );
			_ASSERT(!it->second.m_HasRobber || (it->second.m_HasRobber && it->second.m_IsRobberLoaded && it->second.m_IsRobberSocialUnitLoaded));
            
            #ifdef _DEBUG
			CFS_FILELOGS::WriteDebugLog("TongWarMap %d InitedComplete.\n",it->first);
            #endif

		}//end else

		++it;
	}//end for it
	
	if (bAllSuc)
	{
		
		it = m_WarMapLoadFlag.begin();
		while (it!=m_WarMapLoadFlag.end())
		{
			int iWorldIndex = g_SubWorldSet.SearchWorld(it->first);
			if (iWorldIndex!=INVALID_WORLD_INDEX)
				CheckMapLordAndSocialUnit(it->first,iWorldIndex);
			++it;
		}//end for it

		DumpWarMapComplete();

        #ifdef _DEBUG
		CFS_FILELOGS::WriteDebugLog("TongWarMap InitedComplete All.\n");
        #endif

		m_IsInitedAll = true;
		return true;
	}//endif
	else
		return false;
}



void KTongWarManager::InitWarMapSetting()
{
	KIniFile kWarFile;
	if(kWarFile.Load(MAP_SETTING_FILE_PATH))
	{
       for (int i = 1;i<=MAX_WAR_MAP_NUM ; i++)
	   {
		   char szKeyName[32];
		   snprintf(szKeyName,sizeof(szKeyName),"map%d",i);
		   szKeyName[31]=0;

		   int  iWordMapId = -1;
		   kWarFile.GetInteger("TongWarMapID",szKeyName,-1,&iWordMapId);

		   if (iWordMapId!=INVALID_WORLD_ID)
		   {
               WarInfoLoadFlag::iterator  it = m_WarMapLoadFlag.find(iWordMapId);
			   if (it == m_WarMapLoadFlag.end())
			   {
				   MapWarLoadCondition                   initCond;
				   pair<WarInfoLoadFlag::iterator, bool> ret;
				   ret = m_WarMapLoadFlag.insert( WarInfoLoadFlag::value_type(iWordMapId, initCond) );
				   _ASSERT(ret.second);
				   
				   it=ret.first;
			   }//endif

		   }//endif

	   }//end for i

	}//endif
}

bool KTongWarManager::IsTongWarMap(const int nMapID)
{
	WarInfoLoadFlag::iterator  it = m_WarMapLoadFlag.find(nMapID);
	if (it == m_WarMapLoadFlag.end())
		return false;
	else
		return true;
}

bool KTongWarManager::CheckMapConditionLoadState(WarInfoLoadFlag::iterator &  it)
{
	if (it==m_WarMapLoadFlag.end())
		return false;

	ServerSocialUnitMgr& gSocial =	ServerSocialUnitMgr::Singleton();

	int iSubwordIndex = g_SubWorldSet.SearchWorld(it->first);
	if (iSubwordIndex ==INVALID_WORLD_INDEX)
		return false ;

	//Check Lord Load state
	if (!it->second.m_IsMapLordLoaded)
	{
        int iLordNpc = SubWorld[iSubwordIndex].GetLord();

        if (IsValidNpc(iLordNpc))
		{
            if (Npc[iLordNpc].GetLoadSaveState()==npc_load_save_state_loaded)
			{
				it->second.m_IsMapLordLoaded = true;

				if (!GetGlobalWarInfoManager().ExistWarInfo(it->first))
				{
					if (Npc[iLordNpc].GetRobber().data[0]!=0)
					{
						_ASSERT(false);

						FSGUID  guid;
						Npc[iLordNpc].SetRobber(guid);
						Npc[iLordNpc].Save();
					}//endif

				}//endif

			}//endif
			else
			{
				if (!it->second.m_IsLordLoading)
				{
					NpcSave::LoadNpc(iLordNpc);
					it->second.m_IsLordLoading = true;
				}//endif

			}//end else

		}//endif
	
	}//endif 

	if (!it->second.m_IsMapLordLoaded)
		return false;

	if (!IsValidNpc(SubWorld[iSubwordIndex].GetLord()))
		return false;

    //Check lord socialunit state
	if (!it->second.m_IsLordSocialUnitLoaded)
	{
		if (Npc[SubWorld[iSubwordIndex].GetLord()].GetLord().data[0]!=0)
		{
            SocialUnit * pSocialUnit = gSocial.GetUnit(Npc[SubWorld[iSubwordIndex].GetLord()].GetLord());
			
			if (pSocialUnit)
			{
                it->second.m_IsLordSocialUnitLoaded = true;
			    it->second.m_IsLordSocialUnitLoading= true;

			}//end if
			else
			{
                if (!it->second.m_IsLordSocialUnitLoading)
				{
                    SocialSerializer & ss = SocialSerializer::Singleton();
					ss.LoadTreeUpReq(-1,enSUTplId_Tong,Npc[SubWorld[iSubwordIndex].GetLord()].GetLord());
                    it->second.m_IsLordSocialUnitLoading = true;
				}//endif

			}//end else

		}//endif
		else
		{
			it->second.m_IsLordSocialUnitLoaded = true;
			it->second.m_IsLordSocialUnitLoading= true;
		}//end else

	}//endif

	if (!it->second.m_IsLordSocialUnitLoaded)
		return false;

	int nSubLordLoadedFlagCount  = 0;
	int nSubLordLoadingFlagCount = 0;
	int nSubLordCount  = 0;
	
	for (int nSubLordIter = 0; nSubLordIter < SUBLORD_COUNT; ++nSubLordIter)
	{
		int nSubLordNpcIndex = SubWorld[iSubwordIndex].GetSubLord(nSubLordIter);

		if (!IsValidNpc(nSubLordNpcIndex))
			continue;

		nSubLordCount++;	//记录子领主个数

		
		if (Npc[nSubLordNpcIndex].GetLoadSaveState() == npc_load_save_state_loaded)
		{
			nSubLordLoadedFlagCount++;	//已loaded个数

			if (!GetGlobalWarInfoManager().ExistWarInfo(it->first))
			{
				if (Npc[nSubLordNpcIndex].GetRobber().data[0] != 0)
				{
					_ASSERT(false);

					FSGUID validGuid;

					Npc[nSubLordNpcIndex].SetRobber(validGuid);
					Npc[nSubLordNpcIndex].Save();
				}
			}
		}
		else
		{

			if (!it->second.m_IsSubLordLoading)
			{
				NpcSave::LoadNpc(nSubLordNpcIndex);

				nSubLordLoadingFlagCount++;	//loading个数
			}
		}
	}

	if (nSubLordLoadingFlagCount != 0)
	{
		it->second.m_IsSubLordLoading = true;
		return false;
	}

	if (nSubLordCount != nSubLordLoadedFlagCount)
	{
		return false;
	}

	int iRoberNpc = SubWorld[iSubwordIndex].GetRobber();
	
	if (!IsValidNpc(iRoberNpc))
	{
        it->second.m_HasRobber       = false;
	}//endif
	else
	{
		it->second.m_HasRobber       = true ;

		//Check Robber Load state
		if (!it->second.m_IsRobberLoaded)
		{
            if (Npc[iRoberNpc].GetLoadSaveState() == npc_load_save_state_loaded)
			{
				it->second.m_IsRobberLoaded = true;
				
				if (!GetGlobalWarInfoManager().ExistWarInfo(it->first))  //Invalid
				{
					int nRegion        = Npc[iRoberNpc].m_RegionIndex;

                    SubWorld[iSubwordIndex].SetRobber(-1);
					Npc[iRoberNpc].m_UnaryAttrMgr.Set(nuai_curlife, 0);
                    Npc[iRoberNpc].SendCommand(do_death,0,0,0);
					Npc[iRoberNpc].GetController().SetActive(false);
					Npc[iRoberNpc].SetProcessAI(TRUE);
					Npc[iRoberNpc].ProcCommand(TRUE);

					BuffMgr &mgr = BuffMgr::Singleton();
					mgr.ClearAllBuff( iRoberNpc, TRUE );
					
					SubWorld[iSubwordIndex].m_Region[nRegion].RemoveNpc(iRoberNpc);
	                NpcSet.Remove(iRoberNpc);

					return false;
				}//endif

			}//endif
			else
			{
				if (!it->second.m_IsRobberLoading)
				{
                    NpcSave::LoadNpc(iRoberNpc);
					it->second.m_IsRobberLoading = true;
				}//endif

			}//end else

		}//endif
		
		if (!it->second.m_IsRobberLoaded)
			return false;

		//Robber Socialunit
		if (!it->second.m_IsRobberSocialUnitLoaded)
		{
            if (Npc[iRoberNpc].GetLord().data[0]!=0)
			{
				SocialUnit * pSocialUnit = gSocial.GetUnit(Npc[iRoberNpc].GetLord());
				
				if (pSocialUnit)
				{
					it->second.m_IsRobberSocialUnitLoaded = true;
			     	it->second.m_IsRobberSocialUnitLoading= true;
				}//end if
				else
				{
					if (!it->second.m_IsRobberSocialUnitLoading)
					{
						SocialSerializer & ss = SocialSerializer::Singleton();
						ss.LoadTreeUpReq(-1,enSUTplId_Tong,Npc[iRoberNpc].GetLord());
						it->second.m_IsRobberSocialUnitLoading = true;
					}//endif
					
				}//end else
				
			}//endif 
			else
			{
				it->second.m_IsRobberSocialUnitLoaded = true;
				it->second.m_IsRobberSocialUnitLoading= true;
			}//end else

		}//endif

        if (!it->second.m_IsRobberSocialUnitLoaded)
			return false;

	}//endif

	return true;
}

bool KTongWarManager::IsInitedAll()const
{
	return m_IsInitedAll;
}

void KTongWarManager::OnNpcSaveLoadComplete()
{
    m_GlobalNpcLoadReady = true;
}

void KTongWarManager::OnTongLoaded(const FSGUID & guid)
{
	WarInfoLoadFlag::iterator  it = m_WarMapLoadFlag.begin();
	while (it!=m_WarMapLoadFlag.end())
	{
		int nSubwordIndex = g_SubWorldSet.SearchWorld(it->first);
		if (nSubwordIndex!=INVALID_WORLD_INDEX)
		{ 
            int iLordNpc = SubWorld[nSubwordIndex].GetLord();
			if (IsValidNpc(iLordNpc))
			{
				if (Npc[iLordNpc].GetLord()==guid)
					it->second.m_IsLordSocialUnitLoaded = true;
			}//endif

			int iRobNpc  = SubWorld[nSubwordIndex].GetRobber();
			if (IsValidNpc(iRobNpc))
			{
                if (Npc[iRobNpc].GetLord()==guid)
					it->second.m_IsRobberSocialUnitLoaded = true;
			}//endif

		}//endif
		
		++it;
	}//end for it
}

void KTongWarManager::Breathe()
{
	//Check Load state
	if (!m_IsInitedAll)
	{
        CheckWholeWorldState();
		return;
	}//endif
	
	if ( UNIX_TMIE_STAMP - m_CurTimeStamp >=1 )   //一秒种执行一次
	{
		KWarInfoManager    & gWarInfoMgr=GetGlobalWarInfoManager();
	    ServerSocialUnitMgr& gSocialMgr =ServerSocialUnitMgr::Singleton();

		KWarInfoManager::SelfIterator   iter;
		FSWarInfo                  *    pCurInfo = gWarInfoMgr.NextRecord(iter);
		
		while (pCurInfo)
		{
			_ASSERT( pCurInfo->warState < FS_WAR_TOTAL_STATE_NUM && pCurInfo->warState>=FS_WAR_STATE_INVALID );
			
			if (pCurInfo->warState>=FS_WAR_TOTAL_STATE_NUM || pCurInfo->warState < FS_WAR_STATE_INVALID)
			{
				pCurInfo = gWarInfoMgr.NextRecord(iter);
				continue;
			}//endif
			
			//1. 确认当前状态的运行环境,如果运行环境不正确说明数据库存取有问题，
			//   首先会做一定的修复，如果无法修复则直接废弃这条记录.
			
			StateEnviroment se;
			
			if (!(this->*(m_CheckStateEnviroment[pCurInfo->warState]))(pCurInfo,&se))
			{
				pCurInfo = gWarInfoMgr.NextRecord(iter);
				continue;
			}//endif
			
			//2.状态机转换判断
			
			for (int state = FS_WAR_STATE_INVALID; state < FS_WAR_TOTAL_STATE_NUM ; state ++ )
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
					gWarInfoMgr.NotifyToSaveDB();
					
				}//endif
				
			}//end for state
			
			pCurInfo = gWarInfoMgr.NextRecord(iter);
			
		}//end for while
		
		m_CurTimeStamp = UNIX_TMIE_STAMP;
	}//endif
}

bool KTongWarManager::TestFromNotifyToProcess(FSWarInfo * pWarInfo,StateEnviroment * pSE)
{
    _ASSERT(pWarInfo && pWarInfo->warState == FS_WAR_STATE_NOTIFY);
	_ASSERT(pSE);
	
	tm              notifytime;
	tm              curtime;
	
	time_t          tNotifyTime = pWarInfo->warDecTime;
	if (localtime(&tNotifyTime))
		memcpy(&notifytime,localtime(&tNotifyTime),sizeof(notifytime));
	
	time_t          tCurentTime = UNIX_TMIE_STAMP;
	if (localtime(&tCurentTime))
		memcpy(&curtime,localtime(&tCurentTime),sizeof(curtime));   
	
	
	ConfigManager    & cm       = ConfigManager::Singleton();
	int    nWarStartTimeH       = cm.GetGlobalVariable(global_var_tong_war_start_time);
	
	time_t tInterval = FS_WAR_NOTIFY_PROCESS_MIN_INTERVAL;
	if (notifytime.tm_hour == nWarStartTimeH)
		tInterval += 3600;
	
	if ( tNotifyTime + tInterval <= tCurentTime)
	{
		//宣战之后的某天,可以是第二天或者因维护引起的时间差
		if (curtime.tm_hour == nWarStartTimeH ) 
		{
			if ( (pSE->pInvader && pSE->pDefender)            //掠夺 
				|| 
				(pSE->pInvader && pWarInfo->defenderGUID.data[0]==0) //攻击在野城市
				) 
			{
				return true;
			}//endif
			
		}//endif
		
	}//endif
	
	return false;
}

#define BOSS_AREA_ADJUST 40

void KTongWarManager::ChangeFromNotifyToProcess(FSWarInfo * pWarInfo,StateEnviroment * pSE)
{
	_ASSERT(pWarInfo);
	_ASSERT(pSE);

	//注意：如果这个条件不满足，不会进入这个函数，见 EnviromentCheckInNotify
	
	_ASSERT(pSE->pInvader && pSE->nMapWorldIndex!=INVALID_WORLD_INDEX &&
		IsValidNpc(pSE->nMapLordNpcIndex) && IsValidNpc(pSE->nMapRobberNpcIndex)); 

	if (NULL == pSE || NULL == pSE->pInvader || INVALID_WORLD_INDEX == pSE->nMapWorldIndex || !IsValidNpc(pSE->nMapLordNpcIndex) || !IsValidNpc(pSE->nMapRobberNpcIndex))
		return ;

    /***************************************************************************
     *初始化诸侯战                             
	 ***************************************************************************/

	//1.如果防守方有诸侯,只有防守方才可以对祭镇坛造成伤害.....................................................
    if (pSE->pDefender)
	{	
		Npc[pSE->nMapRobberNpcIndex].SetRobber(pSE->pDefender->GetUnitGuid());
		Npc[pSE->nMapRobberNpcIndex].Save();
	}//endif
	//..........................................................................................................

	//2.初次攻城，即防守方没有诸侯，则在镇诸侯木周围产出BOSS守城..................................................
	if (pSE->pDefender==NULL)
	{
		ConfigManager    & cm       = ConfigManager::Singleton();
		int nBossTemplateID         = cm.GetGlobalVariable(global_var_tong_war_boss_tempID);
		int nBossLevel              = cm.GetGlobalVariable(global_var_tong_war_boss_level);
		int nBossNum                = cm.GetGlobalVariable(global_var_tong_war_boss_num);
			
		int nX,nY;
		int nWorldIndex = Npc[pSE->nMapLordNpcIndex].GetSubWorldIndex( );
		
		Npc[pSE->nMapLordNpcIndex].GetMpsPos( &nX, &nY );
		
		for( int nLoopCount = 0; nLoopCount < nBossNum; nLoopCount++ )
		{
			int	nNpcIdxInfo = MAKELONG(nBossLevel, nBossTemplateID + nLoopCount);

			int nAdjustPosX =0;
			int nAdjustPosY =0;
			
			//散开
			if (nLoopCount %2)
			{
				nAdjustPosX =  nLoopCount * BOSS_AREA_ADJUST;
				nAdjustPosY = -nLoopCount * BOSS_AREA_ADJUST;
			}//endif
			else
			{
				nAdjustPosX = -nLoopCount * BOSS_AREA_ADJUST;
				nAdjustPosY =  nLoopCount * BOSS_AREA_ADJUST;
			}//end else

			int nNpcIdx = NpcSet.Add(
				nNpcIdxInfo, 
				nWorldIndex, 
				nX + nAdjustPosX, 
				nY + nAdjustPosY);
			
			if( nNpcIdx > 0 )
			{
				Npc[nNpcIdx].NormalSync( );

				int nMode = Npc[nNpcIdx].m_UnaryAttrMgr[nuai_deathmode];
				nMode |= npc_deathmode_autodel;
				Npc[nNpcIdx].m_UnaryAttrMgr.Set( nuai_deathmode, nMode );
				
				const KNpcTemplate *pTemplate = Npc[nNpcIdx].GetTemplate();
				if( NULL != pTemplate && pTemplate->NeedSave() )
					Npc[nNpcIdx].Save();
			}//endif

		}//end for loopcount
		
	}//endif

	//3 去掉宣战方战后保护Buff..................................................................................................
    if (pSE->pInvader)
		CheckRemoveProtectBuff(pSE->pInvader);

	for (int nSubLordIter = 0; nSubLordIter< SUBLORD_COUNT ;nSubLordIter ++ )
	{
		int iSubLordIndex = SubWorld[pSE->nMapWorldIndex].GetSubLord(nSubLordIter);
		
		if (!IsValidNpc(iSubLordIndex))
			continue;
		
		if (Npc[iSubLordIndex].GetRobber() != pSE->pInvader->GetUnitGuid())
		{
			Npc[iSubLordIndex].SetRobber(pSE->pInvader->GetUnitGuid());
			Npc[iSubLordIndex].Save();
		}
		
	}//end for nSublord

	//.......................................................................................................
	//5.Init war time
	pWarInfo->warProTime = 0;
    //6.提示诸侯战开始.........................................................................................
	bool      bProtected = false ; //是否有阵眼

	int       nLifeDeathPercent    = ConfigManager::Singleton().GetGlobalVariable(global_var_tong_war_death_percent);
	int       nLordScapegoatBuffID = ConfigManager::Singleton().GetGlobalVariable(global_var_tong_war_scapegoat_buff);
	
	for (int nSubLord = 0; nSubLord< SUBLORD_COUNT ;nSubLord ++ )
	{
		int iSubLordIndex = SubWorld[pSE->nMapWorldIndex].GetSubLord(nSubLord);
        if (IsValidNpc(iSubLordIndex))
		{
			if (Npc[iSubLordIndex].GetLoadSaveState() == npc_load_save_state_waiting || Npc[iSubLordIndex].GetLoadSaveState() ==  npc_load_save_state_loading )
				continue;
			
			if(BuffMgr::Singleton().IsHaveBuff(iSubLordIndex,nLordScapegoatBuffID) && Npc[iSubLordIndex].GetCurrentLifePercentage() > nLifeDeathPercent)
			{
				bProtected    = true;
				break;
			}//endif
			
		}//endif
		
	}//end for nSublord

	if (bProtected)
	{
		InvaderTongWarMsgNotify(pWarInfo->mapID,pSE->pInvader,MSG_WAR_NOTIFY_START_PROTECTING);
	}
	else
		InvaderTongWarMsgNotify(pWarInfo->mapID,pSE->pInvader,MSG_WAR_NOTIFY_START);

	if (pSE->pDefender)
		DefenderTongWarMsgNotify(pWarInfo->mapID,pSE->pDefender,pSE->pInvader,MSG_WAR_DEFENDER_ONWAR);
	//.......................................................................................................
}

bool KTongWarManager::TestFromNotifyToEnd(FSWarInfo * pWarInfo,StateEnviroment * pSE)
{
	_ASSERT(pWarInfo && pWarInfo->warState == FS_WAR_STATE_NOTIFY);
	_ASSERT(pSE);	
				
	if (pSE->pInvader==NULL || (pSE->pDefender==NULL && pWarInfo->defenderGUID.data[0]!=0)) //双方诸侯有任意一方已经不存在(解散)
	{
		return true;
	}//endif
				
	return false;
}

void KTongWarManager::InvaderTongWarMsgNotify(int nMapID,SocialUnit * pInvaderUnit,const char * szWarMsg)
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

void KTongWarManager::DefenderTongWarMsgNotify(int nMapID,SocialUnit * pDefender,SocialUnit * pInvaderUnit ,const char * szWarMsg)
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

#define FORCE_SAVE_INTERVAL_TIME 300

bool KTongWarManager::TestFromProcessToEnd(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	_ASSERT(pWarInfo && pWarInfo->warState == FS_WAR_STATE_PROCESS );
	_ASSERT(lpOutSE);
	_ASSERT(lpOutSE->nMapWorldIndex!=INVALID_WORLD_INDEX && IsValidNpc(lpOutSE->nMapLordNpcIndex) && IsValidNpc(lpOutSE->nMapRobberNpcIndex));	

	if (INVALID_WORLD_INDEX == lpOutSE->nMapWorldIndex || !IsValidNpc(lpOutSE->nMapLordNpcIndex) || !IsValidNpc(lpOutSE->nMapRobberNpcIndex))
		return true;

    BuffMgr & buffman              = BuffMgr::Singleton();

	//1.诸侯解散
	if ( (pWarInfo->defenderGUID.data[0]!=0 && lpOutSE->pDefender==NULL) || (lpOutSE->pInvader==NULL))
	{
        return true;
	}//endif

	ConfigManager    & cm        = ConfigManager::Singleton();
	int         nLifeDeathPercent= cm.GetGlobalVariable(global_var_tong_war_death_percent);
	int         nInvincibilityBuf= cm.GetGlobalVariable(global_var_tong_war_Invincibility_buff);

	//2.镇诸侯木被打到死亡界限的血
    if (Npc[lpOutSE->nMapLordNpcIndex].GetCurrentLifePercentage()<=nLifeDeathPercent)
	{
		buffman.AddNpcBuff(lpOutSE->nMapLordNpcIndex,lpOutSE->nMapLordNpcIndex,nInvincibilityBuf);
		return true;
	}//endif

	//3.祭镇坛被打到死亡界限的血
	if (Npc[lpOutSE->nMapRobberNpcIndex].GetCurrentLifePercentage()<=nLifeDeathPercent)
	{
		buffman.AddNpcBuff(lpOutSE->nMapLordNpcIndex,lpOutSE->nMapLordNpcIndex,nInvincibilityBuf);
		return true;
	}//endif

	//4.战争时间判断
	int nWarProcessMaxTime = cm.GetGlobalVariable(global_var_tong_war_process_time);
	if (pWarInfo->warProTime >= nWarProcessMaxTime)
		return true;

	//5.战争时的内部Breathe
	pWarInfo->warProTime+=1;
	if (pWarInfo->warProTime % 600 == 0) //十分钟存储一下时间
		GetGlobalWarInfoManager().NotifyToSaveDB();

	if (pWarInfo->warProTime % FORCE_SAVE_INTERVAL_TIME == 0)
	{
		for (int i = 0; i < SUBLORD_COUNT; ++i)
		{
			int nSubLordIndex = SubWorld[lpOutSE->nMapWorldIndex].GetSubLord(i);
			
			if (!IsValidNpc(nSubLordIndex))
				continue;
			
			NpcSave::SaveNpc(nSubLordIndex);	
		}
	}

	//当子领主被打掉的时候才让振诸侯木被打
	int       nLordScapegoatBuffID = cm.GetGlobalVariable(global_var_tong_war_scapegoat_buff);
	bool      bSetRober            = true;

	for (int nSubLord = 0; nSubLord< SUBLORD_COUNT ;nSubLord ++ )
	{
		int iSubLordIndex = SubWorld[lpOutSE->nMapWorldIndex].GetSubLord(nSubLord);
        if (IsValidNpc(iSubLordIndex))
		{
			if (Npc[iSubLordIndex].GetLoadSaveState() == npc_load_save_state_waiting || Npc[iSubLordIndex].GetLoadSaveState() ==  npc_load_save_state_loading )
				bSetRober    = false;
			else if(buffman.IsHaveBuff(iSubLordIndex,nLordScapegoatBuffID) && Npc[iSubLordIndex].GetCurrentLifePercentage() > nLifeDeathPercent)
			{
				bSetRober    = false;
				break;
			}//endif

		}//endif

	}//end for nSublord

	if (bSetRober && Npc[lpOutSE->nMapLordNpcIndex].GetRobber() != lpOutSE->pInvader->GetUnitGuid())
	{
		Npc[lpOutSE->nMapLordNpcIndex].SetRobber(lpOutSE->pInvader->GetUnitGuid());
		Npc[lpOutSE->nMapLordNpcIndex].Save();
    }//endif

	return false;
}

void KTongWarManager::JurgeWinFailed(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	_ASSERT(lpOutSE && IsValidNpc(lpOutSE->nMapLordNpcIndex) && IsValidNpc(lpOutSE->nMapRobberNpcIndex));
	if ( !(lpOutSE && IsValidNpc(lpOutSE->nMapLordNpcIndex) && IsValidNpc(lpOutSE->nMapRobberNpcIndex)))
		return ;

	bool bInvaderWin=false;
	
	if (lpOutSE->pInvader)
	{
		if (lpOutSE->pDefender==NULL && pWarInfo->defenderGUID.data[0]!=0)
		{
			//防守方诸侯解散
			bInvaderWin = true;
		}
		else
		{
			ConfigManager    & cm         = ConfigManager::Singleton();
			int         nLifeDeathPercent = cm.GetGlobalVariable(global_var_tong_war_death_percent);
			//镇诸侯木掉血到死亡界限
			if (Npc[lpOutSE->nMapLordNpcIndex].GetCurrentLifePercentage()<=nLifeDeathPercent)	
			{
				bInvaderWin = true;
			}//endif
			
		}//end else
		
	}//endif

	int nRobberCityMapID = INVALID_WORLD_ID;
	if (lpOutSE->pInvader)
	{
		SocialUnitAttr& attr = lpOutSE->pInvader->GetUnitAttr();

		nRobberCityMapID     = GetCityMapId(attr);
		
	}

	int nChangeCityBuffID = ConfigManager::Singleton().GetGlobalVariable(global_var_tong_war_change_city_buff);
	
	//在这里判断是否有防守方
	bool bHasDefender = false;
	if (lpOutSE->pDefender)
		bHasDefender = true;
		
	int ScriptParam = 0;
	/* ScriptParam的解析:
		| ----------- | ----------- | ---------- | ---------- |
		   RobberMapId   WarMapId      WarResult    WarMode
	*/
	FillMapInfo(nRobberCityMapID, pWarInfo->mapID, ScriptParam);
		
	if (bInvaderWin)
	{
		_ASSERT(lpOutSE->pInvader);
		//攻击方胜利
        
		if (nRobberCityMapID != INVALID_WORLD_ID)
		{
			if (IsValidNpc(lpOutSE->nMapRobberNpcIndex) && BuffMgr::Singleton().IsHaveBuff(lpOutSE->nMapRobberNpcIndex,nChangeCityBuffID))
			{
				//换城
				if (lpOutSE->nMapWorldIndex>=0 && lpOutSE->nMapWorldIndex < MAX_SUBWORLD )	
				{
					if (bHasDefender)
						FillWarStateInfo(war_result_invader_win, city_vs_city_change_mode, ScriptParam);
					else
						FillWarStateInfo(war_result_invader_win, city_vs_nocity_change_mode, ScriptParam);

					ChangeCity(nRobberCityMapID,SubWorld[lpOutSE->nMapWorldIndex].GetWorldTemplateId(),lpOutSE->pInvader,lpOutSE->pDefender);
					
					if ( bHasDefender )
						DefenderTongWarMsgNotify(pWarInfo->mapID,lpOutSE->pDefender,lpOutSE->pInvader,MSG_WAR_DEFENDER_CITY_LOSE);
				}
			}
			else
			{	
				//掠夺
				int nRoberCityMapIndex = g_SubWorldSet.SearchWorld(nRobberCityMapID);
				
				if (nRoberCityMapIndex!=INVALID_WORLD_INDEX)
				{
					int iRoberCityNpcIndex  = SubWorld[nRoberCityMapIndex].GetLord();
					if (IsValidNpc(lpOutSE->nMapRobberNpcIndex))
					{	
						const char *szTongName = GetUnitName( lpOutSE->pInvader->GetUnitAttr() );
						if (szTongName)
						{
							if (bHasDefender)
								FillWarStateInfo(war_result_invader_win, city_vs_city_rob_mode, ScriptParam);
							else
								FillWarStateInfo(war_result_invader_win, city_vs_nocity_rob_mode, ScriptParam);

							RobRes(lpOutSE->nMapLordNpcIndex,iRoberCityNpcIndex,pWarInfo->mapID,szTongName);
						}
					}//endif
					
				}//endif

			}
			
		}//endif
		else
		{
			if (lpOutSE->pDefender == NULL)
			{
				FillWarStateInfo(war_result_invader_win, nocity_vs_nocity_mode, ScriptParam);

				//首次攻城战
                GainCity(pWarInfo->mapID,lpOutSE->nMapWorldIndex,lpOutSE->pInvader);
			}//endif
			else
			{
				FillWarStateInfo(war_result_invader_win, nocity_vs_city_mode, ScriptParam);
					
				//夺城战争
                RobCity(pWarInfo->mapID,lpOutSE->nMapWorldIndex,lpOutSE->pInvader,lpOutSE->pDefender);
				DefenderTongWarMsgNotify(pWarInfo->mapID,lpOutSE->pDefender,lpOutSE->pInvader,MSG_WAR_DEFENDER_CITY_LOSE);
			}//end else

		}//end else

		//战争地图城市保护Buff....................................................................................................
		ConfigManager       & cm         = ConfigManager::Singleton();
		int          nProtecteBuffId     = cm.GetGlobalVariable(global_var_tong_war_protect_buff);
		BuffMgr             & buffman    = BuffMgr::Singleton();
		
		if (IsValidNpc(lpOutSE->nMapLordNpcIndex))
			buffman.AddNpcBuff(lpOutSE->nMapLordNpcIndex,lpOutSE->nMapLordNpcIndex,nProtecteBuffId);
	
        InvaderTongWarMsgNotify(pWarInfo->mapID,lpOutSE->pInvader,MSG_WAR_INVADER_WIN);

	}//endif
	else
	{
        if (lpOutSE->pInvader)
		{	
            InvaderTongWarMsgNotify(pWarInfo->mapID,lpOutSE->pInvader,MSG_WAR_INVADER_LOSS);
		}//endif

		if (lpOutSE->pDefender && lpOutSE->pInvader)
		{
            DefenderTongWarMsgNotify(pWarInfo->mapID,lpOutSE->pDefender,lpOutSE->pInvader,MSG_WAR_DEFENDER_WIN);
		}//endf

		if (nRobberCityMapID != INVALID_WORLD_ID)
		{
			if (IsValidNpc(lpOutSE->nMapRobberNpcIndex) && BuffMgr::Singleton().IsHaveBuff(lpOutSE->nMapRobberNpcIndex,nChangeCityBuffID))
			{
				if (bHasDefender)
					FillWarStateInfo(war_result_defender_win, city_vs_city_change_mode, ScriptParam);
				else
					FillWarStateInfo(war_result_defender_win, city_vs_nocity_change_mode, ScriptParam);
			}
			else
			{
				if (bHasDefender)
					FillWarStateInfo(war_result_defender_win, city_vs_city_rob_mode, ScriptParam);
				else
					FillWarStateInfo(war_result_defender_win, city_vs_nocity_rob_mode, ScriptParam);
			}
		}
		else	
		{
			if (bHasDefender)
				FillWarStateInfo(war_result_defender_win, nocity_vs_city_mode, ScriptParam);	
			else
				FillWarStateInfo(war_result_defender_win, nocity_vs_nocity_mode, ScriptParam);
		}

	}//end for else


	ExecuteScript("\\script\\main.lua", "PastTongWar", ScriptParam, g_SubWorldSet.SearchWorld(pWarInfo->mapID));

	NotifyClientClearMap(lpOutSE);

}

void KTongWarManager::ChangeCity(const int nOldCityID,const int nNewCityID,SocialUnit * pInvader,SocialUnit * pDefender)
{
	if (!pInvader)
		return;

	int nOldCityMapIndex = g_SubWorldSet.SearchWorld(nOldCityID);
	if (nOldCityMapIndex == INVALID_WORLD_INDEX)
		return ;

	int nNewCityMapIndex = g_SubWorldSet.SearchWorld(nNewCityID);
	if (nNewCityMapIndex == INVALID_WORLD_INDEX)
		return ;

	//Clear Old city infomation
	int nOldCitylordNpcIndex = SubWorld[nOldCityMapIndex].GetLord();
	_ASSERT(IsValidNpc(nOldCitylordNpcIndex));
	if (!IsValidNpc(nOldCitylordNpcIndex))
		return;

	FSGUID invalid;
	
	Npc[nOldCitylordNpcIndex].SetLord(invalid);
	Npc[nOldCitylordNpcIndex].Save();
	
	for (int nOldSubLord = 0; nOldSubLord< SUBLORD_COUNT ;nOldSubLord ++ )
	{
		int iSubLordIndex = SubWorld[nOldCityMapIndex].GetSubLord(nOldSubLord);
		if (IsValidNpc(iSubLordIndex))
		{
			Npc[iSubLordIndex].SetLord(invalid);
			Npc[iSubLordIndex].Save();
		}//endif
		
	}//end for nSublord
	
	StatueInfoMgr::Singleton().ForceClearStatue(nOldCityID);

	//Refresh New City Infomation
	int nNewCitylordNpcIndex = SubWorld[nNewCityMapIndex].GetLord();
	_ASSERT(IsValidNpc(nNewCitylordNpcIndex));
	if (!IsValidNpc(nNewCitylordNpcIndex))
		return;
	
	Npc[nNewCitylordNpcIndex].SetLord(pInvader->GetUnitGuid());
	Npc[nNewCitylordNpcIndex].Save();
	
	for (int nNewSubLord = 0; nNewSubLord< SUBLORD_COUNT ;nNewSubLord ++ )
	{
		int iSubLordIndex = SubWorld[nNewCityMapIndex].GetSubLord(nNewSubLord);
		if (IsValidNpc(iSubLordIndex))
		{
			Npc[iSubLordIndex].SetLord(pInvader->GetUnitGuid());
			Npc[iSubLordIndex].Save();
		}//endif
		
	}//end for nSublord

	StatueInfoMgr::Singleton().ForceClearStatue(nNewCityID);

	//Social Unit Attr
	SocialUnitAttr	&attr = pInvader->GetUnitAttr();
	attr.ChangeAttr(enSUAttr_CityMap,(const char *)&nNewCityID,sizeof(nNewCityID));

	if (pDefender)
	{
		SocialUnitAttr	&defenderAttr = pDefender->GetUnitAttr();
		defenderAttr.DelAttr(enSUAttr_CityMap);
	}//endif

	KEconomySysManager::Singleton().SetCaptureCityTime(nNewCityID, UNIX_TMIE_STAMP);

}

void KTongWarManager::RobRes(const int nLordNpcIndex,const int nRoberNpcIndex,const int nMapId,const char * szTongName)
{
	return ;

	//因为将掠夺放到了脚本中去执行，所以这里注释掉了
// 	if (!IsValidNpc(nLordNpcIndex) || !IsValidNpc(nRoberNpcIndex))
// 	{
// 		_ASSERT(false);
// 		return ;
// 	}//endif
// 
// 	ConfigManager       & cm         = ConfigManager::Singleton();
// 	int       iRobPercentage         = cm.GetGlobalVariable(global_var_tong_war_rob_percentage);
// 
// 	for (int res = nuai_lord_res0 ; res<= nuai_lord_res3 ; res++)
// 	{
// 		int nType     = res;
// 		int nValue    = Npc[nLordNpcIndex].m_UnaryAttrMgr[nType];
// 		int nRobValue = nValue * iRobPercentage / 100;
// 		
// 		if(nRobValue < 0)
// 			nRobValue = 0;
// 		
// 		if(nRobValue > nValue)
// 			nRobValue = nValue;
// 		
// 		Npc[nLordNpcIndex].m_UnaryAttrMgr.Set( nType, nValue - nRobValue );
// 		Npc[nLordNpcIndex].SetDataChangedFlag(true);
// 		
// 		nValue = Npc[nRoberNpcIndex].m_UnaryAttrMgr[nType];
// 		Npc[nRoberNpcIndex].m_UnaryAttrMgr.Set( nType, nValue + nRobValue );
// 		Npc[nRoberNpcIndex].SetDataChangedFlag(true);
// 
// 		if (szTongName)
// 			NotifyRobRes(szTongName, nMapId, res, nRobValue);
// 	}//end for res
// 
// 	Npc[nLordNpcIndex].Save();
// 	Npc[nRoberNpcIndex].Save();
}

void KTongWarManager::GainCity(int nCityMapId,int nCityWorldIndex,SocialUnit * pInvader)
{
    _ASSERT(pInvader && nCityWorldIndex!=INVALID_WORLD_INDEX);
	
	if(pInvader && nCityWorldIndex != INVALID_WORLD_INDEX)
	{
		SocialUnitAttr	&attr = pInvader->GetUnitAttr();
		attr.AddAttr(enSUAttr_CityMap, (const char*)&nCityMapId, sizeof(nCityMapId));
	//	SocialSerializer::Singleton().UpdateAttrReq(-1, pInvader);
        
		//如果宕在这里，战争记录也停在ProcessState,那么初始化的时候，在SocialUnitMgr::OnMapLordLoadReady 时，会
		//把这个属性去掉.重走正常流程。

		int nCitylordNpcIndex = SubWorld[nCityWorldIndex].GetLord();
		_ASSERT(IsValidNpc(nCitylordNpcIndex));
		if (!IsValidNpc(nCitylordNpcIndex))
			return;

		Npc[nCitylordNpcIndex].SetLord(pInvader->GetUnitGuid());
		Npc[nCitylordNpcIndex].Save();

		for (int nSubLord = 0; nSubLord< SUBLORD_COUNT ;nSubLord ++ )
		{
			int iSubLordIndex = SubWorld[nCityWorldIndex].GetSubLord(nSubLord);
			if (IsValidNpc(iSubLordIndex))
			{
			    Npc[iSubLordIndex].SetLord(pInvader->GetUnitGuid());
				Npc[iSubLordIndex].Save();
			}//endif
			
		}//end for nSublord


		StatueInfoMgr::Singleton().ForceClearStatue(nCityMapId);
		KEconomySysManager::Singleton().SetCaptureCityTime(nCityMapId, UNIX_TMIE_STAMP);
	}//endif
	
}

void KTongWarManager::RobCity(int nCityMapId,int nCityWorldIndex,SocialUnit * pInvader,SocialUnit * pDefender)
{
	if (nCityWorldIndex == INVALID_WORLD_INDEX )
	{
		_ASSERT(false);	
		return ;
	}//endif

	_ASSERT(pInvader && pDefender);
    if(pInvader && pDefender)
	{
		SocialUnitAttr	&invaderAttr = pInvader->GetUnitAttr();
		invaderAttr.AddAttr(enSUAttr_CityMap, (const char*)&nCityMapId, sizeof(nCityMapId));
//		SocialSerializer::Singleton().UpdateAttrReq(-1, pInvader);
		
		//如果宕在这里，战争记录也停在ProcessState,那么初始化的时候，在SocialUnitMgr::OnMapLordLoadReady 时，会
		//把这个属性去掉.重走正常流程。

		int nCitylordNpcIndex = SubWorld[nCityWorldIndex].GetLord();
		_ASSERT(IsValidNpc(nCitylordNpcIndex));
		if (!IsValidNpc(nCitylordNpcIndex))
			return;
		
		Npc[nCitylordNpcIndex].SetLord(pInvader->GetUnitGuid());
		Npc[nCitylordNpcIndex].Save();
		
		for (int nSubLord = 0; nSubLord< SUBLORD_COUNT ;nSubLord ++ )
		{
			int iSubLordIndex = SubWorld[nCityWorldIndex].GetSubLord(nSubLord);
			if (IsValidNpc(iSubLordIndex))
			{
				Npc[iSubLordIndex].SetLord(pInvader->GetUnitGuid());
				Npc[iSubLordIndex].Save();
			}//endif
			
		}//end for nSublord

		//如果宕在这里，defender的Attr会在SocialUnitMgr::OnMapLordLoadReady 时去掉，
		//重走正常流程时,会当作无效记录

		SocialUnitAttr	&defenderAttr = pDefender->GetUnitAttr();
		defenderAttr.DelAttr(enSUAttr_CityMap);
		StatueInfoMgr::Singleton().ForceClearStatue(nCityMapId);
		KEconomySysManager::Singleton().SetCaptureCityTime(nCityMapId, UNIX_TMIE_STAMP);
//		SocialSerializer::Singleton().UpdateAttrReq(-1, pDefender);
	}//endif
	
}

void KTongWarManager::ChangeFromProcessToEnd(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	_ASSERT(pWarInfo && pWarInfo->warState == FS_WAR_STATE_PROCESS);	
	JurgeWinFailed(pWarInfo,lpOutSE);
}

void KTongWarManager::ChangeFromNotifyToEnd(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE)
{ 
    _ASSERT(pWarInfo && pWarInfo->warState == FS_WAR_STATE_NOTIFY);	
	JurgeWinFailed(pWarInfo,lpOutSE);
	
	if (lpOutSE->pInvader)
		CheckRemoveProtectBuff(lpOutSE->pInvader);
}

bool KTongWarManager::TestFromEndToInvalid(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	return true;
}

void KTongWarManager::ChangeFromEndToInvalid(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	_ASSERT(pWarInfo && pWarInfo->warState == FS_WAR_STATE_END && lpOutSE->nMapLordNpcIndex!=INVALID_WORLD_INDEX);

	ClearMapWarState(pWarInfo);

}

bool KTongWarManager::EnviromentCheckInNotify(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	_ASSERT(pWarInfo && lpOutSE);
	
	bool                      bValid     = true;
	//1.Check SocialUnit...
	ServerSocialUnitMgr     & gSocialMgr = ServerSocialUnitMgr::Singleton();
	
	lpOutSE->pInvader       = gSocialMgr.GetUnit(pWarInfo->invaderGUID);
    lpOutSE->pDefender      = gSocialMgr.GetUnit(pWarInfo->defenderGUID);
	
	//2.Check WorldId...
	lpOutSE->nMapWorldIndex = g_SubWorldSet.SearchWorld(pWarInfo->mapID);
	
	if (lpOutSE->nMapWorldIndex == INVALID_WORLD_INDEX)
	{
		//Exception!
		_ASSERT(false);
		pWarInfo->warState =  FS_WAR_STATE_INVALID;
		bValid             =  false;
	}//endif
    else
	{
		//3.Check Lord......
		lpOutSE->nMapLordNpcIndex = SubWorld[lpOutSE->nMapWorldIndex].GetLord();
		
		if (!IsValidNpc(lpOutSE->nMapLordNpcIndex))
		{
			//Exception!
			_ASSERT(false);
			pWarInfo->warState =  FS_WAR_STATE_INVALID;
			bValid             =  false;
		}//endif
		else
		{
			if (lpOutSE->pDefender && pWarInfo->defenderGUID.data[0]!=0)  //诸侯存在才测试这个,因为诸侯可能解散
			{
				if (Npc[lpOutSE->nMapLordNpcIndex].GetLord()!=pWarInfo->defenderGUID)
				{
					//Exception!
					_ASSERT(false);
					pWarInfo->warState =  FS_WAR_STATE_INVALID;
					bValid             =  false;
				}//endif
				
			}//endif

		}//endif
	
		//4.Check Robber...
		lpOutSE->nMapRobberNpcIndex = SubWorld[lpOutSE->nMapWorldIndex].GetRobber();
		
		if (!IsValidNpc(lpOutSE->nMapRobberNpcIndex))
		{
			//可能没有存盘,这里会修复
            if (!CheckAddWorldRobber(lpOutSE->nMapWorldIndex,pWarInfo->invaderGUID))
			{
				//Exception!
				_ASSERT(false);
				pWarInfo->warState =  FS_WAR_STATE_INVALID;
				bValid             =  false;
			}//endif
			else
			{
                lpOutSE->nMapRobberNpcIndex = SubWorld[lpOutSE->nMapWorldIndex].GetRobber();
				_ASSERT(IsValidNpc(lpOutSE->nMapRobberNpcIndex));	
				
				
				//Debug for robber
				if (IsValidNpc(lpOutSE->nMapRobberNpcIndex))
				{					
					char infoStr[512];
					sprintf(infoStr, "Add Robber nodifycheck:MapId:%d,NpcIdx:%d,RegionIdx:%d", SubWorld[lpOutSE->nMapWorldIndex].m_SubWorldID, lpOutSE->nMapRobberNpcIndex, Npc[lpOutSE->nMapRobberNpcIndex].m_RegionIndex);
					infoStr[511] = 0;
					GetGlobalTongWarMgr().DumpRobberInfo(infoStr, strlen(infoStr) + 1);
				}
			}
	        
		}//endif
		else
		{
			if (Npc[lpOutSE->nMapRobberNpcIndex].GetLord()!=pWarInfo->invaderGUID) //注意：Robber Npc 与对应的invaderGUID是一起存盘的
			{
				//Debug for robber
				if (IsValidNpc(lpOutSE->nMapRobberNpcIndex))
				{					
					char infoStr[512];
					sprintf(infoStr, "Remove Robber nodifycheck:MapId:%d,NpcIdx:%d,RegionIdx:%d", SubWorld[lpOutSE->nMapWorldIndex].m_SubWorldID, lpOutSE->nMapRobberNpcIndex, Npc[lpOutSE->nMapRobberNpcIndex].m_RegionIndex);
					infoStr[511] = 0;
					GetGlobalTongWarMgr().DumpRobberInfo(infoStr, strlen(infoStr) + 1);
				}


				CheckRemoveRobber(lpOutSE->nMapWorldIndex,lpOutSE->nMapRobberNpcIndex);

				//Exception!
				_ASSERT(false);
				pWarInfo->warState =  FS_WAR_STATE_INVALID;
				bValid             =  false;
			}//endif

		}//end else
	

 		//5.Check proteced buff
		if (bValid && lpOutSE->pInvader)
 		{
            CheckAddProtectBuff(lpOutSE->pInvader);
 		}//endif 

	}//endif

	if (pWarInfo->warState==FS_WAR_STATE_INVALID)
		DumpInvalidWarInfo(pWarInfo);

	return bValid;
}

bool KTongWarManager::EnviromentCheckInProcess(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE)
{
	_ASSERT(pWarInfo && lpOutSE);

	bool                      bValid     = true;
	//1.Check SocialUnit...
	ServerSocialUnitMgr     & gSocialMgr = ServerSocialUnitMgr::Singleton();
	
	lpOutSE->pInvader       = gSocialMgr.GetUnit(pWarInfo->invaderGUID);
    lpOutSE->pDefender      = gSocialMgr.GetUnit(pWarInfo->defenderGUID);
	
	lpOutSE->nMapWorldIndex = g_SubWorldSet.SearchWorld(pWarInfo->mapID);
	
	//2.Check World id ...
	if (lpOutSE->nMapWorldIndex == INVALID_WORLD_INDEX)
	{
		//Exception!
		_ASSERT(false);
		pWarInfo->warState =  FS_WAR_STATE_INVALID;
		bValid             =  false;
	}//endif
    else
	{
		lpOutSE->nMapLordNpcIndex = SubWorld[lpOutSE->nMapWorldIndex].GetLord();
	
		if (!IsValidNpc(lpOutSE->nMapLordNpcIndex))
		{
			//Exception!
			_ASSERT(false);
			pWarInfo->warState = FS_WAR_STATE_INVALID;
			bValid             =  false;
		}//endif
		else
		{
			if (lpOutSE->pDefender && pWarInfo->defenderGUID.data[0]!=0)  //诸侯存在才测试这个,因为诸侯可能解散
			{
				if (Npc[lpOutSE->nMapLordNpcIndex].GetLord()!=pWarInfo->defenderGUID)
				{
					//Exception!
					_ASSERT(false);
					pWarInfo->warState =  FS_WAR_STATE_INVALID;
					bValid             =  false;
				}//endif
				
			}//endif

			
		}//endif
	
		lpOutSE->nMapRobberNpcIndex = SubWorld[lpOutSE->nMapWorldIndex].GetRobber();
		
		if (!IsValidNpc(lpOutSE->nMapRobberNpcIndex))
		{
			//可能没有存盘,这里会修复
            if (!CheckAddWorldRobber(lpOutSE->nMapWorldIndex,pWarInfo->invaderGUID))
			{
				//Exception!
				_ASSERT(false);
				pWarInfo->warState =  FS_WAR_STATE_INVALID;
				bValid             =  false;
			}//endif
			else
			{
				lpOutSE->nMapRobberNpcIndex = SubWorld[lpOutSE->nMapWorldIndex].GetRobber();
				_ASSERT(IsValidNpc(lpOutSE->nMapRobberNpcIndex));
				Npc[lpOutSE->nMapRobberNpcIndex].SetRobber(pWarInfo->defenderGUID);

				//Debug for robber
				if (IsValidNpc(lpOutSE->nMapRobberNpcIndex))
				{					
					char infoStr[512];
					sprintf(infoStr, "Add Robber processcheck:MapId:%d,NpcIdx:%d,RegionIdx:%d", SubWorld[lpOutSE->nMapWorldIndex].m_SubWorldID, lpOutSE->nMapRobberNpcIndex, Npc[lpOutSE->nMapRobberNpcIndex].m_RegionIndex);
					infoStr[511] = 0;
					GetGlobalTongWarMgr().DumpRobberInfo(infoStr, strlen(infoStr) + 1);
				}

			}//end else

		}//endif
		else
		{
			if (Npc[lpOutSE->nMapRobberNpcIndex].GetLord()!=pWarInfo->invaderGUID)
			{
				//Exception!
				_ASSERT(false);
				pWarInfo->warState = FS_WAR_STATE_INVALID;
				bValid             =  false;

				//Debug for robber
				if (IsValidNpc(lpOutSE->nMapRobberNpcIndex))
				{					
					char infoStr[512];
					sprintf(infoStr, "Remove Robber invild invader process:MapId:%d,NpcIdx:%d,RegionIdx:%d", SubWorld[lpOutSE->nMapWorldIndex].m_SubWorldID, lpOutSE->nMapRobberNpcIndex, Npc[lpOutSE->nMapRobberNpcIndex].m_RegionIndex);
					infoStr[511] = 0;
					GetGlobalTongWarMgr().DumpRobberInfo(infoStr, strlen(infoStr) + 1);
				}

				CheckRemoveRobber(lpOutSE->nMapWorldIndex,lpOutSE->nMapRobberNpcIndex);

			}//endif
			else
			{
				if (Npc[lpOutSE->nMapRobberNpcIndex].GetRobber() != pWarInfo->defenderGUID )
				{
					//修复
					Npc[lpOutSE->nMapRobberNpcIndex].SetRobber(pWarInfo->defenderGUID);
					Npc[lpOutSE->nMapRobberNpcIndex].Save();
				}//endif

			}//end else

		}//end else

	}//endif

	//Check City Attr
	if (bValid && lpOutSE->pInvader)
	{
	    int iInvaderMap = GetCityMapId(lpOutSE->pInvader->GetUnitAttr());
		if (iInvaderMap!=INVALID_WORLD_ID && iInvaderMap == pWarInfo->mapID)
		{
            //Exception!
			_ASSERT(false);
			pWarInfo->warState = FS_WAR_STATE_INVALID;
			bValid             =  false;

			//Debug for robber
			if (IsValidNpc(lpOutSE->nMapRobberNpcIndex))
			{					
				char infoStr[512];
				sprintf(infoStr, "Remove Robber targetself process:MapId:%d,NpcIdx:%d,RegionIdx:%d", SubWorld[lpOutSE->nMapWorldIndex].m_SubWorldID, lpOutSE->nMapRobberNpcIndex, Npc[lpOutSE->nMapRobberNpcIndex].m_RegionIndex);
				infoStr[511] = 0;
				GetGlobalTongWarMgr().DumpRobberInfo(infoStr, strlen(infoStr) + 1);
			}


			CheckRemoveRobber(lpOutSE->nMapWorldIndex,lpOutSE->nMapRobberNpcIndex);
		}//endif

	}//endif

 	if (bValid && lpOutSE->pInvader)
 		CheckRemoveProtectBuff(lpOutSE->pInvader);

	if (pWarInfo->warState==FS_WAR_STATE_INVALID)
		DumpInvalidWarInfo(pWarInfo);
	
	return bValid;
}

bool KTongWarManager::EnviromentCheckInEnd(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE)
{
    _ASSERT(pWarInfo && lpOutSE);
	
	bool                      bValid     = true;
	
	ServerSocialUnitMgr     & gSocialMgr = ServerSocialUnitMgr::Singleton();
	
	lpOutSE->pInvader       = gSocialMgr.GetUnit(pWarInfo->invaderGUID);
    lpOutSE->pDefender      = gSocialMgr.GetUnit(pWarInfo->defenderGUID);
	
	lpOutSE->nMapWorldIndex = g_SubWorldSet.SearchWorld(pWarInfo->mapID);
	
	if (lpOutSE->nMapWorldIndex == INVALID_WORLD_INDEX)
	{
		//Exception!
		_ASSERT(false);
		pWarInfo->warState =  FS_WAR_STATE_INVALID;
		bValid             =  false;
	}//endif
	else
	{
		lpOutSE->nMapLordNpcIndex = SubWorld[lpOutSE->nMapWorldIndex].GetLord();
		
		if (!IsValidNpc(lpOutSE->nMapLordNpcIndex))
		{
			//Exception!
			_ASSERT(false);
			pWarInfo->warState = FS_WAR_STATE_INVALID;
			bValid             =  false;
		}//endif

	}//end else

	if (pWarInfo->warState==FS_WAR_STATE_INVALID)
		DumpInvalidWarInfo(pWarInfo);

	return bValid;
}

bool KTongWarManager::EnviromentCheckInInvalid(FSWarInfo * pWarInfo,StateEnviroment * lpOutSE)
{
    _ASSERT(pWarInfo && lpOutSE);

	int iSubWordIndex = g_SubWorldSet.SearchWorld(pWarInfo->mapID);
	if (iSubWordIndex == INVALID_WORLD_INDEX)
	{
		return false;
	}//endif
	
	int iRoberNpcIndex = SubWorld[iSubWordIndex].GetRobber();
	if (IsValidNpc(iRoberNpcIndex) && (Npc[iRoberNpcIndex].GetLord()==pWarInfo->invaderGUID || Npc[iRoberNpcIndex].GetLord().data[0]==0))
	{
		//Debug for robber
		if (IsValidNpc(iRoberNpcIndex))
		{					
			char infoStr[512];
			sprintf(infoStr, "Remove Robber Invalid:MapId:%d,NpcIdx:%d,RegionIdx:%d", pWarInfo->mapID, iRoberNpcIndex, Npc[iRoberNpcIndex].m_RegionIndex);
			infoStr[511] = 0;
			GetGlobalTongWarMgr().DumpRobberInfo(infoStr, strlen(infoStr) + 1);
		}

        CheckRemoveRobber(iSubWordIndex,iRoberNpcIndex);
	}//endif

    return true;
}

void KTongWarManager::ClearMapWarState(FSWarInfo * pWarInfo)
{
	_ASSERT(pWarInfo);

	//恢复城市  ..................................................................
    //去掉整个地图的Rober ........................................................
	int iSubWordIndex = g_SubWorldSet.SearchWorld(pWarInfo->mapID);
	if (iSubWordIndex == INVALID_WORLD_INDEX)
	{
		return ;
	}//endif
	
	int iRoberNpcIndex = SubWorld[iSubWordIndex].GetRobber();
	if (IsValidNpc(iRoberNpcIndex))
	{
		int nRegion   = Npc[iRoberNpcIndex].m_RegionIndex;
		DWORD nNpcId  = Npc[iRoberNpcIndex].GetId();

		Npc[iRoberNpcIndex].m_UnaryAttrMgr.Set(nuai_curlife, 0);
		Npc[iRoberNpcIndex].SendCommand(do_death,0,0,0);
		Npc[iRoberNpcIndex].GetController().SetActive(false);
		Npc[iRoberNpcIndex].SetProcessAI(TRUE);
		Npc[iRoberNpcIndex].ProcCommand(TRUE);


		BuffMgr &mgr = BuffMgr::Singleton();
		mgr.ClearAllBuff( iRoberNpcIndex, TRUE );
		
		//Debug for robber
		if (IsValidNpc(iRoberNpcIndex))
		{					
			char infoStr[512];
			sprintf(infoStr, "delete Robber clearwarstate:MapId:%d,NpcIdx:%d,RegionIdx:%d", pWarInfo->mapID, iRoberNpcIndex, Npc[iRoberNpcIndex].m_RegionIndex);
			infoStr[511] = 0;
			GetGlobalTongWarMgr().DumpRobberInfo(infoStr, strlen(infoStr) + 1);
		}

		SubWorld[iSubWordIndex].m_Region[nRegion].RemoveNpc(iRoberNpcIndex);

		int nRef = Npc[iRoberNpcIndex].m_Node.m_Ref; //--DebugAdderd

	    NpcSet.Remove(iRoberNpcIndex);   

		//Debug for robber
		char infoStr[512];
		sprintf(infoStr, "After delete robber,npcRef:%d,suc:%d", nRef, NpcSet.SearchID(nNpcId) > 0);
		infoStr[511] = 0;
		GetGlobalTongWarMgr().DumpRobberInfo(infoStr, strlen(infoStr) + 1);

	}//endif

	SubWorld[iSubWordIndex].SetRobber(-1);

	//去掉地图lord的Rober..........................................................
	int iLordNpcIndex = SubWorld[iSubWordIndex].GetLord();
	if (IsValidNpc(iLordNpcIndex))
	{
		FSGUID    guid;
		Npc[iLordNpcIndex].SetCurrentLifePercentage(100);
	    Npc[iLordNpcIndex].SetRobber(guid);
		Npc[iLordNpcIndex].Save();

		for (int SubLordIter = 0; SubLordIter < SUBLORD_COUNT; ++SubLordIter)
		{
			int nSubLordIdx = SubWorld[iSubWordIndex].GetSubLord(SubLordIter);

			if (IsValidNpc(nSubLordIdx))
			{
				Npc[nSubLordIdx].SetRobber(guid);
				Npc[nSubLordIdx].Save();
			}
		}
	}//endif
	
}

#define MAX_INVALID_INFO_LEN 1024

void KTongWarManager::DumpInvalidWarInfo(FSWarInfo * pWarInfo)
{
    _ASSERT(pWarInfo);   
    _ASSERT(false);
	
	if (pWarInfo)
	{
		char szLogString[MAX_INVALID_INFO_LEN];
		char szInvader[34];
		char szDefender[34];
		
		if (pWarInfo->invaderGUID.data[0]!=0)
			memcpy(szInvader,pWarInfo->invaderGUID.data,33);
		else
			sprintf(szInvader,"NoSocialUnit");
		
		szInvader[33]  =0;

		if (pWarInfo->defenderGUID.data[0]!=0)
			memcpy(szDefender,pWarInfo->defenderGUID.data,33);
		else
			sprintf(szDefender,"NoSocialUnit");
		
		szDefender[33] = 0;
		szLogString[0] = 0;

		snprintf(szLogString,sizeof(szLogString),"Invalid WarInfo clear:mapID %d,InvaderGUID: %s,defenderGUID:%s ",pWarInfo->mapID,szInvader,szDefender);
        szLogString[MAX_INVALID_INFO_LEN -1] = 0;
		
		int nSize      =strlen(szLogString);
		g_pLogSystem->SysDbgLog(szLogString,nSize,sys_dbg_log_event_tong_war);
	}//endif

}

void KTongWarManager::DumpTongWarStateChange(FSWarInfo * pWarInfo,int oldstate,int newstate)
{
    _ASSERT(pWarInfo);   
	
	if (pWarInfo)
	{
		char szLogString[MAX_INVALID_INFO_LEN];
		char szInvader[34];
		char szDefender[34];
		
		if (pWarInfo->invaderGUID.data[0]!=0)
			memcpy(szInvader,pWarInfo->invaderGUID.data,33);
		else
			sprintf(szInvader,"NoSocialUnit");
		
		szInvader[33]  =0;
		
		if (pWarInfo->defenderGUID.data[0]!=0)
			memcpy(szDefender,pWarInfo->defenderGUID.data,33);
		else
			sprintf(szDefender,"NoSocialUnit");
		
		szDefender[33] = 0;
		szLogString[0] = 0;
		
		snprintf(szLogString,sizeof(szLogString),"WarStateChange:mapID %d,InvaderGUID: %s,defenderGUID:%s ChangeFrom %d to %d."
			,pWarInfo->mapID,szInvader,szDefender,oldstate,newstate);

        szLogString[MAX_INVALID_INFO_LEN -1] = 0;
		
		int nSize      =strlen(szLogString);
		g_pLogSystem->SysDbgLog(szLogString,nSize,sys_dbg_log_event_tong_war);

	}//endif

}

void KTongWarManager::DumpInvalidMapLord(int nMapID,const FSGUID & guid)
{
	_ASSERT(false);   
	//Log to DB
	char szLogString[MAX_INVALID_INFO_LEN];
	char szUnitGUID[34];
	
	memcpy(szUnitGUID,guid.data,33);
	szUnitGUID[33]=0;
	
	snprintf(szLogString,sizeof(szLogString),"Invalid lord,GUID:%s MapId:%d",szUnitGUID,nMapID);
	szLogString[MAX_INVALID_INFO_LEN -1] = 0;
	
	int nSize      =strlen(szLogString);
	
	g_pLogSystem->SysDbgLog(szLogString,nSize,sys_dbg_log_event_tong_war);
}
 
void KTongWarManager::DumpWarMapComplete()
{
	char szLogString[MAX_INVALID_INFO_LEN];
	snprintf(szLogString,sizeof(szLogString),"Tong War Map Init Ready!");
	szLogString[MAX_INVALID_INFO_LEN -1] = 0;
	int nSize      =strlen(szLogString);
	g_pLogSystem->SysDbgLog(szLogString,nSize,sys_dbg_log_event_tong_war);
}

void KTongWarManager::CheckAddProtectBuff(SocialUnit * pInvader)
{
	if (pInvader == NULL)
		return;

	_ASSERT(pInvader);
    //宣战方保护，如果Buff存储失败，会在这里加上
	SocialUnitAttr      & invaderAttr= pInvader->GetUnitAttr();
	if (invaderAttr.IsAttrHasData(enSUAttr_CityMap))
	{
		char* pData = NULL;
		int nSize = invaderAttr.GetAttr( enSUAttr_CityMap, pData );
		if (nSize == sizeof(int))
		{
			int invaderMapId    = *((int*)pData);
			int invaderMapIndex = g_SubWorldSet.SearchWorld(invaderMapId); 
			if (invaderMapIndex!=INVALID_WORLD_INDEX)
			{
				int nInvaderMapLord              = SubWorld[invaderMapIndex].GetLord();
				if (IsValidNpc(nInvaderMapLord))
				{
					ConfigManager       & cm         = ConfigManager::Singleton();
					int          nProtecteBuffId     = cm.GetGlobalVariable(global_var_tong_war_protect_buff);
					BuffMgr             & buffman    = BuffMgr::Singleton();
					
					if (!buffman.IsHaveBuff(nInvaderMapLord,nProtecteBuffId))
						buffman.AddNpcBuff(nInvaderMapLord,nInvaderMapLord,nProtecteBuffId);
				}//endif
				
			}//endif
			
		}//endif

	}//endif
	
}

#define ROBBER_OFFSET_X 80
#define ROBBER_OFFSET_Y 80

bool KTongWarManager::CheckAddWorldRobber(int iSubWorldIndex,FSGUID & pInvaderGUID)
{
	_ASSERT(iSubWorldIndex!=-1 && pInvaderGUID.data!=0);
	if (iSubWorldIndex!=INVALID_WORLD_INDEX)
	{
		if (!IsValidNpc(SubWorld[iSubWorldIndex].GetRobber()))
		{
			int               iLordNpc  = SubWorld[iSubWorldIndex].GetLord();

			if (IsValidNpc(iLordNpc))
			{
			   ConfigManager    & cm       = ConfigManager::Singleton();
			   int nRobberTempID           = cm.GetGlobalVariable(global_var_tong_war_robber_tempID);
			   int nX,nY;
			   Npc[iLordNpc].GetMpsPos( &nX, &nY );
			
			   int	nNpcIdxInfo = MAKELONG(1, nRobberTempID);
			
			   int nNpcIdx = NpcSet.Add(
					nNpcIdxInfo, 
					iSubWorldIndex, 
					nX + ROBBER_OFFSET_X, 
					nY + ROBBER_OFFSET_Y);
				
				if( nNpcIdx > 0 )
				{
					Npc[nNpcIdx].NormalSync( );
					
					int nMode = Npc[nNpcIdx].m_UnaryAttrMgr[nuai_deathmode];
					nMode |= npc_deathmode_autodel;
					Npc[nNpcIdx].m_UnaryAttrMgr.Set( nuai_deathmode, nMode );
					
					const KNpcTemplate *pTemplate = Npc[nNpcIdx].GetTemplate();
					Npc[nNpcIdx].SetLord(pInvaderGUID);
					SubWorld[iSubWorldIndex].SetRobber(nNpcIdx);

					if( NULL != pTemplate && pTemplate->NeedSave() )
						Npc[nNpcIdx].Save();

					return true;
				}//endif

			}//endif

		}//endif

	}//endif

	return false;
}

void KTongWarManager::CheckRemoveProtectBuff(SocialUnit * pInvader)
{
    _ASSERT(pInvader);
    //
	SocialUnitAttr      & invaderAttr= pInvader->GetUnitAttr();
	if (invaderAttr.IsAttrHasData(enSUAttr_CityMap))
	{
		char* pData = NULL;
		int nSize = invaderAttr.GetAttr( enSUAttr_CityMap, pData );
		if (nSize == sizeof(int))
		{
			int invaderMapId    = *((int*)pData);
			int invaderMapIndex = g_SubWorldSet.SearchWorld(invaderMapId); 
			if (invaderMapIndex!=INVALID_WORLD_INDEX)
			{
				int nInvaderMapLord              = SubWorld[invaderMapIndex].GetLord();
				if (IsValidNpc(nInvaderMapLord))
				{
					ConfigManager       & cm         = ConfigManager::Singleton();
					int          nProtecteBuffId     = cm.GetGlobalVariable(global_var_tong_war_protect_buff);
					BuffMgr             & buffman    = BuffMgr::Singleton();
					
					if(buffman.IsHaveBuff(nInvaderMapLord,nProtecteBuffId))
						buffman.ClearBuffByTempID(nInvaderMapLord,nProtecteBuffId);
				}//endif
				
			}//endif
			
		}//endif
		
	}//endif

}

void KTongWarManager::CheckRemoveRobber(int iSubWorldIndex,int iRoberNpcIndex)
{
	_ASSERT(IsValidNpc(iRoberNpcIndex) && iSubWorldIndex!= INVALID_WORLD_INDEX);

	if (!IsValidNpc(iRoberNpcIndex) || iSubWorldIndex == INVALID_WORLD_INDEX)
		return;

	//No other record use the robber
    if (!GetGlobalWarInfoManager().GetRecord(Npc[iRoberNpcIndex].GetLord(),SubWorld[iSubWorldIndex].m_SubWorldID))
	{
		int iLord = SubWorld[iSubWorldIndex].GetLord();
		if (IsValidNpc(iLord) && Npc[iLord].GetRobber() == Npc[iRoberNpcIndex].GetLord())
		{
			FSGUID guid;
			Npc[iLord].SetRobber(guid);
		}//endif

		int nRegion        = Npc[iRoberNpcIndex].m_RegionIndex;
		
        Npc[iRoberNpcIndex].m_UnaryAttrMgr.Set(nuai_curlife, 0);
		Npc[iRoberNpcIndex].SendCommand(do_death,0,0,0);
		Npc[iRoberNpcIndex].GetController().SetActive(false);
		Npc[iRoberNpcIndex].SetProcessAI(TRUE);
		Npc[iRoberNpcIndex].ProcCommand(TRUE);

		SubWorld[iSubWorldIndex].SetRobber(-1);

		BuffMgr &mgr = BuffMgr::Singleton();
		mgr.ClearAllBuff( iRoberNpcIndex, TRUE );
		
		SubWorld[iSubWorldIndex].m_Region[nRegion].RemoveNpc(iRoberNpcIndex);
	    NpcSet.Remove(iRoberNpcIndex);

	}//endif
	
}

void KTongWarManager::CheckMapLordAndSocialUnit(int nMapID,int iSubwordIndex)
{
	ServerSocialUnitMgr& gSocial     = ServerSocialUnitMgr::Singleton();
	int                  iLordNIndex = SubWorld[iSubwordIndex].GetLord();

	_ASSERT(IsValidNpc(iLordNIndex));
	if (!IsValidNpc(iLordNIndex))
		return ;

	const FSGUID &       lordGUID    = Npc[iLordNIndex].GetLord();

	if (lordGUID.data[0]!=0)
	{ 
		bool         bReset    = false;
		SocialUnit * pLordUnit = gSocial.GetUnit(lordGUID);
		
		if (pLordUnit)
		{
			SocialUnitAttr  & attr    = pLordUnit->GetUnitAttr();

			if (GetCityMapId(attr) == INVALID_WORLD_ID)
			{	
				attr.AddAttr(enSUAttr_CityMap, (const char*)&nMapID, sizeof(nMapID));
				
			}//endif
			else
			{
				_ASSERT(false);
				//One SocialUnit Get two city ! database error
				//唯一的可能是有换城逻辑后，一个城市攻打另一个城市成功后，服务器宕在两个存储过程中间
				bReset = true;
			}//endelse
			
		}//endif
		else
		{
			bReset = true;
		}//end else

		if (bReset)
		{
			_ASSERT(false);
			//可能诸侯在占领城后宕机，造成数据存储不对.
			DumpInvalidMapLord(nMapID,lordGUID);
			
			FSGUID           invalid;
			Npc[iLordNIndex].SetLord(invalid);
			Npc[iLordNIndex].Save();
			
			for (int i=0;i<SUBLORD_COUNT;i++)
			{
				int iSubLordNpc = SubWorld[iSubwordIndex].GetSubLord(i);
				
				if (IsValidNpc(iSubLordNpc))
				{
					Npc[iSubLordNpc].SetLord(invalid);
					Npc[iSubLordNpc].Save();
				}//endif
				
			}//end for i
		}//enmdif
		
	}//endif
}

KTongWarManager & GetGlobalTongWarMgr(void)
{
	static KTongWarManager g_TongWarMgr;
	return g_TongWarMgr;
}

void KTongWarManager::NotifyClientClearMap(StateEnviroment* lpOutSE)
{
	if (lpOutSE -> nMapWorldIndex != INVALID_WORLD_INDEX)
		SubWorld[lpOutSE->nMapWorldIndex].ClearWarMap();
}

void KTongWarManager::FillMapInfo(int nRobberMapId, int nDefenerMapId, int& ScriptParam)
{
	if (nRobberMapId == INVALID_WORLD_ID)
		nRobberMapId = 0;

	if (nDefenerMapId == INVALID_WORLD_ID)
		return;
	
	ScriptParam |= (nRobberMapId<<24 & 0xFF000000);
	ScriptParam |= (nDefenerMapId<<16 & 0x00FF0000);
}

void KTongWarManager::FillWarStateInfo(int nWarResult, int nWarMode, int& ScriptParam)
{
	ScriptParam |= (nWarResult<<8 & 0x0000FF00);
	ScriptParam |= (nWarMode & 0x000000FF);
}

void KTongWarManager::DumpRobberInfo(const char* pStr, int nStrSize)
{
	if (!pStr)
		return ;
	
	char RobberInfoStr[MAX_INVALID_INFO_LEN];

	int nStrLen = nStrSize > MAX_INVALID_INFO_LEN ? MAX_INVALID_INFO_LEN : nStrSize;
	memcpy(RobberInfoStr, pStr, nStrLen);
	RobberInfoStr[sizeof(RobberInfoStr) - 1] = 0;
	int nLen = strlen(RobberInfoStr);

	g_pLogSystem->SysDbgLog(RobberInfoStr, nLen, sys_dbg_log_event_tong_war);

}
