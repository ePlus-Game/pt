
#include "UiFSBible.h"
#include "UiFSBible_SpecialQuestData.h"
#include "CoreShell.h"
#include <stdlib.h>
// sort的定义 
#include <algorithm> 


extern iCoreShell* g_pCoreShell;

KUiFSBibleSpecialQuestData::KUiFSBibleSpecialQuestData()
{
	loadCfg();
}

KUiFSBibleSpecialQuestData::~KUiFSBibleSpecialQuestData()
{

}

KUiFSBibleSpecialQuestData& KUiFSBibleSpecialQuestData::getSingleton()
{
	static KUiFSBibleSpecialQuestData singleton;
	return singleton;
}

void KUiFSBibleSpecialQuestData::LoadDateCfg(int line, KTabFile & cfgFile, SpecialQuestData & questData)
{
	char dateStr[10] = {0};
	cfgFile.GetString(line, 11, "", dateStr, sizeof(dateStr));

	size_t dateStrLen = strlen(dateStr);
	if (dateStrLen > 0)
	{
		char dayStr[2] = {0};
		int day = 1;
		for (size_t i = 0; i < dateStrLen; ++i)
		{
			dayStr[0] = dateStr[i];
			dayStr[1] = 0;
			day = atoi(dayStr);
			questData.canAcceptDate[day % 7] = true;
		}
	}
}

void KUiFSBibleSpecialQuestData::loadCfg()
{
	KTabFile file;
	KIniFile colorFile;

	if (!file.Load(UI_FSBIBLE_SPECIAL_QUEST_DATA_FILE_PATH))
	{
		return;
	}

	if (!colorFile.Load(UI_FSBIBLE_SPECIAL_QUEST_COLOR_FILE_PATH))
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
		SpecialQuestData data;
		file.GetInteger(k + 2, 1, 0, &data.questId);
		file.GetString(k + 2, 2, "", data.questType, sizeof(data.questType));
		file.GetString(k + 2, 3, "", data.questName, sizeof(data.questName));
		file.GetString(k + 2, 4, "", data.count, sizeof(data.count));
		file.GetString(k + 2, 5, "", data.time, sizeof(data.time));
		file.GetString(k + 2, 6, "", data.place, sizeof(data.place));
		file.GetString(k + 2, 7, "", data.npc, sizeof(data.npc));
		file.GetString(k + 2, 8, "", data.level, sizeof(data.level));
		file.GetString(k + 2, 9, "", data.tip, sizeof(data.tip));

		//由NPC模板id，从NPCS表中load出NPC的位置
		int npcId;
		file.GetInteger(k + 2, 10, 0, &npcId);
		LoadDateCfg(k + 2, file, data);
		data.npcId = npcId;
		if(npcId != -1)
		{
			g_pCoreShell->GetGameData(GDI_GET_NPC_POS_BY_TABLE_INDEX, (UINT)&data.npcPos, npcId);
		}

		char defaultSection[COMMON_CLIENT_MSG_LEN_64];
		memset(defaultSection, 0, sizeof(defaultSection));

		sprintf(defaultSection, "ffffffff");
		
		int defaultColor_r = 0;
		int defaultColor_g = 0;
		int defaultColor_b = 0;

		colorFile.GetInteger(defaultSection, "r", 255, &defaultColor_r);
		colorFile.GetInteger(defaultSection, "g", 255, &defaultColor_g);
		colorFile.GetInteger(defaultSection, "b", 255, &defaultColor_b);
		data.showColor = RGB(defaultColor_b, defaultColor_g, defaultColor_r);

		_data.push_back(data);
	}
}

void KUiFSBibleSpecialQuestData::sendDataReq()
{
	filter();

	for(int i = 0; i < _filterData.size(); ++i)
	{
		g_pCoreShell->OperationRequest(GOI_SPECIAL_QUEST_DATA_RQ, _filterData[i].questId, NULL);
	}
}

void KUiFSBibleSpecialQuestData::updateData(int questId, ChangedSpecialQuestData& changedData)
{
	KIniFile colorFile;
	if (!colorFile.Load(UI_FSBIBLE_SPECIAL_QUEST_COLOR_FILE_PATH))
	{
		return;
	}

	for(int i = 0; i < _data.size(); ++i)
	{
		if(questId == _data[i].questId)
		{
			_data[i].changedData = changedData;
			int color_r = 0;
			int color_g = 0;
			int color_b = 0;
			
			char order[COMMON_CLIENT_MSG_LEN_64];
			memset(order, 0, sizeof(order));
			itoa(_data[i].changedData.color, order, 16);
			colorFile.GetInteger(order, "r", 0, &color_r);
			colorFile.GetInteger(order, "g", 0, &color_g);
			colorFile.GetInteger(order, "b", 0, &color_b);
			_data[i].showColor = RGB(color_b, color_g, color_r);
			break;
		}
	}

	freshUi();
}

int KUiFSBibleSpecialQuestData::freshUi()
{
	int ret = filter();
	sortByColor();
	KUiFSBible::getSingleton().updateSpeicalQuestData(_filterData);

	return ret;
}

int KUiFSBibleSpecialQuestData::filter()
{
	KUiPlayerAttribute attr;
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&attr, NULL);

	_filterData.clear();
	for(int i = 0; i < _data.size(); ++i)
	{
		int highLevel = 0;
		int lowLevel = 0;
		char* levelTxt = _data[i].level;

		sscanf(levelTxt, "%d-%d", &lowLevel, &highLevel);
		if(attr.nLevel < lowLevel || attr.nLevel > highLevel)
		{
			continue;
		}

		const time_t curTime_T = time(NULL);
		tm * curTime = localtime(&curTime_T);
		if (curTime != NULL)
		{
			if (curTime->tm_wday >= 0 && curTime->tm_wday <= 6)
			{
				if (_data[i].canAcceptDate[curTime->tm_wday])
				{
					_filterData.push_back(_data[i]);
				}
			}
			else
			{
				_filterData.push_back(_data[i]);
			}
		}
	}

	return _filterData.size();
}

bool cmpSpecialQuestData_Color(const SpecialQuestData& data1, const SpecialQuestData& data2)
{
	if(data1.changedData.color > data2.changedData.color)
	{
		return true;
	}
	return false;
}

void KUiFSBibleSpecialQuestData::sortByColor()
{
	std::sort(_filterData.begin(), _filterData.end(), cmpSpecialQuestData_Color);
}
