//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-5-8
//      File_base        : player_statistic
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 玩家信息统计
//
//////////////////////////////////////////////////////////////////////

#ifndef _PLAYER_STATISTIC_H_
#define _PLAYER_STATISTIC_H_

#include "ItemCommonDef.h"

#define MAX_SKILL_INFO 20
#define MAX_ITEM_COUNT_INFO 30

//技能使用信息
struct SkillUseInfo 
{
	DWORD Id;
	DWORD Count;
};

//物品计数类型
enum ItemCountType
{
	item_count_type_buy,		//购买
	item_count_type_sell,		//卖店
	item_count_type_use,		//使用
	item_count_type_pickup,		//拾取
	item_count_type_trade_out,	//交易（出）
	item_count_type_trade_in,	//交易（入）
	item_count_type_mail_out,	//邮件（出）
	item_count_type_mail_in,	//邮件（入）
	item_count_type_auction,	//拍卖
	item_count_type_destroy,	//销毁
	item_count_type_system_add,	//系统增加
	item_count_type_system_del,	//系统减少
	item_count_type_recommender_reward_ticket,	//推荐人奖励代金券

	item_count_type_buy_item_by_plus_point_0,
	item_count_type_buy_item_by_plus_point_1,
	item_count_type_buy_item_by_plus_point_2,
	item_count_type_buy_item_by_plus_point_3,
	item_count_type_buy_item_by_plus_point_4,
	item_count_type_buy_item_by_plus_point_5,
	item_count_type_buy_item_by_plus_point_6,
	item_count_type_buy_item_by_plus_point_7,
	item_count_type_buy_item_by_plus_point_8,
	item_count_type_buy_item_by_plus_point_9,
	item_count_type_buy_item_by_plus_point_10,
	item_count_type_buy_item_by_plus_point_11,
	item_count_type_buy_item_by_plus_point_12,
	item_count_type_buy_item_by_plus_point_13,
	item_count_type_buy_item_by_plus_point_14,
	item_count_type_buy_item_by_plus_point_15,
	item_count_type_buy_item_by_plus_point_16,
	item_count_type_buy_item_by_plus_point_17,
	item_count_type_buy_item_by_plus_point_18,
	item_count_type_buy_item_by_plus_point_19,
};

//物品计数信息
struct ItemCountInfo
{
	ItemTemplateId Id;
	DWORD Count;
	ItemCountType Type;
};

//玩家信息统计
class PlayerStatistic
{
public:
	PlayerStatistic();
	~PlayerStatistic();

	void Init(int playerIndex);//初始化
	void Active();//活动一次
	void Save();//保存并清空数据
	
	void ChangeExp(long expChange);//改变经验值
	void ChangeMoney(long moneyChange);//改变金钱
	void UseSkill(DWORD skillId);//使用技能
	void AddItem(ItemTemplateId& templateId, DWORD count, ItemCountType type);//增加物品计数
	void UseTaisui();//使用太岁

private:
	void Clear();//清空统计数据

	int m_PlayerIndex;
	unsigned long  m_LastSaveTimeout;
	long m_RecentAddExp;
	long m_RecentAddMoney;
	long m_RecentRemoveMoney;
	SkillUseInfo m_SkillUseInfoList[MAX_SKILL_INFO];
	ItemCountInfo m_ItemCountInfo[MAX_ITEM_COUNT_INFO];
	int m_RecentUseTaisuiCount;
};

inline void PlayerStatistic::ChangeExp(long expChange)
{
	m_RecentAddExp += expChange;
}

inline void PlayerStatistic::ChangeMoney(long moneyChange)
{
	if (moneyChange >= 0)
		m_RecentAddMoney += moneyChange;
	else
		m_RecentRemoveMoney += moneyChange;
}

inline void PlayerStatistic::UseTaisui()
{
	m_RecentUseTaisuiCount++;
}

#endif//_PLAYER_STATISTIC_H_