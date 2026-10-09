//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 10/19/2007 13:08
//      File_base        : CoreUseNameDef
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef COREUSENAMEDEF_H
#define COREUSENAMEDEF_H

/************************************************************************/
/*								常量定义                                */
/************************************************************************/
#define		DEFAULT_GAMESERVER_PORT			8888
#define		MAX_PLAYER_IN_ACCOUNT			20
#define		MAX_CREATE_PLAYER_IN_ACCOUNT	2
#define		COMMON_CLIENT_MSG_LEN_8			8
#define		COMMON_CLIENT_MSG_LEN_16		16
#define		COMMON_CLIENT_MSG_LEN_32		32
#define		COMMON_CLIENT_MSG_LEN_64		64
#define		COMMON_CLIENT_MSG_LEN_80		80
#define		COMMON_CLIENT_MSG_LEN_128		128
#define		COMMON_CLIENT_MSG_LEN_256		256
#define		COMMON_CLIENT_MSG_LEN_512		512
#define		COMMON_CLIENT_MSG_LEN_1024		1024
#define		ERROR_CODE_DEFAULT				-1
#define		GAME_MAX_FPS					30
#define		ClientVersion					2.0
#define		DEFAULT_SCREEN_HEIGHT			600
#define		DEFAULT_SCREEN_WIDTH			800
#define		DEFAULT_MOUSEHOVER_TIME			400
#define		BYTE_IP_ADDRESS_LEN				4
#define		MAX_CHANNEL_CONST				5
#define		GAMEOBJECT_HEIGHT				32
#define		GAMEOBJECT_WIDTH_MID			30
#define		GAMEOBJECT_WIDTH				22
#define		GAMEOBJECT_WIDTH_MIN			15

//---------------------------- 鼠标指针相关 ------------------------------
#define		MOUSE_CURSOR_NORMAL				0//普通情况
#define		MOUSE_CURSOR_FIGHT				1//战斗情况
#define		MOUSE_CURSOR_DIALOG				2//对话情况
#define		MOUSE_CURSOR_REPAIR				3//普通修理情况
#define		MOUSE_CURSOR_REPAIR_PLUS		4//特殊修理情况
#define		MOUSE_CURSOR_PICK				5//拾取物品、OBJ
#define		MOUSE_CURSOR_LOOK_OBJ			6//查看OBJ情况
#define		MOUSE_CURSOR_CHAT				7//私聊功能
#define		MOUSE_CURSOR_VIEW				8//查看功能
#define		MOUSE_CURSOR_FOLLOW				9//跟随功能
#define		MOUSE_CURSOR_FRIEND				10//好友功能
#define		MOUSE_CURSOR_SCREEN				11//黑名单功能
#define		MOUSE_CURSOR_TRADE				12//交易功能
#define		MOUSE_CURSOR_BOOK				13//卷轴功能
#define		MOUSE_CURSOR_TEAM				14//组队功能
#define		MOUSE_CURSOR_PICKCONFIRM		15//点击拾取
#define     MOUSE_CURSOR_ADDFRIEND			16//加好友功能
#define		MOUSE_CURSOR_EDIT				17//编辑状态
#define		MOUSE_CURSOR_TARGETITEM			18//使用目标道具
#define		MOUSE_SUPER_LINK_PLAYER			19
#define		MOUSE_SUPER_LINK_ITEM			20//超级链接
#define		MOUSE_SUPER_LINK_FACE			21
#define		MOUSE_SUPER_LINK_CHANNEL		22
#define		MOUSE_SUPER_LINK_POSITION		23//超级链接
#define		MOUSE_SUPER_LINK_PORTRAIT		24
#define		MOUSE_SUPER_LINK_MAP			25
#define		MOUSE_SUPER_LINK_SKILL			26
#define		MOUSE_SUPER_LINK_TASK			27
#define		MOUSE_SUPER_LINK_NPC			28
#define		MOUSE_CURSOR_USE				29
#define		MOUSE_CURSOR_ITEM_LOCK_BY_DATA	30

/************************************************************************/
/*								字符串                                  */
/************************************************************************/
#define		DUMP_DESC_0								"点击发送按钮发送此错误信息，我们会尽快处理！"
#define		DUMP_DESC_1								"正在发送错误信息 ... ..."
#define		DUMP_DESC_2								"错误信息发送完毕！"
#define		YIN										"阴"
#define		YANG									"阳"
#define		GLOBAL_ROOM_NAME						"交易"
#define		DEF_NONEGROUP_NAME						"好友"
#define		DEF_TEMPGROUP_NAME						"临时好友"
#define		DEF_BLACKGROUP_NAME						"黑名单"
#define		DEF_ENEMYGROUP_NAME						"仇人"
//太岁之论
#define		GIFT_D_AND_MONTH		                "在太岁之轮中获得日月大奖!"
#define		GIFT_JIAZI_EVENT	                    "激活了甲子事件"
//<------EXVERSION
#ifndef _EX_SERVER_MODE_
	#define		APPLICATION_NAME					"FSOnline2"
#else
	#define		APPLICATION_NAME					"FSOnline2Ex"	
#endif
#define		FS2ONLINE_GUID							"FSONLINE2{5E629DD4-126A-4578-AFE4-717B39B9DFFF}"
#define		QUIT_QUESTION_ID						"22"
#define		GAME_TITLE								"23"
#define		REPRESENT_MODULE_2						"Graphic.dll"
#define		REPRESENT_MODULE_3						"Represent3.dll"
#define		CREATE_REPRESENT_SHELL_FUN				"CreateRepresentShell"
#define		ENDURE									"耐久："
#define		WEIGHT									"重量："
#define		EQUIP_REQ_LEVEL							"需求等级："
#define		EQUIP_REQ_PRO							"需求职业："
#define		EQUIP_REQ_WA							"需求物功："
#define		EQUIP_REQ_MA							"需求魔功："
#define		EQUIP_REQ_LING							"需求灵："
#define		EQUIP_REQ_LI							"需求力："
#define		EQUIP_REQ_TI							"需求体："
#define		EQUIP_REQ_SHU							"需求术："
#define		XUANFENG_J								"玄风甲"
#define		XINGTIAN_J								"刑天甲"
#define		TIANSHI_D								"天师"
#define		ZHENREN_D								"真人"
#define		YISHI_S									"羿使"
#define		SHOUSHI_S								"兽使"
#define		SHIZU_NAME								"氏族名称"
#define		ZHUHOU_NAME								"诸侯名称"
#define		SHIZU_CREATE							"建立氏族"
#define		ZHUHOU_CREATE							"建立诸侯"
#define		CHENGSHI_CREATE							"建立城市"
#define		CHENGSHI_NAME							"城市名称"
#define		ACCEPTED_QUEST							"已接任务"

#define		LEAGUE_CREATE							"建立联盟"
#define		LEAGUE_NAME								"联盟名称"

#define		TALISMAN_SKILL							"法宝技能："
#define		TALISMAN_REQ_ENCHASE					"需求镶嵌："
#define		TALISMAN_REQ_ENCHASE_GROUP				"需求镶嵌组："
#define		MAKESURE								"确认使用此物品？"
#define		SHADOW									"影子"
#define		SHADOW_INFO								"影子信息"
#define		HEAD									"头顶"
#define		FOOT									"脚底"
#define		CYC										"循环"
//---------------------------- 程序错误提示相关---------------------------
#define		MSG_16BIT_ADVISE						"是否将颜色设为16位以提高游戏的流畅度？"
#define		ERROR_MSGBOX_TITLE						"封神榜2·网络版"
#define		APPLICATION_ERROR						"未知的错误!"
#define		APPLICATION_ERROR_0						"未能找到游戏运行必需的文件%s!"		
#define		APPLICATION_ERROR_1						"无法加载模块%s，可能是却少必需的文件或者该模块文件的版本不对!"
#define		APPLICATION_ERROR_2						"模块%s无法正常执行，可能是该模块文件的版本不对!"
#define		APPLICATION_ERROR_3						"运行模块%s时发生错误，游戏无法继续，请联系开发公司!"
#define		APPLICATION_ERROR_4						"无法正常启动游戏！请确认已把桌面的颜色设置为增强色16位!"
#define		APPLICATION_ERROR_5						"无法正常启动游戏！请检查是否安装了DirectX7!"
#define		APPLICATION_ERROR_6						"请用AutoRun.exe进入游戏。"

//---------------------------- 聊天相关 ----------------------------------
#define		CHAT_CHANNEL_NAME_SYSTEM				"系统"
#define		CHAT_CHANNEL_NAME_GLOBAL				"世界"
#define		CHAT_CHANNEL_NAME_MAP					"本地"
#define		CHAT_CHANNEL_NAME_LOCAL					"附近"
#define		CHAT_CHANNEL_NAME_TEAM					"队伍"
#define		CHAT_SENDER_SYSTEMNAME					"提示"

#define		ROLE_CAREER_JS							"甲士"
#define		ROLE_CAREER_JS_0						"刑天"
#define		ROLE_CAREER_JS_1						"玄风"
#define 	JIASHI_1								"男甲士"
#define 	SHUSHI_1								"男术士"
#define 	YIREN_1									"男异人"
#define 	JIASHI_0								"女甲士"
#define 	SHUSHI_0								"女术士"
#define 	YIREN_0									"女异人"
#define		ROLE_CAREER_YR							"异人"
#define		ROLE_CAREER_YR_0						"兽使"
#define		ROLE_CAREER_YR_1						"羿使"
#define		ROLE_CAREER_DS							"道士"
#define		ROLE_CAREER_DS_0						"天师"
#define		ROLE_CAREER_DS_1						"真人"
#define		PET_STRING								"召唤兽"
#define		TASK_STRING0							"与%s谈一谈"
#define		TASK_STRING1							"与%s谈一谈(完成)"
#define		TASK_STRING2							"已杀死%s(%d/%d)"
#define		TASK_STRING3							"收集%s(%d/%d)"
#define		TONG									"铜"
#define		MONEY									"银两"
#define		SELF_YOUR								"你"
#define		REFUSE_ADD_TEAM							"<Seg float=wrap><Obj type=text vertical-align=bottom %s>很抱歉，您的邀请被%s拒绝</Obj></Seg>"
#define		NEW_TEAMLEADER							"<Seg float=wrap><Obj type=text vertical-align=bottom %s>%s是新的队长</Obj></Seg>"
#define		TRAP									"陷阱"
#define		SKILLEXP_SKILL							"您获得了一个蕴魂技能!"
#define		ROLE_KIND								"人物名称"
#define		SPR_INFO_NAME							"信息"
#define		SPR_PALINFO_NAME						"配色"
#define		KIND_NAME_SECT							"人物类型"
#define		KIND_NAME_SPECIAL						"特殊npc"
#define		KIND_NAME_NORMAL						"普通npc"
#define		KIND_FILE_SECT1							"部件说明文件名"
#define		KIND_FILE_SECT2							"武器行为关联表1"
#define		KIND_FILE_SECT3							"武器行为关联表2"
#define		KIND_FILE_SECT4							"动作贴图顺序表"
#define		KIND_FILE_SECT5							"资源文件路经"
#define		WEAPON_PARTICULARTYPE					"详细类别"
#define		WEAPON_DETAILTYPE						"具体类别"
#define		WEAPON_SKILLID							"对应物理技能编号"
#define		HOUR									"小时"
#define		MINITE									"分钟"
#define		SECOND									"秒"
#define		MALE									"男"
#define		FEMALE									"女"
//---------------------------- 消息相关 ----------------------------------
// 特别注意：以下字符串长度不能超过32字节，包括 %d %s 等接收具体内容以后的长度
#define		AUTO_DESTROY							"%d天未登陆清理该角色"
#define		AUTO_DESTROY_TODAY						"今天不登陆清理该角色"
#define		ACCEPT_BIG5								"接受"
#define		CANNEL_BIG5								"拒绝"
#define		CANNEL_RETURN							"返回"
#define		CITY_REPAIR_MSG							"城市维护信息:每周消耗青铜%d个,毛皮%d个,木材%d个"
#define		MSG_GET_EXP								"您获得%d点经验值。"
#define		MSG_DEC_EXP								"您损失了%d点经验值。"
#define		MSG_LEVEL_UP							"您的等级提升至%d。"
#define		MSG_CANT_AUTORUN						"目标不在当前地图"
#define		MSG_CANT_SCENE_MAP						"迷宫没有场景地图"
#define		MSG_OPEN								"开"
#define		MSG_CLOSE								"关"
#define     MSG_BASE_INFO                            "等级 %d  %s"
#define		MSG_LEVEL								"%s %d级 %s"
#define		MSG_LEVEL_SELROLEWND					"%d级 %s"
#define     MSG_PLAYER_NAME                         "姓名:%s"
#define		MSG_JIN_YIN_TONG						"%d金%d银%d铜"
#define		MSG_ITEM_CANNT_DESTORY					"<Seg text-align=left float=wrap><Obj color=250,250,250>该物品不能被销毁</Obj></Seg></Layout>"

#define		MSG_SKILL_IN_CD							"技能冷却中！"
#define		MSG_SKILL_BEYOND_ATTACKRADIUS			"您离目标点太远，当前使用技能的有效距离不够。"
#define		MSG_SKILL_NOWEAPON						"当前技能需要武器,或使用不适技能要求的武器！"
#define		MSG_SKILL_INVALID_TARGET				"无效的目标！"
#define		MSG_SKILL_NOTARGET						"当前技能需要目标！"
#define		MSG_SKILL_TARGET_INVALID				"目标无效！"
#define		MSG_SKILL_PK_PROTECTION					"由于PK保护，你无法攻击该目标！"
#define		MSG_SKILL_LVLUP_MAXLEVEL				"技能已达到最大等级"
#define		MSG_SKILL_LVLUP_REQPLAYERLVL			"人物等级不够"
#define		MSG_SKILL_LVLUP_REQSKILL				"需求父技能等级不够"
#define		MSG_SKILL_LVLUP_REQOWNEXP				"拥有的蕴魂不够"
#define		MSG_SKILL_LVLUP_REQITEM					"缺少升级物品"
#define		MSG_OWNMONEY_NOTENOUGH					"拥有的金钱不够"

#define		MSG_TEAM_AUTO_REFUSE_INVITE				"自动拒绝他人对您的组队邀请。"
#define		MSG_TEAM_NOT_AUTO_REFUSE_INVITE			"显示他人对您的组队邀请。"
#define		MSG_TEAM_CREATE_FAIL					"队伍创建失败。"
#define		MSG_TEAM_CANNOT_CREATE					"您现在不能组队！"
#define		MSG_PLAYER_REAL_INFO_MSG				"您输入的信息中有非法字符，请重新来过。"


#define		MSG_SHOP_NO_ROOM						"背包空间不足！"
#define		MSG_SHOP_NO_MONEY						"金钱不足！"

#define		MSG_NPC_NO_MANA							"内力不足！"
#define		MSG_NPC_NO_SKILLEXP						"蕴魂不足！"
#define		MSG_NPC_NO_STAMINA						"体力不足！"
#define		MSG_NPC_NO_LIFE							"生命不足！"
#define		MSG_GET_MONEY_JIN						"你获得%d金%d银%d铜。"
#define		MSG_GET_MONEY_YIN						"你获得%d银%d铜。"
#define		MSG_GET_MONEY_TONG						"你获得%d铜。"
#define		MSG_CANT_RIDE							"您使用这项技能时不能骑坐骑！"

#define		MSG_THROW_AWAY_ITEM_ENABLE				"物品丢弃保护关闭，允许丢弃物品。"
#define		MSG_THROW_AWAY_ITEM_DISABLE				"物品丢弃保护打开，禁止丢弃物品。"

#define		MSG_CITY_DELETE_BUILDING				"城市公告:玩家[%s]删除[%s]的建筑！"
#define		MSG_CITY_FIVEEARNMONEY					"城市公告:您的诸侯获得了封神世界七大城市的税收%d！"
#define		MSG_TARGET_BEHIND_BARRIER				"目标处于障碍后面"
#define		MSG_ONENEWMAIL_NOTIFY					"您从 %s 那里收到了一封新邮件"
#define		MSG_MULTINEWMAIL_NOTIFY					"您的邮箱中有 %d 封新邮件"
#define		MSG_MAILMONEY_TITLE						"您收到了金钱"
#define		MSG_AUTORUN_NOWAY						"目标点无法到达"
#define		MSG_CLICK_TOO_FREQUENTLY				"请不要连续点击"
#define		MSG_SWITCH_SKILL_ON						"您打开了技能: %s"
#define		MSG_SWITCH_SKILL_OFF					"您关闭了技能: %s"
#define		MSG_SKILL_NEED_ITEM						"没有当前技能需要的物品"
#define		MSG_ANTIENTHRALL_WEARINESS				"您已经进入疲劳游戏时间, 您的游戏收益将降为正常值的50%, 建议您尽快下线, 适当休息!"
#define		MSG_ANTIENTHRALL_INSALUBRITY			"您已进入不健康游戏时间, 您的游戏收益已降为零, 建议您立即下线, 以免您的身体健康受到损害!"
#define		MSG_CHAT_TOOFAST						"您发送消息速度过快，请放慢速度!"
#define		MSG_MAIL_SENDSUCCEED					"邮件发送成功"
#define		MSG_CHAT_FRIENDS_ONLINE					"%s 中的 %s 上线了"
#define		MSG_CHAT_FRIENDS_OFFLINE				"%s 中的 %s 下线了"
#define		MSG_ROBOT_AUTOATTACK_ON					"您开启了自动打怪"
#define		MSG_ROBOT_AUTOATTACK_OFF				"您关闭了自动打怪"
#define		MSG_ROBOT_AUTOATTACK_NOT_CLICK_ON		"请您在遥控器面板当中设置自动打怪各种选项，然后点击保存设置按钮，再使用Ctrl+A键功能。"
#define     MSG_ROBOT_AUTOGOBACK_ERROR              "这张地图暂时不支持自动回城"

// Social system
#define		MSG_SOCIAL_ADD_SUBUNIT					"%s 邀请您加入 %s %s"
#define		MSG_SOCIAL_REMOVE_UNIT					"您被 %s 开除出了 %s %s"
#define		MSG_SOCIAL_JOIN_UNIT					"您加入了 %s %s"
#define		MSG_SOCIAL_LEAVE_UNIT					"您离开了 %s %s"
#define		MSG_SOCIAL_SUBUNIT_JOIN					"%s %s 加入了您的 %s"
#define		MSG_SOCIAL_SUBUNIT_LEAVE				"%s %s 离开了您的 %s"
#define		MSG_SOCIAL_BE_REMOVED					"您被 %s 开除出了 %s %s"
#define		MSG_SOCIAL_FORBID_CHAT					"您在 %s 中发言权被 %s 禁止"
#define		MSG_SOCIAL_UNFORBIG_CHAT				"您在 %s 中发言权被 %s 解禁"
#define		MSG_SOCIAL_UNIT_BREAK					"您的 %s 解散了"
#define		MSG_SOCIAL_BEREMOVED_OFFLINE			"您被开除出了 %s"
#define		MSG_SOCIAL_CREATE_UNIT					"您创建了 %s %s"
#define     MSG_SOCIAL_CHANGE_UNIT_OWNER            "%s将%s禅让给了%s"
#define     MSG_SOCIAL_SHIZUZHANG                   "氏族长"
#define     MSG_SOCIAL_ZHUHOUZHANG                  "侯主"
#define     MSG_SOCIAL_MENZU						"盟主"
#define		MSG_SOCIAL_REQ_JOIN  					"%s 申请加入您的 %s %s"
#define     MSG_SOCIAL_LEAGUE_BREAK					"%s 联盟宣布解散，放弃了所占领的 %s 城市！"

//#define     MSG_SOCIAL_GUOJIA                       "国王"
//#define     MAIL_SOCIAL_SENDER                      "系统"
#define     MAIL_SOCIAL_TITLE_BREAK                 "解散提示"
#define     MAIL_SOCIAL_BREAK_CONSTANT              "%s%s解散了%s!"

// Social log msg
#define		LOGMSG_SOCIAL_LOADOWNTREEERR			"玩家 %s 的社会关系数据加载出错: Tpl: %d Layer: %d"
#define		LOGMSG_SOCIAL_LOADUNITERR				"加载节点 %s 出错: Tpl: %d"

// Chat err msg
#define		MSG_CHAT_RECVERPREVENTSENDER			"对方阻止了你的消息"
#define		MSG_MAIL_MAILNOTALLOWRETURN				"此邮件不允许退回"
#define		MSG_MAIL_RETURNMAILFAILED				"退回邮件失败"
#define		MSG_MAIL_RETURNMAILSUCCEED				"退回邮件成功"
#define		MSG_MAIL_PACKAGEFULL					"背包里没有空位置"
#define		MSG_CHAT_NOPRIVCHGROOMOWNER				"没有权限移交房主"
#define		MSG_CHAT_MEMBERNOTINROOM				"成员不在房间里"
#define		MSG_CHAT_NOACCESSTOOPERATE				"没有权限进行此操作"
#define		MSG_CHAT_OPEFAILED						"操作失败"
#define		MSG_CHAT_FORBIDCHATINROOM				"您在房间里的发言权限被剥夺"
#define		MSG_CHAT_UNFORBIDCHATINROOM				"您重新拥有了在房间里的发言权限"
#define		MSG_CHAT_FORBIDCHATSUCCEED				"禁言成功"
#define		MSG_CHAT_UNFORBIDCHATSUCCEED			"解除禁言成功"

// city res
#define		NAME_CITYRES_0							"金钱"
#define		NAME_CITYRES_1							"青铜"
#define		NAME_CITYRES_2							"毛皮"
#define		NAME_CITYRES_3							"木材"

// war error msg
#define		MSG_WAR_MUSTINWARMAP					"布置祭镇坛只能在四镇战争区域才能起效"
#define		MSG_WAR_MUSTINSPECAREA					"布置祭镇坛只能在四镇战争区域才能起效"
#define		MSG_WAR_CITYINWARDECLARED				"所宣战城市正处于战争时期，无法宣战"
#define		MSG_WAR_NOWARTOOWNCITY					"不能对自己的城市宣战"
#define		MSG_WAR_CITYINWARPROTECT				"所宣战城市处在战后保护期"
#define		MSG_WAR_MUSTBETONGOWNER					"只有盟主才有资格布置祭镇坛宣战对方城市"
#define     MSG_WAR_DEST_TONG_IN_WAR                "所宣战城市正处于战争时期，无法宣战！"
#define     MSG_WAR_SOURCE_TONG_IN_WAR              "本方城市正处于战争时期，不能对外征伐"


#define		MSG_WAR_DECLAREWAR						"联盟 %s 对城市 %s 宣战成功，战事一触即发"
#define     MSG_POOL_DECLAREWAR                     "诸侯 %s 对 %s 分星池宣战成功"
#define		MSG_WAR_NOTIFY_DECLARED					"本联盟已对 %s 城宣战，请勇士们做好准备!"
#define     MSG_WAR_NOTIFY_DECLARED_DEFENDER        "联盟 %s 对本城市宣战，请将士们做好迎敌准备"
#define		MSG_WAR_NOTIFY_BEFORESTART				"本联盟对 %s 城的战争即将开始!"
#define		MSG_WAR_NOTIFY_START					"本联盟对 %s 城的战争正式开始，勇士们建功立业的时刻到了!"
#define     MSG_WAR_NOTIFY_START_PROTECTING         "本联盟对 %s 城的战争正式开始，勇士们建功立业的时刻到了!对方已经布置阵法，先摧毁阵眼才能攻击镇国木."
#define		MSG_WAR_NOTIFY_PROCESS					"本联盟正在攻打 %s 城，请火速支援!"
#define     MSG_WAR_NOTIFY_PROCESS_PROTECTED        "本联盟正在攻打 %s 城，请火速支援!对方已经布置阵法，先摧毁阵眼才能攻击镇国木."
#define     MSG_WAR_NOTIFY_PROCESS_DEFENDER         "联盟 %s 正在攻打本城，请将士们奋力防备城池，驱赶外敌！"
#define     MSG_POOL_NOTIFY_PROCESS                 "本诸侯正在攻打 %s 的分星池，请火速支援!"
#define		MSG_WAR_NOTIFY_ROBRES					"联盟 %s 从 %s 城掠夺 %d %s!"
#define		MSG_WAR_NOTIFY_ROBCITY					"联盟 %s 攻占了 %s 城!"
#define		MSG_CITY_NOTIFY_DISCOUNT				"联盟的折价率已调整，现在的折价率为 %d%%"
#define		MSG_MEMBER_ONLINE_NOTIFY				"%s 上线了!"
#define		MSG_MEMBER_OFFLINE_NOTIFY				"%s 下线了!"
#define		MSG_WAR_ROBBERPOS_NOTIFY				"敌方祭镇坛位置：%d %d" 
#define     MSG_WAR_DEFENDER_ONWAR                  "联盟 %s 对本联盟城市 %s 的战争开始了！请火速支援！"
#define     MSG_WAR_INVADER_WIN                     "本联盟对城市 %s 的战争胜利了！"
#define     MSG_WAR_POOL_INVADER_WIN                "本诸侯对 %s 分星池战争胜利了！"
#define     MSG_WAR_DEFENDER_CITY_LOSE              "联盟 %s 攻占了本联盟的城市 %s"
#define     MSG_WAR_DEFENDER_POOL_LOSE              "诸侯 %s 攻占了本诸侯在 %s 的分星池"
#define     MSG_WAR_INVADER_LOSS                    "本联盟对城市%s的战争失败了!"
#define     MSG_WAR_INVADER_POOL_LOSS               "本诸侯对%s分星池的战争失败了!"
#define     MSG_WAR_DEFENDER_WIN                    "本联盟抵挡住了联盟%s对本联盟城市%s的进攻!"
#define     MSG_WAR_POOL_DEFENDER_WIN               "本诸侯抵挡住了诸侯%s对本诸侯在%s的分星池的进攻!"
//auction

#define		MSG_AUCTION_MAILSENDER					"仙灵宝号"
#define		MSG_AUCTION_RETMONEYMAILTITLE			"竞价被超过"
#define		MSG_AUCTION_RETMONEYMAILBODY			"您对物品'%goodsname'的竞价被'%buyername'超过。"
#define		MSG_AUCTION_MAILTITLE					"拍卖成功"
#define		MSG_AUCTION_BUYERMAILBODY				"您成功竞拍获得物品'%goodsname'。"
#define		MSG_AUCTION_SELLERMAILBODY				"您竞拍的物品'%goodsname'被'%buyername'买走了。" 
#define		MSG_AUCTION_CANCELMAILTITLE				"拍卖取消" 
#define		MSG_AUCTION_CANCELMAILBODY				"您取消了对物品'%goodsname'的拍卖。" 
#define		MSG_AUCTION_RETURNMAILTITLE				"拍卖到期" 
#define		MSG_AUCTION_RETURNMAILBODY				"您拍卖的物品'%goodsname'到期了。" 
#define		MSG_MAIL_RETURN_TITLE					"【退回】"

//instance
#define		MSG_INSTANCE_RECYCLE_NOTIFY_NEAR		"您的副本“%s”由于没有玩家将在%d秒后被系统回收！"
#define		MSG_INSTANCE_RECYCLE_NOTIFY_DONE		"您的副本“%s”由于没有玩家已经被系统回收！"

//delayed action
#define		MSG_DELAYED_TRANSFER_CANCELED			"传送已经取消。"
#define		MSG_DELAYED_TRANSFER_NOTIFY				"您将在%d秒后被传送到“%s”！"

//日志事件描述
#define LOG_EVENT_NPC_DROP_ITEM_COMMENT "NPC“%s”掉落物品“%s”"
#define LOG_EVENT_NPC_DROP_MONEY_COMMENT "NPC“%s”掉落金钱%d"
#define LOG_EVENT_PICKUP_ITEM_COMMENT "玩家“%s”拾取物品“%s”"
#define LOG_EVENT_PICKUP_MONEY_COMMENT "玩家“%s”拾取金钱%d"
#define LOG_EVENT_TRADE_ITEM_COMMENT "玩家“%s”把物品“%s”交易给玩家“%s”"
#define LOG_EVENT_TRADE_MONEY_COMMENT "玩家“%s”把金钱%d交易给玩家“%s”"
#define LOG_EVENT_ADD_EXP_COMMENT "玩家“%s”获得经验%d"
#define LOG_EVENT_ADD_MONEY_COMMENT "玩家“%s”获得金钱%d"
#define LOG_EVENT_PLAYER_ENTER_WORLD_COMMENT "玩家“%s”进入游戏世界"
#define LOG_EVENT_PLAYER_LEAVE_WORLD_COMMENT "玩家“%s”离开游戏世界"
#define LOG_EVENT_NPC_SAVE_OP_LOAD_COMMENT "NPC“%s”存盘数据载入"
#define LOG_EVENT_NPC_SAVE_OP_SAVE_COMMENT "NPC“%s”存盘数据保存"
#define LOG_EVENT_NPC_SAVE_OP_DELETE_COMMENT "NPC“%s”存盘数据删除"
#define LOG_EVENT_PLAYER_STATISTIC_ADD_EXP_COMMENT "玩家“%s”最近总计获得经验%d"
#define LOG_EVENT_PLAYER_STATISTIC_ADD_MONEY_COMMENT "玩家“%s”最近总计获得金钱%d"
#define LOG_EVENT_PLAYER_STATISTIC_REMOVE_MONEY_COMMENT "玩家“%s”最近总计消耗金钱%d"
#define LOG_EVENT_PLAYER_STATISTIC_USE_SKILL_COMMENT "玩家“%s”最近总计使用技能(%d)%d次"
#define LOG_EVENT_PLAYER_STATISTIC_BUY_ITEM_COMMENT "玩家“%s”最近总计购买物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_SELL_ITEM_COMMENT "玩家“%s”最近总计出售物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_USE_ITEM_COMMENT "玩家“%s”最近总计使用物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_PICKUP_ITEM_COMMENT "玩家“%s”最近总计拾取物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_TRADE_OUT_ITEM_COMMENT "玩家“%s”最近总计交易（出）物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_TRADE_IN_ITEM_COMMENT "玩家“%s”最近总计交易（入）物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_MAIL_OUT_ITEM_COMMENT "玩家“%s”最近总计邮件（出）物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_MAIL_IN_ITEM_COMMENT "玩家“%s”最近总计邮件（入）物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_AUCTION_ITEM_COMMENT "玩家“%s”最近总计拍卖物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_DESTROY_ITEM_COMMENT "玩家“%s”最近总计销毁物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_SYSTEM_ADD_ITEM_COMMENT "玩家“%s”最近总计被系统增加物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_SYSTEM_DEL_ITEM_COMMENT "玩家“%s”最近总计被系统减少物品(%s)%d个"
#define LOG_EVENT_PLAYER_DEATH_COMMENT "玩家“%s”被“%s”杀死"
#define LOG_EVENT_NPC_DEATH_COMMENT "NPC“%s”被“%s”杀死"
#define LOG_EVENT_LEVEL_UP_COMMENT "玩家“%s”升级到等级%d，游戏时间%d"
#define LOG_EVENT_SKILL_LEVEL_UP_COMMENT "玩家“%s”提升技能(%d)到等级%d，游戏时间%d"
#define LOG_EVENT_QUEST_ACCEPT_COMMENT "玩家“%s”接受任务(%d)，游戏时间%d"
#define LOG_EVENT_QUEST_ABORT_COMMENT "玩家“%s”放弃任务(%d)，游戏时间%d"
#define LOG_EVENT_QUEST_COMPLETE_COMMENT "玩家“%s”完成任务(%d)，游戏时间%d"
#define LOG_EVENT_DESTROY_ITEM_COMMENT "玩家“%s”销毁了物品“%s”"
#define LOG_EVENT_SELL_ITEM_COMMENT "玩家“%s”出售了物品“%s”，赚得%d"
#define LOG_EVENT_BUY_ITEM_COMMENT "玩家“%s”购买了物品“%s”，花费%d"
#define LOG_EVENT_REPAIR_ITEM_COMMENT "玩家“%s”修理了物品“%s”，花费%d"
#define LOG_EVENT_USE_ITEM_COMMENT "玩家“%s”使用了物品“%s”"
#define LOG_EVENT_SEND_MAIL_ITEM_COMMENT "玩家“%s”用邮件给玩家“%s”发送了物品“%s”"
#define LOG_EVENT_GET_MAIL_ITEM_COMMENT "玩家“%s”从玩家“%s”发来的邮件中得到了物品“%s”"
#define LOG_EVENT_SEND_MAIL_COMMENT "玩家“%s”给玩家“%s”发送了一封邮件"
#define LOG_EVENT_AUCTION_ITEM_COMMENT "玩家“%s”开始拍卖物品“%s”"
#define LOG_EVENT_SYSTEM_ADD_ITEM_COMMENT "玩家“%s”从系统获得了物品“%s”"
#define LOG_EVENT_SYSTEM_DROP_ITEM_COMMENT "系统掉落物品“%s”"
#define LOG_EVENT_NPC_STATISTIC_KILL_BY_PLAYER_COMMENT "NPC(%d)最近被玩家杀死%d次"
#define LOG_EVENT_NPC_STATISTIC_KILL_PLAYER_COMMENT "NPC(%d)最近杀死玩家%d次"
#define LOG_EVENT_NPC_STATISTIC_DROP_ITEM_COMMENT "NPC(%d)最近掉落物品(%s)%d个"
#define LOG_EVENT_PLAYER_STATISTIC_USE_TAISUI_COMMENT "玩家“%s”最近总计使用太岁%d次"
#define LOG_EVENT_SOCIAL_UNIT_CREATE_COMMENT "玩家“%s”创建了“%s”"
#define LOG_EVENT_SOCIAL_UNIT_DELETE_COMMENT "玩家“%s”解散了“%s”"
#define LOG_EVENT_INSTANCE_CREATE_COMMENT "玩家“%s”创建了副本“%s”"
#define LOG_EVENT_INSTANCE_CREATE_BY_SYSTEM_COMMENT "系统创建了副本“%s”"

#define PLAYER_ACTION_LOGIN "进入游戏"
#define PLAYER_ACTION_LOGOUT "离开游戏"
#define PLAYER_ACTION_CAST_SKILL "施放技能<skill_%d>"
#define PLAYER_ACTION_USE_ITEM "使用物品<%s>"
#define PLAYER_ACTION_PICKUP_ITEM "拾取物品<%s>"
#define PLAYER_ACTION_SELL_ITEM "出售物品<%s>"
#define PLAYER_ACTION_BUY_ITEM "购买物品<%s>"
#define PLAYER_ACTION_DESTROY_ITEM "销毁物品<%s>"
#define PLAYER_ACTION_AUCTION_ITEM "开始拍卖物品<%s>"
#define PLAYER_ACTION_DIALOG_NPC "与<%s>对话"
#define PLAYER_ACTION_USE_TAISUI_WHEEL "使用太岁之轮"
#define PLAYER_ACTION_PICKUP_MONEY "拾取金钱<%d>"
#define PLAYER_ACTION_TRADE_BEGIN "开始与<%s>的交易"
#define PLAYER_ACTION_TRADE_DONE "完成与<%s>的动交易"
#define PLAYER_ACTION_TRADE_CANCEL "取消与<%s>的交易"
#define PLAYER_ACTION_TRADE_LOCK "锁定与<%s>的交易"
#define PLAYER_ACTION_DEATH "死亡"
#define PLAYER_ACTION_REVIVE "复活"
#define PLAYER_ACTION_DROP_ITEM "掉落物品<%s>"
#define PLAYER_ACTION_ENTER_WORLD "进入地图<map_%d>"
#define PLAYER_ACTION_EXIT_WORLD "离开地图<map_%d>"
//宠物相关//
#define PLAYER_PET_NAME_EXTRA		"的宠物"
#define PLAYER_EMPLOYEE_NAME_EXTRA	"雇佣兵-"

#define ITEM_CANNT_VEN	"该物品已经绑定不能拍卖"
#define ITEM_CANNT_MAIL "该物品已经绑定不能以附件形式发送"
#define ITEM_CANNT_EQUIP "该物品使用后会和你绑定"

#define MAX_NPC_PRIVATE_STATE  10

//----------------- 聊天部分错误消息 ------------------------------------
enum enumMSG_ID
{
	enumMSG_ID_NONE = 0,
	enumMSG_ID_TEAM_KICK_One,
	enumMSG_ID_TEAM_DISMISS,
	enumMSG_ID_TEAM_LEAVE,
	enumMSG_ID_TEAM_REFUSE_INVITE,
	enumMSG_ID_TEAM_SELF_ADD,
	enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL,
	enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL2,
	enumMSG_ID_OBJ_CANNOT_PICKUP,
	enumMSG_ID_OBJ_TOO_FAR,
	enumMSG_ID_DEC_MONEY,
	enumMSG_ID_TRADE_SELF_ROOM_FULL,
	enumMSG_ID_TRADE_DEST_ROOM_FULL,
	enumMSG_ID_TRADE_REFUSE_APPLY,
	enumMSG_ID_TRADE_TASK_ITEM,
	enumMSG_ID_GET_ITEM,
	enumMSG_ID_ITEM_DAMAGED,
	enumMSG_ID_MONEY_CANNOT_PICKUP,
	enumMSG_ID_CANNOT_ADD_TEAM,
	enumMSG_ID_TARGET_CANNOT_ADD_TEAM,
	enumMSG_ID_PK_ERROR_1,
	enumMSG_ID_PK_ERROR_2,
	enumMSG_ID_PK_ERROR_3,
	enumMSG_ID_PK_ERROR_4,
	enumMSG_ID_PK_ERROR_5,
	enumMSG_ID_PK_ERROR_6,
	enumMSG_ID_PK_ERROR_7,
	enumMSG_ID_PK_ERROR_9,
	enumMSG_ID_PK_ERROR_10,
	enumMSG_ID_PK_ERROR_11,
	enumMSG_ID_PK_ERROR_12,
	enumMSG_ID_PK_CAMP_LOCK,
	enumMSG_ID_PK_CAMP_UNLOCK,
	enumMSG_ID_BENPCKILL_INFO1,
	enumMSG_ID_BENPCKILL_INFO2,
	enumMSG_ID_CANT_DISCARD_ITEM,
	enumMSG_ID_DEATH_LOSE_ITEM,
	enumMSG_ID_TONG_REFUSE_ADD,
	enumMSG_ID_TONG_BE_KICK,
	enumMSG_ID_TONG_LEAVE_SUCCESS,
	enumMSG_ID_TONG_LEAVE_FAIL,
	enumMSG_ID_TONG_CHANGE_AS_MASTER,
	enumMSG_ID_TONG_CHANGE_AS_MEMBER,
	enumMSG_ID_TONG_DISMISS,
	enumMSG_ID_PK_ERROR_8,
	enumMSG_ID_YUNBIAO_CANNOT_ADD_TEAM,
	enumMSG_ID_YUNBIAO_CANNOT_APPLYADD_TEAM,
	enumMSG_ID_REWARDADD_DEBUG_NOMEMORY,
	enumMSG_ID_REWARDADD_DEBUG_PUSHTOMAP,
	enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL3,
	enumMSG_ID_GAMBLE_REFUSE_APPLY,
	enumMSG_ID_GAMBLE_TASK_ITEM,
	enumMSG_ID_CITY_ADJUST_SALARY_SYSTEM_SUCCESS,	
	enumMSG_ID_CITY_SALARY_REDEEM_SUCCESS,
	enumMSG_ID_CITY_SALARY_REDEEM_FAILED,
	enumMSG_ID_CITY_SHOP_ADJUST_PRICE_SUCCESS,
	enumMSG_ID_CITY_SHOP_ADJUST_PRICE_FAILED,
	enumMSG_ID_NUM,
};

enum	enChatErrorCode
{
	chat_err_none = 0,

	chat_err_noaccesstosend,
	chat_err_preventreceiver,
	chat_err_failed,
	chat_err_maxfriend,
	chat_err_maxroom,
	chat_err_serverroomfull,
	chat_err_createroomfailed,
	chat_err_denyaddmembertoroom,
	chat_err_memberroomfull,
	chat_err_membernotonline,
	chat_err_noaccesscreateroom,
	chat_err_noaccesskickmember,
	chat_err_groupnamelengtherr,
	chat_err_groupexist,
	char_err_groupnotempty,
	char_err_objnotmoveable,
	chat_err_objectnotexist,
	char_err_groupnotexist,
	chat_err_notallowitemwithmail,
	chat_err_sendmailfailed,
	chat_err_noreceiver,
	char_err_membernotexist,
	char_err_invalidrelation,
	chat_err_roomnotexist,
	chat_err_objalreadyexist,
	chat_err_exceedmaxgroup,
	chat_err_loadfriendsdatafailed,
	chat_err_noaccesstorecv,
	chat_err_accessorycountexceed,
	chat_err_maillengthexceed,
	chat_err_invaliditeminmail,
	chat_err_contentlenexceed,
	chat_err_sendbufoverflow,
	chat_err_mailplusexceed,
	chat_err_mailnotfound,
	chat_err_posinvalid,
	chat_err_itemnotfound,
	chat_err_dstposnotempty,
	chat_err_cantaddself,
	chat_err_roommemberfull,
	char_err_sendmailsucceed,
	chat_err_delmailfailed,
	char_err_moneynotenough,
	char_err_dbreqondoing,
	char_err_chgchatprivfailed,
	char_err_lawlesstext,
	chat_err_delgroupfailed,
	chat_err_renamegroupfailed,
	chat_err_antienthralweariness,
	chat_err_antienthralinsalubrity,
	chat_err_sendmailsuccess,
	chat_err_recverpreventsender,
	chat_err_mailnotallowreturn,
	chat_err_returnmailfailed,
	chat_err_returnmailsucceed,
	char_err_packagefull,
	chat_err_noprivchgroomowner,
	chat_err_membernotinroom,
	chat_err_joinroomnotify,
	chat_err_addobjectfailed,
	chat_err_playernotexist,
	chat_err_noaccesstooperate,
	chat_err_opefailed,

	chat_err_war_mustinwarmap,
	chat_err_war_mustinspecarea,
	chat_err_war_cityinwardeclared,
	chat_err_war_nowartoowncity,
	char_err_war_cityinwarprotected,
	chat_err_war_mustbetongowner,
	char_err_war_dest_tong_inwar,
	char_err_war_source_tong_inwar,

	chat_err_pool_mustbeowner,
	chat_err_pool_mustinpoolmap,
	chat_err_pool_item_needed,
	chat_err_pool_no_combat_to_own,
	chat_err_pool_in_combat,
	chat_err_pool_wrong_time,
	chat_err_pool_sys_busy,

	chat_err_chattoofast,
	chat_err_itembox_not_enough_space,

	chat_err_opetoofast,
	chat_err_playerbecamou,

	chat_err_level_invalid,
	chat_err_prevent_add_friend,
	
	chat_err_end,
};

enum enumItemErrCode
{
	item_inlay_ok_normal,
	item_inlay_ok_yin,
	item_inlay_ok_yang,
	item_inlay_ok_special,
	item_inlay_ok_reset,
	item_inlay_error,
	item_inlay_error_cannot,
	item_inlay_error_empty,
	item_inlay_error_full,
	item_inlay_error_reset,
	item_inlay_error_targetitem_rule,
	item_inlay_error_repair_by_item,
	item_inlay_error_levelup_tool_ok,
	item_inlay_error_count,
};

static const char *g_szItemErrMsg[item_inlay_error_count] = 
{
	"灵石镶嵌成功。",						
	"灵石镶嵌成功，并附加了阴属性。",
	"灵石镶嵌成功，并附加了阳属性。",
	"灵石镶嵌成功，并激活了灵石之语。",
	"道具重新打孔成功。",
	"灵石镶嵌失败。",
	"灵石镶嵌规则。",
	"该装备未开孔。",
	"该装备孔已满。",
	"道具重新打孔失败。",
	"错误的道具使用规则。",
	"修理失败，请使用更高级的寒铁。",
	"赋予装备升级属性成功。",
};

static const char *g_szChatErrMsg[chat_err_end] = 
{
	"",						// reserve, make error code can be used an index
	"没有权限发送消息",
	"阻止了接收人",
	"发送失败",
	"好友数量达到了上限",
	"能加入的房间数量达到上限",
	"服务器房间数量达到上限",
	"创建房间失败",
	"没有权限向房间添加成员",
	"该成员加入的房间已达上限，无法添加该成员",
	"成员不在线",
	"没有权限创建房间",
	"没有权限踢出成员",
	"组名字长度不合法",
	"组的名字不能相同",
	"不能删除非空组",
	"改对象不能被移动",
	"对象不存在",
	"组不存在",
	"此邮件不允许附带附件",
	"发送邮件失败",
	"没有接收者",
	"玩家不存在",
	"玩家关系不合法",
	"房间不存在",
	"玩家已经在列表中",
	"分组数量达到了上限",
	"加载好友数据失败",
	"没有权限接收消息",
	"附件个数超出了上限",
	"邮件长度超出上限",
	"邮件中有无效的物品",
	"内容长度超过了上限",
	"聊天系统发送缓冲区溢出",
	"附件个数超出上限",
	"找不到邮件",
	"此位置不能放该物品",
	"附件中没有此物品",
	"目标位置非空",
	"不能添加自己为好友",
	"房间成员已满",
	"邮件发送成功",
	"删除邮件失败",
	"金钱不够",
	"操作正在进行，请稍候",
	"更改聊天权限失败",
	"您的聊天文本包含非法关键字", // chat_err_lawlesstext
	"删除组失败", // chat_err_delgroupfailed
	"更改组名字失败", // chat_err_renamegroupfailed
	MSG_ANTIENTHRALL_WEARINESS,    // chat_err_antienthralweariness
	MSG_ANTIENTHRALL_INSALUBRITY,  // chat_err_antienthralinsalubrity
	MSG_MAIL_SENDSUCCEED,		   // chat_err_sendmailsuccess
	MSG_CHAT_RECVERPREVENTSENDER,	// chat_err_recverpreventsender
	MSG_MAIL_MAILNOTALLOWRETURN,	// chat_err_mailnotallowreturn
	MSG_MAIL_RETURNMAILFAILED,		// chat_err_returnmailfailed
	MSG_MAIL_RETURNMAILSUCCEED,		// chat_err_returnmailsucceed
	MSG_MAIL_PACKAGEFULL,			// char_err_packagefull
	MSG_CHAT_NOPRIVCHGROOMOWNER,	// chat_err_noprivchgroomowner
	MSG_CHAT_MEMBERNOTINROOM,		// chat_err_membernotinroom
	"您已经加入了一个新的聊天室",	
	"添加好友失败",					// chat_err_addobjectfailed,
	"玩家不存在",					// chat_err_playernotexist,
	MSG_CHAT_NOACCESSTOOPERATE,		// chat_err_noaccesstooperate
	MSG_CHAT_OPEFAILED,				// chat_err_opefailed,

	MSG_WAR_MUSTINWARMAP,			// chat_err_war_mustinwarmap
	MSG_WAR_MUSTINSPECAREA,			// chat_err_war_mustinspecarea
	MSG_WAR_CITYINWARDECLARED,		// chat_err_war_cityinwardeclared
	MSG_WAR_NOWARTOOWNCITY,			// chat_err_war_nowartoowncity
	MSG_WAR_CITYINWARPROTECT,		// char_err_war_cityinwarprotected
	MSG_WAR_MUSTBETONGOWNER,		// chat_err_war_mustbetongowner
	
	MSG_WAR_DEST_TONG_IN_WAR,       //char_err_war_dest_tong_inwar,
	MSG_WAR_SOURCE_TONG_IN_WAR,     //char_err_war_source_tong_inwar,

	"只有侯主才有资格进行操作",     //chat_err_pool_mustbeowner
	"只能对分星池进行这个操作",     //chat_err_pool_mustinpoolmap
	"您必须拥有通幽石",             //chat_err_pool_item_needed
	"您的诸侯已经占有了分星池",     //chat_err_pool_no_combat_to_own
	"对象分星池正在战争中",         //chat_err_pool_in_combat
	"当前不是宣战时间",             //chat_err_pool_wrong_time
	"系统忙，请稍后重试",           //chat_err_pool_sys_busy
	MSG_CHAT_TOOFAST,				// caht_err_chattoofast
	"背包已满",						//chat_err_pool_sys_busy
	"请求失败，请放慢操作速度再试一次",	//chat_err_opetoofast
	"玩家已隐身",					//chat_err_playerbecamou
	"您的等级不足",                 //chat_err_level_invalid
	"您目前的状态不能进行好友操作", //chat_err_prevent_add_friend
};

enum	enAuctionMsgCode
{
	enAucMsgCode_Begin = -1,

	enAucMsgCode_GoodsOnLock,
	enAucMsgCode_OpeOnDoing,
	enAucMsgCode_MoneyNotEnough,
	enAucMsgCode_InvalidPrice,
	enAucMsgCode_NoRecord,
	enAucMsgCode_ItemCantTrade,
	enAucMsgCode_BidSucceed,
	enAucMsgCode_CancelSucceed,
	enAucMsgCode_DBOpeFailed,
	enAucMsgCode_BidExceedByOther,

	enAccMsgCod_OpeTooFast,

	// Add after here

	enAucMsgCode_Num,
};

static const char *g_szAuctionMsg[enAucMsgCode_Num] = 
{
	"操作失败",
	"操作正在进行，请稍候",
	"您的金钱不够",
	"一口价必须大于竞价",
	"没有找到纪录",
	"物品不能交易",
	"竞价成功",
	"取消拍卖成功",
	"数据库操作失败",
	"你正在竞拍的物品已有更高出价",
	"请求失败，请放慢操作速度再试一次",
};

enum	enSocialErrCode
{
	enSocialErr_AnnounceLenExceed = -3,
	enSocialErr_CheckUnitName = -2,

	// None之前的错误码只是用来做一些标识
	// 表示某个操作不是由于出错导致未完成
	// 不同步到客户端
	enSocialErr_None = -1,

	enSocialErr_UnitNotFound,
	enSocialErr_AddChildFailed,
	enSocialErr_CreateFailed,
	enSocialErr_MoneyNotEnough,
	enSocialErr_TeamMemTooLess,
	enSocialErr_TeamMemTooMore,
	enSocialErr_LevelInValid,
	enSocialErr_MemberInvalid,
	enSocialErr_NoPrivilige,
	enSocialErr_LeaveFailed,
	enSocialErr_RemoveFailed,
	enSocialErr_PlayerNotOnline,
	enSocialErr_ForbidChatFailed,
	enSocialErr_UnForbidChatFailed,
	enSocialErr_PubAnnounceFailed,
	enSocialErr_GetSubListFailed,
	enSocialErr_OperationNotExist,
	enSocialErr_OperationFailed,
	enSocialErr_GetAnnounceFailed,
	enSocialErr_AnnounceNotExist,
	enSocialErr_RetryLater,
	enSocialErr_NameAlreadyExist,
	enSocialErr_MemberNumIsFull,
	enSocialErr_AlreadyLastPage,
	enSocialErr_LawlessText,
	enSocialErr_CreaterMustBeCaption,
	enSocialErr_RelationInvalid,
	enSocialErr_HaveLeaveBuff,
	enSocialErr_TeamMemberTooFar,
	enSocialErr_WaitToSetCityTaxRate,
	enSocialErr_RequestIgnored,
	enSocialErr_RequestRefused,
	enSocialErr_ChangeOwnerFaild,
    enSocialErr_ChangeOwnerSubUnitNeeded,
	enSocialErr_ChangeOwnerSubOrderNeeded,
    enSocialErr_ChangeOwnerSubOrderOnlineNeeded,
	enSocialErr_ChangeOwnerSubUnitOwnerNeeded,
	enSocialErr_UnitNoExist,
	enSocialErr_ReqJoinFaild,
	enSocialErr_OwnerNotOnline,
	enSocialErr_RecruitExist,
	enSocialErr_RecruitNotExist,
	enSocialErr_RecruitFailed,
	enSocialErr_NoInfo,
	enSocialErr_WaitToConfirm,
	enSocialErr_RecruitOpSuc,
	enSocialErr_NoChatOpeWhenChangeOwner,
	enSocialErr_NoChatOpeWhenChangeOwnerSelf,

	enSocialErr_Num,
};

//stringresource.txt中的字符串id
enum enumStringResourceId
{
	sid_pk_protection_target = 11370,	//20级以下玩家禁止PK

	sid_can_not_pickup_item = 11381,	//你无法拾取物品（金钱除外）
	sid_bag_is_full,					//您的包裹已满

	sid_instance_expire = 11391,		//副本过期

	sid_already_applyed_join_team = 11398,		//已经申请加入队伍
	sid_already_invited_join_team,		//已经邀请加入队伍
	sid_already_has_team,				//已经加入了一个队伍
	sid_you_create_team,				//创建队伍
	sid_open_big_team,					//开启团队
	sid_you_join_big_team,				//你加入团队
	sid_you_join_team,					//你加入队伍
	sid_create_team_error,				//创建队伍失败
	sid_open_big_team_error,			//转化团队失败
	sid_new_captain,					//新的队长
	sid_new_assistant,					//新的助手
	sid_join_team,						//加入队伍
	sid_leave_team,						//离开队伍
	sid_be_dismiss,						//被解职
	sid_you,							//你
	sid_level_up,						//升级
	sid_you_already_has_team,			//你已经创建了一个队伍
	sid_you_can_not_create_team,		//你暂时无法创建队伍
	sid_invite_not_avaiable,			//目前不能邀请
	sid_team_not_avaiable,				//队伍已满或不存在
	sid_apply_not_avaiable,				//目前不能申请
	sid_apply_join_team_refused,		//申请加入队伍被拒绝

	sid_opposite_remove_trade_item = 11430,		//对方从交易栏中拿起了物品
	sid_self_gain_item,					//自己获得了物品
	sid_self_pickup_item,				//自己通过拾取获得了物品
	sid_self_trade_item,				//自己通过交易获得了物品
	sid_self_buy_item,					//自己通过购买获得了物品
	sid_self_add_exp,					//自己获得经验
	sid_self_add_skill_exp,				//自己获得蕴魂
	sid_unknown_target,					//未知目标
	sid_unknown_skill,					//未知技能
	sid_target_you,						//你（目标）
	sid_damage_critical,				//致命（伤害）
	sid_heal_critical,					//会心（治疗）
	sid_damage_self_life,				//自己损失生命
	sid_damage_target_life,				//他人损失生命
	sid_damage_self_life_critical,		//自己损失生命（致命）
	sid_damage_target_life_critical,	//他人损失生命（致命）
	sid_damage_self_mana,				//自己损失法力
	sid_damage_target_mana,				//他人损失法力
	sid_damage_self_mana_critical,		//自己损失法力（致命）
	sid_damage_target_mana_critical,	//他人损失法力（致命）
	sid_heal_self_life,					//自己补充生命
	sid_heal_target_life,				//他人补充生命
	sid_heal_self_life_critical,		//自己补充生命（致命）
	sid_heal_target_life_critical,		//他人补充生命（致命）
	sid_heal_self_mana,					//自己补充法力
	sid_heal_target_mana,				//他人补充法力
	sid_heal_self_mana_critical,		//自己补充法力（致命）
	sid_heal_target_mana_critical,		//他人补充法力（致命）
	sid_dodge_self,						//自己躲闪
	sid_dodge_target,					//他人躲闪
	sid_dodge_self_critical,			//自己躲闪
	sid_dodge_target_critical,			//他人躲闪
	sid_absorb_self_life,				//自己吸收损失生命
	sid_absorb_target_life,				//他人吸收损失生命
	sid_absorb_self_life_critical,		//自己吸收损失生命（致命）
	sid_absorb_target_life_critical,	//他人吸收损失生命（致命）
	sid_absorb_self_mana,				//自己吸收损失法力
	sid_absorb_target_mana,				//他人吸收损失法力
	sid_absorb_self_mana_critical,		//自己吸收损失法力（致命）
	sid_absorb_target_mana_critical,	//他人吸收损失法力（致命）
	sid_combat_info_basic,				//战斗信息基本模板
	sid_combat_info_skill_name,			//战斗信息技能名称模板
	sid_combat_info_caster_name,		//战斗信息施放者名称模板
	sid_combat_info_target_name,		//战斗信息接受者名称模板
	sid_combat_info_score_get,          //战斗信息积分获取
	sid_walk_adjust,                    //自动寻路位置调整

	sid_private_state_name     = 80000, //玩家蒙面时显示的名字
	sid_private_state_name_end = sid_private_state_name + MAX_NPC_PRIVATE_STATE, //玩家蒙面显示的名字 End
};

static const char *g_szSocialMsg[enSocialErr_Num] = 
{
	"找不到节点",				// enSocialErr_UnitNotFound,
	"添加成员失败",				// enSocialErr_AddChildFailed,
	"创建失败",					// enSocialErr_CreateFailed,
	"所需金钱不够",				// enSocialErr_MoneyNotEnough,
	"队友数量不够",				// enSocialErr_TeamMemTooLess,
	"队友数量太多",				// enSocialErr_TeamMemTooMore,
	"成员等级不足",				// enSocialErr_LevelInValid,
	"成员无效",					// enSocialErr_MemberInvalid,	
	"权限不够",					// enSocialErr_NoPrivilige,
	"离开失败",					// enSocialErr_LeaveFailed,
	"开除成员失败",				// enSocialErr_RemoveFailed,
	"玩家不在线",				// enSocialErr_PlayerNotOnline,
	"禁言失败",					// enSocialErr_ForbidChatFailed,
	"解禁失败",					// enSocialErr_UnforbidChatFailed,
	"发布公告失败",				// enSocialErr_PubAnnounceFailed,
	"获取列表失败",				// enSocialErr_GetSubListFailed,
	"操作不存在",				// enSocialErr_OperationNotExist,
	"操作失败",					// enSocialErr_OperationFailed,
	"获取公告失败",				// enSocialErr_GetAnnounceFailed,
	"当前没有公告",				// enSocialErr_AnnounceNotExist,
	"数据库忙，请稍后重试",		// enSocialErr_RetryLayer,
	"名字已经存在，请更换",		// enSocialErr_NameAlreadyExist,
	"成员数量已满",				// enSocialErr_MemberNumIsFull
	"已经是最后一页",			// enSocialErr_AlreadyLastPage,
	"名字含有非法关键词",		// enSocialErr_LawlessText,
	"创建者必须是队长",			// enSocialErr_CreaterMustBeCaption,
	"玩家已有的社会关系不正确",	// enSocialErr_RelationInvalid,
	"玩家拥有脱离buff",			// enSocialErr_HaveLeaveBuff,
	"队伍中有成员距离过远",		// enSocialErr_TeamMemberTooFar,
	"设置折扣率间隔时间未达到",	// enSocialErr_WaitToSetCityTaxRate,
	"对方忽略了您的请求",       // enSocialErr_RequestIgnored
	"对方拒绝了您的请求",       // enSocialErr_RequestRefuesed
	"禅让失败" ,                 //enSocialErr_ChangeOwnerFaild,
    "只能禅让给您的下级"  ,      //enSocialErr_ChangeOwnerSubUnitNeeded,
	"只能从上到下逐级禅让",      // enSocialErr_ChangeOwnerSubOrderNeeded,
    "对方不在线",                //enSocialErr_ChangeOwnerSubOrderOnlineNeeded,
	"必须禅让给下一层领导",       //enSocialErr_ChangeOwnerSubUnitOwnerNeeded,
	"请求的组织已经不存在",      //enSocialErr_UnitNoExist
	"申请加入失败",              //enSocialErr_ReqJoinFaild
	"对方组织管理者不在线",      //enSocialErr_OwnerNotOnline    
	"您已经发布了招募信息",      //enSocialErr_RecruitExist
	"您操作的信息已经不存在",     //enSocialErr_RecruitNotExist
	"您的操作失败",              //enSocialErr_RecruitFailed
	" ",                         //enSocialErr_NoInfo    
	"操作成功,等待回复",        //enSocialErr_WaitToConfirm
	"操作成功",                 //enSocialErr_RecruitOpSuc
	"对方处于禁言状态，操作失败", //enSocialErr_NoChatOpeWhenChangeOwner          
	"您处于禁言状态，操作失败",   //enSocialErr_NoChatOpeWhenChangeOwnerSelf
};

enum enIBShopErrCode
{
	enIBShopErr_None = -1,
	
	enIBShopErr_UnInit,
	enIBShopErr_NoShop,
	enIBShopErr_NoShelf,
	enIBShopErr_OnLoading,
	enIBShopErr_CannotFindGoods,
	enIBShopErr_GoodsInvalid,
	enIBShopErr_GoodsCountTooMany,
	enIBShopErr_RequestTooFast,
	enIBShopErr_UpdateShelf,
	enIBShopErr_NoEnoughMoney,
	enIBShopErr_NoEnoughJinShanBi,
	enIBShopErr_NoEnoughCreditPoint,
	enIBShopErr_NoEnoughPoint,
	enIBShopErr_NoEnoughTicket,
	enIBShopErr_IsNotIBGoods,
	enIBShopErr_OpeBuyFailed,
	enIBShopErr_NoEnoughJinShanBiReturnFailed,
	enIBShopErr_NoEnoughSpace,
	enIBShopErr_ErrReqBuyNum,
	enIBShopErr_OnceItemUseOk,
	enIBShopErr_Unknown,
	enIBShopErr_ChargeSucc,
	
	enIBShopErr_End,
};

static const char *g_szIBShopErrMsg[enIBShopErr_End] = 
{
	"IB商店更新完成，请稍后再试",	// enIBShopErr_UnInit
	"找不到指定商店数据",			// enIBShopErr_NoShop
	"找不到商店指定货柜",			// enIBShopErr_NoShelf
	"商店更新中，请稍后再试",		// enIBShopErr_OnLoading
	"找不到IB商品",					// enIBShopErr_CannotFindGoods
	"需求物品已下架或不合法",		// enIBShopErr_GoodsInvalid
	"物品个数超出上限",				// enIBShopErr_GoodsCountTooMany
	"请放慢刷新速度",				// enIBShopErr_RequestTooFast
	"IB商店更新完成,请重新操作",	// enIBShopErr_UpdateShelf
	"金钱不足，无法购买",			// enIBShopErr_NoEnoughMoney
	"通宝不足，无法购买",			// enIBShopErr_NoEnoughJinShanBi
	"信用点不足，无法购买",			// enIBShopErr_NoEnoughCreditPoint
	"积分点不足，无法购买",			// enIBShopErr_NoEnoughPoint
	"代金券不足，无法购买",			// enIBShopErr_NoEnoughTicket
	"非IB物品，无法购买",			// enIBShopErr_IsNotIBGoods
	"购买失败，请重新购买",			// enIBShopErr_OpeBuyFailed
	"通宝不足，还款失败",			// enIBShopErr_NoEnoughJinShanBiReturnFailed
	"背包没有足够空间",				// enIBShopErr_NoEnoughSpace
	"每次只能购买一组物品",			// enIBShopErr_ErrReqBuyNum
	"IB道具使用成功",					// enIBShopErr_OnceItemUseOk
	"IB商店错误",					// enIBShopErr_Unknown
	"通宝充值成功",
};

/************************************************************************/
/*								路径定义                                */
/************************************************************************/

#define		ALPHA_BMP_PATH							"\\Spr\\Ui4\\MiniMapAlpha.bmp"
#define		DEFAULT_GROUND							"\\游戏资源\\室外地表\\中型地表图素\\绿草.spr"
#define		NPC_SHADOW								"\\spr\\skill\\补充\\sd_shadow.spr"
#define		BUFF_SOUND								"\\sound\\buff\\buffsound%d.wav"
#define		BUFF_ONCE_SOUND							"\\sound\\buff\\buffoncesound%d.wav"
//--------------------------- 客户端配置 相关 ----------------------------
#define		DEFAULT_ROOT_DIR						"\\"

//<------EXVERSION						
#ifndef _EX_SERVER_MODE_
	#define		PACKAGE_INI							"\\Package.ini"	
	#define		VERSION_CFG							"Version.cfg"
	#define		LAST_VERSION						"LastVersion"
	#define		AUTOUPDATE_INI						"\\settings\\autoupdate.ini"
	#define		DUMP_VER							"Ver_%d"
#else
	#define		PACKAGE_INI							"\\Packageex.ini"
	#define		VERSION_CFG							"Versionex.cfg"
	#define		LAST_VERSION						"LastVersionex"
	#define		AUTOUPDATE_INI						"\\settings\\autoupdateex.ini"
	#define		DUMP_VER							"Ex_Ver_%d"
#endif
#define		CONFIG_INI								"\\config.ini"
#define		UI_AUTOEXEC_SETTING_FILE				"\\UserData\\autoexec.lua"
#define		PLACE_LIST_FILE							"\\Settings\\NativePlaceList.ini"
#define		STRING_RES_SCEME						"UiSettings\\String.ini"
#define		UI_CFG_STRING							"\\UiSettings\\Uicfg.ini"
#define		FACE_LAYOUT_STRING						"\\UiSettings\\Face.ini"
#define		MAP_SETTING_FILE						"\\UiSettings\\Setting.ini"
#define		FONT_SECTION							"FontList"
#define		ROLE_FIRST_LOGIN						"bFirsLogin"
#define		MAP_LIST_SETTING						"\\settings\\maplist.ini"
//------------------------------- UI 相关 --------------------------------
#define		UI_DEFAULT_SKIN_SCHEME					"uisettings/schemes/TaharezLook.sce"
#define     UI_DEFAULT_SKIN_IMAGESET				"TaharezLook"
#define 	UI_DEFAULT_MOUSEARROW					"MouseArrow"
#define		UI_DEFAULT_FONT_SCHEME					"uisettings/fonts/default.fnt" 
#define		UI_SONG_FONT_SCHEME						"uisettings/fonts/Song.fnt" 

#define     UI_LOGINBK_IMAGESET_NAME_SEL_JS_0		"JSBackgroundImage_sel_0"
#define		UI_LOGINBK_IMAGE_PATH_SEL_JS_0			"ui/imagesets/staticimage/selrolejiashi0.spr"
#define     UI_LOGINBK_IMAGESET_NAME_SEL_DS_0		"DSBackgroundImage_sel_0"
#define		UI_LOGINBK_IMAGE_PATH_SEL_DS_0			"ui/imagesets/staticimage/selroledaoshi0.spr"
#define     UI_LOGINBK_IMAGESET_NAME_SEL_YR_0		"YRBackgroundImage_sel_0"
#define		UI_LOGINBK_IMAGE_PATH_SEL_YR_0			"ui/imagesets/staticimage/selroleyiren0.spr"
#define     UI_LOGINBK_IMAGESET_NAME_SEL_JS_1		"JSBackgroundImage_sel_1"
#define		UI_LOGINBK_IMAGE_PATH_SEL_JS_1			"ui/imagesets/staticimage/selrolejiashi1.spr"
#define     UI_LOGINBK_IMAGESET_NAME_SEL_DS_1		"DSBackgroundImage_sel_1"
#define		UI_LOGINBK_IMAGE_PATH_SEL_DS_1			"ui/imagesets/staticimage/selroledaoshi1.spr"
#define     UI_LOGINBK_IMAGESET_NAME_SEL_YR_1		"YRBackgroundImage_sel_1"
#define		UI_LOGINBK_IMAGE_PATH_SEL_YR_1			"ui/imagesets/staticimage/selroleyiren1.spr"

#define     UI_LOGINBK_IMAGESET_NAME_CREATE_JS_0	"JSBackgroundImage_create_0"
#define		UI_LOGINBK_IMAGE_PATH_CREATE_JS_0		"ui/imagesets/staticimage/createrolejiashi0.spr"
#define     UI_LOGINBK_IMAGESET_NAME_CREATE_DS_0	"DSBackgroundImage_create_0"
#define		UI_LOGINBK_IMAGE_PATH_CREATE_DS_0		"ui/imagesets/staticimage/createroledaoshi0.spr"
#define     UI_LOGINBK_IMAGESET_NAME_CREATE_YR_0	"YRBackgroundImage_create_0"
#define		UI_LOGINBK_IMAGE_PATH_CREATE_YR_0		"ui/imagesets/staticimage/createroleyiren0.spr"
#define     UI_LOGINBK_IMAGESET_NAME_CREATE_JS_1	"JSBackgroundImage_create_1"
#define		UI_LOGINBK_IMAGE_PATH_CREATE_JS_1		"ui/imagesets/staticimage/createrolejiashi1.spr"
#define     UI_LOGINBK_IMAGESET_NAME_CREATE_DS_1	"DSBackgroundImage_create_1"
#define		UI_LOGINBK_IMAGE_PATH_CREATE_DS_1		"ui/imagesets/staticimage/createroledaoshi1.spr"
#define     UI_LOGINBK_IMAGESET_NAME_CREATE_YR_1	"YRBackgroundImage_create_1"
#define		UI_LOGINBK_IMAGE_PATH_CREATE_YR_1		"ui/imagesets/staticimage/createroleyiren1.spr"

#define     UI_LOGINBK_IMAGESET_NAME_CLICK_JS_0		"JSBackgroundImage_click_0"
#define		UI_LOGINBK_IMAGE_PATH_CLICK_JS_0		"ui/imagesets/staticimage/clickrolejiashi0.spr"
#define     UI_LOGINBK_IMAGESET_NAME_CLICK_JS_1		"JSBackgroundImage_click_1"
#define		UI_LOGINBK_IMAGE_PATH_CLICK_JS_1		"ui/imagesets/staticimage/clickrolejiashi1.spr"
#define     UI_LOGINBK_IMAGESET_NAME_CLICK_DS_0		"DSBackgroundImage_click_0"
#define		UI_LOGINBK_IMAGE_PATH_CLICK_DS_0		"ui/imagesets/staticimage/clickroledaoshi0.spr"
#define     UI_LOGINBK_IMAGESET_NAME_CLICK_DS_1		"DSBackgroundImage_click_1"
#define		UI_LOGINBK_IMAGE_PATH_CLICK_DS_1		"ui/imagesets/staticimage/clickroledaoshi1.spr"
#define     UI_LOGINBK_IMAGESET_NAME_CLICK_YR_0		"YRBackgroundImage_click_0"
#define		UI_LOGINBK_IMAGE_PATH_CLICK_YR_0		"ui/imagesets/staticimage/clickroleyiren0.spr"
#define     UI_LOGINBK_IMAGESET_NAME_CLICK_YR_1		"YRBackgroundImage_click_1"
#define		UI_LOGINBK_IMAGE_PATH_CLICK_YR_1		"ui/imagesets/staticimage/clickroleyiren1.spr"

#define		UI_DEFAULTGUISHEET						"DefaultGUISheet"
#define		UI_DEFAULT_GUISHEET_ROOT				"root"
#define		UI_DEFAULT_GUISHEET_ROOT_2				"root2"
#define		UI_FULL_IMAGESET						"full_image"
#define		UI_LAYOUT_PATH							"uisettings/layouts/"
#define		UI_LAYOUT_FILTER						"uisettings/layouts/*.ls"
#define		UI_1024_PATH							"uisettings/layouts1024/"
#define		UI_BUBBLETIP							"uisettings/layouts/BubbleTip.ls"
#define		UI_TRAFFICLIGHT							"uisettings/layouts/TrafficLight.ls"
#define		UI_CITYBASEINFOPAGE						"uisettings/layouts/CityBaseInfoPage.ls"
#define		UI_CITYBUILDINGPAGE						"uisettings/layouts/CityBuildingInfoPage.ls"
#define		UI_TONGCREATE							"uisettings/layouts/tongcreate.ls"
#define		UI_TONGMANAGER							"uisettings/layouts/TongManager.ls"
#define		UI_TONGOPERMGR							"uisettings/layouts/TongOperMgr.ls"
#define		UI_CITYMANAGER							"uisettings/layouts/CityManager.ls"
#define		UI_CITYRESMGR							"uisettings/layouts/CityResMgr.ls"
#define		UI_SHIZUPAGE							"uisettings/layouts/ShizuPage"
#define		UI_ZHUHOUPAGE							"uisettings/layouts/ZhuhouPage"
#define		UI_GUOJIAPAGE							"uisettings/layouts/GuojiaPage"
#define		UI_CHATROOM								"uisettings/layouts/ChatRoom.ls"
#define     UI_PRIVATECHATROOM                      "uisettings/layouts/PrivateChatRoom.ls"
#define		UI_ITEMVENDUEBAR						"uisettings/layouts/itemvenduebar.ls"
#define		UI_ITEMVENDUEPAGE						"uisettings/layouts/itemvenduepage.ls"
#define		UI_SHORTCUTBAR							"uisettings/layouts/ShortcutBar.ls"	
#define		UI_SHORTCUTPLUSBAR						"uisettings/layouts/ShortcutPlusBar.ls"	
#define		UI_TEAMMEMBER							"uisettings/layouts/TeamMember.ls"	
#define		UI_TEAMLIST								"uisettings/layouts/TeamList.ls"	
#define		UI_ROLEPOPMENU							"uisettings/layouts/RolePopMenu.ls"	
#define		UI_TEAMMEMBERPOPMENU					"uisettings/layouts/TeamMemberPopMenu.ls"	
#define		UI_LOGINBK								"uisettings/layouts/LoginBK.ls"
#define		UI_PASSWORD								"uisettings/layouts/PassWord.ls"
#define		UI_UPDATETIP							"uisettings/layouts/UpdateTip.ls"
#define		UI_SELROLE								"uisettings/layouts/SelRole.ls"
#define		UI_NEWROLE								"uisettings/layouts/NewRole.ls"
#define		UI_CONNECTINFO							"uisettings/layouts/Connect.ls"
#define		UI_NEWROLEINFO							"uisettings/layouts/NewRoleInfo.ls"
#define		UI_CHATCENTRE							"uisettings/layouts/ChatCentre.ls"
#define		UI_NAVICATION							"uisettings/layouts/Navigation.ls"
#define		UI_MININAVICATION						"uisettings/layouts/MiniNavigation.ls"
#define		UI_COMMONMSGBOX							"uisettings/layouts/CommonMsgBox.ls"
#define		UI_EXITBOX								"uisettings/layouts/ExitGame.ls"
#define		UI_ROLEFACE								"uisettings/layouts/RoleFace.ls"
#define		UI_ROLEEXP								"uisettings/layouts/RoleExp.ls"
#define		UI_SHORTCUTWND							"uisettings/layouts/ShortcutWnd.ls"
#define		UI_SHORTCUTPLUSWND						"uisettings/layouts/ShortcutPlusWnd.ls"
#define		UI_CHATCHANNELWND						"uisettings/layouts/ChannelCentre.ls"
#define		UI_ADDFRIEND							"uisettings/layouts/AddFriend.ls"
#define		UI_CHATMSGNOTIFY						"uisettings/layouts/ChatMsgNotify.ls"
#define		UI_MAILCENTRE							"uisettings/layouts/MailCentre.ls"
#define		UI_MINIMAP								"uisettings/layouts/MiniMap.ls"
#define		UI_BIGMAP								"uisettings/layouts/BigMap.ls"
#define		UI_BUFFERWND							"uisettings/layouts/BufferWnd.ls"
#define		UI_DEATHWND								"uisettings/layouts/DeathBox.ls"
#define		UI_DEBUFFERWND							"uisettings/layouts/DebufferWnd.ls"
#define		UI_TARGETBUFFERWND						"uisettings/layouts/TargetbufferWnd.ls"
#define		UI_TARGETFACE							"uisettings/layouts/TargetFace.ls"
#define		UI_SKILLMANAGE							"uisettings/layouts/SkillManage.ls"
#define		UI_HELPCENTRE							"uisettings/layouts/HelpCentre.ls"
#define		UI_CLIENTHAND							"uisettings/layouts/ClientHand.ls"
#define		UI_SCENEMAP								"uisettings/layouts/SceneMap.ls"
#define		UI_TOPMESSAGE							"uisettings/layouts/TopMessage.ls"
#define		UI_EQUIPMENT							"uisettings/layouts/Equipment.ls"
#define		UI_LEFTRIGHTSKILLBAR					"uisettings/layouts/LeftRightSkillBar.ls"
#define		UI_PKFILTER								"uisettings/layouts/PKFilter.ls"
#define		UI_COMMSGBOX							"uisettings/layouts/ComMsgBox_Normal.ls"
#define		UI_ITEMBOX								"uisettings/layouts/ItemBox.ls"
#define		UI_DRAGITEM								"uisettings/layouts/DragItem.ls"
#define		UI_NPCMSG								"uisettings/layouts/NpcMsgBox.ls"
#define		UI_QUESTMANAGE							"uisettings/layouts/QuestManage.ls"
#define		UI_TRADEBOX								"uisettings/layouts/TradeBox.ls"
#define		UI_SPLITITEMBOX							"uisettings/layouts/SplitItemBox.ls"
#define		UI_POPMESSAGE							"uisettings/layouts/PopMessage.ls"
#define		UI_TRADECONFIRMBOX						"uisettings/layouts/TradeConfirmBox.ls"
#define		UI_SHOP									"uisettings/layouts/Shop.ls"
#define		UI_COMPOUND								"uisettings/layouts/Compound.ls"
#define		UI_ERROR_MESSAGE_BOX					"uisettings/layouts/ErrorMessage.ls"
#define		UI_ITEM_TIP								"uisettings/layouts/ItemTip.ls"
#define		UI_LINKED_ITEM_TIP						"uisettings/layouts/LinkedItemTip.ls"
#define		UI_STORE_BOX							"uisettings/layouts/StoreBox.ls"
#define		UI_DURA_ALERT							"uisettings/layouts/DuraAlert.ls"
#define		UI_SYSTEM_MESSAGE						"uisettings/layouts/SystemMessage.ls"
#define		UI_CHAT_INPUT							"uisettings/layouts/ChannelInput.ls"
#define		UI_VENDUE_WND							"uisettings/layouts/itemvendueshop.ls"	
#define		UI_CAST_BAR								"uisettings/layouts/CastBar.ls"		
#define		UI_TALISMAN								"uisettings/layouts/Talisman.ls"	
#define		UI_TARGET_EQUIPMENT						"uisettings/layouts/TargetEquipment.ls"	
#define		UI_PLAYER_MENU							"uisettings/layouts/RolePopMenu.ls"
#define		UI_LOG_MINIKEYBOARD						"uisettings/layouts/MiniKeyboard.ls"
#define     UI_HELPINFO								"uisettings/layouts/HelpF1.ls"
#define		UI_GAMESET_BOARD						"uisettings/layouts/GameSetting.ls"
#define		UI_PLAYER_MENU							"uisettings/layouts/RolePopMenu.ls"
#define		UI_PLAYER_MENU							"uisettings/layouts/RolePopMenu.ls"	
#define		UI_SMITH								"uisettings/layouts/Smith.ls"	
#define		UI_QUEST_TRACK							"uisettings/layouts/QuestTrack.ls"
#define		UI_AUTO_CONNECT							"uisettings/layouts/AutoConnect.ls"
#define		UI_STUDESKILL							"uisettings/layouts/skillstudy.ls"
#define		UI_WAITINGMSG							"uisettings/layouts/WaitingMsg.ls"
#define		UI_DELETE_COMFIRM						"uisettings/layouts/DelComfirm.ls"
#define		UI_LEVELUP								"uisettings/layouts/LevelUp.ls"
#define		UI_LEVELUP_INFO							"uisettings/layouts/LevelUpInfo.ls"
#define		UI_DELAYQUIT							"uisettings/layouts/DelayQuit.ls"
#define		UI_TARGET_MENU							"uisettings/layouts/TargeMenu.ls"
#define		UI_FIRSTLOGIN_HELP						"uisettings/layouts/UiBeginHelp.ls"
#define		UI_ROLEHEAD								"uisettings/layouts/RoleHead.ls"
#define		UI_CHANGEMAPWND							"uisettings/layouts/ChangeMapWnd.ls"
#define		UI_ROLEFACE_POPMENU						"uisettings/layouts/RoleFaceTeamPopMenu.ls"
#define		UI_ACCOUT_SET							"UserData/Account/"
#define		USERDATA_FOLDER							"\\UserData"
#define		USER_SCHEME_FOLDER						"\\Account"
							
//---------------------------- npc res 相关 ------------------------------
#define		CHAT_TEAM_INFO_FILE_NAME				"Team.cht"
#define		CHAT_CHANNEL_INFO_FILE_NAME				"Channel.cht"
#define		RES_INI_FILE_PATH						"\\settings\\npcres\\"
#define		RES_NPC_STYLE_PATH						"\\spr\\npcres\\style\\"
#define		RES_SOUND_FILE_PATH						"sound"
#define		NPC_RES_KIND_FILE_NAME					"\\settings\\npcres\\人物类型.txt"
#define		NPC_NORMAL_RES_FILE						"\\settings\\npcres\\普通npc资源.txt"
#define		NPC_NORMAL_SPRINFO_FILE					"\\settings\\npcres\\普通npc资源信息.txt"
#define		STATE_MAGIC_TABLE_NAME					"\\settings\\npcres\\状态图形对照表.txt"
#define		PLAYER_RES_SHADOW_FILE					"\\settings\\npcres\\主角动作阴影对应表.txt"
#define		NPC_RES_SHADOW_FILE						"普通npc动作阴影对应表.txt"
#define		PLAYER_SOUND_FILE						"\\settings\\npcres\\主角动作声音表.txt"
#define		NPC_SOUND_FILE							"\\settings\\npcres\\npc动作声音表.txt"
#define		NPC_ACTION_NAME							"npc动作表.txt"
#define		ACTION_FILE_NAME						"动作编号表.txt"
#define		PLAYER_MENU_STATE_RES_FILE				"\\settings\\npcres\\界面状态与图形对照表.txt"
#define		PLAYER_INSTANT_SPECIAL_FILE				"\\settings\\npcres\\瞬间特效.txt"
#define		SETTING_PATH							"\\settings"
#define		TABFILE_PATH							"\\settings\\item"
#define		FACTION_FILE							"\\settings\\faction\\门派设定.ini"
#define		defPK_PUNISH_FILE						"\\settings\\npc\\player\\PKPunish.txt"
#define		defPLAYER_TONG_PARAM_FILE				"\\settings\\tong\\TongSet.ini"
#define		CHAT_PATH								"\\chat"
#define		PLAYER_LEVEL_EXP_FILE					"\\settings\\npc\\player\\level_exp.txt"
#define		PLAYER_LEVEL_ADD_FILE					"\\settings\\npc\\player\\level_add.txt"
#define		PLAYER_LEVEL_LEAD_EXP_FILE				"\\settings\\npc\\player\\level_lead_exp.txt"
#define		BASE_ATTRIBUTE_FILE_NAME				"\\settings\\npc\\player\\NewPlayerBaseAttribute.ini"
#define		PLAYER_PK_RATE_FILE						"\\settings\\npc\\PKRate.ini"
#define		PLAYER_BASE_VALUE						"\\settings\\npc\\player\\BaseValue.ini"
#define		NEW_PLAYER_INI_FILE_NAME				"\\settings\\npc\\player\\NewPlayerIni%02d.ini"
#define		PLAYER_LEVEL_UP_FILE					"\\settings\\player\\levelup_%d_%d.txt"
#define		BUYSELL_FILE							"\\settings\\buysell.txt"
#define		SMITH_SHOP_FILE							"\\settings\\smithshop.txt"
#define		GOODS_FILE								"\\settings\\goods.txt"
#define		STALL_LEVEL_FILE						"\\settings\\stalllevel.ini"
#define		CHANGERES_MELEE_FILE					"\\settings\\item\\meleeres.txt"
#define		CHANGERES_PENDANT_FILE					"\\settings\\item\\pendantres.txt"
#define		CHANGERES_RANGE_FILE					"\\settings\\item\\rangeres.txt"
#define		CHANGERES_ARMOR_FILE					"\\settings\\item\\armorres.txt"
#define		CHANGERES_HELM_FILE						"\\settings\\item\\helmres.txt"
#define		CHANGERES_HORSE_FILE					"\\settings\\item\\horseres.txt"
#define		ARMOR_SET_TABLE_FILE					"/settings/item/armorset.txt"
#define		YAO_TABLE_FILE							"/settings/item/yao.txt"
#define		YAO_SET_TABLE_FILE						"/settings/item/yaoset.txt"
#define		YAO_ADD_ON_TABLE_FILE					"/settings/item/yaoaddon.txt"
#define		INLAY_RULE_FILE							"/settings/item/inlayrule.txt"
#define		INLAY_ADD_ON_TABLE_FILE					"/settings/item/inlayaddon.txt"
#define		ITEM_ABRADE_FILE						"/settings/item/abrade.ini"
#define		COMMON_CONFIG_FILE						"/settings/commonconfig.ini"
#define		COMMON_STYLE_CONFIG_FILE				"/settings/commonstyleconfig.ini"
#define		RELATION_TEMPLATE_CFG_FILE				"/settings/socialrelation/relation.ini"
#define		TALISMAN_ENCHASE_TABLE_FILE				"/settings/item/enchase.txt"
#define		EXP_DISTRIBUTE_TABLE_FILE				"/settings/expdistribute.txt"
#define		CLIENT_TALISMAN_NPC_TABLE_FILE			"/settings/talisman_npc.txt"
#define		EMPLOY_CONFIG_FILE						"/settings/employ.ini"
#define		EMPLOY_LEVEL_EXP_CONFIG_FILE			"/settings/employ_exp.txt"
#define		PK_ADDED_VALUE_CFG_FILE					"/settings/PKValueSettings.ini"
//--------------- npc skill missles 设定文件，用于生成模板 ---------------
#define		SKILL_SETTING_FILE						"\\settings\\skills.txt"
#define		MISSLES_SETTING_FILE					"\\settings\\missles.txt"
#define		NPC_SETTING_FILE						"\\settings\\npcs.txt"
#define		MAP_INFO_FILE							"\\settings\\mapinfo.txt"
#define		NPC_GOLD_TEMPLATE_FILE					"\\settings\\npc\\NpcGoldTemplate.txt"
#define		FILENAME_SKILLSSETTING					"\\settings\\skills.ini"
#define		FILENAME_PLAYERSKILLS					"\\settings\\playerskills.ini"
#define		FILENAME_ITEMVERSION					"\\settings\\item\\itemversion.ini"
#define		FILENAME_BASENUMCOE						"\\settings\\npc\\player\\basenumcalc.ini"
#define		FILENAME_COMBINEATTR					"\\settings\\skillevent.txt"
#define		FILE_SKILLRELATION						"\\settings\\skillrelation.txt"
#define		FILE_MAINSKILL_VALUE					"\\settings\\valueskills.txt"
#define		FILE_SKILL_CHGCOND						"\\settings\\skillcond.txt"
//---------------------------- object 相关 -------------------------------
#define		OBJ_DATA_FILE_NAME						"\\settings\\obj\\ObjData.txt"
#define		MONEY_OBJ_FILE_NAME						"\\settings\\obj\\MoneyObj.txt"
//----------------------------- 声音相关 ---------------------------------
#define		defINSTANT_SOUND_FILE					"\\settings\\SoundList.txt"
#define		defMUSIC_SET_FILE						"\\settings\\music\\MusicSet.txt"
#define		defMUSIC_FIGHT_SET_FILE					"\\settings\\music\\MusicFightSet.ini"
//------------------------------------------------------------------------
#define		NPC_LEVELSCRIPT_FILENAME				"\\script\\npclevelscript\\npclevelscript.lua"
#define		NPC_TEMPLATE_BINFILEPATH				"\\settings"
#define		NPC_TEMPLATE_BINFILE					"NpcTemplate.Bin"
#define		WEAPON_PHYSICSSKILLFILE					"\\settings\\武器物理攻击对照表.txt"
//------------------------------------------------------------------------
#define	WORLD_WAYPOINT_TABFILE						"\\settings\\WayPoint.txt"
#define WORLD_STATION_TABFILE						"\\settings\\Station.txt"
#define WORLD_STATIONPRICE_TABFILE					"\\settings\\StationPrice.txt"
#define WORLD_WAYPOINTPRICE_TABFILE					"\\settings\\WayPointPrice.txt"
#define WORLD_DOCK_TABFILE							"\\settings\\Wharf.txt"
#define WORLD_DOCKPRICE_TABFILE						"\\settings\\WharfPrice.txt"
#define STRINGRESOURSE_TABFILE						"\\settings\\StringResource.txt"
#define PLAYER_RANK_SETTING_TABFILE					"\\settings\\RankSetting.txt"	
#define QUESTITEM_TABFILE							"questkey.txt"
//---------------------------- 其它 --------------------------------------
#define TEXT_FILTER_EXP_FILE						"\\settings\\chatsent.flt"
#define CHAT_TEXT_FILTER_FILE                       "\\settings\\msgchatsent.flt"
#define USER_CHAT_TEXT_FILTER_FILE                  "\\UserData\\chatrecv.flt"
#define CHAT_RECV_TEXT_FILTER_FILE                  "\\settings\\msgchatrecv.flt"
#define TEAM_ICON_FILE								"\\settings\\teamicon.txt"
#define EQUIPMENT_WOMAN_POSTFIX_SMALL				"_woman_small"
#define EQUIPMENT_MAN_POSTFIX_SMALL					"_man_small"
#define ENYMY_IS_COMMING                            "您的仇人%s正在附近"
#define ENYMY_IS_AWAY                               "您的仇人%s已经远离了"
#define FORBID_CHAT_IN_UNIT                         "禁言"
#define CONFIRM_CHAT_IN_UNIT                        "解禁"
#define PUB_ANUCMENT_TOO_LONG                       "公告过长"
#endif
















