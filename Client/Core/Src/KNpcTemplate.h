#ifndef	_KNPCTEMPLATE_H
#define	_KNPCTEMPLATE_H

#define		TMPL_NPC_SKILL_NUM	4

#include "KNpc.h"

/************************************************************************/
/*							DropRate Config		                        */
/************************************************************************/
#ifdef _SERVER
#include "KBinsTree.h"

struct KItemDropRateNode
{
	KItemDropRateNode( void )
	{
		memset( this, 0, sizeof(KItemDropRateNode) );
	}
	~KItemDropRateNode( void )
	{
	}
	//默认掉落表
	KItemDropRate*	m_pItemDropRate;
	char			m_szFileName[COMMON_CLIENT_MSG_LEN_256];
	//掉落分组表
	KItemDropRate*	m_pItemDropRateGroup[MAX_DROP_GROUP];
	char			m_szFileNameGroup[MAX_DROP_GROUP][COMMON_CLIENT_MSG_LEN_256];
};

typedef	BinSTree<KItemDropRateNode> KItemDropRateTree;

extern KItemDropRateTree g_ItemDropRateBinTree;

KItemDropRate* g_GenItemDropRate( char* szDropIniFile );
#endif

/************************************************************************/
/*							Npc Template                                */
/************************************************************************/


#define MAX_NPC_EVENT_BUFF_COUNT 5

#ifdef _SERVER
#include "npc_statistic.h"

//NPC事件激发的BUFF
typedef struct tagNpcEventBuff
{
	tagNpcEventBuff()
	{
		memset(BuffID, 0, sizeof(BuffID));
	}

	int BuffID[NpcEvent_Count][MAX_NPC_EVENT_BUFF_COUNT];
} NpcEventBuff;
#endif

class KNpcTemplate
{
public:

#ifdef _SERVER
	enumNpcState GetInitState() const;//得到初始状态
	int GetSkillStrategyCount() const;//得到技能策略计数
	const PSkillStrategy GetSkillStrategyList();//得到技能策略列表
	bool NeedSave() const;//是否需要存盘
	const FSGUID& GetGUID() const;//得到GUID
	NpcStatistic& GetStatisticInfo();//得到统计信息
#endif

public:
	bool	m_bDisplaySelect;
	bool	m_bShowTargetFace;
	char	Name[32];
	DWORD	m_Kind;
	int		m_Camp;
	int		m_Series;
	char	m_HeadImage[128];
	char	m_HeadImageSet[128];
	int		m_bClientOnly;
// lixuewu 增加阻挡信息
	unsigned int m_nBarrierWidth;
	unsigned int m_nBarrierHeight;
// lixuewu 2004.03.11 咱们不要尸体了
	int		m_CorpseSettingIdx;
// lixuewu 2004.03.11 咱们不要尸体了
	int		m_DeathFrame;
	int		m_WalkFrame;
	int		m_StandFrame;
	int		m_StandFrame1;
	int		m_RunFrame;
	int		m_HurtFrame;
	int		m_WalkSpeed;
	int		m_AttackFrame;
	int		m_CastFrame;
	int		m_RunSpeed;
	int		m_LifeMax;
	int     m_CombatOrg;
	int     m_CombatScore;

	// add by chenshanglin on 2006-2-23 for new skill system
#ifdef _SERVER
	int		m_nDropRateAttenuation;		// NPC是否受掉装衰减，0不受影响
	int     m_nLifeLimitedHold;         // Npc需要保持的最少血量(不死)
#endif
	// add end

	int		m_AiMode;
	int		m_AiParam[MAX_AI_PARAM];
#ifdef _SERVER	
	DWORD	m_dwLevelSettingScript;
	int		m_Treasure;
	// Add by Cooler 2004-7-23
	// Begin -->
	int		m_Treasure1;
	// End <--

	int		m_NearPhysDamLow;
	int		m_NearPhysDamHight;
	int		m_FarPhysDamLow;
	int		m_FarPhysDamHight;
	int		m_WaterDamLow;
	int		m_WaterDamHight;
	int		m_FireDamLow;
	int		m_FireDamHight;
	int		m_ThunderDamLow;
	int		m_ThunderDamHight;
	int		m_WindDamLow;
	int		m_WindDamHight;
	int		m_ShadowDamLow;
	int		m_ShadowDamHight;
	int		m_PoisonDamLow;
	int		m_PoisonDamHight;

	int		m_FarPhysResistLow;
	int		m_FarPhysResistHight;
	int		m_NearPhysResistLow;
	int		m_NearPhysResistHight;
	int		m_WaterResistLow;
	int		m_WaterResistHight;
	int		m_FireResistLow;
	int		m_FireResistHight;
	int		m_ThunderResistLow;
	int		m_ThunderResistHight;
	int		m_WindResistLow;
	int		m_WindResistHight;
	int		m_ShadowResistLow;
	int		m_ShadowResistHight;
	int		m_PoisonResistLow;
	int		m_PoisonResistHight;

	int		m_ActiveRadius;
	int		m_VisionRadius;
	BYTE	m_AIMAXTime;
	int		m_HitRecover;
	int		m_ReviveFrame;

	int		m_Experience;
	int		m_SkillExp;
	int		m_LifeReplenish;
	int		m_AttackRating;
	int		m_Defend;
	int		m_RedLum;
	int		m_GreenLum;
	int		m_BlueLum;
	NpcSkillList m_SkillList;
	KItemDropRate *m_pItemDropRate;
	KItemDropRate *m_pItemDropGroupRate[MAX_DROP_GROUP];
	
	DWORD	m_dwDeathScriptID;
	DWORD	m_dwActionScriptID;
	DWORD	m_dwActionTime;
	DWORD	m_dwBreakTime;
	

	
    // add by hejianfeng.  2005-10-24
    int     m_nColor; //怪物预置在npcs.txt中的颜色
    int     m_nDeadlyStrikeResist;
    int     m_nFatallyStrikeResist;
    // endadd
	int		m_nFreezeTimeReduce;
#endif
	
#ifndef _SERVER	
	float	m_nLifeMultiple;
	int     m_nLifeBarStyle;	
	bool	m_bShowLifePercent;
	int		m_ArmorType;
	int		m_ShoulderType;
	int		m_BootType;
	int		m_CuffType;
	int		m_HelmType;
	int		m_WeaponType;
	int		m_HorseType;
	int		m_bRideHorse;
	char	m_szLevelSettingScript[100];
	char	m_szNpcNameColor[16];
	char	m_szNpcBloodInfo[32];
	int		m_nMapID[6];
	char	m_nMapPos[6][COMMON_CLIENT_MSG_LEN_64];
	int		m_nDisplayID;
	char	m_nDes[COMMON_CLIENT_MSG_LEN_16];
	// --> Rocker Edit Start 2005/12/10 攻击距离
	int		m_AttackRadius[TMPL_NPC_SKILL_NUM];
	// <-- Rocker End

#endif
	
	int		m_NpcSettingIdx;
	BOOL	m_bHaveLoadedFromTemplate;
	int		m_nStature;	
	int		m_nLevel;
#ifdef _SERVER
	NpcEventBuff m_EventBuff;//事件BUFF
	int m_SkillStrategyCount;//技能策略计数
	SkillStrategy m_SkillStrategyList[MAX_SKILL_STRATEGY_LIST_LENGTH];//技能策略列表
	FSGUID m_GUID;//GUID
	NpcStatistic m_Statistic;//统计信息
#endif	
	
public:
	void	InitNpcBaseData(int nNpcTemplateId);
	void	InitNpcLevelData(KTabFile * pKindFile, int nNpcTemplateId, KLuaScript * pLevelScript, int nLevel);
	int		GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, char * szParam) const;
	static int		ST_GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, char * szParam) ;
	int		GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, double nParam1, double nParam2, double nParam3) const;
	static	int		SkillString2Id(char * szSkillString);
	KNpcTemplate(){	m_bHaveLoadedFromTemplate = FALSE;};

protected:
	int		LoadLevelData(int nRow, char *szCol, KLuaScript *pLevelScript, char *szKey, int nLevel);
};

extern KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL]; //0,0为起点

#endif

#ifdef _SERVER

inline int KNpcTemplate::GetSkillStrategyCount() const
{
	return m_SkillStrategyCount;
}

inline const PSkillStrategy KNpcTemplate::GetSkillStrategyList()
{
	return m_SkillStrategyList;
}

inline bool KNpcTemplate::NeedSave() const
{
	return (m_GUID.data[0] == 0) ? false : true;
}

inline const FSGUID& KNpcTemplate::GetGUID() const
{
	return m_GUID;
}

inline NpcStatistic& KNpcTemplate::GetStatisticInfo()
{
	return m_Statistic;
}

#endif
