//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/19/2006 17:18
//      File_base        : KPlayerTeam_S
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef KPLAYERTEAM_S_H
#define KPLAYERTEAM_S_H

#include "ChatDataDef.h"
#include "TeamDef.h"
#include <bitset>

using namespace CHAT;

class KTeam;

//组队邀请信息
struct InviteInfo
{
	int PlayerIndex;
	int Timeout;
};

//服务器端玩家的组队信息
class KPlayerTeam
{
public:
	KPlayerTeam();
	void Init(int playerIndex);
	void Active();

	KTeam* GetTeam();//得到所在队伍
	bool IsInTeam() const;//是否在队伍中
	int GetTeamId() const;//得到队伍ID
	void SetTeamId(int id);//设置队伍ID
	void SetTeamMemberType(enumTeamMemberType type);//设置队员类型
	bool IsCaptain() const;//是否是队长	
	bool CanInvite() const;//可以邀请其他玩家组队
	bool CanKick() const;//可以踢除队员
	bool CanDismiss() const;//可以解散队伍
	bool CanPromoteAssistant() const;//可以提升助手
	bool CanDismissAssistant() const;//可以解职助手
	bool CanOpenBigTeam();//可以开启大队伍
	bool IsAutoAcceptApply() const;//是否自动接受入队请求
	void SetAutoAcceptApply(bool isAuto);//设置是否自动接受入队请求

	bool CreateTeam();//创建队伍
	void OpenBigTeamMode();//开启大队伍模式
	void Dismiss();//解散队伍
	void LeaveTeam();//离开队伍
	void InviteJoinTeam(int invitePlayerIndex);//邀请某人加入自己的队伍
	void KickMember(int memberPlayerIndex);//把某个队员踢出队伍
	void NewCaptain(int newCaptainPlayerIndex);//把队长移交给其他队员
	void PromoteAssistant(int playerIndex);//提升助手
	void DismissAssistant(int playerIndex);//降职助手
	void ApplyJoinTeam(int targetPlayerIndex);//申请加入对方的队伍
	void ReplyApplyJoinTeam(int applyPlayerIndex, bool accept, bool autoAccept = false);//回复加入队伍申请

	void ProcessTeamOperation(TEAM_OPERATION* pProtocol);
	void ReplyInviteJoinTeam(int replyerPlayerIndex, enumInviteJointeamReplay reply);//回复加入队伍邀请		
	void ReceiveApplyJoinTeam(int applyPlayerIndex);//收到加入队伍的申请
	
	bool IsNeedSyncTeammateBuff() const;//是否需要同步队友BUFF
	void SetNeedSyncTeammateBuff(bool need);//设置是否需要同步队友BUFF
	void SendTeamSyncData();//发送队伍同步数据
	void SendSelfTeamInfo();//向自己发送队伍信息
	void SendMemberBasicInfo(unsigned int  clientId = 0);//向指定客户端发送队友基本信息
	void RequestTeamInfo(int playerIndex);//自己请求某个玩家的队伍信息
	void SendOperationResult(enumTeamOperation operation, DWORD npcId, int result);//发送操作结果

private:

	int GetInviteCount() const;//得到邀请计数
	int GetApplyCount() const;//得到申请计数
	bool IsInApplyList(int playerIndex) const;//是否在申请入队列表中
	int GetApplyListIndex(int playerIndex) const;//得到在申请列表中的Index
	int GetFreeApplySlot() const;//得到空闲的申请槽位
	bool IsInInviteList(int playerIndex) const;//是否在邀请入队列表中
	int GetFreeInviteSlot() const;//得到空闲的邀请槽位
	int GetInviteListIndex(int playerIndex) const;//得到在邀请列表中的Index
	bool NeedAutoDismiss();//是否需要自动删除

	int m_PlayerIndex;//玩家Index
	int m_nID;//队伍Index
	BOOL m_bCanTeamFlag;//是否可以组队
	enumTeamMemberType m_TeamMemberType;//队员类型
	InviteInfo m_nInviteList[MAX_BIG_TEAM_MEMBER];//邀请玩家加入本队伍列表
	InviteInfo m_ApplyList[MAX_BIG_TEAM_MEMBER];//申请加入本队伍的玩家列表
	bool m_NeedSyncTeammateBuff;//是否需要同步队友BUFF
	bool m_AutoAcceptApply;//是否自动接受加入队伍的请求
};

inline bool KPlayerTeam::IsCaptain() const
{
	return (m_TeamMemberType == teammember_type_captain);
}

inline bool KPlayerTeam::IsInTeam() const
{
	return (m_nID > 0);
}

inline int KPlayerTeam::GetTeamId() const
{
	return m_nID;
}

inline void KPlayerTeam::SetTeamId(int id)
{
	m_nID = id;
}

inline int KPlayerTeam::GetInviteCount() const
{
	int count = 0;
	for (int i = 0; i < MAX_BIG_TEAM_MEMBER; i++)
	{
		if (m_nInviteList[i].PlayerIndex > 0)
			count++;
	}

	return count;
}

inline int KPlayerTeam::GetApplyCount() const
{
	int count = 0;
	for (int i = 0; i < MAX_BIG_TEAM_MEMBER; i++)
	{
		if (m_ApplyList[i].PlayerIndex > 0)
			count++;
	}

	return count;
}

inline bool KPlayerTeam::IsNeedSyncTeammateBuff() const
{
	return m_NeedSyncTeammateBuff;
}

inline void KPlayerTeam::SetNeedSyncTeammateBuff(bool need)
{
	m_NeedSyncTeammateBuff = need;
}

inline bool KPlayerTeam::CanInvite() const
{
	return (IsInTeam() && (m_TeamMemberType == teammember_type_captain || m_TeamMemberType == teammember_type_assistant));
}

inline bool KPlayerTeam::CanKick() const
{
	return (IsInTeam() && (m_TeamMemberType == teammember_type_captain || m_TeamMemberType == teammember_type_assistant));
}

inline bool KPlayerTeam::CanDismiss() const
{
	return (IsInTeam() && m_TeamMemberType == teammember_type_captain);
}

inline bool KPlayerTeam::CanPromoteAssistant() const
{
	return (IsInTeam() && m_TeamMemberType == teammember_type_captain);
}

inline bool KPlayerTeam::CanDismissAssistant() const
{
	return (IsInTeam() && m_TeamMemberType == teammember_type_captain);
}

inline void KPlayerTeam::SetTeamMemberType(enumTeamMemberType type)
{
	m_TeamMemberType = type;
}

inline bool KPlayerTeam::IsInApplyList(int playerIndex) const
{
	return (GetApplyListIndex(playerIndex) >= 0);
}

inline int KPlayerTeam::GetApplyListIndex(int playerIndex) const
{
	for (int applyIndex = 0; applyIndex < MAX_BIG_TEAM_MEMBER; applyIndex++)
	{
		if (playerIndex == m_ApplyList[applyIndex].PlayerIndex)
			return applyIndex;
	}

	return -1;
}

inline int KPlayerTeam::GetFreeApplySlot() const
{
	for (int applyIndex = 0; applyIndex < MAX_BIG_TEAM_MEMBER; applyIndex++)
	{
		if (0 == m_ApplyList[applyIndex].PlayerIndex)
			return applyIndex;
	}

	return -1;
}

inline bool KPlayerTeam::IsInInviteList(int playerIndex) const
{
	return (GetInviteListIndex(playerIndex) >= 0);
}

inline int KPlayerTeam::GetInviteListIndex(int playerIndex) const
{
	for (int inviteIndex = 0; inviteIndex < MAX_BIG_TEAM_MEMBER; inviteIndex++)
	{
		if (playerIndex == m_nInviteList[inviteIndex].PlayerIndex)
			return inviteIndex;
	}

	return -1;
}

inline int KPlayerTeam::GetFreeInviteSlot() const
{
	for (int inviteIndex = 0; inviteIndex < MAX_BIG_TEAM_MEMBER; inviteIndex++)
	{
		if (0 == m_nInviteList[inviteIndex].PlayerIndex)
			return inviteIndex;
	}

	return -1;
}

inline bool KPlayerTeam::IsAutoAcceptApply() const
{
	return m_AutoAcceptApply;
}

inline void KPlayerTeam::SetAutoAcceptApply(bool isAuto)
{
	m_AutoAcceptApply = isAuto;
}

class KTeam
{
public:
	KTeam();
	void Release();

	bool IsValid() const;//是否合法（有人在使用）
	int GetId() const;//得到队伍ID
	void SetIndex(int nIndex);//设定Team在g_Team中的位置
	int GetCaptain() const;//得到队长
	int GetMemberCount() const;//得到队员数量（包括队长）
	void Create(int creater);//创建
	void Dismiss();//解散
	int AddMember(int member);//添加
	bool DeleteMember(int member);//删除
	bool IsFull();//判断队伍是否已经满员
	int	FindFree();//寻找队员空位
	int GetMemberPlayerIndex(int member) const;//得到队友列表制定位置的PlayerIndex
	bool IsMember(int playerIndex);//判断某人是否是队伍成员	
	int FindByPlayerIndex(int playerIndex) const;
	bool NewCaptain(int member);//设置新的队长
	bool IsBigTeam() const;//是否是大队伍
	void OpenBigTeamMode();//开启大队伍模式
	int GetMaxMemberCount() const;
	bool IsShareExp() const;
	bool HasExp() const;
	enumTeamMemberType GetMemberType(int member) const;
	void SetMemberType(int member, enumTeamMemberType type, bool syncToClient = false);
	bool IsFull() const;//是否满员
	DWORD GetInstanceId(int worldTemplateId) const;//得到副本编号
	void SetInstanceId(int worldTemplateId, DWORD instanceId);//设置副本编号
	unsigned long GetTeamSetting() const;
	void SetTeamSetting(unsigned long teamSetting);
	bool CanKick() const;//可否踢人
	bool CanInvite() const;//可否邀请
	bool CanPromoteAssistant() const;//可否提升助手
	bool CanDismissAssistant() const;//可否撤职助手
	bool CanLeave() const;//可否离开
	bool CanOpenBigTeam() const;//可否开启大队伍
	bool CanDismiss() const;//可否解散
	void SetCanKick(bool canKick);
	void SetCanInvite(bool canInvite);
	void SetCanPromoteAssistant(bool canPromote);
	void SetCanDismissAssistant(bool canDismiss);
	void SetCanLeave(bool canLeave);
	void SetCanOpenBigTeam(bool canOpenBigTeam);
	void SetCanDismiss(bool canDismiss);
	void SetBindWorld(int worldIndex, int worldTeamIndex);	
	
	void ForEach(CHATCALLBACK pCallBack,  const void *pCallbackParam,
				BROADCASTFILTER pFilter = NULL, unsigned int uFilterPassby = 0);

private:
	void TeamChanged();

	int m_nIndex;//本Team在g_Team中的位置
	bool m_bIsBigTeam;//是否开启了大队伍模式
	enumTeamMemberType m_MemberType[MAX_BIG_TEAM_MEMBER];//成员的类型
	DWORD m_InstanceId[INSTANCE_SUBWORLD_START];//副本编号
	int m_BindWorldIndex;//绑定的地图
	int m_BindWorldTeamIndex;//在绑定的地图中的序号
	std::bitset<team_setting_count> m_TeamSetting;//队伍设置
	int m_nCaptain;//队长PlayerIndex
	int m_nMemNum;//成员数量
	int m_nMember[MAX_BIG_TEAM_MEMBER];//成员的PlayerIndex
};

inline bool KTeam::IsValid() const
{
	return m_nCaptain > 0;
}

inline int KTeam::GetId() const
{
	return m_nIndex;
}

inline int KTeam::GetCaptain() const
{
	return m_nCaptain;
}

inline int KTeam::GetMemberCount() const
{
	return m_nMemNum;
}

inline int KTeam::GetMemberPlayerIndex(int member) const
{
	if (member >= 0 && member < MAX_BIG_TEAM_MEMBER)
		return m_nMember[member];
	else
		return INVALID_PLAYER_INDEX;
}

inline bool KTeam::IsBigTeam() const
{
	return m_bIsBigTeam;
}

inline int KTeam::GetMaxMemberCount() const
{
	return m_bIsBigTeam ? MAX_BIG_TEAM_MEMBER : MAX_TEAM_MEMBER;
}

inline bool KTeam::IsShareExp() const
{
	return !IsBigTeam();
}

inline bool KTeam::HasExp() const
{
	return !IsBigTeam();
}

inline enumTeamMemberType KTeam::GetMemberType(int member) const
{
	return m_MemberType[member];
}

inline bool KTeam::IsMember(int playerIndex)
{
	return (FindByPlayerIndex(playerIndex) != INVALID_TEAM_MEMBER_INDEX);
}

inline bool KTeam::IsFull() const
{
	return (GetMemberCount() >= GetMaxMemberCount());
}

inline DWORD KTeam::GetInstanceId(int worldTemplateId) const
{
	if (worldTemplateId >= 0 && worldTemplateId < INSTANCE_SUBWORLD_START)
		return m_InstanceId[worldTemplateId];
	else
		return INVALID_INSTANCE_ID;
}

inline void KTeam::SetInstanceId(int worldTemplateId, DWORD instanceId)
{
	if (worldTemplateId >= 0 && worldTemplateId < INSTANCE_SUBWORLD_START)
		m_InstanceId[worldTemplateId] = instanceId;
}

inline unsigned long KTeam::GetTeamSetting() const
{
	return m_TeamSetting.to_ulong();
}

inline void KTeam::SetTeamSetting(unsigned long teamSetting)
{
	m_TeamSetting = std::bitset<team_setting_count>(teamSetting);
}

inline bool KTeam::CanKick() const
{
	return m_TeamSetting.test(team_setting_can_kick);
}

inline bool KTeam::CanInvite() const
{
	return m_TeamSetting.test(team_setting_can_invite);
}

inline bool KTeam::CanPromoteAssistant() const
{
	return m_TeamSetting.test(team_setting_can_promote_assistant);
}

inline bool KTeam::CanDismissAssistant() const
{
	return m_TeamSetting.test(team_setting_can_dismiss_assistant);
}

inline bool KTeam::CanLeave() const
{
	return m_TeamSetting.test(team_setting_can_leave);
}

inline bool KTeam::CanOpenBigTeam() const
{
	return m_TeamSetting.test(team_setting_can_open_big_team);
}

inline bool KTeam::CanDismiss() const
{
	return m_TeamSetting.test(team_setting_can_dismiss);
}

inline void KTeam::SetCanKick(bool canKick)
{
	m_TeamSetting.set(team_setting_can_kick, canKick);
}

inline void KTeam::SetCanInvite(bool canInvite)
{
	m_TeamSetting.set(team_setting_can_invite, canInvite);
}

inline void KTeam::SetCanPromoteAssistant(bool canPromote)
{
	m_TeamSetting.set(team_setting_can_promote_assistant, canPromote);
}

inline void KTeam::SetCanDismissAssistant(bool canDismiss)
{
	m_TeamSetting.set(team_setting_can_dismiss_assistant, canDismiss);
}

inline void KTeam::SetCanLeave(bool canLeave)
{
	m_TeamSetting.set(team_setting_can_leave, canLeave);
}

inline void KTeam::SetCanOpenBigTeam(bool canOpenBigTeam)
{
	m_TeamSetting.set(team_setting_can_open_big_team, canOpenBigTeam);
}

inline void KTeam::SetCanDismiss(bool canDismiss)
{
	m_TeamSetting.set(team_setting_can_dismiss, canDismiss);
}

inline void KTeam::SetBindWorld(int worldIndex, int worldTeamIndex)
{
	m_BindWorldIndex = worldIndex;
	m_BindWorldTeamIndex = worldTeamIndex;
}

extern	KTeam	g_TeamS[MAX_TEAM];

enum enumListTeamMode
{
	list_team_mode_from_begin,
	list_team_mode_random,
};

class KTeamSet
{
public:
	typedef bool (*TeamFilter)(KTeam* pTeam, void* pPassby);

	void Init();//初始化
	int CreateTeam(int nPlayerID);//创建队伍
	int RemoveTeam(int nIdx);//删除队伍
	KTeam* GetTeam(int teamId);//得到队伍
	int ListTeam(TeamBasicInfo* pTeamInfoBuff, int outputLimit, enumListTeamMode listMode, KTeamSet::TeamFilter pFilter, void* pFilterPassby) const;//列出队伍

	static bool InSubworld(KTeam* pTeam, void *pPassby);//是否在参数指定的地图上，*pPassby：int，地图ID
private:

	KLinkArray		m_FreeIdx;				//	可用表
	int				m_nListCurIdx;			// 用于 GetFirstPlayer 和 GetNextPlayer
};

inline KTeam* KTeamSet::GetTeam(int teamId)
{
	if (teamId >= 0 && teamId < MAX_TEAM)
		return &g_TeamS[teamId];
	else
		return NULL;
}

extern	KTeamSet	g_TeamSet;


#endif