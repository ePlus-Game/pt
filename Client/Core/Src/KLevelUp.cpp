#include "KCore.h"
#include "KLevelUp.h"

KLevelUpInfo::KLevelUpInfo()
{
	m_isLoaded = false;
}

KLevelUpInfo::~KLevelUpInfo()
{
	memset(m_LevelUpAddData, 0, sizeof(m_LevelUpAddData));
}

KLevelUpInfo& KLevelUpInfo::Singleton()
{
	static KLevelUpInfo info;
	return info;
}

bool KLevelUpInfo::Load()
{
	m_isLoaded = false;
	bool sucess = true;	
	
	memset(m_LevelUpAddData, 0, sizeof(m_LevelUpAddData));

	g_SetRootPath(".\\");
	char fileName[MAX_PATH];
	
	for(int j = 0; j < MAX_ROLE; j++)
	{
		for(int skillSeries = 0; skillSeries < 3; skillSeries++)
		{
			sprintf( fileName, PLAYER_LEVEL_UP_FILE, j, skillSeries );
			
			KTabFile levelUpAddTabFile;
			if (TRUE == levelUpAddTabFile.Load(fileName))
			{		
				int row = 0;
				int recordCount = levelUpAddTabFile.GetHeight() - 1;			
				if (recordCount >= MAX_LEVEL)
				{
					recordCount = MAX_LEVEL;
					
					for (int record = 0; record < recordCount; ++record)
					{
						row = record + 2;
						LevelUpAdd& add = m_LevelUpAddData[j][skillSeries][record];
						
						int field = 1;			
						
						if (FALSE == levelUpAddTabFile.GetInteger(row, field, 0, (int*)&(add.Exp)))
						{
							sucess = false;
						}
						++field;
						
						if (FALSE == levelUpAddTabFile.GetInteger(row, field, 0, (int*)&(add.SkillExp)))
						{
							sucess = false;
						}
						++field;
						
						for(int prop = 0; prop < attr_Count; prop++)
						{
							if (FALSE == levelUpAddTabFile.GetInteger(row, field, 0, &(add.Attribute[prop])))
							{
								sucess = false;
							}
							++field;
						}
						
						if (FALSE == levelUpAddTabFile.GetInteger(row, field, 0, &(add.Money)))
						{
							sucess = false;
						}
						++field;
						
						if (FALSE == levelUpAddTabFile.GetString(row, field, "", add.Tip, MAX_LEVEL_UP_TIP_LENGTH))
						{
							add.Tip[0] = '\0';
						}
						++field;
						
						if (FALSE == levelUpAddTabFile.GetString(row, field, "", add.Desc, MAX_LEVEL_UP_DESC_LENGTH))
						{
							add.Desc[0] = '\0';
						}
						++field;
						
						if (!sucess)
						{
							break;
						}
					}
				}
				else
				{
					sucess = false;
					break;
				}
			}
		}
	}

	if (sucess)
	{
		m_isLoaded = true;
	}
	
	return sucess;
}

const LevelUpAdd* KLevelUpInfo::GetLevelUpAdd(int level, int roleSeries, int skillSeries)
{
	if (roleSeries >= 0 && roleSeries < MAX_ROLE && level > 0 && level <= MAX_LEVEL && skillSeries < role_skillseries_count)
	{
		return &(m_LevelUpAddData[roleSeries][(skillSeries >= 0 ? skillSeries : 2)][level - 1]);
	}
	else
	{
		return NULL;
	}
}