#ifndef _KLEVELUP_H_
#define _KLEVELUP_H_

#define MAX_ROLE 3
#define MAX_LEVEL_UP_TIP_LENGTH 200
#define MAX_LEVEL_UP_DESC_LENGTH 300

//Éý¼¶ÐÅÏ¢
class KLevelUpInfo
{
public:

	KLevelUpInfo();
	~KLevelUpInfo();

	static KLevelUpInfo& Singleton();

	bool Load();

	const LevelUpAdd* GetLevelUpAdd(int level, int roleSeries, int skillSeries);

private:

	bool m_isLoaded;

	LevelUpAdd m_LevelUpAddData[MAX_ROLE][3][MAX_LEVEL];

};

#endif //_KLEVELUP_H_
