//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-22
//      File_base        : ConfigManager
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 配置管理器
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "CoreUtil.h"
#include "ConfigManager.h"

//可配置颜色
static const char* ConfigurableColorSectionName = "ConfigurableColor";
static const char* ConfigurableColorNames[color_count] =
{
	"PkNormal",
	"PkAlert",
	"PkPunish",
	"PkDemon",
	"Killer",
	"PkEncourage",
	"PkDiablo",
};

static const char* IBSectionName = "IBInfo";
static const char* IBReturnName = "ReturnId";
static const char* IBTicketName = "TicketId";

//全局变量
static const char* GlobalVariableSectionName = "GlobalVariables";

static const char* IBGlobalVariableNames[ib_global_var_count] =
{
	"CreditLevelLimit",
	"CreditPointDefault",
	"CreditDateDefault",
	"CreditDayDefault",
	"CreditRateJinshanBi",
	"RewardPointRate",
	"RewardPointMax",
	"JinshanbiExchangeLimit",
	"CreditpointExchangeLimit",
	"PointExchangeLimit",
	"CreditToTicketRate",
};

static const char* GlobalVariableNames[global_var_count] =
{
	"ObjectItemLifeTime",
	"ObjectItemBelongTime",
	"MaxShareExpRange",
	"DamageActionDelay",
	"PlayerLogoutTime",
	"MinPlayerSaveInterval",
	"MaxPlayerSaveInterval",
	"PlayerSaveIntervalSeconds",
	"PlayerSaveSetpInterval",
	"PlayerSaveSetpCount",
	"TradeSaveMinMoney",
	"AntiEnthrallWearinessTime",
	"AntiEnthrallInsalubrityTime",
	"AntiEnthrallClearStateTime",
	"LocalRoomChatTimeInterval",
	"GlobalRoomChatTimeInterval",
	"MapRoomChatTimeInterval",
	"InviteJointeamTimeout",
	"ExpPercentage",
	"QuestExpPercentage",
	"SkillExpPercentage",
	"ItemBankOn",
	"IBShopShopReqTimeInterval",
	"IBShopShelfReqTimeInterval",
	"IBShopPanelReqTimeInterval",
	"IBShopStyleReqTimeInterval",
	"RequestTeamListInterval",
	"CheckMailInterval",
	"SendCombatTop10Interval",
	"SendCombatTop10Switch",
	"SortCombatTop10Interval",
	"ClearChatLogInterval",
	"KeepChatLogInterval",
	"TimezoneCorrectHour",
	"RandomSelectTitleInterval",
	"EmployeeUnexpectedDamage",
	"CancelItemExpBuff",

	"AiNpcFollowMaxRange",
	"AiNpcFollowMinRange",
	"NextAiDelay",
	"OverThreatFactor",
	"NpcReturnTime",
	"BaseNextWanderChangeTime",
	"MaxNextWanderChangeTime",
	"ForceInstantReturnDelay",
	"NpcWanderRange",
	"MinNpcFollowRange",
	"ValidEnemyDistance",
	"ClearTargetDistance",
	"AutoSelectNpcDistance",
	"AiPlayerFollowAttackRangeReduce",

	"BuffOverweight",
	"BuffLevelUp",
	"BuffPlayerRevive",
	"BuffPlayerDeath",
	"BuffKiller",
	"BuffNpcReturn",
	"BuffCityWithdraw",
	"BuffCityContribute",
	"BuffCityProduce",
	"BuffCityConsume",
	"BuffGM",
	"BuffEmploy",
		
	"SkillLogicVersion",
    "CommonCoolDownTime1",
	"CommonCoolDownTime2",
	"CommonCoolDownTime3",
	"CommonCoolDownTime4",
	"CommonCoolDownTime5",
	"CommonCoolDownTime6",	

	"PkValueAddBase",
	"PkValueAddFactor",
	"PkValueLevelDiff",
	"PkPunishReviveMap",
	"PkPunishRevivePos",
	"DefaultReviveMap",
	"DefaultRevivePos",
	"DeathDropEquipNormal",
	"DeathDropBagNormal",
	"DeathAbradeEquipNormal",
	"DeathDropEquipAlert",
	"DeathDropBagAlert",
	"DeathAbradeEquipAlert",
	"DeathDropEquipPunish",
	"DeathDropBagPunish",
	"DeathAbradeEquipPunish",
	"PkBoundaryNormalToAlert",
	"PkBoundaryAlertToPunish",
	"PkBoundaryPunishToDemon",
	"PkProtectLevel",
	"PkEncourageLevel",

	"LogDeviceDb",
	"LogDeviceDebug",
	"LogDeviceTextfile",
	"LogItemLogLevel",
	"LogNpcDropMoneyAmount",
	"LogPickupMoneyAmount",
	"LogTradeMoneyAmount",
	"LogAddExpAmount",
	"LogAddMoneyAmount",
	"LogPlayerEnterWorld",
	"LogPlayerLeaveWorld",
	"LogLoadNpc",
	"LogSaveNpc",
	"LogDeleteNpc",
	"LogPlayerStatistic",
	"LogPlayerStatisticAutoSavePeriod",
	"LogNpcStatistic",
	"LogNpcStatisticAutoSavePeriod",
	"LogSocialUnitCreate",
	"LogSocialUnitDelete",
	"LogInstanceCreate",
	"LogChat",
	"LogCombatScoreAdd",
	"LogCombatScoreDec",
	"LogEmployTimeChangeThreshold",
	"LogEmploy",
	"LogRecommender",
	"LogInsuranceMoneyGotAmount",
	"LogInsuranceFetchMoney",
	"LogTotalMoneyStatisticAutoSaveInterval",

	"RoleheadNormalInfoCount",
	"RoleheadHoverInfoCount",


	"NpcLevelDisplayerValue0",
	"NpcLevelDisplayerValue1",
	"NpcLevelDisplayerValue2",
	"NpcLevelDisplayerValue3",

	"NpcLevelDifferent",
	"NpcLevelHitValue1",
	"NpcLevelHitValue2",
	"NpcLevelHurtValue1",
	"NpcLevelHurtValue2",

	"TalismanPotentialGainRate",
	"TalismanPotentialConvertRate",

	"DropGroupCount",
	
	"FuryTimeInterval",
	"FuryKillNum",
	"FuryKeepTime",
	"FurySkillId",
	"FuryFullBuffId",
	"FuryEndBuffId",

	"WarStartTime",
	"WarProcessTime",
	"BossTemplateID",
	"BossLevel",
	"BossNum",
	"DeathPercent",
	"LordScapegoatBuffID",
	"RobResPercentage",
	"ProtectBuffID",
	"InvincibilityBuffID",
	"RobberTemplateID",
	"TongWarCommanderSyncInterval",
	"TongWarCommanderSyncSwitch",
	"TongWarChangeCityBuff",
	"TongWarEconomySwitch",
	"CityResourceSwitch",

	"PoolCombatItemClass",
	"PoolCombatItemDetailType",
	"PoolCombatItemParticualrType",
	"PoolCombatItemLevel",
	"PoolCombatStartDecTime",
    "PoolCombatEndDecTime",
	"PoolCombatProcessTime",
	"PoolBossTemplateID",
	"PoolBossLevel",
	"PoolBossOffsetX",
	"PoolBossOffsetY",
	"PoolSublordFightAlertPercent",
	"PoolSublordFightTime",
	"PoolCombatProtectBuff",
	"PoolCombatSucessBuff",
	
	"ItemInlayOkNormal",
    "ItemInlayOkYin",
	"ItemInlayOkYang",
	"ItemInlayMaxSocketCount",
	
	"SocialRecruitExistTime",
	"SocialRecruitSucessBuffID",
	"SpecialBuff",

	"SendMailTextTax",
	"SendMailItemTax",
	"MaxMailPerPlayer",

	"AuctionShortTime",
	"AuctionMiddleTime",
	"AuctionLongTime",
	"AuctionShortTimeTax",
	"AuctionMiddleTimeTax",
	"AuctionLongTimeTax",
	"AuctionTaxBidPercent",
	"AuctionSearchInterval",

	"ItemBoxInitSize",

	"InlayItemNpcEffect",
	"InlayItemNpcEffect3",
	"InlayItemNpcEffect6",
	"InlayItemNpcEffect9",
   
	"WorldCombatShareRate",
	"WorldCombatInterval",

	"RecommenderMasterRequireLevelMin",
	"RecommenderMasterRequireLevelMax",
	"RecommenderStudentRequireLevelMin",
	"RecommenderStudentRequireLevelMax",
	"RecommenderExpireDay",
	"RecommenderMaxStudentCount",
	"RecommenderRewardTicketRatio",

	"PlusPointAmount",

	"ItemLockByDateItemDetailType",

	"ActiveDegreeFactor1",
	"ActiveDegreeFactor2",
	"ReCalcPopularityInterval",
	"UpdatePopularityInterval",
	"RefreshShizuPopularityInterval",
	"RefreshZhuhouPopularityInterval",

	"RefreshCombatKillRankTimeWeekday",
	"RefreshCombatKillRankTimeHour",
	"RefreshCombatKillRankTimeMinute",
	"RefreshCombatKillRankTimeSecond",

	"PkBoundaryDemonToDiablo",
	"DeathDropEquipDemon",
	"DeathDropBagDemon",
	"DeathAbradeEquipDemon",
	"InsteadSpecieIndex",
	"NewConsumePointIndex",
	"NewConsumePointRate",
};


#ifdef _SERVER

//可配置BUFF
static const char* ConfigurableBuffSectionName = "ConfigurableBuff";
static const char* ConfigurableBuffNames[buff_Count] = 
{
	"Overweight",
	"LevelUp",
	"PlayerRevive",
	"PlayerDeath",
	"Killer",
	"NpcReturn",
	"CityWithdraw",
	"CityContribute",
	"CityProduce",
	"CityConsume",
};

//上线BUFF
static const char* OnlineBuffSectionName = "OnlineBuff";
static const char* OnlineBuffCountName = "BuffCount";
static const char* OnlineBuffName = "Buff_%d";

//技能附加BUFF
static const char* SkillAdditionalBuffSectionName = "SkillAdditionalBuff";
static const char* SkillAdditionalBuffName = "SkillAdditionalBuff_%d_%d_%d";
static const char* CommonSkillAdditionalBuffName = "CommonSkillAdditionalBuff_%d";

#else

static const char* ConfigurableDisplayStyleSectionName = "ConfigurableDisplayStyle";
static const char* ConfigurableDisplayStyleNames[style_count] =
{	
	"NewLineObj_%d",
	"NewLineSeg_%d",
	"ItemName_%d",
	"ItemName_Yao_%d",
	"ItemName_Name_%d",
	"ItemName_YaoLevel_%d",
	"ItemName_EnchaseLevel_%d",
	"ItemImage_%d",
	"ItemDesc_%d",
	"ItemDesc_Basic_%d",
	"ItemDesc_PlusInfo_%d",	
	"ItemDesc_Charm_%d",
	"ItemEquipPos_%d",
	"ItemBasicProperty_%d",
	"ItemBasicProperty_Buff_%d",
	"ItemYaoAddOn_%d",
	"ItemYaoAddOn_Buff_%d",
	"ItemUpgrade_%d",
	"ItemAddMagic_%d",
	"ItemDuration_%d",
	"ItemWeight_%d",
	"ItemRequireLevel_%d",
	"ItemRequireProfession_%d",
	"ItemRequireProperty_%d",
	
	"ItemPrice_%d",
	"ItemCreditProperty_%d",
	
	"ArmorsetName_%d",	
	"ArmorsetPart_%d",
	"ArmorsetPart_Individal_%d",
	"ArmorsetEffect_%d",

	"GuaName_%d",
	"GuaLevel_%d",
	"GuaDesc_%d",
	"GuaActiveItem_%d",
	"GuaStandalone_%d",
	"GuaMutiple_%d",
	"GuaMutipleRow_%d",
	
	"GuaSetName_%d",
	"GuaSetComment_%d",
	"GuaSetEffect_%d",
	"GuaSetDesc_%d",
	
	"GuaLevelText_%d",
	"TalismanLevel_%d",
	"TalismanTopLevel_%d",
	"TalismanQuality_%d",
	"TalismanPotential_%d",
	"TalismanEnchase_%d",
	"TalismanEnchaseName_%d",
	"TalismanEnchaseDesc_%d",
	"TalismanEnchaseBuff_%d",
	"TalismanEnchaseUnused_%d",
	"TalismanEnchaseUnenabled_%d",
	"TalismanEnchaseRequire_%d",
	"TalismanEnchaseRequireGroup_%d",
	"EnchaseItemLevel_%d",

	"Money_image_%d",
	"Money_Text_Color_%d",
	"Money_Font_%d",
	"Money_Text_%d",
	"EquipCompare_Text_%d",
	"EquipCompare_Font_%d",
	"EquipCompare_Color_%d",
	"TipMargin_%d",	
	"ItemTipValidDate_%d",
	"ItemTipOutDateText_%d",

	"ItemInlayBase_%d",	
	"ItemInlayGraphEmpty_%d",	
	"ItemInlayGraphFull_%d",
	"ItemInlayGraphFullSpecial_%d",
	"ItemInlayBaseAttribute_%d",
	"ItemInlayYaoAttribute_%d",
	"ItemInlaySpecialAttribute_%d",
	"ItemInlayStoneName_%d",

	"ItemInlayStonePos_%d",
	"ItemInlayStoneQuality_%d",
	"ItemInlayStoneGroup_%d",
	"ItemInlayStoneRate_%d",

	"ItemCanNotDiscard_%d",
	"ItemCanNotSell_%d",
	"ItemCanNotExchange_%d",
	"ItemBind_%d",
	"ItemCanNotDeathDrop_%d",
	"ItemUnique_%d",
	"ItemEquipBind_%d",

	"RoleHeadInfoBegin_%d",
	"RoleHeadInfoEnd_%d",
	"RoleHeadNormalInfo_%d",
	"RoleHeadHoverInfo_%d",
	"RoleHeadImageInfo_%d",

	"ItemRestrictCount_%d",
	"ItemRestrictLocked_%d",

	"SkillTipHead_%d",
	"SkillTipName_%d",
	"SkillTipLevel_%d",
	"SkillTipDis_%d",
	"SkillTipCos_%d",
	"SkillTipCoolTime_%d",
	"SkillTipDescV_%d",
	"SkillTipLine_%d",
	"SkillTipHaventStudy_%d",
	"SkillTipNextLevelNeed_%d",
    "SkillTipCostNormalString_%d",
	"SkillTipNormalTimeString_%d",
	"SkillTipNormalMoneyString_%d",
	"SkillTipEnd_%d",

	"SkillCondHead_%d",
	"SkillCondName_%d",
	"SkillCondSkillExp_%d",
	"SkillCondMoney_%d",
	"SkillCondItem_%d",
	"SkillCondEnd_%d",

	"QueryInfoHead_%d",
	
	"RoleMetier_%d",
	"RoleSeriesJS_%d",
	"RoleSeriesDS_%d",
	"RoleSeriesYR_%d",

	"RoleInfoBase_%d",
	"RoleInfoSkillTitle_%d",
	"RoleInfoSkill_%d",
	"RoleInfoItemTitle_%d",
	"RoleInfoItem_%d",
	"RoleInfoQuestTitle_%d",
	"RoleInfoQuest_%d",
	"RoleInfoMapTitle_%d",
	"RoleInfoMap_%d",

	"ItemFormatInfoTitle_%d",
	"ItemFormatInfo_%d",

	"SkillFormatInfoTitle_%d",
	"SkillFormatInfo_%d",

	"QuestFormatInfoTitle_%d",
	"QuestFormatInfo_%d",

	"QuestInfoBeginTitle_%d",
	"QuestRequest_%d",
	"QuestInfoBeginNpc_%d",
	"QuestInfoNeedItemTitle_%d",
	"QuestInfoNeedItem_%d",
	"QuestInfoNeedNpcTitle_%d",
	"QuestInfoNeedNpc_%d",
	"QuestInfoDialogNpcTitle_%d",
	"QuestInfoDialogNpc_%d",
	"QuestInfoEndNpcTitle_%d",
	"QuestInfoEndNpc_%d",
	"QuestInfoAwardMoney_%d",
	"QuestInfoAwardExp_%d",
	"QuestInfoAwardChoiceItemTitle_%d",
	"QuestInfoAwardChoiceItem_%d",
	"QuestInfoAwardItemTitle_%d",
	"QuestInfoAwardItem_%d",
	
	"NpcFormatInfoTitle_%d",
	"NpcFormatInfo_%d",

	"NpcType_%d",
	"NpcInfoImage_%d",
	"NpcInfoName_%d",
	"NpcInfoBase_%d",
	"NpcInfoMapTitle_%d",
	"NpcInfoMap_%d",
	"NpcInfoDropItemTitle_%d",
	"NpcInfoDropItem_%d",

	"MapQuestTitle_%d",
	"MapQuest_%d",

	"QueryInfoEnd_%d",

	"SyncToWorldNpcImage_%d",

    "MiniMapIcon_%d",
	"SpecialColor_%d",
	"CombatColor_%d",
	"CombatMinimap_%d",

	"ItemMapPos_%d",
	"ItemCapability_%d",

	"TextFilter_%d",
	"TopMessageDefault_%d",
	"NpcNamePlus_%d",

	"ItemSafeLock_%d",

	"CombatMapOrgImage_%d",
	"WarCommanderImage_%d",
	"ItemLevelupToolDesc_%d",
	"ItemFlushTimeDesc_%d",
	"PlusPointLimitTxt_%d",
};

#endif

ConfigManager::ConfigManager()
{
	CleanUp();
}

ConfigManager::~ConfigManager()
{
}

ConfigManager& ConfigManager::Singleton()
{
	static ConfigManager config;
	return config;
}

#ifdef _SERVER

bool ConfigManager::LoadPKValueSettings()
{
	bool ret = false;
	KTabFile pkValueSettingsFile;
	if (pkValueSettingsFile.Load(PK_ADDED_VALUE_CFG_FILE) == TRUE)
	{
		int maxPKValue = 0;
		int addedPKValue = 0;
		int usedNum = 0;
		for (int i = 0; i < pkValueSettingsFile.GetHeight() && i < MAX_PK_ADDED_VALUE_NUM; ++i)
		{
			pkValueSettingsFile.GetInteger(i + 2, 1, 0, &maxPKValue);
			pkValueSettingsFile.GetInteger(i + 2, 2, 0, &addedPKValue);

			for (; usedNum < maxPKValue && usedNum < MAX_PK_ARRAY_VALUE; ++usedNum)
			{
				m_PKAddedValue[usedNum] = addedPKValue;
			}
		}
		ret = true;
	}
	else
	{
		ret = false;
	}
	return ret;
}

#endif

bool ConfigManager::Load()
{
	CleanUp();
	bool success = true;	

	KIniFile configIniFile;
	KIniFile styleconfigIniFile;
#ifdef _SERVER
	if (TRUE == configIniFile.Load(COMMON_CONFIG_FILE))
#else	
	if (TRUE == configIniFile.Load(COMMON_CONFIG_FILE) && TRUE == styleconfigIniFile.Load(COMMON_STYLE_CONFIG_FILE))
#endif
	{
		char color[COMMON_STRING_LENGTH] = { 0 };
		for(int configurableColor = 0; configurableColor < color_count; configurableColor++)
		{
			BOOL readResult = configIniFile.GetString(ConfigurableColorSectionName, ConfigurableColorNames[configurableColor], "", color, sizeof(color));
			color[COMMON_STRING_LENGTH - 1] = 0;
			if (TRUE == readResult)
			{
				m_Color[configurableColor] = StringToColor(color);
			}
		}

		
		for (int globalVarLoopCount = 0; globalVarLoopCount < global_var_count; globalVarLoopCount++)
		{
			configIniFile.GetInteger(GlobalVariableSectionName, GlobalVariableNames[globalVarLoopCount], 0, &m_GlobalVariable[globalVarLoopCount]);
		}

		configIniFile.GetString(IBSectionName, IBTicketName, "", m_IBTicketId, sizeof(m_IBTicketId));
		configIniFile.GetString(IBSectionName, IBReturnName, "", m_IBReturnId, sizeof(m_IBReturnId));
		for (int ibglobalVarLoopCount = 0; ibglobalVarLoopCount < ib_global_var_count; ibglobalVarLoopCount++)
		{
			configIniFile.GetInteger(IBSectionName, IBGlobalVariableNames[ibglobalVarLoopCount], 0, &m_IBGlobalVariable[ibglobalVarLoopCount]);
		}

		for (int nPrivateState = 0;nPrivateState < MAX_PRIVATE_STATE_NUM ; nPrivateState ++ )
		{
			char szKeyName[32] = "";
			sprintf(szKeyName,"%d",nPrivateState);

			m_PrivateStateName[nPrivateState][0] = 0;
			BOOL bRes = configIniFile.GetString("PrivateStateName",szKeyName," ",m_PrivateStateName[nPrivateState],MAXSIZE_ROLENAME);
			
			if (!bRes)
			{
				sprintf(m_PrivateStateName[nPrivateState],"-_-");   //Default String Name
			}//endif

			m_PrivateStateName[nPrivateState][MAXSIZE_ROLENAME - 1] = 0;
		}//end for nPrivateState

#ifdef _SERVER

		configIniFile.GetInteger(OnlineBuffSectionName, OnlineBuffCountName, 0, &m_OnlineBuffCount);
		char onlineBuff[COMMON_STRING_LENGTH] = { 0 };		
		for(int onlineBuffIndex = 0; onlineBuffIndex < m_OnlineBuffCount; onlineBuffIndex++)
		{
			sprintf(onlineBuff, OnlineBuffName, onlineBuffIndex + 1);
			onlineBuff[COMMON_STRING_LENGTH - 1] = 0;
			configIniFile.GetInteger(OnlineBuffSectionName, onlineBuff, 0, &m_OnLineBuffID[onlineBuffIndex]);
		}

		char skillAdditionalBuff[COMMON_STRING_LENGTH] = { 0 };
		for (int categoryIndexLoopCount = 0; categoryIndexLoopCount < CATEGORY_COUNT; categoryIndexLoopCount++)
		{
			for (int categoryIdLoopCount = 0; categoryIdLoopCount < MAX_CATEGORY_ID; categoryIdLoopCount++)
			{
				for (int buffIndexLoopCount = 0; buffIndexLoopCount < MAX_SKILL_ADDITIONAL_BUFF; buffIndexLoopCount++)
				{
					sprintf(skillAdditionalBuff, SkillAdditionalBuffName,
						categoryIndexLoopCount,
						categoryIdLoopCount,
						buffIndexLoopCount);
					skillAdditionalBuff[COMMON_STRING_LENGTH - 1] = 0;
					configIniFile.GetInteger(SkillAdditionalBuffSectionName, skillAdditionalBuff, 0, &m_SkillAdditionalBuffID[categoryIndexLoopCount][categoryIdLoopCount][buffIndexLoopCount]);
				}
			}
		}

		for (int buffIndexLoopCount = 0; buffIndexLoopCount < MAX_SKILL_ADDITIONAL_BUFF; buffIndexLoopCount++)
		{
			sprintf(skillAdditionalBuff, CommonSkillAdditionalBuffName, buffIndexLoopCount);
			skillAdditionalBuff[COMMON_STRING_LENGTH - 1] = 0;
			configIniFile.GetInteger(SkillAdditionalBuffSectionName, skillAdditionalBuff, 0, &m_CommonSkillAdditionalBuffID[buffIndexLoopCount]);
		}

		LoadPKValueSettings();
#else

		char styleName[COMMON_STRING_LENGTH] = { 0 };
		for(int style = 0; style < style_count; style++)
		{			
			for(int additionalParam = 0; additionalParam < MAX_DISPLAY_STYLE_ADDITIONAL_PARAM; additionalParam++)
			{
				sprintf(styleName, ConfigurableDisplayStyleNames[style], additionalParam);
				styleName[COMMON_STRING_LENGTH - 1] = 0;
				styleconfigIniFile.GetString(ConfigurableDisplayStyleSectionName, styleName, "", m_DisplayStyle[style][additionalParam], COMMON_STRING_LENGTH);
			}
		}

#endif
		for ( int itemLevelupToolIdx = 0; itemLevelupToolIdx < ITEM_LEVELUP_TOOL_LIMIT_COUNT; ++itemLevelupToolIdx)
		{
			char szTXT[COMMON_CLIENT_MSG_LEN_32];
			sprintf( szTXT, "Type_%d", itemLevelupToolIdx );
			configIniFile.GetInteger2("ItemLevelupTool", szTXT, &m_itemLeftLevelupToolLimit[itemLevelupToolIdx], &m_itemRightLevelupToolLimit[itemLevelupToolIdx] );
		}

		m_IsLoaded = true;
	}
	else
	{
		success = false;
	}
		
	return success;
}

void ConfigManager::CleanUp()
{
	m_IsLoaded = false;
	memset(m_Color, 0 , sizeof(m_Color));
    memset(m_GlobalVariable, 0, sizeof(m_GlobalVariable));

#ifdef _SERVER

	m_OnlineBuffCount = 0;
	memset(m_OnLineBuffID, 0, sizeof(m_OnLineBuffID));
	memset(m_SkillAdditionalBuffID, 0, sizeof(m_SkillAdditionalBuffID));
	memset(m_CommonSkillAdditionalBuffID, 0, sizeof(m_CommonSkillAdditionalBuffID));
	memset(m_PKAddedValue, 0, sizeof(m_PKAddedValue));

#else

	memset(m_DisplayStyle, 0, sizeof(m_DisplayStyle));

#endif	
}

void ConfigManager::GetItemLevelupToolLimit( int Idx, int& leftLimit, int& rightLimit)
{
	if ( Idx >= 0 && Idx < ITEM_LEVELUP_TOOL_LIMIT_COUNT )
	{
		leftLimit = m_itemLeftLevelupToolLimit[Idx];
		rightLimit = m_itemRightLevelupToolLimit[Idx];
	}
	else
	{
		leftLimit = 0;
		rightLimit = 0;
	}
}

const char * ConfigManager::GetPlayerPrivateStateName(const int nIndex) const
{
	static   char szDefaultPrivateStateName[16] = "-_-";

	if (nIndex >= 1 && nIndex <= MAX_PRIVATE_STATE_NUM )
	{
		return m_PrivateStateName[nIndex - 1];
	}
	else
		return szDefaultPrivateStateName;
}

unsigned int ConfigManager::GetConfigurableColor(ConfigurableColor color)
{
	if (m_IsLoaded && color >= 0 && color < color_count)
		return m_Color[color];
	else
		return 0;
}

#ifdef _SERVER

int ConfigManager::GetOnlineBuffID(int index)
{
	if (m_IsLoaded && index >= 0 && index < m_OnlineBuffCount)
		return m_OnLineBuffID[index];
	else
		return 0;
}

#else

const char* ConfigManager::GetConfigurableDisplayStyle(ConfigurableDisplayStyle style, int additionParam /* = 0 */)
{
	if (m_IsLoaded && style >= 0 && style < style_count && additionParam >= 0 && additionParam < MAX_DISPLAY_STYLE_ADDITIONAL_PARAM)
		return m_DisplayStyle[style][additionParam];
	else
		return NULL;
}

#endif
