//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/16/2006 17:20
//      File_base        : KPlayerTeam_C
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef KPLAYERTEAM_C_H
#define KPLAYERTEAM_C_H

#include "GameDataDef.h"
#include "TeamDef.h"
#include "buff_def.h"
#include <bitset>

// 客户端保存在队长 player 身上的队伍申请人列表信息
struct KTeamApplyList	// 申请加入队伍者列表中的申请人信息
{
	KTeamApplyList() 
	{
		Release();
	}
	inline void	Release() 
	{
		m_dwNpcID	= 0; 
		m_dwTimer	= 0; 
		m_nLevel	= 0; 
		m_szName[0]	= 0;
	}
	DWORD	m_dwNpcID;									// 申请人 npc id
	DWORD	m_dwTimer;									// 申请时间计数器
	int		m_nLevel;									// 申请人等级
	char	m_szName[COMMON_CLIENT_MSG_LEN_128];		// 申请人姓名
};				

class KTeam;

//客户端组队信息
class KPlayerTeam
{
public:
	KPlayerTeam();
	void Release();
	void ReleaseList();
	
	KTeam* GetTeam();//得到队伍
	void ReceiveInvite(TEAM_INVITE_ADD_SYNC *pInvite);//收到邀请
	void ReplyInvite(int nIdx, enumInviteJointeamReplay nResult);//回复邀请	
	void SetAutoRefuseInvite(BOOL bFlag);//设定是否自动拒绝别人的加入队伍的邀请
	BOOL IsAutoRefuseInvite();//获得是否自动拒绝别人的加入队伍的邀请状态
	int GetInfo (KUiPlayerTeam *pTeam);//获得自身队伍信息（给界面）
	void UpdateInterface(bool bShow=false);//更新界面显示
	void DeleteOneFromApplyList(DWORD dwNpcID);//从申请人列表中删除某个申请人
	bool CanInvite() const;//可以邀请其他玩家组队
	bool CanKick() const;//可以踢除队员
	bool CanDismiss() const;//可以解散队伍
	bool CanPromoteAssistant() const;//可以提升助手
	bool CanDismissAssistant() const;//可以解职助手
	bool CanOpenBigTeam();//可以开启大队伍
	bool IsInTeam() const;//是否在队伍中
	void SetInTeam(bool inTeam);//设置是否在队伍中
	bool IsCaptain() const;//是否是队长
	enumTeamMemberType GetTeamMemberType() const;//得到队员类型
	void SetTeamMemberType(enumTeamMemberType type);//设置队员类型
	int GetAllTeamMemberInfo(KUiTeamMemberItem* teammateList);//得到队员信息
	bool IsAutoAcceptApply() const;//是否自动接受入队请求
	void SetAutoAcceptApply(bool isAuto);//设置是否自动接受入队请求

	void Create();//建立队伍
	void OpenBigTeamMode();//开启大队伍模式
	void Dismiss();//解散队伍
	void LevaveTeam();//离开队伍
	void InviteAdd(DWORD inviteNpcID);//邀请加入队伍
	void InviteAdd(const char* playerName);//邀请加入队伍
	void KickMember(DWORD kickNpcID);//踢出队伍
	void NewCaptain(DWORD newCaptainNpcID);//新的队长
	void PromoteAssistant(DWORD newAssistantNpcID);//把普通队员提升为助手
	void DismissAssistant(DWORD assistantNpcID);//把助手降至为普通队员
	void MoveIndex(int sourceIndex, int targetIndex);//改变排列次序
	void ApplyJoinTeam(DWORD targetNpcID);//申请加入对方的队伍
	void ApplyJoinTeam(const char* playerName);//申请加入对方的队伍

	void OperationResultCreate(TEAM_OPERATION_RESULT* pProtocol);//创建队伍操作结果
	void OperationResultOpenBigTeamMode(TEAM_OPERATION_RESULT* pProtocol);//开启大队伍模式操作结果
	void ProcessTeamOperation(TEAM_OPERATION_RESULT* pProtocol);
	void SendTeamOperation(enumTeamOperation operation, DWORD npcId = 0);//发送队伍操作请求
	void RequestTeamInfo(DWORD npcID);//请求某个npc的队伍信息
	void RequestSelfTeamInfo();//请求玩家自身的队伍信息
	void ReceiveApply(TEAM_APPLY_JOIN *pApply);//收到申请
	void ReplyApply(DWORD applyNpcID, enumApplyJoinTeamReplay result);//回复邀请

	void UpdateSelfTeam(TEAM_INFO* pMsg);
	void UpdateTeamMemberBasicInfo(UPDATE_TEAM_MEMBER_INFO* pProtocol);

public:
	bool m_IsInTeam;//是否已经组队
	enumTeamMemberType m_TeamMemberType;//自己的队员类型
	int m_nApplyCaptainID;//申请加入的目标队伍的队长的 npc id
	DWORD m_dwApplyTimer;//申请时间计数器（申请多久了，超过时间取消申请）
	BOOL m_bAutoRefuseInviteFlag;//是否自动拒绝别人的加入队伍的邀请 TRUE 自动拒绝   FALSE 手动
	KTeamApplyList m_sApplyList[MAX_TEAM_APPLY_LIST];//如果为队长，队伍的申请人列表
	bool m_AutoAcceptApply;//是否自动接受加入队伍的请求
};

inline bool KPlayerTeam::IsInTeam() const
{
	return m_IsInTeam;
}

inline void KPlayerTeam::SetInTeam(bool inTeam)
{
	m_IsInTeam = inTeam;
}

inline bool KPlayerTeam::IsCaptain() const
{
	return (m_TeamMemberType == teammember_type_captain);
}

inline void KPlayerTeam::SetTeamMemberType(enumTeamMemberType type)
{
	m_TeamMemberType = type;
}

inline enumTeamMemberType KPlayerTeam::GetTeamMemberType() const
{
	return m_TeamMemberType;
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

inline bool KPlayerTeam::IsAutoAcceptApply() const
{
	return m_AutoAcceptApply;
}

//队伍成员信息（客户端）
struct ClientTeamMemberInfo
{
	//基本信息（不易变）
	DWORD NpcId;//NpcId
	int Level;//等级
	int Class;//职业
	int Sex;//性别
	int Face;//头像
	int SkillSeries;//技能系	
	char Name[32];//名字
	enumTeamMemberType Type;//类型	

	//运行时信息（易变）
	DWORD Life;//生命
	DWORD LifeMax;//生命最大值
	BYTE LifePercent;//生命值百分比
	DWORD Mana;//法力
	DWORD ManaMax;//法力最大值
	BYTE ManaPercent;//法力值百分比
	int MapId;//所在地图编号
	DWORD PosX;//坐标X
	DWORD PosY;//坐标Y
	int BuffCount;//BUFF数量
	_BuffPair BuffPair[MAX_SYNC_TEAMATE_BUFF_COUNT];//BUFF

	void SetName(const char* szName) { strncpy(Name, szName, sizeof(Name)); }	
};

//客户端队伍
class KTeam
{
public:
	KTeam();
	void Release( void );//清空					
	void SetIndex(int nIndex);//设定 Team 在 g_Team 中的位置
	BOOL SetTeamOpen();//设定队伍状态：打开（允许接受新成员）
	BOOL SetTeamClose();//设定队伍状态：关闭（不允许接受新成员）
	BOOL SetExpShareType(int nShareExpType);//设定队伍状态：经验共享方式 lixuewu 2005.03.08
	int GetMaxMemberCount() const;
	int FindFree();//寻找队员空位
	int FindMemberID(DWORD dwNpcID);//寻找具有指定npc id的队员（不包括队长）	
	void CreateTeam(DWORD nCaptainNpcID, DWORD nTeamServerID);//客户端创建一支队伍
	int AddMember(DWORD dwNpcID, int nLevel, BYTE classAndSexInfo, BYTE face, char *lpszNpcName, enumTeamMemberType type = teammember_type_normal);//添加一个队伍成员
	void DeleteMember(DWORD dwNpcID);//客户端删除一个队伍成员
	int GetMemberCount() const;//得到队员数量（包括队长）
	void SetCaptain( DWORD nCaptain ) { m_nCaptain = nCaptain; }
	int GetTeamServerID( void ) const { return m_nTeamServerID; }
	DWORD GetCaptain( void ) { return m_nCaptain; }
	int GetShareExpFlag( void ) const { return m_nShareExpFlag; }
	int GetState( void ) { return m_nState; }
	bool IsBigTeam() const;//是否是大队伍模式
	void OpenBigTeamMode();//开启大队伍模式
	bool IsFull() const;//是否满员	
	void Rearrange();//重新排列（清除列表中的空槽位）
	void SetTeamServerID( int nTeamServerID ) { m_nTeamServerID = nTeamServerID; }
	ClientTeamMemberInfo* GetMemberInfo(int memberIndex);//得到队员信息
	void MoveIndex(int sourceIndex, int targetIndex);//改变排列次序
	void SetMemberInfo(const TEAMMATE_INFO& info);
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

private:
	int m_nIndex;//本Team在g_Team中的位置
	int m_nShareExpFlag;//表示组队经验分配方式 lixuewu 2005.03.07
	int m_nState;//队伍状态：Team_S_Open Team_S_Close
	DWORD m_nCaptain;//队长NpcId，0为空
	int m_nMemNum;//已有队员数量(不包括队长)
	DWORD m_nTeamServerID;//队伍在服务器上的唯一标识
	bool m_bIsBigTeam;//是否是大队伍
	enumTeamMemberType m_MemberType[MAX_BIG_TEAM_MEMBER];//成员的类型
	ClientTeamMemberInfo m_MemberInfo[MAX_BIG_TEAM_MEMBER];//成员信息
	std::bitset<team_setting_count> m_TeamSetting;//队伍设置
};

inline ClientTeamMemberInfo* KTeam::GetMemberInfo(int memberIndex)
{
	if (memberIndex >= 0 && memberIndex < GetMaxMemberCount())
		return &m_MemberInfo[memberIndex];
	else
		return NULL;
}

inline int KTeam::GetMemberCount() const
{
	return m_nMemNum + 1;
}

// inline void KTeam::SetMemberBuff( int nMemIdx, _BuffPair* pBuffPair, int buffCount )
// {
// 	if (pBuffPair != NULL && buffCount >= 0)
// 	{
// 		memset(m_BuffPair[nMemIdx], 0, MAX_SYNC_TEAMATE_BUFF_COUNT * sizeof(_BuffPair));
// 		memcpy(m_BuffPair[nMemIdx], pBuffPair, buffCount * sizeof(_BuffPair));
// 		m_nBuffCount[nMemIdx] = buffCount;
// 	}
// }

// inline int KTeam::GetMemberBuff( int nMemIdx, _BuffPair* pBuffPair, int offset, int buffCount ) const
// {
// 	if (pBuffPair != NULL && buffCount > 0)
// 	{
// 		int returnBuffCount = buffCount > m_nBuffCount[nMemIdx] ? m_nBuffCount[nMemIdx] : buffCount;
// 		memcpy(pBuffPair, m_BuffPair[nMemIdx], returnBuffCount * sizeof(_BuffPair));
// 		return returnBuffCount;
// 	}
// 
// 	return 0;
// }

inline bool KTeam::IsBigTeam() const
{
	return m_bIsBigTeam;
}

// inline enumTeamMemberType KTeam::GetMemberType(int nMemIdx) const
// {
// 	return m_MemberType[nMemIdx];
// }

inline bool KTeam::IsFull() const
{
	return (GetMemberCount() >= GetMaxMemberCount());
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

extern	KTeam	g_TeamC[MAX_TEAM];

inline KTeam& GetClientTeam()
{
	return g_TeamC[0];
}


//----------------------------------------------------------------------
//客户端队伍图标显示
//----------------------------------------------------------------------

#define INVALID_TEAM_ICON_INDEX -1

//队伍浏览信息
struct TeamViewInfo
{
	TeamViewInfo()
	{
		TeamId = 0;
		NpcCount = 0;
		IconIndex = INVALID_TEAM_ICON_INDEX;
	}

	int TeamId;
	int NpcCount;
	int IconIndex;
};

//队伍图标信息
struct TeamIconInfo
{
	TeamIconInfo()
	{
		memset(ImageSet, 0, sizeof(ImageSet));
		memset(Image, 0, sizeof(Image));
		InUse = false;
	}

	char ImageSet[128];
	char Image[128];
	bool InUse;
};

typedef std::vector<TeamViewInfo> TeamInfoArray;
typedef std::vector<TeamIconInfo> TeamIconArray;

#define TEAM_HEAD_ICON "TeamHeadIcon"	//队伍头顶图标名

//队伍查看
class TeamViewer
{
public:
	TeamViewer();
	~TeamViewer();

	void Init();
	void LoadTeamIcon(const char* settingFile);//载入队伍图标
	void AddNpc(int npcIndex);//新增NPC
	void RemoveNpc(int npcIndex);//删除NPC
	void TeamChanged(int npcIndex, int originalTeamId, int newTeamId);//队伍发生变化

private:
	int GetTeamInfoListIndex(int teamId) const;//得到队伍信息ListIndex
	TeamViewInfo* GetTeamViewInfo(int teamIndex);//得到队伍信息
	TeamIconInfo* GetTeamIconInfo(int iconIndex);//得到队伍图标
	int AddTeamInfo(int teamId);//添加队伍信息
	void RemoveTeamInfo(int listIndex);//删除队伍信息
	int GetFreeTeamIcon() const;//得到空闲的Icon
	void ReleaseTeamIcon(int iconIndex);//释放图标
	void ReleaseAllTeamIcon();//施放所有的图标
	bool ChangeTeamIcon(int teamId, int iconIndex);//改变队伍图标

	TeamInfoArray m_TeamInfoList;
	TeamIconArray m_TeamIconList;
};

inline TeamIconInfo* TeamViewer::GetTeamIconInfo(int iconIndex)
{
	if (iconIndex >= 0 && iconIndex < m_TeamIconList.size())
	{
		return &m_TeamIconList[iconIndex];
	}

	return NULL;
}

inline TeamViewInfo* TeamViewer::GetTeamViewInfo(int teamIndex)
{
	if (teamIndex >= 0 && teamIndex < m_TeamInfoList.size())
	{
		return &m_TeamInfoList[teamIndex];
	}

	return NULL;
}

extern TeamViewer g_TeamViewer;

#endif



