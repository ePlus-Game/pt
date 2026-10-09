/*****************************************************************************************
//	外界访问Core用到数据结构等的定义
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-12
------------------------------------------------------------------------------------------
	一些定义可能处于在游戏世界各模块的头文件中，请在此处包含那个头文件，并请那样的头文件
不要包含一些与游戏世界对外无关的内容。
    开发过程中游戏世界的外部客户在未获得游戏世界接口完整定义的情况下，会现先直接在此文件
定义它需要的数据定义，游戏世界各模块可根据自身需要与设计把定义作修改或移动到模块内的对外
头文件，并在此建立充要的包含。
*****************************************************************************************/
#ifndef GAMEDATADEF_H
#define GAMEDATADEF_H

#include "CoreObjGenreDef.h"
#include "CoreUseNameDef.h"
#include "Int64.h"
#ifndef _WIN32
#include "string.h"
#endif

#include <vector>
#include "cfs_common_def.h"
#include "SkillDef.h"

#include <string>

#ifdef _SERVER
extern unsigned int g_GuidPadding; // GUID填充
#else
#include "IQueryInfo.h"
#endif

#ifdef WIN32
typedef __int64 INT64;
typedef unsigned __int64 UINT64;
#else
typedef long long INT64;
typedef unsigned long long UINT64;
#endif

//FSGUID
typedef struct tagFSGUID
{
	tagFSGUID()
	{
		memset(data, 0, sizeof(data));
	}

	tagFSGUID( int nInitValue )
	{
		memset(data, nInitValue, sizeof(data));
	}

	tagFSGUID( const char* szGUID )
	{
		if (szGUID != NULL)
		{
			memcpy(data, szGUID, sizeof(data));
		}
	}

	tagFSGUID& operator= (const tagFSGUID &rhs)
	{
		if(this != &rhs)
		{
			memcpy(data, rhs.data, sizeof(data));
		}

		return *this;
	}

	bool operator< (const tagFSGUID &rhs) const
	{
		return memcmp(this, &rhs, sizeof(rhs)) < 0;
	}

	bool operator== (const tagFSGUID &rhs) const
	{
		return (memcmp(data, rhs.data, sizeof(data)) == 0) ? true : false;
	}

	bool operator!= (const tagFSGUID &rhs) const
	{
		return (memcmp(data, rhs.data, sizeof(data)) == 0) ? false : true ;
	}

	char data[33];
}FSGUID, *PFSGUID;

#define MAXSIZE_ITEMNAME		32
#define MAXSIZE_DES				1024
#define MAXSIZE_ROLETYPENAME	32
#define MAXSIZE_DETAILDESC		1024
#define CLIENT_NAME_AND_TITLE_MAX 16

struct UI_DISPLAYER_NPC_SYNC
{
	BYTE	m_Doing;			// 行为
	BYTE	m_btKind;			// npc类型
	int		nLevel;
	int		NpcSettingIdx;		// 客户端用于加载玩家资源及基础数值设定
	int		nWeapon;
	int		nHelm;
	int		nArmor;			
	int		nShoulder;
	int		nCuff;
	int		nBoot;
	int		nHorse;
	bool	bRideHorse;
};

enum NPCCMD
{
	do_none,		// 什么也不干
	do_stand,		// 站立
	do_walk,		// 行走
	do_run,			// 跑动
	do_jump,		// 跳跃
	do_skill,		// 发技能的命令
	do_magic,		// 施法
	do_attack,		// 攻击
	do_sit,			// 打坐
	do_hurt,		// 受伤
	do_death,		// 死亡
	do_defense,		// 格挡
	do_idle,		// 喘气
	do_specialskill,// 技能控制动作
	do_special1,	// 特殊1
	do_summonskill,	// 
	do_special3,	// 特殊3
	do_special4,	// 特殊4
	do_runattack,
	do_manyattack,
	do_jumpattack,
	do_revive,
	do_stall,
};

enum enPlayerLogoutOpe
{
	enPLO_Logout = 0,
	enPLO_StartTimer,
	enPLO_StopTimer,
};

/************************************************************************/
/*					Channel system                                      */
/************************************************************************/

struct Ui_Channel_Param 
{
	int dwChannelID;
	char szChannelName[COMMON_CLIENT_MSG_LEN_16];

public:
	Ui_Channel_Param(){	}
	Ui_Channel_Param(const Ui_Channel_Param& other)
	{
		dwChannelID = other.dwChannelID;
		strcpy(szChannelName, other.szChannelName);
	}
};

/************************************************************************/
/*					Change Map param                                    */
/************************************************************************/

enum ChangeMapParam {
	defaultMap = -2,
	randomMap = -1,
};

/************************************************************************/
/*					tong system		                                    */
/************************************************************************/
struct TongOperParam 
{
	TongOperParam()
	{
		nTemplateID = 0;
		nLayerID = 0;
		nOperationID = 0;
		szName[0] = 0;
		szReceiveName[0] = 0;
		szTip[0] = 0;
	}
	TongOperParam( const TongOperParam& rOper )
	{
		nTemplateID = rOper.nTemplateID;
		nLayerID = rOper.nLayerID;
		nOperationID = rOper.nOperationID;
		memcpy( szName, rOper.szName, CLIENT_NAME_AND_TITLE_MAX );
		szName[CLIENT_NAME_AND_TITLE_MAX - 1] = 0;
		memcpy( szReceiveName, rOper.szReceiveName, CLIENT_NAME_AND_TITLE_MAX );
		szReceiveName[CLIENT_NAME_AND_TITLE_MAX - 1] = 0;
		memcpy( szTip, rOper.szTip, COMMON_CLIENT_MSG_LEN_512 );
		szTip[COMMON_CLIENT_MSG_LEN_512 - 1] = 0;
		id = rOper.id;
	}
	int nTemplateID;
	int nLayerID;
	int nOperationID;
	int		nPage;
	char			 szName[CLIENT_NAME_AND_TITLE_MAX];
	char			 szReceiveName[COMMON_CLIENT_MSG_LEN_128];
	char			 szTip[COMMON_CLIENT_MSG_LEN_512];
	FSGUID	id;
};

#define TONGMEMBER_COUNT_PER_PAGE 20
#define TONGMEMBER_MAX_NUM        20
#define MAX_PREVENT_SATE_NUM      3

struct TongPageData 
{
	char	guid[sizeof(FSGUID)];
	char	szName[CLIENT_NAME_AND_TITLE_MAX];
	int		nLevel;
	int		nMetier;
	bool	bOnline;
	int     nTopOwnerLayer;
    bool    bPreventChatState[MAX_PREVENT_SATE_NUM];
	//Tong layer used
	char    szCityName[COMMON_CLIENT_MSG_LEN_16 + 1];
	char    szPoolName[COMMON_CLIENT_MSG_LEN_16 + 1];
	int     nSubUnitNum;
};

//社会关系查询结果回显的UI标识
typedef unsigned long TONG_INFO_UI_IDENTI_FLAG;

#define TIUI_SOCIAL_INFO   0x00000001          //UiSocialInfo
#define TIUI_EX_CHAT       0x00000002          //外挂聊天

struct TongUIReqeustCode
{
    TONG_INFO_UI_IDENTI_FLAG m_IdentiyFlag;
	DWORD                    m_ExtraFlag;
};

struct TongInfoRequestParam
{
	char              szOwnerName[COMMON_CLIENT_MSG_LEN_16 + 1];
	int               nLayer;
	TongUIReqeustCode code;
};

struct TongInfoData
{
	char    szUnitName [COMMON_CLIENT_MSG_LEN_16 + 1];
	char    szOwnerName[COMMON_CLIENT_MSG_LEN_16 + 1];
	int     nSubUnitNum;
	int     nLayer;
	   
	int     nPlayerAvgLevel; //shizu only

	char    nCityMapName[COMMON_CLIENT_MSG_LEN_32];      //zhuhou only
	char    nPoolMapName[COMMON_CLIENT_MSG_LEN_32];      //Zhuhou only 

	TongUIReqeustCode flag;
};

struct TongRecruitPageData
{
    char                     szUnitName [COMMON_CLIENT_MSG_LEN_32];
	char                     szOwnerName[COMMON_CLIENT_MSG_LEN_32];
	int                      nSubUnitCount;
    bool                     bOwnerOnline;

	char                     szCityMapName[COMMON_CLIENT_MSG_LEN_32];
	char                     szPoolMapName[COMMON_CLIENT_MSG_LEN_32];

	int                      nDueTimeDay;
};

#define TONG_RECRUIT_DATA_UI_PAGE_NUM  10  //Notcie Keep the same with SocialComDef

struct TongRecruitData
{
	int                      nCurPageNum;
	int                      nPageNo;
    int                      nLayer;
	TongRecruitPageData      nPageData[TONG_RECRUIT_DATA_UI_PAGE_NUM];
};

enum SocietyClientOper
{
	get_society_baseinfo_name,
	get_society_baseinfo_info,
	get_society_baseinfo_count,
	get_society_memberlist_operation,
	get_society_shizulist_operation,
	get_society_zhuhoulist_operation,

};

struct TongData 
{
	struct _Count
	{
		int nCount;
		int nMaxPlayerCount;
		int nOnlinePlayerCount;
	};
	int nTemplateID;
	int nLayerID;
	int nOperationServerID;
	SocietyClientOper eOperationClientID;
	char szReceiveName[COMMON_CLIENT_MSG_LEN_128];
	char szCityName[COMMON_CLIENT_MSG_LEN_16];
	char szPoolName[COMMON_CLIENT_MSG_LEN_16];
	char szOwnerName[COMMON_CLIENT_MSG_LEN_16 + 1];
	_Count tagCount;
	union{
			char szTip[COMMON_CLIENT_MSG_LEN_512];
			char szName[CLIENT_NAME_AND_TITLE_MAX];
			TongPageData memberList[TONGMEMBER_MAX_NUM];
			char groupList[TONGMEMBER_COUNT_PER_PAGE][CLIENT_NAME_AND_TITLE_MAX];
	};
	
};

/************************************************************************/
/*					City system                                         */
/************************************************************************/

enum CITY_OPER_TYPE
{
	contribute_city_res_oper,
	get_city_res_oper,
	repair_city_building_oper,
};

struct CityResParam 
{
	int nCopperCount;
	int nFlixCount;
	int nWoodCount;
	int nMoney;
};

struct CityOperParam
{
	CITY_OPER_TYPE eType;
	union {
		int nBuildID;
		CityResParam tagCityRes;
	};
};

enum CityInfoType
{
	base_info_city,
	building_info_city,
};

#define BUILDING_PER_CITY 10

struct CityBuildingInfo 
{
	int	 nBuildID;
	char szBuildingImageSet[CLIENT_NAME_AND_TITLE_MAX];
	char szBuildingImage[CLIENT_NAME_AND_TITLE_MAX];
	char szBuildingName[CLIENT_NAME_AND_TITLE_MAX];
	char szBuildingInfo[COMMON_CLIENT_MSG_LEN_256];
};

struct CityBaseInfo
{
	//City base info.
	char szZhuhouName[COMMON_CLIENT_MSG_LEN_16+1];
	char szKingName[CLIENT_NAME_AND_TITLE_MAX];
	int	nShizuCount;
	int nProsonCount;
	//City storage info.
	int nCopperCount;
	int nFlixCount;
	int nWoodCount;
	int nMoney;
	//City tree info.
	int nCreateCopperCount;
	int nCreateFlixCount;
	int nCreateWoodCount;
	//City maintenance info
	int nExpendCopperCount;
	int nExpendFlixCount;
	int nExpendWoodCount;
	//City tax rate info 
	int nTaxRate;
	char szMaintenanceInfo[COMMON_CLIENT_MSG_LEN_128];
	int nDevelopment;
	int nTiredness;
};

struct CityInfoParam  
{
	int eType;
	union{
		CityBaseInfo tagBaseInfo;
		CityBuildingInfo tagBuilding[BUILDING_PER_CITY];
	};
};

#pragma pack(push, 1)

struct KImmediacyParam
{
	BYTE nImmediacyType;
	BYTE nPos;
	int nID;
};

#pragma pack(pop)

#define COMMON_NAME_LENGTH 32
#define COMMON_DESC_LENGTH 128

#define ITEM_BUFF_COUNT	3
#define ITEM_PROP_REQ_COUNT 6

enum{
		login_error_message = 0,
		role_description_message,
		compound_error_message,
		storebox_error_message,
		overweight_error_message,
		exchange_error_message,
		createtong_error_message,
		tong_operation_message,
		castbar_message,
		talisman_message,
		trade_box_message,
		quest_message,
		smith_message,
		cast_bar_message,
		common_message,
		connection_state_message,
		help_edit_message,
		auto_connect,
		delete_comfirm,
		levelup_info,
		quit_delay,
		pk_status_message,
		mail_message,
		friend_message,
		changemap_message,
		vendue_message,
		minitoolbar_message,
		team_message,
		taisui_message,
		elfpop_menu = 30,
		compound_rule_fit_message,
		compound_tip_message,
		compound_rst_message,
		ibshop_message = 39,
		social_info = 40,
		helpTip_message = 50,
		hire_op_message = 51,
		time_message = 52,
		use_yibu_item,
		ib_use,
		serverlist_message,
		infobar_message,
		battlefield_message,
		entrustComputerTip_message,
		not_enough_money_for_repair,
		report_message,
		title_info_message,
	};

/*!
\brief
	组合装备的方式
*/
enum COMPOUNDTYPE
{
	COMPOUND_INVALID = -1,									//!< 没有对应的合成方法
	COMPOUND_LEVELUP = 0,
	COMPOUND_ADDMAGIC,
	COMPOUND_CLEAR,
	COMPOUND_ADDYAO,
	COMPOUND_GETYAO,
	COMPOUND_MAKE,
	COMPOUND_SMITH,
	COMPOUND_COUNT,
};

#define MAX_LEVELUP_ITEMS_COUNT 26
struct KUiCompoundParam
{
	int nMoney;
	int nCompoundType;
	int ruleId;
	int	nItemIndex[MAX_LEVELUP_ITEMS_COUNT];		//装备的index数组
	char szPlusInfo[COMMON_CLIENT_MSG_LEN_64];
};

struct CompoundInitMaterial
{
	int			money;
	int			yunhun;
	int			compoundType;
	int			targetItemIndex;
	std::vector<int>	sourItemIndex;
};

struct ItemType
{
	int genre;
    int detail;
    int particular;
	int	level;

	ItemType()
	{
		genre = 0;
		detail = 0;
		particular = 0;
		level = 0;
	}

	ItemType(const ItemType& other)
	{
		genre = other.genre;
		detail = other.detail;
		particular = other.particular;
		level = other.level;
	}

	bool operator== (const ItemType &other)
	{
		if(genre == other.genre
			&& detail == other.detail
			&& particular == other.particular
			&& level == other.level)
		{
			return true;
		}
		return false;
	}
};

struct SmithItem
{
	ItemType type;
	int itemCount;
};

struct SmithRule
{
	SmithItem	smithItem;
	int			reqItemTypeCount;
	SmithItem*	reqItem;
	unsigned long reqMoney;
	int			reqYunHun;
	std::vector<int>	rate;
};

enum ShopType
{
	ST_Invalid = -1,
	ST_Item = 0,
	ST_Smith,
	ST_Hire,
	ST_TypeCount,
};

enum CommonErrorMsgCode
{
	CE_Bag_Full = 0,
	CE_Not_Enough_Money,
	CE_PrintScreen_Ok,
	CE_Auto_Path_Not_Support_Over_Map,
	CE_Not_Plus_Point_Limit = 15,
	CE_Too_Many_Same_Item = 100,
	CE_GM_Give_You_Msg = 1000,
};

struct CommonStyle 
{
	CommonStyle()
	{
		memset(font, 0, COMMON_CLIENT_MSG_LEN_64);
		color = 0;
	}
	char	font[COMMON_CLIENT_MSG_LEN_64];
	int		color;
	int		speed;
	int		second;
};

struct CommonStyle2
{
	CommonStyle2()
	{
		msgId = 0;
		fontId = 0;
		color = 0xffffffff;
		speed = 50;
		second = -1;
	}
	int		msgId;
	int		fontId;
	int		color;
	int		speed;
	int		second;
};
/************************************************************************/
/*                                                                      */
/************************************************************************/


typedef struct tagRSRETURNITEMINFO
{
	char szItemName[MAXSIZE_ITEMNAME];
	char szItemDesc[MAXSIZE_DES];
	char szItemDetail[MAXSIZE_DETAILDESC];
}RSRETURNITEMINFO, *PRSRETURNITEMINFO;

typedef struct tagRSSKILLINFOREQ
{
	DWORD dwSkillID;
	int nSkillLevel;
}RSSKILLINFOREQ, *PRSSKILLINFOREQ;

typedef struct tagRSRETURNSKILLINFO
{
	char szSkillName[MAXSIZE_SKILLNAME];
}RSRETURNSKILLINFO, *PRSRETURNSKILLINFO;

enum enROLESTUDIOGETINDEX
{
	enRS_ITEM_DATA = 0, 
	enRS_SKILL_DATA, 
};

#define CLIENT_NAME_AND_TITLE_MAX 16

typedef struct _Mail_Param 
{
	int		nItemCount;
	int		nContentLen; 
	DWORD	postMoney;
	DWORD	costMoney;
	char	strReceiver[CLIENT_NAME_AND_TITLE_MAX + 1];
	char	szTitle[MAXLEN_MAILTITLE];
	char	szContent[MAXSIZE_MAILTEXT];
	DWORD	pPlusData[1];
} MAIL_PARAM,  *PMAIL_PARAM;

//#define		defNEW_TONG

#define		_CHAT_SCRIPT_OPEN

// 地图Region设定
#define REGION_PIXEL_WIDTH	512
#define REGION_PIXEL_HEIGHT	1024
#define REGION_CELL_SIZE_X	32
#define REGION_CELL_SIZE_Y	32
#define REGION_CELL_WIDTH	(REGION_PIXEL_WIDTH/REGION_CELL_SIZE_X)
#define REGION_CELL_HEIGHT	(REGION_PIXEL_HEIGHT/REGION_CELL_SIZE_Y)
#define RANDOM_TRANS_RETRY_TIMES 4//随机传送尝试次数

// Add by Cooler -->
// 2005-1-10
#define		CASHTYPE_NORMAL			0
#define		CASHTYPE_COPPERCASH		1
// End add by Cooler <--
// <Add name="Adt.X" time="2005/07/07">
#define		CASHTYPE_COWRIEPLUS		2	// 城市商店，铜贝加FSB
// </Add>

// <Add name="Adt.X" time="2005/11/17">
#define		CASHTYPE_WAR_POTIONSTORE 3	// 国战药店
// </Add>

// <Add name="Adt.X" time="2006/01/10">
#define		CASHTYPE_WAR_EXP_STORE   4	// 国战积分商店
// </Add>

#define MAX_TEAM_MEMBER 6			//普通队伍最多成员数量（包括队长）
#define MAX_BIG_TEAM_MEMBER 30		//大队伍最多成员数量（包括队长）
#define MIN_TEAM_MEMBER_TO_OPEN_BIG_TEAM 2		//开启大队伍需要的最少成员数量
#define INVALID_TEAM_ID -1//非法的队伍ID

#define		MAX_SENTENCE_LENGTH					256		// 聊天每个语句最大长度

#define		FILE_NAME_LENGTH					80
#define		PLAYER_PICKUP_CLIENT_DISTANCE		180
#define		defMAX_EXEC_OBJ_SCRIPT_DISTANCE		200
#define		defMAX_PLAYER_SEND_MOVE_FRAME		1
#define		PLAYER_PICKUP_SERVER_DISTANCE		62500
#define		MAX_INT								0x7fffffff

#define		STALL_EXP_TASK						427	
#define		STALL_HUE_TASK						428	
#define		STALL_LEVEL_TASK					429
extern DWORD g_dwWarTotalTime;
//////////////////////////////////////////////////////////////////////////
// lixuewu 
// 使用了新的角色定义方法
#define		ROLE_NO								2
//#define		PLAYER_MALE_NPCTEMPLATEID			-1
//#define		PLAYER_FEMALE_NPCTEMPLATEID			-2
// lixuewu

#define		PLAYER_SHARE_EXP_DISTANCE			1024

#define		PLAYER_SHARE_EXP_DISTANCE_SQUARE	(PLAYER_SHARE_EXP_DISTANCE * PLAYER_SHARE_EXP_DISTANCE)

#define		MAX_DEATH_PUNISH_PK_VALUE			7			// PK处罚，PK值从 0 到 10
#define		MAX_PK_VALUE						3000

#define     PLAYER_SWITCH_HORSE_INTERVAL        5		    // 上下马快捷键的时间间隔

//装备部位
enum ITEM_PART
{
	itempart_unidentified	= -1,	//表示未指定位置
	itempart_helm			= 0,	//头盔
	itempart_armor,					//衣服
	itempart_shoulder,				//护肩
	itempart_boots,					//靴子
	itempart_pendant,				//坠饰（披风/令牌/绳结）
	itempart_weapon,				//武器
	itempart_amulet,				//玉佩
	itempart_ring,					//戒指
	itempart_cuff,					//护腕
	itempart_talisman,				//法宝
	itempart_horse,					//需要删除
	itempart_num,					//装备部位数目	
};

//卦的位置
enum GUA_POS
{
	gua_pos_1 = 0,
	gua_pos_2,
	gua_pos_3,
	gua_pos_4,
	gua_pos_5,
	gua_pos_6,
	gua_pos_7,
	gua_pos_8,
	gua_pos_count
};

enum GuaType
{
	GT_Invalid = -1,
	GT_Di = 0,
	GT_Lei,
	GT_Shui,
	GT_Ze,
	GT_Shan,
	GT_Huo,
	GT_Feng,
	GT_Tian,
	GT_Num,
};

enum GuaEffectDir
{
	GED_Heng = 0,
	GED_Shu,
	GED_Zuoxie,
	GED_Youxie,
	GED_Num,
};

enum EQUIP_DUR_STATE
{
	estate_normal = 0,
	estate_red,
	estate_yellow,
};

typedef struct tagPlayerItem
{
	enum {INVALIDPRICE = -1};
	tagPlayerItem() {nItemPrice = INVALIDPRICE;}
	int getItemPrice() {return nItemPrice;}
	//调用方保证传入合法的价格
	void setItemPrice(int nPrice)
	{
		nItemPrice = nPrice;
	}

	int		nIdx;
	int		nPlace;
	int		nX;
	int		nY;
	int     nItemPrice;//摆摊时候的物品价格,没有标价为-1,不能为0.
	
} PlayerItem;

enum INVENTORY_ROOM
{
	room_equipment = 0,	// 装备栏
	room_repository,	// 贮物箱
	room_trade,			// 交易栏
	room_trade1,		// 交易过程中对方的交易栏
	room_pet_feed,
	room_beset,
	room_itembox_extend,// 物品栏扩展
	room_store_extend,	// 储物箱扩展
	room_num,			// 空间数量	
};

enum ITEM_POSITION
{
	pos_equip,			// 装备着的
	pos_equiproom,		// 道具栏
	pos_repositoryroom,	// 贮物箱
	pos_traderoom,		// 交易栏
	pos_trade1,			// 交易过程中对方的交易栏
	pos_pet_feed_box,	//使用pos_pet_feed_box来表示其他玩家同步过来的消息
	pos_beset,
	pos_itembox_extend,	// 物品栏扩展
	pos_store_extend,	// 储物箱扩展
};


#define		EQUIPMENT_ROOM_PAGE			5
#define		EQUIPMENT_ROOM_WIDTH		5
#define		EQUIPMENT_ROOM_HEIGHT		6 * EQUIPMENT_ROOM_PAGE
#define		MAX_EQUIPMENT_ITEM			(EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT)
#define		EQUIPMENT_INIT_SPACE		MAX_EQUIPMENT_ITEM / EQUIPMENT_ROOM_PAGE

#define		REPOSITORY_ROOM_PAGE		5
#define		REPOSITORY_ROOM_WIDTH		5
#define		REPOSITORY_ROOM_HEIGHT		6 * REPOSITORY_ROOM_PAGE
#define		MAX_REPOSITORY_ITEM			(REPOSITORY_ROOM_WIDTH * REPOSITORY_ROOM_HEIGHT)
#define		REPOSITORY_INIT_SPACE		MAX_REPOSITORY_ITEM / REPOSITORY_ROOM_PAGE

#define		ITEMBOX_EXTEND_WIDTH		5
#define		ITEMBOX_EXTEND_HEIGHT		1

#define		STORE_EXTEND_WIDTH			5
#define		STORE_EXTEND_HEIGHT			1

#define		TRADE_ROOM_WIDTH			5
#define		TRADE_ROOM_HEIGHT			2
//----->Add by [Ray] 2004-7-28
#define		GAMBLE_ROOM_WIDTH			5
#define		GAMBLE_ROOM_HEIGHT			1
//<-----Add End
//-------> Ray [Luoliang] 2005-7-6
#define		PET_FEED_ROOM_WIDTH			1
#define		PET_FEED_ROOM_HEIGHT		1
//<------- End [Ray]
#define		MAX_TRADE_ITEM				(TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
#define		MAX_TRADE1_ITEM				MAX_TRADE_ITEM
//Add by Ray [Luoliang]  2004-7-28
#define		MAX_GAMBLE_ITEM				(GAMBLE_ROOM_WIDTH * GAMBLE_ROOM_HEIGHT)
//<-----Add End

#define		MAX_PLAYER_ITEM				(MAX_EQUIPMENT_ITEM + MAX_REPOSITORY_ITEM + MAX_TRADE_ITEM + itempart_num + ITEMBOX_EXTEND_WIDTH * ITEMBOX_EXTEND_HEIGHT + STORE_EXTEND_WIDTH * STORE_EXTEND_HEIGHT )
// lixuewu 2004.05.20 修正尺寸

//#define		BESET_ROOM_WIDTH		2
//==>Fixed By Rocker 2004.5.9
#define		BESET_ROOM_WIDTH			10
//<==Fixed End
#define		BESET_ROOM_HEIGHT			10
#define		MAX_BESET_ITEM				4



#define		REMOTE_REVIVE_TYPE			0
#define		LOCAL_REVIVE_TYPE			1

#define		MAX_MELEE_WEAPON			6
#define		MAX_RANGE_WEAPON			3
#define		MAX_ARMOR					14
#define		MAX_HELM					14
#define		MAX_RING					1
#define		MAX_BELT					2
#define		MAX_PENDANT					2
#define		MAX_AMULET					2
#define		MAX_CUFF					2
#define		MAX_BOOT					4
#define		MAX_HORSE					6

#define		MAX_NPC_TYPE	300
#define		MAX_NPC_LEVEL	200


#define		MAX_NPC_DIR		64
#define		MAX_WEAPON		(MAX_MELEE_WEAPON + MAX_RANGE_WEAPON)
#define		MAX_SKILL_STATE 10
#define		MAX_NPC_HEIGHT	128
#define		MAX_RESIST		95
#define		MAX_HIT_PERCENT	95
#define		MIN_HIT_PERCENT	5
#define		MAX_NPC_RECORDER_STATE 8 //最大Npc可以记录下来的状态数量;

#define		PLAYER_MOVE_DO_NOT_MANAGE_DISTANCE	5

#define	NORMAL_NPC_PART_NO		5		// 普通npc图像默认为只有一个部件，这是第几个

#ifndef _SERVER
#define		C_REGION_X(x)	(LOWORD(SubWorld[0].m_Region[ (x) ].m_RegionID))
#define		C_REGION_Y(y)	(HIWORD(SubWorld[0].m_Region[ (y) ].m_RegionID))
#endif

enum
{
	CHAT_S_STOP = 0,						// 非聊天状态
	CHAT_S_SCREEN,							// 与同屏幕玩家聊天
	CHAT_S_SINGLE,							// 与同服务器某玩家私聊
	CHAT_S_TEAM,							// 与队伍全体成员交谈
	CHAT_S_NUM,								// 聊天状态中类数
};

// enum PLAYER_INSTANT_STATE
// {
// 	enumINSTANT_STATE_LEVELUP = 0,
// 	enumINSTANT_STATE_REVIVE,
// 	enumINSTANT_STATE_CREATE_TEAM,
// 	enumINSTANT_STATE_LOGIN,
// // 新增
// //	enumINSTANT_STATE_DEATH, // 主角死亡特效
// //	enumINSTANT_STATE_NPC_REVIVE, // 怪物重生特效
// 	enumINSTANT_STATE_CREATURE_TRANSPOT, // 召唤兽传送(消失)
// 	enumINSTANT_STATE_CREATURE_SUMMON, // 召唤兽出现
// 	enumINSTANT_STATE_HERO_DEATH1,//8	男甲死亡
// 	enumINSTANT_STATE_HERO_DEATH2,//9	男道死亡
// 	enumINSTANT_STATE_HERO_DEATH3,//10	男异死亡
// 	enumINSTANT_STATE_HERO_DEATH4,//11	女甲死亡
// 	enumINSTANT_STATE_HERO_DEATH5,//12	女道死亡
// 	enumINSTANT_STATE_HERO_DEATH6,//13	女异死亡
// 	enumINSTANT_STATE_NPC_DEATH1,//14	小怪死亡
// 	enumINSTANT_STATE_NPC_DEATH2,//15	大怪死亡
// 	enumINSTANT_STATE_USE_SKILL_BOOK,//16	使用技能书
// 	enumINSTANT_STATE_SKILL_LEVEL_UP,//17	技能升级
// 	enumINSTANT_STATE_NUM,
// };

enum CHAT_STATUS
{
	CHAT_S_ONLINE = 0,		//在线
	CHAT_S_BUSY,			//忙碌
	CHAT_S_HIDE,			//隐身
	CHAT_S_LEAVE,			//离开
	CHAT_S_DISCONNECT,		//掉线
};

// 注意：此枚举不允许更改(by zroc)
enum OBJ_ATTRIBYTE_TYPE
{
	series_metal = 0,		//	金系
	series_wood,			//	木系
	series_water,			//	水系
	series_fire,			//	火系
	series_earth,			//	土系
	series_num,
};

enum OBJ_GENDER
{
	OBJ_G_MALE	= 0,	//雄性，男的
	OBJ_G_FEMALE,		//雌的，女的
};

enum NPCCAMP
{
	camp_begin,				// 新手阵营(白)
	camp_justice,			// PK桔阵营
	camp_evil,				// PK紫阵营
	camp_balance,			// PK蓝阵营
	camp_free,				// PK黄阵营
	camp_animal,			// 野兽阵营
	camp_event,				// 路人阵营
	camp_protect,			// PK保护阵营(绿)
	camp_freeattack,		// PK自由攻击类型
	camp_num,				// 阵营数
};

enum ITEM_IN_ENVIRO_PROP
{
	IIEP_NORMAL = 0,	//一般/正常/可用
	IIEP_NOT_USEABLE,	//不可用/不可装配
	IIEP_SPECIAL,		//特定的不同情况
};

// add by chenshanglin on 2006-2-24 for new skill system
enum COMBAT_INFO_TYPE
{
// 	DAMAGE_MISS,
// 	DAMAGE_NORMAL,
// 	DAMAGE_DEADSTRIKE,		// 会心
// 	DAMAGE_CURE,			// 治疗
// 	DAMAGE_POISON,			// 中毒
	COMBAT_INFO_DAMAGE_LIFE,
	COMBAT_INFO_HEAL_LIFE,
	COMBAT_INFO_DAMAGE_MANA,
	COMBAT_INFO_HEAL_MANA,	
	COMBAT_INFO_DODGE,

	COMBAT_INFO_ABSORB_LIFE,	// 吸收生命
	COMBAT_INFO_ABSORB_MANA,	// 吸收法力

	COMBAT_INFO_SCORE_GET,      // 获取积分
};
// add end

#define	GOD_MAX_OBJ_TITLE_LEN	1024	//128临时改为1024为了兼容旧代码 to be modified
#define	GOD_MAX_OBJ_PROP_LEN	516
#define	GOD_MAX_OBJ_DESC_LEN	(1024*5)

//==================================
//	游戏对象的描述
//==================================
struct KGameObjDesc
{
	char	szTitle[GOD_MAX_OBJ_TITLE_LEN];	//标题，名称
	char	szProp[GOD_MAX_OBJ_PROP_LEN];	//属性，每行可以tab划分为靠左与靠右对齐两部分
	char	szDesc[GOD_MAX_OBJ_DESC_LEN];	//描述
};

//==================================
//	以坐标表示的一个区域范围
//==================================
struct KUiRegion
{
	int		h;		//左上角起点横坐标
	int		v;		//左上角起点纵坐标
	int		Width;
	int		Height;
};

//==================================
//	可以游戏对象容纳的地方
//==================================
enum UIOBJECT_CONTAINER
{
	UOC_IDLE,				//默认值
	UOC_GAMESPACE,			//游戏窗口
	UOC_ITEM_TAKE_WITH,		//随身携带
	UOC_TO_BE_TRADE,		//要被买卖，买卖面板上
	UOC_OTHER_TO_BE_TRADE,	//买卖面板上，别人要卖给自己的，
	UOC_EQUIPTMENT,			//身上装备
	UOC_NPC_SHOP,			//npc买卖场所
	UOC_STORE_BOX,			//储物箱
	UOC_SKILL_LIST,			//列出全部拥有技能的窗口，技能窗口
	UOC_SKILL_TREE,			//左、右可用技能树
	UOC_SMITH,              //熔炼(打造,合成)物品(武器)的上古神台！！！
	UOC_STALL_OTHER,		//其他人摊位上的物品
	UOC_STALL_PANEL,		//自己的摆摊操作窗口
	UOC_GAMBLE_SELF,		//互博面板上,自己要用来赌博的
	UOC_GAMBLE_OTHER,		//互博面板上,别人用来赌博的
	UOC_PET_FEED_BOX,		//宠物面板的喂养物品
	UOC_ITEMBOX_EXTEND,		//物品栏扩展
	UOC_STORE_EXTEND,		//储物箱扩展
};

//==================================
// iCoreShell::GetGameData函数调用,uDataId取值为GDI_TRADE_DATA时，
// uParam的许可取值列表
// 注释中的Return:行表示相关的GetGameData调用的返回值的含义
//==================================
enum UI_TRADE_OPER_DATA
{
	UTOD_IS_WILLING,		//是否交易意向(叫卖中)
	//Return: 返回自己是否处于叫卖中的布尔值
	UTOD_IS_LOCKED,			//自己是否处于已锁定状态
	//Return: 返回自己是否处于已锁定状态的布尔值
	UTOD_IS_TRADING,		//是否可以正在等待交易操作（交易是否已确定）
	//Return: 返回是否正在等待交易操作（交易是否已确定）
	UTOD_IS_OTHER_LOCKED,	//对方是否已经处于锁定状态
	//Return: 返回对方是否已经处于锁定状态的布尔值
};

//----->Add by [Ray] 2004-7-28
enum UI_GAMBLE_OPER_DATA
{
	UGOD_IS_LOCKED,			//自己是否处于已锁定状态
	//Return: 返回自己是否处于已锁定状态的布尔值
	UGOD_IS_GAMBLING,		//是否可以正在等待赌博操作（赌博是否已确定）
	//Return: 返回是否正在等待交易操作（赌博是否已确定）
	UGOD_IS_OTHER_LOCKED,	//对方是否已经处于锁定状态
	//Return: 返回对方是否已经处于锁定状态的布尔值
};
//<-----Add End
//==================================
//	买卖物品
//==================================
struct KItemBuySelInfo
{
	char			szItemName[64];	//物品名称
	int				nPrice;			//买卖价钱，正值为卖价格，负值表示买入的价格为(-nPrice)
};

/************************************************************************/
/*							Buffer system                               */
/************************************************************************/

struct KBufferInfo 
{
	char nDesc[COMMON_CLIENT_MSG_LEN_512];		//描述
	int	 nBuff;									//buff or debuff
	char szImage[COMMON_CLIENT_MSG_LEN_16];		//图标名
};

struct KBufferSyncInfo 
{
	int	 nTime;									//秒,持续时间,-1为无限
	int	 nBuffID;								//buff or debuff
	int	 nTempBuffID;
	int	 nPileCount;
};

/************************************************************************/
/*					Game object system                                  */
/************************************************************************/

struct KGameObject
{
	KGameObject()
	{
		uGenre	= CGOG_NOTHING;
		uId		= -1;
	}
	int	uGenre;
	int	uId;
};

struct KBufferObject : KGameObject
{
	KBufferObject()
	{
		uGenre	= CGOG_BUFFER;
		uId		= -1;
	}
};

struct KSkillData
{
	KSkillData()
	{
		nSkillID = 0;
		bAltPressed = false;			
		bLeftMouse = false;
		nSkillX	= -1;
		nSkillY	= -1;
	}
	
	KSkillData( int id, bool lm = false, bool alt = false, int mousex = -1, int mousey = -1 )
	: nSkillID(id)
	, bAltPressed(alt)
	, bLeftMouse(lm)
	,nSkillX(mousex)
	,nSkillY(mousey)
	{
	}

	int nSkillID;
	bool bAltPressed;
	bool bLeftMouse;
	int	nSkillX;
	int nSkillY;
};

struct KSkillObject : KGameObject 
{
	KSkillObject()
	{
		uGenre	= CGOG_SKILL;
		uId		= -1;
	}
	
	int nSkillID;
};

struct KItemObject : KGameObject 
{
	KItemObject()
	{
		uGenre	= CGOG_ITEM;
		uId		= -1;
	}
};

struct KQuestObject : KGameObject
{
	KQuestObject()
	{
		uGenre = CGOG_QUEST;
		uId		= -1;
	}
};

#define CHANGED_SPECIAL_QUEST_DATA_MAX_COUNT 64

struct NpcMapPos
{
	int mapId;
	int x;
	int y;
	NpcMapPos()
	{
		mapId = 0;
		x = 0;
		y = 0;
	}
	NpcMapPos(const NpcMapPos& other)
	{
		mapId = other.mapId;
		x = other.x;
		y = other.y;
	}
};

struct CommonData
{
	int id;
	char name[COMMON_CLIENT_MSG_LEN_32];
	CommonData()
	{
		id = 0;
		memset(name, 0, sizeof(name));
	}

	CommonData(const CommonData& other)
	{
		id = other.id;
		strcpy(name, other.name);
	}
};

struct NpcMapInfo
{
	CommonData idAndName;
	int x;
	int y;
	NpcMapInfo()
	{
		x = 0;
		y = 0;
	}

	NpcMapInfo(const NpcMapInfo& other)
	{
		x = other.x;
		y = other.y;
		idAndName = other.idAndName;
	}
};

struct ChangedSpecialQuestData
{
	int doubleExpTag;
	int count;
	int color;
	ChangedSpecialQuestData()
	{
		doubleExpTag = 0;
		count = 0;
		color = 0xffffffff;
	}
};

struct SpecialQuestData
{
	int	 questId;
	char questType[COMMON_CLIENT_MSG_LEN_64];
	char questName[COMMON_CLIENT_MSG_LEN_64];
	char count[COMMON_CLIENT_MSG_LEN_64];
	char time[COMMON_CLIENT_MSG_LEN_64];
	char place[COMMON_CLIENT_MSG_LEN_64];
	char npc[COMMON_CLIENT_MSG_LEN_64];
	char level[COMMON_CLIENT_MSG_LEN_64];
	char tip[COMMON_CLIENT_MSG_LEN_1024];
	bool canAcceptDate[7];
	int  npcId;
	DWORD	 showColor;
	NpcMapPos  npcPos;//Npcs表的ID

	ChangedSpecialQuestData changedData;
	SpecialQuestData()
	{
		questId = 0;
		questType[0] = 0;
		questName[0] = 0;
		count[0] = 0;
		time[0] = 0;
		place[0] = 0;
		npc[0] = 0;
		level[0] = 0;
		tip[0] = 0;
		npcId = 0;
		showColor = 0;
		memset(canAcceptDate, false, sizeof(canAcceptDate));
	}

	SpecialQuestData(const SpecialQuestData& other)
	{
		questId = other.questId;
		strcpy(questType, other.questType);
		strcpy(questName, other.questName);
		strcpy(count, other.count);
		strcpy(time, other.time);
		strcpy(place, other.place);
		strcpy(npc, other.npc);
		strcpy(level, other.level);
		strcpy(tip, other.tip);

		memcpy(canAcceptDate, other.canAcceptDate, sizeof(canAcceptDate));

		npcId = other.npcId;
		npcPos = other.npcPos;
		showColor = other.showColor;

		changedData = other.changedData;	
	}
};


struct KObjAtRegion
{
	KGameObject	Obj;
	KUiRegion		Region;
	KObjAtRegion()
	{
		Obj.uGenre	= CGOG_NOTHING;
		Obj.uId		= -1;
		Region.h	= -1;
		Region.v	= -1;
		Region.Height = -1;
		Region.Width= -1;
	}
};

#ifndef _SERVER
struct KObjAtContRegion : public KObjAtRegion
{
	KObjAtContRegion():KObjAtRegion()
	{
		eContainer	= UOC_IDLE;
		nContainer	= 0;
		bPlusShop	= false;
	}
	KObjAtContRegion( const KObjAtContRegion& rRegion )
	{
		Obj.uGenre		= rRegion.Obj.uGenre;
		Obj.uId			= rRegion.Obj.uId;
		Region.h		= rRegion.Region.h;
		Region.v		= rRegion.Region.v;
		Region.Width	= rRegion.Region.Width;
		Region.Height	= rRegion.Region.Height;
		nContainer		= rRegion.nContainer;
		eContainer		= rRegion.eContainer;
		bPlusShop		= rRegion.bPlusShop;
	}
	union
	{
		UIOBJECT_CONTAINER	eContainer; 
		int					nContainer;
	};
	bool			bPlusShop;
};

#endif
/************************************************************************/
/*                                                                      */
/************************************************************************/

struct KUiMsgParam
{
	unsigned char	eGenre;	//取值范围为枚举类型MSG_GENRE_LIST,见MsgGenreDef.h文件
	unsigned char	cChatPrefixLen;
	unsigned short	nMsgLength;
	char			szName[32];
#define	CHAT_MSG_PREFIX_MAX_LEN	16
	unsigned char	cChatPrefix[CHAT_MSG_PREFIX_MAX_LEN];
};

struct KUiInformationParam
{
	char	sInformation[512];	//消息文字内容 网上的包中文字长500
	char	sConfirmText[64];	//确认消息(按钮)的标题文字
	short	nInforLen;			//消息文字内容的存储长度
	bool	bNeedConfirmNotify;	//是否要发回确认消息(给core)
	bool	bReserved;			//保留，值固定为0
};

enum PLAYER_ACTION_LIST
{
	PA_NONE = 0,			//无动作
	PA_RUN  = 0x01,			//跑
	PA_SIT  = 0x02,			//打坐
	PA_RIDE = 0x04,			//骑（马）
};

//==================================
//	系统消息分类
//==================================
enum SYS_MESSAGE_TYPE
{
	SMT_NORMAL = 0,	//不参加分类的消息
	SMT_SYSTEM,		//系统，连接相关
	SMT_PLAYER,		//玩家相关
	SMT_TEAM,		//组队相关
	SMT_FRIEND,		//聊天好友相关
	SMT_MISSION,	//任务相关
	SMT_CLIQUE,		//帮派相关
	//-------> Ray [Luoliang] 2005-5-19
	SMT_TIPMSG,		//会在屏幕中用Tip提示的重要消息
	SMT_POPUP_MSG,	//用好友上线提示框显示的消息
	//<------- End [Ray]
};

//==================================
//	系统消息响应方式
//==================================
enum SYS_MESSAGE_CONFIRM_TYPE
{
	SMCT_NONE,				//在对话消息窗口直接掠过，不需要响应。
	SMCT_CLICK,				//点击图标后立即删除。
	SMCT_MSG_BOX,			//点击图标后弹出消息框。
	SMCT_UI_RENASCENCE,		//被玩家杀死的重生界面
	SMCT_UI_ATTRIBUTE,		//打开属性页面
	SMCT_UI_SKILLS,			//打开技能页面
	SMCT_UI_ATTRIBUTE_SKILLS,//打开属性页面技能页面
	SMCT_UI_TEAM_INVITE,	//答应或拒绝加入队伍的邀请,
	//						pParamBuf 指向一个KUiPlayerItem结构的数据，表示邀情人(队长)
	SMCT_UI_TEAM_APPLY,		//答应或拒绝加入队伍的申请,
	//						pParamBuf 指向一个KUiPlayerItem结构的数据，表示申请人
	SMCT_UI_TEAM,			//打开队伍管理面板
	SMCT_UI_INTERVIEW,		//打开聊天对话界面,
	//						pParamBuf 指向一个KUiPlayerItem结构的数据，表示发来消息的好友
	SMCT_UI_FRIEND_INVITE,	//批准或拒绝别人加自己为好友
	//						pParamBuf 指向一个KUiPlayerItem结构的数据，表示发出好友邀请的人
	SMCT_UI_TRADE,			//答应或拒绝交易的请求,
	//						pParamBuf 指向一个KUiPlayerItem结构的数据，表示发出交易邀请的人
	SMCT_DISCONNECT,		//断线
	SMCT_UI_TONG_JOIN_APPLY,//答应或拒绝加入帮会的申请
	//----->Add by [Ray] 2004-7-30
	SMCT_UI_GAMBLE,			//答应或拒绝赌博的请求,
	//						pParamBuf 指向一个KUiPlayerItem结构的数据，表示发出赌博邀请的人
	SMCT_UI_OPTION_FRAME_LOW_ALERT,		//帧速率过低的提示
	//
	//<-----Add End
	//-------> Ray [Luoliang] 2005-3-22
	SMCT_UI_LEVEL_UP,		//升级的提示窗口
	//<------- End [Ray]
	//--> Rocker 2005/06/29
	SMCT_UI_RENASCENCE_KILLEDBY_NPC, // 被NPC杀死的重生界面
	//<-- End
	// <Add name="Adt.X" time="2005/09/21">
	SMCT_UI_RENASCENCE_KILLED_ON_WAR, // 在战场上被杀死
	// </Add>	
	//<---- Add By Ray [Luoliang] [2005-10-27]
	SMCT_UI_ARMY_INVITATION,	//邀请加入军团
	// End. Ray [LuoLiang] [2005-10-27] ---->
};

//==================================
//	系统消息
//==================================
struct KSystemMessage
{
	char			szMessage[256];	//消息文本
	unsigned int	uReservedForUi;	//界面使用的数据域,core里填0即可
	unsigned char	eType;			//消息分类取值来自枚举类型 SYS_MESSAGE_TYPE
	unsigned char	byConfirmType;	//响应类型
	unsigned char	byPriority;		//优先级,数值越大，表示优先级越高
	unsigned short	byParamSize;	//伴随GDCNI_SYSTEM_MESSAGE消息的pParamBuf所指参数缓冲区空间的大小。
	KSystemMessage()
	{
		szMessage[0] = 0;
		uReservedForUi = 0;
		eType = SMT_NORMAL;
		byConfirmType = SMCT_NONE;
		byPriority = 0;
		byParamSize = 0;
	}
	KSystemMessage (const KSystemMessage & rhs) 
	{
		memcpy(this, &rhs, sizeof(KSystemMessage));
	}
};

struct KUiSysChannelMsg
{
	char			szSender[32];
	char			szMsg[512];
	int				nMsgLen;
};
//==================================
//	聊天频道的描述
//==================================
// struct KUiChatChannel
// {
// 	int			 nChannelNo;
// 	unsigned int uChannelId;
// 	union
// 	{
// 		int		 nChannelIndex;
// 		int		 nIsSubscibed;	//是否被订阅
// 	};
// 	char		 cTitle[32];
// };

//==================================
//	聊天好友的一个分组的信息
//==================================
struct KUiChatGroupInfo
{
	char	szTitle[32];	//分组的名称
	int		nNumFriend;		//组内好友的数目
};

//==================================
//	好友发来的聊天话语
//==================================
struct KUiChatMessage
{
	unsigned int uColor;
	short	nContentLen;
	char	szContent[256];
};

enum KChannelType
{
	CT_Unknow,
	CT_World,
	CT_System,
	CT_Country,
	CT_Near,
	CT_Team,
	CT_ChanCount,
};
//==================================
//	主角的一些不易变的数据
//==================================
struct KUiPlayerBaseInfo
{
	char			szTongName[32];
	char			Name[32];		//名字
	char			Title[32];		//称号
	int				nCurFaction;	//当前加入门派 id ，如果为 -1 ，当前没有在门派中
	int				nRankInWorld;	//江湖排名值,值为0表示未上排名板
	unsigned int	nCurTong;		//当前加入帮派name id ，如果为 0 ，当前没有在帮派中
	int				nPortrait;		//主角的头像 
	char			szMateName[32];	//配偶名字
	int				nSkillType;
	bool			bTeam;
};

//==================================
//	主角的一些易变的数据
//==================================
struct KUiPlayerRuntimeInfo
{
	int		nLifeFull;			//生命满值
	int		nLife;				//生命
	int		nManaFull;			//内力满值
	int		nMana;				//内力
	int		nStaminaFull;		//体力满值
	int		nStamina;			//体力
	int		nAngryFull;			//怒满值
	int		nAngry;				//怒
	DWORD	nExperienceFull;	//经验满值
	DWORD	nExperience;		//当前经验值
	DWORD	nCurLevelExperience;//当前级别升级需要的经验值

	unsigned char	byActionDisable;//是否不可进行各种动作，为枚举PLAYER_ACTION_LIST取值的组合
	unsigned char	byAction;	//正在进行的行为动作，为枚举PLAYER_ACTION_LIST取值的组合
	unsigned char	bSleeping;	//正在休眠
	unsigned char	cPKValue;	//PK值

	int		nCurrentCamp;		//当前阵营
};

//==================================
//	主角的一些属性数据索引
//==================================
enum UI_PLAYER_ATTRIBUTE
{
	UIPA_STRENGTH = 0,			//力量
	UIPA_DEXTERITY,				//敏捷
	UIPA_VITALITY,				//活力
	UIPA_ENERGY,				//精力
};

//==================================
//	主角的一些易变的属性数据
//==================================
struct KUiPlayerAttribute
{
	int		nMoney;				//银两
	int		nLevel;				//等级
	int		nSeries;			//职业类型
	int		nBody;				//体
	int		nNimbus;			//灵
	int		nStrength;			//力
	int		nArt;				//术
	int		nAttackRate;		//命中
	int		nJinkRate;			//闪避
	int		nMoveSpeed;			//移动速度
	int		nAttackSpeed;		//攻击速度
	int		nCastSpeed;			//施法速度
	int		nPhysicsAttackLow;	//物理攻击
	int		nPhysicsAttackHight;//
	int		nMagicAttackLow;	//魔法攻击
	int		nMagicAttackHight;	//
	int		nPhysicsDefendLow;	//物理防御
	int		nPhysicsDefendHight;
	int		nEightDiaDefendLow;	// 八卦
	int		nEightDiaDefendHight;
	int		nDarkDefendLow;		//玄溟抗性	
	int		nDarkDefendHight;
	int		nCredit;			//信誉
	int		nShiTuPoint;		//师徒点
	int		nPhysExplode;		//物理暴击
	int		nMagicExplode;		//法术暴击
};

//==================================
//	主角的立即使用物品与武功
//==================================
struct KUiPlayerImmedItemSkill
{
	KSkillObject	IMmediaSkill[2];
};

//==================================
//	主角装备安换的位置
//==================================
enum UI_EQUIPMENT_POSITION
{
// lixuewu 2004.03.11 界面中的可装备类型 xxx 注意:要上边的itempart对应
	UIEP_HELM = 0,
	UIEP_ARMOR,
	UIEP_BELT,
	UIEP_BOOTS,
	UIEP_PENDANT,
	UIEP_WEAPON,
	UIEP_HORSE,
	UIEP_TALISMAN1,
	UIEP_TALISMAN2,
	//Lucifer~yu[zhangjianyu] [03/03/2006] Add for 
	//begin------------------------------------------------------------------------
	UIEP_TALISMAN3,
	//end--------------------------------------------------------------------------	

	// Add by chenshanglin on [2006-3-7 18:31]
// 	UIEP_TALISMAN4,
// 	UIEP_TALISMAN5,
	// Add end

	UIEP_NUM,
//	UIEP_HEAD = 0,		//头戴
//	UIEP_HAND = 1,		//手持
//	UIEP_NECK = 2,		//脖子
//	UIEP_FINESSE = 3,	//手腕
//	UIEP_BODY = 4,		//身穿
//	UIEP_WAIST = 5,		//腰部
//	UIEP_FINGER1 = 6,	//手指甲
//	UIEP_FINGER2 = 7,	//手指乙
//	UIEP_WAIST_DECOR= 8,//腰坠
//	UIEP_FOOT = 9,		//脚踩
//	UIEP_HORSE = 10,	//马匹
// lixuewu 2004.03.11 界面中的可装备类型
};



//==================================
//	一个队伍中最多包含成员的数目
//==================================
#define	PLAYER_TEAM_MAX_MEMBER	8

//==================================
//	统帅能力相关的数据
//==================================
struct KUiPlayerLeaderShip
{
	int		nLeaderShipLevel;			//统帅力等级
	int		nLeaderShipExperience;		//统帅力经验值
	int		nLeaderShipExperienceFull;	//升到下级需要的经验值
};

//==================================
//	一个玩家角色项
//==================================
struct KUiPlayerItem
{
	char			Name[32];	//玩家角色姓名
	unsigned int	uId;		//玩家角色id
	int				nIndex;		//玩家角色索引
	int				nData;		//此玩家相关的一项数值，含义与具体的应用位置有关
	int				nParam;		//整形参数， 含义与具体使用有关
};

#define MAX_SYNC_TEAMATE_BUFF_COUNT 10//最多同步队友BUFF数量

//队伍成员类型
enum enumTeamMemberType
{
	teammember_type_normal = 0,		//普通队员
	teammember_type_assistant,		//助手
	teammember_type_captain,		//队长
};

//<------- End [Ray]

//----->Add by [Ray] 2004-7-14
//----------------------------------
//一个队员信息(姓名,生命,魔法)
//----------------------------------
struct KUiTeamMemberItem
{
	KUiTeamMemberItem()
	{
//		m_nMaxLife = 0;	
//		m_nCurLife = 0;	
//		m_nMaxMana = 0;	
//		m_nCurMana = 0;	
		m_nSex = 0;
		m_nSeries = 0;
		m_nSkillSeries = -1;
		m_nLevel = 0;
		m_bIsNearBy = 0;
		m_uId = 0;		
		m_nIndex = 0;	
		m_nData = 0;	
		m_nParam = 0;	
		m_bLeader = false;
		m_Type = teammember_type_normal;
		m_nPortrait = 0;
		m_LifePercent = 0;
		m_ManaPercent = 0;
	}
	char			m_szName[32];	//玩家角色姓名
//	int				m_nMaxLife;		//玩家生命最大值
//	int				m_nCurLife;		//玩家生命当前值

//	int				m_nMaxMana;		//玩家魔法最大值
//	int				m_nCurMana;		//玩家魔法当前值

	BYTE			m_LifePercent;
	BYTE			m_ManaPercent;

	int				m_nSex;
	int				m_nSeries;
	int				m_nSkillSeries;
	int				m_nLevel;

	bool			m_bIsNearBy;	//是否在玩家附近
	unsigned int	m_uId;			//玩家角色id
	int				m_nIndex;		//玩家角色索引
	int				m_nData;		//此玩家相关的一项数值，含义与具体的应用位置有关
	int				m_nParam;		//整形参数， 含义与具体使用有关
	bool			m_bLeader;
	int				m_nPortrait;	//玩家头像

	enumTeamMemberType m_Type;

	int				m_nBuffCount;
	unsigned int	m_BuffTemplateId[MAX_SYNC_TEAMATE_BUFF_COUNT];
};
//<-----Add End

//==================================
//	组队信息的描述
//==================================
struct KUiTeamItem
{
	KUiPlayerItem	Leader;
};

//本人的队伍信息
struct KUiPlayerTeam
{
	bool bTeamLeader;			//是否是队长
	bool bCanKick;				//是否可以踢人
	bool bCanPromoteAssistant;	//是否可以提升助手
	bool bCanDismissAssistant;	//是否可以撤职助手
	bool bCanDismissTeam;		//是否可以解散队伍
	bool bCanInvite;			//是否可以邀请
	bool bCanOpenBigTeam;		//是否可以启开大队伍模式
	bool bIsBigTeam;			//是否是大队伍
	char cNumMember;			//队员数目（不包括自己）
	int nTeamServerID;			//队伍在服务器上的id，用于标识该队伍，-1 为空
	DWORD dwCaptainNpcID;		//队长NPC编号
	bool bAutoAcceptApply;		//是否自动接受入队请求
};

struct KMapPos
{
	int		nSubWorld;
	int		nRegion;
	int		nMapX;
	int		nMapY;
	int		nOffX;
	int		nOffY;
};

//==================================
//	选项设置项
//==================================
enum OPTIONS_LIST
{
	OPTION_PERSPECTIVE,		//透视模式  nParam = (int)(bool)bEnable 是否开启
	OPTION_DYNALIGHT,		//动态光影	nParam = (int)(bool)bEnable 是否开启
	OPTION_MUSIC_VALUE,		//音乐音量	nParam = 音量大小（取值为0到-10000）
	OPTION_SOUND_VALUE,		//音效音量	nParam = 音量大小（取值为0到-10000）
	OPTION_BRIGHTNESS,		//亮度调节	nParam = 亮度大小（取值为0到-100）
	OPTION_WEATHER,			//天气效果开关 nParam = (int)(bool)bEnable 是否开启
	OPTION_SCREEN_WAY,		//屏幕开始形式
	OPTION_CATOON_QULITY,	//动画质量
	OPTION_RESOLVE_RADIO,	//分辨率
	
	// Rocker 2004.7.30
	OPTION_MAXPLAYERINSCREEN,	//同屏显示最大的玩家数目
	// Rocker 2004.7.30
	// lixuewu 绘制质量控制
	OPTION_HIGH_QUALITY_PAINTING,
	OPTION_FRAMECUT_LEVEL,
	OPTION_FRAMECUT_ALERT,
	OPTION_MSN_POPUP,			//是否显示上线提示
	OPTION_REFUSE_STRAGER,		//是否拒绝陌生人信息
	OPTION_REFUSE_TEAM,			//是否拒绝组队
	OPTION_CLOSE_CHATWIN,		//是否关闭聊天窗口
	OPTION_REFUSE_TRADE,		//是否拒绝贸易
	OPTION_OPEN_SHUTCUT,		//是否打开增强快捷栏
	OPTION_FRIEND_INFO,			//是否拒绝好友信息
	OPTION_FULL_SCREEN,			//全屏显示
	OPTION_SHOW_PLAYER,			//是否玩家绘制
	OPTION_SHOW_NPC,			//是否NPC绘制
	OPTION_SHOW_SHADOW,			//是否绘制阴影
	OPTION_DRAW_GROUND,			//是否绘制地表
	OPTION_DRAW_SMALLOBJ,		//是否绘制小地表对象
	OPTION_DRAW_LARGEOBJ,		//是否绘制大地表对象
	OPTION_FONT_SHADOW,			//字体阴影
};

//==================================
//	所处的地域时间环境信息
//==================================
struct KUiSceneTimeInfo
{
	char	szSceneName[COMMON_CLIENT_MSG_LEN_32];		//场景名
	int		nSceneId;				//场景id
	int		nScenePos0;				//场景当前坐标（东）
	int		nScenePos1;				//场景当前坐标（南）
	int		nGameSpaceTime;			//以分钟为单位
};

//==================================
//	光源信息
//==================================
//整数表示的三维点坐标
struct KPosition3
{
	int nX;
	int nY;
	int nZ;
};
//
//struct KLightInfo
//{
//	KPosition3 oPosition;			// 光源位置
//	DWORD dwColor;					// 光源颜色及亮度
//	long  nRadius;					// 作用半径
//};


//小地图的显示内容项
enum SCENE_PLACE_MAP_ELEM
{ 
	SCENE_PLACE_MAP_ELEM_NONE		= 0x00,		//无东西
	SCENE_PLACE_MAP_ELEM_PIC		= 0x01,		//显示缩略图
	SCENE_PLACE_MAP_ELEM_CHARACTER	= 0x02,		//显示人物
	SCENE_PLACE_MAP_ELEM_PARTNER	= 0x04,		//显示同队伍人
};

//场景的地图信息
struct KSceneMapInfo
{
	int	nScallH;		//真实场景相对于地图的横向放大比例
	int nScallV;		//真实场景相对于地图的纵向放大比例
	int	nFocusMinH;
	int nFocusMinV;
	int nFocusMaxH;
	int nFocusMaxV;
	int nOrigFocusH;
	int nOrigFocusV;
	int nFocusOffsetH;
	int nFocusOffsetV;
};

// --> Rocker Edit Start 2005/11/10
struct KLittleMapCursorInfo
{
	int nCursorX;
	int nCursorY;
	char szInfo[32];
};
// <-- Rocker End

enum NPC_RELATION
{
	relation_none	= 1,
	relation_self	= 2,
	relation_ally	= 4,
	relation_enemy	= 8,
	relation_dialog	= 16,
	relation_produce = 32,
	relation_all	= relation_none | relation_ally | relation_enemy | relation_self | relation_dialog | relation_produce,	
	relation_num,
};

// PK 模式允许的切换者
enum PKMODE_ACCESSER
{
	pkm_accesser_none = 0,
	pkm_accesser_player = 1,
	pkm_accesser_npc = 2,
};

enum PK_MODE
{
	pk_peace,		// 和平
	pk_team,		// 队伍
	pk_tong,		// 诸侯
	pk_friendevil,	// 善恶
	pk_whole,		// 全体
	pk_guard,		// 守卫
	pk_monster,		// 怪物
	pk_gens,		// 氏族
	pk_league,      // 联盟

	// add new mode here

	pk_mode_num,
};

enum PK_TARGET_COND
{
	ptc_cond_none = 0,		// 无
	ptc_cond_self = 1,		// 自己
	ptc_cond_team = 2,		// 队伍
	ptc_cond_tong = 4,		// 诸侯
	ptc_cond_redname = 8,	// 红名
	ptc_cond_grayname = 16,	// 灰名
	ptc_cond_monster = 32,	// 怪物
	ptc_cond_gens = 64,		// 氏族
	ptc_cond_owner = 128,	// 从属
	ptc_cond_league= 256,   //联盟

	// add new condition here

	ptc_cond_num = 10,
};

enum NPCKIND
{
	kind_normal = 0	,		//0
	kind_player,			//1
	kind_partner,			//2
	kind_dialoger,			//3
	kind_bird,				//4
	kind_mouse,				//5
	kind_creature,			//6
	kind_producesrc,	// Produce source NPC //7
	kind_siege_weapon,		// 攻城器械
	kind_building, //2004.09.28
	kind_pet,			//宠物
	kind_field_mine,		// 11, 矿
	kind_field_minewell,	// 12, 矿井		
	kind_war_transferpos,	// 13, 国战传送点
	kind_field_arrowguard,  // 14, 弓箭兵
	kind_field_limit_mine,  // 15, 有限矿
	kind_field_fence,		// 16, 战场栅栏
	kind_guard,				// 17 守卫
	kind_talisman,			// 18 法宝
	kind_employee,			// 19 佣兵
    kind_num
};

enum	// 物件类型
{
	Obj_Kind_MapObj = 0,		// 地图物件，主要用于地图动画
	Obj_Kind_Body,				// npc 的尸体
	Obj_Kind_Box,				// 宝箱
	Obj_Kind_Item,				// 掉在地上的装备
	Obj_Kind_Money,				// 掉在地上的钱
	Obj_Kind_LoopSound,			// 循环音效
	Obj_Kind_RandSound,			// 随机音效
	Obj_Kind_Light,				// 光源（3D模式中发光的东西）
	Obj_Kind_Door,				// 门类
	Obj_Kind_Trap,				// 陷阱
	Obj_Kind_Prop,				// 小道具，可重生
	//--> Rocker 2005/05/12
	Obj_Kind_House_Entry,		// 房间入口
	Obj_Kind_Furniture,			// 家具
	//<-- End
	Obj_Kind_Num,				// 物件的种类数
};

//主角身份地位等一些关键属性项
enum PLAYER_BRIEF_PROP
{
	PBP_LEVEL = 1,	//登级变化	nParam表示当前等级
	PBP_FACTION,	//门派		nParam表示门派属性，如果nParam为-1表示没有门派
	PBP_CLIQUE,		//帮派		nParam为非0值表示入了帮派，0值表示脱离了帮派
};

#define MAX_MESSAGE_LENGTH 512

struct KNewsMessage
{
	int		nType;						//消息类型
	char	sMsg[MAX_MESSAGE_LENGTH];	//消息内容
	int		nMsgLen;					//消息内容存储长度
};

struct KRankIndex
{
	bool			bValueAppened;	//每一项是否有没有额外数据
	bool			bSortFlag;		//每一项是否有没有升降标记
	unsigned short	usIndexId;		//排名项ID数值
};

#define MAX_RANK_MESSAGE_STRING_LENGTH 128

struct KRankMessage
{
	char szMsg[MAX_RANK_MESSAGE_STRING_LENGTH];	// 文字内容
	unsigned short		usMsgLen;				// 文字内容的长度
	short				cSortFlag;				// 旗标值，QOO_RANK_DATA的时候表示出升降，负值表示降，正值表示升，0值表示位置未变
	int					nValueAppend;			// 此项附带的值

};

/************************************************************************/
/*							Target system                               */
/************************************************************************/
struct KTargetInfo
{
	KTargetInfo()
	{
		memset( strName, 0, sizeof(strName)	);
		szHeadImageSet = NULL;
		szHeadImage = NULL;
		nLifePercentage	= 0;
		nMagicPercentage = 0;
		nSex	= 0;
		nMetier	= 0;
		nLevel	= 0;
		nIndex  = 0;
		nId		= 0;
		nPortrait = 0;
		nSkillSeries = -1;
		nPrivateState = false;
		nTeamId = INVALID_TEAM_ID;
	}
	char	strName[64]; 
	int		nLifePercentage;
	int		nMagicPercentage;
	int		nSex;
//	int		nMaxLife;
//	int		nMaxMagic;
	int		nMetier;
	int		nLevel;
	char*	szHeadImageSet;
	char*	szHeadImage;
	//int		nHeadImage;
	int		nPortrait;
	int		nIndex;
	int		nId;
	int     nSkillSeries;
	int     nPrivateState;
	int		nTeamId;
};

/************************************************************************/
/*							Quest system                                */
/************************************************************************/

#define MAX_QUEST_REWARD_ITEM	5
#define MAX_QUEST_REQUIRED_ITEM	5

enum UI_OPERATION_RET
{
	operation_ok,
	operation_cancel,
	operation_delete,
	operation_select,
	operation_input,
	operation_ignore,
	operation_retry,
	operation_continue,
	operation_count,
};

struct KUiAnswer
{
	char	AnswerText[256];	//可选答案文字（可以包含控制符）
	int		AnswerLen;			//可选答案存储长度（包括控制符，不包含结束符）
	int		imageId;
};

struct KUiQuestionAndAnswer
{
	char		Question[1024];	//问题文字（可以包含控制符）
	int			QuestionLen;	//问题文字存储长度（包括控制符，不包含结束符）
	int			AnswerCount;	//可选答案的数目
	KUiAnswer	Answer[1];		//候选答案
};

struct KUiInstanceReward 
{
	int	UseTime;
	int KillNum;
	int	RewardExp;
	int SkillExp;
	int RewardId;
	bool Succeed;
};

#ifndef _SERVER
struct KUiMovieScene
{
	std::string					mainText;
	std::vector<std::string>	selectionText;
	int							imageId;
};
#endif

enum _taskitemtype 
{
	task_item,
	task_skill,	
};

struct KQuestItemInfo
{
	int ItemType;
	int ItemNum;
	int ItemID;
};

struct KQuestRequestItemInfo : KQuestItemInfo
{
	int haveItemCount;
};

enum ObjectiveType
{
	speak_to,
	slay_mob,
	loot_item,
	completion_quest,
	max_objective_type,
};

enum KQuestState
{
	accept_quest,
	incomplete_quest,
	complete_quest,
    failed_quest,
};

#define		MAX_OBJECTIVE		5
#define		MAX_REWARD_ITEM		5
#define		MAX_OTHER_REWARD	5
#define		QUEST_INVALID_ID	0

struct KRewardType
{
	int		rewardId;
	char	imagePath[COMMON_CLIENT_MSG_LEN_64];
	char	tip[COMMON_CLIENT_MSG_LEN_1024];

	KRewardType()
	{
		rewardId = 0;
		imagePath[0] = 0;
		tip[0] = 0;
	}

	KRewardType(const KRewardType& other)
	{
		rewardId = other.rewardId;
		strncpy(imagePath, other.imagePath, COMMON_CLIENT_MSG_LEN_64);
		imagePath[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
		strncpy(tip, other.tip, COMMON_CLIENT_MSG_LEN_1024);
		tip[COMMON_CLIENT_MSG_LEN_1024 - 1] = 0;
	}
};

struct KReward
{
	int			questId;
	ItemType	item[MAX_REWARD_ITEM];
	int			itemCount[MAX_REWARD_ITEM];
	int			otherRewardIds[MAX_OTHER_REWARD];
	KReward()
	{
		questId = 0;
		for(int j = 0; j < MAX_REWARD_ITEM; ++j)
		{
			itemCount[j] = 0;
		}

		for(int i = 0; i < MAX_OTHER_REWARD; ++i)
		{
			otherRewardIds[i] = 0;
		}
	}

	KReward(const KReward& other)
	{
		questId = other.questId;
		for(int i = 0; i < MAX_REWARD_ITEM; ++i)
		{
			item[i] = other.item[i];
			itemCount[i] = other.itemCount[i];
		}

		for(int j = 0; j < MAX_OTHER_REWARD; ++j)
		{
			otherRewardIds[j] = other.otherRewardIds[j];
		}
	}
};

struct KQuestInfo
{
	struct ObjectiveInfo
	{
		unsigned int uID;
		char		 name[COMMON_CLIENT_MSG_LEN_256];
		unsigned int uCount;
		unsigned int uProcess;
        bool changed;
	};
	unsigned int	id;									//!< Mission ID.
    BOOL            showItemBox;
	int				state;								
	char			name[COMMON_CLIENT_MSG_LEN_32];
	char			category[COMMON_CLIENT_MSG_LEN_32];
	char			description[COMMON_CLIENT_MSG_LEN_1024];		//!< 接任务时显示的消息
	char			aim[COMMON_CLIENT_MSG_LEN_1024];				//!< 接任务时显示的消息（任务目标）
	char			completeMsg[COMMON_CLIENT_MSG_LEN_1024];		//!< 任务还未完成时NPC显示的消息
	char			incomplete[COMMON_CLIENT_MSG_LEN_1024];		//!< 任务完成时NPC显示的消息（交任务）
	ObjectiveInfo	requirement[max_objective_type][MAX_OBJECTIVE];

	unsigned long	rewardMoney;
	unsigned long	rewardExp;

	unsigned int	rewardItemCount;
	KQuestItemInfo	rewardItem[MAX_REWARD_ITEM];		

	unsigned int	selectRewardItemCount;
	KQuestItemInfo	selectRewardItem[MAX_REWARD_ITEM];

	int			Button[operation_count];
};

struct KSimpleQuestInfo
{
	char	questName[COMMON_CLIENT_MSG_LEN_64];
	char	questTypeName[COMMON_CLIENT_MSG_LEN_64];
	int		questId;
};

struct KQuestRequest
{
	UI_OPERATION_RET	eOperType;
	unsigned int		uParam;
	unsigned int		uParamEx;
	std::vector<int>			requiredItemIndex;
};

/************************************************************************/
/*                             帮会相关                                 */
/************************************************************************/

//----------------------------  ------------------------

#define		defTONG_MAX_DIRECTOR				7
#define		defTONG_MAX_MANAGER					56
#define		defTONG_MAX_LEAGUE					7
#define		defTONG_ONE_PAGE_MAX_NUM			7
#define		defTONG_SEX_NUM						2
#define		defTONG_BILLBOARD_NUM				10
#define		defTONG_UNION_MAX_NUM				10

#define		defTONG_STR_LENGTH					32

#define		defTONG_NAME_MAX_LENGTH				8
#define		defTONG_CALL_MAX_LENGTH				8
#define		defTONG_UNION_MAX_LENGTH			8

// 从帮会储物箱取钱每次最多能取多少
#define		defTONG_GET_MONEY_MAX				100000000	// 1亿
// 从帮会储物箱存钱每次最多能存多少
#define		defTONG_SAVE_MONEY_MAX				100000000	// 1亿
// 联盟帮会的列表类型
#define		TONG_LIST_TYPE_LEAGUE				0x123
// 所有帮会的列表类型
#define		TONG_LIST_ALL						0x124

enum TONG_MEMBER_FIGURE
{
	enumTONG_FIGURE_MEMBER,				// 帮众
	enumTONG_FIGURE_MANAGER,			// 队长
	enumTONG_FIGURE_DIRECTOR,			// 长老
	enumTONG_FIGURE_MASTER,				// 帮主
	enumTONG_FIGURE_NUM,
};

enum
{
	enumTONG_APPLY_INFO_ID_SELF,		// 申请查询自身信息
	enumTONG_APPLY_INFO_ID_MASTER,		// 申请查询帮主信息
	enumTONG_APPLY_INFO_ID_DIRECTOR,	// 申请查询长老信息
	enumTONG_APPLY_INFO_ID_MANAGER,		// 申请查询队长信息
	enumTONG_APPLY_INFO_ID_MEMBER,		// 申请查询帮众信息(一批帮众)
	enumTONG_APPLY_INFO_ID_ONE,			// 申请查询某帮会成员信息(一个帮众)
	enumTONG_APPLY_INFO_ID_TONG_HEAD,	// 申请查询某帮会信息，用于申请加入帮会
	enumTONG_APPLY_INFO_ID_NUM,
};

enum
{
	enumTONG_CREATE_ERROR_ID1 = 1,	// Player[m_nPlayerIndex].m_nIndex <= 0
	enumTONG_CREATE_ERROR_ID2,	// 交易过程中
	enumTONG_CREATE_ERROR_ID3,	// 帮会名问题
	enumTONG_CREATE_ERROR_ID4,	// 帮会阵营问题
	enumTONG_CREATE_ERROR_ID5,	// 已经是帮会成员
	enumTONG_CREATE_ERROR_ID6,	// 自己的阵营问题
	enumTONG_CREATE_ERROR_ID7,	// 等级问题	
	enumTONG_CREATE_ERROR_ID8,	// 钱问题
	enumTONG_CREATE_ERROR_ID9,	// 组队不能建帮会
	enumTONG_CREATE_ERROR_ID10,	// 帮会模块出错
	enumTONG_CREATE_ERROR_ID11,	// 名字字符串出错
	enumTONG_CREATE_ERROR_ID12,	// 名字字符串过长
	enumTONG_CREATE_ERROR_ID13,	// 帮会同名错误
	enumTONG_CREATE_ERROR_ID14,	// 帮会产生失败
	enumTONG_CREATE_ERROR_ID15,	// 外挂打开的
};

//-->Rocker 2004/12/07 
struct TAX_WND_INIT
{
	int nMinPos;
	int nMaxPos;
	int nCurPos;
};
//<--Rocker 

//---------------------- 打造合成熔炼物品相关A -----------------
//======================================
//	熔炼(合成、打造)物品(武器)的各个部件
//======================================
enum UI_TREMBLE_POSITION
{
	UITP_ITEM = 0,      //物品
	UITP_GOLD,          //金宝石
	UITP_WOOD,          //木宝石
	UITP_WATER,         //水宝石
	UITP_FIRE,          //火宝石
	UITP_EARTH,         //土宝石
	UITP_SPIRIT,        //灵气宝石
	UITP_LEVEL,         //等级宝石
	UITP_ADDITON,		//附加的孔,凑成9个
	UITP_NUM,           //部件数量
};



//--------------------- 打造合成熔炼物品相关end ----------------

struct UiItemInfo
{
	ItemType type;
	int count;
};


struct KUiCompoundRuleInfo
{
	int						money;					//需求金钱
	int						yunhun;					//需求蕴魂
	int						successRate;			//成功率
	int						levelDownRate;			//降级率
	int						destoryRate;			//损毁率
	ItemType				generateItem;			//生产物品
	int						ruleId;					//规则id
};


//UI动作类型
enum UI_ACTION
{
	enumUA_CLEAR_BUY_QUEUE,			//购买队列
	enumUA_BESET_UI_CLOSE,			//镶嵌界面关闭
	enumUA_BESET_CLEAR_ITEM,		//镶嵌界面清空
};

//客户端通知类型
enum enumCLIENT_NOTIFY_TYPE
{
	enumCNT_WAIT_TO_GIVE_SOMETHING_OTHERS,				//想给些东西别人
	enumCNT_ACCEPT_THAT_OTHERS_WANT_TO_GIVE_ME,			//愿意接受别人的施舍
	enumCNT_REJECT_THAT_OTHERS_WANT_TO_GIVE_ME,			//不愿意接受别人的施舍
	enumCNT_I_WANT_TO_SET_PORTRAIT,						//我想要设置头像
};

//服务端通知类型
enum enumSERVER_NOTIFY_TYPE
{
	enumSNT_THE_PEOPLE_ACCEPT_YOUR_DELIVER,				//那可怜人愿意接受你的施舍,uParam = 那人的Npc ID
	enumSNT_THE_PEOPLE_REJECT_YOUR_DELIVER,				//那混蛋不接受你的施舍,uParam = 那人的Npc ID
	enumSNT_SOMEONE_WANT_TO_GIVE_YOU_SOMETHING,			//别人想恳求你接受他的施舍,uParam = 那人的Npc ID
	enumSNT_OTHERS_GIVE_YOU_MONEY,						//别人给你钱了,uParam = 那人的Npc ID
	enumSNT_GIVE_MONEY_TO_OTHER,						//你给别人钱了
	enumSNT_YOU_SET_YOUR_PORTRAIT,						//你设置头像吧
	enumSNT_YOU_SIT_DOWN,								//你坐下吧，这里cStatus[0] = 1就是坐下了， = 0 就是站起来了
};
//-------> Ray [Luoliang] 2004-8-9
//赏金猎人的数据结构

// Hunter, [Adt.X], 2005-3-11.
enum PostReplyCode
{
	addpost_nopos = 0,	// No positions.
	addpost_offered,	// Player already have offered post.
	addpost_nomoney,	// Player have no enough money.
	addpost_success,	// Player successful add post.

	delpost_nooffered,	// Player haven't offered post.
	delpost_novalid,	// Player offered post is not exist or in valid state, can't be del.
	delpost_success,    // Player successful delete post.

	apppost_applied,    // Player have applied post.
	apppost_novalid,    // Player applying post no valid.
	apppost_nomoney,    // Player have not enough money.
	apppost_nopos,		// Exceed max hunter num.
	apppost_success,    // Player successful applied.

	cclpost_noapplied,  // Player haven't applied post.
	cclpost_novalid,    // Player applied post is not exist or in valid state.
	cclpost_success,    // Player successful canceled.
};

enum PostNotifyCode
{
	enter_offerpost_cleaned,   // When player login found offered post cleaned by system.
	enter_offerpost_completed, // When player login found offered post be completed by someone.
	enter_apppost_cleaned,	   // When player login found offered post cleaned by system.	
	enter_apppost_completed,   // When player login found applied post be completed by someone.
	enter_apppost_invalid,	   // When player login found applied post be invalided by someone.
	post_completed_inccount,   // When player killed target increase mission count by one.
	post_completed_owner,	   	
	post_completed_byself,     // When player login found applied post be completed by self.
	post_completed_byother,    // When player login found applied post be completed by others.
	post_overdue_owner,	       
	post_overdue_hunter,		
};

// End.

// 增加一条赏金猎人任务后的结果类型
enum enADDREWARDRETCODE
{
	enAddRewardRetOK = 0, 
	enAddRewardRetHavePost, 
	enAddRewardRetNoTarget, 
	enAddRewardRetNoMoney, 
	enAddRewardRetOther, 
	enAddRewardRetEnd, 
};

// 赏金猎人任务领取结果类型
enum enAPPLYREWARDRETCODE
{
	enApplyRewardRetOK = 0, 
	enApplyRewardRetHaveApply, 
	enApplyRewardRetNoTarget, 
	enApplyRewardRetNoMoney, 
	enApplyRewardRetOther, 
	enApplyRewardRetEnd, 
};

typedef struct tagRewardHunter 
{
	unsigned int	uID;					//赏金信息ID
	char			szTragetName[CLIENT_NAME_AND_TITLE_MAX + 1]; //被追杀目标的姓名
	unsigned short	usTargetLevel;			//被追杀目标的等级
	unsigned char	cKillCount;				//追杀次数
	unsigned int 	uReward;				//赏金
	unsigned char	byTimeLimit;			//期限
}REWARD_HUNTER;
//-------> Ray [Luoliang] 2004-10-8
enum UIMESSAGEBOX_SYTLE
{
	UIMB_OK = 1,
	UIMB_YESNO,
	UIMB_NUM,
};

enum UIMESSAGEBOX_RESULT
{
	UI_OK = 0,
	UI_YES = UI_OK,
	UI_NO,
};
//<------- End [Ray]
//<------- End [Ray]


//-------> Ray [Luoliang] 2004-11-17
//排名所需的数据结构
struct UI_RANK_WORLD				//十大的世界排名
{
	char	szName[CLIENT_NAME_AND_TITLE_MAX + 1];
	BYTE	btLevel;
	BYTE	btCareer;				//series_metal 甲士,
									//series_wood 道士, 
									//series_water 异人
	WORD	wCredit;				//声望
	unsigned int	uMoney;	
	char	szTongName[CLIENT_NAME_AND_TITLE_MAX + 1];
};

struct UI_RANK_CAREER				//按职业划分的十大排名
{
	char	szName[CLIENT_NAME_AND_TITLE_MAX + 1];
	BYTE	btLevel;
	WORD	wCredit;				//声望
	unsigned int	uMoney;
	char	szTongName[CLIENT_NAME_AND_TITLE_MAX + 1];
};

struct UI_RANK_TONG					//帮会的十大排名
{
	char	szTongName[CLIENT_NAME_AND_TITLE_MAX + 1];
	char	szLeaderName[CLIENT_NAME_AND_TITLE_MAX + 1];
	WORD	wMemberNum;
	WORD	wRenQi;					//人气
	CUInt64	cu64Money;	
	char	szCityName[CLIENT_NAME_AND_TITLE_MAX + 1];
};
//<------- End [Ray]

// <Add name="Adt.X" time="2005/09/15">
// 战事列表
struct UI_RANK_WAR
{
	WORD	nYear;
	BYTE	nMonth;
	BYTE	nDay;
	char	szATongName[CLIENT_NAME_AND_TITLE_MAX + 1];
	char	szATongMasterName[CLIENT_NAME_AND_TITLE_MAX + 1];
	WORD	wAMemberNum;
	char	szBTongName[CLIENT_NAME_AND_TITLE_MAX + 1];
	char	szBTongMasterName[CLIENT_NAME_AND_TITLE_MAX + 1];
	WORD	wBMemberNum;
	BYTE	btResult;	// 0 = 未打, 1 = A胜, 2 = B胜
};
// </Add>

//-------> Ray [Luoliang] 2004-11-12
enum enumDRAW_EUQIP_STYLE
{
	DRAW_EQUIP_NORMAL_IMAGE = 0,
	DRAW_EQUIP_SPECIAL_IMAGE_BIG_MALE,
	DRAW_EQUIP_SPECIAL_IMAGE_BIG_FEMALE,
	DRAW_EQUIP_SPECIAL_IMAGE_SMALL_MALE,
	DRAW_EQUIP_SPECIAL_IMAGE_SMALL_FEMALE,
};
//<------- End [Ray]
//通缉追杀任务
typedef struct tagKillerTask
{
	DWORD	dwTaskID;
	DWORD	dwTotalTime;
	DWORD	dwRemainTime;
	int		nTaskReward;
	char	szTargetName[32];
	BYTE	byType:4;					//0:查询自己创建的任务,1:查询自己所领取任务,2:所有任务, 3:杀自己的任务				
	BYTE	byPart:4;					//0:head 1:mid 2:tail
}KILLER_TASK;

typedef struct tagKillerSubmitTask
{
	int m_nRentFee;					//任务激活时间所花费的佣金(单位:JXB)
	char m_chKilleeName[32];		//被追杀角色名
	int m_nReward;					//酬金(单位:剑侠币)
}KILLER_SUBMIT_TASK;

// 彩票窗口打开参数
struct UILottoOpenParam
{
	unsigned int uPicks;
	unsigned int uDrawing;
	int nJackpot;
};


// 建筑基本信息
struct UIBuildingInfo
{
	char szName[32];  // 建筑物的名字 
	char szInfo[256]; // 建筑物的描述信息
};
// 建筑悬停提示信息
struct UIBuildingTip
{
	char szTip[256]; // 悬停信息文字
};
//地形描述结构
typedef UIBuildingTip UITerrainInfo;
//建筑物图标路径
typedef UIBuildingTip UIBuildingIcon;

// lixuewu 常量字符串复制
#define strcpy_const(dest,src)	memcpy((dest),(src),sizeof((src)))

// lixuewu 对象目标类型
enum eTargetType
{
	type_npc =0,
	type_obj,
	targettype_count=type_obj
};

struct _tagMapPersonStatictis
{
	struct _Pair
	{
		int nMapID;
		int nPerson;
	};
	int nCount;
	_Pair p[1];
};

// 头顶图标 lixuewu 2004.12.14
enum eEmoteName
{
	EMOTE_PLAYER_BEGIN=1,
	EMOTE_ADDEXP	= 1,
	EMOTE_DEATH_WEAK = 2,
	EMOTE_FREE_ATTARK = 3,
	EMOTE_UNKNOW3,
	
	EMOTE_WAR_MEMBER = 8,
	EMOTE_WAR_CAPTAIN = 9,
	EMOTE_WAR_COMMANDER = 10,

    // add by hejianfeng for Anti-Wallow.  2005-10-10
    EMOTE_ANTI_WALLOW_ILL = 5,
    // endadd
	EMOTE_CITY_BEGIN = 9,
	EMOTE_WINERCUP = 12,

	EMOTE_FIVECITY = 11, 

	EMOTE_MAX_ID	= 16,
};

//add by zuolizhi
enum eTechEffect
{
	enDoorLife_1	=	0,
	enDoorLife_2,
	enDoorLife_3,
	enDoorLife_4,
	//----------->
	enDoorDefence_1,
	enDoorDefence_2,
	enDoorDefence_3,
	enDoorDefence_4,
	//----------->
	enBuildingLife_1,
	enBuildingLife_2,
	enBuildingLife_3,
	enBuildingLife_4,
	//------------>
	enBuildingDefence_1,
	enBuildingDefence_2,
	enBuildingDefence_3,
	enBuildingDefence_4,
	//------------->
	enTowerAttack_1,
	enTowerAttack_2,
	enTowerAttack_3,
	enTowerAttack_4,
	//------------->
	enTowerRange_1,
	enTowerRange_2,
	enTowerRange_3,
	enTowerRange_4,
	//------------->
	enArcherAttack_1,
	enArcherAttack_2,
	enArcherAttack_3,
	enArcherAttack_4,
	//----------->
	enArcherRange_1,
	enArcherRange_2,
	enArcherRange_3,
	enArcherRange_4,
	//-------------->
	enTankAttack_1,
	enTankAttack_2,
	enTankAttack_3,
	enTankAttack_4,
	//-------------->
	enTankDefence_1,
	enTankDefence_2,
	enTankDefence_3,
	enTankDefence_4,
	//-------------->
	enTankResistance_1,
	enTankResistance_2,
	enTankResistance_3,
	enTankResistance_4,
	//-------------->
	enTankSpeed_1,
	enTankSpeed_2,
	enTankSpeed_3,
	enTankSpeed_4,
	//-------------->
};
//

enum EXP_SHARE_MODE
{
	SHARE_EXP_CLS = 1, 
	SHARE_EXP_AVG = 2,
};

#define MAX_LEVEL 99

// 升级信息
#define MAX_LEVEL_UP_TIP_LENGTH 200
#define MAX_LEVEL_UP_DESC_LENGTH 300

//升级增加属性
enum LevelUpAddAttribute
{
	attr_Ti = 0,		//体
	attr_Ling,			//灵
	attr_Li,			//力
	attr_Shu,			//术
	attr_Weight,		//负重

	attr_Count			//属性数量
};

//等级提升增量
typedef struct tagLevelUpAdd
{
	DWORD Exp;		//经验
	DWORD SkillExp;	//技能经验
	int Attribute[attr_Count];	//属性
	int Money;		//金钱上限
	char Tip[MAX_LEVEL_UP_TIP_LENGTH];
	char Desc[MAX_LEVEL_UP_DESC_LENGTH];
} LevelUpAdd;

struct tagLevelUpInfo 
{
	char szDescription[512];
	
	int nLevelBefore;
	int nLevelAfter;
	
	int nLifeBefore;
	int nLifeAfter;
	
	int nManaBefore;
	int nManaAfter;

	int nStrBefore;
	int nStrAfter;
	
	int nDexBefore;
	int nDexAfter;

	int nConBefore;
	int nConAfter;

	int nIntBefore;	
	int nIntAfter;

	int nStrBonus;
	int nDexBonus;
	int nConBonus;
	int nIntBonus;
};

enum ENCHASE_ERRORMSG
{
	enchaser_error_no = 0,				//	满足规则，没错误
	enchaser_error_type,				//  变化类型
	enchaser_error_levelsfull,			//	装备等级已经到头
	enchaser_error_addmagic,			//	不能点蓝的装备
	enchaser_error_conditionisinvalid,	//	条件不符合，参数不匹配
	enchaser_error_ratesuccess,			//	概率检查成功
	enchaser_error_ratefailed,			//	概率检查失败,但是材料没损毁
	enchaser_error_ratedestroy,			//	概率检查失败,但是材料损毁
	enchaser_error_another_action,		//	已经在施放另一个技能
	enchaser_error_not_enough_space,	//  背包空间满
	enchaser_error_level_down,			//  降级

	//通用
	enchaser_error_no_fill_rule,		//	没有匹配规则
	enchaser_error_money,				//	金钱不够
	enchaser_error_skillpoint,			//	技能点不够
	enchaser_error_less_material,		//  材料不够
	enchaser_error_less_targetItem,		//	缺少源道具
	enchaser_error_targetItem_invalid,	//	源道具无效

	enchaser_error_end,					//
};

enum ENCHANSER_RULE_FIT_CONDITION
{
	//升级
	enchaser_rule_levelup_valid = 0,
	enchaser_rule_levelup_less_material,
	enchaser_rule_levelup_less_targetItem,
	enchaser_rule_levelup_targetitem_invalid,

	//加持
	enchaser_rule_addmagic_valid,
	enchaser_rule_addmagic_less_material,

	//拆爻
	enchaser_rule_getyao_valid,
	enchaser_rule_getyao_less_targetItem,
	enchaser_rule_getyao_targetitem_invalid,

	//附爻
	enchaser_rule_addyao_valid,
	enchaser_rule_addyao_less_material,
	enchaser_rule_addyao_less_targetItem,
	enchaser_rule_addyao_targetitem_invalid,
};

enum ENCHANSER_RESULT_TIP_INDEX
{
	//升级相关
	enchaser_tip_levelup_full_success = 0,							//100%成功
	enchaser_tip_levelup_success_or_not,							//成功或无效
	enchaser_tip_levelup_success_or_leveldown,						//成功或降级
	enchaser_tip_levelup_success_or_destory,						//成功或损毁
	enchaser_tip_levelup_success_or_destory_or_leveldown,			//成功或降级或损毁
	enchaser_tip_levelup_success_or_leveldown_or_not,				//成功或降级或无效
	enchaser_tip_levelup_success_or_destory_or_not,					//成功或损毁或无效
	enchaser_tip_levelup_success_or_destory_or_leveldown_or_not,	//成功或损毁或无效

	//加持
	enchaser_tip_addmagic_full_success,								//100%成功
	enchaser_tip_addmagic_success_or_not,							//成功或无效
	enchaser_tip_addmagic_success_or_destory,						//成功或损毁
	enchaser_tip_addmagic_success_or_not_or_destory,				//成功或损毁或无效

	//拆爻
	enchaser_tip_getyao_full_success,
	enchaser_tip_getyao_success_or_not,								//

	//附爻
	enchaser_tip_addyao_full_success,
	enchaser_tip_addyao_success_or_not,
	enchaser_tip_addyao_success_or_destory,
	enchaser_tip_addyao_success_or_not_or_destory,
};

enum ENCHANSER_RESULT
{
	//升级
	enchaser_rst_levelup_success = 0,
	enchaser_rst_levelup_invalid,
	enchaser_rst_levelup_destory,
	enchaser_rst_levelup_leveldown,
	//加持
	enchaser_rst_addmagic_success,
	enchaser_rst_addmagic_invalid,
	enchaser_rst_addmagic_destory,
	//拆爻
	enchaser_rst_getyao_success,
	enchaser_rst_getyao_fail,
	//附爻
	enchaser_rst_addyao_success,
	enchaser_rst_addyao_invalid,
	enchaser_rst_addyao_destory,
};

enum OVERWEIGHT_ERRORMSG
{
	overweight_error_normal = 0,		//恢复正常负重
	overweight_error_overweight			//超重
};


// lixuewu 2005.03.31
struct tagPillInfo
{
	unsigned int uState;
	unsigned int uTakeWith;
	unsigned int uCheck;
	unsigned int uCanMake;
	unsigned int uExpPower;
};
//-------> Ray [Luoliang] 2005-6-9
#ifndef _SERVER
#include <time.h>

typedef struct tagUiPetInfo
{
	int		nIndex;
	char	szPetName[32];
	BYTE	byPetHonor;
	DWORD	dwPetTimer;

}UiPetInfo;


#endif
//<------- End [Ray]
//-------> Ray [Luoliang] 2005-4-14

struct tagCertifyParam 
{
	BYTE	byType;
	int		nWidth;
	int		nHeight;
	char*	szQuestion;
	char*	pAnswerSet;
	BYTE*	pImageData;
};
//<------- End [Ray]

//-------> Ray [Luoliang] 2005-5-25
enum enInputDlgCallType
{
	enMerchant = 0,
	enCDKey,
	enChangName,
	//Lucifer~yu[zhangjianyu] [12/30/2005] Add for 通知军团全体成员
	//begin------------------------------------------------------------------------
	enAllWarMemberMsg,
	//end--------------------------------------------------------------------------	
};
enum enInputDlgInputType
{
	enInputDig = 0,
	enInputString,
};
struct tagInputDlgParam
{
	char * szText;
	int	   nTextLen;
	int	   nCallType;
	int    nInputType;
};
//<------- End [Ray]

//<---- Add By Ray [Luoliang] [2005-9-13]
struct tagImpeachContent 
{
	BYTE	byType;
	char 	szName[32];
	char 	szContent[128];	
	DWORD	dwID;
};
// End. Ray [LuoLiang] [2005-9-13] ---->
struct TONG_WAR_PROPERTY 
{
	char szWarState[CLIENT_NAME_AND_TITLE_MAX + 1];
	char szWarTime[10];
	char szRivalryCountry[CLIENT_NAME_AND_TITLE_MAX + 1];
};

//<---- Add By Ray [Luoliang] [2005-9-16]
enum
{
	enTechStatusDisable,				//所依赖的建筑不存在		
	enTechStatusDeactive,				//科技等级为0
	enTechStatusActive,					//建筑存在且科技等级大于0

	//enTechStatusUpgrading,				//正在升级
};

struct tagUITechNode
{
	std::string		strName;
	std::string		strDescription;
	UINT			uUpgradeTime;		
	int				nMoneyCost;
	int				nBronzeCost;
	int				nResourceCost;	
	DWORD			dwStatus;
};
struct tagUITechNodeStatus
{
	int				nLevel;
	bool			bEnable;
//	DWORD			dwTimeElapsed;
};
struct tagUITechCategory 
{
	int					nCategory;
	int					nCurUpgradingTree;
	int					nCurUpgradingNode;
	int					nCurUpgradingLevel;
	DWORD				dwTimeElapsed;
	DWORD				dwTimeTotal;
	UINT				uNodeNum;
	tagUITechNodeStatus	tagNodesStatus[1];
		
};

struct tagTechNodeIdx 
{
	UINT			uCategory;
	UINT			uTree;
	UINT			uNode;	
	UINT			uLevel;
};

// End. Ray [LuoLiang] [2005-9-16] ---->


// <Add name="Adt.X" time="2005/07/29">
struct SalaryRedeemParam
{
	bool	state;				// true, 开启兑换功能.
	int		nRedeemRate;		// 兑换比率
	DWORD	nRedeemAlert[2];	// 城市金库兑换警戒线	
};
// </Add>

struct  tagCheckGroupItem
{
	std::string		strText;
	bool			bChecked;
	DWORD			dwID;
};
struct tagCheckGroupBox
{
	char				szText[128];
	int					nCount;
	int					nCanCheckedNum;
	tagCheckGroupItem	tagCheckItems[1];
};

// --> Rocker Edit Start 2005/11/14
struct UiSetBuidlingOption
{
	char				btGroup;
	DWORD				dwNpcID;
	int					nNpcIdx;
	int					nOption;
};
// <-- Rocker End

//Lucifer~yu[zhangjianyu] [01/10/2006] Add for 
//begin------------------------------------------------------------------------
#ifndef _SERVER
	struct UiNormalBuffer 
	{
		int		nExistTime;			/* seconds, 0 = not exist, -1 = always exist */
		BYTE	byStateType;
	};

	struct KUiNewFont 
	{
		char			szName[COMMON_CLIENT_MSG_LEN_32];
		char			szContext[COMMON_CLIENT_MSG_LEN_128];
		bool			bDefaultFont;
		int				nWidth;
		int				nX;
		int				nY;
		int				nZ;
		unsigned int	uColor;
	};
#endif
//end--------------------------------------------------------------------------	


enum ITEMGENRE
{
	item_equip = 0,			// 装备 0
	item_medicine,			// 药品 1
	item_mine,				// 矿石 2
	item_materials,			// 原材料 3
	item_task,				// 任务 4
	item_townportal,		// 传送门 5
	item_magicorscript,		// 触发魔法效果或会触发脚本的物品 6
	item_skillbook,			// Skill book, learn skill 7
	item_target,			// 需要目标的物品 8
	item_enchase,			// 内丹	9
	item_horse,				// 马匹	10
	item_ib,				// IB Item
	item_charm,				// 护身符
	item_number,			// 类型数目
};

enum TARGET_ITEM_PARTICULAR
{
	normal_targetitem,
	inlay_targetitem,
	reset_inlay_targetitem,
	reset_yao_targetitem,
	repair_targetitem,
	levelup_tool_targetitem,
};

//用来区分任务物品表中普通任务物品与背包（背包不再另外开表所以做在了任务物品表中）
//xiehong 2006年11月24日
enum QuestKeyType
{
	QK_TaskItem = 0,
	QK_Bag,
};

//吟唱条操作
//xiehong 2007年1月18日
enum DELAYED_ACTION_OPER
{
	DA_New,
	DA_Delay,
	DA_Cancel,
	DA_Complete,
};

struct CASTBAR_PARAM
{
	DELAYED_ACTION_OPER cmd;
	int					time;
	int					msgCode;
};

#define TM_HOLE_NUM 15//法宝镶嵌位置数量
#define TALISMAN_ENCHASE_PER_LEVEL 3//法宝每等级开启的镶嵌孔数量
#define TALISMAN_ENCHASE_LEVEL_COUNT (TM_HOLE_NUM / TALISMAN_ENCHASE_PER_LEVEL)//法宝镶嵌层数

struct TALISMAN_INFO
{
	int  id;							//法宝的全局id
	char name[COMMON_CLIENT_MSG_LEN_16];//法宝名字
	int  level;							//当前级别
	int	 canUplevel;					//是否可升级，0表示不能升级，1表示可升级不绑定，2表示可升级当时绑定
	int  curYunHun;						//当前蕴魂
	int	 maxYunHun;						//最大蕴魂
	char description[COMMON_CLIENT_MSG_LEN_256];//描述
	int  holeBuffId[TM_HOLE_NUM];		//孔对应的buffid （>0表示buffId，0表示无buff但是油孔，<0表示孔也没有）
};

//孔位置信息
struct TM_HOLE_POS
{
	int talismanId;						//法宝的id
	int holeIndex;						//孔的位置0-17
};

//孔状态信息
struct TM_HOLE_STATE
{
	TM_HOLE_POS holePos;				//孔位置
	int			buffId;					//孔对应的buffid （>0表示buffId，0表示无buff但是油孔，<0表示孔也没有）
};
//法宝相关……end

#define YAO_ADDON_BUFF_COUNT 3

typedef struct
{
    int nGenre;
    int nDetail;
    int nParticular;
	int	nLevel;
	int	nGroup;
	int bIBNoTarget;
}FIND_ITEMINDEX_PARAM, *PFIND_ITEMINDEX_PARAM;
// endadd

//Lucifer~yu[zhangjianyu] [02/22/2006] Add for 
//begin------------------------------------------------------------------------
struct KPillParam
{
	int		nPillExp;
	int		nPillMaxExp;
};
//end--------------------------------------------------------------------------	

struct KItemInfo 
{
	char					szImageSet[COMMON_CLIENT_MSG_LEN_128];
	char					szImage[COMMON_CLIENT_MSG_LEN_128];
	FIND_ITEMINDEX_PARAM	itemIdx;
	int						nGroup;
	char					szName[COMMON_CLIENT_MSG_LEN_32];
	char					szToolTip[GOD_MAX_OBJ_DESC_LEN];
	bool					bVendue;
	int						iReqLevel;
	int						colour;
	bool					bDestory;
	bool					bIsBind;
	bool					bIsEquipBind;
};

#ifdef _SERVER
#define	MAX_SUBWORLD 2000
#define INSTANCE_SUBWORLD_START 300
#define INSTANCE_SUBWORLD_END 2000
#else
#define	MAX_SUBWORLD 1
#define MAX_MAP_TEMPLATE 300 //must be = INSTANCE_SUBWORLD_START
#define INSTANCE_SUBWORLD_START 1
#define INSTANCE_SUBWORLD_END 1
#endif

#ifdef _SERVER

#define MAX_SKILL_STRATEGY_LIST_LENGTH 5//最大技能策略列表长度

//NPC状态
enum enumNpcState
{
	npc_state_none = 0,		//无
	npc_state_guard,		//守卫
	npc_state_follow,		//跟随
	npc_state_combat,		//战斗
	npc_state_flee,			//逃跑
	npc_state_patrol,		//巡逻
	npc_state_trapped,		//无法行动
	npc_state_return,		//回归
	npc_state_follow_wait,	//跟随等待（等待跟随目标出现）
};

//技能策略模式
enum enumSkillStrategyMode
{
	skill_strage_mode_passive = -1,//被动
	skill_strage_mode_neutral = 0,//中立
	skill_strage_mode_positive = 1//主动
};

//技能策略
typedef struct tagSkillStrategy
{
	int SkillId;
	int Weight;
	enumSkillStrategyMode Mode;
} SkillStrategy, *PSkillStrategy;

#else

//数据集定义
#define itemgroupcd_dataset		"itemgroupcd"
#define vendue_dataset			"vendue_dataset"
#define vendue_operation		"vendue_operation"
#define tong_operation			"tong_operation"
#define tong_dataset			"tong_dataset"
#define city_dataset			"city_dataset"
#define city_operation			"city_operation"

#endif// _SERVER

//延迟动作消息
enum enumDelayedActionMessage
{
	delayed_action_msg_none = 0,			//未知
	delayed_action_msg_pickup_object,		//拾取物品
	delayed_action_msg_dialog_npc,			//对话NPC
	delayed_action_msg_use_item,			//使用物品
	delayed_action_msg_smith,				//锻造
};

#define MIN_TALISMAN_LEVEL 0	//最高法宝等级
#define MAX_TALISMAN_LEVEL 5	//最低法宝等级

//技能是否可用结果
enum enumSkillUseableResult
{
	skill_useable_result_unknown = -1,	
	skill_useable_result_ok = 0,			//可以使用
	skill_useable_result_invalid_skill,		//非法的技能（没有这个技能）
	skill_useable_result_out_of_range,		//超出射程
	skill_useable_result_no_mana,			//没魔
	skill_useable_result_no_life,			//没血
	skill_useable_result_no_skill_exp,		//没蕴魂
	skill_useable_result_in_cd,				//技能冷却中
	skill_useable_result_invalid_target,	//非法的目标
	skill_useable_result_no_target,			//没有目标
	skill_useable_result_pk_protection,		//PK保护
	skill_useable_result_no_required_weapon,	//没有装备合适的武器
	skill_useable_result_no_required_item,	//没有需要的物品
	skill_useable_result_behind_barrier,	//在阻挡后面
};

//定时器编号
enum enumTimerId
{
	timer_instance_expire = 5,					//副本过期
};

/*****************雇佣*******************/

/*****************雇佣(end)*******************/
#ifndef _SERVER

struct SearchContentParam 
{
	std::string text;
	int		id;
	QueryType queryType;
	QueryResultType	queryResultType;
};


struct SOCIETY_INFO 
{
	char* szZhuhou;
	char* szShizu;
	char* szLeague;
};

struct Position
{
	int x;
	int y;
	Position()
	{
		x = 0;
		y = 0;
	}
};

struct MapPosInfo
{
	Position pos;
	int mapId;
	char mapName[COMMON_CLIENT_MSG_LEN_64];
	MapPosInfo()
	{
		memset(mapName, 0, COMMON_CLIENT_MSG_LEN_64);
		mapId = 0;
	}
};

//界面通用的列表项接口（树形结构）
struct CommonTreeItem
{
	int id;
	char name[COMMON_CLIENT_MSG_LEN_32];
	int childCount;
};

//无效ID
#define COMMON_ITEM_INVALID_ID -1


struct ItemPriceLayout
{
	const char* jinImage;
	const char* yinImage;
	const char* tongImage;
	const char* moneyNormalColor;
	const char* moneyNotEnoughColor;
	const char* font;
	const char* sellText;
	const char* buyText;
	const char* repairText;
};

struct TargetPlayerInfo
{
	int				npcId;
	BYTE            pkValue;						//pk值
	BYTE			shengwang;						//声望
	BYTE			level;							//等级
	char*			name;							//姓名
	char*			shizu;							//氏族
	char*			zhuhou;							//诸侯
	char*			lianmen;						//联盟
	char*			chenghao;						//称号
	int				skillType;
	int				metier;
};

struct RoleHeadInfo 
{
	DWORD dwID;
	char* szInfo;
	int	nX;
	int nY;
};

struct PlayerInfo
{
	char	szName[17];
	char	szZhuhou[17];
	char	szShizu[17];
	short	sLevel;
	short	sMetier;
	short	sSkillType;	
};


#define npc_type_icon_number 9
#define normal_player_bitmap_idx 0
#define player_team_bitmap_idx 1
#define player_the_same_tong_bitmap_idx 2
#define player_slef_bitmap_idx         3
#define zhuhouzhang_bitmap_idx 4
#define sizhuzhang_bitmap_idx 5
#define creature_bitmap_idx 6
#define badplayer_bitmap_idx 7
#define normal_npc_bitmap_idx 8
#define battle_pic_numbers 200
struct ChatPoint
{
	int x;
	int y;
	unsigned long hdc;
	unsigned long bitmapHandle[npc_type_icon_number+battle_pic_numbers];
};

#define MAX_WORLD_ORG_NAME_LEN 32
#define MAX_WORLD_UI_ORG       2

typedef struct tagWorldCombatClientBaseInfo
{
	char szOrgName[MAX_WORLD_ORG_NAME_LEN];
	
	tagWorldCombatClientBaseInfo()
	{
		memset(szOrgName, 0, sizeof(szOrgName));
	}
}WorldCombatClientBaseInfo;

typedef struct tagMapChannelInfo
{
	char szChannelFullName[MAX_WORLD_ORG_NAME_LEN];
	char szChannelName[MAX_WORLD_ORG_NAME_LEN];
	char szColor[MAX_WORLD_ORG_NAME_LEN];
	char szFont[MAX_WORLD_ORG_NAME_LEN];

	tagMapChannelInfo()
	{
		memset(szChannelFullName, 0, sizeof(szChannelFullName));
		memset(szChannelName, 0, sizeof(szChannelName));
		memset(szColor, 0, sizeof(szColor));
		memset(szFont, 0, sizeof(szFont));
	}
}MapChannelInfo;

typedef struct tagWorldCombatClientInfo
{
	WorldCombatClientBaseInfo baseInfo;
	int                       nScore;
}WorldCombatClientInfo;

typedef struct tagWorldCombatUIParam
{
	int                      nInfoNum;
	WorldCombatClientInfo    detail[MAX_WORLD_UI_ORG];
}WorldCombatUIParam;

#define MAX_UI_INSRUANCE_LEVEL_REC_NUM 120 
typedef struct tagUIInsuranceInfo
{
	int  m_nCurrentInsuranceValue;   //目前的保险金额(单位：通宝)
	int  m_nTotalMoneyGot;           //已经领取的金钱(单位：铜) 
	int  m_nMoneyLeftToGet;          //尚未领取的金钱(单位：铜)
}UIInsuranceInfo;

typedef struct tagUIInsuranceRewardRec
{
	int m_nLevel;
	int m_nMoneyGet;
}UIInsuranceRewardRec;

typedef struct tagUIInsuranceCalc
{
	int m_nImediatelyReward;
	int m_nPreviesReward;
	UIInsuranceRewardRec m_nLevelRewards[MAX_UI_INSRUANCE_LEVEL_REC_NUM];
}UIInsuranceCalc;

#endif

//性别
enum enumNpcSex
{
	sex_male = 0,	//男性
	sex_female		//女性
};

#define INVALID_WORLD_INDEX -1	//非法的世界序号
#define INVALID_WORLD_ID -1		//非法的世界编号
#define INVALID_INSTANCE_ID 0	//非法的副本编号

//提示事件（需要客户端确认）
enum enumPromptEvent
{
	prompt_event_invalid = 0,
	prompt_event_add_buff,
};

#define MAX_TEAM_BASIC_INFO_LIST_SIZE 10	//队伍基本信息列表最大长度

//队伍基本信息
struct TeamBasicInfo
{
	int TeamId;
	BYTE MemberCount;
	DWORD CaptainNpcID;
	char CaptainName[32];
};

//推荐人系统
#define MAX_STUDENT_COUNT 50//最多学生数量
#define MAX_LIST_STUDENT_BUFF_LENGTH (sizeof(LIST_STUDENT) + sizeof(StudentInfo) * MAX_STUDENT_COUNT)

//推荐人操作
enum RecommenderOp
{
	recommender_op_update_student = 0,
	recommender_op_get_master_reward,
};

#ifndef _SERVER

struct RecommendRewardInfo 
{
	int TotalRewardToAdd;
	int TotalRewardTicketAdded;
};

struct RecommendItem
{
	char Name[COMMON_CLIENT_MSG_LEN_32];
	int Level;
	int RewardMoeny;
	int TotalRewardMoney;

	RecommendItem()
	{
		Name[0] = 0;
		Level = 0;
		RewardMoeny = 0;
		TotalRewardMoney = 0;
	}

	RecommendItem(const RecommendItem& other)
	{
		memcpy(Name, other.Name, sizeof(Name));
		Name[sizeof(Name) - 1] = 0;
		
		Level = other.Level;
		RewardMoeny = other.RewardMoeny;
		TotalRewardMoney = other.TotalRewardMoney;
	}
};

typedef std::vector<RecommendItem> RecommedList;
#endif
 
//GM 请求——————————————————begin
struct GMCommunicationData
{
	int type;
	char msg[COMMON_CLIENT_MSG_LEN_256];
};

enum GMCommunicationType
{
	GMCT_COMMON_TYPE,
};

enum OnceItemType
{
	return_money,
	local_revive, //死亡原地复活
	insrance,     //购买保险
};

//GM 请求——————————————————end

struct UICombatTopMemberInfo
{
	UICombatTopMemberInfo()
	{
		Level = 0;
		Sex = 0;
		Class = 0;
		SkillSeries = 0;
		memset(Name, 0, sizeof(Name));
		Score = 0;
	}
	
	BYTE Level; //等级
	BYTE Sex;
	BYTE Class;//职业
	BYTE SkillSeries;//技能系
	char Name[CLIENT_NAME_AND_TITLE_MAX + 1];//名字
	int Score;
};

#define MAXSIZE_PALYER_INFO_STRING 25
#define MAXSIZE_PLAYER_MSN_STRING  37
struct UIPlayerRealInfo
{
	UIPlayerRealInfo() : Sex(0), Age(0)
	{
		ZeroMemory(Name,        sizeof(Name)       );
		ZeroMemory(QQNumber,    sizeof(QQNumber)   );
		ZeroMemory(TeleNumber,  sizeof(TeleNumber) );
		ZeroMemory(MobleNumber, sizeof(MobleNumber));
		ZeroMemory(Address,     sizeof(Address)    );
		ZeroMemory(MSNNumber,   sizeof(MSNNumber)  );
		ZeroMemory(ISNumber,    sizeof(ISNumber)   );
		ZeroMemory(UTNumber,    sizeof(UTNumber)   );
	}

	BYTE Sex;									//		性别         byte(1)
	BYTE Age;									//		年龄         byte(1)
	char Name[CLIENT_NAME_AND_TITLE_MAX + 1];	//		姓名         char(17)	
	char Address[MAXSIZE_PALYER_INFO_STRING];	//		所在地区     char(25)
	char QQNumber[MAXSIZE_PALYER_INFO_STRING];	//		QQ           char(25)
	char MSNNumber[MAXSIZE_PLAYER_MSN_STRING];	//		Msn          char(37)
	char ISNumber[MAXSIZE_PALYER_INFO_STRING];	//		IS           char(25)
	char UTNumber[MAXSIZE_PALYER_INFO_STRING];	//		UT           char(25)
	char TeleNumber[MAXSIZE_PALYER_INFO_STRING];//		固定电话     char(25)
	char MobleNumber[MAXSIZE_PALYER_INFO_STRING];//		移动电话     char(25)
};

struct UIPlayerRealInfoEx : public UIPlayerRealInfo
{
	UIPlayerRealInfoEx()
	{
		ZeroMemory(Consort, sizeof(Consort));
	}

	char Consort[MAXSIZE_PALYER_INFO_STRING];//配偶
};

struct UIPlayerRealInfoGet
{
	UIPlayerRealInfoGet()
	{
		ZeroMemory(&PlayerGUID, sizeof(PlayerGUID));
	}

	FSGUID PlayerGUID;
};
struct UIStatueInfo
{
	UIStatueInfo()
	{
		ZeroMemory(cName, sizeof(cName));
		ZeroMemory(cTongName, sizeof(cTongName));
		ZeroMemory(cMapName, sizeof(cMapName));
		nMapId = INVALID_WORLD_ID;
		dwtime = 0;	
		hasBuff= 0;
	}

	char cName[CLIENT_NAME_AND_TITLE_MAX + 1];
	char cTongName[CLIENT_NAME_AND_TITLE_MAX + 1];
	char cMapName[CLIENT_NAME_AND_TITLE_MAX + 1];
	int  nMapId;
	DWORD dwtime;
	BYTE  hasBuff;
};
#define CITY_RES_TYPE_COUNT 4
#define MAX_SYNC_TO_WORLD_PARAM_COUNT 2		//世界同步参数的最大个数

#define UI_QUESTION_BUFF_SIZE (1024 * 10)

struct UIQuestionData
{
	char ImgData[UI_QUESTION_BUFF_SIZE];
	int ImgDataLength;
	char TextData[UI_QUESTION_BUFF_SIZE];
	int TextDataLength;
	int Timeout;
	char AppendDescStrId[UI_QUESTION_BUFF_SIZE];
};

struct GM_ChuanSong 
{
	int nMapID;
	int nPosX;
	int nPosY;
};

struct PLUS_POINT_PARAM 
{
	std::string name;
	DWORD		maxpluspoint;
}; 

struct SMALL_BATTLE_FIELD_RESULT
{
	SMALL_BATTLE_FIELD_RESULT()
	{
		CombatMapTemplateID = 0;
		PersistTime			= 0;
		Repute				= 0;
		SelfScore			= 0;
		EnemyScore			= 0;
	}

	int	CombatMapTemplateID;
	int	PersistTime;
	int SelfScore;
	int EnemyScore;
	int Repute;
};

struct PLUS_POINT_TOPN_ELEM
{
	int        m_nNumber;
	char       m_szName[17];
	BYTE       m_Level;
	char       m_GensName[17];
	char       m_TongName[17];
	DWORD      m_nScore;
}; 

//自动拾取物品分类信息
struct ItemClassInfo
{
	char	m_className[COMMON_CLIENT_MSG_LEN_32];
	bool	m_isGroup;
	bool	m_isColor;
	bool	m_isPickup;
	
	ItemClassInfo()
	{
		memset(m_className, 0, sizeof(m_className));
		m_isGroup = false;
		m_isColor = false;
		m_isPickup = false;
	}
	void SetItemClassInfo(bool isGroup, bool isColor)
	{
		m_isGroup = isGroup;
		m_isColor = isColor;
	}
};

//自动释放技能信息
struct AutoCastSkillInfo
{
	int				m_SkillID;
	int				m_CastInterval;
	bool			m_IsSelected;
	unsigned long	m_lastCastTime;
	AutoCastSkillInfo()
	{
		m_SkillID = INVALID_SKILL_ID;
		m_lastCastTime = 0;
		m_CastInterval = 0;
		m_IsSelected = false;
	}
};

enum UiTitleInfoActiveState
{
	title_active_state_no = 0,	//未获得
	title_active_state_next,	//下一级
	title_active_state_yes,		//已获得
	title_active_state_first,	//未获得的成长型称号的第一级
};

struct UiTitleInfo
{
	BYTE	ID;
	char	Name[COMMON_CLIENT_MSG_LEN_32];
	BYTE	Type;
	DWORD	State; //基本职业
	char	Tip[COMMON_CLIENT_MSG_LEN_256];
	UiTitleInfoActiveState ActiveState;
};

struct UiShizuPopularityInfo
{
	int Rank;
	char Name[17];
	int Level;
	int PlayerCount;
	DWORD Popularity;
};

struct UiZhuhouPopularityInfo
{
	int Rank;
	char Name[17];
	int Level;
	int PlayerCount;
	DWORD Popularity;
};

struct UiCombatKillRankInfo
{
	int Rank;
	char Name[17];
	int Level;
	char Shizu[17];
	char Zhuhou[17];
	DWORD CombatKill;
};

//玩家属性页属性
struct UiPlayerProperties
{
	DWORD ActiveDegreeTotal;
	DWORD ActiveDegreeCurrent;
	DWORD ShizuPopularity;
	int ShizuPlayerCount;
	DWORD ZhuhouPopularity;
	int ZhuhouPlayerCount;
	int ZhuhouShizuCount;
};

//NPC称号
struct UiNpcTitle
{
	DWORD NpcId;
	char Title[17];
};

#define ACTIVE_DEGREE_DISPLAY_RATIO 100//活跃度显示比例（为提高数据存储精度）

//称号类型
enum TitleType
{
	title_type_permanent = 0,
	title_type_time_limited,
	title_type_upgradable
};

#endif
