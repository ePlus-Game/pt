//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-6-12 10:23
//      File_base        : KProtocol
//      File_ext         : cpp
//      Author           : 
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"

#include "KEngine.h"
#include "KProtocol.h"
#include "KPlayer.h"
#include "KItemList.h"
#ifndef _SERVER
#include "CoreShell.h"
#endif

#include "common_fury_def.h"

int	g_nProtocolSize[MAX_PROTOCOL_NUM] = 
{
#ifndef _SERVER				// 客户端接收到的服务器到客户端的协议长度
	-1,							// s2c_login,
	-1,							// s2c_rolelist
	sizeof(tagNewDelRoleResponse),// s2c_rolenewdelresponse	
	sizeof(BYTE),				// s2c_syncend,
	sizeof(CURPLAYER_SYNC),		// s2c_synccurplayer,
	-1,							// s2c_synccurplayerskill
	sizeof(CURPLAYER_NORMAL_SYNC),// s2c_synccurplayernormal
	sizeof(WORLD_SYNC),			// s2c_syncworld,
	sizeof(PLAYER_SYNC),		// s2c_syncplayer,
	sizeof(PLAYER_NORMAL_SYNC),	// s2c_syncplayermin,
	-1,	//sizeof(NPC_SYNC),			// s2c_syncnpc,
	sizeof(NPC_NORMAL_SYNC),	// s2c_syncnpcmin,
	sizeof(NPC_PLAYER_TYPE_NORMAL_SYNC),	// s2c_syncnpcminplayer,
	-1,//sizeof(OBJ_ADD_SYNC),	// s2c_objadd,
	sizeof(OBJ_SYNC_STATE),		// s2c_syncobjstate,
	sizeof(OBJ_SYNC_DIR),		// s2c_syncobjdir,
	sizeof(OBJ_SYNC_REMOVE),	// s2c_objremove,
	sizeof(OBJ_SYNC_TRAP_ACT),	// s2c_objTrapAct,
	sizeof(NPC_REMOVE_SYNC),	// s2c_npcremove,
	sizeof(NPC_WALK_SYNC),		// s2c_npcwalk,
	sizeof(NPC_RUN_SYNC),		// s2c_npcrun,
	sizeof(NPC_HURT_SYNC),		// s2c_npchurt,
	sizeof(NPC_DEATH_SYNC),		// s2c_npcdeath,
	sizeof(NPC_SKILL_SYNC),		// s2c_skillcast,
	-1,	// s2c_teamselfinfo,
	sizeof(PLAYER_LEAVE_TEAM),				// s2c_teamleave,
	sizeof(PLAYER_LEVEL_UP_SYNC),			// s2c_playerlevelup
	sizeof(PLAYER_SKILLINFO_SYNC),			// s2c_playerskillinfo
	-1,										// s2c_syncitem
	sizeof(ITEM_REMOVE_SYNC),				// s2c_removeitem
	sizeof(PLAYER_MONEY_SYNC),				// s2c_syncmoney
	sizeof(PLAYER_MOVE_ITEM_SYNC),			// s2c_playermoveitem
	-1,										// s2c_playershowui
	sizeof(TRADE_CHANGE_STATE_SYNC),		// s2c_tradechangestate
	sizeof(TRADE_MONEY_SYNC),				// s2c_trademoneysync
	sizeof(TRADE_DECISION_SYNC),			// s2c_tradedecision
	-1,	// sizeof(TEAM_INVITE_ADD_SYNC)		   s2c_teaminviteadd
	sizeof(PING_COMMAND),					// s2c_ping
	sizeof(SALE_BOX_SYNC),					// s2c_opensalebox
	sizeof(OPENSTOREBOX),					// s2c_openstorebox
	sizeof(NPC_REVIVE_SYNC),				// s2c_playerrevive
	sizeof(NPC_REQUEST_FAIL),				// s2c_requestnpcfail
	sizeof(TRADE_APPLY_START_SYNC),			// s2c_tradeapplystart
	-1,		                             	// s2c_viewequip
	sizeof(ENCHASER_SERVERRESULT),			// s2c_enchaseritemresult
	-1,										// s2c_refreshitem
	sizeof(REQUESTREPLY),					// s2c_checkstoragepswok
	sizeof(REQUESTREPLY),					// s2c_createstoragepswok
	sizeof(REQUESTREPLY),					// s2c_modifystoragepswok
	sizeof(TEAMMATE_INFO),					// s2c_sync_teammemberlife
	sizeof(CREATURE_SYNC),					// s2c_sync_creature
	-1,										// s2c_byte_extend
	-1,										// s2c_pet
	sizeof(FINDPATHSYNC),					// s2c_findpathsync
 	sizeof(DAMAGESHOW),
	sizeof(NPCREALPOSITION),				// s2c_npcrealposition
	-1,										// s2c_chat_family
	-1,										// s2c_buff_family
	sizeof(PLAYER_SKILLSERIES_SYNC),		// s2c_sync_skillseries
	-1,										// s2c_sync_npcattr
	sizeof(SYNC_PLAYERATTR),				// s2c_sync_playerattr
	sizeof(SYNC_ITEM_ATTR),					// s2c_sync_item_attr
	-1,										// s2c_auction_family
	-1,										// s2c_social_family
	-1,                          			// s2c_social_relation
	sizeof(DELAYED_ACTION),					// s2c_delayed_action
	sizeof(TALISMAN_OPERATION),				// s2c_talisman_family
	sizeof(SYNC_TALISMAN_ENCHASE),			// s2c_sync_talisman_enchase
	sizeof(SYNC_NPC_EQUIP_TALISMAN),		// s2c_sync_npc_equip_talisman
	-1,										// s2c_quest_family
	-1,										// s2c_team_invite_refuse
	sizeof(SHOW_PREDEFINED_MSG),			// s2c_show_predefined_msg
	-1,										// s2c_IB_family
	-1,										// s2c_show_banner
	sizeof(FindResult),						// s2c_find_family
	-1,                                     // s2c_taisui_wheel
	sizeof(TEAM_OPERATION_RESULT),			// s2c_team_operation_result
	sizeof(UPDATE_TEAM_MEMBER_INFO),		// s2c_update_team_member_info
	-1,										// s2c_prompt
	sizeof(CANCEL_SERVER_PROMPT),			// s2c_cancel_prompt
	sizeof(S2C_PLAYER_STOP),                // s2c_stop
	sizeof(S2C_POS_EDITION),                // s2c_pos_edition
	sizeof(S2C_FURY_SYNC),
	-1,										// s2c_apply_join_team
	sizeof(NPC_SYNC_TO_WORLD),				// s2c_npc_sync_to_world
	sizeof(NPC_SYNC_TO_WORLD_MIN),			// s2c_npc_sync_to_world_min
	sizeof(NPC_SYNC_TO_WORLD_DEL),			// s2c_npc_sync_to_world_del
	-1,										// s2c_team_list
	sizeof(SPECIAL_QUEST_DATA),				// s2c_special_quest_data
	sizeof(SHOW_BANNER_ID),					// s2c_show_banner_id
	-1,										// s2c_gm_feedback_msg
	-1,										// s2c_hire_data_list_exp		//雇佣——begin
	-1,										// s2c_hire_data_list_fighter
	sizeof(HIRE_RET_CODE),					// s2c_hire_ret_code			//雇佣——end
	sizeof(WORLD_COMBAT_INFO),              // s2c_world_combat_info        //积分同步
	sizeof(NPC_INLAYCOUNT_SYNC),
	sizeof(NPC_COMMO_FLAG),                 // s2c_npc_comoflag
	-1,										//s2c_world_combat_top10_info
	-1,										// s2c_list_student
	-1,                                     // s2c_insurance
	-1,                                     // s2c_world_player_info_sync
	-1,										// s2c_war_commander_info_sync
	-1,										// s2c_world_custom_string
	sizeof(CHANGE_TITLE),					// s2c_change_title
	sizeof(UPDATE_SELF_TITLE),				// s2c_update_self_title
	-1,										// s2c_sync_self_title
	sizeof(SELECT_TITLE),					// s2c_select_title_result
	-1,                                     // s2c_plus_point_top_n
	-1,										// s2c_shizu_popularity_top_n
	-1,										// s2c_zhuhou_popularity_top_n
	sizeof(PLAYER_PROPERTIES),				// s2c_player_properties
	sizeof(CURPLAYER_NORMAL_SYNC_EX),		//s2c_curplayer_sync_ex
	-1,										// s2c_player_real_info
#else
	sizeof(LOGIN_COMMAND),		//	c2s_login,
	sizeof(tagDBSelPlayer),		// c2s_dbplayerselect
	sizeof(BYTE),				//	c2s_syncend,
	-1,	// sizeof(NEW_PLAYER_COMMAND)//	c2s_newplayer,
	-1,							//	c2s_removeplayer,
	-1,// c2s_no_employ
	-1,// c2s_get_answer
	sizeof(NPC_REQUEST_COMMAND),//	c2s_requestnpc,
	sizeof(OBJ_CLIENT_SYNC_ADD),//	c2s_requestobj,
	sizeof(NPC_RUN_COMMAND),	//	c2s_npcrun,
	sizeof(NPC_SKILL_COMMAND),	//	c2s_npcskill,
	sizeof(PLAYER_APPLY_TEAM_INFO),				// c2s_teamapplyinfo,
	sizeof(PLAYER_EAT_ITEM_COMMAND),			// c2s_playereatitem
	sizeof(PLAYER_PICKUP_ITEM_COMMAND),			// c2s_playerpickupitem
	sizeof(PLAYER_MOVE_ITEM_COMMAND),			// c2s_playermoveitem
	sizeof(PLAYER_SELL_ITEM_COMMAND),			// c2s_sellitem
	sizeof(PLAYER_BUY_ITEM_COMMAND),			// c2s_buyitem
	sizeof(PLAYER_THROW_AWAY_ITEM_COMMAND),		// c2s_playerthrowawayitem
	-1,											// c2s_playerselui,
	sizeof(TRADE_APPLY_START_COMMAND),			// c2s_tradeapplystart
	sizeof(TRADE_MOVE_MONEY_COMMAND),			// c2s_trademovemoney
	sizeof(TRADE_DECISION_COMMAND),				// c2s_tradedecision
	sizeof(PLAYER_DIALOG_NPC_COMMAND),			// c2s_dialognpc
	sizeof(TEAM_INVITE_ADD_COMMAND),			// c2s_teaminviteadd
	sizeof(TEAM_REPLY_INVITE_COMMAND),			// c2s_teamreplyinvite
	sizeof(PING_CLIENTREPLY_COMMAND),			// c2s_ping
	sizeof(OBJ_MOUSE_CLICK_SYNC),				// c2s_objmouseclick
	sizeof(STORE_MONEY_COMMAND),				// c2s_storemoney
	sizeof(NPC_REVIVE_COMMAND),					// c2s_playerrevive
	sizeof(TRADE_REPLY_START_COMMAND),			// c2s_tradereplystart
	sizeof(VIEW_EQUIP_COMMAND),					// c2s_viewequip
	sizeof(ITEM_REPAIR),						// c2s_repairitem
	sizeof(ENCHASER_CLIENTSEND),				// c2s_enchaseritem
	sizeof(SPLITPILEITEM),						// c2s_splitpileitem
	sizeof(CHECKSTORAGEPSW),					// c2s_checkstoragepassword
	sizeof(CREATESTORAGEPSW),					// c2s_createstoragepassword
	sizeof(MODIFYSTORAGEPSW),					// c2s_modifystoragepassword
	sizeof(BYTE),								// c2s_closestorage
	-1,											// c2s_byte_extend
	-1,											// c2s_pet
	-1,											// c2s_chat_family
	-1,											// c2s_buff_family
	sizeof(Chg_PK_Mode),						// c2s_chg_pkmode
	sizeof(PLAYER_SKILLINFO_SYNC),				// c2s_skill_sync
	-1,											// c2s_auction_family
	sizeof(PLAYER_STOP_NOTIFY),					// c2s_playerstop
	-1,											// c2s_social_family
	sizeof(TALISMAN_OPERATION),					// c2s_talisman_family
	-1,											// c2s_quest_family
	sizeof(PLAYER_LOGOUT),						// c2s_player_logout
	-1,											// c2s_IB_family
	sizeof(FindParam),							// c2s_find_family
	-1,											// c2s_taisui_wheel
	sizeof(TEAM_OPERATION),						// c2s_team_operation
	sizeof(CLIENT_REPLY_PROMPT),			    // c2s_reply_prompt
	sizeof(C2S_POS_SYNC),					    // c2s_pos_sync    
	sizeof(SELECT_SKILL),						// c2s_select_skill
    sizeof(C2S_FURY_EXPLODE),					// c2s_fury_explode
	sizeof(REQUEST_NPC_SYNC_TO_WORLD),			// c2s_request_sync_to_world_npc
	sizeof(REQUEST_TEAM_LIST),					// c2s_list_team
	sizeof(REQ_SPECIAL_QUEST_DATA),				// c2s_req_special_quest_data
	sizeof(GM_COMMUNICATION_DATA),				// c2s_cm_communication
	sizeof(REQ_TO_BE_HIRED_DATA),				// c2s_hire_req_tobe_hired	//雇佣——begin
	sizeof(REQ_HIRE_LIST_DATA),					// c2s_hire_req_list
	sizeof(REQ_HIRE_DATA),						// c2s_hire_req_hire		//雇佣——end
	sizeof(INTERACTIVE_SCRIPT_INPUT),			// c2s_interactive_script_input
	sizeof(RECOMMENDER_OP),						// c2s_recommender
	-1,                                         // c2s_insurance
	sizeof(SELECT_TITLE),						// c2s_select_title
	-1,                                         // c2s_plus_point_top_n
	-1,											// c2s_player_real_info_sync
#endif
};

void g_InitProtocol()
{
#ifdef _SERVER
	g_nProtocolSize[c2s_extend - c2s_gameserverbegin - 1] = -1;
	g_nProtocolSize[c2s_extendchat - c2s_gameserverbegin - 1] = -1;
	g_nProtocolSize[c2s_extendfriend - c2s_gameserverbegin - 1] = -1;
#else
	g_nProtocolSize[s2c_extend - s2c_clientbegin - 1] = -1;
	g_nProtocolSize[s2c_extendchat - s2c_clientbegin - 1] = -1;
	g_nProtocolSize[s2c_extendfriend - s2c_clientbegin - 1] = -1;
#endif
}

#ifndef _SERVER
#include "networkinterface.h"
#include "KCore.h"
#include "KSubWorld.h"

void SendClientCmdRun(int nX, int nY)
{
		NPC_RUN_COMMAND	NetCommand;
		
		NetCommand.ProtocolType = (BYTE)c2s_npcrun;
		NetCommand.nMpsX = nX;
		NetCommand.nMpsY = nY;

		Npc[GetClientPlayer().GetNpcIndex()].GetMpsPos(&NetCommand.nCurMpsX,&NetCommand.nCurMpsY);

		if (g_pClient)
			g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&NetCommand, sizeof(NetCommand));
}

void SendClientCmdSkill(int nSkillID, int nX, int nY)
{
	NPC_SKILL_COMMAND	NetCommand;
	
	NetCommand.ProtocolType = (BYTE)c2s_npcskill;
	NetCommand.nSkillID = nSkillID;
	NetCommand.nMpsX = nX;
	NetCommand.nMpsY = nY;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&NetCommand, sizeof(NPC_SKILL_COMMAND));	
}

void SendClientCmdRequestNpc(int nID)
{
	NPC_REQUEST_COMMAND NpcRequest;
	
	NpcRequest.ProtocolType = c2s_requestnpc;
	NpcRequest.ID = nID;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&NpcRequest, sizeof(NPC_REQUEST_COMMAND));

}

void SendClientCmdSell(int nId)
{
	PLAYER_SELL_ITEM_COMMAND PlayerSell;
	PlayerSell.ProtocolType = c2s_playersellitem;
	PlayerSell.m_ID = nId;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&PlayerSell, sizeof(PLAYER_SELL_ITEM_COMMAND));
}

void SendClientCmdBuy(int nBuyIdx, int buyCount)
{
	PLAYER_BUY_ITEM_COMMAND PlayerBuy;
	PlayerBuy.ProtocolType = c2s_playerbuyitem;
	PlayerBuy.m_BuyIdx = (BYTE)nBuyIdx;
	PlayerBuy.buyCount = buyCount;

	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&PlayerBuy, sizeof(PLAYER_BUY_ITEM_COMMAND));
}

void SendObjMouseClick(int nObjID, DWORD dwRegionID)
{
	OBJ_MOUSE_CLICK_SYNC	sObj;
	sObj.ProtocolType = c2s_objmouseclick;
	sObj.m_dwRegionID = dwRegionID;
	sObj.m_nObjID = nObjID;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&sObj, sizeof(OBJ_MOUSE_CLICK_SYNC));
}

void SendClientCmdStoreMoney(int nDir, int nMoney)
{
	STORE_MONEY_COMMAND	StoreMoneyCmd;

	StoreMoneyCmd.ProtocolType = c2s_storemoney;
	StoreMoneyCmd.m_byDir = (BYTE)nDir;
	StoreMoneyCmd.m_dwMoney = nMoney;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&StoreMoneyCmd, sizeof(STORE_MONEY_COMMAND));
}

void SendClientCmdRevive(int nReviveType)
{
	NPC_REVIVE_COMMAND	ReviveCmd;

	ReviveCmd.ProtocolType = c2s_playerrevive;
	ReviveCmd.ReviveType = nReviveType;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE *)&ReviveCmd, sizeof(NPC_REVIVE_COMMAND));
}

void SendClientCmdRepair(DWORD dwID, bool special)
{
	ITEM_REPAIR ItemRepair;
	ItemRepair.ProtocolType = c2s_repairitem;
	ItemRepair.dwItemID = dwID;
	ItemRepair.special = special;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&ItemRepair, sizeof(ITEM_REPAIR));
}
#endif

