//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-17
//      File_base        : action_delayer
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 动作延迟器
//
//////////////////////////////////////////////////////////////////////

#ifndef _ACTION_DELAYER_H_
#define _ACTION_DELAYER_H_

#include "delayed_action.h"

#define INVALID_INDEPENDENT_DELAYED_ACTION_IDNEX -1

//动作延迟器
class ActionDelayer
{
public:
	ActionDelayer();
	~ActionDelayer();

	void Init(int npcIndex);//初始化
	void Active();//活动一次
	bool HasAction();//是否有动作正在进行
	bool HasAction(DelayedActionType type);//是否有指定类型的独立动作正在进行
	bool NewAction(DelayedAction& action, bool independent = false);//新的动作
	void OnEvent(DelayedActionEvent eventType, void* pEventParam = NULL);//发生事件
	DelayedAction* GetAction(DelayedActionType type);
	
private:
	unsigned long GetCurrentTime();//取得当前时间
	void SendMsgToClient(int command, int time, int messageId);//发送消息到客户端

	int m_NpcIndex;//NPC序号
	unsigned long m_LastActiveTime;//上一次Active的时间
	DelayedAction m_DelayedAction;//延迟的动作
	DelayedAction m_IndependentDelayedActions[delayed_action_count];//独立的延迟动作
};

inline bool ActionDelayer::HasAction()
{
	return m_DelayedAction.IsValid();
}

inline bool ActionDelayer::HasAction(DelayedActionType type)
{
	return m_IndependentDelayedActions[type].IsValid();
}

#endif// _ACTION_DELAYER_H_