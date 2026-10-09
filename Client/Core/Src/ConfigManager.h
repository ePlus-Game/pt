//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-22
//      File_base        : ConfigManager
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 配置管理器
//
//////////////////////////////////////////////////////////////////////

#ifndef _CONFIG_MANAGER_H_
#define _CONFIG_MANAGER_H_

#include "buff_tab.h"

#define COMMON_STRING_LENGTH 512//普通字符串长度
#define ITEM_LEVELUP_TOOL_LIMIT_COUNT	10

//可配置颜色
enum ConfigurableColor
{
	color_pk_normal = 0,
	color_pk_alert,
	color_pk_punish,
	color_pk_demon,
	color_killer,
	color_pk_encourage,
	color_pk_diablo,
	color_count
};

//全局变量
enum enumGlobalVariable
{
	global_var_object_item_life_time,		//掉落物品生存时间
	global_var_object_item_belong_time,		//掉落物品归属时间
	global_var_max_share_exp_range,			//最远经验分享距离
	global_var_damage_action_delay,			//伤害动作延迟（秒）
	global_var_player_logout_time,			//玩家退出延时时间	
	global_var_playersave_min_interval,		//玩家存盘最小时间间隔（秒）
	global_var_playersave_max_interval,		//玩家存盘最大时间间隔（秒）
	global_var_playersave_interval_seconds,	//玩家存盘间隔（秒）
	global_var_playersave_setp_interval,	//玩家存盘步间隔（帧）
	global_var_playersave_setp_count,		//玩家存盘步长（个）
	global_var_tradesave_minmoney,			//当交易额度超过这个值时，将实时存盘玩家基本数据及装备数据
	global_var_antienthrall_wearinesstime,		//反沉迷疲劳时间，收益减半（秒）
	global_var_antienthrall_insalubritytime,	//反沉迷有害时间，0收益（秒）
	global_var_antienthrall_clearstatetime,		//离线时间达到这个值清除有害状态（秒）
	global_var_localroomchat_timeinterval,		//附近频道刷屏间隔
	global_var_globalroomchat_timeinterval,		//世界频道刷屏间隔
	global_var_maproomchat_timeinterval,		//地图频道刷屏间隔
	global_var_invite_jointeam_timeout,			//邀请加入队伍超时（秒）
	global_var_exp_percentage,				//打怪经验获取加成
	global_var_quest_exp_percentage,		//任务经验获取加成
	global_var_skill_exp_percentage,		//蕴魂获取加成
	global_var_item_bank_on,				//是否开启道具银行
	global_var_ibshop_timeinterval,			//IB商店请求时间间隔（秒）
	global_var_ibshop_shelf_timeinterval,	//IB商店页面请求时间间隔（秒）
	global_var_ibshop_panel_timeinterval,	//IB商店Panel请求时间间隔（秒）
	global_var_ibshop_style_timeinterval,	//IB商店描述Style请求时间间隔（秒）
	global_var_request_team_list_interval,	//请求队伍列表的时间间隔（秒）
	global_var_check_mail_interval,			//检测过期邮件的时间间隔（秒）
	global_var_combat_top10_interval,		//战场排名信息同步时间间隔
	global_var_combat_top10_switch,			//战场排名开关
	global_var_combat_top10_sort_interval,	//战场排名服务器排序时间间隔
	global_var_clear_chat_log_interval,		//清除聊天日志的时间间隔（秒）
	global_var_keep_chat_log_interval,		//保留聊天日志的时间间隔（秒）
	global_var_timezone_correct_hour,		//时区修正（小时）
	global_var_random_select_title_interval,	//随机选择称号间隔（秒）
	global_var_employee_unexpected_damage,		//雇佣兵异常伤害阈值
	globar_var_cancel_item_exp_buff,		//取消装备存储经验状态的BUFF编号

	/* AI相关 */
	global_var_ai_npc_follow_max_range,		//最远NPC跟随距离（在这个距离之外NPC将脱离跟随状态）
	global_var_ai_npc_follow_min_range,		//最近NPC跟随距离（在这个距离之内NPC将不再进一步靠近）
	global_var_next_ai_delay,				//下次AI的延迟（游戏帧）
	global_var_over_threat_factor,			//OT因子
	global_var_npc_return_time,				//NPC回归时间（游戏帧）	
	global_var_base_next_wander_change_time,	//基本下次漫步改变时间（游戏帧）
	global_var_max_next_wander_change_time,		//最大下次漫步改变时间（游戏帧）
	global_var_force_instant_return_delay,	//强制立即返回延迟（游戏帧）
	global_var_npc_wander_range,			//NPC漫步范围（像素）
	global_var_min_npc_follow_range,		//最小跟随范围（像素）
	global_var_valid_enemy_distance,		//有效敌人距离
	global_var_valid_clear_target_distance,		//跨域多少个region清除目标
	global_var_valid_auto_selectnpc_distance,	//自动选择目标范围
	global_var_ai_player_follow_attack_range_reduce,	//玩家跟随攻击距离减少

	/* BUFF相关 */
	global_var_buff_overweight,				//超重
	global_var_buff_level_up,				//升级
	global_var_buff_player_revive,			//玩家重生
	global_var_buff_player_death,			//玩家死亡
	global_var_buff_killer,					//杀手
	global_var_buff_npc_return,				//NPC返回
	global_var_buff_city_withdraw,			//城市取钱
	global_var_buff_city_contribute,		//城市存钱
	global_var_buff_city_produce,			//城市生产
	global_var_buff_city_consume,			//城市消耗
	global_var_buff_gm,						//GM
	global_var_buff_employ,					//雇佣兵上榜BUFF

	/* 技能相关 */
	global_var_skill_logic_version,			//技能逻辑版本
	global_var_commoncooldown_interval_1,   //六个技能系的全局冷却时间         
	global_var_commoncooldown_interval_2,
	global_var_commoncooldown_interval_3,
	global_var_commoncooldown_interval_4,
	global_var_commoncooldown_interval_5,
	global_var_commoncooldown_interval_6,	

	/* PK相关 */
	global_var_pkvalue_add_base,			//PK值增加基本值
	global_var_pkvalue_add_factor,			//PK值增加因子
	global_var_pkvalue_level_diff,			//PK惩罚等级差
	global_var_pkpunish_revive_map,			//PK惩罚重生地图
	global_var_pkpunish_revive_pos,			//PK惩罚重生位置
	global_var_default_revive_map,			//默认重生地图
	global_var_default_revive_pos,			//默认重生位置	
	global_var_death_drop_equip_normal,		//普通死亡掉装率（装备上的）
	global_var_death_drop_bag_normal,		//普通死亡掉装率（包裹中的）
	global_var_death_abrade_equip_normal,	//普通死亡装备物品损失耐久
	global_var_death_drop_equip_alert,		//警戒死亡掉装率（装备上的）
	global_var_death_drop_bag_alert,		//警戒死亡掉装率（包裹中的）
	global_var_death_abrade_equip_alert,	//警戒死亡装备物品损失耐久
	global_var_death_drop_equip_punish,		//惩罚死亡掉装率（装备上的）
	global_var_death_drop_bag_punish,		//惩罚死亡掉装率（包裹中的）
	global_var_death_abrade_equip_punish,	//惩罚死亡装备物品损失耐久
	global_var_pk_boundary_normal_to_alert,	//PK边界（普通-警戒）
	global_var_pk_boundary_alert_to_punish,	//PK边界（警戒-惩罚）
	global_var_pk_boundary_punish_to_demon,	//PK边界（惩罚-恶魔）
	global_var_pk_protect_level,			//PK保护等级
	global_var_pk_encourage_level,			//PK鼓励等级

	/* 日志相关 */
	global_var_log_device_db,				//DB日志设备
	global_var_log_device_debug,			//Debug日志设备
	global_var_log_device_textfile,			//文件日志设备
	global_var_log_item_log_level,			//物品日志等级
	global_var_log_npc_drop_money_amount,	//NPC掉落金钱数量警戒值
	global_var_log_pickup_money_amount,		//拾取金钱数量警戒值
	global_var_log_trade_money_amount,		//交易金钱数量警戒值
	global_var_log_add_exp_amount,			//获得经验数量警戒值
	global_var_log_add_money_amount,		//获得金钱数量警戒值
	global_var_log_player_enter_world,		//玩家进入游戏
	global_var_log_player_leave_world,		//玩家离开游戏
	global_var_log_load_npc,				//载入NPC
	global_var_log_save_npc,				//保存NPC
	global_var_log_delete_npc,				//删除NPC
	global_var_log_player_statistic,		//玩家统计信息
	global_var_log_player_statistic_auto_save_period,		//玩家统计信息自动存盘间隔
	global_var_log_npc_statistic,			//NPC统计信息
	global_var_log_npc_statistic_auto_save_period,			//NPC统计信息自动存盘间隔
	global_var_log_social_unit_create,		//创建社会关系节点
	global_var_log_social_unit_delete,		//删除社会关系节点
	global_var_log_instance_create,			//创建副本
	global_var_log_chat,					//记录频道聊天日志
	global_var_log_combat_score_add,        //战场积分增加警戒值
	global_var_log_combat_score_dec,        //战场积分减少警戒值
	global_var_log_employ_time_change_threshold,	//雇用时间改变阈值
	global_var_log_employ,					//记录雇用
	global_var_log_recommender,				//记录推荐人
	global_var_log_insurance_remain_money_add_amount,
	global_var_log_insurance_fetch_money,
	global_var_log_total_money_statistic_auto_save_interval,	//总计金钱统计自动存盘间隔

	/* 头顶显示相关 */
	global_var_role_head_normal_info_count,
	global_var_role_head_hover_info_count,

	/* NPC等级显示范围 */
	global_var_npc_level_displayer_value0,
	global_var_npc_level_displayer_value1,
	global_var_npc_level_displayer_value2,
	global_var_npc_level_displayer_value3,

	/* 等级伤害命中修正 */
	global_var_npc_level_different,
	global_var_npc_level_hit_value1,
	global_var_npc_level_hit_value2,
	global_var_npc_level_hurt_value1,
	global_var_npc_level_hurt_value2,

	/* 法宝相关 */
	global_var_talisman_potential_gain_rate,	//法宝蕴魂获取比率（与技能经验的比率）
	global_var_talisman_potential_convert_rate,	//法宝蕴魂转化比率（把人物蕴魂转化为法宝蕴魂的比率）

	/* 掉落相关 */
	global_var_drop_group_count,	//法宝蕴魂转化比率（把人物蕴魂转化为法宝蕴魂的比率）

	/*爆魂相关 */
	global_var_fury_check_interval, 
	global_var_fury_check_kill_num,
	global_var_fury_keep_time,
	global_var_fury_skill_id,
	global_var_fury_full_buff,
	global_var_fury_end_buff,
	
	/*国战相关*/
	global_var_tong_war_start_time, //国战开始时间
	global_var_tong_war_process_time,
	global_var_tong_war_boss_tempID,
	global_var_tong_war_boss_level,
	global_var_tong_war_boss_num,
	global_var_tong_war_death_percent,
    global_var_tong_war_scapegoat_buff,
	global_var_tong_war_rob_percentage,
	global_var_tong_war_protect_buff,
	global_var_tong_war_Invincibility_buff,
	global_var_tong_war_robber_tempID,
	global_var_tong_war_commander_sync_interval, //国战指挥员战场同步间隔
	global_var_tong_war_commander_sync_switch,   //国战指挥员战场同步开关
	global_var_tong_war_change_city_buff,
	global_var_tong_war_economy_sys_switch,
	global_var_tong_war_resource_switch,	//资源开关


	/*分星池相关*/
	global_var_pool_combat_item_class,
	global_var_pool_combat_item_detail,
	global_var_pool_combat_item_special,
	global_var_pool_combat_item_level,
	global_var_pool_combat_start_dec_t,
	global_var_pool_combat_end_dec_t,
	global_var_pool_combat_process_t,
	global_var_pool_combat_npc_id,
	global_var_pool_combat_npc_level,
	global_var_pool_combat_npc_offset_x,
	global_var_pool_combat_npc_offset_y,
	global_var_pool_combat_npc_alert_per,
	global_var_pool_combat_npc_against_time,
	global_var_pool_combat_protect_buff,
	global_var_pool_combat_success_buff,


	/* 道具镶嵌相关 */
	global_var_item_inlay_ok_normal,
    global_var_item_inlay_ok_yin,
	global_var_item_inlay_ok_yang,
	global_var_item_inlay_max_socket_count,

	/* 社会关系招募相关*/
	global_var_social_recruit_exist_time,
	global_var_social_recruit_suc_buff,
	
	/* 特殊Buff 相关 */
	global_var_special_buff,

	/* 邮件相关 */
	global_var_sendmail_text_tax,
	global_var_sendmail_item_tax,
	global_var_max_mails_per_player,

	/* 拍卖行相关 */
	global_var_auction_short_time,
	global_var_auction_middle_time,
	global_var_auction_long_time,
	global_var_auction_short_time_tax,
	global_var_auction_middle_time_tax,
	global_var_auction_long_time_tax,
	global_var_auction_bid_percent,
	global_var_auction_search_interval,

	/* 包裹 */
	global_var_item_box_init_size,

	/* 镶嵌特效 */
	global_var_inlay_item_npc_effect,
	global_var_inlay_item_npc_effect3,
	global_var_inlay_item_npc_effect6,
	global_var_inlay_item_npc_effect9,


	/*大战场相关*/
	global_var_war_score_share_rate,
	global_var_war_score_interval,

	/* 推荐人系统 */
	global_var_recommender_master_require_level_min,//推荐人需求最低等级
	global_var_recommender_master_require_level_max,//推荐人需求最高等级
	global_var_recommender_student_require_level_min,//被推荐人需求最低等级
	global_var_recommender_student_require_level_max,//被推荐人需求最高等级
	global_var_recommender_expire_day,//推荐人过期天数
	global_var_recommender_max_student_count,//最多徒弟数量
	global_var_recommender_reward_ticket_ratio,//积分兑换代金券的比例

	global_var_log_plus_point_amount,		//获得金钱数量警戒值

	global_var_item_lock_by_date_item_detail_type,		//获得金钱数量警戒值

	/* 人气值相关 */
	global_var_active_degree_factor1,				//活跃度因子1（a1）
	global_var_active_degree_factor2,				//活跃度因子1（b1）
	global_var_recalc_popularity_interval,			//重新计算人气值时间间隔（秒）
	global_var_update_popularity_interval,			//更新人气值间隔（秒）
	global_var_refresh_shizu_popularity_interval,	//刷新氏族人气排行间隔（秒）
	global_var_refresh_zhuhou_popularity_interval,	//刷新诸侯人气排行间隔（秒）

	/* 战场击杀排行 */
	global_var_refresh_combat_kill_rank_time_weekday,	//刷新战场击杀排行时间（周几）
	global_var_refresh_combat_kill_rank_time_hour,		//刷新战场击杀排行时间（小时）
	global_var_refresh_combat_kill_rank_time_minute,	//刷新战场击杀排行时间（分钟）
	global_var_refresh_combat_kill_rank_time_second,	//刷新战场击杀排行时间（秒）
	global_var_pk_boundary_demon_to_diablo,			//PK边界（恶魔-暗黑破坏神）

	global_var_death_drop_equip_demon,		//恶魔死亡掉装率（装备上的）
	global_var_death_drop_bag_demon,		//恶魔死亡掉装率（包裹中的）
	global_var_death_abrade_equip_demon,	//恶魔死亡装备物品损失耐久

	globar_var_instead_specie_index,	//代币索引
	globar_var_new_consume_point_index,		//新消费积分索引
	globar_var_new_consume_point_rate,		//新消费积分转换比率（百分比）

	global_var_count
};

enum enumIBGlobalVariable
{
	/* IB 相关 */
	ib_global_var_credit_level_limit,
	ib_global_var_creditpoint_default,
	ib_global_var_credit_date_default,
	ib_global_var_credit_day_default,
	ib_global_var_credit_rate_jinshanbi,
	ib_global_var_point_rate,
	ib_global_var_point_max,
	ib_global_var_jinshanbi_exchange_limit,
	ib_global_var_creditpoint_exchange_limit,
	ib_global_var_point_exchange_limit,
	ib_global_var_credit_to_ticket_rate,
	ib_global_var_count
};



#define	MAX_IB_TICKET_ID_LONG	16	// IB代金券字符长度

#ifdef _SERVER

#define MAX_ONLINE_BUFF 10//最多上线BUFF数量

#define MAX_CATEGORY_ID 10//最大类别值
#define MAX_SKILL_ADDITIONAL_BUFF 5//最多技能附加BUFF数量


//可配置的BUFF
enum ConfigurableBuff
{
	buff_Overweight = 0,	//超重
	buff_LevelUp,			//升级
	buff_PlayerRevive,		//玩家重生
	buff_PlayerDeath,		//玩家死亡
	buff_Killer,			//杀手
	buff_NpcReturn,			//NPC返回
	buff_CityWithdraw,		//城市取钱
	buff_CityContribute,	//城市存钱
	buff_CityProduce,		//城市生产
	buff_CityConsume,		//城市消耗
	buff_Count				//buff数量
};

#else

#define MAX_DISPLAY_STYLE_ADDITIONAL_PARAM 60//最大显示样式额外参数

enum ConfigurableDisplayStyle
{	
	style_new_line_obj,
	style_new_line_seg,
	style_item_name,	
	style_item_name_yao,
	style_item_name_name,
	style_item_name_yaolevel,	
	style_item_name_enchaselevel,
	style_item_image,
	style_item_desc,
	style_item_desc_basic,
	style_item_desc_plusinfo,
	style_item_charm,
	style_item_equip_pos,
	style_item_basic_property,
	style_item_basic_property_buff,
	style_item_yao_addon,
	style_item_yao_addon_buff,
	style_item_upgrade,
	style_item_addmagic,
	style_item_duration,
	style_item_weight,
	style_item_require_level,
	style_item_require_profession,
	style_item_require_property,
	style_item_price,
	style_item_credit_Property,

	style_armorset_name,
	style_armorset_part,
	style_armorset_part_individal,
	style_armorset_effect,	
	style_gua_name,
	style_gua_level,
	style_gua_desc,
	style_gua_active_item,
	style_gua_standalone,
	style_gua_mutiple,
	style_gua_mutiple_row,	
	style_gua_set_name,
	style_gua_set_comment,
	style_gua_set_effect,
	style_gua_set_desc,	
	style_gua_level_text,
	style_talisman_level,
	style_talisman_top_level,
	style_talisman_quality,
	style_talisman_potential,
	style_talisman_enchase,	
	style_talisman_enchase_name,
	style_talisman_enchase_desc,
	style_talisman_enchase_buff,
	style_talisman_enchase_unused,
	style_talisman_enchase_unenabled,
	style_talisman_enchase_require,
	style_talisman_enchase_require_group,
	style_enchase_item_level,
	style_money_image,
	style_money_text_color,
	style_money_font,
	style_money_text,
	style_equip_compare_text,
	style_equip_compare_font,
	style_equip_compare_color,
	style_tip_margin,
	style_item_valid_date_tip_des,
	style_item_out_date_text_tip_des,

	style_item_inlay_base,	
	style_item_inlay_graph_empty,	
	style_item_inlay_graph_full,	
	style_item_inlay_graph_full_special,	
	style_item_inlay_base_attribute,
	style_item_inlay_yao_attribute,
	style_item_inlay_special_attribute,
	style_item_inlay_stone_name,	

	style_item_inlay_stone_pos,
	style_item_inlay_stone_quality,
	style_item_inlay_stone_group,
	style_item_inlay_stone_rate,


	style_item_can_not_discard,
	style_item_can_not_sell,
	style_item_can_not_exchange,
	style_item_bind,
	style_item_can_not_death_drop,
	style_item_unique,
	style_item_equip_bind,

	/* 头顶显示相关 */
	style_role_head_info_begin,
	style_role_head_info_end,
	style_role_head_normal_info,
	style_role_head_hover_info,
	style_role_head_image_info,

	style_item_restrict_count,
	style_item_locked,

	/* SkillTip 相关 */
	style_skill_tip_head,
	style_skill_tip_name,
	style_skill_tip_level,
	style_skill_tip_dis,
	style_skill_tip_cos,
	style_skill_tip_cool,
	style_skill_tip_desc,
	style_skill_tip_line,
	style_skill_tip_havent_study,
	style_skill_tip_next_level,
	style_skill_tip_cost_nor_string,
	style_skill_tip_time_nor_string,
	style_skill_tip_money_nor_string,
	style_skill_tip_end,

	/*Skill Cond 相关*/
	style_skill_cond_head,
	style_skill_cond_name,
	style_skill_cond_skill_exp,
	style_skill_cond_money,
	style_skill_cond_item,
	style_skill_cond_end,

	/* 查询相关 */
	style_query_info_head,

	style_metier,
	style_series_js,
	style_series_ds,
	style_series_yr,

	/* 角色查询相关 */
	style_role_info_base,
	style_role_info_skill_title,
	style_role_info_skill,
	style_role_info_item_title,
	style_role_info_item,
	style_role_info_quest_title,
	style_role_info_quest,
	style_role_info_map_title,
	style_role_info_map,

	/* 物品查询相关 */
	style_item_format_info_title,
	style_item_format_info,

	/* 技能查询相关 */
	style_skill_format_info_title,
	style_skill_format_info,

	/* Npc查询相关 */
	style_quest_format_info_title,
	style_quest_format_info,

	style_questinfobegintitle,
	style_questrequest,
	style_questinfobeginnpc,
	style_questinfoneeditemtitle,
	style_questinfoneeditem,
	style_questinfoneednpctitle,
	style_questinfoneednpc,
	style_questinfodialognpctitle,
	style_questinfodialognpc,
	style_questinfoendnpctitle,
	style_questinfoendnpc,
	style_questinfoawardmoney,
	style_questinfoawardexp,
	style_questinfoawardchoiceitemtitle,
	style_questinfoawardchoiceitem,
	style_questinfoawarditemtitle,
	style_questinfoawarditem,

	style_npc_format_info_title,
	style_npc_format_info,

	style_npc_type,
	style_npc_info_image,
	style_npc_info_name,
	style_npc_info_base,
	style_npc_info_map_title,
	style_npc_info_map,
	style_npc_info_dropitem_title,
	style_npc_info_dropitem,

	style_map_quest_title,
	style_map_quest,

	style_query_info_end,

	style_sync_to_world_npc_image,

	/*小地图相关*/
	style_mini_map_info,
	
	/*特殊名字颜色*/
	style_special_color,
	style_world_combat_color,
	style_world_combat_minimap_image,

	style_item_map_pos,
	style_item_capability,

	/*聊天屏蔽字符*/
	style_textfilter_char,
	/*横幅默认Style*/
	style_top_message,

	/* npc 名字前缀 */
	style_npc_name_plus,

	/* 装备安全锁 */
	style_item_safe_lock,

	/* 战场大地图显示*/
	style_combat_map_org_image,

	/*国战指挥员显示*/
	style_war_commander_image,

	style_item_levelup_tool_desc,
	/*国战道具分期租用提示*/
	style_item_flush_time_desc,
	/* 积分商店积分购买限制 */
	style_pluspoint_limit_txt,

	style_count
};

#endif

#define MAX_PRIVATE_STATE_NUM       10
#define MAX_PK_ADDED_VALUE_NUM		35
#define MAX_PK_ARRAY_VALUE			4400

//配置管理器
class ConfigManager
{
public:

	ConfigManager();
	~ConfigManager();

	static ConfigManager& Singleton();

	bool Load();	

	//得到PK状态颜色
	unsigned int GetConfigurableColor(ConfigurableColor color);
	
	//得到全局变量
	int GetGlobalVariable(enumGlobalVariable varIndex);

	//设置全局变量
	void SetGlobalVariable(enumGlobalVariable varIndex, int newValue);

	//IB 相关
	void GetIBTicketId(int * Genera,int * Detail,int * Particular,int * Level);
	void GetIBReturnId(int * Genera,int * Detail,int * Particular,int * Level);
	int GetIBGlobalVariable(enumIBGlobalVariable varIndex);

	//蒙面
	const char * GetPlayerPrivateStateName(const int nIndex) const ;

	void GetItemLevelupToolLimit( int Idx, int& leftLimit, int& rightLimit);
#ifdef _SERVER
	bool LoadPKValueSettings();
	//得到在线BUFF的数量
	int GetOnlineBuffCount();

	//得到在线BUFF的编号
	int GetOnlineBuffID(int index);

	//得到技能附加BUFF
	int GetSkillAdditionalBuff(int categoryIndex, int categoryId, int buffIndex);

	//得到通用技能附加BUFF
	int GetCommonSkillAdditionalBuff(int buffIndex);

	int GetPKAddedValue(int currentPKValue);

#else

	//得到可配置的显示样式
	const char* GetConfigurableDisplayStyle(ConfigurableDisplayStyle style, int additionParam = 0);

#endif

private:

	void CleanUp();

	bool m_IsLoaded;

	int	m_itemLeftLevelupToolLimit[ITEM_LEVELUP_TOOL_LIMIT_COUNT];
	int m_itemRightLevelupToolLimit[ITEM_LEVELUP_TOOL_LIMIT_COUNT];

	unsigned int m_Color[color_count];

	int m_GlobalVariable[global_var_count];

	char	m_IBTicketId[MAX_IB_TICKET_ID_LONG];
	char	m_IBReturnId[MAX_IB_TICKET_ID_LONG];
	int		m_IBGlobalVariable[ib_global_var_count];

	char    m_PrivateStateName[MAX_PRIVATE_STATE_NUM][MAXSIZE_ROLENAME];
#ifdef _SERVER

	int m_OnlineBuffCount;

	int m_OnLineBuffID[MAX_ONLINE_BUFF];

	int m_SkillAdditionalBuffID[CATEGORY_COUNT][MAX_CATEGORY_ID][MAX_SKILL_ADDITIONAL_BUFF];

	int m_CommonSkillAdditionalBuffID[MAX_SKILL_ADDITIONAL_BUFF];
	unsigned short m_PKAddedValue[MAX_PK_ARRAY_VALUE];

#else

	char m_DisplayStyle[style_count][MAX_DISPLAY_STYLE_ADDITIONAL_PARAM][COMMON_STRING_LENGTH];

#endif

};

inline int ConfigManager::GetGlobalVariable(enumGlobalVariable varIndex)
{
	if (varIndex >= 0 && varIndex < global_var_count)
	{
		return m_GlobalVariable[varIndex];
	}
	else
	{
		_ASSERT(false);
		return 0;
	}
}

inline void ConfigManager::SetGlobalVariable(enumGlobalVariable varIndex, int newValue)
{
	if (varIndex >= 0 && varIndex < global_var_count)
	{
		m_GlobalVariable[varIndex] = newValue;
	}
	else
	{
		_ASSERT(false);
	}
}

inline void ConfigManager::GetIBTicketId(int * Genera,int * Detail,int * Particular,int * Level)
{
	sscanf(m_IBTicketId, "%d|%d|%d|%d", Genera, Detail, Particular, Level);
}

inline void ConfigManager::GetIBReturnId(int * Genera,int * Detail,int * Particular,int * Level)
{
	sscanf(m_IBReturnId, "%d|%d|%d|%d", Genera, Detail, Particular, Level);
}

inline int ConfigManager::GetIBGlobalVariable(enumIBGlobalVariable varIndex)
{
	if (varIndex >= 0 && varIndex < global_var_count)
	{
		return m_IBGlobalVariable[varIndex];
	}
	else
	{
		_ASSERT(false);
		return 0;
	}
}

#ifdef _SERVER

inline int ConfigManager::GetOnlineBuffCount()
{
	return m_OnlineBuffCount;
}

inline int ConfigManager::GetSkillAdditionalBuff(int categoryIndex, int categoryId, int buffIndex)
{
	if (categoryIndex >= 0 && categoryIndex < CATEGORY_COUNT && categoryId >= 0 && categoryId < MAX_CATEGORY_ID && buffIndex >= 0 && buffIndex < MAX_SKILL_ADDITIONAL_BUFF)
		return m_SkillAdditionalBuffID[categoryIndex][categoryId][buffIndex];
	else
		return 0;
}

inline int ConfigManager::GetCommonSkillAdditionalBuff(int buffIndex)
{
	if (buffIndex >= 0 && buffIndex < MAX_SKILL_ADDITIONAL_BUFF)
		return m_CommonSkillAdditionalBuffID[buffIndex];
	else
		return 0;
}

inline int ConfigManager::GetPKAddedValue(int currentPKValue)
{
	int ret = 0;
	if (currentPKValue >= 0 && currentPKValue < MAX_PK_ARRAY_VALUE)
	{
		ret = m_PKAddedValue[currentPKValue];
	}
	return ret;
}

#else

//TODO 在这里放置客户端专有inline函数

#endif

#endif//_CONFIG_MANAGER_H_