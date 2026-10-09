
#include "UiFSBible_QuestData.h"
#include "KTabFile.h"
#include "CoreShell.h"


extern iCoreShell* g_pCoreShell;

KUiFSBibleQuestData::KUiFSBibleQuestData()
{
	loadRewardType();
	loadRewards();
}

KUiFSBibleQuestData::~KUiFSBibleQuestData()
{

}

KUiFSBibleQuestData& KUiFSBibleQuestData::getSingleton()
{
	static KUiFSBibleQuestData singleton;
	return singleton;
}

void KUiFSBibleQuestData::loadRewardType()
{
	KTabFile file;

	if (!file.Load(UI_FSBIBLE_QUEST_REWARD_TYPE_FILE_PATH))
	{
		return;
	}

	int nHeight = file.GetHeight() - 1;
	int nWidth = file.GetWidth() - 1;
	if (nWidth == 0 || nHeight == 0)
	{
		return;
	}

	for(int k = 0; k < nHeight; k++)
	{
		KRewardType data;
		file.GetInteger(k + 2, 1, 0, &data.rewardId);
		file.GetString(k + 2, 2, "", data.imagePath, sizeof(data.imagePath));
		file.GetString(k + 2, 3, "", data.tip, sizeof(data.tip));
		_rewardTypes.push_back(data);
	}
	file.Clear();
}

void KUiFSBibleQuestData::loadRewards()
{
	KTabFile file;

	if (!file.Load(UI_FSBIBLE_QUEST_REWARD_FILE_PATH))
	{
		return;
	}

	int nHeight = file.GetHeight() - 1;
	int nWidth = file.GetWidth() - 1;
	if (nWidth == 0 || nHeight == 0)
	{
		return;
	}

	char itemTypeString[COMMON_CLIENT_MSG_LEN_32];
	for(int i = 0; i < nHeight; i++)
	{
		KReward data;
		file.GetInteger(i + 2, 1, 0, &data.questId);


		for(int j = 0; j < MAX_REWARD_ITEM; ++j)
		{
			file.GetString(i + 2, 2 + j, "", itemTypeString, sizeof(itemTypeString));
			int ret = sscanf(itemTypeString, "%d,%d,%d,%d,%d", 
				&data.item[j].genre, &data.item[j].detail, &data.item[j].particular, &data.item[j].level,
				&data.itemCount[j]);
		}
		
		for(int k = 0; k < MAX_OTHER_REWARD; ++k)
		{
			file.GetInteger(i + 2, 2 + MAX_REWARD_ITEM + k, 0, &data.otherRewardIds[k]);
		}

		_rewards.push_back(data);
	}
	file.Clear();
}

KRewardType* KUiFSBibleQuestData::getRewardTypeById(int rewardId)
{
	for(int i = 0; i < _rewardTypes.size(); ++i)
	{
		if(rewardId == _rewardTypes[i].rewardId)
		{
			return &_rewardTypes[i];
		}
	}

	return NULL;
}

KReward* KUiFSBibleQuestData::getQuestRewardById(int questId)
{
	for(int i = 0; i < _rewards.size(); ++i)
	{
		if(questId == _rewards[i].questId)
		{
			return &_rewards[i];
		}
	}

	return NULL;
}