//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-11
//      File_base        : ILogSystem
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 日志系统接口
//
//////////////////////////////////////////////////////////////////////

#ifndef _I_LOG_SYSTEM_H_
#define _I_LOG_SYSTEM_H_

//最大日志描述长度
#define MAX_LOG_COMMENT_LENGTH 128

//日志事件
typedef enum enumLogEvent
{
	log_event_invalid = -1,			//非法值
	log_event_npc_drop_item = 0,	//NPC掉落物品
	log_event_npc_drop_money,		//NPC掉落金钱
	log_event_pickup_item,			//拾取物品
	log_event_pickup_money,			//拾取金钱
	log_event_trade_item,			//交易物品
	log_event_add_exp,				//获得经验值
	log_event_add_money,			//获得金钱
	log_event_player_enter_world,	//玩家进入游戏世界
	log_event_player_leave_world,	//玩家离开游戏世界
	log_event_npc_save_op_load,		//NPC存盘数据载入
	log_event_npc_save_op_save,		//NPC存盘数据保存
	log_event_npc_save_op_delete,	//NPC存盘数据删除
	log_event_trade_money,			//交易金钱
	log_event_player_statistic_add_exp,			//玩家统计：获得经验值
	log_event_player_statistic_add_money,		//玩家统计：获得金钱
	log_event_player_statistic_remove_money,	//玩家统计：消耗金钱	
	log_event_npc_death,			//NPC死亡
	log_event_player_death,			//玩家死亡
	log_event_level_up,				//玩家升级
	log_event_quest_accept,			//接受任务
	log_event_quest_abort,			//放弃任务
	log_event_quest_complete,		//完成任务
	log_event_skill_level_up,		//技能升级（学习：升级到第一级）
	log_event_player_statistic_use_skill,		//使用技能
	log_event_destroy_item,			//销毁物品
	log_event_sell_item,			//出售物品
	log_event_repair_item,			//修理物品
	log_event_use_item,				//使用物品
	log_event_send_mail_item,		//邮件发送物品
	log_event_get_mail_item,		//邮件获得物品
	log_event_send_mail,			//发送邮件
	log_event_receive_mail,			//接受邮件
	log_event_buy_item,				//购买物品
	log_event_auction_item,			//拍卖物品
	log_event_system_add_item,		//系统为玩家添加物品
	log_event_system_del_item,		//系统从玩家删除物品
	log_event_system_drop_item,		//系统掉落物品
	log_event_player_statistic_buy_item,		//玩家统计：购买物品
	log_event_player_statistic_sell_item,		//玩家统计：出售物品
	log_event_player_statistic_use_item,		//玩家统计：使用物品
	log_event_player_statistic_pickup_item,		//玩家统计：拾取物品
	log_event_player_statistic_trade_out_item,	//玩家统计：交易（出）物品
	log_event_player_statistic_trade_in_item,	//玩家统计：交易（入）物品
	log_event_player_statistic_mail_out_item,	//玩家统计：邮寄（出）物品
	log_event_player_statistic_mail_in_item,	//玩家统计：邮寄（入）物品
	log_event_player_statistic_auction_item,	//玩家统计：拍卖物品
	log_event_player_statistic_destroy_item,	//玩家统计：销毁物品
	log_event_player_statistic_system_add_item,	//玩家统计：系统增加物品
	log_event_player_statistic_system_del_item,	//玩家统计：系统减少物品
	log_event_npc_statistic_kill_by_player,		//NPC统计：被玩家杀死
	log_event_npc_statistic_kill_player,		//NPC统计：杀死玩家
	log_event_npc_statistic_drop_item,			//NPC统计：掉落物品
	log_event_player_statistic_use_taisui,		//玩家统计：使用太岁
	log_event_social_unit_create,				//创建社会关系节点
	log_event_social_unit_delete,				//删除社会关系节点
	log_event_instance_create,					//创建副本	
	log_event_jinshanbi_dec,					//消耗金山币
	log_event_creditpoint_add,					//获得信用点
	log_event_creditpoint_dec,					//消耗信用点
	log_event_point_add,						//获得积分
	log_event_point_dec,						//消耗积分
	log_event_ticket_add,						//消耗代金券
	log_event_ticket_dec,						//消耗代金券
	log_event_player_compound_add_item,			//玩家统计：合成增加物品
	log_event_player_compound_del_item,			//玩家统计：合成减少物品
	log_event_player_compound_del_money,		//玩家统计：合成减少金钱
	log_event_player_smith_add_item,			//玩家统计：打造增加物品
	log_event_player_smith_del_item,			//玩家统计：打造减少物品
	log_event_player_smith_del_money,			//玩家统计：合成减少金钱
	
	log_event_script_add_money = 100,			//脚本获得金钱
	log_event_get_out_mail_money,				//从邮件取钱
	log_event_send_mail_failed_return_money,	//发送邮件失败退钱
	log_event_get_city_res_add_money,			//得到城市资源得到金钱
	log_event_script_add_box_money,				//脚本增加储物箱金钱
	log_event_item_compound_delete_money,		//合成消耗金钱
	log_event_item_smith_delete_money,			//打造消耗金钱
	log_event_pay_money,						//支付金钱
	log_event_script_pay_money,					//脚本支付金钱
	log_event_skill_update_pay_money,			//学习技能支付金钱
	log_event_get_out_mail_plus_money_pay_money,	//付款取信中金钱
	log_event_get_out_mail_plus_item_pay_money,		//付款取信中物品
	log_event_send_mail_pay_money,				//发送邮件支付金钱
	log_event_auction_pay_money,				//拍卖支付金钱
	log_event_auction_bid_pay_money,			//拍卖竞价支付金钱
	log_event_social_op_pay_money,				//社会关系操作支付金钱

	log_event_buff_add_exp,						//BUFF添加经验
	log_event_script_add_exp,					//脚本添加经验
	log_event_exp_manager_add_exp,				//经验管理器添加经验
	log_event_employ_employee_add_exp,			//佣兵获得经验

	log_event_combat_score_add_own,             //战场积分获取（自己）
	log_event_combat_score_add_share,           //战场积分获取（共享）
	log_event_combat_score_dec_script,          //战场积分减少(脚本)

	log_event_employ_employer_pay_money,		//雇主支付金钱
	log_event_employ_employer_pay_employ_time,	//雇主消耗雇用时间
	log_event_employ_employee_earn_money,		//佣兵获得金钱
	log_event_employ_employee_pay_employ_time,	//佣兵消耗雇用时间
	log_event_employ_post,						//上雇佣榜
	log_event_employ_try_employ,				//尝试雇用
	log_event_employ_employ_success,			//雇用成功
	log_event_employ_cancel,					//取消雇用
	log_event_employ_fire,						//取消雇用

	log_event_recommend_master_reward_add_money,		//推荐人获得奖励金钱
	log_event_recommend_try_add_student,		//尝试指定推荐人
	log_event_recommend_add_student_result,		//指定推荐人结果
	log_event_recommend_try_update_student,		//尝试被推荐人汇报情况
	log_event_recommend_update_student_result,	//被推荐人汇报情况结果
	log_event_recommend_try_get_master_reward,	//推荐人尝试获得奖励
	log_event_recommend_get_master_reward_result,		//推荐人获得奖励结果
	
	log_event_insruance_add_buy,                        //购买保险
	log_event_insurance_fetch_money,                    //取保险金回馈
	log_event_insurance_get_level_up,                   //升级回馈
	log_event_insurance_get_one_time_reward,            //一次性回馈

	log_event_recommend_master_reward_add_ticket,		//推荐人获得奖励代金券
	log_event_recommend_master_reward_add_item,			//推荐人获得奖励代金券
	log_event_recommend_reward_add_ticket,				//推荐人奖励代金券
	log_event_player_statistic_recommender_reward_ticket,	//玩家统计：推荐人奖励代金券

	log_event_jinshanbi_buy_wait,
	log_event_system_reward_add_item,                    //脚本添加绑定物品

	log_event_combat_score_add_script,                   //脚本添加战场积分
	log_event_failed_add_ibitem_to_itemlist,             //Paysys 返回的时候加到玩家身上失败

	log_event_gm_kick_player,					//GM操作：踢人
	log_event_gm_no_chat,						//GM操作：禁言
	log_event_gm_no_login,						//GM操作：禁止登陆
	log_event_gm_freeze_account,				//GM操作：冻结账号
	log_event_gm_transfer,						//GM操作：传送
	log_event_gm_view_ip,						//GM操作：查看IP

	log_event_exp_insurance_add_reward,         //经验保险奖励经验
	log_event_quest_insurance_add_reward,       //任务保险奖励经验

	log_event_player_buy_item_by_plus_point_0,
	log_event_player_buy_item_by_plus_point_1,
	log_event_player_buy_item_by_plus_point_2,
	log_event_player_buy_item_by_plus_point_3,
	log_event_player_buy_item_by_plus_point_4,
	log_event_player_buy_item_by_plus_point_5,
	log_event_player_buy_item_by_plus_point_6,
	log_event_player_buy_item_by_plus_point_7,
	log_event_player_buy_item_by_plus_point_8,
	log_event_player_buy_item_by_plus_point_9,
	log_event_player_buy_item_by_plus_point_10,
	log_event_player_buy_item_by_plus_point_11,
	log_event_player_buy_item_by_plus_point_12,
	log_event_player_buy_item_by_plus_point_13,
	log_event_player_buy_item_by_plus_point_14,
	log_event_player_buy_item_by_plus_point_15,
	log_event_player_buy_item_by_plus_point_16,
	log_event_player_buy_item_by_plus_point_17,
	log_event_player_buy_item_by_plus_point_18,
	log_event_player_buy_item_by_plus_point_19,

	log_event_player_statistic_buy_item_by_plus_point_0,
	log_event_player_statistic_buy_item_by_plus_point_1,
	log_event_player_statistic_buy_item_by_plus_point_2,
	log_event_player_statistic_buy_item_by_plus_point_3,
	log_event_player_statistic_buy_item_by_plus_point_4,
	log_event_player_statistic_buy_item_by_plus_point_5,
	log_event_player_statistic_buy_item_by_plus_point_6,
	log_event_player_statistic_buy_item_by_plus_point_7,
	log_event_player_statistic_buy_item_by_plus_point_8,
	log_event_player_statistic_buy_item_by_plus_point_9,
	log_event_player_statistic_buy_item_by_plus_point_10,
	log_event_player_statistic_buy_item_by_plus_point_11,
	log_event_player_statistic_buy_item_by_plus_point_12,
	log_event_player_statistic_buy_item_by_plus_point_13,
	log_event_player_statistic_buy_item_by_plus_point_14,
	log_event_player_statistic_buy_item_by_plus_point_15,
	log_event_player_statistic_buy_item_by_plus_point_16,
	log_event_player_statistic_buy_item_by_plus_point_17,
	log_event_player_statistic_buy_item_by_plus_point_18,
	log_event_player_statistic_buy_item_by_plus_point_19,

	log_event_repair_item_by_item,			//修理物品

	log_event_repair_item_by_instead_specie,	//代币修理物品
	log_event_get_item_exp,						//从经验物品中取出经验
	log_event_total_add_money_statistic,		//总计产出金钱
	log_event_total_del_money_statistic,		//总计消耗金钱

	log_event_count
} LogEvent;

//系统调试日志类型
typedef enum enumSysDbgLogEvent
{
	sys_dbg_log_event_invalid = -1,			//非法值
	sys_dbg_log_event_common = 0,			//通用
	sys_dbg_log_event_tong_war ,            //国战相关
	sys_dbg_log_event_pool_combat,          //分星池相关
	sys_dbg_log_event_npc_spawn_info,		//刷怪信息
	sys_dbg_log_event_recruit,              //社会招募log
	sys_dbg_log_event_wrong_npc,			//串npc
	sys_dbg_log_event_exe_guard_gm_cmd,		//执行GuardGM指令
	sys_dbg_log_event_exe_player_get_gm_cmd,	//执行Player数据库中的GM指令
	sys_dbg_log_event_invalid_equipment,	//非法的装备
	sys_dbg_log_event_spawn_failure,		//刷怪失败
	sys_dbg_log_event_balance_spawn,		//刷怪清理
	sys_dbg_log_event_item_ope,             //有问题的Item 相关操作
	sys_dbg_log_event_script_add_npc_failure,       //脚本添加NPC失败
	sys_dbg_log_event_prepay_employ_money_failure,  //雇佣预付款失败
	sys_dbg_log_event_employee_unexpected_damage,	//雇佣兵伤害异常
	sys_dbg_log_event_script_custom,		//脚本自定义日志

	sys_dbg_log_event_count
} SysDbgLogEvent;

//日志事件参数
typedef struct tagLogEventParam
{
	tagLogEventParam()
	{
		event = log_event_invalid;
		memset(&param1, 0, sizeof(FSGUID));		//参数一（GUID）
		memset(&param2, 0, sizeof(FSGUID));		//参数二（GUID）
		memset(&param3, 0, sizeof(FSGUID));		//参数三（GUID）
		param4 = 0;
		memset(comment, 0, sizeof(comment));
	}

	LogEvent event;		//事件
	FSGUID param1;		//参数一
	FSGUID param2;		//参数二
	FSGUID param3;		//参数三
	long param4;		//参数四
	char comment[MAX_LOG_COMMENT_LENGTH];	//描述
} LogEventParam, *PLogEventParam;

//日志系统接口
interface ILogSystem
{
	//记录日志
	virtual void Log(const LogEventParam& logEventParam) = 0;

	//系统调试日志
	virtual void SysDbgLog(const char* pLogData, unsigned int size, SysDbgLogEvent logEvent = sys_dbg_log_event_common) = 0;
};

//创建日志系统
extern int CreateLogSystem(ILogSystem* & pLogSystem);

//释放日志系统
extern void ReleaseLogSystem(ILogSystem* & pLogSystem);

#endif// _I_LOG_SYSTEM_H_