//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/16/2006 16:58
//      File_base        : TeamDef
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef TeamDef_h
#define TeamDef_h

#include	"KPlayerDef.h"
#include	"KLinkArray.h"

#define INVALID_TEAM_MEMBER_INDEX -1//非法的队员Index

//队伍操作
enum enumTeamOperation
{
	team_operation_create = 0,			//创建
	team_operation_open_big_team,		//开启大队伍
	team_operation_dismiss,				//解散
	team_operation_leave,				//离开
	team_operation_invite,				//邀请
	team_operation_kick,				//踢出
	team_operation_new_captain,			//移交
	team_operation_promote_assistant,	//提升
	team_operation_dismiss_assistant,	//降职
	team_operation_apply_join,			//申请加入
	team_operation_apply_join_accept,	//申请加入-同意
	team_operation_apply_join_refuse,	//申请加入-拒绝
	team_operation_set_auto_accept_apply_on,	//设置开启自动接受入队请求
	team_operation_set_auto_accept_apply_off,	//设置关闭自动接受入队请求
};

//组队邀请回复
enum enumInviteJointeamReplay
{
	invite_jointeam_replay_refuse = 0,
	invite_jointeam_replay_accept,
	invite_jointeam_replay_timeout,
};

//申请加入回复
enum enumApplyJoinTeamReplay
{
	apply_jointeam_replay_refuse = 0,
	apply_jointeam_replay_accept,
	apply_jointeam_replay_timeout,
};

//队伍设置
enum enumTeamSetting
{
	team_setting_can_kick =	0,
	team_setting_can_invite,
	team_setting_can_promote_assistant,
	team_setting_can_dismiss_assistant,
	team_setting_can_leave,
	team_setting_can_open_big_team,
	team_setting_can_dismiss,

	team_setting_count
};


#ifdef _SERVER
#define		MAX_TEAM			1000
#else
#define		MAX_TEAM			2
#endif

#endif 