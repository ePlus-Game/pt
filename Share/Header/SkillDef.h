//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-8-21 10:56
//      File_base        : SkillDef
//      File_ext         : h
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#ifndef _SkillDef_h
#define _SkillDef_h

#define		MAX_SKILL					3000
#define		MAX_SKILL_LEVEL				50
#define		MAX_NPCSKILL				50
#define		MAX_SKILL_SERIES			6
#define		MAX_MAINSKILL_PER_SERIES	6
#define		MAX_MAIN_SKILL				MAX_SKILL_SERIES * MAX_MAINSKILL_PER_SERIES
#define		MAX_VALUE_SKILL				32
#define		MAX_SUBSKILL_PER_MAINSKILL	255
#define		MAX_BUFF_PER_SKILL			5
#define		INVALID_SKILL_ID			0
#define		MAXSIZE_SKILLNAME			32
#define		MAX_DT_PER_SKILL			4
#define		KNIGHT_NORMALSKILL_ID		31		//甲士普攻技能
#define		ENCHANTER_NORMALSKILL_ID	32		//道士普攻技能
#define		MONSTROUS_NORMAILSKILL_ID	33		//异人普攻技能
#define		RUN_SKILL_ID				34		//移动技能
#define		CUR_SKILL_VERSION			skill_data_version_2
#define		INVALID_SKILL_LEVEL			0
#define		ITEM_KEY_NUM				4
#define		TARGETFILTER_PARAM_NUM		2

#define		MAIN_SKILL_SERIES			0
#define		HIDDEN_SKILL_SERIES			-1

#define		SWITCH_SKILL_TYPE			1
#define		VALUE_SKILL_TYPE			2
#define		PASSIVELEVELUP_SKILL_TYPE	3

//技能数据存储结构版本
enum SkillDataVersions
{
	skill_data_version_1 = 1,
	skill_data_version_2,
};

enum RoleSkillSeries
{
	role_skillseries_invalid = -1,
	role_skillseries_0,
	role_skillseries_1,

	role_skillseries_count,
};

enum SkillStatus
{
	skill_status_begin,

	skill_status_inactive,
	skill_status_active,
	skill_status_usable,
	
	skill_status_end,
};

enum SkillType
{
	skill_type_hostile = -1,
	skill_type_neutral = 0,
	skill_type_friendly = 1
};

enum DamageTargetType
{
	dtt_life,
	dtt_mana,
};

enum DamageOutputType
{
	dot_none = -1,
	dot_farphysics,
	dot_nearphysics,
	dot_water,
	dot_fire,
	dot_thunder,
	dot_wind,
	dot_shadow,
	dot_poison,

	dot_almighty1,
	dot_almighty2,
	dot_almighty3,
	dot_almighty4,
	dot_almighty5,
	dot_almighty6,
	dot_almighty7,
	dot_almighty8,

	// Add new type here, not change previous members' order

	dot_end,
};

enum ActionType
{
	normal_action_type,
	multi_action_type,
};

enum AttackTargetType
{
	att_target_none = 0,
	att_target_only = 1,
	att_target_self = 2,
	att_target_enemyobj = 4,
	att_target_allyobj = 8,
	att_target_enemynpc = 16,
	att_target_allynpc = 32,
	att_target_enemyplayer = 64,
	att_target_allyplayer = 128,
	att_target_deathplayer = 256,

	att_target_ally = att_target_allyobj | att_target_allynpc | att_target_allyplayer,
	att_target_enemy = att_target_enemyobj | att_target_enemynpc | att_target_enemyplayer,
};

enum TargetFilterType
{
	tft_filter_begin,

	tft_filter_beeline,
	tft_filter_rectangle,

	tft_filter_end,
	tft_filter_num = tft_filter_end - tft_filter_begin - 1,
};

// Buff作用目标
enum BuffTarget
{
	buff_target_self = 0x01,		
	buff_target_target = 0x02,	// 有可能是自己	
};

// 技能可穿过的阻挡
// 用 3 个位来分别表示能否穿过对应的阻挡，1 穿过， 0 不能
enum SkillTravBarrier
{
	trav_barrier_none = 0x0,
	trav_barrier_normal = 0x1,
	trav_barrier_fly = 0x2,
	trav_barrier_jump = 0x4,
	trav_barrier_jumpfly = 0x8,

	trav_barrier_all = trav_barrier_normal | trav_barrier_fly | trav_barrier_jump | trav_barrier_jumpfly,
};

enum SkillSyncOperation
{
	skill_ope_invalid,

	skill_ope_levelup,
	skill_ope_chgstatus,
	skill_ope_chgcdtime,
	skill_ope_cooldown,
	skill_ope_addskill,
	skill_ope_removeskill,
	skill_ope_chgcost,
	skill_ope_clearcooldown,
	skill_ope_chgcastspeed,

	skill_ope_end,
};

typedef struct _TargetFilterInfo
{
	int	nFilterType;
	int	param[TARGETFILTER_PARAM_NUM];

} TargetFilterInfo;

typedef struct _DirSkillFilterParam
{
	int nDir;
	int param[TARGETFILTER_PARAM_NUM];

} DirSkillFilterParam;

typedef struct _tagSkillDamageInfo
{
	short int	nTargetType;
	short int	nNpcDamagePercent;
	int			nVal;

} SkillDamageInfo, *PSkillDamageInfo;

typedef struct _tagOutputDamageInfo
{
	int		nTargetType;
	int		nVal;

} OutputDamageInfo, *POutputDamageInfo;

#pragma pack(push, 1)

typedef struct _tagDBSkillData
{
	BYTE	status;					
	WORD	skillId;					
	WORD	skillLevel;
	DWORD	leftCoolDownTime;			

} DBSkillData, *PDBSkillData;

#pragma pack(pop)

typedef struct _tagDBSkillDataVersion1
{
	BYTE	status;					
	WORD	skillId;					
	WORD	skillLevel;
	DWORD	leftCoolDownTime;			

} DBSkillDataVersion1, *PDBSkillDataVersion1;

typedef struct _tagBuffAndProb
{
	int		nBuffId;
	int		nPercent;
	int		nBuffTarget;

} BuffAndProb, *PBuffAndProb;

typedef struct _tagSkillChgCond
{
		int		nPlayerLvl;
		int		nTaskId;
		int		nReqSkillId;
		int		nReqSkillLvl;
		int		nOwnMoney;
		int		nOwnSkillExp;
		int		nCostMoney;
		int		nCostSkillExp;
		int		nOwnItemKey[ITEM_KEY_NUM];
		int		nCostItemKey[ITEM_KEY_NUM];

} SkillChgCond;

// The following code copyed from FS1

#define MAX_MISSLESTYLE  200
#define MISSLE_MIN_COLLISION_ZHEIGHT 0	  //子弹落地碰撞的高度。
#define MISSLE_MAX_COLLISION_ZHEIGHT 20   //子弹高于该高度时,不计算碰撞	
#define FOLLOWMISSLE_GUIDETIME_PRETIME	 8 //跟踪子弹隔多少帧重新导航一下
#define MaxMissleDir	64

enum eMissleMoveKind
{
	MISSLE_MMK_Stand,							//	原地
	MISSLE_MMK_Line,							//	直线飞行
	MISSLE_MMK_Random,							//	随机飞行（暗黑二女巫的Charged Bolt）
	MISSLE_MMK_Circle,							//	环行飞行（围绕在身边，暗黑二刺客的集气）
	MISSLE_MMK_Helix,							//	阿基米德螺旋线（暗黑二游侠的Bless Hammer）
	MISSLE_MMK_Follow,							//	跟踪目标飞行
	MISSLE_MMK_Motion,							//	玩家动作类
	MISSLE_MMK_Parabola,						//	抛物线
	MISSLE_MMK_SingleLine,						//	必中的单一直线飞行魔法
	MISSLE_MMK_RollBack = 100,					//  子单来回飞行
	MISSLE_MMK_Toss		,						//	左右震荡
};

//---------------------------------------------------------------------------
// FollowKind 跟随类型	(主要是针对原地、环行与螺旋线飞行有意义)
//---------------------------------------------------------------------------
enum eMissleFollowKind
{
	MISSLE_MFK_None,							//	不跟随任何物件
	MISSLE_MFK_NPC,								//	跟随NPC或玩家
	MISSLE_MFK_Missle,							//	跟随子弹
};

enum eMissleStatus
{
	MS_DoWait,
	MS_DoFly,
	MS_DoVanish,
	MS_DoCollision,

	MAX_MISSLE_STATUS,
};

enum eMisslesForm
{
	SKILL_MF_NONE,					//表示没有子弹
	SKILL_MF_Wall,					//墙形	多个子弹呈垂直方向排列，类式火墙状
	SKILL_MF_Line,					//线形	多个子弹呈平行于玩家方向排列
	SKILL_MF_Spread,				//散形	多个子弹呈一定的角度的发散状	
	SKILL_MF_Circle,				//圆形	多个子弹围成一个圈
	SKILL_MF_Random,				//随机	多个子弹随机排放
	SKILL_MF_Zone,					//区域	多个子弹放至在某个范围内
	SKILL_MF_AtTarget,				//定点	多个子弹根据
	SKILL_MF_AtFirer,				//本身	多个子弹停在玩家当前位置

	SKILL_MF_COUNT,
};

enum eRelativePosType
{
	//0时就属于一般子碟
	type_bornpos = 1,	//子碟的出生点，一般用于速度超出32的
	type_launcherpos,	//以发送者Npc的当前位置为参靠点
	type_targetpos,		//以目标者的当前位置为参靠点
	type_parentpos,		//以母亲（母亲子碟）的当前位置为参靠点
};

enum eMisslesGenerateStyle
{
	SKILL_MGS_NULL		= 0,
	SKILL_MGS_SAMETIME	,    //同时
	SKILL_MGS_ORDER		,	 //按顺序
	SKILL_MGS_RANDONORDER,
	SKILL_MGS_RANDONSAME,
	SKILL_MGS_CENTEREXTENDLINE,  //由中间向两周扩散
};

enum eSkillLauncherType
{
	SKILL_SLT_Npc = 0,
	SKILL_SLT_Obj ,
	SKILL_SLT_Missle,
};

enum eSkillParamType
{
	SKILL_SPT_TargetIndex	= -1,
	SKILL_SPT_Direction		= -2,
};

enum eSkillLRInfo
{
	BothSkill,          //左右键皆可
	leftOnlySkill,		//左键
	RightOnlySkill,		//右键
	NoneSkill,			//都不可
};

// 技能的类型，默认都是 Active
// 目前代码里面需要用来判断的有: 
// PassivityNpcState
// Melee
// NultiAttack
// NearTrap
// FarTrap
enum eSKillStyle
{
	SKILL_SS_Active,				
	SKILL_SS_PassivityNpcState,		  
	SKILL_SS_Summon,

	SKILL_SS_Melee,

	SKILL_SS_NearMultiAttack,
	SKILL_SS_AddNearTrap,
	SKILL_SS_AddFarTrap,
	SKILL_SS_RmTrap,
	SKILL_SS_FarMultiAttack,
	SKILL_SS_Rush,						// 冲锋到鼠标位置
	SKILL_SS_DIR,						// 鼠标位置和人的位置确定技能攻击方向
	SKILL_SS_TargetDir,					// 以自己和目标连线为方向，需要选中目标
	SKILL_SS_AddFixRangeTrap,			// 以自己和鼠标位置连线为方向，距离自己固定距离处放陷阱
	SKILL_SS_Rectangle,					// 矩形
	
	SKILL_SS_DEFAULT,
};

enum eMeleeForm
{
	Melee_AttackWithBlur = SKILL_MF_COUNT,
	Melee_RunAndAttack,
	Melee_ManyAttack,
};

typedef struct 
{
	int nRegion;
	int nMapX;
	int nMapY;
}
TMisslePos;

typedef struct 
{
	int nLauncher;	
	DWORD dwLauncherID;			
	eSkillLauncherType eLauncherType; //发送者，目前肯定是Npc


	int nParent;
	eSkillLauncherType eParentType;	  //母，可以是Npc也可以是Missle	 
	DWORD dwParentID;

	int nParam1;
	int nParam2;
	int nWaitTime;
	int nTargetId;
	DWORD dwTargetNpcID;
}
TOrdinSkillParam, * LPOrdinSkillParam;

struct KSkillInfo 
{
	char	szName[MAXSIZE_SKILLNAME];
	char	szIconName[MAXSIZE_SKILLNAME];
	int		nLevel;
	eSkillLRInfo eSkillLR;
	int		nCoolingTime;
	DWORD	nSkillPoint;
	DWORD	nSkillMaxPoint;
	bool	bActive;
	bool	bCanStudy;
	bool	bPassivity;
	bool	bCDComplete;
	bool	bIsCommonCoolingDown;
	bool	bIsSelected;
	bool    bCanHumanUse;
//	char	szDemand[COMMON_CLIENT_MSG_LEN_256];
	char	szDesc[COMMON_CLIENT_MSG_LEN_1024];
	char	szDescNext[COMMON_CLIENT_MSG_LEN_1024];
//	char	szTooltip[COMMON_CLIENT_MSG_LEN_256];
	int		nCostSkillExp;

	// 如果返回NULL，就表示技能已达到最大等级
	// 否则，对里面的各项，如果为-1，表示这个条件不用考虑
	// 否则需要考虑，注意装备id 4个全部为-1才表示不考虑
	const SkillChgCond	*pSkillCond;
    //This is used for skill tip mix
	char *  szStudyTipBuff;
    int     nStudyTipBuffSize; 
	//Additional param When getgamedata for the skillinfo
	bool           bDescAvailable;        //表示szDesc有效
	bool           bNextDescAvailable;    //表示下一级的szNextDesc有效
	unsigned long  dwDescStyle;           //有效时候对应的风格
	unsigned long  dwNextDescStyle;

    KSkillInfo():szStudyTipBuff(NULL),nStudyTipBuffSize(0),bDescAvailable(false),
		bNextDescAvailable(false),dwDescStyle(0),dwNextDescStyle(0){}
};

#ifndef _SERVER

#define MISSLE_FILE_NAME_LENGTH 80

struct TMissleRes
{
	char	AnimFileName[MISSLE_FILE_NAME_LENGTH];						// 图像spr 文件名
	int		nTotalFrame;												// 总帧数
	int		nInterval;													// Spr帧间隔
	int		nDir;														// Spr方向
	char	SndFileName[MISSLE_FILE_NAME_LENGTH];						// 声音wav 文件名
};

struct KMissleTemplate 
{
	KMissleTemplate()
	{
		m_bFollowNpcWhenCollid = 1;
		m_ulDamageInterval = 0;
	}
	//	子弹设定文件获得的数据	
	int					m_nMissleId;
	char				m_szMissleName[30];								//	子弹的名称
	eMissleMoveKind		m_eMoveKind;									//	子弹运动类型(爆炸、直线飞行等……)
	eMissleFollowKind	m_eFollowKind;									//	子碟发出时的参照类型
	int					m_nHeight;										//	子弹高度
	//Lucifer~yu(zhangjianyu) 10/22/2007 Modify 暂时无用
	//Begin-------------------------------------------------------------------
	int					m_nLifeTime;									//	生命周期
	int					m_nSpeed;										//	飞行速度		
	//End---------------------------------------------------------------------
	int					m_nSkillId;										//	对应哪个技能
	int					m_nCollideRange;								//	碰撞范围（简化多边形碰撞用）
	BOOL				m_bCollideVanish;								//	碰撞后是否消亡
	BOOL				m_bCollideFriend;								//	是否会碰撞到同伴
	BOOL				m_bCanSlow;										//	是否会被减速（比如说Slow Missle类的技能）
	BOOL				m_bRangeDamage;									//	是否为区域伤害，即是否多人受到伤害
	int					m_nDamageRange;									//	伤害范围
	int					m_nZAcceleration;								//	Z轴的加速度
	int					m_nHeightSpeed;									//	子弹纵行的飞行速度
	int					m_nParam1;										//	参数一
	int					m_nParam2;										//	参数二
	int					m_nParam3;										//	参数三
	BOOL				m_bAutoExplode;									//  Is Missle Would AutoExpode ItSelf For Collision When It's LiftTime Is Over;
	unsigned long		m_ulDamageInterval;								//	伤害计算的间隔时间,主要指对类型火墙技能
	BOOL				m_bMultiShow;									//	子弹有两个显示
	BOOL				m_bFollowNpcWhenCollid;							//	爆炸效果跟随被击中的人物
	int					m_nAcceleration;								//  子弹加速度
	TMissleRes			m_MissleRes[MAX_MISSLE_STATUS * 2];				// 几种状态下的资源情况	
	BOOL				m_bLoopAnim;									// 是否循环播放动画
	BOOL				m_bSubLoop;
	int					m_nSubStart;									// 子循环的起始帧
	int					m_nSubStop;										// 子循环的结束帧
};

#endif


#endif // _SkillDef_h