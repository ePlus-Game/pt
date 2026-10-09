#include "KCore.h"
#include "KNpcTemplate.h"
#include "CoreUtil.h"
#include "KSkills.h"

#define MAX_VALUE_LEN 300
#define NPC_EVENT_BUFF_DELIMITER "|"

/************************************************************************/
/*							DropRate Config		                        */
/************************************************************************/
#ifdef _SERVER
KItemDropRateTree g_ItemDropRateBinTree;

KItemDropRate* g_GenItemDropRate( char* szDropIniFile )
{
	if ((!szDropIniFile)|| (!szDropIniFile[0]))
		return NULL;
	
	KIniFile IniFile;
	if (IniFile.Load(szDropIniFile))
	{
		KItemDropRate * pnewDrop = new KItemDropRate;

		char szBuff[COMMON_CLIENT_MSG_LEN_32 + 1];
		IniFile.GetInteger("Main", "Count", 0, &pnewDrop->nCount);
		IniFile.GetString("Main", "MaxRandRange", "0", szBuff, COMMON_CLIENT_MSG_LEN_32 + 1 );
		pnewDrop->ulMaxRandRate = atol( szBuff );
		IniFile.GetString("Main", "MoneyRandRate", "0", szBuff, COMMON_CLIENT_MSG_LEN_32 + 1 );
		pnewDrop->ulMoneyRandRate = atol( szBuff );
		IniFile.GetString("Main", "MoneyRateMin", "0", szBuff, COMMON_CLIENT_MSG_LEN_32 + 1 );
		pnewDrop->ulMoneyMin = atol( szBuff );
		IniFile.GetString("Main", "MoneyRateMax", "0", szBuff, COMMON_CLIENT_MSG_LEN_32 + 1 );
		pnewDrop->ulMoneyMax = atol( szBuff );

		if(pnewDrop->nCount <= 0)
		{
			delete pnewDrop;
			return NULL;
		}

		pnewDrop->pItemParam = new KItemDropRate::KItemParam[pnewDrop->nCount];
		char szSection[10];
		
		KItemDropRate::KItemParam * pItemParam = pnewDrop->pItemParam;
		for(int i = 0; i < pnewDrop->nCount; i ++, pItemParam ++)
		{
			sprintf(szSection, "%d", i + 1);
			IniFile.GetInteger(szSection, "Genre", 0, &(pItemParam->nGenre));
			IniFile.GetInteger(szSection, "Detail", 0, &(pItemParam->nDetailType));
			IniFile.GetInteger(szSection, "Particular", 0, &(pItemParam->nParticulType));
			IniFile.GetInteger(szSection, "Level", 0, &(pItemParam->nLevel));
			IniFile.GetInteger(szSection, "ItemCount", 0, &(pItemParam->nItemCount));
			IniFile.GetString(szSection, "RandRate", "0", szBuff, COMMON_CLIENT_MSG_LEN_32 + 1 );
			pItemParam->ulRate = atol( szBuff );
		}
		return pnewDrop;
	}
	else
	{
		return NULL;
	}

	return NULL;
}

int	operator<(KItemDropRateNode Left, KItemDropRateNode Right)
{
	return strcmp(Left.m_szFileName, Right.m_szFileName);
};

int operator==(KItemDropRateNode Left, KItemDropRateNode Right)
{
	for ( int nDropIdx = 0; nDropIdx < MAX_DROP_GROUP; ++nDropIdx )
	{
		if ( strcmp(Left.m_szFileNameGroup[nDropIdx], Right.m_szFileNameGroup[nDropIdx]) != 0 )
		{
			return 0;
		}
	}
	return strcmp(Left.m_szFileName, Right.m_szFileName) == 0;
};

/************************************************************************/
/*							Npc Template                                */
/************************************************************************/

static char* NpcEventBuffColumn[NpcEvent_Count] = { "ReviveBuff", "DeathBuff" };

#endif

void	KNpcTemplate::InitNpcBaseData(int nNpcTemplateId)
{
	if (nNpcTemplateId < 0 ) return;
	int nNpcTempRow = nNpcTemplateId + 2;

	g_NpcSetting.GetString(nNpcTempRow, "Name", "", Name, sizeof(Name));
	g_NpcSetting.GetInteger(nNpcTempRow, "Kind", 0, (int *)&m_Kind);
	g_NpcSetting.GetInteger(nNpcTempRow, "Camp", 0, &m_Camp);
	g_NpcSetting.GetInteger(nNpcTempRow, "Series", 0, &m_Series);
	
	g_NpcSetting.GetString(nNpcTempRow, "HeadImageSet", "", m_HeadImageSet, sizeof(m_HeadImageSet));
	g_NpcSetting.GetString(nNpcTempRow, "HeadImage",	"", m_HeadImage, sizeof(m_HeadImage));
	g_NpcSetting.GetInteger(nNpcTempRow, "ClientOnly",	0, &m_bClientOnly);
	g_NpcSetting.GetInteger(nNpcTempRow, "CorpseIdx",	0, &m_CorpseSettingIdx);
// lixuewu Npc阻挡信息
	g_NpcSetting.GetInteger(nNpcTempRow, "BarrierW", 0, (int*)&m_nBarrierWidth);
	g_NpcSetting.GetInteger(nNpcTempRow, "BarrierH", 0, (int*)&m_nBarrierHeight);
// lixuewu Npc阻挡信息
	
	// add by chenshanglin on 2006-2-23 for new skill system
#ifdef _SERVER
	g_NpcSetting.GetInteger(nNpcTempRow, "DropRateAttenuation", 0, &m_nDropRateAttenuation);
	g_NpcSetting.GetInteger(nNpcTempRow,"MinLifeHold",-1,&m_nLifeLimitedHold);
	
	if (m_nLifeLimitedHold <-1 || m_nLifeLimitedHold>100)
	{
        m_nLifeLimitedHold = -1;
	}//endif

#endif
	// add end

	g_NpcSetting.GetInteger(nNpcTempRow,"CombatOrg",INVALID_COMBAT_ORG_ID,&m_CombatOrg);
	g_NpcSetting.GetInteger(nNpcTempRow,"CombatScore",0,&m_CombatScore);

	g_NpcSetting.GetInteger(nNpcTempRow, "DeathFrame",	12, &m_DeathFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "WalkFrame",	15, &m_WalkFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "RunFrame",	15, &m_RunFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "HurtFrame",	10, &m_HurtFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "WalkSpeed",	5, &m_WalkSpeed);
	g_NpcSetting.GetInteger(nNpcTempRow, "AttackSpeed",	20, &m_AttackFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "CastSpeed",	20, &m_CastFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "RunSpeed",	10, &m_RunSpeed);
	g_NpcSetting.GetInteger(nNpcTempRow, "StandFrame",	15, &m_StandFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "StandFrame1", 15, &m_StandFrame1);
	g_NpcSetting.GetInteger(nNpcTempRow, "Stature",		0,  &m_nStature);

	g_NpcSetting.GetInteger(nNpcTempRow, "AIMode",	0, &m_AiMode);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam1",	0, &m_AiParam[0]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam2",	0, &m_AiParam[1]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam3",	0, &m_AiParam[2]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam4",	0, &m_AiParam[3]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam5",	0, &m_AiParam[4]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam6",	0, &m_AiParam[5]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam7",	0, &m_AiParam[6]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam8",	0, &m_AiParam[7]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam9",	0, &m_AiParam[8]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam10",	5, &m_AiParam[9]);
#ifdef _SERVER	
	g_NpcSetting.GetInteger(nNpcTempRow, "Treasure",		0, &m_Treasure);
	g_NpcSetting.GetInteger(nNpcTempRow, "Treasure1",		0, &m_Treasure1);
	g_NpcSetting.GetInteger(nNpcTempRow, "ActiveRadius", 30, &m_ActiveRadius);
	g_NpcSetting.GetInteger(nNpcTempRow, "VisionRadius", 40, &m_VisionRadius);
	int nAIMaxTime = 0;
	g_NpcSetting.GetInteger(nNpcTempRow, "AIMaxTime", 25, (int*)&nAIMaxTime);
	m_AIMAXTime = (BYTE)nAIMaxTime;	
	g_NpcSetting.GetInteger(nNpcTempRow, "HitRecover", 0, &m_HitRecover);
	g_NpcSetting.GetInteger(nNpcTempRow, "ReviveFrame", 2400, &m_ReviveFrame);	
	char szScript[MAX_PATH];
	g_NpcSetting.GetString(nNpcTempRow, "LevelScript", "", szScript, MAX_PATH);
	if (szScript[0] == 0)
	{
		m_dwLevelSettingScript = 0;
	}
	else
	{
		g_StrLower(szScript);
		m_dwLevelSettingScript = g_FileName2Id(szScript);
	}
	g_NpcSetting.GetString(nNpcTempRow, "DeathScript", "", szScript, MAX_PATH);
	if (szScript[0] == 0)
		m_dwDeathScriptID = 0;
	else
	{
		g_StrLower(szScript);
		m_dwDeathScriptID = g_FileName2Id(szScript);
	}
	g_NpcSetting.GetString(nNpcTempRow, "ActionScript", "", szScript, MAX_PATH);
	if (szScript[0] == 0)
		m_dwActionScriptID = 0;
	else
	{
		g_StrLower(szScript);
		m_dwActionScriptID = g_FileName2Id(szScript);
	}
	g_NpcSetting.GetInteger( nNpcTempRow, "ActionTime", 0, (int*)&m_dwActionTime );
	g_NpcSetting.GetInteger( nNpcTempRow, "BreakTime", 0, (int*)&m_dwBreakTime );
    g_NpcSetting.GetInteger(nNpcTempRow, "NpcColor", 0, &m_nColor);
    g_NpcSetting.GetInteger(nNpcTempRow, "DeadlyStrikeResist", 0, &m_nDeadlyStrikeResist);
    g_NpcSetting.GetInteger(nNpcTempRow, "FatallyStrikeResist", 0, &m_nFatallyStrikeResist);
    g_NpcSetting.GetInteger(nNpcTempRow, "FreezeTimeReduce", 0, &m_nFreezeTimeReduce);
	char eventBuffIdString[128] = { 0 };
	for(int eventIndex = 0; eventIndex < NpcEvent_Count; eventIndex++)
	{
		if (NpcEventBuffColumn[eventIndex] != NULL && strlen(NpcEventBuffColumn[eventIndex]) > 0)
		{
			if (TRUE == g_NpcSetting.GetString(nNpcTempRow, NpcEventBuffColumn[eventIndex], "", eventBuffIdString, sizeof(eventBuffIdString)))
			{
				int eventBuffLoopCount = 0;
				char* pToken = strtok(eventBuffIdString, NPC_EVENT_BUFF_DELIMITER);
				while(pToken != NULL)
				{
					m_EventBuff.BuffID[eventIndex][eventBuffLoopCount++] = atoi(pToken);
					pToken = strtok(NULL, NPC_EVENT_BUFF_DELIMITER);
				}
			}
		}
	}
	g_NpcSetting.GetString(nNpcTempRow, "GUID", "", m_GUID.data, sizeof(m_GUID.data));
	m_Statistic.Init(nNpcTemplateId);

	/************************************************************************/
	/*							DropRate Config		                        */
	/************************************************************************/
	//加载npc.txt中npc所对应的掉落表
	KItemDropRateNode DropNode;

	m_pItemDropRate = NULL;
	memset( m_pItemDropGroupRate, 0, sizeof(KItemDropRate*) * MAX_DROP_GROUP );

	char szDropFile[COMMON_CLIENT_MSG_LEN_256];
	g_NpcSetting.GetString(nNpcTempRow, "DropRateFile", "", szDropFile, sizeof(szDropFile));
	strlwr(szDropFile);
	strcpy(DropNode.m_szFileName, szDropFile);
	if ( szDropFile[0] != 0 )
	{
		if (g_ItemDropRateBinTree.Find(DropNode))
		{
			m_pItemDropRate = DropNode.m_pItemDropRate;
		}
		else
		{
			DropNode.m_pItemDropRate = g_GenItemDropRate(szDropFile);
			m_pItemDropRate = DropNode.m_pItemDropRate;
			g_ItemDropRateBinTree.Insert(DropNode);
		}
	}
//*/
	//加载npc.txt中npc所对应的掉落分组表
	ConfigManager& cm = ConfigManager::Singleton();
	int nDropGroupCount = cm.GetGlobalVariable( global_var_drop_group_count ) < MAX_DROP_GROUP ? cm.GetGlobalVariable( global_var_drop_group_count ) : MAX_DROP_GROUP;
	for ( int nDropIdx = 0; nDropIdx < nDropGroupCount; ++nDropIdx )
	{
		char szDropGroupFile[COMMON_CLIENT_MSG_LEN_256];
		char szBuff[COMMON_CLIENT_MSG_LEN_256];
		sprintf( szBuff, "DropRateFile%d", nDropIdx + 1 );
		g_NpcSetting.GetString(nNpcTempRow, szBuff, "", szDropGroupFile, sizeof(szDropGroupFile));
		strlwr(szDropGroupFile);
		strcpy(DropNode.m_szFileNameGroup[nDropIdx], szDropGroupFile);
		if ( szDropGroupFile[0] != 0 )
		{
			if (g_ItemDropRateBinTree.Find(DropNode))
			{
				m_pItemDropGroupRate[nDropIdx] = DropNode.m_pItemDropRate;
			}
			else
			{
				DropNode.m_pItemDropRate = g_GenItemDropRate(szDropGroupFile);
				m_pItemDropGroupRate[nDropIdx] = DropNode.m_pItemDropRate;
				g_ItemDropRateBinTree.Insert(DropNode);
			}
		}

	}//*/	
#else
	char szBuff[64] = { 0 };
	float nV[4] = { 0 };

	g_NpcSetting.GetString(nNpcTempRow,"LifeParam","100,0,0,1000000", szBuff, sizeof(szBuff) );
	sscanf( szBuff, "%f,%f,%f,%f", &nV[0], &nV[1], &nV[2], &nV[3] );
	m_nLifeMultiple = nV[0] / 100.0f;
	g_NpcSetting.GetInteger( nNpcTempRow, "LifeBarType", 0, &m_nLifeBarStyle );
	int nValue;
	g_NpcSetting.GetInteger( nNpcTempRow, "ShowLifePercent", 0, &nValue );
	m_bShowLifePercent = ( nValue != FALSE );
	g_NpcSetting.GetInteger(nNpcTempRow, "ArmorType", 0, &m_ArmorType);
	g_NpcSetting.GetInteger(nNpcTempRow, "HelmType", 0, &m_HelmType);
	g_NpcSetting.GetInteger(nNpcTempRow, "WeaponType", 0, &m_WeaponType);
	g_NpcSetting.GetInteger(nNpcTempRow, "ShoulderType" , 0, &m_ShoulderType);
	g_NpcSetting.GetInteger(nNpcTempRow, "BootType", 0, &m_BootType);
	g_NpcSetting.GetInteger(nNpcTempRow, "CuffType", 0, &m_CuffType);
	g_NpcSetting.GetInteger(nNpcTempRow, "HorseType", -1, &m_HorseType);
	g_NpcSetting.GetInteger(nNpcTempRow, "RideHorse",0, &m_bRideHorse);
	g_NpcSetting.GetString(nNpcTempRow, "LevelScript", "", m_szLevelSettingScript, 100);
	g_NpcSetting.GetString(nNpcTempRow, "NameColor", "", m_szNpcNameColor,sizeof(m_szNpcNameColor));
	g_NpcSetting.GetString(nNpcTempRow, "Blood", "", m_szNpcBloodInfo,sizeof(m_szNpcBloodInfo));
	int i = 0;
	for ( i = 0; i < 6; i++ )
	{
		char mapID[COMMON_CLIENT_MSG_LEN_16] = "MapID%d";
		char key[COMMON_CLIENT_MSG_LEN_16] = "";
		sprintf(key, mapID, i);

		g_NpcSetting.GetInteger( nNpcTempRow, key, 0, &m_nMapID[i] );

		char mapPos[COMMON_CLIENT_MSG_LEN_16] = "MapPos%d";
		sprintf( key, mapPos, i);
		g_NpcSetting.GetString( nNpcTempRow, key, "", m_nMapPos[i], COMMON_CLIENT_MSG_LEN_64 );
	}
	g_NpcSetting.GetInteger( nNpcTempRow, "DisplayID", 0, &m_nDisplayID);

	g_NpcSetting.GetString( nNpcTempRow, "Desc", "", m_nDes, COMMON_CLIENT_MSG_LEN_16 );
	BOOL bDisplayerSelect = TRUE;
	g_NpcSetting.GetInteger( nNpcTempRow, "DisplaySelect", TRUE, &bDisplayerSelect );
	m_bDisplaySelect = (bDisplayerSelect == TRUE ? true : false);

	BOOL bShowTargetFace = TRUE;
	g_NpcSetting.GetInteger( nNpcTempRow, "ShowTargetFace", TRUE, &bShowTargetFace );
	m_bShowTargetFace = (bShowTargetFace == TRUE ? true : false);
#endif

	
}

int	KNpcTemplate::LoadLevelData(int nRow, char *szCol, KLuaScript *pLevelScript, char *szKey, int nLevel)
{
	const int	ATTR_PARM_NUM = 4;
	int			aAttrParm[ATTR_PARM_NUM];
	const char	*szDelimiter = ",";
	char		szValue[MAX_VALUE_LEN];

	memset(&aAttrParm, 0, sizeof(aAttrParm));
	g_NpcSetting.GetString(nRow, szCol, "", szValue, MAX_VALUE_LEN);
	StrToIntArray(szValue, szDelimiter, aAttrParm, ATTR_PARM_NUM);
	
	return aAttrParm[0] * GetNpcLevelDataFromScript(pLevelScript, szKey, nLevel, 
						aAttrParm[1], aAttrParm[2], aAttrParm[3]) / 100;	
}

void KNpcTemplate::InitNpcLevelData(KTabFile * pKindFile, int nNpcTemplateId, KLuaScript * pLevelScript, int nLevel)
{
	if (nNpcTemplateId < 0 || nLevel <= 0 || (!pLevelScript)) return;
	int nNpcTempRow = nNpcTemplateId + 2;
	int	 nTopIndex = 0;
	m_nLevel = nLevel;

	const char	*szDelimiter = ",";

	pLevelScript->SafeCallBegin(&nTopIndex);
	{
		m_NpcSettingIdx = nNpcTemplateId;
#ifdef _SERVER
		//技能
		char szValue1[MAX_VALUE_LEN];
		char npcSkillColumnName[16];
		int skillStrategy[3];	

		m_SkillStrategyCount = 0;
		memset(m_SkillStrategyList, 0, sizeof(m_SkillStrategyList));

		for(int i = 0; i < MAX_SKILL_STRATEGY_LIST_LENGTH; ++i)
		{
			memset(skillStrategy, 0, sizeof(skillStrategy));
			sprintf(npcSkillColumnName, "Skill%d", i + 1);			

			if (TRUE == g_NpcSetting.GetString(nNpcTempRow, npcSkillColumnName,	"", szValue1, MAX_VALUE_LEN))
			{
				StrToIntArray(szValue1, szDelimiter, skillStrategy, 3);
				
				m_SkillList.AddSkillEx(skillStrategy[0], 1, skill_status_usable);
				m_SkillStrategyList[m_SkillStrategyCount].SkillId = skillStrategy[0];
				m_SkillStrategyList[m_SkillStrategyCount].Weight = skillStrategy[1];
				m_SkillStrategyList[m_SkillStrategyCount].Mode = (enumSkillStrategyMode)skillStrategy[2];
				m_SkillStrategyCount++;
			}
		}

		m_Experience		= LoadLevelData(nNpcTempRow, "ExpParam", pLevelScript, "Exp", nLevel);
		m_SkillExp			= LoadLevelData(nNpcTempRow, "SkillExp", pLevelScript, "SkillExp", nLevel);
		m_LifeMax			= LoadLevelData(nNpcTempRow, "LifeParam", pLevelScript, "Life", nLevel);
		m_AttackRating		= LoadLevelData(nNpcTempRow, "ARParam", pLevelScript, "AR", nLevel);
		m_Defend			= LoadLevelData(nNpcTempRow, "DefenseParam", pLevelScript, "Defense", nLevel);

		m_NearPhysDamLow	= LoadLevelData(nNpcTempRow, "MinMeleeDamage", pLevelScript, "MinMeleeDamage", nLevel);
		m_NearPhysDamHight	= LoadLevelData(nNpcTempRow, "MaxMeleeDamage", pLevelScript, "MaxMeleeDamage", nLevel);
		m_FarPhysDamLow		= LoadLevelData(nNpcTempRow, "MinRangeDamage", pLevelScript, "MinRangeDamage", nLevel);
		m_FarPhysDamHight	= LoadLevelData(nNpcTempRow, "MaxRangeDamage", pLevelScript, "MaxRangeDamage", nLevel);
		m_FireDamLow		= LoadLevelData(nNpcTempRow, "MinFireDamage", pLevelScript, "MinFireDamage", nLevel);
		m_FireDamHight		= LoadLevelData(nNpcTempRow, "MaxFireDamage", pLevelScript, "MaxFireDamage", nLevel);
		m_WaterDamLow		= LoadLevelData(nNpcTempRow, "MinColdDamage", pLevelScript, "MinColdDamage", nLevel);
		m_WaterDamHight		= LoadLevelData(nNpcTempRow, "MaxColdDamage", pLevelScript, "MaxColdDamage", nLevel);
		m_WindDamLow		= LoadLevelData(nNpcTempRow, "MinWindDamage", pLevelScript, "MinWindDamage", nLevel);
		m_WindDamHight		= LoadLevelData(nNpcTempRow, "MaxWindDamage", pLevelScript, "MaxWindDamage", nLevel);
		m_ThunderDamLow		= LoadLevelData(nNpcTempRow, "MinLightingDamage", pLevelScript, "MinLightingDamage", nLevel);
		m_ThunderDamHight	= LoadLevelData(nNpcTempRow, "MaxLightingDamage", pLevelScript, "MaxLightingDamage", nLevel);
		m_ShadowDamLow		= LoadLevelData(nNpcTempRow, "MinBlackDamage", pLevelScript, "MinBlackDamage", nLevel);
		m_ShadowDamHight	= LoadLevelData(nNpcTempRow, "MaxBlackDamage", pLevelScript, "MaxBlackDamage", nLevel);
		m_PoisonDamLow		= LoadLevelData(nNpcTempRow, "MinPoisonDamage", pLevelScript, "MinPoisonDamage", nLevel);
		m_PoisonDamHight	= LoadLevelData(nNpcTempRow, "MaxPoisonDamage", pLevelScript, "MaxPoisonDamage", nLevel);
	
		m_RedLum = 0;
		m_GreenLum = 0;
		m_BlueLum = 0;

		g_NpcSetting.GetString(nNpcTempRow, "LifeReplenish", "0|0", szValue1, MAX_VALUE_LEN);
		m_LifeReplenish = GetNpcLevelDataFromScript(pLevelScript, "LifeReplenish", nLevel, szValue1);

		g_NpcSetting.GetString(nNpcTempRow, "RangeResist", "0|0", szValue1, MAX_VALUE_LEN);
		m_FarPhysResistLow = GetNpcLevelDataFromScript(pLevelScript, "RangeResist", nLevel, szValue1);
		m_FarPhysResistHight = m_FarPhysResistLow;

		g_NpcSetting.GetString(nNpcTempRow, "MeleeResist", "0|0", szValue1, MAX_VALUE_LEN);
		m_NearPhysResistLow = GetNpcLevelDataFromScript(pLevelScript, "MeleeResist", nLevel, szValue1);
		m_NearPhysResistHight = m_NearPhysResistLow;

		g_NpcSetting.GetString(nNpcTempRow, "ColdResist", "0|0", szValue1, MAX_VALUE_LEN);
		m_WaterResistLow = GetNpcLevelDataFromScript(pLevelScript, "ColdResist", nLevel, szValue1);
		m_WaterResistHight = m_WaterResistLow;

		g_NpcSetting.GetString(nNpcTempRow, "FireResist", "0|0", szValue1, MAX_VALUE_LEN);
		m_FireResistLow = GetNpcLevelDataFromScript(pLevelScript, "FireResist", nLevel, szValue1);
		m_FireResistHight = m_FireResistLow;

		g_NpcSetting.GetString(nNpcTempRow, "LightResist", "0|0", szValue1, MAX_VALUE_LEN);
		m_ThunderResistLow = GetNpcLevelDataFromScript(pLevelScript, "LightResist", nLevel, szValue1);
		m_ThunderResistHight = m_ThunderResistLow;

		g_NpcSetting.GetString(nNpcTempRow, "WindResist", "0|0", szValue1, MAX_VALUE_LEN);
		m_WindResistLow = GetNpcLevelDataFromScript(pLevelScript, "WindResist", nLevel, szValue1);
		m_WindResistHight = m_WindResistLow;

		g_NpcSetting.GetString(nNpcTempRow, "BlackResist", "0|0", szValue1, MAX_VALUE_LEN);
		m_ShadowResistLow = GetNpcLevelDataFromScript(pLevelScript, "BlackResist", nLevel, szValue1);
		m_ShadowResistHight = m_ShadowResistLow;

		g_NpcSetting.GetString(nNpcTempRow, "PoisonResist", "0|0", szValue1, MAX_VALUE_LEN);
		m_PoisonResistLow = GetNpcLevelDataFromScript(pLevelScript, "PoisonResist", nLevel, szValue1);
		m_PoisonResistHight = m_PoisonResistLow;
#else		
		int aSkillId[TMPL_NPC_SKILL_NUM];
		if (m_Kind == kind_siege_weapon)
		{
			char szValue1[MAX_VALUE_LEN];

			g_NpcSetting.GetString(nNpcTempRow, "Skill",	"", szValue1, MAX_VALUE_LEN);
			memset(&aSkillId, 0, sizeof(aSkillId));
			StrToIntArray(szValue1, szDelimiter, aSkillId, TMPL_NPC_SKILL_NUM);

			for(int i = 0; i < TMPL_NPC_SKILL_NUM; ++i)
			{
				m_AttackRadius[i] = 60;

				if(0 != aSkillId[i])
				{
					KSkill *pSkill = g_SkillManager.GetSkill(aSkillId[i], 1);

					if(pSkill)
						m_AttackRadius[i] = pSkill->GetAttackRadius();
				}
			}
		}		
				m_LifeMax = LoadLevelData(nNpcTempRow, "LifeParam", pLevelScript, "Life", nLevel);

		if (m_LifeMax == 0)
			m_LifeMax = 100;
#endif
	}
	pLevelScript->SafeCallEnd(nTopIndex);
}

int KNpcTemplate::GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, char * szParam) const
{
	int nTopIndex = 0;
	int nReturn = 0;
	if (szParam == NULL|| szParam[0] == 0 || strlen(szParam) < 3)
	{
		CFS_FILELOGS::WriteDebugLog("Npc[%s] Param[%s] error: Call GetNpcLevelData()\n", this->Name, szDataName);
		return 0;
	}

	pScript->SafeCallBegin(&nTopIndex);
	pScript->CallFunction("GetNpcLevelData", 1, "dss", nLevel, szDataName, szParam);
	nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
	nReturn = (int) Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);
	pScript->SafeCallEnd(nTopIndex);
	return nReturn;
}

int KNpcTemplate::ST_GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, char * szParam) 
{
	int nTopIndex = 0;
	int nReturn = 0;
	if (szParam == NULL|| szParam[0] == 0 || strlen(szParam) < 3)
	{
		CFS_FILELOGS::WriteDebugLog("Param[%s] error: Call GetNpcLevelData()\n", szDataName);
		return 0;
	}

	pScript->SafeCallBegin(&nTopIndex);
	pScript->CallFunction("GetNpcLevelData", 1, "dss", nLevel, szDataName, szParam);
	nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
	nReturn = (int) Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);
	pScript->SafeCallEnd(nTopIndex);
	return nReturn;
}

int KNpcTemplate::GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nLevel, double nParam1, double nParam2, double nParam3) const
{
	int nTopIndex = 0;
	int nReturn = 0;
	pScript->SafeCallBegin(&nTopIndex);
	pScript->CallFunction("GetNpcKeyData", 1, "dsnnn", nLevel, szDataName, nParam1, nParam2, nParam3);
	nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
	nReturn = (int) Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);
	pScript->SafeCallEnd(nTopIndex);
	return nReturn;
}


int KNpcTemplate::SkillString2Id(char * szSkillString)
{
	if (!szSkillString[0]) return 0;
	int nSkillNum = g_OrdinSkillsSetting.GetHeight() - 1;
	char szSkillName[100];
	for (int i = 0 ;  i < nSkillNum; i ++)
	{
		g_OrdinSkillsSetting.GetString(i + 2, "SkillName", "", szSkillName, sizeof(szSkillName));
		if (g_StrCmp(szSkillString, szSkillName))
		{
			int nSkillId = 0;
			g_OrdinSkillsSetting.GetInteger(i + 2, "SkillId", 0, &nSkillId);
			return nSkillId;
		}
	}
	return 0;
}

