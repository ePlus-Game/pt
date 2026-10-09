//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/07/2006 11:28
//      File_base        : KNpc
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 此文件为原有封神老代码整理而来
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KNpcH
#define KNpcH

#include "KCore.h"
#include "BaseValue.h"
#include "KIndexNode.h"
#include "GameDataDef.h"
#include "NpcSkillList.h"
#include "SkillManager.h"
#include "KNpcFindPath.h"
#include "ValueAttribute.h"
#ifdef _SERVER
	#include "ai_controller.h"
#else
	#include "KNpcRes.h"
	#include <list>
	#include <map>
	#include <string>
	#include "client_talisman_npc.h"
	#include "client_combat_info.h"
#endif

#define		NPCBEGININDEX		MAX_PLAYER

#define		MAX_NPCSTYLE		2000
#define		MAX_AI_PARAM		11
#define		MAX_NPC_USE_SKILL	4

#define		STATE_FREEZE		0x01
#define		STATE_POISON		0x02
#define		STATE_CONFUSE		0x04
#define		STATE_STUN			0x08
#define		STATE_PRODUCE		0x10
#define		STATE_INSIDE		0x20		// 是否驻扎状态
#define		STATE_BARRIER		0x40
#define		STATE_REDNAME		0x80		// 是否红名状态
#define		SELECT_NPC_SOUND	"\\sound\\selectnpc.wav"
#define		HOVER_NPC_SOUND		"\\sound\\hovernpc.wav"

#define		MAX_ENHANCE_STATE	8

#ifdef	_SERVER
	#define	MAX_NPC					45000
	#define MAX_NPC_CUSTOM_VARIABLE_COUNT 5
#else
	#define	MAX_NPC					512
	#define MAX_BUF_COUNT			16
#endif

#ifndef _SERVER 
    #define HEAD_INFO_GENS         "Gens"
    #define HEAD_INFO_TONG         "Tong"
    #define HEAD_INFO_SPECIAL_BUFF "SpecialEvent"
#endif

enum 
{
	npc_deathmode_none,
	npc_deathmode_nopunish	= 0x1,				//不惩罚模式
	npc_deathmode_nodrop	= 0x2,				//不掉落模式
	npc_deathmode_autodel	= 0x4,
	npc_deathmode_end

};

enum NPCATTRIB
{
	attrib_none = -1,
	attrib_mana,
	attrib_stamina,
	attrib_life,
	attrib_maxmana,
	attrib_maxstamina,
	attrib_maxlife,
};

enum enSETEFFICACYTYPE
{
	enSetEfficacy_Default = 0, 
	enSetEfficacy_1, 
	enSetEfficacy_2, 
	enSetEfficacy_3, 
};

enum CLIENTACTION
{
	cdo_stand = 0,
	cdo_stand1 = 0,//*/
	cdo_fightstand = 1,
	cdo_sit = 1,
	cdo_walk = 2,
	cdo_run = 2,
	cdo_fightwalk = 2,
	cdo_fightrun = 2,
	cdo_attack = 3,
	cdo_attack1 = 4,
	cdo_magic = 5,
	cdo_hurt = 6,	
	cdo_death = 7,
	cdo_jump,	
	cdo_none,   
	cdo_count,
};

enum enNpc_CompoundAttr_Idx
{
	ncai_lifeuplimit,				// Npc生命上限
	ncai_manauplimit,				// 气值上限
	ncai_liferenewspeed,			// 生命恢复速度
	ncai_manarenewspeed,			// 气值恢复速度
	ncai_vision,					// 眼力，(即命中率)
	ncai_dexterity,					// 机敏，(即闪避率)
	ncai_walkspeed,					// 走路速度
	ncai_runspeed,					// 跑步速度
	ncai_attackspeed,				// 攻击速度
	ncai_castspeed,					// 施法速度
	ncai_visionradius,				// Npc的视野范围
	ncai_attackradius,				// Npc的攻击范围
	ncai_activeradius,				// Npc的活动范围	
	ncai_body,						// 人物的体属性，对应原来的 Constitution
	ncai_nimbus,					// 人物的灵属性，对应原来的 Dexterity
	ncai_strength,					// 人物的力属性，对应原来的 Strength
	ncai_art,						// 人物的术属性，对应原来的 Intellect
	ncai_hitrecover,				// 受击回复速度
	ncai_physexplode,				// 物理爆击概率
	ncai_magicexplode,				// 法术爆击概率
	// Add new attribute here, don't change previous' order
	ncai_end,
};

enum enNpc_RangeAttr_Idx
{
	nrai_damage_farphysics,			// 远程物理伤害
	nrai_damage_nearphysics,		// 进程物理伤害
	nrai_damage_water,				// 水伤害 
	nrai_damage_fire,				// 火伤害
	nrai_damage_thunder,			// 雷伤害
	nrai_damage_wind,				// 风伤害
	nrai_damage_shadow,				// 阴伤害
	nrai_damage_poison,				// 毒伤害
	nrai_defend_farphysics,			// 远程物理抗性
	nrai_defend_nearphysics,		// 近程物理抗性
	nrai_defend_water,				// 水抗
	nrai_defend_fire,				// 火抗
	nrai_defend_thunder,			// 雷抗
	nrai_defend_wind,				// 风抗
	nrai_defend_shadow,				// 阴抗
	nrai_defend_poison,				// 毒抗
	nrai_damage_physics,			// 物理攻击加强，人物本身的，上面的8种是魔法属性加上去的
	nrai_damage_magic,				// 法术攻击加强，人物本身的，上面的8种是魔法属性加上的
	nrai_damage_almighty1,			// 全能属性，具体含义由策划决定
	nrai_damage_almighty2,
	nrai_damage_almighty3,
	nrai_damage_almighty4,
	nrai_damage_almighty5,
	nrai_damage_almighty6,
	nrai_damage_almighty7,
	nrai_damage_almighty8,
	nrai_defend_almighty1,			// 全能抗性，具体含义由策划决定
	nrai_defend_almighty2,
	nrai_defend_almighty3,
	nrai_defend_almighty4,
	nrai_defend_almighty5,
	nrai_defend_almighty6,
	nrai_defend_almighty7,
	nrai_defend_almighty8,
	// 这三种抗性是人物本身的，和上面的8种区分开
	nrai_defend_physics,			// 物理抗性
	nrai_defend_eightdiag,			// 八卦抗性
	nrai_defend_dark,				// 玄冥抗性
	// Add new attribute here, don't change previous' order
	nrai_end,

};

enum enNpc_UnaryAttr_Idx
{
	nuai_curlife,					// 当前生命
	nuai_curmana,					// 当前法力
	nuai_experience,				// Npc被杀获得的经验
	nuai_skillexp,					// Npc被杀获得的技能经验
	nuai_pkmode,					// Npc的PK模式
	nuai_titlecolor,				// Npc当前的名字颜色
	nuai_servercont,				// 服务器附加条件
	nuai_needupdate,				// 是否需要更新数据
	nuai_nomove,					// 是否可移动
	nuai_deathmode,					// 死亡模式
	nuai_fightstate,				// 战斗状态

	// 注意: 更改下面四项资源的时候，请务必更改GameDataDef.h中的 CITY_RES_TYPE_COUNT 定义
	
	nuai_lord_res0,					// 诸侯金钱
	nuai_lord_res1,					// 诸侯青铜
	nuai_lord_res2,					// 诸侯毛皮
	nuai_lord_res3,					// 诸侯木材

	nuai_owner,						// 掉落物品主人
	nuai_owner_team,				// 掉落物品主人（队伍）
	nuai_dir,						// 方向
	nuai_city_taxrate,				// 城市税率
	nuai_team_id,					// 队伍编号

	// Add new attribute here, don't change previous' order
	nuai_camou_flage,               // 是否处于蒙面状态

	nuai_end,
};

enum enNpc_Attr_Type
{
	npc_attr_invalid,
	npc_attr_unary,
	npc_attr_comp,
	npc_attr_range,
	npc_attr_end,
};

enum
{
	INSIDE_NONE = 0,	// 非驻扎状态
	INSIDE_WEAPON,		// 驻扎在攻城武器中
	INSIDE_BUILDING,	// 驻扎在建筑中
};

enum
{
	INVADER_STATE_NULL = 0,
	INVADER_STATE_ENEMY,
	INVADER_STATE_FRIEND,
};

typedef struct
{
	NPCCMD		CmdKind;		// 命令C
	int			Param_X;		// 参数X
	int			Param_Y;		// 参数Y
	int			Param_Z;		// 参数Y
} NPC_COMMAND;

typedef struct
{
	int		nTotalFrame;
	int		nCurrentFrame;
} DOING_FRAME;

struct	KSyncPos
{
	DWORD	m_dwRegionID;
	int		m_nMapX;
	int		m_nMapY;
	int		m_nOffX;
	int		m_nOffY;
	int		m_nDoing;
};

typedef struct tagPRODUCESTATE
{
	int nProducePastTime;
	int nProduceSpeed;
	int nProduceNpcIndex;
}PRODUCESTATE, *PPRODUCESTATE;

struct RequestNpcAddon
{
	char	szSayMessage[256];
	BYTE	btSayType;
};

//施放技能参数
struct CastSkillParam
{
	CastSkillParam()
	{
		SkillId = INVALID_SKILL_ID;
		Param1 = 0;
		Param2 = 0;
	}

	CastSkillParam& operator= (const CastSkillParam &rhs)
	{
		if(this != &rhs)
		{
			SkillId = rhs.SkillId;
			Param1 = rhs.Param1;
			Param2 = rhs.Param2;
		}

		return *this;
	}

	int SkillId;
	int Param1;
	int Param2;
};

//NPC载入/保存状态
enum enumNpcLoadSaveState
{
	npc_load_save_state_waiting = 0,	//等待被载入
	npc_load_save_state_loading,		//正在载入
	npc_load_save_state_loaded,			//已经载入
	npc_load_save_state_saving,			//正在保存
	npc_load_save_state_deleted,		//已经删除
};

#ifdef _SERVER
/************************************************************************/
/*							DropRate Config		                        */
/************************************************************************/	
#define MAX_DROP_GROUP 10

struct KItemDropRate
{
	struct	KItemParam
	{
		int		nGenre;
		int		nDetailType;
		int		nParticulType;
		int		nLevel;
		int		nItemCount;
		unsigned long	ulRate;
	};
	int				nCount;
	unsigned long	ulMaxRandRate;
	unsigned long	ulMoneyRandRate;
	unsigned long	ulMoneyMin;
	unsigned long	ulMoneyMax;
	KItemParam*	pItemParam;
};
#else
	//用于标明客户端npc是哪个region的第几个npc，如果这是一个服务器控制的npc ，ID 值为 0 ，No 值为 -1。
	struct	KClientNpcID
	{
		DWORD	m_dwRegionID;
		int		m_nNo;
	};

	struct KBUFPARAM 
	{
		int nBufIdx;	//指向UI中buf的索引
		int nTime;		//buf持续时间(ms)-1无时间限制
		int nState;		//buf状态
		int nSkillID;	//buf对应的技能id
	};
#endif

class KNpcTemplate;
	
class KNpc
{
	friend class KNpcSet;
	friend class KSpawnPointList;
    friend class SpawnRecoder;
	enum _enRoleInfo
	{
		role_info_icon,
		role_info_zhuhou,
		role_info_shizu,
		role_info_title,
		role_info_name,	
		role_info_count,
	};

public:
	
	bool IsValid() const;//判断是否是合法的NPC
	DWORD GetId() const;//得到ID
	int GetCurrentLifePercentage();//当前血量%
	int GetCurrentManaPercentage();//当前魔法值%
	void SetCurrentLifePercentage(int lifePercentage);//设置当前血量%
	void SetCurrentManaPercentage(int manaPercentage);//设置当前魔法值%
	bool IsAlive() const;//判断NPC是否活着
	int GetKind() const;//得到类型
	int GetDialogRadius() const;//得到对话半径
	NpcSkillList& GetSkillList();//得到技能列表
	const KNpcTemplate* GetTemplate();//得到模版
	bool IsInWorldCombatInstance(void);

#ifdef _SERVER
	void SyncNpcDir( int nDir );


	void NoMove( int nOp );
	
	void BeUsedSkill(int skillCaster, SkillType skillType);//受到技能
	int GetSubWorldIndex();
	int GetNextAITime() const;
	void SetNextAITime(int nextAITime);		
	NpcController& GetController();

	const FSGUID& GetLord() const
	{ return m_Lord; }

	void SetLord(const FSGUID& guid)
	{
		m_UnaryAttrMgr.Set( nuai_needupdate, 1 );		
		m_Lord = guid; 
	}

	void AddCityRes(int nResIdx, int nValue);
	int	 GetCurCityLordNpc();

	const FSGUID& GetRobber() const
	{ return m_Robber; }

	void SetRobber(const FSGUID& guid)
	{ m_Robber = guid; }

	int GetActionTime() const;//动作触发时间

	enumNpcLoadSaveState GetLoadSaveState() const;//得到载入/保存状态
	void SetLoadSaveState(enumNpcLoadSaveState state);//设置载入/保存状态
	bool Save();//NPC数据存盘
	void	SetFightState( bool bFight )
	{
		if ( bFight && g_pController )
		{
			m_dwFightStateTimeBegin	= UNIX_TMIE_STAMP;
		}
		else
		{
			m_dwFightStateTimeBegin	= 0;
		}

		int oldState = m_UnaryAttrMgr[nuai_fightstate];
		m_UnaryAttrMgr.Set( nuai_fightstate, bFight );
		
		if (m_UnaryAttrMgr[nuai_fightstate] != oldState)
		{
			SyncAttr( npc_attr_unary, nuai_fightstate, 0, true );
		}
	}
	bool	IsFightState( void )
	{
		return m_UnaryAttrMgr[nuai_fightstate] > 0;
	}

	void                SetCamoflag(const int bSet);
	int                 GetComoflag(void);
	int					GetCombatScoreCalcType();

#else
	void UpdataNpcRes( int nNpcIdx, int nDir, int nAllFrame, int nCurFrame, 
		BOOL bInMenu = FALSE, BOOL bDrawSelected = FALSE, int nSelectedType = 0 );

	ClientTalismanNpcController& GetTalismanNpcController();
	ClientCombatInfoShower& GetCombatInfoShower();

	typedef std::pair< unsigned long, int > BUFFPAIR;
	typedef std::list<BUFFPAIR>	C_BUFFLIST;
#endif

public:
	inline  void AddUnaryAttr(enNpc_UnaryAttr_Idx idx, int nVal)
	{
		m_UnaryAttrMgr.Set(idx, m_UnaryAttrMgr[idx] + nVal);
	}

	inline void AddCompAttr(enNpc_CompoundAttr_Idx attrIdx, enCompoundMemIdx memberIdx, int nVal)
	{
		m_CompAttrMgr.Set(attrIdx, memberIdx, m_CompAttrMgr[attrIdx][memberIdx] + nVal);
	}

	inline void AddRangeAttr(enNpc_RangeAttr_Idx idx, enRangeMemberIdx compIdx, 
							 enCompoundMemIdx memberIdx, int nVal)
	{
		m_RangeAttrMgr.Set(idx, compIdx, memberIdx, m_RangeAttrMgr[idx][compIdx][memberIdx] + nVal);
	}

	inline void ClearRAAppendVal(int idx)
	{
		m_RangeAttrMgr.Set(idx, idx_value_low, idx_append_value, 0);
		m_RangeAttrMgr.Set(idx, idx_value_low, idx_append_percent, 0);
		m_RangeAttrMgr.Set(idx, idx_value_hight, idx_append_value, 0);
		m_RangeAttrMgr.Set(idx, idx_value_hight, idx_append_percent, 0);
	}

	inline void ClearRAAllVal(int idx)
	{
		m_RangeAttrMgr.Set(idx, idx_value_low, idx_base_value, 0);
		m_RangeAttrMgr.Set(idx, idx_value_hight, idx_base_value, 0);
		ClearRAAppendVal(idx);
	}

	inline void SetRABaseValue(enNpc_RangeAttr_Idx idx, int nLowVal, int nHightVal)
	{
		m_RangeAttrMgr.Set(idx, idx_value_low, idx_base_value, nLowVal);
		m_RangeAttrMgr.Set(idx, idx_value_hight, idx_base_value, nHightVal);
	}

	// 更新四项基本属性变化对其他属性的影响
	void				UpdateBodyEffect(int nOldVal, int nAddedVal, bool bSyncToClient, bool bBroadCast);
	void				UpdateNimbusEffect(int nOldVal, int nAddedVal, bool bSyncToClient, bool bBroadCast);
	void				UpdateStrengthEffect(int nOldVal, int nAddedVal, bool bSyncToClient, bool bBroadCast);
	void				UpdateArtEffect(int nOldVal, int nAddedVal, bool bSyncToClient, bool bBroadCast);

	BOOL				WaitForFrame();
	BOOL				IsReachFrame(int nPercent);
	void				DoStand();
	void				OnStand();
	void				DoRevive();
	void				OnRevive();
	void				DoWait();
	void				OnWait();
	void				DoWalk();
	void				OnWalk();
	void				DoRun();
	void				OnRun();
	void				DoSkill(int nX, int nY);
	int					DoOrdinSkill(KSkill * pSkill, int nX, int nY);
	void				OnSkill();
	void				DoSit();
	void				OnSit();
	void				DoHurt(int nHurtFrames = 0, int nX = 0, int nY =0);
	void				OnHurt();

	// mode == 0 npc 导致 == 1 player 导致，不掉东西 == 2 player 导致，掉东西
	// 与 DeathPunish 的参数对应 具体参阅 enumDEATH_MODE
	void				DoDeath(int nMode = 0);
	void				OnDeath();
	void				DoDefense();
	void				DoIdle();
	
//	有关格斗技能的------------------------------------------

	BOOL				DoManyAttack();
	void				OnManyAttack();
	BOOL				DoBlurAttack();
	BOOL				DoRunAttack();
	void				OnRunAttack();
	BOOL				CastMeleeSkill(KSkill * pSkill);
//-----------------------------------------------------------

	void				DoSpecial1();
	void				OnSpecial1();
	void				OnSummonSkill();
	void				DoSpecial3();
	void				OnSpecial3();
	void				DoSpecial4();
	void				OnSpecial4();
	void				Goto(int nMpsX, int nMpsY);
	void				RunTo(int nMpsX, int nMpsY);
	void				ServeMove(int nSpeed);
	BOOL				NewPath(int nMpsX, int nMpsY);
	BOOL				CheckHitTarget(int nAR, int nDf, int nIngore = 0);
private:
#ifdef _SERVER
	void				LoseMoney(KItemDropRate *pDropRate, int nBelongPlayer, float fScale = 1.0f );
	void                WorldCombatScoreKilledCulc( int nKiller);
#else
	void				HurtAutoMove();
#endif

public:
	BOOL				IsAttackTarget(int nTargetIdx, int nAttackTargetType);

private:
	void BakupMoveStatus()
	{
		if(m_Doing == do_run)
		{
			m_MoveStatus = do_run;
			m_nMoveDesX = m_DesX;
			m_nMoveDesY = m_DesY;
		}
	}

	void ClearMoveStatus()
	{
		m_MoveStatus = do_none;
		m_nMoveDesX = 0;
		m_nMoveDesY = 0;
	}

	void RestoreMoveStatus()
	{
		SendCommand(m_MoveStatus, m_nMoveDesX, m_nMoveDesY);
	}

public:

#ifndef _SERVER
	void				RecvAttrSync(int nAttrType, int nAttrIdx, BYTE valMask, int *pVal);
#endif

#ifndef _SERVER
	void				LevelUpSkill(int nSkillId, int nAddedLevel);
#endif

#ifdef _SERVER
	BOOL				LoseItems(KItemDropRate *pDropRate, int nBelongPlayer, BOOL bAutoPicked = FALSE, BOOL *pNoPlace = NULL, float fScale = 1.0f);
	BOOL				LoseSingleItem(KItemDropRate *pDropRate, int nBelongPlayer, BOOL bAutoPicked = FALSE, BOOL *pNoPlace = NULL, float fScale = 1.0f);
	
	void				GetNpcDamage(int *pOutRst);
	void				GetNpcDefend(int *pOutRst);
	void				SyncAttr(int nAttrType, int nAttrIdx, BYTE valMask, bool bBroadCast = false);
	void                SyncCommoFlagInfo(void);
	void				SyncDamageInfo(int nLauncher, int nDamage, COMBAT_INFO_TYPE damType, int skillId, bool isCrit = false, bool bBroadCast = false);
	void				SetupEventBuff(enumNpcEvent npcEvent);
	void				SetNpcTitleColor( unsigned int color );
	void				AddInitSkills();
	void				CheckInitSkills();
	int					IsDeath( void )
						{ return (m_Doing == do_revive ); }

	BOOL				IsCanRandomTrans();
	BOOL				RandomTrans();
#endif

	KNpc();
	void				SetActiveFlag(BOOL bFlag) { m_bActivateFlag = bFlag; };
	void				CheckTrap();
	void				Init();
	void				Remove();
	void				Activate();
	BOOL				IsPlayer();
	bool				IsCreature();
	bool				IsEmployee() const;//是否为佣兵
	void				TurnTo(int nIdx);
	void				SendCommand(NPCCMD cmd, int x = 0, int y = 0, int z = 0);
	void				ProcCommand(int nAI);
	void				ProcStatus();
	KSkill*				GetActiveSkill();

	void				ChangePKMode(PK_MODE mode);

	int					GetSkillLevel(int nSkillId);
	void				SetId(DWORD	dwID)	
	{ 
		m_dwID = dwID;
	};
	BOOL				IsMatch(DWORD dwID)	{ return dwID == m_dwID; };	// 是否ID与该Index匹配
	BOOL				Cost(KSkill *pSkill, BOOL bOnlyCheckCanCast = FALSE, enumSkillUseableResult* pResult = NULL);				// 消耗内力体力等,如果OnlyCheckCanCost为TRUE,表示只是检查当前的内力等是否够消耗，并不实际的扣
	void				SelfDamage(int nDamage);						// 自身的伤害，如牺牲攻击
	void				Load(int nNpcSettingIdx, int nLevel);						// 从TabFile中加载
	void				GetMpsPos(int * pPosX, int *pPosY) const;
	BOOL				SetActiveSkill(int nSkillIdx, int nTarget = -1);
	int					ModifyMissleLifeTime(int nLifeTime);
	void				RestoreNpcBaseInfo(); //Set Current_Data ;
	void				RestoreState();
	BOOL				SetPlayerIdx(int nIdx);
	void				DialogNpc(int nIndex);
	void				Revive();
	void				SetSeries(int nSeries);// 设定此 npc 的五行属性（内容还没完成）
	void				GetNpcCopyFromTemplate(int nNpcTemplateId, int nLevel);
	void				SetBaseAttackRating(int nAttackRating);				// 设定攻击命中率
	int					GetCurActiveWeaponSkill();
	void				LoadDataFromTemplate(int nNpcTemplateId, int nLevel);
	BOOL				IsCommandExist(NPCCMD eCmd);						//一个命令是否已经在NPC的命令队列里
	bool				IsInSafeArea( );
	bool				IsInPKArea( );
	bool				IsInEspecialArea(int areaType);//判断是否在指定的区域
	int					CheckEspecialAreaType() const;//检查并返回当前特殊区域类型	

	inline int			GetPlayerIdx()
	{
		return m_Kind != kind_player ? 0 : m_nPlayerIdx;
	}

	inline	void		SetProcessAI(BOOL bVal)
	{
		m_ProcessAI = bVal;
	}

	inline int GetSummonerIdx() const; //XX 使用原有变量m_nPlayerIdx
	inline BOOL SetSummonerIdx(int nPlayerIdx);	
	int	SetPos(int nX, int nY);
#ifdef	_SERVER
	void				ExecuteRevive(){DoRevive();};
	BOOL				SendSyncData(int nClient, int nNpcIndex);		// 向一个客户端发完整同步数据
	void				NormalSync();									// 广播小同步
	void				BroadCastRevive(int nType);
	void				BroadCastMsg(void * pData, const unsigned int ulSize);
	BOOL				ReceiveDamage(int nLauncher, int skillId, const OutputDamageInfo *pDamage, int nDoHurt, bool isCrit);

	// mode == 0 npc 导致 == 1 player 导致，不掉东西 == 2 player 导致，掉东西
	// 与 DoDeath 的参数对应 具体参阅 enumDEATH_MODE
	int					DeathPunish(int nMode, int nBelongPlayer);
	int					ChangeWorld(DWORD dwSubWorldID, int nX, int nY, bool isInstance = false);	// 切换世界
private:
	int					ChangeWorldNpc(DWORD dwSubWorldID, int nX, int nY, bool isInstance);	// 切换世界
public:
	void				SendDataToNearRegion(void* pBuffer, DWORD dwSize);// 向周围九屏广播
	int					DeathCalcPKValue(int nKiller);					// 死亡时候计算PK值
	BOOL				CheckPlayerAround(int nPlayerIdx);// 查找周围9个Region中是否有指定的 player
	void				InitProduceState(int nProduceNpcIndex, int nProduceSpeed);
	void				ClearProduceState();
	void				SyncNpcPos(int nSyncTarget = 0, BOOL bIncludeSelf = TRUE);
#endif
#ifndef _SERVER
	void SetUiLoginNpcDoing( int nDoing );

	inline void SetPlayUiLoginNpc( bool bCanShowUiLoginPlayer )
	{
		m_bCanShowUiLoginPlayer = bCanShowUiLoginPlayer;
		DoStand();
	}
	inline void	SetCanFollowAndAttack( bool bFAA )
	{
		m_bCanFollowAndAttack = bFAA;
	}
	inline bool GetCanFollowAndAttack( void )
	{
		return m_bCanFollowAndAttack;
	}
	inline KSkill*	KNpc::GetActiveLeftSkill()
	{
		_ASSERT(m_LeftSkillID < MAX_SKILL);
		
		return g_SkillManager.GetSkill(m_LeftSkillID, 1);	
	}

	inline KSkill*	KNpc::GetActiveRightSkill()
	{
		_ASSERT(m_RightSkillID < MAX_SKILL);
		
		return g_SkillManager.GetSkill(m_RightSkillID, 1);	
	}
	void				SetSleepMode(BOOL bSleep) { m_nSleepFlag = bSleep; m_DataRes.SetSleepState(bSleep);};
	void				RemoveRes();
	void				ProcNetCommand(NPCCMD cmd, int x = 0, int y = 0, int z = 0);
	void				Paint();
	void				PaintBubble(void);
	void				PaintUI(int x, int y);
	void				UpdataPaintInfo( int offsetHeight,char* showName);
	int					PaintInfo(int nHeightOffset, bool bSelec);
	void                HideInfo();
	void                DelInfo();
	int					PaintChat(int nHeightOffset);
	int					SetChatInfo(const char* Name, const char* pMsgBuff, unsigned short nMsgLength);
	int					PaintLife(int nHeightOffset, bool bSelect);
	int					PaintMana(int nHeightOffset);
	void				DrawBorder();
	int					DrawMenuState(int n);
	void				DrawBlood();	//绘制血条和名字在固定位置
	BOOL				IsCanInput() { return m_ProcessAI; };
	void				SetMenuState(int nState, char *lpszSentence = NULL, int nLength = 0);	// 设定头顶状态
	int					GetMenuState();				// 获得头顶状态
	DWORD				SearchAroundID(DWORD dwID);	// 查找周围9个Region中是否有指定 ID 的 npc
	void				SetSpecialSpr(char *lpszSprName);// 设定特殊的只播放一遍的随身spr文件
	void				SetInstantSpr(unsigned int nNo);
	int					GetNormalNpcStandDir(int nFrame);
	KNpcRes*			GetNpcRes(){return &m_DataRes;};

	int GetNpcPate();
	int GetNpcPatePeopleInfo();
	void				ProcessAddon(RequestNpcAddon* pAddon);
	void				AddBuffToC(unsigned long ulBuffID, int nTempID) ;
	void				RemoveBuffFromC(unsigned long ulBuffID);
	void				ClearBuffFromC( ) { PolyMorph( -1, true, 0,0, 0 ); m_DataRes.ClearAllState(); m_BuffList_C.clear( ); }
	C_BUFFLIST&	GetBuffList_C( void ) { return m_BuffList_C; } 
	void				CheckAndNotifyEspecialArea();//检查并提示特殊区域
	BOOL				SetEmployerId(int npcId);
#endif

	bool			m_bTheSameTarget;

public:
	bool	IsTheSameTarget( void )
	{
		return m_bTheSameTarget;
	};
	void SetTarget(unsigned int nType,int nIdx);
	inline int GetTargetType(void)
	{
		return m_nTargetType;
	}
	inline int GetTargetObj(void)
	{
		if (type_obj == m_nTargetType) 
		{
			return m_nTargetIdx;
		}
		return 0;
	}
	inline int GetTargetNpc(void)
	{
		if (type_npc == m_nTargetType) 
		{
			return m_nTargetIdx;
		}
		return 0;
	}

private:
	int	m_nTargetType;
	int	m_nTargetIdx;
public:
	int PolyMorph(int nNpcType, BOOL bCanCast, unsigned int nSafeGuardLevel, int uMoveSpeed, unsigned int uTime); //变身
	inline int GetMorphType(void) const { return m_nMorphType; }
public:
	unsigned int m_nSafeGuardLevel;	// 被保护
	int m_uMoveSpeed;		// 移动速度
	unsigned int m_uPolyMorphTime;	// 变身持续时间
	BOOL m_bCanCast;
	int	m_nMorphType;		// 变身的类型
	BOOL	m_bSaveMorphType;// 变身信息是否保存
	int m_nMorphPart[itempart_num];

#ifndef _SERVER
	DWORD	m_dwPolyMorphTimeStamp;
	int		m_bIsPkArea;
#endif

public:

	unsigned int m_nBarrierWidth;
	unsigned int m_nBarrierHeight;
	BOOL m_bHaveBarrier;

public:

	int GetLevel() const 
	{ 
		return m_Level;
	}

	void SetLevel(int newLevel)
	{
		m_Level = newLevel;
	}
	
	int	GetSeries() const 
	{
		return m_Series;
	}

	int GetSex() const
	{
		return m_nSex;
	}

public:

	void	MoveNpc(int nRegion, int nMapX, int nMapY, int nOffX, int nOffY );
	int		GetMapX(void) const {	return m_MapX;	};
	int		GetMapY(void) const {	return m_MapY;	};
	int		GetMapZ(void) const {	return m_MapZ;	};
	int		GetOffX(void) const {	return m_OffX;	};
	int		GetOffY(void) const {	return m_OffY;	};
	int		GetNormalSkillId();

#ifndef _SERVER

	enum    HeadInfoPriority
	{
		HIP_Important = 0,
		HIP_Normal,
        HIP_Special,
		HIP_End
	};

	void			AddIconToHeadInfo( const std::string& imageset, const std::string& image, const std::string name , const HeadInfoPriority pri= HIP_Normal );
	void			AddTxtToHeadInfo( const std::string& text, const std::string colours, const std::string name , const HeadInfoPriority pri= HIP_Normal );
	void            AddLayoutToHeadInfo( const std::string& text, const std::string name , const HeadInfoPriority pri= HIP_Normal );
	void			DelHeadInfo(  const std::string name );
	void			CheckAndSwitchSkill();
	bool			GetBubble()			{ return m_bBubble; };
	void			SetBubble(bool b)	{ m_bBubble = b; };
	KNpcFindPath	GetPathFinder()		{ return m_PathFinder; };
	void			SetHeadInfoChanged(bool b)	{ m_bHeadInfoChanged = b; };

#endif
	
#ifdef _SERVER
public:
	struct KSpawnPoint_Runtime_Info* GetSpawnInfo() const
	{
		return m_pSpawnInfo;
	}

	struct WorldSpawner_RunTimeInfo* GetWorldSpawnInfo() const
	{
		return m_pWorldSpawnInfo;
	}
	void	SetDataChangedFlag(bool bFlag);
	bool	IsDataHasChanged();

private:
	struct KSpawnPoint_Runtime_Info* m_pSpawnInfo;
    struct WorldSpawner_RunTimeInfo* m_pWorldSpawnInfo;
#endif

private:
#ifndef _SERVER
	BYTE						m_UnitOwnerFlag[MAX_SOCIETY_LAYER_COUNT];
	ActionType					m_eActionType;
	int							m_nBeginFrame;
	int							m_nEndFrame;
	bool						m_bShowSelect;

#endif

#ifdef _SERVER
	int							m_nRealActiveSkillIdForCD;				// 被转换前的技能id，用于cooldown
#endif
	int							m_nDropRateAttenuation;					// NPC是否受掉装衰减，0不受影响
	int							m_nCurDamage;							// 当前的伤害值，用于同步到客户端显示
	int							m_LoopFrames;							// 循环帧数
	int							m_nPlayerIdx;				
	int							m_DeathFrame;							// 死亡帧数
	int							m_StandFrame;
	int							m_HurtFrame;
	int							m_AttackFrame;
	int							m_CastFrame;
	int							m_WalkFrame;
	int							m_RunFrame;
	int							m_StandFrame1;
	int							m_ReviveFrame;							// 重生帧数
	int							m_SitFrame;
	NPC_COMMAND					m_Command;								// 命令结构
	BOOL						m_ProcessAI;							// 处理AI标志
	BOOL						m_ProcessState;							// 处理状态标志
	int							m_XFactor;
	int							m_YFactor;
	int							m_SpecialSkillStep;						// 特殊技能步骤
	NPC_COMMAND					m_SpecialSkillCommand;					// 特殊技能行为命令
	KNpcFindPath				m_PathFinder;
	BOOL						m_bActivateFlag;
#ifdef _SERVER
	BOOL						m_bTransSkill;
	int							m_nSaveNpcTimeInternal;					
	bool						m_IsDataHasChanged;						
	//必须为社会组织
	FSGUID						m_Lord;									//Npc占有者
	FSGUID						m_Robber;								//Npc攻击者
#else
	KCacheNode*					m_pBuffSoundNode;
	KWavSound*					m_pBuffWave;
	bool						m_bSelect;
	bool						m_bBubble;
	int							m_nEffectPolyMorphBuffIdx;
	int							m_CurrentLifePercentage;
	int							m_CurrentManaPercentage;
	bool						m_bCanFollowAndAttack;
	bool						m_bHeadInfoChanged;
	char						m_RoleHeadInfo[COMMON_CLIENT_MSG_LEN_1024];
	char*						m_RoleInfoRes[role_info_count];

	typedef struct tagHEAD_INFO
	{
       std::string              m_InfoString;
	   HeadInfoPriority         m_Prioryty;
	}HEAD_INFO;

	std::map<std::string, HEAD_INFO>	m_HeadInfoPlus;

#endif
public:
	int							m_nItemInlayCount;
	bool						m_bShowTargetFace;
	DWORD						m_dwID;									// Npc的ID
	int							m_Index;								// Npc的索引
	DWORD						m_Kind;									// Npc的类型
	KIndexNode					m_Node;									// Npc's Node
	int							m_Level;								// Npc的等级
	int							m_Series;								// Npc的系(0：甲士，1：道士，2：异人)
	int							m_SkillType;								// Npc的系(0：甲士，1：道士，2：异人)
	int							m_Height;								// Npc的高度(跳跃的时候非零)
	int							m_nStature;								// Tall 
	BYTE						m_btRankId;
	BYTE						m_btKilledType;
	int							m_SiegeWeaponIndex;						// 对应攻城武器的索引(如果是攻城器械的话)
	int							m_InsideSiegeWeaponIndex;				// 驻扎的攻城器械索引
	int							m_ResDir;								// 为取得人物当前面对方向
	BYTE						m_btStateInfo[MAX_NPC_RECORDER_STATE];	// Npc当前最新的几个状态 
	int							m_nNextStatePos;						// 下一次状态在m_btState的位置	
	BOOL						m_bHaveLoadedFromTemplate;				//用于Npc模板库中，当FALSE表示该Npc数据当前是无效的 ，数值未经过脚本计算，需要生成.TRUE表示有效数据
	PRODUCESTATE				m_tagProduceState;						// Produce infomation
	int							m_Camp;									// Npc的阵营
	int							m_CurrentCamp;							// Npc的当前阵营
	NPCCMD						m_Doing;								// Npc的行为
	NPCCMD						m_MoveStatus;
	int							m_nMoveDesX;
	int							m_nMoveDesY;
	int							m_ClientDoing;							// Npc的客户端行为
	DOING_FRAME					m_Frames;								// Npc的行为帧数
	int							m_SubWorldIndex;						// Npc所在的SubWorld ID
	int							m_RegionIndex;							// Npc所在的Region ID
	BOOL						m_bPhysicsCanAttack;
	int							m_DialogRadius;							// Npc的对话范围
	CompoundAttrMgr<ncai_end>	m_CompAttrMgr;							// Npc的一些复合属性，由基础值、附加数值、附加百分比组成
	RangeAttrMgr<nrai_end>		m_RangeAttrMgr;							// Npc的一些范围属性，由最小值、最大值组成，其中最小值，最大值由复合属性组成
	UnaryAttrMgr<int, nuai_end>	m_UnaryAttrMgr;							// Npc的一元属性，由单值组成
	//左右手技能
	NpcSkillList				m_SkillList;
	int							m_ActiveSkillID;
	int							m_ActiveSkillLevel;
	// 一些效果, 通过技能或装备动态加上勿删，后期加上
	int							m_CurrentTreasure;						// Npc丢落装备的数量
	int							m_CurrentTreasure1;
	int							m_Treasure;								// Npc丢落装备的数量
	int							m_Treasure1;
//	int							m_nAddProduceSpeedV;
//	int							m_nAddProduceSpeedP;
	// 只需要当前值的数据结束
//	int							m_Dir;									// Npc的方向

//	m_MapX, m_MapY, m_MapZ,m_OffX,m_OffY这些变量会影响阻挡因此不要直接访问
//	请使用相应的Get方法读取，和MoveNpc修改变量
private:
	int							m_MapX, m_MapY, m_MapZ;					// Npc的地图坐标
	int							m_OffX,	m_OffY;							// Npc在格子中的偏移坐标（放大了1024倍）
//	m_MapX, m_MapY, m_MapZ,m_OffX,m_OffY这些变量会影响阻挡因此不要直接访问
//	请使用相应的Get方法读取，和MoveNpc修改变量
public:
	int							m_DesX, m_DesY;							// Npc的目标坐标
	int							m_SkillParam1, m_SkillParam2;
	int							m_SkillParam3, m_SkillParam4;
	int							m_OriginX, m_OriginY;					// Npc的原始坐标
	int							m_NextAITime;
	int							m_AIMAXTime;
	// Npc的装备（决定客户端的换装备）
	int							m_HelmType;								// Npc的头盔类型
	int							m_ArmorType;							// Npc的盔甲类型
	int							m_WeaponType;							// Npc的武器类型
	int							m_ShoulderType;							// Npc的装饰物品
	int							m_BootType;							// Npc的装饰物品
	int							m_CuffType;							// Npc的装饰物品
	int							m_HorseType;							// Npc的骑马类型

	int							m_HelmPal;					// Npc的头盔类型
	int							m_ArmorPal;					// Npc的盔甲类型
	int							m_WeaponPal;				// Npc的武器类型
	int							m_ShoulderPal;							// Npc的装饰物品
	int							m_BootPal;							// Npc的装饰物品
	int							m_CuffPal;							// Npc的装饰物品
	int							m_HorsePal;					// Npc的骑马类型

	BOOL						m_bRideHorse;							// Npc是否骑马
	char						Name[32];								// Npc的名称
	int							m_nSex;									// Npc的性别0为男，1为女
	int							m_NpcSettingIdx;						// Npc的设定文件索引
 	int							m_CorpseSettingIdx;						// Npc的尸体定义索引
	DWORD						m_ActionScriptID;						// Npc的行为脚本ID（使用时用这个来检索）
	DWORD						m_TrapScriptID;							// Npc的当前Trap脚本ID;
	int							m_nLastDamageIdx;						// 最后一次伤害的人物索引
	int							m_nLastPoisonDamageIdx;					// 最后一次毒伤害的人物索引
	BOOL						m_bClientOnly;			 
	int							m_nCurrentMeleeSkill;					// Npc当前正执行的格斗技能
	int							m_nCurrentMeleeTime;	
	// AI参数
	int							m_AiMode;								// AI模式
	int							m_AiParam[MAX_AI_PARAM];				// 用于AI模块计算AI
	int							m_AiAddLifeTime;
	char						m_HeadImage[128];
	char						m_HeadImageSet[128];
	int							m_nHeadImage;
	// 套装效果 -1表示没有 否则是套装ID (变身时不表现套装效果)
	int							m_nActiveGreeItemID;
	int							m_nSetEfficacyType;
	BOOL						m_bRegionRefAdded;	
	KNpcTemplate*				m_pTemplate;
	DWORD                       m_WorldCombatOrg;
	int                         m_WorldCombatKilledScore;

#ifdef _SERVER
	int							m_AiSkillRadiusLoadFlag;				// 战斗npc技能范围是否已经初始化 只需要在构造的时候初始化一次
	KItemDropRate*				m_pDropRate;
	KItemDropRate*				m_pDropRateGroup[MAX_DROP_GROUP];
	DWORD						m_dwDeathScriptID;
	int							m_nCurPKPunishState;					// PK死亡时的惩罚性质，用于国战
	int							m_nInitColorIdx;
    int							m_nNpcColor;							//KNpcTemplate的m_nColor
    int							m_nDeadlyStrikeResist;
    int							m_nFatallyStrikeResist;
	int							m_nFreezeTimeReduce;
	DWORD						m_dwFightStateTimeBegin;
	DWORD						m_dwFightStateTimeMax;
#else	
	DWORD						m_ulTime;
	struct KTechnologyEffect
	{
		int nIndex;
		int nLevel;
	};
	bool						m_bCanShowUiLoginPlayer;
public:	
	bool						m_bShowPlayerAlpha;
	KNpcRes						m_DataRes;								// Npc的客户端资源（图象、声音）
	BYTE						m_byPetHonor;
	int							m_LeftSkillID;
	int							m_RightSkillID;
	int							m_bIsLeftSkill;
	int							m_nRealPosX;
	int							m_nRealPosY;
	int							m_nRealDir;
	int							m_nActiveSkillIdx;
	BOOL						m_bDoubleExp;							//双倍经验
	int							m_nPetIndex;							//当该NPC为玩家,则记录的是宠物的index，当为宠物的时候,记录的是主人的index,宠物的类型记录在m_nPlayerIndex上
	BYTE						m_byPetType;							//宠物的类型
	BYTE						m_byNpcLastInWar;
	int							m_SyncSignal;							// 同步信号
	KClientNpcID				m_sClientNpcID;							// 用于标明客户端npc是哪个region的第几个npc
	DWORD						m_dwRegionID;							// 本npc所在region的id
	KSyncPos					m_sSyncPos;
	int							m_nPKFlag;								// 0 练功模式  1 战斗模式  2 屠杀模式
	char						m_szChatBuffer[MAX_SENTENCE_LENGTH];
	int							m_nChatContentLen;
	int							m_nChatNumLine;
	int							m_nChatFontWidth;
	unsigned int				m_nCurChatTime;
	int							m_nSleepFlag;
	int							m_nHurtHeight;
	int							m_nHurtDesX;
	int							m_nHurtDesY;
	C_BUFFLIST					m_BuffList_C;
	
#endif

#ifdef _SERVER

	int           m_LifeLimitedHoldPercentage;

	NpcController m_Controller;
	int m_ActionTime;
	enumNpcLoadSaveState m_LoadSaveState;
	int m_VisibleToNpcCount;
	int m_VisibleToPlayerCount;
	unsigned long m_ExpireTime;//失效时间（失效后自动删除）
	int m_SyncToWorldMode;//向世界同步的方式（0：不同步；>0：同步）
	int m_SyncToWorldParams[MAX_SYNC_TO_WORLD_PARAM_COUNT];
	int m_CustomVariable[MAX_NPC_CUSTOM_VARIABLE_COUNT];//自定义变量

	int           m_LastDialogPosX;
	int           m_LastDialogPosY;
	int           m_LastDialogSubWordID;

	void          RecordDialogPos(int nSubwordID,int nX,int nY);
	void          GetDialogPos(int & nSubworldID,int & nX,int & nY);
#endif

	int m_EquipTalismanNpcId;//装备法宝对应的NPC的编号
	int GetEquipTalismanNpcId() const;
	void SetEquipTalismanNpcId(int id);
#ifdef _SERVER
	void SyncEquipTalismanNpcId();//同步装备法宝
	bool IsVisibleToNpc() const;//是否可以被怪物发现
	int GetVisibleToNpcCount() const;//得到是否怪物可见计数
	void SetVisibleToNpcCount(int visibleCount);//设置是否怪物可以见计数
	bool IsVisibleToPlayer() const;//是否可以被玩家发现
	int GetVisibleToPlayerCount() const;//得到是否玩家可见计数
	void SetVisibleToPlayerCount(int visibleCount);//设置是否玩家可以见计数
	void BroadCastRegion(const void* pBuffer, unsigned int size, int &maxBroadCastCount);//广播
	bool CheckExpire();//检查是否过期
	void SetExpire(unsigned long expireUnixTimestamp);//设置过期时间
	void SetSyncToWorldMode(int mode);//设置世界同步模式
	int GetSyncToWorldMode() const;//得到世界同步模式
	void SetSyncToWorldParam(int paramIndex, int paramValue);//设置世界同步参数
	int GetSyncToWorldParam(int paramIndex) const;//得到世界同步参数
	void SyncToWorld();//向世界同步（每次）
	void SyncToWorldMin(const unsigned int clientId);//向世界同步（初始）
	void SyncToWorldDel();//向世界同步（删除）
	int GetCustomVariable(int varIndex) const;//得到自定义变量
	void SetCustomVariable(int varIndex, int varValue);//设置自定义变量
	bool IsCastingSkill() const;//是否正在施放技能
	bool CanCancelSkill();//是否可以取消当前技能动作
	bool CancelSkill();//取消当前技能动作
	int GetEmployerIdx() const;
	void SetEmployerIdx(int nPlayerIdx);
#else
	ClientTalismanNpcController m_TalismanNpcController;
	ClientCombatInfoShower m_CombatInfoShower;
#endif
	
#ifndef _SERVER
	DWORD	m_nLastQuestStateQueryTime;
#endif

#ifndef _SERVER
public:
	void SetCityId(int cityId);//设置城市ID
	int  GetCityId();//得到城市ID
	bool IsKing() const;//是否是侯主
	void SetKing(bool isKing);//设置是否是侯主
	bool IsGensMgr()const;//是否是氏族长
	void SetGensMgr(bool isGensMgr);//设置是否是氏族长
	bool IsLeaguer()const;          //是否是盟主
	void SetLeaguer(bool isLeaguer);//设置是否是盟主
	void SetZhuhouName(const char* pName);//设置诸侯名
	void SetShizuName(const char* pName);//设置氏族名
	void SetCityName(const char* pName);//设置氏族名
	void SetLeagueName( const char * pLeagueName);
	char*   GetLeagueName( void ){ return m_LeagueName; }
	char*	GetZhuhouName( void ){ return m_ZhuhouName; };
	char*	GetShizuName(){ return m_ShizuName; };
	char*	GetCityName(){ return m_CityName; };
	void SetTitle(int titleIndex, int titleLevel);//设置称号
	void GetTitle(int& titleIndex, int& titleLevel) const;//得到称号

	void SetUnitOwnerFlag(int nLayer, BYTE flag)
	{
		if(nLayer >= 0 && nLayer < MAX_SOCIETY_LAYER_COUNT)
			m_UnitOwnerFlag[nLayer] = flag;
	}

	bool IsUnitOwner(int nLayer) const
	{
		if(nLayer >= 0 && nLayer < MAX_SOCIETY_LAYER_COUNT)
			return m_UnitOwnerFlag[nLayer] ? true : false;
		else
			return false;
	}

	void SetHasLeagueLayer( const bool bHas)
	{
		if (bHas != m_HasLeagueLayer)
			m_bHeadInfoChanged = true;
		
		m_HasLeagueLayer = bHas;
	}

	bool IsHasLeagueLayer( void )const
	{
		return m_HasLeagueLayer;
	}

	void SetInvaderTongFlag( const int nInvaderTongFlag )
	{
		if (nInvaderTongFlag != m_InvaderTongFlag)
			m_bHeadInfoChanged = true;

		m_InvaderTongFlag = nInvaderTongFlag;
	}

	int  GetInvaderTongFlag(void)const
	{
		return m_InvaderTongFlag;
	}

private:
	int  m_CityId;//城市ID
	bool m_IsKing;//是否是侯主
	bool m_IsGensMgr;//是否是氏族长
	bool m_IsLeaguer;//是否是盟主

	bool m_HasLeagueLayer; //是否加入联盟
	int  m_InvaderTongFlag;//是否是敌对联盟 （国战时有效）


	char m_ZhuhouName[17];//诸侯名
	char m_ShizuName[17];//氏族名
	char m_LeagueName[17];//联盟名 只有自己的才有效
	char m_CityName[32];//城市名
	int m_TitleIndex;//称号序号
	int m_TitleLevel;//称号等级
#endif

#ifdef _SERVER
	//Client To Server Pos Edit 
private:
   /*
   int   m_LastRunDX;
   int   m_LastRunDY;

   int   m_LastDxExp2;
   int   m_LastDyExp2;
   */
   int   m_LastEditFrame;

#ifdef _DEBUG
   int   m_ClientCheckedTimes;
   int   m_ServerPassTimes;
#endif


public:
   bool  CheckClientRunPos(int nCurX,int nCurY,int nDesX=0,int nDesY=0);
   void  SetPosDirectly(int nX,int nY,int iDir=-1);
   bool  CheckClientStandPos(int nStandX,int nStandY);
   void  SendClientStopCmd(void);
   void  SendClientPosEdition(void);
#else
   bool  m_IsPosEditionActive;
   bool  m_IsChangeWorld;
public:
   void  SendServerStopCmd(void);
   void  SendC2SPosSync(void);
   void  BeginEditionState(void);
   void  EndEditionState(void);
   void  SetChangeWorldFlag(const bool bChange);
   bool  GetChangeWorld(void)const;
#endif

public:
	int CalcDexterity();	
	int CalcVision();
	int CalcPhysExplode();
	int CalcMagicExplode();

	int CalcPhysicsDamage();
	int CalcPhysicsDamage(enRangeMemberIdx idx);
	int CalcPhysicsDefense();
	int CalcPhysicsDefense(enRangeMemberIdx idx);

	int CalcMagicDamage();
	int CalcMagicDamage(enRangeMemberIdx idx);
	int CalcEightDiagDfns();
	int CalcEightDiagDfns(enRangeMemberIdx idx);
	int CalcDarkDfns();
	int CalcDarkDfns(enRangeMemberIdx idx);

private:
	int RandomRangeValue(int lowValue, int highValue);
};

#ifdef _SERVER
inline void KNpc::SetDataChangedFlag(bool bFlag)
{
	m_IsDataHasChanged = bFlag;
}

inline bool KNpc::IsDataHasChanged()
{
	return m_IsDataHasChanged;
}


inline void  KNpc::RecordDialogPos(int nWorldID,int nX,int nY)
{
	m_LastDialogSubWordID = nWorldID;
	m_LastDialogPosX      = nX;
	m_LastDialogPosY      = nY;
}

inline void  KNpc::GetDialogPos(int & nWorldID ,int & nX,int & nY)
{
	nWorldID = m_LastDialogSubWordID;
	nX       = m_LastDialogPosX ;
	nY       = m_LastDialogPosY ;
}

#endif

inline int KNpc::GetEquipTalismanNpcId() const
{
	return m_EquipTalismanNpcId;
}

inline int KNpc::GetSummonerIdx() const //XX 使用原有变量m_nPlayerIdx
{
	return (kind_creature==m_Kind)?m_nPlayerIdx:0;
}

#ifdef _SERVER
inline int KNpc::GetEmployerIdx() const
{
	return (kind_employee == m_Kind) ? m_nPlayerIdx:0;
}

inline void KNpc::SetEmployerIdx(int nPlayerIdx)
{
	if (kind_employee == m_Kind)
	{
		m_nPlayerIdx = nPlayerIdx;
	}
}
#endif

// 在服务器存放NpcIdx 客户端存放NpcID
inline int KNpc::SetSummonerIdx(int nIdx)
{
#ifdef _SERVER
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return FALSE;
#endif

	if (kind_creature != m_Kind)
		return FALSE;

	m_nPlayerIdx = nIdx; // 这里存的是NpcIdx
	return TRUE;
}

inline KSkill*	KNpc::GetActiveSkill()
{
	_ASSERT(m_ActiveSkillID < MAX_SKILL);
	
	return g_SkillManager.GetSkill(m_ActiveSkillID, m_ActiveSkillLevel);	
}

inline int KNpc::GetCurrentLifePercentage()
{
#ifdef _SERVER

	int lifePercentage = 0;
	int currentLife = m_UnaryAttrMgr[nuai_curlife];
	int maxLife = m_CompAttrMgr[ncai_lifeuplimit];
	if (maxLife > 0)
	{
		lifePercentage = currentLife * 100 / maxLife;
		if (lifePercentage > 100)
			lifePercentage = 100;
		else if (lifePercentage < 0)
			lifePercentage = 0;
		else if (lifePercentage == 0 && currentLife > 0)
			lifePercentage = 1;
	}

	return lifePercentage;

#else

	return m_CurrentLifePercentage;

#endif
}

inline int KNpc::GetCurrentManaPercentage()
{
#ifdef _SERVER

	int manaPercentage = 0;
	int currentMana = m_UnaryAttrMgr[nuai_curmana];
	int maxMana = m_CompAttrMgr[ncai_manauplimit];
	if (maxMana > 0)
	{
		manaPercentage = currentMana * 100 / maxMana;
		if (manaPercentage > 100)
			manaPercentage = 100;
		else if (manaPercentage < 0)
			manaPercentage = 0;
	}

	return manaPercentage;

#else

	return m_CurrentManaPercentage;

#endif
}

inline void KNpc::SetCurrentLifePercentage(int lifePercentage)
{
#ifdef _SERVER	
	m_UnaryAttrMgr.Set(nuai_curlife, m_CompAttrMgr[ncai_lifeuplimit] * lifePercentage / 100);
#else
	m_CurrentLifePercentage = lifePercentage;
#endif
}

inline void KNpc::SetCurrentManaPercentage(int manaPercentage)
{
#ifdef _SERVER
	m_UnaryAttrMgr.Set(nuai_curmana, m_CompAttrMgr[ncai_manauplimit] * manaPercentage / 100);
#else
	m_CurrentManaPercentage = manaPercentage;
#endif
}

inline bool KNpc::IsValid() const
{
	return (m_dwID > 0);
}

inline DWORD KNpc::GetId() const
{
	return m_dwID;
}

inline bool KNpc::IsAlive() const
{
	return (m_Doing != do_death) && (m_Doing != do_revive);
}

inline int KNpc::GetKind() const
{
	return m_Kind;
}

inline int KNpc::GetDialogRadius() const
{
	return m_DialogRadius;
}

inline NpcSkillList& KNpc::GetSkillList()
{
	return m_SkillList;
}

inline const KNpcTemplate* KNpc::GetTemplate()
{
	return m_pTemplate;
}

inline bool KNpc::IsCreature()
{
	return m_Kind == kind_creature;
}

inline bool KNpc::IsInEspecialArea(int areaType)
{	
	return (CheckEspecialAreaType() == areaType);
}

inline bool KNpc::IsEmployee() const
{
	return m_Kind == kind_employee;
}

#ifdef _SERVER

inline void KNpc::ClearProduceState()
{
	m_tagProduceState.nProduceNpcIndex = 0;
	m_tagProduceState.nProduceSpeed = 0;
	m_tagProduceState.nProducePastTime = 0;
}

inline int KNpc::GetSubWorldIndex()
{
	return m_SubWorldIndex;
}

inline int KNpc::GetNextAITime() const
{
	return m_NextAITime;
}

inline void KNpc::SetNextAITime(int nextAITime)
{
	m_NextAITime = nextAITime;
}

inline NpcController& KNpc::GetController()
{
	return m_Controller;
}

inline int KNpc::GetActionTime() const
{
	return m_ActionTime;
}

inline enumNpcLoadSaveState KNpc::GetLoadSaveState() const
{
	return m_LoadSaveState;
}

inline void KNpc::SetLoadSaveState(enumNpcLoadSaveState state)
{
	m_LoadSaveState = state;
}

inline bool KNpc::IsVisibleToNpc() const
{
	return GetVisibleToNpcCount() > 0;
}

inline void KNpc::SetVisibleToNpcCount(int visibleCount)
{
	m_VisibleToNpcCount = visibleCount;
}

inline int KNpc::GetVisibleToNpcCount() const
{
	return m_VisibleToNpcCount;
}

inline bool KNpc::IsVisibleToPlayer() const
{
	return GetVisibleToPlayerCount() > 0;
}

inline int KNpc::GetVisibleToPlayerCount() const
{
	return m_VisibleToPlayerCount;
}

#else

inline ClientTalismanNpcController& KNpc::GetTalismanNpcController()
{
	return m_TalismanNpcController;
}

inline ClientCombatInfoShower& KNpc::GetCombatInfoShower()
{
	return m_CombatInfoShower;
}

inline void KNpc::SetCityId(int cityId)
{
	m_CityId = cityId;
}

inline int KNpc::GetCityId()
{
	return m_CityId;
}

inline bool KNpc::IsKing() const
{
	return  m_IsKing;
}

inline bool KNpc::IsGensMgr()const
{
    return m_IsGensMgr;
}

inline void KNpc::SetKing(bool isKing)
{
	m_IsKing = isKing;
}

inline void KNpc::SetGensMgr(bool isGensMgr)
{
	m_IsGensMgr = isGensMgr;
}

inline void KNpc::SetLeaguer(bool isLeaguer)
{
	m_IsLeaguer = isLeaguer;
}
inline void KNpc::SetZhuhouName(const char* pName)
{
	m_ZhuhouName[0] = 0;
	m_bHeadInfoChanged = true;

	if ( pName == NULL )
	{
		return;
	}
	if ( strcmp( pName, m_ZhuhouName) == 0 || pName[0] == 0 )
	{
		return;
	}

	if (pName != NULL)
		strcpy(m_ZhuhouName, pName);
	m_ZhuhouName[sizeof(m_ZhuhouName) - 1] = 0;
}

inline void KNpc::SetLeagueName(const char * pLeagueName)
{
	if (pLeagueName == NULL)
		return ;

	m_LeagueName[0] = 0;
	
	if (pLeagueName != NULL)
		strcpy(m_LeagueName, pLeagueName);

	m_LeagueName[sizeof(m_LeagueName) - 1] = 0;

}

inline void KNpc::SetCityName(const char* pName)
{
	m_CityName[0] = 0;
	m_bHeadInfoChanged = true;

	if ( pName == NULL )
	{
		return;
	}//endif

	char szBuff[32];
	sprintf( szBuff,"/Spr/%s.spr", pName );
	szBuff[31] = 0;
	if ( strcmp( m_CityName, szBuff ) == 0 || pName[0] == 0 )
	{
		return;
	}
	if (pName != NULL)
		sprintf(m_CityName,"/Spr/%s.spr", pName);
	m_CityName[31] = 0;
}

inline void KNpc::SetShizuName(const char* pName)
{
	m_ShizuName[0] = 0;
	m_bHeadInfoChanged = true;

	if ( pName == NULL )
	{
		return;
	}
	if ( strcmp( pName, m_ShizuName) == 0 || pName[0] == 0 )
	{
		return;
	}
	if (pName != NULL)
		strcpy(m_ShizuName, pName);
	m_ShizuName[sizeof(m_ShizuName) - 1] = 0;
}

inline BOOL KNpc::SetEmployerId(int npcId)
{
	if (m_Kind != kind_employee)
		return FALSE;

	m_nPlayerIdx = npcId;
	return TRUE;
}

#endif

#ifdef _SERVER
inline void KNpc::AddCityRes(int nResIdx, int nValue)
{
	if(nResIdx >= 0 && nResIdx < CITY_RES_TYPE_COUNT )
	{
		int nType = nResIdx + nuai_lord_res0;

		int nRealDelta = nValue;
		
		if (nRealDelta == 0)
			return;
			
		int nNewValue = m_UnaryAttrMgr[nType] + nRealDelta ;
		
		//check for int overflow!!
		if (nRealDelta > 0)
		{
			if ( nNewValue <= 0)
				nNewValue = 0x7fffffff; //Max int 
			
		}//endif
		else // nRealDelta < 0
		{
			if ( nNewValue < 0 )
				nNewValue = 0;
		}//end else
		
		m_UnaryAttrMgr.Set(nType, nNewValue);
		SetDataChangedFlag(true);
	}	
}
#endif

#ifdef _SERVER
inline void KNpc::SetExpire(unsigned long expireUnixTimestamp)
{
	m_ExpireTime = expireUnixTimestamp;
}
#endif

#ifdef _SERVER
inline int KNpc::GetSyncToWorldMode() const
{
	return m_SyncToWorldMode;
}

inline void KNpc::SetSyncToWorldParam(int paramIndex, int paramValue)
{
	if (paramIndex >= 0 && paramIndex < MAX_SYNC_TO_WORLD_PARAM_COUNT)
	{
		m_SyncToWorldParams[paramIndex] = paramValue;
	}
}

inline int KNpc::GetSyncToWorldParam(int paramIndex) const
{
	if (paramIndex >= 0 && paramIndex < MAX_SYNC_TO_WORLD_PARAM_COUNT)
	{
		return m_SyncToWorldParams[paramIndex];
	}

	return 0;
}
#endif

#ifdef _SERVER
inline bool KNpc::IsCastingSkill() const
{
	return (do_attack == m_Doing || do_magic == m_Doing || do_summonskill == m_Doing);
}
#endif

#ifdef _SERVER
inline int KNpc::GetCustomVariable(int varIndex) const
{
	if (varIndex >= 0 && varIndex < MAX_NPC_CUSTOM_VARIABLE_COUNT)
		return m_CustomVariable[varIndex];
	else
		return 0;
}

inline void KNpc::SetCustomVariable(int varIndex, int varValue)
{
	if (varIndex >= 0 && varIndex < MAX_NPC_CUSTOM_VARIABLE_COUNT)
		m_CustomVariable[varIndex] = varValue;
}
#endif

#ifndef _SERVER
inline void KNpc::SetTitle(int titleIndex, int titleLevel)
{
	if (m_TitleIndex != titleIndex || m_TitleLevel != titleLevel)
		m_bHeadInfoChanged = true;

	m_TitleIndex = titleIndex;
	m_TitleLevel = titleLevel;
}

inline void KNpc::GetTitle(int& titleIndex, int& titleLevel) const
{
	titleIndex = m_TitleIndex;
	titleLevel = m_TitleLevel;
}
#endif

extern KNpc* Npc;

//判断是否是有效的NpcIndex，推荐对每个不确定的NpcIndex进行判断
inline bool IsValidNpc(int npcIndex)
{
	return (npcIndex > 0 && npcIndex < MAX_NPC && Npc[npcIndex].IsValid());
};

//////////////////////////////////////////////////////////////////////////
// 变身设定表,变身只起到更改外观的作用
#define PolyMorphSettings PolyMorphSetting::GetInstance()
class PolyMorphSetting
{
public:
	static inline PolyMorphSetting& GetInstance(void) { return s_Self; }	
public:
	struct PolyMorphType
	{
		int nType;	// 变身类型大于
		int nPartNo[itempart_num]; // 变身部件编号
	};
public:
	unsigned int GetCount(void) { return m_uCount; }
	const PolyMorphType& operator[](unsigned int uIndex)
	{
		_ASSERT(uIndex < m_uCount);
		return m_pSetting[uIndex];
	}
public:
	BOOL Init(void);
	BOOL Release(void);
private:
	PolyMorphSetting()	{}//Init();} 
public:
	~PolyMorphSetting() { Release(); }
private:
	PolyMorphType* m_pSetting;
	unsigned int m_uCount;
private:
	static PolyMorphSetting s_Self;
};

#ifndef _SERVER
#define  MAX_EMOTE_COUNT 32

enum EmoteType
{
	task_0,
	task_1,
	task_2,
	task_3,
};

class KEmoteImage
{
public:
	KEmoteImage(){}
	~KEmoteImage(){}

public:
	BOOL			Load	( void				);
	bool			GetImage( EmoteType type, KUiImageRef& rImage );	

private:
	void			AddImage( int type );

private:
	KIniFile	m_IniFile;
	KUiImageRef	m_EmoteImage[MAX_EMOTE_COUNT];
};

extern KEmoteImage g_EmoteImage;
#endif


#endif

