//-----------------------------------------------------------------------
//	Sword3 KNpc.cpp
//-----------------------------------------------------------------------
#include "KCore.h"
#include "KNpcAI.h"
#include "KObj.h"
#include "KObjSet.h"
#include "KMath.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "GameDataDef.h"
#include "KSubWorldSet.h"
#include "KRegion.h"
#include "KNpcTemplate.h"
#include "KItemSet.h"
#include "SkillDef.h"
#include "KSkills.h"

#ifdef _SERVER
#include "KPlayerSet.h"
#include "KMath.h"
#include "ChatCenter_S.h"
#else
#include "networkinterface.h"
#include "CoreShell.h"
#include "Scene/KScenePlaceC.h"
#include "KIme.h"
#include "iRepresentshell.h"
#include "ImgRef.h"
#include "Text.h"
#endif
#include "CoreUseNameDef.h"
#include "KSubWorld.h"
#include "Scene/ObstacleDef.h"
#include "KLevelUp.h"

#ifndef _SERVER
#include "ai_player_controller.h"
#include "CoreDrawGameObj.h"
#include "KOption.h"
#include "KClientFuryMgr.h"
#endif

#include "CoreUtil.h"
#include "buff_tab.h"
#include "ConfigManager.h"
#include "ChatDataDef.h"

#ifdef _SERVER
#include "exp_manager.h"
#include "npc_save.h"
#include "SocialUnit.h"
#include "SocialUtil.h"
#include "ServerSocialUnitMgr.h"
#include "KSortScript.h"
#include "player_monitor.h"
#include "KWarInfoManager.h"
#endif

#ifndef _SERVER
#include "AutoRobotMgr.h"
//#include "KMessageCentre.h"
#endif

#ifdef _SERVER
#include "PlayerCreator.h"
#endif

#include "KItemChangeRes.h"

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

extern KLuaScript		*g_pNpcLevelScript;

#define	ATTACKACTION_EFFECT_PERCENT		60	// 发技能动作完成百分之多少才真正发出来
#define	MIN_JUMP_RANGE					20
#define	ACCELERATION_OF_GRAVITY			10

#define		SHOW_CHAT_WIDTH				24
#define		SHOW_CHAT_COLOR				0xffffffff
#define		SHOW_BLOOD_COLOR			0x00ff0000


#define		SHOW_LIFE_WIDTH				38
#define		SHOW_LIFE_HEIGHT			3

#define		SHOW_SPACE_HEIGHT			5

//-----------------------------------------------------------------------
// lixuewu 修改恢复计时 2004.07.19
#define	GAME_UPDATE_TIME		10 // 9
#define GAME_UPDATE_TIME_SIT	(GAME_UPDATE_TIME * 2) // 打坐恢复时间
#define GAME_REPLENIS_TIME		(GAME_UPDATE_TIME * 10) // 自然恢复时间		

#define GAME_ENHANCE_UPDATE_TIME GAME_FPS

#define	GAME_SYNC_LOSS			100
#define	STAMINA_RECOVER_SCALE	4
// 区域的宽高（格子单位）
#define	REGIONWIDTH			REGION_CELL_WIDTH //SubWorld[m_SubWorldIndex].m_nRegionWidth
#define	REGIONHEIGHT		REGION_CELL_HEIGHT //SubWorld[m_SubWorldIndex].m_nRegionHeight
// 格子的宽高（像素单位，放大了1024倍）
#define	CELLWIDTH			(REGION_CELL_SIZE_X << 10) //(SubWorld[m_SubWorldIndex].m_nCellWidth << 10)
#define	CELLHEIGHT			(REGION_CELL_SIZE_Y << 10) //(SubWorld[m_SubWorldIndex].m_nCellHeight << 10)
// 当前区域
#define	CURREGION			SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex]
// 相邻区域的索引
#define	LEFTREGIONIDX		CURREGION.m_nConnectRegion[2]
#define	RIGHTREGIONIDX		CURREGION.m_nConnectRegion[6]
#define	UPREGIONIDX			CURREGION.m_nConnectRegion[4]
#define	DOWNREGIONIDX		CURREGION.m_nConnectRegion[0]
#define	LEFTUPREGIONIDX		CURREGION.m_nConnectRegion[3]
#define	LEFTDOWNREGIONIDX	CURREGION.m_nConnectRegion[1]
#define	RIGHTUPREGIONIDX	CURREGION.m_nConnectRegion[5]
#define	RIGHTDOWNREGIONIDX	CURREGION.m_nConnectRegion[7]

#define	LEFTREGION			SubWorld[m_SubWorldIndex].m_Region[LEFTREGIONIDX]
#define	RIGHTREGION			SubWorld[m_SubWorldIndex].m_Region[RIGHTREGIONIDX]
#define	UPREGION			SubWorld[m_SubWorldIndex].m_Region[UPREGIONIDX]
#define	DOWNREGION			SubWorld[m_SubWorldIndex].m_Region[DOWNREGIONIDX]
#define	LEFTUPREGION		SubWorld[m_SubWorldIndex].m_Region[LEFTUPREGIONIDX]
#define	LEFTDOWNREGION		SubWorld[m_SubWorldIndex].m_Region[LEFTDOWNREGIONIDX]
#define	RIGHTUPREGION		SubWorld[m_SubWorldIndex].m_Region[RIGHTUPREGIONIDX]
#define	RIGHTDOWNREGION		SubWorld[m_SubWorldIndex].m_Region[RIGHTDOWNREGIONIDX]

#define	CONREGION(x)		SubWorld[m_SubWorldIndex].m_Region[CURREGION.m_nConnectRegion[x]]
#define	CONREGIONIDX(x)		CURREGION.m_nConnectRegion[x]

#define BROADCAST_REGION(pBuff,uSize,uMaxCount)	 if(m_SubWorldIndex >= 0 && m_SubWorldIndex < MAX_SUBWORLD && SubWorld[m_SubWorldIndex].m_SubWorldID != -1) SubWorld[m_SubWorldIndex].BroadCastRegion((pBuff), (uSize), (uMaxCount), m_RegionIndex, m_MapX, m_MapY);

#define MIN_NPCSAVE_TIMEINTERNAL	GAME_FPS * 180
#define VAR_NPCSAVE_TIEMINTERNAL	MIN_NPCSAVE_TIMEINTERNAL

#ifndef _SERVER
#define ENTER_ESPECIAL_AREA_NONE		11420
#define ENTER_ESPECIAL_AREA_SAFE		11421
#define ENTER_ESPECIAL_AREA_PK			11422
#define ENTER_ESPECIAL_AREA_QUEST		11423
#define ENTER_ESPECIAL_AREA_WAR			11424
#define ENTER_ESPECIAL_AREA_CITY        11425
#endif

//////////////////////////////////////////////////////////////////////////
// lixuewu 2004.02.05
// 主角名称表
#ifndef _SERVER
KEmoteImage g_EmoteImage;

static const char* NPCTEMPLATEID_TO_ROLENAME[] =
{
	JIASHI_1,
	SHUSHI_1,
	YIREN_1	,
	JIASHI_0,
	SHUSHI_0,
	YIREN_0	,
};

#endif

//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
// Npc[0]不在游戏世界中使用，做为一个NpcSet用于添加新的NPC。
KNpc*	Npc;


KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL]; //0,0为起点

//-----------------------------------------------------------------------

UINT g_nNewRandomSeed = 42;

KNpc::KNpc()
{
	m_AiMode = 0;
	ZeroMemory(m_AiParam, sizeof(m_AiParam));
	m_AiAddLifeTime	= 0;

	m_ActiveSkillID = 0;	

#ifdef _SERVER
	m_AiSkillRadiusLoadFlag = 0;	// 只需要在构造的时候初始化一次
#else
	m_bCanShowUiLoginPlayer = false;
#endif

#ifdef _SERVER
	m_nSaveNpcTimeInternal = MIN_NPCSAVE_TIMEINTERNAL + g_Random(VAR_NPCSAVE_TIEMINTERNAL);
	SetDataChangedFlag(false);
#endif
	
	Init();
}

void KNpc::Init()
{
	m_nItemInlayCount	= 0;
	m_nHeadImage		= -1;
	m_bTheSameTarget	= false;
	m_dwID = 0;
	m_Index = 0;
	m_nPlayerIdx = 0;
	m_ProcessAI = 1;
	m_Kind = kind_normal;
	m_Series = series_metal;
	m_SkillType = -1;
	m_Camp = camp_free;
	m_CurrentCamp = camp_free;
	m_UnaryAttrMgr.Set(nuai_pkmode, pk_peace);
	m_Doing = do_stand;
	ClearMoveStatus();
	m_Height = 0;
	m_Frames.nCurrentFrame = 0;
	m_Frames.nTotalFrame = 0;
	m_SubWorldIndex = 0;
	m_RegionIndex = -1;
	m_SkillParam1 = 0;
	m_SkillParam2 = 0;
	m_SkillParam3 = 0;
	m_SkillParam4 = 0;
	m_UnaryAttrMgr.Set(nuai_curlife, 100);
	m_UnaryAttrMgr.Set(nuai_curmana, 100);
	m_UnaryAttrMgr.Set(nuai_experience, 0);
	m_UnaryAttrMgr.Set(nuai_skillexp, 0);
	for(int i = 0; i < ncai_end; ++i)
	{
		m_CompAttrMgr.Set(i, idx_append_value, 0);
		m_CompAttrMgr.Set(i, idx_append_percent, 0);
	}
	m_CompAttrMgr.Set(ncai_lifeuplimit, idx_base_value, 100);
	m_CompAttrMgr.Set(ncai_manauplimit, idx_base_value, 100);
	m_CompAttrMgr.Set(ncai_liferenewspeed, idx_base_value, 0);
	m_CompAttrMgr.Set(ncai_manarenewspeed, idx_base_value, 0);
	m_CompAttrMgr.Set(ncai_vision, idx_base_value, 100);
	m_CompAttrMgr.Set(ncai_dexterity, idx_base_value, 10);
	m_CompAttrMgr.Set(ncai_walkspeed, idx_base_value, 6);
	m_CompAttrMgr.Set(ncai_runspeed, idx_base_value, 10);
	m_CompAttrMgr.Set(ncai_attackspeed, idx_base_value, 0);
	m_CompAttrMgr.Set(ncai_castspeed, idx_base_value, 0);
	m_CompAttrMgr.Set(ncai_visionradius, idx_base_value, 40);
	m_CompAttrMgr.Set(ncai_attackradius, idx_base_value, 30);
	m_UnaryAttrMgr.Set( nuai_servercont, 0 );
	m_UnaryAttrMgr.Set( nuai_needupdate, 0 );
	m_UnaryAttrMgr.Set( nuai_deathmode, 0 );
	m_UnaryAttrMgr.Set( nuai_dir, 0 );
	m_UnaryAttrMgr.Set( nuai_team_id, 0 );
	m_UnaryAttrMgr.Set( nuai_camou_flage, 0);


	m_MapZ = 0;					// Npc的高度
	m_HelmType = -1;			// Npc的头盔类型
	m_ArmorType = -1;			// Npc的盔甲类型
	m_WeaponType = -1;			// Npc的武器类型
	m_HorseType = -1;			// Npc的骑马类型
	m_ShoulderType = -1;		// Npc的护肩类型
	m_BootType = -1;
	m_CuffType = -1;

	m_HelmPal		= Default_PalIndex;
	m_ArmorPal		= Default_PalIndex;
	m_WeaponPal		= Default_PalIndex;
	m_HorsePal		= Default_PalIndex;
	m_ShoulderPal	= Default_PalIndex;
	m_BootType		= Default_PalIndex;
	m_CuffPal		= Default_PalIndex;

	m_bRideHorse = FALSE;		// Npc是否骑马
	ZeroMemory(Name, 32);		// Npc的名称
	m_NpcSettingIdx = 0;		// Npc的设定文件索引
//	m_CorpseSettingIdx = 0;		// Body的设定文件索引
	m_nBarrierWidth = 0;
	m_nBarrierHeight = 0;
	m_bHaveBarrier = TRUE;
	m_ActionScriptID = 0;
	m_TrapScriptID = 0;
	m_btRankId					= 0;
	m_DialogRadius				= 256;		// Npc的对话范围
	m_nTargetType				= 0;
	m_nTargetIdx				= 0;
	m_LoopFrames				= 0;
	m_WalkFrame					= 12;
	m_RunFrame					= 15;
	m_StandFrame				= 15;
	m_DeathFrame				= 15;
	m_HurtFrame					= 10;
	m_AttackFrame				= GAME_FPS;
	m_CastFrame					= GAME_FPS;
	m_SitFrame					= 15;
	m_AIMAXTime					= 25;
	m_NextAITime				= 0;
	m_ProcessState				= 1;
	m_ReviveFrame				= 100;
	m_bActivateFlag				= FALSE;
	m_nLastPoisonDamageIdx = 0;
	m_nLastDamageIdx = 0;
	m_bHaveLoadedFromTemplate	= FALSE;
	m_bClientOnly = FALSE;
	m_nNextStatePos				= 0;
	ZeroMemory(m_btStateInfo, sizeof(m_btStateInfo));
	m_DesX = 0;
	m_DesY = 0;
	m_nCurrentMeleeSkill = 0;
	m_nCurrentMeleeTime	= 0;
	m_XFactor = 0;
	m_YFactor = 0;
	m_SpecialSkillStep = 0;
	ZeroMemory(&m_SpecialSkillCommand, sizeof(NPC_COMMAND));
	m_btKilledType = 0;
	m_nActiveGreeItemID = -1;
	m_nSetEfficacyType = enSetEfficacy_Default;
	m_nMorphType = -1;
	memset(m_nMorphPart, -1, sizeof(m_nMorphPart));
	m_nSafeGuardLevel = 0;
	m_bCanCast	= TRUE;
	m_uMoveSpeed = 0;
	m_uPolyMorphTime = 0;
	m_bSaveMorphType = TRUE;
	m_bRegionRefAdded = FALSE;
	m_Command.CmdKind =  do_none;
	m_UnaryAttrMgr.Set( nuai_nomove, 0 );
	m_UnaryAttrMgr.Set( nuai_titlecolor, 0xffffffff );
	m_UnaryAttrMgr.Set( nuai_fightstate, false );
	m_pTemplate = NULL;
	m_EquipTalismanNpcId = 0;
	m_WorldCombatOrg     = INVALID_COMBAT_ORG_ID;
	m_WorldCombatKilledScore   = 0;
#ifdef _SERVER
	m_Lord = 0;
	m_Robber = 0;
	m_pSpawnInfo = NULL;
    m_pWorldSpawnInfo = NULL;
	m_pDropRate = NULL;
	memset( m_pDropRateGroup, 0, sizeof(KItemDropRate*) * MAX_DROP_GROUP );
	m_dwDeathScriptID = 0;
	m_nInitColorIdx = 0;
	m_bTransSkill = 0;
    m_nNpcColor = 0;
    m_nDeadlyStrikeResist = 0;
    m_nFatallyStrikeResist = 0;
	m_nFreezeTimeReduce = 0;	
	m_LoadSaveState = npc_load_save_state_loaded;
	m_dwFightStateTimeBegin = 0;;
	m_dwFightStateTimeMax	= 10;//10秒
	m_VisibleToNpcCount = 1;
	m_VisibleToPlayerCount = 1;
	m_ExpireTime = 0;
	m_SyncToWorldMode = 0;
	memset(m_SyncToWorldParams, 0, sizeof(m_SyncToWorldParams));
	memset(m_CustomVariable, 0, sizeof(m_CustomVariable));

	m_UnaryAttrMgr.Set(nuai_city_taxrate, DEFAULT_CITY_DISCOUNT);

	m_LifeLimitedHoldPercentage = -1;
    
	/*
	m_LastRunDX=0;
    m_LastRunDY=0;
    m_LastDxExp2 = 0;
    m_LastDyExp2 =0;
    */
	m_LastEditFrame =0;
    
    #ifdef _DEBUG
	m_ClientCheckedTimes=0;
	m_ServerPassTimes=0;
    #endif

	m_LastDialogPosX      = 0;
	m_LastDialogPosY      = 0;
	m_LastDialogSubWordID = -1;
#else
	m_bShowSelect	= true;
	m_bShowTargetFace = true;
	if ( !m_HeadInfoPlus.empty() )
	{
		m_HeadInfoPlus.clear();
	}	
	m_ulTime = 0;
	m_eActionType = normal_action_type;
	m_nBeginFrame = 0;
	m_nEndFrame = 0;
	memset(&m_UnitOwnerFlag, 0, sizeof(m_UnitOwnerFlag));
	m_bIsPkArea	= especial_area_none - 1;
	m_pBuffSoundNode = NULL;
	m_pBuffWave	= NULL;
	memset( m_RoleInfoRes, 0, sizeof(char*)*role_info_count );
	m_nEffectPolyMorphBuffIdx = 0;
	m_CurrentLifePercentage = 0;
	m_CurrentManaPercentage = 0;
	m_bCanFollowAndAttack = false;
	m_LeftSkillID	= 0;
	m_RightSkillID	= 0;
	m_bIsLeftSkill	= -1;
	m_bBubble		= false;
	m_bHeadInfoChanged = true;
	m_bDoubleExp = false;
	m_byNpcLastInWar = false;
	m_nActiveSkillIdx = 0;
	m_dwPolyMorphTimeStamp = 0;
	m_nHurtHeight				= 0;
	m_nHurtDesX					= 0;
	m_nHurtDesY					= 0;
	m_StandFrame1				= 15;
	m_SyncSignal				= 0;
	m_sClientNpcID.m_dwRegionID	= 0;
	m_sClientNpcID.m_nNo		= -1;
	m_ResDir					= 0;
	m_nPKFlag					= 0;
	m_nSleepFlag				= 0;
	memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
	if ( !m_BuffList_C.empty() )
	{
		m_BuffList_C.clear( );
	}	
	m_nChatContentLen = 0;
	m_nCurChatTime = 0;
	m_nChatNumLine = 0;
	m_nChatFontWidth = 0;
	m_nStature = 0;
//	m_ClientDoing = cdo_stand;
	m_nPetIndex = 0;
	m_byPetHonor = 0;	
	m_nRealPosX = 0;
	m_nRealPosY = 0;
	m_nRealDir = 0;	
	m_TalismanNpcController.Init(0);
	m_nRealDir = 0;
	m_nLastQuestStateQueryTime = -1;
	m_CityId = 0;
	m_IsKing = 0;
	m_IsGensMgr = 0;
	m_IsLeaguer = 0;
	m_HasLeagueLayer = false;
	m_InvaderTongFlag= INVADER_STATE_NULL;

	memset(m_ZhuhouName, 0, sizeof(m_ZhuhouName));
	memset(m_ShizuName, 0, sizeof(m_ShizuName));
	memset(m_CityName, 0, sizeof(m_CityName));
	memset(m_LeagueName,0,sizeof(m_LeagueName));

	m_IsPosEditionActive = false;
	m_IsChangeWorld = false;
	m_TitleIndex = 0;
	m_TitleLevel = 0;
#endif
}

#define		NPC_SHOW_CHAT_TIME		15000
int		IR_IsTimePassed(unsigned int uInterval, unsigned int& uLastTimer);

void KNpc::Activate()
{
	// 不存在这个NPC
	if (!m_Index && m_Index != -1)
		return;
	
#ifdef _SERVER

	//检查是否过期
	if (CheckExpire())
		return;

	if (npc_load_save_state_waiting == m_LoadSaveState)
	{		
		if (m_pTemplate != NULL && m_pTemplate->NeedSave())
		{			
			//进行NPC存盘数据载入
			if (!NpcSave::IsBusy())
			{
				NpcSave::LoadNpc(m_Index);
			}
		}
	}	

	// 保证所有npc数据先load进来后再执行存盘操作
	// 以免存盘操作先于load操作执行，初始的数据覆盖了数据库中数据
	if( (NULL != m_pTemplate) && 
		m_pTemplate->NeedSave() && 
		npc_load_save_state_loaded == m_LoadSaveState)
	{
		if( IsDataHasChanged() )
			Save();
		else if( 0 == (g_SubWorldSet.GetGameTime() % m_nSaveNpcTimeInternal) )
			Save();
	}

	DWORD dwFightStatePassTime = UNIX_TMIE_STAMP - m_dwFightStateTimeBegin;
	if ( (dwFightStatePassTime > m_dwFightStateTimeMax) && m_dwFightStateTimeBegin > 0 )
	{
		SetFightState( false );
	}

#endif

	if (m_bActivateFlag)
	{
		m_bActivateFlag = FALSE;	// restore flag
		return;
	}

	m_LoopFrames++;

	if (m_ProcessAI)
	{		
#ifdef _SERVER
		if (m_Controller.IsActive())
		{
			m_Controller.Active();			
		}
		else
		{
			NpcAI.Activate(m_Index);
		}
#else
		if (m_Kind == kind_pet || m_Kind == kind_player)
		{
			NpcAI.Activate(m_Index);
		}
#endif
	}

	ProcCommand(m_ProcessAI);
	ProcStatus();
	
#ifndef _SERVER 

	if (m_RegionIndex == -1)
		return;
	
	//	HurtAutoMove();	
	int		nMpsX, nMpsY;
	
	// 建筑物自身不会换动作 lixuewu 2004.10.08
	if (/*m_Kind != kind_building*/true)
	{

		if (m_nMorphType < 0)
		{
			m_DataRes.SetPart(BODY_PART_HELM, m_HelmType, m_HelmPal);
			m_DataRes.SetPart(BODY_PART_ARMOR, m_ArmorType, m_ArmorPal);
			m_DataRes.SetPart(BODY_PART_WEAPON, m_WeaponType, m_WeaponPal);
			m_DataRes.SetPart(BODY_PART_SHOULDER, m_ShoulderType, m_ShoulderPal);
			m_DataRes.SetPart(BODY_PART_CUFF, m_CuffType, m_CuffPal);
			m_DataRes.SetPart(BODY_PART_BOOT, m_BootType, m_BootPal);
			m_DataRes.SetPart(BODY_PART_HORSE, m_HorseType, m_HorsePal);
			m_DataRes.SetRideHorse(m_bRideHorse);
		}
		else
		{
			m_DataRes.SetRideHorse(FALSE);
			m_DataRes.SetPart(BODY_PART_HELM, 0, Default_PalIndex);
			m_DataRes.SetPart(BODY_PART_ARMOR, 0, Default_PalIndex);
			m_DataRes.SetPart(BODY_PART_WEAPON, 0, Default_PalIndex);
			m_DataRes.SetPart(BODY_PART_SHOULDER, 0, Default_PalIndex);
			m_DataRes.SetPart(BODY_PART_CUFF, 0, Default_PalIndex);
			m_DataRes.SetPart(BODY_PART_BOOT, 0, Default_PalIndex);
			m_DataRes.SetPart(BODY_PART_HORSE, 0, Default_PalIndex);
		}
		if ( m_Kind == kind_normal && ( m_Doing == do_revive || m_Doing == do_death || GetCurrentLifePercentage() == 0 ))
		{
			m_DataRes.SetAction(cdo_death);
			m_DataRes.UpDateImage();
		}
		else
		{
			m_DataRes.SetAction(m_ClientDoing);
		}		
	}
	
	// 处理npc当前状态的特效
	if ( m_BuffList_C.size() )
	{
		C_BUFFLIST::iterator it;
		for ( it = m_BuffList_C.begin(); it != m_BuffList_C.end(); it++ )
		{
			BuffTable& BT = BuffTable::Singleton( );
			PBAT pBAT = BT.GetBuff( (*it).second );
			
			if ( pBAT )
			{
				if ( pBAT->nHSpecID )
				{
					m_DataRes.AddHState( pBAT->nHSpecID, (*it).first );
				}
				
				if ( pBAT->nBSpecID )
				{
					m_DataRes.AddBState( pBAT->nBSpecID, (*it).first );
				}
				
				if ( pBAT->nFSpecID )
				{
					m_DataRes.AddFState( pBAT->nFSpecID, (*it).first );
				}
				
				if ( pBAT->nDispID > -1 )
				{
					m_nEffectPolyMorphBuffIdx = (*it).first;
					PolyMorph( pBAT->nDispID, true, 0,m_CompAttrMgr[ncai_runspeed], 0 );
				}
				if ( pBAT->nSoundID )
				{
					char szBuff[256];
					sprintf( szBuff, BUFF_SOUND, pBAT->nSoundID  );
					m_pBuffSoundNode = g_SoundCache.GetNode(szBuff, (KCacheNode*)m_pBuffSoundNode);
					if ( m_pBuffSoundNode )
					{
						m_pBuffWave = (KWavSound*)m_pBuffSoundNode->m_lpData;
						if (m_pBuffWave)
						{
							if (m_pBuffWave->IsPlaying())
							{
								int nVolume = Option.GetSndVolume();
								m_pBuffWave->SetVolume( nVolume );
							}
							else
							{
								int nVolume = Option.GetSndVolume();
								m_pBuffWave->Play(0, nVolume, true);
							}					
						}
					}

				}
			}
		}
	}
	
	
	// --> Rocker Edit Start 2006/01/06
	time_t t;
	time(&t);
	if (t - m_dwPolyMorphTimeStamp >= 1)
	{
		m_dwPolyMorphTimeStamp = t;
		if (m_uPolyMorphTime > 0)
		{
			m_uPolyMorphTime--;
		}
	}
	// <-- Rocker End

	//////////////////////////////////////////////////////////////////////////
	if (Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index)
	{

		SubWorld[0].Map2Mps(m_RegionIndex, GetMapX(), GetMapY(), GetOffX(), GetOffY(), &nMpsX, &nMpsY);
		m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, TRUE);

	}
	else
	{
		SubWorld[0].Map2Mps(m_RegionIndex, GetMapX(), GetMapY(), GetOffX(), GetOffY(), &nMpsX, &nMpsY);
		m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, FALSE);
	}
	
	// client npc 时间计数处理：不往后跳
	if (m_Kind == kind_bird || m_Kind == kind_mouse)
		m_SyncSignal = SubWorld[0].m_dwCurrentTime;

	if (m_nChatContentLen > 0)
	{
		if (IR_GetCurrentTime() - m_nCurChatTime > NPC_SHOW_CHAT_TIME)
		{
			m_nChatContentLen = 0;
			m_nChatNumLine = 0;
			m_nChatFontWidth = 0;
			m_nCurChatTime = 0;
		}
	}

	m_TalismanNpcController.Active();
	
	if (IsPlayer())
	{
		CheckAndNotifyEspecialArea();
	}
#endif
#ifdef _DEBUG
#ifdef _SERVER
	//if(m_LoopFrames % 6 == 0)
	{
		int nMpsXX, nMpsYY;
		GetMpsPos(&nMpsXX, &nMpsYY);

		NPCREALPOSITION tagNpcRealPos;
		tagNpcRealPos.ProtocolType = s2c_npcrealposition;
		tagNpcRealPos.dwNpcID = m_dwID;
		tagNpcRealPos.nPosX = nMpsXX;
		tagNpcRealPos.nPosY = nMpsYY;
		tagNpcRealPos.nDirection = m_UnaryAttrMgr[nuai_dir];

		int nMaxCount = MAX_BROADCAST_COUNT_MIN;
		BROADCAST_REGION((BYTE*)&tagNpcRealPos, 
			sizeof(NPCREALPOSITION), nMaxCount);
	}
#endif
#endif

#ifndef _SERVER
	//自动取消选中
	int targetnpc = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetNpc();
	BOOL bDrawSelectedImg = (targetnpc!=0 && targetnpc == m_Index);
	int nSelectedType = m_Kind != kind_normal ? 0 : 1;
	UpdataNpcRes(m_Index, m_ResDir, m_Frames.nTotalFrame, m_Frames.nCurrentFrame, 
		FALSE, bDrawSelectedImg, nSelectedType);

	AutoRobotMgr& arm = AutoRobotMgr::Singleton();
	if ( targetnpc > 0 && targetnpc < MAX_NPC && IsPlayer() && !(arm.GetAutoEnstrustMode() & ENSTRUST_AUTO_ATTACK_MODE) )
	{
		ConfigManager& cm = ConfigManager::Singleton();
		int nRegionCnt = cm.GetGlobalVariable( global_var_valid_clear_target_distance );
		if ( nRegionCnt < 1 )
		{
			nRegionCnt = 1;
		}
		int nDistance = NpcSet.GetDistance( m_Index, targetnpc );
		if ( nDistance > nRegionCnt )
		{
			SetTarget( type_npc, 0 );
			CoreDataChanged( GDCNI_SEL_TARGET, FALSE, NULL );
		}
	}
#endif
	
	//灵石激活人物特效
#ifdef _SERVER
	int nItemInlayCount = 0;
	if(m_LoopFrames % (GAME_FPS * 5)  == 0)
	{
		int nPlayerIdx = GetPlayerIdx();
		if ( nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER && IsPlayer() )
		{		
			KItemList& itemList = Player[nPlayerIdx].GetItemList();
			for ( int nIdx = 0; nIdx < itempart_num; ++nIdx )
			{
				int nItemIdx = itemList.GetEquipment( nIdx );
				if ( nItemIdx > 0 && nItemIdx < MAX_ITEM )
				{
					short	inlaySpecialBuffSet[MAX_SPECIALEFFECT_COUNT];
					memcpy( inlaySpecialBuffSet, Item[nItemIdx].GetInlaySpecialBuffSet(), sizeof(inlaySpecialBuffSet) );
					for ( int buffIdx = 0; buffIdx < MAX_SPECIALEFFECT_COUNT; ++buffIdx )
					{
						if ( inlaySpecialBuffSet[buffIdx] > 0 )
						{
							++nItemInlayCount;
							break;
						}
					}
				}
			}
			m_nItemInlayCount = nItemInlayCount;
			NPC_INLAYCOUNT_SYNC	NetCommand;
			NetCommand.ProtocolType = (BYTE)s2c_npc_inlaycount;
			NetCommand.ID = m_dwID;
			NetCommand.nInlayCount = m_nItemInlayCount;
			if (IsVisibleToPlayer())
			{
				int nMaxCount = MAX_BROADCAST_COUNT;
				BROADCAST_REGION(&NetCommand,sizeof(NetCommand),nMaxCount);
			}
			else
			{
				if (IsPlayer() && g_pServer != NULL)
					g_pServer->PackDataToClient(Player[m_nPlayerIdx].GetNetConnectIdx(), (BYTE*)&NetCommand, sizeof(NetCommand));
			}
		}	
	}
#else
	ConfigManager& cm = ConfigManager::Singleton();
	if ( m_nItemInlayCount <= 0 )
	{
		m_DataRes.ClearAllInlayState( );
	}
	else if ( m_nItemInlayCount > 0 && m_nItemInlayCount <= 3 )
	{		
		m_DataRes.AddInlayState(0, cm.GetGlobalVariable(global_var_inlay_item_npc_effect3) );
	}
	else if ( m_nItemInlayCount > 3 && m_nItemInlayCount <= 6 )
	{
		m_DataRes.AddInlayState(0, cm.GetGlobalVariable(global_var_inlay_item_npc_effect6));
	}
	else if ( m_nItemInlayCount > 6 && m_nItemInlayCount < 9  )
	{
		m_DataRes.AddInlayState(0, cm.GetGlobalVariable(global_var_inlay_item_npc_effect9));
	}
	else
	{
		m_DataRes.AddInlayState(0, cm.GetGlobalVariable(global_var_inlay_item_npc_effect));
	}

#endif



}

void KNpc::ProcStatus()
{
	switch(m_Doing)
	{
	case do_stand:
		OnStand();
		break;
	case do_run:
		OnRun();
		break;
	// Modify by Cooler -->
	// 2006-5-18 17:35
/*	case do_walk:
		OnWalk();
		break;*/
	// End modify by Cooler <--
	case do_attack:
	case do_magic:
		OnSkill();
		break;
	case do_sit:
		OnSit();
		break;
	case do_hurt:
		OnHurt();
		break;
	case do_revive:
		OnRevive();
		break;
	case do_death:
		OnDeath();
		break;
	case do_defense:
//		OnDefense();
		break;
	case do_special1:
		OnSpecial1();
		break;
	case do_summonskill:
		OnSummonSkill();
		break;
	case do_special3:
		OnSpecial3();
		break;
	case do_special4:
		OnSpecial4();
		break;
	case do_manyattack:
		OnManyAttack();
		break;
	case do_runattack:
		OnRunAttack();
		break;
	case do_idle:
//		OnIdle();
	default:
		break;
	}
}

void KNpc::ProcCommand(int nAI)
{
	// CmdKind < 0 表示没有指令
	if ( m_Command.CmdKind == do_none )
		return;
	
#ifdef _SERVER
	if (nAI)
#else
	if (nAI || (m_Doing != do_death && m_Doing != do_revive && m_Command.CmdKind == do_skill))
#endif
	{
		if (m_RegionIndex < 0)
			return;
		
		switch (m_Command.CmdKind)
		{
		case do_stand:
			DoStand();
			break;
		case do_death:
			DoDeath( );
			break;

		// Modify by Cooler -->
		// 2006-5-18 17:00
/*		case do_walk:
#ifdef _SERVER
			if(IsPlayer())
			{
				Player[m_nPlayerIdx].m_BuyInfo.Clear();
			}
#endif
			// Add by Cooler 2004-5-21
			// Begin -->
#ifdef _SERVER
#pragma message("Stop produce need optimize. by Cooler!")
			if(m_tagProduceState.nProduceSpeed > 0)
			{
				ClearProduceState();
			}
			// End <--
#endif
			if(IsPlayer())
			{
				// Add by [Adt.X], 2004-9-20
				Player[GetPlayerIdx()].OverWeight(); // 判断超重
				if (m_uMoveSpeed != 0 || m_nMorphType < 0) // lixuewu 2004.07.12 变身
				{
#ifdef _SERVER
					if (m_bWeightMoveable && 
						Player[m_nPlayerIdx].m_ItemList.IsLockStorageBox()) 
#else
						if (m_bWeightMoveable)
#endif
						{ 
							Goto(m_Command.Param_X, m_Command.Param_Y);
						}
				// End.
				}
			}
			else
			{
				if (m_uMoveSpeed != 0 || m_nMorphType < 0) // lixuewu 2004.07.12 变身
				{
					Goto(m_Command.Param_X, m_Command.Param_Y);
				}
			}
			break;*/
		// End modify by Cooler <--
		case do_run:
			
			if(IsPlayer())
			{				
				if (m_uMoveSpeed != 0 || m_nMorphType < 0) // lixuewu 2004.07.12 变身
				{
					RunTo(m_Command.Param_X, m_Command.Param_Y);
					Player[m_nPlayerIdx].onPlayerRun();
				}
			}
			else
			{
				if (m_uMoveSpeed != 0 || m_nMorphType < 0) // lixuewu 2004.07.12 变身
				{
					RunTo(m_Command.Param_X, m_Command.Param_Y);
				}
			}
			break;
		case do_jump:
			break;
		case do_skill:
			{
				// 等表全了放开
				int nSkillIdx = m_SkillList.FindSkill(m_Command.Param_X);
				
				if ( nSkillIdx >= 0 &&
					SetActiveSkill( nSkillIdx, m_Command.Param_Z ) )		
				{
					DoSkill(m_Command.Param_Y, m_Command.Param_Z);
				}
				else
				{
					DoStand();
				}
			}
			break;

		case do_sit:
			DoSit();
			break;
		case do_defense:
			DoDefense();
			break;
		case do_idle:
			DoIdle();
			break;
		case do_hurt:
			DoHurt(m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z);
			break;	
			// 因为跨地图能把ai设为1
		case do_revive:
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
#ifndef _SERVER
			//this->SetInstantSpr(enumINSTANT_STATE_REVIVE);
#endif
			break;
		//-------> Ray [Luoliang] 2005-6-28
#ifndef _SERVER
		case do_special3:		//宠物动作1
			DoSpecial3();
			break;
		case do_special4:		//宠物动作2
			DoSpecial4();
			break;
#endif
		//<------- End [Ray]
		}
	}
	else
	{
		switch(m_Command.CmdKind)
		{
		case do_hurt:
			if (m_RegionIndex >= 0)
				DoHurt(m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z);
			break;
		case do_revive:
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
			break;
		default:
			break;
		}
	}

	m_Command.CmdKind = do_none;
}

void KNpc::DoDeath(int nMode/* = 0*/)
{
#ifdef _SERVER
	if( IsPlayer() )
	{
		m_LastEditFrame=0;
		Player[m_nPlayerIdx].m_Creature.Dismiss();
		Player[m_nPlayerIdx].GetEmployee().Fire();
		Player[m_nPlayerIdx].ReturnMarkCreature();
		Player[m_nPlayerIdx].GetActionDelayer().OnEvent(delayed_action_event_death);
	}
#endif

	if (m_RegionIndex < 0)
		return;

#ifndef _SERVER
	if (IsPlayer())
	{
		PlayerController::Singleton().Stop();
	}
#endif

	if (m_Doing == do_death)
		return;

	if (IsPlayer())	// 城镇内不会死亡
	{
		GetMpsPos(&m_DesX, &m_DesY); //lixuewu 修正BUG 
		SetTarget(type_npc,0);
	}

	if (m_Doing == do_runattack && m_SkillParam3 != 0)
	{
		m_CompAttrMgr.Set(ncai_runspeed, idx_append_value, 
						  m_CompAttrMgr[ncai_runspeed][idx_append_value] - m_SkillParam3);
		m_SkillParam3 = 0;
	}
	m_Doing = do_death;
	m_ProcessAI	= 0;
	m_ProcessState = 0;	

	m_Frames.nTotalFrame = m_DeathFrame;
	m_Frames.nCurrentFrame = 0;
	
	m_Height = 0;

#ifndef _SERVER
	if( IsPlayer() )
	{
		AutoRobotMgr::Singleton().SetMode(enRobotMode_None);
		AutoRobotMgr::Singleton().SetAutoSellState( false );
		AutoRobotMgr::Singleton().StopGoToOtherMap( false );
	}
#endif

#ifdef _SERVER

	if (g_PlayerMonitor.IsNeedRecord(GetPlayerIdx(), player_action_death))
	{ 
		RecordPlayerActionParam param;
		param.PlayerIndex = GetPlayerIdx();
		param.Action = player_action_death;
		snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_DEATH);
		g_PlayerMonitor.RecordPlayerAction(param);
	}

	m_Controller.Init(0);
	
	//设置死亡BUFF
	SetupEventBuff(NpcEvent_Death);
	
	//丢物品
	int nExp = DeathPunish(nMode, m_UnaryAttrMgr[nuai_owner]);

	//删除NPC存盘数据
	if (m_Kind != kind_player)
	{
		NpcSave::DeleteNpc(m_Index);
	}

	//--> Rocker 2005/06/29
	NPC_DEATH_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcdeath;
	NetCommand.ID = m_dwID;
	NetCommand.btKilledType = 0;
//	if (IsPlayer())
//	{
//		if (nMode == enumDEATH_MODE_NPC_KILL)
//		{
//			NetCommand.btKilledType = 1;
//			NetCommand.nExp = nExp;
//			if (nExp == 0)
//			{
//				NetCommand.nWeakTime = 0;
//			}
//			else
//			{
//				int nLevel = (m_Level >= 70) ? 70 : m_Level;
//				NetCommand.nWeakTime = PlayerSet.m_DeathWeakParam.m_nConstantValue1 + 15 * (nLevel - PlayerSet.m_DeathWeakParam.m_nConstantValue2);
//			}
//		}
//		else
//		{
//			NetCommand.btKilledType = 0;
//			NetCommand.nExp = nExp;
//		}
//	}

	m_btKilledType = NetCommand.btKilledType;
	//<-- End
	
	int nMaxCount = MAX_BROADCAST_COUNT_OPTIMIZED;
	BROADCAST_REGION(&NetCommand,sizeof(NetCommand),nMaxCount);
#endif
#ifndef _SERVER
	m_ClientDoing = cdo_death;
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetNpc() == m_Index)
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetTarget(type_npc, 0);
	}
#endif
#ifdef _SERVER
	if (IsPlayer())
	{
		// lixuewu 2004.03.19 todo  
		// lixuewu 2004.03.19
		// 如果交易正在进行，取消交易
		Player[m_nPlayerIdx].tradeServerDoCanceTrade();
	}
#endif

	if (IsValidNpc(m_nLastDamageIdx))
	{
		// 清空攻击者的目标
		if(Npc[m_nLastDamageIdx].GetTargetNpc() == m_Index)
			Npc[m_nLastDamageIdx].SetTarget(type_npc, 0);
		
		if(Npc[m_nLastDamageIdx].m_Kind == kind_player)
		{
			int nPlayerIdx = Npc[m_nLastDamageIdx].m_nPlayerIdx;
			if (IsValidPlayer(nPlayerIdx))
			{
				if( Player[nPlayerIdx].m_Creature.IsALive() )
				{
					KNpc *pCreature = Player[nPlayerIdx].m_Creature.GetCreatureNpc();
					
					if(pCreature && pCreature->GetTargetNpc() == m_Index)
						pCreature->SetTarget(type_npc, 0);
				}
				
#ifdef _SERVER
				int nMarkCreatureIdx = Player[nPlayerIdx].GetMarkCreature();
				
				if( IsValidNpc(nMarkCreatureIdx) )
				{
					if(Npc[nMarkCreatureIdx].GetTargetNpc() == m_Index)
						Npc[nMarkCreatureIdx].SetTarget(type_npc, 0);
				}

				//雇用
				Employee& employee = Player[nPlayerIdx].GetEmployee();
				if (employee.IsExist())
				{
					if (Npc[employee.GetNpcIndex()].GetTargetNpc() == m_Index)
						Npc[employee.GetNpcIndex()].SetTarget(type_npc, 0);
				}
#endif
			}
		}
		else if(Npc[m_nLastDamageIdx].m_Kind == kind_creature)
		{
			// 如果是召唤兽，m_nPlayerIdx 就是主人的 Npc Index
			int nOwnerNpcIdx = Npc[m_nLastDamageIdx].m_nPlayerIdx;
			
			if(Npc[nOwnerNpcIdx].GetTargetNpc() == m_Index)
				Npc[nOwnerNpcIdx].SetTarget(type_npc, 0);
		}
	}
}

void KNpc::OnDeath()
{
#ifdef _SERVER
	if(kind_creature == m_Kind)
	{
		int nSummonerNpcIdx = GetSummonerIdx();
		int nSummonerPlayerIdx = Npc[nSummonerNpcIdx].GetPlayerIdx();

		Player[nSummonerPlayerIdx].m_Creature.Dismiss();

		return;
	}
	else if (kind_employee == m_Kind)
	{
		int employerIndex = GetEmployerIdx();
		if (IsValidPlayer(employerIndex))
		{
			Player[employerIndex].GetEmployee().Fire();
		}

		return;
	}
#endif

	if (WaitForFrame())
	{
		g_DebugLog("[DEATH] WaitForFrame TRUE");
		m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;		// 保证不会有重回第一帧的情况
#ifndef _SERVER
		int iTargetNpc = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetTargetNpc();
		if (m_Index == iTargetNpc)
		{
			Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].SetTarget( type_npc ,0 );
			PlayerController::Singleton().Stop();
		}

		if ( m_nLastDamageIdx == Player[CLIENT_PLAYER_INDEX].GetNpcIndex() )
		{
			Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].SetCanFollowAndAttack( false );
			PlayerController::Singleton().Stop();
		}		
		m_ProcessAI = 1;

#else
		
		if (IsPlayer())
		{
			// lixuewu 2005.05.11 召唤兽打死玩家脚本传入的是召唤者的NpcIndex
			KNpc& aNpc = Npc[m_nLastDamageIdx];
			int nScriptNpcIdx = m_nLastDamageIdx;
			if (aNpc.m_Kind == kind_creature)
			{
				nScriptNpcIdx = aNpc.GetSummonerIdx();
			}
			else if (aNpc.m_Kind == kind_employee)
			{
				nScriptNpcIdx = aNpc.GetEmployerIdx();
			}
			// <-- end
			//如果当间死者为主角，则看有没有死亡脚本，有则运行。	
			if (Player[m_nPlayerIdx].m_dwDeathScriptId)
			{
				Player[m_nPlayerIdx].ExecuteScript2Param(Player[m_nPlayerIdx].m_dwDeathScriptId, "OnDeath", 0, nScriptNpcIdx, Npc[nScriptNpcIdx].m_NpcSettingIdx);
			}
		}
		else
		{
			int nPlayerIdx = m_UnaryAttrMgr[nuai_owner];
			if (nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER)
			{
				// 执行战斗npc死亡脚本
				if (m_ActionScriptID != 0)
				{
					Player[nPlayerIdx].ExecuteScript2Param(m_ActionScriptID, "MobKilled", 0, m_Index, m_NpcSettingIdx);		
				}
				else if (m_dwDeathScriptID != 0)
				{
					Player[nPlayerIdx].ExecuteScript2Param(m_dwDeathScriptID, "MobKilled", 0, m_Index, m_NpcSettingIdx);
				}
			}
		}

		int nDeathSender = m_nLastDamageIdx;
		if(Npc[m_nLastDamageIdx].m_Kind == kind_creature)
		{
			nDeathSender = Npc[m_nLastDamageIdx].GetSummonerIdx();
		}
		else if (Npc[m_nLastDamageIdx].m_Kind == kind_employee)
		{
			nDeathSender = Npc[m_nLastDamageIdx].GetEmployerIdx();
		}

		//filter npc death
		//===================================================
		BuffMgr& BM = BuffMgr::Singleton( );
		BUFF_ENV_PARAM Env;
		Env.nEventFormat	=	buff_event_format_event;
		Env.nEventSender	=	nDeathSender;
		Env.nEventRecever	=	m_Index;


		Env.nEventType		=	buff_event_type_npcdeathin;
		Env.nEventRelation	=	buff_event_relation_recver;
		BM.FilterEvent( Env );

		Env.nEventType		=	buff_event_type_npcdeathout;
		Env.nEventRelation	=	buff_event_relation_sender;
		BM.FilterEvent( Env );
		//===================================================
		
		if( m_Kind == kind_creature )
		{
			//sender creature event to summoner
			//===================================================
			Env.nEventSender	=	m_nLastDamageIdx;
			Env.nEventRecever	=	GetSummonerIdx( );
			Env.nEventType		=	buff_event_type_creaturedeath;
			Env.nEventRelation	=	buff_event_relation_recver;
			BM.FilterEvent( Env );
			//===================================================
		}

#endif

#ifdef _SERVER
	
		if( Npc[m_Index].m_UnaryAttrMgr[nuai_deathmode] & npc_deathmode_autodel )
		{
			int nSubWorld = Npc[m_Index].m_SubWorldIndex;
			int nRegion = Npc[m_Index].m_RegionIndex;
			SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(m_Index);
			NpcSet.Remove(m_Index);
		}
		else
			DoRevive();
#else
		DoRevive();
#endif

#ifndef _SERVER
		// 客户端把NPC删除
		if (m_Kind != kind_player)
		{
#ifndef _SERVER
			if (Npc[m_Index].m_RegionIndex >= 0)
			{
				int nSubWorld = Npc[m_Index].m_SubWorldIndex;
				int nRegion = Npc[m_Index].m_RegionIndex;
				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(m_Index);
			}
			NpcSet.Remove(m_Index, false);
#else
			{
				int nSubWorld = Npc[m_Index].m_SubWorldIndex;
				int nRegion = Npc[m_Index].m_RegionIndex;
				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(m_Index);
				NpcSet.Remove(m_Index);
			}
#endif
			return;
		}
#endif		
	}
	else
	{
		g_DebugLog("[DEATH] WaitForFrame FALSE");
	}
}

void KNpc::DoDefense()
{
	m_ProcessAI = 0;
}

#ifndef _SERVER

void KNpc::UpdataNpcRes( int nNpcIdx, int nDir, int nAllFrame, int nCurFrame, 
		BOOL bInMenu, BOOL bDrawSelected, int nSelectedType )
{

	int		i, j, nGetFrame = 1, nGetDir = 1, nPos;
	int		nCurFrameNo = 0, nCurDirNo = 0;
	int		nScreenX = m_DataRes.m_nXpos, nScreenY = m_DataRes.m_nYpos, nScreenZ = m_DataRes.m_nZpos;
	int		nStateFrameNo;

	if (nDir < 0 || nAllFrame < 0 || nCurFrame < 0)
		return;

	if (!m_DataRes.m_pcResNode)
		return;

	// 状态特效换帧
	for (i = 0; i < MAX_STATE_PART_NUM; i++)
		m_DataRes.m_cStateSpr[i].m_SprContrul.GetNextFrame();
	if ( m_DataRes.m_cSpecialSpr.GetNextFrame(FALSE) )
	{
		if ( m_DataRes.m_cSpecialSpr.CheckEnd() )
			m_DataRes.m_cSpecialSpr.Release();
	}

	// 播放声音
	if (nCurFrame < nAllFrame / 4)
	{
		m_DataRes.GetSoundName();
		m_DataRes.PlaySound(m_DataRes.m_nXpos, m_DataRes.m_nYpos);
		/*if (m_DataRes.m_nDoing == cdo_attack)
		{
			//if (g_Random(5) == 1)
			{
				m_DataRes.GetSoundName();
				m_DataRes.PlaySound(m_DataRes.m_nXpos, m_DataRes.m_nYpos);
			}
		}
		else
		{
			m_DataRes.GetSoundName();
			m_DataRes.PlaySound(m_DataRes.m_nXpos, m_DataRes.m_nYpos);
		}//*/
	}
	
	// 脚底状态特效
	nPos = 0;
	for ( i = 2; i < 4; i++)
	{
		if (m_DataRes.m_cStateSpr[i].m_nID)
		{
			strcpy(m_DataRes.m_cFootFile[nPos].szImage, m_DataRes.m_cStateSpr[i].m_SprContrul.m_szName);
			m_DataRes.m_cFootFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
			m_DataRes.m_cFootFile[nPos].uImage = m_DataRes.m_cStateSpr[i].m_SprContrul.m_dwNameID;
			m_DataRes.m_cFootFile[nPos].nFrame = m_DataRes.m_cStateSpr[i].m_SprContrul.m_nCurFrame;
			m_DataRes.m_cFootFile[nPos].oPosition.nX = nScreenX;
			m_DataRes.m_cFootFile[nPos].oPosition.nY = nScreenY;
			m_DataRes.m_cFootFile[nPos].oPosition.nZ = 0;
			nPos++;
		}
	}
	m_DataRes.m_nFootNum = nPos;

	// 身上状态特效(npc背后)
	nPos = 0;
	for (i = 4; i < 7; i++)
	{
		if (m_DataRes.m_cStateSpr[i].m_nID)
		{
			if (m_DataRes.m_cStateSpr[i].m_nCopyNum < MIN_RES_STATE_COPY || m_DataRes.m_cStateSpr[i].m_nCopyNum > MAX_RES_STATE_COPY)
				continue;
			if (m_DataRes.m_cStateSpr[i].m_nCopyNum == 1)
			{
				if (m_DataRes.m_cStateSpr[i].m_nBackStart <= m_DataRes.m_cStateSpr[i].m_SprContrul.m_nCurFrame && 
					m_DataRes.m_cStateSpr[i].m_SprContrul.m_nCurFrame < m_DataRes.m_cStateSpr[i].m_nBackEnd)
				{
					strcpy(m_DataRes.m_cBodyFile[nPos].szImage, m_DataRes.m_cStateSpr[i].m_SprContrul.m_szName);
					m_DataRes.m_cBodyFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
					m_DataRes.m_cBodyFile[nPos].uImage = m_DataRes.m_cStateSpr[i].m_SprContrul.m_dwNameID;
					m_DataRes.m_cBodyFile[nPos].nFrame = m_DataRes.m_cStateSpr[i].m_SprContrul.m_nCurFrame;
					m_DataRes.m_cBodyFile[nPos].oPosition.nX = nScreenX;
					m_DataRes.m_cBodyFile[nPos].oPosition.nY = nScreenY;
					int nHeightOff = 0;
					if (m_DataRes.m_nAction == cdo_sit) 
					{
						//nHeightOff -= 40;
					}
					else
					{
						if (m_bRideHorse)
							nHeightOff += 38;
					}
					// 设置技能显示部位头顶、身上、脚底
					if ( 2 == m_DataRes.m_cStateSpr[i].m_nType )
					{
						nHeightOff += (Npc[nNpcIdx].GetNpcPate()/2);
					}
					else if ( 1 == m_DataRes.m_cStateSpr[i].m_nType )
					{
						nHeightOff += Npc[nNpcIdx].GetNpcPate();
					}
					m_DataRes.m_cBodyFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
					nPos++;
				}
			}
			else
			{
				nStateFrameNo = m_DataRes.m_cStateSpr[i].m_SprContrul.m_nCurFrame;
				for (j = 0; j < m_DataRes.m_cStateSpr[i].m_nCopyNum; j++)
				{
					nStateFrameNo = m_DataRes.m_cStateSpr[i].m_SprContrul.GetPartFrame(nStateFrameNo, m_DataRes.m_cStateSpr[i].m_nCopyNum);

					if (m_DataRes.m_cStateSpr[i].m_nBackStart <= nStateFrameNo && nStateFrameNo < m_DataRes.m_cStateSpr[i].m_nBackEnd)
					{
						strcpy(m_DataRes.m_cBodyFile[nPos].szImage, m_DataRes.m_cStateSpr[i].m_SprContrul.m_szName);
						m_DataRes.m_cBodyFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
						m_DataRes.m_cBodyFile[nPos].uImage = m_DataRes.m_cStateSpr[i].m_SprContrul.m_dwNameID;
						m_DataRes.m_cBodyFile[nPos].nFrame = nStateFrameNo;
						m_DataRes.m_cBodyFile[nPos].oPosition.nX = nScreenX;
						m_DataRes.m_cBodyFile[nPos].oPosition.nY = nScreenY;
						int nHeightOff = 0;
						if (m_bRideHorse)
							nHeightOff += 38;
						m_DataRes.m_cBodyFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
						nPos++;
					}
				}
			}
		}
	}
	m_DataRes.m_nBodyBackNum = nPos;

	
	// 身上状态特效(npc身前)
	nPos = 0;
	for (i = 4; i < 7; i++)
	{
		if (m_DataRes.m_cStateSpr[i].m_nID)
		{
			if (m_DataRes.m_cStateSpr[i].m_nCopyNum < MIN_RES_STATE_COPY || m_DataRes.m_cStateSpr[i].m_nCopyNum > MAX_RES_STATE_COPY)
				continue;
			if (m_DataRes.m_cStateSpr[i].m_nCopyNum == 1)
			{
				if (m_DataRes.m_cStateSpr[i].m_SprContrul.m_nCurFrame < m_DataRes.m_cStateSpr[i].m_nBackStart || 
					m_DataRes.m_cStateSpr[i].m_SprContrul.m_nCurFrame >= m_DataRes.m_cStateSpr[i].m_nBackEnd)
				{
					strcpy(m_DataRes.m_cBodyFile[nPos].szImage, m_DataRes.m_cStateSpr[i].m_SprContrul.m_szName);
					m_DataRes.m_cBodyFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
					m_DataRes.m_cBodyFile[nPos].uImage = m_DataRes.m_cStateSpr[i].m_SprContrul.m_dwNameID;
					m_DataRes.m_cBodyFile[nPos].nFrame = m_DataRes.m_cStateSpr[i].m_SprContrul.m_nCurFrame;
					m_DataRes.m_cBodyFile[nPos].oPosition.nX = nScreenX;
					m_DataRes.m_cBodyFile[nPos].oPosition.nY = nScreenY;
					int nHeightOff = 0;
					if (m_bRideHorse)
						nHeightOff += 38;
					// 设置技能显示部位头顶、身上、脚底
					if ( 2 == m_DataRes.m_cStateSpr[i].m_nType )
					{
						nHeightOff += (Npc[nNpcIdx].GetNpcPate()/2);
					}
					else if ( 1 == m_DataRes.m_cStateSpr[i].m_nType )
					{
						nHeightOff += Npc[nNpcIdx].GetNpcPate();
					}
					
					m_DataRes.m_cBodyFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
					nPos++;
				}
			}
			else
			{
				nStateFrameNo = m_DataRes.m_cStateSpr[i].m_SprContrul.m_nCurFrame;
				for (j = 0; j < m_DataRes.m_cStateSpr[i].m_nCopyNum; j++)
				{
					nStateFrameNo = m_DataRes.m_cStateSpr[i].m_SprContrul.GetPartFrame(nStateFrameNo, m_DataRes.m_cStateSpr[i].m_nCopyNum);

					if (nStateFrameNo < m_DataRes.m_cStateSpr[i].m_nBackStart || nStateFrameNo >= m_DataRes.m_cStateSpr[i].m_nBackEnd)
					{
						strcpy(m_DataRes.m_cBodyFile[nPos].szImage, m_DataRes.m_cStateSpr[i].m_SprContrul.m_szName);
						m_DataRes.m_cBodyFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
						m_DataRes.m_cBodyFile[nPos].uImage = m_DataRes.m_cStateSpr[i].m_SprContrul.m_dwNameID;
						m_DataRes.m_cBodyFile[nPos].nFrame = nStateFrameNo;
						m_DataRes.m_cBodyFile[nPos].oPosition.nX = nScreenX;
						m_DataRes.m_cBodyFile[nPos].oPosition.nY = nScreenY;
						int nHeightOff = 0;
						if (m_bRideHorse)
							nHeightOff += 38;
						m_DataRes.m_cBodyFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
						nPos++;
					}
				}
			}
		}
	}
	m_DataRes.m_nBodyFrontNum = nPos;

	// 头顶状态特效
	nPos = 0;
	for ( i = 0; i < 2; i++)
	{
		if (m_DataRes.m_cStateSpr[i].m_nID)
		{
			strcpy(m_DataRes.m_cHeadFile[nPos].szImage, m_DataRes.m_cStateSpr[i].m_SprContrul.m_szName);
			m_DataRes.m_cHeadFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
			m_DataRes.m_cHeadFile[nPos].uImage = m_DataRes.m_cStateSpr[i].m_SprContrul.m_dwNameID;
			m_DataRes.m_cHeadFile[nPos].nFrame = m_DataRes.m_cStateSpr[i].m_SprContrul.m_nCurFrame;
			m_DataRes.m_cHeadFile[nPos].oPosition.nX = nScreenX;
			m_DataRes.m_cHeadFile[nPos].oPosition.nY = nScreenY;
			int nHeightOff = 0;
			if (m_bRideHorse)
				nHeightOff += 38;
			m_DataRes.m_cHeadFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
			nPos++;
		}
	}
	m_DataRes.m_nHeadNum = nPos;
}
#endif

void KNpc::DoIdle()
{
	if (m_Doing == do_idle)
		return;
	m_Doing = do_idle;
}

void KNpc::DoHurt(int nHurtFrames, int nX, int nY)
{
//	_ASSERT(m_RegionIndex >= 0);
#ifndef _SERVER
	if ( (nHurtFrames == m_HurtFrame) && (m_Kind == kind_player || m_Kind == kind_creature || m_Kind == kind_employee))
	{
		return;
	}
	m_DataRes.SetBlur(FALSE);
#endif
	if (m_RegionIndex < 0)
		return;
	if (m_Doing == do_hurt || m_Doing == do_death)
		return;

	// Add by Cooler -->
	// 2006-5-19 10:28
//	BakupMoveStatus();
	// End add by Cooler <--
	// 受击回复速度已经达到100%了，不做受伤动作
#ifdef _SERVER
	if (m_CompAttrMgr[ncai_hitrecover] >= 100)
		return;
#define	MIN_HURT_PERCENT	50	
	if (!g_RandPercent(MIN_HURT_PERCENT + m_CompAttrMgr[ncai_hitrecover] * (100 - MIN_HURT_PERCENT) / 100))
	{
		return;
	}
#endif
	
	if (m_Doing == do_runattack && m_SkillParam3 != 0)
	{
		m_CompAttrMgr.Set(ncai_runspeed, idx_append_value, 
						  m_CompAttrMgr[ncai_runspeed][idx_append_value] - m_SkillParam3);
		m_SkillParam3 = 0;
	}
	m_Doing = do_hurt;
	m_ProcessAI	= 0;

#ifdef _SERVER
	m_Frames.nTotalFrame = m_HurtFrame * (100 - m_CompAttrMgr[ncai_hitrecover]) / 100;
#else
	m_ClientDoing = cdo_hurt;
	m_Frames.nTotalFrame = nHurtFrames;
	m_nHurtDesX = nX;
	m_nHurtDesY = nY;
	if (m_Height > 0)
	{
		// 临时记录下来做为高度变化，在OnHurt中使用
		m_nHurtHeight = m_Height;
	}
	else
	{
		m_nHurtHeight = 0;
	}
#endif
	if (m_Frames.nTotalFrame == 0)
		m_Frames.nTotalFrame = 1;
	m_Frames.nCurrentFrame = 0;

#ifdef _SERVER	// 向周围9个Region广播发技能
	NPC_HURT_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npchurt;
	NetCommand.ID = m_dwID;
	NetCommand.nFrames = m_Frames.nTotalFrame;
	GetMpsPos(&NetCommand.nX, &NetCommand.nY);

	if (IsVisibleToPlayer())
	{
		int nMaxCount = MAX_BROADCAST_COUNT_MIN;
		BROADCAST_REGION(&NetCommand,sizeof(NetCommand),nMaxCount);
	}
	else
	{
		if (IsPlayer() && g_pServer != NULL)
			g_pServer->PackDataToClient(Player[m_nPlayerIdx].GetNetConnectIdx(), (BYTE*)&NetCommand, sizeof(NetCommand));
	}
#endif
}

void KNpc::OnHurt()
{
	if (m_RegionIndex < 0)
	{
		g_DebugLog("[error]%s Region Index < 0 when hurt", Name);
		return;
	}
	int nX, nY;
	GetMpsPos(&nX, &nY);
#ifdef _SERVER
	m_Height = 0;
#endif

	if (WaitForFrame())
	{
		g_DebugLog("[DEATH]On Hurt Finished");
		// Modify by Cooler -->
		// 2006-5-19 10:31
		// 放开，否则会不停的做受伤动作
		DoStand();
		m_ProcessAI = 1;
		RestoreMoveStatus();
		// End modify by Cooler <--
	}
}

void KNpc::DoSpecial1()
{
	DoBlurAttack();
}

void KNpc::OnSpecial1()
{
	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (pSkill)
		{
			int nChildSkill = pSkill->GetChildSkillId();
			int nChildSkillLevel = pSkill->GetCurLevel();
			
			if (nChildSkill > 0)
			{
				KSkill * pChildSkill = (KSkill*)g_SkillManager.GetSkill(nChildSkill, nChildSkillLevel);
				if (pChildSkill)
				{
					pChildSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
				}
			}
		}

		if (m_Frames.nTotalFrame == 0)
		{
			m_ProcessAI = 1;
		}
	}
}

void KNpc::OnSummonSkill()
{
	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif

		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		if (IsPlayer())
		{
			// 其实如果把KCreature放到NPC上也许更好
			_ASSERT(m_Kind == kind_player); // 只有玩者能召唤
					
			KCreature& aCreature = Player[m_nPlayerIdx].m_Creature;
			KSkill * pSkill = GetActiveSkill();

			if(NULL == pSkill)
				return;

			const int nSkillID = pSkill->GetSkillId();
					
			if (!aCreature.IsExist(nSkillID)) // 换了技能了,应该可以招新的了
			{
				if (aCreature.IsALive()) // 以前的还在么，这样的话，死掉了要记得清掉
				{	
#ifdef _SERVER
					// 哈，还在 吃经验了
					aCreature.Dismiss();
#endif
				}
	
				pSkill->Cast(m_Index, m_DesX, m_DesY);
				
// 				if( IsPlayer() && g_SkillManager.IsCommonCoolDown(nSkillID))
// 				{
// 					int nSkillSeries = m_Series * role_skillseries_count + Player[m_nPlayerIdx].GetSkillSeries();
// 					_ASSERT(nSkillSeries >= 0 && nSkillSeries <= 5);
// 					m_SkillList.SetComCoolTime(
// 						ConfigManager::Singleton().GetGlobalVariable((enumGlobalVariable)(global_var_commoncooldown_interval_1+nSkillSeries))
// 						);
// 				}

#ifdef _SERVER
				m_SkillList.CoolDown(m_nRealActiveSkillIdForCD);
#endif
			}
		}
			
		if (m_Frames.nTotalFrame == 0)
		{
			m_ProcessAI = 1;
		}
	}	
}

void KNpc::DoSpecial3()
{
	if ( m_Doing == do_special3 )
	{
		return;
	}
	m_Doing = do_special3;
	m_ClientDoing = cdo_attack;
	m_Frames.nCurrentFrame = 0;
	m_Frames.nTotalFrame = m_AttackFrame;
}

void KNpc::OnSpecial3()
{
	if ( IsReachFrame(100) )
	{
		DoStand();
	}
	else
	{		
		++m_Frames.nCurrentFrame;
	}
}

void KNpc::DoSpecial4()
{
	if ( m_Doing == do_special4 )
	{
		return;
	}
	m_Doing = do_special4;	
	m_ClientDoing = cdo_sit;
	m_Frames.nCurrentFrame = 0;
	m_Frames.nTotalFrame = m_SitFrame;
}

void KNpc::OnSpecial4()
{
	if ( IsReachFrame(100) )
	{
		DoStand();
	}
	else
	{		
		++m_Frames.nCurrentFrame;
	}
}

void KNpc::DoStand()
{
#ifndef _SERVER
	/*if ( g_DestPosIdx != -1 )
		ObjSet.RemoveIfClientOnly(g_DestPosIdx);

	g_DestPosIdx = -1;//*/
	
	m_DataRes.SetBlur(FALSE);
#endif
	m_Frames.nTotalFrame = m_StandFrame;
	if (m_Doing == do_stand)
	{
		return;
	}
	else
	{
		if (m_Doing == do_runattack && m_SkillParam3 != 0)
		{
			m_CompAttrMgr.Set(ncai_runspeed, idx_append_value, 
							  m_CompAttrMgr[ncai_runspeed][idx_append_value] - m_SkillParam3);
			m_SkillParam3 = 0;
		}
		m_Doing = do_stand;
		m_Frames.nCurrentFrame = 0;
		GetMpsPos(&m_DesX, &m_DesY);
#ifndef _SERVER
		if ( m_UnaryAttrMgr[nuai_fightstate] )
		{
			m_ClientDoing = cdo_fightstand;
		}
		else
		{
			if (g_Random(6) != 1)
			{
				m_ClientDoing = cdo_stand;
			}
			else
			{
				m_ClientDoing = cdo_stand1;
			}			
		}

		m_DataRes.StopSound();
#endif
	}
}


void KNpc::OnStand()
{
	if (WaitForFrame())
	{
#ifndef _SERVER
		if ( m_UnaryAttrMgr[nuai_fightstate] )
		{
			m_ClientDoing = cdo_fightstand;
		}
		else
		{
			if (g_Random(6) != 1)
			{
				m_ClientDoing = cdo_stand;
			}
			else
			{
				m_ClientDoing = cdo_stand1;
			}
		}
#endif
	}
}

void KNpc::DoRevive()
{
	if (m_RegionIndex < 0)
	{
		g_DebugLog("[error]%s Region Index < 0 when dorevive", Name);
		return;
	}

#ifndef _SERVER
	m_DataRes.SetBlur(FALSE);
#endif

	if (m_Doing == do_revive)
	{
		return;
	}
	else
	{
		if (m_Doing == do_runattack && m_SkillParam3 != 0)
		{
			m_CompAttrMgr.Set(ncai_runspeed, idx_append_value, 
							  m_CompAttrMgr[ncai_runspeed][idx_append_value] - m_SkillParam3);
			m_SkillParam3 = 0;
		}
		m_Doing = do_revive;
		m_ProcessAI = 0;
		m_ProcessState = 0;

#ifdef _SERVER
		
		if (IsPlayer())
		{
			Player[m_nPlayerIdx].SetReviveFlag(TRUE);
			return;
		}

		m_Frames.nTotalFrame = m_ReviveFrame;
		//lixuewu 2006.05.23 取消NPC阻挡
		//SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY, m_Index);
		SubWorld[m_SubWorldIndex].NpcChangeRegion(m_RegionIndex, VOID_REGION, m_Index);	// spe 03/06/28
		m_Frames.nCurrentFrame = 0;
#else
		// 客户端
		/*
		if (IsPlayer())
		{
			//--> Rocker 2005/06/29
			KSystemMessage Msg;
			if (m_btKilledType == 0)
			{
				if (Player[CLIENT_PLAYER_INDEX].m_nLoseExp == 0)
				{
					sprintf(Msg.szMessage, MSG_NO_EXP_DEATH_INFO);
				}
				else
				{
					sprintf(Msg.szMessage, MSG_KILLBYPLAYER_DEATH_INFO, Player[CLIENT_PLAYER_INDEX].m_nLoseExp);
				}
				Msg.byConfirmType = SMCT_UI_RENASCENCE;
			}
			else
			{
				if (Player[CLIENT_PLAYER_INDEX].m_nLoseExp == 0)
				{
					sprintf(Msg.szMessage, MSG_NO_EXP_DEATH_INFO);
					Msg.byConfirmType = SMCT_UI_RENASCENCE;
				}
				else
				{
					sprintf(Msg.szMessage, MSG_KILLBYNPC_DEATH_INFO, Player[CLIENT_PLAYER_INDEX].m_dwWeakTime, 
					Player[CLIENT_PLAYER_INDEX].m_nLoseExp);
					Msg.byConfirmType = SMCT_UI_RENASCENCE_KILLEDBY_NPC;
				}
			}
			//<-- End
			Msg.byParamSize = 0;
			Msg.byPriority = 255;
			Msg.eType = SMT_PLAYER;
//			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
		}//*/
		m_Frames.nTotalFrame = m_DeathFrame;
		m_ClientDoing = cdo_death;
#endif
	}
}

void KNpc::OnRevive()
{
#ifdef _SERVER
	if (!IsPlayer() && WaitForFrame())
	{
		Revive();
	}
#else	// 客户端
	m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;
#endif
}

void KNpc::DoRun()
{
	// Add by chenshanglin on [2006-3-15 13:46]
	if(m_CompAttrMgr[ncai_runspeed] == 0 && m_CompAttrMgr[ncai_walkspeed] == 0)
	{
		return;
	}
	// Add end
	
	_ASSERT(m_RegionIndex >= 0);

	if(m_CompAttrMgr[ncai_runspeed] > 0)
		m_Frames.nTotalFrame = (m_RunFrame * m_CompAttrMgr[ncai_runspeed][idx_base_value]) / m_CompAttrMgr[ncai_runspeed];
	else
		m_Frames.nTotalFrame = m_RunFrame;

#ifndef _SERVER
	//-------> Ray [Luoliang] 2005-6-30
	if ( m_Kind != kind_pet )
	{
		if ( m_UnaryAttrMgr[nuai_fightstate] )
		{
			m_ClientDoing = cdo_fightrun;
		}
		else
		{
			m_ClientDoing = cdo_run;
		}
	}
	else
	{
		m_ClientDoing = cdo_hurt;
	}
#endif

#ifdef _SERVER
	NPC_RUN_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcrun;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = m_DesX;
	NetCommand.nMpsY = m_DesY;
	
	int nMaxCount = MAX_BROADCAST_COUNT_OPTIMIZED;
	BROADCAST_REGION(&NetCommand, sizeof(NetCommand), nMaxCount);
#endif

	if (m_Doing == do_run)
	{
		return;
	}
	if (m_Doing == do_runattack && m_SkillParam3 != 0)
	{
		m_CompAttrMgr.Set(ncai_runspeed, idx_append_value,
						  m_CompAttrMgr[ncai_runspeed][idx_append_value] - m_SkillParam3);
		m_SkillParam3 = 0;
	}
	m_Doing = do_run;

	m_Frames.nCurrentFrame = 0;
}

void KNpc::OnRun()
{
	if( m_UnaryAttrMgr[nuai_nomove] )
		DoStand( );
	
	WaitForFrame();
	ServeMove(m_CompAttrMgr[ncai_runspeed]);
}

void KNpc::DoSit()
{
	_ASSERT(m_RegionIndex >= 0);

	if (m_Doing == do_sit)
	{
//		DoStand();
		return;
	}
	
	m_Doing = do_sit;
//#ifdef _SERVER	// 向周围9个Region广播发技能
//	NPC_SIT_SYNC	NetCommand;
//	NetCommand.ProtocolType = (BYTE)s2c_npcsit;
//	NetCommand.ID = m_dwID;
//		
//	int nMaxCount = MAX_BROADCAST_COUNT;
//	BROADCAST_REGION(&NetCommand, sizeof(NetCommand), nMaxCount);
//#endif
//
//#ifdef _SERVER // 向自己同步坐下信息
//	if (IsPlayer())
//	{
//		if (m_nPlayerIdx > 0 && m_nPlayerIdx < MAX_PLAYER)
//		{
//			NOTIFY_CLIENT sNotify;
//			memset(&sNotify, 0, sizeof(NOTIFY_CLIENT));
//			sNotify.ProtocolType = s2c_notify;
//			sNotify.nNotifyType  = enumSNT_YOU_SIT_DOWN;
//			sNotify.cStatus[0]   = 1;
//			if (g_pServer != NULL)
//				g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE *)&sNotify, sizeof(NOTIFY_CLIENT));
//		}
//	}
//#endif


#ifndef _SERVER
		m_ClientDoing = cdo_sit;
#endif

	m_Frames.nTotalFrame = m_SitFrame;
	m_Frames.nCurrentFrame = 0;
}

void KNpc::OnSit()
{
	// 体力换内力（没有设定）
	if (WaitForFrame())
	{
		m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;
	}
}

void KNpc::DoSkill(int nX, int nY)
{
	_ASSERT(m_RegionIndex >= 0);
	
	if (m_Doing == do_skill)
		return;
	
	// 非战斗状态不能发技能
	if (IsPlayer())
	{
#ifdef _SERVER
		if ((!m_bCanCast && m_nMorphType >= 0))// 保护状态不能发技能 lixuewu
			return;
#endif
	}

	KSkill * pSkill = GetActiveSkill();

	if(pSkill)
	{
		int nAttackTargetType = pSkill->GetAttackTargetType();

		// 变身的安全保护 lixuewu 2004.11.18
		if ( (nAttackTargetType & att_target_only) && nX == SKILL_SPT_TargetIndex )
		{
			if (nY > 0 && nY < MAX_NPC)
			{
				const int nSafeGuardLevel = Npc[nY].m_nSafeGuardLevel;
				if (nSafeGuardLevel >= 2)
				{
					return;
				}
				else if (nSafeGuardLevel >= 1)
				{
					if (m_Kind != kind_normal)
					{
						return;
					}
				}
			}
		}
		
		// 只要选中了目标，不论技能是不是targetonly，都施放
		// 目的是为了当异人使用可以空放的技能左键攻击敌人时，宝宝能马上主动攻击
// 		if ( (nAttackTargetType & att_target_enemy) && nX == SKILL_SPT_TargetIndex )
// 		{
// 			SetTarget(type_npc, nY);
// 		}


		int nSkillListCanCast = FALSE;
#ifdef _SERVER
		if( !m_bTransSkill )
			nSkillListCanCast = m_SkillList.CanCast(
			m_ActiveSkillID);
		else
			nSkillListCanCast = TRUE;

		m_bTransSkill  = FALSE;
#else
		nSkillListCanCast = m_SkillList.CanCast(
			m_ActiveSkillID);
#endif

#ifdef _SERVER	// 向周围9个Region广播发技能
		if ( nSkillListCanCast 
			 && pSkill->CanCastSkill(m_Index, nX, nY)
			 && ( (m_Kind != kind_player && m_Kind != kind_creature && m_Kind != kind_employee) 
			      || Cost(pSkill) )
		   )
		{
	
			
		/*------------------------------------------------------------------------------------
		发技能时，当需指定目标对象时，传至Skill.Cast的两个参数第一个参数为-1,第二个为Npc index
		在S2C时，第二个参数必须由Server的NpcIndex转为NpcdwID参出去。
		在C收到该指令时，将NpcdwID转为本机的NpcIndex
			-------------------------------------------------------------------------------------*/

			//将OnSkill中的判断移动到DoSkill中
			NPC_SKILL_SYNC	NetCommand;			
			NetCommand.ProtocolType = (BYTE)s2c_skillcast;
			NetCommand.ID = m_dwID;
			NetCommand.nSkillID = (WORD)m_ActiveSkillID;
			NetCommand.nSkillLevel = (BYTE)m_ActiveSkillLevel;			
			if (nY <= 0 ) 
			{
				DoStand();
				return;
			}
			
			NetCommand.nMpsX = nX;
			if (nX == SKILL_SPT_TargetIndex) //m_nDesX == -1 means attack someone whose id is DesY , and if m_nDesX == -2 means attack at somedir
			{
				NetCommand.nMpsY = Npc[nY].m_dwID;
				if (0 == NetCommand.nMpsY || Npc[nY].m_SubWorldIndex != m_SubWorldIndex)
					return;
			}
			else
			{
				NetCommand.nMpsY = nY;
			}
			m_SkillParam1 = nX;
			m_SkillParam2 = nY;
			m_DesX = nX;
			m_DesY = nY;			
			if (IsVisibleToPlayer())
			{
				//优先发自己的数据
				int nMaxCount = MAX_BROADCAST_COUNT_OPTIMIZED;
				BROADCAST_REGION(&NetCommand, sizeof(NetCommand), nMaxCount);
			}
			else
			{
				if (IsPlayer() && g_pServer != NULL)
					g_pServer->PackDataToClient(Player[m_nPlayerIdx].GetNetConnectIdx(), (BYTE*)&NetCommand, sizeof(NetCommand));
			}

			if(IsPlayer())
			{
				if (pSkill->IsCostItem())
				{
					pSkill->CostItem(m_Index);
				}
				
				Player[m_nPlayerIdx].GetItemList().Abrade(abrade_attack);//磨损
			}

			DoOrdinSkill(pSkill, nX, nY);
		}	
		else
		{
			//技能丢目标
			//SetTarget(type_npc, 0);
			//m_nPeopleIdx = 0;
			//m_nObjectIdx = 0;
			DoStand();
		}
#else

		if( IsPlayer() )
		{
			// 现在技能是严格同步，客户端能到达这里说明服务器已经
			// 检测通过了
 			/*if ( nSkillListCanCast 
 				 && pSkill->CanCastSkill(m_Index, nX, nY)
 				 && ( (m_Kind != kind_player && m_Kind != kind_creature) 
 					  || Cost(pSkill, TRUE) )
			   )//*/
			{
				int nParam1 = m_Command.Param_Y;
				int nParam2 = m_Command.Param_Z;

				if(SKILL_SPT_TargetIndex == nParam1)
					nParam2 = Npc[nParam2].m_dwID;
				
				//SendClientCmdSkill(m_Command.Param_X, nParam1, nParam2);
				DoOrdinSkill(pSkill, nX, nY);
			}
//			else
//				DoStand();
		}
		else
			DoOrdinSkill(pSkill, nX, nY);
		
#endif		

	}
	else
	{
		_ASSERT(pSkill);
	}
} 

bool KNpc::IsInWorldCombatInstance()
{
	if (m_SubWorldIndex != INVALID_WORLD_INDEX && m_SubWorldIndex < MAX_SUBWORLD)
	{
		return SubWorld[m_SubWorldIndex].IsWorldCombatMap();
	}//endif

	return false;
}


int KNpc::DoOrdinSkill(KSkill * pSkill, int nX, int nY)
{
	_ASSERT(pSkill);

#ifndef _SERVER		
	m_eActionType = pSkill->GeteActionType();
	m_nBeginFrame = pSkill->GeteActionBeginFrame();
	m_nEndFrame = pSkill->GeteActionEndFrame();
	m_DataRes.StopSound();
	int x, y, tx, ty;
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, GetMapX(), GetMapY(), GetOffX(), GetOffY(), &x, &y);
	
	if (nY < 0)
		return 0;
	
	if (nX < 0)
	{
		if (nX == SKILL_SPT_TargetIndex)
		{
			if (nY >= MAX_NPC || Npc[nY].m_dwID == 0 || Npc[nY].m_SubWorldIndex != m_SubWorldIndex)
				return 0;
			Npc[nY].GetMpsPos(&tx, &ty);
		}
		else
		{
			return 0;
		}
		
	}
	else
	{
		tx = nX;
		ty = nY;
	}
	
	m_SkillParam1 = nX;
	m_SkillParam2 = nY;
	m_DesX = nX;
	m_DesY = nY;
	
	// 解决对自己施法的时候老是面向观众的问题，在自己脚底下放技能不需要改变朝向
	if (!(pSkill->GetSkillStyle() == SKILL_SS_NearMultiAttack || pSkill->GetSkillStyle() == SKILL_SS_AddNearTrap))
	{
		int nPreDir = m_UnaryAttrMgr[nuai_dir];
		if (m_Kind != kind_building ) 
			m_UnaryAttrMgr.Set(nuai_dir, g_GetDirIndex(x, y, tx, ty));
		if(-1 == m_UnaryAttrMgr[nuai_dir])
			m_UnaryAttrMgr.Set(nuai_dir, nPreDir);
	}
	
	const char *pszEffectFile = pSkill->GetPreCastEffectFile();

	if (pszEffectFile && pszEffectFile[0])
		m_DataRes.SetSpecialSpr((char*)pszEffectFile);
	
	if (IsPlayer())
		pSkill->PlayPreCastSound(m_nSex,x, y);
	
#endif
	
	int ClientDoing = pSkill->GetActionType();
	
#ifndef _SERVER
	if (ClientDoing >= cdo_count) 
		m_ClientDoing = cdo_magic;
	else if (ClientDoing != cdo_none)
		m_ClientDoing = ClientDoing;
#endif
	
	if (pSkill->GetSkillStyle() == SKILL_SS_Melee)
	{
		if (CastMeleeSkill(pSkill) == FALSE)
		{
			SetTarget(type_npc, 0); // lixuewu
			//m_nPeopleIdx = 0;
			//m_nObjectIdx = 0;
			m_ProcessAI = 1;
			DoStand();
			
			return 1 ;
		}
	}
	//物理技能的技能释放时间与普通技能不同，一个是AttackFrame,一个是CastFrame
	else if (pSkill->IsPhysical())
	{
		if (ClientDoing == cdo_none) 
			m_Frames.nTotalFrame = 0;
		else
		{
			int nSpeedAdd1 = 100 + m_CompAttrMgr[ncai_attackspeed];
			int nSpeedAdd2 = 100 + 
				pSkill->GetCastSpeedEnhance() +
				m_SkillList.GetCastSpeedEnhance( pSkill->GetSkillId() );
				
			if(nSpeedAdd1 <= 0)
				nSpeedAdd1 = 100;

			if(nSpeedAdd2 <= 0)
				nSpeedAdd2 = 100;

			m_Frames.nTotalFrame = m_AttackFrame * 10000 / (nSpeedAdd1 * nSpeedAdd2); 

			// 最小为2桢，否则OnSkill过不了
			if(m_Frames.nTotalFrame < 2)
				m_Frames.nTotalFrame = 2;
		}
		m_Doing = do_attack;
	}
	else
	{
		if (ClientDoing == cdo_none) 
			m_Frames.nTotalFrame = 0;
		else
		{
			int nSpeedAdd1 = 100 + m_CompAttrMgr[ncai_castspeed];
			int nSpeedAdd2 = 100 + 
				pSkill->GetCastSpeedEnhance() + 
				m_SkillList.GetCastSpeedEnhance( pSkill->GetSkillId() );

			if(nSpeedAdd1 <= 0)
				nSpeedAdd1 = 100;

			if(nSpeedAdd2 <= 0)
				nSpeedAdd2 = 100;

			m_Frames.nTotalFrame = m_CastFrame * 10000 / (nSpeedAdd1 * nSpeedAdd2);

			// 最小为2桢，否则OnSkill过不了
			if(m_Frames.nTotalFrame < 2)
				m_Frames.nTotalFrame = 2;
		}

		if( SKILL_SS_Summon == pSkill->GetSkillStyle() )
		{
			m_Doing = do_summonskill;
		}
		else
		{
			m_Doing  = do_magic;
		}
	}
	
	m_ProcessAI = 0;
	m_Frames.nCurrentFrame = 0;	
	return 1;
}

BOOL	KNpc::CastMeleeSkill(KSkill * pSkill)
{
	BOOL bSuceess = FALSE;
	_ASSERT(pSkill);
	
	switch(pSkill->GetMeleeType())
	{
	case Melee_AttackWithBlur:
		{
			bSuceess = DoBlurAttack();
		}break;
	case Melee_RunAndAttack:
		{
			bSuceess = DoRunAttack();

		}break;
	case Melee_ManyAttack:
		{
			bSuceess = DoManyAttack();
		}break;
	default:
		m_ProcessAI = 1;
		break;
	}
	
	if (bSuceess)
	{
#ifdef _SERVER
		m_SkillList.CoolDown(pSkill->GetSkillId());
#endif
	}
	
	return bSuceess;
}

BOOL KNpc::DoBlurAttack()// DoSpecail1
{
	if (m_Doing == do_special1)
		return FALSE;
	
	KSkill * pSkill = GetActiveSkill();
	if (!pSkill) 
        return FALSE;
	
	_ASSERT(pSkill->GetSkillStyle() == SKILL_SS_Melee);

#ifndef _SERVER
		m_ClientDoing = pSkill->GetActionType();
		m_DataRes.SetBlur(TRUE);
#endif

	m_Frames.nTotalFrame = m_AttackFrame * 100 
			/ (100 + m_CompAttrMgr[ncai_attackspeed] * pSkill->GetCastSpeedEnhance() / 100);

	m_Frames.nCurrentFrame = 0;
	m_Doing = do_special1;
	return TRUE;
}

void KNpc::OnSkill()
{
	KSkill * pSkill = GetActiveSkill();

	if(NULL == pSkill)
		return;
	
	if (WaitForFrame() &&m_Frames.nTotalFrame != 0 )
	{
		DoStand();
		m_ProcessAI = 1;
	}
	else//*/
	if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif

		if (m_DesX == SKILL_SPT_TargetIndex) 
		{
			if (m_DesY <= 0) 
				goto Label_ProcessAI;
			
			//此时该角色已经无效时
			if (Npc[m_DesY].m_RegionIndex < 0) 
				goto Label_ProcessAI;
		}
		
		pSkill->Cast(m_Index, m_DesX, m_DesY);
        
		// 增加全局冷却的逻辑
		// 移入CoolDown中
// 		if( IsPlayer() && g_SkillManager.IsCommonCoolDown(pSkill->GetSkillId()) )
// 		{
// 			int nSkillSeries = m_Series * role_skillseries_count + Player[m_nPlayerIdx].GetSkillSeries();
// 			_ASSERT(nSkillSeries >= 0 && nSkillSeries <= 5);
// 			m_SkillList.SetComCoolTime(
// 				ConfigManager::Singleton().GetGlobalVariable((enumGlobalVariable)(global_var_commoncooldown_interval_1+nSkillSeries))
// 				);
// 		}

#ifdef _SERVER
		m_SkillList.CoolDown(m_nRealActiveSkillIdForCD);
#endif
		
Label_ProcessAI:
		if (m_Frames.nTotalFrame == 0)
		{
			m_ProcessAI = 1;
		}
	}
}

void KNpc::RunTo(int nMpsX, int nMpsY)
{
	if (NewPath(nMpsX, nMpsY))
		DoRun();
}

void KNpc::Goto(int nMpsX, int nMpsY)
{
	if (NewPath(nMpsX, nMpsY))
		DoWalk();
}

void KNpc::DoWalk()
{
	// Add by chenshanglin on [2006-3-15 11:57]
	if(0 == m_CompAttrMgr[ncai_runspeed] && 0 == m_CompAttrMgr[ncai_walkspeed])
	{
		return;
	}
	// Add end
	
	_ASSERT(m_RegionIndex >= 0);

	int nCurWalkSpeed = m_CompAttrMgr[ncai_walkspeed];

	if (nCurWalkSpeed)
		m_Frames.nTotalFrame = (m_WalkFrame * m_CompAttrMgr[ncai_walkspeed][idx_base_value]) / nCurWalkSpeed + 1;
	else
		m_Frames.nTotalFrame = m_WalkFrame;
	
#ifdef _SERVER
	NPC_WALK_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcwalk;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = m_DesX;
	NetCommand.nMpsY = m_DesY;

	if (IsVisibleToPlayer())
	{
		int nMaxCount = MAX_BROADCAST_COUNT_OPTIMIZED;
		BROADCAST_REGION(&NetCommand, sizeof(NetCommand), nMaxCount);
	}
	else
	{
		if (IsPlayer() && g_pServer != NULL)
			g_pServer->PackDataToClient(Player[m_nPlayerIdx].GetNetConnectIdx(), (BYTE*)&NetCommand, sizeof(NetCommand));
	}
#endif

	if (m_Doing == do_walk)
	{
		return;
	}
	m_Doing = do_walk;
	m_Frames.nCurrentFrame = 0;
}

void KNpc::OnWalk()
{
#ifndef	_SERVER
	// 处理客户端的动画换帧等……
#endif
	WaitForFrame();
	ServeMove(m_CompAttrMgr[ncai_walkspeed]);
}

int	KNpc::GetSkillLevel(int nSkillId)
{
	return 0;
//	int nIndex = m_SkillList.FindSame(nSkillId);
//	if (nIndex)
//	{
//		return m_SkillList.m_Skills[nIndex].SkillLevel;
//	}
//	else
//	{
//		return 0;
//	}
}

#ifdef _SERVER
BOOL KNpc::ReceiveDamage(int nLauncher, int skillId, const OutputDamageInfo *pDamage, int nDoHurt, bool isCrit)
{
	KNpc& launcherNpc = Npc[nLauncher];

	int	nTotalManaDamage = 0;
	int	nTotalLifeDamage = 0;

	for(int i = 0; i < dot_end; ++i)
	{
		if(dtt_life == pDamage[i].nTargetType)
			nTotalLifeDamage += pDamage[i].nVal;
		else
			nTotalManaDamage += pDamage[i].nVal;
	}

	//等级伤害加成
	if ( (launcherNpc.m_Kind != kind_player && launcherNpc.m_Kind != kind_creature && launcherNpc.m_Kind != kind_employee) && (m_Kind == kind_player || m_Kind == kind_creature || m_Kind == kind_employee ) )
	{		
		int nleveldifferent = 0, nFlevel = 0, a3 = 0, a4 = 0;
		nleveldifferent = launcherNpc.m_Level - m_Level;
		nFlevel = ConfigManager::Singleton().GetGlobalVariable( global_var_npc_level_different );
		a3 =  ConfigManager::Singleton().GetGlobalVariable( global_var_npc_level_hurt_value1 );
		a4 =  ConfigManager::Singleton().GetGlobalVariable( global_var_npc_level_hurt_value2 );
		if ( nleveldifferent > nFlevel )
		{
			nTotalLifeDamage = nTotalLifeDamage * ((nleveldifferent - nFlevel)* a3 + a4) / 100;
			nTotalManaDamage = nTotalManaDamage * ((nleveldifferent - nFlevel)* a3 + a4) / 100;
		}
	}

	// 注意伤害有可能是加血，有正的伤害的时候才做受伤动作和经验计算
	if( nTotalLifeDamage > 0 || nTotalManaDamage > 0 )
	{
		if (GetController().IsActive() && Npc[nLauncher].IsVisibleToNpc())
		{
			GetController().GetThreatMonitor().ChangeEnemyThreat(Npc[nLauncher].GetId(), nLauncher, nTotalLifeDamage + nTotalManaDamage);
		}

		if (IsPlayer())
		{
			KPlayer& player = Player[m_nPlayerIdx];
			player.GetItemList().Abrade(abrade_defend);//装备磨损
			player.GetActionDelayer().OnEvent(delayed_action_event_hurt);
		}

		if( g_RandPercent(nDoHurt) )
		{
			DoHurt();
		}
		
		int launcherKind = launcherNpc.GetKind();
		if(launcherKind == kind_player || launcherKind == kind_creature || launcherKind == kind_employee)
		{
			int nTotalDmg = 0;
			nTotalDmg += nTotalLifeDamage < m_UnaryAttrMgr[nuai_curlife] ? nTotalLifeDamage : m_UnaryAttrMgr[nuai_curlife];
			nTotalDmg += nTotalManaDamage < m_UnaryAttrMgr[nuai_curmana] ? nTotalManaDamage : m_UnaryAttrMgr[nuai_curmana];
		}
	}

	//filter blood event
	//===================================================
	BuffMgr& BM = BuffMgr::Singleton( );
	BUFF_ENV_PARAM Env;
	Env.nEventSender	=	nLauncher;
	Env.nEventRecever	=	m_Index;
	Env.nEventType		=	buff_event_type_blood;
	Env.nEventFormat	=	buff_event_format_blood;
	Env.nEventRelation	=	buff_event_relation_recver;
	Env.nEventValue		=	nTotalLifeDamage;
	BM.FilterEvent( Env );
	nTotalLifeDamage	=	Env.nEventValue;
	//===================================================	

	//filter mana event
	//===================================================
	Env.nEventSender	=	nLauncher;
	Env.nEventRecever	=	m_Index;
	Env.nEventType		=	buff_event_type_mana;
	Env.nEventFormat	=	buff_event_format_mana;
	Env.nEventRelation	=	buff_event_relation_recver;
	Env.nEventValue		=	nTotalManaDamage;
	BM.FilterEvent( Env );
	nTotalManaDamage	=	Env.nEventValue;
	//===================================================	

	//Special NPC Type ,which wanna keep its life percentage at m_LifeLimitedHoldPercentage
	if (m_LifeLimitedHoldPercentage > -1)
	{
		int iLifePercentage = 0;
		int iCurrentLife    = m_UnaryAttrMgr[nuai_curlife];
		int iMaxLife        = m_CompAttrMgr[ncai_lifeuplimit];
		if (iMaxLife > 0)
		{
			iLifePercentage = (iCurrentLife - nTotalLifeDamage) * 100 / iMaxLife;
			if (iLifePercentage < m_LifeLimitedHoldPercentage)
			{
				nTotalLifeDamage = iCurrentLife - iMaxLife * m_LifeLimitedHoldPercentage / 100;
			}//endif

		}//endif

	}//endif
	
	//雇佣兵伤害过高日志
	static DWORD s_nextLogEmployeeDamageTime = 0;
	int unexpectedDamage = ConfigManager::Singleton().GetGlobalVariable(global_var_employee_unexpected_damage);
	if (unexpectedDamage == 0)
		unexpectedDamage = 1500;//默认过高伤害
	if (s_nextLogEmployeeDamageTime < UNIX_TMIE_STAMP && nTotalLifeDamage >= unexpectedDamage && launcherNpc.IsEmployee())
	{
		s_nextLogEmployeeDamageTime = UNIX_TMIE_STAMP + 60;

		const int MAX_LOG_BUFF_COUNT = 20;
		_BuffPairExt BP[MAX_LOG_BUFF_COUNT];
		int nCount = MAX_LOG_BUFF_COUNT;
		BuffMgr::Singleton().GetAllBuffID(nLauncher, BP, nCount);
		if (nCount > 0)
		{
			char logDesc[1024] = { 0 };

			char employerName[32] = { 0 };
			int employerPlayerIndex = launcherNpc.GetEmployerIdx();
			if (IsValidPlayer(employerPlayerIndex))
			{
				strncpy(employerName, Player[employerPlayerIndex].GetPlayerName(), sizeof(employerName));
				employerName[sizeof(employerName) - 1] = 0;
			}

			snprintf(logDesc, sizeof(logDesc), "Employee:%s,Employer:%s,Target:%s,Damage:%d,BuffList:", launcherNpc.Name, employerName, Name, nTotalLifeDamage);

			char buffDesc[32] = { 0 };
			for (int buffIndex = 0; buffIndex < nCount; buffIndex++)
			{
				_BuffPairExt& buffInfo = BP[buffIndex];
				if (buffInfo.nBuffPileCount == 1)
				{
					snprintf(buffDesc, sizeof(buffDesc), "%d,", buffInfo.ulBuffTempID);
				}
				else if (buffInfo.nBuffPileCount > 1)
				{
					snprintf(buffDesc, sizeof(buffDesc), "%d*%d,", buffInfo.ulBuffTempID, buffInfo.nBuffPileCount);
				}
				buffDesc[sizeof(buffDesc) - 1] = 0;
				strncat(logDesc, buffDesc, sizeof(logDesc));
			}

			logDesc[sizeof(logDesc) - 1] = 0;
			if (g_pLogSystem)
				g_pLogSystem->SysDbgLog(logDesc, strlen(logDesc), sys_dbg_log_event_employee_unexpected_damage);
		}
	}

	if(nTotalLifeDamage > 0)
		SyncDamageInfo(nLauncher, nTotalLifeDamage, COMBAT_INFO_DAMAGE_LIFE, skillId, isCrit);
	else if(nTotalLifeDamage < 0)
		SyncDamageInfo(nLauncher, -nTotalLifeDamage, COMBAT_INFO_HEAL_LIFE, skillId, isCrit);

	if (nTotalManaDamage > 0)
		SyncDamageInfo(nLauncher, nTotalManaDamage, COMBAT_INFO_DAMAGE_MANA, skillId, isCrit);
	else if (nTotalManaDamage < 0)
		SyncDamageInfo(nLauncher, -nTotalManaDamage, COMBAT_INFO_HEAL_MANA, skillId, isCrit);

	AddUnaryAttr(nuai_curmana, -nTotalManaDamage);

	if(m_UnaryAttrMgr[nuai_curmana] < 0)
		m_UnaryAttrMgr.Set(nuai_curmana, 0);
	else if(m_UnaryAttrMgr[nuai_curmana] > m_CompAttrMgr[ncai_manauplimit])
		m_UnaryAttrMgr.Set(nuai_curmana, m_CompAttrMgr[ncai_manauplimit]);
	
	AddUnaryAttr(nuai_curlife, -nTotalLifeDamage);

	if(m_UnaryAttrMgr[nuai_curlife] <= 0)
	{
		m_UnaryAttrMgr.Set(nuai_curlife, 0);

		int pkValueAdd = 0;
		if( nTotalLifeDamage > 0 )
			pkValueAdd = DeathCalcPKValue(nLauncher);

		m_nLastDamageIdx = nLauncher;
		
		if (IsValidNpc(nLauncher))
		{
			KNpc& killer = Npc[nLauncher];
			
			if (killer.m_Kind == kind_player || killer.m_Kind == kind_creature || killer.m_Kind == kind_employee)
			{
				if (IsInWorldCombatInstance() && killer.IsInWorldCombatInstance())
				{
					int   nCombatType = killer.GetCombatScoreCalcType();

					if (SubWorld[m_SubWorldIndex].CanGainScore() && nCombatType == PROGRAME_CALU) //远古战场
						WorldCombatScoreKilledCulc(nLauncher);
					else //其他战场
					{

					}
				}

			}//endif

			if (GetKind() == kind_player)
			{
				if (IsValidPlayer(GetPlayerIdx()))
				{
					KPlayer& selfPlayer = Player[GetPlayerIdx()];
					selfPlayer.GetFurySys().CleanFuryExp();

					if (killer.GetKind() == kind_creature || killer.GetKind() == kind_player || killer.GetKind() == kind_employee)
					{
						int killerPlayerIndex = killer.GetPlayerIdx();
						if (killer.GetKind() == kind_creature)
							killerPlayerIndex = killer.GetSummonerIdx();
						else if (killer.GetKind() == kind_employee)
							killerPlayerIndex = killer.GetEmployerIdx();
						
						if (IsValidPlayer(killerPlayerIndex))
						{
							KPlayer& killerPlayer = Player[killerPlayerIndex];
                            //聊天：记录仇人
							
							ChatObjectMgr_S * pSelfChatObj   = g_ChatCenterS.GetChatObjMgr(selfPlayer.GetPlayerIndex());
                            if (pSelfChatObj && !(IsInEspecialArea(especial_area_pk) || IsInEspecialArea(especial_area_war) || IsInEspecialArea(especial_area_city)))
							{
								pSelfChatObj->AddFriendToGroup(killerPlayerIndex,GROUPID_ENEMY);
							//	pSelfChatObj->ObjPkValueChangeNotify(Player[killerPlayerIndex].GetPlayerName());
							}//endif
							
							//日志：玩家杀死玩家
							LogEventParam playerDeathEvent;
							playerDeathEvent.event = log_event_player_death;
							playerDeathEvent.param1 = selfPlayer.GetGUID();
							playerDeathEvent.param2 = killerPlayer.GetGUID();
							snprintf(playerDeathEvent.param3.data, sizeof(playerDeathEvent.param3.data), "%d", skillId);
							playerDeathEvent.param4 = pkValueAdd;
							g_pLogSystem->Log(playerDeathEvent);
						}
					}
					else
					{
						//统计：NPC杀死玩家
						if (m_pTemplate)
							m_pTemplate->GetStatisticInfo().KillPlayer();
					}
				}
			}	
			else if (GetKind() != kind_creature && GetKind() != kind_employee)
			{
				if (killer.GetKind() == kind_creature || killer.GetKind() == kind_player || killer.GetKind() == kind_employee)
				{
					//统计：NPC被玩家杀死
					if (m_pTemplate)
							m_pTemplate->GetStatisticInfo().KillByPlayer();
				}
			}
		}

		DoDeath( TRUE );
	}
	else if(m_UnaryAttrMgr[nuai_curlife] > m_CompAttrMgr[ncai_lifeuplimit])
	{
		m_UnaryAttrMgr.Set(nuai_curlife, m_CompAttrMgr[ncai_lifeuplimit]);
	}

	return TRUE;
}

void KNpc::SetCamoflag(const int bSet)
{
	int nOldFlag = m_UnaryAttrMgr[nuai_camou_flage];
	if (nOldFlag != bSet)
	{
		m_UnaryAttrMgr.Set(nuai_camou_flage,bSet);
		SyncCommoFlagInfo();
	}//endif
}

int  KNpc::GetComoflag()
{
	return m_UnaryAttrMgr[nuai_camou_flage];
}

void KNpc::WorldCombatScoreKilledCulc( int nKiller)
{
	if (IsValidNpc(nKiller))
	{
		if(Npc[nKiller].m_Kind == kind_creature) 
			nKiller = Npc[nKiller].GetSummonerIdx();
		else if(Npc[nKiller].m_Kind == kind_employee)
			nKiller = Npc[nKiller].GetEmployerIdx();
		
		if (IsValidNpc(nKiller) && m_WorldCombatOrg != Npc[nKiller].m_WorldCombatOrg)
		{
			int nScoreGet = 0;
			if (m_Kind == kind_player)
			{
				nScoreGet =  KWorldCombatSetting::Singleton().GetKillScoreByLevel(GetLevel());
			}//endif
			else 
			{
				nScoreGet =  m_WorldCombatKilledScore;
			}//end else
			
			if (nScoreGet > 0)
			{
				int nPlayerIndex = Npc[nKiller].GetPlayerIdx();
				if (IsValidPlayer(nPlayerIndex))
				{
					Player[nPlayerIndex].AddCombatScore(nScoreGet , log_event_combat_score_add_own );
					
					ConfigManager & mgr = ConfigManager::Singleton();
					int nShareRate      = mgr.GetGlobalVariable(global_var_war_score_share_rate);
					int nSharedScore    = nShareRate * nScoreGet / 100;
					
					if (nSharedScore)
					{
						if (m_SubWorldIndex != INVALID_WORLD_INDEX && m_SubWorldIndex < MAX_SUBWORLD)
						{
							KRegion &CurRegion = SubWorld[m_SubWorldIndex].m_Region[Npc[nKiller].m_RegionIndex];
							KIndexNode *pNode = (KIndexNode *)CurRegion.m_NpcList.GetHead();
							
							// -1 表示不用考虑目标数量
							while( pNode )
							{
								int nNpcIdx = pNode->m_nIndex;
								
								if (nNpcIdx != m_Index && nNpcIdx != nKiller && IsValidNpc(nNpcIdx) && Npc[nNpcIdx].m_Kind == kind_player )
								{
									int nCastPlayer = Npc[nNpcIdx].GetPlayerIdx();
									if (IsValidPlayer(nCastPlayer) && Npc[nNpcIdx].m_WorldCombatOrg == Npc[nKiller].m_WorldCombatOrg )
									{
										Player[nCastPlayer].AddCombatScore(nSharedScore , log_event_combat_score_add_share);
									}//endif
									
								}//endif
								
								pNode       = (KIndexNode*)pNode->GetNext();
							}//end for while
							
						}//endif
						
					}//endif
				}
			}//endif
		}
	}
}

#endif

#define POS_EDITION_PERCENT 40

void KNpc::ServeMove(int MoveSpeed)
{
	if (m_Doing != do_walk && m_Doing != do_run && m_Doing != do_hurt && m_Doing != do_runattack)
		return;

#ifdef _SERVER
	if (IsPlayer())
	{
		Player[GetPlayerIdx()].GetActionDelayer().OnEvent(delayed_action_event_move);
	}
#endif

	if (m_nMorphType >= 0 && m_uMoveSpeed > 0)
	{
		MoveSpeed = m_uMoveSpeed;
	}

	if (MoveSpeed <= 0)
		return;

#ifndef _SERVER

	if (m_IsPosEditionActive)
	{
		MoveSpeed=MoveSpeed * (100+POS_EDITION_PERCENT)/100;
	}//endif

	if (m_RegionIndex < 0 || m_RegionIndex >= 9)
	{
		g_DebugLog("[zroc]Npc(%d)ServerMove RegionIdx = %d", m_Index, m_RegionIndex);
		_ASSERT(0);
		DoStand();
		return;
	}
#else
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;
#endif

	if (MoveSpeed >= REGION_CELL_SIZE_X)
	{
		MoveSpeed = REGION_CELL_SIZE_X - 1;
	}

	int x, y;
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, GetMapX(), GetMapY(), 0, 0, &x, &y);
	x = (x << 10) + GetOffX();
	y = (y << 10) + GetOffY();

	if ( !m_bClientOnly && m_Kind == kind_normal)
	{
		CURREGION.DecNpcRef(m_Index);
	}

	// Add by Cooler -->
	// 2005-7-18
	int nStopOK = 0;
	// End add by Cooler <--
	int nDir = m_UnaryAttrMgr[nuai_dir];
	
#ifndef _SERVER
	int nRet = m_PathFinder.GetDir(x, y, m_UnaryAttrMgr[nuai_dir], m_DesX, m_DesY, (MoveSpeed - 1.5f) * KSubWorldSet::s_fScale, &nDir, &nStopOK);
#else
	int nRet = m_PathFinder.GetDir(x, y, m_UnaryAttrMgr[nuai_dir], m_DesX, m_DesY, MoveSpeed, &nDir, &nStopOK);
#endif
	


	m_UnaryAttrMgr.Set(nuai_dir, nDir);

	if (!m_bClientOnly && m_Kind == kind_normal)
	{
		CURREGION.AddNpcRef( m_Index);
	}
#ifndef _SERVER
	if(nRet == 1)
	{
		x = (float)((float)g_DirCos(m_UnaryAttrMgr[nuai_dir], 64) * (MoveSpeed - 1.5f)) * KSubWorldSet::s_fScale;
		y = (float)((float)g_DirSin(m_UnaryAttrMgr[nuai_dir], 64) * (MoveSpeed - 1.5f)) * KSubWorldSet::s_fScale;
		//x = g_DirCos(m_UnaryAttrMgr[nuai_dir], 64) * MoveSpeed;
		//y = g_DirSin(m_UnaryAttrMgr[nuai_dir], 64) * MoveSpeed;
		if (x >= ((REGION_CELL_SIZE_X) << 10))
		{
			x = (((REGION_CELL_SIZE_X) -1) << 10);
		}
		if (y >= ((REGION_CELL_SIZE_Y) << 10))
		{
			y = (((REGION_CELL_SIZE_Y) -1) << 10);
		}
	}
	else if (nRet == 0)
	{
		DoStand();

		// Add by Cooler -->
		// 2006-12-26 15:59
		if(IsPlayer())
		{
			//SendServerStopCmd();
			SendC2SPosSync();
		}
		// End add by Cooler <--

		if (m_IsPosEditionActive)
			EndEditionState();

		return;
	}
	else if (nRet == -1)
	{
		SubWorld[0].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		m_RegionIndex = -1;
		return;
	}
	else
	{
		return;
	}
#else
	if(nRet == 1)
	{
		x = g_DirCos(m_UnaryAttrMgr[nuai_dir], 64) * MoveSpeed;
		y = g_DirSin(m_UnaryAttrMgr[nuai_dir], 64) * MoveSpeed;
		if (x >= ((REGION_CELL_SIZE_X) << 10))
		{
			x = (((REGION_CELL_SIZE_X) -1) << 10);
		}
		if (y >= ((REGION_CELL_SIZE_Y) << 10))
		{
			y = (((REGION_CELL_SIZE_Y) -1) << 10);
		}
	}
	else
	{
		DoStand();
   
		// Add by Cooler -->
		// 2005-7-10
		if(nStopOK != 1)
		{
			if(!IsPlayer())
			{
				int nMpsXX, nMpsYY;
				GetMpsPos(&nMpsXX, &nMpsYY);
				NPC_RUN_SYNC	NetCommand;
				NetCommand.ProtocolType = (BYTE)s2c_npcrun;
				NetCommand.ID = m_dwID;
				NetCommand.nMpsX = nMpsXX;
				NetCommand.nMpsY = nMpsYY;

				int nMaxCount = MAX_BROADCAST_COUNT_OPTIMIZED;
				BROADCAST_REGION(&NetCommand, sizeof(NetCommand), nMaxCount);
			}
			
		}
		else
		{
            if (IsPlayer())  
				SendClientPosEdition();
		}
		// End add by Cooler <--

		/*
		if (IsPlayer())
		{
			SendClientStopCmd();
		}//endif
        */

		return;
	}
#endif

	int nNewOffX = GetOffX() + x;
	int nNewOffY = GetOffY() + y;

	int nNewMapX = GetMapX();
	int nNewMapY = GetMapY();
	int nNewRegion = m_RegionIndex;

	//	处理NPC的坐标变幻
	//	CELLWIDTH、CELLHEIGHT、OffX、OffY均是放大了1024倍	
	if (nNewOffX < 0)
	{
		nNewMapX--;
		nNewOffX += CELLWIDTH;
	}
	else if (nNewOffX > CELLWIDTH)
	{
		nNewMapX++;
		nNewOffX -= CELLWIDTH;
	}

	if (nNewOffY < 0)
	{
		nNewMapY--;
		nNewOffY += CELLHEIGHT;
	}
	else if (nNewOffY > CELLHEIGHT)
	{
		nNewMapY++;
		nNewOffY -= CELLHEIGHT;
	}


	//Lucifer~yu(zhangjianyu) 09/25/2007 Modify
	//Begin-------------------------------------------------------------------
	bool bTo = false;
	if ( nNewMapX < 0 && nNewMapY < 0  )
	{
		nNewMapX += REGIONWIDTH;
		nNewMapY += REGIONHEIGHT;
		nNewRegion = LEFTUPREGIONIDX;
		bTo = true;
	}

	if ( nNewMapX >= REGIONWIDTH && nNewMapY < 0  )
	{
		nNewMapX -= REGIONWIDTH;
		nNewMapY += REGIONHEIGHT;
		nNewRegion = RIGHTUPREGIONIDX;
		bTo = true;
	}

	if ( nNewMapX < 0 && nNewMapY >= REGIONHEIGHT  )
	{
		nNewMapX += REGIONWIDTH;
		nNewMapY -= REGIONHEIGHT;
		nNewRegion = LEFTDOWNREGIONIDX;
		bTo = true;
	}

	if ( nNewMapX >= REGIONWIDTH && nNewMapY >= REGIONHEIGHT  )
	{
		nNewMapX -= REGIONWIDTH;
		nNewMapY -= REGIONHEIGHT;
		nNewRegion = RIGHTDOWNREGIONIDX;
		bTo = true;
	}
		
	if (nNewRegion >= 0 && bTo )
	{
		// 如果所搜寻到的目标位置已经是阻挡则停止
		if (SubWorld[m_SubWorldIndex].TestBarrierMin(nNewRegion, nNewMapX, nNewMapY, nNewOffX, nNewOffY, 0, 0, FALSE) > 0)
		{
			DoStand();
               
            #ifndef _SERVER
			if (IsPlayer())
			{
				//SendServerStopCmd();
				SendC2SPosSync();
			}//endif

			if (m_IsPosEditionActive)
				EndEditionState();

            #else

			if (IsPlayer())
			{
				SendClientStopCmd();
				SendClientPosEdition();
			}

            #endif
			
			return;
		}

		MoveNpc(nNewRegion, nNewMapX, nNewMapY, nNewOffX, nNewOffY);
		return;
	}
	//End---------------------------------------------------------------------
	
	if (nNewMapX < 0)
	{
		nNewRegion = LEFTREGIONIDX;
		nNewMapX += REGIONWIDTH;
	}
	else if (nNewMapX >= REGIONWIDTH)
	{
		nNewRegion = RIGHTREGIONIDX;
		nNewMapX -= REGIONWIDTH;
	}

	if (nNewRegion >= 0)
	{
		if (nNewMapY < 0)
		{
			nNewRegion = UPREGIONIDX;
			nNewMapY += REGIONHEIGHT;
		}
		else if (nNewMapY >= REGIONHEIGHT)
		{
			nNewRegion = DOWNREGIONIDX;
			nNewMapY -= REGIONHEIGHT;
		}

		if (nNewRegion >= 0)
		{
			// 如果所搜寻到的目标位置已经是阻挡则停止
			if (SubWorld[m_SubWorldIndex].TestBarrierMin(nNewRegion, nNewMapX, nNewMapY, nNewOffX, nNewOffY, 0, 0, FALSE) > 0)
			{
				DoStand();
                
                #ifndef _SERVER
				if (IsPlayer())
				{
					//SendServerStopCmd();
					SendC2SPosSync();
				}//endif

				if (m_IsPosEditionActive)
					EndEditionState();

                #else

				if (IsPlayer())
				{
					SendClientStopCmd();
					SendClientPosEdition();
				}

                #endif
				
				return;
			}

			MoveNpc(nNewRegion, nNewMapX, nNewMapY, nNewOffX, nNewOffY);
		}
	}
}

#ifdef _SERVER

#define MIN_DISTANCE_POS_TO_VECTOR 16384  //128*128
#define MIN_DISTANCE_POS_TO_POS    0      //16*16
#define MAX_DISTANCE_POS_TO_POS    4096   //64*64
#define MAX_FRAME_EDIT_DISTANCE    512    //512*512

#define PlayerRecordVector() \
        m_LastRunDX = nDesX-nCurX;\
        m_LastRunDY = nDesY-nCurY;\
        m_LastDxExp2 = m_LastRunDX * m_LastRunDX;\
		m_LastDyExp2 = m_LastRunDY * m_LastRunDY


bool KNpc::CheckClientRunPos(int nCurX,int nCurY,int nDesX,int nDesY)
{
	if (IsDeath())
		return false;

    #ifdef _DEBUG
	m_ClientCheckedTimes+=1;
    #endif

	if (m_LastEditFrame==0)
	{
        m_LastEditFrame=SubWorld[m_SubWorldIndex].m_dwCurrentTime;
		#ifdef _DEBUG
        m_ServerPassTimes+=1;
        #endif
        return false;
	}//endif

	//Get current server pos
	int    nSerX=0;
	int    nSerY=0;
	GetMpsPos(&nSerX,&nSerY);

	int nCheckDisX=nSerX-nCurX ;
	int nCheckDisY=nSerY-nCurY ;
	int nFrameDis=SubWorld[m_SubWorldIndex].m_dwCurrentTime - m_LastEditFrame;
    int nSpeed = m_CompAttrMgr[ncai_runspeed];

	if (nFrameDis==0)
		nFrameDis=1;

	if ( abs(nCheckDisX) < (2* nSpeed * nFrameDis) && abs(nCheckDisY) < (2* nSpeed * nFrameDis)
		&& abs(nCheckDisX < MAX_FRAME_EDIT_DISTANCE) && abs(nCheckDisY<MAX_FRAME_EDIT_DISTANCE)
		)
	{
        m_LastEditFrame=SubWorld[m_SubWorldIndex].m_dwCurrentTime;
		#ifdef _DEBUG
        m_ServerPassTimes+=1;
        #endif
		return true;
	}//endif

	return false;

    /* 
	if (
		m_LastDxExp2==0 && m_LastDyExp2==0 &&
		m_LastRunDX==0 && m_LastRunDY==0               //first run after last stand
	)
	{
		PlayerRecordVector();
		return false;
	}//endif
    */

/*	int nCurCSDX = nCurX-nSerX;
	int nCurCSDy = nCurY-nSerY;
	
	int nCurCSDX2 = nCurCSDX * nCurCSDX;
	int nCurCSDY2 = nCurCSDy * nCurCSDy;

	//1.check the server to the pos
	if (nCurCSDX2+nCurCSDY2 >MAX_DISTANCE_POS_TO_POS || nCurCSDX2+nCurCSDY2<MIN_DISTANCE_POS_TO_POS)
	{
        //PlayerRecordVector();
		return false;
	}
*/
	//PlayerRecordVector();
//	m_ServerPassTimes+=1;

/*
	//2.Check The pos distance to the vector
    int nDxAndDy2 = m_LastDxExp2 + m_LastDyExp2;

	int nDis2=nCurCSDX2+nCurCSDY2-(nCurCSDX2 * m_LastDxExp2) / nDxAndDy2 - (nCurCSDY2 * m_LastDyExp2) / nDxAndDy2 ;
    if (nDis2<=MIN_DISTANCE_POS_TO_VECTOR)
	{
        PlayerRecordVector();
		m_ServerPassTimes+=1;
		return true;
	}

	//Record the vector and exp2
	PlayerRecordVector();
    */
	//return true;
}

void KNpc::SendClientStopCmd()
{
	S2C_PLAYER_STOP syncstop;
	syncstop.ProtocolType = (BYTE)s2c_player_stop;

	if( g_pServer != NULL)
		g_pServer->PackDataToClient( Player[m_nPlayerIdx].GetNetConnectIdx(), (BYTE *)&syncstop, sizeof(syncstop));
}

void KNpc::SendClientPosEdition()
{   
	S2C_POS_EDITION syncpos;
	syncpos.ProtocolType = (BYTE)s2c_pos_edition;
	syncpos.nNpcID = m_dwID;
	GetMpsPos(&syncpos.nX,&syncpos.nY);

	int nMaxCount = MAX_BROADCAST_COUNT_MIN;
    BROADCAST_REGION(&syncpos, sizeof(syncpos), nMaxCount);
}

bool KNpc::CheckClientStandPos(int nCurX,int nCurY)
{
	if (IsDeath())
		return false;

    #ifdef _DEBUG
	m_ClientCheckedTimes+=1;
    #endif

	if (m_LastEditFrame==0)
	{
        m_LastEditFrame=SubWorld[m_SubWorldIndex].m_dwCurrentTime;
		#ifdef _DEBUG
        m_ServerPassTimes+=1;
        #endif
        return false;
	}//endif
	
	//Get current server pos
	int    nSerX=0;
	int    nSerY=0;
	GetMpsPos(&nSerX,&nSerY);
	
	
	int nCheckDisX=nSerX-nCurX ;
	int nCheckDisY=nSerY-nCurY ;
	int nFrameDis=SubWorld[m_SubWorldIndex].m_dwCurrentTime - m_LastEditFrame;

	if (nFrameDis==0)
		nFrameDis=1;

    int nSpeed = m_CompAttrMgr[ncai_runspeed];

	if ( abs(nCheckDisX) < (2* nSpeed * nFrameDis) && abs(nCheckDisY) < (2* nSpeed * nFrameDis)
		&& abs(nCheckDisX) < MAX_FRAME_EDIT_DISTANCE && abs(nCheckDisY<MAX_FRAME_EDIT_DISTANCE)
	)
	{
        m_LastEditFrame=SubWorld[m_SubWorldIndex].m_dwCurrentTime;
        #ifdef _DEBUG
        m_ServerPassTimes+=1;
        #endif
		return true;
	}//endif
	
	return false;

	/*
	//Get current server pos
	int    nSerX=0;
	int    nSerY=0;
	GetMpsPos(&nSerX,&nSerY);
	
	/*if (
		m_LastDxExp2==0 && m_LastDyExp2==0 &&
		m_LastRunDX==0 && m_LastRunDY==0               //first run after last stand
		)
	{
		return false;
	}//endif
	*/
    /*	
	int nCurCSDX = nCurX-nSerX;
	int nCurCSDy = nCurY-nSerY;
	
	int nCurCSDX2 = nCurCSDX * nCurCSDX;
	int nCurCSDY2 = nCurCSDy * nCurCSDy;
	
	//1.check the server to the pos
	if (nCurCSDX2+nCurCSDY2 >MAX_DISTANCE_POS_TO_POS || nCurCSDX2+nCurCSDY2<MIN_DISTANCE_POS_TO_POS)
	{
		return false;
	}
	*/
	//m_ServerPassTimes+=1;

	//2.Check The pos distance to the vector
 /*  int nDxAndDy2 = m_LastDxExp2 + m_LastDyExp2;
	
	int nDis2=nCurCSDX2+nCurCSDY2-(nCurCSDX2 * m_LastDxExp2) / nDxAndDy2 - (nCurCSDY2 * m_LastDyExp2) / nDxAndDy2 ;
    if (nDis2<=MIN_DISTANCE_POS_TO_VECTOR)
	{
		m_ServerPassTimes+=1;
		return true;
	}

	return true;*/
}
#else

void KNpc::SendServerStopCmd()
{
	PLAYER_STOP_NOTIFY NetCommand;
	NetCommand.ProtocolType = c2s_playerstop;
	if ( g_pClient )
	{
		g_pClient->SendPackToServer(g_ConnectID, &NetCommand, sizeof(NetCommand));
	}
	
}

void KNpc::SendC2SPosSync()
{
	if (m_Doing != do_revive && m_Doing!=cdo_death)
	{
		C2S_POS_SYNC    NetCommand;
		SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, GetMapX(), GetMapY(), GetOffX(), GetOffY(), &NetCommand.nStopX, &NetCommand.nStopY);
		//这里取逻辑里的位置是为了消除在某些情况下逻辑和显示的位置不一致导致的问题
		//GetMpsPos(&NetCommand.nStopX,&NetCommand.nStopY);
		NetCommand.nDir=m_UnaryAttrMgr[nuai_dir];
		NetCommand.ProtocolType = c2s_player_pos_sync;
		
		if ( g_pClient )
		{
			g_pClient->SendPackToServer(g_ConnectID, &NetCommand, sizeof(NetCommand));
		}//endif
	}//endif
}

void KNpc::BeginEditionState()
{
	if (!m_IsPosEditionActive)
	{
		m_IsPosEditionActive = true;
//		m_CompAttrMgr.Set(ncai_runspeed, idx_append_percent, m_CompAttrMgr[ncai_runspeed][idx_append_value]+POS_EDITION_PERCENT );
	}//endif
}

void KNpc::EndEditionState()
{
	if (m_IsPosEditionActive)
	{
		m_IsPosEditionActive = false;
/*		if (m_CompAttrMgr[ncai_runspeed][idx_append_value]>=POS_EDITION_PERCENT)
			m_CompAttrMgr.Set(ncai_runspeed, idx_append_percent, m_CompAttrMgr[ncai_runspeed][idx_append_value]-POS_EDITION_PERCENT );
*/
	}//endif
}

void KNpc::SetChangeWorldFlag(const bool bChange)
{
    m_IsChangeWorld = bChange;
}

bool KNpc::GetChangeWorld(void)const
{
	return m_IsChangeWorld;
}
#endif

#define DIALOG_DISTANCE_HANDLE 256

void KNpc::SendCommand(NPCCMD cmd,int x,int y, int z)
{
#ifdef _SERVER
	if (IsPlayer())
	{
		// 防止其他动作将死亡流程给覆盖了
		// 因为buff_dodeath() 会在下一桢才会执行死亡流程
		// 所以在这之前不能把do_death的命令给覆盖了
		if(do_death == m_Command.CmdKind)
			return;

		ActionDelayer& actionDelayer = Player[GetPlayerIdx()].GetActionDelayer();
		switch(cmd)
		{
		case do_walk:
		case do_run:
			{
				int nLastDialogPosX = 0;
				int nLastDialogPosY = 0;
				int nLastDialogWorldId  = -1;

				GetDialogPos(nLastDialogWorldId,nLastDialogPosX,nLastDialogPosY);	

				int nCurrentPosX    = 0;
				int nCurrentPosY    = 0;
				int nCurrentWorldID = 0;

				GetMpsPos(&nCurrentPosX,&nCurrentPosY);

				int nWorldIndex = GetSubWorldIndex();
				if (nWorldIndex >= 0 && nWorldIndex < MAX_SUBWORLD)
				{
					nCurrentWorldID = SubWorld[nWorldIndex].m_SubWorldID;
				}//endif
				
				if ( nCurrentWorldID != nLastDialogWorldId 
					|| abs(nLastDialogPosX - nCurrentPosX) > DIALOG_DISTANCE_HANDLE
					|| abs(nLastDialogPosY - nCurrentPosY) > DIALOG_DISTANCE_HANDLE
					|| abs(nLastDialogPosX - x) > DIALOG_DISTANCE_HANDLE
					|| abs(nLastDialogPosY - y) > DIALOG_DISTANCE_HANDLE
					)
				{
					//开始跑动时清空商店状态
					Player[m_nPlayerIdx].m_BuyInfo.Clear();
					Player[m_nPlayerIdx].GetUIServerState().PlayerWalkNotify();
					RecordDialogPos(-1,0,0);
				}//endif

				//开始跑动时取消交易
				Player[m_nPlayerIdx].tradeServerDoCanceTrade();
			}
			break;
		case do_skill:
			actionDelayer.OnEvent(delayed_action_event_skill);
			m_LastEditFrame=0;
			break;
		}
	}
#else
	if (m_IsPosEditionActive)
	{
		int iCurX;
		int iCurY;
		GetMpsPos(&iCurX,&iCurY);
		
		int iOldDx=m_DesX-iCurX;
		int iOldDy=m_DesY-iCurY;
  
		int iNewDx=x-iCurX;
		int iNewDy=y-iCurY;

		if (cmd==do_run && iOldDx * iNewDx + iOldDy * iNewDy <= 0)
			EndEditionState();
	}//endif
#endif


	// Add by chenshanglin on [2006-3-10 10:11]
	// 如果速度为0，则禁止走以及跑的动作
	if((do_walk == cmd || do_run == cmd) &&
	   (m_CompAttrMgr[ncai_walkspeed] == 0 && m_CompAttrMgr[ncai_runspeed] == 0))
	{
		return;
	}
	// Add end

	m_Command.CmdKind = cmd;
	m_Command.Param_X = x;
	m_Command.Param_Y = y;
	m_Command.Param_Z = z;
}

BOOL KNpc::NewPath(int nMpsX, int nMpsY)
{
	m_DesX = nMpsX;
	m_DesY = nMpsY;
	return TRUE;
}

void KNpc::SelfDamage(int nDamage)
{
	AddUnaryAttr(nuai_curlife, -nDamage);

	if(m_UnaryAttrMgr[nuai_curlife] <= 0)
	{
		m_UnaryAttrMgr.Set(nuai_curlife, 1);
	}
}

// changed by chenshanglin on 2006-2-13 for new skill system
// 召唤兽发技能消耗主人的蓝
BOOL KNpc::Cost(KSkill *pSkill, BOOL bOnlyCheckCanCast, enumSkillUseableResult* pResult)
{
	enumSkillUseableResult skillResult = skill_useable_result_ok;

	_ASSERT(pSkill);
	if(NULL == pSkill)
	{
		if (pResult)
			*pResult = skill_useable_result_unknown;

		return FALSE;
	}

	int	nType = pSkill->GetSkillCostType();
	int	nCost = pSkill->GetSkillCost() + m_SkillList.GetPrivateCost( pSkill->GetSkillId() );	

	if(!IsPlayer() && m_Kind != kind_creature && m_Kind != kind_employee)
	{
		if (pResult)
			*pResult = skillResult;

		return TRUE;
	}

	int	 nIndex = m_Index;
	BOOL bCanCast = FALSE;

	if(m_Kind == kind_creature || m_Kind == kind_employee)
	{
#ifdef _SERVER
		if (m_Kind == kind_creature)
			nIndex = GetSummonerIdx();
		else if (m_Kind == kind_employee)
			nIndex = GetEmployerIdx();
#else
		nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
#endif
	}
	
	switch(nType)
	{
	case attrib_mana:
		bCanCast = Npc[nIndex].m_UnaryAttrMgr[nuai_curmana] >= nCost;
		if (!bCanCast)
			skillResult = skill_useable_result_no_mana;
		break;

	case attrib_life:
		bCanCast = Npc[nIndex].m_UnaryAttrMgr[nuai_curlife] > nCost;
		if (!bCanCast)
			skillResult = skill_useable_result_no_life;
		break;

	default:
		break;
	}

	if ( pSkill->GetCostSkillExp() > 0 && bCanCast )
	{
		bCanCast = Player[Npc[nIndex].m_nPlayerIdx].GetSkillExp() >= pSkill->GetCostSkillExp();
		if ( !bCanCast )
		{
			if (pResult)
				*pResult = skill_useable_result_no_skill_exp;

			return FALSE;
		}
	}	

	
	if (!bCanCast)
	{
		if (pResult)
			*pResult = skillResult;

		return FALSE;
	}
	else
	{
#ifdef _SERVER // lixuewu 2005.01.06 客户端不盲目的减少
		if (!bOnlyCheckCanCast)
		{
			switch(nType)
			{
			case attrib_mana:
				Npc[nIndex].AddUnaryAttr(nuai_curmana, -nCost);
				break;

			case attrib_life:
				Npc[nIndex].AddUnaryAttr(nuai_curlife, -nCost);
				break;
			}
		}
		if ( pSkill->GetCostSkillExp() > 0 )
		{
			Player[Npc[nIndex].m_nPlayerIdx].SetSkillExp( Player[Npc[nIndex].m_nPlayerIdx].GetSkillExp() - pSkill->GetCostSkillExp() );
			Player[Npc[nIndex].m_nPlayerIdx].SyncAttribute(attr_SkillExp);
		}	
#endif
		if (pResult)
			*pResult = skillResult;

		return TRUE;
	}
}

BOOL KNpc::WaitForFrame()
{
	m_Frames.nCurrentFrame++;
	if (m_Frames.nCurrentFrame < m_Frames.nTotalFrame)
	{
		return FALSE;
	}
	m_Frames.nCurrentFrame = 0;
	return TRUE;
}

BOOL KNpc::IsReachFrame(int nPercent)
{
	if (m_Frames.nCurrentFrame == m_Frames.nTotalFrame * nPercent / 100)
	{
		return TRUE;
	}
	return FALSE;
}

//客户端从网络得到的NpcSettingIdx是包含高16位Npc的模板号与低16位为等级
void KNpc::Load(int nNpcSettingIdx, int nLevel)
{
	m_PathFinder.Init(m_Index);
	if (nLevel <= 0) 
	{
		nLevel = 1;
	}

#ifndef _SERVER
	char	szNpcTypeName[32];
#endif

	if (nNpcSettingIdx < 0)
	{
		m_NpcSettingIdx = nNpcSettingIdx;
		m_Level = nLevel;
#ifndef _SERVER
		_ASSERT(m_NpcSettingIdx >= -6);
		strcpy(szNpcTypeName,NPCTEMPLATEID_TO_ROLENAME[-(m_NpcSettingIdx+1)]);
		//m_CorpseSettingIdx = -(m_NpcSettingIdx+1) + enumINSTANT_STATE_NUM;
		m_StandFrame = NpcSet.GetPlayerStandFrame(TRUE);
		m_WalkFrame = NpcSet.GetPlayerWalkFrame(TRUE);
		m_DataRes.m_uRoleType = -(nNpcSettingIdx + 1); // 配色lixuewu 
#endif
		//		TODO: Load Player Data;
		m_RunFrame = NpcSet.GetPlayerRunFrame(TRUE);
		m_CompAttrMgr.Set(ncai_walkspeed, idx_base_value, NpcSet.GetPlayerWalkSpeed());
		m_CompAttrMgr.Set(ncai_runspeed, idx_base_value, NpcSet.GetPlayerRunSpeed());
		m_AttackFrame = NpcSet.GetPlayerAttackFrame();
		m_CastFrame = NpcSet.GetPlayerCastFrame();
		m_HurtFrame	= NpcSet.GetPlayerHurtFrame();
	}
	else
	{
		GetNpcCopyFromTemplate(nNpcSettingIdx, nLevel);

#ifndef _SERVER	
		g_NpcSetting.GetString(nNpcSettingIdx + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));
		if (!szNpcTypeName[0])
		{
			g_NpcKindFile.GetString(2, ROLE_KIND, "", szNpcTypeName, sizeof(szNpcTypeName));
		}

		if (m_AiMode == 11 || m_AiMode == 12 || m_AiMode == 17)
			m_AiParam[6] = m_AiMode;
#endif
	}
#ifndef _SERVER
	m_DataRes.Init(szNpcTypeName, &g_NpcResList, nNpcSettingIdx);
	m_DataRes.SetPart(BODY_PART_HELM, m_HelmType, m_HelmPal);
	m_DataRes.SetPart(BODY_PART_ARMOR, m_ArmorType, m_ArmorPal);
	m_DataRes.SetPart(BODY_PART_WEAPON, m_WeaponType, m_WeaponPal);
	m_DataRes.SetPart(BODY_PART_SHOULDER, m_ShoulderType, m_ShoulderPal);
	m_DataRes.SetPart(BODY_PART_CUFF, m_CuffType, m_CuffPal);
	m_DataRes.SetPart(BODY_PART_BOOT, m_BootType, m_BootPal);
	m_DataRes.SetPart(BODY_PART_HORSE, m_HorseType, m_HorsePal);
	m_DataRes.SetRideHorse(m_bRideHorse);
	m_DataRes.SetAction(m_ClientDoing);
#endif
	m_CurrentCamp = m_Camp;
}

void KNpc::GetMpsPos(int *pPosX, int *pPosY) const
{
#ifdef _SERVER
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, GetMapX(), GetMapY(), GetOffX(), GetOffY(), pPosX, pPosY);
#else
	*pPosX = this->m_DataRes.m_nXpos;
	*pPosY = this->m_DataRes.m_nYpos;
//	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, pPosX, pPosY);
//	KSubWorld::Map2Mps(C_REGION_X(m_RegionIndex), C_REGION_Y(m_RegionIndex), m_MapX, m_MapY, m_OffX, m_OffY, pPosX, pPosY);
#endif
}

BOOL	KNpc::SetActiveSkill(int nSkillIdx, int nTarget)
{
	int nSkillId = m_SkillList.GetIdByIdx(nSkillIdx);
	int nCurLevel = m_SkillList.GetLevelByIdx(nSkillIdx);

#ifdef _SERVER
	// 记录原始的技能id，否则转换以后，无法cooldown
	m_nRealActiveSkillIdForCD = nSkillId;
#endif

#ifdef _SERVER
	//buff filter
	int nTransSkill = nSkillId;

	KSkill* pSkill = g_SkillManager.GetSkill( nSkillId, nCurLevel );

	if( pSkill != NULL )
	{
		//filter skill out
		//===================================================
		BuffMgr& BM = BuffMgr::Singleton( );
		BUFF_ENV_PARAM Env;
		Env.nEventSender	=	m_Index;
		Env.nEventRecever	=	
		pSkill->GetAttackTargetType( ) & att_target_only ? nTarget : 0 ;
		Env.nEventType		=	buff_event_type_skillout;
		Env.nEventFormat	=	buff_event_format_skillid;
		Env.nEventRelation	=	buff_event_relation_sender;
		Env.nEvent			=	nTransSkill;
		BM.FilterEvent( Env );
		nTransSkill			=	Env.nEvent;
		//===================================================
	}

	if( nTransSkill <= 0 )
	{
		return FALSE;
	}

	m_bTransSkill = ( nSkillId != nTransSkill );
	m_ActiveSkillID = nTransSkill;
	//
#else
	m_ActiveSkillID = nSkillId;
#endif

	if(0 >= nSkillId || nCurLevel <= 0)
	{
		return FALSE;
	}

	m_ActiveSkillLevel = nCurLevel;

	// 1.1 因为隐式技能不方便填写最大等级，实际上只用1级，这里防止技能转换
	//     导致等级不匹配，所以强制用1级
	// 1.2 但是现在召唤技能的隐式技能需要明确的等级来用于
	//     召唤兽等级的显示，所以取隐式技能的当前等级
	// 1.3 现在使用人物的等级来作为召唤兽的等级，所以隐式技能可以强制限制1级了
	//	   防止填表出错
	if( g_SkillManager.IsHiddenSkill(m_ActiveSkillID) )
	{
		m_ActiveSkillLevel = 1;
//		m_ActiveSkillLevel = g_SkillManager.GetSubSkillLevel(m_ActiveSkillID);
	}


	KSkill * pISkill =  g_SkillManager.GetSkill(m_ActiveSkillID, m_ActiveSkillLevel);

	if (pISkill)
    {
		m_CompAttrMgr.Set( ncai_attackradius, idx_base_value, pISkill->GetAttackRadius() );
    }

	return TRUE;
}

BOOL KNpc::SetPlayerIdx(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return FALSE;

	if (m_Kind != kind_player)
		return FALSE;

	m_nPlayerIdx = nIdx;
	return TRUE;
}

#ifdef _SERVER

#include "buff_man.h"

/*
带宽优化前
BOOL KNpc::SendSyncData(int nClient, int nNpcIndex )
{
	//如果是对玩家隐形，则不发送数据
	if (!IsVisibleToPlayer())
	{
		//即使隐形，也要给自己（是个玩家）或者自己的主人（是个召唤兽）发送数据
		if (!IsValidPlayer(m_nPlayerIdx) || Player[m_nPlayerIdx].GetNetConnectIdx() != nClient)
		{
			return FALSE;
		}
	}

	BOOL	bRet = FALSE;
	NPC_SYNC	NpcSync;

	NpcSync.ProtocolType		= (BYTE)s2c_syncnpc;
	NpcSync.m_btKind			= (BYTE)m_Kind;
	NpcSync.Camp				= (BYTE)m_Camp;
	NpcSync.CurrentCamp			= (BYTE)m_CurrentCamp;
	NpcSync.m_bySeries			= (BYTE)m_Series;
	NpcSync.m_Doing				= (BYTE)m_Doing;
	NpcSync.dwDoingParamX		= m_Command.Param_X;
	NpcSync.dwDoingParamY		= m_Command.Param_Y;
	NpcSync.dwDoingParamZ		= m_Command.Param_Z;
	NpcSync.dwNameColor			= (DWORD)m_UnaryAttrMgr[nuai_titlecolor];
	NpcSync.bFightState			= (TRUE == m_UnaryAttrMgr[nuai_fightstate]);
	NpcSync.byNpcDir			= m_UnaryAttrMgr[nuai_dir];
	NpcSync.TeamId				= m_UnaryAttrMgr[nuai_team_id];

	NpcSync.RunSpeed[0]		= m_CompAttrMgr[ncai_runspeed][idx_base_value];
	NpcSync.RunSpeed[1]		= m_CompAttrMgr[ncai_runspeed][idx_append_value];
	NpcSync.RunSpeed[2]		= m_CompAttrMgr[ncai_runspeed][idx_append_percent];

	//nNpcIndex 玩家
	NpcSync.nCondition		= NpcSet.GetSUCond( nNpcIndex, m_Index );

	// 告诉客户端这是谁的宝宝
	if (m_Kind == kind_creature)
	{
		const int nSummonerIdx = GetSummonerIdx();
		if (nSummonerIdx > 0  && nSummonerIdx < MAX_NPC )
		{
			NpcSync.nSummonID = Npc[nSummonerIdx].m_dwID;
		}
	}
	else if (m_Kind == kind_employee)
	{
		const int nEmployerIdx = GetEmployerIdx();
		if (nEmployerIdx > 0  && nEmployerIdx < MAX_NPC )
		{
			NpcSync.nSummonID = Npc[nEmployerIdx].m_dwID;
		}
	}

	NpcSync.dwLifeUpLimit = m_CompAttrMgr[ncai_lifeuplimit];
	NpcSync.dwManaUpLimti = m_CompAttrMgr[ncai_manauplimit];
	
	if(NpcSync.dwLifeUpLimit <= 0)
		NpcSync.dwLifeUpLimit = 1;
	if(NpcSync.dwManaUpLimti <= 0)
		NpcSync.dwManaUpLimti = 1;

	if (m_CompAttrMgr[ncai_lifeuplimit] > 0)
		NpcSync.LifePerCent		= (BYTE)((m_UnaryAttrMgr[nuai_curlife] << 7) / m_CompAttrMgr[ncai_lifeuplimit]);//Question只改了这部分，其它的也需要spe修改
	else
		NpcSync.LifePerCent		= 0;

	if (m_CompAttrMgr[ncai_manauplimit] > 0)
		NpcSync.ManaPercent		= (BYTE)((m_UnaryAttrMgr[nuai_curmana] << 7) / m_CompAttrMgr[ncai_manauplimit]);
	else
		NpcSync.ManaPercent		= 0;

	GetMpsPos((int *)&NpcSync.MapX, (int *)&NpcSync.MapY);
	NpcSync.ID					= m_dwID;
	NpcSync.NpcSettingIdx		= MAKELONG(m_Level, m_NpcSettingIdx);
	NpcSync.TalismanNpcId = m_EquipTalismanNpcId;

	strcpy(NpcSync.m_szName, Name);	

	NpcSync.CityId    = INVALID_WORLD_ID;
	NpcSync.IsKing    = 0;
	NpcSync.IsGensMgr = 0;

	memset(NpcSync.ZhuhouName, 0, sizeof(NpcSync.ZhuhouName));
	memset(NpcSync.ShizuName, 0, sizeof(NpcSync.ShizuName));
	memset(NpcSync.invaderTongName, 0, sizeof(NpcSync.invaderTongName));

	NpcSync.HeadImage  = m_nHeadImage;
	NpcSync.nCombatOrg = m_WorldCombatOrg;
	NpcSync.bCamouflage= (BYTE)m_UnaryAttrMgr[nuai_camou_flage];
	
	if (m_Kind == kind_player)
	{
		int playerIndex = GetPlayerIdx();
		if (IsValidPlayer(playerIndex))
		{
			NpcSync.nSkillType = Player[playerIndex].GetSkillSeries();

			SocialUnit *pUnit = GetLeafUnit(playerIndex, enSUTplId_Tong);
			if (pUnit != NULL)
			{
				SocialUnit *pShizu = GetUpNUnit(pUnit, enSULayer_Gens);
				if (pShizu != NULL)
				{
					const char* shizuName = GetUnitName(pShizu->GetUnitAttr());
					
					if (shizuName != NULL)
						strcpy(NpcSync.ShizuName, shizuName);

					if (pShizu->IsOwner(GetPlayerName(playerIndex)))
						NpcSync.IsGensMgr = 1;

				}

				SocialUnit *pZhuhou = GetUpNUnit(pUnit, enSULayer_Tong);
				if (pZhuhou != NULL)
				{
					NpcSync.CityId = GetCityMapId( pZhuhou->GetUnitAttr() );

					const char* pZhuhouName = GetUnitName(pZhuhou->GetUnitAttr());
					if (pZhuhouName != NULL)
						strcpy(NpcSync.ZhuhouName, pZhuhouName);
					
					if (pZhuhou->IsOwner(GetPlayerName(playerIndex)))
						NpcSync.IsKing = 1;
				
					KWarInfoManager &warMgr   = GetGlobalWarInfoManager();
					const FSGUID    *pInvader = warMgr.GetInvaderFromDefender( pZhuhou->GetUnitGuid() );
				
					if(NULL != pInvader)
					{
						SocialUnit * pInvaderUnit = ServerSocialUnitMgr::Singleton().GetUnit(*pInvader);
						if(NULL   != pInvaderUnit)
						{
							const char *szUnitName = GetUnitName( pInvaderUnit->GetUnitAttr() );
							
							if(NULL != szUnitName)
								strncpy(NpcSync.invaderTongName, szUnitName, sizeof(NpcSync.invaderTongName));
						}	
					}
				}				
			}
		}		
	}
	else
	{
		if (m_Lord.data[0]!=0)  //Valid GUID
		{
            SocialUnit * pUnit = ServerSocialUnitMgr::Singleton().GetUnit(m_Lord);
			if (pUnit)
			{
                SocialUnitAttr & attr  = pUnit->GetUnitAttr();
				const char *  szUnName = GetUnitName(attr);
				if (szUnName)
				{
                    switch(pUnit->GetLayer())
					{
					case enSULayer_Tong:
						{
                          strncpy(NpcSync.ZhuhouName,szUnName,sizeof(NpcSync.ZhuhouName));
						}//end for case
						break;

					case enSULayer_Gens:
						{
                          strncpy(NpcSync.ShizuName,szUnName,sizeof(NpcSync.ShizuName));
						}//end for case 
						break;
					}//end for switch

				}//endif

			}//endif

		}//endif

	}//end else

// 	else if (m_Kind == kind_creature)
// 	{
// 	}
// 	else
// 	{
// 		SocialUnit* pUnit = sum.GetUnit(GetLord(), enSUTplId_Tong);
// 	}
		
	NpcSync.m_wLength			= sizeof(NPC_SYNC) - 1;
	

	if (g_pServer != NULL && SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&NpcSync, NpcSync.m_wLength + 1)))
	{
		bRet = TRUE;
	}
	else
	{
		return FALSE;
	}

	if (IsPlayer() || IsEmployee())
	{
		PLAYER_SYNC	PlayerSync;

		PlayerSync.ProtocolType		= (BYTE)s2c_syncplayer;
		PlayerSync.ID				= m_dwID;
		PlayerSync.AttackSpeed		= (BYTE)m_CompAttrMgr[ncai_attackspeed][idx_base_value];
		PlayerSync.CastSpeed		= (BYTE)m_CompAttrMgr[ncai_castspeed][idx_base_value];
		PlayerSync.ArmorType		= (BYTE)(m_nMorphPart[itempart_armor]<0)?m_ArmorType:m_nMorphPart[itempart_armor];
		PlayerSync.HelmType			= (BYTE)(m_nMorphPart[itempart_helm]<0)?m_HelmType:m_nMorphPart[itempart_helm];
		PlayerSync.HorseType		= (BYTE)(m_nMorphPart[itempart_horse]<0)?m_HorseType:m_nMorphPart[itempart_horse];
		PlayerSync.WeaponType		= (BYTE)(m_nMorphPart[itempart_weapon]<0)?m_WeaponType:m_nMorphPart[itempart_weapon];
		PlayerSync.ShoulderType		= (BYTE)(m_nMorphPart[itempart_shoulder]<0)?m_ShoulderType:m_nMorphPart[itempart_shoulder];
		PlayerSync.CuffType			= (BYTE)(m_nMorphPart[itempart_cuff]<0)?m_CuffType:m_nMorphPart[itempart_cuff];
		PlayerSync.BootType			= (BYTE)(m_nMorphPart[itempart_boots]<0)?m_BootType:m_nMorphPart[itempart_boots];
		PlayerSync.RankID			= (BYTE)m_btRankId;
		PlayerSync.m_btSomeFlag		= 0;

		PlayerSync.m_wLength = sizeof(PLAYER_SYNC) - 1;

		if (g_pServer != NULL && SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&PlayerSync, PlayerSync.m_wLength + 1)))
		{
			bRet = TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	//buff sync
	#define MAX_SYNC_NPC_BUFF 50

	char szSendBuff[BUFF_TEMP_SIZE];
	_BuffPair	BP[MAX_SYNC_NPC_BUFF];
	BuffMgr   & BM = BuffMgr::Singleton( );
	int nCount = MAX_SYNC_NPC_BUFF;
	
	_Buff_Sync_Npc*	pBSN = (_Buff_Sync_Npc*)szSendBuff;
	pBSN->Protocol = s2c_buff_family;
	pBSN->ProtocolExtend = buff_sync_npc;
	pBSN->dwID = m_dwID;
	
	BM.GetSyncBuffID( m_Index, BP, nCount );
	pBSN->wCount = nCount;

	for( int nLoopCount = 0; nLoopCount < nCount; nLoopCount++ )
	{
		pBSN->Buff[nLoopCount].dwBuffTempID = BP[nLoopCount].ulBuffTempID;
		pBSN->Buff[nLoopCount].dwBuffTID	= BP[nLoopCount].ulBuffID;
	}
	
	int nSize = sizeof(_Buff_Sync_Npc) + ( ( nCount - 1 ) * sizeof(_Buff_Sync_Npc::_Pair) );
	pBSN->wProtocolSize = nSize - 1;
	
	if( nCount > 0 && g_pServer != NULL)
		g_pServer->PackDataToClient( nClient, pBSN, nSize );

	return bRet;
}
*/

BOOL KNpc::SendSyncData(int nClient, int nNpcIndex )
{
	//如果是对玩家隐形，则不发送数据
	if (!IsVisibleToPlayer())
	{
		//即使隐形，也要给自己（是个玩家）或者自己的主人（是个召唤兽）发送数据
		if (!IsValidPlayer(m_nPlayerIdx) || Player[m_nPlayerIdx].GetNetConnectIdx() != nClient)
		{
			return FALSE;
		}
	}

#define MAX_COPY_NAME_LENGTH 32

	BOOL	bRet = FALSE;
	char sendBuff[sizeof(NPC_SYNC) + (MAX_COPY_NAME_LENGTH + 1) * 4];
	memset(sendBuff, 0, sizeof(sendBuff));
	NPC_SYNC& NpcSync = *((NPC_SYNC*)sendBuff);

	NpcSync.ProtocolType		= (BYTE)s2c_syncnpc;
	NpcSync.m_btKind			= (BYTE)m_Kind;
	NpcSync.m_bySeries			= (BYTE)m_Series;
	NpcSync.m_Doing				= (BYTE)m_Doing;
	NpcSync.LifePercentage		= (BYTE)GetCurrentLifePercentage();
	NpcSync.ManaPercentage		= (BYTE)GetCurrentManaPercentage();
	NpcSync.dwDoingParamX		= m_Command.Param_X;
	NpcSync.dwDoingParamY		= m_Command.Param_Y;
	NpcSync.dwDoingParamZ		= m_Command.Param_Z;
	NpcSync.dwNameColor			= (DWORD)m_UnaryAttrMgr[nuai_titlecolor];
	NpcSync.bFightState			= (TRUE == m_UnaryAttrMgr[nuai_fightstate]);
	NpcSync.byNpcDir			= m_UnaryAttrMgr[nuai_dir];
	NpcSync.TeamId				= m_UnaryAttrMgr[nuai_team_id];
	NpcSync.nLeagueFlag         = 0;

	NpcSync.RunSpeed[0]		= m_CompAttrMgr[ncai_runspeed][idx_base_value];
	NpcSync.RunSpeed[1]		= m_CompAttrMgr[ncai_runspeed][idx_append_value];
	NpcSync.RunSpeed[2]		= m_CompAttrMgr[ncai_runspeed][idx_append_percent];

	//nNpcIndex 玩家
	NpcSync.nCondition		= NpcSet.GetSUCond( nNpcIndex, m_Index );

	// 告诉客户端这是谁的宝宝
	if (m_Kind == kind_creature)
	{
		const int nSummonerIdx = GetSummonerIdx();
		if (nSummonerIdx > 0  && nSummonerIdx < MAX_NPC )
		{
			NpcSync.nSummonID = Npc[nSummonerIdx].m_dwID;
		}
	}
	else if (m_Kind == kind_employee)
	{
		const int nEmployerIdx = GetEmployerIdx();
		if (nEmployerIdx > 0  && nEmployerIdx < MAX_NPC )
		{
			NpcSync.nSummonID = Npc[nEmployerIdx].m_dwID;
		}
	}

	GetMpsPos((int *)&NpcSync.MapX, (int *)&NpcSync.MapY);
	NpcSync.ID					= m_dwID;
	NpcSync.NpcSettingIdx		= MAKELONG(m_Level, m_NpcSettingIdx);
	NpcSync.TalismanNpcId = m_EquipTalismanNpcId;
	NpcSync.CityId    = INVALID_WORLD_ID;	
	NpcSync.HeadImage  = m_nHeadImage;
	
	NpcSync.nCombatOrg = m_WorldCombatOrg;

	if (m_Kind == kind_creature)
	{
		int nSummorIdx = GetSummonerIdx();
		if (IsValidNpc(nSummorIdx))
		{
			NpcSync.nCombatOrg = Npc[nSummorIdx].m_WorldCombatOrg;
		}//endif
	}
	else if (m_Kind == kind_employee)
	{
		int nEmployer = GetEmployerIdx();
		if (IsValidNpc(nEmployer))
		{
			NpcSync.nCombatOrg = Npc[nEmployer].m_WorldCombatOrg;
		}//endif
	}//end for else
	
	BYTE isKing = 0;
	BYTE isGensMgr = 0;
	char* pShizuName = NULL;
	char* pZhuhouName = NULL;

	BYTE  bInLeague = 0;
	BYTE  bInvader  = INVADER_STATE_NULL;

	if (IsEmployee())
	{
		NpcSync.nSkillType = m_SkillType;
	}

	if (m_Kind == kind_player)
	{
		int playerIndex = GetPlayerIdx();
		if (IsValidPlayer(playerIndex))
		{
			NpcSync.nSkillType = (char)Player[playerIndex].GetSkillSeries();

			SocialUnit *pUnit = GetLeafUnit(playerIndex, enSUTplId_Tong);
			if (pUnit != NULL)
			{
				SocialUnit *pShizu = GetUpNUnit(pUnit, enSULayer_Gens);
				if (pShizu != NULL)
				{
					const char* shizuName = GetUnitName(pShizu->GetUnitAttr());
					
					if (shizuName != NULL)
					{
						pShizuName = (char*)shizuName;
					}

					if (pShizu->IsOwner(GetPlayerName(playerIndex)))
						isGensMgr = 1;

				}

				SocialUnit *pZhuhou = GetUpNUnit(pUnit, enSULayer_Tong);
				if (pZhuhou != NULL)
				{
					const char* zhuhouName = GetUnitName(pZhuhou->GetUnitAttr());
					if (zhuhouName != NULL)
					{
						pZhuhouName = (char*)zhuhouName;
					}
					
					if (pZhuhou->IsOwner(GetPlayerName(playerIndex)))
						isKing = 1;
				}//endif

				SocialUnit * pLeague = GetUpNUnit(pUnit , enSULayer_League);
				if ( pLeague )
				{
					NpcSync.CityId = GetCityMapId( pLeague->GetUnitAttr() );
					bInLeague      = 1;
					
					//Toon war sync only 
					int          nTargetPlayerIndex = Npc[nNpcIndex].GetPlayerIdx();
					if (IsValidPlayer(nTargetPlayerIndex))
					{
						SocialUnit * pTargetPlayerUnit = GetLeafUnit(nTargetPlayerIndex,enSUTplId_Tong);
						SocialUnit * pTargetLeagueUnit = GetUpNUnit(pTargetPlayerUnit,enSULayer_League);

						if (pTargetLeagueUnit)
						{
							int nSubWorldIndex = GetSubWorldIndex();
							if ( nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD )
							{
								int nSubworldTemplateId = SubWorld[nSubWorldIndex].GetWorldTemplateId();
								
								//Notice the League name and the Invadertongname is only available when war is decleared on this map and the player is on the either side of the war
								KWarInfoManager                &warMgr     = GetGlobalWarInfoManager();
								KWarInfoManager::SelfIterator   recordIter ;
								FSWarInfo                     * pWarInfo   = warMgr.NextRecord(recordIter);
								
								while ( pWarInfo )
								{
									if ( pWarInfo ->mapID == nSubworldTemplateId )
									{
										if (pLeague->GetUnitGuid() == pWarInfo->invaderGUID )
										{
											if (pTargetLeagueUnit == pLeague)
												bInvader = INVADER_STATE_FRIEND;
											else if (pWarInfo->defenderGUID == pTargetLeagueUnit->GetUnitGuid())
												bInvader = INVADER_STATE_ENEMY;

											break;
										}//endif
										
										if (pLeague->GetUnitGuid() == pWarInfo->defenderGUID )
										{
											if (pTargetLeagueUnit == pLeague)
												bInvader = INVADER_STATE_FRIEND;
											else if (pWarInfo->invaderGUID == pTargetLeagueUnit->GetUnitGuid())
												bInvader = INVADER_STATE_ENEMY;

											break;
										}//endif
										
										break; //Only one war in da map
									}//endif
									
									pWarInfo = warMgr.NextRecord(recordIter);
								}//end for war
								
							}//endif

						}//endif
						
					}//endif
					
				}//endif


			}
		}		

		NpcSync.nLeagueFlag = (bInLeague << 4) + (bInvader) ;

	}
	else
	{
		if (m_Lord.data[0]!=0)  //Valid GUID
		{
            SocialUnit * pUnit = ServerSocialUnitMgr::Singleton().GetUnit(m_Lord);
			if (pUnit)
			{
                SocialUnitAttr & attr  = pUnit->GetUnitAttr();
				const char *  szUnName = GetUnitName(attr);
				if (szUnName)
				{
                    switch(pUnit->GetLayer())
					{
					case enSULayer_Tong:
						{
							pZhuhouName = (char*)szUnName;
						}
						break;

					case enSULayer_Gens:
						{
							pShizuName = (char*)szUnName;
						}
						break;
					}//end for switch

				}//endif

			}//endif

		}//endif

	}//end else

	//下面把四个字符串拼到一起，用“0”隔开
	char* pNameBuff = NpcSync.NameBuff;
	int nameLength = 0;

	//角色名
	int  nCamouFlage   = 0;
	bool bHideRealName = false;

	if  (m_Kind == kind_player)
	{
		nCamouFlage    =  m_UnaryAttrMgr[nuai_camou_flage];

		if ( IsValidPlayer(m_nPlayerIdx) && Player[m_nPlayerIdx].GetNetConnectIdx() != nClient)
			bHideRealName =  true;

	}//endif

	if  (m_Kind == kind_creature)
	{
		int nSummorIdx =  GetSummonerIdx();

		if (IsValidNpc(nSummorIdx))
		{
			nCamouFlage = Npc[nSummorIdx].m_UnaryAttrMgr[nuai_camou_flage];
			bHideRealName = true;
		}//endif
		
	}//endif

	NpcSync.bCamouflage= nCamouFlage;

	if(bHideRealName && nCamouFlage > 0)
	{
		const char * szPrivateStateName = ConfigManager::Singleton().GetPlayerPrivateStateName( nCamouFlage );
		_ASSERT(szPrivateStateName);
		nameLength = strlen(strncpy(pNameBuff,szPrivateStateName,MAX_COPY_NAME_LENGTH));	
	}//endif
	else
	{
		nameLength = strlen(strncpy(pNameBuff, Name, MAX_COPY_NAME_LENGTH));
	}//end else
	
	pNameBuff += nameLength + 1;

	//氏族名
	if (pShizuName)
	{
		nameLength = strlen(strncpy(pNameBuff, pShizuName, MAX_COPY_NAME_LENGTH));
		pNameBuff += nameLength + 1;
	}
	else
	{
		pNameBuff++;
	}

	//诸侯名
	if (pZhuhouName)
	{
		nameLength = strlen(strncpy(pNameBuff, pZhuhouName, MAX_COPY_NAME_LENGTH));
		pNameBuff += nameLength + 1;
	}
	else
	{
		pNameBuff++;
	}


	NpcSync.IsKingOrGens = (isKing << 4) | isGensMgr;
	NpcSync.m_wLength = pNameBuff - sendBuff - 1;

	if (g_pServer != NULL && SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&NpcSync, NpcSync.m_wLength + 1)))
	{
		bRet = TRUE;
	}
	else
	{
		return FALSE;
	}

	if (IsPlayer() || IsEmployee())
	{
		PLAYER_SYNC	PlayerSync;

		PlayerSync.ProtocolType		= (BYTE)s2c_syncplayer;
		PlayerSync.ID				= m_dwID;
		PlayerSync.AttackSpeed		= (BYTE)m_CompAttrMgr[ncai_attackspeed][idx_base_value];
		PlayerSync.CastSpeed		= (BYTE)m_CompAttrMgr[ncai_castspeed][idx_base_value];
		
/*
带宽优化前
		PlayerSync.ArmorType		= (BYTE)(m_nMorphPart[itempart_armor]<0)?m_ArmorType:m_nMorphPart[itempart_armor];
		PlayerSync.HelmType			= (BYTE)(m_nMorphPart[itempart_helm]<0)?m_HelmType:m_nMorphPart[itempart_helm];
		PlayerSync.HorseType		= (BYTE)(m_nMorphPart[itempart_horse]<0)?m_HorseType:m_nMorphPart[itempart_horse];
		PlayerSync.WeaponType		= (BYTE)(m_nMorphPart[itempart_weapon]<0)?m_WeaponType:m_nMorphPart[itempart_weapon];
		PlayerSync.ShoulderType		= (BYTE)(m_nMorphPart[itempart_shoulder]<0)?m_ShoulderType:m_nMorphPart[itempart_shoulder];
		PlayerSync.CuffType			= (BYTE)(m_nMorphPart[itempart_cuff]<0)?m_CuffType:m_nMorphPart[itempart_cuff];
		PlayerSync.BootType			= (BYTE)(m_nMorphPart[itempart_boots]<0)?m_BootType:m_nMorphPart[itempart_boots];
*/

		BYTE weaponType = (BYTE)(m_nMorphPart[itempart_weapon]<0)?m_WeaponType:m_nMorphPart[itempart_weapon];
		BYTE isRideHorse = (BYTE)m_bRideHorse;
		BYTE helmType = (BYTE)(m_nMorphPart[itempart_helm]<0)?m_HelmType:m_nMorphPart[itempart_helm];
		BYTE armorType = (BYTE)(m_nMorphPart[itempart_armor]<0)?m_ArmorType:m_nMorphPart[itempart_armor];
		BYTE horseType = (BYTE)(m_nMorphPart[itempart_horse]<0)?m_HorseType:m_nMorphPart[itempart_horse];
		BYTE shoulderType = (BYTE)(m_nMorphPart[itempart_shoulder]<0)?m_ShoulderType:m_nMorphPart[itempart_shoulder];
		BYTE bootType = (BYTE)(m_nMorphPart[itempart_boots]<0)?m_BootType:m_nMorphPart[itempart_boots];
		BYTE cuffType = (BYTE)(m_nMorphPart[itempart_cuff]<0)?m_CuffType:m_nMorphPart[itempart_cuff];

		//为了节约带宽，2个Byte合并为1个Byte
// 		PlayerSync.WeaponRideType = weaponType << 1) | isRideHorse;
// 		PlayerSync.HelmArmorType = (helmType << 4) | armorType;
// 		PlayerSync.HorseShoulderType = (horseType << 4) | shoulderType;
// 		PlayerSync.BootCuffType = (bootType << 4) | cuffType;

		PlayerSync.WeaponType	= CombinTP2WORD( weaponType,		m_WeaponPal		);
		PlayerSync.HelmType		= CombinTP( helmType,		m_HelmPal		);
		PlayerSync.ArmorType	= CombinTP( armorType,		m_ArmorType		);
		PlayerSync.HorseType	= CombinTP( horseType,		m_HorsePal		);
		PlayerSync.ShoulderType = CombinTP( shoulderType,	m_ShoulderPal	);
		PlayerSync.BootType		= CombinTP( bootType,		m_BootPal		);
		PlayerSync.CuffType		= CombinTP( cuffType,		m_CuffPal		);
		PlayerSync.m_btSomeFlag = 0;
		PlayerSync.m_btSomeFlag	|= ( m_bRideHorse != FALSE ? 0x01 : 0x00 );

		//选中的称号
		if (IsPlayer())
		{
			Player[m_nPlayerIdx].GetTitleManager().GetSelectedTitle(PlayerSync.TitleIndex, PlayerSync.TitleLevel);
		}
		else
		{
			PlayerSync.TitleIndex = 0;
			PlayerSync.TitleLevel = 0;
		}

		if (g_pServer != NULL && SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&PlayerSync, sizeof(PlayerSync))))
		{
			bRet = TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	//buff sync
	#define MAX_SYNC_NPC_BUFF 50

	char szSendBuff[BUFF_TEMP_SIZE];
	_BuffPair	BP[MAX_SYNC_NPC_BUFF];
	BuffMgr   & BM = BuffMgr::Singleton( );
	int nCount = MAX_SYNC_NPC_BUFF;
	
	_Buff_Sync_Npc*	pBSN = (_Buff_Sync_Npc*)szSendBuff;
	pBSN->Protocol = s2c_buff_family;
	pBSN->ProtocolExtend = buff_sync_npc;
	pBSN->dwID = m_dwID;
	
	BM.GetSyncBuffID( m_Index, BP, nCount );
	pBSN->wCount = nCount;

	for( int nLoopCount = 0; nLoopCount < nCount; nLoopCount++ )
	{
		pBSN->Buff[nLoopCount].dwBuffTempID = BP[nLoopCount].ulBuffTempID;
		pBSN->Buff[nLoopCount].dwBuffTID	= BP[nLoopCount].ulBuffID;
	}
	
	int nSize = sizeof(_Buff_Sync_Npc) + ( ( nCount - 1 ) * sizeof(_Buff_Sync_Npc::_Pair) );
	pBSN->wProtocolSize = nSize - 1;
	
	if( nCount > 0 && g_pServer != NULL)
		g_pServer->PackDataToClient( nClient, pBSN, nSize );

	return bRet;
}

// 平时数据的同步
void KNpc::NormalSync()
{
	if (!m_Index || m_RegionIndex < 0)
		return;

	//非玩家死亡后不同步
	if (!IsPlayer() && m_Doing == do_death)
		return;

	int	nMpsX, nMpsY;
	GetMpsPos(&nMpsX, &nMpsY);

	NPC_NORMAL_SYNC NpcSync;
	NpcSync.ProtocolType = (BYTE)s2c_syncnpcmin;
	NpcSync.ID = m_dwID;
	NpcSync.LifePercentage = (BYTE)GetCurrentLifePercentage();
	NpcSync.ManaPercentage = (BYTE)GetCurrentManaPercentage();
	NpcSync.Doing = (BYTE)m_Doing;
	NpcSync.MapX = nMpsX;
	NpcSync.MapY = nMpsY;
	NpcSync.byNeedUpdate = m_UnaryAttrMgr[nuai_needupdate];

/*
带宽优化前
	NpcSync.State = 0;
	NpcSync.Camp = (BYTE)m_CurrentCamp & 0x0F;
	NpcSync.ManaPercentage = (BYTE)GetCurrentManaPercentage();
*/

	m_UnaryAttrMgr.Set( nuai_needupdate, 0 );
	
	if (IsVisibleToPlayer())
	{
		int nMaxBroadcastCount = MAX_BROADCAST_COUNT_OPTIMIZED;
		BROADCAST_REGION(&NpcSync, sizeof(NPC_NORMAL_SYNC), nMaxBroadcastCount);
	}
	else
	{
		//如果是对玩家隐形，则不发送数据
		//即使隐形，也要给自己（是个玩家）或者自己的主人（是个召唤兽）发送数据
		if (IsValidPlayer(m_nPlayerIdx) && g_pServer != NULL)
		{
			g_pServer->PackDataToClient(Player[m_nPlayerIdx].GetNetConnectIdx(), (BYTE*)&NpcSync, sizeof(NpcSync));
		}
	}

	if (IsPlayer() || IsEmployee())
	{
		BYTE weaponType = (BYTE)(m_nMorphPart[itempart_weapon]<0)?m_WeaponType:m_nMorphPart[itempart_weapon];
		BYTE isRideHorse = (BYTE)m_bRideHorse;
		BYTE helmType = (BYTE)(m_nMorphPart[itempart_helm]<0)?m_HelmType:m_nMorphPart[itempart_helm];
		BYTE armorType = (BYTE)(m_nMorphPart[itempart_armor]<0)?m_ArmorType:m_nMorphPart[itempart_armor];
		BYTE horseType = (BYTE)(m_nMorphPart[itempart_horse]<0)?m_HorseType:m_nMorphPart[itempart_horse];
		BYTE shoulderType = (BYTE)(m_nMorphPart[itempart_shoulder]<0)?m_ShoulderType:m_nMorphPart[itempart_shoulder];
		BYTE bootType = (BYTE)(m_nMorphPart[itempart_boots]<0)?m_BootType:m_nMorphPart[itempart_boots];
		BYTE cuffType = (BYTE)(m_nMorphPart[itempart_cuff]<0)?m_CuffType:m_nMorphPart[itempart_cuff];

		PLAYER_NORMAL_SYNC PlayerSync;

		PlayerSync.ProtocolType	= (BYTE)s2c_syncplayermin;
		PlayerSync.ID			= m_dwID;
		PlayerSync.AttackSpeed	= (BYTE)m_CompAttrMgr[ncai_attackspeed];
		PlayerSync.CastSpeed	= (BYTE)m_CompAttrMgr[ncai_castspeed];
		
		//为了节约带宽，2个Byte合并为1个Byte
// 		PlayerSync.WeaponRideType = (weaponType << 1) | isRideHorse;
// 		PlayerSync.HelmArmorType = (helmType << 4) | armorType;
// 		PlayerSync.HorseShoulderType = (horseType << 4) | shoulderType;
// 		PlayerSync.BootCuffType = (bootType << 4) | cuffType;		
		
		PlayerSync.WeaponType	= CombinTP2WORD( weaponType,		m_WeaponPal		);
		PlayerSync.HelmType		= CombinTP( helmType,		m_HelmPal		);
		PlayerSync.ArmorType	= CombinTP( armorType,		m_ArmorType		);
		PlayerSync.HorseType	= CombinTP( horseType,		m_HorsePal		);
		PlayerSync.ShoulderType = CombinTP( shoulderType,	m_ShoulderPal	);
		PlayerSync.BootType		= CombinTP( bootType,		m_BootPal		);
		PlayerSync.CuffType		= CombinTP( cuffType,		m_CuffPal		);
		PlayerSync.m_btSomeFlag = 0;
		PlayerSync.m_btSomeFlag	|= ( m_bRideHorse != FALSE ? 0x01 : 0x00 );
/*
带宽优化前
		PlayerSync.ArmorType		= (BYTE)(m_nMorphPart[itempart_armor]<0)?m_ArmorType:m_nMorphPart[itempart_armor];
		PlayerSync.HelmType			= (BYTE)(m_nMorphPart[itempart_helm]<0)?m_HelmType:m_nMorphPart[itempart_helm];
		PlayerSync.HorseType		= (BYTE)(m_nMorphPart[itempart_horse]<0)?m_HorseType:m_nMorphPart[itempart_horse];
		PlayerSync.WeaponType		= (BYTE)(m_nMorphPart[itempart_weapon]<0)?m_WeaponType:m_nMorphPart[itempart_weapon];
		PlayerSync.ShoulderType		= (BYTE)(m_nMorphPart[itempart_shoulder]<0)?m_ShoulderType:m_nMorphPart[itempart_shoulder];
		PlayerSync.CuffType			= (BYTE)(m_nMorphPart[itempart_cuff]<0)?m_CuffType:m_nMorphPart[itempart_cuff];
		PlayerSync.BootType			= (BYTE)(m_nMorphPart[itempart_boots]<0)?m_BootType:m_nMorphPart[itempart_boots];
		PlayerSync.MorphNpc			= m_nMorphType;
		PlayerSync.bRideHorse		= m_bRideHorse;
		PlayerSync.bySetEfficacy = m_nSetEfficacyType;
*/

		if (IsVisibleToPlayer())
		{
			int nMaxCount = MAX_BROADCAST_COUNT_OPTIMIZED;
			BROADCAST_REGION(&PlayerSync, sizeof(PLAYER_NORMAL_SYNC), nMaxCount);
		}
		else
		{
			if (g_pServer != NULL && IsPlayer())
				g_pServer->PackDataToClient(Player[m_nPlayerIdx].GetNetConnectIdx(), (BYTE*)&PlayerSync, sizeof(PlayerSync));
		}

		if (IsPlayer())
		{
			NPC_PLAYER_TYPE_NORMAL_SYNC	sSync;
			sSync.ProtocolType = s2c_syncnpcminplayer;
			sSync.m_dwNpcID = m_dwID;
			sSync.m_dwMapX = nMpsX;
			sSync.m_dwMapY = nMpsY;
			sSync.m_wOffX = GetOffX();
			sSync.m_wOffY = GetOffY();

/*
带宽优化前
			sSync.m_btCamp = (BYTE)m_CurrentCamp;
			sSync.m_byDoing = this->m_Doing;
*/
			
			if (g_pServer != NULL)
				g_pServer->PackDataToClient(Player[m_nPlayerIdx].GetNetConnectIdx(), (BYTE*)&sSync, sizeof(sSync));
		}
	}

}

void KNpc::BroadCastRevive(int nType)
{
	if (!IsPlayer())
		return;

	if (m_RegionIndex < 0)
		return;

	NPC_REVIVE_SYNC	NpcReviveSync;
	NpcReviveSync.ProtocolType = s2c_playerrevive;
	NpcReviveSync.ID = m_dwID;
	NpcReviveSync.Type = (BYTE)nType;

	int nMaxCount = MAX_BROADCAST_COUNT_MIN;
	BROADCAST_REGION((BYTE*)&NpcReviveSync, sizeof(NPC_REVIVE_SYNC), nMaxCount);
}
#endif

#ifndef _SERVER
#include "scene/KScenePlaceC.h"

void KNpc::UpdataPaintInfo( int offsetHeight ,char* showName)
{
	//	if (m_Kind == kind_player || m_Kind == kind_creature )
	{
		m_RoleInfoRes[role_info_zhuhou] = m_ZhuhouName;
		m_RoleInfoRes[role_info_shizu] = m_ShizuName;
		m_RoleInfoRes[role_info_title] = NULL;
		m_RoleInfoRes[role_info_name] = showName;
		
		RoleHeadInfo roleHeadInfo;
		ZeroMemory( &roleHeadInfo, sizeof(RoleHeadInfo) );
		ConfigManager& cm = ConfigManager::Singleton();
		const char* pTbuff = cm.GetConfigurableDisplayStyle( style_role_head_info_begin );
		if ( pTbuff )
		{
			strcpy( m_RoleHeadInfo, pTbuff );	

			if (m_UnaryAttrMgr[nuai_camou_flage]) //蒙面
			{
				char szInfo[512] = "";

				const char* pStyle = cm.GetConfigurableDisplayStyle( style_role_head_hover_info, role_info_name );
				if (pStyle)
				{
					sprintf( szInfo, pStyle,"", "255,255,255", cm.GetPlayerPrivateStateName( m_UnaryAttrMgr[nuai_camou_flage]),"");
					strcat( m_RoleHeadInfo, szInfo );
				}//endif
				
			}//endif
			else
			{
				char szColour[COMMON_CLIENT_MSG_LEN_16];
				if ( m_Kind == kind_dialoger && g_pNpcTemplate[m_NpcSettingIdx][m_Level] && g_pNpcTemplate[m_NpcSettingIdx][m_Level]->m_szNpcNameColor[0])
				{
					strcpy( szColour, g_pNpcTemplate[m_NpcSettingIdx][m_Level]->m_szNpcNameColor );
				}
				else
					ColorToString( m_UnaryAttrMgr[nuai_titlecolor], szColour );
				
				if (m_Kind == kind_player && m_ZhuhouName[0]!=0 )  //Contry player specially
				{
					int iClientIdx = GetClientPlayer().GetNpcIndex();
					
					if (strlen(Npc[iClientIdx].GetZhuhouName())!=0)  //Contry player too
					{
						//Is the Same ZhoHou
						if (
							( m_UnaryAttrMgr[nuai_titlecolor] == 0xffffffff || m_UnaryAttrMgr[nuai_titlecolor] == cm.GetConfigurableColor(color_pk_encourage)) 
							&& strcmp(Npc[iClientIdx].GetZhuhouName(),m_ZhuhouName) == 0) //诸侯同色
						{
							const char * szTongSameColor =cm.GetConfigurableDisplayStyle( style_special_color );
							
							if (szTongSameColor && szTongSameColor[0] != 0)
								strcpy(szColour,szTongSameColor);
						}//endif
					
					}//endif
				
				}//endif
				
				if (m_Kind == kind_player )
				{
					int iClientIdx = GetClientPlayer().GetNpcIndex();

					if (  iClientIdx != m_Index )
					{
						if ( GetInvaderTongFlag()  == INVADER_STATE_FRIEND)
						{
							sprintf (szColour,"0,0,255,255");
						}//endif
						else
						{
							if( GetInvaderTongFlag()  == INVADER_STATE_ENEMY)
								sprintf(szColour,"255,128,64,255");
						}//end else

					}//endif
					
				}//endif
				
				if ( IsInWorldCombatInstance())
				{
					const char* pTbuff = cm.GetConfigurableDisplayStyle( style_world_combat_color , m_WorldCombatOrg);
					if (pTbuff && pTbuff[0])
					{
						strcpy(szColour,pTbuff);
					}//endif
					
				}//endif
				
				std::map<std::string,HEAD_INFO>::iterator it = m_HeadInfoPlus.begin();
				
				while ( it != m_HeadInfoPlus.end() )
				{
					if (it->second.m_Prioryty == HIP_Special)
						strcat( m_RoleHeadInfo, it->second.m_InfoString.c_str() );
					
					++it;
				}//end for while
				
				it = m_HeadInfoPlus.begin();
				
				while ( it != m_HeadInfoPlus.end() )
				{
					if (it->second.m_Prioryty == HIP_Normal)
						strcat( m_RoleHeadInfo, it->second.m_InfoString.c_str() );
					
					++it;
				}//end for while
				
				for ( int nIdx = 0; nIdx < role_info_count; ++nIdx )
				{
					const char* pTbuff = cm.GetConfigurableDisplayStyle( style_role_head_hover_info, nIdx );
					if ( nIdx == 0 && m_CityName[0] )
					{
						if ( m_CityId )
						{
							char szInfo[COMMON_CLIENT_MSG_LEN_256];
							char szCityId[COMMON_CLIENT_MSG_LEN_16];
							itoa( m_CityId, szCityId, 10 );
							sprintf( szInfo, pTbuff, "ty_chengshi_tu", szCityId );
							strcat( m_RoleHeadInfo, szInfo );
						}
					}
					else if (nIdx == role_info_name && m_Kind == kind_player)
					{
						char independentTitle[COMMON_CLIENT_MSG_LEN_256] = { 0 };
						char inlineTitle[COMMON_CLIENT_MSG_LEN_256] = { 0 };
						if (m_TitleIndex > 0)
						{
							TitleInfo* pTitleInfo = TitleManager::GetTitleInfo(m_TitleIndex);
							if (pTitleInfo)
							{
								if (title_type_upgradable == pTitleInfo->Type)
								{
									if (m_TitleLevel >= 0 && m_TitleLevel < MAX_UPGRADABLE_TITLE_LEVEL_COUNT)
									{
										if (pTitleInfo->UpgradInfo[m_TitleLevel].ShowName[0] != 0)
										{
											snprintf(independentTitle, sizeof(independentTitle), pTitleInfo->UpgradInfo[m_TitleLevel].ShowName, sizeof(independentTitle));
										}
										else
										{
											snprintf(inlineTitle, sizeof(inlineTitle), "%s ", pTitleInfo->UpgradInfo[m_TitleLevel].Name);
										}
									}
								}
								else
								{
									if (pTitleInfo->ShowName[0] != 0)
									{
										snprintf(independentTitle, sizeof(independentTitle), pTitleInfo->ShowName, sizeof(independentTitle));
									}
									else
									{
										snprintf(inlineTitle, sizeof(inlineTitle), "%s ", pTitleInfo->Name);
									}
								}
							}
						}

						if ( pTbuff && m_RoleInfoRes[nIdx] && strlen(m_RoleInfoRes[nIdx]))
						{
							char szInfo[COMMON_CLIENT_MSG_LEN_256];
							sprintf( szInfo, pTbuff, independentTitle, szColour, inlineTitle, m_RoleInfoRes[nIdx]);
							strcat( m_RoleHeadInfo, szInfo );
						}
					}
					else if (nIdx == role_info_name)
					{
						if ( pTbuff && m_RoleInfoRes[nIdx] && strlen(m_RoleInfoRes[nIdx]))
						{
							char szInfo[COMMON_CLIENT_MSG_LEN_256];
							sprintf( szInfo, pTbuff, "", szColour, m_RoleInfoRes[nIdx], "");
							strcat( m_RoleHeadInfo, szInfo );
						}
					}
					else
					{
						if ( pTbuff && m_RoleInfoRes[nIdx] && strlen(m_RoleInfoRes[nIdx]))
						{
							char szInfo[COMMON_CLIENT_MSG_LEN_256];
							sprintf( szInfo, pTbuff, szColour, m_RoleInfoRes[nIdx]);
							strcat( m_RoleHeadInfo, szInfo );
						}
					}
				}
				
				it = m_HeadInfoPlus.begin();
				
				while ( it != m_HeadInfoPlus.end() )
				{
					if (it->second.m_Prioryty == HIP_Important)
						strcat( m_RoleHeadInfo, it->second.m_InfoString.c_str() );
					
					++it;
				}//end for while
				
			}//end else

		}//endif
		
		pTbuff = cm.GetConfigurableDisplayStyle( style_role_head_info_end );
		if ( pTbuff )
		{
			strcat( m_RoleHeadInfo, pTbuff );
		}
		
		int nMpsX = 0,nMpsY = 0;
		GetMpsPos(&nMpsX, &nMpsY);
		m_RoleHeadInfo[COMMON_CLIENT_MSG_LEN_1024-1] = 0;
		roleHeadInfo.dwID = m_dwID;
		roleHeadInfo.szInfo = m_RoleHeadInfo;
		roleHeadInfo.nX			= nMpsX;
		roleHeadInfo.nY			= nMpsY;
		g_pRepresent->CoordinateTransform( roleHeadInfo.nX, roleHeadInfo.nY, offsetHeight );
		CoreDataChanged( GDCNI_ROLEHEADINFO_UPDATA, (unsigned int)&roleHeadInfo, NULL );
	}
}

void KNpc::HideInfo()
{
	RoleHeadInfo roleHeadInfo;
	ZeroMemory( &roleHeadInfo, sizeof(RoleHeadInfo) );
	char npcShowName[33] = {0};
	char* show = 0;
	if(m_Kind == kind_creature)
	{
		strcpy(npcShowName,Name);
		strcat(npcShowName,PLAYER_PET_NAME_EXTRA);
		show = npcShowName;

	}
	else
	{
//		strcpy(npcShowName,Name);
		show = Name;
	}
//	roleHeadInfo.szName = show;
	roleHeadInfo.dwID = m_dwID;
	CoreDataChanged(GDCNI_ROLEHEADINFO_HIDE,(unsigned int)&roleHeadInfo, NULL );
}

void KNpc::DelInfo()
{
	RoleHeadInfo roleHeadInfo;
	ZeroMemory( &roleHeadInfo, sizeof(RoleHeadInfo) );
	char npcShowName[33] = {0};
	char* show = 0;
	if(m_Kind == kind_creature)
	{
		strcpy(npcShowName,Name);
		strcat(npcShowName,PLAYER_PET_NAME_EXTRA);
		show = npcShowName;
		
	}
	else
	{
		show = Name;
	}
	roleHeadInfo.dwID = m_dwID;
	CoreDataChanged( GDCNI_ROLEHEADINFO_DEL,(unsigned int)&roleHeadInfo, NULL );
	SetHeadInfoChanged(true);
}

int KNpc::PaintInfo(int nHeightOffset, bool bSelect)
{
	if (m_RegionIndex == -1)
	{
		HideInfo() ;
		return 0 ;
	}//endif

	if ( m_Kind == kind_normal && ( m_Doing == do_revive || m_Doing == do_death || GetCurrentLifePercentage() == 0) )
	{
		return 0;
	}

	/*if ( m_NpcSettingIdx  < 0 )
	{
		return 0;
	}//*/
	int nMpsX, nMpsY;
	GetMpsPos(&nMpsX, &nMpsY);

	char npcShowName[256] = {0};
	char* show = 0;
	if(m_Kind == kind_creature)
	{
		strcpy(npcShowName,Name);
		strcat(npcShowName,PLAYER_PET_NAME_EXTRA);
		show = npcShowName;

	}
	else if ( IsEmployee() )
	{
		ConfigManager& cm = ConfigManager::Singleton();
		const char* strEmployee = cm.GetConfigurableDisplayStyle( style_npc_name_plus, kind_employee );
		if ( strEmployee && strEmployee[0] != 0 )
		{
			strncpy( npcShowName, strEmployee, 9 );
			npcShowName[8] = 0;
		}
		else
		{
			strncpy( npcShowName, PLAYER_EMPLOYEE_NAME_EXTRA, 9 );
			npcShowName[8] = 0;
		}
		
		strncat( npcShowName, Name, sizeof(Name) );
		show = npcShowName;
	}
	else
	{
		show = Name;
	}

	if (m_Kind == kind_player || m_Kind == kind_employee || m_Kind == kind_building ||  m_Kind == kind_dialoger )
	{
		m_bSelect = bSelect;
		if ( true == m_bHeadInfoChanged )
		{
			if( time(NULL) - m_ulTime > 5 )
			{
				UpdataPaintInfo(nHeightOffset,show);
				m_bHeadInfoChanged = false;
				m_ulTime = time( NULL );		
			}

		}
		RoleHeadInfo roleHeadInfo;
		ZeroMemory( &roleHeadInfo, sizeof(RoleHeadInfo) );
		m_RoleHeadInfo[COMMON_CLIENT_MSG_LEN_1024-1] = 0;
		roleHeadInfo.dwID       = m_dwID;
		roleHeadInfo.nX			= nMpsX;
		roleHeadInfo.nY			= nMpsY ;
		g_pRepresent->CoordinateTransform( roleHeadInfo.nX, roleHeadInfo.nY, nHeightOffset );
		CoreDataChanged( GDCNI_ROLEHEADINFO_UPDATA_POS, (unsigned int)&roleHeadInfo, NULL );
	}
	else if ((m_Kind == kind_dialoger)||(m_Kind == kind_siege_weapon))
	{
		if ( m_NpcSettingIdx  < 0 )
			return 0;

		KUiNewFont newFont;
		strcpy( newFont.szName, "stzhongs-10");
		strcpy( newFont.szContext, Name );
		
		newFont.nX = nMpsX;
		newFont.nY = nMpsY  - nHeightOffset * 2;
		newFont.nWidth	= 100;
		g_pRepresent->CoordinateTransform( newFont.nX, newFont.nY, 0 );
		newFont.nZ = 0;
		if ( 0xffffffff != m_UnaryAttrMgr[nuai_titlecolor] || 
			( g_pNpcTemplate[m_NpcSettingIdx][m_Level] && 
				g_pNpcTemplate[m_NpcSettingIdx][m_Level]->m_szNpcNameColor[0] == 0 ) )
		{
			newFont.uColor = m_UnaryAttrMgr[nuai_titlecolor];
		}
		else
		{
			if ( g_pNpcTemplate[m_NpcSettingIdx][m_Level] )
			{
				DWORD dwColor = StringToColor( g_pNpcTemplate[m_NpcSettingIdx][m_Level]->m_szNpcNameColor );
				newFont.uColor = dwColor;
			}
		}
		
		if ( newFont.nX >= 0 && newFont.nY >= 0 )
		{
			CoreDataChanged( GDCNI_DRAWTEXT, (unsigned int)&newFont, NULL );
		}
		if ( m_bDoubleExp )
		{
/*			KImageParam imgParam;
			KUiImageRef &aImage = g_EmoteImage.GetImage( task_0 );
			
			if ( g_pRepresent->GetImageParam(aImage.szImage, &imgParam, ISI_T_SPR) )
			{								
				aImage.oPosition.nX = nMpsX - nFontSize - imgParam.nWidth;  
				aImage.oPosition.nY = nMpsY + nFontSize + 1 - imgParam.nHeight;			
				aImage.oPosition.nZ = nHeightOffset;
				g_pRepresent->DrawPrimitives(1, &aImage, RU_T_IMAGE, FALSE);
				IR_NextFrame(aImage);
			}//*/
		}
	}
	else
	{
		if ( m_NpcSettingIdx  < 0 )
			return 0;

		KUiNewFont newFont;
		strcpy( newFont.szName, "stzhongs-10");
		strcpy( newFont.szContext, show );
		
		newFont.nX = nMpsX;
		newFont.nY = nMpsY  - nHeightOffset * 2;
		newFont.nWidth	= 100;
		g_pRepresent->CoordinateTransform( newFont.nX, newFont.nY, 0 );
		newFont.nZ = 0;
		if ( 0xffffffff != m_UnaryAttrMgr[nuai_titlecolor] ||
			( g_pNpcTemplate[m_NpcSettingIdx][m_Level] && 
			g_pNpcTemplate[m_NpcSettingIdx][m_Level]->m_szNpcNameColor[0] == 0 ) )
		{
			newFont.uColor = m_UnaryAttrMgr[nuai_titlecolor];
		}
		else
		{
			if ( g_pNpcTemplate[m_NpcSettingIdx][m_Level] )
			{
				DWORD dwColor = StringToColor( g_pNpcTemplate[m_NpcSettingIdx][m_Level]->m_szNpcNameColor );
				newFont.uColor = dwColor;
			}
		}

		if ( IsInWorldCombatInstance())
		{
			ConfigManager &  cm  = ConfigManager::Singleton();
			const char* pTbuff   = cm.GetConfigurableDisplayStyle( style_world_combat_color , m_WorldCombatOrg);
			if (pTbuff && pTbuff[0])
			{
				newFont.uColor = StringToColor(pTbuff);
			}//endif
			
		}//endif
		
		if ( newFont.nX >= 0 && newFont.nY >= 0 )
		{
			CoreDataChanged( GDCNI_DRAWTEXT, (unsigned int)&newFont, NULL );
		}
	}
	
#ifdef SWORDONLINE_SHOW_DBUG_INFO_LUCIFER
	if (Player[CLIENT_PLAYER_INDEX].m_DebugMode)
	{
		char szNameID[50];
		sprintf(szNameID,"[%d] Dir=%d (%d, %d)", m_dwID, m_UnaryAttrMgr[nuai_dir], nMpsX, nMpsY);
		g_pRepresent->OutputText(12, szNameID, KRF_ZERO_END, nMpsX, nMpsY + 20, 0xfff0fff0, 0, m_Height);
	}

	if (Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index && Player[CLIENT_PLAYER_INDEX].m_DebugMode)
	{
		char	szMsg[256];
		int nCount[9];
		for (int i = 0; i < 9; i++)
			nCount[i] = 0;
		if (LEFTUPREGIONIDX >= 0)
			nCount[0] = LEFTUPREGION.m_NpcList.GetNodeCount();
		if (UPREGIONIDX >= 0)
			nCount[1] = UPREGION.m_NpcList.GetNodeCount();
		if (RIGHTUPREGIONIDX >= 0)
			nCount[2] = RIGHTUPREGION.m_NpcList.GetNodeCount();
		if (LEFTREGIONIDX >= 0)
			nCount[3] = LEFTREGION.m_NpcList.GetNodeCount();
		if (m_RegionIndex >= 0)
			nCount[4] = CURREGION.m_NpcList.GetNodeCount();
		if (RIGHTREGIONIDX >= 0)
			nCount[5] = RIGHTREGION.m_NpcList.GetNodeCount();
		if (LEFTDOWNREGIONIDX >= 0)
			nCount[6] = LEFTDOWNREGION.m_NpcList.GetNodeCount();
		if (DOWNREGIONIDX >= 0)
			nCount[7] = DOWNREGION.m_NpcList.GetNodeCount();
		if (RIGHTDOWNREGIONIDX >= 0)
			nCount[8] = RIGHTDOWNREGION.m_NpcList.GetNodeCount();
		
		int nPosX, nPosY;
		GetMpsPos(&nPosX, &nPosY);
		
		sprintf(szMsg,
			"NpcID:%d  Life:%d\nRegionIndex:%d Pos:%d,%d\nPlayerNumber:%d\n"
			"NpcNumber:\n%02d,%02d,%02d\n%02d,%02d,%02d\n%02d,%02d,%02d\n"
			"In Car Region:%02d",
			m_dwID,
			m_UnaryAttrMgr[nuai_curlife],
			m_RegionIndex,
			GetMapX(),
			GetMapY(),
			CURREGION.m_PlayerList.GetNodeCount(),
			nCount[0], nCount[1], nCount[2],
			nCount[3], nCount[4], nCount[5],
			nCount[6], nCount[7], nCount[8],
			0
			);
		
		g_pRepresent->OutputText(12, szMsg, -1, 320, 40, 0xffffffff);

	}
#endif

#ifdef _DEBUG
	if(m_nRealPosX > 0)
	{
		char szPos[64] = {0};
		int nMpsXX, nMpsYY;
		GetMpsPos(&nMpsXX, &nMpsYY);
		sprintf(szPos,"Loc(%d) Dir=%d [%d, %d]", 
			m_dwID, m_UnaryAttrMgr[nuai_dir], nMpsXX, nMpsYY);
		g_pRepresent->OutputText(12, szPos, KRF_ZERO_END, nMpsXX, nMpsYY + 10, 0xfff0fff0, 0, m_Height);

		char szRealPos[64] = {0};
		sprintf(szRealPos,"Svr(%d) Dir=%d [%d, %d]", 
			m_dwID, m_nRealDir, m_nRealPosX, m_nRealPosY);
		g_pRepresent->OutputText(12, szRealPos, KRF_ZERO_END, m_nRealPosX, m_nRealPosY + 40, 0xfff0fff0, 0, m_Height);
	}
	
	if (IsPlayer())
	{
		char szCombatInfo[128] = "";
		
		int nMpsXX, nMpsYY;
		GetMpsPos(&nMpsXX, &nMpsYY);
		sprintf(szCombatInfo,"CombatInfo:Org(%d) Score(%d)", m_WorldCombatOrg , GetClientPlayer().GetCombatInfo().nScore);
		
		g_pRepresent->OutputText(12, szCombatInfo, KRF_ZERO_END, nMpsXX, nMpsYY + 80, 0xfff0fff0, 0, m_Height);
	}//endif

#endif

	return nHeightOffset;
}

int	KNpc::PaintChat(int nHeightOffset)
{
	//--> Rocker 2005/07/14
//	if (m_Kind != kind_player)
//		return nHeightOffset;
	//<-- End
	if (m_nChatContentLen <= 0)
		return nHeightOffset;
	if (m_nChatNumLine <= 0)
		return nHeightOffset;

	int nFontSize = 12;
	int					nWidth, nHeight;
	int					nMpsX, nMpsY;
	
	KOutputTextParam	sParam;
	sParam.BorderColor = 0;
	
	sParam.nNumLine = m_nChatNumLine;
	
	nWidth = m_nChatFontWidth * nFontSize / 2;
	nHeight = sParam.nNumLine * (nFontSize + 1);
	
	nWidth += 6;	//为了好看
	nHeight += 5;	//为了好看
	

	//-------> Ray [Luoliang] 2005-8-1
	int nTailWidth = 0;
	int nTailHeight = 0;
	

	GetMpsPos(&nMpsX, &nMpsY);
	//end--------------------------------------------------------------------------	
	

	//bool bBorder = false;
	if ( kind_pet != m_Kind )
	{
		sParam.nX = nMpsX - nWidth / 2;
		sParam.nY = nMpsY;
		sParam.nZ = nHeightOffset + nHeight + nTailHeight;
		
		sParam.Color = SHOW_CHAT_COLOR;
		sParam.nSkipLine = 0;
		sParam.nVertAlign = 0;
		sParam.bPicPackInSingleLine = true;
	}
	else
	{
// 		g_pRepresent->GetImageParam(g_EmoteImage.m_clsChatImg.imgTail.szImage, &imgParam, g_EmoteImage.m_clsChatImg.imgTail.nType);	
// 		
// 		nTailWidth = imgParam.nWidth;
// 		nTailHeight = imgParam.nHeight + 1;
// 		
// 		nWidth = max(nTailWidth, nWidth);
// 		
// 		sParam.nX = nMpsX - nWidth / 2;
// 		sParam.nY = nMpsY;
// 		sParam.nZ = nHeightOffset + nHeight + nTailHeight;
// 		sParam.Color = g_EmoteImage.clPetChatTextColor;
// 		sParam.BorderColor = g_EmoteImage.clPetChatTextBorderColor;
// 
// 		sParam.nSkipLine = 0;
// 		sParam.nVertAlign = 0;
// 		sParam.bPicPackInSingleLine = true;
// 		
// 		KRUShadow			sShadow;
// 		sShadow.oPosition.nX = sParam.nX;
// 		sShadow.oPosition.nX -= 3;	//为了好看
// 		sShadow.oPosition.nY = sParam.nY;
// 		sShadow.oPosition.nZ = sParam.nZ;
// 		
// 		sShadow.oEndPos.nX = sParam.nX + nWidth;
// 		sShadow.oEndPos.nX += 2;	//为了好看
// 		sShadow.oEndPos.nY = sParam.nY;	
// 		//sShadow.oEndPos.nZ = sParam.nZ;
// 		sShadow.oEndPos.nZ = sParam.nZ - nHeight;
// 		sShadow.Color.Color_dw = g_EmoteImage.clPetChatShadowColor;
// 
// 		int nBkImgWidth, nBkImgHeight;
// 		int nStartX, nStartY, nStartZ;
// 		int nEndX;
// 		g_pRepresent->DrawPrimitives(1, &sShadow, RU_T_SHADOW, FALSE);
// 
// 		KUiImageRef *pImg = &g_EmoteImage.m_clsChatImg.imgLT;
// 		g_pRepresent->GetImageParam(pImg->szImage, &imgParam, pImg->nType);
// 		
// 		nBkImgWidth = imgParam.nWidth;
// 		nBkImgHeight = imgParam.nHeight;
// 		
// 		nStartX = sShadow.oPosition.nX - nBkImgWidth;
// 		nStartY = sShadow.oPosition.nY;
// 		nStartZ = sShadow.oPosition.nZ + nBkImgHeight;
// 		
// 		//左上角
// 		
// 		pImg->oPosition.nX = nStartX;
// 		pImg->oPosition.nY = nStartY;
// 		pImg->oPosition.nZ = nStartZ;
// 		
// 		g_pRepresent->DrawPrimitives(1, pImg, RU_T_IMAGE, FALSE);
// 		
// 		//上面的横边
// 		pImg = &g_EmoteImage.m_clsChatImg.imgTop;
// 		pImg->oPosition.nY = nStartY;
// 		pImg->oPosition.nZ = nStartZ;
// 		int nShadowWidth = sShadow.oEndPos.nX - sShadow.oPosition.nX;
// 		int i = sShadow.oPosition.nX;
// 		nEndX = i + nShadowWidth;
// 		
// 		for ( ; i < nEndX ; ++i )
// 		{
// 			pImg->oPosition.nX = i;
// 			g_pRepresent->DrawPrimitives(1, pImg, RU_T_IMAGE, FALSE);
// 		}
// 		
// 		//右上角
// 		pImg = &g_EmoteImage.m_clsChatImg.imgRT;
// 		
// 		pImg->oPosition.nX = sShadow.oEndPos.nX;
// 		pImg->oPosition.nY = nStartY;
// 		pImg->oPosition.nZ = nStartZ;	
// 		
// 		g_pRepresent->DrawPrimitives(1, pImg, RU_T_IMAGE, FALSE);
// 		
// 		//左右的竖边
// 		nStartX = sShadow.oPosition.nX - nBkImgWidth;
// 		nStartY = sShadow.oPosition.nY;
// 		
// 		nEndX = sShadow.oEndPos.nX;
// 		
// 		
// 		for ( i = 0; i < nHeight; ++i )
// 		{
// 			nStartZ = sShadow.oPosition.nZ - i;
// 			pImg = &g_EmoteImage.m_clsChatImg.imgLeft;
// 			pImg->oPosition.nX = nStartX;
// 			pImg->oPosition.nY = nStartY;
// 			pImg->oPosition.nZ = nStartZ;
// 			g_pRepresent->DrawPrimitives(1, pImg, RU_T_IMAGE, FALSE);
// 			
// 			pImg = &g_EmoteImage.m_clsChatImg.imgRight;
// 			pImg->oPosition.nX = nEndX;
// 			pImg->oPosition.nY = nStartY;
// 			pImg->oPosition.nZ = nStartZ;
// 			g_pRepresent->DrawPrimitives(1, pImg, RU_T_IMAGE, FALSE);
// 		}
// 		
// 		//左下角
// 		pImg = &g_EmoteImage.m_clsChatImg.imgLB;
// 		
// 		pImg->oPosition.nX = sShadow.oPosition.nX - nBkImgWidth;
// 		pImg->oPosition.nY = sShadow.oEndPos.nY;
// 		pImg->oPosition.nZ = sShadow.oEndPos.nZ;
// 		
// 		g_pRepresent->DrawPrimitives(1, pImg, RU_T_IMAGE, FALSE);
// 		
// 		//下面的横边靠左边的部分
// 		pImg = &g_EmoteImage.m_clsChatImg.imgBottom;
// 		
// 		nStartX = sShadow.oPosition.nX;
// 		nEndX = nStartX + (nWidth - nTailWidth) / 2;
// 		
// 		pImg->oPosition.nY = sShadow.oEndPos.nY;
// 		pImg->oPosition.nZ = sShadow.oEndPos.nZ;
// 		
// 		for ( i = nStartX; i < nEndX; ++i )
// 		{
// 			pImg->oPosition.nX = i;
// 			g_pRepresent->DrawPrimitives(1, pImg, RU_T_IMAGE, FALSE);
// 		}
// 		
// 		//尾巴
// 		pImg = &g_EmoteImage.m_clsChatImg.imgTail;
// 		
// 		pImg->oPosition.nX = nEndX;
// 		pImg->oPosition.nY = sShadow.oEndPos.nY;
// 		pImg->oPosition.nZ = sShadow.oEndPos.nZ;
// 		
// 		g_pRepresent->DrawPrimitives(1, pImg, RU_T_IMAGE, FALSE);
// 		
// 		
// 		//下面的横边靠右边的部分
// 		pImg = &g_EmoteImage.m_clsChatImg.imgBottom;
// 		
// 		nStartX = nEndX + nTailWidth;
// 		nEndX = sShadow.oEndPos.nX;
// 		
// 		pImg->oPosition.nY = sShadow.oEndPos.nY;
// 		pImg->oPosition.nZ = sShadow.oEndPos.nZ;
// 		
// 		for ( i = nStartX; i < nEndX; ++i )
// 		{
// 			pImg->oPosition.nX = i;
// 			g_pRepresent->DrawPrimitives(1, pImg, RU_T_IMAGE, FALSE);
// 		}
// 		
// 		//右下角
// 		pImg = &g_EmoteImage.m_clsChatImg.imgRB;
// 		
// 		pImg->oPosition.nX = sShadow.oEndPos.nX;
// 		pImg->oPosition.nY = sShadow.oEndPos.nY;
// 		pImg->oPosition.nZ = sShadow.oEndPos.nZ;
// 		
// 		g_pRepresent->DrawPrimitives(1, pImg, RU_T_IMAGE, FALSE);
// 		//<------- End [Ray]
	}
	
	
	g_pRepresent->OutputRichText(nFontSize, &sParam, m_szChatBuffer, m_nChatContentLen, nWidth);
	
	return sParam.nZ + nTailHeight;
}

#include "Text.h"
int	KNpc::SetChatInfo(const char* Name, const char* pMsgBuff, unsigned short nMsgLength)
{
	int nFontSize = 12;

	char szChatBuffer[MAX_SENTENCE_LENGTH];

	memset(szChatBuffer, 0, sizeof(szChatBuffer));

	if (nMsgLength)
	{
		int nOffset = 0;
		if (pMsgBuff[0] != KTC_TAB)
		{
			szChatBuffer[nOffset] = (char)KTC_COLOR;
			nOffset++;
			szChatBuffer[nOffset] = (char)0xFF;
			nOffset++;
			szChatBuffer[nOffset] = (char)0xFF;
			nOffset++;
			szChatBuffer[nOffset] = (char)0x00;
			nOffset++;
			//<==== Modified by Ray [LuoLiang] [2005-8-8] ====>
			if ( Name[0] != 0 )
			{
				strncpy(szChatBuffer + nOffset, Name, 32);
				nOffset += strlen(Name);
				szChatBuffer[nOffset] = ':';
				nOffset++;
			}
			
			szChatBuffer[nOffset] = (char)KTC_COLOR_RESTORE;
			nOffset++;
		}
		else
		{
			pMsgBuff ++;
			nMsgLength --;
		}

		if (nMsgLength)
		{
			memcpy(szChatBuffer + nOffset, pMsgBuff, min(nMsgLength,200) );
			nOffset += nMsgLength;

			memset(m_szChatBuffer, 0, sizeof(m_szChatBuffer));
			m_nChatContentLen = MAX_SENTENCE_LENGTH;
			TGetLimitLenEncodedString(szChatBuffer, nOffset, nFontSize, SHOW_CHAT_WIDTH,
				m_szChatBuffer, m_nChatContentLen, 2, true);

			m_nChatNumLine = TGetEncodedTextLineCount(m_szChatBuffer, m_nChatContentLen, SHOW_CHAT_WIDTH, m_nChatFontWidth, nFontSize, 0, 0, true);
//			if (m_nChatNumLine >= 2)
//				m_nChatNumLine = 2;
			m_nCurChatTime = IR_GetCurrentTime();
			return true;
		}
	}
	return false;
}

void KNpc::PaintBubble(void)
{
	int	nMpsX, nMpsY;
	GetMpsPos(&nMpsX, &nMpsY);

	g_pRepresent->CoordinateTransform( nMpsX, nMpsY, GetNpcPate() + GetNpcPatePeopleInfo() );

	Position p;
	p.x = nMpsX;
	p.y = nMpsY;
	CoreDataChanged(GDCNI_ROLEHEADINFO_BUBBLE_UPDATE, m_dwID, (unsigned int)&p);

}

int	KNpc::PaintLife(int nHeightOffset, bool bSelect)
{
	if (m_RegionIndex == -1)
			return 0 ;

	if ( m_Kind == kind_normal && ( m_Doing == do_revive || m_Doing == do_death || GetCurrentLifePercentage() == 0) )
	{
		return 0;
	}

	if (IsEmployee())
		return nHeightOffset;
	
	if ( m_Kind == kind_player || 
		m_Kind == kind_employee ||
		m_Kind == kind_creature || 
		m_Kind == kind_normal || 
		(m_Kind == kind_dialoger && 
		g_pNpcTemplate[m_NpcSettingIdx][m_Level] != NULL &&
		KNpcRes::m_bNpcLeftBarTyp[g_pNpcTemplate[m_NpcSettingIdx][m_Level]->m_nLifeBarStyle] == 1))
	{
		if ( m_CompAttrMgr[ncai_lifeuplimit] <= 0)
			return nHeightOffset;

		int	nMpsX, nMpsY;
		GetMpsPos(&nMpsX, &nMpsY);	
		int nWid = SHOW_LIFE_WIDTH;
		int nHei = SHOW_LIFE_HEIGHT;
		KRUShadow	Blood;
		int nX = GetCurrentLifePercentage();

		//不显示0% 的情况.
 		if (nX == 0 && (m_Kind != kind_player))
 			nX = 1;
		
		if ( m_Kind == kind_player || m_Kind == kind_employee || g_pNpcTemplate[m_NpcSettingIdx][m_Level] != NULL )
		{
			int nLifeBarType;
			bool bShowLifePercent;
			int nBloodMuti = 0.0f;
			if ( kind_player == m_Kind || kind_employee == m_Kind )
			{
				nLifeBarType = 0;
				bShowLifePercent = true;
				nBloodMuti = 0;
			}
			else
			{
				nLifeBarType = g_pNpcTemplate[m_NpcSettingIdx][m_Level]->m_nLifeBarStyle;
				bShowLifePercent = g_pNpcTemplate[m_NpcSettingIdx][m_Level]->m_bShowLifePercent;
				nBloodMuti = g_pNpcTemplate[m_NpcSettingIdx][m_Level]->m_nLifeMultiple;
			}
			
			if ( nLifeBarType >= 0 && nLifeBarType < KNpcRes::MAX_NPC_LIFE_BAR_TYPE )
			{
				//draw blood bar back ground
				KUiImagePartRef & imgBkgnd = KNpcRes::m_imgNpcBkgnd[nLifeBarType];
				KUiImagePartRef & imgBar = KNpcRes::m_imgNpcLifeBar[nLifeBarType];
				g_pRepresent->CoordinateTransform( nMpsX, nMpsY, nHeightOffset + imgBkgnd.Height );
				imgBkgnd.oPosition.nX = nMpsX - imgBkgnd.Width / 2;
				imgBkgnd.oPosition.nY = nMpsY;
				
				g_pRepresent->DrawPrimitives( 1, &imgBkgnd, RU_T_IMAGE, TRUE );

				//draw blood bar
				int nFrame = 0;
				if ( nBloodMuti > 1 )
				{
					nFrame = (nX * nBloodMuti) / 100;
					nX = (nX * nBloodMuti) % 100;
				}
				else
				{
					if (nX >= 50)
					{
						imgBar.nFrame = 0;	
					}
					else if (nX >= 25)
					{
						imgBar.nFrame = 1;
					}
					else
					{
						imgBar.nFrame = 2;
					}							
				}
				imgBar.oPosition.nX = imgBkgnd.oPosition.nX + KNpcRes::m_ptNpcLifeBarPos[nLifeBarType].x;
				imgBar.oPosition.nY = imgBkgnd.oPosition.nY + KNpcRes::m_ptNpcLifeBarPos[nLifeBarType].y;
				
				imgBar.oImgLTPos.nX = 0;
				imgBar.oImgLTPos.nY = 0;
				

				imgBar.oImgRBPos.nY = imgBar.Height;
				if ( nBloodMuti > 1  )
				{
					imgBar.oImgRBPos.nX = imgBar.Width;
					imgBar.nFrame = nFrame - 1; 
					g_pRepresent->DrawPrimitives( 1, &imgBar, RU_T_IMAGE_PART, TRUE );
					imgBar.oImgRBPos.nX = imgBar.Width * nX / 100;
					imgBar.nFrame = nFrame;
					g_pRepresent->DrawPrimitives( 1, &imgBar, RU_T_IMAGE_PART, TRUE );
				}
				else
				{
					imgBar.oImgRBPos.nX = imgBar.Width * nX / 100;
					g_pRepresent->DrawPrimitives( 1, &imgBar, RU_T_IMAGE_PART, TRUE );
				}
				


				
				//draw level
				int leveldifference = m_Level - Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level;
				int nValue0 = ConfigManager::Singleton().GetGlobalVariable(global_var_npc_level_displayer_value0);
				int nValue1 = ConfigManager::Singleton().GetGlobalVariable(global_var_npc_level_displayer_value1);
				int nValue2 = ConfigManager::Singleton().GetGlobalVariable(global_var_npc_level_displayer_value2);
				int nValue3 = ConfigManager::Singleton().GetGlobalVariable(global_var_npc_level_displayer_value3);
				KUiImagePartRef * pLvTxtImg;
				if ( Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index || m_Kind == kind_player || m_Kind == kind_employee )						
				{
					pLvTxtImg = &KNpcRes::m_imgNumber;
					int nLevel = m_Level;
					int nLen = 1;
					int nTmp;
					for ( nTmp = 9; nTmp < nLevel; ++nLen, nTmp = nTmp * 10 + 9 );
					
					pLvTxtImg->oPosition.nX = imgBkgnd.oPosition.nX + KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].x + 
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cx - pLvTxtImg->Width * nLen ) / 2 + pLvTxtImg->Width * ( nLen - 1 );
					
					pLvTxtImg->oPosition.nY = imgBkgnd.oPosition.nY + KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].y + 
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cy - pLvTxtImg->Height ) / 2;
					
					while ( nLevel > 0 )
					{
						int nNum = nLevel % 10;									
						pLvTxtImg->nFrame = nNum;
						g_pRepresent->DrawPrimitives( 1, pLvTxtImg, RU_T_IMAGE, TRUE );
						pLvTxtImg->oPosition.nX -= pLvTxtImg->Width;								
						nLevel /= 10;						
					}
					if ( bShowLifePercent )				
					{
						int nLifePercent = nX;					
						for ( nLen = 2, nTmp = 9; nTmp < nLevel; ++nLen, nTmp = nTmp * 10 + 9 );
						
						pLvTxtImg->oPosition.nX = imgBar.oPosition.nX +
							(imgBar.Width - pLvTxtImg->Width * nLen ) / 2 + pLvTxtImg->Width * ( nLen - 1 );
						pLvTxtImg->oPosition.nY = imgBar.oPosition.nY + ( imgBar.Height - pLvTxtImg->Height ) / 2;
						pLvTxtImg->nFrame = 10;				
						g_pRepresent->DrawPrimitives( 1, pLvTxtImg, RU_T_IMAGE, TRUE );
						
						pLvTxtImg->oPosition.nX -= pLvTxtImg->Width;
						
						while ( nLifePercent > 0 )
						{
							int nNum = nLifePercent % 10;
							pLvTxtImg->nFrame = nNum;
							g_pRepresent->DrawPrimitives( 1, pLvTxtImg, RU_T_IMAGE, TRUE );
							pLvTxtImg->oPosition.nX -= pLvTxtImg->Width;
							nLifePercent /= 10;
						}
					}
					
				}
				else if ( leveldifference < nValue0 )
				{
					pLvTxtImg = &KNpcRes::m_imgExceed;
					pLvTxtImg->nFrame = 0;
					pLvTxtImg->oPosition.nX = imgBkgnd.oPosition.nX + 
						KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].x + 
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cx - KNpcRes::m_imgExceed.Width ) / 2;
					pLvTxtImg->oPosition.nY = imgBkgnd.oPosition.nY + KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].y +
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cy - KNpcRes::m_imgExceed.Height ) / 2;
					
					g_pRepresent->DrawPrimitives( 1, pLvTxtImg, RU_T_IMAGE, TRUE );
				}
				else if ( leveldifference >= nValue0 && leveldifference < nValue1 )
				{
					pLvTxtImg = &KNpcRes::m_imgExceed;
					pLvTxtImg->nFrame = 1;
					pLvTxtImg->oPosition.nX = imgBkgnd.oPosition.nX + 
						KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].x + 
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cx - KNpcRes::m_imgExceed.Width ) / 2;
					pLvTxtImg->oPosition.nY = imgBkgnd.oPosition.nY + KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].y +
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cy - KNpcRes::m_imgExceed.Height ) / 2;
					
					g_pRepresent->DrawPrimitives( 1, pLvTxtImg, RU_T_IMAGE, TRUE );
				}
				else if ( leveldifference >= nValue1 && leveldifference < nValue2 )
				{
					pLvTxtImg = &KNpcRes::m_imgExceed;
					pLvTxtImg->nFrame = 2;
					pLvTxtImg->oPosition.nX = imgBkgnd.oPosition.nX + 
						KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].x + 
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cx - KNpcRes::m_imgExceed.Width ) / 2;
					pLvTxtImg->oPosition.nY = imgBkgnd.oPosition.nY + KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].y +
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cy - KNpcRes::m_imgExceed.Height ) / 2;
					
					g_pRepresent->DrawPrimitives( 1, pLvTxtImg, RU_T_IMAGE, TRUE );					
				}
				else if ( leveldifference >= nValue2 && leveldifference < nValue3 )
				{
					pLvTxtImg = &KNpcRes::m_imgExceed;
					pLvTxtImg->nFrame = 3;
					pLvTxtImg->oPosition.nX = imgBkgnd.oPosition.nX + 
						KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].x + 
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cx - KNpcRes::m_imgExceed.Width ) / 2;
					pLvTxtImg->oPosition.nY = imgBkgnd.oPosition.nY + KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].y +
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cy - KNpcRes::m_imgExceed.Height ) / 2;
					
					g_pRepresent->DrawPrimitives( 1, pLvTxtImg, RU_T_IMAGE, TRUE );					
				}
				else
				{
					pLvTxtImg = &KNpcRes::m_imgExceed;
					pLvTxtImg->nFrame = 4;
					pLvTxtImg->oPosition.nX = imgBkgnd.oPosition.nX + 
						KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].x + 
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cx - KNpcRes::m_imgExceed.Width ) / 2;
					pLvTxtImg->oPosition.nY = imgBkgnd.oPosition.nY + KNpcRes::m_ptNpcLevelTextStartPos[nLifeBarType].y +
						( KNpcRes::m_sizeNpcLevelTxtRect[nLifeBarType].cy - KNpcRes::m_imgExceed.Height ) / 2;
					
					g_pRepresent->DrawPrimitives( 1, pLvTxtImg, RU_T_IMAGE, TRUE );
				}
				
				
				return nHeightOffset + imgBkgnd.Height;
			}		
			
		}
		return nHeightOffset + nHei;
	}
	return nHeightOffset;	
}

int	KNpc::PaintMana(int nHeightOffset)
{
	if (m_Kind != kind_player &&
		m_Kind != kind_partner)
		return nHeightOffset;

	if (m_CompAttrMgr[ncai_manauplimit] <= 0)
		return nHeightOffset;

	return nHeightOffset;

	int	nMpsX, nMpsY;
	GetMpsPos(&nMpsX, &nMpsY);
	int nWid = 38;
	int nHei = 5;
	KRUShadow	Blood;
	int nX = m_UnaryAttrMgr[nuai_curmana] * 100 / m_CompAttrMgr[ncai_manauplimit];
	if (nX >= 50)
	{
		Blood.Color.Color_b.r = 0;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 255;
	}
	else if (nX >= 25)
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 255;
		Blood.Color.Color_b.b = 0;
	}
	else
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 0;
	}
	Blood.Color.Color_b.a = 0;
	Blood.oPosition.nX = nMpsX - nWid / 2;
	Blood.oPosition.nY = nMpsY;
	Blood.oPosition.nZ = nHeightOffset + nHei;
	Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nX / 100;
	Blood.oEndPos.nY = nMpsY;
	Blood.oEndPos.nZ = nHeightOffset;
	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);

	Blood.Color.Color_b.r = 255;
	Blood.Color.Color_b.g = 255;
	Blood.Color.Color_b.b = 255;
	Blood.oPosition.nX = Blood.oEndPos.nX;
	Blood.oEndPos.nX = nMpsX + nWid / 2;
	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);
	
	return nHeightOffset + nHei;
}

// Added by rocker 2004.3.19
extern bool g_bPerspectiveMode;
// added end
void KNpc::Paint()
{
	if (m_RegionIndex == -1)
			return ;

	if (m_ResDir != m_UnaryAttrMgr[nuai_dir])
	{
		int nDirOff = m_UnaryAttrMgr[nuai_dir] - m_ResDir;
		if (nDirOff > 32)
			nDirOff -= 64;
		else if (nDirOff < - 32)
			nDirOff += 64;
		m_ResDir += nDirOff / 2;
		if (m_ResDir >= 64)
			m_ResDir -= 64;
		if (m_ResDir < 0)
			m_ResDir += 64;
	}
	
	//--> Rocker 2004.7.30
				
	if (m_Kind == kind_player)
		CoreDraw::gCurrentPlayerPaintedNum++;
	int targetnpc = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetNpc();
	int nHover = Player[CLIENT_PLAYER_INDEX].GetTargetNpc() == m_Index ? TRUE : FALSE;
	BOOL bDrawSelectedImg = (targetnpc!=0 && targetnpc == m_Index && m_bShowSelect);
	int nSelectedType = m_Kind != kind_normal ? 0 : 1;
	if ( m_eActionType == multi_action_type )
	{
		if ( m_Kind == kind_normal && ( m_Doing == do_revive || m_Doing == do_death || GetCurrentLifePercentage() == 0 ))
		{
			m_DataRes.SetAction(cdo_death);
			m_DataRes.Draw(m_Index, m_ResDir, m_Doing == do_revive ? m_ReviveFrame : m_DeathFrame, 0, 
				FALSE, bDrawSelectedImg, nSelectedType, nHover, multi_action_type, m_nBeginFrame, m_nEndFrame );
		}
		else
		{
			m_DataRes.Draw(m_Index, m_ResDir, m_Frames.nTotalFrame * ATTACKACTION_EFFECT_PERCENT / 100, m_Frames.nCurrentFrame, 
				FALSE, bDrawSelectedImg, nSelectedType, nHover, multi_action_type, m_nBeginFrame, m_nEndFrame );
		}
	}
	else
	{
		if ( m_Kind == kind_normal && ( m_Doing == do_revive || m_Doing == do_death || GetCurrentLifePercentage() == 0  ))
		{
			m_DataRes.SetAction(cdo_death);
			m_DataRes.Draw(m_Index, m_ResDir, m_Doing == do_revive ? m_ReviveFrame : m_DeathFrame, 0, 
				FALSE, bDrawSelectedImg, nSelectedType, nHover);
		}
		else
		{
			m_DataRes.Draw(m_Index, m_ResDir, m_Frames.nTotalFrame, m_Frames.nCurrentFrame, 
				FALSE, bDrawSelectedImg, nSelectedType, nHover);
		}
	}

}

//-------> Ray [Luoliang] 2005-7-11
void KNpc::PaintUI(int x, int y)
{
	if ( m_Index == -1 && m_bCanShowUiLoginPlayer && m_Kind == kind_player )
	{
		switch (m_ClientDoing)
		{
		case do_stand:
			DoStand();
		case do_magic:
		case do_skill:
			DoManyAttack();
			break;
		default:
			DoStand();
			break;
		}

		m_DataRes.SetAction(m_ClientDoing);
		m_DataRes.DrawUI(x, y, m_Index, m_ResDir, m_Frames.nTotalFrame, m_Frames.nCurrentFrame);
		ProcStatus();

	}
	
	
}
//<------- End [Ray]
#endif

void	KNpc::Remove()
{
#ifdef _SERVER
	if (GetSyncToWorldMode() > 0)
	{
		SetSyncToWorldMode(0);
	}
#endif

/*	m_LoopFrames = 0;
	m_Index = 0;
	m_PlayerIdx = -1;
	m_Kind = 0;
	m_dwID = 0;
	Name[0] = 0;*/
#ifndef _SERVER
	RoleHeadInfo roleHeadInfo;
	ZeroMemory( &roleHeadInfo, sizeof(RoleHeadInfo) );
	char npcShowName[33] = {0};
	char* show = 0;
	if(m_Kind == kind_creature)
	{
		strcpy(npcShowName,Name);
		strcat(npcShowName,PLAYER_PET_NAME_EXTRA);
		show = npcShowName;

	}
	else
	{
//		strcpy(npcShowName,Name);
		show = Name;
	}
	roleHeadInfo.dwID = m_dwID;
	CoreDataChanged( GDCNI_ROLEHEADINFO_DEL,(unsigned int)&roleHeadInfo, NULL );
	PolyMorph(-1, TRUE, 0, -1, 0);
	switch( m_Kind )
	{
		case kind_player:
			if ( m_nPetIndex > 0 && m_nPetIndex < MAX_NPC )
			{
				SubWorld[0].m_Region[Npc[m_nPetIndex].m_RegionIndex].RemoveNpc(m_nPetIndex);
				NpcSet.Remove(m_nPetIndex, false);
			}			
			break;		
		default:
			
			break;
	}	

	m_DataRes.Remove(m_Index, true);
#endif

	Init();
}

#ifndef _SERVER
void	KNpc::RemoveRes()
{
	m_DataRes.Remove(m_Index, true);
}
#endif
//--------------------------------------------------------------------------
//	功能：设定此 npc 的五行属性（内容还没完成）not end
//--------------------------------------------------------------------------
void	KNpc::SetSeries(int nSeries)
{
	if (nSeries >= series_metal && nSeries < series_num)
		m_Series = nSeries;
	else
		m_Series = series_metal;
}

int KNpc::ModifyMissleLifeTime(int nLifeTime)
{
	if (IsPlayer())
	{
		//return Player[m_PlayerIdx].GetWeapon().GetRange();
		return nLifeTime;
	}
	else
	{
		return nLifeTime;
	}
}

BOOL KNpc::DoManyAttack()
{
	m_ProcessAI = 0;
	
	KSkill * pSkill = GetActiveSkill();
	if (!pSkill) 
        return FALSE;
	
	if (pSkill->GetChildSkillNum() <= m_SpecialSkillStep) 		
        goto ExitManyAttack;
#ifndef _SERVER
        m_DataRes.SetBlur(TRUE);
#endif
	
	m_Frames.nTotalFrame = pSkill->GetMissleGenerateTime(m_SpecialSkillStep);
	
	int x, y;
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, GetMapX(), GetMapY(), GetOffX(), GetOffY(), &x, &y);
//	m_DesX = x;
//	m_DesY = y;

	
#ifndef _SERVER
	if (m_nPlayerIdx > 0)
		pSkill->PlayPreCastSound(m_nSex, x ,y);
	if (g_Random(2))
		m_ClientDoing = cdo_attack;
	else 
		m_ClientDoing = cdo_attack1;
#endif

	
	m_Doing = do_manyattack;
	
	m_Frames.nCurrentFrame = 0;

	return TRUE;

ExitManyAttack:

#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
	DoStand();
	m_ProcessAI = 1;
	m_SpecialSkillStep = 0;

	return TRUE;
}

void KNpc::OnManyAttack()
{
	if (WaitForFrame())
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		KSkill * pSkill = GetActiveSkill();
		if (!pSkill) 
            return ;

		int nPhySkillId =  pSkill->GetChildSkillId();

		if (nPhySkillId > 0)
		{
			KSkill * pOrdinSkill = g_SkillManager.GetSkill(nPhySkillId, pSkill->GetCurLevel());
			if (pOrdinSkill)
            {	
				pOrdinSkill->Cast(m_Index, m_SkillParam1, 
					m_SkillParam2, 0, SKILL_SLT_Npc, m_ActiveSkillID);
            }
		}
		m_SpecialSkillStep ++;
		DoManyAttack();
	}	
}

BOOL	KNpc::DoRunAttack()
{
	m_ProcessAI = 0;
	
	switch(m_SpecialSkillStep)
	{
	case 0:
		m_Frames.nTotalFrame = m_CompAttrMgr[ncai_runspeed][idx_base_value];
		m_ProcessAI = 0;
		{
			KSkill * pSkill = GetActiveSkill();
			if (!pSkill) 
				return FALSE;
			
			if (pSkill->GetStartSkillId() > 0 && pSkill->GetEventSkillLevel() > 0)
			{
				KSkill * pOrdinSkill = g_SkillManager.GetSkill(pSkill->GetStartSkillId(), pSkill->GetEventSkillLevel());
				if (!pOrdinSkill) 
					return FALSE;
				
				pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2 );
			}

			m_SkillParam3 = pSkill->GetValue1();
			AddCompAttr(ncai_runspeed, idx_append_value, m_SkillParam3);
			m_SkillParam4 = pSkill->GetValue2();
		}
		
		
#ifndef _SERVER
		m_DataRes.SetBlur(TRUE);

		if ( m_UnaryAttrMgr[nuai_fightstate] )
		{
			m_ClientDoing = cdo_fightrun;
		}
		else
		{
			m_ClientDoing = cdo_run;
		}
#endif
		
		if (m_DesX < 0 && m_DesY > 0) 
		{
			int x, y;
			SubWorld[m_SubWorldIndex].Map2Mps
				(
				Npc[m_DesY].m_RegionIndex,
				Npc[m_DesY].GetMapX(), 
				Npc[m_DesY].GetMapY(), 
				Npc[m_DesY].GetOffX(), 
				Npc[m_DesY].GetOffY(), 
				&x,
				&y
				);

		m_DesX = x;
		m_DesY = y;
		}

		m_Frames.nCurrentFrame = 0;
		m_Doing = do_runattack;
		break;

	case 1:
#ifndef _SERVER
		if (g_Random(2))	
			m_ClientDoing = cdo_attack;
		else
			m_ClientDoing = cdo_attack1;

		int x, y, tx, ty;
		SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, GetMapX(), GetMapY(), GetOffX(), GetOffY(), &x, &y);
		if (m_SkillParam1 == SKILL_SPT_TargetIndex)
		{
			Npc[m_SkillParam2].GetMpsPos(&tx, &ty);
		}
		else
		{
			tx = m_SkillParam1;
			ty = m_SkillParam2;
		}
		if (m_Kind != kind_building ) m_UnaryAttrMgr.Set(nuai_dir, g_GetDirIndex(x, y, tx, ty));
#endif
		m_Frames.nTotalFrame = m_SkillParam4;
		m_Frames.nCurrentFrame = 0;
		m_Doing = do_runattack;
		break;

	case 2:
	case 3:
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
		break;
	}

	m_Frames.nCurrentFrame = 0;
			
	return TRUE;
}

void	KNpc::OnRunAttack()
{		
	if (m_SpecialSkillStep == 0)
	{
		OnRun();
		KSkill * pSkill = GetActiveSkill();
		if (!pSkill) 
            return ;
		
        if (m_Doing == do_stand || (DWORD)m_nCurrentMeleeTime > pSkill->GetMissleGenerateTime(0)) 
		{
			m_SpecialSkillStep ++;
			m_nCurrentMeleeTime = 0;

			DoRunAttack();
		
		}
		else
			m_nCurrentMeleeTime ++;

		m_ProcessAI = 0;
	}
	else if (m_SpecialSkillStep == 1)
	{
		if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
		{
			DoStand();
			m_ProcessAI = 1;	
		}
		else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
		{
			KSkill * pSkill = GetActiveSkill();
			if (!pSkill) 
                return ;
			
            int nCurPhySkillId = pSkill->GetChildSkillId();//GetCurActiveWeaponSkill();
			if (nCurPhySkillId > 0)
			{
				KSkill * pOrdinSkill = g_SkillManager.GetSkill(nCurPhySkillId, pSkill->GetCurLevel());
				if (pOrdinSkill)
                {
				    pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
                }
			}

			DoStand();
			m_ProcessAI = 1;
			m_SpecialSkillStep = 0;
		}
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
	}
	else
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
	}
}

BOOL KNpc::CheckHitTarget(int nAR, int nDf, int nIngore/* = 0*/)
{
	int nDefense = nDf * (100 - nIngore) / 100;
	int nPercent = 0;

	if (nAR < 0)
		return FALSE;

	if (nDf < 0)
		nPercent = MAX_HIT_PERCENT;
	else if ((nAR + nDefense) == 0) 
		nPercent = 50;
	else
		nPercent = nAR * 100 / (nAR + nDefense);

	if (nPercent > MAX_HIT_PERCENT)
		nPercent = MAX_HIT_PERCENT;

	if (nPercent < MIN_HIT_PERCENT)
		nPercent = MIN_HIT_PERCENT;

	BOOL bRet = g_RandPercent(nPercent);
	g_DebugLog("AttackRating %d : Defense %d: RandomPercent (%d, %d)", nAR, nDf, nPercent, bRet);
	return bRet;
}

void KNpc::GetNpcCopyFromTemplate(int nNpcTemplateId, int nLevel)
{
	if (nNpcTemplateId < 0 || nLevel < 1 ) 
		return ;
	
	if (g_pNpcTemplate[nNpcTemplateId][nLevel]) //数据有效则拷贝，否则重新生成
	{
		LoadDataFromTemplate(nNpcTemplateId, nLevel);
	}
	else
	{
		if (!g_pNpcTemplate[nNpcTemplateId][0])
		{
			g_pNpcTemplate[nNpcTemplateId][0] = new KNpcTemplate;
			g_pNpcTemplate[nNpcTemplateId][0]->InitNpcBaseData(nNpcTemplateId);
			g_pNpcTemplate[nNpcTemplateId][0]->m_NpcSettingIdx = nNpcTemplateId;
			g_pNpcTemplate[nNpcTemplateId][0]->m_bHaveLoadedFromTemplate = TRUE;
		}
		KLuaScript * pLevelScript = NULL;		

#ifdef _SERVER
			pLevelScript = (KLuaScript*)g_GetScript(
			g_pNpcTemplate[nNpcTemplateId][0]->m_dwLevelSettingScript
			);
		
		if (pLevelScript == NULL)
			pLevelScript = g_pNpcLevelScript;
#else
		KLuaScript LevelScript;
		if (!g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript[0])
			pLevelScript = g_pNpcLevelScript;
		else
		{
			LevelScript.Init();
			if (!LevelScript.Load(g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript))
			{
				g_DebugLog ("[error]can 't read file %s", g_pNpcTemplate[nNpcTemplateId][0]->m_szLevelSettingScript);
				_ASSERT(0);
				pLevelScript = g_pNpcLevelScript;
			}
			else
				pLevelScript = &LevelScript;
		}

#endif
		g_pNpcTemplate[nNpcTemplateId][nLevel] = new KNpcTemplate;
		*g_pNpcTemplate[nNpcTemplateId][nLevel] = *g_pNpcTemplate[nNpcTemplateId][0];
		g_pNpcTemplate[nNpcTemplateId][nLevel]->m_nLevel = nLevel;
		g_pNpcTemplate[nNpcTemplateId][nLevel]->InitNpcLevelData(&g_NpcKindFile, nNpcTemplateId, pLevelScript, nLevel);
		g_pNpcTemplate[nNpcTemplateId][nLevel]->m_bHaveLoadedFromTemplate = TRUE;
		LoadDataFromTemplate(nNpcTemplateId,nLevel);
	}
}

void	KNpc::LoadDataFromTemplate(int nNpcTemplateId, int nLevel)
{
	if (nNpcTemplateId < 0 )
	{
		g_DebugLog("load Npc%d template error !", nNpcTemplateId);
		return ;
	}
	
	KNpcTemplate * pNpcTemp = g_pNpcTemplate[nNpcTemplateId][nLevel];
	m_pTemplate = pNpcTemp;

	strcpy(Name,pNpcTemp->Name);
	m_Kind = pNpcTemp->m_Kind;
	m_Camp = pNpcTemp->m_Camp;

	m_UnaryAttrMgr.Set(nuai_pkmode, pNpcTemp->m_Camp);

	m_Series = pNpcTemp->m_Series;
	strcpy(m_HeadImageSet, pNpcTemp->m_HeadImageSet);
	strcpy(m_HeadImage, pNpcTemp->m_HeadImage);
	m_bClientOnly = pNpcTemp->m_bClientOnly;
	m_CorpseSettingIdx =	pNpcTemp->m_CorpseSettingIdx;
	// lixuewu 增加阻挡信息
	m_nBarrierWidth  = pNpcTemp->m_nBarrierWidth;
	m_nBarrierHeight = pNpcTemp->m_nBarrierHeight;
	// lixuewu 增加阻挡信息		
	m_DeathFrame =	pNpcTemp->m_DeathFrame;
	m_WalkFrame =	pNpcTemp->m_WalkFrame;
	m_RunFrame =	pNpcTemp->m_RunFrame;
	m_HurtFrame =	pNpcTemp->m_HurtFrame;

	m_CompAttrMgr.Set(ncai_walkspeed, idx_base_value, pNpcTemp->m_WalkSpeed);
	m_CompAttrMgr.Set(ncai_runspeed, idx_base_value, pNpcTemp->m_RunSpeed);

	m_AttackFrame =	pNpcTemp->m_AttackFrame;
	m_CastFrame =	pNpcTemp->m_CastFrame;
	m_StandFrame =  pNpcTemp->m_StandFrame;
	m_StandFrame1 = pNpcTemp->m_StandFrame1;
	m_NpcSettingIdx = pNpcTemp->m_NpcSettingIdx;
	m_nStature		= pNpcTemp->m_nStature;
	// Ai Param 也要初始化,lixuewu 2004.11.25 解决召唤兽的BUG
	m_AiMode		= pNpcTemp->m_AiMode;
	for (unsigned int j = 0; j < MAX_AI_PARAM - 1; j ++)
		m_AiParam[j] =	pNpcTemp->m_AiParam[j]; // 最后的Param表示攻击范围不能冲掉
	
	// --> Rocker Edit Start 2005/12/10
#ifndef _SERVER
	m_bShowSelect = pNpcTemp->m_bDisplaySelect;
	m_bShowTargetFace = pNpcTemp->m_bShowTargetFace;
	if (m_Kind == kind_siege_weapon)
		m_CompAttrMgr.Set(ncai_attackradius, idx_base_value, pNpcTemp->m_AttackRadius[0]);
#endif
	// <-- Rocker End
	
	m_CompAttrMgr.Set(ncai_lifeuplimit, idx_base_value, pNpcTemp->m_LifeMax);

#ifdef _SERVER	
	m_Treasure		= pNpcTemp->m_Treasure;
	// Add by Cooler 2004-7-23
	// Begin -->
	m_Treasure1		= pNpcTemp->m_Treasure1;
	// End <--

	m_LifeLimitedHoldPercentage = pNpcTemp->m_nLifeLimitedHold;

	m_SkillList		= pNpcTemp->m_SkillList;

	m_SkillList.CastAllValueSkill();
	m_SkillList.CastAllPassiveSkill();

	m_AiAddLifeTime	= 0;

	m_pDropRate		= pNpcTemp->m_pItemDropRate;
	for ( int nDropIdx = 0; nDropIdx < MAX_DROP_GROUP; ++nDropIdx )
	{
		m_pDropRateGroup[nDropIdx] = pNpcTemp->m_pItemDropGroupRate[nDropIdx];
	}


	//--> Rocker 2005/05/30
	m_dwDeathScriptID = pNpcTemp->m_dwDeathScriptID;
	//<-- End
	
    m_ActionScriptID = pNpcTemp->m_dwActionScriptID;

	// add by chenshanglin on 2006-2-23 for new skill system
	m_nDropRateAttenuation = pNpcTemp->m_nDropRateAttenuation;
	// add end
	
	if (!m_AiSkillRadiusLoadFlag)
	{
		m_AiSkillRadiusLoadFlag = 1;
		int		nMaxRadius = 0, nTempRadius;

		int		nSkillIdx;
		NpcSkillList::Iterator iter;

		while( (nSkillIdx = m_SkillList.NextSkillIdx(iter)) != INVALID_SKILL_INDEX )
		{
			KSkill *pSkill = g_SkillManager.GetSkill(m_SkillList.GetIdByIdx(nSkillIdx), 
													 m_SkillList.GetLevelByIdx(nSkillIdx)
													);

			if (!pSkill)
				continue;

			nTempRadius = pSkill->GetAttackRadius();

			if (nTempRadius > nMaxRadius)
				nMaxRadius = nTempRadius;
		}

		m_AiParam[MAX_AI_PARAM - 1] = nMaxRadius * nMaxRadius;
	}

	m_AIMAXTime				= pNpcTemp->m_AIMAXTime;
	m_ReviveFrame			= pNpcTemp->m_ReviveFrame;

	m_Controller.SetSkillStrategyList(pNpcTemp->GetSkillStrategyList(), pNpcTemp->GetSkillStrategyCount());

	m_CompAttrMgr.Set(ncai_activeradius, idx_base_value, pNpcTemp->m_ActiveRadius);
	m_CompAttrMgr.Set(ncai_visionradius, idx_base_value, pNpcTemp->m_VisionRadius);
	m_UnaryAttrMgr.Set(nuai_experience, pNpcTemp->m_Experience);
	m_UnaryAttrMgr.Set(nuai_skillexp, pNpcTemp->m_SkillExp);
	m_CompAttrMgr.Set(ncai_liferenewspeed, idx_base_value, pNpcTemp->m_LifeReplenish);
	m_CompAttrMgr.Set(ncai_vision, idx_base_value, pNpcTemp->m_AttackRating);
	m_CompAttrMgr.Set(ncai_dexterity, idx_base_value, pNpcTemp->m_Defend);

	SetRABaseValue(nrai_defend_farphysics, pNpcTemp->m_FarPhysResistLow, pNpcTemp->m_FarPhysResistHight);
	SetRABaseValue(nrai_defend_nearphysics, pNpcTemp->m_NearPhysResistLow, pNpcTemp->m_NearPhysResistHight);
	SetRABaseValue(nrai_defend_water, pNpcTemp->m_WaterResistLow, pNpcTemp->m_WaterResistHight);
	SetRABaseValue(nrai_defend_fire, pNpcTemp->m_FireResistLow, pNpcTemp->m_FireResistHight);
	SetRABaseValue(nrai_defend_thunder, pNpcTemp->m_ThunderResistLow, pNpcTemp->m_ThunderResistHight);
	SetRABaseValue(nrai_defend_wind, pNpcTemp->m_WindResistLow, pNpcTemp->m_WindResistHight);
	SetRABaseValue(nrai_defend_shadow, pNpcTemp->m_ShadowResistLow, pNpcTemp->m_ShadowResistHight);
	SetRABaseValue(nrai_defend_poison, pNpcTemp->m_PoisonResistLow, pNpcTemp->m_PoisonResistHight);

	SetRABaseValue(nrai_damage_farphysics, pNpcTemp->m_FarPhysDamLow, pNpcTemp->m_FarPhysDamHight);
	SetRABaseValue(nrai_damage_nearphysics, pNpcTemp->m_NearPhysDamLow, pNpcTemp->m_NearPhysDamHight);
	SetRABaseValue(nrai_damage_water, pNpcTemp->m_WaterDamLow, pNpcTemp->m_WaterDamHight);
	SetRABaseValue(nrai_damage_fire, pNpcTemp->m_FireDamLow, pNpcTemp->m_FireDamHight);
	SetRABaseValue(nrai_damage_thunder, pNpcTemp->m_ThunderDamLow, pNpcTemp->m_ThunderDamHight);
	SetRABaseValue(nrai_damage_wind, pNpcTemp->m_WindDamLow, pNpcTemp->m_WindDamHight);
	SetRABaseValue(nrai_damage_shadow, pNpcTemp->m_ShadowDamLow, pNpcTemp->m_ShadowDamHight);
	SetRABaseValue(nrai_damage_poison, pNpcTemp->m_PoisonDamLow, pNpcTemp->m_PoisonDamHight);

    // add by hejianfeng.  2005-10-24
    m_nNpcColor = pNpcTemp->m_nColor;
    m_nDeadlyStrikeResist = pNpcTemp->m_nDeadlyStrikeResist;
    m_nFatallyStrikeResist = pNpcTemp->m_nFatallyStrikeResist;
    // endadd	
	m_nFreezeTimeReduce = pNpcTemp->m_nFreezeTimeReduce;

	m_ActionTime = pNpcTemp->m_dwActionTime;

	if (pNpcTemp->NeedSave())
		SetLoadSaveState(npc_load_save_state_waiting);
	else
		SetLoadSaveState(npc_load_save_state_loaded);

#else
	m_ArmorType				= pNpcTemp->m_ArmorType;
	m_HelmType				= pNpcTemp->m_HelmType;
	m_WeaponType			= pNpcTemp->m_WeaponType;
	m_ShoulderType			= pNpcTemp->m_ShoulderType;
	m_HorseType				= pNpcTemp->m_HorseType;
	m_BootType				= pNpcTemp->m_BootType;
	m_CuffType				= pNpcTemp->m_CuffType;
	m_bRideHorse			= pNpcTemp->m_bRideHorse;
#endif
	
	m_Level = pNpcTemp->m_nLevel;
	m_WorldCombatKilledScore      = pNpcTemp->m_CombatScore;
	m_WorldCombatOrg              = pNpcTemp->m_CombatOrg;

	RestoreNpcBaseInfo();	
}

//-----------------------------------------------------------------------
//	功能：一个命令是否已经在NPC的命令队列里
//-----------------------------------------------------------------------
BOOL KNpc::IsCommandExist(NPCCMD eCmd)
{
	if(m_Command.CmdKind == eCmd)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------------------------------
//	功能：设定攻击命中率
//-----------------------------------------------------------------------
void	KNpc::SetBaseAttackRating(int nAttackRating)
{
	m_CompAttrMgr.Set(ncai_vision, idx_base_value, nAttackRating);
	m_CompAttrMgr.Set(ncai_vision, idx_append_value, 0);
	m_CompAttrMgr.Set(ncai_vision, idx_append_percent, 0);
}

#ifdef _SERVER

extern int g_nRedNameIslandMapId;
int KNpc::DeathPunish(int nMode, int nBelongPlayer)
{
	int nRetExp = 0;
	
	if (m_nCurPKPunishState == 1)
		return 0;

	if (IsPlayer())
	{
		if( m_UnaryAttrMgr[nuai_deathmode] & npc_deathmode_nopunish )
			return 0;

		if( IsInPKArea( ) )
			return 0;

		Player[GetPlayerIdx()].DeathPunish( );//玩家死亡惩罚
	}
	else if ( IsValidPlayer(nBelongPlayer) )
	{
		if( m_UnaryAttrMgr[nuai_deathmode] & npc_deathmode_nodrop )
			return 0;

		int nAntiEnthrallState = Player[nBelongPlayer].m_AntiEnthrall.GetCurState();

		if(AntiEnthrall::enAntiEnthrall_Insalubrity == nAntiEnthrallState)
			return 0;

		bool bDrop = true;

		if(AntiEnthrall::enAntiEnthrall_Weariness == nAntiEnthrallState)
			bDrop = (TRUE == g_RandPercent(100 / AntiEnthrall::WEARINESS_EXP_SCALE));

		DWORD nOldExp = Player[nBelongPlayer].GetExp();
		ExpManager::Singleton().DistributeExp(m_Index, nBelongPlayer);
		DWORD nNewExp = Player[nBelongPlayer].GetExp();
		//if (nNewExp != nOldExp)
		Player[nBelongPlayer].GetFurySys().NpcKillingNotify();

		//默认掉落
		if(m_pDropRate && bDrop)
		{
			LoseMoney(m_pDropRate, nBelongPlayer);
			LoseItems(m_pDropRate, nBelongPlayer);
		}
		//分组掉落
		for ( int nDropIdx = 0; nDropIdx < MAX_DROP_GROUP; ++nDropIdx )
		{
			if ( m_pDropRateGroup[nDropIdx] && bDrop )
			{
				LoseSingleItem(m_pDropRateGroup[nDropIdx], nBelongPlayer);
			}
		}
	}

	return nRetExp;
}

void KNpc::LoseMoney(KItemDropRate *pDropRate, int nBelongPlayer, float fScale)
{
	UINT uRet = 0;
	if ( (uRet =g_Random(pDropRate->ulMaxRandRate)) >= pDropRate->ulMoneyRandRate )
	{
		return;
	}
	int nX, nY;
	POINT	ptLocal;
	KMapPos	Pos;

	unsigned long ulMoney = pDropRate->ulMoneyMin + g_Random( pDropRate->ulMoneyMax - pDropRate->ulMoneyMin );
	ulMoney *= fScale; 

	if (ulMoney <= 0)
		return;


	GetMpsPos(&nX, &nY);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
	
	int nBelong = -1;
	
	if (nBelongPlayer > 0)
		nBelong = nBelongPlayer;

	int nObjIdx = ObjSet.AddMoneyObj(Pos, ulMoney , nBelong);
	
	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		KObj& dropObject =Object[nObjIdx];
				
		if (nBelongPlayer > 0)
		{
			//设置物品属主和属主时间
			dropObject.SetItemBelongTime(ConfigManager::Singleton().GetGlobalVariable(global_var_object_item_belong_time));
		}	
	}

	unsigned long dropMoney = ulMoney;
	if (dropMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_npc_drop_money_amount))
	{
		//掉落金钱日志
		LogEventParam logEventParam;
		logEventParam.event = log_event_npc_drop_money;
		snprintf(logEventParam.param1.data, sizeof(logEventParam.param1.data), "%d", m_NpcSettingIdx);
		logEventParam.param4 = dropMoney;
		g_pLogSystem->Log(logEventParam);
	}
}

//--> Rocker 2004/08/23
// 新Random 函数
UINT New_Random(UINT nMax)
{
	if (nMax)
	{
		unsigned int f = g_nNewRandomSeed * 0x08088405 + 1;
		g_nNewRandomSeed = f;
		#ifndef _WIN32
		long long t = (long long)f * (long long)nMax;
		#else
		_int64 t = (_int64)f * (_int64)nMax;
		#endif
		t = t >> 32;
		return (unsigned int)t;
	}
	else
	{
		return 0;
	}
}
//--> Rocker

BOOL KNpc::LoseItems(KItemDropRate *pDropRate, int nBelongPlayer, BOOL bAutoPicked, BOOL *pNoPlace, float fScale)
{
	if(pNoPlace != NULL)
	{
		*pNoPlace = FALSE;
	}

	if (!pDropRate)
	{
		return false;
	}

	for ( int i = 0; i < pDropRate->nCount; ++i)
	{
		UINT dRand = g_Random( pDropRate->ulMaxRandRate);

		if ( dRand < pDropRate->pItemParam[i].ulRate )
		{
			int nGenre,nDetail, nParticular, nLevel, nCount;
			nGenre		= pDropRate->pItemParam[i].nGenre;
			nDetail		= pDropRate->pItemParam[i].nDetailType;
			nParticular	= pDropRate->pItemParam[i].nParticulType;
			nLevel		= pDropRate->pItemParam[i].nLevel;
			nCount		= pDropRate->pItemParam[i].nItemCount;

			int nIdx = ItemSet.Add(
				nGenre, nDetail, nParticular, nLevel, nCount);

			//服务器的Item数组已经满了
			if (nIdx <= 0)
			{
				if(pNoPlace != NULL)
				{
					*pNoPlace = TRUE;
				}
				return false;
			}
						
			KItem& dropItem = Item[nIdx];
			if (dropItem.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
			{
				//掉落物品日志
				LogEventParam logEventParam;
				logEventParam.event = log_event_npc_drop_item;
				snprintf(logEventParam.param1.data, sizeof(logEventParam.param1.data), "%d", m_NpcSettingIdx);
				logEventParam.param2 = dropItem.GetGUID();
				dropItem.GetItemTemplateId(logEventParam.param3.data, sizeof(logEventParam.param3.data) - 1);
				g_pLogSystem->Log(logEventParam);
			}
			
			//NPC统计：掉落物品
			if (m_pTemplate)
			{
				ItemTemplateId templateId;
				dropItem.GetItemTemplateId(templateId);
				m_pTemplate->GetStatisticInfo().DropItem(templateId);
			}

			if(bAutoPicked)//直接将掉落物品给获得物品的玩家
			{
				EXTRAINFOPLUS tagExtraPlus;
				tagExtraPlus.nItemGenre = Item[nIdx].GetGenre();
				tagExtraPlus.nParticularType = Item[nIdx].GetParticular();
				tagExtraPlus.nDetailType = Item[nIdx].GetDetailType();
				tagExtraPlus.nMaxItem = Item[nIdx].GetMaxItemCount();
				tagExtraPlus.nCurItem = Item[nIdx].GetItemCount();
				tagExtraPlus.pCampareItem = &Item[nIdx];
				ItemPos	sItemPos;
				if(Player[nBelongPlayer].m_ItemList.SearchPosition(&sItemPos, &tagExtraPlus))
				{
					if (!Player[nBelongPlayer].m_ItemList.Add(nIdx, sItemPos.nPlace, sItemPos.nX, sItemPos.nY, NULL, item_sync_type_pickup))
					{
						ItemSet.Remove(nIdx);
						return false;
					}
				}
				else
				{
					//获得物品玩家item数组已经满了					
					if(pNoPlace != NULL)
					{
						*pNoPlace = TRUE;
					}	
					
					ItemSet.Remove(nIdx);
					return false;	
				}
			}
			else//物品掉在地上
			{
				int		nX, nY;
				POINT	ptLocal;
				KMapPos	Pos;

				GetMpsPos(&nX, &nY);
				ptLocal.x = nX;
				ptLocal.y = nY;			
				SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);

				Pos.nSubWorld = m_SubWorldIndex;
				SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, &Pos.nRegion, &Pos.nMapX, &Pos.nMapY, &Pos.nOffX, &Pos.nOffY);

				int nObj;
				KObjItemInfo sInfo;
				sInfo.m_nItemID = nIdx;
				sInfo.m_nMoneyNum = (Item[nIdx].GetMaxItemCount()==0)?0:Item[nIdx].GetItemCount();	
				if ( Item[nIdx].GetYaoID() == yao_yin )
				{
					sprintf( sInfo.m_szName, "[%s]%s", YIN, Item[nIdx].GetName() );
				}
				else if ( Item[nIdx].GetYaoID() == yao_yang )
				{
					sprintf( sInfo.m_szName, "[%s]%s", YANG, Item[nIdx].GetName() );
				}
				else
				{
					memcpy(sInfo.m_szName, Item[nIdx].GetName(), FILE_NAME_LENGTH );
				}				
				sInfo.m_szName[FILE_NAME_LENGTH - 1] = 0;
				sInfo.m_nColorID = Item[nIdx].GetQualityLabel();
				sInfo.m_nMovieFlag = 1;
				sInfo.m_nSoundFlag = 1;

				int nBelong = -1;
				if (nBelongPlayer > 0)
					nBelong = nBelongPlayer;
				
				nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo , nBelong);
				if (nObj == -1)
				{
					//服务器的Obj数组已经满了
					ItemSet.Remove(nIdx);
					if(pNoPlace != NULL)
					{
						*pNoPlace = TRUE;
					}
					return false;
				}
				else
				{
					//ItemDebugLog Begin........................
                    #ifdef _SERVER
					int nItemIndex = nIdx;
					if (nItemIndex > 0 && nItemIndex < MAX_ITEM && Item[nItemIndex].GetBelong() != -1)
					{
						int  nBelong         = Item[nItemIndex].GetBelong();
						char szDumpInfo[512] = "";
						snprintf(szDumpInfo,sizeof(szDumpInfo),"Npc Lose:Index %d,Belong:%d,Name:%s\n",nItemIndex,nBelong,Item[nItemIndex].GetName());
						szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
						DumpInvalidItemOpeStack(false,szDumpInfo,4);
					}//endif
                    #endif
                    //ItemDebugLog End.........................

					KObj& dropObject =Object[nObj];
					
					if (nBelongPlayer > 0)
					{
						//设置物品属主和属主时间
						dropObject.SetItemBelongTime(ConfigManager::Singleton().GetGlobalVariable(global_var_object_item_belong_time));
					}
	
				}
			}		
		}
	}
	return true;
}

BOOL KNpc::LoseSingleItem(KItemDropRate *pDropRate, int nBelongPlayer, BOOL bAutoPicked, BOOL *pNoPlace, float fScale)
{
	if(pNoPlace != NULL)
	{
		*pNoPlace = FALSE;
	}

	if (!pDropRate)
	{
		return false;
	}

	for ( int i = 0; i < pDropRate->nCount; ++i)
	{
		UINT dRand = g_Random( pDropRate->ulMaxRandRate);

		if ( dRand < pDropRate->pItemParam[i].ulRate )
		{
			int nGenre,nDetail, nParticular, nLevel, nCount;
			nGenre		= pDropRate->pItemParam[i].nGenre;
			nDetail		= pDropRate->pItemParam[i].nDetailType;
			nParticular	= pDropRate->pItemParam[i].nParticulType;
			nLevel		= pDropRate->pItemParam[i].nLevel;
			nCount		= pDropRate->pItemParam[i].nItemCount;

			int nIdx = ItemSet.Add(
				nGenre, nDetail, nParticular, nLevel, nCount);

			//服务器的Item数组已经满了
			if (nIdx <= 0)
			{
				if(pNoPlace != NULL)
				{
					*pNoPlace = TRUE;
				}
				return false;
			}
						
			KItem& dropItem = Item[nIdx];
			if (dropItem.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
			{
				//掉落物品日志
				LogEventParam logEventParam;
				logEventParam.event = log_event_npc_drop_item;
				snprintf(logEventParam.param1.data, sizeof(logEventParam.param1.data), "%d", m_NpcSettingIdx);
				logEventParam.param2 = dropItem.GetGUID();
				dropItem.GetItemTemplateId(logEventParam.param3.data, sizeof(logEventParam.param3.data) - 1);
				g_pLogSystem->Log(logEventParam);
			}
			
			//NPC统计：掉落物品
			if (m_pTemplate)
			{
				ItemTemplateId templateId;
				dropItem.GetItemTemplateId(templateId);
				m_pTemplate->GetStatisticInfo().DropItem(templateId);
			}

			if(bAutoPicked)//直接将掉落物品给获得物品的玩家
			{
				EXTRAINFOPLUS tagExtraPlus;
				tagExtraPlus.nItemGenre = Item[nIdx].GetGenre();
				tagExtraPlus.nParticularType = Item[nIdx].GetParticular();
				tagExtraPlus.nDetailType = Item[nIdx].GetDetailType();
				tagExtraPlus.nMaxItem = Item[nIdx].GetMaxItemCount();
				tagExtraPlus.nCurItem = Item[nIdx].GetItemCount();
				tagExtraPlus.pCampareItem = &Item[nIdx];
				
				ItemPos	sItemPos;
				if(Player[nBelongPlayer].m_ItemList.SearchPosition(&sItemPos, &tagExtraPlus))
				{
					if (Player[nBelongPlayer].m_ItemList.Add(nIdx, sItemPos.nPlace, sItemPos.nX, sItemPos.nY, NULL, item_sync_type_pickup))
					{
						return true;
					}
				}
				else
				{
					//获得物品玩家item数组已经满了					
					if(pNoPlace != NULL)
					{
						*pNoPlace = TRUE;
					}		
				}
				ItemSet.Remove(nIdx);
				return false;	

			}
			else//物品掉在地上
			{
				int		nX, nY;
				POINT	ptLocal;
				KMapPos	Pos;

				GetMpsPos(&nX, &nY);
				ptLocal.x = nX;
				ptLocal.y = nY;			
				SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);

				Pos.nSubWorld = m_SubWorldIndex;
				SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, &Pos.nRegion, &Pos.nMapX, &Pos.nMapY, &Pos.nOffX, &Pos.nOffY);

				int nObj;
				KObjItemInfo sInfo;
				sInfo.m_nItemID = nIdx;
				sInfo.m_nMoneyNum = (Item[nIdx].GetMaxItemCount()==0)?0:Item[nIdx].GetItemCount();	
				if ( Item[nIdx].GetYaoID() == yao_yin )
				{
					sprintf( sInfo.m_szName, "[%s]%s", YIN, Item[nIdx].GetName() );
				}
				else if ( Item[nIdx].GetYaoID() == yao_yang )
				{
					sprintf( sInfo.m_szName, "[%s]%s", YANG, Item[nIdx].GetName() );
				}
				else
				{
					memcpy(sInfo.m_szName, Item[nIdx].GetName(), FILE_NAME_LENGTH );
				}				
				sInfo.m_szName[FILE_NAME_LENGTH - 1] = 0;
				sInfo.m_nColorID = Item[nIdx].GetQualityLabel();
				sInfo.m_nMovieFlag = 1;
				sInfo.m_nSoundFlag = 1;
				
				int nBelong = -1;
				if (nBelongPlayer > 0)
					nBelong = nBelongPlayer;

				nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo , nBelong);
				if (nObj == -1)
				{
					//服务器的Obj数组已经满了
					ItemSet.Remove(nIdx);
					if(pNoPlace != NULL)
					{
						*pNoPlace = TRUE;
					}
					return false;
				}
				else
				{
					//ItemDebugLog Begin........................
                    #ifdef _SERVER
					int nItemIndex = nIdx;
					if (nItemIndex > 0 && nItemIndex < MAX_ITEM && Item[nItemIndex].GetBelong() != -1)
					{
						int  nBelong         = Item[nItemIndex].GetBelong();
						char szDumpInfo[512] = "";
						snprintf(szDumpInfo,sizeof(szDumpInfo),"Npc Lose:Index %d,Belong:%d,Name:%s\n",nItemIndex,nBelong,Item[nItemIndex].GetName());
						szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
						DumpInvalidItemOpeStack(false,szDumpInfo,4);
					}//endif
                    #endif
                    //ItemDebugLog End.........................

					KObj& dropObject =Object[nObj];					
					if (nBelongPlayer > 0)
					{
						//设置物品属主和属主时间
						dropObject.SetItemBelongTime(ConfigManager::Singleton().GetGlobalVariable(global_var_object_item_belong_time));
					}
					return true;
				}
			}		
		}
	}
	return true;
}

void KNpc::Revive()
{	
	RestoreNpcBaseInfo();
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[m_SubWorldIndex].Mps2Map(m_OriginX, m_OriginY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if(nRegion < 0)
	{
		return;
	}
	MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);

	DoStand();
	m_ProcessAI = 1;
	m_ProcessState = 1;
	m_AiAddLifeTime = 0;

#ifdef _SERVER

	SetupEventBuff(NpcEvent_Revive);

	if(kind_player != m_Kind)
	{
		m_SkillList.CastAllPassiveSkill();
	}
#endif
}

#endif

#ifdef	_SERVER
// 向周围九屏广播
void	KNpc::SendDataToNearRegion(void* pBuffer, DWORD dwSize)
{
	//_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	int nMaxCount = MAX_BROADCAST_COUNT_OPTIMIZED;
	BROADCAST_REGION(pBuffer, dwSize, nMaxCount);
}
#endif

#ifdef	_SERVER
//-----------------------------------------------------------------------------
//	功能：死亡时候计算PK值
//-----------------------------------------------------------------------------
int KNpc::DeathCalcPKValue(int nKiller)
{
	// 出错
	if (nKiller <= 0 || nKiller >= MAX_NPC)
		return 0;

	if( nKiller == m_Index )
		return 0;

	if( IsInPKArea( ) )
		return 0;

	KNpc& killerNpc = Npc[nKiller];

	if (IsPlayer() && (killerNpc.IsPlayer() || killerNpc.IsCreature() || killerNpc.IsEmployee()))
	{
		if (!IsValidPlayer(killerNpc.m_nPlayerIdx))
			return 0;

		KPlayer& killerPlayer = Player[killerNpc.m_nPlayerIdx];
		int killerPlayerNpcIndex = killerPlayer.GetNpcIndex();

		if (!IsValidNpc(killerPlayerNpcIndex))
			return 0;

		//都到了PK鼓励等级
		const int pkEncourageLevel = ConfigManager::Singleton().GetGlobalVariable(global_var_pk_encourage_level);
		if (pkEncourageLevel > 0 && GetLevel() >= pkEncourageLevel && killerPlayer.GetLevel() >= pkEncourageLevel)
		{
			//非同氏族且非同国家，无PK惩罚
			int socialCondition = NpcSet.GetSUCond(m_Index, killerPlayerNpcIndex);
			if (!(socialCondition & ptc_cond_gens) && !(socialCondition & ptc_cond_tong))
				return 0;
		}
		
		//正在进行战争，无PK惩罚
		if (IsInEspecialArea(especial_area_war) && 
			Npc[killerPlayerNpcIndex].IsInEspecialArea(especial_area_war) 
			)
			return 0;

		//正在进行战争，无PK惩罚
		if (IsInEspecialArea(especial_area_city) && 
			Npc[killerPlayerNpcIndex].IsInEspecialArea(especial_area_city) 
			)
			return 0;

		return killerPlayer.RecalePKValue(GetPlayerIdx());//玩家杀人惩罚
	}

	return 0;
}
#endif

#ifdef	_SERVER
//-----------------------------------------------------------------------------
//	功能：查找周围9个Region中是否有指定的 player
//-----------------------------------------------------------------------------
BOOL	KNpc::CheckPlayerAround(int nPlayerIdx)
{
	if (nPlayerIdx <= 0 || m_RegionIndex < 0)
		return FALSE;
	if (SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].CheckPlayerIn(nPlayerIdx))
		return TRUE;
	int		nRegionNo;
	for (int i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		if (SubWorld[m_SubWorldIndex].m_Region[nRegionNo].CheckPlayerIn(nPlayerIdx))
			return TRUE;
	}
	return FALSE;
}

// Add by Cooler 2004-5-18
// Begin -->
void	KNpc::InitProduceState(int nProduceNpcIndex, int nProduceSpeed)
{
	if(m_tagProduceState.nProduceSpeed <= 0)
	{
		m_tagProduceState.nProduceNpcIndex = nProduceNpcIndex;
		m_tagProduceState.nProduceSpeed = nProduceSpeed;
//		if(m_nAddProduceSpeedV > 0)
//		{
//			m_tagProduceState.nProduceSpeed -= m_nAddProduceSpeedV;
//		}
//		if(m_nAddProduceSpeedP > 0)
//		{
//			m_tagProduceState.nProduceSpeed -= nProduceSpeed * m_nAddProduceSpeedP;
//		}
//		if(m_tagProduceState.nProduceSpeed < PRODUCESPEED_MIN)
//		{
//			m_tagProduceState.nProduceSpeed = PRODUCESPEED_MIN;
//		}
		m_tagProduceState.nProducePastTime = 1;
	}

	if(Player[m_nPlayerIdx].m_ItemList.GetWeaponAttrType() != weaponattr_producetool)
	{
		ClearProduceState();
	}
}
// End <--

void	KNpc::SyncNpcPos(int nSyncTarget, BOOL bIncludeSelf)
{
	if(!IsPlayer())
	{
		return;
	}

	FINDPATHSYNC tagSync;
	tagSync.ProtocolType = s2c_findpathsync;

	int nMpsXX, nMpsYY;
	
	if(bIncludeSelf)
	{
		GetMpsPos(&nMpsXX, &nMpsYY);

		tagSync.dwID = m_dwID;
		tagSync.nPosX = nMpsXX;
		tagSync.nPosY = nMpsYY;
		tagSync.byForce = TRUE;
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE *)&tagSync, sizeof(FINDPATHSYNC));
	}
	
	if(nSyncTarget > 0)
	{
		Npc[nSyncTarget].GetMpsPos(&nMpsXX, &nMpsYY);

		tagSync.dwID = Npc[nSyncTarget].m_dwID;
		tagSync.nPosX = nMpsXX;
		tagSync.nPosY = nMpsYY;
		tagSync.byForce = TRUE;
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE *)&tagSync, sizeof(FINDPATHSYNC));
	}
}

void KNpc::SyncNpcDir( int nDir )
{
	m_UnaryAttrMgr.Set(nuai_dir, nDir);
	SyncAttr(npc_attr_unary, nuai_dir, 0, true );
}

void KNpc::NoMove( int nOp )
{
	int nValue = m_UnaryAttrMgr[nuai_nomove];
	nValue += nOp;
	m_UnaryAttrMgr.Set( nuai_nomove, nValue );
	
	NPCNOMOVE	NpcMove;
	NpcMove.Protocol			=	s2c_byte_extend;
	NpcMove.ProtocolExtend		=	s2c_ex_protocol_nomove;
	NpcMove.wProtocolSize		=	sizeof( NPCNOMOVE ) - 1;
	NpcMove.dwID				=	m_dwID;
	NpcMove.nEnable				=	nValue;

	int nMaxCount = MAX_BROADCAST_COUNT_MIN;
	BROADCAST_REGION(&NpcMove, sizeof(NPCNOMOVE), nMaxCount);
}

#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定头顶状态
//-------------------------------------------------------------------------
void	KNpc::SetMenuState(int nState, char *lpszSentence, int nLength)
{
	this->m_DataRes.SetMenuState(nState, lpszSentence, nLength);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：获得头顶状态
//-------------------------------------------------------------------------
int		KNpc::GetMenuState()
{
	return this->m_DataRes.GetMenuState();
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：查找周围9个Region中是否有指定 ID 的 npc
//-------------------------------------------------------------------------
DWORD	KNpc::SearchAroundID(DWORD dwID)
{
	int		nIdx, nRegionNo;
	nIdx = SubWorld[0].m_Region[m_RegionIndex].SearchNpc(dwID);
	if (nIdx)
		return nIdx;
	for (int i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[0].m_Region[m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nIdx = SubWorld[0].m_Region[nRegionNo].SearchNpc(dwID);
		if (nIdx)
			return nIdx;
	}
	return 0;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定特殊的只播放一遍的随身spr文件
//-------------------------------------------------------------------------
void	KNpc::SetSpecialSpr(char *lpszSprName)
{
	m_DataRes.SetSpecialSpr(lpszSprName);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定瞬间特效
//-------------------------------------------------------------------------
void	KNpc::SetInstantSpr(unsigned int nNo )
{
	char	szName[FILE_NAME_LENGTH];
	szName[0] = 0;
	NpcSet.m_cInstantSpecial.GetSprName(nNo, szName, sizeof(szName));
	if (szName[0])
		this->SetSpecialSpr(szName);
}
#endif

#ifndef _SERVER
int		KNpc::GetNormalNpcStandDir(int nFrame)
{
	return m_DataRes.GetNormalNpcStandDir(nFrame);
}
#endif

#ifndef _SERVER
void 	KNpc::SetUiLoginNpcDoing( int nDoing )
{
	m_ClientDoing = (NPCCMD)nDoing;
	m_Doing = (NPCCMD)nDoing;
}
#endif

void	KNpc::RestoreNpcBaseInfo()
{
	
	m_CurrentCamp = m_Camp;
	
#ifdef _SERVER
	m_nCurPKPunishState = 0;

    // add by hejianfeng.  2005-10-24    
    m_nInitColorIdx = m_nNpcColor;
    //KPlayer[].m_btMorphHue = m_nNpcColor;
    // endadd
#endif

	m_nTargetType  = 0;
	m_nTargetIdx   = 0;
	m_nLastDamageIdx = 0;
	m_nLastPoisonDamageIdx = 0;

	m_UnaryAttrMgr.Set(nuai_curlife, m_CompAttrMgr[ncai_lifeuplimit][idx_base_value]);
	m_UnaryAttrMgr.Set(nuai_curmana, m_CompAttrMgr[ncai_manauplimit][idx_base_value]);

	{
		for(int i = 0; i < ncai_end; ++i)
		{
			m_CompAttrMgr.Set(i, idx_append_value, 0);
			m_CompAttrMgr.Set(i, idx_append_percent, 0);
		}
	}
	
	{
		for(int i = 0; i < nrai_end; ++i)
		{	
			m_RangeAttrMgr.Set(i, idx_value_low, idx_append_value, 0);
			m_RangeAttrMgr.Set(i, idx_value_low, idx_append_percent, 0);	
			m_RangeAttrMgr.Set(i, idx_value_hight, idx_append_value, 0);
			m_RangeAttrMgr.Set(i, idx_value_hight, idx_append_percent, 0);
		}
	}
}

#ifndef _SERVER
void KNpc::DrawBorder()
{
	return;
	if (m_Index <= 0)
		return;

	// lixuewu 2004.03.25 计算m_ResDir的值，以后有必要整理一下
	if (m_ResDir != m_UnaryAttrMgr[nuai_dir])
	{
		int nDirOff = m_UnaryAttrMgr[nuai_dir] - m_ResDir;
		if (nDirOff > 32)
			nDirOff -= 64;
		else if (nDirOff < - 32)
			nDirOff += 64;
		m_ResDir += nDirOff / 2;
		if (m_ResDir >= 64)
			m_ResDir -= 64;
		if (m_ResDir < 0)
			m_ResDir += 64;
	}
	// lixuewu 计算m_ResDir的值，以后有必要整理一下
	
	//g_pRepresent->SetOption(PERSPECTIVE, false);
	m_DataRes.DrawBorder(m_ResDir, m_Frames.nTotalFrame, m_Frames.nCurrentFrame);
	//g_pRepresent->SetOption(PERSPECTIVE, g_bPerspectiveMode);
}

int KNpc::DrawMenuState(int n)
{
	if (m_Index <= 0)
		return n;

	return m_DataRes.DrawMenuState(n);
}

void KNpc::DrawBlood()
{
	int nHeightOff = GetNpcPate();
	nHeightOff = PaintLife(nHeightOff, true);
	nHeightOff += SHOW_SPACE_HEIGHT;
}
#endif

int KNpc::SetPos(int nX, int nY)
{
#ifdef _SERVER
#ifdef _DEBUG
	// get ret address
	int stackVar;
    unsigned long stackVarAddr = (unsigned long)&stackVar;
    unsigned long argAddr = (unsigned long)&nX;
    void ** retAddrAddr = (void **)(stackVarAddr/2 + argAddr/2 + 2);	
    void * retAddr = * retAddrAddr;	
#endif
	if (m_SubWorldIndex < 0)
	{
		_ASSERT(0);
		return 0;
	}
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[m_SubWorldIndex].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	if (nRegion < 0)
	{
		g_DebugLog("[Script]SetPos error:SubWorld:%d, Pos(%d, %d)", SubWorld[m_SubWorldIndex].m_SubWorldID, nX, nY);
		return 0;
	}

	int nOldRegion = m_RegionIndex;
	if (m_RegionIndex >= 0)
	{
		//lixuewu 2006.05.23 取消NPC阻挡
		//SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY, m_Index);
		NPC_REMOVE_SYNC	RemoveSync;
		RemoveSync.ProtocolType = s2c_npcremove;
		RemoveSync.ID = m_dwID;
		SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));
	}

	MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);

	DoStand();
	m_ProcessAI = 1;
	m_ProcessState = 1;

	FINDPATHSYNC tagSync;
	tagSync.ProtocolType = s2c_findpathsync;
	tagSync.dwID = m_dwID;
	tagSync.nPosX = nX;
	tagSync.nPosY = nY;
	tagSync.byForce = TRUE;

	int nCount = MAX_BROADCAST_COUNT_MIN;
	BROADCAST_REGION( &tagSync, sizeof(FINDPATHSYNC), nCount );
	
#else

	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[m_SubWorldIndex].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);
	DoStand();

#endif

	return 1;
}

#ifdef _SERVER

void  KNpc::SetPosDirectly(int nX,int nY,int iDir)
{
    if (m_SubWorldIndex < 0)
	{
		_ASSERT(0);
		return ;
	}

	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[m_SubWorldIndex].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	
	if (nRegion < 0)
	{
		g_DebugLog("[Script]SetPos error:SubWorld:%d, Pos(%d, %d)", SubWorld[m_SubWorldIndex].m_SubWorldID, nX, nY);
		return ;
	}

	int nCheckBarrier = SubWorld[m_SubWorldIndex].TestBarrier(nX, nY);
	if (nCheckBarrier==Obstacle_NULL)
		MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);

	if (iDir!=-1)
		m_UnaryAttrMgr.Set(nuai_dir,iDir);
	
}

#endif

#ifdef _SERVER
int KNpc::ChangeWorld(DWORD dwSubWorldID, int nX, int nY, bool isInstance/* = false*/)
{
	// 全是Player，其它一律禁止通行，不然后边用m_nPlayerIdx的地方很危险
	if (!IsPlayer())
		return ChangeWorldNpc(dwSubWorldID, nX, nY, isInstance);

	int nTargetSubWorld = INVALID_WORLD_INDEX;
	if (isInstance)
	{
		nTargetSubWorld = g_SubWorldSet.GetInstance(dwSubWorldID);
	}
	else
	{
		// 如果试图进入副本模板，则禁止进入
		WorldSetting* pTargetWorldSetting = g_SubWorldSet.GetWorldSetting(dwSubWorldID);
		if (!pTargetWorldSetting || pTargetWorldSetting->IsInstance)
			return 0;

		nTargetSubWorld = g_SubWorldSet.SearchWorld(dwSubWorldID);
	}
	
	// 不在这台服务器上
	if (-1 == nTargetSubWorld)
	{
		g_DebugLog("[Map]World%d haven't been loaded!", dwSubWorldID);
		return 2;
	}

	// 检查是否可以进入目的地
	if (!SubWorld[nTargetSubWorld].CanEnter(m_nPlayerIdx))
		return 0;

	// 如果交易正在进行，取消交易
	Player[m_nPlayerIdx].tradeServerDoCanceTrade();

	// 切换的世界就是本身
	if (nTargetSubWorld == m_SubWorldIndex)
	{
		// 传送的第二种可能(不需要修改)
		// 只需切换座标
		return SetPos(nX, nY);
	}

	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[nTargetSubWorld].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	// 切换到的坐标非法
	if (nRegion < 0)
	{
		g_DebugLog("[Map]Change Pos(%d,%d) Invalid!", nX, nY);
		return 0;
	}

	if (GetSyncToWorldMode() > 0)
	{
		SyncToWorldDel();

		SubWorld[m_SubWorldIndex].RemoveFromSyncToWorldList(m_Index);
		SubWorld[nTargetSubWorld].AddToSyncToWorldList(m_Index);
	}
	
	// 传送的第三种可能
	// 真正开始切换工作
	if (m_SubWorldIndex >= 0 && m_RegionIndex >= 0)
	{
		//离开副本清空临时重生点
		if (SubWorld[m_SubWorldIndex].GetInstanceId() > INVALID_INSTANCE_ID)
		{
			Player[m_nPlayerIdx].ClearDeathRevivaPos();
		}

		Player[m_nPlayerIdx].OnEvent(player_event_exit_world, &m_SubWorldIndex);

		//lixuewu 2006.05.23 取消NPC阻挡
		//SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY, m_Index);
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		NPC_REMOVE_SYNC	RemoveSync;
		RemoveSync.ProtocolType = s2c_npcremove;
		RemoveSync.ID = m_dwID;
		SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));

		//////////////////////////////////////////////////////////////////////////
		// 移除召唤的NPC
		KCreature& aCreature = Player[m_nPlayerIdx].m_Creature;
		if (aCreature.IsALive())
		{
			KNpc* aCreatureNpc = aCreature.GetCreatureNpc();
			// IsAlive 必定可以Get到NPC
			const int SubWorldIdx = aCreatureNpc->m_SubWorldIndex;
			const int RegionIdx = aCreatureNpc->m_RegionIndex;
			const int NpcIdx = aCreatureNpc->m_Index;
			const int MapX = aCreatureNpc->GetMapX();
			const int MapY = aCreatureNpc->GetMapY();
//			//lixuewu 2006.05.23 取消NPC阻挡
//			//SubWorld[SubWorldIdx].m_Region[RegionIdx].DecNpcRef(MapX, MapY, NpcIdx);
			SubWorld[SubWorldIdx].m_Region[RegionIdx].RemoveNpc(NpcIdx);
			RemoveSync.ProtocolType = s2c_npcremove;
			RemoveSync.ID = aCreatureNpc->m_dwID;
			aCreatureNpc->SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));
		}
		//////////////////////////////////////////////////////////////////////////

		Employee& employee = Player[m_nPlayerIdx].GetEmployee();
		if (employee.IsExist())
		{
			int employeeNpcIndex = employee.GetNpcIndex();
			if (IsValidNpc(employeeNpcIndex))
			{
				KNpc& employeeNpc = Npc[employeeNpcIndex];
				const int SubWorldIdx = employeeNpc.m_SubWorldIndex;
				const int RegionIdx = employeeNpc.m_RegionIndex;
				const int MapX = employeeNpc.GetMapX();
				const int MapY = employeeNpc.GetMapY();
				SubWorld[SubWorldIdx].m_Region[RegionIdx].RemoveNpc(employeeNpcIndex);
				RemoveSync.ProtocolType = s2c_npcremove;
				RemoveSync.ID = employeeNpc.m_dwID;
				employeeNpc.SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));
			}
		}
	}

	int nSourceSubWorld = m_SubWorldIndex;
	int nSourceRegion = m_RegionIndex;

// 	MoveNpc(VOID_REGION, 0, 0, 0, 0);
// 	m_SubWorldIndex = nTargetSubWorld;
// 	MoveNpc(nRegion, nMapX, nMapY, nOffX, nOffY);

	m_SubWorldIndex = nTargetSubWorld;
	m_RegionIndex = nRegion;
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;
	SubWorld[nTargetSubWorld].m_Region[nRegion].AddNpc(m_Index);

	//////////////////////////////////////////////////////////////////////////
	// 把移出的召唤兽加入地图
	KCreature& aCreature = Player[m_nPlayerIdx].m_Creature;
	if (aCreature.IsALive())
	{
		KNpc* aCreatureNpc = aCreature.GetCreatureNpc();
		// IsAlive必定可以Get到Npc
// 		aCreatureNpc->MoveNpc(VOID_REGION, 0, 0, 0, 0);
// 		aCreatureNpc->m_SubWorldIndex = nTargetSubWorld;
// 		aCreatureNpc->MoveNpc(nRegion, nMapY, nMapY, nOffX, nOffY);

		aCreatureNpc->m_SubWorldIndex = nTargetSubWorld;
		aCreatureNpc->m_RegionIndex = nRegion;
		aCreatureNpc->m_MapX = nMapX;
		aCreatureNpc->m_MapY = nMapY;
		aCreatureNpc->m_OffX = nOffX;
		aCreatureNpc->m_OffY = nOffY;
		SubWorld[nTargetSubWorld].m_Region[nRegion].AddNpc(aCreatureNpc->m_Index);

		aCreature.Reset();
	}
	
	Employee& employee = Player[m_nPlayerIdx].GetEmployee();
	if (employee.IsExist())
	{
		int employeeNpcIndex = employee.GetNpcIndex();
		if (IsValidNpc(employeeNpcIndex))
		{
			KNpc& employeeNpc = Npc[employeeNpcIndex];
			
			employeeNpc.m_SubWorldIndex = nTargetSubWorld;
			employeeNpc.m_RegionIndex = nRegion;
			employeeNpc.m_MapX = nMapX;
			employeeNpc.m_MapY = nMapY;
			employeeNpc.m_OffX = nOffX;
			employeeNpc.m_OffY = nOffY;
			SubWorld[nTargetSubWorld].m_Region[nRegion].AddNpc(employeeNpcIndex);
		}
	}
	
	// lixuewu 2004.03.29
	//////////////////////////////////////////////////////////////////////////
	DoStand();
	m_ProcessAI = 1;
	m_ProcessState = 1;

	SubWorld[nTargetSubWorld].SendSyncData(m_Index, Player[m_nPlayerIdx].m_nNetConnectIdx);	

	SubWorld[nSourceSubWorld].RemovePlayer(nSourceRegion, m_nPlayerIdx);
	SubWorld[nTargetSubWorld].AddPlayer(nRegion, m_nPlayerIdx);

	NormalSync();
	SyncEquipTalismanNpcId();

	Player[m_nPlayerIdx].OnEvent(player_event_enter_world, &nTargetSubWorld);

	//===================================================
	BuffMgr& BM = BuffMgr::Singleton( );
	BUFF_ENV_PARAM Env;
	Env.nEventFormat	=	buff_event_format_event;
	Env.nEventSender	=	m_Index;
	Env.nEventRecever	=	m_Index;

	Env.nEventType		=	buff_event_type_chgmap;
	Env.nEventRelation	=	buff_event_relation_recver;
	BM.FilterEvent( Env );
	//===================================================
	
	return 1;
}
#endif

#ifdef _SERVER
int KNpc::ChangeWorldNpc(DWORD dwSubWorldID, int nX, int nY, bool isInstance)
{
	if (IsPlayer())
		return 0;

	int nTargetSubWorld = INVALID_WORLD_INDEX;
	if (isInstance)
	{
		nTargetSubWorld = g_SubWorldSet.GetInstance(dwSubWorldID);
	}
	else
	{
		// 如果试图进入副本模板，则禁止进入
		WorldSetting* pTargetWorldSetting = g_SubWorldSet.GetWorldSetting(dwSubWorldID);
		if (!pTargetWorldSetting || pTargetWorldSetting->IsInstance)
			return 0;

		nTargetSubWorld = g_SubWorldSet.SearchWorld(dwSubWorldID);
	}
	
	// 不在这台服务器上
	if (-1 == nTargetSubWorld)
	{
		g_DebugLog("[Map]World%d haven't been loaded!", dwSubWorldID);
		return 2;
	}

	// 切换的世界就是本身
	if (nTargetSubWorld == m_SubWorldIndex)
	{
		// 传送的第二种可能(不需要修改)
		// 只需切换座标
		return SetPos(nX, nY);
	}
	
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[nTargetSubWorld].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	// 切换到的坐标非法
	if (nRegion < 0)
	{
		g_DebugLog("[Map]Change Pos(%d,%d) Invalid!", nX, nY);
		return 0;
	}

	if (GetSyncToWorldMode() > 0)
	{
		SyncToWorldDel();

		SubWorld[m_SubWorldIndex].RemoveFromSyncToWorldList(m_Index);
		SubWorld[nTargetSubWorld].AddToSyncToWorldList(m_Index);
	}
	
	// 传送的第三种可能
	// 真正开始切换工作
	if (m_SubWorldIndex >= 0 && m_RegionIndex >= 0)
	{
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		NPC_REMOVE_SYNC	RemoveSync;
		RemoveSync.ProtocolType = s2c_npcremove;
		RemoveSync.ID = m_dwID;
		SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));
	}

	int nSourceSubWorld = m_SubWorldIndex;
	int nSourceRegion = m_RegionIndex;

	m_SubWorldIndex = nTargetSubWorld;
	m_RegionIndex = nRegion;
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;
	SubWorld[nTargetSubWorld].m_Region[nRegion].AddNpc(m_Index);

	DoStand();
	m_ProcessAI = 1;
	m_ProcessState = 1;

	NormalSync();

	//===================================================
	BuffMgr& BM = BuffMgr::Singleton( );
	BUFF_ENV_PARAM Env;
	Env.nEventFormat	=	buff_event_format_event;
	Env.nEventSender	=	m_Index;
	Env.nEventRecever	=	m_Index;

	Env.nEventType		=	buff_event_type_chgmap;
	Env.nEventRelation	=	buff_event_relation_recver;
	BM.FilterEvent( Env );
	//===================================================
	
	return 1;
}
#endif

void KNpc::CheckTrap()
{
	//--> Rocker 2004/09/17
	if (m_Kind != kind_player)
	//if ((m_Kind != kind_player)&&(m_Kind != kind_siege_weapon))
	//<-- End
		return;
	
	if (m_Index <= 0)
		return;

	if (m_SubWorldIndex < 0 || m_RegionIndex < 0)
		return;

	DWORD	dwTrap = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrap(GetMapX(), GetMapY());
	if (m_TrapScriptID == dwTrap)
	{
		return;
	}
	else
	{
		m_TrapScriptID = dwTrap;
	}

	if (!m_TrapScriptID)
	{
		return;
	}

	Player[m_nPlayerIdx].ExecuteScript(m_TrapScriptID, "main", 0);
}

void KNpc::TurnTo(int nIdx)
{
	if (!Npc[nIdx].m_Index || !m_Index || m_Kind == kind_building)
		return;

	int nX1, nY1, nX2, nY2;

	GetMpsPos(&nX1, &nY1);
	Npc[nIdx].GetMpsPos(&nX2, &nY2);

	m_UnaryAttrMgr.Set(nuai_dir, g_GetDirIndex(nX1, nY1, nX2, nY2));
}

#ifndef _SERVER
void	KNpc::HurtAutoMove()
{
// 	if (this->m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex)
// 		return;
// 	if (this->m_Doing != do_hurt)
// 		return;
// 	if (m_sSyncPos.m_nDoing != do_hurt && m_sSyncPos.m_nDoing != do_stand)
// 		return;
// 
// 	int	nFrames, nRegionIdx;
// 
// 	nFrames = m_Frames.nTotalFrame - m_Frames.nCurrentFrame;
// 	if (nFrames <= 1)
// 	{
// 		if ((DWORD)SubWorld[0].m_Region[m_RegionIndex].m_RegionID == m_sSyncPos.m_dwRegionID)
// 		{
// 			//lixuewu 2006.05.23 取消NPC阻挡
// 			//SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY, m_Index);
// 			m_MapX = m_sSyncPos.m_nMapX;
// 			m_MapY = m_sSyncPos.m_nMapY;
// 			m_OffX = m_sSyncPos.m_nOffX;
// 			m_OffY = m_sSyncPos.m_nOffY;
// 			memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
// 			//lixuewu 2006.05.23 取消NPC阻挡
// 			//SubWorld[0].m_Region[m_RegionIndex].AddNpcRef(m_MapX, m_MapY, m_Index);
// 		}
// 		else
// 		{
// 			nRegionIdx = SubWorld[0].FindRegion(m_sSyncPos.m_dwRegionID);
// 			if (nRegionIdx < 0)
// 				return;
// 			//lixuewu 2006.05.23 取消NPC阻挡
// 			//SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY, m_Index);
// 			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegionIdx].m_RegionID, m_Index);
// 			m_RegionIndex = nRegionIdx;
// 			m_dwRegionID = m_sSyncPos.m_dwRegionID;
// 			m_MapX = m_sSyncPos.m_nMapX;
// 			m_MapY = m_sSyncPos.m_nMapY;
// 			m_OffX = m_sSyncPos.m_nOffX;
// 			m_OffY = m_sSyncPos.m_nOffY;
// 			memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
// 		}
// 	}
// 	else
// 	{
// 		nRegionIdx = SubWorld[0].FindRegion(m_sSyncPos.m_dwRegionID);
// 		if (nRegionIdx < 0)
// 			return;
// 		int		nNpcX, nNpcY, nSyncX, nSyncY;
// 		int		nNewX, nNewY, nMapX, nMapY, nOffX, nOffY;
// 		SubWorld[0].Map2Mps(m_RegionIndex, 
// 			m_MapX, m_MapY,
// 			m_OffX, m_OffY,
// 			&nNpcX, &nNpcY);
// 		SubWorld[0].Map2Mps(nRegionIdx, 
// 			m_sSyncPos.m_nMapX, m_sSyncPos.m_nMapY,
// 			m_sSyncPos.m_nOffX, m_sSyncPos.m_nOffY,
// 			&nSyncX, &nSyncY);
// 		nNewX = nNpcX + (nSyncX - nNpcX) / nFrames;
// 		nNewY = nNpcY + (nSyncY - nNpcY) / nFrames;
// 		SubWorld[0].Mps2Map(nNewX, nNewY, &nRegionIdx, &nMapX, &nMapY, &nOffX, &nOffY);
// 		_ASSERT(nRegionIdx >= 0);
// 		if (nRegionIdx < 0)
// 			return;
// 		if (nRegionIdx != m_RegionIndex)
// 		{
// 			//lixuewu 2006.05.23 取消NPC阻挡
// 			//SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY, m_Index);
// 			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegionIdx].m_RegionID, m_Index);
// 			m_RegionIndex = nRegionIdx;
// 			m_dwRegionID = m_sSyncPos.m_dwRegionID;
// 			m_MapX = nMapX;
// 			m_MapY = nMapY;
// 			m_OffX = nOffX;
// 			m_OffY = nOffY;
// 		}
// 		else
// 		{
// 			//lixuewu 2006.05.23 取消NPC阻挡
// 			//SubWorld[0].m_Region[m_RegionIndex].DecNpcRef(m_MapX, m_MapY, m_Index);
// 			m_MapX = nMapX;
// 			m_MapY = nMapY;
// 			m_OffX = nOffX;
// 			m_OffY = nOffY;
// 			//lixuewu 2006.05.23 取消NPC阻挡
// 			//SubWorld[0].m_Region[m_RegionIndex].AddNpcRef(m_MapX, m_MapY, m_Index);
// 		}
// 	}
}

#endif

#ifndef _SERVER
void KNpc::ProcNetCommand(NPCCMD cmd, int x /* = 0 */, int y /* = 0 */, int z /* = 0 */)
{

	switch (cmd)
	{
	case do_death:
		DoDeath();
		break;
	case do_hurt:
		DoHurt(x, y, z);
		break;
	case do_revive:
		DoStand();
		m_ProcessAI = 1;
		m_ProcessState = 1;
		//SetInstantSpr(enumINSTANT_STATE_REVIVE);
		break;
	case do_stand:
		DoStand();
		m_ProcessAI = 1;
		m_ProcessState = 1;
	default:
		break;
	}
}
#endif


#ifndef _SERVER
int	KNpc::GetNpcPate()
{
	int nHeight = m_Height + m_nStature;
	if ((m_Kind == kind_player || m_Kind == kind_employee) && m_nMorphType < 0)
	{
		switch(m_Series)
		{
		case series_metal:	// 甲士
			{
				nHeight = m_nSex ? 130 : 130;
			
				if (m_Doing == do_sit && MulDiv(10, m_Frames.nCurrentFrame, m_Frames.nTotalFrame) >= 8)
					nHeight -= MulDiv(50, m_Frames.nCurrentFrame, m_Frames.nTotalFrame);

				nHeight+= m_bRideHorse ? 56 : 0;
			}
			break;
		case series_wood:	// 道士
			{
				nHeight = m_nSex ? 130 : 130;
			
				if (m_Doing == do_sit && MulDiv(10, m_Frames.nCurrentFrame, m_Frames.nTotalFrame) >= 8)
					nHeight -= MulDiv(50, m_Frames.nCurrentFrame, m_Frames.nTotalFrame);

				nHeight+= m_bRideHorse ? 25 : 0;
			}
			break;
		case series_water:	// 异人
			{
				nHeight = m_nSex ? 130 : 130;
			
				if (m_Doing == do_sit)
					nHeight += 25;

				if (m_Doing == do_sit && MulDiv(10, m_Frames.nCurrentFrame, m_Frames.nTotalFrame) >= 8)
					nHeight -= MulDiv(50, m_Frames.nCurrentFrame, m_Frames.nTotalFrame);
				
				nHeight+= m_bRideHorse ? 30 : 0;
			}
			break;
		}
	}

	return nHeight;
}
#endif



void KNpc::SetTarget(unsigned int nType,int nIdx)
{
	if ( nIdx != 0 && m_nTargetIdx != 0 && nIdx == m_nTargetIdx )
	{
		m_bTheSameTarget = true;
	}
	else
	{
		m_bTheSameTarget = false;
	}
#ifndef _SERVER
	if ( !m_bTheSameTarget && IsPlayer() )
	{
//		Npc[nIdx].SetHeadInfoChanged( true );
	}
	KCacheNode* pSoundNode = NULL;
	pSoundNode = g_SoundCache.GetNode(SELECT_NPC_SOUND, (KCacheNode*)pSoundNode);
	if ( pSoundNode )
	{
		KWavSound* pWave = (KWavSound*)pSoundNode->m_lpData;
		if (pWave)
		{
			if (pWave->IsPlaying())
			{
				int nVolume = Option.GetSndVolume();
				pWave->SetVolume( nVolume );
			}
			else
			{
				int nVolume = Option.GetSndVolume();
				pWave->Play(0, nVolume, false);
			}
		}
	}
#endif
	_ASSERT(nType <= targettype_count);
	m_nTargetType = nType;
	m_nTargetIdx = nIdx;
}

#ifndef _SERVER

int	KNpc::GetNpcPatePeopleInfo()
{
	int nFontSize = 12;
	if (m_nChatContentLen > 0 && m_nChatNumLine > 0)
		return m_nChatNumLine * (nFontSize + 1);

	int nHeight = 0;
	if (NpcSet.CheckShowLife())
	{
		if (m_Kind == kind_player ||
			m_Kind == kind_partner)
		{
			if (m_CompAttrMgr[ncai_lifeuplimit] > 0 &&
				(relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex))
				)
				nHeight += SHOW_LIFE_HEIGHT;
		}
	}
	if (NpcSet.CheckShowName())
	{
		if (nHeight != 0)
			nHeight += SHOW_SPACE_HEIGHT;//好看

		if (m_Kind == kind_player || m_Kind == kind_dialoger || m_Kind == kind_siege_weapon)
		{
			nHeight += nFontSize + 1;
		}
	}
	return nHeight;
}
#endif

//////////////////////////////////////////////////////////////////////////
// lixuewu 2004.7.8 变身
// 变身函数 
// nNpcType 表示变成的样子类型,-1表示取消变身
// bSafeGuard 表示是否受系统保护，被保护不会受到伤害同时也不能发出攻击
// uMoveSpeed 表示移动速度
// uTime	表示持续时间
int KNpc::PolyMorph(int nNpcType, BOOL bCanCast, unsigned int nSafeGuardLevel, int uMoveSpeed, unsigned int uTime)
{
#ifdef _SERVER	

	unsigned int uPMTimeEN = 0;

	uTime += uPMTimeEN;
	//----------------->
#endif
	if (nNpcType < 0) // 取消变身
	{
		m_nSafeGuardLevel = 0;
		m_bCanCast = TRUE;

		// 速度暂时不处理, 通过其他方式操作
//		m_CompAttrMgr.Set(ncai_runspeed, idx_append_value, uMoveSpeed);
		m_uPolyMorphTime = 0;

				
#ifdef _SERVER
		int nMorphType = m_nMorphType;
#endif
		
#ifndef _SERVER
		if (m_nMorphType != nNpcType)
#endif
		{
			//Lucifer~yu[zhangjianyu] [01/12/2006] Add for Del war poly state buffer
			//begin------------------------------------------------------------------------
#ifndef _SERVER
			if ( Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index )
			{
				int nBufKind = -1;
				switch( m_nMorphType )
				{
				case 301 - 2:
					nBufKind = 12;
					break;
				case 302 - 2:
					nBufKind = 15;
					break;
				case 303 - 2:
					nBufKind = 13;
					break;
				case 304 - 2:
					nBufKind = 14;
					break;
				case 305 - 2:
					nBufKind = 2;
					break;
				case 306 - 2:
					nBufKind = 3;
					break;
				case 307 - 2:
					nBufKind = 4;
					break;
				case 308 - 2:
					nBufKind = 5;
					break;
				case 309 - 2:
					nBufKind = 6;
					break;
				case 310 - 2:
					nBufKind = 7;
					break;
				case 311 - 2:
					nBufKind = 8;
					break;
				case 312 - 2:
					nBufKind = 9;
					break;
				case 313 - 2:
					nBufKind = 10;
					break;
				case 314 - 2:
					nBufKind = 11;
					break;
				default:
					nBufKind = -1;
					break;
				}
//				CoreDataChanged( GDCNI_DEL_ROLE_BUF, nBufKind, NULL );
			}
#endif
			//end--------------------------------------------------------------------------	
			m_nMorphType = nNpcType;
			memset(m_nMorphPart, -1, sizeof(m_nMorphPart)); // -1 可以用memset 因为是0xffffffff
#ifndef _SERVER
			char szNpcTypeName[FILE_NAME_LENGTH];
			if (m_NpcSettingIdx < 0)
			{
				_ASSERT(m_NpcSettingIdx >= -6);
				strcpy(szNpcTypeName,NPCTEMPLATEID_TO_ROLENAME[-(m_NpcSettingIdx+1)]);
			}
			else
			{
				g_NpcSetting.GetString(m_NpcSettingIdx + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));
				if (!szNpcTypeName[0])
				{
					//如果没找到，用第一个npc代替
					g_NpcKindFile.GetString(2, ROLE_KIND, "", szNpcTypeName, sizeof(szNpcTypeName));
				}
			}
			
			const unsigned int uSceneID = m_DataRes.m_SceneID;
			//-------> Ray [Luoliang] 2005-7-19
			//要把变身前的摆摊信息保存
			int nMenuState = m_DataRes.GetMenuState();
			char szBuf[MAX_SENTENCE_LENGTH];
			int nSentenceLen = m_DataRes.GetMenuSentence(szBuf);
			//<------- End [Ray]
			m_DataRes.Init(szNpcTypeName, &g_NpcResList, m_NpcSettingIdx, false );
			m_DataRes.m_SceneID = uSceneID;
			m_DataRes.SetPart(BODY_PART_HELM, m_HelmType, m_HelmPal);
			m_DataRes.SetPart(BODY_PART_ARMOR, m_ArmorType, m_ArmorPal);
			m_DataRes.SetPart(BODY_PART_WEAPON, m_WeaponType, m_WeaponPal);
			m_DataRes.SetPart(BODY_PART_SHOULDER, m_ShoulderType, m_ShoulderPal);
			m_DataRes.SetPart(BODY_PART_CUFF, m_CuffType, m_CuffPal);
			m_DataRes.SetPart(BODY_PART_BOOT, m_BootType, m_BootPal);
			m_DataRes.SetPart(BODY_PART_HORSE, m_HorseType, m_HorsePal);
			m_DataRes.SetRideHorse(m_bRideHorse);
			m_DataRes.SetAction(m_ClientDoing);
			//-------> Ray [Luoliang] 2005-7-19
			//把摆摊的信息恢复
			m_DataRes.SetMenuState(nMenuState, szBuf, nSentenceLen);
			//<------- End [Ray]
#endif
		}
	}
	else // 我变我变我变变变
	{
		m_nSafeGuardLevel = nSafeGuardLevel;
		m_bCanCast = bCanCast;
		m_uMoveSpeed = uMoveSpeed;
// --> Rocker Edit Start 2006/01/06
#ifdef _SERVER
		m_uPolyMorphTime = uTime;
#endif
// <-- Rocker End

#ifndef _SERVER
		if (m_nMorphType != nNpcType)
#endif
		{
			m_nMorphType = nNpcType;
		
			if (PolyMorphSettings.GetCount() > 0 && PolyMorphSettings.GetCount() > m_nMorphType)
			{
				const PolyMorphSetting::PolyMorphType& aPolyMorphInfo = PolyMorphSettings[m_nMorphType];
				if (aPolyMorphInfo.nType >=0 )
				{
#ifndef _SERVER
					char szNpcTypeName[FILE_NAME_LENGTH];
					g_NpcSetting.GetString(nNpcType + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));
					if (!szNpcTypeName[0])
					{
						//如果没找到，用第一个npc代替
						g_NpcKindFile.GetString(2, ROLE_KIND, "", szNpcTypeName, sizeof(szNpcTypeName));
					}
					g_NpcSetting.GetInteger(nNpcType + 2, "Stature", 130, &m_nStature);
					const unsigned int uSceneID = m_DataRes.m_SceneID;
					m_DataRes.Init(szNpcTypeName, &g_NpcResList, nNpcType,false );
					m_DataRes.m_SceneID = uSceneID;			
					m_DataRes.SetPart(BODY_PART_HELM, 0, Default_PalIndex);
					m_DataRes.SetPart(BODY_PART_ARMOR, 0, Default_PalIndex);
					m_DataRes.SetPart(BODY_PART_WEAPON, 0, Default_PalIndex);
					m_DataRes.SetPart(BODY_PART_SHOULDER, 0, Default_PalIndex);
					m_DataRes.SetPart(BODY_PART_CUFF, 0, Default_PalIndex);
					m_DataRes.SetPart(BODY_PART_BOOT, 0, Default_PalIndex);
					m_DataRes.SetPart(BODY_PART_HORSE, 0, Default_PalIndex);
					m_DataRes.SetRideHorse(FALSE);
					m_DataRes.SetAction(m_ClientDoing);
#endif
					memset(m_nMorphPart, -1, sizeof(m_nMorphPart)); // -1 可以用memset 因为是0xffffffff
				}
				else
				{
					memcpy(m_nMorphPart, aPolyMorphInfo.nPartNo, sizeof(m_nMorphPart));
				}
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
PolyMorphSetting PolyMorphSetting::s_Self;

// 变身类型设定文件
#define POLYMORPH_SETTING_FILE "\\settings\\polymorph.txt"
// 设定文件的表头格式
enum
{
	POLYMORPH_TYPE = 1,
	POLYMORPH_HELM,
	POLYMORPH_SHOULDER,
	POLYMORPH_ARMOR,
	POLYMORPH_CUFF,
	POLYMORPH_WEAPON,
	POLYMORPH_BOOTS,
	POLYMORPH_HORSE,
	POLYMORPH_TABLE_COL_COUNT	
};

//************************************************************************
// 初始化模板设定
//************************************************************************
BOOL PolyMorphSetting::Init(void)
{
    if (NULL == m_pSetting) 
    {
        KTabFile aTabFile;
		g_SetRootPath(NULL);
        if (aTabFile.Load(POLYMORPH_SETTING_FILE))
        {
            const unsigned int nCount = aTabFile.GetHeight() - 1;
            _ASSERT(nCount >= 1);
			_ASSERT(POLYMORPH_TABLE_COL_COUNT == aTabFile.GetWidth()+1);
            m_pSetting = new PolyMorphType[nCount];
            if (NULL != m_pSetting) 
            {
                for(unsigned int nIndex = 0; nIndex < nCount; nIndex++)
                {
                    const unsigned int nRow = nIndex + 2;
					PolyMorphType& aSetting = m_pSetting[nIndex];
					memset(aSetting.nPartNo, -1, sizeof(aSetting.nPartNo));
					aTabFile.GetInteger(nRow, POLYMORPH_TYPE, 0,(int*)&(aSetting.nType));
					aTabFile.GetInteger(nRow, POLYMORPH_HELM, 0,(int*)&(aSetting.nPartNo[itempart_helm]));
					aTabFile.GetInteger(nRow, POLYMORPH_ARMOR, 0,(int*)&(aSetting.nPartNo[itempart_armor]));
					aTabFile.GetInteger(nRow, POLYMORPH_SHOULDER, 0,(int*)&(aSetting.nPartNo[itempart_shoulder]));
					aTabFile.GetInteger(nRow, POLYMORPH_CUFF, 0,(int*)&(aSetting.nPartNo[itempart_cuff]));
					aTabFile.GetInteger(nRow, POLYMORPH_BOOTS, 0,(int*)&(aSetting.nPartNo[itempart_boots]));
					aTabFile.GetInteger(nRow, POLYMORPH_WEAPON, 0,(int*)&(aSetting.nPartNo[itempart_weapon]));
					aTabFile.GetInteger(nRow, POLYMORPH_HORSE, 0,(int*)&(aSetting.nPartNo[itempart_horse]));
                }
                m_uCount = nCount;
                return TRUE;
            }
        }
    }
    return FALSE;
}

//************************************************************************
// 释放资源
//************************************************************************
BOOL PolyMorphSetting::Release(void)
{
    if (NULL != m_pSetting) 
    {
        delete[] m_pSetting;
		m_pSetting = NULL;
		m_uCount = 0;
		return TRUE;
    }
	return FALSE;
}

//--> Rocker 2005/07/14
#ifndef _SERVER
void KNpc::ProcessAddon(RequestNpcAddon* pAddon)
{
	if (!pAddon)
		return;
	int nWordsSize = strlen(pAddon->szSayMessage);
	if (nWordsSize == 0)
		return;

	char strContent[1024];
	memset(strContent, 0, sizeof(strContent));
	if (pAddon->btSayType == 0)
	{
		g_StrCpyLen(strContent, pAddon->szSayMessage, nWordsSize);
	}
	else if (pAddon->btSayType == 1)
	{
		int nNum = atoi(pAddon->szSayMessage);
		g_GetStringRes(nNum, strContent ,sizeof(strContent));
	}
				
	SetChatInfo(Name, strContent, sizeof(strContent));
	KSystemMessage	sMsg;
	sMsg.eType = SMT_TIPMSG;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	g_StrCpyLen(sMsg.szMessage, strContent, sizeof(sMsg.szMessage));
//	CoreDataChanged(GDCNI_NEARBY_MESSAGE, (unsigned int)&sMsg, (int)Name);
}

void	KNpc::AddBuffToC(unsigned long ulBuffID, int nTempID) 
{
	m_BuffList_C.push_back( BUFFPAIR( ulBuffID, nTempID ) ); 
	BuffTable& BT = BuffTable::Singleton( );
	PBAT pBAT = BT.GetBuff( nTempID );
	if ( pBAT )
	{
		if ( pBAT->nSoundID > 0 && m_pBuffWave )
		{
			m_pBuffWave->Stop();
		}
	}

	ConfigManager &   mgr = ConfigManager::Singleton();
	int nSpecialEventBuff = mgr.GetGlobalVariable(global_var_special_buff);
	if (nTempID == nSpecialEventBuff)
	{
		const char* pTbuff = mgr.GetConfigurableDisplayStyle( style_role_head_image_info, 2 );
		if (pTbuff)
			AddLayoutToHeadInfo(pTbuff,HEAD_INFO_SPECIAL_BUFF,HIP_Special);
	}//endif

}

void KNpc::RemoveBuffFromC(unsigned long ulBuffID)
{
	C_BUFFLIST::iterator it = m_BuffList_C.begin( );
	
	while( it != m_BuffList_C.end() )
	{
		BUFFPAIR& BP = *it;
		
		if( BP.first == ulBuffID )
		{
			BuffTable& BT = BuffTable::Singleton( );
			PBAT pBAT = BT.GetBuff( (*it).second );
			if ( pBAT )
			{
				if ( pBAT->nHSpecID > 0 || pBAT->nBSpecID > 0 || pBAT->nFSpecID > 0 )
				{
					m_DataRes.ClearState( ulBuffID );
				}
				if ( pBAT->nDispID && m_nEffectPolyMorphBuffIdx == ulBuffID )
				{
					PolyMorph( -1, 0, 0, 0, 0 );
				}
				if ( pBAT->nSoundID > 0 && m_pBuffWave )
				{
					m_pBuffWave->Stop();
				}

				ConfigManager &   mgr = ConfigManager::Singleton();
				int nSpecialEventBuff = mgr.GetGlobalVariable(global_var_special_buff);
				if ((*it).second == nSpecialEventBuff)
				{
					DelHeadInfo(HEAD_INFO_SPECIAL_BUFF);
				}//endif

			}

			m_BuffList_C.erase( it++ );
		}
		else
			++it;
	}
}

#endif

#ifdef _SERVER
void KNpc::GetNpcDamage(int *pOutRst)
{
	// 保证如果伤害大于0，则至少有1点伤害
	int	nOriPhysDam = CalcPhysicsDamage();
	if(nOriPhysDam > 0 && nOriPhysDam < 1024)
		nOriPhysDam = 1024;

	int nOriMagicDam = CalcMagicDamage();
	if(nOriMagicDam > 0 && nOriMagicDam < 1024)
		nOriMagicDam = 1024;

	int nDamPhys = nOriPhysDam >> 10;
	int nDamMagic = nOriMagicDam >> 10;

	pOutRst[dot_farphysics]	  = m_RangeAttrMgr[nrai_damage_farphysics] + nDamPhys;
	pOutRst[dot_nearphysics]  = m_RangeAttrMgr[nrai_damage_nearphysics] + nDamPhys;
	pOutRst[dot_water]		  = m_RangeAttrMgr[nrai_damage_water] + nDamMagic;
	pOutRst[dot_fire]		  = m_RangeAttrMgr[nrai_damage_fire] + nDamMagic;
	pOutRst[dot_thunder]	  = m_RangeAttrMgr[nrai_damage_thunder] + nDamMagic;
	pOutRst[dot_wind]		  = m_RangeAttrMgr[nrai_damage_wind] + nDamMagic;
	pOutRst[dot_shadow]		  = m_RangeAttrMgr[nrai_damage_shadow] + nDamMagic;
	pOutRst[dot_poison]		  = m_RangeAttrMgr[nrai_damage_poison] + nDamMagic;	

	pOutRst[dot_almighty1]	  = m_RangeAttrMgr[nrai_damage_almighty1] + nDamMagic;
	pOutRst[dot_almighty2]	  = m_RangeAttrMgr[nrai_damage_almighty2] + nDamMagic;
	pOutRst[dot_almighty3]	  = m_RangeAttrMgr[nrai_damage_almighty3] + nDamMagic;
	pOutRst[dot_almighty4]	  = m_RangeAttrMgr[nrai_damage_almighty4];
	pOutRst[dot_almighty5]	  = m_RangeAttrMgr[nrai_damage_almighty5];
	pOutRst[dot_almighty6]	  = m_RangeAttrMgr[nrai_damage_almighty6];
	pOutRst[dot_almighty7]	  = m_RangeAttrMgr[nrai_damage_almighty7];
	pOutRst[dot_almighty8]	  = m_RangeAttrMgr[nrai_damage_almighty8];
}
#endif

#ifdef _SERVER
void KNpc::GetNpcDefend(int *pOutRst)
{
	int nPhysDefend = CalcPhysicsDefense();
	int nEightDiagDefend = CalcEightDiagDfns();
	int nDarkDefend = CalcDarkDfns();

	pOutRst[dot_farphysics]    = m_RangeAttrMgr[nrai_defend_farphysics] + nPhysDefend;
	pOutRst[dot_nearphysics]   = m_RangeAttrMgr[nrai_defend_nearphysics] + nPhysDefend;
	pOutRst[dot_water]         = m_RangeAttrMgr[nrai_defend_water] + nEightDiagDefend;
	pOutRst[dot_fire]          = m_RangeAttrMgr[nrai_defend_fire] + nEightDiagDefend;
	pOutRst[dot_thunder]       = m_RangeAttrMgr[nrai_defend_thunder] + nEightDiagDefend;
	pOutRst[dot_wind]		   = m_RangeAttrMgr[nrai_defend_wind] + nEightDiagDefend;
	pOutRst[dot_shadow]		   = m_RangeAttrMgr[nrai_defend_shadow] + nDarkDefend;
	pOutRst[dot_poison]		   = m_RangeAttrMgr[nrai_defend_poison] + nDarkDefend;

	pOutRst[dot_almighty1]	   = m_RangeAttrMgr[nrai_defend_almighty1];
	pOutRst[dot_almighty2]	   = m_RangeAttrMgr[nrai_defend_almighty2];
	pOutRst[dot_almighty3]	   = m_RangeAttrMgr[nrai_defend_almighty3];
	pOutRst[dot_almighty4]	   = m_RangeAttrMgr[nrai_defend_almighty4];
	pOutRst[dot_almighty5]	   = m_RangeAttrMgr[nrai_defend_almighty5];
	pOutRst[dot_almighty6]	   = m_RangeAttrMgr[nrai_defend_almighty6];
	pOutRst[dot_almighty7]	   = m_RangeAttrMgr[nrai_defend_almighty7];
	pOutRst[dot_almighty8]	   = m_RangeAttrMgr[nrai_defend_almighty8];
}
#endif

#ifdef _SERVER

void KNpc::SyncCommoFlagInfo()
{
	int nCount = MAX_BROADCAST_COUNT_MIN;

	NPC_COMMO_FLAG info;
	info.ProtocolType  = s2c_npc_comoflag;
	info.nID           = m_dwID;
	int nCamouFlage    = 0;
	
	if  (m_Kind == kind_player)
	{
		nCamouFlage    =  m_UnaryAttrMgr[nuai_camou_flage];
	}//endif
	
	if  (m_Kind == kind_creature)
	{
		int nSummorIdx =  GetSummonerIdx();
		
		if (IsValidNpc(nSummorIdx))
		{
			nCamouFlage = Npc[nSummorIdx].m_UnaryAttrMgr[nuai_camou_flage];
		}//endif
		
	}//endif
	
	info.Comoflag     = nCamouFlage;

	if( nCamouFlage > 0)
	{
		const char * szPrivateStateName = ConfigManager::Singleton().GetPlayerPrivateStateName( nCamouFlage );
		_ASSERT(szPrivateStateName);
		strncpy(info.szName,szPrivateStateName,sizeof(info.szName));	
	}//endif
	else
	{
		strncpy(info.szName, Name, sizeof(info.szName));
	}//end else
	
	BROADCAST_REGION(&info,sizeof(info),nCount);
}

void KNpc::SyncAttr(int nAttrType, int nAttrIdx, BYTE valMask, bool bBroadCast /* = false */)
{
	char	buf[256];

	PSYNC_NPCATTR	pSync = (PSYNC_NPCATTR)buf;
	pSync->prototol       = s2c_sync_npcattr;
	pSync->subProtocol    = (BYTE)nAttrType;
	pSync->npcId		  = m_dwID;
	pSync->attrIdx		  = (BYTE)nAttrIdx;
	pSync->valueMask	  = valMask;

	int	 nValCnt = 0;
	int	 *pVal = (int*)pSync->data;

	switch(nAttrType)
	{
	case npc_attr_unary:
		*pVal = m_UnaryAttrMgr[nAttrIdx];
		nValCnt = 1;
		break;

	case npc_attr_comp:
		// valMask 的低四位标记 对应的4个值哪些需要同步, 1同步，0 不同步
		{
			for(int i = 0; i <= idx_current_value; ++i)
			{
				if( valMask & (1 << i) )
				{
					*pVal++ = m_CompAttrMgr[nAttrIdx][i];
					nValCnt++;
				}
			}
		}
		break;

	case npc_attr_range:
		// valMask 的高4位标记RangeAttr的上限部分4个值哪些需要同步，1同步，0不同步
		// 低4位标记下限部分4个值哪些需要同步，1同步，0不同步
		{
			for(int i = 0; i <= idx_current_value; ++i)
			{
				if( valMask & (1 << i) )
				{
					*pVal++ = m_RangeAttrMgr[nAttrIdx][idx_value_low][i];
					nValCnt++;
				}

				if( valMask & ( (1 << 4) << i ) )
				{
					*pVal++ = m_RangeAttrMgr[nAttrIdx][idx_value_hight][i];
					nValCnt++;
				}
			}
		}
		break;		

	default:
		return;
	}

	pSync->len = sizeof(SYNC_NPCATTR) + sizeof(int) * nValCnt - 1 - PROTOCOL_SIZE;

	int nMaxCount = MAX_BROADCAST_COUNT_MIN;

	if(bBroadCast)
	{
		BROADCAST_REGION(buf, pSync->len + PROTOCOL_SIZE, nMaxCount);
	}
	else if( IsPlayer() )
	{
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, buf, pSync->len + PROTOCOL_SIZE);
	}
		
}
#endif

#ifdef _SERVER
void KNpc::SyncDamageInfo(int nLauncher, int nDamage, COMBAT_INFO_TYPE damType, int skillId, bool isCrit, bool bBroadCast /* = false */)
{
	DAMAGESHOW	damInfo;

	damInfo.ProtocolType	= s2c_show_damage;	
	damInfo.nDamage			= nDamage;
	damInfo.enType			= (BYTE)damType;
	damInfo.SkillId			= (WORD)skillId;
	damInfo.IsCrit			= isCrit;
	damInfo.dwReceiver		= GetId();
	damInfo.dwLauncher		= Npc[nLauncher].GetId();

	if(bBroadCast)
	{
		int nMaxCount = MAX_BROADCAST_COUNT_MIN;
		BROADCAST_REGION(&damInfo, sizeof(damInfo), nMaxCount);
	}
	else
	{
		if (g_pServer)
		{
			//自己对自己造成的伤害信息不需要重复发送
			if (nLauncher != m_Index)
			{
				int launcherPlayerIndex = Npc[nLauncher].m_nPlayerIdx;
				if (launcherPlayerIndex > 0)
				{
					g_pServer->PackDataToClient(Player[launcherPlayerIndex].GetNetConnectIdx(), &damInfo, sizeof(damInfo));
				}
			}
			
			int selfPlayerIndex = 0;
			if(GetKind() == kind_player)
			{
				selfPlayerIndex = GetPlayerIdx();
			}
			else if (GetKind() == kind_creature)
			{
				//自己的主人对自己造成的伤害信息不需要重复发送
				if (Player[GetSummonerIdx()].GetNpcIndex() != nLauncher)
				{
					selfPlayerIndex = Npc[m_Index].GetSummonerIdx();
				}
			}
			else if (GetKind() == kind_employee)
			{
				if (Player[GetEmployerIdx()].GetNpcIndex() != nLauncher)
				{
					selfPlayerIndex = Npc[m_Index].GetEmployerIdx();
				}
			}

			if (selfPlayerIndex > 0)
			{
				g_pServer->PackDataToClient(Player[selfPlayerIndex].GetNetConnectIdx(), &damInfo, sizeof(damInfo));
			}
		}
	}
}
#endif

#ifndef _SERVER
void KNpc::RecvAttrSync(int nAttrType, int nAttrIdx, BYTE valMask, int *pVal)
{
	switch(nAttrType)
	{
	case npc_attr_unary:
		{
			if(m_UnaryAttrMgr[nuai_titlecolor] != *pVal && nAttrIdx == nuai_titlecolor )
			{
				SetHeadInfoChanged(true);
			}//endif
			
			if (nAttrIdx == nuai_team_id && kind_player == m_Kind)
			{
				int originalTeamId = m_UnaryAttrMgr[nuai_team_id];
				m_UnaryAttrMgr.Set(nAttrIdx, *pVal);
				g_TeamViewer.TeamChanged(m_Index, originalTeamId, *pVal);
			}//endif
			else 
			{
				m_UnaryAttrMgr.Set(nAttrIdx, *pVal);
			}//endif

			if (nAttrIdx == nuai_camou_flage )
			{
				SetHeadInfoChanged(true);
				int nIndex = GetClientPlayer().GetTargetNpc();
				if (nIndex == m_Index)
				{
					CoreDataChanged( CDCNI_UPDATA_SEL_TARGET, NULL, NULL );
				}//endif

			}//endif

		}
		break;
		
	case npc_attr_comp:
		{
			for(int i = 0; i < idx_current_value; ++i)
			{
				if( valMask & (1 << i) )
					m_CompAttrMgr.Set(nAttrIdx, i, *pVal++);
			}
		}
		break;

	case npc_attr_range:
		{
			for(int i = 0; i < idx_current_value; ++i)
			{
				if( valMask & (1 << i) )
					m_RangeAttrMgr.Set(nAttrIdx, idx_value_low, i, *pVal++);

				if( valMask & ( (1 << 4) << i ) )
					m_RangeAttrMgr.Set(nAttrIdx, idx_value_hight, i, *pVal++);
			}
		}
		break;

	default:
		return;
	}	
}
#endif

BOOL KNpc::IsAttackTarget(int nTargetIdx, int nAttackTargetType)
{
	if (!IsValidNpc(nTargetIdx))
		return FALSE;

	KNpc& targetNpc = Npc[nTargetIdx];

	//PK低等级保护
	if ((m_Index != nTargetIdx) && (IsPlayer() || IsCreature() || IsEmployee()) && (targetNpc.IsPlayer() || targetNpc.IsCreature() || targetNpc.IsEmployee()))
	{
		int pkProtectLevel = ConfigManager::Singleton().GetGlobalVariable(global_var_pk_protect_level);
		if ((nAttackTargetType & att_target_enemyplayer) && (GetLevel() < pkProtectLevel || targetNpc.GetLevel() < pkProtectLevel))
			return FALSE;
	}

	if(nAttackTargetType & att_target_deathplayer)
	{
		// 目前仅允许玩家对死亡的玩家进行攻击
		if(kind_player == Npc[nTargetIdx].m_Kind && 
			kind_player == m_Kind && 
			Npc[nTargetIdx].m_Doing == do_revive
		   )
			return TRUE;
		else
			return FALSE;
	}

	BOOL bRet = FALSE;

	int nRelation = NpcSet.GetRelation(m_Index, nTargetIdx);
	switch(nRelation)
	{
	case relation_self:
		if(nAttackTargetType & att_target_self)
			bRet = TRUE;
		break;

	case relation_enemy:
		if(kind_player == Npc[nTargetIdx].m_Kind)
		{
			if(nAttackTargetType & att_target_enemyplayer)
				bRet = TRUE;
		}
		else
		{
			if(nAttackTargetType & att_target_enemynpc || nAttackTargetType & att_target_enemyobj)
				bRet = TRUE;
		}
		break;

	case relation_ally:
		if(kind_player == Npc[nTargetIdx].m_Kind)
		{
			if(nAttackTargetType & att_target_allyplayer)
				bRet = TRUE;
		}
		else
		{
			if(nAttackTargetType & att_target_allynpc || nAttackTargetType & att_target_allyobj)
				bRet = TRUE;
		}
		break;

	default:
		break;
	}

	return bRet;
}

void KNpc::ChangePKMode(PK_MODE mode)
{
	if(mode >= 0 && mode < pk_mode_num)
	{
		if( NpcSet.IsPKModeCanSwitch(m_Index, mode) )
		{
#ifdef _SERVER	
			m_UnaryAttrMgr.Set(nuai_pkmode, mode);
			Player[m_nPlayerIdx].SyncAttribute(attr_pkmode);
#else
			Chg_PK_Mode	reqChgMode;
			reqChgMode.protocol = c2s_chg_pkmode;
			reqChgMode.mode = mode;

			g_pClient->SendPackToServer(g_ConnectID, &reqChgMode, sizeof(Chg_PK_Mode));
#endif

		}
	}	
}

int	KNpc::CheckEspecialAreaType() const
{
	int nX,nY;
	GetMpsPos(&nX, &nY);
	return SubWorld[m_SubWorldIndex].GetEspecialAreaType( nX, nY );
}

#ifndef _SERVER
void KNpc::CheckAndNotifyEspecialArea()
{
	if (IsPlayer())
	{
		static char lightNames[][32] = { "normalarea", "safearea", "fightarea", "questarea", "wararea","cityarea" };
		static int especialAreaMsgIds[] = { ENTER_ESPECIAL_AREA_NONE, ENTER_ESPECIAL_AREA_SAFE, ENTER_ESPECIAL_AREA_PK, ENTER_ESPECIAL_AREA_QUEST, ENTER_ESPECIAL_AREA_WAR , ENTER_ESPECIAL_AREA_CITY};

		int areaType = CheckEspecialAreaType();
		if (m_bIsPkArea != areaType)
		{
			if (m_bIsPkArea > -2)
				CoreDataChanged( GDCNI_CLOSE_LIGHT, (unsigned int)lightNames[m_bIsPkArea + 1], NULL );
			
			if ((areaType > -2 && m_bIsPkArea > -2) || (m_bIsPkArea == (especial_area_none - 1) && areaType > -2))
			{
				CoreDataChanged( GDCNI_OPEN_LIGHT, (unsigned int)lightNames[areaType + 1], NULL );
				int especialAreaMsgId = especialAreaMsgIds[areaType + 1];
				if (especialAreaMsgId > 0)
				{
					char especialAreaMsgBuff[128] = { 0 };			
					g_GetStringRes(especialAreaMsgId, especialAreaMsgBuff, sizeof(especialAreaMsgBuff));
					if (especialAreaMsgBuff[0] != 0)
						CoreDataChanged( GDCNI_ERROR_MESSAGE, (unsigned int)especialAreaMsgBuff, NULL );
				}
			}

			m_bIsPkArea = areaType;
		}
	}
}
#endif

bool KNpc::IsInPKArea()
{	
	return IsInEspecialArea(especial_area_pk);
}

bool KNpc::IsInSafeArea()
{	
	return IsInEspecialArea(especial_area_safe);
}

#ifdef _SERVER
void KNpc::SetupEventBuff(enumNpcEvent npcEvent)
{
	if (npcEvent < 0 || npcEvent >= NpcEvent_Count)
		return;

	if (m_Kind == kind_player)//玩家
	{
		int playerEventBuff = 0;
		int configurableBuff = -1;
		switch(npcEvent)
		{
		case NpcEvent_Revive:
			configurableBuff = global_var_buff_player_revive;
			break;
		case NpcEvent_Death:
			configurableBuff = global_var_buff_player_death;
			break;
		}

		if (configurableBuff >= 0)
		{
			playerEventBuff = ConfigManager::Singleton().GetGlobalVariable((enumGlobalVariable)configurableBuff);
			if (playerEventBuff > 0)
			{
				unsigned long buffIndex = 0;
				buffIndex = BuffMgr::Singleton().AddNpcBuff(
					m_Index, m_Index, playerEventBuff );
			}
		}
	}
	else if (m_Kind == kind_employee)//佣兵，有专用的BUFF
	{
	}
	else//NPC
	{
		if (m_pTemplate != NULL)
		{
			for (int eventBuffLoopCount = 0; eventBuffLoopCount < MAX_NPC_EVENT_BUFF_COUNT; eventBuffLoopCount++)
			{
				int npcEventBuff = m_pTemplate->m_EventBuff.BuffID[npcEvent][eventBuffLoopCount];
				if (npcEventBuff > 0)
				{
					unsigned long buffIndex = 0;
					buffIndex = BuffMgr::Singleton().AddNpcBuff(
						m_Index, m_Index, npcEventBuff );
				}
			}
		}
	}	
}

void KNpc::BeUsedSkill(int skillCaster, SkillType skillType)
{
	bool pkPunish = true;
	if( IsInPKArea( ) || IsInEspecialArea(especial_area_war) || IsInEspecialArea(especial_area_city))
		pkPunish = false;

	KNpc& caster = Npc[skillCaster];
	switch(skillType)
	{
	case skill_type_friendly:
		if (skillCaster != m_Index)
		{
			//当Npc A给Npc B加增益buff时，只要有一方进入战斗状态Npc A、B都进入战斗状态
			if ( caster.IsFightState() )
			{
				SetFightState( true );
			}		
			if ( IsFightState() )
			{
				caster.SetFightState( true );
			}
			//对玩家的召唤兽进行的行为视为对其主人进行的行为
			if (IsPlayer() || IsCreature())
			{
				KPlayer& selfPlayer = Player[IsPlayer() ? GetPlayerIdx() : GetSummonerIdx()];
				
				if (pkPunish)
				{
					//召唤兽对其他玩家进行的行为视为其主人做出的行为
					if (caster.IsPlayer() || caster.IsCreature())
					{
						//当玩家A给玩家B（杀手状态，红名或深红名）加增益buff时，玩家A变成杀手状态（灰名状态）
						if (selfPlayer.IsKiller() || selfPlayer.GetPkValue() >= ConfigManager::Singleton().GetGlobalVariable(global_var_pk_boundary_alert_to_punish))
						{
							int killerBuffID = ConfigManager::Singleton().GetGlobalVariable(global_var_buff_killer);
							if (killerBuffID > 0)
							{
								unsigned long killBuffIndex = 0;
								
								int killerNpcIndex = 0;
								if (caster.IsPlayer())
								{
									killerNpcIndex = skillCaster;
								}
								else
								{
									killerNpcIndex = Player[caster.GetPlayerIdx()].GetNpcIndex();
								}
								
								_ASSERT(killerNpcIndex > 0);
								if (killerNpcIndex > 0)
								{
									killBuffIndex = BuffMgr::Singleton().AddNpcBuff(
										m_Index, killerNpcIndex, killerBuffID );
									_ASSERT(killBuffIndex);						
								}							
							}
						}
					}
				}
			}
		}
		//TODO
		break;
	case skill_type_neutral:
		//TODO
		break;
	case skill_type_hostile:
		if (skillCaster != m_Index)
		{
			//当Npc A给Npc B加减益buff时Npc A、B都进入战斗状态
			SetFightState( true );
			caster.SetFightState( true );

			if (caster.IsPlayer() && IsValidPlayer(caster.GetPlayerIdx()))
			{
				KPlayer& casterPlayer = Player[caster.GetPlayerIdx()];

				//控制召唤兽攻击主人的攻击目标
				KNpc *pCreature = casterPlayer.m_Creature.GetCreatureNpc();
				if(NULL != pCreature && casterPlayer.m_Creature.GetMark() == skillCaster)
					pCreature->SetTarget(type_npc, m_Index);
				int nMarkCreatureIdx = casterPlayer.GetMarkCreature();
				if( IsValidNpc(nMarkCreatureIdx) )
				{
					if(kind_creature == Npc[nMarkCreatureIdx].m_Kind)
						Npc[nMarkCreatureIdx].SetTarget(type_npc, m_Index);
				}
				
				//控制佣兵攻击主人的攻击目标
				Employee& employee = casterPlayer.GetEmployee();
				if (employee.IsExist())
				{
					Npc[employee.GetNpcIndex()].SetTarget(type_npc, m_Index);
				}
			}

			//对玩家的召唤兽进行的行为视为对其主人进行的行为
			if ((IsPlayer() || IsCreature() || IsEmployee()) && IsValidPlayer(m_nPlayerIdx))
			{
				KPlayer& selfPlayer = Player[m_nPlayerIdx];

				//召唤兽没有目标时，如果有NPC攻击召唤兽，将该NPC设置为召唤兽的攻击目标
				if (IsCreature() || IsEmployee())
				{
					if(GetTargetNpc() == 0)
					{
						SetTarget(type_npc, skillCaster);
					}
				}
				else if (IsPlayer())
				{
					//控制召唤兽反击攻击者
					if( selfPlayer.m_Creature.IsALive() && selfPlayer.m_Creature.GetMark() == m_Index)
					{
						KNpc *pCreature = selfPlayer.m_Creature.GetCreatureNpc();
						if( pCreature && 0 == pCreature->GetTargetNpc() )
							pCreature->SetTarget(type_npc, skillCaster);
					}
					int nMarkCreatureIdx = selfPlayer.GetMarkCreature();
					if( IsValidNpc(nMarkCreatureIdx) )
					{
						if(kind_creature == Npc[nMarkCreatureIdx].m_Kind)
						{
							if( 0 == Npc[nMarkCreatureIdx].GetTargetNpc() )
								Npc[nMarkCreatureIdx].SetTarget(type_npc, skillCaster);
						}
					}

					//控制佣兵反击攻击者
					Employee& employee = selfPlayer.GetEmployee();
					if (employee.IsExist() && 0 == Npc[employee.GetNpcIndex()].GetTargetNpc())
					{
						Npc[employee.GetNpcIndex()].SetTarget(type_npc, skillCaster);
					}
				}

				//召唤兽对其他玩家进行的行为视为其主人做出的行为
				if (pkPunish)
				{
					if (caster.IsPlayer() || caster.IsCreature() || caster.IsEmployee())
					{
						//当玩家A攻击玩家B（白名或黄名，且不处于杀手状态）时，玩家A变成杀手状态（灰名状态）
						if (!selfPlayer.IsKiller() && selfPlayer.GetPkValue() < ConfigManager::Singleton().GetGlobalVariable(global_var_pk_boundary_alert_to_punish))
						{
							int killerBuffID = ConfigManager::Singleton().GetGlobalVariable(global_var_buff_killer);
							if (killerBuffID > 0)
							{
								unsigned long killBuffIndex = 0;
								
								int killerNpcIndex = caster.m_nPlayerIdx;
								
								_ASSERT(killerNpcIndex > 0);
								if (killerNpcIndex > 0)
								{
									killBuffIndex = BuffMgr::Singleton().AddNpcBuff(
										m_Index, killerNpcIndex, killerBuffID );
									_ASSERT(killBuffIndex);						
								}							
							}
						}
					}
				}
			}
		}
		break;
	default:
		_ASSERT(false);
	}

	if (skillCaster != m_Index)
	{
		m_Controller.OnEvent(NpcEvent_BeUsedSkill, NULL);
	}
}

void KNpc::SetNpcTitleColor( unsigned int color )
{
	m_UnaryAttrMgr.Set( nuai_titlecolor, color );
	SyncAttr( npc_attr_unary, nuai_titlecolor, 0, true );
}

#endif

#ifdef _SERVER
void KNpc::AddInitSkills()
{	
	if( !IsPlayer() )
		return;

	m_SkillList.Clear();

	PlayerCreator &PC = PlayerCreator::Singleton();
	const DBSkillData* pDBSkillData = (const DBSkillData*)PC.GetInitialSkillData(m_Series, m_nSex);

	if(NULL != pDBSkillData)
	{
		int nInitialSkillCnt = PC.GetInitialSkillCount(m_Series, m_nSex);

		for(int nSkillLoop = 0; nSkillLoop < nInitialSkillCnt; ++nSkillLoop)
			m_SkillList.AddSkillEx(pDBSkillData[nSkillLoop].skillId,
				pDBSkillData[nSkillLoop].skillLevel,
				pDBSkillData[nSkillLoop].status
				);
	}


	int nSkillSeries = Player[m_nPlayerIdx].m_SkillSeries;
	int	MainSkillIds[MAX_MAINSKILL_PER_SERIES];
	int	SubSkillIds[MAX_SUBSKILL_PER_MAINSKILL];
	int nMainSkill;
	int nSubSkill;

	nMainSkill = g_SkillManager.GetMainSkillIds(m_Series, nSkillSeries, MainSkillIds, sizeof(MainSkillIds));

	for(int i = 0; i < nMainSkill; ++i)
	{
		if( g_SkillManager.IsAddToListWhenInit(MainSkillIds[i]) )
		{
			m_SkillList.AddSkillEx(MainSkillIds[i], 0, skill_status_inactive);
			nSubSkill = g_SkillManager.GetInitSubSkillIds(MainSkillIds[i], SubSkillIds, sizeof(SubSkillIds));

			for(int j = 0; j < nSubSkill; ++j)
			{
				if( g_SkillManager.IsAddToListWhenInit(SubSkillIds[j]) )
					m_SkillList.AddSkillEx(SubSkillIds[j], 0, skill_status_inactive);
			}
		}
	}

	Player[m_nPlayerIdx].SendSyncData_Skill();	
}
#endif

#ifdef _SERVER
void KNpc::CheckInitSkills()
{
	if( !IsPlayer() )
		return;

	PlayerCreator &PC = PlayerCreator::Singleton();
	const DBSkillData* pDBSkillData = (const DBSkillData*)PC.GetInitialSkillData(m_Series, m_nSex);
	if(NULL != pDBSkillData)
	{
		int nInitialSkillCnt = PC.GetInitialSkillCount(m_Series, m_nSex);
		for(int nSkillLoop = 0; nSkillLoop < nInitialSkillCnt; ++nSkillLoop)
		{
			if (m_SkillList.FindSkill(pDBSkillData[nSkillLoop].skillId) == INVALID_SKILL_INDEX)
			{
				m_SkillList.AddSkillEx(
					pDBSkillData[nSkillLoop].skillId,
					pDBSkillData[nSkillLoop].skillLevel,
					pDBSkillData[nSkillLoop].status
				);
			}
		}
	}

	int nSkillSeries = Player[m_nPlayerIdx].m_SkillSeries;
	int	MainSkillIds[MAX_MAINSKILL_PER_SERIES];
	int	SubSkillIds[MAX_SUBSKILL_PER_MAINSKILL];
	int nMainSkill;
	int nSubSkill;

	nMainSkill = g_SkillManager.GetMainSkillIds(m_Series, nSkillSeries, MainSkillIds, sizeof(MainSkillIds));

	for(int i = 0; i < nMainSkill; ++i)
	{
		if( g_SkillManager.IsAddToListWhenInit(MainSkillIds[i])	)
		{
			if (m_SkillList.FindSkill(MainSkillIds[i]) == INVALID_SKILL_INDEX)
			{
				m_SkillList.AddSkillEx(MainSkillIds[i], 0, skill_status_inactive);
			}
			
			nSubSkill = g_SkillManager.GetInitSubSkillIds(MainSkillIds[i], SubSkillIds, sizeof(SubSkillIds));

			for(int j = 0; j < nSubSkill; ++j)
			{
				if( g_SkillManager.IsAddToListWhenInit(SubSkillIds[j])
					&& m_SkillList.GetCurSameSubSkillId(SubSkillIds[j]) == INVALID_SKILL_ID)
					m_SkillList.AddSkillEx(SubSkillIds[j], 0, skill_status_inactive);
			}
		}
	}
}
#endif

BOOL KNpc::IsPlayer()
{
#ifdef _SERVER
	return m_Kind == kind_player;
#else
	return m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex;
#endif
}

#ifndef _SERVER
void KNpc::LevelUpSkill(int nSkillId, int nAddedLevel)
{
	int nSkillIdx = m_SkillList.FindSkill(nSkillId);
	int nTargetLvl = m_SkillList.GetLevelByIdx(nSkillIdx) + nAddedLevel;

	if( !g_SkillManager.IsMainSkill(nSkillId) )
	{
		if( g_SkillManager.CanUpdateTo(m_Index, nSkillId, nTargetLvl) )
		{
			PLAYER_SKILLINFO_SYNC	sync;
			
			sync.ProtocolType = c2s_skill_sync;
			sync.Operation = skill_ope_levelup;
			sync.SkillId = nSkillId;
			sync.PlusInfo.comInfo.Level = nTargetLvl;

			g_pClient->SendPackToServer(g_ConnectID, &sync, sizeof(sync));
		}	
	}
}
#endif

#ifdef _SERVER
BOOL KNpc::IsCanRandomTrans()
{
	return SubWorld[m_SubWorldIndex].IsCanRandomTrans();
}

BOOL KNpc::RandomTrans()
{
	int nTransX = 0;
	int nTransY = 0;

	if(SubWorld[m_SubWorldIndex].GetRandomTransPos(nTransX, nTransY))
	{
		if(SetPos(nTransX, nTransY))
		{
			return TRUE;
		}
	}

	return FALSE;
}
#endif

void KNpc::MoveNpc(int nRegion, int nMapX, int nMapY, int nOffX, int nOffY )
{
	if ( !m_bClientOnly && m_RegionIndex >= 0 && m_Kind == kind_normal) 
		CURREGION.DecNpcRef(m_Index);
#ifndef _SERVER
	if ( GetCurrentLifePercentage() == 0 && m_Kind == kind_normal )
	{
		return;
	}
#endif
	int nOldRegion = m_RegionIndex;
	if (m_RegionIndex != nRegion)
	{
#ifdef _SERVER
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, nRegion, m_Index);
		if (IsPlayer())
		{
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, nRegion, m_nPlayerIdx);
		}
#else
		if (nRegion >= 0)
		{		
			if (nOldRegion >= 0)
			{
				SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[nRegion].m_RegionID, m_Index);
			}
			else
			{
				SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, m_Index);		
			}
			m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
		}
		else
		{
			if (m_RegionIndex >= 0)
			{
				SubWorld[0].m_Region[nOldRegion].RemoveNpc(m_Index);
				m_RegionIndex = -1;
				m_dwRegionID = -1;
			}
		}
#endif
	}
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_OffX = nOffX;
	m_OffY = nOffY;
	if ( !m_bClientOnly && m_RegionIndex >= 0 && m_Kind == kind_normal) 
		CURREGION.AddNpcRef(m_Index);
}

#ifdef _SERVER
bool KNpc::Save()
{
	SetDataChangedFlag(false);
	return NpcSave::SaveNpc(m_Index);
}
#endif

#ifdef _SERVER
void KNpc::SyncEquipTalismanNpcId()
{
	SYNC_NPC_EQUIP_TALISMAN equipTalismanSync;
	equipTalismanSync.Protocol = s2c_sync_npc_equip_talisman;
	equipTalismanSync.NpcId = GetId();
	equipTalismanSync.TalismanNpcId = (WORD)m_EquipTalismanNpcId;
	int maxCount = MAX_BROADCAST_COUNT_MIN;
	BROADCAST_REGION(&equipTalismanSync, sizeof(SYNC_NPC_EQUIP_TALISMAN), maxCount);
}
#endif

void KNpc::SetEquipTalismanNpcId(int id)
{
#ifndef _SERVER
	if (id != m_EquipTalismanNpcId || (id > 0 && m_TalismanNpcController.GetTalismanNpc() == 0))
	{
		if (id > 0)
		{	
			int npcPosX, npcPosY;
			SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &npcPosX, &npcPosY);
			
			ClientTalismanNpc* pTalismanNpcInfo = ClientTalismanNpcTable::Singleton().GetClientTalismanNpc(id);
			if (pTalismanNpcInfo != NULL)
			{				
				int	nNpcIdxInfo = MAKELONG(1, pTalismanNpcInfo->NpcId_Normal);
				int npcIndex = NpcSet.Add(nNpcIdxInfo, 0, npcPosX, npcPosY);
				if (npcIndex > 0)
				{
					KNpc& talismanNpc = Npc[npcIndex];
					talismanNpc.Name[0] = 0;
					talismanNpc.m_Kind = kind_talisman;
					talismanNpc.m_SyncSignal = SubWorld[0].m_dwCurrentTime;		
					m_TalismanNpcController.SetTalismanNpc(npcIndex, pTalismanNpcInfo);
				}
			}			
		}
		else
		{
			int talismanNpcIndex = m_TalismanNpcController.GetTalismanNpc();
			if (talismanNpcIndex > 0)
			{
				KNpc& talismanNpc = Npc[talismanNpcIndex];
				if (talismanNpc.m_RegionIndex >= 0)
				{
					int nSubWorld = talismanNpc.m_SubWorldIndex;
					int nRegion = talismanNpc.m_RegionIndex;
					SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(talismanNpcIndex);
				}
				NpcSet.Remove(talismanNpcIndex, false);
				m_TalismanNpcController.SetTalismanNpc(0, NULL);
			}			
		}
	}	
#endif

	m_EquipTalismanNpcId = id;
}

#ifndef _SERVER

BOOL KEmoteImage::Load( void )
{
	if (m_IniFile.Load(FACE_LAYOUT_STRING))
	{
		for ( int i = 0; i < MAX_EMOTE_COUNT; ++i )
		{
			AddImage( i );
		}
		return TRUE;
	}			
				
	return FALSE;
}

void	KEmoteImage::AddImage( int type )
{
	IR_InitUiImageRef(m_EmoteImage[type]);
	char szSection[COMMON_CLIENT_MSG_LEN_64];
	sprintf( szSection, "EmoteIcon_%d", type );
	m_IniFile.GetString( szSection, "ImagePath", "", m_EmoteImage[type].szImage, sizeof(m_EmoteImage[type].szImage));
	m_EmoteImage[type].bRenderFlag		= RUIMAGE_RENDER_FLAG_REF_SPOT;
	m_EmoteImage[type].bRenderStyle		= IMAGE_RENDER_STYLE_3LEVEL;
	m_EmoteImage[type].nType			= ISI_T_SPR;
	m_EmoteImage[type].Color.Color_dw	= 0xFF000000;
	m_EmoteImage[type].oPosition.nX		= 0;
	m_EmoteImage[type].oPosition.nY		= 0;
	m_EmoteImage[type].oEndPos.nX		= 0;
	m_EmoteImage[type].oEndPos.nY		= 0;
	m_EmoteImage[type].oEndPos.nZ		= 0;
	m_EmoteImage[type].uImage			= 0;
	m_EmoteImage[type].nISPosition		= -1;
}

bool	KEmoteImage::GetImage( EmoteType type, KUiImageRef& rImage )
{
	if ( type > 0 && type < MAX_EMOTE_COUNT )
	{
		rImage = m_EmoteImage[type];
		return true;
	}
	return false;
}

#endif

int KNpc::GetNormalSkillId()
{
	if(enRoleType_Knight == m_Series)
		return KNIGHT_NORMALSKILL_ID;
	else if(enRoleType_Enchanter == m_Series)
		return ENCHANTER_NORMALSKILL_ID;
	else if(enRoleType_Monstrous == m_Series)
		return MONSTROUS_NORMAILSKILL_ID;
	else
	{
		_ASSERT(FALSE);
		return INVALID_SKILL_ID;
	}
}

#ifndef _SERVER
void KNpc::AddIconToHeadInfo( const std::string& imageset, const std::string& image, const std::string name ,const HeadInfoPriority pri/*= HIP_Normal*/)
{
	ConfigManager& cm = ConfigManager::Singleton();
	const char* szTemplate = cm.GetConfigurableDisplayStyle( style_role_head_normal_info, 0 );
	if ( szTemplate )
	{
		char szBuff[COMMON_CLIENT_MSG_LEN_1024];
		sprintf( szBuff, szTemplate, imageset.c_str(), image.c_str() );
		m_HeadInfoPlus[name].m_InfoString = szBuff;
		m_HeadInfoPlus[name].m_Prioryty   = pri;
		m_bHeadInfoChanged = true;
	}
}

void KNpc::AddTxtToHeadInfo( const std::string& text, const std::string colours, const std::string name    ,const HeadInfoPriority pri/*= HIP_Normal*/)
{
	ConfigManager& cm = ConfigManager::Singleton();
	const char* szTemplate = cm.GetConfigurableDisplayStyle( style_role_head_normal_info, 1 );
	if ( szTemplate )
	{
		char szBuff[COMMON_CLIENT_MSG_LEN_1024];
		sprintf( szBuff, szTemplate,  colours.c_str(), text.c_str() );
		m_HeadInfoPlus[name].m_InfoString = szBuff;
		m_HeadInfoPlus[name].m_Prioryty   = pri;
		m_bHeadInfoChanged = true;
	}
}

void KNpc::AddLayoutToHeadInfo(const std::string& text, const std::string name , const HeadInfoPriority pri/* = HIP_Normal  */)
{
	m_HeadInfoPlus[name].m_InfoString = text;
	m_HeadInfoPlus[name].m_Prioryty   = pri;
	m_bHeadInfoChanged                = true;
}

void	KNpc::DelHeadInfo(  const std::string name )
{
	std::map<std::string,HEAD_INFO>::iterator it = m_HeadInfoPlus.find( name );
	if ( it != m_HeadInfoPlus.end() )
	{
		m_HeadInfoPlus.erase( it );
		m_bHeadInfoChanged = true;
	}
}

void KNpc::CheckAndSwitchSkill()
{
	int nSwitchSkillId = m_SkillList.GetSwitchSkillId();

	if(INVALID_SKILL_ID != nSwitchSkillId)
	{
		int	nNormalSkillId = GetNormalSkillId();

		if(nNormalSkillId == m_ActiveSkillID || 
			nSwitchSkillId == m_ActiveSkillID)
		{
			KSkill *pSwitchSkill = g_SkillManager.GetSkill(nSwitchSkillId, 1);

			if( pSwitchSkill && Cost(pSwitchSkill, TRUE) )
			{
				if(nSwitchSkillId != m_ActiveSkillID)
				{
					int nIdx = m_SkillList.FindSkill(nSwitchSkillId);
					SetActiveSkill(nIdx);
				}
			}
			else
			{
				if(nNormalSkillId != m_ActiveSkillID)
				{
					int	nIdx = m_SkillList.FindSkill(nNormalSkillId);
					SetActiveSkill(nIdx);
				}
			}
		}
	}	
}
#endif

void KNpc::UpdateBodyEffect(int nOldVal, int nAddedVal, bool bSyncToClient, bool bBroadCast)
{
	int nAddedLifeLimit = PlayerBaseNumeric::Body2LifeUpLimit(m_Series, nAddedVal);
	AddCompAttr(ncai_lifeuplimit, idx_append_value, nAddedLifeLimit);

// 	int nOldAddPhysExplode = PlayerBaseNumeric::Body2PhysExplode(nOldVal);
// 	int nNewAddPhysExplode = PlayerBaseNumeric::Body2PhysExplode(nOldVal + nAddedVal);
// 	int nAddPhysExplode = nNewAddPhysExplode - nOldAddPhysExplode;
// 	AddCompAttr(ncai_physexplode, idx_append_value, nAddPhysExplode);

// 	int nOldAddDexterity = PlayerBaseNumeric::Body2Dexterity(nOldVal);
// 	int nNewAddDexterity = PlayerBaseNumeric::Body2Dexterity(nOldVal + nAddedVal);
// 	int nAddDexterity = nNewAddDexterity - nOldAddDexterity;
// 	AddCompAttr(ncai_dexterity, idx_append_value, nAddDexterity);

#ifdef _SERVER
	if(bSyncToClient)
	{
		SyncAttr(npc_attr_comp, ncai_lifeuplimit, 1 << idx_append_value, bBroadCast);
// 		SyncAttr(npc_attr_comp, ncai_physexplode, 1 << idx_append_value, bBroadCast);
// 		SyncAttr(npc_attr_comp, ncai_dexterity, 1 << idx_append_value, bBroadCast);
	}
#endif
}

void KNpc::UpdateNimbusEffect(int nOldVal, int nAddedVal, bool bSyncToClient, bool bBroadCast)
{
	int nAddedManaLimit = PlayerBaseNumeric::Nimbus2ManaUpLimit(m_Series, nAddedVal);
	AddCompAttr(ncai_manauplimit, idx_append_value, nAddedManaLimit);

// 	int nOldAddMagicExplode = PlayerBaseNumeric::Nimbus2MagicExplode(nOldVal);
// 	int nNewAddMagicExplode = PlayerBaseNumeric::Nimbus2MagicExplode(nOldVal + nAddedVal);
// 	int	nAddMagicExplode = nNewAddMagicExplode - nOldAddMagicExplode;
// 	AddCompAttr(ncai_magicexplode, idx_append_value, nAddMagicExplode);

// 	int nOldAddVision = PlayerBaseNumeric::Nimbus2Vision(nOldVal);
// 	int nNewAddVision = PlayerBaseNumeric::Nimbus2Vision(nOldVal + nAddedVal);
// 	int nAddVision = nNewAddVision - nOldAddVision;
// 	AddCompAttr(ncai_vision, idx_append_value, nAddVision);

#ifdef _SERVER
	if(bSyncToClient)
	{
		SyncAttr(npc_attr_comp, ncai_manauplimit, 1 << idx_append_value, bBroadCast);	
// 		SyncAttr(npc_attr_comp, ncai_magicexplode, 1 << idx_append_value, bBroadCast);
// 		SyncAttr(npc_attr_comp, ncai_vision, 1 << idx_append_value, bBroadCast);
	}
#endif
}

void KNpc::UpdateStrengthEffect(int nOldVal, int nAddedVal, bool bSyncToClient, bool bBroadCast)
{
// 	int nAddedNew = PlayerBaseNumeric::Strength2Damage(m_Series, nOldVal + nAddedVal);
// 	int nAddedOld = PlayerBaseNumeric::Strength2Damage(m_Series, nOldVal);
// 	int nAddedDamage = nAddedNew - nAddedOld;
// 	AddRangeAttr(nrai_damage_physics, idx_value_low, idx_append_value, nAddedDamage);
// 	AddRangeAttr(nrai_damage_physics, idx_value_hight, idx_append_value, nAddedDamage);	

// 	int nOldAddPhysDefense = PlayerBaseNumeric::Strength2PhysDefense(nOldVal);
// 	int nNewAddPhysDefense = PlayerBaseNumeric::Strength2PhysDefense(nOldVal + nAddedVal);
// 	int nAddPhysDefense = nNewAddPhysDefense - nOldAddPhysDefense;
// 	AddRangeAttr(nrai_defend_physics, idx_value_low, idx_append_value, nAddPhysDefense);
// 	AddRangeAttr(nrai_defend_physics, idx_value_hight, idx_append_value, nAddPhysDefense);

// #ifdef _SERVER
// 	if(bSyncToClient)
// 	{
// 		SyncAttr(npc_attr_range, nrai_damage_physics, (1 + 16) << idx_append_value, bBroadCast);
// 		SyncAttr(npc_attr_range, nrai_defend_physics, (1 + 16) << idx_append_value, bBroadCast);
// 	}
// #endif
}

void KNpc::UpdateArtEffect(int nOldVal, int nAddedVal, bool bSyncToClient, bool bBroadCast)
{
// 	int nAddedNew = PlayerBaseNumeric::Magic2Damage(m_Series, nOldVal + nAddedVal);
// 	int nAddedOld = PlayerBaseNumeric::Magic2Damage(m_Series, nOldVal);
// 	int nAddedDamage = nAddedNew - nAddedOld;
// 	AddRangeAttr(nrai_damage_magic, idx_value_low, idx_append_value, nAddedDamage);
// 	AddRangeAttr(nrai_damage_magic, idx_value_hight, idx_append_value, nAddedDamage);	
// 
// 	int nOldAddEightDiagDfns = PlayerBaseNumeric::Magic2EightDiagDefense(nOldVal);
// 	int nNewAddEightDiagDfns = PlayerBaseNumeric::Magic2EightDiagDefense(nOldVal + nAddedVal);
// 	int nAddEightDiagDfns = nNewAddEightDiagDfns - nOldAddEightDiagDfns;
// 	AddRangeAttr(nrai_defend_eightdiag, idx_value_low, idx_append_value, nAddEightDiagDfns);
// 	AddRangeAttr(nrai_defend_eightdiag, idx_value_hight, idx_append_value, nAddEightDiagDfns);
// 
// 	int nOldAddDarkDfns = PlayerBaseNumeric::Magic2DarkDefense(nOldVal);
// 	int nNewAddDarkDfns = PlayerBaseNumeric::Magic2DarkDefense(nOldVal + nAddedVal);
// 	int nAddDarkDfns = nNewAddDarkDfns - nOldAddDarkDfns;
// 	AddRangeAttr(nrai_defend_dark, idx_value_low, idx_append_value, nAddDarkDfns);
// 	AddRangeAttr(nrai_defend_dark, idx_value_hight, idx_append_value, nAddDarkDfns);
// 
// #ifdef _SERVER
// 	if(bSyncToClient)
// 	{
// 		SyncAttr(npc_attr_range, nrai_damage_magic, (1 + 16) << idx_append_value, bBroadCast);
// 		SyncAttr(npc_attr_range, nrai_defend_eightdiag, (1 + 16) << idx_append_value, bBroadCast);
// 		SyncAttr(npc_attr_range, nrai_defend_dark, (1 + 16) << idx_append_value, bBroadCast);
// 	}
// #endif
}

#ifdef _SERVER
void KNpc::BroadCastRegion(const void* pBuffer, unsigned int size, int& maxBroadCastCount)
{
	BROADCAST_REGION(pBuffer, size, maxBroadCastCount);
}
#endif

#ifdef _SERVER
int KNpc::GetCurCityLordNpc()
{
	return SubWorld[m_SubWorldIndex].GetLord();
}
#endif

#ifdef _SERVER
void KNpc::SetVisibleToPlayerCount(int visibleCount)
{
	//消失的时候要通知周围的客户删除这个NPC
	if (m_VisibleToPlayerCount > 0 && visibleCount <= 0)
	{
		NPC_REMOVE_SYNC	RemoveSync;
		RemoveSync.ProtocolType = s2c_npcremove;
		RemoveSync.ID = m_dwID;
		SendDataToNearRegion(&RemoveSync, sizeof(RemoveSync));
	}

	m_VisibleToPlayerCount = visibleCount;
}
#endif

#ifdef _SERVER
bool KNpc::CheckExpire()
{
	if (m_ExpireTime == 0)
		return false;

	//过期了，需要删除
	if (m_ExpireTime < UNIX_TMIE_STAMP)
	{
		if(m_SubWorldIndex >= 0 && m_RegionIndex >= 0)
		{
			SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		}
 		NpcSet.Remove(m_Index);

		return true;
	}

	return false;
}
#endif

#ifdef _SERVER
void KNpc::SyncToWorld()
{
	if (m_SubWorldIndex >= 0)
	{
		NPC_SYNC_TO_WORLD syncWorld;
		syncWorld.Protocol = s2c_npc_sync_to_world;
		syncWorld.NpcId = m_dwID;
		memcpy(syncWorld.Param, m_SyncToWorldParams, sizeof(syncWorld.Param));
		GetMpsPos((int*)&syncWorld.PosX, (int*)&syncWorld.PosY);

		SubWorld[m_SubWorldIndex].BroadCast((char*)&syncWorld, sizeof(syncWorld));
	}
}
#endif

#ifdef _SERVER
void KNpc::SyncToWorldMin(const unsigned int clientId)
{
	if (m_SyncToWorldMode > 0 && m_SubWorldIndex >= 0)
	{
		NPC_SYNC_TO_WORLD_MIN syncWorldMin;
		syncWorldMin.Protocol = s2c_npc_sync_to_world_min;
		syncWorldMin.NpcId = m_dwID;
		syncWorldMin.Mode = m_SyncToWorldMode;
		strncpy(syncWorldMin.Name, Name, sizeof(syncWorldMin.Name));
		syncWorldMin.Name[sizeof(syncWorldMin.Name) - 1] = 0;
		memcpy(syncWorldMin.Param, m_SyncToWorldParams, sizeof(syncWorldMin.Param));
		GetMpsPos((int*)&syncWorldMin.PosX, (int*)&syncWorldMin.PosY);

		if (g_pServer != NULL)
			g_pServer->PackDataToClient(clientId, &syncWorldMin, sizeof(syncWorldMin));
	}
}
#endif

#ifdef _SERVER
void KNpc::SyncToWorldDel()
{
	if (m_SubWorldIndex >= 0)
	{
		NPC_SYNC_TO_WORLD_DEL syncWorldDel;
		syncWorldDel.Protocol = s2c_npc_sync_to_world_del;
		syncWorldDel.NpcId = m_dwID;

		SubWorld[m_SubWorldIndex].BroadCast((char*)&syncWorldDel, sizeof(syncWorldDel));
	}
}
#endif

#ifdef _SERVER
void KNpc::SetSyncToWorldMode(int mode)
{
	if (m_SyncToWorldMode != mode)
	{
		if (m_SubWorldIndex >= 0)
		{
			SubWorld[m_SubWorldIndex].RemoveFromSyncToWorldList(m_Index);
			SyncToWorldDel();
		}

		m_SyncToWorldMode = mode;

		if (m_SubWorldIndex >= 0 && mode > 0)
		{
			SubWorld[m_SubWorldIndex].AddToSyncToWorldList(m_Index);
			SyncToWorld();
		}
	}
}
#endif

#ifdef _SERVER
bool KNpc::CanCancelSkill()
{
	if (IsCastingSkill()
		&& m_Frames.nCurrentFrame > 0
		&& (m_Frames.nCurrentFrame < m_Frames.nTotalFrame * ATTACKACTION_EFFECT_PERCENT / 100))//技能尚未产生效果
	{
		//判断当前技能是否可以被取消
		KSkill* pSkill = GetActiveSkill();
		if (pSkill)
		{
			return pSkill->IsCancelable();
		}
	}

	return false;
}
#endif

#ifdef _SERVER
bool KNpc::CancelSkill()
{
	if (CanCancelSkill())
	{
		DoStand();
		m_ProcessAI = 1;

		return true;
	}

	return false;
}
#endif

int KNpc::CalcDexterity()
{
	return (PlayerBaseNumeric::Body2Dexterity(m_CompAttrMgr[ncai_body]) + m_CompAttrMgr[ncai_dexterity][idx_base_value] + m_CompAttrMgr[ncai_dexterity][idx_append_value])
		* (100 + m_CompAttrMgr[ncai_dexterity][idx_append_percent]) / 100;
}

int KNpc::CalcVision()
{
	return (PlayerBaseNumeric::Nimbus2Vision(m_CompAttrMgr[ncai_nimbus]) + m_CompAttrMgr[ncai_vision][idx_base_value] + m_CompAttrMgr[ncai_vision][idx_append_value])
		* (100 + m_CompAttrMgr[ncai_vision][idx_append_percent]) / 100;
}

int KNpc::CalcPhysExplode()
{
	return (PlayerBaseNumeric::Body2PhysExplode(m_CompAttrMgr[ncai_body]) + m_CompAttrMgr[ncai_physexplode][idx_base_value] + m_CompAttrMgr[ncai_physexplode][idx_append_value])
		* (100 + m_CompAttrMgr[ncai_physexplode][idx_append_percent]) / 100;
}

int KNpc::CalcMagicExplode()
{
	return (PlayerBaseNumeric::Nimbus2MagicExplode(m_CompAttrMgr[ncai_nimbus]) + m_CompAttrMgr[ncai_magicexplode][idx_base_value] + m_CompAttrMgr[ncai_magicexplode][idx_append_value])
		* (100 + m_CompAttrMgr[ncai_magicexplode][idx_append_percent]) / 100;
}

int KNpc::CalcPhysicsDamage()
{
	return RandomRangeValue(CalcPhysicsDamage(idx_value_low), CalcPhysicsDamage(idx_value_hight));
}

int KNpc::CalcPhysicsDamage(enRangeMemberIdx idx)
{
	if (idx_value_low == idx || idx_value_hight == idx)
		return (PlayerBaseNumeric::Strength2Damage(m_Series, m_CompAttrMgr[ncai_strength]) + m_RangeAttrMgr[nrai_damage_physics][idx][idx_base_value] + m_RangeAttrMgr[nrai_damage_physics][idx][idx_append_value])
			* (100 + m_RangeAttrMgr[nrai_damage_physics][idx][idx_append_percent]) / 100;
	else
		return 0;
}

int KNpc::CalcPhysicsDefense()
{
	return RandomRangeValue(CalcPhysicsDefense(idx_value_low), CalcPhysicsDefense(idx_value_hight));
}

int KNpc::CalcPhysicsDefense(enRangeMemberIdx idx)
{
	if (idx_value_low == idx || idx_value_hight == idx)
		return (PlayerBaseNumeric::Strength2PhysDefense(m_CompAttrMgr[ncai_strength]) + m_RangeAttrMgr[nrai_defend_physics][idx][idx_base_value] + m_RangeAttrMgr[nrai_defend_physics][idx][idx_append_value])
			* (100 + m_RangeAttrMgr[nrai_defend_physics][idx][idx_append_percent]) / 100;
	else
		return 0;
}

int KNpc::CalcMagicDamage()
{
	return RandomRangeValue(CalcMagicDamage(idx_value_low), CalcMagicDamage(idx_value_hight));
}

int KNpc::CalcMagicDamage(enRangeMemberIdx idx)
{
	if (idx_value_low == idx || idx_value_hight == idx)
		return (PlayerBaseNumeric::Magic2Damage(m_Series, m_CompAttrMgr[ncai_art]) + m_RangeAttrMgr[nrai_damage_magic][idx][idx_base_value] + m_RangeAttrMgr[nrai_damage_magic][idx][idx_append_value])
			* (100 + m_RangeAttrMgr[nrai_damage_magic][idx][idx_append_percent]) / 100;
	else
		return 0;
}

int KNpc::CalcEightDiagDfns()
{
	return RandomRangeValue(CalcEightDiagDfns(idx_value_low), CalcEightDiagDfns(idx_value_hight));
}

int KNpc::CalcEightDiagDfns(enRangeMemberIdx idx)
{
	if (idx_value_low == idx || idx_value_hight == idx)
		return (PlayerBaseNumeric::Magic2EightDiagDefense(m_CompAttrMgr[ncai_art]) + m_RangeAttrMgr[nrai_defend_eightdiag][idx][idx_base_value] + m_RangeAttrMgr[nrai_defend_eightdiag][idx][idx_append_value])
			* (100 + m_RangeAttrMgr[nrai_defend_eightdiag][idx][idx_append_percent]) / 100;
	else
		return 0;
}

int KNpc::CalcDarkDfns()
{
	return RandomRangeValue(CalcDarkDfns(idx_value_low), CalcDarkDfns(idx_value_hight));
}

int KNpc::CalcDarkDfns(enRangeMemberIdx idx)
{
	if (idx_value_low == idx || idx_value_hight == idx)
		return (PlayerBaseNumeric::Magic2DarkDefense(m_CompAttrMgr[ncai_art]) + m_RangeAttrMgr[nrai_defend_dark][idx][idx_base_value] + m_RangeAttrMgr[nrai_defend_dark][idx][idx_append_value])
			* (100 + m_RangeAttrMgr[nrai_defend_dark][idx][idx_append_percent]) / 100;
	else
		return 0;
}

int KNpc::RandomRangeValue(int lowValue, int highValue)
{
	if(highValue >= lowValue)
	{	// 因为 g_Random 去尾, 无法返回 hight - low 的值，所以加1
		return lowValue + g_Random(highValue - lowValue + 1 );
	}
	else
	{
		return highValue + g_Random(lowValue - highValue + 1);
	}
}
#ifdef _SERVER
int KNpc::GetCombatScoreCalcType()
{
	int nSubWorldIndex = GetSubWorldIndex();

	if (nSubWorldIndex != INVALID_WORLD_INDEX && nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		return SubWorld[nSubWorldIndex].GetCombatScoreCalcType();
	else
		return 0;
}
#endif _SERVER