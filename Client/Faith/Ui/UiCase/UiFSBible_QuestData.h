
#ifndef UI_FSBIBLE_QUEST_DATA_H
#define UI_FSBIBLE_QUEST_DATA_H

#include "GameDataDef.h"
#include <vector>

#define UI_FSBIBLE_QUEST_REWARD_TYPE_FILE_PATH "\\settings\\QuestRewardType.txt"
#define UI_FSBIBLE_QUEST_REWARD_FILE_PATH "\\settings\\QuestReward.txt"

using namespace std;

class KUiFSBibleQuestData
{
public:

private:
	vector<KRewardType>	_rewardTypes;
	vector<KReward>		_rewards;

	void	loadRewardType();
	void	loadRewards();
public:
	KUiFSBibleQuestData();
	~KUiFSBibleQuestData();

	static KUiFSBibleQuestData& getSingleton();

	KRewardType* getRewardTypeById(int rewardId);
	KReward* getQuestRewardById(int questId);
};

#endif