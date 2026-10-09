//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-6-12 10:33
//      File_base        : KProtocolProcess
//      File_ext         : cpp
//      Author           : 
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#include "KEngine.h"
#include "KCore.h"

#ifndef _SERVER
#include "KCache.h"
#include "KWavSound.h"
#include "KOption.h"
#include "networkinterface.h"
#include "CoreShell.h"
#include "KViewItem.h"
#include "QueryInfo.h"
#endif
#include "KObjSet.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#ifndef _SERVER
#include "KPlayerTeam_C.h"
#else
#include "KPlayerTeam_S.h"
#endif
#include "KNpc.h"
#include "KSubWorld.h"
#include "LuaFuns.h"
#include "KProtocolProcess.h"
#include "KItemSet.h"
#include "KBuySell.h"
#include "KSmithShop.h"
#include "KSubWorldSet.h"
#include "Scene/ObstacleDef.h"
#include "KMath.h"
#ifndef _SERVER
#include "Scene\KScenePlaceC.h"
#endif
#include "KItemEnchaser.h"
#include "KItemList.h"
#ifndef _SERVER
#include "KSkills.h"
#include "buff_tab.h"
#include "KClientFuryMgr.h"
#endif

#include <algorithm>
#include <vector>
#include "KDirtyNpcSet.h"


#ifndef _SERVER
#include "ChatCenter_C.h"
#include "KSimulation.h"
#else
#include "ChatCenter_S.h"
#endif

#ifndef _SERVER
#include "ClientAuctionMgr.h"
#include "client_combat_info.h"
#include "ConfigManager.h"
#include "KLinkItem.h"
#endif

#ifdef _SERVER
#include "ServerSocialUnitMgr.h"
#include "player_monitor.h"
#include "IBCenter_S.h"
#endif

#include "talisman_manager.h"

#ifndef _SERVER
#include "ai_player_controller.h"
#include "screeneffect_man.h"
#include "IBCenter_C.h"
#endif

#include "ITaisuiWheel.h"
#include "AutoRobotMgr.h"

#include "EmplomentDataDef.h"



KProtocolProcess g_ProtocolProcess;
KProtocolProcess::KProtocolProcess()
{
	ZeroMemory(ProcessFunc, sizeof(ProcessFunc));
#ifndef _SERVER

	ProcessFunc[s2c_login] = NULL;
	ProcessFunc[s2c_rolelist] = NULL;
	ProcessFunc[s2c_rolenewdelresponse] = NULL;	
	ProcessFunc[s2c_syncend] = SyncEnd;
	ProcessFunc[s2c_synccurplayer] = SyncCurPlayer;
	ProcessFunc[s2c_synccurplayerskill] = s2cSyncAllSkill;
	ProcessFunc[s2c_synccurplayernormal] = SyncCurNormalData;
	ProcessFunc[s2c_syncworld] = SyncWorld;
	ProcessFunc[s2c_syncplayer] = SyncPlayer;
	ProcessFunc[s2c_syncplayermin] = SyncPlayerMin;
	ProcessFunc[s2c_syncnpc] = SyncNpc;
	ProcessFunc[s2c_syncnpcmin] = SyncNpcMin;
	ProcessFunc[s2c_syncnpcminplayer] = SyncNpcMinPlayer;
	ProcessFunc[s2c_objadd] = SyncObjectAdd;
	ProcessFunc[s2c_syncobjstate] = SyncObjectState;
	ProcessFunc[s2c_syncobjdir] = SyncObjectDir;
	ProcessFunc[s2c_objremove] = SyncObjectRemove;
	ProcessFunc[s2c_objTrapAct] = SyncObjectTrap;
	ProcessFunc[s2c_npcremove] = NetCommandRemoveNpc;
	ProcessFunc[s2c_npcwalk] = NetCommandWalk;
	ProcessFunc[s2c_npcrun] = NetCommandRun;
	ProcessFunc[s2c_npchurt] = NetCommandHurt;
	ProcessFunc[s2c_npcdeath] = NetCommandDeath;
	ProcessFunc[s2c_skillcast] = NetCommandSkill;
	ProcessFunc[s2c_teamselfinfo] = s2cUpdataSelfTeamInfo;
	ProcessFunc[s2c_teamleave] = s2cLeaveTeam;
	ProcessFunc[s2c_teaminviteadd] = s2cTeamInviteAdd;
	ProcessFunc[s2c_playerlevelup] = s2cLevelUp;	
	ProcessFunc[s2c_playerskillinfo] = s2cSyncSkillInfo;
	ProcessFunc[s2c_syncitem] = s2cSyncItem;
 	ProcessFunc[s2c_refreshitem] = s2cRefreshItem;
	ProcessFunc[s2c_removeitem] = s2cRemoveItem;
	ProcessFunc[s2c_syncmoney] = s2cSyncMoney;
	ProcessFunc[s2c_playermoveitem] = s2cMoveItem;
	ProcessFunc[s2c_scriptaction] = SyncScriptAction;
	ProcessFunc[s2c_tradechangestate] = s2cTradeChangeState;
	ProcessFunc[s2c_trademoneysync] = s2cTradeMoneySync;
	ProcessFunc[s2c_tradedecision] = s2cTradeDecision;
	ProcessFunc[s2c_ping] = s2cPing;
	ProcessFunc[s2c_opensalebox] = OpenSaleBox;
	ProcessFunc[s2c_openstorebox] = OpenStoreBox;
	ProcessFunc[s2c_playerrevive] = PlayerRevive;
	ProcessFunc[s2c_requestnpcfail] = RequestNpcFail;
	ProcessFunc[s2c_tradeapplystart] = s2cTradeRequest;
	ProcessFunc[s2c_viewequip] = s2cViewEquip;
	ProcessFunc[s2c_enchaseritemresult] = EnchaserItemResult;
	ProcessFunc[s2c_checkstoragepswok] = s2cCheckStoragePswOK;
	ProcessFunc[s2c_createstoragepswok] = s2cCreateStoragePswOK;
	ProcessFunc[s2c_modifystoragepswok] = s2cModifyStoragePswOK;
	ProcessFunc[s2c_sync_teammemberlife] = s2cSyncTeamMemberInfo;
	ProcessFunc[s2c_sync_creature] = s2cCreatureSync; // lixuewu 同步召唤兽状态
	ProcessFunc[s2c_byte_extend] = s2cByteExtend;
	ProcessFunc[s2c_pet] = s2cPetProtocol;
	ProcessFunc[s2c_findpathsync] = s2cFindPathSync;
	ProcessFunc[s2c_show_damage] = s2cShowDamage;
	ProcessFunc[s2c_npcrealposition] = s2cNpcRealPosition;
	ProcessFunc[s2c_chat_family] = s2cChatFamily;
	ProcessFunc[s2c_buff_family] = s2cBuffFamily;
	ProcessFunc[s2c_quest_family] = s2cQuestFamily;
	ProcessFunc[s2c_sync_skillseries] = s2cSyncSkillSeries;
	ProcessFunc[s2c_sync_npcattr] = s2cSyncNpcAttr;
	ProcessFunc[s2c_sync_playerattr] = s2cSyncPlayerAttr;
	ProcessFunc[s2c_sync_item_attr] = s2cSyncItemAttr;
	ProcessFunc[s2c_auction_family] = s2cAuctionSync;
	ProcessFunc[s2c_social_family] = s2cSocialRelationSync;
	ProcessFunc[s2c_social_relation] = s2cSocialRelationInfoSync;
	ProcessFunc[s2c_delayed_action] = s2cDelayedAction;
	ProcessFunc[s2c_talisman_family] = s2cTalismanFamily;
	ProcessFunc[s2c_sync_talisman_enchase] = s2cSyncTalismanEnchase;
	ProcessFunc[s2c_sync_npc_equip_talisman] = s2cSyncNpcEquipTalisman;
	ProcessFunc[s2c_team_invite_refuse] = s2cTeamInviteRefuse;
	ProcessFunc[s2c_show_predefined_msg] = s2cShowPredefinedMsg;
	ProcessFunc[s2c_IB_family] = s2cIBFamily;
	ProcessFunc[s2c_show_banner] = s2cShowBanner;
	ProcessFunc[s2c_find_family] = s2cFindFamily;
    ProcessFunc[s2c_taisui_wheel] = s2cTaisuiWheel;
	ProcessFunc[s2c_team_operation_result] = s2cTeamOperationResult;
	ProcessFunc[s2c_update_team_member_info] = s2cUpdateTeamMemberInfo;
	ProcessFunc[s2c_prompt] = s2cPrompt;
	ProcessFunc[s2c_cancel_prompt] = s2cCancelPrompt;
	ProcessFunc[s2c_player_stop] = s2cPlayerStop;
	ProcessFunc[s2c_pos_edition] = s2cPosEdition;
	ProcessFunc[s2c_fury_sync] = s2cFurySync;
	ProcessFunc[s2c_apply_join_team] = s2cApplyJoinTeam;
	ProcessFunc[s2c_npc_sync_to_world] = s2cNpcSyncToWorld;
	ProcessFunc[s2c_npc_sync_to_world_min] = s2cNpcSyncToWorldMin;
	ProcessFunc[s2c_npc_sync_to_world_del] = s2cNpcSyncToWorldDel;
	ProcessFunc[s2c_list_team] = s2cListTeam;
	ProcessFunc[s2c_special_quest_data] = s2cSpecialQuestData;
	ProcessFunc[s2c_show_banner_id] = s2cShowBannerById;
	ProcessFunc[s2c_gm_feedback_msg] = s2cGMFeedBack;
	ProcessFunc[s2c_hire_data_list_exp] = s2cHireDataListExp;
	ProcessFunc[s2c_hire_data_list_fighter] = s2cHireDataListFighter;
	ProcessFunc[s2c_hire_ret_code] = s2cHireRetCode;
	ProcessFunc[s2c_world_combat_info] = s2cWorldCombatInfo;
	ProcessFunc[s2c_npc_inlaycount] = s2cNpcInlayCount;
	ProcessFunc[s2c_npc_comoflag] = s2cCommoFlag;
	ProcessFunc[s2c_list_student] = s2cListStudent;
	ProcessFunc[s2c_world_combat_top10_info] = s2cWorldCombatTop10Info;	
	ProcessFunc[s2c_insurance] = s2cInsurance;
	ProcessFunc[s2c_world_player_info_sync] = s2cWorldPlayerSync;
	ProcessFunc[s2c_war_commander_sync] = s2cWarCommanderSync;
	ProcessFunc[s2c_world_custom_string] = s2cWorldCustomString;
	ProcessFunc[s2c_change_title] = s2cChangeTitle;
	ProcessFunc[s2c_update_self_title] = s2cUpdateSelfTitle;
	ProcessFunc[s2c_sync_self_title] = s2cSyncSelfTitle;
	ProcessFunc[s2c_select_title_result] = s2cSelectTitleResult;
	ProcessFunc[s2c_plus_point_top_n] = s2cPlusPointTopN;
	ProcessFunc[s2c_shizu_popularity_top_n] = s2cShizuPopularityTopN;
	ProcessFunc[s2c_zhuhou_popularity_top_n] = s2cZhuhouPopularityTopN;
	ProcessFunc[s2c_player_properties] = s2cPlayerProperties;
	ProcessFunc[s2c_synccurplayernormalex] = SyncCurNormalDataEx;
	ProcessFunc[s2c_palyerinfo_sync] = s2cPlayerRealInfoSync;

	ProcessFunc[s2c_extend] = s2cExtend;
	ProcessFunc[s2c_extendchat] = s2cExtendChat;
	ProcessFunc[s2c_extendfriend] = s2cExtendFriend;
#else
	ProcessFunc[c2s_login] = NULL;
	ProcessFunc[c2s_dbplayerselect] = NULL;
	ProcessFunc[c2s_syncend] = NULL;
	ProcessFunc[c2s_newplayer] = NULL;
	ProcessFunc[c2s_removeplayer] = NULL;
	ProcessFunc[c2s_requestnpc] = &KProtocolProcess::NpcRequestCommand;
	ProcessFunc[c2s_requestobj] = &KProtocolProcess::ObjRequestCommand;
	ProcessFunc[c2s_npcrun] = &KProtocolProcess::NpcRunCommand;
	ProcessFunc[c2s_npcskill] = &KProtocolProcess::NpcSkillCommand;
	ProcessFunc[c2s_teamapplyinfo] = &KProtocolProcess::PlayerApplyTeamInfo;
	ProcessFunc[c2s_playereatitem] = &KProtocolProcess::PlayerEatItem;
	ProcessFunc[c2s_playerpickupitem] = &KProtocolProcess::PlayerPickUpItem;
	ProcessFunc[c2s_playermoveitem] = &KProtocolProcess::PlayerMoveItem;
	ProcessFunc[c2s_playersellitem] = &KProtocolProcess::PlayerSellItem;
	ProcessFunc[c2s_playerbuyitem] = &KProtocolProcess::PlayerBuyItem;
	ProcessFunc[c2s_playerthrowawayitem] = &KProtocolProcess::PlayerDropItem;
	ProcessFunc[c2s_playerselui] = &KProtocolProcess::PlayerSelUI;
	ProcessFunc[c2s_tradeapplystart] = &KProtocolProcess::c2sTradeRequest;
	ProcessFunc[c2s_trademovemoney] = &KProtocolProcess::TradeMoveMoney;
	ProcessFunc[c2s_tradedecision] = &KProtocolProcess::TradeDecision;
	ProcessFunc[c2s_dialognpc]	= &KProtocolProcess::DialogNpc;
	ProcessFunc[c2s_teaminviteadd]	= &KProtocolProcess::TeamInviteAdd;
	ProcessFunc[c2s_teamreplyinvite] = &KProtocolProcess::TeamReplyInvite;
	ProcessFunc[c2s_ping] = NULL;//ReplyPing;
	ProcessFunc[c2s_objmouseclick] = &KProtocolProcess::ObjMouseClick;
	ProcessFunc[c2s_storemoney] = &KProtocolProcess::StoreMoneyCommand;
	ProcessFunc[c2s_playerrevive] = &KProtocolProcess::NpcReviveCommand;
	ProcessFunc[c2s_tradereplystart] = &KProtocolProcess::c2sTradeReplyStart;
	ProcessFunc[c2s_viewequip] = &KProtocolProcess::c2sViewEquip;
	ProcessFunc[c2s_repairitem] = &KProtocolProcess::ItemRepair;
	ProcessFunc[c2s_enchaseritem] = &KProtocolProcess::EnchaserItem;
	ProcessFunc[c2s_splitpileitem] = &KProtocolProcess::c2sSplitPileItem;
	ProcessFunc[c2s_checkstoragepassword] = &KProtocolProcess::c2sCheckStoragePassword;
	ProcessFunc[c2s_createstoragepassword] = &KProtocolProcess::c2sCreateStoragePassword;
	ProcessFunc[c2s_modifystoragepassword] = &KProtocolProcess::c2sModifyStoragePassword;
	ProcessFunc[c2s_closestorage] = &KProtocolProcess::c2sCloseStorage;
	ProcessFunc[c2s_byte_extend] = &KProtocolProcess::c2sByteExtend;
	ProcessFunc[c2s_pet] = &KProtocolProcess::c2sPetProtocol;
	ProcessFunc[c2s_chat_family] = &KProtocolProcess::c2sChatFamily;
	ProcessFunc[c2s_buff_family] = &KProtocolProcess::c2sBuffFamily;
	ProcessFunc[c2s_quest_family] = &KProtocolProcess::c2sQuestFamily;
	ProcessFunc[c2s_chg_pkmode]  = &KProtocolProcess::c2sChgPKMode;
	ProcessFunc[c2s_skill_sync]  = &KProtocolProcess::c2sSkillSync;
	ProcessFunc[c2s_auction_family] = &KProtocolProcess::c2sAuctionSync;
	ProcessFunc[c2s_playerstop] = &KProtocolProcess::c2sPlayerStopNotify;
	ProcessFunc[c2s_social_family] = &KProtocolProcess::c2sSocialRelationSync;
	ProcessFunc[c2s_talisman_family] = &KProtocolProcess::c2sTalismanFamily;
	ProcessFunc[c2s_player_logout] = &KProtocolProcess::c2sPlayerLogout;
	ProcessFunc[c2s_IB_family] = &KProtocolProcess::c2sIBFamily;
	ProcessFunc[c2s_find_family] = &KProtocolProcess::c2sFindFamily;
	ProcessFunc[c2s_taisui_wheel] = &KProtocolProcess::c2sTaisuiWheel;
	ProcessFunc[c2s_team_operation] = &KProtocolProcess::c2sTeamOperation;
	ProcessFunc[c2s_reply_prompt] = &KProtocolProcess::c2sReplyPrompt;
	ProcessFunc[c2s_player_pos_sync] = &KProtocolProcess::c2sPosSync;
	ProcessFunc[c2s_select_skill] = &KProtocolProcess::c2sSelectSkill;	
	ProcessFunc[c2s_fury_explode] = &KProtocolProcess::c2sFuryExplode;
	ProcessFunc[c2s_request_sync_to_world_npc] = &KProtocolProcess::c2sRequestSyncToWorldNpc;
	ProcessFunc[c2s_list_team] = &KProtocolProcess::c2sRequestTeamList;
	ProcessFunc[c2s_req_special_quest_data] = &KProtocolProcess::c2sReqSpecialQuestData;
	ProcessFunc[c2s_gm_communication] = &KProtocolProcess::c2sCMCommunication;
	ProcessFunc[c2s_hire_req_tobe_hired] = &KProtocolProcess::c2sReqTobeHired;
	ProcessFunc[c2s_hire_req_list] = &KProtocolProcess::c2sReqHireList;
	ProcessFunc[c2s_hire_req_hire] = &KProtocolProcess::c2sReqHire;
	ProcessFunc[c2s_interactive_script_input] = &KProtocolProcess::c2sInteractiveScriptInput;
	ProcessFunc[c2s_recommender] = &KProtocolProcess::c2sRecommender;
	ProcessFunc[c2s_insurance] = &KProtocolProcess::c2sInsurance;
	ProcessFunc[c2s_select_title] = &KProtocolProcess::c2sSelectTitle;
	ProcessFunc[c2s_plus_point_top_n] = &KProtocolProcess::c2sPlusPointTopN;
	ProcessFunc[c2s_player_real_info_sync] = &KProtocolProcess::c2sPlayerRealInfoSync;
#endif
}

KProtocolProcess::~KProtocolProcess()
{
}

#ifndef _SERVER
void KProtocolProcess::ProcessNetMsg(BYTE* pMsg)
{
	if (!pMsg || pMsg[0] <= s2c_clientbegin || pMsg[0] >= s2c_end || ProcessFunc[pMsg[0]] == NULL)
	{
		g_DebugLog("[error]Net Msg Error");
		return;
	}
	g_DebugLog("[net]Msg:%c", pMsg[0]);

	if (ProcessFunc[pMsg[0]])
	{
		(this->*ProcessFunc[pMsg[0]])(pMsg);
	}
}
#else
void KProtocolProcess::ProcessNetMsg(int nIndex, BYTE* pMsg, int nSize)
{
	_ASSERT(pMsg && pMsg[0] > c2s_gameserverbegin && pMsg[0] < c2s_end);

	BYTE	byProtocol = pMsg[0];
	_ASSERT(nIndex > 0 && nIndex < MAX_PLAYER);
	//根据玩家现在的状态,检查协议,如果不应该收到该协议(client出错???),丢掉它,
	//当处于Trading 状态时候,没有在此处处理
	//目前处于摆摊状态需要禁止的协议在此处理, 建议以后状态需要屏蔽一些协议的操作在这进行, 薛耀永
	if (!IsPermitedNow(nIndex, pMsg, nSize))
	{
		return;
	}

	//记录上行协议
	if (g_PlayerMonitor.IsNeedRecordC2SProtocol(nIndex))
	{
		g_PlayerMonitor.RecordC2SProtocol(nIndex, byProtocol, pMsg, nSize);
	}

	if (ProcessFunc[byProtocol])
	{
		(this->*ProcessFunc[byProtocol])(nIndex, pMsg, nSize);
	}
}
#endif

#ifndef _SERVER

void KProtocolProcess::s2cPing(BYTE* pMsg)
{
	DWORD	dwTimer = ::GetTickCount();
	PING_COMMAND*	PingCmd = (PING_COMMAND *)pMsg;
	g_SubWorldSet.SetPing( dwTimer - PingCmd->m_dwTime );

	CoreDataChanged( GDCNI_PING, g_SubWorldSet.GetPing(), NULL );
}

void KProtocolProcess::EnchaserItemResult(BYTE * pMsg)
{
	ENCHASER_SERVERRESULT* pResult = (ENCHASER_SERVERRESULT*)pMsg;
	if ( pResult )
	{
		if(pResult->compoundType == COMPOUND_SMITH)
		{
			CoreDataChanged( GDCNI_END_SMITH, NULL, pResult->nResult );
		}
		else
		{
			CoreDataChanged( GDCNI_OPEN_COMPOUND_WND,(unsigned int)2, pResult->nResult );
			
			if ( pResult->NewItemID )
			{
				int nIndex = GetClientPlayer().GetItemList().SearchID(pResult->NewItemID);
				ItemPos itemPos;
				GetClientPlayer().GetItemList().GetItemPos(nIndex,&itemPos);

				if (nIndex)
				{
					CoreDataChanged( GDCNI_COMPOUND_NEWITEM_NOTIFY,nIndex,(int)&itemPos );
				}//endif
				
			}//endif
			
		}
		
	}

}

void KProtocolProcess::NetCommandDeath(BYTE* pMsg)
{
	DWORD	dwNpcID;
	dwNpcID = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcID);

 	int nTargetNpcIdx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetNpc();
 	if ( nTargetNpcIdx == nIdx )
 	{
 		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetTarget( type_npc, 0 );
		PlayerController::Singleton().Stop();
		CoreDataChanged( GDCNI_SEL_TARGET, NULL, NULL );
		
		if(AutoRobotMgr::Singleton().GetMode() != enRobotMode_AutoRun && 
			(AutoRobotMgr::Singleton().GetAutoEnstrustMode()&ENSTRUST_AUTO_PICK_UP_MODE)
			)
		{
			AutoRobotMgr::Singleton().SetMode(enRobotMode_AutoPickUpItem);
			AutoRobotMgr::Singleton().SetAutoSellState(false);
			AutoRobotMgr::Singleton().StopGoToOtherMap( false );
			int itemX = 0,itemY = 0;
			Npc[nTargetNpcIdx].GetMpsPos(&itemX,&itemY);
			AutoRobotMgr::Singleton().SetPickUpTargetPosX(itemX);
			AutoRobotMgr::Singleton().SetPickUpTargetPosY(itemY);
			AutoRobotMgr::Singleton().AddAutoPickupItemFlags(AUTO_PICKUP_ITEM_DEATH);
		}
		if(AutoRobotMgr::Singleton().GetAutoEnstrustMode()&ENSTRUST_AUTO_ATTACK_MODE)//
		{
			//清楚不能攻击的NPC表//
			AutoRobotMgr::Singleton().canNotAttackNpcList.clear();
		}
 	}
	NPC_DEATH_SYNC * pSync = (NPC_DEATH_SYNC*)pMsg;
	if (nIdx > 0)
	{
		//Npc[nIdx].SendCommand(do_death);
		if ( Npc[nIdx].m_Kind == kind_player )
		{
			Npc[nIdx].ProcNetCommand(do_death);
		}

		if ( Npc[nIdx].m_Kind == kind_creature )
		{
			int nSummonerID = Npc[nIdx].GetSummonerIdx();
			int nSummonerIdx = NpcSet.SearchID( nSummonerID );
			if ( nSummonerIdx > 0 && nSummonerIdx < MAX_NPC )
			{
				Npc[nSummonerIdx].SetHeadInfoChanged( true );
			}
		}
		
		Npc[nIdx].m_UnaryAttrMgr.Set(nuai_curlife, 0);
		Npc[nIdx].SetCurrentLifePercentage( 0 );
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		//--> Rocker 2005/06/29
		Npc[nIdx].m_btKilledType = pSync->btKilledType;
		//<-- End
		if ( nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex )
		{
			CoreDataChanged( GDCNI_DEATH, NULL, NULL );
		}
		
		g_DebugLog("[Death]Net command comes");
	}
}

void KProtocolProcess::NetCommandHurt(BYTE* pMsg)
{
	NPC_HURT_SYNC*	pSync = (NPC_HURT_SYNC *)pMsg;
	
	int nIdx = NpcSet.SearchID(pSync->ID);
	if (nIdx > 0)
	{
		//Npc[nIdx].SendCommand(do_hurt, pSync->nFrames, pSync->nX, pSync->nY);
		Npc[nIdx].ProcNetCommand(do_hurt, pSync->nFrames, pSync->nX, pSync->nY);
		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::NetCommandRemoveNpc(BYTE* pMsg)
{
	DWORD	dwNpcID;
	dwNpcID = *(DWORD *)&pMsg[1];
	int nIdx = NpcSet.SearchID(dwNpcID);

	if (Player[CLIENT_PLAYER_INDEX].ConformIdx(nIdx))
	{
		if (Npc[nIdx].m_RegionIndex >= 0)
		{
			//lixuewu 2006.05.23 取消NPC阻挡
			//SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, nIdx);
			SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
			NpcSet.Remove(nIdx, true);
		}
	}
}

void KProtocolProcess::NetCommandRun(BYTE* pMsg)
{
	DWORD	dwNpcID;
	DWORD	MapX, MapY;

	NPC_RUN_SYNC* pNpcRun = (NPC_RUN_SYNC*)pMsg;
	dwNpcID = pNpcRun->ID;
	MapX = pNpcRun->nMpsX;
	MapY = pNpcRun->nMpsY;
	
	int nIdx = NpcSet.SearchID(dwNpcID);

	KPlayer& clientPlayer = GetClientPlayer();
	if(nIdx > 0)
	{
		if ((nIdx != clientPlayer.m_nIndex)
			|| clientPlayer.IsBlockClientControl())
		{
			KNpc& npc = Npc[nIdx];
			npc.SendCommand(do_run, MapX, MapY);
			npc.m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	 	}
	}
}

void KProtocolProcess::OpenSaleBox(BYTE* pMsg)
{
	SALE_BOX_SYNC* pSale = (SALE_BOX_SYNC *)pMsg;
	
	if(pSale->shopType == ST_Item)
	{
		BuySell.OpenSale(pSale->nShopIndex, pSale->shopType);
		Player[CLIENT_PLAYER_INDEX].m_CityTaxRate = pSale->cityTaxRate;
	}
	else if(pSale->shopType == ST_Smith)
	{
		KSmithShop::getSinglton().beginSmith(pSale->nShopIndex);
	}
	else if(pSale->shopType == ST_Hire)
	{
		BuySell.OpenHireShop();
	}
}

void KProtocolProcess::OpenStoreBox(BYTE* pMsg)
{
	POPENSTOREBOX pOpenStorage = (POPENSTOREBOX)pMsg;
	//注意：这里不再仅仅是仓库问题，当 pOpenStorage->byNeedPassword = 2 时，唯一需要做的是往服务器发送密码
	//如果 pOpenStorage->byNeedPassword = 0 或 1 则打开仓库界面
	CoreDataChanged(GDCNI_OPEN_STORE_BOX, pOpenStorage->byNeedPassword, NULL);
}

void KProtocolProcess::PlayerRevive(BYTE* pMsg)
{
	NPC_REVIVE_SYNC* pSync = (NPC_REVIVE_SYNC*)pMsg;

	int nIdx = NpcSet.SearchID(pSync->ID);
	if (nIdx > 0)
	{
		if (!Npc[nIdx].IsPlayer() && pSync->Type == REMOTE_REVIVE_TYPE)
		{
#ifndef _SERVER
			if (Npc[nIdx].m_RegionIndex >= 0)
			{
				int nSubWorld = Npc[nIdx].m_SubWorldIndex;
				int nRegion = Npc[nIdx].m_RegionIndex;
				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(nIdx);
			}
			NpcSet.Remove(nIdx, false);
#else
			{
				int nSubWorld = Npc[nIdx].m_SubWorldIndex;
				int nRegion = Npc[nIdx].m_RegionIndex;
				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(nIdx);
				NpcSet.Remove(nIdx);
			}
#endif
			return;
		}
		else
		{
			Npc[nIdx].ProcNetCommand(do_revive);
			if ( nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex )
			{
				CoreDataChanged( GDCNI_REVIVE, NULL, NULL );
			}
		}
	}
}

void KProtocolProcess::RequestNpcFail(BYTE* pMsg)
{
	NPC_REQUEST_FAIL* pNpcSync = (NPC_REQUEST_FAIL *)pMsg;

	if (NpcSet.IsNpcRequestExist(pNpcSync->ID))
		NpcSet.RemoveNpcRequest(pNpcSync->ID);	
}

void KProtocolProcess::NetCommandSkill(BYTE* pMsg)
{
	DWORD	dwNpcID;
	int		nSkillID, nSkillLevel;
	int		MapX, MapY;

	NPC_SKILL_SYNC* pNpcSkill = (NPC_SKILL_SYNC*)pMsg;
	dwNpcID = pNpcSkill->ID;
	nSkillID = pNpcSkill->nSkillID;
	nSkillLevel = pNpcSkill->nSkillLevel;
	MapX = pNpcSkill->nMpsX;
	MapY = pNpcSkill->nMpsY;
	
	if (MapY < 0)
		return ;

	//当指定某个目标时(MapX == -1),MapY为目标的NpcdwID，需要转换成本地的NpcIndex才行
	if (MapX < 0)
	{
		if ((MapX != SKILL_SPT_TargetIndex))			
			return;
	
		if (MapX == SKILL_SPT_TargetIndex)
		{
			MapY = NpcSet.SearchID(MapY);
			if (MapY == 0)
				return;

			if (Npc[MapY].m_RegionIndex < 0)
				return;
		}
	}
	
	int nIdx = NpcSet.SearchID(dwNpcID);
	
	KPlayer& clientPlayer = GetClientPlayer();
	if(nIdx > 0 /*&& nIdx != clientPlayer.m_nIndex*/)
	{
//		if ( clientPlayer.IsBlockClientControl() )
		{
			KNpc& npc = Npc[nIdx];

			if( -1 == Npc[nIdx].m_SkillList.FindSkill(nSkillID) )
			{
				Npc[nIdx].m_SkillList.AddSkillEx(nSkillID, nSkillLevel, skill_status_usable);
			}
			
			KSkill* pSkill = g_SkillManager.GetSkill(nSkillID, nSkillLevel);
			if (pSkill != NULL)
			{
				if (pSkill->IsTalismanSkill())
				{				
					Npc[nIdx].GetTalismanNpcController().UseTalismanSkill(nSkillID, nSkillLevel);
				}
			}
			
			npc.SendCommand(do_skill, nSkillID, MapX, MapY);
			npc.m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		}
	}
}

void KProtocolProcess::NetCommandWalk(BYTE* pMsg)
{
	DWORD	dwNpcID;
	DWORD	MapX, MapY;
	NPC_WALK_SYNC* pNpcWalk = (NPC_WALK_SYNC*)pMsg;
	dwNpcID = pNpcWalk->ID;
	MapX = pNpcWalk->nMpsX;
	MapY = pNpcWalk->nMpsY;
	int nIdx = NpcSet.SearchID(dwNpcID);

	KPlayer& clientPlayer = GetClientPlayer();
	if(nIdx > 0)
	{
		if ((nIdx != clientPlayer.m_nIndex)
			|| clientPlayer.IsBlockClientControl())
		{
			KNpc& npc = Npc[nIdx];
			npc.SendCommand(do_walk, MapX, MapY);
			npc.m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		}
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知队伍创建失败
//-------------------------------------------------------------------------
void KProtocolProcess::s2cApplyCreateTeamFalse(BYTE* pMsg)
{
	PLAYER_SEND_CREATE_TEAM_FALSE *pCreateFalse = (PLAYER_SEND_CREATE_TEAM_FALSE*)pMsg;
	KSystemMessage	sMsg;

	switch (pCreateFalse->m_btErrorID)
	{
	// 已经在队伍中，说明客户端队伍数据有错误，申请重新获得队伍数据
	case Team_Create_Error_InTeam:
		GetClientPlayer().GetTeamInfo().RequestSelfTeamInfo();
		break;

	// 当前处于不能组队状态
	case Team_Create_Error_CannotCreate:
		sprintf(sMsg.szMessage, MSG_TEAM_CANNOT_CREATE);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
//		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		break;


	default:
		sprintf(sMsg.szMessage, MSG_TEAM_CREATE_FAIL);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
//		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		break;
	}
}

void KProtocolProcess::s2cSyncSkillInfo(BYTE* pMsg)
{
	PPLAYER_SKILLINFO_SYNC	pSkillInfo = (PPLAYER_SKILLINFO_SYNC)pMsg;
	
	int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int nMainSkillId = g_SkillManager.GetMainSkillId(pSkillInfo->SkillId);

	switch(pSkillInfo->Operation)
	{
	case skill_ope_levelup:
		// 如果这里的 Level 是目标等级，而不是增加的等级，就可以
		// 不区分技能是否是主技能，都调用LevelUp
		// 但如果Level 是增加的等级，则需要区分，因为子技能是一个
		// 新的Id，对主技能调用LevelUp, 子技能调用LearnSkill
		Npc[nNpcIdx].m_SkillList.LevelUpTo(pSkillInfo->SkillId, pSkillInfo->PlusInfo.comInfo.Level);
		break;

	case skill_ope_chgstatus:
		{
			int nIdx = Npc[nNpcIdx].m_SkillList.FindSkill(pSkillInfo->SkillId);
			Npc[nNpcIdx].m_SkillList.ChangeStatus(nIdx, pSkillInfo->PlusInfo.comInfo.Status);
			
			//CoreDataChanged(GDCNI_UPDATA_SHORTCUT, NULL, NULL);
		}
		break;

	case skill_ope_chgcdtime:
		{
			int nIdx = Npc[nNpcIdx].m_SkillList.FindSkill(pSkillInfo->SkillId);
			Npc[nNpcIdx].m_SkillList.ChangeCollDownTime(nIdx, pSkillInfo->PlusInfo.comInfo.CoolDownTime);
		}
		break;

	case skill_ope_cooldown:
		{
			Npc[nNpcIdx].m_SkillList.CoolDown(pSkillInfo->SkillId);
		}
		break;

	case skill_ope_clearcooldown:
		{
			Npc[nNpcIdx].m_SkillList.ClearCoolDown(pSkillInfo->SkillId);
		}
		break;

	case skill_ope_addskill:
		{
			Npc[nNpcIdx].m_SkillList.AddSkillEx(pSkillInfo->SkillId, 
											  pSkillInfo->PlusInfo.comInfo.Level, 
											  pSkillInfo->PlusInfo.comInfo.Status
											  );
		}
		break;

	case skill_ope_removeskill:
		{
			Npc[nNpcIdx].m_SkillList.RemoveSkillEx(pSkillInfo->SkillId);
		}
		break;

	case skill_ope_chgcost:
		{
			Npc[nNpcIdx].m_SkillList.ChangeCost(pSkillInfo->SkillId, pSkillInfo->PlusInfo.nCost);
		}
		break;

	case skill_ope_chgcastspeed:
		{
			Npc[nNpcIdx].m_SkillList.ChgCastSpeedEnhance(pSkillInfo->SkillId, 
				pSkillInfo->PlusInfo.nCastSpeed);
		}
		break;
	}

	// 通知界面刷新技能信息
	CoreDataChanged(GDCNI_SKILLLIST_CHANGE, nMainSkillId, pSkillInfo->SkillId);
}

void KProtocolProcess::s2cSyncSkillSeries(BYTE* pMsg)
{
	PLAYER_SKILLSERIES_SYNC	*pSeries = (PLAYER_SKILLSERIES_SYNC*)pMsg;

	if(pSeries->m_Series > role_skillseries_invalid && pSeries->m_Series < role_skillseries_count)
		Player[CLIENT_PLAYER_INDEX].m_SkillSeries = (RoleSkillSeries)pSeries->m_Series;

}

void KProtocolProcess::s2cSyncNpcAttr(BYTE *pMsg)
{
	PSYNC_NPCATTR	pSync = (PSYNC_NPCATTR)pMsg;
	int			    nNpcIdx = NpcSet.SearchID(pSync->npcId);

	if(nNpcIdx <= 0)
		return;

	Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	Npc[nNpcIdx].RecvAttrSync(pSync->subProtocol, pSync->attrIdx, pSync->valueMask, (int*)pSync->data);
}

void KProtocolProcess::s2cSyncPlayerAttr(BYTE *pMsg)
{
	PSYNC_PLAYERATTR pSync = (PSYNC_PLAYERATTR)pMsg;
	Player[CLIENT_PLAYER_INDEX].RecvAttributeSync(pSync->Attribute, pSync->Value);	
}

void KProtocolProcess::s2cSyncItemAttr(BYTE * pMsg)
{
	PSYNC_ITEM_ATTR pSync = (PSYNC_ITEM_ATTR)pMsg;
	KItem* pItem = ItemSet.GetItem(pSync->ItemID);
	if (pItem != NULL)
	{
		bool needUpdateEquipment = false;//是否需要更新装备效果
		bool needUpdateAbradeMonitor = false;//是否需要更新耐久监视器
		if (pSync->Attribute == item_attr_durability)
		{
			int originalDur = pItem->GetDurability();
			int currentDur = pSync->Value;
			if ((originalDur == 0 && currentDur > 0) || (originalDur > 0 && currentDur == 0))
			{
				needUpdateEquipment = true;					
			}

			needUpdateAbradeMonitor = true;
		}
		
		pItem->RecvAttributeSync(pSync->Attribute, pSync->Value);
		
		if (needUpdateEquipment)
		{
			Player[CLIENT_PLAYER_INDEX].GetItemList().OnEquipChanged();
		}
		if (needUpdateAbradeMonitor)
		{
			KItemList& itemlist = Player[CLIENT_PLAYER_INDEX].GetItemList();
			itemlist.GetAbradeMonitor().Update(itemlist.m_EquipItem);
		}
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知有成员离开(包括自己离开)
//-------------------------------------------------------------------------
void KProtocolProcess::s2cLeaveTeam(BYTE* pMsg)
{
	KPlayerTeam& teamInfo = GetClientPlayer().GetTeamInfo();

	if (!teamInfo.IsInTeam())
	{
		teamInfo.UpdateInterface();
		return;
	}

	PLAYER_LEAVE_TEAM *pLeaveTeam = (PLAYER_LEAVE_TEAM*)pMsg;
	int leaveMemberId = pLeaveTeam->m_dwNpcID;
	KTeam& clientTeam = GetClientTeam();

	char leaveMemberName[32] = { 0 };

	// 自己离开
	if (leaveMemberId == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
	{
		g_GetStringRes(sid_you, leaveMemberName, sizeof(leaveMemberName));//你

		teamInfo.SetInTeam(false);
		teamInfo.m_nApplyCaptainID = 0;
		clientTeam.Release();		
		clientTeam.DeleteMember(leaveMemberId);
	}
	// 别人离开
	else
	{
		int memberIndex = clientTeam.FindMemberID(leaveMemberId);
		if (memberIndex >= 0)
		{
			ClientTeamMemberInfo* pMemberInfo = clientTeam.GetMemberInfo(memberIndex);
			if (pMemberInfo != NULL)
			{
				sprintf(leaveMemberName, pMemberInfo->Name);
			}			
		}

		clientTeam.DeleteMember(leaveMemberId);
	}

	char msgBuff[COMMON_CLIENT_MSG_LEN_256] = { 0 };
	char templateBuff[COMMON_CLIENT_MSG_LEN_256] = { 0 };
	g_GetStringRes(sid_leave_team, templateBuff, sizeof(templateBuff));//%s离开了队伍
	sprintf(msgBuff, templateBuff, leaveMemberName);
	msgBuff[sizeof(msgBuff) - 1] = 0;
	ShowSystemMessage(msgBuff);

	teamInfo.UpdateInterface();
	return;
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知升级
//-------------------------------------------------------------------------
void KProtocolProcess::s2cLevelUp(BYTE* pMsg)
{
	PLAYER_LEVEL_UP_SYNC* pLevelUpSync = (PLAYER_LEVEL_UP_SYNC*)pMsg;
	
	if (Npc[GetClientPlayer().GetNpcIndex()].GetId() == pLevelUpSync->NpcID)//自己升级
	{
		GetClientPlayer().s2cLevelUp(pMsg);

		CoreDataChanged(GDI_EXP_QUEST_INSURANCE_LEVEL_UP_NOTIFY,0,0);
	}
	else//别人升级
	{
		int index = NpcSet.SearchID(pLevelUpSync->NpcID);		
		if(index > 0)
		{
			Npc[index].SetLevel(pLevelUpSync->ArriveLevel);
		}
	}
}

void KProtocolProcess::s2cMoveItem(BYTE* pMsg)
{
	PLAYER_MOVE_ITEM_SYNC	*pMove = (PLAYER_MOVE_ITEM_SYNC*)pMsg;

	ItemPos		sourPos, destPos;
	sourPos.nPlace = pMove->sourPlace;
	sourPos.nX = pMove->sourX;
	sourPos.nY = pMove->sourY;

	destPos.nPlace = pMove->destPlace;
	destPos.nX = pMove->destX;
	destPos.nY = pMove->destY;
	
	Player[CLIENT_PLAYER_INDEX].m_ItemList.exchangeItem(&sourPos, &destPos);
	
	CoreDataChanged(GDCNI_UPDATA_SHORTCUT, 0, 0);
}

void KProtocolProcess::s2cRemoveItem(BYTE* pMsg)
{
	ITEM_REMOVE_SYNC	*pRemove = (ITEM_REMOVE_SYNC*)pMsg;

	int		nIdx;
	nIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(pRemove->m_ID);
	if (nIdx > 0)
	{
		Player[CLIENT_PLAYER_INDEX].m_ItemList.Remove(nIdx);
		CoreDataChanged(GDCNI_UPDATA_SHORTCUT, NULL, NULL );
	}
}

void KProtocolProcess::s2cRefreshItem(BYTE *pMsg)
{
	char receiveBuff[ITEM_SYNC_BUFF_LENGTH];
	if (DecompressProtocol(pMsg, (BYTE*)receiveBuff, sizeof(receiveBuff)))
	{
		ITEM_REFRESH* pItemSync = (ITEM_REFRESH*)receiveBuff;

		int nIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(pItemSync->m_nId);
		if ( nIdx > 0 && nIdx < MAX_ITEM )
		{
			
			int originalItemWeight = Item[nIdx].GetItemWeight();
			Item[nIdx].SetItemCount( pItemSync->m_btItemCount );
			
			if ( pItemSync->m_nAddMagicBuff > 0)
			{
				Item[nIdx].AddCompoundBuff( COMPOUND_ADDMAGIC, -1, pItemSync->m_nAddMagicBuff );
			}
			
			
			int nTakeIndex = Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSame(nIdx);
			if(nTakeIndex <= 0)
			{
				return;
			}
			
			int nPlace = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nTakeIndex].nPlace;
			switch(nPlace)
			{
			case pos_equiproom:
				Player[CLIENT_PLAYER_INDEX].m_ItemList.
					m_Room[room_equipment].AddWeight(Item[nIdx].GetItemWeight() - originalItemWeight);
				break;
			default:
				break;
			}
			
			Item[nIdx].ClearSocketSet();
			for ( int nSocketIdx = 0; nSocketIdx < MAX_INLAY_COUNT; ++nSocketIdx )
			{
				InlayStuff stuff;
				if ( pItemSync->m_socketSet[nSocketIdx].nGenre == -1 &&
					pItemSync->m_socketSet[nSocketIdx].nDetail == -1 &&
					pItemSync->m_socketSet[nSocketIdx].nParticular == -1 &&
					pItemSync->m_socketSet[nSocketIdx].nLevel == -1 )
				{
					continue;
				}
				else
				{
					
					stuff.nGenre		= pItemSync->m_socketSet[nSocketIdx].nGenre;
					stuff.nDetail		= pItemSync->m_socketSet[nSocketIdx].nDetail;
					stuff.nParticular	= pItemSync->m_socketSet[nSocketIdx].nParticular;
					stuff.nLevel		= pItemSync->m_socketSet[nSocketIdx].nLevel;
					if ( stuff.nGenre == 0 && 
						stuff.nDetail == 0 &&
						stuff.nParticular == 0 && 
						stuff.nLevel == 0 )
					{
						Item[nIdx].CreateSocket();
					}
					else
					{
						Item[nIdx].SetSocketSet( stuff );
					}
					
				}
			}
			Item[nIdx].SetInlayBaseBuffSet((short *)pItemSync->m_InlayBaseBuffSet );
			Item[nIdx].SetInlayYaoBuffSet(pItemSync->m_InlayYaoBuffSet );
			Item[nIdx].SetInlaySpecialBuffSet(pItemSync->m_InlaySpecialBuffSet );
			
			Player[CLIENT_PLAYER_INDEX].m_ItemList.OnBagChanged();
			
			//发送消息给界面
			KObjAtContRegion pInfo;
			
			pInfo.Obj.uGenre = CGOG_ITEM;
			pInfo.Obj.uId = nIdx;
			pInfo.Region.h = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nTakeIndex].nX;
			pInfo.Region.v = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nTakeIndex].nY;
			pInfo.Region.Height = pItemSync->m_btItemCount;
			pInfo.eContainer = KItemList::corePos2ClientContainer((enum ITEM_POSITION)nPlace);
			CoreDataChanged(GDCNI_OBJECT_CHANGED, (unsigned int)&pInfo, true);
			CoreDataChanged(GDCNI_UPDATA_SHORTCUT, NULL, NULL );
		}
	}
}

void KProtocolProcess::s2cSyncItem(BYTE* pMsg)
{
	if(NULL == pMsg)
		return;

	char receiveBuff[ITEM_SYNC_BUFF_LENGTH];		
	if (DecompressProtocol(pMsg, (BYTE*)receiveBuff, sizeof(receiveBuff)))
	{
		ITEM_SYNC* pItemSync = (ITEM_SYNC*)receiveBuff;

		//现在使用pos_pet_feed_box来表示聊天物品链接同步
		if(pos_pet_feed_box == pItemSync->m_btPlace)
		{
			g_cLinkItem.addItem(pItemSync);
		}
		else
		{
			SyncItem(pItemSync);
			CoreDataChanged(GDCNI_UPDATA_SHORTCUT, NULL, NULL );
		}
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器发过来的同步money的消息
//-------------------------------------------------------------------------
void KProtocolProcess::s2cSyncMoney(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].s2cSyncMoney(pMsg);
}

void KProtocolProcess::s2cTeamInviteRefuse(BYTE* pMsg)
{
	PTEAM_INVITE_REFUSE pInviteRefuse = (PTEAM_INVITE_REFUSE)pMsg;
		
	char msgBuff[128] = { 0 };
	snprintf(msgBuff, sizeof(msgBuff), REFUSE_ADD_TEAM,
		"color=255,255,0",
		pInviteRefuse->PlayerName
		);
	msgBuff[sizeof(msgBuff) - 1] = 0;
	CoreDataChanged(GDCNI_APPEND_MESSAGE, (unsigned int)msgBuff, SYSTEM_ROOM_ID);
}

void KProtocolProcess::s2cShowPredefinedMsg(BYTE* pMsg)
{
	PSHOW_PREDEFINED_MSG pShowMsg = (PSHOW_PREDEFINED_MSG)pMsg;
	
	char msgBuff[256] = { 0 };
	g_GetStringRes(pShowMsg->MessageId, msgBuff, 256);
	if (msgBuff[0] != 0)
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)msgBuff, 0);
}

//-------------------------------------------------------------------------
//	功能：收到服务器通知更换队长
//-------------------------------------------------------------------------
void KProtocolProcess::s2cTeamChangeCaptain(BYTE* pMsg)
{
	KPlayer& clientPlayer = GetClientPlayer();

	if (!clientPlayer.GetTeamInfo().IsInTeam())
	{
		clientPlayer.GetTeamInfo().RequestSelfTeamInfo();
		return;
	}

	PLAYER_TEAM_CHANGE_CAPTAIN	*pChange = (PLAYER_TEAM_CHANGE_CAPTAIN*)pMsg;
	int newCaptainNpcId = pChange->m_dwCaptainID;
	KTeam& clientTeam = GetClientTeam();
	int newCaptainMemberIndex = clientTeam.FindMemberID(newCaptainNpcId);	
	char newCaptainName[32] = { 0 };

	//自己被任命为队长
	if (Npc[GetClientPlayer().GetNpcIndex()].GetId() == newCaptainNpcId)
	{	
		sprintf(newCaptainName, SELF_YOUR);
	}
	else
	{
		//通知界面别人被命名为新的队长
		int newCaptainMemberIndex = clientTeam.FindMemberID(newCaptainNpcId);
		if (newCaptainMemberIndex >= 0)
		{
			ClientTeamMemberInfo* pMemberInfo = clientTeam.GetMemberInfo(newCaptainMemberIndex);
			if (pMemberInfo != NULL)
			{
				sprintf(newCaptainName, pMemberInfo->Name);
			}
		}		
	}

	char msgBuff[128] = { 0 };
	sprintf(msgBuff, NEW_TEAMLEADER,
		"color=255,255,0",
		newCaptainName);
	msgBuff[sizeof(msgBuff) - 1] = 0;
	CoreDataChanged(GDCNI_APPEND_MESSAGE, (unsigned int)msgBuff, SYSTEM_ROOM_ID);
}

//-------------------------------------------------------------------------
//	功能：收到服务器发来的自己队伍的组队情况，更新相应信息
//-------------------------------------------------------------------------
void KProtocolProcess::s2cUpdataSelfTeamInfo(BYTE* pMsg)
{
	GetClientPlayer().GetTeamInfo().UpdateSelfTeam((TEAM_INFO*)pMsg);
}

void	KProtocolProcess::s2cNpcInlayCount(BYTE * pMsg)
{
	NPC_INLAYCOUNT_SYNC* pInlayCount = (NPC_INLAYCOUNT_SYNC*)pMsg;
	if ( pInlayCount )
	{
		int nIdx = NpcSet.SearchID(pInlayCount->ID);
		if ( IsValidNpc( nIdx ) )
		{
			Npc[nIdx].m_nItemInlayCount = pInlayCount->nInlayCount;
		}
	}
}

void KProtocolProcess::s2cCommoFlag(BYTE * pMsg)
{
	NPC_COMMO_FLAG * pInfo = (NPC_COMMO_FLAG *) pMsg;
	
	if ( pInfo)
	{
		int nIndex = NpcSet.SearchID(pInfo->nID);
		
		if ( IsValidNpc(nIndex))
		{
			if (Npc[nIndex].m_Kind == kind_player)
			{
				Npc[nIndex].m_UnaryAttrMgr.Set(nuai_camou_flage,pInfo->Comoflag);
				
				int nClientNpcIndex = GetClientPlayer().GetNpcIndex();
				if (nIndex != nClientNpcIndex)
				{
					strncpy(Npc[nIndex].Name,pInfo->szName,sizeof(Npc[nIndex].Name));
				}//endif
				
				Npc[nIndex].SetHeadInfoChanged(true);

				//Search for the Summor
				int nSumNpcIdx = 0;
				bool bFound = false;
				while (nSumNpcIdx = NpcSet.GetNextIdx(nSumNpcIdx))
				{
					if (Npc[nSumNpcIdx].GetSummonerIdx() == pInfo->nID)
					{
						bFound = true;
						break;
					}//endif

				}//end for while
				
				if (bFound)
				{
					strncpy(Npc[nSumNpcIdx].Name,pInfo->szName,sizeof(Npc[nSumNpcIdx].Name));

					int nTarget = GetClientPlayer().GetTargetNpc();
					if (nTarget == nSumNpcIdx)
					{
						CoreDataChanged( CDCNI_UPDATA_SEL_TARGET, NULL, NULL );
					}//endif
				}//endif

			}//endif
			
		}//endif

	}//endif

}

void KProtocolProcess::SyncCurNormalData(BYTE* pMsg)
{
	CURPLAYER_NORMAL_SYNC	*pSync = (CURPLAYER_NORMAL_SYNC*)pMsg;

	if (pSync->Life > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr.Set(nuai_curlife, pSync->Life);
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr.Set(nuai_curlife, 0);

	if (pSync->Mana > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr.Set(nuai_curmana, pSync->Mana);
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr.Set(nuai_curmana, 0);
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
	{
		if (   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr[nuai_curlife] 
			>= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CompAttrMgr[ncai_lifeuplimit]
			&& Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr[nuai_curmana]
			>= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CompAttrMgr[ncai_manauplimit])
		{
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
		}
	}

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	//避免宠物因为时间到了而被移除
	if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPetIndex > 0 && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPetIndex < MAX_NPC )
	{
		Npc[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPetIndex].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::SyncCurNormalDataEx(BYTE* pMsg)
{
	CURPLAYER_NORMAL_SYNC_EX	*pSync = (CURPLAYER_NORMAL_SYNC_EX*)pMsg; 
	
	if (pSync->Life > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr.Set(nuai_curlife, pSync->Life);
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr.Set(nuai_curlife, 0);
	
	if (pSync->Mana > 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr.Set(nuai_curmana, pSync->Mana);
	else
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr.Set(nuai_curmana, 0);

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
	{
		if (   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr[nuai_curlife] 
			>= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CompAttrMgr[ncai_lifeuplimit]
			&& Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr[nuai_curmana]
			>= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CompAttrMgr[ncai_manauplimit])
		{
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
		}
	}
	
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	//避免宠物因为时间到了而被移除
	if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPetIndex > 0 && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPetIndex < MAX_NPC )
	{
		Npc[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPetIndex].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void KProtocolProcess::SyncCurPlayer(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].SyncCurPlayer(pMsg);
	PlayerController::Singleton().Init();
}

/*
带宽优化前
void KProtocolProcess::SyncNpc(BYTE* pMsg)
{
	NPC_SYNC* NpcSync = (NPC_SYNC *)pMsg;

	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[0].Mps2Map(NpcSync->MapX, NpcSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	if (nRegion == -1)
		return;

	int nIdx = NpcSet.SearchID(NpcSync->ID);

	if (NpcSync->m_btKind == kind_player && NpcSync->m_szName[0]!=0)
	{
       bool bIsEnymy=g_ChatCenterC.IsObjectInGroup(NpcSync->m_szName,GROUPID_ENEMY);
	   char szName[256];
	   szName[0]=0;

	   sprintf(szName,ENYMY_IS_COMMING,NpcSync->m_szName);

       if  (bIsEnymy)
	   {
            CoreDataChanged( GDCNI_ERROR_MESSAGE, (unsigned int)szName, 0);     
	   }//endif

	}//endif

	if (!nIdx)
	{
		nIdx = NpcSet.Add(NpcSync->NpcSettingIdx, 0, NpcSync->MapX, NpcSync->MapY);

		Npc[nIdx].m_dwID = NpcSync->ID;
		Npc[nIdx].m_Kind = NpcSync->m_btKind;
		Npc[nIdx].m_Height = 0;			
		
		if (NpcSet.IsNpcRequestExist(NpcSync->ID))
		{
			NpcSet.RemoveNpcRequest(NpcSync->ID);
		}

// 		if (kind_building == NpcSync->m_btKind)
// 		{
// 			g_DirtyNpcSet.PushItem(nIdx);
// 		}
	}
	else
	{
		if ( Npc[nIdx].m_Doing == do_death && 
			 Npc[nIdx].m_Kind != kind_player )
		{
			return;
		}
		Npc[nIdx].m_NpcSettingIdx = (short)HIWORD(NpcSync->NpcSettingIdx);
		Npc[nIdx].m_Level = LOWORD(NpcSync->NpcSettingIdx);
		Npc[nIdx].MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);
	}

	if (!Npc[nIdx].IsPlayer())
	{
		Npc[nIdx].m_UnaryAttrMgr.Set(nuai_dir, NpcSync->byNpcDir);
	}
	Npc[nIdx].m_Camp			= (NPCCAMP)NpcSync->Camp;
	Npc[nIdx].m_CurrentCamp		= (NPCCAMP)NpcSync->CurrentCamp;
	Npc[nIdx].m_Series			= NpcSync->m_bySeries;
	Npc[nIdx].m_WorldCombatOrg  = NpcSync->nCombatOrg;
	Npc[nIdx].m_UnaryAttrMgr.Set(nuai_camou_flage ,NpcSync->bCamouflage);

	if(Npc[nIdx].m_UnaryAttrMgr[nuai_titlecolor] != NpcSync->dwNameColor)
	{
		Npc[nIdx].m_UnaryAttrMgr.Set(nuai_titlecolor, NpcSync->dwNameColor);
		Npc[nIdx].SetHeadInfoChanged(true);
	}

	Npc[nIdx].m_UnaryAttrMgr.Set(nuai_fightstate, NpcSync->bFightState);
	Npc[nIdx].m_CompAttrMgr.Set(ncai_runspeed, idx_base_value, NpcSync->RunSpeed[0]);
	Npc[nIdx].m_CompAttrMgr.Set(ncai_runspeed, idx_append_value, NpcSync->RunSpeed[1]);
	Npc[nIdx].m_CompAttrMgr.Set(ncai_runspeed, idx_append_percent, NpcSync->RunSpeed[2]);

	Npc[nIdx].m_CompAttrMgr.Set(ncai_lifeuplimit, idx_current_value, NpcSync->dwLifeUpLimit);
	Npc[nIdx].m_CompAttrMgr.Set(ncai_manauplimit, idx_current_value, NpcSync->dwManaUpLimti);

	Npc[nIdx].m_UnaryAttrMgr.Set(nuai_servercont, NpcSync->nCondition );
	Npc[nIdx].m_nHeadImage = NpcSync->HeadImage;

	if (kind_player == Npc[nIdx].m_Kind)
	{
		int originalTeamId = Npc[nIdx].m_UnaryAttrMgr[nuai_team_id];
		Npc[nIdx].m_UnaryAttrMgr.Set(nuai_team_id, NpcSync->TeamId);
		int newTeamId = Npc[nIdx].m_UnaryAttrMgr[nuai_team_id];
		g_TeamViewer.TeamChanged(nIdx, originalTeamId, newTeamId);
	}

	if (NpcSync->LifePerCent <= 128)
	{
		int nLife = (Npc[nIdx].m_CompAttrMgr[ncai_lifeuplimit] * NpcSync->LifePerCent) >> 7 ;
		Npc[nIdx].m_UnaryAttrMgr.Set(nuai_curlife, nLife);
		Npc[nIdx].SetCurrentLifePercentage( nLife * 100 / NpcSync->dwLifeUpLimit);
	}
	else
		Npc[nIdx].m_UnaryAttrMgr.Set(nuai_curlife, 0);

	if (NpcSync->ManaPercent <= 128)
	{
		int nMana = (Npc[nIdx].m_CompAttrMgr[ncai_manauplimit] * NpcSync->ManaPercent) >> 7 ;
		Npc[nIdx].m_UnaryAttrMgr.Set(nuai_curmana, nMana);
		Npc[nIdx].SetCurrentManaPercentage(nMana * 100 / NpcSync->dwManaUpLimti );
	}
	else
		Npc[nIdx].m_UnaryAttrMgr.Set(nuai_curmana, 0);
	
//	if (Npc[nIdx].m_Doing != do_death || Npc[nIdx].m_Doing != do_revive)
	if(do_revive == NpcSync->m_Doing && kind_player == Npc[nIdx].m_Kind)
	{
		// 如果是do_revive状态，客户端必须保持这个状态，否则
		// 其他人对这个人的复活技能将会无效
		Npc[nIdx].m_Doing = (NPCCMD)NpcSync->m_Doing;
	}
	else
	{
		if (Npc[nIdx].m_Kind != kind_building)
		{
			Npc[nIdx].SendCommand((NPCCMD)NpcSync->m_Doing, 
				NpcSync->dwDoingParamX, NpcSync->dwDoingParamY, NpcSync->dwDoingParamZ);
		}
		else
		{
			Npc[nIdx].SendCommand(do_stand);
		}
	}

	Npc[nIdx].SetEquipTalismanNpcId(NpcSync->TalismanNpcId);
	Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	strcpy(Npc[nIdx].Name, NpcSync->m_szName);

	Npc[nIdx].SetCityId(NpcSync->CityId);
	Npc[nIdx].SetKing(NpcSync->IsKing == 1 ? true : false);
	Npc[nIdx].SetGensMgr(NpcSync->IsGensMgr == 1?true:false);

	Npc[nIdx].SetZhuhouName(NpcSync->ZhuhouName);
	Npc[nIdx].SetShizuName(NpcSync->ShizuName);
	Npc[nIdx].SetInvaderTongName(NpcSync->invaderTongName);

	ConfigManager& cm = ConfigManager::Singleton();
	
	Npc[nIdx].DelHeadInfo(HEAD_INFO_GENS);
	Npc[nIdx].DelHeadInfo(HEAD_INFO_TONG);
	
	//HeadInfo Added
	if (Npc[nIdx].IsKing())
	{
		const char* pTbuff = cm.GetConfigurableDisplayStyle( style_role_head_image_info, 0 );
		if (pTbuff)
			Npc[nIdx].AddLayoutToHeadInfo(pTbuff,HEAD_INFO_TONG,KNpc::HIP_Important);
		
	}//endif
	else if (Npc[nIdx].IsGensMgr())
	{
		const char* pTbuff = cm.GetConfigurableDisplayStyle( style_role_head_image_info, 1 );
		if (pTbuff)
			Npc[nIdx].AddLayoutToHeadInfo(pTbuff,HEAD_INFO_GENS,KNpc::HIP_Important);
	}//end for if

	if(INVALID_WORLD_ID != NpcSync->CityId)
	{
		char szCityName[MAXSIZE_CITYNAME] = { 0 };
		g_SubWorldSet.GetWorldNameFromID(NpcSync->CityId, szCityName, sizeof(szCityName));
		Npc[nIdx].SetCityName(szCityName);
	}

	if (Npc[nIdx].m_Kind == kind_creature)
	{
		const int nSummonerID = NpcSync->nSummonID;
		// 是自己的宝宝
		if (nSummonerID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
		{
			const SUMMONPARAM SummonParam =
			{ 
				450,
				1,
				Player[CLIENT_PLAYER_INDEX].m_nIndex,nIdx,
				1 // todo 设定文件
			};
			Player[CLIENT_PLAYER_INDEX].m_Creature.Summon(SummonParam);
		}
		Npc[nIdx].SetSummonerIdx(nSummonerID);
	}

	if (Npc[nIdx].m_Kind == kind_player)
	{
		Npc[nIdx].m_SkillType = NpcSync->nSkillType;	
	}//endif

}
*/

void KProtocolProcess::SyncNpc(BYTE* pMsg)
{
	NPC_SYNC* NpcSync = (NPC_SYNC *)pMsg;

	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[0].Mps2Map(NpcSync->MapX, NpcSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	if (nRegion == -1)
		return;

	int nIdx = NpcSet.SearchID(NpcSync->ID);

#define MAX_COPY_NAME_LENGTH 32

	char roleName[MAX_COPY_NAME_LENGTH] = { 0 };
	char shizuName[MAX_COPY_NAME_LENGTH] = { 0 };
	char zhuhouName[MAX_COPY_NAME_LENGTH] = { 0 };

	//把四个拼在一起的字符串拆分成四个字符串
	char* pNameBuff = NpcSync->NameBuff;
	int nameLength = 0;

	//角色名
	nameLength = strlen(strncpy(roleName, pNameBuff, sizeof(roleName)));
	roleName[sizeof(roleName) - 1] = 0;
	pNameBuff += nameLength + 1;

	//氏族名
	nameLength = strlen(strncpy(shizuName, pNameBuff, sizeof(shizuName)));
	shizuName[sizeof(shizuName) - 1] = 0;
	pNameBuff += nameLength + 1;

	//诸侯名
	nameLength = strlen(strncpy(zhuhouName, pNameBuff, sizeof(zhuhouName)));
	zhuhouName[sizeof(zhuhouName) - 1] = 0;
	pNameBuff += nameLength + 1;


	if (NpcSync->m_btKind == kind_player && roleName[0] != 0)
	{
       bool bIsEnymy=g_ChatCenterC.IsObjectInGroup(roleName, GROUPID_ENEMY);
	   char szName[256];
	   szName[0]=0;

	   sprintf(szName,ENYMY_IS_COMMING, roleName);

       if  (bIsEnymy)
	   {
            CoreDataChanged( GDCNI_ERROR_MESSAGE, (unsigned int)szName, 0);     
	   }//endif

	}//endif

	if (!nIdx)
	{
		nIdx = NpcSet.Add(NpcSync->NpcSettingIdx, 0, NpcSync->MapX, NpcSync->MapY);

		Npc[nIdx].m_dwID = NpcSync->ID;
		Npc[nIdx].m_Kind = NpcSync->m_btKind;
		Npc[nIdx].m_Height = 0;			
		
		if (NpcSet.IsNpcRequestExist(NpcSync->ID))
		{
			NpcSet.RemoveNpcRequest(NpcSync->ID);
		}

// 		if (kind_building == NpcSync->m_btKind)
// 		{
// 			g_DirtyNpcSet.PushItem(nIdx);
// 		}
	}
	else
	{
		if ( Npc[nIdx].m_Doing == do_death && 
			 Npc[nIdx].m_Kind != kind_player )
		{
			return;
		}
		Npc[nIdx].m_NpcSettingIdx = (short)HIWORD(NpcSync->NpcSettingIdx);
		Npc[nIdx].m_Level = LOWORD(NpcSync->NpcSettingIdx);
		Npc[nIdx].MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);
	}

	if (!Npc[nIdx].IsPlayer())
	{
		Npc[nIdx].m_UnaryAttrMgr.Set(nuai_dir, NpcSync->byNpcDir);
	}
	Npc[nIdx].m_Series			= NpcSync->m_bySeries;
	Npc[nIdx].m_WorldCombatOrg  = NpcSync->nCombatOrg;
	Npc[nIdx].m_UnaryAttrMgr.Set(nuai_camou_flage ,NpcSync->bCamouflage);

	if(Npc[nIdx].m_UnaryAttrMgr[nuai_titlecolor] != NpcSync->dwNameColor)
	{
		Npc[nIdx].m_UnaryAttrMgr.Set(nuai_titlecolor, NpcSync->dwNameColor);
		Npc[nIdx].SetHeadInfoChanged(true);
	}

	Npc[nIdx].SetCurrentLifePercentage(NpcSync->LifePercentage);
	Npc[nIdx].SetCurrentManaPercentage(NpcSync->ManaPercentage);
	Npc[nIdx].m_UnaryAttrMgr.Set(nuai_fightstate, NpcSync->bFightState);
	Npc[nIdx].m_CompAttrMgr.Set(ncai_runspeed, idx_base_value, NpcSync->RunSpeed[0]);
	Npc[nIdx].m_CompAttrMgr.Set(ncai_runspeed, idx_append_value, NpcSync->RunSpeed[1]);
	Npc[nIdx].m_CompAttrMgr.Set(ncai_runspeed, idx_append_percent, NpcSync->RunSpeed[2]);
	Npc[nIdx].m_UnaryAttrMgr.Set(nuai_servercont, NpcSync->nCondition );
	Npc[nIdx].m_nHeadImage = NpcSync->HeadImage;

	if (kind_player == Npc[nIdx].m_Kind)
	{
		int originalTeamId = Npc[nIdx].m_UnaryAttrMgr[nuai_team_id];
		Npc[nIdx].m_UnaryAttrMgr.Set(nuai_team_id, NpcSync->TeamId);
		int newTeamId = Npc[nIdx].m_UnaryAttrMgr[nuai_team_id];
		g_TeamViewer.TeamChanged(nIdx, originalTeamId, newTeamId);
	}

//	if (Npc[nIdx].m_Doing != do_death || Npc[nIdx].m_Doing != do_revive)
	if(do_revive == NpcSync->m_Doing && kind_player == Npc[nIdx].m_Kind)
	{
		// 如果是do_revive状态，客户端必须保持这个状态，否则
		// 其他人对这个人的复活技能将会无效
		Npc[nIdx].m_Doing = (NPCCMD)NpcSync->m_Doing;
	}
	else
	{
		if (Npc[nIdx].m_Kind != kind_building)
		{
			Npc[nIdx].SendCommand((NPCCMD)NpcSync->m_Doing, 
				NpcSync->dwDoingParamX, NpcSync->dwDoingParamY, NpcSync->dwDoingParamZ);
		}
		else
		{
			Npc[nIdx].SendCommand(do_stand);
		}
	}

	Npc[nIdx].SetEquipTalismanNpcId(NpcSync->TalismanNpcId);
	Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	strncpy(Npc[nIdx].Name, roleName, sizeof(Npc[nIdx].Name));

	BYTE isKing = NpcSync->IsKingOrGens >> 4;
	BYTE isGens = NpcSync->IsKingOrGens & 0x0F;

	int nCityID = NpcSync->CityId==255?INVALID_WORLD_ID:NpcSync->CityId;

	Npc[nIdx].SetCityId(nCityID);
	Npc[nIdx].SetKing(isKing == 1 ? true : false);
	Npc[nIdx].SetGensMgr(isGens == 1 ? true:false);

	Npc[nIdx].SetZhuhouName(zhuhouName);
	Npc[nIdx].SetShizuName(shizuName);
	
	if (nIdx != CLIENT_PLAYER_INDEX)
	{
		BYTE nIsHaveLeagueLayer = (NpcSync->nLeagueFlag >> 4);
		BYTE nInvaderTongFlag   = (NpcSync->nLeagueFlag & 0x0f);

		Npc[nIdx].SetHasLeagueLayer(nIsHaveLeagueLayer?true:false);
		Npc[nIdx].SetInvaderTongFlag(nInvaderTongFlag);
	}//endif

	ConfigManager& cm = ConfigManager::Singleton();
	
	Npc[nIdx].DelHeadInfo(HEAD_INFO_GENS);
	Npc[nIdx].DelHeadInfo(HEAD_INFO_TONG);
	
	//HeadInfo Added
	if (Npc[nIdx].IsKing())
	{
		const char* pTbuff = cm.GetConfigurableDisplayStyle( style_role_head_image_info, 0 );
		if (pTbuff)
			Npc[nIdx].AddLayoutToHeadInfo(pTbuff,HEAD_INFO_TONG,KNpc::HIP_Important);
		
	}//endif
	else if (Npc[nIdx].IsGensMgr())
	{
		const char* pTbuff = cm.GetConfigurableDisplayStyle( style_role_head_image_info, 1 );
		if (pTbuff)
			Npc[nIdx].AddLayoutToHeadInfo(pTbuff,HEAD_INFO_GENS,KNpc::HIP_Important);
	}//end for if

	if(INVALID_WORLD_ID != nCityID)
	{
		char szCityName[MAXSIZE_CITYNAME] = { 0 };
		g_SubWorldSet.GetWorldNameFromID(NpcSync->CityId, szCityName, sizeof(szCityName));
		Npc[nIdx].SetCityName(szCityName);
	}

	if (Npc[nIdx].m_Kind == kind_creature)
	{
		const DWORD nSummonerID = NpcSync->nSummonID;
		// 是自己的宝宝
		if (nSummonerID == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
		{
			const SUMMONPARAM SummonParam =
			{ 
				450,
				1,
				Player[CLIENT_PLAYER_INDEX].m_nIndex,nIdx,
				1 // todo 设定文件
			};
			Player[CLIENT_PLAYER_INDEX].m_Creature.Summon(SummonParam);
		}
		Npc[nIdx].SetSummonerIdx(nSummonerID);
	}
	else if (Npc[nIdx].m_Kind == kind_employee)
	{
		const DWORD nSummonerID = NpcSync->nSummonID;
		Npc[nIdx].SetEmployerId(nSummonerID);
	}

	if (Npc[nIdx].m_Kind == kind_player || Npc[nIdx].m_Kind == kind_employee)
	{
		Npc[nIdx].m_SkillType = NpcSync->nSkillType;	
	}//endif

}

void KProtocolProcess::SyncNpcMin(BYTE* pMsg)
{
	NPC_NORMAL_SYNC* npcSync = (NPC_NORMAL_SYNC *)pMsg;
	
	int nIdx = NpcSet.SearchID(npcSync->ID);
	if (!nIdx)
	{
		// 向服务器请求同步这个NPC的全部数据
		if (!NpcSet.IsNpcRequestExist(npcSync->ID))
		{
			SendClientCmdRequestNpc(npcSync->ID);
			NpcSet.InsertNpcRequest(npcSync->ID);
		}
	}
	else
	{
	
		if ( Npc[nIdx].m_Doing == do_death && 
			 Npc[nIdx].m_Kind != kind_player )
		{
			return;
		}

		KNpc& npc = Npc[nIdx];

		int nRegion, nMapX, nMapY, nOffX, nOffY;
		SubWorld[0].Mps2Map(npcSync->MapX, npcSync->MapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

		if (nIdx == GetClientPlayer().GetNpcIndex())
		{
			if (Npc[GetClientPlayer().GetNpcIndex()].GetChangeWorld())
			{	
				Npc[GetClientPlayer().GetNpcIndex()].SetChangeWorldFlag(false);
				SendClientCmdRun(npcSync->MapX,npcSync->MapY);
			}//endif
			
		}//endif

		if( npcSync->byNeedUpdate )
			SendClientCmdRequestNpc(npcSync->ID);

		if (Npc[nIdx].m_RegionIndex == -1 && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)	
		{
			if (nRegion == -1)
			{		
				return;
			}
			else
			{	
				Npc[nIdx].MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);
			//	SendClientCmdRequestNpc(npcSync->ID);
			}
			
			if (npcSync->Doing == do_stand)
				Npc[nIdx].ProcNetCommand(do_stand);
		}
		else
		{
			//这里是引起怪物跳帧的关键，可考虑在Npc::ServeMove 进行Region的转换逻辑！Brianyao2007
			if (Npc[nIdx].m_RegionIndex != nRegion && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				Npc[nIdx].MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);
			}
        
			// Fix by cooler 2007-05-26
			if(nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex && Npc[nIdx].m_Kind!=kind_player  && Npc[nIdx].m_Kind!=kind_partner && Npc[nIdx].m_Kind!=kind_dialoger)
			{
				BOOL bOutRange = FALSE;
				int nMpsXX, nMpsYY;
				Npc[nIdx].GetMpsPos(&nMpsXX, &nMpsYY);

				int iDis=((int)npcSync->MapX - nMpsXX) * ((int)npcSync->MapX - nMpsXX) + ((int)npcSync->MapY - nMpsYY) * ((int)npcSync->MapY - nMpsYY);
                
				if (Npc[nIdx].m_Doing != do_run )
				{
					
					if(iDis > 16 * 16)
					{
						//	Npc[nIdx].MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);
						if (iDis < 256*256 )
						{
							Npc[nIdx].SendCommand(do_run,npcSync->MapX,npcSync->MapY);
							Npc[nIdx].BeginEditionState();
						}//endif
						else	
							Npc[nIdx].MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);
					}      

				}//endif
				else
				{
					if(iDis > 32 * 32)
					{
						//	Npc[nIdx].MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);
						if (iDis < 512*512 )
						{
							Npc[nIdx].SendCommand(do_run,npcSync->MapX,npcSync->MapY);
							Npc[nIdx].BeginEditionState();
						}//endif
						else
							Npc[nIdx].MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);
					}    
					else
					{
						Npc[nIdx].EndEditionState();
					}

				}//end else

			}
		}

		npc.SetCurrentLifePercentage(npcSync->LifePercentage);
		npc.SetCurrentManaPercentage(npcSync->ManaPercentage);

		if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)	// 非玩家
		{
			//Npc[nIdx].m_CurrentCamp = npcSync->Camp & 0x0F;

			if ( Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetTargetNpc() == nIdx )
			{
				CoreDataChanged( CDCNI_UPDATA_SEL_TARGET, NULL, NULL );
			}

			if(Npc[nIdx].IsCreature())
			{
				pair<int, int> blood;
				blood.first = npcSync->LifePercentage;
				blood.second = 100;
				CoreDataChanged(GDCNI_PET_UPDATE, (unsigned int)&blood, nIdx);
			}
		}
		else
		{
			CoreDataChanged( CDCNI_UPDATA_SELF_FACE, NULL, NULL );
			CoreDataChanged( CDCNI_UPDATA_ROLE_STATE, NULL, NULL );
		}

//		Npc[nIdx].m_tagProduceState.nProduceSpeed = npcSync->State & STATE_PRODUCE;

		Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;//设置时间戳，防止被CheckBalance删掉

		DWORD uLastTime = QuestLog::GetInstance()->GetQuestProcessLastUpDateTime();
		if (Npc[nIdx].m_Kind == kind_dialoger && uLastTime != Npc[nIdx].m_nLastQuestStateQueryTime)
		{
			_QUERY_NPC_QUEST_STATE QueryState;
			QueryState.Protocol		= c2s_quest_family;
			QueryState.ProtocolExtend	= c2s_query_npc_quest_state;
			QueryState.wProtocolSize	= sizeof(_QUERY_NPC_QUEST_STATE) - 1;
			QueryState.dwNpcID		= Npc[nIdx].m_dwID;
			if (g_pClient)
				g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&QueryState, sizeof(_QUERY_NPC_QUEST_STATE));
			Npc[nIdx].m_nLastQuestStateQueryTime = uLastTime;
		}
	}

	//用更精确的信息刷新客户端队友的数据
	KPlayerTeam& teamInfo = GetClientPlayer().GetTeamInfo();
	if (teamInfo.IsInTeam())
	{
		KTeam* pTeam = teamInfo.GetTeam();
		if (pTeam != NULL)
		{
			int memberIndex = pTeam->FindMemberID(npcSync->ID);
			if (memberIndex >= 0)
			{
				ClientTeamMemberInfo* pMemberInfo = pTeam->GetMemberInfo(memberIndex);
				if (pMemberInfo != NULL)
				{
					//pMemberInfo->Life = pMemberInfo->LifeMax * npcSync->LifePercentage / 100;
					//pMemberInfo->Mana = pMemberInfo->ManaMax * npcSync->ManaPercentage / 100;
					pMemberInfo->PosX = npcSync->MapX;
					pMemberInfo->PosY = npcSync->MapY;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息同步本玩家npc数据
//-------------------------------------------------------------------------
void KProtocolProcess::SyncNpcMinPlayer(BYTE* pMsg)
{	
	NPC_PLAYER_TYPE_NORMAL_SYNC	*pSync = (NPC_PLAYER_TYPE_NORMAL_SYNC*)pMsg;

	if (pSync->m_dwNpcID != Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID)
		return;

	int nRegion, nMapX, nMapY, nOffX, nOffY, nNpcIdx;
	SubWorld[0].Mps2Map(pSync->m_dwMapX, pSync->m_dwMapY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nNpcIdx > 0)
	{
		int nRegionX = pSync->m_dwMapX / REGION_PIXEL_WIDTH; 
		int nRegionY = pSync->m_dwMapY / REGION_PIXEL_HEIGHT;	
		DWORD	dwRegionID = MAKELONG(nRegionX, nRegionY);	
		if (nRegion == -1)
		{
			NpcSet.ForceRemoveAllNpcHeadInfo();
			SubWorld[0].LoadMap(SubWorld[0].m_SubWorldID, dwRegionID, 0, false);
			Npc[nNpcIdx].SendCommand(do_stand);			
			//Npc[nNpcIdx].DoStand();
//			AutoRobotMgr::Singleton().ClearCanNotAttackNpcList();
		}
		else if (Npc[nNpcIdx].m_RegionIndex == -1 && nRegion != -1)
		{
			int nMpsX = 0,nMpsY = 0;
			Npc[nNpcIdx].MoveNpc(nRegion, nMapX, nMapY, pSync->m_wOffX, pSync->m_wOffY);
			//强制同步坐标，为自动寻路做准备////////
			Npc[nNpcIdx].m_DataRes.m_nXpos = pSync->m_dwMapX;
			Npc[nNpcIdx].m_DataRes.m_nYpos = pSync->m_dwMapY;
			Npc[nNpcIdx].m_DataRes.m_nXposNew = pSync->m_dwMapX;
			Npc[nNpcIdx].m_DataRes.m_nYposNew = pSync->m_dwMapY;
			Npc[nNpcIdx].SendCommand(do_stand);		
			//Npc[nNpcIdx].DoStand();
			//AutoRobotMgr::Singleton().ProcessChangeWorld();
			NpcSet.ForceRemoveAllNpcHeadInfo();
		}	
		else
		{
			// to do nothing.
		}

		Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}

}

//-------------------------------------------------------------------------
//	功能：收到服务器消息添加一个obj
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectAdd(BYTE* pMsg)
{
	OBJ_ADD_SYNC	*pObjSyncAdd = (OBJ_ADD_SYNC*)pMsg;
	
	bool          bCanPlayerPick = (( pObjSyncAdd->m_btState >> 7) ? true : false);
	pObjSyncAdd->m_btState       = (pObjSyncAdd->m_btState & 0x7f);
	
	int				nObjIndex;
	KObjItemInfo	sInfo;

	nObjIndex = ObjSet.FindID(pObjSyncAdd->m_nID);
	if (nObjIndex > 0)
		return;

	sInfo.m_nItemID = pObjSyncAdd->m_nItemID;
	sInfo.m_nMoneyNum = pObjSyncAdd->m_nMoneyNum;
	sInfo.m_nColorID = pObjSyncAdd->m_btColorID;
	sInfo.m_nMovieFlag = ((pObjSyncAdd->m_btFlag & 0x02) > 0 ? 1 : 0);
	sInfo.m_nSoundFlag = ((pObjSyncAdd->m_btFlag & 0x01) > 0 ? 1 : 0);
	memset(sInfo.m_szName, 0, sizeof(sInfo.m_szName));
	memcpy(sInfo.m_szName, pObjSyncAdd->m_szName, pObjSyncAdd->m_wLength + 1 + sizeof(pObjSyncAdd->m_szName) - sizeof(OBJ_ADD_SYNC));
	nObjIndex = ObjSet.ClientAdd(
		pObjSyncAdd->m_nID,
		pObjSyncAdd->m_nDataID,
		pObjSyncAdd->m_btState,
		pObjSyncAdd->m_btDir,
		pObjSyncAdd->m_wCurFrame,
		pObjSyncAdd->m_nXpos,
		pObjSyncAdd->m_nYpos,
		sInfo);

	if(nObjIndex > 0)
	{
		Object[nObjIndex].m_bCanPickForClient = bCanPlayerPick;

		if(Object[nObjIndex].m_nKind == Obj_Kind_Money)
		{
			if(Object[nObjIndex].m_nMoneyNum > 0)
				sprintf(Object[nObjIndex].m_szName, "%d%s", Object[nObjIndex].m_nMoneyNum, TONG);
			else
				sprintf(Object[nObjIndex].m_szName, "%s", MONEY);
		}
		else if (Object[nObjIndex].m_nKind == Obj_Kind_Item)			
		{
			if (Object[nObjIndex].m_nMoneyNum != 0)
			{
				sprintf(Object[nObjIndex].m_szName,"%sX%d",sInfo.m_szName,sInfo.m_nMoneyNum);
			}
		}
	}
#ifdef WAIGUA_ZROC
	if (nObjIndex <= 0)
		return;
	PLAYER_PICKUP_ITEM_COMMAND	sPickUp;
	if (Object[nObjIndex].m_nKind == Obj_Kind_Money)
	{
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosX = 0;
		sPickUp.m_btPosY = 0;
		if (g_pClient)
			g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
	}
	else if (Object[nObjIndex].m_nKind == Obj_Kind_Item)
	{
		ItemPos	sItemPos;
		if ( FALSE == Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(Object[nObjIndex].m_nItemWidth, Object[nObjIndex].m_nItemHeight, &sItemPos, NULL) )
			return;
		sPickUp.ProtocolType = c2s_playerpickupitem;
		sPickUp.m_nObjID = Object[nObjIndex].m_nID;
		sPickUp.m_btPosX = sItemPos.nX;
		sPickUp.m_btPosY = sItemPos.nY;
		if (g_pClient)
			g_pClient->SendPackToServer(&sPickUp, sizeof(PLAYER_PICKUP_ITEM_COMMAND));
	}
#endif
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息更新某个obj方向
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectDir(BYTE* pMsg)
{
	OBJ_SYNC_DIR	*pObjSyncDir = (OBJ_SYNC_DIR*)pMsg;
	int				nObjIndex;

	nObjIndex = ObjSet.FindID(pObjSyncDir->m_nID);
	if (nObjIndex <= 0)
	{
		// 向服务器发添加请求
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjSyncDir->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(g_ConnectID,&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{	// 同步方向
		Object[nObjIndex].SetDir(pObjSyncDir->m_btDir);
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息删除某个obj
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectRemove(BYTE* pMsg)
{
	OBJ_SYNC_REMOVE	*pObjSyncRemove = (OBJ_SYNC_REMOVE*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncRemove->m_nID);
	if (nObjIndex > 0)
	{	// 删除
		int nTargetType = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetTargetType();
		int nTargetObjIdx = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetTargetObj();
#ifndef _SERVER
		// 播放捡物品动画
		if ( pObjSyncRemove->m_bPickUp && 
			 pObjSyncRemove->m_nPlayerID == Player[CLIENT_PLAYER_INDEX].GetPlayerID() )
		{
			if ( Object[nObjIndex].GetKind() == Obj_Kind_Item )
			{
				ScreenEffectMgr::Singleton().Player(2, BeforeUi);
			}
		}
#endif
		if ( nTargetType == type_obj && nTargetObjIdx == nObjIndex )
		{
			Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].SetTarget(type_obj,0);
		}
		Object[nObjIndex].Remove(pObjSyncRemove->m_btSoundFlag);
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息更新某个obj状态
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectState(BYTE* pMsg)
{
	OBJ_SYNC_STATE	*pObjSyncState = (OBJ_SYNC_STATE*)pMsg;
	int				nObjIndex;
	nObjIndex = ObjSet.FindID(pObjSyncState->m_nID);
	if (nObjIndex <= 0)
	{
		// 向服务器发添加请求
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjSyncState->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(g_ConnectID,&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{	// 同步状态
		if (Object[nObjIndex].m_nRegionIdx == -1)
		{
			int	nRegion;
			nRegion = SubWorld[0].FindRegion(Object[nObjIndex].m_nBelongRegion);
			if (nRegion >= 0)
			{
				Object[nObjIndex].m_nRegionIdx = nRegion;
				SubWorld[0].m_Region[nRegion].AddObj(nObjIndex);
			}
		}
		Object[nObjIndex].SetState(pObjSyncState->m_btState);
	}
}

//-------------------------------------------------------------------------
//	功能：收到服务器消息同步某个 trap 类 obj 的数据
//-------------------------------------------------------------------------
void KProtocolProcess::SyncObjectTrap(BYTE* pMsg)
{
	OBJ_SYNC_TRAP_ACT *pObjTrapSync = (OBJ_SYNC_TRAP_ACT*)pMsg;
	int		nObjIndex;
	nObjIndex = ObjSet.FindID(pObjTrapSync->m_nID);
	if (nObjIndex <= 0)
	{
		// 向服务器发添加请求
		OBJ_CLIENT_SYNC_ADD	sObjClientSyncAdd;
		sObjClientSyncAdd.ProtocolType = c2s_requestobj;
		sObjClientSyncAdd.m_nID = pObjTrapSync->m_nID;
		if (g_pClient)
			g_pClient->SendPackToServer(g_ConnectID,&sObjClientSyncAdd, sizeof(sObjClientSyncAdd));
	}
	else
	{
		Object[nObjIndex].m_nState = OBJ_TRAP_STATE_ACTING;
		Object[nObjIndex].m_cImage.SetDirStart();
		Object[nObjIndex].m_cSkill.m_nTarX = pObjTrapSync->m_nTarX;
		Object[nObjIndex].m_cSkill.m_nTarY = pObjTrapSync->m_nTarY;
	}
}

void KProtocolProcess::SyncPlayer(BYTE* pMsg)
{
	PLAYER_SYNC*	pPlaySync = (PLAYER_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pPlaySync->ID);
	if ( nIdx == 0 )
	{
		return;
	}

// 	Npc[nIdx].m_WeaponType = pPlaySync->WeaponType;
// 	Npc[nIdx].m_HelmType = pPlaySync->HelmType;
// 	Npc[nIdx].m_ArmorType = pPlaySync->ArmorType;
// 	Npc[nIdx].m_HorseType = pPlaySync->HorseType;
// 	Npc[nIdx].m_ShoulderType = pPlaySync->ShoulderType;
// 	Npc[nIdx].m_BootType = pPlaySync->BootType;
// 	Npc[nIdx].m_CuffType = pPlaySync->CuffType;

	Npc[nIdx].m_bRideHorse = pPlaySync->m_btSomeFlag & 0x01;

	SplitTPfromWORD( pPlaySync->WeaponType,		Npc[nIdx].m_WeaponType,		Npc[nIdx].m_WeaponPal	);
	SplitTP( pPlaySync->HelmType,		Npc[nIdx].m_HelmType,		Npc[nIdx].m_HelmPal		);
	SplitTP( pPlaySync->ArmorType,		Npc[nIdx].m_ArmorType,		Npc[nIdx].m_ArmorPal	);
	SplitTP( pPlaySync->HorseType,		Npc[nIdx].m_HorseType,		Npc[nIdx].m_HorsePal	);
	SplitTP( pPlaySync->ShoulderType,	Npc[nIdx].m_ShoulderType,	Npc[nIdx].m_ShoulderPal	);
	SplitTP( pPlaySync->BootType,		Npc[nIdx].m_BootType,		Npc[nIdx].m_BootPal		);
	SplitTP( pPlaySync->CuffType,		Npc[nIdx].m_CuffType,		Npc[nIdx].m_CuffPal		);
	
	Npc[nIdx].m_CompAttrMgr.Set(ncai_attackspeed, idx_base_value, pPlaySync->AttackSpeed);
	Npc[nIdx].m_CompAttrMgr.Set(ncai_castspeed, idx_base_value, pPlaySync->CastSpeed);

	Npc[nIdx].SetTitle(pPlaySync->TitleIndex, pPlaySync->TitleLevel);

	//Npc[nIdx].m_Kind				= kind_player;
}

void KProtocolProcess::SyncPlayerMin(BYTE* pMsg)
{
	PLAYER_NORMAL_SYNC* pPlaySync = (PLAYER_NORMAL_SYNC *)pMsg;

	int nIdx = NpcSet.SearchID(pPlaySync->ID);
	if (nIdx > 0)
	{
/*
优化前
		Npc[nIdx].m_HelmType = pPlaySync->HelmType;
		Npc[nIdx].m_HorseType = pPlaySync->HorseType;
		Npc[nIdx].m_ArmorType = pPlaySync->ArmorType;
		Npc[nIdx].m_ShoulderType = pPlaySync->ShoulderType;
		Npc[nIdx].m_WeaponType = pPlaySync->WeaponType;
		Npc[nIdx].m_BootType = pPlaySync->BootType;
		Npc[nIdx].m_CuffType = pPlaySync->CuffType;
*/

// 		Npc[nIdx].m_WeaponType = pPlaySync->WeaponType;
// 		Npc[nIdx].m_HelmType = pPlaySync->HelmType;
// 		Npc[nIdx].m_ArmorType = pPlaySync->ArmorType;
// 		Npc[nIdx].m_HorseType = pPlaySync->HorseType;
// 		Npc[nIdx].m_ShoulderType = pPlaySync->ShoulderType;
// 		Npc[nIdx].m_BootType = pPlaySync->BootType;
// 		Npc[nIdx].m_CuffType = pPlaySync->CuffType;

		Npc[nIdx].m_bRideHorse = pPlaySync->m_btSomeFlag & 0x01;

		SplitTPfromWORD( pPlaySync->WeaponType,		Npc[nIdx].m_WeaponType,		Npc[nIdx].m_WeaponPal	);
		SplitTP( pPlaySync->HelmType,		Npc[nIdx].m_HelmType,		Npc[nIdx].m_HelmPal		);
		SplitTP( pPlaySync->ArmorType,		Npc[nIdx].m_ArmorType,		Npc[nIdx].m_ArmorPal	);
		SplitTP( pPlaySync->HorseType,		Npc[nIdx].m_HorseType,		Npc[nIdx].m_HorsePal	);
		SplitTP( pPlaySync->ShoulderType,	Npc[nIdx].m_ShoulderType,	Npc[nIdx].m_ShoulderPal	);
		SplitTP( pPlaySync->BootType,		Npc[nIdx].m_BootType,		Npc[nIdx].m_BootPal		);
		SplitTP( pPlaySync->CuffType,		Npc[nIdx].m_CuffType,		Npc[nIdx].m_CuffPal		);

		//Npc[nIdx].m_Kind = kind_player;

		if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
		{
			Npc[nIdx].m_CompAttrMgr.Set(ncai_attackspeed, idx_base_value, pPlaySync->AttackSpeed);
			Npc[nIdx].m_CompAttrMgr.Set(ncai_castspeed, idx_base_value, pPlaySync->CastSpeed);
		}
	}
}

void KProtocolProcess::SyncScriptAction(BYTE* pMsg)
{
	Player[CLIENT_PLAYER_INDEX].OnScriptAction((PLAYER_SCRIPTACTION_SYNC *)pMsg);
}

void KProtocolProcess::SyncWorld(BYTE* pMsg)
{
	WORLD_SYNC *WorldSync = (WORLD_SYNC *)pMsg;	
	KPlayer& player = GetClientPlayer();

	if ( WorldSync->SubWorld != SubWorld[0].m_SubWorldID )
	{
		SearchContentParam searchParam;
		searchParam.id = WorldSync->SubWorld;
		searchParam.queryType = maps_query;
		searchParam.queryResultType = format_string;
		CoreDataChanged( GDCNI_SEARCH_INFO, (unsigned int)&searchParam, NULL );
		Npc[player.GetNpcIndex()].SetChangeWorldFlag(true);
		Player[CLIENT_PLAYER_INDEX].ClearRunPackageRecord();

		NpcSet.RemoveAllSyncToWorldNpcs();
		NpcSet.RemoveAllNpcExceptClient();
	}

	CoreDataChanged( GDCNI_GAME_PRE_LOAD_MAP, true, defaultMap );

	if (player.GetNpcIndex() > 0)
	{
		KNpc& npc = Npc[player.GetNpcIndex()];
		npc.SetEquipTalismanNpcId(0);
		npc.SetHeadInfoChanged(true);
	}

	SubWorld[0].LoadMap(WorldSync->SubWorld, WorldSync->Region, 0, true);
	NpcSet.ForceRemoveAllNpcHeadInfo();
//	AutoRobotMgr::Singleton().ClearCanNotAttackNpcList();
	AutoRobotMgr::Singleton().canNotAttackNpcList.clear();
	SubWorld[0].m_dwCurrentTime = WorldSync->Frame;

	DWORD expireTime = 0;
	if (WorldSync->ExpireLeftTime > 0)
	{
		expireTime = time(NULL) + WorldSync->ExpireLeftTime;
		
		pair<int, int> timerParam(WorldSync->ExpireLeftTime, timer_instance_expire);
		char expireMsg[256] = { 0 };
		g_GetStringRes(sid_instance_expire, expireMsg, sizeof(expireMsg));
		CoreDataChanged(GDCNI_OPEN_TIMER, (unsigned int)(&timerParam), (int)expireMsg);
	}
	else
	{		
		pair<int, int> timerParam(0, timer_instance_expire);
		char expireMsg[256] = { 0 };
		CoreDataChanged(GDCNI_OPEN_TIMER, (unsigned int)(&timerParam), (int)expireMsg);
	}
	SubWorld[0].SetExpireTime(expireTime);
	SubWorld[0].ClearWorldMapPlayerInfoCache();
	SubWorld[0].ClearWarCommanderInfoCache();

	if (player.GetNpcIndex() > 0)
	{
		KNpc& npc = Npc[player.GetNpcIndex()];
		//npc.DoStand();
		npc.SendCommand(do_stand);			
		npc.SetProcessAI(TRUE);
		npc.SetTarget( type_obj, 0 );
		npc.SetTarget( type_npc, 0 );
		npc.SetCanFollowAndAttack( false );
	//	npc.m_bIsPkArea = especial_area_none - 1;
	}


	PlayerController::Singleton().ChangeWorld();
	MapPosInfo mapInfo;
	Position pos;
	g_ScenePlace.getMapInfoAtPos(&mapInfo,&pos);
	AutoGoBack::Singleton().SetCurrentMapInfo(mapInfo.mapName);
	AutoRobotMgr::Singleton().GetPathFinder().StopAutoWalk();
	AutoRobotMgr::Singleton().ProcessChangeWorld();
	if (player.GetNpcIndex() > 0)
	{
		KNpc& npc = Npc[player.GetNpcIndex()];
		npc.SetHeadInfoChanged(true);
	}

	CoreDataChanged(GDCNI_RECV_WORLD_COMBAT_SCORE, 0 , 0);
	CoreDataChanged( GDCNI_GAME_END_LOAD_PROGRESS, NULL, NULL );
}

void	KProtocolProcess::s2cSyncAllSkill(BYTE * pMsg)
{
	SKILL_SEND_ALL_SYNC	* pSync = (SKILL_SEND_ALL_SYNC*) pMsg;

	int nNpcIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	Npc[nNpcIndex].m_SkillList.LoadSkillData((BYTE*)&pSync->m_version, pSync->m_wProtocolLong - 2);
	CoreDataChanged( GDCNI_UPDATA_SHORTCUT, NULL, NULL );
}

void KProtocolProcess::SyncEnd(BYTE* pMsg)
{
	
	BYTE	SyncEnd = (BYTE)c2s_syncend;
	if (g_pClient)
	{
		g_pClient->SendPackToServer(g_ConnectID,&SyncEnd, sizeof(BYTE));
	}

	// 开始播音乐
	g_SubWorldSet.m_cMusic.Start(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, FALSE);

	CoreDataChanged(GDCNI_GAME_START, 0, 0);	
}

void KProtocolProcess::s2cTradeChangeState(BYTE* pMsg)
{
	if(NULL == pMsg)
		return;

	TRADE_CHANGE_STATE_SYNC	*pTrade = (TRADE_CHANGE_STATE_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].tradeClientStateChange(pTrade->m_dwNpcID, pTrade->m_btState);
}

void	KProtocolProcess::s2cTradeMoneySync(BYTE* pMsg)
{
	TRADE_MONEY_SYNC *pMoney = (TRADE_MONEY_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].tradeClientReciveOppositeMoneyChanged(pMoney->m_nMoney);
}

void	KProtocolProcess::s2cTradeDecision(BYTE* pMsg)
{
	TRADE_DECISION_SYNC* sState = (TRADE_DECISION_SYNC*)pMsg;
	switch(sState->m_btDecision)
	{
	case KTrade::TRADE_MSG_SELF_LOCK:
		Player[CLIENT_PLAYER_INDEX].tradeClientReciveLock(true);
		break;
	case KTrade::TRADE_MSG_OPPOSITE_LOCK:
		Player[CLIENT_PLAYER_INDEX].tradeClientReciveLock(false);
		break;
	case KTrade::TRADE_MSG_SELF_UNLOCK:
		Player[CLIENT_PLAYER_INDEX].tradeClientReciveUnlock();
		break;
	case KTrade::TRADE_MSG_SELF_END_TRADE:
		Player[CLIENT_PLAYER_INDEX].tradeClientReciveEndTrade(true);
		break;
	case KTrade::TRADE_MSG_OPPOSITE_END_TRADE:
		Player[CLIENT_PLAYER_INDEX].tradeClientReciveEndTrade(false);
		break;
	case KTrade::TRADE_MSG_OK:
		Player[CLIENT_PLAYER_INDEX].tradeClientReciveTradeOk();
		break;
	case KTrade::TRADE_MSG_CANCEL:
		Player[CLIENT_PLAYER_INDEX].tradeClientReciveTradeCancel();
		break;
	}
}

void	KProtocolProcess::s2cTeamInviteAdd(BYTE * pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_cTeam.ReceiveInvite((TEAM_INVITE_ADD_SYNC*)pMsg);
}

// Add by Cooler 2004-8-11
// Begin -->
void KProtocolProcess::s2cCheckStoragePswOK(BYTE* pMsg)
{
	PREQUESTREPLY pReply = (PREQUESTREPLY)pMsg;
	CoreDataChanged(GDCNI_STORE_BOX_UNLOCK_RESULT, pReply->byReply, 0);
}

void KProtocolProcess::s2cCreateStoragePswOK(BYTE* pMsg)
{
	PREQUESTREPLY pReply = (PREQUESTREPLY)pMsg;
	CoreDataChanged(GDCNI_STORE_BOX_CHANGE_PASSWORD_RESULT, pReply->byReply, 0);
}

void KProtocolProcess::s2cModifyStoragePswOK(BYTE* pMsg)
{
	PREQUESTREPLY pReply = (PREQUESTREPLY)pMsg;
	CoreDataChanged(GDCNI_STORE_BOX_CHANGE_PASSWORD_RESULT, pReply->byReply, 0);
}

// lixuewu  增加召唤兽同步
void KProtocolProcess::s2cCreatureSync(BYTE *pMsg)
{
	const CREATURE_SYNC* pSync = (const CREATURE_SYNC*)pMsg;

	KCreature& aCreature = Player[CLIENT_PLAYER_INDEX].m_Creature;
	if (pSync->wSkillID != 0)
	{
		aCreature.m_nSkillID = pSync->wSkillID;

		// add by chenshanglin on 2006-2-21 for new skill system
		KNpc *pNpc = aCreature.GetCreatureNpc();

		if(pNpc)
		{
			pNpc->m_CompAttrMgr.Set(ncai_lifeuplimit, idx_base_value, pSync->wMaxLife);
			pNpc->m_UnaryAttrMgr.Set(nuai_curlife, pSync->wCurrentLife);
			pNpc->m_ActiveSkillID = pSync->wCreatureSkillID;
		}
		// add end
	}
	else
	{
		aCreature.Dismiss();		
	}
}

void KProtocolProcess::s2cSyncTeamMemberInfo(BYTE* pMsg)
{
	TEAMMATE_INFO* pTeammateInfo = (TEAMMATE_INFO*)pMsg;
	GetClientTeam().SetMemberInfo(*pTeammateInfo);
	GetClientPlayer().GetTeamInfo().UpdateInterface();
}

void KProtocolProcess::s2cTradeRequest(BYTE* pMsg)
{
	if (!pMsg)
		return;
	TRADE_APPLY_START_SYNC	*pApply = (TRADE_APPLY_START_SYNC*)pMsg;
	Player[CLIENT_PLAYER_INDEX].tradeClientReciveRequest(pApply->oppositePlayerNpcId);
}

extern IClientCallback* l_pDataChangedNotifyFunc;

void KProtocolProcess::s2cExtend(BYTE* pMsg)
{
}

void KProtocolProcess::s2cExtendChat(BYTE* pMsg)
{
}

static BOOL sParseUGName(const std::string& name, std::string* pUnit, std::string* pGroup)
{
	static const char char_split = '\n';

	size_t pos = name.find(char_split);
	if (pos == name.npos)
	{
		if (pUnit)
			pUnit->resize(0);
		if (pGroup)
			pGroup->assign(name);
	}
	else
	{
		std::string::const_iterator itSplit = name.begin() + pos;

		if (pUnit)
			pUnit->assign(name.begin(), itSplit);
		if (pGroup)
			pGroup->assign(itSplit + 1, name.end());
	}

	return TRUE;
}

void KProtocolProcess::s2cExtendFriend(BYTE* pMsg)
{
}

void KProtocolProcess::s2cWorldCombatInfo(BYTE* pMsg)
{
	WORLD_COMBAT_INFO * pInfo = (WORLD_COMBAT_INFO *)pMsg;
	
	int nPlayerNpcIdx=GetClientPlayer().GetNpcIndex();
	if (IsValidNpc(nPlayerNpcIdx) && Npc[nPlayerNpcIdx].IsInWorldCombatInstance())
	{
		WorldCombatUIParam  uiParam;
		memset(&uiParam,0,sizeof(uiParam));
		
		for (int n = 0; n < MAX_SCORE_ORG_SYNC && n < MAX_WORLD_UI_ORG ; n ++)
		{
			if ( n < MAX_COMBAT_ORG_NUM && SubWorld[0].m_WorldCombatClientInfo[n].szOrgName[0]!=0)
			{
				uiParam.detail[n].nScore   = pInfo->nScore[n];
				uiParam.detail[n].baseInfo = SubWorld[0].m_WorldCombatClientInfo[n];
				uiParam.nInfoNum ++;
			}
			else
				break;

		}//end for n

		int CombatScoreCalcType = SubWorld[0].GetCombatScoreCalcType();

		switch(CombatScoreCalcType)
		{
		case NO_CALU:
			break;
		case PROGRAME_CALU:
		case SCRIPT_CALU:
			CoreDataChanged(GDCNI_RECV_WORLD_COMBAT_SCORE, (int)&uiParam, 1);
			break;
		}
		
	}//endif

}

void    KProtocolProcess::s2cPlayerStop(BYTE * pMsg)
{
     int nPlayerIdx=GetClientPlayer().GetNpcIndex();
	 
	 Npc[nPlayerIdx].SendCommand(do_stand);
     Npc[nPlayerIdx].SendC2SPosSync();
}

void    KProtocolProcess::s2cPosEdition(BYTE * pMsg)
{
     S2C_POS_EDITION * Pro=(S2C_POS_EDITION *) pMsg;
	 int nNpcID=Pro->nNpcID;
     int index=NpcSet.SearchID(nNpcID);
     
	 if (index && index!=GetClientPlayer().GetNpcIndex() && Npc[index].IsValid())
	 {
		 Npc[index].SendCommand(do_run,Pro->nX,Pro->nY);
		 Npc[index].BeginEditionState();
	 }//endif

}

void    KProtocolProcess::s2cFurySync(BYTE * pMsg)
{
	KClientFuryMgr::Singlton().ProcessMsg(pMsg);
}

void KProtocolProcess::s2cApplyJoinTeam(BYTE* pMsg)
{
	GetClientPlayer().GetTeamInfo().ReceiveApply((TEAM_APPLY_JOIN*)pMsg);
}

void	KProtocolProcess::s2cViewEquip(BYTE* pMsg)
{
	g_cViewItem.GetData(pMsg);
}

//add by zuolizhi for pic question
void KProtocolProcess::s2cByteExtend(BYTE* pMsg)
{
	int nNpcIdx = Player[CLIENT_PLAYER_INDEX].GetNpcIndex();
	if ( nNpcIdx <= 0 )
	{
		return;
	}

	PBYTE_EXTEND_HEADER pExtHeader = (PBYTE_EXTEND_HEADER)pMsg;

	switch(pExtHeader->ProtocolExtend)
	{
	case s2c_ex_protocol_play_animation:
		{
			if (pMsg == NULL)
			{
				break;
			}

			Play_Animation * pData = (Play_Animation *)pMsg;

			CoreDataChanged(GDCNI_PLAY_ANIMATION, (unsigned int)pData, 0);
		}
		break;
	case s2c_ex_protocol_changemap:
		{
			SEND_CHANGEMAP* pChangeMap = (SEND_CHANGEMAP*)pMsg;

			CoreDataChanged(GDCNI_GAME_PRE_LOAD_MAP, pChangeMap->bShowElf, pChangeMap->nMap);
			CoreDataChanged(GDCNI_GAME_END_LOAD_PROGRESS, NULL, NULL);
		}
		break;
	case s2c_ex_protocol_sendquestion:
		{
			PSEND_QUESTION pSendQuestion = (PSEND_QUESTION)pMsg;
			
			if( pSendQuestion->wOffsetData == 0 )
				Player[CLIENT_PLAYER_INDEX].m_nQuestionClientLen = 0;

			/*
			if( pSendQuestion->wOffsetData != 
				Player[CLIENT_PLAYER_INDEX].m_nQuestionClientLen )
			{
				//长度不对,重新发
				_AnswerQuestion.dwAnswer = 0xFFFFFFFF;
				_AnswerQuestion.Protocol = c2s_byte_extend;
				_AnswerQuestion.ProtocolExtend = c2s_ex_protocol_answerquestion;
				_AnswerQuestion.dwAnswer = 1;
				_AnswerQuestion.wProtocolSize = sizeof( ANSWER_QUESTION ) - 1;
				
				g_pClient->SendPackToServer(
					&_AnswerQuestion, 
					sizeof( ANSWER_QUESTION ));		
			}
			else*/
			{
				//长度没有问题
				memcpy( 
					Player[CLIENT_PLAYER_INDEX].szQuestionBuffer + 
					Player[CLIENT_PLAYER_INDEX].m_nQuestionClientLen,
					pMsg + sizeof(SEND_QUESTION),
					pSendQuestion->wQuestionLen);
				
				Player[CLIENT_PLAYER_INDEX].m_nQuestionClientLen += 
					pSendQuestion->wQuestionLen;
			}

			break;
		}
	case s2c_ex_protocol_askquestion:
		{
			
			char				szOutBuffer[QUESTIONSIZE];
			unsigned int		nDecompressLen = 0;
			PBITMAPFILEHEADER	BMPFileHeader = NULL;
			PBITMAPINFOHEADER	BMPInfoHeader = NULL;
			PBYTE				pImageData = NULL;
			tagCertifyParam		_CertifyParam;

			PASK_QUESTION pAskQuestion = (PASK_QUESTION)pMsg;

			if( pAskQuestion->wCompress == TRUE )
			{
				lzo1x_decompress(
					(const unsigned char *)Player[CLIENT_PLAYER_INDEX].szQuestionBuffer,
					Player[CLIENT_PLAYER_INDEX].m_nQuestionClientLen,
					(unsigned char *)szOutBuffer,
					&nDecompressLen,
					NULL);

				BMPFileHeader = 
				(PBITMAPFILEHEADER)szOutBuffer;
				
				pImageData = 
				(PBYTE)(szOutBuffer + 
					BMPFileHeader->bfOffBits);
				
				BMPInfoHeader = 
				(PBITMAPINFOHEADER)(szOutBuffer + 
					sizeof(BITMAPFILEHEADER));
			}
			else
			{
				BMPFileHeader = 
				(PBITMAPFILEHEADER)Player[CLIENT_PLAYER_INDEX].szQuestionBuffer;
				
				pImageData = 
				(PBYTE)(Player[CLIENT_PLAYER_INDEX].szQuestionBuffer + 
					BMPFileHeader->bfOffBits);
				
				BMPInfoHeader = 
				(PBITMAPINFOHEADER)(Player[CLIENT_PLAYER_INDEX].szQuestionBuffer + 
					sizeof(BITMAPFILEHEADER));
			}

			if( pAskQuestion->byQType == question_type_pic )
			{
				memcpy(
					Player[CLIENT_PLAYER_INDEX].szAnswerSet,
					pAskQuestion->szAnswerSet,
					pExtHeader->wProtocolSize + 2 - sizeof(ASK_QUESTION) );
				
				memcpy(
					Player[CLIENT_PLAYER_INDEX].szQuestionDescripte,
					pAskQuestion->szQuestionDescripte,
					100);
			}

			//====================================
			
			_CertifyParam.byType		=	pAskQuestion->byQType;
			_CertifyParam.nWidth		=	BMPInfoHeader->biWidth;
			_CertifyParam.nHeight		=	BMPInfoHeader->biHeight;
			_CertifyParam.pImageData	=	pImageData;
			_CertifyParam.pAnswerSet	=	Player[CLIENT_PLAYER_INDEX].szAnswerSet;
			_CertifyParam.szQuestion	=	Player[CLIENT_PLAYER_INDEX].szQuestionDescripte;

//			CoreDataChanged(GDCNI_OPEN_CERTIFY,(unsigned int)&_CertifyParam,0);

			break;
		}
	case s2c_ex_protocol_nomove:
		{
			PNPCNOMOVE pNoMove = (PNPCNOMOVE)pMsg;
			int nIdx = NpcSet.SearchID( pNoMove->dwID );
			Npc[nIdx].m_UnaryAttrMgr.Set( nuai_nomove, pNoMove->nEnable );
		}
		break;
	case s2c_ex_protocol_shortcut_add:
		{
			_ShortCut_Add* pAdd = (_ShortCut_Add*)pMsg;

			KImmediacyParam	IP = {0};
			IP.nID	= pAdd->dwID;
			IP.nPos = pAdd->dwPos;
			IP.nImmediacyType = pAdd->dwSCType;
			
			CoreDataChanged( GDCNI_ADD_IMMEDIACY, (unsigned int)&IP , 0 );
		
			// LSL
			KItemGroupCD_C tagCD_C;
			tagCD_C.eType		= (_immediacy_type)IP.nImmediacyType;
			if ( tagCD_C.eType == skill_immediacy_type )
			{
				IP.nID = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(IP.nID);
				int index = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().FindSkill(IP.nID);
				KSkill* pSkill = g_SkillManager.GetSkill( IP.nID );
				if ( pSkill )
				{
					tagCD_C.Id			= IP.nID;
					tagCD_C.nGroup		= pSkill->GetGroup();
					tagCD_C.ulCDTime	= Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().GetLeftCDTimeByIdx(index);
				}

			}
			else
			{
				return;
			}
			
			if ( !g_ProtocolSimulationSet.IsExisting( itemgroupcd_dataset ) )
			{
				g_ProtocolSimulationSet.registerSimulation( itemgroupcd_dataset, new KItemGroupCDSimulation( itemgroupcd_dataset ) );
			}
			KItemGroupCDSimulation* pGroupCDSimulation = (KItemGroupCDSimulation*)g_ProtocolSimulationSet.getSimulation( itemgroupcd_dataset );
			if ( pGroupCDSimulation )
			{
				pGroupCDSimulation->AddGroupCD( tagCD_C );
			}
		}
		break;
	case s2c_ex_protocol_additemgroupcd:
		{
			_ItemGroupCD_Add* pAdd = (_ItemGroupCD_Add*)pMsg;
			KItemGroupCD_C tagCD_C;
			tagCD_C.nGroup		= pAdd->nGroup;
			tagCD_C.ulCDTime	= pAdd->ulCDTime;
			tagCD_C.eType		= item_immediacy_type;
			if ( !g_ProtocolSimulationSet.IsExisting( itemgroupcd_dataset ) )
			{
				g_ProtocolSimulationSet.registerSimulation( itemgroupcd_dataset, new KItemGroupCDSimulation( itemgroupcd_dataset ) );
			}
			KItemGroupCDSimulation* pGroupCDSimulation = (KItemGroupCDSimulation*)g_ProtocolSimulationSet.getSimulation( itemgroupcd_dataset );
			if ( pGroupCDSimulation )
			{
				pGroupCDSimulation->AddGroupCD( tagCD_C );
			}
			//CoreDataChanged( GDCNI_BEGIN_GROUP_CD, (unsigned int)&tagCD_C, NULL );
		}
		break;
	case s2c_ex_protocol_delitemgroupcd:
		{
			_ItemGroupCD_Del* pDel = (_ItemGroupCD_Del*)pMsg;
			KItemGroupCD_C tagCD_C;
			tagCD_C.nGroup		= pDel->nGroup;
			tagCD_C.ulCDTime	= 0;
			if ( !g_ProtocolSimulationSet.IsExisting( itemgroupcd_dataset ) )
			{
				g_ProtocolSimulationSet.registerSimulation( itemgroupcd_dataset, new KItemGroupCDSimulation( itemgroupcd_dataset ) );
			}
			KItemGroupCDSimulation* pGroupCDSimulation = (KItemGroupCDSimulation*)g_ProtocolSimulationSet.getSimulation( itemgroupcd_dataset );
			if ( pGroupCDSimulation )
			{
				pGroupCDSimulation->DelGroupCD( tagCD_C );
			}
			//CoreDataChanged( GDCNI_END_GROUP_CD, (unsigned int)&tagCD_C, NULL );
		}
		break;
	case s2c_ex_protocol_question:
		{
			UIQuestionData uiQuestionData;
			if (ParseQuestionProtocol(pMsg, uiQuestionData))
			{
				CoreDataChanged(GDCNI_ASK_QUESTION, (unsigned int)&uiQuestionData, NULL);
			}

// 			QUESTION* pQuestion = (QUESTION*)pMsg;
// 			
// 			char realQuestion[COMMON_QUESTION_BUFF_SIZE];
// 			memset(realQuestion, 0, sizeof(realQuestion));
// 			
// 			int questionDataLength = pQuestion->wProtocolSize - (sizeof(QUESTION) - sizeof(pQuestion->QuestionData) - 1);
// 			
// 			if (TRUE == pQuestion->IsCompressed)
// 			{
// 				const BYTE* pCompressedBuff = (BYTE*)pQuestion->QuestionData;
// 				unsigned int compressedBuffLength = questionDataLength;
// 				BYTE* pDecompressBuff = (BYTE*)realQuestion;
// 				unsigned int decompressBuffLength = sizeof(realQuestion);
// 				lzo1x_decompress(
// 					pCompressedBuff,
// 					compressedBuffLength,
// 					pDecompressBuff,
// 					&decompressBuffLength,
// 					NULL);
// 
// 				questionDataLength = decompressBuffLength;
// 			}
// 			else
// 			{
// 				memcpy(realQuestion, pQuestion->QuestionData, questionDataLength);
// 			}
// 
// 			UIQuestionData uiQuestionData;
// 			memset(&uiQuestionData, 0, sizeof(uiQuestionData));
// 
// 			char* pQuestionData = realQuestion;
// 			int leftQuestionBuffSize = questionDataLength;
// 			
// 			if (leftQuestionBuffSize < sizeof(WORD))
// 				return;
// 			WORD* pImgLength = (WORD*)(pQuestionData);
// 			pQuestionData += sizeof(WORD);
// 			leftQuestionBuffSize -= sizeof(WORD);
// 			
// 			if (leftQuestionBuffSize < *pImgLength || sizeof(uiQuestionData.ImgData) < *pImgLength)
// 				return;
// 			memcpy(uiQuestionData.ImgData, pQuestionData, *pImgLength);
// 			uiQuestionData.ImgDataLength = *pImgLength;
// 			pQuestionData += *pImgLength;
// 			leftQuestionBuffSize -= *pImgLength;
// 			
// 			if (leftQuestionBuffSize < sizeof(WORD))
// 				return;
// 			WORD* pQuestionTextLength = (WORD*)pQuestionData;
// 			pQuestionData += sizeof(WORD);
// 			leftQuestionBuffSize -= sizeof(WORD);
// 			
// 			if (leftQuestionBuffSize < *pQuestionTextLength || sizeof(uiQuestionData.TextData) < *pQuestionTextLength)
// 				return;
// 			memcpy(uiQuestionData.TextData, pQuestionData, *pQuestionTextLength);
// 			uiQuestionData.TextDataLength = *pQuestionTextLength;
// 			pQuestionData += *pQuestionTextLength;
// 			leftQuestionBuffSize -= *pQuestionTextLength;
//
//			CoreDataChanged(GDCNI_ASK_QUESTION, (unsigned int)&uiQuestionData, pQuestion->Timeout);
		}
		break;

	case s2c_ex_protocol_combat_result_org_2:
		{
			COMBAT_MAP_RESULT_ORG_2 * pWorldCombatResult = (COMBAT_MAP_RESULT_ORG_2 *) pMsg;
			int npcIndex = GetClientPlayer().GetNpcIndex();
			if ( IsValidNpc( npcIndex ) && ( NULL != pWorldCombatResult ))
			{
				const int selfCampID = Npc[npcIndex].m_WorldCombatOrg;

				if ( IsValidCombatID( selfCampID ) )
				{
					SMALL_BATTLE_FIELD_RESULT curResult;
					curResult.CombatMapTemplateID	= pWorldCombatResult->wCombatMapTemplateID;
					curResult.PersistTime			= pWorldCombatResult->dwPersistTime;
					curResult.Repute				= pWorldCombatResult->nSelfScoreGet;

					if ( selfCampID == pWorldCombatResult->nOrg[0] )
					{
						curResult.SelfScore		= pWorldCombatResult->nScore[0];
						curResult.EnemyScore	= pWorldCombatResult->nScore[1];
					}
					else if ( selfCampID == pWorldCombatResult->nOrg[1] )
					{
						curResult.SelfScore		= pWorldCombatResult->nScore[1];
						curResult.EnemyScore	= pWorldCombatResult->nScore[0];
					}

					CoreDataChanged( 
						GDCNI_UPDATE_SMALL_BATTLE_FIELD_RESULT,  
						reinterpret_cast< unsigned int >( &curResult ), NULL );
				}
			}
		}//end for case
		break;

	case s2c_ex_protocol_exp_insurance:
		{
			S2C_EXP_INSRUANCE * rewardCacheNotify = ( S2C_EXP_INSRUANCE * ) pMsg;

			switch (rewardCacheNotify->SubProtocol)
			{
			case s2c_exp_insurance_sync_state:
				{
					GetClientPlayer().m_IsExpInsuraceValid = rewardCacheNotify->Data?true:false;
                    
                    #ifdef _DEBUG
					char   szDebugString[256] = "";
					sprintf(szDebugString,"CurrentExpInsuranceState:%d\n",rewardCacheNotify->Data);
					OutputDebugString(szDebugString);
                    #endif

					CoreDataChanged(GDCNI_EXP_INSURANCE_STATE_NOTIFY, rewardCacheNotify->Data, 0);
				}
				break;

			case s2c_exp_insurance_sync_reward:
				{
					GetClientPlayer().m_CurrentExpReward = rewardCacheNotify->Data;
                    #ifdef _DEBUG
					char   szDebugString[256] = "";
					sprintf(szDebugString,"CurrentExpReward:%d\n",rewardCacheNotify->Data);
					OutputDebugString(szDebugString);
                    #endif

					CoreDataChanged(GDCNI_EXP_INSURANCE_REWARD_NOTIFY, rewardCacheNotify->Data, 0);
				}
				break;

			case s2c_exp_insurance_fetch_reward:
				{
                    #ifdef _DEBUG
					char   szDebugString[256] = "";
					sprintf(szDebugString,"FetchExpReward:%d\n",rewardCacheNotify->Data);
					OutputDebugString(szDebugString);
                    #endif

					CoreDataChanged(GDCNI_EXP_INSURANCE_REWARD, rewardCacheNotify->Data, 0);

				}
				break;

			case s2c_exp_insurance_enter_notify:
				{
                    #ifdef _DEBUG
					char   szDebugString[256] = "";
					sprintf(szDebugString,"Enter Exp Insurance State!\n");
					OutputDebugString(szDebugString);
                    #endif

					CoreDataChanged(GDCNI_EXP_INSURANCE_ENTER_STATE, rewardCacheNotify->Data, 0);
				}
				break;

			}//end switch

		}
		break;


	case s2c_ex_protocol_quest_insurnace:
		{
			S2C_QUEST_INSURANCE * rewardCacheNotify = ( S2C_QUEST_INSURANCE * ) pMsg;
			
			switch (rewardCacheNotify->SubProtocol)
			{
			case s2c_quest_insurance_sync_state:
				{
					GetClientPlayer().m_IsQuestInsuranceValid = rewardCacheNotify->Data?true:false;
                    
                    #ifdef _DEBUG
					char   szDebugString[256] = "";
					sprintf(szDebugString,"CurrentQuestInsuranceState:%d\n",rewardCacheNotify->Data);
					OutputDebugString(szDebugString);
                    #endif
					
					CoreDataChanged(GDCNI_QUEST_INSURANCE_STATE_NOTIFY, rewardCacheNotify->Data, 0);
				}
				break;
				
			case s2c_quest_insurance_sync_reward:
				{
					GetClientPlayer().m_CurrentQuestReward = rewardCacheNotify->Data;
                    #ifdef _DEBUG
					char   szDebugString[256] = "";
					sprintf(szDebugString,"CurrentQuestReward:%d\n",rewardCacheNotify->Data);
					OutputDebugString(szDebugString);
                    #endif

					CoreDataChanged(GDCNI_QUEST_INSURANCE_REWARD_NOTIFY, rewardCacheNotify->Data, 0);
				}
				break;
				
			case s2c_quest_insurance_fetch_reward:
				{
                    #ifdef _DEBUG
					char   szDebugString[256] = "";
					sprintf(szDebugString,"FetchQuestReward:%d\n",rewardCacheNotify->Data);
					OutputDebugString(szDebugString);
                    #endif
					
					CoreDataChanged(GDCNI_QUEST_INSURANCE_REWARD, rewardCacheNotify->Data, 0);
			
				}
				break;
				
			case s2c_quest_insurance_enter_notify:
				{
                    #ifdef _DEBUG
					char   szDebugString[256] = "";
					sprintf(szDebugString,"Enter Quest Insurance State!\n");
					OutputDebugString(szDebugString);
                    #endif
					
					CoreDataChanged(GDCNI_QUEST_INSURANCE_ENTER_STATE, rewardCacheNotify->Data, 0);
						
				}
				break;
				
			}//end switch
			
		}
		break;

	case s2c_ex_protocol_statue_info:
		{
			_StatueInfo* pStatueInfo = (_StatueInfo* )pMsg;
			UIStatueInfo Temp;
			
			CreateQueryManager(NULL);
			MapsInfo & mapinfo = MapsInfo::getSingleton();
			
			if ((int)pStatueInfo->nMapId != INVALID_WORLD_ID)
			{
				MapsTab mapTab;
				mapinfo.GetMapInfo(pStatueInfo->nMapId, mapTab);	
				if (mapTab.name.length())
				{
					strncpy(Temp.cMapName, mapTab.name.c_str(), CLIENT_NAME_AND_TITLE_MAX + 1);  
					Temp.cMapName[CLIENT_NAME_AND_TITLE_MAX] = 0;
				}
				
				if (!pStatueInfo->cName)
					return;
				
				strncpy(Temp.cName, pStatueInfo->cName, CLIENT_NAME_AND_TITLE_MAX + 1);
				Temp.cName[CLIENT_NAME_AND_TITLE_MAX] = 0;
				
				if (!pStatueInfo->cTongName)
					return;
				
				strncpy(Temp.cTongName, pStatueInfo->cTongName, CLIENT_NAME_AND_TITLE_MAX + 1);
				Temp.cTongName[CLIENT_NAME_AND_TITLE_MAX] = 0;

				Temp.dwtime = pStatueInfo->dwtime;
				Temp.hasBuff = pStatueInfo->HasBuff;
				Temp.nMapId  = pStatueInfo->nMapId;
				CoreDataChanged(GDCNI_STATUE_INFO, (unsigned int)&Temp, 0);
				
			}//endif	
		}
		break;
		
	default:
		;
	}
}

#include "KNpcTemplate.h"
void KProtocolProcess::s2cPetProtocol(BYTE* pMsg)
{
}
//<------- End [Ray]

void KProtocolProcess::s2cFindPathSync(BYTE* pMsg)
{
	PFINDPATHSYNC pSyncData = (PFINDPATHSYNC)pMsg;

	int nNpcIdx = NpcSet.SearchID(pSyncData->dwID);
	if(nNpcIdx > 0)
	{
		int nMpsXX, nMpsYY;
		Npc[nNpcIdx].GetMpsPos(&nMpsXX, &nMpsYY);

		if(!pSyncData->byForce)
		{
			if((pSyncData->nPosX - nMpsXX) * (pSyncData->nPosX - nMpsXX) + 
				(pSyncData->nPosY - nMpsYY) * (pSyncData->nPosY - nMpsYY) < 50 * 50)
			{
				return;
			}
		}

		int nRegion, nMapX, nMapY, nOffX, nOffY;
		SubWorld[0].Mps2Map(pSyncData->nPosX, pSyncData->nPosY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

		if(nRegion >= 0)
		{
			Npc[nNpcIdx].MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);
		}
	}
}

int KProtocolProcess::SyncItem(const ITEM_SYNC * pItemSync)
{	
	//判断物品的位置上是否已经有个物品了，如果有则删除它
	int itemIndex;
	if(pItemSync->m_btPlace == pos_equip)
	{
		itemIndex = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_EquipItem[pItemSync->m_btX].nEquipIdx;
	}
	else
	{
		INVENTORY_ROOM room = KItemList::corePos2coreRoom((ITEM_POSITION)pItemSync->m_btPlace);
		itemIndex = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[room].FindItem(pItemSync->m_btX, pItemSync->m_btY);
	}

	if(itemIndex > 0)
	{
		if(pItemSync->m_btPlace == pos_trade1)
		{
			char msg[COMMON_CLIENT_MSG_LEN_512];
			char formatString[COMMON_CLIENT_MSG_LEN_256 + 1];
			g_GetStringRes(sid_opposite_remove_trade_item, formatString, 256);
			sprintf(msg, formatString, Item[itemIndex].GetName());
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (UINT)msg, 0);
			CoreDataChanged(GDCNI_TRADE_OPPOSITE_PICKUP_ITEM, 1, itemIndex);
		}
		Player[CLIENT_PLAYER_INDEX].m_ItemList.Remove(itemIndex);
	}

	//如果是删除物品
	if( NULL == pItemSync || pItemSync->m_ID <= 0)
	{
		return 0;
	}
	
	//在itemset中添加该物品并取得物品index
	itemIndex = ItemSet.Add(		
		pItemSync->m_Genre,
		pItemSync->m_Detail,
		pItemSync->m_Particur,		
		pItemSync->m_Level,
		pItemSync->m_btItemCount);
	
	if (itemIndex <= 0)
		return 0;
	
	//按照服务器端发过来的配置信息更新该物品的属性
	Item[itemIndex].SetID(pItemSync->m_ID);
	
	Item[itemIndex].SetMaxDurability(pItemSync->m_MaxDurability);
	Item[itemIndex].SetDurability(pItemSync->m_Durability);
	Item[itemIndex].SetLevelupTimes(pItemSync->m_LevelupTimes);	
	Item[itemIndex].SetLevelupType(pItemSync->m_nLevelupType);
	Item[itemIndex].SetYaoID(pItemSync->m_YaoID);
	for (int yaoAddonBuffLoopCount = 0; yaoAddonBuffLoopCount < YAO_ADDON_BUFF_COUNT; yaoAddonBuffLoopCount++)
	{
		Item[itemIndex].SetYaoAddOn(yaoAddonBuffLoopCount, pItemSync->m_YaoAddOnBuffSet[yaoAddonBuffLoopCount]);
	}	
	Item[itemIndex].SetCompBuffTemplateSet( pItemSync->m_compBuffTemplateSet, sizeof( WORD ) * COMPOUND_COUNT );
	Item[itemIndex].SetPlusInfo( (char*)pItemSync->m_szPlusInfo );
	Item[itemIndex].SetTalismanPotential(pItemSync->m_TalismanPotential);
	for (int talismanEnchaseLoopCount = 0; talismanEnchaseLoopCount < TM_HOLE_NUM; talismanEnchaseLoopCount++)
	{
		Item[itemIndex].SetTalismanEnchase(talismanEnchaseLoopCount, pItemSync->m_TalismanEnchaseSet[talismanEnchaseLoopCount]);
	}	
	Item[itemIndex].SetBind( pItemSync->m_IsBind );
	Item[itemIndex].SetLockCount( pItemSync->m_LockCount );

	Item[itemIndex].ClearSocketSet();
	for ( int nSocketIdx = 0; nSocketIdx < MAX_INLAY_COUNT; ++nSocketIdx )
	{
		InlayStuff stuff;
		if ( pItemSync->m_socketSet[nSocketIdx].nGenre == -1 &&
			pItemSync->m_socketSet[nSocketIdx].nDetail == -1 &&
			pItemSync->m_socketSet[nSocketIdx].nParticular == -1 &&
			pItemSync->m_socketSet[nSocketIdx].nLevel == -1 )
		{
			continue;
		}
		else
		{

			stuff.nGenre		= pItemSync->m_socketSet[nSocketIdx].nGenre;
			stuff.nDetail		= pItemSync->m_socketSet[nSocketIdx].nDetail;
			stuff.nParticular	= pItemSync->m_socketSet[nSocketIdx].nParticular;
			stuff.nLevel		= pItemSync->m_socketSet[nSocketIdx].nLevel;
			if ( stuff.nGenre == 0 && 
				stuff.nDetail == 0 &&
				stuff.nParticular == 0 && 
				stuff.nLevel == 0 )
			{
				Item[itemIndex].CreateSocket();
			}
			else
			{
				Item[itemIndex].SetSocketSet( stuff );
			}
			
		}
	}
	Item[itemIndex].SetInlayBaseBuffSet((short *)pItemSync->m_InlayBaseBuffSet );
	Item[itemIndex].SetInlayYaoBuffSet((short *)pItemSync->m_InlayYaoBuffSet );
	Item[itemIndex].SetInlaySpecialBuffSet((short *)pItemSync->m_InlaySpecialBuffSet );
	Item[itemIndex].SetIBBuyDate( pItemSync->m_dwIBBuyTime );
	Item[itemIndex].SetIBUseCount(pItemSync->m_btItemCount);

	Item[itemIndex].SetCreditFlag( pItemSync->m_CreditFlag );
	Item[itemIndex].SetPosInfo(pItemSync->m_MapID, pItemSync->m_MapX, pItemSync->m_MapY );
	Item[itemIndex].SetStep( pItemSync->m_Step );


	KItemList& itemList = Player[CLIENT_PLAYER_INDEX].GetItemList();
	int listIndex = itemList.Add(itemIndex, pItemSync->m_btPlace, pItemSync->m_btX, pItemSync->m_btY, NULL, (enumItemSyncType)pItemSync->SyncType);
	if (listIndex <= 0)
	{
		ItemSet.Remove(itemIndex);
		return listIndex;
	}
	
	if(pItemSync->m_btPlace == pos_trade1)
	{
		CoreDataChanged(GDCNI_TRADE_OPPOSITE_PICKUP_ITEM, 0, itemIndex);
	}
	return listIndex;
}

#else

void KProtocolProcess::NpcRequestCommand(int nIndex, BYTE* pProtocol, int nSize)
{
	NPC_REQUEST_COMMAND *pNpcRequestSync = (NPC_REQUEST_COMMAND *)pProtocol;
	NpcSet.SyncNpc(pNpcRequestSync->ID, nIndex);
}

//-------------------------------------------------------------------------
//	功能：客户端向服务器请求更新某个obj数据
//-------------------------------------------------------------------------
void KProtocolProcess::ObjRequestCommand(int nIndex, BYTE* pProtocol, int nSize)
{
	OBJ_CLIENT_SYNC_ADD	*pObjClientSyncAdd = (OBJ_CLIENT_SYNC_ADD*)pProtocol;
	ObjSet.SyncAdd(pObjClientSyncAdd->m_nID, nIndex);
}

void KProtocolProcess::NpcRunCommand(int nIndex, BYTE* pProtocol, int nSize)
{
	if (Player[nIndex].IsBlockClientControl())
		return;

	NPC_RUN_COMMAND* pNetCommand = (NPC_RUN_COMMAND *)pProtocol;
	int ParamX = pNetCommand->nMpsX;
	int ParamY = pNetCommand->nMpsY;
	int ParamCurX = pNetCommand->nCurMpsX;
	int ParamCurY = pNetCommand->nCurMpsY;
    
	if (ParamX < 0)
	{
		ParamX = 0;
	}
	if (ParamY < 0)
	{
		ParamY = 0;
	}

	int nIdx = Player[nIndex].m_nIndex;

	if (nIdx > 0 && nIdx < MAX_NPC)
	{
       
		if( Npc[nIdx].m_UnaryAttrMgr[nuai_nomove] )
			return;

		//Edit server pos by client pos
	/*	if (Npc[nIdx].CheckClientRunPos(ParamCurX,ParamCurY,ParamX,ParamY))
		{
            Npc[nIdx].SetPosDirectly(ParamCurX,ParamCurY);
		}//endif
    */

		Npc[nIdx].SendCommand(do_run, ParamX, ParamY);
	}
}


void KProtocolProcess::NpcSkillCommand(int nIndex, BYTE* pProtocol, int nSize)
{
	if (Player[nIndex].IsBlockClientControl())
		return;

	NPC_SKILL_COMMAND* pNetCommand = (NPC_SKILL_COMMAND *)pProtocol;
	CastSkillParam skillParam;
	skillParam.SkillId = pNetCommand->nSkillID;
	skillParam.Param1 = pNetCommand->nMpsX;
	skillParam.Param2 = pNetCommand->nMpsY;

	if (skillParam.SkillId <= 0 || skillParam.SkillId > MAX_SKILL || skillParam.Param2 < 0)
		return ;

	KPlayer& player = Player[nIndex];
	if (skillParam.Param1 == SKILL_SPT_TargetIndex)
	{
		int targetNpcIndex = player.FindAroundNpc((DWORD)skillParam.Param2);
		if (targetNpcIndex > 0)
		{
			skillParam.Param2 = targetNpcIndex;
			player.SetNextSkill(skillParam);
		}
		else
		{
			// 既然攻击的NPC已经不存在了，那就告诉客户端删除掉
			NPC_REMOVE_SYNC	RemoveSync;
			RemoveSync.ProtocolType = s2c_npcremove;
			RemoveSync.ID = (DWORD)skillParam.Param2;
			if (g_pServer != NULL)
				g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &RemoveSync, sizeof(NPC_REMOVE_SYNC));
		}
	}
	else
	{
		player.SetNextSkill(skillParam);
	}
}

void KProtocolProcess::PlayerApplyTeamInfo(int nIndex, BYTE* pProtocol, int nSize)
{
	PLAYER_APPLY_TEAM_INFO	*pApplyTeamInfo = (PLAYER_APPLY_TEAM_INFO*)pProtocol;
	int npcIndex = NpcSet.SearchID(pApplyTeamInfo->m_dwTarNpcID);
	if (IsValidNpc(npcIndex))
	{
		int requestPlayerIndex = Npc[npcIndex].GetPlayerIdx();
		Player[nIndex].GetTeamInfo().RequestTeamInfo(requestPlayerIndex);
	}
}

void KProtocolProcess::PlayerApplyCreateTeam(int nIndex, BYTE* pProtocol, int nSize)
{
	Player[nIndex].GetTeamInfo().CreateTeam();
}

void KProtocolProcess::PlayerApplyLeaveTeam(int nIndex, BYTE* pProtocol, int nSize)
{
	Player[nIndex].GetTeamInfo().LeaveTeam();
}

void KProtocolProcess::PlayerApplyTeamKickMember(int nIndex, BYTE* pProtocol, int nSize)
{
	PLAYER_TEAM_KICK_MEMBER	*pKickMember = (PLAYER_TEAM_KICK_MEMBER*)pProtocol;
	int npcIndex = NpcSet.SearchID(pKickMember->m_dwNpcID);
	if (npcIndex > 0)
	{
		int kickMemberPlayerIndex = Npc[npcIndex].GetPlayerIdx();
		Player[nIndex].GetTeamInfo().KickMember(kickMemberPlayerIndex);
	}
}

void KProtocolProcess::PlayerApplyTeamChangeCaptain(int nIndex, BYTE* pProtocol, int nSize)
{
	PLAYER_APPLY_TEAM_CHANGE_CAPTAIN *pChangeCaptain = (PLAYER_APPLY_TEAM_CHANGE_CAPTAIN*)pProtocol;
	int npcIndex = NpcSet.SearchID(pChangeCaptain->m_dwNpcID);
	if (npcIndex > 0)
	{
		int newCaptainPlayerIndex = Npc[npcIndex].GetPlayerIdx();
		Player[nIndex].GetTeamInfo().NewCaptain(newCaptainPlayerIndex);
	}
}

void KProtocolProcess::PlayerApplyTeamDismiss(int nIndex, BYTE* pProtocol, int nSize)
{
	Player[nIndex].GetTeamInfo().Dismiss();
}

void KProtocolProcess::PlayerEatItem(int nIndex, BYTE* pProtocol, int nSize)
{
	if ( pProtocol == NULL ||  nIndex <= 0 || nIndex >= MAX_PLAYER )
	{
		return;
	}
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].EatItem(pProtocol);
}

void KProtocolProcess::PlayerPickUpItem(int nIndex, BYTE* pProtocol, int nSize)
{
	if ( pProtocol == NULL ||  nIndex <= 0 || nIndex >= MAX_PLAYER )
	{
		return;
	}
	KPlayer& player = Player[nIndex];
	if (player.IsBlockClientControl())
		return;
	if (player.CheckTrading())
		return;

	player.ServerPickUpItem(pProtocol);
}

void KProtocolProcess::PlayerMoveItem(int nIndex, BYTE* pProtocol, int nSize)
{
	if ( pProtocol == NULL ||  nIndex <= 0 || nIndex >= MAX_PLAYER )
	{
		return;
	}
	Player[nIndex].ServerMoveItem(pProtocol);
}

void KProtocolProcess::PlayerSellItem(int nIndex, BYTE* pProtocol, int nSize)
{
	if ( pProtocol == NULL ||  nIndex <= 0 || nIndex >= MAX_PLAYER )
	{
		return;
	}
	Player[nIndex].SellItem(pProtocol);
}

void KProtocolProcess::PlayerBuyItem(int nIndex, BYTE* pProtocol, int nSize)
{
	if ( pProtocol == NULL ||  nIndex <= 0 || nIndex >= MAX_PLAYER )
	{
		return;
	}
	if (Player[nIndex].CheckTrading())
		return;
	Player[nIndex].BuyItem(pProtocol);
}

void KProtocolProcess::PlayerDropItem(int nIndex, BYTE* pProtocol, int nSize)
{
	if ( pProtocol == NULL ||  nIndex <= 0 || nIndex >= MAX_PLAYER )
	{
		return;
	}
	Player[nIndex].ServerThrowAwayItem(pProtocol);
}

void KProtocolProcess::PlayerSelUI(int nIndex, BYTE* pProtocol, int nSize)
{
	Player[nIndex].ProcessPlayerSelectFromUI(pProtocol);
}

void	KProtocolProcess::c2sTradeRequest(int nIndex, BYTE* pProtocol, int nSize)
{	
	if (!pProtocol)
		return;

	TRADE_APPLY_START_COMMAND* c2sTradeRequest = (TRADE_APPLY_START_COMMAND*)pProtocol;
	Player[nIndex].tradeServerReciveRequest(c2sTradeRequest->m_dwID);
}

void	KProtocolProcess::TradeMoveMoney(int nIndex, BYTE* pProtocol, int nSize)
{
	TRADE_MOVE_MONEY_COMMAND *pMoney = (TRADE_MOVE_MONEY_COMMAND*)pProtocol;

	Player[nIndex].tradeServerMoveMoney(pMoney->m_nMoney);
}

void KProtocolProcess::TradeDecision(int nIndex, BYTE* pProtocol, int nSize)
{
	TRADE_DECISION_COMMAND*	sDecision = (TRADE_DECISION_COMMAND*)pProtocol;
	switch((KTrade::TradeMsgType)sDecision->m_btDecision)
	{
	case KTrade::TRADE_MSG_SELF_LOCK:
		Player[nIndex].tradeServerReciveSelfLock();
		break;
	case KTrade::TRADE_MSG_SELF_END_TRADE:
		Player[nIndex].tradeServerReciveTradeEnd();
		break;
	case KTrade::TRADE_MSG_CANCEL:
		Player[nIndex].tradeServerReciveCancel();
		break;
	}
}

void	KProtocolProcess::DialogNpc(int nIndex, BYTE * pProtocol, int nSize)
{
	Player[nIndex].DialogNpc(pProtocol)	;
}

void	KProtocolProcess::TeamInviteAdd(int nIndex, BYTE * pProtocol, int nSize)
{
	TEAM_INVITE_ADD_COMMAND* pInviteJoinTeam = (TEAM_INVITE_ADD_COMMAND*)pProtocol;

	int npcIndex = 0;
	if (pInviteJoinTeam->PlayerName[0] != 0)
	{
		char playerName[sizeof(pInviteJoinTeam->PlayerName)] = { 0 };
		pInviteJoinTeam->PlayerName[31] = 0;
		memcpy(playerName, pInviteJoinTeam->PlayerName, sizeof(pInviteJoinTeam->PlayerName));
		playerName[sizeof(pInviteJoinTeam->PlayerName) - 1] = 0;
		npcIndex = NpcSet.SearchName(playerName);
	}
	
	if (IsValidNpc(npcIndex))
	{
		int invitePlayerIndex = Npc[npcIndex].GetPlayerIdx();
		Player[nIndex].GetTeamInfo().InviteJoinTeam(invitePlayerIndex);
	}
}

void	KProtocolProcess::TeamReplyInvite(int nIndex, BYTE * pProtocol, int nSize)
{
	TEAM_REPLY_INVITE_COMMAND	*pReply = (TEAM_REPLY_INVITE_COMMAND*)pProtocol;

	int inviterNpcIndex = NpcSet.SearchID(pReply->m_nIndex);
	if (IsValidNpc(inviterNpcIndex))
	{
		int inviterPlayerIndex = Npc[inviterNpcIndex].GetPlayerIdx();
		if (IsValidPlayer(inviterPlayerIndex))
		{
			Player[inviterNpcIndex].GetTeamInfo().ReplyInviteJoinTeam(nIndex, (enumInviteJointeamReplay)pReply->m_btResult);
		}
	}
}

void KProtocolProcess::ObjMouseClick(int nIndex, BYTE* pProtocol, int nSize)
{
	if (Player[nIndex].CheckTrading())
		return;

	int		nSubWorldIdx, nRegionIdx, nObjIdx;
	int		nPlayerX, nPlayerY, nObjX, nObjY;
	OBJ_MOUSE_CLICK_SYNC 	*pObj = (OBJ_MOUSE_CLICK_SYNC*)pProtocol;

	nSubWorldIdx = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
	nRegionIdx = SubWorld[nSubWorldIdx].FindRegion(pObj->m_dwRegionID);
	if (nRegionIdx < 0)
		return;
	nObjIdx = SubWorld[nSubWorldIdx].m_Region[nRegionIdx].FindObject(pObj->m_nObjID);
	if (nObjIdx <= 0)
		return;

	SubWorld[nSubWorldIdx].Map2Mps(
		Npc[Player[nIndex].m_nIndex].m_RegionIndex,
		Npc[Player[nIndex].m_nIndex].GetMapX(),
		Npc[Player[nIndex].m_nIndex].GetMapY(),
		Npc[Player[nIndex].m_nIndex].GetOffX(),
		Npc[Player[nIndex].m_nIndex].GetOffY(),
		&nPlayerX,
		&nPlayerY);
	SubWorld[nSubWorldIdx].Map2Mps(
		nRegionIdx,
		Object[nObjIdx].m_nMapX,
		Object[nObjIdx].m_nMapY,
		Object[nObjIdx].m_nOffX,
		Object[nObjIdx].m_nOffY,
		&nObjX,
		&nObjY);
/*
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
*/
	switch (Object[nObjIdx].m_nKind)
	{
	case Obj_Kind_Box:
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		if (Object[nObjIdx].m_nState == OBJ_BOX_STATE_CLOSE)
			Object[nObjIdx].ExecScript(nIndex);
		break;
	//--> Rocker 2005/05/12
	case Obj_Kind_Furniture:
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		Object[nObjIdx].ExecScript(nIndex);
		break;
	case Obj_Kind_House_Entry:
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		Object[nObjIdx].ExecScript(nIndex);
		break;
	//<-- End
	case Obj_Kind_Door:
		break;
	case Obj_Kind_Prop:
		if (g_GetDistance(nPlayerX, nPlayerY, nObjX, nObjY) > defMAX_EXEC_OBJ_SCRIPT_DISTANCE)
			break;
		if (Object[nObjIdx].m_nState == OBJ_PROP_STATE_DISPLAY)
			Object[nObjIdx].ExecScript(nIndex);
		break;
	}
}

void KProtocolProcess::StoreMoneyCommand(int nIndex, BYTE* pProtocol, int nSize)
{
	STORE_MONEY_COMMAND*	pCommand = (STORE_MONEY_COMMAND *)pProtocol;

	if( Player[nIndex].CheckTrading())
	{
		return;
	}
	
	if (!IsValidPlayer(nIndex))
		return;

    if (Player[nIndex].GetUIServerState().GetUIState(player_ui_repository) != player_ui_state_open)
		return;
	
	if (pCommand->m_byDir)	// 取钱
		Player[nIndex].m_ItemList.ExchangeMoney(room_repository, room_equipment, pCommand->m_dwMoney);
	else					// 存钱
		Player[nIndex].m_ItemList.ExchangeMoney(room_equipment, room_repository, pCommand->m_dwMoney);

}

void KProtocolProcess::NpcReviveCommand(int nIndex, BYTE* pProtocol, int nSize)
{
	NPC_REVIVE_COMMAND*		pCommand = (NPC_REVIVE_COMMAND *)pProtocol;
	Player[nIndex].Revive( TRUE );
}

void KProtocolProcess::c2sTradeReplyStart(int nIndex, BYTE* pProtocol, int nSize)
{
	TRADE_REPLY_START_COMMAND *pReply = (TRADE_REPLY_START_COMMAND*)pProtocol;
	if(1 == pReply->m_bDecision)
		Player[nIndex].tradeServerReciveAccept(pReply->oppositePlayerNpcId);
	else if(0 == pReply->m_bDecision)
		Player[nIndex].tradeServerReciveRefuse(pReply->oppositePlayerNpcId);
}

#define		defMAX_VIEW_EQUIP_TIME			30
void	KProtocolProcess::c2sViewEquip(int nIndex, BYTE* pProtocol, int nSize)
{
	if (g_SubWorldSet.GetGameTime() - Player[nIndex].m_nViewEquipTime < defMAX_VIEW_EQUIP_TIME)
		return;
	Player[nIndex].m_nViewEquipTime = g_SubWorldSet.GetGameTime();

	VIEW_EQUIP_COMMAND	*pView = (VIEW_EQUIP_COMMAND*)pProtocol;
	if (pView->m_dwNpcID == Npc[Player[nIndex].m_nIndex].m_dwID)
		return;
	int nPlayerIdx = Player[nIndex].FindAroundPlayer(pView->m_dwNpcID);
	if (nPlayerIdx <= 0)
		return;

	if (nIndex != nPlayerIdx && !Player[nPlayerIdx].GetCamoflag()) //注意:被查询者不可处于蒙面状态
		Player[nPlayerIdx].SendEquipItemInfo(nIndex);
}

/************************************************************
//函数：KProtocolProcess::IsPermitedNow()
//功能：根据目前的状态判断是否禁止该协议,1:禁止,0:允许
//参数：BYTE*	pMsg: 
//返回：void	: 1:允许; 0 :被禁止
//备注：需要检查状态,目前是trade和stall,这里仅仅检查stall,trade历史原因不是这样处理
//条件: 这些需要被检查的状态必须是互斥的, 即玩家一个时间只能处于一个状态.
************************************************************/
int  KProtocolProcess::IsPermitedNow(int nIndex, BYTE* pMsg, int nSize) 
{
// 	if( Player[nIndex].m_cTrade.isTrading() )
// 	{
// 		if(
// 			pMsg[0] != c2s_tradeapplystateopen &&
// 			pMsg[0] != c2s_tradeapplystateclose &&
// 			pMsg[0] != c2s_tradeapplystart &&
// 			pMsg[0] != c2s_trademovemoney &&
// 			pMsg[0] != c2s_tradedecision &&
// 			pMsg[0] != c2s_tradereplystart &&
// 			pMsg[0] != c2s_playermoveitem )
// 			return 0;
// 	}

	return 1;   //
}

void KProtocolProcess::ItemRepair(int nIndex, BYTE* pProtocol, int nSize)
{
	ITEM_REPAIR	*pIR = (ITEM_REPAIR *)pProtocol;

	if ( pIR == NULL )
	{
		return;
	}

	if (nIndex > 0 && nIndex < MAX_PLAYER)
	{
		Player[nIndex].repairItem(pIR->dwItemID, ((TRUE == pIR->special) ? true : false));
	}
}

/************************************************************************/
/*                    服务端升级装备函数                                */
/************************************************************************/
void KProtocolProcess::EnchaserItem(int nIndex, BYTE * pProtocol, int nSize)
{
	ENCHASER_CLIENTSEND* pEnchaserItem = (ENCHASER_CLIENTSEND*)pProtocol;

	//检查包是否正确
	if ( pEnchaserItem == NULL ||  nIndex <= 0 || nIndex >= MAX_PLAYER )
	{
		return;
	}

	if ( pEnchaserItem->ProtocolType != c2s_enchaseritem )
	{
		return;
	}

	if ( pEnchaserItem->nCompoundType < COMPOUND_LEVELUP ||
		pEnchaserItem->nCompoundType >= COMPOUND_COUNT )
	{
		return;
	}

	int posX = 0;
	int posY = 0;
	if (Player[nIndex].m_ItemList.CheckCanPlaceInEquipment(&posX, &posY) == false)
	{	
		g_ItemEnchaser.notifyClient(nIndex, COMPOUND_SMITH, enchaser_error_not_enough_space);
		return;
	}

	//判断客户端发来的装备是否有指向同一位置的道具索引
	for ( int x = 0; x < MAX_LEVELUP_ITEMS_COUNT; ++x )
	{
		ItemPos tmpItemPos;
		tmpItemPos.nPlace	= pEnchaserItem->Part[x].nPlace;
		tmpItemPos.nX		= pEnchaserItem->Part[x].nX;
		tmpItemPos.nY		= pEnchaserItem->Part[x].nY;
		for ( int y = 0; y < MAX_LEVELUP_ITEMS_COUNT; ++y )
		{
			if ( pEnchaserItem->Part[y].nPlace == tmpItemPos.nPlace &&
				pEnchaserItem->Part[y].nX == tmpItemPos.nX &&
				pEnchaserItem->Part[y].nY == tmpItemPos.nY &&
				Player[nIndex].m_ItemList.GetItemFromPlace(&tmpItemPos) &&
				y != x )
			{
				g_ItemEnchaser.notifyClient(nIndex, COMPOUND_SMITH, enchaser_error_conditionisinvalid);
				return;
			}

		}
	}

	//检查装备是否锁定
	for ( int i = 0; i < MAX_LEVELUP_ITEMS_COUNT; ++i )
	{
		ItemPos tmpItemPos;
		tmpItemPos.nPlace	= pEnchaserItem->Part[i].nPlace;
		tmpItemPos.nX		= pEnchaserItem->Part[i].nX;
		tmpItemPos.nY		= pEnchaserItem->Part[i].nY;
		KItem* pItem = Player[nIndex].m_ItemList.GetItemFromPlace(&tmpItemPos);
		if (pItem && pItem->IsLocked(Player[nIndex].GetNetConnectIdx(), false))
		{
			g_ItemEnchaser.notifyClient(nIndex, COMPOUND_SMITH, enchaser_error_conditionisinvalid);
			return;
		}
	}
	
	//检查合成道具是否非法,并初始化compound参数
	KCompoundPart PartArray[MAX_LEVELUP_ITEMS_COUNT];
	
	int nSrcItemCount = 0;	
	int index = 0;
	if(COMPOUND_MAKE == pEnchaserItem->nCompoundType)
	{
		//打造上性协议堵漏，这个流程已经废弃掉了。
	    return ;
	}//endif

	if(pEnchaserItem->nCompoundType == COMPOUND_SMITH)
	{
		for(int i = 0; i < MAX_LEVELUP_ITEMS_COUNT; i++)
		{
			ItemPos itemPos;
			itemPos.nPlace	= pEnchaserItem->Part[i].nPlace;
			itemPos.nX		= pEnchaserItem->Part[i].nX;
			itemPos.nY		= pEnchaserItem->Part[i].nY;
			KItem* item = Player[nIndex].m_ItemList.GetItemFromPlace(&itemPos);
			if(item == NULL)
			{
				continue;
			}
			nSrcItemCount++;
			PartArray[index++].SetItem(item);
		}	
	}
	else
	{
		ItemPos itemPos;
		itemPos.nPlace	= pEnchaserItem->Part[0].nPlace;
		itemPos.nX		= pEnchaserItem->Part[0].nX;
		itemPos.nY		= pEnchaserItem->Part[0].nY;
		KItem* item = Player[nIndex].m_ItemList.GetItemFromPlace(&itemPos);
		PartArray[index++].SetItem(item);
		
		for(int i = 1; i < MAX_LEVELUP_ITEMS_COUNT; i++)
		{
			ItemPos itemPos;
			itemPos.nPlace	= pEnchaserItem->Part[i].nPlace;
			itemPos.nX		= pEnchaserItem->Part[i].nX;
			itemPos.nY		= pEnchaserItem->Part[i].nY;
			KItem* item = Player[nIndex].m_ItemList.GetItemFromPlace(&itemPos);
			if(item == NULL)
			{
				continue;
			}
			nSrcItemCount++;
			PartArray[index++].SetItem(item);
		}	
	}
	
	// 开始合成格式化参数,开始合成
	TCompoundParams					EnchaserParams;
	ZeroMemory(&EnchaserParams, sizeof(TCompoundParams));
	EnchaserParams.pPartArray		= PartArray ;
	EnchaserParams.nTypeCount		= nSrcItemCount > MAX_LEVELUP_ITEMS_COUNT ? 0 : nSrcItemCount;
	EnchaserParams.nPlayerIndex		= nIndex;
	EnchaserParams.nCompoundType	= pEnchaserItem->nCompoundType;
	pEnchaserItem->szPlusInfo[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
	memcpy( EnchaserParams.szPlusInfo, pEnchaserItem->szPlusInfo, COMMON_CLIENT_MSG_LEN_64 );
	EnchaserParams.szPlusInfo[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;

	if(COMPOUND_SMITH == pEnchaserItem->nCompoundType)
	{
		int nEnchaserItemErrorCode = g_ItemEnchaser.smith(&EnchaserParams, pEnchaserItem->nRuleId);
	}//endif
	else
	{
		int nEnchaserItemErrorCode = g_ItemEnchaser.Compund(&EnchaserParams);
	}//end else

}

// Add by Cooler 2004-8-11
// Begin -->
void KProtocolProcess::c2sSplitPileItem(int nIndex, BYTE* pMsg, int nSize)
{
	if ( pMsg == NULL ||  nIndex <= 0 || nIndex >= MAX_PLAYER )
	{
		return;
	}

	const PSPLITPILEITEM pSplitInfo = (const PSPLITPILEITEM)pMsg;

	ItemPos sourPos, destPos;
	sourPos.nPlace = pSplitInfo->sourPlace;
	sourPos.nX = pSplitInfo->sourX;
	sourPos.nY = pSplitInfo->sourY;
	destPos.nPlace = pSplitInfo->destPlace;
	destPos.nX = pSplitInfo->destX;
	destPos.nY = pSplitInfo->destY;
	Player[nIndex].m_ItemList.splitItem(&sourPos, &destPos, pSplitInfo->splitItemCount);
}

void KProtocolProcess::c2sCheckStoragePassword(int nIndex, BYTE* pMsg, int nSize)
{
	const PCHECKSTORAGEPSW pCheckInfo = (const PCHECKSTORAGEPSW)pMsg;
	REQUESTREPLY tagReply;
	tagReply.ProtocolType = s2c_checkstoragepswok;
	
	pCheckInfo->szPassword[sizeof(pCheckInfo->szPassword) - 1] = 0;

	if(Player[nIndex].CheckBoxPassword(pCheckInfo->szPassword))
	{
		Player[nIndex].unlockStoreBox();
		tagReply.byReply = 1;
	}
	else
	{

// 		if(strcmp(pCheckInfo->szPassword, "111111112222222") == 0)
// 		{
// 			Player[nIndex].startAutoUnlock();
// 		}

		tagReply.byReply = 0;
	}
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, 
		&tagReply, sizeof(REQUESTREPLY));
}

void KProtocolProcess::c2sCreateStoragePassword(int nIndex, BYTE* pMsg, int nSize)
{
	const PCREATESTORAGEPSW pCreateInfo = (const PCREATESTORAGEPSW)pMsg;
	REQUESTREPLY tagReply;
	tagReply.ProtocolType = s2c_createstoragepswok;
	
	pCreateInfo->szPassword[sizeof(pCreateInfo->szPassword) - 1] = 0;

	if(Player[nIndex].SetBoxPassword(pCreateInfo->szPassword, ""))
	{
		tagReply.byReply = 1;
	}
	else
	{
		tagReply.byReply = 0;
	}
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, 
		&tagReply, sizeof(REQUESTREPLY));
}

void KProtocolProcess::c2sModifyStoragePassword(int nIndex, BYTE* pMsg, int nSize)
{
	const PMODIFYSTORAGEPSW pModifyInfo = (const PMODIFYSTORAGEPSW)pMsg;
	REQUESTREPLY tagReply;
	tagReply.ProtocolType = s2c_modifystoragepswok;
	
	pModifyInfo->szNewPassword[sizeof(pModifyInfo->szNewPassword) - 1] = 0;
	pModifyInfo->szOldPassword[sizeof(pModifyInfo->szOldPassword) - 1] = 0;

	if(Player[nIndex].SetBoxPassword(pModifyInfo->szNewPassword, 
		pModifyInfo->szOldPassword))
	{
		tagReply.byReply = 1;
	}
	else
	{
		tagReply.byReply = 0;
	}
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, 
		&tagReply, sizeof(REQUESTREPLY));
}

void KProtocolProcess::c2sCloseStorage(int nIndex, BYTE* pMsg, int nSize)
{
	Player[nIndex].lockStoreBox();
}
// End <--

void KProtocolProcess::c2sByteExtend(int nIndex, BYTE* pMsg, int nSize)
{
	PBYTE_EXTEND_HEADER pExtend = (PBYTE_EXTEND_HEADER)pMsg;
	
	switch( pExtend->ProtocolExtend ) 
	{
	case c2s_ex_protocol_shortcut_add:
		{
			_ShortCut_Add* pAdd = (_ShortCut_Add*)pMsg;
			Player[nIndex].GetItemList( ).AddShortCut( 
				pAdd->dwPos, pAdd->dwID, pAdd->dwSCType );
		}
		break;
	case c2s_ex_protocol_shortcut_del:
		{
			_ShortCut_Del* pDel = (_ShortCut_Del*)pMsg;
			Player[nIndex].GetItemList( ).DelShortCut( pDel->dwPos );
		}
		break;
	case c2s_ex_protocol_question:
		{
			ANSWER* pAnswer = (ANSWER*)pMsg;
			Player[nIndex].GetQuestionState().OnAnswer(pAnswer->Answer, pAnswer->wProtocolSize - (sizeof(ANSWER) - 1 - sizeof(pAnswer->Answer)));
		}
		break;
	case c2s_ex_protocol_gm:
		{
			Player[nIndex].ProcessGMOperation(pMsg);
		}
		break;
	default:
		break;
	}

	return;
}

void KProtocolProcess::c2sPetProtocol(int nIndex, BYTE* pMsg, int nSize)
{
}
//<------- End [Ray]

#endif

#ifndef _SERVER
void KProtocolProcess::s2cShowDamage(BYTE *pMsg)
{
	DAMAGESHOW	*pDamage = (DAMAGESHOW*)pMsg;
	int	receiverNpcIndex = NpcSet.SearchID(pDamage->dwReceiver);
	int casterNpcIndex = NpcSet.SearchID(pDamage->dwLauncher);

	if(receiverNpcIndex > 0)
	{
		//Npc[nIdx].SetBlood(pDamage->nDamage, pDamage->enType);
		Npc[receiverNpcIndex].GetCombatInfoShower().AddInfo(casterNpcIndex, pDamage->nDamage, pDamage->SkillId, (COMBAT_INFO_TYPE)pDamage->enType, (TRUE==pDamage->IsCrit ? true : false));
	}
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cNpcRealPosition(BYTE* pMsg)
{
	PNPCREALPOSITION pRet = (PNPCREALPOSITION)pMsg;

	int nNpcIdx = NpcSet.SearchID(pRet->dwNpcID);
	if(nNpcIdx > 0)
	{
		Npc[nNpcIdx].m_nRealPosX = pRet->nPosX;
		Npc[nNpcIdx].m_nRealPosY = pRet->nPosY;
		Npc[nNpcIdx].m_nRealDir = pRet->nDirection;
	}
}
#endif

#ifdef _SERVER

#include "buff_man.h"

void KProtocolProcess::c2sChatFamily(int nIndex, BYTE *pMsg, int nSize)
{
	if (nIndex < 0 || nIndex >= MAX_PLAYER || pMsg == NULL)
		return;

	g_ChatCenterS.ProcessProtocol(nIndex, pMsg, nSize);
}

void KProtocolProcess::c2sChgPKMode(int nIndex, BYTE *pMsg, int nSize)
{
	PChg_PK_Mode	pChgMode = (PChg_PK_Mode)pMsg;
	int nNpcIdx = Player[nIndex].m_nIndex;

	Npc[nNpcIdx].ChangePKMode( (PK_MODE)pChgMode->mode );
}

void KProtocolProcess::c2sSkillSync(int nIndex, BYTE *pMsg, int nSize)
{
	PPLAYER_SKILLINFO_SYNC	pSkillInfo = (PPLAYER_SKILLINFO_SYNC)pMsg;

	if(skill_ope_levelup == pSkillInfo->Operation)
	{
		int nNpcIdx = Player[nIndex].m_nIndex;

		if( g_SkillManager.CanUpdateTo(nNpcIdx, pSkillInfo->SkillId, pSkillInfo->PlusInfo.comInfo.Level) )
		{
			g_SkillManager.ConsumeUpdateCost(nNpcIdx, pSkillInfo->SkillId, pSkillInfo->PlusInfo.comInfo.Level);
			Npc[nNpcIdx].m_SkillList.LevelUpTo(pSkillInfo->SkillId, pSkillInfo->PlusInfo.comInfo.Level);
		}
	}
}

void KProtocolProcess::c2sBuffFamily(int nIndex, BYTE *pMsg, int nSize)
{
	BuffMgr& BM = BuffMgr::Singleton( );

	PBYTE_EXTEND_HEADER pExt = (PBYTE_EXTEND_HEADER)pMsg;
	switch( pExt->ProtocolExtend ) 
	{
	case buff_cop_cancel:
		{
			_Buff_Cancel* pBC = (_Buff_Cancel*)pMsg;
			BM.ClearBuffByClient( 
				Player[nIndex].m_nIndex, 
				pBC->ulBuffID );
		}
		break;
	default:
		break;
	}
}

void KProtocolProcess::c2sQuestFamily(int nPlayerIdx, BYTE *pMsg, int nSize)
{
	PBYTE_EXTEND_HEADER pExt = (PBYTE_EXTEND_HEADER)pMsg;
	switch( pExt->ProtocolExtend ) 
	{
	case c2s_query_npc_quest_state:
		{
			const _QUERY_NPC_QUEST_STATE* pExt = (const _QUERY_NPC_QUEST_STATE*)pMsg;
			const int nFindIndex = NpcSet.SearchID(pExt->dwNpcID);
			if (nFindIndex > 0)
			{
				if (Npc[nFindIndex].m_Kind == kind_dialoger)
				{
					Player[nPlayerIdx].ExecuteScript2Param(Player[nPlayerIdx].m_dwDeathScriptId, "QueryQuestState" , 1 , nFindIndex, Npc[nFindIndex].m_NpcSettingIdx );
					const int nScriptRet = Player[nPlayerIdx].m_nScriptResult;	
					_SYNC_NPC_QUEST_STATE SYNC;
					SYNC.Protocol = s2c_quest_family;
					SYNC.wProtocolSize = sizeof(SYNC) - 1;
					SYNC.ProtocolExtend = s2c_sync_npc_quest_state;
					SYNC.dwNpcID = Npc[nFindIndex].m_dwID;
					SYNC.ucState = nScriptRet;
					if (g_pServer != NULL)
						g_pServer->PackDataToClient( Player[nPlayerIdx].GetNetConnectIdx(),	&SYNC, sizeof(SYNC));
				}
			}
		}
		break;
	case c2s_query_quest_title:
		{
			const _QUERY_QUEST_TITLE* pExt = (const _QUERY_QUEST_TITLE* )pMsg;
			Player[nPlayerIdx].m_cTask.SendTaskInfo(nPlayerIdx);
		}
		break;
// 	case c2s_query_quest_detail_log:
// 		{
// 			const _QUERY_QUEST_DETAIL* pExt = (const _QUERY_QUEST_DETAIL* )pMsg;
// 			Player[nPlayerIdx].ExecuteScript2Param(Player[nPlayerIdx].m_dwDeathScriptId, "SyncQuestDetail",0, pExt->dwQuestID, 1);
// 		}
// 		break;
// 	case c2s_query_quest_detail_log_and_show:
// 		{
// 			const _QUERY_QUEST_DETAIL* pExt = (const _QUERY_QUEST_DETAIL* )pMsg;
// 			Player[nPlayerIdx].ExecuteScript2Param(Player[nPlayerIdx].m_dwDeathScriptId, "SyncQuestDetail",0, pExt->dwQuestID, 2);
// 		}
// 		break;
// 	case c2s_query_quest_detail_cache:
// 		{
// 			const _QUERY_QUEST_DETAIL* pExt = (const _QUERY_QUEST_DETAIL* )pMsg;
// 			Player[nPlayerIdx].ExecuteScript2Param(Player[nPlayerIdx].m_dwDeathScriptId, "SyncQuestDetail",0, pExt->dwQuestID, 3);
// 		}
// 		break;
	case c2s_abandon_quest:
		{
			const _ABANDON_QUEST* pExt = (const _ABANDON_QUEST* )pMsg;
			Player[nPlayerIdx].ExecuteScript2Param(Player[nPlayerIdx].m_dwDeathScriptId, "AbandonQuest",0, pExt->dwQuestID, 0);
		}
		break;
	default:
		break;
	}
}

#endif

#ifndef _SERVER

void KProtocolProcess::s2cChatFamily(BYTE *pMsg)
{
	g_ChatCenterC.ProcessProtocol(pMsg);
}

void KProtocolProcess::s2cBuffFamily(BYTE* pMsg)
{
	PBYTE_EXTEND_HEADER pExt = (PBYTE_EXTEND_HEADER)pMsg;
	switch( pExt->ProtocolExtend ) 
	{
	case buff_sync_add:
		{
			_Buff_Add* pBA = (_Buff_Add*)pMsg;

			int nNpcIdx = NpcSet.SearchID( pBA->dwNpcID );

			if( !nNpcIdx )
				break;
			Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;

			if( !Player[CLIENT_PLAYER_INDEX].ConformIdx( nNpcIdx ) )
			{
				KBufferSyncInfo KBS = {0};
				KBS.nBuffID		= pBA->ulBuffID;
				KBS.nTime		= pBA->ulTime;
				KBS.nTempBuffID = pBA->ulTempID;
				KBS.nPileCount	= pBA->nPileCount;
				CoreDataChanged( GDCNI_BUFFER_OPEN, (UINT)&KBS, 0 );
			}

			BuffTable& BT = BuffTable::Singleton( );
			PBAT pBAT = BT.GetBuff( pBA->ulTempID );
			if ( pBAT )
			{
				Npc[nNpcIdx].SetInstantSpr( pBAT->nOnceID );
				if ( pBAT->nOnceSoundID > 0 )
				{
					char szBuff[256];
					sprintf( szBuff, BUFF_ONCE_SOUND, pBAT->nOnceSoundID  );
					KCacheNode* pSoundNode = NULL;
					pSoundNode = g_SoundCache.GetNode(szBuff, (KCacheNode*)pSoundNode);
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
			}

			Npc[nNpcIdx].AddBuffToC( pBA->ulBuffID, pBA->ulTempID );

			if ( Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetTargetNpc() == nNpcIdx )
			{
				CoreDataChanged( CDCNI_UPDATA_SEL_TARGET, NULL, NULL );
			}

		}
		break;
	case buff_sync_info:
		{
			_Buff_Info* pBI = (_Buff_Info*)pMsg;

			int nNpcIdx = NpcSet.SearchID( pBI->dwNpcID );
			
			if( !nNpcIdx )
				break;
			Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
			
			if( !Player[CLIENT_PLAYER_INDEX].ConformIdx( nNpcIdx ) )
			{
				KBufferSyncInfo KBS = {0};
				KBS.nBuffID		= pBI->ulBuffID;
				KBS.nTime		= pBI->ulTime;
				KBS.nTempBuffID	= pBI->ulTempID;
				KBS.nPileCount	= pBI->nPileCount;
				CoreDataChanged( GDCNI_BUFFER_OPEN, (UINT)&KBS, 0 );
			}
		}
		break;
	case buff_sync_del:
		{
			_Buff_Del* pBD = (_Buff_Del*)pMsg;

			int nNpcIdx = NpcSet.SearchID( pBD->dwNpcID );
			
			if( !nNpcIdx )
				break;
			Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;

			if( !Player[CLIENT_PLAYER_INDEX].ConformIdx( nNpcIdx ) )
				CoreDataChanged( GDCNI_BUFFER_DEL, pBD->ulBuffID, 0 );

			Npc[nNpcIdx].RemoveBuffFromC( pBD->ulBuffID );

			if ( Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetTargetNpc() == nNpcIdx )
			{
				CoreDataChanged( CDCNI_UPDATA_SEL_TARGET, NULL, NULL );
			}
		}
		break;
	case buff_sync_npc:
		{
			_Buff_Sync_Npc* pBSN = (_Buff_Sync_Npc*)pMsg;

			/*
			//如果已经组队，则可能同步的是队友的BUFF
			KPlayerTeam& teamInfo = GetClientPlayer().GetTeamInfo();
			if (teamInfo.IsInTeam())
			{
				KTeam* pTeam = teamInfo.GetTeam();
				if (pTeam != NULL)
				{
					int memberIndex = pTeam->FindMemberID(pBSN->dwID);
					if (memberIndex >= 0)
					{
						ClientTeamMemberInfo* pMemberInfo = pTeam->GetMemberInfo(memberIndex);
						if (pMemberInfo != NULL)
						{
							pMemberInfo->BuffCount = pBSN->wCount < MAX_SYNC_TEAMATE_BUFF_COUNT ? pBSN->wCount : MAX_SYNC_TEAMATE_BUFF_COUNT;
							memcpy(pMemberInfo->BuffPair, pBSN->Buff, sizeof(_BuffPair) * pMemberInfo->BuffCount);
						}
					}
				}
			}
			*/
			
			int nNpcIdx = NpcSet.SearchID( pBSN->dwID );

			if (nNpcIdx > 0)
				Npc[nNpcIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;

			if( !Player[CLIENT_PLAYER_INDEX].ConformIdx( nNpcIdx ) )
				return;

			Npc[nNpcIdx].ClearBuffFromC( );

			for( int nLoopCount = 0; nLoopCount < pBSN->wCount; nLoopCount++ )
				Npc[nNpcIdx].AddBuffToC( 
				pBSN->Buff[nLoopCount].dwBuffTID, 
				pBSN->Buff[nLoopCount].dwBuffTempID );


			if ( Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetTargetNpc() == nNpcIdx )
			{
				CoreDataChanged( CDCNI_UPDATA_SEL_TARGET, NULL, NULL );
			}
		}
		break;
	default:
		break;
	}
}

void KProtocolProcess::s2cQuestFamily(BYTE *pMsg)
{
	PBYTE_EXTEND_HEADER pExt = (PBYTE_EXTEND_HEADER)pMsg;
	switch( pExt->ProtocolExtend ) 
	{
	case s2c_sync_npc_quest_state:
		{
			const _SYNC_NPC_QUEST_STATE* pExt = (const _SYNC_NPC_QUEST_STATE*)pMsg;
			int nFindNpc = NpcSet.SearchID(pExt->dwNpcID);
			if (nFindNpc > 0)
			{
				Npc[nFindNpc].m_DataRes.SetQuestIcon(pExt->ucState);
			}

		}
		break;
// 	case s2c_sync_quest_title:
// 		{
// 			const _SYNC_QUEST_TITLE* pExt = (const _SYNC_QUEST_TITLE*)pMsg;
// 			KQuestInfo Quest  = {0};
// 			const unsigned char* pReadPos = &pExt->pContent[0];
// 
// 			Quest.id = *(int*)pReadPos;
// 			pReadPos += 4;
// 
// 			int nLen = *(int*)pReadPos;
// 			pReadPos += 4;
// 			memcpy(Quest.name, pReadPos, nLen);
// 			Quest.name[nLen + 1] = 0;
// 			pReadPos += nLen;
// 			QuestLog::GetInstance().AppendQuest(Quest);
// 		}
// 		break;
// 	case s2c_sync_quest_detail_log:
// 	case s2c_sync_quest_detail_log_and_show:
// 	case s2c_sync_quest_detail_cache:
// 		{
// 			const _SYNC_QUEST_DETAIL* pExt = (const _SYNC_QUEST_DETAIL*)pMsg;
// 			KQuestInfo Quest = {0};
// 			const unsigned char* pReadPos = &pExt->pContent[0];
// 
// 			Quest.id = *(int*)pReadPos;
// 			pReadPos += 4;
// 
// 			int nLen = *(int*)pReadPos;
// 			pReadPos += 4;
// 			memcpy(Quest.name, pReadPos, nLen);
// 			Quest.name[nLen + 1] = 0;
// 			pReadPos += nLen;
// 
//  			nLen = *(int*)pReadPos;
// 			pReadPos += 4;
// 			memcpy(Quest.category, pReadPos, nLen);
// 			Quest.category[nLen + 1] = 0;
// 			pReadPos += nLen;
//            
// 			nLen = *(int*)pReadPos;
// 			pReadPos += 4;
// 			memcpy(Quest.description, pReadPos, nLen);
// 			Quest.description[nLen + 1] = 0;
// 			pReadPos += nLen;
// 			
// 			nLen = *(int*)pReadPos;
// 			pReadPos += 4;
// 			memcpy(Quest.aim, pReadPos, nLen);
// 			Quest.aim[nLen + 1] = 0;
// 			pReadPos += nLen;
// 			
// 			int nCount ;
// 			int i;
// 
// 			for (unsigned int objective_idx = speak_to ; objective_idx < max_objective_type; objective_idx++)
// 			{
// 				nCount = *(int*)pReadPos;
// 				pReadPos += 4;
// 				if (nCount > 0)
// 				{
// 					for (unsigned int c = 0; c < nCount; c++)
// 					{
// 						Quest.requirement[objective_idx][c].uID = *(int*)pReadPos;
// 						pReadPos += 4;						
// 						Quest.requirement[objective_idx][c].uCount = *(int*)pReadPos;
// 						pReadPos += 4;						
// 					}
// 				}
// 			}	
// 			
// 			Quest.rewardExp = *(int*)pReadPos;
// 			pReadPos += 4;						
// 			
// 			Quest.rewardMoney = *(int*)pReadPos;
// 			pReadPos += 4;						
// 			
// 			nCount = *(int*)pReadPos;
// 			pReadPos += 4;
// 			Quest.rewardItemCount = nCount;
// 			for(i = 0; i < nCount; i ++)
// 			{
// 				Quest.rewardItem[i].ItemType = *(int*)pReadPos;
// 				pReadPos += 4;
// 				Quest.rewardItem[i].ItemNum = *(int*)pReadPos;
// 				pReadPos += 4;
// 				Quest.rewardItem[i].ItemID = *(int*)pReadPos;
// 				pReadPos += 4;
// 			}
// 			
// 			nCount = *(int*)pReadPos;
// 			pReadPos += 4;						
// 			Quest.selectRewardItemCount = nCount;
// 			for(i = 0; i < nCount; i ++)
// 			{
// 				Quest.selectRewardItem[i].ItemType = *(int*)pReadPos;
// 				pReadPos += 4;
// 				Quest.selectRewardItem[i].ItemNum = *(int*)pReadPos;
// 				pReadPos += 4;
// 				Quest.selectRewardItem[i].ItemID = *(int*)pReadPos;
// 				pReadPos += 4;
// 			}
// 
//             switch(pExt->ProtocolExtend)
//             {
//             case s2c_sync_quest_detail_cache:
//                 {
//                     QuestCache::GetInstance().AppendQuest(Quest);
//                 }
//                 break;
//             case s2c_sync_quest_detail_log:
//                 {
// 				    QuestLog::GetInstance().AppendQuest(Quest);
//                 }
//                 break;
//             case s2c_sync_quest_detail_log_and_show:
//                 {
// 				    QuestLog::GetInstance().AppendQuest(Quest);
//                     unsigned int uIdx = QuestLog::GetInstance().FindQuest(Quest.id);
//                     const KQuestInfo& quest = QuestLog::GetInstance().GetQuestInfoByIdx(uIdx);
//                     CoreDataChanged(GDCNI_OPEN_QUEST_NPC_DIALOG,(unsigned int)&quest, 0);
//                 }
//                 break;
//             }
// 		}
// 		break;
// 	case s2c_sync_quest_process:
// 		{
// 			const _SYNC_QUEST_PROCESS* pExt = (const _SYNC_QUEST_PROCESS*)pMsg;
// 			QuestLog::GetInstance().UpDateProcess(pExt->dwQuestID, pExt->btKind, pExt->btObjective, pExt->dwValue);
// 		}
// 		break;
// 	case s2c_remove_quest:
// 		{
// 			const _REMOVE_QUEST* pExt = (const _REMOVE_QUEST*)pMsg;
// 			QuestLog::GetInstance().RemoveQuest(pExt->dwQuestID);
// 		}
// 		break;
//     case s2c_sync_quest_state:
//         {
// 			const _SYNC_QUEST_STATE* pExt = (const _SYNC_QUEST_STATE*)pMsg;
//             QuestLog::GetInstance().UpDateState(pExt->dwQuestID, pExt->dwState);
//         }
//         break;
	default:
		break;
	}
}
#endif

#ifdef _SERVER
void KProtocolProcess::c2sAuctionSync(int nIndex, BYTE *pMsg, int nSize)
{
	if (nIndex < 0 || nIndex >= MAX_PLAYER || pMsg == NULL)
		return;

	Player[nIndex].m_serverAucMgr.ProcessProtocol(nIndex, pMsg, nSize);
}

void KProtocolProcess::c2sPlayerRealInfoSync(int nIndex, BYTE * pMsg, int nSize)
{
	if ( nIndex < 0 || nIndex >= MAX_PLAYER || pMsg == NULL )
		return ;

	Player[nIndex].m_serverPRIMgr.ProtocolProcess(nIndex, pMsg, nSize);
}

void KProtocolProcess::c2sPlayerStopNotify(int nIndex, BYTE *pMsg, int nSize)
{
    PLAYER_STOP_NOTIFY * pStop=(PLAYER_STOP_NOTIFY *)pMsg;
	Npc[Player[nIndex].m_nIndex].SendCommand(do_stand);
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cAuctionSync(BYTE *pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_clientAucMgr.ProcessProtocol(pMsg);
}
#endif

#ifdef _SERVER
void KProtocolProcess::c2sSocialRelationSync(int nIndex, BYTE *pMsg, int nSize)
{
	ServerSocialUnitMgr::Singleton().ProcessProcotol(nIndex, pMsg, nSize);
}

void KProtocolProcess::c2sTalismanFamily(int nIndex, BYTE *pMsg, int nSize)
{
	TalismanManager::TalismanManager().ServerProcessProtocol(nIndex, pMsg, nSize);
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cSocialRelationSync(BYTE *pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_clientSUMgr.ProcessProtocol(pMsg);
}

#define MAX_TEMP_COMPRESION_BUFF 2048
void KProtocolProcess::s2cSocialRelationInfoSync(BYTE *pMsg)
{	
	PSYNC_SOCIAL_RELATION pOldPackage = (PSYNC_SOCIAL_RELATION)pMsg;

	//Decompression.......................................................
	int nOldSize = pOldPackage->Len - sizeof(SYNC_SOCIAL_RELATION) + 1 + PROTOCOL_SIZE;
	
	unsigned char   szCompressionBuff[MAX_TEMP_COMPRESION_BUFF];
	PSYNC_SOCIAL_RELATION pNewPackage = (PSYNC_SOCIAL_RELATION)szCompressionBuff;
	pNewPackage->Protocol = pOldPackage->Protocol;
	pNewPackage->Len      = pOldPackage->Len;

	unsigned int nLen = MAX_TEMP_COMPRESION_BUFF - sizeof(SYNC_SOCIAL_RELATION);
	lzo1x_decompress(
		(const unsigned char *)pOldPackage->data,
		nOldSize,
		(unsigned char *)pNewPackage->data,
		&nLen,
		NULL);
	
	pMsg = (BYTE *)pNewPackage;
	//Decompression end...................................................

	Player[CLIENT_PLAYER_INDEX].GetClientSocialRelation().RecvSocialRelationSync(pMsg);
	Player[CLIENT_PLAYER_INDEX].m_clientSUMgr.NotifyChangeRelation();

	KPlayer& player = GetClientPlayer();
	const PClientRelationInfo pRelationInfo = player.GetClientSocialRelation().GetRelationInfo(enSUTplId_Tong);
	if (pRelationInfo != NULL)
	{
		Npc[player.GetNpcIndex()].SetZhuhouName(pRelationInfo->Names[enSULayer_Tong]);
		Npc[player.GetNpcIndex()].SetShizuName(pRelationInfo->Names[enSULayer_Gens]);
		Npc[player.GetNpcIndex()].SetLeagueName(pRelationInfo->Names[enSULayer_League]);
	}

	SYNC_SOCIAL_RELATION_INFO *ps2cData = (SYNC_SOCIAL_RELATION_INFO*)pNewPackage->data;

	Npc[player.GetNpcIndex()].SetCityId( ps2cData->CityMapId );

	if(INVALID_WORLD_ID != ps2cData->CityMapId)
	{
		char szCityName[MAXSIZE_CITYNAME] = { 0 };
		g_SubWorldSet.GetWorldNameFromID(ps2cData->CityMapId, szCityName, sizeof(szCityName));
		Npc[player.GetNpcIndex()].SetCityName(szCityName);
	}//endif

	for(int nLayer = 0; nLayer < MAX_SOCIETY_LAYER_COUNT; ++nLayer)
		Npc[player.GetNpcIndex()].SetUnitOwnerFlag(nLayer, ps2cData->OwnerFlag[nLayer]);

	Npc[player.GetNpcIndex()].SetGensMgr(Npc[player.GetNpcIndex()].IsUnitOwner(enSULayer_Gens));
	Npc[player.GetNpcIndex()].SetKing(Npc[player.GetNpcIndex()].IsUnitOwner(enSULayer_Tong));
	Npc[player.GetNpcIndex()].SetLeaguer(Npc[player.GetNpcIndex()].IsUnitOwner(enSULayer_League));

	if (ps2cData->TopLayer>=enSULayer_League)
		Npc[player.GetNpcIndex()].SetHasLeagueLayer(true);
	else
		Npc[player.GetNpcIndex()].SetHasLeagueLayer(false);

	//HeadInfo Added
	ConfigManager& cm = ConfigManager::Singleton();
	
	Npc[player.GetNpcIndex()].DelHeadInfo(HEAD_INFO_GENS);
	Npc[player.GetNpcIndex()].DelHeadInfo(HEAD_INFO_TONG);
	
	//HeadInfo Added
	if (Npc[player.GetNpcIndex()].IsKing())
	{
		const char* pTbuff = cm.GetConfigurableDisplayStyle( style_role_head_image_info, 0 );
		if (pTbuff)
			Npc[player.GetNpcIndex()].AddLayoutToHeadInfo(pTbuff,HEAD_INFO_TONG,KNpc::HIP_Important);
		
	}//endif
	else if (Npc[player.GetNpcIndex()].IsGensMgr())
	{
		const char* pTbuff = cm.GetConfigurableDisplayStyle( style_role_head_image_info, 1 );
		if (pTbuff)
			Npc[player.GetNpcIndex()].AddLayoutToHeadInfo(pTbuff,HEAD_INFO_GENS,KNpc::HIP_Important);
	}//end for if

	Npc[player.GetNpcIndex()].SetHeadInfoChanged(true);
}

void KProtocolProcess::s2cDelayedAction(BYTE *pMsg)
{
	PDELAYED_ACTION pDelayedAction = (PDELAYED_ACTION)pMsg;

	CASTBAR_PARAM param;
	param.cmd = (DELAYED_ACTION_OPER)(pDelayedAction->Command);
	param.time = pDelayedAction->Time;
	param.msgCode = pDelayedAction->Message;
	
	CoreDataChanged(CDCNI_CASTBAR_OPER, (unsigned int)&param, NULL);
}

void KProtocolProcess::s2cTalismanFamily(BYTE *pMsg)
{
	TalismanManager::Singleton().ClientProcessProtocol(pMsg);
}

void KProtocolProcess::s2cSyncTalismanEnchase(BYTE *pMsg)
{
	PSYNC_TALISMAN_ENCHASE pTalismanEnchase = (PSYNC_TALISMAN_ENCHASE)pMsg;
	int talismanId = pTalismanEnchase->ItemId;
	int talismanIndex = ItemSet.SearchID(talismanId);
	if (talismanIndex > 0)
	{
		KItem& talisman = Item[talismanIndex];
		for (int enchaseLoopCount = 0; enchaseLoopCount < TM_HOLE_NUM; enchaseLoopCount++)
		{
			talisman.SetTalismanEnchase(enchaseLoopCount, pTalismanEnchase->EnchaseSet[enchaseLoopCount]);
		}	
	}
}

void KProtocolProcess::s2cSyncNpcEquipTalisman(BYTE *pMsg)
{
	PSYNC_NPC_EQUIP_TALISMAN pTalismanSync = (PSYNC_NPC_EQUIP_TALISMAN)pMsg;
	DWORD npcId = pTalismanSync->NpcId;
	int talismanNpcId = pTalismanSync->TalismanNpcId;
	int npcIndex = NpcSet.SearchID(npcId);
	if (npcIndex > 0)
	{
		Npc[npcIndex].SetEquipTalismanNpcId(talismanNpcId);
	}
}
#endif

#ifdef _SERVER
void KProtocolProcess::c2sPlayerLogout(int nIndex, BYTE *pMsg, int nSize)
{
	PLAYER_LOGOUT	*pc2sParam = (PLAYER_LOGOUT*)pMsg;
	
	if(enPLO_StartTimer == pc2sParam->operation)
	{
		int	nNpcIdx = Player[nIndex].m_nIndex;

		if( !Npc[nNpcIdx].IsInSafeArea() )
			Player[nIndex].StartLogoutTimer();
	}
	else if(enPLO_StopTimer == pc2sParam->operation)
	{
		Player[nIndex].StopLogoutTimer();
	}
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cIBFamily(BYTE *pMsg)
{
	IBCenter_C::Singleton().ProcessServerProtocol(pMsg);
}
void KProtocolProcess::s2cTaisuiWheel(BYTE * pMsg)
{
     ITaisuiWheel * pTaisui=GetClientPlayer().GetTaisuiWheelSys();
	 if(pTaisui!=NULL)
		 pTaisui->ProcessMsg(pMsg);
}
#endif

#ifdef _SERVER
void KProtocolProcess::c2sIBFamily(int nIndex, BYTE *pMsg, int nSize)
{
	if (!IsPlayerIdxValid(nIndex) || pMsg == NULL)
		return;

	IBCenter_S::Singleton().ProcClientProtocol(nIndex, pMsg, nSize);
}

void KProtocolProcess::c2sTaisuiWheel(int nIndex,BYTE * pMsg,int nSize)
{
  ITaisuiWheel * pTaisui=Player[nIndex].GetTaisuiWheelSys();
  
  if(pTaisui!=NULL)
	  pTaisui->ProcessMsg(pMsg,nSize);
}

#endif

#ifdef _SERVER
void KProtocolProcess::c2sTeamOperation(int nIndex, BYTE *pMsg, int nSize)
{
	Player[nIndex].GetTeamInfo().ProcessTeamOperation((TEAM_OPERATION*)pMsg);
}
#endif

#ifdef _SERVER
void KProtocolProcess::c2sReplyPrompt(int nIndex, BYTE *pMsg, int nSize)
{
	KPlayer& player = Player[nIndex];
	CLIENT_REPLY_PROMPT* pReply = (CLIENT_REPLY_PROMPT*)pMsg;
	switch(pReply->Event)
	{
	case prompt_event_add_buff:
		{
			DelayedAction* pDelayedAction = player.GetActionDelayer().GetAction(delayed_action_add_buff);
			if (pDelayedAction)
			{
				pDelayedAction->GetAddBuffParam().m_Accept = pReply->Reply == TRUE;
				pDelayedAction->Do();
			}
		}
		break;
	}
}

#define MAX_POS_EDIT_OFFSET_X 128
#define MAX_POS_EDIT_OFFSET_Y 128

void KProtocolProcess::c2sPosSync(int nIndex, BYTE *pMsg, int nSize)
{
	C2S_POS_SYNC     * pSync= (C2S_POS_SYNC     *)pMsg;
	
	/*
	if ( Npc[Player[nIndex].m_nIndex].CheckClientStandPos(pSync->nStopX,pSync->nStopY))
	{
        Npc[Player[nIndex].m_nIndex].SetPosDirectly(pSync->nStopX,pSync->nStopY,pSync->nDir);	
	
		S2C_POS_EDITION syncpos;
		syncpos.ProtocolType = (BYTE)s2c_pos_edition;
		syncpos.nNpcID = Player[nIndex].m_dwID;
		Npc[Player[nIndex].m_nIndex].GetMpsPos(&syncpos.nX,&syncpos.nY);
		
		int nMaxCount = MAX_BROADCAST_COUNT;
        Npc[Player[nIndex].m_nIndex].BroadCastRegion(&syncpos, sizeof(syncpos), nMaxCount);
	}//endif
	*/

	if (Player[nIndex].IsValid())
	{
		int nNpcIndex = Player[nIndex].GetNpcIndex();
		int nMpsX = 0 ;
		int nMpsY = 0 ;
		Npc[nNpcIndex].GetMpsPos(&nMpsX,&nMpsY);

		if (abs(nMpsX - pSync->nStopX) <= MAX_POS_EDIT_OFFSET_X && abs(nMpsY - pSync->nStopY) <= MAX_POS_EDIT_OFFSET_Y)
			Npc[Player[nIndex].m_nIndex].SendCommand(do_run,pSync->nStopX,pSync->nStopY);
	}//endif

}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cShowBanner(BYTE *pMsg)
{
	PSHOW_BANNER pShowBanner = (PSHOW_BANNER)pMsg;

	char* pData = pShowBanner->data;

	char msgBuff[MAX_SHOW_BANNER_MSG_LENGTH];
	memset(msgBuff, 0, sizeof(msgBuff));
	char fontBuff[MAX_SHOW_BANNER_FONT_LENGTH];
	memset(fontBuff, 0, sizeof(fontBuff));

	int msgLength = strlen(strncpy(msgBuff, pData, MAX_SHOW_BANNER_MSG_LENGTH));
	pData += msgLength + 1;
	int fontLength = strlen(strncpy(fontBuff, pData, MAX_SHOW_BANNER_FONT_LENGTH));

	if(pShowBanner->bannerType == 1)
	{
		CommonStyle style;
		
		strncpy(style.font, fontBuff, COMMON_CLIENT_MSG_LEN_64);
		style.font[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
		style.color = pShowBanner->Colour;

		style.speed = pShowBanner->speed;
		style.second = pShowBanner->second;

		CoreDataChanged(GDCNI_TOPMESSAGE, (unsigned int)msgBuff, (int)&style);
	}
	else if(pShowBanner->bannerType == 2)
	{
		//必须是诸侯成员
		const PClientRelationInfo pRelation = Player[CLIENT_PLAYER_INDEX].GetClientSocialRelation().GetRelationInfo(enSUTplId_Tong);
		if(pRelation != NULL && enSULayer_Tong <= pRelation->TopLayer)
		{
			CoreDataChanged(GDCNI_SHIZU_BANNER, (unsigned int)msgBuff, pShowBanner->speed);
		}
	}
}

void KProtocolProcess::s2cShowBannerById(BYTE *pMsg)
{
	PSHOW_BANNER_ID pShowBanner = (PSHOW_BANNER_ID)pMsg;

	if(pShowBanner->bannerType == 1)
	{
		CommonStyle2 style;
		style.msgId = pShowBanner->msgId;
		style.fontId = pShowBanner->fontId;
		style.color = pShowBanner->color;
		style.speed = pShowBanner->speed;
		style.second = pShowBanner->second;

		CoreDataChanged(GDCNI_TOPMESSAGE_ID, (unsigned int)&style, NULL);
	}
	else if(pShowBanner->bannerType == 2)
	{

	}
}

void KProtocolProcess::s2cGMFeedBack(BYTE *pMsg)
{
	PGM_COMMUNICATION_DATA gmFeedBackMsg = (PGM_COMMUNICATION_DATA)pMsg;
	CoreDataChanged(GDCNI_GM_FEED_BACK, (unsigned int)&gmFeedBackMsg->data, NULL);
}

void KProtocolProcess::s2cHireDataListExp(BYTE *pMsg)
{
	char receiveBuff[MAX_HIRE_LIST_SYNC_BUFF_LENGTH];
	if (DecompressProtocol(pMsg, (BYTE*)receiveBuff, sizeof(receiveBuff)))
	{
		PEXP_HIRE_LIST pHireList = (PEXP_HIRE_LIST)receiveBuff;

		vector<ExpHirer> hireList;
		for(int i = 0; i < HireRetListMaxCount; ++i)
		{
			hireList.push_back(pHireList->data[i]);
		}
		CoreDataChanged(GDCNI_RECV_HIRE_DATA_EXP, (unsigned int)&hireList, pHireList->stargIndex);
	}
}

void KProtocolProcess::s2cHireDataListFighter(BYTE *pMsg)
{
	char receiveBuff[MAX_HIRE_LIST_SYNC_BUFF_LENGTH];
	if (DecompressProtocol(pMsg, (BYTE*)receiveBuff, sizeof(receiveBuff)))
	{
		PFIGHTER_HIRE_LIST pHireList = (PFIGHTER_HIRE_LIST)receiveBuff;

		vector<FighterHirer> hireList;
		for(int i = 0; i < HireRetListMaxCount; ++i)
		{
			hireList.push_back(pHireList->data[i]);
		}
		CoreDataChanged(GDCNI_RECV_HIRE_DATA_FIGHT, (unsigned int)&hireList, pHireList->stargIndex);
	}
}

void KProtocolProcess::s2cHireRetCode(BYTE *pMsg)
{
	PHIRE_RET_CODE retCode = (PHIRE_RET_CODE)pMsg;
	CoreDataChanged(GDCNI_HIRE_REQ_RET, NULL, retCode->ret);
}

void KProtocolProcess::s2cFindFamily(BYTE *pMsg)
{
	FindResult* pResult = (FindResult*)pMsg;
	if ( pResult )
	{
		CoreDataChanged( GDCNI_OPEN_PLAYER_INFO, (unsigned int)pResult, NULL );
	}
}
#endif

#ifdef _SERVER
void	KProtocolProcess::c2sFindFamily(int nIndex, BYTE *pMsg, int nSize)
{
	//检查包是否正确
	if ( pMsg == NULL ||  nIndex <= 0 || nIndex >= MAX_PLAYER )
	{
		return;
	}

	FindParam* pParam = (FindParam*)pMsg;
	if ( pParam )
	{
		pParam->szName[16] = 0;
		int nPlayerIdx = g_PlayerInfoToIndex.GetIndexByName( pParam->szName );
		if ( nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER && !Player[nPlayerIdx].GetCamoflag())
		{
			FindResult tagResult;
			memset( &tagResult, 0, sizeof(tagResult));
			tagResult.ProtocolType = s2c_find_family;
			strncpy( tagResult.szName, Player[nPlayerIdx].GetPlayerName(), 17 );
			tagResult.sLevel = Player[nPlayerIdx].GetLevel();
			tagResult.sMetier = Player[nPlayerIdx].GetSeries();
			tagResult.sSkillType = Player[nPlayerIdx].GetSkillSeries();
			SocialUnit *pUnit = GetLeafUnit(nPlayerIdx, enSUTplId_Tong);
			if (pUnit != NULL)
			{
				SocialUnit *pShizu = GetUpNUnit(pUnit, enSULayer_Gens);
				if (pShizu != NULL)
				{
					const char* shizuName = GetUnitName(pShizu->GetUnitAttr());
					if (shizuName != NULL)
						strncpy(tagResult.szShizu, shizuName,17);
				}

				SocialUnit *pZhuhou = GetUpNUnit(pUnit, enSULayer_Tong);
				if (pZhuhou != NULL)
				{
					const char* zhuhouName = GetUnitName(pZhuhou->GetUnitAttr());
					if (zhuhouName != NULL)
						strncpy(tagResult.szZhuhou, zhuhouName,17);
				}

			}
			SendDataToClient( nIndex, &tagResult, sizeof(tagResult) );	
		}
		else
		{
// 			CHAT_ERR_CODE	data;
// 
// 			data.protocol.protocol = s2c_chat_family;
// 			data.protocol.subProtocol = chat_errcode;
// 			data.errorCode = (short int)enSocialErr_PlayerNotOnline;
// 			data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
// 
// 			SendDataToClient(nIndex, &data, data.protocol.len + PROTOCOL_SIZE);			
		}
		
	}

	
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cTeamOperationResult(BYTE *pMsg)
{
	GetClientPlayer().GetTeamInfo().ProcessTeamOperation((TEAM_OPERATION_RESULT*)pMsg);
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cUpdateTeamMemberInfo(BYTE* pMsg)
{
	GetClientPlayer().GetTeamInfo().UpdateTeamMemberBasicInfo((UPDATE_TEAM_MEMBER_INFO*)pMsg);
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cPrompt(BYTE *pMsg)
{
	SERVER_PROMPT* pPrompt = (SERVER_PROMPT*)pMsg;
	switch(pPrompt->Event)
	{
	case prompt_event_add_buff:
		{
			int buffId = pPrompt->Param[0];
			char buffSender[MAXSIZE_ROLENAME] = { 0 };
			strncpy(buffSender, pPrompt->StrParam, sizeof(buffSender));
			buffSender[sizeof(buffSender) - 1] = 0;

			PBAT pBuffTemplate = BuffTable::Singleton().GetBuff(buffId);
			if (pBuffTemplate)
			{
				char addBuffPromptMsg[COMMON_CLIENT_MSG_LEN_512] = { 0 };

				switch(pBuffTemplate->nDelayAddType)
				{
				case delay_add_buff_type_transfer://传送类BUFF，显示“XXX要把你传送到XXX”，BUFF描述应该写成“%s要把你传送到%s”
					{
						int mapId = pPrompt->Param[1];
						char mapName[32] = { 0 };
						g_SubWorldSet.GetWorldNameFromID(mapId, mapName, sizeof(mapName));
						mapName[sizeof(mapName) - 1] = 0;
						
						snprintf(addBuffPromptMsg, sizeof(addBuffPromptMsg), pBuffTemplate->szDesc, buffSender, mapName);
					}
					break;

				case delay_add_buff_type_revive://复活类BUFF，显示“XXX要复活你”，BUFF描述应该写成“%s要复活你”
					{
						snprintf(addBuffPromptMsg, sizeof(addBuffPromptMsg), pBuffTemplate->szDesc, buffSender);
					}
					break;

				default://默认显示BUFF描述
					{
						snprintf(addBuffPromptMsg, sizeof(addBuffPromptMsg), pBuffTemplate->szDesc);
					}
					break;
				}

				CoreDataChanged(GDCNI_PROMPT_ADD_BUFF, (unsigned int)addBuffPromptMsg, 0);
			}
		}
		break;
	}
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cCancelPrompt(BYTE *pMsg)
{
	CANCEL_SERVER_PROMPT* pCancelPrompt = (CANCEL_SERVER_PROMPT*)pMsg;
	switch(pCancelPrompt->Event)
	{
	case prompt_event_add_buff:
		{
			CoreDataChanged(GDCNI_CANCEL_PROMPT_ADD_BUFF, 0, 0);
		}
		break;
	}
}
#endif

#ifdef _SERVER
void KProtocolProcess::c2sSelectSkill(int nIndex, BYTE *pMsg, int nSize)
{
	SELECT_SKILL* pSelectSkill = (SELECT_SKILL*)pMsg;
	CastSkillParam skillParam;
	skillParam.SkillId = pSelectSkill->SkillID;

	if (pSelectSkill->SkillParam1 == SKILL_SPT_TargetIndex)
	{
		int targetNpcIndex = Player[nIndex].FindAroundNpc((DWORD)pSelectSkill->SkillParam2);
		if (targetNpcIndex > 0)
		{
			skillParam.Param1 = SKILL_SPT_TargetIndex;
			skillParam.Param2 = targetNpcIndex;

			Player[nIndex].SetSelectedSkill(skillParam);
		}
	}
	else
	{
		skillParam.Param1 = pSelectSkill->SkillParam1;
		skillParam.Param2 = pSelectSkill->SkillParam2;

		Player[nIndex].SetSelectedSkill(skillParam);
	}
}

void KProtocolProcess::c2sFuryExplode(int nIndex,BYTE * pMsg,int nSize)
{
	if (IsValidPlayer(nIndex))
	{
        Player[nIndex].GetFurySys().FuryExplode();
	}//endif
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cNpcSyncToWorld(BYTE *pMsg)
{
	PNPC_SYNC_TO_WORLD pSyncWorld = PNPC_SYNC_TO_WORLD(pMsg);

	int index = NpcSet.GetSyncToWorldNpcIndex(pSyncWorld->NpcId);
	if (index < 0)
	{
		REQUEST_NPC_SYNC_TO_WORLD requestNpc;
		requestNpc.Protocol = c2s_request_sync_to_world_npc;
		requestNpc.NpcId = pSyncWorld->NpcId;

		if (g_pClient)
		{
			g_pClient->SendPackToServer(g_ConnectID, &requestNpc, sizeof(requestNpc));
		}

		return;
	}

	SyncToWorldNpcInfo* pInfo = NpcSet.GetSyncToWorldNpcInfo(index);
	memcpy(pInfo->Params, pSyncWorld->Param, sizeof(pInfo->Params));
	pInfo->PosX = pSyncWorld->PosX;
	pInfo->PosY = pSyncWorld->PosY;
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cNpcSyncToWorldMin(BYTE *pMsg)
{
	PNPC_SYNC_TO_WORLD_MIN pSyncWorldMin = PNPC_SYNC_TO_WORLD_MIN(pMsg);

	if (NpcSet.ExistSyncToWorldNpc(pSyncWorldMin->NpcId))
		return;

	SyncToWorldNpcInfo info;
	memset(&info, 0, sizeof(info));
	info.Mode = pSyncWorldMin->Mode;
	info.NpcId = pSyncWorldMin->NpcId;
	strncpy(info.Name, pSyncWorldMin->Name, sizeof(info.Name));
	memcpy(info.Params, pSyncWorldMin->Param, sizeof(info.Params));
	info.PosX = pSyncWorldMin->PosX;
	info.PosY = pSyncWorldMin->PosY;

	NpcSet.AddSyncToWorldNpc(info);
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cNpcSyncToWorldDel(BYTE *pMsg)
{
	PNPC_SYNC_TO_WORLD_DEL pSyncWorldDel = PNPC_SYNC_TO_WORLD_DEL(pMsg);

	int listIndex = NpcSet.GetSyncToWorldNpcIndex(pSyncWorldDel->NpcId);
	if (listIndex >= 0)
	{
		NpcSet.RemoveSyncToWorldNpc(listIndex);
	}
}
#endif

#ifdef _SERVER
void KProtocolProcess::c2sRequestSyncToWorldNpc(int nIndex, BYTE * pMsg, int nSize)
{
	PREQUEST_NPC_SYNC_TO_WORLD requestNpc = (PREQUEST_NPC_SYNC_TO_WORLD)pMsg;

	int findNpcIndex = NpcSet.SearchID(requestNpc->NpcId);
	if (!IsValidNpc(findNpcIndex))
		return;
	
	Npc[findNpcIndex].SyncToWorldMin(Player[nIndex].GetNetConnectIdx());
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cListTeam(BYTE *pMsg)
{
	PTEAM_LIST pTeamList = (PTEAM_LIST)pMsg;

	int teamCount = (pTeamList->ProtocolSize + 1 - (sizeof(TEAM_LIST) - sizeof(TeamBasicInfo))) / sizeof(TeamBasicInfo);
	TeamBasicInfo* pTeamInfoArray = pTeamList->TeamList;

	CoreDataChanged(GDCNI_UPDATE_LIST_TEAM, (unsigned int)pTeamInfoArray, teamCount);
}

void KProtocolProcess::s2cSpecialQuestData(BYTE *pMsg)
{
	PSPECIAL_QUEST_DATA data = (PSPECIAL_QUEST_DATA)pMsg;

	CoreDataChanged(GDCNI_SPECIAL_QUEST_DATA, (unsigned int)&data->Data, data->QuestId);
}
#endif

#ifdef _SERVER
void KProtocolProcess::c2sRequestTeamList(int nIndex, BYTE *pMsg, int nSize)
{
	KPlayer& player = Player[nIndex];

	//请求过频繁
	if (player.m_NextTeamTime > UNIX_TMIE_STAMP)
		return;

	//设置下次可以请求的时间
	int requestInterval = ConfigManager::Singleton().GetGlobalVariable(global_var_request_team_list_interval);
	requestInterval = requestInterval > 1 ? requestInterval : 1;
	player.m_NextTeamTime = UNIX_TMIE_STAMP + requestInterval;

	PREQUEST_TEAM_LIST pRequestTeamList = (PREQUEST_TEAM_LIST)pMsg;
	int limit = pRequestTeamList->Limit < MAX_TEAM_BASIC_INFO_LIST_SIZE ? pRequestTeamList->Limit : MAX_TEAM_BASIC_INFO_LIST_SIZE;
	enumListTeamMode listMode = (enumListTeamMode)pRequestTeamList->ListMode;
	int filterType = pRequestTeamList->Filter;

	char buff[sizeof(TEAM_LIST) + MAX_TEAM_BASIC_INFO_LIST_SIZE * sizeof(TeamBasicInfo)];
	PTEAM_LIST pTeamList = (PTEAM_LIST)buff;	

	int listCount = 0;
	switch(filterType)
	{
	case 0:
		{
			int worldTemplateId = pRequestTeamList->FilterParam;//Npc[Player[nIndex].GetNpcIndex()].GetSubWorldIndex();
			listCount = g_TeamSet.ListTeam(pTeamList->TeamList, limit, listMode, &KTeamSet::InSubworld, &worldTemplateId);
		}
		break;
	default:
		break;
	}

	pTeamList->Protocol = s2c_list_team;
	pTeamList->ProtocolSize = sizeof(TEAM_LIST) - 1 + (listCount - 1) * sizeof(TeamBasicInfo);

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(player.GetNetConnectIdx(), pTeamList, pTeamList->ProtocolSize + 1);
}

void KProtocolProcess::c2sReqSpecialQuestData(int nIndex, BYTE *pMsg, int nSize)
{
	PREQ_SPECIAL_QUEST_DATA req = (PREQ_SPECIAL_QUEST_DATA)pMsg;

	KPlayer& player = Player[nIndex];
	player.ExecuteScript2Param(player.m_dwDeathScriptId, "QueryQuestBible",0, req->QuestId, 0);

}

void KProtocolProcess::c2sCMCommunication(int nIndex, BYTE *pMsg, int nSize)
{
	PGM_COMMUNICATION_DATA questionData = (PGM_COMMUNICATION_DATA)pMsg;

	KPlayer& player = Player[nIndex];

	//60秒才能提交一次问题
	if (!player.CanDoOp(player_op_commit_question))
		return;
	player.UpdateOpTime(player_op_commit_question, 60);

	questionData->data.msg[sizeof(questionData->data.msg) - 1] = 0;

	//不接受文字小于32个子节的请求
	if(strlen(questionData->data.msg) < 32)
	{
		return;
	}

	int dbRet = player.RecordPlayerQuestion( questionData->data.type, questionData->data.msg );
}

void KProtocolProcess::c2sReqTobeHired(int nIndex, BYTE *pMsg, int nSize)
{
	PREQ_TO_BE_HIRED_DATA reqData = (PREQ_TO_BE_HIRED_DATA)pMsg;

	switch(reqData->hireType)
	{
	case HT_EXP:
		{
			EmployCenter::Singleton().Post(nIndex, reqData->hireType, 0, reqData->moneyOrExpType);
		}
		break;
	case HT_FIGHT:
		{
			EmployCenter::Singleton().Post(nIndex, reqData->hireType, reqData->moneyOrExpType, 0);
		}
		break;
	}
}

void KProtocolProcess::c2sReqHireList(int nIndex, BYTE *pMsg, int nSize)
{
	PREQ_HIRE_LIST_DATA reqData = (PREQ_HIRE_LIST_DATA)pMsg;

	if (!IsValidPlayer(nIndex))
		return;
	
	if (Player[nIndex].GetUIServerState().GetUIState(player_ui_employ) != player_ui_state_open)
		return;

	reqData->filter.name[sizeof(reqData->filter.name) - 1] = 0;
	KPlayer& player = Player[nIndex];

	EmployCenter::Singleton().SearchEmployee(nIndex, reqData->filter);

// 	//-------test-------
// 	if(reqData->filter.hiretype == HT_EXP)
// 	{
// 		EXP_HIRE_LIST hireList;
// 		hireList.Protocol = s2c_hire_data_list_exp;
// 		hireList.data[0].level = 8;
// 		hireList.data[0].metier.major = 1;
// 		hireList.data[0].metier.minor = -1;
// 		strcpy(hireList.data[0].name, "谢鉷");
// 		strcpy(hireList.data[0].shizu, "wu氏族");
// 		strcpy(hireList.data[0].zhuhou, "wu诸侯");
// 
// 		if (g_pServer != NULL)
// 			g_pServer->PackDataToClient(player.GetNetConnectIdx(), &hireList, sizeof(hireList));
// 	}
// 	else
// 	{
// 		FIGHTER_HIRE_LIST hireList;
// 		hireList.Protocol = s2c_hire_data_list_fighter;
// 		hireList.data[0].level = 8;
// 		hireList.data[0].metier.major = 1;
// 		hireList.data[0].metier.minor = -1;
// 		hireList.data[0].armor = 1000;
// 		hireList.data[0].attack.high = 2000;
// 		hireList.data[0].attack.low = 1000;
// 		hireList.data[0].magic.high = 200;
// 		hireList.data[0].magic.low = 100;
// 		hireList.data[0].blood = 1;
// 		hireList.data[0].salary = 10000;
// 
// 		if (g_pServer != NULL)
// 			g_pServer->PackDataToClient(player.GetNetConnectIdx(), &hireList, sizeof(hireList));
// 	}
// 	//-------test--end--
}

void KProtocolProcess::c2sReqHire(int nIndex, BYTE *pMsg, int nSize)
{
	PREQ_HIRE_DATA reqData = (PREQ_HIRE_DATA)pMsg;

	if (!IsValidPlayer(nIndex))
		return;
	
	if (Player[nIndex].GetUIServerState().GetUIState(player_ui_employ) != player_ui_state_open)
		return;

	EmployCenter::Singleton().Employ(nIndex, reqData->Name);
}

void KProtocolProcess::c2sInteractiveScriptInput(int nIndex, BYTE *pMsg, int nSize)
{
	PINTERACTIVE_SCRIPT_INPUT pInput = (PINTERACTIVE_SCRIPT_INPUT)pMsg;

	if (!IsValidPlayer(nIndex))
		return;

	pInput->Input[sizeof(pInput->Input) - 1] = 0;
	Player[nIndex].SetInteractiveScriptParam(0, pInput->Input);
	Player[nIndex].InteractiveScriptNextStep();
}

#endif

#ifndef _SERVER
void KProtocolProcess::s2cListStudent(BYTE * pMsg)
{
	char receiveBuff[MAX_LIST_STUDENT_BUFF_LENGTH];
	if (!DecompressProtocol(pMsg, (BYTE*)receiveBuff, sizeof(receiveBuff)))
		return;

	LIST_STUDENT* pListStudent = (LIST_STUDENT*)receiveBuff;
	int studentCount = pListStudent->StudentCount;
	StudentInfo* pStudentInfo = pListStudent->StudentInfoData;

	//TODO 更新并显示界面
	RecommedList list;
	for(int i = 0; i < studentCount; ++i)
	{
		StudentInfo& student = pStudentInfo[i];
		student.Name[sizeof(pStudentInfo->Name) - 1] = 0;
		
		RecommendItem item;
		int len = strlen(student.Name) < sizeof(item.Name) ? strlen(student.Name) : sizeof(item.Name) - 1;
		strncpy(item.Name, student.Name, len);
		item.Name[len] = 0;

		item.Level = student.Level;
		item.RewardMoeny = student.RewardMoeny;
		item.TotalRewardMoney = student.TotalRewardMoney;
		
		list.push_back(item);
	}

	RecommendRewardInfo rewardInfo;
	rewardInfo.TotalRewardTicketAdded = pListStudent->TotalRewardTicketAdded;
	rewardInfo.TotalRewardToAdd = pListStudent->TotalRewardToAdd;

	CoreDataChanged(GDCNI_LIST_STUDENT, (UINT)&list, (int)&(rewardInfo) );
}

void KProtocolProcess::s2cInsurance(BYTE * pMsg)
{
	GetClientPlayer().m_InsuranceMgr.ProcessProcotol(pMsg);
}

#define  MAX_UNCOMPRESSED_BUFF_SIZE   5 * 1024
void KProtocolProcess::s2cWorldPlayerSync(BYTE * pMsg)
{
	//Begin Decompress........................................................................
	tagExtendProtoHeader * pHeader         = (tagExtendProtoHeader *)pMsg;
	WORD            wOldLen                =  pHeader->wLength;

	char UnCompressedBuff[MAX_UNCOMPRESSED_BUFF_SIZE];

	memset(UnCompressedBuff, 0, sizeof(UnCompressedBuff));
	memcpy(UnCompressedBuff, pMsg, 3); // copy header only

	const BYTE    * pCompressedBuff   = pMsg + 3;
	unsigned int compressedBuffLength = wOldLen + 1 - 3;

	BYTE* pDecompressBuff = (BYTE *)UnCompressedBuff + 3;
	unsigned int realDecompressBuffLength = MAX_UNCOMPRESSED_BUFF_SIZE - 3;
	lzo1x_decompress(
		pCompressedBuff,
		compressedBuffLength,
		pDecompressBuff,
		&realDecompressBuffLength,
		NULL);
	
    pHeader         = (tagExtendProtoHeader *)UnCompressedBuff;
	pHeader->wLength = realDecompressBuffLength + 3 - 1;

	//End Decompress ......................................................................
	S2C_WORLD_PLAYER_INFO_SYNC * pSync = (S2C_WORLD_PLAYER_INFO_SYNC *)UnCompressedBuff;

	SubWorld[0].RefreshWorldMapPlayerInfoCache(pSync->InfoNum,(WORLD_PLAYER_INFO *)pSync->Infos);

}

void KProtocolProcess::s2cWarCommanderSync(BYTE * pMsg)
{
	char receiveBuff[(MAX_WAR_COMMANDER_NUM + 2) * sizeof(WAR_COMMANDER_INFO) + sizeof(S2C_WAR_COMMANDER_INFO_SYNC)];
	if (!DecompressProtocol(pMsg, (BYTE*)receiveBuff, sizeof(receiveBuff)))
		return;
	
	//End Decompress ......................................................................
	S2C_WAR_COMMANDER_INFO_SYNC * pSync = (S2C_WAR_COMMANDER_INFO_SYNC *)receiveBuff;
	
	SubWorld[0].RefreshWarCommanderInfoCache(pSync->InfoNum,(WAR_COMMANDER_INFO *)pSync->Infos);
}
#define RECV_BUFF_MAX 8 * 1024
void KProtocolProcess::s2cPlusPointTopN(BYTE * pMsg)
{
}
#endif

#ifdef _SERVER

void KProtocolProcess::c2sRecommender(int nIndex, BYTE *pMsg, int nSize)
{
	if (!IsValidPlayer(nIndex))
		return;

	KPlayer& player = Player[nIndex];


	PRECOMMENDER_OP pRecommenderOp = (PRECOMMENDER_OP)pMsg;
	switch (pRecommenderOp->OpType)
	{
	case recommender_op_update_student:
		{
			if (player.GetUIServerState().GetUIState(player_ui_recommender_master) != player_ui_state_open)
				return;

			player.UpdateStudent();
		}		
		break;
	case recommender_op_get_master_reward:
		{
			if (player.GetUIServerState().GetUIState(player_ui_recommender_strudent) != player_ui_state_open)
				return;

			if (!player.CanDoOp(player_op_get_master_reward))
				return;
			player.UpdateOpTime(player_op_get_master_reward, DEFAULT_RECOMMENDER_OP_INTERVAL);

			int addTicket = player.GetRecommenderRewardTicket();

			player.ShowPredefinedMsg(addTicket > 0 ? 11357 : 11358);
		}
		break;
	}
}

void KProtocolProcess::c2sInsurance(int nIndex,BYTE * pMsg,int nSize)
{
	if (!IsValidPlayer(nIndex))
		return ;
	Player[nIndex].m_InsuranceMgr.ProcessProcotol(pMsg,nSize);
}

void KProtocolProcess::c2sPlusPointTopN(int nIndex , BYTE * pMsg,int nSize)
{
}

#endif

#ifndef _SERVER
void KProtocolProcess::s2cWorldCombatTop10Info(BYTE * pMsg)
{
	char receiveBuff[(MAX_COMBAT_TOP - 1) * sizeof(COMBAT_TOP10_MEMBER_INFO) + sizeof(COMBAT_TOP10_INFO)];
	if (!DecompressProtocol(pMsg, (BYTE*)receiveBuff, sizeof(receiveBuff)))
		return;

	COMBAT_TOP10_INFO *pSyncComBatTopInfo = (COMBAT_TOP10_INFO*)receiveBuff;
	int memberCount = pSyncComBatTopInfo->MemberCount;

	UICombatTopMemberInfo uiMemberInfo[MAX_COMBAT_TOP];
	for (int memberIndex = 0; memberIndex < memberCount; memberIndex++)
	{
		COMBAT_TOP10_MEMBER_INFO& memberInfo = pSyncComBatTopInfo->Member[memberIndex];

		strncpy(uiMemberInfo[memberIndex].Name, memberInfo.Name, sizeof(uiMemberInfo[memberIndex].Name));
		uiMemberInfo[memberIndex].Name[sizeof(uiMemberInfo[memberIndex].Name) - 1] = 0;
		uiMemberInfo[memberIndex].Level = memberInfo.Level;
		BYTE classInfo = memberInfo.ClassAndSexInfo & 0x0f;
		BYTE sexInfo = memberInfo.ClassAndSexInfo >> 4;
		uiMemberInfo[memberIndex].SkillSeries = classInfo / 3 - 1;
		uiMemberInfo[memberIndex].Class = classInfo % 3;
		uiMemberInfo[memberIndex].Sex = sexInfo;
		uiMemberInfo[memberIndex].Score = memberInfo.Score;
	}

	CoreDataChanged(GDCNI_COMBAT_TOP_MEMBER_INFO, (unsigned int)uiMemberInfo, memberCount);
}

void KProtocolProcess::s2cPlayerRealInfoSync(BYTE * pMsg)
{
	Player[CLIENT_PLAYER_INDEX].m_clientPRIMgr.ProtocolProcess(pMsg);
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cWorldCustomString(BYTE * pMsg)
{
	char receiveBuff[WORLD_CUSTOM_STRING_PROTOCOL_BUFF];
	if (!DecompressProtocol(pMsg, (BYTE*)receiveBuff, sizeof(receiveBuff)))
		return;
	
	WORLD_CUSTOME_STRING *pProtocol = (WORLD_CUSTOME_STRING*)receiveBuff;
	pProtocol->CustomString[MAX_WORLD_CUSTOM_STRING_LENGTH - 1] = 0;
	int length = strlen(pProtocol->CustomString);

	CoreDataChanged(GDCNI_MAP_PRONUNCIAMENTO_STR, (unsigned int)(pProtocol->CustomString), length);
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cChangeTitle(BYTE * pMsg)
{
	PCHANGE_TITLE changeTitle = (PCHANGE_TITLE)pMsg;
	int npcIndex = NpcSet.SearchID(changeTitle->NpcID);
	if (IsValidNpc(npcIndex))
	{
		Npc[npcIndex].SetTitle(changeTitle->TitleIndex, changeTitle->TitleLevel);

		if (GetClientPlayer().GetNpcIndex() == npcIndex)
		{
			CoreDataChanged(GDCNI_TITLEINFO_UPDATA, 0, 0);
		}
	}
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cUpdateSelfTitle(BYTE * pMsg)
{
	Player[CLIENT_PLAYER_INDEX].GetTitleManager().ReceiveUpdateTitle(pMsg);
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cSyncSelfTitle(BYTE * pMsg)
{
	Player[CLIENT_PLAYER_INDEX].GetTitleManager().ReceiveSyncData(pMsg);
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cSelectTitleResult(BYTE * pMsg)
{
	Player[CLIENT_PLAYER_INDEX].GetTitleManager().ReceiveSelectTitleResult(pMsg);
}
#endif

#ifdef _SERVER
void KProtocolProcess::c2sSelectTitle(int nIndex, BYTE * pMsg, int nSize)
{
	if (!IsValidPlayer(nIndex))
		return;

	if (!Player[nIndex].CanDoOp(player_op_select_title))
		return;
	Player[nIndex].UpdateOpTime(player_op_select_title, DEFAULT_SELECT_TITLE_INTERVAL);

	PSELECT_TITLE pSelectTitle = (PSELECT_TITLE)pMsg;
	TitleManager& titleManager = Player[nIndex].GetTitleManager();

	if (pSelectTitle->TitleIndex == RANDOM_SELECT_TITLE_FALG)
	{
		titleManager.SetRandomSelectTitle(true);
		titleManager.SyncSelectTitleResult();
	}
	else if (pSelectTitle->TitleIndex >= 0 && pSelectTitle->TitleIndex <= MAX_TITLE_COUNT)
	{
		titleManager.SetRandomSelectTitle(false);
		titleManager.SelectTitle(pSelectTitle->TitleIndex);
		titleManager.SyncSelectTitleResult();
	}
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cShizuPopularityTopN(BYTE * pMsg)
{
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cZhuhouPopularityTopN(BYTE * pMsg)
{
}
#endif

#ifndef _SERVER
void KProtocolProcess::s2cPlayerProperties(BYTE * pMsg)
{
}
#endif
