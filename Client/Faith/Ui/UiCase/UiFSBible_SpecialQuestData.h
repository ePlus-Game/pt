
#ifndef UI_FSBIBLE_SPECIAL_QUEST_DATA_H
#define UI_FSBIBLE_SPECIAL_QUEST_DATA_H

#include "GameDataDef.h"
#include "KTabFile.h"
#include <vector>


#define UI_FSBIBLE_SPECIAL_QUEST_DATA_FILE_PATH "\\settings\\SpecialQuestInfo.txt"
#define UI_FSBIBLE_SPECIAL_QUEST_COLOR_FILE_PATH "\\settings\\SpecialQuestColorRef.ini"

using namespace std;

class KUiFSBibleSpecialQuestData
{
	vector<SpecialQuestData> _data;
	vector<SpecialQuestData> _filterData;

private:
	void	loadCfg();
	void	LoadDateCfg(int line, KTabFile & cfgFile, SpecialQuestData & questData);
	int		filter();
	void	sortByColor();
public:
	KUiFSBibleSpecialQuestData();
	~KUiFSBibleSpecialQuestData();

	static KUiFSBibleSpecialQuestData& getSingleton();

	void	sendDataReq();
	void	updateData(int questId, ChangedSpecialQuestData& changedData);

	int		freshUi();
};

#endif