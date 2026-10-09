//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 10/24/2007 14:00
//      File_base        : KSkills
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KSkillsH
#define KSkillsH

#include "KPlayer.h"
#include "KNode.h"
#include "SkillDef.h"
#include "KMath.h"
#include "KNpcSet.h"
#include "buff_man.h"
#ifndef _SERVER
#include "KMissle.h"
#endif

class KMissle;
class KRegion;

enum
{
	Missle_StartEvent = 1,
	Missle_FlyEvent,
	Missle_CollideEvent,
	Missle_VanishEvent,
};

class KSkill
{
#ifndef _SERVER
	friend	class KMissle;
#endif

public:
	KSkill( void );
	~KSkill( void );
public:
	inline void 			SetCurLevel( int level ) { m_ulLevel = level; }
	inline int				GetCurLevel( void ) { return m_ulLevel; }
	inline int				GetAttackTargetType( void ) const { return m_AttackTargetType; };
	SkillType				GetSkillType( void ) const;											//得到技能类型（-1:敌对；0:中立；1:友善）
	int						GetThreat( void ) const;											//得到技能威胁值
	bool					IsTalismanSkill( void ) const;										//是否是法宝技能
	int						GetCostSkillExp( void ) const;
	inline int				GetHorseLimited( void ) { return m_nHorseLimited; }	
	inline int				GetGroup( void ) { return m_SkillGroup; }
	inline int				GetCategory( int nIndex );
	inline int				GetCastSpeedEnhance( void ) { return m_CastSpeedEnhance; }
	inline SkillDamageInfo*	GetCommonDamageInfo( void ) { return m_DamageInfo; }
	inline int				GetSkillStyle( void ){ return m_eSkillStyle; }
	void					GetInfoFromTabFile( KITabFile* pSkillsSettingFile, int nRow );
	inline int				GetAttackRadius( void ) const{ return m_nAttackRadius; }
	inline int				GetSkillCost( void ) const { return m_nCost; }
	inline int				GetSkillCostType( void )const { return m_nSkillCostType; }
	BOOL					GetItemLimit( int ) const;
	inline int				GetActionType( void )const{ return m_nCharActionId; }
	inline int				GetDelayPerCast( void )const{ return m_nMinTimePerCast;	}
	unsigned int			GetMissleGenerateTime( int nNo ) const;		
	inline int				GetChildSkillNum( void ) const { return m_nChildSkillNum; }
	inline int				GetChildSkillId( void ) const { return m_nChildSkillId;};
	inline int				GetChildSkillLevel( void ) const { return m_nChildSkillLevel; }
	inline int				GetSkillId( void ) const { return m_nId; };
	inline void				SetSkillId( int nId ) { m_nId = nId; };		
	inline BOOL				IsPhysical( void ) const { return m_bIsPhysical; };
	inline int				GetWeaponSkill( void ) const { return m_nWeaponSkill; };
	inline int				GetMeleeType( void ) const { return m_eMisslesForm; };
	inline int				GetEventSkillLevel( void ) const { return m_nEventSkillLevel; }
	inline int				GetStartSkillId( void ) const { return m_nStartSkillId; }
	inline int				GetValue1( void ) const { return m_nValue1; }	
	inline int				GetValue2( void ) const { return m_nValue2; }	
	inline bool				IsCostItem( void ) const { return -1 != m_ItemKeys[0]; }
	inline bool				IsCancelable( void ) const { return (TRUE == m_bCancelable); }
	inline void				GetItemKeys(int* pItemKeys) const;

	BOOL					CanCastSkill(
								int nLauncher,
								int &nParam1,
								int &nParam2 )  const ;

	BOOL					Cast( 
								int nLauncher,   
								int nParam1, 
								int nParam2, 
								int nParam3 = 0, 
								eSkillLauncherType eLauncherType = SKILL_SLT_Npc, 
								DWORD dwParentSkillID = 0, 
								int nPosNpc = -1 /*位置Npc,Buff用*/ );							//发出时调用

	BOOL					CastSummonSkill(
								KNpc &aNpc );

	BOOL					CanTravTo( 
								int nLauncher, 
								int nTarget ) const;

	BOOL					CanTravToTargetPos(
								int nLauncher,
								int nDstX,
								int nDstY ) const;

private:
	BOOL					IsHorseLimitSatisfied(
								int nLauncher ) const; 

	bool					IsNeadCheckDistance( 
								void ) const;

	bool					IsInShootRange(
								int nLauncher,
								int nTargetIdx ) const;

	bool					IsInShootRange(
								int nSrcX,
								int nSrcY,
								int nTargetIdx ) const;

	void					LoadCostItemInfo(
								KITabFile *pSkillsSettingFile,
								int nRow );

	BOOL					CanTravTo( 
								int nSubWorldIdx, 
								int nStartX, 
								int nStartY, 
								int nEndX, 
								int nEndY ) const;

	BOOL					CanTravTo( 
								int nStartX, 
								int nStartY, 
								int nTarget ) const;

	void					LoadSkillDamageInfo(
								KITabFile *pSkillsSettingFile,
								int nRow );
#ifdef _SERVER
public:
	inline void				CostItem( 
								int nNpcIdx );
private:
	void					LoadTargetFilterInfo( 
								KITabFile* pSkillSettingFile,
								int nRow );

	void					LoadBuffInfo( 
								KITabFile* pSkillSettingFile,
								int nRow );

	void					AttackTarget( 
								int nLauncher, 
								int nTargetIdx );

	void					AttackMultiTarget( 
								int nLauncher,
								int nSubWorldIdx,
								int nSrcRgnIdx,
								int nSrcX,
								int nSrcY );

	void					AttackTargetInRegion( 
								int nLauncher, 
								KRegion& CurRegion, 
								int nSrcX,
								int nSrcY,
								int& nLeftTargetNum );

	bool					IsHitTheTarget( 
								int nLauncher,
								int nTargetIdx );

	void					AppendDamage( 
								int nLauncher, 
								int nTargetIdx );

	void					AppendBuff( 
								int nLauncher, 
								int nTargetIdx, 
								int nBuffTarget );

	void					CalcDamage(	
								int* pNpcDamage,
								int* pNpcDefend, 
								SkillDamageInfo* pSkillPrivateDam, 
								OutputDamageInfo* pRst );
#else	
public:
	inline ActionType		GeteActionType( void ) const;
	inline int				GeteActionBeginFrame( void ) const;
	inline int				GeteActionEndFrame( void ) const;

	BOOL					OnMissleEvent(
								unsigned short usEvent,
								KMissle* pMissle, 
								int nTargetIdx = -1 ) const;

	void					PlayPreCastSound(
								BOOL bIsFeMale,
								int nX,
								int nY ) const;

	inline eSkillLRInfo		GetSkillLRInfo( 
								void ) const;

	inline const char*		GetPreCastEffectFile( 
								void ) const;

	inline const char*		GetPreCastSoundFile(
								BOOL bIsFeMale ) const;

private:
	void					Vanish(	
								KMissle* pMissle) const;

	void					FlyEvent( 
								KMissle* pMissle)  const;

	void					Collidsion( 
								KMissle* pMissle,
								int nTargetIdx ) const;
	
	int						Param2PCoordinate(
								int nLauncher,
								int nParam1,
								int nParam2,
								int* npPX,
								int* npPY,
								eSkillLauncherType eLauncherType = SKILL_SLT_Npc ) const;

	void					CreateMissle( 
								int nLauncher,
								int ChildSkillId,
								int nMissleIndex,
								int nDesPosX = 0,
								int nDesPoxY = 0 ) const;

	BOOL					CastMissles( 
								int nLauncher,
								int nParam1,
								int nParam2,
								int nParam3 = 0, 
								eSkillLauncherType eLauncherType = SKILL_SLT_Npc, 
								BOOL bUseSpecialCode = FALSE,
								DWORD dwParentSkillID = 0 ) const ;

	int						CastSpread( 
								TOrdinSkillParam* pSkillParam,
								int nDir,
								int nRefPX,
								int nRefPY,
								int nDesPX = 0,
								int nDesPY = 0 ) const;

	int						CastCircle(
								TOrdinSkillParam* pSkillParam,
								int nDir,
								int nRefPX,
								int nRefPY,
								DWORD dwParentSkillID ) const;

	int						CastZone( 
								TOrdinSkillParam* pSkillParam, 
								int nDir, 
								int nRefPX, 
								int nRefPY, 
								BOOL bMustAttack = FALSE, 
								int nTargetIndex = 0 ) const;

	int						CastExtractiveLineMissle( 
								TOrdinSkillParam* pSkillParam,
								int nDir,
								int nSrcX,
								int nSrcY,
								int nXOffset,
								int nYOffset,
								int nDesX, 
								int nDesY ) const;

	int						CastWall(
								TOrdinSkillParam* pSkillParam, 
								int nDir, 
								int nRefPX, 
								int nRefPY ) const;

	int						CastLine( 
								TOrdinSkillParam* pSkillParam, 
								int nDir, 
								int nRefPX, 
								int nRefPY ) const;
#endif
private:	
	//从表格读取的数据
	eSKillStyle				m_eSkillStyle;							// 当前的技能类型	
	BOOL					m_bClientSend;							// 该技能是否对服务器来说有效
	int						m_nInteruptTypeWhenMove;				// 子弹的激活是否受发送者的移动而中止
	BOOL					m_bHeelAtParent;						// 当子弹实际激活时，位置根据父当前位置而确定,而不是由产生那刻parent位置决定
	int						m_nCharActionId;						// 发这个技能时人物做什么动作
	int						m_nWaitTime;							// 该技能正常情况下真正产生的时间
	BOOL					m_bByMissle;							// 当由父技能产生时，是否是根据玩家为基点还是以当前的子弹为基点
	BOOL					m_bIsPhysical;							// 是否为物理技能
	int						m_nWeaponSkill;							// 是否为武器对应的技能，这类技能不在技能列表中显示，属于基本必备技能
	int						m_nStartSkillId;						// 技能刚刚才发出时所引发的事件时，所需要的相关技能id
	int						m_nCost;								// 技能使用时所需要花费的内力、体力、精力、金钱的类型
	int						m_nSkillCostType;						// 发该技能所需的内力、体力等的消耗
	int						m_nChildSkillNum;						// 同时发射子技能的数量
	int						m_nMinTimePerCast;						// 发该技能的最小间阁时间
	eMisslesForm			m_eMisslesForm;							// 多个子弹的起始格式
	int						m_nValue1;								// 附加整形数据1
	int						m_nValue2;								// 附加整形数据2
	int						m_nEventSkillLevel;
	int						m_nChildSkillId;						//	技能引发的子技能Id;	//当该技能为基本技能时，这项无用
	int						m_nChildSkillLevel;
	eMisslesGenerateStyle	m_eMisslesGenerateStyle;				// 同时生成的多个子弹，DoWait的时间顺序
	int						m_nMisslesGenerateData;					// 相关数据
	int						m_nAttackRadius;						// 射程
	int						m_nHorseLimited;						// 骑马限制 0表示没任何限制 1表示不能骑马 2表示必须骑马
	int						m_nDoHurt;								// 让对方后仰的概率
	eRelativePosType		m_eMissleRelativePosType;				// 子弹移动的位置参考
	int						m_AttackTargetType;
	int						m_CreatureType;							// 召唤兽外观
	int						m_TravBarrierInfo;						// 技能穿透阻挡信息
	int						m_SkillGroup;
	int						m_SkillCategory[CATEGORY_COUNT];
	int						m_AppendExplodeProb;					// 附加爆击概率
	int						m_AppendExplodeDamage;					// 附加爆击伤害
	int						m_CastSpeedEnhance;	
	int						m_MostAttackNum;						// 群攻技能最多打几个目标
	int						m_ItemKeys[ITEM_KEY_NUM + 1];			// 技能消耗的物品的各项ID及数量
	BOOL					m_bCheckTeammate;
	int						m_TeammateNum;
	int						m_SkillType;							// 类型（-1:敌对；0:中立；1:友善）
	int						m_Threat;								// 威胁值
	int						m_nCostSkillExp;						// 技能所消耗的蕴魂值
	BOOL					m_IsTalismanSkill;						// 是否是法宝技能
	BOOL					m_bCancelable;							// 是否可被打断
#ifdef _SERVER
	BOOL					m_bUseAttackRate;						// 是否考虑命中率
	BOOL					m_bAddBaseDamage;						// 是否附加Npc的伤害
	int						m_TrapId;								// 陷阱ID
	TargetFilterInfo		m_TargetFilterInfo;						// 目标过滤函数及参数
	BuffAndProb				m_IncorelatedBuff[MAX_BUFF_PER_SKILL];	// 概率不相关Buff
	BuffAndProb				m_CorelatedBuff[MAX_BUFF_PER_SKILL];	// 概率相关Buff
	WORD					m_IncorBuffNum;
	WORD					m_CorBuffNum;
#else
public:
	int						m_nDisplayID;
	eSkillLRInfo			m_eLRSkillInfo;							// 0表示左右键皆可，1表示只可以作右键技能，2表示左右键都不可作
	char					m_szPreCastEffectFile[100];
	char					m_szManPreCastSoundFile[100];
	char					m_szFMPreCastSoundFile[100];
	char					m_szName[MAXSIZE_SKILLNAME];			// 技能名称
	char					m_szSkillIcon[MAXSIZE_SKILLNAME];
	char					m_szDesc[COMMON_CLIENT_MSG_LEN_512];
	eMissleMoveKind			m_eMissleMoveKind;
	int						m_MissleLifeTime;
	int						m_MissleSpeed;
	int						m_MissleHeight;
private:
	BOOL					m_bBaseSkill;							// 是否为最基本技能
	BOOL					m_bFlyingEvent;							// 是否需要在飞行过程消息发生是，调用相关回调函数
	BOOL					m_bStartEvent;							// 是否需要在技能第一次Active时，调用相关回调函数
	BOOL					m_bCollideEvent;						// 是否需要在子技能魔法碰撞时，调用相关回调函数
	BOOL					m_bVanishedEvent;						// 是否需要在子技能消亡时，调用相关的回调函数
	int						m_nFlySkillId;							// 整 个飞行的相关技能
	int						m_nFlyEventTime;						// 每多少帧回调FlyEvent;
	int						m_nVanishedSkillId;						// 技能发出的子弹结束时引发的技能Id;
	int						m_nCollideSkillId;						// 技能发出的子弹碰撞到物件引发的技能Id;
	ActionType				m_eActionType;
	int						m_nBeginFrame;
	int						m_nEndFrame;
#endif
	//动态变化的数据
private:
	unsigned long			m_ulLevel;
	int						m_eRelation;
	DWORD					m_nId;									// 技能Id
	SkillDamageInfo			m_DamageInfo[dot_end];
	WORD					m_DamageNum;
#ifdef _SERVER
	int						m_AttackDir;							// 技能攻击方向
#endif
};

inline int KSkill::GetCategory( int nIndex )
{
	if( nIndex >= 0 && nIndex < CATEGORY_COUNT )
		return m_SkillCategory[nIndex];
	else
		return 0;
}

inline bool KSkill::IsNeadCheckDistance() const
{
	return m_nAttackRadius  > 0;
}

inline SkillType KSkill::GetSkillType() const
{	
	return	(SkillType)m_SkillType;
}

inline int KSkill::GetThreat() const
{
	return m_Threat;
}

inline bool KSkill::IsTalismanSkill() const
{
	return (TRUE == m_IsTalismanSkill);
}

inline int KSkill::GetCostSkillExp() const
{
	return m_nCostSkillExp;
}

inline bool KSkill::IsInShootRange(int nLauncher, int nTargetIdx) const
{
	if(nLauncher == nTargetIdx || 0 == m_nAttackRadius)
		return true;

	int nDisSquare = NpcSet.GetDistanceSquare(nLauncher, nTargetIdx);

#ifdef _SERVER
	//服务器适当放宽对技能施放距离的限制，缓解由于同步导致的技能施放不流畅问题
	return (-1 != nDisSquare) && (nDisSquare - 900 <= m_nAttackRadius * m_nAttackRadius);
#else
	return (-1 != nDisSquare) && (nDisSquare <= m_nAttackRadius * m_nAttackRadius);
#endif
}

inline bool KSkill::IsInShootRange(int nSrcX, int nSrcY, int nTargetIdx) const
{
	if(0 == m_nAttackRadius)
		return true;

	int nDisSquare = NpcSet.GetDistanceSquare(nSrcX, nSrcY, nTargetIdx);
		
#ifdef _SERVER
	//服务器适当放宽对技能施放距离的限制，缓解由于同步导致的技能施放不流畅问题
	return (-1 != nDisSquare) && (nDisSquare - 900 <= m_nAttackRadius * m_nAttackRadius);
#else
	return (-1 != nDisSquare) && (nDisSquare <= m_nAttackRadius * m_nAttackRadius);
#endif
}

void KSkill::GetItemKeys(int* pItemKeys) const
{
	if (pItemKeys)
		memcpy(pItemKeys, m_ItemKeys, sizeof(m_ItemKeys));
}

#ifdef _SERVER

inline void	KSkill::CostItem( int nNpcIdx )
{
	int	nPlayerIdx = Npc[nNpcIdx].GetPlayerIdx();

	ItemType it;
	it.genre		= m_ItemKeys[0];
	it.detail		= m_ItemKeys[1];
	it.particular	= m_ItemKeys[2];
	it.level		= m_ItemKeys[3];

	Player[nPlayerIdx].m_ItemList.removeItemOfTypeCount( pos_equiproom, it, m_ItemKeys[ITEM_KEY_NUM] );
}

inline bool KSkill::IsHitTheTarget( int nLauncher, int nTargetIdx )
{
	if(m_bUseAttackRate)
	{
		int nAttackRate = Npc[nLauncher].CalcVision() - Npc[nTargetIdx].CalcDexterity();
		nAttackRate = nAttackRate * 5 / 100;
		nAttackRate += 90;

		if ( (Npc[nLauncher].m_Kind == kind_player || 
			Npc[nLauncher].m_Kind == kind_creature || Npc[nLauncher].m_Kind == kind_employee) && 
			(Npc[nTargetIdx].m_Kind != kind_player &&
			Npc[nTargetIdx].m_Kind != kind_creature && Npc[nTargetIdx].m_Kind != kind_employee) )
		{		
			int nleveldifferent = 0, nFlevel = 0, a1 = 0, a2 = 0;
			nleveldifferent = Npc[nTargetIdx].m_Level - Npc[nLauncher].m_Level;
			nFlevel = ConfigManager::Singleton().GetGlobalVariable( global_var_npc_level_different );
			a1 =  ConfigManager::Singleton().GetGlobalVariable( global_var_npc_level_hit_value1 );
			a2 =  ConfigManager::Singleton().GetGlobalVariable( global_var_npc_level_hit_value2 );
			if ( nleveldifferent > nFlevel )
			{
				nAttackRate -= ((nleveldifferent - nFlevel) * a1 + a2);
			}
		}

		if( !g_RandPercent(nAttackRate) )
			return false;
	}	

	return true;
}

#else

inline ActionType KSkill::GeteActionType( void ) const 
{
	return m_eActionType; 
}

inline int KSkill::GeteActionBeginFrame( void ) const 
{
	return m_nBeginFrame; 
}

inline int KSkill::GeteActionEndFrame( void ) const 
{
	return m_nEndFrame; 
}

inline eSkillLRInfo	KSkill::GetSkillLRInfo( void ) const
{
	return m_eLRSkillInfo; 
}

inline const char*	KSkill::GetPreCastEffectFile( void ) const
{
	return m_szPreCastEffectFile; 
}

inline const char*	KSkill::GetPreCastSoundFile( BOOL bIsFeMale ) const
{
	return bIsFeMale? m_szFMPreCastSoundFile: m_szManPreCastSoundFile;
}

#endif

#endif





















