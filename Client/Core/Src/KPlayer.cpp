#include "KCore.h"

#ifndef WM_MOUSEHOVER
#define WM_MOUSEHOVER 0x02A1
#endif

#include "KEngine.h"

#ifdef _SERVER
#include "IBCenter_S.h"
#include "KDebug.h"
#else
#include "screeneffect_man.h"
#include "networkinterface.h"
#endif
#include "OnceIBItemMgr.h"
#include "CoreRelated.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KPlayer.h"
#include "LuaFuns.h"
#include "KSortScript.h"
#include "KObjSet.h"
#include "KPlayerSet.h"
#include "KSubWorldSet.h"
#include "GameDataDef.h"
#include "KBuySell.h"
#include "MsgGenreDef.h"
#include "KItemSet.h"
#include "Text.h"
#include <time.h>
#include "KMath.h"

#ifndef _SERVER
#include "CoreShell.h"
#endif
#include "specialskill_tab.h"

#include "GameDataDef.h"
#include "KNpcTemplate.h"

#ifdef _SERVER
unsigned int KPlayer::ms_uCurSystemTime = 0;
#include "BannerMgr.h"
#include "IBLog.h"
#endif

#include "cfs_common_def.h"

#ifdef _SERVER
#include "cfs_fs2_savedef.h"
#include "cfs_db_interface.h"
#include "ChatCenter_S.h"
#include "AccountLoginDef.h"
#endif
#include "ChatDataDef.h"
#include "KItemGenerator.h"
#include "Scene/ObstacleDef.h"
#include "ScriptFuns.h"

#ifdef _SERVER
extern unsigned int g_uShareExpDistance[];
#include "ConfigManager.h"
#endif

#include "buff_tab.h"
#include "buff_man.h"
#include "Abrade_Table.h"
#include "MagicAttribute.h"

#ifdef _SERVER
#include "ServerSocialUnitMgr.h"
#include "fseye_protocol.h"
#endif

#ifdef _SERVER
#include "SocialSerializer.h"
#include "exp_manager.h"
#include "player_monitor.h"
#include "KSkills.h"
#endif

#ifndef _SERVER
#ifdef _AUTO_ROBOT
#include "AutoRobotMgr.h"
#endif
#endif

//Add by Brianyao 2007
#ifndef _SERVER
#include "TaisuiWheelTianXiang.h"
#endif

#include "KTaisuiWheel.h"
//end

#ifndef _SERVER
#include "ai_player_controller.h"
#include "KOption.h"
#endif

#define		defPLAYER_LOGIN_TIMEOUT			GAME_FPS * 20// 20 sec
#define		defPLAYER_SAVE_TIMEOUT			30 * GAME_FPS
#define		PLAYER_LEVEL_1_EXP				48
#define		PLAYER_LEVEL_ADD_ATTRIBUTE		5
#define		PLAYER_LEVEL_ADD_SKILL			1
#define		PLAYER_TEAM_EXP_ADD				50
#define		MAX_APPLY_TEAM_TIME				500
#define		BASE_WALK_SPEED					5
#define		BASE_RUN_SPEED					10
#define		BASE_FIRE_RESIST_MAX			75
#define		BASE_COLD_RESIST_MAX			75
#define		BASE_POISON_RESIST_MAX			75
#define		BASE_LIGHT_RESIST_MAX			75
#define		BASE_PHYSICS_RESIST_MAX			75
#define		BASE_EARTH_RESIST_MAX			75
#define		BASE_ATTACK_SPEED				0
#define		BASE_CAST_SPEED					0
#define		BASE_VISION_RADIUS				120
#define		BASE_HIT_RECOVER				6
#define		TOWN_PORTAL_TIME				1800
#define		MAX_LOG_MONEY_NUM	100000
#define		BASERETRYTIME_LOAD_LITTLETREE	GAME_FPS * 60	// 60 sec	
#define		EXTRETRYTIME_LOAD_LITTLETREE	GAME_FPS * 60
#define		COMBAT_TOP10_SYNC_MAX_INTERVAL  60
#define		COMBAT_TOP10_SYNC_MIN_INTERVAL	16
KPlayer		*Player;

int		g_nLastNetMsgLoop;

#ifdef _SERVER
QUESTION_VECTOR		g_PicQuestion;
QUESTION_VECTOR		g_NumQuestion;
PQ_CONFIG			g_PQConfig	=	{0};
LEVELKILLNPCVECTOR	g_KillNpcConfig;
#endif

#ifdef _SERVER
const DWORD KPlayer::m_DBLoadFinishedFlag = enPDBMask_BaseInfo | 
	enPDBMask_Friend | 
	enPDBMask_Skill | 
	enPDBMask_Item | 
	enPDBMask_Buff | 
	enPDBMask_Task |
	enPDBMask_Reserve
	;
#endif

#ifdef _SERVER

KPlayerUIState::KPlayerUIState()
{
	Init();
}

KPlayerUIState::~KPlayerUIState()
{

}

void KPlayerUIState::Init()
{
	for (int nState = 0; nState < player_ui_num ; nState ++ )
	{
		m_UiState[nState] = player_ui_state_close;
	}//end for nState

}

PLAYER_UI_SERVER_STATE KPlayerUIState::GetUIState(const PLAYER_UI_SERVER nServerIdx)
{
	if (nServerIdx >= 0  && nServerIdx < player_ui_num)
		return m_UiState[nServerIdx];
	else	
		return player_ui_state_invalid;
}

void KPlayerUIState::SetUIState(const PLAYER_UI_SERVER nServerIdx,const PLAYER_UI_SERVER_STATE nState)
{
	if ( nServerIdx >= 0  && nServerIdx < player_ui_num
		 && nState  >= player_ui_state_close && nState < player_ui_state_num 
		)
	{
		m_UiState[nServerIdx] = nState;
	}//endif
}

void KPlayerUIState::PlayerOffLineNotify()
{
	Init();
}

void KPlayerUIState::PlayerWalkNotify()
{
	for (int nState = 0; nState < player_ui_num ; nState ++ )
	{
		m_UiState[nState] = player_ui_state_close;
	}//end for nState
}

#endif

KPlayer::KPlayer()
{
	m_moneyMgr.SetBelongPlayer( this );

	m_pGMScript = NULL;
	m_DebugMode = false;
	SetCreditState( disable );
	SetCreditReturnTime( 0 );

	Release();

	m_dwLastSwitchHorseTime = 0;
#ifdef _SERVER
	m_bIsWillRide			= false;
	m_cTrade.initState();
	m_DeathPunish			= 1;
	m_PkPunish				= 1;
	m_IsKiller				= false;
	m_dwTotolJinshabi			= 0;
	m_dwRecentlyTime			= 0;
	m_dwRecentJinshabi		= 0;

#else
	m_bIsPreRide			= false;
	m_bEnableToThrowAwayItem = true;
	m_IsOverweight			= false;
#endif

#ifndef _SERVER
    m_TaisuiSys.Init();
#endif

}

KPlayer::~KPlayer()
{
	Release();
}

DWORD KPlayer::GetPlusPointRecord(int plusPointRecordIdx)
{
	PlusPointTable& ppt = PlusPointTable::Singleton();
	if ( ppt.IsOkPlusPointIdx( plusPointRecordIdx ) )
	{
		return m_plusPointRecord[plusPointRecordIdx];
	}
	else
	{
		return 0;
	}
}

DWORD KPlayer::GetPlusPoint(int plusPointIdx)
{
	PlusPointTable& ppt = PlusPointTable::Singleton();
	if ( ppt.IsOkPlusPointIdx( plusPointIdx ) )
	{
		return m_plusPointArray[plusPointIdx];
	}
	else
	{
		return 0;
	}
}

bool KPlayer::AddPlusPoint(int plusPointIdx, DWORD plusPoint )
{
	int checkPoint = (int)plusPoint;
	if ( checkPoint < 0 )
	{
		return false;
	}

	PlusPointTable& ppt = PlusPointTable::Singleton();
	if ( ppt.IsOkPlusPointIdx( plusPointIdx ) && 
		plusPoint > 0 )
	{
		DWORD maxPoint = 0;
		ppt.GetPlusPointMax( plusPointIdx, maxPoint );
		
		DWORD resultpoint = m_plusPointArray[plusPointIdx] + plusPoint;
		if ( resultpoint < maxPoint && resultpoint > m_plusPointArray[plusPointIdx] )
		{
			m_plusPointArray[plusPointIdx] += plusPoint;
			if ( ((m_plusPointRecord[plusPointIdx] + plusPoint) < 0xffffffff) &&
				((m_plusPointRecord[plusPointIdx] + plusPoint) > m_plusPointRecord[plusPointIdx] ) )
			{
				m_plusPointRecord[plusPointIdx] += plusPoint;
			}
			else
			{
				m_plusPointRecord[plusPointIdx] = 0xffffffff;
			}
		}
		else
		{
			plusPoint = maxPoint - m_plusPointArray[plusPointIdx];

			m_plusPointArray[plusPointIdx] = maxPoint;

			if ( ((m_plusPointRecord[plusPointIdx] + plusPoint) < 0xffffffff) &&
				((m_plusPointRecord[plusPointIdx] + plusPoint) >= m_plusPointRecord[plusPointIdx] ) )
			{
				m_plusPointRecord[plusPointIdx] += plusPoint;
			}
			else
			{
				m_plusPointRecord[plusPointIdx] = 0xffffffff;
			}
		}
#ifdef _SERVER
		SyncAttribute((enumSyncAttribute)(attr_pluspoint0 + plusPointIdx));
#endif		
		return true;
	}
	else
	{
		return false;
	}
}

bool KPlayer::DecPlusPoint(int plusPointIdx, DWORD plusPoint )
{
	int checkPoint = (int)plusPoint;
	if ( checkPoint < 0 )
	{
		return false;
	}

	PlusPointTable& ppt = PlusPointTable::Singleton();
	if ( ppt.IsOkPlusPointIdx( plusPointIdx )  && 
		plusPoint > 0 && 
		plusPoint <= m_plusPointArray[plusPointIdx] )
	{
		m_plusPointArray[plusPointIdx] -= plusPoint;
#ifdef _SERVER
		SyncAttribute((enumSyncAttribute)(attr_pluspoint0 + plusPointIdx));
#endif
		return true;
	}
	else
	{
		return false;
	}
}


void KPlayer::Release()
{
	m_TaisuiSys.ReFresh();

	m_nDialogNpcKind = -1;
	m_nDeathDecExp = 0;
	DWORD dwLen = sizeof( KPlayer );
	DWORD dwVar = (DWORD)(&m_nNetConnectIdx);
	DWORD dwOff = dwVar - (DWORD)( (DWORD *)this );
	m_dwID = 0;
	m_nIndex = 0;
	m_nNetConnectIdx = -1;
	m_cTrade.Release();
	m_nSkillExp = 0;
	m_nWeightMax = 0;
	m_nWeightMaxTempAdd = 0;
	m_nBeNpcKillOption = 0;
	m_btChatSpecialChannel = 0;
	m_nExp = 0;
	m_nSubExp = 0;
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
	m_nBuildingIdx = 0;
	m_bWaitingPlayerFeedBack = false;
	m_bMultiSelection = false;
	m_dwWaitingPlayerFeedBackSeed = 0;
	m_btTryExecuteScriptTimes = 0;
	m_bRandomAddAttr = TRUE;
	m_bNewPlayer = 0;
	m_SkillSeries = role_skillseries_invalid;
	m_CanPickup = true;
	m_IsBlockClient = false;
	m_Ticket = 0;
	m_EmployTime = 0;
	m_CombatInfo.nScore = 0;
	m_IsGM = false;
	m_TitleManager.Init(m_nPlayerIndex);

	m_lastChongZhiTime = 0;

	m_InsuranceMgr.Init(m_nPlayerIndex);
	memset(&m_plusPointArray, 0, sizeof(m_plusPointArray));
	memset(&m_plusPointRecord, 0, sizeof(m_plusPointRecord));
	

    //end
#ifdef _SERVER

	m_MarkCreatureNpcIdx = 0;
	m_DBLoadProcessFlag = 0;
	m_dwLastOfflineTime = 0;
	m_LogoutTimer = 0;
	m_loadOwnTreeInterval = BASERETRYTIME_LOAD_LITTLETREE + g_Random(EXTRETRYTIME_LOAD_LITTLETREE);
	
	//InitPlayerSaveTimeInterval();

	//time(&m_LastAddExpTime);
	m_LastAddExpTime = UNIX_TMIE_STAMP;

	m_btMorphHue = 0;
	m_bMorphSendHue = true;
	m_bPreventAddFriend = false;
	memset(&m_tagTempAddCredit, 0, sizeof(TEMPADDSTATUSINFO));
	memset(&m_tagTempAddSkill, 0, sizeof(TEMPADDSTATUSINFO));
	memset(&m_tagTempAddAllSkill, 0, sizeof(TEMPADDSTATUSINFO));
	m_offerPostId = 0;
	m_applyPostId = 0;
	m_missionCount = 0;
	memset(m_szMasterName, 0, 32);
	m_nPrenticeNum = 0;
	m_byPrenticeLevel = 0;
	m_byMasterPRValueTemp = 0;
	m_Creature.Dismiss();
	m_Creature.m_nLastCanSummonTime = 0;
	m_Employee.Init();
	m_bUseReviveIdWhenLogin = 0;
	m_dwDeathScriptId = 0;
	m_sLoginRevivalPos.m_nSubWorldID = 0;
	m_sLoginRevivalPos.m_nMpsX = 0;
	m_sLoginRevivalPos.m_nMpsY = 0;	
	m_sDeathRevivalPos.m_nSubWorldID = 0;
	m_sDeathRevivalPos.m_nMpsX = 0;
	m_sDeathRevivalPos.m_nMpsY = 0;	
	m_pLastScriptCacheNode = NULL;
	m_dwLoginTime			= -1;
	m_bFinishLoading = FALSE;
	m_uMustSave = SAVE_IDLE;
	m_bIsQuiting = FALSE;
	m_bIsCanRemove = FALSE;
//	m_TimerTask.SetOwner(this);
	m_bSleepMode = FALSE;
	m_nLastNetOperationTime = 0;
    memset(&m_ExtPointInfo, 0, sizeof(m_ExtPointInfo));
	m_nEarnMoreMoneyP = 0;
	m_nGetMoreExpP = 0;
	m_nGetMoreSkillExpP = 0;
	m_bNotifyLogout = FALSE;
	memset(m_szBoxPassword, 0, MAX_BOXPASSWORD);
	m_nLastUsedSkillType = 0;
	if(m_pGMScript != NULL)
	{
		delete m_pGMScript;
		m_pGMScript = NULL;
	}
	m_uPlayCard = 0;
	unlockStoreBox();
	m_PreFightMode			=	0;
	m_dwQuestionScriptId	=	0;
	memset(m_szSecPW,0,sizeof(m_szSecPW));
	memset(m_szClientSecPW,0,sizeof(m_szClientSecPW));
	m_dwLastLoginIP = 0;
	m_bPermitChangeServer	=	true;
	m_bPermitAttach			=	true;
    m_tmtCurBegin = 0;
    m_tmtCurEnd = 0;    
    m_tmtLastEnd = 0;
    m_dwPowerValue = 0;
    m_byGiftType = 0;
    m_nLSkillID = 1;
    m_nRSkillID = 1;
    m_SendFlag = 0;
	m_bPermitChangeServer = true;	
	m_dwLastDeathTime = 0;
	m_bPetReleased = false;
	m_byPetType = enPetTypeNone;
	m_byPetHonor = 0;
	m_byPetColor = 0;
	m_dwPetTimer = 0;
	m_dwPetChatTimer = 0;
	memset(m_szPetName, 0, sizeof(m_szPetName));
	m_dwInitTreasureCount = 0;
	m_OverweightBuffIndex = 0;
	m_RelationSet.Clear();
	m_cTask.Release();
	m_nExpPercentage = 100;
	m_nQuestExpPercentage = 100;
	m_nSkillExpPercentage = 100;
	m_SpyLevel = 0;
	m_RememberSubworldId = 0;
	m_RememberPosX = 0;
	m_RememberPosY = 0;
	m_NextTeamTime = 0;
	memset(m_NextOpTime, 0, sizeof(m_NextOpTime));
	memset(&m_interactiveScriptState, 0, sizeof(m_interactiveScriptState));
	m_FuryMgr.Release();
	m_CombatScoreOneTime = 0;
	m_nMyTurn = 0;
	m_RecommenderRewardToAdd = 0;
	m_RecommenderRewardTicketAdded = 0;
	m_CreateTime = 0;
	m_LoginGameScriptDone = false;
	m_PlayerUIState.Init();
	m_QuestionState.Init(m_nPlayerIndex);
	m_ExpItemIndex = 0;
	m_PlayerExpGainPercent = 0;
	m_ItemExpGainPercent = 0;
#else

	m_nQuestionClientLen = 0;
	m_RunStatus = 0;
	m_nSendMoveFrames = defMAX_PLAYER_SEND_MOVE_FRAME;
	m_nCurrentAttrSyncTime = 0;
	m_dwWeakTime = 0;
	m_nLoseExp = 0;
	ZeroMemory( &m_HandItemPos, sizeof( m_HandItemPos ) );
	m_IsOverweight = false;
	m_CityTaxRate = 0;
	m_CityGoodsDiscount = 100;
	m_SocialRelation.Clean();
	QuestLog::GetInstance()->Release();
    ClearRunPackageRecord();
	m_Money = 0;
	m_IsPasswordExist = FALSE;
#endif

#ifdef _SERVER
	m_ExpInsuranceMgr.Release();
	m_QuestInsuranceMgr.Release();
#else
	m_CurrentExpReward    = 0;
	m_IsExpInsuraceValid  = false;

	m_CurrentQuestReward    = 0;
	m_IsQuestInsuranceValid = false;

#endif

}

void KPlayer::SetPlayerIndex(int nNo)
{
	if (nNo < 0)
		m_nPlayerIndex = 0;
	else
		m_nPlayerIndex = nNo;
#ifdef _SERVER
	m_cTask.m_nPlayerIdx = m_nPlayerIndex;
	m_Employee.SetPlayerIndex(m_nPlayerIndex);
#endif
}

void KPlayer::SetNewPlayer(BYTE bNew)
{
	m_bNewPlayer = bNew;
}

void KPlayer::Active()
{	
#ifndef _SERVER
	PlayerController::Singleton().Active();
	ActivatePakcageRecord();
#endif

#ifdef _SERVER

	int gameTime = g_SubWorldSet.GetGameTime();
	if ( gameTime % (GAME_FPS / 2) == 0)
		SendCurNormalSyncData();

	if( IsLogoutTiming() )
		--m_LogoutTimer;
	
	if ( gameTime % GAME_FPS == 0 )
		m_ItemList.LoopGroupCoolDown();
	
	if ( gameTime % (2 * GAME_FPS) == 0 )
		GetTeamInfo().SendTeamSyncData();


	if ( gameTime % (10 * GAME_FPS) == 0 )
	{
		CheckCreditState();
	}	
	
	if ( (enPDBMask_Item == (m_DBLoadProcessFlag & enPDBMask_Item))
		  &&  
		  gameTime % ( 4 * GAME_FPS) == 0 )
	{
		m_ItemList.CheckEquipmentExpireTime();
	}//endif

	KNpc& npc = Npc[m_nIndex];
	if (npc.IsInWorldCombatInstance())
		ActiveInCombatMap();

	// 如果玩家上线时由于数据库太繁忙而导致无法Load关系树
	// 那么间隔一定时间后在这里再请求一次
	// m_loadLittleTreeInterval 是随机值，不同player不一样
	// 避免请求操作都出现在同一桢内
	if( !(g_SubWorldSet.GetGameTime() % m_loadOwnTreeInterval) )
	{
		for(int nTplId = enSUTplId_None + 1; nTplId < enSUTplId_Num; ++nTplId)
		{
			if( m_RelationSet.IsOwnTreeLoad(nTplId) )
				continue;

			RelationRecord	*pRec = m_RelationSet.GetRelationByTemplate(nTplId);

			_ASSERT(pRec);
			if(NULL == pRec)
				continue;

			SocialSerializer::Singleton().LoadTreeUpReq(m_nNetConnectIdx, 
														pRec->TplId, 
														pRec->ParentGuid
															 );
		}
	}

//	if( 0 == (g_SubWorldSet.GetGameTime() % m_PlayerSaveTimeInterval) )
//		Save(NULL);

	if( !(g_SubWorldSet.GetGameTime() % AntiEnthrall::INC_ONLINETIME_INTERVAL) )
	{
		m_AntiEnthrall.IncOnlineTime(Npc[m_nIndex].GetPlayerIdx());
	}

	m_ActionDelayer.Active();
	m_PlayerStatistic.Active();
	GetTeamInfo().Active();
	ProcessAutoAttack();
	m_FuryMgr.Active();
	m_Employee.Active();
	m_ExpInsuranceMgr.Active();
	m_TitleManager.Active();

	CheckInteractiveScriptTimeout();

	if (!m_LoginGameScriptDone && (m_DBLoadFinishedFlag == m_DBLoadProcessFlag))
	{	
		m_LoginGameScriptDone = true;
		ExecuteScript(m_dwDeathScriptId, "LoginGame", 0);

		//得到并执行GM指令
		GetGMCmd();
	}
	
#else//client

	// 队伍申请人的处理
	if ( !m_cTeam.IsInTeam() )
	{
		if (m_cTeam.m_nApplyCaptainID > 0)
		{
			if ( m_cTeam.m_dwApplyTimer == 0 )
			{
				m_cTeam.m_nApplyCaptainID = 0;
				
			}
			else
			{
				m_cTeam.m_dwApplyTimer--;
				if ( !NpcSet.SearchID(m_cTeam.m_nApplyCaptainID) )
				{
					m_cTeam.m_nApplyCaptainID = 0;
					m_cTeam.m_dwApplyTimer = 0;
				}
			}
		}
	}
	// 队长的处理
	else if (m_cTeam.IsCaptain())
	{
		for (int i = 0; i < MAX_TEAM_APPLY_LIST; i++)
		{
			if (m_cTeam.m_sApplyList[i].m_dwNpcID > 0)
			{
				if (m_cTeam.m_sApplyList[i].m_dwTimer == 0)
				{
					m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
					m_cTeam.UpdateInterface();
				}
				else
				{
					m_cTeam.m_sApplyList[i].m_dwTimer--;
					if ( !Npc[this->m_nIndex].SearchAroundID(m_cTeam.m_sApplyList[i].m_dwNpcID) )
					{
						m_cTeam.m_sApplyList[i].m_dwNpcID = 0;
						m_cTeam.m_sApplyList[i].m_dwTimer = 0;
						m_cTeam.UpdateInterface();
					}
				}
			}
		}
	}

	m_nSendMoveFrames++;

	KNpc &self = Npc[m_nIndex];
	const int &nPetIndex = self.m_nPetIndex;
	if ( nPetIndex != 0 )
	{
		KNpc &pet = Npc[nPetIndex];

		if ( SubWorld[0].FindRegion(pet.m_dwRegionID) == -1 )
		{		
			int nMapX = GetRandomNumber(0, 15);
			int nMapY = GetRandomNumber(0, 15);
			int nOffX = GetRandomNumber(0, 31);
			int nOffY = GetRandomNumber(0, 31);
			
			KRegion &curRegion = SubWorld[0].m_Region[self.m_RegionIndex];
			const int nDirection[8] = 
			{
				curRegion.m_nConnectRegion[DIR_DOWN],
				curRegion.m_nConnectRegion[DIR_LEFTDOWN],
				curRegion.m_nConnectRegion[DIR_LEFT],
				curRegion.m_nConnectRegion[DIR_LEFTUP],
				curRegion.m_nConnectRegion[DIR_UP],
				curRegion.m_nConnectRegion[DIR_RIGHTUP],
				curRegion.m_nConnectRegion[DIR_RIGHT],
				curRegion.m_nConnectRegion[DIR_RIGHTDOWN],
				
			};

			int nSelfDirection = KNpcFindPath::Dir64To8(self.m_UnaryAttrMgr[nuai_dir]); 

			int nRegionIndex = nDirection[(nSelfDirection + 4) & 0x07];				
			
			pet.MoveNpc(nRegionIndex, nMapX, nMapY, nOffX, nOffY);
			
			int nSelfMpsX, nSelfMpsY;
			self.GetMpsPos(&nSelfMpsX, &nSelfMpsY);
			NPCCMD nDoing = GetRandomNumber(0, 1) == 0 ? do_walk : do_run;

			pet.SendCommand(nDoing, nSelfMpsX, nSelfMpsY);

		}
	}

#endif

#ifndef _SERVER
#ifdef _AUTO_ROBOT
	AutoRobotMgr::Singleton().Activate();
#endif
#endif
	
#ifdef _SERVER
	
	ChatObjectMgr_S	*pObjMgr = g_ChatCenterS.GetChatObjMgr( Npc[m_nIndex].GetPlayerIdx() );
	if (pObjMgr)
		pObjMgr->CheckFriendOnlineFlag();

	m_QuestionState.Active();

#endif

}

#ifdef _SERVER

void KPlayer::ActiveInCombatMap()
{
	DWORD dwGameTime = g_SubWorldSet.GetGameTime();

	if ( dwGameTime % (GAME_FPS) == 0 )//同步战场排名
	{
		KNpc& npc = Npc[m_nIndex];
		int nSubworldIndex = npc.GetSubWorldIndex();

		if (nSubworldIndex != INVALID_WORLD_INDEX && nSubworldIndex >= 0 && nSubworldIndex < MAX_SUBWORLD)
		{
			if (npc.IsInWorldCombatInstance() && SubWorld[nSubworldIndex].CanGainScore() && npc.GetCombatScoreCalcType() == PROGRAME_CALU)
			{
				int CombatTop10Interval = ConfigManager::Singleton().GetGlobalVariable(global_var_combat_top10_interval);
				
				if(CombatTop10Interval < COMBAT_TOP10_SYNC_MIN_INTERVAL || CombatTop10Interval > COMBAT_TOP10_SYNC_MAX_INTERVAL)
					CombatTop10Interval = COMBAT_TOP10_SYNC_MIN_INTERVAL;
				
				int SyncSwitch = ConfigManager::Singleton().GetGlobalVariable(global_var_combat_top10_switch);
				
				if ( SyncSwitch != 0 && (( dwGameTime / GAME_FPS) % CombatTop10Interval) == m_nMyTurn)
				{
					int nOrgId = Npc[m_nIndex].m_WorldCombatOrg;
					SendSyncCombatTop10(nOrgId);
				}//endif
			}//endif
		}
	}//endif
	
	CheckSendWorldCombatInfo(); //同步阵营总分
}

#endif

#ifndef _SERVER

// void KPlayer::Walk(int nDir, int nSpeed)
// {
// 	int	nMapX = Npc[m_nIndex].GetMapX();
// 	int nMapY = Npc[m_nIndex].GetMapY();
// 	int	nOffX = Npc[m_nIndex].GetOffX();
// 	int	nOffY = Npc[m_nIndex].GetOffY();
// 	int	nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
// 	int	nRegion = Npc[m_nIndex].m_RegionIndex;
// 	int	nX, nY;
// 	
// 	SubWorld[nSubWorld].Map2Mps(nRegion, nMapX, nMapY, nOffX, nOffY, &nX, &nY);
// 	SubWorld[nSubWorld].GetMps(&nX, &nY, nSpeed * 2, nDir);
// 	
// 	if (m_RunStatus)
// 	{
// 		Npc[m_nIndex].SendCommand(do_run, nX, nY);
// 
// 		if ( !CheckTrading() )
// 			SendClientCmdRun(nX, nY);		
// 	}
// 	else
// 	{
// 		Npc[m_nIndex].SendCommand(do_walk, nX, nY);
// 		// Send to Server
// 		if (!CheckTrading())
// 			SendClientCmdWalk(nX, nY);
// 	}
// }


void KPlayer::TurnLeft()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir] > 8)
		Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_dir, Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir] - 8 );
	else
		Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_dir, MAX_NPC_DIR - 1 );

	if (Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_Doing != do_stand &&
		Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_Doing != do_sit)
		return;
	
	if (Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_UnaryAttrMgr[nuai_dir] > 8)
		Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_UnaryAttrMgr.Set(nuai_dir, Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir] - 8 );
	else
		Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_UnaryAttrMgr.Set(nuai_dir, MAX_NPC_DIR - 1 );
}

void KPlayer::TurnRight()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir] < MAX_NPC_DIR - 9)
		Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_dir, Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir] + 8 );
	else
		Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_dir, 0 );


	if (Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_Doing != do_stand &&
		Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_Doing != do_sit)
		return;
	
	if (Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_UnaryAttrMgr[nuai_dir] < MAX_NPC_DIR - 9)
		Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_UnaryAttrMgr.Set(nuai_dir, Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir] + 8 );
	else
		Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_UnaryAttrMgr.Set(nuai_dir, 0 );
}

void KPlayer::TurnBack()
{
	if (Npc[m_nIndex].m_Doing != do_stand &&
		Npc[m_nIndex].m_Doing != do_sit)
		return;
	
	if (Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir] < MAX_NPC_DIR / 2)
		Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_dir, Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir] + MAX_NPC_DIR / 2 );
	else
		Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_dir, Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir] - MAX_NPC_DIR / 2 );

	if (Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_Doing != do_stand &&
		Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_Doing != do_sit)
		return;
	
	if (Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_UnaryAttrMgr[nuai_dir] < MAX_NPC_DIR / 2)
		Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_UnaryAttrMgr.Set(nuai_dir, Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir] + MAX_NPC_DIR / 2 );
	else
		Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_UnaryAttrMgr.Set(nuai_dir, Npc[m_nIndex].m_UnaryAttrMgr[nuai_dir] - MAX_NPC_DIR / 2 );
}

void KPlayer::SetTargetNpc(int n) 
{ 
	m_nPeapleIdx = n; 
}

static int ulOldPeapleIdx = 0;

int KPlayer::FindSelectNpc(int x, int y, int nRelation, bool bSearchSelf /* = false */ , bool bNoPlayer /* = false*/)
{
	m_nPeapleIdx = NpcSet.SearchNpcAt(x, y, nRelation, 40, bSearchSelf,bNoPlayer);
	if ( m_nPeapleIdx > 0 && ulOldPeapleIdx != m_nPeapleIdx )
	{
		ulOldPeapleIdx = m_nPeapleIdx;
		KCacheNode* pSoundNode = NULL;
		pSoundNode = g_SoundCache.GetNode(HOVER_NPC_SOUND, (KCacheNode*)pSoundNode);
		if ( pSoundNode )
		{
			KWavSound* pWave = (KWavSound*)pSoundNode->m_lpData;
			if (pWave)
			{
				if (pWave->IsPlaying())
				{
					int nVolume = Option.GetSndVolume();
					pWave->SetVolume( nVolume );
				}
				else
				{
					int nVolume = Option.GetSndVolume();
					pWave->Play(0, nVolume, false);
				}
			}
		}
	}
	return m_nPeapleIdx;
}

int KPlayer::FindSelectObject(int x, int y)
{
	return m_nObjectIdx = ObjSet.SearchObjAt(x, y, 50);	// 数字表示拾取范围
}

BOOL KPlayer::ConformIdx(int nIdx)
{
	if (nIdx == m_nIndex || nIdx == 0)
		return FALSE;
	return TRUE;
}

#endif


#ifdef _SERVER

void KPlayer::WaitForRemove()
{
	m_bIsQuiting = TRUE;
}

BOOL KPlayer::IsLoginTimeOut()
{
	if (m_nNetConnectIdx != -1)
		return FALSE;
	
	if (!m_dwID)
		return FALSE;
	
	if (-1 != m_dwLoginTime &&
		g_SubWorldSet.GetGameTime() - m_dwLoginTime > defPLAYER_LOGIN_TIMEOUT)
	{
		//		m_dwLoginTime = -1;
		return TRUE;
	}
	
	return FALSE;
}


void KPlayer::LoginTimeOut()
{

//	m_pStatusLoadPlayerInfo = NULL;
	
	Release();
}

BOOL KPlayer::Save( BYTE* pBuf, BOOL bOffline )
{
	if (m_nIndex <= 0 && m_dwID == 0)
		return FALSE;

	// 角色数据没有全部加载完成，不允许存盘
	if(m_DBLoadFinishedFlag != m_DBLoadProcessFlag)
		return FALSE;
	
	// Player上的角色名字和Npc上的角色名字不一致了，即NPC串了
	if (0 != strcmp(m_PlayerName, Npc[m_nIndex].Name))
	{
		char wrongNpcInfo[256] = { 0 };
		snprintf(wrongNpcInfo, sizeof(wrongNpcInfo), "WrongNpc: PlayerName=\"%s\", NpcName=\"%s\"", m_PlayerName, Npc[m_nIndex].Name);
		wrongNpcInfo[sizeof(wrongNpcInfo) - 1] = 0;
		g_pLogSystem->SysDbgLog(wrongNpcInfo, strlen(wrongNpcInfo), sys_dbg_log_event_wrong_npc);
		return FALSE;
	}

	//顺序不要轻易改变
	if( !SaveEnhanceData( ) )
		CFS_FILELOGS::WriteLog("%s Buff Save Failed!\n", Npc[m_nIndex].Name);

	if( !SaveItemData( ) )
		CFS_FILELOGS::WriteLog("%s Item Save Failed!\n", Npc[m_nIndex].Name);

	if( !SaveSkillData( ) )
		CFS_FILELOGS::WriteLog("%s Skill Save Failed!\n", Npc[m_nIndex].Name);

	if( !SaveTaskData( ) )
		CFS_FILELOGS::WriteLog("%s Task Save Failed!\n", Npc[m_nIndex].Name);

	if (PlayerSet.GetSocialSaveSwitch() || bOffline)	//下线才存社会关系的判断
	{	
		if( !SaveSocialData( ) )
		CFS_FILELOGS::WriteLog("%s Social Save Failed!\n", Npc[m_nIndex].Name);
	}

	if( !SaveFriendData( ) )
		CFS_FILELOGS::WriteLog("%s Friend Save Failed!\n", Npc[m_nIndex].Name);

	if( !SaveReserveData( ) )
		CFS_FILELOGS::WriteLog("%s Reserve Save Failed!\n", Npc[m_nIndex].Name);

	if( !SaveBaseInfoData( ) )
		CFS_FILELOGS::WriteLog("%s BaseInfo Save Failed!\n", Npc[m_nIndex].Name);

	if (!SaveInsurance())
		CFS_FILELOGS::WriteLog("%s Insurance Save Failed!\n", Npc[m_nIndex].Name);

	if( bOffline )
		BuffMgr::Singleton( ).PlayerOffline( m_nIndex );

	m_ulLastSaveTime = g_SubWorldSet.GetGameTime();

	return TRUE;
}

BOOL KPlayer::CanSave()
{
	if (m_nNetConnectIdx == -1)
		return FALSE;
	
	if (m_nIndex <= 0)
		return FALSE;
	
	if (m_bIsQuiting)
		return FALSE;
	
	if (m_cTrade.getState() == KTrade::TRADE_IDLE)
	{
		return FALSE;
	}
	
	if (m_uMustSave == SAVE_DOING && g_SubWorldSet.GetGameTime() - m_ulLastSaveTime > defPLAYER_SAVE_TIMEOUT)
		return TRUE;
	
	if (m_uMustSave != SAVE_IDLE)
		return FALSE;
	
	return TRUE;
}

BOOL KPlayer::SendSyncData( int nType )
{	
	BOOL bRet = FALSE;
	switch( nType )
	{
	case Proc_GetRoleBaseData:
		{	
			bRet = SubWorld[Npc[m_nIndex].m_SubWorldIndex].SendSyncData(m_nIndex, m_nNetConnectIdx);
			if (!bRet)
			{
				break;
			}
			bRet = Npc[m_nIndex].SendSyncData(m_nNetConnectIdx,m_nIndex);
			if (!bRet)
			{
				break;
			}
			// 这个消息必须在同步世界NPC数据后做，使客户端能找到当前玩家在客户端的Npc索引
			CURPLAYER_SYNC	sSync;	// 同步当前玩家的自身独特信息给客户端（装备等）
			sSync.ProtocolType = (BYTE)s2c_synccurplayer;
			sSync.bRandomAddAttr = m_bRandomAddAttr;
			sSync.m_dwID = Npc[m_nIndex].m_dwID;
			sSync.m_btLevel = (DWORD)Npc[m_nIndex].m_Level;
			sSync.m_btSex = Npc[m_nIndex].m_nSex;
			sSync.m_btKind = Npc[m_nIndex].m_Kind;
			sSync.m_btSeries = Npc[m_nIndex].m_Series;
			sSync.m_wLifeMax = Npc[m_nIndex].m_CompAttrMgr[ncai_lifeuplimit][idx_base_value];
			sSync.m_wManaMax = Npc[m_nIndex].m_CompAttrMgr[ncai_manauplimit][idx_base_value];
			sSync.m_wCurLife = Npc[m_nIndex].m_UnaryAttrMgr[nuai_curlife];
			sSync.m_wCurMana = Npc[m_nIndex].m_UnaryAttrMgr[nuai_curmana];
			sSync.m_HeadImage = Npc[m_nIndex].m_nHeadImage;
			sSync.m_SkillSeries = m_SkillSeries;
			sSync.m_dwExp = m_nExp;
			sSync.m_dwSkillExp = m_nSkillExp;
			sSync.nWeightMax = m_nWeightMax;
			sSync.m_wBody = Npc[m_nIndex].m_CompAttrMgr[ncai_body][idx_base_value];
			sSync.m_wNimbus = Npc[m_nIndex].m_CompAttrMgr[ncai_nimbus][idx_base_value];
			sSync.m_wStrength = Npc[m_nIndex].m_CompAttrMgr[ncai_strength][idx_base_value];
			sSync.m_wArt = Npc[m_nIndex].m_CompAttrMgr[ncai_art][idx_base_value];
			sSync.m_wWorldStat = (WORD)m_nWorldStat;
			sSync.m_nMoney1 = m_ItemList.GetMoney(room_equipment);
			sSync.m_nMoney2 = m_ItemList.GetMoney(room_repository);
			sSync.bIsBlockClientControl = IsBlockClientControl() ? TRUE : FALSE;

			if (g_pServer != NULL && SUCCEEDED(g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(CURPLAYER_SYNC))))
			{
				m_TitleManager.SyncSelfTitle();
				bRet = TRUE;				
			}
			else
			{
				bRet = FALSE;
				break;
			}
		}
		break;
	default:
		break;
	}
	return bRet;
}

//-------> Ray [Luoliang] 2005-6-29
BOOL KPlayer::SendPetSyncData(int nClient)
{
	BOOL bRet = FALSE;
	//如果有宠物的话
	if ( m_bPetReleased && m_byPetType != enPetTypeNone )
	{
		PET_INIT_SYNC petSync;
		petSync.ProtocolType = s2c_pet;
		petSync.PetProtocal = pet_s2c_init_sync;
		petSync.dwID = Npc[m_nIndex].m_dwID;
		int nPetNameLen = strlen(m_szPetName);
		petSync.wLength = sizeof(PET_INIT_SYNC) - 1 - sizeof(petSync.szPetName) + nPetNameLen;
		petSync.bPetReleased = m_bPetReleased;
		petSync.byPetHonor = m_byPetHonor;
		petSync.byPetType = m_byPetType;
		petSync.byPetColor = m_byPetColor;
		memcpy(petSync.szPetName, m_szPetName, nPetNameLen);
		if (g_pServer != NULL && SUCCEEDED(g_pServer->PackDataToClient(nClient, &petSync, petSync.wLength + 1)) )
		{
			bRet = TRUE;
		}		
	}
	return bRet;
}

BOOL KPlayer::SendPetNormalSyncData(int nClient)
{
	BOOL bRet = FALSE;
	PET_NORMAL_SYNC petSync;
	petSync.SetProtocolHeader(pet_s2c_normal_sync, sizeof(PET_NORMAL_SYNC) - 1);
	petSync.dwID = Npc[m_nIndex].m_dwID;
	petSync.bPetReleased = m_bPetReleased;
	petSync.byPetType = m_byPetType;
	petSync.byPetHonor = m_byPetHonor;
	petSync.byPetColor = m_byPetColor;
	if (g_pServer != NULL && SUCCEEDED(g_pServer->PackDataToClient(nClient, &petSync, petSync.wLength + 1)) )
	{
		bRet = TRUE;
	}
	return bRet;
}
//<------- End [Ray]

BOOL KPlayer::SendSyncData_Skill()
{
	SKILL_SEND_ALL_SYNC		syncData;
	syncData.ProtocolType = s2c_synccurplayerskill;

	int nSize = Npc[m_nIndex].m_SkillList.SaveSkillData((BYTE*)&syncData.m_version);
	syncData.m_wProtocolLong = 2 + nSize;

	
	if (g_pServer != NULL && SUCCEEDED(g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&syncData, syncData.m_wProtocolLong + 1)))
		return TRUE;
	else
		return FALSE;
}

void KPlayer::SendSyncCombatTop10(int orgId)
{
	if (IsValidCombatID(orgId))
	{
		int SubworldIndex = INVALID_WORLD_INDEX;

		if(IsValidNpc(m_nIndex))
			SubworldIndex = Npc[m_nIndex].GetSubWorldIndex();
		
		if (SubworldIndex >= 0 && SubworldIndex < MAX_SUBWORLD && SubWorld[SubworldIndex].IsWorldCombatMap())
		{
			WordCombatInstanceInfo& pCombatInfo = SubWorld[SubworldIndex].GetCombatInstanceInfo();
			int index = orgId - 1;
			if (index >=0 && index < MAX_SCORE_ORG_SYNC)
			{
				COMBAT_TOP10_INFO *pSyncComBatTopInfo =  (COMBAT_TOP10_INFO* )pCombatInfo.CombatTop10[index].protocolBuff;
				if (g_pServer != NULL && pCombatInfo.org[index].nScore != 0 && pCombatInfo.CombatTop10[index].CompressSuccess)  //当有人得分以后在发包，避免在等待战场开始时发空包,而且在发包时要保证协议是压缩成功的
					g_pServer->PackDataToClient(GetNetConnectIdx(), (void*)pSyncComBatTopInfo, pSyncComBatTopInfo->ProtocolSize + 1);
			}//endif
			
		}//endif

	}//endif
}

#define LIFE_THRESHOLD 0xffff

void	KPlayer::SendCurNormalSyncData()
{
	if (Npc[m_nIndex].m_UnaryAttrMgr[nuai_curlife] <= LIFE_THRESHOLD)
	{
		CURPLAYER_NORMAL_SYNC	sSync;
		
		sSync.ProtocolType = s2c_synccurplayernormal;
		sSync.Life = (WORD)Npc[m_nIndex].m_UnaryAttrMgr[nuai_curlife];
		sSync.Mana = (WORD)Npc[m_nIndex].m_UnaryAttrMgr[nuai_curmana];
		
		/*
		带宽优化前
		sSync.m_shAngry = 0;
		if ( !GetTeamInfo().IsInTeam() )
		sSync.m_btTeamData = 0;
		else
		{
		if (GetTeamInfo().IsCaptain())
		sSync.m_btTeamData = 0x03;
		else
		sSync.m_btTeamData = 0x01;
		}
		*/
		
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sSync, sizeof(CURPLAYER_NORMAL_SYNC));	
	}
	else
	{
		CURPLAYER_NORMAL_SYNC_EX sSync;

		sSync.ProtocolType = s2c_synccurplayernormalex;
		sSync.Life = Npc[m_nIndex].m_UnaryAttrMgr[nuai_curlife];
		sSync.Mana = Npc[m_nIndex].m_UnaryAttrMgr[nuai_curmana];

		if (g_pServer != NULL)
			g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE* )&sSync, sizeof(CURPLAYER_NORMAL_SYNC_EX));
	}
}

void KPlayer::BuyItem(BYTE* pProtocol)
{ 
	PLAYER_BUY_ITEM_COMMAND* pCommand = (PLAYER_BUY_ITEM_COMMAND *)pProtocol;
	//一次最多20个，客户端也会做限制，防止外挂买大于20个的物品，在此加入判断
	if(pCommand->buyCount > 100)
		pCommand->buyCount = 100;

	if (  BuySell.GetPlusPointType(m_BuyInfo.m_nBuyIdx)!= -1 )
	{
		for(int i = 0; i < pCommand->buyCount; i++)
		{
			if(!BuySell.buyByPlusPoint(m_nPlayerIndex, m_BuyInfo.m_nBuyIdx, pCommand->m_BuyIdx))
			{	
				ChatErrCodeToClient(m_nPlayerIndex, chat_err_itembox_not_enough_space);
				break;
			}
		}
		return;
	}

	for(int i = 0; i < pCommand->buyCount; i++)
	{
		if(!BuySell.buy(m_nPlayerIndex, m_BuyInfo.m_nBuyIdx, pCommand->m_BuyIdx))
		{	
			ChatErrCodeToClient(m_nPlayerIndex, chat_err_itembox_not_enough_space);
			break;
		}
	}
}

void KPlayer::SellItem(BYTE* pProtocol)
{
	PLAYER_SELL_ITEM_COMMAND* pCommand = (PLAYER_SELL_ITEM_COMMAND *)pProtocol;
	int insteadSpecieIndex = ConfigManager::Singleton().GetGlobalVariable(globar_var_instead_specie_index);
	int plusPointType = BuySell.GetPlusPointType(m_BuyInfo.m_nBuyIdx);
	if ( plusPointType != -1 && plusPointType != insteadSpecieIndex )
	{
		return;
	}

	BuySell.Sell(m_nPlayerIndex, m_BuyInfo.m_nBuyIdx, m_ItemList.SearchID(pCommand->m_ID));
}

#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：获取玩家重生点位置
//-------------------------------------------------------------------------
void KPlayer::GetLoginRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY)
{
	*lpnSubWorld = m_sLoginRevivalPos.m_nSubWorldID;
	*lpnMpsX = m_sLoginRevivalPos.m_nMpsX;
	*lpnMpsY = m_sLoginRevivalPos.m_nMpsY;
}


void KPlayer::GetDeathRevivalPos(int *lpnSubWorld, int *lpnMpsX, int *lpnMpsY)
{
	*lpnSubWorld = m_sDeathRevivalPos.m_nSubWorldID;
	*lpnMpsX = m_sDeathRevivalPos.m_nMpsX;
	*lpnMpsY = m_sDeathRevivalPos.m_nMpsY;
}
#endif
// #ifdef _SERVER
// void	KPlayer::SetTimer(DWORD nTime, int nTimerTaskId)					//时间任务脚本，开启计时器
// {
// 	if (!nTime || !nTimerTaskId) return ;
// 	m_TimerTask.SetTimer(nTime, nTimerTaskId);
// }
// 
// void	KPlayer::CloseTimer()							//关闭时间计时器
// {
// 	m_TimerTask.CloseTimer();
// }
// #endif


#ifdef _SERVER
//------------------------------------------------------------------------------
//	功能：设定玩家重生点位置
//------------------------------------------------------------------------------
void	KPlayer::SetRevivalPos(int nSubWorld, int nReviveId)
{
	int nOldSubWorld = m_sLoginRevivalPos.m_nSubWorldID;

	if (nSubWorld >= 0)  //如果小于0，表示沿用当前的
	{
		m_sLoginRevivalPos.m_nSubWorldID = nSubWorld;
	}
	else
	{
		m_sLoginRevivalPos.m_nSubWorldID = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;
	}
	
	POINT Pos;
	
	if (g_SubWorldSet.GetRevivalPosFromId(m_sLoginRevivalPos.m_nSubWorldID, nReviveId, &Pos))
	{
		m_sLoginRevivalPos.m_ReviveID = nReviveId;
		m_sLoginRevivalPos.m_nMpsX = Pos.x;
		m_sLoginRevivalPos.m_nMpsY = Pos.y;
		
		m_sDeathRevivalPos = m_sLoginRevivalPos;
	}
	else
	{
		m_sLoginRevivalPos.m_nSubWorldID = nOldSubWorld;
		return;
	}
}

void	KPlayer::ClearDeathRevivaPos( )
{
	POINT deathRevivePos;
	if (g_SubWorldSet.GetRevivalPosFromId(m_sLoginRevivalPos.m_nSubWorldID, m_sLoginRevivalPos.m_ReviveID, &deathRevivePos))
	{
		m_sDeathRevivalPos.m_nSubWorldID = m_sLoginRevivalPos.m_nSubWorldID;
		m_sDeathRevivalPos.m_nMpsX       = deathRevivePos.x;
		m_sDeathRevivalPos.m_nMpsY       = deathRevivePos.y;
	}//endif
	else
	{
		m_sDeathRevivalPos.m_nSubWorldID = defTRANSFER_PORT_ID;
		m_sDeathRevivalPos.m_nMpsX = defTRANSFER_PORT_X;
		m_sDeathRevivalPos.m_nMpsY = defTRANSFER_PORT_Y;	
	}//end for else
}


#endif

#ifdef _SERVER
// not end
void KPlayer::GetAboutPos(KMapPos *pMapPos)
{
	if (m_nIndex <= 0)
		return;
	
	if (Npc[m_nIndex].m_SubWorldIndex < 0)
		return;
	
	POINT Pos;
	int nX, nY;
	Npc[m_nIndex].GetMpsPos(&nX, &nY);
	Pos.x = nX;
	Pos.y = nY;
	
	SubWorld[Npc[m_nIndex].m_SubWorldIndex].GetFreeObjPos(Pos);
	
	pMapPos->nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	SubWorld[Npc[m_nIndex].m_SubWorldIndex].Mps2Map(
		Pos.x, 
		Pos.y, 
		&pMapPos->nRegion, 
		&pMapPos->nMapX, 
		&pMapPos->nMapY, 
		&pMapPos->nOffX, 
		&pMapPos->nOffY);
}

/*
void	KPlayer::GetAboutPos(KMapPos *pMapPos)
{
POINT	Pos[8] = 
{
{0, 32}, {-32, 32}, {-32, 0}, {-32, -32},
{0, -32}, {32, -32}, {32, 0}, {32, 32},
};

  int nMpsX, nMpsY, nTmpX, nTmpY;
  int nR, nMapX, nMapY, nOffX, nOffY;
  Npc[m_nIndex].GetMpsPos(&nMpsX, &nMpsY);
  
	for (int i = 0; i < 8; i++)
	{
	nTmpX = nMpsX + Pos[i].x;
	nTmpY = nMpsY + Pos[i].y;
	if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].GetBarrier(nTmpX, nTmpY))
	continue;
	SubWorld[Npc[m_nIndex].m_SubWorldIndex].Mps2Map(nTmpX, nTmpY, &nR, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nR == -1)
	continue;
	if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nR].GetRef(nMapX, nMapY, obj_object))
	continue;
	else
	break;
	}
	
	  if (i == 8)
	  {
	  pMapPos->nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	  pMapPos->nRegion = Npc[m_nIndex].m_RegionIndex;
	  pMapPos->nMapX = Npc[m_nIndex].m_MapX;
	  pMapPos->nMapY = Npc[m_nIndex].m_MapY;
	  pMapPos->nOffX = Npc[m_nIndex].m_OffX;
	  pMapPos->nOffY = Npc[m_nIndex].m_OffY;
	  }
	  else
	  {
	  pMapPos->nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	  pMapPos->nRegion = nR;
	  pMapPos->nMapX = nMapX;
	  pMapPos->nMapY = nMapY;
	  pMapPos->nOffX = nOffX;
	  pMapPos->nOffY = nOffY;
	  }
}*/
#endif


#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：寻找玩家周围的某个指定npc id的player index
//-------------------------------------------------------------------------
int		KPlayer::FindAroundPlayer(DWORD dwNpcID)
{
	if (dwNpcID == 0)
		return -1;
	
	int		nPlayer, nRegionNo, i;
	nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].FindPlayer(dwNpcID);
	if ( nPlayer >= 0)
		return nPlayer;
	for (i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nPlayer = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].FindPlayer(dwNpcID);
		if (nPlayer >= 0)
			return nPlayer;
	}
	
	return -1;
}

//-------------------------------------------------------------------------
//	功能：判断某玩家是否在周围
//-------------------------------------------------------------------------
BOOL	KPlayer::CheckPlayerAround(int nPlayerIdx)
{
	if (nPlayerIdx <= 0)
		return FALSE;
	if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].CheckPlayerIn(nPlayerIdx))
		return TRUE;
	int		nRegionNo;
	for (int i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		if (SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].CheckPlayerIn(nPlayerIdx))
			return TRUE;
	}
	
	return FALSE;
}

//-------------------------------------------------------------------------
//	功能：寻找玩家周围的某个指定npc id的npc index
//-------------------------------------------------------------------------
int		KPlayer::FindAroundNpc(DWORD dwNpcID)
{
	if (dwNpcID == 0)
		return 0;
	
	int		nNpc, nRegionNo, i;
	nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].SearchNpc(dwNpcID);
	if ( nNpc > 0)
		return nNpc;
	for (i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[Npc[m_nIndex].m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nNpc = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_Region[nRegionNo].SearchNpc(dwNpcID);
		if (nNpc > 0)
			return nNpc;
	}
	
	return 0;
}
#endif

#ifdef _SERVER
void KPlayer::LevelUp()
{	
	KNpc& npc = Npc[m_nIndex];

	if (npc.m_Level >= MAX_LEVEL)
	{
		m_nExp = 0;		
		return;
	}

	DWORD levelupExp = GetExpMax();
	if (m_nExp < levelupExp)
	{
		return;
	}

	//扣除经验
	m_nExp -= levelupExp;

	//提升等级
	npc.m_Level++;

	//到达等级
	int arriveLevel = npc.m_Level;

	//提升至最高等级后，富余经验清空
	if (arriveLevel >= MAX_LEVEL)
	{
		m_nExp = 0;
	}

	m_InsuranceMgr.NotifyPlayerLevelAddTo(arriveLevel);
	m_ExpInsuranceMgr.NotifyLevelUp(arriveLevel);
	m_QuestInsuranceMgr.NotifyLevelUp(arriveLevel);
	
	RefreshPlayerInfoAttr(npc.GetPlayerIdx(), true);

	const LevelUpAdd* pLevelUpAdd = KLevelUpInfo::Singleton().GetLevelUpAdd(arriveLevel, GetSeries(), GetSkillSeries());	
	_ASSERT(pLevelUpAdd != NULL);
	if (pLevelUpAdd == NULL)
		return;

	//SpeciallSkill Levelup
	int nCurSpecialSkill = SpecialSkillTab::Singleton().GetSpecialSkill( GetSeries(), GetSkillSeries(), arriveLevel );
	if ( nCurSpecialSkill > 0 )
	{
		NpcSkillList& skilllist = npc.GetSkillList();
		int nSkillIdx = skilllist.FindSkill( nCurSpecialSkill );
		if ( nSkillIdx )
		{
			int nCurLevel = skilllist.GetLevelByIdx( nSkillIdx );
			skilllist.LevelUpTo( nCurSpecialSkill, nCurLevel + 1 );//*/
		}
	}
	int nLevelupBuffID = SpecialSkillTab::Singleton().GetLevelupBuffID( GetSeries(), GetSkillSeries(), arriveLevel );
	if ( nLevelupBuffID > 0 )
	{
		BuffMgr::Singleton().AddNpcBuff(m_nIndex, m_nIndex, nLevelupBuffID );			
	}

	//属性变化
	MagicData attributeAdd;
	enMagicAttrNo attributes[attr_Count] = { add_body_b, add_nimbus_b, add_strength_b, add_art_b, add_weightmax_v };
	for (int i = 0; i< 5 ; i++)
	{
		attributeAdd.nMagicNo = attributes[i];
		attributeAdd.nVal = pLevelUpAdd->Attribute[i];
		g_MagicAttrModifier.ModifyMagicAttr(m_nIndex, &attributeAdd);
	}

	//InstantSave
	//升级存盘
	//SaveBaseInfoData();

	//给客户端发送数据	
	PLAYER_LEVEL_UP_SYNC sLevelUp;
	sLevelUp.ProtocolType = s2c_playerlevelup;
	sLevelUp.ArriveLevel = (BYTE)arriveLevel;
	sLevelUp.NpcID = npc.GetId();

	int maxBroadcastCount = MAX_BROADCAST_COUNT_MIN;
	npc.BroadCastRegion(&sLevelUp, sizeof(sLevelUp), maxBroadcastCount);

	//给队友发送等级数据
	if (GetTeamInfo().IsInTeam())
	{
		GetTeamInfo().SendMemberBasicInfo();
	}

	//设置升级BUFF
	int levelUpBuffID = ConfigManager::Singleton().GetGlobalVariable(global_var_buff_level_up);
	if (levelUpBuffID > 0)
	{
		unsigned long levelUpBuffIndex = 0;
		levelUpBuffIndex = BuffMgr::Singleton().AddNpcBuff(
			GetNpcIndex(), GetNpcIndex(), levelUpBuffID );
		_ASSERT(levelUpBuffIndex);
	}

	ValidatePlayerState();

	CheckNameColor();

	//日志：升级
	LogEventParam levelUpEent;
	levelUpEent.event = log_event_level_up;
	levelUpEent.param1 = GetGUID();
	snprintf(levelUpEent.param2.data, sizeof(levelUpEent.param2.data), "%d", arriveLevel);
	snprintf(levelUpEent.param3.data, sizeof(levelUpEent.param3.data), "%d %d", GetSeries(), GetSkillSeries());	
	levelUpEent.param4 = GetOnlineTime();
	g_pLogSystem->Log(levelUpEent);

	CheckCreditState();
}
#endif

// need spe edit not end
void	KPlayer::UpdataCurData()
{
// 	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
// 		return;
// 	
// 	{
// 		for(int i = 0; i < ncai_end; ++i)
// 		{
// 			Npc[m_nIndex].m_CompAttrMgr.Set(i, idx_append_value, 0);
// 			Npc[m_nIndex].m_CompAttrMgr.Set(i, idx_append_percent, 0);
// 		}
// 	}	
// 
// 	{
// 		for(int i = nrai_damage_farphysics; i <= nrai_damage_poison; ++i)
// 		{
// 			Npc[m_nIndex].ClearRAAllVal(i);
// 		}
// 	}
// 
// 	Npc[m_nIndex].ClearRAAppendVal(nrai_damage_physics);
// 	Npc[m_nIndex].ClearRAAppendVal(nrai_damage_magic);
// 
// 	{
// 		for(int i = nrai_defend_farphysics; i <= nrai_defend_poison; ++i)
// 		{
// 			Npc[m_nIndex].ClearRAAppendVal(i);
// 		}
// 	}
// 
// 
// 	// Add by Cooler 2004-5-18
// 	// Begin -->
// 	ZeroMemory(&Npc[m_nIndex].m_tagProduceState, sizeof(PRODUCESTATE));
// 	// End <--
// 
// 	// Add by Cooler 2004-5-24
// 	// Begin -->
// //	Npc[m_nIndex].m_nAddProduceSpeedV = 0;
// //	Npc[m_nIndex].m_nAddProduceSpeedP = 0;
// 	// End <--
// 
// 	// Add by Cooler -->
// 	// 2005-1-11
// 	if(m_nWeightMaxTempAdd != 0)
// 	{
// 		m_nWeightMax -= m_nWeightMaxTempAdd;
// 		m_nWeightMaxTempAdd = 0;
// 	}
// 	// End add by Cooler <--
// 
// 	// lixuewu 2005.05.19 依据其他魔法属性的基本规则需要升级时清除一次，
// 	// 不然装备的ReCalc和其他各类增强系统的ReCalc都会出错
// 	// changed by chenshanglin on 2006-2-21 for new skill system
// 	//m_nGetMoreExpP = m_uMoreExp; // m_nGetMoreExpP = 0 ; m_nGetMoreExpP += m_uMoreExp;
// 	m_nGetMoreExpP = 0;
// 	// changed end
// 	// <-- end
// 	
// 	ReCalcEquip();
// 	ReCalcState();
// 	
// #ifdef _SERVER
// 	// 重算增强状态
// 	for(int i = 0 ; i < MAX_ENHANCE_STATE; i++)
// 	{
// 		KNpc::EnhanceState& aEnhanceState = Npc[m_nIndex].m_EnhanceStates[i];
// 		if (aEnhanceState.nTime > 0) 
// 		{
// //			Npc[m_nIndex].ModifyAttrib(m_nIndex, &aEnhanceState.MagicAttrib);
// 		}
// 	}
// #endif
}

//-------------------------------------------------------------------------
//	功能：改变玩家阵营
//-------------------------------------------------------------------------
void	KPlayer::ChangePlayerCamp(int nCamp)
{
	if (nCamp < camp_begin || nCamp >= camp_num)
		return;
	Npc[m_nIndex].m_Camp = (NPCCAMP)nCamp;
}
#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：向服务器申请某个技能升级
//-------------------------------------------------------------------------
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：向服务器申请使用某个物品（鼠标右键点击该物品，只能用于吃药）
//-------------------------------------------------------------------------
BOOL	KPlayer::ApplyUseItem(int nItemID, ItemPos SrcPos, int nTargetID, ItemPos TargetPos )
{
	if (this->CheckTrading())
		return FALSE;
	int nRet = 0;
	nRet = m_ItemList.UseItem(nItemID, nTargetID);
	if (nRet == 0)
	{
		ShowChatErrorMsg(g_szItemErrMsg[item_inlay_error_targetitem_rule]);
		return FALSE;
	}

	int indexOfList = m_ItemList.FindSame(nItemID);
	if (0 == indexOfList)
	{
		return 0;
	}

	if ( (Item[nItemID].GetGenre() == item_target || Item[nItemID].GetGenre() == item_ib) &&
		Item[nItemID].GetParticular() == levelup_tool_targetitem )
	{
		indexOfList = m_ItemList.FindSame(nTargetID);
		if (0 == indexOfList)
		{
			return 0;
		}

		int targetCompoundLevelupType = Item[nTargetID].GetLevelupType();
		int targetCompoundLevelupTimes = Item[nTargetID].GetLevelupTimes();
		int targetCompoundLevelupBuff = Item[nTargetID].GetCompoundBuff( COMPOUND_LEVELUP );
		
		ConfigManager& cm = ConfigManager::Singleton();
		int leftLevelupTooLimit = 0;
		int rightLevelupTooLimit = 0;
		char szMsg[COMMON_CLIENT_MSG_LEN_128];
		cm.GetItemLevelupToolLimit( Item[nItemID].GetLevel(), leftLevelupTooLimit, rightLevelupTooLimit );
		if ( Item[nItemID].GetLevelupTimes() <= 0 && Item[nItemID].GetCompoundBuff(COMPOUND_LEVELUP) <= 0 && Item[nItemID].GetLevelupType() < 0 )
		{			
			if ( targetCompoundLevelupType < 0 ||
				targetCompoundLevelupTimes <= 0 ||
				targetCompoundLevelupBuff <= 0 )
			{
				ShowChatErrorMsg(g_GetStringRes(10128,szMsg,sizeof(szMsg)));
				return FALSE;
			}

			if ( !(targetCompoundLevelupTimes >= leftLevelupTooLimit && targetCompoundLevelupTimes <= rightLevelupTooLimit ) )
			{
				ShowChatErrorMsg(g_GetStringRes(10131,szMsg,sizeof(szMsg)));
				return FALSE;
			}
		}
		else
		{
			if ( Item[nItemID].GetLevelupTimes() <= targetCompoundLevelupTimes  )
			{
				ShowChatErrorMsg(g_GetStringRes(10130,szMsg,sizeof(szMsg)));
				return  FALSE;
			}							
		}		
	}
	
	if (nRet == REQUEST_EQUIP_ITEM)
	{
		//TODO: 要恢复自动装备功能，要取消下列几行代码的注释。
		ItemPos destPos;
		destPos.nPlace = pos_equip;
		destPos.nX     = itempart_unidentified;
		destPos.nY     = 0;
		clientSendMoveItemCmd(SrcPos, destPos);
	}
	else if (nRet == REQUEST_EAT_MEDICINE)
	{
		PLAYER_EAT_ITEM_COMMAND	sEat;
		sEat.ProtocolType = c2s_playereatitem;
		sEat.m_nItemID = nItemID;
		sEat.m_btPlace = SrcPos.nPlace;
		sEat.m_btX = SrcPos.nX;
		sEat.m_btY = SrcPos.nY;

		sEat.m_nTargetItemID = nTargetID;
		sEat.m_btTargetPlace = TargetPos.nPlace;
		sEat.m_btTargetX = TargetPos.nX;
		sEat.m_btTargetY = TargetPos.nY;
		if (g_pClient)
			g_pClient->SendPackToServer(g_ConnectID,&sEat, sizeof(PLAYER_EAT_ITEM_COMMAND));
	}
	
	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：客户端鼠标点击obj检起某个物品，向服务器发消息
//-------------------------------------------------------------------------
void KPlayer::PickUpObj(int nObjIndex)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	if (Object[nObjIndex].m_nKind != Obj_Kind_Item && Object[nObjIndex].m_nKind != Obj_Kind_Money)
		return;
	
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp;
	if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
	{
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosX = 0;
		sPickUp.m_btPosY = 0;
	}
	else
	{		
		if (!CanPickup())
		{
			char msgBuff[256] = { 0 };
			g_GetStringRes(sid_can_not_pickup_item, msgBuff, 256);
			if (msgBuff[0] != 0)
				CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)msgBuff, 0);

			return;
		}

		ItemPos	sItemPos;
		if ( FALSE == m_ItemList.SearchPosition(&sItemPos, NULL) )
		{
			char msgBuff[256] = { 0 };
			g_GetStringRes(sid_bag_is_full, msgBuff, 256);
			if (msgBuff[0] != 0)
				CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)msgBuff, 0);
			return;
		}
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosX = sItemPos.nX;
		sPickUp.m_btPosY = sItemPos.nY;
	}
	
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：客户端鼠标点击obj，向服务器发消息
//-------------------------------------------------------------------------
void	KPlayer::ObjMouseClick(int nObjIndex)
{
	if (this->CheckTrading())
		return;
	if (nObjIndex <= 0)
		return;
	if (Object[nObjIndex].m_nKind != Obj_Kind_Box && Object[nObjIndex].m_nKind != Obj_Kind_Prop &&
	//--> Rocker 2005/05/12
		Object[nObjIndex].m_nKind != Obj_Kind_Furniture && Object[nObjIndex].m_nKind != Obj_Kind_House_Entry)
	//<-- End
		return;
	SendObjMouseClick(Object[nObjIndex].m_nID, SubWorld[0].m_Region[Object[nObjIndex].m_nRegionIdx].m_RegionID);
}
#endif

#ifndef _SERVER
// DownPos 是面板上的物品的当前坐标，UpPos 必须是手上物品放到面板上的坐标
void	KPlayer::MoveItem(ItemPos sourPos, ItemPos destPos, int moveItemCount)
{
	if(sourPos.nPlace == pos_equip || destPos.nPlace == pos_equip)
	{	//如果源端或者宿端是装备栏肯定是移动物品
		clientSendMoveItemCmd(sourPos, destPos);
		return;
	}
	INVENTORY_ROOM room = KItemList::corePos2coreRoom((ITEM_POSITION)sourPos.nPlace);
	int sourIndex = m_ItemList.m_Room[room].FindItem(sourPos.nX, sourPos.nY);
	int itemCount = Item[sourIndex].GetItemCount();
	if(moveItemCount > 0 && moveItemCount < itemCount)
	{
		clientSendSplitItemCmd(sourPos, destPos, moveItemCount);
	}
	else
	{
		clientSendMoveItemCmd(sourPos, destPos);
	}
}
#endif

#ifndef _SERVER
BOOL KPlayer::GetThrowAwayItemPermit()
{
	return m_bEnableToThrowAwayItem;
}
void KPlayer::EnableToThrowAwayItem(BOOL bEnable)
{
	KSystemMessage msg;	
	
	msg.eType = SMT_NORMAL;
	msg.byConfirmType = SMCT_NONE;
	msg.byPriority = 0;
	msg.byParamSize = 0;
	
	if ( bEnable != FALSE )	//关闭物品丢弃保护,可以丢弃物品
	{
		strcpy(msg.szMessage, MSG_THROW_AWAY_ITEM_ENABLE);		
		m_bEnableToThrowAwayItem = TRUE;		
	}
	else	//打开物品丢弃保护,不能丢弃物品
	{
		strcpy(msg.szMessage, MSG_THROW_AWAY_ITEM_DISABLE);
		m_bEnableToThrowAwayItem = FALSE;		
	}
//	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&msg, 0);
}
int	KPlayer::ThrowAwayItem( int nItemID )
{
	PLAYER_THROW_AWAY_ITEM_COMMAND	sThrow;
	
	sThrow.ProtocolType = c2s_playerthrowawayitem;
	sThrow.nItemID		= nItemID;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&sThrow, sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND));
	
	return 1;
}
#endif

// #ifndef _SERVER
// void	KPlayer::ChatAddFriend(int nPlayerIdx)
// {
// 	CHAT_ADD_FRIEND_COMMAND	sAdd;
// 	sAdd.ProtocolType = c2s_chataddfriend;
// 	sAdd.m_nTargetPlayerIdx = nPlayerIdx;
// 	if (g_pClient)
// 		g_pClient->SendPackToServer(g_ConnectID,&sAdd, sizeof(CHAT_ADD_FRIEND_COMMAND));
// }
// #endif

// #ifndef _SERVER
// void	KPlayer::ChatRefuseFriend(int nPlayerIdx)
// {
// 	CHAT_REFUSE_FRIEND_COMMAND	sRefuse;
// 	sRefuse.ProtocolType = c2s_chatrefusefriend;
// 	sRefuse.m_nTargetPlayerIdx = nPlayerIdx;
// 	if (g_pClient)
// 		g_pClient->SendPackToServer(g_ConnectID,&sRefuse, sizeof(CHAT_REFUSE_FRIEND_COMMAND));
// }
// #endif

#define MAX_ORDINSKILL_LEVEL_ALWAYS  20//不包括其它情况对技能等级的变动之外的，一般最大技能等级
#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端要求增加某个技能的点数
//-------------------------------------------------------------------------

#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：服务器端拣物品
//-------------------------------------------------------------------------
void KPlayer::ServerPickUpItem(BYTE* pProtocol)
{
	PLAYER_PICKUP_ITEM_COMMAND	*pPickUp = (PLAYER_PICKUP_ITEM_COMMAND*)pProtocol;
	int pickupObjectId = pPickUp->m_nObjID;

	int checkResult = CheckPickupObject(pickupObjectId);
	if (TRUE == checkResult)
	{
		int	pickupObjectIndex = ObjSet.FindID(pickupObjectId);
		if (pickupObjectIndex > 0)
		{
			KObj& pickupObject = Object[pickupObjectIndex];
			int pickupTime = pickupObject.GetPickupTime();
			if (pickupTime >= 0)
			{
				DelayedAction pickupObjectAction(m_nPlayerIndex, delayed_action_pickup_item, pickupTime, delayed_action_msg_pickup_object);
				DelayedActionParamPickupObject& param = pickupObjectAction.GetPickupObjectParam();
				param.m_PickupObjectId = pickupObjectId;
				param.m_PickupObjectPosX = pPickUp->m_btPosX;
				param.m_PickupObjectPosY = pPickUp->m_btPosY;
				m_ActionDelayer.NewAction(pickupObjectAction);
			}
		}
	}
}

//BOOL KPlayer::ServerPickUpItem(BYTE* pProtocol)
//{
//	PLAYER_PICKUP_ITEM_COMMAND	*pPickUp = (PLAYER_PICKUP_ITEM_COMMAND*)pProtocol;
// 		//Add 2005-2-2
// 		if(Npc[m_nIndex].m_Doing == do_revive || 
// 			Npc[m_nIndex].m_Doing == do_death)
// 		{
// 			return FALSE;
// 		}
// 	
// 		if(!CanPickup())
// 		{
// 			return FALSE;
// 		}
// 		
// 		int		nObjIndex, nNpcX, nNpcY, nObjX, nObjY;
// 		nObjIndex = ObjSet.FindID(pPickUp->m_nObjID);
// 		if (nObjIndex == 0)
// 			return FALSE;
// 		
// 		if (Object[nObjIndex].m_nBelong != -1)
// 		{
// 			if (!m_cTeam.m_nFlag)
// 			{
// 				if (Object[nObjIndex].m_nBelong != m_nPlayerIndex)
// 				{
// 					SHOW_MSG_SYNC	sMsg;
// 					sMsg.ProtocolType = s2c_msgshow;
// 					if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
// 						sMsg.m_wMsgID = enumMSG_ID_MONEY_CANNOT_PICKUP;
// 					else
// 						sMsg.m_wMsgID = enumMSG_ID_OBJ_CANNOT_PICKUP;
// 					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
// 					g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
// 					return FALSE;
// 				}
// 			}
// 			else
// 			{
// 				if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
// 				{
// 					if (Object[nObjIndex].m_nBelong != m_nPlayerIndex &&
// 						!g_TeamS[m_cTeam.m_nID].CheckIn(Object[nObjIndex].m_nBelong))
// 					{
// 						SHOW_MSG_SYNC	sMsg;
// 						sMsg.ProtocolType = s2c_msgshow;
// 						sMsg.m_wMsgID = enumMSG_ID_MONEY_CANNOT_PICKUP;
// 						sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
// 						g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
// 						return FALSE;
// 					}
// 				}
// 				else if (Object[nObjIndex].m_nKind == Obj_Kind_Item)
// 				{
// 					if (Object[nObjIndex].m_nItemDataID <= 0 || Object[nObjIndex].m_nItemDataID >= MAX_ITEM)
// 					{
// 						_ASSERT(0);
// 						return FALSE;
// 					}
// 					if ((Item[Object[nObjIndex].m_nItemDataID].GetGenre() == item_task && Object[nObjIndex].m_nBelong != m_nPlayerIndex) ||
// 						!g_TeamS[m_cTeam.m_nID].CheckIn(Object[nObjIndex].m_nBelong) || 
// 						// --> Rocker Edit Start 2005/11/21 加上能否丢弃的判断
// 						!Item[Object[nObjIndex].m_nItemDataID].CanDiscard())
// 						// <-- Rocker End
// 					{
// 						SHOW_MSG_SYNC	sMsg;
// 						sMsg.ProtocolType = s2c_msgshow;
// 						sMsg.m_wMsgID = enumMSG_ID_OBJ_CANNOT_PICKUP;
// 						sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
// 						g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
// 						return FALSE;
// 					}
// 				}
// 				else
// 				{
// 					return FALSE;
// 				}
// 			}
// 		}
// 		// 判断距离
// 		if (Object[nObjIndex].m_nSubWorldID != Npc[m_nIndex].m_SubWorldIndex)
// 			return FALSE;
// 		SubWorld[Object[nObjIndex].m_nSubWorldID].Map2Mps(
// 			Object[nObjIndex].m_nRegionIdx,
// 			Object[nObjIndex].m_nMapX,
// 			Object[nObjIndex].m_nMapY,
// 			Object[nObjIndex].m_nOffX,
// 			Object[nObjIndex].m_nOffY,
// 			&nObjX,
// 			&nObjY);
// 		SubWorld[Npc[m_nIndex].m_SubWorldIndex].Map2Mps(
// 			Npc[m_nIndex].m_RegionIndex,
// 			Npc[m_nIndex].GetMapX(),
// 			Npc[m_nIndex].GetMapY(),
// 			Npc[m_nIndex].GetOffX(),
// 			Npc[m_nIndex].GetOffY(),
// 			&nNpcX,
// 			&nNpcY);
// 		if ( PLAYER_PICKUP_SERVER_DISTANCE < (nNpcX - nObjX) * (nNpcX - nObjX) + (nNpcY - nObjY) * (nNpcY - nObjY))
// 		{
// 			SHOW_MSG_SYNC	sMsg;
// 			sMsg.ProtocolType = s2c_msgshow;
// 			sMsg.m_wMsgID = enumMSG_ID_OBJ_TOO_FAR;
// 			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
// 			g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
// 			return FALSE;
// 		}
// 	
// 	switch (Object[nObjIndex].m_nKind)
// 	{
// 	case Obj_Kind_Item:				// 掉在地上的装备
// 		{
// 			KItem& pickupItem = Item[Object[nObjIndex].m_nItemDataID];
// 			LogEventParam pickUpItemEventParam;
// 			bool needLog = pickupItem.GetLogLevel() >= ConfigManager::Singleton().GetConfigurableLogParam(log_param_item_log_level);			
// 			if (needLog)
// 			{
// 				pickUpItemEventParam.event = log_event_pickup_item;
// 				pickUpItemEventParam.param1 = GetGUID();
// 				pickUpItemEventParam.param2 = pickupItem.GetGUID();
// 				sprintf(pickUpItemEventParam.comment, LOG_EVENT_PICKUP_ITEM_COMMENT, GetPlayerName(), pickupItem.GetName());
// 			}			
// 			
// 			int nItemIdx;
// 			if(Item[Object[nObjIndex].m_nItemDataID].GetMaxItemCount() > 0)
// 			{
// 				ItemPos tagPos;
// 				EXTRAINFOPLUS tagExtraPlus;
// 				tagExtraPlus.nItemGenre = Item[Object[nObjIndex].m_nItemDataID].GetGenre();
// 				tagExtraPlus.nParticularType = Item[Object[nObjIndex].m_nItemDataID].GetParticular();
// 				tagExtraPlus.nDetailType = Item[Object[nObjIndex].m_nItemDataID].GetDetailType();
// 				tagExtraPlus.nMaxItem = Item[Object[nObjIndex].m_nItemDataID].GetMaxItemCount();
// 				tagExtraPlus.nCurItem = Item[Object[nObjIndex].m_nItemDataID].GetItemCount();
// 				if(FALSE == m_ItemList.SearchPosition(&tagPos, &tagExtraPlus))
// 				{
// 					return FALSE;
// 				}
// 				pPickUp->m_btPosType = tagPos.nPlace;
// 				pPickUp->m_btPosX = tagPos.nX;
// 				pPickUp->m_btPosY = tagPos.nY;
// 			}
// 			int nRetIndex = 0;
// 
// 			nItemIdx = m_ItemList.Add(Object[nObjIndex].m_nItemDataID, pPickUp->m_btPosType, pPickUp->m_btPosX, pPickUp->m_btPosY, &nRetIndex);
// 			
// 			if (nItemIdx <= 0 || nItemIdx >= MAX_PLAYER_ITEM)
// 				return FALSE;
// 			if (Object[nObjIndex].m_nItemDataID <= 0 || Object[nObjIndex].m_nItemDataID >= MAX_ITEM)
// 				return FALSE;
// 
// 			//拾取物品日志
// 			if (needLog)
// 			{
// 				g_pLogSystem->Log(pickUpItemEventParam);
// 			}			
// 
// 			// 给客户端发送获得装备的消息
// 			SHOW_MSG_SYNC	sMsg;
// 			sMsg.ProtocolType = s2c_msgshow;
// 			sMsg.m_wMsgID = enumMSG_ID_GET_ITEM;
// 			//sMsg.m_lpBuf = (LPVOID)Item[Object[nObjIndex].m_nItemDataID].GetID();
// 			sMsg.m_lpBuf = (LPVOID)Item[nRetIndex].GetID();
// 			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
// 			g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
// 			sMsg.m_lpBuf = 0;
// 			
// 			// 去掉Object[nObjIndex]与道具的关联。避免ItemSet的Remove被Object的Remove调用
// 			Object[nObjIndex].m_nItemDataID = 0;
// 			Object[nObjIndex].Remove(FALSE);
// 			
// 		}
// 		break;
// 	case Obj_Kind_Money:			// 掉在地上的钱
// 		if ( !Earn(Object[nObjIndex].m_nMoneyNum) )
// 			return FALSE;
// 
// 		Object[nObjIndex].SyncRemove(TRUE);
// 		if (Object[nObjIndex].m_nRegionIdx >= 0)
// 			SubWorld[Object[nObjIndex].m_nSubWorldID].m_Region[Object[nObjIndex].m_nRegionIdx].RemoveObj(nObjIndex);
// 		ObjSet.Remove(nObjIndex);
// 		break;
// 	}
// 	
// 	return TRUE;
//}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端要求使用某个物品(鼠标右键点击)
//-------------------------------------------------------------------------
void	KPlayer::EatItem(BYTE* pProtocol)
{
	PLAYER_EAT_ITEM_COMMAND	*pEat = (PLAYER_EAT_ITEM_COMMAND*)pProtocol;
	
	m_ItemList.EatMecidine(pEat->m_btPlace, pEat->m_btX, pEat->m_btY, pEat->m_nTargetItemID, pEat->m_btTargetPlace, pEat->m_btTargetX, pEat->m_btTargetY);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：收到客户端要求使用某个物品(鼠标右键点击)
//-------------------------------------------------------------------------
//void	KPlayer::UseItem(BYTE* pProtocol)
//{
//	PLAYER_USE_ITEM_COMMAND	*pItem = (PLAYER_USE_ITEM_COMMAND*)pProtocol;
//}
#endif

#ifdef _SERVER
void	KPlayer::ServerMoveItem(BYTE* pProtocol)
{
	PLAYER_MOVE_ITEM_COMMAND	*pMove = (PLAYER_MOVE_ITEM_COMMAND*)pProtocol;
	ItemPos		sourPos, destPos;
	
	sourPos.nPlace = pMove->sourPlace;
	sourPos.nX = pMove->sourX;
	sourPos.nY = pMove->sourY;
	destPos.nPlace = pMove->destPlace;
	destPos.nX = pMove->destX;
	destPos.nY = pMove->destY;

	m_ItemList.exchangeItem(&sourPos, &destPos);

//	BYTE	byFinished = s2c_itemexchangefinish;
//	if (g_pServer)
//		g_pServer->PackDataToClient(m_nNetConnectIdx, &byFinished, sizeof(BYTE));

}
#endif

#ifdef _SERVER
void	KPlayer::ServerThrowAwayItem(BYTE* pProtocol)
{
	PLAYER_THROW_AWAY_ITEM_COMMAND	*pThrow = (PLAYER_THROW_AWAY_ITEM_COMMAND*)pProtocol;
	if ( pThrow )
	{
		int nItemIdx = ItemSet.SearchID( pThrow->nItemID );
		if ( nItemIdx > 0 && m_ItemList.SearchID( pThrow->nItemID) > 0 )
		{
			ItemPos pos;
			if (!GetItemList().GetItemPos(nItemIdx,&pos))
				return ;
			
			if (pos.nPlace != pos_equiproom )
				return ;
			
			KItem& destroyItem = Item[nItemIdx];
			if (!destroyItem.IsLocked(GetNetConnectIdx()) && destroyItem.CanDiscard())
			{
				if ( m_ItemList.Remove( nItemIdx ) )
				{
					//统计：销毁物品
					ItemTemplateId templateId;
					destroyItem.GetItemTemplateId(templateId);
					GetPlayerStatistic().AddItem(templateId, destroyItem.GetItemCount(), item_count_type_destroy);
					
					//日志：销毁物品
					bool needLog = (destroyItem.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
					if (needLog)
					{
						LogEventParam destroyItemEvent;
						destroyItemEvent.event = log_event_destroy_item;
						destroyItemEvent.param1 = GetGUID();
						destroyItemEvent.param2 = destroyItem.GetGUID();
						destroyItem.GetItemTemplateId(destroyItemEvent.param3.data, sizeof(destroyItemEvent.param3.data) - 1);						
						g_pLogSystem->Log(destroyItemEvent);
					}

					if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_destroy_item))
					{
						RecordPlayerActionParam param;
						param.PlayerIndex = GetPlayerIndex();
						param.Action = player_action_destroy_item;
						snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_DESTROY_ITEM, destroyItem.GetName());
						g_PlayerMonitor.RecordPlayerAction(param);
					}

					ItemSet.Remove( nItemIdx );
				}
			}			
		}
	}
		
}
#endif

//-------------------------------------------------------------------------
//	功能：发送自己装备在身上的装备信息给别人看
//-------------------------------------------------------------------------
#ifdef _SERVER
#define EQUIP_ITEM_INFO_BUFF 4000
void	KPlayer::SendEquipItemInfo(int nTargetPlayer)
{
	if (nTargetPlayer <= 0 || nTargetPlayer >= MAX_PLAYER || Player[nTargetPlayer].m_nIndex <= 0)
		return;
	
	char Buff[EQUIP_ITEM_INFO_BUFF];
	VIEW_EQUIP_SYNC       * pView = (VIEW_EQUIP_SYNC *) Buff;
	pView->Protocol               = s2c_viewequip;
	pView->Len                    = sizeof(VIEW_EQUIP_SYNC) - 1 + sizeof(VIEW_EQUIP_SYNC_INFO) - PROTOCOL_SIZE;
	
	VIEW_EQUIP_SYNC_INFO  * sView = (VIEW_EQUIP_SYNC_INFO  * )pView->data;
	
	sView->shizu[0] = 0;
	sView->zhuhou[0] = 0;
	//氏族、诸侯, （加上联盟）
	SocialUnit	*pLeafUnit = GetLeafUnit(Npc[m_nIndex].GetPlayerIdx(), enSUTplId_Tong);
	if(pLeafUnit)
	{
		//氏族
		SocialUnit *pOrgUnit = GetUpNUnit(pLeafUnit, enSULayer_Gens);
		if(pOrgUnit)
		{
			const char *szOrgName = GetUnitName(pOrgUnit->GetUnitAttr());
			if(szOrgName != NULL)
			{
				strncpy(sView->shizu, szOrgName, MAXSIZE_ORGNAME);
				sView->shizu[MAXSIZE_ORGNAME - 1] = 0;
			}
		}

		pOrgUnit = GetUpNUnit(pLeafUnit, enSULayer_Tong);
		if(pOrgUnit)
		{
			const char *szOrgName = GetUnitName(pOrgUnit->GetUnitAttr());
			if(szOrgName != NULL)
			{
				strncpy(sView->zhuhou, szOrgName, MAXSIZE_ORGNAME);
				sView->zhuhou[MAXSIZE_ORGNAME - 1] = 0;
			}
		}

		pOrgUnit = GetUpNUnit(pLeafUnit, enSULayer_League);
		if ( pOrgUnit )
		{
			const char* szOrgName = GetUnitName( pOrgUnit->GetUnitAttr() );

			if ( szOrgName != NULL )
			{
				strncpy(sView->lianmen, szOrgName, MAXSIZE_ORGNAME);
				sView->lianmen[MAXSIZE_ORGNAME - 1] = 0;
			}
		}
	}

	sView->chenghao[0] = 0;
	sView->npcId       = Npc[m_nIndex].m_dwID;

	//PK值
	sView->pkValue = GetPkValue();
	if(sView->pkValue > MAX_DEATH_PUNISH_PK_VALUE)
	{
		sView->pkValue = MAX_DEATH_PUNISH_PK_VALUE;
	}
	//等级
	sView->level = GetLevel();
	//声望（还没有）
	sView->shengwang = 0;

	//姓名
	strncpy(sView->name, m_PlayerName, MAXSIZE_ROLENAME);
	sView->name[MAXSIZE_ROLENAME - 1] = 0;

	//装备
	for(int i = 0; i < itempart_num; i++)
	{
		int index = m_ItemList.m_EquipItem[i].nEquipIdx;

		if (index <= 0)
		{
			memset(&sView->m_sInfo[i], 0, sizeof(SViewItemInfo));
			continue;
		}
		SViewItemInfo& equipInfo = sView->m_sInfo[i];

		//物品类型
		equipInfo.m_ID				= Item[index].GetID();													
		equipInfo.m_Genre			= Item[index].GetGenre();
		equipInfo.m_Detail			= Item[index].GetDetailType();
		equipInfo.m_Particur		= Item[index].GetParticular();
		equipInfo.m_Level			= Item[index].GetLevel();
		//耐久
		equipInfo.m_Durability		= Item[index].GetDurability();
		equipInfo.m_MaxDurability	= Item[index].GetMaxDurability();
		//数量
		equipInfo.m_btItemCount	= (unsigned short)Item[index].GetItemCount();
		//升级、爻信息
		equipInfo.m_nLevelupType	= Item[index].GetLevelupType();
		equipInfo.m_LevelupTimes	= Item[index].GetLevelupTimes();
		equipInfo.m_YaoID			= (BYTE)Item[index].GetYaoID();
		equipInfo.m_bExchange		= !Item[index].IsBind();
		
		for (int yaoAddOnBuffLoopCount = 0; yaoAddOnBuffLoopCount < YAO_ADDON_BUFF_COUNT; yaoAddOnBuffLoopCount++)
		{
			equipInfo.m_YaoAddOnBuffSet[yaoAddOnBuffLoopCount] = Item[index].GetYaoAddOn(yaoAddOnBuffLoopCount);
		}
		memcpy( equipInfo.m_compBuffTemplateSet, Item[index].GetCompBuffTemplateSet(), sizeof(WORD) * COMPOUND_COUNT ); 
		memcpy( equipInfo.m_szPlusInfo, Item[index].getPlusInfo(), ITEM_PLUS_INFO_LEN ); 
		equipInfo.m_TalismanPotential = Item[index].GetTalismanPotential();
		for (int talismanEnchaseLoopCount = 0; talismanEnchaseLoopCount < TM_HOLE_NUM; talismanEnchaseLoopCount++)
		{
			equipInfo.m_TalismanEnchaseSet[talismanEnchaseLoopCount] = Item[index].GetTalismanEnchase(talismanEnchaseLoopCount);
		}

		int useMaxCount			= Item[index].GetMaxSocketCount();
		for ( int nIdx = 0; nIdx < MAX_INLAY_COUNT; ++nIdx )
		{
			InlayStuff stuff;
			
			Item[index].GetInlayStuffBySocketIdx( nIdx, stuff );

			if ( nIdx < useMaxCount )
			{
				equipInfo.m_socketSet[nIdx].nGenre		= stuff.nGenre;
				equipInfo.m_socketSet[nIdx].nDetail		= stuff.nDetail;
				equipInfo.m_socketSet[nIdx].nParticular	= stuff.nParticular;
				equipInfo.m_socketSet[nIdx].nLevel		= stuff.nLevel;
			}
			else
			{
				equipInfo.m_socketSet[nIdx].nGenre		= -1;
				equipInfo.m_socketSet[nIdx].nDetail		= -1;
				equipInfo.m_socketSet[nIdx].nParticular	= -1;
				equipInfo.m_socketSet[nIdx].nLevel		= -1;
			}
		}
		memcpy( &equipInfo.m_InlayBaseBuffSet, Item[index].GetInlayBaseBuffSet(), sizeof(equipInfo.m_InlayBaseBuffSet) );
		memcpy( &equipInfo.m_InlayYaoBuffSet, Item[index].GetInlayYaoBuffSet(), sizeof(equipInfo.m_InlayYaoBuffSet) );
		memcpy( &equipInfo.m_InlaySpecialBuffSet, Item[index].GetInlaySpecialBuffSet(), sizeof(equipInfo.m_InlaySpecialBuffSet) );

	}
	
	//Compression begin ..........................................................
	unsigned char szBuff[EQUIP_ITEM_INFO_BUFF];
	unsigned int  nLen = EQUIP_ITEM_INFO_BUFF;
	lzo1x_1_compress( 
		pView->data,
		sizeof(VIEW_EQUIP_SYNC_INFO),
		szBuff,
		&nLen,
		wrkmem);
	
	if (nLen >= EQUIP_ITEM_INFO_BUFF - sizeof(VIEW_EQUIP_SYNC) )
		return;

	memcpy(pView->data,szBuff,nLen);
	pView->Len = sizeof(VIEW_EQUIP_SYNC) - 1 + nLen - PROTOCOL_SIZE;
	//Compression end ............................................................

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[nTargetPlayer].m_nNetConnectIdx, pView, pView->Len + PROTOCOL_SIZE);
}
#endif

bool KPlayer::ChongZhiLeftMoney( void )
{
#ifdef _SERVER 
	if ( g_pController )
	{
		DWORD dwCurChongZhiTime = UNIX_TMIE_STAMP;
		if ( dwCurChongZhiTime - m_lastChongZhiTime > 60 )
		{
			g_pController->ReLogin( GetNetConnectIdx() );
			m_lastChongZhiTime = dwCurChongZhiTime;
			return true;
		}		
	}
	return false;
#else
	return true;
#endif
}

#ifdef _SERVER

// uType = 0: 大银票
// uType = 1: 小银票
// uUseType = 0: 转换为点数
// uUseType = 1: 转为包（周）月

int KPlayer::UseSilver(unsigned uType, unsigned uUseType, unsigned uCount)
{
    if (
        (uType >= 3) ||
        (uUseType >= 3)
    )
        return FALSE;

    if (m_AccoutName[0] == '\0')
        return FALSE;

	const size_t uBufferSize = sizeof(KAccountUserChangeExtPoint) + 1;
	
	BYTE Buffer[sizeof(KAccountUserChangeExtPoint) + 1];
	Buffer[0] = l2p_account_change_extpoint;
	
	KAccountUserChangeExtPoint *pUser = (KAccountUserChangeExtPoint *)(Buffer + 1);
	
	pUser->Size = sizeof( KAccountUserChangeExtPoint );
	pUser->Type = AccountChangeExtPoint;
	pUser->Version = ACCOUNT_CURRENT_VERSION;
    pUser->Operate = m_nNetConnectIdx;
	
    pUser->uSilverType = ((uType & 0xffff) << 16) | (uUseType & 0xffff);
    pUser->nChangeValue = uCount;
    pUser->uFlag = CHANGE_EXT_POINT_SILVER;
    pUser->nPlayerIndex = m_nPlayerIndex;
	
    strncpy(pUser->Account, m_AccoutName, _NAME_LEN);
    pUser->Account[_NAME_LEN - 1] = '\0';
	
	g_pController->PushData( protocol_type_paysys, INVALID_VALUE, Buffer, uBufferSize );
	
/*  
	strcpy((char *)CEP.szAccountName, m_AccoutName);
    CEP.uSilverType = ((uType & 0xffff) << 16) | (uUseType & 0xffff);
    CEP.nChangeValue = uCount;    // 负数表示要扣点
    CEP.uFlag = CHANGE_EXT_POINT_SILVER;
    CEP.nPlayerIndex = m_nPlayerIndex;
*/

    return TRUE;
}

void KPlayer::SetExtPoint( const tagExtPointInfo& ExtPoint )
{
	memcpy( &m_ExtPointInfo, &ExtPoint, sizeof(tagExtPointInfo) );
}
void KPlayer::SetExtPoint(int nIndex, int nPoint)
{
    if (!
        ((nIndex >= 0) && (nIndex < MAX_EXT_POINT_COUNT))
		)
        return;
	
    m_ExtPointInfo.nExtPoint[nIndex] = nPoint;
}

BOOL KPlayer::SetExtPointImmediately(int nIndex, int nPoint)
{
	if ( !((nIndex >= 0) && (nIndex < MAX_EXT_POINT_COUNT)) )
        return FALSE;

	m_ExtPointInfo.nExtPoint[nIndex] = nPoint;
	
	const size_t uBufferSize = sizeof(KAccountUserChangeExtPoint) + 1;
	
	BYTE Buffer[sizeof(KAccountUserChangeExtPoint) + 1];
	Buffer[0] = l2p_account_change_extpoint;
	
	KAccountUserChangeExtPoint *pUser = (KAccountUserChangeExtPoint *)(Buffer + 1);
	
	pUser->Size    = sizeof( KAccountUserChangeExtPoint );
	pUser->Type    = AccountChangeExtPoint;
	pUser->Version = ACCOUNT_CURRENT_VERSION;
    pUser->Operate = m_nNetConnectIdx;
	
    pUser->uExtPointIndex = nIndex;
    pUser->nChangeValue   = nPoint;
    pUser->uFlag          = 0;
    pUser->nPlayerIndex   = m_nPlayerIndex;
	
    strncpy(pUser->Account, m_AccoutName, _NAME_LEN);
    pUser->Account[_NAME_LEN - 1] = '\0';
	
	g_pController->PushData( protocol_type_paysys, INVALID_VALUE, Buffer, uBufferSize );
	
	return TRUE;
}

int KPlayer::GetExtPoint(int nIndex)
{
    if (!
        ((nIndex >= 0) && (nIndex < MAX_EXT_POINT_COUNT))
		)
        return 0;
	
    return m_ExtPointInfo.nExtPoint[nIndex];
}

BOOL KPlayer::AddExtPoint(int nIndex, int nPoint)
{
	if ( !((nIndex >= 0) && (nIndex < MAX_EXT_POINT_COUNT)) )
        return FALSE;

	if( nPoint < 0 )
		return FALSE;
	
    m_ExtPointInfo.nExtPoint[nIndex] += nPoint;
	
	//
	const size_t uBufferSize = sizeof(KAccountUserChangeExtPoint) + 1;
	
	BYTE Buffer[sizeof(KAccountUserChangeExtPoint) + 1];
	Buffer[0] = l2p_account_change_extpoint;
	
	KAccountUserChangeExtPoint *pUser = (KAccountUserChangeExtPoint *)(Buffer + 1);
	
	pUser->Size = sizeof( KAccountUserChangeExtPoint );
	pUser->Type = AccountChangeExtPoint;
	pUser->Version = ACCOUNT_CURRENT_VERSION;
    pUser->Operate = m_nNetConnectIdx;
	
    pUser->uExtPointIndex = nIndex;
    pUser->nChangeValue = nPoint;
    pUser->uFlag = 0;
    pUser->nPlayerIndex = m_nPlayerIndex;
	
    strncpy(pUser->Account, m_AccoutName, _NAME_LEN);
    pUser->Account[_NAME_LEN - 1] = '\0';
	
	g_pController->PushData( protocol_type_paysys, INVALID_VALUE, Buffer, uBufferSize );
	
	return TRUE;
}

BOOL KPlayer::PayExtPoint(int nIndex, int nPoint)
{
    if ( !((nIndex >= 0) && (nIndex < MAX_EXT_POINT_COUNT)) )
        return FALSE;

	if( nPoint < 0 )
		return FALSE;
	
    if (m_ExtPointInfo.nExtPoint[nIndex] < nPoint)
        return FALSE;
	
    m_ExtPointInfo.nExtPoint[nIndex] -= nPoint;
	
	//
	const size_t uBufferSize = sizeof(KAccountUserChangeExtPoint) + 1;
	
	BYTE Buffer[sizeof(KAccountUserChangeExtPoint) + 1];
	Buffer[0] = l2p_account_change_extpoint;
	
	KAccountUserChangeExtPoint *pUser = (KAccountUserChangeExtPoint *)(Buffer + 1);
	
	pUser->Size = sizeof( KAccountUserChangeExtPoint );
	pUser->Type = AccountChangeExtPoint;
	pUser->Version = ACCOUNT_CURRENT_VERSION;
    pUser->Operate = m_nNetConnectIdx;
	
    pUser->uExtPointIndex = nIndex;
    pUser->nChangeValue = -nPoint;
    pUser->uFlag = 0;
    pUser->nPlayerIndex = m_nPlayerIndex;
	
    strncpy(pUser->Account, m_AccoutName, _NAME_LEN);
    pUser->Account[_NAME_LEN - 1] = '\0';
	
	g_pController->PushData( protocol_type_paysys, INVALID_VALUE, Buffer, uBufferSize );
	
	return TRUE;
}

int KPlayer::SetIBPoint( MoneyType type, long money )
{
	if ( money <= 0 )
	{
		return 0;
	}
	return m_moneyMgr.AddReq( type, &money );
}

int KPlayer::BuyIBItem(  IBBuy_Param ibbuy_param )
{
	KAccountBuyIBItem money;
	memset( &money, 0, sizeof(money) );


	strncpy(money.Account, m_AccoutName, _NAME_LEN);
	money.Account[_NAME_LEN - 1] = '\0';		
	
	money.Size				= sizeof( KAccountBuyIBItem );
	money.Type				= AccountIB_ItemBuy;
	money.Version			= ACCOUNT_CURRENT_VERSION;
	money.Operate			= m_nNetConnectIdx;
	money.nPlayerDataIndex	= m_nPlayerIndex;
	money.nGoodsIndex		= ibbuy_param.bOnceItem;

	money.nItemTypeID		= ibbuy_param.eMoneyType == jinshanbi ? ibbuy_param.nItemGenre : GenerateItemHashId( ibbuy_param.nItemGenre, ibbuy_param.nItemDetail, ibbuy_param.nItemParticular );
	money.nItemLevel		= ibbuy_param.eMoneyType == jinshanbi ? ibbuy_param.nItemDetail : ibbuy_param.nItemCount;
	money.nUseType			= ibbuy_param.eIBItemType;
	money.dwOverdueTime		= ibbuy_param.dwOverdueTime;
	money.nPrice			= ibbuy_param.nPrice;

	return m_moneyMgr.DecReq( ibbuy_param.eMoneyType, &money );
}

int KPlayer::BuyIBItem( )
{
	//
	const size_t uBufferSize = sizeof(KAccountBuyIBItem) + 1;
	
	BYTE Buffer[sizeof(KAccountBuyIBItem) + 1];
	Buffer[0] = l2p_ib_buy_item;
	
	KAccountBuyIBItem *pBuyIB = (KAccountBuyIBItem *)(Buffer + 1);
	
	pBuyIB->Size = sizeof( KAccountBuyIBItem );
	pBuyIB->Type = AccountIB_ItemBuy;
	pBuyIB->Version = ACCOUNT_CURRENT_VERSION;
    pBuyIB->Operate = m_nNetConnectIdx;
		
    strncpy(pBuyIB->Account, m_AccoutName, _NAME_LEN);
    pBuyIB->Account[_NAME_LEN - 1] = '\0';

	pBuyIB->nPlayerDataIndex = m_nPlayerIndex;
	pBuyIB->nGoodsIndex = 0;	//填写物品Index原包返回
	pBuyIB->nItemTypeID	= 0;	//物品类别
	pBuyIB->nItemLevel	= 1;	//物品等级
	pBuyIB->nUseType	= 0;	//使用类型	0：永久使用	1：在过期前可以任意使用	2：在过期前可以使用有限次	
	pBuyIB->nPrice		= 10;	//物品价格,自动扣除
	pBuyIB->dwOverdueTime = 1000;//过期时间
	
	g_pController->PushData( protocol_type_paysys, INVALID_VALUE, Buffer, uBufferSize );
	return TRUE;
}

int KPlayer::UseIBItem( )
{
	//
	const size_t uBufferSize = sizeof(KAccountBuyIBItem) + 1;
	
	BYTE Buffer[sizeof(KAccountBuyIBItem) + 1];
	Buffer[0] = l2p_ib_use_item;
	
	KAccountBuyIBItem *pUseIB = (KAccountBuyIBItem *)(Buffer + 1);
	
	pUseIB->Size = sizeof( KAccountBuyIBItem );
	pUseIB->Type = AccountIB_ItemUse;
	pUseIB->Version = ACCOUNT_CURRENT_VERSION;
    pUseIB->Operate = m_nNetConnectIdx;
	
    strncpy(pUseIB->Account, m_AccoutName, _NAME_LEN);
    pUseIB->Account[_NAME_LEN - 1] = '\0';
	pUseIB->nPlayerDataIndex = m_nPlayerIndex;
	pUseIB->nItemTypeID	= 0;	//物品类别
	pUseIB->nItemLevel	= 1;	//物品等级
	
	g_pController->PushData( protocol_type_paysys, INVALID_VALUE, Buffer, uBufferSize );
	return TRUE;
}

int KPlayer::UseIBItem( IBUse_Param ibuse_param )
{
	//
	const size_t uBufferSize = sizeof(KAccountUseIBItem) + 1;
	
	BYTE Buffer[sizeof(KAccountUseIBItem) + 1];
	Buffer[0] = l2p_ib_use_item;
	
	KAccountUseIBItem *pUseIB = (KAccountUseIBItem *)(Buffer + 1);

    strncpy(pUseIB->Account, m_AccoutName, _NAME_LEN);
    pUseIB->Account[_NAME_LEN - 1] = '\0';	
	pUseIB->Size				= sizeof( KAccountUseIBItem );
	pUseIB->Type				= AccountIB_ItemUse;
	pUseIB->Version				= ACCOUNT_CURRENT_VERSION;
    pUseIB->Operate				= m_nNetConnectIdx; 
	pUseIB->nPlayerDataIndex	= m_nPlayerIndex;
	pUseIB->nItemTypeID			= ibuse_param.nItemGenre;
	pUseIB->nItemLevel			= ibuse_param.nItemDetail;	//物品等级
	pUseIB->GUID				= ibuse_param.IBGuid;
	
	g_pController->PushData( protocol_type_paysys, INVALID_VALUE, Buffer, uBufferSize );
	return TRUE;
}

void KPlayer::ProcessPaysys( const char* pChar, int nSize )
{
	if ( pChar == NULL )
	{
		return;
	}
	
	switch( pChar[0] ) 
	{
	case p2l_accountlogin:
		{
			KAccountUserReturnExt* pRet = (KAccountUserReturnExt*)( pChar + 1 );
			if( pRet->nReturn == ACTION_SUCCESS )
			{
				if ( pRet->dwLeftMoney != GetJinshanbi() )
				{
					SetIBMoney( jinshanbi, &(pRet->dwLeftMoney) );
					SyncAttribute(attr_jinshanbi);
					IBCenter_S& ib_s =  IBCenter_S::Singleton();
					ib_s.ErrCodeToClient(GetPlayerIndex(), enIBShopErr_ChargeSucc, GetJinshanbi() );
				}
			}
			break;
		}
	case p2l_account_change_extpoint:
		{
			KAccountUserChangeExtPointRet* pRet = (KAccountUserChangeExtPointRet*)(pChar+1);
		}
		break;

	case p2l_activate_present:
		{
			KGameworldPaysysCommon * pRet   = (KGameworldPaysysCommon *)(pChar + 1);
			if (nSize >= sizeof(KGameworldPaysysCommon) - 1 + sizeof(KAccountActivePresentCodeRet) + 1 )
			{
				ProcessActivatePresentRet((KAccountActivePresentCodeRet *)pRet->byData);
			}//endif
		}
		break;

	case p2l_ib_buy_item:
		{
			KAccountBuyIBItemRet* pBuyRet = (KAccountBuyIBItemRet*)(pChar+1);
			if ( pBuyRet && nSize >= sizeof(KAccountBuyIBItemRet) + 1 )
			{				
				switch( pBuyRet->nResult )
				{
				case ACTION_SUCCESS:
				case S_IB_ITEM_NOT_IN_SAME_GATEWAY:
					{
						m_moneyMgr.DecRet( jinshanbi, pBuyRet );

						if ( strncmp( m_AccoutName, pBuyRet->Account, LOGIN_USER_ACCOUNT_MAX_LEN ) == 0 &&
							m_nPlayerIndex == pBuyRet->nPlayerDataIndex && pBuyRet->nPrice > 0 && pBuyRet->nItemTypeID == item_ib)
						{
							pBuyRet->nItemTypeID = GenerateItemHashId( pBuyRet->nItemTypeID, pBuyRet->nItemLevel, 0 );
							pBuyRet->nItemLevel = 1;
							{
								AddTotolJinshanbi( pBuyRet->nPrice );
								AddRecentJinshanbi( pBuyRet->nPrice );

								SaveIBData();
								m_moneyMgr.AddReq( point ,&(pBuyRet->nPrice));

								//新消费积分
								int newConsumePointIndex = ConfigManager::Singleton().GetGlobalVariable(globar_var_new_consume_point_index);
								if (newConsumePointIndex <= 0 || newConsumePointIndex > MAX_PLUS_POINT_COUNT)
								{
									newConsumePointIndex = DEFAULT_NEW_CONSUME_POINT_INDEX;
								}
								int newConsumePointRate = ConfigManager::Singleton().GetGlobalVariable(globar_var_new_consume_point_rate);
								if (newConsumePointRate < 0)
								{
									newConsumePointRate = 0;
								}
								if (pBuyRet->nPrice > 0)
								{
									DWORD addConsumePoint = (DWORD)(pBuyRet->nPrice * newConsumePointRate / 100);
									AddPlusPoint(newConsumePointIndex, addConsumePoint);
								}
							
								int nItemIdx = ItemSet.Add( pBuyRet->nItemTypeID, 0, 1 );
								if ( nItemIdx > 0 && nItemIdx < MAX_ITEM )
								{
									Item[nItemIdx].SetIBBuyDate(UNIX_TMIE_STAMP);
									if ( pBuyRet->nGoodsIndex > 0 )
									{
										KIBLog::getSingleton().AddIBItem( 
																jinshanbi_onetime_buy, 
																Item[nItemIdx].GetGUID(),
																GetPlayerIndex(), 
																pBuyRet->nItemTypeID, 
																pBuyRet->nItemLevel, 
																pBuyRet->nPrice );

										ConfigManager & mgr = ConfigManager::Singleton();
										int CreditIBGenera  = 0;
										int CreditDetail    = 0;
										int CreditParticular= 0;
										int CreditLevel     = 0;
										
										mgr.GetIBReturnId(&CreditIBGenera,&CreditDetail,&CreditParticular,&CreditLevel);
										int nHashId = GenerateItemHashId(CreditIBGenera,CreditDetail,CreditParticular);
										if (nHashId == pBuyRet->nItemTypeID )
										{
											int nCredit = JinshanbiToCredit(pBuyRet->nPrice);
											m_moneyMgr.AddReq(creditpoint,&nCredit);
											if ( Item[nItemIdx].NeedIBUse() )
											{
												IBUse_Param useParam;
												useParam.IBGuid = Item[nItemIdx].GetIBGuid();
												useParam.nItemGenre = Item[nItemIdx].GetGenre();
												useParam.nItemDetail = Item[nItemIdx].GetDetailType();
												useParam.nItemParticular = Item[nItemIdx].GetParticular();
												UseIBItem( useParam ); 
											}//endif

											KIBLog::getSingleton().DelIBItem( onetime_use_delete, Item[nItemIdx].GetGUID(), GetPlayerIndex() );
											ItemSet.Remove( nItemIdx );
											IBCenter_S& ib_s =  IBCenter_S::Singleton();
											ib_s.ErrCodeToClient(GetPlayerIndex(), enIBShopErr_OnceItemUseOk, nHashId );
										
										}
										else
										{
											if ( !Item[nItemIdx].UseItem( GetPlayerIndex(), 0, GetNpcIndex()) )
											{
												KIBLog::getSingleton().DelIBItem( buy_ok_add_no_delete, Item[nItemIdx].GetGUID(), GetPlayerIndex() );
											}		
											if ( Item[nItemIdx].NeedIBUse() )
											{
												IBUse_Param useParam;
												useParam.IBGuid = Item[nItemIdx].GetIBGuid();
												useParam.nItemGenre = Item[nItemIdx].GetGenre();
												useParam.nItemDetail = Item[nItemIdx].GetDetailType();
												useParam.nItemParticular = Item[nItemIdx].GetParticular();
												UseIBItem( useParam ); 
											}	
											
											//Insurance path......................................................
											ClientBuyGoods goods;
											memset(&goods,0,sizeof(goods));
											KOnceIBItemMgr::GetSingleten().GetOnceItemParam(insrance,goods);
											int nInsuranceHashId = GenerateItemHashId(goods.goods.Genera,goods.goods.Detail,goods.goods.Particular);
											if (nInsuranceHashId == pBuyRet->nItemTypeID )
											{
												m_InsuranceMgr.ProcessPaysysProtocol(pBuyRet->nPrice /100);
											}//endif

											KIBLog::getSingleton().DelIBItem( onetime_use_delete, Item[nItemIdx].GetGUID(), GetPlayerIndex() );
											ItemSet.Remove( nItemIdx );
											IBCenter_S& ib_s =  IBCenter_S::Singleton();
											ib_s.ErrCodeToClient(GetPlayerIndex(), enIBShopErr_OnceItemUseOk, pBuyRet->nItemTypeID );
										}
									}
									else
									{
										if ( m_ItemList.Add( nItemIdx ) == 0 )
										{
											KIBLog::getSingleton().DelIBItem( buy_ok_add_no_delete, Item[nItemIdx].GetGUID(), GetPlayerIndex() );
											
											//Log Failed Add To Player........
											LogEventParam buyEvent;
											buyEvent.event  = log_event_failed_add_ibitem_to_itemlist;
											buyEvent.param1 = GetGUID();
											snprintf(buyEvent.param3.data,sizeof(buyEvent.param3.data),"%d,%d,%d,%d",Item[nItemIdx].GetGenre(),Item[nItemIdx].GetDetailType(),Item[nItemIdx].GetParticular(),Item[nItemIdx].GetLevel());
											buyEvent.param4 = pBuyRet->nPrice;
											
											if (g_pLogSystem)
												g_pLogSystem->Log(buyEvent);
											//End Log...

											m_ItemList.Remove( nItemIdx );
											ItemSet.Remove( nItemIdx );
										}				
										else
										{	
											KIBLog::getSingleton().AddIBItem( 
																	jinshanbi_buy, 
																	Item[nItemIdx].GetGUID(),
																	GetPlayerIndex(), 
																	pBuyRet->nItemTypeID, 
																	pBuyRet->nItemLevel, 
																	pBuyRet->nPrice );		
											Item[nItemIdx].SetIBGuid( pBuyRet->GUID );
											Item[nItemIdx].SyncAttribute( item_attr_buytime, GetNetConnectIdx() );
											Item[nItemIdx].SetCreditFlag( jinshanbi );
											Item[nItemIdx].SyncAttribute( item_attr_credit_flag, GetNetConnectIdx() );
											
											//InstantSave
											//SaveItemData();

											int nHashId = GenerateItemHashId(Item[nItemIdx].GetGenre(), Item[nItemIdx].GetDetailType(), Item[nItemIdx].GetParticular());
											IBCenter_S& ib_s =  IBCenter_S::Singleton();
											ib_s.ErrCodeToClient(GetPlayerIndex(), enIBShopErr_OnceItemUseOk, nHashId );
										}
									}
								}
								else
								{
									//Log Failed Add To Player........
									int nGenre		= 0;
									int nDetail		= 0;
									int nParticular	= 0;
									int itemLevel   = 0;

									SpliteHashId( pBuyRet->nItemTypeID, nGenre, nDetail, nParticular );

									LogEventParam buyEvent;
									buyEvent.event  = log_event_failed_add_ibitem_to_itemlist;
									buyEvent.param1 = GetGUID();
									snprintf(buyEvent.param3.data,sizeof(buyEvent.param3.data),"%d,%d,%d,%d",nGenre,nDetail,nParticular,itemLevel);
									buyEvent.param4 = pBuyRet->nPrice;
									
									if (g_pLogSystem)
										g_pLogSystem->Log(buyEvent);
									//End Log...

									FSGUID fsGuid;
									if ( g_pController )
									{
										g_pController->GenGUID(fsGuid.data, g_GuidPadding);
									}
									
									if ( pBuyRet->nGoodsIndex > 0 )
									{
										KIBLog::getSingleton().AddIBItem( 
																jinshanbi_onetime_buy, 
																fsGuid,
																GetPlayerIndex(), 
																pBuyRet->nItemTypeID, 
																pBuyRet->nItemLevel, 
																pBuyRet->nPrice );
									}
									else
									{
										KIBLog::getSingleton().AddIBItem( 
																jinshanbi_buy, 
																fsGuid,
																GetPlayerIndex(), 
																pBuyRet->nItemTypeID, 
																pBuyRet->nItemLevel, 
																pBuyRet->nPrice );				
									}
								}

							}

						}
					}
					break;
				case E_PARAM_ERROR:
				case E_ZONE_ACCOUNT_ID_NOT_EXIST:
				case E_IB_NO_ENOUGH_COIN:
					m_moneyMgr.DecRet( jinshanbi, NULL );
				    break;
				default:
					m_moneyMgr.DecRet( jinshanbi, NULL );
				    break;
				}
			}
		}
		break;
	case p2l_ib_use_item:
		{
			KAccountUseIBItemRet* pUseRet = (KAccountUseIBItemRet*)(pChar+1);
			if ( pUseRet )
			{
				switch( pUseRet->nResult )
				{
				case ACTION_SUCCESS:
				case S_IB_ITEM_NOT_IN_SAME_GATEWAY:
					{
						//InstantSave
						//if ( strncmp( m_AccoutName, pUseRet->Account, LOGIN_USER_ACCOUNT_MAX_LEN ) == 0 &&
						//	m_nPlayerIndex == pUseRet->nPlayerDataIndex )
						//{	
						//	if( !SaveItemData( ) )
						//		CFS_FILELOGS::WriteLog("%s IB Save Failed!\n", Npc[m_nIndex].Name);
						//}
					}
					break;
				case E_PARAM_ERROR:
				case E_IB_ITEM_NOT_EXIST:		
				case E_IB_ITEM_HAS_BEEN_USED:	
				case E_IB_ITEM_EXPIRED:		
					break;
				default:
				    break;
				}
			}
		}
		break;
	default:
		break;
	}
}

#endif

void KPlayer::CheckCreditState()
{
#ifdef _SERVER
	ConfigManager& cm = ConfigManager::Singleton();
	int nLevel = cm.GetIBGlobalVariable( ib_global_var_credit_level_limit );
	DWORD dwDefaultCreditPoint = cm.GetIBGlobalVariable( ib_global_var_creditpoint_default );
	if ( GetLevel() >= nLevel &&
		nLevel != 0 &&
		GetCreditState() == disable )
	{
		SetCreditState( good );
		SetCreditReturnTime( GetFirstNewReturnDate(UNIX_TMIE_STAMP));
		SyncAttribute( attr_creditreturndata );
		long zero = 0;
		SetIBMoneySize( creditpoint, LONG_MIN_LIMIT, dwDefaultCreditPoint );
		SetIBMoney( creditpoint, &zero );
		SaveIBData();
	}		

	int min = 0;
	int max = 0;
	m_moneyMgr.GetMoneySize( creditpoint, min, max );

	if ( GetCreditState() == good && 
		UNIX_TMIE_STAMP >= GetCreditReturnTime() &&
		m_moneyMgr.GetMoney( creditpoint ) > 0 )
	{
		SetCreditState( bad );
		SaveIBData();
	}


	if ( GetCreditState() == bad && 
		 m_moneyMgr.GetMoney( creditpoint ) <= 0 )
	{
		SetCreditState( good );
		SaveIBData();
	}

	if ( UNIX_TMIE_STAMP >= GetCreditReturnTime() &&  
		GetCreditState() != disable )
	{
		SetCreditReturnTime( GetNewReturnDate(UNIX_TMIE_STAMP));
		SyncAttribute( attr_creditreturndata );
		SaveIBData();
	}

#endif
}

#ifdef _SERVER
//-------------------------------------------------------------------------
//	功能：主角死后重生
//-------------------------------------------------------------------------
void KPlayer::Revive(int nType)
{
	// 加上do_death == Npc[m_nIndex].m_Doing的原因是，如果客户端点击重生按钮比较
	// 快，碰巧服务器端执行比较慢，此时服务器端的状态很有可能是do_death
	// 就会出现重生不了
	if (Npc[m_nIndex].m_Doing == do_revive || do_death == Npc[m_nIndex].m_Doing) 
	{
		SetReviveFlag(FALSE);

		int	nSubWorldID = 0;
		int nMpsX = 0, nMpsY = 0;
		
		NPC_REVIVE_SYNC	Sync;
		Sync.ProtocolType = s2c_playerrevive;
		Sync.ID = Npc[m_nIndex].m_dwID;
		Sync.Type = nType;
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(m_nNetConnectIdx, (const void*)&Sync, sizeof(NPC_REVIVE_SYNC));
		Npc[m_nIndex].BroadCastRevive(nType);

		GetDeathRevivalPos(&nSubWorldID, &nMpsX, &nMpsY);
		
		if( nType )
		{
			int instanceId = INVALID_INSTANCE_ID;
			int subworld = Npc[m_nIndex].GetSubWorldIndex();
			if (subworld >= 0 && nSubWorldID == SubWorld[subworld].m_SubWorldID)
			{
				instanceId = SubWorld[subworld].GetInstanceId();
			}
			
			if (INVALID_INSTANCE_ID == instanceId)
			{
				Npc[m_nIndex].ChangeWorld(nSubWorldID, nMpsX, nMpsY);
			}
			else
			{
				Npc[m_nIndex].ChangeWorld(instanceId, nMpsX, nMpsY, true);
			}
		}
		
		//<-- End
		
//		Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_curlife, Npc[m_nIndex].m_CompAttrMgr[ncai_lifeuplimit]);
//		Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_curmana, Npc[m_nIndex].m_CompAttrMgr[ncai_manauplimit]);

		Npc[m_nIndex].SendCommand(do_revive);

		if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_revive))
		{ 
			RecordPlayerActionParam param;
			param.PlayerIndex = GetPlayerIndex();
			param.Action = player_action_revive;
			snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_REVIVE);
			g_PlayerMonitor.RecordPlayerAction(param);
		}

		Npc[m_nIndex].SetupEventBuff(NpcEvent_Revive);
	}
}


void KPlayer::RestoreLiveData()
{
	Npc[m_nIndex].RestoreNpcBaseInfo();
}

BOOL KPlayer::Pay(int nMoney, bool statisticFlag)
{
	if (nMoney < 0)
		return FALSE;

	BOOL result = m_ItemList.AddMoney(room_equipment, -nMoney);
	if (TRUE == result)
	{
		m_PlayerStatistic.ChangeMoney(-nMoney);
		if (statisticFlag)
		{
			PlayerSet.DelMoney((DWORD)nMoney);
		}
	}

	return result;
}

BOOL KPlayer::Earn(int nMoney, bool statisticFlag)
{
	if (nMoney < 0)
		return FALSE;

	BOOL result = m_ItemList.AddMoney(room_equipment, nMoney);
	if (TRUE == result)
	{
		m_PlayerStatistic.ChangeMoney(nMoney);
		if (statisticFlag)
		{
			PlayerSet.AddMoney((DWORD)nMoney);
		}
		
		if (nMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
		{
			//InstantSave
			//SaveBaseInfoData();
			
			//获得金钱日志	
			LogEventParam logParam;
			logParam.event = log_event_add_money;
			logParam.param1 = GetGUID();
			logParam.param4 = nMoney;
			g_pLogSystem->Log(logParam);
		}
	}
	
	return result;
}

//当服务器从数据库中获得玩家全部数据，并加载之后，使该玩家有效
void	KPlayer::LaunchPlayer()
{
	if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_login))
	{
		RecordPlayerActionParam param;
		param.PlayerIndex = GetPlayerIndex();
		param.Action = player_action_login;
		snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_LOGIN);
		g_PlayerMonitor.RecordPlayerAction(param);
	}

	KNpc& npc = Npc[m_nIndex];

	int nSubWorld = npc.m_SubWorldIndex;
	int nRegion = npc.m_RegionIndex;
	int nX = npc.GetMapX();
	int nY = npc.GetMapY();
	int nOffX = npc.GetOffX();
	int nOffY = npc.GetOffY();
	SubWorld[nSubWorld].AddPlayer(nRegion, m_nPlayerIndex);
	m_ulLastSaveTime = g_SubWorldSet.GetGameTime();
	// 把真正的战斗状态在玩家进入游戏后再设置上

	// lixuewu 阻挡变了
	const int nBarrier = SubWorld[nSubWorld].m_Region[nRegion].GetBarrierMin(nX, nY, nOffX, nOffY, FALSE);
	if ((nBarrier > 0) && (nBarrier < Obstacle_JumpFly))
	{
		npc.ChangeWorld(m_sLoginRevivalPos.m_nSubWorldID, m_sLoginRevivalPos.m_nMpsX, m_sLoginRevivalPos.m_nMpsY);
	}

	//设置AI技能
	SkillStrategy skillStrategy;
	skillStrategy.Mode = skill_strage_mode_positive;
	switch(npc.GetSeries())
	{
	case enRoleType_Knight:
		skillStrategy.SkillId = KNIGHT_NORMALSKILL_ID;
		break;
	case enRoleType_Enchanter:
		skillStrategy.SkillId = ENCHANTER_NORMALSKILL_ID;
		break;
	case enRoleType_Monstrous:
		skillStrategy.SkillId = MONSTROUS_NORMAILSKILL_ID;
		break;
	}
	npc.GetController().SetSkillStrategyList(&skillStrategy, 1);

	enumROLETYPE enRoleType = enRoleType_Knight;
	switch(npc.m_Series)
	{
	case series_metal:
		enRoleType = enRoleType_Knight;
		break;
	case series_wood:
		enRoleType = enRoleType_Enchanter;
		break;
	case series_water:
		enRoleType = enRoleType_Monstrous;
		break;
	}

	lockStoreBox();

	m_dwDeathScriptId = g_FileName2Id("\\script\\main.lua");

	// lixuewu 2005.01.06 生命值为0的玩家,被认为已经死亡需要重生
	if (Npc[m_nIndex].m_UnaryAttrMgr[nuai_curlife] <= 0)
	{
		Revive(REMOTE_REVIVE_TYPE);
	}

	//add by zuolizhi
	if( m_nNetConnectIdx != -1 )
	{
		const char* szIP = NULL;
		
		if (g_pServer != NULL)
			szIP = g_pServer->GetClientInfo( m_nNetConnectIdx );

		m_dwLastLoginIP = inet_addr( szIP );
	}

	/*
	int dbReturn = 0;
	dbReturn = g_pController->SendOPToDB( 
		enDBTask_GetRoleListData, 
		m_PlayerName, 
		&KPlayer::BuffDBLoadCallBack, 
		m_nNetConnectIdx );


	// Load skill data
	dbReturn = g_pController->SendOPToDB( enDBTask_GetRoleListData, 
							   m_PlayerName, 
							   &KPlayer::LoadSkillInfoCallBack, 
							   m_nNetConnectIdx
							 );

	// Load Item data
	dbReturn = g_pController->SendOPToDB( enDBTask_GetRoleListData, 
							   m_PlayerName, 
							   &KPlayer::ItemlistDBLoadCallBack, 
							   m_nNetConnectIdx
							 );

	// Load skill data
	dbReturn = g_pController->SendOPToDB( enDBTask_GetRoleListData, 
							   m_PlayerName, 
							   &KPlayer::LoadTaskListCallBack, 
							   m_nNetConnectIdx
							 );

	// Set Online Flag
	dbReturn = g_pController->SendOPToDB( enDBTask_Procedure,
								(void*)&GetGUID(),
								&KPlayer::OnlineCallBack,
								m_nNetConnectIdx
							);

  */
	LoadEnhance( );
	LoadSkill( );
	LoadItem( );
	LoadTask( );
	LoadFriend( );
	LoadSocial( );
	LoadReserve( );
	DBOnline( );
	GetMarriageData(NULL);
	EmployCenter::Singleton().Cancel(GetPlayerIndex());
	
	CheckNameColor();

	BuffMgr& bm = BuffMgr::Singleton();
	ConfigManager& cm = ConfigManager::Singleton();

	//添加GM BUFF
	int gmBuffID = cm.GetGlobalVariable(global_var_buff_gm);
	if (gmBuffID > 0 && IsGM())
	{
		bm.AddNpcBuff( GetNpcIndex(), GetNpcIndex(), gmBuffID );
	}

	//添加上线就有的BUFF
	int onlineBuffCount = cm.GetOnlineBuffCount();
	for(int onlineBuffIndex = 0; onlineBuffIndex < onlineBuffCount; onlineBuffIndex++)
	{
		int onlineBuffID = cm.GetOnlineBuffID(onlineBuffIndex);
		if (onlineBuffID > 0)
		{
			unsigned long buffIndex = 0;
			buffIndex = bm.AddNpcBuff( GetNpcIndex(), GetNpcIndex(), onlineBuffID );
			_ASSERT(buffIndex);
		}
	}

	int subWorldIndex = npc.GetSubWorldIndex();
	OnEvent(player_event_enter_world, &subWorldIndex);

	if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_player_enter_world))
	{
		//玩家进入游戏世界日志
		LogEventParam enterWorldEvent;
		enterWorldEvent.event = log_event_player_enter_world;
		enterWorldEvent.param1 = GetGUID();
		g_pLogSystem->Log(enterWorldEvent);
	}

	//TODO 暂时凑活着用
	//第一次上线
	if (m_dwPlayGameTime == 0)
	{
		char gmText[64] = { 0 };
		sprintf(gmText, "?gm ds GM_FirstLoginGame(\"%s\")", GetPlayerName());
		gmText[sizeof(gmText) - 1] = 0;
		BOOL result = TextGMFilter(GetPlayerIndex(), gmText, sizeof(gmText));

		ExecuteScript(m_dwDeathScriptId, "FirstLoginGame", 0);
	}

	//TaisuiWheel Init
	m_TaisuiSys.Init(GetPlayerIndex(),GetNetConnectIdx());
	m_FuryMgr.Init(GetPlayerIndex());
	m_ExpInsuranceMgr.PlayerOnline();
	m_QuestInsuranceMgr.PlayerOnline();

	ShizuBannerMgr::getSingleton().showBannerToPlayer(m_nPlayerIndex);

	int random = 0;
	int CombatTop10Interval = ConfigManager::Singleton().GetGlobalVariable(global_var_combat_top10_interval);
	if(CombatTop10Interval > COMBAT_TOP10_SYNC_MAX_INTERVAL || CombatTop10Interval < COMBAT_TOP10_SYNC_MIN_INTERVAL)
			CombatTop10Interval = COMBAT_TOP10_SYNC_MIN_INTERVAL;
	random = g_Random(CombatTop10Interval);
	if(random < 0 || random >= CombatTop10Interval)
		random = 0;
	m_nMyTurn = random;
	
	//查询是否有GM回复的消息
	GetGMReply();

	m_TitleManager.OnLaunchPlayer();
}
#endif

BOOL	KPlayer::ExecuteScript(char * ScriptFileName, char * szFunName, int nParam, unsigned int nResultCount)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	return ExecuteScript(g_FileName2Id(ScriptFileName), szFunName, nParam, nResultCount);	
}
#define MAX_TRYEXECUTESCRIPT_COUNT 5
BOOL	KPlayer::ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam, unsigned int nResultCount)
{
/*	bool	bCanExecuteScript = true;

  //当前脚本未置空
  if (m_bWaitingPlayerFeedBack)
  {
		if (Npc[m_nIndex].m_ActionScriptID && (dwScriptId != Npc[m_nIndex].m_ActionScriptID))
		{
		m_btTryExecuteScriptTimes ++;
		if (m_btTryExecuteScriptTimes <= MAX_TRYEXECUTESCRIPT_COUNT)//最大尝试执行脚本的次数。如果超过后，就执行当前脚本，并放弃原来的脚本。
		{
		bCanExecuteScript = false;
		}
		else
		{
		m_btTryExecuteScriptTimes = 0;
		m_bWaitingPlayerFeedBack = false;
		Npc[m_nIndex].m_ActionScriptID = 0;
		}
		}
		}
		
		  if (!bCanExecuteScript) return FALSE;
	*/
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			ScriptSetPlayerIndex(m_nPlayerIndex);
			ScriptSetSubWorldIndex(Npc[m_nIndex].m_SubWorldIndex);
// 			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
// 			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
// 			
// 			Lua_PushNumber(pScript->m_LuaState, m_dwID);
// 			pScript->SetGlobalName(SCRIPT_PLAYERID);
// 			
// 			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
// 			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			int nTopIndex = 0;
			
			pScript->SafeCallBegin(&nTopIndex);
			if (nResultCount)
				ClearScriptResult();

			if (pScript->CallFunction(szFunName, nResultCount, "d", nParam)) 
			{
				if (nResultCount)
				{
					if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
					{
						SetScriptResult( (int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState) ));
					}
				}
				bExecuteScriptMistake = false;
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL	KPlayer::ExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams, unsigned int nResultCount)
{
/*bool	bCanExecuteScript = true;

  //当前脚本未置空
  if (m_bWaitingPlayerFeedBack)
  {
		if (Npc[m_nIndex].m_ActionScriptID && (dwScriptId != Npc[m_nIndex].m_ActionScriptID))
		{
		m_btTryExecuteScriptTimes ++;
		if (m_btTryExecuteScriptTimes <= MAX_TRYEXECUTESCRIPT_COUNT)//最大尝试执行脚本的次数。如果超过后，就执行当前脚本，并放弃原来的脚本。
		{
		bCanExecuteScript = false;
		}
		else
		{
		m_btTryExecuteScriptTimes = 0;
		m_bWaitingPlayerFeedBack = false;
		Npc[m_nIndex].m_ActionScriptID = 0;
		}
		}
		}
		
		  bCanExecuteScript = true;//test
		  
			if (!bCanExecuteScript) return FALSE;
	*/
	
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nTopIndex = 0;
		
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;
			ScriptSetPlayerIndex(m_nPlayerIndex);
			ScriptSetSubWorldIndex(Npc[m_nIndex].m_SubWorldIndex);
// 			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
// 			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
// 			
// 			Lua_PushNumber(pScript->m_LuaState, m_dwID);
// 			pScript->SetGlobalName(SCRIPT_PLAYERID);
// 			
// 			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
// 			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			pScript->SafeCallBegin(&nTopIndex);
			if (nResultCount)
				ClearScriptResult();

			if ( (!szParams) || !szParams[0]) 
			{
				if (pScript->CallFunction(szFunName, nResultCount, ""))
				{
					if (nResultCount)
					{
						if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
						{
							SetScriptResult((int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
						}
					}
					bExecuteScriptMistake = false;
				}
			}
			else
			{
				if (pScript->CallFunction(szFunName, nResultCount, "sd", szParams,0)) 
				{
					if (nResultCount)
					{
						if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
						{
							SetScriptResult((int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
						}
					}
					bExecuteScriptMistake = false;
				}
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}


BOOL	KPlayer::ExecuteScript(char * ScriptFileName, char * szFunName, char *  szParams, unsigned int nResultCount)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	DWORD dwScriptId = g_FileName2Id(ScriptFileName);
	return ExecuteScript(dwScriptId, szFunName, szParams, nResultCount);
}

// --> Rocker Edit Start 2005/08/30
BOOL	KPlayer::ExecuteScript2Param(DWORD dwScriptId, LPCSTR cFuncName, int nResultCount, int nParam1, int nParam2)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nTopIndex = 0;
		
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;

			ScriptSetPlayerIndex(m_nPlayerIndex);
			ScriptSetSubWorldIndex(Npc[m_nIndex].m_SubWorldIndex);

// 			Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
// 			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
// 			
// 			Lua_PushNumber(pScript->m_LuaState, m_dwID);
// 			pScript->SetGlobalName(SCRIPT_PLAYERID);
// 			
// 			Lua_PushNumber(pScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
// 			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			pScript->SafeCallBegin(&nTopIndex);
			if (nResultCount)
				ClearScriptResult();

			if (pScript->CallFunction(cFuncName, nResultCount, "dd", nParam1, nParam2))
			{
				if (nResultCount)
				{
					if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
					{
						SetScriptResult((int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
					}
				}
				bExecuteScriptMistake = false;
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}
// <-- Rocker End

BOOL KPlayer::ExecuteScript3Param(DWORD dwScriptId, LPCSTR cFuncName, int nResultCount, int nParam1, int nParam2, int nParam3)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nTopIndex = 0;
		
		if (pScript)
		{
			Npc[m_nIndex].m_ActionScriptID = dwScriptId;

			ScriptSetPlayerIndex(m_nPlayerIndex);
			ScriptSetSubWorldIndex(Npc[m_nIndex].m_SubWorldIndex);
			
			pScript->SafeCallBegin(&nTopIndex);
			if (nResultCount)
				ClearScriptResult();

			if (pScript->CallFunction(cFuncName, nResultCount, "ddd", nParam1, nParam2, nParam3))
			{
				if (nResultCount)
				{
					if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
					{
						SetScriptResult((int)Lua_ValueToNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
					}
				}
				bExecuteScriptMistake = false;
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			Npc[m_nIndex].m_ActionScriptID = 0;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		Npc[m_nIndex].m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}

// Modify by Cooler 2004-7-2
// Begin -->
// Optimize!
#define PATHNAME_GMSCRIPT	"\\script\\gmfunctions.lua"
BOOL KPlayer::DoScript(LPSTR pScriptCommand)
{
	if(NULL == pScriptCommand)
	{
		return FALSE;
	}

	if(NULL == m_pGMScript)
	{
		m_pGMScript = new KLuaScript;
		if(NULL == m_pGMScript)
		{
			return FALSE;
		}
		m_pGMScript->Init();
		m_pGMScript->RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());
		
		//Load GM Script Functions
		m_pGMScript->Load(PATHNAME_GMSCRIPT);
		
// 		Lua_PushNumber(m_pGMScript->m_LuaState, m_nPlayerIndex);
// 		m_pGMScript->SetGlobalName(SCRIPT_PLAYERINDEX);
// 		Lua_PushNumber(m_pGMScript->m_LuaState, m_dwID);
// 		m_pGMScript->SetGlobalName(SCRIPT_PLAYERID);
	}

	ScriptSetSubWorldIndex(Npc[m_nIndex].m_SubWorldIndex);
// 	Lua_PushNumber(m_pGMScript->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
// 	m_pGMScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

	if(m_pGMScript->LoadBuffer((PBYTE)pScriptCommand, strlen(pScriptCommand))) 
	{
		ScriptSetSubWorldIndex(Npc[m_nIndex].m_SubWorldIndex);
		ScriptSetPlayerIndex(m_nPlayerIndex);
		return m_pGMScript->ExecuteCode();
	}

	return FALSE;
}
/*BOOL	KPlayer::DoScript(char * ScriptCommand)
{
	if (NULL == ScriptCommand) return FALSE;
	KLuaScript *Script = new KLuaScript;
	Script->Init();
	Script->RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());
	
	//GM Standand Script Functions 
	Script->Load("\\script\\gmscript.lua");
	
	
	Lua_PushNumber(Script->m_LuaState, m_nPlayerIndex);
	Script->SetGlobalName(SCRIPT_PLAYERINDEX);
	Lua_PushNumber(Script->m_LuaState, m_dwID);
	Script->SetGlobalName(SCRIPT_PLAYERID);
	
	Lua_PushNumber(Script->m_LuaState, Npc[m_nIndex].m_SubWorldIndex);
	Script->SetGlobalName(SCRIPT_SUBWORLDINDEX);
	
	if (Script->LoadBuffer((PBYTE)ScriptCommand, strlen(ScriptCommand))) 
	{
		BOOL bResult = Script->ExecuteCode();	
		delete Script; //Question!
		return bResult;
	}
	delete Script; //Question!
	return FALSE;
}*/
// End <--


void	KPlayer::DoScriptAction(PLAYER_SCRIPTACTION_SYNC * pUIInfo) //要求显示某个UI界面
{
	if (!pUIInfo) return;
	
	//服务器端脚本时
	if (pUIInfo->m_bParam2 == 1)
	{
#ifdef _SERVER
		if ( pUIInfo->m_bUIId != UI_TOP_INFO &&  pUIInfo->m_bUIId != UI_NOTEINFO)
		{
			m_dwWaitingPlayerFeedBackSeed = g_GetRandomSeed();
            //pUIInfo->m_dwNpcKind = 0;
			*((int *)(pUIInfo->m_pContent + pUIInfo->m_nBufferLen)) = m_dwWaitingPlayerFeedBackSeed;
		}
		 
		pUIInfo->ProtocolType = (BYTE)s2c_scriptaction;		
		pUIInfo->m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + pUIInfo->m_nBufferLen - 1 + sizeof(int);
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)pUIInfo, pUIInfo->m_wProtocolLong + 1 );
#else
	}
	else //客户端脚本要求显示脚本 直接运行
	{
		OnScriptAction((PLAYER_SCRIPTACTION_SYNC *)pUIInfo);
#endif
	}
	
}

//服务器端获知玩家选择了某项后，处理~~
void	KPlayer::ProcessPlayerSelectFromUI(BYTE* pProtocol)			// 处理当玩家从选择菜单选择某项时的操作	
{
	// 没有等待直接退出
	if (m_bWaitingPlayerFeedBack == false)
	{
		return;
	}
	PLAYER_SELECTUI_COMMAND * pSelUI = (PLAYER_SELECTUI_COMMAND*) pProtocol;
	if (m_dwWaitingPlayerFeedBackSeed != pSelUI->dwSeed)
	{
		return;
	}
	m_bWaitingPlayerFeedBack = false;
	m_dwWaitingPlayerFeedBackSeed = g_GetRandomSeed();
	//如果返回负数，表示退出该脚本执行环境
	if (pSelUI->nSelectIndex < 0  && pSelUI->nSelectType != select_sayortalk) 
		m_nAvailableAnswerNum = 0;
	
	switch(pSelUI->nSelectType)
	{
		//选择对话框选择时传来的协议
	case select_sayortalk: //Say(),Or Talk
		{
			//<---- Modified By Ray [Luoliang] [2005-10-21]
			bool bMultiSelection = m_bMultiSelection;
			m_bMultiSelection = false;
			if ((m_nAvailableAnswerNum > pSelUI->nSelectIndex) && (pSelUI->nSelectIndex >=0))							
			{
				if (m_szTaskAnswerFun[pSelUI->nSelectIndex][0])
				{
					if (m_nIndex)
					{
						ExecuteScript2Param(Npc[m_nIndex].m_ActionScriptID, m_szTaskAnswerFun[pSelUI->nSelectIndex], 0, pSelUI->nSelectIndex, pSelUI->nParam1);
					}
				}	
			}
			// End. Ray [LuoLiang] [2005-10-21] ---->		
			// 取消操作时执行默认函数 lixuewu 2004.10.13
			else if (m_nAvailableAnswerNum > 0 && pSelUI->nSelectIndex == -1)
			{				
				if (m_nIndex)
				{
					ExecuteScript(Npc[m_nIndex].m_ActionScriptID, "no", pSelUI->nSelectIndex);
				}
			}
		}
		break;
	// Added By Rocker 2004.7.16
	case select_inputdialog:
		{
#ifdef _SERVER
			switch( pSelUI->nParam2 )
			{
				case enMerchant:		//跑商
					if (m_nAvailableAnswerNum > pSelUI->nSelectIndex)
					{
						if (m_szTaskAnswerFun[pSelUI->nSelectIndex][0])
						{
							if (m_nIndex)
							{
								ExecuteScript(Npc[m_nIndex].m_ActionScriptID, m_szTaskAnswerFun[pSelUI->nSelectIndex], pSelUI->nParam1);
							}
						}
					}
					break;
				case enCDKey:			//CDKey
					
					break;
				default:
					break;
			}
			
#endif
		}
		break;
	}
}

#ifndef _SERVER
//玩家在界面交互后，选择了某项后，向服务器端发送
void	KPlayer::OnSelectFromUI(PLAYER_SELECTUI_COMMAND * pSelectUI, UIInfo eUIInfo)	//当玩家从选择框中选择某项后，将向服务器发送			
{
	if (!pSelectUI) return;
	
	switch(eUIInfo)
	{
	case UI_SELECTDIALOG:
		{
			if (g_bUISelIntelActiveWithServer)
			{
				pSelectUI->ProtocolType = (BYTE)c2s_playerselui;
				pSelectUI->dwSeed = m_dwWaitingPlayerFeedBackSeed;
				if (g_pClient)
				{					
					//<---- Modified By Ray [Luoliang] [2005-10-21]
					//g_pClient->SendPackToServer((BYTE*)pSelectUI, sizeof(PLAYER_SELECTUI_COMMAND));
					g_pClient->SendPackToServer(g_ConnectID,(BYTE*)pSelectUI, pSelectUI->wLength + 1);
					// End. Ray [LuoLiang] [2005-10-21] ---->
					
				}
			}
			else
			{
				ProcessPlayerSelectFromUI((BYTE *)pSelectUI);			// 处理当玩家从选择菜单选择某项时的操作	
			}
		}
		break;
	case UI_TALKDIALOG:
		{
			if (g_bUISpeakActiveWithServer)
			{
				pSelectUI->ProtocolType = (BYTE)c2s_playerselui;
				pSelectUI->dwSeed = m_dwWaitingPlayerFeedBackSeed;
				//<---- Modified By Ray [Luoliang] [2005-10-21]
				//g_pClient->SendPackToServer((BYTE*)pSelectUI, sizeof(PLAYER_SELECTUI_COMMAND));
				g_pClient->SendPackToServer(g_ConnectID,(BYTE*)pSelectUI, pSelectUI->wLength + 1);
				// End. Ray [LuoLiang] [2005-10-21] ---->
			}
			else
			{
				ProcessPlayerSelectFromUI((BYTE *)pSelectUI);			// 处理当玩家从选择菜单选择某项时的操作	
			}
			
		}break;
	}
}
#endif
/*
#ifdef _SERVER
void	KPlayer::S2CExecuteScript(char * ScriptName, char * szParam = NULL)
{
	if (!ScriptName || (!ScriptName[0])) return; 
	
	PLAYER_SCRIPTACTION_SYNC  ScriptAction;
	ScriptAction.m_nOperateType = SCRIPTACTION_EXESCRIPT;
	ScriptAction.ProtocolType = s2c_scriptaction;
	char * script = NULL;
	if (szParam == NULL || szParam[0] == 0) 
	{
		ScriptAction.m_nBufferLen = strlen(ScriptName) + 1 ;
		strcpy(ScriptAction.m_pContent, ScriptName);
	}
	else
	{
		ScriptAction.m_nBufferLen = strlen(ScriptName) + 2 + strlen(szParam);
		sprintf(ScriptAction.m_pContent, "%s|%s", ScriptName, szParam);
	}
	ScriptAction.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - 300 + ScriptAction.m_nBufferLen - 1;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&ScriptAction, sizeof(PLAYER_SCRIPTACTION_SYNC) - 300 + ScriptAction.m_nBufferLen);	
	
}
#endif
*/
#ifndef _SERVER
void	KPlayer::OnScriptAction(PLAYER_SCRIPTACTION_SYNC * pMsg)
{
	PLAYER_SCRIPTACTION_SYNC * pScriptAction = (PLAYER_SCRIPTACTION_SYNC *)pMsg;
	char szString[1000];
	

	switch(pScriptAction->m_nOperateType)
	{
	case SCRIPTACTION_UISHOW:
		{
			if ( pScriptAction->m_bUIId == UI_AUCTION_DLG ||
				pScriptAction->m_bUIId == UI_INPUTDIALOG ||
				pScriptAction->m_bUIId == UI_SELECTDIALOG ||
				pScriptAction->m_bUIId == UI_ACCEPT_QUEST ||
				pScriptAction->m_bUIId == UI_SHOW_QUEST ||
				pScriptAction->m_bUIId == UI_ITEM_UPDATE ||
				pScriptAction->m_bUIId == UI_ITEM_ADDMAGIC ||
				pScriptAction->m_bUIId == UI_ITEM_SETYAO ||
				pScriptAction->m_bUIId == UI_ITEM_MAKE ||
				pScriptAction->m_bUIId == UI_ITEM_GETYAO ||
				pScriptAction->m_bUIId == UI_SKILL_STUDY_DLG ||
				pScriptAction->m_bUIId == UI_MAIL_CENTRE ||
				pScriptAction->m_bUIId == UI_CREATE_SHIZU ||// 创建氏族
				pScriptAction->m_bUIId == UI_CREATE_ZHUHOU ||// 创建诸侯
				pScriptAction->m_bUIId == UI_CITY_DLG )
			{
				 CoreDataChanged( GDCNI_CLOSE_ALLDIALOG, NULL, NULL );
			}
			
			switch(pScriptAction->m_bUIId)
			{
			case UI_OPEN_USEITEM_DLG:
				{
					CoreDataChanged( GDCNI_OPEN_USEITEM_DLG, pScriptAction->m_nParam, NULL );
				}
				break;
			case UI_AUCTION_DLG:
				{
					CoreDataChanged( GDCNI_AUCTION_WND, NULL, NULL );
				}
				break;
			case UI_CLOSE_DIALOG:
				{
					m_dwWaitingPlayerFeedBackSeed = 0;
					CoreDataChanged(GDCNI_NPC_DLG_CLOSE, 0, 0);
					break;
				}
			// Added By Rocker 2004.7.16
			case UI_INPUTDIALOG: // 通知客户端显示输入窗口
				{
					m_dwWaitingPlayerFeedBackSeed = *((int *)(pScriptAction->m_pContent + pScriptAction->m_nBufferLen));
					tagInputDlgParam param;
					param.szText = pScriptAction->m_pContent;
					param.nTextLen = strchr(param.szText, '|') - param.szText;
					param.nCallType = enMerchant;
					param.nInputType = pScriptAction->m_bParam1;
//					CoreDataChanged(GDCNI_INPUT_DIALOG, (unsigned int)&param, 0);
				}
				break;

			case UI_SELECTDIALOG://通知客户端显示选择窗口
				{
					if (pScriptAction->m_nBufferLen <= 0) break;
					m_dwWaitingPlayerFeedBackSeed = *((int *)(pScriptAction->m_pContent + pScriptAction->m_nBufferLen));
					g_bUISelIntelActiveWithServer = pScriptAction->m_bParam2;
					g_bUISelLastSelCount = pScriptAction->m_bOptionNum;
					// m_nParam 表示对话框类型 0 古老的Say 1 新的Say 2 确定对话框 3 是否对话框 4 建筑操作界面
					const int nType = pScriptAction->m_nParam & 0xFFFF;
					switch(nType)
					{
					case 1:
						{
							KUiQuestionAndAnswer	*pQuest = NULL;
							
							if (pScriptAction->m_bOptionNum <= 0)
								pQuest = (KUiQuestionAndAnswer *)malloc(sizeof(KUiQuestionAndAnswer));
							else
								pQuest = (KUiQuestionAndAnswer *)malloc(sizeof(KUiQuestionAndAnswer) + sizeof(KUiAnswer) * (pScriptAction->m_bOptionNum - 1));
							
							char strContent[2048];
							char * pAnswer = NULL;
							pQuest->AnswerCount = 0;
							//主信息为字符串
							if (pScriptAction->m_bParam1 == 0)
							{
								g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen - sizeof(char) * pScriptAction->m_bOptionNum);
								pAnswer = strstr(strContent, "|");
								if (!pAnswer)
								{
									pScriptAction->m_bOptionNum = 0;
									pQuest->AnswerCount = 0;
								}
								else
									*pAnswer++ = 0;
								
								g_StrCpyLen(pQuest->Question, strContent, sizeof(pQuest->Question));
								
								pQuest->QuestionLen = strlen(pQuest->Question);
							}
							//主信息为数字标识
							else 
							{
								g_StrCpyLen(pQuest->Question, g_GetStringRes(*(int *)pScriptAction->m_pContent, szString, 1000), sizeof(pQuest->Question));
								pQuest->QuestionLen = strlen(pQuest->Question);
								
								g_StrCpyLen(strContent, pScriptAction->m_pContent + sizeof(int), pScriptAction->m_nBufferLen - sizeof(char) * pScriptAction->m_bOptionNum);
								pAnswer = strContent + 1;
							}
							
                            char * pIcons = (char*)(pScriptAction->m_pContent + pScriptAction->m_nBufferLen - pScriptAction->m_bOptionNum * sizeof(char));
                            for (int i = 0; i < pScriptAction->m_bOptionNum; i ++)
							{
								char * pNewAnswer = strstr(pAnswer, "|");
								
								if (pNewAnswer)
								{
									*pNewAnswer = 0;
									strcpy(pQuest->Answer[i].AnswerText, pAnswer);
									pQuest->Answer[i].AnswerLen = strlen(pQuest->Answer[i].AnswerText);
                                    pQuest->Answer[i].imageId = pIcons[i];
									pAnswer = pNewAnswer + 1;
									pQuest->AnswerCount++;
								}
								else
								{
									strcpy(pQuest->Answer[i].AnswerText, pAnswer);
									pQuest->Answer[i].AnswerLen = strlen(pQuest->Answer[i].AnswerText);
                                    pQuest->Answer[i].imageId = pIcons[i];
									pQuest->AnswerCount++;
									break;
								}
							}
							CoreDataChanged(GDCNI_OPEN_QUEST_NPC_DIALOG,(unsigned int) pQuest, 1);
							AutoRobotMgr::Singleton().SetMode(enRobotMode_ReadyForSell);
							free(pQuest);
							pQuest = NULL;
						}
						break;
					}
				}
				break;
            case UI_MOVIE_SCENE:
				{
					break;
					KUiMovieScene movieScene;
					
					string text = string(pScriptAction->m_pContent);
					int startIndex = 0;
					int endIndex = text.find('|');
					movieScene.mainText = text.substr(startIndex, endIndex - startIndex);
					while(endIndex != text.length() - 1)
					{
						startIndex = endIndex + 1;
						endIndex = text.find('|', startIndex);
						string selectionText = text.substr(startIndex, endIndex - startIndex);
						movieScene.selectionText.push_back(selectionText);
					}
					movieScene.imageId = pScriptAction->m_nParam;
					CoreDataChanged(GDCNI_OPEN_MOVIE_SCENE, (unsigned int)&movieScene, 0);
				}
				break;
			case UI_NOTEINFO:
				{
					if (pScriptAction->m_nBufferLen > sizeof(int)* MAX_PARAM)
					{
						return;
					}
					// lixuewu 2004.09.14 新的任务记事
					unsigned int nParam[MAX_PARAM] = {0};
					if (pScriptAction->m_nBufferLen > 0)
					{
						memcpy(&nParam, pScriptAction->m_pContent, pScriptAction->m_nBufferLen);
					}
					if (pScriptAction->m_bParam1 == 255)
					{
						QuestLog::GetInstance()->RemoveQuest(pScriptAction->m_nParam);							
					}
					else
					{
						QuestLog::GetInstance()->UpDateQuest(pScriptAction->m_nParam, (pScriptAction->m_bParam1 << 8) | pScriptAction->m_bOptionNum, nParam);
					}
				}
				break;
//             case UI_ACCEPT_QUEST:
//                 {
//                     m_dwWaitingPlayerFeedBackSeed = *((int *)(pScriptAction->m_pContent + pScriptAction->m_nBufferLen));
//                     const unsigned int uQuestID = pScriptAction->m_nParam;
//                     unsigned int uIdx = QuestCache::GetInstance().FindQuest(uQuestID);
//                     if (uIdx != INVALID_QUEST_IDX)
//                     {
//                         CoreDataChanged(GDCNI_OPEN_QUEST_NPC_DIALOG,(unsigned int)&QuestCache::GetInstance().GetQuestInfoByIdx(uIdx), 0);
//                     }
//                     else
//                     {
//                         _QUERY_QUEST_DETAIL SYN;
//                         SYN.Protocol = c2s_quest_family;
//                         SYN.wProtocolSize = sizeof(SYN) - 1;
//                         SYN.ProtocolExtend = c2s_query_quest_detail_cache;
//                         SYN.dwQuestID = uQuestID;
//                         if (g_pClient)
//                         {
//                             g_pClient->SendPackToServer(g_ConnectID, &SYN, sizeof(SYN));
//                         }
//                     }
//                 }
//                 break;
//             case UI_SHOW_QUEST:
//                 {
//                     m_dwWaitingPlayerFeedBackSeed = *((int *)(pScriptAction->m_pContent + pScriptAction->m_nBufferLen));
// 					const unsigned int uQuestID = pScriptAction->m_nParam;
//                     QuestLog::GetInstance().ShowQuest(uQuestID, pScriptAction->m_pContent, pScriptAction->m_nBufferLen);
// 				}
// 				break;
			//--> Rocker 2005/07/12 顶部的消息提示条
			case UI_TOP_INFO:
				{
					if (pScriptAction->m_nBufferLen <= 0) 
						break;
					
					char strContent[1024];
					if (pScriptAction->m_bParam1 == 0)
					{
						g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
					}
					else
					{
						g_StrCpyLen(strContent, pScriptAction->m_pContent,  pScriptAction->m_nBufferLen + 1);
						int nNum = atoi(strContent);
						g_GetStringRes(nNum, strContent ,sizeof(strContent));
					}
					
					KSystemMessage	sMsg;
					sMsg.eType = SMT_TIPMSG;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					g_StrCpyLen(sMsg.szMessage, strContent, sizeof(sMsg.szMessage));
					CoreDataChanged(GDCNI_TOP_MESSAGE, (UINT)strContent, 0);
				}
				break;
			case UI_PLAYMUSIC:
				{
					char szMusicFile[MAX_PATH];
					memcpy(szMusicFile, pScriptAction->m_pContent, pScriptAction->m_nBufferLen);
					szMusicFile[pScriptAction->m_nBufferLen] = 0;
					g_SubWorldSet.m_cMusic.ScriptPlay(szMusicFile);
				}break;
			
			case UI_ITEM_UPDATE:
				{
//					m_bBeset = true;
					CoreDataChanged(GDCNI_OPEN_COMPOUND_WND,true, COMPOUND_LEVELUP );
				}break;
			case UI_ITEM_ADDMAGIC:
				{	
					CoreDataChanged( GDCNI_OPEN_COMPOUND_WND, true, COMPOUND_ADDMAGIC );
				}break;
			case UI_ITEM_SETYAO:
				{
//					m_bBeset = true;
					CoreDataChanged(GDCNI_OPEN_COMPOUND_WND, true, COMPOUND_ADDYAO );
				}break;
			case UI_ITEM_MAKE:
				{
					CoreDataChanged( GDCNI_OPEN_COMPOUND_WND, true, COMPOUND_MAKE );
				}break;
			case UI_ITEM_GETYAO:
				{	
					CoreDataChanged( GDCNI_OPEN_COMPOUND_WND, true, COMPOUND_GETYAO  );
				}break;
			case UI_SKILL_STUDY_DLG:
				{	
					CoreDataChanged( GDCNI_SKILLLIST_OPEN, true, true );
				}break;
			case UI_MAIL_CENTRE:
				{
					CoreDataChanged( GDCNI_SWITCH_MAIL, TRUE, NULL );
				}
				break;
			case UI_CREATE_SHIZU:// 创建氏族
				{
					CoreDataChanged( GDCNI_OPEN_CREATETONG, 0, NULL );
				}
				break;
			case UI_CREATE_ZHUHOU:// 创建诸侯
				{
					CoreDataChanged( GDCNI_OPEN_CREATETONG, 1, NULL );
				}
				break;

			case UI_CREATE_LIANMENG:
				{
					CoreDataChanged( GDCNI_OPEN_CREATETONG, 2, NULL );
				}
				break;
			case UI_CITY_DLG: //打开城市
				{
					CoreDataChanged( GDCNI_OPEN_CITY, NULL, NULL );
				}
				break;
			
			case UI_TAISUI_DLG: //打开太岁之轮
				{
                    CoreDataChanged(GDCNI_TAISUI_DLG_OPEN,0,0);
				}
				break;
			case UI_NAVIGATION_WND: //打开功能面板
				{
					CoreDataChanged(GDCNI_OPEN_NAVIGATION_WND, 0, 0);
				}
				break;
			case UI_NAVIGATIONEX_WND: //打开扩展功能面板
				{
					CoreDataChanged(GDCNI_OPEN_NAVIGATIONEX_WND, 0, 0);
				}
				break;
			case UI_ACTIVE_NAVIGATION_BUTTON:
				{
					if ( pScriptAction->m_bParam1 >= 0 && pScriptAction->m_bParam1 < 8 )
					{
						CoreDataChanged(GDCNI_ACTIVE_NAVIGATION_BUTTON, (unsigned int)pScriptAction->m_bParam1, 0);
					}
				}
				break;
			case UI_SHORTCUT_WND: //打开快捷栏面板
				{
					CoreDataChanged(GDCNI_OPEN_SHORTCUT_WND, 0, 0);
				}
				break;
			case UI_SHORTCUTPLUS_WND: //打开扩展快捷栏面板
				{
					CoreDataChanged(GDCNI_OPEN_SHORTCUTPLUS_WND, 0, 0);
				}
				break;
			case UI_OPEN_ANY_WINDOW: //打开任意窗口
				{
					int windowId = pScriptAction->m_bParam1;
					CoreDataChanged(GDCNI_OPEN_WINDOW, 0, windowId);
				}
				break;
			case UI_OPEN_TIMER: //打开计时器
				{
					int time = 0;
					int type = 0;
					char description[COMMON_CLIENT_MSG_LEN_512] = "";
					char tempContent[COMMON_CLIENT_MSG_LEN_512] = "";
					if(pScriptAction->m_nBufferLen >= COMMON_CLIENT_MSG_LEN_512)
					{
						strncpy(tempContent, pScriptAction->m_pContent, COMMON_CLIENT_MSG_LEN_512 - 1);
						tempContent[COMMON_CLIENT_MSG_LEN_512 - 1] = 0;
					}
					else
					{
						strncpy(tempContent, pScriptAction->m_pContent, pScriptAction->m_nBufferLen);
						tempContent[pScriptAction->m_nBufferLen] = 0;
					}
					sscanf(tempContent, "%d|%d|%s", &time, &type, description);
					
					pair<int, int> timerArgs;
					timerArgs.first = time;
					timerArgs.second = type;

					CoreDataChanged(GDCNI_OPEN_TIMER, (unsigned int)&timerArgs, (int)description);
				}
				break;

			case UI_OPEN_TONG_RECRUIT:
				{
                    CoreDataChanged(GDCNI_OPEN_TONG_RECRUIT, 0, 0);
				}//end for case
				break;
			case UI_OPEN_INSTANCE_REWARD_WND:
				{
					char tempContent[COMMON_CLIENT_MSG_LEN_64] = "";
					strncpy(tempContent, pScriptAction->m_pContent, pScriptAction->m_nBufferLen);
					tempContent[pScriptAction->m_nBufferLen] = 0;

					KUiInstanceReward	instanceReward;
					int					succeed;
					sscanf(tempContent, "%d|%d|%d|%d|%d|%d", &instanceReward.UseTime, &instanceReward.KillNum,
						&instanceReward.RewardExp, &instanceReward.SkillExp, &instanceReward.RewardId,
						&succeed);
					instanceReward.Succeed = (succeed == 0 ? false : true);

					CoreDataChanged(GDCNI_OPEN_INSTANCE_REWARD, (unsigned int)&instanceReward, 0);
				}
				break;
			case UI_OPEN_CREDIT_SHOP_WND:
				{
					CoreDataChanged(GDCNI_OPEN_CREDIT_SHOP, 0, 0);
				}
				break;

			case UI_WORLD_COMBAT_SCORE:
				{
					CoreDataChanged(GDCNI_RECV_WORLD_COMBAT_SCORE,0,pScriptAction->m_nParam);
				}
			case UI_STUDENT_REPORT:
				{
					char tempContent[COMMON_CLIENT_MSG_LEN_512] = "";
					
					if(pScriptAction->m_nBufferLen >= COMMON_CLIENT_MSG_LEN_512)
					{
						strncpy(tempContent, pScriptAction->m_pContent, COMMON_CLIENT_MSG_LEN_512 - 1);
						tempContent[COMMON_CLIENT_MSG_LEN_512 - 1] = 0;
					}
					else
					{
						strncpy(tempContent, pScriptAction->m_pContent, pScriptAction->m_nBufferLen);
						tempContent[pScriptAction->m_nBufferLen] = 0;
					}
					
					char masterName[COMMON_CLIENT_MSG_LEN_512];
					int lastLevel = 0;
					int curLevel = 0;

					sscanf(tempContent, "%d|%d|%s", &lastLevel, &curLevel, masterName);
					masterName[sizeof(masterName) - 1] = 0;
					pair<int, int> level;
					level.first = lastLevel;
					level.second = curLevel;
					CoreDataChanged(GDCNI_OPEN_RECOMMEND_REPORT, (UINT)masterName, (int)&level);
				}
			}
	} break;
	case SCRIPTACTION_EXESCRIPT://要求客户端调用某个脚本
		{
			if (pScriptAction->m_nBufferLen <= 0 ) break;
			char szScriptInfo[1000];
			g_StrCpyLen(szScriptInfo, pScriptAction->m_pContent,pScriptAction->m_nBufferLen + 1);
			char * pDivPos = strstr(szScriptInfo, "/");
			if (pDivPos)	*pDivPos++ = 0; 
			if (pDivPos)
				ExecuteScript(szScriptInfo, "OnCall", pDivPos);
			else
				ExecuteScript(szScriptInfo, "OnCall", "");
		}
		break;
	}
	
	
}
#endif

#ifdef _SERVER

//客户端请求与某个Npc对话
//服务器版本
void KPlayer::DialogNpc(BYTE * pProtocol)
{
	PLAYER_DIALOG_NPC_COMMAND * pDialogNpc = (PLAYER_DIALOG_NPC_COMMAND*) pProtocol;
	int checkResult = CheckDialogNpc(pDialogNpc->nNpcId);
	if (TRUE == checkResult)
	{
		int dialogNpcIndex = FindAroundNpc(pDialogNpc->nNpcId);
		if (dialogNpcIndex > 0)
		{
			KNpc& dialogNpc = Npc[dialogNpcIndex];
			int dialogTime = dialogNpc.GetActionTime();
			if (dialogTime >= 0)
			{
				DelayedAction dialogNpcAction(m_nPlayerIndex, delayed_action_dialog_npc, dialogTime, delayed_action_msg_dialog_npc);
				dialogNpcAction.GetDialogNpcParam().m_DialogNpcId = pDialogNpc->nNpcId;
				m_ActionDelayer.NewAction(dialogNpcAction);	
				
				int nCurMpsX = 0;
				int nCurMpsY = 0;
				
				dialogNpc.GetMpsPos(&nCurMpsX,&nCurMpsY);
				
				int nWorldID    = 0;
				int nWorldIndex = dialogNpc.GetSubWorldIndex();
				
				if (nWorldIndex >=0 && nWorldIndex < MAX_SUBWORLD)
				{
					nWorldID    = SubWorld[nWorldIndex].m_SubWorldID;
				}//endif
				
				Npc[m_nIndex].RecordDialogPos(nWorldID,nCurMpsX,nCurMpsY);

			}
		}		
	}
}

#endif

#ifndef _SERVER
void KPlayer::s2cLevelUp(BYTE* pMsg)
{
	PLAYER_LEVEL_UP_SYNC* pLevelUp = (PLAYER_LEVEL_UP_SYNC*)pMsg;

	const LevelUpAdd* pLevelUpAdd = KLevelUpInfo::Singleton().GetLevelUpAdd(pLevelUp->ArriveLevel, GetSeries(), GetSkillSeries());
	if (pLevelUpAdd == NULL)
	{
		return;
	}

	// 角色升级
	CoreDataChanged(GDCNI_LEVELUPINFO, (unsigned int)pLevelUpAdd, 0);

	const int nFullExp = pLevelUpAdd->Exp;
	Npc[m_nIndex].m_Level = pLevelUp->ArriveLevel;

    QuestLog::GetInstance()->UpDateQuestProcessLastUpDateTime();
    
	CoreDataChanged(GDCNI_SKILLLIST_CHANGE, 0, 0);
	
	//if ( strlen(pLevelUpAdd->Tip) > 0 )
	//	CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)pLevelUpAdd->Tip, 0);
	
	if ( strlen(pLevelUpAdd->Desc) > 0 )
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)pLevelUpAdd->Desc, 0);	
}
#endif
/*
#ifndef _SERVER
void	KPlayer::s2cGetCurAttribute(BYTE* pMsg)
{
	PLAYER_ATTRIBUTE_SYNC	*pAttribute = (PLAYER_ATTRIBUTE_SYNC*)pMsg;

	switch (pAttribute->m_btAttribute)
	{
	case ATTRIBUTE_STRENGTH:
		Npc[m_nIndex].AddCompAttr(ncai_strength, idx_base_value, pAttribute->m_nBasePoint);
//		UpdataCurData();
		Npc[m_nIndex].UpdateStrengthEffect(Npc[m_nIndex].m_CompAttrMgr[ncai_strength] - pAttribute->m_nBasePoint, 
					pAttribute->m_nBasePoint);
		break;
		
	case ATTRIBUTE_DEXTERITY:
		Npc[m_nIndex].AddCompAttr(ncai_nimbus, idx_base_value, pAttribute->m_nBasePoint);
//		UpdataCurData();
		Npc[m_nIndex].UpdateNimbusEffect(Npc[m_nIndex].m_CompAttrMgr[ncai_nimbus] - pAttribute->m_nBasePoint,
					pAttribute->m_nBasePoint);
		break;

	case ATTRIBUTE_CONSTITUTION:
		Npc[m_nIndex].AddCompAttr(ncai_body, idx_base_value, pAttribute->m_nBasePoint);
//		UpdataCurData();
		Npc[m_nIndex].UpdateBodyEffect(Npc[m_nIndex].m_CompAttrMgr[ncai_body] - pAttribute->m_nBasePoint, 
					pAttribute->m_nBasePoint);
		break;
		
	case ATTRIBUTE_INTELLECT:
		Npc[m_nIndex].m_CompAttrMgr.Set(ncai_art, idx_base_value, pAttribute->m_nBasePoint);
//		UpdataCurData();
		Npc[m_nIndex].UpdateArtEffect(Npc[m_nIndex].m_CompAttrMgr[ncai_art] - pAttribute->m_nBasePoint, 
					pAttribute->m_nBasePoint);
		break;

	default:
		break;
	}
}
#endif
*/
/*
#ifndef _SERVER
void	KPlayer::s2cSetExp(int nExp)
{
	if (nExp > m_nExp)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_GET_EXP, nExp - m_nExp);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		//CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	else if (nExp < m_nExp)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_DEC_EXP, m_nExp - nExp);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		//CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	}
	
	this->m_nExp = nExp;
}
#endif
*/
#ifndef _SERVER

void _SysMoneyToUiMoney(int money, int& jin, int& yin, int& tong)
{
	jin = money / 10000;
	yin = (money % 10000) / 100;
	tong = money % 100; 
}

void	KPlayer::s2cSyncMoney(BYTE* pMsg)
{
	PLAYER_MONEY_SYNC	*pMoney = (PLAYER_MONEY_SYNC*)pMsg;
	
	int nMoney1 = m_ItemList.GetMoney(room_equipment);
	//请在此加入通知客户端金钱变化的消息
	if ( (pMoney->m_nMoney1 - nMoney1) > 0 )
	{
		char szBuff[COMMON_CLIENT_MSG_LEN_256];

		int jin = 0;
		int yin = 0;
		int tong = 0;
		_SysMoneyToUiMoney(pMoney->m_nMoney1 - nMoney1, jin, yin, tong );
		if ( jin <= 0 && yin > 0 && tong >= 0 )
		{
			sprintf( szBuff, MSG_GET_MONEY_YIN, yin, tong );
		}
		else if ( jin <= 0 && yin <= 0 && tong > 0 )
		{
			sprintf( szBuff, MSG_GET_MONEY_TONG, tong );
		}
		else
		{
			sprintf( szBuff, MSG_GET_MONEY_JIN, jin, yin, tong );
		}
		
		// Convert pData to PCHATROOMMSG_TO_SOMEONE due to don't
		// want to change existing interface.
		char	buf[sizeof(CHATROOMMSG_TO_SOMEONE) + MAXSIZE_CHAT_MSG] = {0};
		PCHATROOMMSG_TO_SOMEONE	pRoomMsg = (PCHATROOMMSG_TO_SOMEONE)buf;

		pRoomMsg->roomId = SYSTEM_ROOM_ID;

		memcpy(pRoomMsg->msg, szBuff, COMMON_CLIENT_MSG_LEN_256);
		pRoomMsg->msg[COMMON_CLIENT_MSG_LEN_256-1] = '\0';
		
		CoreDataChanged( GDCNI_RECV_CHAT_DATE_R2P, SYSTEM_ROOM_ID, (int)pRoomMsg );

		ScreenEffectMgr::Singleton().Player(1, BeforeUi);
	}

	m_ItemList.SetMoney(pMoney->m_nMoney1, pMoney->m_nMoney2, pMoney->m_nMoney3);
}

#endif

#ifndef _SERVER
void	KPlayer::SyncCurPlayer(BYTE* pMsg)
{
	m_nCurrentAttrSyncTime = 0;
	
	m_Creature.Dismiss();
	m_ItemList.RemoveAll();
	
	CURPLAYER_SYNC* PlaySync = (CURPLAYER_SYNC *)pMsg;
	this->m_nIndex = NpcSet.SearchID(PlaySync->m_dwID);
	this->m_dwID = g_FileName2Id(Npc[m_nIndex].Name); 
	SetBlockClientControl((TRUE == PlaySync->bIsBlockClientControl) ? true : false);
	
	m_bRandomAddAttr = PlaySync->bRandomAddAttr;
	Npc[m_nIndex].m_Kind = kind_player;
	Npc[m_nIndex].m_Level = (DWORD)PlaySync->m_btLevel;
	Npc[m_nIndex].m_nSex = PlaySync->m_btSex;
	Npc[m_nIndex].m_Series  = PlaySync->m_btSeries;
	Npc[m_nIndex].SetPlayerIdx(CLIENT_PLAYER_INDEX);
	Npc[m_nIndex].m_nHeadImage = PlaySync->m_HeadImage;
	
	m_SkillSeries = (RoleSkillSeries)PlaySync->m_SkillSeries;

	m_nSkillExp = PlaySync->m_dwSkillExp;

	m_nWeightMax = PlaySync->nWeightMax;
	m_nWeightMaxTempAdd = 0;

	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_body, idx_base_value, PlaySync->m_wBody);
	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_nimbus, idx_base_value, PlaySync->m_wNimbus);
	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_strength, idx_base_value, PlaySync->m_wStrength);
	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_art, idx_base_value, PlaySync->m_wArt);
	
	Npc[m_nIndex].m_ActionScriptID = 0;
	Npc[m_nIndex].m_TrapScriptID = 0;
	m_nExp = PlaySync->m_dwExp;

	m_nWorldStat = (int)PlaySync->m_wWorldStat;

	m_ItemList.Init(CLIENT_PLAYER_INDEX);

	m_ItemList.SetMoney(PlaySync->m_nMoney1, PlaySync->m_nMoney2, 0);

	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_lifeuplimit, idx_base_value, PlaySync->m_wLifeMax);
	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_manauplimit, idx_base_value, PlaySync->m_wManaMax);
	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_liferenewspeed, idx_base_value, PLAYER_LIFE_REPLENISH);
	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_manarenewspeed, idx_base_value, PLAYER_MANA_REPLENISH);

	SetBaseSpeedAndRadius();
	
	Npc[m_nIndex].RestoreNpcBaseInfo();

	Npc[m_nIndex].UpdateBodyEffect(0, Npc[m_nIndex].m_CompAttrMgr[ncai_body], false, false);
	Npc[m_nIndex].UpdateNimbusEffect(0, Npc[m_nIndex].m_CompAttrMgr[ncai_nimbus], false, false);
	Npc[m_nIndex].UpdateStrengthEffect(0, Npc[m_nIndex].m_CompAttrMgr[ncai_strength], false, false);
	Npc[m_nIndex].UpdateArtEffect(0, Npc[m_nIndex].m_CompAttrMgr[ncai_art], false, false);

	Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_curlife, PlaySync->m_wCurLife);
	Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_curmana, PlaySync->m_wCurMana);
	
	m_BuyInfo.Clear();
	memset(m_szTaskAnswerFun, 0, sizeof(m_szTaskAnswerFun));
	memset(m_szRelayCallbackFun, 0, sizeof(m_szRelayCallbackFun));
	m_nAvailableAnswerNum = 0;	

	memset(Npc[m_nIndex].m_szChatBuffer, 0, sizeof(Npc[m_nIndex].m_szChatBuffer));
	Npc[m_nIndex].m_nCurChatTime = 0;
	
	m_RunStatus = 1;
}
#endif

BOOL	KPlayer::CheckTrading()
{
	return FALSE;
}

void	KPlayer::SetBaseSpeedAndRadius()
{
	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_walkspeed, idx_base_value, BASE_WALK_SPEED);
//	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_runspeed, idx_base_value, BASE_RUN_SPEED);
	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_attackspeed, idx_base_value, BASE_ATTACK_SPEED);
	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_castspeed, idx_base_value, BASE_CAST_SPEED);
	Npc[m_nIndex].m_CompAttrMgr.Set(ncai_visionradius, idx_base_value, BASE_VISION_RADIUS);
}

#ifndef _SERVER
//客户端版本
void KPlayer::DialogNpc(int nIndex)
{
	if (nIndex > 0 && Npc[nIndex].m_Index > 0)
	{
		//Npc[GetNpcIndex()].SetTarget( 	type_npc, 0 );
		PLAYER_DIALOG_NPC_COMMAND DialogNpcCmd;
		DialogNpcCmd.nNpcId = Npc[nIndex].m_dwID;
		DialogNpcCmd.ProtocolType = c2s_dialognpc;
		if (g_pClient)
			g_pClient->SendPackToServer(g_ConnectID,&DialogNpcCmd, sizeof(PLAYER_DIALOG_NPC_COMMAND));
	}
}
#endif

#ifndef _SERVER
void KPlayer::CheckObject(int nIdx)
{
/*	enum	// 物件类型
{
Obj_Kind_MapObj = 0,		// 地图物件，主要用于地图动画
Obj_Kind_Body,				// npc 的尸体
Obj_Kind_Box,				// 宝箱
Obj_Kind_Item,				// 掉在地上的装备
Obj_Kind_Money,				// 掉在地上的钱
Obj_Kind_LoopSound,			// 循环音效
Obj_Kind_RandSound,			// 随机音效
Obj_Kind_Light,				// 光源（3D模式中发光的东西）
Obj_Kind_Door,				// 门类
Obj_Kind_Trap,				// 陷阱
Obj_Kind_Prop,				// 小道具，可重生
Obj_Kind_Num,				// 物件的种类数
};*/
	
	switch(Object[nIdx].m_nKind)
	{
	case Obj_Kind_Item:
	case Obj_Kind_Money:
		PickUpObj(nIdx);
		Npc[m_nIndex].SetTarget(type_obj, 0);
		break;
	case Obj_Kind_Box:
	case Obj_Kind_Door:
	case Obj_Kind_Trap:
	case Obj_Kind_Prop:
	//--> Rocker 2005/05/12
	case Obj_Kind_House_Entry:
	case Obj_Kind_Furniture:
	//<-- End
		this->ObjMouseClick(nIdx);
		Npc[m_nIndex].SetTarget(type_obj, 0);
		break;
	default:
		break;
	}
//	m_nObjectIdx = 0;
	m_nPickObjectIdx = 0;
}
#endif





#ifndef _SERVER
void KPlayer::DrawSelectInfo()
{
	if (m_nIndex <= 0)
		return;
	const int nPeapleIdx = GetSelectNpc();
	if (nPeapleIdx)
	{
		if ((Npc[nPeapleIdx].m_Kind == kind_player)||(Npc[nPeapleIdx].m_Kind == kind_creature)||
			(Npc[nPeapleIdx].m_Kind == kind_siege_weapon))
		{
			if (!NpcSet.CheckShowName())
			{
				int nHeight = Npc[nPeapleIdx].PaintLife(Npc[nPeapleIdx].GetNpcPate(), true);
				Npc[nPeapleIdx].PaintInfo(nHeight + 5, true);
			}
		}
		else if (Npc[nPeapleIdx].m_Kind == kind_dialoger)
		{
			if (!NpcSet.CheckShowName())
			{
//				Npc[nPeapleIdx].PaintInfo(Npc[nPeapleIdx].GetNpcPate(), true);
			}
		}
		else
		{
			Npc[nPeapleIdx].DrawBlood();
		}
	}
	else if (const int nObjectIdx = GetSelectObj())
	{
		if (!ObjSet.CheckShowName())
			Object[nObjectIdx].DrawInfo();
	}
}
#endif

#ifdef _SERVER
void  KPlayer::repairItemByItem(DWORD dwItemIdx, int nRepairPresent)
{
	if ( dwItemIdx < 0 || dwItemIdx >= MAX_ITEM )
	{
		return;
	}

	KItem& item = Item[dwItemIdx];

	int index = m_ItemList.SearchID(item.GetID());
	if (index <= 0)
		return;
	
	
	
	//是否可以修理
	if (item.CanBeRepaired() == false) 
		return;
	
	//网络是否正常
	if(m_nNetConnectIdx == -1)
		return;
	
	//日志：修理物品
	bool needLog = item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level);
	if (needLog)
	{
		LogEventParam repairItemEvent;
		repairItemEvent.event = log_event_repair_item_by_item;
		repairItemEvent.param1 = GetGUID();
		repairItemEvent.param2 = item.GetGUID();
		item.GetItemTemplateId(repairItemEvent.param3.data, sizeof(repairItemEvent.param3.data) - 1);
		repairItemEvent.param4 = 0;
		g_pLogSystem->Log(repairItemEvent);
	}
	
	int originalMaxDur = item.GetMaxDurability();	
	int originalCurDur = item.GetDurability();
	
	
	int currentCurDur = originalCurDur + originalMaxDur * nRepairPresent / 100;
	if(currentCurDur > originalMaxDur)
		currentCurDur = originalMaxDur;	
	
	item.SetDurability(currentCurDur);
	
	//如果修理的是穿在身上的装备，并且原来的耐久为0（装备已损坏），需要更新装备效果
	if (TRUE == GetItemList().IsItemInEquip(item.GetID()) && originalCurDur == 0)
	{
		GetItemList().OnEquipChanged();
	}
	
	item.SyncAttribute(item_attr_max_durability, m_nNetConnectIdx);
	item.SyncAttribute(item_attr_durability, m_nNetConnectIdx);
}

void KPlayer::repairItem(DWORD dwItemID, bool special)
{
	//是否打开商店的修理界面
	if(m_BuyInfo.m_nBuyIdx < 0)
		return;
	
	int index = m_ItemList.SearchID(dwItemID);
	if (index <= 0)
		return;

	KItem& item = Item[index];
	
	//是否可以修理
	if (item.CanBeRepaired() == false) 
		return;

	//网络是否正常
	if(m_nNetConnectIdx == -1)
		return;

	int cost = item.getRepairPrice(special);

	//如果不需要扣钱，则表示不用修理
	if (cost <= 0)
		return;

	int insteadSpecieIndex = ConfigManager::Singleton().GetGlobalVariable(globar_var_instead_specie_index);

	if (insteadSpecieIndex == 0)
	{
		//默认值
		insteadSpecieIndex = 12;
	}

	unsigned long insteadSpecieCount = GetPlusPoint(insteadSpecieIndex);

	bool needLog = false;
	if (insteadSpecieCount >= cost)
	{
		if (!DecPlusPoint(insteadSpecieIndex, cost))
		{
			return;
		}
		//日志：修理物品
		needLog = (cost >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount) ||
			(item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level)));
		if (needLog)
		{
			LogEventParam repairItemEvent;
			repairItemEvent.event = log_event_repair_item_by_instead_specie;
			repairItemEvent.param1 = GetGUID();
			repairItemEvent.param2 = item.GetGUID();
			item.GetItemTemplateId(repairItemEvent.param3.data, sizeof(repairItemEvent.param3.data) - 1);
			repairItemEvent.param4 = -cost;
			g_pLogSystem->Log(repairItemEvent);
		}
	}
	else
	{
		int curMoney = m_ItemList.GetMoney(room_equipment);

		if (curMoney + insteadSpecieCount < cost)
		{
			return;
		}
		//修理扣钱
		if(!Pay(cost - insteadSpecieCount))
			return;

		needLog = (cost >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount) ||
			(item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level)));
		if (needLog)
		{
			LogEventParam repairItemEvent;
			repairItemEvent.event = log_event_repair_item;
			repairItemEvent.param1 = GetGUID();
			repairItemEvent.param2 = item.GetGUID();
			item.GetItemTemplateId(repairItemEvent.param3.data, sizeof(repairItemEvent.param3.data) - 1);
			repairItemEvent.param4 = -(cost - insteadSpecieCount);
			g_pLogSystem->Log(repairItemEvent);
		}

		if (insteadSpecieCount > 0)
		{
			if (!DecPlusPoint(insteadSpecieIndex, insteadSpecieCount))
			{
				return;
			}
			needLog = (cost >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount) ||
				(item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level)));
			if (needLog)
			{
				LogEventParam repairItemEvent;
				repairItemEvent.event = log_event_repair_item_by_instead_specie;
				repairItemEvent.param1 = GetGUID();
				repairItemEvent.param2 = item.GetGUID();
				item.GetItemTemplateId(repairItemEvent.param3.data, sizeof(repairItemEvent.param3.data) - 1);
				repairItemEvent.param4 = -insteadSpecieCount;
				g_pLogSystem->Log(repairItemEvent);
			}
		}
	}

	int originalMaxDur = item.GetMaxDurability();	
	int originalCurDur = item.GetDurability();

	int maxDurDropFactor = 0;
	if (special)
	{
		maxDurDropFactor = AbradeTable::Singleton().GetSpecialRepairMaxDurDropFactor();
	}
	else
	{
		maxDurDropFactor = AbradeTable::Singleton().GetNormalRepairMaxDurDropFactor();
	}

	int currentMaxDur = originalMaxDur - ( (originalMaxDur - originalCurDur) * maxDurDropFactor / 100 );
	if(currentMaxDur < 1)
		currentMaxDur = 1;	

	item.SetMaxDurability(currentMaxDur);
	item.SetDurability(currentMaxDur);

	//如果修理的是穿在身上的装备，并且原来的耐久为0（装备已损坏），需要更新装备效果
	if (TRUE == GetItemList().IsItemInEquip(item.GetID()) && originalCurDur == 0)
	{
		GetItemList().OnEquipChanged();
	}

	item.SyncAttribute(item_attr_max_durability, m_nNetConnectIdx);
	item.SyncAttribute(item_attr_durability, m_nNetConnectIdx);
}
#endif

// Add by Cooler 2004-7-20
// Begin -->
#ifdef _SERVER
void KPlayer::SetTempAddStatus(TEMPADDSTATUSTYPE enAddType, 
							   const TEMPADDSTATUSINFO &tagAddStatus)
{
	switch(enAddType)
	{
	case enTempAddType_Credit:
		break;
	case enTempAddType_Skill:
		break;
	case enTempAddType_AllSkill:
		break;
	default:
		break;
	}
}

#endif
// End <--

// 物品相应的操作：
//     获得物品：1、从地上拣物品 2、脚本控制直接给 3、交易 4、player 之间赠
//               送(通过交易实现)
//         客户端鼠标点在物件上，然后客户端通过物件得出物品id、应该出现在装备栏或者
//         物品栏的位置或者跟随鼠标的计算，把计算结果发给服务器；服务器收到后首先判
//         断玩家与物品的位置关系，然后检查客户端的计算结果是否正确，然后进行相应的
//         处理，把处理结果发给客户端
//     物品的位置调整：客户端处理好物品来源位置、鼠标上物品目的位置，然后发给服务器，
//     服务器确认后通知客户端最终确定的操作（如果成功，把协议原样发回去）；如果有数
//     值等其他变化，另外通知客户端；
//     使用物品：1、吃药 2、装备（鼠标右键点击）
//         客户端向服务器端申请吃什么位置的药，同时客户端的相应数值先作相应变化，服
//         务器收到客户端申请后，处理完相应数据，通过player同步的方式通知客户端数据
//         的变化；
//         装备：鼠标右键点在一个装备上，自动处理装备上、卸下的位置信息，发给服务器，
//         服务器处理完后原样发回来；

int KPlayer::GetWeightCurrent(int *pWeightTaken, int *pWeightMax)
{
	if ( pWeightTaken == NULL || pWeightMax == NULL)
		return FALSE;

	*pWeightTaken = GetWeightTaken();
	*pWeightMax = GetWeightMax();

	return TRUE;
}

int KPlayer::GetWeightTaken()
{
	KInventory* pRoom = m_ItemList.GetRoom(room_equipment);
	_ASSERT(pRoom != NULL);

	if (pRoom != NULL)
		return pRoom->getWeight();
	else
		return 0;
}

void KPlayer::AddWeightMax(int nDelta, bool bSync, bool bTempAdd)
{
	if (m_nWeightMax + nDelta < 0)
	{
		return;
	}

	m_nWeightMax += nDelta;

	if(bTempAdd)
	{
		m_nWeightMaxTempAdd += nDelta;
	}

#ifdef _SERVER
	if (bSync)
	{
		SyncAttribute(attr_WeightMax);
	}
#endif
}

void KPlayer::CheckWeight() 
{
	if(GetWeightMax() < GetWeightTaken())
	{
#ifdef _SERVER
		if (0 == m_OverweightBuffIndex)
		{
			int overweightBuffID = ConfigManager::Singleton().GetGlobalVariable(global_var_buff_overweight);
			if (overweightBuffID > 0)
			{
				m_OverweightBuffIndex = BuffMgr::Singleton().AddNpcBuff(
				GetNpcIndex(), GetNpcIndex(), overweightBuffID );
				_ASSERT(m_OverweightBuffIndex);
			}			
		}
#else
		if (!m_IsOverweight)
		{
			m_IsOverweight = true;
			//客户端提示超重
			CoreDataChanged(GDCNI_ERROR_MESSAGE_CODE, overweight_error_message, overweight_error_overweight);
		}
#endif
	}
	else
	{
#ifdef _SERVER
		if (m_OverweightBuffIndex > 0)
		{
			BuffMgr::Singleton().ClearBuffByID(	GetNpcIndex(), m_OverweightBuffIndex);
			m_OverweightBuffIndex = 0;
		}
#else
		if (m_IsOverweight)
		{
			m_IsOverweight = false;
			//客户端提示超重
			CoreDataChanged(GDCNI_ERROR_MESSAGE_CODE, overweight_error_message, overweight_error_normal);
		}
#endif
	}

	
}

/*
#ifdef _SERVER
void KPlayer::AutoAddItem(unsigned int uIndex)
{
	POINT point;
	if(m_ItemList.m_Room[room_equipment].FindRoom(&point))
	{
		m_ItemList.Add(uIndex, pos_equiproom, point.x, point.y, NULL, item_sync_type_gain);
	}
	
}
#endif
*/

#ifndef _SERVER
// Add by Cooler 2004-8-11
// Begin -->
void KPlayer::clientSendSplitItemCmd(const ItemPos &sourPos, const ItemPos &destPos, int nSplitCount)
{

	SPLITPILEITEM splitCmd;
	splitCmd.ProtocolType = c2s_splitpileitem;
	splitCmd.sourPlace	= sourPos.nPlace;
	splitCmd.sourX		= sourPos.nX;
	splitCmd.sourY		= sourPos.nY;
	splitCmd.destPlace	= destPos.nPlace;
	splitCmd.destX		= destPos.nX;
	splitCmd.destY		= destPos.nY;
	splitCmd.splitItemCount = nSplitCount;

	if(g_pClient)
	{
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&splitCmd, sizeof(SPLITPILEITEM));
	}
}

void KPlayer::clientSendMoveItemCmd(const ItemPos& sourPos, const ItemPos& destPos)
{
	//在客户端先判断是否
	if(m_ItemList.exchangePrecheck(&sourPos, &destPos) == false)
	{
		CoreDataChanged(GDCNI_ERROR_MESSAGE_CODE, exchange_error_message, 0);
		return;
	}

	PLAYER_MOVE_ITEM_COMMAND	sMove;
	ZeroMemory( &sMove, sizeof(sMove) );
	sMove.ProtocolType = c2s_playermoveitem;
	sMove.sourPlace = sourPos.nPlace;
	sMove.sourX		= sourPos.nX;
	sMove.sourY		= sourPos.nY;
	sMove.destPlace = destPos.nPlace;
	sMove.destX		= destPos.nX;
	sMove.destY		= destPos.nY;

	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&sMove, sizeof(PLAYER_MOVE_ITEM_COMMAND));
}

void KPlayer::CheckStoragePSW(const char *pcPassword)
{
	if (pcPassword == NULL || strlen(pcPassword) >= 16 )
		return;

	CHECKSTORAGEPSW tagCheck;
	tagCheck.ProtocolType = c2s_checkstoragepassword;
	strcpy(tagCheck.szPassword, pcPassword);

	if(g_pClient)
	{
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&tagCheck, sizeof(CHECKSTORAGEPSW));
	}
}

void KPlayer::CreateStoragePSW(const char *pcPassword)
{
	if ( pcPassword == NULL || strlen(pcPassword) >= 16)
		return ;

	CREATESTORAGEPSW tagCreate;
	tagCreate.ProtocolType = c2s_createstoragepassword;
	strcpy(tagCreate.szPassword, pcPassword);

	if(g_pClient)
	{
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&tagCreate, sizeof(CREATESTORAGEPSW));
	}
}
/*
void KPlayer::ReplaceStoragePSW(const char *pcSecondPassword)
{
}
*/

void KPlayer::ModifyStoragePSW(const char *pcNewPassword, 
							   const char *pcOldPassword)
{
	if ( pcNewPassword == NULL || strlen(pcNewPassword) >= 16 ||
		 pcOldPassword == NULL || strlen(pcOldPassword) >= 16)
		 return ;

	MODIFYSTORAGEPSW tagModify;
	tagModify.ProtocolType = c2s_modifystoragepassword;
	strcpy(tagModify.szNewPassword, pcNewPassword);
	strcpy(tagModify.szOldPassword, pcOldPassword);

	if(g_pClient)
	{
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&tagModify, sizeof(MODIFYSTORAGEPSW));
	}
}

void KPlayer::SendCloseStorageCMD()
{
	BYTE byCMD = c2s_closestorage;
	if ( g_pClient ) 
	{
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&byCMD, sizeof(BYTE));
	}
}
// End <--
extern struct iRepresentShell*	g_pRepresent;

#endif

#ifdef _SERVER
void KPlayer::GetRevivePos(PLAYER_REVIVAL_POS* pos)
{
	if (pos)
	{
		memcpy(pos, &m_sLoginRevivalPos, sizeof(PLAYER_REVIVAL_POS));
	}
}
#endif
//add by zuolizhi for pic question

#ifdef _SERVER

#define MAXSENDSIZE		(1024)//最大缓冲
#define	TICKSEC			18
#define	PICCOUNT		8

int	KPlayer::ProcessQuestion( )
{
//	char*			szQuestionBuffer=	NULL;
//	unsigned int	nQuestionLen	=	0;
//	unsigned int	nAnswer			=	0;
//	unsigned int	nLeftLen		=	0;
//	char			szBuffer[MAXSENDSIZE];
//	
//	if( g_PQConfig.nSwitch == FALSE )
//		return FALSE;
//
//	if( g_KillNpcConfig[Npc[m_nIndex].m_Level] == 0 )
//		return FALSE;
//
//	//Kill掉指定NPC数目一半开始下发或者上次答错
//	if( m_nKillNpcCount >= ( g_KillNpcConfig[Npc[m_nIndex].m_Level] / 2 ) || 
//		m_byErrorCount > 0 ) 
//	{
//		
//		//如果没有进入问答状态并且进入到了非问答区域,那么清空NPC数量,并且上次没有答错过
//		if( g_PQConfig.IsNoQMap( SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID ) &&
//			m_nAnswer == 0 &&
//			m_byErrorCount == 0 )
//		{
//			m_nKillNpcCount = 0;
//			return FALSE;
//		}
//
//		//已经选题
//		if( m_nAnswer > 0 )
//		{
//			//===================================================================
//			//取题目信息
//
//			if( m_byQuestionType == question_type_pic )
//			{
//				if( m_bySubQuestionType == question_type_pic )
//				{
//					nQuestionLen		=	g_PicQuestion[m_nQuestionIndex].nLen;
//					szQuestionBuffer	=	g_PicQuestion[m_nQuestionIndex].szQBuf;
//					nAnswer				=	g_PicQuestion[m_nQuestionIndex].nAnswer;
//				}
//				else
//				{
//					nQuestionLen		=	g_NumQuestion[m_nQuestionIndex].nLen;
//					szQuestionBuffer	=	g_NumQuestion[m_nQuestionIndex].szQBuf;
//					nAnswer				=	g_NumQuestion[m_nQuestionIndex].nAnswer;
//				}
//			}
//			else
//				if( m_byQuestionType == question_type_num )
//				{
//					nQuestionLen		=	g_NumQuestion[m_nQuestionIndex].nLen;
//					szQuestionBuffer	=	g_NumQuestion[m_nQuestionIndex].szQBuf;
//					nAnswer				=	g_NumQuestion[m_nQuestionIndex].nAnswer;
//				}
//
//			//===================================================================
//
//			//已经下发完毕
//			if( m_nQuestionSendLen == nQuestionLen )
//			{
//				if( m_nQuestionTimeLimit == 0 )//还没命令发问,发问
//				{
//					if( m_nKillNpcCount < g_KillNpcConfig[Npc[m_nIndex].m_Level] &&
//						m_byErrorCount == 0 )
//						return FALSE;
//					
//					int nOffset						=	0;
//					PASK_QUESTION	pAskQuestion	=	(PASK_QUESTION)szBuffer;
//					pAskQuestion->Protocol			=	s2c_byte_extend;
//					pAskQuestion->ProtocolExtend	=	s2c_ex_protocol_askquestion;
//					pAskQuestion->wProtocolSize		=	sizeof( ASK_QUESTION ) - 1;
//					pAskQuestion->wQuestionLen		=	m_nQuestionSendLen;
//					pAskQuestion->byQType			=	m_byQuestionType;
//					pAskQuestion->wCompress			=	g_PQConfig.nCompress;
//					
//					//=========================================
//					/*
//					if( m_byQuestionType == question_type_pic )
//					{
//						int nInsertPos = g_Random( PICCOUNT - 1 );
//
//						for( int nLoopCount = 0; nLoopCount < PICCOUNT; nLoopCount++)
//						{
//							if( nLoopCount !=  nInsertPos )
//								if( nLoopCount % 2 == 0 )
//									nOffset += sprintf(pAskQuestion->szAnswerSet + nOffset,"%d",m_nAnswer + 1 + g_Random(100)) + 1;
//								else
//									nOffset += sprintf(pAskQuestion->szAnswerSet + nOffset,"%d",m_nAnswer - 1 - g_Random(100)) + 1;								
//							else
//								nOffset += sprintf(pAskQuestion->szAnswerSet + nOffset,"%d",m_nAnswer) + 1;
//						}
//						
//						m_nAnswer = nInsertPos + 1;
//						pAskQuestion->szAnswerSet[nOffset] = 0;
//						nOffset++;
//						pAskQuestion->wProtocolSize += nOffset;
//					}
//					*/
//					if( m_byQuestionType == question_type_pic )
//					{
//						/*
//						char	szNum [50];
//						int		nNumArry[10] = { 0 };
//						int		nNumCount = 0;
//						int		nLoopCount = 0;
//						int		nLocate;
//						int		nValue;
//						int		nInsertPos;
//						bool	bIsO = false;
//						bool	bIsI = false;
//
//						nInsertPos = g_Random( 7 );
//
//						if( m_bySubQuestionType == question_type_pic )
//						{
//							nLocate	=	g_Random(sizeof(int) - 1);
//							nValue	=	*((char*)(&m_nAnswer) + nLocate);
//							m_bySubQuestionType = question_type_none;
//						}
//						else
//						{
//							nNumCount =	sprintf(szNum,"%d",m_nAnswer);
//							nLocate	=	g_Random(nNumCount - 1);
//							nValue	=	szNum[nLocate];
//						}
//						
//						m_nAnswer = nInsertPos + 1;
//						
//						sprintf(
//							pAskQuestion->szQuestionDescripte,
//							"  请从下列 8个选项中选择出与左图第<color=red> %d<color>个字符相同的选项。",
//							nLocate + 1);
//						
//						
//						if( nValue == '0' || nValue == 'O' )
//							bIsO = true;
//						
//						if( nValue == '1' || nValue == 'I' )
//							bIsI = true;
//
//						for( nLoopCount = 0; nLoopCount < 8; nLoopCount++ )
//						{
//							char	cValue;
//
//							if( nLoopCount != nInsertPos )
//							{
//								int		nRandom = g_Random( 25 );
//
//								if( nRandom % 2 == 1 )
//								{//字符
//
//									if( nValue > '9' && nValue == 'A' + nRandom )
//										if( nRandom == 25 )
//											nRandom = 0;
//										else
//											nRandom += 1;
//
//									//---------
//									if( bIsO && nValue == 'O' )
//										nRandom += 1;
//
//									if( bIsI && nValue == 'I' )
//										nRandom += 1;
//
//									//---------
//
//									cValue = 'A' + nRandom;
//
//									if( cValue == 'O' )
//										bIsO = true;
//									if( cValue == 'I' )
//										bIsI = true;
//								}
//								else
//								{
//									nRandom = g_Random( 9 );
//
//									if( nValue < '9' && nValue == '0' + nRandom )
//										if( nRandom == 9 )
//											nRandom = 0;
//										else
//											nRandom += 1;
//
//									//---------
//										
//									if( bIsO && nValue == '0' )
//										nRandom += 1;
//
//									if( bIsO && nValue == '1' )
//										nRandom += 1;
//
//									//---------
//
//									cValue = '0' + nRandom;
//
//									if( cValue == '0' )
//										bIsO = true;
//									if( cValue == '1' )
//										bIsI = true;
//								}
//							}
//							else
//								cValue = nValue;
//
//
//							nOffset += sprintf(pAskQuestion->szAnswerSet + nOffset,"%c",cValue ) + 1;
//						}
//						
//						pAskQuestion->szAnswerSet[nOffset] = 0;
//						nOffset++;
//						pAskQuestion->wProtocolSize += nOffset;
//						*/
//						
//							//-------------------------------------------------
//						char	szNum [50];
//						int		nNumArry[10] = { 0 };
//						int		nNumCount = 0;
//						int		nLoopCount = 0;
//						int		nDelValue1;
//						int		nDelValue2;
//
//						nNumCount	=	sprintf(szNum,"%d",m_nAnswer);
//						int	nLocate	=	g_Random(nNumCount - 1);
//						int nValue	=	szNum[nLocate] - 0x30;
//
//						sprintf(
//							pAskQuestion->szQuestionDescripte,
//							"  请从下列 8个选项中选择出与左图第<color=red> %d<color>个字符相同的选项。",
//							nLocate + 1);
//
//						//-------------------------------------------------
//						int nCount = 0;
//						for( nLoopCount = 0; nLoopCount < 10; nLoopCount++)
//						if( nLoopCount != nValue )
//						{
//							nNumArry[nCount] = nLoopCount;
//							nCount++;
//						}
//
//						nDelValue1 = g_Random( nCount - 1 );
//						nDelValue1 = nNumArry[nDelValue1];
//
//						nCount = 0;
//						for( nLoopCount = 0; nLoopCount < 10; nLoopCount++)
//						if( nLoopCount != nValue && 
//						   nLoopCount != nDelValue1 )
//						{
//							nNumArry[nCount] = nLoopCount;
//							nCount++;
//						}
//
//						nDelValue2 = g_Random( nCount - 1 );
//						nDelValue2 = nNumArry[nDelValue2];
//						//-------------------------------------------------
//
//						nCount = 0;
//						for( nLoopCount = 0; nLoopCount < 10; nLoopCount++)
//						{
//							if( nLoopCount ==  nDelValue1 ||
//								nLoopCount == nDelValue2 )
//								continue;
//							
//							nOffset += sprintf(pAskQuestion->szAnswerSet + nOffset,"%d",nLoopCount) + 1;
//							nCount ++;
//							
//							if( nLoopCount == nValue )
//								m_nAnswer = nCount;
//						}
//
//						pAskQuestion->szAnswerSet[nOffset] = 0;
//						nOffset++;
//						pAskQuestion->wProtocolSize += nOffset;
//											  
//					}
//					
//					//=========================================
//
//
//					//发送
//					g_pServer->PackDataToClient(
//						m_nNetConnectIdx, 
//						szBuffer, 
//						sizeof(ASK_QUESTION) + nOffset );
//
//					if( m_byQuestionType == question_type_pic )
//						m_nQuestionTimeLimit = 
//						(g_PQConfig.nPicQuesAnswerLimit * 60) * TICKSEC + TICKFREE;
//
//					if( m_byQuestionType == question_type_num )
//					{
//						//记录战斗状态
//						m_nQuestionTimeLimit = TICKFREE;
//					}
//
//					if( m_byErrorCount < 0xFF )
//						m_byErrorCount++;
//				}
//				else//已经发问开始倒计时
//				{
//					//到时没有回答,清状态,算作答错,或者是数字题1秒钟回答次数有限
//					if( m_nQuestionTimeLimit == TICKFREE )
//					{
//						if( m_byQuestionType == question_type_pic )
//						{
//							if( m_nQuestionPoint > 0 )
//								m_nQuestionPoint--;
//
//							//清掉重新问
//							m_nAnswer				=	0;
//							m_nQuestionIndex		=	0;
//							m_nQuestionSendLen		=	0;
//							m_nSendTimer			=	0;
//							m_nQuestionTimeLimit	=	0;
//							m_nKillNpcCount			=	0;
//						}
//					}
//					else if( m_nQuestionTimeLimit == TICKSEND &&
//							m_byQuestionType == question_type_num )
//					{
//						//num的答错了间隔一段时间再次发送
//						PASK_QUESTION	pAskQuestion	=	(PASK_QUESTION)szBuffer;
//						pAskQuestion->Protocol			=	s2c_byte_extend;
//						pAskQuestion->ProtocolExtend	=	s2c_ex_protocol_askquestion;
//						pAskQuestion->wProtocolSize		=	sizeof( ASK_QUESTION ) - 1;
//						pAskQuestion->wQuestionLen		=	m_nQuestionSendLen;
//						pAskQuestion->byQType			=	m_byQuestionType;
//						pAskQuestion->wCompress			=	g_PQConfig.nCompress;
//						
//						//发送
//						g_pServer->PackDataToClient(
//							m_nNetConnectIdx, 
//							szBuffer, 
//							sizeof(ASK_QUESTION) );
//
//						m_nQuestionTimeLimit = TICKFREE;
//						
//						if( m_byErrorCount < 0xFF )
//							m_byErrorCount++;
//
//					}
//					else
//						m_nQuestionTimeLimit--;
//
//					//====================================
//
//				}
//
//			}
//			else//下发图片
//			{
//				if( m_nSendTimer == 0 )//下发
//				{
//					PSEND_QUESTION	pSendQuestion = (PSEND_QUESTION)szBuffer;
//					
//					pSendQuestion->Protocol = s2c_byte_extend;
//					pSendQuestion->ProtocolExtend = s2c_ex_protocol_sendquestion;
//
//					//计算下发长度
//					nLeftLen = nQuestionLen - m_nQuestionSendLen;
//					if( nLeftLen > g_PQConfig.nSendByteSec )
//						pSendQuestion->wQuestionLen = g_PQConfig.nSendByteSec;
//					else
//						pSendQuestion->wQuestionLen = nLeftLen;
//
//					//计算协议长度
//					pSendQuestion->wProtocolSize = 
//						sizeof(SEND_QUESTION) + pSendQuestion->wQuestionLen - 1;
//
//					//已经发送数据
//					pSendQuestion->wOffsetData = m_nQuestionSendLen;
//
//					memcpy(
//						szBuffer + sizeof(SEND_QUESTION),
//						szQuestionBuffer + m_nQuestionSendLen, 
//						pSendQuestion->wQuestionLen );
//
//					//发送
//					g_pServer->PackDataToClient(
//						m_nNetConnectIdx, 
//						szBuffer, 
//						sizeof(SEND_QUESTION) + pSendQuestion->wQuestionLen );
//
//
//					m_nQuestionSendLen += pSendQuestion->wQuestionLen;
//					//重新开始计数
//					if( m_nQuestionSendLen != nQuestionLen )
//						m_nSendTimer = TICKSEC;
//				}
//				else//计时
//				{
//					m_nSendTimer--;
//				}
//			}
//		}
//		else//开始选题
//		{
//			//错误少于最大错误数的,选择图片题目
//			if( m_byErrorCount < g_PQConfig.nPicMaxError )
//			{
//				/*if( g_Random( 10000 ) % 2 == 1 )
//				{
//					m_nQuestionIndex = g_Random( g_PicQuestion.size() );
//					m_nAnswer = g_PicQuestion[m_nQuestionIndex].nAnswer;
//					m_bySubQuestionType = question_type_pic;
//				}
//				else
//				{*/
//					m_nQuestionIndex = g_Random( g_NumQuestion.size() );
//					m_nAnswer = g_NumQuestion[m_nQuestionIndex].nAnswer;
//				//}
//				
//				m_byQuestionType = question_type_pic;
//			}
//			else
//			{
//				//错误太多的选择数字题目
//				m_nQuestionIndex = g_Random( g_NumQuestion.size() );
//				m_nAnswer = g_NumQuestion[m_nQuestionIndex].nAnswer;
//				m_byQuestionType = question_type_num;
//			}
//		}
//		
//	}

	return TRUE;
}
#endif
//================================
//-------> Ray [Luoliang] 2005-6-29
#ifdef _SERVER

#include "FilterText.h"

void KPlayer::SetPetName(const char * szPetName)
{
//	_ASSERT(szPetName != NULL);
//	int nNameLen;
//	if ( szPetName == NULL || (nNameLen = strlen(szPetName)) == 0 )
//	{
//		return;
//	}	
//	
//	if ( g_TextFilter.IsTextPass(szPetName) )
//	{		
//		nNameLen = min(31, nNameLen);
//		memcpy(m_szPetName, szPetName, nNameLen);
//		m_szPetName[nNameLen] = 0;
//		
//		KNpc &aNpc = Npc[m_nIndex];
//		
//		PET_S2C_CHANGE_NAME petSync;	
//		petSync.SetProtocolHeader(pet_s2c_change_name, sizeof(PET_S2C_CHANGE_NAME) - 1 - sizeof(petSync.szPetName) + nNameLen);
//		
//		petSync.dwID = aNpc.m_dwID;	
//		strcpy(petSync.szPetName, m_szPetName);
//		
//		if( aNpc.m_SubWorldIndex >= 0 && 
//			aNpc.m_SubWorldIndex < MAX_SUBWORLD && 
//			SubWorld[aNpc.m_SubWorldIndex].m_SubWorldID != -1 ) 
//		{
//			int nMaxPlayer = MAX_PLAYER;
//			SubWorld[aNpc.m_SubWorldIndex].BroadCastRegion(&petSync, petSync.wLength + 1, nMaxPlayer,
//				aNpc.m_RegionIndex, aNpc.GetMapX(), aNpc.GetMapY());
//		}
//	}
//	else
//	{
//		PET_HEADER err;
//		err.SetProtocolHeader(pet_s2c_invalid_name, sizeof(PET_HEADER) - 1);
//
//		if (g_pServer != NULL)
//			g_pServer->PackDataToClient(m_nNetConnectIdx, &err, sizeof(PET_HEADER));
//	}
}

void KPlayer::SetPetHonor(BYTE byPetHonor)
{
	BYTE base = 100;
	byPetHonor = min(base, byPetHonor);
	m_byPetHonor = byPetHonor;
//	Send
}

#endif
//<------- End [Ray]

#ifdef _SERVER
void KPlayer::GetItemTransData(TItemtransfersData &data, DWORD dwItemId)
{
	int nItemIdx = m_ItemList.SearchID(dwItemId);

	if( IsItemIdxValid(nItemIdx) )
		Item[nItemIdx].GetItemtransfersData(data);
}

#endif

#ifdef _SERVER
int KPlayer::AddItem(const TItemtransfersData *pItemData)
{
	int nIndex = ItemSet.Add( pItemData->igenre, 
		pItemData->idetailtype,
		pItemData->iparticulartype,
		pItemData->ilevel,
		pItemData->nItemCount);

	if ( nIndex )
	{
		int x, y;
		Item[nIndex].SetItemtransfersData( *pItemData );
		if (m_ItemList.CheckCanPlaceInEquipment(&x, &y))
		{
			if (m_ItemList.Add(nIndex, pos_equiproom, x, y))
				return nIndex;
		}

		ItemSet.Remove( nIndex );
	}

	return 0;
}

void KPlayer::GetItemName(char *pName, int nBufSize, DWORD dwItemId)
{
	int nItemIdx = m_ItemList.SearchID(dwItemId);

	if( IsItemIdxValid(nItemIdx) )
	{
		strncpy(pName, Item[nItemIdx].GetName(), nBufSize);
	}
}

#endif

#ifdef _SERVER
void KPlayer::GetItemGuid(FSGUID &guid, DWORD dwItemId)
{
	int nItemIdx = m_ItemList.SearchID(dwItemId);

	if( IsItemIdxValid(nItemIdx) )
		guid = Item[nItemIdx].GetGUID();
}
#endif

//交易相关……begin

#ifndef _SERVER
//************************************
// Method:    tradeClientSendRequest
// FullName:  KPlayer::tradeClientSendRequest
// Access:    public 
// Returns:   void
// Qualifier: 客户端发送交易请求
// Parameter: int nNpcIdx
//************************************
void KPlayer::tradeClientSendRequest(int oppositePlayerNpcId)
{
	if (oppositePlayerNpcId < 0)
		return;
	TRADE_APPLY_START_COMMAND sStart;
	sStart.ProtocolType = c2s_tradeapplystart;
	sStart.m_dwID = oppositePlayerNpcId;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&sStart, sizeof(TRADE_APPLY_START_COMMAND));
}

//************************************
// Method:    tradeClientReciveRequest
// FullName:  KPlayer::tradeClientReciveRequest
// Access:    public 
// Returns:   void
// Qualifier: 功能：收到服务器通知有人申请交易
// Parameter: BYTE* pMsg
//************************************
void KPlayer::tradeClientReciveRequest(int oppositePlayerNpcId)
{
	int	nNpcIdx = NpcSet.SearchID(oppositePlayerNpcId);
	if (nNpcIdx == 0)
		return;
	
	CoreDataChanged(GDCNI_RECIVE_TRADE_REQUEST, (UINT)oppositePlayerNpcId, (INT)Npc[nNpcIdx].Name);
}

//************************************
// Method:    tradeClientAccept
// FullName:  KPlayer::tradeClientAccept
// Access:    public 
// Returns:   void
// Qualifier: 客户端发送接受交易
// Parameter: int oper
//************************************
void KPlayer::tradeClientSendAccept(int oppositePlayerNpcId)
{
	if (oppositePlayerNpcId < 0)
		return;
	TRADE_REPLY_START_COMMAND	sReply;
	sReply.ProtocolType = c2s_tradereplystart;
	sReply.m_bDecision = 1;
	sReply.oppositePlayerNpcId = oppositePlayerNpcId;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&sReply, sizeof(TRADE_REPLY_START_COMMAND));
}

//************************************
// Method:    tradeClientSendRefuse
// FullName:  KPlayer::tradeClientSendRefuse
// Access:    public 
// Returns:   void
// Qualifier: 客户端发送拒绝交易
// Parameter: int oppositePlayerNpcId
//************************************
void KPlayer::tradeClientSendRefuse(int oppositePlayerNpcId)
{
	if (oppositePlayerNpcId < 0)
		return;
	TRADE_REPLY_START_COMMAND	sReply;
	sReply.ProtocolType = c2s_tradereplystart;
	sReply.m_bDecision = 0;
	sReply.oppositePlayerNpcId = oppositePlayerNpcId;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&sReply, sizeof(TRADE_REPLY_START_COMMAND));
}

bool KPlayer::tradeClientMoveMoney(int money)
{
	// 钱数量错误
	if (money < 0 || money > m_ItemList.GetEquipmentMoney())
		return false;
	
	TRADE_MOVE_MONEY_COMMAND sMoney;
	sMoney.ProtocolType = c2s_trademovemoney;
	sMoney.m_nMoney = money;

	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&sMoney, sizeof(TRADE_MOVE_MONEY_COMMAND));
	
	return true;
}

void KPlayer::tradeClientReciveOppositeMoneyChanged(int money)
{	
	m_ItemList.SetRoomMoney(room_trade1, money);
	
	// 通知界面
	CoreDataChanged(GDCNI_TRADE_OPPOSITE_MONEY_CHANGED, (unsigned int)money, 0);
}

void KPlayer::tradeClientStateChange(int oppositePlayerNpcId, int state)
{	
	if (state == KTrade::TRADE_MSG_OPPOSITE_REFUSE)			//交易被对方拒绝
	{
		int oppositePlayerNpcIndex = NpcSet.SearchID(oppositePlayerNpcId);
		if (oppositePlayerNpcIndex == 0)
		{
			_ASSERT(0);
			return;
		}

		CoreDataChanged(GDCNI_RECIVE_TRADE_REFUSE, oppositePlayerNpcId, (UINT)Npc[oppositePlayerNpcIndex].Name);
	}
	else if (state == KTrade::TRADE_MSG_START_TRADE)		//对方接受交易，通知界面弹出交易窗口
	{
		m_ItemList.ClearRoom(room_trade1);
		
		//通知界面进入交易界面
		int oppositePlayerNpcIndex = NpcSet.SearchID(oppositePlayerNpcId);
		if (oppositePlayerNpcIndex == 0)
		{
			_ASSERT(0);
			Player[CLIENT_PLAYER_INDEX].tradeClientSendTradeCancel();
			return;
		}

		CoreDataChanged(GDCNI_TRADE_START, oppositePlayerNpcId, (UINT)Npc[oppositePlayerNpcIndex].Name);
	}
	else if (state == KTrade::TRADE_MSG_OPPOSITE_BUSY)		//对方忙
	{
		//通知界面进入交易界面
		int oppositePlayerNpcIndex = NpcSet.SearchID(oppositePlayerNpcId);
		if (oppositePlayerNpcIndex == 0)
		{
			_ASSERT(0);
			return;
		}

		CoreDataChanged(GDCNI_TRADE_OPPOSITE_BUSY, oppositePlayerNpcId, (UINT)Npc[oppositePlayerNpcIndex].Name);
	}
	else
	{
		_ASSERT(0);
	}
}


//************************************
// Method:    tradeClientSendLock
// FullName:  KPlayer::tradeClientSendLock
// Access:    public 
// Returns:   void
// Qualifier: 客户端在交易中锁定
//************************************
void KPlayer::tradeClientSendLock()
{
	TRADE_DECISION_COMMAND	sDecision;
	sDecision.ProtocolType = c2s_tradedecision;
	sDecision.m_btDecision = KTrade::TRADE_MSG_SELF_LOCK;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID, &sDecision, sizeof(TRADE_DECISION_COMMAND));
}

//************************************
// Method:    tradeClientReciveLock
// FullName:  KPlayer::tradeClientReciveLock
// Access:    public 
// Returns:   void
// Qualifier: 客户端收到服务器的锁定同步消息
// Parameter: bool self
//************************************
void KPlayer::tradeClientReciveLock(bool self)
{
	CoreDataChanged(GDCNI_TRADE_LOCK, (unsigned int)self, 0);
}


void KPlayer::tradeClientReciveUnlock()
{
	CoreDataChanged(GDCNI_TRADE_UNLOCK, 0, 0);
}

//************************************
// Method:    tradeClientEndTrade
// FullName:  KPlayer::tradeClientEndTrade
// Access:    public 
// Returns:   void
// Qualifier: 客户端发送交易确定消息给服务器
//************************************
void KPlayer::tradeClientSendEndTrade()
{
	TRADE_DECISION_COMMAND	sDecision;
	sDecision.ProtocolType = c2s_tradedecision;
	sDecision.m_btDecision = KTrade::TRADE_MSG_SELF_END_TRADE;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID, &sDecision, sizeof(TRADE_DECISION_COMMAND));
}

void KPlayer::tradeClientReciveEndTrade(bool self)
{
	CoreDataChanged(GDCNI_END_TRADE, (unsigned int)self, 0);
}

void KPlayer::tradeClientReciveTradeOk()
{
	for(int k = 0; k < TRADE_ROOM_WIDTH; k++)
	{
		for(int l = 0; l < TRADE_ROOM_HEIGHT; l++)
		{
			int index = m_ItemList.m_Room[room_trade1].FindItem(k, l);
			if(index > 0)
			{
				m_ItemList.Remove(index);
			}
		}
	}
	CoreDataChanged(GDCNI_TRADE_OK, 0, 0);
}

void KPlayer::tradeClientSendTradeCancel()
{
	TRADE_DECISION_COMMAND	sDecision;
	sDecision.ProtocolType = c2s_tradedecision;
	sDecision.m_btDecision = KTrade::TRADE_MSG_CANCEL;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID, &sDecision, sizeof(TRADE_DECISION_COMMAND));
}

void KPlayer::tradeClientReciveTradeCancel()
{
	for(int k = 0; k < TRADE_ROOM_WIDTH; k++)
	{
		for(int l = 0; l < TRADE_ROOM_HEIGHT; l++)
		{
			int index = m_ItemList.m_Room[room_trade1].FindItem(k, l);
			if(index > 0)
			{
				m_ItemList.Remove(index);
			}
		}
	}
	CoreDataChanged(GDCNI_TRADE_CANCEL, 0, 0);
}
#else

//************************************
// Method:    tradeSeverReciveRequest
// FullName:  KPlayer::tradeSeverReciveRequest
// Access:    public 
// Returns:   void
// Qualifier: 收到客户端申请开始交易
// Parameter: BYTE* pProtocol
//************************************
void KPlayer::tradeServerReciveRequest(int oppositePlayerNpcId)
{
	//检查自己是否处于交易状态
	if(m_cTrade.getState() != KTrade::TRADE_IDLE)
 		return;

	int	oppositePlayerIndex = FindAroundPlayer(oppositePlayerNpcId);
	if(oppositePlayerIndex <= 0 || oppositePlayerIndex >= MAX_PLAYER)
		return;
	//不能和自己交易
	if(oppositePlayerIndex == m_nPlayerIndex)
		return;
	//检查索引是否有效
	if (Player[oppositePlayerIndex].m_nIndex <= 0)
		return;

	//检查对方是否处于交易状态
	if(Player[oppositePlayerIndex].m_cTrade.getState() != KTrade::TRADE_IDLE)
	{
		TRADE_CHANGE_STATE_SYNC	sState;
		sState.ProtocolType = s2c_tradechangestate;
		sState.m_btState = KTrade::TRADE_MSG_OPPOSITE_BUSY;
		sState.m_dwNpcID = oppositePlayerNpcId;
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sState, sizeof(TRADE_CHANGE_STATE_SYNC));
		return;
	}

	//验证通过，将对方的玩家索引号记录
	m_cTrade.d_oppositePlayerIndex = oppositePlayerIndex;

	//向对方发送交易请求	
	TRADE_APPLY_START_SYNC s2cTradeRequest;
	s2cTradeRequest.ProtocolType = s2c_tradeapplystart;
	s2cTradeRequest.oppositePlayerNpcId = Npc[m_nIndex].m_dwID;
	if (g_pServer)
		g_pServer->PackDataToClient(Player[oppositePlayerIndex].m_nNetConnectIdx, (BYTE*)&s2cTradeRequest, sizeof(TRADE_APPLY_START_SYNC));
}

//************************************
// Method:    c2sTradeReplyStart
// FullName:  KPlayer::c2sTradeReplyStart
// Access:    public 
// Returns:   void
// Qualifier: 执行或取消交易
// Parameter: BYTE* pProtocol
//************************************
void KPlayer::tradeServerReciveAccept(int oppositePlayerNpcId)
{
	//检查状态是否为请求交易状态
	if(m_cTrade.getState() != KTrade::TRADE_IDLE)
		return;
	
	int oppositePlayerIndex = FindAroundPlayer(oppositePlayerNpcId);
	if (oppositePlayerIndex <= 0 || oppositePlayerIndex >= MAX_PLAYER)
		return;
	
	//检查目标是否可以被交易
	if(Player[oppositePlayerIndex].m_cTrade.getState() != KTrade::TRADE_IDLE)
		return;
	//目标是否存在
	if (Player[oppositePlayerIndex].m_nIndex <= 0)
		return;
	//检查对方的交易目标是否自己
	if (Player[oppositePlayerIndex].m_cTrade.d_oppositePlayerIndex != m_nPlayerIndex)
		return;

	//记录对方（请求方）的index
	m_cTrade.d_oppositePlayerIndex = oppositePlayerIndex;

	m_ItemList.m_Room[room_trade].SetMoney(0);
	Player[oppositePlayerIndex].m_ItemList.m_Room[room_trade].SetMoney(0);

	TRADE_CHANGE_STATE_SYNC	sState;
	sState.ProtocolType = s2c_tradechangestate;
	sState.m_btState = KTrade::TRADE_MSG_START_TRADE;
	sState.m_dwNpcID = oppositePlayerNpcId;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sState, sizeof(TRADE_CHANGE_STATE_SYNC));

	
	sState.m_dwNpcID = Npc[m_nIndex].m_dwID;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[oppositePlayerIndex].m_nNetConnectIdx, 
													(BYTE*)&sState, sizeof(TRADE_CHANGE_STATE_SYNC));

	//更新双方状态
	m_cTrade.setState(KTrade::TRADE_TRADING);
	Player[oppositePlayerIndex].m_cTrade.setState(KTrade::TRADE_TRADING);
	
	if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_trade_begin))
	{
		RecordPlayerActionParam param;
		param.PlayerIndex = GetPlayerIndex();
		param.Action = player_action_trade_begin;
		snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_TRADE_BEGIN, Npc[Player[oppositePlayerIndex].GetNpcIndex()].Name);
		g_PlayerMonitor.RecordPlayerAction(param);
	}
}

void KPlayer::tradeServerReciveRefuse(int oppositePlayerNpcId)
{
	int oppositePlayerIndex = FindAroundPlayer(oppositePlayerNpcId);
	if (oppositePlayerIndex <= 0 || oppositePlayerIndex >= MAX_PLAYER)
		return;
	//目标是否存在
	if (Player[oppositePlayerIndex].m_nIndex <= 0)
		return;

	TRADE_CHANGE_STATE_SYNC	sState;
	sState.ProtocolType = s2c_tradechangestate;
	sState.m_btState = KTrade::TRADE_MSG_OPPOSITE_REFUSE;
	sState.m_dwNpcID = Npc[m_nIndex].m_dwID;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[oppositePlayerIndex].m_nNetConnectIdx, 
													(BYTE*)&sState, sizeof(TRADE_CHANGE_STATE_SYNC));
}

void KPlayer::tradeServerMoveMoney(int money)
{
	//检查状态是否正确
	if(m_cTrade.getState() != KTrade::TRADE_TRADING)
		return;
	
	if (money < 0 || money > m_ItemList.GetEquipmentMoney())
		return;
	
	m_ItemList.TradeMoveMoney(money);
	tradeServerSendUnlockToOpposite();
}

void KPlayer::tradeServerReciveSelfLock()
{
	//检查状态是否正确
	if(m_cTrade.getState() != KTrade::TRADE_TRADING)
		return;

	TRADE_DECISION_SYNC	sState;
	sState.ProtocolType = s2c_tradedecision;
	sState.m_btDecision = KTrade::TRADE_MSG_OPPOSITE_LOCK;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_cTrade.d_oppositePlayerIndex].m_nNetConnectIdx, (BYTE*)&sState, sizeof(TRADE_DECISION_SYNC));
	
	sState.m_btDecision = KTrade::TRADE_MSG_SELF_LOCK;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sState, sizeof(TRADE_DECISION_SYNC));

	m_cTrade.setState(KTrade::TRADE_LOCK);

	if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_trade_lock))
	{
		RecordPlayerActionParam param;
		param.PlayerIndex = GetPlayerIndex();
		param.Action = player_action_trade_lock;
		snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_TRADE_LOCK, Npc[Player[m_cTrade.d_oppositePlayerIndex].GetNpcIndex()].Name);
		g_PlayerMonitor.RecordPlayerAction(param);
	}	
}

void KPlayer::tradeServerSendUnlockToOpposite()
{
	//检查状态是否正确
	if(m_cTrade.getState() != KTrade::TRADE_TRADING)
		return;

	TRADE_DECISION_SYNC	sState;
	sState.ProtocolType = s2c_tradedecision;
	sState.m_btDecision = KTrade::TRADE_MSG_SELF_UNLOCK;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_cTrade.d_oppositePlayerIndex].m_nNetConnectIdx, (BYTE*)&sState, sizeof(TRADE_DECISION_SYNC));
	
	Player[m_cTrade.d_oppositePlayerIndex].m_cTrade.setState(KTrade::TRADE_TRADING);
}

bool KPlayer::tradeServerReciveTradeEnd()
{	
	if(m_cTrade.getState() != KTrade::TRADE_LOCK)
		return false;

	int oppositePlayerIndex = m_cTrade.d_oppositePlayerIndex;
	if(oppositePlayerIndex == -1)
	{
		return false;
	}
	KTrade::TradeState oppositeState = Player[oppositePlayerIndex].m_cTrade.getState();
	if(oppositeState == KTrade::TRADE_ENDTRADE)
	{
		//如果对方已经确认交易		
		//检查双方物品栏能否接受买进的物品
		if(!tradeExchangePrecheck())
		{
			tradeServerDoCanceTrade();
			return false;
		}
		//执行交易
		if(!tradeProcessTrade())
		{
			return false;
		}
		TRADE_DECISION_SYNC	sState;
		sState.ProtocolType = s2c_tradedecision;
		sState.m_btDecision = KTrade::TRADE_MSG_OK;
		if (g_pServer != NULL)
		{
			g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sState, sizeof(TRADE_DECISION_SYNC));
			g_pServer->PackDataToClient(Player[m_cTrade.d_oppositePlayerIndex].m_nNetConnectIdx, (BYTE*)&sState, sizeof(TRADE_DECISION_SYNC));
		}
		
		if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_trade_done))
		{
			RecordPlayerActionParam param;
			param.PlayerIndex = GetPlayerIndex();
			param.Action = player_action_trade_done;
			snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_TRADE_DONE, Npc[Player[oppositePlayerIndex].GetNpcIndex()].Name);
			g_PlayerMonitor.RecordPlayerAction(param);
		}
		
		//清除状态
		Player[m_cTrade.d_oppositePlayerIndex].m_cTrade.initState();
		m_cTrade.initState();
	}
	else if(oppositeState == KTrade::TRADE_LOCK)
	{
		//如果对方还未确认交易，则己方先确认交易，并发送同步消息到双方客户端
		
		TRADE_DECISION_SYNC	sState;
		sState.ProtocolType = s2c_tradedecision;
		sState.m_btDecision = KTrade::TRADE_MSG_SELF_END_TRADE;
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sState, sizeof(TRADE_DECISION_SYNC));

		sState.m_btDecision = KTrade::TRADE_MSG_OPPOSITE_END_TRADE;
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(Player[oppositePlayerIndex].m_nNetConnectIdx, (BYTE*)&sState, sizeof(TRADE_DECISION_SYNC));
		
		m_cTrade.setState(KTrade::TRADE_ENDTRADE);
	}
	else
	{
		return false;
	}
	return true;
}

bool KPlayer::tradeExchangePrecheck()
{
	int oppositePlayerIndex = m_cTrade.d_oppositePlayerIndex;
	if(oppositePlayerIndex == -1)
	{
		return false;
	}

	//检查对方的交易目标是否自己(理论上如果前面的逻辑都不出错的话，这个地方不会有问题，但怕出现BUG，所以加此判断)
	if (Player[oppositePlayerIndex].m_cTrade.d_oppositePlayerIndex != m_nPlayerIndex)
		return false;

	//检查钱是否有问题
	int selfMoney = m_ItemList.GetMoney(room_equipment);
	int selfExchangeMoney = m_ItemList.GetMoney(room_trade);
	int oppositeMoney = Player[oppositePlayerIndex].m_ItemList.GetMoney(room_equipment);
	int oppositeExchangeMoney = Player[oppositePlayerIndex].m_ItemList.GetMoney(room_trade);
	
	if (selfExchangeMoney < 0 || selfMoney - selfExchangeMoney < 0)
	{
		//通知客户端钱有问题
		//this->m_ItemList.TradeMoveMoney(0);
		return false;
	}
	if (oppositeExchangeMoney < 0 || oppositeMoney - oppositeExchangeMoney < 0)
	{
		// 通知对方客户端钱有问题 not end
		//Player[nDestIdx].m_ItemList.TradeMoveMoney(0);
		return false;
	}
	//判断己方格子是否够
	int oppositeItemCount = Player[oppositePlayerIndex].m_ItemList.m_Room[room_trade].getUsedSpaceCount();
	int selfFreeSpaceCount = m_ItemList.m_Room[room_equipment].getFreeSpaceCount();
	if(oppositeItemCount > selfFreeSpaceCount)
	{
		return false;
	}
	
	//判断对方格子是否够
	int selfItemCount = m_ItemList.m_Room[room_trade].getUsedSpaceCount();
	int oppositeFreeSpaceCount = Player[oppositePlayerIndex].m_ItemList.m_Room[room_equipment].getFreeSpaceCount();
	if(selfItemCount > oppositeFreeSpaceCount)
	{
		return false;
	}

	return true;
}

bool KPlayer::tradeProcessTrade()
{
	bool needSave = false;
	int oppositePlayerIndex = m_cTrade.d_oppositePlayerIndex;
	if(oppositePlayerIndex == -1)
	{
		return false;
	}

	int nTradeSaveMinMoney = ConfigManager::Singleton().GetGlobalVariable(global_var_tradesave_minmoney);
	int selfExchangeMoney = m_ItemList.GetMoney(room_trade);
	int oppositeExchangeMoney = Player[oppositePlayerIndex].m_ItemList.GetMoney(room_trade);

	//交易中金钱的交换
	if(selfExchangeMoney < oppositeExchangeMoney)
	{
		int exchangeMoney = oppositeExchangeMoney - selfExchangeMoney;
		Earn(exchangeMoney, false);
		Player[oppositePlayerIndex].Pay(exchangeMoney, false);

		if (exchangeMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_trade_money_amount))
		{
			LogEventParam logEventParam;
			logEventParam.event = log_event_trade_money;
			logEventParam.param1 = Player[oppositePlayerIndex].GetGUID();
			logEventParam.param2 = GetGUID();
			logEventParam.param4 = exchangeMoney;
			g_pLogSystem->Log(logEventParam);
		}
	}
	else if(selfExchangeMoney > oppositeExchangeMoney)
	{
		int exchangeMoney = selfExchangeMoney - oppositeExchangeMoney;
		Pay(exchangeMoney, false);
		Player[oppositePlayerIndex].Earn(exchangeMoney, false);

		if (exchangeMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_trade_money_amount))
		{
			LogEventParam logEventParam;
			logEventParam.event = log_event_trade_money;
			logEventParam.param1 = GetGUID();
			logEventParam.param2 = Player[oppositePlayerIndex].GetGUID();
			logEventParam.param4 = exchangeMoney;
			g_pLogSystem->Log(logEventParam);
		}
	}

	//判断金钱是否满足存盘
	if( (selfExchangeMoney >= nTradeSaveMinMoney) || (oppositeExchangeMoney >= nTradeSaveMinMoney) )
	{
		needSave = true;
	}
	
	for(int i = 0; i < TRADE_ROOM_WIDTH; i++)
	{
		for(int j = 0; j < TRADE_ROOM_HEIGHT; j++)
		{
			int index = Player[oppositePlayerIndex].m_ItemList.m_Room[room_trade].FindItem(i, j);	
			if(index > 0)
			{
				//ItemDebugLog Begin........................
				ItemPos oldPos;
				BOOL bHave = Player[oppositePlayerIndex].m_ItemList.GetItemPos(index,&oldPos);
				if (!bHave || oldPos.nPlace != pos_traderoom || Item[index].GetBelong() != oppositePlayerIndex)
				{
					char szDumpInfo[512] = "";
					snprintf(szDumpInfo,sizeof(szDumpInfo),"Invalid ProcessTrade Opposite:PlayerIndex:%d,Belong:%d,ItemName:%s,PlayerName: %s\n",oppositePlayerIndex,Item[index].GetBelong(),Item[index].GetName(),Player[oppositePlayerIndex].GetPlayerName());
					szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
					DumpInvalidItemOpeStack(false,szDumpInfo,4);

				}//endif

			    //ItemDebugLog End.........................

				Player[oppositePlayerIndex].m_ItemList.Remove(index, 0, true, true, false, false);
				Player[oppositePlayerIndex].m_ItemList.syncSelfItem(pos_traderoom, i, j);
				POINT pos = m_ItemList.m_Room[room_equipment].findEmptySpace();
				m_ItemList.Add(index, pos_equiproom, pos.x, pos.y, NULL, item_sync_type_trade);

				KItem& tradeItem = Item[index];
				//判断是否贵重物品
				if(!needSave && KItemList::importantItem(&Item[index]))
				{
					needSave = true;
				}

				//统计：交易物品
				ItemTemplateId templateId;
				tradeItem.GetItemTemplateId(templateId);
				GetPlayerStatistic().AddItem(templateId, tradeItem.GetItemCount(), item_count_type_trade_in);
				Player[oppositePlayerIndex].GetPlayerStatistic().AddItem(templateId, tradeItem.GetItemCount(), item_count_type_trade_out);
				
				//日志：交易物品
				if (tradeItem.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
				{
					LogEventParam logEventParam;
					logEventParam.event = log_event_trade_item;
					logEventParam.param1 = Player[oppositePlayerIndex].GetGUID();
					logEventParam.param2 = tradeItem.GetGUID();
					logEventParam.param3 = GetGUID();
					g_pLogSystem->Log(logEventParam);
				}

				if ( tradeItem.GetGenre() == item_ib )
				{
					KIBLog::getSingleton().TransferIBItem( ib_trade_exchange, tradeItem.GetGUID(), oppositePlayerIndex, GetPlayerIndex() );
				}
			}
		}
	}
	
	for(int k = 0; k < TRADE_ROOM_WIDTH; k++)
	{
		for(int l = 0; l < TRADE_ROOM_HEIGHT; l++)
		{
			int index = m_ItemList.m_Room[room_trade].FindItem(k, l);
			if(index > 0)
			{
				//ItemDebugLog Begin........................
				ItemPos oldPos;
				BOOL bHave = m_ItemList.GetItemPos(index,&oldPos);
				if (!bHave || oldPos.nPlace != pos_traderoom || Item[index].GetBelong() != GetPlayerIndex())
				{	
					char szDumpInfo[512] = "";
					snprintf(szDumpInfo,sizeof(szDumpInfo),"Invalid ProcessTrade Self:PlayerIndex:%d,Belong:%d,ItemName:%s,PlayerName: %s\n",GetPlayerIndex(),Item[index].GetBelong(),Item[index].GetName(),GetPlayerName());
					szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
					DumpInvalidItemOpeStack(false,szDumpInfo,4);
                   	
				}//endif
				 //ItemDebugLog End.........................

				m_ItemList.Remove(index, 0, true, true, false, false);
				m_ItemList.syncSelfItem(pos_traderoom, k, l);
				POINT pos = Player[oppositePlayerIndex].m_ItemList.m_Room[room_equipment].findEmptySpace();
				Player[oppositePlayerIndex].m_ItemList.Add(index, pos_equiproom, pos.x, pos.y, NULL, item_sync_type_trade);

				KItem& tradeItem = Item[index];
				//判断是否贵重物品
				if(!needSave && KItemList::importantItem(&Item[index]))
				{
					needSave = true;
				}

				//统计：交易物品
				ItemTemplateId templateId;
				tradeItem.GetItemTemplateId(templateId);
				GetPlayerStatistic().AddItem(templateId, tradeItem.GetItemCount(), item_count_type_trade_out);
				Player[oppositePlayerIndex].GetPlayerStatistic().AddItem(templateId, tradeItem.GetItemCount(), item_count_type_trade_in);
				
				//日志：交易物品
				if (tradeItem.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
				{
					LogEventParam logEventParam;
					logEventParam.event = log_event_trade_item;
					logEventParam.param1 = GetGUID();
					logEventParam.param2 = tradeItem.GetGUID();
					logEventParam.param3 = Player[oppositePlayerIndex].GetGUID();
					g_pLogSystem->Log(logEventParam);
				}
			}
		}
	}

	//InstantSave
	//if(needSave)
	//{
	//	SaveItemData();
	//	Player[oppositePlayerIndex].SaveItemData();
	//}

	return true;
}

bool KPlayer::tradeServerReciveCancel()
{
	tradeServerDoCanceTrade();
	return true;
}

bool KPlayer::tradeServerDoCanceTrade()
{
	if(m_cTrade.getState() == KTrade::TRADE_IDLE)
		return false;

	if(!IsValidPlayer(m_cTrade.d_oppositePlayerIndex))
	{
		return false;
	}

	for(int k = 0; k < TRADE_ROOM_WIDTH; k++)
	{
		for(int l = 0; l < TRADE_ROOM_HEIGHT; l++)
		{
			int index = m_ItemList.m_Room[room_trade].FindItem(k, l);
			if(index > 0)
			{
				//ItemDebugLog Begin........................
				ItemPos oldPos;
				BOOL bHave = m_ItemList.GetItemPos(index,&oldPos);
				if (!bHave || oldPos.nPlace != pos_traderoom || Item[index].GetBelong() != GetPlayerIndex())
				{
					char szDumpInfo[512] = "";
					snprintf(szDumpInfo,sizeof(szDumpInfo),"Invalid CancelTrade Self:PlayerIndex:%d,Belong:%d,ItemName:%s,PlayerName: %s\n",GetPlayerIndex(),Item[index].GetBelong(),Item[index].GetName(),GetPlayerName());
					szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
					DumpInvalidItemOpeStack(false,szDumpInfo,4);
					
				}//endif
				//ItemDebugLog End.........................
				
				m_ItemList.Remove(index, 0, true, true, false, false);
				m_ItemList.syncSelfItem(pos_traderoom, k, l);
				POINT pos = m_ItemList.m_Room[room_equipment].findEmptySpace();
				m_ItemList.Add(index, pos_equiproom, pos.x, pos.y);
			}

			index = Player[m_cTrade.d_oppositePlayerIndex].m_ItemList.m_Room[room_trade].FindItem(k, l);
			if(index > 0)
			{
				//ItemDebugLog Begin........................
				ItemPos oldPos;
				BOOL bHave = Player[m_cTrade.d_oppositePlayerIndex].m_ItemList.GetItemPos(index,&oldPos);
				if (!bHave || oldPos.nPlace != pos_traderoom || Item[index].GetBelong() != m_cTrade.d_oppositePlayerIndex)
				{
					char szDumpInfo[512] = "";
					snprintf(szDumpInfo,sizeof(szDumpInfo),"Invalid CancelTrade Opposite:PlayerIndex:%d,Belong:%d,ItemName:%s,PlayerName: %s\n",m_cTrade.d_oppositePlayerIndex,Item[index].GetBelong(),Item[index].GetName(),Player[m_cTrade.d_oppositePlayerIndex].GetPlayerName());
					szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
					DumpInvalidItemOpeStack(false,szDumpInfo,4);
					
				}//endif
				//ItemDebugLog End.........................

				Player[m_cTrade.d_oppositePlayerIndex].m_ItemList.Remove(index, 0, true, true, false, false);
				Player[m_cTrade.d_oppositePlayerIndex].m_ItemList.syncSelfItem(pos_traderoom, k, l);
				POINT pos = Player[m_cTrade.d_oppositePlayerIndex].m_ItemList.m_Room[room_equipment].findEmptySpace();
				Player[m_cTrade.d_oppositePlayerIndex].m_ItemList.Add(index, pos_equiproom, pos.x, pos.y);
			}
		}
	}

	
	TRADE_DECISION_SYNC	sState;
	sState.ProtocolType = s2c_tradedecision;
	sState.m_btDecision = KTrade::TRADE_MSG_CANCEL;
	if (g_pServer != NULL)
	{
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&sState, sizeof(TRADE_DECISION_SYNC));
		g_pServer->PackDataToClient(Player[m_cTrade.d_oppositePlayerIndex].m_nNetConnectIdx, (BYTE*)&sState, sizeof(TRADE_DECISION_SYNC));
	}
	
	if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_trade_cancel))
	{
		RecordPlayerActionParam param;
		param.PlayerIndex = GetPlayerIndex();
		param.Action = player_action_trade_cancel;
		snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_TRADE_CANCEL, Npc[Player[m_cTrade.d_oppositePlayerIndex].GetNpcIndex()].Name);
		g_PlayerMonitor.RecordPlayerAction(param);
	}	
	
	Player[m_cTrade.d_oppositePlayerIndex].m_cTrade.initState();
	m_cTrade.initState();
	return true;
}

#endif

//交易相关……end

//储物箱锁定、解锁……begin
#ifdef _SERVER
void KPlayer::lockStoreBox()
{
	m_ItemList.LockStorageBox();
}

bool KPlayer::isStoreBoxLocked()
{
	return m_ItemList.IsLockStorageBox() == TRUE ?true:false;
}

bool KPlayer::checkStoreBox()
{
	if (m_ItemList.IsLockStorageBox())
	{	
		BOOL           bPasswordExist = IsExistBoxPassword();
		
		if(!bPasswordExist)
		{
			//如果没有密码，则自动解锁
			unlockStoreBox();
			return false;
		}//endif	
		else
		{	
			OPENSTOREBOX   tagOpenStorage;
			tagOpenStorage.ProtocolType   = s2c_openstorebox;
		    tagOpenStorage.byNeedPassword = IsExistBoxPassword();

			//如果有密码而且已经锁定，则界面需要输入密码才能操作
			tagOpenStorage.byNeedPassword = 2;	//2表示有密码、已锁定
			
			if (g_pServer != NULL)
				g_pServer->PackDataToClient(m_nNetConnectIdx, &tagOpenStorage, sizeof(OPENSTOREBOX));

			return true;
		}//end else

	}//endif
	else
		return false;

}

void KPlayer::unlockStoreBox()
{
	m_ItemList.LockStorageBox(FALSE);
}

#endif
//储物箱锁定、解锁……end

#ifdef _SERVER

#define      WORLD_COMBAT_INFO_SYNC_INTERVAL (GAME_FPS * 10)

void KPlayer::CheckSendWorldCombatInfo()
{
	if (IsValidNpc(m_nIndex) && Npc[m_nIndex].IsInWorldCombatInstance() && IsValidCombatID(Npc[m_nIndex].m_WorldCombatOrg))
	{
		if ( (g_SubWorldSet.GetGameTime() % WORLD_COMBAT_INFO_SYNC_INTERVAL == 0) )
		{
			int nSubWorldIndex = Npc[m_nIndex].GetSubWorldIndex();
			if (nSubWorldIndex != INVALID_WORLD_INDEX && nSubWorldIndex >=0 && nSubWorldIndex < MAX_SUBWORLD)
			{
				if((SubWorld[nSubWorldIndex].GetCombatScoreCalcType() == PROGRAME_CALU) || (SubWorld[nSubWorldIndex].GetCombatScoreCalcType() == SCRIPT_CALU))
				{
					WORLD_COMBAT_INFO     info;
					memset(&info,0,sizeof(info));
					
					info.Protocol          =  s2c_world_combat_info;
					
					for (int n = 0 ;n < MAX_SCORE_ORG_SYNC && n< MAX_COMBAT_ORG_NUM ; n++)
					{
						const CombatOrgnize * pInfo  =  SubWorld[nSubWorldIndex].GetCombatInstanceOrgInfo(n + 1);
						
						if (pInfo)
						{
							info.nScore[n]            =  pInfo->nScore;
						}//end for n
						
					}//endif
					
					if (g_pServer != NULL)
						g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&info, sizeof(WORLD_COMBAT_INFO));
				}
				
			}//endif
			
		}//endif
		
	}//endif
}

void KPlayer::SetCombatInfoOrg(const unsigned long dwOrgId)
{
	if (IsValidCombatID(dwOrgId))
	{
		if (IsValidNpc(m_nIndex))
		{
			m_CombatScoreOneTime           = 0;
			Npc[m_nIndex].m_WorldCombatOrg = dwOrgId;
			Npc[m_nIndex].SendSyncData(m_nNetConnectIdx,m_nIndex );
		}//endif
		
	}//endif
	
}

bool KPlayer::DecCombatInfoScore(const int nDecScore , LogEvent logEvent, BOOL bEffectOrgScore)
{
	if (nDecScore > 0 && IsValidNpc(m_nIndex) && IsValidPlayer(m_nPlayerIndex) )
	{
		int nRealDec = 0 ;
		
		if ( m_CombatInfo.nScore >= nDecScore)
		{
			nRealDec = nDecScore;
		}
		else
			nRealDec = m_CombatInfo.nScore;	
		
		if ( nRealDec > 0)
		{
			if (nRealDec >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_combat_score_dec) && g_pLogSystem)
			{
				LogEventParam decScoreEvent;
				decScoreEvent.event  = logEvent;
				decScoreEvent.param1 = GetGUID();
				decScoreEvent.param4 = -nDecScore;
				g_pLogSystem->Log(decScoreEvent);
			}//endif
			
			m_CombatInfo.nScore -= nRealDec;
			SyncAttribute(attr_combatscore);
		}//endif
		
		int nSubworldIndex = Npc[m_nIndex].GetSubWorldIndex();
		if (   nSubworldIndex != INVALID_WORLD_INDEX 
			&& nSubworldIndex <  MAX_SUBWORLD 
			&& SubWorld[nSubworldIndex].IsWorldCombatMap()
			&& IsValidCombatID(Npc[m_nIndex].m_WorldCombatOrg)
			&& bEffectOrgScore
			)
		{
			DecOnecCombatScore(nDecScore);
			SubWorld[Npc[m_nIndex].m_SubWorldIndex].DecCombatInstanceOrgScore(Npc[m_nIndex].m_WorldCombatOrg, nDecScore);	
		}//endif
		
	
		
		return true;
		
	}//endif
	else
		return false;
}

bool KPlayer::GetCamoflag()
{
	if (IsValidNpc(m_nIndex))
	{
		return (Npc[m_nIndex].GetComoflag() > 0 );
	}//endif

	return false;
}

void KPlayer::SetComoflag(const bool bSet)
{
	if (IsValidNpc(m_nIndex))
	{
		int nSet = g_Random(MAX_NPC_PRIVATE_STATE) + 1;
		if (bSet)
			Npc[m_nIndex].SetCamoflag(nSet);
		else
			Npc[m_nIndex].SetCamoflag(0);
	}//endif
}

void KPlayer::AddCombatScore(const int nAddScore ,LogEvent logEvent, BOOL bEffectOrgScore)
{
	if (nAddScore > 0 && IsValidNpc(m_nIndex) && IsValidPlayer(m_nPlayerIndex) )
	{
		int nNewScore  = m_CombatInfo.nScore + nAddScore;
		int nMaxScore  = KWorldCombatSetting::Singleton().GetMaxScoreByLevel(GetLevel()); 
		
		if (nNewScore < 0 || nNewScore < m_CombatInfo.nScore || nNewScore > nMaxScore)
		{
			if (nMaxScore > 0)
				nNewScore  =  nMaxScore;
			else
				nNewScore = 0;
		}//endif
		
		int nRealAdded      = nNewScore - m_CombatInfo.nScore;
		
		if (nRealAdded > 0)
		{
			if (nRealAdded >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_combat_score_add) && g_pLogSystem)
			{
				LogEventParam addScoreEvent;
				addScoreEvent.event  = logEvent;
				addScoreEvent.param1 = GetGUID();
				addScoreEvent.param4 = nRealAdded;
				g_pLogSystem->Log(addScoreEvent);
			}//endif
			
			m_CombatInfo.nScore = nNewScore;
			//Show Info to Client
			SyncAttribute(attr_combatscore);
			
		}//endif
		
		int nSubworldIndex = Npc[m_nIndex].GetSubWorldIndex();
		if (   nSubworldIndex != INVALID_WORLD_INDEX 
			&& nSubworldIndex < MAX_SUBWORLD 
			&& SubWorld[nSubworldIndex].IsWorldCombatMap()
			&& IsValidCombatID(Npc[m_nIndex].m_WorldCombatOrg)
			&& bEffectOrgScore
			)
		{
			AddOnceCombatScore(nAddScore);
			SubWorld[nSubworldIndex].AddCombatInstanceOrgScore(Npc[m_nIndex].m_WorldCombatOrg, nAddScore);	
		}//endif
		
		
	}//endif
}


/*void KPlayer::FleshTop10(const int nScore, int nSubworldIndex)
{
	int nOrgId = Npc[m_nIndex].m_WorldCombatOrg;
	CombatOrgnize *pOrgInfo = const_cast<CombatOrgnize *> (SubWorld[nSubworldIndex].GetCombatInstanceOrgInfo(nOrgId));
	bool binTop10 = FALSE;
	int ninTop10index = 0;

	for(int i = 0; i<MAX_COMBAT_TOP; ++i)
	{
		if(m_nPlayerIndex == pOrgInfo->top10playerIndexArray[i])
		{
			binTop10 = TRUE;
			ninTop10index = i;
			break;
		}
	}

	m_CombatScore += nScore;
	if (binTop10)//该玩家已经存在与Top10列表中(可能性最大)
	{
		int i = 0;
		if(ninTop10index == 0)
			return;
		else
			i = ninTop10index - 1;

		while (i >= 0 && m_CombatScore > Player[pOrgInfo->top10playerIndexArray[i]].m_CombatScore)
		{
			pOrgInfo->top10playerIndexArray[i + 1] = pOrgInfo->top10playerIndexArray[i];
			--i;
		}

		pOrgInfo->top10playerIndexArray[i + 1] = m_nPlayerIndex;
	}
	else//玩家进入Top10
	{
		if (pOrgInfo->topused == 10)//
		{
			if (m_CombatScore > Player[pOrgInfo->top10playerIndexArray[MAX_COMBAT_TOP - 1]].m_CombatScore) //如果该玩家分数大于排名列表中排名最低的玩家的分数，才更新排名列表
			{
				int i = MAX_COMBAT_TOP - 2;
				while (i >= 0 && m_CombatScore > Player[pOrgInfo->top10playerIndexArray[i]].m_CombatScore)
				{
					pOrgInfo->top10playerIndexArray[i + 1] = pOrgInfo->top10playerIndexArray[i];
					--i;
				}
				
				pOrgInfo->top10playerIndexArray[i + 1] = m_nPlayerIndex;
			}
			else
			{	
				//do nothing
			}//end of if
		}
		else
		{
			if( pOrgInfo->topused >= 0 && pOrgInfo->topused < 10 )
			{
				pOrgInfo->top10playerIndexArray[pOrgInfo->topused] = m_nPlayerIndex;
				pOrgInfo->topused += 1;
			}
		}//end of if
	}//end of if
}
*/
#endif

//玩家移动……begin
void KPlayer::onPlayerRun()
{
#ifdef _SERVER
	m_ItemList.closeStoreBox();
#else
	CoreDataChanged(GDCNI_PLAYER_RUN, NULL, NULL);
#endif
}
//玩家移动……end

//得到当前等级的经验最大值
DWORD KPlayer::GetExpMax() const
{
	const LevelUpAdd* levelUp = KLevelUpInfo::Singleton().GetLevelUpAdd(GetLevel(), GetSeries(), GetSkillSeries());
	if (levelUp != NULL)
	{
		return levelUp->Exp;
	}
	else
	{
		//_ASSERT(false);
		return 0;
	}	
}

//得到当前等级的技能经验最大值
DWORD KPlayer::GetSkillExpMax() const
{
	const LevelUpAdd* levelUp = KLevelUpInfo::Singleton().GetLevelUpAdd(GetLevel(), GetSeries(), GetSkillSeries());
	if (levelUp != NULL)
	{
		return levelUp->SkillExp;
	}
	else
	{
		//_ASSERT(false);
		return 0;
	}	
}

#ifdef _SERVER
//同步属性
void KPlayer::SyncAttribute(enumSyncAttribute attr)
{
	if (attr < 0 || attr >= playerSyncAttr_Count )
		return;

	int nPlusPointIdx = 0;
	if (attr >= attr_pluspoint0 && attr <= attr_pluspoint19 )
	{
		nPlusPointIdx = attr - attr_pluspoint0;
		if ( nPlusPointIdx < 0 || nPlusPointIdx >= MAX_PLUS_POINT_COUNT )
		{
			return;
		}
	}

	SYNC_PLAYERATTR syncAttr;
	syncAttr.Protocol = s2c_sync_playerattr;
	syncAttr.Attribute = attr;

	DWORD val = 0;
	switch(attr)
	{
	case attr_Exp:
		val = GetExp();
		break;
	case attr_SkillExp:
		val = GetSkillExp();
		break;
	case attr_WeightMax:
		val = GetWeightMax();
		break;
	case attr_CanPickup:
		val = (CanPickup() ? TRUE : FALSE);
		break;
	case attr_IsBlockClientControl:
		val = (IsBlockClientControl() ? TRUE : FALSE);
		break;
	case attr_pkmode:
		val = Npc[m_nIndex].m_UnaryAttrMgr[nuai_pkmode];
		break;
	case attr_jinshanbi:
		val = GetJinshanbi(  );
		break;
	case attr_maxcreditpoint:
		{
			int min = 0;
			int max = 0;
			GetIBMoneySize( creditpoint, min, max );
			val = max;
		}		
		break;
	case attr_creditstate:
		val = GetCreditState();
		break;
	case attr_creditreturndata:
		val = GetCreditReturnTime();
		break;
	case attr_creditpoint:
		val = GetIBMoney( creditpoint );
		break;
	case attr_point:
		val = GetIBMoney( point );
		break;
	case attr_combatscore:
		val = m_CombatInfo.nScore;
		break;
	case attr_employtime:
		val = GetEmployTime();
		break;
	case attr_gmflag:
		val = (m_IsGM ? TRUE : FALSE);
		break;
	case attr_passward_state:
		val = IsExistBoxPassword();
		break;

	case attr_pluspoint0:
	case attr_pluspoint1:
	case attr_pluspoint2:
	case attr_pluspoint3:
	case attr_pluspoint4:
	case attr_pluspoint5:
	case attr_pluspoint6:
	case attr_pluspoint7:
	case attr_pluspoint8:
	case attr_pluspoint9:
	case attr_pluspoint10:
	case attr_pluspoint11:
	case attr_pluspoint12:
	case attr_pluspoint13:
	case attr_pluspoint14:
	case attr_pluspoint15:
	case attr_pluspoint16:
	case attr_pluspoint17:
	case attr_pluspoint18:
	case attr_pluspoint19:
		val = m_plusPointArray[nPlusPointIdx];
		break;
	default:
		_ASSERT(false);
		return;
	}

	syncAttr.Value = val;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&syncAttr, sizeof(SYNC_PLAYERATTR));
}

void KPlayer::BroadCastSocialInfo()
{
	RelationRecord* pRecord = m_RelationSet.GetRelationByTemplate(enSUTplId_Tong);
	if (pRecord && pRecord->pLeafUnit)
	{	
		S2C_SOCIAL_INFO_BROAD_CAST info;
		memset(&info,0,sizeof(info));
		
		info.nNpcID = Npc[GetNpcIndex()].GetId();

		info.comHeader.proHeader.protocol    = s2c_social_family;
		info.comHeader.proHeader.subProtocol = enSRProtocol_InfoBroadCast;
		info.comHeader.proHeader.len         = sizeof(S2C_SOCIAL_INFO_BROAD_CAST) - PROTOCOL_SIZE;
		
		SocialUnit * pGensUnit               = GetUpNUnit(pRecord->pLeafUnit,enSULayer_Gens);
		if (pGensUnit && GetUnitName(pGensUnit->GetUnitAttr()))
		{
			strncpy(info.szGensName,GetUnitName(pGensUnit->GetUnitAttr()),sizeof(info.szGensName));
			if (pGensUnit->IsOwner(GetPlayerName()))
				info.isGensOwner = 1;
		}//endif

		SocialUnit * pTongUnit               = GetUpNUnit(pRecord->pLeafUnit,enSULayer_Tong);
		if (pTongUnit && GetUnitName(pTongUnit->GetUnitAttr()))
		{
			strncpy(info.szTongName,GetUnitName(pTongUnit->GetUnitAttr()),sizeof(info.szTongName));
			if (pTongUnit->IsOwner(GetPlayerName()))
				info.isTongOwner = 1;
		}//endif

		int nNpcIndex = GetNpcIndex();
		int nCount    = MAX_BROADCAST_COUNT_MIN;

		Npc[nNpcIndex].BroadCastRegion(&info,info.comHeader.proHeader.len + PROTOCOL_SIZE,nCount);

	}//endif
	
}

#define  SOCIAL_RELATION_BUFF_SIZE 2048

void KPlayer::SyncSocialRelation(int templateId)
{
	char                 szMsgBuff[SOCIAL_RELATION_BUFF_SIZE];

	if (SOCIAL_RELATION_BUFF_SIZE < sizeof(SYNC_SOCIAL_RELATION) - 1 + sizeof(SYNC_SOCIAL_RELATION_INFO))
	{
		_ASSERT(false);
		return;
	}//endif
	
	SYNC_SOCIAL_RELATION         *syncRelation = (SYNC_SOCIAL_RELATION *)szMsgBuff;
	PSYNC_SOCIAL_RELATION_INFO    pRelationInfo= (PSYNC_SOCIAL_RELATION_INFO)syncRelation->data;

	syncRelation->Protocol = s2c_social_relation;
	pRelationInfo->TemplateId = templateId;
	pRelationInfo->TopLayer = 0;
	pRelationInfo->PrivilegeCount = 0;
	pRelationInfo->CityMapId = INVALID_WORLD_ID;
	memset(pRelationInfo->Privileges, 0, sizeof(pRelationInfo->Privileges));
	memset(pRelationInfo->Names, 0, sizeof(pRelationInfo->Names));

	RelationRecord* pRecord = m_RelationSet.GetRelationByTemplate(templateId);

	if (pRecord != NULL)
	{
		for(int nLayer = 0; nLayer < MAX_SOCIETY_LAYER_COUNT; ++nLayer)
		{
			pRelationInfo->OwnerFlag[nLayer] = 0;

			SocialUnit *pUnit = GetUpNUnit(pRecord->pLeafUnit, nLayer);			
			if(pUnit)
			{
				const char* pUnitName = GetUnitName( pUnit->GetUnitAttr() );
				if (pUnitName != NULL)
				{
					strncpy(pRelationInfo->Names[nLayer], pUnitName, 17);
				}

				if( pUnit->IsOwner( Npc[m_nIndex].Name ) )
					pRelationInfo->OwnerFlag[nLayer] = 1;

				if(enSULayer_League == nLayer)
					pRelationInfo->CityMapId = GetCityMapId( pUnit->GetUnitAttr() );
			}
		}

		SocialUnit* pTopUnit = GetTopUnit(pRecord->pLeafUnit);

		if(pTopUnit && pTopUnit->GetLayer() > enSULayer_Player)
		{
			pRelationInfo->TopLayer = pTopUnit->GetLayer();

			PrivilegeSet& privilegeSet = pRecord->pLeafUnit->GetPrivilegeSet();
			int privilegeCount = privilegeSet.GetPrivilegeCount();
			pRelationInfo->PrivilegeCount = privilegeCount;
			
			for (int privilegeLoopCount = 0; privilegeLoopCount < privilegeCount; privilegeLoopCount++)
			{
				const PRelationPrivilege pPrivilege = privilegeSet.GetPrivilege(privilegeLoopCount);
				if (pPrivilege != NULL)
				{
					pRelationInfo->Privileges[privilegeLoopCount].Layer = pPrivilege->Layer;
					pRelationInfo->Privileges[privilegeLoopCount].OperationId = pPrivilege->OperationId;
				}			
			}
		}
	}

	syncRelation->Len = sizeof(SYNC_SOCIAL_RELATION) - 1 + sizeof(SYNC_SOCIAL_RELATION_INFO) - PROTOCOL_SIZE;

	//Compression verion.......................................................................................
	int nOldSize = sizeof(SYNC_SOCIAL_RELATION_INFO);
	
	unsigned char szCompressionBuff[SOCIAL_RELATION_BUFF_SIZE];
	unsigned int nLen = SOCIAL_RELATION_BUFF_SIZE;
	lzo1x_1_compress( 
		(const unsigned char *)syncRelation->data,
		nOldSize,
		szCompressionBuff,
		&nLen,
		wrkmem);
	
	if (nLen >= SOCIAL_RELATION_BUFF_SIZE - sizeof(SYNC_SOCIAL_RELATION) )
		return;
	
	memcpy(syncRelation->data,szCompressionBuff,nLen);          
	syncRelation->Len = nLen + sizeof(SYNC_SOCIAL_RELATION) - 1 - PROTOCOL_SIZE;
	//Compression end .........................................................................................

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_nNetConnectIdx, szMsgBuff, syncRelation->Len + PROTOCOL_SIZE);
}

void KPlayer::AddExp(int exp, int tragetLevel)
{
	if (exp <= 0 || exp > MAX_ADD_EXP || tragetLevel <= 0)
		return;
	
	DWORD actualAddExp = 0;
	if (GetLevel() - tragetLevel <= 10)
	{
		actualAddExp = (DWORD)exp;
	}

	//防沉迷
	int nAntiEnthrallState = m_AntiEnthrall.GetCurState();

	if(AntiEnthrall::enAntiEnthrall_Weariness == nAntiEnthrallState)
		actualAddExp /= AntiEnthrall::WEARINESS_EXP_SCALE;
	else if(AntiEnthrall::enAntiEnthrall_Insalubrity == nAntiEnthrallState)
		return;

	//计算经验获得百分比
	int expPercentage = m_nExpPercentage + ConfigManager::Singleton().GetGlobalVariable(global_var_exp_percentage);
	if (expPercentage <= 0)
		return;

	//根据实际经验获得百分比进行经验加成
	actualAddExp = (actualAddExp < 1000000) ? (actualAddExp * (DWORD)expPercentage / 100) : (actualAddExp / 100 * (DWORD)expPercentage);

	if (actualAddExp > 0)
	{
		if (actualAddExp >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_exp_amount) && g_pLogSystem)
		{
			LogEventParam addExpEvent;
			addExpEvent.event = log_event_exp_manager_add_exp;
			addExpEvent.param1 = GetGUID();
			addExpEvent.param4 = actualAddExp;
			g_pLogSystem->Log(addExpEvent);
		}
		
		m_ExpInsuranceMgr.ContributeExp( actualAddExp );

		DirectAddExp(actualAddExp, true);
	}
}

void KPlayer::QuestAddExp(int exp)
{
	if (exp <= 0 || exp > MAX_ADD_EXP)
		return;
	
	DWORD actualAddExp = (DWORD)exp;

	//防沉迷
	int nAntiEnthrallState = m_AntiEnthrall.GetCurState();

	if(AntiEnthrall::enAntiEnthrall_Weariness == nAntiEnthrallState)
		actualAddExp /= AntiEnthrall::WEARINESS_EXP_SCALE;
	else if(AntiEnthrall::enAntiEnthrall_Insalubrity == nAntiEnthrallState)
		return;

	//计算经验获得百分比
	int expPercentage = m_nQuestExpPercentage + ConfigManager::Singleton().GetGlobalVariable(global_var_quest_exp_percentage);
	if (expPercentage <= 0)
		return;

	//根据实际经验获得百分比进行经验加成
	actualAddExp = (actualAddExp < 1000000) ? (actualAddExp * (DWORD)expPercentage / 100) : (actualAddExp / 100 * (DWORD)expPercentage);
	
	if (actualAddExp > 0)
	{
		m_ExpInsuranceMgr.ContributeExp( actualAddExp );
		DirectAddExp(actualAddExp, true);
	}
}

void KPlayer::InternalDirectAddExp(DWORD addExp)
{
	if (addExp == 0 || addExp > MAX_ADD_EXP || m_nExp > MAX_EXP || GetLevel() >= MAX_LEVEL)
		return;

	//获取经验日志
	if (addExp >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_exp_amount))
	{
		LogEventParam addExpEvent;
		addExpEvent.event = log_event_add_exp;
		addExpEvent.param1 = GetGUID();
		addExpEvent.param4 = addExp;
		g_pLogSystem->Log(addExpEvent);
	}

	m_PlayerStatistic.ChangeExp(addExp);
	
	m_nExp += addExp;
	
	//这里同步一次经验值是为了让客户端能够收到正确的经验获取信息
	SyncAttribute(attr_Exp);
	
	while (m_nExp > 0 && m_nExp >= GetExpMax())
	{		
		LevelUp();
	}
	
	SyncAttribute(attr_Exp);

	//InstantSave
	//if (addExp >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_exp_amount))
	//{
	//	SaveBaseInfoData();
	//}
}

void KPlayer::DirectAddExp(DWORD addExp, bool absorbExpFlag)
{
	if (addExp == 0 || addExp > MAX_ADD_EXP || m_nExp > MAX_EXP)
		return;

	if (absorbExpFlag)
	{
		//经验存储物品
		if (m_ExpItemIndex > 0 && m_ExpItemIndex < MAX_ITEM)
		{
			KItem& expItem = Item[m_ExpItemIndex];
			if (expItem.IsExpItem())
			{
				const int currentItemExp = expItem.GetDurability();
				const int maxItemExp = expItem.GetMaxDurability();
				if (currentItemExp >= 0 && maxItemExp >= 0)
				{
					DWORD playerGainExp = addExp * m_PlayerExpGainPercent / 100;
					DWORD itemGainExp = addExp * m_ItemExpGainPercent / 100;
					if (currentItemExp < maxItemExp)
					{
						int newItemExp = (currentItemExp + itemGainExp > maxItemExp) ? maxItemExp : (currentItemExp + itemGainExp);
						expItem.SetDurability(newItemExp);
						expItem.SyncAttribute(item_attr_durability, GetNetConnectIdx());
						
						InternalDirectAddExp(playerGainExp);
					}
					else
					{
						InternalDirectAddExp(playerGainExp);
						
						//通知取消物品存储经验状态
						int notifyBuffTemplateId = ConfigManager::Singleton().GetGlobalVariable(globar_var_cancel_item_exp_buff);
						if (notifyBuffTemplateId > 0)
						{
							BuffMgr::Singleton().AddNpcBuff(
								GetNpcIndex(),
								GetNpcIndex(),
								notifyBuffTemplateId);
						}
					}
					
					return;
				}
			}
		}
	}

	InternalDirectAddExp(addExp);
}

void KPlayer::AddSkillExp(int skillExp, int tragetLevel)
{
	if (skillExp <= 0 || skillExp > MAX_ADD_SKILL_EXP || tragetLevel <= 0)
		return;
	
	DWORD actualAddSkillExp = 0;
	if (GetLevel() - tragetLevel <= 10)
	{
		actualAddSkillExp = (DWORD)skillExp;
	}

	//防沉迷
	int nAntiEnthrallState = m_AntiEnthrall.GetCurState();

	if(AntiEnthrall::enAntiEnthrall_Weariness == nAntiEnthrallState)
		actualAddSkillExp /= AntiEnthrall::WEARINESS_EXP_SCALE;
	else if(AntiEnthrall::enAntiEnthrall_Insalubrity == nAntiEnthrallState)
		return;

	//计算蕴魂获得百分比
	int skillExpPercentage = m_nSkillExpPercentage + ConfigManager::Singleton().GetGlobalVariable(global_var_skill_exp_percentage);
	if (skillExpPercentage <= 0)
		return;

	//根据实际蕴魂获得百分比进行加成
	actualAddSkillExp = (actualAddSkillExp < 1000000) ? (actualAddSkillExp * (DWORD)skillExpPercentage / 100) : (actualAddSkillExp / 100 * (DWORD)skillExpPercentage);
	
	if (actualAddSkillExp > 0)
	{
		DirectAddSkillExp(actualAddSkillExp);
		
		int talismanPotentialRate = ConfigManager::Singleton().GetGlobalVariable(global_var_talisman_potential_gain_rate);
		if (talismanPotentialRate > 0)
		{
			DWORD addTalismanPotential = (actualAddSkillExp < 1000000) ? (actualAddSkillExp * talismanPotentialRate / 100) : (actualAddSkillExp / 100 * talismanPotentialRate);
			if (addTalismanPotential > 0)
			{
				AddTalismanPotential(addTalismanPotential);
			}
		}		
	}
}

void KPlayer::DirectAddSkillExp(DWORD addSkillExp)
{
	if (addSkillExp == 0 || addSkillExp > MAX_ADD_SKILL_EXP || m_nSkillExp > MAX_SKILL_EXP)
		return;
	
	m_nSkillExp += addSkillExp;

	DWORD skillExpMax = GetSkillExpMax();
	if(m_nSkillExp > skillExpMax)
		m_nSkillExp = skillExpMax;
	
	SyncAttribute(attr_SkillExp);
}

void KPlayer::PromptAddBuff(int buffSender, int buffId)
{
	if (!IsValidNpc(buffSender))
		return;

	KNpc& senderNpc = Npc[buffSender];

	PBAT pBuffTemplate = BuffTable::Singleton().GetBuff(buffId);
	if (!pBuffTemplate)
		return;

	char sendBuff[sizeof(SERVER_PROMPT) + MAX_SERVER_PROMPT_STR_PARAM_LENGTH];
	memset(sendBuff, 0, sizeof(sendBuff));
	SERVER_PROMPT* pPrompt = (SERVER_PROMPT*)sendBuff;

	pPrompt->Protocol = s2c_prompt;
	pPrompt->Length = sizeof(SERVER_PROMPT);
	pPrompt->Event = prompt_event_add_buff;
	pPrompt->Param[0] = buffId;

	switch(pBuffTemplate->nDelayAddType)
	{
	case delay_add_buff_type_transfer:
		{
			pPrompt->Param[1] = SubWorld[senderNpc.m_SubWorldIndex].m_SubWorldID;
			strncpy(pPrompt->StrParam, senderNpc.Name, MAX_SERVER_PROMPT_STR_PARAM_LENGTH);
			pPrompt->StrParam[MAX_SERVER_PROMPT_STR_PARAM_LENGTH - 1] = 0;

			pPrompt->Length += strlen(pPrompt->StrParam);
		}
		break;

	case delay_add_buff_type_revive:
		{
			strncpy(pPrompt->StrParam, senderNpc.Name, MAX_SERVER_PROMPT_STR_PARAM_LENGTH);
			pPrompt->StrParam[MAX_SERVER_PROMPT_STR_PARAM_LENGTH - 1] = 0;
			
			pPrompt->Length += strlen(pPrompt->StrParam);
		}
		break;

	default:
		break;
	}

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)pPrompt, pPrompt->Length);
}

void KPlayer::CancelPromptAddBuff()
{
	CANCEL_SERVER_PROMPT cancelPrompt;
	cancelPrompt.Protocol = s2c_cancel_prompt;
	cancelPrompt.Event = prompt_event_add_buff;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_nNetConnectIdx, (BYTE*)&cancelPrompt, sizeof(cancelPrompt));
}

bool KPlayer::DelayAddBuff(int buffSender, int buffId, int delay)
{
	DelayedAction action(this->GetPlayerIndex(), delayed_action_add_buff, delay, 0);
	DelayedActionParamAddBuff& param = action.GetAddBuffParam();
	param.m_BuffId = buffId;
	param.m_BuffSender = buffSender;
	param.m_Accept = false;
	if (!GetActionDelayer().NewAction(action, true))
		return false;

	PromptAddBuff(buffSender, buffId);

	return true;
}

void KPlayer::OnEvent(enumPlayerEvent eventType, const void* eventParam)
{
	switch(eventType)
	{
	case player_event_join_team:
		{
			ValidatePlayerState();
			g_ChatCenterS.s2cChannelOpe(GetPlayerIndex(), TEAM_ROOM_ID, chat_addchannel, CHAT_CHANNEL_NAME_TEAM);
		}
		break;
	case player_event_leave_team:
		{
			ValidatePlayerState();
			g_ChatCenterS.s2cChannelOpe(GetPlayerIndex(), TEAM_ROOM_ID, chat_delchannel, CHAT_CHANNEL_NAME_TEAM);
		}
		break;
	case player_event_team_changed:
		{
			ValidatePlayerState();
		}
		break;
	case player_event_exit_world:
		{
			int exitWorldIndex = *((int*)eventParam);

			AddExitWorldBuff();
			ClearSelectedSkill();

			if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_exit_world))
			{
				RecordPlayerActionParam param;
				param.PlayerIndex = GetPlayerIndex();
				param.Action = player_action_exit_world;
				snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_EXIT_WORLD, SubWorld[exitWorldIndex].GetWorldTemplateId());
				g_PlayerMonitor.RecordPlayerAction(param);
			}
		}
		break;
	case player_event_enter_world:
		{
			int enterWorldIndex = *((int*)eventParam);

			if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_enter_world))
			{
				RecordPlayerActionParam param;
				param.PlayerIndex = GetPlayerIndex();
				param.Action = player_action_enter_world;
				snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_ENTER_WORLD, SubWorld[enterWorldIndex].GetWorldTemplateId());
				g_PlayerMonitor.RecordPlayerAction(param);
			}

			AddEnterWorldBuff();
			ValidatePlayerState();
		}
		break;
	}
}

void KPlayer::AddEnterWorldBuff()
{
	BuffMgr& bm = BuffMgr::Singleton();
	
	int worldTemplateId = SubWorld[Npc[m_nIndex].GetSubWorldIndex()].GetWorldTemplateId();
	WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(worldTemplateId);
	if (pSetting)
	{
		const WorldBuffInfo& worldBuffInfo = pSetting->BuffInfo;
		for (int enterBuffIndex = 0; enterBuffIndex < MAX_WORLD_BUFF_COUNT; enterBuffIndex++)
		{
			int buffId = worldBuffInfo.EnterBuff[enterBuffIndex];
			if (buffId > 0)
				bm.AddNpcBuff(m_nIndex, m_nIndex, buffId);
		}
	}
}

void KPlayer::AddExitWorldBuff()
{
	BuffMgr& bm = BuffMgr::Singleton();
	
	int worldTemplateId = SubWorld[Npc[m_nIndex].GetSubWorldIndex()].GetWorldTemplateId();
	WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(worldTemplateId);
	if (pSetting)
	{
		const WorldBuffInfo& worldBuffInfo = pSetting->BuffInfo;
		for (int exitBuffIndex = 0; exitBuffIndex < MAX_WORLD_BUFF_COUNT; exitBuffIndex++)
		{
			int buffId = worldBuffInfo.ExitBuff[exitBuffIndex];
			if (buffId > 0)
				bm.AddNpcBuff(m_nIndex, m_nIndex, buffId);
		}
	}
}

void KPlayer::ValidateInstance()
{
	for (int instanceIndex = 0; instanceIndex < INSTANCE_SUBWORLD_START; instanceIndex++)
	{
		DWORD instanceId = m_InstanceInfo.GetInstanceId(instanceIndex);
		if (instanceId != INVALID_INSTANCE_ID)
		{
			int worldIndex = g_SubWorldSet.GetInstance(instanceId);
			if (worldIndex == INVALID_WORLD_INDEX)
			{
				m_InstanceInfo.SetInstanceId(instanceIndex, INVALID_INSTANCE_ID);
			}
			else
			{
				//如果这个副本需要主人，则让主人重新获得控制权
				WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(SubWorld[worldIndex].GetWorldTemplateId());
				if (pSetting)
				{
					if (enSULayer_Player == pSetting->NeedOwner)
					{
						SubWorld[worldIndex].SetOwner(GetPlayerIndex());
					}
				}
			}
		}		
	}
}

void KPlayer::ValidatePlayerState()
{
	if (m_DBLoadProcessFlag & enPDBMask_Reserve)
	{
		KNpc& npc = Npc[GetNpcIndex()];
		
		DelayedAction* pTransferAction = GetActionDelayer().GetAction(delayed_action_transfer);
		if (SubWorld[npc.GetSubWorldIndex()].CanEnter(GetPlayerIndex()))
		{		
			if (pTransferAction)
			{
				pTransferAction->Cancel();
			}
		}
		else
		{
			if (!pTransferAction)
			{
				POINT revivePos;
				if (g_SubWorldSet.GetRevivalPosFromId(m_sLoginRevivalPos.m_nSubWorldID, m_sLoginRevivalPos.m_ReviveID, &revivePos))
				{
					DelayedAction transferAction(GetPlayerIndex(), delayed_action_transfer, 30, 0);
					DelayedActionParamTransfer& param = transferAction.GetTransferParam();
					param.m_Accept = true;
					param.m_CanDeny = false;
					param.m_IsInstance = false;
					param.m_TransferID = m_sLoginRevivalPos.m_nSubWorldID;
					param.m_PosX = revivePos.x;
					param.m_PosY = revivePos.y;
					GetActionDelayer().NewAction(transferAction, true);
					ShowPredefinedMsg(11390);
				}
			}
		}
	}
}

bool KPlayer::TransferToRevivePos()
{
	int npcIndex = GetNpcIndex();
	if (!(IsValidNpc(npcIndex) && Npc[npcIndex].IsPlayer()))
		return false;

	KNpc& npc = Npc[npcIndex];
	POINT revivePos;
	if (TRUE == g_SubWorldSet.GetRevivalPosFromId(m_sLoginRevivalPos.m_nSubWorldID, m_sLoginRevivalPos.m_ReviveID, &revivePos))
	{
		return (TRUE == npc.ChangeWorld(m_sLoginRevivalPos.m_nSubWorldID, revivePos.x, revivePos.y, false));
	}

	return false;
}

#else

void KPlayer::RecvAttributeSync(BYTE attr, DWORD val)
{
	int nPlusPointIdx = 0;
	if (attr >= attr_pluspoint0 && attr <= attr_pluspoint19 )
	{
		nPlusPointIdx = attr - attr_pluspoint0;
		if ( nPlusPointIdx < 0 || nPlusPointIdx >= MAX_PLUS_POINT_COUNT )
		{
			return;
		}
	}

	switch(attr)
	{
	case attr_Exp:
		SetExp(val);
		break;
	case attr_SkillExp:
		SetSkillExp(val);
		break;
	case attr_WeightMax:
		SetWeightMax((int)val);
		break;
	case attr_CanPickup:
		SetCanPickup((TRUE == val) ? true : false);
		break;
	case attr_IsBlockClientControl:
		SetBlockClientControl((TRUE == val) ? true : false);
		break;
	case attr_pkmode:
		Npc[m_nIndex].m_UnaryAttrMgr.Set(nuai_pkmode, val);
		CoreDataChanged(GDCNI_PK_SETTING, val, 0);
		break;
	case attr_jinshanbi:
		{
			SetIBMoney( jinshanbi, &val );
			CoreDataChanged(GDCNI_RECV_JINSHANBI, val, 0);
		}
		break;
	case attr_creditpoint:
		{
			SetIBMoney( creditpoint, &val );
			CoreDataChanged(GDCNI_RECV_CREDITPOINT, val, 0);
		}
		break;
	case attr_point:
		{
			SetIBMoney( point, &val );
			CoreDataChanged(GDCNI_RECV_POINT, val, 0);
		}
		break;
	case attr_maxcreditpoint:
		{
			SetIBMoneySize( creditpoint, LONG_MIN_LIMIT, val );
			CoreDataChanged(GDCNI_RECV_MAXCREDITPOINT, val, 0);
		}
		break;
	case attr_creditstate:
		{
			SetCreditState( val );
			CoreDataChanged(GDCNI_RECV_CREDITSTATE, val, 0);
		}
		break;
	case attr_creditreturndata:
		{
			SetCreditReturnTime( val );
			CoreDataChanged(GDCNI_RECV_CREDITRETURNDATA, val, 0);
		}
		break;
	case attr_combatscore:
		if (val > m_CombatInfo.nScore)
		{
			ShowScoreGet(val - m_CombatInfo.nScore);
		}//endif
		
		m_CombatInfo.nScore = val;
		break;
	case attr_employtime:
		SetEmployTime(val);
		break;
	case attr_gmflag:
		m_IsGM = (val == TRUE);
		break;
	case attr_passward_state:
		m_IsPasswordExist = val;
		break;

	case attr_pluspoint0:
	case attr_pluspoint1:
	case attr_pluspoint2:
	case attr_pluspoint3:
	case attr_pluspoint4:
	case attr_pluspoint5:
	case attr_pluspoint6:
	case attr_pluspoint7:
	case attr_pluspoint8:
	case attr_pluspoint9:
	case attr_pluspoint10:
	case attr_pluspoint11:
	case attr_pluspoint12:
	case attr_pluspoint13:
	case attr_pluspoint14:
	case attr_pluspoint15:
	case attr_pluspoint16:
	case attr_pluspoint17:
	case attr_pluspoint18:
	case attr_pluspoint19:
		{
			m_plusPointArray[nPlusPointIdx] = val;
			CoreDataChanged(GDCNI_UPDATA_PLUS_POINT, NULL, NULL);
		}
		break;
	default:
		_ASSERT(false);
		break;
	}
}

void KPlayer::ShowScoreGet(const int nScoreGet)
{
	Npc[m_nIndex].GetCombatInfoShower().AddInfo(m_nIndex,nScoreGet,0,COMBAT_INFO_SCORE_GET,false);
}

#endif

void KPlayer::SetWeightMax(int nWeightMax)
{
	_ASSERT(nWeightMax >= 0);
	m_nWeightMax = nWeightMax;
	CheckWeight();
	
#ifndef _SERVER
	CoreDataChanged(GDCNI_PLAYER_WEIGHT_CHANGED, (unsigned int)GetWeightTaken(), GetWeightMax());	
#endif
}

void KPlayer::SetExp(DWORD exp)
{
#ifndef _SERVER
	//通知界面
	if (exp > m_nExp)
	{
		DWORD addExp = exp - m_nExp;
		char msgBuff[256] = { 0 };
		char expAddMsg[256] = { 0 };
		char expAddMsgTemplate[256] = { 0 };
		g_GetStringRes(sid_self_add_exp, expAddMsgTemplate, sizeof(expAddMsgTemplate));
		sprintf(expAddMsg, expAddMsgTemplate, addExp);
		sprintf(msgBuff, "<Seg float=wrap><Obj type=text vertical-align=bottom color=255,255,0>%s</Obj></Seg>", expAddMsg);
		CoreDataChanged(GDCNI_APPEND_MESSAGE, (UINT)msgBuff, COMBAT_INFO_ROOM_ID);
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (UINT)expAddMsg, 0);
	}
#endif

	m_nExp = exp;
}

void KPlayer::SetSkillExp(DWORD skillExp)
{
	if (skillExp < 0)
		return;

#ifndef _SERVER
	//通知界面
	if (skillExp > m_nSkillExp)
	{
		DWORD addSkillExp = skillExp - m_nSkillExp;
		char msgBuff[256] = { 0 };
		char skillExpAddMsg[256] = { 0 };
		char skillExpAddMsgTemplate[256] = { 0 };
		g_GetStringRes(sid_self_add_skill_exp, skillExpAddMsgTemplate, sizeof(skillExpAddMsgTemplate));
		sprintf(skillExpAddMsg, skillExpAddMsgTemplate, addSkillExp);
		sprintf(msgBuff, "<Seg float=wrap><Obj type=text vertical-align=bottom color=255,255,0>%s</Obj></Seg>", skillExpAddMsg);
		CoreDataChanged(GDCNI_APPEND_MESSAGE, (UINT)msgBuff, COMBAT_INFO_ROOM_ID);
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (UINT)skillExpAddMsg, 0);
	}
#endif

	m_nSkillExp = skillExp;

#ifndef _SERVER
	CoreDataChanged(GDCNI_SKILLLIST_CHANGE, 0, 0);
#endif
}

void KPlayer::SetCanPickup(bool canPickup)
{
	m_CanPickup = canPickup;
}

#ifdef _SERVER
void KPlayer::SetPkValue(int pkValue)
{
	ConfigManager& cm = ConfigManager::Singleton();
	if (pkValue < 0)
	{
		pkValue = 0;
	}
	else if (pkValue > cm.GetGlobalVariable(global_var_pk_boundary_demon_to_diablo))
	{
		pkValue = cm.GetGlobalVariable(global_var_pk_boundary_demon_to_diablo);
	}

	if (m_PkValue != pkValue)
	{
//		g_PlayerStatusChgList.PkValueChange(GetPlayerIndex());

		int originalPkValue = m_PkValue;
		m_PkValue = pkValue;
		
		if (IsValidNpc(m_nIndex) && !Npc[m_nIndex].IsInWorldCombatInstance())
		{
			//TODO 将来需要改为越过边界值再改变重生地点
			//红名需要在红名村重生；脱离红名后需要在指定的默认地点重生
			int pkValueBoundary = cm.GetGlobalVariable(global_var_pk_boundary_alert_to_punish);
			if (pkValue >= pkValueBoundary)
				SetRevivalPos(cm.GetGlobalVariable(global_var_pkpunish_revive_map), cm.GetGlobalVariable(global_var_pkpunish_revive_pos));
			else if (originalPkValue >= pkValueBoundary && pkValue < pkValueBoundary)
				SetRevivalPos(cm.GetGlobalVariable(global_var_default_revive_map), cm.GetGlobalVariable(global_var_default_revive_pos));
			
		}//endif
		
		CheckNameColor();
	}	
}

void KPlayer::SetKiller(bool isKiller)
{
	m_IsKiller = isKiller;	
	CheckNameColor();
}

void KPlayer::CheckNameColor()
{
	ConfigManager& cm = ConfigManager::Singleton();	
	KNpc& npc = Npc[GetNpcIndex()];
	unsigned int npcTitleColor;	

	if (m_PkValue < cm.GetGlobalVariable(global_var_pk_boundary_normal_to_alert))//正常
	{		
		if (m_IsKiller)
		{
			npcTitleColor = cm.GetConfigurableColor(color_killer);			
		}
		else
		{
			//到了PK鼓励等级
			const int pkEncourageLevel = ConfigManager::Singleton().GetGlobalVariable(global_var_pk_encourage_level);
			if (pkEncourageLevel > 0 && npc.GetLevel() >= pkEncourageLevel)
			{
				npcTitleColor = cm.GetConfigurableColor(color_pk_encourage);
			}
			else
			{
				npcTitleColor = cm.GetConfigurableColor(color_pk_normal);
			}
		}
	}
	else if (m_PkValue < cm.GetGlobalVariable(global_var_pk_boundary_alert_to_punish))//警戒
	{
		if (m_IsKiller)
		{			
			npcTitleColor = cm.GetConfigurableColor(color_killer);
		}
		else
		{
			npcTitleColor = cm.GetConfigurableColor(color_pk_alert);
		}
	}
	else if (m_PkValue < cm.GetGlobalVariable(global_var_pk_boundary_punish_to_demon))//惩罚
	{	
		npcTitleColor = cm.GetConfigurableColor(color_pk_punish);
	}
	else if (m_PkValue < cm.GetGlobalVariable(global_var_pk_boundary_demon_to_diablo))
	{
		npcTitleColor = cm.GetConfigurableColor(color_pk_demon);
	}
	else//恶魔
	{
		npcTitleColor = cm.GetConfigurableColor(color_pk_diablo);
	}

	npc.SetNpcTitleColor(npcTitleColor);

	//召唤兽的名字颜色与其主人保持同步
	if (TRUE == m_Creature.IsALive())
	{
		KNpc* pCreatureNpc = m_Creature.GetCreatureNpc();
		if (pCreatureNpc != NULL)
		{
			pCreatureNpc->SetNpcTitleColor(npcTitleColor);
		}
	}

	if (m_Employee.IsExist())
	{
		int employeeNpcIndex = m_Employee.GetNpcIndex();
		if (IsValidNpc(employeeNpcIndex))
		{
			Npc[employeeNpcIndex].SetNpcTitleColor(npcTitleColor);
		}
	}
}

int KPlayer::RecalePKValue(int beKilledPlayerIndex)
{
	if (m_PkPunish <= 0)
		return 0;

	if (beKilledPlayerIndex <= 0 || beKilledPlayerIndex >= MAX_PLAYER)
		return 0;
	KPlayer& playerBeKilled = Player[beKilledPlayerIndex];

	ConfigManager& cm = ConfigManager::Singleton();

	//杀死的是“惩罚状态”或“恶魔状态”的玩家，没有惩罚
	if (playerBeKilled.GetPkValue() >= cm.GetGlobalVariable(global_var_pk_boundary_alert_to_punish))
		return 0;

	//杀死的是“杀手”
	if (playerBeKilled.IsKiller())
		return 0;

	//其他情况需要增加PK值
	int tempPkValue = 0;
	int levelDiff = 20;
	levelDiff = cm.GetGlobalVariable(global_var_pkvalue_level_diff);
	int beKilledPlayerLevel = Npc[playerBeKilled.GetNpcIndex()].GetLevel();
	int selfLevel = Npc[GetNpcIndex()].GetLevel();

	int currentPKValue = GetPkValue();
	int pkValueAdd = cm.GetPKAddedValue(currentPKValue);

	if (selfLevel - beKilledPlayerLevel <= levelDiff)
	{
		tempPkValue = currentPKValue + pkValueAdd;
	}
	else
	{
		tempPkValue = currentPKValue + pkValueAdd * 2;
	}

	int maxPkValue = cm.GetGlobalVariable(global_var_pk_boundary_demon_to_diablo);
	if (tempPkValue > maxPkValue)
		tempPkValue = maxPkValue;
	SetPkValue(tempPkValue);

	return (tempPkValue - currentPKValue);
}

int KPlayer::PkPunish(int beKilledPlayerIndex)
{
	//处于无PK惩罚状态
	if (m_PkPunish <= 0)
		return 0;

	if (beKilledPlayerIndex < 0 || beKilledPlayerIndex > MAX_PLAYER)
		return 0;
	KPlayer& playerBeKilled = Player[beKilledPlayerIndex];

 	ConfigManager& cm = ConfigManager::Singleton();

	//杀死的是“惩罚状态”或“恶魔状态”的玩家，没有惩罚
	if (playerBeKilled.GetPkValue() >= cm.GetGlobalVariable(global_var_pk_boundary_alert_to_punish))
		return 0;
	
	//杀死的是“杀手”
	if (playerBeKilled.IsKiller())
		return 0;

	//其他情况需要增加PK值
	int tempPkValue = 0;
	int levelDiff = 20;
	levelDiff = cm.GetGlobalVariable(global_var_pkvalue_level_diff);
	int a1 = cm.GetGlobalVariable(global_var_pkvalue_add_factor);
	int a2 = cm.GetGlobalVariable(global_var_pkvalue_add_base);
	int beKilledPlayerLevel = Npc[playerBeKilled.GetNpcIndex()].GetLevel();
	int selfLevel = Npc[GetNpcIndex()].GetLevel();
	if (selfLevel - beKilledPlayerLevel <= levelDiff)
	{
		tempPkValue = (a1 * m_PkValue / 100) + a2;
	}
	else
	{
		tempPkValue = ((selfLevel - beKilledPlayerLevel + a1) * m_PkValue / 100) + a2;
	}

	int pkValueAdd = tempPkValue - m_PkValue;
	int maxPkValue = cm.GetGlobalVariable(global_var_pk_boundary_punish_to_demon);
	if (tempPkValue > maxPkValue)
		tempPkValue = maxPkValue;
	SetPkValue(tempPkValue);

	return pkValueAdd;
}

void KPlayer::DeathPunish()
{
	if (m_DeathPunish <= 0)
		return;

	ConfigManager& cm = ConfigManager::Singleton();
	
	//掉装惩罚，根据PK值的不同，有不同的掉装率
	int deathDropBag = 0;
	int deathDropEquip = 0;
	float abradeRate = 0;

	if (m_PkValue >= cm.GetGlobalVariable(global_var_pk_boundary_punish_to_demon))
	{
		deathDropBag	= cm.GetGlobalVariable(global_var_death_drop_bag_demon);
		deathDropEquip	= cm.GetGlobalVariable(global_var_death_drop_equip_demon);
		abradeRate		= cm.GetGlobalVariable(global_var_death_abrade_equip_demon);
	}
	else if (m_PkValue >= cm.GetGlobalVariable(global_var_pk_boundary_alert_to_punish))
	{
		deathDropBag = cm.GetGlobalVariable(global_var_death_drop_bag_punish);
		deathDropEquip = cm.GetGlobalVariable(global_var_death_drop_equip_punish);
		abradeRate = cm.GetGlobalVariable(global_var_death_abrade_equip_punish);
	}
	else if (m_PkValue >= cm.GetGlobalVariable(global_var_pk_boundary_normal_to_alert))
	{
		deathDropBag = cm.GetGlobalVariable(global_var_death_drop_bag_alert);
		deathDropEquip = cm.GetGlobalVariable(global_var_death_drop_equip_alert);
		abradeRate = cm.GetGlobalVariable(global_var_death_abrade_equip_alert);
	}
	else
	{
		deathDropBag = cm.GetGlobalVariable(global_var_death_drop_bag_normal);
		deathDropEquip = cm.GetGlobalVariable(global_var_death_drop_equip_normal);
		abradeRate = cm.GetGlobalVariable(global_var_death_abrade_equip_normal);
	}
	abradeRate /= 1000;
	
	KItemList& itemList = GetItemList();

	//遍历穿上的装备
	for(int equip = 0; equip < itempart_num; equip++)
	{
		int equipIndex = itemList.GetEquipment(equip);
		if (equipIndex > 0)
		{
			//穿在身上的装备需要损失耐久
			Item[equipIndex].Abrade(abradeRate);					
			Item[equipIndex].SyncAttribute(item_attr_durability, GetNetConnectIdx());
			if (Item[equipIndex].GetDurability() == 0)
			{
				itemList.OnEquipChanged();
			}

			//锁定物品、绑定物品不会掉落
			if (!Item[equipIndex].IsLocked( GetNetConnectIdx(), false ) && !Item[equipIndex].IsBind() && !Item[equipIndex].IsTaskGiven())
			{
				switch(Item[equipIndex].GetDeathDropType())
				{
				case item_death_drop_no://不掉落
					break;

				case item_death_drop_normal://普通掉落
					if (deathDropEquip > g_Random(1000))
					{
						DropItem(equipIndex);
					}
					break;

				case item_death_drop_always://必然掉落
					DropItem(equipIndex);
					break;

				default:
					break;
				}
			}
		}
	}
	
	//遍历包裹中的物品
	KInventory* bagRoom = itemList.GetRoom(room_equipment);
	if (bagRoom != NULL)
	{
		int bagSize = bagRoom->getValidSize();
		for(int bagIndex = 0; bagIndex < bagSize; bagIndex++)
		{
			int bagItemIndex = bagRoom->GetItem(bagIndex);
			if (bagItemIndex > 0)
			{
				//锁定物品、绑定物品不会掉落
				if (!Item[bagItemIndex].IsLocked( GetNetConnectIdx(), false ) && !Item[bagItemIndex].IsBind() && !Item[bagItemIndex].IsTaskGiven())
				{
					switch(Item[bagItemIndex].GetDeathDropType())
					{
					case item_death_drop_no://不掉落
						break;

					case item_death_drop_normal://普通掉落
						if (deathDropBag > g_Random(1000))
						{
							DropItem(bagItemIndex);
						}
						break;
						
					case item_death_drop_always://必然掉落
						DropItem(bagItemIndex);
						break;	
						
					default:
						break;
					}
				}
			}
		}
	}
}

void KPlayer::DropItem(int itemIndex)
{
	if (itemIndex <= 0)
		return;

	ItemPos pos;
	if (!GetItemList().GetItemPos(itemIndex,&pos))
	{
		//ItemDebugLog Begin........................
        #ifdef _SERVER
		int nItemIndex = itemIndex;
		if (nItemIndex > 0 && nItemIndex < MAX_ITEM)
		{
			int  nBelong         = Item[nItemIndex].GetBelong();
			char szDumpInfo[512] = "";
			snprintf(szDumpInfo,sizeof(szDumpInfo),"Invalid DropItem:Index %d,Belong:%d,PlayerIdx:%d,Name:%s,Player:%s\n",nItemIndex,nBelong,GetPlayerIndex(),Item[nItemIndex].GetName(),GetPlayerName());
			szDumpInfo[sizeof(szDumpInfo) - 1]=0;
			DumpInvalidItemOpeStack(false,szDumpInfo,4);
		}//endif
        #endif
        //ItemDebugLog End.........................

		return;
	}//endif

	KItem& dropItem = Item[itemIndex];	
	GetItemList().Remove(itemIndex, 0, true, false, true, false);

	int x, y;
	POINT ptLocal;
	KMapPos	Pos;
	
	KNpc& npc = Npc[GetNpcIndex()];
	npc.GetMpsPos(&x, &y);	
	ptLocal.x = x;
	ptLocal.y = y;
	KSubWorld& subWorld = SubWorld[npc.GetSubWorldIndex()];
	subWorld.GetFreeObjPos(ptLocal);
	Pos.nSubWorld = npc.GetSubWorldIndex();
	subWorld.Mps2Map(ptLocal.x, ptLocal.y, &Pos.nRegion, &Pos.nMapX, &Pos.nMapY, &Pos.nOffX, &Pos.nOffY);
				
	int nObj;
	KObjItemInfo sInfo;
	sInfo.m_nItemID = itemIndex;
	sInfo.m_nMoneyNum = (dropItem.GetMaxItemCount() == 0) ? 0 : dropItem.GetItemCount();

	if ( dropItem.GetYaoID() == yao_yin )
	{
		sprintf( sInfo.m_szName, "[%s]%s", YIN, dropItem.GetName() );
	}
	else if ( dropItem.GetYaoID() == yao_yang )
	{
		sprintf( sInfo.m_szName, "[%s]%s", YANG, dropItem.GetName() );
	}
	else
	{
		memcpy(sInfo.m_szName, dropItem.GetName(), FILE_NAME_LENGTH );
	}
	
	sInfo.m_szName[FILE_NAME_LENGTH - 1] = 0;
	sInfo.m_nColorID = dropItem.GetQualityLabel();
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
	
	if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_drop_item))
	{
		RecordPlayerActionParam param;
		param.PlayerIndex = GetPlayerIndex();
		param.Action = player_action_drop_item;
		snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_DROP_ITEM, dropItem.GetName());
		g_PlayerMonitor.RecordPlayerAction(param);
	}

	nObj = ObjSet.Add(dropItem.GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
	{
		ItemSet.Remove(itemIndex);
	}
	else
	{
		//ItemDebugLog Begin........................
        #ifdef _SERVER
		int nItemIndex = itemIndex;
		if (nItemIndex > 0 && nItemIndex < MAX_ITEM && Item[nItemIndex].GetBelong() != -1)
		{
			int  nBelong         = Item[nItemIndex].GetBelong();
			char szDumpInfo[512] = "";
			snprintf(szDumpInfo,sizeof(szDumpInfo),"Player Lose:Index %d,Belong:%d,PlayerIdnex:%d,Name:%s\n",nItemIndex,nBelong,GetPlayerIndex(),Item[nItemIndex].GetName());
			szDumpInfo[sizeof(szDumpInfo) - 1]=0;
			DumpInvalidItemOpeStack(false,szDumpInfo,4);
		}//endif
        #endif
        //ItemDebugLog End.........................
		Object[nObj].SetItemBelong(-1);
	}
}

void KPlayer::ReturnMarkCreature()
{
	if( IsValidNpc(m_MarkCreatureNpcIdx) )
	{
		if(kind_creature == Npc[m_MarkCreatureNpcIdx].m_Kind)
		{
			int nSummonerNpcIdx = Npc[m_MarkCreatureNpcIdx].GetSummonerIdx();
			int nSummonerPlayerIdx = Npc[nSummonerNpcIdx].GetPlayerIdx();
			Player[nSummonerPlayerIdx].m_Creature.MarkToPlayer(nSummonerNpcIdx);
		}
		else
			_ASSERT(false);
	}
}

void KPlayer::Offline()
{
	m_dwTotolJinshabi		= 0;
	m_dwRecentlyTime		= 0;
	m_dwRecentJinshabi		= 0;

	int subworldIndex = Npc[m_nIndex].GetSubWorldIndex();
	OnEvent(player_event_exit_world, &subworldIndex);

	tradeServerDoCanceTrade();

	m_Creature.Dismiss();
	
	GetEmployee().Fire();

	//有问题时下线认为回答超时
	if (m_QuestionState.HasQuestion())
		m_QuestionState.OnTimeout();

	ReturnMarkCreature();

	//下线标志
	DBOffline( );

	GetUIServerState().PlayerOffLineNotify();

	//存盘不成功直接Remove
	if( Save( NULL, TRUE ) == FALSE )
		m_bIsCanRemove = TRUE;

	m_PlayerStatistic.Save();

	int	nPlayerIdx = GetPlayerIndex();

	m_serverAucMgr.PlayerOffLine(nPlayerIdx);
	ServerSocialUnitMgr::Singleton().PlayerOffLine(nPlayerIdx);

	g_SubWorldSet.PlayerOffLine(nPlayerIdx);

	m_DBLoadProcessFlag = 0;

	// 需要根据名字找idx的地方都放在这一行前面
	g_ChatCenterS.PlayerOffLine(nPlayerIdx);

	if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_player_leave_world))
	{
		//玩家离开游戏世界日志
		LogEventParam leaveWorldEvent;
		leaveWorldEvent.event = log_event_player_leave_world;
		leaveWorldEvent.param1 = GetGUID();
		leaveWorldEvent.param4 = UNIX_TMIE_STAMP - m_dwLoginTime;
		g_pLogSystem->Log(leaveWorldEvent);	
	}

	//离开游戏
	if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_logout))
	{
		RecordPlayerActionParam param;
		param.PlayerIndex = GetPlayerIndex();
		param.Action = player_action_logout;
		snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_LOGOUT);
		g_PlayerMonitor.RecordPlayerAction(param);
	}
}

int KPlayer::CheckCanPickOBJByIndex(int nPickupObjectIndex)
{
	if(Npc[m_nIndex].m_Doing == do_revive || 
		Npc[m_nIndex].m_Doing == do_death)
	{
		return FALSE;
	}//endif
	
	if (nPickupObjectIndex > 0 && nPickupObjectIndex < MAX_OBJECT)
	{
		int	nObjIndex = nPickupObjectIndex;
		
		if(Object[nObjIndex].m_nKind != Obj_Kind_Money && !CanPickup())
		{
			return FALSE;
		}
		
		if (Object[nObjIndex].m_nBelong != -1)
		{
			if (!GetTeamInfo().IsInTeam())
			{
				if (Object[nObjIndex].m_nBelong != m_nPlayerIndex)
				{
					return FALSE;
				}
			}
			else
			{
				if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
				{
					if (Object[nObjIndex].m_nBelong != m_nPlayerIndex &&
						!GetTeamInfo().GetTeam()->IsMember(Object[nObjIndex].m_nBelong))
					{
						return FALSE;
					}
				}
				else if (Object[nObjIndex].m_nKind == Obj_Kind_Item)
				{
					if (Object[nObjIndex].m_nItemDataID <= 0 || Object[nObjIndex].m_nItemDataID >= MAX_ITEM)
					{
						_ASSERT(0);
						return FALSE;
					}
					if ((Item[Object[nObjIndex].m_nItemDataID].GetGenre() == item_task && Object[nObjIndex].m_nBelong != m_nPlayerIndex) ||
						!GetTeamInfo().GetTeam()->IsMember(Object[nObjIndex].m_nBelong) || 					
						!Item[Object[nObjIndex].m_nItemDataID].CanDiscard())//不能丢弃的物品不能被拾取，这是为了安全（防止不能丢弃的物品被意外放到地上）
					{
						return FALSE;
					}
				}
				else
				{
					return FALSE;
				}
			}
		}
		
	}//endif
	else
		return FALSE;
	
	return TRUE;
}

int KPlayer::CheckPickupObject(int pickupObjectId)
{
	if(Npc[m_nIndex].m_Doing == do_revive || 
		Npc[m_nIndex].m_Doing == do_death)
	{
		return FALSE;
	}//endif

	int	nPickupObjectIndex = ObjSet.FindID(pickupObjectId);
	if (nPickupObjectIndex == 0)
		return FALSE;
	
	if (nPickupObjectIndex > 0 && nPickupObjectIndex < MAX_OBJECT)
	{
		int	nObjIndex, nNpcX, nNpcY, nObjX, nObjY;
		nObjIndex = nPickupObjectIndex;
		
		if(Object[nObjIndex].m_nKind != Obj_Kind_Money && !CanPickup())
		{
			//通知客户端不能拾物品（金钱除外）
			ShowPredefinedMsg(11381);
			return FALSE;
		}
		
		if (Object[nObjIndex].m_nBelong != -1)
		{
			if (!GetTeamInfo().IsInTeam())
			{
				if (Object[nObjIndex].m_nBelong != m_nPlayerIndex)
				{
					//通知客户端不能拾取别人的东西
					ShowPredefinedMsg(11380);
					return FALSE;
				}
			}
			else
			{
				if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
				{
					if (Object[nObjIndex].m_nBelong != m_nPlayerIndex &&
						!GetTeamInfo().GetTeam()->IsMember(Object[nObjIndex].m_nBelong))
					{
						//通知客户端不能拾取别人的东西
						ShowPredefinedMsg(11380);
						return FALSE;
					}
				}
				else if (Object[nObjIndex].m_nKind == Obj_Kind_Item)
				{
					if (Object[nObjIndex].m_nItemDataID <= 0 || Object[nObjIndex].m_nItemDataID >= MAX_ITEM)
					{
						_ASSERT(0);
						return FALSE;
					}
					if ((Item[Object[nObjIndex].m_nItemDataID].GetGenre() == item_task && Object[nObjIndex].m_nBelong != m_nPlayerIndex) ||
						!GetTeamInfo().GetTeam()->IsMember(Object[nObjIndex].m_nBelong) || 					
						!Item[Object[nObjIndex].m_nItemDataID].CanDiscard())//不能丢弃的物品不能被拾取，这是为了安全（防止不能丢弃的物品被意外放到地上）
					{
						ShowPredefinedMsg(11380);
						return FALSE;
					}
				}
				else
				{
					return FALSE;
				}
			}
		}
		// 判断距离
		if (Object[nObjIndex].m_nSubWorldID != Npc[m_nIndex].m_SubWorldIndex)
			return FALSE;
		SubWorld[Object[nObjIndex].m_nSubWorldID].Map2Mps(
			Object[nObjIndex].m_nRegionIdx,
			Object[nObjIndex].m_nMapX,
			Object[nObjIndex].m_nMapY,
			Object[nObjIndex].m_nOffX,
			Object[nObjIndex].m_nOffY,
			&nObjX,
			&nObjY);
		SubWorld[Npc[m_nIndex].m_SubWorldIndex].Map2Mps(
			Npc[m_nIndex].m_RegionIndex,
			Npc[m_nIndex].GetMapX(),
			Npc[m_nIndex].GetMapY(),
			Npc[m_nIndex].GetOffX(),
			Npc[m_nIndex].GetOffY(),
			&nNpcX,
			&nNpcY);
		if ( PLAYER_PICKUP_SERVER_DISTANCE < (nNpcX - nObjX) * (nNpcX - nObjX) + (nNpcY - nObjY) * (nNpcY - nObjY))
		{
			//to do
			//		SHOW_MSG_SYNC	sMsg;
			//		sMsg.ProtocolType = s2c_msgshow;
			//		sMsg.m_wMsgID = enumMSG_ID_OBJ_TOO_FAR;
			//		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			//		if (g_pServer != NULL)
			//			g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			
			return FALSE;
		}
		
	}//endif
	else
		return FALSE;

	return TRUE;
}

int KPlayer::DoPickupObject(int pickupObjectId, int posX, int posY)
{
	int posType = pos_equiproom;
	int checkResult = CheckPickupObject(pickupObjectId);
	if (TRUE != checkResult)
	{
		return checkResult;
	}

	int pickupObjectIndex = ObjSet.FindID(pickupObjectId);
	if (pickupObjectIndex <= 0)
	{
		return FALSE;
	}

	KObj& pickupObject = Object[pickupObjectIndex];
	
	switch (pickupObject.GetKind())
	{
	case Obj_Kind_Item:				// 掉在地上的装备
		{
			KItem& pickupItem = Item[pickupObject.GetItemDataID()];

			//统计：拾取物品
			ItemTemplateId templateId;
			pickupItem.GetItemTemplateId(templateId);
			GetPlayerStatistic().AddItem(templateId, pickupItem.GetItemCount(), item_count_type_pickup);

			//日志：拾取物品
			LogEventParam pickUpItemEventParam;
			bool needLog = (pickupItem.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
			if (needLog)
			{				
				pickUpItemEventParam.event = log_event_pickup_item;
				pickUpItemEventParam.param1 = GetGUID();
				pickUpItemEventParam.param2 = pickupItem.GetGUID();
				pickupItem.GetItemTemplateId(pickUpItemEventParam.param3.data, sizeof(pickUpItemEventParam.param3.data) - 1);
			}
			
			if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_pickup_item))
			{
				RecordPlayerActionParam param;
				param.PlayerIndex = GetPlayerIndex();
				param.Action = player_action_pickup_item;
				snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_PICKUP_ITEM, pickupItem.GetName());
				g_PlayerMonitor.RecordPlayerAction(param);
			}
			
			int nItemIdx;
			if(pickupItem.GetMaxItemCount() > 0)
			{				
				ItemPos itemPos;
				EXTRAINFOPLUS tagExtraPlus;
				tagExtraPlus.nItemGenre = pickupItem.GetGenre();
				tagExtraPlus.nParticularType = pickupItem.GetParticular();
				tagExtraPlus.nDetailType = pickupItem.GetDetailType();
				tagExtraPlus.nMaxItem = pickupItem.GetMaxItemCount();
				tagExtraPlus.nCurItem = pickupItem.GetItemCount();
				tagExtraPlus.pCampareItem = &pickupItem;

				if(FALSE == m_ItemList.SearchPosition(&itemPos, &tagExtraPlus))
				{
					return FALSE;
				}
				posType = itemPos.nPlace;
				posX = itemPos.nX;
				posY = itemPos.nY;
			}
			int nRetIndex = 0;

			pickupItem.SetOBJBelong(-1);
			nItemIdx = m_ItemList.Add(pickupObject.GetItemDataID(), posType, posX, posY, &nRetIndex, item_sync_type_pickup);
			pickupItem.SetOBJBelong(pickupObject.m_nIndex);

			if (nItemIdx <= 0 || nItemIdx >= MAX_PLAYER_ITEM)
			{
				//ItemDebugLog Begin........................
                #ifdef _SERVER
				if ( pickupItem.GetBelong() != -1)
				{
					char szDumpInfo[512] = "";
					snprintf(szDumpInfo,sizeof(szDumpInfo),"Faield Pickup:Index %d,Belong:%d,Name:%s\n",pickupItem.GetItemIndex(),pickupItem.GetBelong(),pickupItem.GetName());
					szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
					DumpInvalidItemOpeStack(true,szDumpInfo,5);
				}//endif
                #endif
                //ItemDebugLog End.........................

				return FALSE;
			}//endif

			if (pickupObject.GetItemDataID() <= 0 || pickupObject.GetItemDataID() >= MAX_ITEM)
				return FALSE;

			//拾取物品日志
			if (needLog)
			{
				g_pLogSystem->Log(pickUpItemEventParam);
			}			
			
			// 去掉Object[nObjIndex]与道具的关联。避免ItemSet的Remove被Object的Remove调用
			pickupItem.SetOBJBelong(-1);
			pickupObject.SetItemDataID(0);
			pickupObject.Remove(FALSE, m_dwID, TRUE);			
		}
		break;
	case Obj_Kind_Money:			// 掉在地上的钱
		{
			if ( !Earn(pickupObject.GetMoney()) )
				return FALSE;

			if (pickupObject.GetMoney() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_pickup_money_amount))
			{
				LogEventParam pickUpMoneyEventParam;
				pickUpMoneyEventParam.event = log_event_pickup_money;
				pickUpMoneyEventParam.param1 = GetGUID();
				pickUpMoneyEventParam.param4 = pickupObject.GetMoney();
				g_pLogSystem->Log(pickUpMoneyEventParam);
			}
			
			if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_pickup_money))
			{
				RecordPlayerActionParam param;
				param.PlayerIndex = GetPlayerIndex();
				param.Action = player_action_pickup_money;
				snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_PICKUP_MONEY, pickupObject.GetMoney());
				g_PlayerMonitor.RecordPlayerAction(param);
			}
			
			pickupObject.SyncRemove(TRUE, m_dwID, TRUE);

			if (pickupObject.GetRegionIndex() >= 0)
				SubWorld[pickupObject.GetSubWorldIndex()].m_Region[pickupObject.GetRegionIndex()].RemoveObj(pickupObjectIndex);
			ObjSet.Remove(pickupObjectIndex);
		}
		break;
	}
	
	return TRUE;	
}

int KPlayer::CheckDialogNpc(int dialogNpcId)
{
	//死亡时不能对话
	if (!Npc[GetNpcIndex()].IsAlive())
		return FALSE;

	if (dialogNpcId < 0)
		return FALSE;

	int dialogNpcIndex = FindAroundNpc(dialogNpcId);
	if (dialogNpcIndex <= 0)
		return FALSE;

	KNpc& dialogNpc = Npc[dialogNpcIndex];	

	// 小于对话半径就开始对话
	const int npcKind = dialogNpc.GetKind();
	if ((npcKind == kind_dialoger)
		|| (npcKind== kind_siege_weapon)
		|| (npcKind == kind_building)
		|| (NpcSet.GetRelation(m_nIndex, dialogNpcIndex) == relation_none)
		|| (NpcSet.GetRelation(m_nIndex, dialogNpcIndex) == relation_dialog))
	{
		int distance = NpcSet.GetDistance(dialogNpcIndex, m_nIndex);
		if (distance <= dialogNpc.GetDialogRadius() * 2)//放大server对话半径
		{
			return TRUE;			
		}
	}

	return FALSE;
}

int KPlayer::DoDialogNpc(int dialogNpcId)
{
	int checkResult = CheckDialogNpc(dialogNpcId);
	if (TRUE == checkResult)
	{
		int dialogNpcIndex = FindAroundNpc(dialogNpcId);
		if (dialogNpcIndex <= 0)
			return FALSE;
		KNpc& dialogNpc = Npc[dialogNpcIndex];
        m_nDialogNpcKind =  dialogNpc.m_NpcSettingIdx;

		//为问答回调准备参数
		CallbackScriptParam callbackParam;
		callbackParam.ScriptId = dialogNpc.m_ActionScriptID;
		strncpy(callbackParam.FuncName, "DialogNpc", sizeof(callbackParam.FuncName));
		callbackParam.FuncName[sizeof(callbackParam.FuncName) - 1] = 0;
		callbackParam.NpcIndex = dialogNpcIndex;
		callbackParam.NpcSettingIdx = dialogNpc.m_NpcSettingIdx;
		callbackParam.NpcId = dialogNpc.GetId();
		m_QuestionState.SetTempParam(callbackParam);

		ExecuteScript2Param(dialogNpc.m_ActionScriptID, "DialogNpc", 0, dialogNpcIndex, dialogNpc.m_NpcSettingIdx);

		if (g_PlayerMonitor.IsNeedRecord(GetPlayerIndex(), player_action_dialog_npc))
		{
			RecordPlayerActionParam param;
			param.PlayerIndex = GetPlayerIndex();
			param.Action = player_action_dialog_npc;
			snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_DIALOG_NPC, dialogNpc.Name);
			g_PlayerMonitor.RecordPlayerAction(param);
		}

		return TRUE;
	}
	else
	{
		return checkResult;
	}
}

#endif

#ifdef _SERVER
void KPlayer::AddTalismanPotential(DWORD potentialAdded)
{
	if (potentialAdded == 0 || potentialAdded > MAX_ADD_TALISMAN_POTENTIAL)
		return;

	int talismanIndex = m_ItemList.GetEquipment(itempart_talisman);
	if (talismanIndex > 0)
	{
		if (!m_ItemList.IsEquipmentMasked(itempart_talisman))
		{
			KItem& talisman = Item[talismanIndex];
			talisman.AddTalismanPotential(potentialAdded);
			talisman.SyncAttribute(item_attr_talisman_potential, GetNetConnectIdx());
		}		
	}
}
#endif

#ifdef _SERVER
void KPlayer::InitPlayerSaveTimeInterval()
{
	ConfigManager &cfgMgr = ConfigManager::Singleton();
	
	int	nMinInterval = cfgMgr.GetGlobalVariable(global_var_playersave_min_interval);
	int	nMaxInterval = cfgMgr.GetGlobalVariable(global_var_playersave_max_interval);

	m_PlayerSaveTimeInterval = nMinInterval + g_Random( abs(nMaxInterval - nMinInterval) );
	
	if(m_PlayerSaveTimeInterval <= 0)
	{
		_ASSERT(false);
		static const int __DEF_MIN_PLAYERSAVE_TIME = 600;  // seconds
		static const int __DEF_MAX_PLAYERSAVE_TIME = 1800; // seconds

		m_PlayerSaveTimeInterval = __DEF_MIN_PLAYERSAVE_TIME + 
			g_Random(__DEF_MAX_PLAYERSAVE_TIME - __DEF_MIN_PLAYERSAVE_TIME);
	}

	m_PlayerSaveTimeInterval *= GAME_FPS;
}
#endif

#ifdef _SERVER
bool KPlayer::ShowPredefinedMsg(int msgId)
{
	SHOW_PREDEFINED_MSG showMsg;
	showMsg.Protocol = s2c_show_predefined_msg;
	showMsg.MessageId = msgId;
	
	if (g_pServer != NULL)
	{
		if (g_pServer->PackDataToClient(GetNetConnectIdx(), &showMsg, sizeof(showMsg)))
		{
			return true;
		}		
	}

	return false;
}
#endif

#ifndef _SERVER
void KPlayer::ReplyPrompt(enumPromptEvent promptEvent, bool accept)
{
	CLIENT_REPLY_PROMPT reply;
	reply.Protocol = c2s_reply_prompt;
	reply.Event = promptEvent;
	reply.Reply = (accept ? TRUE : FALSE);

	if(g_pClient)
		g_pClient->SendPackToServer(g_ConnectID, (BYTE*)&reply, sizeof(reply));
}
#endif

#ifdef _SERVER
void KPlayer::Kick()
{
	if (g_pController)
		g_pController->PushData(protocol_type_clientshutdown, GetNetConnectIdx(), NULL, 0);
}
#endif

#ifdef _SERVER
void KPlayer::RememberCurrentPos()
{
	KNpc& npc = Npc[GetNpcIndex()];
	int worldIndex = npc.GetSubWorldIndex();
	if (worldIndex < INSTANCE_SUBWORLD_START)
	{
		m_RememberSubworldId = SubWorld[worldIndex].m_SubWorldID;
		npc.GetMpsPos(&(m_RememberPosX), &(m_RememberPosY));
	}
}
#endif

#ifdef _SERVER
int KPlayer::TransferToRememberPos()
{
	if (m_RememberSubworldId == INVALID_WORLD_ID)
		return FALSE;

	int npcIndex = GetNpcIndex();
	if (IsValidNpc(npcIndex) && Npc[npcIndex].IsPlayer())
	{
		return Npc[npcIndex].ChangeWorld(m_RememberSubworldId, m_RememberPosX, m_RememberPosY, false);
	}
	else
	{
		return FALSE;
	}
}
#endif

#ifndef _SERVER

void  KPlayer::PushRunPackageRecord(int iDestX,int iDestY)
{
	m_RunPkRecord.bEnble = true;
	m_RunPkRecord.nMpsX  = iDestX;
	m_RunPkRecord.nMpsY  = iDestY;
}

void  KPlayer::ClearRunPackageRecord(void)
{
    m_RunPkRecord.bEnble     = false;
	m_RunPkRecord.nMpsX      = 0;
	m_RunPkRecord.nMpsY      = 0;
	m_RunPkRecord.nLastFrame = 0;
}

#define MAX_RUN_PACKAGE_RECORD_INTERVAL_FRAME 6
void  KPlayer::ActivatePakcageRecord(void)
{
   if (m_RunPkRecord.bEnble && SubWorld[0].m_dwCurrentTime - m_RunPkRecord.nLastFrame > MAX_RUN_PACKAGE_RECORD_INTERVAL_FRAME )
   {
       SendClientCmdRun(m_RunPkRecord.nMpsX,m_RunPkRecord.nMpsY);
	   ClearRunPackageRecord();
       m_RunPkRecord.nLastFrame = SubWorld[0].m_dwCurrentTime;
   }//endif
}

#endif

#ifdef _SERVER
void KPlayer::ProcessAutoAttack()
{
	NPCCMD doing = Npc[m_nIndex].m_Doing;
	KNpc& npc = Npc[GetNpcIndex()];
	CastSkillParam useSkill;
	//------------------------------------------------
	//TODO 技能打断临时代码
	if (m_NextSkill.SkillId != INVALID_SKILL_ID)
	//if (m_NextSkill.SkillId != INVALID_SKILL_ID && (do_stand == doing || do_run == doing))
	//------------------------------------------------
	{
		if (npc.IsCastingSkill())//正在施放技能，看能否打断
		{
			//看技能是否相同，不同技能才需要打断
			KSkill* pSkill = npc.GetActiveSkill();
			if (pSkill)
			{
				if (pSkill->GetSkillId() != m_NextSkill.SkillId)//不同技能
				{
					if (npc.CanCancelSkill())//技能动作可以被打断
					{
						npc.CancelSkill();
						useSkill = m_NextSkill;
						m_NextSkill.SkillId = INVALID_SKILL_ID;
					}
				}
				else
				{
					m_NextSkill.SkillId = INVALID_SKILL_ID;
				}
			}
		}
		else
		{
			useSkill = m_NextSkill;
			m_NextSkill.SkillId = INVALID_SKILL_ID;
		}
	}
	else if (m_SelectedSkill.SkillId != INVALID_SKILL_ID && do_stand == doing)
	{
		useSkill = m_SelectedSkill;
	}
	
	if (useSkill.SkillId != INVALID_SKILL_ID)
	{
		npc.SendCommand(do_skill, useSkill.SkillId, useSkill.Param1, useSkill.Param2);
	}
}


int KPlayer::JinshanbiToCredit(const int nJinshanbi)
{
	if (nJinshanbi > 0)
	{
		ConfigManager & mgr = ConfigManager::Singleton();
		int nJinShanbiPer   = mgr.GetIBGlobalVariable(ib_global_var_credit_rate_jinshanbi);

		if (nJinShanbiPer > 0 )
		{
			return nJinshanbi * nJinShanbiPer;
		}
		else
		{
			return nJinshanbi;
		}
	}

	return 0;
}

#endif

#ifdef _SERVER
int KPlayer::ExecuteInteractiveScript(DWORD scriptId, const char* funcName, int originalParam1, int originalParam2, int originalParam3)
{
	if (m_interactiveScriptState.IsProcessing)
		return FALSE;

	memset(&m_interactiveScriptState, 0, sizeof(m_interactiveScriptState));

	m_interactiveScriptState.IsProcessing = true;
	m_interactiveScriptState.Step = 0;
	m_interactiveScriptState.Timeout = UNIX_TMIE_STAMP + INTERACTIVE_SCRIPT_TIMEOUT;
	m_interactiveScriptState.ScriptId = scriptId;
	strncpy(m_interactiveScriptState.FuncName, funcName, sizeof(m_interactiveScriptState.FuncName));
	m_interactiveScriptState.FuncName[sizeof(m_interactiveScriptState.FuncName) - 1] = 0;
	m_interactiveScriptState.OriginalParam1 = originalParam1;
	m_interactiveScriptState.OriginalParam2 = originalParam2;
	m_interactiveScriptState.OriginalParam3 = originalParam3;

	return ExecuteScript3Param(m_interactiveScriptState.ScriptId,
		m_interactiveScriptState.FuncName,
		0,
		m_interactiveScriptState.OriginalParam1,
		m_interactiveScriptState.OriginalParam2,
		m_interactiveScriptState.OriginalParam3);
}

void KPlayer::InteractiveScriptNextStep()
{
	if (!m_interactiveScriptState.IsProcessing)
		return;

	m_interactiveScriptState.Step++;
	ExecuteScript3Param(m_interactiveScriptState.ScriptId,
		m_interactiveScriptState.FuncName,
		0, 
		m_interactiveScriptState.OriginalParam1,
		m_interactiveScriptState.OriginalParam2, 
		m_interactiveScriptState.OriginalParam3);
}

void KPlayer::InteractiveScriptDone()
{
	if (!m_interactiveScriptState.IsProcessing)
		return;

	memset(&m_interactiveScriptState, 0, sizeof(m_interactiveScriptState));
	m_interactiveScriptState.IsProcessing = false;
}


void KPlayer::CheckInteractiveScriptTimeout()
{
	if (!m_interactiveScriptState.IsProcessing)
		return;

	if (m_interactiveScriptState.Timeout < UNIX_TMIE_STAMP)
	{
		memset(&m_interactiveScriptState, 0, sizeof(m_interactiveScriptState));
	}
}
#endif
/*
void KPlayer::SetMoney(DWORD money)
{
#ifdef _SERVER
	m_ExtPointInfo.dwLeftMoney = money;

	//同步到客户端
	SyncAttribute(attr_jinshanbi);
#else
	m_Money = money;

	//通知界面
	CoreDataChanged(GDCNI_JINSHANBI_CHANGED, 0, m_Money);
#endif
}
//*/

#ifdef _SERVER
void KPlayer::NoChat(DWORD noChatSeconds)
{
	if (noChatSeconds == 0)
	{
		//TODO 通知客户端禁言解除
	}
	else
	{
		//TODO 通知客户端被禁言
	}

	m_NoChatTime = UNIX_TMIE_STAMP + noChatSeconds;
}
#endif

#define DEFAULT_REWARD_TICKET_RATIO 100

#ifdef _SERVER
KPlayerUIState & KPlayer::GetUIServerState(void)
{
	return m_PlayerUIState;
}

int KPlayer::GetRecommenderRewardTicket()
{
	if (m_RecommenderRewardToAdd <= 0)
		return 0;

	int ratio = ConfigManager::Singleton().GetGlobalVariable(global_var_recommender_reward_ticket_ratio);
	if (ratio <= 0)
		ratio = DEFAULT_REWARD_TICKET_RATIO;

	int addTicket = m_RecommenderRewardToAdd / ratio;
	if (addTicket <= 0)
		return 0;

	int itemGenre = 0;
	int itemDetail = 0;
	int itemParticular = 0;
	int itemLevel = 0;
	
	ConfigManager& cm = ConfigManager::Singleton();
	cm.GetIBTicketId( &itemGenre, &itemDetail, &itemParticular, &itemLevel );
	
	int actualRewardTicketCount = 0;
	
	GlobalAddItemToPlayer(
		GetPlayerIndex(),
		itemGenre,
		itemDetail, 
		itemParticular,
		itemLevel,
		addTicket,
		actualRewardTicketCount,
		item_count_type_recommender_reward_ticket,
		log_event_recommend_master_reward_add_item
		);
	
	if (actualRewardTicketCount > 0)
	{
		KIBLog::getSingleton().AddIBMoney( recommender_reward_card, GetPlayerIndex(), actualRewardTicketCount );
	}
	
	if (g_pLogSystem && TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_recommender))
	{
		LogEventParam logParam;
		logParam.event = log_event_recommend_master_reward_add_ticket;
		logParam.param1 = GetGUID();
		snprintf(logParam.param3.data, sizeof(logParam.param3.data), "%d", actualRewardTicketCount);
		logParam.param3.data[sizeof(logParam.param3.data) - 1] = 0;
		logParam.param4 = actualRewardTicketCount;
		g_pLogSystem->Log(logParam);
	}

	int actualReward = actualRewardTicketCount * ratio;
	if (actualReward > 0 && actualReward <= m_RecommenderRewardToAdd)
	{
		m_RecommenderRewardToAdd -= actualReward;

		if (m_RecommenderRewardTicketAdded < MAX_RECOMMENDER_REWARD && actualRewardTicketCount < MAX_ADD_RECOMMENDER_REWARD)
		{
			m_RecommenderRewardTicketAdded += actualRewardTicketCount;
		}
	}
	else
	{
		//TODO 问题严重了，需要打系统日志
	}
	
	if (g_pLogSystem && TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_recommender))
	{
		LogEventParam logEvent;
		logEvent.event = log_event_recommend_get_master_reward_result;
		logEvent.param1 = GetGUID();		
		snprintf(logEvent.param2.data, sizeof(logEvent.param2.data), "%d", actualReward);
		g_pLogSystem->Log(logEvent);
	}

	return actualRewardTicketCount;
}
#endif

#ifdef _SERVER
// bool KPlayer::LoadInstanceData(BYTE *pInBuf, int nSize)
// {
// 	memset(m_InstanceId, 0, sizeof(m_InstanceId));
// 
// 	bool result = false;
// 
// 	if(pInBuf && nSize >= 1)
// 	{
// 		BYTE dataVersion = *pInBuf;
// 		BYTE* pInstanceDataBuff = pInBuf + 1;
// 		int instanceDataSize = nSize - 1;
// 
// 		switch(dataVersion)
// 		{
// 		case instance_data_version_1:
// 			result = LoadInstanceDataVersion1(pInstanceDataBuff, instanceDataSize);
// 			break;
// 		}
// 	}
// 
// 	return result;
// }
// 
// int KPlayer::SaveInstanceData(BYTE *pOutBuf)
// {
// 	int returnSize = 0;
// 
// 	if(pOutBuf)
// 	{
// 		*pOutBuf = CUR_INSTANCE_DATA_VERSION;
// 		BYTE* pInstanceDataBuff = pOutBuf + 1;
// 
// 		switch(CUR_INSTANCE_DATA_VERSION)
// 		{
// 		case instance_data_version_1:
// 			returnSize = SaveInstanceDataVersion1(pInstanceDataBuff);
// 			break;
// 		}
// 
// 		if (returnSize > 0)
// 			returnSize += 1;
// 	}
// 
// 	return returnSize;
// }
// 
// bool KPlayer::LoadInstanceDataVersion1(BYTE *pInBuf, int nSize)
// {
// 	if(NULL == pInBuf)
// 		return false;
// 
// 	int instanceCount = nSize / sizeof(DBInstanceData);
// 
// 	if(instanceCount >= INSTANCE_SUBWORLD_START)
// 		return false;
// 
// 	PDBInstanceData	pInstanceData = (PDBInstanceData)pInBuf;
// 
// 	for(int i = 0; i < instanceCount; ++i)
// 	{
// 		if (pInstanceData[i].InstanceId > 0 && pInstanceData[i].WorldTemplateId < INSTANCE_SUBWORLD_START)
// 		{
// 			m_InstanceId[pInstanceData[i].WorldTemplateId] = pInstanceData[i].InstanceId;
// 		}
// 	}
// 
// 	return true;
// }
// 
// int KPlayer::SaveInstanceDataVersion1(BYTE *pOutBuf)
// {
// 	int nSize = 0;
// 	int	nCount = 0;
// 
// 	if(pOutBuf)
// 	{
// 		PDBInstanceData pInstanceData = (PDBInstanceData)pOutBuf;
// 
// 		for(int i = 0; i < INSTANCE_SUBWORLD_START; ++i)
// 		{
// 			if (m_InstanceId[i] == 0)
// 				continue;
// 
// 			pInstanceData->WorldTemplateId = i;
// 			pInstanceData->InstanceId = m_InstanceId[i];
// 
// 			++pInstanceData;
// 			++nCount;
// 		}
// 
// 		nSize = sizeof(DBInstanceData) * nCount;
// 	}
// 
// 	return nSize;
// }

int InstanceInfo::Save(BYTE *pSaveBuf, int buffSize)
{
	int returnSize = 0;

	if(pSaveBuf && buffSize > 0)
	{
		*pSaveBuf = CUR_INSTANCE_DATA_VERSION;
		BYTE* pInstanceDataBuff = pSaveBuf + 1;
		int instanceDataBuffSize = buffSize - 1;

		switch(CUR_INSTANCE_DATA_VERSION)
		{
		case instance_data_version_1:
			returnSize = SaveVersion1(pInstanceDataBuff, instanceDataBuffSize);
			break;
		}

		if (returnSize > 0)
			returnSize += 1;
	}

	return returnSize;
}

bool InstanceInfo::Load(BYTE *pLoadBuf, int dataSize)
{
	memset(m_InstanceId, 0, sizeof(m_InstanceId));

	bool result = false;

	if(pLoadBuf && dataSize >= 1)
	{
		BYTE dataVersion = *pLoadBuf;
		BYTE* pInstanceDataBuff = pLoadBuf + 1;
		int instanceDataSize = dataSize - 1;

		switch(dataVersion)
		{
		case instance_data_version_1:
			result = LoadVersion1(pInstanceDataBuff, instanceDataSize);
			break;
		}
	}

	return result;
}

int InstanceInfo::SaveVersion1(BYTE *pSaveBuf, int buffSize)
{
	int nSize = 0;
	int	nCount = 0;
	int maxCount = buffSize / sizeof(PDBInstanceData);

	if(pSaveBuf && maxCount > 0)
	{
		PDBInstanceData pInstanceData = (PDBInstanceData)pSaveBuf;

		for(int i = 0; i < INSTANCE_SUBWORLD_START; ++i)
		{
			if (m_InstanceId[i] == 0)
				continue;

			pInstanceData->WorldTemplateId = i;
			pInstanceData->InstanceId = m_InstanceId[i];

			++pInstanceData;
			++nCount;

			if (nCount >= maxCount)
			{
				break;
			}
		}

		nSize = sizeof(DBInstanceData) * nCount;
	}

	return nSize;
}

bool InstanceInfo::LoadVersion1(BYTE *pLoadBuf, int dataSize)
{
	if(NULL == pLoadBuf)
		return false;

	int instanceCount = dataSize / sizeof(DBInstanceData);

	if(instanceCount >= INSTANCE_SUBWORLD_START)
		return false;

	PDBInstanceData	pInstanceData = (PDBInstanceData)pLoadBuf;

	for(int i = 0; i < instanceCount; ++i)
	{
		if (pInstanceData[i].InstanceId > 0 && pInstanceData[i].WorldTemplateId < INSTANCE_SUBWORLD_START)
		{
			m_InstanceId[pInstanceData[i].WorldTemplateId] = pInstanceData[i].InstanceId;
		}
	}

	return true;
}

void KPlayer::AddOnceCombatScore(int addscore)
{
	if (addscore <= 0)
		return ;

	if (m_CombatScoreOneTime + addscore < 0)
	{
		m_CombatScoreOneTime = 0x7fffffff;
	}
	else
	{
		m_CombatScoreOneTime += addscore;
	}
}

void KPlayer::DecOnecCombatScore(int decscore)
{
	if (decscore <= 0)
		return ;

	if (m_CombatScoreOneTime > decscore)
		m_CombatScoreOneTime -= decscore;
	else
		m_CombatScoreOneTime = 0;
}
#endif

#ifdef _SERVER
void KPlayer::ProcessGMOperation(BYTE* pProtocol)
{
	if (!IsGM())
		return;

	GM_OPERATION* pOperation = (GM_OPERATION*)pProtocol;
	char playerName[MAXSIZE_ROLENAME];
	memset(playerName, 0, sizeof(playerName));

	int nameSize = pOperation->wProtocolSize - (sizeof(GM_OPERATION) - 1 - sizeof(pOperation->PlayerName));
	if (nameSize <= 0 || nameSize > sizeof(pOperation->PlayerName))
		return;

	pOperation->PlayerName[nameSize - 1] = 0;
	int nameLength = pOperation->wProtocolSize - (sizeof(GM_OPERATION) - 1 - sizeof(pOperation->PlayerName));
	nameLength = nameLength < sizeof(playerName) ? nameLength : sizeof(playerName);
	strncpy(playerName, pOperation->PlayerName, nameLength);
	playerName[sizeof(playerName) - 1] = 0;
	
	int playerIndex = PlayerSet.GetPlayerIndexByPlayerName(playerName);
	if (!IsValidPlayer(playerIndex))
		return;

	KPlayer& targetPlayer = Player[playerIndex];

	switch(pOperation->OpType)
	{
	case gm_op_kick:
		{
			LogGMOperation(log_event_gm_kick_player, playerName);

			//踢下线
			targetPlayer.Kick();
		}		
		break;
	case gm_op_no_chat:
		{
			DWORD noChatSeconds = (DWORD)pOperation->OpParam1;

			//保存禁止聊天时间
			_DBProcHeader DBHeader = {0};
			DBHeader.ulNetID = m_nNetConnectIdx;
			DBHeader.ProcType = Proc_NoChat;
			
			IProcParam* pParam = g_pController->GetProcParam( );
			
			pParam->BeginPush( PN_NO_CHAT );
			pParam->Push( targetPlayer.GetPlayerName() );
			pParam->Push( noChatSeconds );
			pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
			
			if (g_pController)
				g_pController->CallProc( cfs_db_cnn_role, pParam );

			LogGMOperation(log_event_gm_no_chat, playerName, noChatSeconds);

			//禁止聊天
			targetPlayer.NoChat(noChatSeconds);
		}
		break;
	case gm_op_no_login:
		{
			DWORD noLoginSeconds = (DWORD)pOperation->OpParam1;
			
			//保存禁止登陆时间
			_DBProcHeader DBHeader = {0};
			DBHeader.ulNetID = m_nNetConnectIdx;
			DBHeader.ProcType = Proc_NoLogin;
			
			IProcParam* pParam = g_pController->GetProcParam( );
			
			pParam->BeginPush( PN_NO_LOGIN );
			pParam->Push( targetPlayer.GetPlayerName() );
			pParam->Push( noLoginSeconds );
			pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
			
			if (g_pController)
				g_pController->CallProc( cfs_db_cnn_role, pParam );

			LogGMOperation(log_event_gm_no_login, playerName, noLoginSeconds);

			//踢下线
			targetPlayer.Kick();
		}
		break;
	case gm_op_freeze_account:
		{			
			//通知FSEye冻结账号
			l2e_info info = { 0 };
			info.Header.Protocol = l2e_header_def;
			info.Protocol = l2e_info_def;
			snprintf(info.Info, sizeof(info.Info), "content=GMOperation\nOpType=FreezeAccount\nAccountName=%s\nRoleName=%s\nGM=%s\n", targetPlayer.m_AccoutName, playerName, GetPlayerName());
			info.Info[sizeof(info.Info) - 1] = 0;			
			if (g_pController != NULL)
				g_pController->PushData(protocol_type_guard, NULL, &info, sizeof(info));

			LogGMOperation(log_event_gm_freeze_account, playerName, 0, targetPlayer.m_AccoutName);
		}
		break;
	case gm_op_transfer:
		{
			DWORD mapId = (DWORD)pOperation->OpParam1;
			int posX = pOperation->OpParam2 * 32;
			int posY = pOperation->OpParam3 * 32;

			//传送
			if (!IsValidNpc(targetPlayer.GetNpcIndex()))
				return;
			
			Npc[targetPlayer.GetNpcIndex()].ChangeWorld(mapId, posX, posY);
				
			LogGMOperation(log_event_gm_transfer, playerName, mapId);
		}
		break;
	case gm_op_view_ip:
		{
			//发送IP给GM
			if (NULL == g_pServer)
				return;
			const char* szIP = g_pServer->GetClientInfo(targetPlayer.GetNetConnectIdx());

			char ipDesc[128] = { 0 };
			snprintf(ipDesc, sizeof(ipDesc), "%s IP:\n%s", targetPlayer.GetPlayerName(), szIP);

			ipDesc[sizeof(ipDesc) - 1] = 0;
			g_ChatCenterS.SysMsgToSomeone(GetPlayerIndex(), SYSMSG_TYPE_STR, (const BYTE*)ipDesc, strlen(ipDesc));

			LogGMOperation(log_event_gm_view_ip, playerName);
		}
		break;
	}
}

void KPlayer::LogGMOperation(LogEvent event, const char* szTargetPlayerName, DWORD opParam, const char* additionalInfo)
{
	if (NULL == g_pLogSystem || NULL == szTargetPlayerName)
		return;
	
	LogEventParam logParam;
	logParam.event = event;
	strncpy(logParam.param1.data, GetPlayerName(), sizeof(logParam.param1.data));
	logParam.param1.data[sizeof(logParam.param1.data) - 1] = 0;
	strncpy(logParam.param2.data, szTargetPlayerName, sizeof(logParam.param2.data));
	logParam.param2.data[sizeof(logParam.param2.data) - 1] = 0;

	if (additionalInfo)
	{
		strncpy(logParam.param3.data, additionalInfo, sizeof(logParam.param3.data));
		logParam.param3.data[sizeof(logParam.param3.data) - 1] = 0;
	}

	logParam.param4 = opParam;
	
	g_pLogSystem->Log(logParam);
}

bool KPlayer::RequireActivatePresent(const char * szPresentCode)
{
	if (!szPresentCode)
		return false;
	
	const size_t uBufferSize = (sizeof(KGameworldPaysysCommon) - 1) + sizeof (KAccountActivePresentCode)+ 1;
	
	BYTE Buffer[(sizeof(KGameworldPaysysCommon) - 1) + sizeof (KAccountActivePresentCode)+ 1];
	Buffer[0] = l2p_activate_present;
	
	KGameworldPaysysCommon *pUser = (KGameworldPaysysCommon *)(Buffer + 1);
	
	pUser->Size    =(sizeof(KGameworldPaysysCommon) - 1) + sizeof (KAccountActivePresentCode);
	pUser->Type    = AccountCommon;
	pUser->Version = ACCOUNT_CURRENT_VERSION;
    pUser->Operate = m_nNetConnectIdx;
	pUser->uDataSize = sizeof( KAccountActivePresentCode );

	KAccountActivePresentCode * pPresentCode = (KAccountActivePresentCode *)pUser->byData;
	pPresentCode->ProtocolType               = 50;

    strncpy(pPresentCode->Account, m_AccoutName, _NAME_LEN);
    pPresentCode->Account[_NAME_LEN - 1] = '\0';

	strncpy(pPresentCode->PresentCode,szPresentCode,sizeof(pPresentCode->PresentCode));
	pPresentCode->PresentCode[LOGIN_USER_PRESENT_CODE_MAX_LEN - 1] = 0;
    pPresentCode->dwActiveIP= m_dwLastLoginIP;
	
	g_pController->PushData( protocol_type_paysys, INVALID_VALUE, Buffer, uBufferSize );

	return true;
}

enum ACCOUNT_PRESENT_ACTIVE_CODE
{
	APEA_SUC = 1,
	APEA_FAILED = 2,
	APEA_INVALID_ACCOUNT = 3,
	APEA_INVALID_PARAM   = 1009,
	APEA_INVALID_PRESENT_CODE = 1500,
	APEA_PRESEND_USED_ALREADY = 1501,
	APEA_PRESENT_USED_EXPIRE_TIME = 1502,
};

void KPlayer::ProcessActivatePresentRet(KAccountActivePresentCodeRet * pRet )
{
	if (!pRet)
		return;
	
	if ( strcmp(m_AccoutName,pRet->Account) != 0)
		return;

	//继续后续交互脚本
	SetInteractiveScriptParam(0, pRet->nResult);
	SetInteractiveScriptParam(1, pRet->dwPresentType);
	InteractiveScriptNextStep();
 }

#endif

#ifndef _SERVER
int KPlayer::SendGMOperation(
	const char* szPlayerName,
	enumGMOpType opType,
	int opParam1,
	int opParam2,
	int opParam3) const
{
	if (!IsGM())
		return FALSE;

	GM_OPERATION operation;
	memset(&operation, 0, sizeof(operation));
	operation.Protocol = c2s_byte_extend;
	operation.ProtocolExtend = c2s_ex_protocol_gm;
	operation.wProtocolSize = sizeof(GM_OPERATION) - 1 - sizeof(operation.PlayerName);

	operation.OpType = opType;
	operation.OpParam1 = opParam1;
	operation.OpParam2 = opParam2;
	operation.OpParam3 = opParam3;
	
	if (szPlayerName)
	{
		strncpy(operation.PlayerName, szPlayerName, sizeof(operation.PlayerName));
		operation.PlayerName[sizeof(operation.PlayerName) - 1] = 0;
	}

	operation.wProtocolSize += strlen(operation.PlayerName) + 1;

	if (g_pClient)
		return g_pClient->SendPackToServer(g_ConnectID, &operation, operation.wProtocolSize + 1);
	else
		return FALSE;
}
#endif

DWORD KPlayer::ValidReturnDate( DWORD uReturnDate )
{
	if ( GetCreditState() != bad )
		return uReturnDate;		

	tm cur;
	time_t   clock;
	memcpy( &cur, localtime( ( const long * ) &uReturnDate ) ,sizeof(cur) );
	bool isInValidData = ( 108 == cur.tm_year ) && ( 0 == cur.tm_mon || 1 == cur.tm_mon );
	if ( isInValidData )
	{
		cur.tm_year++;
		SetCreditState( good );
	}
	clock = mktime(&cur);
	return clock;
}

#ifdef _SERVER

bool KPlayer::SendInvitationToFriends(const char * szTitle, const char * szContent)
{
	bool ret = false;
	if (szTitle != NULL && szContent != NULL)
	{
		ret = true;
		ChatObjectMgr_S * pObjMgr = g_ChatCenterS.GetChatObjMgr(m_nPlayerIndex);
		if (pObjMgr != NULL)
		{
			int maxObjCount = 0;
			PCHATOBJECT pObjList = pObjMgr->GetChatObjectList(maxObjCount);
			if (pObjList != NULL)
			{
				for (int i = 0; i < maxObjCount; ++i)
				{
					if (strcmp(pObjList[i].name, "") != 0)
					{
						unsigned int nameLength = sizeof(m_MarriageInfo.m_CoupleName);
						m_MarriageInfo.m_CoupleName[nameLength - 1] = 0;
						if (strncmp(pObjList[i].name, m_MarriageInfo.m_CoupleName, nameLength) != 0)
						{
							ret = SystemSendCustomMail(pObjList[i].name, szTitle, szContent, m_PlayerName);

							if (!ret)
							{
								break;
							}
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	return ret;
}

bool KPlayer::SendInvitationToMembers(const char * szTitle, const char * szContent)
{
	bool ret = false;
	if (szTitle != NULL && szContent != NULL)	
	{
		ret = true;
		SocialUnit * playerLeafUnit = GetLeafUnit(m_nPlayerIndex, enSUTplId_Tong);
		if (playerLeafUnit != NULL)
		{
			SocialUnit * playerSocialUnit = GetUpNUnit(playerLeafUnit, enSULayer_Tong);
			if (playerSocialUnit == NULL)
			{
				playerSocialUnit = GetUpNUnit(playerLeafUnit, enSULayer_Gens);
			}

			if (playerSocialUnit != NULL)
			{
				unsigned int nameLength = sizeof(m_MarriageInfo.m_CoupleName);
				m_MarriageInfo.m_CoupleName[nameLength - 1] = 0;
				ret = playerSocialUnit->SendInvitationToSubPlayer(szTitle, szContent, m_PlayerName, m_MarriageInfo.m_CoupleName);
			}
		}
	}
	return ret;
}

bool KPlayer::SendInvitation(const char * szTitle, const char * szContent)
{
	bool ret = false;
	if (szTitle != NULL && szContent != NULL)
	{
		ret = SendInvitationToFriends(szTitle, szContent);
		if (ret)
		{
			ret = SendInvitationToMembers(szTitle, szContent);
		}
	}

	return ret;
}

bool KPlayer::UnMarry()
{
	bool ret = false;
	if (strcmp(m_MarriageInfo.m_CoupleName, "") != 0)
	{
		if (Divorce() == TRUE)
		{
			ret = true;
		}
	}
	return ret;
}

bool KPlayer::DoMarry(KPlayer & couplePlayer)
{
	bool ret = false;
	if (&couplePlayer != this)
	{
		unsigned int nameLength = sizeof(m_MarriageInfo.m_CoupleName);
		if (strncmp(m_MarriageInfo.m_CoupleName, "", nameLength) == 0 &&
			strncmp(couplePlayer.m_MarriageInfo.m_CoupleName, "", nameLength) == 0)
		{
			//这里必须把配偶名字赋值，在之后的数据库操作返回的时候用
			strncpy(m_MarriageInfo.m_CoupleName, couplePlayer.m_PlayerName, nameLength);
			m_MarriageInfo.m_CoupleName[nameLength - 1] = 0;
			if (Marry(couplePlayer.m_PlayerName) == TRUE)
			{
				ret = true;
			}
		}
	}
	return ret;
}
#endif
