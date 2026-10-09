#include "KCore.h"

#ifdef _SERVER
#include "KEngine.h"
#include "KSubWorldSet.h"
#include "KSubWorld.h"
#include "KPlayer.h"

#include "KNpc.h"
#include "KItem.h"
#include "KItemList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "KNpcSet.h"
#include "KPlayerSet.h"
#include "KItemChangeRes.h"
#include <time.h>
#include "KNpcTemplate.h"

#include "cfs_fs2_savedef.h"
#include "cfs_db_interface.h"

#ifdef _SERVER
#include "ChatCenter_S.h"
#include "ServerSocialUnitMgr.h"
#include "kItemdateparser.h"
#include "employ.h"
#include "recommender.h"
#endif

#ifndef WIN32
#define min(x,y) x<y?x:y
#endif

#ifdef _SERVER
extern PQ_CONFIG			g_PQConfig;
#endif

#include "buff_man.h"

int	KPlayer::PlayerDbOpComplete( IProcRet* pRet )
{
	int nSize;
	int nRet = 0;
	int nRetValue = 0;

	_DBProcHeader* pHeader = (_DBProcHeader*)pRet->GetPassBy( nSize );

	if( nSize < sizeof(_DBProcHeader) )
		return FALSE;

	if( pHeader )
	{
		switch( pHeader->ProcType ) 
		{
		case Proc_GetRoleList:
			break;

		case Proc_DeleteRole:
			break;

		case Proc_CreateRole:
			break;

		case Proc_GetRoleBaseData:
			{
				return LoadBaseInfoData( pRet );
			}
			break;

		case Proc_SetRoleBaseData:
			{
				if( IsWaitingRemove( ) )
					m_bIsCanRemove = TRUE;
				
				if( pRet->GetRet( ) && pRet->GetExeRet( ) )
					CFS_FILELOGS::WriteDebugLog("Save role success!\n");
				else
					CFS_FILELOGS::WriteDebugLog("Save role error!\n");
			}
			break;

		case Proc_GetSkillData:
			{
				LoadSkillData( pRet );
			}
			break;

		case Proc_SetSkillData:
			{
				//save skill success
			}
			break;

		case Proc_GetItemData:
			{
				LoadItemData( pRet );
			}
			break;

		case Proc_SetItemData:
			{
				//save item success
			}
			break;

		case Proc_GetTaskData:
			{
				LoadTaskData( pRet );
			}
			break;

		case Proc_SetTaskData:
			{
				//save task success
			}
			break;

		case Proc_GetEnhanceData:
			{
				LoadEnhanceData( pRet );
			}
			break;

		case Proc_SetEnhanceData:
			{
				//save enhance success
			}
			break;

		case Proc_GetFriendData:
			{
				LoadFriendData( pRet );
			}
			break;

		case Proc_SetFriendData:
			{
				//save friend success
			}
			break;

		case Proc_GetReserveData:
			{
				LoadReserveData( pRet );
			}
			break;

		case Proc_SetReserveData:
			{
				//save reserve success
			}
			break;

		//auction
		case Proc_Auction:
			{
				m_serverAucMgr.ProcessDBRet(
					pRet->GetRet( ) && pRet->GetExeRet( ) ,
					Npc[m_nIndex].GetPlayerIdx(), 
					pRet );
			}
			break;
		//--------------
		//mail
		case Proc_Mail:
			{
				int nPlayerIdx = Npc[m_nIndex].GetPlayerIdx();
				MailManager_S	*pMailMgr = g_ChatCenterS.GetMailMgr(nPlayerIdx);
				
				if(pMailMgr)
					pMailMgr->ProcessDBRet( pRet->GetRet( ) && pRet->GetExeRet( ), nPlayerIdx, pRet );
			}
		//--------------
		case Proc_FindRole:
			{
				int nPlayerIdx = Npc[m_nIndex].GetPlayerIdx();
				g_ChatCenterS.ProcessDBOpeRet(
					pRet->GetExeRet( ) && pRet->GetRet( ), 
					nPlayerIdx,
					pRet );
			}
			break;
		case Proc_Social:
			{
				ServerSocialUnitMgr::Singleton().ProcessDBOpeRet(
					pRet->GetExeRet( ) && pRet->GetRet( ),
					Npc[m_nIndex].GetPlayerIdx(),
					pRet );
			}
			break;
		case Proc_RecordPlayerReward:
			{
			}
			break;
		case Proc_RecordPlayerContact:
			{
			}
			break;
			
		case Proc_GM_RecordPlayerQuestion:
			{
				
			}
			break;
		case Proc_GM_GetGMReply:
			{
				OnGetGMReplyRet( pRet );
			}
			break;
		case Proc_Post:
			{
				EmployCenter::Singleton().PostDbOpComplete( GetPlayerIndex(), pRet );
			}
			break;
		case Proc_Cancel:
			{
				EmployCenter::Singleton().CancelDbOpComplete( GetPlayerIndex(), pRet );
			}
			break;
		case Proc_Employ:
			{
				EmployCenter::Singleton().EmployDbOpComplete( GetPlayerIndex(), pRet );
			}
			break;
		case Proc_Pay:
			{
				EmployCenter::Singleton().PayDbOpComplete( GetPlayerIndex(), pRet );
			}
			break;
		case Proc_Fire:
			{
				EmployCenter::Singleton().FireDbOpComplete( GetPlayerIndex(), pRet );
			}
			break;
		case Proc_SearchEmploy:
			{
				EmployCenter::Singleton().SearchEmployDbOpComplete( GetPlayerIndex(), pRet );
			}
			break;
		case Proc_IB_AllData:
			{
				SyncAttribute(attr_jinshanbi);
				SyncAttribute(attr_maxcreditpoint);
				SyncAttribute(attr_creditpoint);
				SyncAttribute(attr_creditstate);
				SyncAttribute(attr_creditreturndata);
				SyncAttribute(attr_point);
			}
			break;
		case Proc_IB_AddCreditPoint:
			{
				if ( pRet->GetRet( ) && pRet->GetExeRet( ) )
				{
					m_moneyMgr.AddRet( creditpoint, pRet );
				}				
			}
			break;
		case Proc_IB_DecCreditPoint:
			{
				if ( pRet->GetRet( ) && pRet->GetExeRet( ) )
				{
					m_moneyMgr.DecRet( creditpoint, pRet );
					//InstantSave
					//SaveItemData();
				}
			}
			break;
		case Proc_IB_AddPoint:
			{
				if ( pRet->GetRet( ) && pRet->GetExeRet( ) )
				{
					m_moneyMgr.AddRet( point, pRet );
				}
			}
			break;
		case Proc_IB_DecPoint:
			{
				if ( pRet->GetRet( ) && pRet->GetExeRet( ) )
				{
					m_moneyMgr.DecRet( point, pRet );
					//InstantSave
					//SaveItemData();
				}				
			}
			break;
		case Proc_IB_Log_AddIbItem:
			break;
		case Proc_IB_Log_TransferIbItem:
			break;
		case Proc_IB_Log_DelIbItem:
			break;
		case Proc_IB_Log_LogIbItem:
			break;
		case Proc_GM_GetGMCmd:
			{
				OnGetGMCmdRet( pRet );
			}
			break;
		case Proc_Withdraw:
			{
				OnWithdrawRet( pRet );
			}
			break;

		case Proc_AddStudent:
			AddStudentDBRet ( pRet );
			break;
		case Proc_UpdateStudent:
			UpdateStudentDBRet( pRet );
			break;
		case Proc_ListStudent:
			ListStudentDBRet( pRet );
			break;
		case Proc_ListMaster:
			ListMasterDBRet( pRet );
			break;

		case Proc_SaveInsurance:
			break;

		case Proc_PlayerRealInfo:
			{
				m_serverPRIMgr.ProcessDBRet(
					pRet->GetExeRet( ) && pRet->GetRet( ),
					Npc[m_nIndex].GetPlayerIdx(),
					pRet);
			}
			break;
		case Proc_GetMarriageData:
			{
				GetMarriageDataRet( pRet );
			}
			break;
		case Proc_Marry:
			{
				MarryRet( pRet );
			}
			break;
		case Proc_Divorce:
			{
				DivorceRet( pRet );
			}
			break;

		default:
			break;
		}
		
		return TRUE;
	}
	return FALSE;
}

int	KPlayer::ParseBaseInfo( BYTE  * pRoleBuffer )
{
	PFS2DBBASECOLSET pRole = (PFS2DBBASECOLSET)pRoleBuffer;

	int		nSex		=	pRole->enRoleSex;
	int		nLevel		=	pRole->nRoleLevel;
	int		nRoleType	=	pRole->enRoleType;
	
	if (nSex)
		nSex = MAKELONG(nLevel, -(nRoleType + 4));
	else
		nSex = MAKELONG(nLevel, -(nRoleType + 1));

	//----------------------------------------------------------------------------------------
	//上线位置的确定
	//规则如下：
	//1.如果被标志为“使用重生地点”，则在重生地点上线
	//2.如果是副本并且副本存在，则在副本下线位置上线
	//3.如果不是副本，则在下线位置上线
	//4.如果上述第二和第三条规则发生问题，则在重生地点上线
	//5.如果在上述重生地点上线发生问题，则在指定的重生地点上线
	//----------------------------------------------------------------------------------------
	int enterMapId = pRole->nEnterMapID;
	int enterMapPosX = pRole->tagBaseInfoCol.nEnterMapX;
	int enterMapPosY = pRole->tagBaseInfoCol.nEnterMapY;
	m_EnterMapId = enterMapId;
	m_EnterMapPosX = enterMapPosX;
	m_EnterMapPosY = enterMapPosY;
	m_RememberSubworldId = pRole->tagBaseInfoCol.nRememberSubworldId;
	m_RememberPosX = pRole->tagBaseInfoCol.nRememberPosX;
	m_RememberPosY = pRole->tagBaseInfoCol.nRememberPosY;
	int onlineWorldIndex = INVALID_WORLD_INDEX;
	int onlineWorldPosX = enterMapPosX;
	int onlineWorldPosY = enterMapPosY;
	m_sLoginRevivalPos.m_nSubWorldID = pRole->nReviveMapID;
	m_sLoginRevivalPos.m_ReviveID = pRole->nReviveMapX;	

	//死亡重生地点确定
	POINT deathRevivePos;
	if (g_SubWorldSet.GetRevivalPosFromId(pRole->nReviveMapID, pRole->nReviveMapX, &deathRevivePos))
	{
		m_sDeathRevivalPos.m_nSubWorldID = pRole->nReviveMapID;
		m_sDeathRevivalPos.m_nMpsX = deathRevivePos.x;
		m_sDeathRevivalPos.m_nMpsY = deathRevivePos.y;
	}
	else
	{
		m_sDeathRevivalPos.m_nSubWorldID = defTRANSFER_PORT_ID;
		m_sDeathRevivalPos.m_nMpsX = defTRANSFER_PORT_X;
		m_sDeathRevivalPos.m_nMpsY = defTRANSFER_PORT_Y;
	}

	m_nIndex = 0;
	if (FALSE == pRole->bUseRevivePosition)
	{
		WorldSetting* pWorldSetting = g_SubWorldSet.GetWorldSetting(enterMapId);
		if (pWorldSetting)
		{
			if (pWorldSetting->IsInstance)
			{
				if (pWorldSetting->OfflineMode == 0)
				{
					//unsigned long instanceId = pRole->tagOtherInfoCol.dwInstanceId[enterMapId];
					DWORD instanceId = pRole->tagBaseInfoCol.dwEnterInstanceID;
					if (instanceId > 0)
					{
						onlineWorldIndex = g_SubWorldSet.GetInstance(instanceId);
					}
				}
				else if (pWorldSetting->OfflineMode == 1)
				{
					onlineWorldIndex = g_SubWorldSet.SearchWorld(m_RememberSubworldId);
					if (onlineWorldIndex != INVALID_WORLD_INDEX)
					{
						onlineWorldPosX = m_RememberPosX;
						onlineWorldPosY = m_RememberPosY;						
					}					
				}

				if (onlineWorldIndex != INVALID_WORLD_INDEX)
				{
					m_nIndex = NpcSet.Add(nSex, onlineWorldIndex, onlineWorldPosX, onlineWorldPosY, TRUE);
				}
			}
			else
			{
				onlineWorldIndex = g_SubWorldSet.SearchWorld(enterMapId);
				if (onlineWorldIndex != INVALID_WORLD_INDEX)
				{
					m_nIndex = NpcSet.Add(nSex, onlineWorldIndex, onlineWorldPosX, onlineWorldPosY, TRUE);
				}
				else 
				{
					CFS_FILELOGS::WriteDebugLog("World [%d] not found.\n", enterMapId);
				}
			}
		}
	}

	if (m_nIndex <= 0)
	{
		POINT revivePos;
		if (g_SubWorldSet.GetRevivalPosFromId(pRole->nReviveMapID, pRole->nReviveMapX, &revivePos))
		{
			onlineWorldIndex = g_SubWorldSet.SearchWorld(pRole->nReviveMapID);
			onlineWorldPosX = revivePos.x;
			onlineWorldPosY = revivePos.y;
			m_sLoginRevivalPos.m_nMpsX = onlineWorldPosX;
			m_sLoginRevivalPos.m_nMpsY = onlineWorldPosY;
			m_nIndex = NpcSet.Add(nSex, onlineWorldIndex, onlineWorldPosX, onlineWorldPosY, TRUE);
		}
	}

	if (m_nIndex <= 0)
	{
		onlineWorldIndex = g_SubWorldSet.SearchWorld(defTRANSFER_PORT_ID);
		onlineWorldPosX = defTRANSFER_PORT_X;
		onlineWorldPosY = defTRANSFER_PORT_Y;
		m_nIndex = NpcSet.Add(nSex, onlineWorldIndex, onlineWorldPosX, onlineWorldPosY, TRUE);
	}
	
	if(m_nIndex <= 0) 
	{
		CFS_FILELOGS::WriteDebugLog("Can not find a place to put player [%s].", pRole->szRoleName);
		return -1;
	}
	//----------------------------------------------------------------------------------------

	KNpc* pNpc = &Npc[m_nIndex];	
	pNpc->m_Kind = kind_player;
	pNpc->SetPlayerIdx(m_nPlayerIndex);
	pNpc->m_Level = nLevel;
	pNpc->m_Series	= nRoleType;
	
	strcpy( pNpc->Name, pRole->szRoleName );

	memcpy(&m_GUID, pRole->szGUID, sizeof(FSGUID));

	FSGUID	*pParentGuid = (FSGUID*)pRole->szTongGUID;
	if( IsGUIDValid(*pParentGuid) )
		m_RelationSet.Add(enSUTplId_Tong, *pParentGuid);

	m_dwPlayGameTime = pRole->unPlayedTime;
	m_dwLoginTime = UNIX_TMIE_STAMP;
	
	m_nEarnMoreMoneyP = 0;		// Get more money from Npc in percent
	m_nGetMoreExpP = 0;			// Get more experience in percent
	m_nGetMoreSkillExpP = 0;	
	
	m_bRandomAddAttr = 0;

	if(pNpc->m_Level % 3 == 1)
	{
		m_bRandomAddAttr = TRUE;
	}

	m_nWeightMax = pRole->tagNumericInfoCol.nWeightMax;
	m_nWeightMaxTempAdd = 0;
	m_PkValue = pRole->tagNumericInfoCol.nPKValue;

	pNpc->m_nMorphType = (short)-1;
	pNpc->m_nSafeGuardLevel = 0;	// 被保护
	pNpc->m_uMoveSpeed = 0;		// 移动速度
	pNpc->m_uPolyMorphTime = 0;	// 变身持续时间
	pNpc->m_bCanCast = 0;
	m_btMorphHue = 0;
	m_bMorphSendHue = false;
	m_nSkillExp = pRole->tagNumericInfoCol.nSkillExp;

	int nStrength	= pRole->tagNumericInfoCol.nStrength;

	pNpc->m_CompAttrMgr.Set(ncai_strength, idx_base_value, nStrength);
	pNpc->m_CompAttrMgr.Set(ncai_nimbus, idx_base_value, pRole->tagNumericInfoCol.nDexterity);
	pNpc->m_CompAttrMgr.Set(ncai_body, idx_base_value, pRole->tagNumericInfoCol.nConstitution);
	pNpc->m_CompAttrMgr.Set(ncai_art, idx_base_value, pRole->tagNumericInfoCol.nIntellect);

	//玩家等级信息
	m_nExp			= pRole->tagNumericInfoCol.unExp;
	pNpc->m_btRankId= 0;
	m_nWorldStat	= 0;
	
	//现金和贮物箱中的钱
	int nCashMoney = 0;
	int nSaveMoney = 0;

	// 重新初始化玩家的装备列表。
	m_ItemList.Init(GetPlayerIndex());

	nCashMoney		= pRole->unMoney;
	nSaveMoney		= pRole->unMoneyInBox;

	m_ItemList.SetMoney(nCashMoney, nSaveMoney,0);

	m_Ticket = pRole->unTicket;
	
	pNpc->m_nHeadImage = pRole->nPortrait;
	
	pNpc->m_Camp = camp_protect;
	pNpc->m_nSex	= pRole->enRoleSex;

	pNpc->m_CompAttrMgr.Set(ncai_lifeuplimit, idx_base_value, pRole->tagNumericInfoCol.nMaxLife);
	pNpc->m_CompAttrMgr.Set(ncai_manauplimit, idx_base_value, pRole->tagNumericInfoCol.nMaxMana);
	pNpc->m_CompAttrMgr.Set(ncai_liferenewspeed, idx_base_value, PLAYER_LIFE_REPLENISH);
	pNpc->m_CompAttrMgr.Set(ncai_manarenewspeed, idx_base_value, PLAYER_MANA_REPLENISH);

	SetBaseSpeedAndRadius();
	pNpc->RestoreNpcBaseInfo();

	Npc[m_nIndex].UpdateBodyEffect(0, Npc[m_nIndex].m_CompAttrMgr[ncai_body], false, false);
	Npc[m_nIndex].UpdateNimbusEffect(0, Npc[m_nIndex].m_CompAttrMgr[ncai_nimbus], false, false);
	Npc[m_nIndex].UpdateStrengthEffect(0, Npc[m_nIndex].m_CompAttrMgr[ncai_strength], false, false);
	Npc[m_nIndex].UpdateArtEffect(0, Npc[m_nIndex].m_CompAttrMgr[ncai_art], false, false);

	pNpc->m_UnaryAttrMgr.Set(nuai_curlife, pRole->tagNumericInfoCol.nLife);
	pNpc->m_UnaryAttrMgr.Set(nuai_curmana, pRole->tagNumericInfoCol.nMana);

	// 初始化部分数据（这些数据数据库不存储）
	m_BuyInfo.Clear();
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;

	memset(m_szTaskAnswerFun, 0, sizeof(m_szTaskAnswerFun));
	memset(m_szRelayCallbackFun, 0, sizeof(m_szRelayCallbackFun));
	m_nAvailableAnswerNum = 0;
	Npc[m_nIndex].m_ActionScriptID = 0;
	Npc[m_nIndex].m_TrapScriptID = 0;
	m_nViewEquipTime = 0;

	KLibOfBPT* pLibOfBPT = g_ItemGen.GetLibOfBPT( );
	const KBASICPROP_ITEM* pItemBP = NULL;
	
	pItemBP = pLibOfBPT->GetHelmRecord( 0 );
	pNpc->m_HelmType = pItemBP ?  pItemBP->nItemRes : 0;
	pNpc->m_HelmPal = g_ItemChangeRes.GetPal( BODY_PART_HELM, pItemBP->nParticularType, pItemBP->nLevel );
	pItemBP = pLibOfBPT->GetArmorRecord( 0 );
	pNpc->m_ArmorType = pItemBP ? pItemBP->nItemRes : 0;
	pNpc->m_ArmorPal = g_ItemChangeRes.GetPal( BODY_PART_ARMOR, pItemBP->nParticularType, pItemBP->nLevel );
	pItemBP = pLibOfBPT->GetWeaponRecord( 0 );
	pNpc->m_WeaponType = pItemBP ? pItemBP->nItemRes : 0;
	pNpc->m_WeaponPal = g_ItemChangeRes.GetPal( BODY_PART_WEAPON, pItemBP->nParticularType, pItemBP->nLevel );
	pItemBP = pLibOfBPT->GetShoulderRecord( 0 );
	pNpc->m_ShoulderType = pItemBP ? pItemBP->nItemRes : 0;
	pNpc->m_ShoulderPal = g_ItemChangeRes.GetPal( BODY_PART_SHOULDER, pItemBP->nParticularType, pItemBP->nLevel );
	pItemBP = pLibOfBPT->GetCuffRecord( 0 );
	pNpc->m_CuffType = pItemBP ? pItemBP->nItemRes : 0;
	pNpc->m_CuffPal = g_ItemChangeRes.GetPal( BODY_PART_CUFF, pItemBP->nParticularType, pItemBP->nLevel );
	pItemBP = pLibOfBPT->GetBootRecord( 0 );
	pNpc->m_BootType = pItemBP ? pItemBP->nItemRes : 0;
	pNpc->m_BootPal = g_ItemChangeRes.GetPal( BODY_PART_BOOT, pItemBP->nParticularType, pItemBP->nLevel );
	pItemBP = pLibOfBPT->GetHorseRecord( 0 );
	pNpc->m_HorseType = pItemBP ? pItemBP->nItemRes : 0;
	pNpc->m_HorsePal = g_ItemChangeRes.GetPal( BODY_PART_HORSE, pItemBP->nParticularType, pItemBP->nLevel );

	pNpc->m_bRideHorse = FALSE;

	strncpy(m_szBoxPassword, pRole->tagPswInfoCol.szBoxPassword,sizeof(m_szBoxPassword));
	m_szBoxPassword[sizeof(m_szBoxPassword) -1 ] = 0;

//  	m_autoUnlockTime = pRole->tagOffLineInfoCol.lAutoUnlockTime;
//  	m_autoUnlocking = pRole->tagOffLineInfoCol.bIsAutoUnlocking;

	m_offerPostId  = 0;
	m_applyPostId  = 0;
	m_missionCount = 0;
	
	Npc[m_nIndex].m_nSetEfficacyType = 0;

	m_dwUniqueId = pRole->unRoleID;	

	m_ActionDelayer.Init(m_nIndex);
	m_PlayerStatistic.Init(GetPlayerIndex());

	m_CreateTime = pRole->unCreateDate;
	m_dwLastOfflineTime = pRole->unLastPlayingDate;

	pNpc->m_WorldCombatOrg	= pRole->dwCombatOrg;
	m_CombatInfo.nScore		= pRole->dwCombatScore;
	m_NoChatTime			= pRole->dwNoChatTime + UNIX_TMIE_STAMP;

    m_TaisuiSys.LoadDB(&(pRole->tagOtherInfoCol),pRole->unVersion);

	m_SpyLevel = pRole->nSpyLevel;
	m_SkillSeries = (RoleSkillSeries)(pRole->nSkillSeries);
	m_EmployTime = pRole->dwEmployTime;
	m_dwTotolJinshabi = pRole->dwTotolJinshabi;
	m_dwRecentJinshabi = pRole->dwRecentJinshabi;
	m_dwRecentlyTime = pRole->dwRecentlyTime;
	m_MarriageInfo.m_MarriedTimes = pRole->tagNumericInfoCol.nMarriedTimes;

	//同步游戏点数
	SetIBMoney( jinshanbi, &(m_ExtPointInfo.dwLeftMoney) );
	SetIBMoneySize( creditpoint, LONG_MIN_LIMIT, pRole->nMaxCreditPoint );
	SetIBMoney( creditpoint, &(pRole->nCreditPoint) );
	SetCreditState( pRole->uCreditState );
	SetCreditReturnTime( ValidReturnDate( pRole->uReturnDate ) );
	SetIBMoneySize( point,0,ConfigManager::Singleton().GetIBGlobalVariable(ib_global_var_point_max));
	SetIBMoney( point, &(pRole->nPoint) );
	SetIBPlus( point, pRole->nPointPlus);

	CheckCreditState();

	SyncAttribute(attr_jinshanbi);
	SyncAttribute(attr_maxcreditpoint);
	SyncAttribute(attr_creditpoint);
	SyncAttribute(attr_creditstate);
	SyncAttribute(attr_creditreturndata);
	SyncAttribute(attr_point);
	SyncAttribute(attr_combatscore);

	SyncAttribute(attr_employtime);
	SyncAttribute(attr_gmflag);
	SyncAttribute(attr_passward_state);

	for ( int plusPointIdx = 0; plusPointIdx < MAX_PLUS_POINT_COUNT; ++plusPointIdx )
	{
		SyncAttribute((enumSyncAttribute)(attr_pluspoint0 + plusPointIdx));
	}
	
	return 1;
}

int	KPlayer::PackBaseInfo(BYTE * pRoleBuffer)
{

	PFS2DBBASECOLSET pRole = (PFS2DBBASECOLSET)pRoleBuffer;
	
	KNpc * pNpc = &Npc[m_nIndex];
	memset( pRole, 0, sizeof(FS2DBBASECOLSET) );

	//有点危险不过还好,大Buff向小Buffer拷贝
	strcpy(pRole->szRoleName, m_PlayerName);
	strcpy(pRole->szAccountName, m_AccoutName);
	memcpy(pRole->szGUID, &m_GUID, sizeof(FSGUID));

	DWORD onlineTime = GetOnlineTime();
	if (onlineTime == 0)
		onlineTime = 1;
	pRole->unPlayedTime = onlineTime;
	pRole->tagNumericInfoCol.nStrength	= pNpc->m_CompAttrMgr[ncai_strength][idx_base_value];
	pRole->tagNumericInfoCol.nDexterity =  pNpc->m_CompAttrMgr[ncai_nimbus][idx_base_value];
	pRole->tagNumericInfoCol.nConstitution = pNpc->m_CompAttrMgr[ncai_body][idx_base_value];
	pRole->tagNumericInfoCol.nIntellect	= pNpc->m_CompAttrMgr[ncai_art][idx_base_value];
	pRole->tagNumericInfoCol.unExp = m_nExp;
	pRole->tagNumericInfoCol.nWeightMax = m_nWeightMax;
	pRole->tagNumericInfoCol.nSkillExp = m_nSkillExp;
	pRole->tagNumericInfoCol.nPKValue = m_PkValue;
	pRole->tagNumericInfoCol.nMarriedTimes = m_MarriageInfo.m_MarriedTimes;

	pRole->nRoleLevel = pNpc->m_Level;
	pRole->nPortrait = pNpc->m_nHeadImage;

	//现金和贮物箱中的钱
	int nCashMoney = 0;
	int nSaveMoney = 0;
	nCashMoney  = m_ItemList.GetMoney(room_equipment);
	nSaveMoney	= m_ItemList.GetMoney(room_repository);
	
	pRole->unMoney		=	nCashMoney;
	pRole->unMoneyInBox	=	nSaveMoney;
	pRole->unTicket		=	m_Ticket;
	pRole->enRoleSex	=	(enCFSROLESEX)pNpc->m_nSex;
	pRole->enRoleType	=	(enumROLETYPE)pNpc->m_Series;
	pRole->tagNumericInfoCol.nMaxLife	= pNpc->m_CompAttrMgr[ncai_lifeuplimit][idx_base_value];
	pRole->tagNumericInfoCol.nMaxMana	= pNpc->m_CompAttrMgr[ncai_manauplimit][idx_base_value];

	if(pNpc->m_UnaryAttrMgr[nuai_curlife] > pNpc->m_CompAttrMgr[ncai_lifeuplimit])
		pNpc->m_UnaryAttrMgr.Set(nuai_curlife, pNpc->m_CompAttrMgr[ncai_lifeuplimit]);

	if(pNpc->m_UnaryAttrMgr[nuai_curmana] > pNpc->m_CompAttrMgr[ncai_manauplimit])
		pNpc->m_UnaryAttrMgr.Set(nuai_curmana, pNpc->m_CompAttrMgr[ncai_manauplimit]);

	pRole->tagNumericInfoCol.nLife		= pNpc->m_UnaryAttrMgr[nuai_curlife];
	pRole->tagNumericInfoCol.nMana		= pNpc->m_UnaryAttrMgr[nuai_curmana];

	pRole->nReviveMapID					= m_sLoginRevivalPos.m_nSubWorldID;
	pRole->nReviveMapX					= m_sLoginRevivalPos.m_ReviveID;
	pRole->nReviveMapY					= 0;

	pRole->bUseRevivePosition	=	m_bUseReviveIdWhenLogin;
	pRole->nEnterMapID			=	SubWorld[pNpc->m_SubWorldIndex].m_SubWorldID;

	pNpc->GetMpsPos(
		&pRole->tagBaseInfoCol.nEnterMapX, 
		&pRole->tagBaseInfoCol.nEnterMapY);
	pRole->tagBaseInfoCol.dwEnterInstanceID = SubWorld[pNpc->m_SubWorldIndex].GetInstanceId();

	pRole->tagBaseInfoCol.nRememberSubworldId = m_RememberSubworldId;
	pRole->tagBaseInfoCol.nRememberPosX = m_RememberPosX;
	pRole->tagBaseInfoCol.nRememberPosY = m_RememberPosY;

	strncpy( pRole->tagPswInfoCol.szBoxPassword , m_szBoxPassword ,sizeof(pRole->tagPswInfoCol.szBoxPassword));
	pRole->tagPswInfoCol.szBoxPassword[sizeof(pRole->tagPswInfoCol.szBoxPassword) - 1] = 0;

//    		pRole->tagOffLineInfoCol.lAutoUnlockTime = m_autoUnlockTime;
//     	 	pRole->tagOffLineInfoCol.bIsAutoUnlocking = m_autoUnlocking; 	 
	
	RelationRecord	*pRec = m_RelationSet.GetRelationByTemplate(enSUTplId_Tong);

	if(pRec)
		memcpy(pRole->szTongGUID, &pRec->ParentGuid, sizeof(pRole->szTongGUID));

	if( m_dwLastLoginIP != 0 )
		pRole->unLastPlayingIP	= m_dwLastLoginIP;

	pRole->unLastPlayingDate	= UNIX_TMIE_STAMP;

	pRole->dwCombatOrg = pNpc->m_WorldCombatOrg;
	pRole->dwCombatScore = m_CombatInfo.nScore;

	m_TaisuiSys.SaveDB(&(pRole->tagOtherInfoCol));

	pRole->nSpyLevel = m_SpyLevel;
	pRole->nSkillSeries = m_SkillSeries;
	pRole->dwEmployTime = m_EmployTime;

	return 1;
}


int	KPlayer::ParseItem(BYTE * pRoleBuffer ,int nSize )
{
	TDBItemData_Base*	pDBItemForPackage		= (TDBItemData_Base*)pRoleBuffer;
	TDBItemData_Base*	pDBItemForNotPackage	= (TDBItemData_Base*)pRoleBuffer;
	TDBItemData_Base*	pDBItemForCommon		= (TDBItemData_Base*)pRoleBuffer;
	if ( pDBItemForPackage && pDBItemForNotPackage )
	{
		m_ItemList.RemoveAll();																						//!< 清空该NPC身上的装备列表，为重新加载做准备
		KItemDateMgr	idMgr( pDBItemForCommon->nVersion, pDBItemForCommon );
		int				nItemCount		= pDBItemForCommon->nItemlistLength;										//!< nItemCount是保存数据库中载入的装备总数
		int				i				= 0;																		//!< 循环变量，用来遍历装备数据块取值范围( 0 <= x < nItemCount )
		int				nItemPos		= 0;																		//!< 实际处理的装备数据大小
		int				nMaxDBItemSize	= (nSize - idMgr.GetSizeOfTDBItemData() + idMgr.GetSizeOf_TDBItemData());	//!< 数据库中实际返回的最大装备数据大小
		
		if( nItemCount > 0 )
		{
			// 首先遍历数据库中的道具数据优先加载正在使用的包裹类道具
			KItemDateMgr	idMgrForPackage( pDBItemForPackage->nVersion, pDBItemForPackage );						//!< 用从数据库中载入的装备数据的版本号初始化装备数据解析器		
			for( i = 0; i < nItemCount; ++i)
			{
				nItemPos +=  idMgrForPackage.GetSizeOf_TDBItemData();	
				if (  nItemPos <= nMaxDBItemSize )
				{
					if ( idMgrForPackage.IsPackage() )
					{
						idMgrForPackage.Parse( m_ItemList );
					}	
					idMgrForPackage.Next();					
				}
			}
			
			// 其次遍历数据中不是包裹或者没有使用的包裹。
			KItemDateMgr	idMgrForNotPackage( pDBItemForNotPackage->nVersion, pDBItemForNotPackage );						//!< 用从数据库中载入的装备数据的版本号初始化装备数据解析器		
			nItemPos = 0;
			for( i = 0; i < nItemCount; ++i)
			{
				if (  nItemPos <= nMaxDBItemSize )
				{
					nItemPos += idMgrForNotPackage.GetSizeOf_TDBItemData();
					if ( !idMgrForNotPackage.IsPackage() )
					{
						idMgrForNotPackage.Parse( m_ItemList );
					}					
					idMgrForNotPackage.Next();
				}
			}		
		}

		//一定要最后加载快捷方式数据
		KImmediacyParam* pImmArray = m_ItemList.GetShortCut();
		if ( pImmArray )
		{
			for ( int nImmIdx = 0; nImmIdx < MAX_IMMEDIACY_ITEM; ++nImmIdx )
			{
				switch ( pDBItemForCommon->ImmData[nImmIdx].nImmediacyType )
				{
				case skill_immediacy_type:
					{
						pImmArray[nImmIdx].nImmediacyType = (int)skill_immediacy_type;
						pImmArray[nImmIdx].nPos				= pDBItemForCommon->ImmData[nImmIdx].nPos;
						pImmArray[nImmIdx].nID				= pDBItemForCommon->ImmData[nImmIdx].nID;
					}
					break;
				case item_immediacy_type:
					{
						pImmArray[nImmIdx].nImmediacyType = (int)item_immediacy_type;
						pImmArray[nImmIdx].nPos				= pDBItemForCommon->ImmData[nImmIdx].nPos;
						int nGenre = 0; int nDetail = 0; int nParticular = 0;
						SpliteHashId( pDBItemForCommon->ImmData[nImmIdx].nID, nGenre, nDetail, nParticular );
						int nItemIndex = 0;
						m_ItemList.FindSameParticularItem( nGenre, nDetail, nParticular, &nItemIndex );
						if ( nItemIndex > 0 && nItemIndex < MAX_ITEM )
						{
							pImmArray[nImmIdx].nID = Item[nItemIndex].GetID();
						}
					}
					break;
				default:
					{
						pImmArray[nImmIdx].nImmediacyType	= -1;
						pImmArray[nImmIdx].nPos				= -1;
						pImmArray[nImmIdx].nID				= -1;
					}
					break;
				}
			}
		}
		//同步快捷方式到客户端
		m_ItemList.SyncShortCut();

		return 1;
	}//*/
	return 0;
}

#define MAX_ITEM_PER_PLAYER 350//玩家最多拥有物品的数量

struct ItemBankInfo
{
	ItemTemplateId Id;
	int Count;
};

int	KPlayer::PackItem(BYTE* pRoleBuffer, int nSize)
{
	
	TDBItemData_Base*				pItemData	= (TDBItemData_Base*)pRoleBuffer;	
	int								nItemCount	= 0; //实际存储的道具数量
	int								nIdx		= 0; //道具数组	
	int								nBuffSize	= 0; //实际存储道具的大小(pListData大小)
	KItemDateMgr					idMgr( ITEM_VERSION, pItemData );

	memset(pRoleBuffer, 0, nSize);

	//装备版本号
	pItemData->nVersion = ITEM_VERSION;

	//存储快捷方式
	const KImmediacyParam* pImmArray = m_ItemList.GetShortCut();
	if ( pImmArray )
	{
		for ( int nImmIdx = 0; nImmIdx < MAX_IMMEDIACY_ITEM; ++nImmIdx )
		{
			switch ( pImmArray[nImmIdx].nImmediacyType )
			{
			case skill_immediacy_type:
				{
					pItemData->ImmData[nImmIdx].nImmediacyType	= (int)skill_immediacy_type;
					pItemData->ImmData[nImmIdx].nPos			= pImmArray[nImmIdx].nPos;
					pItemData->ImmData[nImmIdx].nID				= pImmArray[nImmIdx].nID;
				}
				break;
			case item_immediacy_type:
				{
					pItemData->ImmData[nImmIdx].nImmediacyType	= (int)item_immediacy_type;
					pItemData->ImmData[nImmIdx].nPos			= pImmArray[nImmIdx].nPos;
					int nItemIdx = m_ItemList.SearchID( pImmArray[nImmIdx].nID );
					if ( nItemIdx > 0 && nItemIdx < MAX_ITEM )
					{
						pItemData->ImmData[nImmIdx].nID = GenerateItemHashId( 
							Item[nItemIdx].GetGenre(), 
							Item[nItemIdx].GetDetailType(), 
							Item[nItemIdx].GetParticular() );
					}
					else
					{
						// to do
					}
				}
				break;
			default:
				{
					pItemData->ImmData[nImmIdx].nImmediacyType	= -1;
					pItemData->ImmData[nImmIdx].nPos			= -1;
					pItemData->ImmData[nImmIdx].nID				= -1;
				}
				break;
			}
		}
	}

	ItemBankInfo itemCountInfos[MAX_ITEM_PER_PLAYER];
	memset(itemCountInfos, 0, sizeof(itemCountInfos));

	//是否开启道具银行功能
	bool isItemBankOn = (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_item_bank_on));

	nBuffSize += idMgr.GetSizeOfTDBItemData() - idMgr.GetSizeOf_TDBItemData(); 

	//存储角色道具
	while( 1 )
	{
		nIdx = m_ItemList.m_UseIdx.GetNext(nIdx);
		if ( nIdx == 0 )
			break;
		int nItemIndex = m_ItemList.m_Items[nIdx].nIdx;
		if (nItemIndex <= 0 || nItemIndex >= MAX_ITEM || m_ItemList.m_Items[nIdx].nPlace == pos_traderoom || Item[nItemIndex].GetID() < 100)
		{
			continue;
		}

		KItem& item = Item[nItemIndex];
		int nItemSize = idMgr.GetSizeOf_TDBItemData();
		
		if ( nBuffSize + nItemSize <= nSize )
		{
			idMgr.Pack( m_ItemList, nIdx, item );
			nBuffSize += nItemSize;
			nItemCount++;
			pItemData->nItemlistLength = nItemCount;
			idMgr.Next();

			if (isItemBankOn)
			{
				//统计同种物品的数量
				ItemTemplateId templateId;
				item.GetItemTemplateId(templateId);
				for (int index = 0; index < MAX_ITEM_PER_PLAYER; index++)
				{
					ItemBankInfo& info = itemCountInfos[index];
					if (info.Count == 0)
					{
						info.Id = templateId;
						info.Count = item.GetItemCount();
						break;
					}
					else if (info.Id == templateId)
					{
						info.Count += item.GetItemCount();
						break;
					}
				}
			}
		}
		else
		{
			break;
		}
	}

	if (isItemBankOn)
	{
		//清除原有统计数据
		{
			_DBProcHeader DBHeader = {0};
			DBHeader.ulNetID = -1;
			DBHeader.ProcType = Proc_ClearItemCount;
			
			IProcParam* pParam = g_pController->GetProcParam( );
			
			if (pParam)
			{
				pParam->BeginPush( PN_CLEARITEMCOUNT );				
				pParam->Push( GetPlayerName() );
				pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
				
				g_pController->CallProc( cfs_db_cnn_log, pParam );
			}
		}

		//物品数量统计存盘
		for (int i = 0; i < MAX_ITEM_PER_PLAYER; i++)
		{
			ItemBankInfo& info = itemCountInfos[i];
			if (info.Count > 0)
			{
				_DBProcHeader DBHeader = {0};
				DBHeader.ulNetID = -1;
				DBHeader.ProcType = Proc_ItemCount;
				
				IProcParam* pParam = g_pController->GetProcParam( );
				
				if (pParam)
				{
					pParam->BeginPush( PN_ITEMCOUNT );
					pParam->Push( GetPlayerName() );
					pParam->Push( info.Id.IDArray[0] );
					pParam->Push( info.Id.IDArray[1] );
					pParam->Push( info.Id.IDArray[2] );
					pParam->Push( info.Id.IDArray[3] );
					pParam->Push( info.Count );
					pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
					
					g_pController->CallProc( cfs_db_cnn_log, pParam );
				}
			}
		}
	}

	if (pItemData->nItemlistLength <= 0)
	{
		return idMgr.GetSizeOfTDBItemData() - idMgr.GetSizeOf_TDBItemData();
	}
	else
	{
		return nBuffSize;
	}
}

int KPlayer::SaveIBData(  )
{
	int min = 0;
	int max = 0;
	GetIBMoneySize(creditpoint, min, max);

	_DBProcHeader DBHeader;
	memset(&DBHeader, 0, sizeof(_DBProcHeader) );
	DBHeader.ulNetID	= GetNetConnectIdx();
	DBHeader.ProcType	= Proc_IB_AllData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_ROLEDATA );
	pParam->Push( m_PlayerName );
	pParam->Push( GetIBMoney(creditpoint) );
	pParam->Push( max );
	pParam->Push( GetCreditState() );
	pParam->Push( GetCreditReturnTime() );
	pParam->Push( GetIBMoney(point));
	pParam->Push( GetIBPlus(point) );
	pParam->Push( GetTotolJinshanbi() );
	pParam->Push( GetRecentJinshanbi());
	pParam->Push( GetRecentTime() );

	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	return g_pController->CallProc( cfs_db_cnn_role, pParam );	
}

int KPlayer::SaveInsurance(  )
{
	if (NULL == g_pController)
		return FALSE;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID	= GetNetConnectIdx();
	DBHeader.ProcType	= Proc_SaveInsurance;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if (pParam)
	{
		pParam->BeginPush( PN_SET_INSURANCE_DATA );
		pParam->Push( m_PlayerName );
		pParam->Push( m_InsuranceMgr.GetCurrentInsuranceValue() );
		pParam->Push( m_InsuranceMgr.GetTotalMoneyGot() );
		pParam->Push( m_InsuranceMgr.GetMoneyLeftToGet() );
		
		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		return g_pController->CallProc( cfs_db_cnn_role, pParam );	
	}

	return FALSE;
}

int KPlayer::SaveBaseInfoData( )
{
	FS2DBBASECOLSET BaseInfo = {0};

	if( !PackBaseInfo( (BYTE*)&BaseInfo ) )
		return FALSE;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_SetRoleBaseData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_SETROLEBD );
	
	pParam->Push( 0 );	//id not use in procedure for keep space
	pParam->Push( BaseInfo.unVersion );
	pParam->Push( BaseInfo.szTongGUID );
	pParam->Push( BaseInfo.szAccountName );
	pParam->Push( BaseInfo.szRoleName );
	pParam->Push( (int)BaseInfo.enRoleSex );
	pParam->Push( (int)BaseInfo.enRoleType );
	pParam->Push( BaseInfo.nRoleLevel );

	pParam->Push( BaseInfo.unMoney );
	pParam->Push( BaseInfo.unMoneyInBox );

	pParam->Push( BaseInfo.unTicket );

	pParam->Push( BaseInfo.unPlayedTime );
	pParam->Push( BaseInfo.unCreateDate );

	pParam->Push( BaseInfo.unLastPlayingDate );
	pParam->Push( BaseInfo.unLastPlayingIP );

	pParam->Push( BaseInfo.bUseRevivePosition );

	pParam->Push( BaseInfo.nReviveMapID );
	pParam->Push( BaseInfo.nReviveMapX );
	pParam->Push( BaseInfo.nReviveMapY );
	pParam->Push( BaseInfo.nEnterMapID );


	pParam->Push( BinPair( (void*)&BaseInfo.tagBaseInfoCol, sizeof(BaseInfo.tagBaseInfoCol) ) );
	pParam->Push( BinPair( (void*)&BaseInfo.tagNumericInfoCol, sizeof(BaseInfo.tagNumericInfoCol) ) );
	pParam->Push( BinPair( (void*)&BaseInfo.tagPswInfoCol, sizeof(BaseInfo.tagPswInfoCol) ) );
	pParam->Push( BinPair( (void*)&BaseInfo.tagTongWarInfoCol, sizeof(BaseInfo.tagTongWarInfoCol) ) );
	pParam->Push( BinPair( (void*)&BaseInfo.tagPetInfoCol, sizeof(BaseInfo.tagPetInfoCol) ) );
	pParam->Push( NullPair( ) );
	pParam->Push( BinPair( (void*)&BaseInfo.tagCompensatoryInfoCol, sizeof(BaseInfo.tagCompensatoryInfoCol) ) );
	pParam->Push( BinPair( (void*)&BaseInfo.tagOtherInfoCol, sizeof(BaseInfo.tagOtherInfoCol) ) );
	pParam->Push( BinPair( (void*)&BaseInfo.tagReserveInfoCol, sizeof(BaseInfo.tagReserveInfoCol) ) );
	pParam->Push( BinPair( (void*)&BaseInfo.tagSettingInfoCol, sizeof(BaseInfo.tagSettingInfoCol) ) );
	pParam->Push( BaseInfo.szGUID );

	pParam->Push( BaseInfo.nPortrait );
	pParam->Push( BaseInfo.nSpyLevel );
	pParam->Push( BaseInfo.nSkillSeries );
	pParam->Push( BaseInfo.dwEmployTime );

	pParam->Push( BaseInfo.dwCombatOrg );
	pParam->Push( BaseInfo.dwCombatScore );

	pParam->Push( m_RecommenderRewardToAdd );
	pParam->Push( m_RecommenderRewardTicketAdded );

	FS2DBRESERVED1COL reserved1;
	memset(&reserved1, 0, sizeof(reserved1));
	m_QuestionState.GetState(
		reserved1.BadAnswerStartTime,
		reserved1.BadAnswerCount,
		reserved1.BadAnswerState,
		reserved1.BadAnswerStageStartTime,
		reserved1.LongTermBadAnswerStartTime,
		reserved1.LongTermBadAnswerCount);
	m_ExpInsuranceMgr.GetData(reserved1.ExpInsuranceCurRewardExp,reserved1.ExpInsuranceLastRewardTime);

	bool              bQuestValid = false;
	int               nQuestTime  = 0;

	m_QuestInsuranceMgr.GetData(nQuestTime,bQuestValid);
	reserved1.QuestInsuranceRewardValue = nQuestTime;
	reserved1.QuestInsurnaceValid       = bQuestValid ? 1:0;

	for ( int plusPointIdx = 0; plusPointIdx < MAX_PLUS_POINT_COUNT; ++plusPointIdx )
	{
		reserved1.PlusPoint[plusPointIdx] = m_plusPointArray[plusPointIdx];
	}

	for ( int plusPointRecordIdx = 0; plusPointRecordIdx < MAX_PLUS_POINT_COUNT; ++plusPointRecordIdx )
	{
		reserved1.PlusPointRecord[plusPointRecordIdx] = m_plusPointRecord[plusPointRecordIdx];
	}

	pParam->Push( BinPair( (void*)&reserved1, sizeof(reserved1) ) );

	char titleDataBuff[sizeof(FS2DB_TITLE_INFO_COL) + MAX_TITLE_COUNT * sizeof(FS2DB_TITLE_INFO)];
	int titleDataBuffSize = sizeof(titleDataBuff);
	memset(titleDataBuff, 0, titleDataBuffSize);
	m_TitleManager.SaveState((BYTE*)titleDataBuff, titleDataBuffSize);
	pParam->Push( BinPair( (void*)&titleDataBuff, titleDataBuffSize ) );	

// 	pParam->Push( BaseInfo.nMaxCreditPoint );
// 	pParam->Push( BaseInfo.nCreditPoint );
// 	pParam->Push( BaseInfo.uCreditState );
// 	pParam->Push( BaseInfo.uReturnDate );
// 	pParam->Push( BaseInfo.nPoint );	
// 	pParam->Push( BaseInfo.nPointPlus );
// 	pParam->Push( BaseInfo.dwTotolJinshabi );
// 	pParam->Push( BaseInfo.dwRecentJinshabi );
// 	pParam->Push( BaseInfo.dwRecentlyTime );	

	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::LoadBaseInfoData( IProcRet* pRet )
{
	int nRet = 0;
	int nRetValue = 0;
	
	FS2DBBASECOLSET BaseInfo = {0};
	int insuranceCurrentValue = 0;
	int insuranceMoneyTotal = 0;
	int insuranceMoneyLeft = 0;

	int nCol = 0;

	pRet->GetData( 0, nCol++, BaseInfo.unRoleID );
	pRet->GetData( 0, nCol++, BaseInfo.unVersion );

	pRet->GetData( 0, nCol++, BaseInfo.szTongGUID, sizeof( BaseInfo.szTongGUID ) );
	pRet->GetData( 0, nCol++, BaseInfo.szAccountName, sizeof( BaseInfo.szAccountName ) );
	pRet->GetData( 0, nCol++, BaseInfo.szRoleName, sizeof( BaseInfo.szRoleName ) );

	pRet->GetData( 0, nCol++, *((int*)&BaseInfo.enRoleSex) );
	pRet->GetData( 0, nCol++, *((int*)&BaseInfo.enRoleType) );
	pRet->GetData( 0, nCol++, BaseInfo.nRoleLevel );

	pRet->GetData( 0, nCol++, BaseInfo.unMoney );
	pRet->GetData( 0, nCol++, BaseInfo.unMoneyInBox );

	pRet->GetData( 0, nCol++, BaseInfo.unTicket );

	pRet->GetData( 0, nCol++, BaseInfo.unPlayedTime );
	pRet->GetData( 0, nCol++, BaseInfo.unCreateDate );

	pRet->GetData( 0, nCol++, BaseInfo.unLastPlayingDate );
	pRet->GetData( 0, nCol++, BaseInfo.unLastPlayingIP );

	pRet->GetData( 0, nCol++, BaseInfo.bUseRevivePosition );

	pRet->GetData( 0, nCol++, BaseInfo.nReviveMapID );
	pRet->GetData( 0, nCol++, BaseInfo.nReviveMapX );
	pRet->GetData( 0, nCol++, BaseInfo.nReviveMapY );
	pRet->GetData( 0, nCol++, BaseInfo.nEnterMapID );


	pRet->GetData( 0, nCol++, (void*)&BaseInfo.tagBaseInfoCol, sizeof( BaseInfo.tagBaseInfoCol ) );
	pRet->GetData( 0, nCol++, (void*)&BaseInfo.tagNumericInfoCol, sizeof( BaseInfo.tagNumericInfoCol ) );
	pRet->GetData( 0, nCol++, (void*)&BaseInfo.tagPswInfoCol, sizeof( BaseInfo.tagPswInfoCol ) );
	pRet->GetData( 0, nCol++, (void*)&BaseInfo.tagTongWarInfoCol, sizeof( BaseInfo.tagTongWarInfoCol ) );
	pRet->GetData( 0, nCol++, (void*)&BaseInfo.tagPetInfoCol, sizeof( BaseInfo.tagPetInfoCol ) );
	nCol++;//pRet->GetData( 0, nCol++, (void*)&BaseInfo.tagOffLineInfoCol, sizeof( BaseInfo.tagOffLineInfoCol ) );
	pRet->GetData( 0, nCol++, (void*)&BaseInfo.tagCompensatoryInfoCol, sizeof( BaseInfo.tagCompensatoryInfoCol ) );
	pRet->GetData( 0, nCol++, (void*)&BaseInfo.tagOtherInfoCol, sizeof( BaseInfo.tagOtherInfoCol ) );
	pRet->GetData( 0, nCol++, (void*)&BaseInfo.tagReserveInfoCol, sizeof( BaseInfo.tagReserveInfoCol ) );
	pRet->GetData( 0, nCol++, (void*)&BaseInfo.tagSettingInfoCol, sizeof( BaseInfo.tagSettingInfoCol ) );
	pRet->GetData( 0, nCol++, BaseInfo.szGUID, sizeof( BaseInfo.szGUID ) );

	pRet->GetData( 0, nCol++, BaseInfo.nPortrait );
	pRet->GetData( 0, nCol++, BaseInfo.nSpyLevel );
	pRet->GetData( 0, nCol++, BaseInfo.nSkillSeries );
	pRet->GetData( 0 ,nCol++, BaseInfo.dwEmployTime );
	
	pRet->GetData( 0 ,nCol++, BaseInfo.dwNoChatTime );
	pRet->GetData( 0 ,nCol++, BaseInfo.dwCombatOrg );
	pRet->GetData( 0 ,nCol++, BaseInfo.dwCombatScore );

	//Lucifer~yu(zhangjianyu) 03/18/2008 Modify for IB Version
	//Begin-------------------------------------------------------------------
	pRet->GetData( 0, nCol++, BaseInfo.nCreditPoint );
	pRet->GetData( 0, nCol++, BaseInfo.nMaxCreditPoint );
	pRet->GetData( 0, nCol++, BaseInfo.uCreditState );
	pRet->GetData( 0, nCol++, BaseInfo.uReturnDate );
	pRet->GetData( 0, nCol++, BaseInfo.nPoint );
	pRet->GetData( 0 ,nCol++, BaseInfo.nPointPlus);
	pRet->GetData( 0, nCol++, BaseInfo.dwTotolJinshabi );
	pRet->GetData( 0, nCol++, BaseInfo.dwRecentJinshabi );
	pRet->GetData( 0 ,nCol++, BaseInfo.dwRecentlyTime);
	
	//End---------------------------------------------------------------------

	pRet->GetData( 0 ,nCol++, insuranceCurrentValue );
	pRet->GetData( 0 ,nCol++, insuranceMoneyTotal );
	pRet->GetData( 0 ,nCol++, insuranceMoneyLeft );
	pRet->GetData( 0 ,nCol++, m_RecommenderRewardToAdd );
	pRet->GetData( 0 ,nCol++, m_RecommenderRewardTicketAdded );

	int gmFlag = FALSE;
	pRet->GetData( 0 ,nCol++, gmFlag );
	m_IsGM = (TRUE == gmFlag);

	FS2DBRESERVED1COL reserved1;
	memset(&reserved1, 0, sizeof(reserved1));
	pRet->GetData( 0 ,nCol++, (void*)&reserved1, sizeof(reserved1));
	m_QuestionState.SetState(
		reserved1.BadAnswerStartTime, 
		reserved1.BadAnswerCount,
		reserved1.BadAnswerState,
		reserved1.BadAnswerStageStartTime,
		reserved1.LongTermBadAnswerStartTime,
		reserved1.LongTermBadAnswerCount);

	char* pTitleData = NULL;
	int titleDataSize = pRet->GetData(0, nCol++, &pTitleData);
	m_TitleManager.Init(GetPlayerIndex());
	m_TitleManager.LoadState((const unsigned char*)pTitleData, titleDataSize);

	m_InsuranceMgr.Init(m_nPlayerIndex);
	m_InsuranceMgr.LoadDBRet(insuranceCurrentValue, insuranceMoneyTotal, insuranceMoneyLeft);

	m_ExpInsuranceMgr.Init(m_nPlayerIndex);
	m_ExpInsuranceMgr.SetData(reserved1.ExpInsuranceCurRewardExp,reserved1.ExpInsuranceLastRewardTime);
	
	m_QuestInsuranceMgr.Init(m_nPlayerIndex);
	m_QuestInsuranceMgr.SetData(reserved1.QuestInsuranceRewardValue,reserved1.QuestInsurnaceValid ? true:false);

	for ( int plusPointIdx = 0; plusPointIdx < MAX_PLUS_POINT_COUNT; ++plusPointIdx )
	{
		m_plusPointArray[plusPointIdx] = reserved1.PlusPoint[plusPointIdx];
	}

	for ( int plusPointRecordIdx = 0; plusPointRecordIdx < MAX_PLUS_POINT_COUNT; ++plusPointRecordIdx )
	{
		m_plusPointRecord[plusPointRecordIdx] = reserved1.PlusPointRecord[plusPointRecordIdx];
	}

	if ((nRet = ParseBaseInfo( (BYTE*)&BaseInfo )) == 1)
	{
		nRetValue = SendSyncData( Proc_GetRoleBaseData );
	}
	else 
	{
		if (nRet == -1)
		{
			return FALSE;
		}
		else
		{
			nRetValue = SendSyncData( Proc_GetRoleBaseData );
		}
	}

	m_DBLoadProcessFlag |= enPDBMask_BaseInfo;

	return TRUE;
}

int KPlayer::SaveSkillData( )
{
	char szBuffer[SAVETEMPBUFLEN];
	int nSize = Npc[m_nIndex].m_SkillList.SaveSkillData( (BYTE*)szBuffer );

	if( nSize <= 0 )
		return FALSE;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_SetSkillData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_SETROLEDATA );
	pParam->Push( m_PlayerName );
	pParam->Push( COLNAMESKILL );
	pParam->Push( BinPair( szBuffer, nSize ) );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::LoadSkill( )
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_GetSkillData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_GETROLEDATA );
	pParam->Push( m_PlayerName );
	pParam->Push( COLNAMESKILL );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::LoadSkillData( IProcRet* pRet )
{
	int nRetValue;
	char* pData;
	int nSize = pRet->GetData( 0, 0, &pData );

	m_DBLoadProcessFlag |= enPDBMask_Skill;

	if( Npc[m_nIndex].m_SkillList.LoadSkillData((BYTE*)pData, nSize) )
	{
		Npc[m_nIndex].m_SkillList.CastAllValueSkill();
		Npc[m_nIndex].m_SkillList.CastAllPassiveSkill();
		
		nRetValue = SendSyncData_Skill();

		return TRUE;
	}
	else
		return FALSE;
}

int KPlayer::SaveItemData( )
{
	char szBuffer[SAVETEMPBUFLEN];
	int nSize = PackItem( (BYTE*)szBuffer, SAVETEMPBUFLEN );
	
	if( nSize <= 0 )
		return FALSE;
	
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_SetItemData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_SETROLEDATA );
	pParam->Push( m_PlayerName );
	pParam->Push( COLNAMEITEM );
	pParam->Push( BinPair( szBuffer, nSize ) );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::LoadItem(  )
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_GetItemData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_GETROLEDATA );
	pParam->Push( m_PlayerName );
	pParam->Push( COLNAMEITEM );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::LoadItemData( IProcRet* pRet )
{
	char* pData;
	int nSize = pRet->GetData( 0, 0, &pData );	
	
	m_DBLoadProcessFlag |= enPDBMask_Item;
	
	if( ParseItem( (BYTE*)pData, nSize ) )
	{	
		return TRUE;
	}
	else
		return FALSE;
}

int KPlayer::SaveTaskData( )
{
	char szBuffer[SAVETEMPBUFLEN];
	int nSize = m_cTask.Save((BYTE*)szBuffer);
	
	if( nSize <= 0 )
		return FALSE;
	
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_SetTaskData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_SETROLEDATA );
	pParam->Push( m_PlayerName );
	pParam->Push( COLNAMETASK );
	pParam->Push( BinPair( szBuffer, nSize ) );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::LoadTask( )
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_GetTaskData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_GETROLEDATA );
	pParam->Push( m_PlayerName );
	pParam->Push( COLNAMETASK );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::LoadTaskData( IProcRet* pRet )
{
	char* pData;
	int nSize = pRet->GetData( 0, 0, &pData );
	
	m_DBLoadProcessFlag |= enPDBMask_Task;
	
	if( m_cTask.Load( (BYTE*)pData, nSize) )
	{	
		return TRUE;
	}
	else
		return FALSE;
}

int KPlayer::SaveEnhanceData( )
{
	char szBuffer[SAVETEMPBUFLEN];
	int nSize = 0;

	BuffMgr& BM = BuffMgr::Singleton( );
	BM.SaveBuff( m_nIndex, (BYTE*)szBuffer, nSize );
	
	if( nSize <= 0 )
		return FALSE;
	
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_SetEnhanceData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_SETROLEDATA );
	pParam->Push( m_PlayerName );
	pParam->Push( COLNAMEENHAN );
	pParam->Push( BinPair( szBuffer, nSize ) );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::LoadEnhance( )
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_GetEnhanceData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_GETROLEDATA );
	pParam->Push( m_PlayerName );
	pParam->Push( COLNAMEENHAN );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::LoadEnhanceData( IProcRet* pRet )
{
	char* pData;
	int nSize = pRet->GetData( 0, 0, &pData );
	
	m_DBLoadProcessFlag |= enPDBMask_Buff;
	
	BuffMgr& BM = BuffMgr::Singleton( );

	if( BM.LoadBuff(m_nIndex, (BYTE*)pData,	nSize ) )
	{	
		return TRUE;
	}
	else
		return FALSE;
}

int KPlayer::SaveFriendData( )
{
	int nPlayerIdx = Npc[m_nIndex].GetPlayerIdx( );
	g_ChatCenterS.SaveFriendsDataReq( nPlayerIdx );	
	return TRUE;
}

int KPlayer::LoadFriend( )
{
	// Load friends data
	int nPlayerIdx = Npc[m_nIndex].GetPlayerIdx();	
	if(nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER)
	{
		g_ChatCenterS.PlayerOnLine(nPlayerIdx);
	}
	return TRUE;
}

int KPlayer::LoadFriendData( IProcRet* pRet )
{
	char* pData;
	int nSize = pRet->GetData( 0, 0, &pData );	
	
	m_DBLoadProcessFlag |= enPDBMask_Friend;
	
	g_ChatCenterS.LoadFriendsDataRet( 
		Npc[m_nIndex].GetPlayerIdx( ), 
		(BYTE*)pData,
		nSize,
		pRet->GetRet( ) && pRet->GetExeRet( ) );
	
	return TRUE;
}

int KPlayer::SaveReserveData( )
{
	char szBuffer[sizeof(DBInstanceData) * INSTANCE_SUBWORLD_START + 1];
	memset(szBuffer, 0, sizeof(szBuffer));

	int nSize = m_InstanceInfo.Save( (BYTE*)szBuffer, sizeof(szBuffer) );

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx( );
	DBHeader.ProcType = Proc_SetReserveData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_SETROLEDATA );
	pParam->Push( m_PlayerName );
	pParam->Push( COLNAMERESVE );
	
	if (nSize > 0)
	{
		pParam->Push( BinPair( (void*)szBuffer, nSize ) );
	}
	else
	{
		pParam->Push( NullPair( ) );
	}

	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::LoadReserve( )
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_GetReserveData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_GETROLEDATA );
	pParam->Push( m_PlayerName );
	pParam->Push( COLNAMERESVE );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::LoadReserveData( IProcRet* pRet )
{
	char* pData = NULL;
	int nSize = pRet->GetData( 0, 0, &pData );
	
	m_DBLoadProcessFlag |= enPDBMask_Reserve;

	if (nSize > 0 && pData != NULL)
	{
		m_InstanceInfo.Load( (BYTE*)pData, nSize );

		ValidateInstance();
		ValidatePlayerState();
		
// 		WorldSetting* pWorldSetting = g_SubWorldSet.GetWorldSetting(m_EnterMapId);
// 		if (pWorldSetting)
// 		{
// 			if (pWorldSetting->IsInstance)
// 			{
// 				unsigned long instanceId = m_InstanceId[m_EnterMapId];
// 				if (instanceId > 0)
// 				{
// 					int onlineWorldIndex = g_SubWorldSet.GetInstance(instanceId);
// 					if (onlineWorldIndex != INVALID_WORLD_INDEX)
// 					{
// 						DelayedAction transferAction(GetPlayerIndex(), delayed_action_transfer, 6, 0);
// 						DelayedActionParamTransfer& param = transferAction.GetTransferParam();
// 						param.m_Accept = true;
// 						param.m_CanDeny = false;
// 						param.m_IsInstance = true;
// 						param.m_TransferID = instanceId;
// 						param.m_PosX = m_EnterMapPosX;
// 						param.m_PosY = m_EnterMapPosY;
// 						GetActionDelayer().NewAction(transferAction, true);
// 						//Npc[GetNpcIndex()].ChangeWorld(instanceId, m_EnterMapPosX, m_EnterMapPosY, true);
// 					}
// 				}
// 			}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int KPlayer::SaveSocialData( )
{
	int nPlayerIdx = Npc[m_nIndex].GetPlayerIdx();
	ServerSocialUnitMgr::Singleton().SaveUnitsRelToPlayer(nPlayerIdx);
	return TRUE;
}

int KPlayer::LoadSocial( )
{
	// Load friends data
	int nPlayerIdx = Npc[m_nIndex].GetPlayerIdx();	
	if(nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER)
	{
		ServerSocialUnitMgr::Singleton().PlayerOnLine(nPlayerIdx);
	}
	return TRUE;
}

int KPlayer::LoadSocialData( IProcRet* pRet )
{
	return TRUE;
}

int KPlayer::DBOnline( )
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_Online;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_ONLINE );
	pParam->Push( GetPlayerName() );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::DBOffline( )
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_Offline;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_OFFLINE );
	pParam->Push( GetPlayerName() );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::RecordPlayerReward( unsigned int rewardTag )
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_RecordPlayerReward;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_RECORD_PLAYER_REWARD );
	pParam->Push( m_PlayerName );
	pParam->Push( rewardTag );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int KPlayer::RecordPlayerContact( unsigned int rewardTag, const char* realName, unsigned int sex, const char* tel, const char* eMail, const char* address, const char* code )
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_RecordPlayerContact;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_RECORD_PLAYER_CONTACT );
	pParam->Push( m_PlayerName );
	pParam->Push( rewardTag );
	pParam->Push( realName );
	pParam->Push( sex );
	pParam->Push( tel );
	pParam->Push( eMail );
	pParam->Push( address );
	pParam->Push( code );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

/************************GM留言板begin*************************/
int KPlayer::RecordPlayerQuestion( unsigned int questionType, const char* question )
{
	if(!g_pController)
	{
		return 0;
	}

	IProcParam* pParam = g_pController->GetProcParam( );
	if(!pParam)
	{
		return 0;
	}

	if(!question)
	{
		return 0;
	}

	const char* account = m_AccoutName;
	const char* roleName = m_PlayerName;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_GM_RecordPlayerQuestion;
	
	pParam->BeginPush( PN_RECORD_PLAYER_QUESTION );
	pParam->Push( account );
	pParam->Push( roleName );
	pParam->Push( questionType );
	pParam->Push( BinPair(question, strlen(question) ) );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	return g_pController->CallProc( cfs_db_cnn_log, pParam );
}

int KPlayer::GetGMReply( )
{
	if(!g_pController)
	{
		return 0;
	}

	IProcParam* pParam = g_pController->GetProcParam( );
	if(!pParam)
	{
		return 0;
	}

	const char* roleName = m_PlayerName;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = m_nNetConnectIdx;
	DBHeader.ProcType = Proc_GM_GetGMReply;

	pParam->BeginPush( PN_GET_GM_REPLY );
	pParam->Push( roleName );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	return g_pController->CallProc( cfs_db_cnn_log, pParam );
}

void KPlayer::OnGetGMReplyRet( IProcRet* pRet )
{
	if( !pRet )
	{
		return;
	}

	int	retCount = pRet->GetRowCount();
	if(!retCount)
	{
		return;
	}
	
	char* replayData = NULL;
	int size = pRet->GetData( 0, 0, &replayData );	
	
	if( !size || !replayData )
	{
		return;
	}

	char sendBuff[sizeof(GM_COMMUNICATION_DATA) * 2];
	memset(sendBuff, 0, sizeof(sendBuff));
	PGM_COMMUNICATION_DATA pFeedBack = (PGM_COMMUNICATION_DATA)sendBuff;
	pFeedBack->Protocol = s2c_gm_feedback_msg;
	pFeedBack->Length = sizeof(GM_COMMUNICATION_DATA) - sizeof(pFeedBack->Protocol) - sizeof(pFeedBack->data.msg);
	
	if( sizeof( pFeedBack->data.msg ) > size )
	{
		strncpy( pFeedBack->data.msg, replayData, size );
		pFeedBack->data.msg[size] = 0;
	}
	else
	{
		strncpy( pFeedBack->data.msg, replayData, sizeof( pFeedBack->data.msg ) );
		pFeedBack->data.msg[ sizeof( pFeedBack->data.msg ) - 1 ] = 0;
	}

	pFeedBack->Length += (strlen(pFeedBack->data.msg) + 1);

	if (g_pServer)
	{
		g_pServer->PackDataToClient( m_nNetConnectIdx, sendBuff, pFeedBack->Length + 1 );
	}
}
/************************GM留言板end*************************/

int KPlayer::GetGMCmd( )
{
	if(!g_pController)
		return 0;

	IProcParam* pParam = g_pController->GetProcParam( );
	if(!pParam)
		return 0;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx();
	DBHeader.ProcType = Proc_GM_GetGMCmd;

	pParam->BeginPush( PN_GET_GM_CMD );
	pParam->Push( GetPlayerName() );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	return g_pController->CallProc( cfs_db_cnn_log, pParam );
}

void KPlayer::OnGetGMCmdRet( IProcRet* pRet )
{
	if( !pRet )
		return;
	
	if ( !(pRet->GetRet( ) && pRet->GetExeRet( )) )
		return;
	
	int	rowCount = pRet->GetRowCount( );
	if(rowCount <= 0)
		return;
	
	char command[256] = { 0 };
	for (int row = 0; row < rowCount; row++)
	{
		pRet->GetData( row, 0, command, sizeof(command) );
		command[sizeof(command) - 1] = 0;
		
		BOOL result = TextGMFilter(GetPlayerIndex(), command, strlen(command));

		//记录日志
		if (g_pLogSystem)
		{
			char exeGMInfo[sizeof(command) + 64];
			snprintf(exeGMInfo, sizeof(exeGMInfo), "Player=\"%s\" Cmd=\"%s\"", GetPlayerName(), command);
			exeGMInfo[sizeof(exeGMInfo) - 1] = 0;

			g_pLogSystem->SysDbgLog(exeGMInfo, strlen(exeGMInfo), sys_dbg_log_event_exe_player_get_gm_cmd);
		}
	}
}

#define MAX_SN_LENGTH 64

int KPlayer::Withdraw( const char* pSN )
{
	if (!pSN)
		return 0;

	char sn[MAX_SN_LENGTH] = { 0 };
	strncpy(sn, pSN, sizeof(sn));
	sn[sizeof(sn) - 1] = 0;

	if(!g_pController)
		return 0;

	IProcParam* pParam = g_pController->GetProcParam( );
	if(!pParam)
		return 0;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx();
	DBHeader.ProcType = Proc_Withdraw;

	pParam->BeginPush( PN_WITHDRAW );
	pParam->Push( sn );	
	pParam->Push( m_AccoutName );
	pParam->Push( GetPlayerName() );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	return g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void KPlayer::OnWithdrawRet( IProcRet* pRet )
{
	int result = 0;
	int nTag = 0;
	int nValue = 0;
	
	if( pRet && pRet->GetRet( ) && pRet->GetExeRet( ) )
	{
		int	rowCount = pRet->GetRowCount( );
		if(rowCount == 1)
		{
			int nCol = 0;
			int nRow = 0;
			
			pRet->GetData( nRow, nCol++, nTag );
			pRet->GetData( nRow, nCol++, nValue );

			result = 1;
		}
	}

	//继续后续交互脚本
	SetInteractiveScriptParam(0, result);
	SetInteractiveScriptParam(1, nTag);
	SetInteractiveScriptParam(2, nValue);
	InteractiveScriptNextStep();
}

#define DEFAULT_RECOMMENDER_EXPIRE_DAY 10//默认推荐人过期天数
#define DEFAULT_RECOMMENDER_MAX_STUDENT_COUNT 20//默认推荐人徒弟数量

int KPlayer::AddStudent(const char* pRecommenderName)
{
	int result = 0;

	if (NULL == pRecommenderName)
		return result;
	
	if (!CanDoOp(player_op_add_student))
	{
		result = 2;
		return result;
	}
	UpdateOpTime(player_op_add_student, DEFAULT_RECOMMENDER_OP_INTERVAL);

	//检查被推荐人的条件（自己/徒弟）
	if (RecommenderSystem::Singleton().CheckStudentRequirements(GetPlayerIndex()) == 0)
	{
		result = 3;
		return result;
	}
	
	char recommenderName[MAXSIZE_ROLENAME] = { 0 };
	strncpy(recommenderName, pRecommenderName, sizeof(recommenderName));
	
	int recommenderPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(recommenderName);
	if (IsValidPlayer(recommenderPlayerIndex))
	{
		KPlayer& masterPlayer = Player[recommenderPlayerIndex];

		//检查推荐人的条件（师傅）
		if (RecommenderSystem::Singleton().CheckMasterRequirements(recommenderPlayerIndex) == 0)
		{
			result = 5;
			return result;
		}

		int expireDay = ConfigManager::Singleton().GetGlobalVariable(global_var_recommender_expire_day);
		if (expireDay <= 0)
			expireDay = DEFAULT_RECOMMENDER_EXPIRE_DAY;

		int maxStudentCount = ConfigManager::Singleton().GetGlobalVariable(global_var_recommender_max_student_count);
		if (maxStudentCount <= 0)
			maxStudentCount = DEFAULT_RECOMMENDER_MAX_STUDENT_COUNT;
		
		_DBProcHeader DBHeader = {0};
		DBHeader.ulNetID = GetNetConnectIdx();
		DBHeader.ProcType = Proc_AddStudent;
		
		IProcParam* pParam = g_pController->GetProcParam( );
		
		pParam->BeginPush( PN_ADD_STUDENT );
		pParam->Push( recommenderName );
		pParam->Push( GetPlayerName() );
		pParam->Push( GetLevel() );
		pParam->Push( expireDay );
		pParam->Push( maxStudentCount );
		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		if (g_pController)
			result = g_pController->CallProc( cfs_db_cnn_role, pParam );
		
		if (result && g_pLogSystem && TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_recommender))
		{
			LogEventParam logEvent;
			logEvent.event = log_event_recommend_try_add_student;
			logEvent.param1 = GetGUID();
			logEvent.param2 = masterPlayer.GetGUID();
			g_pLogSystem->Log(logEvent);
		}
	}
	else
	{
		result = 4;
	}
	
	return result;
}

int KPlayer::UpdateStudent()
{
	int result = 0;
	
	if (!CanDoOp(player_op_update_student))
	{
		return result;
	}
	UpdateOpTime(player_op_update_student, DEFAULT_RECOMMENDER_OP_INTERVAL);
	
	int expireDay = ConfigManager::Singleton().GetGlobalVariable(global_var_recommender_expire_day);
	if (expireDay <= 0)
		expireDay = DEFAULT_RECOMMENDER_EXPIRE_DAY;
	
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx();
	DBHeader.ProcType = Proc_UpdateStudent;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_UPDATE_STUDENT );
	pParam->Push( GetPlayerName() );
	pParam->Push( GetLevel() );
	pParam->Push( expireDay );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	if (g_pController)
		result = g_pController->CallProc( cfs_db_cnn_role, pParam );
	
	if (result && g_pLogSystem && TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_recommender))
	{
		LogEventParam logEvent;
		logEvent.event = log_event_recommend_try_update_student;
		logEvent.param1 = GetGUID();
		g_pLogSystem->Log(logEvent);
	}
	
	return result;
}

int KPlayer::ListStudent()
{
	int result = 0;

	if (!CanDoOp(player_op_list_student))
	{
		return result;
	}
	UpdateOpTime(player_op_list_student, DEFAULT_RECOMMENDER_OP_INTERVAL);

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx();
	DBHeader.ProcType = Proc_ListStudent;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_LIST_STUDENT );
	pParam->Push( GetPlayerName() );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	if (g_pController)
		result = g_pController->CallProc( cfs_db_cnn_role, pParam );

    GetUIServerState().SetUIState(player_ui_recommender_strudent,player_ui_state_open);

	return result;
}

int KPlayer::ListMaster()
{
	int result = 0;

	if (!CanDoOp(player_op_list_master))
	{
		return result;
	}
	UpdateOpTime(player_op_list_master, DEFAULT_RECOMMENDER_OP_INTERVAL);
	
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx();
	DBHeader.ProcType = Proc_ListMaster;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_LIST_MASTER );
	pParam->Push( GetPlayerName() );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	if (g_pController)
		result = g_pController->CallProc( cfs_db_cnn_role, pParam );

	GetUIServerState().SetUIState(player_ui_recommender_master,player_ui_state_open);

	return result;
}

// int KPlayer::GetMasterReward()
// {	
// 	int result = 0;
// 
// 	if (!CanDoOp(player_op_get_master_reward))
// 	{
// 		return result;
// 	}
// 	UpdateOpTime(player_op_get_master_reward, DEFAULT_RECOMMENDER_OP_INTERVAL);
// 	
// 	_DBProcHeader DBHeader = {0};
// 	DBHeader.ulNetID = GetNetConnectIdx();
// 	DBHeader.ProcType = Proc_GetMasterReward;
// 	
// 	IProcParam* pParam = g_pController->GetProcParam( );
// 	
// 	pParam->BeginPush( PN_GET_MASTER_REWARD );
// 	pParam->Push( GetPlayerName() );
// 	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
// 	
// 	if (g_pController)
// 		result = g_pController->CallProc( cfs_db_cnn_role, pParam );
// 	
// 	if (result && g_pLogSystem && TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_recommender))
// 	{
// 		LogEventParam logEvent;
// 		logEvent.event = log_event_recommend_try_get_master_reward;
// 		logEvent.param1 = GetGUID();
// 		g_pLogSystem->Log(logEvent);
// 	}
// 
// 	return result;
// }

void KPlayer::AddStudentDBRet( IProcRet* pRet )
{
	if (NULL == pRet || 0 == pRet->GetExeRet())
		return;

	int result = pRet->GetRet();
	switch (result)
	{
	case 0://失败
		ShowPredefinedMsg(11350);
		break;
	case 1://成功
		{
			if (pRet->GetRowCount() == 1)
			{
				char masterName[MAXSIZE_ROLENAME] = { 0 };
				pRet->GetData(0, 0, masterName, sizeof(masterName));
				masterName[sizeof(masterName) - 1] = 0;

				int masterPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(masterName);
				if (IsValidPlayer(masterPlayerIndex))
				{
					//通知师傅
					Player[masterPlayerIndex].ShowPredefinedMsg(11353);
				}
			}

			//通知徒弟
			ShowPredefinedMsg(11352);
		}
		break;
	case 2://推荐人已经达到最大人数
		ShowPredefinedMsg(11351);
		break;
	case 3://已经有了推荐人
		ShowPredefinedMsg(11359);
		break;
	}
	
	if (g_pLogSystem && TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_recommender))
	{
		LogEventParam logEvent;
		logEvent.event = log_event_recommend_add_student_result;
		logEvent.param1 = GetGUID();		
		snprintf(logEvent.param2.data, sizeof(logEvent.param2.data), "%d", result);
		g_pLogSystem->Log(logEvent);
	}
}

void KPlayer::UpdateStudentDBRet( IProcRet* pRet )
{
	if (NULL == pRet || 0 == pRet->GetExeRet())
		return;

	int result = pRet->GetRet();
	if (result)//成功
	{
// 		if (pRet->GetRowCount() == 1)
// 		{
// 			char masterName[MAXSIZE_ROLENAME] = { 0 };
// 			pRet->GetData(0, 0, masterName, sizeof(masterName));
// 			masterName[sizeof(masterName) - 1] = 0;
// 			
// 			int masterPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(masterName);
// 			if (IsValidPlayer(masterPlayerIndex))
// 			{
// 				//通知师傅
// 				Player[masterPlayerIndex].ShowPredefinedMsg(11356);
// 			}
// 		}

		//通知徒弟
		ShowPredefinedMsg(11355);
	}
	else//失败
	{
		//通知徒弟
		ShowPredefinedMsg(11354);
	}

	if (g_pLogSystem && TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_recommender))
	{
		LogEventParam logEvent;
		logEvent.event = log_event_recommend_update_student_result;
		logEvent.param1 = GetGUID();		
		snprintf(logEvent.param2.data, sizeof(logEvent.param2.data), "%d", result);
		g_pLogSystem->Log(logEvent);
	}
}

void KPlayer::ListStudentDBRet( IProcRet* pRet )
{
	if (NULL == pRet || 0 == pRet->GetExeRet())
		return;

	if (pRet->GetRet())//成功
	{
		char sendBuff[MAX_LIST_STUDENT_BUFF_LENGTH];
		memset(sendBuff, 0, sizeof(sendBuff));
		LIST_STUDENT* pListStudent = (LIST_STUDENT*)sendBuff;
		pListStudent->Protocol = s2c_list_student;
		pListStudent->Length = sizeof(LIST_STUDENT) - sizeof(pListStudent->StudentInfoData) - 1;
		pListStudent->StudentCount = 0;
		StudentInfo* pStudentInfo = pListStudent->StudentInfoData;

		int rowCount = pRet->GetRowCount();
		if (rowCount > MAX_STUDENT_COUNT)
			rowCount = MAX_STUDENT_COUNT;

		int allStudentReward = 0;

		for (int row = 0; row < rowCount; row++)
		{
			int col = 0;
			DWORD rewardMoeny = 0;
			DWORD totalRewardMoney = 0;
			DWORD studentLevel = 1;
			//int leftTime = 0;

			pRet->GetData(row, col++, pStudentInfo->Name, sizeof(pStudentInfo->Name));
			pRet->GetData(row, col++, rewardMoeny);
			pRet->GetData(row, col++, totalRewardMoney);
			pRet->GetData(row, col++, studentLevel);
			//pRet->GetData(row, col++, leftTime);

			//if (leftTime < 0)
			//	leftTime = 0;

			allStudentReward += rewardMoeny;

			pStudentInfo->Name[sizeof(pStudentInfo->Name) - 1] = 0;
			pStudentInfo->RewardMoeny = rewardMoeny;
			pStudentInfo->TotalRewardMoney = totalRewardMoney + rewardMoeny;
			pStudentInfo->Level = studentLevel;
			//pStudentInfo->LeftTime = (DWORD)leftTime;

			pListStudent->Length += sizeof(StudentInfo);
			pListStudent->StudentCount++;
			pStudentInfo++;
		}

		AddRecommenderReward(allStudentReward);

		pListStudent->TotalRewardToAdd = m_RecommenderRewardToAdd;
		pListStudent->TotalRewardTicketAdded = m_RecommenderRewardTicketAdded;
		
		if (CompressProtocol((BYTE*)sendBuff, pListStudent->Length + 1, sizeof(sendBuff)))
		{
			if (g_pServer != NULL)
				g_pServer->PackDataToClient(GetNetConnectIdx(), sendBuff, pListStudent->Length + 1);
		}
	}
	else//失败
	{
	}
}

void KPlayer::ListMasterDBRet( IProcRet* pRet )
{
	if (NULL == pRet || 0 == pRet->GetExeRet())
		return;

	if (pRet->GetRet())//成功
	{
		int rowCount = pRet->GetRowCount();

		if (rowCount == 1)
		{
			char masterName[MAXSIZE_ROLENAME] = { 0 };
			DWORD studentLastlevel = 0;

			int col = 0;
			pRet->GetData(0, col++, masterName, sizeof(masterName));
			pRet->GetData(0, col++, studentLastlevel);
			
			masterName[sizeof(masterName) - 1] = 0;

			PLAYER_SCRIPTACTION_SYNC UiInfo;
			memset(&UiInfo, 0, sizeof(UiInfo));
			UiInfo.m_bOptionNum = 0;
			UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
			UiInfo.m_bUIId = UI_STUDENT_REPORT;
			UiInfo.m_bParam2 = 1;

			snprintf(UiInfo.m_pContent, sizeof(UiInfo.m_pContent), "%d|%d|%s", studentLastlevel, GetLevel(), masterName);
			UiInfo.m_nBufferLen = strlen(UiInfo.m_pContent) + 1;

			DoScriptAction(&UiInfo);
		}
		else
		{
			ShowPredefinedMsg(11349);
		}
	}
	else//失败
	{
	}
}

// void KPlayer::GetMasterRewardDBRet( IProcRet* pRet )
// {
// 	if (NULL == pRet || 0 == pRet->GetExeRet())
// 		return;
// 
// 	bool hasReward = false;
// 	int result = pRet->GetRet();
// 	if (result)//成功
// 	{
// 		int rowCount = pRet->GetRowCount();
// 		if (rowCount == 1)
// 		{
// 			int rewardMoney = 0;
// 			pRet->GetData( 0, 0, rewardMoney);
// 
// 			if (rewardMoney > 0)
// 			{
// //原来奖励金钱，现在奖励代金券
// // 				if (Earn(rewardMoney))
// // 				{
// // 					if (g_pLogSystem && rewardMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
// // 					{
// // 						LogEventParam logParam;
// // 						logParam.event = log_event_recommend_master_reward_add_money;
// // 						logParam.param1 = GetGUID();
// // 						logParam.param4 = rewardMoney;
// // 						g_pLogSystem->Log(logParam);
// // 					}
// // 
// // 					hasReward = true;
// // 				}
// 				
// 				if (TRUE == RecommenderSystem::Singleton().Reward(GetPlayerIndex(), rewardMoney))
// 				{
// 					hasReward = true;
// 				}
// 			}
// 			else
// 			{
// 			}
// 		}
// 	}
// 	else//失败
// 	{
// 	}
// 
// 	//通知师傅
// 	ShowPredefinedMsg(hasReward ? 11357 : 11358);
// 
// 	if (g_pLogSystem && TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_recommender))
// 	{
// 		LogEventParam logEvent;
// 		logEvent.event = log_event_recommend_get_master_reward_result;
// 		logEvent.param1 = GetGUID();		
// 		snprintf(logEvent.param2.data, sizeof(logEvent.param2.data), "%d", result);
// 		g_pLogSystem->Log(logEvent);
// 	}
// }

//-----------------------------------------------------------------------------------
#endif

#ifdef _SERVER
int KPlayer::GetMarriageData(const char* playerName)
{
	if (!IsValidNpc(GetNpcIndex()))
		return FALSE;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx();
	DBHeader.ProcType = Proc_GetMarriageData;

	IProcParam* pParam = g_pController->GetProcParam( );
	if (NULL == pParam)
		return FALSE;

	pParam->BeginPush( PN_GET_MARRIAGE_DATA );
	if (0 == Npc[GetNpcIndex()].GetSex())//自己是男人
	{
		pParam->Push( GetPlayerName() );//丈夫
		pParam->Push( NullPair() );//妻子
	}
	else//自己是女人
	{	
		pParam->Push( NullPair() );//丈夫
		pParam->Push( GetPlayerName() );//妻子
	}
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	if (g_pController)
	{
		return g_pController->CallProc( cfs_db_cnn_role, pParam );
	}
	else
	{
		return FALSE;
	}
}

void KPlayer::GetMarriageDataRet( IProcRet* pRet )
{
	if (NULL == pRet || 0 == pRet->GetExeRet())
	{
		//数据库操作失败
		return;
	}

	if (0 == pRet->GetRet())//没有结婚
	{
		//数据库中没有信息
		m_MarriageInfo.ResetInfo();
	}
	else//结婚了
	{
		char husbandName[32] = { 0 };
		char wifeName[32] = { 0 };
		DWORD marryTime = 0;
		DWORD lastOfflineTime = 0;//对方最后下线时间
		int col = 0;
		pRet->GetData( 0, col++, husbandName, sizeof(husbandName) );
		pRet->GetData( 0, col++, wifeName, sizeof(wifeName) );
		pRet->GetData( 0, col++, marryTime );
		pRet->GetData( 0, col++, lastOfflineTime );

		if (lastOfflineTime != 0)
		{
			if (0 == Npc[GetNpcIndex()].GetSex())
			{
				strncpy(m_MarriageInfo.m_CoupleName, wifeName, sizeof(m_MarriageInfo.m_CoupleName));	
			}
			else
			{
				strncpy(m_MarriageInfo.m_CoupleName, husbandName, sizeof(m_MarriageInfo.m_CoupleName));
			}
			m_MarriageInfo.m_CoupleLastOffLineTime = lastOfflineTime;
			m_MarriageInfo.m_MarriageTime = marryTime;
		}
		else
		{
			Divorce();
			int msgID = COUPLE_DEAD_MSG_ID;
			g_ChatCenterS.SysMsgToSomeone(m_nPlayerIndex, SYSMSG_TYPE_ID, (const BYTE *)&msgID, sizeof(msgID));
		}
	}
}

int KPlayer::Marry( const char* playerName )
{
	if (!IsValidNpc(GetNpcIndex()))
		return FALSE;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx();
	DBHeader.ProcType = Proc_Marry;

	IProcParam* pParam = g_pController->GetProcParam( );
	if (NULL == pParam)
		return FALSE;

	pParam->BeginPush( PN_MARRY );
	if (0 == Npc[GetNpcIndex()].GetSex())//自己是男人
	{
		pParam->Push( GetPlayerName() );//丈夫
		pParam->Push( playerName );//妻子
	}
	else//自己是女人
	{	
		pParam->Push( playerName );//丈夫
		pParam->Push( GetPlayerName() );//妻子
	}
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	if (g_pController)
	{
		return g_pController->CallProc( cfs_db_cnn_role, pParam );
	}
	else
	{
		return FALSE;
	}
}

void KPlayer::MarryRet( IProcRet* pRet )
{
	if (NULL == pRet || 0 == pRet->GetExeRet())
	{
		//数据库操作失败
		return;
	}

	if (0 == pRet->GetRet())//结婚失败
	{
		//提示客户端操作失败
		int msgID = MARRY_FAILED_MSG_ID;
		g_ChatCenterS.SysMsgToSomeone(m_nPlayerIndex, SYSMSG_TYPE_ID, (const BYTE *)&msgID, sizeof(msgID));

		unsigned int nameLength = sizeof(m_MarriageInfo.m_CoupleName);
		m_MarriageInfo.m_CoupleName[nameLength - 1] = 0;
		int coupleIndex = g_PlayerInfoToIndex.GetIndexByName(m_MarriageInfo.m_CoupleName);
		if (IsValidPlayer(coupleIndex))
		{
			g_ChatCenterS.SysMsgToSomeone(coupleIndex, SYSMSG_TYPE_ID, (const BYTE *)&msgID, sizeof(msgID));
		}
	}
	else//结婚成功
	{
		char husbandName[32] = { 0 };
		char wifeName[32] = { 0 };
		DWORD marryTime = 0;
		int col = 0;
		pRet->GetData( 0, col++, husbandName, sizeof(husbandName) );
		pRet->GetData( 0, col++, wifeName, sizeof(wifeName) );
		pRet->GetData( 0, col++, marryTime );

		if (m_MarriageInfo.m_MarriedTimes <= MAX_MARRIED_TIMES)
		{
			++m_MarriageInfo.m_MarriedTimes;
		}
		else
		{
			m_MarriageInfo.m_MarriedTimes = MAX_MARRIED_TIMES;
		}

		int coupleIndex = INVALID_PLAYER_INDEX;
		if (0 == Npc[GetNpcIndex()].GetSex())
		{
			coupleIndex = g_PlayerInfoToIndex.GetIndexByName(wifeName);
		}
		else
		{
			coupleIndex = g_PlayerInfoToIndex.GetIndexByName(husbandName);
		}

		if (IsValidPlayer(coupleIndex))
		{
			if (Player[coupleIndex].m_MarriageInfo.m_MarriedTimes <= MAX_MARRIED_TIMES)
			{
				++Player[coupleIndex].m_MarriageInfo.m_MarriedTimes;
			}
			else
			{
				Player[coupleIndex].m_MarriageInfo.m_MarriedTimes = MAX_MARRIED_TIMES;
			}
			Player[coupleIndex].GetMarriageData(NULL);
		}
		GetMarriageData(NULL);
	}
}

int KPlayer::Divorce( )
{
	if (!IsValidNpc(GetNpcIndex()))
		return FALSE;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx();
	DBHeader.ProcType = Proc_Divorce;

	IProcParam* pParam = g_pController->GetProcParam( );
	if (NULL == pParam)
		return FALSE;

	pParam->BeginPush( PN_DIVORCE );
	if (0 == Npc[GetNpcIndex()].GetSex())//自己是男人
	{
		pParam->Push( GetPlayerName() );//丈夫
		pParam->Push( NullPair() );//妻子
	}
	else//自己是女人
	{
		pParam->Push( NullPair() );//丈夫
		pParam->Push( GetPlayerName() );//妻子
	}
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	if (g_pController)
	{
		return g_pController->CallProc( cfs_db_cnn_role, pParam );
	}
	else
	{
		return FALSE;
	}
}

void KPlayer::DivorceRet( IProcRet* pRet )
{
	if (NULL == pRet || 0 == pRet->GetExeRet())
	{
		//数据库操作失败
		return;
	}

	if (0 == pRet->GetRet())//离婚失败
	{
		//提示客户端操作错误
		int msgID = DIVORCE_FAILED_MSG_ID;
		g_ChatCenterS.SysMsgToSomeone(m_nPlayerIndex, SYSMSG_TYPE_ID, (const BYTE *)&msgID, sizeof(msgID));

		unsigned int nameLength = sizeof(m_MarriageInfo.m_CoupleName);
		m_MarriageInfo.m_CoupleName[nameLength - 1] = 0;
		int coupleIndex = g_PlayerInfoToIndex.GetIndexByName(m_MarriageInfo.m_CoupleName);
		if (IsValidPlayer(coupleIndex))
		{
			g_ChatCenterS.SysMsgToSomeone(coupleIndex, SYSMSG_TYPE_ID, (const BYTE *)&msgID, sizeof(msgID));
		}
	}
	else//离婚成功
	{
		char husbandName[32] = { 0 };
		char wifeName[32] = { 0 };
		DWORD marryTime = 0;
		int col = 0;
		pRet->GetData( 0, col++, husbandName, sizeof(husbandName) );
		pRet->GetData( 0, col++, wifeName, sizeof(wifeName) );
		pRet->GetData( 0, col++, marryTime );

		int coupleIndex = INVALID_PLAYER_INDEX;
		if (0 == Npc[GetNpcIndex()].GetSex())
		{
			coupleIndex = g_PlayerInfoToIndex.GetIndexByName(wifeName);
		}
		else
		{
			coupleIndex = g_PlayerInfoToIndex.GetIndexByName(husbandName);
		}

		if (IsValidPlayer(coupleIndex))
		{
			Player[coupleIndex].m_MarriageInfo.ResetInfo();
		}
		m_MarriageInfo.ResetInfo();
	}
}
#endif
