#ifndef _ITEM_COMMON_DEF_H_
#define _ITEM_COMMON_DEF_H_

#include "GameDataDef.h"

#define ITEM_VERSION			(5)
#define ITEM_EXTEND				2

#define IN //传入参数
#define OUT //传出参数

#define TREASUREITEMCLASS 4
#define TREASUREDETAIL 46
#define TREASUREPARTI 1
#define	TREASURELEVEL 1
#define TREASUREPILECOUNT 1
#define COPPERCASHITEMCLASS 4
#define	COPPERCASHDETAIL 47
#define	COPPERCASHPARTI 1
#define	COPPERCASHLEVEL	1
#define	COPPERCASHPILECOUNT 100
#define SZBUFLEN_0 128 //典型的字符串缓冲区长度
#define SZBUFLEN_1 256 //典型的字符串缓冲区长度
#define YAO_PER_GUA 3 //组成一个卦的爻装的个数
#define MAX_EQUIPMENT_ID_ARRAY_LENGTH 4 //最大装备ID数组的长度
#define MAX_YAO_LEVEL 3//爻最高等级
#define YAO_ADDON_BUFF_COUNT 3//爻附加属性数量
#define ITEM_PLUS_INFO_LEN 16
#define MAX_INLAY_COUNT 6			//最大镶嵌数量
#define MAX_SPECIALEFFECT_COUNT 3	//符文之语附加buff上限
#define MAX_ITEM_INLAY_YAO_EFFECT_COUNT MAX_INLAY_COUNT
#define MAX_SPECIALEFFECT_FILTER 3	//符文之语附加buff上限
#define MAX_SPECIALEFFECT_NAME_LEN 128	//符文之语名字字节上限

#define ITEM_TRANSFER_DATA_RESERVE_SPACE 99	// 物品传输结构预留扩展位

//装备详细类别
enum EQUIPDETAILTYPE
{
	equip_weapon = 0,	//武器
	equip_rangeweapon,	//准备去掉
	equip_armor,		//衣服
	equip_ring,			//戒指
	equip_amulet,		//玉佩
	equip_boots,		//靴子
	equip_shoulder,		//护肩
	equip_helm,			//头盔
	equip_cuff,			//护腕
	equip_pendant,		//坠饰（披风/令牌/绳结）
	equip_talisman,		//法宝
	equip_detailnum,
};

//装备爻属性的类型
enum ITEM_YAO_TYPE
{
	yao_yin = 0,			//阴爻
	yao_yang,				//阳爻
	yao_count,
	yao_invalid = 10		//无爻属性
};

//药品详细类型
enum MEDICINEDETAILTYPE
{
	medicine_blood = 0,
	medicine_mana,
	medicine_both,
	medicine_stamina,
	medicine_antipoison,
	medicine_detailnum,
};

enum MAGIC_SCRIPTDETAILTYPE
{
	magicscript_magic,
	magicscript_script,
	magicscript_detailnum,
};

enum enWEAPONATTRTYPE
{
	weaponattr_shortweapon = 0, 
	weaponattr_longweapon, 
	weaponattr_lightingsword, 
	weaponattr_earthsword, 
	weaponattr_coldsword, 
	weaponattr_firesword, 
	weaponattr_producetool, 
	weaponattr_buildtool,
	weaponattr_none, 
	weaponattr_other, 
};

enum ennWEAPONPOSITIONTPE
{
	weaponposreq_none = 0, 
	weaponposreq_tongmaster, 
};

enum  _series_req_filter
{
	xuanfeng_j	= 0x00000001,
	xingtian_j	= 0x00000002,
	tianshi_d	= 0x00000004,
	zhenren_d	= 0x00000008,
	yishi_s		= 0x00000010,
	shoushi_s	= 0x00000020,
};

//物品品质标签
enum ITEM_QUALITY_LABEL
{
//	quality_poor,			//差劲
	quality_common = 0,		//普通-白
//	quality_uncommon,		//优秀
	quality_rare,			//精良-蓝
	quality_set,			//套装-绿
	quality_epic,			//史诗-黄金
	quality_artificial_1,	//人造的1
	quality_artificial_2,	//人造的2
	quality_artificial_3,	//人造的3
	quality_count			//物品品质标签数量
};

//装备等级（似乎需要用装备品质标签替代）
// enum EQUIPLEVEL
// {
// 	equip_normal = 0,		// 普通装备
// 	equip_magic,			// 魔法装备（1 ~ 2个魔法前后缀）
// 	equip_rare,				// 稀有装备（3 ~ 6个魔法前后缀）
// 	equip_unique,			// 唯一装备
// 	equip_set,				// 套装
// 	equip_number,			// 装备等级数目
// 	equip_gold,				// 黄金装备
// };

//装备磨损模式
enum AbradeMode
{
	abrade_attack = 0,
	abrade_defend,
	abrade_death,
	abrade_count
};

//物品模板编号
struct ItemTemplateId
{
	ItemTemplateId()
	{
		memset(IDArray, 0, sizeof(IDArray));
	}

	ItemTemplateId& operator= (const ItemTemplateId &rhs)
	{
		if(this != &rhs)
		{
			memcpy(IDArray, rhs.IDArray, sizeof(IDArray));
		}

		return *this;
	}

	bool operator== (const ItemTemplateId &rhs) const
	{
		return (memcmp(IDArray, rhs.IDArray, sizeof(IDArray)) == 0) ? true : false;
	}

	bool operator!= (const ItemTemplateId &rhs) const
	{
		return !(*this == rhs);
	}

	int IDArray[MAX_EQUIPMENT_ID_ARRAY_LENGTH];	
};

//装备编号
typedef struct tagEquipmentID
{
	int IDArray[MAX_EQUIPMENT_ID_ARRAY_LENGTH];
} EquipmentID;

//卦的状态
typedef struct tagGuaState
{
	int					GuaID;						//卦编号（-1：未激活；>0：正常的卦）
	int					Level;						//卦等级
	int					EquipmentID[YAO_PER_GUA];	//激活这个卦的装备序号
} GuaState;

//多卦状态
typedef struct tagMutiGuaState
{
	int					GuaID;						//卦编号
	int					Level;						//卦等级
	int					Count;						//同种卦的数量
} MutiGuaState;

//套装激活状态
typedef struct tagArmorSetState
{
	int					ArmorSetID;					//套装编号
	int					ArmorSetCount;				//套装激活数量
	bool				Exist[itempart_num];		//具体激活的是哪几件，true表示激活了
} ArmorSetState;

//装备状态
struct KEquipState
{
	int					nEquipIdx;
	bool				IsMasked;
};

//装备基本BUFF
typedef struct tagItemBasicBuff
{
	int BuffID[ITEM_BUFF_COUNT];
} ItemBasicBuff, *PItemBasicBuff;

//物品音效
enum enumItemSoundEffect
{
	item_sound_effect_pickup = 0,	//拿起
	item_sound_effect_putdown,		//放下
	item_sound_effect_use,			//使用

	item_sound_effect_count			//音效总计数量
};

#define INLAYDESC 38
//物品死亡掉落类型
enum enumItemDeathDropType
{
	item_death_drop_no = 0,			//不会掉落
	item_death_drop_normal,			//普通掉落方式，按照概率随机
	item_death_drop_always,			//必然掉落
};

typedef unsigned long ITEM_IB_BUY_TYPE;   //ITEM 的IB买卖方式
#define IIBT_INVALID                 0x00000000    //无法通过IB方式购买
#define IIBT_JIN_SHAN_BI             0x00000001    //可通过金山币购买
#define IIBT_CREDIT_POINT            0x00000002    //可通过信用点数购买
#define IIBT_TICKET                  0x00000004    //可通过代金券购买
#define IIBT_REWARD_POINT            0x00000008    //可通过积分购买


//物品基本属性
typedef struct tagKBASICPROP_ITEM
{
	char				szName[SZBUFLEN_0];						// 名称
	ITEMGENRE			nItemGenre;								// 道具种类 (武器? 药品? 矿石?)
	EQUIPDETAILTYPE		nDetailType;							// 具体类别
	int					nParticularType;						// 详细类别
	int					nLevel;									// 最小类别
	int					nObjIdx;								// 对应物件索引
	char				szIntro[SZBUFLEN_1];					// 说明文字
	char				szImageSetName[SZBUFLEN_0];				// 界面中的ImageSet文件名
	char				szImageName[SZBUFLEN_0];				// ImageSet中的Image名
	ITEM_QUALITY_LABEL	nColor;									// 品质
	int					nQuality;								// 档次
	int					nYao;									// 爻装
	int					nYaoRate;								// 爻附加概率
	short				sYaoGroup;								// 爻随机属性组ID
	int					nPrice;									// 价格	(谢鉷2007年2月6日改为ulong)
	int					nSellPrice;								// 卖店价格(谢鉷2007年2月6日改为ulong)
	int					nItemWeight;							// 物品重量用于负重计算
	int					nStack;									// =0时不能叠放,大于0时为叠放上限
	int					nDurability;							// 耐久
	int					nEquiSetID;								// 套装ID
	char				scriptFile[COMMON_CLIENT_MSG_LEN_256];	// 调用的脚本文件名
	char				szBuffTarget[COMMON_CLIENT_MSG_LEN_256];				// Buff目标
	ItemBasicBuff		BasicBuff;								// 装备基本Buff
	int					nSkillID;								// 技能ID
	int					nSkillTarget;							// 技能目标,0自己,1其他目标
	int					nGroup;									// 装备组
	int					nCDTime;								// 冷却时间
	int					ActionTime;								// 动作时间
	int					nLogCode;								// Log类型
	int					nItemRes;								// 换装部件
	int					nReqLevel;								// 等级需求
	char				szReqPro[COMMON_CLIENT_MSG_LEN_16];		// 职业需求
	int					nReqSeries;								// 系需求
	int					nReqProperty[ITEM_PROP_REQ_COUNT];		// 属性需求
	int					btRepair;								// 是否修理	
	int					btCanDiscard;							// 是否可以丢弃（销毁）
	int					btBuffSwitch;							// buff标志位
	int					btCanExchage;							// 是否可以交易（玩家之间交易和拍卖行出售）	
	int					btCanYao;								// 是否可以拆爻
	int					btCanUpdate;							// 是否可升级
	DWORD				Potential;								// 潜力（法宝蕴魂上限）	
	int					TalismanBuff;							// 法宝BUFF
	int					CanSell;								// 是否可以出售（卖给系统商人）
	int					DeathDropType;							// 死亡掉落类型
	int					RestrictCount;							// 限制拥有个数
	int					CostSkillExp;							// 消耗蕴魂
	char				SoundEffects[item_sound_effect_count][COMMON_CLIENT_MSG_LEN_64];	// 音效
	int					DropNpcTemplateID;													// 掉落npc template id
	int					DisplayID;															// 显示ID
	char				InlayDesc[INLAYDESC];
	int					EquipBind;								// 装备绑定
	int					IBType;
	DWORD				IBLiveTime;
	int					IBUseCount;
	int					PickupBind;
	char                IBBuyType[COMMON_CLIENT_MSG_LEN_32];
	int					UseType;
	char				szBigImageSetName[SZBUFLEN_0];			// 界面中的ImageSet文件名（大图标）
	char				szBigImageName[SZBUFLEN_0];				// ImageSet中的Image名（大图标）
	char				szMapAreaTarget[COMMON_CLIENT_MSG_LEN_256];				// Buff目标	
	int                 nMaxFlushTimes;                         //国战装备分期付款 (有效值必须>=0 默认值为-1. -2表示是充值类分期付款石)
	int					IsExpItem;
} KBASICPROP_ITEM;

#define MAX_IMMEDIACY_ITEM 20
#define MAX_TEMP_ITEM 32

#pragma pack(push, 1)

struct _TDBItemIndex 
{
	short	nGenre;											//!< 道具Genre类型
	short	nDetail;												//!< 道具Detail类型
	short	nParticular;											//!< 道具Particulare类型
	short	nLevel;														//!< 道具类型
};

#pragma pack(pop)

#pragma pack(push, 1)

struct TItemtransfersDataBase
{
	FSGUID	Guid;														//!< 道具GUID
	int		igenre;														//!< 道具Genre类型
	int		idetailtype;												//!< 道具Detail类型
	int		iparticulartype;											//!< 道具Particulare类型
	int		ilevel;														//!< 道具类型	
	int		iyaoid;														//!< 爻属性编号
	int		iyaoAddOnBuffIDSet[YAO_ADDON_BUFF_COUNT];					//!< 爻装附加属性buff
	int		idurability;												//!< 当前道具耐久
	int		imaxdurability;												//!< 当前耐久度上限
	WORD	icompBuffTemplateSet[COMPOUND_COUNT];						//!< 道具合成后添加的buff数组
	UINT	uLevelupTimes;												//!< 装备升级次数
	int		nLevelupType;												//!< 升级类型
	int		nItemCount;													//!< 叠加数
	char	szPlusInfo[COMMON_CLIENT_MSG_LEN_64];						//!< 附加描述
	int		nTalismanPotential;											//!< 法宝当前蕴魂
	int		TalismanEnchaseSet[TM_HOLE_NUM];							//!< 法宝镶嵌内丹集合
	bool	IsBind;														//!< 是否已绑定
	int		LockCount;													//!< 锁定计数
	_TDBItemIndex inlayItem[MAX_INLAY_COUNT];
	WORD	InlayBaseBuffSet[MAX_INLAY_COUNT][ITEM_BUFF_COUNT];			//灵石附加buff
	WORD	InlayYaoBuffSet[MAX_ITEM_INLAY_YAO_EFFECT_COUNT];			//灵石爻附加buff
	WORD	InlaySpecialBuffSet[MAX_SPECIALEFFECT_COUNT];				//灵石组合附加buff
	
	DWORD	IBBuyDate;													//IB购买日期
	DWORD	CreditFlag;													//信贷标识

	int		MapID;
	int		MapX;
	int		MapY;
	BYTE	Step;

	INT64	IBGUID;		

};

struct TItemtransfersData : public TItemtransfersDataBase
{
	// 预留扩展位（128字节）- IB购买日期（4字节）- 信贷标识（4字节）
	// - MapID（4字节）- MapX（4字节） - MapY（4字节） - Step（1字节）
	//IBGUID （8字节）;		
	char	freeSpace[ITEM_TRANSFER_DATA_RESERVE_SPACE];
};

#pragma pack(pop)

struct TItemtransfersDataBaseVersion1
{
	FSGUID	Guid;														//!< 道具GUID
	int		igenre;														//!< 道具Genre类型
	int		idetailtype;												//!< 道具Detail类型
	int		iparticulartype;											//!< 道具Particulare类型
	int		ilevel;														//!< 道具类型	
	int		iyaoid;														//!< 爻属性编号
	int		iyaoAddOnBuffIDSet[YAO_ADDON_BUFF_COUNT];					//!< 爻装附加属性buff
	int		idurability;												//!< 当前道具耐久
	int		imaxdurability;												//!< 当前耐久度上限
	WORD	icompBuffTemplateSet[COMPOUND_COUNT];						//!< 道具合成后添加的buff数组
	UINT	uLevelupTimes;												//!< 装备升级次数
	int		nLevelupType;												//!< 升级类型
	int		nItemCount;													//!< 叠加数
	char	szPlusInfo[COMMON_CLIENT_MSG_LEN_64];						//!< 附加描述
	int		nTalismanPotential;											//!< 法宝当前蕴魂
	int		TalismanEnchaseSet[TM_HOLE_NUM];							//!< 法宝镶嵌内丹集合
	bool	IsBind;														//!< 是否已绑定
	int		LockCount;													//!< 锁定计数
	_TDBItemIndex inlayItem[MAX_INLAY_COUNT];
	WORD	InlayBaseBuffSet[MAX_INLAY_COUNT][ITEM_BUFF_COUNT];			//灵石附加buff
	WORD	InlayYaoBuffSet[MAX_ITEM_INLAY_YAO_EFFECT_COUNT];			//灵石爻附加buff
	WORD	InlaySpecialBuffSet[MAX_SPECIALEFFECT_COUNT];				//灵石组合附加buff
};

struct TItemtransfersDataVersion1 : public TItemtransfersDataBaseVersion1
{
	char	freeSpace[128];
};

#pragma pack(push, 1)

//道具存盘数据基类
struct _TDBItemData_Base
{
	//BaseInfo
	FSGUID			guid;														//!< 物品唯一标识ID（GUID）	
	short			iequipclasscode;											//!< 道具Genre类型
	short			idetailtype;												//!< 道具Detail类型
	short			iparticulartype;											//!< 道具Particulare类型
	short			ilevel;														//!< 道具类型
	BYTE			ilocal;														//!< 道具容器类型
	BYTE			ix;															//!< 道具容器横坐标
	BYTE			iy;															//!< 道具容器纵坐标
	BYTE			iyaoid;														//!< 爻属性编号
	WORD			iyaoAddOnBuffIDSet[YAO_ADDON_BUFF_COUNT];					//!< 爻装附加属性buff编号
	int				idurability;												//!< 当前道具耐久
	int				imaxdurability;												//!< 当前耐久度上限
	WORD			icompBuffTemplateSet[COMPOUND_COUNT];						//!< 道具合成后添加的buff数组
	BYTE			uLevelupTimes;												//!< 装备升级次数
	short			nLevelupType;
	int				TalismanPotential;											//!< 法宝蕴魂
	short			TalismanEnchaseSet[TM_HOLE_NUM];							//!< 法宝镶嵌内丹集合
	WORD			wItemCount;
	BYTE			IsBind;
	_TDBItemIndex	inlayItem[MAX_INLAY_COUNT]; 
	short			inlayYaoBuffSet[MAX_ITEM_INLAY_YAO_EFFECT_COUNT];
	char			szPlusInfo[ITEM_PLUS_INFO_LEN];
};

struct TDBItemData_Base
{
	int						nVersion;
	int						nItemlistLength;											//!< 装备列表的总长度
	KImmediacyParam			ImmData[MAX_IMMEDIACY_ITEM];
};

//道具版本1 存盘数据结构
struct _TDBItemData_Version_1 : public _TDBItemData_Base
{
	INT64			IBGuid;														//!< IB道具GUID
	DWORD			IBProductionData;											//!< 生产日期
	DWORD			IBCreaditFlag;	       										//!< 保质期
};

struct TDBItemData_Version_1  : public TDBItemData_Base
{
	_TDBItemData_Version_1	ItemData[1];
};

//道具版本2 存盘数据结构

struct ItemMapInfo 
{
	int		m_MapID;
	int		m_MapX;
	int		m_MapY;
	BYTE	m_Step;
};

struct _TDBItemData_Version_2 : public _TDBItemData_Version_1
{
	union{
		ItemMapInfo mapInfo;
	};
};

struct TDBItemData_Version_2  : public TDBItemData_Base
{
	_TDBItemData_Version_2	ItemData[1];
};

//道具版本3 存盘数据结构
struct _TDBItemData_Version_3 : public _TDBItemData_Version_2
{
	BYTE            IsTaskGiven;
};

struct TDBItemData_Version_3  : public TDBItemData_Base
{
	_TDBItemData_Version_3	ItemData[1];
};

struct _TDBItemData_Version_4 : public _TDBItemData_Version_3
{
	DWORD            dwLockLeftTime;
};

struct TDBItemData_Version_4  : public TDBItemData_Base
{
	_TDBItemData_Version_4	ItemData[1];
};

struct _TDBItemData_Version_5 : public _TDBItemData_Version_4
{
	BYTE            nFlushTimes;
};

struct TDBItemData_Version_5  : public TDBItemData_Base
{
	_TDBItemData_Version_5	ItemData[1];
};


#pragma pack(pop)

enum _immediacy_type
{
	skill_immediacy_type = 1,
	item_immediacy_type,
	skill_common_coolingdown,
};

struct KItemGroupCD_C 
{
	DWORD			dwStartCount;
	int				nGroup;
	int				Id;
	_immediacy_type	eType;
	unsigned long	ulCDTime;
};

//法宝相关子协议
enum enumTalismanProtocol
{
	talisman_protocol_upgrade,	//升级
	talisman_protocol_enchase,	//镶嵌
	talisman_protocol_convert_skill_exp,	//转化技能经验为法宝蕴魂
};

//物品同步类型
enum enumItemSyncType
{
	item_sync_type_init,		//初始化
	item_sync_type_normal,		//一般同步
	item_sync_type_gain,		//其他原因的获得
	item_sync_type_pickup,		//拾取
	item_sync_type_trade,		//交易
	item_sync_type_buy,			//购买
    item_sync_type_split,       //拆分物品
	
	item_sync_type_count
};

enum enumIBItemType
{
	ib_item_now,
	ib_item_timelimit,
	ib_item_time_count_limit,
};

typedef struct
{
	int					nPlace;
	int					nX;
	int					nY;
	unsigned int		uGener;
	unsigned int		uId;
} ItemPos;

int GenerateItemHashId( int nGenre, int nDetail, int nParticular );
void SpliteHashId( int nHashId, int &nGenre, int &nDetail, int &nParticular );

#endif//_ITEM_COMMON_DEF_H_