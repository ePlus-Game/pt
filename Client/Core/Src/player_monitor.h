//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007-08-28
//      File_base        : player_monitor
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 玩家监视器
//
//////////////////////////////////////////////////////////////////////

#ifndef _PLAYER_MONITOR_H_
#define _PLAYER_MONITOR_H_

#define MAX_SPY_LEVEL 5		//最大监控等级
#define PLAYER_ACTION_DESC_MAX_LENGTH 64		//玩家行为描述最大长度

//玩家行为
enum enumPlayerAction
{
	player_action_start = 0,

	player_action_login,				//进入游戏
	player_action_run,
	player_action_cast_skill,			//施放技能
	player_action_use_item,				//使用物品
	player_action_pickup_item,			//拾取物品
	player_action_move_item,
	player_action_sell_item,			//出售物品
	player_action_buy_item,				//购买物品
	player_action_destroy_item,			//销毁物品
	player_action_split_item,
	player_action_auction_item,			//拍卖物品（开始）
	player_action_trade_begin,			//开始交易
	player_action_trade_done,			//完成交易（成功）
	player_action_trade_cancel,			//取消交易
	player_action_trade_lock,			//锁定交易
	player_action_dialog_npc,			//与NPC对话
	player_action_use_taisui_wheel,		//使用太岁之轮
	player_action_logout,				//离开游戏
	player_action_pickup_money,			//拾取金钱
	player_action_death,				//死亡
	player_action_revive,				//复活
	player_action_drop_item,			//掉落物品
	player_action_enter_world,			//进入地图
	player_action_exit_world,			//离开地图

	player_buy_item_by_plus_point_0,
	player_buy_item_by_plus_point_1,
	player_buy_item_by_plus_point_2,
	player_buy_item_by_plus_point_3,
	player_buy_item_by_plus_point_4,
	player_buy_item_by_plus_point_5,
	player_buy_item_by_plus_point_6,
	player_buy_item_by_plus_point_7,
	player_buy_item_by_plus_point_8,
	player_buy_item_by_plus_point_9,
	player_buy_item_by_plus_point_10,
	player_buy_item_by_plus_point_11,
	player_buy_item_by_plus_point_12,
	player_buy_item_by_plus_point_13,
	player_buy_item_by_plus_point_14,
	player_buy_item_by_plus_point_15,
	player_buy_item_by_plus_point_16,
	player_buy_item_by_plus_point_17,
	player_buy_item_by_plus_point_18,
	player_buy_item_by_plus_point_19,
		
	player_action_end,
};

//记录角色行为参数
struct RecordPlayerActionParam
{
	RecordPlayerActionParam()
	{
		PlayerIndex = 0;
		Action = player_action_start;
		memset(Desc, 0, sizeof(Desc));
	}

	int PlayerIndex;
	enumPlayerAction Action;
	char Desc[PLAYER_ACTION_DESC_MAX_LENGTH];
};

//玩家监视器
class PlayerMonitor
{
public:
	PlayerMonitor();
	~PlayerMonitor();

	bool Load();//载入配置
	void RecordPlayerAction(const RecordPlayerActionParam& param);//记录玩家行为
	void RecordC2SProtocol(int playerIndex, BYTE protocol, BYTE* data, int size);//记录上行协议
	bool IsNeedRecord(int playerIndex, enumPlayerAction action) const;//是否需要记录
	bool IsNeedRecordC2SProtocol(int playerIndex) const;//是否需要记录上行协议

private:

	bool m_SpyAction[MAX_SPY_LEVEL + 1][player_action_end];
};

extern PlayerMonitor g_PlayerMonitor;

#endif// _PLAYER_MONITOR_H_