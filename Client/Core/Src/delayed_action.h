//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-18
//      File_base        : delayed_action
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 延迟的动作
//
//////////////////////////////////////////////////////////////////////

#ifndef _DELAYED_ACTION_H_
#define _DELAYED_ACTION_H_

//延迟动作类型
enum DelayedActionType
{
	delayed_action_invalid = -1,	//非法的

	delayed_action_pickup_item = 0,	//拾取物品
	delayed_action_dialog_npc,		//对话NPC
	delayed_action_use_item,		//使用物品
	delayed_action_smith,			//打造
	delayed_action_add_buff,		//添加BUFF
	delayed_action_transfer,		//传送

	delayed_action_count,			//类型数量
};

//延迟动作相关的事件
enum DelayedActionEvent
{
	delayed_action_event_hurt = 0,
	delayed_action_event_move,
	delayed_action_event_skill,
	delayed_action_event_death,
};

//拾取物品
struct DelayedActionParamPickupObject
{
	int m_PickupObjectId;
	int m_PickupObjectPosX;
	int m_PickupObjectPosY;
};

//对话NPC
struct DelayedActionParamDialogNpc
{
	int m_DialogNpcId;
};

//使用物品
struct DelayedActionParamUseItem
{
	DWORD m_UseItemId;
	DWORD m_UseItemTargetId;
};

//打造
struct DelayedActionParamSmith
{
	void* m_pSmithResult;
};

//添加BUFF
struct DelayedActionParamAddBuff
{
	bool m_Accept;
	int m_BuffId;
	int m_BuffSender;
};

//传送
struct DelayedActionParamTransfer
{
	bool m_CanDeny;
	bool m_Accept;
	DWORD m_TransferID;
	bool m_IsInstance;
	int m_PosX;
	int m_PosY;
};

//延迟的动作
class DelayedAction
{
public:
	DelayedAction();
	DelayedAction(int playerIndex, DelayedActionType actionType, int totalTime, int messageId);
	~DelayedAction();
	DelayedAction& operator= (const DelayedAction& delayedAction);

	DelayedActionType GetType() const;//类型
	int GetMessageId() const;//消息编号
	bool IsValid() const;//是否合法
	bool Delay(int delayTime);//延迟
	bool Cancel();//取消
	void Do();//执行动作
	void Abandon();//放弃
	void OnEvent(DelayedActionEvent eventType, void* pEventParam);//发生事件
	void OnTime(int escapedTime);//过了一段时间
	int GetTotalTime() const;//总计时间
	int GetRemainingTime() const;//剩余时间
	void SetRemainingTime(int remainingTime);//设置剩余时间

	DelayedActionParamPickupObject& GetPickupObjectParam();//拾取物品参数
	DelayedActionParamDialogNpc& GetDialogNpcParam();//对话NPC参数
	DelayedActionParamUseItem& GetUseItemParam();//使用物品参数
	DelayedActionParamSmith& GetSmithParam();//打造参数
	DelayedActionParamAddBuff& GetAddBuffParam();//添加BUFF参数
	DelayedActionParamTransfer& GetTransferParam();//传送参数

private:
	void SendMsgToClient(int command, int time, int messageId);//发送消息到客户端

	void DoPickupObject();
	void DoDialogNpc();
	void DoUseItem();
	void DoSmith();
	void DoAddBuff();
	void DoTransfer();

	void DelayedTransferNotify();

	int m_PlayerIndex;
	DelayedActionType m_ActionType;
	int m_MessageId;
	int m_TotalTime;
	int m_RemainingTime;
	int m_LastNotifyRemainingTime;

	//动作参数
	union
	{
		DelayedActionParamPickupObject PickupObject;
		DelayedActionParamDialogNpc DialogNpc;
		DelayedActionParamUseItem UseItem;
		DelayedActionParamSmith Smith;
		DelayedActionParamAddBuff AddBuff;
		DelayedActionParamTransfer Transfer;
	} m_Param;
};

inline DelayedActionType DelayedAction::GetType() const
{
	return m_ActionType;
}

inline bool DelayedAction::IsValid() const
{
	return (m_ActionType != delayed_action_invalid);
}

inline int DelayedAction::GetMessageId() const
{
	return m_MessageId;
}

inline int DelayedAction::GetTotalTime() const
{
	return m_TotalTime;
}

inline int DelayedAction::GetRemainingTime() const
{
	return m_RemainingTime;
}

inline DelayedActionParamPickupObject& DelayedAction::GetPickupObjectParam()
{
	return m_Param.PickupObject;
}

inline DelayedActionParamDialogNpc& DelayedAction::GetDialogNpcParam()
{
	return m_Param.DialogNpc;
}

inline DelayedActionParamUseItem& DelayedAction::GetUseItemParam()
{
	return m_Param.UseItem;
}

inline DelayedActionParamSmith& DelayedAction::GetSmithParam()
{
	return m_Param.Smith;
}

inline DelayedActionParamAddBuff& DelayedAction::GetAddBuffParam()
{
	return m_Param.AddBuff;
}

inline DelayedActionParamTransfer& DelayedAction::GetTransferParam()
{
	return m_Param.Transfer;
}

#endif// _DELAYED_ACTION_H_