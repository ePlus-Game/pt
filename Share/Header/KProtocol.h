#ifndef KPROTOCOL_H
#define KPROTOCOL_H

#ifndef __linux
	#ifdef _STANDALONE
		#include "GameDataDef.h"
	#else
		#include "GameDataDef.h"
	#endif
#else
	#include "GameDataDef.h"
	#include <string.h>
#endif

#include "KProtocolDef.h"
#include "SocialComDef.h"
#include "ItemCommonDef.h"
#include "EmplomentDataDef.h"

#pragma	pack(push, 1)

#define	PROTOCOL_MSG_TYPE	BYTE
#define PROTOCOL_MSG_SIZE	(sizeof(PROTOCOL_MSG_TYPE))
#define	MAX_PROTOCOL_NUM	255

#define _NAME_LEN	32
#define _ACTIVEKEY_LEN	64


struct EXTEND_HEADER
{
	BYTE	ProtocolFamily;							//协议所属的家族
	BYTE	ProtocolID;								//协议名称
};

struct tagExtendProtoHeader
{
	BYTE ProtocolType;
	WORD wLength;
};

typedef struct tagCompressedProtocolHeader
{
	BYTE Protocol;
	WORD Length;
} COMPRESSED_PROTOCOL_HEADER, *PCOMPRESSED_PROTOCOL_HEADER;

/************************************************************************/

/*
带宽优化前
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	AttackSpeed;
	BYTE	CastSpeed;
	BYTE	HelmType;
	BYTE	ArmorType;
	BYTE	WeaponType;
	BYTE	HorseType;
	BYTE	ShoulderType;
	BYTE	BootType;
	BYTE	CuffType;

	BYTE	RankID;
	DWORD	ID;
	BYTE	m_btSomeFlag;	// 0x03 PKFlag 0x04 FightModeFlag 0x08 SleepModeFlag 0x10 TongOpenFlag (0x20 maybe m_bRideHorse)  0x40 StallModeFlag  0x80 StallBuyModeFlag

} PLAYER_SYNC;
*/

typedef struct
{
	BYTE	ProtocolType;
	BYTE	AttackSpeed;
	BYTE	CastSpeed;

// 	BYTE	WeaponRideType;
// 	BYTE	HelmArmorType;
// 	BYTE	HorseShoulderType;
// 	BYTE	BootCuffType;

	//caolei+ 2008.12.4
	//由于偏色需要同步，结构体改回来
	BYTE	HelmType;
	BYTE	ArmorType;
	WORD	WeaponType;
	BYTE	HorseType;
	BYTE	ShoulderType;
	BYTE	BootType;
	BYTE	CuffType;

	DWORD	ID;
	BYTE	TitleIndex;
	BYTE	TitleLevel;

	BYTE	m_btSomeFlag;	// 0x01 m_bRideHorse
} PLAYER_SYNC;

/*
带宽优化前
typedef struct
{
	BYTE	ProtocolType;
	BYTE	bySetEfficacy;
	DWORD	ID;
	BYTE	AttackSpeed;
	BYTE	CastSpeed;			// 是否考虑不改变施法速度，或施法速度等于攻击速度
	BYTE	HelmType;
	BYTE	ArmorType;
	BYTE	WeaponType;
	BYTE	HorseType;
	BYTE	ShoulderType;
	BYTE	BootType;
	BYTE	CuffType;
	WORD	MorphNpc;	// 变身成的Npc类型 or 套装特效的编号
	BYTE	bRideHorse;
} PLAYER_NORMAL_SYNC;
*/

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	AttackSpeed;
	BYTE	CastSpeed;

//	BYTE	WeaponRideType;
// 	BYTE	HelmArmorType;
// 	BYTE	HorseShoulderType;
// 	BYTE	BootCuffType;

	BYTE	HelmType;
	BYTE	ArmorType;
	WORD	WeaponType;
	BYTE	HorseType;
	BYTE	ShoulderType;
	BYTE	BootType;
	BYTE	CuffType;
	
	BYTE	m_btSomeFlag; // 0x01 m_bRideHorse
} PLAYER_NORMAL_SYNC;

/*!
\brief
	道具合成c2s协议数据结构
*/
typedef struct
{
	typedef struct
	{
		int		nPlace;
		int		nX;
		int		nY;
	} ItemPos;
	BYTE    ProtocolType;
	BYTE	nCompoundType;
	int		nRuleId;
	char	szPlusInfo[COMMON_CLIENT_MSG_LEN_64];
	ItemPos Part[MAX_LEVELUP_ITEMS_COUNT];
}
ENCHASER_CLIENTSEND;

typedef struct
{
	int		ruleId;
	BYTE    protocolType;
}
SMITH_C2S;

/*!
\brief
	道具合成s2c协议数据结构
*/
typedef struct 
{
	typedef struct
	{
		int		nPlace;
		int		nX;
		int		nY;
	} ItemPos;
	BYTE    ProtocolType;
	int		NewItemID;
	int		nResult;
	BYTE	compoundType;
}ENCHASER_SERVERRESULT;

/*
带宽优化前
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	Camp;				// 阵营
	BYTE	CurrentCamp;		// 当前阵营
	BYTE	m_bySeries;			// 五行系
	BYTE	LifePerCent;		// 生命百分比
	BYTE	ManaPercent;		// 内力百分比
	BYTE	m_btMenuState;		// 组队、交易等状态
	BYTE	m_Doing;			// 行为
	DWORD	dwDoingParamX;		// 行为参数X
	DWORD	dwDoingParamY;		// 行为参数Y
	DWORD	dwDoingParamZ;		// 行为参数Z
	BYTE	m_btKind;			// npc类型
	DWORD	MapX;				// 位置信息
	DWORD	MapY;				// 位置信息
	DWORD	ID;					// Npc的唯一ID
	WORD	TeamId;				// 队伍ID
	int		NpcSettingIdx;		// 客户端用于加载玩家资源及基础数值设定
	DWORD	dwLifeUpLimit;
	DWORD	dwManaUpLimti;
	DWORD	dwNameColor;
	bool	bFightState;
	int		RunSpeed[3];		// 跑速
	int		nCondition;
	int		nSummonID;
	WORD	TalismanNpcId;
	char	m_szName[32];		// 名字
	int		CityId;				// 城市编号
	BYTE	IsKing;				// 是否是侯主
    BYTE    IsGensMgr;          // 是否是氏族长
	char	ZhuhouName[17];		// 诸侯名
	char	ShizuName[17];		// 氏族名
	BYTE	byNpcDir;			// npc方向	
	short	nSkillType;
	short	HeadImage;
	char	invaderTongName[17];		// 国战敌对国家名字
	BYTE    nCombatOrg;
	BYTE    bCamouflage;                // 是否是蒙面状态
} NPC_SYNC;
*/

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_bySeries;			// 五行系
	BYTE	m_Doing;			// 行为
	BYTE	LifePercentage;		// 生命百分比
	BYTE	ManaPercentage;		// 法力百分比
	DWORD	dwDoingParamX;		// 行为参数X
	DWORD	dwDoingParamY;		// 行为参数Y
	DWORD	dwDoingParamZ;		// 行为参数Z
	BYTE	m_btKind;			// npc类型
	DWORD	MapX;				// 位置信息
	DWORD	MapY;				// 位置信息
	DWORD	ID;					// Npc的唯一ID
	WORD	TeamId;				// 队伍ID
	int		NpcSettingIdx;		// 客户端用于加载玩家资源及基础数值设定
	DWORD	dwNameColor;
	bool	bFightState;
	int		RunSpeed[3];		// 跑速
	WORD	nCondition;
	DWORD	nSummonID;
	WORD	TalismanNpcId;
	BYTE	CityId;				// 城市编号
	BYTE	IsKingOrGens;		// 是否是侯主，是否是氏族长
	BYTE	byNpcDir;			// npc方向	
	char	nSkillType;
	char	HeadImage;
	BYTE    nCombatOrg;
	BYTE    bCamouflage;		// 是否是蒙面状态
	BYTE    nLeagueFlag;        // 联盟相关标识

	char	NameBuff[1];
} NPC_SYNC;

//-------> Ray [Luoliang] 2005-6-23
typedef struct
{
	BYTE	ProtocolType;
	WORD	wLength;
	BYTE	PetProtocal;
	void	SetProtocolHeader(BYTE byPetProtocol, WORD wLen)
	{
	#ifdef _SERVER
		ProtocolType = s2c_pet;
	#else
		ProtocolType = c2s_pet;
	#endif
		PetProtocal = byPetProtocol;
		wLength = wLen;
	};
}PET_HEADER;

typedef struct tagPET_INIT_SYNC : PET_HEADER
{	
	DWORD	dwID;						// 主人的ID
	bool	bPetReleased : 1;		// 是否释放了
	BYTE	byPetType : 7;				// 宠物类型
	BYTE	byPetHonor;				// 亲密度
	BYTE	byPetColor;				// 宠物颜色
	char	szPetName[32];			// 宠物的姓名
}PET_INIT_SYNC;

typedef struct tagPET_NORMAL_SYNC : PET_HEADER 
{
	DWORD	dwID;					// 主人的ID
	bool	bPetReleased : 1;		// 是否释放了
	BYTE	byPetType : 7;				// 宠物类型
	BYTE	byPetHonor;				// 亲密度
	BYTE	byPetColor;				// 宠物颜色
}PET_NORMAL_SYNC;

typedef struct tagPET_S2C_CHANGE_NAME : PET_HEADER 
{
	DWORD	dwID;
	char	szPetName[32];
}PET_S2C_CHANGE_NAME;

typedef struct tagPET_S2C_PET_CHAT : PET_HEADER
{
	DWORD	dwID;
	char	szSentence[128];
}PET_S2C_PET_CHAT;

typedef struct tagPET_S2C_OPEN_PANEL : PET_HEADER
{
	DWORD	dwPetTimer;
}PET_S2C_OPEN_PANEL;

typedef struct tagPET_C2S_CHANGE_NAME : PET_HEADER
{
	char	szPetName[32];
}PET_C2S_CHANGE_NAME;


typedef struct tagPET_C2S_FEED_PET : PET_HEADER
{
	DWORD dwItemID;
}PET_C2S_FEED_PET;
//<------- End [Ray]

/*
带宽优化前
typedef struct
{
	BYTE	ProtocolType;
	BYTE	Camp;				// 阵营有一部分用来表示生命百分比了,所以阵营最多只支持32种 lixuewu 2004.11.17
	DWORD	ID;
	DWORD	MapX;
	DWORD	MapY;
	BYTE	LifePercentage;
	BYTE	ManaPercentage;
	BYTE	Doing;
	BYTE	State;
	BYTE	byNeedUpdate;
} NPC_NORMAL_SYNC;
*/

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	DWORD	MapX;
	DWORD	MapY;
	BYTE	LifePercentage;
	BYTE	ManaPercentage;
	BYTE	Doing;
	BYTE	byNeedUpdate;
} NPC_NORMAL_SYNC;

// NPC的增强同步,目前用于召唤兽的速度和外观同步
typedef struct  {
	BYTE	ProtocolType;
	DWORD	ID;				// NPC ID
	BYTE	Speed;			// NPC 移动速度
	WORD	MorphNpc;		// NPC 变身外观
} NPC_ENHANCE_SYNC;

/*
带宽优化前
typedef struct
{
	BYTE	ProtocolType;
	DWORD	m_dwNpcID;
	DWORD	m_dwMapX;
	DWORD	m_dwMapY;
	WORD	m_wOffX;
	WORD	m_wOffY;
	BYTE	m_byDoing;
	BYTE	m_btCamp;
} NPC_PLAYER_TYPE_NORMAL_SYNC;
*/

typedef struct
{
	BYTE	ProtocolType;
	DWORD	m_dwNpcID;
	DWORD	m_dwMapX;
	DWORD	m_dwMapY;
	WORD	m_wOffX;
	WORD	m_wOffY;
} NPC_PLAYER_TYPE_NORMAL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
} NPC_REMOVE_SYNC, NPC_SIT_SYNC, NPC_REQUEST_COMMAND, NPC_REQUEST_FAIL;

/*
带宽优化前
typedef struct  {
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	btKilledType; // =0 Killed by Player =1 killed by npc // btKilledType = 2 弹出是否使用重生值的选择框 
	int		nExp;			
	int		nWeakTime;
}NPC_DEATH_SYNC;
*/

typedef struct  {
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	btKilledType; // =0 Killed by Player =1 killed by npc // btKilledType = 2 弹出是否使用重生值的选择框  // btKilledType = 3 表示同步灵石镶嵌 id表示灵石镶嵌个数
}NPC_DEATH_SYNC;

typedef struct {
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	btColorIdx;
}CHANGE_NPC_COLOR;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_WALK_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	Type;
} NPC_REVIVE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_JUMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	nInlayCount;
} NPC_INLAYCOUNT_SYNC;

typedef struct
{
	BYTE    ProtocolType;
	BYTE    Comoflag;
	char    szName[MAXSIZE_ROLENAME];
	int     nID;
}NPC_COMMO_FLAG;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_RUN_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nFrames;
	int		nX;
	int		nY;
} NPC_HURT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	Camp;
} NPC_CHGCURCAMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	Camp;
} NPC_CHGCAMP_SYNC;

/*
带宽优化前
typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nSkillID;
	int		nSkillLevel;
	int		nMpsX;
	int		nMpsY;
} NPC_SKILL_SYNC;
*/

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	WORD	nSkillID;
	BYTE	nSkillLevel;
	int		nMpsX;
	int		nMpsY;
} NPC_SKILL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	int		nSkillID;
	int		nMpsX;
	int		nMpsY;
} NPC_SKILL_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	int		nMpsX;
	int		nMpsY;
} NPC_WALK_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	ReviveType;		// 6.29 Rocker Fix Revive type 0= 玩家选择确定 1= 玩家选择虚弱 2=玩家选择损失经验
} NPC_REVIVE_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	int     nCurMpsX;
	int     nCurMpsY;
	int		nMpsX;
	int		nMpsY;
} NPC_RUN_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	shopType;
	int		nShopIndex;
	int		cityTaxRate;

} SALE_BOX_SYNC;

typedef struct 
{
	BYTE	ProtocolType;
	int		nNpcId;
	WORD	nFriendCount;
	WORD	nOnLineFriendCount;
} PLAYER_DIALOG_NPC_COMMAND; //主角与nNpcId对话的请求

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_dwExp;				// 当前经验
	int		m_nSkillPoint;		// 当前技能经验
} PLAYER_EXP_SYNC;				// 玩家同步经验

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	ShareExpType;		// 经验共享方式 lixuewu 2005.03.08
} PLAYER_APPLY_CREATE_TEAM;		// 客户端玩家创建队伍，向服务器发请求

struct PLAYER_SEND_CREATE_TEAM_SUCCESS
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	nTeamServerID;		// 队伍在服务器上的唯一标识
	PLAYER_SEND_CREATE_TEAM_SUCCESS() {nTeamServerID = -1;}
};	// 服务器通知玩家队伍创建成功

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btErrorID;		// 队伍创建不成功原因：0 同名 1 玩家本身已经属于某一支队伍 3 当前处于不能组队状态
} PLAYER_SEND_CREATE_TEAM_FALSE;// 服务器通知客户端队伍创建不成功

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwTarNpcID;		// 查询目标 npc id
} PLAYER_APPLY_TEAM_INFO;		// 客户端向服务器申请查询某个npc的组队情况

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
} PLAYER_APPLY_TEAM_INFO_FALSE;	// 服务器告知客户端申请查询某个npc的组队情况失败

// typedef struct PLAYER_SEND_TEAM_INFO_DATA
// {
// 	BYTE	ProtocolType;		// 协议名称
// 	int		m_nCaptain;			// 队长 npc id
// 	int		m_nMember[MAX_TEAM_MEMBER];	// 所有队员 npc id
// 	DWORD	nTeamServerID;		// 队伍在服务器上的唯一标识
// 	PLAYER_SEND_TEAM_INFO_DATA() {nTeamServerID = -1;};
// } PLAYER_SEND_TEAM_INFO;		// 服务器向客户端发送某个队伍的信息数据

//队伍成员信息
typedef struct tagTeamMemberInfo
{
	tagTeamMemberInfo()
	{
		NpcId = 0;
		Level = 0;
		ClassAndSexInfo = 0;
		Face = 0;
		memset(Name, 0, sizeof(Name));
	}

	DWORD	NpcId;//NpcId
	BYTE	Type;//类型
	BYTE	Level;//等级
	BYTE	ClassAndSexInfo;//职业和性别信息
	BYTE	Face;//头像
	char	Name[32];//名字
} TEAM_MEMBER_INFO;

//队伍信息
typedef struct tagTeamInfo
{
	BYTE Protocol;
	WORD ProtocolSize;
	DWORD TeamServerId;
	DWORD CaptainId;//队长的NpcID
	BYTE IsBigTeam;//是否是大队伍
	DWORD TeamSetting;//队伍设置
	BYTE MemberCount;//成员数量
	TEAM_MEMBER_INFO Member[1];//成员信息
} TEAM_INFO;

//战场中前十名成员信息
typedef struct tagCombatTopMemberInfo
{
	tagCombatTopMemberInfo()
	{
		Level = 0;
		ClassAndSexInfo = 0;
		Score = 0;
		memset(Name, 0, sizeof(Name));
	}
	
	BYTE	Level; //等级
	BYTE	ClassAndSexInfo;//职业和性别信息
	char	Name[MAXSIZE_ROLENAME];//名字
	int		Score;
} COMBAT_TOP10_MEMBER_INFO;

//战场前十名排名信息
typedef struct tagCombatTopInfo
{
	BYTE Protocol;
	WORD ProtocolSize;
	BYTE MemberCount;
	COMBAT_TOP10_MEMBER_INFO Member[1];
} COMBAT_TOP10_INFO;

//更新队员信息
typedef struct tagUpdateTeamMemberInfo
{
	BYTE Protocol;
	TEAM_MEMBER_INFO Info;
} UPDATE_TEAM_MEMBER_INFO;

//服务器向客户端发送客户端自身队伍的信息数据
// typedef struct PLAYER_SEND_SELF_TEAM_INFO_DATA
// {
// 	PLAYER_SEND_SELF_TEAM_INFO_DATA()
// 	{
// 		nTeamServerID = -1;
// 		memset(m_dwNpcID, 0 ,sizeof(m_dwNpcID));
// 		memset(m_btLevel, 0, sizeof(m_btLevel));
// 		memset(m_bClassAndSexInfo, 0, sizeof(m_bClassAndSexInfo));
// 		memset(m_szNpcName, 0, sizeof(m_szNpcName));
// 	}
// 
// 	BYTE	ProtocolType;
// 	DWORD	nTeamServerID;//队伍在服务器上的唯一标识
// 	DWORD	CaptainID;//队长的NpcID
// 	DWORD	m_dwNpcID[MAX_TEAM_MEMBER];//每名成员的npc id （队长放在第一位）
// 	BYTE	m_btLevel[MAX_TEAM_MEMBER];//每名成员的等级（队长放在第一位）
// 	BYTE	m_bClassAndSexInfo[MAX_TEAM_MEMBER];//每名成员的职业和性别信息
// 	char	m_szNpcName[MAX_TEAM_MEMBER][32];//每名成员的名字（队长放在第一位）
// } PLAYER_SEND_SELF_TEAM_INFO;

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btFlags;			// 打开或关闭 以及　经验分享方式　
} PLAYER_TEAM_OPEN_CLOSE;		// 队伍队长向服务器申请开放、关闭队伍是否允许接收成员状态

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwTarNpcID;		// 目标队伍队长npc id 或者 申请人 npc id
} PLAYER_APPLY_ADD_TEAM;		// 玩家向服务器申请加入某个队伍或者服务器向某个队长转发某个玩家的加入申请

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwNpcID;			// 被接受入队伍的npc id
} PLAYER_ACCEPT_TEAM_MEMBER;	// 玩家通知服务器接受某个玩家入队伍

typedef struct PLAYER_TEAM_ADD_MEMBER_DATA
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	Level;				// 加入者等级
	DWORD	NpcID;				// 加入者npc id
	char	Name[32];			// 加入者姓名
	BYTE	ClassAndSexInfo;	// 职业和性别信息
	
	PLAYER_TEAM_ADD_MEMBER_DATA()
	{
		memset(Name, 0, 32);
	};
} PLAYER_TEAM_ADD_MEMBER;		// 服务器通知队伍中的各个玩家有新成员加入

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
} PLAYER_APPLY_LEAVE_TEAM;		// 客户端玩家申请离队

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwNpcID;			// 离队npc id
} PLAYER_LEAVE_TEAM;			// 服务器通知各队员某人离队

//队伍操作
typedef struct tagTeamOperation
{
	BYTE Protocol;
	BYTE Operation;
	DWORD NpcId;
} TEAM_OPERATION;

//队伍操作结果
typedef struct tagTeamOperationResult
{
	BYTE Protocol;
	BYTE Operation;
	DWORD NpcId;
	int Result;
} TEAM_OPERATION_RESULT;

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwNpcID;			// 离队npc id
} PLAYER_TEAM_KICK_MEMBER;		// 队长踢除某个队员

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwNpcID;			// 目标npc id
} PLAYER_APPLY_TEAM_CHANGE_CAPTAIN;// 队长向服务器申请把自己的队长身份交给别的队员

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwCaptainID;		// 新队长npc id
	DWORD	m_dwMemberID;		// 新队员npc id
} PLAYER_TEAM_CHANGE_CAPTAIN;	// 服务器通知各队员更换队长

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
} PLAYER_APPLY_TEAM_DISMISS;	// 向服务器申请解散队伍

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btPKFlag;			// pk 开关
} PLAYER_SET_PK;				// 向服务器申请打开、关闭PK

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
	BYTE	m_btCamp;				// 新阵营
	BYTE	m_btCurFaction;			// 当前门派
	BYTE	m_btFirstFaction;		// 首次加入门派
	int		m_nAddTimes;			// 加入门派次数
} PLAYER_FACTION_DATA;				// 服务器发给客户端门派信息

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
} PLAYER_LEAVE_FACTION;				// 服务器通知玩家离开门派

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
	BYTE	m_btCurFactionID;		// 当前门派id
	BYTE	m_btLevel;				// 技能开放等级
} PLAYER_FACTION_SKILL_LEVEL;		// 服务器通知玩家开放当前门派技能到某个等级

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
} PLAYER_APPLY_FACTION_DATA;		// 客户端申请更新门派数据

typedef struct PLAYER_SEND_CHAT_DATA_COMMAND
{
	BYTE	ProtocolType;		// 协议名称
	WORD	m_wLength;
	BYTE	m_btCurChannel;		// 当前聊天频道
	BYTE	m_btType;			// MSG_G_CHAT 或 MSG_G_CMD 或……
	BYTE	m_btChatPrefixLen;	// 格式控制字符长度
	WORD	m_wSentenceLen;		// 聊天语句长度
	DWORD	m_dwTargetID;		// 聊天对象 id
	int		m_nTargetIdx;		// 聊天对象在服务器端的 idx
	char	m_szSentence[MAX_SENTENCE_LENGTH + CHAT_MSG_PREFIX_MAX_LEN];	// 聊天语句内容
	PLAYER_SEND_CHAT_DATA_COMMAND() {memset(m_szSentence, 0, sizeof(m_szSentence));};
} PLAYER_SEND_CHAT_COMMAND;		// 客户端聊天内容发送给服务器

typedef struct PLAYER_SEND_CHAT_DATA_SYNC
{
	BYTE	ProtocolType;		// 协议名称
	WORD	m_wLength;
	BYTE	m_btCurChannel;		// 当前聊天状态
	BYTE	m_btNameLen;		// 名字长度
	BYTE	m_btChatPrefixLen;	// 控制字符长度
	WORD	m_wSentenceLen;		// 聊天语句长度
	DWORD	m_dwSourceID;		// 
	char	m_szSentence[32 + CHAT_MSG_PREFIX_MAX_LEN + MAX_SENTENCE_LENGTH];	// 聊天语句内容
	PLAYER_SEND_CHAT_DATA_SYNC() { memset(m_szSentence, 0, sizeof(m_szSentence)); };
} PLAYER_SEND_CHAT_SYNC;		// 客户端聊天内容发送给服务器

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btState;
	int		m_nID;
	int		m_nDataID;
	int		m_nXpos;
	int		m_nYpos;
	int		m_nMoneyNum;
	int		m_nItemID;
	BYTE	m_btDir;
	WORD	m_wCurFrame;
	BYTE	m_btColorID;
	BYTE	m_btFlag;
	char	m_szName[32];
} OBJ_ADD_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_btState;
	int		m_nID;
} OBJ_SYNC_STATE;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_btDir;
	int		m_nID;
} OBJ_SYNC_DIR;

typedef struct
{
	BYTE	ProtocolType;
	int		m_nID;
	BYTE	m_btSoundFlag;
	BYTE	m_bPickUp;
	int		m_nPlayerID;
} OBJ_SYNC_REMOVE;

typedef struct
{
	BYTE	ProtocolType;
	int		m_nID;
	int		m_nTarX;
	int		m_nTarY;
} OBJ_SYNC_TRAP_ACT;

typedef struct
{
	BYTE	ProtocolType;
	int		m_nID;
} OBJ_CLIENT_SYNC_ADD;


typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	NpcID;				// 升级者的NpcID
	BYTE	ArriveLevel;		// 当前等级
} PLAYER_LEVEL_UP_SYNC;			// 玩家升级

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btLevel;			// 当前等级
	DWORD	m_dwTeammateID;		// 队友 npc id
} PLAYER_TEAMMATE_LEVEL_SYNC;	// 玩家升级的时候通知队友

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btAttribute;		// 属性(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	int		m_nAddNo;			// 加的点数
} PLAYER_ADD_BASE_ATTRIBUTE_COMMAND;	// 玩家添加基本属性点

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nSkillID;			// 技能id
	int		m_nAddPoint;		// 要加的点数
} PLAYER_ADD_SKILL_POINT_COMMAND;// 玩家申请增加某个技能的点数

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btAttribute;		// 属性(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	int		m_nBasePoint;		// 基本点数
} PLAYER_ATTRIBUTE_SYNC;		// 玩家同步属性点

/*
带宽优化前
typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	Operation;

	int		SkillId;

	union
	{
		struct _ComInfo
		{
			int	Level;
			int	Status;
			int	CoolDownTime;

		} comInfo;

		int nCastSpeed;
		int	nCost;
	} PlusInfo;

} PLAYER_SKILLINFO_SYNC, *PPLAYER_SKILLINFO_SYNC;		// 玩家同步技能点
*/

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	Operation;

	WORD	SkillId;

	union
	{
		struct _ComInfo
		{
			BYTE Level;
			BYTE Status;
			int	CoolDownTime;

		} comInfo;

		short nCastSpeed;
		int	nCost;
	} PlusInfo;

} PLAYER_SKILLINFO_SYNC, *PPLAYER_SKILLINFO_SYNC;		// 玩家同步技能点

typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_Series;

} PLAYER_SKILLSERIES_SYNC;

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
}PLAYER_STOP_NOTIFY, *PPLAYER_STOP_NOTIFY;

typedef struct
{
    BYTE    ProtocolType;
	BYTE    nDir;
	int     nStopX;
	int     nStopY;             
}C2S_POS_SYNC,*PC2S_POS_SYNC;  //Player use

typedef struct
{
   BYTE             ProtocolType;

}S2C_PLAYER_STOP,*PS2C_PLAYER_STOP;

typedef struct  
{
   BYTE		ProtocolType;
   DWORD	nNpcID;
   int		nX;
   int		nY;
}S2C_POS_EDITION,*PS2C_POS_EDITION;

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btPlace;			// 药品位置
	BYTE	m_btX;				// 药品位置
	BYTE	m_btY;				// 药品位置
	BYTE	m_btTargetPlace;	// 药品位置
	BYTE	m_btTargetX;		// 药品位置
	BYTE	m_btTargetY;		// 药品位置
	int		m_nItemID;			// 物品id
	int		m_nTargetItemID;		
} PLAYER_EAT_ITEM_COMMAND;		// 玩家鼠标右键点击使用物品(吃药)

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nObjID;			// 鼠标点击的obj的id
	BYTE	m_btPosX;			// 坐标 x
	BYTE	m_btPosY;			// 坐标 y
} PLAYER_PICKUP_ITEM_COMMAND;		// 玩家获得物品（鼠标点击地图上的obj）

struct ItemIndex
{
	short nGenre;
	short nDetail;
	short nParticular;
	short nLevel;
};

struct ITEM_SYNC : COMPRESSED_PROTOCOL_HEADER
{
	BYTE			SyncType;					// 同步类型（enumItemSyncType）
	int				m_ID;						// 物品的ID
	BYTE			m_Genre;					// 物品的类型
	WORD			m_Detail;					// 物品的类别
	WORD			m_Particur;					// 物品的详细类别
	WORD			m_Level;					// 物品的等级
	BYTE			m_btPlace;					// 坐标
	BYTE			m_btX;						// 坐标
	BYTE			m_btY;						// 坐标
	int				m_Durability;				// 耐久度
	int				m_MaxDurability;			// 最大耐久度
	UINT			m_LevelupTimes;				// 升级的次数(必须用32位，因为被复用了)
	int				m_nLevelupType;
	unsigned short  m_btItemCount;				// 叠加物品的个数
	BYTE			m_YaoID;					// 爻属性编号
	int				m_YaoAddOnBuffSet[YAO_ADDON_BUFF_COUNT];	// 爻装附加属性
	WORD			m_compBuffTemplateSet[COMPOUND_COUNT];		// 道具合成后添加的buff数组
	char			m_szPlusInfo[ITEM_PLUS_INFO_LEN];
	WORD			m_TalismanPotential;		// 法宝蕴魂
	int				m_TalismanEnchaseSet[TM_HOLE_NUM];			// 法宝镶嵌
	bool			m_IsBind;					// 是否绑定
	int				m_LockCount;				// 锁定计数
	ItemIndex		m_socketSet[MAX_INLAY_COUNT];										 // 镶嵌槽位
	short			m_InlayBaseBuffSet[MAX_INLAY_COUNT][ITEM_BUFF_COUNT];//灵石附加buff
	short			m_InlayYaoBuffSet[MAX_ITEM_INLAY_YAO_EFFECT_COUNT];//灵石爻附加buff
	short			m_InlaySpecialBuffSet[MAX_SPECIALEFFECT_COUNT];//灵石组合附加buff
	DWORD			m_dwIBBuyTime;
	DWORD           m_CreditFlag;                                  //信贷标识
	int				m_MapID;
	int				m_MapX;
	int				m_MapY;
	BYTE			m_Step;
};

#define ITEM_SYNC_BUFF_LENGTH (sizeof(ITEM_SYNC) * 2)

struct INIT_ITEM_SYNC : public ITEM_SYNC 
{
	int				m_nMarkPrice;		//摆摊物品的标价
};

struct ITEM_REFRESH : COMPRESSED_PROTOCOL_HEADER
{
	int			m_nId;
	unsigned short m_btItemCount;				// 要修改的物品的叠加个数
	int			m_nAddMagicBuff;			// 道具合成后添加的buff数组
	ItemIndex	m_socketSet[MAX_INLAY_COUNT];										 // 镶嵌槽位
	short		m_InlayBaseBuffSet[MAX_INLAY_COUNT][ITEM_BUFF_COUNT];//灵石附加buff
	short		m_InlayYaoBuffSet[MAX_ITEM_INLAY_YAO_EFFECT_COUNT];//灵石爻附加buff
	short		m_InlaySpecialBuffSet[MAX_SPECIALEFFECT_COUNT];//灵石组合附加buff
};

typedef struct
{
	BYTE	ProtocolType;
	int		m_nId;
}SETITEMINVALID;

typedef struct
{
    BYTE byGiftType; //用户选择的离线奖励类型
    int nLSkillID; //左键技能ID
    int nRSkillID; //右键技能ID

}EXIT_INFO, *PEXIT_INFO;

typedef struct tagDamageShow
{
	BYTE			ProtocolType;
	BYTE			enType;
	int				nDamage;
	WORD			SkillId;
	BYTE			IsCrit;
	DWORD			dwReceiver;
	DWORD			dwLauncher;
} DAMAGESHOW, *PDAMAGESHOW;

enum
{
	plug_action_findplug,
	plug_action_answererror
};

enum
{
	question_type_none,
	question_type_pic,
	question_type_num
};

typedef struct _BYTE_EXTEND_HEADER
{
	BYTE Protocol;
	WORD wProtocolSize;
	BYTE ProtocolExtend;
}BYTE_EXTEND_HEADER,*PBYTE_EXTEND_HEADER;

struct Play_Animation : _BYTE_EXTEND_HEADER
{
	unsigned char		m_AnimationId1;
	unsigned char		m_AnimationId2;
	unsigned char		m_AnimationId3;
	unsigned char		m_AnimationId4;

	Play_Animation()
	{
		Protocol = 0;
		wProtocolSize = 0;
		ProtocolExtend = 0;
		m_AnimationId1 = 0;
		m_AnimationId2 = 0;
		m_AnimationId3 = 0;
		m_AnimationId4 = 0;
	}
};

struct SEND_CHANGEMAP : BYTE_EXTEND_HEADER
{
	BYTE	bShowElf;
	int		nMap;
	//后边跟byQuestionLen字节的数据
};

typedef struct _SEND_QUESTION : BYTE_EXTEND_HEADER
{
	WORD	wOffsetData;
	WORD	wQuestionLen;
	//后边跟byQuestionLen字节的数据
}SEND_QUESTION,*PSEND_QUESTION;

typedef struct _ASK_QUESTION : BYTE_EXTEND_HEADER
{
	BYTE	byQType;
	WORD	wQuestionLen;
	WORD	wCompress;
	char	szQuestionDescripte[100];
	char	szAnswerSet[1];
}ASK_QUESTION,*PASK_QUESTION;

typedef struct _ANSWER_QUESTION : BYTE_EXTEND_HEADER
{
	DWORD	dwAnswer;
}ANSWER_QUESTION,*PANSWER_QUESTION;

//fs2 added
typedef	struct _NpcNoMove : BYTE_EXTEND_HEADER 
{
	DWORD dwID;
	int nEnable;
} NPCNOMOVE, *PNPCNOMOVE;

typedef	struct _NpcNoSkill : BYTE_EXTEND_HEADER 
{
	int nEnable;
} NPCNOSKILL, *PNPCNOSKILL;

typedef	struct _NpcNoUseItem : BYTE_EXTEND_HEADER 
{
	int nEnable;
} NPCNOUSEITEM, *PNPCNOUSEITEM;
//

// Add by Cooler -->
// 2005-1-11
#define CHANGETYPE_BRONZE			0
#define CHANGETYPE_COPPERCASH		1
#define CHANGETYPE_TREASURE			2
typedef struct
{
	BYTE	ProtocolType;
	BYTE	byChangeMode;
	BYTE	byChangeType;
	int		nChangeValue;
}TIPS_CHANGEVALUEITEM;
// End add by Cooler <--

typedef struct
{
	BYTE			ProtocolType;		// 协议类型	
	int				m_ID;				// 物品的ID
} ITEM_REMOVE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_ID;				// 物品的ID
} PLAYER_SELL_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_BuyIdx;			// 买第几个东西
	BYTE			buyCount;			// 买几个？
} PLAYER_BUY_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nMoney1;			// 装备栏
	int				m_nMoney2;			// 贮物箱
	int				m_nMoney3;			// 交易栏
} PLAYER_MONEY_SYNC;					// 服务器通知客户端钱的数量

//Lucifer~yu[zhangjianyu] [03/17/2006] Add for new skill
//begin------------------------------------------------------------------------
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	unsigned int	sourPlace;
	unsigned int	sourX;
	unsigned int	sourY;
	unsigned int	destPlace;
	unsigned int	destX;
	unsigned int	destY;
} PLAYER_MOVE_ITEM_COMMAND;
//----->Add by [Ray] 2004-4-8
//typedef struct 
//{
//} PLAYER_MERGE_ITEM_SYNC;
//<-----Add End
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			sourPlace;
	BYTE			sourX;
	BYTE			sourY;
	BYTE			destPlace;
	BYTE			destX;
	BYTE			destY;
} PLAYER_MOVE_ITEM_SYNC;
//end--------------------------------------------------------------------------	



// s2c_ItemAutoMove
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btSrcPos;
	BYTE			m_btSrcX;
	BYTE			m_btSrcY;
	BYTE			m_btDestPos;
	BYTE			m_btDestX;
	BYTE			m_btDestY;
} ITEM_AUTO_MOVE_SYNC;

// Add by Cooler 2004-8-11
// Begin -->
typedef struct tagSPLITPILEITEM
{
	BYTE			ProtocolType;
	unsigned int	sourPlace;
	unsigned int	sourX;
	unsigned int	sourY;
	unsigned int	destPlace;
	unsigned int	destX;
	unsigned int	destY;
	unsigned int    splitItemCount;
}SPLITPILEITEM, *PSPLITPILEITEM;

typedef struct tagOPENSTOREBOX
{
	BYTE			ProtocolType;
	BYTE			byNeedPassword;
}OPENSTOREBOX, *POPENSTOREBOX;

typedef struct tagCHECKSTORAGEPSW
{
	BYTE			ProtocolType;
	char			szPassword[16];
}CHECKSTORAGEPSW, *PCHECKSTORAGEPSW;

typedef struct tagCREATESTORAGEPSW
{
	BYTE			ProtocolType;
	char			szPassword[16];
}CREATESTORAGEPSW, *PCREATESTORAGEPSW;

typedef struct tagMODIFYSTORAGEPSW
{
	BYTE			ProtocolType;
	char			szOldPassword[16];
	char			szNewPassword[16];
}MODIFYSTORAGEPSW, *PMODIFYSTORAGEPSW;

typedef struct tagREPLACESTORAGEPSW
{
	BYTE			ProtocolType;
	char			szSecondPassword[64];
}REPLACESTORAGEPSW, *PREPLACESTORAGEPSW;

typedef struct tagREQUESTREPLY
{
	BYTE			ProtocolType;
	BYTE			byReply;
}REQUESTREPLY, *PREQUESTREPLY;

typedef struct tagFINDPATHSYNC
{
	BYTE			ProtocolType;
	BYTE			byForce;
	DWORD			dwID;
	int				nPosX;
	int				nPosY;
}FINDPATHSYNC, *PFINDPATHSYNC;

typedef struct tagOPENPOSTER
{
	BYTE			ProtocolType;
	BYTE			byPosterID;
}OPENPOSTER, *POPENPOSTER;

typedef struct tagSYNCSKILLEXP
{
	BYTE			ProtocolType;
	int				nSkillID;
	int				nSkillExp;
}SYNCSKILLEXP, *PSYNCSKILLEXP;

typedef struct tagSYNCCREDIT
{
	BYTE			ProtocolType;
	int				nCredit;
}SYNCCREDIT, *PSYNCCREDIT;

typedef struct tagSYNC_MASTERPRVALUE
{
	BYTE ProtocolType;
	WORD wMasterPRValue;
} SYNC_MASTERPRVALUE, *PSYNC_MASTERPRVALUE;

typedef struct tagSYNCCURWEIGHT
{
	BYTE ProtocolType;
	BYTE byRoomIndex;
	int	 nWeightCur;
}SYNCCURWEIGHT, *PSYNCCURWEIGHT;

/*
带宽优化前
//队友信息
typedef struct tagTeammateInfo
{
	BYTE ProtocolType;
	DWORD Id;
	DWORD Life;
	DWORD LifeMax;
	DWORD Mana;
	DWORD ManaMax;
	WORD MapId;
	DWORD PosX;
	DWORD PosY;
} TEAMMATE_INFO;
*/

//队友信息
typedef struct tagTeammateInfo
{
	BYTE ProtocolType;
	DWORD Id;
	BYTE LifePercent;
	BYTE ManaPercent;
	WORD MapId;
	DWORD PosX;
	DWORD PosY;
} TEAMMATE_INFO;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				nItemID;			// 道具ID
} PLAYER_THROW_AWAY_ITEM_COMMAND;

/*
带宽优化前
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	short			m_shLife;
	short			m_shMana;
	short			m_shAngry;
	BYTE			m_btTeamData;
} CURPLAYER_NORMAL_SYNC;
*/

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			Life;
	WORD			Mana;
} CURPLAYER_NORMAL_SYNC;

typedef struct  
{
	BYTE			ProtocolType;
	int				Life;
	int				Mana;
} CURPLAYER_NORMAL_SYNC_EX;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	// npc部分
	BYTE			bRandomAddAttr;
	DWORD			m_dwID;				// Npc的ID
	BYTE			m_btLevel;			// Npc的等级
	BYTE			m_btSex;			// 性别
	BYTE			m_btKind;			// Npc的类型
	BYTE			m_btSeries;			// Npc的五行系
	char			m_SkillSeries;		// Npc的技能系
	int				m_wLifeMax;			// Npc的最大生命
	int				m_wManaMax;			// Npc的最大内力
	int				m_wCurLife;
	int				m_wCurMana;
	int				m_HeadImage;
	int				m_wBody;
	int				m_wNimbus;
	int				m_wStrength;
	int				m_wArt;
	DWORD			m_dwExp;			// 当前经验值
	DWORD			m_dwSkillExp;		// 技能经验值	
	WORD			m_wWorldStat;		// 世界排名
	int				m_nMoney1;
	int				m_nMoney2;
	int				nCredit;
	int				nWeightMax;
	BYTE			bIsBlockClientControl;	//是否拦截客户端控制
} CURPLAYER_SYNC;

#define MAX_SCIRPTACTION_BUFFERNUM 1024

/*
带宽优化前
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wProtocolLong;
	DWORD	m_dwNpcKind;
	BYTE	m_nOperateType;				//操作类型
	BYTE	m_bUIId, m_bOptionNum, m_bParam1, m_bParam2;// m_bParam1,主信息是数字标识还是字符串标识, m_bParam2,是否是与服务器交互的选择界面
	int		m_nParam;
	int		m_nBufferLen;
	char	m_pContent[MAX_SCIRPTACTION_BUFFERNUM];				//带控制符
} PLAYER_SCRIPTACTION_SYNC;
*/

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wProtocolLong;
	BYTE	m_nOperateType;				//操作类型
	BYTE	m_bUIId, m_bOptionNum, m_bParam1, m_bParam2;// m_bParam1,主信息是数字标识还是字符串标识, m_bParam2,是否是与服务器交互的选择界面
	int		m_nParam;
	int		m_nBufferLen;
	char	m_pContent[MAX_SCIRPTACTION_BUFFERNUM];				//带控制符
} PLAYER_SCRIPTACTION_SYNC;

// lixuewu 同步召唤兽状态
typedef struct 
{
	BYTE ProtocolType;
	WORD wSkillID;
	WORD wMaxLife;
	WORD wCurrentLife;

	// add by chenshanglin on 2006-2-21 for new skill system
	WORD wCreatureSkillID;
	// add end
}CREATURE_SYNC;

// Must align by 1 byte
typedef struct
{
	BYTE				ProtocolType;
	WORD				m_wProtocolLong;	
	BYTE				m_version;
	DBSkillData			m_sAllSkill[MAX_NPCSKILL];

} SKILL_SEND_ALL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	WeatherID;
} SYNC_WEATHER;

typedef struct defWORLD_SYNC
{
	BYTE	ProtocolType;
	WORD	SubWorld;
	int		Region;
	DWORD	Frame;
	DWORD	ExpireLeftTime;//过期剩余时间
} WORLD_SYNC;

struct PLAYER_SELECTUI_COMMAND
{
	BYTE	ProtocolType;
	//<---- Add By Ray [Luoliang] [2005-10-21]
	WORD	wLength;
	// End. Ray [LuoLiang] [2005-10-21] ---->
	DWORD	dwSeed;
	int		nSelectIndex;
	int		nSelectType;
	int		nParam1;
	int		nParam2;
	//<---- Add By Ray [Luoliang] [2005-10-21]
	int		nCount;
	// End. Ray [LuoLiang] [2005-10-21] ---->
	DWORD	pdwSelArray[1];
	PLAYER_SELECTUI_COMMAND()
	{
		nCount = nSelectIndex = nSelectType = nParam1 = nParam2 = 0;
	};
};

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	DWORD			m_dwTakeChannel;	// 订阅频道
} CHAT_SET_CHANNEL_COMMAND;				// 设定订阅频道

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;
	DWORD			m_dwTargetNpcID;	// 目标 npc id
	char			m_szInfo[MAX_SENTENCE_LENGTH];// 给对方的话
} CHAT_APPLY_ADD_FRIEND_COMMAND;		// 聊天添加好友

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nSrcPlayerIdx;	// 来源 player idx
	char			m_szSourceName[32];	// 来源玩家名字
	char			m_szInfo[MAX_SENTENCE_LENGTH];// 对方给的话
} CHAT_APPLY_ADD_FRIEND_SYNC;			// 聊天添加好友

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nTargetPlayerIdx;	// 被接受player idx
} CHAT_ADD_FRIEND_COMMAND;				// 添加某玩家为聊天好友

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nTargetPlayerIdx;	// 被拒绝player idx
} CHAT_REFUSE_FRIEND_COMMAND;			// 拒绝添加某玩家为聊天好友

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	DWORD			m_dwID;				// 新添加好友的 id
	int				m_nIdx;				// 新添加好友在 player 数组中的位置
	char			m_szName[32];		// 新添加好友的名字
} CHAT_ADD_FRIEND_SYNC;					// 通知客户端成功添加一个聊天好友

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;
	char			m_szName[32];		// 拒绝者名字
} CHAT_REFUSE_FRIEND_SYNC;				// 通知客户端添加聊天好友的申请被拒绝

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nTargetPlayerIdx;	// 出错 player idx (一般可能是此player下线或者换服务器了)
} CHAT_ADD_FRIEND_FAIL_SYNC;			// 通知客户端添加聊天好友失败

typedef struct
{
	BYTE			ProtocolType;	// c2s_viewequip
	DWORD			m_dwNpcID;
} VIEW_EQUIP_COMMAND;

/*
 * { Add by liupeng 2003.05.10
 *
 * #pragma pack( push, 1 )
*/

/*
 * Nonstandard extension used : zero-sized array in struct/union
 */
#pragma warning(disable: 4200)

#define KSG_PASSWORD_MAX_SIZE   64

typedef struct tagKSG_PASSWORD
{
    char szPassword[KSG_PASSWORD_MAX_SIZE];    // 现在采用MD5的字符串，由于是32个字符，加上末尾'\0'，需要至少33个空间，因此使用64
} KSG_PASSWORD;

struct tagProtoHeader
{
	BYTE	cProtocol;
};

#define	CREATE_ROLE_ERROR_NAME			0
#define CREATE_ROLE_SUCESS				1
#define	CREATE_ROLE_FAIL_NOT_ALLOW		2

#define	DEL_ROLE_NOT_ALLOW				0
#define DEL_ROLE_SUCESS					1
#define DEL_ROLE_FAIL_SOCIETY			2
#define DEL_ROLE_FAIL_ERROR_PASSWORD	3
#define DEL_ROLE_FAIL_ERROR_QUESTION	4
#define DEL_ROLE_FAIL_ERROR_ANSWER		5
#define DEL_ROLE_FAIL_ERROR_OVERANSWER	6
#define DEL_ROLE_SUCESS_ANSWER			7

struct tagDBSelPlayer : public tagProtoHeader
{
	union
	{
		char	szRoleName[_NAME_LEN];
		BYTE	cFailReason;				//见上面的定义FAIL_REASON_*
											//当tagNewDelRoleResponse::bSucceeded == false时启用此数据域
	};
};

struct tagDBDelPlayer : public tagProtoHeader
{
	char	        szAccountName[_NAME_LEN];
    KSG_PASSWORD    Password;
	char	        szRoleName[_NAME_LEN];
};

//删除与新建角色的返回消息带的数据
struct tagNewDelRoleResponse : public tagDBSelPlayer
{
	BYTE	bSucceeded;		//是否成功
};

#define MAX_EXT_POINT_COUNT   8
struct tagExtPointInfo
{
	DWORD nExtPoint[MAX_EXT_POINT_COUNT];
	DWORD dwLeftMoney;		//剩余金币数
	DWORD dwLimitPlayTimeFlag;	//防沉迷标识
	//0：表示该用户不纳入防沉迷管辖，并且没有实名信息（老用户）
	//1：表示该用户纳入防沉迷管辖，并且没有实名信息（老用户）
	//2：表示该用户不纳入防沉迷管辖，并且有实名信息
	//3：表示该用户纳入防沉迷管辖，并且有实名信息
	//255：表示防沉迷系统没有开启
	DWORD dwLimitOnlineSecond;	//防沉迷在线累计
	DWORD dwLimitOfflineSecond;	//防沉迷离线累计
};
/*
#define CHANGE_EXT_POINT_SILVER         0x1

struct tagChangeExtPoint : public tagProtoHeader
{
	BYTE szAccountName[_NAME_LEN];

    union
    {
    unsigned uExtPointIndex;    // 将要改变的附加点的索引
    unsigned uSilverType;       // 银票的类型，高16位（0：表示大银票，1：表示小银票）
                                //             低16位（0：转为点数，：转为包（周）月）
    };

    int      nChangeValue;      // 附加点被修改的值，可正可负，或者银票的数目

    unsigned uFlag;             // 如果是0表示附加点的变化，如果是CHANGE_EXT_POINT_SILVER : 表示银票的处理
    int      nPlayerIndex;      // 玩家的索引号，用来处理返回协议
};

struct tagReturnChangeExtPoint : public tagProtoHeader
{
	int nResult;

    BYTE szAccountName[_NAME_LEN];

    unsigned uFlag;

    int      nPlayerIndex;      // 玩家的索引号，用来处理返回协议
};
*/
struct RoleBaseInfo
{
	RoleBaseInfo()
	{
		szName[0]			= 0;
		Sex					= 0;
		Series				= 0;
		Level				= 0;
		btPortrait			= 0;
		bIsTongMember		= 0;
		byFreeze			= 0;
		byForbid			= 0;
		dwLastLoginTime		= 0;
		dwLastLoginIP		= 0;
		dwLastMapID			= 0;
		dwWillDestoryTime	= 0;
	}
	char	szName[32];			//角色名
	BYTE	Sex;				//角色性别
	BYTE	Series;				//角色职业
	BYTE	Level;				//角色等级
	BYTE	btPortrait;			//角色自选头像
	BYTE	bIsTongMember;		//是否有社会关系目前没有使用建议保留，可以限制有氏族的玩家不能删角色
	BYTE	byFreeze;		//目前没使用，建议保留，并服使用
	BYTE	byForbid;			//是否禁言
	DWORD	dwLastLoginTime;	//上次登陆时间,保留未使用
	DWORD	dwLastLoginIP;		//上次登陆IP,保留未使用
	DWORD	dwLastMapID;		//上次登陆地图,保留未使用
	DWORD	dwWillDestoryTime;	//还有多长时间自动销毁
	DWORD	dwEmployLeftTime;	//剩余雇佣时间
};
typedef RoleBaseInfo/* client */ S3DBI_RoleBaseInfo /* server */;

typedef struct 
{
	BYTE				ProtocolType;
	BYTE				bPermitCreate;
	BYTE				RoleCount;
	char				RoleList[1];
} ROLE_LIST_SYNC;

//新建角色的信息结构
//注释：新建决消息c2s_newplayer，传送的参数为TProcessData结构描述的数据，其中TProcessData::pDataBuffer要扩展为NEW_PLAYER_COMMAND
struct NEW_PLAYER_COMMAND : public tagProtoHeader
{
	BYTE			m_btRoleNo;			// 角色编号
	BYTE			m_btSeries;			// 五行系
	unsigned short	m_NativePlaceId;	//出生地ID
	unsigned char	m_btPortrait;		//肖像索引
	char			m_szName[32];		// 姓名

};

typedef struct
{
	BYTE			ProtocolType;
	BYTE			szAccName[32];
} LOGIN_COMMAND;


typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;
	char			m_szSentence[MAX_SENTENCE_LENGTH];
} TRADE_APPLY_OPEN_COMMAND;

typedef TRADE_APPLY_OPEN_COMMAND	GAMBLE_APPLY_OPEN_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
} TRADE_APPLY_CLOSE_COMMAND;

typedef TRADE_APPLY_CLOSE_COMMAND GAMBLE_APPLY_CLOSE_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btState;			// 0是拒绝交易、1是开始交易 2是对方正在交易
	DWORD			m_dwNpcID;			// 如果是开始交易，对方的 npc id
} TRADE_CHANGE_STATE_SYNC;
typedef TRADE_CHANGE_STATE_SYNC	GAMBLE_CHANGE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;
	DWORD			m_dwID;
	BYTE			m_btState;
	//-------> Ray [Luoliang] 2005-6-16
	//目前是为了摆摊的特效加的,以后可能作其他用途
	UINT			m_uReserved;		
	//<------- End [Ray]
	char			m_szSentence[MAX_SENTENCE_LENGTH];	
} NPC_SET_MENU_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	DWORD			m_dwID;
} TRADE_APPLY_START_COMMAND;
//Add by Ray [Luoliang]  2004-7-28
typedef TRADE_APPLY_START_COMMAND	GAMBLE_APPLY_START_COMMAND;

// 服务器转发交易申请
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	DWORD			oppositePlayerNpcId;			// 申请者的 npc id
} TRADE_APPLY_START_SYNC;
//Add by Ray [Luoliang]  2004-7-28
typedef TRADE_APPLY_START_SYNC	GAMBLE_APPLY_START_SYNC;

// 接受或拒绝别人的交易申请
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_bDecision;		// 同意 1 不同意 0
	int				oppositePlayerNpcId;	// 交易对方在服务器端的player id
} TRADE_REPLY_START_COMMAND;
//Add by Ray [Luoliang]  2004-7-28
typedef TRADE_REPLY_START_COMMAND GAMBLE_REPLY_START_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nMoney;
} TRADE_MOVE_MONEY_COMMAND;		// c2s_trademovemoney
//Add by Ray [Luoliang]  2004-7-28
typedef TRADE_MOVE_MONEY_COMMAND GAMBLE_MOVE_MONEY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nMoney;
} TRADE_MONEY_SYNC;				// s2c_trademoneysync
//Add by Ray [Luoliang]  2004-7-28
typedef TRADE_MONEY_SYNC GAMBLE_MONEY_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btDecision;		// 确定交易 1  退出交易 0  取消确定 4  锁定交易 2  取消锁定 3
} TRADE_DECISION_COMMAND;				// 交易执行或取消 c2s_tradedecision
//Add by Ray [Luoliang]  2004-7-28
typedef TRADE_DECISION_COMMAND GAMBLE_DECISION_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btDecision;		// 交易ok 1  交易取消 0  锁定 2  取消锁定 3
} TRADE_DECISION_SYNC;					// s2c_tradedecision
//Add by Ray [Luoliang]  2004-7-28
typedef struct  
{
	BYTE			ProtocolType;		// 协议类型

	BYTE			m_btDecision : 4;	// 赌博结果
	BYTE			m_btOtherChoice : 4;// 对方的选择
	
} GAMBLE_DECISION_SYNC;

typedef struct
{
	BYTE			ProtocolType;		
	BYTE			m_byDir;			// 取钱的方向（0存，1取）
	DWORD			m_dwMoney;			// 钱数
} STORE_MONEY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;			// 长度
	BYTE			m_btError;			// 错误类型	0 对方关闭了此频道，1 找不到对方
	char			m_szName[32];		// 对方名字
} CHAT_SCREENSINGLE_ERROR_SYNC;

typedef struct 
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btStateInfo[MAX_NPC_RECORDER_STATE];
	DWORD			m_ID;				// Npc的GID
}	NPC_SYNC_STATEINFO;

typedef struct
{
	BYTE			ProtocolType;		//协议类型
	char			PlayerName[32];		//玩家的名字
} TEAM_INVITE_ADD_COMMAND;

typedef struct
{
	BYTE			ProtocolType;//协议类型
	WORD			Length;
	int				InviterNpcId;
	char			InviterName[1];
} TEAM_INVITE_ADD_SYNC;

typedef struct
{
	BYTE			ProtocolType;//协议类型
	WORD			Length;
	int				PlayerNpcId;
	char			PlayerName[1];
} TEAM_APPLY_JOIN;

typedef struct
{
	BYTE			ProtocolType;		//
	int				m_nAuraSkill;
} SKILL_CHANGEAURASKILL_COMMAND;		//更换光环技能

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btResult;
	int				m_nIndex;
} TEAM_REPLY_INVITE_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btSelfLock;
	BYTE			m_btDestLock;
	BYTE			m_btSelfOk;
	BYTE			m_btDestOk;
} TRADE_STATE_SYNC;

typedef TRADE_STATE_SYNC GAMBLE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	DWORD			m_dwSkillID;		// 技能
	int				m_nLevel;
	int				m_nTime;			// 时间
//	KMagicAttrib	m_MagicAttrib[MAX_SKILL_STATE];
} STATE_EFFECT_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwTime;
} PING_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwReplyServerTime;
	DWORD			m_dwClientTime;
} PING_CLIENTREPLY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btSitFlag;
} NPC_SIT_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	int				nMpsX;
	int				nMpsY;
} NPC_JUMP_COMMAND;

typedef struct tagS2C_WORLD_PLAYER_INFO_SYNC
{
	BYTE			ProtocolType;
	WORD            Len;
	WORD            InfoNum;
	BYTE            Infos[1];    //Info Num * WORLD_PLAYER_INFO
}S2C_WORLD_PLAYER_INFO_SYNC;

typedef struct tagS2C_WAR_COMMANDER_INFO_SYNC
{
	BYTE			ProtocolType;
	WORD            Len;
	WORD            InfoNum;
	BYTE            Infos[1];    //Info Num * WAR_COMMANDER_INFO
}S2C_WAR_COMMANDER_INFO_SYNC;

typedef struct tagWAR_COMMANDER_INFO
{
	BYTE			nDuty;
	DWORD			dNpcId;
	int				nMpsX;
	int				nMpsY;
}WAR_COMMANDER_INFO;

typedef struct
{
	BYTE			ProtocolType;
	int				m_dwRegionID;
	int				m_nObjID;
} OBJ_MOUSE_CLICK_SYNC;

typedef struct tagSHOW_MSG_SYNC
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	WORD			m_wMsgID;
	LPVOID			m_lpBuf;
	tagSHOW_MSG_SYNC() {m_lpBuf = NULL;};
	~tagSHOW_MSG_SYNC() {Release();}
	void	Release() {if (m_lpBuf) delete []m_lpBuf; m_lpBuf = NULL;}
} SHOW_MSG_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btState;
} PK_APPLY_NORMAL_FLAG_COMMAND;

typedef struct _ChgPKMode
{
	BYTE	protocol;
	WORD    mode;
} Chg_PK_Mode, *PChg_PK_Mode;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btFlag;
} PK_NORMAL_FLAG_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwNpcID;
} PK_APPLY_ENMITY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	BYTE			m_btState;
	DWORD			m_dwNpcID;
	char			m_szName[32];
} PK_ENMITY_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	BYTE			m_btState;
	DWORD			m_dwNpcID;
	char			m_szName[32];
} PK_EXERCISE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	int				m_nPKValue;
} PK_VALUE_SYNC;

/*
typedef struct
{
	int		m_nID;				// 物品的ID
	BYTE	m_btGenre;			// 物品的类型
	BYTE	m_btDetail;			// 物品的类别
	BYTE	m_btParticur;		// 物品的详细类别
	BYTE	m_btSeries;			// 物品的五行
	BYTE	m_btLevel;			// 物品的等级
	BYTE	m_btLuck;			// MF
	BYTE	m_btMagicLevel[6];	// 生成参数
	WORD	m_wVersion;			// 装备版本
	DWORD	m_dwRandomSeed;		// 随机种子
} SViewItemInfo;
*/
//将上面的结构改为struct SViewItemInfo,这样使得可以继承(生成构造函数).
//在c++里面,struct SViewItemInfo == SViewItemInfo,所以不会影响到其它的地方
//
struct SViewItemInfo
{
	int				m_ID;						// 物品的ID
	int				m_Genre;					// 物品的类型
	int				m_Detail;					// 物品的类别
	int				m_Particur;					// 物品的详细类别
	int				m_Level;					// 物品的等级
	BYTE			m_btPlace;					// 坐标
	BYTE			m_btX;						// 坐标
	BYTE			m_btY;						// 坐标
	WORD			m_Durability;				// 耐久度
	WORD			m_MaxDurability;			// 最大耐久度
	UINT			m_LevelupTimes;				// 升级的次数(必须用32位，因为被复用了)
	int				m_nLevelupType;
	BYTE			m_btItemCount;				// 叠加物品的个数
	BYTE			m_YaoID;					// 爻属性编号
	int				m_YaoAddOnBuffSet[YAO_ADDON_BUFF_COUNT];	// 爻装附加属性
	WORD			m_compBuffTemplateSet[COMPOUND_COUNT];		// 道具合成后添加的buff数组
	char			m_szPlusInfo[ITEM_PLUS_INFO_LEN];
	WORD			m_TalismanPotential;		// 法宝蕴魂
	int				m_TalismanEnchaseSet[TM_HOLE_NUM];			// 法宝镶嵌
	bool			m_bExchange;			// 法宝镶嵌
	ItemIndex		m_socketSet[MAX_INLAY_COUNT];										 // 镶嵌槽位
	short			m_InlayBaseBuffSet[MAX_INLAY_COUNT][ITEM_BUFF_COUNT];//灵石附加buff
	short			m_InlayYaoBuffSet[MAX_ITEM_INLAY_YAO_EFFECT_COUNT];//灵石爻附加buff
	short			m_InlaySpecialBuffSet[MAX_SPECIALEFFECT_COUNT];//灵石组合附加buff
} ;

typedef struct 
{
	BYTE         Protocol;
	WORD         Len;
	BYTE         data[1];

}VIEW_EQUIP_SYNC;

typedef struct
{
	DWORD			npcId;							//npcid
	BYTE            pkValue;						//pk值
	BYTE			shengwang;						//声望
	BYTE			level;							//等级
	char			name[MAXSIZE_ROLENAME];			//姓名
	char			shizu[MAXSIZE_ORGNAME];			//氏族
	char			zhuhou[MAXSIZE_ORGNAME];		//诸侯
	char			lianmen[MAXSIZE_ORGNAME];		//联盟
	char			chenghao[MAXSIZE_ROLENAME];		//称号
	
	SViewItemInfo	m_sInfo[itempart_num];
} VIEW_EQUIP_SYNC_INFO;				                // s2c_viewequip

typedef struct//该结构是所统计的玩家的基本数据
{
	char	Name[20];
	int		nValue;
	BYTE	bySort;
}TRoleList;

// 游戏统计结构
typedef struct
{
	TRoleList MoneyStat[10];			//金钱最多排名列表（十个玩家，最多可达到100个）
	TRoleList LevelStat[10];			//级别最多排名列表（十个玩家，最多可达到100个）
	TRoleList KillerStat[10];			//杀人最多排名列表
	
	//[门派号][玩家数]，其中[0]是没有加入门派的玩家
	TRoleList MoneyStatBySect[11][10];	//各门派金钱最多排名列表
	TRoleList LevelStatBySect[11][10];	//各门派级别最多排名列表

	//[门派号]，其中[0]是没有加入门派的玩家
	int SectPlayerNum[11];				//各个门派的玩家数
	int SectMoneyMost[11];				//财富排名前一百玩家中各门派所占比例数
	int SectLevelMost[11];				//级别排名前一百玩家中各门派所占比例数
}  TGAME_STAT_DATA;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	dwItemID;
	DWORD	special;
} ITEM_REPAIR;

//////////////////////////////////////////////////////////////////////////
// 召唤兽扩展协议结构
// lixuewu 2004.02.12
typedef struct
{
	BYTE ProtocolType;
	BYTE Command;      //命令
	DWORD nParam1;     //参数
} CREATURE_EXTEND;
// lixuewu 2004.02.12

typedef struct //服务器要求客户端同步是否新手
{
	BYTE ProtocolType;			// 协议号
	BYTE bNewPlayer;
}SYNC_PLAYER_NEW;

typedef struct tagNPCREALPOSITION
{
	BYTE	ProtocolType;
	DWORD	dwNpcID;
	int		nPosX;
	int		nPosY;
	int		nDirection;
}NPCREALPOSITION, *PNPCREALPOSITION;

/*
带宽优化前
struct _Buff_Add : BYTE_EXTEND_HEADER 
{
	DWORD dwNpcID;
	unsigned long ulBuffID;
	unsigned long ulTempID;
	unsigned long ulTime;
	int		nPileCount;
};

struct _Buff_Info : BYTE_EXTEND_HEADER 
{
	DWORD dwNpcID;
	unsigned long ulBuffID;
	unsigned long ulTempID;
	unsigned long ulTime;
	int		nPileCount;
};

struct _Buff_Del : BYTE_EXTEND_HEADER 
{
	DWORD dwNpcID;
	unsigned long ulBuffID;
};

struct _Buff_Sync_Npc : BYTE_EXTEND_HEADER
{
	struct _Pair {
		DWORD	dwBuffTempID;
		DWORD	dwBuffTID;
	};
	DWORD	dwID;
	WORD	wCount;
	_Pair	Buff[1];
};

struct _Buff_Sync_Npc_Add : BYTE_EXTEND_HEADER 
{
	DWORD	dwID;
	unsigned long ulTempID;
};

struct  _Buff_Cancel : BYTE_EXTEND_HEADER  
{
	unsigned long ulBuffID;
};
*/

struct _Buff_Add : BYTE_EXTEND_HEADER 
{
	DWORD dwNpcID;
	DWORD ulBuffID;
	WORD ulTempID;
	DWORD ulTime;
	WORD nPileCount;
};

struct _Buff_Info : BYTE_EXTEND_HEADER 
{
	DWORD dwNpcID;
	DWORD ulBuffID;
	WORD ulTempID;
	DWORD ulTime;
	WORD nPileCount;
};

struct _Buff_Del : BYTE_EXTEND_HEADER 
{
	DWORD dwNpcID;
	DWORD ulBuffID;
};

struct _Buff_Sync_Npc : BYTE_EXTEND_HEADER
{
	struct _Pair {
		WORD	dwBuffTempID;
		DWORD	dwBuffTID;
	};
	DWORD	dwID;
	BYTE	wCount;
	_Pair	Buff[1];
};

struct _Buff_Sync_Npc_Add : BYTE_EXTEND_HEADER 
{
	DWORD	dwID;
	WORD ulTempID;
};

struct  _Buff_Cancel : BYTE_EXTEND_HEADER  
{
	DWORD ulBuffID;
};

/*
带宽优化前
typedef struct _Sync_NpcAttr
{
	BYTE	prototol;
	WORD	len;
	BYTE	subProtocol;
	DWORD	npcId;
	WORD	attrIdx;
	WORD	valueMask;
	BYTE	data[1];

} SYNC_NPCATTR, *PSYNC_NPCATTR;
*/

typedef struct _Sync_NpcAttr
{
	BYTE	prototol;
	WORD	len;
	BYTE	subProtocol;
	DWORD	npcId;
	BYTE	attrIdx;
	BYTE	valueMask;
	BYTE	data[1];

} SYNC_NPCATTR, *PSYNC_NPCATTR;

struct _ShortCut_Add : BYTE_EXTEND_HEADER 
{
	DWORD dwSCType;
	DWORD dwPos;
	DWORD dwID;
};

struct _ShortCut_Del : BYTE_EXTEND_HEADER 
{
	DWORD dwPos;
};

typedef struct tagSYNC_PLAYERATTR
{
	BYTE Protocol;
	BYTE Attribute;
	DWORD Value;
} SYNC_PLAYERATTR, *PSYNC_PLAYERATTR;

typedef struct tagSYNC_ITEM_ATTR
{
	BYTE Protocol;
	DWORD ItemID;
	BYTE Attribute;
	DWORD Value;
} SYNC_ITEM_ATTR, * PSYNC_ITEM_ATTR;

struct _ItemGroupCD_Add : BYTE_EXTEND_HEADER 
{
	int nGroup;
	unsigned long ulCDTime;
};

struct _ItemGroupCD_Del : BYTE_EXTEND_HEADER 
{
	int nGroup;
};

struct _StatueInfo : BYTE_EXTEND_HEADER
{
	char cName[MAXSIZE_ROLENAME];
	char cTongName[MAXSIZE_ORGNAME];
	int  nMapId;
	DWORD dwtime;
	BYTE  HasBuff;
};

struct tagMsgRelationPrivilege
{
	BYTE Layer;
	BYTE OperationId;
};

typedef struct tagSyncSocialRelation
{
	BYTE Protocol;
	WORD Len;
	char data[1];
	
} SYNC_SOCIAL_RELATION, *PSYNC_SOCIAL_RELATION;

typedef struct tagSyncSocialRelationInfo
{
	int TemplateId;
	int TopLayer;	
	int PrivilegeCount;
	tagMsgRelationPrivilege Privileges[enSUO_Num * MAX_SOCIETY_LAYER_COUNT];
	char Names[MAX_SOCIETY_LAYER_COUNT][17];
	BYTE OwnerFlag[MAX_SOCIETY_LAYER_COUNT];
	DWORD CityMapId;
} SYNC_SOCIAL_RELATION_INFO, *PSYNC_SOCIAL_RELATION_INFO;

typedef struct tagDelayedAction
{
	BYTE Protocol;
	BYTE Command;	
	DWORD Time;
	BYTE Message;
} DELAYED_ACTION, *PDELAYED_ACTION;

typedef struct tagTalismanOperation
{
	BYTE Protocol;
	BYTE SubProtocol;
	DWORD Params[3];
} TALISMAN_OPERATION, *PTALISMAN_OPERATION;

typedef struct tagSyncTalismanEnchase
{
	BYTE Protocol;
	DWORD ItemId;
	int EnchaseSet[TM_HOLE_NUM];
} SYNC_TALISMAN_ENCHASE, *PSYNC_TALISMAN_ENCHASE;

/*
带宽优化前
typedef struct tagSyncNpcEquipTalisman
{
	BYTE Protocol;
	DWORD NpcId;
	int TalismanNpcId;
} SYNC_NPC_EQUIP_TALISMAN, *PSYNC_NPC_EQUIP_TALISMAN;
*/

typedef struct tagSyncNpcEquipTalisman
{
	BYTE Protocol;
	DWORD NpcId;
	WORD TalismanNpcId;
} SYNC_NPC_EQUIP_TALISMAN, *PSYNC_NPC_EQUIP_TALISMAN;

typedef struct tagTeamInviteRefuse
{
	BYTE Protocol;
	WORD Length;
	char PlayerName[1];
} TEAM_INVITE_REFUSE, *PTEAM_INVITE_REFUSE;

//显示预制消息
typedef struct tagShowPredefinedMsg
{
	BYTE Protocol;
	WORD MessageId;
} SHOW_PREDEFINED_MSG, *PSHOW_PREDEFINED_MSG;

#define MAX_SHOW_BANNER_MSG_LENGTH 256
#define MAX_SHOW_BANNER_FONT_LENGTH 64
#define COMMON_SHOW_BANNER_BUFF_LENGTH (sizeof(SHOW_BANNER) + MAX_SHOW_BANNER_MSG_LENGTH + MAX_SHOW_BANNER_FONT_LENGTH + 2)

//显示滚动标题
typedef struct tagShowBanner
{
	tagShowBanner()
	{
		Colour = 0;
		speed = 50;
		second = 1;
		bannerType = 1;
	}

	BYTE Protocol;
	WORD Length;
	int  Colour;
	int	 speed;			//type=1时用来表示滚动速度，type=2时用来表示第几个banner（总共是4个）
	int	 second;		//type=2时用来表示循环次数，type=2时用来表示图片id
	BYTE bannerType;	//1表示滚动banner，2表示氏族banner
	char data[1];
} SHOW_BANNER, *PSHOW_BANNER;

//显示滚动标题
typedef struct tagShowBannerId
{
	tagShowBannerId()
	{
		msgId = 0;
		fontId = 0;
		color = 0;
		speed = 50;
		second = 1;
		bannerType = 1;
	}

	BYTE Protocol;
	int  msgId;
	int	 fontId;
	int  color;
	int	 speed;			//type=1时用来表示滚动速度，type=2时用来表示第几个banner（总共是4个）
	int	 second;		//type=2时用来表示循环次数，type=2时用来表示图片id
	unsigned char bannerType;	//1表示滚动banner，2表示氏族banner
} SHOW_BANNER_ID, *PSHOW_BANNER_ID;

//TaisuiWheel protocool 
typedef struct tagTAISUI_WHEEL_PROTOCOL_HEADER
{
  unsigned char  	Protocol;
  unsigned short	Len;
  unsigned char     SubProtocol;
  unsigned char     SubSize;
}TAISUI_WHEEL_PROTOCOL_HEADER;

#define MAX_SERVER_PROMPT_STR_PARAM_LENGTH 32		//服务器提示最大字符串参数长度

//服务器提示（请求客户端选择）
typedef struct tagServerPrompt
{
	BYTE Protocol;
	WORD Length;
	BYTE Event;
	int Param[2];
	char StrParam[1];
} SERVER_PROMPT;

//取消服务器提示
typedef struct tagCancelServerPrompt
{
	BYTE Protocol;
	BYTE Event;
} CANCEL_SERVER_PROMPT;

//客户端答复提示（选择了Yes或No）
typedef struct tagClientReplyPrompt
{
	BYTE Protocol;
	BYTE Event;
	BYTE Reply;
} CLIENT_REPLY_PROMPT;

//选中技能
typedef struct tagSelectSkill
{
	BYTE Protocol;
	int SkillID;
	int SkillParam1;
	int SkillParam2;
} SELECT_SKILL;

//NPC世界同步（初始）
typedef struct tagNpcSyncToWorldMin
{
	BYTE Protocol;
	DWORD NpcId;
	char Name[32];
	int Mode;
	int Param[MAX_SYNC_TO_WORLD_PARAM_COUNT];
	DWORD PosX;
	DWORD PosY;
} NPC_SYNC_TO_WORLD_MIN, *PNPC_SYNC_TO_WORLD_MIN;

//NPC世界同步（每次）
typedef struct tagNpcSyncToWorld
{
	BYTE Protocol;
	DWORD NpcId;
	int Param[MAX_SYNC_TO_WORLD_PARAM_COUNT];
	DWORD PosX;
	DWORD PosY;
} NPC_SYNC_TO_WORLD, *PNPC_SYNC_TO_WORLD;

//请求NPC世界同步
typedef struct tagRequestNpcSyncToWorld
{
	BYTE Protocol;
	DWORD NpcId;
} REQUEST_NPC_SYNC_TO_WORLD, *PREQUEST_NPC_SYNC_TO_WORLD;

//NPC世界同步（删除）
typedef struct tagNpcSyncToWorldDel
{
	BYTE Protocol;
	DWORD NpcId;
} NPC_SYNC_TO_WORLD_DEL, *PNPC_SYNC_TO_WORLD_DEL;

//特殊任务
typedef struct tagSpecialQuestData
{
	BYTE Protocol;
	int QuestId;
	ChangedSpecialQuestData	Data;
} SPECIAL_QUEST_DATA, *PSPECIAL_QUEST_DATA;

//队伍列表
typedef struct tagTeamList
{
	BYTE Protocol;
	WORD ProtocolSize;
	TeamBasicInfo TeamList[1];//队伍信息列表
} TEAM_LIST, *PTEAM_LIST;

//请求队伍列表
typedef struct tagRequestTeamList
{
	BYTE Protocol;
	BYTE ListMode;
	BYTE Limit;
	BYTE Filter;
	int FilterParam;
} REQUEST_TEAM_LIST, *PREQUEST_TEAM_LIST;

//请求特殊任务数据
typedef struct tagReqSpecialQuestData
{
	BYTE Protocol;
	int QuestId;
} REQ_SPECIAL_QUEST_DATA, *PREQ_SPECIAL_QUEST_DATA;

//发送GM问题
typedef struct tagGMCommunicationData
{
	BYTE Protocol;
	WORD Length;
	GMCommunicationData data;
} GM_COMMUNICATION_DATA, *PGM_COMMUNICATION_DATA;

/**************雇佣*****************/
//发送被雇佣请求
typedef struct tagReqTobeHiredData
{
	BYTE Protocol;
	int hireType;
	int moneyOrExpType;
}REQ_TO_BE_HIRED_DATA, *PREQ_TO_BE_HIRED_DATA;

//发送查询雇佣列表请求
typedef struct tagReqHireListData
{
	BYTE Protocol;
	HireReqData filter;
}REQ_HIRE_LIST_DATA, *PREQ_HIRE_LIST_DATA;

#define MAX_HIRE_LIST_SYNC_BUFF_LENGTH (sizeof(EXP_HIRE_LIST) * 2)
#define MAX_HIRE_LIST_COUNT 8

//雇佣列表回包(exp)
typedef struct tagExpHireList
{
	BYTE Protocol;
	WORD Length;
	BYTE Count;
	int	stargIndex;
	ExpHirer data[MAX_HIRE_LIST_COUNT];
}EXP_HIRE_LIST, *PEXP_HIRE_LIST;

//雇佣列表回包(fighter)
typedef struct tagFighterHireList
{
	BYTE Protocol;
	WORD Length;
	BYTE Count;
	int	stargIndex;
	FighterHirer data[MAX_HIRE_LIST_COUNT];
}FIGHTER_HIRE_LIST, *PFIGHTER_HIRE_LIST;

//发送雇佣请求
typedef struct tagReqHireData
{
	BYTE Protocol;
	char Name[32];
}REQ_HIRE_DATA, *PREQ_HIRE_DATA;

//服务器端回复雇佣请求错误码
typedef struct tagHireRetCode
{
	BYTE Protocol;
	int ret;
}HIRE_RET_CODE, *PHIRE_RET_CODE;

#define MAX_SCORE_ORG_SYNC 2
typedef struct tagWorldCombatInfo
{
	BYTE  Protocol;
	DWORD nScore[MAX_SCORE_ORG_SYNC];
}WORLD_COMBAT_INFO;

//交互脚本输入
#define INTERACTIVE_SCRIPT_INPUT_LENGTH 64
typedef struct tagInteracitveScriptInput
{
	BYTE Protocol;
	char Input[INTERACTIVE_SCRIPT_INPUT_LENGTH];
} INTERACTIVE_SCRIPT_INPUT, *PINTERACTIVE_SCRIPT_INPUT;

//徒弟信息
struct StudentInfo
{
	char Name[MAXSIZE_ROLENAME];
	BYTE Level;
	DWORD RewardMoeny;
	DWORD TotalRewardMoney;
//	DWORD LeftTime;
};

//徒弟列表
struct LIST_STUDENT : COMPRESSED_PROTOCOL_HEADER
{
	int TotalRewardToAdd;
	int TotalRewardTicketAdded;
	BYTE StudentCount;
	StudentInfo StudentInfoData[1];
};

//推荐人操作协议
typedef struct tagRecommenderOp
{
	BYTE Protocol;
	BYTE OpType;
}RECOMMENDER_OP, *PRECOMMENDER_OP;

//问答-提问
struct QUESTION : BYTE_EXTEND_HEADER
{
	BYTE Timeout;
	BYTE IsCompressed;
	WORD AppendDescStrId;
	char QuestionData[1];
};

struct COMBAT_MAP_RESULT_ORG_2 : BYTE_EXTEND_HEADER
{
	WORD   wCombatMapTemplateID;
	DWORD  dwPersistTime;
	BYTE   nOrg[2];
	int    nScore[2];
	int    nSelfScoreGet;
};

#define MAX_ANSWER_SIZE 16

//问答-回答
struct ANSWER : BYTE_EXTEND_HEADER
{
	char Answer[1];
};

#define COMMON_QUESTION_BUFF_SIZE (1024 * 10)

//GM操作
struct GM_OPERATION : BYTE_EXTEND_HEADER
{
	BYTE OpType;
	int OpParam1;
	int OpParam2;
	int OpParam3;
	char PlayerName[MAXSIZE_ROLENAME];
};

#define MAX_WORLD_CUSTOM_STRING_LENGTH 256
#define WORLD_CUSTOM_STRING_PROTOCOL_BUFF ((sizeof(WORLD_CUSTOME_STRING) + MAX_WORLD_CUSTOM_STRING_LENGTH) * 2)

struct WORLD_CUSTOME_STRING : COMPRESSED_PROTOCOL_HEADER
{
	char CustomString[1];
};

//改变称号
typedef struct tagChangeTitle
{
	BYTE Protocol;
	DWORD NpcID;
	BYTE TitleIndex;
	BYTE TitleLevel;
} CHANGE_TITLE, *PCHANGE_TITLE;

//称号信息
struct TransferTitleInfo
{
	BYTE TitleIndex;
	union
	{
		DWORD ExpireTime;
		struct
		{
			WORD Level;
			WORD Value;
		} LevelInfo;
	};
};

//更新自己的称号
typedef struct tagUpdateSelfTitle
{
	BYTE Protocol;
	BYTE IsActive;
	TransferTitleInfo Info;
} UPDATE_SELF_TITLE, *PUPDATE_SELF_TITLE;

//同步自己的称号
typedef struct tagSyncSelfTitle
{
	BYTE Protocol;
	WORD Length;
	char SelectedTitle;
	BYTE TitleCount;
	TransferTitleInfo InfoList[0];
} SYNC_SELF_TITLE, *PSYNC_SELF_TITLE;

//选择称号
typedef struct tagSelectTitle
{
	BYTE Protocol;
	char TitleIndex;
} SELECT_TITLE, *PSELECT_TITLE;

//氏族人气信息
struct tagShizuPopularityInfo
{
	char Name[17];
	WORD PlayerCount;
	DWORD Popularity;
};

//氏族人气排行
struct SHIZU_POPULARITY : COMPRESSED_PROTOCOL_HEADER
{
	BYTE Count;
	tagShizuPopularityInfo List[0];
};

//诸侯人气信息
struct tagZhuhouPopularityInfo
{
	char Name[17];
	WORD PlayerCount;
	DWORD Popularity;
};

//诸侯人气排行
struct ZHUHOU_POPULARITY : COMPRESSED_PROTOCOL_HEADER
{
	BYTE Count;
	tagZhuhouPopularityInfo List[0];
};

//战场击杀信息
struct tagCombatKillRankInfo
{
	char Name[17];
	BYTE Level;
	char Shizu[17];
	char Zhuhou[17];
	DWORD CombatKill;
};

//战场击杀排行
struct COMBAT_KILL_RANK : COMPRESSED_PROTOCOL_HEADER
{
	BYTE Count;
	tagCombatKillRankInfo List[0];
};

//玩家属性
typedef struct tagPlayerProperties
{
	BYTE Protocol;
	DWORD NpcId;
	DWORD ActiveDegreeTotal;
	DWORD ActiveDegreeCurrent;
	DWORD ShizuPopularity;
	BYTE ShizuPlayerCount;
	DWORD ZhuhouPopularity;
	WORD ZhuhouPlayerCount;
	BYTE ZhuhouShizuCount;
} PLAYER_PROPERTIES, *PPLAYER_PROPERTIES;

//刷新玩家属性页属性
struct REFRESH_PLAYER_PROPERTIES : BYTE_EXTEND_HEADER
{
	DWORD NpcId;
};

/************************************************************************/
/*							Login protocol                              */
/************************************************************************/
/*!
\brief
	Login process use const define, the following values are used in login process.		
*/
#define	LOGIN_ACCOUNT_MIN_LEN				6			//!< Login account min lenght.
#define LOGIN_ACCOUNT_MAX_LEN				16			//!< Login account max lenght.
#define LOGIN_PASSWORD_MIN_LEN				8			//!< Login password min lenght.
#define LOGIN_PASSWORD_MAX_LEN				16			//!< Login password max lenght.
#define LOGIN_ROLE_NAME_MIN_LEN				4			//!< Login role name min lenght.	
#define	LOGIN_ROLE_NAME_MAX_LEN				12			//!< Login role name max lenght.	

/*!
\brief
	Login process use time limit define, the following values are used in login process.		
*/
#define	DEF_TIMEOUT_LIMIT					60000		//!< Default time limit.
#define CONNECT_GAMESERVER_TIMEOUT_LIMIT	30000		//!< Connect gameserver time limit.
#define VALIDATE_ACT_AND_PW_TIMEOUT_LIMIT	30000		//!< Validate account and password time limit.	
#define GET_ROLE_LIST_TIMEOUT_LIMIT			30000		//!< Get role list time limit.
#define CREATE_ROLE_TIMEOUT_LIMIT			30000		//!< Create role time limit.
#define DELETE_ROLE_TIMEOUT_LIMIT			30000		//!< Delete role time limit.
#define SELECT_ROLE_TIMEOUT_LIMIT			30000		//!< Select role time limit.
#define LOGIN_GAMESERVER_TIMEOUT_LIMIT		30000		//!< Login gameserver time limit.

/*!
\brief
	Login action value, the following values are used in login connect operation.		
*/
#define	LOGIN_ACTION_FILTER					0xff0000	//!< Login action type filter.
#define LOGIN_A_CONNECT						0x010000	//!< Connect action.
#define	LOGIN_A_NEWACCOUNT					0x020000	//!< New account action.
#define	LOGIN_A_SERVERLIST					0x030000	//!< Get server list action.(unused)
#define	LOGIN_A_REPORT						0x040000	//!< Tell the gameserver client is online.
#define	LOGIN_A_LOGIN						0x050000	//!< Login the gameserver action.
#define	LOGIN_A_LOGOUT						0x060000	//!< Logout action
#define	LOGIN_A_CHARACTERLIST				0x070000	//!< Get role list action.
#define LOGIN_A_LOGINSIGN					0x100000	//!< Validate account and password action.

/*!
\brief
	Login result value, the following values are used in login connect operation.		
*/
#define	LOGIN_R_REQUEST						0			//!< when the login request is send form client to server.
#define	LOGIN_R_SUCCESS						1			//!< Login operator succeed. 
#define	LOGIN_R_FAILED						2			//!< Login operator failed.
#define	LOGIN_R_ACCOUNT_OR_PASSWORD_ERROR	3			//!< Login account or password error.
#define	LOGIN_R_ACCOUNT_EXIST				4			//!< Login account already exist.
#define	LOGIN_R_TIMEOUT						5			//!< Login operator timeout.
#define	LOGIN_R_IN_PROGRESS					6			//!< 
#define	LOGIN_R_NO_IN_PROGRESS				7			//!< 
#define	LOGIN_R_VALID						8			//!< Login operation valid.
#define	LOGIN_R_INVALID						9			//!< Login operation invalid.
#define LOGIN_R_INVALID_PROTOCOLVERSION     10			//!< Login with invalid protocol version.
#define LOGIN_R_FREEZE						11			//!< Login account already be freeze.
#define	LOGIN_R_ACCOUNT_WAIT				12			//!< 
#define LOGIN_R_ACCOUNT_LOGINING			13			//!< 
#define	LOGIN_R_ACCOUNT_GUESTFULL			14			//!< 
#define LOGIN_R_ACCOUNT_FREEZE_BY_MOBILE	15			//!< Login account already be freeze by mobile.
#define	LOGIN_R_ACCOUNT_QUEUEERROR			16			//!< Queue Error
#define	LOGIN_R_NEED_ACTIVE					17			//!< Queue Error
#define	LOGIN_R_ACTIVEKEY_ERROR				18			//!< Queue Error

/*!
\brief
	Login head struct define.
*/
struct KLoginStructHead : public tagProtoHeader
{
	unsigned short	Size;								//!< Size of the struct, if the struct is been inherit, the size is ref the the derive struct.
	int				Param;								//!< Be one of the LOGIN_R_* define value combin with a LOGIN_A_* value.
};

/*!
\brief
	Login Account information struct define.
*/
struct KLoginAccountInfo : KLoginStructHead
{
	char	        Account[_NAME_LEN];					//!< Login account.
	KSG_PASSWORD    Password;							//!< Login password.
	unsigned long   nLeftTime;							//!< Login left time.
    unsigned		ProtocolVersion;					//!< Login client protocol version.
    unsigned long   nLeftTimeOfPoint;					//!< Login time of left point. 
	BYTE			ActiveKey[_ACTIVEKEY_LEN];
};

/*
 *	UDP Queue Package
 */

enum
{
	queue_proto_queue,
};

typedef struct _RegQueue
{
	unsigned char ucProtocol;
	char	      Account[_NAME_LEN];

}REGQUEUE,*PREGQUEUE;

typedef struct _RegQueue_Ret
{
	unsigned char ucProtocol;
	unsigned short uNumber;
}REGQUEUERET,*PREGQUEUERET;

///////////////////////////////////////////////////////////////////////////
// 任务相关

//同步Npc头顶图标状态
struct _SYNC_NPC_QUEST_STATE : BYTE_EXTEND_HEADER
{
	DWORD	dwNpcID;
	unsigned char ucState;
};

// 客户端请求Npc头顶图标状态
struct _QUERY_NPC_QUEST_STATE : BYTE_EXTEND_HEADER
{
	DWORD	dwNpcID;
};

//同步任务进展情况
struct _SYNC_QUEST_PROCESS : BYTE_EXTEND_HEADER 
{
	DWORD	dwQuestID;
	BYTE	btKind;
	BYTE	btObjective;
	DWORD	dwValue;
};
// 客户端请求Npc头顶图标状态
struct _QUERY_QUEST_DETAIL : BYTE_EXTEND_HEADER
{
	DWORD	dwQuestID;
};

// 客户端请求Npc头顶图标状态
struct _SYNC_QUEST_DETAIL : BYTE_EXTEND_HEADER
{
	BYTE	pContent[MAX_SCIRPTACTION_BUFFERNUM];
};

// 客户端请求任务名称
struct _QUERY_QUEST_TITLE : BYTE_EXTEND_HEADER
{
	DWORD	dwQuestID;
};

// 同步任务名称
struct _SYNC_QUEST_TITLE : BYTE_EXTEND_HEADER
{
	BYTE	pContent[MAX_SCIRPTACTION_BUFFERNUM];
};

// 
// 通知客户端移除一个任务
struct _REMOVE_QUEST : BYTE_EXTEND_HEADER
{
	DWORD	dwQuestID;
};

struct _SYNC_QUEST_STATE : BYTE_EXTEND_HEADER
{
	DWORD	dwQuestID;
    DWORD   dwState;
};

// 
// 客户端放弃一个任务
struct _ABANDON_QUEST : BYTE_EXTEND_HEADER
{
	DWORD	dwQuestID;
};

typedef struct _Player_LogOut
{
	BYTE	protocol;
	BYTE	operation;

} PLAYER_LOGOUT;

struct FindResult
{
	BYTE	ProtocolType;
	BYTE	FindType;
	char	szName[17];
	char	szZhuhou[17];
	char	szShizu[17];
	short	sLevel;
	short	sMetier;
	short	sSkillType;	
};

struct FindParam
{
	BYTE	ProtocolType;
	BYTE	FindType;
	char	szName[17];
};

/************************************************************************/
/*							Trade function	                            */
/************************************************************************/

// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdSell(int nID);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdBuy(int nBuyIdx, int buyCount);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdRun(int nX, int nY);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
// void SendClientCmdWalk(int nX, int nY);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdSkill(int nSkillID, int nX, int nY);
//void SendClientCmdPing();
void SendClientCmdSit(int nSitFlag);

void SendClientCmdQueryLadder(DWORD	dwLadderID);
void SendClientCmdRequestNpc(int nID);
void SendClientCmdStoreMoney(int nDir, int nMoney);
void SendClientCmdRevive(int nReviveType);
void SendObjMouseClick(int nObjID, DWORD dwRegionID);
void SendClientCmdRepair(DWORD dwID, bool special);

extern	int	g_nProtocolSize[MAX_PROTOCOL_NUM];


#pragma pack(pop)
#endif
