//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/03/2006 17:17
//      File_base        : CoreShell
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 外界（如界面系统）通过此接口从Core获取游戏世界数据。
// 
// 			包含伍个接口函数CoreOperationRequest、CoreGetGameData、CoreDrawGameObj、CoreProcessInput
// 		与CoreSetCallDataChangedNofify。
// 			CoreOperationRequest用于发送对游戏的操作请求。参数uOper为操作的id，其值为梅举类型
// 		GAMEOPERATION_INDEX的取值之一。参数uParam以及nParam的具体含义依据uOper的取值情况而定。
// 		如果成功发送操作请求，函数返回非0值，否则返回0值。这些请求都要求Core立即接受，Core的
// 		客户不保证通过此函数发送的数据在函数调用之后依然有效。
// 			获知游戏数据有两种方式，一种是调用接口函数CoreGetGameData主动获取，另外一种是注册
// 		通知函数，当游戏数据变更的时候，被注册的通知函数就会被调用，不一定在调用通知函数的同时
// 		传递改变的游戏数据。并且两种方式所处里的数据项范围并不相同。
// 			接口函数CoreGetGameData参数uDataId表示获取游戏数据的数据项内容索引，其值为梅举类型
// 		GAMEDATA_INDEX的取值之一。参数uParam、nParam以及函数返回值的具体含义依据uDataId的取值
// 		情况而定。
// 			注册通知函数的接口方法为CoreSetCallDataChangedNofify。参数pNotifyFunc为通知函数的
// 		指针。返回值为非0值表示注册成功，否则表示失败。传入参数pNotifyFunc的值入为0，则已经注
// 		册的通知函数将被取消。通过通知函数通知发生游戏数据改变的时候，不一定同时通过通知函数
// 		传送数据改变。设计原则上是如果改变的数据内容少，可以方便简单地通过通知函数的参数传递的，
// 		则随通知函数传递；否则只是发送通知而已，并不传送改变的游戏的
// 
// 			CoreDrawGameObj用于绘制单个游戏对象。参数uObjGenre指出对象的类属，uId指出对象的id，
// 		x、y指出绘制范围的左上角坐标，Width、Heightn指出了绘制范围的大小，Param用于额外的参数传
// 		递，其含义将依赖于具体要绘制的对象类型。
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef CORESHELL_H
#define CORESHELL_H

#include "GameDataDef.h"



//=========================================================
// Core外部客户向core获取游戏数据的数据项内容索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
enum GAMEDATA_INDEX
{
	GDI_MY_ITEM_LAYOUT_DESC = 1,
	GDI_MY_GUA_LAYOUT_DESC,
	GDI_OPPOSITE_ITEM_LAYOUT_DESC,
	GDI_OPPOSITE_GUA_LAYOUT_DESC,
	GDI_INSIDE_BALL_LAYOUT_DESC,
	GDI_LINKED_ITEM_LAYOUT_DESC,
	GDI_VENDUE_ITEM_LAYOUT_DESC,
	GDI_SHOP_ITEM_LAYOUT_DESC,

	//得到UI上各个位置的物品对应的装备位置
	GDI_MY_ITEM_EQUIP_POS,
	GDI_OPPOSITE_EQUIP_POS,
	GDI_LINKED_EQUIP_POS,
	GDI_VENDUE_EQUIP_POS,
	GDI_SHOP_EQUIP_POS,

	//得到某个装备位置上的物品的Index
	GDI_EQUIP_INDEX_BY_EQUIP_POS,

	//得到装备比较界面的标题排版数据
	GDI_EQUIP_COMPARE_TITLE_LAYOUT_DATA,
	//得到修理价格排版数据
	GDI_ITEM_PRICE_LAYOUT_DATA,

	GDI_ITEM_CAN_TRADE,

	GDI_GET_ITEM_QUALITY_BY_TYPE,
	
	GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO,	//游戏对象描述说明文本串(包含交易相关信息)
	//参数含义同GDI_GAME_OBJ_DESC

	GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO,	//游戏对象描述说明文本串(包含修理相关信息)
	//参数含义同GDI_GAME_OBJ_DESC

	GDI_GAME_OBJ_DESC_INCLUDE_STALLINFO,	//游戏对象描述说明文本串(包含摆摊标价相关信息)
	//参数含义同GDI_GAME_OBJ_DESC
	
	GDI_GET_ITEM_ID_BY_INDEX,				//通过INDEX查询物品ID

	GDI_GET_ITEM_INDEX_BY_ID,				//通过id查询物品index

	GDI_GAME_OBJ_LIGHT_PROP,	//对象的光源属性数据
	//uParam = (KUiGameObject*) 描述游戏对象的结构数据的指针
	//nParam = to be def

	GDI_PLAYER_BASE_INFO,		//主角的一些不易变的数据
	//uParam = (KUiPlayerBaseInfo*)pInfo

	GDI_PLAYER_RT_INFO,			//主角的一些易变的数据
	//uParam = (KUiPlayerRuntimeInfo*)pInfo

	GDI_PLAYER_RT_ATTRIBUTE,	//主角的一些易变的属性数据
	//uParam = (KUiPlayerAttribute*)pInfo

	GDI_GET_CUR_SPECIALSKILL_ID,

	/************************************************************************/
	/*							Target system                               */
	/************************************************************************/
	/*!
	\brief

	\param 
	
	\return
		
	*/
	GDI_GET_MY_NAME,

	GDI_SCREEN_POS_OF_MAP,

	GDI_PLAYER_TARGET_INFO,

	GDI_GET_PLAYER_INFO_BY_NPCID,
	/************************************************************************/
	/*							Item system                                 */
	/************************************************************************/
	//-->Rocker 2005/03/22  得到显示法则信息和需求金钱信息
	GDI_GET_COMPOUND_INFO,
	// uParam = (KUiBesetOperationParam*)uParam
	// nParam = 传入并返回(KUiBesetInfo*)nParam 信息
	// Return = 
	//<--Rocker 
	GDI_GET_SMITH_RULE_BY_ID,
	
	GDI_GET_SMITH_RULE_BY_LIFT_ITEM,//通过解禁物品得到打造规则
	/************************************************************************/
	/*								Item system                             */
	/************************************************************************/
	// uParam = (KItemInfo*)uParam
	// nParam = Item ID
	// Return = 
	GDI_ITEM_INFO_ID,

	GDI_GET_ITEM_TYPE_BY_ID,

	GDI_GET_ITEM_TYPE_BY_INDEX,

	GDI_GET_ITEM_COUNT_BY_ID,

	GDI_GET_ITEM_NAME_BY_ID,

	GDI_GET_ITEM_NAME_BY_INDEX,

	GDI_GET_ITEM_NAME_WITH_COLOR_BY_INDEX,

	GDI_GET_ITEM_BASIC_INFO_BY_TYPE,

	GDI_GET_ITEM_SELL_PRICE_BY_ID,

	GDI_GET_ITEM_DURA_BY_INDEX,

	GDI_GET_ITEM_NAME_WITH_COLOR_BY_INDEXPARAM,

	GDI_GET_MAP_NAME,
	
	GDI_INSIDE_BALL_IMAGE_INFO,
	// uParam = (KItemInfo*)uParam
	// nParam = Item ID
	// Return = 
	GDI_ITEM_INFO_INDEX,

	//uParam = (KTalisman*)
	// nParam = Item Id
	GDI_TALISMAN_INFO_ID,

	// uParam = (KItemInfo*)uParam
	// nParam = Item ID
	// Return = 
	GDI_GUA_INFO_INDEX,
	// uParam = (KItemInfo*)uParam
	// nParam = Item index in shop
	// Return = 
	GDI_TARGET_GUA_INFO_INDEX,
	// uParam = (KItemInfo*)uParam
	// nParam = Item index in shop
	// Return = 
	GDI_ITEM_INFO_SHOP,
	// uParam = (FIND_ITEMINDEX_PARAM*)uParam
	// nParam = (KItemInfo*)
	// Return = 
	GDI_ITEM_INFO_PARTICULAR,

	GDI_PLAYER_HOLD_MONEY,		//主角随身携带的钱
	//nRet = 主角随身携带的钱

	GDI_PLAYER_JINSHANBI,
	//nRet = 主角随身携带的金山币

	GDI_PLAYER_MONEY_INFO,		//主角身上的2种货币信息
	// uParam = jinshanbi(0)/creditpoint(1)/point(2) 金山币可能丢符号，建议用GDI_PLAYER_JINSHANBI

	GDI_PLAYER_CREDIT_INFO,		//主角身上的各种信用信息
	// uParam = (PlayerCreditInfo*)uParam

	GDI_MAX_BUY_LIMIT_BY_MONEY_TYPE,	// 单次交易最大货币数量
	// uParam = jinshanbi(0)/creditpoint(1)/point(2)
	// nRet = 单次交易最大货币数量

	GDI_CREDIT_TO_TICKET_RATE,
	// nRet = 信用点与代金券比率

	GDI_CREDIT_LEVEL_LIMIT,
	// nRet = 可以使用信用消费的最低等级

	GDI_PLAYER_PRESENT_TICKET_COUNT,
	//Return = PRESENT_TICKET个数

	GDI_BUY_IBITEM_MONEY_TYPE,
	//uParam = FIND_ITEMINDEX_PARAM* pItemIdx
	//nRet	= ITEM_IB_BUY_TYPE

	GDI_GET_IBITEM_BIG_IMAGE,
	//uParam = FIND_ITEMINDEX_PARAM* pItemIdx
	//nParam = (KItemInfo*)

	GDI_IS_PLAYER_IN_COMBAT_WORLD,
	//nRet = true在战场/false不在战场
	
	GDI_PLAYER_STORE_MONEY,		//主角存的钱
	//nRet = 主角存的钱

	GDI_PLAYER_WEIGHT_CURRENT,	// 主角当前负重参数
	// nRet = 获取是否成功
	// uParam = int* 返回主角当前携带重量
	// nParam = int* 返回主角最大携带重量

	GDI_PLAYER_IS_MALE,			//主角是否男性
	//nRet = (int)(bool)bMale	是否男性

	//Lucifer~yu[zhangjianyu] [02/08/2006] Add for 
	//begin------------------------------------------------------------------------
	//主角是否上马动作
	GDI_PLAYER_ISPRERIDE,
	//nRet = (bool)bIsPreRide是否正在进行上马动作
	//0 - 没有
	//1 - 正在
	//end--------------------------------------------------------------------------	
	GDI_ITEM_COUNT_QUERY,		//----->Add by [Ray] 2004-4-2
	//uParam = id (物品的索引) 查询Item的重叠个数
	//Return = 个数
	GDI_ITEM_TAKEN_WITH,		//主角随身携带的物品等
	//uParam = (KUiObjAtRegion*) pInfo -> KUiObjAtRegion结构数组的指针，KUiObjAtRegion
	//				结构用于存储物品的数据及其放置区域位置信息。
	//nParam = pInfo数组中包含KUiObjAtRegion结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pInfo数组中的前多少个KUiObjAtRegion
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiObjAtRegion结构的数组
	//			才够存储全部的随身携带的物品信息。

	GDI_BAG_EXTEND,		//扩展槽位信息
	GDI_STORE_EXTEND,
	//uParam = (KUiObjAtRegion*) 
	//ret = count
	GDI_ITEM_IN_STORE_BOX,		//储物箱里的物品
	//参数及返回值含义同GDI_ITEM_TAKEN_WITH的

	GDI_EQUIPMENT,				//主角装备物品
	//uParam = (KUiObjAtRegion*)pInfo -> 包含11个元素的KUiObjAtRegion结构数组指针，
	//				KUiObjAtRegion结构用于存储装备的数据和放置位置信息。
	//			KUiObjAtRegion::Region::h = 0
	//			KUiObjAtRegion::Region::v 表示属于哪个位置的装备,其值为梅举类型
	//			UI_EQUIPMENT_POSITION的取值之一。请参看UI_EQUIPMENT_POSITION的注释。
	//Return =  其值表示pInfo数组中的前多少个KUiObjAtRegion结构被填充了有效的数据。

	GDI_TALISMAN_SELF,				//根据物品index查法宝信息
	//uParam =	TALISMAN_INFO
	//nParam =  Index

	GDI_SKILLEXP_TO_TAILSMAN_EXP,   //获取技能经验到法宝经验的转换比例

	GDI_IS_TALISMAN,
	//uParam =  itemId

	GDI_IS_INSIDE_BALL,
	//uParam =  itemId

	GDI_SELF_GUA_OVERVIEW,				//察看本人的挂位信息
	//			uParam = (KUiObjAtRegion*)pInfo -> 包含11个元素的KUiObjAtRegion结构数组指针，
	//			KUiObjAtRegion结构用于存储装备的数据和放置位置信息。
	//			KUiObjAtRegion::Region::v 表示属于哪个位置的挂位,其值为梅举类型
	//Return =  其值表示pInfo数组中的前多少个KUiObjAtRegion结构被填充了有效的数据。

	GDI_TARGET_GUA_OVERVIEW,				//察看目标的挂位信息
	//			uParam = (KUiObjAtRegion*)pInfo -> 包含11个元素的KUiObjAtRegion结构数组指针，
	//			KUiObjAtRegion结构用于存储装备的数据和放置位置信息。
	//			KUiObjAtRegion::Region::v 表示属于哪个位置的挂位,其值为梅举类型
	//Return =  其值表示pInfo数组中的前多少个KUiObjAtRegion结构被填充了有效的数据。

	GDI_TRADE_NPC_ITEM,			//npc列出来交易的物品
	//uParam = (KUiObjAtContRegion*) pInfo -> KUiObjAtContRegion结构数组的指针，KUiObjAtContRegion
	//				结构用于存储物品的数据及其放置区域位置信息。
	//				其中KUiObjAtContRegion::nContainer值表示第几页的物品
	//nParam = pInfo数组中包含KUiObjAtContRegion结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pInfo数组中的前多少个KUiObjAtContRegion
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiObjAtContRegion结构的数组
	//			才够存储全部的npc列出来交易的物品信息。

	GDI_SHOP_ITEM_PRICE,

	GDI_SHOP_ITEM_SELL_PRICE,

	GDI_SHOP_ITEM_NAME,
	
	GDI_REPAIR_ITEM_PRICE,		//修理物品的价格

	GDI_REPAIR_ALL_ITEM_PRICE,		//修理物品的价格
	//uParam = (KUiObjAtContRegion*) pItemInfo -> 用于指出是哪处的哪个物品
	//nParam = (bool) -> 是否特修
	//Return = (int)修理价格
	
	GDI_STALL_ITEM_PRICE,		//将用于摆摊物品的标价
	//uParam = (KUiObjAtContRegion*) pItemInfo -> 用于指出是哪处的哪个物品
	//nParam = (KUiItemBuySelInfo*) pPriceInfo -> 用于接收物品名称修理费用等信息
	//Return = (int)(bool) 是否标价

	/************************************************************************/
	/*							Buffer system                               */
	/************************************************************************/
	/*!
	\brief
		Get buffer Information. 		

 	\param 
		Buffer Information(KBufferInfo*). 

	\param 
		Buffer index(int).
	
	\return
		
	*/
	GDI_GET_BUFFER_INFO,

	/*!
	\brief
		Get buffer Information. 		

 	\param 
		Buffer Information(KBufferSyncInfo*). 

	\param 
		Buffer max count(int).
	
	\return
		Buffer count(int).
		
	*/
	GDI_GET_TARGET_BUFFER_INFO,
	
	/************************************************************************/
	/*							Quest system                                */
	/************************************************************************/
	/*!
	\brief
		Get quest list.

	\param 
		Quest id (int).	

	\param 
		Quest information (KQuestInfo*).

	\return
		Nothing.
	*/
	GDI_GET_QUEST_LIST,	

	/*!
	\brief
		Get quest list.

	\param 
		Quest id (int).	

	\param 
		Quest information (KQuestInfo*).

	\return
		Nothing.
	*/
	GDI_GET_QUEST_INFO,	

	GDI_GET_NPC_POS_BY_TABLE_INDEX,	
	/************************************************************************/
	/*							Skill system                                */
	/************************************************************************/
	/*!
	\brief
		Get player immediacy skill list.

	\Param
		Immediacy skill list;

	\return
		Nothing;
	*/
	GDI_PLAYER_IMMED_ITEMSKILL,

	/*!
	\brief
		Get left enable skills.

	\Param
		Immediacy skill list;

	\return
		Nothing;
	*/
	GDI_LEFT_ENABLE_SKILLS,

	/*!
	\brief
		Get right enable skills.

	\Param
		Immediacy skill list;

	\return
		Nothing;
	*/
	GDI_RIGHT_ENABLE_SKILLS,	

	/*!
	\brief
		Request the kind of skill list.
	
	\return
		Skill points.	
	*/
	GDI_SKILL_POINT,

	/*!
	\brief
		Request the kind of skill list.
	
	\return
		Nothing.	
	*/
	GDI_SKILL_KIND_LIST,

	/*!
	\brief
		Request skill list.
	
	\return
		Nothing.	
	*/
	GDI_SKILL_LIST,

	/*!
	\brief
		Request skill information.
	
	\return
		Nothing.	
	*/
	GDI_SKILL_INFO,
	GDI_GET_CUR_SKILL_ID,

	GDI_GET_CUR_SKILL_POINT,//得到当前蕴魂
    GDI_GET_IS_STUDY_ABLE,  //是否可以学习
	
	/************************************************************************/
	/*                            mail system                               */
	/************************************************************************/
	GDI_GET_NEW_MAIL_COUNT,

	GDI_GET_SEND_MAIL_TAX,

	GDI_GET_MAX_MAILCOUNT,
	// nRet = MAX MailCount
	
	/************************************************************************/
	/*                            auction system                            */
	/************************************************************************/
	GDI_GET_AUCTION_BASE_INFO,

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	GDI_NEARBY_PLAYER_LIST,		//获取周围玩家的列表
	//uParam = (KUiPlayerItem*)pList -> 人员信息列表
	//			KUiPlayerItem::nData = 0
	//nParam = pList数组中包含KUiPlayerItem结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pList数组中的前多少个KUiPlayerItem
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiPlayerItem结构的数组
	//			才够存储全部人员信息。

	GDI_NEARBY_IDLE_PLAYER_LIST,//获取周围孤单可受邀请的玩家的列表
	//参数含义同GDI_NEARBY_PLAYER_LIST

	GDI_NEARBY_NOT_FRIEND_LIST,//获取周围非好友的玩家列表
	//参数含义同GDI_NEARBY_PLAYER_LIST

	GDI_PLAYER_LEADERSHIP,		//主角统帅能力相关的数据
	//uParam = (KUiPlayerLeaderShip*) -> 主角统帅能力相关的数据结构指针

	GDI_ITEM_IN_ENVIRO_PROP,	//获得物品在某个环境位置的属性状态
	//uParam = (KUiGameObject*)pObj（当nParam==0时）物品的信息
	//uParam = (KUiObjAtContRegion*)pObj（当nParam!=0时）物品的信息
	//			此时KUiObjAtContRegion::Region的数据固定为0，无意义。
	//nParam = (int)(bool)bJustTry  是否只是尝试放置
	//Return = (ITEM_IN_ENVIRO_PROP)eProp 物品的属性状态
	
	/************************************************************************/
	/*                              聊天相关                                */
	/************************************************************************/
	GDI_CHAT_GROUP_INFO,			//聊天的好友分组信息
	//从nParam给定的索引开始查找第一个有效的分组，返回该分组的信息与分组索引。
	//uParam = (KUiChatGroupInfo*) pGroupInfo 分组信息
	//nParam = nIndex 欲获取的分组的索引
	//Return = 实际返回数据的分组的索引，如果未获得则返回-1
	GDI_CHAT_UPDATE_PK_VALUE,
	GDI_CHAT_IS_OWNER,
	//uParam = RoomId
	//return weather the client is the room owner

	GDI_CHAT_FRIENDS_IN_A_GROUP,		//聊天一个好友分组中好友的信息
	//uParam = (KUiPlayerItem*)pList -> 人员信息列表
	//			KUiPlayerItem::nData = (CHAT_STATUS)eFriendStatus 好友的当前状态
	//nParam = 要获取列表的好友分组的索引
	//Return = 其值表示pList数组中的前多少个KUiPlayerItem结构被填充了有效的数据.

	GDI_GET_RECENTLIST,	//取得最近联系人列表
	// nParam: pBuff (最近联系人人名列表)
	// nRet = nSize (最近联系人个数)

	GDI_CHAT_BUBBLE,				// 聊天气泡是否显示
	//uParam = bool 是否显示聊天气泡
	//nParam = nId  NPC的ID

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	GDI_PK_SETTING,					//获取pk设置
	//Return = (int)(bool)bEnable	是否允许pk

	GDI_SHOW_PLAYERS_NAME,			//获取显示各玩家人名
	//Return = (int)(bool)bShow	是否显示
	//-------> Ray [Luoliang] 2004-9-2
	GDI_SHOW_ITEMS_NAME,			//获取显示物品名
	//Return = (int)(bool)bShow	是否显示
	//<------- End [Ray]
	GDI_SHOW_PLAYERS_LIFE,			//获取显示各玩家生命
	//Return = (int)(bool)bShow	是否显示
	GDI_SHOW_PLAYERS_MANA,			//获取显示各玩家内力
	//Return = (int)(bool)bShow	是否显示

	GDI_PARADE_EQUIPMENT,				//看玩家装备物品,消息含义同GDI_EQUIPMENT
	
	GDI_TARGET_ROLE_INFO,				//察看玩家装备时显示的玩家其他信息
	
	GDI_IMMEDIATEITEM_NUM,				//取得快捷物品个数
	//uParam = nIndex（0，1，2, 3）
	
	GDI_NPC_OVERVIEW,					//取得某个NPC的浏览数据
										//uParam = Npc的Index
										//nParam = (KUiPlayerRuntimeInfo *)pReturn
	//////////////////////////////////////////////////////////////////////////
	// 	flying defined these folllowing items....
	// this item to find out whether player is in stall status
	GDI_IS_STALL, //Return = (int)(bool)bEnable	
	// uParam: -1: self; other: a specified NPC
	// and whether a player can be set into stall status..
	GDI_ITEM_IN_STALL,  // like GDI_GET_ITEM_TAKEN
	//主角随身携带的物品中插上标签准备摆摊的
	//uParam = (KUiObjAtRegion*) pInfo -> KUiObjAtRegion结构数组的指针，KUiObjAtRegion
	//				结构用于存储物品的数据及其放置区域位置信息。
	//nParam = PlayerID
	//Return = 如果返回值小于等于传入参数nParam，其值表示pInfo数组中的前多少个KUiObjAtRegion
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiObjAtRegion结构的数组
	//			才够存储全部的随身携带的物品信息。
	GDI_ITEM_IN_STALL_OTHER,	//选中的当前摊位中的物品（非主角）
	//uParam = (StallViewItemInfo*) pInfo -> StallViewItemInfo结构数组的指针，StallViewItemInfo
	//				结构用于存储物品的数据、价格及其放置区域位置信息。
	//nParam = pInfo数组中包含StallViewItemInfo结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pInfo数组中的前多少个StallViewItemInfo
	//			结构被填充了有效的数据；否则表示需要传入包含多少个StallViewItemInfo结构的数组
	//			才够存储全部的随身携带的物品信息。
	GDI_STALL_OWNER_NAME,		//当前摊主名字
	GDI_STALL_OWNER_ADV,           //得到自己摆摊输入广告

	GDI_CREATURE_STATUS,
// lixuewu 2004.03.18 取得召唤

// Add by Cooler 2004-7-21
// Begin -->
	GDI_ROLE_TYPE,	

// End <--
//-------> Ray [Luoliang] 2004-11-25
	GDI_NPC_CAREER,
	//uParam
	//nParam = nIndex
	//nRet = series
//<------- End [Ray]

//----->Add by [Ray] 2004-7-28
	GDI_GAMBLE_OPER_DATA,		//交易操作相关的数据
	//uParam = (UI_GAMBLE_OPER_DATA)eOper 具体含义见UI_TRADE_OPER_DATA
	//nParam 具体应用与含义由uParam的取值状况决定,见UI_TRADE_OPER_DATA的说明
	//Return 具体含义由uParam的取值状况决定,见UI_TRADE_OPER_DATA的说明
//<-----Add End
//-------> Ray [Luoliang] 2004-9-17
	
	GDI_GET_PLAYER_CAMP,			// 是什么阵营

	GDI_ITEM_HAS_SPECIAL_IMAGE,	//Item是否有特殊的图片
	
	GDI_GET_PK_INFO,
	// uParam = (const char*)uParam 返回当前PK值对应的字符串
	// nParam = (int*)nParam 返回当前PK值
	//<-- End
	//-------> Ray [Luoliang] 2005-6-16
	GDI_GET_STALL_LEVEL_INFO,			//得到摆摊等级的相关信息//
	GDI_IS_OWN_PET,						//是否是自己的宠物
	GDI_IS_EMPLOEE,						//是否是佣兵
	//uParam = 
	//nParam = nNpcIndex
	//nRet = BOOL
	//<---- Add By Ray [Luoliang] [2005-9-20]
	GDI_GET_TECHNOLOGY_INFO,			//得到指定的科技的信息
	//uParam = (tagTechNodeIdx*)pNodeIdx
	//nParam = (tagUITechNode*)pInfo
	//nRet = BOOL
	// End. Ray [LuoLiang] [2005-9-20] ---->	
	//<------- End [Ray]

	// <Add name="Adt.X" time="2005/07/27">
	GDI_PLAYER_HOLD_COWRIE,				// 得到玩家身上铜贝数
	//nRet = 主角随身携带的钱
	
	GDI_GET_SALARY_REDEEM_RATE,			// 得到城市工资兑换比率
	// nRet = 城市工资兑换比率
	GDI_GET_SALARY_REDEEM_ALERT,		// 得到城市金库兑换警戒线
	// nRet = 城市金库兑换警戒线

	GDI_SALARY_CAN_REDEEM,
	// uParam = 
	// nParam = 玩家要兑换的铜贝数
	// nRet == TRUE 能兑换, nRet == FALSE 不能兑换 

	GDI_SALARY_REDEEM,					// 需要用GDI_SALARY_CAN_REDEEM先测试能否兑换铜贝
	// uParam = 玩家要兑换的铜贝数
	// nParam = 
	// nRet=
	
	GDI_SET_SALARY_REDEEM_PARAM,		// 国王设置工资兑换参数
	// uParam = SalaryRedeemParam*
	// nParam = 
	// nRet = 
	GDI_GET_REDEEM_STATE,
	// nRet == TRUE 兑换状态开启	
	
	GDI_ADJUST_GOODS_PRICE,
	// uParam = Goods price,
	// nParam = Goods index,	
	// nRet == TRUE means successful.
	// </Add>
	
	// <Add name="Adt.X" time="2005/11/22">
	GDI_REVIVE_CRYSTAL_TIME,
	// uParam = 0,
	// nParam = 0,
	// nRet = 重生水晶时间值
	// </Add>	

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
    // add by hejianfeng for Anti-Wallow.  2005-11-21
	GDI_ITEM_COUNT,
	//uParam = 待查询Item的参数
	//Return = 物品的索引
    // endadd

	//Lucien[liusiliang] [04/07/2007] 得到第一个物品的INDEX
	//begin------------------------------------------------------------------------
	GDI_FIRST_ITEM_ID,
	//uParam = 待查询Item的参数
	//Return = 物品的ID
	//end--------------------------------------------------------------------------	

	//Lucifer~yu[zhangjianyu] [03/03/2006] Add for 取得离线经验单药加成时间 
	//begin------------------------------------------------------------------------
	GDI_GET_PILLMAKING_LEFTTIME,
	//nRet = (int)nLeftTime
	//end--------------------------------------------------------------------------	

	//Lucifer~yu[zhangjianyu] [02/22/2006] Add for new fengshen
	//begin------------------------------------------------------------------------
	GDI_GET_PILLPARAM,
	//uParam = (KPillParam*)
	//nRet = BOOL	
	//end--------------------------------------------------------------------------	
	//Lucifer~yu[zhangjianyu] [03/23/2006] Add for 客户端取得Item详细信息
	//begin------------------------------------------------------------------------
	GDI_GET_ITEM_PARTICULAR,
	//uParam = (FIND_ITEMINDEX_PARAM*) 
	//nParam = nItemId
	//nRet   = 无意义
	//end--------------------------------------------------------------------------	

	/************************************************************************/
	/*						Tong system                                     */
	/************************************************************************/
	GDI_GET_SOCIETY,
	/*!
	\brief
		Get player society information.
	
	\param 
		(PlayerSociety*)uParam
  
	\return
		
	*/
	GDI_GET_SOCIETY_PLAYER,
	
	/*!
	\brief
		Judge player society right.	
	
	\param 
		(int)uParam		
  
	\return
		
	*/
	GDI_GET_SOCIETY_RIGHT,

	/*!
	\brief
		Get society template information.
	
	\param 
		(SocietyInfoIndex*)uParam		
  
	\param 
		(SocietyTemplateInfo*)nParam	
  
	\return
		
	*/
	GDI_GET_SOCIETY_TEMPLATE_INFO,

	/*!
	\brief
		Get society layer information.
	
	\param 
		(SocietyInfoIndex*)uParam		
  
	\param 
		(SocietyLayerInfo*)nParam	
  
	\return
		
	*/
	GDI_GET_SOCIETY_LAYER_INFO,

	GDI_CAN_SMITH,

	GDI_GET_SMITH_RULE_LIST,

	GDI_CAN_JOIN_SOCIAL_LAYER,
	/******************************************************************
	 *                    TaisuiWheel Sys                             *
	 ******************************************************************/
	 //Add By Brianyao 2007
 	GDI_GET_CUR_TIAN_XIANG,
	GDI_GET_ACTIVATING_TIAN_XIANG,
	GDI_GET_WHEEL_TIAN_GAN,
	GDI_GET_WHEEL_DI_ZHI,
	GDI_GET_TIMES_INFO,
	GDI_GET_CLIENT_STATE,
	//end


	/*!
	\brief
	 uParam= (KSkillId *)
	 return bool
	*/
	GDI_CANLEARN_SKILL,         //判断Skill是否可以学习
	GDI_STUDY_SKILL_COND,       //uParam=skillid nParam= char * nRed==if can updateto
	GDI_MAP_ID,                 //Get Player Map ID
	GDI_MAP_CHANNEL_INFO,		//地图频道信息 uParam = MapChannelInfo *的一个指针 nParam = 0
	GDI_MAP_CHANNEL_NAME,		//地图频道名称 uParam = char *	nParam = char * 长度
	GDI_MAP_CHANNEL_TIME,		//地图频道说话间隔
	GDI_CAN_GOTO_POS,
	GDI_NPC_LIST_OF_MAP,
	GDI_GET_NPC_ID_BY_NAME,

	//得到player的雇佣时间
	GDI_GET_EMPLOY_TIME,

	GDI_GET_NEARBY_PLAYER,
	GDI_GET_PLAYER_POS,
	GDI_GET_NPC_KIND,
	GDI_IS_GM,

	//经验保险
	GDI_EXP_INSURANCE_STATE,
	GDI_EXP_INSURANCE_VALUE,
	GDI_EXP_INSURANCE_VALUE_MAX,
	GDI_EXP_INSURANCE_ENABLE_LEVEL,

	//任务保险
	GDI_QUEST_INSURANCE_STATE,
	GDI_QUEST_INSURNACE_VALUE,
	GDI_QUEST_INSURANCE_VALUE_MAX,
	GDI_QUEST_INSURANCE_ENABLE_LEVEL,

	//积分商店
	GDI_GET_SHOP_IDX,
	GDI_GET_PLUS_POINT,	// 参数是shopIndex
	GDI_GET_PLUS_POINT_TEMPLATE,
	GDI_GET_PLUS_POINT_BY_PLUSPOINT_INDEX,

	GDI_IS_ITEM_LOCKED_BY_DATE,

	GDI_GET_PASSWORD_STATE, //获取是否有密码保护

	//称号
	GDI_GET_TITLE_INFO,//获取称号信息
	GDI_GET_DETAIL_TITLE_INFO,//获取详细称号信息
	GDI_GET_SELECTED_TITLE,//获取选中称号信息
	GDI_GET_SELF_TITLE,//获取自己当前称号
	GDI_GET_NPC_TITLE,//获取其他玩家的称号

	//获取积分列表的条数   caolei+
	// uParam	= PointType
	// nParam	= NULL
	// nRet		= 积分列表的条数
	GDI_GET_POINTLIST_COUNT,
	
	
	//获取积分列表的第i条内容  caolei+
	// uParam	= PointType
	// nParam	= 指向需要填充的结构的指针
	// nRet		= 积分列表的条数
	GDI_GET_POINTLIST_INFO,

	//获取氏族人气排行信息
	// uParam	= UiShizuPopularityInfo数组
	// nParam	= 缓冲数组长度（个数）
	// nRet		= 实际数据长度（个数）
	GDI_GET_SHIZU_POPULARITY_TOP_N_INFO,

	//获取诸侯人气排行信息
	// uParam	= UiZhuhouPopularityInfo数组
	// nParam	= 缓冲数组长度（个数）
	// nRet		= 实际数据长度（个数）
	GDI_GET_ZHUHOU_POPULARITY_TOP_N_INFO,

	//获取战场击杀排行信息
	// uParam	= UiCombatKillRankInfo数组
	// nParam	= 缓冲数组长度（个数）
	// nRet		= 实际数据长度（个数）
	GDI_GET_COMBAT_KILL_TOP_N_INFO,

	GDI_GET_INSTEAD_SPECIE,
	GDI_GET_PLUS_POINT_INDEX_BY_SHOP_INDEX,

	//获取加入社会关系所需的最低等级
	// uParam	= 指向SocietyInfoIndex结构体的指针
	// nRet		= 加入社会关系所需的最低等级
	GDI_GET_SOCIETY_REQUIRE_LEVEL,
	GDI_GET_SKILL_LEVEL,
};

//=========================================================
// Core外部客户向core获取游戏数据的数据项内容索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
enum GAMEDATA_CHANGED_NOTIFY_INDEX
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	GDCNI_ROLEHEADINFO_UPDATA,
	GDCNI_ROLEHEADINFO_UPDATA_POS,
	GDCNI_ROLEHEADINFO_DEL,
	GDCNI_ROLEHEADINFO_VISIBLE,
	GDCNI_ROLEHEADINFO_HIDE,
	GDCNI_ROLEHEADINFO_BUBBLE_UPDATE,
	GDCNI_SET_MOUSECURSOR,
	GDCNI_SEARCH_INFO,
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	GDCNI_NPC_REFLASH,
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	GDCNI_PING,
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	GDCNI_PK_SETTING,
	/************************************************************************/
	/*						Immediacy room                                  */
	/************************************************************************/
	GDCNI_AUCTION_WND,
	GDCNI_OPEN_USEITEM_DLG,
	/************************************************************************/
	/*						Immediacy room                                  */
	/************************************************************************/

	//likun
	/************************************************************************/
	/*						close all Dialog                                */
	/************************************************************************/
	GDCNI_CLOSE_ALLDIALOG,
	/*!
	\brief
		Add immediacy room.

	\param uParam	
		
	\return
		
	*/	
	GDCNI_ADD_IMMEDIACY,

	/*!
	\brief
		Del immediacy room.

	\param uParam	
		
	\return
		
	*/	
	GDCNI_DEL_IMMEDIACY,

	GDCNI_UPDATA_SHORTCUT,

	GDCNI_REFRESH_SELECTED_SKILL,//更新选中技能

	/************************************************************************/
	/*                     Font system                                      */
	/************************************************************************/
	/*!
	\brief
		Draw text with cegui.

	\param uParam	
		
	\return
		
	*/	
	GDCNI_DRAWTEXT,

	/************************************************************************/
	/*						Item system                                     */
	/************************************************************************/
	/*!
	\brief
		Begin the group cool down.

	\param uParam	
		(KItemGroupCD_C*)uParam.

	\return
		
	*/
	GDCNI_BEGIN_GROUP_CD,

	/*!
	\brief
		End the group cool down.

	\param uParam	
		(KItemGroupCD_C*)uParam.

	\return
		
	*/
	GDCNI_END_GROUP_CD,

	/*!
	\brief
		Open or Close the compound dialog.

	\param uParam	
		(bool) if uParam != 0 open, else close.

	\param nParam	
		(bool)nParam compound result.
		
	\return
		
	*/
	GDCNI_OPEN_COMPOUND_WND,

	/*!
	\brief
		Display Get Object Animation.

	\param uParam = NULL
	\param nParam = NULL		
	\return
			  
	*/
	
	GDCNI_COMPOUND_NEWITEM_NOTIFY,

	GDCNI_PICKUP_OBJECT_TO_BAG,
	/************************************************************************/
	/*				String for client from server                           */
	/************************************************************************/
	/*!
	\brief
		Show string on a framewindow in game space.
	*/
	GDCNI_FRAME_MESSAGE,
	/************************************************************************/
	/*				Login and Exit                                          */
	/************************************************************************/
	/*!
	\brief
		Notify load map.
	*/
	GDCNI_GAME_PRE_LOAD_MAP,

	/*!
	\brief
		Notify end load progress.
	*/
	GDCNI_GAME_END_LOAD_PROGRESS,

	/*!
	\brief
		Notify enter game space.
	*/
	GDCNI_GAME_START,

	/*!
	\brief
		Notify exit game space.
	*/
	GDCNI_END_GAME,

	/************************************************************************/
	/*                         Chat and mail                                */
	/************************************************************************/
	GDCNI_CHAT_ROOM_CHANGEOWNER,
	/*!
	\brief
		Notify chat channel create.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GDCNI_NOTIFY_PLUSITEM_STATE,

	/*!
	\brief
		Notify chat channel create.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GDCNI_CHAT_CHANNEL_CREATE,

	/*!
	\brief
		Notify chat channel create.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GDCNI_CHAT_CHANNEL_CLOSE,

	/*!
	\brief
		Notify chat channel create.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GDCNI_CHAT_All_CHANNEL_CLOSE,

	/*!
	\brief
		Receive message from a person.
	
	\param uParam
		(const string &)The sender role name.

  	\param nParam
		(BYTE *)The message.

	\return
		Nothing.
	*/
	GDCNI_RECV_CHAT_DATE_P2P,
	GDCNI_RECV_CHAT_DATE_P2P_TO_CHAT_WINDOW,				

	/*!
	\brief
		Receive message from a chat room.
	
	\param uParam
		(DWORD)The chat room id.

  	\param nParam
		(BYTE *)The message.

	\return
		Nothing.
	*/
	GDCNI_RECV_CHAT_DATE_R2P,	
	
	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	GDCNI_CHAT_ROOM_CREATE,

	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	GDCNI_CHAT_ROOM_ADD,
	GDCNI_CHAT_ROOM_JOIN,
	GDCNI_CHAT_ROOM_LEAVE,
	GDCNI_CHAT_ROOM_KICK,
	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	GDCNI_FRIENDLIST_NOTIFY,

	/*!
	\brief
		Open or close the mail tip dialog.

	\return
		
	*/
	GDCNI_SWITCH_MAIL_TIP,

	/*!
	\brief
		Open or close the mail dialog.

	\return
		
	*/
	GDCNI_SWITCH_MAIL,

	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	GDCNI_RECV_MAIL,

	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GDCNI_RECV_MAIL_LIST,

	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	GDCNI_SEND_MAIL_RET,

	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	GDCNI_DEL_MAIL_RET,

	GDCNI_OPEN_PLAYER_INFO,

	GDCNI_NEW_MAIL_NOTIFY,

	GDCNI_OPEN_MAIL_NOTIFY,

	GDCNI_OPEN_LIGHT,
	GDCNI_CLOSE_LIGHT,
	GDCNI_PK_VALUE_CHANGE,
	/************************************************************************/
	/*							Buffer system                               */
	/************************************************************************/
	/*!
	\brief
		Notify client to open a buffer.
	
	\param uParam
		(DWORD)The buffer param struct.

  	\param nParam
		(int)no used.

	\return
		Nothing.
	*/	
	GDCNI_BUFFER_OPEN,

	/*!
	\brief
		Notify client to delete buffer.
	
	\param uParam
		(DWORD)The buffer param struct.

  	\param nParam
		(int)no used.

	\return
		Nothing.
	*/	
	GDCNI_BUFFER_DEL,

	/*!
	\brief
		Notify client to delete all buffers.
	
	\param uParam
		(DWORD)The buffer param struct.

  	\param nParam
		(int)no used.

	\return
		Nothing.
	*/	
	GDCNI_BUFFER_DEL_ALL,

	/************************************************************************/
	/*								Death                                   */
	/************************************************************************/
	GDCNI_DEATH,
	GDCNI_DEATH_CLOSE,

	/************************************************************************/
	/*								Death                                   */
	/************************************************************************/
	GDCNI_REVIVE,

	/************************************************************************/
	/*							Select target                               */
	/************************************************************************/
	GDCNI_SEL_TARGET,

	CDCNI_UPDATA_SEL_TARGET,

	CDCNI_UPDATA_SELF_FACE,

	CDCNI_UPDATA_ROLE_STATE,

	/************************************************************************/
	/*							Quest system                                */
	/************************************************************************/
	//点击NPC，由脚本触发打开NpcMsgBox
	GDCNI_OPEN_QUEST_NPC_DIALOG,
	//电影场景的触发
	GDCNI_OPEN_MOVIE_SCENE,
	//脚本触发关闭NpcMsgBox
	GDCNI_NPC_DLG_CLOSE,
	//core内任务发生变更（包括任意一种变更）
	GDCUI_QUEST_LIST_CHANGE,
	/************************************************************************/
	/*							Skill system                                */
	/************************************************************************/

	/*!
	\brief
		Notify skill list change.
	*/
	GDCNI_SKILLLIST_CHANGE,

	/*!
	\brief
		Notify skill list change.

	\param 
		uParam(int) show or hide.		

	\param 
		nParam(int) if study skill.		

	*/
	GDCNI_SKILLLIST_OPEN,

	/*!
	\brief
		Notify skill list change.

	\param 
		uParam(int) SkillID.		
	*/
	GDCNI_SKILL_CD_BEGIN,

	/************************************************************************/
	/*							Team system                                 */
	/************************************************************************/
	/*!
	\brief
		Notify team list change.
	*/
	CDCNI_UPDATE_TEAM_INFO,

	/*!
	\brief
		Notify team list change.

	\param 
		uParam(int) show or hide.		

	\param 
		nParam(int) if study skill.		

	*/
	CDCNI_OPEN_INVOTE_TEAM_REPLY,

	CDCNI_OPEN_APPLY_JOIN_TEAM_REPLY,

	GDCNI_UPDATE_LIST_TEAM,

	/************************************************************************/
	/*							Tong system                                 */
	/************************************************************************/
	GDCNI_OPEN_CREATETONG,

	GDCNI_OPEN_CITY,

	GDCNI_OPEN_INVOTE_SOCIETY_REPLY_COMFIRM,

	GDCNI_UPDATA_TONG_MANAGER,

	//玩家交易相关

	GDCNI_TRADE_START,

	GDCNI_TRADE_OPPOSITE_BUSY,

	GDCNI_TRADE_OPPOSITE_MONEY_CHANGED,

	GDCNI_TRADE_LOCK,		//自己或对方锁定状态同步

	GDCNI_TRADE_UNLOCK,		//锁定状态被对方解除

	GDCNI_END_TRADE,		//同步自己或对方是否点击交易按钮

	GDCNI_TRADE_OK,			//交易成功

	GDCNI_TRADE_CANCEL,		//交易取消

	GDCNI_OBJECT_CHANGED,	//物品交换位置

	GDCNI_JINSHANBI_CHANGED,

	//顶部信息
	GDCNI_TOPMESSAGE,
	
	GDCNI_TOPMESSAGE_ID,
	
	//氏族banner
	GDCNI_SHIZU_BANNER,

	//IB商店信息
	//nParam 格式(CommonStyle)
	//uParam 要显示的字符串(char*)
	GDCNI_IBSHOPMESSAGE,

	//成功还款返回信息
	GDCNI_RETURN_CREDIT_SUCCESS,

	//成功购买IB物品返回信息
	//uParam IBGoods_ID
	GDCNI_IBITEM_BUY_SUCCESS,

	//IB商店Shelf
	//uParam = (Load_IBShelf_Ret*)
	//nParam = ShelfNum
	GDCNI_IBSHOP_SHELF,
	
	//CREDIT商店Shelf
	//uParam = (Load_IBShelf_Ret*)
	//nParam = ShelfNum
	GDCNI_CREDITSHOP_SHELF,

	//POINT商店Shelf
	//uParam = (Load_IBShelf_Ret*)
	//nParam = ShelfNum
	GDCNI_POINTSHOP_SHELF,

	//IB/CREDIT/POINT商店Panel
	//uParam = (Panel*)
	//nParam = PanelNum
	GDCNI_IBSHOP_PANEL,

	//IB/CREDIT/POINT商店ContentStyle
	//uParam = (ContentStyle*)
	//nParam = ContentStyleNum
	GDCNI_IBSHOP_CONTENTSTYLE,

	//IB商店Goods
	//uParam = (IBGoods_ListEntry*)
	//nParam = GoodsCount
	GDCNI_IBSHOP_GOODS,
	
	//CREDIT商店Goods
	//uParam = (IBGoods_ListEntry*)
	//nParam = GoodsCount
	GDCNI_CREDITSHOP_GOODS,

	//POINT商店Goods
	//uParam = (IBGoods_ListEntry*)
	//nParam = GoodsCount
	GDCNI_POINTSHOP_GOODS,

	//弹出气泡
	//nParam 要显示的字符串(char*)
	//uParam 位置(Point)
	GDCNI_POPMESSAGE,
	//弹出通用对话框
	//nParam 要显示的字符串(char*)
	//uParam 位置(Point)
	GDCNI_COMMSG,
	GDCNI_MAKESURE_USEITEM,
	
	//升级窗口信息
	GDCNI_LEVELUPINFO,

	//弹出买卖窗口
	GDCNI_NPC_TRADE,

	//来自游戏世界的错误提示，例如距离过远等
	GDCNI_ERROR_MESSAGE,
	
	//来自IB商店的错误提示
	GDCNI_IBCENTER_ERROR_MESSAGE,
	//uParam 要显示的字符串(char*)
	//nParam 错误码

	//来自游戏世界的错误提示，例如距离过远等
	GDCNI_ERROR_MESSAGE_CODE,

	//重要信息
	GDCNI_TOP_MESSAGE,

	//打开储物箱，并且告诉客户端挪动物品是否需要密码
	GDCNI_OPEN_STORE_BOX,

	//修改储物箱密码成功
	GDCNI_STORE_BOX_CHANGE_PASSWORD_RESULT,
	
	//解除储物箱锁定成功
	GDCNI_STORE_BOX_UNLOCK_RESULT,

	//装备耐久变化
	GDCNI_ALERT_DURA,

	//玩家负重变化
	GDCNI_PLAYER_WEIGHT_CHANGED,

	//包裹格子数变化
	GDCNI_PLAYER_BAG_SIZED,

	//收到交易请求消息
	GDCNI_RECIVE_TRADE_REQUEST,

	//收到交易拒绝消息
	GDCNI_RECIVE_TRADE_REFUSE,

	//收到交易拒绝消息
	GDCNI_TRADE_OPPOSITE_PICKUP_ITEM,

	//吟唱条
	CDCNI_CASTBAR_OPER,

	//法宝属性变更通知
	CDCNI_TALISMAN_PROP_CHANGE,
	
	//法宝蕴魂变更通知
	CDCNI_TALISMAN_POTENTIAL_CHANGE,

	//目标装备可以显示通知
	GDCNI_EQUIPMENT_VIEW_NOTIFY,

	//玩家开始跑动时通知界面
	GDCNI_PLAYER_RUN,
	//追加战斗信息
	GDCNI_APPEND_COMBAT_INFO,

	//打开打造窗口
	GDCNI_OPEN_SMITH_SHOP,

	//打开功能面板
	GDCNI_OPEN_NAVIGATION_WND,
	//打开扩展功能面板
	GDCNI_OPEN_NAVIGATIONEX_WND,

	//打开快捷栏面板
	GDCNI_OPEN_SHORTCUT_WND,
	//打开扩展快捷栏面板
	GDCNI_OPEN_SHORTCUTPLUS_WND,

	//激活功能面板按键
	//uParam 要显示的字符串(char*)
	GDCNI_ACTIVE_NAVIGATION_BUTTON,

	GDCNI_END_SMITH,

	//向指定频道追加消息
	GDCNI_APPEND_MESSAGE,

	//玩家PK提示消息
	GDCNI_PLAYER_ATTACK_NOTIFY,

	//uParam = QuestID
	GDCNI_QUEST_INFO_CHANGED,

	GDCNI_BEGIN_AUTO_PATH,

	GDCNI_STOP_AUTO_PATH,
	
	GDCNI_BEGIN_AUTO_ATTACK,

	GDCNI_STOP_AUTO_ATTACK,

	/***************************************************
	* TaisuiWheel system                              *
	***************************************************/
	
	GDCNI_TIAN_XIANG_CHANGED,
	GDCNI_TAISUI_WHEEL_TIANGAN_RES,
	GDCNI_TAISUI_WHEEL_DIZHI_RES,
	GDCNI_JIAZI_EVENT_CHANGED,	
	GDCNI_WHEEL_TIMES_CHANGED,
	GDCNI_TAISUI_DLG_OPEN,
	GDCNI_TAISUI_DLG_CLOSE,

	GDCNI_PROMPT_ADD_BUFF,//请示是否添加BUFF
	GDCNI_CANCEL_PROMPT_ADD_BUFF,//取消请示

	GDCNI_FURY_CHANGE,
	GDCNI_FURY_WARNNING,
	
	GDCNI_OPEN_WINDOW,
	GDCNI_OPEN_TIMER,
	GDCNI_OPEN_TONG_RECRUIT,
	GDCNI_OPEN_INSTANCE_REWARD,
	GDCNI_OPEN_CREDIT_SHOP,

	GDCNI_UPDATA_TONG_RECRUIT,
	GDCNI_UPDATA_SOCIAL_INFO,

	GDCNI_PET_CALLED_OUT,
	GDCNI_PET_RELEASE,
	GDCNI_PET_UPDATE,
	GDCNI_CHAT_MINI_MAP_UPDATA,
	GDCNI_SPECIAL_QUEST_DATA,
	GDCNI_GM_FEED_BACK,
	GDCNI_RECV_HIRE_DATA_EXP,		//雇佣数据exp
	GDCNI_RECV_HIRE_DATA_FIGHT,		//雇佣数据fight
	GDCNI_HIRE_REQ_RET,				//雇佣请求回应

	GDCNI_RECV_JINSHANBI,			//同步金山币
	GDCNI_RECV_CREDITPOINT,			//同步信用点
	GDCNI_RECV_POINT,				//同步积分
	GDCNI_RECV_MAXCREDITPOINT,		//同步最大信用点
	GDCNI_RECV_CREDITSTATE,			//同步信用状态
	GDCNI_RECV_CREDITRETURNDATA,	//同步还款日期


	GDCNI_RECV_WORLD_COMBAT_SCORE,  //同步战场总积分

	GDCNI_DEBUG_TRACK_INJECT,		//开启窗口消息跟踪（调试用）
	GDCNI_DEBUG_PRINT_WINDOW,		//打印窗口跟踪（调试用）
	
	/*------------------------推荐系统--------------------*/
	GDCNI_LIST_STUDENT,
	GDCNI_OPEN_RECOMMEND_REPORT,	//打开被推荐人报道窗口
	/*------------------------推荐系统--------------------*/

	GDCNI_COMBAT_TOP_MEMBER_INFO,//战场排行榜数据更新
	
	GDCNI_INSURANCE_INFO,

	GDCNI_ASK_QUESTION,

	/*-----------------------经验保险---------------------*/
	GDCNI_EXP_INSURANCE_STATE_NOTIFY,
	GDCNI_EXP_INSURANCE_REWARD_NOTIFY,
	GDCNI_EXP_INSURANCE_REWARD,
	GDCNI_EXP_INSURANCE_ENTER_STATE,

	/*-----------------------任务保险---------------------*/
	GDCNI_QUEST_INSURANCE_STATE_NOTIFY,
	GDCNI_QUEST_INSURANCE_REWARD_NOTIFY,
	GDCNI_QUEST_INSURANCE_REWARD,
	GDCNI_QUEST_INSURANCE_ENTER_STATE,

	GDI_EXP_QUEST_INSURANCE_LEVEL_UP_NOTIFY,
	GDCNI_UPDATA_PLUS_POINT,

	/*-----------------------小战场积分显示---------------------*/
	GDCNI_UPDATE_SMALL_BATTLE_FIELD_RESULT,

	/*-----------------------自动打怪-------------------------*/
	GDCNI_AUTO_ATTACK_BLAST,
	GDCNI_AUTO_REPAIR,
	
	/*-----------------------地图公告字符串-------------------*/
	GDCNI_MAP_PRONUNCIAMENTO_STR,

	/*-----------------------称号刷新通知-------------------*/
	GDCNI_TITLEINFO_UPDATA,

	GDCNI_POINTLIST_REFRESH,

	GDCNI_SHIZU_POPULARITY_UPDATE,		//氏族人气排行更新
	GDCNI_ZHUHOU_POPULARITY_UPDATE,		//诸侯人气排行更新
	GDCNI_COMBAT_KILL_RANK_UPDATE,		//战场击杀排行榜
	GDCNI_SELF_PROPERTIES_UPDATE,		//自己属性页属性更新
	GDCNI_PLAYER_PROPERTIES_UPDATE,		//玩家属性页属性更新
	/*-----------------------雕像信息-------------------*/
	GDCNI_STATUE_INFO,
	GDCNI_PLAYER_REAL_INFO,				//用户信息
	GDCNI_PLAY_ANIMATION,
};

 struct UI_RANK_DATA_TRANSFER 
 {
 	DWORD dwOperID;
 	BYTE* pData;
 	int	  nCount;
 };
 enum GAMEDEBUGCONTROL
 {
 	DEBUG_SHOWINFO = 1,
 	DEBUG_SHOWOBSTACLE,
 	DEBUG_GM_CMD,
 };

//=========================================================
// Core外部客户对core的操作请求的索引定义
//=========================================================
enum GAMEOPERATION_INDEX
{


	GOI_SET_ACTION,

	GOI_EXIT_GAME = 1,		//离开游戏
	//uParam = bIpSpotExit

	GOI_USE_YIBU_ITEM,

	GOI_SWITCH_LIFE_ROLE,

	GOI_SWITCH_NAME_ROLE,
//	GOI_SET_FOLLOW_ATTACK,

	GOI_SWITCH_OBJECT_QUERY,		//交换
	//uParam = (KUiObjAtContRegion*)pObject1 -> 拿起的物品操作前的信息
	//如果无拿起的东西，则uParam = 0
	//nParam = (KUiObjAtContRegion*)pObject2 -> 放下的物品操作后的信息
	//如果无放下的东西，则nParam = 0
	//nRet = bSwitchable -> 是否可交换

	GOI_SWITCH_OBJECT,		//交换
	//uParam = (KUiObjAtContRegion*)pObject1 -> 拿起的物品操作前的信息
	//nParam = (KUiObjAtContRegion*)pObject2 -> 放下的物品操作后的信息
	//nRet = bSwitched -> 是否交换了

	GOI_FIND_A_EMPTY_PLACE_OF_A_CONTAINER,

	GOI_TALISMAN_INLAY,		//法宝镶嵌入孔
	//uParam = int 内丹的物品index
	//nParam = int 法宝上孔的index

	GOI_TALISMAN_UPGRADE,	//法宝镶嵌入孔
	//uParam = int 内丹的物品index
	//nParam = int 法宝上孔的index

	GOI_TALISMAN_CONVERT_SKILL_EXP,		//转化技能经验

	GOI_REJECT_OBJECT,		//丢弃物品
	//uParam = (KUiObjAtContRegion*)pObject -> 欲丢弃的物品

	GOI_MONEY_INOUT_STORE_BOX,	//从StoreBox存取钱
	//uParam = (unsigned int)bIn 为非0值时表示存入，否则表示取出
	//nParam = 钱的数额

	GOI_PLAYER_ACTION,		//玩家执行/取消某个动作
	//uParam = (PLAYER_ACTION_LIST)eAction 动作标识

	GOI_PLAYER_RENASCENCE,		//玩家重生
	//nParam = (int)(bool)bBackTown 是否回城

	GOI_INFORMATION_CONFIRM_NOTIFY,	//消息获得确认的通知

	GOI_QUESTION_CHOOSE,	//问题选择答案
	//nParma = nAnswerIndex
	
	//<---- Add By Ray [Luoliang] [2005-10-21]
	GOI_QUESTION_MULTI_SEL,	//问题答案的多个选择
	//uParam = (int*)nSelIndices
	//nParam = nCount
	// End. Ray [LuoLiang] [2005-10-21] ---->
	
	//uParam = (KUiObjAtRegion*)pInfo -> 装备的数据和放置位置信息
	//			KUiObjAtRegion::Region::h 表示属于第几套装备
	//			KUiObjAtRegion::Region::v 表示属于哪个位置的装备,其值为梅举类型
	//			UI_EQUIPMENT_POSITION的取值之一。请参看UI_EQUIPMENT_POSITION的注释。
	/************************************************************************/
	/*						Tong system                                     */
	/************************************************************************/
	GOI_SOCIETY_INVITE_RESPONSE,
    GOI_SOCIETY_SET_CITY_TEX_RATE,
	/*uParam = new tax*/

	/************************************************************************/
	/*						Immediacy room                                  */
	/************************************************************************/
	/*!
	\brief
		Add immediacy room.

	\param uParam	
		
	\return
		
	*/	
	GOI_ADD_IMMEDIACY,

	/*!
	\brief
		Del immediacy room.

	\param uParam	
		
	\return
		
	*/	
	GOI_DEL_IMMEDIACY,

	/************************************************************************/
	/*								Item system                             */
	/************************************************************************/
		

	/*!
	\brief
		开始进行合成，(KUiBesetOperationParam*)uParam
	
	\return
		Nothing.	
	*/
	GOI_COMPOUND_BEGIN,	

	GOI_DO_SMITH,
	/*!
	\brief
		使用物品
	
	\param 
		uParam = (KUiObjAtRegion*)pInfo -> 物品的数据以及物品原来摆放的位置

	\param 
		nParam = 物品使用前放置的位置，取值为枚举类型UIOBJECT_CONTAINER。
	
	\param 
		Skill Param(int)

	\return
		Nothing.	
	*/
	GOI_USE_ITEM,			

	/************************************************************************/
	/*							Skill system			                    */
	/************************************************************************/
	/*!
	\brief
		set immdia skill.
	
	\param 
		Skill ID(unsigned int)
	
	\param 
		Skill Param(int)

	\return
		Nothing.	
	*/
	GOI_SET_IMMDIA_SKILL,

	/*!
	\brief
		Use a skill.
	
	\param 
		Skill ID(unsigned int)
	
	\param 
		Skill Param(int)

	\return
		Nothing.	
	*/
	GOI_USE_SKILL,

	/*!
	\brief
		Cancel a skill.
	
	\param 
		Skill ID(unsigned int)
	
	\param 
		Skill Param(int)

	\return
		Nothing.	
	*/
	GOI_SKILL_CANCEL,

	/*!
	\brief
		Level up a skill.
	
	\param 
		Skill ID(unsigned int)
	
	\param 
		Skill Param(int)

	\return
		Nothing.	
	*/
	GOI_LEVELUP_SKILL,

	GOI_SET_L_SKILL,
	GOI_SET_R_SKILL,

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	GOI_TONE_UP_ATTRIBUTE,	//增强一些属性的值，一次加一点
	//uParam = 表示要增强的是哪个属性，取值为UI_PLAYER_ATTRIBUTE的梅举值之一

	//============（与其它玩家）交易相关================
	GOI_TRADE_INVITE_RESPONSE,	//答应/拒绝交易请求
	//uParam = (KUiPlayerItem*)pRequestPlayer 发出请求的玩家
	//nParam = (int)(bool)bAccept 是否接受请求

	
	GOI_TRADE_SEND_INVITE_RESPONSE,	//用户决定是否同意交易
	//uParam = 是否同意交易 bool
	//nParam = 0
	//Return = 0

	GOI_TRADE_CHANGE_MONEY,		//增减一个欲卖出的物品
	//uParam = (KUiObjAtRegion*) pObject -> 物品信息，其中坐标信息为在交易界面中的坐标
	//nParam = bAdd -> 0值表示减少，1值表示增加
	//Remark : 如果物品是金钱的话，则KUiObjAtRegion::Obj::uId表示把金钱额调整为这个值，且nParam无意义。

	GOI_TRADE_WILLING,			//有无交易意向
	//uParam = (const char*)pszTradMsg 关于交易消息一句话，当bWilling为true时有效
	//nParam = (int)(bool)bWilling 是否期待交易(叫卖)

	GOI_TRADE_LOCK,				//锁定交易
	//nParam = (int)(bool)bLock 是否锁定

	GOI_TRADE,					//交易
	//nParam = (int)(bool)bTrading
	
	GOI_TRADE_CANCEL,			//交易取消

	//============（与npc）交易相关================
	GOI_TRADE_NPC_BAG_IS_FULL,	//判断玩家背包是否还有空间（现在只做简单检测）
	//uParam = (ItemType)pObj -> 物品信息
	//nParam = nItemCount ->物品数量

	GOI_TRADE_NPC_BUY,			//向npc买物品
	//uParam = (KUiGameObject*)pObj -> 物品信息

	GOI_TRADE_NPC_SELL,			//卖物品给npc
	//uParam = (KUiObjAtContRegion*)pObj -> 物品信息

	GOI_TRADE_NPC_REPAIR,		//修理物品
	//uParam = (KUiObjAtContRegion*) pObj -> 想要的物品的信息

	GOI_TRADE_NPC_REPAIR_ALL,		//修理物品
	//uParam = (KUiObjAtContRegion*) pObj -> 想要的物品的信息

	GOI_TRADE_NPC_CLOSE,		//结束交易

	GOI_DROP_ITEM_QUERY,		//查询是否可以丢某个东西到游戏窗口
	//uParam = (KUiGameObject*)pObject -> 物品信息
	//nParam = 被拖动东西的当前坐标（绝对坐标），横坐标在低16位，纵坐标在高16位。(像素点坐标)
	//Return = 是否可以放下
	
//	GOI_DROP_ITEM,				//放置物品到游戏窗口
	//参数含义同GOI_DROP_ITEM_QUERY参数含义相同
	//Return = 是否东西被放下了

	/************************************************************************/
	/*                 Chat system and friend system                        */
	/************************************************************************/
	GOI_CHANGE_ROOM_OWNER,
	GOI_ROOM_SCREEN,
	GOI_ROOM_UNSCREEN,

	GOI_FIND_PLAYER,
	/*!
	\brief
		Send message to a person by role name.
	
	\param uParam
		(const string &)The person role name.

  	\param nParam
		(BYTE *)The message.

	\return
		Nothing.
	*/
	GOI_SEND_CHAT_DATE_P2P,				

	/*!
	\brief
		Send message to a chat room by the chat room id.
	
	\param uParam
		(DWORD)The chat room id.

  	\param nParam
		(BYTE *)The message.

	\return
		Nothing.
	*/
	GOI_SEND_CHAT_DATE_P2R,					

	/*!
	\brief
		Create a chat room.
	
	\param uParam
		(DWORD)The chat room window data.

  	\param nParam
		(const string &)The member role name.

	\return
		Nothing.
	*/
	GOI_CREATE_CHATROOM,

	/*!
	\brief
		Add a new member to the chat room.
	
	\param uParam
		(DWORD)The room ID.

  	\param nParam
		(const string &)The member role name.

	\return
		Nothing.
	*/
	GOI_ADDTO_CHATROOM,

	/*!
	\brief
		Leave from the chat room.
	
	\param uParam
		(DWORD)The room ID.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_LEAVE_CHATROOM,

	/*!
	\brief
		Kick member from the chat room.
	
	\param uParam
		(DWORD)The room ID.

  	\param nParam
		(const string &)The kicked member role name.

	\return
		Nothing.
	*/
	GOI_KICK_CHATROOM,

	/*!
	\brief
		Add a new friend.
	
	\param uParam
		(const string &)The new friend role name.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_CHAT_FRIEND_ADD,	

	/*!
	\brief
		Add a new friend.
	
	\param uParam
		(const string &)The new friend role name.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_CHAT_FRIEND_CHANGE_GROUP,


	/*!
	\brief
		Add a new friend.
	
	\param uParam
		(const string &)The new friend role name.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_CHAT_ADD_BLACK_LIST,
	GOI_CHAT_ADD_BLACK_LIST_BY_ID,
	
	/*!
	\brief
		Delete a friend.
	
	\param uParam
		(const string &)The deleted role name.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_CHAT_FRIEND_DELETE,		


	/*!
	\brief
		Create a new friend group.
	
	\param uParam
		(const string &)The new friend group name.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_CHAT_GROUP_NEW,	
	
	/*!
	\brief
		Delete a new friend group.
	
	\param uParam
		(const string &)The delete friend group name.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_CHAT_GROUP_DELETE,
	/*!
	\brief
		Rename a new friend group.
	
	\param uParam
		(const string &)The old friend group name.

  	\param nParam
		(const string &)The new friend group name.

	\return
		Nothing.
	*/
	GOI_CHAT_GROUP_RENAME,
	
	/************************************************************************/
	/*					Mail system											*/
	/************************************************************************/
	
	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_SEND_MAIL,

	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_MAIL_LIST_REQ,

	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_MAIL_REQ,

	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_DEL_MAIL,

	GOI_SEND_BACK_MAIL,

	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_GET_MONEY,

	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	GOI_GET_ITEM,
	
	/************************************************************************/
	/*					Buffer system                                       */
	/************************************************************************/
	/*!
	\brief
		Request delete the buffer.
	
	\param uParam
		(DWORD)The buffer ID.

  	\param nParam
		(int)no used.

	\return
		Nothing.
	*/	
	GOI_DEL_BUFFER,

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	GOI_OPTION_SETTING,			//选项设置
	//uParam = (OPTIONS_LIST)eOptionItem 要设置的选项
	//nParam = (int)nValue 设置的值，其含义依赖于eOptionItem的含义
	//					参看OPTIONS_LIST各值的注释

	GOI_PLAY_SOUND,				//播放声音
	//uParam = (const char*)pszFileName

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	/*!
	\brief
		Set PK state		
	*/
	GOI_PK_SETTING,				//设置PK
	//nParam = (int)nEnable		设置pk状态，0练功模式 1战斗模式 2屠杀模式

	GOI_REVENGE_SOMEONE,		//仇杀某人
	//uParam = (KUiPlayerItem*) pTarget	仇杀目标

	GOI_SHOW_PLAYERS_NAME,		//显示各玩家人名
	//nParam = (int)(bool)bShow	是否显示
	GOI_SHOW_PLAYERS_LIFE,		//显示各玩家生命
	//nParam = (int)(bool)bShow	是否显示
	GOI_SHOW_PLAYERS_MANA,		//显示各玩家内力
	//nParam = (int)(bool)bShow	是否显示

	GOI_SHOW_PLAYERS_BODY,		//显示各玩家内力
	//nParam = (int)(bool)bShow	是否显示

	GOI_GAMESPACE_DISCONNECTED,	//游戏世界断开连接了
	
	GOI_VIEW_PLAYERITEM,		//申请看玩家装备
	//uParam = dwNpcID	玩家的m_dwID
	GOI_VIEW_PLAYERITEM_END,	//看玩家装备结束

	GOI_FOLLOW_SOMEONE,			//跟随某人
	//uParam = (KUiPlayerItem*) pTarget	跟随目标

	GOI_QUERY_RANK_INFORMATION, //获取数据请求
	//uParam = usIndexId 排名项的id

	GOI_SWITCH_HORSE,           //上下马状态的切换，uParam = 0 上马       1 下马

	GOI_DROP_MONEY,             //丢钱在地上，uParam = 丢多少钱
	
	GOI_SHOW_GAMESPACE_ITEM_NAME,//显示游戏地图上物品的名字,nParam == 0 不显示   !0 显示
	
	GOI_GIVE_SOMEONE_MONEY,     //给某人金钱，uParam == 玩家ID，nParam == 多少钱
	
	GOI_GAME_APP_ACTIVE,		//游戏程序切换到前台/后台
	//nParam = (int)bool bActive

	GOI_PLAYER_SET_PORTRAIT,	//玩家设置头像,nParam = 头像索引
	// flying commented, about the stall process
	// uParam =	0: Finish the stall status;
	//			1: Enter the stall status;
	//			2: 
	// nParam: not used.
	GOI_SET_STALL,
	// Mark price.
	// uParam: 0 : Exit the stall status.
	//		   none-zero : MarkPrice, item array's start address
	//			   nParam: nCount.
	GOI_SET_ADVSTRING,
	// Set the adv string
	// uParam: char*
	// nParam: int
	//////////////////////////////////////////////////////////////////////////
	//  Customer side
	GOI_STALL_CUST_GET_ADV,		// command to get the advertisement information
	//GOI_STALL_CUST_GET_STALLER_STAT, // command to get the staller's status.
	GOI_STALL_CUST_GET_DETAIL,	// command to get all the goods in stall status.
	// and these common
	GOI_SET_PEOPLE_INDEX_BY_PLAYER,
	// uParam: the target index
	// nParam: the "people" index

	GOI_SWITCH_STALL_BUY,       //切换(摆摊中买方)买东西状态
	GOI_SWITCH_STALL,			//切换摆摊状态 [wxb 2003-11-19]
	//-------> Ray [Luoliang] 2004-8-14
	GOI_CLEAR_STALL_ITEM,		//清除摊中的所有物品
	//<------- End [Ray]
	GOI_STALL_BUY_ITEM,				//在摊子上买东西
	// uParam: the item index
	//-------> Ray [Luoliang] 2005-6-10
	GOI_STALL_SEND_MESSAGE,		//发送摆摊留言
	// uParam = const char * szMsg
	// nParam = nLen (Message Length)
	//<------- End [Ray]
	//请求动感色子老虎机上的累积大奖金额
	GOI_QUERY_DICE_WEAVE_BINGO_MONEY,

	//动感色子老虎机上的下注请求
	GOI_BET_ON_DICE_WEAVE,

	//动感色子老虎机的色子点数，uParam = 第几颗色子(0基)
	GOI_DICE_NUMBER_ON_DICE_WEAVE,

	//发送可以处理结果了的通知
	GOI_CAN_HANDLE_RESULT_ON_DICE_WEAVE,

	/************************************************************************/
	/*							Quest system                                */
	/************************************************************************/
	/*!
	\brief
		Submit a quest.
  
	\param 
		(KQuestRequest*)pQuest.
	
	\return
		Nothing.
	*/
	GOI_OK_QUEST,

	/*!
	\brief
		Cancel a quest.
  
	\param 
		(KQuestRequest*)pQuest.
	
	\return
		Nothing.
	*/	
	GOI_CANCEL_QUEST,

	/*!
	\brief
		Delete a quest.
  
	\param 
		(KQuestRequest*)pQuest.
	
	\return
		Nothing.
	*/	
	GOI_DELETE_QUEST,

	/*!
	\brief
		Request a quest operation.

	\param 
		Quest id (int).

	\return
		Nothing.
	*/
	GOI_QUEST_REQUEST,

	//提交任务
	// uParam: (KILLER_SUBMITTASK*)pTask
	GOI_KILLER_SUBMITTASK,
	
	//取消任务	uParam: TaskID
	GOI_KILLER_CANCELTASK,

	//接收任务  uParam: TaskID
	GOI_KILLER_TAKETASK,

	//问万事通  uParam: (char*)szName
	GOI_KILLER_QUERY_WISEMAN,

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	
	//----->Add by [Ray] 2004-7-16
	//通用输入框返回了结果
	GOI_GENERAL_INPUT_RESULT,
	//uParam = char * 指向输入框中的内容
	//nParam = 输入框中字符数量
	//<-----Add End

	GOI_LOTTO_PICKS, // 设置购买的彩票的内容
	GOI_ROULETTE_NOTIFY, // 通知"太岁彩"结束,让奖励生效

//----->Add by [Ray] 2004-7-28
//互博相关的操作
	GOI_GAMBLE_INVITE_RESPONSE,	//答应/拒绝赌博请求
	//uParam = (KUiPlayerItem*)pRequestPlayer 发出请求的玩家
	//nParam = (int)(bool)bAccept 是否接受请求

	GOI_GAMBLE_DESIRE_ITEM,		//增减一个欲卖出的物品
	//uParam = (KUiObjAtRegion*) pObject -> 物品信息，其中坐标信息为在交易界面中的坐标
	//nParam = bAdd -> 0值表示减少，1值表示增加
	//Remark : 如果物品是金钱的话，则KUiObjAtRegion::Obj::uId表示把金钱额调整为这个值，且nParam无意义。

	GOI_GAMBLE,					//赌博
	//nParam = (int)(bool)bTrading
	
	GOI_GAMBLE_CANCEL,			//赌博取消
//<-----Add End
//----->Add by [Ray] 2004-8-2
	GOI_BIGANDSMALL_BET,		//压注
	//uParam = 	
	//nParam = (-1,0,1,2,3)		分别为0,200,500,1000,和赢得的钱一起加押,-1表示取钱
	GOI_BIGANDSMALL_END,		//结果出来了,通知服务器
	//
//-------> Ray [Luoliang] 2004-8-10
	GOI_TEAM_AUTO_FOLLOW,		//	自动跟随
	//nParam = 0, 1				//	是否自动跟随

	GOI_STOREBOX_ENTER_PASSWORD,	// 储物箱的密码输入
	//uParam = (char*)szPassword
	//nParam = 密码长度
	GOI_STOREBOX_CREATE_PASSWORD,	// 创建储物箱的密码
	//uParam = (char*)szPassword
	//nParam = 密码长度
	GOI_STOREBOX_CHANGE_PASSWORD,	// 修改储物箱的密码
	//uParam = (char*)Old Password,
	//nParam = (char*)New Password,
	GOI_STOREBOX_CLEAN_PASSWORD,	// 清除储物箱的密码
	//uParam = char szPass[64]		二级密码
	//
	GOI_LOCK_STOREBOX,				// 储物箱关闭
	//
	GOI_ITEM_LIST_OPERATION_LOCK,	//  锁定物品操作
	//
	GOI_ITEM_LIST_OPERATION_UNLOCK,	//  取消物品操作锁定

//<------- End [Ray]
//<-----Add End
	//-------> Ray [Luoliang] 2004-9-17
	//赏金猎人的相关操作
//	// Hunter, [Adt.X], 2005-3-9.
//	GOI_REWARD_HUNTER_POSTER_OPERID,	// 发送同步Poster OperId.
//	// End.

	GOI_REWARD_HUNTER_GET_MISSION_LIST,	// 得到赏金猎人任务列表
	//uParam = offset.
	//nParam = count.

	GOI_REWARD_HUNTER_MISSION_APPLY,	// 接受任务
	//uParam = Post Id.

	GOI_REWARD_HUNTER_MISSION_ADD,		// 增加一条赏金猎人任务
	//uParam = (REWARD_HUNTER*)pItem 
	//nParam = 
	
	GOI_REWARD_HUNTER_MISSION_CANCEL,	// 取消接了的任务
	//
	
	GOI_REWARD_HUNTER_MISSION_DELETE,	// 删除发布了的任务
	//
	
	//-->Rocker 2004/11/04 
	GOI_SET_PLAYER_CAMP, // 设置玩家自己的阵营
	//uParam
	//nParam = (NPCCAMP)enumCamp, 玩家想变的阵营
	//下面两个无效
	//camp_animal,			// 野兽阵营
	//camp_event,				// 路人阵营
	//<--Rocker 
	
	
	//-------> Ray [Luoliang] 2004-11-17
	GOI_RANK_OPERATE_ID,	//像服务器发送排名列表的OperID
	//uParam = dwOperID
	//nParam = nIndex 
	// nIndex = 0,	世界十大高手
	//			1,	世界十大富豪
	//			2,	甲士十大高手
	//			3,	甲士十大富豪
	//			4,	道士十大高手
	//			5,	道士十大富豪
	//			6,	异人十大高手
	//			7,	异人十大富豪
	//			8,	军事十大强国
	//			9,	经济十大强国
	
	//<------- End [Ray]

	GOI_SET_SECENE_FOCUSOFFSET, // 设置场景的默认偏移
	//uParam nX 横方向的偏移 有正负
	//nParam nY 纵方向的偏移 有正负
	GOI_SYSCHANNEL_MSG,			// 发送系统消息(显示在聊天框内
	//uParam = (KUiSysChannelMsg*)pMsg
	//nParma = 
	
	GOI_ENABLE_TO_THROW_AWAY_ITEM,	//是否允许将东西扔在地上
	//uParam = (BOOL)bEnable
	//nParam = 

	// Add by Cooler -->
	// 2005-3-20
	GOI_IS_MALE, 
	GOI_IS_MASTERLEVEL, 
	// End add by Cooler <--
	//-------> Ray [Luoliang] 2005-3-26
	GOI_ADD_ITEM,					//增加一个物品到指定位置
	//uParam = (KUiObjAtContRegion *)
	//nParam = 

	// lixuewu 2005.03.31
	GOI_GET_PILL_INFO,
	//uParam = (tagPillInfo*)
	//nParam =
	//-------> Ray [Luoliang] 2005-4-13
	GOI_CERTIFY_RESULT,				//给服务器发认证结果
	//uParam = 1 图形认证 2 识别码认证
	//nParam = nResult
	//<------- End [Ray]
	GOI_ADD_FRIENDCOUNT,
	GOI_DEC_FRIENDCOUNT,
	GOI_ADD_ONLINE_FRIENDCOUNT,
	GOI_DEC_ONLINE_FRIENDCOUNT,
	//-------> Ray [Luoliang] 2005-7-6
	GOI_MODIFY_PET_NAME,					//给宠物改名字
	//uParam = (KUiObjAtContRegion *)pObj
	//nParam =
	GOI_OPEN_PET_PANEL,				//打开宠物面板
	//uParam =
	//nParam =
	//<------- End [Ray]

	// Add by Cooler -->
	// 2005-7-29
	GOI_IMPEACHCHAT,
	// End add by Cooler <--


    // add by hejianfeng for Anti-Wallow.  2005-10-10
    GOI_ANTI_WALLOW_CTOS, //退出时发给服务端的离线获奖类型,左右见技能ID
    // endadd
    
	
	// --> Rocker Edit Start 2005/09/22
	GOI_SIEGEWEAPON_DLG_OPERATION,
	// uParam = option
	// nParam = 1
	// <-- Rocker End
	
	// --> Rocker Edit Start 2005/10/10 通知core改变了屏幕分辨率
	GOI_NOTIFY_SCREEN_RESIZE,
	// uParam = ScreenWidth
	// nParam = ScreenHeight
	// <-- Rocker End

	// --> Rocker Edit Start 2005/11/14
	GOI_POLYMORPH_DLG_OPERATION, 
	// uParam = option
	// nParam = 1
	// <-- Rocker End

	// --> Rocker Edit Start 2005/11/14
	GOI_SET_BULIDNG_DLG_OPERATION,
	// uParam = &UiSetBuidlingOption
	// nParam = 0
	// <-- Rocker End	

	GOI_GOTO_POS,
	
	GOI_SET_AUTO_DIALOG_NPC,//设置自动寻路完成后要打开哪个NPC
	
	GOI_GOTO_MAILCENTRE,	//打开最近的邮件NPC
	// uParam: npcTemplateIndex

	/********************************************************************************
	 *                Taisui Wheel System  Add by Brianyao2007                      * 
	 ********************************************************************************/
	 GOI_WHEEL_TIAN_GAN,
	 GOI_WHEEL_DI_ZHI,
     GOI_DROP_CHANCE,
	 GOI_TAISUI_SHOW_RES,

	 GOI_PLAY_EFFECT,
	 GOI_SET_EFFECT_POS,
	 //auto operator zpc add//
	 GOI_AUTO_PICKUP,

	 GOI_AUTOATTACK_SWITCH,

	GOI_AUTOATTACK_ITEM_FLAGS,//为自动打怪设置标志位

	GOI_AUTOPICKUP_ITEM_SWITCH,//为自动拾取设置标志

	GOI_AUTOPICKUP_ITEM_FLAGS,//优先拾取贵重装备

	GOI_AUTO_RUN_DISTANCE,   //随机跑距离

	GOI_AUTO_ATTACK_RADIUS,	//攻击半径

	GOI_AUTO_ATTACK_BLAST,	//自动爆魂,nParam = bool

	GOI_AUTO_REPAIR,		//自动修理,nParam = bool

	GOI_AUTO_REPAIR_ERROR_MSG,

	GOI_AUTO_CAST_SKILL,	//设置自动施放技能,uParam = AutoCastSkillInfo &, nParam = index

	GOI_AUTOUSE_ITEM_SWITCH,//自动喝药开关
	GOI_AUTOUSE_ITEM_FLAGS,//自动喝药标志
	GOI_FURY_EXPLODE,
	GOI_AUTO_PICKUP_TYPE_FLAGS,//拾取装备分类
	GOI_AUTO_PICKUP_PICK_MEDICINE,
	GOI_AUTO_SELL_ITEMS,
	GOI_AUTO_FIND_MAP_WAY,//跨地图寻路

	/******************************************************************
	 *                    IBShop System	                              *
	 ******************************************************************/
	GOI_IBSHOP_CHONGZHI,
	GOI_IBSHOP_LOAD_SHELF_CATE,	//读取IB商品分类
	// uParam = ShopIdx

	GOI_IBSHOP_LOAD_SHELF,		//读取一类IB商品
	// uParam = ShelfIdx
	// nParam = ShopIdx

	GOI_IBSHOP_CLEAR_ALL,		//清空IB商店和信用商店

	GOI_IBSHOP_BUY,				//购买IB商品
	// uParam = ClientBuyGoods
	
	GOI_LIST_TEAM,				//列出队伍

	GOI_SOCIAL_OWNER_INFO,      //查找社会关系信息

	GOI_MINI_MAP_DRAW_ON_DC , //在外挂聊天上绘制

	GOI_SPECIAL_QUEST_DATA_RQ , //请求特殊任务数据
	GOI_FORCE_DELETE_NPC_HEADINFO,//断线后强制删除NPC头顶信息

	/********************************************************************
	*								GM									*
	*********************************************************************/

	GOI_SEND_GM_QUESTION,		//发送给GM消息

	/********************************************************************
	*							雇佣									*
	*********************************************************************/
	GOI_HIRE_SEND_DATA_REQ,			//请求Hire Exp数据
	GOI_HIRE_SEND_HIRE_REQ,			//请求雇佣某个人
	GOI_HIRE_SEND_WANT_TO_BE_HIRED_REQ, //想被雇佣
	/********************************************************************
	*							拉新									*
	*********************************************************************/
	GOI_STUDENT_REPORT, //学生每十天一次的报告
	GOI_GET_REWARD, //师父领取奖励

	GOI_GET_SELF_COMBAT_ORG_NAME,

	/********************************************************************
	*							问答									*
	*********************************************************************/
	GOI_ANSWER_QUESTION,//回答问题

	GOI_USE_ITEM_DATE_LOCK,

	/********************************************************************
	*							GM OPERATION							*
	*********************************************************************/
	GOI_KICK_PLAYER,
	GOI_JINYAN_PLAYER,
	GOI_DONGJIE_PLAYER,
	GOI_DONGJIEACCOUNT_PLAYER,
	GOI_CHUANSONG,
	GOI_IP,

	/********************************************************************
	*							称号									*
	*********************************************************************/
	GOI_SELECT_TITLE,//选择称号

	/********************************************************************
	*							积分列表								*
	*********************************************************************/
	// uParam = POINTTYPE
	// nParam = NULL
	GOI_POINTLIST_REQ,

	GOI_REFRESH_SHIZU_POPULARITY,//请求氏族人气排行榜
	GOI_REFRESH_ZHUHOU_POPULARITY,//请求诸侯人气排行榜
	GOI_REFRESH_COMBAT_KILL_RANK,//请求战场击杀排行榜
	GOI_REFRESH_SELF_PROPERTIES,//请求自己属性页属性
	GOI_REFRESH_PLAYER_PROPERTIES,//请求玩家属性页属性
	GOI_ARENA_RANK_INFO_REQ,

	GOI_GET_PLAYER_REAL_INFO,						//用户信息
	GOI_SET_PLAYER_REAL_INFO,
};

//=========================================================
// Core外部客户对core的场景地图相关的操作请求的索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
enum GAME_SCENE_MAP_OPERATION_INDEX
{
	GSMOI_GET_PLAYER_NAME_POS_AT_POS,	//得到某个位置NPC的名字

	GSMOI_GET_MAP_INFO_AT_SCENE_POS,	//得到某个位置的地图信息

	GSMOI_GET_MAP_INFO_AT_MINI_POS,	//得到某个位置的地图信息

	GSMOI_SCENE_TIME_INFO,			//当前主角所处的地域时间环境
	//uParam = (KUiSceneTimeInfo*)pInfo

	GSMOI_SCENE_MAP_INFO,				//当前主角所处的场景的地图信息
	//uParam = (KSceneMapInfo*) pInfo 用于获取信息的结构缓冲区的指针
	//Return = (int)(bool)bHaveMap 返回值表示当前场景是否有小地图。如果返回0值时， pInfo内返回的值无意义

	GSMOI_IS_SCENE_MAP_SHOWING,	//设置场景的小地图是否显示的状态
	//uParam = uShowElem,		//显示哪些内容，取值为SCENE_PLACE_MAP_ELEM枚举的一个或多个的组合。
				//SCENE_PLACE_MAP_ELEM在GameDataDef.h中定义
				//浏览小地图与其它一些项是互斥的
	//nParam = 低16位表示显示的宽度，高16位表示显示的高度（单位：像素点）

	GSMOI_PAINT_MINI_MAP,		//绘制场景的小地图
	GSMOI_PAINT_SCENE_MAP,		//绘制场景的小地图
	//uParam = (int)h 表示绘制起始点在屏幕上横坐标坐标（单位：像素点）
	//nParam = (int)v 表示绘制起始点在屏幕上纵坐标坐标（单位：像素点）

	GSMOI_SCENE_MAP_FOCUS_OFFSET,//设置小地图的焦点偏移（/中心）
	//uParam = (int)nOffsetH	设置小地图焦点的水平坐标偏移（单位：场景坐标）
	//nParam = (int)nOffsetV	设置小地图焦点的垂值坐标偏移（单位：场景坐标）

	GSMOI_SCENE_FOLLOW_WITH_MAP,	//设置场景是否随着地图的移动而移动
	//nParam = (int)nbEnable 场景是否随着地图的移动而移动

	GSMOI_SCENE_CHANGE_MAP,			//更换小地图，nParam = 小地图的编号，
									//-1则表示当前的场景的小地图

	GSMOI_SCENE_MAP_FOCUS,			//设置小地图的焦点（/中心）
	//uParam = (int)nFocusH		设置小地图焦点的水平坐标（单位：场景坐标）
	//nParam = (int)nFocusV		设置小地图焦点的垂值坐标（单位：场景坐标）
	
	GSMOI_SCENE_LITTLE_MAP_INFO,	//当前主角所处的场景的地图信息
	//uParam = (KSceneMapInfo*) pInfo 用于获取信息的结构缓冲区的指针
	//Return = (int)(bool)bHaveMap 返回值表示当前场景是否有小地图。如果返回0值时， pInfo内返回的值无意义

	GSMOI_GET_SCENE_ID,			    //获取场景的ID

	GSMOI_SCENE_LITTLE_MAP_FOCUS,	//设置小地图的焦点（/中心）
	//uParam = (int)nFocusH		设置小地图焦点的水平坐标（单位：场景坐标）
	//nParam = (int)nFocusV		设置小地图焦点的垂值坐标（单位：场景坐标）
	
	// <Add name="Adt.X" time="2005/10/13">
	GSMOI_SCENE_LITTLE_MAP_RECT,	// 获得小地图范围矩形	
	//uParam = (unsigned int)RECT*,		
	//nParam = 
	//Return = (int)(bool)bHaveMap 返回值表示当前场景是否有小地图。如果返回0值时， RECT内返回的值无意义
	// </Add>	

	// --> Rocker Edit Start 2005/11/10
	GSMOI_GET_MINIMAP_CURSOR_INFO,
	// uParam = (KLittleMapCursorInfo*)pInfo
	// nParam = nInfoSize
	// <-- Rocker End

	/*!
	\brief
		Show scene map
	\return
		Nothing.
	*/
	GSMOI_SHOW_SCENE_MAP,

	GSMOI_HAVE_SCENE_MAP,
	/*!
	\brief
		Hide scene map.
	\return
		Nothing.
	*/
	GSMOI_HIDE_SCENE_MAP,

	/*!
	\brief
		Is show the scene map.
	\return
		Is show?(bool).
	*/
	GSMOI_IS_SCENE_MAP_SHOW,

	/*!
	\brief
		Get scene map width.
	\return
		Width(int).
	*/
	GSMOI_GET_SCENE_MAP_WIDTH,

	/*!
	\brief
		Get scene map height.
	\return
		Height(int).
	*/
	GSMOI_GET_SCENE_MAP_HEIGHT,

	GSMOI_GO_TO_POS,
};

//=========================================================
// Core外部客户对core的帮会相关的操作请求的索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
enum GAME_TONG_OPERATION_INDEX
{	
	GTOI_TONG_IS_TONG_MEMBER,		//查询自己是否是某个帮会的成员
	//
	//
	//nRet = 0 (不是帮会成员) 1 (等待自己的申请被通过) 2 (是帮会成员)
	GTOI_TONG_IS_OPERATION_ENABLE,	//查询一个操作是否允许
	//uParam = Cmd Type (用的是诸如招收,踢出,禅让,任命之类的权限的mask)
	//nParam = (const char*) szTargetName	目标名字(如果需要的话)
	//nRet = (bool)能否操作
	GTOI_TONG_CREATE,			//创建帮会
	//uParam = (const char*) pszTongName 帮会的名字
	//nParam = (const char*) szLeaderTitle 帮主称号

	// GTOI_TONG_IS_RECRUIT,		//查询某人的招人开关
	//uParam = (KUiPlayerItme*) 要查谁
	//Return = (int)(bool)		是否开着的招人开关
	
	GTOI_TONG_GET_OPERATE_ID,		//获取指定的信息的操作ID
	//uParam =  uCount(要取得的Item的条数)
	//nParam =  enumTongPageAnnounce			公告
	//          enumTongPageLeader              帮主列表
	//          enumTongPageSages               长老列表
	//          enumTongPageMembers             帮众列表
	//          enumTongPageProxy               代理列表
	//          enumTongPageInfoList            消息列表    
	//          enumTongPageTong                帮会列表
	//          enumTongPageApply               申请列表    
	//			enumTongPageCityList			城市列表
	//返回的操作ID通过CoreDataChanged通知界面, 具体的去看GDCNI_TONG_OPERATE_ID 里面的操作信息
	//TODO 军军自己看着办

	GTOI_TONG_GET_LIST_DATA,		//获取指定的信息数据
	//uParam = (在gamedatadef.h)里面定义的结构,特殊的是帮主列表,长老列表,帮众列表和代理列表,
	//需要指定从nPos位置开始的nCount个数据
	//nParam =  (TONG_REQUEST_LIST_PARAM*)pRequest
	//			其中的type取下面的值 
	//			enumTongPageAnnounce			公告
	//			enumTongPageLeader				帮主列表
	//			enumTongPageSages				长老列表
	//			enumTongPageMembers				帮众列表
	//			enumTongPageProxy				代理列表
	//			enumTongPageInfoList			消息列表	
	//			enumTongPageTong				帮会列表
	//			enumTongPageApply				申请列表
	//			enumTongPageCityList			城市列表
	//返回的操作ID通过CoreDataChanged通知界面, 具体的去看GDCNI_TONG_LIST_DATA 里面的操作信息
	//TODO 军军自己看着办

	GTOI_TONG_RECRUIT,          //招人命令
	//uParam = (const char*)	TargetName 要加入帮会的人的名字
	//nParam = bool bAccept		是否同意招收,无论同意与否都将该人从招收列表中删除

	GTOI_TONG_INSTATE,			//任命
	//uParam = (KTONG_MEMBER_INFO*)	pInfo 要任命的人的名字和要任命的职务,以及给他赋予的称号
	//nParam = (DWORD)dwMask		 任命的权限的组合
	
	GTOI_TONG_DEMISE,			//禅让
	//uParam = (const char*)	要禅让给的人的名字
	//

	GTOI_GET_SELF_TITLE,		//得到自己的称号
	//uParam = (char*)szName;
	//

	GTOI_TONG_EXPEL,			//踢人
	//uParam = (const char*)	TargetName 要踢出的人的名字
	
	GTOI_TONG_LEAVE,			//离开帮会

	GTOI_TONG_TAX,				//调整税率
	//uParam = (unsigned int)uNewTax	调整后的税率

	GTOI_TONG_ADJUST_M2T,			//玩家向帮会存
	//uParam = 玩家向帮会调拨的钱
	//nParam = 玩家向帮会调拨的资源
	
	GTOI_TONG_ADJUST_T2C,			//帮会向城市调拨
	//uParam = 向城市调拨的钱
	//nParam = 向城市调拨的资源
	
	GTOI_TONG_ADJUST_T2M,			//帮会向玩家调拨
	//uParam = 帮会向玩家调拨的钱
	//nParam = 帮会向玩家调拨的资源

	GTOI_TONG_ADJUST_C2T,			//城市向帮会调拨
	//uParam = 城市向帮会调拨的钱
	//nParam = 城市向帮会调拨的资源

	GTOI_TONG_ANNOUNCE,			//设置公告
	//uParam = (const char*)szAnnouce	//公告内容
	//nParam = 公告长度

	GTOI_TONG_POST,				//设置留言
	//uParam = (const char*)szInfo
	//nParam = strlen(szInfo);
	
	GTOI_TONG_GET_SELF_RIGHT_MASK,	//获得自己当前拥有的权限组合
	//
	//
	//nRet = dwSelfMask			自己当前拥有的权限的组合
	GTOI_TONG_GET_CUR_RIGHT_MASK,	//获得自己当前拥有的权限组合(包括代理所得)
	//
	//
	//nRet = dwCurMask
	GTOI_TONG_PROXY,			//让某人代理自己的职务
	//uParam = (const char*)szName
	//nParam = (DWORD)权限的Mask

	GTOI_TONG_UNPROXY,			//解除正在代理人的代理职务
	//
	//
	GTOI_TONG_IS_IN_PROXY,			//当前是否有玩家代理了自己的职务
	//
	//
	//nRet = (bool)bInProxy
	
	GTOI_GET_PROXY_NAME,			//得到代理自己职务的人的名字
	//uParam = (char*)szName
	//
	//
	
	GTOI_GET_PROXY_DUTY,			//得到自己代理的职务的ID(s)
	//uParam = (UI_PROXY_DUTY*)pDuty
	//
	//

//	GTOI_TONG_ACTION,           //对帮内成员做的动作，或自己与帮会的关系的改变
//	//uParam = (KTongOperationParam*) pOperParam 动作时的参数
//	//nParam = (KTongMemberItem*) pMember 指出了操作（帮会成员）对象，
	
	GTOI_TONG_JOIN_APPLY,		//申请加入某个帮会
	//uParam = (const char*)szTongName
	//nParam = strlen(szTongName)
	
	GTOI_TONG_WAR,			//向某个帮会宣战
	//uParam = (const char*)szTongName
	//nParam = strlen(szTongName)

	// <Add name="Adt.X" time="2005/09/05">
	GTOI_TONG_GET_WAR_STATE,	//　得到当前帮会战争状态
	// uParam = 
	// nParam = 	
	// nRet = enWARSTATE
	// </Add>
	
	// <Add name="Adt.X" time="2005/09/06">
	GTOI_TONG_WAR_REPLY,	//回应宣战并可能提前战斗时间
	// uParam = 提前天数(最多2天)
	// nParam = 
	// nRet = 
	// </Add>	

	// <Add name="Adt.X" time="2005/09/28">
	GTOI_TONG_APPOINT_GENERAL,   // 设置司马
	// uParam = (const char*)(MemberName)
	// nParam =	RIGHTID_GENERAL (TongDefine.h)
	// nRet =
	
	GTOI_TONG_REMOVE_GENERAL,	 // 取消司马
	// uParam = NULL
	// nParam = RIGHTID_GENERAL (TongDefine.h)
	// nRet =
	// </Add>	
	
	GTOI_TONG_QUERY,			//查询一个人是否在本帮内
	//uParam = (const char*)szName;
	//nParam = strlen(szTongName);	
	
	GTOI_TONG_PROPERTIES,		//帮会属性
	//uParam = (TONG_PROPERTIES*)tongProperies
	//nParam = 
	
	GTOI_CITY_PROPERIES,		//城市属性
	//uParam = (UI_CITY_INFO*)cityInfo
	//nParam =	

	GTOI_WAR_NUMERICINFO,		//国战数字信息
	
	GTOI_PLAYER_HOLD_RESOURCE,	//玩家身上携带的资源的数量
	//
	//
	//nRet = nResNum
	GTOI_TONG_GET_DUTY_NAME,			//给定一个职务ID, 得到职务的名字
	//uParam = (char*)szName
	//nParam = dwDutyID
	//nRet = (BOOL)是否成功

	GTOI_TONG_GET_DUTY_RIGHTS,	//得到一个职务所拥有的权限
	//uParam = (职务ID)
	//nParam = 
	//nRet = uMask				//返回该职务的权限组合的Mask

	GTOI_TONG_GET_DUTY_RANK,	//得到该职务的Rank
	//uParam = (职务ID)
	//
	//nRet = nRank				
	GTOI_TONG_GET_DUTY_CLASS,	//得到该职务的阶层
	//uParam = (职务ID)
	//
	//nRet = nClass
	
	GTOI_TONG_GET_SELF_DUTY_ID,	//得到自己的Duty ID
	//
	//
	//nRet = uDutyID
	GTOI_TONG_DISMISS,			//解散帮会
	//
	//
	GTOI_TONG_OWN_CITY,			//是否有城市
	//
	//
	//nRet = (BOOL)bOwnCity
	GTOI_TONG_OPEN_CHANNEL_NOTIFY,		//通知服务器打开帮会聊天频道
	//
	//
	//

	GTOI_TONG_SELF_CONTRIBUTION,		//得到自己的贡献度
	//
	//
	//nRet = nContribution
	//<---- Add By Ray [Luoliang] [2005-9-20]
	GTOI_TECH_CATEGORY_REQUEST,
	//uParam = (int)nCategory
	//nParam = 
	GTOI_UPGRADE_WAR_TECHNOLOGY,
	//uParam = (tagTechNodeIdx*)pNodeIdx
	//nParam
	GTOI_CANCEL_UPGRADE_TECHNOLOGY,	
	//uParam = (tagTechNodeIdx*)pNodeIdx
	//nParam
	// End. Ray [LuoLiang] [2005-9-20] ---->
	//<---- Add By Ray [Luoliang] [2005-9-26]
	GTOI_REQUEST_ALL_WAR_MEMBER,			// 请求所有国家成员的列表
	//uParam = (tagWarRequestMemberList*)pRequest
	//nParam =
	GTOI_REQUEST_ARMY_LIST,					//请求当前的所有军团的index列表
	//uParam = dwOperID
	//nParam =
	GTOI_REQUEST_ARMY_MEMBER_LIST,			//请求某个军团的成员列表
	//uParam = dwOperID
	//nParam = nAramyIndex
	GTOI_REQUEST_MEMBER_INFO,				//请求某个成员
	//uParam =
	//nParam = nMemberID
	GTOI_SEND_ARMY_FORM_CMD,				//向服务器发送军团编制命令
	//uParam = (tagArmyFormCmd*)pCmd
	//nParam = 0
	// End. Ray [LuoLiang] [2005-9-26] ---->
	GTOI_CREATE_CORPS,						//建立一个军团
	//uParam = uCorpsIndex
	//nParam = (char*)szCorpsName
	
	GTOI_MODIFY_CORPS_NAME,					//修改军团的名字
	//uParam = uCorpsIndex
	//nParam = (char*)szCorpsName

	GTOI_DELETE_CORPS,
	//uParam = uCorpsIndex
	//nParam = 
	//<---- Add By Ray [Luoliang] [2005-10-16]
	GTOI_GET_WAR_TIME_ELAPSED,			//取得当前战争经过的时间
	//uParam =
	//nParam =
	//nRet = (DWORD) dwTimeElasped
	GTOI_GET_WAR_TIME_LEVEL,				//取得当前战争的进度
	//uParam = 
	//nParam =
	GTOI_GET_WAR_TIME_LEVEL_COUNT,			//得到战争时间分段的段数
	//uParam = 
	//nParam =
	//nRet = nCount
	GTOI_GET_WAR_TIME_LEVEL_VALUE,			//得到某段战争时间分段的值
	//uParam = uLevelIndex
	//nParam =
	//nRet = (DWORD)dwTimeValue
	GTOI_CAN_UPGRADE_TECH_LEVEL,			//能否升级该等级的科技
	//uParam = ( tagTechNodeIdx * ) pIndex
	//nParam = 
	GTOI_WAR_GET_COMMANDOR_REQUEST,			//取得当前的指挥官
	//uParam = 
	//nParam =
	// End. Ray [LuoLiang] [2005-10-16] ---->

	// Add by Cooler -->
	// 2005-9-28
	// return in nRet
	GTOI_WAR_ISINWAR, 
	// uParam = dwNpcID
	// return in nRet
	GTOI_WAR_ISHAVERIGHT, 
	// return in nRet
	GTOI_WAR_ISMASTER, 
	// return in nRet
	GTOI_WAR_ISCAPTAIN, 
	// return in nRet, LPCSTR pointer
	GTOI_WAR_GETGROUPNAME, 
	// return in nRet, enWARDUTYTYPE
	CTOI_WAR_GETDUTY, 
	GTOI_WAR_SYNCMINESTATUS, 
	// End add by Cooler <--

	//<---- Add By Ray [Luoliang] [2005-10-13]
	GTOI_WAR_KICK,							//将一个玩家从战场中踢出
	//uParam = uMemberIndex
	//nParam =
	GTOI_WAR_DEMISE,						//将指挥权交给某个万家
	//uParam = uMemberIndex
	//nParam =
	
	// Add by Cooler -->
	// 2005-10-26
	//uParam = WAR_ADDCOMMAND_APPLY*
	//nParam =
	GTOI_WAR_ADDCOMMOND, 
	//uParam = WAR_DELCOMMAND_APPLY*
	//nParam =
	GTOI_WAR_DELCOMMOND, 
	// End add by Cooler <--
	//<---- Add By Ray [Luoliang] [2005-10-27]
	GTOI_WAR_ARMY_INVITE,				//邀请加入军团
	//uParam = uTargetID
	//nParam = 
	GTOI_WAR_ARMY_INVITE_REPLY,			//对加入军团的邀请的回应
	//uParam = (BOOL)bAgree
	//nParam =
	// End. Ray [LuoLiang] [2005-10-27] ---->

	//Lucifer~yu[zhangjianyu] [12/28/2005] Add for 取得当前选中国战科技
	//begin------------------------------------------------------------------------
	GTOI_WAR_TECH_GET_SELECT_TECH,
	//uParam = 
	//nParam =
	//end--------------------------------------------------------------------------	

	//Lucifer~yu[zhangjianyu] [12/29/2005] Add for 是否为士兵 
	//begin------------------------------------------------------------------------
	GTOI_WAR_ISSOLDIER,
	//uParam = 
	//nParam =
	//nRet	 = (bool)IsSoldier
	//end--------------------------------------------------------------------------	

	// <Add name="Adt.X" time="2006/01/10">
	// 获得国王雕像留言.
	GTOI_GET_MASTER_MESSAGE,
	//uParam = 
	//nParam =
	//nRet	 = 
	// </Add>	

	// <Add name="Adt.X" time="2006/03/15">
	// 获得战争双方名字
	GTOI_GET_JOIN_WARINFO,
	//uParam = 
	//nParam =
	//nRet	 = 
	// </Add>
	
	// <Add name="Adt.X" time="2006/01/11">	
	GTOI_TONG_GET_WAR_EXP,					// 得到战争积分.
	// uParam = 
	// nParam =
	// nRet = 积分

	GTOI_TONG_GET_WAR_EXP_CONSUME,			// 得到战争已消耗积分累积.  
	// uParam = 
	// nParam =
	// nRet = 已消耗积分累积.  
	
	GTOI_TONG_GET_WAR_EXP_CONSUME_TITLE,	// 得到战争积分消耗等级.  
	// uParam = 
	// nParam =
	// nRet = 战争积分消耗等级
	// </Add>

	//Lucifer~yu[zhangjianyu] [01/11/2006] Add for 发送国王雕像留言
	//begin------------------------------------------------------------------------
	GTOI_SEND_MASTER_MESSAGE,
	//uParam = (char*)Message
	//end--------------------------------------------------------------------------	
};

//=========================================================
// Core外部客户对core的组队相关的操作请求的索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
enum GAME_TEAM_OPERATION_INDEX
{
	/************************************************************************/
	/*				Information request                                      */
	/************************************************************************/
	/*!
	\brief
		主角所在的队伍信息

	\param 
		(KUiPlayerTeam*)pTeam -> 队伍信息，为空则不取
	
	\return
		如果为非0值表示主角在队伍中，pTeam结构被填充信息。如果为0值表示主角不在队伍中，pTeam结构未被填充有效信息。
	*/
	TEAM_OI_GD_INFO,				

	/*!
	\brief
		获取拒绝邀请的状态
	
	\return
		(int)bEnableRefuse 为真值表示拒绝状态生效，否则表示不拒绝。
	*/	
	TEAM_OI_GD_REFUSE_INVITE_STATUS,

	/************************************************************************/
	/*				Operation request                                       */
	/************************************************************************/

	/*!
	\brief
		申请加入她人队伍

	\param 
		(KUiTeamItem*)	要申请加入的队伍的信息
	
	\return
		Nothing.
	*/
	TEAM_OI_APPLY,				

	/*!
	\brief
		新组队伍

	\param 
		nParam = 经验共享方式　SHARE_EXP_CLS = 1, SHARE_EXP_AVG = 2
	
	\return
		Nothing.
	*/
	TEAM_OI_CREATE,				

	TEAM_OI_OPEN_BIG_TEAM_MODE,//开启大队伍模式

	/*!
	\brief
		任命队长，只有队长调用才有效果

	\param 
		(KUiPlayerItem*)pPlayer -> 新队长的信息 KUiPlayerItem::nData = 0
	
	\return
		Nothing.
	*/
	TEAM_OI_APPOINT,			

	/*!
	\brief
		邀请别人加入队伍，只有队长调用才有效果

	\param 
		uParam = (KUiPlayerItem*)pPlayer -> 要邀请的人的信息KUiPlayerItem::nData = 0
	
	\return
		Nothing.
	*/
	TEAM_OI_INVITE,

	/*!
	\brief
		邀请别人加入队伍（按玩家名字），只有队长调用才有效果

	\param 
		uParam = (KUiPlayerItem*)pPlayer -> 要邀请的人的信息KUiPlayerItem::nData = 0
	
	\return
		Nothing.
	*/
	TEAM_OI_INVITE_BY_NAME,

	/*!
	\brief
		踢除队里的一个队员，只有队长调用才有效果

	\param 
		uParam = (KUiPlayerItem*)pPlayer -> 要踢除的队员的信息 KUiPlayerItem::nData = 0
	
	\return
		Nothing.
	*/
	TEAM_OI_KICK,				

	/*!
	\brief
		离开队伍

	\return
		Nothing.
	*/
	TEAM_OI_LEAVE,

	//解散队伍
	TEAM_OI_DISMISS,
	
	/*!
	\brief
		拒绝别人邀请自己加入队伍

	\param 
		nParam = (int)(bool)bEnableRefuse 为真值表示拒绝状态生效，否则表示不拒绝。
	
	\return
		Nothing.
	*/
	TEAM_OI_REFUSE_INVITE,

	/*!
	\brief
		对组队邀请的回复

	\param 
		uParam = (KUiPlayerItem*)pTeamLeader 发出组队邀请的队长
		nParam = (int)(bool)bAccept 是否接受邀请
	\return
		Nothing.
	*/
	TEAM_OI_INVITE_RESPONSE,

	//回复申请加入队伍
	TEAM_OI_APPLY_RESPONSE,
	
	/*!
	\brief
		获得队员的信息

	\return
		Nothing.
	*/	
	TEAM_OI_MEMBER_INFO,	

	TEAM_OI_MOVE_INDEX,//移动队员顺序

	TEAM_IO_PROMOTE_ASSISTANT,//提升助手

	TEAM_IO_DISMISS_ASSISTANT,//撤职助手

	TEAM_OI_APPLY_JOIN,//申请加入（按对方NpcID）

	TEAM_OI_APPLY_JOIN_BY_NAME,//申请加入（按对方名字）

	TEAM_IO_SET_AUTO_ACCEPT_APPLY,//设置自动接受组队请求
};

enum GAME_CITY_OPERATION_INDEX			//城市操作
{
	GAME_BUILD_GET_ALL_TYPE_COUNT = 0,	//取得所有建筑数目
	//返回所有建筑数目
	GCOI_BUILD_GET_BUILD_TYPE_COUNT,	//可建设的建筑总类
	//返回建筑种类个数	
	GCOI_BUILD_GET_TOTEM_TYPE_COUNT,	//图腾总数
	// 返回图腾总数
	GCOI_BUILD_GET_PLANT_TYPE_COUNT,	//植被总数

	//建筑的简要信息
	GCOI_BUILD_ALL_INFO,
	//uParma = 建筑信息结构 UIBuildingInfo
	//nParam = 建筑类型
	//返回是否成功
	GCOI_BUILD_CANBUILD_INFO,
	//uParma = 建筑信息结构 UIBuildingInfo
	//nParam = 建筑类型
	//返回是否成功
	GCOI_BUILD_TOTEM_INFO,
	//uParma = 建筑信息结构 UIBuildingInfo
	//nParam = 建筑类型
	//返回是否成功
	GCOI_BUILD_PLANT_INFO,
	//uParma = 建筑信息结构 UIBuildingInfo
	//nParam = 建筑类型
	//返回是否成功
	//建筑的鼠标悬停提示信息
	GCOI_BUILD_ALL_TIPS,
	//uParam = 建筑提示信息结构 UIBuildingTip
	//nParam = 建筑类型
	//返回是否成功
	GCOI_BUILD_CANBUILD_TIPS,
	//uParam = 建筑提示信息结构 UIBuildingTip
	//nParam = 建筑类型
	//返回是否成功
	GCOI_BUILD_TOTEM_TIPS,
	//uParam = 建筑提示信息结构 UIBuildingTip
	//nParam = 建筑类型
	//返回是否成功
	GCOI_BUILD_PLANT_TIPS,
	//uParam = 建筑提示信息结构 UIBuildingTip
	//nParam = 建筑类型
	//返回是否成功
	
	//建筑物图标
	GCOI_BUILD_ALL_ICON,
	//uParam = 建筑图标信息结构 UIBuildingIcon
	//nParam = 建筑类型
	//返回是否成功
	GCOI_BUILD_CANBUILD_ICON,
	//uParam = 建筑图标信息结构 UIBuildingIcon
	//nParam = 建筑类型
	//返回是否成功
	GCOI_BUILD_TOTEM_ICON,
	//uParam = 建筑图标信息结构 UIBuildingIcon
	//nParam = 建筑类型
	//返回是否成功
	GCOI_BUILD_PLANT_ICON,
	//uParam = 建筑图标信息结构 UIBuildingIcon
	//nParam = 建筑类型
	//返回是否成功
	
	GCOI_TERRAIN_GET_TYPE_COUNT,		//地形种类个数
	// 返回地形总数
	GCOI_TERRAIN_INFO,					//取的地形的描述
	//uParam = 地形描述结构 UIBuildingTip
	//nParam = 地形类型

	GCOI_GET_CITY_INFO,						//取城市信息命令
	//
	//返回的结果通过CoreDataChanged通知界面

	GCOI_CREATE_CITY,						//创建城市,
	//uParam = (UI_CITY_INFO*)pParam
	//
	//只使用其中的szCityName, btTerrainType, btTotemTpye
	//具体见GameDataDef.h
	GCOI_ENTER_CITY,						//进入城市,
	//uParam = (const char*)szCityName
	//
	GCOI_GET_TECH_TIP,						//取得科技的描述
	//uParam = 科技id nParam 科技等级 返回nRet=(int)(const char* tip)
};


/*!
\brief
	游戏世界数据改变的通知函数原型
*/
struct IClientCallback
{
	virtual void CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam) = 0;
};

struct _declspec (novtable) iCoreShell
{
	virtual int				FindNpcIndexById		( DWORD npcId																																				) = 0;
	virtual	int				ChangeUiLoginDispNpcDir	( int nDir																																					) = 0;
	virtual	int				PlayUiLoginDisplayerNpc	(																																							) = 0;
	virtual	int				StopUiLoginDisplayerNpc	(																																							) = 0;
	virtual	int				ChangeUiLoginDispNpcStat( int nState																																				) = 0;
	virtual	int				SetUiLoginDisplayerNpc	( UI_DISPLAYER_NPC_SYNC* pNpc																																) = 0;
	virtual	int				GetProtocolSize			( BYTE byProtocol																																			) = 0;
	virtual int				OperationRequest		( unsigned int uOper, unsigned int uParam, int nParam																										) = 0;
	virtual void			ProcessInput			( unsigned int uMsg, unsigned int uParam, int nParam																										) = 0;
	virtual int				GetTargetNPC			( void																																						) =	0;
	virtual int				FindSelectNPC			( int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind, bool bSearchSelf = false, bool bIsLeft = true , bool bNoSelectPlayer = false        ) = 0;
	virtual int				AutoSelectNPC			( int nRelation, int nMouseX, int nMouseY																																) = 0;
	virtual bool			SelectNPC				( int nIdx																																					) = 0;
	virtual int				FindSpecialNPC			( char* Name, void* pReturn, int& nKind																														) = 0;
	virtual int				FindSelectObject		( int x, int y, bool bSelect, int& nObjectIdx, int& nKind																									) = 0;
//	virtual void			DialogNpc				(																																							) = 0;
	virtual int				ChatSpecialPlayer		( void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength																							) = 0;
	virtual void			TradeApplyStart			( int targetIndex																																			) = 0;
//	virtual int				UseSkill				( KSkillData skillData																																		) = 0;
	virtual int				LockSomeoneAction		( int nTargetIndex																																			) = 0;
//	virtual int				LockObjectAction		( int nTargetIndex																																			) = 0;
 	virtual void			GotoWhere				( int x, int y, int mode ,bool bDelay = false																												) = 0;	//mode 0 is auto, 1 is walk, 2 is run
	virtual void			Turn					( int nDir																																					) = 0;	//nDir 0 is left, 1 is right, 2 is back
	virtual int				ThrowAwayItem			( int nItemIndex																																			) = 0;
	virtual int				GetNPCRelation			( int nIndex																																				) = 0;
	virtual int				SceneMapOperation		( unsigned int uOper, unsigned int uParam, int nParam																										) = 0;
	virtual int				TeamOperation			( unsigned int uOper, unsigned int uParam, int nParam																										) = 0;
	virtual int				BuildingOperation		( unsigned int uOper, unsigned int uParam, int nParam																										) = 0;
	virtual int				GetGameData				( unsigned int uDataId, unsigned int uParam, int nParam																										) = 0;
	virtual void			DrawGameObj				( unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam																	) = 0;
	virtual void			DrawGameSpace			( void																																						) = 0;
	virtual void            DrawUiEffect            ( void) = 0;
	virtual void			BreatheGameSpace		( void																																						) = 0;
	virtual	void			PaintBehindUi			( void																																						);
	virtual DWORD			GetPing					( void																																						) = 0;
	virtual int				SetCallDataChangedNofify( IClientCallback* pNotifyFunc																																) = 0;
	virtual void			NetMsgCallbackFunc		( void* pMsgData																																			) = 0;
	virtual void			SetRepresentShell		( struct iRepresentShell* pRepresent																														) = 0;
	virtual void			SetMusicInterface		( void* pMusicInterface																																		) = 0;
	virtual void			SetRepresentAreaSize	( int nWidth, int nHeight																																	) = 0;
	virtual int				Breathe					( void																																						) = 0;
	virtual void			Release					( void																																						) = 0;
	virtual void			SetClient				( LPVOID pClient,int nConnectID																																) = 0;
	virtual void			SendNewDataToServer		( void* pData, int nLength																																	) = 0;
	virtual int				InsertItem				( int nGenre, int nDetail, int nParticular, int nLevel, int nSeries, int nLuck, int* pMagicLevel, int nVersion, int nRandSeed								) = 0;
	virtual void			InitSimplifiedNpc		( BOOL bSimplifiedNpc																																		) = 0;
	virtual void			EnableSimplifiedNpc		( BOOL bSimplifiedNpc																																		) = 0;
	virtual void			BegingBuildBuilding		( unsigned int utype, unsigned int nDataIdx																													) = 0;
	virtual void			TryToBuildBuilding		( unsigned int nX, unsigned int nY																															) = 0;
	virtual void			EndBuildBuilding		( void																																						) = 0;
	virtual void			ValidateBuildBuilding	( BOOL bCancel																																				) = 0;
	virtual void			GetPlayerPos			( int &nGridX, int &nGridY																																	) = 0;
	virtual BOOL			IsItemListLocked		( void																																						) = 0;
	virtual int				GetHandItemIndex		( void																																						) = 0;
	virtual BOOL			FindPlacePos			( POINT* pPos																																				) = 0;
	//virtual BOOL			RSGetWannaData			( unsigned int uGetIndex, const void *pInData, void *pOutData																								) = 0;
	virtual void			FindItemIndex			( int nGenre, int nDetail, int nParticular, int *pnIdx																										) = 0;
	virtual BOOL			IsTextPass				( const char *szText) = 0;
	virtual BOOL			IsNamePass				( const char *szText) = 0;
//	virtual void			SwitchDefaultSkill		( void				) = 0;
	virtual void			DrawMovePosition		( int x, int y		) = 0;
	virtual void			RemoveMovePosition		( void				) = 0;
	virtual	void			SetMusic				( bool b			) = 0;
	virtual void			Move					( int direction, int distance ) = 0;

	virtual void			SelectSkill				( int skillId ) = 0;
	virtual void			NextSkill				( int skillId, bool targetSelf = false ) = 0;
//	virtual void			UseSelectedSkill		( int targetSelf = false ) = 0;
	virtual void			FollowAttack			( void ) = 0;
	virtual void			FollowDialog			( void ) = 0;
	virtual void			PickupObject			( int objectIndex ) = 0;
	virtual void			Stop					( void ) = 0;

	virtual void			ReplyPrompt				( enumPromptEvent promptEvent, bool accept ) = 0;

	virtual bool			CoreParseQuestionProtocol	( BYTE* pMsg, UIQuestionData& uiQuestionData ) = 0;
};


#ifndef CORE_EXPORTS

	//获取iCoreShell接口实例的指针
	extern "C" iCoreShell* CoreGetShell();

#else

	//对外发送游戏世界数据发生改变的通知
	void	CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam);

#endif

struct _UseItem 
{
	unsigned int uId;
	int			 nPlace;
	int			 nX;
	int			 nY;
};

typedef struct
{
	int					nPlace;
	int					nX;
	int					nY;
	unsigned int		uGener;
	unsigned int		uId;
} _ItemPos;

extern _UseItem 	g_UseItem;
extern int			g_uItemID;
extern _ItemPos		g_uItemPos;
void ConfirmUseItem();
void CannelUseItem();

extern int		g_nLockItemByDateIdx;
void LockItemByDate();

#endif
