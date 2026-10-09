//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-6-12 10:22
//      File_base        : KProtocolDef
//      File_ext         : h
//      Author           : 
//      Description      : 
//////////////////////////////////////////////////////////////////////

#ifndef	KProtocolDefH
#define	KProtocolDefH

#define DIFF_FOR_EX_SERVER		10

#define KPROTOCOL_VERSION		34

/*
 * It was to judge a package type that 
 * it is a larger package or it is a small package
 */

enum s2c_PROTOCOL
{
	s2c_clientbegin = 64,
	s2c_login,
	s2c_rolelist,
	s2c_rolenewdelresponse,//新建与删除角色的结果返回,所带数据为结构tagNewDelRoleResponse	
	s2c_syncend,
	s2c_synccurplayer,//同步自己的独特信息（装备等）
	s2c_synccurplayerskill,//同步自己的技能
	s2c_synccurplayernormal,//平时给自己的同步数据（每个Player.Active都发送）
	s2c_syncworld,//同步自己所在的世界的基本信息（地图编号、区域编号、地图时间等）
	s2c_syncplayer,//同步玩家信息
	s2c_syncplayermin,//同步玩家最少信息（广播）
	s2c_syncnpc,//同步NPC的完整信息
	s2c_syncnpcmin,//同步NPC的最少信息（广播）
	s2c_syncnpcminplayer,//同步本玩家npc数据
	s2c_objadd,//添加Obj（广播）
	s2c_syncobjstate,//同步Obj状态（广播）
	s2c_syncobjdir,//同步Obj方向（广播）
	s2c_objremove,//删除Obj状态（广播）
	s2c_objTrapAct,//NotUsed
	s2c_npcremove,//删除NPC（广播）
	s2c_npcwalk,//NPC走路（广播）
	s2c_npcrun,//NPC跑步（广播）
	s2c_npchurt,//NPC受伤（广播）
	s2c_npcdeath,//NPC死亡（广播）
	s2c_skillcast,//NPC施放技能（广播）
	s2c_teamselfinfo,//发送自己的队伍信息
	s2c_teamleave,//离开队伍
	s2c_playerlevelup,//升级
	s2c_playerskillinfo,//同步技能信息
	s2c_syncitem,//同步物品
	s2c_removeitem,//删除物品
	s2c_syncmoney,//同步金钱
	s2c_playermoveitem,//移动物品
	s2c_scriptaction,//执行脚本动作
	s2c_tradechangestate,//交易状态改变
	s2c_trademoneysync,//同步交易金钱
	s2c_tradedecision,//同步交易确认
	s2c_teaminviteadd,//邀请加入队伍
	s2c_ping,//ping
	s2c_opensalebox,//打开买卖框
	s2c_openstorebox,//打开储物箱
	s2c_playerrevive,//玩家重生（广播）
	s2c_requestnpcfail,//请求NPC信息失败
	s2c_tradeapplystart,//发送交易请求	
	s2c_viewequip,//发送自己身上的装备给别人查看
	s2c_enchaseritemresult,//服务器返回宝石镶嵌结果	
	s2c_refreshitem,//刷新物品
	s2c_checkstoragepswok,//查看储物箱密码
	s2c_createstoragepswok,//创建储物箱密码
	s2c_modifystoragepswok,//修改储物箱密码
	s2c_sync_teammemberlife,//同步队伍中其他队员的生命
	s2c_sync_creature,//同步召唤兽状态
	s2c_byte_extend,//扩展字节Buff、图片问答等用
	s2c_pet,	//宠物协议族//先保留,以备后用
	s2c_findpathsync,// 同步查找路径
	s2c_show_damage,//显示伤害信息
	s2c_npcrealposition,//NPC实际位置（用于调试）
	s2c_chat_family,//聊天
	s2c_buff_family,//BUFF
	s2c_sync_skillseries,//同步技能流派
	s2c_sync_npcattr,//同步NPC属性
	s2c_sync_playerattr,//同步玩家属性
	s2c_sync_item_attr,//同步物品属性
	s2c_auction_family,//拍卖
	s2c_social_family,//社会关系（尚林）
	s2c_social_relation,//社会关系（小刚）
	s2c_delayed_action,//延迟动作
	s2c_talisman_family,//法宝操作
	s2c_sync_talisman_enchase,//同步法宝镶嵌
	s2c_sync_npc_equip_talisman,//同步NPC装备的法宝
	s2c_quest_family,
	s2c_team_invite_refuse,//拒绝组队邀请
	s2c_show_predefined_msg,//显示预制的消息
	s2c_IB_family,
	s2c_show_banner,//显示滚动标题消息
	s2c_find_family,
	s2c_taisui_wheel,  //太岁之轮协议族 Add by brianyao2007
	s2c_team_operation_result,//队伍操作通用协议
	s2c_update_team_member_info,//更新队伍成员基本信息
	s2c_prompt,//请求客户端选择
	s2c_cancel_prompt,//取消请求
    s2c_player_stop,//Player used
	s2c_pos_edition,//Npc used
	s2c_fury_sync,//fury system used
	s2c_apply_join_team,//申请加入队伍
	s2c_npc_sync_to_world,//NPC世界同步（每次）
	s2c_npc_sync_to_world_min,//NPC世界同步（初始）
	s2c_npc_sync_to_world_del,//NPC世界同步（删除）
	s2c_list_team,//队伍列表
	s2c_special_quest_data,//队伍列表
	s2c_show_banner_id,	//显示滚动消息（发送的是字符串和字体id）
	s2c_gm_feedback_msg,//GM反馈消息
	s2c_hire_data_list_exp,		//雇佣列表（exp）
	s2c_hire_data_list_fighter,	//雇佣列表（fighter）
	s2c_hire_ret_code,			//雇佣操作返回码
	s2c_world_combat_info,      //大战场信息同步
	s2c_npc_inlaycount,
	s2c_npc_comoflag,           //蒙面状态的同步
	s2c_world_combat_top10_info, //战场排名
	s2c_list_student,//列出自己的徒弟（推荐人系统）
	s2c_insurance,
	s2c_world_player_info_sync,
	s2c_war_commander_sync,
	s2c_world_custom_string,//地图自定义字符串
	s2c_change_title,//改变称号
	s2c_update_self_title,//更新自己的称号
	s2c_sync_self_title,//同步自己的所有称号
	s2c_select_title_result,//选择称号结果
	s2c_plus_point_top_n,
	s2c_shizu_popularity_top_n,//氏族人气排行
	s2c_zhuhou_popularity_top_n,//诸侯人气排行
	s2c_player_properties,//玩家属性页属性
	s2c_synccurplayernormalex,//为国战天神变身专用，请勿滥用，谢谢
	s2c_palyerinfo_sync,//PlayerRealInfo


	s2c_extend = 250,
	s2c_extendchat = 251,
	s2c_extendfriend = 252,

	s2c_end,
};

enum c2s_PROTOCOL
{
	c2s_gameserverbegin = 64,
	c2s_login,
	c2s_dbplayerselect,//选择角色
	c2s_syncend,
	c2s_newplayer,
	c2s_removeplayer,
	c2s_no_employ,	//不继续雇佣
	c2s_get_question,//获取问题
	c2s_requestnpc,
	c2s_requestobj,
	c2s_npcrun,//NPC跑动
	c2s_npcskill,//NPC使用技能
	c2s_teamapplyinfo,//查询队伍信息
	c2s_playereatitem,//使用物品
	c2s_playerpickupitem,//拾取物品
	c2s_playermoveitem,//移动物品
	c2s_playersellitem,//出售物品
	c2s_playerbuyitem,//购买物品
	c2s_playerthrowawayitem,//丢弃物品
	c2s_playerselui,//选择某个UIControl
	c2s_tradeapplystart,//交易请求
	c2s_trademovemoney,//交易转移金钱
	c2s_tradedecision,//交易锁定
	c2s_dialognpc,//对话NPC
	c2s_teaminviteadd,//邀请加入队伍
	c2s_teamreplyinvite,//回复组队邀请
	c2s_ping,//ping
	c2s_objmouseclick,//鼠标点击OBJ
	c2s_storemoney,//存钱
	c2s_playerrevive,//选择重生类型
	c2s_tradereplystart,//交易接受
	c2s_viewequip,//查看装备
	c2s_repairitem,//修理装备
	c2s_enchaseritem,//合成	
	c2s_splitpileitem,//拆分堆叠物品
	c2s_checkstoragepassword,//检查储物箱密码
	c2s_createstoragepassword,//创建储物箱密码
	c2s_modifystoragepassword,//修改储物箱密码
	c2s_closestorage,//关闭储物箱
	c2s_byte_extend,//扩展字节Buff、图片问答等用
	c2s_pet,//宠物的协议,是协议族,先保留,以备后用
	c2s_chat_family,//聊天
	c2s_buff_family,//BUFF
	c2s_chg_pkmode,//改变PK模式
	c2s_skill_sync,//同步技能
	c2s_auction_family,//拍卖
	c2s_playerstop,//停止
	c2s_social_family,//社会关系
	c2s_talisman_family,//法宝
	c2s_quest_family,
	c2s_player_logout,
	c2s_IB_family,
	c2s_find_family,
	c2s_taisui_wheel,   //太岁之轮协议族 Add by Brianyao2007
	c2s_team_operation,//队伍操作通用协议
	c2s_reply_prompt,//回答服务器请求
	c2s_player_pos_sync,    //Player used
	c2s_select_skill,//选中技能
	c2s_fury_explode,//fury system used
	c2s_request_sync_to_world_npc,//请求世界同步NPC
	c2s_list_team,//列出队伍
	c2s_req_special_quest_data,//特殊任务数据
	c2s_gm_communication,		//GM反馈
	c2s_hire_req_tobe_hired,	//申请被雇佣
	c2s_hire_req_list,			//申请查看雇佣列表
	c2s_hire_req_hire,			//申请雇佣某人
	c2s_interactive_script_input,	//交互脚本输入
	c2s_recommender,//推荐人操作
	c2s_insurance,
	c2s_select_title,//选择称号
	c2s_plus_point_top_n,
	c2s_player_real_info_sync,

	_c2s_begin_relay = 250,
	c2s_extend = _c2s_begin_relay,
	c2s_extendchat,
	c2s_extendfriend,
	_c2s_end_relay = c2s_extendfriend,

	c2s_end,
};

enum 
{
	buff_sync_add,
	buff_sync_info,
	buff_sync_del,
	buff_sync_npc,
	buff_sync_npc_add,
	buff_cop_cancel,
};

enum
{
	s2c_ex_protocol_sendquestion = 0,
	s2c_ex_protocol_askquestion,
	s2c_ex_protocol_answerquestion,
	s2c_ex_protocol_syncsecpw,
	s2c_ex_protocol_syncsn,
	s2c_ex_protocol_playerlawlessgwlogout,
	s2c_ex_protocol_nomove,
	s2c_ex_protocol_noskill,
	s2c_ex_protocol_nouseitem,
	s2c_ex_protocol_shortcut_add,
	s2c_ex_protocol_additemgroupcd,
	s2c_ex_protocol_delitemgroupcd,
	s2c_ex_protocol_changemap,
	s2c_ex_protocol_question,
	s2c_ex_protocol_exp_insurance,
	s2c_ex_protocol_quest_insurnace,
	s2c_ex_protocol_combat_result_org_2,
	s2c_ex_protocol_statue_info,
	s2c_ex_protocol_play_animation,
};

enum
{
	c2s_ex_protocol_sendquestion = 0,
	c2s_ex_protocol_askquestion,
	c2s_ex_protocol_answerquestion,
	c2s_ex_protocol_syncsecpw,
	c2s_ex_protocol_syncsn,
	c2s_ex_protocol_shortcut_add,
	c2s_ex_protocol_shortcut_del,
	c2s_ex_protocol_question,
	c2s_ex_protocol_gm,
	c2s_ex_protocol_refresh_shizu_popularity,
	c2s_ex_protocol_refresh_zhuhou_popularity,
	c2s_ex_protocol_refresh_self_properties,
	c2s_ex_protocol_refresh_player_properties,
};

#endif
