//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-17
//      File_base        : action_delayer
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 动作延迟器
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "action_delayer.h"
#include "KNpc.h"
#include "KSubWorld.h"

ActionDelayer::ActionDelayer()
{
}

ActionDelayer::~ActionDelayer()
{
}

void ActionDelayer::Init(int npcIndex)
{
	m_NpcIndex = npcIndex;
	m_LastActiveTime = 0;
	m_DelayedAction.Abandon();
	for (int i = 0; i < delayed_action_count; i++)
	{
		m_IndependentDelayedActions[i].Abandon();
	}
}

void ActionDelayer::Active()
{
	unsigned long currentTime = GetCurrentTime();
	int escapedTime = 0;
	if (m_LastActiveTime > 0)
	{
		escapedTime = (currentTime - m_LastActiveTime);
	}
	m_LastActiveTime = currentTime;

	if (m_DelayedAction.IsValid())
		m_DelayedAction.OnTime(escapedTime);
	
	for (int i = 0; i < delayed_action_count; i++)
	{
		if (m_IndependentDelayedActions[i].IsValid())
			m_IndependentDelayedActions[i].OnTime(escapedTime);
	}
}

bool ActionDelayer::NewAction(DelayedAction& action, bool independent)
{
	if (action.GetType() < 0 || action.GetType() >= delayed_action_count)
		return false;

	DelayedAction* pDelayedAction = NULL;
	if (independent)
	{
		if (HasAction(action.GetType()))
		{
			return false;
		}
		pDelayedAction = &(m_IndependentDelayedActions[action.GetType()]);
	}
	else
	{
		if (HasAction())//已经有动作在进行
		{
			if (!m_DelayedAction.Cancel())
				return false;
		}
		pDelayedAction = &m_DelayedAction;
	}
	
	if (pDelayedAction)
	{
		*pDelayedAction = action;
		if (pDelayedAction->GetTotalTime() > 0)
		{
			if (!independent)
				SendMsgToClient(DA_New, pDelayedAction->GetTotalTime(), pDelayedAction->GetMessageId());
		}
		else
		{
			pDelayedAction->Do();
		}

		return true;
	}
	else
	{
		return false;
	}
}

unsigned long ActionDelayer::GetCurrentTime()
{
	if (m_NpcIndex > 0)
	{
		return UNIX_TMIE_STAMP;
	}
	else
	{
		return 0;
	}
}

void ActionDelayer::SendMsgToClient(int command, int time, int messageId)
{
	DELAYED_ACTION delayedAction;
	delayedAction.Protocol = s2c_delayed_action;
	delayedAction.Command = command;
	delayedAction.Time = time;
	delayedAction.Message = messageId;

	int netConnectionIndex = Player[Npc[m_NpcIndex].GetPlayerIdx()].GetNetConnectIdx();	

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(netConnectionIndex, (BYTE*)&delayedAction, sizeof(DELAYED_ACTION));
}

void ActionDelayer::OnEvent(DelayedActionEvent eventType, void* pEventParam)
{
	if (m_DelayedAction.IsValid())
		m_DelayedAction.OnEvent(eventType, pEventParam);

	for (int i = 0; i < delayed_action_count; i++)
	{
		if (m_IndependentDelayedActions[i].IsValid())
			m_IndependentDelayedActions[i].OnEvent(eventType, pEventParam);
	}
}

DelayedAction* ActionDelayer::GetAction(DelayedActionType type)
{
	if (HasAction(type))
	{
		return &(m_IndependentDelayedActions[type]);
	}

	return NULL;
}