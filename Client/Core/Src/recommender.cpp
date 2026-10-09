//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008-04-18
//      File_base        : recommender
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 推荐人系统
//
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "KPlayer.h"
#include "ConfigManager.h"
#include "KItemGenerator.h"
#include "IBLog.h"
#include "CoreRelated.h"
#include "recommender.h"

#define RECOMMENDER_REWARD_CONFIG_FILE "/settings/recommender_reward.txt"

RecommenderSystem::RecommenderSystem()
{
}

RecommenderSystem::~RecommenderSystem()
{
}

RecommenderSystem& RecommenderSystem::Singleton()
{
	static RecommenderSystem recommenderSystem;
	return recommenderSystem;
}

bool RecommenderSystem::Load()
{
	KTabFile recommenderRewardTabFile;
	if (TRUE == recommenderRewardTabFile.Load(RECOMMENDER_REWARD_CONFIG_FILE))
	{		
		int row = 0;
		int recordCount = recommenderRewardTabFile.GetHeight() - 1;
		if (recordCount < 0)
			recordCount = 0;
		if (recordCount > MAX_LEVEL)
			recordCount = MAX_LEVEL;
		
		for (int record = 0; record < recordCount; ++record)
		{
			int result = 0;

			row = record + 2;
			int rewardMoney = 0;
			int studentLevel = 0;

			recommenderRewardTabFile.GetInteger(row, 1, 0, &studentLevel);
			recommenderRewardTabFile.GetInteger(row, 2, 0, &rewardMoney);

			if (rewardMoney < 0)
				rewardMoney = 0;
			if (studentLevel < 0)
				studentLevel = 0;
			
			_DBProcHeader DBHeader = {0};
			DBHeader.ulNetID = -1;
			DBHeader.ProcType = Proc_SetMasterRewardCfg;
			
			IProcParam* pParam = g_pController->GetProcParam( );
			
			pParam->BeginPush( PN_SET_MASTERREWARD_CFG );
			pParam->Push( studentLevel );
			pParam->Push( rewardMoney );
			pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
			
			if (g_pController)
				result = g_pController->CallProc( cfs_db_cnn_role, pParam );

			if (0 == result)
				return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

int RecommenderSystem::CheckMasterRequirements(int masterPlayerIndex)
{
	if (!IsValidPlayer(masterPlayerIndex))
		return 0;

	int masterRequireLevelMin = ConfigManager::Singleton().GetGlobalVariable(global_var_recommender_master_require_level_min);
	int masterRequireLevelMax = ConfigManager::Singleton().GetGlobalVariable(global_var_recommender_master_require_level_max);
	if (masterRequireLevelMin <= 0)
		masterRequireLevelMin = 1;
	if (masterRequireLevelMax <= 0)
		masterRequireLevelMax = MAX_LEVEL;

	int masterLevel = Player[masterPlayerIndex].GetLevel();
	if (masterLevel >= masterRequireLevelMin && masterLevel <= masterRequireLevelMax)
		return 1;
	else
		return 0;
}

int RecommenderSystem::CheckStudentRequirements(int studentPlayerIndex)
{
	if (!IsValidPlayer(studentPlayerIndex))
		return 0;

	int studentRequireLevelMin = ConfigManager::Singleton().GetGlobalVariable(global_var_recommender_student_require_level_min);
	int studentRequireLevelMax = ConfigManager::Singleton().GetGlobalVariable(global_var_recommender_student_require_level_max);
	if (studentRequireLevelMin <= 0)
		studentRequireLevelMin = 1;
	if (studentRequireLevelMax <= 0)
		studentRequireLevelMax = MAX_LEVEL;

	int studentLevel = Player[studentPlayerIndex].GetLevel();
	if (studentLevel >= studentRequireLevelMin && studentLevel <= studentRequireLevelMax)
		return 1;
	else
		return 0;
}

#define MAX_REWARD_COUNT 10000

int RecommenderSystem::Reward(int playerIndex, int rewardCount)
{
	if (!IsValidPlayer(playerIndex))
		return FALSE;

	if (rewardCount <= 0)
		return FALSE;

	if (rewardCount > MAX_REWARD_COUNT)
		rewardCount = MAX_REWARD_COUNT;

	KPlayer& player = Player[playerIndex];

	int itemGenre = 0;
	int itemDetail = 0;
	int itemParticular = 0;
	int itemLevel = 0;
	
	ConfigManager& cm = ConfigManager::Singleton();
	cm.GetIBTicketId( &itemGenre, &itemDetail, &itemParticular, &itemLevel );

	int actualRewardCount = 0;

	GlobalAddItemToPlayer(
		playerIndex,
		itemGenre,
		itemDetail, 
		itemParticular,
		itemLevel,
		rewardCount,
		actualRewardCount,
		item_count_type_recommender_reward_ticket,
		log_event_recommend_master_reward_add_item
		);

	if (actualRewardCount > 0)
	{
		KIBLog::getSingleton().AddIBMoney( recommender_reward_card, playerIndex, actualRewardCount );
	}

	if (g_pLogSystem && TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_recommender))
	{
		LogEventParam logParam;
		logParam.event = log_event_recommend_master_reward_add_ticket;
		logParam.param1 = player.GetGUID();
		snprintf(logParam.param3.data, sizeof(logParam.param3.data), "%d", rewardCount);
		logParam.param3.data[sizeof(logParam.param3.data) - 1] = 0;
		logParam.param4 = actualRewardCount;
		g_pLogSystem->Log(logParam);
	}

	return ((rewardCount == actualRewardCount) ? TRUE : FALSE);
}
