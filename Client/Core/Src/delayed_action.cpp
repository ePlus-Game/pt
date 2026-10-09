//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-18
//      File_base        : delayed_action
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 延迟的动作
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KObjSet.h"
#include "KPlayer.h"
#include "KSubWorld.h"
#include "ConfigManager.h"
#include "delayed_action.h"
#include "KItemEnchaser.h"
#include "buff_man.h"
#include "KSubWorldSet.h"
#include "ChatCenter_S.h"

DelayedAction::DelayedAction()
{
	Abandon();
}

DelayedAction::DelayedAction(int playerIndex, DelayedActionType actionType, int totalTime, int messageId)
{
	m_PlayerIndex = playerIndex;
	m_ActionType = actionType;
	m_MessageId = messageId;
	m_TotalTime = totalTime;
	m_RemainingTime = totalTime;	
}

DelayedAction::~DelayedAction()
{
}

DelayedAction& DelayedAction::operator= (const DelayedAction& delayedAction)
{
	if(this != &delayedAction)
	{
		memcpy(this, &delayedAction, sizeof(DelayedAction));
	}

	return *this;
}

void DelayedAction::Do()
{
	switch(m_ActionType)
	{
	case delayed_action_pickup_item:
		DoPickupObject();
		break;
	case delayed_action_dialog_npc:
		DoDialogNpc();
		break;
	case delayed_action_use_item:
		DoUseItem();
		break;
	case delayed_action_smith:
		DoSmith();
		break;
	case delayed_action_add_buff:
		DoAddBuff();
		break;
	case delayed_action_transfer:
		DoTransfer();
		break;
	default:
		_ASSERT(false);
		break;
	}

	Abandon();
}

void DelayedAction::Abandon()
{
	m_PlayerIndex = 0;
	m_ActionType = delayed_action_invalid;
	m_MessageId = 0;
	m_RemainingTime = 0;
	m_TotalTime = 0;
}

bool DelayedAction::Delay(int delayTime)
{
	int tempTime = m_RemainingTime + delayTime;
	m_RemainingTime = (tempTime < m_TotalTime) ? tempTime : m_TotalTime;

	SendMsgToClient(DA_Delay, delayTime, delayed_action_msg_none);

	return true;
}

bool DelayedAction::Cancel()
{
	if (m_RemainingTime <= 0)
		return false;

	switch(m_ActionType)
	{
	case delayed_action_add_buff:
		{
			Player[m_PlayerIndex].CancelPromptAddBuff();
		}		
		break;
	case delayed_action_transfer:
		{
			g_ChatCenterS.SysMsgToSomeone(m_PlayerIndex, SYSMSG_TYPE_STR, (const BYTE*)MSG_DELAYED_TRANSFER_CANCELED, strlen(MSG_DELAYED_TRANSFER_CANCELED));
		}
		break;
	default:
		{
			SendMsgToClient(DA_Cancel, 0, delayed_action_msg_none);
		}
		break;
	}
	
	Abandon();

	return true;
}

void DelayedAction::SendMsgToClient(int command, int time, int messageId)
{
	if (IsValidPlayer(m_PlayerIndex))
	{
		DELAYED_ACTION delayedAction;
		delayedAction.Protocol = s2c_delayed_action;
		delayedAction.Command = command;
		delayedAction.Time = time;
		delayedAction.Message = messageId;
		
		int netConnectionIndex = Player[m_PlayerIndex].GetNetConnectIdx();	
		
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(netConnectionIndex, (BYTE*)&delayedAction, sizeof(DELAYED_ACTION));
	}
}

void DelayedAction::OnEvent(DelayedActionEvent eventType, void* pEventParam)
{
	switch(m_ActionType)
	{
	case delayed_action_pickup_item:
		switch(eventType)
		{
		case delayed_action_event_hurt:
			Delay(ConfigManager::Singleton().GetGlobalVariable(global_var_damage_action_delay));
			break;
		case delayed_action_event_move:
		case delayed_action_event_death:
			Cancel();
			break;
		}
		break;
	case delayed_action_dialog_npc:
		switch(eventType)
		{
		case delayed_action_event_hurt:
			Delay(ConfigManager::Singleton().GetGlobalVariable(global_var_damage_action_delay));
			break;
		case delayed_action_event_move:
		case delayed_action_event_death:
			Cancel();
			break;
		}
		break;
	case delayed_action_use_item:
		switch(eventType)
		{
		case delayed_action_event_hurt:
			Delay(ConfigManager::Singleton().GetGlobalVariable(global_var_damage_action_delay));
			break;
		case delayed_action_event_move:
		case delayed_action_event_death:
			Cancel();
			break;
		}
		break;
	case delayed_action_smith:
		switch(eventType)
		{
		case delayed_action_event_hurt:
			Delay(ConfigManager::Singleton().GetGlobalVariable(global_var_damage_action_delay));
			break;
		case delayed_action_event_move:
		case delayed_action_event_death:
			Cancel();
			break;
		}
		break;
	case delayed_action_add_buff:
		break;
	case delayed_action_transfer:
		break;
	}
}

void DelayedAction::OnTime(int escapedTime)
{
	int tempTime = m_RemainingTime - escapedTime;
	if (tempTime < 0)
		tempTime = 0;
	m_RemainingTime = tempTime;

	switch(m_ActionType)
	{
	case delayed_action_transfer:
		{
			static int notifyTime[] = { 60, 45, 30, 25, 20, 15, 10, 5, 4, 3, 2, 1 };
			for (int i = 0; i < sizeof(notifyTime) / sizeof(int); i++)
			{
				if (m_RemainingTime == notifyTime[i])
				{
					DelayedTransferNotify();
					break;
				}
			}
		}
		break;
	default:
		break;
	}

	if (m_RemainingTime == 0)
	{
		Do();
	}
}

void DelayedAction::DoPickupObject()
{
	DelayedActionParamPickupObject& param = m_Param.PickupObject;
	Player[m_PlayerIndex].DoPickupObject(param.m_PickupObjectId, param.m_PickupObjectPosX, param.m_PickupObjectPosY);
}

void DelayedAction::DoDialogNpc()
{
	DelayedActionParamDialogNpc& param = m_Param.DialogNpc;
	Player[m_PlayerIndex].DoDialogNpc(param.m_DialogNpcId);
}

void DelayedAction::DoUseItem()
{
	DelayedActionParamUseItem& param = m_Param.UseItem;
	KItemList& itemList = Player[m_PlayerIndex].GetItemList();

	int itemIndex = itemList.SearchID(param.m_UseItemId);
	if (itemIndex > 0)
	{
		int nTargetIdx = 0;
		if (param.m_UseItemTargetId > 0)
		{
			if ( Item[itemIndex].IsPlayerTarget() )
			{
				nTargetIdx = NpcSet.SearchID( param.m_UseItemTargetId );
			}
			else
			{
				nTargetIdx = itemList.SearchID(param.m_UseItemTargetId);
			}
		}

		itemList.EatMecidine(itemIndex, nTargetIdx);
	}
}

void DelayedAction::DoSmith()
{
	DelayedActionParamSmith& param = m_Param.Smith;
	g_ItemEnchaser.doSmith((TCompoundResultEx*)param.m_pSmithResult);
}

void DelayedAction::DoAddBuff()
{
	DelayedActionParamAddBuff& param = m_Param.AddBuff;
	if (param.m_BuffId > 0 && param.m_Accept)
	{
		BUFF_PARAM buffParam;
		buffParam[1] = 1;
		BuffMgr::Singleton().AddNpcBuff(param.m_BuffSender, m_PlayerIndex, param.m_BuffId, &buffParam);
	}
	Player[m_PlayerIndex].CancelPromptAddBuff();
}

void DelayedAction::DoTransfer()
{
	DelayedActionParamTransfer& param = m_Param.Transfer;
	if (param.m_Accept)
	{
		int npcIndex = Player[m_PlayerIndex].GetNpcIndex();

		//如果处于死亡状态，先复活后传送
		if (IsValidNpc(npcIndex) && (Npc[npcIndex].m_Doing == do_revive || Npc[npcIndex].m_Doing == do_death))
		{
			Player[m_PlayerIndex].Revive(0);
		}

		Npc[npcIndex].ChangeWorld(param.m_TransferID, param.m_PosX, param.m_PosY, param.m_IsInstance);
	}
}

void DelayedAction::DelayedTransferNotify()
{	
	if (m_RemainingTime > 0 && m_RemainingTime != m_LastNotifyRemainingTime)
	{
		m_LastNotifyRemainingTime = m_RemainingTime;

		DelayedActionParamTransfer& param = m_Param.Transfer;
		int subWorldIndex = INVALID_WORLD_INDEX;
		if (param.m_IsInstance)
		{
			subWorldIndex = g_SubWorldSet.GetInstance(param.m_TransferID);			
		}
		else
		{
			subWorldIndex = g_SubWorldSet.SearchWorld(param.m_TransferID);
		}
		
		if (subWorldIndex != INVALID_WORLD_INDEX)
		{
			WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(SubWorld[subWorldIndex].GetWorldTemplateId());
			if (pSetting)
			{
				char message[256] = {0};
				snprintf(message, sizeof(message), MSG_DELAYED_TRANSFER_NOTIFY, m_RemainingTime, pSetting->Name);
				message[sizeof(message) - 1] = 0;
				g_ChatCenterS.SysMsgToSomeone(m_PlayerIndex, SYSMSG_TYPE_STR, (const BYTE*)message, strlen(message));
			}
		}
	}
}