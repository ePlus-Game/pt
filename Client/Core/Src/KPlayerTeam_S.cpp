//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/19/2006 17:19
//      File_base        : KPlayerTeam_S
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////



#include	"KCore.h"
#include	"KNpc.h"
#include	"KPlayer.h"
#include	"KPlayerSet.h"
#include	"KPlayerTeam_S.h"
#include	"KSubWorld.h"
#include	"ChatCenter_S.h"

KTeam		g_TeamS[MAX_TEAM];
KTeamSet	g_TeamSet;

KPlayerTeam::KPlayerTeam()
{
	Init(0);
}

void KPlayerTeam::Active()
{
	for (int i = 0; i < MAX_BIG_TEAM_MEMBER; i++)
	{
		InviteInfo& info = m_nInviteList[i];
		if (info.PlayerIndex > 0)
		{
			info.Timeout--;
			if (info.Timeout <= 0)
			{				
				if (IsValidPlayer(info.PlayerIndex))
					//组队邀请超时，自动拒绝
					ReplyInviteJoinTeam(info.PlayerIndex, invite_jointeam_replay_timeout);

				//从邀请列表中删除
				info.PlayerIndex = 0;
			}
		}
	}

	for (int applyIndex = 0; applyIndex < MAX_BIG_TEAM_MEMBER; applyIndex++)
	{
		InviteInfo& info = m_ApplyList[applyIndex];
		if (info.PlayerIndex > 0)
		{
			info.Timeout--;
			if (info.Timeout <= 0)
			{				
				if (IsValidPlayer(info.PlayerIndex))
					//申请加入队伍超时，自动拒绝
					ReplyApplyJoinTeam(info.PlayerIndex, false);

				info.PlayerIndex = 0;
			}
		}
	}
}

//---------------------------------------------------------------------------
//	功能：清空
//---------------------------------------------------------------------------
void KPlayerTeam::Init(int playerIndex)
{
	m_PlayerIndex = playerIndex;
	m_nID = INVALID_TEAM_ID;
	m_bCanTeamFlag = 0;
	m_TeamMemberType = teammember_type_normal;
	memset(m_nInviteList, 0, sizeof(m_nInviteList));
	memset(m_ApplyList, 0, sizeof(m_ApplyList));
	m_NeedSyncTeammateBuff = false;
	m_AutoAcceptApply = false;
}

bool KPlayerTeam::CreateTeam()
{
	KPlayer& selfPlayer = Player[m_PlayerIndex];
	
	if (IsInTeam())
	{
		SendOperationResult(team_operation_create, 0, -1);
		return false;
	}

	int newTeamId = g_TeamSet.CreateTeam(m_PlayerIndex);
	if (newTeamId >= 0)
	{
		m_nID = newTeamId;
		SetTeamMemberType(teammember_type_captain);
		g_ChatCenterS.s2cChannelOpe(m_PlayerIndex, TEAM_ROOM_ID, chat_addchannel, CHAT_CHANNEL_NAME_TEAM);

		SendOperationResult(team_operation_create, 0, newTeamId);
		SendSelfTeamInfo();
		return true;
	}
	else
	{
		SendOperationResult(team_operation_create, 0, -2);
		return false;
	}

	return false;
}

void KPlayerTeam::OpenBigTeamMode()
{
	if (CanOpenBigTeam())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && pTeam->CanOpenBigTeam())
		{
			if (!pTeam->IsBigTeam())
			{
				pTeam->OpenBigTeamMode();
				SendOperationResult(team_operation_open_big_team, 0, TRUE);
				return;
			}
		}
	}
	
	SendOperationResult(team_operation_open_big_team, 0, FALSE);
}

KTeam* KPlayerTeam::GetTeam()
{
	if (m_nID >= 0 && m_nID < MAX_TEAM)
		return &g_TeamS[m_nID];
	else
		return NULL;
}

void KPlayerTeam::ProcessTeamOperation(TEAM_OPERATION* pProtocol)
{
	if (pProtocol == NULL)
		return;

	enumTeamOperation operation = (enumTeamOperation)pProtocol->Operation;
	if (operation == team_operation_create)
	{
		CreateTeam();
	}
	else
	{
		if (IsInTeam())
		{
			KTeam* pTeam = GetTeam();
			if (pTeam != NULL)
			{
				switch(operation)
				{
				case team_operation_open_big_team:
					OpenBigTeamMode();
					break;
				case team_operation_dismiss:
					Dismiss();
					break;
				case team_operation_leave:
					LeaveTeam();
					break;
				case team_operation_set_auto_accept_apply_on:
					SetAutoAcceptApply(true);
					SendOperationResult(team_operation_set_auto_accept_apply_on, 0, 1);
					break;
				case team_operation_set_auto_accept_apply_off:
					SetAutoAcceptApply(false);
					SendOperationResult(team_operation_set_auto_accept_apply_off, 0, 1);
					break;
				default:
					{
						int npcIndex = NpcSet.SearchID(pProtocol->NpcId);
						if (npcIndex > 0)
						{
							int playerIndex = Npc[npcIndex].GetPlayerIdx();
							switch(operation)
							{
							case team_operation_invite:
								InviteJoinTeam(playerIndex);
								break;
							case team_operation_kick:
								KickMember(playerIndex);
								break;
							case team_operation_new_captain:
								NewCaptain(playerIndex);
								break;
							case team_operation_promote_assistant:
								PromoteAssistant(playerIndex);
								break;
							case team_operation_dismiss_assistant:
								DismissAssistant(playerIndex);
								break;
							case team_operation_apply_join_accept:
								ReplyApplyJoinTeam(playerIndex, true);
								break;
							case team_operation_apply_join_refuse:
								ReplyApplyJoinTeam(playerIndex, false);
								break;
							}
						}
					}
					break;
				}
			}
		}
		else
		{
			switch(operation)
			{
			case team_operation_apply_join:
				{
					int npcIndex = NpcSet.SearchID(pProtocol->NpcId);
					if (IsValidNpc(npcIndex))
					{
						ApplyJoinTeam(Npc[npcIndex].GetPlayerIdx());
					}
				}
				break;
			}
		}
	}
}

void KPlayerTeam::KickMember(int memberPlayerIndex)
{
	if (IsValidPlayer(memberPlayerIndex) && CanKick())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && pTeam->CanKick())
		{
			if (pTeam->GetCaptain() != memberPlayerIndex)
				pTeam->DeleteMember(memberPlayerIndex);
		}
	}
}

void KPlayerTeam::NewCaptain(int newCaptainPlayerIndex)
{
	if (IsValidPlayer(newCaptainPlayerIndex) && IsCaptain())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL)
		{
			int originalcaptainPlayerIndex = pTeam->GetCaptain();
			int originalCaptainMemberIndex = pTeam->FindByPlayerIndex(originalcaptainPlayerIndex);	
			pTeam->NewCaptain(newCaptainPlayerIndex);
		}
	}
}

void KPlayerTeam::PromoteAssistant(int playerIndex)
{
	if (IsValidPlayer(playerIndex) && CanPromoteAssistant())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && pTeam->CanPromoteAssistant())
		{
			int memberIndex = pTeam->FindByPlayerIndex(playerIndex);
			if (memberIndex != INVALID_TEAM_MEMBER_INDEX)
			{
				pTeam->SetMemberType(memberIndex, teammember_type_assistant, true);
			}
		}
	}
}

void KPlayerTeam::DismissAssistant(int playerIndex)
{
	if (IsValidPlayer(playerIndex) && CanDismissAssistant())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && pTeam->CanDismissAssistant())
		{
			int memberIndex = pTeam->FindByPlayerIndex(playerIndex);
			if (memberIndex != INVALID_TEAM_MEMBER_INDEX)
			{
				pTeam->SetMemberType(memberIndex, teammember_type_normal, true);
			}
		}
	}
}

void KPlayerTeam::InviteJoinTeam(int invitePlayerIndex)
{	
	if (!IsValidPlayer(m_PlayerIndex) || !IsInTeam() || !CanInvite())
		return;
	
	KTeam* pTeam = GetTeam();
	KPlayer& selfPlayer = Player[m_PlayerIndex];

	if (pTeam->IsFull() || !pTeam->CanInvite())
	{
		//队伍已满或队伍目前不能邀请
		selfPlayer.ShowPredefinedMsg(sid_invite_not_avaiable);
		return;
	}

	if (!IsValidPlayer(invitePlayerIndex) || invitePlayerIndex == m_PlayerIndex)
		return;

	if (Player[invitePlayerIndex].GetTeamInfo().IsInTeam())
	{
		//通知无法邀请这个玩家加入队伍，因为已经加入了其他队伍
		selfPlayer.ShowPredefinedMsg(sid_already_has_team);

		if (NeedAutoDismiss())
			Dismiss();

		return;
	}
	
	if (IsInInviteList(invitePlayerIndex))
	{
		selfPlayer.ShowPredefinedMsg(sid_already_invited_join_team);
		return;
	}

	const int inviteIndex = GetFreeInviteSlot();
	if (inviteIndex >= 0)
	{
		InviteInfo& info = m_nInviteList[inviteIndex];
		info.PlayerIndex = invitePlayerIndex;
		//设置组队邀请超时
		int inviteJointeamTimeout = ConfigManager::Singleton().GetGlobalVariable(global_var_invite_jointeam_timeout);
		if (inviteJointeamTimeout <= 0)
			inviteJointeamTimeout = 30;
		info.Timeout = GAME_FPS * inviteJointeamTimeout;
		
		char sendBuff[sizeof(TEAM_INVITE_ADD_SYNC) + MAXSIZE_ROLENAME];
		memset(sendBuff, 0, sizeof(sendBuff));
		TEAM_INVITE_ADD_SYNC* pAdd = (TEAM_INVITE_ADD_SYNC*)sendBuff;
		pAdd->ProtocolType = s2c_teaminviteadd;
		pAdd->InviterNpcId = Npc[selfPlayer.GetNpcIndex()].GetId();
		int nameLength = strlen(strncpy(pAdd->InviterName, Npc[selfPlayer.GetNpcIndex()].Name, MAXSIZE_ROLENAME));
		pAdd->Length += sizeof(TEAM_INVITE_ADD_SYNC) - 1 + nameLength;

		if (g_pServer != NULL)
			g_pServer->PackDataToClient(Player[invitePlayerIndex].GetNetConnectIdx(), (BYTE*)sendBuff, pAdd->Length + 1);
	}
	else
	{
		//队伍已满或队伍目前不能邀请
		selfPlayer.ShowPredefinedMsg(sid_invite_not_avaiable);
	}
}

void KPlayerTeam::ApplyJoinTeam(int targetPlayerIndex)
{
	if (!IsValidPlayer(m_PlayerIndex) || IsInTeam())
		return;

	if (IsValidPlayer(targetPlayerIndex))
	{
		Player[targetPlayerIndex].GetTeamInfo().ReceiveApplyJoinTeam(m_PlayerIndex);
	}
}

void KPlayerTeam::ReplyApplyJoinTeam(int applyPlayerIndex, bool accept, bool autoAccept)
{
	if (!IsValidPlayer(applyPlayerIndex))
		return;

	if (!autoAccept)//如果是自动接受入队请求，则不在申请列表中，不需要进行检查
	{
		const int applyIndex = GetApplyListIndex(applyPlayerIndex);
		if (applyIndex >= 0)
		{
			//从申请列表中删除
			m_ApplyList[applyIndex].PlayerIndex = 0;
		}
		else
		{
			//不在申请列表中，不能继续
			return;
		}
	}

	KPlayer& selfPlayer = Player[m_PlayerIndex];
	KPlayer& applyPlayer = Player[applyPlayerIndex];
	KTeam* pTeam = GetTeam();
	if (!pTeam)
		return;

	//检查自己的队伍状态
	if (pTeam->IsFull())
	{
		applyPlayer.ShowPredefinedMsg(sid_team_not_avaiable);
		return;
	}

	//拒绝对方
	if (!accept)
	{
		applyPlayer.ShowPredefinedMsg(sid_apply_join_team_refused);
		
		if (NeedAutoDismiss())
			Dismiss();

		return;
	}

	//检查对方的状态
	if (applyPlayer.GetTeamInfo().IsInTeam())
	{
		selfPlayer.ShowPredefinedMsg(sid_already_has_team);
		return;
	}

	pTeam->AddMember(applyPlayerIndex);
}

void KPlayerTeam::ReceiveApplyJoinTeam(int applyPlayerIndex)
{
	if (!IsValidPlayer(m_PlayerIndex) || !IsInTeam())
		return;

	if (!IsValidPlayer(applyPlayerIndex) || Player[applyPlayerIndex].GetTeamInfo().IsInTeam())
		return;

	KTeam* pTeam = Player[m_PlayerIndex].GetTeamInfo().GetTeam();
	if (!pTeam)
		return;

	KPlayer& selfPlayer = Player[m_PlayerIndex];
	KPlayer& applyPlayer = Player[applyPlayerIndex];

	if (pTeam->IsFull())
	{
		applyPlayer.ShowPredefinedMsg(sid_apply_not_avaiable);
		return;
	}

	if (selfPlayer.GetTeamInfo().CanInvite())//自己有组人的权限，自己处理
	{
		if (IsInApplyList(applyPlayerIndex))
		{
			applyPlayer.ShowPredefinedMsg(sid_already_applyed_join_team);
			return;
		}

		if (IsAutoAcceptApply())//自动接受入队请求
		{
			ReplyApplyJoinTeam(applyPlayerIndex, true, true);
			return;
		}

		const int applyIndex = GetFreeApplySlot();
		if (applyIndex >= 0)
		{
			InviteInfo& info = m_ApplyList[applyIndex];

			//记录申请信息
			info.PlayerIndex = applyPlayerIndex;
			int inviteJointeamTimeout = ConfigManager::Singleton().GetGlobalVariable(global_var_invite_jointeam_timeout);
			if (inviteJointeamTimeout <= 0)
				inviteJointeamTimeout = 30;
			info.Timeout = GAME_FPS * inviteJointeamTimeout;
			
			//发送申请息到客户端
			char sendBuff[sizeof(TEAM_APPLY_JOIN) + MAXSIZE_ROLENAME];
			memset(sendBuff, 0, sizeof(sendBuff));
			TEAM_APPLY_JOIN* pApply = (TEAM_APPLY_JOIN*)sendBuff;
			pApply->ProtocolType = s2c_apply_join_team;
			pApply->PlayerNpcId = Npc[applyPlayer.GetNpcIndex()].GetId();
			int nameLength = strlen(strncpy(pApply->PlayerName, Npc[applyPlayer.GetNpcIndex()].Name, MAXSIZE_ROLENAME));
			pApply->Length += sizeof(TEAM_APPLY_JOIN) - 1 + nameLength;

			if (g_pServer != NULL)
				g_pServer->PackDataToClient(selfPlayer.GetNetConnectIdx(), (BYTE*)sendBuff, pApply->Length + 1);
			
			return;
		}
		else
		{
			//申请列表满了，通知客户端
			applyPlayer.ShowPredefinedMsg(sid_apply_not_avaiable);
		}
	}
	else//自己没有组人的权限，把申请移交给队长
	{
		Player[pTeam->GetCaptain()].GetTeamInfo().ReceiveApplyJoinTeam(applyPlayerIndex);
	}
}

void KPlayerTeam::ReplyInviteJoinTeam(int replyerPlayerIndex, enumInviteJointeamReplay reply)
{
	if (m_PlayerIndex == replyerPlayerIndex)
		return;

	if (!IsValidPlayer(m_PlayerIndex) || !IsValidPlayer(replyerPlayerIndex))
		return;

	const int inviteIndex = GetInviteListIndex(replyerPlayerIndex);
	if (inviteIndex >= 0)
	{
		//从邀请列表中删除
		m_nInviteList[inviteIndex].PlayerIndex = 0;
	}
	else
	{
		//不在邀请列表中，不能继续
		return;
	}

	KTeam* pTeam = GetTeam();
	KPlayer& selfPlayer = Player[m_PlayerIndex];
	KPlayer& replyPlayer = Player[replyerPlayerIndex];

	// 状态对不对
	if (pTeam == NULL || !IsInTeam() || pTeam->IsFull())
	{
		//队伍已满或不存在
		replyPlayer.ShowPredefinedMsg(sid_team_not_avaiable);
		return;
	}

	// 对方拒绝邀请
	if (reply == invite_jointeam_replay_refuse
		|| reply == invite_jointeam_replay_timeout
		|| replyPlayer.GetTeamInfo().IsInTeam())
	{
		char sendBuff[sizeof(TEAM_INVITE_REFUSE) + MAXSIZE_ROLENAME];
		memset(sendBuff, 0, sizeof(sendBuff));
		TEAM_INVITE_REFUSE* pRefuse = (TEAM_INVITE_REFUSE*)sendBuff;
		pRefuse->Protocol = s2c_team_invite_refuse;		
		int nameLength = strlen(strncpy(pRefuse->PlayerName, Npc[Player[replyerPlayerIndex].GetNpcIndex()].Name, MAXSIZE_ROLENAME));
		pRefuse->Length = sizeof(TEAM_INVITE_REFUSE) - 1 + nameLength;

		if (g_pServer != NULL)
			g_pServer->PackDataToClient(selfPlayer.GetNetConnectIdx(), sendBuff, pRefuse->Length + 1);

		if (NeedAutoDismiss())
			Dismiss();
			
		return;
	}

	// 队伍添加成员
	pTeam->AddMember(replyerPlayerIndex);
}

void KPlayerTeam::LeaveTeam()
{
	if (IsInTeam())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && pTeam->CanLeave())
			pTeam->DeleteMember(m_PlayerIndex);
	}
}

void KPlayerTeam::Dismiss()
{
	if (CanDismiss())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && pTeam->CanDismiss())
			pTeam->Dismiss();
	}
}

void KPlayerTeam::SendTeamSyncData()
{
	if (IsInTeam()) 
	{
		KPlayer& selfPlayer = Player[m_PlayerIndex];
		KNpc& selfNpc = Npc[selfPlayer.GetNpcIndex()];

		//基本信息
		TEAMMATE_INFO syncInfo;
		syncInfo.ProtocolType = s2c_sync_teammemberlife;
		syncInfo.Id = selfNpc.GetId();
/*
带宽优化前
现在用百分比代替数值
		syncInfo.Life = (WORD)(selfNpc.IsAlive() ? selfNpc.m_UnaryAttrMgr[nuai_curlife] : 0);
		syncInfo.LifeMax = (WORD)(selfNpc.IsAlive() ? selfNpc.m_CompAttrMgr[ncai_lifeuplimit] : 0);
		syncInfo.Mana = (WORD)(selfNpc.IsAlive() ? selfNpc.m_UnaryAttrMgr[nuai_curmana] : 0);
		syncInfo.ManaMax = (WORD)(selfNpc.IsAlive() ? selfNpc.m_CompAttrMgr[ncai_manauplimit] : 0);
*/
		int life = selfNpc.m_UnaryAttrMgr[nuai_curlife];
		int mana = selfNpc.m_UnaryAttrMgr[nuai_curmana];
		int lifeLimit = selfNpc.m_CompAttrMgr[ncai_lifeuplimit];
		int manaLimit = selfNpc.m_CompAttrMgr[ncai_manauplimit];
		if (lifeLimit <= 0)
			lifeLimit = 1;
		if (manaLimit <= 0)
			manaLimit = 1;
		if (life < 0)
			life = 0;
		if (life > lifeLimit)
			life = lifeLimit;
		if (mana < 0)
			mana = 0;
		if (mana > manaLimit)
			mana = manaLimit;	

		syncInfo.LifePercent = 100 * life / lifeLimit;
		syncInfo.ManaPercent = 100 * mana / manaLimit;

		syncInfo.MapId = SubWorld[selfNpc.m_SubWorldIndex].m_SubWorldID;
		int	posX, posY;
		selfNpc.GetMpsPos(&posX, &posY);
		syncInfo.PosX = posX;
		syncInfo.PosY = posY;

		/*
		暂时不同步队友BUFF
		//BUFF信息
		BuffMgr& buffMgr = BuffMgr::Singleton();
		char buffSyncSendBuff[MAX_SYNC_TEAMATE_BUFF_COUNT * sizeof(_BuffPair) + sizeof(_Buff_Sync_Npc)];
		_BuffPair buffPair[MAX_SYNC_TEAMATE_BUFF_COUNT];
		int syncBuffCount = MAX_SYNC_TEAMATE_BUFF_COUNT;		
		_Buff_Sync_Npc*	pBSN = (_Buff_Sync_Npc*)buffSyncSendBuff;
		pBSN->Protocol = s2c_buff_family;
		pBSN->ProtocolExtend = buff_sync_npc;
		pBSN->dwID = selfNpc.GetId();
		buffMgr.GetSyncBuffID(selfPlayer.GetNpcIndex(), buffPair, syncBuffCount);
		pBSN->wCount = syncBuffCount;
		for( int buffLoopCount = 0; buffLoopCount < syncBuffCount; buffLoopCount++ )
		{
			pBSN->Buff[buffLoopCount].dwBuffTempID = buffPair[buffLoopCount].ulBuffTempID;
			pBSN->Buff[buffLoopCount].dwBuffTID	= buffPair[buffLoopCount].ulBuffID;
		}
		int buffSyncSize = sizeof(_Buff_Sync_Npc) + (( syncBuffCount - 1 ) * sizeof(_Buff_Sync_Npc::_Pair));
		pBSN->wProtocolSize = buffSyncSize - 1;
		*/

		KTeam* pTeam = GetTeam();
		if (pTeam != NULL)
		{
			int maxTeammemberCount = pTeam->GetMaxMemberCount();
			for(int memberLoopCount = 0; memberLoopCount < maxTeammemberCount; ++memberLoopCount)
			{
				const int memberPlayerIndex = pTeam->GetMemberPlayerIndex(memberLoopCount);
				if (memberPlayerIndex != INVALID_TEAM_MEMBER_INDEX && memberPlayerIndex != m_PlayerIndex)
				{	
					if (g_pServer != NULL)
					{
						KPlayer& teamatePlayer = Player[memberPlayerIndex];

						//同步基本信息
						g_pServer->PackDataToClient(teamatePlayer.GetNetConnectIdx(), (BYTE*)&syncInfo, sizeof(syncInfo));

						/*
						//同步BUFF信息
						if (teamatePlayer.GetTeamInfo().IsNeedSyncTeammateBuff())
							g_pServer->PackDataToClient(Player[memberPlayerIndex].GetNetConnectIdx(), pBSN, buffSyncSize);
						*/
					}
				}
			}
		}	
	}
}

void KPlayerTeam::SendSelfTeamInfo()
{
	if (IsInTeam())
	{
		char teammemberInfoBuff[(MAX_BIG_TEAM_MEMBER - 1) * sizeof(TEAM_MEMBER_INFO) + sizeof(TEAM_INFO)];
		TEAM_INFO* pTeamInfo = (TEAM_INFO*)teammemberInfoBuff;
		pTeamInfo->Protocol = s2c_teamselfinfo;
		pTeamInfo->MemberCount = 0;
		pTeamInfo->ProtocolSize = sizeof(TEAM_INFO) - 1 - sizeof(TEAM_MEMBER_INFO);

		KTeam* pTeam = GetTeam();
		if (pTeam != NULL)
		{
			pTeamInfo->TeamServerId = GetTeamId();
			pTeamInfo->IsBigTeam = (pTeam->IsBigTeam() ? TRUE : FALSE);
			pTeamInfo->CaptainId = Npc[Player[pTeam->GetCaptain()].GetNpcIndex()].GetId();
			pTeamInfo->TeamSetting = pTeam->GetTeamSetting();
			int maxTeammemberCount = pTeam->GetMaxMemberCount();
			for (int i = 0; i < maxTeammemberCount; i++)
			{
				int memberPlayerIndex = pTeam->GetMemberPlayerIndex(i);
				if (memberPlayerIndex != INVALID_TEAM_MEMBER_INDEX && memberPlayerIndex != m_PlayerIndex)
				{
					KPlayer& player = Player[memberPlayerIndex];
					KNpc& npc = Npc[player.GetNpcIndex()];					
					BYTE sexInfo = npc.GetSex();
					BYTE classInfo = npc.GetSeries() + (player.GetSkillSeries() + 1) * 3;
					
					TEAM_MEMBER_INFO& memberInfo = pTeamInfo->Member[pTeamInfo->MemberCount];
					memberInfo.NpcId = npc.GetId();
					memberInfo.Type = (BYTE)pTeam->GetMemberType(i);
					memberInfo.Level = npc.GetLevel();
					memberInfo.ClassAndSexInfo = (sexInfo << 4) + classInfo;
					memberInfo.Face = npc.m_nHeadImage;
					strcpy(memberInfo.Name, npc.Name);

					pTeamInfo->MemberCount++;
					pTeamInfo->ProtocolSize += sizeof(TEAM_MEMBER_INFO);
				}
			}
			
			if (g_pServer != NULL)
				g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(), (void*)pTeamInfo, pTeamInfo->ProtocolSize + 1);
		}
	}
	else
	{
		//发送离队信息
		PLAYER_LEAVE_TEAM sLeaveTeam;
		sLeaveTeam.ProtocolType = s2c_teamleave;
		sLeaveTeam.m_dwNpcID = Npc[Player[m_PlayerIndex].GetNpcIndex()].GetId();
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(), (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
	}
}

void KPlayerTeam::SendMemberBasicInfo(unsigned int clientId)
{
	if (!IsInTeam())
		return;

	KTeam* pTeam = GetTeam();	
	if (pTeam == NULL)
		return;

	KPlayer& player = Player[m_PlayerIndex];
	KNpc& npc = Npc[player.GetNpcIndex()];

	UPDATE_TEAM_MEMBER_INFO info;	
	info.Protocol = s2c_update_team_member_info;	
	TEAM_MEMBER_INFO& memberInfo = info.Info;
	memberInfo.NpcId = npc.GetId();
	strcpy(memberInfo.Name, npc.Name);
	memberInfo.Type = teammember_type_normal;
	memberInfo.Level = npc.GetLevel();
	BYTE sexInfo = npc.GetSex();
	BYTE classInfo = npc.GetSeries() + (player.GetSkillSeries() + 1) * 3;
	memberInfo.ClassAndSexInfo = (sexInfo << 4) + classInfo;
	memberInfo.Face = npc.m_nHeadImage;
	
	int memberIndex = pTeam->FindByPlayerIndex(m_PlayerIndex);
	if (memberIndex != INVALID_TEAM_MEMBER_INDEX)
	{
		memberInfo.Type = pTeam->GetMemberType(memberIndex);
	}
	
	if (g_pServer == NULL)
		return;

	if (clientId > 0)
	{
		g_pServer->PackDataToClient(clientId, (BYTE*)&info, sizeof(info));
	}
	else
	{
		int maxTeamMember = pTeam->GetMaxMemberCount();
		for (int memberIndex = 0; memberIndex < maxTeamMember; memberIndex++)
		{
			int playerIndex = pTeam->GetMemberPlayerIndex(memberIndex);
			if (playerIndex != INVALID_PLAYER_INDEX)
			{
				g_pServer->PackDataToClient(Player[playerIndex].GetNetConnectIdx(), (BYTE*)&info, sizeof(info));
			}
		}
	}
}
void KPlayerTeam::RequestTeamInfo(int playerIndex)
{	
	if (playerIndex == m_PlayerIndex)
	{
		SendSelfTeamInfo();
		return;
	}

	//TODO 暂时没有提供查询其他玩家的组队信息的功能
}

void KPlayerTeam::SendOperationResult(enumTeamOperation operation, DWORD npcId, int result)
{
	TEAM_OPERATION_RESULT operationResult;
	operationResult.Protocol = s2c_team_operation_result;
	operationResult.Operation = operation;
	operationResult.NpcId = npcId;
	operationResult.Result = result;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(), (BYTE*)&operationResult, sizeof(operationResult));			
}

bool KPlayerTeam::CanOpenBigTeam()
{
	KTeam* pTeam = GetTeam();
	return (IsInTeam()
		&& m_TeamMemberType == teammember_type_captain
		&& pTeam != NULL
		&& !pTeam->IsBigTeam()
		&& pTeam->GetMemberCount() >= MIN_TEAM_MEMBER_TO_OPEN_BIG_TEAM);
}

bool KPlayerTeam::NeedAutoDismiss()
{
	if (CanDismiss())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam)
		{
			return (GetInviteCount() == 0 && GetApplyCount() == 0 && pTeam->GetMemberCount() <= 1);
		}
	}

	return false;
}

KTeam::KTeam()
{
	Release();
}

void KTeam::Release()
{
	m_nCaptain = INVALID_TEAM_MEMBER_INDEX;
	m_nMemNum = 0;
	for (int i = 0; i < MAX_BIG_TEAM_MEMBER; i++)
	{
		m_nMember[i] = INVALID_TEAM_MEMBER_INDEX;
		m_MemberType[i] = teammember_type_normal;
	}
	m_bIsBigTeam = false;
	for (int instance = 0; instance < INSTANCE_SUBWORLD_START; instance++)
	{
		m_InstanceId[instance] = INVALID_INSTANCE_ID;
	}
	m_TeamSetting.reset();
	m_BindWorldIndex = INVALID_WORLD_INDEX;
	m_BindWorldTeamIndex = 0;
}

void KTeam::SetIndex(int nIndex)
{
	m_nIndex = nIndex;
}

int KTeam::FindFree()
{
	int maxMemberCount = GetMaxMemberCount();
	for (int i = 0; i < maxMemberCount; i++)
	{
		if (m_nMember[i] == INVALID_TEAM_MEMBER_INDEX)
			return i;
	}

	return INVALID_TEAM_MEMBER_INDEX;
}

int KTeam::FindByPlayerIndex(int playerIndex) const
{
	int maxMemberCount = GetMaxMemberCount();
	for (int i = 0; i <	maxMemberCount; i++)
	{
		if (m_nMember[i] >= 0 && m_nMember[i] == playerIndex)
			return i;
	}

	return INVALID_TEAM_MEMBER_INDEX;
}

bool KTeam::IsFull()
{
	return (m_nMemNum >= GetMaxMemberCount());
}

void KTeam::Create(int creater)
{
	Release();
	m_TeamSetting = bitset<team_setting_count>(0xffffffff);
	m_nCaptain = creater;
	int memberIndex = AddMember(creater);
	SetMemberType(memberIndex, teammember_type_captain, true);
}

void KTeam::Dismiss()
{
	if (m_nCaptain == INVALID_TEAM_MEMBER_INDEX)
		return;

	PLAYER_LEAVE_TEAM leaveTeam;
	leaveTeam.ProtocolType = s2c_teamleave;	
	
	//通知所有队员，队伍解散
	int maxMemberCount = GetMaxMemberCount();
	for (int i = 0; i < maxMemberCount; i++)
	{
		int memberPlayerIndex = m_nMember[i];
		if (memberPlayerIndex != INVALID_TEAM_MEMBER_INDEX)
		{
			KPlayer& player = Player[memberPlayerIndex];			
			player.GetTeamInfo().Init(memberPlayerIndex);			
			leaveTeam.m_dwNpcID = Npc[player.GetNpcIndex()].GetId();

			if (g_pServer != NULL)
				g_pServer->PackDataToClient(player.GetNetConnectIdx(), (BYTE*)&leaveTeam, sizeof(PLAYER_LEAVE_TEAM));
			
			g_ChatCenterS.s2cChannelOpe(memberPlayerIndex, TEAM_ROOM_ID, chat_delchannel, CHAT_CHANNEL_NAME_TEAM);

			Npc[player.GetNpcIndex()].m_UnaryAttrMgr.Set(nuai_team_id, 0);
			Npc[player.GetNpcIndex()].SyncAttr(npc_attr_unary, nuai_team_id, 0, true);

			player.OnEvent(player_event_leave_team, &m_nIndex);
		}
	}

	if (m_BindWorldIndex >= 0)
	{
		SubWorld[m_BindWorldIndex].SetWorldTeam(m_BindWorldTeamIndex, INVALID_TEAM_ID);
	}

	g_TeamSet.RemoveTeam(m_nIndex);
}

int KTeam::AddMember(int member)
{
	//队伍是否为空
	if (m_nCaptain == INVALID_TEAM_MEMBER_INDEX)
		return INVALID_TEAM_MEMBER_INDEX;

	//队员是否合法
	if (!IsValidPlayer(member))
		return INVALID_TEAM_MEMBER_INDEX;

	//是否已经满员
	if (IsFull())
		return false;

	int freeIndex = FindFree();
	int maxMemberCount = GetMaxMemberCount();
	if (freeIndex >= 0 && freeIndex < maxMemberCount)
	{
		m_nMember[freeIndex] = member;
		m_MemberType[freeIndex] = teammember_type_normal;
		m_nMemNum++;

		KPlayer& player = Player[member];
		KPlayerTeam& teamInfo = player.GetTeamInfo();
		teamInfo.Init(member);
		teamInfo.SetTeamMemberType(teammember_type_normal);
		teamInfo.SetTeamId(m_nIndex);
		teamInfo.SendMemberBasicInfo();
		teamInfo.SendSelfTeamInfo();

		Npc[player.GetNpcIndex()].m_UnaryAttrMgr.Set(nuai_team_id, m_nIndex);
		Npc[player.GetNpcIndex()].SyncAttr(npc_attr_unary, nuai_team_id, 0, true);

		player.OnEvent(player_event_join_team, &m_nIndex);

		TeamChanged();

		return freeIndex;
	}

	return INVALID_TEAM_MEMBER_INDEX;
}

bool KTeam::DeleteMember(int deleteMember)
{
	if (!IsValidPlayer(deleteMember))
		return false;

	KPlayer& deletePlayer = Player[deleteMember];

	if (deleteMember == m_nCaptain)//队长被踢出队伍
	{
		if (m_nMemNum > 2)//队长离开后队伍里还有人，选择其中一个作为新的队长（自动队长移交）
		{
			bool foundNewCaptain = false;
			int maxMemberCount = GetMaxMemberCount();
			for (int memberLoopCount = 0; memberLoopCount < maxMemberCount; memberLoopCount++)
			{
				int memberPlayerIndex = m_nMember[memberLoopCount];
				if (memberPlayerIndex != INVALID_TEAM_MEMBER_INDEX && memberPlayerIndex != m_nCaptain)
				{
					if (NewCaptain(memberPlayerIndex))
					{
						foundNewCaptain = true;
						break;
					}					
				}
			}

			deletePlayer.OnEvent(player_event_leave_team, &m_nIndex);

			if (!foundNewCaptain)
			{				
				Dismiss();				
				return true;
			}
		}
		else//队长离开后队伍里只有没有人了，队伍自行解散
		{
			deletePlayer.OnEvent(player_event_leave_team, &m_nIndex);

			Dismiss();
			return true;
		}		
	}
	
	//普通队员被踢出队伍	
	if (IsMember(deleteMember))
	{
		KPlayer& deletePlayer = Player[deleteMember];
		deletePlayer.GetTeamInfo().Init(deleteMember);

		Npc[deletePlayer.GetNpcIndex()].m_UnaryAttrMgr.Set(nuai_team_id, 0);
		Npc[deletePlayer.GetNpcIndex()].SyncAttr(npc_attr_unary, nuai_team_id, 0, true);
		 
		PLAYER_LEAVE_TEAM sLeaveTeam;
		sLeaveTeam.ProtocolType = s2c_teamleave;
		sLeaveTeam.m_dwNpcID = Npc[deletePlayer.GetNpcIndex()].GetId();
		int maxTeammemberCount = GetMaxMemberCount();
		for (int i = 0; i < maxTeammemberCount; i++)
		{
			if (m_nMember[i] != INVALID_TEAM_MEMBER_INDEX && g_pServer != NULL)
				g_pServer->PackDataToClient(Player[m_nMember[i]].GetNetConnectIdx(), (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
			
			if (m_nMember[i] == deleteMember)
			{
				m_nMember[i] = INVALID_TEAM_MEMBER_INDEX;
				m_nMemNum--;					
			}
		}

		deletePlayer.OnEvent(player_event_leave_team, &m_nIndex);
		
		if (m_nMemNum == 1)
			Dismiss();
		
		TeamChanged();
	
		return true;
	}
	else
	{
		return false;
	}
}

void KTeam::ForEach(CHATCALLBACK pCallBack,  const void *pCallbackParam,
				BROADCASTFILTER pFilter /* = NULL */, unsigned int uFilterPassby /* = 0 */)
{
	int maxMemberCount = GetMaxMemberCount();
	for(int i = 0; i < maxMemberCount; ++i)
	{
		if(m_nMember[i] > 0)
		{
			if( NULL == pFilter || pFilter(m_nMember[i], uFilterPassby) )
				pCallBack(pCallbackParam, m_nMember[i]);
		}
	}
}

bool KTeam::NewCaptain(int member)
{
	//队伍是否为空
	if (m_nCaptain == INVALID_TEAM_MEMBER_INDEX)
		return false;

	//队员是否合法
	if (!IsValidPlayer(member))
		return false;

	//新的队长不是原有队长
	if (m_nCaptain == member)
		return false;

	int originalCaptainMemberIndex = FindByPlayerIndex(m_nCaptain);
	int newCaptainMemberIndex = FindByPlayerIndex(member);
	if (originalCaptainMemberIndex != INVALID_TEAM_MEMBER_INDEX && newCaptainMemberIndex != INVALID_TEAM_MEMBER_INDEX)
	{
		int originalcaptainPlayerIndex = m_nCaptain;
		int newCaptainPlayerIndex = member;

		m_nCaptain = member;
		SetMemberType(originalCaptainMemberIndex, teammember_type_normal, true);
		SetMemberType(newCaptainMemberIndex, teammember_type_captain, true);

		return true;
	}

	return false;
}

void KTeam::OpenBigTeamMode()
{
	if (!m_bIsBigTeam)
	{
		m_bIsBigTeam = true;
		
		int maxMemberCount = GetMaxMemberCount();
		for (int i = 0; i < maxMemberCount; i++)
		{
			int memberPlayerIndex = GetMemberPlayerIndex(i);
			if (memberPlayerIndex != INVALID_TEAM_MEMBER_INDEX)
			{
				KPlayer& player = Player[memberPlayerIndex];
				player.GetTeamInfo().SendSelfTeamInfo();
			}
		}

		TeamChanged();
	}
}

void KTeam::SetMemberType(int member, enumTeamMemberType type, bool syncToClient)
{
	if (member >=0 && member < GetMaxMemberCount())
	{
		m_MemberType[member] = type;		

		if (m_nMember[member] > 0)
		{
			KPlayerTeam& teamInfo = Player[m_nMember[member]].GetTeamInfo();
			teamInfo.SetTeamMemberType(type);

			if (syncToClient)
				teamInfo.SendMemberBasicInfo();
		}

		TeamChanged();
	}
}

void KTeam::TeamChanged()
{
	int maxMemberCount = GetMaxMemberCount();
	for (int i = 0; i < maxMemberCount; i++)
	{
		int memberPlayerIndex = GetMemberPlayerIndex(i);
		if (memberPlayerIndex != INVALID_TEAM_MEMBER_INDEX)
		{
			KPlayer& player = Player[memberPlayerIndex];
			player.OnEvent(player_event_team_changed, &m_nIndex);
		}
	}
}

//---------------------------------------------------------------------------
//	功能：初始化
//---------------------------------------------------------------------------
void	KTeamSet::Init()
{
	m_FreeIdx.Init(MAX_TEAM);
	g_TeamS[0].SetIndex(0);
	for (int i = MAX_TEAM ; --i ; )
	{
		m_FreeIdx.Insert(i);
		g_TeamS[i].SetIndex(i);
	}
}

//---------------------------------------------------------------------------
//	功能：创建一支队伍
//---------------------------------------------------------------------------
int		KTeamSet::CreateTeam(int nPlayerID)//, char *lpszName)
{
	int nTeamID = m_FreeIdx.GetNext(0);
	if (nTeamID <= 0)
	{
		return -2;
	}
	else
	{
		g_TeamS[nTeamID].Create(nPlayerID);
		m_FreeIdx.Remove(nTeamID);
		return nTeamID;
	}
	return -1;
}

int KTeamSet::RemoveTeam(int nIdx)
{
	_ASSERT(nIdx < MAX_TEAM && nIdx > 0);

	g_TeamS[nIdx].Release();
	m_FreeIdx.Insert(nIdx);
	return TRUE;
}

int KTeamSet::ListTeam(TeamBasicInfo* pTeamInfoBuff, int outputLimit, enumListTeamMode listMode, KTeamSet::TeamFilter pFilter, void* pFilterPassby) const
{
	int startIndex = 0;
	switch(listMode)
	{
	case list_team_mode_from_begin:
		startIndex = 1;
		break;
	case list_team_mode_random:
		startIndex = g_Random(MAX_TEAM);
		break;
	default:
		break;
	}

	int findCount = 0;
	bool allSearch = false;
	int index = startIndex;
	while (findCount < outputLimit && !allSearch)
	{
		KTeam& team = g_TeamS[index];
		if (team.IsValid() && (!pFilter || pFilter(&team, pFilterPassby)))
		{
			TeamBasicInfo* pInfo = pTeamInfoBuff + findCount;
			pInfo->TeamId = team.GetId();
			pInfo->MemberCount = team.GetMemberCount();
			KNpc& captainNpc = Npc[Player[team.GetCaptain()].GetNpcIndex()];
			pInfo->CaptainNpcID = captainNpc.GetId();
			memset(pInfo->CaptainName, 0, sizeof(pInfo->CaptainName));
			strncpy(pInfo->CaptainName, captainNpc.Name, sizeof(pInfo->CaptainName));
			pInfo->CaptainName[sizeof(pInfo->CaptainName) - 1] = 0;

			findCount++;
		}

		index = (index + 1) % MAX_TEAM;
		if (index == startIndex)
			allSearch = true;
	}

	return findCount;
}

bool KTeamSet::InSubworld(KTeam* pTeam, void *pPassby)
{
	if (pTeam && pTeam->IsValid())
	{
		int worldIndex = Npc[Player[pTeam->GetCaptain()].GetNpcIndex()].GetSubWorldIndex();
		if (worldIndex >= 0)
		{
			return (SubWorld[worldIndex].GetWorldTemplateId() == *((int*)pPassby));
		}
	}

	return false;
}