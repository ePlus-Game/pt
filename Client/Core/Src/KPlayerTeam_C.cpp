//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/16/2006 17:23
//      File_base        : KPlayerTeam_C
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KNpc.h"
#include "networkinterface.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KPlayerTeam_C.h"
#include "KNpcSet.h"
#include "CoreShell.h"
#include "ChatDataDef.h"
#include "CoreRelated.h"

enum
{
	Team_S_Close = 0,
	Team_S_Open,
};

KTeam g_TeamC[MAX_TEAM];

TeamViewer g_TeamViewer;

KPlayerTeam::KPlayerTeam()
{
	Release();
}

void KPlayerTeam::Release()
{
	m_IsInTeam = false;
	m_nApplyCaptainID = -1;
	m_nApplyCaptainID = 0;
	m_dwApplyTimer = 0;
	m_bAutoRefuseInviteFlag = FALSE;	 // TRUE 自动拒绝   FALSE 手动
	m_AutoAcceptApply = false;
	ReleaseList();
}

void KPlayerTeam::ReleaseList()
{
	for (int i = 0; i < MAX_TEAM_APPLY_LIST; i++)
		m_sApplyList[i].Release();
}


void KPlayerTeam::Create()
{
	if (IsInTeam())
		return;

	SendTeamOperation(team_operation_create);
}

void KPlayerTeam::OpenBigTeamMode()
{
	if (CanOpenBigTeam())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && !pTeam->IsBigTeam() && pTeam->CanOpenBigTeam())
		{
			SendTeamOperation(team_operation_open_big_team);
		}		
	}
}

void	KPlayerTeam::ReceiveInvite(TEAM_INVITE_ADD_SYNC *pInvite)
{
	if ( !pInvite )
		return;
	// 通知界面有人邀请玩家加入某个队伍
	KUiPlayerItem	sPlayer;
	sPlayer.uId = pInvite->InviterNpcId;
	sPlayer.Name[0] = 0;
	strncpy(sPlayer.Name, pInvite->InviterName, MAXSIZE_ROLENAME);
	sPlayer.Name[sizeof(sPlayer.Name) - 1] = 0;
	
	if (m_bAutoRefuseInviteFlag)
	{
		ReplyInvite(sPlayer.uId, invite_jointeam_replay_refuse);
	}
	else
	{
		CoreDataChanged(CDCNI_OPEN_INVOTE_TEAM_REPLY, (unsigned int)&sPlayer, NULL);
	}
}
//---------------------------------------------------------------------------
//	功能：回复邀请
//---------------------------------------------------------------------------
void	KPlayerTeam::ReplyInvite(int nIdx, enumInviteJointeamReplay nResult)
{
	if (nIdx < 0 || nResult < 0 || nResult > 1)
		return;
	if (Player[CLIENT_PLAYER_INDEX].m_nIndex <= 0 || Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp == camp_event)
		nResult = invite_jointeam_replay_refuse;
	TEAM_REPLY_INVITE_COMMAND	sReply;
	sReply.ProtocolType = c2s_teamreplyinvite;
	sReply.m_nIndex = nIdx;
	sReply.m_btResult = nResult;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&sReply, sizeof(TEAM_REPLY_INVITE_COMMAND));
}
//---------------------------------------------------------------------------
//	功能：设定是否自动拒绝别人的加入队伍的邀请
//---------------------------------------------------------------------------
void	KPlayerTeam::SetAutoRefuseInvite(BOOL bFlag)
{
	KSystemMessage	sMsg;
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;

	if (bFlag)
	{
		m_bAutoRefuseInviteFlag = TRUE;
		sprintf(sMsg.szMessage, MSG_TEAM_AUTO_REFUSE_INVITE);
	}
	else
	{
		m_bAutoRefuseInviteFlag = FALSE;
		sprintf(sMsg.szMessage, MSG_TEAM_NOT_AUTO_REFUSE_INVITE);
	}

//	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}

BOOL KPlayerTeam::IsAutoRefuseInvite()
{
	return m_bAutoRefuseInviteFlag;
}
//---------------------------------------------------------------------------
//	功能：获得自身队伍信息（给界面）
//---------------------------------------------------------------------------
int KPlayerTeam::GetInfo(KUiPlayerTeam *pTeam)
{
	if (!pTeam)
		return FALSE;

	pTeam->bTeamLeader = false;
	pTeam->bCanKick = false;
	pTeam->bCanPromoteAssistant = false;
	pTeam->bCanDismissAssistant = false;
	pTeam->bCanDismissTeam = false;
	pTeam->bCanInvite = false;
	pTeam->bCanOpenBigTeam = false;
	pTeam->bIsBigTeam = false;
	pTeam->cNumMember = 0;
	pTeam->nTeamServerID = -1;
	pTeam->dwCaptainNpcID = 0;
	pTeam->bAutoAcceptApply = false;

	if (!IsInTeam())
		return FALSE;

	KTeam* pClientTeam = GetTeam();
	if (pClientTeam == NULL)
		return FALSE;

	pTeam->bTeamLeader = IsCaptain();
	pTeam->bCanKick = CanKick() && pClientTeam->CanKick();
	pTeam->bCanPromoteAssistant = CanPromoteAssistant() && pClientTeam->CanPromoteAssistant();
	pTeam->bCanDismissAssistant = CanDismissAssistant() && pClientTeam->CanPromoteAssistant();
	pTeam->bCanDismissTeam = CanDismiss();
	pTeam->bCanInvite = CanInvite() && pClientTeam->CanInvite();
	pTeam->bCanOpenBigTeam = CanOpenBigTeam() && pClientTeam->CanOpenBigTeam();
	pTeam->bIsBigTeam = pClientTeam->IsBigTeam();
	pTeam->cNumMember = pClientTeam->GetMemberCount() - 1;
	pTeam->nTeamServerID = pClientTeam->GetTeamServerID();
	pTeam->dwCaptainNpcID = pClientTeam->GetCaptain();
	pTeam->bAutoAcceptApply = IsAutoAcceptApply();

	return TRUE;
}
//---------------------------------------------------------------------------
//	功能：更新界面显示
//---------------------------------------------------------------------------
void	KPlayerTeam::UpdateInterface(bool bShow)
{
	KUiPlayerTeam	sTeam;
	if (GetInfo(&sTeam))
	{
		CoreDataChanged(CDCNI_UPDATE_TEAM_INFO, (unsigned int)&sTeam, bShow);
	}
	else
	{
		CoreDataChanged(CDCNI_UPDATE_TEAM_INFO, 0, 0);
	}
}
//---------------------------------------------------------------------------
//	功能：从申请人列表中删除某个申请人
//---------------------------------------------------------------------------
void	KPlayerTeam::DeleteOneFromApplyList(DWORD dwNpcID)
{
	for (int i = 0; i < MAX_TEAM_APPLY_LIST; i++)
	{
		if (m_sApplyList[i].m_dwNpcID == dwNpcID)
		{
			m_sApplyList[i].Release();
			return;
		}
	}
}

void KPlayerTeam::UpdateSelfTeam(TEAM_INFO* pTeamInfo)
{
	KTeam& clientTeam = GetClientTeam();
	KPlayer& clientPlayer = GetClientPlayer();

	//提示消息
	char msgBuff[256] = { 0 };	
	if (IsInTeam())
	{
		if (!clientTeam.IsBigTeam() && (pTeamInfo->IsBigTeam == TRUE))//由普通队伍转化为大队伍
			g_GetStringRes(sid_open_big_team, msgBuff, sizeof(msgBuff));
	}
	else
	{
		if (pTeamInfo->IsBigTeam == TRUE)
			g_GetStringRes(sid_you_join_big_team, msgBuff, sizeof(msgBuff));
		else
			g_GetStringRes(sid_you_join_team, msgBuff, sizeof(msgBuff));
	}
	msgBuff[sizeof(msgBuff) - 1] = 0;
	CoreDataChanged(GDCNI_APPEND_MESSAGE, (unsigned int)msgBuff, SYSTEM_ROOM_ID);

	clientTeam.Release();
	SetInTeam(true);
	
	if (Npc[clientPlayer.GetNpcIndex()].GetId() == pTeamInfo->CaptainId)	// 队长
	{
		m_TeamMemberType = teammember_type_captain;
	}
	else// 队员
	{
		m_TeamMemberType = teammember_type_normal;
		for (int i = 0; i < MAX_TEAM_APPLY_LIST; i++)
			m_sApplyList[i].Release();
	}
		
	clientTeam.SetCaptain(pTeamInfo->CaptainId);
	clientTeam.SetTeamServerID(pTeamInfo->TeamServerId);
	if (pTeamInfo->IsBigTeam == TRUE)
	{
		clientTeam.OpenBigTeamMode();
	}
	
	int clientNpcId = Npc[GetClientPlayer().GetNpcIndex()].GetId();
	for (int memberLoopCount = 0; memberLoopCount < pTeamInfo->MemberCount; memberLoopCount++)
	{
		TEAM_MEMBER_INFO& memberInfo = pTeamInfo->Member[memberLoopCount];
		if (memberInfo.NpcId > 0 && memberInfo.NpcId != clientNpcId)
		{
			int memberIndex = clientTeam.AddMember(
				memberInfo.NpcId,
				memberInfo.Level,
				memberInfo.ClassAndSexInfo,
				memberInfo.Face,
				memberInfo.Name);
			if (memberIndex != INVALID_TEAM_MEMBER_INDEX)
			{
				ClientTeamMemberInfo* pTeamInfo = clientTeam.GetMemberInfo(memberIndex);
				if (pTeamInfo != NULL)
					pTeamInfo->Type = (enumTeamMemberType)memberInfo.Type;
			}
		}
	}

	clientTeam.SetTeamSetting(pTeamInfo->TeamSetting);

	UpdateInterface(true);
}

void KPlayerTeam::UpdateTeamMemberBasicInfo(UPDATE_TEAM_MEMBER_INFO* pProtocol)
{
	if (IsInTeam())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam == NULL)
			return;

		TEAM_MEMBER_INFO& info = pProtocol->Info;
		char memberName[32] = { 0 };
		strncpy(memberName, info.Name, sizeof(memberName));
		memberName[sizeof(memberName) - 1] = 0;
		enumTeamMemberType originalType = teammember_type_normal;
		enumTeamMemberType newType = (enumTeamMemberType)info.Type;

		if (newType == teammember_type_captain)
		{
			pTeam->SetCaptain(info.NpcId);
		}
		
		if (info.NpcId == Npc[GetClientPlayer().GetNpcIndex()].GetId())//自己
		{
			g_GetStringRes(sid_you, memberName, sizeof(memberName));//你
			originalType = GetTeamMemberType();
			if (originalType != newType)
			{
				SetTeamMemberType(newType);
			}
		}
		else//其他人
		{
			int memberIndex = pTeam->FindMemberID(info.NpcId);
			if (memberIndex != INVALID_TEAM_MEMBER_INDEX)//该队员已经存在
			{
				ClientTeamMemberInfo* pMemberInfo = pTeam->GetMemberInfo(memberIndex);
				if (pMemberInfo != NULL)
				{
					originalType = pMemberInfo->Type;
					int originalLevel = pMemberInfo->Level;
					
					pMemberInfo->Type = newType;
					pMemberInfo->SetName(info.Name);
					pMemberInfo->Level = info.Level;
					BYTE classInfo = info.ClassAndSexInfo & 0x0f;
					BYTE sexInfo = info.ClassAndSexInfo >> 4;
					pMemberInfo->SkillSeries = classInfo / 3 - 1;
					pMemberInfo->Class = classInfo % 3;
					pMemberInfo->Sex = sexInfo;
					pMemberInfo->Face = info.Face;
					
					if (originalLevel < info.Level)//升级了
					{
						char msgBuff[256] = { 0 };
						char templateBuff[256] = { 0 };
						g_GetStringRes(sid_level_up, templateBuff, sizeof(templateBuff));//%s升到了%d级
						sprintf(msgBuff, templateBuff, memberName, info.Level);
						msgBuff[sizeof(msgBuff) - 1] = 0;
						ShowSystemMessage(msgBuff);
					}
				}
			}
			else//该队员是新加入队员
			{
				int newMemberIndex = pTeam->AddMember(
					info.NpcId,
					info.Level,
					info.ClassAndSexInfo,
					info.Face,
					info.Name);
				if (newMemberIndex != INVALID_TEAM_MEMBER_INDEX)
				{
					ClientTeamMemberInfo* pMemberInfo = pTeam->GetMemberInfo(newMemberIndex);
					if (pMemberInfo != NULL)
						pMemberInfo->Type = (enumTeamMemberType)info.Type;
				}
				
				char msgBuff[256] = { 0 };
				char templateBuff[256] = { 0 };
				g_GetStringRes(sid_join_team, templateBuff, sizeof(templateBuff));//%s加入了队伍
				sprintf(msgBuff, templateBuff, memberName);
				msgBuff[sizeof(msgBuff) - 1] = 0;
				ShowSystemMessage(msgBuff);

				UpdateInterface();
				return;
			}
		}

		if (originalType != newType)//队伍中的职位发生了变化
		{
			char msgBuff[256] = { 0 };
			char templateBuff[256] = { 0 };
			int stringId = 0;
			switch(newType)
			{
			case teammember_type_normal:
				stringId = sid_be_dismiss;//%s被解职
				break;
			case teammember_type_captain:
				stringId = sid_new_captain;//%s是新的队长
				break;
			case teammember_type_assistant:
				stringId = sid_new_assistant;//%s被任命为助手
				break;
			}
			g_GetStringRes(stringId, templateBuff, sizeof(templateBuff));
			sprintf(msgBuff, templateBuff, memberName);
			msgBuff[sizeof(msgBuff) - 1] = 0;
			CoreDataChanged(GDCNI_APPEND_MESSAGE, (unsigned int)msgBuff, SYSTEM_ROOM_ID);
		}

		UpdateInterface();
	}
}

void KPlayerTeam::KickMember(DWORD kickNpcID)
{
	if (CanKick())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && pTeam->CanKick() && pTeam->GetCaptain() != kickNpcID)
			SendTeamOperation(team_operation_kick, kickNpcID);
	}
}

void KPlayerTeam::LevaveTeam()
{
	if (IsInTeam())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && pTeam->CanLeave())
			SendTeamOperation(team_operation_leave);		
	}
}

void KPlayerTeam::Dismiss()
{
	if (CanDismiss())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && pTeam->CanDismiss())
			SendTeamOperation(team_operation_dismiss);
	}
}

void KPlayerTeam::InviteAdd(DWORD inviteNpcID)
{
	if (inviteNpcID != Npc[GetClientPlayer().GetNpcIndex()].GetId())
	{
		if (IsInTeam())
		{
			KTeam* pTeam = GetTeam();
			if (pTeam != NULL && !pTeam->CanInvite())
				return;
		}
		
		SendTeamOperation(team_operation_invite, inviteNpcID);
	}
}

void KPlayerTeam::InviteAdd(const char* playerName)
{
	if (playerName != NULL &&
		strcmp(playerName, GetClientPlayer().GetPlayerName()) == 0)
		return;

	if (IsInTeam())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && !pTeam->CanInvite())
			return;
	}

	TEAM_INVITE_ADD_COMMAND	inviteAdd;
	inviteAdd.ProtocolType = c2s_teaminviteadd;
	strcpy(inviteAdd.PlayerName, playerName);
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&inviteAdd, sizeof(TEAM_INVITE_ADD_COMMAND));
}

void KPlayerTeam::ApplyJoinTeam(DWORD targetNpcID)
{
	if (targetNpcID != Npc[GetClientPlayer().GetNpcIndex()].GetId())
	{
		if (!IsInTeam())
		{
			SendTeamOperation(team_operation_apply_join, targetNpcID);
		}
	}
}

void KPlayerTeam::ApplyJoinTeam(const char* playerName)
{
}

void KPlayerTeam::NewCaptain(DWORD newCaptainNpcID)
{
	if (IsInTeam() && IsCaptain() && newCaptainNpcID != Npc[GetClientPlayer().GetNpcIndex()].GetId())
	{
		SendTeamOperation(team_operation_new_captain, newCaptainNpcID);
	}
}

void KPlayerTeam::PromoteAssistant(DWORD newAssistantNpcID)
{
	if (CanPromoteAssistant())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && pTeam->IsBigTeam() && pTeam->CanPromoteAssistant())
		{
			int memberIndex = pTeam->FindMemberID(newAssistantNpcID);
			if (memberIndex != INVALID_TEAM_MEMBER_INDEX)
			{
				ClientTeamMemberInfo* pMemberInfo = pTeam->GetMemberInfo(memberIndex);
				if (pMemberInfo != NULL && pMemberInfo->Type == teammember_type_normal)
				{
					SendTeamOperation(team_operation_promote_assistant, newAssistantNpcID);
				}
			}
		}
	}
}

void KPlayerTeam::DismissAssistant(DWORD assistantNpcID)
{
	if (CanDismissAssistant())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL && pTeam->IsBigTeam() && pTeam->CanDismissAssistant())
		{
			int memberIndex = pTeam->FindMemberID(assistantNpcID);
			if (memberIndex != INVALID_TEAM_MEMBER_INDEX)
			{
				ClientTeamMemberInfo* pMemberInfo = pTeam->GetMemberInfo(memberIndex);
				if (pMemberInfo != NULL && pMemberInfo->Type == teammember_type_assistant)
				{
					SendTeamOperation(team_operation_dismiss_assistant, assistantNpcID);
				}
			}
		}
	}
}

void KPlayerTeam::MoveIndex(int sourceIndex, int targetIndex)
{
	if (IsInTeam())
	{
		KTeam* pTeam = GetTeam();
		if (pTeam != NULL)
			pTeam->MoveIndex(sourceIndex, targetIndex);

		UpdateInterface();
	}
}

void KPlayerTeam::RequestTeamInfo(DWORD npcID)
{
	if (npcID <= 0)
		return;
	
	PLAYER_APPLY_TEAM_INFO applyInfo;
	applyInfo.ProtocolType = (BYTE)c2s_teamapplyinfo;
	applyInfo.m_dwTarNpcID = npcID;
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&applyInfo, sizeof(PLAYER_APPLY_TEAM_INFO));
}

void KPlayerTeam::RequestSelfTeamInfo()
{
	RequestTeamInfo(Npc[GetClientPlayer().GetNpcIndex()].GetId());
}

void KPlayerTeam::ReceiveApply(TEAM_APPLY_JOIN *pApply)
{
	if ( !pApply )
		return;

	// 通知界面有人申请加入自己的队伍
	KUiPlayerItem uiPlayer;
	uiPlayer.uId = pApply->PlayerNpcId;
	uiPlayer.Name[0] = 0;
	strncpy(uiPlayer.Name, pApply->PlayerName, sizeof(uiPlayer.Name));
	uiPlayer.Name[sizeof(uiPlayer.Name) - 1] = 0;
	
	CoreDataChanged(CDCNI_OPEN_APPLY_JOIN_TEAM_REPLY, (unsigned int)&uiPlayer, NULL);
}

void KPlayerTeam::ReplyApply(DWORD applyNpcID, enumApplyJoinTeamReplay result)
{
	switch(result)
	{
	case apply_jointeam_replay_refuse:
		SendTeamOperation(team_operation_apply_join_refuse, applyNpcID);
		break;
	case apply_jointeam_replay_accept:
		SendTeamOperation(team_operation_apply_join_accept, applyNpcID);
		break;
	default:
		break;
	}
}

KTeam* KPlayerTeam::GetTeam()
{
	return &g_TeamC[0];
}

void KPlayerTeam::SendTeamOperation(enumTeamOperation operation, DWORD npcId)
{
	TEAM_OPERATION teamOperation;
	teamOperation.Protocol = c2s_team_operation;
	teamOperation.Operation = (BYTE)operation;
	teamOperation.NpcId = npcId;
	
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID, &teamOperation, sizeof(TEAM_OPERATION));
}

int KPlayerTeam::GetAllTeamMemberInfo(KUiTeamMemberItem* teammateList)
{
	if (!IsInTeam() && teammateList == NULL)
		return 0;

	KTeam* pTeam = GetTeam();
	if (pTeam == NULL)
		return 0;

	DWORD selfNpcId = Npc[GetClientPlayer().GetNpcIndex()].GetId();

	int teammateCount = 0;//队友数量（不包括自己）
	int maxTeammemberCount = pTeam->GetMaxMemberCount();
	for (int memberLoopCount = 0; memberLoopCount < maxTeammemberCount; memberLoopCount++)
	{
		ClientTeamMemberInfo* pMemberInfo = pTeam->GetMemberInfo(memberLoopCount);
		if (pMemberInfo != NULL)
		{
			DWORD npcId = pMemberInfo->NpcId;
			if (npcId > 0 && selfNpcId != npcId)
			{
				KUiTeamMemberItem& teammateInfo = teammateList[teammateCount++];
				teammateInfo.m_uId = npcId;			
				strcpy(teammateInfo.m_szName, pMemberInfo->Name);
				//teammateInfo.m_nMaxLife = pMemberInfo->LifeMax;
				//teammateInfo.m_nCurLife = pMemberInfo->Life;
				//teammateInfo.m_nMaxMana = pMemberInfo->ManaMax;
				//teammateInfo.m_nCurMana = pMemberInfo->Mana;
				teammateInfo.m_LifePercent = pMemberInfo->LifePercent;
				teammateInfo.m_ManaPercent = pMemberInfo->ManaPercent;
				teammateInfo.m_nSeries = pMemberInfo->Class;
				teammateInfo.m_nSex = pMemberInfo->Sex;
				teammateInfo.m_nSkillSeries = pMemberInfo->SkillSeries;
				teammateInfo.m_nLevel = pMemberInfo->Level;
				teammateInfo.m_Type = pMemberInfo->Type;
				teammateInfo.m_bLeader = (pMemberInfo->Type == teammember_type_captain);
				teammateInfo.m_bIsNearBy = NpcSet.SearchID(npcId) > 0;
				teammateInfo.m_nBuffCount = pMemberInfo->BuffCount;
				teammateInfo.m_nPortrait = pMemberInfo->Face;
				memset(teammateInfo.m_BuffTemplateId, 0, sizeof(teammateInfo.m_BuffTemplateId));
				for (int buffLoopCount = 0; buffLoopCount < teammateInfo.m_nBuffCount; buffLoopCount++)
				{
					teammateInfo.m_BuffTemplateId[buffLoopCount] = pMemberInfo->BuffPair[buffLoopCount].ulBuffTempID;
				}
			}
		}
	}

	return teammateCount;
}

void KPlayerTeam::ProcessTeamOperation(TEAM_OPERATION_RESULT* pProtocol)
{
	if (pProtocol == NULL)
		return;

	enumTeamOperation operation = (enumTeamOperation)pProtocol->Operation;
	if (operation == team_operation_create)
	{
		OperationResultCreate(pProtocol);
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
					OperationResultOpenBigTeamMode(pProtocol);
					break;
				case team_operation_dismiss:
					//Dismiss();
					break;
				case team_operation_leave:
					//LeaveTeam();
					break;
				case team_operation_set_auto_accept_apply_on:
					m_AutoAcceptApply = true;
					break;
				case team_operation_set_auto_accept_apply_off:
					m_AutoAcceptApply = false;
					break;	
				default:
					{
// 						int npcIndex = NpcSet.SearchID(pProtocol->NpcId);
// 						if (npcIndex > 0)
// 						{
// 							int playerIndex = Npc[npcIndex].GetPlayerIdx();
// 							switch(operation)
// 							{
// 							case team_operation_invite:
// 								//InviteJoinTeam(playerIndex);
// 								break;
// 							case team_operation_kick:
// 								//KickMember(playerIndex);
// 								break;
// 							case team_operation_new_captain:
// 								//NewCaptain(playerIndex);
// 								break;
// 							case team_operation_promote_assistant:
// 								//PromoteAssistant(playerIndex);
// 								break;
// 							case team_operation_dismiss_assistant:
// 								//DismissAssistant(playerIndex);
// 								break;
// 								
// 							}
// 						}
					}
					break;
				}
			}
		}
	}	
}

void KPlayerTeam::OperationResultCreate(TEAM_OPERATION_RESULT* pProtocol)
{
	int opResult = pProtocol->Result;
	if (opResult >= 0)
	{
		int teamServerId = opResult;
		KNpc& selfNpc = Npc[GetClientPlayer().GetNpcIndex()];
		GetClientTeam().CreateTeam(selfNpc.GetId(), teamServerId);
		Release();
		SetInTeam(true);
		SetTeamMemberType(teammember_type_captain);
		m_nApplyCaptainID = 0;
		
		char msgBuff[COMMON_CLIENT_MSG_LEN_256] = { 0 };
		g_GetStringRes(sid_you_create_team, msgBuff, sizeof(msgBuff));
		msgBuff[sizeof(msgBuff) - 1] = 0;
		ShowErrorMessage(msgBuff);
	}
	else
	{
		char msgBuff[COMMON_CLIENT_MSG_LEN_256] = { 0 };
		int messageId = 11405;
		int errorMsg = opResult;
		switch (errorMsg)
		{
		case -1:
			//已经有一个队伍
			messageId = sid_you_already_has_team;
			break;
		case -2:
			//没有空闲的队伍槽位 
			messageId = sid_you_can_not_create_team;
			break;
		default:
			//其他原因导致的创建队伍失败
			messageId = sid_create_team_error;
			break;
		}
		g_GetStringRes(messageId, msgBuff, sizeof(msgBuff));
		msgBuff[sizeof(msgBuff) - 1] = 0;
		ShowErrorMessage(msgBuff);
	}
}

void KPlayerTeam::OperationResultOpenBigTeamMode(TEAM_OPERATION_RESULT* pProtocol)
{
	int opResult = pProtocol->Result;
	switch(opResult)
	{
	case TRUE:
		break;
	case FALSE:
		{
			char msgBuff[256] = { 0 };
			g_GetStringRes(sid_open_big_team_error, msgBuff, sizeof(msgBuff));
			msgBuff[sizeof(msgBuff) - 1] = 0;
			if (msgBuff[0] != 0)
				CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)msgBuff, 0);
		}
		break;
	}
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


void KPlayerTeam::SetAutoAcceptApply(bool isAuto)
{
	SendTeamOperation(isAuto ? team_operation_set_auto_accept_apply_on : team_operation_set_auto_accept_apply_off);
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

KTeam::KTeam()
{
	Release();
}

void KTeam::Release()
{
	m_nIndex			= -1;
	m_nCaptain			= 0;
	m_nMemNum			= 0;
	m_nState			= Team_S_Close;
	m_nShareExpFlag		= 0;
	m_nTeamServerID		= -1;
	m_bIsBigTeam		= false;
	for (int memberIndex = 0; memberIndex < MAX_BIG_TEAM_MEMBER; memberIndex++)
	{
		m_MemberInfo[memberIndex].NpcId = 0;
	}
	m_TeamSetting.reset();
}

void KTeam::SetIndex(int nIndex)
{
	m_nIndex = nIndex;
}

int KTeam::FindFree()
{
	for (int i = 0; i < MAX_BIG_TEAM_MEMBER; i++)
	{
		if (m_MemberInfo[i].NpcId == 0)
			return i;
	}
	return -1;
}

int KTeam::FindMemberID(DWORD dwNpcID)
{
	for (int i = 0; i <	MAX_BIG_TEAM_MEMBER; i++)
	{
		if (m_MemberInfo[i].NpcId > 0 && m_MemberInfo[i].NpcId == dwNpcID)
			return i;
	}

	return INVALID_TEAM_MEMBER_INDEX;
}

BOOL KTeam::SetTeamOpen()
{
	m_nState = Team_S_Open;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	return TRUE;
}

BOOL KTeam::SetExpShareType(int nShareExpType)
{
	if (nShareExpType < SHARE_EXP_CLS || nShareExpType > SHARE_EXP_AVG)
	{
		nShareExpType = SHARE_EXP_CLS;
	}
	m_nShareExpFlag = nShareExpType;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	return TRUE;
}

BOOL KTeam::SetTeamClose()
{
	m_nState = Team_S_Close;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	return TRUE;
}

int KTeam::GetMaxMemberCount() const
{
	return IsBigTeam() ? MAX_BIG_TEAM_MEMBER : MAX_TEAM_MEMBER;
}

void KTeam::CreateTeam(DWORD nCaptainNpcID, DWORD nTeamServerID)
{
	Release();
	m_nCaptain = nCaptainNpcID;
	m_nTeamServerID = nTeamServerID;
	m_nMemNum = 0;
}

int KTeam::AddMember(DWORD dwNpcID, int nLevel, BYTE classAndSexInfo, BYTE face, char *lpszNpcName, enumTeamMemberType type)
{
	if (FindMemberID(dwNpcID) >= 0)
		return INVALID_TEAM_MEMBER_INDEX;

	int freeMemberIndex = FindFree();
	if (freeMemberIndex < 0)
		return INVALID_TEAM_MEMBER_INDEX;

	ClientTeamMemberInfo& memberInfo = m_MemberInfo[freeMemberIndex];
	
	BYTE classInfo = classAndSexInfo & 0x0f;
	BYTE sexInfo = classAndSexInfo >> 4;	
	
	memberInfo.NpcId = dwNpcID;
	memberInfo.Type = type;
	memberInfo.SetName(lpszNpcName);
	memberInfo.Sex = sexInfo;
	memberInfo.Class = classInfo % 3;
	memberInfo.SkillSeries = classInfo / 3 - 1;
	memberInfo.Level = nLevel;
	memberInfo.Face = face;
	m_nMemNum++;

	return freeMemberIndex;
}

void KTeam::DeleteMember(DWORD dwNpcID)
{
	int memberIndex = FindMemberID(dwNpcID);
	if (memberIndex >= 0)
	{
		ClientTeamMemberInfo& memberInfo = m_MemberInfo[memberIndex];
		memberInfo.NpcId = 0;
		m_nMemNum--;
	}

	Rearrange();
}

void KTeam::SetMemberInfo(const TEAMMATE_INFO& info)
{
	int memberIndex = FindMemberID(info.Id);
	if (memberIndex >= 0)
	{
		ClientTeamMemberInfo& memberInfo = m_MemberInfo[memberIndex];
/*
带宽优化前
现在用百分比代替数值
 		memberInfo.Life = info.Life;
 		memberInfo.LifeMax = info.LifeMax;
 		memberInfo.Mana = info.Mana;
 		memberInfo.ManaMax = info.ManaMax;
*/
		memberInfo.LifePercent = info.LifePercent;
		memberInfo.ManaPercent = info.ManaPercent;

		memberInfo.MapId = info.MapId;
		memberInfo.PosX = info.PosX;
		memberInfo.PosY = info.PosY;
	}
}

void KTeam::OpenBigTeamMode()
{
	m_bIsBigTeam = true;
}

void KTeam::Rearrange()
{
	ClientTeamMemberInfo tempMemberInfo[MAX_BIG_TEAM_MEMBER];
	memcpy(tempMemberInfo, m_MemberInfo, sizeof(tempMemberInfo));
	memset(m_MemberInfo, 0, sizeof(m_MemberInfo));
	int newPos = 0;
	int maxMemberCount = GetMaxMemberCount();
	for (int memberIndex = 0; memberIndex < maxMemberCount; memberIndex++)
	{
		if (tempMemberInfo[memberIndex].NpcId > 0)
		{
			memcpy(&(m_MemberInfo[newPos++]), &(tempMemberInfo[memberIndex]), sizeof(ClientTeamMemberInfo));
		}
	}
	if (newPos > 0)
		m_nMemNum = newPos;
}

void KTeam::MoveIndex(int sourceIndex, int targetIndex)
{
	int maxMemberCount = GetMaxMemberCount();
	if (sourceIndex >= 0 && sourceIndex < maxMemberCount && targetIndex >= 0 && targetIndex < maxMemberCount && sourceIndex != targetIndex)
	{
		ClientTeamMemberInfo tempInfo;
		memcpy(&tempInfo, &(m_MemberInfo[sourceIndex]), sizeof(ClientTeamMemberInfo));
		memcpy(&(m_MemberInfo[sourceIndex]), &(m_MemberInfo[targetIndex]), sizeof(ClientTeamMemberInfo));
		memcpy(&(m_MemberInfo[targetIndex]), &tempInfo, sizeof(ClientTeamMemberInfo));
	}
}

TeamViewer::TeamViewer()
{
	Init();
}

TeamViewer::~TeamViewer()
{
	Init();
}

void TeamViewer::Init()
{
	m_TeamInfoList.clear();
	ReleaseAllTeamIcon();
}

void TeamViewer::AddNpc(int npcIndex)
{
	if (IsValidNpc(npcIndex))
	{
		KNpc& npc = Npc[npcIndex];

		if (kind_player != npc.m_Kind)
			return;

		int teamId = npc.m_UnaryAttrMgr[nuai_team_id];

		if (teamId > 0)
		{
			int teamListIndex = GetTeamInfoListIndex(teamId);
			if (teamListIndex < 0)
			{
				teamListIndex = AddTeamInfo(teamId);
			}
			else
			{
				m_TeamInfoList[teamListIndex].NpcCount++;
			}
			
			TeamViewInfo* pInfo = GetTeamViewInfo(teamListIndex);
			if (pInfo)
			{
				TeamIconInfo* pIcon = GetTeamIconInfo(pInfo->IconIndex);
				if (pIcon)
				{
					pIcon->InUse = true;
					npc.AddIconToHeadInfo(pIcon->ImageSet, pIcon->Image, TEAM_HEAD_ICON);
				}
			}
		}
	}
}

void TeamViewer::RemoveNpc(int npcIndex)
{
	if (IsValidNpc(npcIndex))
	{
		KNpc& npc = Npc[npcIndex];

		if (kind_player != npc.m_Kind)
			return;

		int teamId = npc.m_UnaryAttrMgr[nuai_team_id];
		
		const int teamListIndex = GetTeamInfoListIndex(teamId);
		if (teamListIndex >= 0)
		{
			m_TeamInfoList[teamListIndex].NpcCount--;
			if (0 == m_TeamInfoList[teamListIndex].NpcCount)
			{
				RemoveTeamInfo(teamListIndex);
			}
		}
	}
}

void TeamViewer::TeamChanged(int npcIndex, int originalTeamId, int newTeamId)
{
	if (originalTeamId == newTeamId)
		return;

	if (IsValidNpc(npcIndex))
	{
		KNpc& npc = Npc[npcIndex];

		if (kind_player != npc.m_Kind)
			return;

		const int teamListIndex = GetTeamInfoListIndex(originalTeamId);
		if (teamListIndex >= 0)
		{
			npc.DelHeadInfo(TEAM_HEAD_ICON);

			m_TeamInfoList[teamListIndex].NpcCount--;
			if (0 == m_TeamInfoList[teamListIndex].NpcCount)
			{
				RemoveTeamInfo(teamListIndex);
			}
		}

		int newListIndex = GetTeamInfoListIndex(newTeamId);
		if (newListIndex >= 0)
		{
			m_TeamInfoList[newListIndex].NpcCount++;
		}
		else
		{
			if (newTeamId > 0)
			{
				newListIndex = AddTeamInfo(newTeamId);
			}
		}

		TeamViewInfo* pNewInfo = GetTeamViewInfo(newListIndex);
		if (pNewInfo)
		{
			TeamIconInfo* pIcon = GetTeamIconInfo(pNewInfo->IconIndex);
			if (pIcon)
			{
				pIcon->InUse = true;
				npc.AddIconToHeadInfo(pIcon->ImageSet, pIcon->Image, TEAM_HEAD_ICON);
			}
		}

		//自己队伍发生改变需要做特殊处理
		if (npcIndex == GetClientPlayer().GetNpcIndex())
		{
			if (originalTeamId > 0)
			{
				ChangeTeamIcon(originalTeamId, GetFreeTeamIcon());
			}
			if (newTeamId > 0)
			{
				ChangeTeamIcon(newTeamId, 0);
			}
		}
	}
}

int TeamViewer::GetTeamInfoListIndex(int teamId) const
{
	if (teamId > 0)
	{
		const int teamCount = m_TeamInfoList.size();
		for (int teamIndex = 0; teamIndex < teamCount; teamIndex++)
		{
			if (m_TeamInfoList[teamIndex].TeamId == teamId)
				return teamIndex;
		}
	}

	return -1;
}

void TeamViewer::RemoveTeamInfo(int listIndex)
{
	if (listIndex >= 0 && listIndex < m_TeamInfoList.size())
	{
		ReleaseTeamIcon(m_TeamInfoList[listIndex].IconIndex);
		m_TeamInfoList.erase(m_TeamInfoList.begin() + listIndex);
	}
}

int TeamViewer::GetFreeTeamIcon() const
{
	//总是不会返回0，因为0是为自己的队伍预留的
	const int iconCount = m_TeamIconList.size();
	for (int iconIndex = 1; iconIndex < iconCount; iconIndex++)
	{
		if (!m_TeamIconList[iconIndex].InUse)
			return iconIndex;
	}

	return INVALID_TEAM_ICON_INDEX;
}

void TeamViewer::ReleaseTeamIcon(int iconIndex)
{
	if (iconIndex >= 0 && iconIndex < m_TeamIconList.size())
	{
		m_TeamIconList[iconIndex].InUse = false;
	}
}

void TeamViewer::ReleaseAllTeamIcon()
{
	const int iconCount = m_TeamIconList.size();
	for (int iconIndex = 0; iconIndex < iconCount; iconIndex++)
	{
		m_TeamIconList[iconIndex].InUse = false;
	}
}

int TeamViewer::AddTeamInfo(int teamId)
{
	TeamViewInfo info;
	info.TeamId = teamId;
	info.NpcCount = 1;
	info.IconIndex = GetFreeTeamIcon();

	m_TeamInfoList.push_back(info);
	return m_TeamInfoList.size() - 1;
}

void TeamViewer::LoadTeamIcon(const char* settingFile)
{
	KTabFile teamIconSetting;
	teamIconSetting.Load(settingFile);

	TeamIconInfo info;
	info.InUse = false;
	const int rowCount = teamIconSetting.GetHeight() - 1;
	if (rowCount > 0)
	{
		for (int row = 0; row < rowCount; row++)
		{
			teamIconSetting.GetString(row + 2, "ImageSet", "", info.ImageSet, sizeof(info.ImageSet));
			teamIconSetting.GetString(row + 2, "Image", "", info.Image, sizeof(info.Image));
			
			m_TeamIconList.push_back(info);
		}
	}
}

bool TeamViewer::ChangeTeamIcon(int teamId, int newIconIndex)
{
	TeamViewInfo* pTeamInfo = GetTeamViewInfo(GetTeamInfoListIndex(teamId));
	if (pTeamInfo)
	{
		if (pTeamInfo->IconIndex == newIconIndex)
			return false;

		ReleaseTeamIcon(pTeamInfo->IconIndex);
		pTeamInfo->IconIndex = newIconIndex;

		TeamIconInfo* pIcon = GetTeamIconInfo(newIconIndex);
		if (pIcon)
		{
			if (pIcon->InUse)
				return false;
			else
				pIcon->InUse = true;
		}
		
		for (int npcIndex = 0; npcIndex < MAX_NPC; npcIndex++)
		{
			KNpc& npc = Npc[npcIndex];
			if (npc.IsValid() && npc.m_Kind == kind_player)
			{
				int npcTeamId = npc.m_UnaryAttrMgr[nuai_team_id];				
				if (teamId == npcTeamId)
				{
					if (pIcon)
						npc.AddIconToHeadInfo(pIcon->ImageSet, pIcon->Image, TEAM_HEAD_ICON);
					else
						npc.DelHeadInfo(TEAM_HEAD_ICON);
				}
			}
		}

		return true;
	}

	return false;
}