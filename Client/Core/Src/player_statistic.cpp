#include "KCore.h"
#include "ConfigManager.h"
#include "KPlayer.h"
#include "player_statistic.h"

PlayerStatistic::PlayerStatistic()
{	
}

PlayerStatistic::~PlayerStatistic()
{
}

void PlayerStatistic::Init(int playerIndex)
{
	m_PlayerIndex = playerIndex;
	m_LastSaveTimeout = ConfigManager::Singleton().GetGlobalVariable(global_var_log_player_statistic_auto_save_period);	
	Clear();
}

void PlayerStatistic::Clear()
{
	m_RecentAddExp = 0;
	m_RecentAddMoney = 0;
	m_RecentRemoveMoney = 0;
	memset(m_SkillUseInfoList, 0, sizeof(m_SkillUseInfoList));
	memset(m_ItemCountInfo, 0, sizeof(m_ItemCountInfo));
	m_RecentUseTaisuiCount = 0;
}

void PlayerStatistic::Active()
{
	m_LastSaveTimeout--;
	if (m_LastSaveTimeout <= 0)
		Save();
}

void PlayerStatistic::Save()
{
	ConfigManager& cm = ConfigManager::Singleton();

	m_LastSaveTimeout = cm.GetGlobalVariable(global_var_log_player_statistic_auto_save_period);
	if (m_LastSaveTimeout <= 0)
		m_LastSaveTimeout = 3600 * GAME_FPS;
	
	KPlayer& player = Player[m_PlayerIndex];

	if (TRUE == cm.GetGlobalVariable(global_var_log_player_statistic))
	{
		//记录经验
		if (m_RecentAddExp != 0)
		{		
			LogEventParam playerStatisticExpEvent;
			playerStatisticExpEvent.event = log_event_player_statistic_add_exp;
			playerStatisticExpEvent.param1 = player.GetGUID();
			playerStatisticExpEvent.param4 = m_RecentAddExp;
			g_pLogSystem->Log(playerStatisticExpEvent);
		}
		
		//记录获得金钱
		if (m_RecentAddMoney != 0)
		{
			LogEventParam playerStatisticMoneyEvent;
			playerStatisticMoneyEvent.event = log_event_player_statistic_add_money;
			playerStatisticMoneyEvent.param1 = player.GetGUID();
			playerStatisticMoneyEvent.param4 = m_RecentAddMoney;
			g_pLogSystem->Log(playerStatisticMoneyEvent);
		}

		//记录消耗金钱
		if (m_RecentRemoveMoney != 0)
		{
			LogEventParam playerStatisticMoneyEvent;
			playerStatisticMoneyEvent.event = log_event_player_statistic_remove_money;
			playerStatisticMoneyEvent.param1 = player.GetGUID();
			playerStatisticMoneyEvent.param4 = m_RecentRemoveMoney;
			g_pLogSystem->Log(playerStatisticMoneyEvent);
		}

		//记录使用技能
		for (int skillIndex = 0; skillIndex < MAX_SKILL_INFO; skillIndex++)
		{
			SkillUseInfo& info = m_SkillUseInfoList[skillIndex];
			if (info.Id > 0)
			{
				LogEventParam playerStatisticSkillEvent;
				playerStatisticSkillEvent.event = log_event_player_statistic_use_skill;
				playerStatisticSkillEvent.param1 = player.GetGUID();
				snprintf(playerStatisticSkillEvent.param2.data, sizeof(playerStatisticSkillEvent.param2.data), "%d", info.Id);
				playerStatisticSkillEvent.param4 = info.Count;
				g_pLogSystem->Log(playerStatisticSkillEvent);
			}
			else
			{
				break;
			}
		}

		//记录物品
		for (int itemIndex = 0; itemIndex < MAX_ITEM_COUNT_INFO; itemIndex++)
		{
			ItemCountInfo& info = m_ItemCountInfo[itemIndex];
			if (info.Count > 0)
			{
				char itemTemplateIdString[32] = { 0 };
				sprintf(itemTemplateIdString, "%d,%d,%d,%d", info.Id.IDArray[0], info.Id.IDArray[1], info.Id.IDArray[2], info.Id.IDArray[3]);

				LogEventParam playerStatisticItemEvent;
				playerStatisticItemEvent.param1 = player.GetGUID();				
				strcat((char*)&(playerStatisticItemEvent.param2), itemTemplateIdString);
				playerStatisticItemEvent.param4 = info.Count;

				switch(info.Type)
				{
				case item_count_type_buy:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_buy_item;						
					}
					break;
				case item_count_type_sell:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_sell_item;
					}
					break;
				case item_count_type_use:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_use_item;
					}
					break;
				case item_count_type_pickup:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_pickup_item;
					}
					break;
				case item_count_type_trade_out:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_trade_out_item;
					}
					break;
				case item_count_type_trade_in:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_trade_in_item;
					}
					break;
				case item_count_type_mail_out:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_mail_out_item;
					}
					break;
				case item_count_type_mail_in:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_mail_in_item;
					}
					break;
				case item_count_type_auction:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_auction_item;
					}
					break;
				case item_count_type_destroy:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_destroy_item;
					}
					break;
				case item_count_type_system_add:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_system_add_item;
					}
					break;
				case item_count_type_system_del:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_system_del_item;
					}
					break;
				case item_count_type_recommender_reward_ticket:
					{
						playerStatisticItemEvent.event = log_event_player_statistic_recommender_reward_ticket;
					}
					break;
				case item_count_type_buy_item_by_plus_point_0:
				case item_count_type_buy_item_by_plus_point_1:
				case item_count_type_buy_item_by_plus_point_2:
				case item_count_type_buy_item_by_plus_point_3:
				case item_count_type_buy_item_by_plus_point_4:
				case item_count_type_buy_item_by_plus_point_5:
				case item_count_type_buy_item_by_plus_point_6:
				case item_count_type_buy_item_by_plus_point_7:
				case item_count_type_buy_item_by_plus_point_8:
				case item_count_type_buy_item_by_plus_point_9:
				case item_count_type_buy_item_by_plus_point_10:
				case item_count_type_buy_item_by_plus_point_11:
				case item_count_type_buy_item_by_plus_point_12:
				case item_count_type_buy_item_by_plus_point_13:
				case item_count_type_buy_item_by_plus_point_14:
				case item_count_type_buy_item_by_plus_point_15:
				case item_count_type_buy_item_by_plus_point_16:
				case item_count_type_buy_item_by_plus_point_17:
				case item_count_type_buy_item_by_plus_point_18:
				case item_count_type_buy_item_by_plus_point_19:
					{
						int plus = info.Type - item_count_type_buy_item_by_plus_point_0;
						playerStatisticItemEvent.event = (enumLogEvent)(log_event_player_statistic_buy_item_by_plus_point_0 + plus);
					}
					break;
				default:
					break;
				}				
				g_pLogSystem->Log(playerStatisticItemEvent);
			}
			else
			{
				break;
			}
		}

		//记录使用太岁
		if (m_RecentUseTaisuiCount > 0)
		{
			LogEventParam playerStatisticTaisuiEvent;
			playerStatisticTaisuiEvent.event = log_event_player_statistic_use_taisui;
			playerStatisticTaisuiEvent.param1 = player.GetGUID();
			playerStatisticTaisuiEvent.param4 = m_RecentUseTaisuiCount;
			g_pLogSystem->Log(playerStatisticTaisuiEvent);
		}
	}

	Clear();
}

void PlayerStatistic::UseSkill(DWORD skillId)
{
	if (FALSE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_player_statistic))
		return;

	for (int i = 0; i < MAX_SKILL_INFO; i++)
	{
		SkillUseInfo& info = m_SkillUseInfoList[i];
		if (info.Id == 0)
		{
			info.Id = skillId;
			info.Count = 1;
			break;
		}
		else if (info.Id == skillId)
		{
			info.Count++;
			break;
		}
	}
}

void PlayerStatistic::AddItem(ItemTemplateId& templateId, DWORD count, ItemCountType type)
{
	if (count <= 0)
		return;

	if (FALSE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_player_statistic))
		return;

	for (int i = 0; i < MAX_ITEM_COUNT_INFO; i++)
	{
		ItemCountInfo& info = m_ItemCountInfo[i];
		if (info.Count == 0)
		{
			info.Id = templateId;
			info.Count = count;
			info.Type = type;
			break;
		}
		else if (info.Type == type && info.Id == templateId)
		{
			info.Count += count;
			break;
		}
	}
}
